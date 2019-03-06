//
// FILE NAME: CIDKernel_InternalHelpers.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/04/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TKrnlWin32 namespace. This guy holds some
//  stuff that is for use totally within the Win32 kernel driver.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  CIDKernel needs a lot of stuff that are only needed here and no where
//  else, so instead of burdening everyone with all these libraries in the
//  tools driver generated command lines, we use pragmas to force them in.
//  So the standard link lines can be much smaller.
// ---------------------------------------------------------------------------
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Mpr.lib")
#pragma comment(lib, "Secur32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "hid.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Winhttp.lib")


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"



// ---------------------------------------------------------------------------
//  Provide impls for variables from the namespace
// ---------------------------------------------------------------------------

tCIDLib::TVoid TKrnlWin32::AtomicHandleSet(HANDLE& hToFill, HANDLE hToSet)
{
    InterlockedExchangePointer(&hToFill, hToSet);
}


//
//  This method handles the parsing of a command line in the native platform
//  format. It breaks it out into its separate components.
//
tCIDLib::TBoolean
TKrnlWin32::bParseCmdLine(  const   tCIDLib::TCh* const pszToParse
                            ,       tCIDLib::TCh**      apszToFill
                            ,       tCIDLib::TCard4&    c4ToFill
                            , const tCIDLib::TCard4     c4MaxParms)
{
    // Init the count return
    c4ToFill = 0;

    //
    //  Allocate a temp buffer to copy each parm into. Make it the same size
    //  as the whole cmd line, which is the biggest it could ever need to
    //  be in the case where there is just one big parm.
    //
    tCIDLib::TCh* pszVal = new tCIDLib::TCh[TRawStr::c4StrLen(pszToParse)+1];
    TArrayJanitor<tCIDLib::TCh> janVal(pszVal);

    tCIDLib::TBoolean   bParen;
    tCIDLib::TCard4     c4Count;

    // While we are not at the terminating double nul, keep going
    const tCIDLib::TCh* pszCmd = pszToParse;
    while (*pszCmd)
    {
        // Move up to the next non-whitespace character
        while (*pszCmd && TRawStr::bIsSpace(*pszCmd))
            pszCmd++;

        // Move up to the end, keeping up with the count
        bParen = kCIDLib::False;
        c4Count = 0;

        tCIDLib::TBoolean bDone = kCIDLib::False;
        while (!bDone)
        {
            // If we hit the end of the parms, then break out
            if (!*pszCmd)
                break;

            //
            //  If we hit a paren, toggle the paren mode and skip it so it
            //  does not go into the output.
            //
            if (*pszCmd == L'"')
            {
                bParen = !bParen;
                pszCmd++;
                continue;
            }

            // If its a whitespace and not in parens, then this one is done
            if (TRawStr::bIsSpace(*pszCmd) && !bParen)
                break;

            // Copy over the current char
            pszVal[c4Count++] = *pszCmd;

            // Move up to the next char in the source
            pszCmd++;
        }

        // If nothing, then we hit the end, so get out
        if (!c4Count)
            break;

        // If the last char is a paren, then get rid of it
        if (c4Count && (pszVal[c4Count - 1] == L'"'))
        {
            pszVal[c4Count - 1] = 0;
            c4Count--;
        }

        //
        //  Go to the end and work back, getting rid of any trailing white
        //  space.
        //
        while (!bDone)
        {
            if (!TRawStr::bIsSpace(pszVal[c4Count - 1]))
                break;
            c4Count--;
            if (!c4Count)
                break;
        }

        //
        //  Terminate it now at the new count. It could be an empty
        //  string if we got something like "  ".
        //
        pszVal[c4Count] = 0;

        // And add it into the list
        apszToFill[c4ToFill++] = TRawStr::pszReplicate(pszVal);

        // Make sure we don't overflow
        if (c4ToFill >= c4MaxParms)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_TooMany);
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}



