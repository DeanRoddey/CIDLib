//
// FILE NAME: CIDCtrls_MultiColListBox.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/15/2015
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
//  This file implements the multi-column list box class.
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
#include    "CIDCtrls_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TMultiColListBox, TListWnd)


//
//  We have to intercept a few things from the contained listview and forward them to
//  our class. We store a pointer to the list box object in the window data.
//
static tCIDCtrls::TDispatchFunc  pfnOrgLVProc;
tCIDCtrls::TMsgResult CALLBACK
mresLVDispatch( const   tCIDCtrls::TWndHandle   hwndThis
                , const tCIDLib::TUInt          uMsg
                , const tCIDCtrls::TWParam      wParam
                , const tCIDCtrls::TLParam      lParam)
{
    // Get the window pointer out. If
    TMultiColListBox* pwndTar = (TMultiColListBox*)::GetWindowLongPtr(hwndThis, GWL_USERDATA);
    if (pwndTar)
    {
        tCIDCtrls::TMsgResult mresRet = 0;
        if (pwndTar->bHandleLVMsg(uMsg, wParam, lParam, mresRet))
            return mresRet;
    }

    // Pass it on to the underlying window
    return ::CallWindowProc(pfnOrgLVProc, hwndThis, uMsg, wParam, lParam);
}



// ---------------------------------------------------------------------------
//  CLASS: MMCLBIPEIntf
// PREFIX: ipei
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  MMCLBIPEIntf: Constructors and Destructor
// ---------------------------------------------------------------------------
MMCLBIPEIntf::MMCLBIPEIntf()
{
}

MMCLBIPEIntf::~MMCLBIPEIntf()
{
}



// ---------------------------------------------------------------------------
//  CLASS: TMCListOps
// PREFIX: mclops
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMCListOps: Constructors and Destructors
// ---------------------------------------------------------------------------
TMCListOps::~TMCListOps()
{
}


// ---------------------------------------------------------------------------
//  TMCListOps: Hidden constructors
// ---------------------------------------------------------------------------
TMCListOps::TMCListOps()
{
}



// ---------------------------------------------------------------------------
//  CLASS: TMultiColListBox
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TMultiColListBox::TItemData: Constructors and Destructor
// ---------------------------------------------------------------------------
TMultiColListBox::TItemData::
TItemData(  const   tCIDLib::TStrCollect&   colVals
            , const tCIDLib::TCard4         c4RowId) :

    m_bFlag(kCIDLib::False)
    , m_c4RowId(c4RowId)
    , m_c4UserData(0)
    , m_c8UserData(0)
    , m_colCols(colVals.c4ElemCount())
    , m_pstrUserData(nullptr)
{
    TColCursor<TString>& cursVals = *colVals.pcursNew();
    TJanitor<TColCursor<TString>> janCurs(&cursVals);
    for (; cursVals; ++cursVals)
        m_colCols.objAdd(*cursVals);
}

TMultiColListBox::TItemData::TItemData(const tCIDLib::TCard4 c4Cols) :

    m_bFlag(kCIDLib::False)
    , m_c4RowId(0)
    , m_c4UserData(0)
    , m_c8UserData(0)
    , m_colCols(c4Cols)
    , m_pstrUserData(nullptr)
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Cols; c4Index++)
        m_colCols.objAdd(TString::strEmpty());
}

TMultiColListBox::TItemData::TItemData(const TItemData& idSrc) :

    m_bFlag(idSrc.m_bFlag)
    , m_c4RowId(idSrc.m_c4RowId)
    , m_c4UserData(idSrc.m_c4UserData)
    , m_c8UserData(idSrc.m_c8UserData)
    , m_colCols(idSrc.m_colCols)
    , m_pstrUserData(nullptr)
{
    if (idSrc.m_pstrUserData && !m_pstrUserData)
        m_pstrUserData = new TString(*idSrc.m_pstrUserData);
    else if (!idSrc.m_pstrUserData && m_pstrUserData)
        m_pstrUserData->Clear();
    else if (idSrc.m_pstrUserData && m_pstrUserData)
        *m_pstrUserData = *idSrc.m_pstrUserData;
}

TMultiColListBox::TItemData::~TItemData()
{
    delete m_pstrUserData;
}


// ---------------------------------------------------------------------------
// TMultiColListBox::TItemData: Public operators
// ---------------------------------------------------------------------------
TMultiColListBox::TItemData&
TMultiColListBox::TItemData::operator=(const TMultiColListBox::TItemData& idSrc)
{
    if (this != &idSrc)
    {
        m_bFlag      = idSrc.m_bFlag;
        m_c4RowId    = idSrc.m_c4RowId;
        m_c4UserData = idSrc.m_c4UserData;
        m_c8UserData = idSrc.m_c8UserData;
        m_colCols    = idSrc.m_colCols;

        if (idSrc.m_pstrUserData && !m_pstrUserData)
            m_pstrUserData = new TString(*idSrc.m_pstrUserData);
        else if (!idSrc.m_pstrUserData && m_pstrUserData)
            m_pstrUserData->Clear();
        else if (idSrc.m_pstrUserData && m_pstrUserData)
            *m_pstrUserData = *idSrc.m_pstrUserData;
    }
    return *this;
}


// ---------------------------------------------------------------------------
// TMultiColListBox: Constructors and Destructor
// ---------------------------------------------------------------------------
TMultiColListBox::TMultiColListBox() :

    TListWnd(L"MCListBoxNotifyId")
    , m_bSortAsc(kCIDLib::True)
    , m_c4SearchCol(0)
    , m_c4SortCol(0)
    , m_eCustOpts(tCIDCtrls::EMCLBCustOpts::None)
    , m_eMCLBStyles(tCIDCtrls::EMCLBStyles::None)
    , m_hwndListView(kCIDCtrls::hwndInvalid)
    , m_pipeiAuto(nullptr)
{
}

TMultiColListBox::~TMultiColListBox()
{
}



// ---------------------------------------------------------------------------
// TMultiColListBox: Public, inherited methods
// ---------------------------------------------------------------------------

// If not single select, then we are multi
tCIDLib::TBoolean TMultiColListBox::bIsMultiSel() const
{
    return (GetWindowLong(m_hwndListView, GWL_STYLE) & LVS_SINGLESEL) != LVS_SINGLESEL;
}


// We just call c4FindByText and call SelectByIndex if we find a match
tCIDLib::TBoolean
TMultiColListBox::bSelectByText(const   TString&            strToSel
                                ,       tCIDLib::TCard4&    c4FoundAt
                                , const tCIDLib::TBoolean   bForceEvent)
{
    c4FoundAt = c4FindByText(strToSel);
    if (c4FoundAt == kCIDLib::c4MaxCard)
        return kCIDLib::False;

    SelectByIndex(c4FoundAt, bForceEvent);
    return kCIDLib::True;
}


//
//  For a single selection, returns the currently selected item, or max card if
//  none. For multi-select, returns the item with the focus rectangle, or max
//  card if none.
//
tCIDLib::TCard4 TMultiColListBox::c4CurItem() const
{
    tCIDCtrls::TMsgResult mresIndex = ::SendMessage
    (
        m_hwndListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED
    );
    if (mresIndex < 0)
        return kCIDLib::c4MaxCard;

    return mresIndex;
}


//
//  We just search the data for a full match, on whichever column is set on us as
//  the search column. If we are sorted and the search and sort columns are the
//  same, we can optimize and do a binary search.
//
tCIDLib::TCard4 TMultiColListBox::c4FindByText(const TString& strToFind) const
{
    tCIDLib::TCard4 c4Ret = kCIDLib::c4MaxCard;
    if (tCIDLib::bAllBitsOn(m_eMCLBStyles, tCIDCtrls::EMCLBStyles::Sorted)
    &&  (m_c4SearchCol == m_c4SortCol))
    {
        tCIDLib::TCard4 c4At;
        const TItemData* pidFind = m_colList.pobjKeyedBinarySearch
        (
            strToFind
            , [this](const TString& strKey, const TItemData& idComp)
              {
                const tCIDLib::TCard4 c4ColInd = this->m_c4SearchCol;
                return this->eCompColVals(strKey, idComp.m_colCols[c4ColInd], c4ColInd);
              }
            , c4At
        );

        if (pidFind)
            c4Ret = c4At;
    }
     else
    {
        // Just do a brute force linear search
        c4Ret = m_colList.tFind
        (
            [this, &strToFind](const TItemData& idCur)
            {
                return idCur.m_colCols[m_c4SearchCol].bCompareI(strToFind);
            }
        );
    }
    return c4Ret;
}


// Get the id associated with the list box item at the given index
tCIDLib::TCard4 TMultiColListBox::c4IndexToId(const tCIDLib::TCard4 c4Index) const
{
    CheckRowIndex(c4Index, CID_LINE);
    return m_colList[c4Index].m_c4RowId;
}


// Return the number of items in the list
tCIDLib::TCard4 TMultiColListBox::c4ItemCount() const
{
    return m_colList.c4ElemCount();
}


//
//  Find the first item whose first search column text starts with the passed text.
//
tCIDLib::TCard4 TMultiColListBox::c4StartsWith(const TString& strToFind) const
{
    return m_colList.tFind
    (
        [this, &strToFind](const TItemData& idCur)
        {
            return idCur.m_colCols[m_c4SearchCol].bStartsWithI(strToFind);
        }
    );
}


// Initialize a MC list box from a dialog item description-
tCIDLib::TVoid
TMultiColListBox::InitFromDesc( const   TWindow&                wndParent
                                , const TDlgItem&               dlgiSrc
                                , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EMCLBStyles eMCLBStyles = tCIDCtrls::EMCLBStyles::FullRowSel;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisChild
    );
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None;
    tCIDLib::TStrList colColTitles;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_AutoLast))
        eMCLBStyles |= tCIDCtrls::EMCLBStyles::AutoLastCol;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Group))
        eStyles |= tCIDCtrls::EWndStyles::Group;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_MultiVal))
        eMCLBStyles |= tCIDCtrls::EMCLBStyles::MultiSel;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_NoTitles))
        eMCLBStyles |= tCIDCtrls::EMCLBStyles::NoTitles;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Sort))
        eMCLBStyles |= tCIDCtrls::EMCLBStyles::Sorted;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_ColSort))
        eMCLBStyles |= tCIDCtrls::EMCLBStyles::ColSort;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Grid))
        eMCLBStyles |= tCIDCtrls::EMCLBStyles::GridLines;

    if (eTheme == tCIDCtrls::EWndThemes::DialogBox)
    {
        // Set a sunken border and ctrl fill background unless told not to
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
        {
            eExStyles |= tCIDCtrls::EExWndStyles::SunkenEdge;
            SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::DlgCtrlFill), kCIDLib::True);
        }
    }
     else if (eTheme == tCIDCtrls::EWndThemes::MainWnd)
    {
        // Set a regular border unless told not to
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
            eStyles |= tCIDCtrls::EWndStyles::Border;
    }

    // We don't have any columns yet, so just pass empty ones
    CreateMCLB
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , colColTitles
        , 0
        , eStyles
        , eMCLBStyles
        , eExStyles
    );
}


tCIDLib::TVoid TMultiColListBox::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);

    colToFill.objAdd(kCIDCtrls::strHint_AutoLast);
    colToFill.objAdd(kCIDCtrls::strHint_ColSort);
    colToFill.objAdd(kCIDCtrls::strHint_Grid);
    colToFill.objAdd(kCIDCtrls::strHint_Group);
    colToFill.objAdd(kCIDCtrls::strHint_MultiVal);
    colToFill.objAdd(kCIDCtrls::strHint_NoBorder);
    colToFill.objAdd(kCIDCtrls::strHint_NoTitles);
    colToFill.objAdd(kCIDCtrls::strHint_Sort);
}


//
//  Remove all items from the list. This will cause a clear notification to be sent.
//
tCIDLib::TVoid TMultiColListBox::RemoveAll(const tCIDLib::TBoolean bNoEvent)
{
    // If already empty, do nothing
    if (m_colList.bIsEmpty())
        return;

    m_colList.RemoveAll();
    if (!::SendMessage(m_hwndListView, LVM_DELETEALLITEMS, 0, 0))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_RemoveAll
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(widThis())
        );
    }

    if (!bNoEvent)
    {
        TWindow* pwndParent = pwndFindParent();
        if (pwndParent)
            SendListEvent(tCIDCtrls::EListEvents::Cleared, 0, 0, 0, kCIDLib::True);
    }
}


//
//  Remove the item at the indicated index.
//
tCIDLib::TVoid
TMultiColListBox::RemoveAt( const   tCIDLib::TCard4     c4Index
                            , const tCIDLib::TBoolean   bSelectAnother)
{
    CheckRowIndex(c4Index, CID_LINE);

    //
    //  Send the message first so that the handler doesn't get an invalid index if we
    //  are removing the last guy in the list. This is synchronous so we'll come back
    //  after it's been processed and we can then remove it.
    //
    if (!::SendMessage(m_hwndListView, LVM_DELETEITEM, c4Index, 0))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_RemoveItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4Index)
            , TCardinal(widThis())
        );
    }
    m_colList.RemoveAt(c4Index);

    //
    //  If we emptied it, send a notification, since we won't get any notification to
    //  drive it. If nto, and they asked us to, get another item selected.
    //
    if (m_colList.bIsEmpty())
    {
        SendListEvent(tCIDCtrls::EListEvents::Cleared, 0, 0);
    }
     else
    {
        if (bSelectAnother && !bIsMultiSel())
        {
            //
            //  If we have enough, just select the one that fell down into this one's
            //  place. Else move back.
            //
            if (c4Index < m_colList.c4ElemCount())
                SelectByIndex(c4Index, kCIDLib::True);
            else
                SelectByIndex(c4Index - 1, kCIDLib::True);
        }
    }
}


