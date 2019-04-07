//
// FILE NAME: CIDResEd_NewDlgDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/28/2010
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
//  This module implements the about dialog class for the dialog editor.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDResEd.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TResEdNewDlgDlg, TDlgBox)



// ---------------------------------------------------------------------------
//  CLASS: TResEdNewDlgDlg
// PREFIX: dlgb
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResEdNewDlgDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TResEdNewDlgDlg::TResEdNewDlgDlg() :

    m_pcolCurList(0)
    , m_pwndList(0)
    , m_pwndName(0)
{
}

TResEdNewDlgDlg::~TResEdNewDlgDlg()
{
    Destroy();
}


// ---------------------------------------------------------------------------
//  TResEdNewDlgDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TResEdNewDlgDlg::bRunDlg(       TWindow&                    wndOwner
                        , const tCIDResEd::TDlgDescList&    colCurList
                        ,       TString&                    strToFill)
{
    // Save in the incomg list for loading up
    m_pcolCurList = &colCurList;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDResEd, kCIDResEd::ridDlg_NewDlg
    );

    if (c4Res && (c4Res != kCIDLib::c4MaxCard))
    {
        strToFill = m_strNewName;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TResEdNewDlgDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TResEdNewDlgDlg::bCreated()
{
    // Return our parent's return, which indicates if he set the focus or not
    const tCIDLib::TBoolean bRet = TParent::bCreated();

    // Get our typed pointers to important controls
    CastChildWnd(*this, kCIDResEd::ridDlg_NewDlg_List, m_pwndList);
    CastChildWnd(*this, kCIDResEd::ridDlg_NewDlg_Name, m_pwndName);

    // Install event handlers for what we need
    pwndChildAs<TPushButton>(kCIDResEd::ridDlg_NewDlg_Cancel)->pnothRegisterHandler
    (
        this, &TResEdNewDlgDlg::eClickHandler
    );
    pwndChildAs<TPushButton>(kCIDResEd::ridDlg_NewDlg_Save)->pnothRegisterHandler
    (
        this, &TResEdNewDlgDlg::eClickHandler
    );

    // Load up the current list of dialog names to the list box
    const tCIDLib::TCard4 c4Count = m_pcolCurList->c4ElemCount();
    tCIDLib::TStrIdList colNames(c4Count);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        colNames.objAdd(TStringId(m_pcolCurList->objAt(c4Index).strName(), 0));
    m_pwndList->LoadList(colNames);

    return bRet;
}


// ---------------------------------------------------------------------------
//  TResEdNewDlgDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses
TResEdNewDlgDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDResEd::ridDlg_NewDlg_Cancel)
    {
        EndDlg(0);
    }
     else if (wnotEvent.widSource() == kCIDResEd::ridDlg_NewDlg_Save)
    {
        //
        //  Get the text. It can't be empty or the same as any existing
        //  dialog name.
        //
        const TString& strTest = m_pwndName->strWndText();
        tCIDLib::TBoolean bRes = !strTest.bIsEmpty();

        if (bRes)
        {
            const tCIDLib::TCard4 c4Count = m_pcolCurList->c4ElemCount();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                if (m_pcolCurList->objAt(c4Index).strName() == strTest)
                {
                    TErrBox msgbErr(facCIDResEd.strMsg(kResEdErrs::errcGen_DupName, strTest));
                    msgbErr.ShowIt(*this);
                    bRes = kCIDLib::False;
                    break;
                }
            }
        }
         else
        {
            TErrBox msgbErr(facCIDResEd.strMsg(kResEdErrs::errcGen_EmptyName));
            msgbErr.ShowIt(*this);
        }

        if (bRes)
        {
            m_strNewName = strTest;
            EndDlg(1);
        }
    }
    return tCIDCtrls::EEvResponses::Handled;
}



