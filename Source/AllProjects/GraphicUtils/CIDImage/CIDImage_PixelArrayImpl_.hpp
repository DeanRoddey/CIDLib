//
// FILE NAME: CIDImage_PixelArrayImpl_.hpp
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
//  This is the header for the internal base implementation class for pixel arrays.
//  This
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TPixelArrayImpl
// PREFIX: pixai
// ---------------------------------------------------------------------------
class TPixelArrayImpl : public TObject, public MStreamable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPixelArrayImpl();

        TPixelArrayImpl
        (
            const   tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
            , const tCIDImage::ERowOrders   eRowOrder
            , const TSize&                  szImage
        );

        TPixelArrayImpl
        (
            const   tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
            , const tCIDImage::ERowOrders   eRowOrder
            , const TSize&                  szImage
            ,       tCIDLib::TCard1* const  pc1Buf
            , const tCIDLib::TCard4         c4BufSz
            , const tCIDLib::EAdoptOpts     eAdoptOpt
        );

        TPixelArrayImpl
        (
            const   TCIDImage&              imgSrc
        );

        TPixelArrayImpl
        (
            const   TPixelArrayImpl&        pixaiSrc
        );

        ~TPixelArrayImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPixelArrayImpl& operator=(const TPixelArrayImpl&) = delete;

        TPixelArrayImpl& operator=(TPixelArrayImpl&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid ApplyAlphaChannel
        (
            const   TPixelArrayImpl&        pixaiAlpha
            , const TArea&                  areaSrc
            , const TArea&                  areaUpdate
            , const TPoint&                 pntTar
            , const tCIDLib::TBoolean       bPremultiply
            , const tCIDLib::TBoolean       bForceBW
        );

        virtual tCIDLib::TBoolean bIsSameFormat
        (
            const   TPixelArrayImpl&        pixaiToCheck
            , const tCIDLib::TBoolean       bIncludeSz
        )   const;

        virtual tCIDLib::TBoolean bPalToRGB
        (
            const   TClrPalette&            palToUse
            , const tCIDLib::TBoolean       bTransClr
            , const tCIDLib::TCard4         c4TransClr
            , const tCIDLib::TBoolean       bRepTrans
            , const TRGBClr&                rgbRepClr
        );

        virtual tCIDLib::TBoolean bRGBToRGBA
        (
            const   tCIDLib::TCard4         c4TransClr
            , const tCIDLib::TBoolean       bRepTrans
            , const TRGBClr&                rgbRepClr
        );

        virtual tCIDLib::TVoid BSplineScaleTo
        (
                    TPixelArrayImpl&        pixaiToFill
            , const TClrPalette&            palToUse
            , const tCIDLib::TCard4         c4SplineDegree
        )   const;

        virtual tCIDLib::TCard4 c4At
        (
            const   tCIDLib::TCard4         c4XPos
            , const tCIDLib::TCard4         c4YPos
        )   const;

        virtual tCIDLib::TCard4 c4At
        (
            const   TQ1Point&               pntPos
        );

        virtual tCIDLib::TCard4 c4Unused
        (
            const   TClrPalette&            palToUse
        )   const;

        virtual tCIDLib::TVoid CvtToGrayScale
        (
                    TClrPalette&            palToUse
        );

        virtual tCIDLib::TVoid ExtractAlphaChannel
        (
                    TPixelArrayImpl&        pixaiAlpha
        )   const;

        virtual tCIDLib::TVoid FlipHorizontally();

        virtual tCIDLib::TVoid FlipHorizontally
        (
            const   tCIDLib::TCard4         c4StartCol
            , const tCIDLib::TCard4         c4EndCol
        );

        virtual tCIDLib::TVoid FlipRowOrder();

        virtual tCIDLib::TVoid ForceRowOrder
        (
            const   tCIDImage::ERowOrders   eRowOrder
        );

        virtual tCIDLib::TVoid FlipVertically();

        virtual tCIDLib::TVoid FlipVertically
        (
            const   tCIDLib::TCard4         c4StartRow
            , const tCIDLib::TCard4         c4EndRow
        );

        virtual tCIDLib::TVoid GaussianBlur
        (
            const   tCIDLib::TCard4         c4Order
            ,       TPixelArrayImpl* const  ppixaiTmp = nullptr
        );

        virtual tCIDLib::TVoid GaussianBlur
        (
            const   tCIDLib::TCard4         c4Order
            , const tCIDLib::TCard4         c4MaxWidth
            , const tCIDLib::TCard4         c4MaxHeight
            ,       TPixelArrayImpl* const  ppixaiTmp = nullptr
        );

        virtual tCIDLib::TVoid InvertAlphaChannel();

        virtual tCIDLib::TVoid LoadCompRow
        (
                    tCIDLib::TFloat8* const pf8ToLoad
            , const tCIDLib::TCard4         c4Row
            , const tCIDLib::EClrComps      eComp
            , const TClrPalette&            palToUse
        )   const;

        virtual tCIDLib::TVoid MakeMask
        (
                    TPixelArrayImpl&        pixaiToFill
            , const tCIDLib::TCard4         c4ClrVal
            , const tCIDLib::TBoolean       bInvert = kCIDLib::False
        )   const;

        virtual tCIDLib::TVoid MakeScaled
        (
                    TPixelArrayImpl&        pixaiThumb
            , const TClrPalette&            palClrs
            , const tCIDLib::TBoolean       bTransClr
            , const tCIDLib::TCard4         c4TransClr
            , const tCIDLib::TCard4         c4Degree
        )   const;

        virtual const tCIDLib::TCard1* pc1RowPtr
        (
            const   tCIDLib::TCard4         c4Row
        )   const;

        virtual tCIDLib::TCard1* pc1RowPtr
        (
            const   tCIDLib::TCard4         c4Row
        );

        virtual tCIDLib::TVoid Premultiply();

        virtual tCIDLib::TVoid PutAt
        (
            const   tCIDLib::TCard4         c4ToPut
            , const tCIDLib::TCard4         c4XPos
            , const tCIDLib::TCard4         c4YPos
        );

        virtual tCIDLib::TVoid PutAt
        (
            const   tCIDLib::TCard4         c4ToPut
            , const TQ1Point&               pntPos
        );

        virtual tCIDLib::TVoid QueryRGBAt
        (
            const   TPoint&                 pntAt
            , const TClrPalette&            palToUse
            ,       TRGBClr&                rgbToFill
        )   const;

        virtual tCIDLib::TVoid ReplaceClr
        (
            const   tCIDLib::TCard4         c4ToRep
            , const tCIDLib::TCard4         c4RepClr
        );

        virtual tCIDLib::TVoid Reset
        (
            const   tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
            , const tCIDImage::ERowOrders   eRowOrder
            , const TSize&                  szImage
            , const tCIDLib::TBoolean       bForceRealloc = kCIDLib::False
        );

        virtual tCIDLib::TVoid Reset
        (
            const   tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
            , const tCIDLib::TBoolean       bForceRealloc = kCIDLib::False
        );

        virtual tCIDLib::TVoid Reset
        (
            const   TSize&                  szImage
            , const tCIDLib::TBoolean       bForceRealloc = kCIDLib::False
        );

        virtual tCIDLib::TVoid Reset
        (
            const   TCIDImage&              imgSource
            , const tCIDLib::TBoolean       bForceRealloc = kCIDLib::False
        );

        virtual tCIDLib::TVoid ScaleAlpha
        (
                    tCIDLib::EDirs          eDir
            , const tCIDLib::TCard4         c4StartInd
            , const tCIDLib::TCard4         c4EndInd
        );

        virtual tCIDLib::TVoid SetAll
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        virtual tCIDLib::TVoid SetAllAlpha
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        virtual tCIDLib::TVoid SetAlphaAt
        (
            const   tCIDLib::TCard4         c4ToSet
            , const TArea&                  areaTar
        );

        virtual tCIDLib::TVoid SetAlphaFromColor();

        virtual tCIDLib::TVoid SetAlphaFromColor
        (
            const   tCIDLib::TCard4         c4TransClr
        );

        virtual tCIDLib::TVoid SetEmptyTrans();

        virtual tCIDLib::TVoid SetTextAlpha
        (
            const   tCIDLib::TCard4         c4TextClr
            , const tCIDLib::TCard4         c4TransClr
            , const tCIDLib::TBoolean       bPremul
        );

        virtual tCIDLib::TVoid StoreCompRow
        (
                    tCIDLib::TCard2* const  pc2ToStore
            , const tCIDLib::TCard4         c4Row
            , const tCIDLib::EClrComps      eComp
        );

        virtual tCIDLib::TVoid ZeroAll();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4BitsPer() const;

        tCIDLib::TCard4 c4Height() const
        {
            return m_c4Height;
        }

        tCIDLib::TCard4 c4ImageSz() const
        {
            return m_c4ImageSz;
        }

        tCIDLib::TCard4 c4LineWidth() const
        {
            return m_c4LineWidth;
        }

        tCIDLib::TCard4 c4Width() const
        {
            return m_c4Width;
        }

        tCIDImage::EBitDepths eBitDepth() const
        {
            return m_eBitDepth;
        }

        tCIDImage::EPixFmts ePixFmt() const
        {
            return m_eFmt;
        }

        tCIDImage::ERowOrders eRowOrder() const
        {
            return m_eRowOrder;
        }

        const tCIDLib::TCard1* pc1Buffer() const
        {
            return m_pc1Pixels;
        }

        tCIDLib::TCard1* pc1Buffer()
        {
            return m_pc1Pixels;
        }

        TSize szImage() const;


    protected :
        // -------------------------------------------------------------------
        //  Friends
        // -------------------------------------------------------------------
        friend class TPixelArray;


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
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

        tCIDLib::TVoid CalcTransients();

        tCIDLib::TVoid ConvertToCoefs
        (
                    tCIDLib::TFloat8* const pf8ToCvt
            , const tCIDLib::TCard4         c4SampleCnt
            , const tCIDLib::TFloat8* const pf8Poles
            , const tCIDLib::TCard4         c4PoleCnt
            , const tCIDLib::TFloat8        f8Tolerance
        )   const;

        tCIDLib::TFloat8 f8InterpolatePoint
        (
            const   tCIDLib::TFloat4* const pf4Coefs
            , const tCIDLib::TCard4         c4Width
            , const tCIDLib::TCard4         c4Height
            , const tCIDLib::TFloat8        f8X
            , const tCIDLib::TFloat8        f8Y
            , const tCIDLib::TCard4         c4Degree
        )   const;

        tCIDLib::TFloat8 f8InitAntiCausalCoefs
        (
                    tCIDLib::TFloat8* const pf8ToCvt
            , const tCIDLib::TCard4         c4SampleCnt
            , const tCIDLib::TFloat8        f8Pole
        )   const;

        tCIDLib::TFloat8 f8InitCausalCoefs
        (
                    tCIDLib::TFloat8* const pf8ToCvt
            , const tCIDLib::TCard4         c4SampleCnt
            , const tCIDLib::TFloat8        f8Pole
            , const tCIDLib::TFloat8        f8Tolerance
        )   const;

        tCIDLib::TVoid InterpComp
        (
            const   tCIDLib::EClrComps      eComp
            , const tCIDLib::TFloat4* const pf4Coefs
            ,       tCIDLib::TCard2* const  pc2LoadBuf
            ,       TPixelArrayImpl&            pixaiToFill
            , const tCIDLib::TCard4         c4Degree
            , const tCIDLib::TCard2         c2MaxVal
        )   const;

        tCIDLib::TVoid LoadCoefs
        (
            const   tCIDLib::EClrComps      eComp
            ,       tCIDLib::TFloat4* const pf4ToFill
            ,       tCIDLib::TFloat8* const pf8Row
            ,       tCIDLib::TFloat8* const pf8Col
            , const tCIDLib::TCard4         c4Degree
            , const TClrPalette&            palToUse
        )   const;

        tCIDLib::TVoid PutPixel
        (
            const   tCIDLib::TCard4         c4ToPut
            , const tCIDLib::TCard4         c4XPos
            , const tCIDLib::TCard4         c4YPos
            , const tCIDLib::TCard4         c4Line
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4BufSz
        //      The current allocated size of m_pc1Pixels. The buffer can be
        //      be bigger than the current required size for the high/width
        //      and format. The size required for the image data is stored in
        //      m_c4ImageSz.
        //
        //  m_c4Height
        //      The height of the array in pixels.
        //
        //  m_c4ImageSz
        //      The full size of the pixel data, which is not the same as
        //      the allocated buffer size (which is in m_c4BufSz.)
        //
        //  m_c4LineBytePadding
        //      The number of bytes to which the size of each line is rounded
        //      up to. Normaly it's 4 bytes which is what is required for
        //      Windows to pass the data in as a bitmap.
        //
        //  m_c4LineWidth
        //      The width of a scan line in bytes (including padding.)
        //
        //  m_c4Width
        //      The width of the array in pixels.
        //
        //  m_eAdoptOpt
        //      Indicates whether we own the pixel buffer or not. If so, then
        //      we delete it on dtor or if we need to reallocate.
        //
        //  m_eBitDepth
        //      The number of bits per pixel/sample of the image. The full
        //      format description depends on the m_ePixFmt member.
        //
        //  m_eFmt
        //      The pixel format, which tells us how to interpret the bit
        //      depth field.
        //
        //  m_eRowOrder
        //      The ordering of the rows in the image data (top to bottom or
        //      bottom to top.)
        //
        //  m_pc1Pixels
        //      The pixel array data itself. It is allocated as needed to
        //      hold the data required.
        //
        //  m_c4LineWidth
        //      The number of bytes in a padded line.
        //
        //  m_c4ImageSize
        //      The size of the pixels in bytes. The size of this buffer is
        //      stored in m_c4BufSz. We won't re-allocate unless the x/y size
        //      is increased to the point that is required, or unless we are
        //      forced to (which can be done to reduce memory usage.)
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4BufSz;
        tCIDLib::TCard4         m_c4Height;
        tCIDLib::TCard4         m_c4ImageSz;
        tCIDLib::TCard4         m_c4LineBytePadding;
        tCIDLib::TCard4         m_c4LineWidth;
        tCIDLib::TCard4         m_c4Width;
        tCIDLib::EAdoptOpts     m_eAdoptOpt;
        tCIDImage::EBitDepths   m_eBitDepth;
        tCIDImage::EPixFmts     m_eFmt;
        tCIDImage::ERowOrders   m_eRowOrder;
        tCIDLib::TCard1*        m_pc1Pixels;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TPixelArrayImpl, TObject)
};

#pragma CIDLIB_POPPACK
