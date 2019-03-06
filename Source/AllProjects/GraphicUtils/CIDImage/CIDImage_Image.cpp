//
// FILE NAME: CIDImage_Image.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/17/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TCIDImage class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDImage_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDImage,TObject)


// ---------------------------------------------------------------------------
//  CLASS: TCIDImage
// PREFIX: img
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDImage: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TCIDImage::CvtTransClr( const   tCIDImage::EPixFmts     ePixFmt
                        , const tCIDImage::EBitDepths   eBitDepth
                        , const tCIDLib::TCard4         c4TransClr
                        , const TClrPalette&            palClrs
                        ,       TRGBClr&                rgbToFill)
{
    //
    //  Convert our trans color indicator to an RGB value. If our format is
    //  true color, then we can just return that. Else we have to convert to
    //  a color.
    //
    if (tCIDLib::bAllBitsOn(ePixFmt, tCIDImage::EPixFmts::Palette)
    ||  ((ePixFmt == tCIDImage::EPixFmts::GrayScale) && (tCIDLib::c4EnumOrd(eBitDepth) < 16)))
    {
        //
        //  Palette or gray scale without alpha and less than 16 bits can
        //  all be done via the palette.
        //
        if (c4TransClr >= palClrs.c4ClrCount())
            rgbToFill.ToBlack();
        else
            rgbToFill = palClrs[c4TransClr];
    }
     else if (tCIDLib::bAllBitsOn(ePixFmt, tCIDImage::EPixFmts::Color))
    {
        //
        //  If it's 5 bit, then convert, else it's true color, so we can
        //  use it as is.
        //
        if (eBitDepth == tCIDImage::EBitDepths::Five)
        {
            rgbToFill.c1Red(tCIDLib::TCard1(((c4TransClr >> 10) & 0x1F) << 3));
            rgbToFill.c1Green(tCIDLib::TCard1(((c4TransClr >> 5) & 0x1F) << 3));
            rgbToFill.c1Blue(tCIDLib::TCard1((c4TransClr & 0x1F) << 3));
        }
         else
        {
            rgbToFill = TRGBClr(c4TransClr);
        }
    }
     else
    {
        //
        //  It's 8 bit gray with alpha, or 16 bit gray with or without
        //  alpha.
        //
        if (eBitDepth == tCIDImage::EBitDepths::Eight)
        {
            tCIDLib::TCard1 c1Clr = tCIDLib::TCard1((c4TransClr >> 8) & 0xFF);
            rgbToFill.c1Red(c1Clr);
            rgbToFill.c1Green(c1Clr);
            rgbToFill.c1Blue(c1Clr);
            rgbToFill.c1Alpha(tCIDLib::TCard1(c4TransClr & 0xFF));
        }
         else
        {
            tCIDLib::TCard1 c1Clr = tCIDLib::TCard1(c4TransClr >> 24);
            rgbToFill.c1Red(c1Clr);
            rgbToFill.c1Green(c1Clr);
            rgbToFill.c1Blue(c1Clr);
            rgbToFill.c1Alpha(tCIDLib::TCard1((c4TransClr >> 8) & 0xFF));
        }
    }
}


// ---------------------------------------------------------------------------
//  TCIDImage: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDImage::~TCIDImage()
{
}


// ---------------------------------------------------------------------------
//  TCIDImage: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCIDImage::bIsRGBBased() const
{
    return m_pixaBits.bIsRGBBased();
}


