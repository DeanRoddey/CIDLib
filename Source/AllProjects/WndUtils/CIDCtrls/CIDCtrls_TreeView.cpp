//
// FILE NAME: CIDCtrls_TreeView.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/12/2015
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
//  This file implements the single line static text control.
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
RTTIDecls(TTreeEventInfo, TCtrlNotify)
AdvRTTIDecls(TTreeView, TStdCtrlWnd)


// ---------------------------------------------------------------------------
//  CLASS: TTreeMapItem
// PREFIX: tmi
// ---------------------------------------------------------------------------
class TTreeMapItem
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::ESortComps eHandleKeyComp
        (
            const   tCIDLib::TSInt&         hItem
            , const TTreeMapItem&           tmiComp
        );

        static tCIDLib::ESortComps eHandleComp
        (
            const   TTreeMapItem&           tmi1
            , const TTreeMapItem&           tmi2
        );

        static tCIDLib::ESortComps ePathComp
        (
            const   TTreeMapItem&           tmi1
            , const TTreeMapItem&           tmi2
        );

        static const TString& strKey
        (
            const   TTreeMapItem&           tmiSrc
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTreeMapItem
        (
            const   TString&                strParPath
            , const TString&                strName
            ,       tCIDLib::TSInt          hItem
            , const tCIDLib::TBoolean       bIsScope
        );

        TTreeMapItem(const TTreeMapItem&) = delete;

        ~TTreeMapItem();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTreeMapItem& operator=(const TTreeMapItem&) = delete;


        // -------------------------------------------------------------------
        //  Public class members
        //
        //  m_bVirtual
        //      This is a scope that was added with the virtual flag. when we get a
        //      TVN_GETDISPINFO call, and this is set, we say there are children
        //      so it'll show the + sign, but they don't actually get added until
        //      the containing program handles an expansion notification and loads
        //      them up. Once this scope is actually loaded, this flag is cleared.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bRoot;
        tCIDLib::TBoolean   m_bScope;
        tCIDLib::TBoolean   m_bVirtual;
        tCIDLib::TCard4     m_c4UserFlag;
        tCIDLib::TSInt      m_hItem;
        TObject*            m_pobjUser;
        TString             m_strPath;
};




// ---------------------------------------------------------------------------
//   CLASS: TTreeEventInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTreeEventInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TTreeEventInfo::TTreeEventInfo( const   tCIDCtrls::ETreeEvents  eEvent
                                , const TString&                strPath
                                , const TWindow&                wndSrc) :
    TCtrlNotify(wndSrc)
    , m_bState(kCIDLib::False)
    , m_c4Action(kCIDLib::c4MaxCard)
    , m_eEvent(eEvent)
    , m_strPath(strPath)
{
}

TTreeEventInfo::TTreeEventInfo( const tCIDCtrls::ETreeEvents    eEvent
                                , const TString&                strPath
                                , const tCIDLib::TCard4         c4Action
                                , const TWindow&                wndSrc
                                , const tCIDLib::TBoolean       bState) :
    TCtrlNotify(wndSrc)
    , m_bState(bState)
    , m_c4Action(c4Action)
    , m_eEvent(eEvent)
    , m_strPath(strPath)
{
}

TTreeEventInfo::TTreeEventInfo( const tCIDCtrls::ETreeEvents    eEvent
                                , const TString&                strNewPath
                                , const TString&                strOldPath
                                , const TWindow&                wndSrc) :
    TCtrlNotify(wndSrc)
    , m_bState(kCIDLib::False)
    , m_c4Action(0)
    , m_eEvent(eEvent)
    , m_strOldPath(strOldPath)
    , m_strPath(strNewPath)
{
}

TTreeEventInfo::TTreeEventInfo(const TTreeEventInfo& wnotSrc) :

    TCtrlNotify(wnotSrc)
    , m_bState(wnotSrc.m_bState)
    , m_c4Action(wnotSrc.m_c4Action)
    , m_eEvent(wnotSrc.m_eEvent)
    , m_strOldPath(wnotSrc.m_strOldPath)
    , m_strPath(wnotSrc.m_strPath)
{
}

TTreeEventInfo::~TTreeEventInfo()
{
}


// ---------------------------------------------------------------------------
//  TTreeEventInfo: Public operators
// ---------------------------------------------------------------------------
TTreeEventInfo& TTreeEventInfo::operator=(const TTreeEventInfo& wnotSrc)
{
    if (this != &wnotSrc)
    {
        TParent::operator=(wnotSrc);
        m_bState    = wnotSrc.m_bState;
        m_c4Action  = wnotSrc.m_c4Action;
        m_eEvent    = wnotSrc.m_eEvent;
        m_strOldPath= wnotSrc.m_strOldPath;
        m_strPath   = wnotSrc.m_strPath;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TTreeEventInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TTreeEventInfo::bState() const
{
    return m_bState;
}


tCIDLib::TCard4 TTreeEventInfo::c4Action() const
{
    return m_c4Action;
}


tCIDCtrls::ETreeEvents TTreeEventInfo::eEvent() const
{
    return m_eEvent;
}


const TString& TTreeEventInfo::strOldPath() const
{
    return m_strOldPath;
}


const TString& TTreeEventInfo::strPath() const
{
    return m_strPath;
}




// ---------------------------------------------------------------------------
//  CLASS: TTreeMapItem
// PREFIX: tmi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTreeMapItem: Public static methods
// ---------------------------------------------------------------------------

tCIDLib::ESortComps
TTreeMapItem::eHandleKeyComp(const  tCIDLib::TSInt& hItem
                            , const TTreeMapItem&   tmi2)
{
    if (hItem < tmi2.m_hItem)
        return tCIDLib::ESortComps::FirstLess;

    if (hItem > tmi2.m_hItem)
        return tCIDLib::ESortComps::FirstGreater;

    return tCIDLib::ESortComps::Equal;
}

tCIDLib::ESortComps
TTreeMapItem::ePathComp(const   TTreeMapItem&   tmi1
                        , const TTreeMapItem&   tmi2)
{
    return tmi1.m_strPath.eCompareI(tmi2.m_strPath);
}

tCIDLib::ESortComps
TTreeMapItem::eHandleComp(  const   TTreeMapItem& tmi1
                            , const TTreeMapItem& tmi2)
{
    if (tmi1.m_hItem < tmi2.m_hItem)
        return tCIDLib::ESortComps::FirstLess;

    if (tmi1.m_hItem > tmi2.m_hItem)
        return tCIDLib::ESortComps::FirstGreater;

    return tCIDLib::ESortComps::Equal;
}

const TString& TTreeMapItem::strKey(const TTreeMapItem& tmiSrc)
{
    return tmiSrc.m_strPath;
}


// ---------------------------------------------------------------------------
//  TTreeMapItem: Public static methods
// ---------------------------------------------------------------------------
TTreeMapItem::TTreeMapItem( const   TString&            strParPath
                            , const TString&            strName
                            ,       tCIDLib::TSInt      hItem
                            , const tCIDLib::TBoolean   bIsScope) :
    m_bRoot(kCIDLib::False)
    , m_bScope(bIsScope)
    , m_bVirtual(kCIDLib::False)
    , m_c4UserFlag(0)
    , m_hItem(hItem)
    , m_pobjUser(nullptr)
    , m_strPath(strParPath)
{
    if ((m_strPath.chLast() != kCIDLib::chForwardSlash)
    &&  (strName.chFirst() != kCIDLib::chForwardSlash))
    {
        m_strPath.Append(kCIDLib::chForwardSlash);
    }
    m_strPath.Append(strName);
}

TTreeMapItem::~TTreeMapItem()
{
    if (m_pobjUser)
    {
        try
        {
            delete m_pobjUser;
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        catch(...)
        {
        }

        m_pobjUser = nullptr;
    }
}





// ---------------------------------------------------------------------------
//  CLASS: TTreeView
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TTreeView: Public, static method
// ---------------------------------------------------------------------------
const TNotificationId    TTreeView::nidTreeEvent(L"TreeViewEvent");


// ---------------------------------------------------------------------------
// TTreeView: Constructors and Destructor
// ---------------------------------------------------------------------------
TTreeView::TTreeView() :

    TStdCtrlWnd()
    , m_eCustOpts(tCIDCtrls::ETreeCustOpts::None)
    , m_eTreeStyles(tCIDCtrls::ETreeStyles::None)
    , m_pcolAltMap(nullptr)
    , m_pcolMap(nullptr)
{
    m_pcolAltMap = new TItemAltMap(tCIDLib::EAdoptOpts::NoAdopt);
    m_pcolMap = new TItemMap
    (
        tCIDLib::EAdoptOpts::Adopt
        , 109
        , TStringKeyOps(kCIDLib::False)
        , &TTreeMapItem::strKey
    );
}

TTreeView::~TTreeView()
{
    // Clean up the collections
    delete m_pcolAltMap;

    if (m_pcolMap)
    {
        try
        {
            m_pcolMap->RemoveAll();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        delete m_pcolMap;
    }
}


// ---------------------------------------------------------------------------
// TStaticImg: Public, inherited methods
// ---------------------------------------------------------------------------

// To support dialog driven creation
tCIDLib::TVoid
TTreeView::InitFromDesc(const   TWindow&                wndParent
                        , const TDlgItem&               dlgiSrc
                        , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::ETreeStyles eTreeStyles = tCIDCtrls::ETreeStyles::HasLines;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild
    );
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_CheckBox))
        eTreeStyles |= tCIDCtrls::ETreeStyles::CheckBoxes;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Group))
        eStyles |= tCIDCtrls::EWndStyles::Group;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Invoke))
        eTreeStyles |= tCIDCtrls::ETreeStyles::InvokeOnEnter;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_ReadOnly))
        eTreeStyles |= tCIDCtrls::ETreeStyles::ReadOnly;

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

    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , eStyles
        , eExStyles
        , eTreeStyles
    );
}


