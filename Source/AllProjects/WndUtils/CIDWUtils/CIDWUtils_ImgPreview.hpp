//
// FILE NAME: CIDWUtils_ImgPreview_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/20/2015
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
//  This is the header for the CIDWUtils_ImgPreview.cpp file, which provides a
//  standard control for doing image preview. To deal with transparent images, it
//  sets a checkboard background and displays the image over that. That means we
//  have to provide the compositing ourself.
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
//   CLASS: TImgPreviewWnd
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class CIDWUTILSEXP TImgPreviewWnd : public TCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TImgPreviewWnd();

        TImgPreviewWnd(const TImgPreviewWnd&) = delete;

        ~TImgPreviewWnd();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid InitFromDesc
        (
            const   TWindow&                wndParent
            , const TDlgItem&               dlgiSrc
            , const tCIDCtrls::EWndThemes   eTheme
        )   override;

        tCIDLib::TVoid QueryHints
        (
                    tCIDLib::TStrCollect&   colToFill
        )   const override;

        TSize szDefault() const override;


        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TImgPreviewWnd& operator=(const TImgPreviewWnd&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bImagePnt
        (
                    TPoint&                 pntToAdjust
        )   const;

        tCIDLib::TBoolean bTransparency() const;

        tCIDLib::TBoolean bTransparency
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TCard1 c1Opacity() const;

        tCIDLib::TCard1 c1Opacity
        (
            const   tCIDLib::TCard1         c1Opacity
        );

        tCIDLib::TCard4 c4TransClr() const;

        tCIDLib::TCard4 c4TransClr
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid CreateImgPreview
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaAt
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDWUtils::EImgPStyles eIPStyles
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDGraphDev::EPlacement ePlacement() const;

        tCIDGraphDev::EPlacement ePlacement
        (
            const   tCIDGraphDev::EPlacement eToSet
        );

        tCIDLib::TVoid Reset
        (
            const   tCIDGraphDev::EPlacement ePlacement = tCIDGraphDev::EPlacement::Count
        );

        tCIDLib::TVoid SetBitmap
        (
            const   TBitmap&                bmpToSet
            , const tCIDLib::TCard1         c1Opacity = 0xFF
        );

        tCIDLib::TVoid SetBitmap
        (
            const   TBitmap&                bmpToSet
            , const tCIDLib::TCard4         c4TransClr
            , const tCIDLib::TBoolean       bTransClr
            , const tCIDLib::TCard1         c1Opacity = 0xFF
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AreaChanged
        (
            const   TArea&                  areaPrev
            , const TArea&                  areaNew
            , const tCIDCtrls::EPosStates   ePosState
            , const tCIDLib::TBoolean       bOrgChanged
            , const tCIDLib::TBoolean       bSizeChanged
            , const tCIDLib::TBoolean       bStateChanged
        )   override;

        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TBoolean bEraseBgn
        (
                    TGraphDrawDev&          gdevToUse
        )   override;

        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ImageChanged
        (
                    TGraphDrawDev&          gdevToUse
            , const TSize&                  szNew
        );

        tCIDLib::TVoid PlacementChanged
        (
            const   TSize&                  szNew
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaSrc
        //  m_areaTar
        //      We pre-scale the image and the source/target areas that will be
        //      used for dipslay. This makes things a lot more efficient.
        //
        //  m_bmpChecker
        //      Our checkerboard background. We draw it tiled to fill our area
        //      (inside any border.)
        //
        //  m_bmpDraw
        //      This is our compositing image. We keep it sized to our size.
        //
        //  m_bmpImg
        //      The currently set image. It's at it's actual size, and scale it
        //      to fit based on the set placement type. Initially we just set it to
        //      a blank 1x1 image.
        //
        //  m_bmpMask
        //      The mask mask for the image, if the image is color transparent.
        //      It has to be passed in for drawing. See m_bmpPreMasked.
        //
        //  m_bmpPreMasked
        //      We have to pre-mask color transparent images, but we can't
        //      modify ones we get, and we might have to switch back to non
        //      transparent if the style changes, so we couldn't change the
        //      original even if it's one we loaded. So we create a pre-masked
        //      version and draw it when color transparent.
        //
        //  m_ePlacement
        //      The client code can set a placement type on us which we use to
        //      display the image.
        //
        //  m_eIPStyles
        //      Our class specific styles.
        // -------------------------------------------------------------------
        TArea                       m_areaSrc;
        TArea                       m_areaTar;
        TBitmap                     m_bmpChecker;
        TBitmap                     m_bmpDraw;
        TBitmap                     m_bmpImage;
        TBitmap                     m_bmpMask;
        TBitmap                     m_bmpPreMasked;
        tCIDLib::TCard1             m_c1Opacity;
        tCIDLib::TCard4             m_c4TransClr;
        tCIDGraphDev::EBltTypes     m_eBlit;
        tCIDGraphDev::EPlacement    m_ePlacement;
        tCIDWUtils::EImgPStyles     m_eIPStyles;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TImgPreviewWnd,TCtrlWnd)
};

#pragma CIDLIB_POPPACK