tCIDLib::TBoolean
TCIDImage::bLoseAlpha(TPixelArray& pixaNew, tCIDLib::TCard4& c4TransClr)
{
    // If we have no alpha, then do nothing
    const TPixelArray& pixaUs = pixaBits();
    const tCIDImage::EPixFmts eFmt = pixaUs.ePixFmt();
    if (!tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Alpha))
        return kCIDLib::False;

    //
    //  Set the passed pixel array to our format, but without the alpha
    //  part. We know it has alpha, so it can only be truealpha, or 8 or 16
    //  bit gray scale with alpha.
    //
    const tCIDImage::EBitDepths eDepth = pixaUs.eBitDepth();
    if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Color))
    {
        pixaNew.Reset
        (
            tCIDImage::EPixFmts::TrueClr
            , tCIDImage::EBitDepths::Eight
            , eRowOrder()
            , szImage()
        );
    }
     else if (eDepth == tCIDImage::EBitDepths::Eight)
    {
        pixaNew.Reset
        (
            tCIDImage::EPixFmts::GrayScale
            , tCIDImage::EBitDepths::Eight
            , eRowOrder()
            , szImage()
        );
    }
     else if (eDepth == tCIDImage::EBitDepths::One)
    {
        pixaNew.Reset
        (
            tCIDImage::EPixFmts::GrayScale
            , tCIDImage::EBitDepths::One
            , eRowOrder()
            , szImage()
        );
    }
     else
    {
        // Shouldn't get here
        #if CID_DEBUG_ON
        #endif
    }

    //
    //  Find a color not used in the image. That will be our transparency
    //  color.
    //
    c4TransClr = pixaUs.c4Unused(palClrs());

    //
    //  And now loop through the pixel array and for any pixel that isn't
    //  almost completely opaque (by way of the alpha channel) set it to the
    //  transparency color.
    //
    const tCIDLib::TCard4 c4CY = c4Height();
    const tCIDLib::TCard4 c4CX = c4Width();
    tCIDLib::TCard4 c4Cur;

    if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Color))
    {
        tCIDLib::TCodedRGB* prgbCur = (tCIDLib::TCodedRGB*)&c4Cur;
        for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4CY; c4YInd++)
        {
            for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4CX; c4XInd++)
            {
                c4Cur = pixaUs.c4At(c4XInd, c4YInd);
                if (prgbCur->c1Alpha < 0xE0)
                    pixaNew.PutAt(c4TransClr, c4XInd, c4YInd);
                else
                    pixaNew.PutAt(c4Cur, c4XInd, c4YInd);
            }
        }
    }
     else if (eDepth == tCIDImage::EBitDepths::Eight)
    {
        for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4CY; c4YInd++)
        {
            for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4CX; c4XInd++)
            {
                c4Cur = pixaUs.c4At(c4XInd, c4YInd);
                if ((c4Cur & 0xFF) < 0xE0)
                    pixaNew.PutAt(c4TransClr, c4XInd, c4YInd);
                else
                    pixaNew.PutAt(c4Cur >> 8, c4XInd, c4YInd);
            }
        }
    }
     else if (eDepth == tCIDImage::EBitDepths::One)
    {
        for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4CY; c4YInd++)
        {
            for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4CX; c4XInd++)
            {
                c4Cur = pixaUs.c4At(c4XInd, c4YInd);
                if ((c4Cur & 0xFFFF) < 0xE000)
                    pixaNew.PutAt(c4TransClr, c4XInd, c4YInd);
                else
                    pixaNew.PutAt(c4Cur >> 16, c4XInd, c4YInd);
            }
        }
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TCIDImage::bTransClr() const
{
    return m_bTransClr;
}

tCIDLib::TBoolean TCIDImage::bTransClr(const tCIDLib::TBoolean bToSet)
{
    m_bTransClr = bToSet;
    return m_bTransClr;
}


tCIDLib::TVoid
TCIDImage::BSplineScaleFrom(const   TCIDImage&      imgSrc
                           , const  tCIDLib::TCard4 c4Degree)
{
    // Save current format info and then scale into our pixel buffer
    const tCIDImage::EPixFmts eFmt = m_pixaBits.ePixFmt();
    const tCIDImage::EBitDepths eDepth = m_pixaBits.eBitDepth();
    imgSrc.pixaBits().BSplineScaleTo(m_pixaBits, m_palClrs, c4Degree);

    //
    //  It may have changed the format, since palette based images cannot
    //  be scaled and it might have upgraded a low bit depth grayscale format
    //  to the next higher one to provide interpolation colors.
    //
    const tCIDImage::EPixFmts eNewFmt = m_pixaBits.ePixFmt();
    const tCIDImage::EBitDepths eNewDepth = m_pixaBits.eBitDepth();
    if ((eNewFmt != eFmt) || (eNewDepth != eDepth))
    {
        if ((eNewFmt == tCIDImage::EPixFmts::TrueClr)
        ||  (eNewFmt == tCIDImage::EPixFmts::TrueAlpha))
        {
            //
            //  We converted from a pixel format so the palette and trans
            //  color are no longer valid.
            //
            m_palClrs.ResetSize(2);
            m_bTransClr = kCIDLib::False;
        }
         else if ((eNewFmt == tCIDImage::EPixFmts::GrayScale)
              ||  (eNewFmt == tCIDImage::EPixFmts::GrayAlpha))
        {
            //
            //  We converted up from a lower gray scale bit depth, so update
            //  the palette for this new depth.
            //
            BuildGrayScalePal();
        }
    }
}


