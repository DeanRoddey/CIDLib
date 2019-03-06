//
// FILE NAME: CIDWUtils_ListSelDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/18/2015
//
// COPYRIGHT: 2015..2015, Charmed Quark Systems, Ltd
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a reusable dialog for generic selection from a among a
//  list of values.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $Log$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDWUtils_.hpp"
#include    "CIDWUtils_ListSelDlg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDListSelDlg, TDlgBox)
RTTIDecls(TCIDListSelDlg2, TDlgBox)
RTTIDecls(TCIDListSelDlg3, TDlgBox)
RTTIDecls(TCIDKVPListSelDlg, TDlgBox)
RTTIDecls(TCIDMListSelDlg, TDlgBox)
RTTIDecls(TCIDKVPMListSelDlg, TDlgBox)



// ---------------------------------------------------------------------------
//   CLASS: TCIDListSelDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDListSelDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDListSelDlg::TCIDListSelDlg() :

    m_bMultiMode(kCIDLib::False)
    , m_pcolList(nullptr)
    , m_pwndCancelButton(nullptr)
    , m_pwndList(nullptr)
    , m_pwndSelectButton(nullptr)
{
}

TCIDListSelDlg::~TCIDListSelDlg()
{
}


// ---------------------------------------------------------------------------
//  TCIDListSelDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDListSelDlg::bRun(const  TWindow&                wndOwner
                    , const TString&                strTitle
                    , const tCIDLib::TStrCollect&   colList
                    ,       tCIDLib::TStrCollect&   colValues
                    , const tCIDLib::TBoolean       bMultiMode)
{
    // If no values, nothing to do
    if (colList.bIsEmpty())
    {
        TErrBox msgbEmpty(strTitle, L"No values were available to select from");
        msgbEmpty.ShowIt(wndOwner);
        return kCIDLib::False;
    }

    // Store incoming stuff for later use
    m_bMultiMode = bMultiMode;
    m_pcolList = &colList;
    m_strTitle = strTitle;

    // Iterate the incoming values and store them in our list
    TColCursor<TString>* pcursVals = colValues.pcursNew();
    TJanitor<TColCursor<TString>> janCursor(pcursVals);
    if (pcursVals->bReset())
    {
        do
        {
            m_colValues.objAdd(pcursVals->objRCur());
        }   while (pcursVals->bNext());
    }

    //
    //  Since we have to support either single or multi mode, we have to adjust a style
    //  on the list box. So we pre-load the dialog description and update it, then invoke
    //  the dialog box on that.
    //
    TDlgDesc dlgdChildren;
    facCIDWUtils().bCreateDlgDesc(kCIDWUtils::ridDlg_ListSel, dlgdChildren);

    if (bMultiMode)
    {
        TDlgItem& dlgiList = dlgdChildren.dlgiFindById(kCIDWUtils::ridDlg_ListSel_List);
        dlgiList.AddHint(kCIDCtrls::strHint_MultiVal);
    }

    const tCIDLib::TCard4 c4Res = c4RunDlg(wndOwner, dlgdChildren);

    if (c4Res == kCIDWUtils::ridDlg_ListSel_Select)
    {
        colValues.RemoveAll();
        const tCIDLib::TCard4 c4Count = m_colValues.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            colValues.objAdd(m_colValues[c4Index]);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TCIDListSelDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCIDListSelDlg::bCreated()
{
    // Call our parent first
    const tCIDLib::TBoolean bRet = TParent::bCreated();

    // First get typed pointers to the widget we always keep
    CastChildWnd(*this, kCIDWUtils::ridDlg_ListSel_Cancel, m_pwndCancelButton);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ListSel_List, m_pwndList);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ListSel_Select, m_pwndSelectButton);

    // Set the columns on the driver list. We have one column
    tCIDLib::TStrList colCols(1);
    colCols.objAdd(facCIDWUtils().strMsg(kWUtilMsgs::midDlg_ListSel_ListPref));
    m_pwndList->SetColumns(colCols);

    // Load up the value list, selecting the initial one, if any.
    TColCursor<TString>* pcursList = m_pcolList->pcursNew();
    TJanitor<TColCursor<TString>> janCursor(pcursList);
    if (pcursList->bReset())
    {
        do
        {
            colCols[0] = pcursList->objRCur();
            m_pwndList->c4AddItem(colCols, 0);
        }   while (pcursList->bNext());
    }

    // Select the initial values if any
    if (!m_colValues.bIsEmpty())
    {
        if (m_bMultiMode)
        {
            const tCIDLib::TCard4 c4Count = m_colValues.c4ElemCount();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                const tCIDLib::TCard4 c4At = m_pwndList->c4FindByText(m_colValues[c4Index]);
                if (c4At != kCIDLib::c4MaxCard)
                    m_pwndList->MarkAt(c4At, kCIDLib::True);
            }
        }
         else
        {
            m_pwndList->c4SelectByText(m_colValues[0]);
        }
    }
     else
    {
        if (!m_bMultiMode)
            m_pwndList->SelectByIndex(0, kCIDLib::True);
    }

    // Set our title text
    strWndText(m_strTitle);

    // Register our handlers
    m_pwndCancelButton->pnothRegisterHandler(this, &TCIDListSelDlg::eClickHandler);
    m_pwndSelectButton->pnothRegisterHandler(this, &TCIDListSelDlg::eClickHandler);

    // We only support double click selection for single selection mode
    if (!m_bMultiMode)
        m_pwndList->pnothRegisterHandler(this, &TCIDListSelDlg::eLBHandler);

    return bRet;
}


