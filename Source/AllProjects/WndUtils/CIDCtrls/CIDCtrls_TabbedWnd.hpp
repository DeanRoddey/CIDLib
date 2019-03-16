//
// FILE NAME: CIDCtrls_TTabbedWnd.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/15/2015
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
//  This class implements our own tabbed window control. The built in Windows tabbed control
//  sucks so profoundly that it was actually worth the effort do our own.
//
//  We also create a tab class. Each window added as a tab must derive from the tab window.
//  This way we can access required functionality in them generically. These have up to three
//  pieces of text associated with them:
//
//  1. The actual display text for the tab header
//  2. A unique text string used to indentify the tab, since the tab header text is often
//      some shorted bit of text that might not be unique.
//  3. An 'alt text' value that is only used when in two line mode, and is drawn under the
//      main display text in a smaller font.
//
//  The display text is gotten from the window text set on the window. The unique text
//  string is stored in the 'window name', which is a standard thing all windows supports. This
//  allows it to be gotten to even if the code in question doesn't know it's a tab window.
//  The alt text is stored in the tab window base class itself.
//
//  We define the usual event class to send out notifications. One of the events is that we
//  are changing to a new tab. This is sent synchronously and the handler can say yes or no,
//  to allow it or not. If a derived class doesn't override the bAllowChange() method, our
//  default will send out a synchronous event to let the handler make that decision.
//
//  We don't allow tabs to scroll. Instead, if we have more than we can display, we show a
//  drop down button at the far right. This greatly simplifies things for us, and is probably
//  better since it's so easy to lose tabs when they are scrolled.
//
//  Due to stupidity in the Windows tabbing stuff, we cannot have the tabs be children of
//  the tab display window. This will prevent the tab display window itself from ever being
//  able to receive the focus by way of tab key. So we create an underlying background
//  window which positions the tab control at the top, and the tabs under it. This is vastly
//  simpler than trying to maintain the tabs as completely disconnected sibling windows.
//
// CAVEATS/GOTCHAS:
//
//  1)  WE LET THE tabber window position stuff within the underlying parent window. However
//      this means that it sees areas relative to the parent's client area (which is important
//      if the parent has a border.) So we have to be sure to adjust window positions relative
//      to the client area of the parent, not the origin, whereas any painting is done via
//      the tabber's own window area.
//
//      So just be careful to keep them clear!
//
//  2)  There are issues in windows with the composited drawing if hidden tabs start redrawing
//      themselves. They seem to get picked up even if hidden. So, to avoid this, we create
//      a zero sized, hidden window to use as a 'hidden bag' to put unused tabs into.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TTabbedWnd;
class TTabberWnd;


// ---------------------------------------------------------------------------
//   CLASS: TTabEventInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TTabEventInfo : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTabEventInfo
        (
            const   tCIDLib::TCard4         c4TabId
            , const tCIDCtrls::ETabWEvents  eEvent
            , const TWindow&                wndSrc
        );

        TTabEventInfo
        (
            const   tCIDLib::TCard4         c4NewId
            , const tCIDLib::TCard4         c4OldId
            , const tCIDCtrls::ETabWEvents  eEvent
            , const TWindow&                wndSrc
        );

        TTabEventInfo
        (
            const   TTabEventInfo&          wnotToCopy
        );

        ~TTabEventInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTabEventInfo& operator=
        (
            const   TTabEventInfo&          wnotToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAllow() const;

        tCIDLib::TBoolean bAllow
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TCard4 c4PrevTabId() const;

        tCIDLib::TCard4 c4TabId() const;

        tCIDCtrls::ETabWEvents eEvent() const;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TTabEventInfo();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bAllow
        //      Used with the 'pre-change' event. The handler can indicate whether
        //      or not to allow the change to proceed or not. It's defaulted to
        //      true to allow it if no handler.
        //
        //  m_c4PrevTabId
        //      The id of the previous tab, for things like change in tab activation. If
        //      no previous, then it's max card.
        //
        //  m_c4TabId
        //      The id of the tab being reported. If none, it's max card.
        //
        //  m_eEvent
        //      The event type.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bAllow;
        tCIDLib::TCard4         m_c4PrevTabId;
        tCIDLib::TCard4         m_c4TabId;
        tCIDCtrls::ETabWEvents  m_eEvent;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTabEventInfo,TCtrlNotify)
        DefPolyDup(TTabEventInfo)
};


