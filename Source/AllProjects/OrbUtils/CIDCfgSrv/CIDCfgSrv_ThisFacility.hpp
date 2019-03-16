//
// FILE NAME: CIDCfgSrv_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/05/2001
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
//  This is the header for this program's facility class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//   CLASS: TFacCIDCfgSrv
//  PREFIX: fac
// ---------------------------------------------------------------------------
class TFacCIDCfgSrv : public TFacility, public MSignalHandler
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDCfgSrv();

        TFacCIDCfgSrv(const TFacCIDCfgSrv&) = delete;

        ~TFacCIDCfgSrv();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDCfgSrv& operator=(const TFacCIDCfgSrv&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bHandleSignal
        (
            const   tCIDLib::ESignals       eSignal
        );

        tCIDLib::EExitCodes eMainThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        const TString& strRepoPath() const;

        tCIDLib::TVoid Shutdown
        (
            const   tCIDLib::EExitCodes     eToReturn
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Cleanup();

        tCIDLib::TVoid ParseParms();

        tCIDLib::TVoid Setup();


        // -------------------------------------------------------------------
        //  Private data members
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
        //      this on to the server side ORB init. Defaults to 10999 if not
        //      provided on the cmd line.
        //
        //  m_pevWait
        //      The main thread just waits on this event once it has done
        //      the init. It's a pointer because its a named object and we want
        //      to catch an exception of a previous instance exists, and exit
        //      without doing any damage.
        //
        //  m_plgrLogSrv
        //      We install a log server logger. We don't own it after we've
        //      installed it, but we need to keep the pointer around so that
        //      we can force it back to local logging before we toast the
        //      client ORB support.
        //
        //  m_porbsImpl
        //      A pointer to our implementation object. We don't own it, the
        //      Orb does once we register the object. But we do want to keep
        //      our own (typed) pointer to it, for convenience.
        //
        //  m_pathRepoPath
        //      This is the path that we put our repository file in. If its
        //      not provided, then its the same as the executable. The file
        //      is named CIDCfgSrv.Repository.
        //
        //  m_strAltNSScope
        //      We can take on the command line an alternate name server
        //      scope to put a second binding in, which is useful in many
        //      systems. We'll still use the standard binding, but will add
        //      a second one under the indicated scope. It is set via the
        //      /AltNSScope= parameter.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4MaxClients;
        tCIDLib::EExitCodes     m_eReturn;
        tCIDLib::TIPPortNum     m_ippnListen;
        TEvent*                 m_pevWait;
        TLogSrvLogger*          m_plgrLogSrv;
        TCIDCfgServerImpl*      m_porbsImpl;
        TPathStr                m_pathRepoPath;
        TString                 m_strAltNSScope;
};



