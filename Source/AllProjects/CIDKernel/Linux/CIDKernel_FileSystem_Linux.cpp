//
// FILE NAME: CIDKernel_FileSystem_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/12/1998
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file provides the Linux specific implementation of the the
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


// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

namespace CIDKernel_FileSys_Linux
{
    const tCIDLib::TCard2 c2Ext2SuperMagic = 0xEF53;
    const tCIDLib::TCard4 c4Ext2VolNameLen = 16;
    const tCIDLib::TCard4 c4MountBufferSize = 4096;

    struct TExt2SuperBlock
    {
        tCIDLib::TCard1 ac1Dummy0[56];
        tCIDLib::TCard1 ac1Magic[2];
        tCIDLib::TCard1 ac1Dummy1[62];
        tCIDLib::TSCh szVolumeName[c4Ext2VolNameLen];
        // There are 888 bytes left, but we'll ignore them
        // because all we want is the volume name.
        //tCIDLib::TCard1 ac1Dummy2[888];
    };

    struct TMountEntry
    {
        tCIDLib::TSCh szDevice[kCIDLib::c4MaxPathLen + 1];
        tCIDLib::TSCh szMountPoint[kCIDLib::c4MaxPathLen + 1];
        tCIDLib::TSCh szType[64];
    };

    tCIDLib::TVoid
    GetExt2Label(const  tCIDLib::TSCh* const    pszDevice
                ,       tCIDLib::TSCh* const    pszToFill
                ,       tCIDLib::TCard4         c4MaxChars)
    {
        pszToFill[0] = L'\000';

        tCIDLib::TSInt iFd = ::open(pszDevice, O_RDONLY);
        if (iFd == -1)
            return;

        if (::lseek(iFd, 1024, SEEK_SET) != 1024)
        {
            ::close(iFd);
            return;
        }

        TExt2SuperBlock esb;
        tCIDLib::TInt4 i4Count = ::read(iFd, (tCIDLib::TSCh*)&esb, sizeof(esb));
        ::close(iFd);
        if (i4Count != sizeof(esb))
            return;

        if (((esb.ac1Magic[1] << 8) | esb.ac1Magic[0]) != c2Ext2SuperMagic)
            return;

        TRawStr::CopyStr(pszToFill
                         , esb.szVolumeName
                         , tCIDLib::MinVal(c4Ext2VolNameLen, c4MaxChars));
    }

