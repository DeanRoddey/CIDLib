//
// FILE NAME: CIDWUtils_ListSelDlg_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/18/2015
//
// COPYRIGHT: 2015..2015, Charmed Quark Systems, Ltd
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header for the CID_ListSelDlg.cpp file, which lets the user select from a
//  list of values in a couple different ways. These are just generic, reusable dialog
//  boxes.
//
//  We have one that just presents a fixed list of values in a list box and let's the user
//  select either one or multiples. It can be invoked in either mode.
//
//  We have another that uses a writable combo box so that the user can either select from
//  an existing list or enter a value manually, which will be added to the list if not
//  already there. An entered one is added at the top (to become the default next time) and
//  if already in the list will be moved to the top.
//
//  We have one specialized for a list of key/value pairs
//
//  We have one that is a specialized multi-select type, which takes an incoming list of
//  previously selected values to mark and returns the new list.
//
//  And we have another for dedicated, single value selections, where we can return a
//  selection index, so we have to take an ordered list in that case.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $Log$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TCIDListSelDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TCIDListSelDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDListSelDlg();

        TCIDListSelDlg(const TCIDListSelDlg&) = delete;

        ~TCIDListSelDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDListSelDlg& operator=(const TCIDListSelDlg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRun
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const tCIDLib::TStrCollect&   colList
            ,       tCIDLib::TStrCollect&   colSelections
            , const tCIDLib::TBoolean       bMultiMode
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected inherited methods
        // -------------------------------------------------------------------
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
        //  m_bMultiMode
        //      The caller indicates if the user should be able to select multiple values
        //      or just one.
        //
        //  m_pcolList
        //      A pointer to the incoming list until we can get the values loaded up.
        //
        //  m_pwndXXX
        //      Some typed pointers we get to some of our child widgets we need to
        //      interact with on a typed basis.
        //
        //  m_strTitle
        //      Save the incoming user provided title text till we can get it set.
        //
        //  m_colValues
        //      We store the incoming values (if any) for initial selection, and we
        //      store the user selections here until we can get them back into the
        //      caller's parm.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bMultiMode;
        const tCIDLib::TStrCollect* m_pcolList;
        TPushButton*                m_pwndCancelButton;
        TMultiColListBox*           m_pwndList;
        TPushButton*                m_pwndSelectButton;
        TString                     m_strTitle;
        tCIDLib::TStrList           m_colValues;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDListSelDlg,TDlgBox)
};



// ---------------------------------------------------------------------------
//   CLASS: TCIDListSelDlg2
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TCIDListSelDlg2 : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDListSelDlg2();

        TCIDListSelDlg2(const TCIDListSelDlg2&) = delete;

        ~TCIDListSelDlg2();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDListSelDlg2& operator=(const TCIDListSelDlg2&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRun
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            ,       tCIDLib::TStrList&      colList
            ,       TString&                strSelText
            , const tCIDWUtils::EListValFmts eFmt
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eFmt
        //      Lets the caller indicate some basic constraints on the value that we
        //      can check ourself before we will accept it.
        //
        //  m_pcolList
        //      A pointer to the incoming list until we can get the values loaded
        //      up, and to put a new one back into if one is entered manually.
        //
        //  m_pwndXXX
        //      Some typed pointers we get to some of our child widgets we need to
        //      interact with on a typed basis.
        //
        //  m_strTitle
        //      Save the incoming user provided title text till we can get it set.
        //
        //  m_strValue
        //      We store the incoming value (if any) for initial selection, and
        //      we store the user selection here until we can get it back into the
        //      caller's parm.
        // -------------------------------------------------------------------
        tCIDWUtils::EListValFmts    m_eFmt;
        tCIDLib::TStrList*          m_pcolList;
        TPushButton*                m_pwndCancelButton;
        TComboBox*                  m_pwndList;
        TPushButton*                m_pwndSelectButton;
        TString                     m_strTitle;
        TString                     m_strValue;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDListSelDlg2,TDlgBox)
};



// ---------------------------------------------------------------------------
//   CLASS: TCIDListSelDlg3
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TCIDListSelDlg3 : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDListSelDlg3();

        TCIDListSelDlg3(const TCIDListSelDlg3&) = delete;

        ~TCIDListSelDlg3();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDListSelDlg3& operator=(const TCIDListSelDlg3&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Run
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const tCIDLib::TStrList&      colList
            , const tCIDLib::TCard4         c4InitSel
            , const tCIDLib::TBoolean       bSort
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected inherited methods
        // -------------------------------------------------------------------
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
        //  m_bSort
        //      They can choose to have the list sorted or not. By default the style is enable
        //      on the list window, so we force it off if this is false.
        //
        //  m_c4InitSel
        //      The incoming initial selection index.
        //
        //  m_c4SelIndex
        //      We store the index of the selected item until we can get it back to
        //      the caller.
        //
        //  m_pcolList
        //      A pointer to the incoming list until we can get the values loaded up.
        //
        //  m_pwndXXX
        //      Some typed pointers we get to some of our child widgets we need to
        //      interact with on a typed basis.
        //
        //  m_strTitle
        //      Save the incoming user provided title text till we can get it set.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bSort;
        tCIDLib::TCard4             m_c4InitSel;
        tCIDLib::TCard4             m_c4SelIndex;
        const tCIDLib::TStrList*    m_pcolList;
        TPushButton*                m_pwndCancelButton;
        TMultiColListBox*           m_pwndList;
        TPushButton*                m_pwndSelectButton;
        TString                     m_strTitle;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDListSelDlg3,TDlgBox)
};




