//
// FILE NAME: CIDMath_Range.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/08/1993
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
//  This file implements the TRange class, which provides a mechanism for
//  dealing with a value that must stay within a range, and to covert it
//  in and out via various conversions (e.g. percent of range.)
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
#include    "CIDMath_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TRange,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TRange
//  PREFIX: rng
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TRange: Constructors and Destructor
// ---------------------------------------------------------------------------
TRange::TRange() :

    m_c4Visible(100)
    , m_i4CurValue(0)
    , m_i4MinValue(0)
    , m_i4MaxValue(100)
{
}

TRange::TRange( const   tCIDLib::TCard4 c4Visible
                , const tCIDLib::TInt4  i4CurValue
                , const tCIDLib::TInt4  i4MinValue
                , const tCIDLib::TInt4  i4MaxValue) :

    m_c4Visible(c4Visible)
    , m_i4CurValue(i4CurValue)
    , m_i4MinValue(tCIDLib::MinVal(i4MinValue, i4MaxValue))
    , m_i4MaxValue(tCIDLib::MaxVal(i4MinValue, i4MaxValue))
{
    //
    //  Validate it. It does not matter here that we have already stored
    //  bad stuff because the object will never construct anyway.
    //
    Validate(CID_LINE);
}

TRange::~TRange()
{
}


