//
// FILE NAME: CIDImage_PixelArrayImpl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/07/2019
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
//  This file implements the internal base TPixelArrayImpl class.
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
//  Local data
// ---------------------------------------------------------------------------
namespace CIDImage_PixelArrayImpl
{
    namespace
    {
        // -----------------------------------------------------------------------
        //  Our persistent format version
        //
        //  1.  Original version
        //  2.  Moved to the pixelfmt/depth combination to describe the data format
        //      and added new formats to cover the full range of PNG images.
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard2   c2FmtVersion = 2;
    }
}


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TPixelArrayImpl, TObject)



// ---------------------------------------------------------------------------
//  CLASS: TPixelArray
// PREFIX: pixai
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPixelArray: Constructors and Destructor
// ---------------------------------------------------------------------------

// We just create a 1x1 1 B&W image for default ctor
TPixelArrayImpl::TPixelArrayImpl() :

    m_c4BufSz(0)
    , m_c4Height(1)
    , m_c4ImageSz(0)
    , m_c4LineBytePadding(kCIDImage::c4LinePadding)
    , m_c4LineWidth(0)
    , m_c4Width(1)
    , m_eAdoptOpt(tCIDLib::EAdoptOpts::Adopt)
    , m_eBitDepth(tCIDImage::EBitDepths::One)
    , m_eFmt(tCIDImage::EPixFmts::GrayScale)
    , m_eRowOrder(tCIDImage::ERowOrders::TopDown)
    , m_pc1Pixels(nullptr)
{
    // Calculate the transient members
    CalcTransients();

    //
    //  And allocate and zero out our new buffer. In this case, we'll make
    //  it just big enough for the image data.
    //
    m_c4BufSz = m_c4ImageSz;
    m_pc1Pixels = new tCIDLib::TCard1[m_c4BufSz];
    TRawMem::SetMemBuf(m_pc1Pixels, tCIDLib::TCard1(0), m_c4BufSz);
}

TPixelArrayImpl::TPixelArrayImpl(const  tCIDImage::EPixFmts     eFmt
                                , const tCIDImage::EBitDepths   eBitDepth
                                , const tCIDImage::ERowOrders   eRowOrder
                                , const TSize&                  szImage
                                ,       tCIDLib::TCard1* const  pc1Buf
                                , const tCIDLib::TCard4         c4BufSz
                                , const tCIDLib::EAdoptOpts     eAdoptOpt) :

    m_c4BufSz(c4BufSz)
    , m_c4LineBytePadding(kCIDImage::c4LinePadding)
    , m_c4Height(szImage.c4Height())
    , m_c4Width(szImage.c4Width())
    , m_eAdoptOpt(eAdoptOpt)
    , m_eBitDepth(eBitDepth)
    , m_eFmt(eFmt)
    , m_eRowOrder(eRowOrder)
    , m_pc1Pixels(nullptr)
{
    // Validate input and calculate the transient members
    CalcTransients();

    //
    //  Now make sure that the passed buffer is large enough. It's fine if
    //  it is too large, but can't be too small.
    //
    if (c4BufSz < m_c4ImageSz)
    {
        // If we are adopting, delete before we throw
        if (eAdoptOpt == tCIDLib::EAdoptOpts::Adopt)
            delete [] pc1Buf;

        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPixa_BufTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(c4BufSz)
            , TCardinal(m_c4ImageSz)
        );
    }

    //
    //  Looks ok, so store the pointer. Note we don't zero the memory here
    //  since it could already have content.
    //
    m_pc1Pixels = pc1Buf;
}


TPixelArrayImpl::TPixelArrayImpl(const TCIDImage& imgSource) :

    m_c4BufSz(0)
    , m_c4LineBytePadding(kCIDImage::c4LinePadding)
    , m_c4Height(imgSource.szImage().c4Height())
    , m_c4ImageSz(0)
    , m_c4Width(imgSource.szImage().c4Width())
    , m_eAdoptOpt(tCIDLib::EAdoptOpts::Adopt)
    , m_eBitDepth(imgSource.eBitDepth())
    , m_eFmt(imgSource.ePixFmt())
    , m_eRowOrder(imgSource.eRowOrder())
    , m_pc1Pixels(nullptr)
{
    // Validate input and calculate the transient members
    CalcTransients();

    // And allocate and zero out our new buffer
    m_c4BufSz = m_c4ImageSz;
    m_pc1Pixels = new tCIDLib::TCard1[m_c4BufSz];
    TRawMem::SetMemBuf(m_pc1Pixels, tCIDLib::TCard1(0), m_c4BufSz);
}

TPixelArrayImpl::TPixelArrayImpl(const  tCIDImage::EPixFmts     eFmt
                                , const tCIDImage::EBitDepths   eBitDepth
                                , const tCIDImage::ERowOrders   eRowOrder
                                , const  TSize&                 szImage) :

    m_c4BufSz(0)
    , m_c4LineBytePadding(kCIDImage::c4LinePadding)
    , m_c4Height(szImage.c4Height())
    , m_c4Width(szImage.c4Width())
    , m_eAdoptOpt(tCIDLib::EAdoptOpts::Adopt)
    , m_eBitDepth(eBitDepth)
    , m_eFmt(eFmt)
    , m_eRowOrder(eRowOrder)
    , m_pc1Pixels(nullptr)
{
    // Validate input and calculate the transient members
    CalcTransients();

    // And allocate and zero out our new buffer
    m_c4BufSz = m_c4ImageSz;
    m_pc1Pixels = new tCIDLib::TCard1[m_c4BufSz];
    TRawMem::SetMemBuf(m_pc1Pixels, tCIDLib::TCard1(0), m_c4BufSz);
}

TPixelArrayImpl::TPixelArrayImpl(const TPixelArrayImpl& pixaiSrc) :

    m_c4BufSz(pixaiSrc.m_c4BufSz)
    , m_c4Height(pixaiSrc.m_c4Height)
    , m_c4ImageSz(pixaiSrc.m_c4ImageSz)
    , m_c4LineBytePadding(kCIDImage::c4LinePadding)
    , m_c4LineWidth(pixaiSrc.m_c4LineWidth)
    , m_c4Width(pixaiSrc.m_c4Width)
    , m_eAdoptOpt(tCIDLib::EAdoptOpts::Adopt)
    , m_eBitDepth(pixaiSrc.m_eBitDepth)
    , m_eFmt(pixaiSrc.m_eFmt)
    , m_eRowOrder(pixaiSrc.m_eRowOrder)
    , m_pc1Pixels(nullptr)
{
    // Allocate our buffer and copy the source
    m_pc1Pixels = new tCIDLib::TCard1[m_c4BufSz];
    TRawMem::CopyMemBuf(m_pc1Pixels, pixaiSrc.m_pc1Pixels, m_c4BufSz);
}

TPixelArrayImpl::~TPixelArrayImpl()
{
    // Delete the buffer if we are adopting
    if (m_eAdoptOpt == tCIDLib::EAdoptOpts::Adopt)
        delete [] m_pc1Pixels;
    m_pc1Pixels = nullptr;
}


// ---------------------------------------------------------------------------
//  TPixelArray: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TPixelArrayImpl::ApplyAlphaChannel( const   TPixelArrayImpl&    pixaiAlpha
                                    , const TArea&              areaSrc
                                    , const TArea&              areaUpdate
                                    , const TPoint&             pntTar
                                    , const tCIDLib::TBoolean   bPremultiply
                                    , const tCIDLib::TBoolean   bForceBW)
{
    //
    //  We have to figure out some numbers now. For the vert and horz
    //  axes, we have to figure out how many zeroed out pixels we'll do
    //  before we hit the part covered by passed alpha channel array,
    //  how many pixels we'll actually process from the array, and how
    //  many trailing pixels we'll zero out.
    //
    //  This allows us to do very efficient countdown loops and REP
    //  type vector fills below instead of always checking whehter we've
    //  gone beyond this or that index.
    //
    //  Calculate the bytes we skip forward on each row, to get up to
    //  the pixels we care about.
    //
    tCIDLib::TCard4 c4SrcHSkip = tCIDLib::TCard4(areaSrc.i4X());
    if (pixaiAlpha.m_eFmt == tCIDImage::EPixFmts::TrueAlpha)
    {
        c4SrcHSkip *= 4;
    }
     else if (pixaiAlpha.m_eFmt == tCIDImage::EPixFmts::TrueClr)
    {
        c4SrcHSkip *= 3;
    }
     else if (pixaiAlpha.m_eFmt == tCIDImage::EPixFmts::GrayAlpha)
    {
        if (pixaiAlpha.m_eBitDepth == tCIDImage::EBitDepths::Eight)
            c4SrcHSkip *= 2;
        else
            c4SrcHSkip *= 4;
    }

    const tCIDLib::TCard4 c4TarHSkip = tCIDLib::TCard4(areaUpdate.i4X()) * 4;

    // Get the bytes per row of both into locals for easier access below
    const tCIDLib::TCard4 c4SPerRow = pixaiAlpha.m_c4LineWidth;
    const tCIDLib::TCard4 c4TPerRow = m_c4LineWidth;

    //
    //  Get raw pointers to both pixel buffers, but start them off at the
    //  first row/col we are going to actually do. Then we can move forward
    //  a padded row at a time on each outter loop to get to the starting r/c
    //  for the next row.
    //
    const tCIDLib::TCard1* pc1Src
    (
        pixaiAlpha.pc1Buffer() + ((c4SPerRow * areaSrc.i4Y()) + c4SrcHSkip)
    );

    tCIDLib::TCard1* pc1Dest
    (
        m_pc1Pixels + ((c4TPerRow * areaUpdate.i4Y()) + c4TarHSkip)
    );

    // And the count of rows and colums we will do
    const tCIDLib::TCard4 c4Cols = areaUpdate.c4Width();
    const tCIDLib::TCard4 c4Rows = areaUpdate.c4Height();

    if ((pixaiAlpha.m_eFmt == tCIDImage::EPixFmts::GrayScale)
    &&  (pixaiAlpha.m_eBitDepth == tCIDImage::EBitDepths::Eight))
    {
        if (!bForceBW && !bPremultiply)
        {
            // If neither of the special cases, we can do a particularly tight loop here
            pc1Dest += 3;
            for (tCIDLib::TCard4 c4RowInd = 0; c4RowInd < c4Rows; c4RowInd++)
            {
                const tCIDLib::TCard1* pc1CurSrc = pc1Src;
                tCIDLib::TCard1* pc1CurDest = pc1Dest;
                for (tCIDLib::TCard4 c4ColInd = 0; c4ColInd < c4Cols; c4ColInd++)
                {
                    *pc1CurDest = *pc1CurSrc;
                    pc1CurDest += 4;
                    pc1CurSrc++;
                }

                pc1Src += c4SPerRow;
                pc1Dest += c4TPerRow;
            }
        }
         else if (bForceBW && !bPremultiply)
        {
            // If just forcing B&W, that's almost as efficient
            pc1Dest += 3;
            for (tCIDLib::TCard4 c4RowInd = 0; c4RowInd < c4Rows; c4RowInd++)
            {
                const tCIDLib::TCard1* pc1CurSrc = pc1Src;
                tCIDLib::TCard1* pc1CurDest = pc1Dest;

                // Move the destination up to the alpha byte
                for (tCIDLib::TCard4 c4ColInd = 0; c4ColInd < c4Cols; c4ColInd++)
                {
                    tCIDLib::TCard1 c1Alpha = *pc1CurSrc;
                    if (c1Alpha)
                        c1Alpha = 0xFF;
                    *pc1CurDest = c1Alpha;
                    pc1CurDest += 4;
                    pc1CurSrc++;
                }

                pc1Src += c4SPerRow;
                pc1Dest += c4TPerRow;
            }
        }
         else
        {
            // It's the worst case with possibly both optional mods
            for (tCIDLib::TCard4 c4RowInd = 0; c4RowInd < c4Rows; c4RowInd++)
            {
                const tCIDLib::TCard1* pc1CurSrc = pc1Src;
                tCIDLib::TCard1* pc1CurDest = pc1Dest;

                for (tCIDLib::TCard4 c4ColInd = 0; c4ColInd < c4Cols; c4ColInd++)
                {
                    // Get the next available src alpha byte and B&W if needed
                    tCIDLib::TCard1 c1Alpha = *pc1CurSrc++;
                    if (bForceBW && c1Alpha)
                        c1Alpha = 0xFF;

                    //
                    //  If the alpha is FF, no need to pre-multiply it won't change anything
                    //  and if zero the result is always zero.
                    //
                    if (bPremultiply)
                    {
                        if (c1Alpha == 0)
                        {
                            *pc1CurDest++ = 0;
                            *pc1CurDest++ = 0;
                            *pc1CurDest++ = 0;
                        }
                         else if (c1Alpha != 0xFF)
                        {
                            tCIDLib::TCard2 c2Val = *pc1CurDest;
                            c2Val *= c1Alpha;
                            c2Val /= 0xFF;
                            *pc1CurDest++ = tCIDLib::TCard1(c2Val);

                            c2Val = *pc1CurDest;
                            c2Val *= c1Alpha;
                            c2Val /= 0xFF;
                            *pc1CurDest++ = tCIDLib::TCard1(c2Val);

                            c2Val = *pc1CurDest;
                            c2Val *= c1Alpha;
                            c2Val /= 0xFF;
                            *pc1CurDest++ = tCIDLib::TCard1(c2Val);
                        }
                         else
                        {
                            pc1CurDest += 3;
                        }
                    }
                     else
                    {
                        pc1CurDest += 3;
                    }

                    // Now we can store the alpha and move dest to the next pixel
                    *pc1CurDest++ = c1Alpha;
                }

                pc1Src += c4SPerRow;
                pc1Dest += c4TPerRow;
            }
        }
    }
     else if ((pixaiAlpha.m_eBitDepth == tCIDImage::EBitDepths::Eight)
          &&  ((pixaiAlpha.m_eFmt == tCIDImage::EPixFmts::TrueAlpha)
          ||   (pixaiAlpha.m_eFmt == tCIDImage::EPixFmts::TrueClr)))
    {
        // The number of bytes we move forward for each source pixel
        tCIDLib::TCard4 c4ExtraSrc = 0;
        if (pixaiAlpha.m_eFmt == tCIDImage::EPixFmts::TrueAlpha)
            c4ExtraSrc++;

        if (bPremultiply)
        {
            for (tCIDLib::TCard4 c4RowInd = 0; c4RowInd < c4Rows; c4RowInd++)
            {
                const tCIDLib::TCard1* pc1CurSrc = pc1Src;
                tCIDLib::TCard1* pc1CurDest = pc1Dest;

                // We have to average the source pixels to create an alpha
                for (tCIDLib::TCard4 c4ColInd = 0; c4ColInd < c4Cols; c4ColInd++)
                {
                    tCIDLib::TCard4 c4CurPix = *pc1CurSrc++;
                    c4CurPix += *pc1CurSrc++;
                    c4CurPix += *pc1CurSrc++;
                    c4CurPix /= 3;
                    tCIDLib::TCard1 c1Alpha = tCIDLib::TCard1(c4CurPix);

                    if (c1Alpha && bForceBW)
                        c1Alpha = 0xFF;

                    if (bPremultiply)
                    {
                        if (c1Alpha == 0)
                        {
                            *pc1CurDest++ = 0;
                            *pc1CurDest++ = 0;
                            *pc1CurDest++ = 0;
                        }
                         else if (c1Alpha != 0xFF)
                        {
                            tCIDLib::TCard2 c2Val = *pc1CurDest;
                            c2Val *= c1Alpha;
                            c2Val /= 0xFF;
                            *pc1CurDest++ = tCIDLib::TCard1(c2Val);

                            c2Val = *pc1CurDest;
                            c2Val *= c1Alpha;
                            c2Val /= 0xFF;
                            *pc1CurDest++ = tCIDLib::TCard1(c2Val);

                            c2Val = *pc1CurDest;
                            c2Val *= c1Alpha;
                            c2Val /= 0xFF;
                            *pc1CurDest++ = tCIDLib::TCard1(c2Val);
                        }
                         else
                        {
                            pc1CurDest += 3;
                        }
                    }
                     else
                    {
                        pc1CurDest += 3;
                    }

                    // Now we can store the alpha and move to the next pixels
                    *pc1CurDest++ = c1Alpha;
                    pc1CurSrc += c4ExtraSrc;
                }

                pc1Src += c4SPerRow;
                pc1Dest += c4TPerRow;
            }
        }
         else
        {
            // We can optimize reasonably well if not pre-multiplying
            pc1Dest += 3;
            for (tCIDLib::TCard4 c4RowInd = 0; c4RowInd < c4Rows; c4RowInd++)
            {
                const tCIDLib::TCard1* pc1CurSrc = pc1Src;
                tCIDLib::TCard1* pc1CurDest = pc1Dest;

                for (tCIDLib::TCard4 c4ColInd = 0; c4ColInd < c4Cols; c4ColInd++)
                {
                    tCIDLib::TCard4 c4CurPix = *pc1CurDest;
                    c4CurPix += *pc1CurDest++;
                    c4CurPix += *pc1CurDest++;
                    c4CurPix /= 3;
                    tCIDLib::TCard1 c1Alpha = tCIDLib::TCard1(c4CurPix);

                    if (c1Alpha && bForceBW)
                        c1Alpha = 0xFF;

                    // Now we can store the alpha and move to the next pixels
                    *pc1CurDest = c1Alpha;

                    pc1CurDest += 4;
                    pc1CurSrc += c4ExtraSrc;
                }

                pc1Src += c4SPerRow;
                pc1Dest += c4TPerRow;
            }
        }
    }
     else
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcPixa_WrongSrcFormat
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TString(L"gray scale(8) or true color/alpha")
        );
    }
}


// Makes sure that our format (and optionally size) is the same as the passed one
tCIDLib::TBoolean
TPixelArrayImpl::bIsSameFormat( const   TPixelArrayImpl&    pixaiToCheck
                                , const tCIDLib::TBoolean   bIncludeSz) const
{
    if ((m_eFmt != pixaiToCheck.m_eFmt)
    ||  (m_eBitDepth != pixaiToCheck.m_eBitDepth)
    ||  (m_eRowOrder != pixaiToCheck.m_eRowOrder))
    {
        return kCIDLib::False;
    }

    // If the image size is included then return that result
    if (bIncludeSz)
    {
        return
        (
            (m_c4Width == pixaiToCheck.m_c4Width)
            && (m_c4Height == pixaiToCheck.m_c4Height)
        );
    }

    // Else it's obviously the same format
    return kCIDLib::True;
}


