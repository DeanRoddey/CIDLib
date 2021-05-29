//
// FILE NAME: CIDPNG_Image.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/19/2004
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
//  This file implements the TPNGImage class.
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
#include    "CIDPNG_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TPNGImage,TCIDImage)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDPNG_Image
    {
        // -----------------------------------------------------------------------
        //  Line filter constants
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard1   c1Filter_None   = 0;
        constexpr tCIDLib::TCard1   c1Filter_Sub    = 1;
        constexpr tCIDLib::TCard1   c1Filter_Up     = 2;
        constexpr tCIDLib::TCard1   c1Filter_Avg    = 3;
        constexpr tCIDLib::TCard1   c1Filter_Paeth  = 4;


        // -----------------------------------------------------------------------
        //  Contants from the data stream, some correctly flipped if necessary for
        //  the local endianness.
        // -----------------------------------------------------------------------
        #ifdef CIDLIB_LITTLEENDIAN
        constexpr tCIDLib::TCard8   c8FileMarker    = 0x0A1A0A0D474E5089;
        constexpr tCIDLib::TCard4   c4Chunk_BgnClr  = 0x624B4744;
        constexpr tCIDLib::TCard4   c4Chunk_Data    = 0x49444154;
        constexpr tCIDLib::TCard4   c4Chunk_End     = 0x49454E44;
        constexpr tCIDLib::TCard4   c4Chunk_Gamma   = 0x67414D41;
        constexpr tCIDLib::TCard4   c4Chunk_Header  = 0x49484452;
        constexpr tCIDLib::TCard4   c4Chunk_Palette = 0x504C5445;
        constexpr tCIDLib::TCard4   c4Chunk_Trans   = 0x74524E53;

        // Private chunk
        constexpr tCIDLib::TCard4   c4Chunk_TransClr= 0x63644243;

        #else
        constexpr tCIDLib::TCard4   c8FileMarker    = 0x89504E470D0A1A0A;
        constexpr tCIDLib::TCard4   c4Chunk_BgnClr  = 0x44474B62;
        constexpr tCIDLib::TCard4   c4Chunk_Data    = 0x54414449;
        constexpr tCIDLib::TCard4   c4Chunk_End     = 0x444E4549;
        constexpr tCIDLib::TCard4   c4Chunk_Gamma   = 0x414D4167;
        constexpr tCIDLib::TCard4   c4Chunk_Header  = 0x52444849;
        constexpr tCIDLib::TCard4   c4Chunk_Palette = 0x45544C50;
        constexpr tCIDLib::TCard4   c4Chunk_Trans   = 0x534E5274;

        // Private chunk
        constexpr tCIDLib::TCard4   c4Chunk_TransClr= 0x43426463;

        #endif

        //
        //  We screwed up the transparency chunk id and used this bad one in
        //  some early CQC 1.3.5 code. So we have to check for this on import and
        //  treat it the same as the correct one.
        //
        #ifdef CIDLIB_LITTLEENDIAN
        constexpr tCIDLib::TCard4   c4Chunk_BadTrans= 0x72524E53;
        #else
        constexpr tCIDLib::TCard4   c4Chunk_BadTrans= 0x534E5272;
        #endif


        // -----------------------------------------------------------------------
        //  For interlaced format, we have tables that tell us how many bits of
        //  each h/v 8 bits we are going to get on each pass, and how many rows
        //  we will get. That let's us calculate the expected width and height of
        //  the scan lines for each pass. The method that we call to load the data
        //  to the pixel array will be told the pass and he will have separate
        //  loops to pull out the data appropriately for each pass, so all we have
        //  to do is calculate the amount of data to pull out on each pass. The
        //  lines are always padded to the nearest byte, so we don't have to worry
        //  here about whether the last bits in the byte are really there or not.
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard4 ac4XCnt[7] = { 1, 1, 2, 2, 4, 4, 8 };
        constexpr tCIDLib::TCard4 ac4YCnt[7] = { 1, 1, 1, 2, 2, 4, 4 };

        // -----------------------------------------------------------------------
        //  There is a special case where some rows might not exist, because the
        //  image is less than 5 bits in one or both directions. But it depends on
        //  the offsets from the original of the 8x8 area as to whether this is
        //  true or not. So we keep a set of flags that indicate where the bits
        //  start for that pass.
        //
        //  And we need to keep up with, if the image is wide enough to get some
        //  bits, how many it will get if the image is less than
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard4 ac4XOfs[7] = { 0, 4, 0, 2, 0, 1, 0 };
        constexpr tCIDLib::TCard4 ac4YOfs[7] = { 0, 0, 4, 0, 2, 0, 1 };

        // -----------------------------------------------------------------------
        //  Many images will not be an even multiple of 8 samples, so we have to
        //  figure out how many pixels we'll get in that trailing fractional part
        //  of the interlace pattern. These tables tell us how many pixels we will
        //  get for pass X (first dimension) if we have Y trailing pixels.
        //
        //  So we multiply dim/8 times the correct pass entry in the Cnt arrays
        //  above for each dimention, then we look up [pass][(dim-1)%8] in this
        //  table and add it to the previous values.
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard4 ac4XLeft[7][8] =
        {
              { 1, 1, 1, 1, 1, 1, 1, 1 }
            , { 0, 0, 0, 0, 1, 1, 1, 1 }
            , { 1, 1, 1, 1, 2, 2, 2, 2 }
            , { 0, 0, 1, 1, 1, 1, 2, 2 }
            , { 1, 1, 2, 2, 3, 3, 4, 4 }
            , { 0, 1, 1, 2, 2, 3, 3, 4 }
            , { 1, 2, 3, 4, 5, 6, 7, 8 }
        };

        constexpr tCIDLib::TCard4 ac4YLeft[7][8] =
        {
              { 1, 1, 1, 1, 1, 1, 1, 1 }
            , { 1, 1, 1, 1, 1, 1, 1, 1 }
            , { 0, 0, 0, 0, 1, 1, 1, 1 }
            , { 1, 1, 1, 1, 2, 2, 2, 2 }
            , { 0, 0, 1, 1, 1, 1, 2, 2 }
            , { 1, 1, 2, 2, 3, 3, 4, 4 }
            , { 0, 1, 1, 2, 2, 3, 3, 4 }
        };
    }
}


// ---------------------------------------------------------------------------
//  Local methods
// ---------------------------------------------------------------------------

// An inlined Paeth predictor method for speed.
inline tCIDLib::TCard4 c4PaethAbs(  const   tCIDLib::TCard4 c4Lhs
                                    , const tCIDLib::TCard4 c4Rhs)
{
    tCIDLib::TInt4 i4Res = tCIDLib::TInt4(c4Lhs) - tCIDLib::TInt4(c4Rhs);
    if (i4Res < 0)
        i4Res *= -1;
    return tCIDLib::TCard4(i4Res);
}

inline tCIDLib::TCard4
c4PaethPredictor(const  tCIDLib::TCard4 c4Prev
                , const tCIDLib::TCard4 c4Above
                , const tCIDLib::TCard4 c4AbovePrev)
{
    tCIDLib::TCard4 c4Est = c4Prev + c4Above - c4AbovePrev;
    tCIDLib::TCard4 c4PEst = c4PaethAbs(c4Est, c4Prev);
    tCIDLib::TCard4 c4AEst = c4PaethAbs(c4Est, c4Above);
    tCIDLib::TCard4 c4APEst = c4PaethAbs(c4Est, c4AbovePrev);

    if ((c4PEst <= c4AEst) && (c4PEst <= c4APEst))
        return c4Prev;
    else if (c4AEst <= c4APEst)
        return c4Above;
    return c4AbovePrev;
}



// ---------------------------------------------------------------------------
//  CLASS: TPNGImage
// PREFIX: img
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPNGImage: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TPNGImage::QueryImgInfo(        TBinInStream&           strmIn
                        , const tCIDLib::TCard4         c4DataSz
                        ,       TSize&                  szImage
                        ,       tCIDImage::EPixFmts&    eImgFmt
                        ,       tCIDImage::EBitDepths&  eBitDepth
                        ,       tCIDPNG::EInterlaces&   eInterlace
                        ,       tCIDLib::TBoolean&      bTrans
                        ,       TRGBClr&                rgbTransClr
                        , const tCIDLib::TBoolean       bStrict)
{
    // First thing in the file is the file type indicator
    tCIDLib::TCard8 c8FileType;
    strmIn >> c8FileType;
    if (c8FileType != CIDPNG_Image::c8FileMarker)
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcBmp_UnknownFileType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , TCardinal64(c8FileType, tCIDLib::ERadices::Hex)
        );
    }

    //
    //  Call the main header parsing method, passing in the parms above
    //  for the stuff we want to return and just providing local dummies
    //  for the other stuff.
    //
    tCIDLib::TCard4         c4ClrsImportant;
    tCIDLib::TCard4         c4ClrsUsed;
    tCIDLib::TCard4         c4OrgDepth;
    tCIDLib::TCard4         c4SrcBitsPer;
    tCIDPNG::ECompTypes     eCompType;
    eParseHeader
    (
        strmIn
        , c4SrcBitsPer
        , eBitDepth
        , eImgFmt
        , szImage
        , eCompType
        , eInterlace
        , c4ClrsImportant
        , c4ClrsUsed
        , c4OrgDepth
        , bStrict
    );

    // Assume no transparency until we find it
    bTrans = kCIDLib::False;

    //
    //  Now we'll search for a transparency chunk. If we hit a data chunk,
    //  we know that's as far as we need to go.
    //
    tCIDLib::TCard4 c4CRC;
    tCIDLib::TCard4 c4DataLen;
    tCIDLib::TCard4 c4ChunkId;
    TClrPalette     palClrs(256);
    while(kCIDLib::True)
    {
        eParseChunkHdr(strmIn, c4CRC, c4DataLen, c4ChunkId);

        // If we hit the end, or data, then we are done
        if ((c4ChunkId == CIDPNG_Image::c4Chunk_Data)
        ||  (c4ChunkId == CIDPNG_Image::c4Chunk_End))
        {
            break;
        }

        //
        //  We have to handle a palette if present, since our private
        //  transparency chunk will be in terms of that palette.
        //
        if (c4ChunkId == CIDPNG_Image::c4Chunk_Palette)
        {
            ParsePalette(strmIn, c4CRC, c4DataLen, palClrs, bStrict);
        }
         else if (c4ChunkId == CIDPNG_Image::c4Chunk_TransClr)
        {
            // It's our transparency chunk, then take it and break out
            tCIDLib::TCard4 c4Color;
            ParsePrivTrans(strmIn, c4CRC, c4DataLen, c4Color, bStrict);
            bTrans = kCIDLib::True;

            //
            //  Our value is format specific, but the caller wants RGB, so
            //  we have to convert in some cases.
            //
            CvtTransClr(eImgFmt, eBitDepth, c4Color, palClrs, rgbTransClr);
            break;
        }
         else
        {
            // Nothing we care about, so skip this chunk
            SkipChunk(strmIn, c4CRC, c4DataLen, c4ChunkId, bStrict);
        }
    }
}


// ---------------------------------------------------------------------------
//  TPNGImage: Constructors and Destructor
// ---------------------------------------------------------------------------
TPNGImage::TPNGImage() :

    TCIDImage(tCIDImage::EImgTypes::PNG)
    , m_bStrictMode(kCIDLib::False)
    , m_eCompType(tCIDPNG::ECompTypes::Deflate)
    , m_eInterlaceType(tCIDPNG::EInterlaces::None)
{
}

TPNGImage::TPNGImage(const TPixelArray& pixaBits) :

    TCIDImage(tCIDImage::EImgTypes::PNG, pixaBits, 0, 0)
    , m_bStrictMode(kCIDLib::False)
    , m_eCompType(tCIDPNG::ECompTypes::Deflate)
    , m_eInterlaceType(tCIDPNG::EInterlaces::None)
{
    // We need our data to be top down, so if it's not, flip it
    ForceRowOrder(tCIDImage::ERowOrders::TopDown);
}

TPNGImage::TPNGImage(const  TPixelArray&    pixaBits
                    , const TClrPalette&    palToSet
                    , const tCIDLib::TCard4 c4ClrsImportant
                    , const tCIDLib::TCard4 c4ClrsUsed) :

    TCIDImage
    (
        tCIDImage::EImgTypes::PNG
        , pixaBits
        , palToSet
        , c4ClrsImportant
        , c4ClrsUsed
    )
    , m_bStrictMode(kCIDLib::False)
    , m_eCompType(tCIDPNG::ECompTypes::Deflate)
    , m_eInterlaceType(tCIDPNG::EInterlaces::None)
{
    // We need our data to be top down, so if it's not, flip it
    ForceRowOrder(tCIDImage::ERowOrders::TopDown);
}