    tCIDLib::TBoolean
    GetMountEntry(const tCIDLib::TSCh* const pszFile, TMountEntry& MountEntryToFill)
    {
        FILE* pFile = ::setmntent(MOUNTED, "r");
        if (!pFile)
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        TMountEntry CandidateEntry;
        tCIDLib::TCard4 c4LongestLength = 0;
        struct mntent Context;
        tCIDLib::TSCh szMountBuffer[c4MountBufferSize];
        struct mntent* pEntry = ::getmntent_r(pFile, &Context, szMountBuffer, c4MountBufferSize);
        while (pEntry)
        {
            tCIDLib::TCard4 c4CurLen = ::strlen(pEntry->mnt_dir);
            if (c4CurLen > c4LongestLength)
            {
                if (::strstr(pszFile, pEntry->mnt_dir) == pszFile)
                {
                    c4LongestLength = c4CurLen;
                    TRawStr::CopyStr(CandidateEntry.szDevice
                                     , pEntry->mnt_fsname
                                     , sizeof(CandidateEntry.szDevice));
                    TRawStr::CopyStr(CandidateEntry.szMountPoint
                                     , pEntry->mnt_dir
                                     , sizeof(CandidateEntry.szMountPoint));
                    TRawStr::CopyStr(CandidateEntry.szType
                                     , pEntry->mnt_type
                                     , sizeof(CandidateEntry.szType));
                }
            }
            pEntry = ::getmntent_r(pFile, &Context, szMountBuffer, c4MountBufferSize);
        }
        ::endmntent(pFile);

        if (!c4LongestLength)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotFound);
            return kCIDLib::False;
        }

        TRawStr::CopyStr(MountEntryToFill.szDevice
                         , CandidateEntry.szDevice
                         , sizeof(MountEntryToFill.szDevice) - 1);
        TRawStr::CopyStr(MountEntryToFill.szMountPoint
                         , CandidateEntry.szMountPoint
                         , sizeof(MountEntryToFill.szMountPoint) - 1);
        TRawStr::CopyStr(MountEntryToFill.szType
                         , CandidateEntry.szType
                         , sizeof(MountEntryToFill.szType) - 1);

        return kCIDLib::True;
    }

    // <TBD> deal with this
    tCIDLib::TBoolean
    bQueryVolumeInfo(const  tCIDLib::TSCh* const    pszSrcPath
                    , const TMountEntry&            MountEntry
                    ,       TKrnlVolumeInfo&        kvoliToFill
                    ,       tCIDLib::EVolHWTypes&   eHWType)
    {
        /*
        // Perform the statfs call
        struct statfs StatBuf;
        if (::statfs(pszSrcPath, &StatBuf))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        // Now get the total and available byte counts
        tCIDLib::TFloat8 f8TotalBytes = 0;
        tCIDLib::TFloat8 f8AvailableBytes = 0;
        // If the block size isn't returned, then there's not much
        // we can do.
        if (StatBuf.f_bsize != -1)
        {
            if (StatBuf.f_blocks != fsblkcnt_t(-1))
                f8TotalBytes = StatBuf.f_blocks * StatBuf.f_bsize;
            if (StatBuf.f_bfree != fsblkcnt_t(-1))
                f8AvailableBytes = StatBuf.f_bfree * StatBuf.f_bsize;
        }

        // Now the file name length
        tCIDLib::TCard4 c4MaxPathCompLen =
            (StatBuf.f_namelen == -1) ? 0 : StatBuf.f_namelen;

        // Always set the flags to this
        tCIDLib::EVolumeFlags eFlags =
            tCIDLib::EVolumeFlags(tCIDLib::EVolumeFlags::CasePreserved
                                  | tCIDLib::EVolumeFlags::CaseSensitive);

        // If the device is a symbolic link, then we'd better follow it.
        tCIDLib::TSCh szDeviceName[kCIDLib::c4MaxPathLen + 1];
        tCIDLib::TSInt iCopied = ::readlink(MountEntry.szDevice, szDeviceName, kCIDLib::c4MaxPathLen);
        if (iCopied == -1)
        {
            TRawStr::CopyStr(szDeviceName, MountEntry.szDevice);
        }
        else
        {
            szDeviceName[iCopied] = '\000';
        }

        // In some cases we can make an educated guess about the hardware
        eHWType = tCIDLib::EVolHWTypes::Unknown;

        // The first guess is based on the device name, so we need to
        // extract the name part of the path.
        tCIDLib::TSCh* pszName = ::strrchr(szDeviceName, '\\');
        if (pszName && ::strlen(pszName) > 1)
        {
            pszName++;
            if (::strstr(pszName, "fd") == pszName)
                eHWType = tCIDLib::EVolHWTypes::Removable;
            else if (::strstr(pszName, "ram") == pszName)
                eHWType = tCIDLib::EVolHWTypes::RAMDisk;
        }

        // The next guess is based on the type
        // If it's an ISO9660 file system, then it's probably
        // a CD.
        if (StatBuf.f_type == 0x9660)
        {
            eHWType = tCIDLib::EVolHWTypes::CD;
        }
        // If it's NFS, SMB or NCP, then it's network
        else if (StatBuf.f_type == 0x6969
             ||  StatBuf.f_type == 0x517B
             ||  StatBuf.f_type == 0x564C)
        {
            eHWType = tCIDLib::EVolHWTypes::Remote;
        }

        // If it's ext2, then we can get the label, otherwise not.
        // PS - This only works if you have read permission to the
        // device. In other words, it only works if you're root.
        tCIDLib::TSCh szLabel[c4Ext2VolNameLen + 1];
        if (StatBuf.f_type == 0xEF53)
            GetExt2Label(szDeviceName, szLabel, c4Ext2VolNameLen);
        else
            szLabel[0] = '\000';

        tCIDLib::TCh* pszConvertedType = TRawStr::pszConvert(MountEntry.szType);
        tCIDLib::TCh* pszConvertedPoint = TRawStr::pszConvert(MountEntry.szMountPoint);
        tCIDLib::TCh* pszConvertedLabel = TRawStr::pszConvert(szLabel);

        kvoliToFill.Set(f8TotalBytes
                        , f8AvailableBytes
                        , c4MaxPathCompLen
                        , eFlags
                        , eHWType
                        , pszConvertedType
                        , pszConvertedPoint
                        , pszConvertedLabel
                        , L"");

        delete [] pszConvertedLabel;
        delete [] pszConvertedPoint;
        delete [] pszConvertedType;

        return kCIDLib::True;
        */
       return kCIDLib::False;
    }


    tCIDLib::TVoid
    FillFindBuf(const           tCIDLib::TSCh* const        pszFullName
                , const struct  stat&                       StatBuf
                ,               TKrnlFileSys::TRawFileFind& fndbToFill)
    {
        TRawStr::pszConvert(pszFullName
                            , fndbToFill.szName
                            , c4MaxBufChars(fndbToFill.szName));

        TKrnlLinux::LinuxFileTimeToCIDFileTime(StatBuf.st_atime, fndbToFill.enctLastAccessTime);
        TKrnlLinux::LinuxFileTimeToCIDFileTime(StatBuf.st_mtime, fndbToFill.enctLastWriteTime);

        fndbToFill.c8FileSize = StatBuf.st_size;

        TKrnlLinux::StatBufToInfoFlags(StatBuf, fndbToFill.eInfoFlags);
    }

    //
    //  This method will check a dirent buffer to see if the thing
    //  found meets the search criteria in the passed search flags.
    //
    tCIDLib::TBoolean
    bCheckSearchCriteria(const  tCIDLib::EDirSearchFlags  eFlags
                        , const TDirSearchHandleImpl&     hdiriToCheck
                        , const struct dirent&            DirEntry
                        ,       tCIDLib::TSCh*            pszNameToFill
                        ,       struct stat&              StatToFill)
    {
        // If the stinking d_type field of the dirent structure were
        // actually used, we wouldn't need all this stuff!

        // We can make a quick decision on special dirs
        if (!::strcmp(DirEntry.d_name, ".")
        ||  !::strcmp(DirEntry.d_name, ".."))
        {
            if (!tCIDLib::bAllBitsOn(eFlags, tCIDLib::EDirSearchFlags::FindDirs)
            ||  tCIDLib::bAllBitsOn(eFlags, tCIDLib::EDirSearchFlags::NormalDirsOnly))
            {
                return kCIDLib::False;
            }
        }

        // If the file name doesn't match, there's no point in doing
        // the other stuff
        if (::fnmatch(hdiriToCheck.pszFileSpec, DirEntry.d_name, 0))
            return kCIDLib::False;

        // Make the full name
        ::strcpy(pszNameToFill, hdiriToCheck.pszDirName);
        ::strcat(pszNameToFill, DirEntry.d_name);

        // Get the info
        if (::stat(pszNameToFill, &StatToFill))
            return kCIDLib::False;

        tCIDLib::TBoolean bMatches = kCIDLib::False;

        if (S_ISDIR(StatToFill.st_mode)
        &&  tCIDLib::bAllBitsOn(eFlags, tCIDLib::EDirSearchFlags::FindDirs))
        {
            bMatches = kCIDLib::True;
        }
        else if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::EDirSearchFlags::FindFiles))
        {
            if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::EDirSearchFlags::NormalFilesOnly))
            {
                if (S_ISREG(StatToFill.st_mode))
                    bMatches = kCIDLib::True;
            }
            else
            {
                bMatches = kCIDLib::True;
            }
        }

        return bMatches;
    }
}


// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::bInitTermFileSys(const tCIDLib::EInitTerm eState)
{
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
    }
    else
    {
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

    m_phdiriThis(nullptr)
{
    m_phdiriThis = new TDirSearchHandleImpl;
    m_phdiriThis->pDir = 0;
    m_phdiriThis->pNextDirEntry = 0;
    m_phdiriThis->pszDirName = 0;
    m_phdiriThis->pszFileSpec = 0;
}

TDirSearchHandle::TDirSearchHandle(const TDirSearchHandle& hdirToCopy) :
    m_phdiriThis(0)
{
    m_phdiriThis = new TDirSearchHandleImpl;
    m_phdiriThis->pDir = hdirToCopy.m_phdiriThis->pDir;
    m_phdiriThis->pNextDirEntry = hdirToCopy.m_phdiriThis->pNextDirEntry;
    m_phdiriThis->pszDirName =
        new tCIDLib::TSCh[::strlen(hdirToCopy.m_phdiriThis->pszDirName) + 1];
    ::strcpy(m_phdiriThis->pszDirName, hdirToCopy.m_phdiriThis->pszDirName);
    m_phdiriThis->pszFileSpec =
        new tCIDLib::TSCh[::strlen(hdirToCopy.m_phdiriThis->pszFileSpec) + 1];
    ::strcpy(m_phdiriThis->pszFileSpec, hdirToCopy.m_phdiriThis->pszFileSpec);
}

TDirSearchHandle::~TDirSearchHandle()
{
    delete [] m_phdiriThis->pszFileSpec;
    delete [] m_phdiriThis->pszDirName;
    delete m_phdiriThis;
    m_phdiriThis = 0;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TDirSearchHandle&
TDirSearchHandle::operator=(const TDirSearchHandle& hdirToAssign)
{
    if (this == &hdirToAssign)
        return *this;

    m_phdiriThis->pDir = hdirToAssign.m_phdiriThis->pDir;
    m_phdiriThis->pNextDirEntry = hdirToAssign.m_phdiriThis->pNextDirEntry;
    delete [] m_phdiriThis->pszDirName;
    m_phdiriThis->pszDirName
        = new tCIDLib::TSCh[::strlen(hdirToAssign.m_phdiriThis->pszDirName) + 1];
    ::strcpy(m_phdiriThis->pszDirName, hdirToAssign.m_phdiriThis->pszDirName);
    delete [] m_phdiriThis->pszFileSpec;
    m_phdiriThis->pszFileSpec
        = new tCIDLib::TSCh[::strlen(hdirToAssign.m_phdiriThis->pszFileSpec) + 1];
    ::strcpy(m_phdiriThis->pszFileSpec, hdirToAssign.m_phdiriThis->pszFileSpec);

    return *this;
}


tCIDLib::TBoolean
TDirSearchHandle::operator==(const TDirSearchHandle& hdirToCompare) const
{
    return (m_phdiriThis->pDir == hdirToCompare.m_phdiriThis->pDir
            && m_phdiriThis->pNextDirEntry == hdirToCompare.m_phdiriThis->pNextDirEntry
            && !::strcmp(m_phdiriThis->pszDirName
                         , hdirToCompare.m_phdiriThis->pszDirName)
            && !::strcmp(m_phdiriThis->pszFileSpec
                         , hdirToCompare.m_phdiriThis->pszFileSpec));
}



// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TDirSearchHandle::bIsValid() const
{
    return (m_phdiriThis->pDir && m_phdiriThis->pszDirName && m_phdiriThis->pszFileSpec);
}

tCIDLib::TVoid TDirSearchHandle::Clear()
{
    m_phdiriThis->pDir = 0;
    m_phdiriThis->pNextDirEntry = 0;
    delete [] m_phdiriThis->pszDirName;
    m_phdiriThis->pszDirName = 0;
    delete [] m_phdiriThis->pszFileSpec;
    m_phdiriThis->pszFileSpec = 0;
}

tCIDLib::TVoid
TDirSearchHandle::FormatToStr(          tCIDLib::TCh* const pszToFill
                                , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal(tCIDLib::TCard4(m_phdiriThis->pDir)
                        , pszToFill
                        , c4MaxChars
                        , tCIDLib::ERadices::Hex);

    tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszToFill);
    if (c4Len < c4MaxChars)
    {
        TRawStr::CatStr(pszToFill, L":", c4MaxChars - c4Len);
        c4Len++;
    }

    tCIDLib::TCh szTmp[kCIDLib::c4MaxPathLen + 1];
    TRawStr::pszConvert(m_phdiriThis->pszDirName, szTmp, c4MaxBufChars(szTmp));
    tCIDLib::TCard4 c4DirLen = TRawStr::c4StrLen(szTmp);
    TRawStr::pszConvert(m_phdiriThis->pszFileSpec, &szTmp[c4DirLen], c4MaxBufChars(szTmp) - c4DirLen);
    c4Len += TRawStr::c4StrLen(szTmp);

    if (c4Len < c4MaxChars)
    {
        TRawStr::CopyStr(pszToFill + c4Len, szTmp, c4MaxChars - c4Len);
    }
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
            , kmodCIDKernel.pszLoadCIDFacMsg(kKrnlErrs::errcGen_CloseHandle)
        );
        #endif
    }
}


