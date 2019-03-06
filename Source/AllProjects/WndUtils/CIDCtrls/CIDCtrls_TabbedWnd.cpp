//
// FILE NAME: CIDCtrls_TTabbedWnd.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/15/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the tabbed window control
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
RTTIDecls(TTabWindow, TWindow)
RTTIDecls(TTabEventInfo,TCtrlNotify)
AdvRTTIDecls(TTabbedWnd, TCtrlWnd)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDCtrls_TabbedWnd
{
    // Pseudo menu command ids for use in our accelerator table
    const tCIDLib::TResId ridMenu_Close     = 100;
    const tCIDLib::TResId ridMenu_NextTab   = 101;
}



// ---------------------------------------------------------------------------
//   CLASS: TTabEventInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTabEventInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TTabEventInfo::TTabEventInfo(const  tCIDLib::TCard4         c4TabId
                            , const tCIDCtrls::ETabWEvents  eEvent
                            , const TWindow&                wndSrc) :

    TCtrlNotify(wndSrc)
    , m_bAllow(kCIDLib::True)
    , m_c4TabId(c4TabId)
    , m_c4PrevTabId(kCIDLib::c4MaxCard)
    , m_eEvent(eEvent)
{
}

TTabEventInfo::TTabEventInfo(const  tCIDLib::TCard4         c4NewTabId
                            , const tCIDLib::TCard4         c4PrevTabId
                            , const tCIDCtrls::ETabWEvents  eEvent
                            , const TWindow&                wndSrc) :

    TCtrlNotify(wndSrc)
    , m_bAllow(kCIDLib::True)
    , m_c4TabId(c4NewTabId)
    , m_c4PrevTabId(c4PrevTabId)
    , m_eEvent(eEvent)
{
}

TTabEventInfo::TTabEventInfo(const TTabEventInfo& wnotSrc) :

    TCtrlNotify(wnotSrc)
    , m_bAllow(wnotSrc.m_bAllow)
    , m_c4TabId(wnotSrc.m_c4TabId)
    , m_c4PrevTabId(wnotSrc.m_c4PrevTabId)
    , m_eEvent(wnotSrc.m_eEvent)
{
}

TTabEventInfo::~TTabEventInfo()
{
}


// ---------------------------------------------------------------------------
//  TTabEventInfo: Public operators
// ---------------------------------------------------------------------------
TTabEventInfo&
TTabEventInfo::operator=(const TTabEventInfo& wnotSrc)
{
    if (this != &wnotSrc)
    {
        TParent::operator=(wnotSrc);
        m_bAllow        = wnotSrc.m_bAllow;
        m_c4TabId       = wnotSrc.m_c4TabId;
        m_c4PrevTabId   = wnotSrc.m_c4PrevTabId;
        m_eEvent        = wnotSrc.m_eEvent;
    }
    return *this;
}


// -------------------------------------------------------------------
//  TTabEventInfo: Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TTabEventInfo::bAllow() const
{
    return m_bAllow;
}

tCIDLib::TBoolean TTabEventInfo::bAllow(const tCIDLib::TBoolean bToSet)
{
    m_bAllow = bToSet;
    return m_bAllow;
}


tCIDLib::TCard4 TTabEventInfo::c4PrevTabId() const
{
    return m_c4PrevTabId;
}


tCIDLib::TCard4 TTabEventInfo::c4TabId() const
{
    return m_c4TabId;
}


tCIDCtrls::ETabWEvents TTabEventInfo::eEvent() const
{
    return m_eEvent;
}



// ---------------------------------------------------------------------------
//  CLASS: TTabWindow
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTabWindow: Destructor
// ---------------------------------------------------------------------------
TTabWindow::~TTabWindow()
{
}


// ---------------------------------------------------------------------------
//  TTabWindow: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Make our tab areas available for drawing/hit testing purposes
const TArea& TTabWindow::areaTab() const
{
    return m_areaUs;
}

const TArea& TTabWindow::areaClose() const
{
    return m_areaClose;
}

const TArea& TTabWindow::areaText1() const
{
    return m_areaText1;
}

const TArea& TTabWindow::areaText2() const
{
    return m_areaText2;
}


// If our area is not empty, then we are visible
tCIDLib::TBoolean TTabWindow::bTabIsVisible() const
{
    return !m_areaUs.bIsEmpty();
}


// See if the passed point hits one of our areas
tCIDCtrls::ETabWEvents TTabWindow::eHitTest(const TPoint& pntAt)
{
    // If our area is zero sized, we are not visible so return _Count
    if (m_areaUs.bIsEmpty())
        return tCIDCtrls::ETabWEvents::Count;

    if (m_areaClose.bContainsPoint(pntAt))
        return tCIDCtrls::ETabWEvents::Close;

    if (m_areaUs.bContainsPoint(pntAt))
        return tCIDCtrls::ETabWEvents::Hit;

    return tCIDCtrls::ETabWEvents::None;
}


// Make the alt text available
const TString& TTabWindow::strAltText() const
{
    return m_strAltText;
}



//
//  Lets the outside world put a tab into one of the standard tab states. They can ask
//  that we redraw immediately, meaning the tab header, not the tab itself. These states
//  just affect the tab header.
//
tCIDLib::TVoid
TTabWindow::SetState(const  tCIDCtrls::ETabStates   eToSet
                    , const tCIDLib::TBoolean       bRedraw)
{
    // If our owning tabbed window isn't set that's bad
    CIDAssert(m_pwndOwningTabbedWnd != nullptr, L"The owning tabbed window is not set");

    if (eToSet != m_eState)
    {
        m_eState = eToSet;
        if (bRedraw)
            m_pwndOwningTabbedWnd->RedrawTabHdr(m_c4TabId);
    }
}


// ---------------------------------------------------------------------------
//  TTabWindow: Hidden constructors
// ---------------------------------------------------------------------------
TTabWindow::TTabWindow( const   TString&            strUniqueId
                        , const TString&            strAltText
                        , const tCIDLib::TBoolean   bCanClose
                        , const tCIDLib::TBoolean   bDlgProc) :

    TWindow(bDlgProc)
    , m_bCanClose(bCanClose)
    , m_c4TabId(0)
    , m_eState(tCIDCtrls::ETabStates::Normal)
    , m_hwndLastFocus(kCIDCtrls::hwndInvalid)
    , m_pwndOwningTabbedWnd(nullptr)
    , m_strAltText(strAltText)
{
    //
    //  Set the unique id on the window as its window name (the hidden text of the window.)
    //  The window doesn't have to be created to do this.
    //
    strName(strUniqueId);
}


// ---------------------------------------------------------------------------
//  TTabWindow: Protected, virtual methods
// ---------------------------------------------------------------------------

// Do nothing if not overridden
tCIDLib::TVoid TTabWindow::TabActivationChange(const tCIDLib::TBoolean)
{
}


// ---------------------------------------------------------------------------
//  TTabWindow: Protected, inherited methods
// ---------------------------------------------------------------------------

// Do it all in the paint, where we are clipped to our children
tCIDLib::TBoolean TTabWindow::bEraseBgn(TGraphDrawDev&)
{
    return kCIDLib::True;
}


tCIDLib::TBoolean TTabWindow::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    gdevToUse.Fill(areaUpdate, rgbBgnFill());
    return kCIDLib::True;
}


tCIDLib::TVoid
TTabWindow::ChildFocusChange(const  TWindow&            wndParent
                            , const tCIDCtrls::TWndId   widChild
                            , const tCIDLib::TBoolean   bGotFocus)
{
    if (bGotFocus)
        m_hwndLastFocus = facCIDCtrls().hwndChildFromId(wndParent.hwndSafe(), widChild);
}


tCIDLib::TVoid TTabWindow::GettingFocus()
{
}


// ---------------------------------------------------------------------------
//  TTabWindow: Protected, non-virtual methods
// ---------------------------------------------------------------------------

// Create this window, forcing on any options required
tCIDLib::TVoid
TTabWindow::CreateTabWnd(const  TWindow&                wndParent
                        ,       TTabbedWnd* const       pwndOwningTabbedWnd
                        , const TString&                strTabText
                        , const TArea&                  areaTab
                        , const tCIDCtrls::TWndId       widToUse
                        , const tCIDCtrls::EWndStyles   eStyles
                        , const tCIDCtrls::EExWndStyles eExStyles)
{
    // Save the ponter to our owning tabbed window
    m_pwndOwningTabbedWnd = pwndOwningTabbedWnd;

   CreateWnd
    (
        wndParent.hwndThis()
        , kCIDCtrls::pszCustClass
        , strTabText.pszBuffer()
        , areaTab
        , eStyles | tCIDCtrls::EWndStyles::ClippingTabChild
        , eExStyles | tCIDCtrls::EExWndStyles::ControlParent
        , widToUse
    );

    // Indicate we want to see child focus changes
    bWantsChildFocusNot(kCIDLib::True);
}


//
//  The tabber calls this once the tab window has been created and set up, and the derived
//  class has had time to create his controls and set an initial focus and all that. This
//  is necessary because SelectChange(), below, is only called on a new tab if it is to be
//  activated, and generally they are not. So we need some way to handle things like the
//  last child focus.
//
tCIDLib::TVoid TTabWindow::PostTabInit()
{
    // For now we just need to remember if any child within us has the focus
    m_hwndLastFocus = facCIDCtrls().hwndChildWithFocus(hwndThis());
}



