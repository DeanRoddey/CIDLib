//
// FILE NAME: CIDXML_AttDef.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/08/1999
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
//  This file implements the TXMLAttrDef class.
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
RTTIDecls(TXMLAttrDef,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TXMLAttrDef
// PREFIX: xad
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TXMLAttrDef: Public destructors
// ---------------------------------------------------------------------------
TXMLAttrDef::~TXMLAttrDef()
{
    delete m_pcolEnumValues;
}


// ---------------------------------------------------------------------------
//  TXMLAttrDef: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TXMLAttrDef::bIsInEnumList(const tCIDLib::TCh* const pszToCheck) const
{
    // If not collection of enum values, then definitely not
    if (!m_pcolEnumValues)
        return kCIDLib::False;

    // Check it against the list of values
    TCursor cursList(m_pcolEnumValues);
    for (; cursList; ++cursList)
    {
        if (*cursList == pszToCheck)
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TXMLAttrDef::bIsInEnumList(const TString& strToCheck) const
{
    return bIsInEnumList(strToCheck.pszBuffer());
}


tCIDLib::TBoolean TXMLAttrDef::bProvided() const
{
    return m_bProvided;
}


tCIDLib::TBoolean TXMLAttrDef::bMultiValued() const
{
    return (m_eType == tCIDXML::EAttrTypes::IDRefs)
         | (m_eType == tCIDXML::EAttrTypes::Entities)
         | (m_eType == tCIDXML::EAttrTypes::NmTokens)
         | (m_eType == tCIDXML::EAttrTypes::Notation)
         | (m_eType == tCIDXML::EAttrTypes::Enumeration);
}


tCIDLib::TBoolean TXMLAttrDef::bNamedType() const
{
    return (m_eType == tCIDXML::EAttrTypes::ID)
         | (m_eType == tCIDXML::EAttrTypes::IDRef)
         | (m_eType == tCIDXML::EAttrTypes::IDRefs)
         | (m_eType == tCIDXML::EAttrTypes::Entity)
         | (m_eType == tCIDXML::EAttrTypes::Entities)
         | (m_eType == tCIDXML::EAttrTypes::Notation);
}


tCIDLib::TBoolean TXMLAttrDef::bProvided(const tCIDLib::TBoolean bNewValue)
{
    m_bProvided = bNewValue;
    return m_bProvided;
}


tCIDLib::TBoolean TXMLAttrDef::bRefType() const
{
    return (m_eType == tCIDXML::EAttrTypes::ID)
         | (m_eType == tCIDXML::EAttrTypes::IDRef)
         | (m_eType == tCIDXML::EAttrTypes::IDRefs);
}


tCIDLib::TCard4 TXMLAttrDef::c4Id() const
{
    return m_c4Id;
}


TXMLAttrDef::TCursor TXMLAttrDef::cursEnumValues() const
{
    //
    //  If this one is not a notation or enumeration, then this is not
    //  legal.
    //
    if ((m_eType != tCIDXML::EAttrTypes::Notation)
    &&  (m_eType != tCIDXML::EAttrTypes::Enumeration))
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcAttr_NotEnumType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // If the collection doesn't exist, the value was never set
    if (!m_pcolEnumValues)
    {
        facCIDXML().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kXMLErrs::errcAttr_EnumValsNeverSet
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // And return a cursor for it
    return TCursor(m_pcolEnumValues);
}


tCIDLib::TCard4 TXMLAttrDef::c4Id(const tCIDLib::TCard4 c4Id)
{
    m_c4Id = c4Id;
    return m_c4Id;
}


tCIDXML::EDefAttrTypes TXMLAttrDef::eDefaultType() const
{
    return m_eDefType;
}


tCIDXML::EDefAttrTypes
TXMLAttrDef::eDefaultType(const tCIDXML::EDefAttrTypes eNewType)
{
    m_eDefType = eNewType;
    return m_eDefType;
}


tCIDXML::EAttrTypes TXMLAttrDef::eType() const
{
    return m_eType;
}

tCIDXML::EAttrTypes
TXMLAttrDef::eType(const tCIDXML::EAttrTypes eNewType)
{
    m_eType = eNewType;
    return m_eType;
}


const TString& TXMLAttrDef::strValue() const
{
    return m_strValue;
}

const TString& TXMLAttrDef::strValue(const TString& strNewValue)
{
    // Store the new value string
    m_strValue = strNewValue;
    return m_strValue;
}


tCIDLib::TVoid TXMLAttrDef::SetEnumValues(const TString& strValueList)
{
    //
    //  Construct the bag we use to hold the values, if it does not already
    //  exist. If it does, then flush it.
    //
    if (m_pcolEnumValues)
        m_pcolEnumValues->RemoveAll();
    else
        m_pcolEnumValues = new TBag<TString>;

    //
    //  We need to break out the values. We know that they are already
    //  checked for validity and mormalized, so this isn't too hard. It will
    //  be a sequence of name tokens separated by | characters and spaces. So
    //  we can just use a string tokenizer.
    //
    TStringTokenizer stokValue(&strValueList, L" |");
    TString          strToken;
    while (stokValue.bGetNextToken(strToken))
        m_pcolEnumValues->objAdd(strToken);
}


// ---------------------------------------------------------------------------
//  TXMLAttrDef: Hidden constructors and operators
// ---------------------------------------------------------------------------
TXMLAttrDef::TXMLAttrDef() :

    m_bProvided(kCIDLib::False)
    , m_c4Id(kCIDLib::c4MaxCard)
    , m_eDefType(tCIDXML::EDefAttrTypes::Implied)
    , m_eType(tCIDXML::EAttrTypes::CData)
    , m_pcolEnumValues(nullptr)
{
}

TXMLAttrDef::TXMLAttrDef(const  tCIDLib::TCh* const     pszValue
                        , const tCIDXML::EAttrTypes     eType
                        , const tCIDXML::EDefAttrTypes  eDefaultType) :

    m_bProvided(kCIDLib::False)
    , m_c4Id(kCIDLib::c4MaxCard)
    , m_eDefType(eDefaultType)
    , m_eType(eType)
    , m_pcolEnumValues(nullptr)
    , m_strValue(pszValue)
{
}

TXMLAttrDef::TXMLAttrDef(const  TString&                strValue
                        , const tCIDXML::EAttrTypes     eType
                        , const tCIDXML::EDefAttrTypes  eDefaultType) :

    m_bProvided(kCIDLib::False)
    , m_c4Id(kCIDLib::c4MaxCard)
    , m_eDefType(eDefaultType)
    , m_eType(eType)
    , m_pcolEnumValues(nullptr)
    , m_strValue(strValue)
{
}

TXMLAttrDef::TXMLAttrDef(const TXMLAttrDef& xadSrc) :

    m_bProvided(kCIDLib::False)
    , m_c4Id(kCIDLib::c4MaxCard)
    , m_eDefType(xadSrc.m_eDefType)
    , m_eType(xadSrc.m_eType)
    , m_pcolEnumValues(nullptr)
    , m_strValue(xadSrc.m_strValue)
{
    // If the source has enumerated values, then copy them
    if (xadSrc.m_pcolEnumValues)
        m_pcolEnumValues = new TBag<TString>(*xadSrc.m_pcolEnumValues);
}

TXMLAttrDef::TXMLAttrDef(TXMLAttrDef&& xadSrc) :

    TXMLAttrDef()
{
    *this = operator=(tCIDLib::ForceMove(xadSrc));
}

TXMLAttrDef& TXMLAttrDef::operator=(const TXMLAttrDef& xadSrc)
{
    if (this != &xadSrc)
    {
        m_bProvided = xadSrc.m_bProvided;
        m_eDefType  = xadSrc.m_eDefType;
        m_eType     = xadSrc.m_eType;
        m_strValue  = xadSrc.m_strValue;

        // The id is assigned by the container, so reset it on copy or assign
        m_c4Id = kCIDLib::c4MaxCard;

        //
        //  If the source has enumerated values, then copy them. We may have to
        //  fault in our collection. If it does not, and we have one, then flush
        //  it.
        //
        if (xadSrc.m_pcolEnumValues)
        {
            if (!m_pcolEnumValues)
                m_pcolEnumValues = new TBag<TString>(*xadSrc.m_pcolEnumValues);
            else
                *m_pcolEnumValues = *xadSrc.m_pcolEnumValues;
        }
         else
        {
            if (m_pcolEnumValues)
                m_pcolEnumValues->RemoveAll();
        }
    }
    return *this;
}

TXMLAttrDef& TXMLAttrDef::operator=(TXMLAttrDef&& xadSrc)
{
    if (this != &xadSrc)
    {
        tCIDLib::Swap(m_bProvided, xadSrc.m_bProvided);
        tCIDLib::Swap(m_c4Id, xadSrc.m_c4Id);
        tCIDLib::Swap(m_eDefType, xadSrc.m_eDefType);
        tCIDLib::Swap(m_eType, xadSrc.m_eType);
        tCIDLib::Swap(m_pcolEnumValues, xadSrc.m_pcolEnumValues);
        m_strValue = tCIDLib::ForceMove(xadSrc.m_strValue);
    }
    return *this;
}


