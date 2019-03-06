//
// FILE NAME: CIDMacroDbg_BuildResTab.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/11/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the tab that shows the parser's output.
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
#include    "CIDMacroDbg_.hpp"



// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TPrsErrPosInfo,TCtrlNotify)
RTTIDecls(TBuildResTab, TTabWindow)



// ---------------------------------------------------------------------------
//   CLASS: TPrsErrPosInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPrsErrPosInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TPrsErrPosInfo::TPrsErrPosInfo( const   TWindow&            wndSrc
                                , const TString&            strClassPath
                                , const tCIDLib::TCard4     c4Line
                                , const tCIDLib::TCard4     c4Col) :

    TCtrlNotify(wndSrc)
    , m_c4Col(c4Col)
    , m_c4Line(c4Line)
    , m_strClassPath(strClassPath)
{
}

TPrsErrPosInfo::TPrsErrPosInfo(const TPrsErrPosInfo& wnotSrc) :

    TCtrlNotify(wnotSrc)
    , m_c4Col(wnotSrc.m_c4Col)
    , m_c4Line(wnotSrc.m_c4Line)
    , m_strClassPath(wnotSrc.m_strClassPath)
{
}

TPrsErrPosInfo::~TPrsErrPosInfo()
{
}


// ---------------------------------------------------------------------------
//  TPrsErrPosInfo: Public operators
// ---------------------------------------------------------------------------
TPrsErrPosInfo& TPrsErrPosInfo::operator=(const TPrsErrPosInfo& wnotSrc)
{
    if (this != &wnotSrc)
    {
        TParent::operator=(wnotSrc);
        m_c4Col         = wnotSrc.m_c4Col;
        m_c4Line        = wnotSrc.m_c4Line;
        m_strClassPath  = wnotSrc.m_strClassPath;

    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TPrsErrPosInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TPrsErrPosInfo::c4Col() const
{
    return m_c4Col;
}


tCIDLib::TCard4 TPrsErrPosInfo::c4Line() const
{
    return m_c4Line;
}


const TString& TPrsErrPosInfo::strClassPath() const
{
    return m_strClassPath;
}




// ---------------------------------------------------------------------------
//   CLASS: TBuildResTab::TErrInfo
//  PREFIX: erri
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBuildResTab::TErrInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TBuildResTab::TErrInfo::TErrInfo() :

    m_c4ColNum(0)
    , m_c4LineNum(0)
{
}

TBuildResTab::TErrInfo::TErrInfo(const TErrInfo& erriSrc) :

    m_c4ColNum(erriSrc.m_c4ColNum)
    , m_c4LineNum(erriSrc.m_c4LineNum)
    , m_eEvent(erriSrc.m_eEvent)
    , m_strClassPath(erriSrc.m_strClassPath)
    , m_strText(erriSrc.m_strText)
{
}

TBuildResTab::TErrInfo::~TErrInfo()
{
}




// ---------------------------------------------------------------------------
//   CLASS: TBuildResTab
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMacroDbgPrsOutWnd: Public, static data
// ---------------------------------------------------------------------------
const TNotificationId   TBuildResTab::nidErrInfo(L"MacroDbgPrsErrInfo");


// ---------------------------------------------------------------------------
//  TBuildResTab: Constructors and Destructor
// ---------------------------------------------------------------------------
TBuildResTab::TBuildResTab() :

    TTabWindow(kCIDMacroDbg_::pszTab_BuildResults, L"", kCIDLib::False, kCIDLib::False)
    , m_pwndList(nullptr)
{
}

TBuildResTab::~TBuildResTab()
{
}


// ---------------------------------------------------------------------------
//  TBuildResTab: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  These methods are called to have us add an item to the list. We add it and
//  make sure that we keep the list pushed to the bottom.
//
tCIDLib::TVoid
TBuildResTab::ParseEvent(const  tCIDMacroEng::EPrsEvs   eEvent
                        , const TString&                strText
                        , const tCIDLib::TCard4         c4Line
                        , const tCIDLib::TCard4         c4Col
                        , const TString&                strClassPath)
{
    // Add a new item to the list, then fill it in
    TErrInfo& erriNew = m_colErrs.objAdd(TErrInfo());
    erriNew.m_c4LineNum = c4Line;
    erriNew.m_c4ColNum = c4Col;
    erriNew.m_eEvent = eEvent;
    erriNew.m_strClassPath = strClassPath;
    erriNew.m_strText = strText;

    // Add an item. The text doesn't matter since we are custom draw
    m_pwndList->c4AddItem(TString::strEmpty());
}


tCIDLib::TVoid
TBuildResTab::ParseException(const  TError&         errCaught
                            , const tCIDLib::TCard4 c4Line
                            , const tCIDLib::TCard4 c4Col
                            , const TString&        strClassPath)
{
    // Add a new item to the list, then fill it in
    TErrInfo& erriNew = m_colErrs.objAdd(TErrInfo());
    erriNew.m_c4LineNum = c4Line;
    erriNew.m_c4ColNum = c4Col;
    erriNew.m_eEvent = tCIDMacroEng::EPrsEvs::CIDLibExcept;
    erriNew.m_strClassPath = strClassPath;
    erriNew.m_strText = errCaught.strErrText();

    // Add an item. The text doesn't matter since we are custom draw
    m_pwndList->c4AddItem(TString::strEmpty());
}

tCIDLib::TVoid
TBuildResTab::ParseException(const  tCIDLib::TCard4 c4Line
                            , const tCIDLib::TCard4 c4Col
                            , const TString&        strClassPath)
{
    // Add a new item to the list, then fill it in
    TErrInfo& erriNew = m_colErrs.objAdd(TErrInfo());
    erriNew.m_c4LineNum = c4Line;
    erriNew.m_c4ColNum = c4Col;
    erriNew.m_eEvent = tCIDMacroEng::EPrsEvs::UnknownExcept;
    erriNew.m_strClassPath = strClassPath;

    // Add an item. The text doesn't matter since we are custom draw
    m_pwndList->c4AddItem(TString::strEmpty());
}


// ---------------------------------------------------------------------------
//  TBuildResTab: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TBuildResTab::CreateBldResTab(TTabbedWnd& wndParent)
{
    wndParent.c4CreateTab(this, facCIDMacroDbg().strMsg(kMDbgMsgs::midTab_BuildResults));
}


// Clear out list of any output and clear our error info list
tCIDLib::TVoid TBuildResTab::Reset()
{
    m_colErrs.RemoveAll();
    m_pwndList->RemoveAll();
}



// ---------------------------------------------------------------------------
//  TBuildResTab: Protected, inherited methods
// ---------------------------------------------------------------------------

// Keep our list window sized to us
tCIDLib::TVoid
TBuildResTab::AreaChanged(  const   TArea&                  areaPrev
                            , const TArea&                  areaNew
                            , const tCIDCtrls::EPosStates   ePosState
                            , const tCIDLib::TBoolean       bOrgChanged
                            , const tCIDLib::TBoolean       bSizeChanged
                            , const tCIDLib::TBoolean       bStateChanged)
{
    // Call our parent first
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    if ((ePosState != tCIDCtrls::EPosStates::Minimized) && bSizeChanged)
    {
        if (m_pwndList && m_pwndList->bIsValid())
            m_pwndList->SetSize(areaClient().szArea(), kCIDLib::False);
    }
}


// Create our list box
tCIDLib::TBoolean TBuildResTab::bCreated()
{
    TParent::bCreated();

    TArea areaInit;
    QueryClientArea(areaInit, kCIDLib::True);
    m_pwndList = new TListBox();
    m_pwndList->CreateListBox
    (
        *this
        , kCIDCtrls::widFirstCtrl
        , areaInit
        , tCIDCtrls::EWndStyles::VisChild
        , tCIDLib::eOREnumBits
          (
            tCIDCtrls::ELBStyles::AutoAddBottom
            , tCIDCtrls::ELBStyles::CustDraw
          )
        , tCIDCtrls::EExWndStyles::None
    );

    // Set our background color to the standard window color
    SetBgnColor
    (
        facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::Window), kCIDLib::True
    );

    // Register for event notification from the list box
    m_pwndList->pnothRegisterHandler(this, &TBuildResTab::eLBHandler);

    return kCIDLib::True;
}


//
//  We custom draw the list box so that we can have two lines per entry. It's not
//  sorted so the id is a direct index into the list.
//
tCIDLib::TVoid
TBuildResTab::CustomDraw(       TGraphDrawDev&          gdevTar
                        , const tCIDLib::TCard4         c4Id
                        , const tCIDCtrls::ECustDrFlags eFlags
                        , const TArea&                  areaTar)
{
    // It might be max card, in which case nothing to do
    if (c4Id == kCIDLib::c4MaxCard)
        return;

    const TErrInfo& erriCur = m_colErrs[c4Id];

    // Set up the colors for selected vs. unselected
    TRGBClr rgbBgn;
    TRGBClr rgbFgn;
    if (tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::ECustDrFlags::Selected))
    {
        // We use a different color based on focus or not
        if (tCIDLib::bAllBitsOn(eFlags, tCIDCtrls::ECustDrFlags::Focus))
        {
            rgbFgn = facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::HighlightText);
            rgbBgn = facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::HighlightBgn);
        }
         else
        {
            rgbBgn = facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::HighlightBgnNF);
            rgbFgn = facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::HighlightTextNF);
        }
    }
     else
    {
        rgbBgn = rgbBgnFill();
        rgbFgn = facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::WindowText);
    }

    gdevTar.Fill(areaTar, rgbBgn);
    gdevTar.SetTextColor(rgbFgn);
    gdevTar.eBackMixMode(tCIDGraphDev::EBackMixModes::Transparent);

    // Take the drawing area and move it inwards
    TArea areaDraw(areaTar);
    areaDraw.Deflate(8, 4);

    //
    //  Based on the error event type, we draw a thin color rectangle on the left
    //  edge of the entry.
    //
    TArea areaColor(areaDraw);
    areaColor.c4Width(4);

    TRGBClr rgbEvent;
    switch(erriCur.m_eEvent)
    {
        case tCIDMacroEng::EPrsEvs::InfoMsg :
            rgbEvent = facCIDGraphDev().rgbGreen;
            break;

        case tCIDMacroEng::EPrsEvs::WarningMsg :
            rgbEvent = facCIDGraphDev().rgbPaleGrey;
            break;

        case tCIDMacroEng::EPrsEvs::ErrorMsg :
        case tCIDMacroEng::EPrsEvs::CIDLibExcept :
        case tCIDMacroEng::EPrsEvs::UnknownExcept :
            rgbEvent = facCIDGraphDev().rgbRed;
            break;

        default :
            break;
    };
    gdevTar.Fill(areaColor, rgbEvent);

    // Move the drawing area over past that with some margin
    areaDraw.AdjustAndShrink(8, 0);

    // Format everything but the error text into the top row
    m_strFmt.Clear();
    m_strFmt = L"[Line=";
    m_strFmt.AppendFormatted(erriCur.m_c4LineNum);
    m_strFmt.Append(L", Col=");
    m_strFmt.AppendFormatted(erriCur.m_c4ColNum);
    m_strFmt.Append(L"] - ");
    m_strFmt.Append(erriCur.m_strClassPath);

    // First line goes into the top half
    areaDraw.ScaleSize(1, 0.5);
    gdevTar.DrawString(m_strFmt, areaDraw);

    // And the error text in the bottom
    areaDraw.Move(tCIDLib::EDirs::Down);
    gdevTar.DrawString(erriCur.m_strText, areaDraw);
}


