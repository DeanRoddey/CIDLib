//
// FILE NAME: CIDResEd_EditWnd.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/17/2000
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
//  Implements the editor window, which contains the actual controls that we are
//  designing.
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
#include "CIDResEd.hpp"



// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TResEdDisplay, TWindow)
RTTIDecls(TResEdEditor, TWindow)
RTTIDecls(TResEdCont, TWindow)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDResEd_EdWnd
{
    // -----------------------------------------------------------------------
    //  Command ids for our accelerator table. We start at 10000 to insure we
    //  can't conflict with any command ids for our popup menu.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4Cmd_Delete    = 10000;

    const tCIDLib::TCard4   c4Cmd_Down      = 10001;
    const tCIDLib::TCard4   c4Cmd_Left      = 10002;
    const tCIDLib::TCard4   c4Cmd_Right     = 10003;
    const tCIDLib::TCard4   c4Cmd_Up        = 10004;

    const tCIDLib::TCard4   c4Cmd_HGrow     = 10006;
    const tCIDLib::TCard4   c4Cmd_HShrink   = 10007;
    const tCIDLib::TCard4   c4Cmd_VGrow     = 10008;
    const tCIDLib::TCard4   c4Cmd_VShrink   = 10009;
}



// ---------------------------------------------------------------------------
//   CLASS: TResEdDisplay
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResEdDisplay: Constructors and Destructor
// ---------------------------------------------------------------------------
TResEdDisplay::TResEdDisplay() :

    TWindow(kCIDLib::False)
{
}

TResEdDisplay::~TResEdDisplay()
{
}


// ---------------------------------------------------------------------------
//  TResEdDisplay: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TResEdDisplay::Create(const TResEdCont& wndParent, const tCIDCtrls::TWndId widThis)
{
    TArea areaInit;
    wndParent.QueryClientArea(areaInit, kCIDLib::True);

    CreateWnd
    (
        wndParent.hwndSafe()
        , kCIDCtrls::pszCustClass
        , L""
        , areaInit
        , tCIDCtrls::EWndStyles::ClippingVisChild
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );
}


// Remove all of the child controls that are in the passed list
tCIDLib::TVoid
TResEdDisplay::DeleteCtrls(const tCIDResEd::TDlgItemRefList& colDelete)
{
    const tCIDLib::TCard4 c4Count = colDelete.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        DestroyChild(colDelete[c4Index]->ridThis());
}


//
//  The editor calls here here when it's time to edit a new dialog. We remove
//  any existing children then load up the new ones.
//
tCIDLib::TVoid TResEdDisplay::LoadDialog(const TResEdDlgDesc& reddLoad)
{
    // Clean up any existing children
    DestroyAllChildren();

    //
    //  Since we aren't calling the usual method to create the dialog window, we
    //  have to make sure that the appropriate bgn color has been explicitly set
    //  on our window, for the dialog's theme. Otherwise, we'd just pick up our
    //  parent's color.
    //
    SetBgnColor(TDlgBox::rgbDlgBgn(reddLoad.eTheme()));

    // And let's create the controls
    const tCIDLib::TCard4 c4Count = reddLoad.c4ChildCount();
    if (c4Count)
    {
        TDlgItem dlgiNew;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TResEdDlgItem& reditCur = reddLoad.reditAt(c4Index);
            TResEdDlgItem::InitCtrl(reditCur, reddLoad.eTheme(), *this, nullptr);
        }
    }
    Redraw(areaWnd(), tCIDCtrls::ERedrawFlags::Full);
}


tCIDLib::TVoid TResEdDisplay::Reset()
{
    // Destroy all our child windows
    DestroyAllChildren();

    //
    //  And force a redraw so that the transparent editor window over us will
    //  have to fully redraw and make sure we clear out any selection handles
    //  and such.
    //
    Redraw(areaWnd(), tCIDCtrls::ERedrawFlags::Full);
}


tCIDLib::TVoid
TResEdDisplay::ResizeCtrl(  const   TResEdDlgItem&  reditTar
                            , const TArea&          areaNew)
{
    TWindow* pwndTar = pwndChildByUserId(reditTar.c4EditId());
    CIDAssert(pwndTar != 0, L"ResizeCtrl called with an unknown edit id");

    pwndTar->SetSizePos(areaNew, kCIDLib::False);
}


TWindow& TResEdDisplay::wndByEditId(const tCIDLib::TCard4 c4EditId)
{
    // Tell it throw if not found, since we are returning by reference
    return *pwndChildByUserId(c4EditId, kCIDLib::True);
}




// ---------------------------------------------------------------------------
//   CLASS: TResEdEditor
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResEdEditor: Constructors and Destructor
// ---------------------------------------------------------------------------
TResEdEditor::TResEdEditor() :

    TWindow(kCIDLib::False)
    , m_bHideSelHandles(kCIDLib::False)
    , m_c4EditId(kCIDLib::c4MaxCard)
    , m_colSelList(tCIDLib::EAdoptOpts::NoAdopt)
    , m_pwndAttrPane(nullptr)
    , m_pwndDisplay(nullptr)
{
}

TResEdEditor::~TResEdEditor()
{
}


// ---------------------------------------------------------------------------
//  TResEdEditor: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TResEdEditor::bProcessAccel(const tCIDLib::TVoid* const pMsgData) const
{
    return m_accelEd.bProcessAccel(*this, pMsgData);
}