tCIDLib::TBoolean
TPixelArrayImpl::bPalToRGB( const   TClrPalette&        palToUse
                        , const tCIDLib::TBoolean   bTransClr
                        , const tCIDLib::TCard4     c4TransClr
                        , const tCIDLib::TBoolean   bRepTrans
                        , const TRGBClr&            rgbRepClr)
{
    //
    //  We are palette based, so do it. We create a new pixel buffer of the
    //  new size and format, then go through and update it with the RGB
    //  (or RBBA if trans color) equiv of our current data.
    //
    const tCIDImage::EPixFmts eNewFmt = bTransClr ? tCIDImage::EPixFmts::TrueAlpha
                                        : tCIDImage::EPixFmts::TrueClr;
    const tCIDImage::EBitDepths eNewDepth = tCIDImage::EBitDepths::Eight;
    const tCIDLib::TCard4 c4NewWidth = TPixelArray::c4CalcLineWidth
    (
        m_c4Width
        , eNewFmt
        , eNewDepth
        , m_c4LineBytePadding
    );

    tCIDLib::TCard1* pc1NewBuf = new tCIDLib::TCard1[c4NewWidth * m_c4Height];
    TArrayJanitor<tCIDLib::TCard1> janBuf(pc1NewBuf);

    for (tCIDLib::TCard4 c4YInd = 0; c4YInd < m_c4Height; c4YInd++)
    {
        // Because of padding we have to re-calc this for every line
        tCIDLib::TCard1* pc1Out = pc1NewBuf + (c4NewWidth * c4YInd);

        for (tCIDLib::TCard4 c4XInd = 0; c4XInd < m_c4Width; c4XInd++)
        {
            //
            //  Get the palette index for the current pixel, and get out the
            //  RGB value for that index.
            //
            const tCIDLib::TCard4 c4Cur = c4At(c4YInd, c4XInd);
            const TRGBClr& rgbCur = palToUse[c4Cur];

            //
            //  Put out the color values. If we are replacing the transparency
            //  color and this one matches it, then put out the replacement
            //  color. Else output the palette color.
            //
            if (bRepTrans && bTransClr && (rgbRepClr == rgbCur))
            {
                *pc1Out++ = rgbRepClr.c1Red();
                *pc1Out++ = rgbRepClr.c1Green();
                *pc1Out++ = rgbRepClr.c1Blue();
            }
             else
            {
                *pc1Out++ = rgbCur.c1Red();
                *pc1Out++ = rgbCur.c1Green();
                *pc1Out++ = rgbCur.c1Blue();
            }

            // If doing alpha, set it to fully opaque or fully transparent
            if (bTransClr)
            {
                if (c4TransClr == c4Cur)
                    *pc1Out++ = 0;
                else
                    *pc1Out++ = 0xFF;
            }
        }
    }

    // Ok, let's swap over to the new stuff now. Delete current buffer if adopting
    if (m_eAdoptOpt == tCIDLib::EAdoptOpts::Adopt)
    {
        delete [] m_pc1Pixels;
        m_pc1Pixels = nullptr;
    }

    m_eFmt = eNewFmt;
    m_eBitDepth = eNewDepth;
    m_c4LineWidth = c4NewWidth;
    m_c4ImageSz = m_c4LineWidth * m_c4Height;
    m_c4BufSz = m_c4ImageSz;
    m_pc1Pixels = janBuf.paOrphan();

    // And we are adopting now since we created a new buffer
    m_eAdoptOpt = tCIDLib::EAdoptOpts::Adopt;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TPixelArrayImpl::bRGBToRGBA(const   tCIDLib::TCard4     c4TransClr
                        , const tCIDLib::TBoolean   bRepTrans
                        , const TRGBClr&            rgbRepClr)
{
    // Create a new pixel buffer for the new format
    const tCIDImage::EPixFmts eNewFmt = tCIDImage::EPixFmts::TrueAlpha;
    const tCIDImage::EBitDepths eNewDepth = tCIDImage::EBitDepths::Eight;
    const tCIDLib::TCard4 c4NewWidth = TPixelArray::c4CalcLineWidth
    (
        m_c4Width, eNewFmt, eNewDepth, m_c4LineBytePadding
    );

    tCIDLib::TCard1* pc1NewBuf = new tCIDLib::TCard1[c4NewWidth * m_c4Height];
    TArrayJanitor<tCIDLib::TCard1> janBuf(pc1NewBuf);

    tCIDLib::TCard4 c4Cur;
    tCIDLib::TCodedRGB* prgbCur = (tCIDLib::TCodedRGB*)&c4Cur;
    for (tCIDLib::TCard4 c4YInd = 0; c4YInd < m_c4Height; c4YInd++)
    {
        // Because of padding we have to re-calc this for every line
        tCIDLib::TCard1* pc1Out = pc1NewBuf + (c4NewWidth * c4YInd);

        for (tCIDLib::TCard4 c4XInd = 0; c4XInd < m_c4Width; c4XInd++)
        {
            c4Cur = c4At(c4XInd, c4YInd);

            if (c4Cur == c4TransClr)
            {
                //
                //  It matches the transparency color. If replacing it, then
                //  put out the replacement color, else the current color
                //
                if (bRepTrans)
                {
                    *pc1Out++ = rgbRepClr.c1Blue();
                    *pc1Out++ = rgbRepClr.c1Green();
                    *pc1Out++ = rgbRepClr.c1Red();
                }
                 else
                {
                    *pc1Out++ = prgbCur->c1Blue;
                    *pc1Out++ = prgbCur->c1Green;
                    *pc1Out++ = prgbCur->c1Red;
                }

                // Set it fully transparent either way
               *pc1Out++ = 0;
            }
             else
            {
                // No match so just put fully opaque original color
                *pc1Out++ = prgbCur->c1Blue;
                *pc1Out++ = prgbCur->c1Green;
                *pc1Out++ = prgbCur->c1Red;
                *pc1Out++ = 0xFF;
            }
        }
    }

    // Ok, let's swap over to the new stuff now. Delete current buffer if adopting
    if (m_eAdoptOpt == tCIDLib::EAdoptOpts::Adopt)
    {
        delete [] m_pc1Pixels;
        m_pc1Pixels = nullptr;
    }

    m_eFmt = eNewFmt;
    m_eBitDepth = eNewDepth;
    m_c4LineWidth = c4NewWidth;
    m_c4ImageSz = m_c4LineWidth * m_c4Height;
    m_c4BufSz = m_c4ImageSz;
    m_pc1Pixels = janBuf.paOrphan();

    // And we are adopting now since we created a new buffer
    m_eAdoptOpt = tCIDLib::EAdoptOpts::Adopt;
    return kCIDLib::True;
}


//
//  Does a bicubic spline scale of this image to the passed pixel array,
//  using the target's size as the size to scale to. They pass a pallete
//  for us to use if we are a palette based image.
//
tCIDLib::TVoid
TPixelArrayImpl::BSplineScaleTo(        TPixelArrayImpl&    pixaiTar
                                , const TClrPalette&        palToUse
                                , const tCIDLib::TCard4     c4Degree) const
{
    const TSize& szTar = pixaiTar.szImage();

    //
    //  If we are palette based, then force the target pixel array to be
    //  true color, since we have to scale to true color.
    //
    tCIDLib::TCard2 c2Max;
    if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Palette))
    {
        pixaiTar.Reset(tCIDImage::EPixFmts::TrueClr, tCIDImage::EBitDepths::Eight);
        c2Max = 255;
    }
     else
    {
        // We are gray scale or color, so keep it as is
        pixaiTar.Reset(m_eFmt, m_eBitDepth);
        c2Max = (0x1U << tCIDLib::c4EnumOrd(m_eBitDepth)) - 1;
    }

    //
    //  Ok, we have to first allocate a float array of the same size as
    //  the image, to fill with cooefficients. We'll do each color component
    //  (and alpha if present) separately.
    //
    tCIDLib::TFloat4* pf4Cof = new tCIDLib::TFloat4[m_c4Height * m_c4Width];
    TArrayJanitor<tCIDLib::TFloat4> janCof(pf4Cof);

    // We also will also need row/col arrays, which must be Float8s
    tCIDLib::TFloat8* pf8Row = new tCIDLib::TFloat8[m_c4Width];
    TArrayJanitor<tCIDLib::TFloat8> janRow(pf8Row);

    tCIDLib::TFloat8* pf8Col = new tCIDLib::TFloat8[m_c4Height];
    TArrayJanitor<tCIDLib::TFloat8> janCol(pf8Col);

    //
    //  And we'll need a buffer to load interpolated rows back into so
    //  that we can load them back into the pixel array. This one has to
    //  be one row in size (of the TARGET array) and TCard2, since the
    //  largest single component we can have is 16 bit gray scale/alpha.
    //
    tCIDLib::TCard2* pc2Buf = new tCIDLib::TCard2[szTar.c4Width()];
    TArrayJanitor<tCIDLib::TCard2> janLoad(pc2Buf);

    //
    //  If it's gray scale, do the gray scale component, else do the three
    //  color components separately.
    //
    if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Color))
    {
        LoadCoefs(tCIDLib::EClrComps::Red, pf4Cof, pf8Row, pf8Col, c4Degree, palToUse);
        InterpComp(tCIDLib::EClrComps::Red, pf4Cof, pc2Buf, pixaiTar, c4Degree, c2Max);

        LoadCoefs(tCIDLib::EClrComps::Green, pf4Cof, pf8Row, pf8Col, c4Degree, palToUse);
        InterpComp(tCIDLib::EClrComps::Green, pf4Cof, pc2Buf, pixaiTar, c4Degree, c2Max);

        LoadCoefs(tCIDLib::EClrComps::Blue, pf4Cof, pf8Row, pf8Col, c4Degree, palToUse);
        InterpComp(tCIDLib::EClrComps::Blue, pf4Cof, pc2Buf, pixaiTar, c4Degree, c2Max);
    }
     else
    {
        LoadCoefs(tCIDLib::EClrComps::Gray, pf4Cof, pf8Row, pf8Col, c4Degree, palToUse);
        InterpComp(tCIDLib::EClrComps::Gray, pf4Cof, pc2Buf, pixaiTar, c4Degree, c2Max);
    }

    // If alpha is available do it too
    if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
    {
        LoadCoefs(tCIDLib::EClrComps::Alpha, pf4Cof, pf8Row, pf8Col, c4Degree, palToUse);
        InterpComp(tCIDLib::EClrComps::Alpha, pf4Cof, pc2Buf, pixaiTar, c4Degree, c2Max);
    }
}


// Access the raw pixel value at a given position
tCIDLib::TCard4 TPixelArrayImpl::c4At(  const   tCIDLib::TCard4 c4XPos
                                        , const tCIDLib::TCard4 c4YPos) const
{
    return c4GetPixel(c4XPos, c4YPos, CID_LINE);
}

tCIDLib::TCard4 TPixelArrayImpl::c4At(const TQ1Point& pntPos)
{
    return c4GetPixel(pntPos.c4X(), pntPos.c4Y(), CID_LINE);
}


// Return the bits per pixel for our format
tCIDLib::TCard4 TPixelArrayImpl::c4BitsPer() const
{
    return TPixelArray::c4CalcBitsPerPixel(m_eFmt, m_eBitDepth);
}


//
//  This method will find a color not used in the image. It is returned in
//  the native image format, i.e. index, gray scale value, or RGB color. If
//  an RGB color, no alpha is set.
//
tCIDLib::TCard4 TPixelArrayImpl::c4Unused(const TClrPalette& palToUse) const
{
    //
    //  We will do two different scenarios here according to whether we
    //  are dealing with palette/gray scale or RGB based images.
    //
    tCIDLib::TCard4 c4Ret = 0;
    tCIDLib::TCard4 c4Cur;
    if ((m_eFmt == tCIDImage::EPixFmts::TrueClr)
    ||  (m_eFmt == tCIDImage::EPixFmts::TrueAlpha))
    {
        //
        //  It's RGB based, so create 3 arrays of 256 bytes and zero them
        //  all out. By using the r,g,b components as indices into these
        //  arrays we can remember which color components are used.
        //
        tCIDLib::TCard1 ac1Red[256];
        tCIDLib::TCard1 ac1Green[256];
        tCIDLib::TCard1 ac1Blue[256];
        TRawMem::SetMemBuf(ac1Red, tCIDLib::TCard1(0), 256);
        TRawMem::SetMemBuf(ac1Green, tCIDLib::TCard1(0), 256);
        TRawMem::SetMemBuf(ac1Blue, tCIDLib::TCard1(0), 256);

        //
        //  Now look through the image and get each color out as RGB and
        //  set their rgb value's bytes in the arrays.
        //
        tCIDLib::TCodedRGB* prgbCur = (tCIDLib::TCodedRGB*)&c4Cur;
        for (tCIDLib::TCard4 c4YInd = 0; c4YInd < m_c4Height; c4YInd++)
        {
            for (tCIDLib::TCard4 c4XInd = 0; c4XInd < m_c4Width; c4XInd++)
            {
                c4Cur = c4At(c4XInd, c4YInd);

                ac1Red[prgbCur->c1Red] = 1;
                ac1Green[prgbCur->c1Green] = 1;
                ac1Blue[prgbCur->c1Blue] = 1;
            }
        }

        // Find the first unused value in each array
        prgbCur = (tCIDLib::TCodedRGB*)&c4Ret;
        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < 256; c4Ind++)
        {
            if (!ac1Red[c4Ind])
            {
                prgbCur->c1Red = tCIDLib::TCard1(c4Ind);
                break;
            }
        }

        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < 256; c4Ind++)
        {
            if (!ac1Green[c4Ind])
            {
                prgbCur->c1Green = tCIDLib::TCard1(c4Ind);
                break;
            }
        }

        for (tCIDLib::TCard4 c4Ind = 0; c4Ind < 256; c4Ind++)
        {
            if (!ac1Blue[c4Ind])
            {
                prgbCur->c1Blue = tCIDLib::TCard1(c4Ind);
                break;
            }
        }
    }
     else
    {
        //
        //  It's either gray scale or palette based, so we are only dealing
        //  with a single value here.
        //
        //  If it's 16 bit gray scale we have to handle that specially,
        //  else it's 8 bit or less so we can use a 256 element array.
        //
        //  Everything else can be treated as palette based, but we have to
        //  deal with 8 bit gray scale with alpha by throwing out the alpha
        //  part and then it's just like the other 8 bit stuff.
        //
        if (m_eBitDepth == tCIDImage::EBitDepths::Sixteen)
        {
            //
            //  In this case, we have up to 64K actual values, so we don't
            //  want to do a byte per. Instead we use a bit set object so
            //  that we only use a bit per.
            //
            TBitset btsUsed(0x10000);

            for (tCIDLib::TCard4 c4YInd = 0; c4YInd < m_c4Height; c4YInd++)
            {
                for (tCIDLib::TCard4 c4XInd = 0; c4XInd < m_c4Width; c4XInd++)
                {
                    c4Cur = c4At(c4XInd, c4YInd);

                    // If alpha, throw away the alpha part and keep the color
                    if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
                        c4Cur >>= 16;
                    btsUsed.bSetBitState(c4Cur, kCIDLib::True);
                }
            }

            for (; c4Ret < 0x10000; c4Ret++)
            {
                if (!btsUsed.bBitState(c4Ret))
                    break;
            }

            // If all used, can't do much but return 0
            if (c4Ret > 0xFFFFF)
                c4Ret = 0;
        }
         else
        {
            tCIDLib::TCard1 ac1Clr[256];
            TRawMem::SetMemBuf(ac1Clr, tCIDLib::TCard1(0), 256);

            for (tCIDLib::TCard4 c4YInd = 0; c4YInd < m_c4Height; c4YInd++)
            {
                for (tCIDLib::TCard4 c4XInd = 0; c4XInd < m_c4Width; c4XInd++)
                {
                    c4Cur = c4At(c4XInd, c4YInd);

                    // If 8 bit gray with alpha, toss the alpha part
                    if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
                        c4Cur >>= 8;

                    #if CID_DEBUG_ON
                    if (c4Cur > 255)
                    {
                        facCIDImage().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kImgErrs::errcDbg_BadClrIndex
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Format
                            , TCardinal(c4Cur)
                        );
                    }
                    #endif
                    ac1Clr[c4Cur] = 1;
                }
            }

            for (; c4Ret < 256; c4Ret++)
            {
                if (!ac1Clr[c4Ret])
                    break;
            }

            // If all used, can't do much but return 0
            if (c4Ret > 255)
                c4Ret = 0;
        }
    }
    return c4Ret;
}


//
//  Converts this image to a gray scale image. If we are a palette based
//  image, then we just conver the palette to gray scale, which effectively
//  will make this image grays scale when rendered. Else we convert our
//  pixel data to gray scale. We don't convert RGB data format to gray scale
//  format. We just set each RGB value to the calculate gray scale equiv. So
//  there is no reason to reallocate the buffer ever.
//
tCIDLib::TVoid TPixelArrayImpl::CvtToGrayScale(TClrPalette& palToUse)
{
    //
    //  It's a color component based format, so we have to go through the
    //  color data and convert them all to equiv gray scale.
    //
    tCIDLib::TCard1     c1Alpha;
    tCIDLib::TCard1     c1Blue;
    tCIDLib::TCard1     c1Green;
    tCIDLib::TCard1     c1Red;
    tCIDLib::TCard4     c4Clr;
    tCIDLib::TFloat4    f4Tmp;
    tCIDLib::TCodedRGB* prgb32 = reinterpret_cast<tCIDLib::TCodedRGB*>(&c4Clr);
    tCIDLib::TRawRGB*   prgb24 = reinterpret_cast<tCIDLib::TRawRGB*>(&c4Clr);
    for (tCIDLib::TCard4 c4Row = 0; c4Row < m_c4Height; c4Row++)
    {
        for (tCIDLib::TCard4 c4Col = 0; c4Col < m_c4Width; c4Col++)
        {
            c4Clr = c4At(c4Col, c4Row);

            // Get the value to it's RGB constituents
            if (m_eBitDepth == tCIDImage::EBitDepths::Five)
            {
                c1Red = tCIDLib::TCard1((c4Clr >> 10) & 0x1F);
                c1Green = tCIDLib::TCard1((c4Clr >> 5) & 0x1F);
                c1Blue = tCIDLib::TCard1(c4Clr & 0x1F);
            }
             else
            {
                if (tCIDLib::bAllBitsOn(m_eFmt,tCIDImage::EPixFmts::Alpha))
                {
                    c1Red = prgb32->c1Red;
                    c1Green = prgb32->c1Green;
                    c1Blue = prgb32->c1Blue;
                    c1Alpha = prgb32->c1Alpha;
                }
                 else
                {
                    c1Red = prgb24->c1Red;
                    c1Green = prgb24->c1Green;
                    c1Blue = prgb24->c1Blue;
                }
            }

            // Ok, convert these to a gray scale
            f4Tmp = (c1Red * 0.299F) + (c1Green * 0.587F) + (c1Blue * 0.114F);
            if (f4Tmp > 255.0)
                c1Red = 255;
            else
                c1Red = tCIDLib::TCard1(f4Tmp);

            // Get the color back into the right form
            if (m_eBitDepth == tCIDImage::EBitDepths::Five)
            {
                c4Clr = c1Red;
                c4Clr <<= 5;
                c4Clr |= c1Red;
                c4Clr <<= 5;
                c4Clr |= c1Red;
            }
             else
            {
                if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
                {
                    prgb32->c1Red = c1Red;
                    prgb32->c1Green = c1Red;
                    prgb32->c1Blue = c1Red;
                    prgb32->c1Alpha = c1Alpha;
                }
                 else
                {
                    prgb24->c1Red = c1Red;
                    prgb24->c1Green = c1Red;
                    prgb24->c1Blue = c1Red;
                }
            }

            // And store it back
            PutAt(c4Clr, c4Col, c4Row);
        }
    }
}


