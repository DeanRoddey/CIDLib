//
// FILE NAME: CIDLib_Float.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/15/1992
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
//  This file provides simple wrapper classes for floating point values,
//  which allow them to be used as objects where needed.
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
AdvRTTIDecls(TFloat,TObject)
AdvRTTIDecls(TLocFloat,TFloat)



// ---------------------------------------------------------------------------
//   CLASS: TFloat
//  PREFIX: f
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFloat: Public, static methods
// ---------------------------------------------------------------------------
TFloat& TFloat::Nul_TFloat()
{
    static TFloat* pfNull = nullptr;
    if (!pfNull)
    {
        TBaseLock lockInit;
        if (!pfNull)
            TRawMem::pExchangePtr(&pfNull, new TFloat);
    }
    return *pfNull;
}


// ---------------------------------------------------------------------------
//  TFloat: Constructors and Destructor
// ---------------------------------------------------------------------------
TFloat::TFloat(const tCIDLib::TFloat4& f4Val) :

    m_f8Val(f4Val)
{
}

TFloat::TFloat(const tCIDLib::TFloat8& f8Val) :

    m_f8Val(f8Val)
{
}

TFloat::TFloat( const   tCIDLib::TFloat8&   f8Val
                , const tCIDLib::TCard1     c1Precision
                , const tCIDLib::ETrailFmts eStyle) :

    m_c1Precision(c1Precision)
    , m_eTrailingStyle(eStyle)
    , m_f8Val(f8Val)
{
}

TFloat::TFloat( const   tCIDLib::TCard4     c4Val
                , const tCIDLib::TCard1     c1Precision
                , const tCIDLib::ETrailFmts eStyle) :

    m_c1Precision(c1Precision)
    , m_eTrailingStyle(eStyle)
    , m_f8Val(tCIDLib::TFloat8(c4Val))
{
}

TFloat::TFloat( const   tCIDLib::TInt4      i4Val
                , const tCIDLib::TCard1     c1Precision
                , const tCIDLib::ETrailFmts eStyle) :

    m_c1Precision(c1Precision)
    , m_eTrailingStyle(eStyle)
    , m_f8Val(tCIDLib::TFloat8(i4Val))
{
}

TFloat::TFloat( const   tCIDLib::TSInt      sVal
                , const tCIDLib::TCard1     c1Precision
                , const tCIDLib::ETrailFmts eStyle) :

    m_c1Precision(c1Precision)
    , m_eTrailingStyle(eStyle)
    , m_f8Val(tCIDLib::TFloat8(sVal))
{
}


// ---------------------------------------------------------------------------
//  TFloat: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFloat::operator==(const TFloat& fToTest) const
{
    if (this == &fToTest)
        return kCIDLib::True;

    // Compare the values of the objects
    if ((fToTest.m_f8Val != m_f8Val)
    ||  (fToTest.m_c1Precision != m_c1Precision)
    ||  (fToTest.m_eTrailingStyle != m_eTrailingStyle))
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


TFloat& TFloat::operator=(const tCIDLib::TFloat8& f8NewVal)
{
    m_f8Val = f8NewVal;
    return *this;
}

tCIDLib::TBoolean TFloat::operator!=(const TFloat& fToTest) const
{
    return !operator==(fToTest);
}

tCIDLib::TBoolean TFloat::operator==(const tCIDLib::TFloat8& f8ToTest) const
{
    return (f8ToTest == m_f8Val);
}

tCIDLib::TBoolean TFloat::operator!=(const tCIDLib::TFloat8& f8ToTest) const
{
    return (f8ToTest != m_f8Val);
}

tCIDLib::TBoolean TFloat::operator<(const TFloat& fToTest) const
{
    return (m_f8Val < fToTest.m_f8Val);
}

tCIDLib::TBoolean TFloat::operator<=(const TFloat& fToTest) const
{
    return (m_f8Val <= fToTest.m_f8Val);
}

tCIDLib::TBoolean TFloat::operator>(const TFloat& fToTest) const
{
    return (m_f8Val > fToTest.m_f8Val);
}

tCIDLib::TBoolean TFloat::operator>=(const TFloat& fToTest) const
{
    return (m_f8Val >= fToTest.m_f8Val);
}

tCIDLib::TBoolean TFloat::operator<(const tCIDLib::TFloat8& f8ToTest) const
{
    return (m_f8Val < f8ToTest);
}

tCIDLib::TBoolean TFloat::operator<=(const tCIDLib::TFloat8& f8ToTest) const
{
    return (m_f8Val <= f8ToTest);
}

tCIDLib::TBoolean TFloat::operator>(const tCIDLib::TFloat8& f8ToTest) const
{
    return (m_f8Val > f8ToTest);
}

tCIDLib::TBoolean TFloat::operator>=(const tCIDLib::TFloat8& f8ToTest) const
{
    return (m_f8Val >= f8ToTest);
}


// ---------------------------------------------------------------------------
//  TFloat: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard1 TFloat::c1Precision() const
{
    return m_c1Precision;
}

tCIDLib::TCard1 TFloat::c1Precision(const tCIDLib::TCard1 c1Precision)
{
    m_c1Precision = c1Precision;
    return m_c1Precision;
}

tCIDLib::ETrailFmts TFloat::eTrailingStyle() const
{
    return m_eTrailingStyle;
}

tCIDLib::ETrailFmts
TFloat::eTrailingStyle(const tCIDLib::ETrailFmts eStyle)
{
    m_eTrailingStyle = eStyle;
    return eStyle;
}

tCIDLib::TFloat8 TFloat::f8Val(const tCIDLib::TFloat8& f8NewVal)
{
    m_f8Val = f8NewVal;
    return m_f8Val;
}


// ---------------------------------------------------------------------------
//  TFloat: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFloat::FormatTo(TTextOutStream& strmToWriteTo) const
{
    // Format to a temp string, then dump it
    tCIDLib::TZStr128   szTmp;
    if (!TRawStr::bFormatVal(m_f8Val, szTmp, m_c1Precision, 128, m_eTrailingStyle))
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


tCIDLib::TVoid TFloat::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom >> m_f8Val;
    strmToReadFrom >> m_c1Precision;
    strmToReadFrom >> m_eTrailingStyle;
}

tCIDLib::TVoid TFloat::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo << m_f8Val;
    strmToWriteTo << m_c1Precision;
    strmToWriteTo << m_eTrailingStyle;
}



