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
#include    "CIDKernel_InternalHelpers_.hpp"


namespace CIDKernel_Module_Linux
{
    const tCIDLib::TCh* const       szLibraryPrefix = L"lib";


    // ---------------------------------------------------------------------------
    //  Local Functions
    // ---------------------------------------------------------------------------
    tCIDLib::TBoolean bCheckAndResolvePath( const   tCIDLib::TSCh* const    pszPath
                                            ,       tCIDLib::TCh* const     pszToFill
                                            , const tCIDLib::TCard4         c4MaxChars)
    {
        if (::access(pszPath, X_OK) == 0)
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
                if (szCurDir[c4CurLen - 1] != '/')
                {
                    if (++c4Required > kCIDLib::c4MaxPathLen)
                        goto NextOne;
                    ::strcat(szCurDir, "/");
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
                if (szCurDir[c4CurLen - 1] != kCIDLib::chPathSep)
                {
                    if (++c4Required > kCIDLib::c4MaxPathLen)
                        goto NextOne;
                    ::strcat(szCurDir, "/");
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

//
//  Given the handle, get the module path and name. Since we are just querying
//  from the OS, the platform specific name is returned.
//
tCIDLib::TBoolean
TKrnlModule::bRawQueryModName(  const   TModuleHandle&  hmodSrc
                                ,       TKrnlString&    kstrName
                                ,       TKrnlString&    kstrPath)
{
    // Init the buffers to empty
    kstrName.Clear();
    kstrPath.Clear();

    if (!hmodSrc.m_phmodiThis->pszFullPath)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidHandle);
        return kCIDLib::False;
    }

    tCIDLib::TCh achPath[kCIDLib::c4MaxPathLen + 1];
    tCIDLib::TCh achName[kCIDLib::c4MaxPathLen + 1];
    if (!TKrnlPathStr::bQueryPath(hmodSrc.m_phmodiThis->pszFullPath, achPath, kCIDLib::c4MaxPathLen)
    ||  !TKrnlPathStr::bQueryNameExt(hmodSrc.m_phmodiThis->pszFullPath, achName, kCIDLib::c4MaxPathLen))
    {
        return kCIDLib::False;
    }

    kstrName = achName;
    kstrPath = achPath;
    return kCIDLib::True;
}



tCIDLib::TBoolean
TKrnlModule::bRawQueryModPath(  const   TModuleHandle&  hmodSrc
                                ,       TKrnlString&    kstrToFill)
{
    // Init the buffer to empty
    kstrToFill.Clear();

    if (!hmodSrc.m_phmodiThis->pszFullPath)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidHandle);
        return kCIDLib::False;
    }

    tCIDLib::TCh achPath[kCIDLib::c4MaxPathLen + 1];
    if (!TKrnlPathStr::bQueryPath(hmodSrc.m_phmodiThis->pszFullPath, achPath, kCIDLib::c4MaxPathLen))
        return kCIDLib::False;

    kstrToFill = achPath;
    return kCIDLib::True;
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


// ---------------------------------------------------------------------------
//  TKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlModule::bPlatCleanup()
{
    if (m_hmodThis.m_phmodiThis->pHandle)
    {
        if (::dlclose(m_hmodThis.m_phmodiThis->pHandle))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }

        m_hmodThis.m_phmodiThis->pHandle = nullptr;
        delete [] m_hmodThis.m_phmodiThis->pszFullPath;
        m_hmodThis.m_phmodiThis->pszFullPath = nullptr;

    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlModule::bLoadPlatByName()
{
    if (m_eModType == tCIDLib::EModTypes::Exe)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidParameter);
        return kCIDLib::False;
    }


    //
    //  First find the library. dlopen would go through the same  steps, and since
    //  we need to get the full path name of it, it makes since to do it manually first.
    //
    tCIDLib::TCh szRealPath[kCIDLib::c4MaxPathLen + 1];
    if (!CIDKernel_Module_Linux::bFindSharedLibrary(m_kstrLoadName.pszValue(), szRealPath, kCIDLib::c4MaxPathLen))
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

