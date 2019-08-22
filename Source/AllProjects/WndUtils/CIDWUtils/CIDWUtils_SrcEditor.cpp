//
// FILE NAME: CIDWUtils_SrcEdit.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/16/2015
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
//  This file implements a simple wrapper around the Scintilla src editing
//  control.
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
#include    "CIDWUtils_.hpp"
#include    <Scintilla.h>
#include    <SciLexer.h>



// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TSrcEdChangeInfo,TCtrlNotify)
RTTIDecls(TSrcEditor, TStdCtrlWnd)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDWUtils_SrcEditor
{
    //
    //  The Scintilla marker ids we use. Order is important, since they overlay
    //  each other based on lower ids.
    //
    const tCIDLib::TCard4   c4Marker_BreakPoint     = 0;
    const tCIDLib::TCard4   c4Marker_BreakPointD    = 1;
    const tCIDLib::TCard4   c4Marker_CurLine        = 2;
};



// ---------------------------------------------------------------------------
//   CLASS: TSrcEdChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSrcEdChangeInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TSrcEdChangeInfo::TSrcEdChangeInfo( const   tCIDWUtils::ESEdEvents  eEvent
                                    , const TString&                strPath
                                    , const tCIDLib::TCard4         c4LineNum
                                    , const TWindow&                wndSrc) :
    TCtrlNotify(wndSrc)
    , m_c4LineNum(c4LineNum)
    , m_eEvent(eEvent)
    , m_i4LineChanges(0)
    , m_strPath(strPath)
{
}

TSrcEdChangeInfo::TSrcEdChangeInfo( const   TString&        strPath
                                    , const tCIDLib::TInt4  i4LineChanges
                                    , const TWindow&        wndSrc) :
    TCtrlNotify(wndSrc)
    , m_c4LineNum(0)
    , m_eEvent(tCIDWUtils::ESEdEvents::Modified)
    , m_i4LineChanges(i4LineChanges)
    , m_strPath(strPath)
{
}

TSrcEdChangeInfo::~TSrcEdChangeInfo()
{
}


// ---------------------------------------------------------------------------
//  TSrcEdChangeInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TSrcEdChangeInfo::c4LineNum() const
{
    return m_c4LineNum;
}


tCIDWUtils::ESEdEvents TSrcEdChangeInfo::eEvent() const
{
    return m_eEvent;
}


tCIDLib::TInt4 TSrcEdChangeInfo::i4LineChanges() const
{
    return m_i4LineChanges;
}


const TString& TSrcEdChangeInfo::strPath() const
{
    return m_strPath;
}




// ---------------------------------------------------------------------------
//   CLASS: TSrcEditor
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSrcEditor: Public static data
// ---------------------------------------------------------------------------
const TNotificationId TSrcEditor::nidChange(L"SrcEditorChangeEvent");


// ---------------------------------------------------------------------------
//  TSrcEditor: Constructors and Destructor
// ---------------------------------------------------------------------------
TSrcEditor::TSrcEditor() :

    TStdCtrlWnd()
    , m_bSelOnly(kCIDLib::False)
    , m_c4LastLine(1)
{
}

TSrcEditor::~TSrcEditor()
{
}


// ---------------------------------------------------------------------------
//  TSrcEditor: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TSrcEditor::InitFromDesc(const  TWindow&                wndParent
                        , const TDlgItem&               dlgiSrc
                        , const tCIDCtrls::EWndThemes   eTheme)
{
    CreateSrcEditor
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , tCIDCtrls::EWndStyles::ClippingVisTabChild
        , TString::strEmpty()
    );
}


tCIDLib::TVoid TSrcEditor::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
}


// ---------------------------------------------------------------------------
//  TSrcEditor: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TSrcEditor::AddRemoveBP(const   tCIDLib::TCard4     c4LineNum
                        , const tCIDLib::TBoolean   bState
                        , const tCIDLib::TBoolean   bSendEvent)
{
    if (!bState && bHasEnabledBP(c4LineNum))
    {
        RemoveLineState(c4LineNum, tCIDWUtils::ECEdLStates::BreakPoint);
        if (bSendEvent)
            SendEdEvent(tCIDWUtils::ESEdEvents::RemoveBP, c4LineNum);
    }
     else if (!bState && bHasDisabledBP(c4LineNum))
    {
        RemoveLineState(c4LineNum, tCIDWUtils::ECEdLStates::DisabledBreak);
        if (bSendEvent)
            SendEdEvent(tCIDWUtils::ESEdEvents::RemoveBP, c4LineNum);
    }
     else if (bState)
    {
        AddLineState(c4LineNum, tCIDWUtils::ECEdLStates::BreakPoint);
        if (bSendEvent)
            SendEdEvent(tCIDWUtils::ESEdEvents::AddBP, c4LineNum);
    }
}


// Return whether redo and undo are possible currently
tCIDLib::TBoolean TSrcEditor::bCanRedo() const
{
    if (::SendMessage(hwndSafe(), SCI_CANREDO, 0, 0))
        return kCIDLib::True;
    return kCIDLib::False;
}

tCIDLib::TBoolean TSrcEditor::bCanUndo() const
{
    if (::SendMessage(hwndSafe(), SCI_CANUNDO, 0, 0))
        return kCIDLib::True;
    return kCIDLib::False;
}


// Returns whether we have changes since load or the last call to ChangesSaved.
tCIDLib::TBoolean TSrcEditor::bChanges() const
{
    return (::SendMessage(hwndSafe(), SCI_GETMODIFY, 0, 0) != 0);
}


// Clear any selection but leave the current position unchanged
tCIDLib::TVoid TSrcEditor::ClearSelection()
{
    // Get the current position and set an empty selection there
    tCIDCtrls::TMsgResult mresCur = ::SendMessage(hwndSafe(),SCI_GETCURRENTPOS,0,0);
    ::SendMessage(hwndThis(), SCI_SETEMPTYSELECTION, mresCur, 0);
}


