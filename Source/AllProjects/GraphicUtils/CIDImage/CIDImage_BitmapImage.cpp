//
// FILE NAME: CIDImage_BitmapImage.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/17/1998
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
//  This file implements the TBitmapImage class.
//
//  <TBD> We need to honor the compression scheme indicated when streaming
//  out the bitmap data.
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
RTTIDecls(TBitmapImage,TCIDImage)



// ---------------------------------------------------------------------------
//  Local types
//
//  EDecompStates
//      This enum holds the two states for our RLE decompression state
//      machine.
//
//  TNibbleBufInfo
//      This is used when decoding 4 bit per pixel encoded bitmap bits. It
//      holds the state data required to spool out nibbles of data to a
//      byte buffer.
// ---------------------------------------------------------------------------
enum EDecompStates
{
    EDecomp_Marker
    , EDecomp_Length
};

struct TNibbleBufInfo
{
    tCIDLib::TBoolean   bLowFilled;
    tCIDLib::TCard1     c1NextByte;
    tCIDLib::TCard4     c4Index;
    tCIDLib::TCard1*    ac1Data;
    tCIDLib::TCard4     c4DataBufSz;
};


namespace CIDImage_BitmapImage
{
    // -----------------------------------------------------------------------
    //  Local, const data
    //
    //  bhiDefault
    //      The default value for a bitmap header info structure.
    //
    //  c2BA
    //      This is the 'BA' header stamp that is on OS/2 multiple image
    //      bitmap files.
    //
    //  c2BM
    //      This is the 'BM' header stamp that is on single image bitmap
    //      files.
    //
    //  c4SizeXXX
    //      These are some magic numbers for bitmap info header sizes of
    //      various verions of bitmaps.
    //
    //  c4LinePadding
    //      This is the number of bytes of padding required for a bitmap's
    //      pixel data.
    // -----------------------------------------------------------------------
    const TBitmapImage::TBmpHdrInfo   bhiDefault =
    {
        tCIDImage::EBmpTypes::Win_1x
        , tCIDImage::EBmpComp::None
        , tCIDImage::ERowOrders::TopDown
        , tCIDLib::c4EnumOrd(tCIDImage::EBitDepths::One)
        , 0
    };
    constexpr tCIDLib::TCard2   c2BA  = 0x4142;
    constexpr tCIDLib::TCard2   c2BM  = 0x4D42;
    constexpr tCIDLib::TCard4   c4Size1xHeader = 10;
    constexpr tCIDLib::TCard4   c4SizeBasicHeader = 12;
    constexpr tCIDLib::TCard4   c4Size3xHeader = 40;
    constexpr tCIDLib::TCard4   c4Size4xHeader = 108;
    constexpr tCIDLib::TCard4   c4SizeOS2Max = 64;
    constexpr tCIDLib::TCard4   c4SizeFileHeader = 14;
    constexpr tCIDLib::TCard4   c4LinePadding = 4;


    // -----------------------------------------------------------------------
    //  The file header format
    // -----------------------------------------------------------------------
    #pragma CIDLIB_PACK(1)
    struct TBMPFlHdr
    {
        tCIDLib::TCard2     c2Sig;
        tCIDLib::TCard4     c4FlSize;
        tCIDLib::TCard2     c2Res1;
        tCIDLib::TCard2     c2Res2;
        tCIDLib::TCard4     c4PixOfs;
    };
    #pragma CIDLIB_POPPACK
}



// ---------------------------------------------------------------------------
//  Local, static methods
// ---------------------------------------------------------------------------
static tCIDImage::EBmpComp
eXlatCompression(const  tCIDLib::TCard4 c4Compression
                , const tCIDLib::TCard4 c4BitDepth)
{
    switch(c4Compression)
    {
        case 0 :
            return tCIDImage::EBmpComp::None;

        case 1 :
            return tCIDImage::EBmpComp::RLE8;

        case 2 :
            return tCIDImage::EBmpComp::RLE4;

        case 3 :
            //
            //  Do a little magic here. A 40 byte OS/2 header and a Win 3.x
            //  header could both look the same but have a different
            //  interpretation of the compression value of 3. If its an
            //  OS/2 bmp with a compression of 3, then it must be a 1 bit
            //  format (i.e. a fax style image.) If its a Win 3.x with
            //  that compression value it cannot be 1, so we can tell them
            //  apart.
            //
            if (c4BitDepth == 1)
                return tCIDImage::EBmpComp::Huffman1D;
            return tCIDImage::EBmpComp::BitFields;

        case 4 :
            return tCIDImage::EBmpComp::RLE24;

        default :
            facCIDImage().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kImgErrs::errcBmp_UnknownCompress
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , TCardinal(c4Compression)
            );
            break;
    }

    // Make the compiler happy
    return tCIDImage::EBmpComp::None;
}

static tCIDLib::TVoid NibbleSpooler(const   tCIDLib::TCard1     c1Data
                                    , const tCIDLib::TCard4     c4RunCount
                                    ,       TNibbleBufInfo&     nbiTarget)
{
    tCIDLib::TCard1 c1Tmp;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4RunCount; c4Index++)
    {
        if (nbiTarget.c4Index >= nbiTarget.c4DataBufSz)
        {
            facCIDImage().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kImgErrs::errcBmp_TargetOverflow
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(nbiTarget.c4DataBufSz)
            );
        }

        if (c4Index & 0x1)
            c1Tmp = c1Data >> 4;
        else
            c1Tmp = c1Data & 0x0F;

        //
        //  If the low nibble is filled on the next byte buffer, then put the
        //  low src nibble into the high nibble of the next byte nad write
        //  it to the target buffer.
        //
        if (nbiTarget.bLowFilled)
        {
            nbiTarget.c1NextByte |= c1Tmp << 4;
            nbiTarget.ac1Data[nbiTarget.c4Index++] = nbiTarget.c1NextByte;
            nbiTarget.bLowFilled = kCIDLib::False;
        }
         else
        {
            nbiTarget.c1NextByte |= c1Tmp & 0x0F;
            nbiTarget.bLowFilled = kCIDLib::True;
        }
    }
}

