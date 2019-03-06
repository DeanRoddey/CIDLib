//
// FILE NAME: CIDKernel_DVDTypes.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/20/2006
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  Implements some helper methods for the DVD types header.
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
#include    "CIDKernel_DVDTypes.hpp"


// ---------------------------------------------------------------------------
//  NAMESPACE: TDVDTypes
// ---------------------------------------------------------------------------

//
//  Converts a DVD time structure into a number of frames. The values in the
//  DVD time structure are in BCD format, I guess to make them human readable
//  directly. So we can have up to 99 hours, minutes, seconds, and the frames
//  field indicates the frame rate in the top two bits and the bottom six are
//  the left over frames less than a full second.
//
tCIDLib::TCard8 TDVDTypes::c8DVDTimeToFrames(const tDVDTypes::TDVDTime& ToCvt)
{
    tCIDLib::TCard8 c8Ret;
    tCIDLib::TCard4 c4Val;

    c4Val = ((ToCvt.c1Hour >> 4) * 10);
    c4Val += (ToCvt.c1Hour & 0xF);
    c4Val *= 3600;
    c8Ret = c4Val;

    c4Val = ((ToCvt.c1Minute >> 4) * 10);
    c4Val += (ToCvt.c1Minute & 0xF);
    c4Val *= 60;
    c8Ret += c4Val;

    c4Val = ((ToCvt.c1Second >> 4) * 10);
    c4Val += (ToCvt.c1Second & 0xF);
    c8Ret += c4Val;

    // Figure out the frame rate
    if (((ToCvt.c1Frames & 0xC0) >> 6) == 1)
        c8Ret *= 25;
    else
        c8Ret *= 30;

    // Add the remaining frames
    c4Val = (((ToCvt.c1Frames & 0x3F) >> 4) * 10);
    c4Val += (ToCvt.c1Frames & 0xF);
    c8Ret += c4Val;

    return c8Ret;
}