//
//  This will do a next or previous find. We get the current position and, based
//  on prev/next flag, we set the Scintilla target range accordingly. If we find
//  something in the target range, we select that new match and return true.
//
tCIDLib::TBoolean
TSrcEditor::bFind(const TString& strToFind, const tCIDLib::TBoolean bNext)
{
    tCIDCtrls::TWndHandle hwndUs = hwndSafe();

    //
    //  Get the current position offset. This is the starting point. Depending
    //  on whether it's a forward or back, we will set the other end to either
    //  the start/end of the document.
    //
    tCIDCtrls::TMsgResult mresCurPos = ::SendMessage(hwndUs, SCI_GETCURRENTPOS, 0, 0);


    tCIDCtrls::TMsgResult mresStart;
    tCIDCtrls::TMsgResult mresEnd;

    if (bNext)
    {
        mresEnd = ::SendMessage(hwndUs, SCI_GETLENGTH, 0, 0);
        mresStart = mresCurPos;

        // If the start is at or beyond the end, do nothing
        if (mresStart >= mresEnd)
            return kCIDLib::False;
    }
     else
    {
        mresEnd = 0;
        mresStart = mresCurPos;

        // If the start is at or before the end, do nothing
        if (mresStart <= mresEnd)
            return kCIDLib::False;
    }

    // Set the search range based on the above
    ::SendMessage(hwndUs, SCI_SETTARGETSTART, mresStart, 0);
    ::SendMessage(hwndUs, SCI_SETTARGETEND, mresEnd, 0);

    //
    //  Convert the text to UTF-8 since that's what we have set up the
    //  editor for.
    //
    TUTF8Converter tcvtFind;
    THeapBuf mbufFind(strToFind.c4Length() * 2);

    tCIDLib::TCard4 c4FindBytes;
    tcvtFind.c4ConvertTo(strToFind, mbufFind, c4FindBytes);

    // And do the search
    tCIDCtrls::TMsgResult mresAt = ::SendMessage
    (
        hwndUs, SCI_SEARCHINTARGET, c4FindBytes, tCIDCtrls::TLParam(mbufFind.pc1Data())
    );

    // If nothing, then we failed
    if (mresAt == -1)
        return kCIDLib::False;

    //
    //  Llet's set the selection to the found text. If forward, leave the current
    //  position at the end, else at the beginning, so that the next find will
    //  not find the same stuff.
    //
    if (bNext)
        ::SendMessage(hwndUs, SCI_SETSEL, mresAt, mresAt + c4FindBytes);
    else
        ::SendMessage(hwndUs, SCI_SETSEL, mresAt + c4FindBytes, mresAt);

    return kCIDLib::True;
}


// Returns true if the indicated line as a disable break point
tCIDLib::TBoolean TSrcEditor::bHasDisabledBP(const tCIDLib::TCard4 c4Line) const
{
    tCIDLib::TCard4 c4Mask = ::SendMessage(hwndSafe(), SCI_MARKERGET, c4Line, 0);
    return (c4Mask & (0x1UL << CIDWUtils_SrcEditor::c4Marker_BreakPointD)) != 0;
}

// Returns true if the indicated line as an enabled break point
tCIDLib::TBoolean TSrcEditor::bHasEnabledBP(const tCIDLib::TCard4 c4Line) const
{
    tCIDLib::TCard4 c4Mask = ::SendMessage(hwndSafe(), SCI_MARKERGET, c4Line, 0);
    return (c4Mask & (0x1UL << CIDWUtils_SrcEditor::c4Marker_BreakPoint)) != 0;
}


// Indicate whether we have any text selected
tCIDLib::TBoolean TSrcEditor::bHasSelection() const
{
    return (::SendMessage(hwndSafe(), SCI_GETSELECTIONEMPTY, 0, 0) == 0);
}


// Indicate whether the passed line/col is a real position
tCIDLib::TBoolean
TSrcEditor::bIsValidPos(const   tCIDLib::TCard4 c4Line
                        , const tCIDLib::TCard4 c4Col) const
{
    return kCIDLib::True;
}


// Returns the selected text
tCIDLib::TBoolean TSrcEditor::bQuerySelectedText(TString& strToFill)
{
    // Get the size required
    tCIDCtrls::TMsgResult mresSize = ::SendMessage(hwndSafe(), SCI_GETSELTEXT, 0, 0);
    if (mresSize == 0)
    {
        strToFill.Clear();
        return kCIDLib::False;
    }

    //
    //  Make sure our string is this large, plus one for the null! And remember the
    //  editor has UTF8 text, not Unicode.
    //
    tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[mresSize + 1];
    TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Buf);

    // And now query the text
    ::SendMessage(hwndSafe(), SCI_GETSELTEXT, 0, tCIDCtrls::TLParam(pc1Buf));

    // Now convert it into our buffer
    TUTF8Converter tcvtText;
    tcvtText.c4ConvertFrom(pc1Buf, mresSize, strToFill, tCIDLib::EAppendOver::Overwrite);
    return kCIDLib::True;
}


// Get or set the read only flag
tCIDLib::TBoolean TSrcEditor::bReadOnly() const
{
    return (::SendMessage(hwndSafe(), SCI_GETREADONLY, 0, 0) != 0);
}

tCIDLib::TBoolean TSrcEditor::bReadOnly(const tCIDLib::TBoolean bToSet)
{
    //
    //  Get the current position. The read only thing seems to select the whole file and
    //  move back to the 1st line. We want to undo that, which we'll do by setting the
    //  selection back to here and zero length.
    //
    tCIDCtrls::TMsgResult mresCur = ::SendMessage(hwndSafe(),SCI_GETCURRENTPOS,0,0);
    ::SendMessage(hwndSafe(), SCI_SETREADONLY, bToSet, 0);
    ::SendMessage(hwndThis(), SCI_SETEMPTYSELECTION, mresCur, 0);
    return bToSet;
}


