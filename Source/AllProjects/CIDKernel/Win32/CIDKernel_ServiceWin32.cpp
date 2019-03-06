//
// FILE NAME: CIDKernel_ServiceWin32.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/07/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides the Windows specific namespace that supports Windows
//  service functionality.
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
#include    "CIDKernel_.hpp"
#include    "CIDKernel_ServiceWin32.hpp"
#include    <Dbt.h>


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDKernel_SvcWin32
{
    // -----------------------------------------------------------------------
    //  We can be started in debug mode, which is useful for debugging the
    //  client app, since its hard to debug a service. This is set in the
    //  init method according to the debug parm.
    // -----------------------------------------------------------------------
    tCIDLib::TBoolean           bDebugMode = kCIDLib::False;


    // -----------------------------------------------------------------------
    //  The handler, if any, that the client app has installed
    // -----------------------------------------------------------------------
    TWin32ServiceHandler*       psrvhHandler = 0;


    // -----------------------------------------------------------------------
    //  Our service related status data
    // -----------------------------------------------------------------------
    SERVICE_STATUS              SrvStatus;
    SERVICE_STATUS_HANDLE       hService;
    HDEVNOTIFY                  hNotify;


    // -----------------------------------------------------------------------
    //  The name of the service we were given in the init
    // -----------------------------------------------------------------------
    const tCIDLib::TCh*         pszServiceName;


    // -----------------------------------------------------------------------
    //  The handle to a log file we use to log core service events. This is set
    //  up during service init. If we can't open it, it's null and nothing will
    //  be logged.
    // -----------------------------------------------------------------------
    HANDLE                      hflDbgLog = INVALID_HANDLE_VALUE;
}



// ---------------------------------------------------------------------------
//  This is from the file system section, which allows us to call over to it
//  when we get volume change notifications.
// ---------------------------------------------------------------------------
extern tCIDCtrls::TWndHandle CIDKrnlFS_NotHandle();



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------
static tCIDLib::TCard4 c4XlatSrvState(const tCIDKernel::EWSrvStates eToXlat)
{
    tCIDLib::TCard4 c4Res;
    switch(eToXlat)
    {
        case tCIDKernel::EWSrvStates::Continue_Pending :
            c4Res = SERVICE_CONTINUE_PENDING;
            break;

        case tCIDKernel::EWSrvStates::Pause_Pending :
            c4Res = SERVICE_PAUSE_PENDING;
            break;

        case tCIDKernel::EWSrvStates::Paused :
            c4Res = SERVICE_PAUSED;
            break;

        case tCIDKernel::EWSrvStates::Running :
            c4Res = SERVICE_RUNNING;
            break;

        case tCIDKernel::EWSrvStates::Start_Pending :
            c4Res = SERVICE_START_PENDING;
            break;

        case tCIDKernel::EWSrvStates::Stop_Pending :
            c4Res = SERVICE_STOP_PENDING;
            break;

        case tCIDKernel::EWSrvStates::Stopped :
            c4Res = SERVICE_STOPPED;
            break;

        default :
            c4Res = kCIDLib::c4MaxCard;
            break;
    }
    return c4Res;
}


static tCIDLib::TCard4 c4XlatOpcode(const tCIDKernel::EWSrvOps eToXlat)
{
    tCIDLib::TCard4 c4Ret;
    switch(eToXlat)
    {
        case tCIDKernel::EWSrvOps::Continue :
            c4Ret = SERVICE_CONTROL_CONTINUE;
            break;

        case tCIDKernel::EWSrvOps::Interrogate :
            c4Ret = SERVICE_CONTROL_INTERROGATE;
            break;

        case tCIDKernel::EWSrvOps::NetBindAdd :
            c4Ret = SERVICE_CONTROL_NETBINDADD;
            break;

        case tCIDKernel::EWSrvOps::NetBindDisable :
            c4Ret = SERVICE_CONTROL_NETBINDDISABLE;
            break;

        case tCIDKernel::EWSrvOps::NetBindEnable :
            c4Ret = SERVICE_CONTROL_NETBINDENABLE;
            break;

        case tCIDKernel::EWSrvOps::NetBindRemove :
            c4Ret = SERVICE_CONTROL_NETBINDREMOVE;
            break;

        case tCIDKernel::EWSrvOps::ParamChange :
            c4Ret = SERVICE_CONTROL_PARAMCHANGE;
            break;

        case tCIDKernel::EWSrvOps::Pause :
            c4Ret = SERVICE_CONTROL_PAUSE;
            break;

        case tCIDKernel::EWSrvOps::PowerEvent :
            c4Ret = SERVICE_CONTROL_POWEREVENT;
            break;

        case tCIDKernel::EWSrvOps::Shutdown :
            c4Ret = SERVICE_CONTROL_SHUTDOWN;
            break;

        case tCIDKernel::EWSrvOps::Stop :
            c4Ret = SERVICE_CONTROL_STOP;
            break;

        default :
            c4Ret = kCIDLib::c4MaxCard;
            break;
    };
    return c4Ret;
}


static tCIDKernel::EWSrvPwrOps eXlatPowerOp(const tCIDLib::TCard4 c4EvType)
{
    tCIDKernel::EWSrvPwrOps ePwrOp;
    switch(c4EvType)
    {
        case PBT_APMBATTERYLOW :
            ePwrOp = tCIDKernel::EWSrvPwrOps::BatteryLow;
            break;

        case PBT_APMPOWERSTATUSCHANGE :
            ePwrOp = tCIDKernel::EWSrvPwrOps::StatusChange;
            break;

        case PBT_APMQUERYSUSPENDFAILED :
            ePwrOp = tCIDKernel::EWSrvPwrOps::QueryFailed;
            break;

        case PBT_APMRESUMEAUTOMATIC :
            ePwrOp = tCIDKernel::EWSrvPwrOps::ResumeAuto;
            break;

        case PBT_APMRESUMECRITICAL :
            ePwrOp = tCIDKernel::EWSrvPwrOps::ResumeCritical;
            break;

        case PBT_APMRESUMESUSPEND :
            ePwrOp = tCIDKernel::EWSrvPwrOps::Resume;
            break;

        case PBT_APMSUSPEND :
            ePwrOp = tCIDKernel::EWSrvPwrOps::Suspend;
            break;

        default :
            ePwrOp = tCIDKernel::EWSrvPwrOps::Unknown;
            break;
    };
    return ePwrOp;
}


