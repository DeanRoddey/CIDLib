//
// FILE NAME: CIDAppSh_Engine.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/29/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDAppSh_Engine.cpp file, which implements
//  the TAppShEngine class. This is the main class of this facility and each
//  instance of this class is a unique application shell.
//
// CAVEATS/GOTCHAS:
//
//  1)  If you destroy an app shell engine, it will try to shut down the
//      controlled processes by calling the ShutdownProcess() virtual method
//      for each running process. If you don't override this, the default
//      implementation here does nothing.
//
//  2)  Don't use this to manage hundreds of apps. If you need that kind of
//      control, develop something more optimized for that kind of bulk. This
//      is designed to handle more like tens of processes. Mostly its designed
//      for distributed systems which need run a small set of server processes
//      on each host.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TAppShEngine
//  PREFIX: aseng
// ---------------------------------------------------------------------------
class CIDAPPSHEXP TAppShEngine : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TAppShEngine();

        ~TAppShEngine();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ActivateAll();

        tCIDLib::TVoid AddApp
        (
                    TAppShRecord* const     pasrToAdd
        );

        tCIDLib::TCard4 c4QueryAppsInfo
        (
                    tCIDLib::TKVPList&      colToFill
        );

        tCIDLib::TVoid Cycle
        (
            const   tCIDLib::TCard4         c4WaitFor
        );

        tCIDLib::TVoid StartAll();

        tCIDLib::TVoid StopAll
        (
            const   tCIDLib::TCard4         c4WaitFor
        );

        tCIDLib::TVoid StopEngine
        (
            const   tCIDLib::TCard4         c4WaitFor
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bCheckShutdownComplete
        (
                    TAppShRecord&           asrTarget
            ,       tCIDLib::EExitCodes&    eToFill
        );

        virtual tCIDLib::TVoid Exited
        (
                    TAppShRecord&           asrExited
            , const tCIDLib::EExitCodes     eCode
        );

        virtual tCIDLib::TVoid IdleCallback();

        virtual tCIDLib::TVoid Restarted
        (
                    TAppShRecord&           asrRestarted
        );

        virtual tCIDLib::TVoid ShellException
        (
                    TError&                 errThatOccured
            ,       TAppShRecord&           asrInvolved
        );

        virtual tCIDLib::TVoid ShellMessage
        (
            const   TString&                strMsg
        );

        virtual tCIDLib::TVoid ShellUnknownException
        (
                    TAppShRecord&           asrInvolved
        );

        virtual tCIDLib::TVoid Started
        (
                    TAppShRecord&           asrRestarted
        );

        virtual tCIDLib::TVoid StartShutdown
        (
                    TAppShRecord&           asrTarget
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TAppShEngine(const TAppShEngine&);
        tCIDLib::TVoid operator=(const TAppShEngine&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eMonThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::TVoid ShutdownAllProcesses
        (
            const   tCIDLib::TCard4         c4WaitFor
            , const tCIDLib::TBoolean       bMarkInactive
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colRecs
        //      This is our list of app shell records. Each one represents
        //      an external process we are managing. The derived class creates
        //      them via the pasrMakeNew() virtual method, which allows them
        //      to store extra data. But we provide a default implementation
        //      that creates a basic TAppShRecord object.
        //
        //  m_mtxSync
        //      We have to syncrhonize between the monitor thread and the
        //      client threads that can in to add/remove/modify records.
        //
        //  m_thrMonitor
        //      This is an internal thread that just runs through the list of
        //      processes on a regular basis and, for the ones that are
        //      supposed to be running, checks them to see if they've died
        //      and restarts them if needed.
        // -------------------------------------------------------------------
        TRefVector<TAppShRecord>    m_colRecs;
        TMutex                      m_mtxSync;
        TThread                     m_thrMonitor;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TAppShEngine,TObject)
};

#pragma CIDLIB_POPPACK

