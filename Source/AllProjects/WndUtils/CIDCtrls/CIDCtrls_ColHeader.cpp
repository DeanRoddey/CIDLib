//
// FILE NAME: CIDCtrls_ColHeader.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/28/2017
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a simple column header control.
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

// Temporary so we can work on this without causing constant rebuilds downstream
#include    "CIDCtrls_ColHeader.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TColHdrChangeInfo,TCtrlNotify)
AdvRTTIDecls(TColHeader, TCtrlWnd)




// ---------------------------------------------------------------------------
//  CLASS: TColHeaderInfo
// PREFIX: colhi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TColHeaderInfo: Constructors and destructor
// ---------------------------------------------------------------------------
TColHeaderInfo::TColHeaderInfo() :

    m_bBgnOver(kCIDLib::False)
    , m_bBold(kCIDLib::False)
    , m_bTextOver(kCIDLib::False)
    , m_c4Width(24)
    , m_eType(tCIDCtrls::ECHdrTypes::TextOnly)
    , m_eJustify(tCIDLib::EHJustify::Left)
    , m_strTitle(L"Column")
{
}

TColHeaderInfo::TColHeaderInfo( const   TString&            strTitle
                                , const tCIDLib::TCard4     c4Width
                                , const tCIDLib::EHJustify  eJustify) :

    m_bBgnOver(kCIDLib::False)
    , m_bBold(kCIDLib::False)
    , m_bTextOver(kCIDLib::False)
    , m_c4Width(c4Width)
    , m_eType(tCIDCtrls::ECHdrTypes::TextOnly)
    , m_eJustify(eJustify)
    , m_strTitle(strTitle)
{
}

TColHeaderInfo::TColHeaderInfo( const   TBitmap&            bmpTitle
                                , const tCIDLib::TCard4     c4Width
                                , const tCIDLib::EHJustify  eJustify) :

    m_bBgnOver(kCIDLib::False)
    , m_bBold(kCIDLib::False)
    , m_bTextOver(kCIDLib::False)
    , m_bmpTitle(bmpTitle)
    , m_c4Width(c4Width)
    , m_eType(tCIDCtrls::ECHdrTypes::BitmapOnly)
    , m_eJustify(eJustify)
{
}

TColHeaderInfo::TColHeaderInfo( const   TString&                strTitle
                                , const TBitmap&                bmpTitle
                                , const tCIDCtrls::ECHdrTypes   eType
                                , const tCIDLib::TCard4         c4Width
                                , const tCIDLib::EHJustify      eJustify) :

    m_bBgnOver(kCIDLib::False)
    , m_bBold(kCIDLib::False)
    , m_bTextOver(kCIDLib::False)
    , m_bmpTitle(bmpTitle)
    , m_c4Width(c4Width)
    , m_eType(eType)
    , m_eJustify(eJustify)
    , m_strTitle(strTitle)
{
}

TColHeaderInfo::TColHeaderInfo(const TColHeaderInfo& colhiSrc) :

    m_bBgnOver(colhiSrc.m_bBgnOver)
    , m_bBold(colhiSrc.m_bBold)
    , m_bTextOver(colhiSrc.m_bTextOver)
    , m_bmpTitle(colhiSrc.m_bmpTitle)
    , m_c4Width(colhiSrc.m_c4Width)
    , m_eType(colhiSrc.m_eType)
    , m_eJustify(colhiSrc.m_eJustify)
    , m_rgbBgn1(colhiSrc.m_rgbBgn1)
    , m_rgbBgn2(colhiSrc.m_rgbBgn2)
    , m_rgbText(colhiSrc.m_rgbText)
    , m_strTitle(colhiSrc.m_strTitle)
{
}

TColHeaderInfo::~TColHeaderInfo()
{
}


