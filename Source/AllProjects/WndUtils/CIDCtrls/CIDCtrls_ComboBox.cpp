//
// FILE NAME: CIDCtrls_ComboBox.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/12/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the combo box control.
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
AdvRTTIDecls(TComboBox, TListWnd)




// ---------------------------------------------------------------------------
//  CLASS: TComboBox
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TComboBox: Constructors and Destructor
// ---------------------------------------------------------------------------
TComboBox::TComboBox() :

    TListWnd(L"ComboBoxNotifyId")
    , m_c4CurSel(kCIDLib::c4MaxCard)
    , m_eComboStyles(tCIDCtrls::EComboStyles::None)
    , m_pwndEditor(nullptr)
{
}

TComboBox::~TComboBox()
{
}


// ---------------------------------------------------------------------------
// TComboBox: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  Select the item that has the indicated text. We just work in terms of other of our
//  our own methods. The index selection method we call will send the event.
//
//  If we dont' find it, and they say don't throw, we just select the 0th item if
//  there is one.
//
//  This just calls the index based selection and we update our cached current
//  selection there.
//
tCIDLib::TBoolean
TComboBox::bSelectByText(   const   TString&            strSel
                            ,       tCIDLib::TCard4&    c4FoundAt
                            , const tCIDLib::TBoolean   bForceEvent)
{
    c4FoundAt = c4FindByText(strSel);

    // If not found, just return false
    if (c4FoundAt == kCIDLib::c4MaxCard)
        return kCIDLib::False;

    SelectByIndex(c4FoundAt, bForceEvent);
    return kCIDLib::True;
}



// Return the item currently selected in the list
tCIDLib::TCard4 TComboBox::c4CurItem() const
{
    // Return our cached current item
    return m_c4CurSel;
}


// Look up an item by its exact text match
tCIDLib::TCard4 TComboBox::c4FindByText(const TString& strToFind) const
{
    // The list is sorted, so we have to find it by text
    const tCIDCtrls::TMsgResult mresIndex =
    (
        ::SendMessage
        (
            hwndSafe()
            , CB_FINDSTRINGEXACT
            , tCIDLib::TUInt(-1)
            , tCIDLib::TSInt(strToFind.pszBuffer())
        )
    );

    if (mresIndex == CB_ERR)
        return kCIDLib::c4MaxCard;

    return mresIndex;
}


// Return the id associated with the indicated index
tCIDLib::TCard4 TComboBox::c4IndexToId(const tCIDLib::TCard4 c4Index) const
{
    tCIDCtrls::TMsgResult mresIndex = ::SendMessage(hwndSafe(), CB_GETITEMDATA, c4Index, 0);
    if (mresIndex == CB_ERR)
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


// Get the count of items in the list
tCIDLib::TCard4 TComboBox::c4ItemCount() const
{
    tCIDCtrls::TMsgResult mresCnt = ::SendMessage(hwndSafe(), CB_GETCOUNT, 0, 0);
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


// Find the first item that starts with this text
tCIDLib::TCard4 TComboBox::c4StartsWith(const TString& strToFind) const
{
    // The list is sorted, so we have to find it by text
    const tCIDCtrls::TMsgResult mresIndex =
    (
        ::SendMessage
        (
            hwndSafe()
            , CB_FINDSTRING
            , tCIDLib::TUInt(-1)
            , tCIDLib::TSInt(strToFind.pszBuffer())
        )
    );

    if (mresIndex == CB_ERR)
        return kCIDLib::c4MaxCard;

    return mresIndex;
}


tCIDLib::TVoid
TComboBox::InitFromDesc(const   TWindow&                wndParent
                        , const TDlgItem&               dlgiSrc
                        , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EComboStyles eCBStyles = tCIDCtrls::EComboStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild
    );

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Group))
        eStyles |= tCIDCtrls::EWndStyles::Group;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_ReadOnly))
        eCBStyles |= tCIDCtrls::EComboStyles::ReadOnly;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Sort))
        eCBStyles |= tCIDCtrls::EComboStyles::Sort;

    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , eStyles
        , eCBStyles
    );

    // If there is cue text, then set it.
    if (!dlgiSrc.strCueText().bIsEmpty())
        SetCueText(dlgiSrc.strCueText());

    // Make sure the initial selection is reset
    m_c4CurSel = kCIDLib::c4MaxCard;
}


tCIDLib::TVoid TComboBox::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_Group);
    colToFill.objAdd(kCIDCtrls::strHint_ReadOnly);
    colToFill.objAdd(kCIDCtrls::strHint_Sort);
}


