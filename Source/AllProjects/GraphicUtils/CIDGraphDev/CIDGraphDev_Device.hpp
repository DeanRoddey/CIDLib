//
// FILE NAME: CIDGraphDev_Device.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
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
//  This is the header for the CIDGraphDev_Device.cpp file. This file implements
//  the TGraphicDevice class, which is a wrapper for the host OS graphics
//  output device handle, a DC in Windows speak. TGraphicDevice provides
//  basic graphics output mechanisms, and there are derivatives of it (defined
//  elsewhere) that are used for particular circumstances.
//
//  The only derivative defined here is the TGraphDrawDev from which are
//  derived all of the device classes that support graphics output. Other
//  derivatives of TGraphicDevice might not support any drawing, i.e. they
//  might be only for querying information.
//
//  The drawing methods are virtual so that we can support remote graphics
//  output. It's always a real device, but it provides the means to do a
//  derivative that intercepts any desired graphical operations and send them
//  to some other program for rendering. It's always a real device because
//  there is also the need to do local capabilities and queries and other
//  ops, which would be very piggy to have to do remotely. So the app would
//  keep the local device content up to date with the remote for that reason.
//
// CAVEATS/GOTCHAS:
//
//  1)  Devices are NOT threadsafe, so protect them fully if you draw to them
//      from multiple threads.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class   TBitmap;
class   TIcon;
class   TPointArray;
struct  tagLOGFONTW;


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TGraphicDevice
// PREFIX: gdev
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TGraphicDevice : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TGraphicDevice();

        TGraphicDevice(const TGraphicDevice&) = delete;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGraphicDevice& operator=(const TGraphicDevice&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bCapConstAlpha() const;

        virtual tCIDLib::TBoolean bCapSrcAlpha() const;

        virtual tCIDLib::TBoolean bIsPaletteBased() const;

        virtual tCIDLib::TCard4 c4BitsPerPixel() const;

        virtual tCIDLib::EAdoptOpts eAdopted() const;

        virtual tCIDGraphDev::TDeviceHandle hdevThis() const;

        virtual TSize szPixels() const;

        virtual TSize szPixelsPerInch() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TGraphicDevice();

        TGraphicDevice
        (
            const   tCIDGraphDev::TDeviceHandle hdevToUse
            , const tCIDLib::EAdoptOpts         eAdopt = tCIDLib::EAdoptOpts::Adopt
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetHandle
        (
            const   tCIDGraphDev::TDeviceHandle hdevToUse
            , const tCIDLib::EAdoptOpts         eAdopt = tCIDLib::EAdoptOpts::Adopt
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid OneTimeDeviceInit();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eAdopt
        //      This indicates whether we adopted the device or are just
        //      referencing it.
        //
        //  m_c4BitsPerPixel
        //      The bits per pixel in the target device. We need this to
        //      decide whether we can do certain things, such as alpha
        //      blending.
        //
        //  m_c4XXXCaps
        //      Some capabilities flags that we pre-lookup and use to avoid
        //      doing things not supported by the local platform.
        //
        //  m_hdevThis
        //      This is the device handle that we are wrapping. Pretty much
        //      all of the methods just massage parameters then call a system
        //      API, passing this handle on.
        // -------------------------------------------------------------------
        tCIDLib::EAdoptOpts         m_eAdopt;
        tCIDLib::TCard4             m_c4BitsPerPixel;
        tCIDLib::TCard4             m_c4BlendCaps;
        tCIDGraphDev::TDeviceHandle m_hdevThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGraphicDevice,TObject)
        NulObject(TGraphicDevice)
};


// ---------------------------------------------------------------------------
//  CLASS: TGraphDrawDev
// PREFIX: gdev
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TGraphDrawDev : public TGraphicDevice
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid WaitRetrace();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TGraphDrawDev
        (
            const   tCIDGraphDev::TDeviceHandle hdevToUse
            , const tCIDLib::EAdoptOpts         eAdopt = tCIDLib::EAdoptOpts::Adopt
        );

        TGraphDrawDev(const TGraphDrawDev&) = delete;

        ~TGraphDrawDev();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGraphDrawDev& operator=(const TGraphDrawDev&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid AlphaBlit
        (
            const   TBitmap&                    bmpSrc
            , const TArea&                      areaSource
            , const TArea&                      areaDest
            , const tCIDLib::TCard1             c1Alpha
            , const tCIDLib::TBoolean           bSrcAlpha
            , const tCIDLib::TBoolean           bWaitRetrace = kCIDLib::False
        );

        virtual tCIDLib::TVoid AlphaBlit
        (
            const   TBitmap&                    bmpSrc
            , const TArea&                      areaDest
            , const tCIDLib::TCard1             c1Alpha
            , const tCIDLib::TBoolean           bSrcAlpha
            , const tCIDLib::TBoolean           bWaitRetrace = kCIDLib::False
        );

        virtual tCIDLib::TVoid AlphaBlit
        (
            const   TBitmap&                    bmpSrc
            , const TPoint&                     pntDest
            , const tCIDLib::TCard1             c1Alpha
            , const tCIDLib::TBoolean           bSrcAlpha
            , const tCIDLib::TBoolean           bWaitRetrace = kCIDLib::False
        );

        virtual tCIDLib::TVoid AlphaBlit
        (
            const   TBitmap&                    bmpSrc
            , const TArea&                      areaSource
            , const TPoint&                     pntDestUL
            , const tCIDLib::TCard1             c1Alpha
            , const tCIDLib::TBoolean           bSrcAlpha
            , const tCIDLib::TBoolean           bWaitRetrace = kCIDLib::False
        );

        virtual tCIDLib::TVoid AlphaBlit
        (
            const   TGraphDrawDev&              gdevSrc
            , const TArea&                      areaSource
            , const TArea&                      areaDest
            , const tCIDLib::TCard1             c1Alpha
            , const tCIDLib::TBoolean           bSrcAlpha
            , const tCIDLib::TBoolean           bWaitRetrace = kCIDLib::False
        );

        virtual TArea areaBounds
        (
            const   tCIDLib::TBoolean           bReset = kCIDLib::False
        )   const;

        virtual TArea areaMLText
        (
            const   TString&                    strText
            , const tCIDLib::TCard4             c4Width
            , const tCIDLib::TCard4             c4Start = 0
            , const tCIDLib::TCard4             c4Len = kCIDLib::c4MaxCard
        );

        virtual TArea areaString
        (
            const   TString&                    strText
            , const tCIDLib::TBoolean           bSkipMnemonic = kCIDLib::True
        )   const;

        virtual TArea areaString
        (
            const   TString&                    strText
            , const tCIDLib::TCard4             c4Start
            , const tCIDLib::TCard4             c4Len = kCIDLib::c4MaxCard
            , const tCIDLib::TBoolean           bSkipMnemonic = kCIDLib::True
        )   const;

        virtual tCIDLib::TBoolean bBoundsEnabled() const;

        virtual tCIDLib::TBoolean bExcludeClipArea
        (
            const   TArea&                      areaExclude
        );

        virtual tCIDLib::TBoolean bIntersectClipArea
        (
            const   TArea&                      areaInter
        );

        virtual tCIDLib::TVoid BeginPath();

        virtual tCIDLib::TCard4 c4Extra() const;

        virtual tCIDLib::TCard4 c4Extra
        (
            const   tCIDLib::TCard4             c4ToSet
        );

        virtual tCIDLib::TCard4 c4HPelsPerInch() const;

        virtual tCIDLib::TCard4 c4VPelsPerInch() const;

        virtual tCIDLib::TVoid ClearClipRegion();

        virtual tCIDLib::TVoid CopyBits
        (
                    TGraphDrawDev&              gdevSrc
            , const TPoint&                     pntSrc
            , const TArea&                      areaTarget
            , const tCIDGraphDev::EBmpModes     eMode = tCIDGraphDev::EBmpModes::SrcCopy
            , const tCIDLib::TBoolean           bWaitRetrace = kCIDLib::False
        );

        virtual tCIDLib::TVoid CopyBits
        (
                    TGraphDrawDev&              gdevSrc
            , const TArea&                      areaSrc
            , const TArea&                      areaTarget
            , const tCIDGraphDev::EBmpModes     eMode = tCIDGraphDev::EBmpModes::SrcCopy
            , const tCIDLib::TBoolean           bWaitRetrace = kCIDLib::False
        );

        virtual tCIDLib::TVoid DisableBoundsAccumulation();

        virtual tCIDLib::TVoid DrawArc
        (
            const   TPoint&                     pntCenter
            , const tCIDLib::TCard4             c4Radius
            , const tCIDLib::TCard4             c4StartDegree
            , const tCIDLib::TCard4             c4EndDegree
        );

        virtual tCIDLib::TVoid DrawBitmap
        (
            const   TBitmap&                    bmpToDraw
            , const TPoint&                     pntDestUL
            , const tCIDGraphDev::EBmpModes     eMode = tCIDGraphDev::EBmpModes::SrcCopy
        );

        virtual tCIDLib::TVoid DrawBitmap
        (
            const   TBitmap&                    bmpToDraw
            , const TPoint&                     pntSrcUL
            , const TPoint&                     pntDestUL
            , const tCIDGraphDev::EBmpModes     eMode = tCIDGraphDev::EBmpModes::SrcCopy
        );

        virtual tCIDLib::TVoid DrawBitmap
        (
            const   TBitmap&                    bmpToDraw
            , const TArea&                      areaDest
            , const tCIDGraphDev::EBmpModes     eMode = tCIDGraphDev::EBmpModes::SrcCopy
        );

        virtual tCIDLib::TVoid DrawBitmap
        (
            const   TBitmap&                    bmpToDraw
            , const TArea&                      areaSource
            , const TArea&                      areaDest
            , const tCIDGraphDev::EBmpModes     eMode = tCIDGraphDev::EBmpModes::SrcCopy
        );

        virtual tCIDLib::TVoid DrawBitmap
        (
            const   TBitmap&                    bmpToDraw
            , const TArea&                      areaSource
            , const TPoint&                     pntDestUL
            , const tCIDGraphDev::EBmpModes     eMode = tCIDGraphDev::EBmpModes::SrcCopy
        );

        virtual tCIDLib::TVoid DrawBitmapAlpha
        (
            const   TBitmap&                    bmpToDraw
            , const TArea&                      areaSource
            , const TArea&                      areaDest
            , const tCIDGraphDev::EBmpModes     eMode = tCIDGraphDev::EBmpModes::SrcCopy
        );

        virtual tCIDLib::TVoid DrawBitmapMasked
        (
            const   TBitmap&                    bmpToDraw
            , const TArea&                      areaSource
            , const TArea&                      areaDest
            , const tCIDLib::TCard4             c4TransClr
            , const tCIDLib::TBoolean           bForceAlpha = kCIDLib::False
        );

        virtual tCIDLib::TVoid DrawBitmapMasked
        (
            const   TBitmap&                    bmpToDraw
            , const TArea&                      areaSource
            , const TPoint&                     pntDestUL
            , const tCIDLib::TCard4             c4TransClr
            , const tCIDLib::TBoolean           bForceAlpha = kCIDLib::False
        );

        virtual tCIDLib::TVoid DrawBitmapMasked
        (
            const   TBitmap&                    bmpToDraw
            , const TPoint&                     pntDestUL
            , const tCIDLib::TCard4             c4TransClr
            , const tCIDLib::TBoolean           bForceAlpha = kCIDLib::False
        );

        virtual tCIDLib::TVoid DrawBitmapMasked
        (
            const   TBitmap&                    bmpToDraw
            , const TBitmap&                    bmpMask
            , const TArea&                      areaSource
            , const TArea&                      areaDest
            , const tCIDLib::TCard4             c4TransClr
            , const tCIDLib::TBoolean           bForceAlpha = kCIDLib::False
        );

        virtual tCIDLib::TVoid DrawBitmapMasked
        (
            const   TBitmap&                    bmpToDraw
            , const TBitmap&                    bmpMask
            , const TArea&                      areaSource
            , const TPoint&                     pntDestUL
            , const tCIDLib::TCard4             c4TransClr
            , const tCIDLib::TBoolean           bForceAlpha = kCIDLib::False
        );

        virtual tCIDLib::TVoid DrawBitmapMasked
        (
            const   TBitmap&                    bmpToDraw
            , const TBitmap&                    bmpMask
            , const TPoint&                     pntDestUL
            , const tCIDLib::TCard4             c4TransClr
            , const tCIDLib::TBoolean           bForceAlpha = kCIDLib::False
        );

        virtual tCIDLib::TVoid DrawBox
        (
            const   TPoint&                     pntFrom
            , const TPoint&                     pntTo
            , const tCIDLib::TCard4             c4Rounding = 0
        );

        virtual tCIDLib::TVoid DrawBox
        (
            const   TArea&                      areaBox
            , const tCIDLib::TCard4             c4Rounding = 0
        );

        virtual tCIDLib::TVoid DrawCircle
        (
            const   tCIDLib::TCard4             c4Radius
            , const TPoint&                     pntCenter
        );

        virtual tCIDLib::TVoid DrawFilledCircle
        (
            const   tCIDLib::TCard4             c4Radius
            , const TPoint&                     pntCenter
        );

        virtual tCIDLib::TVoid DrawFocusArea
        (
            const   TArea&                      areaToDraw
        );

        virtual tCIDLib::TVoid DrawGradientCircle
        (
            const   tCIDLib::TCard4             c4Radius
            , const TPoint&                     pntCenter
            , const TRGBClr&                    rgbLight
            , const TRGBClr&                    rgbDark
        );

        virtual tCIDLib::TVoid DrawIcon
        (
            const   TIcon&                      icoToDraw
            , const TPoint&                     pntDestUL
        );

        virtual tCIDLib::TVoid DrawIcon
        (
            const   TIcon&                      icoToDraw
            , const TPoint&                     pntDestUL
            , const TSize&                      szStretchTo
        );

        virtual tCIDLib::TVoid DrawIcon
        (
            const   TIcon&                      icoToDraw
            , const TArea&                      areaToFill
        );

        virtual tCIDLib::TVoid DrawIconPart
        (
            const   TIcon&                      icoToDraw
            , const TPoint&                     pntDestUL
            , const tCIDGraphDev::EBmpModes     eMode
            , const tCIDLib::TBoolean           bMask = kCIDLib::False
        );

        virtual tCIDLib::TVoid DrawIconPart
        (
            const   TIcon&                      icoToDraw
            , const TPoint&                     pntDestUL
            , const TSize&                      szStretchTo
            , const tCIDGraphDev::EBmpModes     eMode
            , const tCIDLib::TBoolean           bMask = kCIDLib::False
        );

        virtual tCIDLib::TVoid DrawIconPart
        (
            const   TIcon&                      icoToDraw
            , const TArea&                      areaToFill
            , const tCIDGraphDev::EBmpModes     eMode
            , const tCIDLib::TBoolean           bMask = kCIDLib::False
        );

        virtual tCIDLib::TVoid DrawLine
        (
            const   TPoint&                     pntTo
        );

        virtual tCIDLib::TVoid DrawLine
        (
            const   TPoint&                     pntTo
            , const TRGBClr&                    rgbClr
        );

        virtual tCIDLib::TVoid DrawLine
        (
            const   tCIDLib::TInt4              i4X
            , const tCIDLib::TInt4              i4Y
        );

        virtual tCIDLib::TVoid DrawLine
        (
            const   tCIDLib::TInt4              i4X
            , const tCIDLib::TInt4              i4Y
            , const TRGBClr&                    rgbClr
        );

        virtual tCIDLib::TVoid DrawLine
        (
            const   TPoint&                     pntFrom
            , const TPoint&                     pntTo
        );

        virtual tCIDLib::TVoid DrawLine
        (
            const   TPoint&                     pntFrom
            , const TPoint&                     pntTo
            , const TRGBClr&                    rgbClr
        );

        virtual tCIDLib::TVoid DrawMText
        (
            const   TString&                    strText
            , const TArea&                      areaFormat
            , const tCIDLib::EHJustify          eHJustify = tCIDLib::EHJustify::Left
            , const tCIDLib::EVJustify          eVJustify = tCIDLib::EVJustify::Top
            , const tCIDLib::TBoolean           bWordBreak = kCIDLib::True
        );

        virtual tCIDLib::TVoid DrawMTextAlpha
        (
            const   TString&                    strText
            , const TArea&                      areaFormat
            ,       TBitmap&                    bmpBuffer
            ,       TBitmap&                    bmpMask
            , const tCIDLib::EHJustify          eHJustify = tCIDLib::EHJustify::Left
            , const tCIDLib::EVJustify          eVJustify = tCIDLib::EVJustify::Top
            , const tCIDLib::TBoolean           bWordBreak = kCIDLib::True
            , const tCIDLib::TCard1             c1ConstAlpha = 0xFF
        );

        virtual tCIDLib::TVoid DrawPBar
        (
            const   TBitmap&                    bmpMask
            , const tCIDLib::TFloat4            f4Percent
            , const TArea&                      areaSrc
            , const TArea&                      areaTar
            , const tCIDGraphDev::EBltTypes     eBlt
            , const tCIDLib::EDirs              eDir
            , const TRGBClr&                    rgbStart
            , const TRGBClr&                    rgbEnd
            , const TRGBClr&                    rgbFill
            , const tCIDLib::TCard1             c1Opacity
        );

        virtual tCIDLib::TVoid DrawPolyLine
        (
            const   TPointArray&                pntaToDraw
            , const tCIDLib::TCard4             c4Count = kCIDLib::c4MaxCard
        );

        virtual tCIDLib::TVoid DrawPolyLine
        (
            const   TPointArray&                pntaToDraw
            , const TRGBClr&                    rgbClr
            , const tCIDLib::TCard4             c4Count = kCIDLib::c4MaxCard
        );

        virtual tCIDLib::TVoid DrawPolySegments
        (
            const   TPointArray&                pntaToDraw
            , const tCIDLib::TCard4             c4Count = kCIDLib::c4MaxCard
        );

        virtual tCIDLib::TVoid DrawPolySegments
        (
            const   TPointArray&                pntaToDraw
            , const TRGBClr&                    rgbClr
            , const tCIDLib::TCard4             c4Count = kCIDLib::c4MaxCard
        );

        virtual tCIDLib::TVoid DrawShadowText
        (
            const   TString&                    strText
            , const TArea&                      areaFormat
            , const TPoint&                     pntShadowOfs
            , const TRGBClr&                    rgbShadowClr
            , const tCIDLib::EHJustify          eHJustify = tCIDLib::EHJustify::Left
            , const tCIDLib::EVJustify          eVJustify = tCIDLib::EVJustify::Center
        );

        virtual tCIDLib::TVoid DrawString
        (
            const   TString&                    strText
            , const TPoint&                     pntAlign
        );

        virtual tCIDLib::TVoid DrawString
        (
            const   TString&                    strText
            , const tCIDLib::TCard4             c4StartInd
            , const TPoint&                     pntAlign
        );

        virtual tCIDLib::TVoid DrawString
        (
            const   TString&                    strText
            , const tCIDLib::TCard4             c4StartInd
            , const tCIDLib::TCard4             c4Len
            , const TPoint&                     pntAlign
        );

        virtual tCIDLib::TVoid DrawString
        (
            const   TString&                    strText
            , const TArea&                      areaFormat
            , const tCIDLib::EHJustify          eHJustify = tCIDLib::EHJustify::Left
            , const tCIDLib::EVJustify          eVJustify = tCIDLib::EVJustify::Center
            , const tCIDGraphDev::ETextFmts     eFormat = tCIDGraphDev::ETextFmts::None
            , const TRGBClr&                    rgbBgnFill = TRGBClr::Nul_TRGBClr()
        );

        virtual tCIDLib::TVoid DrawString
        (
            const   TString&                    strText
            , const tCIDLib::TCard4             c4StartInd
            , const tCIDLib::TCard4             c4Len
            , const TArea&                      areaFormat
            , const tCIDLib::EHJustify          eHJustify = tCIDLib::EHJustify::Left
            , const tCIDLib::EVJustify          eVJustify = tCIDLib::EVJustify::Center
            , const tCIDGraphDev::ETextFmts     eFormat = tCIDGraphDev::ETextFmts::None
            , const TRGBClr&                    rgbBgnFill = TRGBClr::Nul_TRGBClr()
        );

        virtual tCIDLib::TVoid DrawStringAlpha
        (
            const   TString&                    strText
            , const tCIDLib::TCard4             c4StartInd
            , const TPoint&                     pntAlign
            ,       TBitmap&                    bmpBuffer
            ,       TBitmap&                    bmpMask
            , const tCIDLib::TCard1             c1ConstAlpha
        );

        virtual tCIDLib::TVoid DrawStringAlpha
        (
            const   TString&                    strText
            , const TArea&                      areaFormat
            ,       TBitmap&                    bmpBuffer
            ,       TBitmap&                    bmpMask
            , const tCIDLib::EHJustify          eHJustify = tCIDLib::EHJustify::Left
            , const tCIDLib::EVJustify          eVJustify = tCIDLib::EVJustify::Center
            , const tCIDGraphDev::ETextFmts     eFormat = tCIDGraphDev::ETextFmts::None
            , const tCIDLib::TCard1             c1ConstAlpha = 0xFF
            , const TRGBClr&                    rgbBgnFill = TRGBClr::Nul_TRGBClr()
        );

        virtual tCIDLib::TVoid DrawStringAlpha
        (
            const   TString&                    strText
            , const tCIDLib::TCard4             c4StartInd
            , const tCIDLib::TCard4             c4Len
            , const TArea&                      areaFormat
            ,       TBitmap&                    bmpBuffer
            ,       TBitmap&                    bmpMask
            , const tCIDLib::EHJustify          eHJustify = tCIDLib::EHJustify::Left
            , const tCIDLib::EVJustify          eVJustify = tCIDLib::EVJustify::Center
            , const tCIDGraphDev::ETextFmts     eFormat = tCIDGraphDev::ETextFmts::None
            , const tCIDLib::TCard1             c1ConstAlpha = 0xFF
            , const TRGBClr&                    rgbBgnFill = TRGBClr::Nul_TRGBClr()
        );

        virtual tCIDLib::TVoid DrawStringFX
        (
            const   TString&                    strText
            , const TArea&                      areaDraw
            , const tCIDGraphDev::ETextFX      eEffect
            , const TRGBClr&                    rgbClr1
            , const TRGBClr&                    rgbClr2
            , const tCIDLib::EHJustify          eHJustify
            , const tCIDLib::EVJustify          eVJustify
            , const tCIDLib::TBoolean           bNoTextWrap
            , const TPoint&                     pntOffset
            ,       TBitmap&                    bmpBuffer
            ,       TBitmap&                    bmpMask
            ,       TPixelArray&                pixaBlur
            , const tCIDLib::TCard1             c1ConstAlpha = 0xFF
        );

        virtual tCIDGraphDev::EBackMixModes eBackMixMode() const;

        virtual tCIDGraphDev::EBackMixModes eBackMixMode
        (
            const   tCIDGraphDev::EBackMixModes eToSet
        );

        virtual tCIDGraphDev::EMixModes eMixMode() const;

        virtual tCIDGraphDev::EMixModes eMixMode
        (
            const   tCIDGraphDev::EMixModes     eToSet
        );

        virtual tCIDLib::TVoid EnableBoundsAccumulation();

        virtual tCIDLib::TVoid EndPath();

        virtual tCIDLib::TVoid Fill
        (
            const   TArea&                      areaToFill
            , const TGUIBrush&                  gbrToUse
        );

        virtual tCIDLib::TVoid Fill
        (
            const   TArea&                      areaToFill
            , const TRGBClr&                    rgbToUse
        );

        virtual tCIDLib::TVoid Fill
        (
            const   TGUIRegion&                 grgnToFill
            , const TRGBClr&                    rgbToUse
        );

        virtual tCIDLib::TVoid Fill
        (
            const   TPoint&                     pntFrom
            , const TPoint&                     pntTo
            , const TRGBClr&                    rgbToUse
        );

        virtual tCIDLib::TVoid FillPath();

        virtual tCIDLib::TVoid FillWithBmpPattern
        (
            const   TArea&                      areaToFill
            , const TBitmap&                    bmpToUse
            , const tCIDGraphDev::EBmpModes     eMode
        );

        virtual tCIDLib::TVoid FillWithBmpPattern
        (
            const   TPoint&                     pntFrom
            , const TPoint&                     pntTo
            , const TBitmap&                    bmpToUse
            , const tCIDGraphDev::EBmpModes     eMode
        );

        virtual tCIDLib::TVoid FillWithBmpPattern
        (
            const   TArea&                      areaToFill
            , const TBitmap&                    bmpToUse
            , const tCIDGraphDev::EBmpModes     eMode
            , const TPoint&                     pntPatOrg
        );

        virtual tCIDLib::TVoid FillWithBmpPattern
        (
            const   TPoint&                     pntFrom
            , const TPoint&                     pntTo
            , const TBitmap&                    bmpToUse
            , const tCIDGraphDev::EBmpModes     eMode
            , const TPoint&                     pntPatOrg
        );

        tCIDLib::TVoid Flush();

        virtual tCIDLib::TVoid Gradient3DFill
        (
            const   TArea&                      areaFill
            , const TRGBClr&                    rgbLight
            , const TRGBClr&                    rgbDark
        );

        virtual tCIDLib::TVoid Gradient3DFill2
        (
            const   TArea&                      areaFill
            , const TRGBClr&                    rgbFill
        );

        virtual tCIDLib::TVoid GradientFill
        (
            const   TArea&                      areaFill
            , const TRGBClr&                    rgbLeft
            , const TRGBClr&                    rgbRight
            , const tCIDGraphDev::EGradDirs     eDir = tCIDGraphDev::EGradDirs::Horizontal
        );

        virtual tCIDLib::TVoid GradientFill
        (
            const   TPoint&                     pntUL
            , const TPoint&                     pntLR
            , const TRGBClr&                    rgbLeft
            , const TRGBClr&                    rgbRight
            , const tCIDGraphDev::EGradDirs     eDir = tCIDGraphDev::EGradDirs::Horizontal
        );

        virtual tCIDLib::TVoid GradientTriangleFill
        (
            const   TPoint&                     pnt1
            , const TRGBClr&                    rgb1
            , const TPoint&                     pnt2
            , const TRGBClr&                    rgb2
            , const TPoint&                     pnt3
            , const TRGBClr&                    rgb3
        );

        virtual tCIDGraphDev::TFontHandle hfontSetFont
        (
            const   TGUIFont&                   gfontToSet
        );

        virtual tCIDGraphDev::TRegionHandle hrgnSetClipArea
        (
            const   tCIDGraphDev::EClipModes    eMode
            , const TArea&                      areaToSet
            , const tCIDLib::TCard4             c4Rounding
        );

        virtual tCIDGraphDev::TRegionHandle hrgnSetClipRegion
        (
            const   tCIDGraphDev::EClipModes    eMode
            , const TGUIRegion&                 hrgnToSet
        );

        virtual tCIDGraphDev::TPenHandle hpenSetPen
        (
                    TGUIPen&                    gpenNew
        );

        virtual tCIDLib::TVoid InvertArea
        (
            const   TArea&                      areaToInvert
        );

        virtual tCIDLib::TVoid MaskedAlphaBlit
        (
            const   TBitmap&                    bmpSrc
            , const TBitmap&                    bmpMask
            , const TArea&                      areaSource
            , const TPoint&                     pntTarget
            , const tCIDLib::TCard1             c1Alpha
            , const tCIDLib::TBoolean           bSrcAlpha
            , const tCIDLib::TCard4             c4TransClr
        );

        virtual tCIDLib::TVoid MaskedAlphaBlit
        (
            const   TBitmap&                    bmpSrc
            , const TBitmap&                    bmpMask
            , const TPoint&                     pntTarget
            , const tCIDLib::TCard1             c1Alpha
            , const tCIDLib::TBoolean           bSrcAlpha
            , const tCIDLib::TCard4             c4TransClr
        );

        virtual tCIDLib::TVoid MaskedAlphaBlit
        (
            const   TBitmap&                    bmpSrc
            , const TBitmap&                    bmpMask
            , const TArea&                      areaSource
            , const TArea&                      areaTarget
            , const tCIDLib::TCard1             c1Alpha
            , const tCIDLib::TBoolean           bSrcAlpha
            , const tCIDLib::TCard4             c4TransClr
        );

        virtual tCIDLib::TVoid MaskedAlphaBlit
        (
            const   TGraphDrawDev&              gdevSrc
            , const TBitmap&                    bmpMask
            , const TArea&                      areaSource
            , const TArea&                      areaTarget
            , const tCIDLib::TCard1             c1Alpha
            , const tCIDLib::TBoolean           bSrcAlpha
            , const tCIDLib::TCard4             c4TransClr
        );

        virtual tCIDLib::TVoid MoveRelative
        (
            const   TPoint&                     pntNewPos
        );

        virtual tCIDLib::TVoid MoveRelative
        (
            const   tCIDLib::TInt4              i4X
            , const tCIDLib::TInt4              i4Y
        );

        virtual tCIDLib::TVoid MoveTo
        (
            const   TPoint&                     pntNewPos
        );

        virtual tCIDLib::TVoid MoveTo
        (
            const   tCIDLib::TInt4              i4X
            , const tCIDLib::TInt4              i4Y
        );

        virtual TPoint pntCurPosition() const;

        virtual tCIDLib::TVoid PatternFill
        (
            const   TArea&                      areaToFill
            , const tCIDGraphDev::EBmpModes     eMode
        );

        virtual tCIDLib::TVoid PopClipArea
        (
                    tCIDGraphDev::TRegionHandle hrgnToPutBack
        );

        virtual tCIDLib::TVoid PopFont
        (
            const   TGUIFont&                   gfontCur
            ,       tCIDGraphDev::TFontHandle   hfontToPutBack
        );

        virtual tCIDLib::TVoid PopContext();

        virtual tCIDLib::TVoid PopPen
        (
                    TGUIPen&                    gpenCur
            ,       tCIDGraphDev::TPenHandle    hpenToPutBack
        );

        virtual tCIDLib::TVoid PushContext();

        virtual tCIDLib::TVoid QueryCharOffsets
        (
            const   TString&                    strSource
            , const tCIDLib::TCard4             c4Index
            ,       tCIDLib::TInt4&             i4Offset
            ,       tCIDLib::TCard4&            c4Width
        );

        virtual tCIDLib::TVoid QueryCharWidths
        (
            const   TString&                    strSource
            ,       tCIDLib::TCardList&         fcolToFill
        );

        virtual tCIDLib::TVoid QueryFontMetrics
        (
                    TFontMetrics&               fmtrToFill
        );

        virtual TRGBClr rgbAt
        (
            const   TPoint&                     pntAt
        )   const;

        virtual TRGBClr rgbBgnColor() const;

        virtual TRGBClr rgbTextColor() const;

        virtual tCIDLib::TVoid ResetBoundsArea();

        virtual TSize szPelsPerInch() const;

        virtual tCIDLib::TVoid ScrollBits
        (
            const   TArea&                      areaToScroll
            , const TArea&                      areaClip
            , const tCIDLib::TInt4              i4XOfs
            , const tCIDLib::TInt4              i4YOfs
            ,       TArea&                      areaUpdate
            , const tCIDLib::TBoolean           bWaitRetrace
        );

        virtual tCIDLib::TVoid SetBgnColor
        (
            const   TRGBClr&                    rgbToSet
        );

        virtual tCIDLib::TVoid SetClipPath
        (
            const   tCIDGraphDev::EClipModes    eMode
        );

        virtual tCIDLib::TVoid SetPixel
        (
            const   TPoint&                     pntAt
            , const TRGBClr&                    rgbToSet
        );

        virtual tCIDLib::TVoid SetTextColor
        (
            const   TRGBClr&                    rgbToSet
        );

        virtual tCIDLib::TVoid SetWorldTranslation
        (
            const   tCIDLib::TFloat4            f4XOfs
            , const tCIDLib::TFloat4            f4YOfs
        );

        virtual tCIDLib::TVoid Stroke
        (
            const   TArea&                      areaToStroke
        );

        virtual tCIDLib::TVoid Stroke
        (
            const   TArea&                      areaToStroke
            , const TRGBClr&                    rgbClr
            , const tCIDLib::TCard4             c4Rounding = 0
        );

        virtual tCIDLib::TVoid Stroke
        (
            const   TGUIRegion&                 grgnToStroke
            , const TRGBClr&                    rgbToUse
            , const tCIDLib::TCard4             c4Width = 1
        );

        virtual tCIDLib::TVoid StrokeAndFillPath();

        virtual tCIDLib::TVoid StrokePath();


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TGraphDrawDev();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid GradientCircleLR
        (
                    tCIDGraphDev::TDeviceHandle hdevTar
            , const TPoint&                     pntCenter
            , const tCIDLib::TCard4             c4Radius
            , const tCIDLib::TCard4             c4Light
            , const tCIDLib::TCard4             c4Dark
        );

        tCIDLib::TVoid GradientCircleUL
        (
                    tCIDGraphDev::TDeviceHandle hdevTar
            , const TPoint&                     pntCenter
            , const tCIDLib::TCard4             c4Radius
            , const tCIDLib::TCard4             c4Light
            , const tCIDLib::TCard4             c4Dark
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bAdvModeSet
        //      World transform settings require that the device be put into
        //      advanced mode. We use this to fault that in as needed, to avoid
        //      the overhead of doing it every time or doing it on devices that
        //      will never need it.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bAdvModeSet;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGraphDrawDev,TGraphicDevice)
};

#pragma CIDLIB_POPPACK