//
//  Select an item in the list by index. For listviews, which is what we are
//  implemented in terms of, we have to set the item's state. Ownerdata type lists
//  handle this much functionality, so we don't have to do anything with our own
//  data.
//
tCIDLib::TVoid
TMultiColListBox::SelectByIndex(const   tCIDLib::TCard4     c4ToSel
                                , const tCIDLib::TBoolean   bForceEvent
                                , const tCIDLib::TBoolean   bThrowIfNot)
{
    CheckRowIndex(c4ToSel, CID_LINE);

    // Get the current selection
    const tCIDLib::TCard4 c4CurInd = c4CurItem();

    // If not the same, do the selection
    if (c4CurInd != c4ToSel)
    {
        LVITEM Item = {0};
        Item.mask = LVIF_STATE;
        Item.state = LVIS_FOCUSED | LVIS_SELECTED;
        Item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;

        const tCIDCtrls::TMsgResult mresSel = ::SendMessage
        (
            m_hwndListView, LVM_SETITEMSTATE, c4ToSel, tCIDCtrls::TLParam(&Item)
        );

        if (!mresSel)
        {
            if (bThrowIfNot)
            {
                facCIDCtrls().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCtrlsErrs::errcList_BadIndex
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Index
                    , TCardinal(c4ToSel)
                    , TCardinal(widThis())
                    , TCardinal(c4ItemCount())
                );
            }
            return;
        }
    }

    //
    //  Make sure it's visible if single select, even if no change in selection
    if (!bIsMultiSel())
        ListView_EnsureVisible(m_hwndListView, c4ToSel, FALSE);

    // If they asked us to force an event, then do so
    if (bForceEvent)
        SendListEvent(tCIDCtrls::EListEvents::SelChanged, c4ToSel, c4IndexToId(c4ToSel));
}


//
//  We have to override this and pass it on to our nested list window. NOTE WE DO NOT
//  call our parent since we don't want it set on us.
//
tCIDLib::TVoid
TMultiColListBox::SetWndStyle(  const   tCIDCtrls::EWndStyles   eToSet
                                , const tCIDLib::TBoolean       bSetIt)
{
    tCIDLib::TCard4 c4Cur = ::GetWindowLong(m_hwndListView, GWL_STYLE);

    if (bSetIt)
        c4Cur |= tCIDLib::TCard4(eToSet);
    else
        c4Cur &= ~tCIDLib::TCard4(eToSet);

    ::SetWindowLong(m_hwndListView, GWL_STYLE, c4Cur);
}


// ---------------------------------------------------------------------------
// TMultiColListBox: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Auto-size a column by index. They indicate the type of auto-size to use, either
//  driven by header width or by content width. If the 'auto last' style is set, then
//  it should already automatically be set using the header type any time anything
//  changes that would affect it. If you set it otherwise, it'll change back the
//  next time it's automatically updated.
//
tCIDLib::TVoid
TMultiColListBox::AutoSizeCol(  const   tCIDLib::TCard4     c4Index
                                , const tCIDLib::TBoolean   bUseHeader)
{
    ::SendMessage
    (
        m_hwndListView
        , LVM_SETCOLUMNWIDTH
        , c4Index
        , bUseHeader ? LVSCW_AUTOSIZE_USEHEADER : LVSCW_AUTOSIZE
    );

    // If this wasn't the last column, and we are auto-last, then update the last column
    if ((c4Index + 1 != c4ColCount())
    &&  tCIDLib::bAllBitsOn(m_eMCLBStyles, tCIDCtrls::EMCLBStyles::AutoLastCol))
    {
        UpdateAutoLast();
    }
}


// Returns the per-row flag for the indicated row index
tCIDLib::TBoolean TMultiColListBox:: bFlagAt(const tCIDLib::TCard4 c4Index) const
{
    CheckRowIndex(c4Index, CID_LINE);
    return m_colList[c4Index].m_bFlag;
}


//
//  Returns true if the indicated item is marked. This is really only useful for
//  multi-select lists, but if it's single, this effectively indicates if the
//  indicated index is the selected item.
//
tCIDLib::TBoolean TMultiColListBox::bIsMarked(const tCIDLib::TCard4 c4Index) const
{
    CheckRowIndex(c4Index, CID_LINE);
    return (::SendMessage(m_hwndListView, LVM_GETITEMSTATE, c4Index, LVIS_SELECTED) != 0);
}


//
//  Move the current selection up or down, if we can. We return whether we did or not.
//  They can indicate whether the selection should move along with it, or stay where it
//  is.
//
tCIDLib::TBoolean TMultiColListBox::bMoveDn(const tCIDLib::TBoolean bFollowSel)
{
    // Get the selection index. If none, we can't do anything
    tCIDLib::TCard4 c4CurIndex = c4CurItem();
    if (c4CurIndex == kCIDLib::c4MaxCard)
        return kCIDLib::False;

    // Get the count, if we are at the end, we can't do anything
    const tCIDLib::TCard4 c4Count = c4ItemCount();
    if (c4CurIndex + 1 >= c4Count)
        return kCIDLib::False;

    // Looks like we can do it, so swap it with the next item
    SwapRows(c4CurIndex, c4CurIndex + 1, bFollowSel);
    return kCIDLib::True;
}

tCIDLib::TBoolean TMultiColListBox::bMoveUp(const tCIDLib::TBoolean bFollowSel)
{
    //
    //  Get the selection index. If none or at zero, we can't do anything. This effectively
    //  also proves we have more than one item, so we don't have to check the count.
    //
    tCIDLib::TCard4 c4CurIndex = c4CurItem();
    if (!c4CurIndex || (c4CurIndex == kCIDLib::c4MaxCard))
        return kCIDLib::False;

    // Looks like we can do it, so swap it with the previous item
    SwapRows(c4CurIndex, c4CurIndex - 1, bFollowSel);
    return kCIDLib::True;
}


//
//  This method adds a new item to the list.
//
//  The list must have the correct number of values for the number of colums this list
//  has, and they must be in the correct order.
//
//  If the list was empty, and this is a single select list, then we will select the 0th
//  item unless bNoSelect is set. Sometimes they don't want to do an initial selection
//  for bootstrapping reasons.
//
//  We return the index it went in at, in case they don't indicate a specific index (they
//  pass maxcard) or we are sorted (in which the incoming index is meaningless.)
//
//  In this case, we do send a housekeeping message to the list, so that it knows a new
//  item was inserted and can update accordingly, but it doesn't contain any actual data
//  since we maintain the data ourself.
//
tCIDLib::TCard4
TMultiColListBox::c4AddItem(const   tCIDLib::TStrCollect&   colColVals
                            , const tCIDLib::TCard4         c4Id
                            , const tCIDLib::TCard4         c4InsertAt
                            , const tCIDLib::TBoolean       bSelect)
{
    // Have to have the right count for our columns
    const tCIDLib::TCard4 c4Count = m_colColTitles.c4ElemCount();
    if (colColVals.c4ElemCount() != c4Count)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_WrongColCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4Count)
            , TCardinal(colColVals.c4ElemCount())
            , TCardinal(widThis())
        );
    }

    // Loop through the values and load them up
    LVITEM lviCur = {0};
    lviCur.mask = LVIF_TEXT;

    //
    //  Insert it into our list, deciding where it will go. If sorting, then we
    //  figure it out by finding the right spot. Else, if max card insert point
    //  then add it at the end, else add it where they asked.
    //
    TItemData idNew(colColVals, c4Id);

    if (tCIDLib::bAllBitsOn(m_eMCLBStyles, tCIDCtrls::EMCLBStyles::Sorted))
    {
        //
        //  Find the insert point. The current contents of the sort column should
        //  be sorted already, so we can do a binary search. Even if it doesn't find
        //  a match, it'll return where we should insert.
        //
        tCIDLib::TCard4 c4At = 0;
        if (!m_colList.bIsEmpty())
        {
            m_colList.pobjBinarySearch
            (
                idNew
                , [this](const TItemData& id1, const TItemData& id2)
                  {
                    const tCIDLib::TCard4 c4ColInd = this->m_c4SortCol;
                    return this->eCompColVals
                    (
                        id1.m_colCols[c4ColInd], id2.m_colCols[c4ColInd], c4ColInd
                    );
                  }
                , c4At
            );
        }
        lviCur.iItem = c4At;
        m_colList.InsertAt(idNew, lviCur.iItem);
    }
     else if (c4InsertAt == kCIDLib::c4MaxCard)
    {
        // Just insert at the last slot
        lviCur.iItem = m_colList.c4ElemCount();
        m_colList.objAdd(idNew);
    }
     else
    {
        lviCur.iItem = c4InsertAt;
        m_colList.InsertAt(idNew, c4InsertAt);
    }

    //
    //  And now set let's add it. We are owner data so there's no need to actually
    //  add the sub-items or the text for any of them. This call just tells the
    //  control that we added an item and to update as required.
    //
    lviCur.iSubItem = 0;
    tCIDCtrls::TMsgResult mresIndex = ::SendMessage
    (
        m_hwndListView, LVM_INSERTITEM, 0, tCIDCtrls::TLParam(&lviCur)
    );

    if (mresIndex < 0)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_AddItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(widThis())
        );
    }

    //
    //  If they asked us to select it, then do so.
    //
    if (bSelect)
        SelectByIndex(mresIndex);

    // Just in case this causes a scroll bar to change the available area
    UpdateAutoLast();

    return tCIDLib::TCard4(mresIndex);
}


//
//  A convenience to add an item and set both the row id and the string user data at the
//  same time. We just call the other version then set the user data on it.
//
tCIDLib::TCard4
TMultiColListBox::c4AddItem(const   tCIDLib::TStrCollect&   colColVals
                            , const tCIDLib::TCard4         c4Id
                            , const TString&                strUserData
                            , const tCIDLib::TCard4         c4InsertAt
                            , const tCIDLib::TBoolean       bNoSelect)
{
    tCIDLib::TCard4 c4Ret = c4AddItem(colColVals, c4Id, c4InsertAt, bNoSelect);
    SetUserDataAt(c4Ret, strUserData);
    return c4Ret;
}


// Get the number of colums currently set on this list
tCIDLib::TCard4 TMultiColListBox::c4ColCount() const
{
    return m_colColTitles.c4ElemCount();
}


//
//  Using the headers we figure out the width of the columns area.
//
tCIDLib::TCard4 TMultiColListBox::c4ColsWidth() const
{
    if (m_hwndListView == kCIDCtrls::hwndInvalid)
        return 0;

    tCIDCtrls::TMsgResult mresRes = ::SendMessage(m_hwndListView, LVM_GETHEADER, 0, 0);
    if (!mresRes)
        return 0;

    // Get the count of header items
    tCIDCtrls::TWndHandle hwndHdr = reinterpret_cast<tCIDCtrls::TWndHandle>(mresRes);
    mresRes = ::SendMessage(hwndHdr, HDM_GETITEMCOUNT, 0, 0);

    // If none or error, return zero now
    if (mresRes < 1)
        return 0;

    // Get the rectangle of the last column header
    RECT rectLast;
    if (!::SendMessage(hwndHdr, HDM_GETITEMRECT, mresRes - 1, tCIDCtrls::TLParam(&rectLast)))
        return 0;

    // Return the point after the right side
    return tCIDLib::TCard4(rectLast.right + 1);
}


//
//  Returns the index of an item by its user data. The index of course is in terms of the
//  current sort order.
//
tCIDLib::TCard4
TMultiColListBox::c4FindByUserData( const   tCIDLib::TCard4     c4ToFind
                                    , const tCIDLib::TCard4     c4StartInd) const
{
    return m_colList.tFind
    (
        [c4ToFind](const TItemData& idCur){ return idCur.m_c4UserData == c4ToFind; }
        , kCIDLib::c4MaxCard
        , c4StartInd
    );
}

tCIDLib::TCard4
TMultiColListBox::c4FindByUserData( const   tCIDLib::TCard8     c8ToFind
                                    , const tCIDLib::TCard4     c4StartInd) const
{
    return m_colList.tFind
    (
        [c8ToFind](const TItemData& idCur){ return idCur.m_c8UserData == c8ToFind; }
        , kCIDLib::c4MaxCard
        , c4StartInd
    );
}


tCIDLib::TCard4
TMultiColListBox::c4FindByUserData( const   TString&            strToFind
                                    , const tCIDLib::TCard4     c4StartInd
                                    , const tCIDLib::TBoolean   bCase) const
{
    return m_colList.tFind
    (
        [&strToFind, bCase](const TItemData& idCur)
        {
            if (!idCur.m_pstrUserData)
                return kCIDLib::False;
            if (bCase)
                idCur.m_pstrUserData->bCompare(strToFind);
            return idCur.m_pstrUserData->bCompareI(strToFind);
        }
        , kCIDLib::c4MaxCard
        , c4StartInd
    );
}


//
//  Find a value in a given column using one of the standard matching schemes. For now we just
//  do a brute force search because, even if we are sorted and it's the sort column, we don't
//  currently have a way to search a sub-set of our collection, which is necessary here since
//  they can provide a starting index, in order to be able to move forward to subsequent
//  matches.
//
tCIDLib::TCard4
TMultiColListBox::c4FindText(const  TString&                strFind
                            , const tCIDLib::TCard4         c4ColIndex
                            , const tCIDLib::TCard4         c4StartInd
                            , const tCIDLib::EMatchTypes    eType
                            , const tCIDLib::TBoolean       bCaseSensitive) const
{
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = c4StartInd; c4Index < c4Count; c4Index++)
    {
        const TString& strCurVal = m_colList[c4Index].m_colCols[c4ColIndex];

        tCIDLib::TCard4 c4Pos = 0;
        switch(eType)
        {
            case tCIDLib::EMatchTypes::Contains :
                if (strCurVal.bFirstOccurrence(strFind, c4Pos, kCIDLib::False, bCaseSensitive))
                    return c4Index;
                break;

            case tCIDLib::EMatchTypes::Equals :
                if (bCaseSensitive)
                {
                    if (strFind == strCurVal)
                        return c4Index;
                }
                 else
                {
                    if (strFind.bCompareI(strCurVal))
                        return c4Index;
                }
                break;

            case tCIDLib::EMatchTypes::StartsWith :
                if (bCaseSensitive)
                {
                    if (strCurVal.bStartsWith(strFind))
                        return c4Index;
                }
                 else
                {
                    if (strCurVal.bStartsWithI(strFind))
                        return c4Index;
                }
                break;

            default :
                CIDAssert2(L"Unknown MCLB text find match type");
                break;
        };
    }

    return kCIDLib::c4MaxCard;
}



