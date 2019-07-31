//
// FILE NAME: CIDLib_Area.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/23/1993
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
//  This is the header for the CIDLib_Area.Cpp file. This file implements the TArea
//  class, which is a 2D rectangular area with a signed upper left origin and an
//  unsigned size in the positive direction of each axis (left to right, top to
//  bottom.) I.e. its a 4th quadrant coordinate system.
//
//  An area of 10x10 at 10,10 will cover the pixels from 10,10 to 19,19, since 10,10
//  is the first pixel.
//
//  See the docs for  discussion of inclusive vs exclusive rectanges and such.
//  When converting to/from system rectangles, or UL/LR points, we have to be
//  careful to follow specific rules. Rectangles can be inclusive or non-inclusive.
//  Areas are neither, they are (origin - size) based.
//
//  Inclusive Rectl to Area
//
//      The resulting area includes the outer edge. So 0,0,1,1 becomes 0,0 - 2,2
//      so it's 2 pixels wide.
//
//  Non-Inclusive Rectl to Area
//
//      The resulting area excludes the outer edge. So 0,0,1,1 becomes 0,0 - 1,1
//      so it's 1 pixel wide.
//
//  Area to Inculsive Rectl
//
//      An area of 0,0 - 1,1 is 1 pixel in size, and the inclusive rectangle includes
//      the edges, so it becomes 0,0,0,0.
//
//  Area to Non-Inculsive Rectl
//
//      An area of 0,0 - 1,1 is 1 pixel in size, and the inclusive rectangle does not
//      include the edges, so it has to become 0,0,1,1.
//
//
//  From UL/LR points to Area
//  From Area to UL/LR points.
//      The points are assumed to represent points on an non-inclusive rectangle, so
//      the same as the non-inclusive rect to area scenario above, and vice versa.
//
//      So points 10,10 and 20,20 create an area of 10x10, because the area does not
//      include the 20 coordinates.It goes 10 to 19.
//
//      This may seem non-intuitive, but ultimately it's a lot more sane. Otherwise
//      you can't create an empty area, or tell the difference between an empty or
//      1x1 rectangle when you convert it to an area.
//
//  i4Right()/i4Bottom
//
//      These return the right/bottom points of the area. They return inclusive
//      coordinates. If they were non-inclusive, they would actually return the first
//      point past the right/bottom. For an empty area the left/top is the same as
//      the right/bottom. For x=1,cx=1, you will get 1, because pixel 1 is the only
//      pixel in the area horizontally.
//
//      When setting them, it's also assumed to be inclusive. So if x-1 and you set
//      right=1, then the width will be 1. You can't set an empty area this way.
//
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
//   CLASS: TArea
//  PREFIX: area
// ---------------------------------------------------------------------------
class CIDLIBEXP TArea :

    public TObject, public MStreamable, public MDuplicable, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static members
        // -------------------------------------------------------------------
        static const TArea  areaEmpty;


        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TArea() = default;

        TArea
        (
            const   tCIDLib::THostRectl&    rectlToCopy
            , const tCIDLib::ERectlTypes    eInclusive
        );

        explicit TArea
        (
            const   tCIDLib::TRawArea&      areaToCopy
        );

        TArea
        (
            const   tCIDLib::THostPoint&    ptULeft
            , const tCIDLib::THostPoint&    ptLRight
            , const tCIDLib::ERectlTypes    eInclusive
        );

        TArea
        (
            const   TPoint&                 pntULeft
            , const TPoint&                 pntLRight
            , const tCIDLib::ERectlTypes    eInclusive
        );

        TArea
        (
            const   tCIDLib::TInt4          i4X
            , const tCIDLib::TInt4          i4Y
            , const tCIDLib::TCard4         c4CX
            , const tCIDLib::TCard4         c4CY
        );

        TArea
        (
            const   TPoint&                 pntULeft
            , const tCIDLib::TCard4         c4CX
            , const tCIDLib::TCard4         c4CY
        );

        TArea
        (
            const   TPoint&                 pntULeft
            , const TSize&                  szExtent
        );

        TArea(const TArea&) = default;

        ~TArea() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TArea& operator=(const TArea&) = default;

        TArea& operator=
        (
            const   tCIDLib::TRawArea&      areaToAssign
        );

        friend TArea CIDLIBEXP operator|
        (
            const   TArea&                  area1
            , const TArea&                  area2
        );

        tCIDLib::TVoid operator|=
        (
            const   TArea&                  areaToOR
        );

        friend TArea CIDLIBEXP operator-
        (
            const   TArea&                  area1
            , const TArea&                  area2
        );

        tCIDLib::TVoid operator-=
        (
            const   TArea&                  areaToSub
        );

        friend TArea CIDLIBEXP operator&
        (
            const   TArea&                  area1
            , const TArea&                  area2
        );

        tCIDLib::TVoid operator&=
        (
            const   TArea&                  areaToAND
        );

        tCIDLib::TBoolean operator==
        (
            const   TArea&                  areaToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TArea&                  areaToTest
        )   const;

        tCIDLib::TVoid operator+=
        (
            const   TPoint&                 pntOffset
        );

        tCIDLib::TVoid operator-=
        (
            const   TPoint&                 pntOffset
        );


        // No addition operator
        TArea operator+=(const TArea&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdjustAndShrink
        (
            const   tCIDLib::TCard4         c4XAdj
            , const tCIDLib::TCard4         c4YAdj
        );

        tCIDLib::TVoid AdjustOrg
        (
            const   tCIDLib::TInt4          i4XOfs
            , const tCIDLib::TInt4          i4YOfs
        );

        tCIDLib::TVoid AdjustOrg
        (
            const   TPoint&                 pntOfs
        );

        tCIDLib::TVoid AdjustSides(const TPoint& pntOfs)
        {
            DoAdjustments(pntOfs.m_i4X, pntOfs.m_i4Y);
        }

        tCIDLib::TVoid
        AdjustSides(const tCIDLib::TInt4 i4XOfs, const tCIDLib::TInt4 i4YOfs)
        {
            DoAdjustments(i4XOfs, i4YOfs);
        }

        tCIDLib::TVoid AdjustSize
        (
            const   tCIDLib::TInt4          i4XOfs
            , const tCIDLib::TInt4          i4YOfs
        );

        tCIDLib::TVoid AdjustSize
        (
            const   TPoint&                 pntOfs
        );

        tCIDLib::TBoolean bContainsArea
        (
            const   TArea&                  areaTest
        )   const;

        tCIDLib::TBoolean bContainsPoint
        (
            const   tCIDLib::TInt4          i4X
            , const tCIDLib::TInt4          i4Y
        )   const;

        tCIDLib::TBoolean bContainsPoint(const TPoint& pntTest) const
        {
            return bContainsPoint(pntTest.m_i4X, pntTest.m_i4Y);
        }

        tCIDLib::TBoolean bHorzOverlap
        (
            const   TArea&                  areaToTest
        )   const;

        constexpr tCIDLib::TBoolean bIsEmpty() const
        {
            return (m_c4CX == 0) || (m_c4CY == 0);
        }

        tCIDLib::TBoolean bIntersects
        (
            const   TArea&                  areaToTest
        )   const;

        tCIDLib::TBoolean bIsInside
        (
            const   TArea&                  areaToTest
        )   const;

        tCIDLib::TBoolean bMoveWithin
        (
                    TPoint&                 pntToMove
        )   const;

        tCIDLib::TBoolean bParseFromText
        (
            const   TString&                strText
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
            , const tCIDLib::TCh            chSepChar = L','
        );

        tCIDLib::TBoolean bPercentFromOrg
        (
            const   TPoint&                 pntToTest
            ,       tCIDLib::TFloat8&       f8XPercent
            ,       tCIDLib::TFloat8&       f8YPercent
        )   const;

        tCIDLib::TBoolean bSameOrg
        (
            const   TArea&                  areaToTest
        )   const;

        tCIDLib::TBoolean bSameSize
        (
            const   TArea&                  areaToTest
        )   const;

        tCIDLib::TBoolean bVertOverlap
        (
            const   TArea&                  areaToTest
        )   const;

        tCIDLib::TVoid BottomAlign
        (
            const   tCIDLib::TInt4          i4NewBottom
        );

        tCIDLib::TVoid BottomJustifyIn
        (
            const   TArea&                  areaToJustifyIn
            , const tCIDLib::TBoolean       bHCenter = kCIDLib::False
        );

        constexpr tCIDLib::TCard4 c4Height() const
        {
            return m_c4CY;
        }

        tCIDLib::TCard4 c4Height(const tCIDLib::TCard4 c4NewCY)
        {
            m_c4CY = c4NewCY;
            return m_c4CY;
        }

        constexpr tCIDLib::TCard4 c4SquareUnits() const
        {
            return (m_c4CX * m_c4CY);
        }

        constexpr tCIDLib::TCard4 c4Width() const
        {
            return m_c4CX;
        }

        tCIDLib::TCard4 c4Width(const tCIDLib::TCard4 c4NewCX)
        {
            m_c4CX = c4NewCX;
            return m_c4CX;
        }

        tCIDLib::TVoid CenterIn
        (
            const   TArea&                  areaTarget
        );

        tCIDLib::TVoid CenterOn
        (
            const   TPoint&                 pntTarget
        );

        tCIDLib::TVoid Deflate(const tCIDLib::TCard4 c4XOfs, const tCIDLib::TCard4 c4YOfs)
        {
            DoAdjustments(-tCIDLib::TInt4(c4XOfs), -tCIDLib::TInt4(c4YOfs));
        }

        tCIDLib::TVoid Deflate(const tCIDLib::TCard4 c4Ofs)
        {
            DoAdjustments(-tCIDLib::TInt4(c4Ofs), -tCIDLib::TInt4(c4Ofs));
        }

        tCIDLib::TVoid FromAreaAtOrg
        (
            const   TArea&                  areaSource
        );

        tCIDLib::TVoid FromAreaScaled
        (
            const   TArea&                  areaSource
            , const tCIDLib::TFloat8        f8CXScale
            , const tCIDLib::TFloat8        f8CYScale
        );

        tCIDLib::TVoid FromPoints
        (
            const   TPoint&                 pntULeft
            , const TPoint&                 pntLRight
            , const tCIDLib::ERectlTypes    eInclusive
        );

        tCIDLib::TVoid FromPoints
        (
            const   tCIDLib::THostPoint&    ptULeft
            , const tCIDLib::THostPoint&    ptLRight
            , const tCIDLib::ERectlTypes    eInclusive
        );

        tCIDLib::TVoid FromPoints
        (
            const   tCIDLib::TRawPoint&     ptULeft
            , const tCIDLib::TRawPoint&     ptLRight
            , const tCIDLib::ERectlTypes    eInclusive
        );

        tCIDLib::TVoid FromRectl
        (
            const   tCIDLib::THostRectl&    rectlSrc
            , const tCIDLib::ERectlTypes    eInclusive
        );

        tCIDLib::TVoid ForceWithin
        (
            const   TArea&                  areaToFit
            , const tCIDLib::TBoolean       bTotal = kCIDLib::False
        );

        tCIDLib::TVoid FormatToText
        (
                    TString&                strTar
            , const tCIDLib::TCh            chSepChar = kCIDLib::chComma
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        )   const;

        tCIDLib::TInt4 i4Bottom() const;

        tCIDLib::TInt4 i4Bottom
        (
            const   tCIDLib::TInt4          i4NewBottom
        );

        constexpr tCIDLib::TInt4 i4Left() const
        {
            return m_i4X;
        }

        tCIDLib::TInt4 i4Left
        (
            const   tCIDLib::TInt4          i4NewLeft
            , const tCIDLib::TBoolean       bLockRight = kCIDLib::False
        );

        tCIDLib:: TInt4 i4Right() const;

        tCIDLib:: TInt4 i4Right
        (
            const   tCIDLib::TInt4          i4XRight
        );

        constexpr tCIDLib::TInt4 i4Top() const
        {
            return m_i4Y;
        }

        tCIDLib::TInt4 i4Top
        (
            const   tCIDLib::TInt4          i4YTop
            , const tCIDLib::TBoolean       bLockBottom = kCIDLib::False
        );

        constexpr tCIDLib::TInt4 i4X() const
        {
            return m_i4X;
        }

        tCIDLib::TInt4 i4X
        (
            const   tCIDLib::TInt4          i4NewX
            , const tCIDLib::TBoolean       bLockRight = kCIDLib::False
        );

        tCIDLib::TInt4 i4Y() const
        {
            return m_i4Y;
        }

        tCIDLib::TInt4 i4Y
        (
            const   tCIDLib::TInt4          i4NewY
            , const tCIDLib::TBoolean       bLockBottom = kCIDLib::False
        );

        tCIDLib::TVoid Inflate( const tCIDLib::TCard4 c4XOfs, const tCIDLib::TCard4 c4YOfs)
        {
            DoAdjustments(c4XOfs, c4YOfs);
        }

        tCIDLib::TVoid Inflate(const tCIDLib::TCard4 c4Ofs)
        {
            DoAdjustments(c4Ofs, c4Ofs);
        }

        tCIDLib::TVoid JustifyIn
        (
            const   TArea&                  areaToJustifyIn
            , const tCIDLib::EHJustify      eHJustify
            , const tCIDLib::EVJustify      eVJustify
        );

        tCIDLib::TVoid LeftJustifyIn
        (
            const   TArea&                  areaToJustifyIn
            , const tCIDLib::TBoolean       bVCenter = kCIDLib::False
        );

        tCIDLib::TVoid Move
        (
            const   tCIDLib::EDirs          eDir
        );

        tCIDLib::TVoid NegateOrg()
        {
            m_i4X *= -1;
            m_i4Y *= -1;
        }

        TPoint pntCenter() const;

        TPoint pntLL(const tCIDLib::TInt4 i4XAdj= 0, const tCIDLib::TInt4 i4YAdj = 0) const
        {
            return TPoint(m_i4X + i4XAdj, i4Bottom() + i4YAdj);
        }

        TPoint pntLR(const tCIDLib::TInt4 i4XAdj = 0, const tCIDLib::TInt4 i4YAdj = 0) const
        {
            return TPoint(i4Right() + i4XAdj, i4Bottom() + i4YAdj);
        }

        TPoint pntOrg() const
        {
            return TPoint(m_i4X, m_i4Y);
        }

        TPoint pntUL(const tCIDLib::TInt4 i4XAdj = 0, const tCIDLib::TInt4 i4YAdj = 0) const
        {
            return TPoint(m_i4X + i4XAdj, m_i4Y + i4YAdj);
        }

        TPoint pntUR(const tCIDLib::TInt4 i4XAdj = 0, const tCIDLib::TInt4 i4YAdj = 0) const
        {
            return TPoint(i4Right() + i4XAdj, m_i4Y + i4YAdj);
        }

        tCIDLib::TVoid ParseFromText
        (
            const   TString&                strText
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
            , const tCIDLib::TCh            chSepChar = kCIDLib::chComma
        );

        tCIDLib::TVoid RightAlign
        (
            const   tCIDLib::TInt4          i4NewRight
        );

        tCIDLib::TVoid RightJustifyIn
        (
            const   TArea&                  areaToJustifyIn
            , const tCIDLib::TBoolean       bVCenter = kCIDLib::False
        );

        tCIDLib::TVoid ScaleSize
        (
            const   tCIDLib::TFloat8        f8CXScale
            , const tCIDLib::TFloat8        f8CYScale
        );

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TInt4          i4X
            , const tCIDLib::TInt4          i4Y
            , const tCIDLib::TCard4         c4CX
            , const tCIDLib::TCard4         c4CY
        );

        tCIDLib::TVoid Set
        (
            const   TPoint&                 pntOrg
            , const TSize&                  szSize
        );

        tCIDLib::TVoid SetOrg
        (
            const   TPoint&                 pntNew
        );

        tCIDLib::TVoid SetOrg
        (
            const   tCIDLib::TInt4          i4X
            , const tCIDLib::TInt4          i4Y
        );

        tCIDLib::TVoid SetSize
        (
            const   tCIDLib::TCard4         c4CX
            , const tCIDLib::TCard4         c4CY
        );

        tCIDLib::TVoid SetSize
        (
            const   TSize&                  szNew
        );

        tCIDLib::TVoid SubFromOrg
        (
            const   TArea&                  areaSub
        );

        tCIDLib::TVoid SubFromOrg
        (
            const   TPoint&                 pntSub
        );

        TSize szArea() const
        {
            return TSize(m_c4CX, m_c4CY);
        }

        tCIDLib::TVoid TakeLarger
        (
            const   TArea&                  areaOther
        );

        tCIDLib::TVoid TakeLarger
        (
            const   TSize&                  szOther
        );

        tCIDLib::TVoid TakeSmaller
        (
            const   TArea&                  areaOther
        );

        tCIDLib::TVoid TakeSmaller
        (
            const   TSize&                  szOther
        );

        tCIDLib::TVoid ToPointArray
        (
                    tCIDLib::THostPoint* const aptTarget
            , const tCIDLib::ERectlTypes    eInclusive
        )   const;

        tCIDLib::TVoid ToCornerPoints
        (
                    tCIDLib::THostPoint&    ptULeft
            ,       tCIDLib::THostPoint&    ptURight
            ,       tCIDLib::THostPoint&    ptLRight
            ,       tCIDLib::THostPoint&    ptLLeft
            , const tCIDLib::ERectlTypes    eInclusive
        )   const;

        tCIDLib::TVoid ToCornerPoints
        (
                    TPoint&                 pntULeft
            ,       TPoint&                 pntURight
            ,       TPoint&                 pntLRight
            ,       TPoint&                 pntLLeft
            , const tCIDLib::ERectlTypes    eInclusive
        )   const;

        tCIDLib::TVoid ToPoints
        (
                    TPoint&                 pntULeft
            ,       TPoint&                 pntLRight
            , const tCIDLib::ERectlTypes    eInclusive
        )   const;

        tCIDLib::TVoid ToPoints
        (
                    tCIDLib::THostPoint&    ptULeft
            ,       tCIDLib::THostPoint&    ptLRight
            , const tCIDLib::ERectlTypes    eInclusive
        )   const;

        tCIDLib::TVoid ToRawArea
        (
                    tCIDLib::TRawArea&      areaTarget
        )   const;

        tCIDLib::TVoid TopJustifyIn
        (
            const   TArea&                  areaToJustifyIn
            , const tCIDLib::TBoolean       bHCenter = kCIDLib::False
        );

        tCIDLib::TVoid ToRectl
        (
                    tCIDLib::THostRectl&    rectlTarget
            , const tCIDLib::ERectlTypes    eInclusive
        )   const;

        tCIDLib::TVoid ToRectl
        (
                    tCIDLib::THostRectl&    rectlTarget
            , const tCIDLib::ERectlTypes    eInclusive
            , const TPoint&                 pntOfs
        )   const;

        tCIDLib::TVoid UpdateSizesIfZero
        (
            const   tCIDLib::TCard4         c4CX
            , const tCIDLib::TCard4         c4CY
        );

        tCIDLib::TVoid ZeroAll();

        tCIDLib::TVoid ZeroOrg();

        tCIDLib::TVoid ZeroSizes();


    protected           :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class    TPoint;


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const override;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        ) override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private             :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DoAdjustments
        (
            const   tCIDLib::TInt4          i4CXOfs
            , const tCIDLib::TInt4          i4CYOfs
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  The init order is important here! Keep the origin members first
        //  then the length members!!
        //
        //  m_i4X, m_i4Y
        //      The origin of the area. It is integral because it could start
        //      behind the origin of the coordinate space it represents.
        //
        //  m_c4CX, m_c4CY
        //      The length and width of the area.
        // -------------------------------------------------------------------
        tCIDLib::TInt4      m_i4X = 0;
        tCIDLib::TInt4      m_i4Y = 0;
        tCIDLib::TCard4     m_c4CX = 0;
        tCIDLib::TCard4     m_c4CY = 0;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TArea,TObject)
        DefPolyDup(TArea)
        NulObject(TArea)
};

#pragma CIDLIB_POPPACK