// ---------------------------------------------------------------------------
//  TColHeaderInfo: Public operators
// ---------------------------------------------------------------------------
TColHeaderInfo& TColHeaderInfo::operator=(const TColHeaderInfo& colhiSrc)
{
    if (this != &colhiSrc)
    {
        m_bBgnOver  = colhiSrc.m_bBgnOver;
        m_bBold     = colhiSrc.m_bBold;
        m_bTextOver = colhiSrc.m_bTextOver;
        m_bmpTitle  = colhiSrc.m_bmpTitle;
        m_c4Width   = colhiSrc.m_c4Width;
        m_eType     = colhiSrc.m_eType;
        m_eJustify  = colhiSrc.m_eJustify;
        m_rgbBgn1   = colhiSrc.m_rgbBgn1;
        m_rgbBgn2   = colhiSrc.m_rgbBgn2;
        m_rgbText   = colhiSrc.m_rgbText;
        m_strTitle  = colhiSrc.m_strTitle;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TColHeaderInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get/set the bold text flag
tCIDLib::TBoolean TColHeaderInfo::bBold() const
{
    return m_bBold;
}

tCIDLib::TBoolean TColHeaderInfo::bBold(const tCIDLib::TBoolean bToSet)
{
    m_bBold = bToSet;
    return m_bBold;
}


// Override the colors used by this slot
tCIDLib::TVoid
TColHeaderInfo::SetColors(  const   TRGBClr&    rgbBgnClr
                            , const TRGBClr&    rgbTextClr)
{
    m_bBgnOver = kCIDLib::True;
    m_bTextOver = kCIDLib::True;
    m_rgbBgn1 = rgbBgnClr;
    m_rgbBgn2 = rgbBgnClr;
    m_rgbText = rgbTextClr;

    m_rgbBgn1.Adjust(16, 16, 16);
    m_rgbBgn2.Adjust(-16, -16, -16);
}



// ---------------------------------------------------------------------------
//   CLASS: TColHdrChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TColHdrChangeInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TColHdrChangeInfo::TColHdrChangeInfo(const  tCIDCtrls::ECHdrEvents  eEvent
                                    , const tCIDLib::TCard4         c4ColIndex
                                    , const TWindow&                wndSrc) :
    TCtrlNotify(wndSrc)
    , m_c4ColIndex(c4ColIndex)
    , m_c4OldWidth(0)
    , m_c4NewWidth(0)
    , m_eEvent(eEvent)
{
}

// Sets up a column drag notification
TColHdrChangeInfo::TColHdrChangeInfo(const tCIDLib::TCard4  c4ColIndex
                                    , const tCIDLib::TCard4 c4OldWidth
                                    , const tCIDLib::TCard4 c4NewWidth
                                    , const TWindow&        wndSrc) :
    TCtrlNotify(wndSrc)
    , m_c4ColIndex(c4ColIndex)
    , m_c4OldWidth(c4OldWidth)
    , m_c4NewWidth(c4NewWidth)
    , m_eEvent(tCIDCtrls::ECHdrEvents::DragDrag)
{
}

TColHdrChangeInfo::TColHdrChangeInfo(const TColHdrChangeInfo& wnotSrc) :

    TCtrlNotify(wnotSrc)
    , m_c4ColIndex(wnotSrc.m_c4ColIndex)
    , m_c4OldWidth(wnotSrc.m_c4OldWidth)
    , m_c4NewWidth(wnotSrc.m_c4NewWidth)
    , m_eEvent(wnotSrc.m_eEvent)
{
}

TColHdrChangeInfo::~TColHdrChangeInfo()
{
}


// ---------------------------------------------------------------------------
//  TColHdrChangeInfo: Public operators
// ---------------------------------------------------------------------------
TColHdrChangeInfo& TColHdrChangeInfo::operator=(const TColHdrChangeInfo& wnotSrc)
{
    if (this != &wnotSrc)
    {
        TParent::operator=(wnotSrc);

        m_c4ColIndex    = wnotSrc.m_c4ColIndex;
        m_c4OldWidth    = wnotSrc.m_c4OldWidth;
        m_c4NewWidth    = wnotSrc.m_c4NewWidth;
        m_eEvent        = wnotSrc.m_eEvent;
    }
    return *this;
}




// ---------------------------------------------------------------------------
//  CLASS: TColHeader
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TColHeader: Public static data
// ---------------------------------------------------------------------------
const TNotificationId TColHeader::nidChangeId(L"ColHdrChangeEvent");


// ---------------------------------------------------------------------------
// TColHeader: Constructors and Destructor
// ---------------------------------------------------------------------------
TColHeader::TColHeader() :

    TCtrlWnd()
    , m_eCHdrStyles(tCIDCtrls::ECHdrStyles::None)
    , m_eTrack(ETrackStates::Idle)
    , m_rgbText(0x16, 16, 16)
{
}

TColHeader::~TColHeader()
{
}


// ---------------------------------------------------------------------------
// TColHeader: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TColHeader::InitFromDesc(const  TWindow&                wndParent
                        , const TDlgItem&               dlgiSrc
                        , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles::VisGroupChild;
    tCIDCtrls::ECHdrStyles eHdrStyles = tCIDCtrls::ECHdrStyles::None;

    CreateColHeader(wndParent, dlgiSrc.ridChild(), eStyles, eHdrStyles);
}


