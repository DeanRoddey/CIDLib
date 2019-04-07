//
// FILE NAME: CIDImage_PixelArray.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/28/1998
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
//  This file implements the TPixelArray class.
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
RTTIDecls(TPixelArray,TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDImage_PixelArray
{
    // -----------------------------------------------------------------------
    //  Our persistent format version
    //
    //  1.  Original version
    //  2.  Moved to the pixelfmt/depth combination to describe the data format
    //      and added new formats to cover the full range of PNG images.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard2   c2FmtVersion = 2;
}



// ---------------------------------------------------------------------------
//  CLASS: TPixelArray
// PREFIX: pixa
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPixelArray: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TPixelArray::c4CalcBitsPerPixel(const   tCIDImage::EPixFmts     eFmt
                                , const tCIDImage::EBitDepths   eBitDepth)
{
    return c4ValidateFmt(eFmt, eBitDepth);
}


tCIDLib::TCard4
TPixelArray::c4CalcBytesForImage(const  tCIDLib::TCard4         c4Width
                                , const tCIDLib::TCard4         c4Height
                                , const tCIDImage::EPixFmts     eFmt
                                , const tCIDImage::EBitDepths   eBitDepth
                                , const tCIDLib::TCard4         c4LineBytePadding)
{
    return c4CalcLineWidth(c4Width, eFmt, eBitDepth, c4LineBytePadding) * c4Height;
}


tCIDLib::TCard4
TPixelArray::c4CalcLineWidth(const  tCIDLib::TCard4         c4Width
                            , const tCIDImage::EPixFmts     eFmt
                            , const tCIDImage::EBitDepths   eBitDepth
                            , const tCIDLib::TCard4         c4LineBytePadding)
{
    if (!c4LineBytePadding)
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPixa_BadPadding
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }

    const tCIDLib::TCard4 c4PadBits = c4LineBytePadding * 8;

    // Validate the fmt/depth combo and get back the per-sample bits
    const tCIDLib::TCard4 c4BitsPer = c4ValidateFmt(eFmt, eBitDepth);

    // Calculate the byte width of the image, padded correctly
    const tCIDLib::TCard4 c4Ret
    (
        ((c4Width * c4BitsPer) + (c4PadBits - 1)) / c4PadBits
    );
    return c4Ret * c4LineBytePadding;
}


tCIDLib::TCard4
TPixelArray::c4ValidateFmt( const   tCIDImage::EPixFmts     eFmt
                            , const tCIDImage::EBitDepths   eBitDepth)
{
    // Calculate the data per pixel
    tCIDLib::TBoolean   bValid = kCIDLib::False;
    tCIDLib::TCard4     c4Ret;
    if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Palette))
    {
        // It will be just the depth as is, but cannot be 5 or greater than 8
        bValid = (eBitDepth != tCIDImage::EBitDepths::Five)
                 && (eBitDepth <= tCIDImage::EBitDepths::Eight);
        if (bValid)
            c4Ret = tCIDLib::c4EnumOrd(eBitDepth);
    }
     else if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Color))
    {
        // It can only be 5 or 8 bit
        if (eBitDepth == tCIDImage::EBitDepths::Five)
        {
            if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Alpha))
                bValid = kCIDLib::False;
            else
                bValid = kCIDLib::True;

            // Not 15 bits, but 16 with 1 unused
            c4Ret = 16;
        }
         else if (eBitDepth == tCIDImage::EBitDepths::Eight)
        {
            if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Alpha))
                c4Ret = 32;
            else
                c4Ret = 24;
            bValid = kCIDLib::True;
        }
    }
     else
    {
        //
        //  It's gray scale. Verify that the depth is valid for it. The
        //  only depth not valid for gray scale is 5. 5 is a color format.
        //
        bValid = eBitDepth != tCIDImage::EBitDepths::Five;

        // If valid, then the per-pixel is just the depth
        if (bValid)
        {
            c4Ret = tCIDLib::c4EnumOrd(eBitDepth);

            // If alpha, then add another depth, but it can only be 8/16
            if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Alpha))
            {
                bValid = (eBitDepth == tCIDImage::EBitDepths::Eight)
                         || (eBitDepth == tCIDImage::EBitDepths::Sixteen);
                if (bValid)
                    c4Ret += tCIDLib::c4EnumOrd(eBitDepth);
            }
        }
    }

    if (!bValid)
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcImg_BadFmt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , TInteger(tCIDLib::i4EnumOrd(eFmt))
            , TInteger(tCIDLib::i4EnumOrd(eBitDepth))
        );
    }
    return c4Ret;
}



// ---------------------------------------------------------------------------
//  TPixelArray: Constructors and Destructor
// ---------------------------------------------------------------------------

// We just create a 1x1 1 B&W image for default ctor
TPixelArray::TPixelArray() :

    m_pixaiInt(nullptr)
{
    m_pixaiInt = ppixaiMakeNew();
}

