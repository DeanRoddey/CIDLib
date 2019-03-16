//
// FILE NAME: CIDCtrls_ListBox.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/07/2015
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
//  This file implements the
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
AdvRTTIDecls(TListBox, TListWnd)


// ---------------------------------------------------------------------------
//  CLASS: TListBoxOps
// PREFIX: lbops
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Constructors and destructors
// ---------------------------------------------------------------------------
TListBoxOps::TListBoxOps()
{
}

TListBoxOps::~TListBoxOps()
{
}



// ---------------------------------------------------------------------------
//  CLASS: TListBox
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TListBox: Constructors and Destructor
// ---------------------------------------------------------------------------
TListBox::TListBox() :

    TListWnd(L"ListBoxNotifyId")
    , m_eLBStyles(tCIDCtrls::ELBStyles::None)
{
}

TListBox::~TListBox()
{
}

// ---------------------------------------------------------------------------
// TListBox: Public, inherited methods
// ---------------------------------------------------------------------------

// Check our styles and see if we are multiselect
tCIDLib::TBoolean TListBox::bIsMultiSel() const
{
    return (GetWindowLong(hwndSafe(), GWL_STYLE) & LBS_MULTIPLESEL) != 0;
}


//
//  Move an item up or down in the list. We optionally move the selection along with
//  it to keep the same item selected.
//
tCIDLib::TBoolean TListBox::bMoveDn(const tCIDLib::TBoolean bMoveSelection)
{
    const tCIDLib::TCard4 c4CurInd = c4CurItem();
    const tCIDLib::TCard4 c4Count = c4ItemCount();

    // Have to have at least 2 items or it can't be possible
    if (c4Count < 2)
        return kCIDLib::False;

    // If already at the end in the requested direction, can't do it
    if (c4CurInd + 1 >= c4Count)
        return kCIDLib::False;

    // Get the text and id of the current item
    TString strText;
    ValueAt(c4CurInd, strText);
    const tCIDLib::TCard4 c4Id = c4IndexToId(c4CurInd);

    // Now remove it
    RemoveAt(c4CurInd);

    // And put it back at the new position, restoring the saved id
    tCIDLib::TCard4 c4NewPos = c4CurInd;
    c4NewPos++;
    InsertItem(strText, c4NewPos, c4Id);

    // Select it again at the new position to keep it selected, if asked
    if (bMoveSelection)
        SelectByIndex(c4NewPos, kCIDLib::False);

    return kCIDLib::True;
}


tCIDLib::TBoolean TListBox::bMoveUp(const tCIDLib::TBoolean bMoveSelection)
{
    const tCIDLib::TCard4 c4CurInd = c4CurItem();
    const tCIDLib::TCard4 c4Count = c4ItemCount();

    // Have to have at least 2 items or it can't be possible
    if (c4Count < 2)
        return kCIDLib::False;

    // If already at the end in the requested direction, can't do it
    if (!c4CurInd)
        return kCIDLib::False;

    // Get the text and id of the current item
    TString strText;
    ValueAt(c4CurInd, strText);
    const tCIDLib::TCard4 c4Id = c4IndexToId(c4CurInd);

    // Now remove it
    RemoveAt(c4CurInd);

    // And put it back at the new position, restoring the original id as well
    tCIDLib::TCard4 c4NewPos = c4CurInd;
    c4NewPos--;
    InsertItem(strText, c4NewPos, c4Id);

    // Select it again at the new position to keep it selected, if requested
    if (bMoveSelection)
        SelectByIndex(c4NewPos, kCIDLib::False);

    return kCIDLib::True;
}

