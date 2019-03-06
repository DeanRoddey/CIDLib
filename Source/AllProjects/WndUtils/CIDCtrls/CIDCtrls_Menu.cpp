//
// FILE NAME: CIDCtrls_Menu.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/09/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the check box control.
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
RTTIDecls(TMenu, TObject)
RTTIDecls(TPopupMenu, TPopupMenu)
RTTIDecls(TMenuBar, TMenu)
RTTIDecls(TSubMenu, TMenu)



// ---------------------------------------------------------------------------
//  CLASS: TMenu
// PREFIX: menu
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMenu: Destructor
// ---------------------------------------------------------------------------
TMenu::~TMenu()
{
    //
    //  If it's a menu bar type that's not been assigned, or it's a popup, then
    //  detroy the menu if not already. If it's a submenu, then the parent menu
    //  owns the handle.
    //
    if (m_hmenuThis != kCIDCtrls::hmenuInvalid)
    {
        if (((m_eType == EMenuTypes::MenuBar) && (m_hwndAssoc == kCIDCtrls::hwndInvalid))
        ||  (m_eType == EMenuTypes::PopUp))
        {
            ::DestroyMenu(m_hmenuThis);
        }
        m_hmenuThis = kCIDCtrls::hmenuInvalid;
    }
}


// ---------------------------------------------------------------------------
//  TMenu: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TMenu::AddActionItem(const  tCIDLib::TResId     ridTarMenu
                    , const TString&            strName
                    , const TString&            strText
                    , const tCIDLib::TCard4     ridNew
                    , const tCIDLib::TBoolean   bEnabled)
{
    // Find the target menu
    tCIDCtrls::TMenuHandle hmenuActual = hmenuFindSubMenu(ridTarMenu);

    // Now all the static method to do the work
    AddActItem
    (
        hmenuActual
        , strName
        , strText
        , ridNew
        , bEnabled
    );

    //
    //  Make sure it reports by position. We do this upon menu creation, but this
    //  is being done after the fact.
    //
    //  Only matters for menu bar types. We set up popups to only return a value
    //  not notify, and sub-menus take on their parent's setting.
    //
    if (m_eType == EMenuTypes::MenuBar)
    {
        MENUINFO Info = {0};
        Info.cbSize = sizeof(Info);
        Info.fMask = MIM_APPLYTOSUBMENUS | MIM_STYLE;
        Info.dwStyle = MNS_NOTIFYBYPOS;
        if (!::SetMenuInfo(hmenuActual, &Info))
        {
        }
    }
}

tCIDLib::TVoid
TMenu::AddActionItem(const  TString&            strName
                    , const TString&            strText
                    , const tCIDLib::TCard4     ridNew
                    , const tCIDLib::TBoolean   bEnabled)
{
    AddActionItem(kCIDLib::c4MaxCard, strName, strText, ridNew, bEnabled);
}


tCIDLib::TVoid
TMenu::AddSubMenu(  const   TString&            strName
                    , const TString&            strText
                    , const tCIDLib::TCard4     ridNew)
{
    tCIDCtrls::TMenuHandle hmenuNew = hmenuAddSub(m_hmenuThis, strName, strText, ridNew);

    //
    //  Make sure it reports by position. We do this upon menu creation, but this
    //  is being done after the fact.
    //
    if (m_eType == EMenuTypes::MenuBar)
    {
        MENUINFO Info = {0};
        Info.cbSize = sizeof(Info);
        Info.fMask = MIM_APPLYTOSUBMENUS | MIM_STYLE;
        Info.dwStyle = MNS_NOTIFYBYPOS;
        if (!::SetMenuInfo(hmenuNew, &Info))
        {
        }
    }

}


tCIDLib::TVoid TMenu::AddSeparator()
{
    // Get the current count so that we can add at the end
    const tCIDLib::TCard4 c4Count = ::GetMenuItemCount(m_hmenuThis);

    // Fill in an info object with the info we need
    MENUITEMINFO AddInfo = {0};
    AddInfo.cbSize = sizeof(MENUITEMINFO);
    AddInfo.fMask = MIIM_FTYPE;
    AddInfo.fType = MFT_SEPARATOR;

    if (!::InsertMenuItem(m_hmenuThis, c4Count, 1, &AddInfo))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcMenu_AddSeparator
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }
}


// Returns whether the indicated menu item is checked or not
tCIDLib::TBoolean TMenu::bIsItemChecked(const tCIDLib::TResId ridItem) const
{
    CheckHandle();

    // Find the actual item using the local recursive searcher
    tCIDCtrls::TMenuHandle hmenuActual;
    const tCIDLib::TCard4 c4Offset = c4FindItem(ridItem, hmenuActual);


    // Get it's current check state and then flip it the other way and set it back
    MENUITEMINFO Info = {0};
    Info.cbSize = sizeof(MENUITEMINFO);
    Info.fMask = MIIM_STATE;

    // Get the target item's info
    if (!::GetMenuItemInfo(hmenuActual, c4Offset, TRUE, &Info))
        ThrowGetInfo(ridItem, CID_LINE);

    return (Info.fState & MFS_CHECKED) != 0;
}


// Return true if we are a valid menu
tCIDLib::TBoolean TMenu::bIsValid() const
{
    return (m_hmenuThis != kCIDCtrls::hmenuInvalid) && (::IsMenu(m_hmenuThis) != 0);
}


