//
// FILE NAME: CIDCtrl2_StaticImage.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/10/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is a static image widget that displays a bitmap set on it.
//
// CAVEATS/GOTCHAS:
//
//  1)  We assume modern times here, and that it will be a 24 or 32 bit image,
//      and that we will alpha blit it.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TStaticImg
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TStaticImg : public TCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStaticImg();

        TStaticImg(const TStaticImg&) = delete;

        ~TStaticImg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStaticImg& operator=(const TStaticImg&) = delete;


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


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTransparency() const;

        tCIDLib::TBoolean bTransparency
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TCard1 c1Opacity() const;

        tCIDLib::TCard1 c1Opacity
        (
            const   tCIDLib::TCard1         c1ToSet
        );

        tCIDLib::TCard4 c4TransClr() const;

        tCIDLib::TCard4 c4TransClr
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid CreateStaticImg
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDGraphDev::EPlacement eToSet
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::ESImgStyles  eImgStyles = tCIDCtrls::ESImgStyles::None
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDGraphDev::EPlacement ePlacement() const;

        tCIDGraphDev::EPlacement ePlacement
        (
            const   tCIDGraphDev::EPlacement eToSet
        );

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid SetAppImage
        (
            const   TString&                strImageName
            , const tCIDCtrls::EAppImgSz    eSize
        );

        tCIDLib::TVoid SetFromBmpFile
        (
            const   TPathStr&               pathToLoad
        );

        tCIDLib::TVoid SetBitmap
        (
            const   TBitmap&                bmpToSet
        );

        tCIDLib::TVoid SetImage
        (
            const   TGraphicDevice&         gdevCompat
            , const TCIDImage&              imgiToSet
        );


    protected :
        // -------------------------------------------------------------------
        //  Public, virtual methods
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

        tCIDLib::TBoolean bClick
        (
            const   tCIDCtrls::EMouseButts  eButton
            , const tCIDCtrls::EMouseClicks eClickType
            , const TPoint&                 pntAt
            , const tCIDLib::TBoolean       bCtrlShift
            , const tCIDLib::TBoolean       bShift
        )   override;

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


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bCleanup
        //      If we set any images on the control ourself, then the control
        //      will stop cleaning up after us. This means that, upon destroy
        //      we have to delete whatever bitmap is currently set on the
        //      control.
        //
        //  m_bmpSrc
        //      The bitmap we are displaying
        //
        //  m_c1Opacity
        //      The global opacity to use when we draw.
        //
        //  m_ePlacement
        //      The placement type we use to draw the image.
        //
        //  m_eSImgStyles
        //      Our class specific styles
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bCleanup;
        TBitmap                     m_bmpMask;
        TBitmap                     m_bmpImage;
        TBitmap                     m_bmpPreMasked;
        tCIDLib::TCard1             m_c1Opacity;
        tCIDLib::TCard4             m_c4TransClr;
        tCIDGraphDev::EPlacement    m_ePlacement;
        tCIDCtrls::ESImgStyles      m_eSImgStyles;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStaticImg, TCtrlWnd)
};

#pragma CIDLIB_POPPACK