//
//  Select the item that has the indicated text. The index selection method we
//  call will send the event.
//
tCIDLib::TBoolean
TListBox::bSelectByText(const   TString&            strSel
                        ,       tCIDLib::TCard4&    c4FoundAt
                        , const tCIDLib::TBoolean   bForceEvent)
{
    c4FoundAt = c4FindByText(strSel);
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
tCIDLib::TCard4 TListBox::c4CurItem() const
{
    tCIDCtrls::TMsgResult mresIndex;
    if (bIsMultiSel())
        mresIndex = ::SendMessage(hwndSafe(), LB_GETCARETINDEX, 0, 0);
    else
        mresIndex = ::SendMessage(hwndSafe(), LB_GETCURSEL, 0, 0);

    if (mresIndex == LB_ERR)
        return kCIDLib::c4MaxCard;
    return mresIndex;
}


// Look up an item by its exact text match
tCIDLib::TCard4 TListBox::c4FindByText(const TString& strToFind) const
{
    // The list is sorted, so we have to find it by text
    const tCIDCtrls::TMsgResult mresIndex =
    (
        ::SendMessage
        (
            hwndSafe()
            , LB_FINDSTRINGEXACT
            , tCIDCtrls::TWParam(-1)
            , tCIDCtrls::TLParam(strToFind.pszBuffer())
        )
    );

    if (mresIndex == LB_ERR)
        return kCIDLib::c4MaxCard;

    return mresIndex;
}


// Return the id associated with the indicated index
tCIDLib::TCard4 TListBox::c4IndexToId(const tCIDLib::TCard4 c4Index) const
{
    tCIDCtrls::TMsgResult mresIndex = ::SendMessage(hwndSafe(), LB_GETITEMDATA, c4Index, 0);
    if (mresIndex < 0)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_GetItemId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4Index)
            , TCardinal(widThis())
        );
    }
    return tCIDLib::TCard4(mresIndex);
}


// Get teh count of items in the list
tCIDLib::TCard4 TListBox::c4ItemCount() const
{
    tCIDCtrls::TMsgResult mresCnt = ::SendMessage(hwndSafe(), LB_GETCOUNT, 0, 0);
    if (mresCnt < 0)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_GetCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(widThis())
        );
    }
    return mresCnt;
}


// Find the first item that starts with the passed text
tCIDLib::TCard4 TListBox::c4StartsWith(const TString& strToFind) const
{
    // The list is sorted, so we have to find it by text
    const tCIDCtrls::TMsgResult mresIndex =
    (
        ::SendMessage
        (
            hwndSafe()
            , LB_FINDSTRING
            , tCIDCtrls::TWParam(-1)
            , tCIDCtrls::TLParam(strToFind.pszBuffer())
        )
    );

    if (mresIndex == LB_ERR)
        return kCIDLib::c4MaxCard;
    return mresIndex;
}


// Initialize a list box from a dialog item description
tCIDLib::TVoid
TListBox::InitFromDesc( const   TWindow&                wndParent
                        , const TDlgItem&               dlgiSrc
                        , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::ELBStyles eLBStyles = tCIDCtrls::ELBStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDLib::eOREnumBits
    (
        tCIDCtrls::EWndStyles::VisTabChild, tCIDCtrls::EWndStyles::VScroll
    );
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Group))
        eStyles = tCIDLib::eOREnumBits(eStyles, tCIDCtrls::EWndStyles::Group);

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_MultiVal))
        eLBStyles = tCIDLib::eOREnumBits(eLBStyles, tCIDCtrls::ELBStyles::MultiSel);

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Sort))
        eLBStyles = tCIDLib::eOREnumBits(eLBStyles, tCIDCtrls::ELBStyles::Sorted);

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_ReadOnly))
    {
        eLBStyles = tCIDLib::eOREnumBits(eLBStyles, tCIDCtrls::ELBStyles::ReadOnly);
        eStyles = tCIDLib::eClearEnumBits(eStyles, tCIDCtrls::EWndStyles::TabStop);
    }

    if (eTheme == tCIDCtrls::EWndThemes::MainWnd)
    {
        eLBStyles = tCIDLib::eOREnumBits(eLBStyles, tCIDCtrls::ELBStyles::ScrollAlways);

        // Set a single line border unless told not to
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
            eStyles = tCIDLib::eOREnumBits(eStyles, tCIDCtrls::EWndStyles::Border);
    }
     else if (eTheme == tCIDCtrls::EWndThemes::DialogBox)
    {
        // Set a sunken border and ctrl fill background unless told not to
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
        {
            eExStyles = tCIDLib::eOREnumBits(eExStyles, tCIDCtrls::EExWndStyles::SunkenEdge);
            SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::DlgCtrlFill), kCIDLib::True);
        }
    }

    CreateListBox
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , eStyles
        , eLBStyles
        , eExStyles
    );
}


