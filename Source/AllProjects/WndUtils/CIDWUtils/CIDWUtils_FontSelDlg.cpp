//
// FILE NAME: CIDWUtils_FontSelDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/27/2015
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
//  Implements the TFontSelDlg dialog.
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
#include    "CIDWUtils_FontSelDlg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFontSelDlg, TDlgBox)
RTTIDecls(TFontFaceSelDlg, TDlgBox)





// ---------------------------------------------------------------------------
//   CLASS: TFontSelDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFontSelDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TFontSelDlg::TFontSelDlg() :

    m_bBold(kCIDLib::False)
    , m_bItalic(kCIDLib::False)
    , m_c4Height(kCIDLib::False)
    , m_pwndBold(nullptr)
    , m_pwndCancel(nullptr)
    , m_pwndCurSize(nullptr)
    , m_pwndFontList(nullptr)
    , m_pwndItalic(nullptr)
    , m_pwndSample(nullptr)
    , m_pwndSave(nullptr)
    , m_pwndSize(nullptr)
    , m_pwndUnderline(nullptr)
{
}

TFontSelDlg::~TFontSelDlg()
{
}


// ---------------------------------------------------------------------------
//  TFontSelDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TFontSelDlg::bRun(  const   TWindow&            wndOwner
                    ,       TString&            strFaceName
                    ,       tCIDLib::TCard4&    c4Height
                    ,       tCIDLib::TBoolean&  bBold
                    ,       tCIDLib::TBoolean&  bItalic)
{
    // Store the incoming text until we can set it
    m_bBold        = bBold;
    m_bItalic      = bItalic;
    m_c4Height     = c4Height;
    m_strFaceName  = strFaceName;

    // If no face name, set a default
    if (m_strFaceName.bIsEmpty())
        m_strFaceName = L"Arial";

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_FontSel
    );

    if (c4Res == kCIDWUtils::ridDlg_FontSel_Save)
    {
        // Put back the stored stuff
        bBold        = m_bBold;
        bItalic      = m_bItalic;
        c4Height     = m_c4Height;
        strFaceName  = m_strFaceName;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TFontSelDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFontSelDlg::bCreated()
{
    // Now call our parent first and return his return
    const tCIDLib::TBoolean bRes = TParent::bCreated();

    // Get typed pointers to our controls
    CastChildWnd(*this, kCIDWUtils::ridDlg_FontSel_Bold, m_pwndBold);
    CastChildWnd(*this, kCIDWUtils::ridDlg_FontSel_Cancel, m_pwndCancel);
    CastChildWnd(*this, kCIDWUtils::ridDlg_FontSel_CurSize, m_pwndCurSize);
    CastChildWnd(*this, kCIDWUtils::ridDlg_FontSel_Italic, m_pwndItalic);
    CastChildWnd(*this, kCIDWUtils::ridDlg_FontSel_FontList, m_pwndFontList);
    CastChildWnd(*this, kCIDWUtils::ridDlg_FontSel_Sample, m_pwndSample);
    CastChildWnd(*this, kCIDWUtils::ridDlg_FontSel_Save, m_pwndSave);
    CastChildWnd(*this, kCIDWUtils::ridDlg_FontSel_Size, m_pwndSize);

    // Load up all the incoming values
    m_pwndBold->SetCheckedState(m_bBold);
    m_pwndItalic->SetCheckedState(m_bItalic);

    // Set the min/max range on the slider
    m_pwndSize->SetRange(4, 96);
    if (m_c4Height < 4)
        m_c4Height = 4;
    else if (m_c4Height > 96)
        m_c4Height = 96;
    m_pwndSize->SetValue(m_c4Height);

    // Set in on the numeric display as well
    m_strTmpFmt.SetFormatted(m_c4Height);
    m_pwndCurSize->strWndText(m_strTmpFmt);

    // Load the available font face names to the list
    TVector<TFontMetrics> colFonts;
    TGraphWndDev gdevTmp(*this);
    const tCIDLib::TCard4 c4Count = TFontMetrics::c4EnumAllFonts
    (
        gdevTmp
        , colFonts
        , tCIDGraphDev::EFontTypes::Scalable
        , tCIDGraphDev::EFontBases::Either
        , tCIDGraphDev::EFontPitches::Either
    );
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        m_pwndFontList->c4AddItem(colFonts[c4Index].strFaceName());

    //
    //  Select the current one if we have one and it's still available. Else select
    //  the 0th one.
    //
    tCIDLib::TCard4 c4At;
    if (m_strFaceName.bIsEmpty() || !m_pwndFontList->bSelectByText(m_strFaceName, c4At))
        m_pwndFontList->SelectByIndex(0);

    // Set the sample text
    m_pwndSample->strWndText(L"WvMnBXaK");

    // Do an initial update of the font on it
    UpdateSample();

    // Register handlers
    m_pwndBold->pnothRegisterHandler(this, &TFontSelDlg::eClickHandler);
    m_pwndCancel->pnothRegisterHandler(this, &TFontSelDlg::eClickHandler);
    m_pwndFontList->pnothRegisterHandler(this, &TFontSelDlg::eListHandler);
    m_pwndItalic->pnothRegisterHandler(this, &TFontSelDlg::eClickHandler);
    m_pwndSave->pnothRegisterHandler(this, &TFontSelDlg::eClickHandler);
    m_pwndSize->pnothRegisterHandler(this, &TFontSelDlg::eSliderHandler);

    return bRes;
}


// ---------------------------------------------------------------------------
//  TFontSelDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TFontSelDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if ((wnotEvent.widSource() == kCIDWUtils::ridDlg_FontSel_Bold)
    ||  (wnotEvent.widSource() == kCIDWUtils::ridDlg_FontSel_Italic))
    {
        // Just update the sample to reflect the change
        UpdateSample();
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_FontSel_Cancel)
    {
        EndDlg(0);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_FontSel_Save)
    {
        // Store away the current settings
        m_bBold = m_pwndBold->bCheckedState();
        m_bItalic = m_pwndItalic->bCheckedState();
        m_c4Height = tCIDLib::TCard4(m_pwndSize->i4CurValue());

        m_strFaceName = m_pwndFontList->strCurSelection();

        EndDlg(1);
    }
    return tCIDCtrls::EEvResponses::Handled;
}


