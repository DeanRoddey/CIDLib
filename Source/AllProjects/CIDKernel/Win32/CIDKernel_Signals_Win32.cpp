//
// FILE NAME: CIDKernel_SignalHandler_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/25/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides the Win32 specific signal handling stuff.
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


// ---------------------------------------------------------------------------
//  Local static methods
// ---------------------------------------------------------------------------

//
//  This function is installed during init below, as the handler for signals.
//  It just calls a TKrnlSignals method to pass the event on to the outside
//  world.
//
static BOOL WINAPI bSignalHandler(DWORD dwFlag)
{
    // Translate the system id into an enum of ours
    tCIDLib::ESignals eSignal = tCIDLib::ESignals::Unknown;
    switch(dwFlag)
    {
        case CTRL_C_EVENT :
            eSignal = tCIDLib::ESignals::CtrlC;
            break;

        case CTRL_BREAK_EVENT :
            eSignal = tCIDLib::ESignals::Break;
            break;

        case CTRL_CLOSE_EVENT :
            eSignal = tCIDLib::ESignals::CloseConsole;
            break;

        case CTRL_LOGOFF_EVENT :
            eSignal = tCIDLib::ESignals::LogOff;
            break;

        case CTRL_SHUTDOWN_EVENT :
            eSignal = tCIDLib::ESignals::Shutdown;
            break;

        default :
            break;
    }

    // And invoke all our handlers
    const tCIDLib::TBoolean bRes = TKrnlSignals::bCallHandlers(eSignal);

    //
    //  If the handler returned true, then it wants to supress the normal
    //  processing, so return TRUE. Else return FALSE.
    //
    if (bRes)
        return TRUE;
    return FALSE;
}



// ---------------------------------------------------------------------------
//  TKrnlSignals methods
// ---------------------------------------------------------------------------

//
//  The platform independent section calls this during its initialization.
//  At this point, the platform independent handler system infrastructure is
//  set up.
//
tCIDLib::TVoid TKrnlSignals::PlatformInit()
{
    // Set the local handler as our console control handler
    if (!::SetConsoleCtrlHandler(bSignalHandler, 1))
        TKrnlError::ThrowHostError(::GetLastError());
}


tCIDLib::TVoid TKrnlSignals::GenerateCtrlCSignal()
{
    ::GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
}

