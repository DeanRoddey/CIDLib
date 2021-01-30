//
// FILE NAME: CIDCtrls_EntryField.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/07/2015
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
//  This file implements the entry field control wrapper.
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
RTTIDecls(TEFChangeInfo,TCtrlNotify)
AdvRTTIDecls(TEntryField, TStdCtrlWnd)



// ---------------------------------------------------------------------------
//   CLASS: TEFChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TEFChangeInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TEFChangeInfo::TEFChangeInfo(const  tCIDCtrls::EEFEvents    eEvent
                            , const TWindow&                wndSrc) :

    TCtrlNotify(wndSrc)
    , m_eEvent(eEvent)
{
}

TEFChangeInfo::~TEFChangeInfo()
{
}


// ---------------------------------------------------------------------------
//  TEFChangeInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEFEvents TEFChangeInfo::eEvent() const
{
    return m_eEvent;
}




// ---------------------------------------------------------------------------
//  CLASS: TEntryField
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TEntryField: Public static data
// ---------------------------------------------------------------------------
const TNotificationId TEntryField::nidChange(L"EntryFieldChangeEvent");


// ---------------------------------------------------------------------------
// TEntryField: Constructors and Destructor
// ---------------------------------------------------------------------------
TEntryField::TEntryField() :

    TStdCtrlWnd()
    , m_eEFStyles(tCIDCtrls::EEFStyles::None)
{
}


TEntryField::TEntryField(const tCIDCtrls::TWndHandle hwndToUse)
{
    // Don't adopt but subclass
    UseHandle(hwndToUse, kCIDLib::False, kCIDLib::True);
}

TEntryField::~TEntryField()
{
}


// ---------------------------------------------------------------------------
// TEntryField: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TEntryField::InitFromDesc(  const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EEFStyles eEFStyles = tCIDCtrls::EEFStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisChild
    );
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Group))
        eStyles |= tCIDCtrls::EWndStyles::Group;

    // Set up any styles based on theme and hitns
    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Number))
        eEFStyles |= tCIDCtrls::EEFStyles::Number;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Password))
        eEFStyles |= tCIDCtrls::EEFStyles::Password;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_ParentMenu))
        eEFStyles |= tCIDCtrls::EEFStyles::ParentMenu;

    // If read only, set that style, else enable tab stop
    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_ReadOnly))
        eEFStyles |= tCIDCtrls::EEFStyles::ReadOnly;
    else
        eStyles |= tCIDCtrls::EWndStyles::TabStop;

    if (eTheme == tCIDCtrls::EWndThemes::DialogBox)
    {
        // Set a sunken border and ctrl fill background unless told not to
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
        {
            eExStyles |= tCIDCtrls::EExWndStyles::SunkenEdge;
            SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::DlgCtrlFill), kCIDLib::True);
        }
    }
     else if (eTheme == tCIDCtrls::EWndThemes::MainWnd)
    {
        // Set a regular border unless told not to
        if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
            eStyles |= tCIDCtrls::EWndStyles::Border;
    }

    // Create it but don't set the text until after we've set the font
    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , TString::strEmpty()
        , eStyles
        , eEFStyles
        , eExStyles
    );

    // Set our window text now that the font is set
    strWndText(dlgiSrc.strText());

    // If there is cue text, then set it.
    if (!dlgiSrc.strCueText().bIsEmpty())
        SetCueText(dlgiSrc.strCueText());
}


tCIDLib::TVoid TEntryField::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);

    colToFill.objAdd(kCIDCtrls::strHint_Group);
    colToFill.objAdd(kCIDCtrls::strHint_NoBorder);
    colToFill.objAdd(kCIDCtrls::strHint_ParentMenu);
    colToFill.objAdd(kCIDCtrls::strHint_Password);
    colToFill.objAdd(kCIDCtrls::strHint_ReadOnly);
}


// ---------------------------------------------------------------------------
// TEntryField: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TEntryField::bIsEmpty() const
{
    return (::GetWindowTextLength(hwndSafe()) == 0);
}