tCIDLib::TVoid TListBox::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);

    colToFill.objAdd(kCIDCtrls::strHint_Group);
    colToFill.objAdd(kCIDCtrls::strHint_MultiVal);
    colToFill.objAdd(kCIDCtrls::strHint_NoBorder);
    colToFill.objAdd(kCIDCtrls::strHint_ReadOnly);
    colToFill.objAdd(kCIDCtrls::strHint_Sort);
}


tCIDLib::TVoid TListBox::RemoveAll(const tCIDLib::TBoolean bNoEvent)
{
    // If not already empty, then empty it. If already empty, do nothing
    tCIDCtrls::TMsgResult mresCnt = ::SendMessage(hwndSafe(), LB_GETCOUNT, 0, 0);
    if (mresCnt == LB_ERR)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_RemoveAll
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(widThis())
        );
    }

    if (mresCnt != 0)
    {
        ::SendMessage(hwndSafe(), LB_RESETCONTENT, 0, 0);

        // Send a list cleared notification, since we don't get anything to drive it.
        if (!bNoEvent)
        {
            TWindow* pwndParent = pwndFindParent();
            if (pwndParent)
                SendListEvent(tCIDCtrls::EListEvents::Cleared, 0, 0, 0);
        }
    }
}


// Remove the item at the indicated index
tCIDLib::TVoid
TListBox::RemoveAt( const   tCIDLib::TCard4     c4Index
                    , const tCIDLib::TBoolean   bSelectAnother)
{
    tCIDCtrls::TMsgResult mresCnt = ::SendMessage(hwndSafe(), LB_DELETESTRING, c4Index, 0);
    if (mresCnt < 0)
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

    //
    //  If we emptied it, send a notification, since we won't get any notification to
    //  drive it. If we didn't and they asked us to, select another item.
    //
    if (mresCnt)
    {
        mresCnt--;
        if (bSelectAnother && !bIsMultiSel())
        {
            //
            //  If we have enough, just select the one that fell down into this one's
            //  place. Else move back.
            //
            if (c4Index < tCIDLib::TCard4(mresCnt))
                SelectByIndex(c4Index, kCIDLib::True);
            else
                SelectByIndex(c4Index - 1, kCIDLib::True);
        }
    }
     else
    {
        TWindow* pwndParent = pwndFindParent();
        if (pwndParent)
            SendListEvent(tCIDCtrls::EListEvents::Cleared, 0, 0, 0);
    }
}


// Select the item at the indicated index
tCIDLib::TVoid
TListBox::SelectByIndex(const   tCIDLib::TCard4     c4ToSel
                        , const tCIDLib::TBoolean   bForceEvent
                        , const tCIDLib::TBoolean   bThrowIfNot)
{
    // Get the current selection
    const tCIDLib::TCard4 c4PrevInd = c4CurItem();

    if (c4PrevInd != c4ToSel)
    {
        if (::SendMessage(hwndSafe(), LB_SETCURSEL, c4ToSel, 0) == LB_ERR)
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

    // If ask to force an event, then do so
    if (bForceEvent)
        SendListEvent(tCIDCtrls::EListEvents::SelChanged, c4ToSel, 0);
}



// ---------------------------------------------------------------------------
// TListBox: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Returns true if the indicated item is marked. This is really only useful for
//  multi-select lists, but if it is single select this really just indicates if the
//  passed index is the currently selection or not.
//
tCIDLib::TBoolean TListBox::bIsMarked(const tCIDLib::TCard4 c4Index) const
{
    const tCIDCtrls::TMsgResult mresIndex = ::SendMessage
    (
        hwndThis(), LB_GETSEL, c4Index, 0
    );

    if (mresIndex < 0)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_GetIsMarked
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4Index)
            , TCardinal(widThis())
        );
    }

    // If not zero, it has the selection state
    return mresIndex != 0;
}


