//
// FILE NAME: CIDLib_Point.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/10/1996
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
//  This is the header for the CIDLib_Point.Cpp file. This file implements
//  the TPoint class, which is a point in a 2 dimensional, integral
//  coordinate space.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class   TArea;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TPoint
//  PREFIX: pnt
// ---------------------------------------------------------------------------
class CIDLIBEXP TPoint :

    public TObject, public MStreamable, public MDuplicable, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TPoint pntOrigin;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPoint() = default;

        explicit TPoint
        (
            const   tCIDLib::THostPoint&    ptSrc
        );

        TPoint
        (
            const   tCIDLib::TRawPoint&     ptSrc
        );

        TPoint(const tCIDLib::TInt4 i4X, const tCIDLib::TInt4 i4Y) :

            m_i4X(i4X)
            , m_i4Y(i4Y)
        {
        }

        explicit TPoint(const tCIDLib::TInt4 i4Both) :

            m_i4X(i4Both)
            , m_i4Y(i4Both)
        {
        }

        TPoint(const TPoint&) = default;
        TPoint(TPoint&&) = default;

        ~TPoint() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPoint& operator=(const TPoint&) = default;
        TPoint& operator=(TPoint&&) = default;

        explicit operator const tCIDLib::THostPoint*() const;

        explicit operator tCIDLib::THostPoint*();

        TPoint& operator=
        (
            const   tCIDLib::TRawPoint&     ptSrc
        );

        TPoint& operator=
        (
            const   tCIDLib::THostPoint&    ptSrc
        );

        tCIDLib::TVoid operator+=
        (
            const   TPoint&                 pntToAdd
        );

        tCIDLib::TVoid operator-=
        (
            const   TPoint&                 pntToSub
        );

        constexpr tCIDLib::TBoolean operator==(const TPoint& pntToTest) const
        {
            return (m_i4X == pntToTest.m_i4X) && (m_i4Y == pntToTest.m_i4Y);
        }

        constexpr tCIDLib::TBoolean operator!=(const TPoint& pntToTest) const
        {
            return !operator==(pntToTest);
        }

        friend TPoint CIDLIBEXP operator+
        (
            const   TPoint&                 pnt1
            , const TPoint&                 pnt2
        );

        friend TPoint CIDLIBEXP operator-
        (
            const   TPoint&                 pnt1
            , const TPoint&                 pnt2
        );


        // -------------------------------------------------------------------
        //  Pubic, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AbsDiffFrom
        (
            const   TPoint&                 pntOther
            ,       TPoint&                 pntDiff
        )   const;

        tCIDLib::TVoid Adjust
        (
            const   tCIDLib::TInt4          i4XOfs
            , const tCIDLib::TInt4          i4YOfs
        );

        tCIDLib::TVoid AdjustX
        (
            const   tCIDLib::TInt4          i4Ofs
        );

        tCIDLib::TVoid AdjustY
        (
            const   tCIDLib::TInt4          i4Ofs
        );

        tCIDLib::TBoolean bAtOrg() const
        {
            return (!m_i4X && !m_i4Y);
        }

        tCIDLib::TBoolean bInArea
        (
            const   TArea&                  areaToTest
        )   const;

        tCIDLib::TBoolean bParseFromText
        (
            const   TString&                strText
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
            , const tCIDLib::TCh            chSepChar = kCIDLib::chComma
        );

        tCIDLib::TCard4 c4AbsDiff
        (
            const   TPoint&                 pntOther
        )   const;

        tCIDLib::TCard4 c4Packed() const;

        tCIDLib::TVoid Exchange()
        {
            const tCIDLib::TInt4 i4Tmp = m_i4X;
            m_i4X=m_i4Y;
            m_i4Y=i4Tmp;
        }

        tCIDLib::TVoid FormatToText
        (
                    TString&                strToFill
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
            , const tCIDLib::TCh            chSepChar = kCIDLib::chComma
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        )   const;

        tCIDLib::TVoid ForceWithin
        (
            const   TArea&                  areaLimits
        );

        tCIDLib::TVoid FromPolarDegrees
        (
            const   tCIDLib::TCard4         c4Theta
            , const tCIDLib::TCard4         c4Radius
        );

        tCIDLib::TVoid FromPolarRadians
        (
            const   tCIDLib::TFloat8&       f8Theta
            , const tCIDLib::TCard4         c4Radius
        );

        tCIDLib::TInt4 i4LargestDim() const;

        constexpr tCIDLib::TInt4 i4X() const
        {
            return m_i4X;
        }

        tCIDLib::TInt4 i4X(tCIDLib::TInt4 i4XNew)
        {
            m_i4X = i4XNew;
            return m_i4X;
        }

        constexpr tCIDLib::TInt4 i4Y() const
        {
            return m_i4Y;
        }

        tCIDLib::TInt4 i4Y(tCIDLib::TInt4 i4YNew)
        {
            m_i4Y = i4YNew;
            return m_i4Y;
        }

        tCIDLib::TVoid Negate()
        {
            m_i4X *= -1;
            m_i4Y *= -1;
        }

        tCIDLib::TVoid NegateX()
        {
            m_i4X *= -1;
        }

        tCIDLib::TVoid NegateY()
        {
            m_i4Y *= -1;
        }

        tCIDLib::TVoid Scale
        (
            const   tCIDLib::TFloat8        f8CXScale
            , const tCIDLib::TFloat8        f8CYScale
        );

        tCIDLib::TVoid Set(const tCIDLib::TInt4 i4XNew, const tCIDLib::TInt4 i4YNew)
        {
            m_i4X = i4XNew;
            m_i4Y = i4YNew;
        }

        tCIDLib::TVoid TakeLarger
        (
            const   TPoint&                 pntOther
        );

        tCIDLib::TVoid TakeSmaller
        (
            const   TPoint&                 pntOther
        );

        tCIDLib::TVoid ToHostPt
        (
                    tCIDLib::THostPoint&    ptToFill
        )   const;

        tCIDLib::TVoid ToRawPt
        (
                    tCIDLib::TRawPoint&     ptToFill
        )   const;

        tCIDLib::TVoid ToPolarDegrees
        (
                    tCIDLib::TCard4&        c4Theta
            ,       tCIDLib::TCard4&        c4Radius
        )   const;

        tCIDLib::TVoid ToPolarRadians
        (
                    tCIDLib::TFloat8&       f8Theta
            ,       tCIDLib::TCard4&        c4Radius
        )   const;

        tCIDLib::TVoid Zero()
        {
            m_i4X = 0;
            m_i4Y = 0;
        }


    protected   :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class    TArea;


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmToWriteTo
        )   const final;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const final;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  Note that these are packed to 4 bytes so that they will be
        //  just like a raw host point structure.
        //
        //  m_i4X
        //  m_i4Y
        //      Storage for the x,y coordinates.
        // -------------------------------------------------------------------
        #pragma CIDLIB_PACK(4)
        tCIDLib::TInt4  m_i4X = 0;
        tCIDLib::TInt4  m_i4Y = 0;
        #pragma CIDLIB_POPPACK


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TPoint,TObject)
        DefPolyDup(TPoint)
        NulObject(TPoint)
};

#pragma CIDLIB_POPPACK