//
//  Get the indices of the selection, if any. If none, returns false. The end is the point
//  after the last selected character.
//
tCIDLib::TBoolean
TEntryField::bGetSelectionIndices(tCIDLib::TCard4& c4Start, tCIDLib::TCard4& c4End)
{
    // If no text, then clearly can't have any selection
    const tCIDLib::TCard4 c4CurLen = strWndText().c4Length();
    if (!c4CurLen)
        return kCIDLib::False;

    // Fet the selection indices
    ::SendMessage
    (
        hwndSafe(), EM_GETSEL, tCIDCtrls::TWParam(&c4Start), tCIDCtrls::TLParam(&c4End)
    );

    //
    //  If the start is -1, then there's no selection. If the end is then the selection
    //  goes to the end.
    //
    if ((c4Start == kCIDLib::c4MaxCard) || (c4End == 0))
        return kCIDLib::False;

    return kCIDLib::True;
}


// Query any selected text. If none, we return false and the output string is empty.
tCIDLib::TBoolean
TEntryField::bGetSelectedText(TString& strToFill) const
{
    // First we hvae to get the selection indices
    tCIDLib::TCard4 c4Start, c4End;
    ::SendMessage
    (
        hwndSafe(), EM_GETSEL, tCIDCtrls::TWParam(&c4Start), tCIDCtrls::TLParam(&c4End)
    );

    //
    //  If the start is -1, then there's no selection. If the end is, then that
    //  is fine since our substring copy takes that to mean just to the end of
    //  the string.
    //
    if (c4Start == kCIDLib::c4MaxCard)
        return kCIDLib::False;

    // Now query the text and pull out the substring
    TString strCurText = strWndText();

    // If start is beyond the length, then we don't want to cause an exception
    if (c4Start >= strCurText.c4Length())
        return kCIDLib::False;

    // Figure out the length
    tCIDLib::TCard4 c4Len(c4End - c4Start);
    strCurText.CopyOutSubStr(strToFill, c4Start, c4Len);

    return !strToFill.bIsEmpty();
}


// Set or clear the read only style
tCIDLib::TBoolean TEntryField::bReadOnly() const
{
    return tCIDLib::bAllBitsOn(eWndStyles(), tCIDCtrls::EWndStyles(ES_READONLY));
}

tCIDLib::TBoolean TEntryField::bReadOnly(const tCIDLib::TBoolean bToSet)
{
    // This has to be done via a message, not by updating the style
    ::SendMessage(hwndSafe(), EM_SETREADONLY, bToSet, 0);
    return bToSet;
}


