//
// FILE NAME: CIDWUtils_ImgPreview.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/20/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  Implements the TImgPreviewWnd dialog.
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
#include    "CIDWUtils_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TImgPreviewWnd, TCtrlWnd)



// ---------------------------------------------------------------------------
//   CLASS: TImgPreviewWnd
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TImgPreviewWnd: Constructors and Destructor
// ---------------------------------------------------------------------------
TImgPreviewWnd::TImgPreviewWnd() :

    m_bmpChecker(facCIDCtrls(), kCIDCtrls::ridBmp_Checker)
    , m_c1Opacity(0xFF)
    , m_eBlit(tCIDGraphDev::EBltTypes::None)
    , m_ePlacement(tCIDGraphDev::EPlacement::Center)
    , m_eIPStyles(tCIDWUtils::EImgPStyles::None)
{
}

TImgPreviewWnd::~TImgPreviewWnd()
{
}


// ---------------------------------------------------------------------------
//  TImgPreviewWnd: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TImgPreviewWnd::InitFromDesc(const  TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDWUtils::EImgPStyles eIPStyles = tCIDWUtils::EImgPStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDLib::eOREnumBits(tCIDCtrls::EWndStyles::VisChild, tCIDCtrls::EWndStyles::Border)
    );

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Transparent))
        eIPStyles = tCIDLib::eOREnumBits(eIPStyles, tCIDWUtils::EImgPStyles::Transparent);

    CreateImgPreview
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , eStyles
        , eIPStyles
    );
}


tCIDLib::TVoid TImgPreviewWnd::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_Transparent);
}


TSize TImgPreviewWnd::szDefault() const
{
    TSize szInit(256, 128);
    TGUIFacility::AdjustDlgUnits(szInit);
    return szInit;
}


// ---------------------------------------------------------------------------
//  TImgPreviewWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Takes a point (assumed to be relative to this window, and indicates if it falls
//  within the actual displayed image area and if so, adjusts it so that it is a point
//  relative ot the image origin, i.e. the point within the image that was hit.
//
tCIDLib::TBoolean TImgPreviewWnd::bImagePnt(TPoint& pntToAdjust) const
{
    // If the passed point isn't within the image area, then return false
    if (!m_areaTar.bContainsPoint(pntToAdjust))
        return kCIDLib::False;

    pntToAdjust -= m_areaTar.pntOrg();
    return kCIDLib::True;
}


//
//  Convenience methods to set or clear the transparency value. if it's set
//  and we are valid, then we redraw. And we also have to update or clear
//  the mask bitmaps according to the new transparency.
//
tCIDLib::TBoolean TImgPreviewWnd::bTransparency() const
{
    return tCIDLib::bAnyBitsOn(m_eIPStyles, tCIDWUtils::EImgPStyles::Transparent);
}

tCIDLib::TBoolean TImgPreviewWnd::bTransparency(const tCIDLib::TBoolean bToSet)
{
    // Create the two masks or clear them, as required, and update the styles
    if (bToSet)
        m_eIPStyles = tCIDLib::eOREnumBits(m_eIPStyles, tCIDWUtils::EImgPStyles::Transparent);
     else
        m_eIPStyles = tCIDLib::eClearEnumBits(m_eIPStyles, tCIDWUtils::EImgPStyles::Transparent);

    if (bIsValid())
    {
        TGraphWndDev gdevTmp(*this);
        const TArea& areaCur = areaClient();
        ImageChanged(gdevTmp, areaCur.szArea());

        Redraw(areaClient(), tCIDCtrls::ERedrawFlags::ImmedErase);
    }
    return bToSet;
}


//
//  Get or set the opacity. If valid when set, we redraw to show the new
//  results.
//
tCIDLib::TCard1 TImgPreviewWnd::c1Opacity() const
{
    return m_c1Opacity;
}

tCIDLib::TCard1 TImgPreviewWnd::c1Opacity(const tCIDLib::TCard1 c1OpToSet)
{
    m_c1Opacity = c1OpToSet;

    // If we are currently created, then update
    if (bIsValid())
        Redraw(areaClient(), tCIDCtrls::ERedrawFlags::ImmedErase);

    return m_c1Opacity;
}



//
//  Set or get the transparency color. If the styles don't currently indicate
//  we are transparent, then we just store the value, else we need to update
//  the masks and redraw if valid.
//
tCIDLib::TCard4 TImgPreviewWnd::c4TransClr() const
{
    return m_c4TransClr;
}