tCIDLib::TVoid TTreeView::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_CheckBox);
    colToFill.objAdd(kCIDCtrls::strHint_Group);
    colToFill.objAdd(kCIDCtrls::strHint_Invoke);
    colToFill.objAdd(kCIDCtrls::strHint_NoBorder);
    colToFill.objAdd(kCIDCtrls::strHint_ReadOnly);
}




// ---------------------------------------------------------------------------
// TTreeView: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We find the node that has the indicated parent id, and add a new child under
//  it, either an item or a scope.
//
tCIDLib::TVoid
TTreeView::AddItem( const   TString&        strParPath
                    , const TString&        strText
                    , const tCIDLib::TCard4 c4UserFlag)
{
    // Get the parent
    tCIDLib::TCard4 c4At;
    TTreeMapItem* ptmiPar = ptmiFromPath(strParPath, kCIDLib::True, c4At);

    // And now add a a first value
    TVINSERTSTRUCTW Insert = {0};

    Insert.hParent = (HTREEITEM)ptmiPar->m_hItem;
    Insert.hInsertAfter = TVI_SORT;
    Insert.itemex.mask = TVIF_TEXT;
    Insert.itemex.pszText = (LPWSTR)strText.pszBuffer();
    Insert.itemex.iIntegral = 1;
    tCIDCtrls::TMsgResult mresHandle = ::SendMessage
    (
        hwndThis(), TVM_INSERTITEM, 0, tCIDCtrls::TLParam(&Insert)
    );

    if (!mresHandle)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_InsertItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"TreeView")
            , TCardinal(widThis())
        );
    }

    // Add an entry to the map for this one
    ptmiAddMapItem
    (
        strParPath
        , strText
        , tCIDLib::TSInt(mresHandle)
        , kCIDLib::False
        , kCIDLib::False
        , c4UserFlag
    );

    //
    //  Update the parent, so that the +/- sign shows up if this is the first child.
    //  We know at this point that it has to be '1 or more' state.
    //
    TVITEM Item = {0};
    Item.hItem = (HTREEITEM)ptmiPar->m_hItem;
    Item.mask = TVIF_CHILDREN | TVIF_HANDLE;
    Item.cChildren = 1;
    ::SendMessage(hwndThis(), TVM_SETITEM, 0, tCIDCtrls::TLParam(&Item));

    //
    //  Also, force the expanded once flag on so it won't later try to fault this
    //  scope in after we've added stuff to it.
    //
    ForceExpandedOnce(strParPath);
}

tCIDLib::TVoid
TTreeView::AddScope(const   TString&            strParPath
                    , const TString&            strText
                    , const tCIDLib::TBoolean   bVirtual)
{
    tCIDLib::TCard4 c4At;
    TTreeMapItem* ptmiPar = ptmiFromPath(strParPath, kCIDLib::True, c4At);

    TVINSERTSTRUCTW Insert = {0};

    Insert.hParent = (HTREEITEM)ptmiPar->m_hItem;
    Insert.hInsertAfter = TVI_SORT;
    Insert.itemex.mask = TVIF_TEXT;
    Insert.itemex.pszText = (LPWSTR)strText.pszBuffer();
    Insert.itemex.iIntegral = 1;

    // If virtual, then set the child count to indicte we don't know yet
    if (bVirtual)
    {
        Insert.itemex.mask |= TVIF_CHILDREN;
        Insert.itemex.cChildren = I_CHILDRENCALLBACK;
    }

    tCIDCtrls::TMsgResult mresHandle = ::SendMessage
    (
        hwndSafe(), TVM_INSERTITEM, 0, tCIDCtrls::TLParam(&Insert)
    );

    if (!mresHandle)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_InsertItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"TreeView")
            , TCardinal(widThis())
        );
    }

    // Add an entry to the map for this one
    ptmiAddMapItem
    (
        strParPath
        , strText
        , tCIDLib::TSInt(mresHandle)
        , kCIDLib::True
        , bVirtual
        , kCIDLib::False
    );

    //
    //  Update the parent, so that the +/- sign shows up if this is the first child.
    //  We know at this point that it has to be '1 or more' state.
    //
    TVITEM Item = {0};
    Item.hItem = (HTREEITEM)ptmiPar->m_hItem;
    Item.mask = TVIF_CHILDREN | TVIF_HANDLE;
    Item.cChildren = 1;
    ::SendMessage(hwndThis(), TVM_SETITEM, 0, tCIDCtrls::TLParam(&Item));

    //
    //  Also, force the expanded once flag on so it won't later try to fault this
    //  scope in after we've added stuff to it.
    //
    ForceExpandedOnce(strParPath);
}


//
//  When used in a file open type of capacity, they typically want to initially
//  select the previous value (if any.) We return the path that we could actually
//  expand out to.
//
tCIDLib::TBoolean
TTreeView::bExpandPath(const TString& strPath, TString& strGotToPath)
{
    tCIDLib::TBoolean bRet(kCIDLib::True);

    //
    //  The path may be the root, in whiuch case we need to specal case it. Else see how
    //  far we can get into the tree.
    //
    if (strPath == L"/")
    {
        SetExpandState(strPath, kCIDLib::True);
        return kCIDLib::True;
    }

    //
    //  Tokenize the path on forward slashes. These are the path components that
    //  need to be expanded.
    //
    TStringTokenizer stokPath(&strPath, L"/");
    TString strToken;
    strGotToPath.Clear();
    TString strTestPath;

    while (stokPath.bGetNextToken(strToken))
    {
        strTestPath.Append(kCIDLib::chForwardSlash);
        strTestPath.Append(strToken);

        //
        //  Make sure it's expanded. This could cause an exception if the path is
        //  bad because sometimes it's faulted in by derived code. So we catch the
        //  exception and we'll stop at this point.
        //
        //  Check that it's a scope first of course. We may get a final, trailing
        //  value.
        //
        try
        {
            if (bIsScope(strTestPath))
                SetExpandState(strTestPath, kCIDLib::True);

            // It worked, so add this to the return path
            strGotToPath.Append(kCIDLib::chForwardSlash);
            strGotToPath.Append(strToken);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
            bRet = kCIDLib::False;
            break;
        }
    }

    //
    //  For how far we got done, find that one and make sure it's selected
    //  and visible. The above code will insure it's been expanded.
    //
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiCheck = ptmiFromPath(strGotToPath, kCIDLib::True, c4At);
    TreeView_EnsureVisible(hwndSafe(), ptmiCheck->m_hItem);
    BOOL bRes = ::SendMessage
    (
        hwndThis()
        , TVM_SELECTITEM
        , TVGN_CARET
        , tCIDCtrls::TLParam(ptmiCheck->m_hItem)
    );
    return bRet;
}


// Return whether the indicated item has been expanded at least once so far
tCIDLib::TBoolean TTreeView::bHasBeenExpanded(const TString& strPath) const
{
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiCheck = ptmiFromPath(strPath, kCIDLib::True, c4At);

    // Looks like we have it, so query the item state
    const tCIDCtrls::TMsgResult mresRes = ::SendMessage
    (
        hwndSafe()
        , TVM_GETITEMSTATE
        , tCIDCtrls::TWParam(ptmiCheck->m_hItem)
        , tCIDCtrls::TLParam(TVIS_EXPANDEDONCE)
    );

    // If the expanded once flag is set, then return true
    return (mresRes & TVIS_EXPANDEDONCE) != 0;
}


// Returns true if the passed item is checked
tCIDLib::TBoolean TTreeView::bIsChecked(const TString& strPath) const
{
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiCheck = ptmiFromPath(strPath, kCIDLib::True, c4At);

    const tCIDCtrls::TMsgResult mresRes = ::SendMessage
    (
        hwndSafe()
        , TVM_GETITEMSTATE
        , tCIDCtrls::TWParam(ptmiCheck->m_hItem)
        , tCIDCtrls::TLParam(TVIS_STATEIMAGEMASK)
    );

    // If the state image index is 2, then it's checked
    return ((mresRes >> 12) == 2);
}


// Returns true if the passed scope is expanded. Doesn't apply to items, only scopes
tCIDLib::TBoolean TTreeView::bIsExpanded(const TString& strPath) const
{
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiCheck = ptmiFromPath(strPath, kCIDLib::True, c4At);

    // Make sure it's a scope
    CheckIsScope(*ptmiCheck, CID_LINE);

    const tCIDCtrls::TMsgResult mresRes = ::SendMessage
    (
        hwndSafe()
        , TVM_GETITEMSTATE
        , tCIDCtrls::TWParam(ptmiCheck->m_hItem)
        , tCIDCtrls::TLParam(TVIS_EXPANDED)
    );

    return (mresRes & TVIS_EXPANDED) != 0;
}


// Returns true if the passed id is for the root item
tCIDLib::TBoolean TTreeView::bIsRoot(const TString& strPath) const
{
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiCheck = ptmiFromPath(strPath, kCIDLib::True, c4At);
    return ptmiCheck->m_bRoot;
}


