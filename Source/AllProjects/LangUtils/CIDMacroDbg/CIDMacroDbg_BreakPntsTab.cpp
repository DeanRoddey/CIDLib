//
// FILE NAME: CIDMacroDbg_BreakPntsTab.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/29/2015
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
//  This file implements the break points tab window.
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
RTTIDecls(TBreakPntsTab, TTabWindow)



// ---------------------------------------------------------------------------
//   CLASS: TBreakPntsTab
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBreakPntsTab: Constructors and Destructor
// ---------------------------------------------------------------------------
TBreakPntsTab::TBreakPntsTab() :

    TTabWindow(kCIDMacroDbg_::pszTab_BreakPoints, L"", kCIDLib::False, kCIDLib::True)
    , m_c4NextId(0)
    , m_pwndDel(nullptr)
    , m_pwndDelAll(nullptr)
    , m_pwndDelSrc(nullptr)
    , m_pwndList(nullptr)
    , m_pwndMain(nullptr)
{
}

TBreakPntsTab::~TBreakPntsTab()
{
}


// ---------------------------------------------------------------------------
//  TBreakPntsTab: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TBreakPntsTab::AddBreakPoint(const  TSrcTab&            wndSrcTab
                            , const tCIDLib::TCard4     c4LineNum
                            , const tCIDLib::TBoolean   bEnabled)
{
    //
    //  Search the list to see if this one is already there. If so, that's wrong
    //  so log it, but we'll just make sure it's set to the right enable/disabled
    //  state.
    //
    TString strCurPath;
    TString strCurLine;

    tCIDLib::TCard4 c4Index = 0;
    const tCIDLib::TCard4 c4Count = m_pwndList->c4ItemCount();
    for (; c4Index < c4Count; c4Index++)
    {
        // Get the 3rd column which is the class path
        m_pwndList->QueryColText(c4Index, 2, strCurPath);

        if (strCurPath.bCompare(wndSrcTab.strClassPath()))
        {
            // It's the right class, check the line number
            m_pwndList->QueryColText(c4Index, 1, strCurLine);

            const tCIDLib::TCard4 c4CurLn = strCurLine.c4Val();
            if (c4CurLn == c4LineNum)
            {
                #if CID_DEBUG_ON
                TOkBox msgbErr
                (
                    L"Break Point Tab"
                    , L"The indicated break point is already in the BP tab"
                );
                msgbErr.ShowIt(*this);
                #endif
                return;
            }

            //
            //  If the current line is greater than our line, then we want to
            //  insert here.
            //
            if (c4CurLn > c4LineNum)
                break;
        }
    }

    //
    //  Let's insert the new one here. The numbers or zero based so bump it up
    //  to one based for display.
    //
    tCIDLib::TStrList colVals(3);
    colVals.objAdd(TString::strEmpty());
    colVals.objAdd(TCardinal(c4LineNum + 1));
    colVals.objAdd(wndSrcTab.strClassPath());
    const tCIDLib::TCard4 c4At = m_pwndList->c4AddItem(colVals, ++m_c4NextId, c4Index);
    m_pwndList->SetCheckAt(c4At, bEnabled);
}


tCIDLib::TVoid
TBreakPntsTab::CreateBreakPntsTab(  TTabbedWnd&                 wndParent
                                    , TMacroDbgMainWnd* const   pwndMain)
{
    // Save the main window for later use
    m_pwndMain = pwndMain;
    wndParent.c4CreateTab(this, facCIDMacroDbg().strMsg(kMDbgMsgs::midTab_BreakPnts));
}


tCIDLib::TVoid
TBreakPntsTab::EnableBreakPoint(const   TSrcTab&            wndSrcTab
                                , const tCIDLib::TCard4     c4LineNum
                                , const tCIDLib::TBoolean   bNewState)
{
    // Find this one in the list
    TString strCurPath;
    TString strCurLine;

    tCIDLib::TCard4 c4Index = 0;
    tCIDLib::TCard4 c4Count = m_pwndList->c4ItemCount();
    while (c4Index < c4Count)
    {
        // Get the 3rd column which is the class path
        m_pwndList->QueryColText(c4Index, 2, strCurPath);

        if (strCurPath.bCompare(wndSrcTab.strClassPath()))
        {
            m_pwndList->QueryColText(c4Index, 1, strCurLine);
            const tCIDLib::TCard4 c4CurLn = strCurLine.c4Val() - 1;
            if (c4CurLn == c4LineNum)
            {
                m_pwndList->SetCheckAt(c4Index, bNewState);
                break;
            }
        }
        c4Index++;
    }
}


