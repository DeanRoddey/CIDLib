//
// FILE NAME: CIDWUtils_EditTextDlg_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/27/2015
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
//  This is the header for the CIDWUtils_EditTextDlg.cpp file, which provides a
//  simple dialog for editing single or multi-line text. We don't try to force it
//  to be single line.
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
//   CLASS: TEditTextDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TEditTextDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TEditTextDlg();

        TEditTextDlg(const TEditTextDlg&) = delete;
        TEditTextDlg(TEditTextDlg&&) = delete;

        ~TEditTextDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TEditTextDlg& operator=(const TEditTextDlg&) = delete;
        TEditTextDlg& operator=(TEditTextDlg&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRun
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const TString&                strOrgText
            ,       TString&                strNewText
            , const tCIDLib::TBoolean       bTakeCRs = kCIDLib::True
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
        //  m_bTakeCRs
        //      The caller tells us if he wants to accept CRs in the editor, to create
        //      multi-line text, or to treat them as Enter commands for the default
        //      button. Defaults to true.
        //
        //  m_strText
        //      To save the incoming text till we can get it loaded, and to save
        //      the edited text until we can get it back to the caller's parameter.
        //
        //  m_strTitle
        //      To save the provided title text until we can get it loaded.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bTakeCRs;
        TString             m_strText;
        TString             m_strTitle;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TEditTextDlg,TDlgBox)
};

#pragma CIDLIB_POPPACK