// Returns true if the path is for a scope
tCIDLib::TBoolean TTreeView::bIsScope(const TString& strPath) const
{
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiCheck = ptmiFromPath(strPath, kCIDLib::True, c4At);
    return ptmiCheck->m_bScope;
}


// We see if there's an item under the point and get the path if so
tCIDLib::TBoolean TTreeView::bItemAt(const TPoint& pntAt, TString& strPath) const
{
    TVHITTESTINFO Info = {0};
    Info.pt.x = pntAt.i4X();
    Info.pt.y = pntAt.i4Y();

    tCIDCtrls::TMsgResult mresHit = ::SendMessage
    (
        hwndSafe(), TVM_HITTEST, 0, tCIDCtrls::TLParam(&Info)
    );

    // If we hit something, let's process it
    if (!mresHit)
        return kCIDLib::False;

    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiTar = ptmiFromHandle(mresHit, kCIDLib::True, c4At);
    if (!ptmiTar)
        return kCIDLib::False;

    strPath = ptmiTar->m_strPath;
    return kCIDLib::True;
}


// Check to see if the passed path is in the tree
tCIDLib::TBoolean TTreeView::bPathExists(const TString& strToCheck) const
{
    tCIDLib::TCard4 c4At;
    return (ptmiFromPath(strToCheck, kCIDLib::False, c4At) != nullptr);
}


// Find the indicated item and return its area if found
tCIDLib::TBoolean
TTreeView::bQueryItemArea(  const TString&              strPath
                            ,       TArea&              areaToFill
                            , const tCIDLib::TBoolean   bJustText
                            , const tCIDLib::TBoolean   bThrowIfNot) const
{
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiQ = ptmiFromPath(strPath, bThrowIfNot, c4At);
    if (!ptmiQ)
        return kCIDLib::False;

    // Get the area of the item clicked
    RECT rcDummy;
    *(HTREEITEM*)&rcDummy = (HTREEITEM)ptmiQ->m_hItem;
    const BOOL bFullWidth(bJustText ? FALSE : TRUE);
    if (!::SendMessage(hwndSafe(), TVM_GETITEMRECT, bFullWidth, tCIDCtrls::TLParam(&rcDummy)))
        return kCIDLib::False;

    areaToFill.FromRectl(*reinterpret_cast<tCIDLib::THostRectl*>(&rcDummy));
    return kCIDLib::True;
}


//
//  Get the selelected item and return its path, return false if there is no item
//  with the cursor.
//
tCIDLib::TBoolean TTreeView::bSelectedItem(TString& strToFill) const
{
    tCIDCtrls::TMsgResult mresIndex = ::SendMessage
    (
        hwndSafe(), TVM_GETNEXTITEM, TVGN_CARET, 0
    );

    if (!mresIndex)
        return kCIDLib::False;

    // We got the handle
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiSel = ptmiFromHandle(mresIndex, kCIDLib::True, c4At);
    strToFill = ptmiSel->m_strPath;
    return kCIDLib::True;
}

//
//  Given a path, start working up the path until we find a component that exists,
//  then select that component. If we end up back at the root, we don't bother.
//
tCIDLib::TBoolean TTreeView::bSelectBestTry(const TString& strPath)
{
    // Get a copy of the path. If it's empty, do nothing
    TString strCur = strPath;
    if (strCur.bIsEmpty())
        return kCIDLib::False;

    // Make sure it doesn't end with a slash
    if (strCur.chLast() == kCIDLib::chForwardSlash)
        strCur.DeleteLast();

    // Now let's loop until we find one that exists or give up
    while (strCur.c4Length() > 1)
    {
        tCIDLib::TCard4 c4At;
        const TTreeMapItem* ptmiCur = ptmiFromPath(strCur, kCIDLib::False, c4At);
        if (ptmiCur)
        {
            ::SendMessage
            (
                hwndSafe()
                , TVM_SELECTITEM
                , TVGN_CARET
                , tCIDCtrls::TLParam(ptmiCur->m_hItem)
            );
            return kCIDLib::True;
        }

        //
        //  Didn't find it, so move back to the previous layer in the path. We
        //  find the last slash and cap it there. If we don't find one, something is
        //  wrong so just give up.
        //
        if (!strCur.bLastOccurrence(kCIDLib::chForwardSlash, c4At))
            break;

        strCur.CapAt(c4At);
    }

    // Never found a good one
    return kCIDLib::False;
}


//
//  Returns a list of the paths of all of the checked items in the whole tree. So we need to
//  do a recursive iteration. We just use a stack of item handles to fake the recursion.
//  We are only returning items, not scopes. We also return the user flags for the ones we
//  give back, since that will often be the most efficient for the caller.
//
tCIDLib::TCard4
TTreeView::c4QueryAllChecked(tCIDLib::TStrCollect&  colToFill
                            , tCIDLib::TCardList&   fcolFlags) const
{
    const tCIDCtrls::TWndHandle hwndUs = hwndSafe();
    colToFill.RemoveAll();
    fcolFlags.RemoveAll();

    // Create a stack for our recursion
    TFundStack<tCIDLib::TSInt> fcolStack(1024);

    // Get the root item
    HTREEITEM hCur = (HTREEITEM)::SendMessage(hwndUs, TVM_GETNEXTITEM, TVGN_ROOT, 0);
    while (hCur)
    {
        tCIDLib::TCard4 c4At;
        const TTreeMapItem* ptmiCur = ptmiFromHandle(tCIDLib::TSInt(hCur), kCIDLib::True, c4At);

        if (ptmiCur->m_bScope)
        {
            // It's a scoe, so see if this guy has any children
            HTREEITEM hSub = (HTREEITEM)::SendMessage
            (
                hwndUs, TVM_GETNEXTITEM, TVGN_CHILD, tCIDCtrls::TLParam(hCur)
            );

            // And get the next child of the current one
            HTREEITEM hNext = (HTREEITEM)
            (
                ::SendMessage(hwndUs, TVM_GETNEXTITEM, TVGN_NEXT, tCIDCtrls::TLParam(hCur))
            );

            if (hSub)
            {
                //
                //  There are children, so push the next child of the current one, if it
                //  is non-null, and make the first new child the current.
                //
                if (hNext)
                    fcolStack.Push(tCIDLib::TSInt(hNext));
                hCur = hSub;
            }
             else
            {
                // No need to recurse, so just move forward
                hCur = hNext;

                // If no more at this level, try to pop if we can
                if (!hCur)
                {
                    if (!fcolStack.bIsEmpty())
                        hCur = (HTREEITEM)fcolStack.tPop();
                }
            }

            //
            //  Now go back to the top and start again. If we ran out, then we will just
            //  fall out.
            //
            continue;
        }

        // See if this one is checked. If so, store it's path
        tCIDCtrls::TMsgResult mresState = ::SendMessage
        (
            hwndUs
            , TVM_GETITEMSTATE
            , tCIDCtrls::TWParam(hCur)
            , tCIDCtrls::TLParam(TVIS_STATEIMAGEMASK)
        );

        if ((mresState >> 12) == 2)
        {
            colToFill.objAdd(ptmiCur->m_strPath);
            fcolFlags.c4AddElement(ptmiCur->m_c4UserFlag);
        }

        // Get the next sibling, if any
        hCur = (HTREEITEM)::SendMessage
        (
            hwndUs, TVM_GETNEXTITEM, TVGN_NEXT, tCIDCtrls::TLParam(hCur)
        );

        // If no more, see if we can move up another level
        if (!hCur)
        {
            if (!fcolStack.bIsEmpty())
                hCur = (HTREEITEM)fcolStack.tPop();
        }
    }

    return colToFill.c4ElemCount();
}


// Return the user flag for the indicated path
tCIDLib::TCard4
TTreeView::c4UserFlag(  const   TString&            strPath
                        , const tCIDLib::TBoolean   bThrowIfNot) const
{
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiSrc = ptmiFromPath(strPath, bThrowIfNot, c4At);
    if (!ptmiSrc)
        return kCIDLib::False;

    return ptmiSrc->m_c4UserFlag;
}


// Create a tree view ith the passed attributes
tCIDLib::TVoid
TTreeView::Create(  const   TWindow&                wndParent
                    , const tCIDCtrls::TWndId       widThis
                    , const TArea&                  areaInit
                    , const tCIDCtrls::EWndStyles   eStyles
                    , const tCIDCtrls::EExWndStyles eExStyles
                    , const tCIDCtrls::ETreeStyles  eTreeStyles)
{
    // Start with any styles we enforce
    tCIDLib::TCard4 c4Styles = TVS_NONEVENHEIGHT | TVS_NOTOOLTIPS
                               | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;

    // Add any class specific styles
    if (tCIDLib::bAllBitsOn(eTreeStyles, tCIDCtrls::ETreeStyles::CheckBoxes))
        c4Styles |= TVS_CHECKBOXES;

    if (tCIDLib::bAllBitsOn(eTreeStyles, tCIDCtrls::ETreeStyles::HasLines))
        c4Styles |= TVS_HASLINES | TVS_LINESATROOT;

    if (tCIDLib::bAllBitsOn(eTreeStyles, tCIDCtrls::ETreeStyles::ToolTips))
        c4Styles |= TVS_INFOTIP;

    if (tCIDLib::bAllBitsOn(eTreeStyles, tCIDCtrls::ETreeStyles::SingleExp))
        c4Styles |= TVS_SINGLEEXPAND;

    if (!tCIDLib::bAllBitsOn(eTreeStyles, tCIDCtrls::ETreeStyles::ReadOnly))
        c4Styles |= TVS_EDITLABELS;

    // Store our class specific styles away
    m_eTreeStyles = eTreeStyles;

    // And create the control
    CreateWnd
    (
        wndParent.hwndThis()
        , WC_TREEVIEWW
        , kCIDLib::pszEmptyZStr
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles(c4Styles)
        , eExStyles
        , widThis
    );

    // All standard controls must be subclassed
    SetSubclass();
}


