//
// FILE NAME: CIDWUtils_TriChoiceDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/15/2015
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
//  Implements the TTriChoiceDlg dialog.
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
#include    "CIDWUtils_TriChoiceDlg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TTriChoiceDlg, TDlgBox)



// ---------------------------------------------------------------------------
//   CLASS: TTriChoiceDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTriChoiceDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TTriChoiceDlg::TTriChoiceDlg()
{
}

TTriChoiceDlg::~TTriChoiceDlg()
{
}


// ---------------------------------------------------------------------------
//  TTriChoiceDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TTriChoiceDlg::c4Run(const  TWindow&    wndOwner
                    , const TString&    strTitle
                    , const TString&    strInstruct
                    , const TString&    strOpt1
                    , const TString&    strOpt2
                    , const TString&    strOpt3)
{
    // Store the incoming text until we can set it
    m_strTitle    = strTitle;
    m_strInstruct = strInstruct;
    m_strOpt1     = strOpt1;
    m_strOpt2     = strOpt2;
    m_strOpt3     = strOpt3;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_TriChoice
    );

    // If zero or max card, return 3
    if (!c4Res || (c4Res == kCIDLib::c4MaxCard))
        return 3;

    // Else just return the value
    return c4Res;
}


// ---------------------------------------------------------------------------
//  TTriChoiceDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TTriChoiceDlg::bCreated()
{
    // Now call our parent first and return his return
    const tCIDLib::TBoolean bRes = TParent::bCreated();

    // Load up the text values we got
    strWndText(m_strTitle);
    pwndChildById(kCIDWUtils::ridDlg_TriChoice_Instruct)->strWndText(m_strInstruct);
    pwndChildById(kCIDWUtils::ridDlg_TriChoice_Opt1)->strWndText(m_strOpt1);
    pwndChildById(kCIDWUtils::ridDlg_TriChoice_Opt2)->strWndText(m_strOpt2);
    pwndChildById(kCIDWUtils::ridDlg_TriChoice_Opt3)->strWndText(m_strOpt3);

    // Register a handler for the buttons
    pwndChildAs<TPushButton>(kCIDWUtils::ridDlg_TriChoice_Opt1)->pnothRegisterHandler
    (
        this, &TTriChoiceDlg::eClickHandler
    );
    pwndChildAs<TPushButton>(kCIDWUtils::ridDlg_TriChoice_Opt2)->pnothRegisterHandler
    (
        this, &TTriChoiceDlg::eClickHandler
    );
    pwndChildAs<TPushButton>(kCIDWUtils::ridDlg_TriChoice_Opt3)->pnothRegisterHandler
    (
        this, &TTriChoiceDlg::eClickHandler
    );
    return bRes;
}


// ---------------------------------------------------------------------------
//  TTriChoiceDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TTriChoiceDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_TriChoice_Opt1)
    {
        EndDlg(1);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_TriChoice_Opt2)
    {
        EndDlg(2);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_TriChoice_Opt3)
    {
        EndDlg(0);
    }
    return tCIDCtrls::EEvResponses::Handled;
}