// ---------------------------------------------------------------------------
//  TRange: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TRange::operator==(const TRange& rngToTest) const
{
    if (&rngToTest != this)
    {
        if ((rngToTest.m_c4Visible != m_c4Visible)
        ||  (rngToTest.m_i4CurValue != m_i4CurValue)
        ||  (rngToTest.m_i4MinValue != m_i4MinValue)
        ||  (rngToTest.m_i4MaxValue != m_i4MaxValue))
        {
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean TRange::operator!=(const TRange& rngToTest) const
{
    return !operator==(rngToTest);
}


tCIDLib::TInt4 TRange::operator--()
{
    if (m_i4CurValue > m_i4MinValue)
        m_i4CurValue--;
    return m_i4CurValue;
}

tCIDLib::TInt4 TRange::operator--(int)
{
    const tCIDLib::TInt4 i4Ret = m_i4CurValue;
    if (m_i4CurValue > m_i4MinValue)
        m_i4CurValue--;
    return i4Ret;
}

tCIDLib::TInt4 TRange::operator++()
{
    if (m_i4CurValue < m_i4MaxValue)
        m_i4CurValue++;
    return m_i4CurValue;
}

tCIDLib::TInt4 TRange::operator++(int)
{
    const tCIDLib::TInt4 i4Ret = m_i4CurValue;
    if (m_i4CurValue < m_i4MaxValue)
        m_i4CurValue++;
    return i4Ret;
}


// ---------------------------------------------------------------------------
//  TRange: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TRange::bAllVisible() const
{
    if (m_c4Visible >= c4FullRange())
        return kCIDLib::True;

    return kCIDLib::False;
}


// Indicates if the current value is at the max
tCIDLib::TBoolean TRange::bAtMax() const
{
    return (m_i4CurValue == m_i4MaxValue);
}


// Indicates if the current value is at the max legal
tCIDLib::TBoolean TRange::bAtLegalMax() const
{
    return (m_i4CurValue == i4MaxLegalValue());
}


// Indicates if the current value is at the min
tCIDLib::TBoolean TRange::bAtMin() const
{
    return (m_i4CurValue == m_i4MinValue);
}


// Indicates if the current value is at the minimum or maximum legal positions
tCIDLib::TBoolean TRange::bAtMinOrLegalMax() const
{
    return (bAtMin() || bAtLegalMax());
}


// Indicates if the current value is the min or max positions
tCIDLib::TBoolean TRange::bAtMinOrMax() const
{
    return (bAtMin() || bAtMax());
}


// Indicates if the passed value is in the visible section of the range
tCIDLib::TBoolean TRange::bIsValueVisible(const tCIDLib::TInt4 i4Value) const
{
    if ((i4Value >= m_i4CurValue)
    &&  (i4Value < m_i4CurValue + tCIDLib::TInt4(m_c4Visible)))
    {
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// Calcs the passed value as percentage of the range, from 0 to 100
tCIDLib::TCard4 TRange::c4CalcPercent(const tCIDLib::TInt4 i4Value) const
{
    if ((i4Value < m_i4MinValue) || (i4Value > m_i4MaxValue))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcRange_PercentValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(i4Value)
            , TInteger(m_i4MinValue)
            , TInteger(m_i4MaxValue)
        );
    }

    // Get the two values in question into floating point form.
    tCIDLib::TFloat4 f4Full = tCIDLib::TFloat4(c4FullRange());
    tCIDLib::TFloat4 f4Cur  = tCIDLib::TFloat4(i4Value - m_i4MinValue);

    // Calculate in floating point and cast to a cardinal for return
    tCIDLib::TCard4 c4Ret = tCIDLib::TCard4(((f4Cur / f4Full) * 100.0) + 0.5);

    // Clip it to a percent
    if (c4Ret > 100.0)
        c4Ret = 100;

    return c4Ret;
}


// Returns the absolute offset of the current value from the bottom of the range
tCIDLib::TCard4 TRange::c4AbsoluteValue() const
{
    return tCIDLib::TCard4(m_i4CurValue - m_i4MinValue);
}


// Returns the full range, inclusive
tCIDLib::TCard4 TRange::c4FullRange() const
{
    return tCIDLib::TCard4(m_i4MaxValue - m_i4MinValue) + 1;
}


// Returns the legal range, inclusive
tCIDLib::TCard4 TRange::c4LegalRange() const
{
    return tCIDLib::TCard4(i4MaxLegalValue() - m_i4MinValue) + 1;
}


// Get or set the amount number of units of the range that are visible
tCIDLib::TCard4 TRange::c4Visible() const
{
    return m_c4Visible;
}

tCIDLib::TCard4 TRange::c4Visible(const tCIDLib::TCard4 c4New)
{
    m_c4Visible = c4New;
    return m_c4Visible;
}


// Returns the current value as a percent of the range, from 0 to 100
tCIDLib::TCard4 TRange::c4CurPercent() const
{
    // Get the two values in question into floating point form.
    tCIDLib::TFloat4 f4Full = tCIDLib::TFloat4(c4FullRange());
    tCIDLib::TFloat4 f4Cur  = tCIDLib::TFloat4(m_i4CurValue - m_i4MinValue);

    // Calculate in floating point and cast to a cardinal for return
    tCIDLib::TCard4 c4Ret = tCIDLib::TCard4(((f4Cur / f4Full) * 100.0) + 0.5);

    // Clip it to a percent
    if (c4Ret > 100.0)
        c4Ret = 100;

    return c4Ret;
}


// Returns the current value as a percent of the legal range
tCIDLib::TCard4 TRange::c4LegalRangePercent() const
{
    // Get the two values in question into floating point form
    tCIDLib::TFloat4 f4Range = tCIDLib::TFloat4(i4MaxLegalValue() - m_i4MinValue);
    tCIDLib::TFloat4 f4Cur   = tCIDLib::TFloat4(m_i4CurValue - m_i4MinValue);

    // Calculate in floating point and cast to a cardinal for return
    tCIDLib::TCard4 c4Ret = tCIDLib::TCard4(((f4Cur / f4Range) * 100.0) + 0.5);

    // Clip it to a percent
    if (c4Ret > 100.0)
        c4Ret = 100;

    return c4Ret;
}


// Indicates the percent of the range that is visible, as a value from 0 to 100
tCIDLib::TCard4 TRange::c4PercentVisible() const
{
    // Get the two values in question into floating point form.
    //
    tCIDLib::TFloat4 f4Full    = tCIDLib::TFloat4(c4FullRange());
    tCIDLib::TFloat4 f4Visible = tCIDLib::TFloat4(m_c4Visible);

    // Calculate in floating point and cast to a cardinal for return
    tCIDLib::TCard4 c4Ret = tCIDLib::TCard4(((f4Visible / f4Full) * 100.0) + 0.5);

    // Clip it to a percent
    if (c4Ret > 100.0)
        c4Ret = 100;

    return c4Ret;
}


// Returns a state value that indicates the current value's basica disposition
tCIDLib::ERangeStates TRange::eState() const
{
    if (m_i4CurValue == m_i4MinValue)
        return tCIDLib::ERangeStates::Min;
    else if (m_i4CurValue == m_i4MaxValue)
        return tCIDLib::ERangeStates::Max;
    else if (m_i4CurValue == i4MaxLegalValue())
        return tCIDLib::ERangeStates::LegalMax;

    return tCIDLib::ERangeStates::None;
}


// Calcs the passed value as percentage of the range, from 0.0 to 1.0
tCIDLib::TFloat8 TRange::f8CalcPercent(const tCIDLib::TInt4 i4Value) const
{
    if ((i4Value < m_i4MinValue) || (i4Value > m_i4MaxValue))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcRange_PercentValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(i4Value)
            , TInteger(m_i4MinValue)
            , TInteger(m_i4MaxValue)
        );
    }

    // Get the two values in question into floating point form.
    tCIDLib::TFloat8 f8Full = tCIDLib::TFloat8(c4FullRange());
    tCIDLib::TFloat8 f8Cur  = tCIDLib::TFloat8(i4Value - m_i4MinValue);

    // Calc percent and handle slight rounding problems
    tCIDLib::TFloat8 f8Ret = (f8Cur / f8Full);

    // Clip it to a percent
    if (f8Ret > 1.0)
        f8Ret = 1.0;

    return f8Ret;
}


// Calcs the current value's percentage within the range
tCIDLib::TFloat8 TRange::f8CurPercent() const
{
    // Get the two values in question into floating point form
    tCIDLib::TFloat8 f8Full = tCIDLib::TFloat8(c4FullRange());
    tCIDLib::TFloat8 f8Cur  = tCIDLib::TFloat8(m_i4CurValue - m_i4MinValue);

    // Calc percent and handle slight rounding problems
    tCIDLib::TFloat8 f8Ret = (f8Cur / f8Full);

    // Clip it to a percent
    if (f8Ret > 1.0)
        f8Ret = 1.0;

    return f8Ret;
}


// Calcs the percent of the range that is legal
tCIDLib::TFloat8 TRange::f8LegalRangePercent() const
{
    // Get the two values in question into floating point form
    tCIDLib::TFloat8 f8Range = tCIDLib::TFloat8(i4MaxLegalValue() - m_i4MinValue);
    if (f8Range == 0.0)
        return 0.0;
    tCIDLib::TFloat8 f8Cur   = tCIDLib::TFloat8(m_i4CurValue - m_i4MinValue);

    // Calc percent and handle slight rounding problems
    tCIDLib::TFloat8 f8Ret = (f8Cur / f8Range);

    // Clip it to a percent
    if (f8Ret > 1.0)
        f8Ret = 1.0;

    return f8Ret;
}


// Calcs the percentage of the range that is visible, as a value from 0.0 to 1.0
tCIDLib::TFloat8 TRange::f8PercentVisible() const
{
    // Get the two values in question into floating point form.
    tCIDLib::TFloat8 f8Full    = tCIDLib::TFloat8(c4FullRange());
    tCIDLib::TFloat8 f8Visible = tCIDLib::TFloat8(m_c4Visible);

    // Calc percent and handle slight rounding problems
    tCIDLib::TFloat8 f8Ret = (f8Visible / f8Full);

    // Clip it to a percent
    if (f8Ret > 1.0)
        f8Ret = 1.0;

    return f8Ret;
}


//
//  Returns a value that is within the valid range. If the value passed is
//  in that range, it is returned as is. If it is below the range, the low
//  range value is returned. If it is above the range, the high range value
//  is returned.
//
tCIDLib::TInt4 TRange::i4ClipToRange(const tCIDLib::TInt4 i4ToClip)
{
    if (i4ToClip < m_i4MinValue)
        return m_i4MinValue;

    const tCIDLib::TInt4 i4Max = i4MaxLegalValue();
    if (i4ToClip > i4Max)
        return i4Max;

    return i4ToClip;
}


// Get or set the current value
tCIDLib::TInt4 TRange::i4CurValue() const
{
    return m_i4CurValue;
}

tCIDLib::TInt4 TRange::i4CurValue(const tCIDLib::TInt4 i4New)
{
    if ((i4New < m_i4MinValue) || (i4New > i4MaxLegalValue()))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcRange_InvalidValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(i4New)
            , TCardinal(c4FullRange())
            , TCardinal(m_c4Visible)
        );
    }

    m_i4CurValue = i4New;
    return i4New;
}