tCIDCtrls::ETreeStyles TTreeView::eTreeStyles() const
{
    return m_eTreeStyles;
}


tCIDLib::TVoid TTreeView::ForceExpandedOnce(const TString& strPath)
{
    tCIDLib::TCard4 c4At;
    TTreeMapItem* ptmiScope = ptmiFromPath(strPath, kCIDLib::True, c4At);

    TVITEM Item = {0};
    Item.hItem = (HTREEITEM)ptmiScope->m_hItem;
    Item.mask = TVIF_STATE | TVIF_HANDLE;

    if (!::SendMessage(hwndSafe(), TVM_GETITEM, 0, tCIDCtrls::TLParam(&Item)))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcTree_GetItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strPath
        );
    }

    // Make sure it's a scope
    CheckIsScope(*ptmiScope, CID_LINE);

    // Clear the virtual flag at this point
    ptmiScope->m_bVirtual = kCIDLib::False;

    // Set the expanded one state stuff, then set it back
    Item.state |= TVIS_EXPANDEDONCE;
    Item.stateMask |= TVIS_EXPANDEDONCE;

    if (!::SendMessage(hwndSafe(), TVM_SETITEM, 0, tCIDCtrls::TLParam(&Item)))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcTree_SetItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strPath
        );
    }
}


//
//  Scroll/expand the list as requird to make the indicated item visible, and it will
//  select it as well.
//
tCIDLib::TVoid TTreeView::MakeItemVisible(const TString& strPath)
{
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiCheck = ptmiFromPath(strPath, kCIDLib::True, c4At);
    TreeView_EnsureVisible(hwndSafe(), ptmiCheck->m_hItem);

    ::SendMessage
    (
        hwndThis()
        , TVM_SELECTITEM
        , TVGN_CARET
        , tCIDCtrls::TLParam(ptmiCheck->m_hItem)
    );
}


// Provide access to the user defined per-item value
TObject* TTreeView::pobjUser(const TString& strPath, const tCIDLib::TBoolean bThrowIfNot)
{
    tCIDLib::TCard4 c4At;
    TTreeMapItem* ptmiChild = ptmiFromPath(strPath, bThrowIfNot, c4At);

    if (!ptmiChild)
        return nullptr;

    return ptmiChild->m_pobjUser;
}

const TObject*
TTreeView::pobjUser(const TString& strPath, const tCIDLib::TBoolean bThrowIfNot) const
{
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiChild = ptmiFromPath(strPath, bThrowIfNot, c4At);

    if (!ptmiChild)
        return nullptr;

    return ptmiChild->m_pobjUser;
}


//
//  Queries the children under a given scope. It can return just items or items and
//  sub-scopes.
//
tCIDLib::TVoid
TTreeView::QueryChildren(const  TString&            strParScope
                        ,       tCIDLib::TStrList&  colToFill
                        , const tCIDLib::TBoolean   bItemsOnly)
{
    const tCIDCtrls::TWndHandle hwndUs = hwndSafe();
    colToFill.RemoveAll();

    // Find the target and make sure it's a scope
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiPar = ptmiFromPath(strParScope, kCIDLib::True, c4At);
    CheckIsScope(*ptmiPar, CID_LINE);

    // Get the parent handle and get its first child item
    HTREEITEM hParent = (HTREEITEM)ptmiPar->m_hItem;
    HTREEITEM hCur = (HTREEITEM)::SendMessage
    (
        hwndUs, TVM_GETNEXTITEM, TVGN_CHILD, tCIDCtrls::TLParam(hParent)
    );

    // While we have children, continue
    TString strName;
    while (hCur)
    {
        // Get the text of this guy and use that to get the text
        TTreeMapItem* ptmiCur = ptmiFromHandle(tCIDLib::TSInt(hCur), kCIDLib::True, c4At);

        if (!ptmiCur->m_bScope || !bItemsOnly)
        {
            QueryItemText(ptmiCur->m_strPath, strName);
            colToFill.objAdd(strName);
        }

        // Get the next sibling. If no more we'll get null
        hCur = (HTREEITEM)::SendMessage
        (
            hwndUs, TVM_GETNEXTITEM, TVGN_NEXT, tCIDCtrls::TLParam(hCur)
        );
    }
}


//
//  Called to get the item text of the indicate path. In our case, because of the
//  way we reference items via a path built from their text, it's just the text of
//  the last path component.
//
tCIDLib::TVoid
TTreeView::QueryItemText(const TString& strPath, TString& strToFill)
{
    // Handle a special case, which will make the code below better
    if (strPath == L"/")
    {
        strToFill = strPath;
        return;
    }

    tCIDLib::TCard4 c4At;
    if (!strPath.bLastOccurrence(kCIDLib::chForwardSlash, c4At))
    {
        // Can't be legal
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcTree_GetItemText
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strPath
        );
    }

    //
    //  If the last character is a slash, then it's really the one before that
    //  that we want. There has to be one, since we checked for the root above.
    //
    if (strPath.chLast() == kCIDLib::chForwardSlash)
    {
        if (!strPath.bLastOccurrence(kCIDLib::chForwardSlash, c4At))
        {
            // Can't be legal
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcTree_GetItemText
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , strPath
            );
        }
    }

    // We want to keep everything from the next character forward
    strToFill.CopyInSubStr(strPath, c4At + 1);

    // If we got a trailing slash, remove it
    if (strToFill.chLast() == kCIDLib::chForwardSlash)
        strToFill.DeleteLast();
}


//
//  Remove all of the children of the indicated scope. The silly tree control doesn't
//  provide an efficient means to do this, so we have to do it by just deleting all
//  of the children one at a time.
//
//  Though we get notifications of the deletions and remove them from our list,
//
tCIDLib::TVoid TTreeView::RemoveChildrenOf(const TString& strParScope)
{
    const tCIDCtrls::TWndHandle hwndUs = hwndSafe();

    // Find the target and make sure it's a scope
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiPar = ptmiFromPath(strParScope, kCIDLib::True, c4At);
    CheckIsScope(*ptmiPar, CID_LINE);

    // Disable drawing while we do this
    TWndPaintJanitor janPaint(this);

    //
    //  Now just loop, querying and destroying the first child of this parent item
    //  until we get an error. So we always pass the parent and ask for the first child.
    //
    HTREEITEM hParent = (HTREEITEM)ptmiPar->m_hItem;
    while (kCIDLib::True)
    {
        HTREEITEM hCur = (HTREEITEM)::SendMessage
        (
            hwndUs, TVM_GETNEXTITEM, TVGN_CHILD, tCIDCtrls::TLParam(hParent)
        );

        // If null, then we are done
        if (!hCur)
            break;

        //
        //  Remove this guy. Our notification handler callback will see this and
        //  clean up our associated data.
        //
        ::SendMessage(hwndUs, TVM_DELETEITEM, 0, tCIDCtrls::TLParam(hCur));
    }

    // Update the parent
    UpdateChildCnt(strParScope);
}


//
//  Remove the target item (and its contents if it's a scope). When we do the delete,
//  we'll get a notification for each of the items removed, and will use that to
//  remove our map items.
//
tCIDLib::TVoid TTreeView::RemoveItem(const TString& strPath)
{
    //
    //  Look for the target item and throw if not found. If we find it, get out the
    //  handle.
    //
    tCIDLib::TCard4 c4At;
    HTREEITEM hItem = 0;
    {
        const TTreeMapItem* ptmiCur = ptmiFromPath(strPath, kCIDLib::True, c4At);
        hItem = (HTREEITEM)ptmiCur->m_hItem;
    }

    // Before we delete it, get the parent item
    HTREEITEM hPar = (HTREEITEM)::SendMessage
    (
        hwndSafe(), TVM_GETNEXTITEM, TVGN_PARENT, tCIDCtrls::TLParam(hItem)
    );

    // Now delete it
    ::SendMessage(hwndThis(), TVM_DELETEITEM, 0, tCIDCtrls::TLParam(hItem));

    // Update the parent child count indicator
    TTreeMapItem* pmtiPar = ptmiFromHandle(tCIDLib::TSInt(hPar), kCIDLib::True, c4At);
    UpdateChildCnt(pmtiPar->m_strPath);
}


// Select the indicated item
tCIDLib::TVoid TTreeView::SelectPath(const TString& strPath)
{
    // Get the handle for the indicated item
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiChild = ptmiFromPath(strPath, kCIDLib::True, c4At);

    ::SendMessage
    (
        hwndSafe()
        , TVM_SELECTITEM
        , TVGN_CARET
        , tCIDCtrls::TLParam(ptmiChild->m_hItem)
    );
}


