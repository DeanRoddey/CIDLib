//
// FILE NAME: CIDXML_Attr.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/10/1999
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
//  This file implements the TXMLAttr class.
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
RTTIDecls(TXMLAttr,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TXMLAttr
// PREFIX: xattr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLAttr: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::ESortComps
TXMLAttr::eComp(const   TXMLAttr& xad1, const TXMLAttr& xad2)
{
    return xad1.strQName().eCompare(xad2.strQName());
}


// ---------------------------------------------------------------------------
//  TXMLAttr: Constructors and Destructor
// ---------------------------------------------------------------------------
TXMLAttr::TXMLAttr() :

    m_bExplicit(kCIDLib::False)
    , m_eType(tCIDXML::EAttrTypes::Count)
{
}

TXMLAttr::TXMLAttr( const   tCIDLib::TCh* const pszBaseName
                    , const tCIDLib::TCh* const pszPrefix
                    , const tCIDLib::TCh* const pszURI
                    , const tCIDLib::TCh* const pszValue
                    , const tCIDXML::EAttrTypes eType
                    , const tCIDLib::TBoolean   bExplicit) :

    m_bExplicit(bExplicit)
    , m_eType(eType)
    , m_strBaseName(pszBaseName)
    , m_strPrefix(pszPrefix)
    , m_strURI(pszURI)
    , m_strValue(pszValue)
{
    // Build up the QName
    if (!m_strPrefix.bIsEmpty())
    {
        m_strQName = pszPrefix;
        m_strQName.Append(kCIDLib::chColon);
    }
    m_strQName.Append(pszBaseName);
}

TXMLAttr::TXMLAttr( const   TString&            strBaseName
                    , const TString&            strPrefix
                    , const TString&            strURI
                    , const TString&            strValue
                    , const tCIDXML::EAttrTypes eType
                    , const tCIDLib::TBoolean   bExplicit) :

    m_bExplicit(bExplicit)
    , m_eType(eType)
    , m_strBaseName(strBaseName)
    , m_strPrefix(strPrefix)
    , m_strURI(strURI)
    , m_strValue(strValue)
{
    // Build up the QName
    if (!m_strPrefix.bIsEmpty())
    {
        m_strQName = strPrefix;
        m_strQName.Append(kCIDLib::chColon);
    }
    m_strQName.Append(strBaseName);
}

TXMLAttr::TXMLAttr(const TXMLAttr& xattrToCopy) :

    m_bExplicit(xattrToCopy.m_bExplicit)
    , m_eType(xattrToCopy.m_eType)
    , m_strBaseName(xattrToCopy.m_strBaseName)
    , m_strPrefix(xattrToCopy.m_strPrefix)
    , m_strQName(xattrToCopy.m_strQName)
    , m_strURI(xattrToCopy.m_strURI)
    , m_strValue(xattrToCopy.m_strValue)
{
}

TXMLAttr::~TXMLAttr()
{
}


// ---------------------------------------------------------------------------
//  TXMLAttr: Public operators
// ---------------------------------------------------------------------------
TXMLAttr& TXMLAttr::operator=(const TXMLAttr& xattrToAssign)
{
    if (this == &xattrToAssign)
        return *this;

    m_bExplicit     = xattrToAssign.m_bExplicit;
    m_eType         = xattrToAssign.m_eType;
    m_strBaseName   = xattrToAssign.m_strBaseName;
    m_strPrefix     = xattrToAssign.m_strPrefix;
    m_strQName      = xattrToAssign.m_strQName;
    m_strURI        = xattrToAssign.m_strURI;
    m_strValue      = xattrToAssign.m_strValue;

    return *this;
}


tCIDLib::TBoolean TXMLAttr::operator==(const TXMLAttr& xattrToComp)
{
    // As always, put the stuff most likely to be different first
    return
    (
        (m_strBaseName == xattrToComp.m_strBaseName)
        && (m_strValue  == xattrToComp.m_strValue)
        && (m_bExplicit == xattrToComp.m_bExplicit)
        && (m_eType     == xattrToComp.m_eType)
        && (m_strPrefix == xattrToComp.m_strPrefix)
        && (m_strQName  == xattrToComp.m_strQName)
        && (m_strURI    == xattrToComp.m_strURI)
    );
}

tCIDLib::TBoolean TXMLAttr::operator!=(const TXMLAttr& xattrToComp)
{
    return !operator==(xattrToComp);
}


// ---------------------------------------------------------------------------
//  TXMLAttr: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TXMLAttr::bExplicit() const
{
    return m_bExplicit;
}


tCIDXML::EAttrTypes TXMLAttr::eType() const
{
    return m_eType;
}


const TString& TXMLAttr::strBaseName() const
{
    return m_strBaseName;
}


const TString& TXMLAttr::strQName() const
{
    return m_strQName;
}


const TString& TXMLAttr::strPrefix() const
{
    return m_strPrefix;
}


const TString& TXMLAttr::strURI() const
{
    return m_strURI;
}


const TString& TXMLAttr::strValue() const
{
    return m_strValue;
}


tCIDLib::TVoid TXMLAttr::Set(const  tCIDLib::TCh* const pszBaseName
                            , const tCIDLib::TCh* const pszPrefix
                            , const tCIDLib::TCh* const pszURI
                            , const tCIDLib::TCh* const pszValue
                            , const tCIDXML::EAttrTypes eType
                            , const tCIDLib::TBoolean   bExplicit)
{
    m_bExplicit     = bExplicit;
    m_eType         = eType;
    m_strBaseName   = pszBaseName;
    m_strPrefix     = pszPrefix;
    m_strURI        = pszURI;
    m_strValue      = pszValue;

    // Build up the QName
    m_strQName.Clear();
    if (!m_strPrefix.bIsEmpty())
    {
        m_strQName = pszPrefix;
        m_strQName.Append(kCIDLib::chColon);
    }
    m_strQName.Append(pszBaseName);
}

tCIDLib::TVoid TXMLAttr::Set(const  TString&            strBaseName
                            , const TString&            strPrefix
                            , const TString&            strURI
                            , const TString&            strValue
                            , const tCIDXML::EAttrTypes eType
                            , const tCIDLib::TBoolean   bExplicit)
{
    m_bExplicit     = bExplicit;
    m_eType         = eType;
    m_strBaseName   = strBaseName;
    m_strPrefix     = strPrefix;
    m_strURI        = strURI;
    m_strValue      = strValue;

    // Build up the QName
    m_strQName.Clear();
    if (!m_strPrefix.bIsEmpty())
    {
        m_strQName = strPrefix;
        m_strQName.Append(kCIDLib::chColon);
    }
    m_strQName.Append(strBaseName);
}