//
//  See if there is an item that maps to the indicated id. If not, we either return
//  max card or throw. If the parent is handling the data, then this shouldn't get
//  called, since he has that information himself.
//
tCIDLib::TCard4
TMultiColListBox::c4IdToIndex(  const   tCIDLib::TCard4     c4Id
                                , const tCIDLib::TBoolean   bThrowIfNot) const
{
    tCIDLib::TCard4 c4Ret = kCIDLib::c4MaxCard;

    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colList[c4Index].m_c4RowId == c4Id)
        {
            c4Ret = c4Index;
            break;
        }
    }

    if (c4Ret == kCIDLib::c4MaxCard)
    {
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcList_NoIdMapping
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(c4Id)
                , TCardinal(widThis())
            );
        }
    }
    return c4Ret;
}


// Get or set the search column
tCIDLib::TCard4 TMultiColListBox::c4SearchCol() const
{
    return m_c4SearchCol;
}

tCIDLib::TCard4 TMultiColListBox::c4SearchCol(const tCIDLib::TCard4 c4ToSet)
{
    m_c4SearchCol = c4ToSet;
    return m_c4SearchCol;
}



//
//  Select an item by it's unique id. We map the id to an index. If it's found,
//  then we jus call the standard by index selection method.
//
tCIDLib::TCard4
TMultiColListBox::c4SelectById( const   tCIDLib::TCard4     c4ToSel
                                , const tCIDLib::TBoolean   bThrowIfNot
                                , const tCIDLib::TBoolean   bForceEvent)
{
    tCIDLib::TCard4 c4Index = c4IdToIndex(c4ToSel, bThrowIfNot);

    //
    //  If not found, and they don't want to throw, then just return. Else just
    //  let it go to the index selection method, which will throw for us.
    //
    if ((c4Index == kCIDLib::c4MaxCard) && !bThrowIfNot)
        return c4Index;

    SelectByIndex(c4Index, bForceEvent);
    return c4Index;
}


tCIDLib::TCard4 TMultiColListBox::c4SortCol() const
{
    // If not sorted, this is a meaningless call
    if (!tCIDLib::bAllBitsOn(m_eMCLBStyles, tCIDCtrls::EMCLBStyles::Sorted))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_NotSorted
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }
    return m_c4SortCol;
}


//
//  We just search our list for the indicated text. If not, we say we can't find it.
//  In this case, they tell us a specific column to search, so we don't use the
//  currently set search column.
//
tCIDLib::TCard4
TMultiColListBox::c4TextToIndex(const   TString&        strToFind
                                , const tCIDLib::TCard4 c4ColIndex) const
{

    return m_colList.tFind
    (
        [&strToFind, c4ColIndex](const TItemData& idCur)
        {
            return idCur.m_colCols[c4ColIndex].bCompareI(strToFind);
        }
    );
}


// This is something we have to get from the list window
tCIDLib::TCard4 TMultiColListBox::c4TopIndex() const
{
    if (!c4ItemCount())
        return kCIDLib::c4MaxCard;

    return ::SendMessage(m_hwndListView, LVM_GETTOPINDEX, 0, 0);
}


// Return the Card4/Card8 per-row user data for the indicated row
tCIDLib::TCard4
TMultiColListBox::c4UserDataAt(const tCIDLib::TCard4 c4Index) const
{
    CheckRowIndex(c4Index, CID_LINE);
    return m_colList[c4Index].m_c4UserData;
}

tCIDLib::TCard8
TMultiColListBox::c8UserDataAt(const tCIDLib::TCard4 c4Index) const
{
    CheckRowIndex(c4Index, CID_LINE);
    return m_colList[c4Index].m_c8UserData;
}


//
//  For single selection, it finds the current selection, if any, and deselects it.
//  We'll send a selection changed event with an index of max card to let any
//  listeners know that this has happened.
//
tCIDLib::TVoid TMultiColListBox::ClearSelection()
{
    const tCIDLib::TCard4 c4TarIndex = c4CurItem();
    if (c4TarIndex != kCIDLib::c4MaxCard)
    {
        LVITEM Item = {0};
        Item.mask = LVIF_STATE;
        Item.state = 0;
        Item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;

        ::SendMessage(m_hwndListView, LVM_SETITEMSTATE, c4TarIndex, tCIDCtrls::TLParam(&Item));
        SendListEvent
        (
            tCIDCtrls::EListEvents::SelChanged, kCIDLib::c4MaxCard, kCIDLib::c4MaxCard
        );
    }
}


//
//  Set up our styles and call our parent to create us.
//
tCIDLib::TVoid
TMultiColListBox::CreateMCLB(const  TWindow&                wndParent
                            , const tCIDCtrls::TWndId       widThis
                            , const TArea&                  areaInit
                            , const tCIDLib::TStrCollect&   colColTitles
                            , const tCIDLib::TCard4         c4SearchCol
                            , const tCIDCtrls::EWndStyles   eStyles
                            , const tCIDCtrls::EMCLBStyles  eMCLBStyles
                            , const tCIDCtrls::EExWndStyles eExStyles)
{
    //
    //  Store the column titles away and the search colon, if they set any
    //  columns at this point.
    //
    if (colColTitles.c4ElemCount() && (c4SearchCol >= colColTitles.c4ElemCount()))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_BadColIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4SearchCol)
            , TCardinal(widThis)
        );
    }
    m_c4SearchCol = c4SearchCol;
    TColCursor<TString>& cursCols = *colColTitles.pcursNew();
    TJanitor<TColCursor<TString>> janCurs(&cursCols);
    for (; cursCols; ++cursCols)
        m_colColTitles.objAdd(*cursCols);

    // Store our class specific styles
    m_eOrgStyles = eStyles;
    m_eMCLBStyles = eMCLBStyles;

    //
    //  Create us. Make sure we don't have the tab stop or group styles. We'll apply
    //  those to the nested control and set the control parent on us.
    //
    CreateWnd
    (
        wndParent.hwndThis()
        , kCIDCtrls::pszCustClass
        , kCIDLib::pszEmptyZStr
        , areaInit
        , tCIDCtrls::EWndStyles
          (
            (tCIDLib::TCard4(eStyles) | WS_CLIPCHILDREN) & ~(WS_TABSTOP | WS_GROUP)
          )
        , eExStyles | tCIDCtrls::EExWndStyles(WS_EX_CONTROLPARENT)
        , widThis
    );
}


tCIDCtrls::EMCLBStyles TMultiColListBox::eMCLBStyles() const
{
    return m_eMCLBStyles;
}


//
//  This allows the containing application to force us into in-place edit mode on a
//  cell, so that he can do things like immediately let the user edit the name of
//  something after it's added to the list. We act just like we were invoked via a
//  click.
//
tCIDLib::TVoid
TMultiColListBox::EditCell( const   tCIDLib::TCard4 c4Row
                            , const tCIDLib::TCard4 c4Col)
{
    CIDAssert(m_pipeiAuto != nullptr, L"Can't edit cell if IPE handler isn't set");

    // Make sure this row is visible and then selected and take the focus
    MakeItemVisible(c4Row, kCIDLib::True);

    // Select the item and give the focus to the list
    SelectByIndex(c4Row);
    TakeFocus();

    // And invoke the Auto-IPE
    AutoIPE(c4Row, c4Col);
}


//
//  In our case, each string is a quoted comma list of values, one per column. All
//  column values must be provided, pass an empty string at least. This version doesn't
//  provide any mapping between list items and objects. It's for when you just want
//  to load up text.
//
//  If we have sorted style, we'll sort the list after loading.
//
tCIDLib::TVoid
TMultiColListBox::LoadList( const   tCIDLib::TStrCollect&   colLoadWith
                            , const tCIDLib::TBoolean       bAppend)
{
    // Remove all existing items first if not appending
    if (!bAppend)
        RemoveAll();

    // Remember the original count
    const tCIDLib::TCard4 c4OrgCount = c4ItemCount();

    // And now loop through the items and load them
    tCIDLib::TCard4     c4ErrInd;
    tCIDLib::TStrList   colVals;
    TItemData           idNew(m_colColTitles.c4ElemCount());

    TColCursor<TString>* pcursLoad = colLoadWith.pcursNew();
    TJanitor<TColCursor<TString>> janCursor(pcursLoad);
    for (; pcursLoad->bIsValid(); pcursLoad->bNext())
    {
        // Ask the tokenizer to parse it for us as a quoted comma list
        if (!TStringTokenizer::bParseQuotedCommaList(pcursLoad->objRCur(), idNew.m_colCols, c4ErrInd))
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcList_BadColVals
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(widThis())
            );
        }

        // Add it to our list
        m_colList.objAdd(idNew);
    }

    if (tCIDLib::bAllBitsOn(m_eMCLBStyles, tCIDCtrls::EMCLBStyles::Sorted))
        SortList();

    //
    //  Set up the flags. If we flushed any previous content, we want to let it
    //  scroll appropriately. If we appended, then we let it stay where it is.
    //
    tCIDCtrls::TLParam lFlags = LVSICF_NOINVALIDATEALL;
    if (c4OrgCount)
        lFlags |= LVSICF_NOSCROLL;

    // Set the item count
    ::SendMessage(m_hwndListView, LVM_SETITEMCOUNT, m_colList.c4ElemCount(), lFlags);

    // If single select and we were originally empty, select the first one
    if (!bIsMultiSel() && !c4OrgCount)
        SelectByIndex(0);

    UpdateAutoLast();
}


//
//  Another way to load a list of values and to provide an object mapping for each of
//  them at the same time. Since we are setting the ids, we have to actually add the
//  items to the list as we go.
//
//  If we have sort style, we sort the list after loading it.
//
tCIDLib::TVoid
TMultiColListBox::LoadList( const   tCIDLib::TStrList&      colRows
                            , const tCIDLib::TCardList&     fcolIds
                            , const tCIDLib::TBoolean       bAppend)
{
    const tCIDLib::TCard4 c4Count = colRows.c4ElemCount();
    if (c4Count != fcolIds.c4ElemCount())
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_CountNEqual
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(widThis())
        );
    }

    // Remove all existing items first if not appending
    if (!bAppend)
        RemoveAll();

    const tCIDLib::TCard4   c4OrgCount = m_colList.c4ElemCount();
    tCIDLib::TCard4         c4ErrInd;
    TItemData               idNew(m_colColTitles.c4ElemCount());
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TString& strCurRow = colRows[c4Index];

        // Ask the tokenizer to parse it for us as a quoted comma list
        if (!TStringTokenizer::bParseQuotedCommaList(strCurRow, idNew.m_colCols, c4ErrInd))
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcList_BadColVals
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(widThis())
            );
        }

        // Add it to our list
        idNew.m_c4RowId = fcolIds[c4Index];
        m_colList.objAdd(idNew);
    }

    if (tCIDLib::bAllBitsOn(m_eMCLBStyles, tCIDCtrls::EMCLBStyles::Sorted))
        SortList();

    //
    //  Set up the flags. If we flushed any previous content, we want to let it
    //  scroll appropriately. If we appended, then we let it stay where it is.
    //
    tCIDCtrls::TLParam lFlags = LVSICF_NOINVALIDATEALL;
    if (c4OrgCount)
        lFlags |= LVSICF_NOSCROLL;

    // Set the item count
    ::SendMessage
    (
        m_hwndListView, LVM_SETITEMCOUNT, m_colList.c4ElemCount(), lFlags
    );

    // If single select, select the first one (visible order)
    if (!bIsMultiSel())
        SelectByIndex(0);

    UpdateAutoLast();
}


//
//  This method will use a MC list ops object to load items into the list. We
//  implement our own insertion code so that we don't have to copy the text just
//  to load it. This is the most efficient means to load content.
//
//  If we have sort style, we sort the list after loading it.
//
tCIDLib::TVoid
TMultiColListBox::LoadList( const   TMCListOps&         mclopsSrc
                            , const tCIDLib::TBoolean   bAppend)
{
    // Remove all items first if not appending
    if (!bAppend)
        RemoveAll();

    // Get the count of rows and columns
    tCIDLib::TStrList colVals(m_colColTitles);

    const tCIDLib::TCard4   c4OrgCount = m_colList.c4ElemCount();
    const tCIDLib::TCard4   c4RowCnt = mclopsSrc.c4RowCount();
    const tCIDLib::TCard4   c4ColCnt = m_colColTitles.c4ElemCount();
    TItemData               idNew(c4ColCnt);
    for (tCIDLib::TCard4 c4RowInd = 0; c4RowInd < c4RowCnt; c4RowInd++)
    {
        // Load the column text to our item data object
        for (tCIDLib::TCard4 c4ColIndex = 0; c4ColIndex < c4ColCnt; c4ColIndex++)
            idNew.m_colCols[c4ColIndex] = mclopsSrc.strColumnText(c4RowInd, c4ColIndex);

        // Store the row id and add it to our list
        idNew.m_c4RowId = mclopsSrc.c4RowId(c4RowInd);
        m_colList.objAdd(idNew);
    }

    if (tCIDLib::bAllBitsOn(m_eMCLBStyles, tCIDCtrls::EMCLBStyles::Sorted))
        SortList();

    //
    //  Set up the flags. If we flushed any previous content, we want to let it
    //  scroll appropriately. If we appended, then we let it stay where it is.
    //
    tCIDCtrls::TLParam lFlags = LVSICF_NOINVALIDATEALL;
    if (c4OrgCount)
        lFlags |= LVSICF_NOSCROLL;

    // Set the item count
    ::SendMessage(m_hwndListView, LVM_SETITEMCOUNT, m_colList.c4ElemCount(), lFlags);

    //
    //  See we were emtpy (now have one item, and are not multi-select, then select
    //  the 0th item (visible order)
    //
    if (!bIsMultiSel() && (m_colList.c4ElemCount() == 1))
        SelectByIndex(0);

    UpdateAutoLast();
}


//
//  Make sure that the indicated item is visible. This is something the list window
//  has to do for us.
//
tCIDLib::TVoid
TMultiColListBox::MakeItemVisible(  const   tCIDLib::TCard4     c4Index
                                    , const tCIDLib::TBoolean   bFullyVisible)
{
    CheckRowIndex(c4Index, CID_LINE);
    ::SendMessage(m_hwndListView, LVM_ENSUREVISIBLE, c4Index, bFullyVisible ? TRUE : FALSE);
}