tCIDLib::TVoid
TCIDImage::BSplineScaleTo(          TPixelArray&    pixaTar
                            , const tCIDLib::TCard4 c4Degree) const
{
    m_pixaBits.BSplineScaleTo(pixaTar, m_palClrs, c4Degree);
}


tCIDLib::TCard4 TCIDImage::c4BitsPerPixel() const
{
    return m_pixaBits.c4BitsPer();
}


tCIDLib::TCard4 TCIDImage::c4ClrAt(const TPoint& pntAt) const
{
    if ((pntAt.i4X() < 0) || (pntAt.i4Y() < 0)
    ||  (tCIDLib::TCard4(pntAt.i4X()) >= m_pixaBits.c4Width())
    ||  (tCIDLib::TCard4(pntAt.i4Y()) >= m_pixaBits.c4Height()))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcImg_BadImgPoint
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , pntAt
            , m_pixaBits.szImage()
        );
    }
    return m_pixaBits.c4At(pntAt.i4X(), pntAt.i4Y());
}


tCIDLib::TCard4 TCIDImage::c4ClrsAvail() const
{
    return m_palClrs.c4ClrCount();
}


tCIDLib::TCard4 TCIDImage::c4ClrsImportant() const
{
    return m_c4ClrsImportant;
}


tCIDLib::TCard4 TCIDImage::c4ClrsUsed() const
{
    return m_c4ClrsUsed;
}


tCIDLib::TCard4 TCIDImage::c4Height() const
{
    return m_pixaBits.szImage().c4Height();
}


tCIDLib::TCard4 TCIDImage::c4ImageSz() const
{
    return m_pixaBits.c4ImageSz();
}


tCIDLib::TCard4 TCIDImage::c4LineByteWidth() const
{
    return m_pixaBits.c4LineWidth();
}


tCIDLib::TCard4 TCIDImage::c4TransClr() const
{
    return m_c4TransClr;
}

tCIDLib::TCard4 TCIDImage::c4TransClr(const tCIDLib::TCard4 c4ToSet)
{
    m_c4TransClr = c4ToSet;
    return m_c4TransClr;
}


tCIDLib::TCard4 TCIDImage::c4UserData() const
{
    return m_c4UserData;
}

tCIDLib::TCard4 TCIDImage::c4UserData(const tCIDLib::TCard4 c4ToSet)
{
    m_c4UserData = c4ToSet;
    return m_c4UserData;
}


tCIDLib::TCard4 TCIDImage::c4Width() const
{
    return m_pixaBits.szImage().c4Width();
}


tCIDLib::TVoid TCIDImage::CvtToGrayScale()
{
    //
    //  Just delegate to our pixel array, and pass long our palette in case
    //  he needs to update the palette with a gray scale palette.
    //
    m_pixaBits.CvtToGrayScale(m_palClrs);
}


tCIDImage::EBitDepths TCIDImage::eBitDepth() const
{
    return m_pixaBits.eBitDepth();
}


tCIDImage::EPixFmts TCIDImage::ePixFmt() const
{
    return m_pixaBits.ePixFmt();
}


tCIDImage::ERowOrders TCIDImage::eRowOrder() const
{
    return m_pixaBits.eRowOrder();
}


tCIDImage::EImgTypes TCIDImage::eType() const
{
    return m_eType;
}


tCIDLib::TFloat8 TCIDImage::f8Gamma() const
{
    return m_f8Gamma;
}

tCIDLib::TFloat8 TCIDImage::f8Gamma(const tCIDLib::TFloat8 f8ToSet)
{
    m_f8Gamma = f8ToSet;
    return m_f8Gamma;
}


// Flip our pixel array around the horz or vertical axis
tCIDLib::TVoid TCIDImage::FlipHorizontally()
{
    m_pixaBits.FlipHorizontally();
}

