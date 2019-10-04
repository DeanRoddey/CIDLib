//
// FILE NAME: CIDKernel_MemCheck_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 01/02/1999
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
//  This file provides the Linux specific implementation of the TKrnlMemCheck
//  class, which is a simple abstraction for the host development environment
//  or OS heap debugging services.
//
// CAVEATS/GOTCHAS:
//
//  There ain't nothin' there.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"
#include    "CIDKernel_MemCheck.hpp"

#if CID_DEBUG_ON

// ---------------------------------------------------------------------------
//   CLASS: TKrnlMemCheck
//  PREFIX: kmchk
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlMemCheck: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlMemCheck::TKrnlMemCheck() :

    m_pSnapShot(0)
{
}

TKrnlMemCheck::~TKrnlMemCheck()
{
}


// ---------------------------------------------------------------------------
//  TKrnlMemCheck: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKrnlMemCheck::ReportToFile(const tCIDLib::TCh* const)
{
}


tCIDLib::TVoid TKrnlMemCheck::ReportToStdOut()
{
}


tCIDLib::TVoid TKrnlMemCheck::ValidateHeap()
{
}

#endif