// Add a single item to the end of the list
tCIDLib::TCard4
TListBox::c4AddItem(const TString& strToAdd, const tCIDLib::TCard4 c4ItemId)
{
    //
    //  The item id cannot be max card because that would show up as an error
    //  status when we query the id back later, due to a shortcomingin how the
    //  list box interface works.
    //
    if (c4ItemId == kCIDLib::c4MaxCard)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_BadIItemId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(widThis())
        );
    }

    tCIDCtrls::TMsgResult mresIndex = ::SendMessage
    (
        hwndSafe(), LB_ADDSTRING, 0, tCIDCtrls::TLParam(strToAdd.pszBuffer())
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

    // Store the item id
    if (::SendMessage(hwndThis(), LB_SETITEMDATA, mresIndex, c4ItemId) == LB_ERR)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_SetItemId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TInteger(mresIndex)
            , TCardinal(widThis())
        );
    }

    // If the auto-add bottom style is set, keep us bottom scrolled
    if (tCIDLib::bAllBitsOn(m_eLBStyles, tCIDCtrls::ELBStyles::AutoAddBottom))
        ::SendMessage(hwndThis(), LB_SETTOPINDEX, c4ItemCount(), 0);

    return tCIDLib::TCard4(mresIndex);
}


//
//  Update the text of the indicated item index. THis one is tricky, because there
//  is no command to do this. We have to remove this one, add it back with the new
//  text, and then get any previous item data set back on it.
//
//  It could go back at a new index if this list is sorted, so we return the new
//  insertion index.
//
tCIDLib::TCard4
TListBox::c4SetText(const tCIDLib::TCard4 c4Index, const TString& strToSet)
{
    // Save the item data for this guy first
    const tCIDLib::TCard4 c4SaveData = c4IndexToId(c4Index);

    // Remember if it was selected
    const tCIDLib::TCard4 c4OrgSel = c4CurItem();
    const tCIDLib::TBoolean bWasSel(c4Index == c4OrgSel);

    // Remove this guy
    RemoveAt(c4Index);

    // Disable drawing during this to make it smoother
    TWndPaintJanitor janPaint(this);

    //
    //  Add it back, and we have to get the new index since if this guy is sorted
    //  it won't be the same as the original. So, if sorted, we do add string, and let
    //  it go whereever, and the return value is what we return. Else we insert at
    //  the specific index and return that.
    //
    tCIDCtrls::TMsgResult mresIndex;
    tCIDLib::TCard4 c4RetVal = c4Index;
    if (tCIDLib::bAllBitsOn(m_eLBStyles, tCIDCtrls::ELBStyles::Sorted))
    {
        mresIndex = ::SendMessage
        (
            hwndThis(), LB_ADDSTRING, 0, tCIDCtrls::TLParam(strToSet.pszBuffer())
        );
    }
     else
    {
        mresIndex = ::SendMessage
        (
            hwndThis(), LB_INSERTSTRING, c4Index, tCIDCtrls::TLParam(strToSet.pszBuffer())
        );
    }

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
    c4RetVal = mresIndex;

    // Put back the saved item data
    SetItemData(c4RetVal, c4SaveData);

    // And reselect it if it was originally. Force an event if the index changed
    if (bWasSel)
        SelectByIndex(mresIndex, tCIDLib::TCard4(mresIndex) != c4OrgSel);

    return c4RetVal;
}


