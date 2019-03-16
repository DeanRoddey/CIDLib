//
// FILE NAME: CIDMath_Range.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/08/1993
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
//  This is the header for the CIDMath_Range.cpp file. It provides the
//  TRange class. TRange is a small class that provides min/max values
//  for the range, a current value within that range, and an amount of the
//  range that is 'visible'.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TRange
//  PREFIX: rng
// ---------------------------------------------------------------------------
class CIDMATHEXP TRange : public TObject, public MStreamable, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRange();

        TRange
        (
            const   tCIDLib::TCard4         c4Visible
            , const tCIDLib::TInt4          i4CurValue
            , const tCIDLib::TInt4          i4MinValue
            , const tCIDLib::TInt4          i4MaxValue
        );

        TRange(const TRange&) = default;

        ~TRange();


        // -------------------------------------------------------------------
        //  Public operator
        // -------------------------------------------------------------------
        TRange& operator=(const TRange&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TRange&                 rngToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TRange&                 rngToTest
        )   const;

        tCIDLib::TInt4 operator--();

        tCIDLib::TInt4 operator--(int);

        tCIDLib::TInt4 operator++();

        tCIDLib::TInt4 operator++(int);



        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAllVisible() const;

        tCIDLib::TBoolean bAtMax() const;

        tCIDLib::TBoolean bAtLegalMax() const;

        tCIDLib::TBoolean bAtMin() const;

        tCIDLib::TBoolean bAtMinOrLegalMax() const;

        tCIDLib::TBoolean bAtMinOrMax() const;

        tCIDLib::TBoolean bIsValueVisible
        (
            const   tCIDLib::TInt4          i4Value
        )   const;

        tCIDLib::TCard4 c4AbsoluteValue() const;

        tCIDLib::TCard4 c4CalcPercent
        (
            const   tCIDLib::TInt4          i4Value
        )   const;

        tCIDLib::TCard4 c4CurPercent() const;

        tCIDLib::TCard4 c4LegalRangePercent() const;

        tCIDLib::TCard4 c4FullRange() const;

        tCIDLib::TCard4 c4LegalRange() const;

        tCIDLib::TCard4 c4PercentVisible() const;

        tCIDLib::TCard4 c4Visible() const;

        tCIDLib::TCard4 c4Visible
        (
            const   tCIDLib::TCard4         c4New
        );

        tCIDLib::ERangeStates eState() const;

        tCIDLib::TFloat8 f8CalcPercent
        (
            const   tCIDLib::TInt4          i4Value
        )   const;

        tCIDLib::TFloat8 f8CurPercent() const;

        tCIDLib::TFloat8 f8LegalRangePercent() const;

        tCIDLib::TFloat8 f8PercentVisible() const;

        tCIDLib::TInt4 i4ClipToRange
        (
            const   tCIDLib::TInt4          i4ToClip
        );

        tCIDLib::TInt4 i4CurValue() const;

        tCIDLib::TInt4 i4CurValue
        (
            const   tCIDLib::TInt4          i4New
        );

        tCIDLib::TInt4 i4MaxLegalValue() const;

        tCIDLib::TInt4 i4MinValue() const;

        tCIDLib::TInt4 i4MinValue
        (
            const   tCIDLib::TInt4          i4New
        );

        tCIDLib::TInt4 i4MaxValue() const;

        tCIDLib::TInt4 i4MaxValue
        (
            const   tCIDLib::TInt4          i4New
        );

        tCIDLib::TVoid SetAll
        (
            const   tCIDLib::TCard4         c4Visible
            , const tCIDLib::TInt4          i4CurValue
            , const tCIDLib::TInt4          i4MinValue
            , const tCIDLib::TInt4          i4MaxValue
        );

        tCIDLib::TVoid SetToMaxLegalValue();


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmToWriteTo
        )   const override;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Validate
        (
            const   tCIDLib::TCard4         c4Line
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Visible
        //      The number of the items in the range that are visible. If this
        //      is set to 1, then it basically has no effect and the range is
        //      just like a range without a 'visible count'. The max legal
        //      range will be the same as the max value.
        //
        //  m_i4CurValue
        //      This is the current value within the range.
        //
        //  m_i4MinValue
        //  m_i4MaxValue
        //      These define the current range. The min is always less than
        //      or equal to the max.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Visible;
        tCIDLib::TInt4  m_i4CurValue;
        tCIDLib::TInt4  m_i4MinValue;
        tCIDLib::TInt4  m_i4MaxValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TRange,TObject)
};

#pragma CIDLIB_POPPACK