//
//  This will replace all instances of the to find string with the replacement text.
//  We return the number of replacements done.
//
tCIDLib::TCard4
TSrcEditor::c4Replace(const TString& strToFind, const TString& strReplaceWith)
{
    tCIDCtrls::TWndHandle hwndUs = hwndSafe();

    // Convert the two text strings
    TUTF8Converter tcvtRep;
    THeapBuf mbufFind(strToFind.c4Length() * 2);
    tCIDLib::TCard4 c4FindBytes;
    tcvtRep.c4ConvertTo(strToFind, mbufFind, c4FindBytes);

    THeapBuf mbufRep(strReplaceWith.c4Length() * 2);
    tCIDLib::TCard4 c4RepBytes;
    tcvtRep.c4ConvertTo(strReplaceWith, mbufRep, c4RepBytes);

    // Get the start end, based on selection only or full document
    tCIDCtrls::TMsgResult mresStart;
    tCIDCtrls::TMsgResult mresEnd;

    if (m_bSelOnly)
    {
        mresStart = ::SendMessage(hwndUs, SCI_GETSELECTIONSTART, 0, 0);
        mresEnd = ::SendMessage(hwndUs, SCI_GETSELECTIONEND, 0, 0);
    }
     else
    {
        mresStart = 0;
        mresEnd = ::SendMessage(hwndUs, SCI_GETLENGTH, 0, 0);
    }

    const tCIDLib::TBoolean bRegEx
    (
        (::SendMessage(hwndUs, SCI_GETSEARCHFLAGS, 0, 0) & SCFIND_REGEXP) != 0
    );

    //
    //  Do an initial search to see if there's anything to replace. We have to
    //  set the target to the current start/end values.
    //
    ::SendMessage(hwndUs, SCI_SETTARGETSTART, mresStart, 0);
    ::SendMessage(hwndUs, SCI_SETTARGETEND, mresEnd, 0);

    // We get back the position of the match, if any
    tCIDCtrls::TMsgResult mresAt = ::SendMessage
    (
        hwndUs
        , SCI_SEARCHINTARGET
        , c4FindBytes
        , tCIDCtrls::TLParam(mbufFind.pc1Data())
    );

    // If not, then we are done
    if (mresAt == -1)
        return 0;

    //
    //  We have at least one, so let's do the loop until we get a failure to
    //  find another match, or we hit the end.
    //
    tCIDLib::TCard4 c4Count = 0;
    while ((mresAt < mresEnd) && (mresAt >= 0))
    {
        c4Count++;

        // Do a replace of the correct type, regex or nromal
        tCIDCtrls::TMsgResult mresLen;
        if (bRegEx)
        {
            mresLen = ::SendMessage
            (
                hwndUs
                , SCI_REPLACETARGETRE
                , c4RepBytes
                , tCIDCtrls::TLParam(mbufRep.pc1Data())
            );
        }
         else
        {
            mresLen = ::SendMessage
            (
                hwndUs
                , SCI_REPLACETARGET
                , c4RepBytes
                , tCIDCtrls::TLParam(mbufRep.pc1Data())
            );
        }

        // Update the end again since we will have affected it
        if (m_bSelOnly)
            mresEnd = ::SendMessage(hwndUs, SCI_GETSELECTIONEND, 0, 0);
        else
            mresEnd = ::SendMessage(hwndUs, SCI_GETLENGTH, 0, 0);

        // Put the target back again, moving the start up past this find
        ::SendMessage(hwndUs, SCI_SETTARGETSTART, mresAt + mresLen, 0);
        ::SendMessage(hwndUs, SCI_SETTARGETEND, mresEnd, 0);

        // And do the next find
        mresAt = ::SendMessage
        (
            hwndUs, SCI_SEARCHINTARGET, c4FindBytes, tCIDCtrls::TLParam(mbufFind.pc1Data())
        );
    }
    return c4Count;
}


// Returns the one based current line suitable for outsider's consumption
tCIDLib::TCard4 TSrcEditor::c4CurLine() const
{
    // Get the position, and then the line that falls on
    tCIDCtrls::TMsgResult mresCur = ::SendMessage(hwndSafe(), SCI_GETCURRENTPOS, 0, 0);
    mresCur = ::SendMessage(hwndThis(), SCI_LINEFROMPOSITION, mresCur, 0);

    if (mresCur < 0)
        return 1;
    return mresCur + 1;
}


//
//  The client code should call this once it has saved changes.
//
tCIDLib::TVoid TSrcEditor::ChangesSaved()
{
    ::SendMessage(hwndSafe(), SCI_SETSAVEPOINT, 0, 0);
}


// Copy or cut the selection to the clipboard
tCIDLib::TVoid TSrcEditor::CopySelection(const tCIDLib::TBoolean bCut)
{
    if (bCut)
        ::SendMessage(hwndSafe(), SCI_CUT, 0, 0);
    else
        ::SendMessage(hwndSafe(), SCI_COPY, 0, 0);
}


