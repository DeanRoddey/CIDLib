//
// FILE NAME: CIDCtrls_ScrollBar.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/28/2017
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a wrapper around the standard scroll bar control
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes.
// ---------------------------------------------------------------------------
#include    "CIDCtrls_.hpp"



// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TSBChangeInfo,TCtrlNotify)
AdvRTTIDecls(TScrollBar, TStdCtrlWnd)



// ---------------------------------------------------------------------------
//   CLASS: TSBChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSBChangeInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TSBChangeInfo::TSBChangeInfo(const  tCIDLib::TCard4 c4Position
                            , const TWindow&        wndSrc) :

    TCtrlNotify(wndSrc)
    , m_c4Position(c4Position)
{
}

TSBChangeInfo::TSBChangeInfo(const TSBChangeInfo& wnotSrc) :

    TCtrlNotify(wnotSrc)
    , m_c4Position(wnotSrc.m_c4Position)
{
}

TSBChangeInfo::~TSBChangeInfo()
{
}


// ---------------------------------------------------------------------------
//  TSBChangeInfo: Public operators
// ---------------------------------------------------------------------------
TSBChangeInfo& TSBChangeInfo::operator=(const TSBChangeInfo& wnotSrc)
{
    if (this != &wnotSrc)
    {
        TParent::operator=(wnotSrc);
        m_c4Position = wnotSrc.m_c4Position;
    }
    return *this;
}


// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TCard4 TSBChangeInfo::c4Position() const
{
    return m_c4Position;
}




// ---------------------------------------------------------------------------
//  CLASS: TScrollBar
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TScrollBar: Public static data
// ---------------------------------------------------------------------------
const TNotificationId TScrollBar::nidChange(L"ScrollBarEvent");


// ---------------------------------------------------------------------------
// TScrollBar: Constructors and Destructor
// ---------------------------------------------------------------------------
TScrollBar::TScrollBar() :

    TStdCtrlWnd()
    , m_bHorizontal(kCIDLib::False)
    , m_eScrollStyles(tCIDCtrls::ESBStyles::None)
{
}

TScrollBar::~TScrollBar()
{
}


// ---------------------------------------------------------------------------
// TScrollBar: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TScrollBar::InitFromDesc(const  TWindow&                wndParent
                        , const TDlgItem&               dlgiSrc
                        , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles::VisGroupChild;
    tCIDCtrls::ESBStyles eScrollStyles = tCIDCtrls::ESBStyles::None;

    tCIDLib::TBoolean bHorizontal = kCIDLib::False;
    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_HorzScroll))
        bHorizontal = kCIDLib::True;

    CreateScrollBar
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , bHorizontal
        , eStyles
        , eScrollStyles
    );
}


tCIDLib::TVoid TScrollBar::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_HorzScroll);
}


TSize TScrollBar::szDefault() const
{
    TSize szInit;
    if (m_bHorizontal)
        szInit.Set(96, facCIDCtrls().c4ScrollBarHeight());
    else
        szInit.Set(facCIDCtrls().c4ScrollBarWidth(), 96);

    TGUIFacility::AdjustDlgUnits(szInit);
    return szInit;
}


