//
// FILE NAME: CIDCtrls_StatusBar.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/01/2015
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
//  This file implements the status bar window.
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
AdvRTTIDecls(TStatusBar, TWindow)




// ---------------------------------------------------------------------------
//  CLASS: TStatusBar
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TStatusBar: Constructors and Destructor
// ---------------------------------------------------------------------------
TStatusBar::TStatusBar() :

    TWindow(kCIDLib::False)
{
}

TStatusBar::~TStatusBar()
{
}


// ---------------------------------------------------------------------------
// TStatusBar: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Just set up the styles and call our parent to create the control
tCIDLib::TVoid
TStatusBar::Create( const   TWindow&                wndParent
                    , const tCIDCtrls::TWndId       widThis)
{
    CreateWnd
    (
        wndParent.hwndThis()
        , STATUSCLASSNAME
        , kCIDLib::pszEmptyZStr
        , TArea(1, 1, 1, 1)
        , tCIDCtrls::EWndStyles::VisChild
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );

    // All standard controls must be subclassed
    SetSubclass();
}


//
//  Send a size message to ourself. The status bar will resize to fit. The parent
//  frame mus send us this when his size changes.
//
tCIDLib::TVoid TStatusBar::ParentSizeChanged()
{
    ::SendMessage(hwndSafe(), WM_SIZE, 0, 0);
}



//
//  We get a list of size that we use to set the area widths. The last one can be
//  zero if you want it to get the rest of teh available area.
//
//  If you want to go back to one area, then pass an empty list. The values can be
//  percentages of our window width or pixels.
//
tCIDLib::TVoid
TStatusBar::SetAreas(const  tCIDLib::TCardList&     fcolVals
                    , const tCIDLib::TBoolean       bPercents)
{
    tCIDLib::TCard4 c4Count = fcolVals.c4ElemCount();

    // If none, we just set up one big one
    int* paiEndPnts = nullptr;
    tCIDLib::TCard4 c4LoopCnt = c4Count;
    if (!c4Count)
    {
        paiEndPnts = new int[1];
        paiEndPnts[0] = -1;
        c4LoopCnt = 0;
        c4Count = 1;
    }
     else
    {
        paiEndPnts = new int[c4Count];

        // If the last incoming is zero, do one less
        if (fcolVals[c4Count - 1] == 0)
            c4LoopCnt--;
    }
    TArrayJanitor<int> janEndPnts(paiEndPnts);

    tCIDLib::TCard4 c4Index = 0;
    int iSoFar = 0;
    if (bPercents)
    {
        // Get our window area
        TArea areaSz = areaWndSize();
        const tCIDLib::TFloat4 f4Width = tCIDLib::TFloat4(areaSz.c4Width());
        while (c4Index < c4LoopCnt)
        {
            iSoFar += int(f4Width * (tCIDLib::TFloat4(fcolVals[c4Index]) / 100.0));
            paiEndPnts[c4Index++] = iSoFar;
        }
    }
     else
    {
        while (c4Index < c4LoopCnt)
        {
            iSoFar += int(fcolVals[c4Index]);
            paiEndPnts[c4Index++] = iSoFar;
        }
    }

    // If the last one is to be the rest, set it
    if (c4LoopCnt < c4Count)
        paiEndPnts[c4Index] = -1;

    // And now set them
    if (!::SendMessage(hwndSafe(), SB_SETPARTS, c4Count, tCIDCtrls::TLParam(paiEndPnts)))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcSBar_SetAreas
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4Count)
        );
    }
}


//
//  Set the text of an area by index
//
tCIDLib::TVoid
TStatusBar::SetAreaText(const   tCIDLib::TCard4 c4Index
                        , const TString&        strToSet)
{
    tCIDCtrls::TWParam wpIndex;
    wpIndex = SBT_POPOUT;
    wpIndex <<= 8;
    wpIndex |= tCIDLib::TCard1(c4Index);

    BOOL bRes = ::SendMessage
    (
        hwndSafe()
        , SB_SETTEXT
        , wpIndex
        , tCIDCtrls::TLParam(strToSet.pszBuffer())
    );

    if (!bRes)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcSBar_SetAreaText
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4Index)
        );
    }
}