//
//  Resets the tree
//
tCIDLib::TVoid TTreeView::Reset(const tCIDLib::TBoolean bVirtual)
{
    // Remove any existing content and clear our map out
    ::SendMessage(hwndSafe(), TVM_DELETEITEM, 0, 0);

    m_pcolAltMap->RemoveAll();
    m_pcolMap->RemoveAll();

    // And now add a a first value
    TVINSERTSTRUCTW Insert = {0};

    Insert.itemex.mask = TVIF_TEXT;
    Insert.itemex.pszText = (LPWSTR)L"/";
    Insert.itemex.iIntegral = 1;

    if (bVirtual)
    {
        Insert.itemex.mask |= TVIF_CHILDREN;
        Insert.itemex.cChildren = I_CHILDRENCALLBACK;
    }

    tCIDCtrls::TMsgResult mresHandle = ::SendMessage
    (
        hwndSafe(), TVM_INSERTITEM, 0, tCIDCtrls::TLParam(&Insert)
    );

    if (!mresHandle)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_InsertItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"TreeView")
            , TCardinal(widThis())
        );
    }

    //
    //  Add an entry to the map for this one, marked as the root and with the
    //  indicated text. It will have no parent path.
    //
    TTreeMapItem* ptmiNew = ptmiAddMapItem
    (
        TString::strEmpty()
        , L"/"
        , tCIDLib::TSInt(mresHandle)
        , kCIDLib::True
        , bVirtual
        , 0
    );
    ptmiNew->m_bRoot = kCIDLib::True;
}


// Sets the check marks on all items in the tree
tCIDLib::TVoid TTreeView::SetAllChecks(const tCIDLib::TBoolean bToSet)
{
    // If we don't have the check marks style, then bad
    CIDAssert
    (
        tCIDLib::bAllBitsOn(m_eTreeStyles, tCIDCtrls::ETreeStyles::CheckBoxes)
        , L"This tree control doesn't have the check boxes style"
    );

    // Disable drawing while we do this
    TWndPaintJanitor janPaint(this);

    // Create a stack for our recursion
    TFundStack<tCIDLib::TSInt> fcolStack(1024);

    // Set up everything we need to set, except the per-item handle
    TVITEM tvi = {0};
    tvi.mask = TVIF_HANDLE | TVIF_STATE;
    tvi.state = INDEXTOSTATEIMAGEMASK(bToSet ? 2 : 1);
    tvi.stateMask = TVIS_STATEIMAGEMASK;

    // Get the root item and start iterating
    const tCIDCtrls::TWndHandle hwndUs = hwndSafe();
    HTREEITEM hCur = (HTREEITEM)::SendMessage(hwndUs, TVM_GETNEXTITEM, TVGN_ROOT, 0);
    while (hCur)
    {
        tCIDLib::TCard4 c4At;
        const TTreeMapItem* ptmiCur = ptmiFromHandle(tCIDLib::TSInt(hCur), kCIDLib::True, c4At);

        // Set this guy's check
        tvi.hItem = hCur;
        ::SendMessage(hwndUs, TVM_SETITEM, 0, tCIDCtrls::TLParam(&tvi));

        // If a scope, see if we need to recurse
        if (ptmiCur->m_bScope)
        {
            // See if this guy has any children
            HTREEITEM hSub = (HTREEITEM)::SendMessage
            (
                hwndUs, TVM_GETNEXTITEM, TVGN_CHILD, tCIDCtrls::TLParam(hCur)
            );

            // And get the next child of the current one
            HTREEITEM hNext = (HTREEITEM)
            (
                ::SendMessage(hwndUs, TVM_GETNEXTITEM, TVGN_NEXT, tCIDCtrls::TLParam(hCur))
            );

            if (hSub)
            {
                //
                //  There are children, so push the next child of the current one, if it
                //  is non-null, and make the first new child the current.
                //
                if (hNext)
                    fcolStack.Push(tCIDLib::TSInt(hNext));
                hCur = hSub;
            }
             else
            {
                // No need to recurse, so just move forward
                hCur = hNext;

                // If no more at this level, try to pop if we can
                if (!hCur)
                {
                    if (!fcolStack.bIsEmpty())
                        hCur = (HTREEITEM)fcolStack.tPop();
                }
            }

            //
            //  Now go back to the top and start again. If we ran out, then we will just
            //  fall out.
            //
            continue;
        }

        // Get the next sibling, if any
        hCur = (HTREEITEM)::SendMessage
        (
            hwndUs, TVM_GETNEXTITEM, TVGN_NEXT, tCIDCtrls::TLParam(hCur)
        );

        // If no more, see if we can move up another level
        if (!hCur)
        {
            if (!fcolStack.bIsEmpty())
                hCur = (HTREEITEM)fcolStack.tPop();
        }
    }
}


// Sets the check mark on all (direct, non-scope) children of the passed path
tCIDLib::TVoid
TTreeView::SetAllChildChecks(const  TString&            strPath
                            , const tCIDLib::TBoolean   bToSet)
{
    // If we don't have the check marks style, then bad
    CIDAssert
    (
        tCIDLib::bAllBitsOn(m_eTreeStyles, tCIDCtrls::ETreeStyles::CheckBoxes)
        , L"This tree control doesn't have the check boxes style"
    );

    // Find the target and make sure it's a scope
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiPar = ptmiFromPath(strPath, kCIDLib::True, c4At);
    CheckIsScope(*ptmiPar, CID_LINE);

    // Disable drawing while we do this
    TWndPaintJanitor janPaint(this);

    // Set up everything we need to set, except the per-round handle
    TVITEM tvi = {0};
    tvi.mask = TVIF_HANDLE | TVIF_STATE;
    tvi.state = INDEXTOSTATEIMAGEMASK(bToSet ? 2 : 1);
    tvi.stateMask = TVIS_STATEIMAGEMASK;

    // Get the first child to start us off
    const tCIDCtrls::TWndHandle hwndUs = hwndSafe();
    HTREEITEM hCur = (HTREEITEM)::SendMessage
    (
        hwndUs, TVM_GETNEXTITEM, TVGN_CHILD, tCIDCtrls::TLParam(ptmiPar->m_hItem)
    );

    while (hCur)
    {
        // If it's a scope, we don't do it
        const TTreeMapItem* ptmiCur = ptmiFromHandle
        (
            tCIDLib::TSInt(hCur), kCIDLib::True, c4At
        );

        if (!ptmiCur->m_bScope)
        {
            // Set this guy's check
            tvi.hItem = hCur;
            ::SendMessage(hwndUs, TVM_SETITEM, 0, tCIDCtrls::TLParam(&tvi));
        }

        // Get the next one, if any
        hCur = (HTREEITEM)::SendMessage
        (
            hwndUs, TVM_GETNEXTITEM, TVGN_NEXT, tCIDCtrls::TLParam(hCur)
        );
    }
}



//
//  Put the indicate scope into auto-child mode, so that it's ready for faulting in
//  if need be.
//
tCIDLib::TVoid TTreeView::SetAutoChild(const TString& strPath)
{
    tCIDLib::TCard4 c4At;
    TTreeMapItem* ptmiChild = ptmiFromPath(strPath, kCIDLib::True, c4At);
    CheckIsScope(*ptmiChild, CID_LINE);

    ::SendMessage
    (
        hwndThis()
        , TVM_EXPAND
        , TVE_COLLAPSE | TVE_COLLAPSERESET
        , tCIDCtrls::TLParam(ptmiChild->m_hItem)
    );

    // Set the virtual scope
    ptmiChild->m_bVirtual = kCIDLib::True;
}


// Set custom drawing options
tCIDLib::TVoid TTreeView::SetCustomDrawOpts(const tCIDCtrls::ETreeCustOpts eToSet)
{
    m_eCustOpts = eToSet;
}


//
//  Expand or contract an item
//
tCIDLib::TVoid
TTreeView::SetExpandState(  const   TString&            strPath
                            , const tCIDLib::TBoolean   bExpand)
{
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiChild = ptmiFromPath(strPath, kCIDLib::True, c4At);

    ::SendMessage
    (
        hwndSafe()
        , TVM_EXPAND
        , bExpand ? TVE_EXPAND : TVE_COLLAPSE
        , tCIDCtrls::TLParam(ptmiChild->m_hItem)
    );

    //
    //  If expanding, force the freaking expanded once flag on, since it won't do it
    //  otherwise if this path ended up being empty after faulting in.
    //
    if (bExpand)
        ForceExpandedOnce(strPath);
}


//
//  Set the per-item user flag, optionally redrawing the item if the caller knows
//  that the user flag is being used to control custom drawing.
//
tCIDLib::TVoid
TTreeView::SetUserFlag( const   TString&            strPath
                        , const tCIDLib::TCard4     c4ToSet
                        , const tCIDLib::TBoolean   bRedraw)
{
    tCIDLib::TCard4 c4At;
    TTreeMapItem* ptmiTar = ptmiFromPath(strPath, kCIDLib::True, c4At);
    ptmiTar->m_c4UserFlag = c4ToSet;

    if (bRedraw)
        RedrawItem(strPath);
}


