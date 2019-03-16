//
// FILE NAME: CIDKernel_BaseLock.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/25/1997
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
//  This is the header for the CIDKernel_BaseLock.Cpp file. This file
//  implements a very simple class, TBaseLock, that is used for ad hoc
//  synchronization purposes during initialization. For instance, if a class
//  wants to lazily create statics or file scope globals, it does not make
//  any sense for them to have their own synchronization object for that
//  since they will only ever do it once (if at all) during any one run. So
//  this class provides a single, primal lock that is always available.
//
//  Just create an object of this type and you will have locked this primal
//  lock. When this object destructs the lock will be released. So its an
//  incredibly simple class and only has a default constructor and a
//  destructor.
//
//  Its not called TKrnlBaseLock because its really a publically used class,
//  but it just needs to live down here. Since it does not throw any
//  kernel exceptions this is all ok.
//
// CAVEATS/GOTCHAS:
//
//  1)  This is not to be used for general purpose synchronization because it
//      would become a bottleneck. Use it for one shot atomic operations such
//      as lazy creation of statics and such.
//
//  2)  This guy cannot throw any exceptions, or call any code that does!
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TBaseLock
//  PREFIX: lock
// ---------------------------------------------------------------------------
class KRNLEXPORT TBaseLock
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TBaseLock();

        TBaseLock(const TBaseLock&) = delete;

        ~TBaseLock();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBaseLock& operator=(const TBaseLock&) = delete;
        tCIDLib::TVoid* operator new(const size_t) = delete;
};

#pragma CIDLIB_POPPACK