//
//  Mark or unmark a specific item in a multi-select list.
//
tCIDLib::TVoid
TMultiColListBox::MarkAt(const tCIDLib::TCard4 c4Index, const tCIDLib::TBoolean bState)
{
    CheckRowIndex(c4Index, CID_LINE);

    // Only valid for multi-select
    if (!bIsMultiSel())
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_NotMultiSelect
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotSupported
            , TCardinal(widThis())
        );
    }

    LVITEM Item = {0};
    Item.mask = LVIF_STATE;

    Item.stateMask = LVIS_SELECTED;
    if (bState)
        Item.state = LVIS_SELECTED;
    else
        Item.state = 0;

    const tCIDCtrls::TMsgResult mresSel = ::SendMessage
    (
        m_hwndListView, LVM_SETITEMSTATE, c4Index, tCIDCtrls::TLParam(&Item)
    );

    if (!mresSel)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_MarkAt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4Index)
            , TCardinal(widThis())
        );
    }
}


//
//  Mark or unmark all of the items in a multi-select list box. This is something we
//  still do at the window control level.
//
tCIDLib::TVoid TMultiColListBox::MarkAll(const tCIDLib::TBoolean bState)
{
    // Only valid for multi-select
    if (!bIsMultiSel())
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_NotMultiSelect
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotSupported
            , TCardinal(widThis())
        );
    }

    TWndPaintJanitor janDraw(this);

    LVITEM Item = {0};
    Item.mask = LVIF_STATE;
    Item.stateMask = LVIS_SELECTED;
    if (bState)
        Item.state = LVIS_SELECTED;
    else
        Item.state = 0;
    const tCIDCtrls::TMsgResult mresSel = ::SendMessage
    (
        m_hwndListView, LVM_SETITEMSTATE, -1, tCIDCtrls::TLParam(&Item)
    );

    if (!mresSel)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_MarkAll
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(widThis())
        );
    }
}


//
//  Moves a row from one index to another. Since we are a user data type of control, this
//  isn't too too difficult. We just pull it out of the list and put it back somewhere
//  else.
//
//  They can ask that we keep the selection updated, i.e. if the moved item was selected
//  (or marked if multi-select) it will remain so in the new location.
//
//  If not following the selection, then if either of them was the current selection
//  in a single selection list, then we send a selection change event, either because
//  it's now pointing to something different even though the index didn't change or
//  because we moved the selection to stay with the original.
//
//  We have to deal with the special case of moving to the end. That is one beyond the
//  actual end. But it has to be allowed in order to get an item to the end.
//
tCIDLib::TVoid
TMultiColListBox::MoveRow(  const   tCIDLib::TCard4     c4From
                            , const tCIDLib::TCard4     c4To
                            , const tCIDLib::TBoolean   bFollowSel)
{
    // Validate both row indices, but check for the to to be one more
    CheckRowIndex(c4From, CID_LINE);
    if (c4To != m_colList.c4ElemCount())
        CheckRowIndex(c4To, CID_LINE);

    //
    //  If multi-select, see if the item to move it marked or not
    //
    const tCIDLib::TBoolean bIsMulti = bIsMultiSel();
    tCIDLib::TBoolean bWasMarked = kCIDLib::False;
    if (bIsMulti)
        bWasMarked = bIsMarked(c4From);

    // Prevent redrawing until we have it done, so that it doesn't get ragged looking
    TWndPaintJanitor janPaint(this);

    //
    //  Get a copy of the data for the from row and then remove. We do a real remove
    //  here so that we get the marks adjusted for us if we are a multi-select. We
    //  tell it not to select something else for us.
    //
    TItemData idFrom = m_colList[c4From];
    RemoveAt(c4From, kCIDLib::False);

    //  If the from is below the to, we have to subtract 1 since we reduced the indices
    //  beyond the from point. We did a real remove above, so we have to do a real
    //  add here. That also deals with the issue of moving to the end, where the to
    //  index is one past the actual end.
    //
    tCIDLib::TCard4 c4AddBackAt = c4To;
    if (c4From < c4To)
        c4AddBackAt--;

    c4AddItem(idFrom.m_colCols, idFrom.m_c4RowId, c4AddBackAt, kCIDLib::False);
    m_colList[c4AddBackAt].m_bFlag = idFrom.m_bFlag;
    m_colList[c4AddBackAt].m_c4UserData = idFrom.m_c4UserData;
    m_colList[c4AddBackAt].m_c8UserData = idFrom.m_c8UserData;
    if (idFrom.m_pstrUserData)
        m_colList[c4AddBackAt].m_pstrUserData = new TString(*idFrom.m_pstrUserData);

    // Just in case either was visible, redraw the visible ones
    RedrawVisible();

    if (bIsMulti)
    {
        if (bFollowSel)
        {
            // Mark the to location if from was originally so
            MarkAt(c4AddBackAt, bWasMarked);
        }
    }
     else
    {
        const tCIDLib::TCard4 c4CurSel = c4CurItem();
        if (c4From == c4CurSel)
        {
            if (bFollowSel)
            {
                // We have to move the selection to the new place
                if (c4From == c4CurSel)
                    SelectByIndex(c4AddBackAt);
            }
             else
            {
                //
                //  Force an event even though the index didn't change, because there's
                //  a new item in the same slot now.
                //
                SendListEvent
                (
                    tCIDCtrls::EListEvents::SelChanged
                    , c4CurSel
                    , m_colList[c4CurSel].m_c4RowId
                );
            }
        }
    }
}


// Get the area of the indicated column
tCIDLib::TVoid
TMultiColListBox::QueryColArea( const   tCIDLib::TCard4     c4Row
                                , const tCIDLib::TCard4     c4Col
                                ,       TArea&              areaToFill
                                , const tCIDLib::TBoolean   bScreenRel) const
{
    CheckRowIndex(c4Row, CID_LINE);

    RECT rectCol;
    int iRes;

    //
    //  Another stupidity of this control. You can't get the area of the 0th
    //  column. If you pass a zero sub-item to sub-item rect, you get the whole
    //  thing. If you call get item rect, you get the whole thing.
    //
    //  So, if it's the 0th column, we get the item rect. Then, we query the
    //  width of the 0th column.
    //
    if (c4Col)
    {
        iRes = ListView_GetSubItemRect(m_hwndListView, c4Row, c4Col, LVIR_BOUNDS, &rectCol);
    }
     else
    {
        iRes = ListView_GetItemRect(m_hwndListView, c4Row, &rectCol, LVIR_LABEL);
        if (iRes && (m_colColTitles.c4ElemCount() > 1))
        {
            LVCOLUMN ColInfo = {0};
            ColInfo.mask = LVCF_WIDTH;
            iRes = ListView_GetColumn(m_hwndListView, 0, &ColInfo);

            if (iRes)
                rectCol.right = rectCol.left + ColInfo.cx;
        }
    }

    if (!iRes)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_GetColArea
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4Row)
            , TCardinal(c4Col)
            , TCardinal(widThis())
        );
    }

    if (bScreenRel)
    {
        TArea areaTmp;
        areaTmp.FromRectl
        (
            reinterpret_cast<tCIDLib::THostRectl&>(rectCol)
            , tCIDLib::ERectlTypes::NonInclusive
        );
        ToScreenCoordinates(areaTmp, areaToFill);
    }
     else
    {
       areaToFill.FromRectl
        (
            reinterpret_cast<tCIDLib::THostRectl&>(rectCol)
            , tCIDLib::ERectlTypes::NonInclusive
        );
    }
}


//
//  Query the text of the indicated column. We just get it out of our data
//  directly.
//
tCIDLib::TVoid
TMultiColListBox::QueryColText( const   tCIDLib::TCard4 c4RowIndex
                                , const tCIDLib::TCard4 c4ColIndex
                                ,       TString&        strToFill) const
{
    CheckRowIndex(c4RowIndex, CID_LINE);
    strToFill = m_colList[c4RowIndex].m_colCols[c4ColIndex];
}


//
//  Query the current widths of all of the columns, mainly so that the client code
//  can restore them later.
//
tCIDLib::TVoid TMultiColListBox::QueryColWidths(tCIDLib::TCardList& fcolToFill) const
{
    const tCIDLib::TCard4 c4ColCount = m_colColTitles.c4ElemCount();
    fcolToFill.RemoveAll();

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ColCount; c4Index++)
    {
        tCIDCtrls::TMsgResult mresCur = ::SendMessage
        (
            m_hwndListView, LVM_GETCOLUMNWIDTH, c4Index, 0
        );

        if (!mresCur)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcList_GetColWidth
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TCardinal(c4Index)
                , TCardinal(widThis())
            );
        }
        fcolToFill.c4AddElement(mresCur);
    }
}


// Get the area of the indicated row
tCIDLib::TVoid
TMultiColListBox::QueryRowArea( const tCIDLib::TCard4 c4Row, TArea& areaToFill) const
{
    CheckRowIndex(c4Row, CID_LINE);

    RECT rectCol;
    int iRes = ListView_GetItemRect(m_hwndListView, c4Row, &rectCol, LVIR_BOUNDS);
    if (!iRes)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_GetRowArea
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4Row)
            , TCardinal(widThis())
        );
    }

    areaToFill.FromRectl
    (
        reinterpret_cast<tCIDLib::THostRectl&>(rectCol)
        , tCIDLib::ERectlTypes::NonInclusive
    );
}


tCIDLib::TVoid
TMultiColListBox::RedrawRange(  const   tCIDLib::TCard4 c4StartInd
                                , const tCIDLib::TCard4 c4EndInd)
{
    tCIDLib::TCard4 c4First;
    tCIDLib::TCard4 c4Last;
    if (c4StartInd > c4EndInd)
    {
        c4Last = c4StartInd;
        c4First = c4EndInd;
    }
     else
    {
        c4First = c4StartInd;
        c4Last = c4EndInd;
    }

    CheckRowIndex(c4StartInd, CID_LINE);

    if (!::SendMessage(m_hwndListView, LVM_REDRAWITEMS, c4First, c4Last))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_RedrawRange
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(widThis())
        );
    }

    // Have to force an update since the above only adds the area to the invalid region
    ::UpdateWindow(m_hwndListView);
}


tCIDLib::TVoid TMultiColListBox::RedrawRow(const tCIDLib::TCard4 c4RowIndex)
{
    CheckRowIndex(c4RowIndex, CID_LINE);

    // Get the area of this row and force it to redraw
    TArea areaRow;
    QueryRowArea(c4RowIndex, areaRow);
    Redraw(areaRow, tCIDCtrls::ERedrawFlags::Full);
}


//
//  Get the range of items that are visible, and then call RedrawRange to make them
//  redraw.
//
tCIDLib::TVoid TMultiColListBox::RedrawVisible()
{
    // Get the top item
    tCIDLib::TCard4 c4Start = c4TopIndex();

    // If none, then nothing to do
    if (c4Start == kCIDLib::c4MaxCard)
        return;

    //
    //  Get a point that's at the bottom of our window and find the item that is closest
    //  to that. We take that as the last item.
    //
    TPoint pntEnd = areaClient().pntLL();

    LVFINDINFO Find = {0};
    Find.flags = LVFI_NEARESTXY;
    Find.pt.x = pntEnd.i4X();
    Find.pt.y = pntEnd.i4Y();

    tCIDCtrls::TMsgResult mresEnd = ::SendMessage
    (
        m_hwndListView, LVM_FINDITEM, c4Start, tCIDCtrls::TLParam(&Find)
    );

    // If that fails, go with the item count
    if (mresEnd == -1)
        mresEnd = c4ItemCount();

    RedrawRange(c4Start, mresEnd);
}


//
//  Just a convenience since there's no built in way to remove a range of items. We just
//  do them one at a time. But we make sure it's done safely and as effiently as is
//  reasonable.
//
tCIDLib::TVoid
TMultiColListBox::RemoveRange(  const   tCIDLib::TCard4 c4Start
                                , const tCIDLib::TCard4 c4End)
{
    tCIDLib::TCard4 c4Count = c4ItemCount();
    if ((c4Start >= c4Count) || (c4End >= c4Count))
    {
        const tCIDLib::TCard4 c4Bad((c4Start >= c4Count) ? c4Start : c4End);
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_BadIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4Bad)
            , TCardinal(widThis())
            , TCardinal(c4ItemCount())
        );
    }

    //
    //  Check for the special case where they've essentially indicated the whole thing
    //  and optimize that case.
    //
    if ((c4Start == 0) && (c4End + 1 == c4Count))
    {
        RemoveAll();
    }
     else
    {
        //
        //  Do them in reverse order so that the indices stay correct, and potentially it
        //  minimizes the amount of copying down data. And stop redrawing while we do this
        //  to speed it up if it's a fair number of items.
        //
        TWndPaintJanitor janPaint(this);

        // Use signed indices to avoid potential wrap around
        tCIDLib::TInt4 i4Index = tCIDLib::TInt4(c4End);
        while (i4Index >= tCIDLib::TInt4(c4Start))
        {
            //
            //  Tell it not to select another one, we'll do that at the end to avoid a long
            //  sequence of useless selection events being sent.
            //
            RemoveAt(tCIDLib::TCard4(i4Index), kCIDLib::False);
            i4Index--;
            c4Count--;
        }

        //
        //  We know we didn't remove them all, so let's select a new one now. If the range
        //  was at the end, the we have to move back one, else select the one that fell into
        //  the starting index. Force a selection event in case the selection is already on
        //  the index we use.
        //
        SelectByIndex((c4Start >= c4Count) ? c4Start - 1 : c4Start, kCIDLib::True);
    }
}


//
//  This can be called after renaming one or more items, since that will not automatically
//  cause the list to be resorted. We'll do it even if we are not marked for auto-sorting,
//  to allow for manual sorting where desired.
//
tCIDLib::TVoid TMultiColListBox::Resort()
{
    SortList();
    RedrawVisible();
}



// Return the per-row user data for the indicated row
const TString&
TMultiColListBox::strUserDataAt(const tCIDLib::TCard4 c4Index) const
{
    CheckRowIndex(c4Index, CID_LINE);

    if (m_colList[c4Index].m_pstrUserData)
        return *m_colList[c4Index].m_pstrUserData;
    return TString::strEmpty();
}


//
//  Returns the size of the grid. So we get the area of the last column and return the
//  horizontal size, and we get the area of one of the columns on the last row and reutrn
//  the vertical size.
//
TSize TMultiColListBox::szGridExtent() const
{
    const tCIDLib::TCard4 c4RowCnt = c4ItemCount();
    const tCIDLib::TCard4 c4ColCnt = m_colColTitles.c4ElemCount();

    // If either no columns or no rows, return empty
    if (!c4RowCnt || !c4ColCnt)
        return TSize();

    // Get the last cell of the first row
    TSize szRet;
    TArea areaCell;
    QueryColArea(0, c4ColCnt - 1, areaCell);
    szRet.c4Width(tCIDLib::TCard4(areaCell.i4Right() + 1));

    // Get the 0th cell of the last row
    QueryColArea(c4RowCnt - 1, 0, areaCell);
    szRet.c4Height(tCIDLib::TCard4(areaCell.i4Bottom() + 1));

    return szRet;
}


