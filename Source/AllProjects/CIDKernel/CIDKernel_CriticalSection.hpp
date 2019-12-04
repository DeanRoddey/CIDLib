//
// FILE NAME: CIDKernel_CriticalSection.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/01/1997
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
//  This is the header for the CIDKernel_CriticalSection.Cpp file. This
//  file implements the TKrnlCritSec class, which is a low overhead means
//  of protecting critical sections of code or data.  Unlike a mutex, these
//  cannot be shared among processes and cannot timeout, which is why they
//  are speedier.
//
// CAVEATS/GOTCHAS:
//
//  1)  These objects MUST be allocated appropriately aligned to work with a locked
//      exchange, because one of these is used to create the TBaseLock class. This
//      is the bootstrapping mechanism for all other situations where we need to
//      protect some quick, thread safe setup of some other type of synchronization
//      mechanism.
//
//      New/Delete operators are defined for this purpose.
//
//  2)  The actual critical section itself (m_pData) may also need such alignment
//      but that is a separate thing and can be handled internally by the platform
//      layer.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TKrnlCritSec
//  PREFIX: kcrs
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlCritSec
{
    public  :
        // -------------------------------------------------------------------
        //  Forward declare our internal platform structure
        // -------------------------------------------------------------------
        struct TPlatData;

        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlCritSec();

        TKrnlCritSec(const TKrnlCritSec&) = delete;

        ~TKrnlCritSec();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlCritSec& operator=(const TKrnlCritSec&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Enter() const noexcept;

        tCIDLib::TVoid Exit() const noexcept;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pPlatData
        //      This is the per-platform data, which they define as desired.
        // -------------------------------------------------------------------
        TPlatData*  m_pPlatData;
};


// ---------------------------------------------------------------------------
//   CLASS: TKrnlCritSecLocker
//  PREFIX: kcrsl
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlCritSecLocker
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlCritSecLocker() = delete;

        TKrnlCritSecLocker(const TKrnlCritSec* const pkcrsToLock) :

            m_pkcrsLocked(pkcrsToLock)
        {
            if (m_pkcrsLocked)
                m_pkcrsLocked->Enter();
        }

        TKrnlCritSecLocker(const TKrnlCritSecLocker&) = delete;

        ~TKrnlCritSecLocker()
        {
            Release();
        }



        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlCritSecLocker& operator=(const TKrnlCritSecLocker&) = delete;
        tCIDLib::TVoid* operator new(const size_t) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Release() noexcept
        {
            // If its been released or never was set, don't bother
            if (!m_pkcrsLocked)
                return;

            m_pkcrsLocked->Exit();
            m_pkcrsLocked = nullptr;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data
        //
        //  m_pkcrsLocked
        //      This is the stored pointer the critical section object that
        //      we are locking.
        // -------------------------------------------------------------------
        const TKrnlCritSec* m_pkcrsLocked;
};

#pragma CIDLIB_POPPACK