static tCIDKernel::EWSrvOps eXlatOpcode(const tCIDLib::TCard4 c4ToXlat)
{
    tCIDKernel::EWSrvOps eOp;
    switch(c4ToXlat)
    {
        case SERVICE_CONTROL_CONTINUE :
            eOp = tCIDKernel::EWSrvOps::Continue;
            break;

        case SERVICE_CONTROL_INTERROGATE :
            eOp = tCIDKernel::EWSrvOps::Interrogate;
            break;

        case SERVICE_CONTROL_NETBINDADD :
            eOp = tCIDKernel::EWSrvOps::NetBindAdd;
            break;

        case SERVICE_CONTROL_NETBINDDISABLE :
            eOp = tCIDKernel::EWSrvOps::NetBindDisable;
            break;

        case SERVICE_CONTROL_NETBINDENABLE :
            eOp = tCIDKernel::EWSrvOps::NetBindEnable;
            break;

        case SERVICE_CONTROL_NETBINDREMOVE :
            eOp = tCIDKernel::EWSrvOps::NetBindRemove;
            break;

        case SERVICE_CONTROL_PARAMCHANGE :
            eOp = tCIDKernel::EWSrvOps::ParamChange;
            break;

        case SERVICE_CONTROL_PAUSE :
            eOp = tCIDKernel::EWSrvOps::Pause;
            break;

        case SERVICE_CONTROL_POWEREVENT :
            eOp = tCIDKernel::EWSrvOps::PowerEvent;
            break;

        case SERVICE_CONTROL_SHUTDOWN :
            eOp = tCIDKernel::EWSrvOps::Shutdown;
            break;

        case SERVICE_CONTROL_STOP :
            eOp = tCIDKernel::EWSrvOps::Stop;
            break;

        default :
            eOp = tCIDKernel::EWSrvOps::Unknown;
            break;
    };
    return eOp;
}


static tCIDKernel::EWSrvStates eXlatSrvState(const tCIDLib::TCard4 c4ToXlat)
{
    tCIDKernel::EWSrvStates eRes;
    switch(c4ToXlat)
    {
        case SERVICE_CONTINUE_PENDING :
            eRes = tCIDKernel::EWSrvStates::Continue_Pending;
            break;

        case SERVICE_PAUSE_PENDING :
            eRes = tCIDKernel::EWSrvStates::Pause_Pending;
            break;

        case SERVICE_PAUSED :
            eRes = tCIDKernel::EWSrvStates::Paused;
            break;

        case SERVICE_RUNNING :
            eRes = tCIDKernel::EWSrvStates::Running;
            break;

        case SERVICE_START_PENDING :
            eRes = tCIDKernel::EWSrvStates::Start_Pending;
            break;

        case SERVICE_STOP_PENDING :
            eRes = tCIDKernel::EWSrvStates::Stop_Pending;
            break;

        case SERVICE_STOPPED :
            eRes =  tCIDKernel::EWSrvStates::Stopped;
            break;

        default :
            eRes = tCIDKernel::EWSrvStates::Stopped;
            break;
    }
    return eRes;
}


static const tCIDLib::TCh* pszXlatPowerOp(const tCIDKernel::EWSrvPwrOps ePwrOp)
{
    switch(ePwrOp)
    {
        case tCIDKernel::EWSrvPwrOps::BatteryLow :
            return L"Battery Low";

        case tCIDKernel::EWSrvPwrOps::StatusChange :
            return L"Status Change";

        case tCIDKernel::EWSrvPwrOps::QueryFailed :
            return L"Query Failed";

        case tCIDKernel::EWSrvPwrOps::ResumeAuto :
            return L"Resume Auto";

        case tCIDKernel::EWSrvPwrOps::ResumeCritical :
            return L"Resume Critical";

        case tCIDKernel::EWSrvPwrOps::Resume :
            return L"Resume";

        case tCIDKernel::EWSrvPwrOps::Suspend :
            return L"Suspend";

        default :
            break;
    };

    return L"Unknown";
}


static const tCIDLib::TCh* pszXlatSrvOp(const tCIDKernel::EWSrvOps eToXlat)
{
    switch(eToXlat)
    {
        case tCIDKernel::EWSrvOps::Continue :
            return L"Continue";

        case tCIDKernel::EWSrvOps::Interrogate :
            return L"Interrogate";

        case tCIDKernel::EWSrvOps::NetBindAdd :
            return L"NetBindAdd";

        case tCIDKernel::EWSrvOps::NetBindDisable :
            return L"NetBindDisable";

        case tCIDKernel::EWSrvOps::NetBindEnable :
            return L"NetBindEnable";

        case tCIDKernel::EWSrvOps::NetBindRemove :
            return L"NetBindRemove";

        case tCIDKernel::EWSrvOps::ParamChange :
            return L"ParamChange";

        case tCIDKernel::EWSrvOps::Pause :
            return L"Pause";

        case tCIDKernel::EWSrvOps::PowerEvent :
            return L"PowerEvent";

        case tCIDKernel::EWSrvOps::Shutdown :
            return L"Shutdown";

        case tCIDKernel::EWSrvOps::Stop :
            return L"Stop";

        default :
            break;
    };

    return L"Unknown";
}


static SC_HANDLE hOpenManager()
{
    SC_HANDLE hSCM = 0;
    hSCM = ::OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
    if (!hSCM)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return 0;
    }
    return hSCM;
}


static tCIDLib::TVoid LogDebugStr(const tCIDLib::TCh* const pszStr)
{
    if (CIDKernel_SvcWin32::hflDbgLog != INVALID_HANDLE_VALUE)
    {
        static const tCIDLib::TCard1 c1LF = 0xA;
        DWORD c4BytesWritten;

        // Convert to local code page and write
        tCIDLib::TSCh* pszToWrite = TRawStr::pszConvert(pszStr);
        ::WriteFile
        (
            CIDKernel_SvcWin32::hflDbgLog
            , pszToWrite
            , TRawStr::c4StrLen(pszToWrite)
            , &c4BytesWritten
            , 0
        );
        delete [] pszToWrite;

        // And add a new line
        ::WriteFile(CIDKernel_SvcWin32::hflDbgLog, &c1LF, 1, &c4BytesWritten, 0);
    }
}


static tCIDLib::TVoid LogDebugErr(const tCIDLib::TCh* const pszMsg)
{
    const tCIDLib::TCard4 c4BufSz = 2048;
    tCIDLib::TCh achErr[c4BufSz + 1];

    TRawStr::CopyStr(achErr, pszMsg, c4BufSz);
    TRawStr::CatStr(achErr, L". Error Code=", c4BufSz);

    const tCIDLib::TCard4 c4Err = ::GetLastError();
    TKrnlError::SetLastHostError(c4Err);

    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(achErr);
    TRawStr::bFormatVal(c4Err, &achErr[c4Len], c4BufSz - c4Len);

    LogDebugStr(achErr);
}


