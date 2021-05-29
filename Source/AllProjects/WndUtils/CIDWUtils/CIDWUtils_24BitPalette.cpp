//
// FILE NAME: CIDWUtils_24BitPalette.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/28/2002
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
//  This file implements the 24 bit palette window.
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
RTTIDecls(T24BPalChangeInfo,TCtrlNotify)
AdvRTTIDecls(T24BitPalette,TCtrlWnd)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDWUtils_24BitPalette
    {
        constexpr tCIDLib::TCard4   c4BorderWidth       = 2;
        constexpr tCIDLib::TCard4   c4AreaBorderWidth   = 2;
    }
}



// ---------------------------------------------------------------------------
//   CLASS: T24BPalChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  T24BPalChangeInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
T24BPalChangeInfo::T24BPalChangeInfo(const  TRGBClr&    rgbColor
                                    , const TWindow&    wndSrc) :
    TCtrlNotify(wndSrc)
    , m_rgbColor(rgbColor)
{
}

T24BPalChangeInfo::~T24BPalChangeInfo()
{
}


// ---------------------------------------------------------------------------
//  T24BPalChangeInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TRGBClr& T24BPalChangeInfo::rgbNewColor() const
{
    return m_rgbColor;
}





// ---------------------------------------------------------------------------
//   CLASS: T24BitPalette
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  T24BitPalette: Public static data
// ---------------------------------------------------------------------------
const TNotificationId T24BitPalette::nidChange(L"24BitPaletteChangeEvent");


// ---------------------------------------------------------------------------
//  T24BitPalette: Constructors and Destructor
// ---------------------------------------------------------------------------

// We just use a default initial bitmap
T24BitPalette::T24BitPalette() :

    m_bmpPal(tCIDGraphDev::ESysBmps::Close)
    , m_eTracking(ETrackModes::None)
    , m_hsvCur(180, 0.5, 0.5)
    , m_bmpValSel(facCIDCtrls().bmpLoadSmallAppImg(L"Right Arrow"))
{
    m_szValSel = m_bmpValSel.szBitmap();
}

T24BitPalette::T24BitPalette(const TRGBClr& rgbInit) :

    m_bmpPal(tCIDGraphDev::ESysBmps::Close)
    , m_eTracking(ETrackModes::None)
    , m_hsvCur(rgbInit)
    , m_bmpValSel(facCIDCtrls().bmpLoadSmallAppImg(L"Right Arrow"))
{
    m_szValSel = m_bmpValSel.szBitmap();
}

T24BitPalette::~T24BitPalette()
{
}


// ---------------------------------------------------------------------------
//  T24BitPalette: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
T24BitPalette::InitFromDesc(const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles::VisChild;

    // Create the window based on the incoming info
    Create
    (
        wndParent
        , eStyles
        , dlgiSrc.areaPos()
        , dlgiSrc.ridChild()
    );
}


tCIDLib::TVoid T24BitPalette::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
}



// ---------------------------------------------------------------------------
//  T24BitPalette: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
T24BitPalette::Create(  const   TWindow&                wndParent
                        , const tCIDCtrls::EWndStyles   eWndStyles
                        , const TArea&                  areaInit
                        , const tCIDCtrls::TWndId       widToUse)
{
    CreateWnd
    (
        wndParent.hwndSafe()
        , kCIDCtrls::pszCustClassHV
        , L""
        , areaInit
        , eWndStyles
        , tCIDCtrls::EExWndStyles::None
        , widToUse
    );
}


TRGBClr T24BitPalette::rgbCurrent() const
{
    TRGBClr rgbRet;
    m_hsvCur.ToRGB(rgbRet);
    return rgbRet;
}


tCIDLib::TVoid T24BitPalette::SetColor(const TRGBClr& rgbToSet)
{
    //
    //  To avoid possible circular freakouts, don't do anything if its
    //  the same as the current value.
    //
    THSVClr hsvTmp(rgbToSet);
    if (hsvTmp == m_hsvCur)
        return;

    TGraphWndDev gdevTarget(*this);
    RemoveSelector(gdevTarget);
    DrawValueBarPtr(gdevTarget, m_hsvCur, kCIDLib::False);
    m_hsvCur = hsvTmp;
    DrawSelector(gdevTarget);
    DrawValueBar(gdevTarget);
    SendChangeNotification(rgbToSet);
}


