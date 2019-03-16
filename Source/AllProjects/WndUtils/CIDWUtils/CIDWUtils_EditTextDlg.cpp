//
// FILE NAME: CIDWUtils_EditTextDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/26/2015
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
//  Implements the TEditTextDlg dialog.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDWUtils_.hpp"
#include    "CIDWUtils_EditTextDlg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TEditTextDlg, TDlgBox)



// ---------------------------------------------------------------------------
//   CLASS: TEditTextDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TEditTextDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TEditTextDlg::TEditTextDlg() :

    m_bTakeCRs(kCIDLib::True)
{
}

TEditTextDlg::~TEditTextDlg()
{
}


// ---------------------------------------------------------------------------
//  TEditTextDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TEditTextDlg::bRun( const   TWindow&            wndOwner
                    , const TString&            strTitle
                    , const TString&            strOrgText
                    ,       TString&            strNewText
                    , const tCIDLib::TBoolean   bTakeCRs)
{
    // Store the incoming info until we can set it
    m_bTakeCRs  = bTakeCRs;
    m_strTitle  = strTitle;
    m_strText   = strOrgText;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_EdText
    );

    if (c4Res == kCIDWUtils::ridDlg_EdText_Save)
    {
        strNewText = m_strText;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TEditTextDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TEditTextDlg::bCreated()
{
    // Now call our parent first and return his return
    const tCIDLib::TBoolean bRes = TParent::bCreated();

    // Load up the text values we got
    strWndText(m_strTitle);

    TMultiEdit* pwndEdit = nullptr;
    CastChildWnd(*this, kCIDWUtils::ridDlg_EdText_Text, pwndEdit);
    pwndEdit->strWndText(m_strText);

    // Set the want CRs flag that we got
    pwndEdit->SetWantEnter(m_bTakeCRs);

    // Register a handler for the buttons
    pwndChildAs<TPushButton>(kCIDWUtils::ridDlg_EdText_Save)->pnothRegisterHandler
    (
        this, &TEditTextDlg::eClickHandler
    );
    pwndChildAs<TPushButton>(kCIDWUtils::ridDlg_EdText_Cancel)->pnothRegisterHandler
    (
        this, &TEditTextDlg::eClickHandler
    );
    return bRes;
}


// ---------------------------------------------------------------------------
//  TEditTextDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TEditTextDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    // If saving, store the text away before we exit
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_EdText_Save)
        m_strText = pwndChildById(kCIDWUtils::ridDlg_EdText_Text)->strWndText();

    EndDlg(wnotEvent.widSource());

    return tCIDCtrls::EEvResponses::Handled;
}


