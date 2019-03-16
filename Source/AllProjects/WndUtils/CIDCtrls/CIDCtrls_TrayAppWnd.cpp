//
// FILE NAME: CIDWnd_TrayAppWnd.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/03/2012
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
//  This file implements the base tray app support class.
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
#include    <shellapi.h>



// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TTrayAppWnd,TWindow)



// ---------------------------------------------------------------------------
//   CLASS: TTrayAppWnd
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTrayAppWnd: Constructors and Destructor
// ---------------------------------------------------------------------------
TTrayAppWnd::TTrayAppWnd() :

    TWindow(kCIDLib::False)
    , m_hicoTray(kCIDGraphDev::hicoInvalid)
{
}

TTrayAppWnd::~TTrayAppWnd()
{
}


// ---------------------------------------------------------------------------
//  TTrayAppWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We create ourself initially invisible since we generally want to stay
//  hidden until the user clicks on the icon in the tray.
//
tCIDLib::TVoid
TTrayAppWnd::CreateTrayApp( const   TArea&      areaInit
                            , const TIcon&      icoTray
                            , const TString&    strText)
{
    // Make a copy of the icon handle for our own use
    m_hicoTray = ::CopyIcon(icoTray.hicoThis());

    TParent::CreateWnd
    (
        HWND_DESKTOP
        , kCIDCtrls::pszCustClass
        , kCIDLib::pszEmptyZStr
        , areaInit
        , tCIDCtrls::EWndStyles::None
        , tCIDCtrls::EExWndStyles::None
        , 0
    );

    // Do our initial registration
    _NOTIFYICONDATAW Data = {0};
    DoInfoSetup(Data);

    //
    //  This can sometimes fail when a tray app is set up to auto-start
    //  and the system is strugging to get going. So, if it fails, back off
    //  a while and try again. Do this a number of times before giving up.
    //
    tCIDLib::TCard4 c4Rnds = 0;
    while (c4Rnds < 5)
    {
        if (::Shell_NotifyIcon(NIM_ADD, &Data))
            break;
        facCIDCtrls().MsgYield(5000);
        c4Rnds++;
    }

    if (c4Rnds > 5)
    {
        facCIDCtrls().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcTray_CantAdd
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


//
//  This will display a balloon popup above the tray area. The title and
//  text are pretty limited (48 chars for title, 200 for text), so keep them
//  short or they will be truncated.
//
tCIDLib::TVoid
TTrayAppWnd::ShowBalloonText(const  TString&            strTitle
                            , const TString&            strText
                            , const tCIDLib::TBoolean   bError
                            , const tCIDLib::TCard4     c4TimeoutMSs)
{
    _NOTIFYICONDATAW Data = {0};
    DoInfoSetup(Data);

    // Copy out up to 199 characters of text and up to 47 for the title
    Data.uFlags |= NIF_INFO;
    Data.dwInfoFlags = bError ? NIIF_ERROR : NIIF_INFO;
    strText.ToZStr(Data.szInfo, 199);
    strTitle.ToZStr(Data.szInfoTitle, 47);

    ::Shell_NotifyIcon(NIM_MODIFY, &Data);
}



//
//  Set up the tip text we should display. It can be empty to get rid of any
//  tip display.
//
tCIDLib::TVoid TTrayAppWnd::SetTipText(const TString& strToSet)
{
    m_strTipText = strToSet;

    // Update the tray with this new info
    _NOTIFYICONDATAW Data = {0};
    DoInfoSetup(Data);

    ::Shell_NotifyIcon(NIM_MODIFY, &Data);
}



// ---------------------------------------------------------------------------
//  TTrayAppWnd: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TTrayAppWnd::bCreated()
{
    TParent::bCreated();

    //
    //  Do the virtual init call so he can get ready before we add ourself to
    //  the tray.
    //
    try
    {
        InitTrayApp();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);

        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TTrayAppWnd::bPaint(TGraphDrawDev&, const TArea&)
{
    //
    //  Don't do anything. We are just here to receive notifications. Any
    //  display is via the popup menu or bubble popup. If the derived class
    //  wants to have any display, it can override.
    //
    return kCIDLib::True;
}


tCIDLib::TVoid TTrayAppWnd::Destroyed()
{
    // Call the derived class first
    try
    {
        CleanupTrayApp();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    // And now remove us from the tray
    _NOTIFYICONDATAW Data = {0};
    DoInfoSetup(Data);
    ::Shell_NotifyIcon(NIM_DELETE, &Data);
}


//
//  We have to register a user level message with the tray which he will
//  send back to us. So register one of the CIDWnd defined user messages,
//  which will come to us as this.
//
tCIDLib::TVoid
TTrayAppWnd::UserMsg(const  tCIDCtrls::TWParam  wParam
                    , const tCIDCtrls::TLParam  lParam)
{
    if (lParam == WM_LBUTTONUP)
    {
        // Tell the derived window that it's icon has been clicked
        try
        {
            Invoked();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
     else if (lParam == WM_RBUTTONUP)
    {
        // Get the current mouse point
        TPoint pntAt = facCIDCtrls().pntPointerPos(*this);

        //
        //  And invoke the derived method. If they return false, that means
        //  a selection was not made, so we want focus to go back to the
        //  tray app icon.
        //
        try
        {
            if (!bShowTrayMenu(pntAt))
            {
                _NOTIFYICONDATAW Data = {0};
                DoInfoSetup(Data);
                ::Shell_NotifyIcon(NIM_SETFOCUS, &Data);
            }
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  TTrayAppWnd: Protected, inherited methods
// ---------------------------------------------------------------------------

// Do the standard bits of the icon data structure setup
tCIDLib::TVoid TTrayAppWnd::DoInfoSetup(_NOTIFYICONDATAW& Data)
{
    // Update the tray with this new info
    Data.cbSize = NOTIFYICONDATA_V2_SIZE;
    Data.hWnd = hwndThis();
    Data.uID = 1000;
    Data.uFlags = NIF_MESSAGE;
    Data.uCallbackMessage = kCIDCtrls::wmsgFirstUser;

    // Up to 127 chars plus null
    if (!m_strTipText.bIsEmpty())
    {
        Data.uFlags |= NIF_TIP;
        m_strTipText.ToZStr(Data.szTip, 127);
    }

    // And set the icon if we have one
    if (m_hicoTray != kCIDGraphDev::hicoInvalid)
    {
        Data.uFlags |= NIF_ICON;
        Data.hIcon = ::CopyIcon(m_hicoTray);
    }
}


