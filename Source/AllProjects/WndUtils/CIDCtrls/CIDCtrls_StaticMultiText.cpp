//
// FILE NAME: CIDCtrls_StaticMultiText.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/07/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the single line static text control.
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
AdvRTTIDecls(TStaticMultiText, TStdCtrlWnd)



// ---------------------------------------------------------------------------
//  CLASS: TStaticMultiText
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TStaticMultiText: Constructors and Destructor
// ---------------------------------------------------------------------------
TStaticMultiText::TStaticMultiText() :

    TStdCtrlWnd()
    , m_eMLSStyles(tCIDCtrls::EMLSStyles::None)
    , m_prgbText(nullptr)
{
}

TStaticMultiText::~TStaticMultiText()
{
    // If a text color override was set, free it
    delete m_prgbText;
}


// ---------------------------------------------------------------------------
//  TStaticMultiText: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TStaticMultiText::InitFromDesc( const   TWindow&                wndParent
                                , const TDlgItem&               dlgiSrc
                                , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EMLSStyles eMLSStyles = tCIDCtrls::EMLSStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisChild
    );
    tCIDLib::EHJustify eHorz = tCIDLib::EHJustify::Left;
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None;

    if (eTheme == tCIDCtrls::EWndThemes::DialogBox)
    {
        //
        //  Set a sunken border and ctrl fill background unless told not to or this
        //  is a 'fine print' type.
        //
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder)
        &&  !dlgiSrc.bHasHint(kCIDCtrls::strHint_FinePrint))
        {
            eExStyles |= tCIDCtrls::EExWndStyles::StaticEdge;
            SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::DlgCtrlFill), kCIDLib::True);
        }
    }
     else if (eTheme == tCIDCtrls::EWndThemes::MainWnd)
    {
        //
        //  Set a sunken border and ctrl fill background unless told not to or this
        //  is a 'fine print' type.
        //
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder)
        &&  !dlgiSrc.bHasHint(kCIDCtrls::strHint_FinePrint))
        {
            eStyles |= tCIDCtrls::EWndStyles::Border;
        }
    }

    // An instruction multi-text, which are always centered
    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Instruct))
        eHorz = tCIDLib::EHJustify::Center;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_SourceCode))
        eMLSStyles |= tCIDCtrls::EMLSStyles::NoWrap;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_ReportClicks))
        eMLSStyles |= tCIDCtrls::EMLSStyles::ReportClicks;

    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , TString::strEmpty()
        , eStyles
        , eMLSStyles
        , eHorz
        , eExStyles
    );

    // If fine print, override the default font
    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_FinePrint))
    {
        tCIDGraphDev::TFontHandle hfontUse = TGUIFacility::gfontDefSmall().hfontThis();
        if (hfontUse != kCIDGraphDev::hfontInvalid)
            ::SendMessage(hwndThis(), WM_SETFONT, tCIDCtrls::TWParam(hfontUse), 0);
    }

    //
    //  Set our window text now that the font is set. In our case we have to deal with
    //  new lines, so do that at this point as well.
    //
    TString strTmp(dlgiSrc.strText());
    tCIDLib::TCard4 c4At = 0;
    strTmp.bReplaceSubStr(L"\\n", L"\n", c4At, kCIDLib::True, kCIDLib::False);
    strWndText(strTmp);
}


tCIDLib::TVoid TStaticMultiText::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_FinePrint);
    colToFill.objAdd(kCIDCtrls::strHint_Instruct);
    colToFill.objAdd(kCIDCtrls::strHint_NoBorder);
    colToFill.objAdd(kCIDCtrls::strHint_ReportClicks);
    colToFill.objAdd(kCIDCtrls::strHint_SourceCode);
}


