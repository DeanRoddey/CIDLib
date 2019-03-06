//
// FILE NAME: CIDWUtils_OverOptsDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/28/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  Implements the TOverOptsDlg dialog.
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
#include    "CIDWUtils_OverOptsDlg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TOverOptsDlg, TDlgBox)



// ---------------------------------------------------------------------------
//   CLASS: TOverOptsDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TOverOptsDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TOverOptsDlg::TOverOptsDlg() :

    m_bDoAll(kCIDLib::False)
    , m_bMultiMode(kCIDLib::False)
    , m_pwndCancel(nullptr)
    , m_pwndDiscard(nullptr)
    , m_pwndDoAll(nullptr)
    , m_pwndOverwrite(nullptr)
{
}

TOverOptsDlg::~TOverOptsDlg()
{
}


// ---------------------------------------------------------------------------
//  TOverOptsDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDWUtils::EOverOpts
TOverOptsDlg::eRun( const   TWindow&            wndOwner
                    ,       tCIDLib::TBoolean&  bDoAll
                    , const TString&            strTarget
                    , const tCIDLib::TBoolean   bMultiMode)
{
    // Store the info until we can use it
    m_bMultiMode = bMultiMode;
    m_strTarget = strTarget;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_OverOpts
    );

    bDoAll = m_bDoAll;
    if (c4Res == kCIDWUtils::ridDlg_OverOpts_Discard)
        return tCIDWUtils::EOverOpts::Discard;
    else if (c4Res == kCIDWUtils::ridDlg_OverOpts_Overwrite)
        return tCIDWUtils::EOverOpts::Overwrite;

    return tCIDWUtils::EOverOpts::Cancel;
}


// ---------------------------------------------------------------------------
//  TOverOptsDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TOverOptsDlg::bCreated()
{
    // Now call our parent first and return his return
    const tCIDLib::TBoolean bRes = TParent::bCreated();

    // Get some typed pointers to some of our widgets, for convenience
    CastChildWnd(*this, kCIDWUtils::ridDlg_OverOpts_Cancel, m_pwndCancel);
    CastChildWnd(*this, kCIDWUtils::ridDlg_OverOpts_Discard, m_pwndDiscard);
    CastChildWnd(*this, kCIDWUtils::ridDlg_OverOpts_DoAll, m_pwndDoAll);
    CastChildWnd(*this, kCIDWUtils::ridDlg_OverOpts_Overwrite, m_pwndOverwrite);
    CastChildWnd(*this, kCIDWUtils::ridDlg_OverOpts_Target, m_pwndTarget);

    // Load up the incoming target
    m_pwndTarget->strWndText(m_strTarget);

    // If in multi-mode, set the incoming state, else disable the check box
    if (m_bMultiMode)
        m_pwndDoAll->SetCheckedState(m_bDoAll);
    else
        m_pwndDoAll->SetVisibility(kCIDLib::False);

    // Register our handlers
    m_pwndCancel->pnothRegisterHandler(this, &TOverOptsDlg::eClickHandler);
    m_pwndDiscard->pnothRegisterHandler(this, &TOverOptsDlg::eClickHandler);
    m_pwndOverwrite->pnothRegisterHandler(this, &TOverOptsDlg::eClickHandler);

    return bRes;
}



// ---------------------------------------------------------------------------
//  TOverOptsDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses
TOverOptsDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_OverOpts_Discard)
    {
        m_bDoAll = m_pwndDoAll->bCheckedState();
        EndDlg(kCIDWUtils::ridDlg_OverOpts_Discard);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_OverOpts_Overwrite)
    {
        m_bDoAll = m_pwndDoAll->bCheckedState();
        EndDlg(kCIDWUtils::ridDlg_OverOpts_Overwrite);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_OverOpts_Cancel)
    {
        EndDlg(kCIDWUtils::ridDlg_OverOpts_Cancel);
    }
    return tCIDCtrls::EEvResponses::Handled;
}

