//
// FILE NAME: CIDLib_KeyValuePair.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/17/2001
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  Implements the key value pair class, which is widely used.
//
// CAVEATS/GOTCHAS:
//
//  1)  We made a historically stupid decision not to have a flattened format
//      version for these at the start, but ones added later we have implemented
//      that. At some point we need to address the painful decision to replace
//      the non-versioned ones with some new versioned version.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TKeyValuePair,TObject)
RTTIDecls(TKeyValFPair, TKeyValuePair)
RTTIDecls(TKeyNumPair,TObject)
RTTIDecls(TKeyValues,TObject)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDLib_KeyValuePair
{
    namespace
    {
        // The persistent format version for the key/value/flag class
        constexpr tCIDLib::TCard1       c1KVPFFmtVersion = 1;
    }
}




// ---------------------------------------------------------------------------
//   CLASS: TKeyValuePair
//  PREFIX: kval
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKeyValuePair: Constructors and Destructor
// ---------------------------------------------------------------------------
TKeyValuePair::TKeyValuePair(const  TString&    strKey
                            , const TString&    strValue) :
    m_strKey(strKey)
    , m_strValue(strValue)
{
}

TKeyValuePair::TKeyValuePair(const  TString&            strKey
                            , const tCIDLib::TCh* const pszValue) :
    m_strKey(strKey)
    , m_strValue(pszValue)
{
}

TKeyValuePair::TKeyValuePair(const  tCIDLib::TCh* const pszKey
                            , const tCIDLib::TCh* const pszValue) :
    m_strKey(pszKey)
    , m_strValue(pszValue)
{
}

TKeyValuePair::TKeyValuePair(const  TString&            strKey
                            , const tCIDLib::TBoolean   bValue) :
    m_strKey(strKey)
    , m_strValue(bValue ? L"Yes" : L"No")
{
}

TKeyValuePair::TKeyValuePair(const  TString&        strKey
                            , const tCIDLib::TCh    chValue) :
    m_strKey(strKey)
    , m_strValue(1UL)
{
    m_strValue.Append(chValue);
}

TKeyValuePair::TKeyValuePair(const  TString&            strKey
                            , const tCIDLib::TCard1     c1Value
                            , const tCIDLib::ERadices   eRadix) :
    m_strKey(strKey)
    , m_strValue(6UL)
{
    if (eRadix == tCIDLib::ERadices::Hex)
        m_strValue.Append(L"0x");
    m_strValue.AppendFormatted(c1Value, eRadix);
}

TKeyValuePair::TKeyValuePair(const  TString&            strKey
                            , const tCIDLib::TCard2     c2Value
                            , const tCIDLib::ERadices   eRadix) :
    m_strKey(strKey)
    , m_strValue(7UL)
{
    if (eRadix == tCIDLib::ERadices::Hex)
        m_strValue.Append(L"0x");
    m_strValue.AppendFormatted(c2Value, eRadix);
}

TKeyValuePair::TKeyValuePair(const  TString&            strKey
                            , const tCIDLib::TCard4     c4Value
                            , const tCIDLib::ERadices   eRadix) :
    m_strKey(strKey)
    , m_strValue(12UL)
{
    if (eRadix == tCIDLib::ERadices::Hex)
        m_strValue.Append(L"0x");
    m_strValue.AppendFormatted(c4Value, eRadix);
}

TKeyValuePair::TKeyValuePair(const  TString&            strKey
                            , const tCIDLib::TCard8     c8Value
                            , const tCIDLib::ERadices   eRadix) :
    m_strKey(strKey)
    , m_strValue(20UL)
{
    if (eRadix == tCIDLib::ERadices::Hex)
        m_strValue.Append(L"0x");
    m_strValue.AppendFormatted(c8Value, eRadix);
}

TKeyValuePair::TKeyValuePair(const  TString&            strKey
                            , const tCIDLib::TFloat4    f4Value
                            , const tCIDLib::TCard4     c4DecDigs) :
    m_strKey(strKey)
    , m_strValue(12UL)
{
    m_strValue.AppendFormatted(f4Value, c4DecDigs);
}

TKeyValuePair::TKeyValuePair(const  TString&            strKey
                            , const tCIDLib::TFloat8    f8Value
                            , const tCIDLib::TCard4     c4DecDigs) :
    m_strKey(strKey)
    , m_strValue(18UL)
{
    m_strValue.AppendFormatted(f8Value, c4DecDigs);
}

