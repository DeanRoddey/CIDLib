//
// FILE NAME: CIDWUtils_ErrOptsDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/28/2002
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
//  Implements the TErrOptsDlg dialog.
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
#include    "CIDWUtils_ErrOptsDlg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TErrOptsDlg, TDlgBox)
RTTIDecls(TErrOptsDlg2, TDlgBox)



// ---------------------------------------------------------------------------
//   CLASS: TErrOptsDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TErrOptsDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TErrOptsDlg::TErrOptsDlg() :

    m_bDiscardMode(kCIDLib::False)
    , m_bDoAll(kCIDLib::False)
    , m_bMultiMode(kCIDLib::False)
    , m_pwndCancel(nullptr)
    , m_pwndDo(nullptr)
    , m_pwndDoAll(nullptr)
    , m_pwndInstruct(nullptr)
    , m_pwndOpName(nullptr)
{
}

TErrOptsDlg::~TErrOptsDlg()
{
}


// ---------------------------------------------------------------------------
//  TErrOptsDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDWUtils::EErrOpts
TErrOptsDlg::eRun(  const   TWindow&            wndOwner
                    ,       tCIDLib::TBoolean&  bDoAll
                    , const TString&            strOpName
                    , const tCIDLib::TBoolean   bMultiMode
                    , const tCIDLib::TBoolean   bDiscardMode)
{
    // Store the info till we can put it to use
    m_bDiscardMode = bDiscardMode;
    m_bMultiMode = bMultiMode;
    m_strOpName = strOpName;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_ErrOpts
    );

    if (c4Res == kCIDWUtils::ridDlg_ErrOpts_Do)
    {
        bDoAll = m_bDoAll;
        return tCIDWUtils::EErrOpts::Ignore;
    }
    return tCIDWUtils::EErrOpts::Cancel;
}


// ---------------------------------------------------------------------------
//  TErrOptsDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TErrOptsDlg::bCreated()
{
    // Now call our parent first and return his return
    const tCIDLib::TBoolean bRes = TParent::bCreated();

    // Get some typed pointers to some of our widgets, for convenience
    CastChildWnd(*this, kCIDWUtils::ridDlg_ErrOpts_Cancel, m_pwndCancel);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ErrOpts_Do, m_pwndDo);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ErrOpts_DoAll, m_pwndDoAll);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ErrOpts_Instruct, m_pwndInstruct);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ErrOpts_OpName, m_pwndOpName);

    // Load up the incoming name of the op that failed
    m_pwndOpName->strWndText(m_strOpName);

    //
    //  Set the appropriate text for the ignore button, the check box, and the instruction
    //  text.
    //
    tCIDLib::TMsgId midButton;
    tCIDLib::TMsgId midCheck;
    tCIDLib::TMsgId midInstruct;
    if (m_bDiscardMode)
    {
        midButton = kWUtilMsgs::midDlg_ErrOpts_Discard;
        midCheck = kWUtilMsgs::midDlg_ErrOpts_DiscardAll;
        midInstruct = kWUtilMsgs::midDlg_ErrOpts_DiscardInstruct;
    }
     else
    {
        midButton = kWUtilMsgs::midDlg_ErrOpts_Ignore;
        midCheck = kWUtilMsgs::midDlg_ErrOpts_IgnoreAll;
        midInstruct = kWUtilMsgs::midDlg_ErrOpts_IgnoreInstruct;
    }
    m_pwndDo->strWndText(facCIDWUtils().strMsg(midButton));
    m_pwndInstruct->strWndText(facCIDWUtils().strMsg(midInstruct));

    // If in multi-mode, set the incoming state, else hide the check box
    if (m_bMultiMode)
    {
        m_pwndDoAll->SetCheckedState(m_bDoAll);
        m_pwndDoAll->strWndText(facCIDWUtils().strMsg(midCheck));
    }
    else
    {
        m_pwndDoAll->SetEnable(kCIDLib::False);
        m_pwndDoAll->SetVisibility(kCIDLib::False);
    }

    // Register our handlers
    m_pwndCancel->pnothRegisterHandler(this, &TErrOptsDlg::eClickHandler);
    m_pwndDo->pnothRegisterHandler(this, &TErrOptsDlg::eClickHandler);

    return bRes;
}



// ---------------------------------------------------------------------------
//  TErrOptsDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses
TErrOptsDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ErrOpts_Cancel)
    {
        EndDlg(kCIDWUtils::ridDlg_ErrOpts_Cancel);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ErrOpts_Do)
    {
        if (m_bMultiMode)
            m_bDoAll = m_pwndDoAll->bCheckedState();
        EndDlg(kCIDWUtils::ridDlg_ErrOpts_Do);
    }
    return tCIDCtrls::EEvResponses::Handled;
}





