//
// FILE NAME: CIDMath_2DVector.hpp
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
//  This is the header for the CIDMath_2DVector.cpp file. This file
//  implements the T2DVector class.
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
//  CLASS: T2DVector
// PREFIX: vec
// ---------------------------------------------------------------------------
class CIDMATHEXP T2DVector :

    public TObject, public MDuplicable, public MFormattable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        T2DVector();

        T2DVector(const T2DVector&) = default;
        T2DVector(T2DVector&&) = default;

        T2DVector
        (
            const   tCIDLib::TFloat8&       f8XMag
            , const tCIDLib::TFloat8&       f8YMag
        );

        ~T2DVector();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        T2DVector& operator=(const T2DVector&) = default;
        T2DVector& operator=(T2DVector&&) = default;

        tCIDLib::TBoolean operator==
        (
            const   T2DVector&              vecSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   T2DVector&              vecSrc
        )   const;

        tCIDLib::TVoid operator+=
        (
            const   T2DVector&              vecToAdd
        );

        tCIDLib::TVoid operator-=
        (
            const   T2DVector&              vecToSub
        );

        tCIDLib::TVoid operator*=
        (
            const   T2DVector&              vecToMulBy
        );

        tCIDLib::TVoid operator*=
        (
            const   tCIDLib::TFloat8&       f8ToMulBy
        );

        tCIDLib::TVoid operator/=
        (
            const   T2DVector&              vecToDivBy
        );

        tCIDLib::TVoid operator/=
        (
            const   tCIDLib::TFloat8&       f8ToDivBy
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAtOrg() const;

        tCIDLib::TFloat8 f8Magnitude() const;

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

        tCIDLib::TVoid Negate();

        tCIDLib::TVoid Normalize();

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TFloat8&       f8XMag
            , const tCIDLib::TFloat8&       f8YMag
        );

        tCIDLib::TVoid Square();

        tCIDLib::TVoid ToRadians();


    protected   :
        // -------------------------------------------------------------------
        //  Our binary operators and helpers are friends
        // -------------------------------------------------------------------
        friend T2DVector CIDMATHEXP operator+(const T2DVector&, const T2DVector&);
        friend T2DVector CIDMATHEXP operator-(const T2DVector&, const T2DVector&);
        friend T2DVector CIDMATHEXP operator*(const T2DVector&, const T2DVector&);
        friend T2DVector CIDMATHEXP operator/(const T2DVector&, const T2DVector&);
        friend T2DVector CIDMATHEXP operator/(const T2DVector&, const tCIDLib::TFloat8&);
        friend tCIDLib::TFloat8 CIDMATHEXP f8Dot(const T2DVector&, const T2DVector&);
        friend T2DVector CIDMATHEXP vecHalfBetween(const T2DVector&, const T2DVector&);


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
        //      These are the magnitude values in the x and y directions.
        // -------------------------------------------------------------------
        tCIDLib::TFloat8        m_f8XMag;
        tCIDLib::TFloat8        m_f8YMag;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(T2DVector,TObject)
        DefPolyDup(T2DVector);
};


T2DVector CIDMATHEXP operator+(const T2DVector& vecLHS, const T2DVector& );
T2DVector CIDMATHEXP operator-(const T2DVector& vecLHS, const T2DVector&  vecRHS);
T2DVector CIDMATHEXP operator*(const T2DVector& vecLHS, const T2DVector&  vecRHS);
T2DVector CIDMATHEXP operator/(const T2DVector& vecLHS, const T2DVector&  vecRHS);
T2DVector CIDMATHEXP operator/(const T2DVector& vecLHS, const tCIDLib::TFloat8& f8ToDivBy);
tCIDLib::TFloat8 CIDMATHEXP f8Dot(const T2DVector& vecLHS, const T2DVector&  vecRHS);
T2DVector CIDMATHEXP vecHalfBetween(const T2DVector& vecLHS, const T2DVector& vecRHS);

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  Alias the vector to TFPoint so that it can be more self documenting
//  when used as a point in a floating point space.
// ---------------------------------------------------------------------------
using TFPoint = T2DVector;