tCIDLib::TVoid TColHeader::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);

}


//
//  We override this so that we can set up our gradient fill, which is based on the set
//  background color.
//
tCIDLib::TVoid
TColHeader::SetBgnColor(const   TRGBClr&            rgbToSet
                        , const tCIDLib::TBoolean   bOver
                        , const tCIDLib::TBoolean   bRedraw)
{
    // Call our parent first
    TParent::SetBgnColor(rgbToSet, bOver, bRedraw);

    // Now set up our gradient colors based on that
    SetFillColors(rgbToSet);
}


TSize TColHeader::szDefault() const
{
    // Make it a bit more than a standard text height
    TSize szInit(96, 10);
    TGUIFacility::AdjustDlgUnits(szInit);
    return szInit;
}


// ---------------------------------------------------------------------------
// TColHeader: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Return the count of columns
tCIDLib::TCard4 TColHeader::c4ColCount() const
{
    return m_colList.c4ElemCount();
}


// Returns the combined width of all of the columns
tCIDLib::TCard4 TColHeader::c4FullWidth() const
{
    tCIDLib::TCard4 c4Ret = 0;
    const tCIDLib::TCard4 c4Count  = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        c4Ret += m_colList[c4Index].m_c4Width;

    return c4Ret;
}


// Return the current widths of all of our columns
tCIDLib::TCard4 TColHeader::c4QueryWidths(tCIDLib::TCardList& fcolToFill)
{
    fcolToFill.RemoveAll();

    const tCIDLib::TCard4 c4Count  = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        fcolToFill.c4AddElement(m_colList[c4Index].m_c4Width);

    return c4Count;
}


// Give access to a specific header info object by index
const TColHeaderInfo& TColHeader::colhiAt(const tCIDLib::TCard4 c4At) const
{
    return m_colList[c4At];
}


tCIDLib::TVoid
TColHeader::CreateColHeader(const   TWindow&                wndParent
                            , const tCIDCtrls::TWndId       widThis
                            , const tCIDCtrls::EWndStyles   eStyles
                            , const tCIDCtrls::ECHdrStyles  eCHdrStyles
                            , const tCIDCtrls::EExWndStyles eExStyles)
{
    // Figure out the area
    TArea areaInit;
    wndParent.QueryClientArea(areaInit, kCIDLib::True);
    areaInit.SetOrg(0, 0);
    areaInit.c4Height(szDefault().c4Height());

    // STore our type specific styles
    m_eCHdrStyles = eCHdrStyles;

    CreateWnd
    (
        wndParent.hwndThis()
        , kCIDCtrls::pszCustClass
        , L""
        , areaInit
        , eStyles
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );
}


// Lets the client code set the columns. We store the new info and do a full redraw
tCIDLib::TVoid TColHeader::SetColumns(const TColHeader::TColList& colToSet)
{
    // Get our own copy of the column info objects
    m_colList = colToSet;

    //
    //  Go through them and, for any that have a width of max card, calculate the needed
    //  width and set that.
    //
    const tCIDLib::TCard4 c4Count  = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TColHeaderInfo& colhiCur = m_colList[c4Index];
        if (colhiCur.m_c4Width == kCIDLib::c4MaxCard)
        {
        }
    }
    Redraw(tCIDCtrls::ERedrawFlags::Immediate);
}


//
//  Once created, we seldom change area other than our width being modified to keep us
//  filling the width of the parent.
//
tCIDLib::TVoid TColHeader::SetColHdrWidth(const tCIDLib::TCard4 c4ToSet)
{
    TSize szNew = areaWndSize().szArea();
    szNew.c4Width(c4ToSet);

    SetSize(szNew, kCIDLib::True);
}


// Set the color used for the text of columns that haven't been overridden
tCIDLib::TVoid TColHeader::SetDefTextColor(const TRGBClr& rgbToSet)
{
    m_rgbText = rgbToSet;
}


