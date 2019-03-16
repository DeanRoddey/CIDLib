//
// FILE NAME: CIDLib_Size.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/10/1996
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
//  This is the header for the CIDLib_Size.Cpp file, which implements the
//  TSize class. This class represents a positive extent in a 2D cardinal
//  coordinate space.
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
//   CLASS: TSize
//  PREFIX: sz
// ---------------------------------------------------------------------------
class CIDLIBEXP TSize :

    public TObject, public MStreamable, public MDuplicable, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSize() = default;

        TSize
        (
            const   tCIDLib::THostSize&     szToCopy
        );

        TSize
        (
            const   tCIDLib::TRawSize&      szToCopy
        );

        TSize(const tCIDLib::TCard4 c4X, const tCIDLib::TCard4 c4Y) :

            m_c4CX(c4X)
            , m_c4CY(c4Y)
        {
        }

        explicit TSize(const tCIDLib::TCard4 c4Both) :

            m_c4CX(c4Both)
            , m_c4CY(c4Both)
        {
        }

        TSize(const TSize& szSrc) = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        operator const tCIDLib::THostSize&() const;

        TSize& operator=(const TSize&) = default;

        tCIDLib::TVoid operator=
        (
            const   tCIDLib::TRawSize&      szSrc
        );

        tCIDLib::TVoid operator+=
        (
            const   TSize&                  szToAdd
        );

        friend TSize CIDLIBEXP operator+
        (
            const   TSize&                  sz1
            , const TSize&                  sz2
        );

        tCIDLib::TVoid operator-=
        (
            const   TSize&                  szToSub
        );

        friend TSize CIDLIBEXP operator-
        (
            const   TSize&                  sz1
            , const TSize&                  sz2
        );

        constexpr tCIDLib::TBoolean operator==(const TSize& szToTest) const
        {
            return (m_c4CX == szToTest.m_c4CX) && (m_c4CY == szToTest.m_c4CY);
        }

        constexpr tCIDLib::TBoolean operator!=(const TSize& szToTest) const
        {
            return !operator==(szToTest);
        }

        tCIDLib::TVoid operator|=
        (
            const   TSize&                  szToOR
        );

        friend TSize CIDLIBEXP operator|
        (
            const   TSize&                  szOR1
            , const TSize&                  szOR2
        );

        tCIDLib::TVoid operator&=
        (
            const   TSize&                  szToAND
        );

        friend TSize CIDLIBEXP operator&
        (
            const   TSize&                  szAND1
            , const TSize&                  szAND2
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Adjust
        (
            const   tCIDLib::TInt4          i4XOfs
            , const tCIDLib::TInt4          i4YOfs
        );

        tCIDLib::TVoid Adjust
        (
            const   tCIDLib::TInt4          i4Ofs
        );

        tCIDLib::TVoid AdjustCX
        (
            const   tCIDLib::TInt4          i4Ofs
        );

        tCIDLib::TVoid AdjustCY
        (
            const   tCIDLib::TInt4          i4Ofs
        );

        tCIDLib::TBoolean bAllZero() const
        {
            return (!m_c4CX && !m_c4CY);
        }

        tCIDLib::TBoolean bEitherLargerThan
        (
            const   TSize&                  szThan
        )   const;

        tCIDLib::TBoolean bEitherZero() const
        {
            return (!m_c4CX || !m_c4CY);
        }

        tCIDLib::TBoolean bParseFromText
        (
            const   TString&                strText
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
            , const tCIDLib::TCh            chSepChar = L','
        );

        tCIDLib::TCard4 c4Packed() const;

        constexpr tCIDLib::TCard4 c4Height() const
        {
            return m_c4CY;
        }

        tCIDLib::TCard4 c4Height(const tCIDLib::TCard4 c4New)
        {
            m_c4CY = c4New;
            return m_c4CY;
        }

        constexpr tCIDLib::TCard4 c4SquareUnits() const
        {
            return m_c4CX * m_c4CY;
        }

        constexpr tCIDLib::TCard4 c4Width() const
        {
            return m_c4CX;
        }

        tCIDLib::TCard4 c4Width(const tCIDLib::TCard4 c4New)
        {
            m_c4CX = c4New;
            return m_c4CX;
        }

        constexpr tCIDLib::TCard4 c4X() const
        {
            return m_c4CX;
        }

        tCIDLib::TCard4 c4X(tCIDLib::TCard4 c4XNew)
        {
            m_c4CX = c4XNew;
            return m_c4CX;
        }

        constexpr tCIDLib::TCard4 c4Y() const
        {
            return m_c4CY;
        }

        tCIDLib::TCard4 c4Y(tCIDLib::TCard4 c4YNew)
        {
            m_c4CY = c4YNew;
            return m_c4CY;
        }

        tCIDLib::TVoid Exchange()
        {
            tCIDLib::TCard4 i4Tmp = m_c4CX;
            m_c4CX = m_c4CY;
            m_c4CY = i4Tmp;
        }

        tCIDLib::TVoid FormatToText
        (
                    TString&                strToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
            , const tCIDLib::TCh            chSepChar = L','
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        )   const;

        tCIDLib::TVoid ParseFromText
        (
            const   TString&                strText
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
            , const tCIDLib::TCh            chSepChar = L','
        );

        tCIDLib::TVoid Scale
        (
            const   tCIDLib::TFloat8&       f8HScale
            , const tCIDLib::TFloat8&       f8VScale
        );

        tCIDLib::TVoid Scale
        (
            const   tCIDLib::TFloat8&       f8Scale
        );

        tCIDLib::TVoid ScaleAR
        (
            const   TSize&                  szTarArea
        );

        tCIDLib::TVoid ScaleToWidthAR
        (
            const   tCIDLib::TCard4         c4NewWidth
        );

        tCIDLib::TVoid ScaleToHeightAR
        (
            const   tCIDLib::TCard4         c4NewHeight
        );

        tCIDLib::TVoid Set(const tCIDLib::TCard4 c4CXNew, const tCIDLib::TCard4 c4CYNew)
        {
            m_c4CX = c4CXNew;
            m_c4CY = c4CYNew;
        }

        tCIDLib::TVoid SetFromPacked
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid TakeLarger
        (
            const   TSize&                  szOther
        );

        tCIDLib::TVoid TakeSmaller
        (
            const   TSize&                  szOther
        );

        tCIDLib::TVoid ToHostPt
        (
                    tCIDLib::THostPoint&    ptToFill
        )   const;

        tCIDLib::TVoid ToRawSize
        (
                    tCIDLib::TRawSize&      szToFill
        )   const;

        tCIDLib::TVoid ToHostSize
        (
                    tCIDLib::THostSize&     szToFill
        )   const;

        tCIDLib::TVoid Zero()
        {
            m_c4CX = 0;
            m_c4CY = 0;
        }


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
        ) override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4CX
        //  m_c4CY
        //      These are the extents in the x and y axes.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4CX = 0;
        tCIDLib::TCard4     m_c4CY = 0;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSize,TObject)
        DefPolyDup(TSize)
        NulObject(TSize)
};

#pragma CIDLIB_POPPACK



// ---------------------------------------------------------------------------
//  Alias TSize to another name, TQ1Point. Since a cartesian point in the
//  1st quadrant has basically the same functionality, we use this class for
//  both. It has methods to look at the values either way, i.e. as a size or
//  as a point value.
// ---------------------------------------------------------------------------
using TQ1Point = TSize;
