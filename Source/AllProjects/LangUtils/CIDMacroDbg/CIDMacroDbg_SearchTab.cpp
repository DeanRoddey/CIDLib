//
// FILE NAME: CIDMacroDbg_SearchTab.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/11/2015
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
//  This file implements the search and replace tab.
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
RTTIDecls(TSearchTab, TTabWindow)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDMacroDbg_SearchTab
{
    // Faux menu items for use with our accel table
    const tCIDLib::TCard4   c4Menu_FindNext = 1000;
}


// ---------------------------------------------------------------------------
//   CLASS: TSearchTab
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSearchTab: Constructors and Destructor
// ---------------------------------------------------------------------------

//
//  We get a pointer to the sources tabbed window. Tell our parent class that
//  want dialog processing on this one since we have direct child windows.
//
TSearchTab::TSearchTab() :

    TTabWindow(kCIDMacroDbg_::pszTab_SearchTab, L"", kCIDLib::False, kCIDLib::True)
    , m_pwndCase(nullptr)
    , m_pwndFindNext(nullptr)
    , m_pwndFindPrev(nullptr)
    , m_pwndMainDbg(nullptr)
    , m_pwndPosix(nullptr)
    , m_pwndRegEx(nullptr)
    , m_pwndReplace(nullptr)
    , m_pwndRepText(nullptr)
    , m_pwndSelOnly(nullptr)
    , m_pwndToFind(nullptr)
    , m_pwndWholeWords(nullptr)
{
}

TSearchTab::~TSearchTab()
{
}


// ---------------------------------------------------------------------------
//  TSearchTab: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  The main debugger window will pass on accellerator processing calls to the
//  active source editor. Currently we aren't processing any here, so we jut
//  return false.
//
tCIDLib::TBoolean TSearchTab::bProcessAccel(const tCIDLib::TVoid* const pMsgData) const
{
    return m_accelSearch.bProcessAccel(*this, pMsgData);
}


// ---------------------------------------------------------------------------
//  TSearchTab: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TSearchTab::CreateSearchTab(TTabbedWnd&                 wndParent
                            , TMacroDbgMainWnd* const   pwndMainDbg)
{
    // Save incoming info for later use
    m_pwndMainDbg = pwndMainDbg;

    // Ask our parent to create us a tab
    wndParent.c4CreateTab(this, facCIDMacroDbg().strMsg(kMDbgMsgs::midTab_Search));
}


//
//  This is to support a Find hot key. The main window brings us forward and calls us
//  here. We get the currently selected text in the current source tab. If none, we don't
//  do anything. If we get some, we set it in the search window. It is already selected
//  so we don't need to do an initial search.
//
tCIDLib::TVoid TSearchTab::Find()
{
    // Get the current source tab. If none, nothing to do
    TSrcTab* pwndCur = m_pwndMainDbg->pwndCurSrcTab();
    if (!pwndCur)
        return;

    // And tell it to present any currently selected text as the search text
    TString strSearchText;
    pwndCur->pwndEditor()->bQuerySelectedText(strSearchText);

    if (!strSearchText.bIsEmpty())
        m_pwndToFind->strWndText(strSearchText);

    // Move the focus to the find window
    m_pwndToFind->TakeFocus();
}


//
//  This is to support a 'find next' hot key, as opposed to the one we invoke here
//  interally via the find next button.
//
tCIDLib::TVoid TSearchTab::FindNext()
{
    // Get the current source tab. If none, nothing to do
    TSrcTab* pwndCur = m_pwndMainDbg->pwndCurSrcTab();
    if (!pwndCur)
        return;

    // Get the find text. If empty, nothing to do
    TString strFind = m_pwndToFind->strWndText();
    if (strFind.bIsEmpty())
        return;

    SetSearchFlags(*pwndCur->pwndEditor());
    const tCIDLib::TBoolean bRes = pwndCur->pwndEditor()->bFind
    (
        strFind, kCIDLib::True
    );

    if (!bRes)
    {
        TOkBox msgbNotFound
        (
            facCIDMacroDbg().strMsg(kMDbgMsgs::midDlg_Search_Title)
            , facCIDMacroDbg().strMsg(kMDbgMsgs::midStatus_NotFound)
        );
        msgbNotFound.ShowIt(*pwndCur->pwndEditor());
    }
}


