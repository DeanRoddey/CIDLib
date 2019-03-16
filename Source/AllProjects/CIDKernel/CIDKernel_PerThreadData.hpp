//
// FILE NAME: CIDKernel_PerThreadData.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/25/1998
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_PerThreadData.Cpp file, which
//  provides the abstractions for per-thread data storage. There are static
//  methods to get/set per-thread data slots. And the class itself is the
//  abstract base class used to store each slot down at this level. Its
//  allows us to clean them up without knowing what they actually store.
//  CIDLib.Dll will implement a derivative of this class that is a template
//  so it provides a typesafe interface and handles cleanup.
//
// CAVEATS/GOTCHAS:
//
//  1)  We have to store data in a typeless way at this level. So we take
//      and return the user's data value as a void pointer. The CIDLib
//      layer that is built on this class is a typesafe template class
//      that will handle casting the returned data back to its actual
//      type. This is a safe transformation by C++ rules.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
struct TThreadInfo;


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TKrnlPerThreadData
//  PREFIX: kptd
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlPerThreadData
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid CleanUpThreadData();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlPerThreadData();

        TKrnlPerThreadData(const TKrnlPerThreadData&) = delete;

        ~TKrnlPerThreadData();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlPerThreadData& operator=(const TKrnlPerThreadData&) = delete;
        tCIDLib::TVoid* operator new(const tCIDLib::TUInt) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual void CleanUpUserData() = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid* pUserData();

        const tCIDLib::TVoid* pUserData() const;

        tCIDLib::TVoid* pUserData
        (
                    tCIDLib::TVoid* const   pNewData
        );


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TCIDKrnlModule;


    private :
        // -------------------------------------------------------------------
        //  Private class types
        //
        //  TThreadInfo
        //      This is a simple structure used to track the information for
        //      each thread. We need to keep up with a few things per thread
        //      but only have a single slot to store it in, so this handles
        //      that issue.
        //
        //  EConstants
        //      An enum to let us have some class specific constants. The
        //      c4ArrayIncrement constant is how much the per thread slot
        //      array is grown when it overflows.
        // -------------------------------------------------------------------
        struct TThreadInfo
        {
            tCIDLib::TCard4     c4CurSlots;
            tCIDLib::TVoid**    apDataSlots;
        };

        static const tCIDLib::TCard4 c4ArrayIncrement = 64;


        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bGetThreadInfo
        (
                    TThreadInfo*&           pToFill
            , const tCIDLib::TBoolean       bFaultIn = kCIDLib::True
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Id
        //      This is set when a new per-thread object is created, and is
        //      set by the constructor. It is used to map back to where the
        //      actual data is stored.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4Id;


        // -------------------------------------------------------------------
        //  Private, static data members
        //
        //  s_apkptdList
        //      This is an array of pointers to all of the kernel per thread
        //      objects that are created. This list is required in order to
        //      clean up all of the data that is associated with a particular
        //      thread. When a thread ends, and calls CleanUpThreadData(),
        //      the list is iterated and the CleanUpUserData() virtual method
        //      is called on each element to ask it to delete its data for
        //      that thread (if any.)
        //
        //  s_c4CurDataSlotInd
        //      This is the current data slot index that is to be given out
        //      when a new per-thread object is created. It is bumped up,
        //      atomically, when each new per-thread object is created.
        //
        //  s_c4CurListSize
        //      This is the size of the s_apkptdList array. When the current
        //      data slot index gets bigger than this, we need to reallocate
        //      that list. As with the per-thread data slot array, its bumped
        //      up by the c4ArrayIncrement value.
        //
        //  s_c4SlotId
        //      This is the id of the TLS slot that we get for our use. It
        //      comes from the system's thread local services.
        //
        //  s_pkcrsLock
        //      This is a critical section used to synchronize access to the
        //      few pieces of data that are not per-thread in here. Its
        //      allocated during module init.
        // -------------------------------------------------------------------
        static TKrnlPerThreadData** s_apkptdList;
        static tCIDLib::TCard4      s_c4CurDataSlotInd;
        static tCIDLib::TCard4      s_c4CurListSize;
        static tCIDLib::TCard4      s_c4SlotId;
        static TKrnlCritSec*        s_pkcrsLock;
};

#pragma CIDLIB_POPPACK


