//
// FILE NAME: CIDWUtils_ShowTextDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/28/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  Implements some simple dialogs to get a line of text from the user.
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
#include    "CIDWUtils_ShowTextDlg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TShowTextDlg, TDlgBox)



// ---------------------------------------------------------------------------
//   CLASS: TShowTextDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TShowTextDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TShowTextDlg::TShowTextDlg() :

    m_pstrText(nullptr)
{
}

TShowTextDlg::~TShowTextDlg()
{
}


// ---------------------------------------------------------------------------
//  TShowTextDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TShowTextDlg::Run(  const   TWindow&    wndOwner
                    , const TString&    strTitle
                    , const TString&    strToShow)
{
    // Store the incoming info until we can set it
    m_strTitle  = strTitle;
    m_pstrText  = &strToShow;

    c4RunDlg(wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_ShowText);

}


// ---------------------------------------------------------------------------
//  TShowTextDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TShowTextDlg::bCreated()
{
    // Now call our parent first and return his return
    const tCIDLib::TBoolean bRes = TParent::bCreated();

    // Set the title text we were given
    strWndText(m_strTitle);

    // And load up the incoming display text (Default is not to adopt)
    TTextStringInStream strmSrc(m_pstrText);
    pwndChildById(kCIDWUtils::ridDlg_ShowText_Text)->strWndText(*m_pstrText);

    // Register a handler for the OK button
    pwndChildAs<TPushButton>(kCIDWUtils::ridDlg_ShowText_OK)->pnothRegisterHandler
    (
        this, &TShowTextDlg::eClickHandler
    );

    return bRes;
}


// ---------------------------------------------------------------------------
//  TShowTextDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TShowTextDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    EndDlg(wnotEvent.widSource());
    return tCIDCtrls::EEvResponses::Handled;
}


