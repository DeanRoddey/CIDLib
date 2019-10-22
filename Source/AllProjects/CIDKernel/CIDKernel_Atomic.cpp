//
// FILE NAME: CIDKernel_BaseLock.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/25/1997
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
//  This file implements a very simple class, TBaseLock, that is used for
//  initialization synchronization purposes. It is all platform independent
//  since it just uses other kernel abstractions.
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
#include    "CIDKernel_.hpp"


// To get a safely faulted in lock
static TKrnlCritSec* pkcrsBaseLock()
{
    static TKrnlCritSec kcrsBaseLock;
    return &kcrsBaseLock;
}


// ---------------------------------------------------------------------------
//   CLASS: TBaseLock
//  PREFIX: lock
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBaseLock: Contructors and Destructor
// ---------------------------------------------------------------------------
TBaseLock::TBaseLock()
{
    pkcrsBaseLock()->Enter();
}

TBaseLock::~TBaseLock()
{
    pkcrsBaseLock()->Exit();
}