//
//  A wrapper called when the selection has changed. For now we are just passing it on
//  to the derived class, and insuring this new guy has the focus. This will try to
//  get the focus onto something useful if possible.
//
tCIDLib::TVoid
TTabWindow::SelectChange(const  tCIDLib::TBoolean bGetting
                        , const tCIDLib::TBoolean bTakeFocus)
{
    if (bGetting && bTakeFocus)
    {
        // Try to restore the previous focus, or find one if there isn't an old one that's valid
        if ((m_hwndLastFocus != kCIDCtrls::hwndInvalid)
        &&  facCIDCtrls().bIsWindowValid(m_hwndLastFocus))
        {
            facCIDCtrls().SetFocus(m_hwndLastFocus, kCIDLib::False);
        }
         else
        {
            //
            //  See if we can find one to give it to. If so, give it to him. Else, we
            //  just take it ourself.
            //
            TWindow* pwndFirstTab = pwndFindFirstTab();
            if (pwndFirstTab)
                pwndFirstTab->TakeFocus();

            // It still may not have accepted it. If not, take it ourself
            if (!bContainsFocus())
                TakeFocus();
        }
    }

    // Let derived classes know
    TabActivationChange(bGetting);
}


//
//  Store the calculated areas for this tab, for use by the tabbed window control.
//
tCIDLib::TVoid
TTabWindow::SetTabAreas(const   TArea&  areaTab
                        , const TArea&  areaClose
                        , const TArea&  areaText1
                        , const TArea&  areaText2)
{
    m_areaUs = areaTab;
    m_areaClose = areaClose;
    m_areaText1 = areaText1;
    m_areaText2 = areaText2;
}


// Just for use by the tabbed window control, to set our unique tab id
tCIDLib::TVoid TTabWindow::SetId(const tCIDLib::TCard4 c4ToSet)
{
    m_c4TabId = c4ToSet;
}




// ---------------------------------------------------------------------------
//  CLASS: TTabberWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class TTabberWnd : public TWindow
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTabberWnd();

        TTabberWnd(const TTabberWnd&) = delete;

        ~TTabberWnd();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTabberWnd& operator=(const TTabberWnd&) = delete;

        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bProcessAccel
        (
            const   tCIDLib::TVoid* const   pMsgData
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TArea& areaTabs() const;

        tCIDLib::TVoid ActivateAt
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid ActivateNextTab();

        tCIDLib::TBoolean bIsActiveTab
        (
            const   TTabWindow&             wndToCheck
        )   const;

        tCIDLib::TCard4 c4ActivateById
        (
            const   tCIDLib::TCard4         c4TabId
        );

        tCIDLib::TCard4 c4ActivateByName
        (
            const   TString&                strTabName
        );

        tCIDLib::TCard4 c4ActiveTabId() const;

        tCIDLib::TCard4 c4ActiveTabIndex() const;

        tCIDLib::TCard4 c4IdToIndex
        (
            const   tCIDLib::TCard4         c4Id
            , const tCIDLib::TBoolean       bThrowIfNot
        )   const;

        tCIDLib::TCard4 c4NameToIndex
        (
            const   TString&                strName
            , const tCIDLib::TBoolean       bThrowIfNot
        )   const;

        tCIDLib::TCard4 c4CreateTab
        (
                    TTabWindow* const       pwndToCreate
            , const TString&                strTabText
            , const tCIDLib::TCard4         c4At
            , const tCIDLib::TBoolean       bActivate
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EExWndStyles eExStyles
        );

        tCIDLib::TCard4 c4TabCount() const;

        tCIDLib::TVoid CloseAllTabs();

        tCIDLib::TVoid CloseCurrent();

        tCIDLib::TVoid CloseTab
        (
            const   TString&                strTabName
        );

        tCIDLib::TVoid CloseTabAt
        (
            const   tCIDLib::TCard4         c4At
        );

        tCIDLib::TVoid CloseTabById
        (
            const   tCIDLib::TCard4         c4Id
        );

        tCIDLib::TVoid Create
        (
                    TTabbedWnd&             wndParent
            , const tCIDCtrls::ETabWStyles  eTWStyles
        );

        const TTabWindow* pwndFindById
        (
            const   tCIDLib::TCard4         c4ToFind
        )   const;

        TTabWindow* pwndFindById
        (
            const   tCIDLib::TCard4         c4ToFind
        );

        const TTabWindow* pwndFindByName
        (
            const   TString&                strToFind
        )   const;

        TTabWindow* pwndFindByName
        (
            const   TString&                strToFind
        );

        const TTabWindow* pwndSelected() const;

        TTabWindow* pwndSelected();

        const TTabWindow* pwndTabAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        TTabWindow* pwndTabAt
        (
            const   tCIDLib::TCard4         c4At
        );

        // For the parent window to tell us his size changed
        tCIDLib::TVoid ParentSzChanged
        (
            const   TArea&                  areaNew
        );

        tCIDLib::TVoid RedrawActiveTabHdr();

        tCIDLib::TVoid RedrawTabHdrById
        (
            const   tCIDLib::TCard4         c4Id
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bClick
        (
            const   tCIDCtrls::EMouseButts  eButton
            , const tCIDCtrls::EMouseClicks eClickType
            , const TPoint&                 pntAt
            , const tCIDLib::TBoolean       bCtrlShift
            , const tCIDLib::TBoolean       bShift
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

        tCIDLib::TVoid Destroyed() override;

        tCIDLib::TVoid GettingFocus() override;

        tCIDLib::TVoid MenuCommand
        (
            const   tCIDLib::TResId         ridItem
            , const tCIDLib::TBoolean       bChecked
            , const tCIDLib::TBoolean       bEnabled
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        // -------------------------------------------------------------------
        typedef TRefVector<TTabWindow>      TTabList;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTwoLineMode() const;

        tCIDLib::TVoid CalcAreas
        (
            const   TArea&                  areaNew
        );

        tCIDLib::TVoid CalcHeaderAreas();

        tCIDLib::TVoid DoClose
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid DrawTabHeaders
        (
                    TGraphDrawDev&          gdevTar
            , const TArea&                  areaInvalid
        );

        tCIDLib::TVoid ShowMenu();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_accelTabs
        //      We have some hot keys we need to respond to, if the client calls our
        //      process accel table method.
        //
        //  m_areaDown
        //  m_areaHeaders
        //  m_areaTabs
        //  m_areaTop
        //      The calculate area of the headers and tabs. The Top area is the whole top
        //      part, inside any border. The Headers area is the top, minus the bit to the
        //      right end where the down button goes, if there's room to display it. If not
        //      Headers and Top are the same. Tabs is the bottom part, inside any border.
        //      If we have room for the down button, its area is in the Down one.
        //
        //      THESE ARE relative to our origin! Mostly this isn't an issue, but we do
        //      have to remember that setting the size/pos of us or the tab windows is
        //      relative to our parent, not to us.
        //
        //  m_bDownButton
        //      During area calcs, we remember if there's enough room to show the down
        //      button. This is used during drawing to display it if we can.
        //
        //  m_bmpClose
        //      The little X bitmap we display to the right of the tabs' headers, for
        //      closing the tab.
        //
        //  m_c4ChangeIndWidth
        //      We pre-calculate the width required for our changed state indicator,
        //      which is a space plus an asterisk.
        //
        //  m_c4CurTabId
        //      The unique id of the currently selected tab. If no tabs exist, it will be
        //      c4MaxCard.
        //
        //  m_c4BoldCY
        //  m_c4NormCY
        //  m_c4SmallCY
        //      We allow one or two lines of text in the tabs, and these are the
        //      precalculated heights of them. When using a single line, it uses our
        //      normal font. When two line, the top is bold and shows the tab text, and
        //      the bottom is in our small font and shows a secondary line, usually a
        //      type indicator.
        //
        //  m_c4NextId
        //      A running counter we use to set a unique id on each tab. We start it at
        //      and wrap back to 1 though that would never happen in the real world.
        //
        //  m_c4TabWidth
        //      We used fixed size tabs, and pre-calc the width. We have to include space
        //      for the close bitmap as well, for those tabs that can be closed.
        //
        //  m_colTabs
        //      The list of tab windows that we are controlling. We adopt them so
        //      they must be dynamically allocated.
        //
        //  m_eTWStyles
        //      Our parent class passes us his tabbed window styles so we can check them
        //      as requierd.
        //
        //  m_pwndParent
        //      A pointer to our parent window so that we can manage the actual tabs,
        //      which are children of him.
        //
        //  m_rgbBgn
        //  m_rgbText
        //      We pre-load our bgn and text colors for efficiency.
        //
        //  m_szCloseBmp
        //      The size of the close bitmap, so we don't have to keep getting it back out.
        //
        //  m_wndTabBag
        //      A zero sized, hidden window into which we put unused tabs, to prevent them
        //      from causing drawing artifacts, which they will even if hidden. It's probably
        //      something to do with the composited drawing scheme.
        // -------------------------------------------------------------------
        TAccelTable             m_accelTabs;
        TArea                   m_areaDown;
        TArea                   m_areaHeaders;
        TArea                   m_areaTabs;
        TArea                   m_areaTop;
        tCIDLib::TBoolean       m_bDownButton;
        TBitmap                 m_bmpClose;
        TBitmap                 m_bmpDown;
        tCIDLib::TCard4         m_c4ChangeIndWidth;
        tCIDLib::TCard4         m_c4CurTabId;
        tCIDLib::TCard4         m_c4BoldCY;
        tCIDLib::TCard4         m_c4NormCY;
        tCIDLib::TCard4         m_c4SmallCY;
        tCIDLib::TCard4         m_c4NextId;
        TTabList                m_colTabs;
        tCIDCtrls::ETabWStyles  m_eTWStyles;
        TTabbedWnd*             m_pwndParent;
        TRGBClr                 m_rgbBgn;
        TRGBClr                 m_rgbText;
        TSize                   m_szCloseBmp;
        TSize                   m_szDownBmp;
        TGenericWnd             m_wndTabBag;
};




// ---------------------------------------------------------------------------
//  CLASS: TTabberWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TTabberWnd: Constructors and Destructor
// ---------------------------------------------------------------------------
TTabberWnd::TTabberWnd() :

    TWindow(kCIDLib::False)
    , m_bDownButton(kCIDLib::False)
    , m_c4CurTabId(kCIDLib::c4MaxCard)
    , m_c4BoldCY(0)
    , m_c4NormCY(0)
    , m_c4SmallCY(0)
    , m_c4NextId(1)
    , m_colTabs(tCIDLib::EAdoptOpts::Adopt)
    , m_pwndParent(nullptr)
{
}

TTabberWnd::~TTabberWnd()
{
}


// ---------------------------------------------------------------------------
// TTabberWnd: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TTabberWnd::bProcessAccel(const tCIDLib::TVoid* const pMsgData) const
{
    // See if we have a current tab. If not, nothing more to do
    const TTabWindow* pwndCur = pwndSelected();
    if (!pwndCur)
        return kCIDLib::False;

    // If we do, and he contains the focus, then give him a shot at it
    if (pwndCur->bContainsFocus() && pwndCur->bProcessAccel(pMsgData))
        return kCIDLib::True;

    //
    //  We assume we are only called if we contain the focus, so give our
    //  own table a shot at it.
    //
    return m_accelTabs.bProcessAccel(*this, pMsgData);
}


// ---------------------------------------------------------------------------
// TTabberWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------

const TArea& TTabberWnd::areaTabs() const
{
    return m_areaTabs;
}


//
//  Activate a tab by index. This is ultimately the only activation method. All the
//  others work through this.
//
//  NOTE THAT we may have to move the tab if it is not visible.
//
tCIDLib::TVoid TTabberWnd::ActivateAt(const tCIDLib::TCard4 c4NewIndex)
{
    // Get the window at that index, which will also validate the index
    TTabWindow* pwndNew = pwndTabAt(c4NewIndex);

    // If it's id is the same as the current tab id, we do nothing
    if (pwndNew->c4TabId() == m_c4CurTabId)
        return;

    // OK, we need to do it. Get the index of the current tab
    const tCIDLib::TCard4 c4OldInd = c4ActiveTabIndex();

    //
    //  If there was an active one, hide it and let it know it's losing selection.
    //
    if (c4OldInd != kCIDLib::c4MaxCard)
    {
        m_colTabs[c4OldInd]->SelectChange(kCIDLib::False, kCIDLib::False);
        m_colTabs[c4OldInd]->SetParent(m_wndTabBag);
    }

    // We may have to move it if it's not visible
    tCIDLib::TCard4 c4RealIndex = c4NewIndex;
    if (!pwndNew->bTabIsVisible())
    {
        c4RealIndex = 0;
        if (tCIDLib::bAllBitsOn(m_eTWStyles, tCIDCtrls::ETabWStyles::FixedFirst)
        &&  (m_colTabs.c4ElemCount() > 1))
        {
            c4RealIndex = 1;
        }

        // Move it to the correct position in the collection and recalc
        if (c4RealIndex != c4NewIndex)
        {
            m_colTabs.MoveItem(c4NewIndex, c4RealIndex);
            CalcHeaderAreas();
        }
    }

    //
    //  Show the new one. Update the current tab id first. We show it by moving it from
    //  the tab bag parent, to our parent. Then we make sure it's the right size
    //
    m_c4CurTabId = pwndNew->c4TabId();
    pwndNew->SetSizePos(m_areaTabs, kCIDLib::True);
    pwndNew->SetParent(*m_pwndParent);

    // Force a redraw to get the new guy visible
    m_pwndParent->Redraw(tCIDCtrls::ERedrawFlags::Full);

    // Let the new guy know he's selected and force new focus selection
    pwndNew->SelectChange(kCIDLib::True, kCIDLib::True);
}


//
//  This is called to move to the next tab in the list. Note that we only move through the
//  visible tabs.
//
tCIDLib::TVoid TTabberWnd::ActivateNextTab()
{
    const tCIDLib::TCard4 c4Count = m_colTabs.c4ElemCount();
    if (!c4Count)
        return;

    // Figure out the next one, remember the current one for notifications below
    const tCIDLib::TCard4 c4OldId = c4ActiveTabIndex();
    tCIDLib::TCard4 c4Index = c4OldId;
    if (c4Index == kCIDLib::c4MaxCard)
    {
        //
        //  Unlikely to happen, but it means we have tabs but none is activated, so just
        //  move to the first one.
        //
        c4Index = 0;
    }
     else
    {
        c4Index++;
        if (c4Index >= c4Count)
            c4Index = 0;

        // If this one is not visible, go back to zero
        if (!m_colTabs[c4Index]->bTabIsVisible())
            c4Index = 0;
    }

    // Get this guy's id out now since ActivateAt can change the index
    const tCIDLib::TCard4 c4NewId = m_colTabs[c4Index]->c4TabId();

    // Now just call the regular tab activiation method
    ActivateAt(c4Index);

    //
    //  In this case we will send a notification, since they didn't explicitly activate a
    //  particular tab.
    //
    SendSyncNotify
    (
        TTabEventInfo
        (
            c4NewId, c4OldId, tCIDCtrls::ETabWEvents::Change, *m_pwndParent
        )
        , TTabbedWnd::nidEventId
    );
}


// Returns true if the passed window is the active tab
tCIDLib::TBoolean TTabberWnd::bIsActiveTab(const TTabWindow& wndToCheck) const
{
    const TTabWindow* pwndCur = pwndSelected();
    if (!pwndCur)
        return kCIDLib::False;

    //
    //  To be safe, we make sure both handles are valid and equal, and that
    //  they ahve the same tab id.
    //
    return
    (
        (pwndCur->hwndSafe() == wndToCheck.hwndSafe())
        && (pwndCur->c4TabId() == wndToCheck.c4TabId())
    );
}


//
//  Find the tab with the given tab id and active it, if it is found. It will return the
//  index of the tab if it is found, else max card.
//
tCIDLib::TCard4 TTabberWnd::c4ActivateById(const tCIDLib::TCard4 c4TabId)
{
    // Find the index of this one
    const tCIDLib::TCard4 c4Index = c4IdToIndex(c4TabId, kCIDLib::True);

    // And call the other version if found
    if (c4Index != kCIDLib::c4MaxCard)
    {
        ActivateAt(c4Index);
        return c4IdToIndex(c4TabId, kCIDLib::True);
    }

    // Just return max card
    return c4Index;
}


//
//  Find the named tab and activate it. This will throw if the tab name is not
//  found. If found, it returns teh index of the tab found.
//
tCIDLib::TCard4 TTabberWnd::c4ActivateByName(const TString& strTabName)
{
    // Look up the index then call the other version
    const tCIDLib::TCard4 c4Index = c4NameToIndex(strTabName, kCIDLib::True);
    ActivateAt(c4Index);

    // It may have been moved to make it visible, so loo up the index again
    return c4NameToIndex(strTabName, kCIDLib::True);
}


// Returns the tab id of the active tab, or max card if none
tCIDLib::TCard4 TTabberWnd::c4ActiveTabId() const
{
    const TTabWindow* pwndCur = pwndSelected();
    if (!pwndCur)
        return kCIDLib::c4MaxCard;
    return pwndCur->c4TabId();
}


// Return the index of the active tab, or max card if there aren't any tabs
tCIDLib::TCard4 TTabberWnd::c4ActiveTabIndex() const
{
    if (m_c4CurTabId == kCIDLib::c4MaxCard)
        return m_c4CurTabId;

    return c4IdToIndex(m_c4CurTabId, kCIDLib::False);
}


//
//  Look up a tab by id and return its index. If not found, it can either
//  throw or return max card.
//
tCIDLib::TCard4
TTabberWnd::c4IdToIndex(const   tCIDLib::TCard4     c4Id
                        , const tCIDLib::TBoolean   bThrowIfNot) const
{
    const tCIDLib::TCard4 c4Count = m_colTabs.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colTabs[c4Index]->c4TabId() == c4Id)
            return c4Index;
    }

    if (bThrowIfNot)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcTab_IdNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , TCardinal(c4Id)
        );
    }
    return kCIDLib::c4MaxCard;
}


