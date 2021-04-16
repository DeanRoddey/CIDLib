//
// FILE NAME: CIDKernel_FileSystem_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/19/1996
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file provides the Win32 specific implementation of the the
//  TKrnlFileSys namespace.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"

#pragma     warning(push)
#include    <CodeAnalysis\Warnings.h>
#pragma     warning(disable : ALL_CODE_ANALYSIS_WARNINGS 26812)
#include    <devguid.h>
#include    <shlwapi.h>
#include    <shlobj.h>
#include    <Dbt.h>
#include    <process.h>
#include    <Aclapi.h>
#include    <Winnetwk.h>
#pragma     warning(pop)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDKernel_FileSystem
{
    namespace
    {
        //
        //  We need a critical section to protect our list of volumes, which
        //  are only updated when changes have occured in the volume list. The
        //  current thread calling into bQueryVolumeList() will update the
        //  list if the flag is set by the background thread to indicate that
        //  changes have been seen.
        //
        TKrnlCritSec*   pkcrsLock;

        //
        //  Our list of volumes and failures. And a list of volume statuses to
        //  indicate which volumes should be found in which lists.
        //
        enum EVolStatuses
        {
            EVolStat_NotUsed
            , EVolStat_Used
            , EVolStat_Error
        };

        constexpr tCIDLib::TCard4   c4MaxVolumes = 64;
        EVolStatuses                aeVolStatuses[c4MaxVolumes];
        TKrnlVolumeInfo             akvoliCur[c4MaxVolumes];
        TKrnlVolFailureInfo         akvolfiCur[c4MaxVolumes];

        // The name of the local window class we register
        const tCIDLib::TCh* const   pszVolNotClass = L"CIDKrnlVolNot";

        // The thread handle and id of the volume notification thread
        HANDLE              hthrVolNot;
        tCIDLib::TThreadId  tidVolNot;

        // Our notification window handle
        HWND                hwndVolNot;

        //
        //  We have to do an initial full scan, so whichever thread get around
        //  to it first needs to do this. Usually it'll be our background thread
        //  but not always, so we need to know if it's already been done.
        //
        tCIDLib::TBoolean   bFirstScan = kCIDLib::False;

        //
        //  A change counter that gets bumped each time we get a notification
        //  that volumes have changed. The next time anyone asks for the info
        //  that will force an update of the lists above, and this will be
        //  zeroed out.
        //
        tCIDLib::TCard4     c4VolChangeCnt = 1;

        //
        //  We also need an ongoing serial number that we just keep incrementing
        //  each time we change the volume list. This is required for some special
        //  case scenarios where client code may need to take actions that it
        //  only wants to do if it knows the list has changed. So they can store
        //  this value and watch for a new value to show up.
        //
        tCIDLib::TCard4     c4VolListSerialNum = 1;

        //
        //  For general debugging purposes we keep a counter of how many times we have to do
        //  retries of file system operations. We make this available to the application.
        //  No sync, it's just a basic type and even if we missed a bump once in a while it
        //  wouldn't matter.
        //
        tCIDLib::TCard4     c4FSRetryCount = 0;
    }
}


// ---------------------------------------------------------------------------
//   CLASS: TFindJanitor
//  PREFIX: jan
//
//  Just a simple inlined janitor class that lets us insure we get directory
//  search handles closed.
// ---------------------------------------------------------------------------
class TFindJanitor
{
    public :
        TFindJanitor(const HANDLE hFind) :
            m_hFind(hFind)
        {
        }

        ~TFindJanitor()
        {
            ::FindClose(m_hFind);
        }

        HANDLE m_hFind;
};


// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  This method will check a Win32 file find buffer to see if the thing
//  found meets the search criteria in the passed search flags.
//
static tCIDLib::TBoolean
bCheckSearchCriteria(   const   WIN32_FIND_DATA&            toCheck
                        , const tCIDLib::EDirSearchFlags    eFlags)
{
    // Assume no match
    tCIDLib::TBoolean bMatches = kCIDLib::False;

    if (toCheck.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        //
        //  We don't return the current/parent directory entries, so filter those out
        //  always.
        //
        if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::EDirSearchFlags::FindDirs)
        &&  !TRawStr::bCompareStrI(toCheck.cFileName, L".")
        &&  !TRawStr::bCompareStrI(toCheck.cFileName, L".."))
        {
            //
            //  It was a directory and they want see directories. So now check
            //  to see if they want just normal directories. If so, then
            //  filter out the non-normal directories.
            //
            if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::EDirSearchFlags::NormalDirsOnly))
            {
                if (!(toCheck.dwFileAttributes & TKrnlWin32::c4NonNormalAttrs))
                {
                    bMatches = kCIDLib::True;
                }
            }
             else
            {
                bMatches = kCIDLib::True;
            }
        }
    }
     else
    {
        if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::EDirSearchFlags::FindFiles))
        {
            //
            //  Else it was a file and they wanted to find files. See if they
            //  want only normal files and filter out non-normal ones if so.
            //
            if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::EDirSearchFlags::NormalFilesOnly))
            {
                //
                //  They only want normal files, so make sure that none of
                //  the non-normal attributes are on.
                //
                if (!(toCheck.dwFileAttributes & TKrnlWin32::c4NonNormalAttrs))
                    bMatches = kCIDLib::True;
            }
             else
            {
                bMatches = kCIDLib::True;
            }
        }
    }
    return bMatches;
}


static tCIDLib::TBoolean
bCheckExists(const  tCIDLib::TCh* const         pszToFind
            ,       tCIDLib::TBoolean&          bResult
            ,       tCIDLib::TBoolean&          bIsUnique
            , const tCIDLib::EDirSearchFlags    eFlags)

{
    // Init the unique and result flags to defaults
    const tCIDLib::TBoolean bCheckUnique = bIsUnique;
    bIsUnique = kCIDLib::True;
    bResult = kCIDLib::False;

    //
    //  Watch for the special case of X: which is not really legal but find
    //  first will work if you pass that. We always say this is not found.
    //
    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszToFind);
    if ((c4Len == 2) && (pszToFind[1] == L':'))
        return kCIDLib::False;

    //
    //  We may need to modify the path, so create a buffer and a pointer. We
    //  point the pointer at the incoming and if we don't modify the path,
    //  then we are good and avoid the copy.
    //
    const tCIDLib::TCh* pszActual = pszToFind;
    tCIDLib::TCh achModBuf[MAX_PATH + 1];

    //
    //  We have to deal with the special case of the root directory of a
    //  drive or of a UNC share. So see if they are checking for directories,
    //  and if so, we'll look for the special cases.
    //
    //  What we'll do is turn them into searches for anything under the
    //  root. We'll set a flag that indicates we'd done this and below if
    //  we get even one match, we'll know we are good.
    //
    tCIDLib::TBoolean bForced = kCIDLib::False;
    if (tCIDLib::bAnyBitsOn(eFlags, tCIDLib::EDirSearchFlags::FindDirs))
    {
        //
        //  Has to be equal to 3 for the root directory of a drive scenario.
        //  Has to be at least 6 for the UNC share.
        //
        if (c4Len == 3)
        {
            if ((pszToFind[1] == kCIDLib::chColon)
            &&  (pszToFind[2] == L'\\'))
            {
                // Just do a search for something in the directory
                TRawStr::CopyStr(achModBuf, pszToFind, MAX_PATH);
                TRawStr::CatStr(achModBuf, L"*");
                pszActual = achModBuf;
                bForced = kCIDLib::True;
            }
        }
         else if (c4Len > 5)
        {
            if (TRawStr::bCompareStrN(pszToFind, L"\\\\", 2))
            {
                //
                //  Count the slashes we see after the leading two. If it
                //  is 1 or 2 and the second is the last character, then it's
                //  our guy so just do a search for something inside it
                //
                tCIDLib::TCard4 c4Count = 0;
                const tCIDLib::TCh* pszPtr = pszToFind + 2;
                while (*pszPtr)
                {
                    if (*pszPtr == L'\\')
                        c4Count++;
                    pszPtr++;
                }

                if (c4Count == 1)
                {
                    TRawStr::CopyStr(achModBuf, pszToFind, MAX_PATH);
                    TRawStr::CatStr(achModBuf, L"\\*");
                    pszActual = achModBuf;
                    bForced = kCIDLib::True;
                }
                 else if ((c4Count == 2) && (pszToFind[c4Len - 1] == L'\\'))
                {
                    TRawStr::CopyStr(achModBuf, pszToFind, MAX_PATH);
                    TRawStr::CatStr(achModBuf, L"*");
                    pszActual = achModBuf;
                    bForced = kCIDLib::True;
                }
            }
        }
    }

    WIN32_FIND_DATA FileData;
    HANDLE hFind = ::FindFirstFile(pszActual, &FileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        //
        //  If it was for any other reason that the file not being found,
        //  then its an error. Otherwise just return not found but no error.
        //
        const tCIDLib::TOSErrCode errcReason = ::GetLastError();
        if ((errcReason != ERROR_FILE_NOT_FOUND)
        &&  (errcReason != ERROR_PATH_NOT_FOUND))
        {
            TKrnlError::SetLastHostError(errcReason);
            return kCIDLib::False;
        }
        bResult = kCIDLib::False;
        return kCIDLib::True;
    }

    // We create the handle, so insure it gets cleaned up
    TFindJanitor janFind(hFind);

    // If we forced above, then any find is a success
    if (bForced)
    {
        bResult = kCIDLib::True;
        return kCIDLib::True;
    }

    //
    //  Else we continue to check until we find one that meets the search
    //  criteria provided.
    //
    bResult = kCIDLib::False;
    while (!bResult)
    {
        //
        //  Now we need to see if this one matches the search criteria that
        //  were passed in.
        //
        bResult = bCheckSearchCriteria(FileData, eFlags);

        // If not, then keep going
        if (!bResult)
        {
            if (!::FindNextFile(hFind, &FileData))
            {
                const tCIDLib::TOSErrCode errcReason = ::GetLastError();
                if (errcReason == ERROR_NO_MORE_FILES)
                    break;
                TKrnlError::SetLastHostError(errcReason);
                return kCIDLib::False;
            }
        }
    }

    // If they want to check for uniqueness, then keep going
    if (bCheckUnique)
    {
        while (::FindNextFile(hFind, &FileData))
        {
            if (bCheckSearchCriteria(FileData, eFlags))
            {
                bIsUnique = kCIDLib::False;
                break;
            }
        }
    }
    return kCIDLib::True;
}