// Remove all items from the list
tCIDLib::TVoid TComboBox::RemoveAll(const tCIDLib::TBoolean bNoEvent)
{
    // If already empty, then do nothing
    tCIDCtrls::TMsgResult mresCnt = ::SendMessage(hwndSafe(), CB_GETCOUNT, 0, 0);
    if (mresCnt == CB_ERR)
    {
        // Make sure our cached selection is cleared, just in case
        m_c4CurSel = kCIDLib::c4MaxCard;
        return;
    }

    if (mresCnt != 0)
    {
        // Clear our current selection index and clear the list
        m_c4CurSel = kCIDLib::c4MaxCard;
        ::SendMessage(hwndSafe(), CB_RESETCONTENT, 0, 0);

        //
        //  Unless told not to, send a list cleared notification, since we don't get
        //  any notification to drive it otherwise. It's an async notification in this
        //  case.
        //
        if (!bNoEvent)
        {
            TWindow* pwndParent = pwndFindParent();
            if (pwndParent)
                SendListEvent(tCIDCtrls::EListEvents::Cleared, 0, 0, 0, kCIDLib::False);
        }
    }
}


// Remove the item at the indicated index
tCIDLib::TVoid
TComboBox::RemoveAt(const   tCIDLib::TCard4     c4Index
                    , const tCIDLib::TBoolean   bSelectAnother)
{
    tCIDCtrls::TMsgResult mresCnt = ::SendMessage(hwndSafe(), CB_DELETESTRING, c4Index, 0);
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

    // If it is the same as our cached current selection, clear our selection
    if (c4Index == m_c4CurSel)
        m_c4CurSel = kCIDLib::c4MaxCard;

    //
    //  If we removed it, then either seend a cleared event if we removed the last one,
    //  or if they asked us to automatically select another, do that.
    //
    if (mresCnt)
    {
        mresCnt--;
        if (bSelectAnother && mresCnt)
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
            SendListEvent(tCIDCtrls::EListEvents::Cleared, 0, 0, 0, kCIDLib::False);
    }
}


// Select the item at the indicated index
tCIDLib::TVoid
TComboBox::SelectByIndex(const  tCIDLib::TCard4     c4ToSel
                        , const tCIDLib::TBoolean   bForceEvent
                        , const tCIDLib::TBoolean   bThrowIfNot)
{
    // If we need to actually change the selection, then do it
    if (m_c4CurSel != c4ToSel)
    {
        if (::SendMessage(hwndSafe(), CB_SETCURSEL, c4ToSel, 0) == CB_ERR)
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

        //
        //  Store the new selection. We will not get a notification in this case because
        //  it's not user driven.
        //
        m_c4CurSel = c4ToSel;
    }

    //
    //  If they asked to force an event to be sent (it's normally not for non-user
    //  driven changes) then do so.
    //
    if (bForceEvent)
        SendListEvent(tCIDCtrls::EListEvents::SelChanged, c4ToSel, c4CurItemId());
}


// ---------------------------------------------------------------------------
// TComboBox: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If we are read only, we get the value of the selection in the list. Else we get
//  the value in the text box. In the later case, if a selection is made then text
//  box is updated, and if the user edits manually, it's there as well.
//
tCIDLib::TBoolean TComboBox::bQueryCurVal(TString& strToFill) const
{
    tCIDLib::TBoolean bRet = kCIDLib::False;

    if (tCIDLib::bAllBitsOn(m_eComboStyles, tCIDCtrls::EComboStyles::ReadOnly))
    {
        // There's a selection, so get that
        const tCIDLib::TCard4 c4Index = c4CurItem();
        if (c4Index != kCIDLib::c4MaxCard)
        {
            ValueAt(c4Index,strToFill);
            bRet = kCIDLib::True;
        }
    }
     else
    {
        // WE have to get the handle of the entry field
        COMBOBOXINFO Info;
        Info.cbSize = sizeof(Info);
        if (::SendMessage(hwndSafe(), CB_GETCOMBOBOXINFO, 0, tCIDCtrls::TLParam(&Info)))
        {
            // We got the info so let's query the text of the entry field
            tCIDCtrls::TMsgResult mresRet;
            mresRet = ::SendMessage(Info.hwndItem, WM_GETTEXTLENGTH, 0, 0);
            if (mresRet)
            {
                tCIDLib::TCh* pszBuf = new tCIDLib::TCh[mresRet + 2];
                pszBuf[0] = kCIDLib::chNull;
                TArrayJanitor<tCIDLib::TCh> janBuf(pszBuf);
                mresRet = ::SendMessage
                (
                    hwndSafe(), WM_GETTEXT, mresRet + 1, tCIDCtrls::TLParam(pszBuf)
                );
                strToFill.FromZStr(pszBuf, mresRet);
            }
             else
            {
                strToFill.Clear();
            }
            bRet = kCIDLib::True;
        }
    }
    return bRet;
}