// ---------------------------------------------------------------------------
//  TCIDListSelDlg: Private, non-virutal methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TCIDListSelDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ListSel_Cancel)
    {
        EndDlg(kCIDWUtils::ridDlg_ListSel_Cancel);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ListSel_Select)
    {
        m_colValues.RemoveAll();
        if (m_bMultiMode)
        {
            TString strVal;
            m_colValues.RemoveAll();
            const tCIDLib::TCard4 c4Count = m_pwndList->c4ItemCount();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                if (m_pwndList->bIsMarked(c4Index))
                {
                    m_pwndList->QueryColText(c4Index, 0, strVal);
                    m_colValues.objAdd(strVal);
                }
            }
            EndDlg(kCIDWUtils::ridDlg_ListSel_Select);
        }
         else
        {
            const tCIDLib::TCard4 c4Index = m_pwndList->c4CurItem();
            if (c4Index != kCIDLib::c4MaxCard)
            {
                TString strVal;
                m_pwndList->QueryColText(c4Index, 0, strVal);
                m_colValues.objAdd(strVal);
                EndDlg(kCIDWUtils::ridDlg_ListSel_Select);
            }
        }
    }
    return tCIDCtrls::EEvResponses::Handled;
}


// Allow a double click selection if in single selection mode
tCIDCtrls::EEvResponses TCIDListSelDlg::eLBHandler(TListChangeInfo& wnotEvent)
{
    if ((wnotEvent.eEvent() == tCIDCtrls::EListEvents::Invoked) && !m_bMultiMode)
    {
        TString strVal;
        m_pwndList->QueryColText(wnotEvent.c4Index(), 0, strVal);
        m_colValues.objAdd(strVal);
        EndDlg(kCIDWUtils::ridDlg_ListSel_Select);
    }

    return tCIDCtrls::EEvResponses::Handled;
}



// ---------------------------------------------------------------------------
//   CLASS: TCIDListSelDlg2
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDListSelDlg2: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDListSelDlg2::TCIDListSelDlg2() :

    m_eFmt(tCIDWUtils::EListValFmts::String)
    , m_pcolList(nullptr)
    , m_pwndCancelButton(nullptr)
    , m_pwndList(nullptr)
    , m_pwndSelectButton(nullptr)
{
}

TCIDListSelDlg2::~TCIDListSelDlg2()
{
}