//
//  Set up our styles and call our parent to create us.
//
tCIDLib::TVoid
TListBox::CreateListBox(const   TWindow&                wndParent
                        , const tCIDCtrls::TWndId       widThis
                        , const TArea&                  areaInit
                        , const tCIDCtrls::EWndStyles   eStyles
                        , const tCIDCtrls::ELBStyles    eLBStyles
                        , const tCIDCtrls::EExWndStyles eExStyles)
{
    tCIDLib::TCard4 c4Styles = LBS_HASSTRINGS | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT;

    // And add in any of the list box specific ones
    if (tCIDLib::bAllBitsOn(eLBStyles, tCIDCtrls::ELBStyles::ScrollAlways))
        c4Styles |= LBS_DISABLENOSCROLL;

    if (tCIDLib::bAllBitsOn(eLBStyles, tCIDCtrls::ELBStyles::MultiSel))
        c4Styles |= LBS_EXTENDEDSEL | LBS_MULTIPLESEL;

    if (tCIDLib::bAllBitsOn(eLBStyles, tCIDCtrls::ELBStyles::ReadOnly))
        c4Styles |= LBS_NOSEL;

    if (tCIDLib::bAllBitsOn(eLBStyles, tCIDCtrls::ELBStyles::Sorted))
        c4Styles |= LBS_SORT;

    if (tCIDLib::bAllBitsOn(eLBStyles, tCIDCtrls::ELBStyles::CustDraw))
        c4Styles |= LBS_OWNERDRAWFIXED;

    // Store our LB specific styles away
    m_eLBStyles = eLBStyles;

    // And now we can create the list box
    CreateWnd
    (
        wndParent.hwndThis()
        , L"LISTBOX"
        , kCIDLib::pszEmptyZStr
        , areaInit
        , tCIDLib::eOREnumBits(eStyles, tCIDCtrls::EWndStyles(c4Styles))
        , eExStyles
        , widThis
    );

    // All standard controls must be subclassed
    SetSubclass();
}


// Only valid for multi-select, returns a list of all selected indices
tCIDLib::TCard4
TListBox::c4QuerySelList(tCIDLib::TCardList& fcolToFill) const
{
    // Clear it first, no matter what
    fcolToFill.RemoveAll();

    // Get the count of selected items so we can allocate space
    tCIDCtrls::TMsgResult mresCnt = ::SendMessage(hwndSafe(), LB_GETSELCOUNT, 0, 0);
    if (mresCnt < 0)
        CheckMultiSelect(CID_LINE);

    // If none, do the light weight scenario
    if (!mresCnt)
        return 0;

    // Allocate a temp buffer and get the values
    tCIDLib::TUInt* puList = new tCIDLib::TUInt[mresCnt];
    TArrayJanitor<tCIDLib::TUInt> janList(puList);

    tCIDCtrls::TMsgResult mresRes = ::SendMessage
    (
        hwndSafe(), LB_GETSELITEMS, mresCnt, tCIDCtrls::TLParam(puList)
    );
    if (mresRes < 0)
        CheckMultiSelect(CID_LINE);

    //
    //  Copy the values to the caller's vector. To avoid issues with element
    //  sizes on differetn OS versions, we do a loop and assign them singly.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < tCIDLib::TCard4(mresCnt); c4Index++)
        fcolToFill.c4AddElement(puList[c4Index]);

    return mresCnt;
}


tCIDLib::TVoid
TListBox::InsertItem(const  TString&            strToAdd
                    , const tCIDLib::TCard4     c4At
                    , const tCIDLib::TCard4     c4ItemId)
{
    //
    //  The item id cannot be max card because that would show up as an error
    //  status when we query the id back later, due to a shortcomingin how the
    //  list box interface works.
    //
    if (c4ItemId == kCIDLib::c4MaxCard)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_BadIItemId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(widThis())
        );
    }

    tCIDCtrls::TMsgResult mresIndex = ::SendMessage
    (
        hwndSafe(), LB_INSERTSTRING, c4At, tCIDCtrls::TLParam(strToAdd.pszBuffer())
    );

    if (mresIndex < 0)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_InsertItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4At)
            , TCardinal(widThis())
        );
    }

    // Store the item id
    if (::SendMessage(hwndThis(), LB_SETITEMDATA, mresIndex, c4ItemId) == LB_ERR)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_SetItemId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TInteger(mresIndex)
            , TCardinal(widThis())
        );
    }

    // If the auto-add bottom style is set, keep us bottom scrolled
    if (tCIDLib::bAllBitsOn(m_eLBStyles, tCIDCtrls::ELBStyles::AutoAddBottom))
        ::SendMessage(hwndThis(), LB_SETTOPINDEX, c4ItemCount(), 0);
}


