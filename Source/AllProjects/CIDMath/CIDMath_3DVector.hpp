//
// FILE NAME: CIDMath_3DVector.hpp
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
//  This is the header for the CIDMath_3DVector.cpp file. This file
//  implements the T3DVector class
//
//  This is just old code that isn't very good. It's not currently used and
//  needs some love.
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
//  CLASS: T3DVector
// PREFIX: vec3
// ---------------------------------------------------------------------------
class CIDMATHEXP T3DVector :

    public TObject, public MStreamable, public MDuplicable, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        T3DVector();

        T3DVector
        (
            const   tCIDLib::TFloat8&       f8XMag
            , const tCIDLib::TFloat8&       f8YMag
            , const tCIDLib::TFloat8&       f8ZMag
        );

        T3DVector(const T3DVector&) = default;

        ~T3DVector();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        T3DVector& operator=(const T3DVector&) = default;

        tCIDLib::TBoolean operator==
        (
            const   T3DVector&              vecToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   T3DVector&              vecToCompare
        )   const;

        tCIDLib::TVoid operator+=
        (
            const   T3DVector&              vecToAdd
        );

        tCIDLib::TVoid operator+=
        (
            const   tCIDLib::TFloat8&       f8ToAdd
        );


        tCIDLib::TVoid operator-=
        (
            const   T3DVector&              vecToSub
        );

        tCIDLib::TVoid operator-=
        (
            const   tCIDLib::TFloat8&       f8ToSub
        );

        tCIDLib::TVoid operator*=
        (
            const   T3DVector&              vecToMulBy
        );

        tCIDLib::TVoid operator*=
        (
            const   tCIDLib::TFloat8&       f8ToMulBy
        );

        tCIDLib::TVoid operator/=
        (
            const   T3DVector&              vecToDivBy
        );

        tCIDLib::TVoid operator/=
        (
            const   tCIDLib::TFloat8&       f8ToDivBy
        );



        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Abs();

        tCIDLib::TVoid AddScaled
        (
            const   T3DVector&              vecSrc
            , const tCIDLib::TFloat8&       f8Scale
        );

        tCIDLib::TVoid Adjust
        (
            const   tCIDLib::TFloat8&       f8X
            , const tCIDLib::TFloat8&       f8Y
            , const tCIDLib::TFloat8&       f8Z
        );

        tCIDLib::TBoolean bAtOrg() const;

        tCIDLib::TBoolean bWithinLimits
        (
            const   tCIDLib::TFloat8&       f8Limit
        )   const;

        tCIDLib::TFloat8 f8Dot
        (
            const   T3DVector&               vec2
        )   const;

        tCIDLib::TFloat8 f8X() const;

        tCIDLib::TFloat8 f8X
        (
            const   tCIDLib::TFloat8&       f8New
        );

        tCIDLib::TFloat8 f8Y() const;

        tCIDLib::TFloat8 f8Y
        (
            const   tCIDLib::TFloat8&       f8New
        );

        tCIDLib::TFloat8 f8Z() const;

        tCIDLib::TFloat8 f8Z
        (
            const   tCIDLib::TFloat8&       f8New
        );

        tCIDLib::TFloat8 f8Magnitude() const;

        tCIDLib::TFloat8 f8MagSum() const;

        tCIDLib::TVoid ClipToDegrees();

        tCIDLib::TVoid Cross
        (
            const   T3DVector&              vec2
        );

        tCIDLib::TVoid HalfBetween
        (
            const   T3DVector&              vec2
        );

        tCIDLib::TVoid Mix
        (
            const   T3DVector&              vec2
        );

        tCIDLib::TVoid Negate();

        tCIDLib::TVoid Normalize();

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TFloat8&       f8XMag
            , const tCIDLib::TFloat8&       f8YMag
            , const tCIDLib::TFloat8&       f8ZMag
        );

        tCIDLib::TVoid Square();

        tCIDLib::TVoid SubScaled
        (
            const   T3DVector&              vecSrc
            , const tCIDLib::TFloat8&       f8Scale
        );

        tCIDLib::TVoid ToRadians();



    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class    T4By4Matrix;
        friend T3DVector CIDMATHEXP operator+(const T3DVector&, const T3DVector&);
        friend T3DVector CIDMATHEXP operator+(const T3DVector&, const tCIDLib::TFloat8&);
        friend T3DVector CIDMATHEXP operator-(const T3DVector&, const T3DVector&);
        friend T3DVector CIDMATHEXP operator-(const T3DVector&, const tCIDLib::TFloat8&);
        friend T3DVector CIDMATHEXP operator*(const T3DVector&, const T3DVector&);
        friend T3DVector CIDMATHEXP operator*(const T3DVector&, const tCIDLib::TFloat8&);
        friend T3DVector CIDMATHEXP operator/(const T3DVector&, const T3DVector&);
        friend T3DVector CIDMATHEXP operator/(const T3DVector&, const tCIDLib::TFloat8&);
        friend T3DVector CIDMATHEXP vecCross(const T3DVector&, const T3DVector&);
        friend T3DVector CIDMATHEXP vecHalfBetween(const T3DVector&, const T3DVector&);
        friend T3DVector CIDMATHEXP vecMix(const T3DVector&, const T3DVector&);



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
        //  m_f8XMag
        //  m_f8YMag
        //  m_f8ZMag
        //      These are the magnitude values in the x,y, and z directions.
        // -------------------------------------------------------------------
        tCIDLib::TFloat8        m_f8XMag;
        tCIDLib::TFloat8        m_f8YMag;
        tCIDLib::TFloat8        m_f8ZMag;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(T3DVector,TObject)
        DefPolyDup(T3DVector);
};


T3DVector CIDMATHEXP operator+
(
    const   T3DVector&              vecLHS
    , const T3DVector&              vecRHS
);

T3DVector CIDMATHEXP operator+
(
    const   T3DVector&              vecLHS
    , const tCIDLib::TFloat8&       f8ToAdd
);

T3DVector CIDMATHEXP operator-
(
    const   T3DVector&              vecLHS
    , const T3DVector&              vecRHS
);

T3DVector CIDMATHEXP operator-
(
    const   T3DVector&              vecLHS
    , const tCIDLib::TFloat8&       f8ToSub
);        T3DVector CIDMATHEXP operator*
(
    const   T3DVector&              vecLHS
    , const T3DVector&              vecRHS
);

T3DVector CIDMATHEXP operator*
(
    const   T3DVector&              vecLHS
    , const tCIDLib::TFloat8&       f8ToMulBy
);
T3DVector CIDMATHEXP operator/
(
    const   T3DVector&              vecLHS
    , const T3DVector&              vecRHS
);

T3DVector CIDMATHEXP operator/
(
    const   T3DVector&              vecLHS
    , const tCIDLib::TFloat8&       f8ToDivBy
);


T3DVector CIDMATHEXP vecCross
(
    const   T3DVector&              vecLHS
    , const T3DVector&              vecRHS
);

T3DVector CIDMATHEXP vecHalfBetween
(
    const   T3DVector&              vecLHS
    , const T3DVector&              vecRHS
);

T3DVector CIDMATHEXP vecMix
(
    const   T3DVector&              vecLHS
    , const T3DVector&              vecRHS
);

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  Typedef the vector to T3DPoint so that it can be more self documenting
//  when used as a point in a 3D floating point space.
// ---------------------------------------------------------------------------
typedef T3DVector   T3DPoint;

