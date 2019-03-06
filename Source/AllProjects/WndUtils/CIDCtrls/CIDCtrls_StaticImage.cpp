//
// FILE NAME: CIDCtrls_StaticImage.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/07/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the static image widget.
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
#include    "CIDCtrls_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TStaticImg, TStdCtrlWnd)



// ---------------------------------------------------------------------------
//  CLASS: TStaticImg
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TStaticImg: Constructors and Destructor
// ---------------------------------------------------------------------------
TStaticImg::TStaticImg() :

    TCtrlWnd()
    , m_bCleanup(kCIDLib::False)
    , m_c1Opacity(0xFF)
    , m_c4TransClr(0)
    , m_ePlacement(tCIDGraphDev::EPlacement::Center)
    , m_eSImgStyles(tCIDCtrls::ESImgStyles::None)
{
}

TStaticImg::~TStaticImg()
{
}


// ---------------------------------------------------------------------------
// TStaticImg: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TStaticImg::InitFromDesc(const  TWindow&                wndParent
                        , const TDlgItem&               dlgiSrc
                        , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::ESImgStyles eSImgStyles = tCIDCtrls::ESImgStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles::VisChild;
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_ReportClicks))
        eSImgStyles |= tCIDCtrls::ESImgStyles::ReportClicks;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Transparent))
        eSImgStyles |= tCIDCtrls::ESImgStyles::Transparent;

    if (eTheme == tCIDCtrls::EWndThemes::DialogBox)
    {
        // Set a sunken border and ctrl fill background if asked to
        if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Border))
        {
            eExStyles |= tCIDCtrls::EExWndStyles::StaticEdge;
            SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::DlgCtrlFill), kCIDLib::True);
        }
    }
     else if (eTheme == tCIDCtrls::EWndThemes::MainWnd)
    {
        //  Set a line border
        if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Border))
            eStyles |= tCIDCtrls::EWndStyles::Border;
    }

    CreateStaticImg
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , tCIDGraphDev::EPlacement::Center
        , eStyles
        , eSImgStyles
        , eExStyles
    );

    //
    //  If there's an app image name, then load it. It can either be just the name
    //  or the name prefixed by Lrg:, Med:, or Sml: If none, then default.
    //
    if (!dlgiSrc.strImage().bIsEmpty())
    {
        TString strToLoad;
        const tCIDCtrls::EAppImgSz eSz = TDlgItem::eParseAppImage
        (
            dlgiSrc.strImage(), strToLoad
        );
        if (!strToLoad.bIsEmpty())
            SetAppImage(strToLoad, eSz);
    }
}


tCIDLib::TVoid TStaticImg::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_Border);
    colToFill.objAdd(kCIDCtrls::strHint_ReportClicks);
    colToFill.objAdd(kCIDCtrls::strHint_Transparent);
}


// ---------------------------------------------------------------------------
// TStaticImg: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Convenience methods to set or clear the transparency value. if it's set
//  and we are valid, then we redraw. And we also have to update or clear
//  the mask bitmaps according to the new transparency.
//
tCIDLib::TBoolean TStaticImg::bTransparency() const
{
    return tCIDLib::bAnyBitsOn(m_eSImgStyles, tCIDCtrls::ESImgStyles::Transparent);
}

tCIDLib::TBoolean TStaticImg::bTransparency(const tCIDLib::TBoolean bToSet)
{
    // Create the two masks or clear them, as required, and update the styles
    if (bToSet)
        m_eSImgStyles |= tCIDCtrls::ESImgStyles::Transparent;
     else
        m_eSImgStyles |= tCIDCtrls::ESImgStyles::Transparent;

    if (bIsValid())
    {
        TGraphWndDev gdevTmp(*this);
        ImageChanged(gdevTmp, areaClient().szArea());
        Redraw(tCIDCtrls::ERedrawFlags::ImmedErase);
    }
    return bToSet;
}


tCIDLib::TCard1 TStaticImg::c1Opacity() const
{
    return m_c1Opacity;
}