// Clean all of the items and sub-items out of a target menu
tCIDLib::TVoid TMenu::CleanMenu(const tCIDLib::TResId ridMenu)
{
    // Find the target menu
    tCIDCtrls::TMenuHandle hmenuActual;
    if (bFindSubMenu(m_hmenuThis, ridMenu, hmenuActual))
    {
        // Just keep delete the 0th item until we get a failure
        while (::DeleteMenu(hmenuActual, 0, MF_BYPOSITION))
        {
        }
    }
}


// Delete an item from this menu
tCIDLib::TVoid TMenu::DeleteItem(const tCIDLib::TResId ridToDel)
{
    CheckHandle();
    if (!::DeleteMenu(m_hmenuThis, ridToDel, MF_BYCOMMAND))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcMenu_DeleteItem
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(ridToDel)
            , m_strName
        );
    }
}


//
//  Load up this menu using the indicated menu resource. We load the resource, then
//  we call a recursive helper that recurses through the data structure and creates
//  the menu layout.
//
tCIDLib::TVoid
TMenu::LoadFromResource(const TGUIFacility& facSrc, const tCIDLib::TResId ridMenu)
{
    // Try to load the raw menu resource
    const tCIDCtrls::TRawMenuDesc& rmdSrc = *facSrc.prmdLoad(ridMenu);

    // We got it so load us up from it
    try
    {
        tCIDLib::TCard4 c4ItemCnt = 0;
        LoadToMenu
        (
            m_hmenuThis, facSrc, rmdSrc.armiList[0], m_strName, c4ItemCnt
        );
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}



//
//  Query all of the hot keys in this menu. This is recursive and just iterates
//  through all of the items and sub-items and looks for regular items, and checks
//  their text for a tab. Anything after the tab has to be a hot key.
//
tCIDLib::TVoid
TMenu::QueryAccels(tCIDLib::TStrList& colKeyStrs, tCIDLib::TCardList& fcolCmds) const
{
    CheckHandle();

    // Make sure that targets are empty
    colKeyStrs.RemoveAll();
    fcolCmds.RemoveAll();

    //
    //  We just invoke a private, static recursive method. We start it off with
    //  our main handle.
    //
    tCIDLib::TCh achBuf[1024];
    const TString& strSepChar(L"\t");
    TString strTmp;
    FindHotKeys
    (
        m_hmenuThis, colKeyStrs, fcolCmds, strSepChar, strTmp, achBuf, 1023
    );
}


//
//  Return the display text of a direct item, or an item indirectly underneath a
//  direct sub-menu child.
//
tCIDLib::TVoid
TMenu::QueryItemText(const tCIDLib::TCard4 c4Id, TString& strToFill) const
{
    MENUITEMINFO Info = {0};
    Info.cbSize = sizeof(Info);

    // First get the size of the text. We leave dwTypeData null
    Info.fMask = MIIM_STRING;
    BOOL bRes = ::GetMenuItemInfo(m_hmenuThis, c4Id, FALSE, &Info);
    if (!bRes)
        ThrowGetInfo(c4Id, CID_LINE);

    // Allocate the buffer
    Info.cch++;
    tCIDLib::TCh* pszBuf = new tCIDLib::TCh[Info.cch];
    TArrayJanitor<tCIDLib::TCh> janBuf(pszBuf);
    Info.dwTypeData = pszBuf;
    Info.fMask = MIIM_STRING;
    bRes = ::GetMenuItemInfo(m_hmenuThis, c4Id, FALSE, &Info);
    if (!bRes)
        ThrowGetInfo(c4Id, CID_LINE);

    strToFill = pszBuf;
}

tCIDLib::TVoid
TMenu::QuerySubItemText(const   tCIDLib::TCard4 c4SubMenuId
                        , const tCIDLib::TCard4 c4ItemId
                        ,       TString&        strToFill) const
{
    // Find the submenu
    tCIDCtrls::TMenuHandle hmenuSub = hmenuFindSubMenu(c4SubMenuId);

    MENUITEMINFO Info = {0};
    Info.cbSize = sizeof(Info);

    // First get the size of the text. We leave dwTypeData null
    Info.fMask = MIIM_STRING;
    BOOL bRes = ::GetMenuItemInfo(hmenuSub, c4ItemId, FALSE, &Info);
    if (!bRes)
        ThrowGetInfo(c4ItemId, CID_LINE);

    // Allocate the buffer
    Info.cch++;
    tCIDLib::TCh* pszBuf = new tCIDLib::TCh[Info.cch];
    TArrayJanitor<tCIDLib::TCh> janBuf(pszBuf);
    Info.fMask = MIIM_STRING;
    Info.dwTypeData = pszBuf;

    bRes = ::GetMenuItemInfo(hmenuSub, c4ItemId, FALSE, &Info);
    if (!bRes)
        ThrowGetInfo(c4ItemId, CID_LINE);

    strToFill = pszBuf;
}


//
//  Set or clear the check mark on an item by id. We can also optionaly uncheck
//  any other sibling items (in the same submenu.)
//
tCIDLib::TVoid
TMenu::SetItemCheck(const   tCIDLib::TResId     ridTar
                    , const tCIDLib::TBoolean   bState
                    , const tCIDLib::TBoolean   bUncheckSiblings)
{
    CheckHandle();

    // Find the actual item using the local recursive searcher
    tCIDCtrls::TMenuHandle hmenuActual;
    const tCIDLib::TCard4 c4Offset = c4FindItem(ridTar, hmenuActual);

    //
    //  Now let's set the check state of the found item. This is a by position
    //  call, since we located it's offset and parent menu above.
    //
    MENUITEMINFO Info = {0};
    Info.cbSize = sizeof(MENUITEMINFO);
    Info.fMask = MIIM_STATE;

    // Get the target item's info
    if (!::GetMenuItemInfo(hmenuActual, c4Offset, TRUE, &Info))
        ThrowGetInfo(ridTar, CID_LINE);

    // Toggle the checked bit anmd set it back
    if (bState)
        Info.fState |= MFS_CHECKED;
    else
        Info.fState &= ~MFS_CHECKED;

    ::SetMenuItemInfo(hmenuActual, c4Offset, TRUE, &Info);

    // If asked, uncheck any siblings
    if (bUncheckSiblings)
    {
        tCIDLib::TCard4 c4Index = 0;
        Info.fMask = MIIM_STATE;
        while (::GetMenuItemInfo(hmenuActual, c4Index, TRUE, &Info))
        {
            // If checked and not the one we just did, uncheck it
            if ((Info.fState | MFS_CHECKED) && (c4Index != c4Offset))
            {
                Info.fMask = MIIM_STATE;
                Info.fState &= ~MFS_CHECKED;
                ::SetMenuItemInfo(hmenuActual, c4Index, TRUE, &Info);
            }
            c4Index++;
        }
    }
}


// Enable or disable an item by id
tCIDLib::TVoid
TMenu::SetItemEnable(const tCIDLib::TCard4 c4Id, const tCIDLib::TBoolean bState)
{
    CheckHandle();

    // Find the actual item using the local recursive searcher
    tCIDCtrls::TMenuHandle hmenuActual;
    const tCIDLib::TCard4 c4Offset = c4FindItem(c4Id, hmenuActual);

    // Set up the flags appropriately and do the call
    tCIDLib::TCard4 c4Flags = 0;
    if (bState)
        c4Flags = MF_ENABLED;
    else
        c4Flags = MF_DISABLED | MF_GRAYED;

    c4Flags |= MF_BYPOSITION;

    if (::EnableMenuItem(hmenuActual, c4Offset, c4Flags) == -1)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcMenu_EnableItem
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4Id)
            , m_strName
        );
    }
}


