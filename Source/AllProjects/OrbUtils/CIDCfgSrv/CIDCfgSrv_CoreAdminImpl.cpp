//
// FILE NAME: CIDCfgSrv_CoreAdminImpl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/05/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides our implementation of the core admin protocol.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "CIDCfgSrv.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDCoreAdminImpl,TCIDCoreAdminBaseImpl)



// ---------------------------------------------------------------------------
//   CLASS: TCIDCoreAdminImpl
//  PREFIX: orbs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDCoreAdminImpl::TCIDCoreAdminImpl() :

    TCIDCoreAdminBaseImpl(L"CIDCfgSrv")
{
}

TCIDCoreAdminImpl::~TCIDCoreAdminImpl()
{
}


// ---------------------------------------------------------------------------
// Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDCoreAdminImpl::AdminStop()
{
    facCIDCfgSrv.Shutdown(tCIDLib::EExitCodes::AdminStop);
}


// ---------------------------------------------------------------------------
//  TCIDCoreAdminImpl: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDCoreAdminImpl::Initialize()
{
    // Nothing to do
}


tCIDLib::TVoid TCIDCoreAdminImpl::Terminate()
{
    // Nothing to do
}