//
//  We must be True-Alpha format. The caller's pixel array must be the same
//  size as us, and be 8 bit gray scale format. This one must be 8 bit
//  true color.
//
//  We will copy our alpha channel bytes into the caller's array.
//
tCIDLib::TVoid TPixelArrayImpl::ExtractAlphaChannel(TPixelArrayImpl& pixaiAlpha) const
{
    // Get raw pointers to both pixel buffers
    tCIDLib::TCard1* pc1Tar = pixaiAlpha.pc1Buffer();
    const tCIDLib::TCard1* pc1Src = m_pc1Pixels;

    // Get the bytes per row of both into locals for easier access below
    const tCIDLib::TCard4 c4TPerRow = pixaiAlpha.m_c4LineWidth;
    const tCIDLib::TCard4 c4SPerRow = m_c4LineWidth;

    // Move the src up 3 bytes so it's always starting on the alpha pixel
    pc1Src += 3;
    for (tCIDLib::TCard4 c4RowInd = 0; c4RowInd < m_c4Height; c4RowInd++)
    {
        const tCIDLib::TCard1* pc1CurSrc = pc1Src;
        tCIDLib::TCard1* pc1CurTar = pc1Tar;
        for (tCIDLib::TCard4 c4ColInd = 0; c4ColInd < m_c4Width; c4ColInd++)
        {
            *pc1CurTar++ = *pc1CurSrc;
            pc1CurSrc += 4;
        }

        pc1Tar += c4TPerRow;
        pc1Src += c4SPerRow;
    }
}


//
//  Flips the image around it's vertical axis (swaps left and right sides.)
//  by just mirroring the pixels in each colum of the pixel array. We have
//  a base version that takes start/end columns, and another that just calls
//  the base version with 0 and the last column.
//
tCIDLib::TVoid
TPixelArrayImpl::FlipHorizontally(  const   tCIDLib::TCard4 c4StartCol
                                , const tCIDLib::TCard4 c4EndCol)
{
    tCIDLib::TCard4 c4LowCol = c4StartCol;
    tCIDLib::TCard4 c4HighCol = c4EndCol;

    // Flip them if out of order
    if (c4LowCol > c4HighCol)
    {
        tCIDLib::TCard4 c4Tmp = c4LowCol;
        c4LowCol = c4HighCol;
        c4HighCol = c4LowCol;
    }

    // Just on the off chance we have a single line, do nothing
    if (c4HighCol - c4LowCol < 2)
        return;

    //
    //  We do a loop where we move inwards swap until we swapped the last
    //  one or we hit the center (odd number of columns) column that doesn't
    //  need any swapping.
    //
    for (tCIDLib::TCard4 c4Row = 0; c4Row < m_c4Height; c4Row++)
    {
        tCIDLib::TCard4 c4Low = c4LowCol;
        tCIDLib::TCard4 c4High = c4HighCol;
        tCIDLib::TCard4 c4L;
        tCIDLib::TCard4 c4H;
        while (c4Low < c4High)
        {
            // Save the low and high samples
            c4L = c4At(c4Low, c4Row);
            c4H = c4At(c4High, c4Row);

            // And put them back at the opposite positions
            PutAt(c4H, c4Low, c4Row);
            PutAt(c4L, c4High, c4Row);

            c4Low++;
            c4High--;
        }
    }
}

tCIDLib::TVoid TPixelArrayImpl::FlipHorizontally()
{
    if (m_c4Width)
        FlipHorizontally(0, m_c4Width - 1);
}


//
//  This method will flip the image data from top to bottom to bottom to top,
//  and vice versa. We don't really care what the current order is, we just
//  need to exchange the rows from top to bottom. So we call FlipVertically()
//  to flip the image, and then change the row order indicator.
//
tCIDLib::TVoid TPixelArrayImpl::FlipRowOrder()
{
    // Flip the image vertically
    FlipVertically();

    // And flip the row order indicator
    if (m_eRowOrder == tCIDImage::ERowOrders::TopDown)
        m_eRowOrder = tCIDImage::ERowOrders::BottomUp;
    else
        m_eRowOrder = tCIDImage::ERowOrders::TopDown;
}


tCIDLib::TVoid TPixelArrayImpl::ForceRowOrder(const tCIDImage::ERowOrders eRowOrder)
{
    // We have to flip the order
    FlipRowOrder();
}


//
//  Flips the image around it's horizontal axis, i.e. swaps top to bottom.
//  We hvae a base version that takes start/end rows, and another that just
//  calls the base version with 0 and the last row.
//
tCIDLib::TVoid
TPixelArrayImpl::FlipVertically(const   tCIDLib::TCard4 c4StartRow
                                , const tCIDLib::TCard4 c4EndRow)
{
    if (c4StartRow == c4EndRow)
        return;

    tCIDLib::TCard4 c4Low = c4StartRow;
    tCIDLib::TCard4 c4High = c4EndRow;

    // Flip them if out of order
    if (c4Low > c4High)
    {
        tCIDLib::TCard4 c4Tmp = c4Low;
        c4Low = c4High;
        c4High = c4Low;
    }

    // If the low is beyond our last row, do nothing though technically it's an error
    if (c4Low >= m_c4Height)
        return;

    // If the high one is beyond our height, clip it back
    if (c4High >= m_c4Height)
        c4High = m_c4Height - 1;

    // If we end up with a single line, nothing to do
    if (c4High == c4Low)
        return;

    // Create a line buffer so that we can move lines around
    tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[m_c4LineWidth];
    TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Buf);

    // Set up the initial buffer addresses
    tCIDLib::TCard1* pc1Low = m_pc1Pixels + (c4StartRow * m_c4LineWidth);
    tCIDLib::TCard1* pc1High = m_pc1Pixels + (c4EndRow * m_c4LineWidth);
    const tCIDLib::TCard4  c4RowCnt = m_c4LineWidth;

    //
    //  We do a loop where we move inwards from the first/last lines and swap
    //  until we swapped the last one or we hit the center (odd number of
    //  lines) line that doesn't need any swapping.
    //
    //  c4High has to be at least 1, so it cannot underflow, even if we just have
    //  two lines.
    //
    while (c4High > c4Low)
    {
        tCIDLib::TCard1* pc1T = pc1Buf;
        tCIDLib::TCard1* pc1H = pc1High;

        // Copy the high to the temp buffer
        for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4LineWidth; c4Index++)
            *pc1T++ = *pc1H++;

        // Copy the low to the high
        tCIDLib::TCard1* pc1L = pc1Low;
        pc1H = pc1High;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4LineWidth; c4Index++)
            *pc1H++ = *pc1L++;

        // And now the temp to the low
        pc1L = pc1Low;
        pc1T = pc1Buf;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4LineWidth; c4Index++)
            *pc1L++ = *pc1T++;

        // Move our indices and base pointers inward a line
        c4High--;
        c4Low++;
        pc1High -= m_c4LineWidth;
        pc1Low += m_c4LineWidth;
    }
}