    // Store handle and path
    m_hmodThis.m_phmodiThis->pHandle = pTmp;

    tCIDLib::TCh szPathPart[kCIDLib::c4MaxPathLen + 1];
    TKrnlPathStr::bQueryPath(szRealPath, szPathPart, kCIDLib::c4MaxPathLen);
    m_kstrSrcPath = szPathPart;

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlModule::bQueryPlatByName()
{
    tCIDLib::TCh szRealPath[kCIDLib::c4MaxPathLen + 1];

    if (m_eModType == tCIDLib::EModTypes::SharedLib)
    {
        if (!CIDKernel_Module_Linux::bFindSharedLibrary(m_kstrLoadName.pszValue(), szRealPath, c4MaxBufChars(szRealPath)))
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
     else if (m_eModType == tCIDLib::EModTypes::Exe)
    {
        if (!CIDKernel_Module_Linux::bFindExecutable(m_kstrLoadName.pszValue(), szRealPath, c4MaxBufChars(szRealPath)))
            return kCIDLib::False;
        m_hmodThis.m_phmodiThis->pHandle = 0;
    }
    else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidParameter);
        return kCIDLib::False;        
    }
    

    tCIDLib::TCh szPathPart[kCIDLib::c4MaxPathLen + 1];
    TKrnlPathStr::bQueryPath(szRealPath, szPathPart, kCIDLib::c4MaxPathLen);
    m_kstrSrcPath = szPathPart;

    return kCIDLib::True;
}


//
//  This is for when we need to load a CIDLib facility, but from a specific path,
//  so something like a CQC device driver. We get the path that it's in. This
//  is after the names and type are set.
//
tCIDLib::TBoolean TKrnlModule::bLoadPlatByPath(const  tCIDLib::TCh* const pszLoadPath)
{
    if (m_eModType == tCIDLib::EModTypes::Exe)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidParameter);
        return kCIDLib::False;
    }

    tCIDLib::TCh szRealPath[kCIDLib::c4MaxPathLen + 1];
    if (!CIDKernel_Module_Linux::bFindSharedLibrary(pszLoadPath, szRealPath, c4MaxBufChars(szRealPath)))
        return kCIDLib::False;

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

    tCIDLib::TCh szPathPart[kCIDLib::c4MaxPathLen + 1];
    TKrnlPathStr::bQueryPath(szRealPath, szPathPart, kCIDLib::c4MaxPathLen);
    m_kstrSrcPath = szPathPart;

    return kCIDLib::True;
}


//
//  This is used to load up non-CIDLib based modules. We get the full path to load
//  and module type. No names or type have been set in this case. In most cases this
//  will be a shared library, but it's possible we might load executables in some
//  cases (to do something to to them and save them back out.)
//
tCIDLib::TBoolean
TKrnlModule::bLoadPlatExt(  const   tCIDLib::TCh* const pszLoadPath
                            , const tCIDLib::EModTypes  eModType)
{
    if (eModType == tCIDLib::EModTypes::Exe)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidParameter);
        return kCIDLib::False;
    }

    tCIDLib::TCh szRealPath[kCIDLib::c4MaxPathLen + 1];
    if (!CIDKernel_Module_Linux::bFindSharedLibrary(pszLoadPath, szRealPath, c4MaxBufChars(szRealPath)))
        return kCIDLib::False;

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

    tCIDLib::TCh szPathPart[kCIDLib::c4MaxPathLen + 1];
    TKrnlPathStr::bQueryPath(szRealPath, szPathPart, kCIDLib::c4MaxPathLen);
    m_kstrSrcPath = szPathPart;
}