TPNGImage::TPNGImage(const TCIDImage& imgSrc) :

    TCIDImage(tCIDImage::EImgTypes::PNG, imgSrc)
    , m_bStrictMode(kCIDLib::False)
    , m_eCompType(tCIDPNG::ECompTypes::Deflate)
    , m_eInterlaceType(tCIDPNG::EInterlaces::None)
{
    // We need our data to be top down, so if it's not, flip it
    ForceRowOrder(tCIDImage::ERowOrders::TopDown);
}

TPNGImage::TPNGImage(const  TCIDImage&  imgSrc
                    , const TSize&      szNew) :

    TCIDImage(tCIDImage::EImgTypes::PNG, imgSrc, szNew)
    , m_bStrictMode(kCIDLib::False)
    , m_eCompType(tCIDPNG::ECompTypes::Deflate)
    , m_eInterlaceType(tCIDPNG::EInterlaces::None)
{
}

TPNGImage::TPNGImage(const TPNGImage& imgSrc) :

    TCIDImage(tCIDImage::EImgTypes::PNG, imgSrc)
    , m_bStrictMode(imgSrc.m_bStrictMode)
    , m_eCompType(imgSrc.m_eCompType)
    , m_eInterlaceType(imgSrc.m_eInterlaceType)

{
}

TPNGImage::TPNGImage(TPNGImage&& imgSrc) :

    TPNGImage()
{
    *this = tCIDLib::ForceMove(imgSrc);
}

TPNGImage::~TPNGImage()
{
}


// ---------------------------------------------------------------------------
//  TPNGImage: Public operators
// ---------------------------------------------------------------------------
TPNGImage& TPNGImage::operator=(const TCIDImage& imgSrc)
{
    if (this != &imgSrc)
    {
        // Call our parent
        TParent::operator=(imgSrc);

        m_bStrictMode = kCIDLib::False;
        m_eCompType = tCIDPNG::ECompTypes::Deflate;
        m_eInterlaceType = tCIDPNG::EInterlaces::None;

        // We need our data to be top down, so if it's not, flip it
        ForceRowOrder(tCIDImage::ERowOrders::TopDown);
    }
    return *this;
}


TPNGImage& TPNGImage::operator=(const TPNGImage& imgSrc)
{
    if (this != &imgSrc)
    {
        // Call our parent
        TParent::operator=(imgSrc);

        // And do our own stuff
        m_bStrictMode    = imgSrc.m_bStrictMode;
        m_eCompType      = imgSrc.m_eCompType;
        m_eInterlaceType = imgSrc.m_eInterlaceType;
    }
    return *this;
}

TPNGImage& TPNGImage::operator=(TPNGImage&& imgSrc)
{
    if (this != &imgSrc)
    {
        TParent::operator=(tCIDLib::ForceMove(imgSrc));

        tCIDLib::Swap(m_bStrictMode, imgSrc.m_bStrictMode);
        tCIDLib::Swap(m_eCompType, imgSrc.m_eCompType);
        tCIDLib::Swap(m_eInterlaceType, imgSrc.m_eInterlaceType);
    }
    return *this;
}



// ---------------------------------------------------------------------------
//  TPNGImage: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TPNGImage::StreamFrom(TBinInStream& strmToReadFrom)
{
    //
    //  Ok, here we go. First thing in the file is the file type indicator.
    //
    tCIDLib::TCard8 c8FileType;
    strmToReadFrom >> c8FileType;
    if (c8FileType != CIDPNG_Image::c8FileMarker)
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcBmp_UnknownFileType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , TCardinal64(c8FileType, tCIDLib::ERadices::Hex)
        );
    }

    //
    //  Parse the header chunk, which must be first. This method is static
    //  and just returns everything in parms, because it is also used from
    //  a static method that just pulls out image info.
    //
    tCIDLib::TCard4         c4ClrsImportant;
    tCIDLib::TCard4         c4ClrsUsed;
    tCIDLib::TCard4         c4OrgDepth;
    tCIDLib::TCard4         c4SrcBitsPer;
    tCIDImage::EBitDepths   eTarDepth;
    tCIDImage::EPixFmts     eTarFmt;
    TSize                   szImg;

    const tCIDImage::EPixFmts eOrgFmt = eParseHeader
    (
        strmToReadFrom
        , c4SrcBitsPer
        , eTarDepth
        , eTarFmt
        , szImg
        , m_eCompType
        , m_eInterlaceType
        , c4ClrsImportant
        , c4ClrsUsed
        , c4OrgDepth
        , m_bStrictMode
    );

    // Set the stuff that goes to the base image info class
    if (tCIDLib::bAllBitsOn(eOrgFmt, tCIDImage::EPixFmts::Palette))
    {
        //
        //  Do the set here that indicates palette sizes, but doesn't provide
        //  one. We'll parse them out later.
        //
        Set
        (
            eTarFmt
            , eTarDepth
            , tCIDImage::ERowOrders::TopDown
            , szImg
            , c4ClrsImportant
            , c4ClrsUsed
        );
    }
     else if ((eOrgFmt == tCIDImage::EPixFmts::GrayScale)
          ||  ((eOrgFmt == tCIDImage::EPixFmts::GrayAlpha)))
    {
        //
        //  Create a palette to use later as a convenience to the outside
        //  world to convert the values to their RGB equiv.
        //
        const tCIDLib::TCard4 c4Clrs
        (
            c4ClrsImportant ? c4ClrsImportant : c4ClrsUsed
        );
        const tCIDLib::TCard1 c1Inc = tCIDLib::TCard1(255 / (c4Clrs - 1));

        TClrPalette palTmp(c4ClrsUsed);
        tCIDLib::TCard1 c1Cur = 0;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Clrs; c4Index++)
        {
            palTmp.SetAt(c4Index, c1Cur, c1Cur, c1Cur);
            c1Cur += c1Inc;
        }

        // And do the set that indicates the palette content
        Set
        (
            eTarFmt
            , eTarDepth
            , tCIDImage::ERowOrders::TopDown
            , szImg
            , palTmp
            , c4ClrsImportant
            , c4ClrsUsed
        );
    }
     else
    {
        // Non-special case, so just call the simple set of base info
        Set
        (
            eTarFmt
            , eTarDepth
            , tCIDImage::ERowOrders::TopDown
            , szImg
            , 0
            , 0
        );
    }

    tCIDLib::TBoolean bGotData = kCIDLib::False;
    tCIDLib::TBoolean bEndChunk = kCIDLib::False;
    while (!bEndChunk)
    {
        //
        //  Get the next chunk header. We get back our internal enumerated
        //  version of the chunk type, the raw (local byte order) chunk id,
        //  and the CRC that includes the type data (the first bytes that
        //  go into the CRC), and the bytes to expect for the next chunk.
        //
        tCIDLib::TCard4 c4CRC;
        tCIDLib::TCard4 c4Len;
        tCIDLib::TCard4 c4ChunkId;
        const tCIDPNG::EChunks eChunk = eParseChunkHdr
        (
            strmToReadFrom
            , c4CRC
            , c4Len
            , c4ChunkId
        );

        try
        {
            bEndChunk = bProcessChunk
            (
                strmToReadFrom
                , eChunk
                , c4CRC
                , c4Len
                , c4ChunkId
                , eOrgFmt
                , c4SrcBitsPer
                , c4OrgDepth
            );
        }

        catch(TError& errToCatch)
        {

            // If strict mode or we haven't gotten our data yet, just rethrow
            if (m_bStrictMode || !bGotData)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                throw;
            }

            // Just break out with what we have
            break;
        }

        // Remember if we've gotten through the data chunk
        if (!bGotData)
            bGotData = (tCIDLib::c4EnumOrd(eChunk) == CIDPNG_Image::c4Chunk_Data);
    }
}

tCIDLib::TVoid TPNGImage::StreamTo(TBinOutStream& strmToWriteTo) const
{
    //
    //  Writing out, we don't attempt to completely recreate the original,
    //  we just write it out in whatever format we ended up supporting
    //  internally. This can mean a loss of information if the incoming
    //  was beyond Windows' ability to handle and we reduced it.
    //
    //  Start by writing out the magic file marker.
    //
    strmToWriteTo << CIDPNG_Image::c8FileMarker;

    // Get the size of the image we are going to process
    const tCIDLib::TCard4 c4CY = c4Height();
    const tCIDLib::TCard4 c4CX = c4Width();

    //
    //  Get the format out. We have to modify the format in some cases
    //  becasue the PNG format doesn't support all the possible bitmap
    //  layouts.
    //
    tCIDImage::EPixFmts eFmt = ePixFmt();
    tCIDLib::TCard1     c1Depth = tCIDLib::TCard1(eBitDepth());

    //
    //  Most likely we'll take the current format, but if PNG doesn't
    //  support it, we'll have to convert. So we keep both source and
    //  target depths.
    //
    tCIDImage::EPixFmts     eSrcFmt = ePixFmt();
    tCIDImage::EBitDepths   eSrcDepth = eBitDepth();

    tCIDImage::EPixFmts     eTarFmt = eSrcFmt;
    tCIDImage::EBitDepths   eTarDepth = eSrcDepth;

    //
    //  PNG doesn't support 5 bit per color component type true color, so
    //  convert to 8 bit per.
    //
    if ((eTarFmt == tCIDImage::EPixFmts::TrueClr)
    &&  (eTarDepth == tCIDImage::EBitDepths::Five))
    {
        eTarDepth = tCIDImage::EBitDepths::Eight;
    }

    //
    //  Now do the header info.
    //
    //  NOTE:   We don't ever write out images in interlaced format, so
    //          we hard code a zero for the interlace type, and don't use
    //          our own value which is what was in the file we read in.
    //
    //          And we store the target format and depth here, which are
    //          what we are going to write to the file.
    //
    PutCard4(13, strmToWriteTo);
    tCIDLib::TCard4 c4CRC = kCIDLib::c4MaxCard;
    PutCard4(CIDPNG_Image::c4Chunk_Header, strmToWriteTo, c4CRC);
    PutCard4(c4CX, strmToWriteTo, c4CRC);
    PutCard4(c4CY, strmToWriteTo, c4CRC);
    PutCard1(tCIDLib::TCard1(eTarDepth), strmToWriteTo, c4CRC);
    PutCard1(tCIDLib::TCard1(eTarFmt), strmToWriteTo, c4CRC);
    PutCard1(0, strmToWriteTo, c4CRC);
    PutCard1(0, strmToWriteTo, c4CRC);
    PutCard1(0, strmToWriteTo, c4CRC);
    PutCard4(c4CRC ^ kCIDLib::c4MaxCard, strmToWriteTo);

    //
    //  Write out a gamma chunk. If we read in n image with gamma, then
    //  that's what we'll write out. It is supposed to be supposed to be
    //  before the palette if present.
    //
    //  NOTE:   What is stored is the exponent, i.e. 1.0, 2.2, 2,3, ect...
    //          but what PNG wants is the reciprocal so we have to divide
    //          the stored gamma into 1.0.
    //
    PutCard4(4, strmToWriteTo);
    c4CRC = kCIDLib::c4MaxCard;
    PutCard4(CIDPNG_Image::c4Chunk_Gamma, strmToWriteTo, c4CRC);
    tCIDLib::TCard4 c4Gamma = tCIDLib::TCard4((1.0 / f8Gamma()) * 100000);
    PutCard4(c4Gamma, strmToWriteTo, c4CRC);
    PutCard4(c4CRC ^ kCIDLib::c4MaxCard, strmToWriteTo);

    // If we have a palette, then we need to write it out next
    if (tCIDLib::bAllBitsOn(eSrcFmt, tCIDImage::EPixFmts::Palette))
    {
        const TClrPalette& palSrc = palClrs();
        const tCIDLib::TCard4 c4Count = palSrc.c4ClrCount();
        PutCard4(c4Count * 3, strmToWriteTo);
        c4CRC = kCIDLib::c4MaxCard;
        PutCard4(CIDPNG_Image::c4Chunk_Palette, strmToWriteTo, c4CRC);
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TRGBClr& rgbCur = palSrc.rgbAt(c4Index);
            PutCard1(rgbCur.c1Red(), strmToWriteTo, c4CRC);
            PutCard1(rgbCur.c1Green(), strmToWriteTo, c4CRC);
            PutCard1(rgbCur.c1Blue(), strmToWriteTo, c4CRC);
        }
        PutCard4(c4CRC ^ kCIDLib::c4MaxCard, strmToWriteTo);
    }

    //
    //  If we have a transparency color, then write out a chunk for that.
    //  We write out a standard PNG one if it's one we can support, and we
    //  write out a private chunk with our style of info.
    //
    if (bTransClr())
        WriteTransClr(strmToWriteTo, c4CRC);

    //
    //  We get back the actual size from this. He will use a chunked output
    //  stream to build up the compressed data, and tehn allocate our buffer
    //  large enough to hold the data, so that it's reasonably efficient.
    //
    THeapBuf mbufComp(8);
    const tCIDLib::TCard4 c4DataSz = c4StoreCompBuf
    (
        mbufComp, eSrcFmt, eSrcDepth, eTarFmt, eTarDepth
    );

    // Now we can write out the data chunk
    PutCard4(c4DataSz, strmToWriteTo);
    c4CRC = kCIDLib::c4MaxCard;
    PutCard4(CIDPNG_Image::c4Chunk_Data, strmToWriteTo, c4CRC);
    c4CRC = TRawMem::hshHashBuffer3309(c4CRC, mbufComp.pc1Data(), c4DataSz);
    strmToWriteTo.c4WriteRawBuffer(mbufComp.pc1Data(), c4DataSz);
    PutCard4(c4CRC ^ kCIDLib::c4MaxCard, strmToWriteTo);

    // Write out the end marker, which has no data
    PutCard4(0, strmToWriteTo);
    c4CRC = kCIDLib::c4MaxCard;
    PutCard4(CIDPNG_Image::c4Chunk_End, strmToWriteTo, c4CRC);
    PutCard4(c4CRC ^ kCIDLib::c4MaxCard, strmToWriteTo);
}



