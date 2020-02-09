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
//  names and type. We need to try to load the library, store the handle, and store
//  the path where it was found.
//
tCIDLib::TBoolean TKrnlModule::bLoadPlatByName(const tCIDLib::EModTypes)
{
    HINSTANCE hTmp = ::LoadLibrary(m_kstrLoadName.pszValue());
    if (!hTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    m_hmodThis.m_phmodiThis->hInstance = hTmp;

    // Query the path. This insures we get the cononical path
    if (!bQueryPath(hTmp, m_kstrSrcPath))
        return kCIDLib::False;

    return kCIDLib::True;
}


//
//  This is for when we need to load a CIDLib facility, but from a specific path,
//  so something like a CQC device driver. We get the path that it's in. This
//  is after the names and type are set.
//
tCIDLib::TBoolean
TKrnlModule::bLoadPlatByPath(const  tCIDLib::TCh* const pszLoadPath
                            , const tCIDLib::EModTypes  )
{
    // Build up the full path
    TKrnlString kstrFullPath(pszLoadPath, m_kstrLoadName.pszValue());

    // Try to load the path as is
    HINSTANCE hTmp = ::LoadLibrary(kstrFullPath.pszValue());

    // If it failed, cleanup and return false
    if (!hTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    m_hmodThis.m_phmodiThis->hInstance = hTmp;

    // Query the path. This makes sure we have the FQ, canonical path
    if (!bQueryPath(hTmp, m_kstrSrcPath))
        return kCIDLib::False;

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
    // Try to load the path as is
    HINSTANCE hTmp = ::LoadLibrary(pszLoadPath);

    // If it failed, cleanup and return false
    if (!hTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    m_hmodThis.m_phmodiThis->hInstance = hTmp;

    // Query the path and load name
    if (!bQueryPathAndName(hTmp, m_kstrSrcPath, m_kstrLoadName))
        return kCIDLib::False;

    // The base name will be the same in this case, no portable name is set
    m_kstrBaseName = m_kstrLoadName;
    m_eModType = eModType;

    return kCIDLib::True;
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
    HINSTANCE hTmp = ::GetModuleHandle(pszName);
    if (!hTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    m_hmodThis.m_phmodiThis->hInstance = hTmp;

    // Query back the path and name, to get caonical info
    if (!bQueryPathAndName(hTmp, m_kstrSrcPath, m_kstrLoadName))
        return kCIDLib::False;

    // The base name is the same as the loadable in this case, no portable name is set
    m_kstrBaseName = m_kstrLoadName;
    return kCIDLib::True;
}


//
//  Query an already loaded module by name. The names and type are already stored
//  at this point.
//
tCIDLib::TBoolean TKrnlModule::bQueryPlatByName()
{
    HINSTANCE hTmp = ::GetModuleHandle(m_kstrLoadName.pszValue());
    if (!hTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    m_hmodThis.m_phmodiThis->hInstance = hTmp;

    // Store the path it originally loaded from
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
