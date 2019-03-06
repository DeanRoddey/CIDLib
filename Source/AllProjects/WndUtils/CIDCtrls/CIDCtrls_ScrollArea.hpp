//
// FILE NAME: CIDCtrl_ScrollArea.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/12/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is a commonly used scrollable area. You set another window as the child
//  window and this window will allow the user to scroll it. The set child window
//  is owned by this window, so it will be destroyed when this window is destroyed.
//
//  The window object is adopted as well. You can create the windwo as a child of
//  this window, but you still need to pass in the window object as well. So you
//  can also create it initially without a parent, and hidden, then set it on us.
//  We will show it once it's been made our child.
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
//  CLASS: TScrollArea
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TScrollArea : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TScrollArea();

        ~TScrollArea();


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
        TArea areaGetChildSize() const;

        tCIDLib::TBoolean bAtBottom
        (
            const   tCIDLib::TBoolean       bHorz
        )   const;

        tCIDLib::TBoolean bAtTop
        (
            const   tCIDLib::TBoolean       bHorz
        )   const;

        tCIDLib::TCard4 c4Margin() const;

        tCIDLib::TCard4 c4Margin
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EScrAStyles  eScrAStyles
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDLib::TVoid GoToEnd
        (
            const   tCIDLib::TBoolean       bHorz
        );

        tCIDLib::TVoid GoToStart
        (
            const   tCIDLib::TBoolean       bHorz
        );

        tCIDLib::TVoid SetChild
        (
                    TWindow* const          pwndToAdopt
        );

        tCIDLib::TVoid SetChildSize
        (
            const   TSize&                  szToSet
        );


    protected :
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

        tCIDLib::TBoolean bEraseBgn
        (
                    TGraphDrawDev&          gdevToUse
        )   override;

        tCIDLib::TBoolean bMouseWheel
        (
            const   tCIDLib::TInt4          i4Clicks
        )   override;

        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;

        tCIDLib::TVoid ChildAreaChanged
        (
            const   tCIDCtrls::TWndId       widChild
            , const TArea&                  areaNew
        )   override;

        tCIDLib::TVoid Destroyed() override;

        tCIDLib::TVoid ScrollDrag
        (
            const   tCIDLib::TBoolean       bHorizontal
            , const tCIDLib::TCard4         c4Position
            , const tCIDLib::TBoolean       bFinal
        )   override;

        tCIDLib::TVoid ScrollEvent
        (
            const   tCIDCtrls::EScrollEvs   eEvent
            , const tCIDLib::TBoolean       bHorizontal
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TScrollArea(const TScrollArea&);
        tCIDLib::TVoid operator=(const TScrollArea&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdjustBars();

        tCIDLib::TVoid CheckHaveChild
        (
            const   tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid CleanupChild();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Margin
        //      A number of pixels to inset the child window, to provide some
        //      margin for the content.
        //
        //  m_eScrAStyles
        //      Our class specific styles
        //
        //  m_pwndChild
        //      A pointer to the window that has been set to scroll.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4Margin;
        tCIDCtrls::EScrAStyles  m_eScrAStyles;
        TWindow*                m_pwndChild;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TScrollArea, TStdCtrlWnd)
};

#pragma CIDLIB_POPPACK

