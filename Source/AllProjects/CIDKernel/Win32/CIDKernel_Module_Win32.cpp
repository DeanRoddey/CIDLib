//
// FILE NAME: CIDKernel_Module_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/12/1998
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
//  This file provides the Win32 specific implementation of the the class
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


// ---------------------------------------------------------------------------
//  Local helper functions
// ---------------------------------------------------------------------------

//
//  A helper to get the path to a module, remove the name part to leave just the path
//  and replicate to the caller's passed target string.
//
static tCIDLib::TBoolean
bQueryPath(const HINSTANCE& hmodSrc, TKrnlString& kstrToFill)
{
    //
    //  Get the module path. We combine that with the portable module name we
    //  created above, and  us those to laod the messages.
    //
    tCIDLib::TCh szModPath[kCIDLib::c4MaxPathLen + 1];
    if (!::GetModuleFileName(hmodSrc, szModPath, kCIDLib::c4MaxPathLen))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Remove the name part and store it
    TKrnlPathStr::bRemoveLevel(szModPath);
    kstrToFill = szModPath;

    return kCIDLib::True;
}

// Like above, but for externals where we need to get both the path and the name
static tCIDLib::TBoolean
bQueryPathAndName(  const   HINSTANCE       hmodSrc
                    ,       TKrnlString&    kstrPath
                    ,       TKrnlString&    kstrName)
{
    //
    //  Get the module path. We combine that with the portable module name we
    //  created above, and  us those to laod the messages.
    //
    tCIDLib::TCh szModPath[kCIDLib::c4MaxPathLen + 1];
    if (!::GetModuleFileName(hmodSrc, szModPath, kCIDLib::c4MaxPathLen))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Get the name out
    tCIDLib::TCh szName[256];
    if (!TKrnlPathStr::bQueryName(szModPath, szName, 255))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_NoModName);
        return kCIDLib::False;
    }

    // Remove the name part from the path, and then we can give both back
    TKrnlPathStr::bRemoveLevel(szModPath);

    kstrPath = szModPath;
    kstrName = szName;
    return kCIDLib::True;
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
    m_phmodiThis->hInstance = nullptr;
}

TModuleHandle::TModuleHandle(const TModuleHandle& hmodToCopy) :

    m_phmodiThis(nullptr)
{
    m_phmodiThis = new TModuleHandleImpl;
    m_phmodiThis->hInstance = hmodToCopy.m_phmodiThis->hInstance;
}

TModuleHandle::~TModuleHandle()
{
    delete m_phmodiThis;
    m_phmodiThis = nullptr;
}


// ---------------------------------------------------------------------------
//  Public operators
// ---------------------------------------------------------------------------
TModuleHandle& TModuleHandle::operator=(const TModuleHandle& hmodSrc)
{
    if (this != &hmodSrc)
    {
        m_phmodiThis->hInstance = hmodSrc.m_phmodiThis->hInstance;
    }
    return *this;
}


tCIDLib::TBoolean TModuleHandle::operator==(const TModuleHandle& hmodSrc) const
{
    return (m_phmodiThis->hInstance == hmodSrc.m_phmodiThis->hInstance);
}

tCIDLib::TBoolean TModuleHandle::operator!=(const TModuleHandle& hmodSrc) const
{
    return !operator==(hmodSrc);
}


// ---------------------------------------------------------------------------
//  Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TModuleHandle::bIsValid() const
{
    return (m_phmodiThis->hInstance != nullptr);
}


tCIDLib::TVoid TModuleHandle::Clear()
{
    m_phmodiThis->hInstance = nullptr;
}

tCIDLib::TVoid
TModuleHandle::FormatToStr(         tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_phmodiThis->hInstance), pszToFill, c4MaxChars, tCIDLib::ERadices::Hex
    );
}