//
//  Converts a system file information buffer to our raw file find
//  structure type.
//
static tCIDLib::TBoolean
bConvertFindBuf(const   WIN32_FIND_DATA&            FileData
                ,       TKrnlFileSys::TRawFileFind& fndbToFill)
{
    //
    //  Copy over all of the time members. We store these as the UCT
    //  value in a encoded time value, and we have to adjust them for our
    //  base time since these are relative to the Windows base time.
    //  Normally the time class would handle this but in this case we
    //  are getting these times ourselves.
    //
    FILETIME LocalTime;
    if (!::FileTimeToLocalFileTime(&FileData.ftLastAccessTime, &LocalTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    fndbToFill.enctLastAccessTime = TRawBits::c8From32
    (
        LocalTime.dwLowDateTime, LocalTime.dwHighDateTime
    );

    if (!::FileTimeToLocalFileTime(&FileData.ftLastWriteTime, &LocalTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    fndbToFill.enctLastWriteTime = TRawBits::c8From32
    (
        LocalTime.dwLowDateTime, LocalTime.dwHighDateTime
    );

    if (!::FileTimeToLocalFileTime(&FileData.ftCreationTime, &LocalTime))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    fndbToFill.enctCreationTime = TRawBits::c8From32
    (
        LocalTime.dwLowDateTime, LocalTime.dwHighDateTime
    );

    fndbToFill.enctCreationTime -= TKrnlTimeStamp::enctBaseOfs;
    fndbToFill.enctLastAccessTime -= TKrnlTimeStamp::enctBaseOfs;
    fndbToFill.enctLastWriteTime -= TKrnlTimeStamp::enctBaseOfs;

    // Store the file size, just the 32 bit file size for now
    fndbToFill.c8FileSize = FileData.nFileSizeLow;

    // Convert the file attributes over
    fndbToFill.eInfoFlags = TKrnlWin32::eConvertAttrs
    (
        FileData.dwFileAttributes, FileData.cFileName
    );

    // Copy the file name to our buffer
    TRawStr::CopyStr
    (
        fndbToFill.szName
        , FileData.cFileName
        , c4MaxBufChars(fndbToFill.szName)
    );
    return kCIDLib::True;
}


//
//  A helper to iterate the network and load up any network shares into the
//  passed list of volumes. This is called when they ask to iterate volumes
//  and want to include network shares.
//
static tCIDLib::TVoid
LoadNetShares(  NETRESOURCE*                    pRes
                , TKrnlLList<TKrnlVolumeInfo>&  kllistGood
                , tCIDLib::TCard4&              c4VolCount)
{
    // Open up the passed resource (null on the top level round)
    HANDLE hCont;
    DWORD ErrCode = ::WNetOpenEnum
    (
        RESOURCE_CONNECTED, RESOURCETYPE_ANY, 0, pRes, &hCont
    );

    // If we can't even open it, then return with no action
    if (ErrCode)
    {
        if (ErrCode == ERROR_EXTENDED_ERROR)
            ErrCode = ::GetLastError();
        return;
    }

    constexpr tCIDLib::TCard4 c4BufSz(16 * 1024);
    void* pBuf = ::GlobalAlloc(GPTR, c4BufSz);
    NETRESOURCE* pInfo = reinterpret_cast<NETRESOURCE*>(pBuf);

    DWORD Count = kCIDLib::c4MaxCard;
    DWORD Size = c4BufSz;
    TKrnlVolumeInfo kvoliCur;
    while (kCIDLib::True)
    {
        TRawMem::SetMemBuf(pBuf, kCIDLib::c1MinCard, c4BufSz);
        Size = c4BufSz;
        Count = kCIDLib::c4MaxCard;
        ErrCode = ::WNetEnumResource(hCont, &Count, pBuf, &Size);

        // If we are done, or not successful, break out
        if (ErrCode == ERROR_NO_MORE_ITEMS)
            break;

        if (ErrCode != NO_ERROR)
            break;

        for (tCIDLib::TCard4 c4Index = 0; c4Index < Count; c4Index++)
        {
            //
            //  Recurse on any containers. Else check it for a share type
            //  and add it to the list if so.
            //
            if ((pInfo[c4Index].dwUsage & RESOURCEUSAGE_CONTAINER) == RESOURCEUSAGE_CONTAINER)
            {
                LoadNetShares(&pInfo[c4Index], kllistGood, c4VolCount);
            }
             else if (pInfo[c4Index].dwDisplayType == RESOURCEDISPLAYTYPE_SHARE)
            {
                //
                //  If it has a local name, then skip it. It'll show up as a regular
                //  drive.
                //
                if ((pInfo[c4Index].lpLocalName == 0)
                &&  (pInfo[c4Index].lpRemoteName != 0)
                &&  TKrnlFileSys::bQueryVolumeInfo(pInfo[c4Index].lpRemoteName, kvoliCur))
                {
                    kllistGood.pobjAddNew(new TKrnlVolumeInfo(kvoliCur));
                }
            }
        }
    }

    // Free the buffer
    ::GlobalFree((HGLOBAL)pBuf);

    // Release the network enum handle
    ::WNetCloseEnum(hCont);
}


//
//  This function is called when we get a device change notification. It tells us
//  to update our volume list. We get a mask of the drive volumes we need to remove
//  or udpate our info for.
//
static tCIDLib::TVoid UpdateVolList(const   tCIDLib::TBoolean   bAdd
                                    , const tCIDLib::TCard4     c4Drives)
{
    //
    //  Set up the volume path template that we can plop a drive letter into
    //  for each driver.
    //
    tCIDLib::TCh szVolumePath[4] = L" :\\";

    // Loop through the mask and process each one that is on in the mask
    tCIDLib::TBoolean bRes;
    tCIDLib::TCard4 c4Mask = 0x1;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 26; c4Index++)
    {
        if (c4Drives & c4Mask)
        {
            const tCIDLib::TCh chLetter = tCIDLib::TCh
            (
                kCIDLib::chLatin_A + c4Index
            );
            szVolumePath[0] = chLetter;

            if (bAdd)
            {
                //
                //  If it's A or B, don't try to get volume info since it will likely
                //  cause a long delay. Just fake some info in for those and go back
                //  to the top. we just put some bogus info in it that will let us
                //  know later to query the actual values if it's accessed.
                //
                if ((chLetter == kCIDLib::chLatin_A)
                ||  (chLetter == kCIDLib::chLatin_B))
                {
                    CIDKernel_FileSystem::aeVolStatuses[c4Index]
                                    = CIDKernel_FileSystem::EVolStat_Used;
                    CIDKernel_FileSystem::akvoliCur[c4Index].Set
                    (
                        kCIDLib::c4MaxPathLen
                        , tCIDLib::EVolumeFlags::None
                        , tCIDLib::EVolHWTypes::Removable
                        , kCIDLib::pszEmptyZStr
                        , szVolumePath
                        , kCIDLib::pszEmptyZStr
                        , kCIDLib::pszEmptyZStr
                    );
                }
                 else
                {
                    // Attempt to query volume info on this guy
                    bRes = TKrnlFileSys::bQueryVolumeInfo
                    (
                        szVolumePath
                        , CIDKernel_FileSystem::akvoliCur[c4Index]
                    );

                    if (bRes)
                    {
                        // Mark this one as used
                        CIDKernel_FileSystem::aeVolStatuses[c4Index]
                                    = CIDKernel_FileSystem::EVolStat_Used;
                    }
                     else
                    {
                        // Mark this one as in error
                        CIDKernel_FileSystem::aeVolStatuses[c4Index]
                                    = CIDKernel_FileSystem::EVolStat_Error;

                        // And store the error info
                        CIDKernel_FileSystem::akvolfiCur[c4Index].Set
                        (
                            szVolumePath
                            , tCIDLib::EVolHWTypes::Unknown
                            , TKrnlError::kerrLast()
                        );
                    }
                }
            }
             else
            {
                // Mark this one as no longer in use
                CIDKernel_FileSystem::aeVolStatuses[c4Index]
                                = CIDKernel_FileSystem::EVolStat_NotUsed;

                // Clear the label, type, and a couple other things
                CIDKernel_FileSystem::akvoliCur[c4Index].ClearUsedInfo();
            }
        }
        c4Mask <<= 1;
    }
}


//
//  If we get a notification that a more major change has occurred, we will
//  just reset the whole list and reload it from scratch.
//
static tCIDLib::TVoid ResetVolList()
{
    // Go through and reset them all
    tCIDLib::TCh szVolumePath[4] = L" :\\";
    TKrnlError kerrDummy;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 26; c4Index++)
    {
        szVolumePath[0] = tCIDLib::TCh(L'A' + c4Index);
        CIDKernel_FileSystem::aeVolStatuses[c4Index]
                                = CIDKernel_FileSystem::EVolStat_Error;
        CIDKernel_FileSystem::akvoliCur[c4Index].ClearUsedInfo();
        CIDKernel_FileSystem::akvolfiCur[c4Index].Set
        (
            szVolumePath, tCIDLib::EVolHWTypes::Unknown, kerrDummy
        );
    }

    // And now do a full reload
    UpdateVolList(kCIDLib::True, ::GetLogicalDrives());
}


//
//  A window procedure for a local message-only window that we register to
//  get device change notifications. We bump a change counter to let any
//  later incoming volume list queries know that they need to update the
//  list.
//
//  Note that we lock while doing even this simple bump. The reason is that
//  when GUI threads get a notification and force an immediate update we
//  could get a notification as well. It's unlikely but possible that ours
//  is not for the same one that they are currently processing. This way,
//  if they are currently processing and we get another, we'll be blocked
//  until they complete and then we do our bump of the change counter.
//
//  Otherwise, they would clear our bump if we did it while they were in
//  the middle of an update. Unfortunately, this almost always will actually
//  do just the opposite, which is to cause us to bump it unnecessarily
//  and so the next query of the volume list requires another list update.
//  But there's seemingly no way to avoid it without theoretically possible
//  dropping of a change, so it's best to be safe.
//
static LRESULT WNDPROCAPI
mresVolNotify(  HWND        hwndThis
                , UINT      wmsgCur
                , WPARAM    uParam
                , LPARAM    i4Param)
{
    if (wmsgCur == WM_CREATE)
    {
        // Register for device changes
        DEV_BROADCAST_DEVICEINTERFACE NotificationFilter = {0};
        NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
        NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        HDEVNOTIFY hDevNotify = RegisterDeviceNotification
        (
            hwndThis
            , &NotificationFilter
            , DEVICE_NOTIFY_WINDOW_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES
        );
        return 0;
    }
     else if (wmsgCur == WM_DEVICECHANGE)
    {
        // We only care about device arrival and the final removal of media
        if ((uParam == DBT_DEVICEARRIVAL)
        ||  (uParam == DBT_DEVICEREMOVECOMPLETE))
        {
            // We know we can look at these as broadcast header structs
            DEV_BROADCAST_HDR* pHdr = (DEV_BROADCAST_HDR*)i4Param;

            //
            //  We only care about volume events. If we get one, we update
            //  our list of volumes. In some cases it's the actual arrival
            //  of a volume, in others it's a change in media of removeable
            //  media volume. In the later case we still need to update in
            //  order to update our volume name info, which is driven by
            //  the disk in the drive (or lack thereof.)
            //
            if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
            {
                TKrnlCritSecLocker kcrslSync(CIDKernel_FileSystem::pkcrsLock);
                CIDKernel_FileSystem::c4VolChangeCnt++;
            }
        }
         else if (uParam == DBT_DEVNODES_CHANGED)
        {
            TKrnlCritSecLocker kcrslSync(CIDKernel_FileSystem::pkcrsLock);
            CIDKernel_FileSystem::c4VolChangeCnt++;
        }
        return 1;
    }
     else if (wmsgCur == WM_DESTROY)
    {
        ::PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwndThis, wmsgCur, uParam, i4Param);
}


static tCIDLib::TCard4 WINAPI c4VolNotThread(tCIDLib::TVoid* pData)
{
    HINSTANCE hInst = *(HINSTANCE*)pData;

    // Register our local window class for our message only window
    WNDCLASSEX  wcData = {0};
    wcData.cbSize           = sizeof(WNDCLASSEX);
    wcData.style            = 0;
    wcData.lpfnWndProc      = mresVolNotify;
    wcData.cbWndExtra       = 0;
    wcData.hInstance        = hInst;
    wcData.hCursor          = 0;
    wcData.lpszClassName    = CIDKernel_FileSystem::pszVolNotClass;
    ::RegisterClassEx(&wcData);

    //
    //  Create a window instance. This has to be done here within this
    //  thread so that we own it.
    //
    CIDKernel_FileSystem::hwndVolNot = ::CreateWindowEx
    (
        0
        , CIDKernel_FileSystem::pszVolNotClass
        , L"CIDVolNotify"
        , 0
        , 1
        , 1
        , 1
        , 1
        , HWND_MESSAGE
        , 0
        , hInst
        , 0
    );

    //
    //  Start processing messages. Note that we will never really exit
    //  this loop. We'll just get killed when the system kills this
    //  thread.
    //
    MSG msgCur;
    while (::GetMessage(&msgCur, 0, 0, 0) != 0)
    {
        ::TranslateMessage(&msgCur);
        ::DispatchMessage(&msgCur);
    }

    // Deregister our notification window handle
    CIDKernel_FileSystem::hwndVolNot = 0;
    return 0;
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
tCIDCtrls::TWndHandle CIDKrnlFS_NotHandle()
{
    return CIDKernel_FileSystem::hwndVolNot;
}


// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::bInitTermFileSys(const tCIDLib::EInitTerm eState)
{
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
        // Allocate the critical section used to protect our volume list
        CIDKernel_FileSystem::pkcrsLock = new TKrnlCritSec;

        //
        //  We have to create a local thread that will create the notification
        //  window, and process it's message queue.
        //
        CIDKernel_FileSystem::hthrVolNot = ::CreateThread
        (
            0
            , 0
            , c4VolNotThread
            , (tCIDLib::TVoid*)&hmodThis().hmodiThis()
            , 0
            , &CIDKernel_FileSystem::tidVolNot
        );

        if (!CIDKernel_FileSystem::hthrVolNot)
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }
    }
     else
    {
        // Get the volume notify thread to exit
        if (CIDKernel_FileSystem::hwndVolNot)
            ::DestroyWindow(CIDKernel_FileSystem::hwndVolNot);
    }
    return kCIDLib::True;
}




// ---------------------------------------------------------------------------
//   CLASS: TDirSearchHandle
//  PREFIX: hdir
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDirSearchHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TDirSearchHandle::TDirSearchHandle() :

    m_phdiriThis(0)
{
    m_phdiriThis = new TDirSearchHandleImpl;
    m_phdiriThis->hDir = 0;
}

TDirSearchHandle::TDirSearchHandle(const TDirSearchHandle& hdirToCopy) :

    m_phdiriThis(0)
{
    m_phdiriThis = new TDirSearchHandleImpl;
    m_phdiriThis->hDir = hdirToCopy.m_phdiriThis->hDir;
}

TDirSearchHandle::~TDirSearchHandle()
{
    delete m_phdiriThis;
    m_phdiriThis = 0;
}


// ---------------------------------------------------------------------------
//  TDirSearchHandle: Public operators
// ---------------------------------------------------------------------------
TDirSearchHandle&
TDirSearchHandle::operator=(const TDirSearchHandle& hdirToAssign)
{
    if (this == &hdirToAssign)
        return *this;

    m_phdiriThis->hDir = hdirToAssign.m_phdiriThis->hDir;

    return *this;
}


tCIDLib::TBoolean
TDirSearchHandle::operator==(const TDirSearchHandle& hdirToCompare) const
{
    return (m_phdiriThis->hDir == hdirToCompare.m_phdiriThis->hDir);
}

tCIDLib::TBoolean
TDirSearchHandle::operator!=(const TDirSearchHandle& hdirToCompare) const
{
    return !operator==(hdirToCompare);
}


// ---------------------------------------------------------------------------
//  TDirSearchHandle: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TDirSearchHandle::bIsValid() const
{
    return (m_phdiriThis->hDir != 0);
}

tCIDLib::TVoid TDirSearchHandle::Clear()
{
    m_phdiriThis->hDir = 0;
}

tCIDLib::TVoid
TDirSearchHandle::FormatToStr(          tCIDLib::TCh* const pszToFill
                                , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_phdiriThis->hDir)
        , pszToFill
        , c4MaxChars
        , tCIDLib::ERadices::Hex
    );
}

