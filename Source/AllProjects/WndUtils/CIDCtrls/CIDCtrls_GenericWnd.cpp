//
// FILE NAME: CIDCtrls_InPlaceEdit.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/06/2015
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


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TGenericWnd, TWindow)




// ---------------------------------------------------------------------------
//  CLASS: TGenericWnd
// PREFIX: ipe
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGenericWnd: Constructors and Destructor
// ---------------------------------------------------------------------------
TGenericWnd::TGenericWnd() :

    TCtrlWnd()
    , m_bLimitMinSz(kCIDLib::False)
{
}

TGenericWnd::~TGenericWnd()
{
}


// ---------------------------------------------------------------------------
//  TGenericWnd: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TGenericWnd::InitFromDesc(  const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild | tCIDCtrls::EWndStyles::ClipChildren
    );
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::ControlParent;

    if (eTheme == tCIDCtrls::EWndThemes::DialogBox)
        SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::DlgCtrlFill), kCIDLib::True);
    else if (eTheme == tCIDCtrls::EWndThemes::MainWnd)
        SetBgnColor(facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::Window), kCIDLib::True);

    CreateGenWnd(wndParent, dlgiSrc.areaPos(), eStyles, eExStyles, dlgiSrc.ridChild());
}


// ---------------------------------------------------------------------------
//  TGenericWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TGenericWnd::CreateGenWnd(  const   TWindow&                wndParent
                            , const TArea&                  areaInit
                            , const tCIDCtrls::EWndStyles   eStyles
                            , const tCIDCtrls::EExWndStyles eExStyles
                            , const tCIDCtrls::TWndId       widThis)
{
    // Just pass it on to our parent class
    CreateWnd
    (
        wndParent.hwndThis()
        , kCIDCtrls::pszCustClass
        , kCIDLib::pszEmptyZStr
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles::ClipChildren
        , eExStyles
        , widThis
    );
}


//
//  Let client code make us load up children from a dialog box. Otherwise, they'd
//  have to create a derived class to do this. They can also request that we do
//  an initial auto-size to get the content sized to fit our current size.
//
tCIDLib::TVoid
TGenericWnd::PopulateFromDlg(const  TDlgDesc&           dlgdToLoad
                            ,       tCIDLib::TCard4&    c4InitFocus
                            , const tCIDLib::TBoolean   bDoInitSize
                            , const tCIDLib::TBoolean   bLimitMinSz)
{
    // Remember this info for later use
    m_bLimitMinSz = bLimitMinSz;
    m_szOrg = dlgdToLoad.areaPos().szArea();

    // Create the dialog items
    CreateDlgItems(dlgdToLoad, c4InitFocus);

    // If asked do an init size from original content size to current window size
    if (bDoInitSize)
        DoAdjustment(dlgdToLoad.areaPos(), areaClient());
}


// ---------------------------------------------------------------------------
//  TGenericWnd: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TGenericWnd::AreaChanged(const  TArea&                  areaPrev
                        , const TArea&                  areaNew
                        , const tCIDCtrls::EPosStates   ePosState
                        , const tCIDLib::TBoolean       bOrgChanged
                        , const tCIDLib::TBoolean       bSizeChanged
                        , const tCIDLib::TBoolean       bStateChanged)
{
    // Call our parent first
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    // If our size changed, then auto-resize any children that have anchors set
    if ((ePosState != tCIDCtrls::EPosStates::Minimized) && bSizeChanged)
        DoAdjustment(areaPrev, areaNew);
}


// Do it in the paint to avoid flicker
tCIDLib::TBoolean TGenericWnd::bEraseBgn(TGraphDrawDev&)
{
    return kCIDLib::True;
}


tCIDLib::TBoolean
TGenericWnd::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    gdevToUse.Fill(areaUpdate, rgbBgnFill());
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TGenericWnd: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid TGenericWnd::DoAdjustment(const TArea& areaPrev, const TArea& areaNew)
{
    //
    //  If the previous or new is smaller than the min size, and we are enforcing that,
    //  we take the larger of the two.
    //
    if (m_bLimitMinSz)
    {
        TArea areaRealPrev(areaPrev);
        TArea areaRealNew(areaNew);
        areaRealPrev.TakeLarger(m_szOrg);
        areaRealNew.TakeLarger(m_szOrg);

        AutoAdjustChildren(areaRealPrev, areaRealNew);
    }
     else
    {
        // Just do the usual thing
        AutoAdjustChildren(areaPrev, areaNew);
    }
}
