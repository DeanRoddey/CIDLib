//
// FILE NAME: CIDKernel_HiPerfTimer.hpp
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
//  This is the header for the CIDKernel_HiPerfTimer.Cpp file, which implements
//  the TKrnlHPTimer namespace. This provides a current time stamp at a much higher
//  resolution than the regular millisecond tick counter. The system provides
//  varying levels of resolution, but we always return them as microseconds, which
//  is more than enough. Like the millisecond timer it's just a running counter.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  NAMESPACE: TKrnlHPTimer
// ---------------------------------------------------------------------------
namespace TKrnlHPTimer
{
    tCIDLib::TBoolean KRNLEXPORT bHighResDelay(const tCIDLib::TCard4 c4MicroSecs);
    tCIDLib::TCard8 KRNLEXPORT c8GetTicks();
};

#pragma CIDLIB_POPPACK