static tCIDLib::TVoid LogDebugExcept(const tCIDLib::TCh* const pszText)
{
    const tCIDLib::TCard4 c4BufSz = 2048;
    tCIDLib::TCh achErr[c4BufSz + 1];
    TRawStr::CopyStr(achErr, pszText, c4BufSz);
    TRawStr::CatStr(achErr, L". Error=", c4BufSz);
    TRawStr::CatStr(achErr, pszText, c4BufSz);

    LogDebugStr(achErr);
}


static tCIDLib::TVoid LogDebugMsg(const tCIDLib::TCh* const pszMsg)
{
    LogDebugStr(pszMsg);
}


static tCIDLib::TVoid LogDebugPowerOp(const tCIDKernel::EWSrvPwrOps ePwrOp)
{
    const tCIDLib::TCard4 c4BufSz = 2048;
    tCIDLib::TCh achErr[c4BufSz + 1];
    TRawStr::CopyStr(achErr, L"Got power Op: ", c4BufSz);
    TRawStr::CatStr(achErr, pszXlatPowerOp(ePwrOp), c4BufSz);

    LogDebugStr(achErr);
}


static tCIDLib::TVoid LogDebugSrvOp(const tCIDKernel::EWSrvOps eOp)
{
    const tCIDLib::TCard4 c4BufSz = 2048;
    tCIDLib::TCh achErr[c4BufSz + 1];
    TRawStr::CopyStr(achErr, L"\nGot service Op: ", c4BufSz);
    TRawStr::CatStr(achErr, pszXlatSrvOp(eOp), c4BufSz);

    LogDebugStr(achErr);
}


static DWORD WINAPI CIDServiceCtrlHandler(tCIDLib::TCard4   c4Opcode
                                          , tCIDLib::TCard4 c4EventType
                                          , tCIDLib::TVoid* pEventData
                                          , tCIDLib::TVoid* )
{
    //
    //  Watch for special case of volume change notifications. In these
    //  cases, we call over to the file system module to let it update
    //  it's volume list.
    //
    if (c4Opcode == SERVICE_CONTROL_DEVICEEVENT)
    {
        //
        //  Post it the file system code's notification window, as a device
        //  change message. The parameters we get are the same as those that
        //  would normally go to that message.
        //
        tCIDCtrls::TWndHandle hwndTar(CIDKrnlFS_NotHandle());
        if (hwndTar)
        {
            ::PostMessage
            (
                hwndTar
                , WM_DEVICECHANGE
                , c4EventType
                , (tCIDLib::TInt4)pEventData
            );
        }
        return NO_ERROR;
    }

    // Convert their opcode to our op enum
    const tCIDKernel::EWSrvOps eOp = eXlatOpcode(c4Opcode);;

    // Log what we got
    LogDebugSrvOp(eOp);

    // If it's not something we know, return an error
    if (eOp == tCIDKernel::EWSrvOps::Unknown)
        return ERROR_CALL_NOT_IMPLEMENTED;

    // Default the error return to no error unless something happens otherwise
    tCIDLib::TCard4 c4RetVal = NO_ERROR;

    if (eOp == tCIDKernel::EWSrvOps::PowerEvent)
    {
        //
        //  If it's a suspend query, we call a specific API for that. Else,
        //  if it's a power op we care about, we call the regular power op
        //  handler, with a translate power op value.
        //
        if (c4EventType == PBT_APMQUERYSUSPEND)
        {
            LogDebugMsg(L"Got suspend request");

            //
            //  If they don't want to allow a suspend, then we return an
            //  error code.
            //
            if (!CIDKernel_SvcWin32::psrvhHandler->bSuspendRequest())
            {
                LogDebugMsg(L"Suspend was denied");
                c4RetVal = ERROR_NOT_SUPPORTED;
            }
             else
            {
                LogDebugMsg(L"Suspend was allowed");
            }
        }
         else
        {
            // It's a regular power op call
            tCIDKernel::EWSrvPwrOps ePwr = eXlatPowerOp(c4EventType);
            LogDebugPowerOp(ePwr);

            if (ePwr == tCIDKernel::EWSrvPwrOps::Unknown)
                c4RetVal = ERROR_CALL_NOT_IMPLEMENTED;
            else
                CIDKernel_SvcWin32::psrvhHandler->ServiceOp(ePwr);
        }
    }
     else if (eOp != tCIDKernel::EWSrvOps::Unknown)
    {
        // If stopping, then deregister any handlers
        if ((eOp == tCIDKernel::EWSrvOps::Stop) && CIDKernel_SvcWin32::hNotify)
        {
            LogDebugMsg(L"Unregistering device status notification handler");
            ::UnregisterDeviceNotification(CIDKernel_SvcWin32::hNotify);
            CIDKernel_SvcWin32::hNotify = 0;
        }

        //
        //  If some control request, then do a dummy call just to quickly
        //  get a wait hint set, before we call the handler.
        //
        if ((c4Opcode == SERVICE_CONTROL_STOP)
        ||  (c4Opcode == SERVICE_CONTROL_SHUTDOWN)
        ||  (c4Opcode == SERVICE_CONTROL_PAUSE)
        ||  (c4Opcode == SERVICE_CONTROL_CONTINUE))
        {
            CIDKernel_SvcWin32::SrvStatus.dwWaitHint = 3000;
            ::SetServiceStatus
            (
                CIDKernel_SvcWin32::hService, &CIDKernel_SvcWin32::SrvStatus
            );
        }

        // Just needs a call to the regular service op handler
        CIDKernel_SvcWin32::psrvhHandler->ServiceOp(eOp);

        //
        //  If a shutdown, then ask the service controller to stop the
        //  service thread so that it'll shutdown.
        //
        if ((c4Opcode == SERVICE_CONTROL_SHUTDOWN)
        ||  (c4Opcode == SERVICE_CONTROL_STOP))
        {
            LogDebugMsg(L"Stopping service");

            CIDKernel_SvcWin32::psrvhHandler->bStopServiceThread();

            // Zero out the handle now. There is no close command
            CIDKernel_SvcWin32::hService = 0;
        }
    }
     else
    {
        c4RetVal = ERROR_CALL_NOT_IMPLEMENTED;
    }

    //
    //  Update the service state now, even if it hasn't changed, to indicate
    //  we are still doing alive. Only do this if the handle is set. We zero it
    //  back out above upon stop/shutdown, since the handle is no longer useful
    //  and trying to set the status will fail.
    //
    if (CIDKernel_SvcWin32::hService)
    {
        CIDKernel_SvcWin32::SrvStatus.dwCheckPoint++;
        if (!::SetServiceStatus(CIDKernel_SvcWin32::hService
                                , &CIDKernel_SvcWin32::SrvStatus))
        {
            LogDebugErr(L"Failed to set op status");
        }
    }
    return c4RetVal;
}


