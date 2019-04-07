//
// FILE NAME: CIDMacroDbg_TextOutTab.cpp
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
//  This file implements the text output tab window.
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
RTTIDecls(TTextOutTab, TTabWindow)



// ---------------------------------------------------------------------------
//   CLASS: TTextOutTab
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTextOutTab: Constructors and Destructor
// ---------------------------------------------------------------------------
TTextOutTab::TTextOutTab(const TString& strName) :

    TTabWindow(strName, L"", kCIDLib::False, kCIDLib::False)
    , m_pwndTextOut(nullptr)
{
}

TTextOutTab::~TTextOutTab()
{
}


// ---------------------------------------------------------------------------
//  TTextOutTab: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TTextOutTab::AddLines(const tCIDLib::TStrList& colLines)
{
    if (m_pwndTextOut)
        m_pwndTextOut->AddStrings(colLines);
}


tCIDLib::TVoid
TTextOutTab::CreateTOTab(TTabbedWnd& wndParent, const TString& strTabText)
{
    wndParent.c4CreateTab(this, strTabText);
}


tCIDLib::TVoid TTextOutTab::Reset()
{
    // Clear the text output
    if (m_pwndTextOut)
        m_pwndTextOut->Clear();
}


// ---------------------------------------------------------------------------
//  TTextOutTab: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TTextOutTab::AreaChanged(const  TArea&                  areaPrev
                        , const TArea&                  areaNew
                        , const tCIDCtrls::EPosStates   ePosState
                        , const tCIDLib::TBoolean       bOrgChanged
                        , const tCIDLib::TBoolean       bSizeChanged
                        , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    // Keep the text output window fitted to our client area
    if (m_pwndTextOut && bSizeChanged && (ePosState != tCIDCtrls::EPosStates::Minimized))
        m_pwndTextOut->SetSize(areaClient().szArea(), kCIDLib::True);
}


tCIDLib::TBoolean TTextOutTab::bCreated()
{
    TParent::bCreated();

    m_pwndTextOut = new TTextOutWnd();
    m_pwndTextOut->CreateTextOut
    (
        *this, areaClient(), kCIDCtrls::widFirstCtrl, tCIDCtrls::EWndStyles::VisChild
    );
    return kCIDLib::True;
}


