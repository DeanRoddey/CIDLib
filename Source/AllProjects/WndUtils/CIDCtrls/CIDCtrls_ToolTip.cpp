//
// FILE NAME: CIDCtrls_ToolTip.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/26/2015
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
//  This file implements the tooltip class.
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
AdvRTTIDecls(TToolTip, TCtrlWnd)



// ---------------------------------------------------------------------------
//  CLASS: TToolTip
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TToolTip: Constructors and Destructor
// ---------------------------------------------------------------------------
TToolTip::TToolTip() :

    TWindow(kCIDLib::False)
{
}

TToolTip::~TToolTip()
{
}


// ---------------------------------------------------------------------------
// TToolTip: Public, non-virtual methods
// ---------------------------------------------------------------------------


//
//  Create us and do basic setup of the tool tip. If we got an iconwe set the title
//  and icon.
//
tCIDLib::TVoid
TToolTip::Create(const  TWindow&                wndOwner
                , const TString&                strTitle
                , const tCIDCtrls::ETTipIcons   eIcon)
{
    // And create the cnotrol
    CreateWnd
    (
        wndOwner.hwndThis()
        , TOOLTIPS_CLASS
        , kCIDLib::pszEmptyZStr
        , facCIDCtrls().areaDefWnd()
        , tCIDCtrls::EWndStyles(WS_POPUP | TTS_BALLOON) //| TTS_ALWAYSTIP
        , tCIDCtrls::EExWndStyles::None
        , 0
    );

    // All standard controls must be subclassed
    SetSubclass();

    // Set up the tool tip association, but not text yet
    TOOLINFO Info = {0};
    Info.cbSize = sizeof(TOOLINFO);
    Info.hwnd = wndOwner.hwndSafe();
    Info.uFlags = TTF_TRACK;
    ::SendMessage(hwndSafe(), TTM_ADDTOOL, 0, tCIDCtrls::TLParam(&Info));

    // Set up title and icon stuff if set
    if (eIcon != tCIDCtrls::ETTipIcons::None)
    {
        tCIDCtrls::TWParam wParam;
        if (eIcon == tCIDCtrls::ETTipIcons::Info)
            wParam = TTI_INFO_LARGE;
        else if (eIcon == tCIDCtrls::ETTipIcons::Warning)
            wParam = TTI_WARNING_LARGE;
        else if (eIcon == tCIDCtrls::ETTipIcons::Error)
            wParam = TTI_ERROR_LARGE;

        ::SendMessage
        (
            hwndThis(), TTM_SETTITLE, wParam, tCIDCtrls::TLParam(strTitle.pszBuffer())
        );
    }
}


//
//  Display us with new text and a given point. The point must be in screen coordinates
//  since this guy is a popup.
//
tCIDLib::TVoid
TToolTip::Display(  const   TWindow&    wndOwner
                    , const TPoint&     pntAt
                    , const TString&    strText)
{
    // Set up the tool tip info with text now
    TOOLINFO Info = {0};
    Info.cbSize = sizeof(TOOLINFO);
    Info.hwnd = wndOwner.hwndSafe();
    Info.uFlags = TTF_TRACK;
    Info.lpszText = (LPWSTR)strText.pszBuffer();

    // Update the text to display
    ::SendMessage(hwndSafe(), TTM_UPDATETIPTEXT, 0, tCIDCtrls::TLParam(&Info));

    // Then set the position and show it
    ::SendMessage(hwndThis(), TTM_TRACKPOSITION, 0, MAKELPARAM(pntAt.i4X(), pntAt.i4Y()));
    ::SendMessage(hwndThis(), TTM_TRACKACTIVATE, TRUE, tCIDCtrls::TLParam(&Info));
}


tCIDLib::TVoid TToolTip::Hide()
{
    ::SendMessage(hwndSafe(), TTM_TRACKACTIVATE, FALSE, 0);
}

