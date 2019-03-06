//
// FILE NAME: CIDMath_FArea.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/10/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDMath_FArea.cpp file, which implements
//  the TFArea class. This class represents a rectangular area  in a
//  floating point coordinate space. The internal representation is as
//  a left, right, top, and bottom value.
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
//  CLASS: TFArea
// PREFIX: farea
// ---------------------------------------------------------------------------
class CIDMATHEXP TFArea :

    public TObject, public MDuplicable, public MFormattable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TFArea();

        TFArea
        (
            const   TArea&                  areaFrom
            , const tCIDLib::TFloat8&       f8ScaleBy = 1.0
        );

        TFArea
        (
            const   tCIDLib::TFloat8&       f8Left
            , const tCIDLib::TFloat8&       f8Right
            , const tCIDLib::TFloat8&       f8Bottom
            , const tCIDLib::TFloat8&       f8Top
        );

        TFArea
        (
            const   T2DVector&              vecLL
            , const T2DVector&              vecUR
        );

        TFArea(const TFArea&) = default;

        ~TFArea();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFArea& operator=(const TFArea&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TFArea&                 fareaToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TFArea&                 fareaToCompare
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TFloat8 f8Area() const;

        tCIDLib::TFloat8 f8Bottom() const;

        tCIDLib::TFloat8 f8Bottom
        (
            const   tCIDLib::TFloat8&       f8New
        );

        tCIDLib::TFloat8 f8CX() const;

        tCIDLib::TFloat8 f8CXDiv
        (
            const   tCIDLib::TCard4         c4DivBy
        )   const;

        tCIDLib::TFloat8 f8CY() const;

        tCIDLib::TFloat8 f8CYDiv
        (
            const   tCIDLib::TCard4         c4DivBy
        )   const;

        tCIDLib::TFloat8 f8Left() const;

        tCIDLib::TFloat8 f8Left
        (
            const   tCIDLib::TFloat8&       f8New
        );

        tCIDLib::TFloat8 f8Right() const;

        tCIDLib::TFloat8 f8Right
        (
            const   tCIDLib::TFloat8&       f8New
        );

        tCIDLib::TFloat8 f8Top() const;

        tCIDLib::TFloat8 f8Top
        (
            const   tCIDLib::TFloat8&       f8New
        );

        tCIDLib::TVoid SetAll
        (
            const   tCIDLib::TFloat8&       f8Left
            , const tCIDLib::TFloat8&       f8Right
            , const tCIDLib::TFloat8&       f8Bottom
            , const tCIDLib::TFloat8&       f8Top
        );

        tCIDLib::TVoid SetOrg
        (
            const   T2DVector&              vecToSet
        );

        tCIDLib::TVoid SetLowerRight
        (
            const   T2DVector&              vecToSet
        );

        T2DVector vecLL() const;

        T2DVector vecLR() const;

        T2DVector vecUL() const;

        T2DVector vecUR() const;


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmToWriteTo
        ) const;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToWriteTo
        );

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdjustCoordinates();


        // -------------------------------------------------------------------
        // Private data members
        //
        //  m_f8Left
        //  m_f8Right
        //  m_f8Bottom
        //  m_f8Top
        //      These are the 8 coordinates the define the rectangular area.
        // -------------------------------------------------------------------
        tCIDLib::TFloat8        m_f8Left;
        tCIDLib::TFloat8        m_f8Right;
        tCIDLib::TFloat8        m_f8Bottom;
        tCIDLib::TFloat8        m_f8Top;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFArea,TObject)
        DefPolyDup(TFArea)
};

#pragma CIDLIB_POPPACK