// ---------------------------------------------------------------------------
//  TPNGImage: Private, static methods
// ---------------------------------------------------------------------------

//
//  After each chunk's data is parsed out, this is called to check the CRC,
//  which is in the trailing 4 bytes of each chunk.
//
tCIDLib::TBoolean
TPNGImage::bCheckCRC(       TBinInStream&       strmToReadFrom
                    , const tCIDLib::TCard4     c4CRCToCheck
                    , const tCIDLib::TCard4     c4ChunkId
                    , const tCIDLib::TBoolean   bStrict)
{
    // Use a dummy CRC when getting the CRC out
    tCIDLib::TCard4 c4Dummy = kCIDLib::c4MaxCard;
    tCIDLib::TCard4 c4SrcCRC = c4GetNext(strmToReadFrom, c4Dummy);

    // Do the final flip of our CRC
    tCIDLib::TCard4 c4CRC = c4CRCToCheck ^ kCIDLib::c4MaxCard;

    if (c4SrcCRC != c4CRC)
    {
        if (bStrict)
        {
            facCIDPNG().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kPNGErrs::errcPNG_BadChunkCRC
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(c4ChunkId, tCIDLib::ERadices::Hex)
            );
        }
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



//
//  These methods handle getting various sized values out of the stream and
//  handling byte swapping if required. It also updates a passed CRC value
//  that has to be kept up with for each chunk.
//
tCIDLib::TCard1 TPNGImage::c1GetNext(TBinInStream&      strmToReadFrom
                                    , tCIDLib::TCard4&  c4CRC)
{
    // Read in the next byte and update the CRC
    tCIDLib::TCard1 c1Ret;
    strmToReadFrom >> c1Ret;
    c4CRC = TRawMem::hshHashBuffer3309(c4CRC, &c1Ret, 1);
    return c1Ret;
}


tCIDLib::TCard2 TPNGImage::c2GetNext(TBinInStream&      strmToReadFrom
                                    , tCIDLib::TCard4&  c4CRC)
{
    // Read in the next two bytes and update the CRC
    tCIDLib::TCard1 ac1Buf[2];
    strmToReadFrom.c4ReadRawBuffer(ac1Buf, 2);
    c4CRC = TRawMem::hshHashBuffer3309(c4CRC, ac1Buf, 2);

    // Convert to Card1, byte swap if on little endian host
    tCIDLib::TCard2 c2Ret = *(tCIDLib::TCard2*)ac1Buf;
    #ifdef CIDLIB_LITTLEENDIAN
    c2Ret = TRawBits::c2SwapBytes(c2Ret);
    #endif
    return c2Ret;
}


tCIDLib::TCard4 TPNGImage::c4GetNext(TBinInStream&      strmToReadFrom
                                    , tCIDLib::TCard4&  c4CRC)
{
    // Read in the next 4 bytes and update the CRC
    tCIDLib::TCard1 ac1Buf[4];
    strmToReadFrom.c4ReadRawBuffer(ac1Buf, 4);
    c4CRC = TRawMem::hshHashBuffer3309(c4CRC, ac1Buf, 4);

    // Convert to Card1, byte swap if on little endian host
    tCIDLib::TCard4 c4Ret = *(tCIDLib::TCard4*)ac1Buf;
    #ifdef CIDLIB_LITTLEENDIAN
    c4Ret = TRawBits::c4SwapBytes(c4Ret);
    #endif
    return c4Ret;
}


//
//  This method will parse the first two fields of the next chunk, convert
//  the type field to our enumeration for return, return the expected len
//  of the data bytes of the new chunk, and return the starting CRC value
//  for the chunk, to be used during the subsequent parsing.
//
tCIDPNG::EChunks
TPNGImage::eParseChunkHdr(  TBinInStream&       strmToReadFrom
                            , tCIDLib::TCard4&  c4CRC
                            , tCIDLib::TCard4&  c4DataLen
                            , tCIDLib::TCard4&  c4ChunkId)
{
    //
    //  Get the data length. It's not included in the chunk CRC, so we don't
    //  need to reset the CRC yet.
    //
    c4DataLen = c4GetNext(strmToReadFrom, c4CRC);

    // Now reset the CRC and get the type
    c4CRC = kCIDLib::c4MaxCard;
    c4ChunkId = c4GetNext(strmToReadFrom, c4CRC);

    // Convert the marker to a chunk header enum, or unknown if we don't know it
    tCIDPNG::EChunks eRet = tCIDPNG::EChunks::None;
    switch(c4ChunkId)
    {
        case CIDPNG_Image::c4Chunk_BgnClr :
            eRet = tCIDPNG::EChunks::BgnClr;
            break;

        case CIDPNG_Image::c4Chunk_End :
            eRet = tCIDPNG::EChunks::End;
            break;

        case CIDPNG_Image::c4Chunk_Data :
            eRet = tCIDPNG::EChunks::Data;
            break;

        case CIDPNG_Image::c4Chunk_Gamma :
            eRet = tCIDPNG::EChunks::Gamma;
            break;

        case CIDPNG_Image::c4Chunk_Header :
            eRet = tCIDPNG::EChunks::Header;
            break;

        case CIDPNG_Image::c4Chunk_Palette :
            eRet = tCIDPNG::EChunks::Palette;
            break;

        case CIDPNG_Image::c4Chunk_BadTrans :
        case CIDPNG_Image::c4Chunk_Trans :
            //
            //  Note that we had a bad transparency chunk id for a while, so
            //  we look for both on read and treat them the same, so that any
            //  images we wrote out will be treated correcty now that we've
            //  fixed this.
            //
            eRet = tCIDPNG::EChunks::Trans;
            break;

        case CIDPNG_Image::c4Chunk_TransClr:
            eRet = tCIDPNG::EChunks::TransClr;
            break;

        default :
            // Already set to unkonwn
            break;
    };
    return eRet;
}


//
//  This method will parse the main image header and returns the relevant
//  info in the passed parameters. This has to be called from both the static
//  image info query and from the regular streaming, it so has to be a static
//  and just return data as parameters.
//
tCIDImage::EPixFmts
TPNGImage::eParseHeader(TBinInStream&               strmToReadFrom
                        , tCIDLib::TCard4&          c4SrcBitsPer
                        , tCIDImage::EBitDepths&    eBitDepth
                        , tCIDImage::EPixFmts&      ePixFmt
                        , TSize&                    szImg
                        , tCIDPNG::ECompTypes&      eCompType
                        , tCIDPNG::EInterlaces&     eInterlaceType
                        , tCIDLib::TCard4&          c4ClrsImportant
                        , tCIDLib::TCard4&          c4ClrsUsed
                        , tCIDLib::TCard4&          c4OrgDepth
                        , const tCIDLib::TBoolean   bStrict)
{
    // Get the next chunk header info, which must be the header in this case
    tCIDLib::TCard4 c4CRC;
    tCIDLib::TCard4 c4Len;
    tCIDLib::TCard4 c4Id;
    if (eParseChunkHdr(strmToReadFrom, c4CRC, c4Len, c4Id) != tCIDPNG::EChunks::Header)
    {
        facCIDPNG().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kPNGErrs::errcPNG_ExpectedHdr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Pull out the header info and store it in our image info object
    const tCIDLib::TCard4 c4Width = c4GetNext(strmToReadFrom, c4CRC);
    const tCIDLib::TCard4 c4Height = c4GetNext(strmToReadFrom, c4CRC);
    c4OrgDepth = c1GetNext(strmToReadFrom, c4CRC);
    const tCIDLib::TCard1 c1ClrType = c1GetNext(strmToReadFrom, c4CRC);
    const tCIDLib::TCard1 c1Compression = c1GetNext(strmToReadFrom, c4CRC);
    c1GetNext(strmToReadFrom, c4CRC);
    const tCIDLib::TCard1 c1Interlace = c1GetNext(strmToReadFrom, c4CRC);

    // Update the caller's image size parm
    szImg.Set(c4Width, c4Height);

    // Validate the color type, which drives a lot of stuff below
    if (c1ClrType & ~tCIDLib::c4EnumOrd(tCIDImage::EPixFmts::ValidBits))
    {
        facCIDPNG().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kPNGErrs::errcPNG_BadClrType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c1ClrType)
        );
    }

    //
    //  Calculate the bits bits per pixel in the original source content,
    //  and determine the pixel format we'll use. We cannot represent the
    //  16 bit true color or color/alpha formats, so we reduce them to
    //  8 bit (i.e. 24/32 bit color.)
    //
    //  We need to calc the original source stuff in order to parse data
    //  out of the data chunk later. We need to set our stuff so that we
    //  can initialize our parent class with our desired format.
    //
    c4ClrsUsed = 0;
    c4ClrsImportant = 0;

    c4SrcBitsPer = c4OrgDepth;
    if (c1ClrType & tCIDLib::c4EnumOrd(tCIDImage::EPixFmts::Palette))
    {
        // For all palette foramts, it's just the actual bit depth
        ePixFmt = tCIDImage::EPixFmts::ClrPal;
        eBitDepth = tCIDImage::EBitDepths(c4OrgDepth);
        c4ClrsUsed = 0x1UL << c4OrgDepth;
        c4ClrsImportant = 0;
    }
     else if (c1ClrType & tCIDLib::c4EnumOrd(tCIDImage::EPixFmts::Color))
    {
        //
        //  Our target depth is 8 bit per sample, even if the original is
        //  16 bit. We'll down convert it in that case.
        //
        eBitDepth = tCIDImage::EBitDepths::Eight;

        // The source has at least three samples of the original depth
        c4SrcBitsPer = (c4OrgDepth * 3);

        // If alpha, then bump up source size by another depth
        if (c1ClrType & tCIDLib::c4EnumOrd(tCIDImage::EPixFmts::Alpha))
        {
            ePixFmt = tCIDImage::EPixFmts::TrueAlpha;
            c4SrcBitsPer += c4OrgDepth;
        }
         else
        {
            ePixFmt = tCIDImage::EPixFmts::TrueClr;
        }
    }
     else
    {
        // It's gray scale. We take all these as is
        c4SrcBitsPer = c4OrgDepth;
        eBitDepth = tCIDImage::EBitDepths(c4OrgDepth);
        if (c1ClrType & tCIDLib::c4EnumOrd(tCIDImage::EPixFmts::Alpha))
        {
            ePixFmt = tCIDImage::EPixFmts::GrayAlpha;
            c4SrcBitsPer += c4OrgDepth;
        }
         else
        {
            ePixFmt = tCIDImage::EPixFmts::GrayScale;
        }

        if (c4OrgDepth < 16)
        {
            c4ClrsUsed = 0x1UL << c4OrgDepth;
            c4ClrsImportant = 0;
        }
         else
        {
            c4ClrsUsed = 256;
            c4ClrsImportant = 0;
        }
    }

    // And store our own stuff. Start by checking the compression
    if (c1Compression >= tCIDLib::c4EnumOrd(tCIDPNG::ECompTypes::Count))
    {
        facCIDPNG().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kPNGErrs::errcPNG_BadCompType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c1Compression)
        );
    }
    eCompType = tCIDPNG::ECompTypes::Deflate;

    // Do the interlace style
    if (c1Interlace >= tCIDLib::c4EnumOrd(tCIDPNG::EInterlaces::Count))
    {
        facCIDPNG().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kPNGErrs::errcPNG_BadInterlType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c1Interlace)
        );
    }
    eInterlaceType = tCIDPNG::EInterlaces(c1Interlace);

    // Check the CRC now
    bCheckCRC(strmToReadFrom, c4CRC, CIDPNG_Image::c4Chunk_Header, bStrict);

    //
    //  Return the original format of the PNG data. This is not stored,
    //  it is just used during decoding.
    //
    return tCIDImage::EPixFmts(c1ClrType);
}