// ---------------------------------------------------------------------------
//  TCIDListSelDlg2: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDListSelDlg2::bRun(const TWindow&                    wndOwner
                    , const TString&                    strTitle
                    ,       tCIDLib::TStrList&          colList
                    ,       TString&                    strSelText
                    , const tCIDWUtils::EListValFmts    eFmt)
{
    // Store incoming stuff for later use
    m_eFmt = eFmt;
    m_pcolList = &colList;
    m_strValue = strSelText;
    m_strTitle = strTitle;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_ListSel2
    );

    if (c4Res == kCIDWUtils::ridDlg_ListSel2_Select)
    {
        strSelText = m_strValue;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TCIDListSelDlg2: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCIDListSelDlg2::bCreated()
{
    // Call our parent first
    const tCIDLib::TBoolean bRet = TParent::bCreated();

    // First get typed pointers to the widget we always keep
    CastChildWnd(*this, kCIDWUtils::ridDlg_ListSel2_Cancel, m_pwndCancelButton);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ListSel2_List, m_pwndList);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ListSel2_Select, m_pwndSelectButton);

    //
    //  Load up the value list if not empty, selecting the initial one, if any, else
    //  the first one.
    //
    const tCIDLib::TCard4 c4Count = m_pcolList->c4ElemCount();
    if (c4Count)
    {
        tCIDLib::TCard4 c4InitSel = kCIDLib::c4MaxCard;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TString& strCur = m_pcolList->objAt(c4Index);
            m_pwndList->c4AddItem(strCur, c4Index);
            if (strCur.bCompareI(m_strValue))
                c4InitSel = c4Index;
        }

        // Select the initial one we found above, if any, else just the 0th one
        if (c4InitSel == kCIDLib::c4MaxCard)
            c4InitSel = 0;

        m_pwndList->c4SelectById(c4InitSel);
    }

    // Set our title text
    strWndText(m_strTitle);

    // Register our handlers
    m_pwndCancelButton->pnothRegisterHandler(this, &TCIDListSelDlg2::eClickHandler);
    m_pwndSelectButton->pnothRegisterHandler(this, &TCIDListSelDlg2::eClickHandler);

    return bRet;
}


// ---------------------------------------------------------------------------
//  TCIDListSelDlg2: Private, non-virutal methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses
TCIDListSelDlg2::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ListSel_Cancel)
    {
        EndDlg(kCIDWUtils::ridDlg_ListSel2_Cancel);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ListSel_Select)
    {
        //
        //  Get the current value text. This gets us either the selected list item, or the
        //  entered text if any has been entered.
        //
        m_pwndList->bQueryCurVal(m_strValue);
        m_strValue.StripWhitespace();

        //
        //  Do some basic validation on it based on the format requested. If it fails,
        //  complain and don't take it.
        //
        tCIDLib::TMsgId midErr = 0;
        if ((m_eFmt == tCIDWUtils::EListValFmts::Cardinal)
        ||  (m_eFmt == tCIDWUtils::EListValFmts::NZCardinal))
        {
            tCIDLib::TCard4 c4Test;
            if (!m_strValue.bToCard4(c4Test, tCIDLib::ERadices::Auto))
                midErr = kWUtilErrs::errcVal_NotCard;
            else if (!c4Test && (m_eFmt == tCIDWUtils::EListValFmts::NZCardinal))
                midErr = kWUtilErrs::errcVal_ZeroValue;
        }
         else if (m_eFmt == tCIDWUtils::EListValFmts::Integer)
        {
            tCIDLib::TInt4 i4Test;
            if (!m_strValue.bToInt4(i4Test, tCIDLib::ERadices::Auto))
                midErr = kWUtilErrs::errcVal_NotInt;
        }
         else if (m_eFmt == tCIDWUtils::EListValFmts::StringNE)
        {
            if (m_strValue.bIsEmpty())
                midErr = kWUtilErrs::errcVal_Empty;
        }

        if (midErr == 0)
        {
            //
            //  Looks ok, so let's add it to the list if needed, or move this value to the
            //  top of the list if it's already there.
            //
            const tCIDLib::TCard4 c4Count = m_pcolList->c4ElemCount();
            tCIDLib::TCard4 c4Index = 0;
            for (; c4Index < c4Count; c4Index++)
            {
                if (m_pcolList->objAt(c4Index) == m_strValue)
                    break;
            }

            //
            //  If already in the list, and not at the top already, then remove it
            if (c4Index < c4Count)
            {
                // Already in the list. If not already at the top remove and reinsert
                m_pcolList->RemoveAt(c4Index);
                m_pcolList->InsertAt(m_strValue, 0);
            }
             else
            {
                // Not already there so add at the top
                m_pcolList->InsertAt(m_strValue, 0);
            }
            EndDlg(kCIDWUtils::ridDlg_ListSel2_Select);
        }
         else
        {
            TErrBox msgbFailed(facCIDWUtils().strMsg(midErr));
            msgbFailed.ShowIt(*this);
        }
    }
    return tCIDCtrls::EEvResponses::Handled;
}



