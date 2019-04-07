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


namespace
{
    // ---------------------------------------------------------------------------
    //  Local Functions
    // ---------------------------------------------------------------------------
    tCIDLib::TVoid __BuildModName(tCIDLib::TCh* const         pszNameBuf
                                  , const tCIDLib::TCard4     c4MaxChars
                                  , const tCIDLib::TCh* const pszModName
                                  , const tCIDLib::TCard4     c4MajVer
                                  , const tCIDLib::TCard4     c4MinVer)
    {
        TRawStr::CopyCatStr(pszNameBuf, c4MaxChars
                            , kCIDLib::szLibraryPrefix
                            , pszModName
                            , kCIDLib::szLibrarySuffix);
        const tCIDLib::TCard4 c4ValBufSize = 256;
        tCIDLib::TCh szValBuf[c4ValBufSize + 1];
        TRawStr::CatStr(pszNameBuf, L".", c4MaxChars);
        TRawStr::bFormatVal(c4MajVer, szValBuf, c4ValBufSize);
        TRawStr::CatStr(pszNameBuf, szValBuf, c4MaxChars);
        TRawStr::CatStr(pszNameBuf, L".", c4MaxChars);
        TRawStr::bFormatVal(c4MinVer, szValBuf, c4ValBufSize);
        TRawStr::CatStr(pszNameBuf, szValBuf, c4MaxChars);
    }

    tCIDLib::TBoolean __bCheckAndResolvePath(const tCIDLib::TSCh* const pszPath
                                             , tCIDLib::TCh* const pszToFill
                                             , const tCIDLib::TCard4 c4MaxChars)
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

    tCIDLib::TBoolean __bFindSharedLibrary(const tCIDLib::TCh* const pszLib
                                           , tCIDLib::TCh* const pszToFill
                                           , const tCIDLib::TCard4 c4MaxChars)
    {
        tCIDLib::TSCh* pszLocLib = TRawStr::pszConvert(pszLib);
        TArrayJanitor<tCIDLib::TSCh> janLocLib(pszLocLib);

        if (pszLib[0] == kCIDLib::chPathSeparator)
        {
            return __bCheckAndResolvePath(pszLocLib, pszToFill, c4MaxChars);
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

                if (__bCheckAndResolvePath(szCurDir, pszToFill, c4MaxChars))
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

                    if (__bCheckAndResolvePath(szCurDir, pszToFill, c4MaxChars))
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
            if (__bCheckAndResolvePath(szCurDir, pszToFill, c4MaxChars))
                return kCIDLib::True;
        }

        // No need to check the length here, because we know
        // it's shorter than the last one!
        ::strcpy(szCurDir, "\\lib\\");
        ::strcat(szCurDir, pszLocLib);
        if (__bCheckAndResolvePath(szCurDir, pszToFill, c4MaxChars))
            return kCIDLib::True;

        return kCIDLib::False;
    }

    tCIDLib::TBoolean __bIsLibLoaded(const tCIDLib::TCh* const pszLibName
                                     , tCIDLib::TBoolean& bLoaded)
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