tCIDLib::TBoolean TEntryField::bSelectionToClipboard() const
{
    TString strSel;
    if (bGetSelectedText(strSel))
    {
        try
        {
            TGUIClipboard gclipTmp(*this);
            gclipTmp.Clear();
            gclipTmp.StoreText(strSel);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);

            TErrBox msgbFailed(L"Entry Field Error", errToCatch.strErrText());
            msgbFailed.ShowIt(*this);

            return kCIDLib::False;
        }

        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TVoid
TEntryField::Create(const   TWindow&                wndParent
                    , const tCIDCtrls::TWndId       widThis
                    , const TArea&                  areaInit
                    , const TString&                strText
                    , const tCIDCtrls::EWndStyles   eStyles
                    , const tCIDCtrls::EEFStyles    eEFStyles
                    , const tCIDCtrls::EExWndStyles eExStyles)
{
    // Set up any control specific styles
    tCIDLib::TCard4 c4Styles = ES_AUTOHSCROLL; // | ES_NOHIDESEL;

    if (tCIDLib::bAllBitsOn(eEFStyles, tCIDCtrls::EEFStyles::Password))
        c4Styles |= ES_PASSWORD;

    if (tCIDLib::bAllBitsOn(eEFStyles, tCIDCtrls::EEFStyles::Number))
        c4Styles |= ES_NUMBER;

    if (tCIDLib::bAllBitsOn(eEFStyles, tCIDCtrls::EEFStyles::ReadOnly))
        c4Styles |= ES_READONLY;

    if (tCIDLib::bAllBitsOn(eEFStyles, tCIDCtrls::EEFStyles::UpperCase))
        c4Styles |= ES_UPPERCASE;

    // Store away our class specific styles
    m_eEFStyles = eEFStyles;

    // And now create the control
    CreateWnd
    (
        wndParent.hwndThis()
        , L"EDIT"
        , strText.pszBuffer()
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles(c4Styles)
        , eExStyles
        , widThis
    );

    // All standard controls must be subclassed
    SetSubclass();
}


tCIDCtrls::EEFStyles TEntryField::eEFStyles() const
{
    return m_eEFStyles;
}


//
//  If there's a selection, it replaces the selection. If not, it inserts at the
//  caret position. They can tell us if they want us to select it or not.
//
tCIDLib::TVoid
TEntryField::InsertText(const TString& strToIns, const tCIDLib::TBoolean bSelect)
{
    // Remember the insert position if selecting new stuff
    tCIDLib::TCard4 c4InsertAt, c4End;
    ::SendMessage
    (
        hwndSafe(), EM_GETSEL, tCIDCtrls::TWParam(&c4InsertAt), tCIDCtrls::TLParam(&c4End)
    );

    if (c4InsertAt == kCIDLib::c4MaxCard)
        c4InsertAt = 0;

    ::SendMessage(hwndSafe(), EM_REPLACESEL, TRUE, tCIDCtrls::TLParam(strToIns.pszBuffer()));

    // If asked to select, do that
    const tCIDLib::TCard4 c4SrcLen = strToIns.c4Length();
    if (bSelect && c4SrcLen)
        SetSelection(c4InsertAt, c4InsertAt + c4SrcLen);

}


// Remove any current selection
tCIDLib::TVoid TEntryField::RemoveSelection()
{
    ::SendMessage(hwndSafe(), EM_SETSEL, -1, 0);
}


// Set our cue text
tCIDLib::TVoid TEntryField::SetCueText(const TString& strToSet)
{
    tCIDCtrls::TWParam wShowAlways = FALSE;
    if (tCIDLib::bAllBitsOn(m_eEFStyles, tCIDCtrls::EEFStyles::CueOnFocus))
        wShowAlways = TRUE;

    ::SendMessage
    (
        hwndSafe()
        , EM_SETCUEBANNER
        , wShowAlways
        , tCIDCtrls::TLParam(strToSet.pszBuffer())
    );
}


// Select all or a subset of the text
tCIDLib::TVoid TEntryField::SelectAll()
{
    ::SendMessage(hwndSafe(), EM_SETSEL, 0, -1);
}

tCIDLib::TVoid
TEntryField::SetSelection(  const   tCIDLib::TCard4 c4StartInd
                            , const tCIDLib::TCard4 c4EndInd)
{
    tCIDCtrls::TLParam lEnd;
    if (c4EndInd == kCIDLib::c4MaxCard)
        lEnd = -1;
    else
        lEnd = tCIDCtrls::TLParam(c4EndInd);

    ::SendMessage(hwndSafe(), EM_SETSEL, c4StartInd, lEnd);
}


// Return a default size for the resource editor
TSize TEntryField::szDefault() const
{
    TSize szInit(96, 11);
    TGUIFacility::AdjustDlgUnits(szInit);
    return szInit;
}


// ---------------------------------------------------------------------------
//  TEngryField: Protected, inherited methods
// ---------------------------------------------------------------------------

// Handle some notifications and pass them on to any listeners
tCIDLib::TBoolean
TEntryField::bNotReflect(       TWindow&                wndTar
                        , const tCIDCtrls::TWndMsg      wmsgMsg
                        , const tCIDCtrls::TWParam      wParam
                        , const tCIDCtrls::TLParam      iParam
                        ,       tCIDCtrls::TMsgResult&  mresResult)
{
    // We expect a command type message, not notifity
    if (wmsgMsg != WM_COMMAND)
        return 0;

    // Get the type. The source is us, so we don't care about that
    const tCIDLib::TCard4 c4Type = (wParam & 0xFFFF0000) >> 16;

    if (c4Type == EN_CHANGE)
    {
        TEFChangeInfo wnotToSend(tCIDCtrls::EEFEvents::Changed, *this);
        SendSyncNotify(wnotToSend, TEntryField::nidChange);

        return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  If we have the parent menu style, we return propogate, to let it go to the parent,
//  else we return Pass, to let it go to the underlying control.
//
tCIDCtrls::ECtxMenuOpts
TEntryField::eShowContextMenu(const TPoint& pntAt, const tCIDCtrls::TWndId)
{
    if (tCIDLib::bAllBitsOn(m_eEFStyles, tCIDCtrls::EEFStyles::ParentMenu))
    {
        // Make sure we have the focus
        TakeFocus();
        return tCIDCtrls::ECtxMenuOpts::Propogate;
    }
    return tCIDCtrls::ECtxMenuOpts::Pass;
}