// Get or set the max value. Can't be less than the min
tCIDLib::TInt4 TRange::i4MaxValue() const
{
    return m_i4MaxValue;
}

tCIDLib::TInt4 TRange::i4MaxValue(const tCIDLib::TInt4 i4New)
{
    if (i4New < m_i4MinValue)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcRange_BadMinMax
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(i4New)
            , TInteger(m_i4MinValue)
        );
    }

    m_i4MaxValue = i4New;

    // Make sure we don't leave the current value hanging out
    if (m_i4CurValue > i4MaxLegalValue())
        m_i4CurValue = i4MaxLegalValue();

    return i4New;
}


// Returns the current maximum legal value
tCIDLib::TInt4 TRange::i4MaxLegalValue() const
{
    // Get the full range for quick access
    tCIDLib::TCard4 c4Full = c4FullRange();

    //
    //  If we have more items than are visible, then the max value is
    //  (full range minus visible) above the min value.
    //
    if (c4Full > m_c4Visible)
        return m_i4MinValue + (c4Full - m_c4Visible);

    //
    //  If there are more visible than there are items, then we never
    //  get off the min value because we never have to scroll.
    //
    return m_i4MinValue;
}


// Get or set the min value. Can't be more than the max
tCIDLib::TInt4 TRange::i4MinValue() const
{
    return m_i4MinValue;
}

