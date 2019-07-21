//
// FILE NAME: CIDLib_Atomic.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/20/2019
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
//  This name space provides some more convenient wrappers around some of the
//  CIDKernel level atomic operations in TRawMem. Those cannot throw exceptions
//  because they don't know about such things.
//
//  Really low level code should use the CIDKernel versions. But other stuff can
//  use these.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

namespace TAtomic
{
    CIDLIBEXP tCIDLib::TCard4 c4SafeAcquire
    (
        volatile    tCIDLib::TCard4&    c4Target
    );

    CIDLIBEXP tCIDLib::TCard4 c4SafeRelease
    (
        volatile    tCIDLib::TCard4&    c4Target
    );
}
