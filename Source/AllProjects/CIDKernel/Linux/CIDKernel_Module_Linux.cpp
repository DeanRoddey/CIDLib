//
// FILE NAME: CIDKernel_Module_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/18/1998
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
//  This file provides the Linux specific implementation of the the class
//  TKrnlModule class.
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


namespace CIDKernel_Module_Linux
{
    const tCIDLib::TCh* const       szLibraryPrefix = L"lib";


    // ---------------------------------------------------------------------------
    //  Local Functions
    // ---------------------------------------------------------------------------
    tCIDLib::TVoid BuildModName(        tCIDLib::TCh* const     pszNameBuf
                                , const tCIDLib::TCard4         c4MaxChars
                                , const tCIDLib::TCh* const     pszModName
                                , const tCIDLib::TCard4         c4MajVer
                                , const tCIDLib::TCard4         c4MinVer)
    {
        TRawStr::CopyCatStr
        (
            pszNameBuf, c4MaxChars
            , szLibraryPrefix
            , pszModName
            , kCIDLib::szLibExtension
        );
        const tCIDLib::TCard4 c4ValBufSize = 256;
        tCIDLib::TCh szValBuf[c4ValBufSize + 1];
        TRawStr::CatStr(pszNameBuf, L".", c4MaxChars);
        TRawStr::bFormatVal(c4MajVer, szValBuf, c4ValBufSize);
        TRawStr::CatStr(pszNameBuf, szValBuf, c4MaxChars);
        TRawStr::CatStr(pszNameBuf, L".", c4MaxChars);
        TRawStr::bFormatVal(c4MinVer, szValBuf, c4ValBufSize);
        TRawStr::CatStr(pszNameBuf, szValBuf, c4MaxChars);
    }

