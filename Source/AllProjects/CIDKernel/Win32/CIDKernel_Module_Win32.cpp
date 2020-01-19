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
static tCIDLib::TBoolean
bCreateModNames(    const   tCIDLib::TCh* const pszModName
                    , const tCIDLib::TCard4     c4MajVer
                    , const tCIDLib::TCard4     c4MinVer
                    , const tCIDLib::EModTypes  eModType
                    , const tCIDLib::TCh*&      pszBaseName
                    , const tCIDLib::TCh*&      pszLoadName
                    , const tCIDLib::TCh*&      pszPortName)
{
    // The name must just be  a name, no path
    if (TRawStr::pszFindChar(pszModName, L'\\'))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_NoPathAllowed);
        return kCIDLib::False;
    }

    const tCIDLib::TCard4 c4NameBufSz = 1023;
    tCIDLib::TCh szPortName[c4NameBufSz + 1];
    tCIDLib::TCh szLoadName[c4NameBufSz + 1];
    const tCIDLib::TBoolean bRes = TKrnlModule::bBuildModName
    (
        szPortName, szLoadName, c4NameBufSz, pszModName, c4MajVer, c4MinVer, eModType
    );

    if (bRes)
    {
        pszBaseName = TRawStr::pszReplicate(pszModName);
        pszLoadName = TRawStr::pszReplicate(szLoadName);
        pszPortName = TRawStr::pszReplicate(szPortName);
    }
    return bRes;
}


//
//  A helper to get the path to a module, remove the name part to leave just the path
//  and replicate to the caller's passed target string.
//
static tCIDLib::TBoolean
bQueryPath(const HINSTANCE& hmodSrc, const tCIDLib::TCh*& pszModPath)
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
    pszModPath = TRawStr::pszReplicate(szModPath);

    return kCIDLib::True;
}

// Like above, but for externals where we need to get both the path and the name
static tCIDLib::TBoolean
bQueryPathAndName(  const   HINSTANCE       hmodSrc
                    , const tCIDLib::TCh*&  pszModPath
                    , const tCIDLib::TCh*&  pszModName)
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

    pszModPath = TRawStr::pszReplicate(szModPath);
    pszModName = TRawStr::pszReplicate(szName);

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
    m_phmodiThis->hInstance = 0;
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
TModuleHandle& TModuleHandle::operator=(const TModuleHandle& hmodToAssign)
{
    if (this == &hmodToAssign)
        return *this;

    m_phmodiThis->hInstance = hmodToAssign.m_phmodiThis->hInstance;

    return *this;
}


tCIDLib::TBoolean
TModuleHandle::operator==(const TModuleHandle& hmodToCompare) const
{
    return (m_phmodiThis->hInstance == hmodToCompare.m_phmodiThis->hInstance);
}

tCIDLib::TBoolean
TModuleHandle::operator!=(const TModuleHandle& hmodToCompare) const
{
    return !operator==(hmodToCompare);
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
        tCIDLib::TCard4(m_phmodiThis->hInstance)
        , pszToFill
        , c4MaxChars
        , tCIDLib::ERadices::Hex
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
//  Build up the the portable and loadable names for the indicated module and version and
//  module type.
//
tCIDLib::TBoolean
TKrnlModule::bBuildModName(             tCIDLib::TCh* const     pszPortableBuf
                                ,       tCIDLib::TCh* const     pszLoadableBuf
                                , const tCIDLib::TCard4         c4MaxChars
                                , const tCIDLib::TCh* const     pszModName
                                , const tCIDLib::TCard4         c4MajVer
                                , const tCIDLib::TCard4         c4MinVer
                                ,  const tCIDLib::EModTypes     eModType)
{
    // Init the buffers to empty
    *pszPortableBuf = kCIDLib::chNull;
    *pszLoadableBuf = kCIDLib::chNull;

    // Make sure the module type is understood
    if ((eModType != tCIDLib::EModTypes::Exe) && (eModType != tCIDLib::EModTypes::SharedLib))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcMod_BadModType);
        return kCIDLib::False;
    }
    TKrnlWin32::BuildModName
    (
        pszPortableBuf, pszLoadableBuf, c4MaxChars, pszModName, c4MajVer, c4MinVer, eModType
    );
    return kCIDLib::True;
}


