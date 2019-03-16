//
// FILE NAME: TestService.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/08/2002
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
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
#include    "CIDLib.hpp"
#include    "CIDKernel_ServiceWin32.hpp"


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc
(
        TThread&            thrThis
    ,   tCIDLib::TVoid*     pData
);


static TFacility    facTestService
(
    L"TestService"
    , tCIDLib::EModTypes::Exe
    , kCIDLib::c4MajVersion
    , kCIDLib::c4MinVersion
    , kCIDLib::c4Revision
    , tCIDLib::EModFlags::None
);


// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid LogServiceMsg(const tCIDLib::TCh* const pszMsg)
{
    facTestService.LogMsg(CID_FILE, CID_LINE, pszMsg, tCIDLib::ESeverities::Status);
}



// ---------------------------------------------------------------------------
//   CLASS: TTestService
//  PREFIX: srvh
// ---------------------------------------------------------------------------
class TTestService : public TWin32ServiceHandler
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTestService();

        ~TTestService();


        // -------------------------------------------------------------------
        //  Inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bStartServiceThread();

        tCIDLib::TBoolean bStopServiceThread();

        tCIDLib::TBoolean bSuspendRequest();

        tCIDLib::TVoid ServiceOp
        (
            const   tCIDKernel::EWSrvOps    eOperation
        );

        tCIDLib::TVoid ServiceOp
        (
            const   tCIDKernel::EWSrvPwrOps ePowerOp
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        TThread m_thrService;


        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TTestService(const TTestService&);
        tCIDLib::TVoid operator=(const TTestService&);
};


TTestService::TTestService() :

    m_thrService(L"TestServiceThread", eMainThreadFunc)
{
}

TTestService::~TTestService()
{
}


// Our state is 'start pending' at this point
tCIDLib::TBoolean TTestService::bStartServiceThread()
{
    if (!TKrnlWin32Service::bSetState(tCIDKernel::EWSrvStates::Start_Pending, 5000, 0))
        return kCIDLib::False;

    LogServiceMsg(L"Starting service thread");
    try
    {
        m_thrService.Start();
        if (!TKrnlWin32Service::bSetState(tCIDKernel::EWSrvStates::Running, 0, 0))
            return kCIDLib::False;
    }

    catch(TError& errToCatch)
    {
        if (!TKrnlWin32Service::bSetState(tCIDKernel::EWSrvStates::Stopped, 0, 1))
            return kCIDLib::False;

        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TTestService::bStopServiceThread()
{
    // Set a stop pending state
    if (!TKrnlWin32Service::bPleaseWait(tCIDKernel::EWSrvStates::Stop_Pending))
        return kCIDLib::False;

    // Count how many times we wait. We don't want to wait too long
    tCIDLib::TCard4 c4Count = 0;

    // Ask it to shut down and then loop while we wait
    m_thrService.ReqShutdownSync();
    while (kCIDLib::True)
    {
        //
        //  In each time through, wait for a second for it to die. If it
        //  doesn't, then tell the service manager to wait a bit more.
        //
        try
        {
            m_thrService.eWaitForDeath(1000);
            break;
        }

        catch(TError& errToCatch)
        {
            if (!errToCatch.bLogged())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            TKrnlWin32Service::bPleaseWait(tCIDKernel::EWSrvStates::Stop_Pending);
            c4Count++;
            if (c4Count == 25)
                return kCIDLib::False;
        }
    }

    // It worked, so set our state as stopped
    TKrnlWin32Service::bSetState(tCIDKernel::EWSrvStates::Stopped, 0, 0);

    return kCIDLib::True;
}


tCIDLib::TBoolean TTestService::bSuspendRequest()
{
    // Let a suspend happen
    return kCIDLib::True;
}


tCIDLib::TVoid TTestService::ServiceOp(const tCIDKernel::EWSrvOps eOperation)
{
    LogServiceMsg(L"Got service request");
}


tCIDLib::TVoid TTestService::ServiceOp(const tCIDKernel::EWSrvPwrOps ePowerOp)
{
    if (ePowerOp == tCIDKernel::EWSrvPwrOps::Suspend)
    {
        LogServiceMsg(L"Got suspend request");
    }
     else if (ePowerOp == tCIDKernel::EWSrvPwrOps::Resume)
    {
        LogServiceMsg(L"Got resume request");
    }
}



// ---------------------------------------------------------------------------
//  Do the magic main module code
// ---------------------------------------------------------------------------
CIDLib_MainService(L"TestCIDService", TTestService, kCIDLib::False)


// ----------------------------------------------------------------------------
//  Functions for local use
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    LogServiceMsg(L"Service thread startup");
    while (kCIDLib::True)
    {
        try
        {
            if (thrThis.bCheckShutdownRequest())
            {
                LogServiceMsg(L"Saw shutdown request");
                break;
            }
            TThread::Sleep(1000);
        }

        // Catch any CIDLib runtime errors
        catch(const TError& errToCatch)
        {
        }

        //
        //  Kernel errors should never propogate out of CIDLib, but test
        //  for them here just in case.
        //
        catch(const TKrnlError& kerrToCatch)
        {
        }

        // Catch a general exception
        catch(...)
        {
        }
    }

    return tCIDLib::EExitCodes::Normal;
}