static tCIDLib::TVoid
NibbleSpooler(  const   tCIDLib::TCard1     c1Data
                ,       TNibbleBufInfo&     nbiTarget
                , const tCIDLib::TBoolean   bLowOnly)
{
    if (nbiTarget.c4Index >= nbiTarget.c4DataBufSz)
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcBmp_TargetOverflow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(nbiTarget.c4DataBufSz)
        );
    }

    //
    //  If the low nibble is filled on the next byte buffer, then put the
    //  low src nibble into the high nibble of the next byte nad write
    //  it to the target buffer.
    //
    if (nbiTarget.bLowFilled)
    {
        nbiTarget.c1NextByte |= c1Data << 4;
        nbiTarget.ac1Data[nbiTarget.c4Index++] = nbiTarget.c1NextByte;
        nbiTarget.bLowFilled = kCIDLib::False;
    }
     else
    {
        nbiTarget.c1NextByte |= c1Data & 0x0F;
        nbiTarget.bLowFilled = kCIDLib::True;
    }

    if (!bLowOnly)
    {
        if (nbiTarget.c4Index >= nbiTarget.c4DataBufSz)
        {
            facCIDImage().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kImgErrs::errcBmp_TargetOverflow
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(nbiTarget.c4DataBufSz)
            );
        }

        if (nbiTarget.bLowFilled)
        {
            nbiTarget.c1NextByte |= c1Data & 0xF0;
            nbiTarget.ac1Data[nbiTarget.c4Index++] = nbiTarget.c1NextByte;
            nbiTarget.bLowFilled = kCIDLib::False;
        }
         else
        {
            nbiTarget.c1NextByte |= c1Data >> 4;
            nbiTarget.bLowFilled = kCIDLib::True;
        }
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TBitmapImage
// PREFIX: img
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBitmapImage: Constructors and Destructor
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TBitmapImage::bIsValidBmpFmt(const TCIDImage& imgToCheck)
{
    //
    //  Indicates whether the passed image is a format that can be passed
    //  directly in to create a bitmap. If not, then it would have to be
    //  converted first.
    //
    const tCIDImage::EPixFmts eFmt = imgToCheck.ePixFmt();
    const tCIDImage::EBitDepths eDepth = imgToCheck.eBitDepth();

    if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Palette))
    {
        //
        //  Windows cannot handle 2 bit palettes. It will have to be converted
        //  to a 4 bit palette and some of the colors just not used.
        //
        if (eDepth == tCIDImage::EBitDepths::Two)
            return kCIDLib::False;
    }
     else if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Color))
    {
        //
        //  We can handle any of these. Super-high resolution images, such as
        //  supported by PNG, are down converted to 24/32 bit color when they
        //  are loaded.
        //
    }
     else
    {
        //
        //  We cannot handle 16 bit gray scale or any gray scale with alpha.
        //  These will have to be converted to true color first.
        //
        //  1 bit we can handle directly. The others will be treated like they
        //  are palette based, using a generated gray scale palette.
        //
        if ((eDepth == tCIDImage::EBitDepths::One)
        ||  tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Alpha))
        {
            return kCIDLib::False;
        }
    }

    // We can handle it directly
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TBitmapImage: Constructors and Destructor
// ---------------------------------------------------------------------------
TBitmapImage::TBitmapImage() :

    TCIDImage(tCIDImage::EImgTypes::Bitmap)
    , m_c4CompressedSize(0)
    , m_eBmpType(tCIDImage::EBmpTypes::Win_3x)
    , m_eCompression(tCIDImage::EBmpComp::None)
    , m_c4BlueMask(0)
    , m_c4GreenMask(0)
    , m_c4RedMask(0)
{
    // Set the compressed size to the regular size, since no compression
    m_c4CompressedSize = c4ImageSz();
}

TBitmapImage::TBitmapImage( const   TPixelArray&            pixaBits
                            , const TClrPalette&            palToSet
                            , const tCIDImage::EBmpTypes    eType
                            , const tCIDLib::TCard4         c4ClrsImportant
                            , const tCIDLib::TCard4         c4ClrsUsed) :

    TCIDImage
    (
        tCIDImage::EImgTypes::Bitmap
        , pixaBits
        , palToSet
        , c4ClrsImportant
        , c4ClrsUsed
    )
    , m_eCompression(tCIDImage::EBmpComp::None)
    , m_eBmpType(eType)
{
    // Set the compressed size to the regular size, since no compression
    m_c4CompressedSize = c4ImageSz();
}

TBitmapImage::TBitmapImage( const   TPixelArray&            pixaBits
                            , const tCIDImage::EBmpTypes    eBmpType
                            , const tCIDLib::TCard4         c4ClrsImportant
                            , const tCIDLib::TCard4         c4ClrsUsed) :

    TCIDImage
    (
        tCIDImage::EImgTypes::Bitmap
        , pixaBits
        , c4ClrsImportant
        , c4ClrsUsed
    )
    , m_eCompression(tCIDImage::EBmpComp::None)
    , m_eBmpType(eBmpType)
{
    // Set the compressed size to the regular size, since no compression
    m_c4CompressedSize = c4ImageSz();
}

//
//  All image classes have to be able to construct via the base image class,
//  so that we can easily convert between image types.
//
TBitmapImage::TBitmapImage(const TCIDImage& imgSrc) :

    TCIDImage(tCIDImage::EImgTypes::Bitmap, imgSrc)
    , m_eCompression(tCIDImage::EBmpComp::None)
    , m_eBmpType(tCIDImage::EBmpTypes::Win_3x)
{
    // Set the compressed size to the regular size, since no compression
    m_c4CompressedSize = c4ImageSz();
}


TBitmapImage::TBitmapImage(const TBitmapImage& imgToCopy) :

    TCIDImage(tCIDImage::EImgTypes::Bitmap, imgToCopy)
    , m_c4CompressedSize(imgToCopy.m_c4CompressedSize)
    , m_eCompression(imgToCopy.m_eCompression)
    , m_eBmpType(imgToCopy.m_eBmpType)
{
}

TBitmapImage::~TBitmapImage()
{
}


// ---------------------------------------------------------------------------
//  TBitmapImage: Public operators
// ---------------------------------------------------------------------------
TBitmapImage& TBitmapImage::operator=(const TBitmapImage& imgToAssign)
{
    if (this != &imgToAssign)
    {
        // Call our parent
        TParent::operator=(imgToAssign);

        // And copy over our stuff
        m_c4CompressedSize= imgToAssign.m_c4CompressedSize;
        m_eCompression = imgToAssign.m_eCompression;
        m_eBmpType = imgToAssign.m_eBmpType;
    }
    return *this;
}


TBitmapImage& TBitmapImage::operator=(const TCIDImage& imgToAssign)
{
    if (this == &imgToAssign)
        return *this;

    // Call our parent
    TParent::operator=(imgToAssign);

    // Default our stuff
    m_eCompression = tCIDImage::EBmpComp::None;
    m_eBmpType = tCIDImage::EBmpTypes::Win_3x;

    // Set the compressed size to the regular size, since no compression
    m_c4CompressedSize = c4ImageSz();

    return *this;
}