//
//  Given the handle, get the module path and name. Since we are just querying
//  from the OS, the platform specific name is returned.
//
tCIDLib::TBoolean
TKrnlModule::bRawQueryModName(  const   TModuleHandle&      hmodSrc
                                ,       tCIDLib::TCh* const pszNameToFill
                                ,       tCIDLib::TCh* const pszPathToFill
                                , const tCIDLib::TCard4     c4MaxChars)
{
    // Init the buffers to empty
    *pszNameToFill = kCIDLib::chNull;
    *pszPathToFill = kCIDLib::chNull;

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
        TRawStr::CopyStr(pszNameToFill, szTmpBuf, c4MaxChars);
        return kCIDLib::True;
    }

    TRawStr::CopyStr(pszNameToFill, pszTmp+1, c4MaxChars);
    *pszTmp = kCIDLib::chNull;
    TRawStr::CopyStr(pszPathToFill, szTmpBuf, c4MaxChars);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlModule::bRawQueryModPath(  const   TModuleHandle&      hmodSrc
                                ,       tCIDLib::TCh* const pszPathToFill
                                , const tCIDLib::TCard4     c4MaxChars)
{
    // Init the buffer to empty
    *pszPathToFill = kCIDLib::chNull;

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
    TRawStr::CopyStr(pszPathToFill, szTmpBuf, c4MaxChars);

    return kCIDLib::True;
}

// ---------------------------------------------------------------------------
//  TKrnlModule: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlModule::TKrnlModule() :

    m_bViaLoad(kCIDLib::False)
    , m_eModType(tCIDLib::EModTypes::Count)
    , m_pmiThis(nullptr)
    , m_pszBaseName(nullptr)
    , m_pszLoadName(nullptr)
    , m_pszPortName(nullptr)
    , m_pszSrcPath(nullptr)
{
}

TKrnlModule::~TKrnlModule()
{
    // Delete the message file buffer if any
    delete [] reinterpret_cast<tCIDLib::TCard1*>(m_pmiThis);
    m_pmiThis = nullptr;

    // Do the required handle cleanup stuff
    if (!bCleanup())
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
    pfnToFill = reinterpret_cast<tCIDLib::FuncPtr>
    (
        ::GetProcAddress(m_hmodThis.m_phmodiThis->hInstance, pszFuncName)
    );
    return (pfnToFill != 0);
}


tCIDLib::TBoolean
TKrnlModule::bLoadFromName( const   tCIDLib::TCh* const pszBaseName
                            , const tCIDLib::TCard4     c4MajVer
                            , const tCIDLib::TCard4     c4MinVer
                            , const tCIDLib::EModTypes  eModType
                            , const tCIDLib::EModFlags  eModFlags)
{
    // Clear the current info if any
    if (!bCleanup())
        return kCIDLib::False;

    // Create/store our various names
    tCIDLib::TBoolean bRes = bCreateModNames
    (
        pszBaseName
        , c4MajVer
        , c4MinVer
        , eModType
        , m_pszBaseName
        , m_pszLoadName
        , m_pszPortName
    );
    if (!bRes)
        return kCIDLib::False;

    // And now try to load the library using the loadable name
    HINSTANCE hTmp = ::LoadLibrary(m_pszLoadName);
    if (!hTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        bCleanup();
        return kCIDLib::False;
    }

    //
    //  It worked, so store the handle. Set the flag that indicates this
    //  handle came from a load operation.
    //
    m_bViaLoad = kCIDLib::True;
    m_hmodThis.m_phmodiThis->hInstance = hTmp;

    // Store path that it was loaded from
    if (!bQueryPath(hTmp, m_pszSrcPath))
    {
        bCleanup();
        return kCIDLib::False;
    }

    //
    //  If this one has loadable text, then load the message file. We don't return a
    //  falure if we can't load it. That could cause some catastrophic failure. Thkis
    //  way the module is there, but any msgs loaded just won't be found.
    //
    if (tCIDLib::bAllBitsOn(eModFlags, tCIDLib::EModFlags::HasMsgFile))
        bLoadMessages(m_pszSrcPath, m_pszPortName, m_pmiThis);

    return kCIDLib::True;
}


