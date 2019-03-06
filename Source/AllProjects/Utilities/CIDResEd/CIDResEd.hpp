//
// FILE NAME: CIDResEd.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/10/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main header for the dialog editor program. It sub-includes
//  all of the other programs headers and the needed headers for underlying
//  facilities.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Include the needed underlying facility headers
// ----------------------------------------------------------------------------
#include    "CIDGraphDev.hpp"

#include    "CIDCtrls.hpp"

#include    "CIDWUtils.hpp"


// ----------------------------------------------------------------------------
//  Facility constants
// ----------------------------------------------------------------------------
namespace kCIDResEd
{
    // ------------------------------------------------------------------------
    //  Indices for the slots we put into the status bar (at the bottom of the
    //  main frame.)
    // ------------------------------------------------------------------------
    const tCIDCtrls::TWndId c4SBar_DlgInfo   = 0;
    const tCIDCtrls::TWndId c4SBar_ItemInfo  = 1;
    const tCIDCtrls::TWndId c4SBar_MousePos  = 2;


    // ------------------------------------------------------------------------
    //  The base id for the menu items we load into the New submenu of the popup
    //  menu. We load up an entry for each available control type we can create.
    // ------------------------------------------------------------------------
    const tCIDLib::TResId   ridFirstDynMenu = 2000;
    const tCIDLib::TResId   ridLastDynMenu  = 2300;


    // ------------------------------------------------------------------------
    //  The names we give to some our windows, in some cases so that they can be
    //  distinguished if they send notifications to the same (non-parent) window.
    // ------------------------------------------------------------------------
    const TString           strWnd_DlgAttrEd(L"DlgAttrEditor");
    const TString           strWnd_ItemAttrEd(L"ItemAttrEditor");
    const TString           strWnd_Tab_Dialogs(L"DialogsTab");
};



// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
class TResEdMainFrame;


// ----------------------------------------------------------------------------
//  And include our own headers
// ----------------------------------------------------------------------------
#include    "CIDResEd_MessageIds.hpp"
#include    "CIDResEd_ErrorIds.hpp"
#include    "CIDResEd_ResourceIds.hpp"
#include    "CIDResEd_Types.hpp"

#include    "CIDResEd_FileSpooler.hpp"
#include    "CIDResEd_BaseResData.hpp"
#include    "CIDResEd_DlgItem.hpp"
#include    "CIDResEd_ImgData.hpp"
#include    "CIDResEd_MenuData.hpp"
#include    "CIDResEd_MsgData.hpp"

// Slip some more types in
namespace tCIDResEd
{
    typedef TVector<TResEdDlgItem>      TDlgItemList;
    typedef TRefVector<TResEdDlgItem>   TDlgItemRefList;
}

#include    "CIDResEd_DlgDescr.hpp"

namespace tCIDResEd
{
    typedef TVector<TResEdDlgDesc>  TDlgDescList;
}

#include    "CIDResEd_CBData.hpp"
#include    "CIDResEd_ResData.hpp"
#include    "CIDResEd_EditTextDlg.hpp"
#include    "CIDResEd_NewDlgDlg.hpp"
#include    "CIDResEd_AttrEdit.hpp"
#include    "CIDResEd_ProjectTabs.hpp"
#include    "CIDResEd_EditWnd.hpp"
#include    "CIDResEd_MainFrame.hpp"
#include    "CIDResEd_ThisFacility.hpp"


// ----------------------------------------------------------------------------
//  Global data from CIDResEd.cpp
// ----------------------------------------------------------------------------
extern TFacCIDResEd     facCIDResEd;


