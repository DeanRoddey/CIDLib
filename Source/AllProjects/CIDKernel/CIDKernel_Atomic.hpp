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
//  Copy Assign
//
//  For the non-pointer types, we can reasonably support move and copy construction.
//  We don't need to change the source in either case. We just do an exchange of the
//  value with itself to get an atomic copy.
//
//  For the pointer type, we only support move construction, not move assignment,
//  since it would be tricky to do the bi-directional exchange atomically.
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

        tCIDLib::TVoid* operator new(size_t) = delete;
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

        TAtomicCard(const TAtomicCard& atomSrc) :

            m_c4Value(TRawMem::c4Exchange(atomSrc.m_c4Value, atomSrc.m_c4Value))
        {
        }

        TAtomicCard(TAtomicCard&& atomSrc) :

            m_c4Value(TRawMem::c4Exchange(atomSrc.m_c4Value, atomSrc.m_c4Value))
        {
        }

        ~TAtomicCard() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TVoid* operator new(size_t) = delete;

        TAtomicCard& operator=(const TAtomicCard& atomSrc)
        {
            if (&atomSrc != this)
            {
                TRawMem::c4CompareAndExchange(m_c4Value, atomSrc.m_c4Value, atomSrc.m_c4Value);
            }
            return *this;
        }

        TAtomicCard& operator=(TAtomicCard&& atomSrc)
        {
            if (&atomSrc != this)
            {
                TRawMem::c4CompareAndExchange(m_c4Value, atomSrc.m_c4Value, atomSrc.m_c4Value);
            }
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4SetValue(const tCIDLib::TInt4 c4ToSet) noexcept
        {
            return TRawMem::c4Exchange(m_c4Value, c4ToSet);
        }

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

        TAtomicFlag(const TAtomicFlag& atomSrc) :

            m_c4Flag(TRawMem::c4Exchange(atomSrc.m_c4Flag, atomSrc.m_c4Flag))
        {
        }

        TAtomicFlag(TAtomicFlag&& atomSrc) :

            m_c4Flag(TRawMem::c4Exchange(atomSrc.m_c4Flag, atomSrc.m_c4Flag))
        {
        }

        ~TAtomicFlag() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TVoid* operator new(size_t) = delete;

        TAtomicFlag& operator=(const TAtomicFlag& atomSrc)
        {
            if (&atomSrc != this)
            {
                TRawMem::c4CompareAndExchange(m_c4Flag, atomSrc.m_c4Flag, atomSrc.m_c4Flag);
            }
            return *this;
        }

        TAtomicFlag& operator=(TAtomicFlag&& atomSrc)
        {
            if (&atomSrc != this)
            {
                TRawMem::c4CompareAndExchange(m_c4Flag, atomSrc.m_c4Flag, atomSrc.m_c4Flag);
            }
            return *this;
        }

        operator tCIDLib::TBoolean() const
        {
            return bValue();
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bSetValue(const tCIDLib::TBoolean bToSet) noexcept
        {
            return (TRawMem::c4Exchange(m_c4Flag, bToSet ? 1 : 0) != 0);
        }

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

        TAtomicInt(const TAtomicInt& atomSrc) :

            m_i4Value
            (
                static_cast<tCIDLib::TInt4>
                (
                    TRawMem::c4Exchange
                    (
                        *tCIDLib::pReCastPtr<tCIDLib::TCard4>(&atomSrc.m_i4Value)
                        , static_cast<tCIDLib::TCard4>(atomSrc.m_i4Value)
                    )
                )
            )
        {
        }

        TAtomicInt(TAtomicInt&& atomSrc) :

            m_i4Value
            (
                TRawMem::c4Exchange
                (
                    *tCIDLib::pReCastPtr<tCIDLib::TCard4>(&atomSrc.m_i4Value)
                    , static_cast<tCIDLib::TCard4>(atomSrc.m_i4Value)
                )
            )
        {
        }


        ~TAtomicInt() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TVoid* operator new( size_t) = delete;

        TAtomicInt& operator=(const TAtomicInt& atomSrc)
        {
            if (&atomSrc != this)
            {
                TRawMem::c4CompareAndExchange
                (
                    *tCIDLib::pReCastPtr<tCIDLib::TCard4>(&m_i4Value)
                    , atomSrc.m_i4Value
                    , atomSrc.m_i4Value
                );
            }
            return *this;
        }

        TAtomicInt& operator=(TAtomicInt&& atomSrc)
        {
            if (&atomSrc != this)
            {
                TRawMem::c4CompareAndExchange
                (
                    *tCIDLib::pReCastPtr<tCIDLib::TCard4>(&m_i4Value)
                    , atomSrc.m_i4Value
                    , atomSrc.m_i4Value);
            }
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TInt4 i4SetValue(const tCIDLib::TInt4 i4ToSet) noexcept
        {
            CIDLib_Suppress(26490) // We need the reinterpret cast here
            return static_cast<tCIDLib::TInt4>
            (
                TRawMem::c4Exchange
                (
                    reinterpret_cast<tCIDLib::TCard4&>(m_i4Value), i4ToSet
                )
            );
        }

        tCIDLib::TInt4 i4Value() const noexcept
        {
            CIDLib_Suppress(26490) // We need the reinterpret cast here
            const tCIDLib::TInt4 i4Value = static_cast<tCIDLib::TInt4>
            (
                TRawMem::c4CompareAndExchange
                (
                    *tCIDLib::pReCastPtr<tCIDLib::TCard4>(&m_i4Value), 0, 0
                )
            );
            return (i4Value != 0);
        }

        tCIDLib::TVoid SetValue(const tCIDLib::TInt4 i4ToSet) noexcept
        {
            CIDLib_Suppress(26490) // We need the reinterpret cast here
            TRawMem::c4Exchange
            (
                *tCIDLib::pReCastPtr<tCIDLib::TCard4>(&m_i4Value), i4ToSet
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

        // Copy is not supported, only move construction
        TAtomicPtr(const TAtomicPtr&) = delete;

        //
        //  We allow move contruction, but not move assignment. This one way operation
        //  we can do atomically, putting null into the caller and getting his old value
        //  to keep.
        //
        TAtomicPtr(TAtomicPtr&& atomSrc) :

            m_pValue(TRawMem::pExchangeRawPtr(&atomSrc.m_pValue, nullptr))
        {
        }

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

        // Assignment isn't meaningful for us and and move would be hard to do atomically
        TAtomicPtr& operator=(const TAtomicPtr&) = delete;
        TAtomicPtr& operator=(TAtomicPtr&&) = delete;

        // Don't allow these to be dynmically allocated
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
            // If not null, just returns current value. If null, doesn't matter and we get a null back
            return TRawMem::pCompareAndExchangePtr<T>(&m_pValue, nullptr, nullptr);
        }

        tCIDLib::TVoid SetValue(T* const pToSet) noexcept
        {
            // Swap in the new pointer and get the old out
            T* pOld = TRawMem::pExchangePtr<T>(&m_pValue, pToSet);

            // If not null we have to delete it, since we always own our objects
            if (pOld)
                delete pOld;
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