static void WINAPI
CIDServiceStart(tCIDLib::TCard4 c4ArgC, tCIDLib::TCh** apszArgs)
{
    //
    //  Try to get the debugging log file opened. This should not be called more
    //  than once, but be careful just in case.
    //
    if (CIDKernel_SvcWin32::hflDbgLog == INVALID_HANDLE_VALUE)
    {
        tCIDLib::TCh achName[kCIDLib::c4MaxPathLen + 1];
        tCIDLib::TCh achPath[kCIDLib::c4MaxPathLen + 1];

        tCIDLib::TBoolean bRes = TKrnlModule::bRawQueryModName
        (
            kmodCIDKernel.hmodThis()
            , achName
            , achPath
            , kCIDLib::c4MaxPathLen
        );

        if (bRes)
        {
            TRawStr::CatStr(achPath, L"\\", kCIDLib::c4MaxPathLen);
            TRawStr::CatStr(achPath, CIDKernel_SvcWin32::pszServiceName, kCIDLib::c4MaxPathLen);
            TRawStr::CatStr(achPath, L"_DbgLog.Txt", kCIDLib::c4MaxPathLen);

            // If it's gotten too large, we want to clip it back
            DWORD OpenFlag = OPEN_ALWAYS;

            WIN32_FIND_DATA FileData;
            HANDLE hFnd = ::FindFirstFile(achPath, &FileData);
            if (hFnd != INVALID_HANDLE_VALUE)
            {
                ::FindClose(hFnd);
                if (FileData.nFileSizeLow > 64 * 1024)
                    OpenFlag = CREATE_NEW;
            }

            CIDKernel_SvcWin32::hflDbgLog = ::CreateFile
            (
                achPath
                , GENERIC_WRITE
                , FILE_SHARE_READ
                , 0
                , OpenFlag
                , FILE_FLAG_WRITE_THROUGH
                , 0
            );

            // Seek to the end
            LARGE_INTEGER liActual;
            liActual.QuadPart = 0;
            tCIDLib::TCard4 c4SeekType = SEEK_END;
            ::SetFilePointerEx(CIDKernel_SvcWin32::hflDbgLog, liActual, 0, c4SeekType);
        }
    }

    LogDebugMsg(L"\n\nStarting up service");

    //
    //  Initialize any service status stuff that doesn't change while we are
    //  started.
    //
    CIDKernel_SvcWin32::SrvStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    CIDKernel_SvcWin32::SrvStatus.dwServiceSpecificExitCode = 0;

    // And now try to register ourself and get our handle
    CIDKernel_SvcWin32::hService = ::RegisterServiceCtrlHandlerEx
    (
        CIDKernel_SvcWin32::pszServiceName, &CIDServiceCtrlHandler, 0
    );

    if (!CIDKernel_SvcWin32::hService)
    {
        LogDebugErr(L"Failed to register control handler");
        return;
    }

    // Initialize our service status to start pending
    if (!TKrnlWin32Service::bSetState(tCIDKernel::EWSrvStates::Start_Pending, 3000))
    {
        LogDebugErr(L"failed to set pending status");
        return;
    }

    //
    //  Override the command line parms that would have been parsed out
    //  automatically by the system info kernel class. He won't have seen
    //  the parms that we are getting as a service.
    //
    //  Only do this if we got more than one parm, since if we get started
    //  the other way, they will come to the process main, and we won't get
    //  any and we'd wipe out the other ones.
    //
    if (c4ArgC > 1)
        TKrnlSysInfo::OverrideCmdLine(c4ArgC, apszArgs);

    // Register for device volume change notifications
    {
        LogDebugMsg(L"Registering device status notification handler");

        DEV_BROADCAST_DEVICEINTERFACE DevNot = {0};
        DevNot.dbcc_size = sizeof(DevNot);
        DevNot.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        DevNot.dbcc_classguid = GUID_DEVINTERFACE_VOLUME;
        CIDKernel_SvcWin32::hNotify = ::RegisterDeviceNotification
        (
            CIDKernel_SvcWin32::hService, &DevNot, DEVICE_NOTIFY_SERVICE_HANDLE
        );

        if (!CIDKernel_SvcWin32::hNotify)
            LogDebugErr(L"failed to register device notifications");
    }

    //
    //  Do another pending call before invoking the handler, so that we set a
    //  fresh wait hint.
    //
    if (!TKrnlWin32Service::bSetState(tCIDKernel::EWSrvStates::Start_Pending, 3000))
    {
        LogDebugErr(L"failed to set pending status");
        return;
    }

    //
    //  Now we can start up our applications's thread. If he successfully
    //  gets it going, set our status to running and return.
    //
    try
    {
        if (!CIDKernel_SvcWin32::psrvhHandler->bStartServiceThread())
        {
            LogDebugErr(L"thread start failed");
            TKrnlWin32Service::bSetState
            (
                tCIDKernel::EWSrvStates::Stopped
                , 0
                , tCIDLib::c4EnumOrd(tCIDLib::EExitCodes::InitFailed)
            );
            return;
        }
        TKrnlWin32Service::bSetState(tCIDKernel::EWSrvStates::Running, 0);
    }

    catch(...)
    {
        LogDebugExcept(L"Exception during thread startup");
        TKrnlWin32Service::bSetState
        (
            tCIDKernel::EWSrvStates::Stopped
            , 0
            , tCIDLib::c4EnumOrd(tCIDLib::EExitCodes::InitFailed)
        );
        return;
    }

    LogDebugMsg(L"Service startup complete");
}



// ---------------------------------------------------------------------------
//   CLASS: TWin32ServiceHandler
//  PREFIX: srvh
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TWin32ServiceHandler: Constructors and Destructor
// ---------------------------------------------------------------------------
TWin32ServiceHandler::TWin32ServiceHandler()
{
}

TWin32ServiceHandler::~TWin32ServiceHandler()
{
}




// ---------------------------------------------------------------------------
//  Namespace: TKrnlServiceWin32
// ---------------------------------------------------------------------------