tCIDLib::TVoid TPixelArrayImpl::FlipVertically()
{
    if (m_c4Height)
        FlipVertically(0, m_c4Height - 1);
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
TPixelArrayImpl::GaussianBlur(  const   tCIDLib::TCard4         c4Order
                                ,       TPixelArrayImpl* const  ppixaiTmp)
{
    GaussianBlur(c4Order, m_c4Width, m_c4Height, ppixaiTmp);
}

tCIDLib::TVoid
TPixelArrayImpl::GaussianBlur(  const   tCIDLib::TCard4         c4Order
                                , const tCIDLib::TCard4         c4MaxWidth
                                , const tCIDLib::TCard4         c4MaxHeight
                                ,       TPixelArrayImpl* const  ppixaiTmp)
{
    // If they didn't provide a temp, we need to create one
    TPixelArrayImpl* ppixaiOut = ppixaiTmp;
    TPixelArrayImpl* ppixaiLocal = nullptr;
    if (!ppixaiOut)
    {
        ppixaiLocal = new TPixelArrayImpl(*this);
        ppixaiOut = ppixaiLocal;
    }
    TJanitor<TPixelArrayImpl> janLocal(ppixaiLocal);

    // If the order is too big, clip it
    static const tCIDLib::TCard4 c4MaxOrder = 12;
    const tCIDLib::TCard4 c4OrderInd
    (
        (c4Order > c4MaxOrder) ? c4MaxOrder : c4Order
    );

    //
    //  We have two lists. One is the set of factors for a given order, and
    //  the order is the number of factors for each order. They don't
    //  strictly match the order number because we use 0, 1, and 2 for
    //  special case factors, since they'd otherwise go unused.
    //
    static const tCIDLib::TInt4 ai4OrderFactors[c4MaxOrder + 1][c4MaxOrder + 1] =
    {
        //
        //  These first 3 are special ones designed to create a greater
        //  dispersion, for live graphics to create a good blur with fewer
        //  rounds.
        //
        {   2,   4,   5,   5,   2,   0,   0,   0,   0,   0,   0,   0 }

      , {   2,   8,  20,  32,  32,  20,   8,   2,   0,   0,   0,   0 }
      , {   3,  13,  24,  36,  42,  42,  36,  24,  13,   3,   0,   0 }

      , {   1,   2,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0 }
      , {   1,   3,   3,   1,   0,   0,   0,   0,   0,   0,   0,   0 }
      , {   1,   4,   6,   4,   1,   0,   0,   0,   0,   0,   0,   0 }
      , {   1,   5,  10,  10,   5,   1,   0,   0,   0,   0,   0,   0 }
      , {   1,   6,  15,  20,  15,   6,   1,   0,   0,   0,   0,   0 }
      , {   1,   7,  21,  35,  35,  21,   7,   1,   0,   0,   0,   0 }
      , {   1,   8,  28,  56,  70,  56,  28,   8,   1,   0,   0,   0 }
      , {   1,   9,  36,  84, 126, 126,  84,  36,   9,   1,   0,   0 }
      , {   1,  10,  45, 120, 210, 252, 210, 120,  45,  10,   1,   0 }
      , {   1,  11,  55, 165, 330, 462, 462, 330, 165,  55,  11,   1 }
    };
    static const tCIDLib::TInt4 ai4FactorsPerOrder[c4MaxOrder + 1] =
    {
        5, 8, 10, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
    };

    // Set the width and pre-calc the half-way point for this width
    tCIDLib::TInt4 i4GWidth = ai4FactorsPerOrder[c4OrderInd];
    const tCIDLib::TInt4 i4GWidth2 = (i4GWidth - 1) >> 1;

    //
    //  Set up the factor list ofr this order, and the divisor (which is just
    //  the sum of the factors.
    //
    const tCIDLib::TInt4* pi4GFactors = ai4OrderFactors[c4OrderInd];
    tCIDLib::TInt4 i4Divisor = 0;
    for (tCIDLib::TInt4 i4Ind = 0; i4Ind < i4GWidth; i4Ind++)
        i4Divisor += pi4GFactors[i4Ind];

    //
    //  Set the up h/v limits and as signed values to make the stuff below
    //  easier. This algorithm skips the outer pixels in the loop interation
    //  values, so we move them back by one.
    //
    const tCIDLib::TInt4 i4HLim = tCIDLib::TInt4(c4MaxWidth - 1);
    const tCIDLib::TInt4 i4VLim = tCIDLib::TInt4(c4MaxHeight - 1);

    //
    //  For the two passes, the ingoing data is not changed as we go, so we
    //  we can pre-multiply the values in each row/col by the current factor
    /// and avoid many multiplications since they are done very redundantly.
    //  So allocate an array big enough to hold a pre-multiplied value for
    //  each pixel in the biggest axis of ours.
    //
    //  This can save millions of multiplies in a large, 32 bit image and
    //  vastly speed up the operation.
    //
    const tCIDLib::TCard4 c4MaxAxis = tCIDLib::MaxVal(c4MaxWidth, c4MaxHeight);
    tCIDLib::TInt4* pi4TableBuf = new tCIDLib::TInt4[i4GWidth * c4MaxAxis];
    TArrayJanitor<tCIDLib::TInt4> janTableBuf(pi4TableBuf);
    tCIDLib::TInt4* api4TableSlots[c4MaxOrder];

    if (m_eBitDepth == tCIDImage::EBitDepths::Five)
    {
        //
        //  Three component 5 bit color, so we special case this one because it's
        //  such an odd man out, and not an often used format, so we don't
        //  bother to overly optimize it either.
        //
        tCIDLib::TInt4 i4C1;
        tCIDLib::TInt4 i4C2;
        tCIDLib::TInt4 i4C3;
        tCIDLib::TCard4 c4Clr;

        for (tCIDLib::TInt4 i4HInd = 1; i4HInd < i4HLim; i4HInd++)
        {
            for (tCIDLib::TInt4 i4VInd = 1; i4VInd < i4VLim; i4VInd++)
            {
                i4C1 = 0; i4C2 = 0; i4C2 = 0;
                for (tCIDLib::TInt4 i4KInd = 0; i4KInd < i4GWidth; i4KInd++)
                {
                    c4Clr = c4GetBoundaryPixel(i4HInd - i4GWidth2 + i4KInd, i4VInd, CID_LINE);
                    tCIDLib::TInt4 i4Factor = pi4GFactors[i4KInd];
                    i4C1 += tCIDLib::TInt4((c4Clr >> 10) & 0x1F) * i4Factor;
                    i4C2 += tCIDLib::TInt4((c4Clr >> 5) & 0x1F) * i4Factor;
                    i4C3 += tCIDLib::TInt4(c4Clr & 0x1F) * i4Factor;
                }
                i4C1 /= i4Divisor; i4C2 /= i4Divisor; i4C3 /= i4Divisor;

                // Build up the color again
                c4Clr = tCIDLib::TCard4(i4C1);
                c4Clr <<= 5;
                c4Clr |= tCIDLib::TCard4(i4C2);
                c4Clr <<= 5;
                c4Clr |= tCIDLib::TCard4(i4C3);
                ppixaiOut->PutPixel(c4Clr, tCIDLib::TCard4(i4HInd), tCIDLib::TCard4(i4VInd), CID_LINE);
            }
        }

        for (tCIDLib::TInt4 i4HInd = 1; i4HInd < i4HLim; i4HInd++)
        {
            for (tCIDLib::TInt4 i4VInd = 1; i4VInd < i4VLim; i4VInd++)
            {
                i4C1 = 0; i4C2 = 0; i4C2 = 0;
                for (tCIDLib::TInt4 i4KInd = 0; i4KInd < i4GWidth; i4KInd++)
                {
                    c4Clr = ppixaiOut->c4GetBoundaryPixel(i4HInd, i4VInd - i4GWidth2 + i4KInd, CID_LINE);
                    tCIDLib::TInt4 i4Factor = pi4GFactors[i4KInd];
                    i4C1 += tCIDLib::TInt4((c4Clr >> 10) & 0x1F) * i4Factor;
                    i4C2 += tCIDLib::TInt4((c4Clr >> 5) & 0x1F) * i4Factor;
                    i4C3 += tCIDLib::TInt4(c4Clr & 0x1F) * i4Factor;
                }
                i4C1 /= i4Divisor; i4C2 /= i4Divisor; i4C3 /= i4Divisor;

                // Build up the color again
                c4Clr = tCIDLib::TCard4(i4C1);
                c4Clr <<= 5;
                c4Clr |= tCIDLib::TCard4(i4C2);
                c4Clr <<= 5;
                c4Clr |= tCIDLib::TCard4(i4C3);
                PutPixel(c4Clr, tCIDLib::TCard4(i4HInd), tCIDLib::TCard4(i4VInd), CID_LINE);
            }
        }
    }
     else if (m_eBitDepth == tCIDImage::EBitDepths::Eight)
    {
        //
        //  These are all byte or oriented, so we can do a generic algorithm
        //  that will handle them all. We just need to set up some counters
        //  that represent the number of bytes per pixel.
        //
        tCIDLib::TCard4 c4CCnt;
        switch(m_eFmt)
        {
            case tCIDImage::EPixFmts::TrueClr :
                c4CCnt = 3;
                break;

            case tCIDImage::EPixFmts::TrueAlpha :
                c4CCnt = 4;
                break;

            case tCIDImage::EPixFmts::GrayScale :
                c4CCnt = 1;
                break;

            case tCIDImage::EPixFmts::GrayAlpha :
                c4CCnt = 2;
                break;
        };
        tCIDLib::TInt4 i4Accum;

        // Divide the pre-mul table up for horizontal use
        for (tCIDLib::TInt4 i4Index = 0; i4Index < i4GWidth; i4Index++)
            api4TableSlots[i4Index] = pi4TableBuf + (i4Index * c4MaxWidth);

        for (tCIDLib::TInt4 i4VInd = 1; i4VInd < i4VLim; i4VInd++)
        {
            tCIDLib::TCard1* pc1BaseTPtr = ppixaiOut->pc1RowPtr(i4VInd) + c4CCnt;
            tCIDLib::TCard1* pc1BaseRow = pc1RowPtr(i4VInd);
            for (tCIDLib::TCard4 c4CInd = 0; c4CInd < c4CCnt; c4CInd++)
            {
                tCIDLib::TCard1* pc1CompPtr = pc1BaseRow + c4CInd;
                tCIDLib::TCard1* pc1TPtr = pc1BaseTPtr + c4CInd;
                for (tCIDLib::TInt4 i4FInd = 0; i4FInd < i4GWidth; i4FInd++)
                {
                    tCIDLib::TInt4* pi4FRow = api4TableSlots[i4FInd];

                    const tCIDLib::TInt4 i4Factor = pi4GFactors[i4FInd];
                    tCIDLib::TCard1* pc1MPtr = pc1CompPtr;
                    for (tCIDLib::TCard4 c4RInd = 0; c4RInd < c4MaxWidth; c4RInd++)
                    {
                        *pi4FRow++ = tCIDLib::TInt4(*pc1MPtr) * i4Factor;
                        pc1MPtr += c4CCnt;
                    }
                }

                for (tCIDLib::TInt4 i4HInd = 1; i4HInd < i4HLim; i4HInd++)
                {
                    tCIDLib::TInt4 i4HOfs = i4HInd - i4GWidth2;
                    i4Accum = 0;
                    for (tCIDLib::TInt4 i4KInd = 0; i4KInd < i4GWidth; i4KInd++)
                    {
                        if ((i4HOfs >= 0) && (i4HOfs <= i4HLim))
                            i4Accum += api4TableSlots[i4KInd][i4HOfs];
                        i4HOfs++;
                    }
                    i4Accum /= i4Divisor;
                    *pc1TPtr = tCIDLib::TCard1(i4Accum);
                    pc1TPtr += c4CCnt;
                }
            }
        }

        // Divide the pre-mul table up for vertical use
        for (tCIDLib::TInt4 i4Index = 0; i4Index < i4GWidth; i4Index++)
            api4TableSlots[i4Index] = pi4TableBuf + (i4Index * c4MaxHeight);

        for (tCIDLib::TInt4 i4HInd = 1; i4HInd < i4HLim; i4HInd++)
        {
            // The current horizontal offset (the component column)
            tCIDLib::TCard4 c4HOfs = (i4HInd * c4CCnt);
            for (tCIDLib::TCard4 c4CInd = 0; c4CInd < c4CCnt; c4CInd++)
            {
                // Get a pointer to the current source byte column
                tCIDLib::TCard1* pc1CompPtr = ppixaiOut->m_pc1Pixels
                                              + c4HOfs + c4CInd;
                for (tCIDLib::TInt4 i4FInd = 0; i4FInd < i4GWidth; i4FInd++)
                {
                    tCIDLib::TCard1* pc1MPtr = pc1CompPtr;
                    tCIDLib::TInt4* pi4FRow = api4TableSlots[i4FInd];
                    const tCIDLib::TInt4 i4Factor = pi4GFactors[i4FInd];
                    for (tCIDLib::TCard4 c4VInd = 0; c4VInd < c4MaxHeight; c4VInd++)
                    {
                        *pi4FRow++ = tCIDLib::TInt4(*pc1MPtr) * i4Factor;
                        pc1MPtr += ppixaiOut->m_c4LineWidth;
                    }
                }

                tCIDLib::TCard1* pc1TPtr = m_pc1Pixels + c4HOfs + c4CInd;
                for (tCIDLib::TInt4 i4VInd = 1; i4VInd < i4VLim; i4VInd++)
                {
                    tCIDLib::TInt4 i4VOfs = i4VInd - i4GWidth2;
                    i4Accum = 0;
                    for (tCIDLib::TInt4 i4KInd = 0; i4KInd < i4GWidth; i4KInd++)
                    {
                        if ((i4VOfs >= 0) && (i4VOfs <= i4VLim))
                            i4Accum += api4TableSlots[i4KInd][i4VOfs];
                        i4VOfs++;
                    }
                    i4Accum /= i4Divisor;
                    *pc1TPtr = tCIDLib::TCard1(i4Accum);
                    pc1TPtr += m_c4LineWidth;
                }
            }
        }
    }
     else if (m_eBitDepth == tCIDImage::EBitDepths::Sixteen)
    {
        // These are word oriented, so we can do a single generic one for these
        tCIDLib::TCard4 c4CCnt;
        if (m_eFmt == tCIDImage::EPixFmts::GrayScale)
            c4CCnt = 1;
        else if (m_eFmt == tCIDImage::EPixFmts::GrayAlpha)
            c4CCnt = 2;
        tCIDLib::TInt4 i4Accum;

        // Divide the pre-mul table up for horizontal use
        for (tCIDLib::TInt4 i4Index = 0; i4Index < i4GWidth; i4Index++)
            api4TableSlots[i4Index] = pi4TableBuf + (i4Index * c4MaxWidth);

        for (tCIDLib::TInt4 i4VInd = 1; i4VInd < i4VLim; i4VInd++)
        {
            tCIDLib::TCard2* pc2BaseTPtr = reinterpret_cast<tCIDLib::TCard2*>
            (
                ppixaiOut->pc1RowPtr(i4VInd) + c4CCnt
            );
            tCIDLib::TCard2* pc2BaseRow = reinterpret_cast<tCIDLib::TCard2*>
            (
                pc1RowPtr(i4VInd)
            );
            for (tCIDLib::TCard4 c4CInd = 0; c4CInd < c4CCnt; c4CInd++)
            {
                tCIDLib::TCard2* pc2CompPtr = pc2BaseRow + c4CInd;
                tCIDLib::TCard2* pc2TPtr = pc2BaseTPtr + c4CInd;
                for (tCIDLib::TInt4 i4FInd = 0; i4FInd < i4GWidth; i4FInd++)
                {
                    tCIDLib::TInt4* pi4FRow = api4TableSlots[i4FInd];

                    const tCIDLib::TInt4 i4Factor = pi4GFactors[i4FInd];
                    tCIDLib::TCard2* pc2MPtr = pc2CompPtr;
                    for (tCIDLib::TCard4 c4RInd = 0; c4RInd < c4MaxWidth; c4RInd++)
                    {
                        *pi4FRow++ = tCIDLib::TInt4(*pc2MPtr) * i4Factor;
                        pc2MPtr += c4CCnt;
                    }
                }

                for (tCIDLib::TInt4 i4HInd = 1; i4HInd < i4HLim; i4HInd++)
                {
                    tCIDLib::TInt4 i4HOfs = i4HInd - i4GWidth2;
                    i4Accum = 0;
                    for (tCIDLib::TInt4 i4KInd = 0; i4KInd < i4GWidth; i4KInd++)
                    {
                        if ((i4HOfs >= 0) && (i4HOfs <= i4HLim))
                            i4Accum += api4TableSlots[i4KInd][i4HOfs];
                        i4HOfs++;
                    }
                    i4Accum /= i4Divisor;
                    *pc2TPtr = tCIDLib::TCard2(i4Accum);
                    pc2TPtr += c4CCnt;
                }
            }
        }

        // Divide the pre-mul table up for vertical use
        for (tCIDLib::TInt4 i4Index = 0; i4Index < i4GWidth; i4Index++)
            api4TableSlots[i4Index] = pi4TableBuf + (i4Index * c4MaxHeight);

        for (tCIDLib::TInt4 i4HInd = 1; i4HInd < i4HLim; i4HInd++)
        {
            // The current horizontal offset (the component column)
            tCIDLib::TCard4 c4HOfs = (i4HInd * c4CCnt);
            for (tCIDLib::TCard4 c4CInd = 0; c4CInd < c4CCnt; c4CInd++)
            {
                // Get a pointer to the current source byte column
                tCIDLib::TCard2* pc2CompPtr = reinterpret_cast<tCIDLib::TCard2*>
                (
                    ppixaiOut->m_pc1Pixels + c4HOfs + c4CInd
                );

                // Create the pre-mul table
                for (tCIDLib::TInt4 i4FInd = 0; i4FInd < i4GWidth; i4FInd++)
                {
                    tCIDLib::TCard2* pc2MPtr = pc2CompPtr;
                    tCIDLib::TInt4* pi4FRow = api4TableSlots[i4FInd];
                    const tCIDLib::TInt4 i4Factor = pi4GFactors[i4FInd];
                    for (tCIDLib::TCard4 c4VInd = 0; c4VInd < c4MaxHeight; c4VInd++)
                    {
                        *pi4FRow++ = tCIDLib::TInt4(*pc2MPtr) * i4Factor;
                        pc2MPtr += ppixaiOut->m_c4LineWidth;
                    }
                }

                // Set the target byte column pointer for output
                tCIDLib::TCard2* pc2TPtr = reinterpret_cast<tCIDLib::TCard2*>
                (
                    m_pc1Pixels + m_c4LineWidth + c4HOfs + c4CInd
                );
                for (tCIDLib::TInt4 i4VInd = 1; i4VInd < i4VLim; i4VInd++)
                {
                    tCIDLib::TInt4 i4VOfs = i4VInd - i4GWidth2;
                    i4Accum = 0;
                    for (tCIDLib::TInt4 i4KInd = 0; i4KInd < i4GWidth; i4KInd++)
                    {
                        if ((i4VOfs >= 0) && (i4VOfs <= i4VLim))
                            i4Accum += api4TableSlots[i4KInd][i4VOfs];
                        i4VOfs++;
                    }
                    i4Accum /= i4Divisor;
                    *pc2TPtr = tCIDLib::TCard2(i4Accum);
                    pc2TPtr += m_c4LineWidth;
                }
            }
        }
    }
}


//
//  Inverts the alpha channel on this pixel array. So it just subtracts
//  each alpha byte from 0xFF.
//
tCIDLib::TVoid TPixelArrayImpl::InvertAlphaChannel()
{
    // Get a pointers to our raw pixel buffer
    tCIDLib::TCard1* pc1Src = m_pc1Pixels;
    for (tCIDLib::TCard4 c4RowInd = 0; c4RowInd < m_c4Height; c4RowInd++)
    {
        // Start on the alpha byte so we can just move forward a pixel at a time
        tCIDLib::TCard1* pc1Cur = pc1Src + 3;
        for (tCIDLib::TCard4 c4ColInd = 0; c4ColInd < m_c4Width; c4ColInd++)
        {
            *pc1Cur = 0xFF - *pc1Cur;
            pc1Cur += 4;
        }
        pc1Src += m_c4LineWidth;
    }
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
TPixelArrayImpl::LoadCompRow(       tCIDLib::TFloat8* const pf8ToLoad
                        , const tCIDLib::TCard4         c4Row
                        , const tCIDLib::EClrComps      eComp
                        , const TClrPalette&            palToUse) const
{
    tCIDLib::TCard4 c4Cur;
    tCIDLib::TFloat8* pf8Cur = pf8ToLoad;

    //
    //  Special case some of the very common ones so that we get maximum
    //  performance. So watch for the true color and true-alpha variations
    //  and handle them separately.
    //
    if ((m_eBitDepth == tCIDImage::EBitDepths::Eight)
    &&  ((m_eFmt == tCIDImage::EPixFmts::TrueClr)
    ||   (m_eFmt == tCIDImage::EPixFmts::TrueAlpha)))
    {
        tCIDLib::TCard4 c4Comps = 3;
        if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
            c4Comps++;
        tCIDLib::TCard1* pc1Ptr = m_pc1Pixels + (c4Row * m_c4LineWidth);
        switch(eComp)
        {
            case tCIDLib::EClrComps::Alpha :
                pc1Ptr += 3;
                break;

            case tCIDLib::EClrComps::Green :
                pc1Ptr++;
                break;

            case tCIDLib::EClrComps::Red :
                pc1Ptr += 2;
                break;

            default :
                break;
        };
        for (tCIDLib::TCard4 c4Col = 0; c4Col < m_c4Width; c4Col++)
        {
            *pf8Cur++ = *pc1Ptr;
            pc1Ptr += c4Comps;
        }
    }
     else
    {
        // Just use generic pixel management stuff
        for (tCIDLib::TCard4 c4Col = 0; c4Col < m_c4Width; c4Col++)
        {
            c4Cur = c4GetPixel(c4Col, c4Row, CID_LINE);

            switch(m_eBitDepth)
            {
                case tCIDImage::EBitDepths::One :
                case tCIDImage::EBitDepths::Two :
                case tCIDImage::EBitDepths::Four :
                {
                    //
                    //  These will all be gray scale, or palette based. If
                    //  palette based, we have to pull out the color from the
                    //  palette.
                    //
                    if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Palette))
                        *pf8Cur = palToUse.rgbAt(c4Cur).c1ClrComp(eComp);
                    else
                        *pf8Cur = c4Cur;
                    break;
                }

                // It's 5 bit color compressed into a 16 bit value
                case tCIDImage::EBitDepths::Five :
                {
                    if (eComp == tCIDLib::EClrComps::Red)
                        *pf8Cur = tCIDLib::TCard1((c4Cur >> 10) & 0x1F);
                    else if (eComp == tCIDLib::EClrComps::Green)
                        *pf8Cur = tCIDLib::TCard1((c4Cur >> 5) & 0x1F);
                    else
                        *pf8Cur = tCIDLib::TCard1(c4Cur & 0x1F);
                    break;
                }

                case tCIDImage::EBitDepths::Eight :
                {
                    //
                    //  It'll be either gray scale (with or without alpha) or
                    //  palette based.
                    //
                    if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Palette))
                    {
                        *pf8Cur = palToUse.rgbAt(c4Cur).c1ClrComp(eComp);
                    }
                     else
                    {
                        if (eComp == tCIDLib::EClrComps::Alpha)
                        {
                            *pf8Cur = c4Cur & 0xFF;
                        }
                        else
                        {
                            if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
                                *pf8Cur = (c4Cur >> 8) & 0xFF;
                            else
                                *pf8Cur = c4Cur & 0xFF;
                        }
                    }
                    break;
                }

                case tCIDImage::EBitDepths::Sixteen :
                {
                    if (eComp == tCIDLib::EClrComps::Alpha)
                    {
                        *pf8Cur = c4Cur & 0xFFFF;
                    }
                     else
                    {
                        if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
                            *pf8Cur = (c4Cur >> 16) & 0xFFFF;
                        else
                            *pf8Cur = c4Cur & 0xFFFF;
                    }
                    break;
                }

                default :
                    facCIDImage().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kImgErrs::errcImg_BadBitDepth
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Internal
                        , TInteger(tCIDLib::i4EnumOrd(m_eBitDepth))
                    );
                    break;
            };

            pf8Cur++;
        }
    }
}


tCIDLib::TVoid TPixelArrayImpl::MakeMask(       TPixelArrayImpl&    pixaiToFill
                                        , const tCIDLib::TCard4     c4ClrVal
                                        , const tCIDLib::TBoolean   bInvert) const
{
    // Make sure that passed array is the same size as us and 1 bit
    pixaiToFill.Reset
    (
        tCIDImage::EPixFmts::GrayScale
        , tCIDImage::EBitDepths::One
        , m_eRowOrder
        , szImage()
    );

    //
    //  Ok, lets loop through our pixels. For each one that is equal to the
    //  mask color, set it on (or off, if inverting.) We optimize the true/color
    //  and true/alpha scenarios since they are the ones most likely used.
    //
    if ((m_eFmt == tCIDImage::EPixFmts::TrueAlpha)
    &&  (m_eBitDepth == tCIDImage::EBitDepths::Eight))
    {
        const tCIDLib::TCard1* pc1Src = m_pc1Pixels;
        for (tCIDLib::TCard4 c4RowInd = 0; c4RowInd < m_c4Height; c4RowInd++)
        {
            tCIDLib::TCard1* pc1Tar = pixaiToFill.pc1RowPtr(c4RowInd);
            const tCIDLib::TCard1* pc1CurSrc = pc1Src;
            for (tCIDLib::TCard4 c4ColInd = 0; c4ColInd < m_c4Width; c4ColInd++)
            {
                // Get the current color out and see if it's the one we want
                tCIDLib::TCard4 c4Clr = *(pc1CurSrc + 2);
                c4Clr <<= 8;
                c4Clr |= *(pc1CurSrc + 1);
                c4Clr <<= 8;
                c4Clr |= *pc1CurSrc;

                if (c4Clr == c4Clr)
                    pc1Tar[c4ColInd / 8] |= tCIDLib::TCard1(0x1) << (7 - (c4ColInd % 8));

                pc1CurSrc += 4;
            }

            // Move to the next src scan line
            pc1Src += m_c4LineWidth;
        }
    }
     else if ((m_eFmt == tCIDImage::EPixFmts::TrueClr)
          &&  (m_eBitDepth == tCIDImage::EBitDepths::Eight))
    {
        const tCIDLib::TCard1* pc1Src = m_pc1Pixels;
        for (tCIDLib::TCard4 c4RowInd = 0; c4RowInd < m_c4Height; c4RowInd++)
        {
            tCIDLib::TCard1* pc1Tar = pixaiToFill.pc1RowPtr(c4RowInd);
            const tCIDLib::TCard1* pc1CurSrc = pc1Src;
            for (tCIDLib::TCard4 c4ColInd = 0; c4ColInd < m_c4Width; c4ColInd++)
            {
                // Get the current color out and see if it's the one we want
                tCIDLib::TCard4 c4Clr = *(pc1CurSrc + 2);
                c4Clr <<= 8;
                c4Clr |= *(pc1CurSrc + 1);
                c4Clr <<= 8;
                c4Clr |= *pc1CurSrc;

                if (c4Clr == c4Clr)
                    pc1Tar[c4ColInd / 8] |= tCIDLib::TCard1(0x1) << (7 - (c4ColInd % 8));

                pc1CurSrc += 3;
            }

            // Move to the next src scan line
            pc1Src += m_c4LineWidth;
        }
    }
     else
    {
        tCIDLib::TCard4 c4PosVal = bInvert ? 0 : 1;
        tCIDLib::TCard4 c4NegVal = bInvert ? 1 : 0;
        for (tCIDLib::TCard4 c4Row = 0; c4Row < m_c4Height; c4Row++)
        {
            for (tCIDLib::TCard4 c4Col = 0; c4Col < m_c4Width; c4Col++)
            {
                if ((c4GetPixel(c4Col, c4Row, CID_LINE) == c4ClrVal))
                    pixaiToFill.PutAt(c4PosVal, c4Col, c4Row);
                else
                    pixaiToFill.PutAt(c4NegVal, c4Col, c4Row);
            }
        }
    }
}