//
//  Create the editor window and set it up for our needs.
//
tCIDLib::TVoid
TSrcEditor::CreateSrcEditor(const   TWindow&                wndParent
                            , const tCIDCtrls::TWndId       widToUse
                            , const TArea&                  areaInit
                            , const tCIDCtrls::EWndStyles   eWndStyles
                            , const   TString&              strPath)
{
    // Load the implementation DLL if not already done
    static tCIDLib::TBoolean bLoaded = kCIDLib::False;
    if (!bLoaded)
    {
        TBaseLock lockInit;
        if (!bLoaded)
        {
            ::LoadLibrary(L"SciLexer.dll");
            bLoaded = kCIDLib::True;
        }
    }

    // Store the provided path, if any
    m_strPath = strPath;

    //
    //  And call TWindow to do the actual creation. Make sure that clip children and tab
    //  stop styles are on. We are directly creating a Scintilla window here, so it is this
    //  window and we are subclassing it. We aren't creating a nested one or anything.
    //
    CreateWnd
    (
        wndParent.hwndSafe()
        , L"Scintilla"
        , L""
        , areaInit
        , tCIDLib::eOREnumBits(eWndStyles, tCIDCtrls::EWndStyles::ClippingTabChild)
        , tCIDCtrls::EExWndStyles::None
        , widToUse
    );

    // All standard controls must be subclassed
    SetSubclass();

    // Get out our window handle for all the setup stuff below
    tCIDCtrls::TWndHandle hwndUs = hwndThis();

    // Set the code page to UTF-8
    ::SendMessage(hwndUs, SCI_SETCODEPAGE, SC_CP_UTF8, 0);

    // Set it up to use a non-proportional font
    ::SendMessage
    (
        hwndUs, SCI_STYLESETFONT, STYLE_DEFAULT, tCIDCtrls::TLParam("Courier New")
    );

    ::SendMessage(hwndUs, SCI_STYLESETSIZE, STYLE_DEFAULT, 10);

    // Set up the markers we want to use
    ::SendMessage
    (
        hwndUs
        , SCI_MARKERDEFINE
        , CIDWUtils_SrcEditor::c4Marker_CurLine
        , SC_MARK_BACKGROUND
    );
    ::SendMessage
    (
        hwndUs
        , SCI_MARKERSETBACK
        , CIDWUtils_SrcEditor::c4Marker_CurLine
        , TRGBClr(0xEE, 0xEE, 0xEE).c4Color()
    );

    ::SendMessage
    (
        hwndUs
        , SCI_MARKERDEFINE
        , CIDWUtils_SrcEditor::c4Marker_BreakPoint
        , SC_MARK_ARROWS
    );

    ::SendMessage
    (
        hwndUs
        , SCI_MARKERDEFINE
        , CIDWUtils_SrcEditor::c4Marker_BreakPointD
        , SC_MARK_DOTDOTDOT
    );

    // Set the breakpoint/current line margin as sensitive
    ::SendMessage(hwndUs, SCI_SETMARGINSENSITIVEN, 1, 1);

    // Set the end line format to LF
    ::SendMessage(hwndUs, SCI_SETEOLMODE, 2, 0);

    // Set our tab width and indicate we want space based tabs
    ::SendMessage(hwndUs, SCI_SETTABWIDTH, 4, 0);
    ::SendMessage(hwndUs, SCI_SETUSETABS, 0, 0);

    // Set our Lexer
    ::SendMessage(hwndUs, SCI_SETLEXER, SCLEX_CML, 0);


    // And set up the colors for our styles
    ::SendMessage(hwndUs, SCI_STYLESETFORE, SCE_CML_BRACKET, 0x0000CC);
    ::SendMessage(hwndUs, SCI_STYLESETFORE, SCE_CML_COMMENT, 0xBBBB00);
    ::SendMessage(hwndUs, SCI_STYLESETFORE, SCE_CML_IDENTIFIER, 0x000000);
    ::SendMessage(hwndUs, SCI_STYLESETFORE, SCE_CML_KEYWORD, 0x777777);
    ::SendMessage(hwndUs, SCI_STYLESETFORE, SCE_CML_NUMBER, 0x00AA00);
    ::SendMessage(hwndUs, SCI_STYLESETFORE, SCE_CML_OPERATOR, 0x0000CC);
    ::SendMessage(hwndUs, SCI_STYLESETFORE, SCE_CML_STRING, 0xFF0000);
    ::SendMessage(hwndUs, SCI_STYLESETFORE, SCE_CML_TYPE, 0x00AACC);
    ::SendMessage(hwndUs, SCI_STYLESETFORE, SCE_CML_CHAR, 0xFF0000);

    ::SendMessage(hwndUs, SCI_STYLESETBOLD, SCE_CML_BRACKET, 1);
    ::SendMessage(hwndUs, SCI_STYLESETBOLD, SCE_CML_OPERATOR, 1);
    ::SendMessage(hwndUs, SCI_STYLESETBOLD, SCE_CML_TYPE, 1);

    // We only want notifications of actual text modifitions
    ::SendMessage
    (
        hwndUs, SCI_SETMODEVENTMASK, SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT, 0
    );
}


//
//  Move the passed line number so it's visible. It doesn't set the current line
//  though.
//
tCIDLib::TVoid TSrcEditor::GoToLine(const tCIDLib::TCard4 c4LineNum)
{
    ::SendMessage(hwndSafe(), SCI_GOTOLINE, int(c4LineNum), 0);
}


// We pass this through to the editor
tCIDLib::TVoid
TSrcEditor::LoadText(const TString& strPath, const TString& strToSet)
{
    //
    //  We replace all tabs with 4 spaces, because tabs mess everything up
    //  wrt to offsets vs. columns.
    //
    TString strLoad(strToSet.c4Length() * 2);;
    ExpandTabs(strToSet, strLoad);

    // We have to convert to UTF-8 to load it
    TUTF8Converter tcvtLoad;
    THeapBuf mbufText(strLoad.c4Length() * 2);

    tCIDLib::TCard4 c4Bytes;
    tcvtLoad.c4ConvertTo(strLoad, mbufText, c4Bytes);

    //
    //  It expects a null terminated string, so do that. The transcoding doesn't
    //  transcode anything but the actual text.
    //
    mbufText.PutCard1(0, c4Bytes++);
    ::SendMessage
    (
        hwndSafe(), SCI_SETTEXT, 0, tCIDCtrls::TLParam(mbufText.pc1Data())
    );

    // We don't want to be able to undo the loading of the text
    ::SendMessage(hwndSafe(), SCI_EMPTYUNDOBUFFER, 0, 0);

    // Remove any selection and move to the 0th position
    ::SendMessage(hwndThis(), SCI_GOTOPOS, 0, 0);
    ::SendMessage(hwndThis(), SCI_SETSEL, -1, 0);

    // Remember the path
    m_strPath = strPath;
}