//
//  Make sure we are bottom scrolled.
//
tCIDLib::TVoid TMultiColListBox::ScrollToBottom()
{
    const tCIDLib::TCard4 c4Count = c4ItemCount();
    if (c4Count)
        MakeItemVisible(c4Count - 1, kCIDLib::True);
}


//
//  Set an auto in-place editor interface. We don't own it, since it's just a mixin
//  usually implemented by client code or perhaps a derived class.
//
tCIDLib::TVoid TMultiColListBox::SetAutoIPE(MMCLBIPEIntf* const pmcipeToSet)
{
    m_pipeiAuto = pmcipeToSet;
}


// This updates a given column in all rows to a particular value
tCIDLib::TVoid
TMultiColListBox::SetColTo( const   tCIDLib::TCard4 c4ColIndex
                            , const TString&        strToSet)
{
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        m_colList[c4Index].m_colCols[c4ColIndex] = strToSet;

    // If this is the sort column, we have to resort
    if (tCIDLib::bAllBitsOn(m_eMCLBStyles,  tCIDCtrls::EMCLBStyles::Sorted)
    &&  (c4ColIndex == m_c4SortCol))
    {
        SortList();
    }

    // Redraw the visible items
    RedrawVisible();
}


//
//  If created from a dialog description, then the columns have to be set after
//  the fact. This also has to be called if you want to searchs to work against
//  a column other than 0.
//
tCIDLib::TVoid
TMultiColListBox::SetColumns(const  tCIDLib::TStrCollect&   colColTitles
                            , const tCIDLib::TCard4         c4SearchCol)
{
    if (c4SearchCol >= colColTitles.c4ElemCount())
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_BadColIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4SearchCol)
            , TCardinal(widThis())
        );
    }

    m_c4SearchCol = c4SearchCol;
    TColCursor<TString>& cursCols = *colColTitles.pcursNew();
    TJanitor<TColCursor<TString>> janCurs(&cursCols);
    for (; cursCols; ++cursCols)
        m_colColTitles.objAdd(*cursCols);

    LoadColumns();
}


// Manually set a column width
tCIDLib::TVoid
TMultiColListBox::SetColOpts(const  tCIDLib::TCard4     c4ColIndex
                            , const tCIDLib::TCard4     c4Width
                            , const tCIDLib::EHJustify  eJustify
                            , const tCIDLib::TCard4     c4MinWidth)
{
    const tCIDLib::TCard4 c4Count = c4ColCount();

    // Make sure it's a valid column
    if (c4ColIndex >= c4Count)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_BadColIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4ColIndex)
            , TCardinal(widThis())
        );
    }

    LVCOLUMN ColInfo = {0};
    ColInfo.mask = LVCF_FMT | LVCF_SUBITEM;
    ColInfo.iSubItem = c4ColIndex;

    if (eJustify == tCIDLib::EHJustify::Left)
        ColInfo.fmt = LVCFMT_LEFT;
    else if (eJustify == tCIDLib::EHJustify::Center)
        ColInfo.fmt = LVCFMT_CENTER;
    else if (eJustify == tCIDLib::EHJustify::Right)
        ColInfo.fmt = LVCFMT_RIGHT;

    if (c4MinWidth != 0)
    {
        ColInfo.mask |= LVCF_MINWIDTH;
        ColInfo.cxMin = c4MinWidth;
    }

    //
    //  Only explicitly set the last column if not auto-sizing it and if the passed width
    //  isn't max card.
    //
    if (c4Width != kCIDLib::c4MaxCard)
    {
        if (!tCIDLib::bAllBitsOn(m_eMCLBStyles, tCIDCtrls::EMCLBStyles::AutoLastCol)
        ||  (c4ColIndex + 1 != c4Count))
        {
            ColInfo.mask |= LVCF_WIDTH;
            ColInfo.cx = c4Width;
        }
    }

    if (!ListView_SetColumn(m_hwndListView, int(c4ColIndex), &ColInfo))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_SetColumnInfo
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4ColIndex)
            , TCardinal(widThis())
        );
    }

    UpdateAutoLast();
}


// Update the text of a single column cell
tCIDLib::TVoid
TMultiColListBox::SetColText(const  tCIDLib::TCard4 c4RowIndex
                            , const tCIDLib::TCard4 c4ColIndex
                            , const TString&        strToSet)
{
    // Make sure it's a valid row
    CheckRowIndex(c4RowIndex, CID_LINE);

    // Make sure it's a valid column
    const tCIDLib::TCard4 c4ColCnt = c4ColCount();
    if (c4ColIndex >= c4ColCnt)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_BadColIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4ColIndex)
            , TCardinal(widThis())
        );
    }

    // Update our data, then get the column area and redraw it
    m_colList[c4RowIndex].m_colCols[c4ColIndex] = strToSet;

    // Invalidate the area of the target cell
    TArea areaCol;
    QueryColArea(c4RowIndex, c4ColIndex, areaCol);
    Redraw(areaCol, tCIDCtrls::ERedrawFlags::Full);
}


// Update the text of a column title
tCIDLib::TVoid
TMultiColListBox::SetColTitle(  const   tCIDLib::TCard4 c4ColIndex
                                , const TString&        strToSet)
{
    LVCOLUMN ColInfo = {0};
    ColInfo.mask = LVCF_TEXT;
    ColInfo.pszText = (LPTSTR)strToSet.pszBuffer();

    if (!::SendMessage(m_hwndListView, LVM_SETCOLUMN, c4ColIndex, tCIDCtrls::TLParam(&ColInfo)))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_SetColTitle
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4ColIndex)
            , TCardinal(widThis())
        );
    }
}


// Set up the width of all of our columns
tCIDLib::TVoid TMultiColListBox::SetColWidths(const tCIDLib::TCardList& fcolToSet)
{
    const tCIDLib::TCard4 c4ColCount = m_colColTitles.c4ElemCount();
    if (fcolToSet.c4ElemCount() != c4ColCount)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_WrongColCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4ColCount)
            , TCardinal(fcolToSet.c4ElemCount())
            , TCardinal(widThis())
        );
    }

    // For each column update it's widgets
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ColCount; c4Index++)
    {
        ::SendMessage(m_hwndListView, LVM_SETCOLUMNWIDTH, c4Index, fcolToSet[c4Index]);
    }
}


// Set custom drawing options
tCIDLib::TVoid
TMultiColListBox::SetCustomDrawOpts(const tCIDCtrls::EMCLBCustOpts eToSet)
{
    m_eCustOpts = eToSet;
}


// Set the per-row flag value for the indicated index
tCIDLib::TVoid
TMultiColListBox::SetFlagAt(const   tCIDLib::TCard4     c4Index
                            , const tCIDLib::TBoolean   bToSet)
{
    CheckRowIndex(c4Index, CID_LINE);
    m_colList[c4Index].m_bFlag = bToSet;
}


tCIDLib::TVoid
TMultiColListBox::SetMCLBStyle( const   tCIDCtrls::EMCLBStyles  eToChange
                                , const tCIDLib::TBoolean       bNewState)
{
    if (bNewState)
        m_eMCLBStyles |= eToChange;
    else
        m_eMCLBStyles = tCIDLib::eClearEnumBits(m_eMCLBStyles, eToChange);
}


// Set the per-row user data value for the indicated index
tCIDLib::TVoid
TMultiColListBox::SetUserDataAt(const   tCIDLib::TCard4 c4Index
                                , const tCIDLib::TCard4 c4ToSet)
{
    CheckRowIndex(c4Index, CID_LINE);
    m_colList[c4Index].m_c4UserData = c4ToSet;
}

tCIDLib::TVoid
TMultiColListBox::SetUserDataAt(const   tCIDLib::TCard4 c4Index
                                , const tCIDLib::TCard8 c8ToSet)
{
    CheckRowIndex(c4Index, CID_LINE);
    m_colList[c4Index].m_c8UserData = c8ToSet;
}

tCIDLib::TVoid
TMultiColListBox::SetUserDataAt(const   tCIDLib::TCard4 c4Index
                                , const TString&        strToSet)
{
    CheckRowIndex(c4Index, CID_LINE);

    if (m_colList[c4Index].m_pstrUserData)
        *m_colList[c4Index].m_pstrUserData = strToSet;
    else
        m_colList[c4Index].m_pstrUserData = new TString(strToSet);
}


//
//  Swaps two rows. Since we are a user data type of control, this isn't too
//  too difficult. We just swap the items in our list. If either is visible, we
//  redraw the visible items.
//
//  They can ask that, if the first index is the selection index, that we move the
//  selection to keep it on that item.
//
//  If not following the selection, then if either of them was the current selection
//  in a single selection list, then we send a selection change event, either because
//  it's now pointing to something different even though the index didn't change or
//  because we moved the selection to stay with the original.
//
tCIDLib::TVoid
TMultiColListBox::SwapRows( const   tCIDLib::TCard4     c4First
                            , const tCIDLib::TCard4     c4Sec
                            , const tCIDLib::TBoolean   bFollowSel)
{
    // Validate both row indices
    CheckRowIndex(c4First, CID_LINE);
    CheckRowIndex(c4Sec, CID_LINE);

    //
    //  If multi-select, we have to do things a bit differently. We want to keep the
    //  items marked if they were or not if they weren't.
    //
    const tCIDLib::TBoolean bIsMulti(bIsMultiSel());
    tCIDLib::TBoolean bFirstMarked = kCIDLib::False;
    tCIDLib::TBoolean bSecMarked = kCIDLib::False;
    if (bIsMulti)
    {
        bFirstMarked = bIsMarked(c4First);
        bSecMarked = bIsMarked(c4Sec);
    }

    TItemData idFirst = m_colList[c4First];
    m_colList[c4First] = m_colList[c4Sec];
    m_colList[c4Sec] = idFirst;

    // Just in case either was visible, redraw the visible ones
    RedrawVisible();

    // Handle marking/selection
    if (bIsMulti)
    {
        if (bFollowSel)
        {
            // Mark each one with the state of the other now
            MarkAt(c4Sec, bFirstMarked);
            MarkAt(c4First, bSecMarked);
        }
    }
     else
    {
        //
        //  If either of them was teh selected item, then either keep it selected,
        //  or force a faux reselection of the item that now falls into the uncovered
        //  slot.
        //
        const tCIDLib::TCard4 c4CurSel = c4CurItem();
        if ((c4First == c4CurSel) || (c4Sec == c4CurSel))
        {
            // One of them was the current selection
            if (bFollowSel)
            {
                // We have to move the selection to the other one
                if (c4First == c4CurSel)
                    SelectByIndex(c4Sec);
                else
                    SelectByIndex(c4First);
            }
             else
            {
                //
                //  Force an event even though the index didn't change. There's a new
                //  guy at the index now.
                //
                SendListEvent
                (
                    tCIDCtrls::EListEvents::SelChanged
                    , c4CurSel
                    , m_colList[c4CurSel].m_c4RowId
                );
            }
        }
    }
}


//
//  Update the id of the indicated row. No need to involve the list control in
//  this case. We just store it away.
//
tCIDLib::TVoid
TMultiColListBox::UpdateIdAt(const  tCIDLib::TCard4 c4RowIndex
                            , const tCIDLib::TCard4 c4NewId)
{
    CheckRowIndex(c4RowIndex, CID_LINE);
    m_colList[c4RowIndex].m_c4RowId = c4NewId;
}



//
//  Update the indicated row, using a list ops object. If we are managing the data
//  we update our info. If the parent is managing the data, they can still call this
//  in order to get the row parameter updated and to force the required redraw, which
//  would not otherwise happen since we don't actually update any visual data of
//  the list itself.
//
tCIDLib::TVoid
TMultiColListBox::UpdateRowAt(  const   TMCListOps&     mclopsSrc
                                , const tCIDLib::TCard4 c4RowIndex)
{
    CheckRowIndex(c4RowIndex, CID_LINE);

    const tCIDLib::TCard4 c4Count= c4ColCount();
    for (tCIDLib::TCard4 c4ColIndex = 0; c4ColIndex < c4Count; c4ColIndex++)
        m_colList[c4RowIndex].m_colCols[c4ColIndex] = mclopsSrc.strColumnText(0, c4ColIndex);

    // Update the id
    m_colList[c4RowIndex].m_c4RowId = mclopsSrc.c4RowId(c4RowIndex);

    // Force a redraw of the row
    TArea areaRow;
    QueryRowArea(c4RowIndex, areaRow);
    Redraw(areaRow, tCIDCtrls::ERedrawFlags::Full);
}

tCIDLib::TVoid
TMultiColListBox::UpdateRowAt(  const   tCIDLib::TStrList&  colVals
                                , const tCIDLib::TCard4     c4RowIndex
                                , const tCIDLib::TCard4     c4RowId
                                , const tCIDLib::TBoolean   bRedraw)
{
    CheckRowIndex(c4RowIndex, CID_LINE);

    const tCIDLib::TCard4 c4Count= c4ColCount();
    for (tCIDLib::TCard4 c4ColIndex = 0; c4ColIndex < c4Count; c4ColIndex++)
        m_colList[c4RowIndex].m_colCols[c4ColIndex] = colVals[c4ColIndex];

    // Update the id
    m_colList[c4RowIndex].m_c4RowId = c4RowId;

    // Force a redraw of the row if asked
    if (bRedraw)
    {
        TArea areaRow;
        QueryRowArea(c4RowIndex, areaRow);
        Redraw(areaRow, tCIDCtrls::ERedrawFlags::Full);
    }
}


// ---------------------------------------------------------------------------
//  TMultiColListBox: Protected, virtual methods
// ---------------------------------------------------------------------------

tCIDCtrls::EMCLBCustRets
TMultiColListBox::eCustomDraw(          TGraphDrawDev&
                                , const tCIDLib::TCard4
                                , const tCIDLib::TCard4
                                , const tCIDLib::TBoolean
                                , const TArea&
                                ,       TRGBClr&
                                ,       TRGBClr&)
{
    //
    //  If not overridden, do nothing. Shouldn't happen since they have to set a
    //  style to get this to be called, so they should handle it.
    //
    return tCIDCtrls::EMCLBCustRets::None;
}


