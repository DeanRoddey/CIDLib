//
// FILE NAME: CIDNameSrv_CoreAdminImpl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/30/2001
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
//  This file provides our implementation of the core admin protocol.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "CIDNameSrv.hpp"


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

    TCIDCoreAdminBaseImpl(L"CIDNameSrv")
{
}

TCIDCoreAdminImpl::~TCIDCoreAdminImpl()
{
}


// ---------------------------------------------------------------------------
//  TCIDCoreAdminImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDCoreAdminImpl::AdminStop()
{
    facCIDNameSrv.Shutdown(tCIDLib::EExitCodes::AdminStop);
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