tCIDCtrls::EEvResponses TFontSelDlg::eListHandler(TListChangeInfo& wnotEvent)
{
    // If a new selection is made, update the sample to match
    if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::SelChanged)
        UpdateSample();
    return tCIDCtrls::EEvResponses::Handled;
}


tCIDCtrls::EEvResponses TFontSelDlg::eSliderHandler(TSliderChangeInfo& wnotEvent)
{
    if ((wnotEvent.eEvent() == tCIDCtrls::ESldrEvents::Change)
    ||  (wnotEvent.eEvent() == tCIDCtrls::ESldrEvents::Track))
    {
        // Just update the sample to match
        UpdateSample();
    }
    return tCIDCtrls::EEvResponses::Handled;
}


//
//  We grab the current settings, set up a font selection attribute, and update the
//  font on the sample text window.
//
tCIDLib::TVoid TFontSelDlg::UpdateSample()
{
    TFontSelAttrs fselSet;
    fselSet.strFaceName(m_pwndFontList->strCurSelection());
    fselSet.c4Height
    (
        tCIDLib::TCard4(m_pwndSize->i4CurValue()), tCIDGraphDev::EFontHeights::Cell
    );
    fselSet.bItalic(m_pwndItalic->bCheckedState());
    if (m_pwndBold->bCheckedState())
        fselSet.eWeight(tCIDGraphDev::EFontWeights::Bold);

    // Update the numeric size display
    m_strTmpFmt.SetFormatted(fselSet.c4Height());
    m_pwndCurSize->strWndText(m_strTmpFmt);

    // Create a font based on these selection criteria
    m_gfontSample.SetSelAttrs(fselSet);

    // And set it on the sample and force it to redraw with the new setting
    m_pwndSample->SetFont(m_gfontSample);
    m_pwndSample->ForceRepaint();
}






// ---------------------------------------------------------------------------
//   CLASS: TFontFaceSelDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFontFaceSelDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TFontFaceSelDlg::TFontFaceSelDlg() :

    m_bBold(kCIDLib::False)
    , m_bItalic(kCIDLib::False)
    , m_c4Height(8)
    , m_pwndCancel(nullptr)
    , m_pwndFontList(nullptr)
    , m_pwndSample(nullptr)
    , m_pwndSave(nullptr)
{
}

TFontFaceSelDlg::~TFontFaceSelDlg()
{
}