//
//  Set the text of an item after it's been created
//
tCIDLib::TVoid
TMenu::SetItemText(const tCIDLib::TResId ridTar, const TString& strToSet)
{
    CheckHandle();

    // Find the actual item using the local recursive searcher
    tCIDCtrls::TMenuHandle hmenuActual;
    const tCIDLib::TCard4 c4Offset = c4FindItem(ridTar, hmenuActual);

    MENUITEMINFO Info = {0};
    Info.cbSize = sizeof(MENUITEMINFO);
    Info.fMask = MIIM_STRING;
    Info.dwTypeData = (LPWSTR)strToSet.pszBuffer();
    ::SetMenuItemInfo(hmenuActual, c4Offset, TRUE, &Info);
}


//
//  Toggles the check state of the indicated item.
//
tCIDLib::TVoid TMenu::ToggleItemCheck(const tCIDLib::TCard4 c4Id)
{
    CheckHandle();

    // Find the actual item using the local recursive searcher
    tCIDCtrls::TMenuHandle hmenuActual;
    const tCIDLib::TCard4 c4Offset = c4FindItem(c4Id, hmenuActual);

    // Get it's current check state and then flip it the other way and set it back
    MENUITEMINFO Info = {0};
    Info.cbSize = sizeof(MENUITEMINFO);
    Info.fMask = MIIM_STATE;

    if (!::GetMenuItemInfo(hmenuActual, c4Offset, TRUE, &Info))
        ThrowGetInfo(c4Id, CID_LINE);

    if (Info.fState & MFS_CHECKED)
        Info.fState &= ~MFS_CHECKED;
    else
        Info.fState |= MFS_CHECKED;

    Info.fMask = MIIM_STATE;
    ::SetMenuItemInfo(hmenuActual, c4Offset, TRUE, &Info);
}


// ---------------------------------------------------------------------------
//  TMenu: Protected, static methods
// ---------------------------------------------------------------------------

//
//  A recursive function that will search the contents in and under the
//  passed menu, until it finds an item with the given id. It will return
//  the immediate parent menu of that item, and it's offset.
//
//  We have a second one which will return the sequence of offsets and
//  menu handles required to get to the target item. This allows us to
//  backtrack and find ids of parent menus and such, which we cannot
//  otherwise do because there's no way to ask for the item info of a
//  handle directly, you have to have the parent menu and the offset of
//  the thing to get info on.
//
tCIDLib::TBoolean
TMenu::bFindItem(       tCIDCtrls::TMenuHandle  hmenuParent
                , const tCIDLib::TCard4         c4ToFind
                ,       tCIDCtrls::TMenuHandle& hmenuToFill
                ,       tCIDLib::TCard4&        c4Offset)
{
    //
    //  Set up a menu item to get the id of the items, and the submenu
    //  handle so we can recurse when needed.
    //
    MENUITEMINFO TmpInfo = {0};
    TmpInfo.cbSize = sizeof(MENUITEMINFO);
    TmpInfo.fMask = MIIM_ID | MIIM_SUBMENU;

    tCIDLib::TCard4 c4Index = 0;
    while (::GetMenuItemInfo(hmenuParent, c4Index, TRUE, &TmpInfo))
    {
        // If it has the target id, it's our guy
        if (TmpInfo.wID == c4ToFind)
        {
            hmenuToFill = hmenuParent;
            c4Offset = c4Index;
            return kCIDLib::True;
        }

        // If it's a submenu, then recurse. If it's found, return now
        if (TmpInfo.hSubMenu)
        {
            if (bFindItem(TmpInfo.hSubMenu, c4ToFind, hmenuToFill, c4Offset))
                return kCIDLib::True;
        }

        // Move up to the next index under our current menu
        c4Index++;
    }

    // Not found at this level
    return kCIDLib::False;
}