//
//  Look up a tab by name and return its index. If not found, it can either throw
//  or return max card.
//
tCIDLib::TCard4
TTabberWnd::c4NameToIndex(  const   TString&            strName
                            , const tCIDLib::TBoolean   bThrowIfNot) const
{
    const tCIDLib::TCard4 c4Count = m_colTabs.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colTabs[c4Index]->strName() == strName)
            return c4Index;
    }

    if (bThrowIfNot)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcTab_NameNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strName
        );
    }
    return kCIDLib::c4MaxCard;
}


// Return the count of tabs
tCIDLib::TCard4 TTabberWnd::c4TabCount() const
{
    return m_colTabs.c4ElemCount();
}


// Close all tabs, destroying and deleting the associated windows
tCIDLib::TVoid TTabberWnd::CloseAllTabs()
{
    // We no longer have an active window
    m_c4CurTabId = kCIDLib::c4MaxCard;

    // Now destroy all of the tab windows
    const tCIDLib::TCard4 c4Count = m_colTabs.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        try
        {
            m_colTabs[c4Index]->Destroy();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        catch(...)
        {
        }
    }

    // Delete the actual objects now
    try
    {
        m_colTabs.RemoveAll();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    catch(...)
    {
    }

    // Send a tab change notification with an id of max card for new and old tabs
    SendSyncNotify
    (
        TTabEventInfo
        (
            kCIDLib::c4MaxCard, kCIDLib::c4MaxCard, tCIDCtrls::ETabWEvents::Change, *m_pwndParent
        )
        , TTabbedWnd::nidEventId
    );
}


//
//  If we have a current tab, close it. WE assume, since client code is calling this,
//  that they know it's ok to close, even if it's marked not to be closeable by way of
//  user action.
//
tCIDLib::TVoid TTabberWnd::CloseCurrent()
{
    if (m_c4CurTabId == kCIDLib::c4MaxCard)
        return;

    const tCIDLib::TCard4 c4Index = c4IdToIndex(m_c4CurTabId, kCIDLib::True);
    CloseTabAt(c4Index);
}


//
//  Find the named tab, tab at, or tab with the indicated tab id and close it,
//  destroying and deleting the associated window.
//
//  These are unconditional. Client code is asking us to do this so we assume he's
//  cleaned up and saved and all that. And we assume that, even if marked as not
//  closeable via user action that we should still close it.
//
tCIDLib::TVoid TTabberWnd::CloseTab(const TString& strTabName)
{
    tCIDLib::TCard4 c4Index = c4NameToIndex(strTabName, kCIDLib::True);
    CloseTabAt(c4Index);
}

tCIDLib::TVoid TTabberWnd::CloseTabAt(const tCIDLib::TCard4 c4At)
{
    CIDAssert(c4At != kCIDLib::c4MaxCard, L"The incoming tab index was -1");

    // It can be close so orphan the target tab out of the list.
    TTabWindow* pwndClose = m_colTabs.pobjOrphanAt(c4At);
    const tCIDLib::TCard4 c4CloseId = pwndClose->c4TabId();


    //
    //  If we are destroying the current tab, then select another if we can. Assume no
    //  new tab initially.
    //
    tCIDLib::TCard4 c4NewId = kCIDLib::c4MaxCard;
    if (c4CloseId == m_c4CurTabId)
    {
        const tCIDLib::TCard4 c4TabsLeft = m_colTabs.c4ElemCount();

        // If any left, try to keep the same position. Else move back one
        if (c4TabsLeft)
        {
            tCIDLib::TCard4 c4NewIndex = c4At;
            if (c4NewIndex >= c4TabsLeft)
                c4NewIndex = c4TabsLeft - 1;

            c4NewId = m_colTabs[c4NewIndex]->c4TabId();
        }
    }
     else
    {
        // We are just keeping the current one
        c4NewId = m_c4CurTabId;
    }

    //
    //  If we couldn't get a new one, then set our current tab id to max card. Else,
    //  if we actually selected a new one, then activate it.
    //
    if (c4NewId == kCIDLib::c4MaxCard)
        m_c4CurTabId = kCIDLib::c4MaxCard;
    else if (c4NewId != m_c4CurTabId)
        c4ActivateById(c4NewId);

    // Recalculate the header positions
    CalcHeaderAreas();

    //
    //  Now that we have the new id set, we can hide the one we are closing them. WE HAVE TO
    //  do this afterwards, because this can cause a focus change, which will cause us to
    //  try to do some focus change related stuff. If do this before we get the new id set
    //  it could cause that focus reaction code to see a bad current id.
    //
    pwndClose->SetVisibility(kCIDLib::False);

    //
    //  And now let's destroy the tab we are closing. It needs to be out of the list before
    //  we destroy it, to avoid the possibility of events coming back to us (such as focus
    //  change events) causing us to try to look up an already dead tab.
    try
    {
        pwndClose->Destroy();
        delete pwndClose;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    // Force a redraw
    m_pwndParent->Redraw(m_pwndParent->areaClient(), tCIDCtrls::ERedrawFlags::Full);

    //
    //  Send a notification. Though this may not be user input driven, it's not changing
    //  because of an explicit request on their part for a specific tab. If there's no
    //  tab left, we send max card as the id.
    //
    SendSyncNotify
    (
        TTabEventInfo
        (
            m_c4CurTabId, kCIDLib::c4MaxCard, tCIDCtrls::ETabWEvents::Change, *m_pwndParent
        )
        , TTabbedWnd::nidEventId
    );
}

tCIDLib::TVoid TTabberWnd::CloseTabById(const tCIDLib::TCard4 c4Id)
{
    // Just call the index based one, translating the id to an index
    const tCIDLib::TCard4 c4Index = c4IdToIndex(c4Id, kCIDLib::False);

    // It could have already gotten destroyed, so be tolerant
    if (c4Index != kCIDLib::c4MaxCard)
        CloseTabAt(c4Index);
}


//
//  Create ourself with the indicated area and styles.
//
tCIDLib::TVoid
TTabberWnd::Create(TTabbedWnd& wndParent, const tCIDCtrls::ETabWStyles eTWStyles)
{
    // Store our parent's styles and a pointer to our parent
    m_eTWStyles = eTWStyles;
    m_pwndParent = &wndParent;

    //
    //  Initial area doesn't matter. We have to be created before we can calculate the
    //  areas.
    //
    CreateWnd
    (
        wndParent.hwndThis()
        , kCIDCtrls::pszCustClass
        , kCIDLib::pszEmptyZStr
        , TArea(0, 0, 64, 64)
        , tCIDCtrls::EWndStyles::VisGroupChild
        , tCIDCtrls::EExWndStyles::None
        , kCIDCtrls::widFirstCtrl
    );
}


//
//  Adds a new tab window to our list. The caller indicates where to insert the tab.
//  If max card, we add it to the end. The tab window's text is the display text for
//  the tab's header.
//
//  We return the id of the tab.
//
tCIDLib::TCard4
TTabberWnd::c4CreateTab(        TTabWindow* const       pwndToCreate
                        , const TString&                strTabText
                        , const tCIDLib::TCard4         c4At
                        , const tCIDLib::TBoolean       bActivate
                        , const tCIDCtrls::EWndStyles   eStyles
                        , const tCIDCtrls::EExWndStyles eExStyles)
{
    // Put a janitor on it until we get it stored away
    TJanitor<TTabWindow> janNew(pwndToCreate);

    // Make sure it's unique
    if (pwndToCreate->strName().bIsEmpty()
    ||  (c4NameToIndex(pwndToCreate->strName(), kCIDLib::False) != kCIDLib::c4MaxCard))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcTab_BadTabName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , pwndToCreate->strName()
        );
    }

    //
    //  Figure out where to insert it. Note that, if they ask us to activate it, then
    //  we need to insure it's visible, so it could get moved.
    //
    const tCIDLib::TCard4 c4OrgCount = m_colTabs.c4ElemCount();
    tCIDLib::TCard4 c4InsertAt;
    if (c4At == kCIDLib::c4MaxCard)
        c4InsertAt = c4OrgCount;
    else
        c4InsertAt = c4At;

    // Assign it the next available id
    pwndToCreate->SetId(m_c4NextId++);

    // Handle id wrap, though very unlikely
    if (!m_c4NextId)
        m_c4NextId++;

    //
    //  OK, let's create the window now. We set the window id to the first control id plus
    //  the id we assigned. The parent is initially our tab bag window to keep it hidden
    //  away. He will make sure control parent is enabled, which we always want for tabs.
    //
    pwndToCreate->CreateTabWnd
    (
        m_wndTabBag
        , m_pwndParent
        , strTabText
        , m_areaTabs
        , kCIDCtrls::widFirstCtrl + pwndToCreate->c4TabId()
        , eStyles | tCIDCtrls::EWndStyles::ClippingVisTabChild
        , eExStyles
    );

    // Insert it, orphaning it out of the janitor
    m_colTabs.InsertAt(janNew.pobjOrphan(), c4InsertAt);

    // Recalculate tab header areas
    CalcHeaderAreas();

    // Tell the tab it's ready now
    pwndToCreate->PostTabInit();

    //
    //  If this was the first one, or they asked us to, activate it. This will force it
    //  visible if needed.
    //
    if (!c4OrgCount || bActivate)
    {
        ActivateAt(c4InsertAt);

        //
        //  And in this case report it as a change since it wasn't an explicit activation.
        //
        //  NOTE that the activation could change the index, but we don't use it here
        //  at this point.
        //
        SendSyncNotify
        (
            TTabEventInfo
            (
                pwndToCreate->c4TabId()
                , kCIDLib::c4MaxCard
                , tCIDCtrls::ETabWEvents::Change
                , *m_pwndParent
            )
            , TTabbedWnd::nidEventId
        );
    }
     else
    {
        // Force a redraw of our tabs area to get this one visible
        Redraw(tCIDCtrls::ERedrawFlags::Full);
    }

    // Return the new guy's tab id
    return pwndToCreate->c4TabId();
}


