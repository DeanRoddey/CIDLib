//
// FILE NAME: CIDXML_DTDNotationDecl.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/07/1999
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
//  This file implements the TDTDNotationDecl class.
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
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TDTDNotationDecl,TXMLNotationDecl)



// ---------------------------------------------------------------------------
//  CLASS: TDTDNotationDecl
// PREFIX: xdecl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDTDNotationDecl: Public, static methods
// ---------------------------------------------------------------------------
const TString& TDTDNotationDecl::strExtractKey(const TDTDNotationDecl& xdeclSrc)
{
    return xdeclSrc.strName();
}


// ---------------------------------------------------------------------------
//  TDTDNotationDecl: Construtors and Destructor
// ---------------------------------------------------------------------------
TDTDNotationDecl::TDTDNotationDecl()
{
}

TDTDNotationDecl::TDTDNotationDecl(const TString& strName) :

    TXMLNotationDecl(strName)
{
}

TDTDNotationDecl::TDTDNotationDecl( const   TString&    strName
                                    , const TString&    strPublicId
                                    , const TString&    strSystemId) :

    TXMLNotationDecl(strName, strPublicId, strSystemId)
{
}

TDTDNotationDecl::TDTDNotationDecl( const   tCIDLib::TCh* const pszName
                                    , const tCIDLib::TCh* const pszPublicId
                                    , const tCIDLib::TCh* const pszSystemId) :

    TXMLNotationDecl(pszName, pszPublicId, pszSystemId)
{
}

TDTDNotationDecl::TDTDNotationDecl(const TDTDNotationDecl& xdeclToCopy) :

    TXMLNotationDecl(xdeclToCopy)
{
}

TDTDNotationDecl::~TDTDNotationDecl()
{
}


// ---------------------------------------------------------------------------
//  TDTDNotationDecl: Public operators
// ---------------------------------------------------------------------------
TDTDNotationDecl&
TDTDNotationDecl::operator=(const TDTDNotationDecl& xdeclToAssign)
{
    if (this == &xdeclToAssign)
        return *this;

    TParent::operator=(xdeclToAssign);
    return *this;
}


// ---------------------------------------------------------------------------
//  TDTDNotationDecl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDTDNotationDecl::FormatTo( TTextOutStream&         strmDest
                            , const TXMLValidator&  xvalPools) const
{
    strmDest << L"<!NOTATION " << strName() << L" ";

    if (strSystemId().bIsEmpty())
        strmDest << L"PUBLIC ";
    else
        strmDest << L"SYSTEM ";

    if (!strPublicId().bIsEmpty())
        strmDest << L"\"" << strPublicId() << L"\"";

    if (!strSystemId().bIsEmpty())
        strmDest << L" \"" << strSystemId() << L"\"";

    strmDest << L">";
}