// ---------------------------------------------------------------------------
//  TBitmapImage: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TBitmapImage::StreamFrom(TBinInStream& strmToReadFrom)
{
    //
    //  Ok, here we go. First thing in the file is the file type indicator.
    //
    tCIDLib::TCard2 c2FileType;
    strmToReadFrom >> c2FileType;

    //
    //  Check that the file type is something that we can understand. We
    //  only do 'BM' type of files. If we find anything else, then log an
    //  error. Do a special one for 'BA' type files.
    //
    if (c2FileType != CIDImage_BitmapImage::c2BM)
    {
        if (c2FileType == CIDImage_BitmapImage::c2BA)
        {
            facCIDImage().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kImgErrs::errcBmp_ArrayNotSupported
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
         else
        {
            facCIDImage().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kImgErrs::errcBmp_UnknownFileType
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , TCardinal(c2FileType, tCIDLib::ERadices::Hex)
            );
        }
    }

    //
    //  If the file type is 0, then its a Win 1.x file so the file header is also
    //  the image header. Otherwise, there is a separate file header that we need
    //  to skip over. We read it in so that we can see the contentes for debugging.
    //
    //   We have already read the signature, so we have to skip that.
    //
    tCIDLib::TCard4 c4HdrPelOfs = 0;
    if (c2FileType)
    {
        CIDImage_BitmapImage::TBMPFlHdr flHdr;
        strmToReadFrom.c4ReadRawBuffer
        (
            (tCIDLib::TCard1*)&flHdr.c4FlSize
            , CIDImage_BitmapImage::c4SizeFileHeader - 2
        );
        c4HdrPelOfs = flHdr.c4PixOfs;
    }

    //
    //  Parse out the header information, and fill in our parent class
    //  the image information.
    //
    TBmpHdrInfo bhiTmp;
    ParseHeader(strmToReadFrom, c2FileType, bhiTmp);

    //
    //  Store our header information away. This will set up our parent
    //  class with all of the pixel format, palette, etc... base info, which
    //  will set up the pixel array and palette and all appropriately for
    //  us to start filling in.
    //
    SetFromBH(bhiTmp);

    //
    //  Now, if there is a palette for this bitmap, we need to parse it
    //  in.
    //
    if (tCIDLib::bAllBitsOn(ePixFmt(), tCIDImage::EPixFmts::Palette))
        ParsePalette(strmToReadFrom);

    // If we have a header pixel data offset, move up to there if not already
    if (c4HdrPelOfs)
    {
        tCIDLib::TCard4 c4CurPos(tCIDLib::TCard4(strmToReadFrom.c8CurPos()));
        if (c4CurPos < c4HdrPelOfs)
            strmToReadFrom.SkipForwardBy(c4HdrPelOfs - c4CurPos);
    }
    ParseBits(strmToReadFrom, bhiTmp);
}