TPixelArray::TPixelArray(const  tCIDImage::EPixFmts     eFmt
                        , const tCIDImage::EBitDepths   eBitDepth
                        , const tCIDImage::ERowOrders   eRowOrder
                        , const TSize&                  szImage
                        ,       tCIDLib::TCard1* const  pc1Buf
                        , const tCIDLib::TCard4         c4BufSz
                        , const tCIDLib::EAdoptOpts     eAdoptOpt) :

    m_pixaiInt(nullptr)
{
    m_pixaiInt = ppixaiMakeNew(eFmt, eBitDepth, eRowOrder, szImage, pc1Buf, c4BufSz, eAdoptOpt);
}


TPixelArray::TPixelArray(const TCIDImage& imgSource) :

    m_pixaiInt(nullptr)
{
    m_pixaiInt = ppixaiMakeNew
    (
        imgSource.ePixFmt()
        , imgSource.eBitDepth()
        , imgSource.eRowOrder()
        , imgSource.szImage()
    );
}

TPixelArray::TPixelArray(const  tCIDImage::EPixFmts     eFmt
                        , const tCIDImage::EBitDepths   eBitDepth
                        , const tCIDImage::ERowOrders   eRowOrder
                        , const  TSize&                 szImage) :

    m_pixaiInt(nullptr)
{
    m_pixaiInt = ppixaiMakeNew(eFmt, eBitDepth, eRowOrder, szImage);
}

TPixelArray::TPixelArray(const TPixelArray& pixaSrc) :

    m_pixaiInt(nullptr)
{
    m_pixaiInt = new TPixelArrayImpl(*pixaSrc.m_pixaiInt);
}

TPixelArray::~TPixelArray()
{
    try
    {
        delete m_pixaiInt;
        m_pixaiInt = nullptr;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }
}


// ---------------------------------------------------------------------------
//  TPixelArray: Public operators
// ---------------------------------------------------------------------------
TPixelArray& TPixelArray::operator=(const TPixelArray& pixaSrc)
{
    if (this != &pixaSrc)
    {
        // Clean up any current impl object and create a new one
        delete m_pixaiInt;
        m_pixaiInt = nullptr;

        m_pixaiInt = new TPixelArrayImpl(*pixaSrc.m_pixaiInt);
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TPixelArray: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Similar to CreateAlphaChannelFrom() except this one only will update the
//  pixels in the indicated area. The other will set any pixels not within
//  the indicated area to fully transparent.
//
//  The passed alpha array must fit within this one, when it is put at pntPlaceAt
//  relative to our origin. That is the area where it will be applied.
//
//  This guy can pre-multiply the values to the new alpha, And it can force
//  the alpha to black and white, i.e. if bForceBW is true, then any non-zero
//  value is treated as 255.
//
tCIDLib::TVoid
TPixelArray::ApplyAlphaChannel( const   TPixelArray&        pixaAlpha
                                , const TArea&              areaSrc
                                , const TPoint&             pntTar
                                , const tCIDLib::TBoolean   bPremultiply
                                , const tCIDLib::TBoolean   bForceBW)
{
    // We have to be a true alpha based format
    if ((m_pixaiInt->m_eFmt != tCIDImage::EPixFmts::TrueAlpha)
    ||  (m_pixaiInt->m_eBitDepth != tCIDImage::EBitDepths::Eight))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPixa_WrongTarFormat
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(L"true alpha")
        );
    }

    //
    //  Calculate the real source area that we can actually use. We need to clip away
    //  any of the indicated source that would fall outside of the source pixel array.
    //  If empty, we are done.
    //
    TArea areaRealSrc(areaSrc);
    areaRealSrc &= TArea(0, 0, pixaAlpha.c4Width(), pixaAlpha.c4Height());
    if (areaRealSrc.bIsEmpty())
        return;

    //
    //  Now get the actual area of us that we can possibly update. It's the real
    //  source size shifted up to the target point within us, clipped against our
    //  area. Again, if empty, we are done.
    //
    const TArea areaUs(0, 0, m_pixaiInt->m_c4Width, m_pixaiInt->m_c4Height);
    TArea areaUpdate(areaRealSrc);
    areaUpdate.SetOrg(pntTar);
    areaUpdate &= areaUs;
    if (areaUpdate.bIsEmpty())
        return;

    m_pixaiInt->ApplyAlphaChannel
    (
        *pixaAlpha.m_pixaiInt, areaRealSrc, areaUpdate, pntTar, bPremultiply, bForceBW
    );
}


// Makes sure that our format (and optionally size) is the same as the passed one
tCIDLib::TBoolean
TPixelArray::bIsSameFormat( const   TPixelArray&        pixaToCheck
                            , const tCIDLib::TBoolean   bIncludeSz) const
{
    return m_pixaiInt->bIsSameFormat(*pixaToCheck.m_pixaiInt, bIncludeSz);
}


//
//  We must be a palette based image, and we use the passed palette to
//  convert ourself to our RGB equivalent.
//
tCIDLib::TBoolean
TPixelArray::bPalToRGB( const   TClrPalette&        palToUse
                        , const tCIDLib::TBoolean   bTransClr
                        , const tCIDLib::TCard4     c4TransClr
                        , const tCIDLib::TBoolean   bRepTrans
                        , const TRGBClr&            rgbRepClr)
{
    // If not palette based, nothing to do
    if (!tCIDLib::bAllBitsOn(m_pixaiInt->m_eFmt, tCIDImage::EPixFmts::Palette))
        return kCIDLib::False;

    return m_pixaiInt->bPalToRGB(palToUse, bTransClr, c4TransClr, bRepTrans, rgbRepClr);
}


