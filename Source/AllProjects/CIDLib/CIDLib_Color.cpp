//
// FILE NAME: CIDLib_Color.cpp
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
//  This file implements the TRGBClr class.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TRGBClr,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TRGBClr
//  PREFIX: rgb
//
//  This class is used for all color graphic output.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TRGBClr: Public, static methods
// ---------------------------------------------------------------------------
TRGBClr& TRGBClr::Nul_TRGBClr()
{
    static TRGBClr rgbNull;
    return rgbNull;
}


// ---------------------------------------------------------------------------
//  TRGBClr: Constructors and operators
// ---------------------------------------------------------------------------
TRGBClr::TRGBClr(const tCIDLib::TCard4 c4Color) :

    m_c1Red(((const tCIDLib::TCodedRGB*)&c4Color)->c1Red)
    , m_c1Green(((const tCIDLib::TCodedRGB*)&c4Color)->c1Green)
    , m_c1Blue(((const tCIDLib::TCodedRGB*)&c4Color)->c1Blue)
    , m_c1Alpha(((const tCIDLib::TCodedRGB*)&c4Color)->c1Alpha)
{
}

TRGBClr::TRGBClr(const tCIDLib::TRawRGB& rgbRaw) :

    m_c1Blue(rgbRaw.c1Blue)
    , m_c1Green(rgbRaw.c1Green)
    , m_c1Red(rgbRaw.c1Red)
{
}

TRGBClr::TRGBClr(const tCIDLib::TCodedRGB& rgbCoded) :

    m_c1Blue(rgbCoded.c1Blue)
    , m_c1Green(rgbCoded.c1Green)
    , m_c1Red(rgbCoded.c1Red)
    , m_c1Alpha(rgbCoded.c1Alpha)
{
}

TRGBClr::TRGBClr(const  tCIDLib::TCard1 c1Red
                , const tCIDLib::TCard1 c1Green
                , const tCIDLib::TCard1 c1Blue) :

    m_c1Blue(c1Blue)
    , m_c1Green(c1Green)
    , m_c1Red(c1Red)
{
}

TRGBClr::TRGBClr(const  tCIDLib::TCard1 c1Red
                , const tCIDLib::TCard1 c1Green
                , const tCIDLib::TCard1 c1Blue
                , const tCIDLib::TCard1 c1Alpha) :

    m_c1Blue(c1Blue)
    , m_c1Green(c1Green)
    , m_c1Red(c1Red)
    , m_c1Alpha(c1Alpha)
{
}


// ---------------------------------------------------------------------------
//  TRGBClr: Public operators
// ---------------------------------------------------------------------------
TRGBClr& TRGBClr::operator=(const tCIDLib::TCard4 c4Color)
{
    m_c1Red     = ((const tCIDLib::TCodedRGB*)&c4Color)->c1Red;
    m_c1Green   = ((const tCIDLib::TCodedRGB*)&c4Color)->c1Green;
    m_c1Blue    = ((const tCIDLib::TCodedRGB*)&c4Color)->c1Blue;
    m_c1Alpha   = ((const tCIDLib::TCodedRGB*)&c4Color)->c1Alpha;

    return *this;
}

TRGBClr& TRGBClr::operator=(const tCIDLib::TRawRGB& rgbRaw)
{
    m_c1Red     = rgbRaw.c1Red;
    m_c1Green   = rgbRaw.c1Green;
    m_c1Blue    = rgbRaw.c1Blue;
    m_c1Alpha   = 0;

    return *this;
}

TRGBClr& TRGBClr::operator=(const tCIDLib::TRawColor& clrSrc)
{
    m_c1Red     = clrSrc.c1Red;
    m_c1Green   = clrSrc.c1Green;
    m_c1Blue    = clrSrc.c1Blue;
    m_c1Alpha   = 0;

    return *this;
}

TRGBClr& TRGBClr::operator=(const tCIDLib::TRawAlphaColor& clrSrc)
{
    m_c1Red     = clrSrc.c1Red;
    m_c1Green   = clrSrc.c1Green;
    m_c1Blue    = clrSrc.c1Blue;
    m_c1Alpha   = clrSrc.c1Alpha;

    return *this;
}