// ---------------------------------------------------------------------------
//   CLASS: TCIDListSelDlg3
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDListSelDlg3: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDListSelDlg3::TCIDListSelDlg3() :

    m_bSort(kCIDLib::True)
    , m_c4InitSel(0)
    , m_c4SelIndex(kCIDLib::c4MaxCard)
    , m_pcolList(nullptr)
    , m_pwndCancelButton(nullptr)
    , m_pwndList(nullptr)
    , m_pwndSelectButton(nullptr)
{
}

TCIDListSelDlg3::~TCIDListSelDlg3()
{
}


// ---------------------------------------------------------------------------
//  TCIDListSelDlg3: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TCIDListSelDlg3::c4Run( const   TWindow&            wndOwner
                        , const TString&            strTitle
                        , const tCIDLib::TStrList&  colList
                        , const tCIDLib::TCard4     c4InitSel
                        , const tCIDLib::TBoolean   bSort)
{
    // If no values, nothing to do
    if (colList.bIsEmpty())
    {
        TErrBox msgbEmpty(strTitle, L"No values were available to select from");
        msgbEmpty.ShowIt(wndOwner);
        return kCIDLib::False;
    }

    // Store incoming stuff for later use
    m_bSort = bSort;
    m_c4InitSel = c4InitSel;
    m_pcolList = &colList;
    m_strTitle = strTitle;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_ListSel
    );

    if (c4Res == kCIDWUtils::ridDlg_ListSel_Select)
        return m_c4SelIndex;

    return kCIDLib::c4MaxCard;
}


// ---------------------------------------------------------------------------
//  TCIDListSelDlg3: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCIDListSelDlg3::bCreated()
{
    // Call our parent first
    const tCIDLib::TBoolean bRet = TParent::bCreated();

    // First get typed pointers to the widget we always keep
    CastChildWnd(*this, kCIDWUtils::ridDlg_ListSel_Cancel, m_pwndCancelButton);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ListSel_List, m_pwndList);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ListSel_Select, m_pwndSelectButton);

    // If they asked not to sort, force that style off on the list window
    if (!m_bSort)
        m_pwndList->SetMCLBStyle(tCIDCtrls::EMCLBStyles::Sorted, kCIDLib::False);

    // Set the columns on the driver list. We have one column
    tCIDLib::TStrList colCols(1);
    colCols.objAdd(facCIDWUtils().strMsg(kWUtilMsgs::midDlg_ListSel_ListPref));
    m_pwndList->SetColumns(colCols);

    //
    //  Load up the value list, selecting the initial one, if any. It may be sorted so
    //  we store the original index as the row id. And we we have to do the initial
    //  selection by text if sorted.
    //
    const tCIDLib::TCard4 c4Count = m_pcolList->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        colCols[0] = m_pcolList->objAt(c4Index);
        m_pwndList->c4AddItem(colCols, c4Index);
    }

    if (m_bSort)
    {
        tCIDLib::TCard4 c4At;
        m_pwndList->bSelectByText(m_pcolList->objAt(m_c4InitSel), c4At);
    }
     else
    {
        if (m_c4InitSel < c4Count)
            m_pwndList->SelectByIndex(m_c4InitSel);
        else
            m_pwndList->SelectByIndex(0);
    }

    // Set our title text
    strWndText(m_strTitle);

    // Register our handlers
    m_pwndCancelButton->pnothRegisterHandler(this, &TCIDListSelDlg3::eClickHandler);
    m_pwndSelectButton->pnothRegisterHandler(this, &TCIDListSelDlg3::eClickHandler);

    m_pwndList->pnothRegisterHandler(this, &TCIDListSelDlg3::eLBHandler);

    return bRet;
}


// ---------------------------------------------------------------------------
//  TCIDListSelDlg3: Private, non-virutal methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TCIDListSelDlg3::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ListSel_Cancel)
    {
        EndDlg(kCIDWUtils::ridDlg_ListSel_Cancel);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ListSel_Select)
    {
        m_c4SelIndex = m_pwndList->c4IndexToId(m_pwndList->c4CurItem());
        EndDlg(kCIDWUtils::ridDlg_ListSel_Select);
    }
    return tCIDCtrls::EEvResponses::Handled;
}