tCIDLib::TVoid
TPNGImage::ParsePalette(        TBinInStream&       strmToReadFrom
                        ,       tCIDLib::TCard4&    c4CRC
                        , const tCIDLib::TCard4     c4Len
                        ,       TClrPalette&        palTar
                        , const tCIDLib::TBoolean   bStrict)
{
    //
    //  Calculate how many palette entries there are, which we can do based
    //  size, since it must be a multiple of 3 bytes.
    //
    const tCIDLib::TCard4 c4Count = c4Len / 3;

    //
    //  Make sure it really is an even multiple of 3 and that the palette is
    //  large enough.
    //
    if ((c4Len % 3) || (palTar.c4ClrCount() < c4Count))
    {
        facCIDPNG().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kPNGErrs::errcPNG_BadPalSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Run through and pull them out and load them into the palette
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        tCIDLib::TCard1 c1Red = c1GetNext(strmToReadFrom, c4CRC);
        tCIDLib::TCard1 c1Green = c1GetNext(strmToReadFrom, c4CRC);
        tCIDLib::TCard1 c1Blue = c1GetNext(strmToReadFrom, c4CRC);
        palTar.SetAt(c4Index, c1Red, c1Green, c1Blue);
    }

    // Check the CRC now
    bCheckCRC(strmToReadFrom, c4CRC, CIDPNG_Image::c4Chunk_Palette, bStrict);
}


tCIDLib::TVoid
TPNGImage::ParsePrivTrans(          TBinInStream&       strmToReadFrom
                            ,       tCIDLib::TCard4&    c4CRC
                            , const tCIDLib::TCard4     c4Len
                            ,       tCIDLib::TCard4&    c4Color
                            , const tCIDLib::TBoolean   bStrict)
{
    // It should be 4 bytes. If not just skip it
    if (c4Len != 4)
    {
        SkipChunk
        (
            strmToReadFrom
            , c4CRC
            , c4Len
            , CIDPNG_Image::c4Chunk_TransClr
            , kCIDLib::False
        );
        return;
    }

    c4Color = c4GetNext(strmToReadFrom, c4CRC);
    bCheckCRC(strmToReadFrom, c4CRC, CIDPNG_Image::c4Chunk_TransClr, bStrict);
}


tCIDLib::TVoid
TPNGImage::SkipChunk(       TBinInStream&       strmToReadFrom
                    ,       tCIDLib::TCard4&    c4CRC
                    , const tCIDLib::TCard4     c4ToSkip
                    , const tCIDLib::TCard4     c4ChunkId
                    , const tCIDLib::TBoolean   bStrict)
{
    //
    //  Read in the data of the chunk and calculate the CRC, so that we know
    //  at least that it is valid, though we are not processing it. Since it
    //  could be arbitrarily large, we read it in 1K chunks.
    //
    const tCIDLib::TCard4 c4BufSz = 1024;
    tCIDLib::TCard1 ac1Buf[c4BufSz];
    tCIDLib::TCard4 c4Count = 0;

    while (c4Count < c4ToSkip)
    {
        // Read up to another 1K chunk
        const tCIDLib::TCard4 c4Read = tCIDLib::MinVal(1024UL, c4ToSkip - c4Count);
        strmToReadFrom.c4ReadRawBuffer(ac1Buf, c4Read);
        c4Count += c4Read;
        c4CRC = TRawMem::hshHashBuffer3309(c4CRC, ac1Buf, c4Read);
    }

    // Validate the CRC
    bCheckCRC(strmToReadFrom, c4CRC, c4ChunkId, bStrict);
}


// ---------------------------------------------------------------------------
//  TPNGImage: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TPNGImage::bParseData(          TBinInStream&       strmToReadFrom
                        ,       tCIDLib::TCard4&    c4CRC
                        , const tCIDLib::TCard4     c4Len
                        , const tCIDImage::EPixFmts eOrgFmt
                        , const tCIDLib::TCard4     c4SrcBitsPer
                        , const tCIDLib::TCard4     c4OrgDepth)
{
    tCIDLib::TCard4 c4CurLen = c4Len;

    TChunkedBinOutStream strmCompData(32 * (1024 * 1024));
    const tCIDLib::TCard4 c4BufSz = 4096;
    tCIDLib::TCard1 ac1Buf[c4BufSz];
    tCIDLib::TBoolean bEndChunk = kCIDLib::False;
    while (kCIDLib::True)
    {
        //
        //  Read in the current data chunk, updating the CRC as we go. We
        //  use a local 4K buffer to read the data in and copy over to the
        //  mbufComp buffer.
        //
        tCIDLib::TCard4 c4Count = 0;
        while (c4Count < c4CurLen)
        {
            // Read up to another 4K chunk
            const tCIDLib::TCard4 c4Read = tCIDLib::MinVal(4096UL, c4CurLen - c4Count);
            strmToReadFrom.c4ReadRawBuffer(ac1Buf, c4Read);
            c4CRC = TRawMem::hshHashBuffer3309(c4CRC, ac1Buf, c4Read);
            strmCompData.c4WriteRawBuffer(ac1Buf, c4Read);

            // Keep up with now much we read in this chunk
            c4Count += c4Read;
        }

        // Check the CRC of this chunk
        bCheckCRC(strmToReadFrom, c4CRC, CIDPNG_Image::c4Chunk_Data, m_bStrictMode);

        //
        //  Get another chunk. If it's not a data chunk, process it and break
        //  out since we are done with data. Else, just loop around and add
        //  more data to our compressed data buffer.
        //
        tCIDLib::TCard4 c4ChunkId;
        const tCIDPNG::EChunks eChunk = eParseChunkHdr
        (
            strmToReadFrom, c4CRC, c4CurLen, c4ChunkId
        );

        if (eChunk != tCIDPNG::EChunks::Data)
        {
            try
            {
                bEndChunk = bProcessChunk
                (
                    strmToReadFrom
                    , eChunk
                    , c4CRC
                    , c4CurLen
                    , c4ChunkId
                    , eOrgFmt
                    , c4SrcBitsPer
                    , c4OrgDepth
                );
            }

            catch(TError& errToCatch)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);

                // If strict mode, rethrow, else fall through with the data
                if (m_bStrictMode)
                    throw;
            }
            break;
        }
    }

    TChunkedBinOutStream strmDecomp(32 * (1024 * 1024));
    tCIDLib::TCard4 c4DecompSz = 0;
    {
        //
        //  Create an input stream linked to the original compressed data output
        //  stream we loaded the data into above. We'll use this to feed the
        //  data to the compressor.
        //
        strmCompData.Flush();
        TChunkedBinInStream strmSrc(strmCompData);

        TZLibCompressor zlibPNG;
        c4DecompSz = zlibPNG.c4Decompress(strmSrc, strmDecomp);
    }

    //
    //  Now we can allocate an buffer for the decompressed data and copy it over
    //  from the chunked stream. We know how large it needs to be low.
    //
    THeapBuf mbufDecomp(c4DecompSz, c4DecompSz);
    if (strmDecomp.c4CopyOutTo(mbufDecomp, 0) != c4DecompSz)
    {
        CIDAssert2(L"Streamed size != decomp size");
    }

    //
    //  Ok, we now have the raw decompresesd data and it's size. So using
    //  the header info, we can set up the pixel array and transform the
    //  PNG data into our array.
    //
    LoadPixelArray(mbufDecomp, eOrgFmt, c4SrcBitsPer, c4OrgDepth);

    return bEndChunk;
}


tCIDLib::TBoolean
TPNGImage::bProcessChunk(       TBinInStream&       strmToReadFrom
                        , const tCIDPNG::EChunks    eChunk
                        ,       tCIDLib::TCard4&    c4CRC
                        , const tCIDLib::TCard4     c4Len
                        , const tCIDLib::TCard4     c4ChunkId
                        , const tCIDImage::EPixFmts   eOrgFmt
                        , const tCIDLib::TCard4     c4SrcBitsPer
                        , const tCIDLib::TCard4     c4OrgDepth)
{
    tCIDLib::TBoolean bEndChunk = kCIDLib::False;
    switch(eChunk)
    {
        case tCIDPNG::EChunks::BgnClr :
        {
            ParseBgnClr(strmToReadFrom, c4CRC, c4Len, eOrgFmt, c4OrgDepth);
            break;
        }

        case tCIDPNG::EChunks::Data :
        {
            //
            //  These must all be contiguous if more than one are present, so
            //  this guy will process them all before returning. Since he has
            //  to look ahead, he may see the end chunk and returns true if so.
            //
            bEndChunk = bParseData
            (
                strmToReadFrom, c4CRC, c4Len, eOrgFmt, c4SrcBitsPer, c4OrgDepth
            );
            break;
        }

        case tCIDPNG::EChunks::End :
        {
            bEndChunk = kCIDLib::True;
            break;
        }

        case tCIDPNG::EChunks::Gamma :
        {
            ParseGamma(strmToReadFrom, c4CRC, c4Len);
            break;
        }

        case tCIDPNG::EChunks::Palette :
        {
            TClrPalette& palTar = palClrs();
            ParsePalette(strmToReadFrom, c4CRC, c4Len, palTar, m_bStrictMode);
            break;
        }

        case tCIDPNG::EChunks::Trans :
        {
            ParseTransparency(strmToReadFrom, c4CRC, c4Len);
            break;
        }

        case tCIDPNG::EChunks::TransClr :
        {
            // This one is our private transparency color chunk
            tCIDLib::TCard4 c4Color;
            ParsePrivTrans(strmToReadFrom, c4CRC, c4Len, c4Color, m_bStrictMode);

            // Indicate we have a transparency color and store it
            bTransClr(kCIDLib::True);
            c4TransClr(c4Color);
            break;
        }

        default :
        {
            // Nothing we know, so just skip forward
            SkipChunk(strmToReadFrom, c4CRC, c4Len, c4ChunkId, m_bStrictMode);
            break;
        }
    };
    return bEndChunk;
}


