//
// FILE NAME: CIDCtrls_CtrlWnd.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/14/2015
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
//  This file implements the base class for all standard control type classes.
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
RTTIDecls(TCtrlWnd, TWindow)
RTTIDecls(TStdCtrlWnd, TCtrlWnd)



// ---------------------------------------------------------------------------
//  CLASS: TCtrlWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCtrlWnd: Destructure, ctors are hidden
// ---------------------------------------------------------------------------
TCtrlWnd::~TCtrlWnd()
{
}


// ---------------------------------------------------------------------------
//  TCtrlWnd: Public, virtual methods
// ---------------------------------------------------------------------------

//
//  To support the dialog editor, this method is implemented by any controls that
//  need to have some hints, beyond the parent window's theme, to tell them how
//  to set themselves up.
//
//  They should all call us first, so we can add some standard ones, though we don't
//  currently have any.
//
tCIDLib::TVoid TCtrlWnd::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
}


//
//  To support the dialog editor as well. Let's controls provide a default initial
//  size that is appropriate. If just a smallish squarish size is fine, then they
//  don't have to override this.
//
//  These are in window units!
//
TSize TCtrlWnd::szDefault() const
{
    return TSize(80, 80);
}



// ---------------------------------------------------------------------------
//  TCtrlWnd: Hidden constructors
// ---------------------------------------------------------------------------
TCtrlWnd::TCtrlWnd() :

    TWindow(kCIDLib::False)
{
}




// ---------------------------------------------------------------------------
//  CLASS: TStdCtrlWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStdCtrlWnd: Destructure, ctors are hidden
// ---------------------------------------------------------------------------
TStdCtrlWnd::~TStdCtrlWnd()
{
}


// ---------------------------------------------------------------------------
//  TStdCtrlWnd: Hidden constructors
// ---------------------------------------------------------------------------
TStdCtrlWnd::TStdCtrlWnd() :

    TCtrlWnd()
{
}


// ---------------------------------------------------------------------------
//  TStdCtrlWnd: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TStdCtrlWnd::bCreated()
{
    if (!TParent::bCreated())
        return kCIDLib::False;

    // Wrapped standard controls always use the normal font
    ::SendMessage
    (
        hwndThis()
        , WM_SETFONT
        , tCIDCtrls::TWParam(TGUIFacility::gfontDefNorm().hfontThis())
        , 0
    );
    return kCIDLib::True;
}


//
//  TWindow's default indicates it should just propogate to the parent. But for
//  overridden standard controls, we want to indicate it should pass on to the
//  underlying control. If he choose to let it propogate that's his business. Else
//  he can display his own menu.
//
tCIDCtrls::ECtxMenuOpts
TStdCtrlWnd::eShowContextMenu(const TPoint&, const tCIDCtrls::TWndId)
{
    return tCIDCtrls::ECtxMenuOpts::Pass;
}


//
//  For all the standard controls the text is stored in the window text, so we
//  have to get it from there and return it.
//
tCIDLib::TVoid TStdCtrlWnd::WndTextPreQuery(TString& strToFill) const
{
    tCIDCtrls::TMsgResult mresRet;
    mresRet = ::SendMessage(hwndSafe(), WM_GETTEXTLENGTH, 0, 0);
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
}


//
//  For standard controls, the text has to be stored in the window text.
//
tCIDLib::TVoid TStdCtrlWnd::WndTextStore(const TString& strToStore)
{
    ::SendMessage
    (
        hwndSafe(), WM_SETTEXT, 0, tCIDCtrls::TLParam(strToStore.pszBuffer())
    );
}