// Allow a double click selection if in single selection mode
tCIDCtrls::EEvResponses TCIDListSelDlg3::eLBHandler(TListChangeInfo& wnotEvent)
{
    if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::Invoked)
    {
        m_c4SelIndex = m_pwndList->c4IndexToId(wnotEvent.c4Index());
        EndDlg(kCIDWUtils::ridDlg_ListSel_Select);
    }
    return tCIDCtrls::EEvResponses::Handled;
}




// ---------------------------------------------------------------------------
//   CLASS: TCIDKVPListSelDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDKVPListSelDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDKVPListSelDlg::TCIDKVPListSelDlg() :

    m_bShowValue(kCIDLib::False)
    , m_pcolList(nullptr)
    , m_pwndCancelButton(nullptr)
    , m_pwndList(nullptr)
    , m_pwndSelectButton(nullptr)
{
}

TCIDKVPListSelDlg::~TCIDKVPListSelDlg()
{
}


// ---------------------------------------------------------------------------
//  TCIDKVPListSelDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKVPListSelDlg::bRun(const   TWindow&                wndOwner
                        , const TString&                strTitle
                        , const tCIDLib::TKVPCollect&   colList
                        ,       TKeyValuePair&          kvalSelText
                        , const tCIDLib::TBoolean       bShowValue)
{
    // Store incoming stuff for later use
    m_bShowValue = bShowValue;
    m_pcolList = &colList;
    m_kvalValue = kvalSelText;
    m_strTitle = strTitle;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_ListSel
    );

    if (c4Res == kCIDWUtils::ridDlg_ListSel_Select)
    {
        kvalSelText = m_kvalValue;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TCIDKVPListSelDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCIDKVPListSelDlg::bCreated()
{
    // Call our parent first
    const tCIDLib::TBoolean bRet = TParent::bCreated();

    // First get typed pointers to the widget we always keep
    CastChildWnd(*this, kCIDWUtils::ridDlg_ListSel_Cancel, m_pwndCancelButton);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ListSel_List, m_pwndList);
    CastChildWnd(*this, kCIDWUtils::ridDlg_ListSel_Select, m_pwndSelectButton);

    // We don't us the column titles but have to set them
    tCIDLib::TStrList colCols(1);
    colCols.objAdd(L"Name");
    m_pwndList->SetColumns(colCols);

    //
    //  Load up the value list if not empty, selecting the initial one, if any, else
    //  the first one. We use the key as the display text.
    //
    TColCursor<TKeyValuePair>* pcursLoad = m_pcolList->pcursNew();
    TJanitor<TColCursor<TKeyValuePair>> janCursor(pcursLoad);
    if (pcursLoad->bReset())
    {
        do
        {
            const TKeyValuePair& kvalCur = pcursLoad->objRCur();

            colCols[0] = kvalCur.strKey();
            if (m_bShowValue && !kvalCur.strValue().bIsEmpty())
            {
                colCols[0].Append(L" (");
                colCols[0].Append(kvalCur.strValue());
                colCols[0].Append(L")");
            }
            const tCIDLib::TCard4 c4At = m_pwndList->c4AddItem(colCols, 0);
            m_pwndList->SetUserDataAt(c4At, kvalCur.strKey().c4Length());
            m_pwndList->SetUserDataAt(c4At, kvalCur.strValue());

        }   while (pcursLoad->bNext());

        // Try to select any incoming one
        tCIDLib::TCard4 c4FoundAt;
        m_pwndList->bSelectByText(m_kvalValue.strKey(), c4FoundAt);
    }

    // Set our title text
    strWndText(m_strTitle);

    // Register our handlers
    m_pwndCancelButton->pnothRegisterHandler(this, &TCIDKVPListSelDlg::eClickHandler);
    m_pwndList->pnothRegisterHandler(this, &TCIDKVPListSelDlg::eLBHandler);
    m_pwndSelectButton->pnothRegisterHandler(this, &TCIDKVPListSelDlg::eClickHandler);

    return bRet;
}