tCIDLib::TVoid TBitmapImage::StreamTo(TBinOutStream& strmToWriteTo) const
{
    //
    //  Get some short cut copies of some info inside the image info
    //  object, since we are going to use them a lot below.
    //
    const tCIDImage::EPixFmts eFmt = ePixFmt();
    const tCIDImage::EBitDepths eDepth = eBitDepth();
    const tCIDLib::TCard4 c4LineWidth = c4LineByteWidth();
    const tCIDLib::TCard4 c4CX = c4Width();
    const tCIDLib::TCard4 c4Sz = c4ImageSz();
    const tCIDLib::TCard4 c4CY = c4Height();
    const tCIDLib::TCard4 c4BitsPer = c4BitsPerPixel();
    const TPixelArray& pixaSrc = pixaBits();

    //
    //  Make sure that we can store the image size, which might not be
    //  possible for some of the earlier formats which take the size as
    //  16 bit signed or unsigned values.
    //
    if (m_eBmpType == tCIDImage::EBmpTypes::Win_1x)
    {
        if ((c4CX > kCIDLib::c2MaxCard)
        ||  (c4CY > kCIDLib::c2MaxCard))
        {
            facCIDImage().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kImgErrs::errcBmp_SizeTooLarge
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::OutResource
                , TSize(c4CX, c4CY)
            );
        }
    }

    if (m_eBmpType == tCIDImage::EBmpTypes::Win_2x)
    {
        if ((c4CX > kCIDLib::i2MaxInt)
        ||  (c4CY > kCIDLib::i2MaxInt))
        {
            facCIDImage().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kImgErrs::errcBmp_SizeTooLarge
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::OutResource
                , TSize(c4CX, c4CY)
            );
        }
    }

    //
    //  Calculate how many bytes are in a palette entry for this type
    //  of bitmap. And then calculate how many actual colors we will write
    //  out and how many bytes that will be.
    //
    tCIDLib::TCard4 c4PalEntryBytes = 0;
    tCIDLib::TCard4 c4ActualClrs = 0;
    if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Palette))
    {
        c4PalEntryBytes = (m_eBmpType <= tCIDImage::EBmpTypes::Win_2x) ? 3 : 4;
        c4ActualClrs = (0x1UL << tCIDLib::c4EnumOrd(eDepth));
    }

    // Calculate the size of the header for the type of the bitmap
    tCIDLib::TCard4 c4HeaderSize = 0;
    if (m_eBmpType == tCIDImage::EBmpTypes::Win_1x)
        c4HeaderSize = CIDImage_BitmapImage::c4Size1xHeader;
    else if (m_eBmpType == tCIDImage::EBmpTypes::Win_2x)
        c4HeaderSize = CIDImage_BitmapImage::c4SizeBasicHeader;
    else if (m_eBmpType == tCIDImage::EBmpTypes::Win_3x)
        c4HeaderSize = CIDImage_BitmapImage::c4Size3xHeader;
    else if (m_eBmpType == tCIDImage::EBmpTypes::Win_4x)
        c4HeaderSize = CIDImage_BitmapImage::c4Size4xHeader;
    else if (m_eBmpType == tCIDImage::EBmpTypes::OS2_2x)
        c4HeaderSize = CIDImage_BitmapImage::c4SizeOS2Max;
    {
        // <TBD> Should do something here if in debug mode
    }

    //
    //  Ok, lets start off at the beginning, which is the writing out of
    //  the file header.
    //
    if (m_eBmpType == tCIDImage::EBmpTypes::Win_1x)
    {
        //
        //  Write the file type out, which is zero for this format, then
        //  the other header fields.
        //
        strmToWriteTo   << tCIDLib::TCard2(0)
                        << tCIDLib::TCard2(c4CX)
                        << tCIDLib::TCard2(c4CY)
                        << tCIDLib::TCard2(c4LineWidth)
                        << tCIDLib::TCard2(1)
                        << tCIDLib::TCard2(c4BitsPer);
    }
     else if ((m_eBmpType == tCIDImage::EBmpTypes::Win_2x)
          ||  (m_eBmpType == tCIDImage::EBmpTypes::Win_3x)
          ||  (m_eBmpType == tCIDImage::EBmpTypes::Win_4x))
    {
        // Calc the offset ot the image data
        tCIDLib::TCard4 c4ImageOfs = CIDImage_BitmapImage::c4SizeFileHeader + c4HeaderSize;
        if (c4ActualClrs <= 256)
            c4ImageOfs += (c4PalEntryBytes * c4ActualClrs);

        //
        //  Write out the file header first, which is the same for all of
        //  these versions. We write out the type marker, the file size
        //  which is zero for uncompressed data, then two zero reserved
        //  16 bit values (as a single 32 bit write.)
        //
        strmToWriteTo   << CIDImage_BitmapImage::c2BM
                        << tCIDLib::TCard4(c4Sz + c4ImageOfs)
                        << tCIDLib::TCard4(0)
                        << c4ImageOfs;
        //
        //  Ok, now we can start writing out header data. We just write out
        //  what is needed for each version step.
        //
        if (m_eBmpType == tCIDImage::EBmpTypes::Win_2x)
        {
            strmToWriteTo   << CIDImage_BitmapImage::c4SizeBasicHeader
                            << tCIDLib::TCard2(c4CX);
            if (pixaSrc.eRowOrder() == tCIDImage::ERowOrders::TopDown)
                strmToWriteTo << -tCIDLib::TInt2(c4CY);
            else
                strmToWriteTo << tCIDLib::TCard2(c4CY);
        }
         else if (m_eBmpType == tCIDImage::EBmpTypes::Win_3x)
        {
            strmToWriteTo   << CIDImage_BitmapImage::c4Size3xHeader
                            << tCIDLib::TCard4(c4CX);
            if (pixaSrc.eRowOrder() == tCIDImage::ERowOrders::TopDown)
                strmToWriteTo << -tCIDLib::TInt4(c4CY);
            else
                strmToWriteTo << tCIDLib::TCard4(c4CY);
        }
         else
        {
            strmToWriteTo   << CIDImage_BitmapImage::c4Size4xHeader
                            << tCIDLib::TCard4(c4CX);
            if (pixaSrc.eRowOrder() == tCIDImage::ERowOrders::TopDown)
                strmToWriteTo << -tCIDLib::TInt4(c4CY);
            else
                strmToWriteTo << tCIDLib::TCard4(c4CY);
        }

        // These fields are the same size for all
        strmToWriteTo   << tCIDLib::TCard2(1)
                        << tCIDLib::TCard2(c4BitsPer);

        if (m_eBmpType >= tCIDImage::EBmpTypes::Win_3x)
        {
            strmToWriteTo   << tCIDLib::TCard4(0) // Compression
                            << tCIDLib::TCard4(0) // Not required when uncomp
                            << tCIDLib::TCard4(2835) // szResolution().c4Width()
                            << tCIDLib::TCard4(2835) // szResolution().c4Height()
                            << c4ActualClrs
                            << tCIDLib::TCard4(0);
        }

        if (m_eBmpType >= tCIDImage::EBmpTypes::Win_4x)
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < 17; c4Index++)
                strmToWriteTo << tCIDLib::TCard4(0);
        }
    }
     else if (m_eBmpType == tCIDImage::EBmpTypes::OS2_2x)
    {
        // <TBD>
    }
     else
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcBmp_UnknownTypeEnum
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , TInteger(tCIDLib::i4EnumOrd(m_eBmpType))
        );
    }

    //
    //  Ok, lets write out the palette now. The palette entries are not
    //  the same order (in terms of RGB values) as our standard encoded
    //  color value, so we cannot just write out the coded color from each
    //  TRGBClr object in the palette. And we have to deal with 3 bytes
    //  per entry palettes too.
    //
    if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Palette))
    {
        const TClrPalette& palSrc = palClrs();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ActualClrs; c4Index++)
        {
            const TRGBClr& rgbCur = palSrc[c4Index];
            strmToWriteTo   << rgbCur.c1Blue()
                            << rgbCur.c1Green()
                            << rgbCur.c1Red();

            if (c4PalEntryBytes == 4)
                strmToWriteTo << tCIDLib::TCard1(0);
        }
    }
    strmToWriteTo.c4WriteRawBuffer(pixaSrc.pc1Buffer(), c4ImageSz());
}


// ---------------------------------------------------------------------------
//  TBitmapImage: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If we see a 32 bit file, that means it is really 24 bit with an unused
//  byte, so we need to convert it. Our pixel array will have been set up for
//  24 bit data, so we go through and toss every 4 byte to create the actual
//  data.
//
tCIDLib::TVoid
TBitmapImage::Load32BitData(        TBinInStream&           strmFrom
                            ,       tCIDLib::TCard1* const  pc1ToLoad
                            , const TSize&                  szImg
                            , const tCIDLib::TCard4         c4LineWidth)
{
    const tCIDLib::TCard4 c4YCount = szImg.c4Height();
    const tCIDLib::TCard4 c4XCount = szImg.c4Width();

    tCIDLib::TCard1  c1Eat;
    tCIDLib::TCard1* pc1Out;
    for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4YCount; c4YInd++)
    {
        // Because of padding we recalc each time
        pc1Out = pc1ToLoad + (c4LineWidth * c4YInd);
        for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4XCount; c4XInd++)
        {
            // Read three bytes and eat the dummy alpha byte
            strmFrom.c4ReadRawBuffer(pc1Out, 3);
            strmFrom >> c1Eat;

            // And move our output pointer up by the 3 we kept
            pc1Out += 3;
        }
    }
}