//
//  Converts this array, in place, from RGB to RGBA, setting pixels transparent that
//  match a specific transparency color. It can also replace the color of the pixels
//  that it makes transparent.
//
//  This one must be RGB or it just returns false and does nothing.
//
tCIDLib::TBoolean
TPixelArray::bRGBToRGBA(const   tCIDLib::TCard4     c4TransClr
                        , const tCIDLib::TBoolean   bRepTrans
                        , const TRGBClr&            rgbRepClr)
{
    // If not RGB format
    if (m_pixaiInt->m_eFmt != tCIDImage::EPixFmts::TrueClr)
        return kCIDLib::False;

    return m_pixaiInt->bRGBToRGBA(c4TransClr, bRepTrans, rgbRepClr);
}


// Returns true if this pixel array is RGB based (with or without alpha)
tCIDLib::TBoolean TPixelArray::bIsRGBBased() const
{
    return
    (
        (m_pixaiInt->m_eFmt == tCIDImage::EPixFmts::TrueClr)
        || (m_pixaiInt->m_eFmt == tCIDImage::EPixFmts::TrueAlpha)
    );
}


//
//  Does a bicubic spline scale of this image to the passed pixel array,
//  using the target's size as the size to scale to. They pass a pallete
//  for us to use if we are a palette based image.
//
tCIDLib::TVoid
TPixelArray::BSplineScaleTo(        TPixelArray&        pixaTar
                            , const TClrPalette&        palToUse
                            , const tCIDLib::TCard4     c4Degree) const
{
    m_pixaiInt->BSplineScaleTo(*pixaTar.m_pixaiInt, palToUse, c4Degree);
}


// Access the raw pixel value at a given position
tCIDLib::TCard4 TPixelArray::c4At(  const   tCIDLib::TCard4 c4XPos
                                    , const tCIDLib::TCard4 c4YPos) const
{
    return m_pixaiInt->c4GetPixel(c4XPos, c4YPos, CID_LINE);
}

tCIDLib::TCard4 TPixelArray::c4At(const TQ1Point& pntPos)
{
    return m_pixaiInt->c4GetPixel(pntPos.c4X(), pntPos.c4Y(), CID_LINE);
}


// Return the bits per pixel for our format
tCIDLib::TCard4 TPixelArray::c4BitsPer() const
{
    return c4CalcBitsPerPixel(m_pixaiInt->m_eFmt, m_pixaiInt->m_eBitDepth);
}


// Return our image height
tCIDLib::TCard4 TPixelArray::c4Height() const
{
    return m_pixaiInt->m_c4Height;
}


// Return our image size (bytes required to store the whole image with padding)
tCIDLib::TCard4 TPixelArray::c4ImageSz() const
{
    return m_pixaiInt->m_c4ImageSz;
}


// Return the number of bytes per line (including padding)
tCIDLib::TCard4 TPixelArray::c4LineWidth() const
{
    return m_pixaiInt->m_c4LineWidth;
}


// Return our image width
tCIDLib::TCard4 TPixelArray::c4Width() const
{
    return m_pixaiInt->m_c4Width;
}


//
//  This method will find a color not used in the image. It is returned in
//  the native image format, i.e. index, gray scale value, or RGB color. If
//  an RGB color, no alpha is set.
//
tCIDLib::TCard4 TPixelArray::c4Unused(const TClrPalette& palToUse) const
{
    return m_pixaiInt->c4Unused(palToUse);
}


//
//  This method accepts a pixel array which must be either 8 bit gray scale
//  or true color in format, and the same size as us. We must be true color
//  or true alpha format. It will use the passed data to set our alpha channel.
//  It uses the areaSrc area from with the source array and applies that at
//  pntTar within us.
//
//  If the passed image is 8 bit gray scale, we take each byte as is for the
//  alpha channel value. If it's true color or true alpha, we'll average the
//  RGB bytes and use that.
//
//  It doesn't have to be the same size as us, and it can be offset from our
//  origin via pntTar. Any pixels not covered by the passed array size will be
//  set to fully transparent.
//
//  They can tell us to premultiply the pixels by the new alpha, or just to drop
//  the alpha values in. The latter is used to re-apply the alpha after
//  drawing GDI output over an in-memory image, so that it can then be
//  alpha blitted. GDI output clears the alpha channel.
//
tCIDLib::TVoid
TPixelArray::CreateAlphaChannelFrom(const   TPixelArray&        pixaAlpha
                                    , const TArea&              areaSrc
                                    , const TPoint&             pntTar
                                    , const tCIDLib::TBoolean   bPremultiply)
{
    // We have to be a true alpha based format
    if ((m_pixaiInt->m_eFmt != tCIDImage::EPixFmts::TrueAlpha)
    ||  (m_pixaiInt->m_eBitDepth != tCIDImage::EBitDepths::Eight))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPixa_WrongTarFormat
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(L"true alpha")
        );
    }

    //
    //  Calculate the real source area that we can actually use. We need to clip away
    //  any of the indicated source that would fall outside of the source pixel array.
    //  If empty, we are done.
    //
    TArea areaRealSrc(areaSrc);
    areaRealSrc &= TArea(0, 0, pixaAlpha.c4Width(), pixaAlpha.c4Height());
    if (areaRealSrc.bIsEmpty())
        return;

    //
    //  Now get the actual area of us that we can possibly update. It's the real
    //  source size shifted up to the target point within us, clipped against our
    //  area. Again, if empty, we are done.
    //
    const TArea areaUs(0, 0, m_pixaiInt->m_c4Width, m_pixaiInt->m_c4Height);
    TArea areaUpdate(areaRealSrc);
    areaUpdate.SetOrg(pntTar);
    areaUpdate &= areaUs;
    if (areaUpdate.bIsEmpty())
        return;

    //
    //  If the update area isn't equal to our area, then set all of the alpha to zero
    //  so that any we don't update will be zero.
    //
    if (areaUpdate != areaUpdate)
        SetAllAlpha(0);

    // Then apply the alpha to the area covered
    m_pixaiInt->ApplyAlphaChannel
    (
        *pixaAlpha.m_pixaiInt, areaSrc, areaUpdate, pntTar, bPremultiply, kCIDLib::False
    );
}