// ---------------------------------------------------------------------------
//  T24BitPalette: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
T24BitPalette::AreaChanged( const   TArea&                  areaPrev
                            , const TArea&                  areaNew
                            , const tCIDCtrls::EPosStates   ePosState
                            , const tCIDLib::TBoolean       bOrgChanged
                            , const tCIDLib::TBoolean       bSizeChanged
                            , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    // Update our area calculations if appropriate
    if ((ePosState != tCIDCtrls::EPosStates::Minimized) && bSizeChanged)
        CalcAreas(areaNew.szArea());
}


tCIDLib::TBoolean
T24BitPalette::bClick(  const   tCIDCtrls::EMouseButts  eButton
                        , const tCIDCtrls::EMouseClicks eClickType
                        , const TPoint&                 pntAt
                        , const tCIDLib::TBoolean
                        , const tCIDLib::TBoolean       )
{
    if (eButton == tCIDCtrls::EMouseButts::Left)
    {
        if (eClickType == tCIDCtrls::EMouseClicks::Down)
        {
            if (m_areaPal.bContainsPoint(pntAt))
            {
                // Remove the current selection
                TGraphWndDev gdevTarget(*this);
                RemoveSelector(gdevTarget);

                // Set the tracking color and the tracking flag
                PointToColor(pntAt, m_hsvTracking);
                m_eTracking   = ETrackModes::Palette;

                // And send an initial change for this color
                TRGBClr rgbTmp;
                m_hsvTracking.ToRGB(rgbTmp);
                SendChangeNotification(rgbTmp);
            }
             else if (m_areaValueBar.bContainsPoint(pntAt))
            {
                // Indicate that we are tracking the value
                m_eTracking = ETrackModes::Value;

                // Set the tracking color to the current color
                m_hsvTracking = m_hsvCur;

                // Move the value selector to indicate this new value.
                TGraphWndDev gdevTarget(*this);
                DrawValueBarPtr(gdevTarget, m_hsvTracking, kCIDLib::False);
                m_hsvTracking.f4Value(f4ValueFromPoint(pntAt));
                DrawValueBarPtr(gdevTarget, m_hsvTracking, kCIDLib::True);

                // And send an initial change for this color
                TRGBClr rgbTmp;
                m_hsvTracking.ToRGB(rgbTmp);
                SendChangeNotification(rgbTmp);
            }
             else
            {
                m_eTracking = ETrackModes::None;
            }

            if (m_eTracking != ETrackModes::None)
                CaptureMouse();
        }
         else
        {
            // If tracking, then release the mouse
            if (m_eTracking != ETrackModes::None)
                ReleaseMouse();

            // And take the appropriate actions based on what we were tracking
            if (m_eTracking == ETrackModes::Palette)
            {
                if (m_areaPal.bContainsPoint(pntAt))
                    m_hsvCur = m_hsvTracking;
                SetFromColor(m_hsvCur);
            }
             else if (m_eTracking == ETrackModes::Value)
            {
                m_hsvCur = m_hsvTracking;
                SetFromColor(m_hsvCur);
            }

            // Clear the tracking flag no matter what
            m_eTracking = ETrackModes::None;
        }
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean T24BitPalette::bCreated()
{
    TParent::bCreated();

    // Do an initial calculation of our areas
    CalcAreas(areaWndSize().szArea());

    // Let creation continue
    return kCIDLib::True;
}



tCIDLib::TBoolean
T24BitPalette::bMouseMove(const  TPoint&             pntAt
                        , const tCIDLib::TBoolean
                        , const tCIDLib::TBoolean   )
{
    // If we are tracking, then handle that
    if (m_eTracking == ETrackModes::Palette)
    {
        //
        //  If we are inside the palette area, then update the tracking color
        //  based on the point. Else, fall back to the starting color.
        //
        if (m_areaPal.bContainsPoint(pntAt))
            PointToColor(pntAt, m_hsvTracking);
        else
            m_hsvTracking = m_hsvCur;

        TRGBClr rgbTmp;
        m_hsvTracking.ToRGB(rgbTmp);
        SendChangeNotification(rgbTmp);
    }
     else if (m_eTracking == ETrackModes::Value)
    {
        // Get rid of the current pointer
        TGraphWndDev gdevTarget(*this);
        DrawValueBarPtr(gdevTarget, m_hsvTracking, kCIDLib::False);

        // Store the new value value
        m_hsvTracking.f4Value(f4ValueFromPoint(pntAt));

        // Now draw the new one
        DrawValueBarPtr(gdevTarget, m_hsvTracking, kCIDLib::True);

        // And send the notification for this new value
        TRGBClr rgbTmp;
        m_hsvTracking.ToRGB(rgbTmp);
        SendChangeNotification(rgbTmp);
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
T24BitPalette::bPaint(TGraphDrawDev& gdevTarget, const TArea& areaUpdate)
{
    //
    //  If the update area intersects the bitmap display, then we need to
    //  update it.
    //
    if (areaUpdate.bIntersects(m_areaPal))
    {
        TArea areaBorder(m_areaPal);
        areaBorder.Inflate(CIDWUtils_24BitPalette::c4AreaBorderWidth);
        gdevTarget.Stroke(areaBorder, facCIDGraphDev().rgbBlack);

        //
        //  Figure out the part that we really need to refresh and just
        //  blit that part.
        //
        TArea areaSrc(m_areaPal);
        areaSrc &= areaUpdate;
        TArea areaTarget(areaSrc);
        areaSrc -= m_areaPal.pntOrg();
        gdevTarget.DrawBitmap(m_bmpPal, areaSrc, areaTarget);
    }

    //
    //  If the udpate area intersects the value bar, then we need to
    //  redraw it. We don't attempt to be smart here and just redraw the
    //  whole thing.
    //
    if (areaUpdate.bIntersects(m_areaValueBar))
    {
        TArea areaBorder(m_areaValueBar);
        areaBorder.Inflate(CIDWUtils_24BitPalette::c4AreaBorderWidth);
        gdevTarget.Stroke(areaBorder, facCIDGraphDev().rgbBlack);
        DrawValueBar(gdevTarget);
    }

    // Redraw the selection indicator if we need to
    DrawSelector(gdevTarget);

    return kCIDLib::True;
}



tCIDLib::TVoid T24BitPalette::LosingFocus()
{
    if (m_eTracking != ETrackModes::None)
    {
        ReleaseMouse();
        m_eTracking = ETrackModes::None;
    }
    TParent::LosingFocus();
}



// ---------------------------------------------------------------------------
//  T24BitPalette: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid T24BitPalette::CalcAreas(const TSize& szNew)
{
    TArea areaNew(TPoint::pntOrigin, szNew);
    areaNew.Deflate(CIDWUtils_24BitPalette::c4BorderWidth + 4);

    // The palette is on the left side and the value bar on the right
    m_areaPal = areaNew;
    m_areaValueBar = areaNew;

    m_areaPal.ScaleSize(0.85, 1.0);
    m_areaValueBar.ScaleSize(0.05, 0.9);
    m_areaValueBar.RightJustifyIn(areaNew, kCIDLib::True);

    // Force the value bar length to be a multiple of 3 pixels
    m_areaValueBar.c4Height((m_areaValueBar.c4Height() / 3) * 3);

    //
    //  Regenerate the bitmap to fit the current palette area. We divide it
    //  into 3x3 blocks and assign each one a color based on the H/S values
    //  that would be at their center point.
    //
    //  So adjust the size to evenly map to a 3x3 chunk.
    //
    const tCIDLib::TCard4   c4Cols = m_areaPal.c4Width() / 3;
    const tCIDLib::TCard4   c4Rows = m_areaPal.c4Height() / 3;
    const tCIDLib::TFloat4  f4HInc = 360.0F / c4Cols;
    const tCIDLib::TFloat4  f4VInc = 1.0F / c4Rows;

    m_areaPal.c4Width(c4Cols * 3);
    m_areaPal.c4Height(c4Rows * 3);

    TGraphWndDev gdevCompat(*this);
    TGraphMemDev gdevBmp(gdevCompat, m_areaPal.szArea());

    TArea               areaChunk(0, 0, 3, 3);
    THSVClr             hsvCur;
    TRGBClr             rgbCur;
    tCIDLib::TFloat4    f4HVal = 0;
    tCIDLib::TFloat4    f4VVal = 0;
    for (tCIDLib::TCard4 c4Row = 0; c4Row < c4Rows; c4Row++)
    {
        f4HVal = 0;
        for (tCIDLib::TCard4 c4Col = 0; c4Col < c4Cols; c4Col++)
        {
            hsvCur.Set(f4HVal, f4VVal, 0.85F);
            hsvCur.ToRGB(rgbCur);

            gdevBmp.Fill(areaChunk, rgbCur);
            areaChunk.Move(tCIDLib::EDirs::Right);

            f4HVal += f4HInc;
            if (f4HVal > 360)
                f4HVal = 360;
        }

        f4VVal += f4VInc;
        if (f4VVal > 1)
            f4VVal = 1;
        areaChunk.Move(tCIDLib::EDirs::Down);
        areaChunk.i4X(0);
    }

    // Get a copy of the bitmap we drew
    m_bmpPal = gdevBmp.bmpCurrent();
}


tCIDLib::TVoid
T24BitPalette::ColorToPoint(const THSVClr& hsvSrc, TPoint& pntTarget) const
{
    // Figure out the actual point where the selected color is at
    pntTarget.Set
    (
        tCIDLib::TInt4((hsvSrc.f4Hue() / 360) * m_areaPal.c4Width())
        , tCIDLib::TInt4(hsvSrc.f4Saturation() * m_areaPal.c4Height())
    );

    // Move it up to make it relative to the origin of the palette area
    pntTarget += m_areaPal.pntOrg();
}


tCIDLib::TVoid T24BitPalette::DrawSelector(TGraphDrawDev& gdevTarget)
{
    // Make sure we clip it to the palette area
    TGUIRegion grgnClip(m_areaPal);
    TRegionJanitor janClip(&gdevTarget, grgnClip, tCIDGraphDev::EClipModes::Copy);

    TCosmeticPen gpenLine(facCIDGraphDev().rgbBlack);
    TPenJanitor  janLine(&gdevTarget, &gpenLine);
    TPoint pnt1(0, 5);
    TPoint pnt2(5, 0);
    TPoint pnt3(0, 2);
    TPoint pnt4(2, 0);

    // Calc the current color's selector position based on the color
    const TPoint pntSel(pntSelector());
    gdevTarget.DrawLine(pntSel - pnt1, pntSel - pnt3);
    gdevTarget.DrawLine(pntSel + pnt3, pntSel + pnt1);
    gdevTarget.DrawLine(pntSel - pnt2, pntSel - pnt4);
    gdevTarget.DrawLine(pntSel + pnt2, pntSel + pnt4);
}


tCIDLib::TVoid T24BitPalette::DrawValueBar(TGraphDrawDev& gdevTarget)
{
    // Create a temp HSV value based on the current value
    THSVClr hsvTmp(m_hsvCur);

    // Divide the vertical area of the value bar into 3 pixel areas
    const tCIDLib::TCard4   c4Increments = m_areaValueBar.c4Height() / 3;
    const tCIDLib::TFloat4  f4Inc = 1.0F / c4Increments;

    TArea areaCur(m_areaValueBar);
    areaCur.c4Height(3);
    TRGBClr rgbCur;
    tCIDLib::TFloat4 f4Val = 1;
    hsvTmp.f4Value(0.99F);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Increments; c4Index++)
    {
        // Convert the current value to RGB
        hsvTmp.ToRGB(rgbCur);

        // And use it to fill the current block
        gdevTarget.Fill(areaCur, rgbCur);

        // Move to the next area
        areaCur.Move(tCIDLib::EDirs::Down);

        // And adjust the value
        f4Val -= f4Inc;
        if (f4Val < 0)
            f4Val = 0;
        hsvTmp.f4Value(f4Val);
    }
    DrawValueBarPtr(gdevTarget, m_hsvCur, kCIDLib::True);
}


tCIDLib::TVoid
T24BitPalette::DrawValueBarPtr(         TGraphDrawDev&      gdevTarget
                                , const THSVClr&            hsvToUse
                                , const tCIDLib::TBoolean   bState)
{
    //
    //  Draw the current value indicator. Its a small triangle to the left
    //  of the area, in the gap between the palette and the value bar.
    //
    tCIDLib::TInt4 i4YPos = m_areaValueBar.i4Bottom()
                            - tCIDLib::TInt4
                              (
                                m_areaValueBar.c4Height()
                                * hsvToUse.f4Value()
                              );
    i4YPos -= m_szValSel.c4Height() / 2;

    // Don't forget to include the border which is outside of the area
    tCIDLib::TInt4 i4XPos = m_areaValueBar.i4X()
                            - tCIDLib::TInt4
                              (
                                m_szValSel.c4Width()
                                + CIDWUtils_24BitPalette::c4AreaBorderWidth
                              );

    if (bState)
    {
        gdevTarget.AlphaBlit(m_bmpValSel, TPoint(i4XPos, i4YPos), 0xFF, kCIDLib::True);
    }
     else
    {
        gdevTarget.Fill
        (
            TArea(i4XPos, i4YPos, m_szValSel.c4Width(), m_szValSel.c4Height())
            , rgbBgnFill()
        );
    }
}


tCIDLib::TFloat4 T24BitPalette::f4ValueFromPoint(const TPoint& pntAt) const
{
    //
    //  If its above or below the value bar vertically, then return the
    //  min or max value, else calculate the value.
    //
    if (pntAt.i4Y() > m_areaValueBar.i4Bottom())
        return 0;
    else if (pntAt.i4Y() < m_areaValueBar.i4Y())
        return 1.0F;

    // Convert it to a value from 0 to 1
    tCIDLib::TFloat4 f4Ret = tCIDLib::TFloat4(pntAt.i4Y() - m_areaValueBar.i4Y());
    f4Ret /= m_areaValueBar.c4Height();
    return 1.0F - f4Ret;
}


TPoint T24BitPalette::pntSelector() const
{
    TPoint pntRet;
    ColorToPoint(m_hsvCur, pntRet);
    return pntRet;
}


tCIDLib::TVoid
T24BitPalette::PointToColor(const TPoint& pntSrc, THSVClr& hsvTarget) const
{
    //
    //  Convert to the hue/saturation values and update our current color
    //  with the new values. We don't set the Value attribute.
    //
    tCIDLib::TFloat4 f4Hue = tCIDLib::TFloat4(pntSrc.i4X() - m_areaPal.i4X());
    f4Hue /= tCIDLib::TFloat4(m_areaPal.c4Width());
    f4Hue *= 360;
    tCIDLib::TFloat4 f4Saturation = tCIDLib::TFloat4(pntSrc.i4Y() - m_areaPal.i4Y());
    f4Saturation /= tCIDLib::TFloat4(m_areaPal.c4Height());

    // Give it the current Value value
    hsvTarget.Set(f4Hue, f4Saturation, m_hsvCur.f4Value());
}


tCIDLib::TVoid T24BitPalette::RemoveSelector(TGraphDrawDev& gdevTarget)
{
    // Make sure we clip it to the palette area
    TGUIRegion grgnClip(m_areaPal);
    TRegionJanitor janClip(&gdevTarget, grgnClip, tCIDGraphDev::EClipModes::Copy);

    //
    //  If we just blit back in a 16x16 block centered on the current point
    //  that will cover it.
    //
    TPoint pntSel(pntSelector());
    TArea areaTarget
    (
        pntSel.i4X() - 8
        , pntSel.i4Y() - 8
        , pntSel.i4X() + 8
        , pntSel.i4Y() + 8
    );
    TArea areaSrc(areaTarget);
    areaSrc -= m_areaPal.pntOrg();
    gdevTarget.DrawBitmap(m_bmpPal, areaSrc, areaTarget);
}


tCIDLib::TVoid
T24BitPalette::SendChangeNotification(const TRGBClr& rgbClr)
{
    // Send a synchronous notification
    T24BPalChangeInfo wnotSend(rgbClr, *this);
    SendSyncNotify(wnotSend, T24BitPalette::nidChange);
}


tCIDLib::TVoid T24BitPalette::SetFromColor(const THSVClr& hsvToSet)
{
    // Remove any current selector
    TGraphWndDev gdevTarget(*this);
    RemoveSelector(gdevTarget);

    // Copy over just the hue/saturation
    m_hsvCur.f4Hue(hsvToSet.f4Hue());
    m_hsvCur.f4Saturation(hsvToSet.f4Saturation());

    // Now draw the new selector
    DrawSelector(gdevTarget);

    // And update the value bar
    DrawValueBar(gdevTarget);

    TRGBClr rgbTmp;
    m_hsvCur.ToRGB(rgbTmp);
    SendChangeNotification(rgbTmp);
}


tCIDLib::TVoid T24BitPalette::SetFromPoint(const TPoint& pntAt)
{
    // Remove any current selector
    TGraphWndDev gdevTarget(*this);
    RemoveSelector(gdevTarget);

    // And draw the selector at the new point
    PointToColor(pntAt, m_hsvCur);
    DrawSelector(gdevTarget);

    // And update the value bar
    DrawValueBar(gdevTarget);

    TRGBClr rgbTmp;
    m_hsvCur.ToRGB(rgbTmp);
    SendChangeNotification(rgbTmp);
}