//
//  We implement the in place editor's interface, and pass ourself to the attribute
//  editor windows, which in turn pass us to the IPE for it to use for validation.
//  So we get called back here when the IPE needs to validate an entered value. Most
//  of them we will pass to the base mixin interface, but we handle those that require
//  extra checking.
//
tCIDLib::TBoolean
TResEdEditor::bIPEValidate( const   TString&            strSrc
                            ,       TAttrData&          adatVal
                            , const TString&            strNewVal
                            ,       TString&            strErrMsg
                            ,       tCIDLib::TCard8&    c8UserId) const
{
    //
    //  Call the base mixin class to do basic validation. If that fails, then
    //  no need to look further.
    //
    if (!MIPEIntf::bIPEValidate(strSrc, adatVal, strNewVal, strErrMsg, c8UserId))
        return kCIDLib::False;

    // If from the item attrs window, there must be a single selected item
    #if CID_DEBUG_ON
    if ((strSrc == kCIDResEd::strWnd_ItemAttrEd) && (m_colSelList.c4ElemCount() != 1))
    {
        CIDAssert2(L"Got item attribute val but select count != 1");
    }
    #endif

    // Let's do our own validation for the things that need it
    if (adatVal.strSpecType() == kCIDResEd::strSpecType_LoadText)
    {
        // It's got to be one of the text symbols
        tCIDResEd::ETextSyms eSym = tCIDResEd::eXlatETextSyms(adatVal.strId());
        if (eSym == tCIDResEd::ETextSyms::Count)
        {
            strErrMsg = L"Unknown loadable text symbol";
            return kCIDLib::False;
        }

        // Ask the main window if this symbol exists
        if (!facCIDResEd.wndMain().bTextSymExists(strNewVal))
        {
            strErrMsg = L"Could not find text symbol: ";
            strErrMsg.Append(strNewVal);
            return kCIDLib::False;
        }

        adatVal.SetString(strNewVal);
    }
     else if (adatVal.strId() == kCIDResEd::strAttr_ResId)
    {
        //
        // If not zero, and the id isn't the same as the original, it can't
        //  already exist.
        //
        tCIDLib::TResId ridNew = strNewVal.c4Val();
        if (ridNew)
        {
            if (strSrc == kCIDResEd::strWnd_DlgAttrEd)
            {
                if ((m_reddEdit.ridDlg() != ridNew)
                &&  facCIDResEd.wndMain().preddByResId(ridNew))
                {
                    strErrMsg = L"That dialog id is already in use";
                    return kCIDLib::False;
                }
            }
             else
            {
                if ((m_colSelList[0]->ridThis() != ridNew)
                &&  m_reddEdit.bResIdUsed(ridNew))
                {
                    strErrMsg = L"That id is already in use within this dialog";
                    return kCIDLib::False;
                }
            }
        }
        adatVal.SetCard(ridNew);
    }
     else if (adatVal.strId() == kCIDResEd::strAttr_Symbol)
    {
        if (strSrc == kCIDResEd::strWnd_DlgAttrEd)
        {
            // If it's not the same name as the current dialog, it cannot be found
            if ((m_reddEdit.strName() != strNewVal)
            &&   facCIDResEd.wndMain().preddByName(strNewVal))
            {
                strErrMsg = L"This dialog name is already taken";
                return kCIDLib::False;
            }
        }
         else
        {
            // In this case it doesn't have to have a symbolic id, so it can be empty
            if (!strNewVal.bIsEmpty()
            &&  (m_colSelList[0]->strSymName() != strNewVal)
            &&  m_reddEdit.preditBySymName(strNewVal))
            {
                strErrMsg = L"This item name is already taken";
                return kCIDLib::False;
            }
        }
        adatVal.SetString(strNewVal);
    }
     else if (adatVal.strId() == kCIDResEd::strAttr_Title)
    {
        // Ask the main window if this symbol exists
        if (!facCIDResEd.wndMain().bTextSymExists(strNewVal))
        {
            strErrMsg = L"Could not find text symbol: ";
            strErrMsg.Append(strNewVal);
            return kCIDLib::False;
        }
        adatVal.SetString(strNewVal);
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TResEdEditor: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Return whether we have any changes that might need saving.
//
tCIDLib::TBoolean TResEdEditor::bChanges() const
{
    if (m_c4EditId == kCIDLib::c4MaxCard)
        return kCIDLib::False;

    return m_reddEdit != m_reddOrg;
}


//
//  We just copy our current dialog info to the original so that we no longer think
//  there are any changes, if there were any, and generaly this won't be called unless
//  we reported changes.
//
tCIDLib::TVoid TResEdEditor::CommitChanges()
{
    m_reddOrg = m_reddEdit;
}


tCIDLib::TVoid
TResEdEditor::Create(const  TResEdCont&             wndParent
                    , const tCIDCtrls::TWndId       widThis
                    ,       TResEdDisplay* const    pwndDisplay
                    ,       TResAttrEdPane*         pwndAttrPane)
{
    // Store the incoming windows for later use
    m_pwndDisplay = pwndDisplay;
    m_pwndAttrPane = pwndAttrPane;

    // Set us as the IPE handler for both of the attribute editors in the attr pane
    m_pwndAttrPane->wndDlgAttrs().SetIPEHandler(this);
    m_pwndAttrPane->wndItemAttrs().SetIPEHandler(this);

    // Create us to fit in the client area of parent
    TArea areaInit;
    wndParent.QueryClientArea(areaInit, kCIDLib::True);
    CreateWnd
    (
        wndParent.hwndSafe()
        , kCIDCtrls::pszCustClass
        , L""
        , areaInit
        , tCIDCtrls::EWndStyles::ClippingVisChild
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );

    //
    //  Register for attribute change notifications from both of the attribute
    //  editor windows.
    //
    m_pwndAttrPane->wndDlgAttrs().pnothRegisterHandler(this, &TResEdEditor::eAttrEditHandler);
    m_pwndAttrPane->wndItemAttrs().pnothRegisterHandler(this, &TResEdEditor::eAttrEditHandler);

    // Take the focus
    TakeFocus();

    //
    //  Create our accelerator table. This one we do manually since there's no
    //  menu that corresponds to these. We start at id 10000, which insures that
    //  our non-menu commands don't interfere with the popup menu items. For those
    //  that are in the menu, we use the same command id.
    //
    {
        tCIDLib::TStrList colKeyStrs;
        tCIDLib::TCardList fcolCmds;

        // These are dups of our popup menus
        colKeyStrs.objAdd(L"Ctrl-B");
        fcolCmds.c4AddElement(kCIDResEd::ridMenu_DlgEdit_Back);

        colKeyStrs.objAdd(L"Ctrl-F");
        fcolCmds.c4AddElement(kCIDResEd::ridMenu_DlgEdit_Forward);

        colKeyStrs.objAdd(L"Ctrl-Shift-B");
        fcolCmds.c4AddElement(kCIDResEd::ridMenu_DlgEdit_ToBack);

        colKeyStrs.objAdd(L"Ctrl-Shift-F");
        fcolCmds.c4AddElement(kCIDResEd::ridMenu_DlgEdit_ToFront);

        // These are non-menu keys
        colKeyStrs.objAdd(L"Delete");
        fcolCmds.c4AddElement(CIDResEd_EdWnd::c4Cmd_Delete);

        colKeyStrs.objAdd(L"Down");
        fcolCmds.c4AddElement(CIDResEd_EdWnd::c4Cmd_Down);
        colKeyStrs.objAdd(L"Left");
        fcolCmds.c4AddElement(CIDResEd_EdWnd::c4Cmd_Left);
        colKeyStrs.objAdd(L"Right");
        fcolCmds.c4AddElement(CIDResEd_EdWnd::c4Cmd_Right);
        colKeyStrs.objAdd(L"Up");
        fcolCmds.c4AddElement(CIDResEd_EdWnd::c4Cmd_Up);

        colKeyStrs.objAdd(L"Ctrl-Down");
        fcolCmds.c4AddElement(CIDResEd_EdWnd::c4Cmd_VGrow);
        colKeyStrs.objAdd(L"Ctrl-Left");
        fcolCmds.c4AddElement(CIDResEd_EdWnd::c4Cmd_HShrink);
        colKeyStrs.objAdd(L"Ctrl-Right");
        fcolCmds.c4AddElement(CIDResEd_EdWnd::c4Cmd_HGrow);
        colKeyStrs.objAdd(L"Ctrl-Up");
        fcolCmds.c4AddElement(CIDResEd_EdWnd::c4Cmd_VShrink);

        m_accelEd.Create(colKeyStrs, fcolCmds);
    }
}


tCIDLib::TVoid TResEdEditor::EditDlg(const TResEdDlgDesc& reddEdit)
{
    m_c4EditId = reddEdit.c4EditId();
    m_reddOrg = reddEdit;
    m_reddEdit = m_reddOrg;

    // Load up our overall dialog attributes to the dlg attr editor
    LoadDlgAttrs();

    // Tell the main frame window about our new dialog
    facCIDResEd.wndMain().UpdateDlgInfo(reddEdit.strName(), m_reddEdit.ridDlg());
}


// Called when it's time to close a dialog
tCIDLib::TVoid TResEdEditor::Reset()
{
    m_c4EditId = kCIDLib::c4MaxCard;

    // Clear the selection list and update item attrs, which will clear them out
    m_colSelList.RemoveAll();
    UpdateItemAttrs();

    // Clear the dialog oriented attrs
    m_pwndAttrPane->wndDlgAttrs().RemoveAll();

    // Tell the main frame we no longer have a dialog open
    facCIDResEd.wndMain().ClearDlgInfo();
}


//
//  We store our current dialog info to the passed resource editor data object.
//
tCIDLib::TVoid TResEdEditor::SaveChanges(TResEdData& redatTar)
{
    CIDAssert(m_c4EditId != kCIDLib::c4MaxCard, L"No active dialog changes to save");
    redatTar.bSetDialog(m_reddEdit);
}


// ---------------------------------------------------------------------------
//  TResEdEditor: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  We hvae to handle clicks, so that the underlying display window doesn't see
//  them.
//
tCIDLib::TBoolean
TResEdEditor::bClick(const  tCIDCtrls::EMouseButts  eButton
                    , const tCIDCtrls::EMouseClicks eClickType
                    , const TPoint&                 pntAt
                    , const tCIDLib::TBoolean       bCtrlShift
                    , const tCIDLib::TBoolean       bShift)
{
    // See if we hit any dialog item
    tCIDLib::TCard4 c4Index;
    TResEdDlgItem* preditHit = preditHitTest(pntAt, c4Index);

    if (eClickType == tCIDCtrls::EMouseClicks::Up)
        return kCIDLib::True;

    // We know its a down event, so take focus if we don't already have it
    if (!bHasFocus())
        TakeFocus();

    if (eButton == tCIDCtrls::EMouseButts::Left)
    {
        if (preditHit)
        {
            //
            //  If a left button click on a control, then it's a select or deselect,
            //  or possibly a drag.
            //
            tCIDLib::TCard4 c4SelInd;
            if (bIsSelected(preditHit, c4SelInd))
            {
                //
                //  This item is selected already. If control shifted,
                //  then just deselect it. Else we do a drag.
                //
                if (bCtrlShift)
                {
                    DeselectOne(c4SelInd);
                }
                 else
                {
                    //
                    //  Do a drag. If they don't actually end up dragging,
                    //  then just the clicked and released. In that case,
                    //  we de-select everything and then reselect the one
                    //  that was hit.
                    //
                    if (!bDragItems(pntAt))
                    {
                        DeselectAll();
                        SelectItem(preditHit);
                    }
                }
            }
             else
            {
                //
                //  It's not selected. If control shifted, we just
                //  add it to the select list. Else, we deselect any
                //  current stuff and make this one the new selected
                //  item.
                //
                if (!bCtrlShift)
                    DeselectAll();
                SelectItem(preditHit);
            }
        }
         else
        {
            // They clicked outside any item, so deselect all
            DeselectAll();

            //
            //  Do a hit test on ourself. If it's on an edge, and it's
            //  the bottom or right, or the bottom right corner, allow
            //  then to track the area, in order to change the size of
            //  the dialog.
            //
            //  If it's not that, then we track an area and do a lasso
            //  type selection.
            //
            if (!bDragWndSize(pntAt))
            {
                TArea areaSel;
                if (bMarqueeSelect(pntAt, areaSel, areaWndSize()))
                {
                    // Select any widget that intersects this area
                    SelectIntersecting(areaSel);
                }
            }
        }
    }
     else if (eButton == tCIDCtrls::EMouseButts::Right)
    {
        //
        //  If we hit a control and it is not selected, then deselect everything
        //  else and select the new one. If we didn't hit anything, then make sure
        //  everything is deselected.
        //
        if (preditHit)
        {
            if (m_colSelList.c4ElemCount() != 0)
            {
                tCIDLib::TCard4 c4At;
                if (!bIsSelected(preditHit, c4At))
                {
                    DeselectAll();
                    SelectItem(preditHit);
                    UpdateItemAttrs();
                }
            }
        }
         else
        {
            DeselectAll();
        }

        PopupMenu(pntAt);
    }

    return kCIDLib::True;
}


// We want to be transparent, so we have to suppress background erasing
tCIDLib::TBoolean TResEdEditor::bEraseBgn(TGraphDrawDev&)
{
    return kCIDLib::True;
}


//
//  We have to handle mouse movement, so that we can change the pointer when it's
//  over the controls and update the widget info dipslay in the status bar.
//
tCIDLib::TBoolean
TResEdEditor::bMouseMove(const  TPoint&             pntAt
                        , const tCIDLib::TBoolean   bCtrlShift
                        , const tCIDLib::TBoolean   bShift)
{
    //
    //  We need to update the display area the main frame window with the current
    //  size/pos info. If we are over a control, then we display it's pos and size.
    //  Else, we just display the position of the mouse (in dialog units)
    //
    //  We do this indirectly via the facility object. He knows where the main windwo
    //  is and passes this info along.
    //
    tCIDLib::TCard4 c4Index;
    TResEdDlgItem* preditHit = preditHitTest(pntAt, c4Index);
    if (preditHit)
    {
        // In this case, the info is already in dialog units
        facCIDResEd.wndMain().UpdateItemInfo
        (
            preditHit->strSymName()
            , preditHit->ridThis()
            , preditHit->strType()
            , preditHit->areaItem()
        );
    }
     else
    {
        facCIDResEd.wndMain().ClearItemInfo();
    }

    // Update the mouse position display
    {
        //  We have to convert the coordinates to dialog units in this case
        TPoint pntDialog(pntAt);
        TGUIFacility::AdjustWndUnits(pntDialog);
        TString strInfo(pntDialog);
        facCIDResEd.wndMain().UpdateMousePos(strInfo);
    }


    //
    //  Now we need to update the pointer based on where the mouse is over either
    //  the item we hit, or the edge of our window.
    //
    tCIDCtrls::ESysPtrs ePtr = tCIDCtrls::ESysPtrs::Arrow;
    if (preditHit)
    {
        //
        //  Set it relative to where we are on the area of the item that
        //  we are over.
        //
        TArea areaActual(preditHit->areaItem());
        TGUIFacility::AdjustDlgUnits(areaActual);
        ePtr = ePtrForArea
        (
            facCIDCtrls().eGenericHitCalc(pntAt, areaActual, 2), kCIDLib::False
        );
    }
     else
    {
        //
        //  Not over any item, so let's test relative to ourself. We'll
        //  not do anything unless it's on the edges/corner we allow to
        //  be moved.
        //
        ePtr = ePtrForArea
        (
            facCIDCtrls().eGenericHitCalc(pntAt, areaWndSize(), 2), kCIDLib::True
        );
    }
    facCIDCtrls().SetPointer(ePtr);

    return kCIDLib::True;
}


//
//  Handles redrawing. We suppress background erase above. But we still need to
//  draw selection handles.
//
tCIDLib::TBoolean
TResEdEditor::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    //
    //  Draw handles on all selected controls that intersect the update area,
    //  if not suppressed.
    //
    DrawAllSelHandles(gdevToUse, areaUpdate);
    return kCIDLib::True;
}


//
//  Do our poup menu and accellerator drive commands.
//
tCIDLib::TVoid
TResEdEditor::MenuCommand(  const   tCIDLib::TResId     ridItem
                            , const tCIDLib::TBoolean   bChecked
                            , const tCIDLib::TBoolean   bEnabled)
{
    //
    //  These are commands that come from or match our popup menu
    //
    if ((ridItem == kCIDResEd::ridMenu_DlgEdit_Back)
    ||  (ridItem == kCIDResEd::ridMenu_DlgEdit_Forward))
    {
        // There can only be one item selected, else we do nothing
        if (m_colSelList.c4ElemCount() == 1)
        {
            // Change the order of the actual windows
            m_pwndDisplay->AdjustZOrder
            (
                m_pwndDisplay->wndByEditId(m_colSelList[0]->c4EditId()).widThis()
                , (ridItem == kCIDResEd::ridMenu_DlgEdit_Forward)
            );
            m_reddEdit.AdjustZOrder
            (
                *m_colSelList[0], (ridItem == kCIDResEd::ridMenu_DlgEdit_Forward)
            );
        }
    }
     else if ((ridItem == kCIDResEd::ridMenu_DlgEdit_Copy)
          ||  (ridItem == kCIDResEd::ridMenu_DlgEdit_Cut))
    {
        CopyCutSelected(ridItem == kCIDResEd::ridMenu_DlgEdit_Cut);
    }
     else if (ridItem == kCIDResEd::ridMenu_DlgEdit_Paste)
    {
        PasteItems(facCIDCtrls().pntPointerPos(*this));
    }
     else if (ridItem == kCIDResEd::ridMenu_DlgEdit_ToFront)
    {
        // There can only be one item selected, else we do nothing
        if (m_colSelList.c4ElemCount() == 1)
        {
            m_pwndDisplay->wndByEditId(m_colSelList[0]->c4EditId()).ToTop();
            m_reddEdit.ToTop(*m_colSelList[0]);
        }
    }
     else if (ridItem == kCIDResEd::ridMenu_DlgEdit_ToBack)
    {
        // There can only be one item selected, else we do nothing
        if (m_colSelList.c4ElemCount() == 1)
        {
            m_pwndDisplay->wndByEditId(m_colSelList[0]->c4EditId()).ToBottom();
            m_reddEdit.ToBottom(*m_colSelList[0]);
        }
    }


    //
    //  These are non-menu hot keys
    //
     else if (ridItem == CIDResEd_EdWnd::c4Cmd_Delete)
    {
        DeleteSelected();
    }
     else if ((ridItem == CIDResEd_EdWnd::c4Cmd_Down)
          ||  (ridItem == CIDResEd_EdWnd::c4Cmd_Left)
          ||  (ridItem == CIDResEd_EdWnd::c4Cmd_Right)
          ||  (ridItem == CIDResEd_EdWnd::c4Cmd_Up))
    {
        tCIDLib::TInt4      i4XOfs = 0;
        tCIDLib::TInt4      i4YOfs = 0;

        if (ridItem == CIDResEd_EdWnd::c4Cmd_Down)
            i4YOfs = 1;
        else if (ridItem == CIDResEd_EdWnd::c4Cmd_Left)
            i4XOfs = -1;
        else if (ridItem == CIDResEd_EdWnd::c4Cmd_Up)
            i4YOfs = -1;
        else if (ridItem == CIDResEd_EdWnd::c4Cmd_Right)
            i4XOfs = 1;
        NudgeSelItems(i4XOfs, i4YOfs, kCIDLib::True);
    }
     else if ((ridItem == CIDResEd_EdWnd::c4Cmd_HShrink)
          ||  (ridItem == CIDResEd_EdWnd::c4Cmd_HGrow)
          ||  (ridItem == CIDResEd_EdWnd::c4Cmd_VShrink)
          ||  (ridItem == CIDResEd_EdWnd::c4Cmd_VGrow))
    {
        tCIDLib::TInt4      i4XOfs = 0;
        tCIDLib::TInt4      i4YOfs = 0;

        if (ridItem == CIDResEd_EdWnd::c4Cmd_VGrow)
            i4YOfs = 1;
        else if (ridItem == CIDResEd_EdWnd::c4Cmd_HShrink)
            i4XOfs = -1;
        else if (ridItem == CIDResEd_EdWnd::c4Cmd_VShrink)
            i4YOfs = -1;
        else if (ridItem == CIDResEd_EdWnd::c4Cmd_HGrow)
            i4XOfs = 1;
        NudgeSelItems(i4XOfs, i4YOfs, kCIDLib::False);
    }

}


// If the mouse leaves our window clear our widget/position info display
tCIDLib::TVoid TResEdEditor::MouseExit()
{
    facCIDResEd.wndMain().UpdateMousePos(TString::strEmpty());
}


// ---------------------------------------------------------------------------
//  TResEdEditor: Private, non-virtual methods
// ---------------------------------------------------------------------------


//
//  Returns an area that holds the bounds of all of the dialog items. If none,
//  it's empty.
//
TArea TResEdEditor::areaAllBounds() const
{
    TArea areaCur;
    TArea areaBounds;
    const tCIDLib::TCard4 c4Count = m_reddEdit.c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdDlgItem& reditCur = m_reddEdit.reditAt(c4Index);
        areaCur = reditCur.areaItem();
        TGUIFacility::AdjustDlgUnits(areaCur);

        // On the first one, set the initial region, then start combining
        if (c4Index)
            areaBounds |= areaCur;
        else
            areaBounds = areaCur;
    }
    return areaBounds;
}