//
//  Set a user defined object on an item. We adopt it. The path has to exist or
//  we throw.
//
tCIDLib::TVoid
TTreeView::SetUserObj(const TString& strPath, TObject* const pobjToAdopt)
{
    // Put a janitor on the object until we get it stored
    TJanitor<TObject> janUser(pobjToAdopt);

    tCIDLib::TCard4 c4At;
    TTreeMapItem* ptmiTar = ptmiFromPath(strPath, kCIDLib::True, c4At);

    // If an old one exists, delete it
    try
    {
        if (ptmiTar->m_pobjUser)
        {
            delete ptmiTar->m_pobjUser;
            ptmiTar->m_pobjUser = nullptr;
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    ptmiTar->m_pobjUser = janUser.pobjOrphan();
}


//
//  Any time we update the contents of a scope, we need to update the scope's child
//  count, to control the +/- sign indicator.
//
tCIDLib::TVoid TTreeView::UpdateChildCnt(const TString& strPath)
{
    tCIDLib::TCard4 c4At;
    TTreeMapItem* ptmiScope = ptmiFromPath(strPath, kCIDLib::True, c4At);

    // See if this one has any children
    const tCIDLib::TBoolean bHasChildren
    (
        ::SendMessage
        (
            hwndSafe(), TVM_GETNEXTITEM, TVGN_CHILD, tCIDCtrls::TLParam(ptmiScope->m_hItem)
        )   != NULL
    );

    // Make sure the virtual flag is off, since we are setting the count explicitly
    ptmiScope->m_bVirtual = kCIDLib::False;

    TVITEM Item = {0};
    Item.hItem = (HTREEITEM)ptmiScope->m_hItem;
    Item.mask = TVIF_CHILDREN | TVIF_HANDLE;
    if (bHasChildren)
        Item.cChildren = 1;
    else
        Item.cChildren = 0;

    ::SendMessage(hwndThis(), TVM_SETITEM, 0, tCIDCtrls::TLParam(&Item));
}


//
//  Update the text of an item
//
tCIDLib::TVoid
TTreeView::UpdateItem(  const   TString&    strParPath
                        , const TString&    strOldText
                        , const TString&    strNewText)
{
    TString strOrgPath(strParPath);
    strOrgPath.Append(kCIDLib::chForwardSlash);
    strOrgPath.Append(strOldText);

    tCIDLib::TCard4 c4At;
    TTreeMapItem* ptmiPar = ptmiFromPath(strOrgPath, kCIDLib::True, c4At);

    //
    //  Update the path in the map item. It's a hash set, so we have to orphan the
    //  item out update it, and add it back.
    //
    m_pcolMap->OrphanElem(ptmiPar);
    TString strNewPath(strParPath);
    strNewPath.Append(kCIDLib::chForwardSlash);
    strNewPath.Append(strNewText);
    ptmiPar->m_strPath = strNewPath;
    m_pcolMap->Add(ptmiPar);

    // And update the display text
    TVITEMW Item = {0};
    Item.mask = TVIF_TEXT | TVIF_HANDLE;
    Item.hItem = (HTREEITEM)ptmiPar->m_hItem;
    Item.pszText = (LPWSTR)strNewText.pszBuffer();

    ::SendMessage(hwndThis(), TVM_SETITEM, 0, tCIDCtrls::TLParam(&Item));

    //
    //  Now we have to deal with the fact that our item objects contain the full
    //  path to themselves. If this was a scope we just changed, then anything
    //  under it will have to be updated. So we need to iterate all of the items
    //  and anything that starts with the old path needs to have that part of its
    //  path replaced with the new path. As above, we have to orphan, update, then
    //  add back.
    //
    if (ptmiPar->m_bScope)
    {
        try
        {
            const tCIDLib::TCard4 c4Count = m_pcolAltMap->c4ElemCount();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                TTreeMapItem* ptmiCur = m_pcolAltMap->pobjAt(c4Index);
                if (ptmiCur->m_strPath.bStartsWithI(strOrgPath))
                {
                    m_pcolMap->OrphanElem(ptmiCur);
                    ptmiCur->m_strPath.Cut(0, strOrgPath.c4Length());
                    ptmiCur->m_strPath.Insert(strNewPath, 0);
                    m_pcolMap->Add(ptmiCur);
                }
            }
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);

            TExceptDlg dlgErr
            (
                *this
                , L"CIDLib Tree Control"
                , L"An error occurred while renaming a scope. You probably should \
                    restart the program just to be safe."
                , errToCatch
            );
        }
    }
}


// ---------------------------------------------------------------------------
// TTreeView: Protected, virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::ETreeCustRets
TTreeView::eCustomDraw( const   TString&
                        ,       TObject* const
                        , const tCIDLib::TCard4
                        , const tCIDLib::TBoolean
                        , const tCIDLib::TBoolean
                        , const tCIDLib::TCard4
                        ,       TRGBClr&
                        ,       TRGBClr&)
{
    //
    //  If not overridden, do nothing. Shouldn't happen since they have to set a
    //  style to get this to be called, so they should handle it.
    //
    return tCIDCtrls::ETreeCustRets::None;
}


// ---------------------------------------------------------------------------
// TTreeView: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  Make sure we have a root element, which we assume isn't virtual. If the caller thinks
//  otherwise, he has to reset it again.
//
tCIDLib::TBoolean TTreeView::bCreated()
{
    TParent::bCreated();

    Reset(kCIDLib::False);
    return kCIDLib::True;
}



