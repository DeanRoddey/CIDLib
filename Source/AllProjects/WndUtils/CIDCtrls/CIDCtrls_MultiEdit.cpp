//
// FILE NAME: CIDCtrls_MultiEdit.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/08/2015
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the
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
#include    <richedit.h>




// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMLEChangeInfo,TCtrlNotify)
AdvRTTIDecls(TMultiEdit, TStdCtrlWnd)



// ---------------------------------------------------------------------------
//   CLASS: TMLEChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMLEChangeInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMLEChangeInfo::TMLEChangeInfo( const   tCIDCtrls::EMLEEvents   eEvent
                                , const TWindow&                wndSrc) :

    TCtrlNotify(wndSrc)
    , m_eEvent(eEvent)
{
}

TMLEChangeInfo::TMLEChangeInfo(const TMLEChangeInfo& wnotSrc) :

    TCtrlNotify(wnotSrc)
    , m_eEvent(wnotSrc.m_eEvent)
{
}

TMLEChangeInfo::~TMLEChangeInfo()
{
}


// ---------------------------------------------------------------------------
//  TMLEChangeInfo: Public operators
// ---------------------------------------------------------------------------
TMLEChangeInfo& TMLEChangeInfo::operator=(const TMLEChangeInfo& wnotSrc)
{
    if (this != &wnotSrc)
    {
        TParent::operator=(wnotSrc);
        m_eEvent = wnotSrc.m_eEvent;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TMLEChangeInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EMLEEvents TMLEChangeInfo::eEvent() const
{
    return m_eEvent;
}



// ---------------------------------------------------------------------------
//  CLASS: TMultiEdit
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMultiEdit: Public static data
// ---------------------------------------------------------------------------
const TNotificationId TMultiEdit::nidChange(L"MLEChangeEvent");


// ---------------------------------------------------------------------------
// TMultiEdit: Constructors and Destructor
// ---------------------------------------------------------------------------
TMultiEdit::TMultiEdit() :

    TStdCtrlWnd()
    , m_eMLEStyles(tCIDCtrls::EMLEStyles::None)
{
}

TMultiEdit::~TMultiEdit()
{
}


// ---------------------------------------------------------------------------
// TMultiEdit: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TMultiEdit::InitFromDesc(const  TWindow&                wndParent
                        , const TDlgItem&               dlgiSrc
                        , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EMLEStyles eMLEStyles = tCIDCtrls::EMLEStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild
    );
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Group))
        eStyles |= tCIDCtrls::EWndStyles::Group;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_ReadOnly))
    {
        eMLEStyles |= tCIDCtrls::EMLEStyles::ReadOnly;

        // Turn off tab stop if read only
        eStyles = tCIDLib::eClearEnumBits(eStyles, tCIDCtrls::EWndStyles::TabStop);
    }

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_ReportChanges))
        eMLEStyles |= tCIDCtrls::EMLEStyles::ReportChanges;

    if (eTheme == tCIDCtrls::EWndThemes::DialogBox)
    {
        // Set a sunken border and ctrl fill background unless told not to
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
        {
            eExStyles |= tCIDCtrls::EExWndStyles::SunkenEdge;
            SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::DlgCtrlFill), kCIDLib::True);
        }
         else
        {
            // If the parent is a dialog box, set the background to dialog fill
            if (wndParent.bIsDescendantOf(TDlgBox::clsThis()))
                SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::DialogBgn), kCIDLib::True);
        }
    }
     else if (eTheme == tCIDCtrls::EWndThemes::MainWnd)
    {
        // Set a regular border unless told not to
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
            eStyles |= tCIDCtrls::EWndStyles::Border;
    }

    // If auto-scrolling, turn that style on, and enable the vertical scroll bar
    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_AutoScroll))
    {
        eStyles |= tCIDCtrls::EWndStyles::VScroll;
        eMLEStyles |= tCIDCtrls::EMLEStyles::AutoScroll;
    }

    // If they want this guy to process enter keys, turn that on
    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_WantEnter))
        eMLEStyles |= tCIDCtrls::EMLEStyles::WantEnter;


    Create
    (
        wndParent, dlgiSrc.ridChild(), dlgiSrc.areaPos(), eStyles, eMLEStyles, eExStyles
    );

    // Set the text. We have to deal with new lines in our case
    TString strTmp(dlgiSrc.strText());
    tCIDLib::TCard4 c4At = 0;
    strTmp.bReplaceSubStr(L"\\n", L"\n", c4At, kCIDLib::True, kCIDLib::False);
    strWndText(strTmp);
}