tCIDLib::TBoolean
TMenu::bFindItem(       tCIDCtrls::TMenuHandle  hmenuParent
                , const tCIDLib::TResId         ridToFind
                ,       THandStack&             fcolHandles
                ,       TIdStack&               fcolIds)
{
    //
    //  Set up a menu item to get the id of the items, and the submenu
    //  handle so we can recurse when needed.
    //
    MENUITEMINFO TmpInfo = {0};
    TmpInfo.cbSize = sizeof(MENUITEMINFO);
    TmpInfo.fMask = MIIM_ID | MIIM_SUBMENU;

    tCIDLib::TCard4 c4Index = 0;
    while (::GetMenuItemInfo(hmenuParent, c4Index, TRUE, &TmpInfo))
    {
        // If it has the target id, it's our guy
        if (TmpInfo.wID == ridToFind)
        {
            //
            //  Add these final values to the lists. We push a null for the
            //  menu handle in this case, since it's the item itself.
            //
            fcolHandles.Push(0);
            fcolIds.Push(c4Index);
            return kCIDLib::True;
        }

        //
        //  If it's a submenu, then recurse. If it's found, return now.
        //  We push ourself onto the lists just in case the item is found
        //  inside us. If not, we pop ourselves back off.
        //
        if (TmpInfo.hSubMenu)
        {
            fcolHandles.Push(hmenuParent);
            fcolIds.Push(TmpInfo.wID);

            if (bFindItem(TmpInfo.hSubMenu, ridToFind, fcolHandles, fcolIds))
                return kCIDLib::True;

            // It's not in here, so pop back off
            fcolHandles.tPop();
            fcolIds.tPop();
        }

        // Move up to the next index under our current menu
        c4Index++;
    }

    // Not found at this level
    return kCIDLib::False;
}


// Finds the handle of a submenu of a given id
tCIDLib::TBoolean
TMenu::bFindSubMenu(        tCIDCtrls::TMenuHandle  hmenuParent
                    , const tCIDLib::TResId         ridToFind
                    ,       tCIDCtrls::TMenuHandle& hmenuToFill)
{
    MENUITEMINFO TmpInfo = {0};
    TmpInfo.cbSize = sizeof(MENUITEMINFO);
    TmpInfo.fMask = MIIM_SUBMENU;

    if (!::GetMenuItemInfo(hmenuParent, ridToFind, FALSE, &TmpInfo))
        return kCIDLib::False;

    hmenuToFill = TmpInfo.hSubMenu;
    return kCIDLib::True;
}


//
//  A recursive helper that works through the data in a menu resource and creates the
//  menu structure. It calls private helper methods to add the specific menu items.
//
tCIDLib::TVoid
TMenu::LoadToMenu(          tCIDCtrls::TMenuHandle      hmenuTarget
                    , const TGUIFacility&               facSrc
                    , const tCIDCtrls::TRawMenuItem&    rmiCurLevel
                    , const TString&                    strName
                    ,       tCIDLib::TCard4&            c4Count)
{
    //
    //  Get a TCard1 pointer which we use to run through the data. Move
    //  it up past the current incoming item.
    //
    const tCIDLib::TCard1* pc1Cur
            = reinterpret_cast<const tCIDLib::TCard1*>(&rmiCurLevel);
    pc1Cur += sizeof(tCIDCtrls::TRawMenuItem);

    tCIDLib::TCard2 c2Index = 0;
    for (; c2Index < rmiCurLevel.c2SubCount; c2Index++)
    {
        // Cast the current pointer to a raw menu item and process it
        const tCIDCtrls::TRawMenuItem* prmiCur
               = reinterpret_cast<const tCIDCtrls::TRawMenuItem*>(pc1Cur);

        // Add this one
        AddToMenu(hmenuTarget, strName, facSrc, *prmiCur);
        c4Count++;

        //
        //  If it's a submenu, then we need to recurse on this and allow
        //  it to add any sub-items.
        //
        if (prmiCur->eType == tCIDCtrls::EMenuItemTypes::SubMenu)
        {
            // Get the submenu handle out of it
            tCIDCtrls::TMenuHandle hmenuSub = ::GetSubMenu(hmenuTarget, c2Index);
            LoadToMenu(hmenuSub, facSrc, *prmiCur, strName, c4Count);

            // Move up to the next sibling item at this level
            pc1Cur += prmiCur->c2NextOfs;
        }
         else
        {
            // Just move up one item
            pc1Cur += sizeof(tCIDCtrls::TRawMenuItem);
        }
    }
}


// ---------------------------------------------------------------------------
//  TMenu: Hidden Constructors
// ---------------------------------------------------------------------------
TMenu::TMenu(const  tCIDLib::TCh* const pszName
            , const EMenuTypes          eType) :

    m_eType(eType)
    , m_hmenuThis(kCIDCtrls::hmenuInvalid)
    , m_hwndAssoc(kCIDCtrls::hwndInvalid)
    , m_strName(pszName)
{
}

