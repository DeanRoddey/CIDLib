//
// FILE NAME: CIDGraphDev_Bitmap.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_Bitmap.cpp file. This file implements
//  the TBitmap class, which encapsulates a bitmap in memory. The actual
//  bitmap bits are maintained by the system, though this class provides
//  methods to get copies of the palette and pixel bits.
//
//  Any derivative of TCIDImage, from CIDImage, can be used to construct
//  a TBitmap object. This allows you to read in from storage any supported
//  image format, turn it into a bitmap, and use it for drawing, drawing on,
//  etc... If you query back out the header, palette, and bit data, you can
//  use the image classes to write the data back out again in the desired
//  image format.
//
//  We also provide a small class, TSysBmpInfo, which is used to return lists
//  of system bitmaps to apps. Apps who want to persist the use of a system
//  bitmap need to persist the name. And, we want them to be able to be
//  independent of the list of bitmaps and be able to just query a list to
//  show to the user, along with the names so that they can perist them when
//  the user selects on. So this class provides the simple container for the
//  name and bitmap, so that we can return a collection of them.
//
// CAVEATS/GOTCHAS:
//
//  1)  The bitmap handle is available as a practical matter for those folks
//      who must interface with third party code. But use it carefully since
//      the handle will be destroyed when this object is destroyed.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: TBitmap
//  PREFIX: bmp
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TBitmap : public TObject, public MDuplicable, public MDrawable
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDGraphDev::TBmpHandle hbmpFromImage
        (
            const   TGraphicDevice&         gdevTarget
            , const TPixelArray&            pixaBits
            , const TClrPalette&            palToUse
            , const tCIDLib::TFloat8        f8Gamma
            , const tCIDLib::TBoolean       bTransClr
            , const tCIDLib::TCard4         c4TransClr
        );

        static tCIDGraphDev::ESysBmps eFromName
        (
            const   TString&                strName
        );

        static tCIDGraphDev::ESysBmps eFromName
        (
            const   tCIDLib::TCh* const     pszName
        );

        static tCIDGraphDev::TBmpHandle hbmpCreate
        (
            const   TFacility&              facSrc
            , const tCIDLib::TResId         ridToCreate
            , const TGraphicDevice&         gdevCompat
        );

        static tCIDGraphDev::TBmpHandle hbmpDupHandle
        (
            const   tCIDGraphDev::TBmpHandle     hbmpToDup
        );

        static const tCIDLib::TCh* pszToName
        (
            const   tCIDGraphDev::ESysBmps    eBmp
        );


        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TBitmap();

        TBitmap
        (
            const   tCIDGraphDev::ESysBmps eBitmap
        );

        TBitmap
        (
            const   TFacility&              facSource
            , const tCIDLib::TResId         ridBitmap
        );

        TBitmap
        (
            const   TSize&                  szSize
            , const tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eDepth
            , const tCIDLib::TBoolean       bForceMem = kCIDLib::False
        );

        TBitmap
        (
            const   TSize&                  szSize
            , const tCIDImage::EPixFmts     eFmt
            , const tCIDImage::EBitDepths   eDepth
            , const TClrPalette&            palToUse
            , const tCIDLib::TBoolean       bForceMem = kCIDLib::False
        );

        TBitmap
        (
            const   TGraphicDevice&         gdevTarget
            , const TString&                strFileName
        );

        TBitmap
        (
            const   TGraphicDevice&         gdevTarget
            ,       TBinInStream&           strmSrc
        );

        TBitmap
        (
            const   TGraphicDevice&         gdevTarget
            , const TCIDImage&              imgFrom
        );

        TBitmap
        (
            const   TGraphicDevice&         gdevTarget
            , const TPixelArray&            pixaFrom
            , const TClrPalette&            palToUse
        );

        TBitmap
        (
            const   TGraphicDevice&         gdevCompatible
            , const TSize&                  szSize
        );

        TBitmap
        (
            const   tCIDGraphDev::TBmpHandle hbmpSrc
            , const tCIDLib::EAdoptOpts     eAdopt
        );

        TBitmap
        (
            const   TBitmap&                bmpToCopy
        );

        ~TBitmap();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBitmap& operator=
        (
            const   TBitmap&                bmpToAssign
        );

        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid DrawOn
        (
                    TGraphDrawDev&          gdevTarget
            , const TPoint&                 pntOrg
        )   const override;

        virtual tCIDLib::TVoid DrawOn
        (
                    TGraphDrawDev&          gdevTarget
            , const TArea&                  areaToFill
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdvDrawOn
        (
                    TGraphDrawDev&          gdevTarget
            , const TArea&                  areaSource
            , const TArea&                  areaTarget
            , const tCIDLib::TBoolean       bTransparent
            , const tCIDLib::TCard4         c4TransClr
            , const TBitmap&                bmpMask
            , const tCIDGraphDev::EBltTypes eBlit
            , const tCIDLib::TCard1         c1Opacity
            , const tCIDLib::TBoolean       bForceAlpha = kCIDLib::False
        )   const;

        TBitmap bmpMakeGrayScale() const;

        TBitmap bmpMakeMask
        (
            const   TGraphDrawDev&          gdevCompat
            , const tCIDLib::TCard4         c4TransClr
            , const tCIDLib::TBoolean       bInvert
        )   const;

        TBitmap bmpMakeMask
        (
            const   TGraphDrawDev&          gdevCompat
            , const TPoint&                 pntTransClrAt
            , const tCIDLib::TBoolean       bInvert
        )   const;

        TBitmap bmpMakeMask
        (
            const   TGraphDrawDev&          gdevCompat
            , const TPoint&                 pntTransClrAt
            , const tCIDLib::TBoolean       bInvert
            ,       tCIDLib::TCard4&        c4TransClr
        )   const;

        tCIDLib::TBoolean bHasAlpha() const;

        tCIDLib::TCard4 c4RefCount() const;

        tCIDLib::TCard4 c4SerialNum() const;

        tCIDLib::TCard4 c4SerialNum
        (
            const   tCIDLib::TCard4         c4SerialNum
        );

        tCIDLib::TVoid DeepCopy();

        tCIDLib::TVoid DrawAsMask
        (
                    TGraphDrawDev&          gdevTarget
            , const TBitmap&                bmpMask
            , const TArea&                  areaSource
            , const TArea&                  areaTarget
            , const tCIDGraphDev::EBltTypes eBlt
        )   const;

        tCIDLib::TVoid DrawTransparent
        (
                    TGraphDrawDev&          gdevTarget
            , const TPoint&                 pntTarget
            , const tCIDLib::TCard4         c4TransClr
        )   const;

        tCIDLib::TVoid DrawTransparent
        (
                    TGraphDrawDev&          gdevTarget
            , const TArea&                  areaTarget
            , const tCIDLib::TCard4         c4TransClr
        )   const;

        tCIDLib::TVoid DrawTransparent
        (
                    TGraphDrawDev&          gdevTarget
            , const TArea&                  areaSource
            , const TArea&                  areaTarget
            , const tCIDLib::TCard4         c4TransClr
        )   const;

        tCIDGraphDev::TBmpHandle hbmpThis() const;

        tCIDLib::TVoid IncSerialNum();

        tCIDLib::TVoid Invert
        (
            const   TGraphDrawDev&          gdevCompat
        );

        tCIDLib::TVoid QueryHdrInfo
        (
                    TBitmapImage::TBmpHdrInfo& bhiToFill
        )   const;

        tCIDLib::TVoid QueryImgData
        (
            const   TGraphDrawDev&          gdevCompat
            ,       TPixelArray&            pixaToFill
            ,       TClrPalette&            palToFill
            , const tCIDLib::TBoolean       bForceAlphaIf32 = kCIDLib::False
        )   const;

        tCIDLib::TVoid QuerySize
        (
                    tCIDLib::TCard4&        c4Width
            ,       tCIDLib::TCard4&        c4Height
        )   const;

        tCIDLib::TVoid MaskWith
        (
            const   TGraphDrawDev&          gdevCompat
            , const TBitmap&                bmpInvMask
        );

        tCIDLib::TVoid MaskWith
        (
            const   TGraphDrawDev&          gdevCompat
            , const tCIDLib::TCard4         c4TransClr
        );

        const TPixelArray& pixaData() const;

        TPixelArray& pixaData();

        const TPixelArray* ppixaData() const;

        TPixelArray* ppixaData();

        TSize szBitmap() const;

        tCIDLib::TVoid ReapplyAlphaMask
        (
            const   TBitmap&                bmpOriginal
            ,       TGraphDrawDev&          gdevCompat
            , const TPoint&                 pntSrc
            , const TPoint&                 pntTar
            , const tCIDLib::TBoolean       bResetAllAlpha = kCIDLib::False
        );

        tCIDLib::TVoid Reset();

        const TString& strUserData() const;

        const TString& strUserData
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid Set
        (
            const   tCIDGraphDev::ESysBmps eBitmap
        );


    private :
        // -------------------------------------------------------------------
        //   CLASS: TRefBmpHandle
        //  PREFIX: rbh
        // -------------------------------------------------------------------
        class TRefBmpHandle
        {
            public :
                // -----------------------------------------------------------
                //  Ctors and Dtor
                // -----------------------------------------------------------
                TRefBmpHandle
                (
                    const   tCIDLib::TBoolean           bSrcAlpha = kCIDLib::False
                );

                TRefBmpHandle
                (
                    const   tCIDGraphDev::TBmpHandle    hbmpToAdopt
                    , const tCIDLib::TBoolean           bSrcAlpha = kCIDLib::False
                    , const tCIDGraphDev::TDeviceHandle hdevToUse = 0
                    ,       TPixelArray* const          pixaData = 0
                );

                TRefBmpHandle(const TRefBmpHandle&) = delete;

                ~TRefBmpHandle();


                // -----------------------------------------------------------
                //  Public operators
                // -----------------------------------------------------------
                TRefBmpHandle& operator=(const TRefBmpHandle&) = delete;


                // -----------------------------------------------------------
                //  Public, non-virtual methods
                // -----------------------------------------------------------
                tCIDLib::TBoolean bSrcAlpha() const;

                tCIDLib::TBoolean bSrcAlpha
                (
                    const   tCIDLib::TBoolean           bToSet
                );

                tCIDLib::TCard4 c4SerialNum() const;

                tCIDLib::TCard4 c4SerialNum
                (
                    const   tCIDLib::TCard4             c4ToSet
                );

                const TBitmapImage::TBmpHdrInfo& bhiThis() const;

                tCIDGraphDev::TBmpHandle hbmpDup() const;

                tCIDGraphDev::TBmpHandle hbmpThis() const;

                tCIDLib::TVoid IncSerialNum();

                const TPixelArray* ppixaData() const;

                TPixelArray* ppixaData();

                const TString& strUserData() const;

                const TString& strUserData
                (
                    const   TString&                    strToSet
                );

                TSize szBitmap() const;


            private :
                // -----------------------------------------------------------
                //  Private, non-virtual methods
                // -----------------------------------------------------------
                tCIDLib::TVoid UpdateInfo
                (
                    const   tCIDGraphDev::TDeviceHandle hdevToUse
                );


                // -----------------------------------------------------------
                //  Private data members
                //
                //  m_bHasSrcAlpha
                //      If the original data we created the bitmap from had
                //      source alpha, we set this flag to let later users of
                //      this flag know this.
                //
                //  m_bhiThis
                //      Bitmap info about this bitmap, for later use.
                //
                //  m_c4SerialNum
                //      A serial number that the client code can use to bump
                //      any time it changes the image if it wants, for it's
                //      own purposes. This is not persisted in any way, and
                //      we never change it ourself. It's purely up to the
                //      client to change it as he sees fit.
                //
                //  m_hbmpThis
                //      The handle to the bitmap represented by this object.
                //
                //  m_ppixaData
                //      If this is a DIB section type bitmap, and we created
                //      it, i.e. we allocated the memory, then this is a
                //      pointer to the pixel array. This lets us get access
                //      to the data for efficient direct manipulation. Note
                //      that it damn well better match the actual bitmap layout
                //      or bad things are going to happen.
                //
                //  m_strUserData
                //      This is for the use of client code. They can store
                //      whatever they want here. It's normally used to keep
                //      track of where a bitmap came from, since normally the
                //      original was a JPEG or PNG or whatever, which got
                //      converted to a bitmap when loaded so there's no way
                //      to know where it came from later when it's used. So
                //      it's often a path of some sort or unique identifier
                //      within an external storage system and so forth.
                // -----------------------------------------------------------
                tCIDLib::TBoolean           m_bHasSrcAlpha;
                tCIDLib::TCard4             m_c4SerialNum;
                TBitmapImage::TBmpHdrInfo   m_bhiThis;
                tCIDGraphDev::TBmpHandle    m_hbmpThis;
                TPixelArray*                m_ppixaData;
                TString                     m_strUserData;
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid LoadSysBmp
        (
            const   tCIDGraphDev::ESysBmps    eToLoad
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_cptrHandle
        //      This is our counted pointer that we use to ref count the
        //      underlying bitmap handle object.
        // -------------------------------------------------------------------
        TCntPtr<TRefBmpHandle>  m_cptrHandle;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBitmap,TObject)
        DefPolyDup(TBitmap)
};


// ---------------------------------------------------------------------------
//   CLASS: TSysBitmapInfo
//  PREFIX: sbmpi
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TSysBitmapInfo : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TSysBitmapInfo();

        TSysBitmapInfo
        (
            const   TString&                strName
            , const TBitmap&                bmpImage
        );

        TSysBitmapInfo
        (
            const   TSysBitmapInfo&         sbmpiToCopy
        );

        ~TSysBitmapInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSysBitmapInfo& operator=
        (
            const   TSysBitmapInfo&         sbmpiToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TBitmap& bmpImage() const;

        const TString& strName() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bmpImage
        //      The bitmap image itself. Remember that bitmap objects are
        //      just reference counters of the underlying handle, so this is
        //      a relatively light operation to have a by value copy.
        //
        //  m_strName
        //      The name of the system image that was loaded.
        // -------------------------------------------------------------------
        TBitmap     m_bmpImage;
        TString     m_strName;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSysBitmapInfo,TObject)
};

#pragma CIDLIB_POPPACK