//
//  Converts this image to a gray scale image. If we are a palette based
//  image, then we just conver the palette to gray scale, which effectively
//  will make this image grays scale when rendered. Else we convert our
//  pixel data to gray scale. We don't convert RGB data format to gray scale
//  format. We just set each RGB value to the calculate gray scale equiv. So
//  there is no reason to reallocate the buffer ever.
//
tCIDLib::TVoid TPixelArray::CvtToGrayScale(TClrPalette& palToUse)
{
    // If it's already gray scale, then there's nothing to do
    if ((m_pixaiInt->m_eFmt == tCIDImage::EPixFmts::GrayScale)
    ||  (m_pixaiInt->m_eFmt == tCIDImage::EPixFmts::GrayAlpha))
    {
        return;
    }

    //
    //  If it's palette based, then we just convert the existing colors in
    //  the palette to their gray scale equiv and we are done.
    //
    if (tCIDLib::bAllBitsOn(m_pixaiInt->m_eFmt, tCIDImage::EPixFmts::Palette))
    {
        palToUse.CvtToGrayScale();
        return;
    }

    m_pixaiInt->CvtToGrayScale(palToUse);
}


// Return our bit depth
tCIDImage::EBitDepths TPixelArray::eBitDepth() const
{
    return m_pixaiInt->m_eBitDepth;
}


// REturn our pixel format
tCIDImage::EPixFmts TPixelArray::ePixFmt() const
{
    return m_pixaiInt->m_eFmt;
}


// Return our row order
tCIDImage::ERowOrders TPixelArray::eRowOrder() const
{
    return m_pixaiInt->m_eRowOrder;
}


//
//  We must be True-Alpha format. The caller's pixel array must be the same
//  size as us, and be 8 bit gray scale format. This one must be 8 bit
//  true color.
//
//  We will copy our alpha channel bytes into the caller's array.
//
tCIDLib::TVoid TPixelArray::ExtractAlphaChannel(TPixelArray& pixaAlpha) const
{
    // Make sure they are the same size
    if ((pixaAlpha.c4Width() != m_pixaiInt->m_c4Width)
    ||  (pixaAlpha.c4Height() != m_pixaiInt->m_c4Height))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPixa_DifferentSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , pixaAlpha.szImage()
            , TSize(m_pixaiInt->m_c4Width, m_pixaiInt->m_c4Height)
        );
    }

    // Make sure we are an 8 bit true alpha
    if ((m_pixaiInt->m_eFmt != tCIDImage::EPixFmts::TrueAlpha)
    ||  (m_pixaiInt->m_eBitDepth != tCIDImage::EBitDepths::Eight))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPixa_WrongSrcFormat
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(L"true alpha")
        );
    }

    // Make sure he's an 8 bit gray scale
    if ((pixaAlpha.ePixFmt() != tCIDImage::EPixFmts::GrayScale)
    ||  (pixaAlpha.eBitDepth() != tCIDImage::EBitDepths::Eight))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPixa_WrongTarFormat
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(L"8 bit grayscale")
        );
    }
    m_pixaiInt->ExtractAlphaChannel(*pixaAlpha.m_pixaiInt);
}


//
//  Flips the image around it's vertical axis (swaps left and right sides.)
//  by just mirroring the pixels in each colum of the pixel array. We have
//  a base version that takes start/end columns, and another that just calls
//  the base version with 0 and the last column.
//
tCIDLib::TVoid
TPixelArray::FlipHorizontally(  const   tCIDLib::TCard4 c4StartCol
                                , const tCIDLib::TCard4 c4EndCol)
{
    m_pixaiInt->FlipHorizontally(c4StartCol, c4EndCol);
}

tCIDLib::TVoid TPixelArray::FlipHorizontally()
{
    if (m_pixaiInt->m_c4Width)
        m_pixaiInt->FlipHorizontally(0, m_pixaiInt->m_c4Width - 1);
}