tCIDLib::TVoid
TSrcEditor::LoadText(const TString& strPath, TTextInStream& strmSrc)
{
    //
    //  We have to read in all of the text into newline separated lines within
    //  a single string, then UTF-8 encode it. So, we create a UTF-8 formatted
    //  buffer based text outstream. We read lines from the source and write
    //  them to the target. We also expand tabs during this process.
    //
    TTextMBufOutStream strmOut(32 * 1024, 256 * 1024, new TUTF8Converter);
    strmOut.eNewLineType(tCIDLib::ENewLineTypes::LF);

    TString strCurLine;
    TString strExpLine;
    strmSrc.Reset();
    while (!strmSrc.bEndOfStream())
    {
        strmSrc >> strCurLine;
        ExpandTabs(strCurLine, strExpLine);
        strmOut << strExpLine << kCIDLib::NewLn;
    }
    strmOut.Flush();

    //
    //  Sadly it requires a null terminator, so we have to make a copy of the
    //  data purely to do that.
    //
    tCIDLib::TCard4 c4Bytes = strmOut.c4CurSize();
    THeapBuf mbufData(c4Bytes, c4Bytes + 1);
    mbufData.CopyIn(strmOut.mbufData(), c4Bytes);

    mbufData.PutCard1(0, c4Bytes++);
    ::SendMessage
    (
        hwndSafe(), SCI_SETTEXT, 0, tCIDCtrls::TLParam(mbufData.pc1Data())
    );

    // We don't want to be able to undo the loading of the text
    ::SendMessage(hwndSafe(), SCI_EMPTYUNDOBUFFER, 0, 0);

    // Remove any selection and move to the 0th position
    ::SendMessage(hwndThis(), SCI_GOTOPOS, 0, 0);
    ::SendMessage(hwndThis(), SCI_SETSEL, -1, 0);

    // Remember the path
    m_strPath = strPath;
}


// Paste any text in the clipboard into the editor
tCIDLib::TVoid TSrcEditor::Paste()
{
    ::SendMessage(hwndSafe(), SCI_PASTE, 0, 0);
}


//
//  Query all of our lines that have breakpoints. We also return a parallel
//  enabled/disable list of booleans.
//
tCIDLib::TVoid
TSrcEditor::QueryBPLines(tCIDLib::TCardList&    fcolLines
                        , tCIDLib::TBoolList&   fcolEnabled) const
{
    fcolEnabled.RemoveAll();
    fcolLines.RemoveAll();

    const tCIDLib::TCard4 c4FndMask
    (
        CIDWUtils_SrcEditor::c4Marker_BreakPoint
        | CIDWUtils_SrcEditor::c4Marker_BreakPointD
    );

    tCIDCtrls::TWndHandle hwndUs = hwndSafe();

    tCIDCtrls::TMsgResult mresLn = ::SendMessage(hwndUs, SCI_MARKERNEXT, 0, c4FndMask);
    while (mresLn != -1)
    {
        // Store the line for this one
        fcolLines.c4AddElement(tCIDLib::TCard4(mresLn));

        //
        //  Get the actual marker so we can tell if it's a enabled or disabled
        //  BP.
        //
        tCIDLib::TCard4 c4CurMask = ::SendMessage(hwndSafe(), SCI_MARKERGET, mresLn, 0);

        if (c4CurMask & (0x1UL << CIDWUtils_SrcEditor::c4Marker_BreakPoint))
            fcolEnabled.c4AddElement(kCIDLib::True);
        else
            fcolEnabled.c4AddElement(kCIDLib::False);

        // Search again, starting at the next line
        mresLn++;
        mresLn = ::SendMessage(hwndUs, SCI_MARKERNEXT, mresLn, c4FndMask);
    }
}


// Pass through to the editor
tCIDLib::TVoid
TSrcEditor::QuerySrcText(TString& strToFill, const tCIDLib::TBoolean bAppend) const
{
    tCIDCtrls::TMsgResult mresLen = ::SendMessage(hwndSafe(), SCI_GETLENGTH, 0, 0);

    if (!mresLen)
    {
        // If not appending, then clear the output string
        if (!bAppend)
            strToFill.Clear();
        return;
    }

    // Allocate a buffer for the size, bumped for the null it adds
    tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[mresLen + 1];
    TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Buf);

    // Pull it over and then we need to transcode it to the caller's buffer
    ::SendMessage(hwndSafe(), SCI_GETTEXT, mresLen + 1, tCIDCtrls::TLParam(pc1Buf));

    TUTF8Converter tcvtText;
    tcvtText.c4ConvertFrom
    (
        pc1Buf
        , mresLen
        , strToFill
        , bAppend ? tCIDLib::EAppendOver::Append
                  : tCIDLib::EAppendOver::Overwrite
    );
}


// Allow the outside world to force an Redo
tCIDLib::TVoid TSrcEditor::Redo()
{
    // If we can redo, then redi
    if (::SendMessage(hwndSafe(), SCI_CANREDO, 0, 0))
        ::SendMessage(hwndSafe(), SCI_REDO, 0, 0);
}

tCIDLib::TVoid TSrcEditor::RemoveAllBPs()
{
    const tCIDLib::TCard4 c4FndMask
    (
        CIDWUtils_SrcEditor::c4Marker_BreakPoint
        | CIDWUtils_SrcEditor::c4Marker_BreakPointD
    );

    tCIDCtrls::TWndHandle hwndUs = hwndSafe();
    tCIDCtrls::TMsgResult mresLn = ::SendMessage(hwndUs, SCI_MARKERNEXT, 0, c4FndMask);
    while (mresLn != -1)
    {
        ::SendMessage
        (
            hwndSafe()
            , SCI_MARKERDELETE
            , mresLn
            , CIDWUtils_SrcEditor::c4Marker_BreakPoint
        );

        ::SendMessage
        (
            hwndSafe()
            , SCI_MARKERDELETE
            , mresLn
            , CIDWUtils_SrcEditor::c4Marker_BreakPointD
        );

        // Search again, starting at the next line
        mresLn++;
        mresLn = ::SendMessage(hwndUs, SCI_MARKERNEXT, mresLn, c4FndMask);
    }
}


//
//  Find the line with the current line state and remove it. Actually, just in case, we
//  keep checking till we don't find one. So, if we accidentally get more than one line
//  marked, we'll remove them all.
//
tCIDLib::TVoid TSrcEditor::RemoveCurLineState()
{
    // See if a line has that marker
    tCIDCtrls::TMsgResult mresLine = -1;
    do
    {
        mresLine = ::SendMessage
        (
            hwndSafe()
            , SCI_MARKERNEXT
            , 0
            , 1UL << CIDWUtils_SrcEditor::c4Marker_CurLine
        );

        // If found, remove it
        if (mresLine >= 0)
        {
            ::SendMessage
            (
                hwndSafe()
                , SCI_MARKERDELETE
                , mresLine
                , CIDWUtils_SrcEditor::c4Marker_CurLine
            );
        }
    }   while (mresLine >= 0);
}