tCIDLib::TVoid TCIDImage::FlipVertically()
{
    m_pixaBits.FlipVertically();
}


//
//  This method will do the work to create a thumb image of this image into
//  the passed pixel array. The pixel array indicates the required target
//  size.
//
tCIDLib::TVoid
TCIDImage::MakeScaled(TPixelArray& pixaThumb, const tCIDLib::TCard4 c4Degree) const
{
    m_pixaBits.MakeScaled(pixaThumb, m_palClrs, m_bTransClr, m_c4TransClr, c4Degree);
}


tCIDLib::TVoid
TCIDImage::MakeScaledFrom(const TCIDImage& imgThumb, const tCIDLib::TCard4 c4Degree)
{
    imgThumb.m_pixaBits.MakeScaled
    (
        m_pixaBits
        , imgThumb.m_palClrs
        , imgThumb.m_bTransClr
        , imgThumb.m_c4TransClr
        , c4Degree
    );

    //
    //  If we are now no longer palette based, trash the palette and convert
    //  the transparency color to RGB.
    //
    m_bTransClr = imgThumb.m_bTransClr;
    if (tCIDLib::bAllBitsOn(imgThumb.ePixFmt(), tCIDImage::EPixFmts::Palette))
    {
        if ((m_pixaBits.ePixFmt() == tCIDImage::EPixFmts::TrueClr)
        ||  (m_pixaBits.ePixFmt() == tCIDImage::EPixFmts::TrueAlpha))
        {
            m_palClrs.ResetSize(2);
            m_c4ClrsImportant = 0;
            m_c4ClrsUsed = 0;
        }
        m_c4TransClr = imgThumb.rgbTransClr().c4Color();
    }
     else
    {
        m_c4TransClr = imgThumb.m_c4TransClr;
    }
}


// Get access to the palete data
const TClrPalette& TCIDImage::palClrs() const
{
    return m_palClrs;
}

TClrPalette& TCIDImage::palClrs()
{
    return m_palClrs;
}


// Get read access to the raw image bits
const TPixelArray& TCIDImage::pixaBits() const
{
    return m_pixaBits;
}


//
//  Ask our pixel array object to give us the RGB (or RGB equiv) of the pixel
//  at the indicated point.
//
TRGBClr TCIDImage::rgbAt(const TPoint& pntAt) const
{
    TRGBClr rgbRet;
    m_pixaBits.QueryRGBAt(pntAt, m_palClrs, rgbRet);
    return rgbRet;
}


// Get/set the image background color
const TRGBClr& TCIDImage::rgbBgnClr() const
{
    return m_rgbBgn;
}

const TRGBClr& TCIDImage::rgbBgnClr(const TRGBClr& rgbToSet)
{
    m_rgbBgn = rgbToSet;
    return m_rgbBgn;
}


// Get the image transparency color
TRGBClr TCIDImage::rgbTransClr() const
{
    //
    //  Call the static helper method that does this conversion, and get
    //  the result into a local we can return.
    //
    TRGBClr rgbRet;
    CvtTransClr
    (
        m_pixaBits.ePixFmt()
        , m_pixaBits.eBitDepth()
        , m_c4TransClr
        , m_palClrs
        , rgbRet
    );
    return rgbRet;
}


TSize TCIDImage::szImage() const
{
    return m_pixaBits.szImage();
}


TSize TCIDImage::szResolution() const
{
    return TSize(m_c4HorzRes, m_c4VertRes);
}

TSize TCIDImage::szResolution(const TSize& szToSet)
{
    m_c4HorzRes = szToSet.c4Width();
    m_c4VertRes = szToSet.c4Height();
    return szToSet;
}


tCIDLib::TVoid TCIDImage::SetBits(const TPixelArray& pixaToSet)
{
    // It has to be the same format as we currently have and the same size
    if (!m_pixaBits.bIsSameFormat(pixaToSet, kCIDLib::True))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcImg_NewBitsDifferentFmt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }
    m_pixaBits = pixaToSet;
}