// Find a tab by name
const TTabWindow* TTabberWnd::pwndFindByName(const TString& strToFind) const
{
    const tCIDLib::TCard4 c4Index = c4NameToIndex(strToFind, kCIDLib::False);
    if (c4Index == kCIDLib::c4MaxCard)
        return nullptr;
    return m_colTabs[c4Index];
}

TTabWindow* TTabberWnd::pwndFindByName(const TString& strToFind)
{
    const tCIDLib::TCard4 c4Index = c4NameToIndex(strToFind, kCIDLib::False);
    if (c4Index == kCIDLib::c4MaxCard)
        return nullptr;
    return m_colTabs[c4Index];
}


// Find a tab by id
const TTabWindow* TTabberWnd::pwndFindById(const tCIDLib::TCard4 c4Id) const
{
    const tCIDLib::TCard4 c4Index = c4IdToIndex(c4Id, kCIDLib::False);
    if (c4Index == kCIDLib::c4MaxCard)
        return nullptr;
    return m_colTabs[c4Index];
}

TTabWindow* TTabberWnd::pwndFindById(const tCIDLib::TCard4 c4Id)
{
    const tCIDLib::TCard4 c4Index = c4IdToIndex(c4Id, kCIDLib::False);
    if (c4Index == kCIDLib::c4MaxCard)
        return nullptr;
    return m_colTabs[c4Index];
}


// Return a pointer to the current tab, or null if none
const TTabWindow* TTabberWnd::pwndSelected() const
{
    if (m_c4CurTabId == kCIDLib::c4MaxCard)
        return nullptr;

    const tCIDLib::TCard4 c4Index = c4IdToIndex(m_c4CurTabId, kCIDLib::True);
    if (c4Index >= m_colTabs.c4ElemCount())
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcTab_CurTabIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , TCardinal(c4Index)
            , TCardinal(m_colTabs.c4ElemCount())
        );
    }
    return m_colTabs[c4Index];
}