TArea
TResEdEditor::areaAllBounds(const   tCIDLib::TInt4      i4XOfs
                            , const tCIDLib::TInt4      i4YOfs
                            , const tCIDLib::TBoolean   bOrigin) const
{
    TArea areaCur;
    TArea areaBounds;
    const tCIDLib::TCard4 c4Count = m_reddEdit.c4ChildCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdDlgItem& reditCur = m_reddEdit.reditAt(c4Index);
        areaCur = reditCur.areaItem();
        if (bOrigin)
            areaCur.AdjustOrg(i4XOfs, i4YOfs);
        else
            areaCur.AdjustSize(i4XOfs, i4YOfs);
        TGUIFacility::AdjustDlgUnits(areaCur);

        // On the first one, set the initial region, then start combining
        if (c4Index)
            areaBounds |= areaCur;
        else
            areaBounds = areaCur;
    }
    return areaBounds;
}


//
//  Returns an area that holds the bounds of all of the selected dialog
//  items. If none, it's empty. We have another one that allows an origin
//  or size adjustment (in dialog units) to be applied before the calculation.
//
TArea TResEdEditor::areaSelBounds() const
{
    TArea areaCur;
    TArea areaBounds;
    const tCIDLib::TCard4 c4Count = m_colSelList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdDlgItem* preditCur = m_colSelList[c4Index];
        areaCur = preditCur->areaItem();
        TGUIFacility::AdjustDlgUnits(areaCur);

        // On the first one, set the initial region, then start combining
        if (c4Index)
            areaBounds |= areaCur;
        else
            areaBounds = areaCur;
    }
    return areaBounds;
}

