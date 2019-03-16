//
// FILE NAME: CIDKernel_Module_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/12/1998
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
//   CLASS: TModuleHandle
//  PREFIX: hmod
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TModuleHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TModuleHandle::TModuleHandle() :

    m_phmodiThis(0)
{
    m_phmodiThis = new TModuleHandleImpl;
    m_phmodiThis->hInstance = 0;
}

TModuleHandle::TModuleHandle(const TModuleHandle& hmodToCopy) :

    m_phmodiThis(0)
{
    m_phmodiThis = new TModuleHandleImpl;
    m_phmodiThis->hInstance = hmodToCopy.m_phmodiThis->hInstance;
}

TModuleHandle::~TModuleHandle()
{
    delete m_phmodiThis;
    m_phmodiThis = 0;
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
    return (m_phmodiThis->hInstance != 0);
}


tCIDLib::TVoid TModuleHandle::Clear()
{
    m_phmodiThis->hInstance = 0;
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
tCIDLib::TBoolean
TKrnlModule::bBuildRawModName(  const   tCIDLib::TCh* const pszBaseName
                                , const tCIDLib::TCard4     c4MajVersion
                                , const tCIDLib::TCard4     c4MinVersion
                                , const tCIDLib::EModTypes  eModType
                                , const tCIDLib::TCard4     c4MaxChars
                                ,       tCIDLib::TCh* const pchToFill)
{
    // We have a helper method to handle this work
    TKrnlWin32::BuildModName
    (
        pchToFill
        , c4MaxChars
        , pszBaseName
        , c4MajVersion
        , c4MinVersion
        , eModType
    );

    // This doesn't add the extension, so lets do that now
    if (eModType == tCIDLib::EModTypes::Exe)
        TRawStr::CatStr(pchToFill, kCIDLib::szExeExtension, c4MaxChars);
    else
        TRawStr::CatStr(pchToFill, kCIDLib::szLibExtension, c4MaxChars);
    return kCIDLib::True;
}


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
    tCIDLib::TCh szTmpBuf[kCIDLib::c4MaxPathLen+1];

    if (!::GetModuleFileName
    (
        hmodSrc.m_phmodiThis->hInstance
        , szTmpBuf
        , kCIDLib::c4MaxPathLen))
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


// ---------------------------------------------------------------------------
//  TKrnlModule: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlModule::TKrnlModule() :

    m_bViaLoad(kCIDLib::False)
    , m_pmiThis(0)
{
}

TKrnlModule::~TKrnlModule()
{
    // Delete the message file buffer if any
    delete [] reinterpret_cast<tCIDLib::TCard1*>(m_pmiThis);
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
    pfnToFill = ::GetProcAddress
    (
        m_hmodThis.m_phmodiThis->hInstance
        , pszFuncName
    );

    return (pfnToFill != 0);
}


tCIDLib::TBoolean
TKrnlModule::bLoadFromName( const   tCIDLib::TCh* const pszModName
                            , const tCIDLib::EModTypes  eModType
                            , const tCIDLib::TCard4     c4MajVer
                            , const tCIDLib::TCard4     c4MinVer
                            , const tCIDLib::EModFlags  eModFlags)
{
    // Clear the current handle if any
    if (!bClearHandle())
        return kCIDLib::False;

    //
    //  Ok, lets build up the name of the DLL or Exe we are dealing with
    //  here. First, we'll call a local to build up the name. This appends
    //  the standard version number info if its a DLL type. Or it will append
    //  the Exe extension if its an Exe.
    //
    //  If the passed name is fully qualified, then we don't do any of this
    //  and just copy it to the buffer.
    //
    const tCIDLib::TCard4 c4BufSize = 2047;
    tCIDLib::TCh szNameBuf[c4BufSize + 1];
    if (TRawStr::pszFindChar(pszModName, L'\\'))
    {
        TRawStr::CopyStr(szNameBuf, pszModName, c4BufSize);
    }
     else
    {
        TKrnlWin32::BuildModName
        (
            szNameBuf
            , c4BufSize
            , pszModName
            , c4MajVer
            , c4MinVer
            , eModType
        );
    }

    // And now try to load the library
    HINSTANCE hTmp = ::LoadLibrary(szNameBuf);

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
TKrnlModule::bLoadFromName(const tCIDLib::TCh* const pszFullPath)
{
    // Clear the current handle if any
    if (!bClearHandle())
        return kCIDLib::False;

    // And now try to load the library
    HINSTANCE hTmp = ::LoadLibrary(pszFullPath);

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

    // Clean up messages, since this one cannot have any
    delete [] reinterpret_cast<tCIDLib::TCh*>(m_pmiThis);
    m_pmiThis = 0;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlModule::bQueryFromName(const   tCIDLib::TCh* const pszModName
                            , const tCIDLib::EModTypes  eModType
                            , const tCIDLib::TCard4     c4MajVer
                            , const tCIDLib::TCard4     c4MinVer
                            , const tCIDLib::EModFlags  eModFlags)
{
    // Clear the previous handle, if any
    if (!bClearHandle())
        return kCIDLib::False;

    //
    //  Ok, lets build up the name of the DLL or Exe we are dealing with
    //  here. First, we'll call a local to build up the name. This appends
    //  the standard version number info if its a DLL type. Or it will append
    //  the Exe extension if its an Exe.
    //
    //  If the passed name is fully qualified, then we don't do any of this
    //  and just copy it to the buffer.
    //
    const tCIDLib::TCard4 c4BufSize = 2047;
    tCIDLib::TCh szNameBuf[c4BufSize + 1];
    if (TRawStr::pszFindChar(pszModName, L'\\'))
    {
        TRawStr::CopyStr(szNameBuf, pszModName, c4BufSize);
    }
     else
    {
        TKrnlWin32::BuildModName
        (
            szNameBuf
            , c4BufSize
            , pszModName
            , c4MajVer
            , c4MinVer
            , eModType
        );
    }

    // And now try to look up the name
    HINSTANCE hTmp = ::GetModuleHandle(szNameBuf);

    // If it failed, return the error with our handle stuff still cleared
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
TKrnlModule::bQueryFromName(const tCIDLib::TCh* const pszModName)
{
    // Clear the previous handle, if any
    if (!bClearHandle())
        return kCIDLib::False;

    // And now try to look up the name
    HINSTANCE hTmp = ::GetModuleHandle(pszModName);

    // If it failed, return the error with our handle stuff still cleared
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

    // Clean up messages, since this guy cannot have any
    delete [] reinterpret_cast<tCIDLib::TCh*>(m_pmiThis);
    m_pmiThis = 0;

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlModule::bClearHandle()
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

    m_hmodThis.m_phmodiThis->hInstance = 0;
    m_bViaLoad = kCIDLib::False;

    return kCIDLib::True;
}