TRGBClr& TRGBClr::operator=(const tCIDLib::TCodedRGB& rgbCoded)
{
    m_c1Red     = rgbCoded.c1Red;
    m_c1Green   = rgbCoded.c1Green;
    m_c1Blue    = rgbCoded.c1Blue;
    m_c1Alpha   = rgbCoded.c1Alpha;

    return *this;
}

tCIDLib::TBoolean TRGBClr::operator==(const TRGBClr& rgbToTest) const
{
    if (this == &rgbToTest)
        return kCIDLib::True;

    return (rgbToTest.m_c1Red == m_c1Red)
            &&  (rgbToTest.m_c1Green == m_c1Green)
            &&  (rgbToTest.m_c1Blue == m_c1Blue)
            &&  (rgbToTest.m_c1Alpha == m_c1Alpha);
}

tCIDLib::TBoolean TRGBClr::operator!=(const TRGBClr& rgbToTest) const
{
    return !operator==(rgbToTest);
}


// ---------------------------------------------------------------------------
//  TRGBClr: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TRGBClr::Adjust( const   tCIDLib::TInt4  i4RedOffset
                                , const tCIDLib::TInt4  i4GreenOffset
                                , const tCIDLib::TInt4  i4BlueOffset)
{
    tCIDLib::TInt4   i4Cur;

    i4Cur = m_c1Red;
    i4Cur += i4RedOffset;
    if (i4Cur < 0)
        m_c1Red = 0;
    else if (i4Cur > kCIDLib::c1MaxCard)
        m_c1Red = kCIDLib::c1MaxCard;
    else
        m_c1Red = tCIDLib::TCard1(i4Cur);

    i4Cur = m_c1Green;
    i4Cur += i4GreenOffset;
    if (i4Cur < 0)
        m_c1Green = 0;
    else if (i4Cur > kCIDLib::c1MaxCard)
        m_c1Green = kCIDLib::c1MaxCard;
    else
        m_c1Green = tCIDLib::TCard1(i4Cur);

    i4Cur = m_c1Blue;
    i4Cur += i4BlueOffset;
    if (i4Cur < 0)
        m_c1Blue = 0;
    else if (i4Cur > kCIDLib::c1MaxCard)
        m_c1Blue = kCIDLib::c1MaxCard;
    else
        m_c1Blue = tCIDLib::TCard1(i4Cur);
}


tCIDLib::TVoid TRGBClr::AdjustHue(const tCIDLib::TFloat4 f4Offset)
{
    // Convert to HSV first
    tCIDLib::TFloat4 f4Hue;
    tCIDLib::TFloat4 f4Saturation;
    tCIDLib::TFloat4 f4Value;
    ToHSV(f4Hue, f4Saturation, f4Value);

    f4Hue += f4Offset;
    if (f4Hue < 0)
        f4Hue = 0;
    else if (f4Hue > 360)
        f4Hue = 360;

    // And covert back with the new hue
    FromHSV(f4Hue, f4Saturation, f4Value);
}


tCIDLib::TVoid TRGBClr::AdjustSaturation(const tCIDLib::TFloat4 f4Offset)
{
    // Convert to HSV first
    tCIDLib::TFloat4 f4Hue;
    tCIDLib::TFloat4 f4Saturation;
    tCIDLib::TFloat4 f4Value;
    ToHSV(f4Hue, f4Saturation, f4Value);

    f4Saturation += f4Offset;
    if (f4Saturation < 0)
        f4Saturation = 0;
    else if (f4Saturation > 1)
        f4Saturation = 1;

    // And covert back with the new saturation
    FromHSV(f4Hue, f4Saturation, f4Value);
}


tCIDLib::TVoid TRGBClr::AdjustValue(const tCIDLib::TFloat4 f4Offset)
{
    // Convert to HSV first
    tCIDLib::TFloat4 f4Hue;
    tCIDLib::TFloat4 f4Saturation;
    tCIDLib::TFloat4 f4Value;
    ToHSV(f4Hue, f4Saturation, f4Value);

    f4Value += f4Offset;
    if (f4Value < 0)
        f4Value = 0;
    else if (f4Value > 1)
        f4Value = 1;

    // And covert back with the new value
    FromHSV(f4Hue, f4Saturation, f4Value);
}


