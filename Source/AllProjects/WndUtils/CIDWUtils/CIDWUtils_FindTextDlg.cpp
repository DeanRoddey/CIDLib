//
// FILE NAME: CIDWUtils_FindTextDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/28/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  Implements the TFindTextDlg dialog.
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
#include    "CIDWUtils_FindTextDlg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFindTextDlg, TDlgBox)



// ---------------------------------------------------------------------------
//   CLASS: TFindTextDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFindTextDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TFindTextDlg::TFindTextDlg() :

    m_bFullMatchOnly(kCIDLib::False)
    , m_bRepMode(kCIDLib::False)
    , m_bSelectionOnly(kCIDLib::False)
    , m_eFlags(tCIDWUtils::EFindFlags::None)
    , m_pcolFindRecall(nullptr)
    , m_pcolRepRecall(nullptr)
    , m_pwndClose(nullptr)
    , m_pwndFind(nullptr)
    , m_pwndFullMatch(nullptr)
    , m_pwndFindList(nullptr)
    , m_pwndRepList(nullptr)
    , m_pwndSelection(nullptr)
{
}

TFindTextDlg::~TFindTextDlg()
{
}


// ---------------------------------------------------------------------------
//  TFindTextDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TFindTextDlg::bRun( const   TWindow&                wndOwner
                    , const tCIDLib::TBoolean       bReplaceMode
                    ,       TString&                strToFind
                    ,       TString&                strRepWith
                    ,       tCIDLib::TStrList&      colFindRecall
                    ,       tCIDLib::TStrList&      colRepRecall
                    ,       tCIDLib::TBoolean&      bFullMatchOnly
                    ,       tCIDLib::TBoolean&      bSelectionOnly
                    , const tCIDWUtils::EFindFlags  eFlags)
{
    // Store the info until we can use it
    m_bFullMatchOnly = bFullMatchOnly;
    m_bRepMode = bReplaceMode;
    m_bSelectionOnly = bSelectionOnly;
    m_eFlags = eFlags;
    m_pcolFindRecall = &colFindRecall;
    m_pcolRepRecall = &colRepRecall;
    m_strRepWith = strRepWith;
    m_strToFind = strToFind;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_Search
    );

    if (c4Res == kCIDWUtils::ridDlg_Search_Find)
    {
        bFullMatchOnly = m_bFullMatchOnly;
        bSelectionOnly = m_bSelectionOnly;
        strRepWith = m_strRepWith;
        strToFind = m_strToFind;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TFindTextDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFindTextDlg::bCreated()
{
    // Now call our parent first and return his return
    const tCIDLib::TBoolean bRes = TParent::bCreated();

    // Get some typed pointers to some of our widgets, for convenience
    CastChildWnd(*this, kCIDWUtils::ridDlg_Search_Close, m_pwndClose);
    CastChildWnd(*this, kCIDWUtils::ridDlg_Search_Find, m_pwndFind);
    CastChildWnd(*this, kCIDWUtils::ridDlg_Search_FullMatch, m_pwndFullMatch);
    CastChildWnd(*this, kCIDWUtils::ridDlg_Search_FindList, m_pwndFindList);
    CastChildWnd(*this, kCIDWUtils::ridDlg_Search_RepList, m_pwndRepList);
    CastChildWnd(*this, kCIDWUtils::ridDlg_Search_SelOnly, m_pwndSelection);

    // Load the incoming recall lists into the combo boxs
    tCIDLib::TCard4 c4Count = m_pcolFindRecall->c4ElemCount();
    if (c4Count)
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TString& strCur = m_pcolFindRecall->objAt(c4Index);
            m_pwndFindList->c4AddItem(strCur, c4Index);
        }

        if (!m_strToFind.bIsEmpty())
        {
            tCIDLib::TCard4 c4At;
            if (!m_pwndFindList->bSelectByText(m_strToFind, c4At))
                m_pwndFindList->SelectByIndex(0);
        }
    }

    // If in replace mode, set up the replace list, else disable it
    if (m_bRepMode)
    {
        c4Count = m_pcolRepRecall->c4ElemCount();
        if (c4Count)
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                const TString& strCur = m_pcolRepRecall->objAt(c4Index);
                m_pwndRepList->c4AddItem(strCur, c4Index);
            }

            if (!m_strToFind.bIsEmpty())
            {
                tCIDLib::TCard4 c4At;
                if (!m_pwndRepList->bSelectByText(m_strRepWith, c4At))
                    m_pwndRepList->SelectByIndex(0);
            }
        }

        // Also update the text of the Find button
        m_pwndFind->strWndText(facCIDWUtils().strMsg(kWUtilMsgs::midDlg_Search_Replace));
    }
     else
    {
        m_pwndRepList->SetEnable(kCIDLib::False);
    }

    //
    //  Set or disable the options check boxes.
    //
    if (tCIDLib::bAllBitsOn(m_eFlags, tCIDWUtils::EFindFlags::CanFullOnly))
    {
        m_pwndFullMatch->SetCheckedState(m_bFullMatchOnly);
    }
     else
    {
        m_bFullMatchOnly = kCIDLib::True;
        m_pwndFullMatch->SetEnable(kCIDLib::False);
    }

    if (tCIDLib::bAllBitsOn(m_eFlags, tCIDWUtils::EFindFlags::CanSearchSel))
    {
        m_pwndSelection->SetCheckedState(m_bSelectionOnly);
    }
     else
    {
        m_bSelectionOnly = kCIDLib::False;
        m_pwndSelection->SetEnable(kCIDLib::False);
    }

    // Register our handlers
    m_pwndClose->pnothRegisterHandler(this, &TFindTextDlg::eClickHandler);
    m_pwndFind->pnothRegisterHandler(this, &TFindTextDlg::eClickHandler);

    return bRes;
}