// ---------------------------------------------------------------------------
//  TCIDKVPListSelDlg: Private, non-virutal methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TCIDKVPListSelDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ListSel_Cancel)
    {
        EndDlg(kCIDWUtils::ridDlg_ListSel_Cancel);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ListSel_Select)
    {
        const tCIDLib::TCard4 c4ListInd = m_pwndList->c4CurItem();
        if (c4ListInd != kCIDLib::c4MaxCard)
        {
            StoreSelection(c4ListInd);
            EndDlg(kCIDWUtils::ridDlg_ListSel_Select);
        }
    }
    return tCIDCtrls::EEvResponses::Handled;
}


// Allow a double click selection if in single selection mode
tCIDCtrls::EEvResponses TCIDKVPListSelDlg::eLBHandler(TListChangeInfo& wnotEvent)
{
    if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::Invoked)
    {
        StoreSelection(wnotEvent.c4Index());
        EndDlg(kCIDWUtils::ridDlg_ListSel_Select);
    }

    return tCIDCtrls::EEvResponses::Handled;
}


// Called to store the selection away for return to the caller
tCIDLib::TVoid TCIDKVPListSelDlg::StoreSelection(const tCIDLib::TCard4 c4Index)
{
    // Get the selected key
    TString strValue;
    m_pwndList->QueryColText(c4Index, 0, strValue);

    //
    //  If showing the value, then we have to remove that stuff. We stored the
    //  length of the key part in the user data. If there was no value, then
    //  we will basically just have no effect.
    //
    if (m_bShowValue)
        strValue.CapAt(m_pwndList->c4UserDataAt(c4Index));

    // Set up the return value
    m_kvalValue.Set(strValue, m_pwndList->strUserDataAt(c4Index));
}



// ---------------------------------------------------------------------------
//   CLASS: TCIDMListSelDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDMListSelDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDMListSelDlg::TCIDMListSelDlg() :

    m_pcolAvail(nullptr)
    , m_pcolSelected(nullptr)
    , m_pwndCancel(nullptr)
    , m_pwndList(nullptr)
    , m_pwndSelect(nullptr)
{
}

TCIDMListSelDlg::~TCIDMListSelDlg()
{
}


// ---------------------------------------------------------------------------
//  TCIDMListSelDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDMListSelDlg::bRun(const TWindow&            wndOwner
                    , const TString&            strTitle
                    , const TString&            strDataTitle
                    , const tCIDLib::TStrList&  colAvail
                    ,       tCIDLib::TStrList&  colSelected)
{
    // Store incoming stuff for later use
    m_pcolAvail = &colAvail;
    m_pcolSelected= &colSelected;
    m_strDataTitle = strDataTitle;
    m_strTitle = strTitle;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_MListSel
    );
    return (c4Res == kCIDWUtils::ridDlg_MListSel_Select);
}


// ---------------------------------------------------------------------------
//  TCIDMListSelDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCIDMListSelDlg::bCreated()
{
    // Call our parent first
    const tCIDLib::TBoolean bRet = TParent::bCreated();

    // First get typed pointers to the widget we always keep
    CastChildWnd(*this, kCIDWUtils::ridDlg_MListSel_Cancel, m_pwndCancel);
    CastChildWnd(*this, kCIDWUtils::ridDlg_MListSel_List, m_pwndList);
    CastChildWnd(*this, kCIDWUtils::ridDlg_MListSel_Select, m_pwndSelect);

    // Set the columns on the list box, with the text column being the 2nd one
    tCIDLib::TStrList colCols(2);
    colCols.objAdd(TString::strEmpty());
    colCols.objAdd(m_strDataTitle);
    m_pwndList->SetColumns(colCols, 1);

    // Set our own user provided caption
    strWndText(m_strTitle);

    // Load up the available values
    tCIDLib::TCard4 c4Count = m_pcolAvail->c4ElemCount();
    if (c4Count)
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            colCols[1] = m_pcolAvail->objAt(c4Index);
            m_pwndList->c4AddItem(colCols, c4Index);
        }
    }

    //
    //  If any selected items, check them, or remove them if they are not in the
    //  available values list.
    //
    c4Count = m_pcolSelected->c4ElemCount();
    tCIDLib::TCard4 c4Index = 0;
    while (c4Index < c4Count)
    {
        const tCIDLib::TCard4 c4At = m_pwndList->c4FindByText
        (
            m_pcolSelected->objAt(c4Index)
        );

        if (c4At == kCIDLib::c4MaxCard)
        {
            m_pcolSelected->RemoveAt(c4Index);
            c4Count--;
        }
         else
        {
            m_pwndList->SetCheckAt(c4At, kCIDLib::True);
            c4Index++;
        }
    }


    // Set our title text
    strWndText(m_strTitle);

    // Register our handlers
    m_pwndCancel->pnothRegisterHandler(this, &TCIDMListSelDlg::eClickHandler);
    m_pwndList->pnothRegisterHandler(this, &TCIDMListSelDlg::eListHandler);
    m_pwndSelect->pnothRegisterHandler(this, &TCIDMListSelDlg::eClickHandler);

    return bRet;
}