tCIDLib::TVoid
TBitmapImage::ParseBits(TBinInStream& strmFrom, const TBmpHdrInfo& bhiSrc)
{
    TPixelArray& pixaUs = pixaNCBits();

    // Zero the pixels so any defaulted pixels will be zero
    pixaUs.ZeroAll();

    // Get a pointer to the raw pixel buffer
    tCIDLib::TCard1* pc1Buffer = pixaUs.pc1Buffer();

    //
    //  Ok, now the hard part. According to the compression used by this
    //  image, we handle the parsing differently. We just spool bytes out
    //  of the stream, letting it handle buffering for us.
    //
    //  For the uncompressed case we read in the calculated image size.
    //  For the others we read in the compressed image size.
    //
    if (m_eCompression == tCIDImage::EBmpComp::None)
    {
        //
        //  For this one we can usually just read in the raw data in a single
        //  big read. But if the source is 32 bit, it's not really true-alpha,
        //  since the alpha isn't used in the file format. So the pixel array
        //  will have been set up for 24 bit color and we have to convert.
        //
        if (bhiSrc.c4BitDepth == 32)
            Load32BitData(strmFrom, pc1Buffer, pixaUs.szImage(), pixaUs.c4LineWidth());
        else
            TBinInStream_ReadArray(strmFrom, pc1Buffer, pixaUs.c4ImageSz());
    }
     else if (m_eCompression == tCIDImage::EBmpComp::RLE8)
    {
        ParseRLE8Bits(strmFrom, pixaUs);
    }
     else if (m_eCompression == tCIDImage::EBmpComp::RLE4)
    {
        ParseRLE4Bits(strmFrom, pixaUs);
    }
     else if (m_eCompression == tCIDImage::EBmpComp::BitFields)
    {
        // <TBD> We don't handle this yet!!
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcBmp_NoBitFields
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotSupported
        );
//      ParseBitFieldBits(strmFrom, pixaUs);
    }
     else if (m_eCompression == tCIDImage::EBmpComp::Huffman1D)
    {
        // <TBD> We don't handle this yet!!
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcBmp_NoHuffman
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotSupported
        );
//      ParseBitHuffman(strmFrom, pixaUs);
    }
     else
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcBmp_UnknownCompress
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , TInteger(tCIDLib::i4EnumOrd(m_eCompression))
        );
    }
}


tCIDLib::TVoid
TBitmapImage::ParseHeader(          TBinInStream&   strmFrom
                            , const tCIDLib::TCard2 c2FileType
                            ,       TBmpHdrInfo&    bhiToFill)
{
    // Init the raw bitmap header info structure to defaults
    bhiToFill = CIDImage_BitmapImage::bhiDefault;

    // Some temps for reading data into
    tCIDLib::TCard2 c2Tmp1;
    tCIDLib::TCard4 c4HeaderSize;
    tCIDLib::TCard4 c4Tmp1;
    tCIDLib::TInt2  i2Tmp1;
    tCIDLib::TInt4  i4Tmp1;

    // Remember our starting position
    const tCIDLib::TCard8 c8Start = strmFrom.c8CurPos();

    //
    //  If the file type indicates a 1.x Win DDB, we have to deal with it
    //  separately.
    //
    if (!c2FileType)
    {
        // Get out the pixel width and height
        strmFrom >> c2Tmp1 >> i2Tmp1;
        if (i2Tmp1 < 0)
        {
            bhiToFill.c4Height = tCIDLib::TCard4(i2Tmp1 * -1);
            bhiToFill.eRowOrder = tCIDImage::ERowOrders::TopDown;
        }
         else
        {
            bhiToFill.c4Height = tCIDLib::TCard4(i2Tmp1);
            bhiToFill.eRowOrder = tCIDImage::ERowOrders::BottomUp;
        }
        bhiToFill.c4Width = c2Tmp1;

        //
        //  Get the byte width but throw it away. This is something that
        //  we always calculate. Only this 1.x version has it in the data.
        //
        strmFrom >> c2Tmp1;

        // Skip the planes, which is always 1
        strmFrom.SkipForwardBy(2);

        // And get out the bits per pixel and test it before storing
        strmFrom >> c2Tmp1;
        TestBitDepth(c2Tmp1);
        bhiToFill.c4BitDepth = c2Tmp1;

        // Set the type
        bhiToFill.eType = tCIDImage::EBmpTypes::Win_1x;
    }
     else
    {
        //
        //  We read in the size field first and use that to know how
        //  much other stuff we have available.
        //
        strmFrom >> c4HeaderSize;

        //
        //  According to the header size, we need to set the type of
        //  this image.
        //
        if (c4HeaderSize == CIDImage_BitmapImage::c4SizeBasicHeader)
        {
            //
            //  It could be either a Win 2.x or an OS/2 1.x file. We don't
            //  really care because they are the same so we just declare it
            //  a Win 2.x file.
            //
            bhiToFill.eType = tCIDImage::EBmpTypes::Win_2x;
        }
         else if (c4HeaderSize == CIDImage_BitmapImage::c4Size3xHeader)
        {
            bhiToFill.eType = tCIDImage::EBmpTypes::Win_3x;
        }
         else if (c4HeaderSize == CIDImage_BitmapImage::c4Size4xHeader)
        {
            bhiToFill.eType = tCIDImage::EBmpTypes::Win_4x;
        }
         else
        {
            if ((c4HeaderSize > CIDImage_BitmapImage::c4SizeOS2Max)
            ||  (c4HeaderSize < CIDImage_BitmapImage::c4SizeBasicHeader))
            {
                // Don't know what the heck this is
                facCIDImage().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kImgErrs::errcBmp_UnknownHdrSize
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::AppError
                    , TCardinal(c4HeaderSize)
                );
            }

            //
            //  Just assume its an OS/2 2.x file, which can have a variable
            //  header size.
            //
            bhiToFill.eType = tCIDImage::EBmpTypes::OS2_2x;
        }

        //
        //  Ok, so now lets read in the header fields. Because of the fact
        //  that OS/2 allows the header to be variable size, there is no
        //  really clean way to do this other than checking to see whether
        //  there is enough room to read the next value in. We can though
        //  assume that the basic fields are always available, the ones
        //  within the basic header.
        //
        //  Read in the width and height. If this is anything less than
        //  or equal to a Win 2.x header, then we have to read it
        //  into temp 16 bit values first.
        //
        //  We have to watch for negative heights, which indicate a top
        //  down image.
        //
        if (bhiToFill.eType <= tCIDImage::EBmpTypes::Win_2x)
        {
            strmFrom >> c2Tmp1 >> i2Tmp1;
            if (i2Tmp1 < 0)
            {
                bhiToFill.c4Height = tCIDLib::TCard4(i2Tmp1 * -1);
                bhiToFill.eRowOrder = tCIDImage::ERowOrders::TopDown;
            }
             else
            {
                bhiToFill.c4Height = tCIDLib::TCard4(i2Tmp1);
                bhiToFill.eRowOrder = tCIDImage::ERowOrders::BottomUp;
            }
        }
         else
        {
            strmFrom >> bhiToFill.c4Width >> i4Tmp1;
            if (i4Tmp1 < 0)
            {
                bhiToFill.c4Height = tCIDLib::TCard4(i4Tmp1 * -1);
                bhiToFill.eRowOrder = tCIDImage::ERowOrders::TopDown;
            }
             else
            {
                bhiToFill.c4Height = tCIDLib::TCard4(i4Tmp1);
                bhiToFill.eRowOrder = tCIDImage::ERowOrders::BottomUp;
            }
        }

        // The planes and bits per pixel are available. Skip the planes
        strmFrom.SkipForwardBy(2);

        // And read in the bits per pixel
        strmFrom >> c2Tmp1;
        TestBitDepth(c2Tmp1);
        bhiToFill.c4BitDepth = c2Tmp1;

        //
        //  Now, with the compression field, we have to start checking
        //  to see whether each new field is available.
        //
        if (strmFrom.c8CurPos() + 4 <= c4HeaderSize + c8Start)
        {
            // Get the compression
            strmFrom >> c4Tmp1;
            bhiToFill.eCompression = eXlatCompression(c4Tmp1, bhiToFill.c4BitDepth);

            // The image size field is available
            if (strmFrom.c8CurPos() + 4 <= c4HeaderSize + c8Start)
            {
                strmFrom >> bhiToFill.c4CompressedSize;

                if (!bhiToFill.c4CompressedSize
                &&  (bhiToFill.eCompression != tCIDImage::EBmpComp::None))
                {
                    facCIDImage().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kImgErrs::errcBmp_CompressError
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Format
                    );
                }

                // The device res fields are available
                if (strmFrom.c8CurPos() + 8 <= c4HeaderSize + c8Start)
                {
                    // And get the device resolution
                    strmFrom >> bhiToFill.c4HorzRes >> bhiToFill.c4VertRes;

                    // The colors used field is available
                    if (strmFrom.c8CurPos() + 4 <= c4HeaderSize + c8Start)
                    {
                        strmFrom >> bhiToFill.c4ClrsUsed;

                        // The colors important field is available skip it
                        if (strmFrom.c8CurPos() + 4 <= c4HeaderSize + c8Start)
                            strmFrom.SkipForwardBy(4);
                    }
                }
            }
        }

        //
        //  !!!!! ------------------------------------------
        //  This gets us up to the point where its either a 2.x OS/2
        //  header or a Win 4.x header. If its an OS/2 header then we
        //  don't want any more info from it. If its the size of a Win 4.x
        //  header we can get anything else from that header that we want
        //  without all the BS we had to go through above.
        //  !!!!! ------------------------------------------
        //
        if (c4HeaderSize == CIDImage_BitmapImage::c4Size4xHeader)
        {
            // Read in the color mask values
            strmFrom    >> bhiToFill.c4RedMask
                        >> bhiToFill.c4GreenMask
                        >> bhiToFill.c4BlueMask
                        >> bhiToFill.c4AlphaMask;
        }

        //
        //  If the compression is of the bitfield style and the header size
        //  indicates a Win 3.x header, then we have to get out the bit
        //  mask fields which follow the header.
        //
        if ((bhiToFill.eCompression == tCIDImage::EBmpComp::BitFields)
        &&  (c4HeaderSize == CIDImage_BitmapImage::c4Size3xHeader))
        {
            strmFrom    >> bhiToFill.c4RedMask
                        >> bhiToFill.c4GreenMask
                        >> bhiToFill.c4BlueMask;
        }
    }

    // The line byte rounding is always 4 bytes for bitmaps
    bhiToFill.c4LineByteRounding = 4;
}


