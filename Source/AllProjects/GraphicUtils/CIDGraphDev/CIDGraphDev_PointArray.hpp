//
// FILE NAME: CIDGraphDev_PointArray.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/09/1995
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TPointArray class, which is a specialized array
//  type that contains lists of 2 dimensional points. Because of the need to
//  be able to efficiently process poly lines, this class does not store the
//  points internally as TPoint objects, instead storing them as host point
//  structures that can be passed directly into poly line calls.
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
//   CLASS: TPointArray
//  PREFIX: pnta
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TPointArray : public TObject, public MDuplicable
{
    public  :
        // -------------------------------------------------------------------
        //  Class specific types
        //
        //  pfnInitPntAry
        //      This is a function pointer that can be passed in to provide
        //      non-default initialization of the point array. It is called
        //      for each element.
        // -------------------------------------------------------------------
        using pfnInitPntAry = tCIDLib::TVoid (*)
        (
            const   tCIDLib::TCard4             c4Index
            ,       tCIDLib::THostPoint&        ptToFill
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPointArray
        (
            const   tCIDLib::TCard4             c4PointCount
            ,       pfnInitPntAry               pfnInitFunc = nullptr
        );

        TPointArray
        (
            const   TPointArray&                pntaSrc
        );

        ~TPointArray();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPointArray& operator=
        (
            const   TPointArray&                pntaSrc
        );

        const TPoint operator[]
        (
            const   tCIDLib::TCard4             c4Index
        );

        tCIDLib::TBoolean operator==
        (
            const   TPointArray&                pntaToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TPointArray&                pntaToTest
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Count() const;

        tCIDLib::TCard4 c4SetFromGraph
        (
            const   TGraphData&                 grdatSrc
            , const tCIDMath::EGraphScales      eScale
            , const tCIDLib::TCard4             c4Height
            , const tCIDLib::TFloat4            f4MinVal
            , const tCIDLib::TFloat4            f4MaxVal
        );

        tCIDLib::TVoid CopyPoint
        (
            const   tCIDLib::TCard4             c4FromInd
            , const tCIDLib::TCard4             c4ToInd
        );

        tCIDLib::TVoid CopyPointAndOffset
        (
            const   tCIDLib::TCard4             c4FromInd
            , const tCIDLib::TCard4             c4ToInd
            , const tCIDLib::TInt4              i4XOfs
            , const tCIDLib::TInt4              i4YOfs
        );

        tCIDLib::TVoid CopyPointAndOffset
        (
            const   tCIDLib::TCard4             c4FromInd
            , const tCIDLib::TCard4             c4ToInd
            , const TPoint&                     pntOfs
        );

        tCIDLib::TVoid CopyRange
        (
            const   tCIDLib::TCard4             c4FromInd
            , const tCIDLib::TCard4             c4ToInd
            , const tCIDLib::TCard4             c4Count
        );

        tCIDLib::TVoid GetPoint
        (
            const   tCIDLib::TCard4             c4Index
            ,       tCIDLib::TInt4&             i4XToFill
            ,       tCIDLib::TInt4&             i4YToFill
        )   const;

        tCIDLib::TVoid GetPoint
        (
            const   tCIDLib::TCard4             c4Index
            ,       TPoint&                     pntToFill
        )   const;

        tCIDLib::TVoid OffsetAll
        (
            const   tCIDLib::TInt4              i4XOfs
            , const tCIDLib::TInt4              i4YOfs
        );

        tCIDLib::TVoid OffsetAll
        (
            const   TPoint&                     pntOfs
        );

        tCIDLib::TVoid OffsetPoint
        (
            const   tCIDLib::TCard4             c4Index
            , const tCIDLib::TInt4              i4XOfs
            , const tCIDLib::TInt4              i4YOfs
        );

        tCIDLib::TVoid OffsetPoint
        (
            const   tCIDLib::TCard4             c4Index
            , const TPoint&                     pntOfs
        );

        tCIDLib::TVoid OffsetRange
        (
            const   tCIDLib::TCard4             c4FromInd
            , const tCIDLib::TCard4             c4ToInd
            , const tCIDLib::TInt4              i4XOfs
            , const tCIDLib::TInt4              i4YOfs
        );

        tCIDLib::TVoid OffsetRange
        (
            const   tCIDLib::TCard4             c4FromInd
            , const tCIDLib::TCard4             c4ToInd
            , const TPoint&                     pntOfs
        );

        tCIDLib::TVoid Reallocate
        (
            const   tCIDLib::TCard4             c4NewCount
            ,       pfnInitPntAry               pfnInitFunc = 0
        );

        tCIDLib::TVoid SetAll
        (
            const   tCIDLib::TInt4              i4XNew
            , const tCIDLib::TInt4              i4YNew
        );

        tCIDLib::TVoid SetAll
        (
            const   TPoint&                     pntNew
        );

        tCIDLib::TVoid SetFromOffset
        (
            const   TPointArray&                pntaSrc
            , const TPoint&                     pntOffset
        );

        tCIDLib::TVoid SetPoint
        (
            const   tCIDLib::TCard4             c4Index
            , const tCIDLib::TInt4              i4XNew
            , const tCIDLib::TInt4              i4YNew
        );

        tCIDLib::TVoid SetPoint
        (
            const   tCIDLib::TCard4             c4Index
            , const TPoint&                     pntNew
        );

        tCIDLib::TVoid SetRange
        (
            const   tCIDLib::TCard4             c4FromInd
            , const tCIDLib::TCard4             c4ToInd
            , const tCIDLib::TInt4              c4XNew
            , const tCIDLib::TInt4              c4YNew
        );

        tCIDLib::TVoid SetRange
        (
            const   tCIDLib::TCard4             c4FromInd
            , const tCIDLib::TCard4             c4ToInd
            , const TPoint&                     pntNew
        );

        tCIDLib::TVoid ZeroAll();

        tCIDLib::TVoid ZeroPoint
        (
            const   tCIDLib::TCard4             c4Index
        );

        tCIDLib::TVoid ZeroRange
        (
            const   tCIDLib::TCard4             c4FromIndex
            , const tCIDLib::TCard4             c4ToIndex
        );


    protected   :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TGraphDrawDev;


        // -------------------------------------------------------------------
        //  Hidden Constructors and Destructor
        // -------------------------------------------------------------------
        TPointArray();


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        const tCIDLib::THostPoint* aptList() const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckIndex
        (
            const   tCIDLib::TCard4             c4Line
            , const tCIDLib::TCard4             c4Index
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Count
        //      This is the count of points in the array.
        //
        //  m_paptPoints
        //      This is the array of host point structures that are used to
        //      store the points.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4Count;
        tCIDLib::THostPoint*    m_paptPoints;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TPointArray,TObject)
        DefPolyDup(TPointArray)
};

#pragma CIDLIB_POPPACK