//
//  This method will map in the optional image header of the passed exe
//  file.
//
tCIDLib::TBoolean
TKrnlWin32::bQueryExeHdr(   const   tCIDLib::TCh* const     pszExePath
                            ,       IMAGE_OPTIONAL_HEADER&  toFill)
{
    //
    //  Lets try to open the file. If that doesn't work then there is
    //  obviously not much more we need to do. We just read access, and
    //  don't care if anyone else reads it while we do.
    //
    HANDLE hExe = ::CreateFile
    (
        pszExePath
        , GENERIC_READ
        , FILE_SHARE_READ
        , 0
        , OPEN_EXISTING
        , FILE_ATTRIBUTE_NORMAL
        , 0
    );

    if (hExe == INVALID_HANDLE_VALUE)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    //
    //  Ok, try to memory map the file. This gives us a mapping handle that
    //  we can use to map the file into our address space. Ask for readonly
    //  access here too.
    //
    HANDLE hMem = ::CreateFileMapping
    (
        hExe
        , 0
        , PAGE_READONLY | SEC_IMAGE
        , 0
        , 0
        , 0
    );

    // Should always work but, hey, what can you do?
    if (!hMem)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        ::CloseHandle(hExe);
        return kCIDLib::False;
    }

    //
    //  Map the view to a pointer, get it as a byte pointer for convenience.
    //  This pointer now 'points' into the file. Once again just ask for
    //  read only access. Just map a page, since that's all we need and the
    //  Exe could be a couple MB for all we know.
    //
    tCIDLib::TCard1* pc1Tmp = reinterpret_cast<tCIDLib::TCard1*>
    (
        ::MapViewOfFile(hMem, FILE_MAP_READ, 0, 0, 4096)
    );

    // Here again, it should never fail but nothing we can do about it
    if (!pc1Tmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        ::CloseHandle(hMem);
        ::CloseHandle(hExe);
        return kCIDLib::False;
    }

    //
    //  Amazing, it all worked. So lets now get a pointer into the file
    //  for the optional PE header. We first have to get the DOS header
    //  and get the offset to the new header from it. Then we can offset
    //  from that by what we know precedes the optional header we want.
    //
    IMAGE_DOS_HEADER* pDosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(pc1Tmp);
    IMAGE_OPTIONAL_HEADER* pHeader = reinterpret_cast<IMAGE_OPTIONAL_HEADER*>
    (
        pc1Tmp
        + pDosHeader->e_lfanew
        + sizeof(DWORD)
        + sizeof(IMAGE_FILE_HEADER)
    );

    // Copy the header over to the caller's buffer
    memcpy(&toFill, pHeader, sizeof(IMAGE_OPTIONAL_HEADER));

    // And now lets let the memory mapping and the file handle go
    tCIDLib::TBoolean bCloseOk = kCIDLib::True;
    if (!::UnmapViewOfFile(pc1Tmp))
        bCloseOk = kCIDLib::False;

    if (!::CloseHandle(hMem))
        bCloseOk = kCIDLib::False;

    if (!::CloseHandle(hExe))
        bCloseOk = kCIDLib::False;

    #if CID_DEBUG_ON
    if (!bCloseOk)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        kmodCIDKernel.KrnlErrorPopUp
        (
            TKrnlError::kerrLast()
            , CID_FILE
            , CID_LINE
            , kmodCIDKernel.pszLoadCIDFacMsg(kKrnlErrs::errcGen_CloseHandle)
        );
    }
    #endif

    return kCIDLib::True;
}



// Builds up a module (dll/exe) name in the format that CIDLib uses
tCIDLib::TVoid
TKrnlWin32::BuildModName(       tCIDLib::TCh* const pszNameBuf
                        , const tCIDLib::TCard4     c4MaxChars
                        , const tCIDLib::TCh* const pszModName
                        , const tCIDLib::TCard4     c4MajVer
                        , const tCIDLib::TCard4     c4MinVer
                        , const tCIDLib::EModTypes  eModType)
{
    // Start off with the basic bmodule name
    TRawStr::CopyStr(pszNameBuf, pszModName, c4MaxChars);

    //
    //  If the passed type is an Exe facility, then we need to add the Exe
    //  extension under the NT platform or it won't work. The facility names
    //  names that we get here never have extensions.
    //
    //  Else, its if a DLL type, then we need to add on the version
    //  extension.
    //
    if (eModType == tCIDLib::EModTypes::Exe)
    {
        TRawStr::CatStr(pszNameBuf, kCIDLib::szExeExtension, c4MaxChars);
    }
     else
    {
        const tCIDLib::TCard4 c4BufSz = 256;
        tCIDLib::TCh szTmpBuf[c4BufSz + 1];

        TRawStr::CatStr(pszNameBuf, L"_", c4MaxChars);
        TRawStr::bFormatVal(c4MajVer, szTmpBuf, c4BufSz);
        TRawStr::CatStr(pszNameBuf, szTmpBuf, c4MaxChars);

        TRawStr::CatStr(pszNameBuf, L"_", c4MaxChars);
        TRawStr::bFormatVal(c4MinVer, szTmpBuf, c4BufSz);
        TRawStr::CatStr(pszNameBuf, szTmpBuf, c4MaxChars);
    }
}