//
//  To query an already loaded module, but not a CIDLib based one, so the name will
//  not conform to CIDLib module naming standards. No module names or type will have
//  be set in this case. It's always a shared library, since the program itself is by
//  definition a CIDLib program.
//
tCIDLib::TBoolean
TKrnlModule::bQueryPlatExt(const tCIDLib::TCh* const  pszName)
{
    tCIDLib::TCh szRealPath[kCIDLib::c4MaxPathLen + 1];

    if (!CIDKernel_Module_Linux::bFindSharedLibrary(m_kstrLoadName.pszValue(), szRealPath, c4MaxBufChars(szRealPath)))
        return kCIDLib::False;

    tCIDLib::TBoolean bLoaded;
    if (!CIDKernel_Module_Linux::bIsLibLoaded(szRealPath, bLoaded))
        return kCIDLib::False;

    if (!bLoaded)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_ModuleNotFound);
        return kCIDLib::False;
    }

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

    // Store the path it was loaded from
    tCIDLib::TCh szPathPart[kCIDLib::c4MaxPathLen + 1];
    TKrnlPathStr::bQueryPath(szRealPath, szPathPart, kCIDLib::c4MaxPathLen);
    m_kstrSrcPath = szPathPart;

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlModule: Private, static methods
// ---------------------------------------------------------------------------

//
//  Build up the name of a CIDLib module, both the portable part (the basic name
//  that would be used to build up the names of related files (msg text, resources,
//  etc...) and the loadable version that would be used to load a module dynamically
//  or to run an executable.
//
//  So, for a facility named Foobar, and a version of 1.2, we get:
//
//  Library:
//      Portable: FooBar_1_2
//      Loadable: FooBar.so_1_2
//
//  Exe:
//      Portable: FooBar
//      Loadable: FooBar
//
tCIDLib::TVoid
TKrnlModule::MakePlatNames( const   tCIDLib::TCh* const     pszModName
                            ,       TKrnlString&            kstrPortable
                            ,       TKrnlString&            kstrLoadable
                            , const tCIDLib::TCard4         c4MajVer
                            , const tCIDLib::TCard4         c4MinVer
                            , const tCIDLib::EModTypes      eModType)
{
    const tCIDLib::TCard4 c4BufSz = kCIDLib::c4MaxPathLen;
    tCIDLib::TCh achNameBuf[c4BufSz + 1];

    if (eModType == tCIDLib::EModTypes::SharedLib)
    {
        constexpr tCIDLib::TCard4 c4BufSz = 256;
        tCIDLib::TCh szTmpBuf[c4BufSz + 1];

        // Do the loadable name
        TRawStr::CopyStr(achNameBuf, L"lib");
        TRawStr::CatStr(achNameBuf, pszModName);
        TRawStr::CatStr(achNameBuf, L".so_");
        TRawStr::bFormatVal(c4MajVer, szTmpBuf, c4BufSz);
        TRawStr::CatStr(achNameBuf, szTmpBuf, c4BufSz);
        TRawStr::CatStr(achNameBuf, L"_", c4BufSz);
        TRawStr::bFormatVal(c4MinVer, szTmpBuf, c4BufSz);
        TRawStr::CatStr(achNameBuf, szTmpBuf, c4BufSz);
        kstrLoadable = achNameBuf;

        // Do the portable name
        TRawStr::CopyStr(achNameBuf, pszModName);
        TRawStr::CatStr(achNameBuf, L"_");
        TRawStr::bFormatVal(c4MajVer, szTmpBuf, c4BufSz);
        TRawStr::CatStr(achNameBuf, szTmpBuf, c4BufSz);
        TRawStr::CatStr(achNameBuf, L"_", c4BufSz);
        TRawStr::bFormatVal(c4MinVer, szTmpBuf, c4BufSz);
        TRawStr::CatStr(achNameBuf, szTmpBuf, c4BufSz);
        kstrPortable = achNameBuf;        
    }
     else if (eModType == tCIDLib::EModTypes::Exe)
    {
        // For us, the portable name is just the base name
        kstrLoadable = pszModName;
        kstrPortable = pszModName;
    }
     else
    {
        // Should not happen since the portable layer should have checked this
        TKrnlPopup::Show
        (
            CID_FILE
            , CID_LINE
            , L"Linux Module Support"
            , nullptr
            , 0
            , 0
            , 0
            , L"Only shared lib and exes are supported for name building"
        );
    }
}