tCIDLib::TInt4 TRange::i4MinValue(const tCIDLib::TInt4 i4New)
{
    if (i4New > m_i4MaxValue)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcRange_BadMinMax
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(m_i4MaxValue)
            , TInteger(i4New)
        );
    }

    m_i4MinValue = i4New;

    // Make sure we don't leave the current value hanging out
    if (m_i4CurValue < m_i4MinValue)
        m_i4CurValue = m_i4MinValue;

    return i4New;
}


// Sets all of the values at once. They are validated before storing
tCIDLib::TVoid TRange::SetAll(  const   tCIDLib::TCard4 c4Visible
                                , const tCIDLib::TInt4  i4CurValue
                                , const tCIDLib::TInt4  i4MinValue
                                , const tCIDLib::TInt4  i4MaxValue)
{
    //
    //  Create a temp first. If any of the parms were illegal, we will
    //  get an exception and never affect this object.
    //
    TRange  rngTest(c4Visible, i4CurValue, i4MinValue, i4MaxValue);

    // We did not get an exception so copy it in
    m_c4Visible  = c4Visible;
    m_i4CurValue = i4CurValue;
    m_i4MinValue = i4MinValue;
    m_i4MaxValue = i4MaxValue;
}


// Set the current value to the maximum legal value
tCIDLib::TVoid TRange::SetToMaxLegalValue()
{
    m_i4MinValue = i4MaxLegalValue();
}


// ---------------------------------------------------------------------------
//  TRange: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TRange::FormatTo(TTextOutStream& strmToWriteTo) const
{
    strmToWriteTo << facCIDLib().strMsg(kCIDMsgs::midGen_Value) << L"="
                  << m_i4CurValue
                  << facCIDLib().strMsg(kCIDMsgs::midGen_Min) << L"="
                  << m_i4MinValue
                  << facCIDLib().strMsg(kCIDMsgs::midGen_Max) << L"="
                  << m_i4MaxValue
                  << facCIDLib().strMsg(kCIDMsgs::midGen_Visible) << L"="
                  << m_c4Visible;
}

tCIDLib::TVoid TRange::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Parse the data fields
    strmToReadFrom  >> m_c4Visible
                    >> m_i4CurValue
                    >> m_i4MinValue
                    >> m_i4MaxValue;

    // Validate it
    Validate(CID_LINE);
}

tCIDLib::TVoid TRange::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Write out the data fields
    strmToWriteTo   << m_c4Visible
                    << m_i4CurValue
                    << m_i4MinValue
                    << m_i4MaxValue;
}


// ---------------------------------------------------------------------------
//  TRange: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Validates the current settings for consistency. If not valid, then we
//  log a fatal error. By passing in the name by value, we prevent having
//  to construct strings objects unless there's an error!
//
tCIDLib::TVoid TRange::Validate(const tCIDLib::TCard4 c4Line)
{
    if (m_i4MaxValue < m_i4MinValue)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , c4Line
            , kCIDErrs::errcRange_BadMinMax
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(m_i4MaxValue)
            , TInteger(m_i4MinValue)
        );
    }

    if ((m_i4CurValue < m_i4MinValue) || (m_i4CurValue > i4MaxLegalValue()))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , c4Line
            , kCIDErrs::errcRange_InvalidValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TInteger(m_i4CurValue)
            , TCardinal(c4FullRange())
            , TCardinal(m_c4Visible)
        );
    }
}