tCIDLib::TCard4
TPNGImage::c4StoreCompBuf(          THeapBuf&               mbufToFill
                            , const tCIDImage::EPixFmts     eSrcFmt
                            , const tCIDImage::EBitDepths   eSrcDepth
                            , const tCIDImage::EPixFmts     eTarFmt
                            , const tCIDImage::EBitDepths   eTarDepth) const
{
    //
    //  Here we are writing out, so we want to use the format that we
    //  created in our pixel array. So we lose the ability to write back out
    //  16 bit true/true-alpha stuff because we downconverted it to 8 bit
    //  per color/alpha format.
    //
    const tCIDLib::TCard4     c4CY = c4Height();
    const tCIDLib::TCard4     c4CX = c4Width();
    const TPixelArray&        pixaSrc = pixaBits();

    //
    //  Get the actual storage per scan line for the format we are going to write it
    //  out in. We need this to calculate the target buffer size of the uncompressed
    //  data, and to get to the previous line for any filtering that refers back to
    //  the previous line.
    //
    //  Note that the PNG line format doesn't pad scan lines except to the extent that
    //  the last part of a byte might be unused. So we cannot use the pixel array which
    //  could have (and probably will have)  a different padding since they are generally
    //  set up to have padding for passing directly in to create a host bitmap.
    //
    //  So we indicate a padding of 1 when we calculate the line width to account for a
    //  partial trailing byte in the buffer we allocate. That will cover a worst case
    //  scenario.
    //
    const tCIDLib::TCard4 c4TarLineWidth = TPixelArray::c4CalcLineWidth
    (
        c4CX, eTarFmt, eTarDepth, 1
    );

    //
    //  Calculate the full size of the uncompressed data buffer and allocate the buffer.
    //  Go ahead and fully allocate it since we are going to use it all.
    //
    //  Add one to the width for the filter byte that goes on each scan line.
    //
    //  Watch for psycho scenario of a zero size, which would make the memory buffer
    //  constructor freak out.
    //
    tCIDLib::TCard4 c4UncompSz = (c4TarLineWidth + 1) * c4CY;
    if (!c4UncompSz)
        c4UncompSz = 8;
    THeapBuf mbufUncomp(c4UncompSz, c4UncompSz);

    //
    //  And now load up the data into the buffer. This is fairly complex and involves
    //  filtering and all that.
    //
    tCIDLib::TCard4 c4TarInd = 0;

    if (tCIDLib::bAllBitsOn(eSrcFmt, tCIDImage::EPixFmts::Palette)
    ||  ((eSrcFmt == tCIDImage::EPixFmts::GrayScale) && (tCIDLib::c4EnumOrd(eSrcDepth) < 16)))
    {
        tCIDLib::TCard4 c4Bits;
        tCIDLib::TCard1 c1Cur;
        tCIDLib::TCard1 c1Prev;

        //
        //  It's palette based or it's one of the gray scales less than
        //  16 bits and without alpha. We can treat all of these the same
        //  for our purposes here since they are all just single numeric
        //  values per pixel of a byte or less per pixel.
        //
        //  We'll handle 8 bit gray with alpha and the 16 bit gray scales
        //  below.
        //
        //  None of these, being 1 byte or less, have any byte order
        //  concerns. We can handle 1, 2, and 4 bit depths all via the
        //  sample generic loop, but we do the 8 bit version separately
        //  for better efficiency.
        //
        //  We do sub type filtering on all these.
        //
        if (eTarDepth < tCIDImage::EBitDepths::Eight)
        {
            for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4CY; c4YInd++)
            {
                mbufUncomp.PutCard1(CIDPNG_Image::c1Filter_Sub, c4TarInd++);
                c4Bits = 0;
                c1Cur = 0;
                c1Prev = 0;
                for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4CX; c4XInd++)
                {
                    c1Cur <<= tCIDLib::TCard1(eTarDepth);
                    c1Cur |= tCIDLib::TCard1(pixaSrc.c4At(c4XInd, c4YInd));
                    c4Bits += tCIDLib::TCard4(eTarDepth);

                    if (c4Bits == 8)
                    {
                        mbufUncomp.PutCard1(c1Cur - c1Prev, c4TarInd++);
                        c4Bits = 0;
                        c1Prev = c1Cur;
                    }
                }

                // If any spare bits left over, put them out
                if (c4Bits)
                {
                    c1Cur <<= tCIDLib::TCard1(eTarDepth);
                    mbufUncomp.PutCard1(c1Cur, c4TarInd++);
                }
            }
        }
         else if (eSrcDepth == tCIDImage::EBitDepths::Eight)
        {
            for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4CY; c4YInd++)
            {
                mbufUncomp.PutCard1(CIDPNG_Image::c1Filter_Sub, c4TarInd++);
                c1Prev = 0;
                for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4CX; c4XInd++)
                {
                    c1Cur = tCIDLib::TCard1(pixaSrc.c4At(c4XInd, c4YInd));
                    mbufUncomp.PutCard1(c1Cur - c1Prev, c4TarInd++);
                    c1Prev = c1Cur;
                }
            }
        }
    }
     else if (tCIDLib::bAllBitsOn(eSrcFmt, tCIDImage::EPixFmts::Color))
    {
        //
        //  It's either true color or true color with alpha, or 5 bit
        //  per component color (which doesn't support alpha.) PNG doesn't
        //  support 5 bit type color, so we'll extend it out to make it
        //  24 bit RGB.
        //
        if (eSrcDepth == tCIDImage::EBitDepths::Five)
        {
            // Current pixel color
            tCIDLib::TCard1 c1CurBlue;
            tCIDLib::TCard1 c1CurGreen;
            tCIDLib::TCard1 c1CurRed;

            // From the row above
            tCIDLib::TCard1 c1PriorBlue;
            tCIDLib::TCard1 c1PriorGreen;
            tCIDLib::TCard1 c1PriorRed;

            // From the previous pixel to the left
            tCIDLib::TCard1 c1PrevBlue;
            tCIDLib::TCard1 c1PrevGreen;
            tCIDLib::TCard1 c1PrevRed;

            for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4CY; c4YInd++)
            {
                //
                //  On the first line we'll do sub, and on the subsequent
                //  ones we'll do average.
                //
                tCIDLib::TCard2* pc2Prior;
                if (c4YInd)
                {
                    mbufUncomp.PutCard1(CIDPNG_Image::c1Filter_Avg, c4TarInd++);
                    pc2Prior = (tCIDLib::TCard2*)pixaSrc.pc1RowPtr(c4YInd - 1);
                }
                 else
                {
                    mbufUncomp.PutCard1(CIDPNG_Image::c1Filter_Sub, c4TarInd++);
                    pc2Prior = 0;
                }

                const tCIDLib::TCard2* pc2Cur = (tCIDLib::TCard2*)pixaSrc.pc1RowPtr(c4YInd);

                //
                //  Do an initial pulling out of the color values. After this,
                //  we'll only update them as requireda t the bottom of the
                //  inner loop instead of pulling them all back out each time.
                //
                if (pc2Prior)
                {
                    c1PriorRed = tCIDLib::TCard1((*pc2Prior >> 10) & 0x1F) << 3;
                    c1PriorGreen = tCIDLib::TCard1((*pc2Prior >> 5) & 0x1F) << 3;
                    c1PriorBlue = tCIDLib::TCard1((*pc2Prior & 0x1F)) << 3;
                }
                 else
                {
                    c1PriorRed = 0;
                    c1PriorGreen = 0;
                    c1PriorBlue = 0;
                }

                c1CurRed = tCIDLib::TCard1((*pc2Cur >> 10) & 0x1F) << 3;
                c1CurGreen = tCIDLib::TCard1((*pc2Cur >> 5) & 0x1F) << 3;
                c1CurBlue = tCIDLib::TCard1((*pc2Cur & 0x1F)) << 3;

                c1PrevRed = c1CurRed;
                c1PrevGreen = c1CurGreen;
                c1PrevBlue = c1CurBlue;

                for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4CX; c4XInd++)
                {
                    if (!c4YInd && !c4XInd)
                    {
                        //
                        //  We can't do either filter or sub here since we
                        //  we aren't far enough along.
                        //
                        mbufUncomp.PutCard1(c1CurRed, c4TarInd++);
                        mbufUncomp.PutCard1(c1CurGreen, c4TarInd++);
                        mbufUncomp.PutCard1(c1CurBlue, c4TarInd++);
                    }
                     else if (!c4YInd && c4XInd)
                    {
                        //
                        //  We can do sub filter since we are past the first
                        //  horizontal sample but not average since there's
                        //  no previous line yet.
                        //
                        mbufUncomp.PutCard1(c1CurRed - c1PrevRed, c4TarInd++);
                        mbufUncomp.PutCard1(c1CurGreen - c1PrevGreen, c4TarInd++);
                        mbufUncomp.PutCard1(c1CurBlue - c1PrevBlue, c4TarInd++);
                    }
                     else if (c4YInd && !c4XInd)
                    {
                        //
                        //  Here we have a prior scan line but not a prior
                        //  sample so we can do a simple version of average.
                        //
                        mbufUncomp.PutCard1(c1CurRed - (c1PriorRed >> 1), c4TarInd++);
                        mbufUncomp.PutCard1(c1CurGreen - (c1PriorGreen >> 1), c4TarInd++);
                        mbufUncomp.PutCard1(c1CurBlue - (c1PriorBlue >> 1), c4TarInd++);
                    }
                     else
                    {
                        //
                        //  Here we have both a prior scan line and a previous
                        //  sample, so we do average filtering here.
                        //
                        mbufUncomp.PutCard1
                        (
                            c1CurRed - tCIDLib::TCard1
                            (
                                (tCIDLib::TCard2(c1PrevRed)
                                 + tCIDLib::TCard2(c1PriorRed)) >> 1
                            )
                            , c4TarInd++
                        );

                        mbufUncomp.PutCard1
                        (
                            c1CurGreen - tCIDLib::TCard1
                            (
                                (tCIDLib::TCard2(c1PrevGreen)
                                 + tCIDLib::TCard2(c1PriorGreen)) >> 1
                            )
                            , c4TarInd++
                        );

                        mbufUncomp.PutCard1
                        (
                            c1CurBlue - tCIDLib::TCard1
                            (
                                (tCIDLib::TCard2(c1PrevBlue)
                                 + tCIDLib::TCard2(c1PriorBlue)) >> 1
                            )
                            , c4TarInd++
                        );
                    }

                    //
                    //  Move our pointers forward and get new values out. Save
                    //  the current as the previous before we change them.
                    //
                    c1PrevRed = c1CurRed;
                    c1PrevGreen = c1CurGreen;
                    c1PrevBlue = c1CurBlue;

                    // Now move the current up and get the values out
                    pc2Cur++;
                    c1CurRed = tCIDLib::TCard1((*pc2Cur >> 10) & 0x1F) << 3;
                    c1CurGreen = tCIDLib::TCard1((*pc2Cur >> 5) & 0x1F) << 3;
                    c1CurBlue = tCIDLib::TCard1((*pc2Cur & 0x1F)) << 3;

                    if (pc2Prior)
                    {
                        pc2Prior++;
                        c1PriorRed = tCIDLib::TCard1((*pc2Prior >> 10) & 0x1F) << 3;
                        c1PriorGreen = tCIDLib::TCard1((*pc2Prior >> 5) & 0x1F) << 3;
                        c1PriorBlue = tCIDLib::TCard1((*pc2Prior & 0x1F)) << 3;
                    }
                }
            }
        }
         else if (tCIDLib::bAllBitsOn(eSrcFmt, tCIDImage::EPixFmts::Alpha))
        {
            TPixelArray::TRGBQuad* pqPrior;
            for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4CY; c4YInd++)
            {
                //
                //  On the first line we'll do sub, and on the subsequent
                //  ones we'll do average.
                //
                if (c4YInd)
                {
                    mbufUncomp.PutCard1(CIDPNG_Image::c1Filter_Avg, c4TarInd++);
                    pqPrior = (TPixelArray::TRGBQuad*)pixaSrc.pc1RowPtr(c4YInd - 1);
                }
                 else
                {
                    mbufUncomp.PutCard1(CIDPNG_Image::c1Filter_Sub, c4TarInd++);
                    pqPrior = 0;
                }

                TPixelArray::TRGBQuad* pqCur = (TPixelArray::TRGBQuad*)pixaSrc.pc1RowPtr(c4YInd);
                TPixelArray::TRGBQuad* pqPrev = pqCur;
                for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4CX; c4XInd++)
                {
                    //
                    //  Note that the data in the pixel array is in native
                    //  bit color format, so we cannot just copy over bytes
                    //  directly one for one. We have to do an out of order
                    //  operation on each pixel pass.
                    //
                    if (!c4YInd && !c4XInd)
                    {
                        //
                        //  We can't do either filter or sub here since we
                        //  we aren't far enough along.
                        //
                        mbufUncomp.PutCard1(pqCur->c1Red, c4TarInd++);
                        mbufUncomp.PutCard1(pqCur->c1Green, c4TarInd++);
                        mbufUncomp.PutCard1(pqCur->c1Blue, c4TarInd++);
                        mbufUncomp.PutCard1(pqCur->c1Alpha, c4TarInd++);
                    }
                     else if (!c4YInd && c4XInd)
                    {
                        //
                        //  We can do sub filter since we are past the first
                        //  horizontal sample but not average since there's
                        //  no previous line yet.
                        //
                        mbufUncomp.PutCard1(pqCur->c1Red - pqPrev->c1Red, c4TarInd++);
                        mbufUncomp.PutCard1(pqCur->c1Green - pqPrev->c1Green, c4TarInd++);
                        mbufUncomp.PutCard1(pqCur->c1Blue - pqPrev->c1Blue, c4TarInd++);
                        mbufUncomp.PutCard1(pqCur->c1Alpha - pqPrev->c1Alpha, c4TarInd++);
                    }
                     else if (c4YInd && !c4XInd)
                    {
                        //
                        //  Here we have a prior scan line but not a prior
                        //  sample so we can do a simple version of average.
                        //
                        mbufUncomp.PutCard1(pqCur->c1Red - (pqPrior->c1Red >> 1), c4TarInd++);
                        mbufUncomp.PutCard1(pqCur->c1Green - (pqPrior->c1Green >> 1), c4TarInd++);
                        mbufUncomp.PutCard1(pqCur->c1Blue - (pqPrior->c1Blue >> 1), c4TarInd++);
                        mbufUncomp.PutCard1(pqCur->c1Alpha - (pqPrior->c1Alpha >> 1), c4TarInd++);
                    }
                     else
                    {
                        //
                        //  Here we have both a prior scan line and a previous
                        //  sample, so we do average filtering here.
                        //
                        mbufUncomp.PutCard1
                        (
                            pqCur->c1Red - tCIDLib::TCard1
                            (
                                (tCIDLib::TCard2(pqPrev->c1Red)
                                 + tCIDLib::TCard2(pqPrior->c1Red)) >> 1
                            )
                            , c4TarInd++
                        );

                        mbufUncomp.PutCard1
                        (
                            pqCur->c1Green - tCIDLib::TCard1
                            (
                                (tCIDLib::TCard2(pqPrev->c1Green)
                                 + tCIDLib::TCard2(pqPrior->c1Green)) >> 1
                            )
                            , c4TarInd++
                        );

                        mbufUncomp.PutCard1
                        (
                            pqCur->c1Blue - tCIDLib::TCard1
                            (
                                (tCIDLib::TCard2(pqPrev->c1Blue)
                                 + tCIDLib::TCard2(pqPrior->c1Blue)) >> 1
                            )
                            , c4TarInd++
                        );

                        mbufUncomp.PutCard1
                        (
                            pqCur->c1Alpha - tCIDLib::TCard1
                            (
                                (tCIDLib::TCard2(pqPrev->c1Alpha)
                                 + tCIDLib::TCard2(pqPrior->c1Alpha)) >> 1
                            )
                            , c4TarInd++
                        );
                    }

                    pqCur++;
                    if (c4XInd)
                        pqPrev++;
                    if (pqPrior)
                        pqPrior++;
                }
            }
        }
         else
        {
            TPixelArray::TRGBTriple* ptPrior;
            for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4CY; c4YInd++)
            {
                //
                //  On the first line we'll do sub, and on the subsequent
                //  ones we'll do average.
                //
                if (c4YInd)
                {
                    mbufUncomp.PutCard1(CIDPNG_Image::c1Filter_Avg, c4TarInd++);
                    ptPrior = (TPixelArray::TRGBTriple*)pixaSrc.pc1RowPtr(c4YInd - 1);
                }
                 else
                {
                    mbufUncomp.PutCard1(CIDPNG_Image::c1Filter_Sub, c4TarInd++);
                    ptPrior = 0;
                }

                TPixelArray::TRGBTriple* ptCur = (TPixelArray::TRGBTriple*)pixaSrc.pc1RowPtr(c4YInd);
                TPixelArray::TRGBTriple* ptPrev = ptCur;
                for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4CX; c4XInd++)
                {
                    //
                    //  Note that the data in the pixel array is in native
                    //  bit color format, so we cannot just copy over bytes
                    //  directly one for one. We have to do an out of order
                    //  operation on each pixel pass.
                    //
                    if (!c4YInd && !c4XInd)
                    {
                        //
                        //  We can't do either filter or sub here since we
                        //  we aren't far enough along.
                        //
                        mbufUncomp.PutCard1(ptCur->c1Red, c4TarInd++);
                        mbufUncomp.PutCard1(ptCur->c1Green, c4TarInd++);
                        mbufUncomp.PutCard1(ptCur->c1Blue, c4TarInd++);
                    }
                     else if (!c4YInd && c4XInd)
                    {
                        //
                        //  We can do sub filter since we are past the first
                        //  horizontal sample but not average since there's
                        //  no previous line yet.
                        //
                        mbufUncomp.PutCard1(ptCur->c1Red - ptPrev->c1Red, c4TarInd++);
                        mbufUncomp.PutCard1(ptCur->c1Green - ptPrev->c1Green, c4TarInd++);
                        mbufUncomp.PutCard1(ptCur->c1Blue - ptPrev->c1Blue, c4TarInd++);
                    }
                     else if (c4YInd && !c4XInd)
                    {
                        //
                        //  Here we have a prior scan line but not a prior
                        //  sample so we can do a simple version of average.
                        //
                        mbufUncomp.PutCard1(ptCur->c1Red - (ptPrior->c1Red >> 1), c4TarInd++);
                        mbufUncomp.PutCard1(ptCur->c1Green - (ptPrior->c1Green >> 1), c4TarInd++);
                        mbufUncomp.PutCard1(ptCur->c1Blue - (ptPrior->c1Blue >> 1), c4TarInd++);
                    }
                     else
                    {
                        //
                        //  Here we have both a prior scan line and a previous
                        //  sample, so we do average filtering here.
                        //
                        mbufUncomp.PutCard1
                        (
                            ptCur->c1Red - tCIDLib::TCard1
                            (
                                (tCIDLib::TCard2(ptPrev->c1Red)
                                 + tCIDLib::TCard2(ptPrior->c1Red)) >> 1
                            )
                            , c4TarInd++
                        );

                        mbufUncomp.PutCard1
                        (
                            ptCur->c1Green - tCIDLib::TCard1
                            (
                                (tCIDLib::TCard2(ptPrev->c1Green)
                                 + tCIDLib::TCard2(ptPrior->c1Green)) >> 1
                            )
                            , c4TarInd++
                        );

                        mbufUncomp.PutCard1
                        (
                            ptCur->c1Blue - tCIDLib::TCard1
                            (
                                (tCIDLib::TCard2(ptPrev->c1Blue)
                                 + tCIDLib::TCard2(ptPrior->c1Blue)) >> 1
                            )
                            , c4TarInd++
                        );
                    }

                    ptCur++;
                    if (c4XInd)
                        ptPrev++;
                    if (ptPrior)
                        ptPrior++;
                }
            }
        }
    }
     else
    {
        //
        //  If it's 8 bit with alpha, or 16 bit without alpha, we can do
        //  those together since they both involve two bytes. We just need
        //  to get the data into bit endian order. We did 8 bit without
        //  alpha in the first section since it's the same in terms of data
        //  format as 8 bit palette.
        //
        //  And finally we do 16 bit alpha with alpha.
        //
        if ((eSrcDepth == tCIDImage::EBitDepths::Eight)
        ||  !tCIDLib::bAllBitsOn(eSrcFmt, tCIDImage::EPixFmts::Alpha))
        {
            for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4CY; c4YInd++)
            {
                mbufUncomp.PutCard1(CIDPNG_Image::c1Filter_Sub, c4TarInd++);

                tCIDLib::TCard1 c1V1;
                tCIDLib::TCard1 c1V2;
                tCIDLib::TCard1 c1PV1 = 0;
                tCIDLib::TCard1 c1PV2 = 0;
                tCIDLib::TCard4 c4Cur;
                for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4CX; c4XInd++)
                {
                    c4Cur = pixaSrc.c4At(c4XInd, c4YInd);
                    c1V1 = tCIDLib::TCard1(c4Cur >> 8);
                    c1V2 = tCIDLib::TCard1(c4Cur & 0xFF);
                    mbufUncomp.PutCard1(c1V1 - c1PV1, c4TarInd++);
                    mbufUncomp.PutCard1(c1V2 - c1PV2, c4TarInd++);
                    c1PV1 = c1V1;
                    c1PV2 = c1V2;
                }
            }
        }
         else
        {
            // It's 16 bit with alpha
            for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4CY; c4YInd++)
            {
                mbufUncomp.PutCard1(CIDPNG_Image::c1Filter_Sub, c4TarInd++);

                tCIDLib::TCard1 c1V1;
                tCIDLib::TCard1 c1V2;
                tCIDLib::TCard1 c1V3;
                tCIDLib::TCard1 c1V4;

                tCIDLib::TCard1 c1PV1 = 0;
                tCIDLib::TCard1 c1PV2 = 0;
                tCIDLib::TCard1 c1PV3 = 0;
                tCIDLib::TCard1 c1PV4 = 0;

                tCIDLib::TCard4 c4Cur;
                for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4CX; c4XInd++)
                {
                    c4Cur = pixaSrc.c4At(c4XInd, c4YInd);

                    c1V1 = tCIDLib::TCard1(c4Cur >> 24);
                    c1V2 = tCIDLib::TCard1((c4Cur >> 16) & 0xFF);
                    c1V3 = tCIDLib::TCard1((c4Cur >> 8) & 0xFF);
                    c1V4 = tCIDLib::TCard1(c4Cur & 0xFF);

                    mbufUncomp.PutCard1(c1V1 - c1PV1, c4TarInd++);
                    mbufUncomp.PutCard1(c1V2 - c1PV2, c4TarInd++);
                    mbufUncomp.PutCard1(c1V3 - c1PV3, c4TarInd++);
                    mbufUncomp.PutCard1(c1V4 - c1PV4, c4TarInd++);

                    c1PV1 = c1V1;
                    c1PV2 = c1V2;
                    c1PV3 = c1V3;
                    c1PV4 = c1V4;
                }
            }
        }
    }

    //
    //  Set up the streams and compress the data to the caller's buffer. The
    //  calculated buffer size may be larger than what we actually used so
    //  we use the actual final index we ended up on as the size of the data.
    //
    //  Insure a minimum size in case it was a zero by zero image. That will cause
    //  the size to come out zero, but there's at least some compressed data header
    //  info that has to go out.
    //
    TBinMBufInStream strmUncomp(&mbufUncomp, c4TarInd);
    TChunkedBinOutStream strmComp(tCIDLib::MaxVal(c4TarInd * 2, 64UL));
    TZLibCompressor zlibPNG;
    const tCIDLib::TCard4 c4CompSz = zlibPNG.c4Compress(strmUncomp, strmComp);

    //
    //  Make sure this guy is capable of containing the data. If not, reset it
    //  so that it will.
    //
    if (mbufToFill.c4MaxSize() < c4CompSz)
        mbufToFill.Reset(c4CompSz, tCIDLib::MaxVal(c4CompSz, mbufToFill.c4MaxSize()));

    if (strmComp.c4CopyOutTo(mbufToFill, 0) != c4CompSz)
    {
        CIDAssert2(L"Comp size != streamed size");
    }

    return c4CompSz;
}