//
//  Parses an color of out a string. It expects the format to be r,g,b[,a]
//  but you can indicate another separate character as a parameter, as long
//  as the values are in the right order. The alpha value is optional.
//
tCIDLib::TBoolean
TRGBClr::bParseFromText(const   TString&            strText
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chSepChar)
{
    // We need to pull the separate tokens out from the string
    TString strR = strText;
    TString strG;
    TString strB;
    TString strA;

    // If not at least three parameters, it's bad
    if (!strR.bSplit(strG, chSepChar)
    ||  !strG.bSplit(strB, chSepChar))
    {
        return kCIDLib::False;
    }

    tCIDLib::TCard4 c4R;
    tCIDLib::TCard4 c4G;
    tCIDLib::TCard4 c4B;
    tCIDLib::TCard4 c4A = 0;

    // We can have an alpha value, but it's optional
    if (strB.bSplit(strA, chSepChar))
    {
        //
        //  If we have it, check it now. It might be a floating point, so check for a
        //  period. If so, treat it as a percentage.
        //
        if (strA.bContainsChar(kCIDLib::chPeriod))
        {
            tCIDLib::TFloat8 f8Per;
            if (!strA.bToFloat8(f8Per))
                return kCIDLib::False;

            c4A = tCIDLib::TCard4(f8Per * 255);
        }
         else
        {
            // Has to just be a byte value
            if (!strA.bToCard4(c4A, eRadix) || (c4A > 255))
                return kCIDLib::False;
        }
    }

    // Check the required bits
    if (!strR.bToCard4(c4R, eRadix) || (c4R > 255))
        return kCIDLib::False;
    if (!strG.bToCard4(c4G, eRadix) || (c4G > 255))
        return kCIDLib::False;
    if (!strB.bToCard4(c4B, eRadix) || (c4B > 255))
        return kCIDLib::False;

    // Looks ok, so store the color values. Alpha was stored above
    m_c1Red = tCIDLib::TCard1(c4R);
    m_c1Green = tCIDLib::TCard1(c4G);;
    m_c1Blue = tCIDLib::TCard1(c4B);;
    m_c1Alpha = tCIDLib::TCard1(c4A);

    return kCIDLib::True;
}


tCIDLib::TCard1 TRGBClr::c1AdjustAlpha(const tCIDLib::TInt4 i4Offset)
{
    #if CID_DEBUG_ON
    if ((i4Offset < -kCIDLib::c1MaxCard) || (i4Offset > kCIDLib::c1MaxCard))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcClr_RGBRange
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }
    #endif

    tCIDLib::TInt4 i4Cur = m_c1Alpha;
    i4Cur += i4Offset;
    if (i4Cur < 0)
        m_c1Alpha = 0;
    else if (i4Cur > kCIDLib::c1MaxCard)
        m_c1Alpha = kCIDLib::c1MaxCard;
    else
        m_c1Alpha = tCIDLib::TCard1(i4Cur);

    return m_c1Alpha;
}


tCIDLib::TCard1 TRGBClr::c1AdjustBlue(const tCIDLib::TInt4 i4Offset)
{
    #if CID_DEBUG_ON
    if ((i4Offset < -kCIDLib::c1MaxCard) || (i4Offset > kCIDLib::c1MaxCard))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcClr_RGBRange
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }
    #endif

    tCIDLib::TInt4 i4Cur = m_c1Blue;
    i4Cur += i4Offset;
    if (i4Cur < 0)
        m_c1Blue = 0;
    else if (i4Cur > kCIDLib::c1MaxCard)
        m_c1Blue = kCIDLib::c1MaxCard;
    else
        m_c1Blue = tCIDLib::TCard1(i4Cur);

    return m_c1Blue;
}

tCIDLib::TCard1 TRGBClr::c1AdjustGreen(const tCIDLib::TInt4 i4Offset)
{
    #if     CID_DEBUG_ON
    if ((i4Offset < -kCIDLib::c1MaxCard) || (i4Offset > kCIDLib::c1MaxCard))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcClr_RGBRange
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }
    #endif

    tCIDLib::TInt4 i4Cur = m_c1Green;
    i4Cur += i4Offset;
    if (i4Cur < 0)
        m_c1Green = 0;
    else if (i4Cur > kCIDLib::c1MaxCard)
        m_c1Green = kCIDLib::c1MaxCard;
    else
        m_c1Green = tCIDLib::TCard1(i4Cur);

    return m_c1Green;
}