tCIDLib::TVoid TBitmapImage::ParsePalette(TBinInStream& strmFrom)
{
    // Sanity check to avoid major problems in case of an error upstream
    const tCIDImage::EBitDepths eDepth = eBitDepth();
    if (tCIDLib::c4EnumOrd(eDepth) > 8)
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcBmp_InternalPalErr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    //
    //  Calculate the colors for convenience below. We will lower this
    //  number if the colors used value is used and less than the maximum
    //  colors we could have.
    //
    tCIDLib::TCard4 c4Colors = 0x1UL << tCIDLib::c4EnumOrd(eDepth);
    if ((tCIDLib::c4EnumOrd(eDepth) < 16) && c4ClrsUsed())
        c4Colors = c4ClrsUsed();

    //
    //  See how large the palette entries are for this image. They are
    //  either 3 or 4 byte values.
    //
    const tCIDLib::TCard4 c4EntryBytes
                = (m_eBmpType <= tCIDImage::EBmpTypes::Win_2x) ? 3 : 4;

    //
    //  And now calculate the number of bytes in the palette, which is the
    //  color count times the palette entry bytes.
    //
    const tCIDLib::TCard4 c4PalSize = c4Colors * c4EntryBytes;

    //
    //  Use a local buffer for the read, since we know that we will never
    //  get here with more than 256 colors.
    //
    tCIDLib::TCard1 ac1Buffer[1024];

    //
    //  Read in the buffer using a raw buffer read. The default is throw
    //  if the full requested bytes are not read, which is what we want.
    //
    TBinInStream_ReadArray(strmFrom, ac1Buffer, c4PalSize);

    //
    //  Get a ref to the palette, which was set up appropriately for us when
    //  we parsed the header and set the basic image info on our parent.
    //
    TClrPalette& palTar = palClrs();

    //
    //  Look at the old buffer in the old 3 byte way and in the new 4 byte
    //  way. We'll use the appropriate one below.
    //
    TPixelArray::TRGBTriple* p3Byte = reinterpret_cast<TPixelArray::TRGBTriple*>(ac1Buffer);
    TPixelArray::TRGBQuad* p4Byte = reinterpret_cast<TPixelArray::TRGBQuad*>(ac1Buffer);

    tCIDLib::TCodedRGB rgbTmp;
    rgbTmp.c1Alpha = 0;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Colors; c4Index++)
    {
        if (c4EntryBytes == 3)
        {
            rgbTmp.c1Red = p3Byte[c4Index].c1Red;
            rgbTmp.c1Green = p3Byte[c4Index].c1Green;
            rgbTmp.c1Blue = p3Byte[c4Index].c1Blue;
        }
        else
        {
            rgbTmp.c1Red = p4Byte[c4Index].c1Red;
            rgbTmp.c1Blue = p4Byte[c4Index].c1Blue;
            rgbTmp.c1Green = p4Byte[c4Index].c1Green;
        }
        palTar[c4Index] = rgbTmp;
    }
}


