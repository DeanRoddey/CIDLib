//
// FILE NAME: CIDKernel_HiPerfTimer_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/25/2021
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2021
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the platform specific aspects of the TKrnlHPtimer
//  namespace.
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
#include    <profileapi.h>

#pragma comment(lib, "ntdll.lib")


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDKernel_HiPerfTimer_Win32
    {
        //
        //  We'll set this to the number of ticks per microsecond reported by this platform.
        //  We divide the current counter by this to get current microsecond offset.
        //
        tCIDLib::TCard8 c8FreqDiv = 0;

        //
        //  This is the ticks per second resolution we return, microseconds. The high
        //  perf counter is guarantted to be higher than microsecond resolution, so we
        //  we shouldn't ever be lower than what's available.
        //
        constexpr tCIDLib::TCard8 c8OurRes = 1000000;
    }
}


// We need to access an undocumented call for this
extern "C" NTSYSAPI NTSTATUS NTAPI NtDelayExecution(IN BOOLEAN Alertable,  IN PLARGE_INTEGER Interval);


// Our initialization call, which the kernel init will call.
tCIDLib::TBoolean TCIDKrnlModule::bInitTermHPTimer(const tCIDLib::EInitTerm eState)
{
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
       // The frequency divisor has not been set yet, so let's get it
        LARGE_INTEGER Freq = {};
        if (!QueryPerformanceFrequency(&Freq))
        {
            TKrnlPopup::Show
            (
                CID_FILE
                , CID_LINE
                , L"High Performance Timer Support"
                , nullptr
                , kKrnlErrs::errcGen_NotSupported
                , 0
                , 0
                , L"High performance timers are not supported on this system"
            );
            return kCIDLib::False;
        }

        CIDKernel_HiPerfTimer_Win32::c8FreqDiv = Freq.QuadPart / CIDKernel_HiPerfTimer_Win32::c8OurRes;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlHPTimer::bHighResDelay(const tCIDLib::TCard4 c4MicroSecs)
{
    LARGE_INTEGER SleepTime;
    SleepTime.QuadPart = -1 * (static_cast<tCIDLib::TInt8>(c4MicroSecs) * 10);
    NtDelayExecution(false, &SleepTime);
    return kCIDLib::True;
}

tCIDLib::TCard8 TKrnlHPTimer::c8GetTicks()
{
    LARGE_INTEGER Counter;
    ::QueryPerformanceCounter(&Counter);

    return static_cast<tCIDLib::TCard8>(Counter.QuadPart / CIDKernel_HiPerfTimer_Win32::c8FreqDiv);
}
