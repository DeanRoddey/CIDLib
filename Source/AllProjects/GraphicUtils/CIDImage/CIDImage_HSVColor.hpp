//
// FILE NAME: CIDImage_HSVColor.hpp
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
//  This is the header file for the CIDLib_HSVColor.cpp file, which implements
//  the THSVColor class. This is a small class that presents a color in the
//  Hue-Saturation-Value model. It is never used directly, but we often need
//  to represent them or convert between them and RGB colors.
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
//   CLASS: THSVClr
//  PREFIX: hsv
// ---------------------------------------------------------------------------
class CIDIMGEXP THSVClr :

    public TObject, public MDuplicable, public MFormattable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        THSVClr() = default;

        THSVClr
        (
            const   TRGBClr&                rgbSrc
        );

        THSVClr
        (
            const   tCIDLib::TFloat4        f4Hue
            , const tCIDLib::TFloat4        f4Saturation
            , const tCIDLib::TFloat4        f4Value
        );

        THSVClr(const THSVClr&) = default;
        THSVClr(THSVClr&&) = default;

        ~THSVClr() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        THSVClr& operator=(const THSVClr&) = default;
        THSVClr& operator=(THSVClr&&) = default;

        THSVClr& operator=
        (
            const   TRGBClr&                rgbToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const   THSVClr&                hsvToTest
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   THSVClr&                hsvToTest
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bParseFromText
        (
            const   TString&                strText
            , const tCIDLib::TBoolean       bIntegral
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Auto
            , const tCIDLib::TCh            chSepChar = L','
        );

        tCIDLib::TFloat4 f4Hue() const
        {
            return m_f4Hue;
        }

        tCIDLib::TFloat4 f4Hue
        (
            const   tCIDLib::TFloat4        f4ToSet
        );

        tCIDLib::TFloat4 f4Saturation() const
        {
            return m_f4Saturation;
        }

        tCIDLib::TFloat4 f4Saturation
        (
            const   tCIDLib::TFloat4        f4ToSet
        );

        tCIDLib::TFloat4 f4Value() const
        {
            return m_f4Value;
        }

        tCIDLib::TFloat4 f4Value
        (
            const   tCIDLib::TFloat4        f4ToSet
        );

        tCIDLib::TVoid FormatToText
        (
                    TString&                strToFill
            , const tCIDLib::TCh            chSepChar
            , const tCIDLib::TBoolean       bIntegral
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        )   const;

        tCIDLib::TVoid FromRGB
        (
            const   TRGBClr&                rgbSrc
        );

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TFloat4        f4Hue
            , const tCIDLib::TFloat4        f4Saturation
            , const tCIDLib::TFloat4        f4Value
        );

        tCIDLib::TVoid ToBlack()
        {
            m_f4Hue = 0;
            m_f4Saturation = 0;
            m_f4Value = 0;
        }

        tCIDLib::TVoid ToRGB
        (
                    TRGBClr&                rgbToFill
        )   const;


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
        //  m_f4Hue
        //  m_f4Saturation
        //  m_f4Value
        //      These are the color components
        // -------------------------------------------------------------------
        tCIDLib::TFloat4    m_f4Hue = 0.0F;
        tCIDLib::TFloat4    m_f4Saturation = 0.0F;
        tCIDLib::TFloat4    m_f4Value = 0.0F;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(THSVClr,TObject)
        DefPolyDup(THSVClr)
        NulObject(THSVClr)
};

#pragma CIDLIB_POPPACK