// ---------------------------------------------------------------------------
//  TFindTextDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TFindTextDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_Search_Find)
    {
        m_bFullMatchOnly = m_pwndFullMatch->bCheckedState();
        m_bSelectionOnly = m_pwndSelection->bCheckedState();

        m_pwndFindList->bQueryCurVal(m_strToFind);
        m_strToFind.StripWhitespace();

        //
        //  Let's add it to the list if needed, or move this value to the top of the
        //  list if it's already there.
        //
        const tCIDLib::TCard4 c4Count = m_pcolFindRecall->c4ElemCount();
        tCIDLib::TCard4 c4Index = 0;
        for (; c4Index < c4Count; c4Index++)
        {
            if (m_pcolFindRecall->objAt(c4Index) == m_strToFind)
                break;
        }

        //
        //  If already in the list, and not at the top already, then remove it
        if (c4Index < c4Count)
        {
            // Already in the list. If not already at the top remove and reinsert
            m_pcolFindRecall->RemoveAt(c4Index);
            m_pcolFindRecall->InsertAt(m_strToFind, 0);
        }
         else
        {
            // Not already there so add at the top
            m_pcolFindRecall->InsertAt(m_strToFind, 0);
        }


        //
        //  Do the same for the replace list if in that mode
        //
        if (m_bRepMode)
        {
            m_pwndRepList->bQueryCurVal(m_strRepWith);
            m_strRepWith.StripWhitespace();

            //
            //  Let's add it to the list if needed, or move this value to the top
            //  of the list if it's already there.
            //
            const tCIDLib::TCard4 c4Count = m_pcolRepRecall->c4ElemCount();
            tCIDLib::TCard4 c4Index = 0;
            for (; c4Index < c4Count; c4Index++)
            {
                if (m_pcolRepRecall->objAt(c4Index) == m_strRepWith)
                    break;
            }

            //
            //  If already in the list, and not at the top already, then remove it
            if (c4Index < c4Count)
            {
                // Already in the list. If not already at the top remove and reinsert
                m_pcolRepRecall->RemoveAt(c4Index);
                m_pcolRepRecall->InsertAt(m_strToFind, 0);
            }
             else
            {
                // Not already there so add at the top
                m_pcolRepRecall->InsertAt(m_strRepWith, 0);
            }
        }

        EndDlg(kCIDWUtils::ridDlg_Search_Find);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_Search_Close)
    {
        EndDlg(kCIDWUtils::ridDlg_Search_Close);
    }
    return tCIDCtrls::EEvResponses::Handled;
}