// ---------------------------------------------------------------------------
//  TKrnlDirSearch: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlDirSearch::bClose()
{
    if (m_hdirSearch.m_phdiriThis->pDir)
    {
        if (::closedir(m_hdirSearch.m_phdiriThis->pDir))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }
    }
    m_hdirSearch.Clear();
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlDirSearch::bFindFirst( const   tCIDLib::TCh* const         pszToSearch
                            ,       TKrnlFileSys::TRawFileFind& fndbToFill
                            ,       tCIDLib::TBoolean&          bFound
                            , const tCIDLib::EDirSearchFlags    eFlags)
{
    // <TBD>
    /*
    // If there is an existing search, then close it
    if (!m_hdirSearch.m_phdiriThis->pDir)
    {
        if (!bClose())
            return kCIDLib::False;
    }

    // Make a copy of the input string so we can modify it
    tCIDLib::TCh* pszCopy = new tCIDLib::TCh[TRawStr::c4StrLen(pszToSearch) + 1];
    TArrayJanitor<tCIDLib::TCh> janCopy(pszCopy);
    TRawStr::CopyStr(pszCopy, pszToSearch);

    // Get its length
    tCIDLib::TCard4 c4ToSearchLen = TRawStr::c4StrLen(pszCopy);
    if (pszCopy[0] == kCIDLib::chPathSep && c4ToSearchLen == 1)
    {
        // We have to have at least one other character if there's a
        // path separator
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidName);
        return kCIDLib::False;
    }

    // If it ends with a path separator, then cap it
    if (pszCopy[c4ToSearchLen - 1] == kCIDLib::chPathSep)
        pszCopy[c4ToSearchLen - 1] = L'\000';

    // Find where we're going to chop the string in half
    tCIDLib::TCh* pszLastPathSep = TRawStr::pszFindLastChar(pszCopy
                                                            , kCIDLib::chPathSep);
    if (!pszLastPathSep)
    {
        // If there's no path separator, then we're looking for files in the
        // current directory
        m_hdirSearch.m_phdiriThis->pszDirName = new tCIDLib::TSCh[::strlen("./") + 1];
        ::strcpy(m_hdirSearch.m_phdiriThis->pszDirName, "./");
        m_hdirSearch.m_phdiriThis->pszFileSpec = TRawStr::pszConvert(pszCopy);
    }
    else
    {
        // Chop in half and convert
        // This method of doing the conversion saves the headache of trying
        // to figure out how many multi-byte characters are in each part of
        // the two strings.
        tCIDLib::TCard4 c4PathSepIdx = pszLastPathSep - pszCopy;
        pszCopy[c4PathSepIdx] = '\000';
        m_hdirSearch.m_phdiriThis->pszDirName = TRawStr::pszConvert(pszCopy);
        ::strcat(m_hdirSearch.m_phdiriThis->pszDirName, "/");
        m_hdirSearch.m_phdiriThis->pszFileSpec = TRawStr::pszConvert(&pszCopy[c4PathSepIdx + 1]);
    }

    DIR* pDirTmp = ::opendir(m_hdirSearch.m_phdiriThis->pszDirName);
    if (!pDirTmp)
    {
        m_hdirSearch.Clear();
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    m_hdirSearch.m_phdiriThis->pDir = pDirTmp;
    m_eSearchFlags = eFlags;

    bFound = kCIDLib::False;
    struct dirent DirEntry;
    struct stat StatBuf;
    tCIDLib::TSCh szFullName[kCIDLib::c4MaxPathLen + 1];

    ::readdir_r
    (
        m_hdirSearch.m_phdiriThis->pDir
        , &DirEntry
        , &m_hdirSearch.m_phdiriThis->pNextDirEntry
    );
    while (!bFound && m_hdirSearch.m_phdiriThis->pNextDirEntry)
    {
        bFound = CIDKernel_FileSys_Linux::bCheckSearchCriteria
        (
            m_eSearchFlags
            , *m_hdirSearch.m_phdiriThis
            , DirEntry
            , szFullName
            , StatBuf
        );

        if (!bFound)
            ::readdir_r(m_hdirSearch.m_phdiriThis->pDir
                        , &DirEntry
                        , &m_hdirSearch.m_phdiriThis->pNextDirEntry);
    }

    if (bFound)
    {
        CIDKernel_FileSys_Linux::FillFindBuf(szFullName, StatBuf, fndbToFill);
    }
    else
    {
        ::closedir(m_hdirSearch.m_phdiriThis->pDir);
        m_hdirSearch.Clear();
    }

    return kCIDLib::True;
    */

   // <TBD> 
   return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlDirSearch::bFindNext(  TKrnlFileSys::TRawFileFind& fndbToFill
                            , tCIDLib::TBoolean&        bFound)
{
    /*
    // If the find first was not done yet, then its not ready
    if (!m_hdirSearch.m_phdiriThis->pDir)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    bFound = kCIDLib::False;
    struct dirent DirEntry;
    struct stat StatBuf;
    tCIDLib::TSCh szFullName[kCIDLib::c4MaxPathLen + 1];

    while (!bFound && m_hdirSearch.m_phdiriThis->pNextDirEntry)
    {
        //
        //  Look for another file. If its just no more files, then break
        //  out; else, its an error so return false.
        //
        ::readdir_r(m_hdirSearch.m_phdiriThis->pDir
                    , &DirEntry
                    , &m_hdirSearch.m_phdiriThis->pNextDirEntry);

        //
        //  Check this one against the search criteria. If it matches, then
        //  we will break out and return this file. Otherwise, we'll keep
        //  looking.
        //
        bFound = CIDKernel_FileSys_Linux::bCheckSearchCriteria
        (
            m_eSearchFlags
            , *m_hdirSearch.m_phdiriThis
            , DirEntry
            , szFullName
            , StatBuf
        );
    }

    // Convert the system structure to our structure, if we found one.
    if (bFound)
        CIDKernel_FileSys_Linux::FillFindBuf(szFullName, StatBuf, fndbToFill);

    return kCIDLib::True;
    */

   // <TBD>
   return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlDirSearch::bSearchIsOpen() const
{
    return (m_hdirSearch.m_phdiriThis->pDir != 0);
}




// ---------------------------------------------------------------------------
//  TKrnlFileSys functions
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlFileSys::bCopy(const  tCIDLib::TCh* const  pszSourceFile
                    , const tCIDLib::TCh* const pszTargetFile
                    , const tCIDLib::TBoolean   bFailIfExists)
{
    tCIDLib::TSCh* pszSource = TRawStr::pszConvert(pszSourceFile);
    TArrayJanitor<tCIDLib::TSCh> janSource(pszSource);
    tCIDLib::TSCh* pszTarget = TRawStr::pszConvert(pszTargetFile);
    TArrayJanitor<tCIDLib::TSCh> janTarget(pszTarget);

    if (!::access(pszTarget, F_OK) && bFailIfExists)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_AlreadyExists);
        return kCIDLib::False;
    }

    struct stat StatBuf;
    if (::stat(pszSource, &StatBuf))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    if (!S_ISREG(StatBuf.st_mode))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidFormat);
        return kCIDLib::False;
    }

    tCIDLib::TSInt iSrcFd = ::open(pszSource, O_RDONLY);
    if (iSrcFd == -1)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    mode_t mode = StatBuf.st_mode | S_IWUSR | S_IWGRP | S_IWOTH;

    tCIDLib::TSInt iTgtFd = ::open(pszTarget, O_CREAT | O_WRONLY | O_TRUNC,  mode);
    if (iTgtFd == -1)
    {
        ::close(iSrcFd);
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[StatBuf.st_blksize];
    ssize_t count = 0;

    do
    {
        count = ::read(iSrcFd, pc1Buf, StatBuf.st_blksize);
        if (count == -1)
            break;

        count = ::write(iTgtFd, pc1Buf, count);
        if (count == -1)
            break;
    } while (count);

    delete [] pc1Buf;
    ::close(iSrcFd);

    if (count == -1)
    {
        ::close(iTgtFd);
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    tCIDLib::TSInt iRc = 0;
    if (mode != StatBuf.st_mode)
    {
        iRc = ::fchmod(iTgtFd, StatBuf.st_mode);
    }

    ::close(iTgtFd);

    if (iRc)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bCreateTmpFileName(       tCIDLib::TCh* const pszToFillIn
                                , const tCIDLib::TCard4     c4BufChars
                                , const tCIDLib::TCard4     c4UniqueNum)
{
    // <TBD> Use the unique num?

    tCIDLib::TSCh* pszTmp = ::tempnam(0, 0);
    if (!pszTmp)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    if (::strlen(pszTmp) > c4BufChars)
    {
        ::free(pszTmp);
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
        return kCIDLib::False;
    }

    TRawStr::pszConvert(pszTmp, pszToFillIn, c4BufChars);

    ::free(pszTmp);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bCreateTmpFileNameIn( const   tCIDLib::TCh* const pszTarDir
                                    ,       tCIDLib::TCh* const pszToFillIn
                                    , const tCIDLib::TCard4     c4BufChars
                                    , const tCIDLib::TCard4     c4UniqueNum)
{
    // <TBD>
    return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlFileSys::bExists(  const   tCIDLib::TCh* const      pszToFind
                        ,       tCIDLib::TBoolean&       bResult
                        ,       tCIDLib::TBoolean&       bIsUnique
                        ,       tCIDLib::EDirSearchFlags eFlags)
{
    // Init the unique and result flags to defaults
    bIsUnique = kCIDLib::True;
    bResult = kCIDLib::False;

    TKrnlFileSys::TRawFileFind  fndbToFill;
    TKrnlDirSearch              kdsSearch;

    if (!kdsSearch.bFindFirst(pszToFind, fndbToFill, bResult, eFlags))
        return kCIDLib::False;

    if (!bResult)
        return kCIDLib::True;

    // See if its unique
    tCIDLib::TBoolean bFound;
    if (!kdsSearch.bFindNext(fndbToFill, bFound))
        return kCIDLib::False;

    if (bFound)
        bIsUnique = kCIDLib::False;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bExists(  const   tCIDLib::TCh* const         pszToFind
                        ,       tCIDLib::TBoolean&          bResult
                        ,       tCIDLib::EDirSearchFlags    eFlags)
{
    tCIDLib::TBoolean bDummy;
    return bExists(pszToFind, bResult, bDummy, eFlags);
}


tCIDLib::TBoolean
TKrnlFileSys::bFindInPath(  const   tCIDLib::TCh* const pszPath
                            , const tCIDLib::TCh* const pszSpec
                            ,       tCIDLib::TCh* const pszMatch
                            , const tCIDLib::TCard4     c4BufChars)
{

    if (!pszPath || !pszSpec)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidParameter);
        return kCIDLib::False;
    }

    tCIDLib::TSCh* pszLocPath = TRawStr::pszConvert(pszPath);
    TArrayJanitor<tCIDLib::TSCh> janPath(pszLocPath);
    tCIDLib::TSCh* pszLocSpec = TRawStr::pszConvert(pszSpec);
    TArrayJanitor<tCIDLib::TSCh> janSpec(pszLocSpec);
    tCIDLib::TSCh szCurFull[kCIDLib::c4MaxPathLen + 1];
    tCIDLib::TSCh* pszSaveToke;
    tCIDLib::TCard4 c4SpecLen = ::strlen(pszLocSpec);
    tCIDLib::TCard4 c4RequiredLen;
    tCIDLib::TSCh* pszCurPath = ::strtok_r(pszLocPath, ":", &pszSaveToke);

    while (pszCurPath)
    {
        c4RequiredLen = ::strlen(pszCurPath) + c4SpecLen;

        if (c4RequiredLen > kCIDLib::c4MaxPathLen)
            goto NextOne;

        ::strcpy(szCurFull, pszCurPath);
        if (szCurFull[::strlen(szCurFull) - 1] != '\\')
        {
            if (++c4RequiredLen > kCIDLib::c4MaxPathLen)
                goto NextOne;
            ::strcat(szCurFull, "\\");
        }
        ::strcat(szCurFull, pszLocSpec);

        if (!::access(szCurFull, F_OK))
        {
            TRawStr::pszConvert(szCurFull, pszMatch, c4BufChars);
            break;
        }

        NextOne:
        pszCurPath = ::strtok_r(0, ":", &pszSaveToke);
    }

    return (pszCurPath != 0);
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

    // And now call the other version with our path variable
    return bFindInPath(pszTmpPath, pszSpec, pszMatch, c4BufChars);
}


tCIDLib::TBoolean
TKrnlFileSys::bHasWildCards(const tCIDLib::TCh* const pszToSearch)
{
    static const tCIDLib::TCh* const pszWildCards = L"?*";

    // Search the path for any wild card characters for Linux
    return (TRawStr::pszFindChars(pszToSearch, pszWildCards, 0) != 0);
}


tCIDLib::TBoolean
TKrnlFileSys::bIsDirectory(const tCIDLib::TCh* const pszDirToCheck)
{
   // <TBD>
   return kCIDLib::False;        
}


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
   // <TBD>
   return kCIDLib::False;    
}


tCIDLib::TBoolean
TKrnlFileSys::bMakeDirectory(const  tCIDLib::TCh* const pszToCreate
                            , const tCIDLib::TBoolean   bErrIfExists
                            , const tCIDLib::TBoolean   bAllAccess)
{
    /*
    mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;

    if (pszTemplate)
    {
        tCIDLib::TSCh* pszLocTemplate = TRawStr::pszConvert(pszTemplate);
        TArrayJanitor<tCIDLib::TSCh> janTmpl(pszLocTemplate);

        struct stat StatBuf;
        if (::stat(pszLocTemplate, &StatBuf))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }
        mode = StatBuf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
    }

    tCIDLib::TSCh* pszLocToCreate = TRawStr::pszConvert(pszToCreate);
    TArrayJanitor<tCIDLib::TSCh> janCreate(pszLocToCreate);

    if (::mkdir(pszLocToCreate, mode))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    return kCIDLib::True;
    */

   // <TBD>
   return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlFileSys::bMakePath(const tCIDLib::TCh* const pszToCreate)
{
    // <TBD>
    return kCIDLib::False;        
}


tCIDLib::TBoolean
TKrnlFileSys::bNormalizePath(const  tCIDLib::TCh* const pszToNormalize
                            ,       tCIDLib::TCh* const pszResult
                            , const tCIDLib::TCard4     c4MaxChars)
{
    const char* pszNorm = TRawStr::pszConvert(pszToNormalize);
    TArrayJanitor<const char> janNorm(pszNorm);

    char szTmp[PATH_MAX + 1];
    if (!::realpath(pszNorm, szTmp))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;    
    }
    TRawStr::pszConvert(szTmp, pszResult, c4MaxChars);
    return kCIDLib::True;
}                            



tCIDLib::TBoolean
TKrnlFileSys::bQueryCurrentDir( tCIDLib::TCh* const     pszToFillIn
                                , const tCIDLib::TCard4 c4BufChars)
{
    tCIDLib::TSCh szTmp[kCIDLib::c4MaxPathLen + 1];

    if (!::getcwd(szTmp, kCIDLib::c4MaxPathLen))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    TRawStr::pszConvert(szTmp, pszToFillIn, c4BufChars);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bQueryFullPath(const  tCIDLib::TCh* const pszPartialPath
                            ,       tCIDLib::TCh* const pszToFillIn
                            , const tCIDLib::TCard4     c4BufChars)
{
    if (pszPartialPath[0] == kCIDLib::chPathSep)
    {
        if (c4BufChars < TRawStr::c4StrLen(pszPartialPath))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
            return kCIDLib::False;
        }
        TRawStr::CopyStr(pszToFillIn, pszPartialPath, c4BufChars);
    }
    else
    {
        tCIDLib::TSCh szTmp[kCIDLib::c4MaxPathLen + 1];

        if (!::getcwd(szTmp, kCIDLib::c4MaxPathLen))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        if (szTmp[::strlen(szTmp) - 1] != '/')
            ::strcat(szTmp, "/");

        if (c4BufChars < (::strlen(szTmp) + TRawStr::c4StrLen(pszPartialPath)))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
            return kCIDLib::False;
        }

        TRawStr::pszConvert(szTmp, pszToFillIn, c4BufChars);
        TRawStr::CatStr(pszToFillIn, pszPartialPath, c4BufChars);
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bQueryMountInfo(  const   tCIDLib::TCh* const pszVolume
                                ,       tCIDLib::TCh* const pszHostToFill
                                , const tCIDLib::TCard4     c4MaxHostChars
                                ,       tCIDLib::TCh* const pszMountInfo
                                , const tCIDLib::TCard4     c4MaxInfoChars)
{
    // <TBD>
    return kCIDLib::False;    
}


tCIDLib::TBoolean
TKrnlFileSys::bQueryTmpPath(        tCIDLib::TCh* const pszToFillIn
                            , const tCIDLib::TCard4     c4BufChars)
{
    const tCIDLib::TSCh* pszTmpDir = ::getenv("TMPDIR");

    // First try the environment variable
    if (!pszTmpDir || ::access(pszTmpDir, W_OK))
    {
        // Next try the C library
        pszTmpDir = P_tmpdir;
        if (::access(pszTmpDir, W_OK))
        {
            // Ok, now we're in crazy mode! Just hard code it.
            pszTmpDir = "/tmp";
        }
    }

    if (::strlen(pszTmpDir) > c4BufChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
        return kCIDLib::False;
    }

    TRawStr::pszConvert(pszTmpDir, pszToFillIn, c4BufChars);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bQueryVolumeInfo( const   tCIDLib::TCh* const pszSrcPath
                                ,       tCIDLib::TCard8&    c8VolTotalBytes
                                ,       tCIDLib::TCard8&    c8VolFreeBytes
                                ,       tCIDLib::TCard8&    c8UserAvailBytes)
{
    // <TBD>
    return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlFileSys::bQueryVolumeInfo( const   tCIDLib::TCh* const pszSrcPath
                                ,       TKrnlVolumeInfo&    kvoliToFill)
{
    tCIDLib::TSCh* pszLocPath = TRawStr::pszConvert(pszSrcPath);
    TArrayJanitor<tCIDLib::TSCh> janPath(pszLocPath);

    CIDKernel_FileSys_Linux::TMountEntry MountEntry;
    if (!CIDKernel_FileSys_Linux::GetMountEntry(pszLocPath, MountEntry))
        return false;


    tCIDLib::EVolHWTypes eHWType;
    return CIDKernel_FileSys_Linux::bQueryVolumeInfo
    (
        pszLocPath, MountEntry, kvoliToFill, eHWType
    );
}


tCIDLib::TBoolean TKrnlFileSys
::bQueryVolumeList(         TKrnlLList<TKrnlVolumeInfo>&        kllistGood
                    ,       tCIDLib::TCard4&                    c4VolCount
                    ,       TKrnlLList<TKrnlVolFailureInfo>&    kllistBad
                    ,       tCIDLib::TCard4&                    c4ErrCount
                    , const tCIDLib::TBoolean                   bIncludeShares
                    , const tCIDLib::TBoolean                   bForceUpdate)
{
    // First get the number of mounted volumes
    FILE* pFile = ::setmntent(MOUNTED, "r");
    if (!pFile)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    tCIDLib::TCard4 c4MountCount = 0;
    struct mntent Context;
    tCIDLib::TSCh szMountBuffer[CIDKernel_FileSys_Linux::c4MountBufferSize];
    struct mntent* pEntry = ::getmntent_r
    (
        pFile, &Context, szMountBuffer, CIDKernel_FileSys_Linux::c4MountBufferSize
    );
    while (pEntry)
    {
        c4MountCount++;
        pEntry = ::getmntent_r(pFile, &Context, szMountBuffer, CIDKernel_FileSys_Linux::c4MountBufferSize);
    }

    ::endmntent(pFile);

    if (!c4MountCount)
    {
        TKrnlError::SetLastError(kKrnlErrs::errcGen_NoData);
        return kCIDLib::False;
    }

    CIDKernel_FileSys_Linux::TMountEntry MountEntry;
    pFile = ::setmntent(MOUNTED, "r");
    if (!pFile)
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }

    TKrnlVolumeInfo voliCur;
    TKrnlVolFailureInfo volfiCur;
    tCIDLib::EVolHWTypes eHWType;
    pEntry = ::getmntent_r(pFile, &Context, szMountBuffer, CIDKernel_FileSys_Linux::c4MountBufferSize);
    while (pEntry)
    {
        TRawStr::CopyStr(MountEntry.szDevice
                         , pEntry->mnt_fsname
                         , sizeof(MountEntry.szDevice));
        TRawStr::CopyStr(MountEntry.szMountPoint
                         , pEntry->mnt_dir
                         , sizeof(MountEntry.szMountPoint));
        TRawStr::CopyStr(MountEntry.szType
                         , pEntry->mnt_type
                         , sizeof(MountEntry.szType));
        
        if (CIDKernel_FileSys_Linux::bQueryVolumeInfo(MountEntry.szMountPoint
                                                    , MountEntry
                                                    , voliCur
                                                    , eHWType))
        {
            kllistGood.pobjAddNew(new TKrnlVolumeInfo(voliCur));
        }
         else
        {
            kllistBad.pobjAddNew
            (
                new TKrnlVolFailureInfo
                (
                    TRawStr::pszConvert(MountEntry.szMountPoint), eHWType, TKrnlError::kerrLast()
                )
            );
        }
        pEntry = ::getmntent_r
        (
            pFile, &Context, szMountBuffer, CIDKernel_FileSys_Linux::c4MountBufferSize
        );
    }

    ::endmntent(pFile);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bQueryVolHWType( const   tCIDLib::TCh* const     pszVolPath
                               ,       tCIDLib::EVolHWTypes&   eType)
{
    // <TBD>
    return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlFileSys::bRemoveDir(const tCIDLib::TCh* const pszToDelete)
{
    tCIDLib::TSCh* pszLocToDelete = TRawStr::pszConvert(pszToDelete);
    TArrayJanitor<tCIDLib::TSCh> janDelete(pszLocToDelete);

    if (::rmdir(pszLocToDelete))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bRemoveFile(const tCIDLib::TCh* const pszToDelete)
{
    tCIDLib::TSCh* pszLocToDelete = TRawStr::pszConvert(pszToDelete);
    TArrayJanitor<tCIDLib::TSCh> janDelete(pszLocToDelete);

    if (::unlink(pszLocToDelete))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bRename(  const   tCIDLib::TCh* const pszOldName
                        , const tCIDLib::TCh* const pszNewName)
{
    tCIDLib::TSCh* pszLocOld = TRawStr::pszConvert(pszOldName);
    TArrayJanitor<tCIDLib::TSCh> janOld(pszLocOld);
    tCIDLib::TSCh* pszLocNew = TRawStr::pszConvert(pszNewName);
    TArrayJanitor<tCIDLib::TSCh> janNew(pszLocNew);

    if (::rename(pszLocOld, pszLocNew))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlFileSys::bReplaceFile( const   tCIDLib::TCh* const pszToReplace
                            , const tCIDLib::TCh* const pszNewFile
                            , const tCIDLib::TCh* const pszBackupName)
{
    // <TBD>
    return kCIDLib::False;    
}                            


tCIDLib::TBoolean
TKrnlFileSys::bSetCurrentDir(const tCIDLib::TCh* const pszNewDir)
{
    // <TBD>
    return kCIDLib::False;    
}


tCIDLib::TBoolean
TKrnlFileSys::bSetPerms(const   tCIDLib::TCh* const pszTarget
                        , const tCIDLib::EFilePerms ePerms)
{
    // <TBD>
    return kCIDLib::False;
}


tCIDLib::TCard4 TKrnlFileSys::c4FSRetryCount()
{
    // <TBD>
    static tCIDLib::TCard4 c4Num = 1;

    return c4Num++;
}


tCIDLib::TCard4 TKrnlFileSys::c4VolListSerialNum()
{
    // <TBD>
    static tCIDLib::TCard4 c4Num = 1;

    return c4Num++;
}


tCIDLib::EConnRes
TKrnlFileSys::eAccessRemoteShare(const  tCIDLib::TCh* const pszTarShare
                                , const tCIDLib::TCh* const pszUserName
                                , const tCIDLib::TCh* const pszPassword
                                , const tCIDLib::TBoolean   bTemp)
{
    return tCIDLib::EConnRes::Failed;
}
