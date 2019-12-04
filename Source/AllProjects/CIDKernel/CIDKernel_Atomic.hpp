//
// FILE NAME: CIDKernel_Atomic.hpp
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
//  This is the header for the CIDKernel_Atomic.Cpp file. This file
//  implements two very fundamental classes that are used for boostrapping
//  purposes in a multi-threaded world.
//
//  1.  TBaseLock is a fundamental lock object that our per-platform code
//      guarantees will be available from the very lowest level forward.
//  2.  TAtomicFlag is aclass  that allows for fenced atomic get/set of
//      a boolean flag.
//
//  These two together are used for faulting in things in a thread safe way.
//
//  NOTE that, for just single objects that don't require any decisions or
//  or that don't call any methods to set parameters, the language will safely
//  handle those. So put a static inside a lazy eval method that returns the
//  static, and the language will initialize it safely on first use.
//
//  But, where there's multiple things to initialize or decisions have be
//  made or other things gen'd up to pass to the thing that will be created,
//  we have to fend for ourself. The scheme should be:
//
//      static TAtomicFlag atomDone;
//      static TWhatever1* pw1;
//      static TWhatever2* pw2;
//
//      tCIDLib::TVoid SomeMethod()
//      {
//          if (!atomDone)
//          {
//              TBaseLock lockInit;
//              if (!atomDone)
//              {
//                  // Do init
//                  // ....
//
//                  atomDone.Set();
//              }
//          }
//      }
//
//  Since the TAtomicFlag get/set methods provide full memory fences, that means
//  that we WILL see the done flag in the first bValue if it has been set. And it
//  means that the set WILL be seen by everyone before exiting the inner loop and
//  releasing the lock.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  The base lock is not to be used for general purpose synchronization because
//      it would become a bottleneck. Use it for fundamental bootstrapping stuff
//      where single object (compiler provided) safe static init isn't enough.
//
//  2)  These guys cannot throw any exceptions, or call any code that does!
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
        TBaseLock(TBaseLock&&) = delete;

        ~TBaseLock();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBaseLock& operator=(const TBaseLock&) = delete;
        TBaseLock& operator=(TBaseLock&&) = delete;

        tCIDLib::TVoid* operator new(const size_t) = delete;
};




// ---------------------------------------------------------------------------
//   CLASS: TAtomicCard
//  PREFIX: atom
// ---------------------------------------------------------------------------
class TAtomicCard
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TAtomicCard() noexcept : m_c4Value(0) {}
        TAtomicCard(const tCIDLib::TInt4 c4InitVal) : m_c4Value(c4InitVal) {}
        TAtomicCard(const TAtomicCard&) = delete;
        TAtomicCard(TAtomicCard&&) = delete;
        ~TAtomicCard() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TAtomicCard& operator=(const TAtomicCard&) = delete;
        tCIDLib::TVoid* operator new( size_t) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TInt4 c4Value() const noexcept
        {
            const tCIDLib::TInt4 c4Value = TRawMem::c4CompareAndExchange(m_c4Value, 0, 0);
            return (c4Value != 0);
        }

        tCIDLib::TVoid SetValue(const tCIDLib::TInt4 c4ToSet) noexcept
        {
            TRawMem::c4Exchange(m_c4Value, c4ToSet);
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Value
        //      A cache aligned TInt4 bit value that we can do atomic ops on. It's
        //      mutable because just reading it requires a compare and exchange.
        // -------------------------------------------------------------------
        alignas(kCIDLib::c4CacheAlign) mutable tCIDLib::TCard4 m_c4Value;
};