//
//  Do a basic load of the list box with just strings. There's no means to map back
//  to the original values other than looking up the text of the item selected within
//  whatever list provided the original set of string values.
//
tCIDLib::TVoid TListBox::LoadList(const tCIDLib::TStrCollect& colLoadWith)
{
    // Remove any existing content
    RemoveAll();

    TColCursor<TString>* pcursLoad = colLoadWith.pcursNew();
    TJanitor<TColCursor<TString>> janCurs(pcursLoad);

    if (pcursLoad->bIsValid())
    {
        // Get our window handle, using the getter that throws if not valid
        tCIDCtrls::TWndHandle hwndUs = hwndSafe();

        // Loop through the items and load them up
        for (; pcursLoad->bIsValid(); pcursLoad->bNext())
            c4AddItem(pcursLoad->objRCur());
    }
}


//
//  Do a load of the list with strings and ids. They provide us with a list of
//  stringid object, so we have a string and presumably an id to set one ach item,
//  though they may not use them.
//
tCIDLib::TVoid TListBox::LoadList(const tCIDLib::TStrIdList& colLoadWith)
{
    // Remove any existing content
    RemoveAll();

    TColCursor<TStringId>* pcursLoad = colLoadWith.pcursNew();
    TJanitor<TColCursor<TStringId>> janCurs(pcursLoad);

    if (pcursLoad->bIsValid())
    {
        // Get our window handle, using the getter that throws if not valid
        tCIDCtrls::TWndHandle hwndUs = hwndSafe();

        // Loop through the items and load them up
        for (; pcursLoad->bIsValid(); pcursLoad->bNext())
        {
            const TStringId& stridCur = pcursLoad->objRCur();
            c4AddItem(stridCur.strText(), stridCur.c4Id());
        }
    }
}


//
//  Load our list using a provided list box ops object. We use it to iterate
//  the values available and store them.
//
tCIDLib::TVoid TListBox::LoadList(TListBoxOps& lbopsToUse)
{
    // Remove any existing content
    RemoveAll();

    const tCIDLib::TCard4 c4Count = lbopsToUse.c4RowCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const tCIDLib::TCard4 c4CurId = lbopsToUse.c4RowId(c4Index);
        const TString& strCurText = lbopsToUse.strRowText(c4Index);
        c4AddItem(strCurText, c4CurId);
    }
}


// Set the mark attribute on all items
tCIDLib::TVoid TListBox::MarkAll(const tCIDLib::TBoolean bState)
{
    // Make sure we are multi-select
    CheckMultiSelect(CID_LINE);
    ::SendMessage(hwndSafe(), LB_SETSEL, bState ? 1 : 0, -1);
}


// Set the mark attribute on an item in a multi-select list
tCIDLib::TVoid
TListBox::MarkItem( const   tCIDLib::TCard4     c4Index
                    , const tCIDLib::TBoolean   bState)
{
    // Make sure we are multi-select
    CheckMultiSelect(CID_LINE);

    ::SendMessage(hwndSafe(), LB_SETSEL, bState ? 1 : 0, tCIDCtrls::TLParam(c4Index));
}


//
//  Call other methods to get the current selection and then the text from that index
//  and return that string.
//
TString TListBox::strCurSelection() const
{
    const tCIDLib::TCard4 c4Index = c4CurItem();
    if (c4Index == kCIDLib::c4MaxCard)
        return TString::strEmpty();

    TString strRet;
    ValueAt(c4Index, strRet);
    return strRet;
}


