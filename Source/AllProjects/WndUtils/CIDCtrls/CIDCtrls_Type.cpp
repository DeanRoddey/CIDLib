//
// FILE NAME: CIDCtrls_Type.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/01/2015
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
//  Some of our types have to be down in CIDKernel so that the build tool can
//  use them, so we have to provide format/xlat stuff manually.
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
//  The edge anchors enumeration
// ---------------------------------------------------------------------------
static const TString astrEEdgeAnchorsVals[tCIDLib::c4EnumOrd(tCIDCtrls::EEdgeAnchors::Count)] =
{
    L"None"
    , L"MoveBottom"
    , L"MoveRight"
    , L"MoveBottomRight"
    , L"SizeBottom"
    , L"SizeRight"
    , L"SizeBottomRight"
    , L"MoveBottomSizeRight"
    , L"SizeBottomMoveRight"
};

static const TEArray<TString, tCIDCtrls::EEdgeAnchors, tCIDCtrls::EEdgeAnchors::Count>
astrEEdgeAnchorsMap(astrEEdgeAnchorsVals);

tCIDCtrls::EEdgeAnchors tCIDCtrls::eXlatEEdgeAnchors(const TString& strToXlat)
{
    tCIDCtrls::EEdgeAnchors eAnchor = tCIDCtrls::EEdgeAnchors::Min;
    while (eAnchor < tCIDCtrls::EEdgeAnchors::Count)
    {
        // We will be case tolerant
        if (strToXlat.bCompareI(astrEEdgeAnchorsMap[eAnchor]))
            break;
        eAnchor++;
    }
    return eAnchor;
}

const TString& tCIDCtrls::strLoadEEdgeAnchors(const tCIDCtrls::EEdgeAnchors eToXlat)
{
    return astrEEdgeAnchorsMap[eToXlat];
}




// ---------------------------------------------------------------------------
//  The window themes enumeration
// ---------------------------------------------------------------------------
static const TString astrEWndThemesVals[tCIDLib::c4EnumOrd(tCIDCtrls::EWndThemes::Count)] =
{
    L"None"
    , L"MainWnd"
    , L"DialogBox"
};
static const TEArray<TString, tCIDCtrls::EWndThemes, tCIDCtrls::EWndThemes::Count>
astrEWndThemesMap(astrEWndThemesVals);

tCIDCtrls::EWndThemes tCIDCtrls::eXlatEWndThemes(const TString& strToXlat)
{
    tCIDCtrls::EWndThemes eTheme = tCIDCtrls::EWndThemes::Min;
    while (eTheme < tCIDCtrls::EWndThemes::Count)
    {
        // We will be case tolerant
        if (strToXlat.bCompareI(astrEWndThemesMap[eTheme]))
            break;
        eTheme++;
    }
    return eTheme;
}

const TString& tCIDCtrls::strLoadEWndThemes(const tCIDCtrls::EWndThemes eToXlat)
{
    return astrEWndThemesMap[eToXlat];
}