//
//  The main window tells us when it's save to do searches. It's disabled while
//  the debugger is actively running, and of course when there's no session open
//  it makes no sense.
//
tCIDLib::TVoid TSearchTab::SetState(const tCIDLib::TBoolean bCanSearch)
{
    m_pwndFindNext->SetEnable(bCanSearch);
    m_pwndFindPrev->SetEnable(bCanSearch);
    m_pwndReplace->SetEnable(bCanSearch);
}


// ---------------------------------------------------------------------------
//  TSearchTab: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TSearchTab::bCreated()
{
    TParent::bCreated();

    // Load the dialog resource that defines our content
    TDlgDesc dlgdSearch;
    TArea areaInit;
    if (!facCIDMacroDbg().bCreateDlgDesc(kCIDMacroDbg::ridDlg_Search, dlgdSearch))
        return kCIDLib::False;

    // And create the content
    tCIDLib::TCard4 c4InitFocus;
    CreateDlgItems(dlgdSearch, c4InitFocus);

    // Get pointers to the widgets we need to interact with
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_Search_Case, m_pwndCase);
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_Search_ToFind, m_pwndToFind);
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_Search_Posix, m_pwndPosix);
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_Search_RegEx, m_pwndRegEx);
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_Search_RepText, m_pwndRepText);
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_Search_FindPrev, m_pwndFindPrev);
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_Search_FindNext, m_pwndFindNext);
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_Search_Replace, m_pwndReplace);
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_Search_SelOnly, m_pwndSelOnly);
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_Search_WholeWords, m_pwndWholeWords);

    // Wire up event handlers
    m_pwndFindNext->pnothRegisterHandler(this, &TSearchTab::eClickHandler);
    m_pwndFindPrev->pnothRegisterHandler(this, &TSearchTab::eClickHandler);
    m_pwndReplace->pnothRegisterHandler(this, &TSearchTab::eClickHandler);

    //
    //  Initially disable the find/replace buttons, since initially there will
    //  be no session open.
    //
    SetState(kCIDLib::False);

    //
    //  Since the controls are directly in our tab window, we need to set our
    //  background color.
    //
    SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::Window));

    // Set up our accelerator table
    tCIDLib::TStrList colKeyStrs;
    tCIDLib::TCardList fcolCmds;

    colKeyStrs.objAdd(L"Enter");
    fcolCmds.c4AddElement(CIDMacroDbg_SearchTab::c4Menu_FindNext);
    m_accelSearch.Create(colKeyStrs, fcolCmds);

    // Put the focus initially on the to find text
    m_pwndToFind->TakeFocus();

    return kCIDLib::True;
}


tCIDLib::TVoid
TSearchTab::MenuCommand(const   tCIDLib::TResId     ridItem
                        , const tCIDLib::TBoolean   bChecked
                        , const tCIDLib::TBoolean   bEnabled)
{
    if (ridItem == CIDMacroDbg_SearchTab::c4Menu_FindNext)
    {
        // Fake a call to our click handler
        TButtClickInfo wnotEvent(kCIDLib::True, *m_pwndFindNext);
        eClickHandler(wnotEvent);
    }
}


