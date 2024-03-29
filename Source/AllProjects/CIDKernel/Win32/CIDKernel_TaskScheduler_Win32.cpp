//
// FILE NAME: CIDKernel_TaskSched_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/07/2012
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
//  This file provides the Win32 specific implementation of the the
//  TKrnlTaskSched namespace.
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
#include    "CIDKernel_InternalHelpers_.hpp"



// ---------------------------------------------------------------------------
//  Force in some headers and libraries we require
// ---------------------------------------------------------------------------
#pragma     warning(push)
#include    <CodeAnalysis\Warnings.h>
#pragma     warning(disable : ALL_CODE_ANALYSIS_WARNINGS 26812)
#include    <comdef.h>
#include    <taskschd.h>
#pragma     warning(pop)

#pragma comment(lib, "taskschd.lib")



// ---------------------------------------------------------------------------
//  TKrnlTaskSched functions
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TKrnlTaskSched::bCreateSchedTask(const  tCIDLib::TCh* const pszName
                                , const tCIDLib::TCh* const pszAuthor
                                , const tCIDLib::TCh* const pszPath
                                , const tCIDLib::TCh* const pszArgs
                                , const tCIDLib::TBoolean   bAdminRights
                                , const EStartTypes         eStartType
                                , const tCIDLib::TCard4     c4DelaySecs)
{
    HRESULT hRes;

    ITaskService* pService = nullptr;
    hRes = ::CoCreateInstance
    (
        CLSID_TaskScheduler
        , NULL
        , CLSCTX_INPROC_SERVER
        , IID_ITaskService
        , tCIDLib::pToVoidPP(&pService)
    );

    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_SetupErr, hRes);
        return kCIDLib::False;
    }

    // We got the service so put a janitor on it and try to connect
    TCOMJanitor<ITaskService> janService(&pService);
    hRes = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_SetupErr, hRes);
        return kCIDLib::False;
    }

    // Get the root folder
    ITaskFolder* pRootFolder = nullptr;
    hRes = pService->GetFolder(_bstr_t(L"\\") , &pRootFolder);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_SetupErr, hRes);
        return kCIDLib::False;
    }

    // If the same task exists, remove it.
    pRootFolder->DeleteTask(_bstr_t(pszName), 0);

    // Create the task builder object to create the task.
    ITaskDefinition* pTask = nullptr;
    hRes = pService->NewTask(0, &pTask);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_SetupErr, hRes);
        return kCIDLib::False;
    }

    // Put a janitor on the task and let's do the setup
    TCOMJanitor<ITaskDefinition> janTask(&pTask);

    // Set the registration info. If it fails, we don't stop
    {
        IRegistrationInfo* pRegInfo = nullptr;
        hRes = pTask->get_RegistrationInfo(&pRegInfo);
        if (FAILED(hRes))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_SetupErr, hRes);
            return kCIDLib::False;
        }

        pRegInfo->put_Author(_bstr_t(pszAuthor));
        pRegInfo->Release();
    }

    // Get the task settings object so we can set it up
    ITaskSettings* pSettings = nullptr;
    hRes = pTask->get_Settings(&pSettings);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_SetupErr, hRes);
        return kCIDLib::False;
    }
    TCOMJanitor<ITaskSettings> janSettings(&pSettings);
    hRes = pSettings->put_StartWhenAvailable(VARIANT_TRUE);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_SetupErr, hRes);
        return kCIDLib::False;
    }

    // We run indefinitely, so disable the 'kill after time' option
    hRes = pSettings->put_ExecutionTimeLimit(_bstr_t(L"PT0S"));
    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_SetupErr, hRes);
        return kCIDLib::False;
    }

    // Set the run type and remember a logon type appropriately
    TASK_LOGON_TYPE LoginType = TASK_LOGON_NONE;
    const tCIDLib::TCh* pszUser = nullptr;
    {
        ITriggerCollection* pTriggerCol = nullptr;
        hRes = pTask->get_Triggers(&pTriggerCol);
        if (FAILED(hRes))
            return kCIDLib::False;

        // Add a trigger for the run type
        TCOMJanitor<ITriggerCollection> janTrigCol(&pTriggerCol);

        TASK_TRIGGER_TYPE2 TrigType;
        switch(eStartType)
        {
            case EStartTypes::OnLogon :
                TrigType = TASK_TRIGGER_LOGON;
                LoginType = TASK_LOGON_GROUP;
                pszUser = L"Builtin\\Administrators";
                break;

            case EStartTypes::OnBoot :
                TrigType = TASK_TRIGGER_BOOT;
                LoginType = TASK_LOGON_SERVICE_ACCOUNT;
                pszUser = L"Local Service";
                break;

            default :
                return kCIDLib::False;
        };

        // Create the basic trigger
        ITrigger* pTrigger = nullptr;
        hRes = pTriggerCol->Create(TrigType, &pTrigger);
        if (FAILED(hRes))
            return kCIDLib::False;

        // Set an id on this trigger
        hRes = pTrigger->put_Id(_bstr_t( L"Trigger1"));
        if (FAILED(hRes))
            return kCIDLib::False;

        // Set it the delay string if they provided one
        tCIDLib::TZStr32 szDelStr = L"";
        if (c4DelaySecs)
        {
            tCIDLib::TZStr32 szDelay;
            TRawStr::bFormatVal(c4DelaySecs, szDelay, tCIDLib::c4MaxBufChars(szDelay));
            TRawStr::CopyCatStr(szDelay, tCIDLib::c4MaxBufChars(szDelStr), szDelay, L"S");
        }

        // Get it as the actual type and set specific values
        TCOMJanitor<ITrigger> janTrigger(&pTrigger);
        switch(eStartType)
        {
            case EStartTypes::OnLogon :
            {
                ILogonTrigger* pLTrigger = nullptr;
                hRes = pTrigger->QueryInterface(IID_ILogonTrigger, tCIDLib::pToVoidPP(&pLTrigger));
                if (FAILED(hRes))
                {
                    TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_SetupErr, hRes);
                    return kCIDLib::False;
                }

                // Set a delay if provided
                if (c4DelaySecs)
                    pLTrigger->put_Delay(szDelStr);

                pLTrigger->Release();
                break;
            }

            case EStartTypes::OnBoot :
            {
                IBootTrigger* pBTrigger = 0;
                hRes = pTrigger->QueryInterface(IID_IBootTrigger, tCIDLib::pToVoidPP(&pBTrigger));
                if (FAILED(hRes))
                {
                    TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_SetupErr, hRes);
                    return kCIDLib::False;
                }

                // Set a delay if provided
                if (c4DelaySecs)
                    pBTrigger->put_Delay(szDelStr);

                pBTrigger->Release();
                break;
            }

            default :
                TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_BadStartType);
                return kCIDLib::False;
        };
    }

    // Set it up to run at admin level if indicated
    if (bAdminRights)
    {
        IPrincipal* pPrincipal = nullptr;
        hRes = pTask->get_Principal(&pPrincipal);
        if (FAILED(hRes))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_SetupErr, hRes);
            return kCIDLib::False;
        }
        pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);

        pPrincipal->put_GroupId(_bstr_t(L"Users"));
        pPrincipal->Release();
    }

    // And let's set up the task steps
    {
        IActionCollection* pActionCol = nullptr;
        hRes = pTask->get_Actions(&pActionCol);
        if (FAILED(hRes))
            return kCIDLib::False;

        TCOMJanitor<IActionCollection> janActCol(&pActionCol);
        IAction* pAction = nullptr;
        hRes = pActionCol->Create(TASK_ACTION_EXEC, &pAction);
        if (FAILED(hRes))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_SetupErr, hRes);
            return kCIDLib::False;
        }

        IExecAction* pExecAction = nullptr;
        hRes = pAction->QueryInterface(IID_IExecAction, tCIDLib::pToVoidPP(&pExecAction));
        pAction->Release();
        if (FAILED(hRes))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_SetupErr, hRes);
            return kCIDLib::False;
        }

        // Set the program path
        hRes = pExecAction->put_Path(_bstr_t(pszPath));
        if (FAILED(hRes))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_SetupErr, hRes);
            return kCIDLib::False;
        }

        // Set the arguments
        hRes = pExecAction->put_Arguments(_bstr_t(pszArgs));
        if (FAILED(hRes))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_SetupErr, hRes);
            return kCIDLib::False;
        }
    }

    // And finally, a novel later, let's create the task
    IRegisteredTask* pRegisteredTask = nullptr;
    VARIANT varPassword;
    ::VariantInit(&varPassword);
    varPassword.vt = VT_EMPTY;

    hRes = pRootFolder->RegisterTaskDefinition
    (
        _bstr_t(pszName)
        , pTask
        , TASK_CREATE_OR_UPDATE
        , _variant_t(pszUser)
        , varPassword
        , LoginType
        , _variant_t(L"")
        , &pRegisteredTask
    );

    if (FAILED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcSchTask_CreateErr, hRes);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlTaskSched::bDeleteSchedTask(const tCIDLib::TCh* const pszName)
{
    HRESULT hRes;

    ITaskService* pService = nullptr;
    hRes = ::CoCreateInstance
    (
        CLSID_TaskScheduler
        , NULL
        , CLSCTX_INPROC_SERVER
        , IID_ITaskService
        , tCIDLib::pToVoidPP(&pService)
    );

    if (FAILED(hRes))
        return kCIDLib::False;

    // We got the service so put a janitor on it and try to connect
    TCOMJanitor<ITaskService> janService(&pService);
    hRes = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
    if (FAILED(hRes))
        return kCIDLib::False;

    // Get the root folder
    ITaskFolder* pRootFolder = nullptr;
    hRes = pService->GetFolder(_bstr_t(L"\\") , &pRootFolder);
    if (FAILED(hRes))
        return kCIDLib::False;

    // If this task exists, remove it
    hRes = pRootFolder->DeleteTask(_bstr_t(pszName), 0);
    if (FAILED(hRes))
    {
        // If it failed for some reason other than the task wasn't found
        if (hRes != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
            return kCIDLib::False;
    }

    return kCIDLib::True;
}


//
//  See if support is available. We just try to creat the task service.
//
tCIDLib::TBoolean TKrnlTaskSched::bSupportAvail()
{
    ITaskService* pService = nullptr;
    HRESULT hRes = ::CoCreateInstance
    (
        CLSID_TaskScheduler
        , NULL
        , CLSCTX_INPROC_SERVER
        , IID_ITaskService
        , tCIDLib::pToVoidPP(&pService)
    );

    if (FAILED(hRes))
        return kCIDLib::False;

    TCOMJanitor<ITaskService> janService(&pService);
    return kCIDLib::True;
}