TArea
TResEdEditor::areaSelBounds(const   tCIDLib::TInt4      i4XOfs
                            , const tCIDLib::TInt4      i4YOfs
                            , const tCIDLib::TBoolean   bOrigin) const
{
    TArea areaCur;
    TArea areaBounds;
    const tCIDLib::TCard4 c4Count = m_colSelList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TResEdDlgItem* preditCur = m_colSelList[c4Index];
        areaCur = preditCur->areaItem();
        if (bOrigin)
            areaCur.AdjustOrg(i4XOfs, i4YOfs);
        else
            areaCur.AdjustSize(i4XOfs, i4YOfs);
        TGUIFacility::AdjustDlgUnits(areaCur);

        // On the first one, set the initial region, then start combining
        if (c4Index)
            areaBounds |= areaCur;
        else
            areaBounds = areaCur;
    }
    return areaBounds;
}



//
//  When the user clicks down on an item that's already selected, we do a drag
//  operation on any selected items. We return true or false to indicate whether
//  the user actually dragged or just clicked and released.
//
tCIDLib::TBoolean TResEdEditor::bDragItems(const TPoint& pntAt)
{
    // There has to be at least one item selected
    const tCIDLib::TCard4 c4Count = m_colSelList.c4ElemCount();
    CIDAssert(c4Count != 0, L"Drag called with zero items selected");

    //
    //  If there's a single item selected, then we have to deal with the
    //  fact that it may be a size operation, not a drag. So check that
    //  first. Otherwise, it's a drag of either a single or multiples.
    //
    TArea areaCur;
    if (c4Count == 1)
    {
        //
        //  Get a copy of the current dialog area and convert to window units
        //  so we can track the area.
        //
        const TArea areaOrg = m_colSelList[0]->areaItem();
        TArea areaOrgWnd = areaOrg;
        TGUIFacility::AdjustDlgUnits(areaOrgWnd);
        tCIDCtrls::EWndAreas eArea = facCIDCtrls().eGenericHitCalc(pntAt, areaOrgWnd, 2);

        //
        //  If it's not the client area, then they are sizing it by an edge or,
        //  so do that. Otherwise, we just fall through for the usual thing below.
        //
        if (eArea != tCIDCtrls::EWndAreas::Client)
        {
            // Track a copy of the original window area
            TArea areaLimit = areaWndSize();
            TArea areaTrack = areaOrgWnd;
            if (bTrackArea(pntAt, areaTrack, 2, areaLimit))
            {
                //
                //  Convert to dialog units, but we have to be careful. We
                //  only want to convert the thing's that changed, else we
                //  can get creeping issues due to rounding errors on the
                //  stuff that didn't change.
                //
                TArea areaNewDlg = areaTrack;
                TGUIFacility::AdjustWndUnits(areaNewDlg);

                // For any that didn't change, put back the original
                if (areaTrack.i4X() == areaOrgWnd.i4X())
                    areaNewDlg.i4X(areaOrg.i4X());
                if (areaTrack.i4Y() == areaOrgWnd.i4Y())
                    areaNewDlg.i4Y(areaOrg.i4Y());
                if (areaTrack.c4Width() == areaOrgWnd.c4Width())
                    areaNewDlg.c4Width(areaOrg.c4Width());
                if (areaTrack.c4Height() == areaOrgWnd.c4Height())
                    areaNewDlg.c4Height(areaOrg.c4Height());

                ResizeOneItem(0, areaNewDlg);

                // Update the attribute editor window
                m_pwndAttrPane->wndDlgAttrs().UpdateArea(kCIDResEd::strAttr_Area, areaNewDlg);
                return kCIDLib::True;
            }
            return kCIDLib::False;
        }
    }

    // Set up an area that contains the bounds of all the selected items
    TArea areaBounds = areaSelBounds();

    //
    //  Now track the area. Set the limit area to our area, since we don't
    //  want them to be able to move outside the window area.
    //
    const TArea areaLimit = areaWnd();
    TArea areaFinal(areaBounds);
    if (bTrackArea(pntAt, areaFinal, 2, areaLimit))
    {
        //
        //  They did the move, so we now just figure out the difference
        //  between the original bounds origin and the final one, and that's
        //  how much we have to adjust each control's origin. We convert
        //  that to dialog units, since we want to move in dialog units.
        //
        TPoint pntOfs = areaFinal.pntOrg() - areaBounds.pntOrg();
        TGUIFacility::AdjustWndUnits(pntOfs);
        TArea areaNew;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            TResEdDlgItem* preditCur = m_colSelList[c4Index];
            areaNew = preditCur->areaItem();
            areaNew.AdjustOrg(pntOfs);
            ResizeOneItem(c4Index, areaNew);
        }

        // And now redrawing the intersection of the new and old areas
        areaFinal |= areaBounds;
        EdRedraw(areaFinal);

        //
        //  If there's a single selected item, then that's what we were dragging
        //  so we need to update the area attribute for it.
        //
        if (m_colSelList.c4ElemCount() == 1)
        {
            m_pwndAttrPane->wndItemAttrs().UpdateArea
            (
                kCIDResEd::strAttr_Area, m_colSelList[0]->areaItem()
            );
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}



//
//  If the user clicks outside of any dialog item, this is called so that
//  we can see if they are on the window border. We allow the to drag the
//  bottom or right sides (o the BR corner.)
//
tCIDLib::TBoolean TResEdEditor::bDragWndSize(const TPoint& pntAt)
{
    TArea areaNew(areaWndSize());
    tCIDCtrls::EWndAreas eArea = facCIDCtrls().eGenericHitCalc(pntAt, areaNew, 2);

    //
    //  If it's not one of the bottom edges or the bottom right corner we
    //  care about, we're done.
    //
    if ((eArea != tCIDCtrls::EWndAreas::Right)
    &&  (eArea != tCIDCtrls::EWndAreas::Bottom)
    &&  (eArea != tCIDCtrls::EWndAreas::BottomRight))
    {
        return kCIDLib::False;
    }

    TArea areaLimit = wndParent().wndParent().areaWndSize();
    if (bTrackArea(pntAt, areaNew, 2, areaLimit))
    {
        //
        //  We want to round it to dialog unit intervals. We just round
        //  trip convert to do that.
        //
        TGUIFacility::AdjustWndUnits(areaNew);
        TGUIFacility::AdjustDlgUnits(areaNew);

        //
        //  We size the display window and ourself to match, as long as it's
        //  not too small. Either just absolutely, or too small to contain
        //  all the controls (plus some margin.) So get the bounds, then
        //  take the larger of it or a minimum size. If the new area doesn't
        //  contain that, bounce it back out to at least that.
        //
        TArea areaBounds = areaAllBounds(4, 2, kCIDLib::False);
        areaBounds.TakeLarger(TSize(31, 31));
        if (areaNew.i4Right() < areaBounds.i4Right())
            areaNew.i4Right(areaBounds.i4Right());
        if (areaNew.i4Bottom() < areaBounds.i4Bottom())
            areaNew.i4Bottom(areaBounds.i4Bottom());

        // Resize our parent
        wndParent().SetSize(areaNew.szArea(), kCIDLib::False);

        // And now update the dialog description with the area in dialog units
        TGUIFacility::AdjustWndUnits(areaNew);
        m_reddEdit.areaDlg(areaNew);

        ForceRepaint();

        // And update the displayed value in the attribute editor
        m_pwndAttrPane->wndDlgAttrs().UpdateArea(kCIDResEd::strAttr_Area, areaNew);
    }
    return kCIDLib::True;
}




// Checks to see if the passed item is in the selection list
tCIDLib::TBoolean
TResEdEditor::bIsSelected(  TResEdDlgItem* const    preditToCheck
                            , tCIDLib::TCard4&      c4SelInd)
{
    const tCIDLib::TCard4 c4Count = m_colSelList.c4ElemCount();
    for (c4SelInd = 0; c4SelInd < c4Count; c4SelInd++)
    {
        if (preditToCheck == m_colSelList[c4SelInd])
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  Find the index of the passed item in our list, i.e. the z-order.
//
tCIDLib::TCard4 TResEdEditor::c4ToIndex(const TResEdDlgItem& reditFind)
{
    const tCIDLib::TCard4 c4Count = m_reddEdit.c4ChildCount();
    tCIDLib::TCard4 c4Index = 0;
    for (; c4Index < c4Count; c4Index++)
    {
        TResEdDlgItem& reditCur = m_reddEdit.reditAt(c4Index);
        if (&reditCur == &reditFind)
            break;
    }

    // We can't not find it, so that's a nerror
    CIDAssert(c4Index != c4Count, L"Passed item was not in this dialog");
    return c4Index;
}


//
//  Copies or cuts the selected items from the current dialog and puts
//  them on the clip board.
//
tCIDLib::TVoid TResEdEditor::CopyCutSelected(const tCIDLib::TBoolean bCut)
{
    // If nothing selected, then nothing to do
    if (m_colSelList.bIsEmpty())
        return;

    //
    //  Clear the clipboard, and put the target object(s) on it. We
    //  use a special class, which is just small wrapper around a
    //  memory buffer, to flatten the objects into and then to put
    //  onto the clipboard. We can only put a single object, and there
    //  could be multiple objects selected, so we do it this way with
    //  the simple wrapper. It supports multiple types of data, but it'll
    //  know when it gets a list of dialog items what type we are doing.
    //
    try
    {
        TGUIClipboard gclipTmp(*this);
        gclipTmp.Clear();

        TResEdCBItems cbiTmp(m_colSelList);
        gclipTmp.WriteObj<TResEdCBItems>(cbiTmp);

        // If its a cut, then then delete the selected items
        if (bCut)
            DeleteSelected();
    }

    catch(const TError& errToCatch)
    {
        if (facCIDResEd.bShouldLog(errToCatch))
            TModule::LogEventObj(errToCatch);

        // Use the standard exception display dialog to show it
        TExceptDlg dlgShowErr
        (
            wndRoot()
            , facCIDResEd.strMsg(kResEdMsgs::midGen_Title1)
            , errToCatch.strErrText()
            , errToCatch
        );
    }
}


tCIDLib::TVoid TResEdEditor::DeleteSelected()
{
    const tCIDLib::TCard4 c4Count = m_colSelList.c4ElemCount();
    if (c4Count)
    {
        //
        //  Get a copy of all of the selected items list, so that we can clear
        //  the selection list before we do this. Otherwise, we'll keep
        //  redrawing selection handles over non-existant items when the display
        //  window starts redrawing the areas uncovered by the destruction
        //  of the controls.
        //
        TArea areaBounds = areaSelBounds();
        tCIDResEd::TDlgItemRefList colTmp
        (
            tCIDLib::EAdoptOpts::NoAdopt, m_colSelList.c4ElemCount()
        );
        colTmp = tCIDLib::ForceMove(m_colSelList);

        // Now we can can ask the display window to delete these controls
        m_pwndDisplay->DeleteCtrls(colTmp);

        // And now remove them from the dialog description
        m_reddEdit.DeleteItems(colTmp);

        EdRedraw(areaBounds);
        UpdateItemAttrs();
    }
}


//
//  Removes any selected controls from the selection list and redraw to get
//  rid of any of the displayed handles. To make this work, we have to
//  basically make each selected control redraw, but after we have removed
//  them from the list, so that our paint handler won't then draw them
//  back.
//
//  So we make a copy of the list, empty the real list, then go through
//  the copy and force a redraw of that area.
//
//  We have to actually make the underlying display window redraw, so that
//  it will overwrite our handles.
//
tCIDLib::TVoid TResEdEditor::DeselectAll()
{
    const tCIDLib::TCard4 c4Count = m_colSelList.c4ElemCount();
    if (c4Count)
    {
        TArea areaBounds = areaSelBounds();
        m_colSelList.RemoveAll();

        EdRedraw(areaBounds);
        UpdateItemAttrs();
    }
}


//
//  Similar to above but deselects a single item, the index of which in
//  the select list is passed.
//
tCIDLib::TVoid TResEdEditor::DeselectOne(const tCIDLib::TCard4 c4SelIndex)
{
    // Get a copy of the area, then remove from the list
    TArea areaTar = m_colSelList[c4SelIndex]->areaItem();
    m_colSelList.RemoveAt(c4SelIndex);
    UpdateItemAttrs();

    TGUIFacility::AdjustDlgUnits(areaTar);
    EdRedraw(areaTar);
}


// Draws all selection handles that interact the indicated area
tCIDLib::TVoid
TResEdEditor::DrawAllSelHandles(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    const tCIDLib::TCard4 c4Count = m_colSelList.c4ElemCount();
    if (c4Count)
    {
        //
        //  We need a solid brush to fill the handle blocks, which we fill with
        //  black. And we use a white stroke around it, so that no matter what
        //  type of background, one of them should show up.
        //
        TCosmeticPen gpenStroke(facCIDGraphDev().rgbWhite);

        //
        //  Got through all the items in our selection list and if any of them
        //  intersect the update area, draw their selection handles.
        //
        TArea areaCur;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            TResEdDlgItem* preditCur = m_colSelList[c4Index];
            areaCur = preditCur->areaItem();
            TGUIFacility::AdjustDlgUnits(areaCur);
            if (areaCur.bIntersects(areaUpdate))
                DrawSelHandles(gdevToUse, areaCur, gpenStroke);
        }
    }
}


//
//  This is called to draw the selection handles for a dialog item. We just
//  draw small squares in the four corners.
//
tCIDLib::TVoid
TResEdEditor::DrawSelHandles(       TGraphDrawDev&  gdevToUse
                            , const TArea&          areaTar
                            ,       TCosmeticPen&   gpenStroke)
{
    // If handle drawing is suppressed, then don't do it
    if (m_bHideSelHandles)
        return;

    // Set the pen on the graphic device
    TPenJanitor janPen(&gdevToUse, &gpenStroke);

    TArea areaReal(areaTar);
    areaReal.Inflate(4);

    // Draw the handles just inside the widget, on each border
    TArea areaHandle(areaReal);
    areaHandle.SetSize(5, 5);

    // Do the left side one
    areaHandle.LeftJustifyIn(areaReal, kCIDLib::True);
    gdevToUse.Fill(areaHandle, facCIDGraphDev().rgbBlack);
    gdevToUse.Stroke(areaHandle);

    // Do the right side one
    areaHandle.RightJustifyIn(areaReal, kCIDLib::True);
    gdevToUse.Fill(areaHandle, facCIDGraphDev().rgbBlack);
    gdevToUse.Stroke(areaHandle);

    // Do the top one
    areaHandle.TopJustifyIn(areaReal, kCIDLib::True);
    gdevToUse.Fill(areaHandle, facCIDGraphDev().rgbBlack);
    gdevToUse.Stroke(areaHandle);

    // Do the bottom one
    areaHandle.BottomJustifyIn(areaReal, kCIDLib::True);
    gdevToUse.Fill(areaHandle, facCIDGraphDev().rgbBlack);
    gdevToUse.Stroke(areaHandle);
}

tCIDLib::TVoid
TResEdEditor::DrawSelHandles(const TArea& areaTar)
{
    TCosmeticPen gpenStroke(facCIDGraphDev().rgbWhite);
    TGraphWndDev gdevTmp(*this);
    DrawSelHandles(gdevTmp, areaTar, gpenStroke);
}


//
//  Handles changes to attributes from the settings window. He reports them via
//  synchronous notifications.
//
tCIDCtrls::EEvResponses TResEdEditor::eAttrEditHandler(TAttrEditInfo& wnotInfo)
{
    //
    //  Depending on the source we are dealing with either the dialog or the selected
    //  item.
    //
    if (wnotInfo.strSrcName() == kCIDResEd::strWnd_DlgAttrEd)
    {
        m_reddEdit.SetAttr(*this, wnotInfo.adatNew());

        //
        //  If it was a new theme, then we have to basically destroy all of the
        //  children and recreate the, so they will adapt to the new theme.
        //
        if (wnotInfo.adatNew().strId() == kCIDResEd::strAttr_Theme)
        {
            DeselectAll();
            m_pwndDisplay->LoadDialog(m_reddEdit);
        }
         else if (wnotInfo.adatNew().strId() == kCIDResEd::strAttr_Area)
        {
            // We need to adjust our size, same as when it's done by mouse
            TArea areaNew = m_reddEdit.areaDlg();
            TGUIFacility::AdjustDlgUnits(areaNew);
            wndParent().SetSize(areaNew.szArea(), kCIDLib::False);

            ForceRepaint();
        }
         else if ((wnotInfo.adatNew().strId() == kCIDResEd::strAttr_Symbol)
              ||  (wnotInfo.adatNew().strId() == kCIDResEd::strAttr_ResId))
        {
            // We need to update the dialogs tab on the left to reflect this change
            facCIDResEd.wndMain().UpdateDlg(m_reddEdit);
        }
    }
     else if (wnotInfo.strSrcName() == kCIDResEd::strWnd_ItemAttrEd)
    {
        //
        //  There should be a single selected control. Otherwise the attr editor
        //  window shouldn't be visible.
        //
        CIDAssert(m_colSelList.c4ElemCount() == 1, L"Got attr change with > 1 selected item");

        //
        //  The only reasonable way to apply these changes is to destroy it and create
        //  it again. Do this first before we store the new value, since that might
        //  change the id or something and we wouldn't be able to find it anymore.
        //
        //  Before we kill it, we need to find the previous window, if any, so that
        //  we can put it back in the z-order where it was.
        //
        TWindow* pwndPrev = nullptr;
        TResEdDlgItem& reditCur = *m_colSelList[0];
        {
            TWindow* pwndTar = m_pwndDisplay->pwndChildByUserId(reditCur.c4EditId());
            pwndPrev = m_pwndDisplay->pwndPrevChild(*pwndTar);
            pwndTar->Destroy();
            delete pwndTar;
        }

        //
        //  Also, save the original area before we might change it, so that we can
        //  invaldiate both the old and new.
        //
        const TArea areaOrg = reditCur.areaItem();

        //  Now update the dialog item. We have one special case of setting initial
        //  focus. We need to insure that it's only set on one item. So we handle
        //  it ourself.
        //
        if ((wnotInfo.adatNew().strSpecType() == kCIDResEd::strSpecType_DlgItemFlag)
        &&  (wnotInfo.adatNew().strId() == kCIDCtrls::strDlgItemFlag_InitFocus)
        &&  (wnotInfo.adatNew().strValue() == kCIDMData::strAttrVal_True))
        {
            // This removes any previous and sets it on the target
            m_reddEdit.SetInitFocus(reditCur.c4EditId());
        }
         else
        {
            reditCur.SetAttr(*this, wnotInfo.adatNew());
        }

        // And recreate the control, putting it back after the same control
        TResEdDlgItem::InitCtrl(reditCur, m_reddEdit.eTheme(), *m_pwndDisplay, pwndPrev);

        // Update the area contains the control
        TArea areaUpdate(reditCur.areaItem());
        areaUpdate |= areaOrg;
        TGUIFacility::AdjustDlgUnits(areaUpdate);
        EdRedraw(areaUpdate);
    }
     else
    {
        TErrBox msgbErr
        (
            L"Resource Editor"
            , TStrCat(L"Unknown attr event source:", wnotInfo.strSrcName())
        );
        msgbErr.ShowIt(*this);
    }

    return tCIDCtrls::EEvResponses::Handled;
}



//
//  Just returns the pointer type that would be approriate for the window
//  or item area that the mouse was found to be over. If they tell us they
//  are over the window itself, we only allow the bottom and right stuff.
//
tCIDCtrls::ESysPtrs
TResEdEditor::ePtrForArea(  const   tCIDCtrls::EWndAreas    eArea
                            , const tCIDLib::TBoolean       bOverWindow) const
{
    //
    //  Handle the special case for ove the window. But not over one
    //  of the bits we allow for the window.
    //
    if (bOverWindow
    &&  (eArea != tCIDCtrls::EWndAreas::Right)
    &&  (eArea != tCIDCtrls::EWndAreas::Bottom)
    &&  (eArea != tCIDCtrls::EWndAreas::BottomRight))
    {
        return tCIDCtrls::ESysPtrs::Arrow;
    }

    tCIDCtrls::ESysPtrs ePtr = tCIDCtrls::ESysPtrs::Arrow;
    switch(eArea)
    {
        case tCIDCtrls::EWndAreas::Left :
        case tCIDCtrls::EWndAreas::Right :
            ePtr = tCIDCtrls::ESysPtrs::SizeWE;
            break;

        case tCIDCtrls::EWndAreas::Top :
        case tCIDCtrls::EWndAreas::Bottom :
            ePtr = tCIDCtrls::ESysPtrs::SizeNS;
            break;

        case tCIDCtrls::EWndAreas::TopLeft :
        case tCIDCtrls::EWndAreas::BottomRight :
            ePtr = tCIDCtrls::ESysPtrs::SizeNWSE;
            break;

        case tCIDCtrls::EWndAreas::TopRight :
        case tCIDCtrls::EWndAreas::BottomLeft :
            ePtr = tCIDCtrls::ESysPtrs::SizeNESW;
            break;

        default :
            ePtr = tCIDCtrls::ESysPtrs::Arrow;
            break;
    }
    return ePtr;
}


//
//  When we need to invalidate an area this is called, so that we can play various
//  tricks to deal with the layered windows.
//
tCIDLib::TVoid TResEdEditor::EdRedraw(const TArea& areaTar)
{
    TArea areaReal(areaTar);
    areaReal.Inflate(4);
    m_pwndDisplay->Redraw
    (
        areaReal, tCIDCtrls::ERedrawFlags::Full
    );

    Redraw(areaReal, tCIDCtrls::ERedrawFlags::Immediate);
}

tCIDLib::TVoid TResEdEditor::EdRedraw(const TGUIRegion& grnTar)
{
    TArea areaTar = grnTar.areaBounds();
    TArea areaReal(areaTar);
    areaReal.Inflate(4);

    m_pwndDisplay->Redraw(areaReal, tCIDCtrls::ERedrawFlags::Full);
    Redraw(areaReal, tCIDCtrls::ERedrawFlags::Immediate);
}


tCIDLib::TVoid TResEdEditor::LoadDlgAttrs()
{
    TAttrData adatTmp;
    tCIDMData::TAttrList colAttrs(16);

    //
    //  Ask the current dialog description for his attributes and load them into
    //  the dlg attributes editor.
    //
    m_reddEdit.QueryAttrs(colAttrs, adatTmp);
    m_pwndAttrPane->wndDlgAttrs().LoadValues(colAttrs, 0);
}


tCIDLib::TVoid
TResEdEditor::LoadItemAttrs(const TResEdDlgItem& reditSrc)
{
    TAttrData adatTmp;
    tCIDMData::TAttrList colAttrs(32);

    //
    //  Let the dialog item add his attrs to the list. We need to pass in the
    //  actual control as well, to figure out what hints he supports.
    //
    //  We can't depend on resource id to find them, so we use a window name that we
    //  set on the window.
    //
    TCtrlWnd* pwndSrc = nullptr;
    TWindow* pwndCur = m_pwndDisplay->pwndBottomChild();
    while (pwndCur)
    {
        if (pwndCur->strName() == reditSrc.strWndName())
        {
            pwndSrc = static_cast<TCtrlWnd*>(pwndCur);
            break;
        }
        pwndCur = m_pwndDisplay->pwndNextChild(*pwndCur);
    }

    #if CID_DEBUG_ON
    if (!pwndCur)
    {
        TString strErr(L"Could not find control with name ");
        strErr.Append(reditSrc.strWndName());
        CIDAssert2(strErr);
    }
    #endif

    reditSrc.QueryAttrs(colAttrs, adatTmp, *pwndSrc, m_reddEdit);

    // Load them into the item attributes editor
    m_pwndAttrPane->wndItemAttrs().LoadValues(colAttrs, 0);
}


//
//  This will move all of the selected items by the indicated offset, in
//  dialog units, as long as that wouldn't move any of them out of the
//  window area.
//
tCIDLib::TVoid
TResEdEditor::NudgeSelItems(const   tCIDLib::TInt4      i4XOfs
                            , const tCIDLib::TInt4      i4YOfs
                            , const tCIDLib::TBoolean   bOrigin)
{
    //
    //  Get a bounds of all of the selected items. Adjusted by the indicated
    //  offsets (in dialog units.)
    //
    TArea areaBounds = areaSelBounds(i4XOfs, i4YOfs, kCIDLib::True);

    // If it's not contained within the window area, we can't do it
    if (!areaWndSize().bContainsArea(areaBounds))
        return;

    TArea areaOrg;
    TArea areaNew;
    const tCIDLib::TCard4 c4Count = m_colSelList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TResEdDlgItem* preditCur = m_colSelList[c4Index];
        areaOrg = preditCur->areaItem();
        areaNew = areaOrg;
        if (bOrigin)
            areaNew.AdjustOrg(i4XOfs, i4YOfs);
        else
            areaNew.AdjustSize(i4XOfs, i4YOfs);

        ResizeOneItem(c4Index, areaNew);
    }

    //
    //  If there's a single selected item, then that's what we were dragging
    //  so we need to update the area attribute for it.
    //
    if (c4Count == 1)
    {
        m_pwndAttrPane->wndItemAttrs().UpdateArea
        (
            kCIDResEd::strAttr_Area, m_colSelList[0]->areaItem()
        );
    }
}



//
//  Tests the passed point to see if it is over any of the controls. WE just
//  run through the current dialog items, converting their areas to window
//  coordinates and test the passed ponit against it.
//
TResEdDlgItem*
TResEdEditor::preditHitTest(const TPoint& pntTest, tCIDLib::TCard4& c4Index)
{
    //
    //  We have to go backwards since we need to find the highest z-order one
    //  that contains the point.
    //
    TArea areaCur;
    tCIDLib::TInt4 i4Index = tCIDLib::TInt4(m_reddEdit.c4ChildCount());
    if (!i4Index)
        return 0;

    TResEdDlgItem* preditRet = nullptr;
    while (i4Index)
    {
        i4Index--;
        TResEdDlgItem& reditCur = m_reddEdit.reditAt(i4Index);
        areaCur = reditCur.areaItem();
        TGUIFacility::AdjustDlgUnits(areaCur);
        if (areaCur.bContainsPoint(pntTest))
        {
            c4Index = tCIDLib::TCard4(i4Index);
            preditRet = &reditCur;
            break;
        }
    }
    return preditRet;
}


//
//  Called from the command handler when the user asks to paste. The paste
//  command ins only enabled if there's actually the required data on the
//  clip board. We just get the data out, and paste them in, relative to
//  the mouse position and adjusting symbol names to avoid dups.
//
tCIDLib::TVoid TResEdEditor::PasteItems(const TPoint& pntAt)
{
    // Try to read a dialog item clipboard object of the clipboard
    TResEdCBItems* pciTmp = nullptr;
    try
    {
        TGUIClipboard gclipPaste(*this);
        if (!gclipPaste.bReadObj<TResEdCBItems>(pciTmp))
        {
            // Clear the clipboard since something isn't right, and return
            gclipPaste.Clear();
            return;
        }
    }

    catch(const TError& errToCatch)
    {
        if (facCIDResEd.bShouldLog(errToCatch))
            TModule::LogEventObj(errToCatch);

        // Use the standard exception display dialog to show it
        TExceptDlg dlgShowErr
        (
            wndRoot()
            , facCIDResEd.strMsg(kResEdMsgs::midGen_Title1)
            , errToCatch.strErrText()
            , errToCatch
        );
        return;
    }

    // Make sure it get's cleaned up
    TJanitor<TResEdCBItems> janTmp(pciTmp);

    // Convert the position to dialog units
    TPoint pntDlgOrg = pntAt;
    TGUIFacility::AdjustWndUnits(pntDlgOrg);

    // If no items in the list, nothing to do
    const tCIDResEd::TDlgItemList colItems = pciTmp->colItems();
    const tCIDLib::TCard4 c4Count = colItems.c4ElemCount();
    if (!c4Count)
        return;

    //
    //  In order to correctly offset them, we have to get the area that
    //  contains all of the items. These are all dialog units.
    //
    TArea areaHull = colItems[0].areaItem();
    for (tCIDLib::TCard4 c4Index = 1; c4Index < c4Count; c4Index++)
        areaHull |= colItems[c4Index].areaItem();

    // Ok, let's do all the items
    TArea               areaNew;
    TString             strSym;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TResEdDlgItem reditCur = colItems[c4Index];

        // If the symbol is already taken, try to find another one
        strSym = reditCur.strSymName();
        if (!strSym.bIsEmpty())
        {
            tCIDLib::TCard4 c4Append = 1;
            while (m_reddEdit.preditBySymName(strSym))
            {
                strSym = reditCur.strSymName();
                strSym.AppendFormatted(c4Append++);
            }

            //
            //  OK, update the current one with the name we found, which
            //  may be the original but not worth checking. Then add it
            //  to our list.
            //
            reditCur.strSymName(strSym);
        }

        //
        //  We have to find an available resource id as well. If the current one is
        //  taken, then create a new one.
        //
        if (m_reddEdit.bResIdUsed(reditCur.ridThis()))
            reditCur.ridThis(ridNextAvail());

        // We have to give it a new edit id
        reditCur.c4EditId(facCIDResEd.c4NextEditId());

        //
        //  Adjust the origin. We get its offset from the original of the
        //  hull, then add to that the mouse position.
        //
        areaNew = reditCur.areaItem();
        areaNew.SetOrg(areaNew.pntOrg() - areaHull.pntOrg());
        areaNew.AdjustOrg(pntDlgOrg);
        reditCur.areaItem(areaNew);

        // Now create the new control
        TResEdDlgItem::InitCtrl(reditCur, m_reddEdit.eTheme(), *m_pwndDisplay, nullptr);

        // And add it to the dialog descrition
        m_reddEdit.c4AddNew(reditCur);

        // Force a redraw of this area
        m_pwndDisplay->Redraw(reditCur.areaItem(), tCIDCtrls::ERedrawFlags::Full);
    }
}


