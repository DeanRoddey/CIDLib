//
// FILE NAME: CIDMath_4By4Matrix.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/14/1994
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDMath_4By4Matrix.cpp file. This file
//  implements the T4By4Matrix class, which is a 4x4 homogenous coordinate
//  matrix.
//
//  The 4x4 matrix has members in the following configuration:
//
//      f81_1   f81_2   f81_3   0
//      f82_1   f82_2   f82_3   0
//      f83_1   f83_2   f83_3   0
//      f8Xt    f8Yt    f8Zt    1
//
//  Translation elements are on the bottom row. The last column is not
//  accessible to the outside world, and are just here because of the
//  need to work in homogenous coordinates. Using homogenous coordinates
//  means that translations are treated just like rotations, skews, etc...
//  i.e. they can be concatenated into the matrix via a multiply operation.
//
// CAVEATS/GOTCHAS:
//
//  1)  The matrices are intended for a left handled coordinate system since
//      they are for use by 3D and ray tracing software, which uses such
//      systems. So positive rotation angles are clockwise around the axis
//      when looking towards the origin from the positive end.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: T4By4Matrix
// PREFIX: mtrx
// ---------------------------------------------------------------------------
class CIDMATHEXP T4By4Matrix :

    public TObject, public MDuplicable, public MFormattable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        T4By4Matrix();

        T4By4Matrix
        (
            const   tCIDLib::TFloat8&       f81_1
            , const tCIDLib::TFloat8&       f81_2
            , const tCIDLib::TFloat8&       f81_3
            , const tCIDLib::TFloat8&       f82_1
            , const tCIDLib::TFloat8&       f82_2
            , const tCIDLib::TFloat8&       f82_3
            , const tCIDLib::TFloat8&       f83_1
            , const tCIDLib::TFloat8&       f83_2
            , const tCIDLib::TFloat8&       f83_3
            , const tCIDLib::TFloat8&       f8Xt
            , const tCIDLib::TFloat8&       f8Yt
            , const tCIDLib::TFloat8&       f8Zt
        );

        T4By4Matrix
        (
            const   T4By4Matrix&            mtrxToCopy
        );


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        T4By4Matrix& operator=
        (
            const   T4By4Matrix&            mtrxToAssign
        );

        tCIDLib::TFloat8 operator[]
        (
            const   tCIDMath::EMatrixElems  eElem
        )   const;

        tCIDLib::TFloat8& operator[]
        (
            const   tCIDMath::EMatrixElems  eElem
        );

        tCIDLib::TVoid operator*=
        (
            const   T4By4Matrix&            mtrxToMulBy
        );

        tCIDLib::TBoolean operator==
        (
            const   T4By4Matrix&            mtrxToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   T4By4Matrix&            mtrxToTest
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AppendRotation
        (
            const   tCIDLib::TFloat8&       f8XAngle
            , const tCIDLib::TFloat8&       f8YAngle
            , const tCIDLib::TFloat8&       f8ZAngle
        );

        tCIDLib::TVoid AppendRotation
        (
            const   T3DVector&              vecAngles
        );

        tCIDLib::TVoid AppendScale
        (
            const   tCIDLib::TFloat8&       f8XScale
            , const tCIDLib::TFloat8&       f8YScale
            , const tCIDLib::TFloat8&       f8ZScale
        );

        tCIDLib::TVoid AppendScale
        (
            const   T3DVector&              vecScale
        );

        tCIDLib::TVoid AppendTranslation
        (
            const   tCIDLib::TFloat8&       f8XTrans
            , const tCIDLib::TFloat8&       f8YTrans
            , const tCIDLib::TFloat8&       f8ZTrans
        );

        tCIDLib::TVoid AppendTranslation
        (
            const   T3DVector&              vecTrans
        );

        tCIDLib::TVoid SetToIdentity();

        tCIDLib::TVoid SetToRotation
        (
            const   tCIDLib::TFloat8&       f8XDeg
            , const tCIDLib::TFloat8&       f8YDeg
            , const tCIDLib::TFloat8&       f8ZDeg
        );

        tCIDLib::TVoid SetToRotation
        (
            const   T3DVector&              vecAngles
        );

        tCIDLib::TVoid SetToScale
        (
            const   tCIDLib::TFloat8&       f8XScale
            , const tCIDLib::TFloat8&       f8YScale
            , const tCIDLib::TFloat8&       f8ZScale
        );

        tCIDLib::TVoid SetToScale
        (
            const   T3DVector&              vecScale
        );

        tCIDLib::TVoid SetToTranslation
        (
            const   tCIDLib::TFloat8&       f8XTrans
            , const tCIDLib::TFloat8&       f8YTrans
            , const tCIDLib::TFloat8&       f8ZTrans
        );

        tCIDLib::TVoid SetToTranslation
        (
            const   T3DVector&              vecTrans
        );

        tCIDLib::TVoid SetToTransposed
        (
            const   T4By4Matrix&            mtrxSrc
        );

        tCIDLib::TVoid Transform
        (
                    T3DVector&              vecTarget
        )   const;

        tCIDLib::TVoid Transform
        (
            const   T3DVector&              vecSource
            ,       T3DVector&              vecTarget
        )   const;

        tCIDLib::TVoid Transpose();

        tCIDLib::TVoid Zero();


    protected   :
        // -------------------------------------------------------------------
        //  Our binary operators are friends
        // -------------------------------------------------------------------
        friend T4By4Matrix CIDMATHEXP operator*(const T4By4Matrix&, const T4By4Matrix&);


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
        //  Private data members
        //
        //  m_af8Matrix
        //  m_af8Array
        //      These two members are a union that allow the matrix to be
        //      accessed as a two level array and a single level array. When
        //      accessing as a single level, the order is row1, row2, row3,
        //      row4.
        // -------------------------------------------------------------------
        union
        {
            tCIDLib::TFloat8    m_af8Matrix[4][4];
            tCIDLib::TFloat8    m_af8Array[16];
        };


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(T4By4Matrix,TObject)
        DefPolyDup(T4By4Matrix)
};


T4By4Matrix CIDMATHEXP operator*
(
    const   T4By4Matrix&            mtrxLHS
    , const T4By4Matrix&            mtrxRHS
);


#pragma CIDLIB_POPPACK