const TDirSearchHandleImpl& TDirSearchHandle::hdiriThis() const
{
    return *m_phdiriThis;
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlDirSearch
//  PREFIX: kds
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlDirSearch: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlDirSearch::TKrnlDirSearch() :

    m_eSearchFlags(tCIDLib::EDirSearchFlags::AllNormal)
{
}

TKrnlDirSearch::~TKrnlDirSearch()
{
    if (!bClose())
    {
        //
        //  If it fails and we are debugging, then do a popup. Otherwise
        //  there is not much we can do since we cannot throw out of the
        //  destructor.
        //
        #if CID_DEBUG_ON
        kmodCIDKernel.KrnlErrorPopUp
        (
            TKrnlError::kerrLast()
            , CID_FILE
            , CID_LINE
            , L"Failed to close the directory search handle"
        );
        #endif
    }
}


// ---------------------------------------------------------------------------
//  TKrnlDirSearch: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlDirSearch::bClose()
{
    // If the search handle is non-zero, then it needs to be closed
    if (m_hdirSearch.m_phdiriThis->hDir)
    {
        if (!::FindClose(m_hdirSearch.m_phdiriThis->hDir))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }
        m_hdirSearch.m_phdiriThis->hDir = 0;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlDirSearch::bFindFirst( const   tCIDLib::TCh* const         pszToSearch
                            ,       TKrnlFileSys::TRawFileFind& fndbToFill
                            ,       tCIDLib::TBoolean&          bFound
                            , const tCIDLib::EDirSearchFlags    eFlags)
{
    // If there is an existing search, then close it
    if (m_hdirSearch.m_phdiriThis->hDir)
    {
        if (!bClose())
            return kCIDLib::False;
    }

    WIN32_FIND_DATA FileData;
    HANDLE hTmp = ::FindFirstFile(pszToSearch, &FileData);

    if (hTmp == INVALID_HANDLE_VALUE)
    {
        //
        //  If it was for any other reason that the file not being found,
        //  then its an error. Otherwise just return not found but no error.
        //
        const tCIDLib::TOSErrCode errcReason = ::GetLastError();
        if ((errcReason != ERROR_FILE_NOT_FOUND)
        &&  (errcReason != ERROR_PATH_NOT_FOUND))
        {
            TKrnlError::SetLastHostError(errcReason);
            return kCIDLib::False;
        }
        bFound = kCIDLib::False;
        return kCIDLib::True;
    }

    //
    //  Now we continue to check until we find one that meets the search
    //  criteria provided.
    //
    bFound = kCIDLib::False;
    while (!bFound)
    {
        //
        //  Now we need to see if this one matches the search criteria that
        //  were passed in.
        //
        bFound = bCheckSearchCriteria(FileData, eFlags);

        if (!bFound)
        {
            if (!::FindNextFile(hTmp, &FileData))
            {
                const tCIDLib::TOSErrCode errcReason = ::GetLastError();
                if (errcReason == ERROR_NO_MORE_FILES)
                    break;;
                TKrnlError::SetLastHostError(errcReason);
                return kCIDLib::False;
            }
        }
    }

    if (bFound)
    {
        //
        //  We found at least one, so convert the find buffer to our format,
        //  into the caller's buffer.
        //
        if (!bConvertFindBuf(FileData, fndbToFill))
        {
            ::FindClose(hTmp);
            return kCIDLib::False;
        }

        // Store the temp handle and the search criteria flags
        m_hdirSearch.m_phdiriThis->hDir = hTmp;
        m_eSearchFlags = eFlags;
    }
     else
    {
        // We never found one, so close the handle
        ::FindClose(hTmp);
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlDirSearch::bFindNext(  TKrnlFileSys::TRawFileFind& fndbToFill
                            , tCIDLib::TBoolean&        bFound)
{
    // If the find first was not done yet, then its not ready
    if (!m_hdirSearch.m_phdiriThis->hDir)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    //
    //  Now we continue to check until we find one that meets the search
    //  criteria provided.
    //
    bFound = kCIDLib::False;
    WIN32_FIND_DATA FileData;
    while (!bFound)
    {
        //
        //  Look for another file. If its just no more files, then break
        //  out; else, its an error so return false.
        //
        if (!::FindNextFile(m_hdirSearch.m_phdiriThis->hDir, &FileData))
        {
            const tCIDLib::TOSErrCode errcReason = ::GetLastError();
            if (errcReason == ERROR_NO_MORE_FILES)
                break;

            TKrnlError::SetLastHostError(errcReason);
            return kCIDLib::False;
        }

        //
        //  Check this one against the search criteria. If it matches, then
        //  we will break out and return this file. Otherwise, we'll keep
        //  looking.
        //
        bFound = bCheckSearchCriteria(FileData, m_eSearchFlags);
    }

    // Convert the system structure to our structure, if we found one.
    if (bFound)
        return bConvertFindBuf(FileData, fndbToFill);

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlDirSearch::bSearchIsOpen() const
{
    return (m_hdirSearch.m_phdiriThis->hDir != 0);
}




// ---------------------------------------------------------------------------
//   CLASS: TKrnlDirChangeInfo
//  PREFIX: kdchi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlDirChangeInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlDirChangeInfo::TKrnlDirChangeInfo() :

    m_eChange(tCIDLib::EDirChanges::None)
{
}


// ---------------------------------------------------------------------------
//  TKrnlDirChangeInfo: Public operators
// ---------------------------------------------------------------------------
TKrnlDirChangeInfo& TKrnlDirChangeInfo::operator=(const TKrnlDirChangeInfo& kdchiSrc)
{
    if (&kdchiSrc != this)
    {
        m_eChange = kdchiSrc.m_eChange;
        m_kstrName = kdchiSrc.m_kstrName;
        m_kstrNew = kdchiSrc.m_kstrNew;
    }
    return *this;
}



// ---------------------------------------------------------------------------
//   CLASS: TKrnlDirChangeMon
//  PREFIX: kdchm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlDirChangeMon: Public data
// ---------------------------------------------------------------------------
constexpr tCIDLib::TCard4 c4DirMonBufSz = 32 * 1024;
struct TKrnlDirChangeMon::TDirChangeMonitorData
{
    tCIDLib::TVoid* m_pBuffer;
    HANDLE  m_hDir;
};


// ---------------------------------------------------------------------------
//  TKrnlDirChangeMon: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlDirChangeMon::TKrnlDirChangeMon() :

    m_bDoSubDirs(kCIDLib::False)
    , m_eFilters(tCIDLib::EDirChFilters::None)
    , m_pData(nullptr)
{
    // Allocate our internal structure
    m_pData = new TDirChangeMonitorData{0};
    m_pData->m_hDir = INVALID_HANDLE_VALUE;
}

TKrnlDirChangeMon::~TKrnlDirChangeMon()
{
    // Clean up our data if not already
    if (m_pData)
    {
        bCleanup();
        delete m_pData;
        m_pData = nullptr;
    }
}


// ---------------------------------------------------------------------------
//  TKrnlDirChangeMon: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Do a cancel on the directory handle, to break out any thread that is blocked on it
tCIDLib::TBoolean TKrnlDirChangeMon::bCancel()
{
    if (m_pData->m_hDir != INVALID_HANDLE_VALUE)
    {
        if (!::CancelIoEx(m_pData->m_hDir, NULL))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_Cancel);
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


// Clean up anything that got created
tCIDLib::TBoolean TKrnlDirChangeMon::bCleanup()
{
    // Close the handle if needed
    if (m_pData->m_hDir != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_pData->m_hDir);
        m_pData->m_hDir = INVALID_HANDLE_VALUE;
    }

    // Delete the memory buffer if needed
    if (m_pData->m_pBuffer)
    {
        _aligned_free(m_pData->m_pBuffer);
        m_pData->m_pBuffer = nullptr;
    }

    m_bDoSubDirs = kCIDLib::False;
    m_eFilters = tCIDLib::EDirChFilters::None;
    m_kstrTargetDir.Clear();
    return kCIDLib::True;
}



// Indicate whether we currently are monitoring or not
tCIDLib::TBoolean TKrnlDirChangeMon::bIsReady() const
{
    return (m_pData && (m_pData->m_hDir != INVALID_HANDLE_VALUE));
}


tCIDLib::TBoolean
TKrnlDirChangeMon::bReadChanges(TChangeList& kllstToFill, tCIDLib::TCard4& c4ValidCnt)
{
    // Make sure we return zero unless proven otherwise
    c4ValidCnt = 0;

    // Make sure it's ready
    if (m_pData->m_hDir == INVALID_HANDLE_VALUE)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    //
    //  Let's do the read. Our changes enum has the same bits as the Windows flags
    //  so we can pass it directly in.
    //
    tCIDLib::TCard4 c4OutBytes = 0;
    BOOL bRes = ::ReadDirectoryChangesW
    (
        m_pData->m_hDir
        , m_pData->m_pBuffer
        , c4DirMonBufSz
        , m_bDoSubDirs
        , (DWORD)m_eFilters
        , &c4OutBytes
        , NULL
        , NULL
    );

    if (!bRes)
    {
        DWORD dwErr = ::GetLastError();

        // We treat the overflow error specially
        if (dwErr == ERROR_NOTIFY_ENUM_DIR)
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_OutOfSync);
        else
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_ReadFault, dwErr);
        return kCIDLib::False;
    }

    // Look at the buffer as the correct structure type
    FILE_NOTIFY_INFORMATION* pNotifyInfo
    (
        static_cast<FILE_NOTIFY_INFORMATION*>(m_pData->m_pBuffer)
    );

    // Short circuit if nothing. Valid count was already set to zero
    if (!c4OutBytes)
        return kCIDLib::True;

    //
    //  There something to deal with, so let's load them up. Set up the end
    //  flag to indicate if we have any incoming entries we can reuse.
    //
    tCIDLib::TBoolean bEnd = kllstToFill.bIsEmpty();
    tCIDLib::TCard4 c4Offset = 0;
    kllstToFill.bResetCursor();
    while (kCIDLib::True)
    {
        // Update this guy from the current report
        tCIDLib::EDirChanges eChange = tCIDLib::EDirChanges::None;
        switch(pNotifyInfo->Action)
        {
            case FILE_ACTION_RENAMED_OLD_NAME :
            case FILE_ACTION_RENAMED_NEW_NAME :
                eChange = tCIDLib::EDirChanges::Renamed;
                break;

            case FILE_ACTION_ADDED :
                eChange = tCIDLib::EDirChanges::Added;
                break;

            case FILE_ACTION_REMOVED :
                eChange = tCIDLib::EDirChanges::Removed;
                break;

            case FILE_ACTION_MODIFIED :
                eChange = tCIDLib::EDirChanges::Modified;
                break;

            default :
                break;
        };


        // If it's something we understand, process it
        if (eChange != tCIDLib::EDirChanges::None)
        {
            if (pNotifyInfo->Action == FILE_ACTION_RENAMED_OLD_NAME)
            {
                //
                //  It's the first of a rename pair. So save it away and we'll use
                //  the next time around
                //
                //  We can't see two olds in a row. So if it's not cleared, we
                //  have to return an out of sync error and he has to rescan.
                //
                if (m_kdchiOwedRename.m_eChange != tCIDLib::EDirChanges::None)
                {
                    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_OutOfSync);
                    return kCIDLib::False;
                }

                m_kdchiOwedRename.m_eChange = tCIDLib::EDirChanges::Renamed;
                m_kdchiOwedRename.m_kstrName.SetUnterminated
                (
                    pNotifyInfo->FileName
                    , pNotifyInfo->FileNameLength / kCIDLib::c4UniBytes
                );
            }
             else
            {
                //
                //  We are going to add another to the list. If we have used up
                //  all of the existing entries, add another. Else target the
                //  next available one.
                //
                TKrnlDirChangeInfo* pkdchiCur = nullptr;
                if (!bEnd)
                {
                    // We have another existing one
                    bEnd = kllstToFill.bNext(pkdchiCur);
                }
                 else
                {
                    pkdchiCur = kllstToFill.pobjAddNew(new TKrnlDirChangeInfo());
                }

                // And update the caller's count so he knows how many are good
                c4ValidCnt++;

                //
                //  If this was a renamed old, then we owe a rename operation
                //  and the next one should be a new name.
                //
                if (pNotifyInfo->Action == FILE_ACTION_RENAMED_NEW_NAME)
                {
                    // We should have an owed rename to add to
                    if (m_kdchiOwedRename.m_eChange != tCIDLib::EDirChanges::Renamed)
                    {
                        //
                        //  This is bad since we missed something, so we return an
                        //  out of sync error. The caller needs to rescan and start
                        //  over.
                        //
                        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_OutOfSync);
                        return kCIDLib::False;
                    }

                    // Copy this over to the current one and clear it
                    *pkdchiCur = m_kdchiOwedRename;
                    m_kdchiOwedRename.m_eChange = tCIDLib::EDirChanges::None;

                    // And now add in the new name
                    pkdchiCur->m_kstrNew.SetUnterminated
                    (
                        pNotifyInfo->FileName
                        , pNotifyInfo->FileNameLength / kCIDLib::c4UniBytes
                    );
                }
                 else
                {
                    // This means we only got one rename event, so out of sync
                    if (m_kdchiOwedRename.m_eChange != tCIDLib::EDirChanges::None)
                    {
                        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_OutOfSync);
                        return kCIDLib::False;
                    }

                    // Nothing special so set it
                    pkdchiCur->m_eChange = eChange;
                    pkdchiCur->m_kstrName.SetUnterminated
                    (
                        pNotifyInfo->FileName
                        , pNotifyInfo->FileNameLength / kCIDLib::c4UniBytes
                    );
                }
            }
        }

        // If no next offset, we are done
        if (!pNotifyInfo->NextEntryOffset)
            break;

        //
        //  As a sanity check, keep track off the offset. If we see it go beyond the
        //  returned bytes, that's bad.
        //
        c4Offset += pNotifyInfo->NextEntryOffset;
        if (c4Offset >= c4OutBytes)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_BufferOverflow);
            return kCIDLib::False;
        }

        // Seems like more so move forward
        pNotifyInfo = tCIDLib::pOffsetNCPtr<FILE_NOTIFY_INFORMATION>
        (
            pNotifyInfo, pNotifyInfo->NextEntryOffset
        );
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlDirChangeMon::bStartMonitor(const  tCIDLib::TCh* const     pszTargetDir
                                , const tCIDLib::EDirChFilters  eFilters
                                , const tCIDLib::TBoolean       bDoSubDirs)
{
    TDirChangeMonitorData* pData = static_cast<TDirChangeMonitorData*>(m_pData);

    // If already going, that's an error
    if (pData->m_hDir != INVALID_HANDLE_VALUE)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    // Allocate the buffer, which must be aligned
    if (!pData->m_pBuffer)
    {
        pData->m_pBuffer = _aligned_malloc(c4DirMonBufSz, 32);
        if (!pData->m_pBuffer)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcMem_AllocError, ::GetLastError());
            return kCIDLib::False;
        }
    }

    //
    //  Try to open the target. All non-exclusive since the point of this is to watch
    //  what other folks do here.
    //
    HANDLE hDir = ::CreateFile
    (
        pszTargetDir
        , GENERIC_READ
        , FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE
        , NULL
        , OPEN_EXISTING
        , FILE_FLAG_BACKUP_SEMANTICS
        , NULL
    );

    if (!hDir)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_OpenFailed, ::GetLastError());
        return kCIDLib::False;
    }

    // OK, it worked so store the info flag and return success
    pData->m_hDir = hDir;
    m_bDoSubDirs = bDoSubDirs;
    m_eFilters = eFilters;
    m_kstrTargetDir = pszTargetDir;

    return kCIDLib::True;
}