//
//  Does a reverse filter transformation of a scan line. The line is updated
//  in place.
//
tCIDLib::TVoid
TPNGImage::DefilterScanLine(        tCIDLib::TCard1*    pc1Ln
                            , const tCIDLib::TCard4     c4LineBytes
                            , const tCIDLib::TCard4     c4Back
                            , const tCIDLib::TCard4     c4LineInd)
{
    const tCIDLib::TCard1* pc1Prev = pc1Ln - c4LineBytes;
    const tCIDLib::TCard1 c1Filter = *pc1Ln++;
    if (!c1Filter)
        return;

    //
    //  We are filtered, so process the bytes in this line, doing
    //  the filter operation on each byte.
    //
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < c4LineBytes; c4Ind++)
    {
        switch(c1Filter)
        {
            case CIDPNG_Image::c1Filter_Sub :
            {
                // We do nothing for the first pixel's worth
                if (c4Ind >= c4Back)
                    *pc1Ln += tCIDLib::TCard1(*(pc1Ln - c4Back));
                break;
            }

            case CIDPNG_Image::c1Filter_Up :
            {
                if (c4LineInd)
                    *pc1Ln += *pc1Prev;
                break;
            }

            case CIDPNG_Image::c1Filter_Avg :
            {
                tCIDLib::TCard2 c2Tmp = 0;
                if (c4Ind >= c4Back)
                    c2Tmp = *(pc1Ln - c4Back);
                if (c4LineInd)
                    c2Tmp += *pc1Prev;
                *pc1Ln += tCIDLib::TCard1(c2Tmp >> 1);
                break;
            }

            case CIDPNG_Image::c1Filter_Paeth :
            {
                tCIDLib::TCard4 c4Prev = 0;
                tCIDLib::TCard4 c4Above = 0;
                tCIDLib::TCard4 c4AbovePrev = 0;
                if (c4Ind >= c4Back)
                    c4Prev = *(pc1Ln - c4Back);
                if (c4LineInd)
                {
                    if (c4Ind >= c4Back)
                        c4AbovePrev = *(pc1Prev - c4Back);
                    c4Above = *pc1Prev;
                }
                *pc1Ln = tCIDLib::TCard1
                (
                    tCIDLib::TCard4
                    (
                        *pc1Ln + c4PaethPredictor(c4Prev, c4Above, c4AbovePrev)
                    )
                );
                break;
            }

            default :
                break;
        };

        // Move both pointers up
        pc1Ln++;
        pc1Prev++;
    }
}


