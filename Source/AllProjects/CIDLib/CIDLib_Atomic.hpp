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
//  low level atomic operations in TRawMem. Those cannot throw exceptions
//  because they don't know about such things. Really low level code should use
//  the CIDKernel versions. But other stuff can use these.
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
    inline tCIDLib::TCard4
    c4CompareAndExchange(volatile   tCIDLib::TCard4&    c4ToFill
                        , const     tCIDLib::TCard4     c4New
                        , const     tCIDLib::TCard4     c4Compare)
    {
        return TRawMem::c4CompareAndExchange(c4ToFill, c4New, c4Compare);
    }

    inline tCIDLib::TCard4
    c4Exchange(tCIDLib::TCard4& c4ToFill, const tCIDLib::TCard4 c4New)
    {
        return TRawMem::c4Exchange(c4ToFill, c4New);
    }

    CIDLIBEXP tCIDLib::TCard4 c4SafeAcquire
    (
        volatile    tCIDLib::TCard4&    c4Target
    );

    CIDLIBEXP tCIDLib::TCard4 c4SafeRelease
    (
        volatile    tCIDLib::TCard4&    c4Target
    );


    template <class T> T* pExchangePtr(T** ppToFill, T* const pNew)
    {
        return reinterpret_cast<T*>
        (
            TRawMem::pExchangeRawPtr
            (
                reinterpret_cast<tCIDLib::TVoid**>(ppToFill), pNew
            )
        );
    }

    template <class T> T* pCompareAndExchangePtr(T**        ppToFill
                                                , T* const  pNew
                                                , T* const  pCompare)
    {
        return reinterpret_cast<T*>
        (
            TRawMem::pCompareAndExchangeRawPtr
            (
                reinterpret_cast<tCIDLib::TVoid**>(ppToFill), pNew, pCompare
            )
        );
    }
}