//
//  This method will convert a palette based image to an RGBA image, using
//  the current palette and the current transparency color (if any.)
//
tCIDLib::TVoid TCIDImage::PalToRGB()
{
    //
    //  Ask the pixel array to convert itself, but in this case don't ask
    //  it to change the transparency color.
    //
    TRGBClr rgbDummy;
    if (m_pixaBits.bPalToRGB(m_palClrs
                            , m_bTransClr
                            , m_c4TransClr
                            , kCIDLib::False
                            , rgbDummy))
    {
        //
        //  It was converted, so update ourself. We back the palette down
        //  to the minimum B&W palette and turn off transparency color.
        //
        m_palClrs.ResetSize(2);
        m_palClrs[1].Set(0xFF, 0xFF, 0xFF);
        m_bTransClr = kCIDLib::False;
    }
}


tCIDLib::TVoid TCIDImage::PalToRGB(const TRGBClr& rgbTransRep)
{
    // Ask the pixel array to convert itself
    if (m_pixaBits.bPalToRGB(m_palClrs
                            , m_bTransClr
                            , m_c4TransClr
                            , kCIDLib::True
                            , rgbTransRep))
    {
        //
        //  It was converted, so update ourself. We back the palette down
        //  to the minimum B&W palette and turn off transparency color.
        //
        m_palClrs.ResetSize(2);
        m_palClrs[1].Set(0xFF, 0xFF, 0xFF);
        m_bTransClr = kCIDLib::False;
    }
}


//
//  This method will convert an RGB image using color based tranparency to
//  an RGBA image using source alpha transparency.
//
tCIDLib::TVoid TCIDImage::RGBTToRGBA()
{
    //
    //  Ask the pixel array to convert itself, but in this case don't ask
    //  it to change the transparency color.
    //
    TRGBClr rgbDummy;
    if (m_pixaBits.bRGBToRGBA(m_c4TransClr, kCIDLib::False, rgbDummy))
    {
        // We aren't using the transparency color anymore
        m_bTransClr = kCIDLib::False;
    }
}


tCIDLib::TVoid TCIDImage::RGBTToRGBA(const TRGBClr& rgbTransRep)
{
    // Ask the pixel array to convert itself
    if (m_pixaBits.bRGBToRGBA(m_c4TransClr, kCIDLib::True, rgbTransRep))
    {
        // We don't use the transparency color anymore
        m_bTransClr = kCIDLib::False;
    }
}


tCIDLib::TVoid TCIDImage::ReplaceTransClr(const tCIDLib::TCard4 c4RepClr)
{
    if (m_bTransClr)
        m_pixaBits.ReplaceClr(m_c4TransClr, c4RepClr);
}


tCIDLib::TVoid
TCIDImage::Scale(const TSize& szNew, const tCIDLib::TCard4 c4Degree)
{
    // Scale into a local temp array
    TPixelArray pixaTmp
    (
        tCIDImage::EPixFmts::GrayScale
        , tCIDImage::EBitDepths::One
        , m_pixaBits.eRowOrder()
        , szNew
    );
    m_pixaBits.MakeScaled(pixaTmp, m_palClrs, m_bTransClr, m_c4TransClr, c4Degree);

    //
    //  If we were palette based, we won't be now, so clear the palette
    //  colors values and reset the palette to a default 2 color size.
    //
    if (tCIDLib::bAllBitsOn(m_pixaBits.ePixFmt(), tCIDImage::EPixFmts::Palette))
    {
        //
        //  Before we toss the palette, change the transparency color, if
        //  any, to true color form. It's current an index into the palette.
        //
        if (m_bTransClr)
        {
            if (m_c4TransClr >= m_palClrs.c4ClrCount())
                m_c4TransClr = 0;
            else
                m_c4TransClr = m_palClrs[m_c4TransClr].c4Color();
        }

        m_c4ClrsImportant = 0;
        m_c4ClrsUsed = 0;
        m_palClrs.ResetSize(2);
    }

    // Now store the new pixels as ours
    m_pixaBits = pixaTmp;
}



// ---------------------------------------------------------------------------
//  TCIDImage: Hidden constructors and operators
// ---------------------------------------------------------------------------
TCIDImage::TCIDImage(const tCIDImage::EImgTypes eType) :

    m_bTransClr(kCIDLib::False)
    , m_c4ClrsImportant(0)
    , m_c4ClrsUsed(0)
    , m_c4HorzRes(72)
    , m_c4TransClr(0)
    , m_c4VertRes(72)
    , m_c4UserData(0)
    , m_eType(eType)
    , m_f8Gamma(2.2)
    , m_pixaBits
      (
        tCIDImage::EPixFmts::GrayScale
        , tCIDImage::EBitDepths::One
        , tCIDImage::ERowOrders::TopDown
        , TSize(1, 1)
      )
    , m_palClrs(2)
{
}