// Set a user data value on an item in the list box
tCIDLib::TVoid
TListBox::SetItemData(const tCIDLib::TCard4 c4Index, const tCIDLib::TCard4 c4ToSet)
{
    const tCIDCtrls::TMsgResult mresRes = ::SendMessage
    (
        hwndSafe(), LB_SETITEMDATA, c4Index, c4ToSet
    );

    if (mresRes < 0)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcList_SetItemId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4Index)
            , TCardinal(widThis())
        );
    }
}


// Get the text from the item at the indicated index
tCIDLib::TVoid
TListBox::ValueAt(  const   tCIDLib::TCard4 c4Index
                    ,       TString&        strToFill) const
{
    tCIDCtrls::TMsgResult mresRet;
    mresRet = ::SendMessage(hwndSafe(), LB_GETTEXTLEN, c4Index, 0);
    if (mresRet < 0)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcGen_GetText
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(widThis())
        );
    }

    tCIDLib::TCh* pszBuf = new tCIDLib::TCh[mresRet + 1];
    pszBuf[0] = kCIDLib::chNull;
    TArrayJanitor<tCIDLib::TCh> janBuf(pszBuf);

    mresRet = ::SendMessage
    (
        hwndSafe(), LB_GETTEXT, c4Index, tCIDCtrls::TLParam(pszBuf)
    );
    if (mresRet < 0)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcGen_GetText
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(widThis())
        );
    }

    strToFill.FromZStr(pszBuf, mresRet);
}



// ---------------------------------------------------------------------------
//  TListBox: Protected, inherited methods
// ---------------------------------------------------------------------------

// We intercept some keys to report to listeners
tCIDLib::TBoolean
TListBox::bExtKey(  const   tCIDCtrls::EExtKeys eKey
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
        // If there's a current selection, send an event
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


tCIDLib::TBoolean
TListBox::bNotReflect(          TWindow&                wndTar
                        , const tCIDCtrls::TWndMsg      wmsgMsg
                        , const tCIDCtrls::TWParam      wParam
                        , const tCIDCtrls::TLParam      lParam
                        ,       tCIDCtrls::TMsgResult&  mresResult)
{
    // We expect a command type message, not notifity
    if (wmsgMsg != WM_COMMAND)
        return kCIDLib::False;

    // Get the type. The source is us, so we don't care about that
    const tCIDLib::TCard4 c4Type = (wParam & 0xFFFF0000) >> 16;

    if ((c4Type == LBN_SELCHANGE) || (c4Type == LBN_DBLCLK))
    {
        tCIDCtrls::TMsgResult mresSel;
        if (GetWindowLong(hwndSafe(), GWL_STYLE) & LBS_MULTIPLESEL)
            mresSel = ::SendMessage(hwndThis(), LB_GETCARETINDEX, 0, 0);
        else
            mresSel = ::SendMessage(hwndSafe(), LB_GETCURSEL, 0, 0);

        tCIDLib::TCard4 c4Sel;
        if (mresSel == LB_ERR)
            c4Sel = kCIDLib::c4MaxCard;
        else
            c4Sel = tCIDLib::TCard4(mresSel);

        tCIDCtrls::EListEvents eEvent;
        if (c4Type == LBN_DBLCLK)
            eEvent = tCIDCtrls::EListEvents::Invoked;
        else
            eEvent = tCIDCtrls::EListEvents::SelChanged;

        SendListEvent(eEvent, c4Sel, 0);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}



// ---------------------------------------------------------------------------
// TListBox: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Check if we are multi-select and throw if not
tCIDLib::TVoid TListBox::CheckMultiSelect(const tCIDLib::TCard4 c4Line) const
{
    if (!bIsMultiSel())
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , c4Line
            , kCtrlsErrs::errcList_NotMultiSelect
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(widThis())
        );
    }
}