TTabWindow* TTabberWnd::pwndSelected()
{
    if (m_c4CurTabId == kCIDLib::c4MaxCard)
        return nullptr;

    const tCIDLib::TCard4 c4Index = c4IdToIndex(m_c4CurTabId, kCIDLib::True);
    if (c4Index >= m_colTabs.c4ElemCount())
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcTab_CurTabIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , TCardinal(c4Index)
            , TCardinal(m_colTabs.c4ElemCount())
        );
    }
    return m_colTabs[c4Index];
}


// Directly access tabs by their index
const TTabWindow* TTabberWnd::pwndTabAt(const tCIDLib::TCard4 c4At) const
{
    if (c4At >= m_colTabs.c4ElemCount())
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcTab_BadTabIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4At)
            , TCardinal(m_colTabs.c4ElemCount())
            , TCardinal(widThis())
        );
    }

    return m_colTabs[c4At];
}

TTabWindow* TTabberWnd::pwndTabAt(const tCIDLib::TCard4 c4At)
{
    if (c4At >= m_colTabs.c4ElemCount())
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcTab_BadTabIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4At)
            , TCardinal(m_colTabs.c4ElemCount())
            , TCardinal(widThis())
        );
    }
    return m_colTabs[c4At];
}


//
//  Our parent tells us when he changes size. We recalc our areas and move things around
//  as required. We get the area of his client area (but with zero origin.) So this is
//  where the client content will go if he has a border.
//
//  This works well enough since size/pos stuff works relative to his client area, and our
//  own drawing works relative to ours. So we can use the areas without worrying about
//  these things.
//
tCIDLib::TVoid TTabberWnd::ParentSzChanged(const TArea& areaNew)
{
    // Recalculate our areas
    CalcAreas(areaNew);

    // Recalculate header areas
    CalcHeaderAreas();

    // Fit us to our area in the top
    SetSizePos(m_areaTop, kCIDLib::True);

    // Redraw our area
    m_pwndParent->Redraw(tCIDCtrls::ERedrawFlags::Full);

    // If there's an active tab, resize it to the tabs area of our parent
    TTabWindow* pwndSel = pwndSelected();
    if (pwndSel)
        pwndSel->SetSizePos(m_areaTabs, kCIDLib::True);
}


//
//  Our parent sometimes wants us to redraw the tab header for the active tab, mainly
//  when it sees that the focus has moved into or out of it.
//
tCIDLib::TVoid TTabberWnd::RedrawActiveTabHdr()
{
    const TTabWindow* pwndActive = pwndSelected();
    if (pwndActive)
        Redraw(pwndActive->areaTab(), tCIDCtrls::ERedrawFlags::Full);
}


// Redraw a specific tab's header, if it is visible
tCIDLib::TVoid TTabberWnd::RedrawTabHdrById(const tCIDLib::TCard4 c4Id)
{
    const TTabWindow* pwndTar = pwndFindById(c4Id);
    if (pwndTar)
        Redraw(pwndTar->areaTab(), tCIDCtrls::ERedrawFlags::Full);
}


