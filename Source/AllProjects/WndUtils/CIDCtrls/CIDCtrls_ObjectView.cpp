//
// FILE NAME: CIDCtrls_ObjView.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/27/2015
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
//  This file implements the standard object oriented display of images representing
//  objects of whatever type.
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
RTTIDecls(TObjVChangeInfo,TCtrlNotify)
RTTIDecls(TObjView, TStdCtrlWnd)




// ---------------------------------------------------------------------------
//   CLASS: TObjVChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TObjVChangeInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TObjVChangeInfo::TObjVChangeInfo(const  tCIDCtrls::EObjVEvents  eEvent
                                , const tCIDLib::TCard4         c4ObjId
                                , const TWindow&                wndSrc) :
    TCtrlNotify(wndSrc)
    , m_c4Action(kCIDLib::c4MaxCard)
    , m_c4ObjId(c4ObjId)
    , m_eEvent(eEvent)
{
}

TObjVChangeInfo::TObjVChangeInfo(const  tCIDCtrls::EObjVEvents  eEvent
                                , const tCIDLib::TCard4         c4ObjId
                                , const tCIDLib::TCard4         c4Action
                                , const TWindow&                wndSrc) :
    TCtrlNotify(wndSrc)
    , m_c4Action(c4Action)
    , m_c4ObjId(c4ObjId)
    , m_eEvent(eEvent)
{
}

TObjVChangeInfo::TObjVChangeInfo(const TObjVChangeInfo& wnotSrc) :

    TCtrlNotify(wnotSrc)
    , m_c4Action(wnotSrc.m_c4Action)
    , m_c4ObjId(wnotSrc.m_c4ObjId)
    , m_eEvent(wnotSrc.m_eEvent)
{
}

TObjVChangeInfo::~TObjVChangeInfo()
{
}


// ---------------------------------------------------------------------------
//  TObjVChangeInfo: Public operators
// ---------------------------------------------------------------------------
TObjVChangeInfo& TObjVChangeInfo::operator=(const TObjVChangeInfo& wnotSrc)
{
    if (this != &wnotSrc)
    {
        TParent::operator=(wnotSrc);

        m_c4Action = wnotSrc.m_c4Action;
        m_c4ObjId = wnotSrc.m_c4ObjId;
        m_eEvent = wnotSrc.m_eEvent;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TObjVChangeInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TObjVChangeInfo::c4Action() const
{
    return m_c4Action;
}


tCIDLib::TCard4 TObjVChangeInfo::c4ObjId() const
{
    return m_c4ObjId;
}


tCIDCtrls::EObjVEvents TObjVChangeInfo::eEvent() const
{
    return m_eEvent;
}




// ---------------------------------------------------------------------------
//  CLASS: TObjView
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TObjView: Public, static data members
// ---------------------------------------------------------------------------
const TNotificationId   TObjView::nidObjViewChange(L"ObjViewChangeId");


// ---------------------------------------------------------------------------
// TObjView: Constructors and Destructor
// ---------------------------------------------------------------------------
TObjView::TObjView() :

    TStdCtrlWnd()
    , m_eObjVStyles(tCIDCtrls::EObjVStyles::None)
    , m_menuAction(L"object view")
{
}

TObjView::~TObjView()
{
}


// ---------------------------------------------------------------------------
// TCalendar: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TObjView::InitFromDesc( const   TWindow&                wndParent
                        , const TDlgItem&               dlgiSrc
                        , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EObjVStyles eObjVStyles = tCIDCtrls::EObjVStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild
    );

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Group))
        eStyles |= tCIDCtrls::EWndStyles::Group;

    // We don't have any columns yet, so just pass empty ones
    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , eStyles
        , eObjVStyles
    );
}



tCIDLib::TVoid TObjView::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_Group);
}



// ---------------------------------------------------------------------------
// TObjView: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Add an item to the view
//
tCIDLib::TVoid
TObjView::AddItem(  const   tCIDLib::TCard4 c4ImgIndex
                    , const TString&        strText
                    , const tCIDLib::TCard4 c4Id
                    , const tCIDLib::TCard4 c4At)
{
    // Get our window handle, using the getter that throws if not valid
    tCIDCtrls::TWndHandle hwndUs = hwndSafe();

    // Loop through the values and load them up
    LVITEM lviCur = {0};
    lviCur.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
    lviCur.stateMask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;

    // Add the item first
    if (c4At == kCIDLib::c4MaxCard)
        lviCur.iItem = ::SendMessage(hwndUs, LVM_GETITEMCOUNT, 0, 0);
    else
        lviCur.iItem = c4At;

    lviCur.iSubItem = 0;
    lviCur.pszText = (LPWSTR)strText.pszBuffer();
    lviCur.iImage = c4ImgIndex;
    lviCur.lParam = c4Id;

    // And now set lset's add it
    tCIDCtrls::TMsgResult mresIndex = ::SendMessage
    (
        hwndUs, LVM_INSERTITEM, 0, tCIDCtrls::TLParam(&lviCur)
    );

    if (mresIndex < 0)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcObjV_AddItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(widThis())
        );
    }
}


