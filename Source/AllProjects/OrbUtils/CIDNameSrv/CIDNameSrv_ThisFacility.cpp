//
// FILE NAME: CIDIDL_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/09/2001
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
//  CIDLib Naming Service. Some private methods, which implement various
//  specific pieces of the server, are implemented in other files.
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
#include    "CIDNameSrv.hpp"



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
TFacCIDNameSrv::TFacCIDNameSrv() :

    TFacility
    (
        L"CIDNameSrv"
        , tCIDLib::EModTypes::Exe
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
    , m_bNoLease(kCIDLib::False)
    , m_c4MaxClients(0)
    , m_eReturn(tCIDLib::EExitCodes::InitFailed)
    , m_ippnListen(kCIDOrbUC::ippnNameSrvDefPort)
    , m_pevWait(nullptr)
    , m_porbsCache(nullptr)
{
}

TFacCIDNameSrv::~TFacCIDNameSrv()
{
    delete m_pevWait;
    m_pevWait = nullptr;

    // NOTE: We don't own the cache object, the Orb does!
}


// ---------------------------------------------------------------------------
//  TFacCIDNameSrv: Public, inherited virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFacCIDNameSrv::bHandleSignal(const tCIDLib::ESignals eSig)
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
//  TFacCIDNameSrv: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes TFacCIDNameSrv::eMainThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let our caller go
    thrThis.Sync();

    //
    //  First thing of all, create our wait event. If a previous instance of
    //  this program is running, then we will exist back out without doing
    //  any damage to the other instance's state.
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
                , kCIDOrbUC::pszEvNameSrv
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

    m_eReturn = tCIDLib::EExitCodes::Normal;
    try
    {
        // Register ourself as a signal handler
        TSignals::c4AddHandler(this);

        // Parse the parameters
        ParseParms();

        //
        //  Ok, lets lets crank up the server side ORB. Note that we don't
        //  need any client side support. We are end of the line. Everyone
        //  is a client of us, but there's no one that we talk to as a
        //  client.
        //
        facCIDOrb().InitServer(m_ippnListen, m_c4MaxClients);

        //
        //  Lets create our server side object instance and register it.
        //  Note that we keep a pointer of our own, so that we can control
        //  it, but the Orb owns it.
        //
        //  The name server object is special, in that it must have a fixed
        //  instance id for all time (because its the first object that all
        //  clients must be able to find.) So we create a known object id and
        //  pass that in for it to use when constructing itself.
        //
        //  Normally, servers figure out what local IP address to register
        //  by looking at what local IP they got bound to when they contact
        //  the name server through this special object id. But of course we
        //  are the name server, so we do a name resolution on ourself.
        //
        TOrbObjId ooidNS
        (
            TCIDNameSrvServerBase::strInterfaceId
            , kCIDOrbUC::pszNSInstanceId
            , facCIDSock().strIPHostName()
            , m_ippnListen
            , L"TCIDNameSrvClientProxy"
        );
        m_porbsCache = new TCIDNameServerImpl(ooidNS, m_bNoLease);
        facCIDOrb().RegisterObject(m_porbsCache, tCIDLib::EAdoptOpts::Adopt);

        // Register an instance of our core admin implementation
        m_porbsCache->CreateScopePath
        (
            TCIDNameSrvServerBase::strAdminScope
            , L"CIDLib Name Server Scope"
        );

        //
        //  We mark this one permanent so it won't be removed by the
        //  landlord thread.
        //
        TCIDCoreAdminImpl* porbsAdmin = new TCIDCoreAdminImpl();
        facCIDOrb().RegisterObject(porbsAdmin, tCIDLib::EAdoptOpts::Adopt);
        m_porbsCache->BindObj
        (
            TCIDNameSrvServerBase::strAdminBinding
            , L"CIDLib Name Server Core Admin Object"
            , TString::strEmpty()
            , TString::strEmpty()
            , TString::strEmpty()
            , TString::strEmpty()
            , porbsAdmin->ooidThis()
        );
        m_porbsCache->MarkPermanent(TCIDNameSrvServerBase::strAdminBinding);

        //
        //  Block forever on this event until we are let go. We make this a
        //  named event so that, worst case, if a controlling shell cannot get
        //  to us via the admin interface, it can post the event.
        //
        m_pevWait->Reset();
        m_pevWait->WaitFor();

        // Deregister our admin object. Tell it not to throw if not found
        m_porbsCache->RemoveBinding
        (
            TCIDNameSrvServerBase::strAdminBinding
            , kCIDLib::False
        );
    }

    catch(TError& errToCatch)
    {
        // If its not been logged, then log it
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            LogEventObj(errToCatch);
        }

        // Log why we fell over
        LogMsg
        (
            CID_FILE
            , CID_LINE
            , kNSMsgs::midStatus_CIDLibExcept
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
            , strMsg(kNSMsgs::midStatus_ExitingApp)
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
            , kNSMsgs::midStatus_UnknownExcept
            , tCIDLib::ESeverities::Status
            , tCIDLib::EErrClasses::AppStatus
            , strMsg(kNSMsgs::midStatus_ExitingApp)
        );
        m_eReturn = tCIDLib::EExitCodes::SystemException;
    }

    try
    {
        facCIDOrb().Terminate();
    }

    catch(...)
    {
        // Not much we can do here
    }

    return m_eReturn;
}


//
//  This is called by our ORB based admin interface handler. We just store
//  the return value he want's us to return, and trigger the event that the
//  main thread is blocked on.
//
tCIDLib::TVoid TFacCIDNameSrv::Shutdown(const tCIDLib::EExitCodes eReturn)
{
    // Store the value we want to return
    m_eReturn = eReturn;

    // Trigger the wait event
    m_pevWait->Trigger();
}


// ---------------------------------------------------------------------------
//  TFacCIDNameSrv: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFacCIDNameSrv::ParseParms()
{
    TSysInfo::TCmdLineCursor cursParms = TSysInfo::cursCmdLineParms();
    for (; cursParms; ++cursParms)
    {
        TString strCurParm = *cursParms;

        // We know what is by what it starts with
        if (strCurParm.bStartsWithI(L"/Port="))
        {
            strCurParm.Cut(0, 6);
            m_ippnListen = strCurParm.c4Val();
        }
         else if (strCurParm.bStartsWithI(L"/Max="))
        {
            strCurParm.Cut(0, 5);
            m_c4MaxClients = strCurParm.c4Val();
        }
         else if (strCurParm.bCompareI(L"/NoLease"))
        {
            m_bNoLease = kCIDLib::True;
        }
         else
        {
            LogMsg
            (
                CID_FILE
                , CID_LINE
                , kNSErrs::errcParm_UnknownParm
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , strCurParm
            );
        }
    }
}

