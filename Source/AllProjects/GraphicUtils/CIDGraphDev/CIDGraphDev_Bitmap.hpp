//
// FILE NAME: CIDGraphDev_Bitmap.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/1998
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
//  This is the header for the CIDLib_Bitmap.cpp file. This file implements
//  the TBitmap class, which encapsulates a Windows bitmap (the graphics
//  object, not the file format, the file format class, TBitmapImage, is in
//  the CIDImage facility.)
//
//  Portability
//
//  Like the other  graphics object (devices, bitmaps, brushes, pens, etc...)
//  and the UI controls, though we fully encapsulate them, we aren't trying to
//  make these portable at this time. Read/writing images, and manipulation of
//  the image content via TPixelArray, is portable since it doesn't involve any
//  actual UI functionality. But this guy is currenctly just a Win32 bitmap
//  wrapper.
//
//  Portability is greatly complicated by the requirement of Win32 for bitmaps
//  to be pre-multiplied. The hbmpFromImage() static method, which is used
//  internally here to convert raw image data to bitmaps, will do pre-mul if
//  there is an alpha channel. So typically the deal is that you only deal with
//  non-pre-multiplied data in TPixelArray objects (and TCIDImage derivatives
//  which use a pixel array internally.) Once it's turned into a graphical
//  bitmap object it needs to be in drawable condition.
//
//
//  Copying/Value Semantics
//
//  This class actually maintains a reference counted bitmap handle object,
//  so that we can support copy semantics cheaply. But, it does mean you have
//  to be careful that you force a deep copy if you really want to create a
//  copy of the actual bits!
//
//  Memory vs. Device
//
//  Bitmaps can be device or memory based. It may end up being memory based
//  just because the device can't handle the format. But, if you need it to be
//  memory based, be sure to force it to be. Otehrwise, it might end up on
//  the device.
//
//  If you create in memory, then you can access the pixel data via a TPixelArray
//  object and directly manipulate the pixel data. If the bitmap is memory based
//  then you must actually extract the pixel data to a separate pixel array. You
//  can then manipulate it and push it back in if desired.
//
//  Creation
//
//  You can create bitmaps in many ways, from an image file, from an in-memory
//  TCIDImage derived class, based on another bitmap, a manually indicated
//  format, with a size plus a compatible graphics device whose bit format should
//  be used, or from a bitmap resource attached to a library/executable module.
//
//  Serial Number
//
//  This class provides a serial number that client code can use to track
//  changes. This class cannot manage that serial number since it cannot know
//  when changes have actually been made. It's just for the application's use.
//  CQC uses it extensively, for instance.
//
//  Drawing
//
//  The TGraphicDevice (and derivatives) provide APIs for drawing these onto
//  graphics output devices. We also implement MDrawable for a generic drawing
//  interface if that's useful.
//
//  And we also provide an 'advance drawing' method that combines a lot of very
//  common bitmap drawing operations in a single call. It handles scaling or
//  clipping, transparency (alpha or masked color based), and tiling all at
//  once. This can be a big help for programs that need to deal with various
//  types of bitmaps and drawing scenarios. It doesn't support any raster ops,
//  just source copy.
//
//
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
//      the handle will be destroyed when this object is destroyed. If you
//      want your own copy of it, see #2 below.
//
//  2)  This underlying bitmap is actually an internally ref counted handle
//      object. YOU MUST force a deep copy to create a new copy of the
//      underlying pixel data.
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
            const   tCIDGraphDev::TBmpHandle hbmpToDup
        );

        static const tCIDLib::TCh* pszToName
        (
            const   tCIDGraphDev::ESysBmps  eBmp
        );


        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TBitmap();

        TBitmap
        (
            const   tCIDGraphDev::ESysBmps  eBitmap
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
            const   TBitmap&                bmpSrc
        );

        ~TBitmap();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBitmap& operator=
        (
            const   TBitmap&                bmpSrc
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
            const   tCIDGraphDev::ESysBmps  eBitmap
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
            const   tCIDGraphDev::ESysBmps  eToLoad
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

