//
// FILE NAME: CIDLib_Enum.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/08/2018
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
//  This file provides supporting code for IDL generated enumeration code.
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


//
//  A helper to generate the actual enum value name from the binary enum value. The
//  IDL compiler needs this to generate the code that references the text values
//  it gets from the XML.
//
const TString& TEnumMap::strFormatEnumTextVals(const ETextVals eVal)
{
    static const TString astrText[tCIDLib::c4EnumOrd(ETextVals::Count)] =
    {
        L"TEnumMap::ETextVals::None"
        , L"TEnumMap::ETextVals::AltText"
        , L"TEnumMap::ETextVals::AltText2"
        , L"TEnumMap::ETextVals::BaseName"
        , L"TEnumMap::ETextVals::Name"
        , L"TEnumMap::ETextVals::Text"
    };

    CIDAssert(eVal < ETextVals::Count, L"Invalid enum text values");
    return astrText[tCIDLib::c4EnumOrd(eVal)];
}
static const TString strUnknownEnumValText(L"???");


//
//  We use this internally but some code that is too low level to use the IDL system
//  (and has to do translation manually) may use it.
//
tCIDLib::TVoid
TEnumMap::ThrowBadEnumVal(const TString& strType, const tCIDLib::TInt4 i4Val)
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcGen_BadEnumValue
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Range
        , TInteger(i4Val)
        , strType
    );
}




// ---------------------------------------------------------------------------
//  TEnumMap: Constructors and destructor
// ---------------------------------------------------------------------------
TEnumMap::TEnumMap( const   TString&            strTypeName
                    , const tCIDLib::TCard4     c4ValCount
                    , const tCIDLib::TBoolean   bNonContig
                    ,       TEnumValItem        aeitemValues[]
                    , const TFacility* const    pfacLoad
                    , const tCIDLib::TInt4      i4NotFoundVal) :

    m_bNonContig(bNonContig)
    , m_c4ValCount(c4ValCount)
    , m_i4NotFoundVal(i4NotFoundVal)
    , m_aeitemValues(aeitemValues)
    , m_pfacLoad(pfacLoad)
    , m_strTypeName(strTypeName)
{
}

TEnumMap::~TEnumMap()
{
}


// ---------------------------------------------------------------------------
//  TEnumMap: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Return whether the passed value is legal for this enum
tCIDLib::TBoolean TEnumMap::bIsValidEnum(const tCIDLib::TInt4 i4ToFind)
{
    return c4MapEnumVal(i4ToFind, kCIDLib::False) != kCIDLib::c4MaxCard;
}


tCIDLib::TCard4
TEnumMap::c4MapEnumVal( const tCIDLib::TInt4        i4ToFind
                        , const tCIDLib::TBoolean   bThrowIfNot)
{
    tCIDLib::TCard4 c4Index = 0;
    if (m_bNonContig)
    {
        while (c4Index < m_c4ValCount)
        {
            if (m_aeitemValues[c4Index].m_i4Value == i4ToFind)
                break;
            c4Index++;
        }
    }
     else
    {
        if (tCIDLib::TCard4(i4ToFind) >= m_c4ValCount)
            c4Index = m_c4ValCount;
        else
            c4Index = tCIDLib::TCard4(i4ToFind);
    }
    if (c4Index == m_c4ValCount)
    {
        if (bThrowIfNot)
            ThrowBadEnumVal(m_strTypeName, i4ToFind);
        return kCIDLib::c4MaxCard;
    }
    return c4Index;
}

//
//  For searching an enum map for a specific text value. In this case, it
//  has to be used for contiguous and non-contiguous enums. It returns either
//  the index of the match, or it either throws or returns c4MaxCard.
//
tCIDLib::TCard4
TEnumMap::c4MapEnumText(const   TString&            strToFind
                        , const ETextVals           eTextVal
                        , const tCIDLib::TBoolean   bThrowIfNot)
{
    // Fault in loadable text if needed
    if (m_pfacLoad && (eTextVal == ETextVals::Text))
        LoadResText();

    tCIDLib::TCard4 c4Index = 0;
    while (c4Index < m_c4ValCount)
    {
        const TEnumValItem& aeitemCur = m_aeitemValues[c4Index];
        if (aeitemCur.m_astrTextVals[tCIDLib::c4EnumOrd(eTextVal)].bCompareI(strToFind))
            break;
        c4Index++;
    }
    if (c4Index == m_c4ValCount)
    {
        if (bThrowIfNot)
            ThrowBadEnumText(m_strTypeName, strToFind);
        return kCIDLib::c4MaxCard;
    }
    return c4Index;
}


//
//  Finds an enum value by the alt numeric value. If it finds a match, it returns
//  the enum value. We use an 'e' prefix even though this guy returns an Int4 because
//  otherwise, we'd have the same name as the one that maps the other direction.
//
tCIDLib::TInt4
TEnumMap::eMapEnumAltNum(const  tCIDLib::TInt4      i4AltToFind
                        , const tCIDLib::TBoolean   bThrowIfNot)
{
    tCIDLib::TCard4 c4Index = 0;
    while (c4Index < m_c4ValCount)
    {
        if (m_aeitemValues[c4Index].m_i4AltNumVal == i4AltToFind)
            break;
        c4Index++;
    }
    if (c4Index == m_c4ValCount)
    {
        if (bThrowIfNot)
            ThrowBadEnumAltNum(m_strTypeName, i4AltToFind);
        return m_i4NotFoundVal;
    }
    return m_aeitemValues[c4Index].m_i4Value;
}