// A special one for sub-menus
TMenu::TMenu(const TMenu& menuParent, const tCIDLib::TResId ridToFind) :

    m_eType(EMenuTypes::SubMenu)
    , m_hmenuThis(kCIDCtrls::hmenuInvalid)
    , m_hwndAssoc(kCIDCtrls::hwndInvalid)
{
    m_hmenuThis = menuParent.hmenuFindSubMenu(ridToFind);
}


// ---------------------------------------------------------------------------
//  TMenu: Protected, non-virtual methods
// ---------------------------------------------------------------------------


// A frame window calls this to assign us to himself
tCIDLib::TVoid TMenu::AssignToWnd(const TFrameWnd* const pwndTar)
{
    // Make sure it's not already assigned
    if (m_hwndAssoc != kCIDCtrls::hwndInvalid)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcMenu_AlreadyAssigned
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , m_strName
        );
    }

    // Make sure that we are a menu bar type
    if (m_eType != EMenuTypes::MenuBar)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcMenu_NotBarType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }

    tCIDCtrls::TWndHandle hwndTar = kCIDCtrls::hwndInvalid;
    if (pwndTar)
        hwndTar = pwndTar->hwndThis();

    if (!::SetMenu(hwndTar, m_hmenuThis))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcMenu_Assign
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
        );
    }

    // It worked, so store his window handle
    m_hwndAssoc = hwndTar;
}



// Find and item by id or throw
tCIDLib::TCard4
TMenu::c4FindItem(  const   tCIDLib::TCard4         c4ToFind
                    ,       tCIDCtrls::TMenuHandle& hmenuToFill) const
{
    tCIDLib::TCard4 c4Offset;

    if (!bFindItem(m_hmenuThis, c4ToFind, hmenuToFill, c4Offset))
    {
        // The item doesn't exist
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcMenu_CantFindItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , TCardinal(c4ToFind)
            , m_strName
        );
    }

    return c4Offset;
}


// Throw if the menu already exists
tCIDLib::TVoid TMenu::CheckExists() const
{
    if (m_hmenuThis != kCIDCtrls::hmenuInvalid)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcGen_AlreadyCreated
            , m_strName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"menu")
        );
    }
}


// Check that the menu handle is valid, else throw
tCIDLib::TVoid TMenu::CheckHandle() const
{
    if (m_hmenuThis == kCIDCtrls::hmenuInvalid)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcGen_CreateFailed
            , m_strName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"menu")
        );
    }
}


//
//  Works in terms of bFindSubMenu, but is often more convenient since it throws
//  if not found, just searches the whole menu, and just returns the menu handle.
//
tCIDCtrls::TMenuHandle
TMenu::hmenuFindSubMenu(const tCIDLib::TResId ridToFind) const
{
    // If the id is max card, then it's the top level menu
    if (ridToFind == kCIDLib::c4MaxCard)
        return m_hmenuThis;

    tCIDCtrls::TMenuHandle hmenuRet;
    if (!bFindSubMenu(m_hmenuThis, ridToFind, hmenuRet))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcMenu_FindSubMenu
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(ridToFind)
            , m_strName
        );
    }
    return hmenuRet;
}




// Provide access to our derived class if needed
tCIDCtrls::TMenuHandle TMenu::hmenuThis() const
{
    return m_hmenuThis;
}


// Make the associated window handle available to the menu bar derivative
tCIDCtrls::TWndHandle TMenu::hwndAssoc() const
{
    return m_hwndAssoc;
}


// Providess access to our name
const TString& TMenu::strName() const
{
    return m_strName;
}


// For the derived class to store the menu handle
tCIDLib::TVoid
TMenu::StoreHandle( const   tCIDCtrls::TMenuHandle  hmenuToStore
                    , const tCIDLib::TBoolean       bOverwrite)
{
    //
    //  If the 'ok to overwrite' flag isn't set, then check and throw if it
    //  already exists. Typically we want to do that check, but sub-menus are
    //  transient and often get reset, and the handle isn't adopted in that
    //  case anyway, so it's never an issue.
    //
    if (!bOverwrite)
        CheckExists();

    m_hmenuThis = hmenuToStore;
}


tCIDLib::TVoid
TMenu::ThrowGetInfo(const   tCIDLib::TCard4 c4Id
                    , const tCIDLib::TCard4 c4Line) const
{
    // The item doesn't exist
    facCIDCtrls().ThrowErr
    (
        CID_FILE
        , c4Line
        , kCtrlsErrs::errcMenu_GetItemInfo
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotFound
        , TCardinal(c4Id)
        , m_strName
    );
}


tCIDLib::TVoid TMenu::Unassign()
{
    if (m_hwndAssoc != kCIDCtrls::hwndInvalid)
    {
        ::SetMenu(m_hwndAssoc, 0);
        ::DrawMenuBar(m_hwndAssoc);
        m_hwndAssoc = kCIDCtrls::hwndInvalid;
    }
}