//
//  Creates a thumbnail version of this pixel array data into the passed
//  pixel array, taking into account format, alpha, transparency info, and
//  so forth.
//
tCIDLib::TVoid
TPixelArrayImpl::MakeScaled(        TPixelArrayImpl&    pixaiThumb
                            , const TClrPalette&        palClrs
                            , const tCIDLib::TBoolean   bTransClr
                            , const tCIDLib::TCard4     c4TransClr
                            , const tCIDLib::TCard4     c4Degree) const
{
    BSplineScaleTo(pixaiThumb, palClrs, c4Degree);

    //
    //  If there is color based transparency to deal with we have to handle
    //  that separately. We need to mask back off the image we created in
    //  in the scaling operation above.
    //
    if (!tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha) && bTransClr)
    {
        //
        //  Make a mask based on the transparency color. We do a special
        //  type of mask here. It's a 16 color gray scale. We set fgn pixels
        //  to 15 and transparent pixels to 0. Then we can scale and will
        //  end up with a mask that has 16 levels of possible transparency.
        //
        TPixelArray pixaiMask
        (
            tCIDImage::EPixFmts::GrayScale
            , tCIDImage::EBitDepths::Four
            , m_eRowOrder
            , TSize(m_c4Width, m_c4Height)
        );
        for (tCIDLib::TCard4 c4Row = 0; c4Row < m_c4Height; c4Row++)
        {
            for (tCIDLib::TCard4 c4Col = 0; c4Col < m_c4Width; c4Col++)
            {
                if ((c4GetPixel(c4Col, c4Row, CID_LINE) == c4TransClr))
                    pixaiMask.PutAt(0, c4Col, c4Row);
                else
                    pixaiMask.PutAt(15, c4Col, c4Row);
            }
        }

        // Now scale the mask. Doesn't matter what palette we pass here
        TPixelArray pixaiSMask
        (
            tCIDImage::EPixFmts::GrayScale
            , tCIDImage::EBitDepths::Four
            , m_eRowOrder
            , pixaiThumb.szImage()
        );
        pixaiMask.BSplineScaleTo(pixaiSMask, palClrs, 3);

        //
        //  Now go back and for every pixel in the mask that didn't come
        //  out 14 or higher, make it the transparent color.
        //
        const tCIDLib::TCard4 c4TWidth = pixaiThumb.c4Width();
        const tCIDLib::TCard4 c4THeight = pixaiThumb.c4Height();

        //
        //  If we are palette based, then it will have been converted
        //  to RGB, so we have to update the transparency color over to
        //  a true color format from it's palette format. Else we can
        //  go with the index/grayscale value we got.
        //
        tCIDLib::TCard4 c4ActTrans = c4TransClr;
        if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Palette))
            c4ActTrans = palClrs[c4TransClr].c4Color();

        for (tCIDLib::TCard4 c4Row = 0; c4Row < c4THeight; c4Row++)
        {
            for (tCIDLib::TCard4 c4Col = 0; c4Col < c4TWidth; c4Col++)
            {
                if (pixaiSMask.c4At(c4Col, c4Row) < 12)
                    pixaiThumb.PutAt(c4ActTrans, c4Col, c4Row);
            }
        }
    }
}


// Public class checks the row, but assert just in case
const tCIDLib::TCard1*
TPixelArrayImpl::pc1RowPtr(const tCIDLib::TCard4 c4Row) const
{
    CIDAssert(c4Row <= m_c4Height, L"Invalid pixel array row index");
    return m_pc1Pixels + (c4Row * m_c4LineWidth);
}

tCIDLib::TCard1* TPixelArrayImpl::pc1RowPtr(const tCIDLib::TCard4 c4Row)
{
    CIDAssert(c4Row <= m_c4Height, L"Invalid pixel array row index");
    return m_pc1Pixels + (c4Row * m_c4LineWidth);
}


//
//  Assuming stuff has been drawn into this pixel array, go back and do a
//  pre-multiply pass on us. We must be true alpha
//
tCIDLib::TVoid TPixelArrayImpl::Premultiply()
{
    tCIDLib::TCard1* pc1Dest = m_pc1Pixels;
    for (tCIDLib::TCard4 c4RowInd = 0; c4RowInd < m_c4Height; c4RowInd++)
    {
        tCIDLib::TCard1* pc1Cur = pc1Dest;
        for (tCIDLib::TCard4 c4ColInd = 0; c4ColInd < m_c4Width; c4ColInd++)
        {
            // Get the alpha out first
            const tCIDLib::TCard1 c1Alpha = *(pc1Cur + 3);

            //
            //  If it's zero just zero the pixel. If it's 0xFF, do nothing since
            //  it won't have any effect. Else do the pre-mul.
            //
            if (c1Alpha == 0)
            {
                *pc1Cur++ = 0;
                *pc1Cur++ = 0;
                *pc1Cur++ = 0;
                pc1Cur++;
            }
             else if (c1Alpha == 0xFF)
            {
                pc1Cur += 4;
            }
             else
            {
                tCIDLib::TCard2 c2Val = *pc1Cur;
                c2Val *= c1Alpha;
                c2Val /= 0xFF;
                *pc1Cur++ = tCIDLib::TCard1(c2Val);

                c2Val = *pc1Cur;
                c2Val *= c1Alpha;
                c2Val /= 0xFF;
                *pc1Cur++ = tCIDLib::TCard1(c2Val);

                c2Val = *pc1Cur;
                c2Val *= c1Alpha;
                c2Val /= 0xFF;
                *pc1Cur++ = tCIDLib::TCard1(c2Val);

                pc1Cur++;
            }
        }

        // Move our base pointer up a line
        pc1Dest += m_c4LineWidth;
    }
}



// Put pixel values into the array at specific points
tCIDLib::TVoid TPixelArrayImpl::PutAt(  const   tCIDLib::TCard4 c4ToPut
                                        , const tCIDLib::TCard4 c4XPos
                                        , const tCIDLib::TCard4 c4YPos)
{
    PutPixel(c4ToPut, c4XPos, c4YPos, CID_LINE);
}

tCIDLib::TVoid TPixelArrayImpl::PutAt(  const   tCIDLib::TCard4 c4ToPut
                                        , const TQ1Point&       pntPos)
{
    PutPixel(c4ToPut, pntPos.c4X(), pntPos.c4Y(), CID_LINE);
}


// Get the RGB value of a pixel at a particular point
tCIDLib::TVoid
TPixelArrayImpl::QueryRGBAt(const   TPoint&         pntAt
                            , const TClrPalette&    palToUse
                            ,       TRGBClr&        rgbToFill) const
{
    tCIDLib::TCard4 c4Clr = c4At(pntAt.i4X(), pntAt.i4Y());

    switch(m_eBitDepth)
    {
        case tCIDImage::EBitDepths::One :
        case tCIDImage::EBitDepths::Two :
        case tCIDImage::EBitDepths::Four :
        {
            //
            //  It's either palette or gray scale. Even if gray scale, we
            //  went ahead and set up a palette for it, so that values can be
            //  converted to RGB easily.
            //
            rgbToFill = palToUse.rgbAt(c4Clr);
            break;
        }

        case tCIDImage::EBitDepths::Eight :
        {
            // Either 8 bit palette/gray scale, or 24/32 bit color
            if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Palette))
            {
                // It's a palette/gray scale
                rgbToFill = palToUse.rgbAt(c4Clr);
            }
             else
            {
                // Look at the value as 24 or 32 as required
                if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
                    rgbToFill = *((tCIDLib::TCodedRGB*)&c4Clr);
                else
                    rgbToFill = *((tCIDLib::TRawRGB*)&c4Clr);
            }
            break;
        }

        case tCIDImage::EBitDepths::Five :
        {
            // It's 5 bit color components in the low 15 bits
            rgbToFill.Set
            (
                tCIDLib::TCard1((c4Clr >> 10) & 0x1F)
                , tCIDLib::TCard1((c4Clr >> 5) & 0x1F)
                , tCIDLib::TCard1(c4Clr & 0x1F)
            );
            break;
        }

        case tCIDImage::EBitDepths::Sixteen :
        {
            //
            //  It's gray scale. In this case, we couldn't create a palette,
            //  so just set each color component to the top 8 bits of the
            //  gray scale value.
            //
            //  If it has alpha, then take the top 8 bits of the alpha.
            //
            if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
            {
                const tCIDLib::TCard1 c1Alpha = tCIDLib::TCard1(c4Clr >> 8);
                const tCIDLib::TCard1 c1Val = tCIDLib::TCard1(c4Clr >> 24);
                rgbToFill.Set(c1Val, c1Val, c1Val, c1Alpha);
            }
             else
            {
                const tCIDLib::TCard1 c1Val = tCIDLib::TCard1(c4Clr >> 24);
                rgbToFill.Set(c1Val, c1Val, c1Val);
            }
            break;
        }
    };
}


tCIDLib::TVoid
TPixelArrayImpl::ReplaceClr(const   tCIDLib::TCard4 c4ToRep
                            , const tCIDLib::TCard4 c4RepClr)
{
    for (tCIDLib::TCard4 c4YInd = 0; c4YInd < m_c4Height; c4YInd++)
    {
        for (tCIDLib::TCard4 c4XInd = 0; c4XInd < m_c4Width; c4XInd++)
        {
            if (c4At(c4XInd, c4YInd) == c4ToRep)
                PutAt(c4RepClr, c4XInd, c4YInd);
        }
    }
}


//
//  These methods all reset our size or format, and deal with reallocating
//  the buffer if required. This first one does the real work, and the
//  others just call this first one.
//
tCIDLib::TVoid
TPixelArrayImpl::Reset( const   tCIDImage::EPixFmts     eFmt
                        , const tCIDImage::EBitDepths   eBitDepth
                        , const tCIDImage::ERowOrders   eRowOrder
                        , const TSize&                  szImage
                        , const tCIDLib::TBoolean       bForceRealloc)
{
    m_c4LineBytePadding = kCIDImage::c4LinePadding;
    m_c4Height = szImage.c4Height();
    m_c4Width = szImage.c4Width();
    m_eBitDepth = eBitDepth;
    m_eFmt = eFmt;
    m_eRowOrder = eRowOrder;

    // Validate input and calc transient values
    CalcTransients();

    //
    //  If our current buffer is too small for the image contents, or we
    //  we need to reallocate. Else, we can keep it.
    //
    if (bForceRealloc || (m_c4BufSz < m_c4ImageSz))
    {
        // Delete current buffer if adopting
        if (m_eAdoptOpt == tCIDLib::EAdoptOpts::Adopt)
            delete [] m_pc1Pixels;

        m_pc1Pixels = nullptr;
        m_c4BufSz = m_c4ImageSz;
        m_pc1Pixels = new tCIDLib::TCard1[m_c4BufSz];

        // We are adopting now since we created a new buffer
        m_eAdoptOpt = tCIDLib::EAdoptOpts::Adopt;
    }

    // Zero out the data buffer
    TRawMem::SetMemBuf(m_pc1Pixels, tCIDLib::TCard1(0), m_c4BufSz);
}

tCIDLib::TVoid
TPixelArrayImpl::Reset( const   tCIDImage::EPixFmts     eFmt
                        , const tCIDImage::EBitDepths   eBitDepth
                        , const tCIDLib::TBoolean       bForceRealloc)
{
    Reset(eFmt, eBitDepth, m_eRowOrder, TSize(m_c4Width, m_c4Height), bForceRealloc);
}

tCIDLib::TVoid TPixelArrayImpl::Reset(  const   TCIDImage&          imgSource
                                        , const tCIDLib::TBoolean   bForceRealloc)
{
    Reset
    (
        imgSource.ePixFmt()
        , imgSource.eBitDepth()
        , imgSource.eRowOrder()
        , imgSource.szImage()
        , bForceRealloc
    );
}

tCIDLib::TVoid TPixelArrayImpl::Reset(  const   TSize&              szNew
                                        , const tCIDLib::TBoolean   bForceRealloc)
{
    Reset(m_eFmt, m_eBitDepth, m_eRowOrder, szNew, bForceRealloc);
}


// Return our size as a TSize object
TSize TPixelArrayImpl::szImage() const
{
    return TSize(m_c4Width, m_c4Height);
}


//
//  Does a scaling of the alpha in the indicated direction from the starting
//  row/col to the ending row/col. The starting row/col is 100% scaled, i.e.
//  unchanged, and by the end row/col, it is scaled to zero, so fully
//  transparent. It's done in a non-linear way, so it ramps down quickly at
//  first, and then slower.
//
//  If asked, we'll pre-multiply the color components as well.
//
tCIDLib::TVoid
TPixelArrayImpl::ScaleAlpha(        tCIDLib::EDirs      eDir
                            , const tCIDLib::TCard4     c4StartInd
                            , const tCIDLib::TCard4     c4EndInd
                            , const tCIDLib::TBoolean   bPremultiply)
{
    // Copy to some locals since we mway want to flip them based on direction
    tCIDLib::TCard4 c4Low = c4StartInd;
    tCIDLib::TCard4 c4High = c4EndInd;

    //
    //  We use a sinusoidal function to get a smooth ramp, going more slowly
    //  at first and then more rapidly. We go from 0 to 90 degrees, i.e. one
    //  quadrant of the sine wave, or 0 to 1.57 radians.
    //
    //  For down and right, we go from the low to high line/col. For up and
    //  left we go from high to low, so we set offset values that we'll use
    //  to adjust the index and buffer pointer.
    //
    tCIDLib::TBoolean   bHorz = kCIDLib::True;
    tCIDLib::TFloat4    f4CurRad = 0;
    tCIDLib::TFloat4    f4IncPer = 1.57F / (c4High - c4Low);
    tCIDLib::TInt4      i4IndOfs = 1;
    tCIDLib::TInt4      i4BufOfs = m_c4LineWidth;
    switch(eDir)
    {
        case tCIDLib::EDirs::Up :
            bHorz = kCIDLib::False;
            c4Low = c4EndInd;
            c4High = c4StartInd;
            i4IndOfs = -1;
            i4BufOfs *= -1;
            break;

        case tCIDLib::EDirs::Down :
            bHorz = kCIDLib::False;
            break;

        case tCIDLib::EDirs::Left :
            c4Low = c4EndInd;
            c4High = c4StartInd;
            i4IndOfs = -1;
            i4BufOfs *= -1;
            break;

        case tCIDLib::EDirs::Right :
            break;
    };

    if (bHorz)
    {
        #pragma message(CodeReminder "Implement horz alpha scaling")
    }
     else
    {
        //
        //  We can optimize here since there are really only 3 scenarios, 8
        //  bit gray/alpha, 16 bit gray/alpha, and true alpha. So we do each
        //  one separately.
        //
        //  The algorith is:
        //
        //      get a pointer to the starting row
        //      for each line
        //          set the scaling factor
        //          get a pointer to the line buffer
        //          for each pixel
        //              scale each color component
        //         adjust row and index
        //      endfor
        //
        //  We optimize the highest value version, the 32 bit one, with an
        //  assembly language version.
        //
        if (tCIDLib::c4EnumOrd(m_eBitDepth) == 8)
        {
            if (m_eFmt == tCIDImage::EPixFmts::TrueAlpha)
            {
                tCIDLib::TCard1* pc1Row = m_pc1Pixels + (c4Low * m_c4LineWidth);
                while (c4Low != c4High)
                {
                    const tCIDLib::TCard1 c1Alpha = tCIDLib::TCard1
                    (
                        (1.0 - TMathLib::f4Sine(f4CurRad)) * 0xFF
                    );
                    f4CurRad += f4IncPer;

                    tCIDLib::TCard1* pc1Cur = pc1Row;
                    for (tCIDLib::TCard4 c4HInd = 0; c4HInd < m_c4Width; c4HInd++)
                    {
                        if (bPremultiply)
                        {
                            tCIDLib::TCard2 c2Val = *pc1Cur;
                            c2Val *= c1Alpha;
                            c2Val /= 0xFF;
                            *pc1Cur++ = tCIDLib::TCard1(c2Val);

                            c2Val = *pc1Cur;
                            c2Val *= c1Alpha;
                            c2Val /= 0xFF;
                            *pc1Cur++ = tCIDLib::TCard1(c2Val);

                            c2Val = *pc1Cur;
                            c2Val *= c1Alpha;
                            c2Val /= 0xFF;
                            *pc1Cur++ = tCIDLib::TCard1(c2Val);
                        }
                         else
                        {
                            pc1Cur += 3;
                        }

                        *pc1Cur++ = c1Alpha;
                    }
                    pc1Row += i4BufOfs;
                    c4Low += i4IndOfs;
                }
            }
             else if (m_eFmt == tCIDImage::EPixFmts::GrayAlpha)
            {
                tCIDLib::TCard1* pc1Row = m_pc1Pixels + (c4Low * m_c4LineWidth);
                while (c4Low != c4High)
                {
                    const tCIDLib::TCard1 c1Alpha = tCIDLib::TCard1
                    (
                        (1.0 - TMathLib::f4Sine(f4CurRad)) * 0xFF
                    );
                    f4CurRad += f4IncPer;
                    tCIDLib::TCard1* pc1Pix = pc1Row;
                    for (tCIDLib::TCard4 c4HInd = 0; c4HInd < m_c4Width; c4HInd++)
                    {
                        if (bPremultiply)
                        {
                            tCIDLib::TCard2 c2Val = *pc1Pix;
                            c2Val *= c1Alpha;
                            c2Val /= 0xFF;
                            *pc1Pix = tCIDLib::TCard1(c2Val);
                        }
                        pc1Pix++;
                        *pc1Pix++ = c1Alpha;
                    }
                    pc1Row += i4BufOfs;
                    c4Low += i4IndOfs;
                }
            }
        }
         else
        {
            tCIDLib::TCard2* pc2Row = reinterpret_cast<tCIDLib::TCard2*>
            (
                m_pc1Pixels + (c4Low * m_c4LineWidth)
            );
            while (c4Low != c4High)
            {
                const tCIDLib::TCard2 c2Alpha = tCIDLib::TCard2
                (
                    (1.0 - TMathLib::f4Sine(f4CurRad)) * 0xFFFF
                );
                f4CurRad += f4IncPer;
                tCIDLib::TCard2* pc2Pix = pc2Row;
                for (tCIDLib::TCard4 c4HInd = 0; c4HInd < m_c4Width; c4HInd++)
                {
                    if (bPremultiply)
                    {
                        tCIDLib::TCard4 c4Tmp = *pc2Pix;
                        c4Tmp *= c2Alpha;
                        c4Tmp /= 0xFFFF;
                        *pc2Pix = tCIDLib::TCard2(c4Tmp);
                    }
                    pc2Pix++;
                    *pc2Pix++ = c2Alpha;
                }
                pc2Row = tCIDLib::pOffsetNCPtr(pc2Row, i4BufOfs);
                c4Low += i4IndOfs;
            }
        }
    }
}


tCIDLib::TVoid TPixelArrayImpl::SetAll(const tCIDLib::TCard4 c4ToSet)
{
    for (tCIDLib::TCard4 c4VInd = 0; c4VInd < m_c4Height; c4VInd++)
    {
        for (tCIDLib::TCard4 c4HInd = 0; c4HInd < m_c4Width; c4HInd++)
            PutPixel(c4ToSet, c4HInd, c4VInd, CID_LINE);
    }
}