tCIDLib::TVoid TBreakPntsTab::RemoveAll()
{
    m_pwndList->RemoveAll();
}


//
//  Remove a specific break point in a particular source tab, or all of them if
//  the line number is max card.
//
tCIDLib::TVoid
TBreakPntsTab::RemoveBreakPoint(const  TSrcTab&         wndSrcTab
                                , const tCIDLib::TCard4 c4LineNum)
{
    TString strCurPath;
    TString strCurLine;

    tCIDLib::TCard4 c4Index = 0;
    tCIDLib::TCard4 c4Count = m_pwndList->c4ItemCount();
    while (c4Index < c4Count)
    {
        // Get the 3rd column which is the class path
        m_pwndList->QueryColText(c4Index, 2, strCurPath);

        if (strCurPath.bCompare(wndSrcTab.strClassPath()))
        {
            //
            //  It's the right class. If linenum is max card, then remove it
            //  always and reduce the count. Else check the line number.
            //
            if (c4LineNum == kCIDLib::c4MaxCard)
            {
                m_pwndList->RemoveAt(c4Index);
                c4Count--;
            }
             else
            {
                //
                //  We are removing a specific one, so get the line number from
                //  this one to compare to the line number. Sub one from the
                //  list item version since we display 1 based lines numbers but
                //  they are really zero based.
                //
                m_pwndList->QueryColText(c4Index, 1, strCurLine);
                const tCIDLib::TCard4 c4CurLn = strCurLine.c4Val() - 1;
                if (c4CurLn == c4LineNum)
                {
                    // It's our guy, so remove and break out
                    m_pwndList->RemoveAt(c4Index);
                    break;
                }

                // Not our line, so move forward
                c4Index++;
            }
        }
         else
        {
            // Not our class, so move forward
            c4Index++;
        }
    }
}


tCIDLib::TVoid TBreakPntsTab::Reset()
{
    // Clear the list if it exists
    if (m_pwndList)
        m_pwndList->RemoveAll();
}


//
//  The file has been modified, which means that the break points may have
//  moved or been removed, so we have to run through and update them all. We
//  just find each breakpoint in the source tab, and then run through the
//  ones for that class path in our list. We set each one to the line we found.
//  If we run out of BPs in this list first we start adding more. Else, we start
//  removing them until we hit a different class path.
//
tCIDLib::TVoid TBreakPntsTab::UpdateBreakPoints(const TSrcTab& wndSrcTab)
{
    // Query all of the breakpoints for this source
    tCIDLib::TCardList  fcolLines;
    tCIDLib::TBoolList  fcolEnabled;
    wndSrcTab.pwndEditor()->QueryBPLines(fcolLines, fcolEnabled);

    // Find the first line for this class path
    TString strCurPath;
    TString strCurLine;

    tCIDLib::TCard4 c4TarIndex = 0;
    tCIDLib::TCard4 c4TarCount = m_pwndList->c4ItemCount();

    tCIDLib::TCard4 c4SrcIndex = 0;
    tCIDLib::TCard4 c4SrcCount = fcolLines.c4ElemCount();

    // Count how many
    tCIDLib::TCard4 c4FoundCnt = 0;
    TString strLine;
    while ((c4TarIndex < c4TarCount) && (c4SrcIndex < c4SrcCount))
    {
        // Get the 3rd column which is the class path
        m_pwndList->QueryColText(c4TarIndex, 2, strCurPath);
        if (strCurPath.bCompare(wndSrcTab.strClassPath()))
        {
            c4FoundCnt++;

            // Take one of the source items and update this one with his info
            m_pwndList->SetCheckAt(c4TarIndex, fcolEnabled[c4SrcIndex]);

            // Make the line number 1 based for display
            strLine.SetFormatted(fcolLines[c4SrcIndex] + 1);
            m_pwndList->SetColText(c4TarIndex, 1, strLine);

            // We ate a source item
            c4SrcIndex++;
        }
         else if (c4FoundCnt)
        {
            //
            //  If it's not our class, we have seen previous ones that are
            //  we've obviously moved past our class path.
            //
            break;
        }

        // Move to the next target
        c4TarIndex++;
    }


    if (c4TarIndex < c4TarCount)
    {
        //
        //  We potentially have more items for this guy in our list than are
        //  now in the source, so as long as we are seeing the passed class
        //  path, remove items.
        //
        while (c4TarIndex < c4TarCount)
        {
            m_pwndList->QueryColText(c4TarIndex, 2, strCurPath);
            if (!strCurPath.bCompare(wndSrcTab.strClassPath()))
                break;
            m_pwndList->RemoveAt(c4TarIndex);
            c4TarCount--;
        }
    }
     else if (c4SrcIndex < c4SrcCount)
    {
        //
        //  The source tab now has more break points than we have in the list,
        //  so we need to add the rest
        //
        tCIDLib::TStrList colVals(3);
        colVals.objAdd(TString::strEmpty());
        colVals.objAdd(TString::strEmpty());
        colVals.objAdd(TString::strEmpty());

        while (c4SrcIndex < c4SrcCount)
        {
            // Add one to the line number for 1 based display
            strLine.SetFormatted(fcolLines[c4SrcIndex] + 1);
            colVals[1] = strLine;
            colVals[2] = wndSrcTab.strClassPath();

            // Create an id the same as we do
            const tCIDLib::TCard4 c4At = m_pwndList->c4AddItem
            (
                colVals, ++m_c4NextId, c4SrcIndex
            );
            m_pwndList->SetCheckAt(c4At, fcolEnabled[c4SrcIndex]);

            c4SrcCount++;
        }
    }
}