// ---------------------------------------------------------------------------
//  CLASS: TTabWindow
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TTabWindow : public TWindow
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TTabWindow() = delete;

        TTabWindow(const TTabWindow&) = delete;

        ~TTabWindow();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TVoid operator=(const TTabWindow&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual, methods
        // -------------------------------------------------------------------
        const TArea& areaTab() const;

        const TArea& areaClose() const;

        const TArea& areaText1() const;

        const TArea& areaText2() const;

        tCIDLib::TBoolean bCanClose() const
        {
            return m_bCanClose;
        }

        tCIDLib::TBoolean bTabIsVisible() const;

        tCIDLib::TCard4 c4TabId() const
        {
            return m_c4TabId;
        }

        tCIDCtrls::ETabWEvents eHitTest
        (
            const   TPoint&                 pntAt
        );

        tCIDCtrls::ETabStates eState() const
        {
            return m_eState;
        }

        TTabbedWnd* pwndOwningTabbedWnd() const
        {
            return m_pwndOwningTabbedWnd;
        }

        const TString& strAltText() const;

        tCIDLib::TVoid SetState
        (
            const   tCIDCtrls::ETabStates   eToSet
            , const tCIDLib::TBoolean       bRedraw
        );


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TTabberWnd;
        friend class TTabbedWnd;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TTabWindow
        (
            const   TString&                strUniqueId
            , const TString&                strAltText
            , const tCIDLib::TBoolean       bCanClose
            , const tCIDLib::TBoolean       bDlgProc
        );


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid TabActivationChange
        (
            const   tCIDLib::TBoolean       bGetting
        );


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bEraseBgn
        (
                    TGraphDrawDev&          gdevToUse
        )   override;

        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;

        tCIDLib::TVoid ChildFocusChange
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widChild
            , const tCIDLib::TBoolean       bGotFocus
        ) override;

        tCIDLib::TVoid GettingFocus() override;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CreateTabWnd
        (
            const   TWindow&                wndParent
            ,       TTabbedWnd* const       pwndOwningTabbedWnd
            , const TString&                strTabText
            , const TArea&                  areaTab
            , const tCIDCtrls::TWndId       widToUse
            , const tCIDCtrls::EWndStyles   eStyles = tCIDCtrls::EWndStyles::None
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDLib::TVoid PostTabInit();

        tCIDLib::TVoid SelectChange
        (
            const   tCIDLib::TBoolean       bGetting
            , const tCIDLib::TBoolean       bTakeFocus
        );

        tCIDLib::TVoid SetTabAreas
        (
            const   TArea&                  areaToSet
            , const TArea&                  areaClose
            , const TArea&                  areaText1
            , const TArea&                  areaText2
        );

        tCIDLib::TVoid SetId
        (
            const   tCIDLib::TCard4         c4ToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaClose
        //  m_areaText1
        //  m_areaText2
        //  m_areaUs
        //      Every time the tabber window does something that affects the order or
        //      presence of tabs, it will recalculate their areas and store them here.
        //      The drawing and hit testing code uses these areas subsequently.
        //
        //  m_bCanClose
        //      Indicates if this tab can be closed. If so we draw the close bitmap on
        //      the tab and respond to clicks on it.
        //
        //  m_eState
        //      Tabs can be marked as being in one of a few different states and will
        //      display the header content to reflect that. Defaults to normal state and
        //      stays there unless client code changes it.
        //
        //  m_c4TabId
        //      A unique id set on this tab window when it is inserted. It can be used
        //      later to activate it (in addition to by name.) It's never set to zero, so
        //      if it's zero it's not been set yet.
        //
        //  m_hwndLastFocus
        //      When we are deselected we will see if we currently have the focus inside
        //      of us. If so, we save the handle. Upon restore, we put the focus back there.
        //      If not, it's null.
        //
        //  m_pwndOwningTabbedWnd
        //      Our parent window may not be the tabbed window. Only the visible one is
        //      a child of it. The others are put into a hidden window to make them
        //      invisible and take them out of the focus tabbing sequence. So we can't
        //      get to him by getting our parent. So we get passed a pointer to it.
        //
        //  m_strAltText
        //      The alt text, which is only used if in two line mode.
        // -------------------------------------------------------------------
        TArea                   m_areaClose;
        TArea                   m_areaText1;
        TArea                   m_areaText2;
        TArea                   m_areaUs;
        tCIDLib::TBoolean       m_bCanClose;
        tCIDLib::TCard4         m_c4TabId;
        tCIDCtrls::ETabStates   m_eState;
        tCIDCtrls::TWndHandle   m_hwndLastFocus;
        TTabbedWnd*             m_pwndOwningTabbedWnd;
        TString                 m_strAltText;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTabWindow, TWindow)
};