tCIDLib::TVoid
TMultiColListBox::CellClicked(  const   tCIDLib::TCard4     c4RowIndex
                                , const tCIDLib::TCard4     c4ColIndex
                                , const tCIDLib::TBoolean   bLeftButton)
{
    //
    //  If not overriden, we just send out a column clicked notification if it's
    //  a left button. If it's a column title click, then the row index is max
    //  card.
    //
    if (bLeftButton)
    {
        SendListEvent
        (
            tCIDCtrls::EListEvents::ColClicked
            , c4RowIndex
            , c4IndexToId(c4RowIndex)
            , c4ColIndex
        );
    }
}


tCIDLib::TVoid TMultiColListBox::ColumnsSet(const tCIDLib::TCard4)
{
    // Do nothing if not overridden
}


//
//  If they don't override this, we provide standard string based column comparison
//
tCIDLib::ESortComps
TMultiColListBox::eCompColVals( const   TString&        strVal1
                                , const TString&        strVal2
                                , const tCIDLib::TCard4 c4ColInd) const
{
    return strVal1.eCompare(strVal2);
}


//
//  If not overridden, then see if we support column sorting. If so, we'll re-sort
//  by the clicked column. Else, we'll send a notification in case a listener wants
//  to handle it (with the row index set to max card.)
//
tCIDLib::TVoid TMultiColListBox::TitleClicked(const tCIDLib::TCard4 c4ColIndex)
{
    if (tCIDLib::bAllBitsOn(m_eMCLBStyles, tCIDCtrls::EMCLBStyles::AutoSort))
    {
        //
        //  If we are already sorted by this column, then we switch sort directions.
        //  Else we make this the new sort column and set it to ascending direction.
        //
        if (!m_colList.bIsEmpty())
        {
            if (c4ColIndex == m_c4SortCol)
                m_bSortAsc = !m_bSortAsc;
            else
                m_bSortAsc = kCIDLib::True;

            // Store the incoming column, which may be the same thing
            m_c4SortCol = c4ColIndex;

            // And save the current item index
            const tCIDLib::TCard4 c4OrgInd = c4CurItem();

            // Sort by the currently selected direction
            SortList();

            // Redraw now to make it pick up the changes
            RedrawVisible();

            //
            //  Reselect the original item if there was one. Force an event so that any
            //  data dependent on the current selection updates.
            //
            if (c4OrgInd != kCIDLib::c4MaxCard)
                SelectByIndex(c4OrgInd, kCIDLib::True);
        }
    }
     else
    {
        SendListEvent
        (
            tCIDCtrls::EListEvents::ColClicked
            , kCIDLib::c4MaxCard
            , kCIDLib::c4MaxCard
            , c4ColIndex
        );
    }
}


// ---------------------------------------------------------------------------
//  TMultiColListBox: Protected, inherited methods
// ---------------------------------------------------------------------------

// We have to keep our nested listview sized to fit
tCIDLib::TVoid
TMultiColListBox::AreaChanged(  const   TArea&                  areaPrev
                                , const TArea&                  areaNew
                                , const tCIDCtrls::EPosStates   ePosState
                                , const tCIDLib::TBoolean       bOrgChanged
                                , const tCIDLib::TBoolean       bSizeChanged
                                , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    if ((ePosState != tCIDCtrls::EPosStates::Minimized) && m_hwndListView)
    {
        if (bSizeChanged)
        {
            tCIDLib::TCard4 c4Flags = SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOACTIVATE
                                      | SWP_NOMOVE | SWP_NOCOPYBITS;

            // Get the area inside any border
            TArea areaLV = areaClient();
            ::SetWindowPos
            (
                m_hwndListView
                , 0
                , 0
                , 0
                , areaLV.c4Width()
                , areaLV.c4Height()
                , c4Flags
            );

            if (tCIDLib::bAllBitsOn(m_eMCLBStyles, tCIDCtrls::EMCLBStyles::AutoLastCol))
                UpdateAutoLast();
        }
    }
}


