//
// FILE NAME: CIDWUtils_ShowKVPListDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/29/2016
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
#include    "CIDWUtils_ShowKVPListDlg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TShowKVPListDlg,TDlgBox)



// ---------------------------------------------------------------------------
//   CLASS: TShowKVPListDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TShowKVPListDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TShowKVPListDlg::TShowKVPListDlg() :

    m_pcolList(nullptr)
    , m_pwndCloseButton(nullptr)
    , m_pwndList(nullptr)
{
}

TShowKVPListDlg::~TShowKVPListDlg()
{
}


// ---------------------------------------------------------------------------
//  TShowKVPListDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TShowKVPListDlg::Run(   const   TWindow&                wndOwner
                        , const tCIDLib::TKVPCollect&   colList
                        , const TString&                strTitle
                        , const TString&                strKeyTitle
                        , const TString&                strValueTitle)
{
    // Save incoming stuff for later user
    m_pcolList = &colList;
    m_strTitle = strTitle;
    m_strKeyTitle = strKeyTitle;
    m_strValueTitle = strValueTitle;

    c4RunDlg(wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_ShowKVPList);
}


// ---------------------------------------------------------------------------
//  TShowKVPListDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TShowKVPListDlg::bCreated()
{
    // Call our parent first
    const tCIDLib::TBoolean bRes = TParent::bCreated();

    // Set our title text as provided by the caller
    strWndText(m_strTitle);

    // First get typed pointers to the widget we always keep
    CastChildWnd(*this, kCIDWUtils::ridDlg_ShowList_Close, m_pwndCloseButton);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ShowList_List, m_pwndList);

    //
    //  Set up the columns, making the 2nd column the initial sort columns since the
    //  first one is hidden.
    //
    tCIDLib::TStrList colCols(3);
    colCols.objAdd(TString::strEmpty());
    colCols.objAdd(m_strKeyTitle);
    colCols.objAdd(m_strValueTitle);
    m_pwndList->SetColumns(colCols);

    // Make sure the first one is zero sized
    m_pwndList->SetColOpts(0, 0);

    // Register our handlers
    m_pwndCloseButton->pnothRegisterHandler(this, &TShowKVPListDlg::eClickHandler);

    // Load up the list
    TColCursor<TKeyValuePair>* pcursList = m_pcolList->pcursNew();
    TJanitor<TColCursor<TKeyValuePair>> janCurs(pcursList);

    if (pcursList->bReset())
    {
        tCIDLib::TCard4 c4Index = 0;
        do
        {
            const TKeyValuePair& kvalCur = pcursList->objRCur();
            colCols[1] = kvalCur.strKey();
            colCols[2] = kvalCur.strValue();

            m_pwndList->c4AddItem(colCols, c4Index++);
        }   while(pcursList->bNext());

        //
        //  Size the 2nd column to the content in it. The value column is set to
        //  stay sized to fit.
        //
        m_pwndList->AutoSizeCol(1, kCIDLib::True);

        // Select the 0th one
        m_pwndList->SelectByIndex(0);
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TShowKVPListDlg: Private, non-virutal methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TShowKVPListDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    EndDlg(wnotEvent.widSource());
    return tCIDCtrls::EEvResponses::Handled;
}

