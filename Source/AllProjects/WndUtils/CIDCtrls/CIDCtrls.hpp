//
// FILE NAME: CIDCtrls.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/02/1997
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
//  This is the main public header for the facility. It is the single point
//  of contact for the outside world. By including it, they get what we
//  have to offer.
//
//  This facility is the core UI framework. This stuff is all wrapped just as
//  well as the back end stuff, but there is not much hope of making it per-platform.
//  So guy will only be built on Win32.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Set up our import/export attributes
// ---------------------------------------------------------------------------
#if     defined(PROJ_CIDCTRLS)
#define CIDCTRLSEXP  DLLEXPORT
#else
#define CIDCTRLSEXP  DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our needed underlying headers.
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"
#include    "CIDMData.hpp"
#include    "CIDGraphDev.hpp"


// ---------------------------------------------------------------------------
//  Include our types headers first since we referenced some of these types in
//  the constants namespace below. One of these is actually defined down in
//  CIDKernel, so that the build tool can use those types.
// ---------------------------------------------------------------------------
#include    "CIDKernel_Type_Ctrls.hpp"
#include    "CIDCtrls_Type.hpp"


// ---------------------------------------------------------------------------
//  Facility constants
// ---------------------------------------------------------------------------
namespace kCIDCtrls
{
    // -----------------------------------------------------------------------
    //  Some special window ids. The OK/Cancel ones have to be these values in
    //  order for standard focus management to work. We provide FirstCtrl as
    //  an arbitrary value to start assigning non-special child controls ids,
    //  so that we know they won't interfere with anything special.
    // -----------------------------------------------------------------------
    constexpr tCIDCtrls::TWndId     widOK           = 1;
    constexpr tCIDCtrls::TWndId     widCancel       = 2;

    constexpr tCIDCtrls::TWndId     widFirstCtrl    = 1000;


    // -----------------------------------------------------------------------
    //  Some internal message ids we define. These are actually in the application
    //  specific range, so that we avoid any possible conflicts with standard controls
    //  we might sub-class and the like. Since we don't use messages for custom
    //  windows (we use virtual methods), these are application-wide anyway.
    //
    //  If any do use msgs, then should be between wmsgFirstUser and wmsgLastUser.
    //  Any messages received with these values will be sent to the UserMsg() virtual
    //  method.
    // -----------------------------------------------------------------------
    constexpr tCIDCtrls::TWndMsg    wmsgAsyncEvent1     = 0x8000;
    constexpr tCIDCtrls::TWndMsg    wmsgAsyncEvent2     = 0x8001;
    constexpr tCIDCtrls::TWndMsg    wmsgSyncEvent1      = 0x8002;
    constexpr tCIDCtrls::TWndMsg    wmsgChildFocusNot   = 0x8003;
    constexpr tCIDCtrls::TWndMsg    wmsgFirstUser       = 0x8100;
    constexpr tCIDCtrls::TWndMsg    wmsgLastUser        = 0x8200;


    // ------------------------------------------------------------------------
    //  Some GUI oriented character constants
    // ------------------------------------------------------------------------
    constexpr tCIDLib::TCh          chMnemonic      = L'&';


    // ------------------------------------------------------------------------
    //  The maximum chars in a notification id
    // ------------------------------------------------------------------------
    constexpr tCIDLib::TCard4       c4MaxNotIdLen   = 63;


    // ------------------------------------------------------------------------
    //  The maximum number of frames that the pane window manager class will
    //  manage.
    // ------------------------------------------------------------------------
    constexpr tCIDLib::TCard4       c4MaxPaneWnds   = 3;


    // ------------------------------------------------------------------------
    //  Defaults for window sizes and positions.
    // ------------------------------------------------------------------------
    constexpr tCIDCtrls::TWndCoord  wcrDefault      = kCIDLib::i4MinInt;
    constexpr tCIDCtrls::TWndSize   wszDefault      = kCIDLib::c4MaxCard;


    // ------------------------------------------------------------------------
    //  We create two window classes for any custom windows. The class used doesn't
    //  much matter, we just need a class, so as to be able to create windows and
    //  point them at our internal dispatcher. We create one for windows that want
    //  horz/vert resize redrawing, and one for ones that don't.
    // ------------------------------------------------------------------------
    const tCIDLib::TCh* const   pszCustClass      = L"CIDCustWindow";
    const tCIDLib::TCh* const   pszCustClassHV    = L"CIDCustWindowHV";


    // -----------------------------------------------------------------------
    //  The max number of positions we'll capture for determinging gesture inertia.
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4       c4InertiaBufSz = 16;


    // -----------------------------------------------------------------------
    //  The maximum number of timers per window
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4       c4MaxWndTimers = 32;
};


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class TGUIFacility;
class TFrameWnd;
class TMenu;
class TWindow;
class TCaretJanitor;

// Slip in some facility types
namespace tCIDCtrls
{
    using TWndList      = TRefVector<const TWindow>;
    using TKeyedWndList = TRefKeyedHashSet<TWindow,TString,TStringKeyOps>;
    using THintsList    = tCIDLib::TStrList;
}