//
//  Run through the samples for a given scan line and load them up into the
//  target pixel array.
//
//  It is assumed that we've set up the pixel array to either match the
//  native format, or in the case of 16 bit color/alpha that it's been set
//  up for 8 bit color/alpha and we'll down convert here. We get the original
//  format and depth, so we know what the source data is.
//
//  This method also handles interlaced format, in which case it has to
//  distribute the incoming pixels (which are sub-images of the full image)
//  differently. If the c4PassNum parameter is c4MaxCard, then it's a
//  full progressive scan line, else it's a pass number from 0 to 6.
//
tCIDLib::TVoid
TPNGImage::LoadScanLine(const   tCIDLib::TCard1* const  pc1Ln
                        , const tCIDImage::EPixFmts     eOrgFmt
                        , const tCIDLib::TCard4         c4Line
                        , const tCIDLib::TCard4         c4Samples
                        , const tCIDLib::TCard4         c4SrcBitsPer
                        , const tCIDLib::TCard4         c4OrgDepth
                        ,       TPixelArray&            pixaToFill
                        , const tCIDLib::TCard4         c4PassNum)
{
    //
    //  Get a pointer to the line info we can run up. Because of byte order
    //  issues (PNG data is big endian) we deal with everything as bytes
    //  as we pull it out. This makes us byte order independent here.
    //
    const tCIDLib::TCard1* pc1Cur = pc1Ln;

    //
    //  Calculate the starting target index and the amount we move forward for
    //  each outgoing sample. If not doing interlaced, it's just 0 and 1.
    //
    tCIDLib::TCard4 c4TarInd = 0;
    tCIDLib::TCard4 c4TarOfs = 1;
    if (c4PassNum != kCIDLib::c4MaxCard)
    {
        c4TarInd = CIDPNG_Image::ac4XOfs[c4PassNum];
        c4TarOfs = 8 / CIDPNG_Image::ac4XCnt[c4PassNum];
    }

    tCIDLib::TCard4 c4Ind;
    tCIDLib::TCard4 c4Shift;
    tCIDLib::TCard4 c4Col = 0;
    if (tCIDLib::bAllBitsOn(eOrgFmt, tCIDImage::EPixFmts::Palette)
    ||  ((eOrgFmt == tCIDImage::EPixFmts::GrayScale) && (c4OrgDepth < 16)))
    {
        //
        //  It is palette based or gray scale of 8 bit or less and without
        //  alpha, so just 1, 2, 4, or 8 bit stuff and we can handle them
        //  all the same.
        //
        while (c4Col < c4Samples)
        {
            tCIDLib::TCard1 c1Cur = *pc1Cur++;
            switch(c4OrgDepth)
            {
                case 1 :
                {
                    for (c4Ind = 0; c4Ind < 8 && c4Col < c4Samples; c4Ind++)
                    {
                        pixaToFill.PutAt
                        (
                            (c1Cur & 0x80) ? 1 : 0
                            , c4TarInd
                            , c4Line
                        );
                        c1Cur <<= 1;
                        c4TarInd += c4TarOfs;
                        c4Col++;
                    }
                    break;
                }

                case 2 :
                {
                    c4Shift = 8;
                    for (c4Ind = 0; c4Ind < 4 && c4Col < c4Samples; c4Ind++)
                    {
                        c4Shift -= 2;
                        pixaToFill.PutAt(c1Cur >> c4Shift, c4TarInd, c4Line);
                        c4TarInd += c4TarOfs;
                        c4Col++;
                    }
                    break;
                }

                case 4 :
                {
                    pixaToFill.PutAt(c1Cur >> 4, c4TarInd, c4Line);
                    c4TarInd += c4TarOfs;
                    if (++c4Col < c4Samples)
                    {
                        pixaToFill.PutAt(c1Cur & 0xF, c4TarInd, c4Line);
                        c4TarInd += c4TarOfs;
                        c4Col++;
                    }
                    break;
                }

                case 8 :
                {
                    pixaToFill.PutAt(c1Cur, c4TarInd, c4Line);
                    c4TarInd += c4TarOfs;
                    c4Col++;
                    break;
                }
            };
        }
    }
     else if (tCIDLib::bAllBitsOn(eOrgFmt, tCIDImage::EPixFmts::Color))
    {
        //
        //  It's 24 or 48 bit color, possibly with alpha. We can't handle
        //  48 bit so it becomes 24 bit, so either way we are doing to
        //  be doing 24 bit color, possibly with alpha.
        //
        if (tCIDLib::bAllBitsOn(eOrgFmt, tCIDImage::EPixFmts::Alpha))
        {
            if (c4OrgDepth == 16)
            {
                //
                //  It's 48 bit color plus alpha, so convert to 24 bit by
                //  taking the high 8 bits of each sample, and do the same
                //  for the alpha, for a 32 bit value.
                //
                tCIDLib::TCard4 c4Cur;
                for (c4Col = 0; c4Col < c4Samples; c4Col++)
                {
                    c4Cur = *pc1Cur;
                    pc1Cur += 2;
                    c4Cur |= tCIDLib::TCard4(*pc1Cur) << 8;
                    pc1Cur += 2;
                    c4Cur |= tCIDLib::TCard4(*pc1Cur) << 16;
                    pc1Cur += 2;
                    c4Cur |= tCIDLib::TCard4(*pc1Cur) << 24;
                    pc1Cur += 2;
                    pixaToFill.PutAt(c4Cur, c4TarInd, c4Line);
                    c4TarInd += c4TarOfs;
                }
            }
             else
            {
                //
                //  It's 8 bit plus Alpha. The order we get is r,g,b,a.
                //  For little endian, we can just cast that to a Card4
                //  and it's good for the storage format we need. For a big
                //  endian, we have to flip it in order to pass in as a Card4.
                //
                const tCIDLib::TCard4* pc4Cur = (const tCIDLib::TCard4*)pc1Ln;
                for (c4Col = 0; c4Col < c4Samples; c4Col++)
                {
                    #ifdef CIDLIB_LITTLEENDIAN
                    pixaToFill.PutAt(*pc4Cur++, c4TarInd, c4Line);
                    #else
                    pixaToFill.PutAt(TRawBits::c4SwapBytes(*pc4Cur++), c4TarInd, c4Line);
                    #endif
                    c4TarInd += c4TarOfs;
                }
            }
        }
         else
        {
            if (c4OrgDepth == 8)
            {
                //
                //  It's 24 bit color, so just grab 3 bytes at a time from
                //  the source.
                //
                tCIDLib::TCard4 c4Cur;
                for (c4Col = 0; c4Col < c4Samples; c4Col++)
                {
                    c4Cur = *pc1Cur++;
                    c4Cur |= tCIDLib::TCard4(*pc1Cur++) << 8;
                    c4Cur |= tCIDLib::TCard4(*pc1Cur++) << 16;
                    pixaToFill.PutAt(c4Cur, c4TarInd, c4Line);
                    c4TarInd += c4TarOfs;
                }
            }
             else
            {
                //
                //  It's 48 bit color, so take the high 8 bits of each
                //  of the 3 color samples to create a 24 bit sample.
                //
                tCIDLib::TCard4 c4Cur;
                for (c4Col = 0; c4Col < c4Samples; c4Col++)
                {
                    c4Cur = *pc1Cur;
                    pc1Cur += 2;
                    c4Cur |= tCIDLib::TCard4(*pc1Cur) << 8;
                    pc1Cur += 2;
                    c4Cur |= tCIDLib::TCard4(*pc1Cur) << 16;
                    pc1Cur += 2;
                    pixaToFill.PutAt(c4Cur, c4TarInd, c4Line);
                    c4TarInd += c4TarOfs;
                }
            }
        }
    }
     else
    {
        //
        //  Any gray scale without alpha was handled above with the palette
        //  based stuff. So all we have left here is 8 bit gray with alpha
        //  or 16 bit gray scale with or without alpha.
        //
        if (c4OrgDepth == 8)
        {
            //
            //  It's 8 bit gray plus alpha, so we have to pull out two
            //  bytes for each sample. The first is the value and the second
            //  is the alpha. We put it into the pixel array with the value
            //  in the high 8 bits and the alpha in the low.
            //
            tCIDLib::TCard4 c4Cur;
            for (c4Col = 0; c4Col < c4Samples; c4Col++)
            {
                c4Cur = *pc1Cur++;
                c4Cur <<= 8;
                c4Cur |= *pc1Cur++;
                pixaToFill.PutAt(c4Cur, c4TarInd, c4Line);
                c4TarInd += c4TarOfs;
            }
        }
         else
        {
            // It's 16 bit maybe iwth alpha, so 2 bytes at least 4 if alpha
            const tCIDLib::TCard1* pc1Cur = pc1Ln;
            tCIDLib::TCard4 c4Cur;
            for (c4Col = 0; c4Col < c4Samples; c4Col++)
            {
                c4Cur = *pc1Cur++;
                c4Cur <<= 8;
                c4Cur |= *pc1Cur++;
                if (tCIDLib::bAllBitsOn(eOrgFmt, tCIDImage::EPixFmts::Alpha))
                {
                    c4Cur <<= 8;
                    c4Cur |= *pc1Cur++;
                    c4Cur <<= 8;
                    c4Cur |= *pc1Cur++;
                }
                pixaToFill.PutAt(c4Cur, c4TarInd, c4Line);
                c4TarInd += c4TarOfs;
            }
        }
    }
}


//
//  When we've decompressed the data chunk, this is called to pull the data
//  out of the raw data format and into the pixel array object that will
//  hold it in our version of an image.
//
//  We have two different loops here. One is for progressive data and the
//  other is for interlaced data. They each call a helper method to defilter
//  scaline lines and then load those scan lines into the pixel array. We
//  can use the same defilter method for both, by adjusting the values we
//  pass to it for each pass of the Adam7 loop. And we use the same method
//  to store by passing in a pass number. c4MaxCard means progressive data.
//
tCIDLib::TVoid
TPNGImage::LoadPixelArray(          TMemBuf&            mbufDecomp
                            , const tCIDImage::EPixFmts eOrgFmt
                            , const tCIDLib::TCard4     c4SrcBitsPer
                            , const tCIDLib::TCard4     c4OrgDepth)
{
    //
    //  Get out the image info stuff for fast and convenient access, and
    //  calculate the bytes we have to look back for the previous byte in
    //  filters that require that.
    //
    const tCIDLib::TCard4 c4CY = c4Height();
    const tCIDLib::TCard4 c4CX = c4Width();

    //
    //  The distance we have to look back for filtering. This is the same
    //  whether interlaced or not.
    //
    tCIDLib::TCard4 c4Back = 1;
    if (c4SrcBitsPer > 8)
    {
        c4Back = c4SrcBitsPer / 8;
        if (c4SrcBitsPer % 8)
            c4Back++;
    }

    //
    //  To optimize things, we do progressive and interlaced schemes
    //  separately, because the progressive one is so much simpler and
    //  more direct, and we can work directly on the buffer itself. For
    //  the interlaced version, we have to maintain previous line buffers
    //  and write data to the pixel array incrementally as we go.
    //
    tCIDLib::TCard4 c4LineBytes;
    TPixelArray& pixaToFill = pixaNCBits();
    tCIDLib::TCard1* pc1Cur = mbufDecomp.pc1Data();
    if (m_eInterlaceType == tCIDPNG::EInterlaces::None)
    {
        // Calc the bytes per src scan line
        const tCIDLib::TCard4 c4LineBits = c4SrcBitsPer * c4CX;
        c4LineBytes = c4LineBits / 8;
        if (c4LineBits % 8)
            c4LineBytes++;

        // Just go through the lines in order
        for (tCIDLib::TCard4 c4LInd = 0; c4LInd < c4CY; c4LInd++)
        {
            DefilterScanLine(pc1Cur, c4LineBytes, c4Back, c4LInd);

            // Write the scan line to the pixel array
            LoadScanLine
            (
                pc1Cur + 1
                , eOrgFmt
                , c4LInd
                , c4CX
                , c4SrcBitsPer
                , c4OrgDepth
                , pixaToFill
            );

            pc1Cur += (c4LineBytes + 1);
        }
    }
     else if (m_eInterlaceType == tCIDPNG::EInterlaces::Adam7)
    {
        //
        //  The outer loop goes through the various passes of the Adam7
        //  format.
        //
        for (tCIDLib::TCard4 c4PassInd = 0; c4PassInd < 7; c4PassInd++)
        {
            //
            //  For each pass, we read a given set of lines and columns,
            //  based on a table. The first thing we need to do is calculate
            //  the number of rows we'll get and the samples on that row,
            //  for each pass. We have to handle any trailing fractional
            //  (less than 8) samples.
            //
            //  So start by seeing how many full 8 sample units we have
            //  and then adjust by fractionals based on the table.
            //
            tCIDLib::TCard4 c4Rows = (c4CY / 8)
                                     * CIDPNG_Image::ac4YCnt[c4PassInd];
            if (c4CY % 8)
                c4Rows += CIDPNG_Image::ac4YLeft[c4PassInd][(c4CY % 8) - 1];

            tCIDLib::TCard4 c4Cols = (c4CX / 8)
                                     * CIDPNG_Image::ac4XCnt[c4PassInd];;
            if ((c4CX % 8))
                c4Cols += CIDPNG_Image::ac4XLeft[c4PassInd][(c4CX % 8) - 1];

            // If no rows or cols for this round, then we skip it
            if (!c4Cols|| !c4Rows)
                continue;

            // We need a separate index for the target row
            const tCIDLib::TCard4 c4OutLOfs = 8 / CIDPNG_Image::ac4YCnt[c4PassInd];
            tCIDLib::TCard4 c4OutLInd = CIDPNG_Image::ac4YOfs[c4PassInd];

            //
            //  It's there so we have to do a pass. So run through all the
            //  rows we got for this pass. And calc the line bytes based on
            //  the samples in this row and the bits per sample.
            //
            const tCIDLib::TCard4 c4Bits = c4Cols * c4SrcBitsPer;
            c4LineBytes = c4Bits / 8;
            if (c4Bits % 8)
                c4LineBytes++;
            for (tCIDLib::TCard4 c4LInd = 0; c4LInd < c4Rows; c4LInd++)
            {
                DefilterScanLine(pc1Cur, c4LineBytes, c4Back, c4LInd);

                // Write the scan line to the pixel array
                LoadScanLine
                (
                    pc1Cur + 1
                    , eOrgFmt
                    , c4OutLInd
                    , c4Cols
                    , c4SrcBitsPer
                    , c4OrgDepth
                    , pixaToFill
                    , c4PassInd
                );
                pc1Cur += (c4LineBytes + 1);
                c4OutLInd += c4OutLOfs;
            }
        }
    }
     else
    {
        //
        //  Don't know what this is and we shouldn't get here since it should
        //  have been dealt with in the parsing of the header info.
        //
        facCIDPNG().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kPNGErrs::errcPNG_BadInterlType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TInteger(tCIDLib::i4EnumOrd(m_eInterlaceType))
        );
    }

}


