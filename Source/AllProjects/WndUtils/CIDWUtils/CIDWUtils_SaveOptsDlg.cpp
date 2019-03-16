//
// FILE NAME: CIDWUtils_SaveOptsDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/28/2002
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
//  Implements the TSaveOptsDlg dialog.
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
#include    "CIDWUtils_SaveOptsDlg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TSaveOptsDlg, TDlgBox)



// ---------------------------------------------------------------------------
//   CLASS: TSaveOptsDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSaveOptsDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TSaveOptsDlg::TSaveOptsDlg() :

    m_bDoAll(kCIDLib::False)
    , m_bMultiMode(kCIDLib::False)
    , m_pwndCancel(nullptr)
    , m_pwndDiscard(nullptr)
    , m_pwndDoAll(nullptr)
    , m_pwndSave(nullptr)
{
}

TSaveOptsDlg::~TSaveOptsDlg()
{
}


// ---------------------------------------------------------------------------
//  TSaveOptsDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDWUtils::ESaveOpts
TSaveOptsDlg::eRun( const   TWindow&            wndOwner
                    ,       tCIDLib::TBoolean&  bDoAll
                    , const TString&            strSource
                    , const tCIDLib::TBoolean   bMultiMode)
{
    // Store the target until we can get it stored
    m_bMultiMode = bMultiMode;
    m_strSource = strSource;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_SaveOpts
    );

    // Give back the 'do all' check box state, though it may be meaningless
    bDoAll = m_bDoAll;
    if (c4Res == kCIDWUtils::ridDlg_SaveOpts_Discard)
        return tCIDWUtils::ESaveOpts::Discard;
    else if (c4Res == kCIDWUtils::ridDlg_SaveOpts_Save)
        return tCIDWUtils::ESaveOpts::Save;

    return tCIDWUtils::ESaveOpts::Cancel;
}


// ---------------------------------------------------------------------------
//  TSaveOptsDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TSaveOptsDlg::bCreated()
{
    // Now call our parent first and return his return
    const tCIDLib::TBoolean bRes = TParent::bCreated();

    // Get some typed pointers to some of our widgets, for convenience
    CastChildWnd(*this, kCIDWUtils::ridDlg_SaveOpts_Cancel, m_pwndCancel);
    CastChildWnd(*this, kCIDWUtils::ridDlg_SaveOpts_Discard, m_pwndDiscard);
    CastChildWnd(*this, kCIDWUtils::ridDlg_SaveOpts_DoAll, m_pwndDoAll);
    CastChildWnd(*this, kCIDWUtils::ridDlg_SaveOpts_Save, m_pwndSave);
    CastChildWnd(*this, kCIDWUtils::ridDlg_SaveOpts_Source, m_pwndSource);

    // Load up the incoming source
    m_pwndSource->strWndText(m_strSource);

    // Register our handlers
    m_pwndCancel->pnothRegisterHandler(this, &TSaveOptsDlg::eClickHandler);
    m_pwndDiscard->pnothRegisterHandler(this, &TSaveOptsDlg::eClickHandler);
    m_pwndSave->pnothRegisterHandler(this, &TSaveOptsDlg::eClickHandler);

    // If in multi-mode, set the incoming state, else hide the check box
    if (m_bMultiMode)
        m_pwndDoAll->SetCheckedState(m_bDoAll);
    else
        m_pwndDoAll->SetVisibility(kCIDLib::False);

    return bRes;
}


// ---------------------------------------------------------------------------
//  TSaveOptsDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TSaveOptsDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_SaveOpts_Cancel)
    {
        EndDlg(kCIDWUtils::ridDlg_SaveOpts_Cancel);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_SaveOpts_Save)
    {
        m_bDoAll = m_pwndDoAll->bCheckedState();
        EndDlg(kCIDWUtils::ridDlg_SaveOpts_Save);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_SaveOpts_Cancel)
    {
        m_bDoAll = m_pwndDoAll->bCheckedState();
        EndDlg(kCIDWUtils::ridDlg_SaveOpts_Cancel);
    }
    return tCIDCtrls::EEvResponses::Handled;
}