// ---------------------------------------------------------------------------
//  CLASS: TTabbedWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TTabbedWnd : public TCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TNotificationId        nidEventId;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTabbedWnd();

        TTabbedWnd(const TTabbedWnd&) = delete;

        ~TTabbedWnd();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTabbedWnd& operator=(const TTabbedWnd&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bProcessAccel
        (
            const   tCIDLib::TVoid* const   pMsgData
        )   const override;

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
            , const tCIDLib::TCard4         c4At = kCIDLib::c4MaxCard
            , const tCIDLib::TBoolean       bActivate = kCIDLib::False
            , const tCIDCtrls::EWndStyles   eStyles = tCIDCtrls::EWndStyles::None
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDLib::TCard4 c4TabCount() const;

        tCIDLib::TVoid CloseAllTabs();

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

        tCIDLib::TVoid CreateTabbedWnd
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::ETabWStyles  eTWStyles
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
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


        // -------------------------------------------------------------------
        //  We provide a templatized method to allow for typesafe registration
        //  of event handlers.
        // -------------------------------------------------------------------
        template <class T> TNotHandlerBase*
        pnothRegisterHandler(        T* const        pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TTabEventInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(pwndGetTabber(), pTarget);

            // Add a request object to our tabber
            pwndGetTabber()->AddNotifyRequest
            (
                new TNotifyRequestFor<T, TTabEventInfo>(pTarget, TTabbedWnd::nidEventId)
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TTabEventInfo>
            (
                pTarget, TTabbedWnd::nidEventId, pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
        }


    protected :
        // -------------------------------------------------------------------
        //  The internal tabber and the base tab window are our friends
        // -------------------------------------------------------------------
        friend class TTabberWnd;
        friend class TTabWindow;


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bAllowTabChange
        (
                    TTabWindow&             wndCur
            , const tCIDLib::TCard4         c4TabIndex
        );


        // -------------------------------------------------------------------
        //  Protected, inherited methods
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

        tCIDLib::TVoid ChildFocusChange
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widChild
            , const tCIDLib::TBoolean       bGotFocus
        ) override;

        tCIDLib::TVoid Destroyed() override;

        tCIDLib::TVoid GettingFocus() override;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid RedrawTabHdr
        (
            const   tCIDLib::TCard4         c4TabId
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        TWindow* pwndGetTabber();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eTWStyles
        //      Our class specific style. Mostly for use by our tabber, to which we pass
        //      them along, but we at least have to store them till we can pass them.
        //
        //  m_pwndTabber
        //      This is the tabber control that we place at the top of our area. It
        //      actually does almost all the work. We just act as a go between, passing
        //      calls on to it.
        // -------------------------------------------------------------------
        tCIDCtrls::ETabWStyles  m_eTWStyles;
        TTabberWnd*             m_pwndTabber;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTabbedWnd, TCtrlWnd)
};

#pragma CIDLIB_POPPACK



