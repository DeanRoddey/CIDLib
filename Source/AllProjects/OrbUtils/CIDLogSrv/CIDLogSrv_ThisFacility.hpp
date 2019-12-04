//
// FILE NAME: CIDLogSrv_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/28/2001
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
//  This is the header for the facility class for the log server. This app
//  publishes an IDL based interface via which other programs can log their
//  errors and messages remotely.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//   CLASS: TFacCIDLogSrv
//  PREFIX: fac
// ---------------------------------------------------------------------------
class TFacCIDLogSrv : public TFacility, public MSignalHandler
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDLogSrv();

        TFacCIDLogSrv(const TFacCIDLogSrv&) = delete;
        TFacCIDLogSrv(TFacCIDLogSrv&&) = delete;

        ~TFacCIDLogSrv();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDLogSrv& operator=(const TFacCIDLogSrv&) = delete;
        TFacCIDLogSrv& operator=(TFacCIDLogSrv&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bHandleSignal
        (
            const   tCIDLib::ESignals       eSignal
        )   final;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eMainThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

         const TString& strLogPath() const
        {
            return m_pathLogPath;
        }

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
        //  m_porbsImpl
        //      A pointer to our implementation object. We don't own it, the
        //      Orb does once we register the object. But we do want to keep
        //      our own (typed) pointer to it, for convenience.
        //
        //  m_pathLogPath
        //      This is the path that we put our log file in. If its not
        //      provided, then its the same as the executable. The file is
        //      named CIDLogSrv.LogData.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4MaxClients;
        tCIDLib::EExitCodes     m_eReturn;
        tCIDLib::TIPPortNum     m_ippnListen;
        TEvent*                 m_pevWait;
        TCIDLogServerImpl*      m_porbsImpl;
        TPathStr                m_pathLogPath;
};

