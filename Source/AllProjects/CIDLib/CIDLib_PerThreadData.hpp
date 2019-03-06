//
// FILE NAME: CIDLib_PerThreadData.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/26/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_PerThreadData.Cpp file, which implements
//  the TPerThreadData class. This is a template class that allows you to
//  store a different value inside it for each thread. I.e. one per-thread
//  data object presents a different value for each thread that accesses
//  it.
//
//  No synchronization is required when dealing with per-thread data because
//  each thread sees only its own copy of the stored value.
//
// CAVEATS/GOTCHAS:
//
//  1)  These types of objects must ALWAYS be global/static objects. They
//      are guaranteed to work correctly as global/static objects. So not on the
//      stack and not dynamically allocated or as a member of something else.
//
//  2)  The stored value for each thread is initially null. Each thread must
//      set its own copy of the value if required.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TPerThreadImpl
//  PREFIX: pti
// ---------------------------------------------------------------------------
class CIDLIBEXP TPerThreadImpl : public TObject, public TKrnlPerThreadData
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TPerThreadImpl(const TPerThreadImpl&) = delete;

        ~TPerThreadImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPerThreadImpl& operator=(const TPerThreadImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid* pThis();

        const tCIDLib::TVoid* pThis() const;

        tCIDLib::TVoid* pThis
        (
                    tCIDLib::TVoid* const   pToAdopt
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden Constructor
        // -------------------------------------------------------------------
        TPerThreadImpl();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pkptdThis
        //      This is the kernel object that gives us access to the
        //      underlying system's per-thread data services.
        // -------------------------------------------------------------------
        TKrnlPerThreadData* m_pkptdThis;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TPerThreadImpl, TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TPerThreadDataFor
//  PREFIX: ptd
// ---------------------------------------------------------------------------
template <class T> class TPerThreadDataFor : public TPerThreadImpl
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPerThreadDataFor()
        {
        }

        TPerThreadDataFor(const TPerThreadDataFor<T>&) = delete;

        ~TPerThreadDataFor()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPerThreadDataFor<T>& operator=(const TPerThreadDataFor<T>&) = delete;
        tCIDLib::TVoid* operator new(const tCIDLib::TUInt) = delete;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        T* operator->()
        {
            return static_cast<T*>(pThis());
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        void CleanUpUserData()
        {
            delete pobjThis();
            pThis(nullptr);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsSet() const
        {
            // Return true if something is stored here
            return (pThis() != 0);
        }

        T& objThis()
        {
            return *static_cast<T*>(pThis());
        }

        const T& objThis() const
        {
            return *static_cast<T*>(pThis());
        }

        T* pobjThis()
        {
            return static_cast<T*>(pThis());
        }

        const T* pobjThis() const
        {
            return static_cast<T*>(pThis());
        }

        T* pobjThis(T* const pobjToAdopt)
        {
            return static_cast<T*>(pThis(pobjToAdopt));
        }


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TPerThreadDataFor<T>,TPerThreadImpl)
};

#pragma CIDLIB_POPPACK

