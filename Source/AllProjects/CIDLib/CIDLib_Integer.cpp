//
// FILE NAME: CIDLib_Numeric.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/15/1992
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
//  This file provides classes for the basic data types integer, cardinal,
//  and float. They are just to allow numeric values to be treated as
//  objects when needed. They all have their own headers, but all of the
//  implementation is here.
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
//  Do our standard RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TInteger,TObject)
AdvRTTIDecls(TInteger64,TObject)
AdvRTTIDecls(TLocInteger,TInteger)
AdvRTTIDecls(TLocInteger64,TInteger64)



// ---------------------------------------------------------------------------
//   CLASS: TInteger
//  PREFIX: i
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TInteger: Public, static methods
// ---------------------------------------------------------------------------
TInteger& TInteger::Nul_TInteger()
{
    static TInteger* piNull = nullptr;
    if (!piNull)
    {
        TBaseLock lockInit;
        if (!piNull)
            TRawMem::pExchangePtr(&piNull, new TInteger);
    }
    return *piNull;
}


// ---------------------------------------------------------------------------
//  TInteger: Constructors and Destructor
// ---------------------------------------------------------------------------
TInteger::TInteger(const tCIDLib::TInt1 i1Val) :

    m_i4Val(i1Val)
{
}

TInteger::TInteger(const tCIDLib::TInt2 i2Val) :

    m_i4Val(i2Val)
{
}

TInteger::TInteger(const tCIDLib::TInt4 i4Val) :

    m_i4Val(i4Val)
{
}

TInteger::TInteger(const tCIDLib::TSInt iVal) :

    m_i4Val(iVal)
{
}

TInteger::TInteger( const   tCIDLib::TInt4      i4Val
                    , const tCIDLib::ERadices   eRadix) :

    m_i4Val(i4Val)
    , m_eRadix(eRadix)
{
}


// ---------------------------------------------------------------------------
//  TInteger: Public operators
// ---------------------------------------------------------------------------
TInteger& TInteger::operator=(const tCIDLib::TInt4 i4NewVal)
{
    m_i4Val = i4NewVal;
    return *this;
}

tCIDLib::TBoolean TInteger::operator==(const tCIDLib::TInt4 i4ToTest) const
{
    return (i4ToTest == m_i4Val);
}

tCIDLib::TBoolean TInteger::operator!=(const tCIDLib::TInt4 i4ToTest) const
{
    return (i4ToTest != m_i4Val);
}

tCIDLib::TBoolean TInteger::operator<(const TInteger& cComp) const
{
    return (m_i4Val < cComp.m_i4Val);
}

tCIDLib::TBoolean TInteger::operator<=(const TInteger& cComp) const
{
    return (m_i4Val <= cComp.m_i4Val);
}

tCIDLib::TBoolean TInteger::operator>(const TInteger& cComp) const
{
    return (m_i4Val > cComp.m_i4Val);
}

tCIDLib::TBoolean TInteger::operator>=(const TInteger& cComp) const
{
    return (m_i4Val >= cComp.m_i4Val);
}

tCIDLib::TBoolean TInteger::operator<(const tCIDLib::TInt4 i4ToTest) const
{
    return (m_i4Val < i4ToTest);
}

tCIDLib::TBoolean TInteger::operator<=(const tCIDLib::TInt4 i4ToTest) const
{
    return (m_i4Val <= i4ToTest);
}

tCIDLib::TBoolean TInteger::operator>(const tCIDLib::TInt4 i4ToTest) const
{
    return (m_i4Val > i4ToTest);
}

tCIDLib::TBoolean TInteger::operator>=(const tCIDLib::TInt4 i4ToTest) const
{
    return (m_i4Val >= i4ToTest);
}



// ---------------------------------------------------------------------------
//  TInteger: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TInt4 TInteger::i4Val(const tCIDLib::TInt4 i4NewVal)
{
    m_i4Val = i4NewVal;
    return m_i4Val;
}

tCIDLib::ERadices TInteger::eRadix() const
{
    return m_eRadix;
}

tCIDLib::ERadices TInteger::eRadix(const tCIDLib::ERadices eNewRadix)
{
    m_eRadix = eNewRadix;
    return m_eRadix;
}


