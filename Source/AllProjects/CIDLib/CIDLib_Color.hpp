//
// FILE NAME: CIDLib_Color.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/05/1993
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
//  This is the header file for the CIDLib_Color.cpp file, which implements
//  the basic TRGBClr class. The TRGBClr class is a pretty simple class that
//  is used for all colors in CIDLib color output to the underlying operating
//  system. It contains a value from 0..255 for the red, green, and blue
//  components of a color value. It has methods for manipulating those
//  components in interesting ways.
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
//   CLASS: TRGBClr
//  PREFIX: rgb
// ---------------------------------------------------------------------------
class CIDLIBEXP TRGBClr :

    public TObject, public MDuplicable, public MFormattable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TRGBClr() = default;

        TRGBClr
        (
            const   tCIDLib::TCard1         c1Red
            , const tCIDLib::TCard1         c1Green
            , const tCIDLib::TCard1         c1Blue
        );

        TRGBClr
        (
            const   tCIDLib::TCard1         c1Red
            , const tCIDLib::TCard1         c1Green
            , const tCIDLib::TCard1         c1Blue
            , const tCIDLib::TCard1         c1Alpha
        );

        TRGBClr
        (
            const   tCIDLib::TRawRGB&       rgbRaw
        );

        TRGBClr
        (
            const   tCIDLib::TCodedRGB&     rgbCoded
        );

        TRGBClr
        (
            const   tCIDLib::TCard4         c4Color
        );

        TRGBClr(const TRGBClr&) = default;
        TRGBClr(TRGBClr&&) = default;

        ~TRGBClr() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TRGBClr& operator=(const TRGBClr&) = default;
        TRGBClr& operator=(TRGBClr&&) = default;

        TRGBClr& operator=
        (
            const   tCIDLib::TCard4         c4Color
        );

        TRGBClr& operator=
        (
            const   tCIDLib::TRawRGB&       rgbRaw
        );

        TRGBClr& operator=
        (
            const   tCIDLib::TCodedRGB&     rgbCoded
        );

        TRGBClr& operator=
        (
            const   tCIDLib::TRawColor&     clrSrc
        );

        TRGBClr& operator=
        (
            const   tCIDLib::TRawAlphaColor& clrSrc
        );

        tCIDLib::TBoolean operator==
        (
            const   TRGBClr&                rgbToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TRGBClr&                rgbToTest
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Adjust
        (
            const   tCIDLib::TInt4          i4RedOffset
            , const tCIDLib::TInt4          i4GreenOffset
            , const tCIDLib::TInt4          i4BlueOffset
        );

        tCIDLib::TVoid AdjustHue
        (
            const   tCIDLib::TFloat4        f4Offset
        );

        tCIDLib::TVoid AdjustSaturation
        (
            const   tCIDLib::TFloat4        f4Offset
        );

        tCIDLib::TVoid AdjustValue
        (
            const   tCIDLib::TFloat4        f4Offset
        );

        tCIDLib::TBoolean bParseFromText
        (
            const   TString&                strText
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
            , const tCIDLib::TCh            chSepChar = L','
        );

        tCIDLib::TCard1 c1AdjustAlpha
        (
            const   tCIDLib::TInt4          i4Offset
        );

        tCIDLib::TCard1 c1AdjustBlue
        (
            const   tCIDLib::TInt4          i4Offset
        );

        tCIDLib::TCard1 c1AdjustGreen
        (
            const   tCIDLib::TInt4          i4Offset
        );

        tCIDLib::TCard1 c1AdjustRed
        (
            const   tCIDLib::TInt4          i4Offset
        );

        tCIDLib::TCard1 c1Alpha() const
        {
            return m_c1Alpha;
        }

        tCIDLib::TCard1 c1Alpha(const tCIDLib::TCard1 c1Alpha)
        {
            m_c1Alpha = c1Alpha;
            return m_c1Alpha;
        }

        tCIDLib::TCard1 c1Blue() const
        {
            return m_c1Blue;
        }

        tCIDLib::TCard1 c1Blue(const tCIDLib::TCard1 c1Blue)
        {
            m_c1Blue = c1Blue;
            return c1Blue;
        }

        tCIDLib::TCard1 c1ClrComp
        (
            const   tCIDLib::EClrComps      eComp
        )   const;

        tCIDLib::TCard1 c1Green() const
        {
            return m_c1Green;
        }

        tCIDLib::TCard1 c1Green(const tCIDLib::TCard1 c1Green)
        {
            m_c1Green = c1Green;
            return c1Green;
        }

        tCIDLib::TCard1 c1MaxComponent() const;

        tCIDLib::TCard1 c1MinComponent() const;

        tCIDLib::TCard1 c1Red() const
        {
            return m_c1Red;
        }

        tCIDLib::TCard1 c1Red(const tCIDLib::TCard1 c1Red)
        {
            m_c1Red = c1Red;
            return c1Red;
        }

        tCIDLib::TCard4 c4Alpha() const
        {
            return m_c1Alpha;
        }

        tCIDLib::TCard4 c4Blue() const
        {
            return m_c1Blue;
        }

        tCIDLib::TCard4 c4Color() const
        {
            return *((const tCIDLib::TCard4*)&m_c1Red);
        }

        tCIDLib::TCard4 c4Green() const
        {
            return m_c1Green;
        }

        tCIDLib::TCard4 c4Magnitude() const;

        tCIDLib::TCard4 c4Red() const
        {
            return m_c1Red;
        }

        tCIDLib::TFloat4 f4Alpha() const;

        tCIDLib::TFloat4 f4Blue() const;

        tCIDLib::TFloat4 f4Green() const;

        tCIDLib::TFloat4 f4Red() const;

        tCIDLib::TVoid FormatToText
        (
                    TString&                strToFill
            , const tCIDLib::TCh            chSepChar
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
            , const tCIDLib::TBoolean       bIncludeAlpha = kCIDLib::False
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        )   const;

        tCIDLib::TVoid FromHSV
        (
            const   tCIDLib::TFloat4        f4Hue
            , const tCIDLib::TFloat4        f4Saturation
            , const tCIDLib::TFloat4        f4Value
        );

        tCIDLib::TVoid Interpolate
        (
            const   TRGBClr&                rgbSecond
            ,       TRGBClr&                rgbToFill
        )   const;

        tCIDLib::TVoid Scale
        (
            const   tCIDLib::TFloat4        f4Percent
        );

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard1         c1Red
            , const tCIDLib::TCard1         c1Green
            , const tCIDLib::TCard1         c1Blue
        );

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard1         c1Red
            , const tCIDLib::TCard1         c1Green
            , const tCIDLib::TCard1         c1Blue
            , const tCIDLib::TCard1         c1Alpha
        );

        tCIDLib::TVoid SetSaturation
        (
            const   tCIDLib::TFloat4        f4ToSet
        );

        tCIDLib::TVoid SetValue
        (
            const   tCIDLib::TFloat4        f4ToSet
        );

        tCIDLib::TVoid ToBlack()
        {
            m_c1Red = 0;
            m_c1Green = 0;
            m_c1Blue = 0;
            m_c1Alpha = 0;
        }

        tCIDLib::TVoid ToHSV
        (
                    tCIDLib::TFloat4&       f4Hue
            ,       tCIDLib::TFloat4&       f4Saturation
            ,       tCIDLib::TFloat4&       f4Value
        )   const;

        tCIDLib::TVoid ToCodedRGB
        (
                    tCIDLib::TCodedRGB&     rgbTarget
        )   const;

        tCIDLib::TVoid ToRawColor
        (
                    tCIDLib::TRawColor&     clrTarget
        )   const;

        tCIDLib::TVoid ToRawAlphaColor
        (
                    tCIDLib::TRawAlphaColor&  clrTarget
        )   const;

        tCIDLib::TVoid ToRGB
        (
                    tCIDLib::TRawRGB&       rgbTarget
        )   const;

        tCIDLib::TVoid WashOut();


    protected   :
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
        )   final;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const final;


    private         :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  NOTE:   These are packed to a single byte so as to fit within a
        //          single 32 bit value. This greatly speeds up and simplifies
        //          some internal processing. It basically gives it the same
        //          layout as the tCIDLib::TRawRGB structure.
        //
        //  m_c1Red
        //  m_c1Green
        //  m_c1Blue
        //      These are the color components, stored in the correct order
        //      to match the system format.
        //
        //  m_c1Alpha
        //      This is the alpha channel. Currently it is not used because
        //      The Win32 graphics model doesn't support it. So it will be
        //      defaulted to 255 (fully opaque.)
        // -------------------------------------------------------------------
        #pragma CIDLIB_PACK(1)
        tCIDLib::TCard1     m_c1Red     = 0;
        tCIDLib::TCard1     m_c1Green   = 0;
        tCIDLib::TCard1     m_c1Blue    = 0;
        tCIDLib::TCard1     m_c1Alpha   = 0;
        #pragma CIDLIB_POPPACK


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TRGBClr,TObject)
        DefPolyDup(TRGBClr)
        NulObject(TRGBClr)
};

#pragma CIDLIB_POPPACK