// ---------------------------------------------------------------------------
// TTabberWnd: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  We override this to handle a click in the tab. If a tab was hit, we send out
//  a notification about it. It's async, so that by the time the client gets it,
//  whatever was done has already happened and he won't interfere. We only send
//  it if this is the active tab, so that they don't accidentally close a tab
//  by just trying to activate it.
//
tCIDLib::TBoolean
TTabberWnd::bClick( const   tCIDCtrls::EMouseButts  eButton
                    , const tCIDCtrls::EMouseClicks eClickType
                    , const TPoint&                 pntAt
                    , const tCIDLib::TBoolean       bCtrlShift
                    , const tCIDLib::TBoolean       bShift)
{
    // If it's a left button down, unshifted, let's check it
    if ((eButton == tCIDCtrls::EMouseButts::Left)
    &&  (eClickType == tCIDCtrls::EMouseClicks::Down)
    &&  !bCtrlShift
    &&  !bShift)
    {
        // First check the down area. If so, process the drop down menu and return
        if (m_areaDown.bContainsPoint(pntAt))
        {
            ShowMenu();
            return kCIDLib::True;
        }

        // Remember the current tab
        const tCIDLib::TCard4 c4OldId = m_c4CurTabId;

        //
        //  Go through all the visible tabs and see if we hit any and what we hit if so.
        //  Send a notification if needed. Any that aren't visible will have had their
        //  areas zeroed out, so we know when we hit the last possible one.
        //
        const tCIDLib::TCard4 c4Count = m_colTabs.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            TTabWindow& wndCur = *m_colTabs[c4Index];
            tCIDCtrls::ETabWEvents eEvent = wndCur.eHitTest(pntAt);

            // If it returns count, then we are past the visible ones so stop
            if (eEvent == tCIDCtrls::ETabWEvents::Count)
                break;

            // If nothing, then go try another one
            if (eEvent == tCIDCtrls::ETabWEvents::None)
                continue;

            // We got something so process it and break out
            if (eEvent == tCIDCtrls::ETabWEvents::Hit)
            {
                //
                //  If it's the current tab, just see if it contains the focus. If not, do
                //  a select change on it, to force that. Else, we try to bring the new one
                //  forward.
                //
                if (wndCur.c4TabId() == m_c4CurTabId)
                {
                    // Force a selection change and focus taking
                    wndCur.SelectChange(kCIDLib::True, kCIDLib::True);
                }
                 else
                {
                    // First send a synchronous pre-change to see if they want to allow it
                    TTabEventInfo wnotPreChange
                    (
                        m_c4CurTabId
                        , wndCur.c4TabId()
                        , tCIDCtrls::ETabWEvents::PreChange
                        , *m_pwndParent
                    );
                    SendSyncNotify(wnotPreChange, TTabbedWnd::nidEventId);

                    // If they allowed it let's do it
                    if (wnotPreChange.bAllow())
                    {
                        // NOTE this could change the index but we don't use it anymore
                        ActivateAt(c4Index);

                        // Send a change event instead
                        SendSyncNotify
                        (
                            TTabEventInfo
                            (
                                wndCur.c4TabId()
                                , c4OldId
                                , tCIDCtrls::ETabWEvents::Change
                                , *m_pwndParent
                            )
                            , TTabbedWnd::nidEventId
                        );
                    }
                }
            }
             else if (eEvent == tCIDCtrls::ETabWEvents::Close)
            {
                DoClose(c4Index);
            }
             else
            {
                // Shouldn't get here
                TOkBox msgbErr(L"Tabbed Window", L"Unknown tab hit test result");
                msgbErr.ShowIt(*this);
            }

            // We got something so break out
            break;
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TTabberWnd::bCreated()
{
    //
    //  Set up the height values for our two lines.
    //
    //  We also lculate the width required for a space and asterisk. This is
    //  displayed if the tab is in changed state, so we always reserve space for
    //  it in each tab, so we don't need to redraw all tabs just because one of
    //  them changed its changed state. We use the bold font for worst case.
    //
    TGraphWndDev gdevTmp(*this);
    {
        TFontJanitor janSmall(&gdevTmp, &TGUIFacility::gfontDefSmall());
        TFontMetrics fmtrSmall(gdevTmp);
        m_c4SmallCY = fmtrSmall.c4Height() + fmtrSmall.c4ExternalLeading();
    }
    {
        TFontJanitor janBold(&gdevTmp, &TGUIFacility::gfontDefBold());
        TFontMetrics fmtrBold(gdevTmp);
        m_c4BoldCY = fmtrBold.c4Height() + fmtrBold.c4ExternalLeading();

        m_c4ChangeIndWidth = gdevTmp.areaString(L"*").c4Width() + 4;
    }
    {
        TFontJanitor janNorm(&gdevTmp, &TGUIFacility::gfontDefNorm());
        TFontMetrics fmtrNorm(gdevTmp);
        m_c4NormCY = fmtrNorm.c4Height() + fmtrNorm.c4ExternalLeading();
    }

    //
    //  Create our tab bag to hold the hidden tabs. We give it the first child control.
    //  All of the tabs get this plus 1 and beyond, so there's no conflict.
    //
    m_wndTabBag.CreateGenWnd
    (
        *this
        , TArea(0, 0, 0, 0)
        , tCIDCtrls::EWndStyles::Child
        , tCIDCtrls::EExWndStyles::None
        , kCIDCtrls::widFirstCtrl
    );

    // Load our close bitmap
    m_bmpClose = facCIDCtrls().bmpLoadSmallAppImg(L"V5 Black X Mark");
    m_szCloseBmp = m_bmpClose.szBitmap();

    // And our down area for the drop down, which is a system bitmap
    m_bmpDown = facCIDCtrls().bmpLoadSysBmp(tCIDCtrls::ESysBmps::DnArrow);
    m_szDownBmp = m_bmpDown.szBitmap();

    // Calculate our areas (load the bitmap first!)
    CalcAreas(m_pwndParent->areaWndSize());

    // Set us to the right place now within our parent
    SetSizePos(m_areaTop, kCIDLib::False);

    // Set up our accelerator table
    tCIDLib::TStrList colKeys;
    tCIDLib::TCardList fcolCmds;

    colKeys.objAdd(L"Ctrl-F4");
    fcolCmds.c4AddElement(CIDCtrls_TabbedWnd::ridMenu_Close);
    colKeys.objAdd(L"Ctrl-F6");
    fcolCmds.c4AddElement(CIDCtrls_TabbedWnd::ridMenu_NextTab);


    if (tCIDLib::bAllBitsOn(m_eTWStyles, tCIDCtrls::ETabWStyles::TopLevel))
    {
        colKeys.objAdd(L"Ctrl-Shift--F4");
        fcolCmds.c4AddElement(CIDCtrls_TabbedWnd::ridMenu_Close);
        colKeys.objAdd(L"Ctrl-Shift-F6");
        fcolCmds.c4AddElement(CIDCtrls_TabbedWnd::ridMenu_NextTab);
    }

    m_accelTabs.Create(colKeys, fcolCmds);

    return kCIDLib::True;
}


tCIDLib::TBoolean TTabberWnd::bEraseBgn(TGraphDrawDev& gdevToUse)
{
    gdevToUse.Fill(m_areaTop, facCIDGraphDev().rgbDarkWhite);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TTabberWnd::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    // Draw our divider under the tabs
    TPoint pntFrom(0, m_areaHeaders.i4Bottom());
    TPoint pntTo(m_areaTabs.i4Right(), m_areaHeaders.i4Bottom());
    gdevToUse.DrawLine(pntFrom, pntTo, facCIDGraphDev().rgbPaleGrey);

    // Go through our tabs and draw as many as will fit the available horizontal area.
    DrawTabHeaders(gdevToUse, areaUpdate);

    // If there is space, draw the down button
    if (m_bDownButton && m_areaDown.bIntersects(areaUpdate))
        gdevToUse.DrawBitmap(m_bmpDown, m_areaDown.pntUL());

    return kCIDLib::True;
}


//  Clean up our tab windows. We adopt them. In theory we don't have to do that
//  since they are dynamically allocated and TWindow will clean them up. but we
//  want to provide better logging of the error location and such.
//
tCIDLib::TVoid TTabberWnd::Destroyed()
{
    // Clean up the tabs
    CloseAllTabs();

    // And destroy the tab bag window
    m_wndTabBag.Destroy();
}


//
//  If we are getting the focus, just pass it on to the active tab.
//
tCIDLib::TVoid TTabberWnd::GettingFocus()
{
    TTabWindow* pwndActive = pwndSelected();
    if (pwndActive)
        pwndActive->SelectChange(kCIDLib::True, kCIDLib::True);
}


// Generated by our handling accelerator events
tCIDLib::TVoid
TTabberWnd::MenuCommand(const   tCIDLib::TResId     ridItem
                        , const tCIDLib::TBoolean   bChecked
                        , const tCIDLib::TBoolean   bEnabled)
{
    if (ridItem == CIDCtrls_TabbedWnd::ridMenu_Close)
        DoClose(c4ActiveTabIndex());
    else if (ridItem == CIDCtrls_TabbedWnd::ridMenu_NextTab)
        ActivateNextTab();
}


// ---------------------------------------------------------------------------
// TTabberWnd: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TTabberWnd::bTwoLineMode() const
{
    return tCIDLib::bAllBitsOn(m_eTWStyles, tCIDCtrls::ETabWStyles::TwoLineHdr);
}


tCIDLib::TVoid TTabberWnd::CalcAreas(const TArea& areaNew)
{
    //
    //  Our top height is the larger of the height of the text lines plus padding or the
    //  close icon.
    //
    m_areaTop = areaNew;
    tCIDLib::TCard4 c4TabHeight;
    if (bTwoLineMode())
        c4TabHeight = m_areaTop.c4Height(m_c4BoldCY + m_c4SmallCY + 8);
    else
        c4TabHeight = m_areaTop.c4Height(m_c4NormCY + 6);

    // Is its less than the bitmap, take that plus some margin
    if (c4TabHeight < m_szCloseBmp.c4Height() + 5)
        c4TabHeight = m_szCloseBmp.c4Height() + 5;

    //
    //  Remove twice the area of the down button from the right side of the headers
    //  if we have enough. Set the down button flag to remember if we can draw it.
    //
    const tCIDLib::TCard4 c4Bmp2X = m_szDownBmp.c4Width() * 2;
    m_areaHeaders = m_areaTop;
    m_bDownButton = m_areaTop.c4Width() > c4Bmp2X;
    if (m_bDownButton)
    {
        m_areaHeaders.AdjustSize(-tCIDLib::TInt4(c4Bmp2X), 0);

        // Set up the down arrow area
        TArea areaDown = m_areaTop;
        areaDown.SetSize(c4Bmp2X, m_szDownBmp.c4Height());
        areaDown.RightJustifyIn(m_areaTop);
        m_areaDown.Set(TPoint::pntOrigin, m_szDownBmp);
        m_areaDown.CenterIn(areaDown);
    }

    //
    //  The tab area is the remainder below the header. If there isn't enough left to
    //  give the tabs a minimum height, then just set the tabs to the min and just let
    //  them get clipped.
    //
    m_areaTabs = areaNew;
    if (m_areaTop.c4Height() + 32 >= areaNew.c4Height())
    {
        m_areaTabs.c4Height(32);
    }
     else
    {
        m_areaTabs.i4Top(m_areaTop.i4Bottom() + 1);
        m_areaTabs.i4Bottom(areaNew.i4Bottom());
    }
}


//
//  Any time a tab is added or removed or moved, this is called to recalculate the areas
//  of the tabs. For those that are visible (and need to be drawn), we store the overall
//  area, the area of the bitmap (if any), and the area of the text strings. For those not
//  visible, we set them all to empty areas to indicate this.
//
tCIDLib::TVoid TTabberWnd::CalcHeaderAreas()
{
    const tCIDLib::TCard4 c4Count = m_colTabs.c4ElemCount();
    if (!c4Count)
        return;

    // Remember some info for fast access
    const tCIDLib::TBoolean bTwoLine = bTwoLineMode();

    TArea areaBmp;
    TArea areaText1;
    TArea areaText2;
    TArea areaFill;
    TArea areaDraw;

    //
    //  We need the tabs to be inside the tab area's height, with some margin at the top,
    //  which we accounted for when calculating areas. So make it 2 pixels shorter and drop
    //  it down to keep the bottom aligned on the bottom of the header area.
    //
    TArea areaCur(m_areaHeaders);
    areaCur.AdjustSize(0, -2);
    areaCur.AdjustOrg(0, 2);

    TGraphWndDev gdevTar(*this);
    tCIDLib::TCard4 c4Index = 0;
    while (c4Index < c4Count)
    {
        TTabWindow& wndCur = *m_colTabs[c4Index];
        const TString& strCurText = wndCur.strWndText();

        // Calculate the required width
        tCIDLib::TCard4 c4Width = 0;
        if (bTwoLine)
        {
            // Set the bold font and get the area of the text
            {
                TFontJanitor janFont(&gdevTar, &TGUIFacility::gfontDefBold());
                c4Width = gdevTar.areaString(strCurText).c4Width();
            }

            //
            //  Set the small font and get alt text area plus four pixels we'll indent it.
            //  We take the larger of this and the main text width above.
            //
            {
                TFontJanitor janFont(&gdevTar, &TGUIFacility::gfontDefSmall());
                c4Width = tCIDLib::MaxVal
                (
                    gdevTar.areaString(wndCur.strAltText()).c4Width() + 4, c4Width
                );
            }
        }
         else
        {
            // Set the normal font and get the area of the text
            TFontJanitor janFont(&gdevTar, &TGUIFacility::gfontDefNorm());
            c4Width = gdevTar.areaString(strCurText).c4Width();
        }

        //
        //  Add a bit of margin on either side, and room for a changed indicator
        //  if needed.
        //
        c4Width += 8 + m_c4ChangeIndWidth;

        //
        //  If it is closeable, then we need the bitmap as well with margin. 2 pixels
        //  from the right edge and 6 between it and the text Add two more if we don't
        //  have a close bitmap.
        //
        if (wndCur.bCanClose())
            c4Width += m_szCloseBmp.c4Width() + 10;
        else
            c4Width += 2;

        // OK set up this width for the current tab
        areaCur.c4Width(c4Width);

        // Once we got beyond what can fully fit, break out
        if (!m_areaHeaders.bContainsArea(areaCur))
            break;

        //
        //  If this one can be closed calculate the bitmap area, and leave areaDraw
        //  with the remainder for the text below.
        //
        areaDraw = areaCur;
        areaDraw.Deflate(4, 1);
        if (wndCur.bCanClose())
        {
            // We need to draw the close bitmap
            areaBmp = areaDraw;
            areaBmp.SetSize(m_szCloseBmp);
            areaBmp.RightJustifyIn(areaDraw, kCIDLib::True);
            areaBmp.AdjustOrg(-2, 0);
            areaDraw.i4Right(areaBmp.i4Left() - 8);
        }
         else
        {
            areaBmp.ZeroAll();
        }

        // And draw the text
        if (bTwoLine)
        {
            areaText1 = areaDraw;
            areaText1.c4Height(m_c4BoldCY);

            {
                areaText2 = areaText1;
                areaText2.Move(tCIDLib::EDirs::Down);
                areaText2.c4Height(m_c4NormCY);
                areaText2.AdjustOrg(4, 0);
            }
        }
         else
        {
            areaText1 = areaDraw;
            areaText2.ZeroAll();
        }

        // Set the calculated areas for this tab for later use
        wndCur.SetTabAreas(areaCur, areaBmp, areaText1, areaText2);

        // Move the area over past it's right edge for the next round
        areaCur.Move(tCIDLib::EDirs::Right);

        // Move to the next one
        c4Index++;
    }

    //
    //  For any remaining ones zero their areas, to let the click handler code know that
    //  they are not visible, without having to go through the overhead of calculating
    //  their areas.
    //
    areaCur.ZeroAll();
    while (c4Index < c4Count)
    {
        TTabWindow& wndCur = *m_colTabs[c4Index];
        wndCur.SetTabAreas(areaCur, areaCur, areaCur, areaCur);
        c4Index++;
    }
}


//
//
//  This is called from a mouse click or hot key driven tab close. We give any listeners
//  a chance to save data and/or to cancel the close. If they allow it, we call the tab
//  close method. If it is marked as non-closeable, then we just ignore this altogether.
//
tCIDLib::TVoid TTabberWnd::DoClose(const tCIDLib::TCard4 c4Index)
{
    // See if this tab is marked as non-closable. If so, ignore this
    if (!m_colTabs[c4Index]->bCanClose())
        return;

    // Else, let the containing application choose whether to allow it
    TTabEventInfo wnotSend
    (
        m_colTabs[c4Index]->c4TabId()
        , kCIDLib::c4MaxCard
        , tCIDCtrls::ETabWEvents::Close
        , *m_pwndParent
    );
    SendSyncNotify(wnotSend, TTabbedWnd::nidEventId);

    //
    //  If they said ok, then do it. This will select a new tab as well if there
    //  is one and send a notification.
    //
    if (wnotSend.bAllow())
        CloseTabAt(c4Index);
}


//
//  Draws any tab headers that intersect the invalid area. We've already calculated the
//  areas for any of the visible ones (which are the only ones we'd have to draw worst
//  case.)
//
tCIDLib::TVoid
TTabberWnd::DrawTabHeaders(TGraphDrawDev& gdevTar, const TArea& areaInvalid)
{
    const tCIDLib::TCard4 c4Count = m_colTabs.c4ElemCount();
    if (!c4Count)
        return;

    // Remember some info for fast access
    const tCIDLib::TBoolean bTwoLine = bTwoLineMode();

    // We treat the selected tab specially below
    const tCIDLib::TCard4 c4ActiveInd = c4ActiveTabIndex();

    //
    //  If we have the focus, we fill the background of the active tab with the background
    //  fill color, else we draw it slightly lighter than the dark white that we use for
    //  the non-active tabs.
    //
    TRGBClr rgbActiveTab(0xE0, 0xFF, 0xFF);

    gdevTar.eBackMixMode(tCIDGraphDev::EBackMixModes::Transparent);
    tCIDLib::TCard4 c4Index = 0;
    TArea areaFill;
    TArea areaText1;

    while (c4Index < c4Count)
    {
        TTabWindow& wndCur = *m_colTabs[c4Index];

        // If this one isn't visible, we are done
        if (!wndCur.bTabIsVisible())
            break;

        // Get a copy of the text and add the changed indicator if needed
        TString strCurText = wndCur.strWndText();
        if (wndCur.eState() == tCIDCtrls::ETabStates::Changes)
            strCurText.Append(L" *");

        // If it intersects the invalid area, then we need to draw it
        const TArea& areaTab = wndCur.areaTab();
        if (areaTab.bIntersects(areaInvalid))
        {
            // Set the text color based on the error state
            if (wndCur.eState() == tCIDCtrls::ETabStates::Errors)
                gdevTar.SetTextColor(facCIDGraphDev().rgbRed);
            else
                gdevTar.SetTextColor(facCIDGraphDev().rgbBlack);

            // Stroke the border around it
            gdevTar.Stroke(areaTab, facCIDGraphDev().rgbPaleGrey);

            //
            //  And  fill it. For the active one, extend the size to overwrite the
            //  divider line below it visually connect it to the tab content area.
            //
            areaFill = areaTab;
            areaFill.Deflate(1, 1);
            if (c4Index == c4ActiveInd)
            {
                areaFill.AdjustSize(0, 1);

                //
                //  If this one contains the focus, it in a very light cyan. Else we draw
                //  it in dark white.
                //
                if (wndCur.bContainsFocus())
                    gdevTar.Fill(areaFill, rgbActiveTab);
                else
                    gdevTar.Fill(areaFill, rgbBgnFill());
            }
             else
            {
                gdevTar.Fill(areaFill, facCIDGraphDev().rgbDarkWhite);
            }

            if (wndCur.bCanClose())
            {
                const TArea& areaBmp = wndCur.areaClose();
                gdevTar.AlphaBlit(m_bmpClose, areaBmp.pntUL(), 0xFF, kCIDLib::True);
            }

            // And draw the text
            areaText1 = wndCur.areaText1();
            if (bTwoLine)
            {
                {
                    TFontJanitor janFont(&gdevTar, &TGUIFacility::gfontDefBold());
                    gdevTar.DrawString
                    (
                        strCurText, areaText1, tCIDLib::EHJustify::Center
                    );
                }

                {
                    TFontJanitor janFont(&gdevTar, &TGUIFacility::gfontDefSmall());
                    gdevTar.DrawString
                    (
                        wndCur.strAltText(), wndCur.areaText2(), tCIDLib::EHJustify::Center
                    );
                }
            }
             else
            {
                // Just a single line in this case, centered
                TFontJanitor janFont(&gdevTar, &TGUIFacility::gfontDefNorm());
                gdevTar.DrawString
                (
                    strCurText, areaText1, tCIDLib::EHJustify::Center
                );
            }
        }

        // Move to the next one
        c4Index++;
    }
}


//
//  If the user clicks the down arrow, and there are any current tabs, we drop down the
//  list of tabs available. If they select one, we get it selected. If it's already
//  visible, we just activate it. If it's off the right side, then we will move it to the
//  left. If we have the 'fixed first' style, we'll move it to the 1st position, since the
//  0th is protected.
//
tCIDLib::TVoid TTabberWnd::ShowMenu()
{
    const tCIDLib::TCard4 c4Count = m_colTabs.c4ElemCount();
    if (!c4Count)
        return;

    TPopupMenu menuSelect(L"Tab Selector");
    menuSelect.Create();

    const tCIDLib::TBoolean bTwoLine = bTwoLineMode();
    TString strText;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TTabWindow& wndCur = *m_colTabs[c4Index];
        if (bTwoLine)
        {
            strText = wndCur.strWndText();
            strText.Append(L" [");
            strText.Append(wndCur.strAltText());
            strText.Append(L"]");
        }
         else
        {
            strText = wndCur.strWndText();
        }

        menuSelect.AddActionItem
        (
            kCIDLib::c4MaxCard
            , m_colTabs[c4Index]->strName()
            , strText
            , 1000 + c4Index
        );
    }

    // Convert the point to screen coordinates
    TPoint pntAt;
    ToScreenCoordinates(m_areaDown.pntLL(), pntAt);
    tCIDLib::TCard4 c4Sel = menuSelect.c4Process
    (
        *this, pntAt, tCIDLib::EVJustify::Top, tCIDLib::EHJustify::Left
    );

    if (c4Sel)
    {
        // The selection minus 1000 is the tab index
        c4Sel -= 1000;

        // Activate this one. It will be made visible if neded
        ActivateAt(c4Sel);
    }
}





// ---------------------------------------------------------------------------
//  CLASS: TTabbedWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTabbedWnd: Public static data
// ---------------------------------------------------------------------------
const TNotificationId TTabbedWnd::nidEventId(L"TabbedWindowEvent");


// ---------------------------------------------------------------------------
// TTabbedWnd: Constructors and Destructor
// ---------------------------------------------------------------------------
TTabbedWnd::TTabbedWnd() :

    TCtrlWnd()
    , m_pwndTabber(nullptr)
{
}

TTabbedWnd::~TTabbedWnd()
{
}



// ---------------------------------------------------------------------------
// TTabbedWnd: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  We override this and just pass it on to our current focus tab, if any. This
//  way any applications that use tabbed windows can see if we contain the focus
//  and just pass along these calls to us in order to distribute the accel table
//  processing.
//
tCIDLib::TBoolean
TTabbedWnd::bProcessAccel(const tCIDLib::TVoid* const pMsgData) const
{
    // Just pass it to our tabber who will handle it
    if (m_pwndTabber)
        return m_pwndTabber->bProcessAccel(pMsgData);

    return kCIDLib::False;
}


// Handle dialog box init
tCIDLib::TVoid
TTabbedWnd::InitFromDesc(const  TWindow&                wndParent
                        , const TDlgItem&               dlgiSrc
                        , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::ETabWStyles eTWStyles = tCIDCtrls::ETabWStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisGroupChild
    );
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None;

    if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
        eStyles |= tCIDCtrls::EWndStyles::Border;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_TopLevel))
        eTWStyles |= tCIDCtrls::ETabWStyles::TopLevel;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_TwoLine))
        eTWStyles |= tCIDCtrls::ETabWStyles::TwoLineHdr;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_FixedFirst))
        eTWStyles |= tCIDCtrls::ETabWStyles::FixedFirst;

    // Set our background color based on the theme
    if (eTheme == tCIDCtrls::EWndThemes::DialogBox)
        SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::DlgCtrlFill), kCIDLib::True);
    else if (eTheme == tCIDCtrls::EWndThemes::MainWnd)
        SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::Window), kCIDLib::True);

    CreateTabbedWnd
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , eStyles
        , eTWStyles
        , eExStyles
    );
}


