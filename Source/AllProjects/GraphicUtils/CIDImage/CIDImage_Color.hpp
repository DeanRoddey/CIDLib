//
// FILE NAME: CIDImage_Color.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/05/1993
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDImage_Color.cpp file, which
//  implements the TFRGBClr class. TFRGBClr is a floating point version of
//  TRGBClr, which is used in ray tracing and many other applications. It
//  contains a value from 0.0 to 1.0 for each color component. It is always
//  translated to a regular TRGBClr value for output. It has a lot of methods
//  for manipulating it in various ways. It also supports an opacity value
//  that controls its transparency.
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
//  CLASS: TFRGBClr
// PREFIX: frgb
// ---------------------------------------------------------------------------
class CIDIMGEXP TFRGBClr :

    public TObject, public MDuplicable, public MStreamable, public MFormattable
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TFRGBClr();

        TFRGBClr
        (
            const   tCIDLib::TFloat8&       f8Red
            , const tCIDLib::TFloat8&       f8Green
            , const tCIDLib::TFloat8&       f8Blue
            , const tCIDLib::TFloat8&       f8Alpha = 0.0
        );

        TFRGBClr
        (
            const   TFRGBClr&               frgbToCopy
        );

        ~TFRGBClr();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFRGBClr& operator=
        (
            const   TFRGBClr&               frgbToAssign
        );

        friend TFRGBClr CIDIMGEXP operator+
        (
            const   TFRGBClr&               frgb1
            , const TFRGBClr&               frgb2
        );

        friend TFRGBClr CIDIMGEXP operator+
        (
            const   TFRGBClr&               frgbSrc
            , const tCIDLib::TFloat8&       f8Inc
        );

        tCIDLib::TVoid operator+=
        (
            const   TFRGBClr&               frgbSrc
        );

        tCIDLib::TVoid operator+=
        (
            const   tCIDLib::TFloat8&       f8Inc
        );

        friend TFRGBClr CIDIMGEXP operator-
        (
            const   TFRGBClr&               frgb1
            , const TFRGBClr&               frgb2
        );

        friend TFRGBClr CIDIMGEXP operator-
        (
            const   TFRGBClr&               frgbSrc
            , const tCIDLib::TFloat8&       f8Dec
        );

        tCIDLib::TVoid operator-=
        (
            const   TFRGBClr&               frgbSrc
        );

        tCIDLib::TVoid operator-=
        (
            const   tCIDLib::TFloat8&       f8Dec
        );

        friend TFRGBClr CIDIMGEXP operator*
        (
            const   TFRGBClr&               frgb1
            , const TFRGBClr&               frgb2
        );

        friend TFRGBClr CIDIMGEXP operator*
        (
            const   TFRGBClr&               frgb1
            , const tCIDLib::TFloat8&       f8Scale
        );

        tCIDLib::TVoid operator*=
        (
            const   TFRGBClr&               frgbSrc
        );

        tCIDLib::TVoid operator*=
        (
            const   tCIDLib::TFloat8&       f8Scale
        );

        friend TFRGBClr CIDIMGEXP operator/
        (
            const   TFRGBClr&               frgb1
            , const TFRGBClr&               frgb2
        );

        friend TFRGBClr CIDIMGEXP operator/
        (
            const   TFRGBClr&               frgb1
            , const tCIDLib::TFloat8&       f8Divisor
        );

        tCIDLib::TVoid operator/=
        (
            const   TFRGBClr&               frgbSrc
        );

        tCIDLib::TVoid operator/=
        (
            const   tCIDLib::TFloat8&       f8Divisor
        );

        tCIDLib::TBoolean operator==
        (
            const   TFRGBClr&               frgbToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TFRGBClr&               frgbToTest
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddScaled
        (
            const   TFRGBClr&               frgbBase
            , const tCIDLib::TFloat8&       f8Scale
        );

        tCIDLib::TVoid Adjust
        (
            const   tCIDLib::TFloat8&       f8Red
            , const tCIDLib::TFloat8&       f8Green
            , const tCIDLib::TFloat8&       f8Blue
            , const tCIDLib::TFloat8&       f8Alpha = 0.0
        );

        tCIDLib::TVoid AlphaBlend
        (
            const   TFRGBClr&               frgbSrc
            , const tCIDLib::TFloat8&       f8Alpha
        );

        tCIDLib::TBoolean bParseFromString
        (
            const   TString&                strSrc
        );

        tCIDLib::TVoid Clip();

        tCIDLib::TVoid ConvertToRGB
        (
                    TRGBClr&                rgbToFill
        )   const;

        tCIDLib::TFloat8 f8Alpha() const;

        tCIDLib::TFloat8 f8Alpha
        (
            const   tCIDLib::TFloat8&       f8New
        );

        tCIDLib::TFloat8 f8Blue() const;

        tCIDLib::TFloat8 f8Blue
        (
            const   tCIDLib::TFloat8&       f8New
        );

        friend tCIDLib::TFloat8 CIDIMGEXP f8Distance
        (
            const   TFRGBClr&               frgb1
            , const TFRGBClr&               frgb2
        );

        tCIDLib::TFloat8 f8Distance
        (
            const   TFRGBClr&               frgb2
        );

        tCIDLib::TFloat8 f8Green() const;

        tCIDLib::TFloat8 f8Green
        (
            const   tCIDLib::TFloat8&       f8New
        );

        tCIDLib::TFloat8 f8Magnitude() const;

        tCIDLib::TFloat8 f8Red() const;

        tCIDLib::TFloat8 f8Red
        (
            const   tCIDLib::TFloat8&       f8New
        );

        tCIDLib::TVoid FadeToBlack();

        tCIDLib::TVoid Interpolate
        (
            const   tCIDLib::TFloat8&       f8Frac
            , const TFRGBClr&               frgbStartClr
            , const TFRGBClr&               frgbEndClr
        );

        tCIDLib::TVoid Normalize();

        tCIDLib::TVoid SetToScaled
        (
            const   TFRGBClr&               frgbBase
            , const tCIDLib::TFloat8&       f8Scale
        );

        tCIDLib::TVoid SrcAlphaBlend
        (
            const   TFRGBClr&               frgbSrc
        );

        tCIDLib::TVoid TarAlphaBlend
        (
            const   TFRGBClr&               frgbSrc
        );


    protected   :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmToWriteTo
        )   const;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        );

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const;


    private             :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_f8Red
        //  m_f8Green
        //  m_f8Blue
        //      The red,green, and blue values as percentage values from 0.0
        //      to 1.0.
        //
        //  m_f8Alpha
        //      The alpha component is a measure of transparency, which 0.0
        //      be totally opaque and 1.0 being totally transparent.
        // -------------------------------------------------------------------
        tCIDLib::TFloat8        m_f8Red;
        tCIDLib::TFloat8        m_f8Green;
        tCIDLib::TFloat8        m_f8Blue;
        tCIDLib::TFloat8        m_f8Alpha;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFRGBClr,TObject)
        DefPolyDup(TFRGBClr)
        NulObject(TFRGBClr)
};

#pragma CIDLIB_POPPACK


