//
// FILE NAME: CIDMacroDbg_BreakPntsTab.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/29/2015
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
//  This is the header for the breakpoint tracking tab.
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
//   CLASS: TBreakPntsTab
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class TBreakPntsTab : public TTabWindow
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBreakPntsTab();

        TBreakPntsTab(const TBreakPntsTab&) = delete;

        ~TBreakPntsTab();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBreakPntsTab& operator=(const TBreakPntsTab&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddBreakPoint
        (
            const   TSrcTab&                wndSrcTab
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TBoolean       bEnabled
        );

        tCIDLib::TVoid CreateBreakPntsTab
        (
                    TTabbedWnd&             wndParent
            ,       TMacroDbgMainWnd* const pwndMain
        );

        tCIDLib::TVoid EnableBreakPoint
        (
            const   TSrcTab&                wndSrcTab
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TBoolean       bNewState
        );

        tCIDLib::TVoid RemoveAll();

        tCIDLib::TVoid RemoveBreakPoint
        (
            const   TSrcTab&                wndSrcTab
            , const tCIDLib::TCard4         c4LineNum
        );

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid UpdateBreakPoints
        (
            const   TSrcTab&                wndSrcTab
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
        )   override;

        tCIDLib::TBoolean bCreated() override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );

        tCIDCtrls::EEvResponses eLBHandler
        (
                    TListChangeInfo&        wnotEvent
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4NextId
        //      Used to assign unique ids to each item added.
        //
        //  m_pwndXXX
        //      The controls we load from our dialog resource.
        //
        //  m_pwndMain
        //      We get a pointer to the main window. When they double click a break
        //      point we need to call to the main window to go there.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4NextId;
        TPushButton*        m_pwndDel;
        TPushButton*        m_pwndDelAll;
        TPushButton*        m_pwndDelSrc;
        TCheckedMCListWnd*  m_pwndList;
        TMacroDbgMainWnd*   m_pwndMain;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBreakPntsTab, TTabWindow)
};


#pragma CIDLIB_POPPACK



