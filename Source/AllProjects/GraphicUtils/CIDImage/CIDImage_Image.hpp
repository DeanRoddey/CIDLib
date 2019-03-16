//
// FILE NAME: CIDImage_Image.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/17/1998
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header for the CIDImage_Image.cpp file. This file implements
//  the TCIDImage class, which is the base class from which all image classes
//  are derived. This allows any image to be dealt with polymorphically.
//
// CAVEATS/GOTCHAS:
//
//  1)  This class does not provide streaming itself. It assumes that each
//      derive class will do so, and that they will stream in/out their
//      native graphics file formats, not a CIDLib specific format.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TCIDImage
// PREFIX: img
// ---------------------------------------------------------------------------
class CIDIMGEXP TCIDImage :

    public TObject, public MDuplicable, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid CvtTransClr
        (
            const   tCIDImage::EPixFmts     ePixFmt
            , const tCIDImage::EBitDepths   eBitDepth
            , const tCIDLib::TCard4         c4TransClr
            , const TClrPalette&            palImg
            ,       TRGBClr&                rgbToFill
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TCIDImage();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsRGBBased() const;

        tCIDLib::TBoolean bLoseAlpha
        (
                    TPixelArray&            pixaTmp
            ,       tCIDLib::TCard4&        c4TransClr
        );

        tCIDLib::TBoolean bTransClr() const;

        tCIDLib::TBoolean bTransClr
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TVoid BSplineScaleFrom
        (
            const   TCIDImage&              imgSrc
            , const tCIDLib::TCard4         c4SplineDegree
        );

        tCIDLib::TVoid BSplineScaleTo
        (
                    TPixelArray&            pixaTar
            , const tCIDLib::TCard4         c4SplineDegree
        )   const;

        tCIDLib::TCard4 c4BitsPerPixel() const;

        tCIDLib::TCard4 c4ClrAt
        (
            const   TPoint&                 pntAt
        )   const;

        tCIDLib::TCard4 c4ClrsAvail() const;

        tCIDLib::TCard4 c4ClrsImportant() const;

        tCIDLib::TCard4 c4ClrsUsed() const;

        tCIDLib::TCard4 c4TransClr() const;

        tCIDLib::TCard4 c4TransClr
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4UserData() const;

        tCIDLib::TCard4 c4UserData
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4Height() const;

        tCIDLib::TCard4 c4ImageSz() const;

        tCIDLib::TCard4 c4LineByteWidth() const;

        tCIDLib::TCard4 c4Width() const;

        tCIDLib::TVoid CvtToGrayScale();

        tCIDImage::EBitDepths eBitDepth() const;

        tCIDImage::EPixFmts ePixFmt() const;

        tCIDImage::ERowOrders eRowOrder() const;

        tCIDImage::EImgTypes eType() const;

        tCIDLib::TFloat8 f8Gamma() const;

        tCIDLib::TFloat8 f8Gamma
        (
            const   tCIDLib::TFloat8        f8ToSet
        );

        tCIDLib::TVoid FlipHorizontally();

        tCIDLib::TVoid FlipVertically();

        tCIDLib::TVoid MakeScaled
        (
                    TPixelArray&            pixaThumb
            , const tCIDLib::TCard4         c4Degree
        )   const;

        tCIDLib::TVoid MakeScaledFrom
        (
            const   TCIDImage&              imgSrc
            , const tCIDLib::TCard4         c4Degree
        );

        const TClrPalette& palClrs() const;

        TClrPalette& palClrs();

        const TPixelArray& pixaBits() const;

        TRGBClr rgbAt
        (
            const   TPoint&                 pntAt
        )   const;

        TRGBClr rgbAt
        (
            const   tCIDLib::TCard4         c4Col
            , const tCIDLib::TCard4         c4Row
        )   const;

        const TRGBClr& rgbBgnClr() const;

        const TRGBClr& rgbBgnClr
        (
            const   TRGBClr&                rgbToSet
        );

        TRGBClr rgbTransClr() const;

        TSize szImage() const;

        TSize szResolution() const;

        TSize szResolution
        (
            const   TSize&                  szToSet
        );

        tCIDLib::TVoid SetBits
        (
            const   TPixelArray&            pixaToSet
        );

        tCIDLib::TVoid PalToRGB();

        tCIDLib::TVoid PalToRGB
        (
            const   TRGBClr&                rgbTransRep
        );

        tCIDLib::TVoid RGBTToRGBA();

        tCIDLib::TVoid RGBTToRGBA
        (
            const   TRGBClr&                rgbTransRep
        );

        tCIDLib::TVoid ReplaceTransClr
        (
            const   tCIDLib::TCard4         c4RepClr
        );

        tCIDLib::TVoid Scale
        (
            const   TSize&                  szNew
            , const tCIDLib::TCard4         c4SplineDegree
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TCIDImage
        (
            const   tCIDImage::EImgTypes    eType
        );

        TCIDImage
        (
            const   tCIDImage::EImgTypes    eType
            , const tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
            , const tCIDImage::ERowOrders   eRowOrder
            , const TSize&                  szImage
            , const TClrPalette&            palToUse
            , const tCIDLib::TCard4         c4ClrsImportant
            , const tCIDLib::TCard4         c4ClrsUsed
        );

        TCIDImage
        (
            const   tCIDImage::EImgTypes    eType
            , const TPixelArray&            pixaToSet
            , const tCIDLib::TCard4         c4ClrsImportant
            , const tCIDLib::TCard4         c4ClrsUsed
        );

        TCIDImage
        (
            const   tCIDImage::EImgTypes    eType
            , const TPixelArray&            pixaToSet
            , const TClrPalette&            palToUse
            , const tCIDLib::TCard4         c4ClrsImportant
            , const tCIDLib::TCard4         c4ClrsUsed
        );

        TCIDImage
        (
            const   tCIDImage::EImgTypes    eType
            , const TCIDImage&              imgSrc
            , const TSize&                  szNew
        );

        TCIDImage
        (
            const   tCIDImage::EImgTypes    eType
            , const TCIDImage&              imgToCopy
        );

        TCIDImage& operator=
        (
            const   TCIDImage&              imgToAssign
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ForceRowOrder
        (
            const   tCIDImage::ERowOrders   eRowOrder
        );

        TPixelArray& pixaNCBits();

        tCIDLib::TVoid Set
        (
            const   tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
            , const tCIDImage::ERowOrders   eRowOrder
            , const TSize&                  szImage
            , const TClrPalette&            palToUse
            , const tCIDLib::TCard4         c4ClrsImportant
            , const tCIDLib::TCard4         c4ClrsUsed
        );

        tCIDLib::TVoid Set
        (
            const   tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eBitDepth
            , const tCIDImage::ERowOrders   eRowOrder
            , const TSize&                  szImage
            , const tCIDLib::TCard4         c4ClrsImportant
            , const tCIDLib::TCard4         c4ClrsUsed
        );

        tCIDLib::TVoid Set
        (
            const   TPixelArray&            pixaToSet
            , const tCIDLib::TCard4         c4ClrsImportant
            , const tCIDLib::TCard4         c4ClrsUsed
        );

        tCIDLib::TVoid Set
        (
            const   TPixelArray&            pixaToSet
            , const TClrPalette&            palToUse
            , const tCIDLib::TCard4         c4ClrsImportant
            , const tCIDLib::TCard4         c4ClrsUsed
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BuildGrayScalePal();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bTransClr
        //      Indicates whether color based transparency is enabled for this
        //      image, in which case m_rgbTrans is the transparency color.
        //
        //  m_c4ClrsImportant
        //  m_c4ClrsUsed
        //      Only used if the palette is valid. If not all of the palette
        //      colors are really used, this can indicate so. Else it will
        //      be set to the same as the palette size.
        //
        //  m_c4HorzRes
        //      The horizontal pixels per meter of the device for which this
        //      bitmap was created. This can be zero if the image has no
        //      particular resolution target.
        //
        //  m_c4VertRes
        //      The vertical pixels per meter of the device for which this
        //      bitmap was created. This can be zero if the image has no
        //      particular resolution target.
        //
        //  m_c4TransClr
        //      If m_bTransClr is set, then this is the transparency 'color'.
        //      Color is quoted because it must be in the native format of
        //      the image, so if the image is palette/gray scale, then it
        //      must be a gray scale value or a palette index.
        //
        //  m_c4UserData
        //      For use by the application.
        //
        //  m_eType
        //      The derived class passes the image type to our ctor to store
        //      away and make available to clients to quickly see what type
        //      of image.
        //
        //  m_palClrs
        //      The color palette for this image. If the color format does
        //      support a palette, it'll just be a default black/white 2 color
        //      palette.
        //
        //  m_f8Gamma
        //      The gamma setting. Defaults to 2.2 if not read in from image.
        //
        //  m_pixaBits
        //      The pixel data. The format is defined by the bit depth and
        //      pixel format. This guy contains the pixel format, bit depth,
        //      image size, etc..., so we don't store our own copies.
        //
        //  m_rgbBgn
        //      A preferred background color, mostly used when previewing
        //      transparent images.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bTransClr;
        tCIDLib::TCard4         m_c4ClrsImportant;
        tCIDLib::TCard4         m_c4ClrsUsed;
        tCIDLib::TCard4         m_c4HorzRes;
        tCIDLib::TCard4         m_c4VertRes;
        tCIDLib::TCard4         m_c4TransClr;
        tCIDLib::TCard4         m_c4UserData;
        tCIDImage::EImgTypes    m_eType;
        tCIDLib::TFloat8        m_f8Gamma;
        TClrPalette             m_palClrs;
        TPixelArray             m_pixaBits;
        TRGBClr                 m_rgbBgn;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDImage,TObject)
};

#pragma CIDLIB_POPPACK