// ---------------------------------------------------------------------------
//  TBreakPntsTab: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TBreakPntsTab::AreaChanged( const   TArea&                  areaPrev
                            , const TArea&                  areaNew
                            , const tCIDCtrls::EPosStates   ePosState
                            , const tCIDLib::TBoolean       bOrgChanged
                            , const tCIDLib::TBoolean       bSizeChanged
                            , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    // Just keep the list window sized to fit our client area
    if (m_pwndList && bSizeChanged && (ePosState != tCIDCtrls::EPosStates::Minimized))
        AutoAdjustChildren(areaPrev, areaNew);
}


tCIDLib::TBoolean TBreakPntsTab::bCreated()
{
    TParent::bCreated();

    // Load our dialog resource and create our controls
    TDlgDesc dlgdBP;
    facCIDMacroDbg().bCreateDlgDesc(kCIDMacroDbg::ridDlg_BPTab, dlgdBP);
    tCIDLib::TCard4 c4InitFocus;
    CreateDlgItems(dlgdBP, c4InitFocus);

    // Do an initial auto-size to get it fitted to us
    AutoAdjustChildren(dlgdBP.areaPos(), areaClient());

    // Look up our controls
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_BPTab_Del, m_pwndDel);
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_BPTab_DelAll, m_pwndDelAll);
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_BPTab_DelSrc, m_pwndDelSrc);
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_BPTab_List, m_pwndList);

    // Set the columns on the list window
    tCIDLib::TStrList colTitles(3);
    colTitles.objAdd(facCIDMacroDbg().strMsg(kMDbgMsgs::midCol_Enabled));
    colTitles.objAdd(facCIDMacroDbg().strMsg(kMDbgMsgs::midCol_Line));
    colTitles.objAdd(facCIDMacroDbg().strMsg(kMDbgMsgs::midCol_ClassPath));
    m_pwndList->SetColumns(colTitles);

    // Size the first column to the title
    m_pwndList->AutoSizeCol(0, kCIDLib::True);

    // Set up an event handler for it
    m_pwndDel->pnothRegisterHandler(this, &TBreakPntsTab::eClickHandler);
    m_pwndDelAll->pnothRegisterHandler(this,&TBreakPntsTab::eClickHandler);
    m_pwndDelSrc->pnothRegisterHandler(this,&TBreakPntsTab::eClickHandler);
    m_pwndList->pnothRegisterHandler(this, &TBreakPntsTab::eLBHandler);

    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TBreakPntsTab: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDCtrls::EEvResponses TBreakPntsTab::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDMacroDbg::ridDlg_BPTab_Del)
    {
        const tCIDLib::TCard4 c4Index = m_pwndList->c4CurItem();
        if (c4Index != kCIDLib::c4MaxCard)
        {
            // Get the class path of this one
            TString strTarPath;
            m_pwndList->QueryColText(c4Index, 2, strTarPath);

            // And the line number
            TString strLine;
            m_pwndList->QueryColText(c4Index, 1, strLine);
            tCIDLib::TCard4 c4LineNum;
            if (!strLine.bToCard4(c4LineNum, tCIDLib::ERadices::Dec) || (c4LineNum == 0))
            {
                // Shouldn't happen
                return tCIDCtrls::EEvResponses::Handled;
            }

            // Has to be zero based
            c4LineNum--;

            // Remove from the list and then from the window
            m_pwndList->RemoveAt(c4Index);
            m_pwndMain->AddRemoveBP(strTarPath, c4LineNum, kCIDLib::False);
        }
    }
     else if (wnotEvent.widSource() == kCIDMacroDbg::ridDlg_BPTab_DelAll)
    {
        // Remove all our list items
        m_pwndList->RemoveAll();

        // Tel the main window to remove all open source BPs
        m_pwndMain->RemoveAllBPs();
    }
     else if (wnotEvent.widSource() == kCIDMacroDbg::ridDlg_BPTab_DelSrc)
    {
        tCIDLib::TCard4 c4Index = m_pwndList->c4CurItem();
        if (c4Index != kCIDLib::c4MaxCard)
        {
            // Get the class path of this one
            TString strTarPath;
            m_pwndList->QueryColText(c4Index, 2, strTarPath);

            TWndPaintJanitor janPaint(m_pwndList);

            // Go ahead and get rid of this one
            m_pwndList->RemoveAt(c4Index);

            // Now loop through and get rid of any breakpoints associated with this class
            TString strCurPath;
            tCIDLib::TCard4 c4Count = m_pwndList->c4ItemCount();
            while (c4Index < c4Count)
            {
                m_pwndList->QueryColText(c4Index, 2, strCurPath);
                if (strCurPath == strTarPath)
                {
                    m_pwndList->RemoveAt(c4Index);
                    c4Count--;
                }
                 else
                {
                    c4Index++;
                }
            }

            m_pwndMain->RemoveAllSrcBPs(strTarPath);
        }
    }
    return tCIDCtrls::EEvResponses::Handled;
}