// ---------------------------------------------------------------------------
// TStaticMultiText: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get the current text
tCIDLib::TCard4 TStaticMultiText::c4GetText(TString& strToFill)
{
    tCIDCtrls::TMsgResult mresLen = ::SendMessage(hwndSafe(), WM_GETTEXTLENGTH, 0, 0);

    strToFill.Clear();
    if (mresLen)
    {
        tCIDLib::TCh* pchBuf = new tCIDLib::TCh[mresLen + 1];
        TArrayJanitor<tCIDLib::TCh> janBuf(pchBuf);

        ::SendMessage(hwndSafe(), WM_GETTEXT, mresLen + 1, tCIDLib::TInt4(pchBuf));
        strToFill.ImportChars(pchBuf, mresLen);
    }

    return mresLen;
}


tCIDLib::TVoid
TStaticMultiText::Create(const  TWindow&                wndParent
                        , const tCIDCtrls::TWndId       widThis
                        , const TArea&                  areaInit
                        , const TString&                strText
                        , const tCIDCtrls::EWndStyles   eStyles
                        , const tCIDCtrls::EMLSStyles   eMLSStyles
                        , const tCIDLib::EHJustify      eHJustify
                        , const tCIDCtrls::EExWndStyles eExStyles)
{
    //
    //  Has to be a child window and single line (which is the default). We
    //  we allow it to autoscroll and we don't hide the selection when it loses
    //  focus.
    //
    tCIDLib::TCard4 c4Styles = SS_EDITCONTROL;

    // Set the justificiation
    if (eHJustify == tCIDLib::EHJustify::Left)
        c4Styles |= SS_LEFT;
    else if (eHJustify == tCIDLib::EHJustify::Center)
        c4Styles |= SS_CENTER;
    else
        c4Styles |= SS_RIGHT;

    // Turn on notifications if needed
    if (tCIDLib::bAllBitsOn(eMLSStyles, tCIDCtrls::EMLSStyles::ReportClicks))
        c4Styles |= SS_NOTIFY;

    if (tCIDLib::bAllBitsOn(eMLSStyles, tCIDCtrls::EMLSStyles::NoWrap))
        c4Styles |= SS_LEFTNOWORDWRAP;

    // Make sure tab stop and group are off, since that will cause issues
    tCIDCtrls::EWndStyles eRealStyles(eStyles | tCIDCtrls::EWndStyles(c4Styles));

    eRealStyles = tCIDLib::eClearEnumBits
    (
        eRealStyles, tCIDCtrls::EWndStyles::TabGroup
    );

    // Store our class speicfic styles
    m_eMLSStyles = eMLSStyles;

    // If asked to size to the text, then start us off invisible
    if (tCIDLib::bAllBitsOn(eMLSStyles, tCIDCtrls::EMLSStyles::SizeToText))
        eRealStyles = tCIDLib::eClearEnumBits(eRealStyles, tCIDCtrls::EWndStyles::Visible);

    CreateWnd
    (
        wndParent.hwndThis()
        , L"STATIC"
        , strText.pszBuffer()
        , areaInit
        , eRealStyles
        , eExStyles
        , widThis
    );

    // If asked to size to the text, then do that
    if (tCIDLib::bAllBitsOn(eMLSStyles, tCIDCtrls::EMLSStyles::SizeToText))
    {
        SizeToText();

        // And now show us if we were originally asked ot
        if (tCIDLib::bAllBitsOn(eStyles, tCIDCtrls::EWndStyles::Visible))
            SetVisibility(kCIDLib::True);
    }

    // All standard controls must be subclassed
    SetSubclass();
}


// Set a text color to use
tCIDLib::TVoid TStaticMultiText::SetTextColor(const TRGBClr& rgbToSet)
{
    if (!m_prgbText)
        m_prgbText = new TRGBClr(rgbToSet);
    else
        *m_prgbText = rgbToSet;

    // If we've been created, then force a redraw
    if (bIsValid())
        Redraw(tCIDCtrls::ERedrawFlags::ImmedErase);
}


