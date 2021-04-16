//
// FILE NAME: CIDWUtils_ShowListDlg_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/28/2014
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
//  This is the header for the CIDWUtils_ShowListDlg.cpp file, which implements a
//  simple dialog for showing a list of text values, which is quite common. It
//  just has a close button.
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
//   CLASS: TShowListDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TShowListDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TShowListDlg();

        TShowListDlg(const TShowListDlg&) = delete;
        TShowListDlg(TShowListDlg&&) = delete;

        ~TShowListDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TShowListDlg& operator=(const TShowListDlg&) = delete;
        TShowListDlg& operator=(TShowListDlg&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Run
        (
            const   TWindow&                wndOwner
            , const tCIDLib::TStrCollect&   colList
            , const TString&                strTitle
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
        //      A pointer to the incoming collection of strings that we use
        //      to fill the list.
        //
        //  m_pwndXXX
        //      Some typed pointers we get to some of our child widgets we
        //      need to interact with on a typed basis.
        //
        //  m_strTitle
        //      The incoming title is put here until we can get it set.
        // -------------------------------------------------------------------
        const tCIDLib::TStrCollect* m_pcolList;
        TPushButton*                m_pwndCloseButton;
        TListBox*                   m_pwndList;
        TString                     m_strTitle;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TShowListDlg,TDlgBox)
};

#pragma CIDLIB_POPPACK