    tCIDLib::TBoolean bCheckAndResolvePath( const   tCIDLib::TSCh* const    pszPath
                                            ,       tCIDLib::TCh* const     pszToFill
                                            , const tCIDLib::TCard4         c4MaxChars)
    {
        if (!::access(pszPath, X_OK))
        {
            tCIDLib::TSCh szRealPath[kCIDLib::c4MaxPathLen + 1];
            if (::realpath(pszPath, szRealPath))
            {
                TRawStr::pszConvert(szRealPath, pszToFill, c4MaxChars);
                TKrnlError::SetLastKrnlError(kKrnlErrs::errcNoError);
                return kCIDLib::True;
            }
            TKrnlError::SetLastHostError(errno);
        }
        else
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_ModuleNotFound);
        }

        return kCIDLib::False;
    }

    tCIDLib::TBoolean bFindSharedLibrary(const  tCIDLib::TCh* const pszLib
                                        ,       tCIDLib::TCh* const pszToFill
                                        , const tCIDLib::TCard4     c4MaxChars)
    {
        tCIDLib::TSCh* pszLocLib = TRawStr::pszConvert(pszLib);
        TArrayJanitor<tCIDLib::TSCh> janLocLib(pszLocLib);

        if (pszLib[0] == kCIDLib::chPathSep)
        {
            return bCheckAndResolvePath(pszLocLib, pszToFill, c4MaxChars);
        }

        tCIDLib::TSCh szCurDir[kCIDLib::c4MaxPathLen + 1];
        tCIDLib::TCard4 c4LibLen = ::strlen(pszLocLib);
        tCIDLib::TCard4 c4CurLen;
        tCIDLib::TCard4 c4Required;

        tCIDLib::TSCh* pszLibPath = ::getenv("LD_LIBRARY_PATH");
        if (pszLibPath)
        {
            tCIDLib::TSCh* pszLibsCpy = new tCIDLib::TSCh[::strlen(pszLibPath) + 1];
            ::strcpy(pszLibsCpy, pszLibPath);
            TArrayJanitor<tCIDLib::TSCh> janLibs(pszLibsCpy);
            tCIDLib::TSCh* pszSaveToke;

            tCIDLib::TSCh* pszCurPath = ::strtok_r(pszLibsCpy, ":", &pszSaveToke);
            while (pszCurPath)
            {
                c4CurLen = ::strlen(pszCurPath);
                c4Required = c4CurLen + c4LibLen;
                if (c4Required > kCIDLib::c4MaxPathLen)
                    goto NextOne;

                ::strcpy(szCurDir, pszCurPath);
                if (szCurDir[c4CurLen - 1] != '\\')
                {
                    if (++c4Required > kCIDLib::c4MaxPathLen)
                        goto NextOne;
                    ::strcat(szCurDir, "\\");
                }
                ::strcat(szCurDir, pszLocLib);

                if (CIDKernel_Module_Linux::bCheckAndResolvePath(szCurDir, pszToFill, c4MaxChars))
                    return kCIDLib::True;

                NextOne:
                pszCurPath = ::strtok_r(0, ":", &pszSaveToke);
            }
        }

        FILE* SoConf = ::fopen("/etc/ld.so.conf", "r");
        if (SoConf)
        {
            while (!::feof(SoConf))
            {
                if (::fgets(szCurDir, sizeof(szCurDir), SoConf))
                {
                    tCIDLib::TSCh* pszEOL = ::strpbrk(szCurDir, "\r\n");
                    if (pszEOL)
                        *pszEOL = 0;

                    c4CurLen = ::strlen(szCurDir);
                    c4Required = c4CurLen + c4LibLen;
                    if (c4Required > kCIDLib::c4MaxPathLen)
                        continue;

                    if (szCurDir[::strlen(szCurDir) - 1] != '\\')
                    {
                        if (++c4Required > kCIDLib::c4MaxPathLen)
                            continue;
                        ::strcat(szCurDir, "\\");
                    }
                    ::strcat(szCurDir, pszLocLib);

                    if (CIDKernel_Module_Linux::bCheckAndResolvePath(szCurDir, pszToFill, c4MaxChars))
                    {
                        ::fclose(SoConf);
                        return kCIDLib::True;
                    }
                }
                else
                {
                    break;
                }
            }
            ::fclose(SoConf);
        }

        ::strcpy(szCurDir, "/usr/lib/");
        if (::strlen(szCurDir) + c4LibLen <= kCIDLib::c4MaxPathLen)
        {
            ::strcat(szCurDir, pszLocLib);
            if (CIDKernel_Module_Linux::bCheckAndResolvePath(szCurDir, pszToFill, c4MaxChars))
                return kCIDLib::True;
        }

        // No need to check the length here, because we know
        // it's shorter than the last one!
        ::strcpy(szCurDir, "\\lib\\");
        ::strcat(szCurDir, pszLocLib);
        if (CIDKernel_Module_Linux::bCheckAndResolvePath(szCurDir, pszToFill, c4MaxChars))
            return kCIDLib::True;

        return kCIDLib::False;
    }

    tCIDLib::TBoolean bIsLibLoaded(const tCIDLib::TCh* const pszLibName, tCIDLib::TBoolean& bLoaded)
    {
        bLoaded = kCIDLib::False;

        tCIDLib::TSCh* pszLocLib = TRawStr::pszConvert(pszLibName);
        TArrayJanitor<tCIDLib::TSCh> janLibName(pszLocLib);

        struct stat StatBuf;
        if (::stat(pszLocLib, &StatBuf))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        FILE* Maps = ::fopen("/proc/self/maps", "r");
        if (!Maps)
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        tCIDLib::TCard4 c4DevMinor, c4DevMajor;
        tCIDLib::TCard4 c4Inode;
        __dev_t devMapped;
        ::memset(&devMapped, 0, sizeof(devMapped));
        tCIDLib::TCard4* pc4Mapped =
            reinterpret_cast<tCIDLib::TCard4*>(&devMapped);

        const tCIDLib::TSInt iLineSize = 512;
        tCIDLib::TSCh szLine[iLineSize];

        // Ok, I use the combination of ::fgets and ::sscanf here rather
        // than ::fscanf alone because I got burned when they changed the
        // number of tokens in a line in the maps file. I hope that doesn't
        // happen again. The upshot is that you have to read it a line at
        // a time.
        while (::fgets(szLine, iLineSize, Maps))
        {
            if (::sscanf(szLine, " %*s %*s %*s %lu:%lu %lu"
                         , &c4DevMajor
                         , &c4DevMinor
                         , &c4Inode) != 3)
            {
                continue;
            }

            *pc4Mapped = (c4DevMajor << 8) | c4DevMinor;

            if (!::memcmp(&devMapped, &StatBuf.st_dev, sizeof(__dev_t))
            &&  c4Inode == StatBuf.st_ino)
            {
                bLoaded = kCIDLib::True;
                return kCIDLib::True;
            }
        }
        return kCIDLib::True;
    }

    tCIDLib::TBoolean bFindExecutable(  const   tCIDLib::TCh* const pszExe
                                        ,       tCIDLib::TCh* const pszToFill
                                        , const tCIDLib::TCard4     c4MaxChars)
    {
        tCIDLib::TSCh* pszLocExe = TRawStr::pszConvert(pszExe);
        TArrayJanitor<tCIDLib::TSCh> janExe(pszLocExe);

        // If it's an absolute path, then we're good (or bad...)
        if (pszExe[0] == kCIDLib::chPathSep)
        {
            return CIDKernel_Module_Linux::bCheckAndResolvePath(pszLocExe, pszToFill, c4MaxChars);
        }

        tCIDLib::TSCh szCurDir[kCIDLib::c4MaxPathLen + 1];
        tCIDLib::TCard4 c4Required;
        tCIDLib::TCard4 c4CurLen;
        tCIDLib::TCard4 c4ExeLen = ::strlen(pszLocExe);

        // First try the current directory
        if (::getcwd(szCurDir, sizeof(szCurDir)))
        {
            c4CurLen = ::strlen(szCurDir);
            c4Required = c4CurLen + c4ExeLen;

            if (szCurDir[c4CurLen - 1] != '/')
            {
                ::strcat(szCurDir, "/");
                c4Required++;
            }

            if (c4Required <= kCIDLib::c4MaxPathLen)
            {
                ::strcat(szCurDir, pszLocExe);
                if (CIDKernel_Module_Linux::bCheckAndResolvePath(szCurDir, pszToFill, c4MaxChars))
                    return kCIDLib::True;
            }
        }

        // Now search the path
        tCIDLib::TSCh* pszPath = ::getenv("PATH");
        if (pszPath)
        {
            tCIDLib::TSCh* pszPathCpy = new tCIDLib::TSCh[::strlen(pszPath) + 1];
            ::strcpy(pszPathCpy, pszPath);
            TArrayJanitor<tCIDLib::TSCh> janLibs(pszPathCpy);
            tCIDLib::TSCh* pszSaveToke;

            tCIDLib::TSCh* pszCurPath = ::strtok_r(pszPathCpy, ":", &pszSaveToke);
            while (pszCurPath)
            {
                c4CurLen = ::strlen(pszCurPath);
                c4Required = c4CurLen + c4ExeLen;
                if (c4Required > kCIDLib::c4MaxPathLen)
                    goto NextOne;

                ::strcpy(szCurDir, pszCurPath);
                if (szCurDir[c4CurLen - 1] != '\\')
                {
                    if (++c4Required > kCIDLib::c4MaxPathLen)
                        goto NextOne;
                    ::strcat(szCurDir, "\\");
                }
                ::strcat(szCurDir, pszLocExe);

                if (CIDKernel_Module_Linux::bCheckAndResolvePath(szCurDir, pszToFill, c4MaxChars))
                    return kCIDLib::True;

                NextOne:
                pszCurPath = ::strtok_r(0, ":", &pszSaveToke);
            }
        }

        return kCIDLib::False;
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TModuleHandle
//  PREFIX: hmod
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TModuleHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TModuleHandle::TModuleHandle() :

    m_phmodiThis(nullptr)
{
    m_phmodiThis = new TModuleHandleImpl;
    m_phmodiThis->pHandle = nullptr;
    m_phmodiThis->pszFullPath = nullptr;
}

TModuleHandle::TModuleHandle(const TModuleHandle& hmodToCopy) :

    m_phmodiThis(nullptr)
{
    m_phmodiThis = new TModuleHandleImpl;
    m_phmodiThis->pHandle = hmodToCopy.m_phmodiThis->pHandle;
    m_phmodiThis->pszFullPath = TRawStr::pszReplicate(hmodToCopy.m_phmodiThis->pszFullPath);
}

TModuleHandle::~TModuleHandle()
{
    delete [] m_phmodiThis->pszFullPath;
    delete m_phmodiThis;

    m_phmodiThis->pszFullPath = nullptr;
    m_phmodiThis = nullptr;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TModuleHandle& TModuleHandle::operator=(const TModuleHandle& hmodToAssign)
{
    if (this == &hmodToAssign)
        return *this;

    m_phmodiThis->pHandle = hmodToAssign.m_phmodiThis->pHandle;
    delete [] m_phmodiThis->pszFullPath;
    m_phmodiThis->pszFullPath = nullptr;
    m_phmodiThis->pszFullPath = TRawStr::pszReplicate(hmodToAssign.m_phmodiThis->pszFullPath);

    return *this;
}


tCIDLib::TBoolean
TModuleHandle::operator==(const TModuleHandle& hmodToCompare) const
{
    return 
    (
        (m_phmodiThis->pHandle == hmodToCompare.m_phmodiThis->pHandle)
        && TRawStr::bCompareStr(m_phmodiThis->pszFullPath, hmodToCompare.m_phmodiThis->pszFullPath)
    );
}



// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TModuleHandle::bIsValid() const
{
    return
    (
        (m_phmodiThis->pHandle && m_phmodiThis->pszFullPath)
        || (m_phmodiThis->pszFullPath)
    );
}


tCIDLib::TVoid TModuleHandle::Clear()
{
    m_phmodiThis->pHandle = nullptr;
    delete [] m_phmodiThis->pszFullPath;
    m_phmodiThis->pszFullPath = nullptr;
}

tCIDLib::TVoid
TModuleHandle::FormatToStr(         tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    if (!m_phmodiThis->pHandle)
    {
        TRawStr::CopyStr(pszToFill, m_phmodiThis->pszFullPath, c4MaxChars);
    }
     else
    {
        TRawStr::bFormatVal
        (
            tCIDLib::TCard4(m_phmodiThis->pHandle)
            , pszToFill
            , c4MaxChars
            , tCIDLib::ERadices::Hex
        );
    }
}




// ---------------------------------------------------------------------------
//   CLASS: TKrnlModule
//  PREFIX: kmod
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlModule: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlModule::bRawQueryModName(  const   TModuleHandle&      hmodSrc
                                ,       tCIDLib::TCh* const pszNameToFill
                                ,       tCIDLib::TCh* const pszPathToFill
                                , const tCIDLib::TCard4     c4MaxChars)
{
    if (!hmodSrc.m_phmodiThis->pszFullPath)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidHandle);
        return kCIDLib::False;
    }

    if (!(TKrnlPathStr::bQueryPath(hmodSrc.m_phmodiThis->pszFullPath, pszPathToFill, c4MaxChars)
          && TKrnlPathStr::bQueryNameExt(hmodSrc.m_phmodiThis->pszFullPath, pszNameToFill, c4MaxChars)))
    {
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlModule: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlModule::TKrnlModule() :

    m_bViaLoad(kCIDLib::False)
    , m_pmiThis(nullptr)
{
}

TKrnlModule::~TKrnlModule()
{
    // Delete the message file buffer if any
    delete [] reinterpret_cast<tCIDLib::TCh*>(m_pmiThis);
    m_pmiThis = 0;

    // Do the required handle cleanup stuff
    if (!bClearHandle())
    {
        //
        //  If we are debugging, then do a popup here. Otherwise there is
        //  not much we can do.
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
//  TKrnlModule: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlModule::bGetFuncPtr(const  tCIDLib::TSCh* const    pszFuncName
                        ,       tCIDLib::FuncPtr&       pfnToFill) const
{
    if (!m_hmodThis.m_phmodiThis->pHandle)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    pfnToFill = (tCIDLib::FuncPtr)::dlsym(m_hmodThis.m_phmodiThis->pHandle, pszFuncName);

    if (::dlerror() != 0)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_ProcNotFound);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}

tCIDLib::TBoolean
TKrnlModule::bLoadFromName( const   tCIDLib::TCh* const pszModName
                            , const tCIDLib::EModTypes  eModType
                            , const tCIDLib::TCard4     c4MajVer
                            , const tCIDLib::TCard4     c4MinVer
                            , const tCIDLib::EModFlags  eModFlags)
{
    if (eModType == tCIDLib::EModTypes::Exe)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidParameter);
        return kCIDLib::False;
    }

    // Clear the current handle if any
    if (!bClearHandle())
        return kCIDLib::False;

    // Now make a platform-specific name out of the module name
    tCIDLib::TCh szNameBuf[kCIDLib::c4MaxPathLen + 1];
    CIDKernel_Module_Linux::BuildModName(szNameBuf, kCIDLib::c4MaxPathLen, pszModName, c4MajVer, c4MinVer);

    //
    //  First find the library. dlopen would go through the same  steps, and since
    //  we need to get the full path name of it, it makes since to do it manually first.
    //
    tCIDLib::TCh szRealPath[kCIDLib::c4MaxPathLen + 1];
    if (!CIDKernel_Module_Linux::bFindSharedLibrary(szNameBuf, szRealPath, kCIDLib::c4MaxPathLen))
        return kCIDLib::False;

    tCIDLib::TSCh szRealCopy[kCIDLib::c4MaxPathLen + 1];
    TRawStr::pszConvert(szRealPath, szRealCopy, kCIDLib::c4MaxPathLen);

    // And now try to load the library
    tCIDLib::TVoid* pTmp = ::dlopen(szRealCopy, RTLD_LAZY);

    if (!pTmp)
    {
        if (::access(szRealCopy, F_OK))
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_ModuleNotFound);
        else if (::access(szRealCopy, R_OK))
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcAcc_InvalidAccess);
        else
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_DLLInitFailed);
        return kCIDLib::False;
    }

    //
    //  It worked, so store the handle. Set the flag that indicates this
    //  handle came from a load operation.
    //
    m_bViaLoad = kCIDLib::True;
    m_hmodThis.m_phmodiThis->pHandle = pTmp;

    delete [] m_hmodThis.m_phmodiThis->pszFullPath;
    m_hmodThis.m_phmodiThis->pszFullPath = TRawStr::pszReplicate(szRealPath);

    //
    //  See if the caller says this facility has a message file, and load it
    //  up if so. Delete the old one first, if any.
    //
    delete [] reinterpret_cast<tCIDLib::TCh*>(m_pmiThis);
    m_pmiThis = 0;

    if (tCIDLib::bAllBitsOn(eModFlags, tCIDLib::EModFlags::HasMsgFile))
    {
        if (!bLoadMessages(m_hmodThis, m_pmiThis))
            return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlModule::bQueryFromName(const   tCIDLib::TCh* const pszModName
                            , const tCIDLib::EModTypes  eModType
                            , const tCIDLib::TCard4     c4MajVer
                            , const tCIDLib::TCard4     c4MinVer
                            , const tCIDLib::EModFlags  eModFlags)
{
    if (!bClearHandle())
        return kCIDLib::False;

    // Now make a platform-specific name out of the module name
    tCIDLib::TCh szNameBuf[kCIDLib::c4MaxPathLen + 1];
    CIDKernel_Module_Linux::BuildModName(szNameBuf, kCIDLib::c4MaxPathLen, pszModName, c4MajVer, c4MinVer);

    tCIDLib::TCh szRealPath[kCIDLib::c4MaxPathLen + 1];

    if (eModType == tCIDLib::EModTypes::Dll)
    {
        if (!CIDKernel_Module_Linux::bFindSharedLibrary(szNameBuf, szRealPath, c4MaxBufChars(szRealPath)))
            return kCIDLib::False;

        tCIDLib::TBoolean bLoaded;
        if (!CIDKernel_Module_Linux::bIsLibLoaded(szRealPath, bLoaded))
            return kCIDLib::False;

        if (!bLoaded)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_ModuleNotFound);
            return kCIDLib::False;
        }

        // A reference count is maintained, so even though it's loaded already
        // calling dlopen() again will give us the handle.
        tCIDLib::TSCh szRealCopy[kCIDLib::c4MaxPathLen + 1];
        TRawStr::pszConvert(szRealPath, szRealCopy, kCIDLib::c4MaxPathLen);

        tCIDLib::TVoid* pTmp = ::dlopen(szRealCopy, RTLD_LAZY);

        if (!pTmp)
        {
            if (::access(szRealCopy, F_OK))
                TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_ModuleNotFound);
            else if (::access(szRealCopy, R_OK))
                TKrnlError::SetLastKrnlError(kKrnlErrs::errcAcc_InvalidAccess);
            else
                TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_DLLInitFailed);
            return kCIDLib::False;
        }

        m_hmodThis.m_phmodiThis->pHandle = pTmp;
    }
    else
    {
        if (!CIDKernel_Module_Linux::bFindExecutable(pszModName, szRealPath, c4MaxBufChars(szRealPath)))
            return kCIDLib::False;
        m_hmodThis.m_phmodiThis->pHandle = 0;
    }

    m_bViaLoad = kCIDLib::False;

    delete [] m_hmodThis.m_phmodiThis->pszFullPath;
    m_hmodThis.m_phmodiThis->pszFullPath = TRawStr::pszReplicate(szRealPath);

    //
    //  See if the caller says this facility has a message file, and load it
    //  up if so. Delete the old one first, if any.
    //
    delete [] reinterpret_cast<tCIDLib::TCh*>(m_pmiThis);
    m_pmiThis = 0;

    if (tCIDLib::bAllBitsOn(eModFlags, tCIDLib::EModFlags::HasMsgFile))
    {
        if (!bLoadMessages(m_hmodThis, m_pmiThis))
            return kCIDLib::False;
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlModule::bClearHandle()
{
    //
    //  If there is a module handle, it was via a load, and we have adopted
    //  it then we need to free it.
    //
    if (m_hmodThis.m_phmodiThis->pHandle)
    {
        if (::dlclose(m_hmodThis.m_phmodiThis->pHandle))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }
    }

    m_hmodThis.Clear();
    m_bViaLoad = kCIDLib::False;

    return kCIDLib::True;
}