// Set the selection to our full content
tCIDLib::TVoid TSrcEditor::SelectAll()
{
    ::SendMessage
    (
        hwndSafe(), SCI_SETSEL, 0, ::SendMessage(hwndSafe(), SCI_GETLENGTH, 0, 0)
    );
}


// Select the current line
tCIDLib::TVoid TSrcEditor::SelectCurLine()
{
    // Get the current position
    tCIDCtrls::TMsgResult mresCur = ::SendMessage(hwndSafe(), SCI_GETCURRENTPOS, 0, 0);

    // Get the line that falls on
    mresCur = ::SendMessage(hwndThis(), SCI_LINEFROMPOSITION, mresCur, 0);

    // Get the start position of that line
    const tCIDCtrls::TMsgResult mresStart = ::SendMessage
    (
        hwndThis(), SCI_POSITIONFROMLINE, mresCur, 0
    );

    // And get the end position of that line
    const tCIDCtrls::TMsgResult mresEnd = ::SendMessage
    (
        hwndThis(), SCI_GETLINEENDPOSITION, mresCur, 0
    );

    // And now set the selection
    ::SendMessage(hwndThis(), SCI_SETSEL, mresStart, mresEnd);
}


tCIDLib::TVoid
TSrcEditor::SetBPEnable(const   tCIDLib::TCard4     c4LineNum
                        , const tCIDLib::TBoolean   bState
                        , const tCIDLib::TBoolean   bSendEvent)
{
    if (bHasEnabledBP(c4LineNum) && !bState)
    {
        // We need to move to the disabled state
        RemoveLineState(c4LineNum, tCIDWUtils::ECEdLStates::BreakPoint);
        AddLineState(c4LineNum, tCIDWUtils::ECEdLStates::DisabledBreak);
        if (bSendEvent)
            SendEdEvent(tCIDWUtils::ESEdEvents::DisableBP, c4LineNum);
    }
     else if (bHasDisabledBP(c4LineNum) && bState)
    {
        // We need to move to the enabled state
        RemoveLineState(c4LineNum, tCIDWUtils::ECEdLStates::DisabledBreak);
        AddLineState(c4LineNum, tCIDWUtils::ECEdLStates::BreakPoint);
        if (bSendEvent)
            SendEdEvent(tCIDWUtils::ESEdEvents::EnableBP, c4LineNum);
    }
}


//
//  Set the current position to a given line/column. Optionally, we can set a
//  current line state on this new line.
//
//  If the bKeepCentered flag is set, we try to keep it in the center of the window
//  vertically.
//
tCIDLib::TVoid
TSrcEditor::SetPosition(const   tCIDLib::TCard4     c4Line
                        , const tCIDLib::TCard4     c4Col
                        , const tCIDLib::TBoolean   bSetCurLine
                        , const tCIDLib::TBoolean   bKeepCentered)
{
    const tCIDLib::TCard4 c4Pos = ::SendMessage
    (
        hwndSafe(), SCI_FINDCOLUMN, c4Line, c4Col
    );
    ::SendMessage(hwndSafe(), SCI_GOTOPOS, c4Pos, 0);
    if (bSetCurLine)
        AddLineState(c4Line, tCIDWUtils::ECEdLStates::CurLine);

    if (bKeepCentered)
    {
        // Get the number of lines on the screen and the total lines.
        tCIDCtrls::TMsgResult mresScrCnt = ::SendMessage(hwndThis(), SCI_LINESONSCREEN, 0, 0);
        tCIDCtrls::TMsgResult mresDocCnt = ::SendMessage(hwndThis(), SCI_GETLINECOUNT, 0, 0);

        //
        //  If we have more document lines than screen lines, and the target line is beyond
        //  the first page's worth, we can do something
        //
        if ((mresDocCnt > mresScrCnt) && (tCIDLib::TInt4(c4Line) > mresScrCnt))
        {
            ::SendMessage
            (
                hwndThis()
                , SCI_SETFIRSTVISIBLELINE
                , c4Line - (mresScrCnt / 2)
                , 0
            );
        }
    }
}


//
//  Sets the search and replace flags. We just send the appropriate messages to
//  the editor for most of them. Some we save for later use.
//
tCIDLib::TVoid
TSrcEditor::SetSearchFlag(  const   tCIDWUtils::EFindOpts   eOpt
                            , const tCIDLib::TBoolean       bState)
{
    if (eOpt == tCIDWUtils::EFindOpts::SelOnly)
    {
        // This one we handle
        m_bSelOnly = bState;
    }
     else
    {
        tCIDCtrls::TWParam wNewFlag;
        switch(eOpt)
        {
            case tCIDWUtils::EFindOpts::Case :
                wNewFlag = SCFIND_MATCHCASE;
                break;

            case tCIDWUtils::EFindOpts::PosixRE :
                wNewFlag = SCFIND_POSIX;
                break;

            case tCIDWUtils::EFindOpts::RegEx  :
                wNewFlag = SCFIND_REGEXP;
                break;

            case tCIDWUtils::EFindOpts::WholeWords :
                wNewFlag = SCFIND_WHOLEWORD;
                break;

            default :
                break;
        };

        // Get the current flags and blt in the new one
        tCIDCtrls::TWParam wFlags = ::SendMessage(hwndSafe(), SCI_GETSEARCHFLAGS, 0, 0);
        if (bState)
            wFlags |= wNewFlag;
        else
            wFlags &= ~wNewFlag;

        ::SendMessage(hwndSafe(), SCI_SETSEARCHFLAGS, wFlags, 0);
    }
}


//
//  If there's a breakpoint at the current line, then toggle it off, else toggle
//  it on.
//
tCIDLib::TVoid TSrcEditor::ToggleBPoint()
{
    // Get the line for the current position
    const tCIDLib::TCard4 c4LineNum = ::SendMessage
    (
        hwndSafe()
        , SCI_LINEFROMPOSITION
        , ::SendMessage(hwndThis(), SCI_GETCURRENTPOS, 0, 0)
        , 0
    );

    // And call a private BP toggle helper
    DoBPToggle(c4LineNum);
}