//
//  Sets all alpha channel bytes to a specific opacity, which is a very commonly
//  desired operation.
//
tCIDLib::TVoid TPixelArrayImpl::SetAllAlpha(const tCIDLib::TCard4 c4Alpha)
{
    for (tCIDLib::TCard4 c4VInd = 0; c4VInd < m_c4Height; c4VInd++)
    {
        tCIDLib::TCard1* pc1Row = m_pc1Pixels + (c4VInd * m_c4LineWidth);
        if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Color))
        {
            //
            //  It has to be true alpha, so move up 3 bytes and then do every
            //  4th byte.
            //
            tCIDLib::TCard1 c1Alpha = tCIDLib::TCard1(c4Alpha);
            pc1Row += 3;
            for (tCIDLib::TCard4 c4HInd = 0; c4HInd < m_c4Width; c4HInd++)
            {
                *pc1Row = c1Alpha;
                pc1Row += 4;
            }
        }
         else if (m_eBitDepth == tCIDImage::EBitDepths::Eight)
        {
            // Has to be 8 bit gray/alpha, so do every other byte
            tCIDLib::TCard1 c1Alpha = tCIDLib::TCard1(c4Alpha);
            pc1Row++;
            for (tCIDLib::TCard4 c4HInd = 0; c4HInd < m_c4Width; c4HInd++)
            {
                *pc1Row = c1Alpha;
                pc1Row += 2;
            }
        }
         else
        {
            // Has to be 16 bit gray/alpha
            tCIDLib::TCard2* pc2Row = reinterpret_cast<tCIDLib::TCard2*>(pc1Row);
            tCIDLib::TCard2 c2Alpha = tCIDLib::TCard2(c4Alpha);
            pc2Row++;
            for (tCIDLib::TCard4 c4HInd = 0; c4HInd < m_c4Width; c4HInd++)
            {
                *pc2Row = c2Alpha;
                pc2Row += 2;
            }
        }
    }
}


tCIDLib::TVoid
TPixelArrayImpl::SetAlphaAt(const   tCIDLib::TCard4 c4Alpha
                            , const TArea&          areaTar)
{
    tCIDLib::TCard4 c4PelsPer;
    if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Color))
        c4PelsPer = 4;
    else if (m_eBitDepth == tCIDImage::EBitDepths::Eight)
        c4PelsPer = 2;
    else
        c4PelsPer = 4;

    // The amount to move the left forward each time
    const tCIDLib::TCard4 c4LeftOfs(areaTar.i4X() * c4PelsPer);

    // We'll need the alpha in one or two byte form
    const tCIDLib::TCard1 c1Alpha = tCIDLib::TCard1(c4Alpha);
    const tCIDLib::TCard2 c2Alpha = tCIDLib::TCard2(c4Alpha);

    tCIDLib::TInt4 i4HInd;
    const tCIDLib::TInt4 i4HEnd = areaTar.i4Right();
    const tCIDLib::TInt4 i4VEnd = areaTar.i4Bottom();

    // The starting row pointer within the to target area
    tCIDLib::TCard1* pc1Row = m_pc1Pixels + (areaTar.i4Y() * m_c4LineWidth);

    for (tCIDLib::TInt4 i4VInd = areaTar.i4Y(); i4VInd <= i4VEnd; i4VInd++)
    {
        // Start at the left side of the target area
        i4HInd = areaTar.i4X();

        if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Color))
        {
            //
            //  It has to be true alpha, so move up to the left offset and then
            //  move up by three to get to the first alpha value.
            //
            tCIDLib::TCard1* pc1Fill = pc1Row + 3 + c4LeftOfs;
            for (; i4HInd <= i4HEnd; i4HInd++)
            {
                *pc1Fill = c1Alpha;
                pc1Fill += 4;
            }
        }
         else if (m_eBitDepth == tCIDImage::EBitDepths::Eight)
        {
            //
            //  Has to be 8 bit gray/alpha, so do every other byte. Move upt o the
            //  left offset plus one to get to the first alpha value.
            //
            tCIDLib::TCard1* pc1Fill = pc1Row + 1 + c4LeftOfs;

            for (; i4HInd <= i4HEnd; i4HInd++)
            {
                *pc1Fill = c1Alpha;
                pc1Fill += 2;
            }
        }
         else
        {
            //
            //  Has to be 16 bit gray/alpha, so move up to the left offset, and
            //  then two more to get to the first alpha value.
            //
            tCIDLib::TCard2* pc2Fill
            (
                reinterpret_cast<tCIDLib::TCard2*>(pc1Row + c4LeftOfs + 2)
            );

            for (; i4HInd <= i4HEnd; i4HInd++)
            {
                *pc2Fill = c2Alpha;
                pc2Fill += 2;
            }
        }

        // Move the row pointer forward to the next row
        pc1Row += m_c4LineWidth;
    }
}



//
//  Sets the alpha channel using the color/gray scale. If gray scale, the
//  alpha is just a copy of it. If it is color, then the alpha is the three
//  color channels combined and divided by 3. We obviously only support
//  formats with alpha here.
//
//  And we have another version that will go through the array and set the
//  alpha to transparent for any pixelst that match a passed transaprency
//  color. It only works for true/alpha.
//
tCIDLib::TVoid TPixelArrayImpl::SetAlphaFromColor()
{
    tCIDLib::TCard1* pc1Dest = m_pc1Pixels;
    if (tCIDLib::c4EnumOrd(m_eBitDepth) == 8)
    {
        if (m_eFmt == tCIDImage::EPixFmts::TrueAlpha)
        {
            for (tCIDLib::TCard4 c4RowInd = 0; c4RowInd < m_c4Height; c4RowInd++)
            {
                tCIDLib::TCard1* pc1Cur = pc1Dest;
                for (tCIDLib::TCard4 c4ColInd = 0; c4ColInd < m_c4Width; c4ColInd++)
                {
                    //
                    //  Add up the color components of the current color, divide by
                    //  three and store that as the alpha byte.
                    //
                    tCIDLib::TCard4 c4Clr = *pc1Cur++;
                    c4Clr += *pc1Cur++;
                    c4Clr += *pc1Cur++;
                    c4Clr /= 3;
                    *pc1Cur++ = tCIDLib::TCard1(c4Clr);
                }
                pc1Dest += m_c4LineWidth;
            }
        }
         else if (m_eFmt == tCIDImage::EPixFmts::GrayAlpha)
        {
            for (tCIDLib::TCard4 c4VInd = 0; c4VInd < m_c4Height; c4VInd++)
            {
                tCIDLib::TCard1* pc1Cur = pc1Dest;
                for (tCIDLib::TCard4 c4HInd = 0; c4HInd < m_c4Width; c4HInd++)
                {
                    const tCIDLib::TCard1 c1Color = *pc1Cur++;
                    *pc1Cur++ = c1Color;
                }
                pc1Dest += m_c4LineWidth;
            }
        }
         else
        {
            CIDAssert2(L"Unknown format for SetAlphaFromColor");
        }
    }
     else
    {
        for (tCIDLib::TCard4 c4VInd = 0; c4VInd < m_c4Height; c4VInd++)
        {
            tCIDLib::TCard2* pc2Ptr = reinterpret_cast<tCIDLib::TCard2*>(pc1Dest);
            for (tCIDLib::TCard4 c4HInd = 0; c4HInd < m_c4Width; c4HInd++)
            {
                const tCIDLib::TCard2 c2Color = *pc2Ptr++;
                *pc2Ptr++ = c2Color;
            }
            pc1Dest += m_c4LineWidth;
        }
    }
}

tCIDLib::TVoid TPixelArrayImpl::SetAlphaFromColor(const tCIDLib::TCard4 c4TransClr)
{
    CIDAssert
    (
        (m_eBitDepth == tCIDImage::EBitDepths::Eight)
        && (m_eFmt == tCIDImage::EPixFmts::TrueAlpha)
        , L"SetAlphaFromColor(trans) only supports true/alpha format"
    );
    const tCIDLib::TCard4 c4Cols = m_c4Width;
    const tCIDLib::TCard4 c4Rows = m_c4Height;
    const tCIDLib::TCard4 c4BytesPerRow = m_c4LineWidth;

    tCIDLib::TCard1* pc1Dest = m_pc1Pixels;
    for (tCIDLib::TCard4 c4RowInd = 0; c4RowInd < m_c4Height; c4RowInd++)
    {
        tCIDLib::TCard1* pc1Cur = pc1Dest;
        for (tCIDLib::TCard4 c4ColInd = 0; c4ColInd < m_c4Width; c4ColInd++)
        {
            tCIDLib::TCard4 c4Clr = *(pc1Cur + 2);
            c4Clr <<= 8;
            c4Clr |= *(pc1Cur + 1);
            c4Clr <<= 8;
            c4Clr |= *pc1Cur++;

            if (c4Clr == c4TransClr)
                *pc1Cur = 0;
            pc1Cur++;
        }
        pc1Dest += m_c4LineWidth;
    }
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
tCIDLib::TVoid TPixelArrayImpl::SetEmptyTrans()
{
    if (m_c4LineBytePadding == 4)
        TRawMem::SetMemBuf(m_pc1Pixels, 0UL, c4ImageSz() >> 2);
    else
        TRawMem::SetMemBuf(m_pc1Pixels, tCIDLib::TCard1(0), c4ImageSz());
}


//
//  This is only legal for true/alpha format.
//
tCIDLib::TVoid
TPixelArrayImpl::SetTextAlpha(  const   tCIDLib::TCard4     c4TextClr
                                , const tCIDLib::TCard4     c4TransClr
                                , const tCIDLib::TBoolean   bPremul)
{
    const tCIDLib::TCard1 c1AliasAlpha = 0x10;

    tCIDLib::TCard1* pc1Dest = m_pc1Pixels;
    for (tCIDLib::TCard4 c4RowInd = 0; c4RowInd < m_c4Height; c4RowInd++)
    {
        tCIDLib::TCard1* pc1Cur = pc1Dest;
        for (tCIDLib::TCard4 c4ColInd = 0; c4ColInd < m_c4Width; c4ColInd++)
        {
            //
            //  Get the current color out and see if it's the text color or
            //  trans color, or other.
            //
            tCIDLib::TCard4 c4Clr = *(pc1Cur + 2);
            c4Clr <<= 8;
            c4Clr |= *(pc1Cur + 1);
            c4Clr <<= 8;
            c4Clr |= *pc1Cur;

            if (c4Clr == c4TransClr)
            {
                // Set everything to zero
                *reinterpret_cast<tCIDLib::TCard4*>(pc1Cur) = 0;
                pc1Cur += 4;
            }
                else if (c4Clr == c4TextClr)
            {
                // Keep it as is, set the transparency to 0xFF
                *(pc1Cur + 3) = 0xFF;
                pc1Cur += 4;
            }
                else
            {
                // Set a low transparency and pre-multiply if asked to
                if (bPremul)
                {
                    tCIDLib::TCard2 c2Tmp = *pc1Cur;
                    c2Tmp *= c1AliasAlpha;
                    c2Tmp /= 0xFF;
                    *pc1Cur++ = tCIDLib::TCard1(c2Tmp);

                    c2Tmp = *pc1Cur;
                    c2Tmp *= c1AliasAlpha;
                    c2Tmp /= 0xFF;
                    *pc1Cur++ = tCIDLib::TCard1(c2Tmp);

                    c2Tmp = *pc1Cur;
                    c2Tmp *= c1AliasAlpha;
                    c2Tmp /= 0xFF;
                    *pc1Cur++ = tCIDLib::TCard1(c2Tmp);
                }
                    else
                {
                    pc1Cur += 3;
                }

                *pc1Cur = c1AliasAlpha;
            }
        }

        // Move to the next scan line
        pc1Dest += m_c4LineWidth;
    }
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
TPixelArrayImpl::StoreCompRow(          tCIDLib::TCard2* const  pc2ToStore
                                , const tCIDLib::TCard4         c4Row
                                , const tCIDLib::EClrComps      eComp)
{
    tCIDLib::TCard4 c4Shift;
    tCIDLib::TCard4 c2Cur;
    tCIDLib::TCard4 c4Cur;
    for (tCIDLib::TCard4 c4Col = 0; c4Col < m_c4Width; c4Col++)
    {
        c2Cur = pc2ToStore[c4Col];

        switch(m_eBitDepth)
        {
            case tCIDImage::EBitDepths::One :
            case tCIDImage::EBitDepths::Two :
            case tCIDImage::EBitDepths::Four :
            {
                //
                //  These will all be gray scale, since we don't support
                //  palette based images here. Since there is only one
                //  color component, we don't have to get the current
                //  value out, we can just store.
                //
                PutPixel(c2Cur, c4Col, c4Row, CID_LINE);
                break;
            }

            // It's 5 bit color compressed into a 16 bit value
            case tCIDImage::EBitDepths::Five :
            {
                // Get the current value out
                c4Cur = c4GetPixel(c4Col, c4Row, CID_LINE);
                if (eComp == tCIDLib::EClrComps::Red)
                    c4Shift = 10;
                else if (eComp == tCIDLib::EClrComps::Green)
                    c4Shift = 5;
                else
                    c4Shift = 0;

                c4Cur &= 0x1F << c4Shift;
                c4Cur |= (c2Cur & 0x1F) << c4Shift;

                PutPixel(c4Cur, c4Col, c4Row, CID_LINE);
                break;
            }

            case tCIDImage::EBitDepths::Eight :
            {
                //
                //  It'll be either color (with or without alpha), or gray
                //  scale (with or without alpha.)
                //
                c4Cur = c4GetPixel(c4Col, c4Row, CID_LINE);

                if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Color))
                {
                    tCIDLib::TCodedRGB* prgbCur = (tCIDLib::TCodedRGB*)&c4Cur;
                    if (eComp == tCIDLib::EClrComps::Red)
                        prgbCur->c1Red = tCIDLib::TCard1(c2Cur);
                    else if (eComp == tCIDLib::EClrComps::Green)
                        prgbCur->c1Green = tCIDLib::TCard1(c2Cur);
                    else if (eComp == tCIDLib::EClrComps::Blue)
                        prgbCur->c1Blue = tCIDLib::TCard1(c2Cur);
                    else if (eComp == tCIDLib::EClrComps::Alpha)
                        prgbCur->c1Alpha = tCIDLib::TCard1(c2Cur);
                }
                 else
                {
                    if (eComp == tCIDLib::EClrComps::Alpha)
                    {
                        c4Cur &= 0xFF00;
                        c4Cur |= tCIDLib::TCard1(c2Cur);
                    }
                     else
                    {
                        if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
                        {
                            c4Cur &= 0x00FF;
                            c4Cur |= c2Cur << 8;
                        }
                         else
                        {
                            c4Cur = tCIDLib::TCard1(c2Cur);
                        }
                    }
                }
                PutPixel(c4Cur, c4Col, c4Row, CID_LINE);
                break;
            }

            case tCIDImage::EBitDepths::Sixteen :
            {
                //
                //  Has to be gray scale, with or without alpha.
                //
                c4Cur = c4GetPixel(c4Col, c4Row, CID_LINE);
                if (eComp == tCIDLib::EClrComps::Alpha)
                {
                    c4Cur &= 0xFFFF0000;
                    c4Cur |= c2Cur;
                }
                 else
                {
                    if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
                    {
                        c4Cur &= 0xFFFF;
                        c4Cur |= (tCIDLib::TCard4(c2Cur) << 16);
                    }
                     else
                    {
                        c4Cur = c2Cur;
                    }
                }
                PutPixel(c4Cur, c4Col, c4Row, CID_LINE);
                break;
            }

            default :
                facCIDImage().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kImgErrs::errcImg_BadBitDepth
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Internal
                    , TInteger(tCIDLib::i4EnumOrd(m_eBitDepth))
                );
                break;
        };
    }
}


tCIDLib::TVoid TPixelArrayImpl::ZeroAll()
{
    //
    //  We only need to zero the used part of the buffer, i.e. the part
    //  that contains the iamge data. The buffer may be larger than that.
    //
    TRawMem::SetMemBuf(m_pc1Pixels, tCIDLib::TCard1(0), m_c4ImageSz);
}


