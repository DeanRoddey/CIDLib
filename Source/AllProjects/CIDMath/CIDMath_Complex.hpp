//
// FILE NAME: CIDMath_Complex.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/01/1999
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
//  This is the header for the CIDMath_Complex.cpp file, which implements
//  the TComplex class. This class represents a standard complex floating
//  point number, for doing math in the complex number plane.
//
//  This is just old code that probably isn't very good. It's not currently
//  used and needs some love.
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
//  CLASS: TComplex
// PREFIX: cplx
// ---------------------------------------------------------------------------
class CIDMATHEXP TComplex :

    public TObject, public MStreamable, public MDuplicable, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TComplex
        (
            const   tCIDLib::TFloat8        f8Real
            , const tCIDLib::TFloat8        f8Imag
        );

        TComplex
        (
            const   tCIDLib::TFloat8        f8Real
        );

        TComplex(const TComplex&) = default;
        TComplex(TComplex&&) = default;

        ~TComplex();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TComplex& operator=(const TComplex&) = default;
        TComplex& operator=(TComplex&&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TComplex&               cplxToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TComplex&               cplxToCompare
        )   const;

        tCIDLib::TVoid operator+=
        (
            const   TComplex&               cplxToAdd
        );

        tCIDLib::TVoid operator-=
        (
            const   TComplex&               cplxToSub
        );

        tCIDLib::TVoid operator*=
        (
            const   TComplex&               cplxMulBy
        );

        tCIDLib::TVoid operator*=
        (
            const   tCIDLib::TFloat8        f8MulBy
        );

        tCIDLib::TVoid operator/=
        (
            const   TComplex&               cplxDivBy
        );

        tCIDLib::TVoid operator/=
        (
            const   tCIDLib::TFloat8        f8DivBy
        );

        friend TComplex CIDMATHEXP operator-(const TComplex&, const TComplex&);
        friend TComplex CIDMATHEXP operator+(const TComplex&, const TComplex&);
        friend TComplex CIDMATHEXP operator*(const TComplex&, const TComplex&);
        friend TComplex CIDMATHEXP operator/(const TComplex&, const TComplex&);


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TComplex cplxConjugate() const;

        tCIDLib::TFloat8 f8Imag() const;

        tCIDLib::TFloat8 f8Modulus() const;

        tCIDLib::TFloat8 f8Real() const;

        tCIDLib::TVoid SetToConjugate();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherted methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmToWriteTo
        )   const final;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   final;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const final;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_f8Real
        //  m_f8Imag
        //      The real and imaginary parts of the complex number.
        // -------------------------------------------------------------------
        tCIDLib::TFloat8    m_f8Real;
        tCIDLib::TFloat8    m_f8Imag;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TComplex,TObject)
        DefPolyDup(TComplex)
};


TComplex CIDMATHEXP operator-(const TComplex& cplxLHS, const TComplex& cplxRHS);
TComplex CIDMATHEXP operator+(const TComplex& cplxLHS, const TComplex& cplxRHS);
TComplex CIDMATHEXP operator*(const TComplex& cplxLHS, const TComplex& cplxRHS);
TComplex CIDMATHEXP operator/(const TComplex& cplxLHS, const TComplex&  cplxRHS);

#pragma CIDLIB_POPPACK
