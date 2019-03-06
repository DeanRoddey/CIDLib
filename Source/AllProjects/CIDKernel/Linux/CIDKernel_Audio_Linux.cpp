//
// FILE NAME: CIDKernel_Audio_Linux.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/17/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the platfrom dependent parts of the TKrnlAudio
//  class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"

namespace
{
    const tCIDLib::TCard4 c4PCClock = 1193180;
    tCIDLib::TSInt iFdTTY = -1;
}

// ---------------------------------------------------------------------------
//  TFacCIDKernel: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::__bInitTermAudio(const tCIDLib::EInitTerm eState)
{
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
        TKrnlAudio::__bAudioSupport = kCIDLib::False;
        iFdTTY = ::open("/dev/tty0", O_WRONLY);
    }
    else
    {
        if (iFdTTY != -1)
            ::close(iFdTTY);
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TKrnlAudio: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKrnlAudio::Beep(const   tCIDLib::TCard4 c4Frequency
                                , const tCIDLib::TCard4 c4Duration)
{
    if (iFdTTY == -1)
        return;

    tCIDLib::TCard4 c4DurationSec = c4Duration / 1000;
    tCIDLib::TCard4 c4DurationMilliSec = c4Duration % 1000;

    struct timeval TimeVal;
    ::gettimeofday(&TimeVal, 0);

    struct timeval TargetTimeVal;
    TargetTimeVal.tv_sec = TimeVal.tv_sec + c4DurationSec;
    TargetTimeVal.tv_usec = TimeVal.tv_usec + (c4DurationMilliSec * 1000);

    struct timespec TimeRequested;
    TimeRequested.tv_sec = c4DurationSec;
    TimeRequested.tv_nsec = c4DurationMilliSec * 1000000;

    ::ioctl(iFdTTY, KIOCSOUND, c4PCClock / c4Frequency);
    struct timespec TimeRemaining;
    while (::nanosleep(&TimeRequested, &TimeRemaining) == EINTR)
    {
        ::gettimeofday(&TimeVal, 0);
        if (TimeVal.tv_sec > TargetTimeVal.tv_sec
            || (TimeVal.tv_sec == TargetTimeVal.tv_sec
            && TimeVal.tv_usec > TargetTimeVal.tv_usec))
        {
            break;
        }
        TimeRequested.tv_sec = TimeRemaining.tv_sec;
        TimeRequested.tv_nsec = TimeRemaining.tv_nsec;
    }
    ::ioctl(iFdTTY, KIOCSOUND, 0);
}


tCIDLib::TBoolean
TKrnlAudio::bPlayWavFile(const  tCIDLib::TCh* const
                        , const tCIDLib::EWaitModes)
{
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}