// ---------------------------------------------------------------------------
//  TFontFaceSelDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TFontFaceSelDlg::bRun(  const   TWindow&            wndOwner
                        ,       TString&            strFaceName
                        , const tCIDLib::TCard4     c4Height
                        , const tCIDLib::TBoolean   bBold
                        , const tCIDLib::TBoolean   bItalic)
{
    // Store the incoming text until we can set it
    m_bBold        = bBold;
    m_bItalic      = bItalic;
    m_c4Height     = c4Height;
    m_strFaceName  = strFaceName;

    // If no face name, set a default
    if (m_strFaceName.bIsEmpty())
        m_strFaceName = L"Arial";

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_FontFaceSel
    );

    if (c4Res == kCIDWUtils::ridDlg_FontFaceSel_Save)
    {
        // Put back the stored stuff
        strFaceName  = m_strFaceName;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TFontFaceSelDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFontFaceSelDlg::bCreated()
{
    // Now call our parent first and return his return
    const tCIDLib::TBoolean bRes = TParent::bCreated();

    // Get typed pointers to our controls
    CastChildWnd(*this, kCIDWUtils::ridDlg_FontFaceSel_Cancel, m_pwndCancel);
    CastChildWnd(*this, kCIDWUtils::ridDlg_FontFaceSel_FontList, m_pwndFontList);
    CastChildWnd(*this, kCIDWUtils::ridDlg_FontFaceSel_Sample, m_pwndSample);
    CastChildWnd(*this, kCIDWUtils::ridDlg_FontFaceSel_Save, m_pwndSave);

    // Load the available font face names to the list
    TVector<TFontMetrics> colFonts;
    TGraphWndDev gdevTmp(*this);
    const tCIDLib::TCard4 c4Count = TFontMetrics::c4EnumAllFonts
    (
        gdevTmp
        , colFonts
        , tCIDGraphDev::EFontTypes::Scalable
        , tCIDGraphDev::EFontBases::Either
        , tCIDGraphDev::EFontPitches::Either
    );
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        m_pwndFontList->c4AddItem(colFonts[c4Index].strFaceName());

    // Select the current one if we have one
    tCIDLib::TCard4 c4At;
    if (m_strFaceName.bIsEmpty() || !m_pwndFontList->bSelectByText(m_strFaceName, c4At))
        m_pwndFontList->SelectByIndex(0);

    // Set the sample text
    m_pwndSample->strWndText(L"WvMnBXaK");

    // Do an initial update of the font on it
    UpdateSample();

    // Register handlers
    m_pwndCancel->pnothRegisterHandler(this, &TFontFaceSelDlg::eClickHandler);
    m_pwndFontList->pnothRegisterHandler(this, &TFontFaceSelDlg::eListHandler);
    m_pwndSave->pnothRegisterHandler(this, &TFontFaceSelDlg::eClickHandler);

    return bRes;
}


// ---------------------------------------------------------------------------
//  TFontFaceSelDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TFontFaceSelDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_FontFaceSel_Cancel)
    {
        EndDlg(kCIDWUtils::ridDlg_FontSel_Cancel);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_FontFaceSel_Save)
    {
        m_strFaceName = m_pwndFontList->strCurSelection();
        EndDlg(kCIDWUtils::ridDlg_FontSel_Save);
    }
    return tCIDCtrls::EEvResponses::Handled;
}


tCIDCtrls::EEvResponses TFontFaceSelDlg::eListHandler(TListChangeInfo& wnotEvent)
{
    // If a new selection is made, update the sample to match
    if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::SelChanged)
        UpdateSample();
    return tCIDCtrls::EEvResponses::Handled;
}


//
//  We grab the current settings, set up a font selection attribute, and update the
//  font on the sample text window.
//
tCIDLib::TVoid TFontFaceSelDlg::UpdateSample()
{
    TFontSelAttrs fselSet;
    fselSet.strFaceName(m_pwndFontList->strCurSelection());
    fselSet.c4Height(m_c4Height, tCIDGraphDev::EFontHeights::Cell);
    fselSet.bItalic(m_bItalic);
    if (m_bBold)
        fselSet.eWeight(tCIDGraphDev::EFontWeights::Bold);

    // Create a font based on these selection criteria
    m_gfontSample.SetSelAttrs(fselSet);

    // And set it on the sample and force it to redraw with the new setting
    m_pwndSample->SetFont(m_gfontSample);
    m_pwndSample->ForceRepaint();
}