// ---------------------------------------------------------------------------
//  Include the rest of our public headers
// ---------------------------------------------------------------------------
#include    "CIDCtrls_ErrorIds.hpp"
#include    "CIDCtrls_ResourceIds.hpp"
#include    "CIDCtrls_Shared.hpp"

#include    "CIDCtrls_NotificationId.hpp"
#include    "CIDCtrls_NotificationHandler.hpp"
#include    "CIDCtrls_NotificationEvent.hpp"
#include    "CIDCtrls_NotificationRequest.hpp"

#include    "CIDCtrls_AccelTable.hpp"
#include    "CIDCtrls_DlgDesc.hpp"
#include    "CIDCtrls_ImageList.hpp"
#include    "CIDCtrls_WndDevice.hpp"
#include    "CIDCtrls_Clipboard.hpp"
#include    "CIDCtrls_DragDropIntf.hpp"
#include    "CIDCtrls_GUIShell.hpp"

#include    "CIDCtrls_WndState.hpp"
#include    "CIDCtrls_Window.hpp"
#include    "CIDCtrls_WndMultiPos.hpp"

#include    "CIDCtrls_PaneWndMgr.hpp"
#include    "CIDCtrls_ToolTip.hpp"
#include    "CIDCtrls_Menu.hpp"

#include    "CIDCtrls_GestHandler.hpp"
#include    "CIDCtrls_TouchWnd.hpp"

#include    "CIDCtrls_InPlaceEdit.hpp"

#include    "CIDCtrls_CtrlWindow.hpp"
#include    "CIDCtrls_GenericWnd.hpp"
// #include    "CIDCtrls_ColHeader.hpp"
#include    "CIDCtrls_ScrollBar.hpp"
#include    "CIDCtrls_Button.hpp"
#include    "CIDCtrls_CheckBox.hpp"
#include    "CIDCtrls_PushButton.hpp"
#include    "CIDCtrls_RadioButton.hpp"
#include    "CIDCtrls_ListWnd.hpp"
#include    "CIDCtrls_ComboBox.hpp"
#include    "CIDCtrls_ListBox.hpp"
// #include    "CIDCtrls_MColListBox.hpp"
#include    "CIDCtrls_MultiColListBox.hpp"
#include    "CIDCtrls_Calendar.hpp"
#include    "CIDCtrls_EntryField.hpp"
#include    "CIDCtrls_MultiEdit.hpp"
#include    "CIDCtrls_ObjectView.hpp"
#include    "CIDCtrls_ProgressBar.hpp"
#include    "CIDCtrls_ScrollArea.hpp"
#include    "CIDCtrls_Slider.hpp"
#include    "CIDCtrls_Separator.hpp"
#include    "CIDCtrls_Spin.hpp"
#include    "CIDCtrls_StaticColor.hpp"
#include    "CIDCtrls_StaticImage.hpp"
#include    "CIDCtrls_StaticText.hpp"
#include    "CIDCtrls_StaticMultiText.hpp"
#include    "CIDCtrls_StatusBar.hpp"
#include    "CIDCtrls_TabbedWnd.hpp"
#include    "CIDCtrls_TreeView.hpp"
#include    "CIDCtrls_TrayAppWnd.hpp"

#include    "CIDCtrls_DlgBox.hpp"
#include    "CIDCtrls_FrameWnd.hpp"
#include    "CIDCtrls_ExceptDlg.hpp"
#include    "CIDCtrls_MessageBox.hpp"

#include    "CIDCtrls_GUIFacility.hpp"
#include    "CIDCtrls_ThisFacility.hpp"



// ---------------------------------------------------------------------------
//  Export the facility object lazy evaluator function.
// ---------------------------------------------------------------------------
extern CIDCTRLSEXP TFacCIDCtrls& facCIDCtrls();


#define CIDLib_CtrlsModule(thrCtor) \
int __stdcall WinMain(  tCIDLib::TVoid* hmodThis \
                        , tCIDLib::TVoid* \
                        , tCIDLib::TSCh* \
                        , tCIDLib::TSInt sShowState) \
{ \
    CIDLib_Init(); \
    TThread* pthrMain = new thrCtor; \
    facCIDCtrls().SetInitShowState(sShowState); \
    CIDLib_MakePrimary(pthrMain); \
    pthrMain->MarkAsGUIThread(); \
    pthrMain->Start(); \
    return int(pthrMain->eWaitForDeath()); \
}



// ---------------------------------------------------------------------------
//  For those collection types that we pre-generate in our main cpp file, we want to
//  insure that they don't get generated again in users of the DLL.
// ---------------------------------------------------------------------------
#if     !defined(CIDSCTRLS_PREINST)
extern template class TVector<TAttrData>;
extern template class TVector<TDlgItem>;
extern template class TVector<TWndState>;
extern template class TRefVector<TWindow>;
extern template class TRefKeyedHashSet<TWindow,TString,TStringKeyOps>;
#endif


