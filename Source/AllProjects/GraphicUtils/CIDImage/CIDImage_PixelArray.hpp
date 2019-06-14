//
// FILE NAME: CIDImage_PixelArray.hpp
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
//  This is the header for the CIDImage_PixelArray.cpp file. This file
//  implements the TPixelArray class, which manages a 2 dimensional array of
//  pixels (i.e. a raster image.) It handles all of the standard bit depths
//  of raster images, namely 1, 2, 4, 8, 16, 24, and 32 bit. This is the way
//  that TCIDImage derived classes give out and accept pixel data.
//
//  Internally, this class will manage the pixel data such that it is in
//  the correct format to write out to a Win32 or OS/2 bitmap, i.e. 4 byte
//  rounded rows, top to bottom format.
//
// CAVEATS/GOTCHAS:
//
//  1)  The default constructor does a simple 1x1, 1 bit scheme.
//
//  2)  For 24 and 32 bit pixels, the RGB values are stored in the Win32
//      and OS/2 bitmap format. This is not correct for passing to a TRGBClr
//      object!!
//
//      24 bit values are: blue,green,red (low to high byte)
//      32 bit values are: blue,green,red,alpha (low to high byte)
//
//      There are structures nested in TPixelArray to decode these.
//
//  3)  Though this guys manages a possibly large buffer, supporting a move ctor
//      here just isn't worth it. The buffer may not belong to us, so we don't want
//      to move it between objects, and there's no really any likely scenarios
//      where a temporary of this type would be created with a large buffer, and
//      we wouldn't get a move unless it was a temp source.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TPixelArray
// PREFIX: pixa
// ---------------------------------------------------------------------------
class CIDIMGEXP TPixelArray :

    public TObject, public MDuplicable, public MStreamable
{
    public :
        // -------------------------------------------------------------------
        //  Public nested types. These are used internally and are in the
        //  format required to pass in to create a bitmap so that, where
        //  possible the image data can be directly passed in.
        // -------------------------------------------------------------------
        #pragma CIDLIB_PACK(1)
        struct TRGBQuad
        {
            tCIDLib::TCard1     c1Blue;
            tCIDLib::TCard1     c1Green;
            tCIDLib::TCard1     c1Red;
            tCIDLib::TCard1     c1Alpha;
        };

        struct TRGBTriple
        {
            tCIDLib::TCard1     c1Blue;
            tCIDLib::TCard1     c1Green;
            tCIDLib::TCard1     c1Red;
        };
        #pragma CIDLIB_POPPACK


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TCard4 c4CalcBitsPerPixel
        (
            const   tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
        );

        static tCIDLib::TCard4 c4CalcBytesForImage
        (
            const   tCIDLib::TCard4         c4Width
            , const tCIDLib::TCard4         c4Height
            , const tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
            , const tCIDLib::TCard4         c4LineBytePadding = 4
        );

        static tCIDLib::TCard4 c4CalcLineWidth
        (
            const   tCIDLib::TCard4         c4Width
            , const tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
            , const tCIDLib::TCard4         c4LineBytePadding = 4
        );

        static tCIDLib::TCard4 c4ValidateFmt
        (
            const   tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPixelArray();

        TPixelArray
        (
            const   tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
            , const tCIDImage::ERowOrders   eRowOrder
            , const TSize&                  szImage
        );

        TPixelArray
        (
            const   tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
            , const tCIDImage::ERowOrders   eRowOrder
            , const TSize&                  szImage
            ,       tCIDLib::TCard1* const  pc1Buf
            , const tCIDLib::TCard4         c4BufSz
            , const tCIDLib::EAdoptOpts     eAdoptOpt
        );

        TPixelArray
        (
            const   TCIDImage&              imgSource
        );

        TPixelArray
        (
            const   TPixelArray&            pixaSrc
        );

        ~TPixelArray();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPixelArray& operator=
        (
            const   TPixelArray&            pixaSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ApplyAlphaChannel
        (
            const   TPixelArray&            pixaAlpha
            , const TArea&                  areaSrc
            , const TPoint&                 pntTar
            , const tCIDLib::TBoolean       bPremultiply
            , const tCIDLib::TBoolean       bForceBW
        );

        tCIDLib::TBoolean bIsSameFormat
        (
            const   TPixelArray&            pixaToCheck
            , const tCIDLib::TBoolean       bIncludeSz
        )   const;

        tCIDLib::TBoolean bPalToRGB
        (
            const   TClrPalette&            palToUse
            , const tCIDLib::TBoolean       bTransClr
            , const tCIDLib::TCard4         c4TransClr
            , const tCIDLib::TBoolean       bRepTrans
            , const TRGBClr&                rgbRepClr
        );

        tCIDLib::TBoolean bIsRGBBased() const;

        tCIDLib::TBoolean bRGBToRGBA
        (
            const   tCIDLib::TCard4         c4TransClr
            , const tCIDLib::TBoolean       bRepTrans
            , const TRGBClr&                rgbRepClr
        );

        tCIDLib::TVoid BSplineScaleTo
        (
                    TPixelArray&            pixaToFill
            , const TClrPalette&            palToUse
            , const tCIDLib::TCard4         c4SplineDegree
        )   const;

        tCIDLib::TCard4 c4At
        (
            const   tCIDLib::TCard4         c4XPos
            , const tCIDLib::TCard4         c4YPos
        )   const;

        tCIDLib::TCard4 c4At
        (
            const   TQ1Point&               pntPos
        );

        tCIDLib::TCard4 c4BitsPer() const;

        tCIDLib::TCard4 c4Height() const;

        tCIDLib::TCard4 c4ImageSz() const;

        tCIDLib::TCard4 c4LineWidth() const;

        tCIDLib::TCard4 c4Width() const;

        tCIDLib::TCard4 c4Unused
        (
            const   TClrPalette&            palToUse
        )   const;

        tCIDLib::TVoid CreateAlphaChannelFrom
        (
            const   TPixelArray&            pixaAlpha
            , const TArea&                  areaSrc
            , const TPoint&                 pntTar
            , const tCIDLib::TBoolean       bPreMultiply
        );

        tCIDLib::TVoid CvtToGrayScale
        (
                    TClrPalette&            palToUse
        );

        tCIDImage::EBitDepths eBitDepth() const;

        tCIDImage::EPixFmts ePixFmt() const;

        tCIDImage::ERowOrders eRowOrder() const;

        tCIDLib::TVoid ExtractAlphaChannel
        (
                    TPixelArray&            pixaAlpha
        )   const;

        tCIDLib::TVoid FlipHorizontally();

        tCIDLib::TVoid FlipHorizontally
        (
            const   tCIDLib::TCard4         c4StartCol
            , const tCIDLib::TCard4         c4EndCol
        );

        tCIDLib::TVoid FlipRowOrder();

        tCIDLib::TVoid ForceRowOrder
        (
            const   tCIDImage::ERowOrders   eRowOrder
        );

        tCIDLib::TVoid FlipVertically();

        tCIDLib::TVoid FlipVertically
        (
            const   tCIDLib::TCard4         c4StartRow
            , const tCIDLib::TCard4         c4EndRow
        );

        tCIDLib::TVoid GaussianBlur
        (
            const   tCIDLib::TCard4         c4Order
            ,       TPixelArray* const      ppixaTmp = 0
        );

        tCIDLib::TVoid GaussianBlur
        (
            const   tCIDLib::TCard4         c4Order
            , const tCIDLib::TCard4         c4MaxWidth
            , const tCIDLib::TCard4         c4MaxHeight
            ,       TPixelArray* const      ppixaTmp = 0
        );

        tCIDLib::TVoid InvertAlphaChannel();

        tCIDLib::TVoid LoadCompRow
        (
                    tCIDLib::TFloat8* const pf8ToLoad
            , const tCIDLib::TCard4         c4Row
            , const tCIDLib::EClrComps      eComp
            , const TClrPalette&            palToUse
        )   const;

        tCIDLib::TVoid MakeMask
        (
                    TPixelArray&            pixaToFill
            , const tCIDLib::TCard4         c4ClrVal
            , const tCIDLib::TBoolean       bInvert = kCIDLib::False
        )   const;

        tCIDLib::TVoid MakeScaled
        (
                    TPixelArray&            pixaThumb
            , const TClrPalette&            palClrs
            , const tCIDLib::TBoolean       bTransClr
            , const tCIDLib::TCard4         c4TransClr
            , const tCIDLib::TCard4         c4Degree
        )   const;

        const tCIDLib::TCard1* pc1Buffer() const;

        tCIDLib::TCard1* pc1Buffer();

        const tCIDLib::TCard1* pc1RowPtr
        (
            const   tCIDLib::TCard4         c4Row
        )   const;

        tCIDLib::TCard1* pc1RowPtr
        (
            const   tCIDLib::TCard4         c4Row
        );

        tCIDLib::TVoid Premultiply();

        tCIDLib::TVoid PutAt
        (
            const   tCIDLib::TCard4         c4ToPut
            , const tCIDLib::TCard4         c4XPos
            , const tCIDLib::TCard4         c4YPos
        );

        tCIDLib::TVoid PutAt
        (
            const   tCIDLib::TCard4         c4ToPut
            , const TQ1Point&               pntPos
        );

        tCIDLib::TVoid QueryRGBAt
        (
            const   TPoint&                 pntAt
            , const TClrPalette&            palToUse
            ,       TRGBClr&                rgbToFill
        )   const;

        tCIDLib::TVoid ReplaceClr
        (
            const   tCIDLib::TCard4         c4ToRep
            , const tCIDLib::TCard4         c4RepClr
        );

        tCIDLib::TVoid Reset
        (
            const   tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
            , const tCIDImage::ERowOrders   eRowOrder
            , const TSize&                  szImage
            , const tCIDLib::TBoolean       bForceRealloc = kCIDLib::False
        );

        tCIDLib::TVoid Reset
        (
            const   tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
            , const tCIDLib::TBoolean       bForceRealloc = kCIDLib::False
        );

        tCIDLib::TVoid Reset
        (
            const   TSize&                  szImage
            , const tCIDLib::TBoolean       bForceRealloc = kCIDLib::False
        );

        tCIDLib::TVoid Reset
        (
            const   TCIDImage&              imgSource
            , const tCIDLib::TBoolean       bForceRealloc = kCIDLib::False
        );

        TSize szImage() const;

        tCIDLib::TVoid ScaleAlpha
        (
                    tCIDLib::EDirs          eDir
            , const tCIDLib::TCard4         c4StartInd
            , const tCIDLib::TCard4         c4EndInd
            , const tCIDLib::TBoolean       bPremultiply
        );

        tCIDLib::TVoid SetAll
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid SetAllAlpha
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid SetAlphaAt
        (
            const   tCIDLib::TCard4         c4ToSet
            , const TArea&                  areaTar
        );

        tCIDLib::TVoid SetAlphaFromColor();

        tCIDLib::TVoid SetAlphaFromColor
        (
            const   tCIDLib::TCard4         c4TransClr
        );

        tCIDLib::TVoid SetEmptyTrans();

        tCIDLib::TVoid SetTextAlpha
        (
            const   tCIDLib::TCard4         c4TextClr
            , const tCIDLib::TCard4         c4TransClr
            , const tCIDLib::TBoolean       bPremul
        );

        tCIDLib::TVoid StoreCompRow
        (
                    tCIDLib::TCard2* const  pc2ToStore
            , const tCIDLib::TCard4         c4Row
            , const tCIDLib::EClrComps      eComp
        );

        tCIDLib::TVoid ZeroAll();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4GetBoundaryPixel
        (
            const   tCIDLib::TInt4          i4XPos
            , const tCIDLib::TInt4          i4YPos
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TCard4 c4GetPixel
        (
            const   tCIDLib::TCard4         c4XPos
            , const tCIDLib::TCard4         c4YPos
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        TPixelArrayImpl* ppixaiMakeNew();

        TPixelArrayImpl* ppixaiMakeNew
        (
            const   tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
            , const tCIDImage::ERowOrders   eRowOrder
            , const TSize&                  szImage
        );

        TPixelArrayImpl* ppixaiMakeNew
        (
            const   tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
            , const tCIDImage::ERowOrders   eRowOrder
            , const TSize&                  szImage
            ,       tCIDLib::TCard1* const  pc1Buf
            , const tCIDLib::TCard4         c4BufSz
            , const tCIDLib::EAdoptOpts     eAdoptOpt
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pixaiInt
        //      Our internal pixel array implementation object.
        // -------------------------------------------------------------------
        TPixelArrayImpl*    m_pixaiInt;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        BefriendFactory(TPixelArray)
        RTTIDefs(TPixelArray,TObject)
        DefPolyDup(TPixelArray)
};

#pragma CIDLIB_POPPACK
