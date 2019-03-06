//
// FILE NAME: CIDCtrls_TouchWnd.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/19/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the touch window class.
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
RTTIDecls(TTouchWnd, TCtrlWnd)



// ---------------------------------------------------------------------------
//  CLASS: TTouchWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTouchWnd: Destructor
// ---------------------------------------------------------------------------
TTouchWnd::~TTouchWnd()
{
}


// ---------------------------------------------------------------------------
//  TTouchWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TTouchWnd::CreateTouchWnd(  const   TWindow&                wndParent
                            , const tCIDCtrls::TWndId       widThis
                            , const TArea&                  areaInit
                            , const tCIDCtrls::EWndStyles   eStyles)
{
    CreateWnd
    (
        wndParent.hwndSafe()
        , kCIDCtrls::pszCustClass
        , kCIDLib::pszEmptyZStr
        , areaInit
        , eStyles
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );
}


// ---------------------------------------------------------------------------
//  TTouchWnd: Hidden constructors
// ---------------------------------------------------------------------------

TTouchWnd::TTouchWnd() :

    TWindow(kCIDLib::False)
    , m_bFlag(kCIDLib::False)
    , m_pgesthCur(nullptr)
{
}


// ---------------------------------------------------------------------------
//  TTouchWnd: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  We get the appropriate type of gesture handler from the gesture handler class, and
//  store it away for subsequent use.
//
tCIDLib::TBoolean TTouchWnd::bCreated()
{
    TParent::bCreated();

    //
    //  Create the correct gesture handler. We pass ourself as the target for the gesture
    //  callbacks, and we pass our window handle as the target window, which it will use
    //  to some necessary window based operations for us.
    //
    m_pgesthCur = TCIDWndGestHandler::pgesthMakeNew(this, hwndThis());
    return kCIDLib::True;
}


//
//  We need to intercept gesture and mouse messages and pass them to the gesture handler.
//  We return whether he ate them or not.
//
tCIDLib::TBoolean
TTouchWnd::bEatSubClassMsg( const   tCIDCtrls::TWndMsg      wmsgMsg
                            , const tCIDCtrls::TWParam      wParam
                            , const tCIDCtrls::TLParam      lParam
                            ,       tCIDCtrls::TMsgResult&  mresRet)
{
    tCIDLib::TBoolean bRet = kCIDLib::False;

    if ((wmsgMsg == WM_GESTURE)
    ||  (wmsgMsg == WM_LBUTTONDOWN)
    ||  (wmsgMsg == WM_MOUSEMOVE)
    ||  (wmsgMsg == WM_LBUTTONUP))
    {
        if (m_pgesthCur)
            bRet = m_pgesthCur->bHandleMsg(wmsgMsg, wParam, lParam, mresRet, m_bFlag);
    }
    return bRet;
}


// If we got our gesture handler, we need to clean it up
tCIDLib::TVoid TTouchWnd::Destroyed()
{

    if (m_pgesthCur)
    {
        try
        {
            m_pgesthCur->Terminate();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
        delete m_pgesthCur;
        m_pgesthCur = nullptr;
    }

    TParent::Destroyed();
}