tCIDLib::TCard1 TStaticImg::c1Opacity(const tCIDLib::TCard1 c1ToSet)
{
    if (c1ToSet != m_c1Opacity)
    {
        m_c1Opacity = c1ToSet;
        Redraw(tCIDCtrls::ERedrawFlags::ImmedErase);
    }
    return m_c1Opacity;
}


//
//  Set or get the transparency color. If the styles don't currently indicate
//  we are transparent, then we just store the value, else we need to update
//  the masks and redraw if valid.
//
tCIDLib::TCard4 TStaticImg::c4TransClr() const
{
    return m_c4TransClr;
}

tCIDLib::TCard4 TStaticImg::c4TransClr(const tCIDLib::TCard4 c4ToSet)
{
    m_c4TransClr = c4ToSet;

    if (tCIDLib::bAnyBitsOn(m_eSImgStyles, tCIDCtrls::ESImgStyles::Transparent))
    {
        if (bIsValid())
        {
            TGraphWndDev gdevToUse(*this);
            ImageChanged(gdevToUse, areaClient().szArea());
            Redraw(tCIDCtrls::ERedrawFlags::ImmedErase);
        }
    }
    return m_c4TransClr;
}



tCIDLib::TVoid
TStaticImg::CreateStaticImg(const   TWindow&                wndParent
                            , const tCIDCtrls::TWndId       widThis
                            , const TArea&                  areaInit
                            , const tCIDGraphDev::EPlacement ePlacement
                            , const tCIDCtrls::EWndStyles   eStyles
                            , const tCIDCtrls::ESImgStyles  eImgStyles
                            , const tCIDCtrls::EExWndStyles eExStyles)
{
    m_ePlacement = ePlacement;

    // Make sure tab stop and group are off, since that will cause issues
    tCIDCtrls::EWndStyles eRealStyles = tCIDCtrls::EWndStyles(eStyles);
    eRealStyles = tCIDLib::eClearEnumBits(eRealStyles, tCIDCtrls::EWndStyles::TabGroup);

    // Store our class specific styles
    m_eSImgStyles = eImgStyles;

    // And create the cnotrol
    CreateWnd
    (
        wndParent.hwndThis()
        , kCIDCtrls::pszCustClass
        , kCIDLib::pszEmptyZStr
        , areaInit
        , eRealStyles
        , eExStyles
        , widThis
    );
}


tCIDGraphDev::EPlacement TStaticImg::ePlacement() const
{
    return m_ePlacement;
}

tCIDGraphDev::EPlacement
TStaticImg::ePlacement(const tCIDGraphDev::EPlacement eToSet)
{
    if (eToSet != m_ePlacement)
    {
        m_ePlacement = eToSet;
        Redraw(tCIDCtrls::ERedrawFlags::ImmedErase);
    }
    return m_ePlacement;
}


// Reset us to an empty image
tCIDLib::TVoid TStaticImg::Reset()
{
    m_c1Opacity = 0xFF;
    m_bmpImage.Reset();
    Redraw(tCIDCtrls::ERedrawFlags::ImmedErase);
}


//
//  Load an app image to bitmap and set it on us
//
tCIDLib::TVoid
TStaticImg::SetAppImage(const   TString&                strImageName
                        , const tCIDCtrls::EAppImgSz    eSize)
{
    m_bmpImage = facCIDCtrls().bmpLoadAppImg(strImageName, eSize);
    Redraw(tCIDCtrls::ERedrawFlags::ImmedErase);
}


//
//  Load up a bitmap image from a file and load it as our current image.
//
tCIDLib::TVoid TStaticImg::SetFromBmpFile(const TPathStr& pathToLoad)
{
    HBITMAP hbmpNew = (HBITMAP)::LoadImage
    (
        NULL
        , pathToLoad.pszBuffer()
        , IMAGE_BITMAP
        , 0
        , 0
        , LR_LOADFROMFILE | LR_SHARED
    );

    if (hbmpNew != NULL)
        m_bmpImage = TBitmap(hbmpNew, tCIDLib::EAdoptOpts::Adopt);
    else
        m_bmpImage.Reset();

    Redraw(tCIDCtrls::ERedrawFlags::ImmedErase);
}


