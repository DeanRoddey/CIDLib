//
// FILE NAME: CIDCtrls_Button.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/14/2015
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
//  This file implements the common notification class for the button types.
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
RTTIDecls(TButtClickInfo,TCtrlNotify)



// ---------------------------------------------------------------------------
//   CLASS: TButtClickInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TButtClickInfo: Public static data
// ---------------------------------------------------------------------------
const TNotificationId TButtClickInfo::nidClick(L"ButtonClickEvent");


// ---------------------------------------------------------------------------
//  TButtClickInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TButtClickInfo::TButtClickInfo(const tCIDLib::TBoolean bState, const TWindow& wndSrc) :

    TCtrlNotify(wndSrc)
    , m_bState(bState)
{
}

TButtClickInfo::TButtClickInfo(const TButtClickInfo& wnotSrc) :

    TCtrlNotify(wnotSrc)
    , m_bState(wnotSrc.m_bState)
{
}

TButtClickInfo::~TButtClickInfo()
{
}


// ---------------------------------------------------------------------------
//  TButtClickInfo: Public operators
// ---------------------------------------------------------------------------
TButtClickInfo& TButtClickInfo::operator=(const TButtClickInfo& wnotSrc)
{
    if (this != &wnotSrc)
    {
        TParent::operator=(wnotSrc);
        m_bState = wnotSrc.m_bState;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TButtClickInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TButtClickInfo::bState() const
{
    return m_bState;
}