//
//  Set up our styles and call our parent to create us. We have one that takes an
//  images list and another that doesn't, which we need for dialog box driven creation.
//
tCIDLib::TVoid
TObjView::Create(const  TWindow&                wndParent
                , const tCIDCtrls::TWndId       widThis
                , const TArea&                  areaInit
                , const tCIDCtrls::EWndStyles   eStyles
                , const TImageList&             imglNormal
                , const tCIDCtrls::EObjVStyles  eObjVStyles)
{
    //
    //  Has to be a child window, plus any other styles we enforce for this
    //  type of window.
    //
    tCIDLib::TCard4 c4Styles = LVS_ICON | LVS_SHOWSELALWAYS;

    if (!tCIDLib::bAllBitsOn(eObjVStyles, tCIDCtrls::EObjVStyles::MultiSel))
        c4Styles |= LVS_SINGLESEL;

    // Store our class specific styles
    m_eObjVStyles = eObjVStyles;

    // And create the list view
    CreateWnd
    (
        wndParent.hwndThis()
        , WC_LISTVIEW
        , kCIDLib::pszEmptyZStr
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles(c4Styles)
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );

    // All standard controls must be subclassed
    SetSubclass();

    // Duplicate the image list handle
    tCIDCtrls::TImgListHandle himglNew = ::ImageList_Duplicate
    (
        (HIMAGELIST)imglNormal.m_himglThis
    );

    if (!himglNew)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcImgList_DupFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    //
    //  It worked so set the handle on us. The tree control destroys the image list when
    //  it is destroyed, unless you explicitly tell it otherwise, so we don't have to worry
    //  about cleaning this up.
    //
    ::SendMessage(hwndThis(), LVM_SETIMAGELIST, LVSIL_NORMAL, tCIDCtrls::TLParam(himglNew));
}


tCIDLib::TVoid
TObjView::Create(const  TWindow&                wndParent
                , const tCIDCtrls::TWndId       widThis
                , const TArea&                  areaInit
                , const tCIDCtrls::EWndStyles   eStyles
                , const tCIDCtrls::EObjVStyles  eObjVStyles)
{
    //
    //  Has to be a child window, plus any other styles we enforce for this
    //  type of window.
    //
    tCIDLib::TCard4 c4Styles = LVS_ICON | LVS_SHOWSELALWAYS;

    if (!tCIDLib::bAllBitsOn(eObjVStyles, tCIDCtrls::EObjVStyles::MultiSel))
        c4Styles |= LVS_SINGLESEL;

    // Store our class specific styles
    m_eObjVStyles = eObjVStyles;

    // And create the list view
    CreateWnd
    (
        wndParent.hwndThis()
        , WC_LISTVIEW
        , L""
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles(c4Styles)
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );

    // All standard controls must be subclassed
    SetSubclass();
}


// Load up an action menu
tCIDLib::TVoid
TObjView::SetActionMenu(TGUIFacility& facSrc, const tCIDLib::TResId ridMenu)
{
    if (!m_menuAction.bIsValid())
        m_menuAction.Create(facSrc, ridMenu);
    else
        m_menuAction.LoadFromResource(facSrc, ridMenu);
}


// ---------------------------------------------------------------------------
//  TObjView: Protected, inherited methods
// ---------------------------------------------------------------------------

// Handle notifications reflected back to us
tCIDLib::TBoolean
TObjView::bNotReflect(          TWindow&                wndTar
                        , const tCIDCtrls::TWndMsg      wmsgMsg
                        , const tCIDCtrls::TWParam      wParam
                        , const tCIDCtrls::TLParam      lParam
                        ,       tCIDCtrls::TMsgResult&  mresResult)
{
    if (wmsgMsg == WM_COMMAND)
    {
        // Get the type. The source is us, so we don't care about that
        const tCIDLib::TCard4 c4Type = (wParam & 0xFFFF0000) >> 16;

        if (c4Type == LVN_DELETEALLITEMS)
        {
            // Send a cleared notifiation
            SendObjVEvent(tCIDCtrls::EObjVEvents::Cleared, 0);
            return kCIDLib::True;
        }
    }
     else if (wmsgMsg == WM_NOTIFY)
    {
        // Get a pointer to the notification header
        NMHDR* pNotHdr = (NMHDR*)lParam;
        if (pNotHdr->code == NM_DBLCLK)
        {
            // Send an invoked event
            NMITEMACTIVATE* pItem = (NMITEMACTIVATE*)lParam;
            if (pItem->iItem != -1)
                SendObjVEvent(tCIDCtrls::EObjVEvents::Invoked, pItem->iItem);
        }
         else if (pNotHdr->code == NM_RCLICK)
        {
            // If we have a valid menu, then pop it up
            if (m_menuAction.bIsValid())
            {
                const NMITEMACTIVATE* pActHdr = (NMITEMACTIVATE*)lParam;

                TPoint pntAt(pActHdr->ptAction.x, pActHdr->ptAction.y);
                ToScreenCoordinates(pntAt, pntAt);

                const tCIDLib::TCard4 c4Act = m_menuAction.c4Process
                (
                    *this, pntAt, tCIDLib::EVJustify::Bottom
                );

                if (c4Act && (c4Act != kCIDLib::c4MaxCard))
                {
                }

                return kCIDLib::True;
            }
        }
    }
    return kCIDLib::False;
}



// ---------------------------------------------------------------------------
// TObjView: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  A helper for derived classes to call when they want to send out a list
//  event. These are al done synchronously.
//
tCIDLib::TVoid
TObjView::SendObjVEvent(const   tCIDCtrls::EObjVEvents  eEvent
                        , const tCIDLib::TCard4         c4ObjId
                        , const tCIDLib::TCard4         c4Action)
{
    TObjVChangeInfo wnotSend(eEvent, c4ObjId, c4Action, *this);
    SendSyncNotify(wnotSend, TObjView::nidObjViewChange);
}


