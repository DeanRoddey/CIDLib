//
// FILE NAME: CIDCtrls_StaticText.cpp
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
AdvRTTIDecls(TStaticText, TStdCtrlWnd)



// ---------------------------------------------------------------------------
//  CLASS: TStaticText
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TStaticText: Constructors and Destructor
// ---------------------------------------------------------------------------
TStaticText::TStaticText() :

    TStdCtrlWnd()
    , m_eSTextStyles(tCIDCtrls::ESTextStyles::None)
    , m_prgbText(nullptr)
{
}

TStaticText::~TStaticText()
{
    // If a text color override was set, free it
    delete m_prgbText;
}


// ---------------------------------------------------------------------------
// TStaticText: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TStaticText::InitFromDesc(  const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::ESTextStyles eTextStyles = tCIDCtrls::ESTextStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles::VisChild;
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None;
    tCIDLib::EHJustify eHorz = tCIDLib::EHJustify::Left;;

    // If a prefix, then set right justification
    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Prefix))
        eHorz = tCIDLib::EHJustify::Right;

    // If centered then set that
    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Center))
        eHorz = tCIDLib::EHJustify::Center;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_ReportClicks))
        eTextStyles |= tCIDCtrls::ESTextStyles::ReportClicks;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_EndEllipsis))
        eTextStyles |= tCIDCtrls::ESTextStyles::EndEllipsis;

    //
    //  If an emphasized value, we use a border and if in dialog theme a dialog
    //  control background. If it's a current value, then the same plus center.
    //
    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_EmphVal)
    ||  dlgiSrc.bHasHint(kCIDCtrls::strHint_CurValue))
    {
        if (eTheme == tCIDCtrls::EWndThemes::MainWnd)
        {
            eStyles |= tCIDCtrls::EWndStyles::Border;
        }
         else
        {
            eExStyles |= tCIDCtrls::EExWndStyles::SunkenEdge;
            SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::DlgCtrlFill), kCIDLib::True);
        }

        if (dlgiSrc.bHasHint(kCIDCtrls::strHint_CurValue))
            eHorz = tCIDLib::EHJustify::Center;
    }

    // Create it but don't set the text until after we've set the font
    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , TString::strEmpty()
        , eStyles
        , eTextStyles
        , eHorz
        , eExStyles
    );

    //
    //  If a title, fineprint, prefix, or label, override the default normal font
    //  that would have been set by our parent class.
    //
    {
        tCIDGraphDev::TFontHandle hfontUse = kCIDGraphDev::hfontInvalid;
        if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Prefix)
        ||  dlgiSrc.bHasHint(kCIDCtrls::strHint_Label))
            hfontUse = TGUIFacility::gfontDefBold().hfontThis();
        else if (dlgiSrc.bHasHint(kCIDCtrls::strHint_FinePrint))
            hfontUse = TGUIFacility::gfontDefSmall().hfontThis();
        else if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Title))
            hfontUse = TGUIFacility::gfontDefTitle().hfontThis();

        if (hfontUse != kCIDGraphDev::hfontInvalid)
            ::SendMessage(hwndThis(), WM_SETFONT, tCIDCtrls::TWParam(hfontUse), 0);
    }

    // Set our window text now that the font is set
    strWndText(dlgiSrc.strText());
}


tCIDLib::TVoid TStaticText::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_Center);
    colToFill.objAdd(kCIDCtrls::strHint_CurValue);
    colToFill.objAdd(kCIDCtrls::strHint_EmphVal);
    colToFill.objAdd(kCIDCtrls::strHint_EndEllipsis);
    colToFill.objAdd(kCIDCtrls::strHint_FinePrint);
    colToFill.objAdd(kCIDCtrls::strHint_Label);
    colToFill.objAdd(kCIDCtrls::strHint_Prefix);
    colToFill.objAdd(kCIDCtrls::strHint_ReportClicks);
    colToFill.objAdd(kCIDCtrls::strHint_Title);
}


TSize TStaticText::szDefault() const
{
    TSize szInit(96, 10);
    TGUIFacility::AdjustDlgUnits(szInit);
    return szInit;
}