// ---------------------------------------------------------------------------
//  TKrnlFileSys functions
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TKrnlFileSys::bCopy(const  tCIDLib::TCh* const  pszSourceFile
                    , const tCIDLib::TCh* const pszTargetFile
                    , const tCIDLib::TBoolean   bFailIfExists)
{
    if (!::CopyFile(pszSourceFile, pszTargetFile, bFailIfExists))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Create a temporary file name in the system temp directory, or a particular directory.
//
//  If the unique number is non-zero then we just create a path and return. We don't try to
//  create the file. Else we create the file with zero size and then close it, reserving that
//  name until the caller can use it.
//
tCIDLib::TBoolean
TKrnlFileSys::bCreateTmpFileName(       tCIDLib::TCh* const pszToFillIn
                                , const tCIDLib::TCard4     c4BufChars
                                , const tCIDLib::TCard4     c4UniqueNum)
{
    // Get the system temp path first
    tCIDLib::TCh szPath[kCIDLib::c4MaxPathLen];
    if (!::GetTempPath(kCIDLib::c4MaxPathLen, szPath))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // And call the other version
    return bCreateTmpFileNameIn(szPath, pszToFillIn, c4BufChars, c4UniqueNum);
}

tCIDLib::TBoolean
TKrnlFileSys::bCreateTmpFileNameIn( const   tCIDLib::TCh* const pszTarDir
                                    ,       tCIDLib::TCh* const pszToFillIn
                                    , const tCIDLib::TCard4     c4BufChars
                                    , const tCIDLib::TCard4     c4UniqueNum)
{
    // And get a temp file in the target path, no prefix
    tCIDLib::TCh szFile[kCIDLib::c4MaxPathLen];
    if (!::GetTempFileName(pszTarDir, 0, c4UniqueNum, szFile))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    if (TRawStr::c4StrLen(szFile) > c4BufChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
        return kCIDLib::False;
    }

    // Now copy the created path over
    TRawStr::CopyStr(pszToFillIn, szFile, c4BufChars);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bExists(  const   tCIDLib::TCh* const         pszToFind
                        ,       tCIDLib::TBoolean&          bResult
                        ,       tCIDLib::TBoolean&          bIsUnique
                        , const tCIDLib::EDirSearchFlags    eFlags)
{
    // Call a local helper. The in value of unqiue indicates if we want to check it
    bIsUnique = kCIDLib::True;
    return bCheckExists(pszToFind, bResult, bIsUnique, eFlags);
}


tCIDLib::TBoolean
TKrnlFileSys::bExists(  const   tCIDLib::TCh* const         pszToFind
                        ,       tCIDLib::TBoolean&          bResult
                        , const tCIDLib::EDirSearchFlags    eFlags)
{
    // Call a local helper. Indicate no uniqueness check
    tCIDLib::TBoolean bIsUnique = kCIDLib::False;
    return bCheckExists(pszToFind, bResult, bIsUnique, eFlags);
}


tCIDLib::TBoolean
TKrnlFileSys::bFindInPath(  const   tCIDLib::TCh* const pszPath
                            , const tCIDLib::TCh* const pszSpec
                            ,       tCIDLib::TCh* const pszMatch
                            , const tCIDLib::TCard4     c4BufChars)
{
    tCIDLib::TCh* pszDummy;
    if (!::SearchPath(pszPath, pszSpec, 0, c4BufChars, pszMatch, &pszDummy))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bFindInSysPath(   const   tCIDLib::TCh* const pszSpec
                                ,       tCIDLib::TCh* const pszMatch
                                , const tCIDLib::TCard4     c4BufChars)
{
    //
    //  Just get the system path and call the other version. We don't want
    //  to lock the environment during this, so get a copy of the path.
    //  Otherwise someone could change while we are working with it. So we
    //  lock it just long enough to find it and replicate it.
    //
    tCIDLib::TCh* pszTmpPath = 0;
    {
        TKrnlEnvironment::Lock();
        const tCIDLib::TCh* pszPATH = TKrnlEnvironment::pszFind(L"PATH");
        if (!pszPATH)
        {
            TKrnlEnvironment::Unlock();
            return kCIDLib::False;
        }
        pszTmpPath = TRawStr::pszReplicate(pszPATH);
        TKrnlEnvironment::Unlock();
    }

    // Insure it gets cleaned up
    TArrayJanitor<tCIDLib::TCh> janPath(pszTmpPath);

    tCIDLib::TCh* pszDummy;
    if (!::SearchPath(pszTmpPath, pszSpec, 0, c4BufChars, pszMatch, &pszDummy))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bHasWildCards(const tCIDLib::TCh* const pszToSearch)
{
    static const tCIDLib::TCh* const pszWildCards = L"?*";

    // Search the path for any wild card characters for Win32
    return (TRawStr::pszFindChars(pszToSearch, pszWildCards, 0) != 0);
}


tCIDLib::TBoolean
TKrnlFileSys::bIsDirectory(const tCIDLib::TCh* const pszDirToCheck)
{
    // If null or empty, then obviously not
    if (!pszDirToCheck || !*pszDirToCheck)
        return kCIDLib::False;

    //
    //  We have to deal with a couple of special cases here. If they pass us
    //  something like X: or X:\, we will say yes it is a directory. We cannot
    //  pass this to FindFirst because it will say its not one.
    //
    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszDirToCheck);
    if (c4Len < 4)
    {
        if ((TRawStr::bIsAlpha(pszDirToCheck[0]))
        &&  (pszDirToCheck[1] == L':'
        ||   (pszDirToCheck[2] == L'\\')
        ||   (pszDirToCheck[2] == kCIDLib::chNull)))
        {
            return kCIDLib::True;
        }
    }

    //
    //  If it ends with a separator, we have to remove it, so we have to
    //  make a copy unfortunately.
    //
    const tCIDLib::TCh* pszCheck = pszDirToCheck;
    tCIDLib::TCh* pszClean = 0;
    if (pszDirToCheck[c4Len - 1] == L'\\')
    {
        pszClean = TRawStr::pszReplicate(pszCheck);
        pszClean[c4Len - 1] = kCIDLib::chNull;
        pszCheck = pszClean;
    }
    TArrayJanitor<tCIDLib::TCh> janClean(pszClean);

    // Else we just use the bExists method
    tCIDLib::TBoolean bRes;
    if (!bExists(pszCheck, bRes, tCIDLib::EDirSearchFlags::AllDirs))
        return kCIDLib::False;

    // Make sure it doesn't have any wild cards
    return (bRes && !bHasWildCards(pszCheck));
}


//
//  We don't return these, but if they should get directory info from another
//  source we want to be able to let them test those paths.
//
tCIDLib::TBoolean
TKrnlFileSys::bIsNormalDir(const tCIDLib::TCh* const pszDirToCheck)
{
    if (TRawStr::bCompareStr(pszDirToCheck, L".")
    ||  TRawStr::bCompareStr(pszDirToCheck, L".."))
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlFileSys::bIsRedirected(const tCIDLib::EStdFiles eStdFile)
{
    //
    //  Get the standard handle for the indicated file. Then try to do a console
    //  specific operation on it. If it fails because the handle isn't a console
    //  handle, then its redirected.
    //
    HANDLE hTmp = INVALID_HANDLE_VALUE;
    if (eStdFile == tCIDLib::EStdFiles::StdIn)
        hTmp = ::GetStdHandle(STD_INPUT_HANDLE);
    else if (eStdFile == tCIDLib::EStdFiles::StdOut)
        hTmp = ::GetStdHandle(STD_OUTPUT_HANDLE);
    else if (eStdFile == tCIDLib::EStdFiles::StdErr)
        hTmp = ::GetStdHandle(STD_ERROR_HANDLE);

    tCIDLib::TCard4 c4Mode;
    if (::GetConsoleMode(hTmp, &c4Mode))
        return kCIDLib::False;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bMakeDirectory(const  tCIDLib::TCh* const pszToCreate
                            , const tCIDLib::TBoolean   bErrIfExists
                            , const tCIDLib::TBoolean   bAllAccess)
{
    //
    //  Create the directory with default access first. If that fails, we
    //  give up.
    //
    if (!::CreateDirectory(pszToCreate, 0))
    {
        const tCIDLib::TCard4 c4Last = ::GetLastError();

        //
        //  See if its because it already exists. If so, and they don't care
        //  about that, then we are ok.
        //
        if ((c4Last == ERROR_ALREADY_EXISTS) && !bErrIfExists)
            return kCIDLib::True;

        // Its its a boo-boo, so return the error
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    //
    //  If they want to apply all access, then we go back and add read
    //  write for all users.
    //
    tCIDLib::TBoolean bRes = kCIDLib::True;
    if (bAllAccess)
    {
        //
        //  Open it back up again and ask for rights to write the
        //  discretionary access rights.
        //
        HANDLE hDir = ::CreateFile
        (
            pszToCreate
            , READ_CONTROL | WRITE_DAC
            , 0
            , NULL
            , OPEN_EXISTING
            , FILE_FLAG_BACKUP_SEMANTICS
            , NULL
        );

        // Almost impossible this would happen, but just in case
        if (hDir == INVALID_HANDLE_VALUE)
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }

        // Get the existing security info
        ACL* pOldDACL;
        SECURITY_DESCRIPTOR* pSD = NULL;
        ::GetSecurityInfo
        (
            hDir
            , SE_FILE_OBJECT
            , DACL_SECURITY_INFORMATION
            , NULL
            , NULL
            , &pOldDACL
            , NULL
            , tCIDLib::pToVoidPP(&pSD)
        );

        // Set up new access info for the Users group
        PSID pSid = NULL;
        SID_IDENTIFIER_AUTHORITY authNt = SECURITY_NT_AUTHORITY;
        ::AllocateAndInitializeSid
        (
            &authNt
            , 2
            , SECURITY_BUILTIN_DOMAIN_RID
            , DOMAIN_ALIAS_RID_USERS
            , 0
            , 0
            , 0
            , 0
            , 0
            , 0
            , &pSid
        );
        EXPLICIT_ACCESS ea= {0};
        ea.grfAccessMode = GRANT_ACCESS;
        ea.grfAccessPermissions = GENERIC_ALL;
        ea.grfInheritance = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE;
        ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
        ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
        ea.Trustee.ptstrName = (LPTSTR)pSid;
        ACL* pNewDACL = 0;

        // And try to set the new permissions
        DWORD err = ::SetEntriesInAcl(1, &ea, pOldDACL, &pNewDACL);
        bRes = (pNewDACL != 0);
        if(bRes)
        {
            ::SetSecurityInfo
            (
                hDir
                , SE_FILE_OBJECT
                , DACL_SECURITY_INFORMATION
                , NULL
                , NULL
                , pNewDACL
                , NULL
            );
        }
         else
        {
            TKrnlError::SetLastKrnlError
            (
                kKrnlErrs::errcSec_SetAccessFailed, ::GetLastError()
            );
        }

        // And free up everything
        ::FreeSid(pSid);
        if (pNewDACL)
            ::LocalFree(pNewDACL);
        ::LocalFree(pSD);
        ::LocalFree(pOldDACL);
        ::CloseHandle(hDir);
    }
    return bRes;
}


tCIDLib::TBoolean
TKrnlFileSys::bMakePath(const tCIDLib::TCh* const pszToCreate)
{
    // Test for a null path
    if (!pszToCreate)
    {
        TKrnlError::SetLastError(kKrnlErrs::errcGen_NullName);
        return kCIDLib::False;
    }

    //
    //  Lets start from the start and create any path components that don't
    //  already exist. The passed string must be a fully qualified path. The
    //  CIDLib version of this method, which is what the outside world would
    //  use, will insure this.
    //
    //  So lets get a temp copy of the string that we can mangle by inserting
    //  nulls into as we move through it. Leave an extra char space so we can
    //  insure it ends with a slash.
    //
    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszToCreate);
    tCIDLib::TCh* pszPath = new tCIDLib::TCh[c4Len + 2];
    TRawStr::CopyStr(pszPath, pszToCreate);
    TArrayJanitor<tCIDLib::TCh> janPath(pszPath);

    // Get rid of any leading/trailng whitespace
    TRawStr::StripStr(pszPath, kCIDLib::szWhitespace, tCIDLib::EStripModes::LeadTrail);

    // If nothing but space, that's obviously wrong
    if (!*pszPath)
    {
        TKrnlError::SetLastError(kKrnlErrs::errcFl_InvalidPathName);
        return kCIDLib::False;
    }

    // If the last char isn't a separator then add one
    if (pszPath[c4Len - 1] != L'\\')
    {
        pszPath[c4Len] = L'\\';
        pszPath[c4Len + 1] = 0;
    }

    //
    //  Use the path string support to find the path part. If it reports a
    //  bad path, then just return with its error stored.
    //
    tCIDLib::TCard4 c4Start;
    tCIDLib::TCard4 c4End;
    if (!TKrnlPathStr::bFindPart(pszPath, c4Start, c4End, tCIDLib::EPathParts::Path))
        return kCIDLib::False;

    // Cap it off at the end of the path part, and get a pointer to it
    tCIDLib::TCh* pszPastEnd = pszPath + c4End;
    *pszPastEnd = 0;

    //
    //  Move up to the start of the path part, which will be past the first
    //  slash after the drive, so we don't have to worry about hitting that
    //  first slash. We just find each separator, put a null there and check
    //  that much of the path, then move forard.
    //
    tCIDLib::TCh* pszEnd = pszPath + c4Start;
    while (pszEnd < pszPastEnd)
    {
        while ((pszEnd < pszPastEnd) && (*pszEnd != L'\\'))
            pszEnd++;

        // Put a null there temporarily
        *pszEnd = 0;

        // And see if this part exists
        tCIDLib::TCard4 c4Attr = ::GetFileAttributes(pszPath);

        if (c4Attr == kCIDLib::c4MaxCard)
        {
            // It doesn't exist, so lets create it
            if (!::CreateDirectory(pszPath, 0))
            {
                TKrnlError::SetLastHostError(::GetLastError());
                return kCIDLib::False;
            }
        }
         else if (!(c4Attr & FILE_ATTRIBUTE_DIRECTORY))
        {
            // It exists but its not a directory
            TKrnlError::SetLastError(kKrnlErrs::errcFl_FlAlreadyExists);
            return kCIDLib::False;
        }

        // Put the slash back again
        *pszEnd = L'\\';

        // Now move up for the next part
        pszEnd++;
    }
    return kCIDLib::True;
}


//
//  Normalize a path and return the adjusted path. This gets rid of . and ..
//  type entries and such.
//
tCIDLib::TBoolean
TKrnlFileSys::bNormalizePath(const  tCIDLib::TCh* const pszToNormalize
                            ,       tCIDLib::TCh* const pszResult
                            , const tCIDLib::TCard4     c4MaxChars)
{
    // And canonicalize it to the local output
    tCIDLib::TCh szOut[MAX_PATH + 1];
    if (!::PathCanonicalize(szOut, pszToNormalize))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcFl_Normalize);
        return kCIDLib::False;
    }

    //
    //  The system call is kind of dumb on a couple of special cases, so
    //  watch for them ourself.
    //
    if (TRawStr::bCompareStr(&szOut[1], L":..")
    ||  TRawStr::bCompareStr(&szOut[1], L":."))
    {
        szOut[2] = kCIDLib::chNull;
    }

    // Ok, copy back to the caller's buffer
    TRawStr::CopyStr(pszResult, szOut, c4MaxChars);
    return kCIDLib::True;
}

tCIDLib::TBoolean
TKrnlFileSys::bNormalizePath(const  tCIDLib::TCh* const pszToNormalize
                            ,       TKrnlString&        kstrToFill)
{
    // And canonicalize it to the local output
    tCIDLib::TCh szOut[MAX_PATH + 1];
    if (!::PathCanonicalize(szOut, pszToNormalize))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcFl_Normalize);
        return kCIDLib::False;
    }

    //
    //  The system call is kind of dumb on a couple of special cases, so
    //  watch for them ourself.
    //
    if (TRawStr::bCompareStr(&szOut[1], L":..")
    ||  TRawStr::bCompareStr(&szOut[1], L":."))
    {
        szOut[2] = kCIDLib::chNull;
    }

    // Ok, copy back to the caller's buffer
    kstrToFill = szOut;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bQueryCurrentDir( tCIDLib::TCh* const     pszToFillIn
                                , const tCIDLib::TCard4 c4BufChars)
{
    if (!::GetCurrentDirectory(c4BufChars, pszToFillIn))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



// Query the full path for a (potentially) partial path
tCIDLib::TBoolean
TKrnlFileSys::bQueryFullPath(const  tCIDLib::TCh* const pszPartialPath
                            ,       tCIDLib::TCh* const pszToFillIn
                            , const tCIDLib::TCard4     c4BufChars)
{
    // Skip any whitespace
    const tCIDLib::TCh* pszTmp = pszPartialPath;
    while (*pszTmp && TRawStr::bIsSpace(*pszTmp))
        pszTmp++;

    // Flesh it out into a temp buffer
    tCIDLib::TCh* pszDummy;
    tCIDLib::TCh szTmp[MAX_PATH + 1];
    tCIDLib::TCard4 c4Len = ::GetFullPathName(pszTmp, MAX_PATH, szTmp, &pszDummy);

    if (!c4Len)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    if (c4Len > c4BufChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
        return kCIDLib::False;
    }

    //
    //  Now canonicalize it, to handle any '..' or '.' entries and such. This
    //  puts it inot another buffer. We can't use the caller's since it has
    //  to be MAX_PATH or larger, and the caller's might not.
    //
    tCIDLib::TCh szCan[MAX_PATH + 1];
    if (!::PathCanonicalize(szCan, szTmp))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Remove any trailing slash while still in internal format
    ::PathRemoveBackslash(szCan);

    // Copy to the caller's buffer
    TRawStr::CopyStr(pszToFillIn, szCan, c4BufChars);

    return kCIDLib::True;
}


//
//  Given a shared drive type volume, return the UNC name for the actual
//  resource shared.
//
tCIDLib::TBoolean
TKrnlFileSys::bQueryMountInfo(  const   tCIDLib::TCh* const pszVolume
                                ,       tCIDLib::TCh* const pszHostToFill
                                , const tCIDLib::TCard4     c4MaxHostChars
                                ,       tCIDLib::TCh* const pszMountInfo
                                , const tCIDLib::TCard4     c4MaxInfoChars)
{
    //
    //  We need to call the 'get universal name' API with the volume name,
    //  and it will give us back a structure that has the UNC name for the
    //  volume, i.e. \\host\share. We then parse that out to get the two
    //  strings we need.
    //
    tCIDLib::TCh achBuf[kCIDLib::c4MaxPathLen];

    tCIDLib::TCard4 c4ActualSz = kCIDLib::c4MaxPathLen;
    const tCIDLib::TCard4 c4Res = ::WNetGetUniversalName
    (
        pszVolume, UNIVERSAL_NAME_INFO_LEVEL, &achBuf, &c4ActualSz
    );

    if (c4Res)
    {
        TKrnlError::SetLastHostError(c4Res);
        return kCIDLib::False;
    }

    // We got it, so lets parse the two bits out of the string that we need
    UNIVERSAL_NAME_INFO* pInfo = (UNIVERSAL_NAME_INFO*)&achBuf;
    tCIDLib::TCh* pszUN = pInfo->lpUniversalName;

    // Check that we have two leading slashes as we should
    if ((*pszUN != L'\\') && (*(pszUN + 1) != L'\\'))
    {
        // Its badly formed for some reason
        return kCIDLib::False;
    }

    // We've got them, so skip them and now search for the next slash
    pszUN += 2;
    tCIDLib::TCh* pszEnd = pszUN;
    while (*pszEnd && (*pszEnd != L'\\'))
        pszEnd++;

    // If we didn't hit, its badly formed
    if (*pszEnd != L'\\')
    {
        // <TBD> Set some error here
        return kCIDLib::False;
    }

    // Put a null there and we have our host name
    *pszEnd = kCIDLib::chNull;
    TRawStr::CopyStr(pszHostToFill, pszUN, c4MaxHostChars);

    // Move move up and look for the next slash or end
    pszEnd++;
    pszUN = pszEnd;
    while (*pszEnd && (*pszEnd != L'\\'))
        pszEnd++;

    // Put a null there and we have our mount info
    *pszEnd = kCIDLib::chNull;
    TRawStr::CopyStr(pszMountInfo, pszUN, c4MaxInfoChars);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bQueryTmpPath(        tCIDLib::TCh* const pszToFillIn
                            , const tCIDLib::TCard4     c4BufChars)
{
    tCIDLib::TCard4 c4Len = ::GetTempPath(c4BufChars, pszToFillIn);
    if (!c4Len)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    if (c4Len > c4BufChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bQueryVolHWType( const   tCIDLib::TCh* const     pszVolPath
                               ,       tCIDLib::EVolHWTypes&   eType)
{
    // If it's a shared directory, we handle that specially
    if (TRawStr::bCompareStrN(pszVolPath, L"\\\\", 2))
    {
        eType = tCIDLib::EVolHWTypes::Share;
        return kCIDLib::True;
    }

    // Use the volume path to get the type
    const tCIDLib::TCard4 c4Type = ::GetDriveType(pszVolPath);

    if (c4Type == DRIVE_UNKNOWN)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcFl_PathNotFound);
        return kCIDLib::False;
    }

    // And translate the type to our enum
    if (c4Type == DRIVE_REMOVABLE)
        eType = tCIDLib::EVolHWTypes::Removable;
    else if (c4Type == DRIVE_FIXED)
        eType = tCIDLib::EVolHWTypes::Fixed;
    else if (c4Type == DRIVE_REMOTE)
        eType = tCIDLib::EVolHWTypes::Remote;
    else if (c4Type == DRIVE_CDROM)
        eType = tCIDLib::EVolHWTypes::CD;
    else if (c4Type == DRIVE_RAMDISK)
        eType = tCIDLib::EVolHWTypes::RAMDisk;
    else
        eType = tCIDLib::EVolHWTypes::Unknown;

    //
    //  If its removeable, then lets go further and try to find out what
    //  type of removeable. We can distinguish floppies at least.
    //
    if (eType == tCIDLib::EVolHWTypes::Removable)
    {
        tCIDLib::TZStr64 szTmpName;
        TRawStr::CopyStr(szTmpName, L"\\\\.\\ :", c4MaxBufChars(szTmpName));
        szTmpName[4] = pszVolPath[0];
        szTmpName[6] = 0;
        HANDLE hDev = ::CreateFile
        (
            szTmpName
            , GENERIC_READ
            , FILE_SHARE_READ | FILE_SHARE_WRITE
            , 0
            , OPEN_EXISTING
            , 0
            , 0
        );

        if (hDev != INVALID_HANDLE_VALUE)
        {
            tCIDLib::TCard4 c4BytesGot;
            DISK_GEOMETRY   DiskGeom[32];
            if (::DeviceIoControl(  hDev
                                    , IOCTL_STORAGE_GET_MEDIA_TYPES
                                    , 0
                                    , 0
                                    , DiskGeom
                                    , sizeof(DiskGeom)
                                    , &c4BytesGot
                                    , 0))
            {
                //
                //  And check the media type. If its something we don't
                //  understand, we'll just keep the generic 'removable'.
                //
                switch(DiskGeom[0].MediaType)
                {
                    case F3_1Pt44_512 :
                    case F3_2Pt88_512 :
                    case F3_20Pt8_512 :
                    case F3_720_512 :
                        eType = tCIDLib::EVolHWTypes::Floppy35;
                        break;

                    case F5_1Pt2_512 :
                    case F5_360_512 :
                    case F5_320_512 :
                    case F5_320_1024 :
                    case F5_180_512 :
                    case F5_160_512 :
                        eType = tCIDLib::EVolHWTypes::Floppy525;
                        break;

                    default :
                        // Just leave it alone
                        break;
                }
            }
        }

        // We can close the device now
        ::CloseHandle(hDev);
    }
    return kCIDLib::True;
}


//
//  Given a volume (drive in the form X:\ or share in the form \\host\share)
//  we get some inf about it.
//
tCIDLib::TBoolean
TKrnlFileSys::bQueryVolumeInfo( const   tCIDLib::TCh* const pszSrcPath
                                ,       tCIDLib::TCard8&    c8VolTotalBytes
                                ,       tCIDLib::TCard8&    c8VolFreeBytes
                                ,       tCIDLib::TCard8&    c8UserAvailBytes)
{
    //
    //  For us, the volume part of the path is the 'X:\' part of it. So lets
    //  just make sure that that part of the path is available and make a copy
    //  of it to pass in to the system.
    //
    if (TRawStr::c4StrLen(pszSrcPath) < 3)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcFl_CantDetermineVol);
        return kCIDLib::False;
    }

    ULARGE_INTEGER uliUserAvail;
    ULARGE_INTEGER uliTotal;
    ULARGE_INTEGER uliVolFree;
    if (!::GetDiskFreeSpaceEx(pszSrcPath, &uliUserAvail, &uliTotal, &uliVolFree))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    c8VolTotalBytes = uliTotal.QuadPart;
    c8VolFreeBytes = uliVolFree.QuadPart;
    c8UserAvailBytes = uliUserAvail.QuadPart;
    return kCIDLib::True;
}


//
//  Query volume info for a single volume indicated in the path. It can be a
//  drive type volume or a directory share type volume.
//
tCIDLib::TBoolean
TKrnlFileSys::bQueryVolumeInfo( const   tCIDLib::TCh* const pszSrcPath
                                ,       TKrnlVolumeInfo&    kvoliToFill)
{
    const tCIDLib::TCard4 c4SrcLen = TRawStr::c4StrLen(pszSrcPath);

    //
    //  For us, the volume part of the path is the 'X:\' part of it. So lets
    //  just make sure that that part of the path is available and make a copy
    //  of it to pass in to the system.
    //
    if (c4SrcLen < 3)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcFl_CantDetermineVol);
        return kCIDLib::False;
    }

    //
    //  Use some local buffers to get the data into before we create the
    //  object.
    //
    tCIDLib::TCard4     c4MaxPathCompLen;
    tCIDLib::TZStr128   szVolumeLabel;
    tCIDLib::TZStr64    szFileSysType;
    tCIDLib::TZStr64    szSerialNum;

    //
    //  It has to end in a slash, which the incoming may not if it's a file
    //  share type of volume, so we have to make sure.
    //
    tCIDLib::TCh szActual[MAX_PATH + 1];
    TRawStr::CopyStr(szActual, pszSrcPath, MAX_PATH);
    if (TRawStr::bCompareStrNI(pszSrcPath, L"\\\\", 2))
    {
        if (pszSrcPath[c4SrcLen - 1] != L'\\')
            TRawStr::CatStr(szActual, L"\\", c4SrcLen + 1);
    }

    //
    //  Query the volume info for this volume. Some stuff we can put straight
    //  into the structure and others we have to put into temps and calculate
    //  for the structure.
    //
    tCIDLib::TCard4 c4VolFlags;
    tCIDLib::TCard4 c4SerNum;
    if (!::GetVolumeInformation
    (
        szActual
        , szVolumeLabel
        , c4MaxBufChars(szVolumeLabel)
        , &c4SerNum
        , &c4MaxPathCompLen
        , &c4VolFlags
        , szFileSysType
        , c4MaxBufChars(szFileSysType)))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    //
    //  Translate the volume flags to our format. We do them into a temp
    //  TCard4 first for convenience.
    //
    tCIDLib::TCard4 c4TmpFlags = 0;
    if (c4VolFlags & FS_CASE_SENSITIVE)
        c4TmpFlags |= tCIDLib::c4EnumOrd(tCIDLib::EVolumeFlags::CaseSensitive);
    if (c4VolFlags & FS_CASE_IS_PRESERVED)
        c4TmpFlags |= tCIDLib::c4EnumOrd(tCIDLib::EVolumeFlags::CasePreserved);
    if (c4VolFlags & FS_UNICODE_STORED_ON_DISK)
        c4TmpFlags |= tCIDLib::c4EnumOrd(tCIDLib::EVolumeFlags::UnicodeNames);
    const tCIDLib::EVolumeFlags eFlags = tCIDLib::EVolumeFlags(c4TmpFlags);

    //
    //  Format the serial number into the structure. Under Win32, its done
    //  as two hex parts with a dash between them.
    //
    const tCIDLib::TCard2 c2Low = TRawBits::c2Low16From32(c4SerNum);
    const tCIDLib::TCard2 c2High = TRawBits::c2High16From32(c4SerNum);

    tCIDLib::TZStr8 szLow;
    tCIDLib::TZStr8 szHigh;

    TRawStr::bFormatVal(c2High, szHigh, 8, tCIDLib::ERadices::Hex);
    TRawStr::bFormatStr
    (
        szHigh, szSerialNum, 4, L'0', tCIDLib::EHJustify::Left
    );
    szSerialNum[4] = L'-';

    TRawStr::bFormatVal(c2Low, szLow, 8, tCIDLib::ERadices::Hex);
    TRawStr::bFormatStr
    (
        szLow, &szSerialNum[5], 4, L'0', tCIDLib::EHJustify::Left
    );

    // Loop up the drive type of the volume
    tCIDLib::EVolHWTypes eType;
    if (!bQueryVolHWType(szActual, eType))
        return kCIDLib::False;

    //
    //  If it's a share, then it may have been entered by hand and we want
    //  to get the cannonical form of it, i.e. with the right upper/lower
    //  case for the host and share name.
    //
    if (eType == tCIDLib::EVolHWTypes::Share)
    {
        tCIDLib::TCh szTmp[MAX_PATH + 1];
        if (::GetVolumePathName(szActual, szTmp, MAX_PATH))
        {
            TRawStr::CopyStr(szActual, szTmp);
        }
    }

    //
    //  Ok, now we can set up the object, using all the temp info we've saved
    //  away. We use the adjusted path here since volumes are expected to end
    //  with a slash.
    //
    kvoliToFill.Set
    (
        c4MaxPathCompLen
        , eFlags
        , eType
        , szFileSysType
        , szActual
        , szVolumeLabel
        , szSerialNum
    );

    return kCIDLib::True;
}