//
//  This method will flip the image data from top to bottom to bottom to top,
//  and vice versa. We don't really care what the current order is, we just
//  need to exchange the rows from top to bottom. So we call FlipVertically()
//  to flip the image, and then change the row order indicator.
//
tCIDLib::TVoid TPixelArray::FlipRowOrder()
{
    m_pixaiInt->FlipRowOrder();
}


tCIDLib::TVoid TPixelArray::ForceRowOrder(const tCIDImage::ERowOrders eRowOrder)
{
    // If it's the same as our order, then nothing to do
    if (eRowOrder == m_pixaiInt->m_eRowOrder)
        return;
    m_pixaiInt->ForceRowOrder(eRowOrder);
}


//
//  Flips the image around it's horizontal axis, i.e. swaps top to bottom.
//  We hvae a base version that takes start/end rows, and another that just
//  calls the base version with 0 and the last row.
//
tCIDLib::TVoid
TPixelArray::FlipVertically(const   tCIDLib::TCard4 c4StartRow
                            , const tCIDLib::TCard4 c4EndRow)
{
    m_pixaiInt->FlipVertically(c4StartRow, c4EndRow);
}

tCIDLib::TVoid TPixelArray::FlipVertically()
{
    if (m_pixaiInt->m_c4Height)
        m_pixaiInt->FlipVertically(0, m_pixaiInt->m_c4Height - 1);
}


//
//  Performs a gaussian blur, of the indicated order. This object's format
//  cannot be a palette based format, or less than 5 bits per component.
//
//  The passed pixel array is used as a temp. We do a horizontal pass into
//  the temp, then the veritical pass back into us. If one isn't provided we
//  create a temporary one, but often we are called by code that has one
//  available and it's much more efficient that way. It must be large enough
//  to handle the area of this array that we are blurring.
//
//  We have a second version that calls the other with the full size of
//  this pixel array.
//
tCIDLib::TVoid
TPixelArray::GaussianBlur(  const   tCIDLib::TCard4     c4Order
                            ,       TPixelArray* const  ppixaTmp)
{
    // Call the other version, not directly to the impl, to do checks!
    GaussianBlur(c4Order, m_pixaiInt->m_c4Width, m_pixaiInt->m_c4Height, ppixaTmp);
}

tCIDLib::TVoid
TPixelArray::GaussianBlur(  const   tCIDLib::TCard4     c4Order
                            , const tCIDLib::TCard4     c4MaxWidth
                            , const tCIDLib::TCard4     c4MaxHeight
                            ,       TPixelArray* const  ppixaTmp)
{
    //
    //  We don't deal with palette based formats in this one. THere is a
    //  separate one for palette stuff. Keeps things much more efficient.
    //
    if (tCIDLib::bAllBitsOn(m_pixaiInt->m_eFmt, tCIDImage::EPixFmts::Palette))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPixa_NoPalFormat
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // We reject anything that's less than 5 bits in depth
    if ((m_pixaiInt->m_eBitDepth == tCIDImage::EBitDepths::One)
    ||  (m_pixaiInt->m_eBitDepth == tCIDImage::EBitDepths::Two)
    ||  (m_pixaiInt->m_eBitDepth == tCIDImage::EBitDepths::Four))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPixa_BitDepthTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TInteger(5)
        );
    }

    //
    //  The passed pixel array must be of the same type, and at least the
    //  same size. If they didn't pass one, we don't need to do these checks
    //  since we'll create one that we know is right.
    //
    if (ppixaTmp)
    {
        if ((ppixaTmp->m_pixaiInt->m_eFmt != m_pixaiInt->m_eFmt)
        ||  (ppixaTmp->m_pixaiInt->m_eBitDepth != m_pixaiInt->m_eBitDepth))
        {
            facCIDImage().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kImgErrs::errcPixa_DifferentFmt
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }

        if ((ppixaTmp->m_pixaiInt->m_c4Width < c4MaxWidth)
        ||  (ppixaTmp->m_pixaiInt->m_c4Height < c4MaxHeight))
        {
            facCIDImage().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kImgErrs::errcPixa_TarTooSmall
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }
    }

    m_pixaiInt->GaussianBlur
    (
        c4Order, c4MaxWidth, c4MaxHeight, ppixaTmp ? ppixaTmp->m_pixaiInt : nullptr
    );
}


//
//  Inverts the alpha channel on this pixel array. So it just subtracts
//  each alpha byte from 0xFF.
//
tCIDLib::TVoid TPixelArray::InvertAlphaChannel()
{
    // We have to be a true alpha format
    if ((m_pixaiInt->m_eFmt != tCIDImage::EPixFmts::TrueAlpha)
    ||  (m_pixaiInt->m_eBitDepth != tCIDImage::EBitDepths::Eight))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPixa_WrongSrcFormat
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(L"true alpha")
        );
    }
    m_pixaiInt->InvertAlphaChannel();
}