tCIDLib::TCard1 TRGBClr::c1AdjustRed(const tCIDLib::TInt4 i4Offset)
{
    #if     CID_DEBUG_ON
    if ((i4Offset < -kCIDLib::c1MaxCard) || (i4Offset > kCIDLib::c1MaxCard))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcClr_RGBRange
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }
    #endif

    tCIDLib::TInt4 i4Cur = m_c1Red;
    i4Cur += i4Offset;
    if (i4Cur < 0)
        m_c1Red = 0;
    else if (i4Cur > kCIDLib::c1MaxCard)
        m_c1Red = kCIDLib::c1MaxCard;
    else
        m_c1Red = tCIDLib::TCard1(i4Cur);

    return m_c1Red;
}


//
//  Returns a particular color component generically, which is very useful
//  in many algorithms to avoid having to have separate loops or switches
//  per color component.
//
//  If the gray scale component is asked for, which is not really supported
//  here but is in some other places, we'll gen up a gray scale equiv of
//  our color
//
tCIDLib::TCard1 TRGBClr::c1ClrComp(const tCIDLib::EClrComps eComp) const
{
    tCIDLib::TCard1 c1Ret = 0;
    switch(eComp)
    {
        case tCIDLib::EClrComps::Red :
            c1Ret = m_c1Red;
            break;

        case tCIDLib::EClrComps::Green :
            c1Ret = m_c1Green;
            break;

        case tCIDLib::EClrComps::Blue :
            c1Ret = m_c1Blue;
            break;

        case tCIDLib::EClrComps::Alpha :
            c1Ret = m_c1Alpha;
            break;

        case tCIDLib::EClrComps::Gray :
            c1Ret = tCIDLib::TCard1
            (
                (m_c1Red * 0.299F)
                + (m_c1Green * 0.587F)
                + (m_c1Blue * 0.114F)
            );
            break;
    };
    return c1Ret;
}


tCIDLib::TCard1 TRGBClr::c1MaxComponent() const
{
    if (m_c1Red > m_c1Green)
    {
        if (m_c1Red > m_c1Blue)
            return m_c1Red;
        return m_c1Blue;
    }
    if (m_c1Green > m_c1Blue)
        return m_c1Green;
    return m_c1Blue;
}


tCIDLib::TCard1 TRGBClr::c1MinComponent() const
{
    if (m_c1Red < m_c1Green)
    {
        if (m_c1Red < m_c1Blue)
            return m_c1Red;
        return m_c1Blue;
    }
    if (m_c1Green < m_c1Blue)
        return m_c1Green;
    return m_c1Blue;
}


tCIDLib::TCard4 TRGBClr::c4Magnitude() const
{
    return tCIDLib::TCard4(m_c1Red + m_c1Green + m_c1Blue);
}


// Make our components available as floating point as a convenience
tCIDLib::TFloat4 TRGBClr::f4Alpha() const
{
    return tCIDLib::TFloat4(m_c1Alpha) / 255;
}


tCIDLib::TFloat4 TRGBClr::f4Blue() const
{
    return tCIDLib::TFloat4(m_c1Blue) / 255;
}


tCIDLib::TFloat4 TRGBClr::f4Green() const
{
    return tCIDLib::TFloat4(m_c1Green) / 255;
}


tCIDLib::TFloat4 TRGBClr::f4Red() const
{
    return tCIDLib::TFloat4(m_c1Red) / 255;
}


//
//  Format us out to a string as delimited numbers. The caller indicates the
//  delimiter to use.
//
tCIDLib::TVoid
TRGBClr::FormatToText(          TString&            strToFill
                        , const tCIDLib::TCh        chSepChar
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TBoolean   bIncludeAlpha
                        , const tCIDLib::TBoolean   bAppend) const
{
    if (!bAppend)
        strToFill.Clear();

    strToFill.AppendFormatted(m_c1Red, eRadix);
    strToFill.Append(chSepChar);
    strToFill.AppendFormatted(m_c1Green, eRadix);
    strToFill.Append(chSepChar);
    strToFill.AppendFormatted(m_c1Blue, eRadix);

    if (bIncludeAlpha)
    {
        strToFill.Append(chSepChar);
        strToFill.AppendFormatted(m_c1Alpha, eRadix);
    }
}


