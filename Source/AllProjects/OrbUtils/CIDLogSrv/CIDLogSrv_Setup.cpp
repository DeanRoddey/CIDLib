//
// FILE NAME: CIDLogSrv_Setup.cpp
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
//  This method implements the private facility methods that are related to
//  program setup.
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
//  TFacCIDLogSrv: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFacCIDLogSrv::ParseParms()
{
    // If verbose logging, then log that we are starting the parms
    if (facCIDLogSrv.bLogStatus())
    {
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , kLogSMsgs::midStatus_StartParms
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );
    }

    TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
    for (; cursParms; ++cursParms)
    {
        TString strCurParm = *cursParms;

        // We know what is by what it starts with
        if (strCurParm.bStartsWithI(L"/Port="))
        {
            // Sets the port that we tell our Orb to listen on
            strCurParm.Cut(0, 6);
            m_ippnListen = strCurParm.c4Val();
        }
         else if (strCurParm.bStartsWithI(L"/Max="))
        {
            // Sets the maximum clients we'll accept at once
            strCurParm.Cut(0, 5);
            m_c4MaxClients = strCurParm.c4Val();
        }
         else if (strCurParm.bStartsWithI(L"/OutPath="))
        {
            // Sets the output path for log files
            strCurParm.Cut(0, 9);
            m_pathLogPath = strCurParm;
        }
         else
        {
            LogMsg
            (
                CID_FILE
                , CID_LINE
                , kLogSErrs::errcParm_UnknownParm
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , strCurParm
            );
        }
    }

    // If no log path was set, then its the program path
    if (m_pathLogPath.bIsEmpty())
        m_pathLogPath = strPath();

    // If verbose logging, then log that we completed the parms
    if (facCIDLogSrv.bLogStatus())
    {
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , kLogSMsgs::midStatus_ParmsComplete
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );
    }
}


tCIDLib::TVoid TFacCIDLogSrv::Setup()
{
    // If verbose logging, then log that we are starting setup
    if (facCIDLogSrv.bLogStatus())
    {
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , kLogSMsgs::midStatus_StartSetup
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );
    }

    //
    //  Lets see if the log path exists. If not, lets try to create it. If
    //  we cannot, then lets give up.
    //
    if (!TFileSys::bIsDirectory(m_pathLogPath))
    {
        try
        {
            TFileSys::MakePath(m_pathLogPath);
        }

        catch(const TError& errToCatch)
        {
            TPopUp::PopUpErr
            (
                errToCatch.strFileName()
                , errToCatch.c4LineNum()
                , facCIDLogSrv.strMsg(kLogSMsgs::midStatus_Title)
                , facCIDLogSrv.strMsg(kLogSMsgs::midStatus_Title2)
                , errToCatch.errcId()
                , errToCatch.errcKrnlId()
                , errToCatch.errcHostId()
                , errToCatch.strErrText()
                , errToCatch.strAuxText()
            );
            TProcess::ExitProcess(tCIDLib::EExitCodes::InitFailed);
        }
    }

    //
    //  Init the client and server side of the Orb. We are a server to
    //  our clients, but a client to other standard services.
    //
    facCIDOrb().InitClient();
    facCIDOrb().InitServer(m_ippnListen, m_c4MaxClients);

    //
    //  Create and register our server object with the Orb, and with
    //  the rebinder.
    //
    m_porbsImpl = new TCIDLogServerImpl;
    facCIDOrb().RegisterObject(m_porbsImpl);
    facCIDOrbUC().RegRebindObj
    (
        m_porbsImpl->ooidThis()
        , TCIDLogSrvServerBase::strBinding
        , L"CIDLib Log Server Logger Object"
    );

    // Register an instance of our core admin implementation
    TCIDCoreAdminImpl* porbsAdmin = new TCIDCoreAdminImpl;
    facCIDOrb().RegisterObject(porbsAdmin);
    facCIDOrbUC().RegRebindObj
    (
        porbsAdmin->ooidThis()
        , TCIDLogSrvServerBase::strAdminBinding
        , L"Log Server Core Admin Object"
    );

    // And start the rebinder now
    facCIDOrbUC().StartRebinder();

    // If verbose logging, then log that we completed setup
    if (facCIDLogSrv.bLogStatus())
    {
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , kLogSMsgs::midStatus_SetupComplete
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );
    }
}