//
//  The mouse click method calls this if it's time to do the popup menu.
//
tCIDLib::TVoid TResEdEditor::PopupMenu(const TPoint& pntAt)
{
    //
    //  We need to pop up the popup menu. Depending on whether one or
    //  more than one control is selected, we enable/disable some items.
    //  Put the bottom of the menu at the click point.
    //
    //  We need to load up the New submenu with the list of available
    //  control types.
    //
    TPopupMenu menuPopup(L"control popup");
    menuPopup.Create(facCIDResEd, kCIDResEd::ridMenu_DlgEdit);
    tCIDLib::TResId ridNextId = kCIDResEd::ridFirstDynMenu;
    tCIDLib::TKVPList::TCursor cursTypes(&facCIDCtrls().colDlgClasses());
    for (; cursTypes; ++cursTypes)
    {
        const TKeyValuePair& kvalCur = *cursTypes;
        menuPopup.AddActionItem
        (
            kCIDResEd::ridMenu_DlgEdit_New
            , kvalCur.strValue()
            , kvalCur.strKey()
            , ridNextId
        );
        ridNextId++;
    }

    //
    //  If there's something on the clipboard, remember it for below. It
    //  can fail if someone else hasn't released it, so deal with that.
    //
    tCIDLib::TBoolean bCBData = kCIDLib::False;
    TString strCBClass;
    {
        try
        {
            TGUIClipboard gclipTmp(*this);
            bCBData = gclipTmp.bGetAsClass(strCBClass);

            if (!bCBData || (strCBClass != L"TResEdCBItems"))
                bCBData = kCIDLib::False;
        }

        catch(const TError& errToCatch)
        {
            if (facCIDResEd.bShouldLog(errToCatch))
                TModule::LogEventObj(errToCatch);
        }
    }

    // Enable/disable items based on whether a single or multiples selected
    if (m_colSelList.c4ElemCount() == 0)
    {
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_Back, kCIDLib::False);
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_Forward, kCIDLib::False);
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_New, kCIDLib::True);
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_ToBack, kCIDLib::False);
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_ToFront, kCIDLib::False);
    }
     else if (m_colSelList.c4ElemCount() == 1)
    {
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_Back, kCIDLib::True);
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_Forward, kCIDLib::True);
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_New, kCIDLib::False);
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_ToBack, kCIDLib::True);
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_ToFront, kCIDLib::True);
    }
     else
    {
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_Back, kCIDLib::False);
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_Forward, kCIDLib::False);
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_New, kCIDLib::False);
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_ToFront, kCIDLib::False);
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_ToBack, kCIDLib::False);
    }

    // And enable/disable based on whether we have data to paste
    if (bCBData)
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_Paste, kCIDLib::True);
    else
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_Paste, kCIDLib::False);

    // Enable/disable copy and cut based on whether there's selected items
    if (m_colSelList.bIsEmpty())
    {
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_Copy, kCIDLib::False);
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_Cut, kCIDLib::False);
    }
     else
    {
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_Copy, kCIDLib::True);
        menuPopup.SetItemEnable(kCIDResEd::ridMenu_DlgEdit_Cut, kCIDLib::True);
    }

    // Convert the point to screen coordinates
    TPoint pntScr;
    ToScreenCoordinates(pntAt, pntScr);
    const tCIDLib::TCard4 c4Cmd = menuPopup.c4Process
    (
        *this, pntScr, tCIDLib::EVJustify::Bottom
    );

    //
    //  The ones that are sensitive to mouse position, we handle here. The ohers
    //  we just pass to the menu command handler, so that it's handled the same
    //  for this menu and for hot keys.
    //
    if ((c4Cmd >= kCIDResEd::ridFirstDynMenu)
    &&  (c4Cmd <= kCIDResEd::ridLastDynMenu))
    {
        //
        //  It's one of the values in the New submenu, so we need to create
        //  one and place it at the mouse position.
        //
        const TKeyValuePair& kvalClass = facCIDCtrls().colDlgClasses()
        [
            c4Cmd - kCIDResEd::ridFirstDynMenu
        ];

        // Get the next available window id to use
        const tCIDLib::TResId ridToUse = ridNextAvail();

        //
        //  We have to play a trick here. We need to get a default size for this
        //  guy, but we need to create one before we can get that default size. So
        //  we create an object, but don't create the window. Thats enough to
        //  ask it for the default size.
        //
        TArea areaInit;
        {
            TString strWndClass = facCIDCtrls().strXlatDlgClass(kvalClass.strKey());
            TCtrlWnd* pwndNew = ::pobjMakeNewOfClass<TCtrlWnd>(strWndClass);
            TJanitor<TCtrlWnd> janNew(pwndNew);
            areaInit.Set(pntAt, pwndNew->szDefault());
        }

        //
        //  Make sure it's within the our area. This will move and even resize it
        //  to make it fit.
        //
        areaInit.ForceWithin(areaWndSize(), kCIDLib::True);

        // Now get a dialog units version of it
        TArea areaDlgU(areaInit);
        TGUIFacility::AdjustWndUnits(areaDlgU);

        //
        //  Before we create the new control, the user has to provide initial config. So
        //  create a new dialog item object, with the basic info we have so far. We have
        //  to create a default initial symbol, which we'll base off of the overall dialog
        //  symbol, which is how it's done in CIDWorld.
        //
        TResEdDlgItem reditNew(kvalClass.strKey(), areaDlgU, ridToUse);

        TString strDefSym(m_reddEdit.strName());
        strDefSym.Append(kCIDLib::chUnderscore);
        strDefSym.AppendFormatted(ridToUse);
        reditNew.strSymName(strDefSym);

        // OK, create the control and initialize it with the default info
        TResEdDlgItem::InitCtrl
        (
            reditNew, m_reddEdit.eTheme(), *m_pwndDisplay, nullptr
        );

        //
        //  It worked so update the current dialog description with this new guy.
        //  He just gets added to the end of the list.
        //
        const tCIDLib::TCard4 c4NewInd = m_reddEdit.c4AddNew(reditNew);

        // And select it
        m_colSelList.RemoveAll();
        m_colSelList.Add(&m_reddEdit.reditAt(c4NewInd));

        // Force a redraw of this area, in window units in this case of course
        TGUIFacility::AdjustDlgUnits(areaInit);

        EdRedraw(areaInit);

        // It's now the only selecetd one, so load it into the item attrs editor
        UpdateItemAttrs();
    }
     else if (c4Cmd == kCIDResEd::ridMenu_DlgEdit_Paste)
    {
        PasteItems(pntAt);
    }
     else if (c4Cmd != kCIDLib::c4MaxCard)
    {
        //
        //  Fake a call to menu command, since we get hot keys there, and can
        //  just do it all in one place.
        //
        MenuCommand(c4Cmd, kCIDLib::False, kCIDLib::True);
    }
}