tCIDLib::TVoid
TBuildResTab::MeasureItem(  const   tCIDCtrls::TWndId   widCtrl
                            , const tCIDLib::TCard4     c4ItemId
                            ,       tCIDLib::TCard4&    c4Height)
{
    //
    //  Make it twice the default font height, plus one external leading for the
    //  the spacing between them, plus 4 pixels on top and bottom.
    //
    TGraphWndDev gdevTmp(*this);
    TFontJanitor janFont(&gdevTmp, &TGUIFacility::gfontDefNorm());
    TFontMetrics fmtrDef(gdevTmp);
    c4Height = (fmtrDef.c4Height() * 2) + fmtrDef.c4ExternalLeading() + 8;
}




// ---------------------------------------------------------------------------
//  TMacroDbgPrsOutWnd: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TBuildResTab::eLBHandler(TListChangeInfo& wnotEvent)
{
    //
    //  If it's an invocation event, then we want to send a notification
    //  to any registered handlers.
    //
    if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::Invoked)
    {
        CIDAssert
        (
            wnotEvent.c4Index() < m_colErrs.c4ElemCount()
            , L"The received list box index was beyond the count of errors"
        );

        const TErrInfo& erriCur = m_colErrs[wnotEvent.c4Index()];

        //
        //  NOTE:   We only do this for events that have non-zero line,col
        //          information. Some info and warning messages aren't related
        //          to a particular location.
        //
        if (erriCur.m_c4LineNum && erriCur.m_c4ColNum)
        {
            SendAsyncNotify
            (
                new TPrsErrPosInfo
                (
                  *this
                  , erriCur.m_strClassPath
                  , erriCur.m_c4LineNum
                  , erriCur.m_c4ColNum
                )
                , TBuildResTab::nidErrInfo
            );
        }
    }
    return tCIDCtrls::EEvResponses::Handled;
}