// ---------------------------------------------------------------------------
//  TCIDMListSelDlg: Private, non-virutal methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses
TCIDMListSelDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ListSel_Cancel)
        EndDlg(kCIDWUtils::ridDlg_MListSel_Cancel);
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ListSel_Select)
        EndDlg(kCIDWUtils::ridDlg_MListSel_Select);
    return tCIDCtrls::EEvResponses::Handled;
}


tCIDCtrls::EEvResponses TCIDMListSelDlg::eListHandler(TListChangeInfo& wnotEvent)
{
    // They toggled one of the values
    if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::Invoked)
    {
        // See if this one is currently enabled or disabled, and get the opposite
        tCIDLib::TBoolean bNewState = !m_pwndList->bIsCheckedAt(wnotEvent.c4Index());

        // If now disabled, remove it from the selected list, else add it.
        TString strItemName;
        m_pwndList->QueryColText(wnotEvent.c4Index(), 1, strItemName);

        // See if this name is in the selected list
        tCIDLib::TCard4 c4Count = m_pcolSelected->c4ElemCount();
        tCIDLib::TCard4 c4FoundAt = 0;
        while (c4FoundAt < c4Count)
        {
            if (m_pcolSelected->objAt(c4FoundAt) == strItemName)
                break;
            c4FoundAt++;
        }

        if (bNewState)
        {
            //
            //  If we don't already have one in the selected list with this name,
            //  then add it.
            //
            if (c4FoundAt == c4Count)
                m_pcolSelected->objAdd(strItemName);
        }
         else
        {
            // While there are any in the selected list with this name, remove them
            while (c4FoundAt < c4Count)
            {
                if (m_pcolSelected->objAt(c4FoundAt) == strItemName)
                {
                    m_pcolSelected->RemoveAt(c4FoundAt);
                    c4Count--;
                }
                 else
                {
                    c4FoundAt++;
                }
            }
        }

        // Update the check on this row
        m_pwndList->SetCheckAt(wnotEvent.c4Index(), bNewState);
    }
    return tCIDCtrls::EEvResponses::Handled;
}





// ---------------------------------------------------------------------------
//   CLASS: TCIDKVPMListSelDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDKVPMListSelDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDKVPMListSelDlg::TCIDKVPMListSelDlg() :

    m_pcolAvail(nullptr)
    , m_pcolSelected(nullptr)
    , m_pwndCancel(nullptr)
    , m_pwndList(nullptr)
    , m_pwndSelect(nullptr)
{
}

TCIDKVPMListSelDlg::~TCIDKVPMListSelDlg()
{
}


// ---------------------------------------------------------------------------
//  TCIDKVPMListSelDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKVPMListSelDlg::bRun(const  TWindow&            wndOwner
                        , const TString&            strTitle
                        , const TString&            strDataTitle
                        , const tCIDLib::TKVPList&  colAvail
                        ,       tCIDLib::TKVPList&  colSelected)
{
    // Store incoming stuff for later use
    m_pcolAvail = &colAvail;
    m_pcolSelected= &colSelected;
    m_strDataTitle = strDataTitle;
    m_strTitle = strTitle;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_MListSel
    );
    return (c4Res == kCIDWUtils::ridDlg_MListSel_Select);
}