tCIDLib::TVoid TStaticMultiText::SizeToText()
{
    //
    //  This is a bit of a pain. We have to parse through the text and find the
    //  longest line for the width, and count the lines as we go to figure out
    //  the height.
    //
    TGraphWndDev gdevTmp(*this);

    const TString& strText = strWndText();
    TTextStringInStream strmSrc(&strText);
    TString strCurLine;
    tCIDLib::TCard4 c4LineCount = 0;
    tCIDLib::TCard4 c4MaxWidth = 0;
    TArea areaCur;
    while (!strmSrc.bEndOfStream())
    {
        strmSrc >> strCurLine;
        c4LineCount++;
        areaCur = gdevTmp.areaString(strCurLine);
        if (areaCur.c4Width() > c4MaxWidth)
            c4MaxWidth = areaCur.c4Width();
    }

    TFontMetrics fmtrCur(gdevTmp);
    TSize szNew(c4MaxWidth, c4LineCount * (fmtrCur.c4Height() - 1));
    SetSize(szNew, kCIDLib::False);
};



// Set new text on us
tCIDLib::TVoid TStaticMultiText::SetText(const TString& strToSet)
{
    ::SendMessage(hwndSafe(), WM_SETTEXT, 0, tCIDLib::TInt4(strToSet.pszBuffer()));
}


// Revert to the default text color if an override was previously done
tCIDLib::TVoid TStaticMultiText::UseDefTextColor()
{
    if (m_prgbText)
    {
        delete m_prgbText;
        m_prgbText = nullptr;

        // If we've been created, then force a redraw
        if (bIsValid())
            Redraw(tCIDCtrls::ERedrawFlags::ImmedErase);
    }
}



// ---------------------------------------------------------------------------
// TStaticMultiText: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TStaticMultiText::AreaChanged(  const   TArea&                  areaPrev
                                , const TArea&                  areaNew
                                , const tCIDCtrls::EPosStates   ePosState
                                , const tCIDLib::TBoolean       bOrgChanged
                                , const tCIDLib::TBoolean       bSizeChanged
                                , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    if (bSizeChanged && (ePosState != tCIDCtrls::EPosStates::Minimized))
        Redraw(tCIDCtrls::ERedrawFlags::Immediate);
}


//
//  We have to report clicks to our parent if our style indicates so. So we override
//  this to watch for those clicks.
//
tCIDLib::TBoolean
TStaticMultiText::bClick(const  tCIDCtrls::EMouseButts  eButton
                        , const tCIDCtrls::EMouseClicks eClick
                        , const TPoint&
                        , const tCIDLib::TBoolean
                        , const tCIDLib::TBoolean)

{
    //
    //  If a left down or left double and we have the report clicks style , then
    //  let's report it.
    //
    if (tCIDLib::bAllBitsOn(m_eMLSStyles, tCIDCtrls::EMLSStyles::ReportClicks)
    &&  ((eClick == tCIDCtrls::EMouseClicks::Down)
    ||   (eClick == tCIDCtrls::EMouseClicks::Double))
    &&  (eButton == tCIDCtrls::EMouseButts::Left))
    {
        StaticChildClick(widThis(), eButton, eClick);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  The base window class will treat static control color type messages as notifications
//  to the extent that it will reflect them back to the control. In our case we override
//  in order to set the text color if one has been set on us. The return doesn't matter
//  for us since the message will be handled normally as well no matter what.
//
tCIDLib::TBoolean
TStaticMultiText::bNotReflect(          TWindow&                wndTar
                                , const tCIDCtrls::TWndMsg      wmsgToDo
                                , const tCIDCtrls::TWParam      wParam
                                , const tCIDCtrls::TLParam      lParam
                                ,       tCIDCtrls::TMsgResult&  mresRet)
{
    switch(wmsgToDo)
    {
        case WM_CTLCOLORSTATIC :
            // Set our text color on it if we have one
            if (m_prgbText)
            {
                HDC hdcDraw = (HDC)wParam;
                ::SetTextColor
                (
                    hdcDraw
                    , RGB(m_prgbText->c4Red(), m_prgbText->c4Green(), m_prgbText->c4Blue())
                );
            }
            break;

        default :
            break;
    };
    return kCIDLib::False;
}