TKeyValuePair::TKeyValuePair(const  TString&        strKey
                            , const tCIDLib::TInt1  i1Value) :
    m_strKey(strKey)
    , m_strValue(5UL)
{
    m_strValue.AppendFormatted(i1Value);
}

TKeyValuePair::TKeyValuePair(const  TString&        strKey
                            , const tCIDLib::TInt2  i2Value) :
    m_strKey(strKey)
    , m_strValue(8UL)
{
    m_strValue.AppendFormatted(i2Value);
}

TKeyValuePair::TKeyValuePair(const  TString&        strKey
                            , const tCIDLib::TInt4  i4Value) :
    m_strKey(strKey)
    , m_strValue(11UL)
{
    m_strValue.AppendFormatted(i4Value);
}

TKeyValuePair::TKeyValuePair(const  TString&        strKey
                            , const tCIDLib::TInt8  i8Value) :
    m_strKey(strKey)
    , m_strValue(18UL)
{
    m_strValue.AppendFormatted(i8Value);
}



// ---------------------------------------------------------------------------
//  TKeyValuePair: Public Operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKeyValuePair::operator<(const TKeyValuePair& kvalSrc) const
{
    return (m_strKey < kvalSrc.m_strKey);
}

tCIDLib::TBoolean TKeyValuePair::operator>(const TKeyValuePair& kvalSrc) const
{
    return (m_strKey > kvalSrc.m_strKey);
}

tCIDLib::TBoolean TKeyValuePair::operator==(const TKeyValuePair& kvalSrc) const
{
    return
    (
        (m_strKey == kvalSrc.m_strKey)
        && (m_strValue == kvalSrc.m_strValue)
    );
}

tCIDLib::TBoolean TKeyValuePair::operator!=(const TKeyValuePair& kvalSrc) const
{
    return !operator==(kvalSrc);
}


// ---------------------------------------------------------------------------
//  TKeyValuePair: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Clear both key and value
tCIDLib::TVoid TKeyValuePair::Clear()
{
    m_strKey.Clear();
    m_strValue.Clear();
}


// Get or set the key
const TString& TKeyValuePair::strKey() const
{
    return m_strKey;
}

const TString& TKeyValuePair::strKey(const TString& strNewKey)
{
    m_strKey = strNewKey;
    return m_strKey;
}


// Get/set the value. We also allow direct modifications to the value
TString& TKeyValuePair::strValue()
{
    return m_strValue;
}

const TString& TKeyValuePair::strValue() const
{
    return m_strValue;
}

const TString& TKeyValuePair::strValue(const TString& strNewValue)
{
    m_strValue = strNewValue;
    return m_strValue;
}


// Set the key and value directly
tCIDLib::TVoid TKeyValuePair::Set(  const   TString&    strNewKey
                                    , const TString&    strNewValue)
{
    m_strKey = strNewKey;
    m_strValue = strNewValue;
}


// ---------------------------------------------------------------------------
//  TKeyValuePair: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKeyValuePair::FormatTo(CIOP TTextOutStream& strmDest) const
{
    strmDest << L"{" << m_strKey << L"," << m_strValue << L"}";
}

tCIDLib::TVoid TKeyValuePair::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom  >> m_strKey >> m_strValue;
}

tCIDLib::TVoid TKeyValuePair::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo << m_strKey << m_strValue;
}



// ---------------------------------------------------------------------------
//   CLASS: TKeyValFPair
//  PREFIX: kvalf
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKeyValFPair: Constructors and destructor
// ---------------------------------------------------------------------------
TKeyValFPair::TKeyValFPair(const tCIDLib::TBoolean bFlag) :

    m_bFlag(bFlag)
{
}

TKeyValFPair::TKeyValFPair( const   TString&            strKey
                            , const TString&            strValue
                            , const tCIDLib::TBoolean   bFlag) :

    TParent(strKey, strValue)
    , m_bFlag(bFlag)
{
}

TKeyValFPair::TKeyValFPair( const   TString&                strKey
                            , const tCIDLib::TCh* const     pszValue
                            , const tCIDLib::TBoolean       bFlag) :

    TParent(strKey, pszValue)
    , m_bFlag(bFlag)
{
}

