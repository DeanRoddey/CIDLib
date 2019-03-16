//
// FILE NAME: CIDWUtils_ShowListDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/28/2014
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
//  This file implements a dialog that just shows a list.
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
#include    "CIDWUtils_.hpp"
#include    "CIDWUtils_ShowListDlg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TShowListDlg,TDlgBox)



// ---------------------------------------------------------------------------
//   CLASS: TShowListDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TShowListDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TShowListDlg::TShowListDlg() :

    m_pcolList(nullptr)
    , m_pwndCloseButton(nullptr)
    , m_pwndList(nullptr)
{
}

TShowListDlg::~TShowListDlg()
{
}


// ---------------------------------------------------------------------------
//  TShowListDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TShowListDlg::Run(  const   TWindow&                wndOwner
                    , const tCIDLib::TStrCollect&   colList
                    , const TString&                strTitle)
{
    // Save incoming stuff for later user
    m_pcolList = &colList;
    m_strTitle = strTitle;

    c4RunDlg(wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_ShowList);
}


// ---------------------------------------------------------------------------
//  TShowListDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TShowListDlg::bCreated()
{
    // Call our parent first
    const tCIDLib::TBoolean bRes = TParent::bCreated();

    // Set our title text as provided by the caller
    strWndText(m_strTitle);

    // First get typed pointers to the widget we always keep
    CastChildWnd(*this, kCIDWUtils::ridDlg_ShowList_Close, m_pwndCloseButton);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ShowList_List, m_pwndList);

    // Register our handlers
    m_pwndCloseButton->pnothRegisterHandler(this, &TShowListDlg::eClickHandler);

    // Load up the list
    TColCursor<TString>* pcursList = m_pcolList->pcursNew();
    TJanitor<TColCursor<TString>> janCurs(pcursList);

    if (pcursList->bReset())
    {
        tCIDLib::TCard4 c4Index = 0;
        do
        {
            m_pwndList->c4AddItem(pcursList->objRCur());
        }   while(pcursList->bNext());

        m_pwndList->SelectByIndex(0);
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TShowListDlg: Private, non-virutal methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TShowListDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    EndDlg(wnotEvent.widSource());
    return tCIDCtrls::EEvResponses::Handled;
}

