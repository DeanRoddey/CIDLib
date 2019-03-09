//
// FILE NAME: CIDLogSrv_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/28/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the more generic parts of the facility class for the
//  CIDLib Log Server. Some private methods, which implement various specific
//  pieces of the server, are implemented in other files.
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
#include    "CIDLogSrv.hpp"



// ---------------------------------------------------------------------------
//   CLASS: TFacNameSrv
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacNameSrv: Constructors and Destructor
// ---------------------------------------------------------------------------

//
//  Note that we use the same version constants as the toolkit facility. Since
//  they must be used together, this makes sense.
//
TFacCIDLogSrv::TFacCIDLogSrv() :

    TFacility
    (
        L"CIDLogSrv"
        , tCIDLib::EModTypes::Exe
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
    , m_c4MaxClients(0)
    , m_ippnListen(kCIDOrbUC::ippnLogSrvDefPort)
    , m_pevWait(nullptr)
{
}

TFacCIDLogSrv::~TFacCIDLogSrv()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDLogSrv: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFacCIDLogSrv::bHandleSignal(const tCIDLib::ESignals eSig)
{
    //
    //  If it's a Ctrl-C or Ctrl-Break, just trigger the wait event. Ignore
    //  the others. This way, we can be stopped manually when running in
    //  debug mode, but don't get blown away by logoff signals when running
    //  under a service based app shell.
    //
    if ((eSig == tCIDLib::ESignals::CtrlC) || (eSig == tCIDLib::ESignals::Break))
        m_pevWait->Trigger();

    // And return that we handled it
    return kCIDLib::True;
}


tCIDLib::EExitCodes TFacCIDLogSrv::eMainThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let our caller go
    thrThis.Sync();

    //
    //  First thing of all, create our wait event. We make this a named event
    //  so that a controlling shell can shut us down by posting the event.
    //  Indicate that we must create it. If a previous instance of this program
    //  is running, then we will exit back out without doing any damage to the
    //  other instance's state.
    //
    //  By default it's a manual mode event. We want it to stay triggered once
    //  triggered so that if it happens while we are starting up, we'll see it
    //  later when we get to the point of blocking on it.
    //
    try
    {
        m_pevWait = new TEvent
        (
            TResourceName
            (
                kCIDLib::pszResCompany
                , kCIDOrbUC::pszEvLogSrv
                , kCIDOrbUC::pszEvResource
            )
            , tCIDLib::EEventStates::Reset
            , tCIDLib::ECreateActs::CreateIfNew
        );
    }

    catch(const TError& errToCatch)
    {
        if (errToCatch.bCheckEvent( facCIDLib().strName()
                                    , kCIDErrs::errcEv_AlreadyExists))
        {
            return tCIDLib::EExitCodes::InitFailed;
        }
        return tCIDLib::EExitCodes::RuntimeError;
    }

    // And perform the requested action
    tCIDLib::TMsgId  midToLog = kCIDLib::c4MaxCard;
    m_eReturn = tCIDLib::EExitCodes::Normal;
    try
    {
        // Register ourself as a signal handler
        TSignals::c4AddHandler(this);

        // If verbose logging, then indicate that we are starting up
        if (bLogStatus())
        {
            LogMsg
            (
                CID_FILE
                , CID_LINE
                , kLogSMsgs::midStatus_Startup
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::AppStatus
                , strVersion()
            );
        }

        // Parse the parameters
        midToLog = kLogSMsgs::midStatus_ParmParseFailed;
        ParseParms();

        // And do the program setup
        midToLog = kLogSMsgs::midStatus_SetupFailed;
        Setup();

        // If verbose logging, then indicate that startup is complete
        if (bLogStatus())
        {
            LogMsg
            (
                CID_FILE
                , CID_LINE
                , kLogSMsgs::midStatus_StartupComplete
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::AppStatus
            );
        }

        // Block forever on this event until we are let go
        m_pevWait->WaitFor();

        // And do the cleanup
        Cleanup();
    }

    catch(TError& errToCatch)
    {
        // If it hasn't been logged yet, then log it
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            LogEventObj(errToCatch);
        }

        // Log this error as why we fell over
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , midToLog
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );

        // Try to do the cleanup
        Cleanup();
        return tCIDLib::EExitCodes::RuntimeError;
    }

    catch(...)
    {
        // Log this error as why we fell over
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , midToLog
            , tCIDLib::ESeverities::Status
        );

        // Try to do the cleanup
        Cleanup();
        return tCIDLib::EExitCodes::SystemException;
    }

    return m_eReturn;
}


//
//  This is called by our ORB based admin interface handler. We just store
//  the return value he want's us to return, and trigger the event that the
//  main thread is blocked on.
//
tCIDLib::TVoid TFacCIDLogSrv::Shutdown(const tCIDLib::EExitCodes eReturn)
{
    // Store the value we want to return
    m_eReturn = eReturn;

    // Trigger the wait event
    m_pevWait->Trigger();
}

