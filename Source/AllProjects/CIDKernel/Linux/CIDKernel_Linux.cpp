//
// FILE NAME: CIDKernel_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 01/17/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file contains the Linux specific parts of the main module,
//  CIDKernel.Cpp. In this case we provide the module init for the main
//  module which just does any process wide fundamental setup.
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
#include    "CIDKernel_InternalHelpers_.hpp"



// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::__bInitTermCore(const tCIDLib::EInitTerm eState)
{
    return TKrnlLinux::bInitTermExtProcess(eState);
}
