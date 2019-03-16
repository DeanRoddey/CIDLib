//
// FILE NAME: CIDNameSrv_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/08/2000
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
//  This is the header for the facility class for the name server.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//   CLASS: TFacCIDNameSrv
//  PREFIX: fac
// ---------------------------------------------------------------------------
class TFacCIDNameSrv : public TFacility, public MSignalHandler
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDNameSrv();

        TFacCIDNameSrv(const TFacCIDNameSrv&) = delete;

        ~TFacCIDNameSrv();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDNameSrv& operator=(const TFacCIDNameSrv&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bHandleSignal
        (
            const   tCIDLib::ESignals       eSignal
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eMainThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::TVoid Shutdown
        (
            const   tCIDLib::EExitCodes     eToReturn
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ParseParms();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bNoLease
        //      Indicates that we shouldn't enforce binding leases. Mostly
        //      for development purposes, to keep bindings from timeing out
        //      when programs are blocked for debugging purposes.
        //
        //  m_c4MaxClients
        //      The maximum simultaneous clients we'll accept. It defaults
        //      to 16 if its not set on the command line. We just pass this
        //      on to the server side orb.
        //
        //  m_eReturn
        //      We support the core admin protocol, which can shut us down.
        //      We have to return some specific values that tell any shell
        //      we are being managed by what to do.
        //
        //  m_ippnListen
        //      The IP port on which we listen for connections. We just pass
        //      this on to the server side ORB init. If not provided via the
        //      environment or command line, kCIDOrbUC::ippnDefNSPort will be
        //      used.
        //
        //  m_pevWait
        //      The main thread just waits on this event once it has done
        //      the init. It's a pointer because its a named object and we want
        //      to catch an exception of a previous instance exists, and exit
        //      without doing any damage.
        //
        //  m_porbsCache
        //      This is a pointer to our server side object which implements
        //      the name server interface. It really does all of the work and
        //      we just have a few APIs that allow us to control it.
        //
        //      NOTE:   We don't own it, the Orb owns it. We need a pointer
        //              of our own in order to interact with it as required.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bNoLease;
        tCIDLib::TCard4         m_c4MaxClients;
        tCIDLib::EExitCodes     m_eReturn;
        tCIDLib::TIPPortNum     m_ippnListen;
        TEvent*                 m_pevWait;
        TCIDNameServerImpl*     m_porbsCache;
};