tCIDLib::TCard4 TImgPreviewWnd::c4TransClr(const tCIDLib::TCard4 c4ToSet)
{
    m_c4TransClr = c4ToSet;

    if (tCIDLib::bAnyBitsOn(m_eIPStyles, tCIDWUtils::EImgPStyles::Transparent))
    {
        if (bIsValid())
        {
            TGraphWndDev gdevToUse(*this);
            const TArea& areaCur = areaClient();
            ImageChanged(gdevToUse, areaCur.szArea());
            Redraw(areaClient(), tCIDCtrls::ERedrawFlags::ImmedErase);
        }
    }
    return m_c4TransClr;
}


// Create this object's window
tCIDLib::TVoid
TImgPreviewWnd::CreateImgPreview(const  TWindow&                wndParent
                                , const tCIDCtrls::TWndId       widThis
                                , const TArea&                  areaAt
                                , const tCIDCtrls::EWndStyles   eStyles
                                , const tCIDWUtils::EImgPStyles eIPStyles
                                , const tCIDCtrls::EExWndStyles eExStyles)
{
    // Store our class specific styles
    m_eIPStyles = eIPStyles;

    CreateWnd
    (
        wndParent.hwndThis()
        , kCIDCtrls::pszCustClassHV
        , kCIDLib::pszEmptyZStr
        , areaAt
        , eStyles
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );
}


//
//  Set or get the placement. If setting and we are valid, we redraw to show
//  the new placement.
//
tCIDGraphDev::EPlacement TImgPreviewWnd::ePlacement() const
{
    return m_ePlacement;
}

tCIDGraphDev::EPlacement
TImgPreviewWnd::ePlacement(const tCIDGraphDev::EPlacement eToSet)
{
    m_ePlacement = eToSet;
    if (bIsValid())
    {
        const TArea& areaCur = areaClient();
        PlacementChanged(areaCur.szArea());
        Redraw(areaCur, tCIDCtrls::ERedrawFlags::ImmedErase);
    }
    return m_ePlacement;
}


tCIDLib::TVoid TImgPreviewWnd::Reset(const tCIDGraphDev::EPlacement ePlacement)
{
    // If the passed placement isn't the _count value, take it
    if (ePlacement != tCIDGraphDev::EPlacement::Count)
        m_ePlacement = ePlacement;

    m_bmpImage.Reset();
    m_bmpMask.Reset();
    m_bmpPreMasked.Reset();
    m_c1Opacity = 0xFF;
    m_c4TransClr = 0;

    // If we are currently created, then update
    if (bIsValid())
        Redraw(areaClient(), tCIDCtrls::ERedrawFlags::ImmedErase);
}


//
//  Set a new bitmap, or set a new bitmap along with transparency information. If
//  either transparent or setting transparency, we have to update the masks. In
//  either case, if valid, we redraw to show the new bitmap.
//
tCIDLib::TVoid TImgPreviewWnd::SetBitmap(const  TBitmap&        bmpToSet
                                        , const tCIDLib::TCard1 c1Opacity)
{
    m_eIPStyles = tCIDLib::eClearEnumBits(m_eIPStyles, tCIDWUtils::EImgPStyles::Transparent);
    m_c1Opacity = c1Opacity;
    m_c4TransClr = 0;
    m_bmpImage = bmpToSet;

    // Make our own copy so we don't affect the original
    m_bmpImage.DeepCopy();

    // If we are currently created, then update
    if (bIsValid())
    {
        TGraphWndDev gdevTmp(*this);
        const TArea& areaCur = areaClient();
        ImageChanged(gdevTmp, areaCur.szArea());
        Redraw(areaCur, tCIDCtrls::ERedrawFlags::ImmedErase);
    }
}

tCIDLib::TVoid
TImgPreviewWnd::SetBitmap(  const   TBitmap&            bmpToSet
                            , const tCIDLib::TCard4     c4TransClr
                            , const tCIDLib::TBoolean   bTransClr
                            , const tCIDLib::TCard1     c1OpToSet)
{
    if (bTransClr)
    {
        m_eIPStyles = tCIDLib::eOREnumBits(m_eIPStyles, tCIDWUtils::EImgPStyles::Transparent);
        m_c4TransClr = c4TransClr;
    }
     else
    {
        m_eIPStyles = tCIDLib::eClearEnumBits(m_eIPStyles, tCIDWUtils::EImgPStyles::Transparent);
        m_c4TransClr = 0;
    }
    m_c1Opacity = c1OpToSet;
    m_bmpImage = bmpToSet;

    // Make our own copy so we don't affect the original
    m_bmpImage.DeepCopy();

    if (bIsValid())
    {
        TGraphWndDev gdevTmp(*this);
        const TArea& areaCur = areaClient();
        ImageChanged(gdevTmp, areaCur.szArea());
        Redraw(areaCur, tCIDCtrls::ERedrawFlags::ImmedErase);
    }
}