//
//  Query a list of all available volumes. This can optionally include file
//  shares. If not, then it only includes defined (real and virtual) drives.
//
//  They provide a serial number so we can know if they already have the
//  most recent info or not
//
tCIDLib::TBoolean TKrnlFileSys
::bQueryVolumeList(         TKrnlLList<TKrnlVolumeInfo>&        kllistGood
                    ,       tCIDLib::TCard4&                    c4VolCount
                    ,       TKrnlLList<TKrnlVolFailureInfo>&    kllistBad
                    ,       tCIDLib::TCard4&                    c4ErrCount
                    , const tCIDLib::TBoolean                   bIncludeShares
                    , const tCIDLib::TBoolean                   bForceUpdate)
{
    // Lock the sync critical section that protects the volume data
    {
        TKrnlCritSecLocker kcrslSync(CIDKernel_FileSystem::pkcrsLock);

        //
        //  If they want to a new scan, or we've not done an initial
        //  scan yet or there are changes, then do an update of the list.
        //
        if (bForceUpdate
        ||  !CIDKernel_FileSystem::bFirstScan
        ||  (CIDKernel_FileSystem::c4VolChangeCnt != 0))
        {
            ResetVolList();

            //
            //  If we'd not done the first scan yet, then we have now so
            //  update the flag.
            //
            CIDKernel_FileSystem::bFirstScan = kCIDLib::True;

            // Zero out the change counter now that we are up to date
            CIDKernel_FileSystem::c4VolChangeCnt = 0;

            // Bump the volume list serial number
            CIDKernel_FileSystem::c4VolListSerialNum++;
        }

        //
        //  Now loop through all the possible drives and count how many
        //  volumes and error info objects there are.
        //
        c4VolCount = 0;
        c4ErrCount = 0;

        tCIDLib::TCard4 c4Index;
        for (c4Index = 0; c4Index < CIDKernel_FileSystem::c4MaxVolumes; c4Index++)
        {
            switch(CIDKernel_FileSystem::aeVolStatuses[c4Index])
            {
                case CIDKernel_FileSystem::EVolStat_Error :
                    kllistBad.pobjAddNew
                    (
                        new TKrnlVolFailureInfo(CIDKernel_FileSystem::akvolfiCur[c4Index])
                    );
                    c4ErrCount++;
                    break;

                case CIDKernel_FileSystem::EVolStat_Used :
                    kllistGood.pobjAddNew
                    (
                        new TKrnlVolumeInfo(CIDKernel_FileSystem::akvoliCur[c4Index])
                    );
                    c4VolCount++;
                    break;

                default :
                    break;
            };
        }
    }

    //
    //  We can unlock after that, because the rest we query per call. So if they
    //  want to include shares, let's call a helper to do that since it's a good
    //  bit of work.
    //
    if (bIncludeShares)
        LoadNetShares(0, kllistGood, c4VolCount);

    return kCIDLib::True;
}