tCIDLib::TVoid TSrcEditor::ToggleBPEnable()
{
    const tCIDLib::TCard4 c4LineNum = ::SendMessage
    (
        hwndSafe()
        , SCI_LINEFROMPOSITION
        , ::SendMessage(hwndThis(), SCI_GETCURRENTPOS, 0, 0)
        , 0
    );

    if (bHasEnabledBP(c4LineNum))
    {
        // We need to move to the disabled state
        RemoveLineState(c4LineNum, tCIDWUtils::ECEdLStates::BreakPoint);
        AddLineState(c4LineNum, tCIDWUtils::ECEdLStates::DisabledBreak);
        SendEdEvent(tCIDWUtils::ESEdEvents::DisableBP, c4LineNum);
    }
     else if (bHasDisabledBP(c4LineNum))
    {
        // We need to move to the enabled state
        RemoveLineState(c4LineNum, tCIDWUtils::ECEdLStates::DisabledBreak);
        AddLineState(c4LineNum, tCIDWUtils::ECEdLStates::BreakPoint);
        SendEdEvent(tCIDWUtils::ESEdEvents::EnableBP, c4LineNum);
    }
}


// Allow the outside world to force an Undo
tCIDLib::TVoid TSrcEditor::Undo()
{
    // If we can undo, then undo
    if (::SendMessage(hwndSafe(), SCI_CANUNDO, 0, 0))
        ::SendMessage(hwndSafe(), SCI_UNDO, 0, 0);
}


// ---------------------------------------------------------------------------
//  TSrcEditor: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  We need to override the dialog code query so that we can turn off the autoselection
//  flag. Otherwise, upon getting focus the editor will auto-select everything.
//
//  And we watch for Ctrl-C/Y/X keys and Ctrl-Y/Z for undo/redo, and call methods to do
//  those operations. Scintilla doesn't handle them itself.
//
tCIDLib::TBoolean
TSrcEditor::bEatSubClassMsg(const   tCIDCtrls::TWndMsg      wmsgMsg
                            , const tCIDCtrls::TWParam      wParam
                            , const tCIDCtrls::TLParam      lParam
                            ,       tCIDCtrls::TMsgResult&  mresRet)
{
    if (wmsgMsg == WM_GETDLGCODE)
    {
        // We have to first pass it on the underlying editor
        mresRet = mresCallSubClass(wmsgMsg, wParam, lParam);

        mresRet = tCIDCtrls::TMsgResult(mresRet & ~DLGC_HASSETSEL);
        return kCIDLib::True;
    }
     else if (wmsgMsg == WM_CHAR)
    {
        if (!(lParam & KF_UP) && !(lParam & KF_EXTENDED))
        {
            tCIDLib::TBoolean bCtrl = (::GetKeyState(VK_CONTROL) & 0x80) != 0;
            tCIDLib::TBoolean bAlt = (::GetKeyState(VK_MENU) & 0x80) != 0;
            tCIDLib::TBoolean bShift = (::GetKeyState(VK_SHIFT) & 0x80) != 0;

            tCIDLib::TCh chChar = tCIDLib::TCh(wParam);

            //
            //  If control shifted and the char is 1 to 26 then it's a control
            //  shifted A to Z, so translate to a character and watch for the
            //  ones we want.
            //
            if (bCtrl && !bAlt && !bShift && (chChar >= 1) && (chChar <= 26))
            {
                chChar = tCIDLib::TCh(kCIDLib::chLatin_a + (chChar - 1));

                if ((chChar == kCIDLib::chLatin_c) || (chChar == kCIDLib::chLatin_x))
                {
                    CopySelection(chChar == kCIDLib::chLatin_x);
                    return kCIDLib::True;
                }
                 else if (chChar == kCIDLib::chLatin_v)
                {
                    Paste();
                    return kCIDLib::True;
                }
                 else if (chChar == kCIDLib::chLatin_y)
                {
                    Redo();
                    return kCIDLib::True;
                }
                 else if (chChar == kCIDLib::chLatin_z)
                {
                    Undo();
                    return kCIDLib::True;
                }
            }
        }
    }

    return kCIDLib::False;
}


