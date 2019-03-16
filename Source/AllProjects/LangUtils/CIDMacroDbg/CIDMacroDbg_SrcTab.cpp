//
// FILE NAME: CIDMacroDbg_SrcTab.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/11/2015
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
//  This file implements the source editor tab window.
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
RTTIDecls(TSrcTab, TTabWindow)



// ---------------------------------------------------------------------------
//   CLASS: TSrcTab
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSrcTab: Constructors and Destructor
// ---------------------------------------------------------------------------

//
//  We pass the class path through to the tab window name, and that's where we get
//  it from if it's needed. This way we can look up a tab by its class path.
//
//  If it's the session path we indicate it cannot be closed.
//
TSrcTab::TSrcTab(const  TString&            strClassName
                , const tCIDLib::TBoolean   bIsSessionClass) :

    TTabWindow(strClassName, L"", !bIsSessionClass, kCIDLib::True)
    , m_bSessionClass(kCIDLib::False)
    , m_eMode(tCIDMacroEng::EResModes::ReadOnly)
    , m_pnothEdit(0)
    , m_pwndEditor(nullptr)
{
}

TSrcTab::~TSrcTab()
{
}


// ---------------------------------------------------------------------------
//  TSrcTab: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  The main debugger window will pass on accellerator processing calls to the
//  active source editor. Currently we aren't processing any here, so we just
//  return false.
//
tCIDLib::TBoolean TSrcTab::bProcessAccel(const tCIDLib::TVoid* const) const
{
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TSrcTab: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TSrcTab::bChanged() const
{
    if (!m_pwndEditor)
        return kCIDLib::False;
    return m_pwndEditor->bChanges();
}


// Make our goto line list available to the main window
tCIDLib::TStrList& TSrcTab::colGotoList()
{
    return m_colGotoList;
}


tCIDLib::TVoid
TSrcTab::CreateSrcTab(TTabbedWnd& wndParent, const TString& strTabText)
{
    wndParent.c4CreateTab(this, strTabText);
}



tCIDMacroEng::EResModes TSrcTab::eMode() const
{
    return m_eMode;
}

tCIDMacroEng::EResModes TSrcTab::eMode(const tCIDMacroEng::EResModes eToSet)
{
    m_eMode = eToSet;

    // Update the editor's read only attribute to match
    if (m_pwndEditor)
        m_pwndEditor->bReadOnly(eToSet == tCIDMacroEng::EResModes::ReadOnly);

    return m_eMode;
}


tCIDLib::TVoid TSrcTab::GoToLine(const tCIDLib::TCard4 c4LineNum)
{
    if (m_pwndEditor)
        m_pwndEditor->GoToLine(c4LineNum);
}


// Provide access to our editor
const TSrcEditor* TSrcTab::pwndEditor() const
{
    return m_pwndEditor;
}

TSrcEditor* TSrcTab::pwndEditor()
{
    return m_pwndEditor;
}


const TString& TSrcTab::strClassPath() const
{
    // We just store the class path in the window name, so just pass that back
    return strName();
}


tCIDLib::TVoid TSrcTab::SetText(const TString& strToSet)
{
    if (m_pwndEditor)
        m_pwndEditor->LoadText(strToSet, strName());
}


// ---------------------------------------------------------------------------
//  TSrcTab: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TSrcTab::AreaChanged(const  TArea&                  areaPrev
                    , const TArea&                  areaNew
                    , const tCIDCtrls::EPosStates   ePosState
                    , const tCIDLib::TBoolean       bOrgChanged
                    , const tCIDLib::TBoolean       bSizeChanged
                    , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    // Keep the editor fitted to us
    if (m_pwndEditor)
        m_pwndEditor->SetSize(areaClient().szArea(), kCIDLib::True);
}


tCIDLib::TBoolean TSrcTab::bCreated()
{
    TParent::bCreated();

    // Create our scroll area and text output windows
    TArea areaClient;
    QueryClientArea(areaClient, kCIDLib::True);

    m_pwndEditor = new TSrcEditor();
    m_pwndEditor->CreateSrcEditor
    (
        *this, 1, areaClient, tCIDCtrls::EWndStyles::VisChild, strName()
    );

    //
    //  Force it's background to the standard window background, else he will pick
    //  up ours, which is a contrast color, not a background color.
    //
    m_pwndEditor->SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::Window));

    return kCIDLib::True;
}


// Clean up the editor window that we created
tCIDLib::TVoid TSrcTab::Destroyed()
{
    m_pwndEditor->Destroy();
    delete m_pwndEditor;
    m_pwndEditor = nullptr;

    TParent::Destroyed();
}


// Make sure we pass the focus on to the editor
tCIDLib::TVoid TSrcTab::GettingFocus()
{
    if (m_pwndEditor && m_pwndEditor->bIsValid())
        m_pwndEditor->TakeFocus();
}