tCIDLib::TVoid
TBitmapImage::ParseRLE4Bits(TBinInStream& strmFrom, TPixelArray& pixaBits)
{
    //
    //  Get a local copy of the compressed size, byte width, ect... for
    //  fast access and less typing.
    //
    const tCIDLib::TCard4 c4ByteWidth = c4LineByteWidth();
    const tCIDLib::TCard4 c4CX = c4Width();

    //
    //  Allocate a buffer big enough to read in the compressed data. We'll
    //  throw it away when we've uncompressed it, so put a janitor on it.
    //
    tCIDLib::TCard1* pc1Data = new tCIDLib::TCard1[m_c4CompressedSize];
    TArrayJanitor<tCIDLib::TCard1> janData(pc1Data);

    //
    //  Read in the compressed data. The default is to throw if the requested
    //  bytes are not read, which is what we want.
    //
    TBinInStream_ReadArray(strmFrom, pc1Data, m_c4CompressedSize);

    //
    //  We use a state machine to decompress, so lets start our state in the
    //  marker state since that's the first thing we should find.
    //
    EDecompStates eState = EDecomp_Marker;

    //
    //  Some state variables for the state machine. We have to track where
    //  we are decompressing to in the target buffer and the current marker.
    //
    tCIDLib::TCard1 c1CurMarker(0);
    tCIDLib::TCard4 c4RunInd;

    //
    //  This is a data structure that is used to manage the spooling of
    //  nibbles into the target data buffer. It makes the logic here much,
    //  much simpler to understand and maintain and the cost of a little
    //  performance hit.
    //
    TNibbleBufInfo nbiParse =
    {
        kCIDLib::False, 0, 0, pixaBits.pc1Buffer(), c4ImageSz()
    };

    // And handle every byte in the buffer
    tCIDLib::TCard4 c4SrcInd = 0;
    while (c4SrcInd < m_c4CompressedSize)
    {
        if (eState == EDecomp_Marker)
        {
            c1CurMarker = pc1Data[c4SrcInd++];
            eState = EDecomp_Length;
        }
         else if (eState == EDecomp_Length)
        {
            tCIDLib::TCard1 c1Byte2 = pc1Data[c4SrcInd++];

            if (!c1CurMarker)
            {
                //
                //  The new byte is one of the special case markers or an
                //  unecoded run.
                //
                if (c1Byte2 == 0)
                {
                    //
                    //  Its an end of scan line marker, so we need to move
                    //  up to the next line in the output buffer.
                    //
                    nbiParse.c4Index += nbiParse.c4Index % c4ByteWidth;
                    nbiParse.bLowFilled = kCIDLib::False;
                    nbiParse.c1NextByte = 0;
                }
                 else if (c1Byte2 == 1)
                {
                    //
                    //  This is the end of encoded data marker. If we are
                    //  not at the last byte of the buffer, then something
                    //  is wrong.
                    //
                    if (c4SrcInd != m_c4CompressedSize)
                    {
                        facCIDImage().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kImgErrs::errcBmp_NotAtEnd
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Format
                        );
                    }

                    //
                    //  Return here so that any fall out below will indicate
                    //  a problem, since we never saw the end of data marker.
                    //
                    return;
                }
                 else if (c1Byte2 == 2)
                {
                    //
                    //  Its a delta marker, which tells us to jump to
                    //  somewhere else in the target buffer. We have to
                    //  play some magic here and read in the two delta bytes
                    //  and bump up the index manually.
                    //
                    tCIDLib::TCard1 c1XDelta, c1YDelta;

                    //
                    //  Sanity check that this would not go beyond the
                    //  available encoded data.
                    //
                    if (c4SrcInd + 2 >= m_c4CompressedSize)
                    {
                        facCIDImage().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kImgErrs::errcBmp_DeltaBeyondEnd
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Format
                        );
                    }

                    c1XDelta = pc1Data[c4SrcInd++];
                    c1YDelta = pc1Data[c4SrcInd++];

                    //
                    //  And now jump forward this many pixel positions in
                    //  the buffer.
                    //
                    nbiParse.c4Index += ((c1YDelta * c4CX)
                                        + c1XDelta) / 2;
                }
                 else
                {
                    //
                    //  Its an unencoded run. In order to make it more
                    //  efficient, we will do the whole bytes first and
                    //  handle any hanging nibbles separately.
                    //
                    for (c4RunInd = 0; c4RunInd < c1Byte2 / 2U; c4RunInd++)
                        NibbleSpooler(pc1Data[c4SrcInd++], nbiParse, kCIDLib::False);

                    //
                    //  If the run count was odd, then we have to deal with
                    //  the trailing nibble. We spool out just the low nibble
                    //  of the next byte and throw the top nibble away.
                    //
                    if ((c1Byte2 & 0x1))
                        NibbleSpooler(pc1Data[c4SrcInd++], nbiParse, kCIDLib::True);

                    //
                    //  If the number of full bytes in this run is odd,
                    //  then we need to skip a byte.
                    //
                    if ((c1Byte2 / 2) & 0x1)
                        c4SrcInd++;
                }
            }
             else
            {
                //
                //  This is the repeated byte value for this run. So we
                //  need to spool out this many run bytes to the target
                //  buffer.
                //
                NibbleSpooler(c1Byte2, c1CurMarker, nbiParse);
            }

            // And now we are back into marker state
            eState = EDecomp_Marker;
        }
    }

    //
    //  We return up above when we hit the end of data marker. If we get
    //  here then its an error because we did not hit the end of marker
    //  before we hit the end of the source data buffer.
    //
    facCIDImage().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kImgErrs::errcBmp_NoEndOfData
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Format
    );
}