// Change the configuration of the passed existing service
tCIDLib::TBoolean
TKrnlWin32Service::bChangeConfig(const  tCIDKernel::TWSvcHandle hsvcToChange
                                , const tCIDLib::TCh* const     pszPath
                                , const tCIDLib::TCh* const     pszDisplayName
                                , const tCIDLib::TCh* const     pszDescription
                                , const tCIDKernel::EWSrvStarts eStartType
                                , const tCIDLib::TCh**          apszDepends
                                , const tCIDLib::TCard4         c4DepCount
                                , const tCIDLib::TCh**          apszParms
                                , const tCIDLib::TCard4         c4ParmCount)
{
    // Convert the start up type to the Win32 type
    tCIDLib::TCard4 c4StartType = SERVICE_DISABLED;
    if ((eStartType == tCIDKernel::EWSrvStarts::Auto)
    ||  (eStartType == tCIDKernel::EWSrvStarts::Delayed))
    {
        // These are both auto-start
        c4StartType = SERVICE_AUTO_START;
    }
     else if (eStartType == tCIDKernel::EWSrvStarts::Demand)
    {
        c4StartType = SERVICE_DEMAND_START;
    }

    //
    //  If there are dependencies, then set up that buffer. Its a list of
    //  nul terminated strings, wtih a final null.
    //
    tCIDLib::TCh* pszDepBuf = 0;
    tCIDLib::TCard4 c4Index = 0;
    if (c4DepCount)
    {
        tCIDLib::TCard4 c4Len = 0;
        for (c4Index = 0; c4Index < c4DepCount; c4Index++)
            c4Len += (TRawStr::c4StrLen(apszDepends[c4Index]) + 1);
        pszDepBuf = new tCIDLib::TCh[c4Len + 1];

        tCIDLib::TCard4 c4CurOfs = 0;
        for (c4Index = 0; c4Index < c4DepCount; c4Index++)
        {
            TRawStr::CopyStr
            (
                &pszDepBuf[c4CurOfs]
                , apszDepends[c4Index]
                , c4Len - c4CurOfs
            );
            c4CurOfs += (TRawStr::c4StrLen(apszDepends[c4Index]) + 1);
        }
        pszDepBuf[c4CurOfs] = kCIDLib::chNull;
    }

    //
    //  And do the same thing for the path and parameters. In order to get
    //  auto-start parms set, we have to add them to the path. The ones we
    //  pass in the bStart() method only apply on that start.
    //
    tCIDLib::TCh* pszPathBuf = 0;
    tCIDLib::TCard4 c4PathBufLen = TRawStr::c4StrLen(pszPath) + 3;
    for (c4Index = 0; c4Index < c4ParmCount; c4Index++)
        c4PathBufLen += (TRawStr::c4StrLen(apszParms[c4Index]) + 3);
    pszPathBuf = new tCIDLib::TCh[c4PathBufLen + 1];

    const tCIDLib::TCh* const pszStartQ = L"\"";
    const tCIDLib::TCh* const pszEndQ = L"\" ";

    TRawStr::CopyStr(pszPathBuf, pszStartQ, c4PathBufLen);
    TRawStr::CatStr(pszPathBuf, pszPath, c4PathBufLen);
    TRawStr::CatStr(pszPathBuf, pszEndQ, c4PathBufLen);
    for (c4Index = 0; c4Index < c4ParmCount; c4Index++)
    {
        TRawStr::CatStr(pszPathBuf, pszStartQ, c4PathBufLen);
        TRawStr::CatStr(pszPathBuf, apszParms[c4Index], c4PathBufLen);
        TRawStr::CatStr(pszPathBuf, pszEndQ, c4PathBufLen);
    }

    // Put janitors on our buffers
    TArrayJanitor<tCIDLib::TCh> janDeps(pszDepBuf);
    TArrayJanitor<tCIDLib::TCh> janPath(pszPathBuf);

    //
    //  Since we are doing multiple API calls, we need to lock the service
    //  database across them.
    //
    SC_HANDLE hSCM = hOpenManager();
    SC_LOCK lockSC = 0;
    tCIDLib::TBoolean bRet = kCIDLib::False;
    if (hSCM)
    {
        lockSC = ::LockServiceDatabase(hSCM);
        if (lockSC)
        {
            bRet = ::ChangeServiceConfig
            (
                hsvcToChange
                , SERVICE_WIN32_OWN_PROCESS
                , c4StartType
                , SERVICE_ERROR_NORMAL
                , pszPathBuf
                , 0
                , 0
                , pszDepBuf
                , 0
                , 0
                , pszDisplayName
            ) != 0;
        }

        if (bRet)
        {
            // Update the desscription, which requires another API
            SERVICE_DESCRIPTION DescrInfo;
            DescrInfo.lpDescription = const_cast<tCIDLib::TCh*>(pszDescription);
            if (!::ChangeServiceConfig2(hsvcToChange, SERVICE_CONFIG_DESCRIPTION, &DescrInfo))
                bRet = kCIDLib::False;

            // And handle delayed start or not
            SERVICE_DELAYED_AUTO_START_INFO AutoInfo;
            AutoInfo.fDelayedAutostart = (eStartType == tCIDKernel::EWSrvStarts::Delayed);
            if (!::ChangeServiceConfig2(hsvcToChange,SERVICE_CONFIG_DELAYED_AUTO_START_INFO, &AutoInfo))
                bRet = kCIDLib::False;
        }
    }

    // If it failed, store the error now before we do anything else
    if (!bRet)
        TKrnlError::SetLastHostError(::GetLastError());

    // And clean up stuff
    if (lockSC)
        ::UnlockServiceDatabase(lockSC);

    if (hSCM)
        ::CloseServiceHandle(hSCM);

    return bRet;
}


