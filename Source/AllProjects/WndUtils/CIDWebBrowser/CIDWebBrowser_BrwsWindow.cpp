//
// FILE NAME: CIDWebBrowser_BrwsWindow.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/10/2005
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the browser window class.
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
#include    <Windows.h>
#include    "CIDWebBrowser_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"
#include    "CIDWebBrowser_EmbedBrowser.h"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TWebBrowserWnd,TWindow)


// ---------------------------------------------------------------------------
//  CLASS: TWebBrowserWnd
// PREFIX: brwsw
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Constructors and Destructor
// ---------------------------------------------------------------------------
TWebBrowserWnd::TWebBrowserWnd() :

    TWindow(kCIDLib::True)
    , m_bFlag(kCIDLib::False)
    , m_enctCleanup(0)
    , m_pEmbedInfo(nullptr)
{
}

TWebBrowserWnd::~TWebBrowserWnd()
{
}


// ---------------------------------------------------------------------------
//  TWebBrowserWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TWebBrowserWnd::bFlag() const
{
    return m_bFlag;
}

tCIDLib::TBoolean TWebBrowserWnd::bFlag(const tCIDLib::TBoolean bToSet)
{
    m_bFlag = bToSet;
    return m_bFlag;
}


// Query one of a set of boolean options we support
tCIDLib::TBoolean
TWebBrowserWnd::bQueryOpt(const tWebBrowser::EBoolOpts eOpt) const
{
    if (!m_pEmbedInfo)
        return kCIDLib::False;

    tCIDLib::TBoolean bRet = kCIDLib::False;
    switch(eOpt)
    {
        case tWebBrowser::EBoolOpts::ShowAddrBar :
            bRet = (GetShowAddrBar(m_pEmbedInfo) != 0);
            break;

        case tWebBrowser::EBoolOpts::ShowToolBar :
            bRet = (GetShowToolBar(m_pEmbedInfo) != 0);
            break;

        case tWebBrowser::EBoolOpts::SilentMode :
            bRet = (GetSilentMode(m_pEmbedInfo) != 0);
            break;

        default :
        {
            facCIDWebBrowser().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kWBrsErrs::errcOpt_UnknownOp
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , TInteger(tCIDLib::TInt4(eOpt))
            );
            break;
        }
    };
    return bRet;
}


tCIDLib::TBoolean
TWebBrowserWnd::bReadyForCleanup(const tCIDLib::TEncodedTime enctTest) const
{
    return (m_enctCleanup <= enctTest);
}


tCIDLib::TVoid
TWebBrowserWnd::CreateBrwsWindow(const  TWindow&                wndParent
                                , const tCIDCtrls::EWndStyles   eWndStyles
                                , const TArea&                  areaInit
                                , const tCIDCtrls::TWndId       widToUse)
{
    //
    //  Call the basic window creation method to create us. Make sure that we clip and don't
    //  interfere with the browser window.
    //
    TParent::CreateWnd
    (
        wndParent.hwndThis()
        , kCIDCtrls::pszCustClass
        , L""
        , areaInit
        , tCIDLib::eOREnumBits(eWndStyles, tCIDCtrls::EWndStyles::ClipChildren)
        , tCIDCtrls::EExWndStyles::ControlParent
        , widToUse
    );
}


tCIDLib::TVoid
TWebBrowserWnd::DoNavOp(const tWebBrowser::ENavOps eOp)
{
    if (!m_pEmbedInfo)
        return;

    DoPageAction(m_pEmbedInfo, tCIDLib::c4EnumOrd(eOp));
}


tCIDLib::TVoid TWebBrowserWnd::SetCleanupStamp(const tCIDLib::TCard4 c4Secs)
{
    m_enctCleanup = TTime::enctNowPlusSecs(c4Secs);
}


// Set one of a set of boolean options we support
tCIDLib::TVoid
TWebBrowserWnd::SetOpt( const   tWebBrowser::EBoolOpts  eOpt
                        , const tCIDLib::TBoolean       bToSet)
{
    if (!m_pEmbedInfo)
        return;

    switch(eOpt)
    {
        case tWebBrowser::EBoolOpts::ShowAddrBar :
            SetShowAddrBar(m_pEmbedInfo, bToSet);
            break;

        case tWebBrowser::EBoolOpts::ShowToolBar :
            SetShowToolBar(m_pEmbedInfo, bToSet);
            break;

        case tWebBrowser::EBoolOpts::SilentMode :
            SetSilentMode(m_pEmbedInfo, bToSet);
            break;

        default :
        {
            facCIDWebBrowser().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kWBrsErrs::errcOpt_UnknownOp
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , TInteger(tCIDLib::TInt4(eOpt))
            );
            break;
        }
    };
}


// Shows a directly provided string of HTML content
tCIDLib::TVoid TWebBrowserWnd::ShowHTML(const TString& strToShow)
{
    // DisplayHTMLStr(m_pEmbedInfo, strToShow.pszBuffer());
}


// Loads the indicated URL
tCIDLib::TVoid TWebBrowserWnd::SetURL(const TString& strToSet)
{
    const tCIDLib::TInt4 i4Res = DisplayHTMLPage(m_pEmbedInfo, strToSet.pszBuffer());
}


// This must be called before closing the window
tCIDLib::TVoid TWebBrowserWnd::Terminate()
{
    if (m_pEmbedInfo)
        UnEmbedBrowserObject(&m_pEmbedInfo, hwndThis());
}


// ---------------------------------------------------------------------------
//  TWebBrowserWnd: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TWebBrowserWnd::AreaChanged(const   TArea&                  areaPrev
                            , const TArea&                  areaNew
                            , const tCIDCtrls::EPosStates   ePosState
                            , const tCIDLib::TBoolean       bOrgChanged
                            , const tCIDLib::TBoolean       bSizeChanged
                            , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    if (ePosState != tCIDCtrls::EPosStates::Minimized)
    {
        if (bSizeChanged)
            ResizeBrowser(m_pEmbedInfo, areaNew.c4Width(), areaNew.c4Height());
//        else if (bOrgChanged)
//            ReposBrowser(m_pEmbedInfo, areaNew.i4X(), areaNew.i4Y());
    }
}


tCIDLib::TBoolean TWebBrowserWnd::bCreated()
{
    TParent::bCreated();

    // Do the embed
    EmbedBrowserObject(&m_pEmbedInfo, hwndThis());

    return kCIDLib::True;
}

// We are fully covered by the browser window, so just suppress this
tCIDLib::TBoolean TWebBrowserWnd::bEraseBgn(TGraphDrawDev&)
{
    return kCIDLib::True;
}


// We are fully covered by the browser window, so just suppress this
tCIDLib::TBoolean
TWebBrowserWnd::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    return kCIDLib::True;
}


//
//  Make sure the embedded window gets cleaned up though it definitely should be done
//  explicitly by the application for safety.
//
tCIDLib::TVoid TWebBrowserWnd::Destroyed()
{
    // If somehow it didn't get terminated explicitly, then do it now
    if (m_pEmbedInfo)
        UnEmbedBrowserObject(&m_pEmbedInfo, hwndThis());

    TParent::Destroyed();
}