// ---------------------------------------------------------------------------
// TColHeader: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  We have to handle clicks and dragging/tracking of edges.
//
tCIDLib::TBoolean
TColHeader::bClick( const   tCIDCtrls::EMouseButts  eButton
                    , const tCIDCtrls::EMouseClicks eClickType
                    , const TPoint&                 pntAt
                    , const tCIDLib::TBoolean       bCtrlShift
                    , const tCIDLib::TBoolean       bShift)
{
    // We only care about left clicks
    if (eButton != tCIDCtrls::EMouseButts::Left)
        return kCIDLib::True;

    if (eClickType == tCIDCtrls::EMouseClicks::Down)
    {
        // Just in case, make sure we are in idle state
        m_eTrack = ETrackStates::Idle;

        // See if it's in any slots, or in the in-between where we need to start a drag
        tCIDLib::TCard4 c4Index;
        tCIDCtrls::EWndAreas eArea = eSlotHitTest(pntAt, c4Index);

        if (eArea == tCIDCtrls::EWndAreas::Client)
        {
            m_eTrack = ETrackStates::Click;
            m_c4TrackIndex = c4Index;
        }
         else if (eArea == tCIDCtrls::EWndAreas::Right)
        {
            m_eTrack = ETrackStates::Drag;
            m_c4TrackIndex = c4Index;

            // Capture the mouse while we do this
            CaptureMouse();

            TColHdrChangeInfo wnotSend(tCIDCtrls::ECHdrEvents::DragStart, c4Index, *this);
            SendSyncNotify(wnotSend, nidChangeId);
        }
    }
     else if (eClickType == tCIDCtrls::EMouseClicks::Up)
    {
        // Save the current info and reset it
        const tCIDLib::TCard4 c4Index = m_c4TrackIndex;
        const ETrackStates eState = m_eTrack;

        // Release the mouse capture if we were tracking
        if (eState != ETrackStates::Idle)
            ReleaseMouse();

        m_c4TrackIndex = 0;
        m_eTrack = ETrackStates::Idle;

        // If we were tracking, then we either send a click event or end tracking.
        if (eState == ETrackStates::Click)
        {
            SendAsyncNotify
            (
                new TColHdrChangeInfo(tCIDCtrls::ECHdrEvents::ColClick, c4Index, *this), nidChangeId
            );
        }
         else if (eState == ETrackStates::Drag)
        {
            TColHdrChangeInfo wnotSend(tCIDCtrls::ECHdrEvents::DragEnd, c4Index, *this);
            SendSyncNotify(wnotSend, nidChangeId);
        }
    }

    return kCIDLib::True;
}


// Handle any post-creation setup
tCIDLib::TBoolean TColHeader::bCreated()
{
    // Call our parent first
    TParent::bCreated();

    // Set our fill colors with a default initial color
    SetBgnColor(facCIDGraphDev().rgbDarkWhite);
    SetFillColors(rgbBgnFill());

    return kCIDLib::True;
}


// We want to minimize the flicker, so we do nothing here and just do it in the paint
tCIDLib::TBoolean TColHeader::bEraseBgn(TGraphDrawDev&)
{
    return kCIDLib::True;
}