// ---------------------------------------------------------------------------
//  TMenu: Private, static methods
//
//  These are helpers that are used to create or manipulate the menu info. They don't
//  work in terms of our menu handle, since each sub-menu is a separate handle. So
//  they are static in order to avoid any possible mistakes.
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TMenu::AddActItem(          tCIDCtrls::TMenuHandle  hmenuTarget
                    , const TString&                strName
                    , const TString&                strText
                    , const tCIDLib::TCard4         c4Id
                    , const tCIDLib::TBoolean       bEnabled)
{
    // Get the current count so that we can add at the end
    const tCIDLib::TCard4 c4Count = ::GetMenuItemCount(hmenuTarget);

    // Fill in an info object with the info we need
    MENUITEMINFO AddInfo = {0};
    AddInfo.cbSize = sizeof(MENUITEMINFO);
    AddInfo.fMask = MIIM_ID | MIIM_STRING;
    AddInfo.wID = c4Id;
    AddInfo.fType = MFT_STRING;
    AddInfo.dwTypeData = (tCIDLib::TCh*)strText.pszBuffer();

    // If disabled, then add some more stuff
    if (!bEnabled)
    {
        AddInfo.fMask |= MIIM_STATE;
        AddInfo.fState = MFS_DISABLED;
    }

    if (!::InsertMenuItem(hmenuTarget, c4Count, 1, &AddInfo))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcMenu_AddActionItem
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strName
        );
    }
}


tCIDLib::TVoid
TMenu::AddSep(tCIDCtrls::TMenuHandle hmenuTarget, const TString& strName)
{
    // Get the current count so that we can add at the end
    const tCIDLib::TCard4 c4Count = ::GetMenuItemCount(hmenuTarget);

    // Fill in an info object with the info we need
    MENUITEMINFO AddInfo = {0};
    AddInfo.cbSize = sizeof(MENUITEMINFO);
    AddInfo.fMask = MIIM_FTYPE;
    AddInfo.fType = MFT_SEPARATOR;

    if (!::InsertMenuItem(hmenuTarget, c4Count, 1, &AddInfo))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcMenu_AddSeparator
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strName
        );
    }
}


//
//  Add a single item to the passed menu
//
tCIDLib::TVoid
TMenu::AddToMenu(       tCIDCtrls::TMenuHandle      hmenuTarget
                , const TString&                    strName
                , const TGUIFacility&               facSrc
                , const tCIDCtrls::TRawMenuItem&    rmiToAdd)
{
    switch (rmiToAdd.eType)
    {
        case tCIDCtrls::EMenuItemTypes::Decoration :
        {
            // Add a separator
            AddSep(hmenuTarget, strName);
        }
        break;

        case tCIDCtrls::EMenuItemTypes::ActionItem :
        {
            AddActItem
            (
                hmenuTarget
                , strName
                , facSrc.strMsg(rmiToAdd.midText)
                , rmiToAdd.c4Id
            );
        }
        break;

        case tCIDCtrls::EMenuItemTypes::SubMenu :
        {
            // Create the submenu item
            hmenuAddSub
            (
                hmenuTarget
                , strName
                , facSrc.strMsg(rmiToAdd.midText)
                , rmiToAdd.c4Id
                , kCIDLib::True
            );
        }
        break;

        default :
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcMenu_BadItemType
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , TInteger(tCIDLib::i4EnumOrd(rmiToAdd.eType))
                , strName
            );
        }
        break;
    }
}


//
//  Given a menu description, we recursively iterate it and build up a list of
//  hot keys that can be used to create an accelerator table object if the caller
//  wants to do so.
//
//  We skip disabled entries, since it's assumed they would not be processed via
//  the menu so they shouldn't be processed via hot keys either.
//
tCIDLib::TVoid
TMenu::FindHotKeys( const   tCIDCtrls::TMenuHandle  hmenuCur
                    ,       tCIDLib::TStrList&      colKeyStrs
                    ,       tCIDLib::TCardList&     fcolCmds
                    , const TString&                strSepChar
                    ,       TString&                strTmpStr
                    ,       tCIDLib::TCh* const     pszBuf
                    , const tCIDLib::TCard4         c4BufSz)
{
    MENUITEMINFO TmpInfo = {0};
    TmpInfo.cbSize = sizeof(MENUITEMINFO);
    TmpInfo.fMask = MIIM_SUBMENU | MIIM_TYPE | MIIM_ID | MIIM_STATE;
    TmpInfo.dwTypeData = pszBuf;
    TmpInfo.cch = c4BufSz;

    tCIDLib::TCard4 c4Index = 0;
    while (::GetMenuItemInfo(hmenuCur, c4Index, TRUE, &TmpInfo))
    {
        //
        //  If it's a submenu, then recurse. Else see if it has any text. If so
        //  check it out.
        //
        if (TmpInfo.hSubMenu)
        {
            FindHotKeys
            (
                TmpInfo.hSubMenu
                , colKeyStrs
                , fcolCmds
                , strSepChar
                , strTmpStr
                , pszBuf
                , c4BufSz
            );
        }
         else if ((TmpInfo.fType == MFT_STRING)
              &&  ((TmpInfo.fState & MFS_DISABLED) == 0))
        {
            // Get the text. WE have to update the info structure for this
            TmpInfo.fMask = MIIM_STRING;
            TmpInfo.cch = c4BufSz;
            TmpInfo.dwTypeData = pszBuf;
            if (::GetMenuItemInfo(hmenuCur, c4Index, TRUE, &TmpInfo))
            {
                // See it has what we want
                strTmpStr = TmpInfo.dwTypeData;

                tCIDLib::TCard4 c4At = 0;
                if (strTmpStr.bFirstOccurrence(strSepChar, c4At))
                {
                    // But up to the tab and keep it
                    strTmpStr.Cut(0, c4At + 1);
                    colKeyStrs.objAdd(strTmpStr);
                    fcolCmds.c4AddElement(TmpInfo.wID);
                }
            }
        }

        // Go back the general mask bits
        TmpInfo.fMask = MIIM_SUBMENU | MIIM_TYPE | MIIM_ID | MIIM_STATE;

        // Move up to the next index under our current menu
        c4Index++;
    }
}


