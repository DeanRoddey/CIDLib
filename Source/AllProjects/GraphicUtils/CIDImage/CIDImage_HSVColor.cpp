//
// FILE NAME: CIDImage_HSVColor.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/28/2002
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
//  This file implements the THSVClr class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  CIDLib includes
// ---------------------------------------------------------------------------
#include    "CIDImage_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(THSVClr,TObject)



// ---------------------------------------------------------------------------
//   CLASS: THSVClr
//  PREFIX: hsv
//
//  This class is used for all color graphic output.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  THSVClr: Public, static methods
// ---------------------------------------------------------------------------
THSVClr& THSVClr::Nul_THSVClr() noexcept
{
    static THSVClr hsvNull;
    return hsvNull;
}


// ---------------------------------------------------------------------------
//  THSVClr: Constructors and Destructor
// ---------------------------------------------------------------------------
THSVClr::THSVClr(const TRGBClr& rgbSrc)
{
    FromRGB(rgbSrc);
}

THSVClr::THSVClr(const  tCIDLib::TFloat4    f4HueInit
                , const tCIDLib::TFloat4    f4SatInit
                , const tCIDLib::TFloat4    f4ValueInit)
{
    m_f4Hue = TMathLib::f4Mod(f4HueInit, 360);
    f4Saturation(f4SatInit);
    f4Value(f4ValueInit);
}


// ---------------------------------------------------------------------------
//  THSVClr: Public operators
// ---------------------------------------------------------------------------
THSVClr& THSVClr::operator=(const TRGBClr& rgbSrc)
{
    FromRGB(rgbSrc);
    return *this;
}

tCIDLib::TBoolean THSVClr::operator==(const THSVClr& hsvToTest) const
{
    if (this != &hsvToTest)
    {
        if ((hsvToTest.m_f4Hue != m_f4Hue)
        ||  (hsvToTest.m_f4Saturation != m_f4Saturation)
        ||  (hsvToTest.m_f4Value != m_f4Value))
        {
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean THSVClr::operator!=(const THSVClr& hsvToTest) const
{
    return !operator==(hsvToTest);
}


// ---------------------------------------------------------------------------
//  THSVClr: Public, non-virtual methods
// ---------------------------------------------------------------------------
//
//  Parses an color of out a string. It expects the format to be in the form
//  "hue sat val", so three values, separated by the indicated by the caller.
//
//  It can either be integral or floating point format. Hue is 0 to 360 in either
//  format. For floating point sat/val are 0 to 1.0. FOr integral format they are
//  0 to 255.
//
tCIDLib::TBoolean
THSVClr::bParseFromText(const   TString&            strText
                        , const tCIDLib::TBoolean   bIntegral
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chSepChar)
{
    // We need to pull the separate tokens out from the string
    TString strA = strText;
    TString strB;
    TString strC;

    // If not three parameters, it's bad
    if (!strA.bSplit(strB, chSepChar)
    ||  !strB.bSplit(strC, chSepChar))
    {
        return kCIDLib::False;
    }

    // Strip whitespace on all of them
    strA.StripWhitespace();
    strB.StripWhitespace();
    strC.StripWhitespace();

    tCIDLib::TFloat4 f4Hue;
    tCIDLib::TFloat4 f4Sat;
    tCIDLib::TFloat4 f4Value;
    if (bIntegral)
    {
        tCIDLib::TCard4 c4Hue;
        tCIDLib::TCard4 c4Sat;
        tCIDLib::TCard4 c4Val;

        // Convert and check the values
        if (!strA.bToCard4(c4Hue, eRadix) || (c4Hue > 360))
            return kCIDLib::False;
        if (!strB.bToCard4(c4Sat, eRadix) || (c4Sat > 255))
            return kCIDLib::False;
        if (!strC.bToCard4(c4Val, eRadix) || (c4Val > 255))
            return kCIDLib::False;

        f4Hue = tCIDLib::TFloat4(c4Hue);
        f4Sat = tCIDLib::TFloat4(c4Sat);
        f4Sat /= 255.0;
        f4Value = tCIDLib::TFloat4(c4Val);
        f4Value /= 255.0;

    }
     else
    {
        // Convert the floating point values
        if (!strA.bToFloat4(f4Hue) || !strB.bToFloat4(f4Sat) || !strC.bToFloat4(f4Value))
            return kCIDLib::False;
    }

    // Clip the values
    if (f4Hue > 360.0)
        f4Hue = 360.0;
    if (f4Sat > 1.0)
        f4Sat = 1.0;
    if (f4Value > 1.0)
        f4Value = 1.0;

    // And now we can store the values
    m_f4Hue = f4Hue;
    m_f4Saturation = f4Sat;
    m_f4Value = f4Value;

    return kCIDLib::True;
}


tCIDLib::TFloat4 THSVClr::f4Hue(const tCIDLib::TFloat4 f4ToSet)
{
    // Mod it to 360 degrees before we store
    m_f4Hue = TMathLib::f4Mod(f4ToSet, 360);
    return m_f4Hue;
}


tCIDLib::TFloat4 THSVClr::f4Saturation(const tCIDLib::TFloat4 f4ToSet)
{
    // Must be from 0 to 1
    if ((f4ToSet < 0) || (f4ToSet > 1.0))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcClr_BadHSVSaturation
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TFloat(f4ToSet)
        );
    }
    m_f4Saturation = f4ToSet;
    return m_f4Saturation;
}


tCIDLib::TFloat4 THSVClr::f4Value(const tCIDLib::TFloat4 f4ToSet)
{
    // Must be from 0 to 1
    if ((f4ToSet < 0) || (f4ToSet > 1.0))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcClr_BadHSVValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TFloat(f4ToSet)
        );
    }
    m_f4Value = f4ToSet;
    return m_f4Value;
}