tCIDLib::TVoid
TBitmapImage::ParseRLE8Bits(TBinInStream& strmFrom, TPixelArray& pixaBits)
{
    //
    //  Get a local copy of the compressed size, byte width, ect... for
    //  fast access and less typing.
    //
    const tCIDLib::TCard4 c4ByteWidth = c4LineByteWidth();
    const tCIDLib::TCard4 c4ImgSz = c4ImageSz();

    //
    //  Allocate a buffer big enough to read in the compressed data. We'll
    //  throw it away when we've uncompressed it, so put a janitor on it.
    //
    tCIDLib::TCard1* pc1Data = new tCIDLib::TCard1[m_c4CompressedSize];
    TArrayJanitor<tCIDLib::TCard1> janData(pc1Data);

    //
    //  Read in the compressed data. The default is to throw if the requested
    //  bytes are not read, which is what we want.
    //
    TBinInStream_ReadArray(strmFrom, pc1Data, m_c4CompressedSize);

    //
    //  We use a state machine to decompress, so lets start our state in the
    //  marker state since that's the first thing we should find.
    //
    EDecompStates eState = EDecomp_Marker;

    // Get a quickly pointer to the target buffer
    tCIDLib::TCard1* pc1ToFill = pixaBits.pc1Buffer();

    //
    //  Some state variables for the state machine. We have to track where
    //  we are decompressing to in the target buffer and the current marker.
    //
    tCIDLib::TCard1 c1CurMarker = 0;
    tCIDLib::TCard4 c4TargetInd = 0;
    tCIDLib::TCard4 c4RunInd;

    // And handle every byte in the buffer
    tCIDLib::TCard4 c4SrcInd = 0;
    while (c4SrcInd < m_c4CompressedSize)
    {
        if (eState == EDecomp_Marker)
        {
            c1CurMarker = pc1Data[c4SrcInd++];
            eState = EDecomp_Length;
        }
         else if (eState == EDecomp_Length)
        {
            tCIDLib::TCard1 c1Byte2 = pc1Data[c4SrcInd++];

            if (!c1CurMarker)
            {
                //
                //  The new byte is one of the special case markers or an
                //  unecoded run.
                //
                if (c1Byte2 == 0)
                {
                    //
                    //  Its an end of scan line marker, so we need to
                    //  move up to the next line in the output buffer.
                    //
                    c4TargetInd += c4TargetInd % c4ByteWidth;
                }
                 else if (c1Byte2 == 1)
                {
                    //
                    //  This is the end of encoded data marker. If we are
                    //  not at the last byte of the buffer, then something
                    //  is wrong.
                    //
                    if (c4SrcInd != m_c4CompressedSize)
                    {
                        facCIDImage().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kImgErrs::errcBmp_NotAtEnd
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Format
                        );
                    }

                    //
                    //  Return here so that any fall out below will indicate
                    //  a problem, since we never saw the end of data marker.
                    //
                    return;
                }
                 else if (c1Byte2 == 2)
                {
                    //
                    //  Its a delta marker, which tells us to jump to
                    //  somewhere else in the target buffer. We have to
                    //  play some magic here and read in the two delta bytes
                    //  and bump up the index manually.
                    //
                    tCIDLib::TCard1 c1XDelta, c1YDelta;

                    //
                    //  Sanity check that this would not go beyond the
                    //  available encoded data.
                    //
                    if (c4SrcInd + 2 >= m_c4CompressedSize)
                    {
                        facCIDImage().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kImgErrs::errcBmp_DeltaBeyondEnd
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Format
                        );
                    }

                    c1XDelta = pc1Data[c4SrcInd++];
                    c1YDelta = pc1Data[c4SrcInd++];

                    //
                    //  And now jump forward this many pixel positions in
                    //  the buffer.
                    //
                    c4TargetInd += (c1YDelta * c4ByteWidth) + c1XDelta;

                    if (c4TargetInd >= c4ImgSz)
                    {
                        facCIDImage().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kImgErrs::errcBmp_TargetOverflow
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Format
                            , TCardinal(c4ImgSz)
                        );
                    }
                }
                 else
                {
                    // Make sure we don't overflow the target buffer
                    if (c4TargetInd + c1Byte2 >= c4ImgSz)
                    {
                        facCIDImage().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kImgErrs::errcBmp_TargetOverflow
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Format
                            , TCardinal(c4ImgSz)
                        );
                    }

                    //
                    //  Its an unencoded run, so lets loop though the
                    //  indicated number of bytes and copy them to the
                    //  target buffer.
                    //
                    for (c4RunInd = 0; c4RunInd < c1Byte2; c4RunInd++)
                        pc1ToFill[c4TargetInd++] = pc1Data[c4SrcInd++];

                    //
                    //  If the run count was odd, then we have to skip
                    //  over a padding byte.
                    //
                    if (c1Byte2 & 0x1)
                        c4SrcInd++;
                }
            }
             else
            {
                //
                //  This is the repeated byte value for this run. So we
                //  need to spool out this many run bytes to the target
                //  buffer.
                //
                for (c4RunInd = 0; c4RunInd < c1CurMarker; c4RunInd++)
                    pc1ToFill[c4TargetInd++] = c1Byte2;
            }

            // And now we are back into marker state
            eState = EDecomp_Marker;
        }
    }

    //
    //  We return up above when we hit the end of data marker. If we get
    //  here then its an error because we did not hit the end of marker
    //  before we hit the end of the source data buffer.
    //
    facCIDImage().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kImgErrs::errcBmp_NoEndOfData
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Format
    );
}


tCIDLib::TVoid TBitmapImage::SetFromBH(const TBmpHdrInfo& bhiToSet)
{
    //
    //  Decide what the format flags should be for a bitmap of the indicated
    //  type, and adjust the bit depth where necessary.
    //
    tCIDImage::EPixFmts   eFmt;
    tCIDImage::EBitDepths eDepth;

    switch(bhiToSet.c4BitDepth)
    {
        case 1 :
        case 4 :
        case 8 :
        {
            // Gotta be palette based, keep the original depth
            eFmt = tCIDImage::EPixFmts::ClrPal;
            eDepth = tCIDImage::EBitDepths(bhiToSet.c4BitDepth);
            break;
        }

        case 16 :
        {
            // It's five bit per color RGB
            eFmt = tCIDImage::EPixFmts::TrueClr;
            eDepth = tCIDImage::EBitDepths::Five;
            break;
        }

        case 24 :
        case 32 :
        {
            //
            //  There is no 32 bit file format really. The alpha byte is not
            //  used, so we have to convert it to 24 bit data.
            //
            eFmt = tCIDImage::EPixFmts::TrueClr;
            eDepth = tCIDImage::EBitDepths::Eight;
            break;
        }
    };

    // Set our parent's members
    Set
    (
        eFmt
        , eDepth
        , bhiToSet.eRowOrder
        , TSize(bhiToSet.c4Width, bhiToSet.c4Height)
        , bhiToSet.c4ClrsImportant
        , bhiToSet.c4ClrsUsed
    );

    // Store some other stuff that we have
    szResolution(TSize(bhiToSet.c4HorzRes, bhiToSet.c4VertRes));

    // And then do our stuff
    m_c4CompressedSize = bhiToSet.c4CompressedSize;
    m_eBmpType = bhiToSet.eType;
    m_eCompression = bhiToSet.eCompression;
    m_c4BlueMask = bhiToSet.c4BlueMask;
    m_c4GreenMask = bhiToSet.c4GreenMask;
    m_c4RedMask = bhiToSet.c4RedMask;
}


//
//  Makes sure that the bit depth is legal for our type of image. If not, it
//  throws.
//
tCIDLib::TVoid TBitmapImage::TestBitDepth(const tCIDLib::TCard2 c2ToTest)
{
    if ((c2ToTest != 1) && (c2ToTest != 4) && (c2ToTest != 8)
    &&  (c2ToTest != 16) && (c2ToTest != 24) && (c2ToTest != 32))
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcImg_BadBitDepth
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(c2ToTest)
        );
    }
}