//
//  We give the caller back a column of single color components as float8
//  values. The passed pf8ToLoad array obviously must be big enough to hold
//  a column's worth of this image's pixels.
//
//  This method, along with StoreCompRow, are primarily designed to support
//  bi-cubic spline scaling, which needs to pull out columns from a source
//  array during the building of the coefficient matrix and to store the
//  resulting scaled rows into the target array.
//
tCIDLib::TVoid
TPixelArray::LoadCompRow(       tCIDLib::TFloat8* const pf8ToLoad
                        , const tCIDLib::TCard4         c4Row
                        , const tCIDLib::EClrComps      eComp
                        , const TClrPalette&            palToUse) const
{
    #if CID_DEBUG_ON

    // Can't be the alpha component unless we have alpha
    if ((eComp == tCIDLib::EClrComps::Alpha)
    &&  !tCIDLib::bAllBitsOn(m_pixaiInt->m_eFmt, tCIDImage::EPixFmts::Alpha))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcImg_NoAlpha
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    // Can't be the gray component unless we are are gray scale
    if ((eComp == tCIDLib::EClrComps::Gray)
    &&  tCIDLib::bAllBitsOn(m_pixaiInt->m_eFmt, tCIDImage::EPixFmts::Color))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcImg_NotGrayScale
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    m_pixaiInt->LoadCompRow(pf8ToLoad, c4Row, eComp, palToUse);
}


tCIDLib::TVoid TPixelArray::MakeMask(       TPixelArray&        pixaToFill
                                    , const tCIDLib::TCard4     c4ClrVal
                                    , const tCIDLib::TBoolean   bInvert) const
{
    m_pixaiInt->MakeMask(*pixaToFill.m_pixaiInt, c4ClrVal, bInvert);
}


//
//  Creates a thumbnail version of this pixel array data into the passed
//  pixel array, taking into account format, alpha, transparency info, and
//  so forth.
//
tCIDLib::TVoid
TPixelArray::MakeScaled(        TPixelArray&        pixaThumb
                        , const TClrPalette&        palClrs
                        , const tCIDLib::TBoolean   bTransClr
                        , const tCIDLib::TCard4     c4TransClr
                        , const tCIDLib::TCard4     c4Degree) const
{
    m_pixaiInt->MakeScaled(*pixaThumb.m_pixaiInt, palClrs, bTransClr, c4TransClr, c4Degree);
}


const tCIDLib::TCard1* TPixelArray::pc1Buffer() const
{
    return m_pixaiInt->m_pc1Pixels;
}

tCIDLib::TCard1* TPixelArray::pc1Buffer()
{
    return m_pixaiInt->m_pc1Pixels;
}


const tCIDLib::TCard1* TPixelArray::pc1RowPtr(const tCIDLib::TCard4 c4Row) const
{
    if (c4Row >= m_pixaiInt->m_c4Height)
    {
        // NOTE: We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Row)
            , clsThis()
            , TCardinal(m_pixaiInt->m_c4Height)
        );
    }
    return m_pixaiInt->pc1RowPtr(c4Row);
}

tCIDLib::TCard1* TPixelArray::pc1RowPtr(const tCIDLib::TCard4 c4Row)
{
    // Call the other version, not the impl, for safety checks
    return const_cast<tCIDLib::TCard1*>(const_cast<const TPixelArray*>(this)->pc1RowPtr(c4Row));
}


//
//  Assuming stuff has been drawn into this pixel array, go back and do a
//  pre-multiply pass on us. We must be true alpha
//
tCIDLib::TVoid TPixelArray::Premultiply()
{
    // We have to be a true alpha based format
    if ((m_pixaiInt->m_eFmt != tCIDImage::EPixFmts::TrueAlpha)
    ||  (m_pixaiInt->m_eBitDepth != tCIDImage::EBitDepths::Eight))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPixa_WrongTarFormat
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(L"true alpha")
        );
    }
    return m_pixaiInt->Premultiply();
}



// Put pixel values into the array at specific points
tCIDLib::TVoid TPixelArray::PutAt(  const   tCIDLib::TCard4 c4ToPut
                                    , const tCIDLib::TCard4 c4XPos
                                    , const tCIDLib::TCard4 c4YPos)
{
    m_pixaiInt->PutPixel(c4ToPut, c4XPos, c4YPos, CID_LINE);
}

tCIDLib::TVoid TPixelArray::PutAt(  const   tCIDLib::TCard4 c4ToPut
                                    , const TQ1Point&       pntPos)
{
    m_pixaiInt->PutPixel(c4ToPut, pntPos.c4X(), pntPos.c4Y(), CID_LINE);
}


// Get the RGB value of a pixel at a particular point
tCIDLib::TVoid
TPixelArray::QueryRGBAt(const   TPoint&         pntAt
                        , const TClrPalette&    palToUse
                        ,       TRGBClr&        rgbToFill) const
{
    m_pixaiInt->QueryRGBAt(pntAt, palToUse, rgbToFill);
}


tCIDLib::TVoid
TPixelArray::ReplaceClr(const   tCIDLib::TCard4 c4ToRep
                        , const tCIDLib::TCard4 c4RepClr)
{
    m_pixaiInt->ReplaceClr(c4ToRep, c4RepClr);
}