TCIDImage::TCIDImage(const  tCIDImage::EImgTypes    eType
                    , const tCIDImage::EPixFmts     eFmt
                    , const tCIDImage::EBitDepths   eDepth
                    , const tCIDImage::ERowOrders   eRowOrder
                    , const TSize&                  szImage
                    , const TClrPalette&            palToUse
                    , const tCIDLib::TCard4         c4ClrsImportant
                    , const tCIDLib::TCard4         c4ClrsUsed) :
    m_eType(eType)
{
    // Call the set method, which does all this work already
    Set(eFmt, eDepth, eRowOrder, szImage, palToUse, c4ClrsImportant, c4ClrsUsed);
}

TCIDImage::TCIDImage(const  tCIDImage::EImgTypes    eType
                    , const TPixelArray&            pixaToSet
                    , const tCIDLib::TCard4         c4ClrsImportant
                    , const tCIDLib::TCard4         c4ClrsUsed) :

    m_eType(eType)
{
    // Call the set method, which does all this work
    Set(pixaToSet, c4ClrsImportant, c4ClrsUsed);
}

TCIDImage::TCIDImage(const  tCIDImage::EImgTypes    eType
                    , const TPixelArray&            pixaToSet
                    , const TClrPalette&            palToUse
                    , const tCIDLib::TCard4         c4ClrsImportant
                    , const tCIDLib::TCard4         c4ClrsUsed) :
    m_eType(eType)
{
    // Call the set method, which does all this work
    Set(pixaToSet, palToUse, c4ClrsImportant, c4ClrsUsed);
}

//
//  Creates a new image with all of the attributes of the passed source
//  image, but with a pixel array of a new size. The pixel data is not
//  passed since it can't fit. The caller must provide new content of
//  the right size (usually through some scaling operation.)
//
TCIDImage::TCIDImage(const  tCIDImage::EImgTypes    eType
                    , const TCIDImage&              imgSrc
                    , const TSize&                  szNew) :

    m_bTransClr(imgSrc.m_bTransClr)
    , m_c4ClrsImportant(imgSrc.m_c4ClrsImportant)
    , m_c4ClrsUsed(imgSrc.m_c4ClrsUsed)
    , m_c4HorzRes(imgSrc.m_c4HorzRes)
    , m_c4TransClr(imgSrc.m_c4TransClr)
    , m_c4VertRes(imgSrc.m_c4VertRes)
    , m_c4UserData(0)
    , m_eType(eType)
    , m_f8Gamma(imgSrc.m_f8Gamma)
    , m_pixaBits
      (
        imgSrc.m_pixaBits.ePixFmt()
        , imgSrc.m_pixaBits.eBitDepth()
        , imgSrc.m_pixaBits.eRowOrder()
        , szNew
      )
    , m_rgbBgn(imgSrc.m_rgbBgn)
    , m_palClrs(imgSrc.m_palClrs)
{
}

TCIDImage::TCIDImage(const  tCIDImage::EImgTypes    eType
                    , const  TCIDImage&             imgSrc) :

    m_bTransClr(imgSrc.m_bTransClr)
    , m_c4ClrsImportant(imgSrc.m_c4ClrsImportant)
    , m_c4ClrsUsed(imgSrc.m_c4ClrsUsed)
    , m_c4HorzRes(imgSrc.m_c4HorzRes)
    , m_c4TransClr(imgSrc.m_c4TransClr)
    , m_c4VertRes(imgSrc.m_c4VertRes)
    , m_c4UserData(imgSrc.m_c4UserData)
    , m_eType(eType)
    , m_f8Gamma(imgSrc.m_f8Gamma)
    , m_pixaBits(imgSrc.m_pixaBits)
    , m_rgbBgn(imgSrc.m_rgbBgn)
    , m_palClrs(imgSrc.m_palClrs)
{
}