// Supports the dialog editor
tCIDLib::TVoid TTabbedWnd::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_FixedFirst);
    colToFill.objAdd(kCIDCtrls::strHint_NoBorder);
    colToFill.objAdd(kCIDCtrls::strHint_TopLevel);
    colToFill.objAdd(kCIDCtrls::strHint_TwoLine);
}


// ---------------------------------------------------------------------------
// TTabbedWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------

const TArea& TTabbedWnd::areaTabs() const
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->areaTabs();
}


tCIDLib::TVoid TTabbedWnd::ActivateAt(const tCIDLib::TCard4 c4NewIndex)
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    m_pwndTabber->ActivateAt(c4NewIndex);
}


tCIDLib::TVoid TTabbedWnd::ActivateNextTab()
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    m_pwndTabber->ActivateNextTab();
}


// Returns true if the passed window is the active tab
tCIDLib::TBoolean TTabbedWnd::bIsActiveTab(const TTabWindow& wndToCheck) const
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->bIsActiveTab(wndToCheck);
}


//
//  Find the tab with the given tab id and active it. This will throw if the
//  id is not found. It will return the index of the tab if it is found.
//
tCIDLib::TCard4 TTabbedWnd::c4ActivateById(const tCIDLib::TCard4 c4TabId)
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->c4ActivateById(c4TabId);
}


//
//  Find the named tab and activate it. This will throw if the tab name is not
//  found. If found, it returns the index of the tab found.
//
tCIDLib::TCard4 TTabbedWnd::c4ActivateByName(const TString& strTabName)
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->c4ActivateByName(strTabName);
}