// Set our color by converting an HSV color to RGB
tCIDLib::TVoid TRGBClr::FromHSV(const   tCIDLib::TFloat4    f4Hue
                                , const tCIDLib::TFloat4    f4Saturation
                                , const tCIDLib::TFloat4    f4Value)
{
    // Validate the values
    tCIDLib::TErrCode   errcBad = 0;
    tCIDLib::TFloat4    f4Bad = 0;
    if (f4Hue < 0)
    {
        errcBad = kCIDErrs::errcClr_BadHSVHue;
        f4Bad = f4Hue;
    }
     else if ((f4Saturation < 0) || (f4Saturation > 1))
    {
        errcBad = kCIDErrs::errcClr_BadHSVSaturation;
        f4Bad = f4Saturation;
    }
     else if ((f4Value < 0) || (f4Value > 1))
    {
        errcBad = kCIDErrs::errcClr_BadHSVValue;
        f4Bad = f4Value;
    }
    if (errcBad)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , errcBad
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TFloat(f4Bad)
        );
    }

    // If no saturation, then its gray scale
    if (f4Saturation == 0)
    {
        m_c1Red   = tCIDLib::TCard1(255 * f4Value);
        m_c1Green = tCIDLib::TCard1(255 * f4Value);
        m_c1Blue  = tCIDLib::TCard1(255 * f4Value);
    }
     else
    {
        const tCIDLib::TFloat4  f4Sector = f4Hue / 60;
        const tCIDLib::TInt4    i4Int    = TMathLib::i4Floor(f4Sector);
        const tCIDLib::TFloat4  f4Frac   = f4Sector - tCIDLib::TFloat4(i4Int);
        const tCIDLib::TFloat4  f4P      = f4Value * (1 - f4Saturation);
        const tCIDLib::TFloat4  f4Q      = f4Value
                                           * (1 - (f4Saturation * f4Frac));
        const tCIDLib::TFloat4  f4T      = f4Value
                                           * (1 - (f4Saturation * (1 - f4Frac)));

        switch(i4Int)
        {
            case 0 :
                m_c1Red = tCIDLib::TCard1(f4Value * 255);
                m_c1Green = tCIDLib::TCard1(f4T * 255);
                m_c1Blue = tCIDLib::TCard1(f4P * 255);
                break;

            case 1 :
                m_c1Red = tCIDLib::TCard1(f4Q * 255);
                m_c1Green = tCIDLib::TCard1(f4Value * 255);
                m_c1Blue = tCIDLib::TCard1(f4P * 255);
                break;

            case 2 :
                m_c1Red = tCIDLib::TCard1(f4P * 255);
                m_c1Green = tCIDLib::TCard1(f4Value * 255);
                m_c1Blue = tCIDLib::TCard1(f4T * 255);
                break;

            case 3 :
                m_c1Red = tCIDLib::TCard1(f4P * 255);
                m_c1Green = tCIDLib::TCard1(f4Q * 255);
                m_c1Blue = tCIDLib::TCard1(f4Value * 255);
                break;

            case 4 :
                m_c1Red = tCIDLib::TCard1(f4T * 255);
                m_c1Green = tCIDLib::TCard1(f4P * 255);
                m_c1Blue = tCIDLib::TCard1(f4Value * 255);
                break;

            default :
                m_c1Red = tCIDLib::TCard1(f4Value * 255);
                m_c1Green = tCIDLib::TCard1(f4P * 255);
                m_c1Blue = tCIDLib::TCard1(f4Q * 255);
                break;
        }
    }
}