// We need to show a left/right sizing pointer if that is appropriate
tCIDLib::TBoolean
TColHeader::bMouseMove( const   TPoint&             pntAt
                        , const tCIDLib::TBoolean   bControlDown
                        , const tCIDLib::TBoolean   bShiftDown)
{
    //
    //  Limit the right side to our right side. It may be more if we have captured the
    //  mouse.
    //
    const TArea& areaUs = areaWndSize();
    TPoint pntReal(pntAt);
    if (pntReal.i4X() > areaUs.i4Right())
        pntReal.i4X(areaUs.i4Right());

    //
    //  If we are doing a drag style track, or we are over a spot where it could be started,
    //  we need to keep the mouse in the horz sizing shape.
    //
    tCIDLib::TBoolean bSizeType = (m_eTrack == ETrackStates::Drag);
    if (!bSizeType)
    {
        tCIDLib::TCard4 c4Index;
        bSizeType = eSlotHitTest(pntReal, c4Index) == tCIDCtrls::EWndAreas::Right;
    }

    if (bSizeType)
    {
        if (m_eTrack == ETrackStates::Drag)
        {
            const tCIDLib::TCard4 c4SlotCount = m_colList.c4ElemCount();
            TColHeaderInfo& colhiDrag = m_colList[m_c4TrackIndex];

            // Get original x offset info
            tCIDLib::TInt4 i4OrgSlotsEndX;
            tCIDLib::TInt4 i4OrgSlotEndX;
            tCIDLib::TInt4 i4OrgSlotX = i4SlotXOffset(m_c4TrackIndex, i4OrgSlotEndX, i4OrgSlotsEndX);

            // Set the new width and get the new info. Don't let it get less than 12 pixels.
            const tCIDLib::TCard4 c4OrgSlotWidth = colhiDrag.m_c4Width;
            tCIDLib::TInt4 i4NewWidth = tCIDLib::TInt4(c4OrgSlotWidth) + (pntReal.i4X() - i4OrgSlotEndX);
            if (i4NewWidth < 12)
                i4NewWidth = 12;
            colhiDrag.m_c4Width = tCIDLib::TCard4(i4NewWidth);

            // If the width actually changed, we need to do something
            if (c4OrgSlotWidth != colhiDrag.m_c4Width)
            {
                // Now get the new x offsets
                tCIDLib::TInt4 i4NewSlotsEndX;
                tCIDLib::TInt4 i4NewSlotEndX;
                tCIDLib::TInt4 i4NewSlotX = i4SlotXOffset
                (
                    m_c4TrackIndex, i4NewSlotEndX, i4NewSlotsEndX
                );

                //
                //  We do different things depending on whether we are sizing the last
                //  column or not. For the last one, we just redraw the exposed part at
                //  the end.
                //
                TArea areaUpdate(areaUs);
                areaUpdate.AdjustSize(0, -1);
                if (m_c4TrackIndex + 1 == c4SlotCount)
                {
                    // New and old X offset should be the same here
                    areaUpdate.i4X(i4OrgSlotX);

                    // Go to the farthest end point
                    if (i4OrgSlotsEndX < i4NewSlotsEndX)
                        areaUpdate.i4Right(i4NewSlotsEndX + 1);
                    else
                        areaUpdate.i4Right(i4OrgSlotsEndX + 1);

                    Redraw(areaUpdate, tCIDCtrls::ERedrawFlags::Immediate);
                }
                 else
                {
                    //
                    //  We need to scroll what content we can and only redraw what we have
                    //  to redraw.
                    //
                    //  1.  If we got larger, then we scroll everything from the original next
                    //      slot to the new slots end.
                    //
                    //  2.  If we got smaller, then we scroll everything from the original
                    //      right side to the end left to the new right side. That leaves
                    //      the trailing bit at the end to be refreshed.
                    //
                    TArea areaInv;
                    if (i4OrgSlotsEndX < i4NewSlotsEndX)
                    {
                        // It's scenario #1, we got bigger
                        areaUpdate.i4X(i4OrgSlotEndX + 1);
                        areaUpdate.i4Right(i4NewSlotsEndX);
                        Scroll
                        (
                            areaUpdate
                            , areaUpdate
                            , areaInv
                            , i4NewSlotsEndX - i4OrgSlotsEndX
                            , 0
                            , kCIDLib::False
                        );
                    }
                     else
                    {
                        // It's scenario #2, we got smaller
                        areaUpdate.i4X(i4NewSlotEndX + 1);
                        areaUpdate.i4Right(i4OrgSlotsEndX);
                        Scroll
                        (
                            areaUpdate
                            , areaUpdate
                            , areaInv
                            , i4NewSlotsEndX - i4OrgSlotsEndX
                            , 0
                            , kCIDLib::False
                        );

                        //
                        //  Redraw the trailing bit at the end that was uncovered. If the new
                        //  slots end offset is still beyond our window area, then we just want
                        //  redraw a chunk at the end of our window area that is the size of
                        //  reduction difference.
                        //
                        if(i4NewSlotsEndX >= areaUs.i4Right())
                        {
                            areaUpdate.i4X(areaUs.i4Right() - (i4OrgSlotsEndX - i4NewSlotsEndX));
                            areaUpdate.i4Right(areaUs.i4Right() + 1);
                        }
                         else
                        {
                            // The normal scheme, so
                            areaUpdate.i4X(i4NewSlotsEndX);
                            areaUpdate.i4Right(i4OrgSlotsEndX);
                        }
                        Redraw(areaUpdate, tCIDCtrls::ERedrawFlags::Immediate);

                        // Redraw(areaInv, tCIDCtrls::ERedrawFlags::Immediate);
                    }

                    // Redraw the target slot (now set to its new size)
                    areaUpdate.i4X(i4OrgSlotX);
                    areaUpdate.i4Right(i4NewSlotEndX + 1);
//                    Redraw(areaUpdate, tCIDCtrls::ERedrawFlags::Immediate);
                }

                // And let's send a change notification, synchronous
                TColHdrChangeInfo wnotSend
                (
                    m_c4TrackIndex, c4OrgSlotWidth, colhiDrag.m_c4Width, *this
                );
                SendSyncNotify(wnotSend, nidChangeId);
            }
        }

        facCIDCtrls().SetPointer(tCIDCtrls::ESysPtrs::SizeWE);
        return kCIDLib::True;
    }

    // Let the default happen
    return kCIDLib::False;
}


