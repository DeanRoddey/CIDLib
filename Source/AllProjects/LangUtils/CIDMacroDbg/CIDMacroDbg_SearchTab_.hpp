//
// FILE NAME: CIDMacroDbg_SearchTab.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/11/2015
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
//  This is the header for a search and replace window that we insert as a tab
//  in the lower pane of the editor. It just interacts wtih the current editor
//  tab to find/replace text.
//
//  We load up our content from a dialog description, and just create the controls
//  as direct children of the tab, so we enable dailog style processing on this
//  window, in order to get focus management and such.
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
//   CLASS: TSearchTab
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class TSearchTab : public TTabWindow
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSearchTab();

        TSearchTab(const TSearchTab&) = delete;

        ~TSearchTab();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSearchTab& operator=(const TSearchTab&) = delete;


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
        tCIDLib::TVoid CreateSearchTab
        (
                    TTabbedWnd&             wndParent
            ,       TMacroDbgMainWnd* const pwndMainDbg
        );

        tCIDLib::TVoid Find();

        tCIDLib::TVoid FindNext();

        tCIDLib::TVoid Replace();

        tCIDLib::TVoid SetState
        (
            const   tCIDLib::TBoolean       bCanSearch
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TVoid MenuCommand
        (
            const   tCIDLib::TResId         ridItem
            , const tCIDLib::TBoolean       bChecked
            , const tCIDLib::TBoolean       bEnabled
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );

        tCIDLib::TVoid SetSearchFlags
        (
                    TSrcEditor&             wndTar
        );

        tCIDLib::TVoid StoreRecall
        (
            const   TString&                strStore
            , const tCIDLib::TBoolean       bFind
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_accelSearch
        //      We need to support some hot keys at our level
        //
        //  m_pwndMainDbg
        //      We get a pointer to the main debugger window (our ultimate parent
        //      within the debugger), which we use to get to the current source
        //      tab.
        // -------------------------------------------------------------------
        TAccelTable         m_accelSearch;
        TCheckBox*          m_pwndCase;
        TPushButton*        m_pwndFindNext;
        TPushButton*        m_pwndFindPrev;
        TMacroDbgMainWnd*   m_pwndMainDbg;
        TCheckBox*          m_pwndPosix;
        TCheckBox*          m_pwndRegEx;
        TPushButton*        m_pwndReplace;
        TComboBox*          m_pwndRepText;
        TCheckBox*          m_pwndSelOnly;
        TComboBox*          m_pwndToFind;
        TCheckBox*          m_pwndWholeWords;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSearchTab, TTabWindow)
};

#pragma CIDLIB_POPPACK