// ---------------------------------------------------------------------------
//  TInteger: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TInteger::FormatTo(TTextOutStream& strmToWriteTo) const
{
    // Do the conversion into a temporary string
    tCIDLib::TZStr64    szTmp;
    if (!TRawStr::bFormatVal(m_i4Val, szTmp, 64, m_eRadix))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcLoc_NumericFormat
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // And append the formatted string to the stream
    strmToWriteTo << szTmp;
}


tCIDLib::TVoid TInteger::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom  >> m_i4Val
                    >> m_eRadix;
}

tCIDLib::TVoid TInteger::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << m_i4Val
                    << m_eRadix;
}



// ---------------------------------------------------------------------------
//   CLASS: TInteger64
//  PREFIX: i
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TInteger64: Public, static methods
// ---------------------------------------------------------------------------
TInteger64& TInteger64::Nul_TInteger64()
{
    static TInteger64* piNull = nullptr;
    if (!piNull)
    {
        TBaseLock lockInit;
        if (!piNull)
            TRawMem::pExchangePtr(&piNull, new TInteger64);
    }
    return *piNull;
}


// ---------------------------------------------------------------------------
//  TInteger64: Constructors and Destructor
// ---------------------------------------------------------------------------
TInteger64::TInteger64( const   tCIDLib::TInt8&     i8Val
                        , const tCIDLib::ERadices   eRadix) :
    m_i8Val(i8Val)
    , m_eRadix(eRadix)
{
}


// ---------------------------------------------------------------------------
//  TInteger64: Public operators
// ---------------------------------------------------------------------------
TInteger64& TInteger64::operator=(const tCIDLib::TInt8& i8NewVal)
{
    m_i8Val = i8NewVal;
    return *this;
}

tCIDLib::TBoolean TInteger64::operator==(const tCIDLib::TInt8& i8ToTest) const
{
    return (i8ToTest == m_i8Val);
}

tCIDLib::TBoolean TInteger64::operator!=(const tCIDLib::TInt8& i8ToTest) const
{
    return (i8ToTest != m_i8Val);
}

tCIDLib::TBoolean TInteger64::operator<(const TInteger64& cComp) const
{
    return (m_i8Val < cComp.m_i8Val);
}

tCIDLib::TBoolean TInteger64::operator<=(const TInteger64& cComp) const
{
    return (m_i8Val <= cComp.m_i8Val);
}

tCIDLib::TBoolean TInteger64::operator>(const TInteger64& cComp) const
{
    return (m_i8Val > cComp.m_i8Val);
}

tCIDLib::TBoolean TInteger64::operator>=(const TInteger64& cComp) const
{
    return (m_i8Val >= cComp.m_i8Val);
}

tCIDLib::TBoolean TInteger64::operator<(const tCIDLib::TInt8& i8ToTest) const
{
    return (m_i8Val < i8ToTest);
}

tCIDLib::TBoolean TInteger64::operator<=(const tCIDLib::TInt8& i8ToTest) const
{
    return (m_i8Val <= i8ToTest);
}

tCIDLib::TBoolean TInteger64::operator>(const tCIDLib::TInt8& i8ToTest) const
{
    return (m_i8Val > i8ToTest);
}

tCIDLib::TBoolean TInteger64::operator>=(const tCIDLib::TInt8& i8ToTest) const
{
    return (m_i8Val >= i8ToTest);
}



// ---------------------------------------------------------------------------
//  TInteger64: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TInt8 TInteger64::i8Val(const tCIDLib::TInt8& i8NewVal)
{
    m_i8Val = i8NewVal;
    return m_i8Val;
}

tCIDLib::ERadices TInteger64::eRadix() const
{
    return m_eRadix;
}

tCIDLib::ERadices TInteger64::eRadix(const tCIDLib::ERadices eNewRadix)
{
    m_eRadix = eNewRadix;
    return m_eRadix;
}


// ---------------------------------------------------------------------------
//  TInteger64: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TInteger64::FormatTo(TTextOutStream& strmToWriteTo) const
{
    // Do the conversion into a temporary string
    tCIDLib::TZStr128    szTmp;
    if (!TRawStr::bFormatVal(m_i8Val, szTmp, 128, m_eRadix))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcLoc_NumericFormat
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // And append the formatted string to the stream
    strmToWriteTo << szTmp;
}