tCIDLib::TBoolean TMultiColListBox::bCreated()
{
    // Call our parent first
    TParent::bCreated();

    //
    //  Disable notification reflection. We need the notifications sent from our nested
    //  list view to come to us, not go back to him.
    //
    DisableNotReflection();

    //
    //  Has to be a child window, plus any other styles we enforce for this
    //  type of window.
    //
    tCIDLib::TCard4 c4Styles = LVS_REPORT | LVS_OWNERDATA | WS_CHILD | WS_TABSTOP
                               | WS_VISIBLE | LVS_SHOWSELALWAYS;

    if (!tCIDLib::bAllBitsOn(m_eMCLBStyles, tCIDCtrls::EMCLBStyles::MultiSel))
        c4Styles |= LVS_SINGLESEL;

    if (tCIDLib::bAllBitsOn(m_eMCLBStyles, tCIDCtrls::EMCLBStyles::NoTitles))
        c4Styles |= LVS_NOCOLUMNHEADER;

    if (!tCIDLib::bAllBitsOn(m_eMCLBStyles, tCIDCtrls::EMCLBStyles::ColSort))
        c4Styles |= LVS_NOSORTHEADER;

    if (tCIDLib::bAllBitsOn(m_eOrgStyles, tCIDCtrls::EWndStyles::TabStop))
        c4Styles |= WS_TABSTOP;

    if (tCIDLib::bAllBitsOn(m_eOrgStyles, tCIDCtrls::EWndStyles::Group))
        c4Styles |= WS_GROUP;

    //
    //  KEEP THIS ON, or redraw issues can happen! It can get caught in an infinite redrawing
    //  loop, for reasons completely unfathomable. Unfortunately, leaving it on also causes it
    //  to not really redraw it's content reliably, hence why we jump through hoops in this
    //  class and create a nested control.
    //
    c4Styles |= WS_CLIPCHILDREN;

    //
    //  And create the list view window. We set it to our client area, which should
    //  be within any border.
    //
    //  We give it the same id as us. Since it's our only child, any id is fine. And
    //  this way we don't have to translate the id when we pass through notifications
    //  and such.
    //
    TArea areaLV = areaClient();
    m_hwndListView = ::CreateWindowExW
    (
        tCIDLib::c4EnumOrd(tCIDCtrls::EExWndStyles::None)
        , WC_LISTVIEW
        , L""
        , c4Styles
        , areaLV.i4X()
        , areaLV.i4Y()
        , areaLV.c4Width()
        , areaLV.c4Height()
        , hwndThis()
        , (HMENU)widThis()
        , *(HINSTANCE*)&facCIDCtrls().kmodThis().hmodThis().hmodiThis()
        , 0
    );

    // Store a pointer to us in the list view's extra user data
    ::SetWindowLongPtr(m_hwndListView, GWL_USERDATA, (LONG_PTR)this);

    //
    //  And let's sub-class it to our local handler. This will just continue to set the
    //  original function pointer, but oh well.
    //
    pfnOrgLVProc = (tCIDCtrls::TDispatchFunc)::SetWindowLong
    (
        m_hwndListView, GWLP_WNDPROC, tCIDCtrls::TLParam(&mresLVDispatch)
    );

    // Set any extended styles
    if (m_eMCLBStyles != tCIDCtrls::EMCLBStyles::None)
    {
        tCIDLib::TCard4 c4ExStyles = 0; // LVS_EX_DOUBLEBUFFER;

        if (tCIDLib::bAllBitsOn(eMCLBStyles(), tCIDCtrls::EMCLBStyles::FullRowSel))
            c4ExStyles |= LVS_EX_FULLROWSELECT;

        if (tCIDLib::bAllBitsOn(eMCLBStyles(), tCIDCtrls::EMCLBStyles::GridLines))
            c4ExStyles |= LVS_EX_GRIDLINES;

        ::SendMessage(m_hwndListView, LVM_SETEXTENDEDLISTVIEWSTYLE, c4ExStyles, c4ExStyles);
    }

    // Add our columns if we have any yet
    if (!m_colColTitles.bIsEmpty())
        LoadColumns();

    // Allow creation to continue
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMultiColListBox::bHandleLVMsg( const   tCIDCtrls::TWndMsg      wmsgMsg
                                , const tCIDCtrls::TWParam      wParam
                                , const tCIDCtrls::TLParam      lParam
                                ,       tCIDCtrls::TMsgResult&  mresRet)
{
    // Assume we don't eat it until proven otherwise
    tCIDLib::TBoolean bRet = kCIDLib::False;
    switch(wmsgMsg)
    {
        case WM_NOTIFY :
        {
            // Get a pointer to the notification header
            NMHDR* pNotHdr = reinterpret_cast<NMHDR*>(lParam);

            if ((pNotHdr->code == HDN_BEGINTRACK)
            &&  tCIDLib::bAllBitsOn(m_eMCLBStyles, tCIDCtrls::EMCLBStyles::NoColResize))
            {
                // If a begin track and we have the 'no column resize' style, prevent it
                mresRet = 1;
                bRet = kCIDLib::True;
            }
            break;
        }

        //
        //  Handle any button down msgs. Do a hit test and if we are not on an item
        //  eat it, so as to prevent it from deselecting the current selection.
        //
        //  If we are, and it's a left or right down, then we either call a virtual
        //  to allow a derived class to handle the click, or if there is an auto
        //  IPE handler in place, we call a helper to do in place editing.
        //
        case WM_LBUTTONDBLCLK :
        case WM_LBUTTONDOWN :
        case WM_RBUTTONDOWN :
        {
            // Take focus if we don't have it
            if (::GetFocus() != m_hwndListView)
                TakeFocus();

            // See what the user hit
            LVHITTESTINFO Info = {0};
            Info.pt.x = TRawBits::i2Low16From32(lParam);
            Info.pt.y = TRawBits::i2High16From32(lParam);

            tCIDCtrls::TMsgResult mresInd = ::SendMessage
            (
                m_hwndListView, LVM_HITTEST, tCIDCtrls::TWParam(-1), tCIDCtrls::TLParam(&Info)
            );

            // If not -1, then we hit one
            const tCIDLib::TBoolean bHit(mresInd != -1);

            // Get the currently selected item, if any
            const tCIDLib::TCard4 c4OrgIndex = c4CurItem();

            //
            //  If we hit something and it was a button left or right, and it's the
            //  same row as was originally selected, then do the IPE or inform them
            //  of a cell click.
            //
            if (bHit
            &&  ((wmsgMsg == WM_LBUTTONDOWN) || (wmsgMsg == WM_RBUTTONDOWN))
            &&  (mresInd == c4OrgIndex))
            {
                // And we eat this one
                bRet = kCIDLib::True;

                // Find out what cell the click was on
                LVHITTESTINFO Info = {0};
                Info.pt.x = TRawBits::i2Low16From32(lParam);
                Info.pt.y = TRawBits::i2High16From32(lParam);
                ListView_SubItemHitTest(m_hwndListView, &Info);

                // If we hit something
                if (Info.iItem != -1)
                {
                    if (m_pipeiAuto && (wmsgMsg == WM_LBUTTONDOWN))
                    {
                        // Has to be the one already selected
                        if (Info.iItem == c4CurItem())
                            AutoIPE(Info.iItem, Info.iSubItem);
                    }
                     else
                    {
                        CellClicked
                        (
                            Info.iItem, Info.iSubItem, (wmsgMsg == WM_LBUTTONDOWN)
                        );
                    }
                }
            }
             else if (!bHit)
            {
                //
                //  Not a special case, but we didn't hit anything, so eat it to prevent
                //  deselection.
                //
                bRet = kCIDLib::True;
            }
            break;
        }

        case WM_KEYDOWN :
        case WM_SYSKEYDOWN:
        {
            tCIDLib::TBoolean bAlt = (::GetKeyState(VK_MENU) & 0x80) != 0;
            tCIDLib::TBoolean bCtrl = (::GetKeyState(VK_CONTROL) & 0x80) != 0;
            tCIDLib::TBoolean bShift = (::GetKeyState(VK_SHIFT) & 0x80) != 0;

            tCIDCtrls::EExtKeys eKey = eMapExtKey(wParam, bShift);
            if ((eKey == tCIDCtrls::EExtKeys::None) || bExtKey(eKey, bAlt, bCtrl, bShift))
            {
                mresRet = 0;
                bRet = kCIDLib::True;
            }
            break;
        }


        // Normally these would be handled by TWindow but we have to do them ourself
        case WM_KILLFOCUS :
        {
            LosingFocus();
            TWindow* const pwndPar = pwndParent();
            TWindow* pwndCur = pwndPar;
            const tCIDCtrls::TWndId widUs = widThis();
            while (pwndCur)
            {
                if (pwndCur->bWantsChildFocusNot())
                    SendChildFocusNot(*pwndCur, *pwndPar, widUs, kCIDLib::False);
                pwndCur = pwndCur->pwndParent();
            }
            break;
        }

        case WM_SETFOCUS :
        {
            LosingFocus();
            TWindow* const pwndPar = pwndParent();
            TWindow* pwndCur = pwndPar;
            const tCIDCtrls::TWndId widUs = widThis();
            while (pwndCur)
            {
                if (pwndCur->bWantsChildFocusNot())
                    SendChildFocusNot(*pwndCur, *pwndPar, widUs, kCIDLib::True);
                pwndCur = pwndCur->pwndParent();
            }
            break;
        }

        default :
            // Just let it pass on
            break;
    };

    return bRet;
}


//
//  We intercept some keys to report to listeners. In our case, these are intercepted
//  by our internal sub-class of the list view, and forwarded on to us. This way, our
//  derived classes can see them normally as well.
//
tCIDLib::TBoolean
TMultiColListBox::bExtKey(  const   tCIDCtrls::EExtKeys eKey
                            , const tCIDLib::TBoolean   bAlt
                            , const tCIDLib::TBoolean   bCtrl
                            , const tCIDLib::TBoolean   bShift
                            , const TWindow* const      pwndChild)
{
    // No shifted combos supported
    if (bAlt || bCtrl || bShift)
        return kCIDLib::False;

    if (eKey == tCIDCtrls::EExtKeys::Delete)
    {
        // If there's a current selections, send an event
        const tCIDLib::TCard4 c4Index = c4CurItem();
        if (c4Index != kCIDLib::c4MaxCard)
            SendListEvent(tCIDCtrls::EListEvents::DeleteReq, c4Index, c4IndexToId(c4Index));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Our content is always filled by something else and we want to avoid blinkies, so we
//  we suppress this.
//
tCIDLib::TBoolean TMultiColListBox::bEraseBgn(TGraphDrawDev&)
{
    return kCIDLib::True;
}



tCIDLib::TBoolean
TMultiColListBox::bInvoked(const tCIDLib::TCard4, const tCIDLib::TCard4)
{
    // If not overridden, just say we didn't handle it
    return kCIDLib::False;
}


tCIDLib::TBoolean
TMultiColListBox::bNotReflect(          TWindow&                wndTar
                                , const tCIDCtrls::TWndMsg      wmsgMsg
                                , const tCIDCtrls::TWParam      wParam
                                , const tCIDCtrls::TLParam      lParam
                                ,       tCIDCtrls::TMsgResult&  mresResult)
{
    if (wmsgMsg == WM_NOTIFY)
    {
        // Get a pointer to the notification header
        NMHDR* pNotHdr = (NMHDR*)lParam;

        if (pNotHdr->code == LVN_COLUMNCLICK)
        {
            //
            //  We only report column title clicks this way. Regular cell clicking
            //  is handled by button down handling in the sub-class msg handler
            //  method. So there's no conflict.
            //
            //  We call a virtual method. If it's not overridden, it'll just send
            //  out a notification.
            //
            NMLISTVIEW* pInfo = (NMLISTVIEW*)lParam;

            if ((pInfo->iSubItem >= 0)
            &&  (tCIDLib::TCard4(pInfo->iSubItem) < m_colColTitles.c4ElemCount()))
            {
                TitleClicked(pInfo->iSubItem);
                return kCIDLib::True;
            }
        }
         else if (pNotHdr->code == NM_DBLCLK)
        {
            NMITEMACTIVATE* pItem = (NMITEMACTIVATE*)lParam;
            if (pItem->iItem != -1)
            {
                // If not handled by a derived class, just send an event
                if (!bInvoked(pItem->iItem, m_colList[pItem->iItem].m_c4RowId))
                {
                    SendListEvent
                    (
                        tCIDCtrls::EListEvents::Invoked
                        , pItem->iItem
                        , m_colList[pItem->iItem].m_c4RowId
                        , tCIDLib::TCard4(pItem->iSubItem)
                    );
                }
                return kCIDLib::True;
            }
        }
         else if (pNotHdr->code == NM_CUSTOMDRAW)
        {
            mresResult = CDRF_DODEFAULT;
            if (pNotHdr->hwndFrom == m_hwndListView)
            {
                // This is our own message reflected back to us
                if (!m_colList.bIsEmpty() && (m_eCustOpts != tCIDCtrls::EMCLBCustOpts::None))
                {
                    NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)lParam;

                    tCIDCtrls::EMCLBCustRets eCustRet = tCIDCtrls::EMCLBCustRets::None;
                    if (pDraw->nmcd.dwDrawStage == CDDS_PREPAINT)
                    {
                        //
                        //  If any of the pre-post row/col stuff is set, then we need to
                        //  set item drawing.
                        //
                        if (tCIDLib::bAnyBitsOn(m_eCustOpts, tCIDCtrls::EMCLBCustOpts::AllRowCol))
                            mresResult |= CDRF_NOTIFYITEMDRAW;

                        // If they want posts stuff, set that flag
                        if (tCIDLib::bAnyBitsOn(m_eCustOpts, tCIDCtrls::EMCLBCustOpts::AllPost))
                            mresResult |= CDRF_NOTIFYPOSTPAINT;
                    }
                     else if (pDraw->nmcd.dwDrawStage == CDDS_POSTPAINT)
                    {
                        if (tCIDLib::bAllBitsOn(m_eCustOpts, tCIDCtrls::EMCLBCustOpts::PostDraw))
                        {
                            //
                            //  Pass max card for both row and column, to indicate this is
                            //  neither row nor column. REturn dosn't matter here since we
                            //  are done.
                            //
                            TGraphWndDev gdevTmp(pDraw->nmcd.hdc, tCIDLib::EAdoptOpts::NoAdopt);
                            eCustomDraw
                            (
                                gdevTmp
                                , kCIDLib::c4MaxCard
                                , kCIDLib::c4MaxCard
                                , kCIDLib::True
                                , TArea::areaEmpty
                                , m_rgbCustBgn
                                , m_rgbCustText
                            );
                        }
                    }
                     else if (pDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
                    {
                        // If this guy wants to see column stuff, then set that flag
                        if (tCIDLib::bAllBitsOn(m_eCustOpts, tCIDCtrls::EMCLBCustOpts::PreCol)
                        ||  tCIDLib::bAllBitsOn(m_eCustOpts, tCIDCtrls::EMCLBCustOpts::PostCol))
                        {
                            mresResult |= CDRF_NOTIFYSUBITEMDRAW;
                        }

                        //
                        //  If they want posts stuff, set that flag. Note that we have
                        //  to indicate this if we are post row or column, unfortunately
                        //  because otherwise we won't get the post row. But that means
                        //  we'll also get all of the post columns even if we don't want
                        //  them.
                        //
                        if (tCIDLib::bAllBitsOn(m_eCustOpts, tCIDCtrls::EMCLBCustOpts::PostCol)
                        ||  tCIDLib::bAllBitsOn(m_eCustOpts, tCIDCtrls::EMCLBCustOpts::PostRow))
                        {
                            mresResult |= CDRF_NOTIFYPOSTPAINT;
                        }

                        if (tCIDLib::bAllBitsOn(m_eCustOpts, tCIDCtrls::EMCLBCustOpts::PreRow))
                        {
                            //
                            //  For this one, the passed rectangle is valid, so we
                            //  don't have to query the row area.
                            //
                            RECT rectRow;
                            ListView_GetItemRect
                            (
                                m_hwndListView, pDraw->nmcd.dwItemSpec, &rectRow, LVIR_BOUNDS
                            );

                            TArea areaRow
                            (
                                reinterpret_cast<tCIDLib::THostRectl&>(rectRow)
                                , tCIDLib::ERectlTypes::NonInclusive
                            );

                            if (areaClient().bIntersects(areaRow))
                            {
                                TGraphWndDev gdevTmp(pDraw->nmcd.hdc, tCIDLib::EAdoptOpts::NoAdopt);
                                eCustRet = eCustomDraw
                                (
                                    gdevTmp
                                    , pDraw->nmcd.dwItemSpec
                                    , kCIDLib::c4MaxCard
                                    , kCIDLib::False
                                    , areaRow
                                    , m_rgbCustBgn
                                    , m_rgbCustText
                                );

                                // If custom colors were returned set them
                                if (tCIDLib::bAllBitsOn(eCustRet, tCIDCtrls::EMCLBCustRets::NewTextClr))
                                    pDraw->clrText = m_rgbCustText.c4Color();

                                if (tCIDLib::bAllBitsOn(eCustRet, tCIDCtrls::EMCLBCustRets::NewBgnClr))
                                    pDraw->clrTextBk = m_rgbCustBgn.c4Color();
                            }
                        }
                    }
                     else if (pDraw->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM))
                    {
                        if (tCIDLib::bAllBitsOn(m_eCustOpts, tCIDCtrls::EMCLBCustOpts::PreCol))
                        {
                            RECT rectCol;
                            ListView_GetSubItemRect
                            (
                                m_hwndListView
                                , pDraw->nmcd.dwItemSpec
                                , pDraw->iSubItem
                                , LVIR_BOUNDS
                                , &rectCol
                            );

                            TArea areaCol
                            (
                                reinterpret_cast<tCIDLib::THostRectl&>(rectCol)
                                , tCIDLib::ERectlTypes::NonInclusive
                            );

                            if (areaClient().bIntersects(areaCol))
                            {
                                TGraphWndDev gdevTmp(pDraw->nmcd.hdc, tCIDLib::EAdoptOpts::NoAdopt);
                                eCustRet = eCustomDraw
                                (
                                    gdevTmp
                                    , pDraw->nmcd.dwItemSpec
                                    , pDraw->iSubItem
                                    , kCIDLib::False
                                    , areaCol
                                    , m_rgbCustBgn
                                    , m_rgbCustText
                                );

                                // If custom colors were returned set them
                                if (tCIDLib::bAllBitsOn(eCustRet, tCIDCtrls::EMCLBCustRets::NewTextClr))
                                    pDraw->clrText = m_rgbCustText.c4Color();

                                if (tCIDLib::bAllBitsOn(eCustRet, tCIDCtrls::EMCLBCustRets::NewBgnClr))
                                    pDraw->clrTextBk = m_rgbCustBgn.c4Color();
                            }
                        }
                    }
                     else if (pDraw->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT)
                    {
                        if (tCIDLib::bAllBitsOn(m_eCustOpts, tCIDCtrls::EMCLBCustOpts::PostRow))
                        {
                            RECT rectRow;
                            ListView_GetItemRect
                            (
                                m_hwndListView, pDraw->nmcd.dwItemSpec, &rectRow, LVIR_BOUNDS
                            );

                            TArea areaRow
                            (
                                reinterpret_cast<tCIDLib::THostRectl&>(rectRow)
                                , tCIDLib::ERectlTypes::NonInclusive
                            );

                            if (areaClient().bIntersects(areaRow))
                            {
                                TGraphWndDev gdevTmp(pDraw->nmcd.hdc, tCIDLib::EAdoptOpts::NoAdopt);
                                eCustRet = eCustomDraw
                                (
                                    gdevTmp
                                    , pDraw->nmcd.dwItemSpec
                                    , kCIDLib::c4MaxCard
                                    , kCIDLib::True
                                    , areaRow
                                    , m_rgbCustBgn
                                    , m_rgbCustText
                                );
                            }
                        }
                    }
                     else if (pDraw->nmcd.dwDrawStage == (CDDS_ITEMPOSTPAINT & CDDS_SUBITEM))
                    {
                        if (tCIDLib::bAllBitsOn(m_eCustOpts, tCIDCtrls::EMCLBCustOpts::PostCol))
                        {
                            RECT rectCol;
                            ListView_GetSubItemRect
                            (
                                m_hwndListView
                                , pDraw->nmcd.dwItemSpec
                                , pDraw->iSubItem
                                , LVIR_BOUNDS
                                , &rectCol
                            );

                            TArea areaCol
                            (
                                reinterpret_cast<tCIDLib::THostRectl&>(rectCol)
                                , tCIDLib::ERectlTypes::NonInclusive
                            );

                            if (areaClient().bIntersects(areaCol))
                            {
                                TGraphWndDev gdevTmp(pDraw->nmcd.hdc, tCIDLib::EAdoptOpts::NoAdopt);
                                eCustRet = eCustomDraw
                                (
                                    gdevTmp
                                    , pDraw->nmcd.dwItemSpec
                                    , pDraw->iSubItem
                                    , kCIDLib::True
                                    , areaCol
                                    , m_rgbCustBgn
                                    , m_rgbCustText
                                );
                            }
                        }
                    }

                    // Add any other flags to the return based on our custom return
                    if (tCIDLib::bAllBitsOn(eCustRet, tCIDCtrls::EMCLBCustRets::NewFont))
                        mresResult |= CDRF_NEWFONT;

                    if (tCIDLib::bAllBitsOn(eCustRet, tCIDCtrls::EMCLBCustRets::DontDraw))
                        mresResult |= CDRF_SKIPDEFAULT;

                    return kCIDLib::True;
                }
            }
        }
         else if (pNotHdr->code == LVN_GETDISPINFO)
        {
            NMLVDISPINFO* pInfo = (NMLVDISPINFO*)lParam;

            if (pInfo->item.mask & LVIF_TEXT)
            {
                if ((tCIDLib::TCard4(pInfo->item.iItem) < m_colList.c4ElemCount())
                &&  (tCIDLib::TCard4(pInfo->item.iSubItem) < m_colColTitles.c4ElemCount()))
                {
                    pInfo->item.pszText =
                    (
                        (LPWSTR)m_colList[pInfo->item.iItem].
                                            m_colCols[pInfo->item.iSubItem].pszBuffer()
                    );
                }
            }
        }
         else if (pNotHdr->code == LVN_ITEMCHANGED)
        {
            //
            //  Checking item for >= 0 lets us avoid the scenario where it's applied to
            //  all items. We would only care about such a thing for marked state in
            //  a multi-select, and that's handled by ODSTATECHANGED.
            //
            const NMLISTVIEW* pInfo = (NMLISTVIEW*)lParam;
            if ((pInfo->iItem >= 0) && (pInfo->uChanged & LVIF_STATE))
            {
                if (bIsMultiSel())
                {
                    //
                    //  If not focused before and now focused, we send a selection
                    //  changed.
                    //
                    //  If the selected state changed, then we send a marked event
                    //  as well, so we can send both.
                    //
                    if (((pInfo->uOldState & LVIS_FOCUSED) == 0)
                    &&  (pInfo->uNewState & LVIS_FOCUSED))
                    {
                        // It has become selected
                        SendListEvent
                        (
                            tCIDCtrls::EListEvents::SelChanged
                            , pInfo->iItem
                            , m_colList[pInfo->iItem].m_c4RowId
                        );
                    }

                    if ((pInfo->uOldState & LVIS_SELECTED)
                                                != (pInfo->uNewState & LVIS_SELECTED))
                    {
                        //
                        //  This will only happen if it's a single item operation. If
                        //  multiple items are affected, we get the ODSTATECHANGED
                        //  event below.
                        //
                        SendListEvent
                        (
                            tCIDCtrls::EListEvents::Marked
                            , pInfo->iItem
                            , m_colList[pInfo->iItem].m_c4RowId
                        );
                    }
                }
                 else if (((pInfo->uOldState & LVIS_SELECTED) == 0)
                      &&  (pInfo->uNewState & LVIS_SELECTED))
                {
                    //
                    //  It's a single selection so just treat it as a movement of the
                    //  selection.
                    //
                    SendListEvent
                    (
                        tCIDCtrls::EListEvents::SelChanged
                        , pInfo->iItem
                        , m_colList[pInfo->iItem].m_c4RowId
                    );
                }
            }
            return kCIDLib::True;
        }
         else if (pNotHdr->code == LVN_ODCACHEHINT)
        {
            // Doesn't matter. We always have all data available
            mresResult = 0;
            return kCIDLib::True;
        }
         else if (pNotHdr->code == LVN_ODFINDITEM)
        {
            NMLVFINDITEM* pNot = (NMLVFINDITEM*)lParam;

            tCIDCtrls::EFindCtrlData eFind = tCIDCtrls::EFindCtrlData::None;

            // Get a copy of the flags and get the wrap flag out
            tCIDLib::TCard4 c4Flags = pNot->lvfi.flags;
            tCIDLib::TBoolean bWrap = (c4Flags & LVFI_WRAP) != 0;

            // Now mask off anything but the modes we support
            c4Flags &= (LVFI_STRING | LVFI_PARTIAL | LVFI_SUBSTRING);

            // And set our find flag base don the remaining bits
            if (c4Flags == (LVFI_STRING | LVFI_PARTIAL))
                eFind = tCIDCtrls::EFindCtrlData::PrefixText;
            else if (c4Flags == (LVFI_STRING | LVFI_SUBSTRING))
                eFind = tCIDCtrls::EFindCtrlData::PrefixText;
            else if (c4Flags == LVFI_STRING)
                eFind = tCIDCtrls::EFindCtrlData::Text;

            // If it's a mode we support do it, else always return -1 for no match
            mresResult = -1;
            if (eFind != tCIDCtrls::EFindCtrlData::None)
            {
                TString strFind(pNot->lvfi.psz);

                //
                //  Else we will handle it. Not that we can get element count as
                //  the starting index. It's not smart enough to pass us 0 as
                //  the start if the cursor is on the last item.
                //
                tCIDLib::TCard4 c4EndIndex = m_colList.c4ElemCount();
                tCIDLib::TCard4 c4Index = tCIDLib::TCard4(pNot->iStart);
                if (c4Index == c4EndIndex)
                    c4Index = 0;

                while (c4Index < c4EndIndex)
                {
                    const TString& strCur = m_colList[c4Index].m_colCols[m_c4SearchCol];
                    if (eFind == tCIDCtrls::EFindCtrlData::PrefixText)
                    {
                        if (strCur.bStartsWithI(strFind))
                        {
                            mresResult = c4Index;
                            break;
                        }
                    }
                     else
                    {
                        if (strCur.bCompareI(strFind))
                        {
                            mresResult = c4Index;
                            break;
                        }
                    }

                    // Handle wrap if needed
                    if ((c4Index + 1 == c4EndIndex) && bWrap)
                    {
                        //
                        //  Make the end index the original start and zero the
                        //  index. If we started at zero, then we'll just fall
                        //  out at the top of the loop.
                        //
                        c4EndIndex = tCIDLib::TCard4(pNot->iStart);
                        c4Index = 0;

                        // Make sure we don't wrap again
                        bWrap = kCIDLib::False;
                    }
                     else
                    {
                        c4Index++;
                    }
                }
            }
            return kCIDLib::True;
        }
         else if (pNotHdr->code == LVN_ODSTATECHANGED)
        {
            //
            //  If multi-select and items have been marked/unmarked, then we send a
            //  marked notification with max card for index and id.
            //
            NMLVODSTATECHANGE* pNot = (NMLVODSTATECHANGE*)lParam;

            if (bIsMultiSel()
            && ((pNot->uOldState & LVIS_SELECTED) != (pNot->uNewState & LVIS_SELECTED)))
            {
                SendListEvent
                (
                    tCIDCtrls::EListEvents::Marked, kCIDLib::c4MaxCard, kCIDLib::c4MaxCard
                );
            }
            return kCIDLib::True;
        }
         else if (pNotHdr->code == LVN_KEYDOWN)
        {
            //
            //  Watch for \ and / keys to implement mark all and unmark all, only
            //  valid for multi-select mode. Only if unshifted.
            //
            if (bIsMultiSel())
            {
                NMLVKEYDOWN* pKey = (NMLVKEYDOWN*)lParam;

                if ((pKey->wVKey == VK_OEM_2)
                ||  (pKey->wVKey == VK_OEM_5))
                {
                    MarkAll(pKey->wVKey == VK_OEM_2);

                    // This won't cause any notifications, we we send one out manually
                    SendListEvent
                    (
                        tCIDCtrls::EListEvents::Marked
                        , kCIDLib::c4MaxCard
                        , kCIDLib::c4MaxCard
                    );
                    return kCIDLib::True;
                }
            }
        }
    }
    return kCIDLib::False;
}


//
//  The only part of us not covered by the listview is potentially a border. But that's
//  drawn for us, so we just want to suppress any painting.
//
tCIDLib::TBoolean TMultiColListBox::bPaint(TGraphDrawDev&, const TArea&)
{
    return kCIDLib::True;
}


tCIDLib::TVoid TMultiColListBox::Destroyed()
{
    //
    //  Make sure no trailing messages try to access our window object in our subclass
    //  handler above.
    //
    ::SetWindowLongPtr(m_hwndListView, GWL_USERDATA, (LONG_PTR)0);

    // And then pass it on
    TParent::Destroyed();
}


// Make sure we don't get the focus, pass it on to the list view
tCIDLib::TVoid TMultiColListBox::GettingFocus()
{
    if (m_hwndListView)
        ::SetFocus(m_hwndListView);
}


// ---------------------------------------------------------------------------
// TMultiColListBox: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If the user left clicks in a column, and there's an auto IPE handler installed,
//  then we kick off the in place edit.
//
tCIDLib::TVoid
TMultiColListBox::AutoIPE(  const   tCIDLib::TCard4 c4Row
                            , const tCIDLib::TCard4 c4Col)
{
    //
    //  Store the row info on the IPE object so it will have access to the target
    //  row when called back through the IPE's validation callback method.
    //
    m_pipeiAuto->c4IPERow(c4Row);

    //
    //  First see if the handler allows editing of this cell. If not, then there's
    //  nothing to do. If so, it'll fill in the key and title of the attribute
    //  object. Then we can use the existing attribute validation stuff to validate
    //  the entered data.
    //
    tCIDLib::TBoolean bValueSet = kCIDLib::False;
    TAttrData adatEdit;
    if (!m_pipeiAuto->bIPETestCanEdit(widThis(), c4Col, adatEdit, bValueSet))
        return;

    //
    //  If they didn't set a value on the attribute, we'll do it generically by just
    //  getting the value of the cell (which means they will have to have set the
    //  type to string.
    //
    if (!bValueSet)
    {
        TString strEdit;
        QueryColText(c4Row, c4Col, strEdit);
        adatEdit.SetString(strEdit);
    }

    //
    //  Get the area of this cell, but clip it to the width of our client area, so that
    //  we don't end up with some hugely long window.
    //
    TArea areaCol;
    QueryColArea(c4Row, c4Col, areaCol);

    TArea areaAvail = areaClient();
    if (areaCol.i4Right() >= areaAvail.i4Right())
    {
        // If there's too little space to do anything useful, give up
        if ((areaAvail.i4Right() - areaCol.i4Left()) < 24)
            return;

        areaCol.i4Right(areaAvail.i4Right() - 4);
    }

    // Save the original attribute data so we can pass it later as a comparison value
    const TAttrData adatOrg(adatEdit);

    //
    //  At this point, we special case a couple things. If it's a boolean, or an
    //  enum that has a reasonable number of values, instead of doing an in place
    //  edit, we do a little drop down.
    //
    tCIDLib::TBoolean bRes = kCIDLib::False;

    if ((adatEdit.eType() == tCIDMData::EAttrTypes::Bool)
    ||  ((adatEdit.eType() == tCIDMData::EAttrTypes::String)
    &&   adatEdit.strLimits().bStartsWithI(kCIDMData::strAttrLim_EnumPref)))
    {
        TArea areaColScr;
        ToScreenCoordinates(areaCol, areaColScr);

        tCIDLib::TStrList colEnumVals;
        tCIDLib::TCard4 c4Sel = 0;
        TPopupMenu menuSelect(L"Ad Hoc IPE Menu");
        menuSelect.Create();
        if (adatEdit.eType() == tCIDMData::EAttrTypes::Bool)
        {
            menuSelect.AddActionItem(kCIDLib::c4MaxCard, L"False", L"False", 1000);
            menuSelect.AddActionItem(kCIDLib::c4MaxCard, L"True", L"True", 1001);
        }
         else
        {
            // Break out the limit values into the menu
            TString strEnum = adatEdit.strLimits();
            strEnum.Cut(0, kCIDMData::strAttrLim_EnumPref.c4Length());

            TStringTokenizer stokVals(&strEnum, L",");
            TString strToken;
            tCIDLib::TCard4 c4ResId = 1;
            while (stokVals.bGetNextToken(strToken))
            {
                strToken.StripWhitespace();
                menuSelect.AddActionItem
                (
                    kCIDLib::c4MaxCard, L"", strToken, c4ResId++
                );
            }
        }

        // Get a selection from the user now
        c4Sel = menuSelect.c4Process
        (
            *this, areaColScr.pntLL(), tCIDLib::EVJustify::Top, tCIDLib::EHJustify::Left
        );

        // If they chose something, then proces it
        if (c4Sel)
        {
            TString strSelVal;
            menuSelect.QueryItemText(c4Sel, strSelVal);

            // Update the column text with the selected value
            SetColText(c4Row, c4Col, strSelVal);

            // Update the attribute since we have to pass it to a callback
            if (adatEdit.eType() == tCIDMData::EAttrTypes::Bool)
            {
                const tCIDLib::TBoolean bNew(c4Sel == 1001);
                adatEdit.SetBool(bNew);
            }
             else
            {
                adatEdit.SetString(strSelVal);
            }

            // And do the value change callback so the program can store it if desired
            m_pipeiAuto->IPEValChanged(widThis(), strName(), adatEdit, adatOrg, 0);
            bRes = kCIDLib::True;
        }
    }
     else
    {
        //
        //  Invoke the in place editor. Note that our auto edit mixin derives from
        //  the IPE's, so we can pass it to him for validation of the edited value.
        //  We already set the row as the IPE row so he can know which row is being
        //  processed.
        //
        TInPlaceEdit ipeVal;
        bRes = ipeVal.bProcess(*this, areaCol, adatEdit, *m_pipeiAuto, 0);
    }

    //
    //  If it was validated, then it will have been stored back to the client, from
    //  the bProcess method above. So we need to update our column data.
    //
    if (bRes)
    {
        TString strFmtVal;
        adatEdit.FormatToText(strFmtVal);
        SetColText(c4Row, c4Col, strFmtVal);
    }
}


//
//  Public methods call this to make sure an incoming row index is valid. If not
//  we throw.
//
tCIDLib::TVoid
TMultiColListBox::CheckRowIndex(const   tCIDLib::TCard4 c4ToCheck
                                , const tCIDLib::TCard4 c4Line) const
{
    const tCIDLib::TCard4 c4Count = c4ItemCount();
    if (c4ToCheck >= c4Count)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_BadIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c4ToCheck)
            , TCardinal(widThis())
            , TCardinal(c4Count)
        );
    }
}