tCIDLib::TVoid TRGBClr::Interpolate(const   TRGBClr&    rgbSec
                                    ,       TRGBClr&    rgbToFill) const
{
    const tCIDLib::TCard1 c1MaxR = tCIDLib::MaxVal(m_c1Red, rgbSec.m_c1Red);
    const tCIDLib::TCard1 c1MinR = tCIDLib::MinVal(m_c1Red, rgbSec.m_c1Red);

    const tCIDLib::TCard1 c1MaxG = tCIDLib::MaxVal(m_c1Green, rgbSec.m_c1Green);
    const tCIDLib::TCard1 c1MinG = tCIDLib::MinVal(m_c1Green, rgbSec.m_c1Green);

    const tCIDLib::TCard1 c1MaxB = tCIDLib::MaxVal(m_c1Blue, rgbSec.m_c1Blue);
    const tCIDLib::TCard1 c1MinB = tCIDLib::MinVal(m_c1Blue, rgbSec.m_c1Blue);

    rgbToFill.Set
    (
        (c1MinR + ((c1MaxR - c1MinR) / 2)) % 256
        , (c1MinG + ((c1MaxG - c1MinG) / 2)) % 256
        , (c1MinB + ((c1MaxB - c1MinB) / 2)) % 256
    );
}


tCIDLib::TVoid TRGBClr::Scale(const tCIDLib::TFloat4 f4Percent)
{
    if (f4Percent < 0)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcClr_BadBrightnessPer
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TFloat(f4Percent)
        );
    }

    tCIDLib::TFloat4 f4Tmp = f4Percent * m_c1Red;
    if (f4Tmp > 255)
        m_c1Red = 255;
    else
        m_c1Red = tCIDLib::TCard1(f4Tmp);

    f4Tmp = f4Percent * m_c1Green;
    if (f4Tmp > 255)
        m_c1Green = 255;
    else
        m_c1Green = tCIDLib::TCard1(f4Tmp);

    f4Tmp = f4Percent * m_c1Blue;
    if (f4Tmp > 255)
        m_c1Blue = 255;
    else
        m_c1Blue = tCIDLib::TCard1(f4Tmp);
}


tCIDLib::TVoid TRGBClr::Set(const   tCIDLib::TCard1 c1Red
                            , const tCIDLib::TCard1 c1Green
                            , const tCIDLib::TCard1 c1Blue)
{
    m_c1Red     = c1Red;
    m_c1Green   = c1Green;
    m_c1Blue    = c1Blue;
    m_c1Alpha   = 0;
}

tCIDLib::TVoid TRGBClr::Set(const   tCIDLib::TCard1 c1Red
                            , const tCIDLib::TCard1 c1Green
                            , const tCIDLib::TCard1 c1Blue
                            , const tCIDLib::TCard1 c1Alpha)
{
    m_c1Red     = c1Red;
    m_c1Green   = c1Green;
    m_c1Blue    = c1Blue;
    m_c1Alpha   = c1Alpha;
}


tCIDLib::TVoid TRGBClr::SetSaturation(const tCIDLib::TFloat4 f4ToSet)
{
    if ((f4ToSet < 0) || (f4ToSet > 1))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcClr_BadHSVSaturation
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TFloat(f4ToSet)
        );
    }

    // Convert to HSV first
    tCIDLib::TFloat4 f4Hue;
    tCIDLib::TFloat4 f4Saturation;
    tCIDLib::TFloat4 f4Value;
    ToHSV(f4Hue, f4Saturation, f4Value);

    // And covert back with the new saturation
    FromHSV(f4Hue, f4ToSet, f4Value);
}


tCIDLib::TVoid TRGBClr::SetValue(const tCIDLib::TFloat4 f4ToSet)
{
    if ((f4ToSet < 0) || (f4ToSet > 1))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcClr_BadHSVValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TFloat(f4ToSet)
        );
    }

    // Convert to HSV first
    tCIDLib::TFloat4 f4Hue;
    tCIDLib::TFloat4 f4Saturation;
    tCIDLib::TFloat4 f4Value;
    ToHSV(f4Hue, f4Saturation, f4Value);

    // And covert back with the new value
    FromHSV(f4Hue, f4Saturation, f4ToSet);
}


tCIDLib::TVoid TRGBClr::ToCodedRGB(tCIDLib::TCodedRGB& rgbTarget) const
{
    rgbTarget.c1Red     = m_c1Red;
    rgbTarget.c1Green   = m_c1Green;
    rgbTarget.c1Blue    = m_c1Blue;
    rgbTarget.c1Alpha   = m_c1Alpha;
}