// ---------------------------------------------------------------------------
// TScrollBar: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Returns the current scroll position
tCIDLib::TCard4 TScrollBar::c4CurPos() const
{
    // We need to get the current info
    SCROLLINFO Info = {0};
    Info.cbSize = sizeof(SCROLLINFO);
    Info.fMask = SIF_POS;

    if (!GetScrollInfo(hwndSafe(), SB_CTL, &Info) || (Info.nPos < 0))
    {
        facCIDCtrls().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcSBar_GetScrollPos
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return tCIDLib::TCard4(Info.nPos);
}


//
//  We just create ourself, which kicks off the initialization of our window.
//
tCIDLib::TVoid
TScrollBar::CreateScrollBar(const   TWindow&                wndParent
                            , const tCIDCtrls::TWndId       widThis
                            , const TArea&                  areaInit
                            , const tCIDLib::TBoolean       bHorizontal
                            , const tCIDCtrls::EWndStyles   eStyles
                            , const tCIDCtrls::ESBStyles    eScrollStyles)
{
    // Store our class specific styles and orientation flag
    m_bHorizontal = bHorizontal;
    m_eScrollStyles = eScrollStyles;

    // We need to add some scroll bar specific Win32 styles
    tCIDLib::TCard4 c4SBStyles = 0;
    if (bHorizontal)
        c4SBStyles |= SBS_HORZ;
    else
        c4SBStyles |= SBS_VERT;

    CreateWnd
    (
        wndParent.hwndThis()
        , L"SCROLLBAR"
        , L""
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles(c4SBStyles)
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );
}


//
//  This is called to set (or reset) our columns. We clear the display window contents,
//  then we set up the header control to match the indicated column set, then we call
//  the display winodw and pass on the column info to him to use moving forward.
//
tCIDLib::TVoid
TScrollBar::Setup(  const   tCIDLib::TCard4 c4VisUnits
                    , const tCIDLib::TCard4 c4AvailUnits
                    , const tCIDLib::TCard4 c4InitPos)
{
    //
    //  If more visible units than avaialble, the max pos is zero, since we can't be scrolled.
    //  Else it is the available minus the visible plus one. Min position is always zero.
    //
    tCIDLib::TCard4 c4MaxPos = 0;
    if (c4AvailUnits > c4VisUnits)
        c4MaxPos = (c4AvailUnits - c4VisUnits) + 1;

    // Set the initial position, forcing it within the available range if needed
    tCIDLib::TCard4 c4ToSet = c4InitPos;
    if (c4InitPos > c4MaxPos)
        c4ToSet = c4MaxPos;

    SCROLLINFO Info = {0};
    Info.cbSize = sizeof(Info);
    Info.fMask = SIF_RANGE | SIF_POS | SIF_DISABLENOSCROLL | SIF_PAGE;
    Info.nMin = 0;
    Info.nMax = c4MaxPos;
    Info.nPos = c4InitPos;
    Info.nPage = c4VisUnits;
    SetScrollInfo(hwndThis(), SB_CTL, &Info, TRUE);
}

tCIDLib::TVoid
TScrollBar::Setup(const tCIDLib::TCard4 c4VisUnits, const tCIDLib::TCard4 c4AvailUnits)
{
    //
    //  If more visible units than avaialble, the max pos is zero, since we can't be scrolled.
    //  Else it is the available minus the visible plus one. Min position is always zero.
    //
    tCIDLib::TCard4 c4MaxPos = 0;
    if (c4AvailUnits > c4VisUnits)
        c4MaxPos = (c4AvailUnits - c4VisUnits) + 1;

    SCROLLINFO Info = {0};
    Info.cbSize = sizeof(Info);
    Info.fMask = SIF_RANGE | SIF_DISABLENOSCROLL | SIF_PAGE;
    Info.nMin = 0;
    Info.nMax = c4MaxPos;
    Info.nPage = c4VisUnits;
    SetScrollInfo(hwndThis(), SB_CTL, &Info, TRUE);
}


// This will manually set the position of a scroll bar
tCIDLib::TVoid TScrollBar::SetPosition(const tCIDLib::TCard4 c4ToSet)
{
    SCROLLINFO Info = {0};
    Info.cbSize = sizeof(Info);
    Info.fMask = SIF_POS | SIF_DISABLENOSCROLL;
    Info.nPos = c4ToSet;
    SetScrollInfo(hwndThis(), SB_CTL, &Info, TRUE);
}


// ---------------------------------------------------------------------------
// TScrollBar: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TScrollBar::bCreated()
{
    // Call our parent first
    TParent::bCreated();

    // Make us visible at the scroll bar level
    ShowScrollBar(hwndThis(), SB_CTL, TRUE);

    return kCIDLib::True;
}



//
//  We need to handle notifications reflected back to us (which would normally go to the
//  parent window but TWindow reflects them to us.)
//
tCIDLib::TBoolean
TScrollBar::bNotReflect(        TWindow&                wndTar
                        , const tCIDCtrls::TWndMsg      wmsgMsg
                        , const tCIDCtrls::TWParam      wParam
                        , const tCIDCtrls::TLParam      lParam
                        ,       tCIDCtrls::TMsgResult&  mresRet)
{
    if ((wmsgMsg == WM_HSCROLL)
    ||  (wmsgMsg == WM_VSCROLL))
    {
        // Get the event and convert to one of our events
        tCIDLib::TCard2 c2Event(wParam & 0xFFFF);

        tCIDCtrls::EScrollEvs eEvent = tCIDCtrls::EScrollEvs::None;
        if (c2Event == SB_LEFT)
            eEvent = tCIDCtrls::EScrollEvs::Home;
        else if (c2Event == SB_RIGHT)
            eEvent = tCIDCtrls::EScrollEvs::End;
        else if (c2Event == SB_LINELEFT)
            eEvent = tCIDCtrls::EScrollEvs::PrevLn;
        else if (c2Event == SB_LINERIGHT)
            eEvent = tCIDCtrls::EScrollEvs::NextLn;
        else if (c2Event == SB_PAGELEFT)
            eEvent = tCIDCtrls::EScrollEvs::PrevPg;
        else if (c2Event == SB_PAGERIGHT)
            eEvent = tCIDCtrls::EScrollEvs::NextPg;

        // If not one we care about, then just return now
        if (eEvent == tCIDCtrls::EScrollEvs::None)
            return kCIDLib::True;

        // We need to get the current info
        SCROLLINFO Info = {0};
        Info.cbSize = sizeof(SCROLLINFO);
        Info.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;

        if (!GetScrollInfo(hwndSafe(), SB_CTL, &Info))
        {
            facCIDCtrls().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcSBar_GetScrollPos
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }

        // Decide the new position based on the event
        tCIDLib::TInt4 i4NewPos = Info.nPos;

        switch(eEvent)
        {
            case tCIDCtrls::EScrollEvs::End :
                i4NewPos = Info.nMax - Info.nPage + 1;
                break;

            case tCIDCtrls::EScrollEvs::Home :
                i4NewPos = 0;
                break;

            case tCIDCtrls::EScrollEvs::NextLn :
                i4NewPos++;
                break;

            case tCIDCtrls::EScrollEvs::NextPg :
                 i4NewPos += Info.nPage;
                break;

            case tCIDCtrls::EScrollEvs::PrevLn :
                i4NewPos--;
                break;

            case tCIDCtrls::EScrollEvs::PrevPg :
                i4NewPos -= Info.nPage;
                break;

            default :
                return kCIDLib::True;
        };

        // Clip the value back to valid range
        if (i4NewPos < 0)
            i4NewPos = 0;
        else if (i4NewPos > (Info.nMax - int(Info.nPage) + 1))
            i4NewPos = (Info.nMax - Info.nPage + 1);

        // Update the scroll bar if the position changed and send a notification
        if (i4NewPos != Info.nPos)
        {
            Info.fMask = SIF_POS | SIF_DISABLENOSCROLL;
            Info.nPos = i4NewPos;
            SetScrollInfo(hwndThis(), SB_CTL, &Info, TRUE);

            TSBChangeInfo wnotSend(tCIDLib::TCard4(i4NewPos), *this);
            SendSyncNotify(wnotSend, TScrollBar::nidChange);
        }
    };

    return kCIDLib::False;
}