//
//  Format us out to text as delimited numbers. The caller indicates the
//  delimiter to use. We can formatted it in float or integral format. The float
//  format is:
//
//      Hue : 0.0 to 360
//      Sat : 0.0 to 1.0
//      Val : 0.0 to 1.0
//
//  In integral format it is:
//
//      Hue : 0 to 360
//      Sat : 0 to 255
//      Val : 0 to 255
//
//      The values will be actually in the radix requested in this case.
//
tCIDLib::TVoid
THSVClr::FormatToText(          TString&            strToFill
                        , const tCIDLib::TCh        chSepChar
                        , const tCIDLib::TBoolean   bIntegral
                        , const tCIDLib::TBoolean   bAppend
                        , const tCIDLib::ERadices   eRadix) const
{
    if (!bAppend)
        strToFill.Clear();

    if (bIntegral)
    {
        strToFill.AppendFormatted(tCIDLib::TCard4(m_f4Hue), eRadix);
        strToFill.Append(chSepChar);
        strToFill.AppendFormatted(tCIDLib::TCard4(m_f4Saturation * 255), eRadix);
        strToFill.Append(chSepChar);
        strToFill.AppendFormatted(tCIDLib::TCard4(m_f4Value * 255), eRadix);
    }
     else
    {
        strToFill.AppendFormatted(m_f4Hue, 1);
        strToFill.Append(chSepChar);
        strToFill.AppendFormatted(m_f4Saturation, 3);
        strToFill.Append(chSepChar);
        strToFill.AppendFormatted(m_f4Value, 3);
    }
}


// Convert the passed RGB value to HSV and set our values from that
tCIDLib::TVoid THSVClr::FromRGB(const TRGBClr& rgbSrc)
{
    //
    //  He has a method for doing the conversion so ask him to fill our
    //  members with an HSV representation of himself.
    //
    rgbSrc.ToHSV(m_f4Hue, m_f4Saturation, m_f4Value);
}


tCIDLib::TVoid THSVClr::Set(const   tCIDLib::TFloat4    f4HueToSet
                            , const tCIDLib::TFloat4    f4SatToSet
                            , const tCIDLib::TFloat4    f4ValueToSet)
{
    m_f4Hue = TMathLib::f4Mod(f4HueToSet, 360);
    f4Saturation(f4SatToSet);
    f4Value(f4ValueToSet);
}


tCIDLib::TVoid THSVClr::ToRGB(TRGBClr& rgbToFill) const
{
    // He has a conversion method, so let him do the work
    rgbToFill.FromHSV(m_f4Hue, m_f4Saturation, m_f4Value);
}


// ---------------------------------------------------------------------------
//  THSVClr: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid THSVClr::FormatTo(TTextOutStream& strmToWriteTo) const
{
    strmToWriteTo << m_f4Hue << L"," << m_f4Saturation << L"," << m_f4Value;
}


tCIDLib::TVoid THSVClr::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom  >> m_f4Hue
                    >> m_f4Saturation
                    >> m_f4Value;
}

tCIDLib::TVoid THSVClr::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << m_f4Hue
                    << m_f4Saturation
                    << m_f4Value;
}