// ---------------------------------------------------------------------------
//  TCIDKVPMListSelDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCIDKVPMListSelDlg::bCreated()
{
    // Call our parent first
    const tCIDLib::TBoolean bRet = TParent::bCreated();

    // First get typed pointers to the widget we always keep
    CastChildWnd(*this, kCIDWUtils::ridDlg_MListSel_Cancel, m_pwndCancel);
    CastChildWnd(*this, kCIDWUtils::ridDlg_MListSel_List, m_pwndList);
    CastChildWnd(*this, kCIDWUtils::ridDlg_MListSel_Select, m_pwndSelect);

    // Set the columns on the list box, with the text column being the 2nd one
    tCIDLib::TStrList colCols(2);
    colCols.objAdd(TString::strEmpty());
    colCols.objAdd(m_strDataTitle);
    m_pwndList->SetColumns(colCols, 1);

    // Set our own user provided caption
    strWndText(m_strTitle);

    // Load up the available values, store the original indices in each row
    tCIDLib::TCard4 c4Count = m_pcolAvail->c4ElemCount();
    if (c4Count)
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            colCols[1] = m_pcolAvail->objAt(c4Index).strKey();
            const tCIDLib::TCard4 c4At = m_pwndList->c4AddItem(colCols, c4Index);
        }
    }

    //
    //  If any incoming selected items, check them, or remove them if they are not in
    //  the available values list.
    //
    c4Count = m_pcolSelected->c4ElemCount();
    tCIDLib::TCard4 c4Index = 0;
    while (c4Index < c4Count)
    {
        const tCIDLib::TCard4 c4At = m_pwndList->c4FindByText
        (
            m_pcolSelected->objAt(c4Index).strKey()
        );

        if (c4At == kCIDLib::c4MaxCard)
        {
            m_pcolSelected->RemoveAt(c4Index);
            c4Count--;
        }
         else
        {
            m_pwndList->SetCheckAt(c4At, kCIDLib::True);
            c4Index++;
        }
    }


    // Set our title text
    strWndText(m_strTitle);

    // Register our handlers
    m_pwndCancel->pnothRegisterHandler(this, &TCIDKVPMListSelDlg::eClickHandler);
    m_pwndList->pnothRegisterHandler(this, &TCIDKVPMListSelDlg::eListHandler);
    m_pwndSelect->pnothRegisterHandler(this, &TCIDKVPMListSelDlg::eClickHandler);

    return bRet;
}


// ---------------------------------------------------------------------------
//  TCIDKVPMListSelDlg: Private, non-virutal methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses
TCIDKVPMListSelDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ListSel_Cancel)
        EndDlg(kCIDWUtils::ridDlg_MListSel_Cancel);
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_ListSel_Select)
        EndDlg(kCIDWUtils::ridDlg_MListSel_Select);
    return tCIDCtrls::EEvResponses::Handled;
}


tCIDCtrls::EEvResponses TCIDKVPMListSelDlg::eListHandler(TListChangeInfo& wnotEvent)
{
    // They toggled one of the values
    if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::Invoked)
    {
        // See if this one is currently enabled or disabled, and get the opposite
        tCIDLib::TBoolean bNewState = !m_pwndList->bIsCheckedAt(wnotEvent.c4Index());

        // Get the original list index and a ref to that data
        const tCIDLib::TCard4 c4OrgInd = m_pwndList->c4IndexToId(wnotEvent.c4Index());
        const TKeyValuePair& kvalSel = m_pcolAvail->objAt(c4OrgInd);

        // Find it the selected list as well
        tCIDLib::TCard4 c4Count = m_pcolSelected->c4ElemCount();
        tCIDLib::TCard4 c4FoundAt = 0;
        while (c4FoundAt < c4Count)
        {
            if (m_pcolSelected->objAt(c4FoundAt).strKey() == kvalSel.strKey())
                break;
            c4FoundAt++;
        }

        if (bNewState)
        {
            //
            //  If we don't already have one in the selected list with this name,
            //  then add it.
            //
            if (c4FoundAt == c4Count)
            {
                m_pcolSelected->objAdd(kvalSel);
            }
        }
         else
        {
            // While there are any in the selected list with this name, remove them
            while (c4FoundAt < c4Count)
            {
                if (m_pcolSelected->objAt(c4FoundAt).strKey() == kvalSel.strKey())
                {
                    m_pcolSelected->RemoveAt(c4FoundAt);
                    c4Count--;
                }
                 else
                {
                    c4FoundAt++;
                }
            }
        }

        // Update the check on this row
        m_pwndList->SetCheckAt(wnotEvent.c4Index(), bNewState);
    }
    return tCIDCtrls::EEvResponses::Handled;
}