// ---------------------------------------------------------------------------
//  TPixelArray: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TPixelArrayImpl::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Look for the start object marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (!c2FmtVersion || (c2FmtVersion > CIDImage_PixelArrayImpl::c2FmtVersion))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , clsThis()
        );
    }

    //
    //  Pull in the new size and bit depth info. NOTE that since we set
    //  the bit depth enum to it's actual numeric value, it doesn't matter
    //  if we add new depths, old ones will still come in ok. If we remove
    //  values, then it won't be a valid value, but we'll fix that below.
    //
    strmToReadFrom  >> m_c4Height
                    >> m_c4Width
                    >> m_eBitDepth;

    //
    //  If version 2 or beyond, then read in the V2 stuff . Else, init
    //  to what it would have to be given the old definitions.
    //
    if (c2FmtVersion >= 2)
    {
        strmToReadFrom  >> m_eFmt
                        >> m_eRowOrder;
    }
     else
    {
        //
        //  Treat it as a number since we have removed values. We only have
        //  to deal with the depths that were previously supported.
        //
        switch(tCIDLib::c4EnumOrd(m_eBitDepth))
        {
            case 1 :
            case 2 :
            case 4 :
            case 8 :
                // Had to be color palette based
                m_eFmt = tCIDImage::EPixFmts::ClrPal;
                break;

            case 24 :
                // Now this is true color with 8 bits per color
                m_eFmt = tCIDImage::EPixFmts::TrueClr;
                m_eBitDepth = tCIDImage::EBitDepths::Eight;
                break;

            case 32 :
                // Now this is true color with 8 bits per color/alpha
                m_eFmt = tCIDImage::EPixFmts::TrueAlpha;
                m_eBitDepth = tCIDImage::EBitDepths::Eight;
                break;
        };
        m_eRowOrder = tCIDImage::ERowOrders::TopDown;
    }

    // Validate the input and calc the transients
    CalcTransients();

    //
    //  If the buffer would not be the same size, then we cannot keep the
    //  current one. So delete it and create another one.
    //
    //  In this case, we don't keep our current buffer, even if it is large
    //  enough to hold the image data!
    //
    if (m_c4ImageSz != m_c4BufSz)
    {
        // Delete current buffer if adopting
        if (m_eAdoptOpt == tCIDLib::EAdoptOpts::Adopt)
            delete [] m_pc1Pixels;
        m_pc1Pixels = nullptr;

        // And create our new buffer
        m_c4BufSz = m_c4ImageSz;
        m_pc1Pixels = new tCIDLib::TCard1[m_c4BufSz];

        // And we are adopting now since we created a new buffer
        m_eAdoptOpt = tCIDLib::EAdoptOpts::Adopt;
    }

    // Stream in the pixels
    strmToReadFrom.ReadArray(m_pc1Pixels, m_c4ImageSz);

    // And it should end up with an end object marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TPixelArrayImpl::StreamTo(TBinOutStream& strmToWriteTo) const
{
    //
    //  Start with a start object, and a format version so that we can later
    //  auto upgrade this data.
    //
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDImage_PixelArrayImpl::c2FmtVersion
                    << m_c4Height
                    << m_c4Width
                    << m_eBitDepth

                    // Version 2 stuff
                    << m_eFmt
                    << m_eRowOrder;

    strmToWriteTo.WriteArray(m_pc1Pixels, m_c4ImageSz);

    // End it up with an end object marker
    strmToWriteTo << tCIDLib::EStreamMarkers::EndObject;
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
TPixelArrayImpl::c4GetBoundaryPixel(const   tCIDLib::TInt4  i4XPos
                                    , const tCIDLib::TInt4  i4YPos
                                    , const tCIDLib::TCard4 c4Line) const
{
    if ((i4XPos < 0) || (i4YPos < 0))
        return 0;
    if ((i4XPos >= tCIDLib::TInt4(m_c4Width)) || (i4YPos >= tCIDLib::TInt4(m_c4Height)))
        return 0;
    return c4GetPixel(tCIDLib::TCard4(i4XPos), tCIDLib::TCard4(i4YPos), c4Line);
}


//
//  Gets a pixel out at that indicated coordinates. It goes by the current
//  pixel format data to know how to pull out the data. No matter what the
//  format, it's returned in a TCard4 value.
//
tCIDLib::TCard4
TPixelArrayImpl::c4GetPixel(const   tCIDLib::TCard4 c4XPos
                            , const tCIDLib::TCard4 c4YPos
                            , const tCIDLib::TCard4 c4Line) const
{
    if ((c4XPos >= m_c4Width)
    ||  (c4YPos >= m_c4Height))
    {
        // NOTE: We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , c4Line
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TSize(c4XPos, c4YPos)
            , clsThis()
            , TSize(m_c4Width, m_c4Height)
        );
    }

    // Calculate the offset of the line its in
    const tCIDLib::TCard4 c4LineOfs = m_c4LineWidth * c4YPos;

    tCIDLib::TCard4             c4Ret;
    tCIDLib::TCard4             c4Byte;
    tCIDLib::TCard4             c4Bit;
    TPixelArray::TRGBTriple*    prgbTmp;
    TPixelArray::TRGBQuad*      prgbTmp2;
    switch(m_eBitDepth)
    {
        case tCIDImage::EBitDepths::One :
        {
            c4Byte = c4XPos / 8;
            c4Bit = 7 - (c4XPos % 8);
            c4Ret = (m_pc1Pixels[c4LineOfs + c4Byte] & (0x1 << c4Bit)) ? 1 : 0;
            break;
        }

        case tCIDImage::EBitDepths::Two :
        {
            c4Byte = c4XPos / 4;
            c4Bit = (3 - (c4XPos % 4)) << 1;
            c4Ret = ((m_pc1Pixels[c4LineOfs + c4Byte] & (0x3 << c4Bit)) >> c4Bit);
            break;
        }

        case tCIDImage::EBitDepths::Four :
        {
            c4Byte = c4XPos / 2;
            c4Bit = (c4XPos & 0x1) ? 0 : 4;
            c4Ret = ((m_pc1Pixels[c4LineOfs + c4Byte] & (0xF << c4Bit)) >> c4Bit);
            break;
        }

        case tCIDImage::EBitDepths::Five :
        {
            // It's 3 color components in a 16 bit value. Can't have alpha
            c4Ret = *reinterpret_cast<tCIDLib::TCard2*>
            (
                &m_pc1Pixels[c4LineOfs + (c4XPos * 2)]
            );
            break;
        }

        case tCIDImage::EBitDepths::Eight :
        {
            //
            //  For either gray scale or 8 bit palette, we just pull out the
            //  the next byte. If alpha, then get that next. For color, it's
            //  24 or 32 bit.
            //
            if ((m_eFmt == tCIDImage::EPixFmts::TrueClr)
            ||  (m_eFmt == tCIDImage::EPixFmts::TrueAlpha))
            {
                if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
                {
                    prgbTmp2 = reinterpret_cast<TPixelArray::TRGBQuad*>
                    (
                        &m_pc1Pixels[c4LineOfs + (c4XPos * 4)]
                    );
                    c4Ret = prgbTmp2->c1Alpha;
                    c4Ret <<= 8;
                    c4Ret |= prgbTmp2->c1Blue;
                    c4Ret <<= 8;
                    c4Ret |= prgbTmp2->c1Green;
                    c4Ret <<= 8;
                    c4Ret |= prgbTmp2->c1Red;
                }
                 else
                {
                    prgbTmp = reinterpret_cast<TPixelArray::TRGBTriple*>
                    (
                        &m_pc1Pixels[c4LineOfs + (c4XPos * 3)]
                    );
                    c4Ret = prgbTmp->c1Blue;
                    c4Ret <<= 8;
                    c4Ret |= prgbTmp->c1Green;
                    c4Ret <<= 8;
                    c4Ret |= prgbTmp->c1Red;
                }
            }
             else
            {
                if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
                {
                    c4Ret = m_pc1Pixels[c4LineOfs + (c4XPos << 1)];
                    if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
                    {
                        c4Ret <<= 8;
                        c4Ret |= m_pc1Pixels[c4LineOfs + (c4XPos << 1) + 1];
                    }
                }
                 else
                {
                    c4Ret = m_pc1Pixels[c4LineOfs + c4XPos];
                    if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
                    {
                        c4Ret <<= 8;
                        c4Ret |= m_pc1Pixels[c4LineOfs + c4XPos + 1];
                    }
                }
            }
            break;
        }

        case tCIDImage::EBitDepths::Sixteen:
        {
            //
            //  It can only be gray scale. So get a 16 bit value out. If
            //  alpha, get another one.
            //
            if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
            {
                c4Ret = m_pc1Pixels[c4LineOfs + (c4XPos << 2)];
                c4Ret <<= 8;
                c4Ret |= m_pc1Pixels[c4LineOfs + (c4XPos << 2) + 1];
                c4Ret <<= 8;
                c4Ret |= m_pc1Pixels[c4LineOfs + (c4XPos << 2) + 2];
                c4Ret <<= 8;
                c4Ret |= m_pc1Pixels[c4LineOfs + (c4XPos << 2) + 3];
            }
             else
            {
                c4Ret = m_pc1Pixels[c4LineOfs + (c4XPos << 1)];
                c4Ret <<= 8;
                c4Ret |= m_pc1Pixels[c4LineOfs + (c4XPos << 1) + 1];
            }
            break;
        }

        default :
            facCIDImage().ThrowErr
            (
                CID_FILE
                , c4Line
                , kImgErrs::errcImg_BadBitDepth
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , TInteger(tCIDLib::i4EnumOrd(m_eBitDepth))
            );

            // Make compiler happy
            c4Ret = 0;
            break;
    }
    return c4Ret;
}


//
//  We update the line width (bytes per line, including padding) and the
//  overall image size, based on the current width/height and format.
//
tCIDLib::TVoid TPixelArrayImpl::CalcTransients()
{
    // This will validate the fmt/depth stuff for us as well
    m_c4LineWidth = TPixelArray::c4CalcLineWidth
    (
        m_c4Width, m_eFmt, m_eBitDepth, kCIDImage::c4LinePadding
    );
    m_c4ImageSz = m_c4LineWidth * m_c4Height;
}


//
//  Used in Bicubic spline based scaling. Converts our array into an array
//  of coefficients.
//
tCIDLib::TVoid
TPixelArrayImpl::ConvertToCoefs(        tCIDLib::TFloat8* const pf8ToCvt
                                , const tCIDLib::TCard4         c4SampleCnt
                                , const tCIDLib::TFloat8* const pf8Poles
                                , const tCIDLib::TCard4         c4PoleCnt
                                , const tCIDLib::TFloat8        f8Tolerance) const
{
    tCIDLib::TFloat8 f8Lambda = 1.0;

    // Special case required by mirror boundaries
    if (c4SampleCnt == 1)
        return;

    // Compute the overall gain
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4PoleCnt; c4Index++)
    {
        f8Lambda = f8Lambda * (1.0 - pf8Poles[c4Index])
                    * (1.0 - (1.0 / pf8Poles[c4Index]));
    }

    // Apply the gain
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SampleCnt; c4Index++)
        pf8ToCvt[c4Index] *= f8Lambda;

    // Loop over all poles
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4PoleCnt; c4Index++)
    {
        // Causal initialization
        pf8ToCvt[0] = f8InitCausalCoefs
        (
            pf8ToCvt, c4SampleCnt, pf8Poles[c4Index], f8Tolerance
        );

        // Causal recursion
        for (tCIDLib::TCard4 c4RInd = 1; c4RInd < c4SampleCnt; c4RInd++)
            pf8ToCvt[c4RInd] += pf8Poles[c4Index] * pf8ToCvt[c4RInd - 1];

        // Anticausal initialization
        pf8ToCvt[c4SampleCnt - 1] = f8InitAntiCausalCoefs
        (
            pf8ToCvt, c4SampleCnt, pf8Poles[c4Index]
        );

        // Anticausal recursion
        const tCIDLib::TInt4 i4ARCount = tCIDLib::TInt4(c4SampleCnt - 2);
        for (tCIDLib::TInt4 i4RInd = i4ARCount; i4RInd >= 0; i4RInd--)
        {
            pf8ToCvt[i4RInd] = pf8Poles[c4Index]
                            * (pf8ToCvt[i4RInd + 1L] - pf8ToCvt[i4RInd]);
        }
    }
}


//
//  Used in Bicubic spline scaling. Interpolates a point based on the incoming
//  coefficient array.
//
tCIDLib::TFloat8
TPixelArrayImpl::f8InterpolatePoint(const   tCIDLib::TFloat4* const pf4Coefs
                                    , const tCIDLib::TCard4         c4Width
                                    , const tCIDLib::TCard4         c4Height
                                    , const tCIDLib::TFloat8        f8X
                                    , const tCIDLib::TFloat8        f8Y
                                    , const tCIDLib::TCard4         c4Degree) const
{
    // Compute the interpolation indexes
    tCIDLib::TInt4 i4I;
    tCIDLib::TInt4 i4J;
    tCIDLib::TInt4 ai4XIndex[6];
    tCIDLib::TInt4 ai4YIndex[6];

    const tCIDLib::TInt4 i4HalfDegree = tCIDLib::TInt4(c4Degree >> 1);
    if (c4Degree & 1)
    {
        i4I = tCIDLib::TInt4(TMathLib::f8Floor(f8X)) - i4HalfDegree;
        i4J = tCIDLib::TInt4(TMathLib::f8Floor(f8Y)) - i4HalfDegree;
    }
     else
    {
        i4I = tCIDLib::TInt4(TMathLib::f8Floor(f8X + 0.5)) - i4HalfDegree;
        i4J = tCIDLib::TInt4(TMathLib::f8Floor(f8Y + 0.5)) - i4HalfDegree;
    }

    for (tCIDLib::TCard4 c4Index = 0; c4Index <= c4Degree; c4Index++)
    {
        ai4XIndex[c4Index] = i4I++;
        ai4YIndex[c4Index] = i4J++;
    }

    // Do the  weights
    tCIDLib::TFloat8 af8XWeight[6];
    tCIDLib::TFloat8 af8YWeight[6];
    tCIDLib::TFloat8 f8Tmp;
    switch(c4Degree)
    {
        case 2 :
        {
            f8Tmp = f8X - tCIDLib::TFloat8(ai4XIndex[1]);
            af8XWeight[1] = 3.0 / 4.0 - f8Tmp * f8Tmp;
            af8XWeight[2] = 1.0 / 2.0 * (f8Tmp - af8XWeight[1] + 1.0);
            af8XWeight[0] = 1.0 - af8XWeight[1] - af8XWeight[2];

            f8Tmp = f8Y - tCIDLib::TFloat8(ai4YIndex[1]);
            af8YWeight[1] = 3.0 / 4.0 - (f8Tmp * f8Tmp);
            af8YWeight[2] = (1.0 / 2.0) * (f8Tmp - af8YWeight[1] + 1.0);
            af8YWeight[0] = 1.0 - af8YWeight[1] - af8YWeight[2];
            break;
        }

        case 3 :
        {
            f8Tmp = f8X - ai4XIndex[1];
            af8XWeight[3] = (1.0 / 6.0) * f8Tmp * f8Tmp * f8Tmp;
            af8XWeight[0] = (1.0 / 6.0) + (1.0 / 2.0) * f8Tmp
                            * (f8Tmp - 1.0) - af8XWeight[3];
            af8XWeight[2] = f8Tmp + af8XWeight[0] - 2.0 * af8XWeight[3];
            af8XWeight[1] = 1.0 - af8XWeight[0] - af8XWeight[2] - af8XWeight[3];

            f8Tmp = f8Y - ai4YIndex[1];
            af8YWeight[3] = (1.0 / 6.0) * f8Tmp * f8Tmp * f8Tmp;
            af8YWeight[0] = (1.0 / 6.0) + (1.0 / 2.0) * f8Tmp
                            * (f8Tmp - 1.0) - af8YWeight[3];
            af8YWeight[2] = f8Tmp + af8YWeight[0] - 2.0 * af8YWeight[3];
            af8YWeight[1] = 1.0 - af8YWeight[0] - af8YWeight[2] - af8YWeight[3];
            break;
        }

        case 4 :
        {
            f8Tmp = f8X - tCIDLib::TFloat8(ai4XIndex[2]);
            const tCIDLib::TFloat8 f8W2 = f8Tmp * f8Tmp;
            tCIDLib::TFloat8 f8T = (1.0 / 6.0) * f8W2;
            af8XWeight[0] = 1.0 / 2.0 - f8Tmp;
            af8XWeight[0] *= af8XWeight[0];
            af8XWeight[0] *= (1.0 / 24.0) * af8XWeight[0];
            tCIDLib::TFloat8 f8T0 = f8Tmp * (f8T - 11.0 / 24.0);
            tCIDLib::TFloat8 f8T1 = 19.0 / 96.0 + f8W2 * (1.0 / 4.0 - f8T);
            af8XWeight[1] = f8T1 + f8T0;
            af8XWeight[3] = f8T1 - f8T0;
            af8XWeight[4] = af8XWeight[0] + f8T0 + (1.0 / 2.0) * f8Tmp;
            af8XWeight[2] = 1.0 - af8XWeight[0] - af8XWeight[1]
                            - af8XWeight[3] - af8XWeight[4];

            f8Tmp = f8Y - tCIDLib::TFloat8(ai4YIndex[2]);
            f8T = (1.0 / 6.0) * f8W2;
            af8YWeight[0] = 1.0 / 2.0 - f8Tmp;
            af8YWeight[0] *= af8YWeight[0];
            af8YWeight[0] *= (1.0 / 24.0) * af8YWeight[0];
            f8T0 = f8Tmp * (f8T - 11.0 / 24.0);
            f8T1 = 19.0 / 96.0 + f8W2 * (1.0 / 4.0 - f8T);
            af8YWeight[1] = f8T1 + f8T0;
            af8YWeight[3] = f8T1 - f8T0;
            af8YWeight[4] = af8YWeight[0] + f8T0 + (1.0 / 2.0) * f8Tmp;
            af8YWeight[2] = 1.0 - af8YWeight[0] - af8YWeight[1]
                            - af8YWeight[3] - af8YWeight[4];
            break;
        }

        case 5 :
        {
            f8Tmp = f8X - tCIDLib::TFloat8(ai4XIndex[2]);
            tCIDLib::TFloat8 f8W2 = f8Tmp * f8Tmp;
            af8XWeight[5] = (1.0 / 120.0) * f8Tmp * f8W2 * f8W2;
            f8W2 -= f8Tmp;
            tCIDLib::TFloat8 f8W4 = f8W2 * f8W2;
            f8Tmp -= 1.0 / 2.0;
            tCIDLib::TFloat8 f8T = f8W2 * (f8W2 - 3.0);
            af8XWeight[0] = (1.0 / 24.0)
                            * (1.0 / 5.0 + f8W2 + f8W4) - af8XWeight[5];
            tCIDLib::TFloat8 f8T0 = (1.0 / 24.0)
                                    * (f8W2 * (f8W2 - 5.0) + 46.0 / 5.0);
            tCIDLib::TFloat8 f8T1 = (-1.0 / 12.0) * f8Tmp * (f8T + 4.0);
            af8XWeight[2] = f8T0 + f8T1;
            af8XWeight[3] = f8T0 - f8T1;
            f8T0 = (1.0 / 16.0) * (9.0 / 5.0 - f8T);
            f8T1 = (1.0 / 24.0) * f8Tmp * (f8W4 - f8W2 - 5.0);
            af8XWeight[1] = f8T0 + f8T1;
            af8XWeight[4] = f8T0 - f8T1;

            f8Tmp = f8Y - tCIDLib::TFloat8(ai4YIndex[2]);
            f8W2 = f8Tmp * f8Tmp;
            af8YWeight[5] = (1.0 / 120.0) * f8Tmp * f8W2 * f8W2;
            f8W2 -= f8Tmp;
            f8W4 = f8W2 * f8W2;
            f8Tmp -= 1.0 / 2.0;
            f8T = f8W2 * (f8W2 - 3.0);
            af8YWeight[0] = (1.0 / 24.0)
                            * (1.0 / 5.0 + f8W2 + f8W4) - af8YWeight[5];
            f8T0 = (1.0 / 24.0) * (f8W2 * (f8W2 - 5.0) + 46.0 / 5.0);
            f8T1 = (-1.0 / 12.0) * f8Tmp * (f8T + 4.0);
            af8YWeight[2] = f8T0 + f8T1;
            af8YWeight[3] = f8T0 - f8T1;
            f8T0 = (1.0 / 16.0) * (9.0 / 5.0 - f8T);
            f8T1 = (1.0 / 24.0) * f8Tmp * (f8W4 - f8W2 - 5.0);
            af8YWeight[1] = f8T0 + f8T1;
            af8YWeight[4] = f8T0 - f8T1;
            break;
        }

        default :
            // <TBD> Log somethign here if debugging
            #if CID_DEBUG_ON
            #endif
            break;

    };

    // Apply the mirror boundary conditions
    const tCIDLib::TInt4 i4H2 = tCIDLib::TInt4(c4Height * 2) - 2;
    const tCIDLib::TInt4 i4W2 = tCIDLib::TInt4(c4Width * 2) - 2;
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind <= c4Degree; c4Ind++)
    {
        if (c4Width == 1)
        {
            ai4XIndex[c4Ind] = 0;
        }
         else if (ai4XIndex[c4Ind] < 0)
        {
            ai4XIndex[c4Ind] = -ai4XIndex[c4Ind]
                                - i4W2 * (-ai4XIndex[c4Ind] / i4W2);
        }
         else
        {
            ai4XIndex[c4Ind] = ai4XIndex[c4Ind]
                               - i4W2 * (ai4XIndex[c4Ind] / i4W2);
        }

        if (tCIDLib::TInt4(c4Width) <= ai4XIndex[c4Ind])
            ai4XIndex[c4Ind] = i4W2 - ai4XIndex[c4Ind];

        if (c4Height == 1)
        {
            ai4YIndex[c4Ind] = 0;
        }
         else if (ai4YIndex[c4Ind] < 0)
        {
            ai4YIndex[c4Ind] = -ai4YIndex[c4Ind]
                               - i4H2 * (-ai4YIndex[c4Ind] / i4H2);
        }
         else
        {
            ai4YIndex[c4Ind] = ai4YIndex[c4Ind]
                               - i4H2 * (ai4YIndex[c4Ind] / i4H2);
        }

        if (tCIDLib::TInt4(c4Height) <= ai4YIndex[c4Ind])
            ai4YIndex[c4Ind] = i4H2 - ai4YIndex[c4Ind];
    }

    // Interpolate the new value
    tCIDLib::TFloat8 f8Interp = 0.0;
    for (tCIDLib::TCard4 c4DInd = 0; c4DInd <= c4Degree; c4DInd++)
    {
        const tCIDLib::TFloat4* pf4Line = pf4Coefs + (ai4YIndex[c4DInd] * c4Width);
        f8Tmp = 0.0;
        for (tCIDLib::TCard4 c4D2Ind = 0; c4D2Ind <= c4Degree; c4D2Ind++)
            f8Tmp += af8XWeight[c4D2Ind] * pf4Line[ai4XIndex[c4D2Ind]];
        f8Interp += af8YWeight[c4DInd] * f8Tmp;
    }
    return f8Interp;
}