//
//  These methods all reset our size or format, and deal with reallocating
//  the buffer if required. This first one does the real work, and the
//  others just call this first one.
//
tCIDLib::TVoid
TPixelArray::Reset( const   tCIDImage::EPixFmts     eFmt
                    , const tCIDImage::EBitDepths   eBitDepth
                    , const tCIDImage::ERowOrders   eRowOrder
                    , const TSize&                  szImage
                    , const tCIDLib::TBoolean       bForceRealloc)
{
    m_pixaiInt->Reset(eFmt, eBitDepth, eRowOrder, szImage, bForceRealloc);
}

tCIDLib::TVoid
TPixelArray::Reset( const   tCIDImage::EPixFmts     eFmt
                    , const tCIDImage::EBitDepths   eBitDepth
                    , const tCIDLib::TBoolean       bForceRealloc)
{
    m_pixaiInt->Reset
    (
        eFmt
        , eBitDepth
        , m_pixaiInt->m_eRowOrder
        , TSize(m_pixaiInt->m_c4Width, m_pixaiInt->m_c4Height)
        , bForceRealloc
    );
}

tCIDLib::TVoid TPixelArray::Reset(  const   TCIDImage&          imgSource
                                    , const tCIDLib::TBoolean   bForceRealloc)
{
    m_pixaiInt->Reset
    (
        imgSource.ePixFmt()
        , imgSource.eBitDepth()
        , imgSource.eRowOrder()
        , imgSource.szImage()
        , bForceRealloc
    );
}

tCIDLib::TVoid TPixelArray::Reset(  const   TSize&              szNew
                                    , const tCIDLib::TBoolean   bForceRealloc)
{
    m_pixaiInt->Reset
    (
        m_pixaiInt->m_eFmt
        , m_pixaiInt->m_eBitDepth
        , m_pixaiInt->m_eRowOrder
        , szNew
        , bForceRealloc
    );
}


// Return our size as a TSize object
TSize TPixelArray::szImage() const
{
    return TSize(m_pixaiInt->m_c4Width, m_pixaiInt->m_c4Height);
}


//
//  Does a scaling of the alpha in the indicated direction from the starting
//  row/col to the ending row/col. The starting row/col is 100% scaled, i.e.
//  unchanged, and by the end row/col, it is scaled to zero, so fully
//  transparent. It's done in a non-linear way, so it ramps down quickly at
//  first, and then slower.
//
tCIDLib::TVoid
TPixelArray::ScaleAlpha(        tCIDLib::EDirs  eDir
                        , const tCIDLib::TCard4 c4StartInd
                        , const tCIDLib::TCard4 c4EndInd)
{
    // If the start/end are reversed, flip them
    tCIDLib::TCard4 c4Low;
    tCIDLib::TCard4 c4High;
    if (c4StartInd > c4EndInd)
    {
        c4High = c4StartInd;
        c4Low = c4EndInd;
    }
     else
    {
        c4Low = c4StartInd;
        c4High = c4EndInd;
    }

    // If this is not an alpha format, or there are less than 2 lines, do nothing
    if (!tCIDLib::bAllBitsOn(m_pixaiInt->m_eFmt, tCIDImage::EPixFmts::Alpha)
    ||  ((c4High - c4Low) < 2))
    {
        return;
    }

    m_pixaiInt->ScaleAlpha(eDir, c4Low, c4High);
}


tCIDLib::TVoid TPixelArray::SetAll(const tCIDLib::TCard4 c4ToSet)
{
    m_pixaiInt->SetAll(c4ToSet);
}


//
//  Sets all alpha channel bytes to a specific opacity, which is a very commonly
//  desired operation.
//
tCIDLib::TVoid TPixelArray::SetAllAlpha(const tCIDLib::TCard4 c4Alpha)
{
    // If this is not an alpha format, then do nothing
    if (!tCIDLib::bAllBitsOn(m_pixaiInt->m_eFmt, tCIDImage::EPixFmts::Alpha))
        return;

    m_pixaiInt->SetAllAlpha(c4Alpha);
}


tCIDLib::TVoid
TPixelArray::SetAlphaAt(const   tCIDLib::TCard4 c4Alpha
                        , const TArea&          areaTar)
{
    // If this is not an alpha format, then do nothing
    if (!tCIDLib::bAllBitsOn(m_pixaiInt->m_eFmt, tCIDImage::EPixFmts::Alpha))
        return;

    // Figure out the intersection area, if none, we are done
    TArea areaDo(0, 0, m_pixaiInt->m_c4Width, m_pixaiInt->m_c4Height);
    if (!areaDo.bIntersects(areaTar))
        return;

    // Get the overlapping area. This is what we'll do
    areaDo &= areaTar;
    m_pixaiInt->SetAlphaAt(c4Alpha, areaDo);
}



//
//  Sets the alpha channel using the color/gray scale. If gray scale, the
//  alpha is just a copy of it. If it is color, then the alpha is the three
//  color channels combined and divided by 3. We obviously only support
//  formats with alpha here.
//
//  And we have another version that will go through the array and set the
//  alpha to transparent for any pixelst that match a passed transaprency
//  color.
//
tCIDLib::TVoid TPixelArray::SetAlphaFromColor()
{
    // We have to be an alpha oriented format
    if (!tCIDLib::bAllBitsOn(m_pixaiInt->m_eFmt, tCIDImage::EPixFmts::Alpha))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcImg_NoAlpha
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }
    m_pixaiInt->SetAlphaFromColor();
}