// ---------------------------------------------------------------------------
//  TImgPreviewWnd: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TImgPreviewWnd::AreaChanged(const   TArea&                  areaPrev
                            , const TArea&                  areaNew
                            , const tCIDCtrls::EPosStates   ePosState
                            , const tCIDLib::TBoolean       bOrgChanged
                            , const tCIDLib::TBoolean       bSizeChanged
                            , const tCIDLib::TBoolean       bStateChanged)
{
    // Call our parent first
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    // If we are restored, keep our image centered
    if ((ePosState != tCIDCtrls::EPosStates::Minimized) && bSizeChanged)
    {
        // Remember the current target area
        TArea areaOrg = m_areaTar;

        // Update the placement info
        PlacementChanged(areaClient().szArea());

        // Build an area that covers both of them
        TArea areaRefresh(m_areaTar);
        areaRefresh |= areaOrg;

        // And redraw that
        Redraw(areaRefresh, tCIDCtrls::ERedrawFlags::Immediate);
    }
}


tCIDLib::TBoolean TImgPreviewWnd::bCreated()
{
    TParent::bCreated();

    // Do an initial setup of our image data
    TGraphWndDev gdevToUse(*this);
    ImageChanged(gdevToUse, areaClient().szArea());

    return kCIDLib::True;
}


//
//  We always fill our entire area during the paint, so we disable this to avoid blinkies.
//
tCIDLib::TBoolean TImgPreviewWnd::bEraseBgn(TGraphDrawDev&)
{
    return kCIDLib::True;
}


//
//  We don't call our parent to do standard stuff since we are never transparent and
//  our content always completely fills us, so there's no need for that stuff.
//
tCIDLib::TBoolean
TImgPreviewWnd::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    // Get the client area, inside any border
    TArea areaDraw = areaClient();

    // Do a pattern fill of the checker
    gdevToUse.FillWithBmpPattern(areaDraw, m_bmpChecker, tCIDGraphDev::EBmpModes::PatCopy);

    // If our image drawing area intersects the update area, then do it
    if (areaDraw.bIntersects(areaUpdate))
    {
        const tCIDLib::TBoolean bTransparent
        (
            bTransparency() && !m_bmpImage.bHasAlpha()
        );

        //
        //  If it's transparent and not alpha based, then we draw the pre-
        //  masked version. Otherwise, we draw the original image.
        //
        if (bTransparent)
        {
            m_bmpPreMasked.AdvDrawOn
            (
                gdevToUse
                , m_areaSrc
                , m_areaTar
                , kCIDLib::True
                , m_c4TransClr
                , m_bmpMask
                , m_eBlit
                , bIsEnabled() ? m_c1Opacity : 0x60
            );
        }
         else
        {
            m_bmpImage.AdvDrawOn
            (
                gdevToUse
                , m_areaSrc
                , m_areaTar
                , kCIDLib::False
                , 0
                , m_bmpMask
                , m_eBlit
                , bIsEnabled() ? m_c1Opacity : 0x60
            );
        }
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TImgPreviewWnd: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TImgPreviewWnd::ImageChanged(TGraphDrawDev& gdevToUse, const TSize& szNew)
{
    // In case the size of the image changed, do placement calcs
    PlacementChanged(szNew);

    if (bTransparency())
    {
        // Create the transparency mask
        m_bmpMask = m_bmpImage.bmpMakeMask(gdevToUse, m_c4TransClr, kCIDLib::False);

        // And create the temporary inverted mask and pre-mask the image
        TBitmap bmpInvMask(m_bmpImage.bmpMakeMask(gdevToUse, m_c4TransClr, kCIDLib::True));
        m_bmpPreMasked = m_bmpImage;
        m_bmpPreMasked.DeepCopy();
        m_bmpPreMasked.MaskWith(gdevToUse, bmpInvMask);
    }
     else
    {
        // Nothing special, so clear the transparency masks
        m_bmpMask.Reset();
        m_bmpPreMasked.Reset();
    }

    ForceRepaint();
}


tCIDLib::TVoid TImgPreviewWnd::PlacementChanged(const TSize& szNew)
{
    //
    //  Calculate the size and area where we'll display the image. We pre-
    //  calc these so that we can do a very efficient paint.
    //
    TArea areaDraw(TPoint::pntOrigin, szNew);
    if (tCIDLib::bAllBitsOn(eWndStyles(), tCIDCtrls::EWndStyles::Border))
        areaDraw.Deflate(1);

    m_areaSrc.Set(TPoint::pntOrigin, m_bmpImage.szBitmap());
    m_eBlit = facCIDGraphDev().eCalcPlacement
    (
        areaDraw, m_areaSrc, m_areaTar, m_ePlacement, kCIDLib::False
    );

    ForceRepaint();
}