const TModuleHandleImpl& TModuleHandle::hmodiThis() const
{
    return *m_phmodiThis;
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

    // Query the raw module name from the OS, which gives the full path
    tCIDLib::TCh szTmpBuf[kCIDLib::c4MaxPathLen + 1];
    if (!::GetModuleFileName(hmodSrc.m_phmodiThis->hInstance, szTmpBuf, kCIDLib::c4MaxPathLen))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Find the last period and put a nul there
    tCIDLib::TCh* pszTmp;
    pszTmp = TRawStr::pszFindLastChar(szTmpBuf, kCIDLib::chPeriod);
    if (pszTmp)
        *pszTmp = kCIDLib::chNull;

    //
    //  Now find the last separator. The stuff past it will be the name. If
    //  there is none, then its all the name.
    //
    pszTmp = TRawStr::pszFindLastChar(szTmpBuf, L'\\');
    if (!pszTmp)
    {
        kstrName = szTmpBuf;
        return kCIDLib::True;
    }

    kstrName = (pszTmp + 1);
    *pszTmp = kCIDLib::chNull;
    kstrPath = szTmpBuf;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlModule::bRawQueryModPath(  const   TModuleHandle&  hmodSrc
                                ,       TKrnlString&    kstrToFill)
{
    // Init the buffer to empty
    kstrToFill.Clear();

    // Query the raw module name from the OS, which gives the full path
    tCIDLib::TCh szTmpBuf[kCIDLib::c4MaxPathLen+1];
    if (!::GetModuleFileName(hmodSrc.m_phmodiThis->hInstance, szTmpBuf, kCIDLib::c4MaxPathLen))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Find the last period and put a nul there
    tCIDLib::TCh* pszTmp;
    pszTmp = TRawStr::pszFindLastChar(szTmpBuf, kCIDLib::chPeriod);
    if (pszTmp)
        *pszTmp = kCIDLib::chNull;

    //
    //  Now find the last separator. The stuff past it will be the name. If
    //  there is none, then its all the name and there is no path.
    //
    pszTmp = TRawStr::pszFindLastChar(szTmpBuf, L'\\');
    if (!pszTmp)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_NoModPath);
        return kCIDLib::False;
    }

    // Cap it at the name separator, and copy what's left to the caller's buf
    *pszTmp = kCIDLib::chNull;
    kstrToFill = szTmpBuf;

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlModule: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlModule::bGetFuncPtr(const  tCIDLib::TSCh* const    pszFuncName
                        ,       tCIDLib::FuncPtr&       pfnToFill) const
{
    pfnToFill = reinterpret_cast<tCIDLib::FuncPtr>
    (
        ::GetProcAddress(m_hmodThis.m_phmodiThis->hInstance, pszFuncName)
    );
    return (pfnToFill != 0);
}


