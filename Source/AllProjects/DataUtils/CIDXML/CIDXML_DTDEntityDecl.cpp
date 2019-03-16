//
// FILE NAME: CIDXML_DTDElementDecl.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/31/1999
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
//  This file implements the TDTDElementDecl class.
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
#include    "CIDXML_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TDTDEntityDecl,TXMLEntityDecl)


// ---------------------------------------------------------------------------
//  CLASS: TDTDEntityDecl
// PREFIX: xdecl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDTDEntityDecl: Public, static methods
// ---------------------------------------------------------------------------
const TString& TDTDEntityDecl::strExtractKey(const TDTDEntityDecl& xdeclSrc)
{
    return xdeclSrc.strName();
}


// ---------------------------------------------------------------------------
//  TDTDEntityDecl: Constructors and Destructor
// ---------------------------------------------------------------------------
TDTDEntityDecl::TDTDEntityDecl(const TString& strEntityName) :

    TXMLEntityDecl(strEntityName)
    , m_bFromIntSS(kCIDLib::False)
    , m_bIsPE(kCIDLib::False)
{
}

TDTDEntityDecl::TDTDEntityDecl( const   TString&    strEntityName
                                , const TString&    strEntityValue) :

    TXMLEntityDecl(strEntityName, strEntityValue)
    , m_bFromIntSS(kCIDLib::False)
    , m_bIsPE(kCIDLib::False)
{
}

TDTDEntityDecl::TDTDEntityDecl(const tCIDLib::TCh* const pszEntityName) :

    TXMLEntityDecl(pszEntityName)
    , m_bFromIntSS(kCIDLib::False)
    , m_bIsPE(kCIDLib::False)
{
}

TDTDEntityDecl::TDTDEntityDecl( const   tCIDLib::TCh* const pszEntityName
                                , const tCIDLib::TCh* const pszEntityValue) :

    TXMLEntityDecl(pszEntityName, pszEntityValue)
    , m_bFromIntSS(kCIDLib::False)
    , m_bIsPE(kCIDLib::False)
{
}

TDTDEntityDecl::TDTDEntityDecl(const TDTDEntityDecl& xdeclToCopy) :

    TXMLEntityDecl(xdeclToCopy)
    , m_bFromIntSS(xdeclToCopy.m_bFromIntSS)
    , m_bIsPE(xdeclToCopy.m_bIsPE)
{
}


TDTDEntityDecl::~TDTDEntityDecl()
{
}


// ---------------------------------------------------------------------------
//  TDTDEntityDecl: Public operators
// ---------------------------------------------------------------------------
TDTDEntityDecl& TDTDEntityDecl::operator=(const TDTDEntityDecl& xdeclToAssign)
{
    if (this == &xdeclToAssign)
        return *this;

    TXMLEntityDecl::operator=(xdeclToAssign);

    m_bFromIntSS    = xdeclToAssign.m_bFromIntSS;
    m_bIsPE         = xdeclToAssign.m_bIsPE;

    return *this;
}



// ---------------------------------------------------------------------------
//  TDTDEntityDecl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDTDEntityDecl::FormatTo(TTextOutStream& strmDest, const TXMLValidator&) const
{
    strmDest << L"<!ENTITY ";

    if (bIsPE())
        strmDest << L"% ";

    strmDest << strName() << L" ";

    if (bIsInternal())
    {
        strmDest << L"\"" << strValue() << L"\"";
    }
     else
    {
        if (!strPublicId().bIsEmpty())
        {
            strmDest << L"PUBLIC \""
                     << strPublicId() << L"\" \""
                     << strSystemId() << L"\"";
        }
         else
        {
            strmDest << L"SYSTEM \"" << strSystemId() << L"\"";
        }
    }

    if (!strNotationName().bIsEmpty())
        strmDest << L" NDATA " << strNotationName();

    strmDest << L">";
}


// ---------------------------------------------------------------------------
//  TDTDEntityDecl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TDTDEntityDecl::bFromIntSS() const
{
    return m_bFromIntSS;
}

tCIDLib::TBoolean TDTDEntityDecl::bFromIntSS(const tCIDLib::TBoolean bNewValue)
{
    m_bFromIntSS = bNewValue;
    return m_bFromIntSS;
}

tCIDLib::TBoolean TDTDEntityDecl::bIsPE() const
{
    return m_bIsPE;
}

tCIDLib::TBoolean TDTDEntityDecl::bIsPE(const tCIDLib::TBoolean bNewValue)
{
    m_bIsPE = bNewValue;
    return m_bIsPE;
}

