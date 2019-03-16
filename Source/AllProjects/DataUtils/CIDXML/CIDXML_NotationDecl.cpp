//
// FILE NAME: CIDXML_NotationDecl.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/06/1999
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
//  This file implements the TXMLNotationDecl class.
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
RTTIDecls(TXMLNotationDecl,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TXMLNotationDecl
// PREFIX: xdecl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLNotationDecl: Destructor
// ---------------------------------------------------------------------------
TXMLNotationDecl::~TXMLNotationDecl()
{
}


// ---------------------------------------------------------------------------
//  TXMLNotationDecl: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLNotationDecl::TXMLNotationDecl() :

    m_c4Id(0)
{
}

TXMLNotationDecl::TXMLNotationDecl(const TXMLNotationDecl& xdeclToCopy) :

    m_c4Id(xdeclToCopy.m_c4Id)
    , m_strName(xdeclToCopy.m_strName)
    , m_strPublicId(xdeclToCopy.m_strPublicId)
    , m_strSystemId(xdeclToCopy.m_strSystemId)
{
}

TXMLNotationDecl::TXMLNotationDecl( const   TString&    strName
                                    , const TString&    strPublicId
                                    , const TString&    strSystemId) :
    m_c4Id(0)
    , m_strName(strName)
    , m_strPublicId(strPublicId)
    , m_strSystemId(strSystemId)
{
}

TXMLNotationDecl::TXMLNotationDecl(const TString& strName) :

    m_c4Id(0)
    , m_strName(strName)
{
}

TXMLNotationDecl::TXMLNotationDecl( const   tCIDLib::TCh* const pszName
                                    , const tCIDLib::TCh* const pszPublicId
                                    , const tCIDLib::TCh* const pszSystemId) :
    m_c4Id(0)
    , m_strName(pszName)
    , m_strPublicId(pszPublicId)
    , m_strSystemId(pszSystemId)
{
}

TXMLNotationDecl&
TXMLNotationDecl::operator=(const TXMLNotationDecl& xdeclToAssign)
{
    if (this == &xdeclToAssign)
        return *this;

    m_c4Id          = xdeclToAssign.m_c4Id;
    m_strName       = xdeclToAssign.m_strName;
    m_strPublicId   = xdeclToAssign.m_strPublicId;
    m_strSystemId   = xdeclToAssign.m_strSystemId;

    return *this;
}


// ---------------------------------------------------------------------------
//  TXMLNotationDecl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TXMLNotationDecl::c4Id() const
{
    return m_c4Id;
}

tCIDLib::TCard4 TXMLNotationDecl::c4Id(const tCIDLib::TCard4 c4NewId)
{
    m_c4Id = c4NewId;
    return m_c4Id;
}


const TString& TXMLNotationDecl::strName() const
{
    return m_strName;
}

const TString& TXMLNotationDecl::strName(const TString& strName)
{
    m_strName = strName;
    return m_strName;
}

const TString& TXMLNotationDecl::strName(const tCIDLib::TCh* const pszName)
{
    m_strName = pszName;
    return m_strName;
}


const TString& TXMLNotationDecl::strPublicId() const
{
    return m_strPublicId;
}

const TString& TXMLNotationDecl::strPublicId(const TString& strId)
{
    m_strPublicId = strId;
    return m_strPublicId;
}

const TString& TXMLNotationDecl::strPublicId(const tCIDLib::TCh* const pszId)
{
    m_strPublicId = pszId;
    return m_strPublicId;
}


const TString& TXMLNotationDecl::strSystemId() const
{
    return m_strSystemId;
}

const TString& TXMLNotationDecl::strSystemId(const TString& strId)
{
    m_strSystemId = strId;
    return m_strSystemId;
}

const TString& TXMLNotationDecl::strSystemId(const tCIDLib::TCh* const pszId)
{
    m_strSystemId = pszId;
    return m_strSystemId;
}