// Close the passed service handle
tCIDLib::TBoolean
TKrnlWin32Service::bClose(const tCIDKernel::TWSvcHandle hsvcToClose)
{
    // In debug mode, just say it worked
    if (CIDKernel_SvcWin32::bDebugMode)
        return kCIDLib::True;

    if (!::CloseServiceHandle(hsvcToClose))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlWin32Service::bInService()
{
    return (CIDKernel_SvcWin32::hService != 0);
}


// Used by the service itself to initialize the service handler stuff
tCIDLib::TBoolean
TKrnlWin32Service::bInit(const  tCIDLib::TCh* const         pszServiceName
                        ,       TWin32ServiceHandler* const psrvhToSet
                        , const tCIDLib::TBoolean           bDebugMode)
{
#if CID_DEBUG_ON
// #define DBG_SERVICE_MODE 1
#if defined(DBG_SERVICE_MODE)

        tCIDLib::TBoolean bWait = kCIDLib::True;
        while (bWait)
        {
            ::Sleep(500);
        }
#endif
#endif


    // Store the passed debug mode
    CIDKernel_SvcWin32::bDebugMode = bDebugMode;

    // Remember the passed info for later use in the service main
    CIDKernel_SvcWin32::psrvhHandler    = psrvhToSet;
    CIDKernel_SvcWin32::pszServiceName  = TRawStr::pszReplicate(pszServiceName);

    // Tell it about our entry points
    SERVICE_TABLE_ENTRYW aDispatchTable[] =
    {
            { (tCIDLib::TCh*)pszServiceName , &CIDServiceStart  }
        ,   { 0                             , 0                 }
    };

    //
    //  And now enter the dispatch thread. We won't come back from here
    //  until its time to shut down the system or we get shut down.
    //
    if (bDebugMode)
    {
        try
        {
            if (!CIDKernel_SvcWin32::psrvhHandler->bStartServiceThread())
                return kCIDLib::False;

            // And now just sleep forever
            TKrnlThread::Sleep(kCIDLib::c4MaxCard);
        }

        catch(...)
        {
            return kCIDLib::False;
        }
    }
     else
    {
        if (!::StartServiceCtrlDispatcher(aDispatchTable))
        {
            LogDebugErr(L"failed to start");
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlWin32Service::bPleaseWait(const tCIDKernel::EWSrvStates eToSet)
{
    // In debug mode, just ignore it
    if (CIDKernel_SvcWin32::bDebugMode)
        return kCIDLib::True;

    if (!bSetState(eToSet, 3000, 0))
    {
        LogDebugErr(L"failed to set service status on PleaseWait");
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Query the start type of the indicated service
tCIDLib::TBoolean
TKrnlWin32Service::bQueryStartType( const   tCIDKernel::TWSvcHandle     hsvcSrc
                                    ,       tCIDKernel::EWSrvStarts&    eToFill)
{
    // Figure out the required size
    tCIDLib::TCard4 c4BufSz;
    BOOL Res = ::QueryServiceConfig(hsvcSrc, 0, 0, &c4BufSz);
    DWORD Err = ::GetLastError();
    if (Err != ERROR_INSUFFICIENT_BUFFER)
    {
        // It wasn't the expected error, so we failed
        TKrnlError::SetLastHostError(Err);
        return kCIDLib::False;
    }

    // Allocate the buffer and cast to the right type
    tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[c4BufSz];
    QUERY_SERVICE_CONFIG* pConfig = reinterpret_cast<QUERY_SERVICE_CONFIG*>(pc1Buf);

    // Now we can do the real thing
    Res = ::QueryServiceConfig(hsvcSrc, pConfig, c4BufSz, &c4BufSz);
    if (!Res)
    {
        delete [] pc1Buf;

        // It wasn't the expected error, so we failed
        TKrnlError::SetLastHostError(Err);
        return kCIDLib::False;
    }

    // Convert the system type to our type
    switch(pConfig->dwStartType)
    {
        case SERVICE_AUTO_START :
            eToFill = tCIDKernel::EWSrvStarts::Auto;
            break;

        case SERVICE_DEMAND_START :
            eToFill = tCIDKernel::EWSrvStarts::Demand;
            break;

        case SERVICE_DISABLED :
        default :
            //
            //  We consider anything other tahn the above to be disabled.
            //  In effect we wouldn't likely see the others.
            //
            eToFill = tCIDKernel::EWSrvStarts::Disabled;
            break;
    };

    // If it's auto-start, get the delayed start option and update to delayed if so
    if (eToFill == tCIDKernel::EWSrvStarts::Auto)
    {
        DWORD ReqSz;
        SERVICE_DELAYED_AUTO_START_INFO AutoInfo;
        Res = ::QueryServiceConfig2
        (
            hsvcSrc
            , SERVICE_CONFIG_DELAYED_AUTO_START_INFO
            , (LPBYTE)&AutoInfo
            , sizeof(AutoInfo)
            , &ReqSz
        );

        if (Res && AutoInfo.fDelayedAutostart)
            eToFill = tCIDKernel::EWSrvStarts::Delayed;
    }

    // Delete the buffer before we leave
    delete [] pc1Buf;

    return kCIDLib::True;
}


// Query the state of the indicated service
tCIDLib::TBoolean
TKrnlWin32Service::bQueryState( const   tCIDKernel::TWSvcHandle     hsvcToQuery
                                ,       tCIDKernel::EWSrvStates&    eToFill)
{
    // In debug mode, just ignore it
    if (CIDKernel_SvcWin32::bDebugMode)
        return kCIDLib::True;

    // Get the status and translate it to our enum for return
    SERVICE_STATUS Status = {0};
    if (!::QueryServiceStatus(hsvcToQuery, &Status))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    eToFill = eXlatSrvState(Status.dwCurrentState);
    return kCIDLib::True;
}


//
//  Removes the indicated service from the service table. It shoudl be stopped
//  before calling this.
//
tCIDLib::TBoolean
TKrnlWin32Service::bRemove(const tCIDKernel::TWSvcHandle hsvcToRemove)
{
    // In debug mode, just say it worked
    if (CIDKernel_SvcWin32::bDebugMode)
        return kCIDLib::True;

    if (!::DeleteService(hsvcToRemove))
    {
        const tCIDLib::TCard4 c4Err = ::GetLastError();

        // We could get a 'marked for delete' error, which isn't really an error
        if (c4Err != ERROR_SERVICE_MARKED_FOR_DELETE)
        {
            TKrnlError::SetLastHostError(c4Err);
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


// Send a service opcode directly to the service loop
tCIDLib::TBoolean
TKrnlWin32Service::bSendOpCode( const   tCIDKernel::TWSvcHandle hsvcTarget
                                , const tCIDKernel::EWSrvOps    eToSend)
{
    // In debug mode, send it directly
    if (CIDKernel_SvcWin32::bDebugMode)
    {
        CIDKernel_SvcWin32::psrvhHandler->ServiceOp(eToSend);
        return kCIDLib::True;
    }

    SERVICE_STATUS Status = {0};
    if (!::ControlService(hsvcTarget, c4XlatOpcode(eToSend), &Status))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Set the service state. Generally should only be called by the service
//  itself.
//
tCIDLib::TBoolean
TKrnlWin32Service::bSetState(const  tCIDKernel::EWSrvStates eToSet
                            , const tCIDLib::TCard4         c4WaitHint
                            , const tCIDLib::TCard4         c4ExitCode)
{
    CIDKernel_SvcWin32::SrvStatus.dwCurrentState = c4XlatSrvState(eToSet);
    CIDKernel_SvcWin32::SrvStatus.dwControlsAccepted = 0;
    CIDKernel_SvcWin32::SrvStatus.dwWaitHint = 0;

    // In debug mode, we are done
    if (CIDKernel_SvcWin32::bDebugMode)
        return kCIDLib::True;

    // Otherwise, set up stuff based on the new state
    switch(eToSet)
    {
        case tCIDKernel::EWSrvStates::Continue_Pending :
        case tCIDKernel::EWSrvStates::Pause_Pending :
        case tCIDKernel::EWSrvStates::Start_Pending :
        case tCIDKernel::EWSrvStates::Stop_Pending :
        {
            CIDKernel_SvcWin32::SrvStatus.dwWaitHint = c4WaitHint;
            CIDKernel_SvcWin32::SrvStatus.dwCheckPoint++;
            break;
        }

        case tCIDKernel::EWSrvStates::Paused :
        {
            CIDKernel_SvcWin32::SrvStatus.dwCheckPoint = 0;
            break;
        }

        case tCIDKernel::EWSrvStates::Stopped :
        {
            CIDKernel_SvcWin32::SrvStatus.dwServiceSpecificExitCode = c4ExitCode;
            CIDKernel_SvcWin32::SrvStatus.dwCheckPoint = 0;
            break;
        }

        case tCIDKernel::EWSrvStates::Running :
        {
            // If running, we can accept controls
            CIDKernel_SvcWin32::SrvStatus.dwCheckPoint = 0;
            CIDKernel_SvcWin32::SrvStatus.dwControlsAccepted =
            (
                SERVICE_ACCEPT_SHUTDOWN
                | SERVICE_ACCEPT_STOP
                | SERVICE_ACCEPT_POWEREVENT
            );
            break;
        }

        default :
            CIDKernel_SvcWin32::SrvStatus.dwCheckPoint = 0;
            break;
    }

    if (!::SetServiceStatus(CIDKernel_SvcWin32::hService
                            , &CIDKernel_SvcWin32::SrvStatus))
    {
        LogDebugErr(L"Failed to set op status");
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Start the indicate service up with the passed startup parms.
//
tCIDLib::TBoolean
TKrnlWin32Service::bStart(  const   tCIDKernel::TWSvcHandle hsvcToStart
                            , const tCIDLib::TCh**          apszParms
                            , const tCIDLib::TCard4         c4ParmCount
                            , const tCIDLib::TCard4         c4WaitFor)
{
    LogDebugMsg(L"Got start request");

    // In debug mode, just ignore it
    if (CIDKernel_SvcWin32::bDebugMode)
        return kCIDLib::True;

    if (!::StartService(hsvcToStart, c4ParmCount, apszParms))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    //
    //  It seems to have worked, so wait for up to the indicate amount of
    //  time for it to actually show up as having started. Do an initial
    //  query to init the check point and time stamp.
    //
    SERVICE_STATUS  Status;
    if (!::QueryServiceStatus(hsvcToStart, &Status))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    tCIDLib::TCard4 c4LastMillis = ::GetTickCount();
    tCIDLib::TCard4 c4LastCheckPoint = Status.dwCheckPoint;

    //
    //  Now loop for either the indicated time or it shows up running
    //
    const tCIDLib::TCard4 c4EndTime = ::GetTickCount() + c4WaitFor;
    while ((::GetTickCount() < c4EndTime) && (Status.dwCurrentState != SERVICE_RUNNING))
    {
        ::Sleep(500);

        // Check the status again
        if (!::QueryServiceStatus(hsvcToStart, &Status))
        {
            LogDebugErr(L"Failed to query service status");

            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }
#if 0
        // As long as we are making progress, keep going
        if (Status.dwCheckPoint != c4LastCheckPoint)
        {
            c4LastMillis = ::GetTickCount();
            c4LastCheckPoint = Status.dwCheckPoint;
        }
         else
        {
            // If no progress in the allotted time, so give up on him
            if ((::GetTickCount() - c4LastMillis) > Status.dwWaitHint)
                break;
        }
#endif
    }

    LogDebugMsg(L"Started successfully");

    // Return a boolean that indicates whether it's running yet
    return (Status.dwCurrentState == SERVICE_RUNNING);
}


tCIDLib::TBoolean
TKrnlWin32Service::bStop(const  tCIDKernel::TWSvcHandle hsvcToStop
                        , const tCIDLib::TCard4         c4WaitFor)
{
    // In debug mode, just ignore it
    if (CIDKernel_SvcWin32::bDebugMode)
        return kCIDLib::True;

    LogDebugMsg(L"Got stop request");

    // Do an initial check of the status
    SERVICE_STATUS  Status = {0};
    if (!::QueryServiceStatus(hsvcToStop, &Status))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // If already stopped, we are done
    if (Status.dwCurrentState == SERVICE_STOPPED)
        return kCIDLib::True;

    //
    //  It's not stopped, so see if a stop is pending. If not, then we need
    //  to ask it to stop, else we can fall through into the wait loop.
    //
    if (Status.dwCurrentState != SERVICE_STOP_PENDING)
    {
        if (!::ControlService(hsvcToStop, SERVICE_CONTROL_STOP, &Status))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }
    }

    // Calculate the end time
    const tCIDLib::TCard4 c4EndTime = ::GetTickCount() + c4WaitFor;

    while (Status.dwCurrentState != SERVICE_STOPPED)
    {
        ::Sleep(500);
        if (!::QueryServiceStatus(hsvcToStop, &Status))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return kCIDLib::False;
        }

        if ((Status.dwCurrentState != SERVICE_STOPPED)
        &&  (::GetTickCount() > c4EndTime))
        {
            TKrnlError::SetLastHostError(ERROR_TIMEOUT);
            return kCIDLib::False;
        }

        ::Sleep(10);
    }
    return kCIDLib::True;
}


// Return the current state of the service
tCIDKernel::EWSrvStates TKrnlWin32Service::eCurState()
{
    return eXlatSrvState(CIDKernel_SvcWin32::SrvStatus.dwCurrentState);
}


tCIDKernel::TWSvcHandle
TKrnlWin32Service::hsvcCreate(  const   tCIDLib::TCh* const     pszPath
                                , const tCIDLib::TCh* const     pszServiceName
                                , const tCIDLib::TCh* const     pszDisplayName
                                , const tCIDLib::TCh* const     pszDescription
                                , const tCIDKernel::EWSrvStarts eStartType
                                , const tCIDLib::TCh**          apszDepends
                                , const tCIDLib::TCard4         c4DepCount
                                , const tCIDLib::TCh**          apszParms
                                , const tCIDLib::TCard4         c4ParmCount)
{
    // Convert the start up type to the Win32 type
    tCIDLib::TCard4 c4StartType = SERVICE_DISABLED;
    if ((eStartType == tCIDKernel::EWSrvStarts::Auto)
    ||  (eStartType == tCIDKernel::EWSrvStarts::Delayed))
    {
        // These are both auto-start
        c4StartType = SERVICE_AUTO_START;
    }
     else if (eStartType == tCIDKernel::EWSrvStarts::Demand)
    {
        c4StartType = SERVICE_DEMAND_START;
    }

    //
    //  If there are dependencies, then set up that buffer. Its a list of
    //  nul terminated strings, wtih a final null.
    //
    tCIDLib::TCh*   pszDepBuf = 0;
    tCIDLib::TCard4 c4Index = 0;
    if (c4DepCount)
    {
        tCIDLib::TCard4 c4Len = 0;
        for (c4Index = 0; c4Index < c4DepCount; c4Index++)
            c4Len += (TRawStr::c4StrLen(apszDepends[c4Index]) + 1);
        pszDepBuf = new tCIDLib::TCh[c4Len + 1];

        tCIDLib::TCard4 c4CurOfs = 0;
        for (c4Index = 0; c4Index < c4DepCount; c4Index++)
        {
            TRawStr::CopyStr
            (
                &pszDepBuf[c4CurOfs]
                , apszDepends[c4Index]
                , c4Len - c4CurOfs
            );
            c4CurOfs += (TRawStr::c4StrLen(apszDepends[c4Index]) + 1);
        }
        pszDepBuf[c4CurOfs] = kCIDLib::chNull;
    }

    //
    //  And do the same thing for the path and parameters. In order to get
    //  auto-start parms set, we have to add them to the path. The ones we
    //  pass in the bStart() method only apply on that start.
    //
    tCIDLib::TCh* pszPathBuf = 0;
    tCIDLib::TCard4 c4PathBufLen = TRawStr::c4StrLen(pszPath) + 3;
    for (c4Index = 0; c4Index < c4ParmCount; c4Index++)
        c4PathBufLen += (TRawStr::c4StrLen(apszParms[c4Index]) + 3);
    pszPathBuf = new tCIDLib::TCh[c4PathBufLen + 1];

    const tCIDLib::TCh* const pszStartQ = L"\"";
    const tCIDLib::TCh* const pszEndQ = L"\" ";

    TRawStr::CopyStr(pszPathBuf, pszStartQ, c4PathBufLen);
    TRawStr::CatStr(pszPathBuf, pszPath, c4PathBufLen);
    TRawStr::CatStr(pszPathBuf, pszEndQ, c4PathBufLen);
    for (c4Index = 0; c4Index < c4ParmCount; c4Index++)
    {
        TRawStr::CatStr(pszPathBuf, pszStartQ, c4PathBufLen);
        TRawStr::CatStr(pszPathBuf, apszParms[c4Index], c4PathBufLen);
        TRawStr::CatStr(pszPathBuf, pszEndQ, c4PathBufLen);
    }

    // Put janitors on our buffers
    TArrayJanitor<tCIDLib::TCh> janDeps(pszDepBuf);
    TArrayJanitor<tCIDLib::TCh> janPath(pszPathBuf);

    // Start by gettting a handle to the service manager
    SC_HANDLE hSCM = hOpenManager();
    SC_LOCK lockSC = 0;
    tCIDKernel::TWSvcHandle hsvcRet = 0;
    if (hSCM)
    {
        // Since we have to do multiple calls, we need to lock
        lockSC = ::LockServiceDatabase(hSCM);
        if (lockSC)
        {
            // Ok, let's try to create the service
            hsvcRet = ::CreateService
            (
                hSCM
                , pszServiceName
                , pszDisplayName
                , SERVICE_ALL_ACCESS
                , SERVICE_WIN32_OWN_PROCESS
                , c4StartType
                , SERVICE_ERROR_NORMAL
                , pszPathBuf
                , 0
                , 0
                , pszDepBuf
                , 0
                , 0
            );

            if (hsvcRet)
            {
                // Set the service description
                SERVICE_DESCRIPTION DescrInfo;
                DescrInfo.lpDescription = const_cast<tCIDLib::TCh*>(pszDescription);
                ::ChangeServiceConfig2(hsvcRet, SERVICE_CONFIG_DESCRIPTION, &DescrInfo);

                // And handle delayed start or not
                SERVICE_DELAYED_AUTO_START_INFO AutoInfo;
                AutoInfo.fDelayedAutostart = (eStartType == tCIDKernel::EWSrvStarts::Delayed);
                ::ChangeServiceConfig2(hsvcRet,SERVICE_CONFIG_DELAYED_AUTO_START_INFO, &AutoInfo);
            }
        }
    }

    // It failed for some reason above so store the error
    if (!hsvcRet)
        TKrnlError::SetLastHostError(::GetLastError());

    // And clean up stuff
    if (lockSC)
        ::UnlockServiceDatabase(lockSC);

    if (hSCM)
        ::CloseServiceHandle(hSCM);

    return hsvcRet;
}


tCIDKernel::TWSvcHandle
TKrnlWin32Service::hsvcOpen(const tCIDLib::TCh* const pszServiceName)
{
    // Try to get a handle to the database
    SC_HANDLE hSCM = hOpenManager();
    if (!hSCM)
        return 0;

    // And try to open the service
    SC_HANDLE hsvcRet = ::OpenService(hSCM, pszServiceName, SERVICE_ALL_ACCESS);

    // If the open failed, store the error
    if (!hsvcRet)
        TKrnlError::SetLastHostError(::GetLastError());

    // Let the manager handle go if we got it open
    if (hSCM)
        ::CloseServiceHandle(hSCM);

    return hsvcRet;
}


//
//  The service program can call this periodically to prevent the system from
//  powering down due to inactivity, since some services need to stay active
//  even if the user isn't using teh system. Call it every five seconds or so.
//
tCIDLib::TVoid TKrnlWin32Service::RefreshStayAwake()
{
    ::SetThreadExecutionState(ES_SYSTEM_REQUIRED);
}

