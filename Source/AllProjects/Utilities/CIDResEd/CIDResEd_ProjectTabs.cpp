//
// FILE NAME: CIDResEd_ProjTabs.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/17/2015
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the project management tabs.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "CIDResEd.hpp"


// ---------------------------------------------------------------------------
//  CLASS: TMCListDlgOps
// PREFIX: mclops
// ---------------------------------------------------------------------------
class TMCListDlgOps : public TMCListOps
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructors
        // -------------------------------------------------------------------
        TMCListDlgOps(const tCIDResEd::TDlgDescList* const pcolDlgs) :

            m_pcolDlgs(pcolDlgs)
            , m_preddSingle(0)
        {
        }

        TMCListDlgOps(const TResEdDlgDesc* const preddSingle) :

            m_pcolDlgs(0)
            , m_preddSingle(preddSingle)
        {
        }

        TMCListDlgOps(const TMCListDlgOps&) = delete;

        ~TMCListDlgOps() {}


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ColCount() const override;

        tCIDLib::TCard4 c4RowCount() const override;

        tCIDLib::TCard4 c4RowId(const tCIDLib::TCard4 c4RowIndex) const override;

        const TString& strColumnText
        (
            const   tCIDLib::TCard4     c4RowIndex
            , const tCIDLib::TCard4     c4ColIndex
        )   const override;


        const TResEdDlgDesc*            m_preddSingle;
        const tCIDResEd::TDlgDescList*  m_pcolDlgs;
        mutable TString                 m_strTmpFmt;
};


// ---------------------------------------------------------------------------
//  TMCListDlgOps: Public, virtual methods
// ---------------------------------------------------------------------------

// The colums are the resource id and the resource symbol name
tCIDLib::TCard4 TMCListDlgOps::c4ColCount() const
{
    return 2;
}


tCIDLib::TCard4 TMCListDlgOps::c4RowCount() const
{
    // If in single item mode, we only have 1
    if (m_preddSingle)
        return 1;

    // Else, however many are in the list
    return m_pcolDlgs->c4ElemCount();
}


tCIDLib::TCard4 TMCListDlgOps::c4RowId(const tCIDLib::TCard4 c4RowIndex) const
{
    if (m_preddSingle)
    {
        CIDAssert(c4RowIndex == 0, L"MCListDlgOps index must be 0 for single mode");
        return m_preddSingle->c4EditId();
    }

    return m_pcolDlgs->objAt(c4RowIndex).c4EditId();
}


const TString&
TMCListDlgOps::strColumnText(const  tCIDLib::TCard4 c4RowIndex
                            , const tCIDLib::TCard4 c4ColIndex) const
{
    // Get a pointer to teh single item or the list entry
    const TResEdDlgDesc* preddRow = 0;
    if (m_preddSingle)
    {
        CIDAssert(c4RowIndex == 0, L"MCListDlgOps index must be 0 for single mode");
        preddRow = m_preddSingle;
    }
     else
    {
        preddRow = &m_pcolDlgs->objAt(c4RowIndex);
    }

    // The id we have to format into a temp string
    if (c4ColIndex == 0)
    {
        m_strTmpFmt.SetFormatted(preddRow->ridDlg());
        return m_strTmpFmt;
    }
    return preddRow->strName();
}




// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TDialogsTab, TTabWindow)


// ---------------------------------------------------------------------------
//   CLASS: TDialogsTab
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDialogsTab: Constructors and Destructor
// ---------------------------------------------------------------------------
TDialogsTab::TDialogsTab() :

    TTabWindow(L"DialogList", L"", kCIDLib::False, kCIDLib::True)
    , m_pwndList(nullptr)
{
}

TDialogsTab::~TDialogsTab()
{
}


