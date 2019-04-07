//
// FILE NAME: CIDCfgSrv_ThisFacility.cpp
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
//  This file implements the more generic parts of the facility class for the
//  CIDLib Cfg Server. Some private methods, which implement various specific
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
#include    "CIDCfgSrv.hpp"



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
TFacCIDCfgSrv::TFacCIDCfgSrv() :

    TFacility
    (
        L"CIDCfgSrv"
        , tCIDLib::EModTypes::Exe
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
    , m_c4MaxClients(0)
    , m_ippnListen(kCIDOrbUC::ippnCfgSrvDefPort)
    , m_pevWait(nullptr)
    , m_plgrLogSrv(nullptr)
    , m_porbsImpl(nullptr)
{
}

TFacCIDCfgSrv::~TFacCIDCfgSrv()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDCfgSrv: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFacCIDCfgSrv::bHandleSignal(const tCIDLib::ESignals eSig)
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


// ---------------------------------------------------------------------------
//  TFacCIDCfgSrv: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes TFacCIDCfgSrv::eMainThread(TThread& thrThis, tCIDLib::TVoid*)
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
    //  The last parm indicate that it's not a manual mode event.
    //
    try
    {
        m_pevWait = new TEvent
        (
            TResourceName
            (
                kCIDLib::pszResCompany
                , kCIDOrbUC::pszEvCfgSrv
                , kCIDOrbUC::pszEvResource
            )
            , tCIDLib::EEventStates::Reset
            , tCIDLib::ECreateActs::CreateIfNew
            , kCIDLib::False
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
                , kCfgSrvMsgs::midStatus_Startup
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::AppStatus
                , strVersion()
            );
        }

        // Parse the parameters
        midToLog = kCfgSrvMsgs::midStatus_ParmParseFailed;
        ParseParms();

        // And do the program setup
        midToLog = kCfgSrvMsgs::midStatus_SetupFailed;
        Setup();

        // If verbose logging, then indicate that startup is complete
        if (bLogStatus())
        {
            LogMsg
            (
                CID_FILE
                , CID_LINE
                , kCfgSrvMsgs::midStatus_StartupComplete
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::AppStatus
            );
        }

        // Block forever on this event until we are let go
        m_pevWait->WaitFor();
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
        m_eReturn = tCIDLib::EExitCodes::RuntimeError;
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
        m_eReturn = tCIDLib::EExitCodes::SystemException;
    }

    // And do the cleanup
    try
    {
        Cleanup();
    }

    catch(...)
    {
        // Nothing we can do at this point
    }
    return m_eReturn;
}


const TString& TFacCIDCfgSrv::strRepoPath() const
{
    return m_pathRepoPath;
}


tCIDLib::TVoid TFacCIDCfgSrv::Shutdown(const tCIDLib::EExitCodes eReturn)
{
    // Store the value we want to return
    m_eReturn = eReturn;

    // Trigger the wait event
    m_pevWait->Trigger();
}