tCIDLib::TCard4
TKrnlWin32::c4XlatCreateAction(const tCIDLib::ECreateActs eAction)
{
    if (eAction == tCIDLib::ECreateActs::CreateIfNew)
        return CREATE_NEW;
    else if (eAction == tCIDLib::ECreateActs::CreateAlways)
        return CREATE_ALWAYS;
    else if (eAction == tCIDLib::ECreateActs::OpenIfExists)
        return OPEN_EXISTING;
    else if (eAction == tCIDLib::ECreateActs::OpenOrCreate)
        return OPEN_ALWAYS;
    else if (eAction == tCIDLib::ECreateActs::TruncateExisting)
        return TRUNCATE_EXISTING;

    #if CID_DEBUG_ON
    kmodCIDKernel.MsgPopUp
    (
        L"Invalid tCIDLib::ECreateActs value"
        , CID_FILE
        , CID_LINE
    );
    #endif
    return 0;
}


tCIDLib::EFileInfoFlags
TKrnlWin32::eConvertAttrs(  const   tCIDLib::TCard4     c4Win32Attrs
                            , const tCIDLib::TCh* const pszFileName)
{
    // To make things easier we build up first in a non-enum value
    tCIDLib::TCard4 c4Ret = 0;

    // We can always read it
    c4Ret = tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::CanRead);

    //
    //  If its not read only, we can write it. And write under Win32
    //  implies execute.
    //
    if (!(c4Win32Attrs & FILE_ATTRIBUTE_READONLY))
    {
        c4Ret |= tCIDLib::TCard4
        (
            tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::CanWrite)
            | tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::CanExecute)
        );
    }

    //
    //  Check the directory/file status
    //
    //  If its a directory, see if its one of the special directories. If
    //  not, mark it normal. If its a file, see if any of the hidden,
    //  system, etc... attributes are on. If not, mark it normal.
    //
    if (c4Win32Attrs & FILE_ATTRIBUTE_DIRECTORY)
    {
        c4Ret |= tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::IsDirectory);

        if (!TRawStr::bCompareStrI(pszFileName, L".")
        &&  !TRawStr::bCompareStrI(pszFileName, L".."))
        {
            c4Ret |= tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::IsNormal);
        }
    }
     else
    {
        if (!(c4Win32Attrs & c4NonNormalAttrs))
            c4Ret |= tCIDLib::c4EnumOrd(tCIDLib::EFileInfoFlags::IsNormal);
    }

    return tCIDLib::EFileInfoFlags(c4Ret);
}



//
//  Any time we block on a handle, if not in the GUI thread, we need to
//  pump messages sufficient to make COM stuff work. If in the GUI thread
//  we don't want to do that since it would allow reentrance on the message
//  loop while doing blocking operations.
//
//  So everyone has to block on handles via the calling thread's thread
//  object. The thread is marked as GUI or not, so we know whether we should
//  do the regular block or the message pumping block.
//
//  We have one that takes a single handlea nd just calls the other version.
//
tCIDLib::EWaitRes
TKrnlWin32::eWaitHandle(        HANDLE              hWait
                        , const tCIDLib::TCard4     c4WaitMillis)
{
    tCIDLib::TCard4 c4Dummy;
    return eWaitHandles(&hWait, 1, c4Dummy, c4WaitMillis);
}