//
//  For loading external modules, not CIDLIb facilities. So we get whatever bit
//  of the name/path that the caller things is necessary.
//
tCIDLib::TBoolean TKrnlModule::bLoadExternal(const tCIDLib::TCh* const pszToLoad)
{
    // Clear the current handle if any
    if (!bCleanup())
        return kCIDLib::False;

    // And now try to load the library
    HINSTANCE hTmp = ::LoadLibrary(pszToLoad);

    // If it failed, return the error with our handle stuff still cleared
    if (!hTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    //
    //  It worked, so store the handle. Set the flag that indicates this
    //  handle came from a load operation.
    //
    m_bViaLoad = kCIDLib::True;
    m_hmodThis.m_phmodiThis->hInstance = hTmp;

    // Query back the path and name
    if (!bQueryPathAndName(hTmp, m_kstrSrcPath, m_kstrBaseName))
    {
        bCleanup();
        return kCIDLib::False;
    }

    // We assume these are always libraries
    m_eModType = tCIDLib::EModTypes::SharedLib;

    // We don't set the portable name, but the load name is the same as the base here
    m_kstrLoadName = m_kstrBaseName;

    return kCIDLib::True;
}


//
//  Given the already built up loadable module name for the current platform, we
//  try to load it up. In this case, this is just for loading external stuff, not
//  CIDLib facilities, so no msg text loading or any of that. The passed value
//  may be a name or a path or a partial path.
//
tCIDLib::TBoolean TKrnlModule::bQueryExternal(const tCIDLib::TCh* const pszModName)
{
    // Clear the previous handle, if any
    if (!bCleanup())
        return kCIDLib::False;

    // And now try to look up the name
    HINSTANCE hTmp = ::GetModuleHandle(pszModName);

    // If it failed, we just return false and are done, we already cleaned up
    if (!hTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    //
    //  It worked, so store the handle. And set the field that indicates
    //  that this was not a load, just a query.
    //
    m_bViaLoad = kCIDLib::False;
    m_hmodThis.m_phmodiThis->hInstance = hTmp;

    // We assume these are always libraries
    m_eModType = tCIDLib::EModTypes::SharedLib;

    // Query back the path and name
    if (!bQueryPathAndName(hTmp, m_kstrSrcPath, m_kstrBaseName))
    {
        bCleanup();
        return kCIDLib::False;
    }

    //
    //  We don't set the portable name for externals, but the load name is the same as
    //  the base name here
    //
    m_kstrLoadName = m_kstrBaseName;
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------

// We do any Win32 specific cleanup
tCIDLib::TBoolean TKrnlModule::bPlatCleanup()
{
    //
    //  If there is a module handle and it was gotten via a load, then we need
    //  to free it.
    //
    if (m_bViaLoad && m_hmodThis.m_phmodiThis->hInstance)
    {
        if (!::FreeLibrary(m_hmodThis.m_phmodiThis->hInstance))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


//
//  This is called from the platform independent code after it has stores the module
//  names. We need to try to load the library, store the handle, and store the path
//  where it was found.
//
tCIDLib::TBoolean TKrnlModule::bLoadPlatModByName(const tCIDLib::EModTypes)
{
    HINSTANCE hTmp = ::LoadLibrary(m_kstrLoadName.pszValue());
    if (!hTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    m_hmodThis.m_phmodiThis->hInstance = hTmp;

    // Store path that it was loaded from
    if (!bQueryPath(hTmp, m_kstrSrcPath))
        return kCIDLib::False;

    return kCIDLib::True;
}


//
//  This is for when we need to load a CIDLib facility, but from a specific path,
//  so something like a CQC device driver.
//
tCIDLib::TBoolean
TKrnlModule::bLoadPlatModByPath(const tCIDLib::EModTypes, const tCIDLib::TCh* const pszLoadPath)
{
    // Build up the full path using the loadable name
    tCIDLib::TCh szFullPath[kCIDLib::c4MaxPathLen + 1];
    if (!TKrnlPathStr::bCombinePath(szFullPath, pszLoadPath, m_kstrLoadName.pszValue(), kCIDLib::c4MaxPathLen))
        return kCIDLib::False;

    // OK, let's try to load what we got
    HINSTANCE hTmp = ::LoadLibrary(szFullPath);

    // If it failed, cleanup and return false
    if (!hTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    m_hmodThis.m_phmodiThis->hInstance = hTmp;

    //
    //  Get the path of our module. We got a path, but this insures we have a canonical
    //  fully qualfied path format.
    //
    if (!bQueryPath(hTmp, m_kstrSrcPath))
        return kCIDLib::False;

    return kCIDLib::True;
}


//
//  Give the basic module name (facility name) and the version, we will load this
//  module up into ourself. If the flags indicate it has loadable text, we will
//  try to load the best loadable text file for the locale.
//
tCIDLib::TBoolean TKrnlModule::bQueryPlatModByName(const tCIDLib::EModTypes)
{
    HINSTANCE hTmp = ::GetModuleHandle(m_kstrLoadName.pszValue());
    if (!hTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    m_hmodThis.m_phmodiThis->hInstance = hTmp;

    // Store the path it loaded from
    if (!bQueryPath(hTmp, m_kstrSrcPath))
        return kCIDLib::False;

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
//      Loadable: FooBar_1_2    (DLLs don't use the extension for loading.)
//
//  Exe:
//      Portable: FooBar
//      Loadable: FooBar.exe    (Exe's are not versioned)
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
        //
        //  If a shared library, we have to to add the versioning info, but the
        //  loadable and portable names are the same.
        //
        constexpr tCIDLib::TCard4 c4BufSz = 256;
        tCIDLib::TCh szTmpBuf[c4BufSz + 1];

        TRawStr::CopyStr(achNameBuf, pszModName);
        TRawStr::CatStr(achNameBuf, L"_", c4BufSz);
        TRawStr::bFormatVal(c4MajVer, szTmpBuf, c4BufSz);
        TRawStr::CatStr(achNameBuf, szTmpBuf, c4BufSz);

        TRawStr::CatStr(achNameBuf, L"_", c4BufSz);
        TRawStr::bFormatVal(c4MinVer, szTmpBuf, c4BufSz);
        TRawStr::CatStr(achNameBuf, szTmpBuf, c4BufSz);

        kstrPortable = achNameBuf;
        kstrLoadable = achNameBuf;
    }
     else if (eModType == tCIDLib::EModTypes::Exe)
    {
        // The loadable name is the base name plus the exe extension
        TRawStr::CopyStr(achNameBuf, pszModName, c4BufSz);
        TRawStr::CatStr(achNameBuf, kCIDLib::szExeExtension, c4BufSz);
        kstrLoadable = achNameBuf;

        // For us, the portable name is just the base name
        kstrPortable = pszModName;
    }
     else
    {
        // Should not happen since the portable layer should have checked this
        TKrnlPopup::Show
        (
            CID_FILE
            , CID_LINE
            , L"Win32 Module Support"
            , nullptr
            , 0
            , 0
            , 0
            , L"Only shared lib and exes are supported for name building"
        );
    }
}