TKeyValFPair::TKeyValFPair( const   tCIDLib::TCh* const     pszKey
                            , const tCIDLib::TCh* const     pszValue
                            , const tCIDLib::TBoolean       bFlag) :

    TParent(pszKey, pszValue)
    , m_bFlag(bFlag)
{
}

TKeyValFPair::~TKeyValFPair()
{
}


// ---------------------------------------------------------------------------
//  TKeyValFPair: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKeyValFPair::operator==(const TKeyValFPair& kvalfSrc) const
{
    if (&kvalfSrc == this)
        return kCIDLib::True;

    // Check our parent first
    if (!TParent::operator==(kvalfSrc))
        return kCIDLib::False;

    // And check our stuff
    return (kvalfSrc.m_bFlag == m_bFlag);
}

tCIDLib::TBoolean TKeyValFPair::operator!=(const TKeyValFPair& kvalfSrc) const
{
    return !operator==(kvalfSrc);
}


// ---------------------------------------------------------------------------
//  TKeyValFPair: Protected, non-virtual methods
// ---------------------------------------------------------------------------

// Ours hides the parent's version, so we have to provide a passthrough
tCIDLib::TVoid TKeyValFPair::Set(const  TString& strNewKey, const TString& strNewValue)
{
    TParent::Set(strNewKey, strNewValue);
    m_bFlag = kCIDLib::False;
}

tCIDLib::TVoid TKeyValFPair::Set(const  TString&            strNewKey
                                , const TString&            strNewValue
                                , const tCIDLib::TBoolean   bFlag)
{
    TParent::Set(strNewKey, strNewValue);
    m_bFlag = bFlag;
}


// ---------------------------------------------------------------------------
//  TKeyValFPair: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKeyValFPair::StreamFrom(TBinInStream& strmToReadFrom)
{
    // The first thing has to be a start object marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard1 c1FmtVersion;
    strmToReadFrom  >> c1FmtVersion;
    if (!c1FmtVersion || (c1FmtVersion > CIDLib_KeyValuePair::c1KVPFFmtVersion))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c1FmtVersion)
            , clsThis()
        );
    }

    // Do our parent first
    TParent::StreamFrom(strmToReadFrom);

    // And do our stuff
    strmToReadFrom >> m_bFlag;

    // And it should end with an end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}

tCIDLib::TVoid TKeyValFPair::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Start with a marker and format version
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDLib_KeyValuePair::c1KVPFFmtVersion;

    // Do our parent first
    TParent::StreamTo(strmToWriteTo);

    // And do our stuff and an end marker
    strmToWriteTo   << m_bFlag
                    << tCIDLib::EStreamMarkers::EndObject;
}




// ---------------------------------------------------------------------------
//   CLASS: TKeyNumPair
//  PREFIX: knum
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKeyNumPair: Constructors and Destructor
// ---------------------------------------------------------------------------
TKeyNumPair::TKeyNumPair(const  TString&            strKey
                        , const tCIDLib::TFloat4    f4Value) :
    m_f4Value(f4Value)
    , m_strKey(strKey)
{
}

TKeyNumPair::TKeyNumPair(const  TString&            strKey
                        , const tCIDLib::TFloat4    f4Value
                        , const TString&            strValue) :
    m_f4Value(f4Value)
    , m_strKey(strKey)
    , m_strValue(strValue)
{
}


// ---------------------------------------------------------------------------
//  TKeyNumPair: Public Operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKeyNumPair::operator<(const TKeyNumPair& knumToCompare) const
{
    return (m_strKey < knumToCompare.m_strKey);
}

tCIDLib::TBoolean TKeyNumPair::operator>(const TKeyNumPair& knumToCompare) const
{
    return (m_strKey > knumToCompare.m_strKey);
}

tCIDLib::TBoolean TKeyNumPair::operator==(const TKeyNumPair& knumToCompare) const
{
    return
    (
        (m_strKey == knumToCompare.m_strKey)
        && (m_f4Value == knumToCompare.m_f4Value)
        && (m_strValue == knumToCompare.m_strValue)
    );
}

tCIDLib::TBoolean TKeyNumPair::operator!=(const TKeyNumPair& knumToCompare) const
{
    return !operator==(knumToCompare);
}