//
//  If the containing code has set the 'invoke on enter' style, then we want to make sure
//  that we get enter keys. Normally the tree control doesn't process them, which is usually
//  best since we want that to handle default button stuff. But, in those cases where a
//  default button doesn't exist, it can be convenient.
//
tCIDLib::TBoolean
TTreeView::bEatSubClassMsg( const   tCIDCtrls::TWndMsg      wmsgMsg
                            , const tCIDCtrls::TWParam      wParam
                            , const tCIDCtrls::TLParam      lParam
                            ,       tCIDCtrls::TMsgResult&  mresRet)
{
    if (wmsgMsg == WM_GETDLGCODE)
    {
        if ((wParam == VK_RETURN)
        &&  tCIDLib::bAllBitsOn(m_eTreeStyles, tCIDCtrls::ETreeStyles::InvokeOnEnter))
        {
            mresRet = DLGC_WANTMESSAGE;
            return kCIDLib::True;
        }
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
TTreeView::bNotReflect(         TWindow&                wndTar
                        , const tCIDCtrls::TWndMsg      wmsgMsg
                        , const tCIDCtrls::TWParam      wParam
                        , const tCIDCtrls::TLParam      lParam
                        ,       tCIDCtrls::TMsgResult&  mresResult)
{
    // We are looking only for notifications
    if (wmsgMsg != WM_NOTIFY)
        return kCIDLib::False;

    // Get a pointer to the notification header
    NMHDR* pNotHdr = (NMHDR*)lParam;

    if (pNotHdr->code == NM_DBLCLK)
    {
        // Get the mouse position and see if we hit an item
        const tCIDLib::TCard4 c4Pos = ::GetMessagePos();

        TVHITTESTINFO Info = {0};
        Info.pt.x = TRawBits::i2Low16From32(c4Pos);
        Info.pt.y = TRawBits::i2High16From32(c4Pos);

        // Convert to our client area coordinates
        ::ScreenToClient(hwndThis(), &Info.pt);

        tCIDCtrls::TMsgResult mresHit = ::SendMessage
        (
            hwndThis(), TVM_HITTEST, 0, tCIDCtrls::TLParam(&Info)
        );

        // If we hit something, let's process it
        if (mresHit)
        {
            // Find the this item
            tCIDLib::TCard4 c4At;
            const TTreeMapItem* ptmiTar = ptmiFromHandle(mresHit, kCIDLib::True, c4At);
            if (ptmiTar)
            {
                SendAsyncNotify
                (
                    new TTreeEventInfo
                    (
                        tCIDCtrls::ETreeEvents::Invoked, ptmiTar->m_strPath, *this
                    )
                    , TTreeView::nidTreeEvent
                );
            }
        }
        return kCIDLib::True;
    }
     else if (pNotHdr->code == NM_RETURN)
    {
        //
        //  Only do this if asked to. Note that we handle the sub-class override method
        //  above and indicate we want enter keys if this style is on, else we would never
        //  get called here.
        //
        if (tCIDLib::bAllBitsOn(m_eTreeStyles, tCIDCtrls::ETreeStyles::InvokeOnEnter))
        {
            // Get the selected item. If there is one, then send a notification
            TString strPath;
            if (bSelectedItem(strPath))
            {
                SendAsyncNotify
                (
                    new TTreeEventInfo(tCIDCtrls::ETreeEvents::Invoked, strPath, *this)
                    , TTreeView::nidTreeEvent
                );
            }
            return kCIDLib::True;
        }
    }
     else if (pNotHdr->code == NM_TVSTATEIMAGECHANGING)
    {
        //
        //  The only state images we ever have is the built in check box stuff. So we look
        //  at the new state and send a check state change event. This should only happen
        //  if we have the check boxes style, but check just in case.
        //
        if (tCIDLib::bAllBitsOn(m_eTreeStyles, tCIDCtrls::ETreeStyles::CheckBoxes))
        {
            NMTVSTATEIMAGECHANGING* pInfo = (NMTVSTATEIMAGECHANGING*)lParam;

            // Find the reported item
            tCIDLib::TCard4 c4At;
            const TTreeMapItem* ptmiTar = ptmiFromHandle
            (
                tCIDLib::TSInt(pInfo->hti), kCIDLib::True, c4At
            );

            if (ptmiTar)
            {
                //
                //  We found it so send the event. If new index is 2, then it's checked,
                //  else it's not checked or the check was removed which we treat as not
                //  checked.
                //
                SendAsyncNotify
                (
                    new TTreeEventInfo
                    (
                        tCIDCtrls::ETreeEvents::CheckState
                        , ptmiTar->m_strPath
                        , 0
                        , *this
                        , (pInfo->iNewStateImageIndex == 2)
                    )
                    , TTreeView::nidTreeEvent
                );
            }
        }
    }
     else if (pNotHdr->code == NM_RCLICK)
    {
        //
        //  If a right click happens on a new item, we can get it using the drop
        //  hilite flag. So, if it returns a handle, select that one. Else, assume
        //  the user clicked on the already selected one, and get that.
        //
        HTREEITEM hItem = TreeView_GetNextItem(pNotHdr->hwndFrom, 0, TVGN_DROPHILITE);
        if(hItem)
        {
            TreeView_SelectItem(pNotHdr->hwndFrom, hItem);
        }
         else
        {
            hItem = (HTREEITEM)::SendMessage(hwndSafe(), TVM_GETNEXTITEM, TVGN_CARET, 0);
        }

        // If we got something, look it up and send a notification
        if (hItem)
        {
            tCIDLib::TCard4 c4At;
            const TTreeMapItem* ptmiTar = ptmiFromHandle
            (
                tCIDLib::TSInt(hItem), kCIDLib::False, c4At
            );
            if (ptmiTar)
            {
                // If we have this one in our map, send an event
                tCIDLib::TCard4 c4At;
                const TTreeMapItem* ptmiTar = ptmiFromHandle
                (
                    tCIDLib::TSInt(hItem), kCIDLib::False, c4At
                );
                if (ptmiTar)
                {
                    SendAsyncNotify
                    (
                        new TTreeEventInfo
                        (
                            tCIDCtrls::ETreeEvents::Menu, ptmiTar->m_strPath, *this
                        )
                        , TTreeView::nidTreeEvent
                    );
                    return kCIDLib::True;
                }
            }
        }
    }
     else if ((pNotHdr->code == TVN_ITEMEXPANDING)
          ||  (pNotHdr->code == TVN_ITEMEXPANDED))
    {
        // Get the parameter as it's real type
        NMTREEVIEW* pTView = (NMTREEVIEW*)lParam;

        // Find the this item
        tCIDLib::TCard4 c4At;
        const TTreeMapItem* ptmiTar = ptmiFromHandle
        (
            tCIDLib::TSInt(pTView->itemNew.hItem), kCIDLib::True, c4At
        );

        TTreeEventInfo* pwnotNew = nullptr;
        if (pTView->action & TVE_COLLAPSE)
        {
            pwnotNew = new TTreeEventInfo
            (
                (pNotHdr->code == TVN_ITEMEXPANDING) ? tCIDCtrls::ETreeEvents::Collapsing
                                                     : tCIDCtrls::ETreeEvents::Collapsed
                , ptmiTar->m_strPath
                , *this
            );
        }
         else if (pTView->action & TVE_EXPAND)
        {
            pwnotNew = new TTreeEventInfo
            (
                (pNotHdr->code == TVN_ITEMEXPANDING) ? tCIDCtrls::ETreeEvents::Expanding
                                                     : tCIDCtrls::ETreeEvents::Expanded
                , ptmiTar->m_strPath
                , *this
            );
        }

        // If it's one we understand, send it
        if (pwnotNew)
        {
            //
            //  The 'ing' versions are synchronous, to get updated before the user
            //  invoked operation returns. The final 'ed' versions are async to do
            //  something after it's all completed.
            //
            if (pNotHdr->code == TVN_ITEMEXPANDING)
            {
                // Set a janitor, since we have to clean up for a sync
                TJanitor<TTreeEventInfo> janEvent(pwnotNew);
                SendSyncNotify(*pwnotNew, TTreeView::nidTreeEvent);
            }
             else
            {
                SendAsyncNotify(pwnotNew, TTreeView::nidTreeEvent);
            }
        }
    }
     else if (pNotHdr->code == TVN_SELCHANGING)
    {
        // Get the parameter as it's real type
        NMTREEVIEW* pTView = (NMTREEVIEW*)lParam;

        // The old may not be there, if this is, say, the first item being selected
        tCIDLib::TCard4 c4At;
        const TTreeMapItem* ptmiOld = ptmiFromHandle
        (
            tCIDLib::TSInt(pTView->itemOld.hItem), kCIDLib::False, c4At
        );
        const TTreeMapItem* ptmiNew = ptmiFromHandle
        (
            tCIDLib::TSInt(pTView->itemNew.hItem), kCIDLib::True, c4At
        );

        TString strOldPath;
        if (ptmiOld)
            strOldPath = ptmiOld->m_strPath;

        // Synchronous in this case
        TTreeEventInfo wnotSend
        (
            tCIDCtrls::ETreeEvents::SelChanging
            , ptmiNew->m_strPath
            , strOldPath
            , *this
        );
        SendSyncNotify(wnotSend, TTreeView::nidTreeEvent);
    }
     else if (pNotHdr->code == TVN_SELCHANGED)
    {
        // Get the parameter as it's real type
        NMTREEVIEW* pTView = (NMTREEVIEW*)lParam;

        // The old may not be there, if this is, say, the first item being selected
        tCIDLib::TCard4 c4At;
        const TTreeMapItem* ptmiOld = ptmiFromHandle
        (
            tCIDLib::TSInt(pTView->itemOld.hItem), kCIDLib::False, c4At
        );
        const TTreeMapItem* ptmiNew = ptmiFromHandle
        (
            tCIDLib::TSInt(pTView->itemNew.hItem), kCIDLib::True, c4At
        );

        TString strOldPath;
        if (ptmiOld)
            strOldPath = ptmiOld->m_strPath;

        // Async in this case
        SendAsyncNotify
        (
            new TTreeEventInfo
            (
                tCIDCtrls::ETreeEvents::SelChanged
                , ptmiNew->m_strPath
                , strOldPath
                , *this
            )
            , TTreeView::nidTreeEvent
        );
    }
     else if (pNotHdr->code == TVN_DELETEITEM)
    {
        // Get the parameter as it's real type
        NMTREEVIEW* pTView = (NMTREEVIEW*)lParam;

        //
        //  Remove it first from the non-collecting alt map, and that gets us a
        //  pointer to the item, so we can get the path and remove from the main
        //  collection.
        //
        tCIDLib::TCard4 c4At;
        TTreeMapItem* ptmiRem = m_pcolAltMap->pobjKeyedBinarySearch
        (
            tCIDLib::TSInt(pTView->itemOld.hItem), &TTreeMapItem::eHandleKeyComp, c4At
        );

        if (ptmiRem)
        {
            m_pcolAltMap->RemoveAt(c4At);
            const TString strKey = ptmiRem->m_strPath;
            m_pcolMap->bRemoveKey(strKey);
        }
    }
     else if (pNotHdr->code == TVN_BEGINLABELEDIT)
    {
        // Get the parameter as it's real type
        NMTVDISPINFO* pInfo = (NMTVDISPINFO*)lParam;

        tCIDLib::TCard4 c4At;
        TTreeMapItem* ptmiRen = m_pcolAltMap->pobjKeyedBinarySearch
        (
            tCIDLib::TSInt(pInfo->item.hItem), &TTreeMapItem::eHandleKeyComp, c4At
        );

        if (ptmiRen)
        {
            SendAsyncNotify
            (
                new TTreeEventInfo(tCIDCtrls::ETreeEvents::Rename, ptmiRen->m_strPath, *this)
                , TTreeView::nidTreeEvent
            );
        }

        // Cancel the edit
        mresResult = 1;
        return kCIDLib::True;
    }
     else if (pNotHdr->code == TVN_GETDISPINFO)
    {
        NMTVDISPINFO* pInfo = (NMTVDISPINFO*)lParam;
        tCIDLib::TCard4 c4At;
        TTreeMapItem* ptmiInfo = m_pcolAltMap->pobjKeyedBinarySearch
        (
            tCIDLib::TSInt(pInfo->item.hItem), &TTreeMapItem::eHandleKeyComp, c4At
        );

        if (ptmiInfo)
        {
            if (pInfo->item.mask & TVIF_CHILDREN)
                pInfo->item.cChildren = ptmiInfo->m_bVirtual ? 1 : 0;
        }
    }
     else if (pNotHdr->code == NM_CUSTOMDRAW)
    {
        mresResult = CDRF_DODEFAULT;
        if (m_eCustOpts != tCIDCtrls::ETreeCustOpts::None)
        {
            NMTVCUSTOMDRAW* pDraw = (NMTVCUSTOMDRAW*)lParam;

            tCIDCtrls::ETreeCustRets eCustRet = tCIDCtrls::ETreeCustRets::None;
            if (pDraw->nmcd.dwDrawStage == CDDS_PREPAINT)
            {
                //
                //  We know there are some custom options set, or we wouldn't have
                //  gotten here, so set the return flags appropriately to match
                //  our flags.
                //
                mresResult = 0;
                if (tCIDLib::bAllBitsOn(m_eCustOpts, tCIDCtrls::ETreeCustOpts::PostDraw))
                    mresResult |= CDRF_NOTIFYPOSTPAINT;

                if (tCIDLib::bAnyBitsOn(m_eCustOpts, tCIDCtrls::ETreeCustOpts::AllItem))
                    mresResult |= CDRF_NOTIFYITEMDRAW;
            }
             else if (pDraw->nmcd.dwDrawStage == CDDS_POSTPAINT)
            {
                if (tCIDLib::bAllBitsOn(m_eCustOpts, tCIDCtrls::ETreeCustOpts::PostDraw))
                {
                    //
                    //  Pass an empty path and user object, to indicate this is not for
                    //  a specific item, and true for post mode.
                    //
                    eCustomDraw
                    (
                        TString::strEmpty()
                        , nullptr
                        , kCIDLib::False
                        , kCIDLib::True
                        , kCIDLib::False
                        , kCIDLib::c4MaxCard
                        , m_rgbCustBgn
                        , m_rgbCustText
                    );
                }
            }
             else if (pDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
            {
                if (tCIDLib::bAllBitsOn(m_eCustOpts, tCIDCtrls::ETreeCustOpts::PreItem))
                {
                    tCIDLib::TCard4 c4At;
                    TTreeMapItem* ptmiTar = ptmiFromHandle
                    (
                        pDraw->nmcd.dwItemSpec, kCIDLib::False, c4At
                    );

                    if (ptmiTar)
                    {
                        eCustRet = eCustomDraw
                        (
                            ptmiTar->m_strPath
                            , ptmiTar->m_pobjUser
                            , ptmiTar->m_c4UserFlag
                            , kCIDLib::False
                            , (pDraw->nmcd.uItemState & CDIS_SELECTED) != 0
                            , tCIDLib::TCard4(pDraw->iLevel)
                            , m_rgbCustBgn
                            , m_rgbCustText
                        );

                        // If custom colors were returned set them
                        if (tCIDLib::bAllBitsOn(eCustRet, tCIDCtrls::ETreeCustRets::NewTextClr))
                            pDraw->clrText = m_rgbCustText.c4Color();

                        if (tCIDLib::bAllBitsOn(eCustRet, tCIDCtrls::ETreeCustRets::NewBgnClr))
                            pDraw->clrTextBk = m_rgbCustBgn.c4Color();
                    }
                }

                // Indicate if we need to do post paint
                if (tCIDLib::bAllBitsOn(m_eCustOpts, tCIDCtrls::ETreeCustOpts::PostItem))
                    mresResult |= CDRF_NOTIFYPOSTPAINT;
            }
             else if (pDraw->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT)
            {
                if (tCIDLib::bAllBitsOn(m_eCustOpts, tCIDCtrls::ETreeCustOpts::PostItem))
                {
                    tCIDLib::TCard4 c4At;
                    TTreeMapItem* ptmiTar = ptmiFromHandle
                    (
                        pDraw->nmcd.dwItemSpec, kCIDLib::False, c4At
                    );

                    if (ptmiTar)
                    {
                        eCustRet = eCustomDraw
                        (
                            ptmiTar->m_strPath
                            , ptmiTar->m_pobjUser
                            , ptmiTar->m_c4UserFlag
                            , kCIDLib::True
                            , (pDraw->nmcd.uItemState & CDIS_SELECTED) != 0
                            , tCIDLib::TCard4(pDraw->iLevel)
                            , m_rgbCustBgn
                            , m_rgbCustText
                        );
                    }
                }
            }

            // Add any other flags to the return based on our custom return
            if (tCIDLib::bAllBitsOn(eCustRet, tCIDCtrls::ETreeCustRets::NewFont))
                mresResult |= CDRF_NEWFONT;
        }
        return kCIDLib::True;
    }

    return kCIDLib::False;
}


tCIDLib::TVoid TTreeView::Destroyed()
{
    // If we we set up for check boxes, then we have to clean up the image list
    if (tCIDLib::bAllBitsOn(m_eTreeStyles, tCIDCtrls::ETreeStyles::CheckBoxes))
    {
        tCIDCtrls::TMsgResult mresHand = ::SendMessage(hwndThis(), TVM_GETIMAGELIST, TVSIL_STATE, 0);
        if (mresHand)
            ImageList_Destroy(HIMAGELIST(mresHand));
    }

    TParent::Destroyed();
}


// ---------------------------------------------------------------------------
// TTreeView: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Mostly for custom drawn derivatives. If they see that something has changed that
//  would make them draw an item differently, they need to force it to redraw. We
//  also can use it internally if we need to force a redraw and haven't already done
//  the path lookup.
//
tCIDLib::TVoid TTreeView::RedrawItem(const TString& strPath)
{
    tCIDLib::TCard4 c4At;
    const TTreeMapItem* ptmiTar = ptmiFromPath(strPath, kCIDLib::False, c4At);

    // If we found it, then get its bounding rect
    if (ptmiTar)
    {
        RECT itemRect;
        *((HTREEITEM*)&itemRect) = (HTREEITEM)ptmiTar->m_hItem;
        tCIDCtrls::TMsgResult mresTest = ::SendMessage
        (
            hwndSafe(), TVM_GETITEMRECT, FALSE, tCIDCtrls::TLParam(&itemRect)
        );

        // Invalidate the rectangle we got, if any
        if (mresTest)
        {
            ::RedrawWindow
            (
                hwndThis()
                , &itemRect
                , 0
                , RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_INVALIDATE
            );
        }
    }
}


// ---------------------------------------------------------------------------
// TTreeView: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Checks that the passed tree map item is a scope. If it's an item, it throws
tCIDLib::TVoid
TTreeView::CheckIsScope(const TTreeMapItem& tmiCheck, const tCIDLib::TCard4 c4Line) const
{
    if (!tmiCheck.m_bScope)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , c4Line
            , kCtrlsErrs::errcTree_NotAScope
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , tmiCheck.m_strPath
        );
    }
}


//
//  This is called to create a new map item and add it to the two mapping collections
//  we keep.
//
TTreeMapItem*
TTreeView::ptmiAddMapItem(  const   TString&            strParPath
                            , const TString&            strName
                            ,       tCIDLib::TSInt      hItem
                            , const tCIDLib::TBoolean   bIsScope
                            , const tCIDLib::TBoolean   bVirtual
                            , const tCIDLib::TCard4     c4UserFlag)
{
    TTreeMapItem* ptmiRet = new TTreeMapItem(strParPath, strName, hItem, bIsScope);
    try
    {
        // Store the user flag and virtual flag on it (make sure it's a scope on the altter)
        if (bIsScope)
            ptmiRet->m_bVirtual = bVirtual;
        ptmiRet->m_c4UserFlag = c4UserFlag;

        // Add it first to the
        m_pcolMap->Add(ptmiRet);
    }

    catch(TError& errToCatch)
    {
        // The map is adopting, so it will clean it up if needed
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // Add it to the non-adopting alt map, sorted by handle
    tCIDLib::TCard4 c4At;
    m_pcolAltMap->InsertSorted(ptmiRet, &TTreeMapItem::eHandleComp, c4At);

    return ptmiRet;
}


//
//  Look up an item handle in the map list. We do a binary search of the alt map
//  for this one.
//
const TTreeMapItem*
TTreeView::ptmiFromHandle( const    tCIDLib::TSInt      hItem
                            , const tCIDLib::TBoolean   bThrowIfNot
                            ,       tCIDLib::TCard4&    c4At) const
{
    TTreeMapItem* ptmiRet = m_pcolAltMap->pobjKeyedBinarySearch
    (
        hItem, TTreeMapItem::eHandleKeyComp, c4At
    );

    if (!ptmiRet)
    {
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcTree_HandleNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
            );
        }
        return nullptr;
    }
    return ptmiRet;
}