//
//  Note we don't change the type. The source may be a diferent type, i.e.
//  an assigment used for conversion. The ctor already set the type so we
//  don't want to change do it.
//
//  So we can't use a defaulted assignment.
//
TCIDImage& TCIDImage::operator=(const TCIDImage& imgSrc)
{
    if (this != &imgSrc)
    {
        m_bTransClr       = imgSrc.m_bTransClr;
        m_c4ClrsImportant = imgSrc.m_c4ClrsImportant;
        m_c4ClrsUsed      = imgSrc.m_c4ClrsUsed;
        m_c4HorzRes       = imgSrc.m_c4HorzRes;
        m_c4TransClr      = imgSrc.m_c4TransClr;
        m_c4VertRes       = imgSrc.m_c4VertRes;
        m_c4UserData      = imgSrc.m_c4UserData;
        m_f8Gamma         = imgSrc.m_f8Gamma;
        m_pixaBits        = imgSrc.m_pixaBits;
        m_rgbBgn          = imgSrc.m_rgbBgn;
        m_palClrs         = imgSrc.m_palClrs;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TCIDImage: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDImage::ForceRowOrder(const tCIDImage::ERowOrders eRowOrder)
{
    // Just delegate to the pixel array
    m_pixaBits.ForceRowOrder(eRowOrder);
}


// We only allow non-const direct access to the bits to our derived classes
TPixelArray& TCIDImage::pixaNCBits()
{
    return m_pixaBits;
}


tCIDLib::TVoid
TCIDImage::Set( const   tCIDImage::EPixFmts     eFmt
                , const tCIDImage::EBitDepths   eDepth
                , const tCIDImage::ERowOrders   eRowOrder
                , const TSize&                  szImage
                , const tCIDLib::TCard4         c4ClrsImportant
                , const tCIDLib::TCard4         c4ClrsUsed)
{
    m_bTransClr = kCIDLib::False;
    m_c4HorzRes = 72;
    m_c4TransClr = 0;
    m_c4VertRes = 72;
    m_c4UserData = 0;
    m_f8Gamma   = 2.2;
    m_rgbBgn.ToBlack();

    // And reset the pixel array for the new data
    m_pixaBits.Reset(eFmt, eDepth, eRowOrder, szImage);

    //
    //  Reset the colors and palette. If the format indicates a palette,
    //  then set the palette to the correct number of colors.
    //
    //  Note that, if it's a gray scale, we build a palette for it so that
    //  it's easier to get the values out as RGB. If it's 16 bit gray, then
    //  they have to take just the top 8 bits to use with the palette.
    //
    if ((m_pixaBits.ePixFmt() == tCIDImage::EPixFmts::GrayScale)
    ||  (m_pixaBits.ePixFmt() == tCIDImage::EPixFmts::GrayAlpha))
    {
        BuildGrayScalePal();
    }
     else if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Palette))
    {
        const tCIDLib::TCard4 c4FullSz = 0x1UL << tCIDLib::c4EnumOrd(eDepth);
        if (c4ClrsUsed && (c4ClrsUsed <= c4FullSz))
            m_c4ClrsUsed = c4ClrsUsed;
        else
            m_c4ClrsUsed = 0;

        m_c4ClrsImportant = m_c4ClrsImportant;
        if (m_c4ClrsImportant > c4FullSz)
        {
            if (m_c4ClrsUsed && (m_c4ClrsImportant > m_c4ClrsUsed))
                m_c4ClrsImportant = m_c4ClrsUsed;
            else
                m_c4ClrsImportant = c4FullSz;
        }

        if (m_c4ClrsUsed)
            m_palClrs.ResetSize(m_c4ClrsUsed);
        else
            m_palClrs.ResetSize(c4FullSz);
    }
     else
    {
        m_palClrs.ResetSize(2);
        m_c4ClrsImportant = 0;
        m_c4ClrsUsed = 0;
    }
}