// ---------------------------------------------------------------------------
//  TKeyNumPair: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Clear both key and value
tCIDLib::TVoid TKeyNumPair::Clear()
{
    m_strKey.Clear();
    m_f4Value = 0;
    m_strValue.Clear();
}


// Get or set the numeric values
tCIDLib::TFloat4 TKeyNumPair::f4Value() const
{
    return m_f4Value;
}

tCIDLib::TFloat4 TKeyNumPair::f4Value(const tCIDLib::TFloat4  f4ToSet)
{
    m_f4Value = f4ToSet;
    return m_f4Value;
}


// Get or set the key
const TString& TKeyNumPair::strKey() const
{
    return m_strKey;
}

const TString& TKeyNumPair::strKey(const TString& strNewKey)
{
    m_strKey = strNewKey;
    return m_strKey;
}


// Get or set the string value
const TString& TKeyNumPair::strValue() const
{
    return m_strValue;
}

const TString& TKeyNumPair::strValue(const TString& strToSet)
{
    m_strValue = strToSet;
    return m_strValue;
}



// Set the key and values after the face
tCIDLib::TVoid
TKeyNumPair::Set(const  TString&            strNewKey
                , const tCIDLib::TFloat4    f4Value)
{
    m_f4Value = f4Value;
    m_strKey = strNewKey;
    m_strValue.Clear();
}

tCIDLib::TVoid
TKeyNumPair::Set(const  TString&            strNewKey
                , const tCIDLib::TFloat4    f4Value
                , const TString&            strValue)
{
    m_f4Value = f4Value;
    m_strKey = strNewKey;
    m_strValue = strValue;
}

// ---------------------------------------------------------------------------
//  TKeyNumPair: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKeyNumPair::FormatTo(CIOP TTextOutStream& strmDest) const
{
    strmDest << L"{"
             << m_strKey << L"," << m_f4Value;

    if (!m_strValue.bIsEmpty())
        strmDest << L", " << m_strValue;

    strmDest << L"}";
}

tCIDLib::TVoid TKeyNumPair::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom  >> m_strKey >> m_f4Value >> m_strValue;
}

tCIDLib::TVoid TKeyNumPair::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo << m_strKey << m_f4Value << m_strValue;
}




// ---------------------------------------------------------------------------
//   CLASS: TKeyValues
//  PREFIX: kvals
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKeyValues: Constructors and Destructor
// ---------------------------------------------------------------------------
TKeyValues::TKeyValues( const   TString&    strKey
                        , const TString&    strVal1) :
    m_strKey(strKey)
    , m_strVal1(strVal1)
{
}

TKeyValues::TKeyValues( const   TString&    strKey
                        , const TString&    strVal1
                        , const TString&    strVal2) :
    m_strKey(strKey)
    , m_strVal1(strVal1)
    , m_strVal2(strVal2)
{
}

TKeyValues::TKeyValues( const  TString&     strKey
                        , const TString&    strVal1
                        , const TString&    strVal2
                        , const TString&    strVal3) :
    m_strKey(strKey)
    , m_strVal1(strVal1)
    , m_strVal2(strVal2)
    , m_strVal3(strVal3)
{
}

TKeyValues::TKeyValues( const   TString&            strKey
                        , const tCIDLib::TCh* const pszVal1
                        , const tCIDLib::TCh* const pszVal2
                        , const tCIDLib::TCh* const pszVal3) :
    m_strKey(strKey)
    , m_strVal1(pszVal1)
    , m_strVal2(pszVal2)
    , m_strVal3(pszVal3)
{
}

TKeyValues::TKeyValues( const   tCIDLib::TCh* const pszKey
                        , const tCIDLib::TCh* const pszVal1
                        , const tCIDLib::TCh* const pszVal2
                        , const tCIDLib::TCh* const pszVal3) :
    m_strKey(pszKey)
    , m_strVal1(pszVal1)
    , m_strVal2(pszVal2)
    , m_strVal3(pszVal3)
{
}


// ---------------------------------------------------------------------------
//  TKeyValues: Public Operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKeyValues::operator<(const TKeyValues& kvalsToCompare) const
{
    return (m_strKey < kvalsToCompare.m_strKey);
}

tCIDLib::TBoolean TKeyValues::operator>(const TKeyValues& kvalsToCompare) const
{
    return (m_strKey > kvalsToCompare.m_strKey);
}

