//
// FILE NAME: CIDLib_Atomic.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/04/1993
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
//  TAtomic is a wrapper around some of the atomic operations from the
//  TRawMem namespace in the CIDKernel layer. Mostly you would just use those
//  but these are ones that can get errors and the kernel level stuff cannot
//  throw, so they are more awkward to use.
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
#include    "CIDLib_.hpp"

tCIDLib::TCard4 TAtomic::c4SafeAcquire(tCIDLib::TCard4& c4Target)
{
    tCIDLib::TBoolean bRes;
    const tCIDLib::TCard4 c4Ret = TRawMem::c4SafeRefAcquire(c4Target, bRes);
    if (!bRes)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcAtomic_AcquireRel
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return c4Ret;
}


tCIDLib::TCard4 TAtomic::c4SafeRelease(tCIDLib::TCard4& c4Target)
{
    tCIDLib::TBoolean bRes;
    const tCIDLib::TCard4 c4Ret = TRawMem::c4SafeRefRelease(c4Target, bRes);
    if (!bRes)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcAtomic_AcquireRel
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return c4Ret;
}