tCIDCtrls::EEvResponses TBreakPntsTab::eLBHandler(TListChangeInfo& wnotEvent)
{
    if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::Invoked)
    {
        // Let's pass it on the main window, who will move to this location.
        TString strClass;
        TString strLine;
        m_pwndList->QueryColText(wnotEvent.c4Index(), 1, strLine);
        m_pwndList->QueryColText(wnotEvent.c4Index(), 2, strClass);
        tCIDLib::TCard4 c4LineNum;
        if (!strLine.bToCard4(c4LineNum, tCIDLib::ERadices::Dec) || (c4LineNum == 0))
        {
            // Shouldn't happen
            return tCIDCtrls::EEvResponses::Handled;
        }

        // Needs to be zero based
        c4LineNum--;

        //
        //  If they clicked on the first column we toggle the enable state, else we
        //  go to that position.
        //
        if (wnotEvent.c4ColIndex() == 0)
        {
            // Toggle the enable state
            tCIDLib::TBoolean bState = m_pwndList->bIsCheckedAt(wnotEvent.c4Index());
            bState = !bState;

            // Update the check state on the list window
            m_pwndList->SetCheckAt(wnotEvent.c4Index(), bState);

            // And update the window
            m_pwndMain->EnableBP(strClass, c4LineNum, bState);
        }
         else
        {
            tCIDLib::TCard4 c4LineNum;
            if (strLine.bToCard4(c4LineNum, tCIDLib::ERadices::Dec) && (c4LineNum > 0))
                m_pwndMain->Goto(strClass, c4LineNum);
        }
    }
    return tCIDCtrls::EEvResponses::Handled;
}

