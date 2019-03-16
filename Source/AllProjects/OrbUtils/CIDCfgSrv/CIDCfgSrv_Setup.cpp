//
// FILE NAME: CIDCfgSrv_Setup.cpp
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
#include    "CIDCfgSrv.hpp"


// ---------------------------------------------------------------------------
//  TFacCIDCfgSrv: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFacCIDCfgSrv::ParseParms()
{
    // If verbose logging, then log that we are starting the parms
    if (facCIDCfgSrv.bLogStatus())
    {
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , kCfgSrvMsgs::midStatus_StartParms
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
            // Sets the port that we tell our ORB to listen on
            strCurParm.Cut(0, 6);
            m_ippnListen = strCurParm.c4Val();
        }
         else if (strCurParm.bStartsWithI(L"/Max="))
        {
            // Sets the maximum clients we'll accept at once
            strCurParm.Cut(0, 5);
            m_c4MaxClients = strCurParm.c4Val();
        }
         else if (strCurParm.bStartsWithI(L"/RepoPath="))
        {
            // Sets the output path for log files
            strCurParm.Cut(0, 10);
            m_pathRepoPath = strCurParm;
        }
         else if (strCurParm.bStartsWithI(L"/AltNSScope="))
        {
            strCurParm.Cut(0, 12);
            m_strAltNSScope = strCurParm;
        }
         else
        {
            LogMsg
            (
                CID_FILE
                , CID_LINE
                , kCfgSrvErrs::errcParm_UnknownParm
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , strCurParm
            );
        }
    }

    // If no log path was set, then its the program path
    if (m_pathRepoPath.bIsEmpty())
        m_pathRepoPath = strPath();

    // If verbose logging, then log that we completed the parms
    if (facCIDCfgSrv.bLogStatus())
    {
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , kCfgSrvMsgs::midStatus_ParmsComplete
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );
    }
}


tCIDLib::TVoid TFacCIDCfgSrv::Setup()
{
    // If verbose logging, then log that we are starting setup
    if (facCIDCfgSrv.bLogInfo())
    {
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , kCfgSrvMsgs::midStatus_StartSetup
            , tCIDLib::ESeverities::Info
            , tCIDLib::EErrClasses::AppStatus
        );
    }


    //
    //  The very first thing we want to do is the most fundamental boot-
    //  strapping. We need to crank up the ORB client support (so that
    //  we can find the name server.) And then install a log server
    //  logger, which will allow anything that goes wrong after that to
    //  go to the log server.
    //
    facCIDOrb().InitClient();

    //
    //  The next thing we want to do is to install a log server logger. We
    //  just use the standard one that's provided by CIDLib. It logs to
    //  the standard CIDLib log server, and uses a local file for fallback
    //  if it cannot connect.
    //
    //  NOTE:   We give it our Exe directory as the place to put its temp
    //          local log files (which are used if we lose the connection
    //          to the log server.)
    //
    m_plgrLogSrv = new TLogSrvLogger(strPath());
    TModule::InstallLogger(m_plgrLogSrv);

    //
    //  Init the client and server side of the ORB. We are a server to
    //  our clients, but a client to other standard services.
    //
    facCIDOrb().InitServer(m_ippnListen, m_c4MaxClients);

    //
    //  Create and register our server object with the ORB. This will cause
    //  a new repository to be created, or the existing one opened, and a
    //  new index created.
    //
    m_porbsImpl = new TCIDCfgServerImpl;
    facCIDOrb().RegisterObject(m_porbsImpl);

    //
    //  Store our object on the auto rebinder. Tell it to do a deferred
    //  registration.
    //
    TString strTmp(TCIDCfgSrvServerBase::strBinding);
    strTmp.eReplaceToken(facCIDSock().strIPHostName(), L'h');
    facCIDOrbUC().RegRebindObj
    (
        m_porbsImpl->ooidThis()
        , strTmp
        , L"CIDLib Config Respository Object"
    );

    //
    //  And if we were given a secondary scope, then add another binding of
    //  the same object under it.
    //
    if (!m_strAltNSScope.bIsEmpty())
    {
        strTmp = m_strAltNSScope;
        if (strTmp.chLast() != kCIDLib::chForwardSlash)
            strTmp.Append(kCIDLib::chForwardSlash);
        strTmp.Append(L"CfgRepoObj");
        facCIDOrbUC().RegRebindObj
        (
            m_porbsImpl->ooidThis()
            , strTmp
            , L"Alt CIDLib Config Respository Object"
        );
    }

    //
    //  And create our core admin impl object, and register it with the ORB
    //  and auto rebinder. Use deferred binding mode.
    //
    TCIDCoreAdminImpl* porbsAdmin = new TCIDCoreAdminImpl;
    facCIDOrb().RegisterObject(porbsAdmin);
    strTmp = TCIDCfgSrvServerBase::strAdminBinding;
    strTmp.eReplaceToken(facCIDSock().strIPHostName(), L'h');
    facCIDOrbUC().RegRebindObj
    (
        porbsAdmin->ooidThis()
        , strTmp
        , L"Cfg Server Core Admin Object"
    );

    // And start the rebinder now
    facCIDOrbUC().StartRebinder();

    // If verbose logging, then log that we completed setup
    if (facCIDCfgSrv.bLogStatus())
    {
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , kCfgSrvMsgs::midStatus_SetupComplete
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
        );
    }
}