tCIDLib::TVoid TMultiEdit::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_AutoScroll);
    colToFill.objAdd(kCIDCtrls::strHint_Group);
    colToFill.objAdd(kCIDCtrls::strHint_NoBorder);
    colToFill.objAdd(kCIDCtrls::strHint_ReadOnly);
    colToFill.objAdd(kCIDCtrls::strHint_ReportChanges);
    colToFill.objAdd(kCIDCtrls::strHint_WantEnter);
}


//
//  This guy doesn't use the standard mechanism for setting the background color,
//  which is annoying. So this method is virtual in order to allow controls like this
//  to override and do type specific stuff.
//
tCIDLib::TVoid
TMultiEdit::SetBgnColor(const   TRGBClr&            rgbToSet
                        , const tCIDLib::TBoolean   bOver
                        , const tCIDLib::TBoolean   bRedraw)
{
    //
    //  Do ours first, but only if either our background hasn't already been explicitly
    //  set or they say we can override. Also make sure we are valid, since we may not
    //  be yet.
    //
    if ((bOver || !bBgnColorOver()) && bIsValid())
        ::SendMessage(hwndSafe(), EM_SETBKGNDCOLOR, 0, rgbToSet.c4Color());


    //
    //  Then pass on to our parent, since he will handle the redraw flag and we want that
    //  to happen after we do our thing.
    //
    TParent::SetBgnColor(rgbToSet, bOver, bRedraw);
}