// Find the next available window id we could use for a new control
tCIDLib::TResId TResEdEditor::ridNextAvail() const
{
    // If no controls yet, then just return one
    const tCIDLib::TCard4 c4Count = m_reddEdit.c4ChildCount();
    if (!c4Count)
        return 1;

    //
    //  More than one, so let's find an unused value, so just fill in a vector
    //  with all of the current ids in this dialog.
    //
    TFundVector<tCIDCtrls::TWndId> fcolVals(c4Count);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        fcolVals.c4AddElement(m_reddEdit.reditAt(c4Index).ridThis());

    // Now just find the first unused value past the default first control id
    tCIDLib::TResId ridRet = kCIDCtrls::widFirstCtrl;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        // If the current test id is not present in the list, take it
        if (!fcolVals.bElementPresent(ridRet))
            break;
        ridRet++;
    }

    // Return the id we ended up on
    return ridRet;
}


tCIDLib::TVoid
TResEdEditor::ResizeOneItem(const   tCIDLib::TCard4 c4SelInd
                            , const TArea&          areaNewDU)
{
    // If it's overly small, just ignore it
    if ((areaNewDU.c4Width() < 2) || (areaNewDU.c4Height() < 2))
        return;

    // Get the equiv actual pixel area
    TArea areaNew(areaNewDU);
    TGUIFacility::AdjustDlgUnits(areaNew);

    // And the same for the original area
    TArea areaOrg = m_colSelList[c4SelInd]->areaItem();
    TGUIFacility::AdjustDlgUnits(areaOrg);

    // Suppress handles and redraw the original area
    {
        TBoolJanitor janHandles(&m_bHideSelHandles, kCIDLib::True);
        EdRedraw(areaOrg);

        // Now do the operation on the actual control
        m_pwndDisplay->ResizeCtrl(*m_colSelList[c4SelInd], areaNew);

        // And now update our dialog item
        m_colSelList[c4SelInd]->areaItem(areaNewDU);
    }

    // And we can now redraw the new area to get the handles back
    EdRedraw(areaNew);
}