// ---------------------------------------------------------------------------
// TStaticText: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TStaticText::Create(const   TWindow&                wndParent
                    , const tCIDCtrls::TWndId       widThis
                    , const TArea&                  areaInit
                    , const TString&                strText
                    , const tCIDCtrls::EWndStyles   eStyles
                    , const tCIDCtrls::ESTextStyles eSTextStyles
                    , const tCIDLib::EHJustify      eHJustify
                    , const tCIDCtrls::EExWndStyles eExStyles)
{
    tCIDLib::TCard4 c4Styles = 0;

    // Set the justificiation
    if (eHJustify == tCIDLib::EHJustify::Left)
        c4Styles |= SS_LEFTNOWORDWRAP;
    else if (eHJustify == tCIDLib::EHJustify::Center)
        c4Styles |= SS_CENTER;
    else
        c4Styles |= SS_RIGHT;

    // Add in any of our class specific styles
    if (tCIDLib::bAllBitsOn(eSTextStyles, tCIDCtrls::ESTextStyles::NoMnemonics))
        c4Styles |= SS_NOPREFIX;

    if (tCIDLib::bAllBitsOn(eSTextStyles, tCIDCtrls::ESTextStyles::ReportClicks))
        c4Styles |= SS_NOTIFY;

    if (tCIDLib::bAllBitsOn(eSTextStyles, tCIDCtrls::ESTextStyles::EndEllipsis))
        c4Styles |= SS_ENDELLIPSIS;

    // Make sure tab stop and group are off, since that will cause issues
    tCIDCtrls::EWndStyles eRealStyles(eStyles | tCIDCtrls::EWndStyles(c4Styles));
    eRealStyles = tCIDLib::eClearEnumBits(eRealStyles, tCIDCtrls::EWndStyles::TabGroup);

    // Save our class specific styles
    m_eSTextStyles = eSTextStyles;

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

    // All standard controls must be subclassed
    SetSubclass();
}


// Set a text color to use
tCIDLib::TVoid TStaticText::SetTextColor(const TRGBClr& rgbToSet)
{
    if (!m_prgbText)
        m_prgbText = new TRGBClr(rgbToSet);
    else
        *m_prgbText = rgbToSet;

    // If we've been created, then force a redraw
    if (bIsValid())
        Redraw(tCIDCtrls::ERedrawFlags::ImmedErase);
}


// Revert to the default text color if an override was previously done
tCIDLib::TVoid TStaticText::UseDefTextColor()
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
// TStaticText: Protected, inherited methods
// ---------------------------------------------------------------------------

// This guy sucks at redrawing itself if resized, so we force it
tCIDLib::TVoid
TStaticText::AreaChanged(const  TArea&                  areaPrev
                        , const TArea&                  areaNew
                        , const tCIDCtrls::EPosStates   ePosState
                        , const tCIDLib::TBoolean       bOrgChanged
                        , const tCIDLib::TBoolean       bSizeChanged
                        , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    if (bSizeChanged)
        Redraw(tCIDCtrls::ERedrawFlags::ImmedErase);
}


//
//  We have to report clicks to our parent if our style indicates so. So we override
//  this to watch for those clicks.
//
tCIDLib::TBoolean
TStaticText::bClick(const   tCIDCtrls::EMouseButts  eButton
                    , const tCIDCtrls::EMouseClicks eClick
                    , const TPoint&
                    , const tCIDLib::TBoolean
                    , const tCIDLib::TBoolean)

{
    //
    //  If a left down or left double and we have the report clicks style , then
    //  let's report it.
    //
    if (tCIDLib::bAllBitsOn(m_eSTextStyles, tCIDCtrls::ESTextStyles::ReportClicks)
    &&  ((eClick == tCIDCtrls::EMouseClicks::Down)
    ||   (eClick == tCIDCtrls::EMouseClicks::Double))
    &&  (eButton == tCIDCtrls::EMouseButts::Left))
    {
        PassStaticChildClick(eButton, eClick);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  The base window class will treat static control color type messages as notifications
//  to the extent that it will reflect them back to the control. In our case we override
//  in order to set the text color if one has been set on us.
//
tCIDLib::TBoolean
TStaticText::bNotReflect(       TWindow&                wndTar
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
                return kCIDLib::True;
            }
            break;

        default :
            break;
    };
    return kCIDLib::False;
}