// Add a single item to the list
tCIDLib::TCard4 TComboBox::c4AddItem(const TString& strInsert, const tCIDLib::TCard4 c4Id)
{
    tCIDCtrls::TMsgResult mresIndex = ::SendMessage
    (
        hwndSafe(), CB_ADDSTRING, 0, tCIDLib::TSInt(strInsert.pszBuffer())
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

    // Store the id
    UpdateIdAt(mresIndex, c4Id);
    return mresIndex;
}


//
//  Get the index associated with the indicated id, optionally throwing if not found. Or
//  we can return max card to indicate not found.
//
//  This guy doesn't have any mechanism to search by id, so we have to do it ourself.
//
tCIDLib::TCard4
TComboBox::c4IdToIndex(const tCIDLib::TCard4 c4Id, const tCIDLib::TBoolean bThrowIfNot) const
{
    tCIDLib::TCard4 c4Ret = kCIDLib::c4MaxCard;

    const tCIDLib::TCard4 c4Count = c4ItemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (c4IndexToId(c4Index) == c4Id)
        {
            c4Ret = c4Index;
            break;
        }
    }

    // If we never found it and they said to throw, do that
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


// Just set up the styles and call our parent to create the control
tCIDLib::TVoid
TComboBox::Create(  const   TWindow&                wndParent
                    , const tCIDCtrls::TWndId       widThis
                    , const TArea&                  areaInit
                    , const tCIDCtrls::EWndStyles   eStyles
                    , const tCIDCtrls::EComboStyles eComboStyles)
{
    // Set up any class specific styles
    tCIDLib::TCard4 c4Styles = CBS_AUTOHSCROLL | CBS_HASSTRINGS | WS_VSCROLL;

    if (tCIDLib::bAllBitsOn(eComboStyles, tCIDCtrls::EComboStyles::ReadOnly))
        c4Styles |= CBS_DROPDOWNLIST;
    else
        c4Styles |= CBS_DROPDOWN;

    if (tCIDLib::bAllBitsOn(eComboStyles, tCIDCtrls::EComboStyles::Sort))
        c4Styles |= CBS_SORT;

    // Store away our combo specific styles
    m_eComboStyles = eComboStyles;

    // Make sure our cached selection index is cleared
    m_c4CurSel = kCIDLib::c4MaxCard;

    // And now we can create the combo box
    CreateWnd
    (
        wndParent.hwndThis()
        , L"COMBOBOX"
        , kCIDLib::pszEmptyZStr
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles(c4Styles)
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );

    // All standard controls must be subclassed
    SetSubclass();

    //
    //  We need to make the embedded entry field a window as well, though we don't own it
    //  and so we tell our wrapper not to adopt the handle. We don't care what kind it is,
    //  we just need to be sure we see notifications from it and such.
    //
    //  It's only there if it's editable.
    //
    tCIDCtrls::TWndHandle hwndEditor = ::GetWindow(hwndThis(), GW_CHILD);
    if (hwndEditor != kCIDCtrls::hwndInvalid)
        m_pwndEditor = new TEntryField(hwndEditor);
}


// Insert a single item into the list at a specific index
tCIDLib::TVoid
TComboBox::InsertItem(  const   tCIDLib::TCard4 c4At
                        , const TString&        strInsert
                        , const tCIDLib::TCard4 c4Id)
{
    tCIDCtrls::TMsgResult mresIndex = ::SendMessage
    (
        hwndSafe(), CB_INSERTSTRING, c4At, tCIDLib::TSInt(strInsert.pszBuffer())
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

    // Store the id
    UpdateIdAt(mresIndex, c4Id);
}


//
//  Do a basic load of the combo box with strings.
//
tCIDLib::TVoid TComboBox::LoadList(const tCIDLib::TStrCollect& colLoadWith)
{
    // Remove any existing content, which will also clear our cached selection
    RemoveAll();

    TColCursor<TString>* pcursLoad = colLoadWith.pcursNew();
    TJanitor<TColCursor<TString>> janCurs(pcursLoad);

    if (pcursLoad->bIsValid())
    {
        // Get our window handle, using the getter that throws if not valid
        tCIDCtrls::TWndHandle hwndUs = hwndSafe();

        // Loop through the items and load them up
        for (; pcursLoad->bIsValid(); pcursLoad->bNext())
        {
            tCIDCtrls::TMsgResult mresIndex = ::SendMessage
            (
                hwndUs
                , CB_ADDSTRING
                , 0
                , tCIDLib::TSInt(pcursLoad->objRCur().pszBuffer())
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
        }
    }
}


// Load the combo with a list of values defined by a range of numbers
tCIDLib::TVoid
TComboBox::LoadNumRange(const tCIDLib::TInt4 i4First, const tCIDLib::TInt4 i4Last)
{
    // Remove any existing content, which will also clear our cached selection
    RemoveAll();

    // Just in case...
    if (i4First > i4Last)
        return;

    tCIDCtrls::TWndHandle hwndUs = hwndSafe();
    TString strFmt;
    for (tCIDLib::TInt4 i4Val = i4First; i4Val <= i4Last; i4Val++)
    {
        strFmt.SetFormatted(i4Val);

        tCIDCtrls::TMsgResult mresIndex = ::SendMessage
        (
            hwndUs, CB_ADDSTRING, 0, tCIDLib::TSInt(strFmt.pszBuffer())
        );

        if (mresIndex < 0)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcList_LoadNumRange
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TCardinal(widThis())
                , TInteger(i4First)
                , TInteger(i4Last)
                , TInteger(i4Val)
            );
        }
    }
}


//
//  A convenience to select an item by id. We just translate the id to an index and then
//  select it, both using our own public methods.
//
tCIDLib::TCard4
TComboBox::c4SelectById(const   tCIDLib::TCard4     c4IdToSel
                        , const tCIDLib::TBoolean   bForceEvent
                        , const tCIDLib::TBoolean   bThrowIfNot)
{
    const tCIDLib::TCard4 c4Index = c4IdToIndex(c4IdToSel, bThrowIfNot);
    if (c4Index != kCIDLib::c4MaxCard)
        SelectByIndex(c4Index, bForceEvent, bThrowIfNot);
    return c4Index;
}


//
//  Set our cue text. It'll only be useful if we are a non-read only type, since
//  otherwise we have no entry field.
//
tCIDLib::TVoid TComboBox::SetCueText(const TString& strToSet)
{
    ::SendMessage(hwndSafe(), CB_SETCUEBANNER, 0, tCIDLib::TSInt(strToSet.pszBuffer()));
}


// Set a per row id on an item in the list box
tCIDLib::TVoid
TComboBox::UpdateIdAt(const tCIDLib::TCard4 c4Index, const tCIDLib::TCard4 c4ToSet)
{
    tCIDCtrls::TMsgResult mresRes = ::SendMessage(hwndSafe(), CB_SETITEMDATA, c4Index, c4ToSet);
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
tCIDLib::TVoid TComboBox::ValueAt(const tCIDLib::TCard4 c4Index, TString& strToFill) const
{
    tCIDCtrls::TMsgResult mresRet;
    mresRet = ::SendMessage(hwndSafe(), CB_GETLBTEXTLEN, c4Index, 0);
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
        hwndSafe(), CB_GETLBTEXT, c4Index, tCIDCtrls::TLParam(pszBuf)
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
//  TComboBox: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TComboBox::bNotReflect(         TWindow&                wndTar
                        , const tCIDCtrls::TWndMsg      wmsgMsg
                        , const tCIDCtrls::TWParam      wParam
                        , const tCIDCtrls::TLParam      lParam
                        ,       tCIDCtrls::TMsgResult&  mresResult)
{
    // We expect a command type message, not notifity
    if (wmsgMsg != WM_COMMAND)
        return kCIDLib::False;

    // Get the type. We know the source is us, so we don't care about that
    const tCIDLib::TCard4 c4Type = (wParam & 0xFFFF0000) >> 16;

    // If they make a selection handle it and report it
    if (c4Type == CBN_SELENDOK)
    {
        // Get our newly selected item in the drop down list
        tCIDCtrls::TMsgResult mresSel = ::SendMessage(hwndSafe(), CB_GETCURSEL, 0, 0);
        if (mresSel != CB_ERR)
        {
            // Store this as our new selection
            m_c4CurSel = tCIDLib::TCard4(mresSel);

            // Get the id for this item
            const tCIDLib::TCard4 c4Id = ::SendMessage
            (
                hwndThis(), CB_GETITEMDATA, m_c4CurSel, 0
            );

            // And let's report this as a selection change
            SendListEvent(tCIDCtrls::EListEvents::SelChanged, m_c4CurSel, c4Id);
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TVoid TComboBox::Destroyed()
{
    // Clean up our pseudo entry field
    if (m_pwndEditor)
    {
        m_pwndEditor->Destroy();
        delete m_pwndEditor;
        m_pwndEditor = nullptr;
    }

    TParent::Destroyed();
}