//
//  Select all widgets that intersect the passed area. We'll deselect any
//  current ones just in case the caller didn't do that first.
//
tCIDLib::TVoid TResEdEditor::SelectIntersecting(const TArea& areaTest)
{
    DeselectAll();

    // If we don't have any children, nothing to do yet
    tCIDLib::TInt4 i4Index = tCIDLib::TInt4(m_reddEdit.c4ChildCount());
    if (!i4Index)
        return;

    //
    //  We need to accumulate a region that contains all of the selection
    //  areas so we can redraw them at the end.
    //
    TGUIRegion grgnUpdate;

    //
    //  We go backwards so that we get them selected in z-order.
    //
    TArea areaCur;
    while (i4Index)
    {
        i4Index--;
        TResEdDlgItem& reditCur = m_reddEdit.reditAt(i4Index);
        areaCur = reditCur.areaItem();
        TGUIFacility::AdjustDlgUnits(areaCur);
        if (areaCur.bIntersects(areaTest))
        {
            if (m_colSelList.bIsEmpty())
                grgnUpdate.Set(areaCur);
            else
                grgnUpdate.CombineWith(areaCur, tCIDGraphDev::EClipModes::Or);

            m_colSelList.Add(&reditCur);
        }
    }

    // If any, then redraw
    if (!m_colSelList.bIsEmpty())
        EdRedraw(grgnUpdate);
}