tCIDLib::TBoolean
TKrnlFileSys::bRemoveDir(const tCIDLib::TCh* const pszToDelete)
{
    constexpr tCIDLib::TCard4 c4MaxTries = 15;
    tCIDLib::TCard4 c4Tries = 0;
    while (kCIDLib::True)
    {
        // Try it. If it works, break out and return true
        if (::RemoveDirectory(pszToDelete))
            break;

        // Indicate we've made another try
        c4Tries++;

        // It didn't, so get the last error
        tCIDLib::TCard4 c4Err = ::GetLastError();

        //
        //  If a sharing violation or dir not empty, we'll
        //  pause a bit and try again.
        //
        if (((c4Err == ERROR_SHARING_VIOLATION)
        ||   (c4Err == ERROR_DIR_NOT_EMPTY))
        &&  (c4Tries < c4MaxTries))
        {
            CIDKernel_FileSystem::c4FSRetryCount++;
            ::Sleep(200);
            continue;
        }

        // Not one of our special cases or we've timed out
        TKrnlError::SetLastHostError(c4Err);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bRemoveFile(const tCIDLib::TCh* const pszToDelete)
{
    constexpr tCIDLib::TCard4 c4MaxTries = 15;
    tCIDLib::TCard4 c4Tries = 0;
    tCIDLib::TBoolean bRet = kCIDLib::False;
    while (kCIDLib::True)
    {
        // Try it. If it works, break out and return true
        if (::DeleteFile(pszToDelete))
        {
            bRet = kCIDLib::True;
            break;
        }

        c4Tries++;
        tCIDLib::TCard4 c4Err = ::GetLastError();
        if (c4Err == ERROR_ACCESS_DENIED)
        {
            //
            //  It's read only, so try to remove that. If we can't, then we have
            //  failed. If we can, go back around and try again.
            //
            //  If tries is > 1, then something is wrong since we already had to
            //  have removed the read-only attribute.
            //
            if (!::SetFileAttributes(pszToDelete, FILE_ATTRIBUTE_NORMAL)
            || (c4Tries > 1))
            {
                TKrnlError::SetLastHostError(c4Err);
                break;
            }
        }
         else if (c4Err == ERROR_SHARING_VIOLATION)
        {
            //
            //  Someone has it open, If we are maxed out on retries, we have to
            //  give up. Else sleep a little and go back to the top.
            //
            if (c4Tries >= c4MaxTries)
            {
                TKrnlError::SetLastHostError(c4Err);
                break;
            }

            CIDKernel_FileSystem::c4FSRetryCount++;
            ::Sleep(200);
        }
         else
        {
            // Anything else, just consider it an error
            TKrnlError::SetLastHostError(c4Err);
            break;
        }
    }
    return bRet;
}


//
//  This is one where we may have issues because of stupid anti-virus
//  programs that are more like viruses than preventatives. It an keep
//  us from trying to remove the directory. So we'll retry it a few times
//  if we get errors that indicate that might be the problem.
//
tCIDLib::TBoolean
TKrnlFileSys::bRename(  const   tCIDLib::TCh* const pszOldName
                        , const tCIDLib::TCh* const pszNewName)
{
    constexpr tCIDLib::TCard4 c4MaxTries = 15;
    tCIDLib::TCard4 c4Tries = 0;
    while (kCIDLib::True)
    {
        // Try it> If it works, break out and return true
        if (::MoveFile(pszOldName, pszNewName))
            break;

        // Indicate we've made another try
        c4Tries++;

        // It didn't, so get the last error
        tCIDLib::TCard4 c4Err = ::GetLastError();

        //
        //  If a sharing violation or dir not empty, we'll
        //  pause a bit and try again.
        //
        if (((c4Err == ERROR_SHARING_VIOLATION)
        ||   (c4Err == ERROR_DIR_NOT_EMPTY))
        &&  (c4Tries < c4MaxTries))
        {
            CIDKernel_FileSystem::c4FSRetryCount++;
            ::Sleep(200);
            continue;
        }

        // Not one of our special cases or we've timed out
        TKrnlError::SetLastHostError(c4Err);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bReplaceFile( const   tCIDLib::TCh* const pszToReplace
                            , const tCIDLib::TCh* const pszNewFile
                            , const tCIDLib::TCh* const pszBackupName)
{
    //
    //  This will fail if the target doesn't exist. So, if we get a file not found
    //  error, we will call bRename above instead, since it just becomes a simple
    //  rename.
    //
    //  We could also get blocked by an anti-virus, so we will retry a few times
    //  if we get one of the errors that indicate that something prevented one of
    //  the actions required.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 15; c4Index++)
    {
        BOOL bRes = ::ReplaceFile(pszToReplace, pszNewFile, pszBackupName, 0, 0, 0);

        // If it worked, we are happy, so break out
        if (bRes)
            break;

        // Else let's see what happened
        DWORD dwErr = ::GetLastError();

        if (dwErr == ERROR_FILE_NOT_FOUND)
        {
            //
            //  Just means that the target file to replace wasn't found. So just change
            //  it to a rename.
            //
            return bRename(pszNewFile, pszToReplace);
        }
         else if (dwErr == ERROR_UNABLE_TO_REMOVE_REPLACED)
        {
            //
            //  We couldn't do it, but in a way that basically leaves us still in
            //  a position to just try it again since the original and new files are
            //  still where they were. So sleep a bit then go back around.
            //
            ::Sleep(200);
            CIDKernel_FileSystem::c4FSRetryCount++;
        }
         else if (dwErr == ERROR_UNABLE_TO_MOVE_REPLACEMENT)
        {
            //
            //  If there was a backup file, then we can just try again since that prevented
            //  the operation from getting far enough to change the names of the original
            //  and new files.
            //
            //  Else, we have to try to rename the new file ourself.
            //
            if (pszBackupName)
            {
                ::Sleep(200);
                CIDKernel_FileSystem::c4FSRetryCount++;
            }
             else
            {
                // If this fails, we are screwed. This also does retries.
                if (bRename(pszNewFile, pszToReplace))
                    return kCIDLib::True;

                // Make sure the reported error is the original one
                TKrnlError::SetLastHostError(dwErr);
                return kCIDLib::False;
            }
        }
         else if (dwErr == ERROR_UNABLE_TO_MOVE_REPLACEMENT_2)
        {
            //
            //  The new file is still there, so let's try to just complete the rename
            //  ourself. If that fails, and they provided a backup name, the original
            //  file is still there under that name, so try to rename it back.
            //
            //  The rename method does its own retries.
            //
            //  If both these fail, we are screwed.
            //
            if (bRename(pszNewFile, pszToReplace))
                return kCIDLib::True;

            if (pszBackupName)
                return bRename(pszBackupName, pszToReplace);

            //
            //  Make sure the actual error reported is the original one, not one set
            //  by the rename operations above.
            //
            TKrnlError::SetLastHostError(dwErr);
            return kCIDLib::False;
        }
         else
        {
            // Some other error, so we just failed outright
            TKrnlError::SetLastHostError(dwErr);
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bSetCurrentDir(const tCIDLib::TCh* const pszNewDir)
{
    if (!::SetCurrentDirectory(pszNewDir))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bSetPerms(const   tCIDLib::TCh* const pszTarget
                        , const tCIDLib::EFilePerms ePerms)
{
    //
    //  For now, we just check for any write/exec flags to be set. If any
    //  are, then we remove read-only. Else, we set the file to read-only.
    //
    BOOL bRes;
    if (tCIDLib::bAllBitsOn(ePerms, tCIDLib::EFilePerms::AllWE))
    {
        bRes = ::SetFileAttributes
        (
            pszTarget, ::GetFileAttributes(pszTarget) & ~FILE_ATTRIBUTE_READONLY
        );
    }
     else
    {
        bRes = ::SetFileAttributes
        (
            pszTarget, ::GetFileAttributes(pszTarget) | FILE_ATTRIBUTE_READONLY
        );
    }

    if (!bRes)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TCard4 TKrnlFileSys::c4FSRetryCount()
{
    return CIDKernel_FileSystem::c4FSRetryCount;
}


// Return the latest volume list serial number
tCIDLib::TCard4 TKrnlFileSys::c4VolListSerialNum()
{
    return CIDKernel_FileSystem::c4VolListSerialNum;
}


// Allows for login to remote shares
tCIDLib::EConnRes
TKrnlFileSys::eAccessRemoteShare(const  tCIDLib::TCh* const pszTarShare
                                , const tCIDLib::TCh* const pszUserName
                                , const tCIDLib::TCh* const pszPassword
                                , const tCIDLib::TBoolean   bTemp)
{
    NETRESOURCE NetRes = { 0 };
    NetRes.dwType = RESOURCETYPE_DISK;
    NetRes.lpLocalName = nullptr;
    NetRes.lpRemoteName = (LPWSTR)pszTarShare;

    DWORD dwRes = ::WNetAddConnection2W
    (
        &NetRes, pszPassword, pszUserName, bTemp ? CONNECT_TEMPORARY : 0
    );

    if (dwRes != NO_ERROR)
    {
        DWORD dwErr = ::GetLastError();

        //
        //  If it's that we are already connnected using other credentials, then
        //  return that status.
        //
        if (dwErr == ERROR_SESSION_CREDENTIAL_CONFLICT)
            return tCIDLib::EConnRes::AlreadyConnected;

        // Else we consider it a failiure
        TKrnlError::SetLastHostError(dwErr);
        return tCIDLib::EConnRes::Failed;
    }

    // It worked, and we were the first connection
    return tCIDLib::EConnRes::Connected;
}
