//
// FILE NAME: CIDWUtils_ShowKVPListDlg_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/29/2016
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
//  This is the header for the CIDWUtils_ShowKVPListDlg.cpp file, which implements a
//  simple dialog for showing a list of key/value pairs. We set up a two column list
//  (well three actually but the first one is 0 sized) and right justify the first
//  column.
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
//   CLASS: TShowKVPListDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TShowKVPListDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TShowKVPListDlg();

        TShowKVPListDlg(const TShowKVPListDlg&) = delete;

        ~TShowKVPListDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TShowKVPListDlg& operator=(const TShowKVPListDlg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Run
        (
            const   TWindow&                wndOwner
            , const tCIDLib::TKVPCollect&   colList
            , const TString&                strTitle
            , const TString&                strKeyTitle
            , const TString&                strValueTitle
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
        //  m_pcolList
        //      A pointer to the incoming collection of key/value pairs that we use
        //      to fill the list.
        //
        //  m_pwndXXX
        //      Some typed pointers we get to some of our child widgets we need to
        //      interact with on a typed basis.
        //
        //  m_strTitle
        //      The incoming title is put here until we can get it set.
        // -------------------------------------------------------------------
        const tCIDLib::TKVPCollect* m_pcolList;
        TPushButton*                m_pwndCloseButton;
        TMultiColListBox*           m_pwndList;
        TString                     m_strKeyTitle;
        TString                     m_strTitle;
        TString                     m_strValueTitle;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TShowKVPListDlg,TDlgBox)
};

#pragma CIDLIB_POPPACK