// ---------------------------------------------------------------------------
//  TDialogsTab: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TDialogsTab::CreateDlgsTab(TResEdMainFrame& wndMainFrm, TTabbedWnd& wndParent)
{
    // Aks the tabbed window to create us and install us as a tab
    wndParent.c4CreateTab(this, facCIDResEd.strMsg(kResEdMsgs::midTabs_Dialogs));

    // Install event handlers on the main frame
    m_pwndList->pnothRegisterHandler(&wndMainFrm, &TResEdMainFrame::eListHandler);
}


//
//  When a new dialog is added, this is called to put it into the list.
//
tCIDLib::TVoid TDialogsTab::InsertNew(const TResEdDlgDesc& reddAdd)
{
    TMCListDlgOps mclopsLoad(&reddAdd);
    m_pwndList->LoadList(mclopsLoad, kCIDLib::True);
}


//
//  Load up the info on the dialogs. We get the currently active resource data, and
//  just put it into one of our multi-column list ops objects and use that to load
//  the list.
//
tCIDLib::TVoid TDialogsTab::LoadList(const TResEdData& redatSrc)
{
    TMCListDlgOps mclopsLoad(&redatSrc.colDlgs());
    m_pwndList->LoadList(mclopsLoad);
}


// Called when the session is closed
tCIDLib::TVoid TDialogsTab::Reset()
{
    m_pwndList->RemoveAll();
}


tCIDLib::TVoid TDialogsTab::UpdateDlg(const TResEdDlgDesc& reddNew)
{
    const tCIDLib::TCard4 c4RowInd = m_pwndList->c4IdToIndex(reddNew.c4EditId());

    TMCListDlgOps mclopsLoad(&reddNew);
    m_pwndList->UpdateRowAt(mclopsLoad, c4RowInd);
}


// ---------------------------------------------------------------------------
//  TDialogsTab: Protected, inherited methods
// ---------------------------------------------------------------------------

// Keep our list sized to fit us
tCIDLib::TVoid
TDialogsTab::AreaChanged(const  TArea&                  areaPrev
                        , const TArea&                  areaNew
                        , const tCIDCtrls::EPosStates   ePosState
                        , const tCIDLib::TBoolean       bOrgChanged
                        , const tCIDLib::TBoolean       bSizeChanged
                        , const tCIDLib::TBoolean       bStateChanged)
{
    // Call our parent first
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    // Keep our list sized to fit
    if ((ePosState != tCIDCtrls::EPosStates::Minimized) && m_pwndList && bSizeChanged)
        m_pwndList->SetSize(areaClient().szArea(), kCIDLib::True);
}

tCIDLib::TBoolean TDialogsTab::bCreated()
{
    //
    //  Set our name for notification purposes, since we are notifying to a non-
    //  parent window, so our id may not be unique.
    //
    strName(kCIDResEd::strWnd_Tab_Dialogs);

    TArea areaList(areaClient());

    // Create a multi-column box and size it to fit our whole area
    m_pwndList = new TMultiColListBox();

    tCIDLib::TStrList colTitles(2);
    colTitles.objAdd(L"Id");
    colTitles.objAdd(L"Dialog Symbol");

    m_pwndList->CreateMCLB
    (
        *this
        , kCIDCtrls::widFirstCtrl
        , areaList
        , colTitles
        , 0
        , tCIDCtrls::EWndStyles::VisTabChild
        , tCIDLib::eOREnumBits
          (
            tCIDCtrls::EMCLBStyles::FullRowSel, tCIDCtrls::EMCLBStyles::AutoLastCol
          )
    );

    const tCIDLib::TCard4 c4ColZWidth = TGUIFacility::c4AdjustHDlgUnit(28);
    m_pwndList->SetColOpts(0, c4ColZWidth);
    return kCIDLib::True;
}


//
//  Since we dynamically allocated our children, we don't technically need to
//  clean them up ourself. The base window class would do it, but just to be
//  good people.
//
tCIDLib::TVoid TDialogsTab::Destroyed()
{
    DestroyAllChildren();
    TParent::Destroyed();
}