// Handles our drawing logics
tCIDLib::TBoolean
TColHeader::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    const TArea& areaUs = areaWndSize();

    gdevToUse.eBackMixMode(tCIDGraphDev::EBackMixModes::Transparent);

    //
    //  Draw any slots that intersect the update area
    //
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    TArea areaSlot(areaUs);
    areaSlot.AdjustSize(0, -1);
    TArea areaText;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TColHeaderInfo& colhiCur = m_colList[c4Index];
        areaSlot.c4Width(colhiCur.m_c4Width);

        // If this guy doesn't intersect the update area, nothing to do
        if (areaSlot.bIntersects(areaUpdate))
        {
            // If this guy has overridden bgn colors, use his, else use ours
            if (colhiCur.m_bBgnOver)
            {
                gdevToUse.GradientFill
                (
                    areaSlot
                    , colhiCur.m_rgbBgn1
                    , colhiCur.m_rgbBgn2
                    , tCIDGraphDev::EGradDirs::Vertical
                );
            }
             else
            {
                 gdevToUse.GradientFill
                 (
                    areaSlot
                    , m_rgbBgn1
                    , m_rgbBgn2
                    , tCIDGraphDev::EGradDirs::Vertical
                 );
            }

            // Draw the text if needed
            areaText = areaSlot;
            areaText.Deflate(6, 2);
            if (areaText.bIntersects(areaUpdate))
            {
                if (colhiCur.m_bTextOver)
                    gdevToUse.SetTextColor(colhiCur.m_rgbText);
                else
                    gdevToUse.SetTextColor(m_rgbText);

                if (colhiCur.m_bBold)
                {
                    TFontJanitor janFont(&gdevToUse, &TGUIFacility::gfontDefNorm());
                    gdevToUse.DrawString(colhiCur.m_strTitle, areaText, colhiCur.m_eJustify);
                }
                 else
                {
                    TFontJanitor janFont(&gdevToUse, &TGUIFacility::gfontDefLight());
                    gdevToUse.DrawString(colhiCur.m_strTitle, areaText, colhiCur.m_eJustify);
                }
            }

            //
            //  Draw a a separator line to the right. It doesn't include the final point so
            //  we have to adjust it.
            //
            gdevToUse.DrawLine(areaSlot.pntUR(), areaSlot.pntLR(0, 1), facCIDGraphDev().rgbPaleGrey);
        }

        // Move over to the next slot X
        areaSlot.Move(tCIDLib::EDirs::Right);
    }

    //
    //  Fill any remaining area with our own background colors, if we have any left and it
    //  intersects the update area. We have to special case if we have no headers since the
    //  slot area will not have gotten adjusted and will be as wide us whic would prevent us
    //  from doing anything here.
    //
    if ((areaSlot.i4X() < areaUs.i4Right()) || !c4Count)
    {
        areaSlot.i4Right(areaUs.i4Right());
        if (areaSlot.bIntersects(areaUpdate))
        {
            gdevToUse.GradientFill
            (
                areaSlot, m_rgbBgn1, m_rgbBgn2, tCIDGraphDev::EGradDirs::Vertical
            );
        }
    }

    // Draw a divider line along bottom if needed
    if (areaUpdate.i4Bottom() == areaUs.i4Bottom())
        gdevToUse.DrawLine(areaUs.pntLL(), areaUs.pntLR(1, 0), facCIDGraphDev().rgbPaleGrey);

    // Say we did it
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
// TColHeader: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is used to see if we get a hit on a slot. We return either a 'client' area hit
//  which means it was clicked in the main area of the slot, or we return right edge if
//  it was on the edge. If it's on the left edge, we return a hit on the previous index
//  (if it's not the left of the 0th.) Else we just say no hit.
//
tCIDCtrls::EWndAreas TColHeader::eSlotHitTest(const TPoint& pntAt, tCIDLib::TCard4& c4Index)
{
    TArea areaSlot(areaWndSize());
    areaSlot.AdjustSize(0, -1);

    tCIDCtrls::EWndAreas eArea = tCIDCtrls::EWndAreas::Nowhere;
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    c4Index = 0;
    for (; c4Index < c4Count; c4Index++)
    {
        TColHeaderInfo& colhiCur = m_colList[c4Index];
        areaSlot.c4Width(colhiCur.m_c4Width);

        eArea = facCIDCtrls().eGenericHitCalc(pntAt, areaSlot, 3);
        switch(eArea)
        {
            case tCIDCtrls::EWndAreas::Bottom :
            case tCIDCtrls::EWndAreas::Top :
            case tCIDCtrls::EWndAreas::Client :
                //
                //  Make sure it's actually within the slot area. The sizing marging could
                //  allow bottom/top hits to be outside it.
                //
                if (areaSlot.bContainsPoint(pntAt))
                    eArea = tCIDCtrls::EWndAreas::Client;
                break;

            case tCIDCtrls::EWndAreas::Left :
            case tCIDCtrls::EWndAreas::BottomLeft :
            case tCIDCtrls::EWndAreas::TopLeft :
                //
                //  If it's the 0th one, we can't report left, else report previous slot
                //  on its right side.
                //
                if (!c4Index)
                {
                    eArea = tCIDCtrls::EWndAreas::Nowhere;
                }
                 else
                {
                    eArea = tCIDCtrls::EWndAreas::Right;
                    c4Index--;
                }
                break;

            case tCIDCtrls::EWndAreas::Right :
            case tCIDCtrls::EWndAreas::BottomRight :
            case tCIDCtrls::EWndAreas::TopRight :
                eArea = tCIDCtrls::EWndAreas::Right;
                break;

            default :
                break;
        };

        // If we got a hit, break out
        if (eArea != tCIDCtrls::EWndAreas::Nowhere)
            break;

        areaSlot.Move(tCIDLib::EDirs::Right);
    }

    return eArea;
}