tCIDLib::TFloat8
TPixelArrayImpl::f8InitAntiCausalCoefs(         tCIDLib::TFloat8* const pf8ToCvt
                                    , const tCIDLib::TCard4         c4SampleCnt
                                    , const tCIDLib::TFloat8        f8Pole) const
{
    return
    (
        (f8Pole / ((f8Pole * f8Pole) - 1.0))
        * ((f8Pole * pf8ToCvt[c4SampleCnt - 2]) + pf8ToCvt[c4SampleCnt - 1])
    );
}


tCIDLib::TFloat8
TPixelArrayImpl::f8InitCausalCoefs(         tCIDLib::TFloat8* const pf8ToCvt
                                    , const tCIDLib::TCard4         c4SampleCnt
                                    , const tCIDLib::TFloat8        f8Pole
                                    , const tCIDLib::TFloat8        f8Tolerance) const
{
    tCIDLib::TInt4 i4Horizon = tCIDLib::TInt4(c4SampleCnt);
    if (f8Tolerance > 0.0)
    {
        i4Horizon = tCIDLib::TInt4
        (
            TMathLib::f8Ceil
            (
                TMathLib::f8Log(f8Tolerance)
                / TMathLib::f8Log(TMathLib::f8Abs(f8Pole))
            )
        );
    }

    tCIDLib::TFloat8 f8Sum;
    if (i4Horizon < tCIDLib::TInt4(c4SampleCnt))
    {
        // Do an optimized loop in this case
        tCIDLib::TFloat8 f8Zn = f8Pole;
        f8Sum = pf8ToCvt[0];
        for (tCIDLib::TCard4 c4Ind = 1; c4Ind < tCIDLib::TCard4(i4Horizon); c4Ind++)
        {
            f8Sum += f8Zn * pf8ToCvt[c4Ind];
            f8Zn *= f8Pole;
        }
    }
     else
    {
        // Do the full loop
        tCIDLib::TFloat8 f8Zn = f8Pole;
        tCIDLib::TFloat8 f8Iz = 1.0 / f8Pole;
        tCIDLib::TFloat8 f8Z2n = TMathLib::f8Power
        (
            f8Pole, tCIDLib::TFloat8(c4SampleCnt - 1)
        );
        f8Sum = pf8ToCvt[0] + f8Z2n * pf8ToCvt[c4SampleCnt - 1];
        f8Z2n *= f8Z2n * f8Iz;
        for (tCIDLib::TCard4 c4Ind = 1; c4Ind <= c4SampleCnt - 2; c4Ind++)
        {
            f8Sum += (f8Zn + f8Z2n) * pf8ToCvt[c4Ind];
            f8Zn *= f8Pole;
            f8Z2n *= f8Iz;
        }
        f8Sum /= (1.0 - f8Zn * f8Zn);
    }
    return f8Sum;
}


//
//  This method does the interpolation of the whole image for a single color
//  component. We get the built coeficients array, and the target pixel array
//  to fill in, along with the spline degree and the maximum value our current
//  component can have, so that we can clip the values correctly.
//
//  The caller provides us with a buffer of width TCard2 values (the max
//  size of a single component is 16 bits, for 16 bit gray scale and the same
//  with alpha.)
//
tCIDLib::TVoid
TPixelArrayImpl::InterpComp(const   tCIDLib::EClrComps      eComp
                            , const tCIDLib::TFloat4* const pf4Coefs
                            ,       tCIDLib::TCard2* const  pc2LoadBuf
                            ,       TPixelArrayImpl&        pixaiToFill
                            , const tCIDLib::TCard4         c4Degree
                            , const tCIDLib::TCard2         c2MaxVal) const
{
    const tCIDImage::EBitDepths eDepth = pixaiToFill.eBitDepth();

    //
    //  We have to scale the outgoing coordinates to the incoming coordinate
    //  system, so that when we interpolate against the data (which was built
    //  for the incoming original data), we'll pass legal coordinates. So
    //  calculate scaling factors for the two axes.
    //
    const tCIDLib::TCard4 c4TH = pixaiToFill.szImage().c4Height();
    const tCIDLib::TCard4 c4TW = pixaiToFill.szImage().c4Width();
    const tCIDLib::TFloat8 f8XScale = tCIDLib::TFloat8(m_c4Width)
                                      / tCIDLib::TFloat8(c4TW);
    const tCIDLib::TFloat8 f8YScale = tCIDLib::TFloat8(m_c4Height)
                                      / tCIDLib::TFloat8(c4TH);

    // Calc the amount required to get to the middle of the source pixel
    const tCIDLib::TFloat8 f8XOfs = 0.5 * f8XScale;
    const tCIDLib::TFloat8 f8YOfs = 0.5 * f8YScale;

    // Get a float8 version of the max value, to avoid conversions below
    const tCIDLib::TFloat8 f8MaxVal = tCIDLib::TFloat8(c2MaxVal);

    //
    //  Ok, now run through all the pixels and interpolate the new value and
    //  store it in the outboard pixel array. We build up a scan line at a
    //  time and then ask the pixel array to suck them back in and store them
    //  based on it's format.
    //
    tCIDLib::TFloat8 f8Cur;
    for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4TH; c4YInd++)
    {
        tCIDLib::TFloat8 f8YInd = (tCIDLib::TFloat8(c4YInd) + 0.5) * f8YScale;
        if (f8YInd >= m_c4Height)
            f8YInd = m_c4Height - 1.0;

        for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4TW; c4XInd++)
        {
            tCIDLib::TFloat8 f8XInd = (tCIDLib::TFloat8(c4XInd) + 0.5) * f8XScale;
            if (f8XInd >= m_c4Width)
                f8XInd = m_c4Width - 1.0;

            // Interpolate the new point
            f8Cur = f8InterpolatePoint
            (
                pf4Coefs
                , m_c4Width
                , m_c4Height
                , f8XInd
                , f8YInd
                , c4Degree
            );

            // And clip it and store in the load buffer
            if (f8Cur < 0.0)
            {
                pc2LoadBuf[c4XInd] = 0;
            }
             else
            {
                f8Cur = TMathLib::f8Floor(f8Cur + 0.5);
                if (f8Cur > f8MaxVal)
                    pc2LoadBuf[c4XInd] = c2MaxVal;
                else
                    pc2LoadBuf[c4XInd] = tCIDLib::TCard2(f8Cur);
            }
        }
        pixaiToFill.StoreCompRow(pc2LoadBuf, c4YInd, eComp);
    }
}


tCIDLib::TVoid
TPixelArrayImpl::LoadCoefs( const   tCIDLib::EClrComps      eComp
                            ,       tCIDLib::TFloat4* const pf4ToFill
                            ,       tCIDLib::TFloat8* const pf8Row
                            ,       tCIDLib::TFloat8* const pf8Col
                            , const tCIDLib::TCard4         c4Degree
                            , const TClrPalette&            palToUse) const
{
    CIDAssert
    (
        ((c4Degree > 0) && (c4Degree < 6))
        , L"The scaling degree must be from 2 to 5"
    );

    // Set up the poles info
    tCIDLib::TCard4 c4PoleCnt;
    tCIDLib::TFloat8 af8Poles[2];

    switch(c4Degree)
    {
        case 2 :
            c4PoleCnt = 1;
            af8Poles[0] = TMathLib::f8SqrRoot(8.0) - 3.0;
            break;

        case 3 :
            c4PoleCnt = 1;
            af8Poles[0] = TMathLib::f8SqrRoot(3.0) - 2.0;
            break;

        case 4 :
            c4PoleCnt = 2;
            af8Poles[0] = TMathLib::f8SqrRoot
            (
                664.0 - TMathLib::f8SqrRoot(438976.0)
            ) + TMathLib::f8SqrRoot(304.0) - 19.0;

            af8Poles[1] = TMathLib::f8SqrRoot
            (
                664.0 + TMathLib::f8SqrRoot(438976.0)
            ) - TMathLib::f8SqrRoot(304.0) - 19.0;
            break;

        case 5 :
            c4PoleCnt = 2;
            af8Poles[0] = TMathLib::f8SqrRoot
            (
                135.0 / 2.0 - TMathLib::f8SqrRoot(17745.0 / 4.0)
            ) + TMathLib::f8SqrRoot(105.0 / 4.0) - 13.0 / 2.0;

            af8Poles[1] = TMathLib::f8SqrRoot
            (
                135.0 / 2.0 + TMathLib::f8SqrRoot(17745.0 / 4.0)
            ) - TMathLib::f8SqrRoot(105.0 / 4.0) - 13.0 / 2.0;
            break;

        default :
            break;
    };

    //
    //  Do the vertical pass, in which we get each row and convert them
    //  and put them into the pf4ToFill array.
    //
    tCIDLib::TFloat4* pf4Tar = pf4ToFill;
    for (tCIDLib::TCard4 c4YInd = 0; c4YInd < m_c4Height; c4YInd++)
    {
        //
        //  Load up a row from our pixel array, converted to floating point
        //  values, then convert that row into coefficients, and put them
        //  into the output array.
        //
        LoadCompRow(pf8Row, c4YInd, eComp, palToUse);
        ConvertToCoefs(pf8Row, m_c4Width, af8Poles, c4PoleCnt, kCIDLib::f8Epsilon);
        for (tCIDLib::TCard4 c4XInd = 0; c4XInd < m_c4Width; c4XInd++)
            *pf4Tar++ = tCIDLib::TFloat4(pf8Row[c4XInd]);
    }

    for (tCIDLib::TCard4 c4XInd = 0; c4XInd < m_c4Width; c4XInd++)
    {
        //
        //  In this case, we load up from the previously output info from
        //  the row pass, not from the original data.
        //
        tCIDLib::TFloat8* pf8Tar = pf8Col;
        tCIDLib::TFloat4* pf4Src = pf4ToFill + c4XInd;
        for (tCIDLib::TCard4 c4YInd = 0; c4YInd < m_c4Height; c4YInd++)
        {
            *pf8Tar++ = *pf4Src;
            pf4Src += m_c4Width;
        }
        ConvertToCoefs(pf8Col, m_c4Height, af8Poles, c4PoleCnt, kCIDLib::f8Epsilon);
        tCIDLib::TFloat4* pf4Tar = pf4ToFill + c4XInd;
        for (tCIDLib::TCard4 c4YInd = 0; c4YInd < m_c4Height; c4YInd++)
        {
            //
            //  In this case we add it to the previous value and move down
            //  a full line.
            //
            *pf4Tar = tCIDLib::TFloat4(pf8Col[c4YInd]);
            pf4Tar += m_c4Width;
        }
    }
}


tCIDLib::TVoid
TPixelArrayImpl::PutPixel(  const   tCIDLib::TCard4 c4ToPut
                            , const tCIDLib::TCard4 c4XPos
                            , const tCIDLib::TCard4 c4YPos
                            , const tCIDLib::TCard4 c4Line)
{
    if ((c4XPos >= m_c4Width)
    ||  (c4YPos >= m_c4Height))
    {
        // NOTE: We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , c4Line
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TSize(c4XPos, c4YPos)
            , clsThis()
            , TSize(m_c4Width, m_c4Height)
        );
    }

    // Calculate the offset of the line its in
    const tCIDLib::TCard4 c4LineOfs = m_c4LineWidth * c4YPos;

    tCIDLib::TCard4             c4Byte;
    tCIDLib::TCard4             c4Bit;
    tCIDLib::TCard4             c4Tmp;
    TPixelArray::TRGBTriple*    prgbTmp;
    TPixelArray::TRGBQuad*      prgbTmp2;
    switch(m_eBitDepth)
    {
        case tCIDImage::EBitDepths::One :
        {
            c4Byte = c4XPos / 8;
            c4Bit = 7 - (c4XPos % 8);
            m_pc1Pixels[c4LineOfs + c4Byte] &= ~tCIDLib::TCard1(0x1 << c4Bit);
            m_pc1Pixels[c4LineOfs + c4Byte] |= tCIDLib::TCard1(c4ToPut << c4Bit);
            break;
        }

        case tCIDImage::EBitDepths::Two :
        {
            c4Byte = c4XPos / 4;
            c4Bit = (3 - (c4XPos % 4)) << 1;
            m_pc1Pixels[c4LineOfs + c4Byte] &= ~tCIDLib::TCard1(0x3 << c4Bit);
            m_pc1Pixels[c4LineOfs + c4Byte] |= tCIDLib::TCard1((c4ToPut & 0x3) << c4Bit);
            break;
        }

        case tCIDImage::EBitDepths::Four :
        {
            c4Byte = c4XPos / 2;
            c4Bit = (c4XPos & 0x1) ? 0 : 4;
            m_pc1Pixels[c4LineOfs + c4Byte] &= ~tCIDLib::TCard1(0xF << c4Bit);
            m_pc1Pixels[c4LineOfs + c4Byte] |= tCIDLib::TCard1((c4ToPut & 0xF) << c4Bit);
            break;
        }

        case tCIDImage::EBitDepths::Five :
        {
            // It's 3 color components in a 16 bit value. Can't have alpha
            *reinterpret_cast<tCIDLib::TCard2*>
            (
                &m_pc1Pixels[c4LineOfs + (c4XPos * 2)]
            ) = tCIDLib::TCard2(c4ToPut);
            break;
        }

        case tCIDImage::EBitDepths::Eight :
        {
            //
            //  For either gray scale or 8 bit palette, we just pull out the
            //  the next byte. If alpha, then get that next. For color, it's
            //  24 or 32 bit.
            //
            if ((m_eFmt == tCIDImage::EPixFmts::TrueClr)
            ||  (m_eFmt == tCIDImage::EPixFmts::TrueAlpha))
            {
                if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
                {
                    prgbTmp2 = reinterpret_cast<TPixelArray::TRGBQuad*>
                    (
                        &m_pc1Pixels[c4LineOfs + (c4XPos << 2)]
                    );
                    c4Tmp = c4ToPut;
                    prgbTmp2->c1Red = tCIDLib::TCard1(c4Tmp & 0xFFUL);
                    c4Tmp >>= 8;
                    prgbTmp2->c1Green = tCIDLib::TCard1(c4Tmp & 0xFFUL);
                    c4Tmp >>= 8;
                    prgbTmp2->c1Blue = tCIDLib::TCard1(c4Tmp & 0xFFUL);
                    c4Tmp >>= 8;
                    prgbTmp2->c1Alpha = tCIDLib::TCard1(c4Tmp);
                }
                 else
                {
                    prgbTmp = reinterpret_cast<TPixelArray::TRGBTriple*>
                    (
                        &m_pc1Pixels[c4LineOfs + (c4XPos * 3)]
                    );
                    c4Tmp = c4ToPut;
                    prgbTmp->c1Red = tCIDLib::TCard1(c4Tmp & 0xFFUL);
                    c4Tmp >>= 8;
                    prgbTmp->c1Green = tCIDLib::TCard1(c4Tmp & 0xFFUL);
                    c4Tmp >>= 8;
                    prgbTmp->c1Blue = tCIDLib::TCard1(c4Tmp & 0xFFUL);
                }
            }
             else
            {
                //
                //  Its 8 bit gray scale with possibly alpha, or 8 bit pal
                //  (which can't have alpha.)
                //
                if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
                {
                    m_pc1Pixels[c4LineOfs + (c4XPos << 1)] = tCIDLib::TCard1(c4ToPut >> 8);
                    m_pc1Pixels[c4LineOfs + (c4XPos << 1) + 1] = tCIDLib::TCard1(c4ToPut);
                }
                 else
                {
                    m_pc1Pixels[c4LineOfs + c4XPos] = tCIDLib::TCard1(c4ToPut);
                }
            }
            break;
        }

        case tCIDImage::EBitDepths::Sixteen :
        {
            // It's 16 bit gray scale, possibly with alpha
            if (tCIDLib::bAllBitsOn(m_eFmt, tCIDImage::EPixFmts::Alpha))
            {
                m_pc1Pixels[c4LineOfs + (c4XPos << 2)] = tCIDLib::TCard1(c4ToPut >> 24);
                m_pc1Pixels[c4LineOfs + (c4XPos << 2) + 1] = tCIDLib::TCard1(c4ToPut >> 16);
                m_pc1Pixels[c4LineOfs + (c4XPos << 2) + 2] = tCIDLib::TCard1(c4ToPut >> 8);
                m_pc1Pixels[c4LineOfs + (c4XPos << 2) + 3] = tCIDLib::TCard1(c4ToPut);
            }
             else
            {
                m_pc1Pixels[c4LineOfs + (c4XPos << 1)] = tCIDLib::TCard1(c4ToPut >> 8);
                m_pc1Pixels[c4LineOfs + (c4XPos << 1) + 1] = tCIDLib::TCard1(c4ToPut);
            }
            break;
        }

        default :
            facCIDImage().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kImgErrs::errcImg_BadBitDepth
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , TInteger(tCIDLib::i4EnumOrd(m_eBitDepth))
            );
            break;
    }
}