//
//  Set the caller's already loaded bitmap as our image
//
tCIDLib::TVoid TStaticImg::SetBitmap(const TBitmap& bmpNew)
{
    m_bmpImage = bmpNew;
    m_bmpImage.DeepCopy();
    Redraw(tCIDCtrls::ERedrawFlags::ImmedErase);
}


// Set our image from any of the standard image info derivatives
tCIDLib::TVoid
TStaticImg::SetImage(const TGraphicDevice& gdevCompat, const TCIDImage& imgiNew)
{
    m_bmpImage = TBitmap(gdevCompat, imgiNew);
    Redraw(tCIDCtrls::ERedrawFlags::ImmedErase);
}


// ---------------------------------------------------------------------------
// TStaticImg: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TStaticImg::AreaChanged(const   TArea&                  areaPrev
                        , const TArea&                  areaNew
                        , const tCIDCtrls::EPosStates   ePosState
                        , const tCIDLib::TBoolean       bOrgChanged
                        , const tCIDLib::TBoolean       bSizeChanged
                        , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    // If the size changed, then we need to redraw
    if (bSizeChanged)
        Redraw(tCIDCtrls::ERedrawFlags::ImmedErase);
}


//
//  We have to report clicks to our parent if our style indicates so. So we override
//  this to watch for those clicks.
//
tCIDLib::TBoolean
TStaticImg::bClick( const  tCIDCtrls::EMouseButts  eButton
                    , const tCIDCtrls::EMouseClicks eClick
                    , const TPoint&
                    , const tCIDLib::TBoolean
                    , const tCIDLib::TBoolean)

{
    //
    //  If a left down or left double and we have the report clicks style , then
    //  let's report it.
    //
    if (tCIDLib::bAllBitsOn(m_eSImgStyles, tCIDCtrls::ESImgStyles::ReportClicks)
    &&  ((eClick == tCIDCtrls::EMouseClicks::Down)
    ||   (eClick == tCIDCtrls::EMouseClicks::Double))
    &&  (eButton == tCIDCtrls::EMouseButts::Left))
    {
        PassStaticChildClick(eButton, eClick);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// Suppress this
tCIDLib::TBoolean TStaticImg::bEraseBgn(TGraphDrawDev&)
{
    return kCIDLib::True;
}


tCIDLib::TBoolean
TStaticImg::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    // Set up the source and target areas
    TArea areaSrc(TPoint::pntOrigin, m_bmpImage.szBitmap());
    TArea areaTar(areaClient());

    const tCIDLib::TBoolean bTransparent
    (
        bTransparency() && !m_bmpImage.bHasAlpha()
    );

    // Calculate the correct actual target based on placement
    TArea areaDest;
    tCIDGraphDev::EBltTypes eBltType = facCIDGraphDev().eCalcPlacement
    (
        areaTar
        , areaSrc
        , areaDest
        , m_ePlacement
        , kCIDLib::False
    );

    // Do a background fill
    gdevToUse.Fill(areaTar, gbrBgn());

    if (bTransparent)
    {
        m_bmpPreMasked.AdvDrawOn
        (
            gdevToUse
            , areaSrc
            , areaDest
            , kCIDLib::True
            , m_c4TransClr
            , m_bmpMask
            , eBltType
            , bIsEnabled() ? m_c1Opacity : 0x60
        );
    }
     else
    {
        // And let's draw it
        m_bmpImage.AdvDrawOn
        (
            gdevToUse
            , areaSrc
            , areaDest
            , kCIDLib::False
            , 0
            , m_bmpMask
            , eBltType
            , bIsEnabled() ? m_c1Opacity : 0x60
        );
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
// TStaticImg: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TStaticImg::ImageChanged(TGraphDrawDev& gdevToUse, const TSize& szNew)
{
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
}