TTreeMapItem*
TTreeView::ptmiFromHandle( const    tCIDLib::TSInt      hItem
                            , const tCIDLib::TBoolean   bThrowIfNot
                            ,       tCIDLib::TCard4&    c4At)
{
    return const_cast<TTreeMapItem*>
    (
        const_cast<const TTreeView*>(this)->ptmiFromHandle(hItem, bThrowIfNot, c4At)
    );
}


//
//  Look up a path in the map list. The list is sorted by path, so we can do a
//  binary search.
//
const TTreeMapItem*
TTreeView::ptmiFromPath(const   TString&            strToFind
                        , const tCIDLib::TBoolean   bThrowIfNot
                        ,       tCIDLib::TCard4&    c4At) const
{
    // Find this item
    const TTreeMapItem* ptmiTar = m_pcolMap->pobjFindByKey(strToFind, kCIDLib::False);
    if (!ptmiTar)
    {
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcTree_PathNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strToFind
                , TString(L"TreeView")
            );
        }
        return nullptr;
    }
    return ptmiTar;
}

TTreeMapItem*
TTreeView::ptmiFromPath(const   TString&            strToFind
                        , const tCIDLib::TBoolean   bThrowIfNot
                        ,       tCIDLib::TCard4&    c4At)
{
    return const_cast<TTreeMapItem*>
    (
        const_cast<const TTreeView*>(this)->ptmiFromPath(strToFind, bThrowIfNot, c4At)
    );
}




// ---------------------------------------------------------------------------
//   CLASS: TTreeItemJan
//  PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTreeItemJan: Constructors and Destructor
// ---------------------------------------------------------------------------
TTreeItemJan::TTreeItemJan(         TTreeView* const    pwndTar
                            , const TString&            strPath) :

    m_pwndTar(pwndTar)
    , m_strPath(strPath)
{
}

TTreeItemJan::~TTreeItemJan()
{
    if (m_pwndTar && !m_strPath.bIsEmpty())
    {
        try
        {
            m_pwndTar->RemoveItem(m_strPath);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  TTreeItemJan: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Just null the window pointer
tCIDLib::TVoid TTreeItemJan::Orphan()
{
    m_pwndTar = nullptr;
}


const TString& TTreeItemJan::strPath() const
{
    return m_strPath;
}