tCIDLib::TVoid TPixelArray::SetAlphaFromColor(const tCIDLib::TCard4 c4TransClr)
{
    // We have to be an 8-bit, alpha oriented format
    if ((m_pixaiInt->m_eFmt != tCIDImage::EPixFmts::TrueAlpha)
    ||  (m_pixaiInt->m_eBitDepth != tCIDImage::EBitDepths::Eight))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPixa_WrongSrcFormat
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(L"true alpha")
        );
    }

    m_pixaiInt->SetAlphaFromColor(c4TransClr);
}



//
//  Resets the array to have all pixels be zero and all alpha if pressent
//  to be fully transparent. This is needed if we are going to draw onto a
//  bitmap and we want any pixels we didn't draw into to be transparent. Since
//  we are setting everything to zero, we can just do a single memory block
//  set on the whole buffer.
//
//  Note that we can a Card4 based fill for maximum efficiency, as long as
//  the (almost always) buffer line rounding is 4 bytes. This will ensure
//  that the whole thing must be a multiple of 4 bytes.
//
tCIDLib::TVoid TPixelArray::SetEmptyTrans()
{
    m_pixaiInt->SetEmptyTrans();
}



tCIDLib::TVoid
TPixelArray::SetTextAlpha(  const   tCIDLib::TCard4     c4TextClr
                            , const tCIDLib::TCard4     c4TransClr
                            , const tCIDLib::TBoolean   bPremul)
{
    // We have to be an 8-bit, alpha oriented format
    if ((m_pixaiInt->m_eFmt != tCIDImage::EPixFmts::TrueAlpha)
    ||  (m_pixaiInt->m_eBitDepth != tCIDImage::EBitDepths::Eight))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPixa_WrongSrcFormat
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(L"true alpha")
        );
    }
    m_pixaiInt->SetTextAlpha(c4TextClr, c4TransClr, bPremul);
}


//
//  The caller passes us a row of values for a single color component. The
//  max size a single component can be is 16 bits (for 16 bit gray scale or
//  the same with alpha), so we get an array of Card2 values.
//
//  This method, along with LoadCompCol, are primarily designed to support
//  bi-cubic spline scaling, which needs to pull out columns from a source
//  array during the building of the coefficient matrix and to store the
//  resulting scaled rows into the target array.
//
tCIDLib::TVoid
TPixelArray::StoreCompRow(          tCIDLib::TCard2* const  pc2ToStore
                            , const tCIDLib::TCard4         c4Row
                            , const tCIDLib::EClrComps      eComp)
{
    #if CID_DEBUG_ON

    // We cannot be palette based
    if (tCIDLib::bAllBitsOn(m_pixaiInt->m_eFmt, tCIDImage::EPixFmts::Palette))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcImg_PalNotSupported
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    // Can't be the alpha component unless we have alpha
    if ((eComp == tCIDLib::EClrComps::Alpha)
    &&  !tCIDLib::bAllBitsOn(m_pixaiInt->m_eFmt, tCIDImage::EPixFmts::Alpha))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcImg_NoAlpha
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    // Can't be the gray component unless we are are gray scale
    if ((eComp == tCIDLib::EClrComps::Gray)
    &&  tCIDLib::bAllBitsOn(m_pixaiInt->m_eFmt, tCIDImage::EPixFmts::Color))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcImg_NotGrayScale
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    m_pixaiInt->StoreCompRow(pc2ToStore, c4Row, eComp);
}


tCIDLib::TVoid TPixelArray::ZeroAll()
{
    m_pixaiInt->ZeroAll();
}


// ---------------------------------------------------------------------------
//  TPixelArray: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TPixelArray::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom >> *m_pixaiInt;
}


tCIDLib::TVoid TPixelArray::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo << *m_pixaiInt;
}


// ---------------------------------------------------------------------------
//  TPixelArray: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  A special pixel getter that deals with access beyond the array. It's used
//  in filters that use the pixels in an array around the target pixel, and on
//  edges some of those are off the screen.
//
tCIDLib::TCard4
TPixelArray::c4GetBoundaryPixel(const   tCIDLib::TInt4  i4XPos
                                , const tCIDLib::TInt4  i4YPos
                                , const tCIDLib::TCard4 c4Line) const
{
    return m_pixaiInt->c4GetBoundaryPixel(i4XPos, i4YPos, c4Line);
}


//
//  Gets a pixel out at that indicated coordinates. It goes by the current
//  pixel format data to know how to pull out the data. No matter what the
//  format, it's returned in a TCard4 value.
//
tCIDLib::TCard4
TPixelArray::c4GetPixel(const   tCIDLib::TCard4 c4XPos
                        , const tCIDLib::TCard4 c4YPos
                        , const tCIDLib::TCard4 c4Line) const
{
    return m_pixaiInt->c4GetPixel(c4XPos, c4YPos, c4Line);
}


