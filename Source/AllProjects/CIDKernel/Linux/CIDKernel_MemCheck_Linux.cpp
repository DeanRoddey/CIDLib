//
// FILE NAME: CIDKernel_MemCheck_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 01/02/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides the Linux specific implementation of the TKrnlMemCheck
//  class, which is a simple abstraction for the host development environment
//  or OS heap debugging services.
//
// CAVEATS/GOTCHAS:
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



// ---------------------------------------------------------------------------
//   CLASS: TKrnlMemCheck
//  PREFIX: kmchk
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlMemCheck: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlMemCheck::TKrnlMemCheck() :
    __pSnapShot(0)
{
}

TKrnlMemCheck::~TKrnlMemCheck()
{
}


// ---------------------------------------------------------------------------
//  TKrnlMemCheck: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKrnlMemCheck::ReportToFile(const TKrnlFile& kflTarget)
{
}


tCIDLib::TVoid TKrnlMemCheck::ReportToStdOut()
{
}


tCIDLib::TVoid TKrnlMemCheck::ValidateHeap()
{
}