// ---------------------------------------------------------------------------
//   CLASS: TLocFloat
//  PREFIX: f
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TLocFloat: Constructors and Destructor
// ---------------------------------------------------------------------------
TLocFloat::TLocFloat()
{
}

TLocFloat::TLocFloat(const  tCIDLib::TFloat8& f8Val) :

    TFloat(f8Val)
{
}

TLocFloat::TLocFloat(const  tCIDLib::TFloat8&   f8Val
                    , const tCIDLib::TCard1     c1Precision
                    , const tCIDLib::ETrailFmts eStyle) :
    TFloat(f8Val, c1Precision, eStyle)
{
}

TLocFloat::TLocFloat(const  tCIDLib::TCard4     c4Val
                    , const tCIDLib::TCard1     c1Precision
                    , const tCIDLib::ETrailFmts eStyle) :
    TFloat(c4Val, c1Precision, eStyle)
{
}


TLocFloat::TLocFloat(const  tCIDLib::TInt4      i4Val
                    , const tCIDLib::TCard1     c1Precision
                    , const tCIDLib::ETrailFmts eStyle) :
    TFloat(i4Val, c1Precision, eStyle)
{
}

TLocFloat::TLocFloat(const TLocFloat& fToCopy) :

    TFloat(fToCopy)
{
}

TLocFloat::~TLocFloat()
{
}


// ---------------------------------------------------------------------------
//  TLocFloat: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TLocFloat::operator==(const TLocFloat& fToTest) const
{
    if (this == &fToTest)
        return kCIDLib::True;

    return TParent::operator==(fToTest);
}

tCIDLib::TBoolean TLocFloat::operator!=(const TLocFloat& fToTest) const
{
    return !TParent::operator==(fToTest);
}

tCIDLib::TBoolean TLocFloat::operator<(const TLocFloat& cComp) const
{
    return TParent::operator<(cComp);
}

tCIDLib::TBoolean TLocFloat::operator<=(const TLocFloat& cComp) const
{
    return TParent::operator<=(cComp);
}

tCIDLib::TBoolean TLocFloat::operator>(const TLocFloat& cComp) const
{
    return TParent::operator>(cComp);
}

tCIDLib::TBoolean TLocFloat::operator>=(const TLocFloat& cComp) const
{
    return TParent::operator>=(cComp);
}


// ---------------------------------------------------------------------------
//  TLocFloat: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TLocFloat::FormatTo(TTextOutStream& strmToWriteTo) const
{
    // Format to a temp string, then dump it
    tCIDLib::TZStr128   szTmp;
    if (!TRawStr::bFormatVal(f8Val()
                            , szTmp
                            , c1Precision()
                            , 128
                            , tCIDLib::ETrailFmts::Zeroes
                            , TLocale::chNegativeSign()
                            , TLocale::chDecimalSymbol()
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