tCIDLib::TVoid
TCIDImage::Set( const   tCIDImage::EPixFmts     eFmt
                , const tCIDImage::EBitDepths   eDepth
                , const tCIDImage::ERowOrders   eRowOrder
                , const TSize&                  szImage
                , const TClrPalette&            palToUse
                , const tCIDLib::TCard4         c4ClrsImportant
                , const tCIDLib::TCard4         c4ClrsUsed)
{
    // Call the other version
    Set(eFmt, eDepth, eRowOrder, szImage, c4ClrsImportant, c4ClrsUsed);

    // If palette based, then copy over the palette data
    if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Palette))
    {
        if (m_c4ClrsUsed)
            m_palClrs.CopyNFrom(palToUse, m_c4ClrsUsed);
        else
            m_palClrs.CopyNFrom(palToUse, 0x1UL << tCIDLib::c4EnumOrd(eDepth));
    }
}


tCIDLib::TVoid
TCIDImage::Set( const   TPixelArray&    pixaToUse
                , const tCIDLib::TCard4 c4ClrsImportant
                , const tCIDLib::TCard4 c4ClrsUsed)
{
    m_bTransClr = kCIDLib::False;
    m_c4HorzRes = 72;
    m_c4TransClr = 0;
    m_c4VertRes = 72;
    m_c4UserData = 0;
    m_f8Gamma   = 2.2;
    m_rgbBgn.ToBlack();

    // Copy over the pixel array
    m_pixaBits = pixaToUse;

    //
    //  Reset the colors and palette. If the format indicates a palette,
    //  then set the palette to the correct number of colors.
    //
    //  Note that, if it's a gray scale, we build a palette for it so that
    //  it's easier to get the values out as RGB. If it's 16 bit gray, then
    //  they have to take just the top 8 bits to use with the palette.
    //
    const tCIDImage::EPixFmts eFmt = m_pixaBits.ePixFmt();
    if ((eFmt == tCIDImage::EPixFmts::GrayScale)
    ||  (eFmt == tCIDImage::EPixFmts::GrayAlpha))
    {
        BuildGrayScalePal();
    }
     else if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Palette))
    {
        const tCIDLib::TCard4 c4FullSz = 0x1UL << tCIDLib::c4EnumOrd(m_pixaBits.eBitDepth());
        if (c4ClrsUsed && (c4ClrsUsed <= c4FullSz))
            m_c4ClrsUsed = c4ClrsUsed;
        else
            m_c4ClrsUsed = 0;

        m_c4ClrsImportant = m_c4ClrsImportant;
        if (m_c4ClrsImportant > c4FullSz)
        {
            if (m_c4ClrsUsed && (m_c4ClrsImportant > m_c4ClrsUsed))
                m_c4ClrsImportant = m_c4ClrsUsed;
            else
                m_c4ClrsImportant = c4FullSz;
        }
        m_palClrs.ResetSize(tCIDLib::MinVal(m_c4ClrsUsed, c4FullSz));

        if (m_c4ClrsUsed)
            m_palClrs.ResetSize(m_c4ClrsUsed);
        else
            m_palClrs.ResetSize(c4FullSz);
    }
     else
    {
        m_palClrs.ResetSize(2);
        m_c4ClrsImportant = 0;
        m_c4ClrsUsed = 0;
    }
}


tCIDLib::TVoid
TCIDImage::Set( const   TPixelArray&    pixaToUse
                , const TClrPalette&    palToUse
                , const tCIDLib::TCard4 c4ClrsImportant
                , const tCIDLib::TCard4 c4ClrsUsed)
{
    // Call the other version first
    Set(pixaToUse, c4ClrsImportant, c4ClrsUsed);

    // If palette based, then copy over the palette data
    if (tCIDLib::bAllBitsOn(pixaToUse.ePixFmt(), tCIDImage::EPixFmts::Palette))
    {
        if (m_c4ClrsUsed)
            m_palClrs.CopyNFrom(palToUse, m_c4ClrsUsed);
        else
            m_palClrs.CopyNFrom(palToUse, 0x1UL << tCIDLib::c4EnumOrd(pixaToUse.ePixFmt()));
    }
}


// ---------------------------------------------------------------------------
//  TCIDImage: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If we are set up for a gray scale less than 16 bits, we set up a palette
//  for it so that it's easier to get RGB data out.
//
tCIDLib::TVoid TCIDImage::BuildGrayScalePal()
{
    m_c4ClrsUsed = m_palClrs.c4BuildGrayScale(m_pixaBits.eBitDepth());
    m_c4ClrsImportant = m_c4ClrsUsed;
}