// ---------------------------------------------------------------------------
//   CLASS: TErrOptsDlg2
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TErrOptsDlg2: Constructors and Destructor
// ---------------------------------------------------------------------------
TErrOptsDlg2::TErrOptsDlg2() :

    m_bDoAll(kCIDLib::False)
    , m_bDiscardMode(kCIDLib::False)
    , m_bMultiMode(kCIDLib::False)
    , m_pwndCancel(nullptr)
    , m_pwndErrorMsg(nullptr)
    , m_pwndDo(nullptr)
    , m_pwndDoAll(nullptr)
    , m_pwndInstruct(nullptr)
    , m_pwndTarget(nullptr)
{
}

TErrOptsDlg2::~TErrOptsDlg2()
{
}


// ---------------------------------------------------------------------------
//  TErrOptsDlg2: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDWUtils::EErrOpts
TErrOptsDlg2::eRun( const   TWindow&            wndOwner
                    ,       tCIDLib::TBoolean&  bDoAll
                    , const TString&            strTarget
                    , const TString&            strErrMsg
                    , const tCIDLib::TBoolean   bMultiMode
                    , const tCIDLib::TBoolean   bDiscardMode)
{
    // Store the info until we can get it stored
    m_bDiscardMode = bDiscardMode;
    m_bMultiMode = bMultiMode;
    m_strErrorMsg = strErrMsg;
    m_strTarget = strTarget;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_ErrOpts2
    );

    if (c4Res == kCIDWUtils::ridDlg_ErrOpts2_Do)
    {
        bDoAll = m_bDoAll;
        return tCIDWUtils::EErrOpts::Ignore;
    }
    return tCIDWUtils::EErrOpts::Cancel;
}


// ---------------------------------------------------------------------------
//  TErrOptsDlg2: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TErrOptsDlg2::bCreated()
{
    // Now call our parent first and return his return
    const tCIDLib::TBoolean bRes = TParent::bCreated();

    // Get some typed pointers to some of our widgets, for convenience
    CastChildWnd(*this, kCIDWUtils::ridDlg_ErrOpts2_Cancel, m_pwndCancel);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ErrOpts2_ErrorMsg, m_pwndErrorMsg);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ErrOpts2_Do, m_pwndDo);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ErrOpts2_DoAll, m_pwndDoAll);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ErrOpts2_Instruct, m_pwndInstruct);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ErrOpts2_Target, m_pwndTarget);

    // Load up the incoming target and error message
    m_pwndTarget->strWndText(m_strTarget);
    m_pwndErrorMsg->strWndText(m_strErrorMsg);

    //
    //  Set the appropriate text for the ignore button, the check box, and the instruction
    //  text.
    //
    tCIDLib::TMsgId midButton;
    tCIDLib::TMsgId midCheck;
    tCIDLib::TMsgId midInstruct;
    if (m_bDiscardMode)
    {
        midButton = kWUtilMsgs::midDlg_ErrOpts_Discard;
        midCheck = kWUtilMsgs::midDlg_ErrOpts_DiscardAll;
        midInstruct = kWUtilMsgs::midDlg_ErrOpts_DiscardInstruct;
    }
     else
    {
        midButton = kWUtilMsgs::midDlg_ErrOpts_Ignore;
        midCheck = kWUtilMsgs::midDlg_ErrOpts_IgnoreAll;
        midInstruct = kWUtilMsgs::midDlg_ErrOpts_IgnoreInstruct;
    }
    m_pwndDo->strWndText(facCIDWUtils().strMsg(midButton));
    m_pwndInstruct->strWndText(facCIDWUtils().strMsg(midInstruct));

    // If in multi-mode, set the incoming state, else disable the check box
    if (m_bMultiMode)
    {
        m_pwndDoAll->SetCheckedState(m_bDoAll);
        m_pwndDoAll->strWndText(facCIDWUtils().strMsg(midCheck));
    }
    else
    {
        m_pwndDoAll->SetEnable(kCIDLib::False);
        m_pwndDoAll->SetVisibility(kCIDLib::False);
    }

    // Register our handlers
    m_pwndCancel->pnothRegisterHandler(this, &TErrOptsDlg2::eClickHandler);
    m_pwndDo->pnothRegisterHandler(this, &TErrOptsDlg2::eClickHandler);

    return bRes;
}



// ---------------------------------------------------------------------------
//  TErrOptsDlg2: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses
TErrOptsDlg2::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ErrOpts2_Cancel)
    {
        EndDlg(kCIDWUtils::ridDlg_ErrOpts2_Cancel);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ErrOpts2_Do)
    {
        if (m_bMultiMode)
            m_bDoAll = m_pwndDoAll->bCheckedState();
        EndDlg(kCIDWUtils::ridDlg_ErrOpts2_Do);
    }
    return tCIDCtrls::EEvResponses::Handled;
}