tCIDLib::TVoid
TPNGImage::ParseBgnClr(         TBinInStream&       strmToReadFrom
                        ,       tCIDLib::TCard4&    c4CRC
                        , const tCIDLib::TCard4
                        , const tCIDImage::EPixFmts eOrgFmt
                        , const tCIDLib::TCard4     c4OrgDepth)
{
    //
    //  The format depends on the color format of the data. It's either
    //  a palette index, a gray scale value, or a color value. Alpha is
    //  not relevant here.
    //
    if (tCIDLib::bAllBitsOn(eOrgFmt, tCIDImage::EPixFmts::Palette))
    {
        // It's a palette index, so get it out of the palette
        const tCIDLib::TCard1 c1Index = c1GetNext(strmToReadFrom, c4CRC);

        // If we don't have a palette, then just ignore it
        if (tCIDLib::bAllBitsOn(eOrgFmt, tCIDImage::EPixFmts::Palette))
            rgbBgnClr(palClrs().rgbAt(c1Index));
    }
     else if (tCIDLib::bAllBitsOn(eOrgFmt, tCIDImage::EPixFmts::Color))
    {
        rgbBgnClr
        (
            TRGBClr
            (
                tCIDLib::TCard1(c2GetNext(strmToReadFrom, c4CRC))
                , tCIDLib::TCard1(c2GetNext(strmToReadFrom, c4CRC))
                , tCIDLib::TCard1(c2GetNext(strmToReadFrom, c4CRC))
            )
        );
    }
     else
    {
        //
        //  It's a gray scale value. If 8 bit or less, then it's an index
        //  into a palette that we created on the fly. Else we just take
        //  the upper 8 bits as a color.
        //
        const tCIDLib::TCard2 c2Val = c2GetNext(strmToReadFrom, c4CRC);
        if (c4OrgDepth < 16)
        {
            rgbBgnClr(palClrs().rgbAt(c2Val));
        }
         else
        {
            const tCIDLib::TCard1 c1Tmp = tCIDLib::TCard1(c2Val >> 8);
            rgbBgnClr(TRGBClr(c1Tmp, c1Tmp, c1Tmp));
        }
    }

    // Check the CRC now
    bCheckCRC(strmToReadFrom, c4CRC, CIDPNG_Image::c4Chunk_BgnClr, m_bStrictMode);
}


tCIDLib::TVoid
TPNGImage::ParseGamma(          TBinInStream&       strmToReadFrom
                        ,       tCIDLib::TCard4&    c4CRC
                        , const tCIDLib::TCard4     )
{
    tCIDLib::TCard4 c4Gamma = c4GetNext(strmToReadFrom, c4CRC);

    //
    //  Set it on our image info object. Note that the reciprocal is stored
    //  in PNG files, so we have to flip the other way to be the actual
    //  exponent.
    //
    //  Watch for dumb programs who put a zero out for gamma, and if it's
    //  some ridiculously large value, clip it back to 4.0.
    //
    if (c4Gamma)
    {
        tCIDLib::TFloat8 f8New
        (
            1.0 / (tCIDLib::TFloat8(c4Gamma) / tCIDLib::TFloat8(100000))
        );
        if (f8New > 4.0)
            f8New = 4.0;
        f8Gamma(f8New);
    }
     else
    {
        f8Gamma(2.2);
    }

    // Check the CRC now
    bCheckCRC(strmToReadFrom, c4CRC, CIDPNG_Image::c4Chunk_Gamma, m_bStrictMode);
}


tCIDLib::TVoid
TPNGImage::ParseTransparency(       TBinInStream&       strmToReadFrom
                            ,       tCIDLib::TCard4&    c4CRC
                            , const tCIDLib::TCard4     c4Len)
{
    const tCIDImage::EPixFmts eFmt = pixaBits().ePixFmt();
    const tCIDImage::EBitDepths eDepth = pixaBits().eBitDepth();

    if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Palette))
    {
        TClrPalette& palTar = palClrs();
        const tCIDLib::TCard4 c4Max = palTar.c4ClrCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Len; c4Index++)
        {
            const tCIDLib::TCard1 c1Cur = c1GetNext(strmToReadFrom, c4CRC);

            //
            //  It's not supposed to have more items than are in the
            //  palette, but if so, just skip them.
            //
            if (c4Index < c4Max)
                palTar.rgbAt(c4Index).c1Alpha(c1Cur);
        }
    }
     else if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Color))
    {
        //
        //  It's 3 2 byte values. Take the low 8 bits if 8 bit or the high
        //  if 16 bit. We don't do the 16 bit color scheme and they image
        //  colors will also have been clipped off like this.
        //
        const tCIDLib::TCard2 c2Red = c2GetNext(strmToReadFrom, c4CRC);
        const tCIDLib::TCard2 c2Green = c2GetNext(strmToReadFrom, c4CRC);
        const tCIDLib::TCard2 c2Blue = c2GetNext(strmToReadFrom, c4CRC);

        TRGBClr rgbTmp;
        if (eDepth == tCIDImage::EBitDepths::Eight)
        {
            rgbTmp.Set
            (
                tCIDLib::TCard1(c2Red)
                , tCIDLib::TCard1(c2Green)
                , tCIDLib::TCard1(c2Blue)
            );
        }
         else
        {
            rgbTmp.Set
            (
                tCIDLib::TCard1(c2Red >> 8)
                , tCIDLib::TCard1(c2Green >> 8)
                , tCIDLib::TCard1(c2Blue >> 8)
            );
        }
        c4TransClr(rgbTmp.c4Color());
    }
     else
    {
        // It's a single 16 bit value. Take the low 8 if 8 bit
        if (eDepth == tCIDImage::EBitDepths::Eight)
            c4TransClr(c2GetNext(strmToReadFrom, c4CRC) >> 8);
        else
            c4TransClr(c2GetNext(strmToReadFrom, c4CRC));
    }

    // Indicate now that we have transparency color set
    bTransClr(kCIDLib::True);

    // Check the CRC now
    bCheckCRC(strmToReadFrom, c4CRC, CIDPNG_Image::c4Chunk_Trans, m_bStrictMode);
}


tCIDLib::TVoid
TPNGImage::PutCard1(const   tCIDLib::TCard1     c1ToWrite
                    ,       TBinOutStream&      strmToWriteTo
                    ,       tCIDLib::TCard4&    c4CRC) const
{
    c4CRC = TRawMem::hshHashBuffer3309(c4CRC, &c1ToWrite, 1);
    strmToWriteTo << c1ToWrite;
}

tCIDLib::TVoid
TPNGImage::PutCard2(const   tCIDLib::TCard2     c2ToWrite
                    ,       TBinOutStream&      strmToWriteTo
                    ,       tCIDLib::TCard4&    c4CRC) const
{
    //
    //  We cannot write it out directly, since the streams write in
    //  canonical byte order. So convert to a buffer. If we are little
    //  endian, we have to swap the bytes first.
    //
    #ifdef CIDLIB_LITTLEENDIAN
    tCIDLib::TCard2 c2Tmp = TRawBits::c2SwapBytes(c2ToWrite);
    #else
    tCIDLib::TCard2 c2Tmp = c2ToWrite;
    #endif
    tCIDLib::TCard1* pc1Buf = (tCIDLib::TCard1*)&c2Tmp;
    c4CRC = TRawMem::hshHashBuffer3309(c4CRC, pc1Buf, 2);
    strmToWriteTo.c4WriteRawBuffer(pc1Buf, 2);
}

tCIDLib::TVoid
TPNGImage::PutCard4(const   tCIDLib::TCard4     c4ToWrite
                    ,       TBinOutStream&      strmToWriteTo
                    ,       tCIDLib::TCard4&    c4CRC) const
{
    //
    //  We cannot write it out directly, since the streams write in
    //  canonical byte order. So convert to a buffer. If we are little
    //  endian, we have to swap the bytes first.
    //
    #ifdef CIDLIB_LITTLEENDIAN
    tCIDLib::TCard4 c4Tmp = TRawBits::c4SwapBytes(c4ToWrite);
    #else
    tCIDLib::TCard4 c4Tmp = c4ToWrite;
    #endif
    const tCIDLib::TCard1* pc1Buf = (const tCIDLib::TCard1*)&c4Tmp;
    c4CRC = TRawMem::hshHashBuffer3309(c4CRC, pc1Buf, 4);
    strmToWriteTo.c4WriteRawBuffer(pc1Buf, 4);
}

//
//  This one is for writing values that we don't want to be part of the CRC,
//  such as the CRC itself and the length field.
//
tCIDLib::TVoid
TPNGImage::PutCard4(const   tCIDLib::TCard4     c4ToWrite
                    ,       TBinOutStream&      strmToWriteTo) const
{
    //
    //  We cannot write it out directly, since the streams write in
    //  canonical byte order. So convert to a buffer. If we are little
    //  endian, we have to swap the bytes first.
    //
    #ifdef CIDLIB_LITTLEENDIAN
    tCIDLib::TCard4 c4Tmp = TRawBits::c4SwapBytes(c4ToWrite);
    #else
    tCIDLib::TCard4 c4Tmp = c4ToWrite;
    #endif
    const tCIDLib::TCard1* pc1Buf = (const tCIDLib::TCard1*)&c4Tmp;
    strmToWriteTo.c4WriteRawBuffer(pc1Buf, 4);
}


tCIDLib::TVoid
TPNGImage::WriteTransClr(TBinOutStream&     strmToWriteTo
                        , tCIDLib::TCard4&  c4CRC) const
{
    //
    //  If it's a palette based image, we have to do the palette based
    //  version of the transparency color. Else we do the regular thing.
    //
    const tCIDImage::EPixFmts eFmt = pixaBits().ePixFmt();
    const TRGBClr rgbTrans = rgbTransClr();
    if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Palette))
    {
        //
        //  We have to put out zeros up until we hit an entry in the
        //  palette that matches the transparency color.
        //
        const TClrPalette& palSrc = palClrs();
        tCIDLib::TCard4 c4TarInd = 0;
        tCIDLib::TCard4 c4Count = palSrc.c4ClrCount();
        while (c4TarInd < c4Count)
        {
            if (palSrc[c4TarInd] == rgbTrans)
                break;
            c4TarInd++;
        }

        // If we found the color, then do it
        if (c4TarInd < c4Count)
        {
            PutCard4(c4TarInd + 1, strmToWriteTo);
            PutCard4(CIDPNG_Image::c4Chunk_Trans, strmToWriteTo, c4CRC);
            c4CRC = kCIDLib::c4MaxCard;
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TarInd; c4Index++)
                PutCard1(0xFF, strmToWriteTo, c4CRC);
            PutCard1(0, strmToWriteTo, c4CRC);
        }
        PutCard4(c4CRC ^ kCIDLib::c4MaxCard, strmToWriteTo);
    }
     else
    {
        //
        //  It's not palette based. As long as it doesn't have an alpha
        //  channel, we can write out a transparency color for it. This
        //  chunk isn't legal for alpha based images.
        //
        if (!tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Alpha))
        {
            PutCard4(6, strmToWriteTo);
            c4CRC = kCIDLib::c4MaxCard;
            PutCard4(CIDPNG_Image::c4Chunk_Trans, strmToWriteTo, c4CRC);
            PutCard2(rgbTrans.c1Red(), strmToWriteTo, c4CRC);
            PutCard2(rgbTrans.c1Green(), strmToWriteTo, c4CRC);
            PutCard2(rgbTrans.c1Blue(), strmToWriteTo, c4CRC);
            PutCard4(c4CRC ^ kCIDLib::c4MaxCard, strmToWriteTo);
        }
    }

    //
    //  Write our private transparent color chunk. We don't support the
    //  palette type transparency, we use a single palette value if it
    //  is palette based.
    //
    PutCard4(4, strmToWriteTo);
    c4CRC = kCIDLib::c4MaxCard;
    PutCard4(CIDPNG_Image::c4Chunk_TransClr, strmToWriteTo, c4CRC);
    PutCard4(c4TransClr(), strmToWriteTo, c4CRC);
    PutCard4(c4CRC ^ kCIDLib::c4MaxCard, strmToWriteTo);
}


