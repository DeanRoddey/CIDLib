//
// FILE NAME: CIDXML_EntityDecl.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/1999
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
//  This file implements the TXMLEntityDecl class.
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
RTTIDecls(TXMLEntityDecl,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TXMLEntityDecl
// PREFIX: xdecl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLEntityDecl: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLEntityDecl::~TXMLEntityDecl()
{
}


// ---------------------------------------------------------------------------
//  TXMLEntityDecl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TXMLEntityDecl::bIsExternal() const
{
    // If it has a system id, its an external entity
    return !m_strSystemId.bIsEmpty();
}

tCIDLib::TBoolean TXMLEntityDecl::bIsInternal() const
{
    // If it has a system id, its not an internal entity
    return m_strSystemId.bIsEmpty();
}

tCIDLib::TBoolean TXMLEntityDecl::bIsSpecial() const
{
    return m_bSpecial;
}

tCIDLib::TBoolean
TXMLEntityDecl::bIsSpecial(const tCIDLib::TBoolean bNewValue)
{
    m_bSpecial = bNewValue;
    return m_bSpecial;
}

tCIDLib::TBoolean TXMLEntityDecl::bIsUnparsed() const
{
    // If we have a notation, its not a parsed entity
    return !m_strNotationName.bIsEmpty();
}

tCIDLib::TCard4 TXMLEntityDecl::c4Id() const
{
    return m_c4Id;
}

tCIDLib::TCard4 TXMLEntityDecl::c4Id(const tCIDLib::TCard4 c4NewId)
{
    m_c4Id = c4NewId;
    return m_c4Id;
}

const TString& TXMLEntityDecl::strName() const
{
    return m_strName;
}

const TString& TXMLEntityDecl::strName(const TString& strName)
{
    m_strName = strName;
    return m_strName;
}

const TString& TXMLEntityDecl::strName(const tCIDLib::TCh* const pszName)
{
    m_strName = pszName;
    return m_strName;
}

const TString& TXMLEntityDecl::strValue() const
{
    return m_strValue;
}

const TString& TXMLEntityDecl::strValue(const TString& strValue)
{
    m_strValue = strValue;
    return m_strValue;
}

const TString& TXMLEntityDecl::strValue(const tCIDLib::TCh* const pszValue)
{
    m_strValue = pszValue;
    return m_strValue;
}

const TString& TXMLEntityDecl::strNotationName() const
{
    return m_strNotationName;
}

const TString& TXMLEntityDecl::strNotationName(const TString& strName)
{
    m_strNotationName = strName;
    return m_strNotationName;
}

const TString&
TXMLEntityDecl::strNotationName(const tCIDLib::TCh* const pszName)
{
    m_strNotationName = pszName;
    return m_strNotationName;
}

const TString& TXMLEntityDecl::strPublicId() const
{
    return m_strPublicId;
}

const TString& TXMLEntityDecl::strPublicId(const TString& strId)
{
    m_strPublicId = strId;
    return m_strPublicId;
}

const TString& TXMLEntityDecl::strPublicId(const tCIDLib::TCh* const pszId)
{
    m_strPublicId = pszId;
    return m_strPublicId;
}

const TString& TXMLEntityDecl::strSystemId() const
{
    return m_strSystemId;
}

const TString& TXMLEntityDecl::strSystemId(const TString& strId)
{
    m_strSystemId = strId;
    return m_strSystemId;
}

const TString& TXMLEntityDecl::strSystemId(const tCIDLib::TCh* const pszId)
{
    m_strSystemId = pszId;
    return m_strSystemId;
}



// ---------------------------------------------------------------------------
//  TXMLEntityDecl: Hidden constructors and operators
// ---------------------------------------------------------------------------
TXMLEntityDecl::TXMLEntityDecl() :

    m_bSpecial(kCIDLib::False)
    , m_c4Id(0)
    , m_c4ValueLen(0)
{
}

TXMLEntityDecl::TXMLEntityDecl(const TString& strEntityName) :

    m_bSpecial(kCIDLib::False)
    , m_c4Id(0)
    , m_c4ValueLen(0)
    , m_strName(strEntityName)
{
}

TXMLEntityDecl::TXMLEntityDecl( const   TString&    strEntityName
                                , const TString&    strEntityValue) :

    m_bSpecial(kCIDLib::False)
    , m_c4Id(0)
    , m_c4ValueLen(0)
    , m_strName(strEntityName)
    , m_strValue(strEntityValue)
{
}

TXMLEntityDecl::TXMLEntityDecl(const tCIDLib::TCh* const pszEntityName) :

    m_bSpecial(kCIDLib::False)
    , m_c4Id(0)
    , m_c4ValueLen(0)
    , m_strName(pszEntityName)
{
}

TXMLEntityDecl::TXMLEntityDecl( const   tCIDLib::TCh* const pszEntityName
                                , const tCIDLib::TCh* const pszEntityValue) :

    m_bSpecial(kCIDLib::False)
    , m_c4Id(0)
    , m_c4ValueLen(0)
    , m_strName(pszEntityName)
    , m_strValue(pszEntityValue)
{
}

