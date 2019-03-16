//
// FILE NAME: CIDCfgSrv_CoreAdminImpl.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/05/2001
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
//
// DESCRIPTION:
//
//  This is the header for the CIDCfgSrv_CoreAdminImpl.cpp file, which
//  implements the TCIDCoreAdminImpl class. This is our implementation of the
//  CIDOrbUC defined core admin remote protocol. It allows us to be
//  controlled by any management client that understands this protocol.
//  Actually we derive from a helper class that provides some of the core
//  functionality for us, not directly from the IDL stub.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TCIDCoreAdminImpl
//  PREFIX: orbs
// ---------------------------------------------------------------------------
class TCIDCoreAdminImpl : public TCIDCoreAdminBaseImpl
{
    public :
        // --------------------------------------------------------------------
        // Constructors and Destructor
        // --------------------------------------------------------------------
        TCIDCoreAdminImpl();

        ~TCIDCoreAdminImpl();


        // --------------------------------------------------------------------
        // Public, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid AdminStop();


    protected :
        // --------------------------------------------------------------------
        //  Protected, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid Initialize() override;

        tCIDLib::TVoid Terminate() override;


    private :
        // --------------------------------------------------------------------
        //  Unimplemented ctors and operators
        // --------------------------------------------------------------------
        TCIDCoreAdminImpl(const TCIDCoreAdminImpl&);
        tCIDLib::TVoid operator=(const TCIDCoreAdminImpl&);


        // --------------------------------------------------------------------
        //  Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TCIDCoreAdminImpl,TCIDCoreAdminBaseImpl)
};

#pragma CIDLIB_POPPACK