// ---------------------------------------------------------------------------
//   CLASS: TAtomicFlag
//  PREFIX: atom
// ---------------------------------------------------------------------------
class TAtomicFlag
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TAtomicFlag() : m_c4Flag(0) {}
        TAtomicFlag(const tCIDLib::TBoolean bInitVal) : m_c4Flag(bInitVal ? 1 : 0) {}
        TAtomicFlag(const TAtomicFlag&) = delete;
        TAtomicFlag(TAtomicFlag&&) = delete;
        ~TAtomicFlag() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TAtomicFlag& operator=(const TAtomicFlag&) = delete;
        tCIDLib::TVoid* operator new(size_t) = delete;

        operator tCIDLib::TBoolean() const
        {
            return bValue();
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bValue() const noexcept
        {
            const tCIDLib::TCard4 c4Value = TRawMem::c4CompareAndExchange(m_c4Flag, 0, 0);
            return (c4Value != 0);
        }

        tCIDLib::TVoid SetValue(const tCIDLib::TBoolean bToSet) noexcept
        {
            TRawMem::c4Exchange(m_c4Flag, bToSet ? 1 : 0);
        }

        tCIDLib::TVoid Set() noexcept
        {
            SetValue(kCIDLib::True);
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Flag
        //      A cache aligned 32 bit value that we can do atomic ops on. It's
        //      mutable because just reading it requires a compare and exchange.
        // -------------------------------------------------------------------
        alignas(kCIDLib::c4CacheAlign) mutable tCIDLib::TCard4 m_c4Flag;
};


// ---------------------------------------------------------------------------
//   CLASS: TAtomicInt
//  PREFIX: atom
// ---------------------------------------------------------------------------
class TAtomicInt
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TAtomicInt() : m_i4Value(0) {}
        TAtomicInt(const tCIDLib::TInt4 i4InitVal) : m_i4Value(i4InitVal) {}
        TAtomicInt(const TAtomicInt&) = delete;
        TAtomicInt(TAtomicInt&&) = delete;
        ~TAtomicInt() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TAtomicInt& operator=(const TAtomicInt&) = delete;
        tCIDLib::TVoid* operator new( size_t) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TInt4 i4Value() const noexcept
        {
            const tCIDLib::TInt4 i4Value = TRawMem::c4CompareAndExchange
            (
                reinterpret_cast<tCIDLib::TCard4&>(m_i4Value), 0, 0
            );
            return (i4Value != 0);
        }

        tCIDLib::TVoid SetValue(const tCIDLib::TInt4 i4ToSet) noexcept
        {
            TRawMem::c4Exchange
            (
                reinterpret_cast<tCIDLib::TCard4&>(m_i4Value), i4ToSet
            );
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_i4Value
        //      A cache aligned TInt4 bit value that we can do atomic ops on. It's
        //      mutable because just reading it requires a compare and exchange.
        // -------------------------------------------------------------------
        alignas(kCIDLib::c4CacheAlign) mutable tCIDLib::TInt4 m_i4Value;
};



// ---------------------------------------------------------------------------
//   CLASS: TAtomicPtr
//  PREFIX: atom
// ---------------------------------------------------------------------------
template <typename T> class TAtomicPtr
{
    public  :
        // -------------------------------------------------------------------
        //  Constuctors and Destructor
        // -------------------------------------------------------------------
        TAtomicPtr() : m_pValue(nullptr) {}
        TAtomicPtr(const T* const pInitVal) : m_pValue(pInitVal) {}
        TAtomicPtr(const TAtomicPtr&) = delete;
        TAtomicPtr(TAtomicPtr&&) = delete;

        // We always adopt the pointer
        ~TAtomicPtr()
        {
            try
            {
                delete m_pValue;
                m_pValue = nullptr;
            }

            catch(...)
            {
                //
                //  We can't really do much since these are almost always going
                //  to be at static scope and destroyed after our code is done.
                //
            }
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TAtomicPtr& operator=(const TAtomicPtr&) = delete;
        tCIDLib::TVoid* operator new( size_t) = delete;

        operator tCIDLib::TBoolean() const
        {
            return (pValue() != nullptr);
        }

        T* operator->() const
        {
            return pValue();
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        T* pValue() const noexcept
        {
            return TRawMem::pCompareAndExchangePtr<T>
            (
                &m_pValue, nullptr, nullptr
            );
        }

        tCIDLib::TVoid SetValue(T* const pToSet) noexcept
        {
            TRawMem::pExchangePtr<T>(&m_pValue, pToSet);
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pValue
        //      A cache aligned pointer value that we can do atomic ops on. It's
        //      mutable because just reading it requires a compare and exchange.
        // -------------------------------------------------------------------
        alignas(kCIDLib::c4CacheAlign) mutable T*   m_pValue;
};
#pragma CIDLIB_POPPACK