tCIDLib::EWaitRes
TKrnlWin32::eWaitHandles(       HANDLE* const       ahWait
                        ,       tCIDLib::TCard4     c4Count
                        ,       tCIDLib::TCard4&    c4Signaled
                        , const tCIDLib::TCard4     c4WaitMillis)
{
    // Make sure it's within the limit
    if (c4Count > kCIDLib::c4MaxWaitHandles)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_MaxWaitHandles);
        return tCIDLib::EWaitRes::Error;
    }

    tCIDLib::TBoolean bSawQuit = kCIDLib::False;
    tCIDLib::TSInt iQuitCode = 0;

    //
    //  We wait until the current time matches the end time. If we are
    //  in infinit wait mode, we don't bother to set the times since
    //  we never use them.
    //
    tCIDLib::TEncodedTime enctCur = 0;
    tCIDLib::TEncodedTime enctEnd = 0;
    if (c4WaitMillis != INFINITE)
    {
        enctCur = TKrnlTimeStamp::enctNow();
        enctEnd = enctCur + (kCIDLib::enctOneMilliSec * c4WaitMillis);
    }

    //
    //  See if the calling thread is a GUI thread. We act differently if
    //  it is or isn't.
    //
    const tCIDLib::TBoolean bGUIThread = TKrnlThread::bIsCallerGUIThread();

    //
    //  We could be in infinite wait mode, in which case we don't want to
    //  use the end time for loop exit, since it can be iffy doing that
    //  with infinit wait. So we just do an unconditional loop and test
    //  for time out specifically at the end.
    //
    tCIDLib::EWaitRes eRes = tCIDLib::EWaitRes::Timeout;
    while (kCIDLib::True)
    {
        //
        //  Decide how long to wait this time around. If in infinite wait
        //  mode, just always set it to infinite. Else, calculate the number
        //  of milllis to wait this time.
        //
        tCIDLib::TCard4 c4CurMillis;
        if (c4WaitMillis == INFINITE)
            c4CurMillis = INFINITE;
        else
            c4CurMillis = tCIDLib::TCard4((enctEnd - enctCur) / kCIDLib::enctOneMilliSec);

        DWORD Res;
        if (bGUIThread)
        {
            Res = ::WaitForMultipleObjects(c4Count, ahWait, 0, c4CurMillis);
        }
         else
        {
            Res = ::MsgWaitForMultipleObjectsEx
            (
                c4Count
                , ahWait
                , c4CurMillis
                , QS_SENDMESSAGE | QS_POSTMESSAGE
                , MWMO_INPUTAVAILABLE
            );
        }

        if (Res < WAIT_OBJECT_0 + c4Count)
        {
            // An object was signaled
            c4Signaled = Res - WAIT_OBJECT_0;
            eRes = tCIDLib::EWaitRes::Success;
            break;
        }

        if (Res == (WAIT_OBJECT_0 + c4Count))
        {
            //
            //  Pump a message, then keep going. If there are more
            //  messages, we'll just come back again.
            //
            MSG Msg;
            if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
            {
                if (Msg.message == WM_QUIT)
                {
                    bSawQuit = kCIDLib::True;
                    iQuitCode = (int)Msg.wParam;
                }
                 else
                {
                    TranslateMessage(&Msg);
                    DispatchMessage(&Msg);
                }
            }
        }
         else if (Res == WAIT_TIMEOUT)
        {
            //
            //  We used up all remaining time, so we timed out. However, there
            //  is a potential glitch in Windows where it can return a timeout
            //  even if doing an infinite wait. So we watch for that and only
            //  break out if not inifinite.
            //
            if (c4WaitMillis != INFINITE)
                break;
        }
         else
        {
            // It was a failure of some sort
            eRes = tCIDLib::EWaitRes::Error;
            TKrnlError::SetLastKrnlError
            (
                kKrnlErrs::errcGen_HandleWait, ::GetLastError()
            );
            break;
        }

        //
        //  If not in infinite wait mode, then check to see if we need
        //  to do a timeout exit.
        //
        if (c4WaitMillis != INFINITE)
        {
            enctCur = TKrnlTimeStamp::enctNow();
            if (enctCur >= enctEnd)
                break;
        }
    }

    //
    //  If we saw a quit while waiting, dispatch it. Will only happen if
    //  a background thread.
    //
    if (bSawQuit)
        PostQuitMessage(iQuitCode);

    // If we timed out, set that error
    if (eRes == tCIDLib::EWaitRes::Timeout)
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_Timeout);

    //
    //  If we never set it otherwise, then it's still set to timeout, which is
    //  appropriate.
    //
    return eRes;
}



// ---------------------------------------------------------------------------
//  A simple BSTR janitor
// ---------------------------------------------------------------------------
TBSTRJanitor::TBSTRJanitor(const tCIDLib::TCh* const pszVal) :

    bsData(::SysAllocString(pszVal))
{
}

TBSTRJanitor::~TBSTRJanitor()
{
    SysFreeString(bsData);
}

const tCIDLib::TCh* TBSTRJanitor::pszData() const
{
    return reinterpret_cast<const tCIDLib::TCh*>(bsData);
}

tCIDLib::TCh* TBSTRJanitor::pszData()
{
    return reinterpret_cast<tCIDLib::TCh*>(bsData);
}

//
//  This is so this guy can be passed into a method that allocates a BSTR.
//  So we clear it before we return it, so the new one will not make us
//  leak this one.
//
BSTR* TBSTRJanitor::pbsData()
{
    ::SysFreeString(bsData);
    bsData = 0;

    return &bsData;
}

tCIDLib::TVoid TBSTRJanitor::Set(const tCIDLib::TCh* const pszToSet)
{
    ::SysFreeString(bsData);
    bsData = ::SysAllocString(pszToSet);
}

