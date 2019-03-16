//
// FILE NAME: CIDResEd_ProjectTab.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/17/2015
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the tabs that we put into the tabbed control that goes to
//  left of the client area. There is one that shows the current list of dialogs,
//  and another that shows the list of controls within the dialog being edited.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TDialogsTab
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class TDialogsTab : public TTabWindow
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDialogsTab();

        ~TDialogsTab();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CreateDlgsTab
        (
                    TResEdMainFrame&        wndMainFrm
            ,       TTabbedWnd&             wndParent
        );

        tCIDLib::TVoid InsertNew
        (
            const   TResEdDlgDesc&          reddAdd
        );

        tCIDLib::TVoid LoadList
        (
            const   TResEdData&             redatSrc
        );

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid UpdateDlg
        (
            const   TResEdDlgDesc&          reddNew
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

        tCIDLib::TVoid Destroyed() override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pwndList
        //      We use a multi-column list box to show our available dialogs.
        // -------------------------------------------------------------------
        TMultiColListBox*   m_pwndList;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TDialogsTab, TTabWindow)
};