//
//  We handle some notifications from the editor, either to do something auto-
//  matically or to pass on a CIDCtrls type notification to registered handlers.
//
tCIDLib::TBoolean
TSrcEditor::bNotReflect(        TWindow&                wndTar
                        , const tCIDCtrls::TWndMsg      wmsgMsg
                        , const tCIDCtrls::TWParam      wParam
                        , const tCIDCtrls::TLParam      lParam
                        ,       tCIDCtrls::TMsgResult&  mresResult)
{
    if (wmsgMsg == WM_NOTIFY)
    {
        // Get a pointer to the notification header
        NMHDR* pNotHdr = (NMHDR*)lParam;

        // Based on the notification type, process it if we care
        if (pNotHdr->code == SCN_MARGINCLICK)
        {
            //
            //  Watch for a double click in the marker area, in which case we
            //  see if the line has a break point marker. If so, we turn it off.
            //  If it doesn't, we add one.
            //
            const SCNotification* pNotInfo = (const SCNotification*)lParam;

            // If the margin number is 1, then it's our guy
            if (pNotInfo->margin == 1)
            {
                // Position has the position, so get the line for that
                const tCIDLib::TCard4 c4LineNum = ::SendMessage
                (
                    hwndSafe(), SCI_LINEFROMPOSITION, pNotInfo->position, 0
                );

                // And call a private BP toggle helper
                DoBPToggle(c4LineNum);
            }
        }
         else if (pNotHdr->code == SCN_MODIFIED)
        {
            const SCNotification* pNotInfo = (const SCNotification*)lParam;

            if (pNotInfo->modificationType & (SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT))
            {
                // It's one we care about, so send a modified event
                TSrcEdChangeInfo wnotSend(m_strPath, pNotInfo->linesAdded, *this);
                SendSyncNotify(wnotSend, TSrcEditor::nidChange);
            }

            tCIDLib::TCard4 c4NewLine = c4CurLine();
            if(c4NewLine != m_c4LastLine)
            {
                m_c4LastLine = c4NewLine;
                TSrcEdChangeInfo wnotSend
                (
                    tCIDWUtils::ESEdEvents::LineNum,m_strPath,m_c4LastLine,*this
                );
                SendSyncNotify(wnotSend, TSrcEditor::nidChange);
            }
        }
         else if (pNotHdr->code == SCN_UPDATEUI)
        {
            //
            //  If the line number has changed since the last one we reported, let's
            //  send a notification.
            //
            tCIDLib::TCard4 c4NewLine = c4CurLine();
            if (c4NewLine != m_c4LastLine)
            {
                m_c4LastLine = c4NewLine;
                TSrcEdChangeInfo wnotSend
                (
                    tCIDWUtils::ESEdEvents::LineNum, m_strPath, m_c4LastLine, *this
                );
                SendSyncNotify(wnotSend,TSrcEditor::nidChange);
            }
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  By default, the TStdCtrlWnd class, from which we derive, overrides this and sends
//  the command on to the underlying control, so it can pop up its own menu, if any.
//  But this guy doesn't have one, it'e expected that someone else will provide it.
//  In our case we assume by default that the parent application will do this, so we
//  override this and indicate it should propogate to the parent.
//
//  In some cases a derivative of this class may in turn override this and provide
//  its own handler and return 'done'.
//
tCIDCtrls::ECtxMenuOpts
TSrcEditor::eShowContextMenu(const TPoint&, const tCIDCtrls::TWndId)
{
    return tCIDCtrls::ECtxMenuOpts::Propogate;
}


// ---------------------------------------------------------------------------
//  TSrcEditor: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Add the indicated line state to the indicated line. It's a generic helper
//  used by more specific public methods called by the outside world.
//
tCIDLib::TVoid
TSrcEditor::AddLineState(const  tCIDLib::TCard4         c4LineIndex
                        , const tCIDWUtils::ECEdLStates eToAdd)
{
    const tCIDLib::TCard4 c4MarkerInd = c4XlatMarker(eToAdd);
    if (c4MarkerInd != kCIDLib::c4MaxCard)
        ::SendMessage(hwndSafe(), SCI_MARKERADD, c4LineIndex, c4MarkerInd);
}


//
//  Translates one of our line state enum values to the correct marker index for
//  Scintilla. If we don't understand it, we return max card.
//
tCIDLib::TCard4 TSrcEditor::c4XlatMarker(const tCIDWUtils::ECEdLStates eToAdd)
{
    tCIDLib::TCard4 c4Ret = kCIDLib::c4MaxCard;
    switch(eToAdd)
    {
        case tCIDWUtils::ECEdLStates::CurLine :
            c4Ret = CIDWUtils_SrcEditor::c4Marker_CurLine;
            break;

        case tCIDWUtils::ECEdLStates::BreakPoint :
            c4Ret = CIDWUtils_SrcEditor::c4Marker_BreakPoint;
            break;

        case tCIDWUtils::ECEdLStates::DisabledBreak :
            c4Ret = CIDWUtils_SrcEditor::c4Marker_BreakPointD;
            break;

        case tCIDWUtils::ECEdLStates::None :
        default :
            break;
    };

    return c4Ret;
}


//
//  This is called from the public breakpoint toggle method, which just gets
//  the line for the current cursor position and calls us here, and from our
//  notification handler which gets gets the line for a click within the margin
//  column and calls us here.
//
tCIDLib::TVoid TSrcEditor::DoBPToggle(const tCIDLib::TCard4 c4LineNum)
{
    if (bHasEnabledBP(c4LineNum))
    {
        RemoveLineState(c4LineNum, tCIDWUtils::ECEdLStates::BreakPoint);
        SendEdEvent(tCIDWUtils::ESEdEvents::RemoveBP, c4LineNum);
    }
     else if (bHasDisabledBP(c4LineNum))
    {
        RemoveLineState(c4LineNum, tCIDWUtils::ECEdLStates::DisabledBreak);
        SendEdEvent(tCIDWUtils::ESEdEvents::RemoveBP, c4LineNum);
    }
     else
    {
        AddLineState(c4LineNum, tCIDWUtils::ECEdLStates::BreakPoint);
        SendEdEvent(tCIDWUtils::ESEdEvents::AddBP, c4LineNum);
    }
}


//
//  We expand tabs upon loading text into the editor. Replacing each tab with
//  four spaces (which is what we set up the editor to do as well.)
//
tCIDLib::TVoid
TSrcEditor::ExpandTabs(const TString& strToExp, TString& strToFill)
{
    const TString strSpaces(L"    ");

    strToFill.Clear();
    const tCIDLib::TCard4 c4SrcLen = strToExp.c4Length();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SrcLen; c4Index++)
    {
        const tCIDLib::TCh chCur = strToExp[c4Index];
        if (chCur == kCIDLib::chTab)
            strToFill.Append(strSpaces);
        else
            strToFill.Append(chCur);
    }
}


//
//  A generic method for removing a line state, called by more specialized
//  public methods.
//
tCIDLib::TVoid
TSrcEditor::RemoveLineState(const   tCIDLib::TCard4         c4LineIndex
                            , const tCIDWUtils::ECEdLStates eToRemove)
{
    const tCIDLib::TCard4 c4MarkerInd = c4XlatMarker(eToRemove);
    if (c4MarkerInd != kCIDLib::c4MaxCard)
    {
        ::SendMessage(hwndSafe(), SCI_MARKERDELETE, c4LineIndex, c4MarkerInd);
    }
}


//
//  A helper for derived classes to call when they want to send out a list
//  event. These are al done synchronously.
//
tCIDLib::TVoid
TSrcEditor::SendEdEvent(const   tCIDWUtils::ESEdEvents  eEvent
                        , const tCIDLib::TCard4         c4LineNum)
{
    TSrcEdChangeInfo wnotSend(eEvent, m_strPath, c4LineNum, *this);
    SendSyncNotify(wnotSend, TSrcEditor::nidChange);
}