tCIDLib::TBoolean
TKeyValues::operator==(const TKeyValues& kvalsToCompare) const
{
    return
    (
        (m_strKey == kvalsToCompare.m_strKey)
        && (m_strVal1 == kvalsToCompare.m_strVal1)
        && (m_strVal2 == kvalsToCompare.m_strVal2)
        && (m_strVal3 == kvalsToCompare.m_strVal3)
    );
}

tCIDLib::TBoolean
TKeyValues::operator!=(const TKeyValues& kvalsToCompare) const
{
    return !operator==(kvalsToCompare);
}


// ---------------------------------------------------------------------------
//  TKeyValues: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Clear both key and value
tCIDLib::TVoid TKeyValues::Clear()
{
    m_strKey.Clear();
    m_strVal1.Clear();
    m_strVal2.Clear();
    m_strVal3.Clear();
}


// Get or set the key
const TString& TKeyValues::strKey() const
{
    return m_strKey;
}

const TString& TKeyValues::strKey(const TString& strNewKey)
{
    m_strKey = strNewKey;
    return m_strKey;
}


// Get/set the values
const TString& TKeyValues::strVal1() const
{
    return m_strVal1;
}

const TString& TKeyValues::strVal1(const TString& strNewValue)
{
    m_strVal1 = strNewValue;
    return m_strVal1;
}

const TString& TKeyValues::strVal2() const
{
    return m_strVal2;
}

const TString& TKeyValues::strVal2(const TString& strNewValue)
{
    m_strVal2 = strNewValue;
    return m_strVal2;
}


const TString& TKeyValues::strVal3() const
{
    return m_strVal3;
}

const TString& TKeyValues::strVal3(const TString& strNewValue)
{
    m_strVal3 = strNewValue;
    return m_strVal3;
}


// Set the key and values together
tCIDLib::TVoid TKeyValues::Set( const   TString&    strNewKey
                                , const TString&    strVal1)
{
    m_strKey = strNewKey;
    m_strVal1 = strVal1;

    m_strVal2.Clear();
    m_strVal3.Clear();
}

tCIDLib::TVoid TKeyValues::Set( const   TString&    strNewKey
                                , const TString&    strVal1
                                , const TString&    strVal2)
{
    m_strKey = strNewKey;
    m_strVal1 = strVal1;
    m_strVal2 = strVal2;

    m_strVal3.Clear();
}

tCIDLib::TVoid TKeyValues::Set( const   TString&    strNewKey
                                , const TString&    strVal1
                                , const TString&    strVal2
                                , const TString&    strVal3)
{
    m_strKey = strNewKey;
    m_strVal1 = strVal1;
    m_strVal2 = strVal2;
    m_strVal3 = strVal3;
}

tCIDLib::TVoid TKeyValues::Set( const   TString&            strNewKey
                                , const tCIDLib::TCh* const pszVal1
                                , const tCIDLib::TCh* const pszVal2
                                , const tCIDLib::TCh* const pszVal3)
{
    m_strKey = strNewKey;
    if (pszVal1)
        m_strVal1 = pszVal1;

    if (pszVal2)
        m_strVal2 = pszVal2;

    if (pszVal3)
        m_strVal3 = pszVal3;
}

tCIDLib::TVoid TKeyValues::Set( const   tCIDLib::TCh* const pszNewKey
                                , const tCIDLib::TCh* const pszVal1
                                , const tCIDLib::TCh* const pszVal2
                                , const tCIDLib::TCh* const pszVal3)
{
    m_strKey = pszNewKey;
    m_strVal1 = pszVal1;
    m_strVal2 = pszVal2;
    m_strVal3 = pszVal3;
}


// ---------------------------------------------------------------------------
//  TKeyValues: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKeyValues::FormatTo(CIOP TTextOutStream& strmDest) const
{
    strmDest    << L"{"
                << m_strKey << L" - "
                << m_strVal1 << L","
                << m_strVal2 << L","
                << m_strVal3
                << L"}";
}

tCIDLib::TVoid TKeyValues::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom  >> m_strKey >> m_strVal1 >> m_strVal2 >> m_strVal3;
}

tCIDLib::TVoid TKeyValues::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo << m_strKey << m_strVal1 << m_strVal2 << m_strVal3;
}