//
//  Load up the columns
//
tCIDLib::TVoid TMultiColListBox::LoadColumns()
{
    // Remove all existing items and columns
    RemoveColumns();

    // Now we can add the new ones in
    const tCIDLib::TCard4 c4Count = m_colColTitles.c4ElemCount();
    LVCOLUMN lvcCur = {0};

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        lvcCur.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_MINWIDTH;
        lvcCur.fmt = LVCFMT_LEFT;
        lvcCur.cxMin = 16;
        lvcCur.iSubItem = c4Index;
        lvcCur.pszText = (LPWSTR)m_colColTitles[c4Index].pszBuffer();

        //
        //  Only do this if not auto-sizing this column. We'll just set a basic
        //  default width for now.
        //
        if (!tCIDLib::bAllBitsOn(m_eMCLBStyles, tCIDCtrls::EMCLBStyles::AutoLastCol)
        ||  (c4Index != c4Count - 1))
        {
            lvcCur.mask |= LVCF_WIDTH;
            lvcCur.cx = 100;
        }

        // Insert the columns into the list view.
        if (SendMessage(m_hwndListView, LVM_INSERTCOLUMN, c4Index, tCIDCtrls::TLParam(&lvcCur)) == -1)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcWnd_AddColumn
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TCardinal(widThis())
            );
        }
    }

    // Let derived classes react to this
    ColumnsSet(c4Count);

    // Make sure the last one is updated if if it marked auto-size
    UpdateAutoLast();
}


//
//  Removes all columns. It first removes all items, since it would be messy to try
//  to keep the items while changing the columns.
//
tCIDLib::TVoid TMultiColListBox::RemoveColumns()
{
    // Remove any content
    RemoveAll();

    //
    //  We have to just delete column 0 until we get an error, though we do set
    //  a maximum, just in case.
    //
    int iCol = 0;
    while (iCol < 256)
    {
        if (!ListView_DeleteColumn(m_hwndListView, 0))
            break;
        iCol++;
    }
}


//
//  Resort the list by the sort column, either ascending or descending depending
//  on what we are set up for.
//
tCIDLib::TVoid TMultiColListBox::SortList()
{
    m_colList.Sort
    (
        [this](const TItemData& id1, const TItemData& id2)
        {
          const tCIDLib::TCard4 c4ColInd = this->m_c4SortCol;
          if (this->m_bSortAsc)
          {
              return this->eCompColVals
              (
                  id1.m_colCols[c4ColInd], id2.m_colCols[c4ColInd], c4ColInd
              );
          }
           else
          {
              return this->eCompColVals
              (
                  id2.m_colCols[c4ColInd], id1.m_colCols[c4ColInd], c4ColInd
              );
          }
        }
    );
}


//
//  Called any timed something is done which might affect the available horizontal
//  space. We check to see if the 'auto last' style is set. If so, then we reset it
//  to make the last column re-adjust to fit the available area.
//
tCIDLib::TVoid TMultiColListBox::UpdateAutoLast()
{
    const tCIDLib::TCard4 c4Count = m_colColTitles.c4ElemCount();
    if (c4Count && tCIDLib::bAllBitsOn(m_eMCLBStyles, tCIDCtrls::EMCLBStyles::AutoLastCol))
        ::SendMessage(m_hwndListView, LVM_SETCOLUMNWIDTH, c4Count - 1, LVSCW_AUTOSIZE_USEHEADER);
}