// Supports the FormatMap functionality of IDL enums
tCIDLib::TVoid
TEnumMap::FormatValues(         TString&        strToFill
                        , const TString&        strPrefix
                        , const tCIDLib::TCh    chSepChar
                        , const ETextVals       eTextVal)
{
    // Fault in loadable text if needed
    if (m_pfacLoad && (eTextVal == ETextVals::Text))
        LoadResText();

    strToFill = strPrefix;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4ValCount; c4Index++)
    {
        if (c4Index)
            strToFill.Append(chSepChar);
        strToFill.Append(m_aeitemValues[c4Index].m_astrTextVals[tCIDLib::c4EnumOrd(eTextVal)]);
    }
}


//
//  Returns the alt numeric value for the indicicated enumerated value.
//
tCIDLib::TInt4
TEnumMap::i4MapEnumAltNum(  const   tCIDLib::TInt4      i4EnumToFind
                            , const tCIDLib::TBoolean   bThrowIfNot)
{
    tCIDLib::TCard4 c4Index = 0;
    if (m_bNonContig)
    {
        while (c4Index < m_c4ValCount)
        {
            if (m_aeitemValues[c4Index].m_i4Value == i4EnumToFind)
                break;
            c4Index++;
        }
    }
     else
    {
        if (tCIDLib::TCard4(i4EnumToFind) >= m_c4ValCount)
            c4Index = m_c4ValCount;
        else
            c4Index = tCIDLib::TCard4(i4EnumToFind);
    }
    if (c4Index == m_c4ValCount)
    {
        if (bThrowIfNot)
            ThrowBadEnumVal(m_strTypeName, i4EnumToFind);
        return -1;
    }
    return m_aeitemValues[c4Index].m_i4AltNumVal;
}


//
//  Similar to above but returns the actual enum value. If not found, it either throws
//  or returns the 'not found' value.
//
tCIDLib::TInt4
TEnumMap::i4MapEnumText(const   TString&            strToFind
                        , const ETextVals           eTextVal
                        , const tCIDLib::TBoolean   bThrowIfNot)
{
    // Fault in loadable text if needed
    if (m_pfacLoad && (eTextVal == ETextVals::Text))
        LoadResText();

    tCIDLib::TCard4 c4Index = 0;
    while (c4Index < m_c4ValCount)
    {
        const TEnumValItem& aeitemCur = m_aeitemValues[c4Index];
        if (aeitemCur.m_astrTextVals[tCIDLib::c4EnumOrd(eTextVal)].bCompareI(strToFind))
            break;
        c4Index++;
    }
    if (c4Index == m_c4ValCount)
    {
        if (bThrowIfNot)
            ThrowBadEnumText(m_strTypeName, strToFind);
        return m_i4NotFoundVal;
    }
    return m_aeitemValues[c4Index].m_i4Value;
}


//
//  Calls c4MapEnumVal(). If it finds it, we return the requested string. If not,
//  we either throw or return the special unknown string.
//
const TString&
TEnumMap::strMapEnumVal(const   tCIDLib::TInt4      i4ToFind
                        , const ETextVals           eTextVal
                        , const tCIDLib::TBoolean   bThrowIfNot)
{
    // Fault in loadable text if needed
    if (m_pfacLoad && (eTextVal == ETextVals::Text))
        LoadResText();

    tCIDLib::TCard4 c4Index = 0;
    if (m_bNonContig)
    {
        // We have to do a search
        while (c4Index < m_c4ValCount)
        {
            if (m_aeitemValues[c4Index].m_i4Value == i4ToFind)
                break;
            c4Index++;
        }
    }
     else
    {
        // Call the finder that returns an index
        c4Index = c4MapEnumVal(i4ToFind, bThrowIfNot);
    }

    // If not found, return the standard unknown value text
    if (c4Index == kCIDLib::c4MaxCard)
        return strUnknownEnumValText;

    return m_aeitemValues[c4Index].m_astrTextVals[tCIDLib::c4EnumOrd(eTextVal)];
}



// ---------------------------------------------------------------------------
//  TEnumMap: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is called to fault in the _Text values if this enum is marked as having loadable
//  main text. Once done we zero the source facility pointer to indicate it's done.
//
tCIDLib::TVoid TEnumMap::LoadResText()
{
    // Lock and check again to make sure no one else beat us here
    TBaseLock lockLoad;
    if (m_pfacLoad)
    {
        // Loop through the values and load them
        for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4ValCount; c4Index++)
        {
            TEnumValItem& eitemCur = m_aeitemValues[c4Index];
            if (!m_pfacLoad->bLoadCIDMsg(eitemCur.m_midLoad
                                        , eitemCur.m_astrTextVals[tCIDLib::c4EnumOrd(ETextVals::Text)]))
                eitemCur.m_astrTextVals[tCIDLib::c4EnumOrd(ETextVals::Text)] = L"???";
        }

        // As the last thing, clear the facility pointer
        m_pfacLoad = nullptr;
    }
}


//
//  Helpers for some magic IDL generated enum functionality. We need this to be somewhere
//  that any valid CIDLib program would have to load. Some are public statics to allow
//  other to throw them.
//
tCIDLib::TVoid
TEnumMap::ThrowBadEnumAltNum(const TString& strType, const tCIDLib::TInt4 i4Val)
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcGen_BadEnumAltNum
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Range
        , TInteger(i4Val)
        , strType
    );
}

tCIDLib::TVoid
TEnumMap::ThrowBadEnumText(const TString& strType, const TString& strValue)
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcGen_BadEnumText
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Range
        , strValue
        , strType
    );
}