// ---------------------------------------------------------------------------
//  TSearchTab: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TSearchTab::eClickHandler(TButtClickInfo& wnotEvent)
{
    // Get the current tab, if any. If none, do nothing
    TSrcTab* pwndCur = m_pwndMainDbg->pwndCurSrcTab();
    if (pwndCur)
    {
        // Get the search and replace strings
        TString strFind = m_pwndToFind->strWndText();
        const TString& strRepWith = m_pwndRepText->strWndText();

        // If no find text, then nothing to do. Replace can be empty
        if (!strFind.bIsEmpty())
        {
            // Check for some special characters
            tCIDLib::TCard4 c4At = 0;
            strFind.bReplaceSubStr(L"\\t", L"\t", c4At, kCIDLib::True);

            if ((wnotEvent.widSource() == kCIDMacroDbg::ridDlg_Search_FindNext)
            ||  (wnotEvent.widSource() == kCIDMacroDbg::ridDlg_Search_FindPrev))
            {
                SetSearchFlags(*pwndCur->pwndEditor());
                const tCIDLib::TBoolean bRes = pwndCur->pwndEditor()->bFind
                (
                    strFind
                    , wnotEvent.widSource() == kCIDMacroDbg::ridDlg_Search_FindNext
                );

                if (bRes)
                {
                    // Store the find text in the previous values list
                    StoreRecall(strFind, kCIDLib::True);
                }
                 else
                {
                    TOkBox msgbNotFound
                    (
                        facCIDMacroDbg().strMsg(kMDbgMsgs::midDlg_Search_Title)
                        , facCIDMacroDbg().strMsg(kMDbgMsgs::midStatus_NotFound)
                    );
                    msgbNotFound.ShowIt(*pwndCur->pwndEditor());
                }
            }
             else if (wnotEvent.widSource() == kCIDMacroDbg::ridDlg_Search_Replace)
            {
                SetSearchFlags(*pwndCur->pwndEditor());
                const tCIDLib::TCard4 c4Count = pwndCur->pwndEditor()->c4Replace
                (
                    strFind, strRepWith
                );

                if (c4Count)
                {
                    TOkBox msgbDone
                    (
                        facCIDMacroDbg().strMsg(kMDbgMsgs::midDlg_Search_Title)
                        , facCIDMacroDbg().strMsg
                          (
                            kMDbgMsgs::midStatus_Replaced, TCardinal(c4Count)
                          )
                    );
                    msgbDone.ShowIt(*pwndCur->pwndEditor());

                    // Store both strings for recall
                    StoreRecall(strFind, kCIDLib::True);
                    StoreRecall(strRepWith, kCIDLib::False);
                }
                 else
                {
                    TOkBox msgbNotFound
                    (
                        facCIDMacroDbg().strMsg(kMDbgMsgs::midDlg_Search_Title)
                        , facCIDMacroDbg().strMsg(kMDbgMsgs::midStatus_NotFound)
                    );
                    msgbNotFound.ShowIt(*pwndCur->pwndEditor());
                }
            }
        }
    }
    return tCIDCtrls::EEvResponses::Handled;
}


// Make sure the search flags on the target editor match our settings
tCIDLib::TVoid TSearchTab::SetSearchFlags(TSrcEditor& wndTar)
{
    wndTar.SetSearchFlag(tCIDWUtils::EFindOpts::Case, m_pwndCase->bCheckedState());
    wndTar.SetSearchFlag(tCIDWUtils::EFindOpts::PosixRE, m_pwndPosix->bCheckedState());
    wndTar.SetSearchFlag(tCIDWUtils::EFindOpts::RegEx, m_pwndRegEx->bCheckedState());
    wndTar.SetSearchFlag(tCIDWUtils::EFindOpts::SelOnly, m_pwndSelOnly->bCheckedState());
    wndTar.SetSearchFlag(tCIDWUtils::EFindOpts::WholeWords, m_pwndWholeWords->bCheckedState());
}


//
//  Updates our recall values for find and replace with text values.
//
tCIDLib::TVoid
TSearchTab::StoreRecall(const TString& strStore, const tCIDLib::TBoolean bFind)
{
    // Get the correct combo box
    TComboBox* pwndTar = bFind ? m_pwndToFind : m_pwndRepText;

    //
    //  If the value is already in the list, we just want to move it to the top
    //  of the list. Else we need to add it at the top, removing the one from
    //  the bottom.
    //
    tCIDLib::TCard4 c4At = pwndTar->c4FindByText(strStore);

    if (c4At == kCIDLib::c4MaxCard)
    {
        // Need to add it. Remove the last one if needed
        if (pwndTar->c4ItemCount() >= 10)
            pwndTar->RemoveAt(9);

        pwndTar->InsertItem(0, strStore);
    }
     else
    {
        // Remove it from it's current pos if not zero, and put it back at the top
        if (c4At)
        {
            pwndTar->RemoveAt(c4At);
            pwndTar->InsertItem(0, strStore);

            // And re-select it at the new point
            pwndTar->SelectByIndex(0);
        }
    }
}