tCIDLib::TVoid TRGBClr::ToRawColor(tCIDLib::TRawColor& clrTarget) const
{
    clrTarget.c1Red     = m_c1Red;
    clrTarget.c1Green   = m_c1Green;
    clrTarget.c1Blue    = m_c1Blue;
}

tCIDLib::TVoid
TRGBClr::ToRawAlphaColor(tCIDLib::TRawAlphaColor& clrTarget) const
{
    clrTarget.c1Red     = m_c1Red;
    clrTarget.c1Green   = m_c1Green;
    clrTarget.c1Blue    = m_c1Blue;
    clrTarget.c1Alpha   = m_c1Alpha;
}


tCIDLib::TVoid TRGBClr::ToRGB(tCIDLib::TRawRGB& rgbTarget) const
{
    rgbTarget.c1Red     = m_c1Red;
    rgbTarget.c1Green   = m_c1Green;
    rgbTarget.c1Blue    = m_c1Blue;
}


tCIDLib::TVoid TRGBClr::ToHSV(  tCIDLib::TFloat4&   f4Hue
                                , tCIDLib::TFloat4& f4Saturation
                                , tCIDLib::TFloat4& f4Value) const
{
    const tCIDLib::TFloat4 f4Red = tCIDLib::TFloat4(m_c1Red) / 255;
    const tCIDLib::TFloat4 f4Green = tCIDLib::TFloat4(m_c1Green) / 255;
    const tCIDLib::TFloat4 f4Blue = tCIDLib::TFloat4(m_c1Blue) / 255;

    // Get the min and max values of the src color components
    const tCIDLib::TFloat4 f4Min = tCIDLib::MinVal
    (
        tCIDLib::MinVal(f4Red, f4Green)
        , f4Blue
    );
    const tCIDLib::TFloat4 f4Max = tCIDLib::MaxVal
    (
        tCIDLib::MaxVal(f4Red, f4Green)
        , f4Blue
    );
    const tCIDLib::TFloat4 f4Delta = f4Max - f4Min;

    // The value is the maximum RGB color component
    f4Value = f4Max;

    if (f4Delta == 0)
    {
        //
        //  Its a gray scale value, so hue doesn't matter. Intensity
        //  of the gray is set by the value.
        //
        f4Saturation = 0;
        f4Hue = 0;
    }
     else if (f4Max == 0)
    {
        // Its pure black
        f4Saturation = 0;
        f4Hue = 0;
    }
     else
    {
        // Its the normal scenario so do the calcs
        f4Saturation = f4Delta / f4Max;

        if (f4Red == f4Max)
            f4Hue = (f4Green - f4Blue) / f4Delta;
        else if (f4Green == f4Max)
            f4Hue = 2 + (f4Blue - f4Red) / f4Delta;
        else
            f4Hue = 4 + (f4Red - f4Green) / f4Delta;

        f4Hue *= 60.0;
        if (f4Hue < 0)
            f4Hue += 360.0;
    }
}


tCIDLib::TVoid TRGBClr::WashOut()
{
    // Convert to HSV first
    tCIDLib::TFloat4 f4Hue;
    tCIDLib::TFloat4 f4Saturation;
    tCIDLib::TFloat4 f4Value;
    ToHSV(f4Hue, f4Saturation, f4Value);

    //
    //  Make sure that the saturation is less than 10% and the value is
    //  greater than 95%
    //
    if (f4Saturation > 0.10F)
        f4Saturation = 0.10F;

    if (f4Value < 0.95F)
        f4Value = 0.95F;

    // And covert back with the new values
    FromHSV(f4Hue, f4Saturation, f4Value);
}


// ---------------------------------------------------------------------------
//  TRGBClr: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TRGBClr::FormatTo(TTextOutStream& strmToWriteTo) const
{
    strmToWriteTo   << m_c1Red << L"," << m_c1Green << L"," << m_c1Blue;
}


tCIDLib::TVoid TRGBClr::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom  >> m_c1Red
                    >> m_c1Green
                    >> m_c1Blue
                    >> m_c1Alpha;
}


tCIDLib::TVoid TRGBClr::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << m_c1Red
                    << m_c1Green
                    << m_c1Blue
                    << m_c1Alpha;
}