//
//  This is for when we need to load a CIDLib facility, but from a specific path,
//  so something like a CQC device driver.
//
tCIDLib::TBoolean
TKrnlModule::bLoadFromPath( const   tCIDLib::TCh* const pszBaseName
                            , const tCIDLib::TCard4     c4MajVer
                            , const tCIDLib::TCard4     c4MinVer
                            , const tCIDLib::TCh* const pszLoadPath
                            , const tCIDLib::EModTypes  eModType
                            , const tCIDLib::EModFlags  eModFlags)
{
    // Clear the current handle if any
    if (!bCleanup())
        return kCIDLib::False;

    // Create/store our various names
    tCIDLib::TBoolean bRes = bCreateModNames
    (
        pszBaseName
        , c4MajVer
        , c4MinVer
        , eModType
        , m_pszBaseName
        , m_pszLoadName
        , m_pszPortName
    );
    if (!bRes)
        return kCIDLib::False;

    // Build up the full path
    tCIDLib::TCh szFullPath[kCIDLib::c4MaxPathLen + 1];
    if (!TKrnlPathStr::bCombinePath(szFullPath, pszLoadPath, pszBaseName, kCIDLib::c4MaxPathLen))
        return kCIDLib::False;

    // OK, let's try to load what we got
    HINSTANCE hTmp = ::LoadLibrary(szFullPath);

    // If it failed, cleanup and return false
    if (!hTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        bCleanup();
        return kCIDLib::False;
    }

    //
    //  It worked, so store the handle. Set the flag that indicates this
    //  handle came from a load operation.
    //
    m_bViaLoad = kCIDLib::True;
    m_hmodThis.m_phmodiThis->hInstance = hTmp;
    m_eModType = eModType;

    //
    //  Get the path of our module. We got a path, but this insures we have a canonical
    //  fully qualfied path format.
    //
    if (!bQueryPath(hTmp, m_pszSrcPath))
    {
        bCleanup();
        return kCIDLib::False;
    }

    //
    //  If this one has loadable text, then load the message file. We don't return a
    //  falure if we can't load it. That could cause some catastrophic failure. This
    //  way the module is there, but any msgs loaded just won't be found.
    //
    if (tCIDLib::bAllBitsOn(eModFlags, tCIDLib::EModFlags::HasMsgFile))
        bLoadMessages(m_pszSrcPath, m_pszPortName, m_pmiThis);

    return kCIDLib::True;
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
    if (!bQueryPathAndName(hTmp, m_pszSrcPath, m_pszBaseName))
    {
        bCleanup();
        return kCIDLib::False;
    }

    // We assume these are always libraries
    m_eModType = tCIDLib::EModTypes::SharedLib;

    // We don't set the portable name, but the load name is the same as the base here
    m_pszLoadName = TRawStr::pszReplicate(m_pszBaseName);

    return kCIDLib::True;
}


//
//  Give the basic module name (facility name) and the version, we will load this
//  module up into ourself. If the flags indicate it has loadable text, we will
//  try to load the best loadable text file for the locale.
//
tCIDLib::TBoolean
TKrnlModule::bQueryFromName(const   tCIDLib::TCh* const pszBaseName
                            , const tCIDLib::TCard4     c4MajVer
                            , const tCIDLib::TCard4     c4MinVer
                            , const tCIDLib::EModTypes  eModType
                            , const tCIDLib::EModFlags  eModFlags)
{
    // Clear the previous handle, if any
    if (!bCleanup())
        return kCIDLib::False;

    // Create/store our various names
    tCIDLib::TBoolean bRes = bCreateModNames
    (
        pszBaseName
        , c4MajVer
        , c4MinVer
        , eModType
        , m_pszBaseName
        , m_pszLoadName
        , m_pszPortName
    );
    if (!bRes)
        return kCIDLib::False;

    // And now try to look up the name
    HINSTANCE hTmp = ::GetModuleHandle(m_pszLoadName);

    // If it failed, cleanup and return false
    if (!hTmp)
    {
        bCleanup();
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    //
    //  It worked, so store the handle. And set the field that indicates
    //  that this was not a load, just a query.
    //
    m_bViaLoad = kCIDLib::False;
    m_hmodThis.m_phmodiThis->hInstance = hTmp;
    m_eModType = eModType;

    // Store the path it loaded from
    if (!bQueryPath(hTmp, m_pszSrcPath))
    {
        bCleanup();
        return kCIDLib::False;
    }

    //
    //  If this one has loadable text, then load the message file. We don't return a
    //  falure if we can't load it. That could cause some catastrophic failure. Thkis
    //  way the module is there, but any msgs loaded just won't be found.
    //
    if (tCIDLib::bAllBitsOn(eModFlags, tCIDLib::EModFlags::HasMsgFile))
        bLoadMessages(m_pszSrcPath, m_pszPortName, m_pmiThis);

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
    if (!bQueryPathAndName(hTmp, m_pszSrcPath, m_pszBaseName))
    {
        bCleanup();
        return kCIDLib::False;
    }

    //
    //  We don't set the portable name for externals, but the load name is the same as
    //  the base name here
    //
    m_pszLoadName = TRawStr::pszReplicate(m_pszBaseName);

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlModule::bCleanup()
{
    // Clean up any name/path storage that got set
    delete [] m_pszBaseName;
    m_pszBaseName = nullptr;
    delete [] m_pszLoadName;
    m_pszLoadName = nullptr;
    delete [] m_pszPortName;
    m_pszPortName = nullptr;
    delete [] m_pszSrcPath;
    m_pszSrcPath = nullptr;

    // Store the handle and then clean up the data
    HINSTANCE hClean = m_hmodThis.m_phmodiThis->hInstance;
    m_hmodThis.m_phmodiThis->hInstance = 0;
    m_bViaLoad = kCIDLib::False;
    m_eModType = tCIDLib::EModTypes::Count;

    // Delete any message text if we got any
    delete [] reinterpret_cast<tCIDLib::TCh*>(m_pmiThis);
    m_pmiThis = nullptr;

    //
    //  If there is a module handle and it was gotten via a load, then we need
    //  to free it.
    //
    if (m_bViaLoad && hClean)
    {
        if (!::FreeLibrary(hClean))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }
    }

    return kCIDLib::True;
}