// Returns the tab id of the active tab, or max card if none
tCIDLib::TCard4 TTabbedWnd::c4ActiveTabId() const
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->c4ActiveTabId();
}


// Return the index of the active tab, or max card if there aren't any tabs
tCIDLib::TCard4 TTabbedWnd::c4ActiveTabIndex() const
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->c4ActiveTabIndex();
}


//
//  Look up a tab by id and return its index. If not found, it can either
//  throw or return max card.
//
tCIDLib::TCard4
TTabbedWnd::c4IdToIndex(const   tCIDLib::TCard4     c4Id
                        , const tCIDLib::TBoolean   bThrowIfNot) const
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->c4IdToIndex(c4Id, bThrowIfNot);
}


//
//  Look up a tab by name and return its index. If not found, it can either throw
//  or return max card.
//
tCIDLib::TCard4
TTabbedWnd::c4NameToIndex(  const   TString&            strName
                            , const tCIDLib::TBoolean   bThrowIfNot) const
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->c4NameToIndex(strName, bThrowIfNot);
}


// Return the count of tabs
tCIDLib::TCard4 TTabbedWnd::c4TabCount() const
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->c4TabCount();
}


// Close all tabs, destroying and deleting the associated windows
tCIDLib::TVoid TTabbedWnd::CloseAllTabs()
{
    TWndPaintJanitor janThis(this);
    TWndPaintJanitor janTabber(m_pwndTabber);

    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    m_pwndTabber->CloseAllTabs();
}


//
//  Find the named tab, tab at, or tab with the indicated tab id and close it,
//  destroying and deleting the associated window. If the tab is marked as non-
//  closeable this will be ignored.
//
tCIDLib::TVoid TTabbedWnd::CloseTab(const TString& strTabName)
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    m_pwndTabber->CloseTab(strTabName);
}

tCIDLib::TVoid TTabbedWnd::CloseTabAt(const tCIDLib::TCard4 c4At)
{
    CIDAssert(c4At != kCIDLib::c4MaxCard, L"The incoming tab index was -1");
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    m_pwndTabber->CloseTabAt(c4At);
}

tCIDLib::TVoid TTabbedWnd::CloseTabById(const tCIDLib::TCard4 c4Id)
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    m_pwndTabber->CloseTabById(c4Id);
}


//
//  Create our control with the indicated area and styles.
//
tCIDLib::TVoid
TTabbedWnd::CreateTabbedWnd(const   TWindow&                wndParent
                            , const tCIDCtrls::TWndId       widThis
                            , const TArea&                  areaInit
                            , const tCIDCtrls::EWndStyles   eStyles
                            , const tCIDCtrls::ETabWStyles  eTWStyles
                            , const tCIDCtrls::EExWndStyles eExStyles)
{
    // Store our class styles
    m_eTWStyles = eTWStyles;

    // We force the control parent style on since we have child windows by definition
    CreateWnd
    (
        wndParent.hwndThis()
        , kCIDCtrls::pszCustClass
        , kCIDLib::pszEmptyZStr
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles::Group
        , eExStyles | tCIDCtrls::EExWndStyles::ControlParent
        , widThis
    );
}


//
//  Adds a new tab window to our list. The caller indicates where to insert the tab.
//  If max card, we add it to the end. The tab window's text is the display text for
//  the tab's header.
//
//  We create the window. So they construct it, but we create it.
//
tCIDLib::TCard4
TTabbedWnd::c4CreateTab(        TTabWindow* const       pwndToCreate
                        , const TString&                strTabText
                        , const tCIDLib::TCard4         c4At
                        , const tCIDLib::TBoolean       bActivate
                        , const tCIDCtrls::EWndStyles   eStyles
                        , const tCIDCtrls::EExWndStyles eExStyles)
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->c4CreateTab
    (
        pwndToCreate, strTabText, c4At, bActivate, eStyles, eExStyles
    );
}


// Find a tab by name
const TTabWindow* TTabbedWnd::pwndFindByName(const TString& strToFind) const
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->pwndFindByName(strToFind);
}

TTabWindow* TTabbedWnd::pwndFindByName(const TString& strToFind)
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->pwndFindByName(strToFind);
}


// Find a tab by id
const TTabWindow* TTabbedWnd::pwndFindById(const tCIDLib::TCard4 c4Id) const
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->pwndFindById(c4Id);
}

TTabWindow* TTabbedWnd::pwndFindById(const tCIDLib::TCard4 c4Id)
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->pwndFindById(c4Id);
}


// Return a pointer to the current tab, or null if none
const TTabWindow* TTabbedWnd::pwndSelected() const
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->pwndSelected();
}


TTabWindow* TTabbedWnd::pwndSelected()
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->pwndSelected();
}


// Directly access tabs by their index
const TTabWindow* TTabbedWnd::pwndTabAt(const tCIDLib::TCard4 c4At) const
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->pwndTabAt(c4At);
}

TTabWindow* TTabbedWnd::pwndTabAt(const tCIDLib::TCard4 c4At)
{
    CIDAssert(m_pwndTabber != nullptr, L"Tabber window is not set");
    return m_pwndTabber->pwndTabAt(c4At);
}



// ---------------------------------------------------------------------------
// TTabbedWnd: Protected, virtual methods
// ---------------------------------------------------------------------------

//
//  If not overridden, we send out a sync 'pre-change' notification to let a handler
//  deal with it. The value is defaulted to true, so if there is no handler, we will
//  allow it.
//
tCIDLib::TBoolean
TTabbedWnd::bAllowTabChange(TTabWindow& wndCur, const tCIDLib::TCard4 c4TabIndex)
{
    TTabEventInfo wnotPre(c4TabIndex, tCIDCtrls::ETabWEvents::PreChange, *this);
    SendSyncNotify(wnotPre, TTabbedWnd::nidEventId);
    return wnotPre.bAllow();
}


// ---------------------------------------------------------------------------
// TTabbedWnd: Protected, inherited methods
// ---------------------------------------------------------------------------

// Pass on size changes to our tabber which does the work
tCIDLib::TVoid
TTabbedWnd::AreaChanged(const   TArea&                  areaPrev
                        , const TArea&                  areaNew
                        , const tCIDCtrls::EPosStates   ePosState
                        , const tCIDLib::TBoolean       bOrgChanged
                        , const tCIDLib::TBoolean       bSizeChanged
                        , const tCIDLib::TBoolean       bStateChanged)
{
    // Call our parent first
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    if (bSizeChanged && (ePosState != tCIDCtrls::EPosStates::Minimized) && m_pwndTabber)
    {
        // Just pass it on to our tabber
        m_pwndTabber->ParentSzChanged(areaClient());
    }
}


//
//  The only time we'd see this is if they managed to click on the border, or there are no
//  tabs. We just move the focus to the tabber.
//
tCIDLib::TBoolean
TTabbedWnd::bClick( const   tCIDCtrls::EMouseButts  eButton
                    , const tCIDCtrls::EMouseClicks eClickType
                    , const TPoint&                 pntAt
                    , const tCIDLib::TBoolean       bCtrlShift
                    , const tCIDLib::TBoolean       bShift)
{
    // If clicked on, just pass the focus on to the tabber
    if (eClickType == tCIDCtrls::EMouseClicks::Down)
    {
        if (m_pwndTabber)
            m_pwndTabber->TakeFocus();
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TTabbedWnd::bCreated()
{
    if (!TParent::bCreated())
        return kCIDLib::False;

    // Create our tabber
    m_pwndTabber = new TTabberWnd;
    m_pwndTabber->Create(*this, m_eTWStyles);

    // Indicate we want to see child focus changes
    bWantsChildFocusNot(kCIDLib::True);

    return kCIDLib::True;
}

// Do it all in the paint, where we are clipped to our children
tCIDLib::TBoolean TTabbedWnd::bEraseBgn(TGraphDrawDev&)
{
    return kCIDLib::True;
}


tCIDLib::TBoolean TTabbedWnd::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    gdevToUse.Fill(areaUpdate, rgbBgnFill());
    return kCIDLib::True;
}


tCIDLib::TVoid
TTabbedWnd::ChildFocusChange(const  TWindow&
                            , const tCIDCtrls::TWndId
                            , const tCIDLib::TBoolean bGetting)
{
    // Redraw the active tab header to keep it up to date
    if (m_pwndTabber)
        m_pwndTabber->RedrawActiveTabHdr();
}


//
// Just destroy our tabber, which will destroy the tabs
//
tCIDLib::TVoid TTabbedWnd::Destroyed()
{
    try
    {
        m_pwndTabber->Destroy();
        delete m_pwndTabber;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    m_pwndTabber = nullptr;

    // Pass it on to our parent
    TParent::Destroyed();
}


//
//  If we get the focus, we just pass it to the tabber. Since this happens async, it's possible
//  the tabber is already gone by the time we see this.
//
tCIDLib::TVoid TTabbedWnd::GettingFocus()
{
    if (m_pwndTabber && m_pwndTabber->bIsValid())
        m_pwndTabber->TakeFocus();
}


// ---------------------------------------------------------------------------
// TTabbedWnd: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This allows the tabs to ask to get their header's redraw because they've changed
//  a state that the header reflects.
//
tCIDLib::TVoid TTabbedWnd::RedrawTabHdr(const tCIDLib::TCard4 c4TabId)
{
    if (m_pwndTabber && m_pwndTabber->bIsValid())
        m_pwndTabber->RedrawTabHdrById(c4TabId);
}



// ---------------------------------------------------------------------------
// TTabbedWnd: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  WE have a pointer to the tabber, but it's just forward referenced in the header. But
//  we need to see it as a window for use within the templatized notification handler
//  adding method. So we provide this method it can call, which just returns it as a
//  window.
//
TWindow* TTabbedWnd::pwndGetTabber()
{
    return m_pwndTabber;
}
