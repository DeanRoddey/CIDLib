//
// FILE NAME: CIDCtrls_NotificationHandler.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/04/1997
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
//  This file provide the bit of out of line code of the base notification
//  handler class.
//
//  This was moved over from the old CIDWnd when we moved to wrapping standard
//  controls instead of doing our own.
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
#include    "CIDCtrls_.hpp"



// ---------------------------------------------------------------------------
//  CLASS: TNotHandlerBase
// PREFIX: wev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TNotHandlerBase: Constructors and destructor
// ---------------------------------------------------------------------------
TNotHandlerBase::TNotHandlerBase(const TNotificationId& nidRequested) :

    m_nidRequested(nidRequested)
    , m_pnothNext(0)
{
}

TNotHandlerBase::~TNotHandlerBase()
{
}


// ---------------------------------------------------------------------------
//  TNotHandlerBase: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TNotificationId& TNotHandlerBase::nidRequested() const
{
    return m_nidRequested;
}

const TNotHandlerBase* TNotHandlerBase::pnothNext() const
{
    return m_pnothNext;
}

TNotHandlerBase* TNotHandlerBase::pnothNext()
{
    return m_pnothNext;
}