// Add a new sub-menu to the target menu. We return the new sub-menu handle
tCIDCtrls::TMenuHandle
TMenu::hmenuAddSub(         tCIDCtrls::TMenuHandle  hmenuTarget
                    , const TString&                strName
                    , const TString&                strText
                    , const tCIDLib::TCard4         c4Id
                    , const tCIDLib::TBoolean       bEnabled)
{
    // Get the current count so that we can add at the end
    const tCIDLib::TCard4 c4Count = ::GetMenuItemCount(hmenuTarget);

    // Fill in an info object with the info we need
    MENUITEMINFO AddInfo = {0};
    AddInfo.cbSize = sizeof(MENUITEMINFO);
    AddInfo.fMask = MIIM_SUBMENU | MIIM_ID | MIIM_STRING;
    AddInfo.wID = c4Id;
    AddInfo.fType = MFT_STRING;
    AddInfo.dwTypeData = (tCIDLib::TCh*)strText.pszBuffer();

    // Create a new empty dropdown menu for this item
    AddInfo.hSubMenu = hmenuCreate(TMenu::EMenuTypes::PopUp);

    // If disabled, then add some more stuff
    if (!bEnabled)
    {
        AddInfo.fMask |= MIIM_STATE;
        AddInfo.fState = MFS_DISABLED;
    }

    if (!::InsertMenuItem(hmenuTarget, c4Count, 1, &AddInfo))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcMenu_AddSubMenu
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strName
        );
    }
    return AddInfo.hSubMenu;
}


//
//  Create a new menu of the appropriate type
//
tCIDCtrls::TMenuHandle TMenu::hmenuCreate(const EMenuTypes eType)
{
    tCIDCtrls::TMenuHandle hmenuRet;
    if (eType == EMenuTypes::MenuBar)
        hmenuRet = ::CreateMenu();
    else
        hmenuRet = ::CreatePopupMenu();

    return hmenuRet;
}




// ---------------------------------------------------------------------------
//  CLASS: TMenuBase
// PREFIX: menu
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TMenuBar: Constructors and Destructor
// ---------------------------------------------------------------------------
TMenuBar::TMenuBar() :

    TMenu(L"Menu Bar", EMenuTypes::MenuBar)
{
}

TMenuBar::~TMenuBar()
{
}


// ---------------------------------------------------------------------------
// TMenuBar: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Return whether we have been assigned to a window or not
tCIDLib::TBoolean TMenuBar::bIsAssigned() const
{
    return (hwndAssoc() != kCIDCtrls::hwndInvalid);
}


//
//  Create the menu and store the handle on our parent class.
//
tCIDLib::TVoid TMenuBar::Create()
{
    // Make sure it doesn't already exist
    CheckExists();

    // Looks reasonable so try to create it
    tCIDCtrls::TMenuHandle hmenuNew = ::CreateMenu();
    if (hmenuNew == kCIDCtrls::hmenuInvalid)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcMenu_Create
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strName()
        );
    }

    // It worked so store it
    try
    {
        StoreHandle(hmenuNew);
    }

    catch(TError& errToCatch)
    {
        // Just in case, destroy it since we couldn't store it
        ::DestroyMenu(hmenuNew);

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    //
    //  We have to make sure that all our menu items are marked to notify by
    //  position, so that we get a WM_MENUCOMMAND instead of WM_COMMAND. That's
    //  the only way our low level dispatcher can know that it's a menu command
    //  and not treat it as a child control notification. Since new items can be
    //  added any time, we just do it here to be sure.
    //
    //  Only matters for menu bar types. We set up popups to only return a value
    //  not notify.
    //
    MENUINFO Info = {0};
    Info.cbSize = sizeof(Info);
    Info.fMask = MIM_APPLYTOSUBMENUS | MIM_STYLE;
    Info.dwStyle = MNS_NOTIFYBYPOS;
    if (!::SetMenuInfo(hmenuThis(), &Info))
    {
    }
}


//
//  Create the menu and store the handle on our our parent class, then load a menu
//  resource and load up the menu to match that menu resource.
//
tCIDLib::TVoid
TMenuBar::Create(const TGUIFacility& facSrc, const tCIDLib::TResId ridMenu)
{
    // Call the other version to create the menu
    Create();

    // Load it up from the resource
    LoadFromResource(facSrc, ridMenu);
}


tCIDLib::TVoid TMenuBar::Disassociate()
{
    // Just pass it to our parent class
    Unassign();
}

//
//  This has to be called after make changes to a menu bar. If we have been associated
//  with a window, we'll tell it to redraw its menu bar.
//
tCIDLib::TVoid TMenuBar::Redraw()
{
    if (bIsAssigned())
        ::DrawMenuBar(hwndAssoc());
}




// ---------------------------------------------------------------------------
//  CLASS: TPopupMenu
// PREFIX: menu
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TPopupMenu: Constructors and Destructor
// ---------------------------------------------------------------------------
TPopupMenu::TPopupMenu(const tCIDLib::TCh* const pszName) :

    TMenu(pszName, EMenuTypes::PopUp)
{
}

TPopupMenu::~TPopupMenu()
{
}