// ---------------------------------------------------------------------------
//   CLASS: TCIDKVPListSelDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TCIDKVPListSelDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDKVPListSelDlg();

        TCIDKVPListSelDlg(const TCIDKVPListSelDlg&) = delete;

        ~TCIDKVPListSelDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDKVPListSelDlg& operator=(const TCIDKVPListSelDlg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRun
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const tCIDLib::TKVPCollect&   colList
            ,       TKeyValuePair&          kvalSelText
            , const tCIDLib::TBoolean       bShowValue
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected inherited methods
        // -------------------------------------------------------------------
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

        tCIDLib::TVoid StoreSelection
        (
            const   tCIDLib::TCard4         c4Index
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bShowValue
        //      We can either just show the key, or the key plus the value in parens
        //      to the right of it.
        //
        //  m_kvalValue
        //      We store the incoming value (if any) for initial selection, and
        //      we store the user selection here until we can get it back into the
        //      caller's parm.
        //
        //  m_pcolList
        //      A pointer to the incoming list until we can get the values loaded
        //      up.
        //
        //  m_pwndXXX
        //      Some typed pointers we get to some of our child widgets we need to
        //      interact with on a typed basis.
        //
        //  m_strTitle
        //      Save the incoming user provided title text till we can get it set.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bShowValue;
        TKeyValuePair               m_kvalValue;
        const tCIDLib::TKVPCollect* m_pcolList;
        TPushButton*                m_pwndCancelButton;
        TMultiColListBox*           m_pwndList;
        TPushButton*                m_pwndSelectButton;
        TString                     m_strTitle;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDKVPListSelDlg,TDlgBox)
};



// ---------------------------------------------------------------------------
//   CLASS: TCIDMListSelDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TCIDMListSelDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDMListSelDlg();

        TCIDMListSelDlg(const TCIDMListSelDlg&) = delete;

        ~TCIDMListSelDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDMListSelDlg& operator=(const TCIDMListSelDlg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRun
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const TString&                strDataTitle
            , const tCIDLib::TStrList&      colAvail
            ,       tCIDLib::TStrList&      colSelected
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );

        tCIDCtrls::EEvResponses eListHandler
        (
                    TListChangeInfo&        wnotEvent
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pcolAvail
        //      A pointer to the incoming list of values, so we can get them loaded up.
        //
        //  m_pcolSelected
        //      A point to the list of incoming selected values and outgoing selected
        //      values if any changes were made.
        //
        //  m_pwndXXX
        //      Some typed pointers we get to some of our child widgets we need to interact
        //      with on a typed basis.
        //
        //  m_strDataTitle
        //      The column header for the 2nd column in the list box, to describe what's
        //      being displayed there.
        //
        //  m_strTitle
        //      Save the incoming user provided title text till we can get it set.
        // -------------------------------------------------------------------
        const tCIDLib::TStrList*    m_pcolAvail;
        tCIDLib::TStrList*          m_pcolSelected;
        TPushButton*                m_pwndCancel;
        TCheckedMCListWnd*          m_pwndList;
        TPushButton*                m_pwndSelect;
        TString                     m_strDataTitle;
        TString                     m_strTitle;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDMListSelDlg,TDlgBox)
};



// ---------------------------------------------------------------------------
//   CLASS: TCIDKVPMListSelDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TCIDKVPMListSelDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDKVPMListSelDlg();

        TCIDKVPMListSelDlg(const TCIDKVPMListSelDlg&) = delete;

        ~TCIDKVPMListSelDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDKVPMListSelDlg& operator=(const TCIDKVPMListSelDlg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRun
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const TString&                strDataTitle
            , const tCIDLib::TKVPList&      colAvail
            ,       tCIDLib::TKVPList&      colSelected
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );

        tCIDCtrls::EEvResponses eListHandler
        (
                    TListChangeInfo&        wnotEvent
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pcolAvail
        //      A pointer to the incoming list of values, so we can get them loaded up.
        //      We show the key. The value is a behind the scenes value that we just give
        //      back with the selections, as a convenience for the calling code.
        //
        //  m_pcolSelected
        //      A point to the list of incoming selected values and outgoing selected
        //      values if any changes were made.
        //
        //  m_pwndXXX
        //      Some typed pointers we get to some of our child widgets we need to interact
        //      with on a typed basis.
        //
        //  m_strDataTitle
        //      The column header for the 2nd column in the list box, to describe what's
        //      being displayed there.
        //
        //  m_strTitle2
        //      Save the incoming user provided title text till we can it set.
        // -------------------------------------------------------------------
        const tCIDLib::TKVPList*    m_pcolAvail;
        tCIDLib::TKVPList*          m_pcolSelected;
        TPushButton*                m_pwndCancel;
        TCheckedMCListWnd*          m_pwndList;
        TPushButton*                m_pwndSelect;
        TString                     m_strDataTitle;
        TString                     m_strTitle;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDKVPMListSelDlg,TDlgBox)
};

#pragma CIDLIB_POPPACK