    tCIDLib::TBoolean __bFindExecutable(const tCIDLib::TCh* const pszExe
                                        , tCIDLib::TCh* const pszToFill
                                        , const tCIDLib::TCard4 c4MaxChars)
    {
        tCIDLib::TSCh* pszLocExe = TRawStr::pszConvert(pszExe);
        TArrayJanitor<tCIDLib::TSCh> janExe(pszLocExe);

        // If it's an absolute path, then we're good (or bad...)
        if (pszExe[0] == kCIDLib::chPathSeparator)
        {
            return __bCheckAndResolvePath(pszLocExe, pszToFill, c4MaxChars);
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
                if (__bCheckAndResolvePath(szCurDir, pszToFill, c4MaxChars))
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

                if (__bCheckAndResolvePath(szCurDir, pszToFill, c4MaxChars))
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
    __phmodiThis(0)
{
    __phmodiThis = new TModuleHandleImpl;
    __phmodiThis->pHandle = 0;
    __phmodiThis->pszFullPath = 0;
}

TModuleHandle::TModuleHandle(const TModuleHandle& hmodToCopy) :
    __phmodiThis(0)
{
    __phmodiThis = new TModuleHandleImpl;
    __phmodiThis->pHandle = hmodToCopy.__phmodiThis->pHandle;
    __phmodiThis->pszFullPath = TRawStr::pszReplicate(hmodToCopy.__phmodiThis->pszFullPath);
}

TModuleHandle::~TModuleHandle()
{
    delete [] __phmodiThis->pszFullPath;
    delete __phmodiThis;
    __phmodiThis = 0;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TModuleHandle& TModuleHandle::operator=(const TModuleHandle& hmodToAssign)
{
    if (this == &hmodToAssign)
        return *this;

    __phmodiThis->pHandle = hmodToAssign.__phmodiThis->pHandle;
    delete [] __phmodiThis->pszFullPath;
    __phmodiThis->pszFullPath = TRawStr::pszReplicate(hmodToAssign.__phmodiThis->pszFullPath);

    return *this;
}


tCIDLib::TBoolean
TModuleHandle::operator==(const TModuleHandle& hmodToCompare) const
{
    return (__phmodiThis->pHandle == hmodToCompare.__phmodiThis->pHandle
    &&      TRawStr::eCompareStr(__phmodiThis->pszFullPath
                                 , hmodToCompare.__phmodiThis->pszFullPath)
            == tCIDLib::ESortComps::Equal);
}



// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TModuleHandle::bValid() const
{
    return ((__phmodiThis->pHandle && __phmodiThis->pszFullPath)
    ||      (__phmodiThis->pszFullPath));
}


tCIDLib::TVoid TModuleHandle::Clear()
{
    __phmodiThis->pHandle = 0;
    delete [] __phmodiThis->pszFullPath;
    __phmodiThis->pszFullPath = 0;
}

tCIDLib::TVoid
TModuleHandle::FormatToStr(         tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    if (!__phmodiThis->pHandle)
    {
        TRawStr::CopyStr(pszToFill, __phmodiThis->pszFullPath, c4MaxChars);
    }
    else
    {
        TRawStr::bFormatVal(tCIDLib::TCard4(__phmodiThis->pHandle)
                            , pszToFill
                            , c4MaxChars
                            , tCIDLib::ERadices::Hex);
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
    if (!hmodSrc.__phmodiThis->pszFullPath)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InvalidHandle);
        return kCIDLib::False;
    }

    if (!(TKrnlPathStr::bQueryPath(hmodSrc.__phmodiThis->pszFullPath, pszPathToFill, c4MaxChars)
          && TKrnlPathStr::bQueryNameExt(hmodSrc.__phmodiThis->pszFullPath, pszNameToFill, c4MaxChars)))
    {
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlModule: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlModule::TKrnlModule() :
    __bViaLoad(kCIDLib::False)
    , __pmiThis(0)
{
}

TKrnlModule::~TKrnlModule()
{
    // Delete the message file buffer if any
    delete [] reinterpret_cast<tCIDLib::TCh*>(__pmiThis);
    __pmiThis = 0;

    // Do the required handle cleanup stuff
    if (!__bClearHandle())
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
                        ,       tCIDLib::TVoid*&        pfnToFill) const
{
    if (!__hmodThis.__phmodiThis->pHandle)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotReady);
        return kCIDLib::False;
    }

    pfnToFill = ::dlsym(__hmodThis.__phmodiThis->pHandle
                        , pszFuncName);

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
    if (!__bClearHandle())
        return kCIDLib::False;

    // Now make a platform-specific name out of the module name
    tCIDLib::TCh szNameBuf[kCIDLib::c4MaxPathLen + 1];
    __BuildModName(szNameBuf, kCIDLib::c4MaxPathLen, pszModName, c4MajVer, c4MinVer);

    // First find the library. dlopen would go through the same
    // steps, and since we need to get the full path name of it,
    // it makes since to do it manually first.
    tCIDLib::TCh szRealPath[kCIDLib::c4MaxPathLen + 1];
    if (!__bFindSharedLibrary(szNameBuf, szRealPath, kCIDLib::c4MaxPathLen))
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
    __bViaLoad = kCIDLib::True;
    __hmodThis.__phmodiThis->pHandle = pTmp;

    delete [] __hmodThis.__phmodiThis->pszFullPath;
    __hmodThis.__phmodiThis->pszFullPath = TRawStr::pszReplicate(szRealPath);

    //
    //  See if the caller says this facility has a message file, and load it
    //  up if so. Delete the old one first, if any.
    //
    delete [] reinterpret_cast<tCIDLib::TCh*>(__pmiThis);
    __pmiThis = 0;

    if (eModFlags & tCIDLib::EModFlags::HasMsgFile)
    {
        if (!__bLoadMessages(__hmodThis, __pmiThis))
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
    if (!__bClearHandle())
        return kCIDLib::False;

    // Now make a platform-specific name out of the module name
    tCIDLib::TCh szNameBuf[kCIDLib::c4MaxPathLen + 1];
    __BuildModName(szNameBuf, kCIDLib::c4MaxPathLen, pszModName, c4MajVer, c4MinVer);

    tCIDLib::TCh szRealPath[kCIDLib::c4MaxPathLen + 1];

    if (eModType == tCIDLib::EModTypes::Dll)
    {
        if (!__bFindSharedLibrary(szNameBuf, szRealPath, c4MaxBufChars(szRealPath)))
            return kCIDLib::False;

        tCIDLib::TBoolean bLoaded;
        if (!__bIsLibLoaded(szRealPath, bLoaded))
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

        __hmodThis.__phmodiThis->pHandle = pTmp;
    }
    else
    {
        if (!__bFindExecutable(pszModName, szRealPath, c4MaxBufChars(szRealPath)))
            return kCIDLib::False;
        __hmodThis.__phmodiThis->pHandle = 0;
    }

    __bViaLoad = kCIDLib::False;

    delete [] __hmodThis.__phmodiThis->pszFullPath;
    __hmodThis.__phmodiThis->pszFullPath = TRawStr::pszReplicate(szRealPath);

    //
    //  See if the caller says this facility has a message file, and load it
    //  up if so. Delete the old one first, if any.
    //
    delete [] reinterpret_cast<tCIDLib::TCh*>(__pmiThis);
    __pmiThis = 0;

    if (eModFlags & tCIDLib::EModFlags::HasMsgFile)
    {
        if (!__bLoadMessages(__hmodThis, __pmiThis))
            return kCIDLib::False;
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlModule::__bClearHandle()
{
    //
    //  If there is a module handle, it was via a load, and we have adopted
    //  it then we need to free it.
    //
    if (__hmodThis.__phmodiThis->pHandle)
    {
        if (::dlclose(__hmodThis.__phmodiThis->pHandle))
        {
            TKrnlError::SetLastHostError(errno);
            return kCIDLib::False;
        }
    }

    __hmodThis.Clear();
    __bViaLoad = kCIDLib::False;

    return kCIDLib::True;
}
