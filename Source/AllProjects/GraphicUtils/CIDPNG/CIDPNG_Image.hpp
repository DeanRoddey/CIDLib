//
// FILE NAME: CIDPNG_Image.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/18/2004
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDPNG_Image.cpp file. This file implements
//  the TPNGImage class, which handles streaming in and out PNG files. This
//  class is derived from TCIDImage so it can be dealt with polymorphically
//  via that standard image interface.
//
//  We also provide a specialized static method that will extract some
//  important info from the file without parsing the image data. This info
//  is often useful to know up front, or for display, and having to parse
//  out and decompress a huge image just to get that info would be really
//  undesirable.
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
//  CLASS: TPNGImage
// PREFIX: img
// ---------------------------------------------------------------------------
class CIDPNGEXP TPNGImage : public TCIDImage
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid QueryImgInfo
        (
                    TBinInStream&           strmSrc
            , const tCIDLib::TCard4         c4DataSz
            ,       TSize&                  szImage
            ,       tCIDImage::EPixFmts&    eImgFmt
            ,       tCIDImage::EBitDepths&  eBitDepth
            ,       tCIDPNG::EInterlaces&   eInterlace
            ,       tCIDLib::TBoolean&      bTrans
            ,       TRGBClr&                rgbTransClr
            , const tCIDLib::TBoolean       bStrict
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPNGImage();

        TPNGImage
        (
            const   TPixelArray&            pixaToSet
        );

        TPNGImage
        (
            const   TPixelArray&            pixaToSet
            , const TClrPalette&            palToUse
            , const tCIDLib::TCard4         c4ClrsImportant = 0
            , const tCIDLib::TCard4         c4ClrsUsed = 0
        );

        TPNGImage
        (
            const   TPNGImage&              imgToCopy
        );

        TPNGImage
        (
            const   TCIDImage&              imgToCopy
        );

        TPNGImage
        (
            const   TCIDImage&              imgToCopy
            , const TSize&                  szNew
        );

        ~TPNGImage();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPNGImage& operator=
        (
            const   TPNGImage&              imgToAssign
        );

        TPNGImage& operator=
        (
            const   TCIDImage&              imgToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bStrictMode() const;

        tCIDLib::TBoolean bStrictMode
        (
            const   tCIDLib::TBoolean       bToSet
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        );

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private, statiic methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bCheckCRC
        (
                    TBinInStream&           strmToReadFrom
            , const tCIDLib::TCard4         c4CRCToCheck
            , const tCIDLib::TCard4         c4ChunkId
            , const tCIDLib::TBoolean       bStrict
        );

        static tCIDLib::TCard1 c1GetNext
        (
                    TBinInStream&           strmToReadFrom
            ,       tCIDLib::TCard4&        c4CRC
        );

        static tCIDLib::TCard2 c2GetNext
        (
                    TBinInStream&           strmToReadFrom
            ,       tCIDLib::TCard4&        c4CRC
        );

        static tCIDLib::TCard4 c4GetNext
        (
                    TBinInStream&           strmToReadFrom
            ,       tCIDLib::TCard4&        c4CRC
        );

        static tCIDPNG::EChunks eParseChunkHdr
        (
                    TBinInStream&           strmToReadFrom
            ,       tCIDLib::TCard4&        c4CRC
            ,       tCIDLib::TCard4&        c4DataLen
            ,       tCIDLib::TCard4&        c4ChunkId
        );

        static tCIDImage::EPixFmts eParseHeader
        (
                    TBinInStream&           strmToReadFrom
            ,       tCIDLib::TCard4&        c4SrcBitsPer
            ,       tCIDImage::EBitDepths&  eDepth
            ,       tCIDImage::EPixFmts&    ePixFmt
            ,       TSize&                  szImg
            ,       tCIDPNG::ECompTypes&    eCompType
            ,       tCIDPNG::EInterlaces&   eInterlaceType
            ,       tCIDLib::TCard4&        c4ClrsImportant
            ,       tCIDLib::TCard4&        c4ClrsUsed
            ,       tCIDLib::TCard4&        c4OrgDepth
            , const tCIDLib::TBoolean       bStrict
        );

        static tCIDLib::TVoid ParsePalette
        (
                    TBinInStream&           strmToReadFrom
            ,       tCIDLib::TCard4&        c4CRC
            , const tCIDLib::TCard4         c4Len
            ,       TClrPalette&            palClrs
            , const tCIDLib::TBoolean       bStrict
        );

        static tCIDLib::TVoid ParsePrivTrans
        (
                    TBinInStream&           strmToReadFrom
            ,       tCIDLib::TCard4&        c4CRC
            , const tCIDLib::TCard4         c4Len
            ,       tCIDLib::TCard4&        c4Color
            , const tCIDLib::TBoolean       bStrict
        );

        static tCIDLib::TVoid SkipChunk
        (
                    TBinInStream&           strmToReadFrom
            ,       tCIDLib::TCard4&        c4CRC
            , const tCIDLib::TCard4         c4ToSkip
            , const tCIDLib::TCard4         c4ChunkId
            , const tCIDLib::TBoolean       bStrict
        );


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bParseData
        (
                    TBinInStream&           strmToReadFrom
            ,       tCIDLib::TCard4&        c4CRC
            , const tCIDLib::TCard4         c4Len
            , const tCIDImage::EPixFmts     eOrgFmt
            , const tCIDLib::TCard4         c4SrcBitsPer
            , const tCIDLib::TCard4         c4OrgDepth
        );

        tCIDLib::TBoolean bProcessChunk
        (
                    TBinInStream&           strmToReadFrom
            , const tCIDPNG::EChunks        eChunk
            ,       tCIDLib::TCard4&        c4CRC
            , const tCIDLib::TCard4         c4Len
            , const tCIDLib::TCard4         c4ChunkId
            , const tCIDImage::EPixFmts     eOrgFmt
            , const tCIDLib::TCard4         c4SrcBitsPer
            , const tCIDLib::TCard4         c4OrgDepth
        );

        tCIDLib::TCard4 c4StoreCompBuf
        (
                    THeapBuf&               mbufToFill
            , const tCIDImage::EPixFmts     eSrcFmt
            , const tCIDImage::EBitDepths   eSrcDepth
            , const tCIDImage::EPixFmts     eTarFmt
            , const tCIDImage::EBitDepths   eTarDepth
        )   const;

        tCIDLib::TVoid DefilterScanLine
        (
                    tCIDLib::TCard1*        pc1Ln
            , const tCIDLib::TCard4         c4LineBytes
            , const tCIDLib::TCard4         c4Back
            , const tCIDLib::TCard4         c4LineInd
        );

        tCIDLib::TVoid LoadScanLine
        (
            const   tCIDLib::TCard1* const  pc1Ln
            , const tCIDImage::EPixFmts     eFmt
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TCard4         c4Samples
            , const tCIDLib::TCard4         c4SrcBitsPer
            , const tCIDLib::TCard4         c4OrgDepth
            ,       TPixelArray&            pixaToFill
            , const tCIDLib::TCard4         c4PassNum = kCIDLib::c4MaxCard
        );

        tCIDLib::TVoid LoadPixelArray
        (
                    TMemBuf&                mbufDecomp
            , const tCIDImage::EPixFmts     eOrgFmt
            , const tCIDLib::TCard4         c4SrcBitsPer
            , const tCIDLib::TCard4         c4OrgDepth
        );

        tCIDLib::TVoid ParseBgnClr
        (
                    TBinInStream&           strmToReadFrom
            ,       tCIDLib::TCard4&        c4CRC
            , const tCIDLib::TCard4         c4Len
            , const tCIDImage::EPixFmts     eOrgFmt
            , const tCIDLib::TCard4         c4OrgDepth
        );

        tCIDLib::TVoid ParseGamma
        (
                    TBinInStream&           strmToReadFrom
            ,       tCIDLib::TCard4&        c4CRC
            , const tCIDLib::TCard4         c4Len
        );

        tCIDLib::TVoid ParseTransparency
        (
                    TBinInStream&           strmToReadFrom
            ,       tCIDLib::TCard4&        c4CRC
            , const tCIDLib::TCard4         c4Len
        );

        tCIDLib::TVoid PutCard1
        (
            const   tCIDLib::TCard1         c1ToWrite
            ,       TBinOutStream&          strmToWriteTo
            ,       tCIDLib::TCard4&        c4CRC
        )   const;

        tCIDLib::TVoid PutCard2
        (
            const   tCIDLib::TCard2         c2ToWrite
            ,       TBinOutStream&          strmToWriteTo
            ,       tCIDLib::TCard4&        c4CRC
        )   const;

        tCIDLib::TVoid PutCard4
        (
            const   tCIDLib::TCard4         c4ToWrite
            ,       TBinOutStream&          strmToWriteTo
            ,       tCIDLib::TCard4&        c4CRC
        )   const;

        tCIDLib::TVoid PutCard4
        (
            const   tCIDLib::TCard4         c4ToWrite
            ,       TBinOutStream&          strmToWriteTo
        )   const;

        tCIDLib::TVoid WriteTransClr
        (
                    TBinOutStream&          strmToWriteTo
            ,       tCIDLib::TCard4&        c4CRC
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bStrictMode
        //      Normally we try to be forgiving of CRC errors when reading in
        //      files. But they can put us into strict mode in which case we
        //      will throw if any CRC fails.
        //
        //  m_eCompType
        //      The type of compression used. Currently only Deflate is
        //      supported.
        //
        //  m_eInterlaceType
        //      The type of interlacing used by this image.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bStrictMode;
        tCIDPNG::ECompTypes     m_eCompType;
        tCIDPNG::EInterlaces    m_eInterlaceType;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TPNGImage,TCIDImage)
        DefPolyDup(TPNGImage)
};

#pragma CIDLIB_POPPACK