tCIDLib::TVoid TInteger64::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom >> m_i8Val;
    strmToReadFrom >> m_eRadix;
}

tCIDLib::TVoid TInteger64::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo << m_i8Val;
    strmToWriteTo << m_eRadix;
}



// ---------------------------------------------------------------------------
//   CLASS: TLocInteger
//  PREFIX: i
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TLocInteger: Constructors and Destructor
// ---------------------------------------------------------------------------
TLocInteger::TLocInteger()
{
}

TLocInteger::TLocInteger(const tCIDLib::TInt4 i4Val) :

    TInteger(i4Val, tCIDLib::ERadices::Dec)
{
}

TLocInteger::~TLocInteger()
{
}


// ---------------------------------------------------------------------------
//  TLocInteger: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TLocInteger::operator==(const TLocInteger& iToTest) const
{
    if (this == &iToTest)
        return kCIDLib::True;
    return TParent::operator==(iToTest);
}

tCIDLib::TBoolean TLocInteger::operator!=(const TLocInteger& iToTest) const
{
    return !TParent::operator==(iToTest);
}

tCIDLib::TBoolean TLocInteger::operator<(const TLocInteger& cComp) const
{
    return TParent::operator<(cComp);
}

tCIDLib::TBoolean TLocInteger::operator<=(const TLocInteger& cComp) const
{
    return TParent::operator<=(cComp);
}

tCIDLib::TBoolean TLocInteger::operator>(const TLocInteger& cComp) const
{
    return TParent::operator>(cComp);
}

tCIDLib::TBoolean TLocInteger::operator>=(const TLocInteger& cComp) const
{
    return TParent::operator>=(cComp);
}


// ---------------------------------------------------------------------------
//  TLocInteger: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TLocInteger::FormatTo(TTextOutStream& strmToWriteTo) const
{
    // Do the conversion into a temporary string
    tCIDLib::TZStr64    szTmp;
    if (!TRawStr::bFormatVal(i4Val()
                            , szTmp
                            , 64
                            , tCIDLib::ERadices::Dec
                            , TLocale::chNegativeSign()
                            , TLocale::c4GroupSize()
                            , TLocale::chGroupSeparator()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcLoc_NumericFormat
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    strmToWriteTo << szTmp;
}



// ---------------------------------------------------------------------------
//   CLASS: TLocInteger64
//  PREFIX: i
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TLocInteger64: Constructors and Destructor
// ---------------------------------------------------------------------------
TLocInteger64::TLocInteger64()
{
}

TLocInteger64::TLocInteger64(const tCIDLib::TInt8& i8Val) :

    TInteger64(i8Val, tCIDLib::ERadices::Dec)
{
}

TLocInteger64::~TLocInteger64()
{
}


// ---------------------------------------------------------------------------
//  TLocInteger64: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TLocInteger64::operator==(const TLocInteger64& iToTest) const
{
    if (this == &iToTest)
        return kCIDLib::True;
    return TParent::operator==(iToTest);
}

tCIDLib::TBoolean TLocInteger64::operator!=(const TLocInteger64& iToTest) const
{
    return !TParent::operator==(iToTest);
}

tCIDLib::TBoolean TLocInteger64::operator<(const TLocInteger64& cComp) const
{
    return TParent::operator<(cComp);
}

tCIDLib::TBoolean TLocInteger64::operator<=(const TLocInteger64& cComp) const
{
    return TParent::operator<=(cComp);
}

tCIDLib::TBoolean TLocInteger64::operator>(const TLocInteger64& cComp) const
{
    return TParent::operator>(cComp);
}

tCIDLib::TBoolean TLocInteger64::operator>=(const TLocInteger64& cComp) const
{
    return TParent::operator>=(cComp);
}


// ---------------------------------------------------------------------------
//  TLocInteger64: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TLocInteger64::FormatTo(TTextOutStream& strmToWriteTo) const
{
    // Do the conversion into a temporary string
    tCIDLib::TZStr128 szTmp;
    if (!TRawStr::bFormatVal(i8Val()
                            , szTmp
                            , 128
                            , tCIDLib::ERadices::Dec
                            , TLocale::chNegativeSign()
                            , TLocale::c4GroupSize()
                            , TLocale::chGroupSeparator()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcLoc_NumericFormat
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    strmToWriteTo << szTmp;
}