//
//  Calculate the x offset of a given slot by index. We just add up the widths of the
//  slots before it. We have another that also gives back the X offset that is just past
//  the end of the last slot.
//
tCIDLib::TInt4 TColHeader::i4SlotXOffset(const tCIDLib::TCard4 c4At) const
{
    // To simplify things below, check special case. Doesn't matter how many slots we have
    if (!c4At)
        return 0;

    tCIDLib::TInt4 i4Ret = 0;
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    CIDAssert(c4At < c4Count, L"Invalid slot index, can't calc X offset");

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4At; c4Index++)
        i4Ret += tCIDLib::TInt4(m_colList[c4Index].m_c4Width);

    return i4Ret;
}

tCIDLib::TInt4
TColHeader::i4SlotXOffset(  const   tCIDLib::TCard4     c4At
                            ,       tCIDLib::TInt4&     i4SlotEndX
                            ,       tCIDLib::TInt4&     i4SlotsEndX) const
{
    i4SlotsEndX = 0;
    tCIDLib::TInt4 i4Ret = 0;

    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    CIDAssert(c4At < c4Count, L"Invalid slot index, can't calc X offset");

    tCIDLib::TCard4 c4Index = 0;
    for (; c4Index < c4At; c4Index++)
        i4Ret += tCIDLib::TInt4(m_colList[c4Index].m_c4Width);

    // Set the end of this slot
    i4SlotEndX = i4Ret + tCIDLib::TInt4(m_colList[c4Index].m_c4Width);

    // Pick up from there and do the rest
    i4SlotsEndX = i4Ret;
    for (; c4Index < c4Count; c4Index++)
        i4SlotsEndX += tCIDLib::TInt4(m_colList[c4Index].m_c4Width);

    return i4Ret;
}


// Sets up our fill colors based on the bgn color set on us
tCIDLib::TVoid TColHeader::SetFillColors(const TRGBClr& rgbBase)
{
    m_rgbBgn1 = rgbBase;
    m_rgbBgn2 = rgbBase;

    // Lighten 1 a bit and darken 2 a bit
    m_rgbBgn1.Adjust(16, 16, 16);
    m_rgbBgn2.Adjust(-16, -16, -16);
}