// ---------------------------------------------------------------------------
// TPopupMenu: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  If no selection is made we return 0, so zero should never be used as a menu
//  item id.
//
tCIDLib::TCard4
TPopupMenu::c4Process(  const   TWindow&            wndOwner
                        , const TPoint&             pntAt
                        , const tCIDLib::EVJustify  eVJustify
                        , const tCIDLib::EHJustify  eHJustify)
{
    // Call the other version with an empty area
    TArea areaEmpty;
    return c4Process(wndOwner, pntAt, areaEmpty, eVJustify, eHJustify);
}

tCIDLib::TCard4
TPopupMenu::c4Process(  const   TWindow&            wndOwner
                        , const TPoint&             pntAt
                        , const TArea&              areaAvoid
                        , const tCIDLib::EVJustify  eVJustify
                        , const tCIDLib::EHJustify  eHJustify)
{
    // Make sure the popup is created
    CheckHandle();

    // Set up the flags
    UINT uFlags = TPM_LEFTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY;

    if (eHJustify == tCIDLib::EHJustify::Count)
    {
        // They want us to use the system default
        if (::GetSystemMetrics(SM_MENUDROPALIGNMENT))
            uFlags |= TPM_RIGHTALIGN | TPM_HORNEGANIMATION;
        else
            uFlags |= TPM_LEFTALIGN | TPM_HORPOSANIMATION;
    }
     else
    {
        if (eHJustify == tCIDLib::EHJustify::Left)
            uFlags |= TPM_LEFTALIGN;
        else if (eHJustify == tCIDLib::EHJustify::Center)
            uFlags |= TPM_CENTERALIGN;
        else if (eHJustify == tCIDLib::EHJustify::Right)
            uFlags |= TPM_RIGHTALIGN;
    }

    if (eVJustify == tCIDLib::EVJustify::Bottom)
        uFlags |= TPM_BOTTOMALIGN;
    else if (eVJustify == tCIDLib::EVJustify::Center)
        uFlags |= TPM_VCENTERALIGN;
    else if (eVJustify == tCIDLib::EVJustify::Top)
        uFlags |= TPM_TOPALIGN;

    //
    //  Set up the optional parameters. If they provided a non-empty avoid area,
    //  we do it.
    //
    TPMPARAMS Params = {0};
    TPMPARAMS* pParams = 0;
    if (!areaAvoid.bIsEmpty())
    {
        Params.cbSize = sizeof(TPMPARAMS);
        areaAvoid.ToRectl
        (
            *reinterpret_cast<tCIDLib::THostRectl*>(&Params.rcExclude)
            , tCIDLib::ERectlTypes::Inclusive
        );
        pParams = &Params;
    }

    //
    //  In theory this returns error info, but it's not reliable, so we don't try to use
    //  it. Even if we set the last error before invoking, it will still indicate an error
    //  if you just click outside the error to dismiss it, or hit escape.
    //
    //  It does reliably return 0 if no selection was made which is what we ultimately need.
    //
    tCIDLib::TCard4 Res = ::TrackPopupMenuEx
    (
        hmenuThis()
        , uFlags
        , pntAt.i4X()
        , pntAt.i4Y()
        , wndOwner.hwndSafe()
        , pParams
    );

    return Res;
}


//
//  Create the menu and store the handle on our parent class.
//
tCIDLib::TVoid TPopupMenu::Create()
{
    // Make sure it doesn't already exist
    CheckExists();

    // Looks reasonable so try to create it
    tCIDCtrls::TMenuHandle hmenuNew = ::CreatePopupMenu();
    if (hmenuNew == kCIDCtrls::hmenuInvalid)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcMenu_Create
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strName()
        );
    }

    //
    //  It worked so store it. These don't get assigned to a window, so we don't
    //  call the assignment method.
    //
    try
    {
        StoreHandle(hmenuNew);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);

        // Just in case, destroy it since we couldn't store it
        ::DestroyMenu(hmenuNew);
        throw;
    }
}


//
//  Create the menu and store the handle on our our parent class, then load a menu
//  resource and load up the menu to match that menu resource.
//
tCIDLib::TVoid
TPopupMenu::Create(const TGUIFacility& facSrc, const tCIDLib::TResId ridMenu)
{
    // Call the other version to create the menu
    Create();

    // Call a helper to load the resource info into the menu
    LoadFromResource(facSrc, ridMenu);
}



// ---------------------------------------------------------------------------
//  CLASS: TSubMenu
// PREFIX: menu
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TSubMenu: Constructors and Destructor
// ---------------------------------------------------------------------------

TSubMenu::TSubMenu() :

    TMenu(L"", EMenuTypes::SubMenu)
{
}

//
//  We call a parent ctor that takes a menu handle. It knows in these cases that
//  it's a sub-menu and will set the type appropriately.
//
TSubMenu::TSubMenu(const TMenu& menuParent, const tCIDLib::TCard4 c4SubMenuId) :

    TMenu(menuParent, c4SubMenuId)
{
}

TSubMenu::~TSubMenu()
{
}


// ---------------------------------------------------------------------------
// TSubMenu: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Change us to point to a new sub-menu. Since we don't adopt the handle, there's
//  nothing to worry about here. We just store the new menu handle. Our type is
//  already set to sub-menu so this new menu handle will not get adopted either.
//
tCIDLib::TVoid
TSubMenu::Set(const TMenu& menuParent, const tCIDLib::TResId ridToFind)
{
    //
    //  Indicate that we know overwrite of the handle is safe, so don't throw
    //  if we already have a handle.
    //
    StoreHandle(menuParent.hmenuFindSubMenu(ridToFind), kCIDLib::True);
}