// ---------------------------------------------------------------------------
// TMultiEdit: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TMultiEdit::bFindText(  const   TString&            strToFind
                        ,       tCIDLib::TCard4&    c4StartOfs
                        ,       tCIDLib::TCard4&    c4EndOfs
                        , const tCIDLib::TBoolean   bFullMatchOnly
                        , const tCIDLib::TBoolean   bFirst)
{
    tCIDCtrls::TWParam wParam = FR_DOWN;
    if (bFullMatchOnly)
        wParam |= FR_WHOLEWORD;

    FINDTEXTEX Find = { 0 };
    Find.lpstrText = (LPCTSTR)strToFind.pszBuffer();

    // Get the current selection
    DWORD wStart, wEnd;
    ::SendMessage
    (
        hwndSafe(), EM_GETSEL,tCIDCtrls::TWParam(&wStart), tCIDCtrls::TLParam(&wEnd)
    );

    // We always start at the selection if a first find, else the end
    Find.chrg.cpMin = wStart;
    if (!bFirst)
        Find.chrg.cpMin = wEnd;

    // And we search to the end of the file
    Find.chrg.cpMax = -1;

    // And try it
    tCIDCtrls::TMsgResult mresAt = ::SendMessage
    (
        hwndSafe(), EM_FINDTEXTEXW, wParam, tCIDCtrls::TLParam(&Find)
    );

    if (mresAt != -1)
    {
        c4StartOfs = Find.chrgText.cpMin;
        c4EndOfs = Find.chrgText.cpMax;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
TMultiEdit::bQuerySelectRange(tCIDLib::TCard4& c4StartOfs, tCIDLib::TCard4& c4EndOfs)
{
    CHARRANGE chrgSel;
   ::SendMessage(hwndSafe(), EM_GETSEL, 0, tCIDCtrls::TLParam(&chrgSel));

    if (chrgSel.cpMin == chrgSel.cpMax)
        return kCIDLib::False;

    c4StartOfs = chrgSel.cpMin;
    c4EndOfs = chrgSel.cpMax;
    return kCIDLib::True;
}


tCIDLib::TCard4 TMultiEdit::c4QueryCharCount() const
{
    // Get the position of the end of the file
    GETTEXTLENGTHEX GetLen = {0};
    GetLen.flags = GTL_DEFAULT;
    GetLen.codepage = 1200;
    tCIDCtrls::TMsgResult mresLen = ::SendMessage
    (
        hwndThis(), EM_GETTEXTLENGTHEX, tCIDCtrls::TWParam(&GetLen), 0
    );

    // Supposedly it only returns an error if we give bad values in the structure
    return tCIDLib::TCard4(mresLen);
}


//
//  Set up our styles and call our parent to create us
//
tCIDLib::TVoid
TMultiEdit::Create( const   TWindow&                wndParent
                    , const tCIDCtrls::TWndId       widThis
                    , const TArea&                  areaInit
                    , const tCIDCtrls::EWndStyles   eStyles
                    , const tCIDCtrls::EMLEStyles   eMLEStyles
                    , const tCIDCtrls::EExWndStyles eExStyles)
{
    // Load the containing DLL if not already done
    static TAtomicFlag atomLoaded;
    if (!atomLoaded)
    {
        TBaseLock lockInit;
        if (!atomLoaded)
        {
            ::LoadLibrary(L"Msftedit.dll");
            atomLoaded.Set();
        }
    }

    // Set up our styles, starting with any we have to have for this class
    tCIDLib::TCard4 c4Styles = ES_MULTILINE; // | ES_NOHIDESEL;

    // And add in any of the lib box specific ones
    if (tCIDLib::bAllBitsOn(eMLEStyles, tCIDCtrls::EMLEStyles::AutoScroll))
         c4Styles |= ES_AUTOVSCROLL;

    if (tCIDLib::bAllBitsOn(eMLEStyles, tCIDCtrls::EMLEStyles::WantEnter)
    &&  !tCIDLib::bAnyBitsOn(eMLEStyles, tCIDCtrls::EMLEStyles::ReadOnly))
    {
        c4Styles |= ES_WANTRETURN;
    }

    if (tCIDLib::bAllBitsOn(eMLEStyles, tCIDCtrls::EMLEStyles::ReadOnly))
        c4Styles |= ES_READONLY;

    // Store our class specific styles
    m_eMLEStyles = eMLEStyles;

    // And create the control
    CreateWnd
    (
        wndParent.hwndThis()
        , MSFTEDIT_CLASS
        , kCIDLib::pszEmptyZStr
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles(c4Styles)
        , eExStyles
        , widThis
    );

    // All standard controls must be subclassed
    SetSubclass();

    // If they asked to have changes reported, set that event mask
    if (tCIDLib::bAllBitsOn(eMLEStyles, tCIDCtrls::EMLEStyles::ReportChanges))
    {
        ::SendMessage(hwndThis(), EM_SETEVENTMASK, 0, ENM_CHANGE);
    }

    //
    //  They may have set our background color before we were created and this control
    //  requires a special message. So update it if it's been overridden already.
    //
    if (bBgnColorOver())
        ::SendMessage(hwndSafe(), EM_SETBKGNDCOLOR, 0, rgbBgnFill().c4Color());

    // Set margins on it to make it more readable
    ::SendMessage
    (
        hwndThis()
        , EM_SETMARGINS
        , EC_LEFTMARGIN | EC_RIGHTMARGIN
        , 0x00080008
    );

    //
    //  By default make them friendly to tabbing by just letting them see arrows and
    //  and characters. This will let tabs go through for tabbing. In those few places
    //  where need more they can set input wants specifically.
    //
    //  But if WantEnter is set, we have to make it all characters.
    //
    if (tCIDLib::bAllBitsOn(eMLEStyles, tCIDCtrls::EMLEStyles::WantEnter))
        SetWantInput(tCIDCtrls::EWantInputs::AllKeys);
    else
        SetWantInput(tCIDCtrls::EWantInputs::CharsArrows);
}


//
//  Move us to the start of the indicated line, if possible. The lines are assumed to be
//  the 1 based numbers of the outside world.
//
tCIDLib::TVoid TMultiEdit::GoToLine(const tCIDLib::TCard4 c4LineNum)
{
    tCIDLib::TCard4 c4RealLine = c4LineNum;
    if (!c4RealLine)
        c4RealLine++;

    tCIDCtrls::TMsgResult mresPos = ::SendMessage(hwndSafe(), EM_LINEINDEX, c4RealLine - 1, 0);
    if (mresPos == -1)
    {
        // It wasn't valid, so get the line count, and it's offset, and go there
        mresPos = ::SendMessage(hwndThis(), EM_GETLINECOUNT, 0, 0);
        mresPos = ::SendMessage(hwndSafe(), EM_LINEINDEX, mresPos - 1, 0);

        CHARRANGE Range{ mresPos, mresPos };
        ::SendMessage(hwndThis(), EM_EXSETSEL, 0, tCIDCtrls::TLParam(&Range));
    }
     else
    {
        // Take the caller's position as is
        CHARRANGE Range{ mresPos, mresPos };
        ::SendMessage(hwndThis(), EM_EXSETSEL, 0, tCIDCtrls::TLParam(&Range));
    }
}


// Select any text in the editor control
tCIDLib::TVoid TMultiEdit::SelectAll()
{
    CHARRANGE Range{ 0, -1 };
    ::SendMessage(hwndSafe(), EM_EXSETSEL, 0, tCIDCtrls::TLParam(&Range));
}


tCIDLib::TVoid
TMultiEdit::SelectRange(const tCIDLib::TCard4 c4StartOfs, const tCIDLib::TCard4 c4EndOfs)
{
    CHARRANGE Range{ int(c4StartOfs), int(c4EndOfs) };
    ::SendMessage(hwndSafe(), EM_EXSETSEL, 0, tCIDCtrls::TLParam(&Range));
}


// Let client code set a new font on us
tCIDLib::TVoid TMultiEdit::SetNewFont(const TFontSelAttrs& fselNew)
{
    TGraphWndDev gdevTmp(*this);

    CHARFORMAT2 Fmt;
    TRawMem::SetMemBuf(&Fmt, kCIDLib::c1MinCard, sizeof(Fmt));
    Fmt.cbSize = sizeof(Fmt);

    Fmt.dwMask = CFM_SIZE | CFM_WEIGHT | CFM_CHARSET;
    const LOGFONT& FontInfo = fselNew.HostInfo();
    Fmt.bCharSet = FontInfo.lfCharSet;
    Fmt.bPitchAndFamily = FontInfo.lfPitchAndFamily;
    if (FontInfo.lfHeight < 0)
        Fmt.yHeight = (FontInfo.lfHeight * -1) * (1440 / gdevTmp.c4VPelsPerInch());
    else
        Fmt.yHeight = FontInfo.lfHeight * (1440 / gdevTmp.c4VPelsPerInch());
    Fmt.wWeight = (WORD)FontInfo.lfWeight;

    if (TRawStr::c4StrLen(FontInfo.lfFaceName))
    {
        TRawStr::CopyStr(Fmt.szFaceName, FontInfo.lfFaceName, LF_FACESIZE);
        Fmt.dwMask |= CFM_FACE;
    }
    // Update the MLE with this foramt, for all text
    ::SendMessage
    (
        hwndSafe()
        , EM_SETCHARFORMAT
        , tCIDCtrls::TWParam(SCF_ALL)
        , tCIDCtrls::TLParam(&Fmt)
    );
}


//
//  Change the WantEnter style after the fact. We also have to update SetWantInput
//  to match.
//
tCIDLib::TVoid TMultiEdit::SetWantEnter(const tCIDLib::TBoolean bToSet)
{
    // Get the current styles
    DWORD dwCurStyles = (DWORD)::GetWindowLongPtr(hwndThis(), GWL_STYLE);

    // Set the want input and update the current styles accordingly
    if (bToSet)
    {
        SetWantInput(tCIDCtrls::EWantInputs::AllKeys);
        dwCurStyles |= ES_WANTRETURN;
    }
     else
    {
        SetWantInput(tCIDCtrls::EWantInputs::CharsArrows);
        dwCurStyles &= ~ES_WANTRETURN;
    }

    // And put the styles back
    ::SetWindowLongPtr(hwndThis(), GWL_STYLE, dwCurStyles);
}


// ---------------------------------------------------------------------------
// TMultiEdit: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  To want to implement some standard hot keys, so we intercept some keyboard input
//  messages.
//
tCIDLib::TBoolean
TMultiEdit::bEatSubClassMsg(const   tCIDCtrls::TWndMsg      wmsgMsg
                            , const tCIDCtrls::TWParam      wParam
                            , const tCIDCtrls::TLParam      lParam
                            ,       tCIDCtrls::TMsgResult&  mresRet)
{
    tCIDLib::TBoolean bRet = kCIDLib::False;

    if (wmsgMsg == WM_CHAR)
    {
        if (wmsgMsg == WM_CHAR)
        {
            if (wParam == 1)
            {
                // Ctrl-A, so do a select all
                CHARRANGE Range{ 0, -1 };
                ::SendMessage(hwndSafe(), EM_EXSETSEL, 0, tCIDCtrls::TLParam(&Range));
            }
             else if (wParam == 3)
            {
                // Ctrl-C so copy text
                ::SendMessage(hwndSafe(), WM_COPY, 0, 0);
            }
             else if (wParam == 22)
            {
                // Ctrl-V so paste text
                ::SendMessage(hwndSafe(), WM_PASTE, 0, 0);
            }
             else if (wParam == 24)
            {
                // Ctrl-X so cut text
                ::SendMessage(hwndSafe(), WM_CUT, 0, 0);
            }
        }
    }
    return bRet;
}



//
//  This guy doesn't update the scroll bar position during drags, so we have to
//  intervene and do it for him. He moves it at the final release location, so we
//  only do the non-final ones.
//
//  We also post pass on some notifications
//
tCIDLib::TBoolean
TMultiEdit::bNotReflect(        TWindow&                wndTar
                        , const tCIDCtrls::TWndMsg      wmsgMsg
                        , const tCIDCtrls::TWParam      wParam
                        , const tCIDCtrls::TLParam      lParam
                        ,       tCIDCtrls::TMsgResult&  mresRet)
{
    if ((wmsgMsg == WM_HSCROLL) || (wmsgMsg == WM_VSCROLL))
    {
        tCIDLib::TCard2 c2Event(wParam & 0xFFFF);
        if (c2Event == SB_THUMBTRACK)
        {
            tCIDLib::TCard4 c4Pos = tCIDLib::TCard4(wParam >> 16);
            ::SetScrollPos
            (
                wndTar.hwndSafe()
                , (wmsgMsg == WM_HSCROLL) ? SB_HORZ : SB_VERT
                , c4Pos
                , kCIDLib::True
            );
        }
    }
     else if (wmsgMsg == WM_COMMAND)
    {
        const tCIDLib::TCard4 c4Type = (wParam & 0xFFFF0000) >> 16;
        if (c4Type == EN_CHANGE)
        {
            TMLEChangeInfo wnotToSend(tCIDCtrls::EMLEEvents::Changed, *this);
            SendSyncNotify(wnotToSend, TMultiEdit::nidChange);
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}

