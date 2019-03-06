//
// FILE NAME: CIDLib_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/26/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the facility class for this facility. It is a
//  derivative (as all facility objects are) of TFacility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDLib,TFacility)



// ---------------------------------------------------------------------------
//   CLASS: TFacCIDLib
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDLib: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDLib::TFacCIDLib() :

    TFacility
    (
        L"CIDLib"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
    , m_scntThread(1)
{
}

TFacCIDLib::~TFacCIDLib()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDLib: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Return the next available numeric id from our safe counter
tCIDLib::TCard4 TFacCIDLib::c4NextId()
{
    return m_scntThread++;
}


//
// Build a new, unique thread name and return
//
TString TFacCIDLib::strNextThreadName(const tCIDLib::TCh* const pszBaseName)
{
    TString strRet(pszBaseName, 8);
    strRet.Append(L"Thread");
    strRet.AppendFormatted(m_scntThread++);

    return strRet;
}

TString TFacCIDLib::strNextThreadName(const TString& strBaseName)
{
    TString strRet(strBaseName, 8);
    strRet.Append(L"Thread");
    strRet.AppendFormatted(m_scntThread++);

    return strRet;
}