//
//  This is called to add a dialog item to the selection list. We add it to
//  our collection of selected items, and force a redraw of it's area so
//  cause the selection handles to be redrawn.
//
tCIDLib::TVoid TResEdEditor::SelectItem(TResEdDlgItem* const preditSel)
{
    m_colSelList.Add(preditSel);
    UpdateItemAttrs();

    TArea areaRefresh = preditSel->areaItem();
    TGUIFacility::AdjustDlgUnits(areaRefresh);
    EdRedraw(areaRefresh);
}


//
//  This is called any time we change the selection list. We either clear the
//  attributes tab, or load it if a single item is selected.
//
tCIDLib::TVoid TResEdEditor::UpdateItemAttrs()
{
    if (m_colSelList.c4ElemCount() != 1)
        m_pwndAttrPane->wndItemAttrs().RemoveAll();
    else
        LoadItemAttrs(*m_colSelList[0]);
}




// ---------------------------------------------------------------------------
//   CLASS: TResEdCont
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TResEdCont: Constructors and Destructor
// -------------------------------------------------------------------
TResEdCont::TResEdCont() :

    TWindow(kCIDLib::False)
    , m_pwndDisplay(0)
    , m_pwndEditor(0)
{
}

TResEdCont::~TResEdCont()
{
}


// -------------------------------------------------------------------
//  TResEdCont: Public, inherited methods
// -------------------------------------------------------------------

tCIDLib::TBoolean TResEdCont::bProcessAccel(const tCIDLib::TVoid* const pMsgData) const
{
    if (m_pwndEditor && m_pwndEditor->bContainsFocus())
        return m_pwndEditor->bProcessAccel(pMsgData);

    return kCIDLib::False;
}



// -------------------------------------------------------------------
//  TResEdCont: Public, non-virtual methods
// -------------------------------------------------------------------

// Just pass it on to our editor window
tCIDLib::TBoolean TResEdCont::bChanges() const
{
    return m_pwndEditor->bChanges();
}


tCIDLib::TVoid
TResEdCont::Create( const   TScrollArea&        wndParent
                    , const tCIDCtrls::TWndId   widThis
                    ,       TResAttrEdPane*     pwndAttrPane)
{
    CreateWnd
    (
        wndParent.hwndSafe()
        , kCIDCtrls::pszCustClass
        , L""
        , TArea(0, 0, 16, 16)
        , tCIDCtrls::EWndStyles::Child
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );

    // Create the display window and register it
    m_pwndDisplay = new TResEdDisplay();
    m_pwndDisplay->Create(*this, kCIDCtrls::widFirstCtrl);

    // Create the editor window and register it
    m_pwndEditor = new TResEdEditor();
    m_pwndEditor->Create(*this, kCIDCtrls::widFirstCtrl + 1, m_pwndDisplay, pwndAttrPane);

    //
    //  We had to create the display first, so we could pass a poitner to it to the
    //  editor. But we want the editor on top.
    //
    m_pwndEditor->ToTop();
}


tCIDLib::TVoid
TResEdCont::EditDlg(const TResEdDlgDesc& reddEdit)
{
    //
    //  Let's size ourself to the size indicated in the dialog description. Our
    //  parent will get a notification that we changed size and update the scroll
    //  bars as required.
    //
    TArea areaDlg(reddEdit.areaDlg());
    TGUIFacility::AdjustDlgUnits(areaDlg);
    SetSize(areaDlg.szArea(), kCIDLib::True);

    // Pass the info to the display window
    m_pwndDisplay->LoadDialog(reddEdit);

    // And to the editor
    m_pwndEditor->EditDlg(reddEdit);

    // Make sure we are visible. We are hidden until a dialog is edited.
    SetVisibility(kCIDLib::True);
}


//
//  Pass it on to our two child windows. If asked, hide us.
//
tCIDLib::TVoid TResEdCont::Reset(const tCIDLib::TBoolean bHide)
{
    m_pwndDisplay->Reset();
    m_pwndEditor->Reset();

    if (bHide)
        SetVisibility(kCIDLib::False);
}



TResEdEditor& TResEdCont::wndEditor()
{
    CIDAssert(m_pwndEditor != nullptr, L"The editor window has not been set yet")
    return *m_pwndEditor;
}



// -------------------------------------------------------------------
//  TResEdCont: Protected, inherited methods
// -------------------------------------------------------------------

// WE need to keep our contained windows sized to fit us
tCIDLib::TVoid
TResEdCont::AreaChanged(const   TArea&                  areaPrev
                        , const TArea&                  areaNew
                        , const tCIDCtrls::EPosStates   ePosState
                        , const tCIDLib::TBoolean       bOrgChanged
                        , const tCIDLib::TBoolean       bSizeChanged
                        , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    //
    //  Keep our child windows sized to fit. They may not have been created yet
    //  so we have check.
    //
    if (m_pwndDisplay && bSizeChanged)
        m_pwndDisplay->SetSize(areaNew.szArea(), kCIDLib::True);

    if (m_pwndEditor && bSizeChanged)
        m_pwndEditor->SetSize(areaNew.szArea(), kCIDLib::True);
}


