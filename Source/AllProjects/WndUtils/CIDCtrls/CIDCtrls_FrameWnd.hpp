//
// FILE NAME: CIDCtrls_FrameWnd.hpp
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
//  This is a standard frame type window, of the sort you'd use for a top level
//  application window. Generally speaking it will be derived from in order to
//  do anything useful, to load up date, create child windows, etc...
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
//  CLASS: TFrameWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TFrameWnd : public TWindow
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid AreaForClient
        (
            const   TArea&                  areaClient
            ,       TArea&                  areaNew
            , const tCIDCtrls::EWndStyles   eWndStyles
            , const tCIDCtrls::EExWndStyles eExWndStyles
            , const tCIDLib::TBoolean       bHasMenu
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFrameWnd();

        ~TFrameWnd();


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
        tCIDLib::TVoid Activate();

        tCIDLib::TVoid AddWndState
        (
            const   TWndState&              wstateToAdd
        );

        tCIDLib::TVoid ApplyWndState
        (
            const   TString&                strToSet
        );

        tCIDLib::TBoolean bAnyActivePopup
        (
            const   tCIDLib::TBoolean       bModalOnly
        )   const;

        tCIDLib::TBoolean bInFSMode() const;

        tCIDLib::TBoolean bIsActive() const;

        tCIDLib::TBoolean bIsMaximized() const;

        tCIDLib::TBoolean bIsMinimized() const;

        tCIDLib::TBoolean bIsOwned() const;

        tCIDLib::TBoolean bIsRestored() const;

        tCIDLib::TVoid CreateFrame
        (
            const   TWindow* const          pwndOwner
            , const TArea&                  areaInit
            , const TString&                strText
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EExWndStyles eExWndStyles
            , const tCIDCtrls::EFrameStyles eFrameStyles
            , const tCIDLib::TBoolean       bHVRedraw
        );

        tCIDLib::TVoid CreateFrame
        (
            const   TWindow* const          pwndOwner
            , const TArea&                  areaInit
            , const TString&                strText
            , const tCIDLib::TCardList&     fcolStatusBarSlots
            , const TGUIFacility&           facSrc
            , const tCIDLib::TResId         ridMenu
        );

        tCIDLib::TVoid DelWindowState
        (
            const   TString&                strToRemove
        );

        tCIDLib::TVoid ExitFullScreenMode();

        tCIDCtrls::EFrameStyles eFrameStyles() const;

        tCIDLib::TVoid ForceToFront();

        tCIDLib::TVoid FullScreenMode();

        tCIDLib::TVoid Maximize();

        tCIDLib::TVoid Minimize();

        const TMenuBar& menuCur() const;

        TMenuBar& menuCur();

        const TMenuBar* pmenuCur() const;

        TMenuBar* pmenuCur();

        tCIDLib::TVoid RemoveMenu();

        tCIDLib::TVoid RestoreMenuBar();

        tCIDLib::TVoid Restore();

        tCIDLib::TVoid RestorePosState
        (
            const   tCIDCtrls::EPosStates   eState
            , const TArea&                  areaNormal
            , const tCIDLib::TBoolean       bShow
        );

        tCIDLib::TVoid RestorePosState
        (
            const   TArea&                  areaTo
            , const tCIDLib::TBoolean       bShow
        );

        tCIDLib::TVoid SetMenuBar
        (
            const   TGUIFacility&           facSrc
            , const tCIDLib::TResId         ridMenu
        );

        tCIDLib::TVoid SetMinMaxSize
        (
            const   TSize&                  szMin
            , const TSize&                  szMax
        );

        tCIDLib::TVoid SetStatusBarAreas
        (
            const   tCIDLib::TCardList&     fcolValues
            , const tCIDLib::TBoolean       bPercents
        );

        tCIDLib::TVoid SetStatusBarText
        (
            const   tCIDLib::TCard4         c4Index
            , const TString&                strText
        );

        tCIDLib::TVoid SizeToClient
        (
            const   TArea&                  areaFitTo
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid ActivationChange
        (
            const   tCIDLib::TBoolean       bState
            , const tCIDCtrls::TWndHandle   hwndOther
        );

        virtual tCIDLib::TBoolean bAllowShutdown();

        virtual tCIDLib::TBoolean bMaximizing();

        virtual tCIDLib::TBoolean bMinimizing();

        virtual tCIDLib::TBoolean bRestoring();


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
        )  override;

        tCIDLib::TBoolean bCreated();

        tCIDLib::TVoid Destroyed() override;

        tCIDLib::TVoid GettingFocus() override;

        tCIDCtrls::TMsgResult mresDispatch
        (
            const   TWindow&                wndThis
            , const tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
        )   override;

        tCIDLib::TVoid WndTextPreQuery
        (
                    TString&                strToFill
        )   const override;

        tCIDLib::TVoid WndTextStore
        (
            const   TString&                strToStore
        )   override;


        // -------------------------------------------------------------------
        //  Protected, non-virtula methods
        // -------------------------------------------------------------------
        tCIDCtrls::TWndId widNext();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TFrameWnd(const TFrameWnd&);
        tCIDLib::TVoid operator=(const TFrameWnd&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4FindWndState
        (
            const   TString&                strToFind
        )   const;

        tCIDLib::TVoid DoInit();

        tCIDLib::TVoid RestoreLastFocus();


        // -------------------------------------------------------------------
        //  Private class members
        //
        //  m_accelMenu
        //      If a menu is set on us, then we create an accelerator table for it,
        //      if it has any hot keys anyway. We override bProcessAccel and if
        //      we have a table, we give it a chance to process it.
        //
        //  m_areaPreFS
        //      Once we go into full screen mode, we are not actually maximizing, which
        //      has issues because Windows wants to prevent it from overlaying docked
        //      tools like the task bar. So we need to provide our own means to return
        //      to the pre-full screen area.
        //
        //  m_bActive
        //      Indicates whether we are the active frame. This is kept up
        //      to date via the ActivationChanged() method.
        //
        //  m_bFSMode
        //      To remember if we are in fulls creen mode or not.
        //
        //  m_c4FSStyles
        //  m_c4FSExStyles
        //      Upon entering full screen mode we save the current styles so that
        //      we can restore them.
        //
        //  m_colWndStates
        //      We support a list of named states that enable/disable menu items
        //      and child windows, set focus, etc.. The SetWindowState() method
        //      will set one.
        //
        //  m_eFrameStyles
        //      Our frame specific styles
        //
        //  m_hwndLastFocus
        //      When we lose activation, we store the current child with the
        //      focus. When we get focus, if this is set, we put focus back
        //      on that child.
        //
        //  m_pmenuBar
        //      A menu bar can be set on us by client code or derived classes. We
        //      adopt the menu bar and associate it with us, which makes it our
        //      visible menu bar. We also
        //
        //  m_pwndStatusBar
        //      We can have a status bar. If not this is null, so this has to be
        //      checked for.
        //
        //  m_szMaximum
        //  m_szMinimum
        //      These can be set to limit the min/max size that the frame can be
        //      sized to by the user. If it's empty, then no limitation is
        //      applied.
        //
        //  m_widNext
        //      Since these types of windows almost always create children, we provide
        //      an easy way for them to assign unique ids.
        // -------------------------------------------------------------------
        TAccelTable                 m_accelMenu;
        TArea                       m_areaPreFS;
        tCIDLib::TBoolean           m_bActive;
        tCIDLib::TBoolean           m_bFSMode;
        tCIDLib::TCard4             m_c4FSStyles;
        tCIDLib::TCard4             m_c4FSExStyles;
        tCIDCtrls::TWndStateList    m_colWndStates;
        tCIDCtrls::EFrameStyles     m_eFrameStyles;
        tCIDCtrls::TWndHandle       m_hwndLastFocus;
        TMenuBar*                   m_pmenuBar;
        TStatusBar*                 m_pwndStatusBar;
        TSize                       m_szMaximum;
        TSize                       m_szMinimum;
        tCIDCtrls::TWndId           m_widNext;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFrameWnd, TWindow)
};

#pragma CIDLIB_POPPACK


