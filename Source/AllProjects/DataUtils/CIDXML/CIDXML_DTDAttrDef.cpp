//
// FILE NAME: CIDXML_DTDAttrDef.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/08/1999
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
//  This file implements the TDTDAttrDef class.
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
RTTIDecls(TDTDAttrDef,TXMLAttrDef)



// ---------------------------------------------------------------------------
//  CLASS: TDTDAttrDef
// PREFIX: xad
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDTDAttrDef: Public, static methods
// ---------------------------------------------------------------------------
const TString& TDTDAttrDef::strExtractKey(const TDTDAttrDef& xadSrc)
{
    return xadSrc.strName();
}


// ---------------------------------------------------------------------------
//  TDTDAttrDef: Public Constructors and Destructor
// ---------------------------------------------------------------------------
TDTDAttrDef::TDTDAttrDef(const  tCIDLib::TCh* const     pszName
                        , const tCIDLib::TCh* const     pszValue
                        , const tCIDXML::EAttrTypes     eType
                        , const tCIDXML::EDefAttrTypes  eDefaultType) :

    TXMLAttrDef(pszValue, eType, eDefaultType)
    , m_strName(pszName)
{
}

TDTDAttrDef::TDTDAttrDef(const  TString&                strName
                        , const TString&                strValue
                        , const tCIDXML::EAttrTypes     eType
                        , const tCIDXML::EDefAttrTypes  eDefaultType) :

    TXMLAttrDef(strValue, eType, eDefaultType)
    , m_strName(strName)
{
}

TDTDAttrDef::TDTDAttrDef(const tCIDLib::TCh* const pszName) :

    TXMLAttrDef()
    , m_strName(pszName)
{
}

TDTDAttrDef::TDTDAttrDef(const TString& strName) :

    TXMLAttrDef()
    , m_strName(strName)
{
}

TDTDAttrDef::TDTDAttrDef(const TDTDAttrDef& xadToCopy) :

    TXMLAttrDef(xadToCopy)
    , m_strName(xadToCopy.m_strName)
{
}

TDTDAttrDef::~TDTDAttrDef()
{
}


// ---------------------------------------------------------------------------
//  TDTDAttrDef: Public operators
// ---------------------------------------------------------------------------
TDTDAttrDef& TDTDAttrDef::operator=(const TDTDAttrDef& xadToCopy)
{
    if (this == &xadToCopy)
        return *this;

    TXMLAttrDef::operator=(xadToCopy);
    m_strName = xadToCopy.m_strName;

    return *this;
}


// ---------------------------------------------------------------------------
//  TDTDAttrDef: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDTDAttrDef::FormatTo(TTextOutStream& strmDest, const TXMLValidator&) const
{
    strmDest << m_strName << L" ";

    if ((eType() == tCIDXML::EAttrTypes::Enumeration)
    ||  (eType() == tCIDXML::EAttrTypes::Notation))
    {
        if (eType() == tCIDXML::EAttrTypes::Notation)
            strmDest << eType();

        // Now output the parenthesized list of values
        strmDest << L"(";

        TXMLAttrDef::TCursor cursValues = cursEnumValues();
        tCIDLib::TCard4 c4Count = 0;
        for (; cursValues; ++cursValues)
        {
            if (c4Count)
                strmDest << L"|";
            strmDest << *cursValues;
            c4Count++;
        }
        strmDest << L")";
    }
     else
    {
        // We can just output the text for the enum
        strmDest << eType();
    }

    if (eDefaultType() == tCIDXML::EDefAttrTypes::Default)
        strmDest << L" \"" << strValue() << L"\"";
    else
        strmDest << L" " << eDefaultType();
}


const TString& TDTDAttrDef::strFullName() const
{
    return m_strName;
}


// ---------------------------------------------------------------------------
//  TDTDAttrDef: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TString& TDTDAttrDef::strName() const
{
    return m_strName;
}

const TString& TDTDAttrDef::strName(const TString& strName)
{
    m_strName = strName;
    return m_strName;
}

const TString& TDTDAttrDef::strName(const tCIDLib::TCh* const pszName)
{
    m_strName = pszName;
    return m_strName;
}


