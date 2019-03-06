//
// FILE NAME: CIDLib_CountedPointer.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/23/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file defines some template classes which wrap pointers to things,
//  i.e. they are 'smart pointer' classes. They act like pointers to things,
//  and provide dereference operators, but they are actually objects and have
//  'by value' semantics.
//
//  We define a base class that gets most of of the code out of line in order
//  to reduce code bloat from template instantiations. From that base class
//  we derive TCntPtr, which provides the type specific bits, but it's almost
//  all in terms of the base class.
//
//  We also provide TMngPtr from the base class, which just provides a non-
//  counted smart pointer, which just allows pointers to be used in a by
//  value manner, where there is no need to keep up with who is going to
//  delete it. This one doesn't derive from the base class since it's a
//  totally different animule.
//
//  Ane we provide TUniquePtr, which uses move semantics and disables assignment
//  and copy ctor, to insure that only one instance of it will ever have the pointer
//  that it contains (unless you of course explictly set two of them to the same
//  pointer.) It is not thread safe, but it intended to allow you to pass around a
//  pointer and insure that only the last copy of it has the original pointer, and
//  that you can't accidentally assign it to another one (you have to explicitly use
//  tCIDLib::ForceMove() to do that.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: TCntPtrBase
//  PREFIX: cptr
// ---------------------------------------------------------------------------
class CIDLIBEXP TCntPtrBase : public TObject, public MDuplicable
{
    public  :
        // -------------------------------------------------------------------
        //   CLASS: TCntDataBase
        //  PREFIX: cntd
        // -------------------------------------------------------------------
        class CIDLIBEXP TCntDataBase
        {
            public :
                // -----------------------------------------------------------
                //  Constructors and destructor
                // -----------------------------------------------------------
                TCntDataBase();

                TCntDataBase(const TCntDataBase&) = delete;

                ~TCntDataBase();


                // -----------------------------------------------------------
                //  Public operators
                // -----------------------------------------------------------
                TCntDataBase& operator=(const TCntDataBase&) = delete;


                // -----------------------------------------------------------
                //  Public, virtual methods
                // -----------------------------------------------------------
                virtual tCIDLib::TVoid CleanupData() = 0;


                // -----------------------------------------------------------
                //  Public, non-virtual methods
                // -----------------------------------------------------------
                tCIDLib::TCard4 c4RefCount() const;

                tCIDLib::TCard4 c4DecCount();

                tCIDLib::TCard4 c4IncCount();

                TCriticalSection* pcrsSync();


            private :
                // -----------------------------------------------------------
                //  Private data members
                //
                //  m_c4Count
                //      This is the reference count for the counted data. We
                //      can't store the data here since it needs to be typed.
                //      The derived classes will provided templatized derived
                //      classes.
                //
                //  m_crsSync
                //      A critical section that we use to sync access to this
                //      object. We cannot use a single global sync mechanism
                //      because of really bad deadlocks that would occur if a
                //      counted object contains thread objects and they tried
                //      to do counted pointer ops when deleted.
                // -----------------------------------------------------------
                tCIDLib::TCard4     m_c4Count;
                TCriticalSection    m_crsSync;
        };

        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TCntPtrBase() = delete;

        ~TCntPtrBase();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4RefCount() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        explicit TCntPtrBase
        (
                    TCntDataBase* const     pcntdToAdopt
        );

        TCntPtrBase
        (
            const   TCntPtrBase&            cptrSrc
        );

        tCIDLib::TVoid operator=
        (
            const   TCntPtrBase&            cptrSrc
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Destructed();

        tCIDLib::TVoid SetNewPointer
        (
                    TCntDataBase* const     pcntdToAdopt
        );

        const TCntDataBase* pcntdThis() const;

        TCntDataBase* pcntdThis();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pcntdManaged
        //      A pointer to the counted data that we are managing. We store the pointer
        //      here on behalf of all of our derived classes.
        // -------------------------------------------------------------------
        TCntDataBase*  m_pcntdManaged;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCntPtrBase,TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TCntPtr
//  PREFIX: cptr
// ---------------------------------------------------------------------------
template <class T> class TCntPtr : public TCntPtrBase
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCntPtr() :

            TCntPtrBase(new TCntPtr<T>::TCntData<T>)
        {
        }

        explicit TCntPtr(T* pToAdopt) :

            TCntPtrBase(new TCntPtr<T>::TCntData<T>(pToAdopt))
        {
        }

        TCntPtr(const TCntPtr<T>& cptrSrc) :

            TCntPtrBase(cptrSrc)
        {
        }

        ~TCntPtr()
        {
            // Tell the base class that we are destructing
            Destructed();
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TBoolean operator!() const
        {
            return static_cast<const TCntData<T>*>(pcntdThis())->m_pData == nullptr;
        }

        T* operator->()
        {
            return static_cast<TCntData<T>*>(pcntdThis())->m_pData;
        }

        const T* operator->() const
        {
            return static_cast<const TCntData<T>*>(pcntdThis())->m_pData;
        }

        T& operator*()
        {
            return *static_cast<TCntData<T>*>(pcntdThis())->m_pData;
        }

        const T& operator*() const
        {
            return *static_cast<const TCntData<T>*>(pcntdThis())->m_pData;
        }

        TCntPtr& operator=(const TCntPtr<T>& cptrSrc)
        {
            if (this != &cptrSrc)
                TParent::operator=(cptrSrc);
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DropRef()
        {
            // Set a default constructed data object on our parent
            SetNewPointer(new TCntPtr<T>::TCntData<T>);
        }

        T& objData()
        {
            return *static_cast<TCntData<T>*>(pcntdThis())->m_pData;
        }

        const T& objData() const
        {
            return *static_cast<const TCntData<T>*>(pcntdThis())->m_pData;
        }

        T* pobjData()
        {
            return static_cast<TCntData<T>*>(pcntdThis())->m_pData;
        }

        const T* pobjData() const
        {
            return static_cast<const TCntData<T>*>(pcntdThis())->m_pData;
        }

        tCIDLib::TVoid SetPointer(T* const pobjNew)
        {
            // Set a newly allocated data object on our parent
            SetNewPointer(new TCntPtr<T>::TCntData<T>(pobjNew));
        }


    private :
        // -------------------------------------------------------------------
        //   CLASS: TCntData
        //  PREFIX: cntd
        // -------------------------------------------------------------------
        template <class TD> class TCntData : public TCntDataBase
        {
            public  :
                // ------------------------------------------------------------
                //  Constructors and Destructor
                // ------------------------------------------------------------
                explicit TCntData(TD* const pToAdopt) :

                    m_pData(pToAdopt)
                {
                }

                TCntData() :

                    m_pData(nullptr)
                {
                }

                ~TCntData()
                {
                    // Should have been cleaned up, but just in case
                    try
                    {
                        delete m_pData;
                    }

                    catch(...)
                    {
                    }
                    m_pData = nullptr;
                }


                // ------------------------------------------------------------
                //  Public, inherited methods
                // ------------------------------------------------------------
                tCIDLib::TVoid CleanupData()
                {
                    // Make sure our pointer is cleared one way or another
                    TD* pTmp = m_pData;
                    m_pData = nullptr;
                    delete pTmp;
                }


                // ------------------------------------------------------------
                //  Public data members
                //
                //  m_pData
                //      This is the pointer being reference counted.
                // ------------------------------------------------------------
                TD* m_pData;
        };


        // -------------------------------------------------------------------
        //  Do any standard macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TCntPtr<T>,TCntPtrBase)
        DefPolyDup(TCntPtr<T>)
};



// ---------------------------------------------------------------------------
//   CLASS: TMngPtr
//  PREFIX: mptr
// ---------------------------------------------------------------------------
template <class T> class TMngPtr : public TObject, public MDuplicable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMngPtr() :

            m_pData(nullptr)
        {
        }

        explicit TMngPtr(T* const pData) :

            m_pData(pData)
        {
        }

        TMngPtr(const TMngPtr<T>& mptrSrc) :

            m_pData(nullptr)
        {
            m_pData = mptrSrc.m_pData;
        }

        ~TMngPtr()
        {
            // Nothing to do, we just reference the data
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
       tCIDLib::TBoolean operator!() const
        {
            return (m_pData == nullptr);
        }

        T* operator->()
        {
            return m_pData;
        }

        const T* operator->() const
        {
            return m_pData;
        }

        const T& operator*() const
        {
            return *m_pData;
        }

        T& operator*()
        {
            return *m_pData;
        }

        TMngPtr& operator=(const TMngPtr<T>& mptrSrc)
        {
            if (this == &mptrSrc)
                return *this;

            m_pData = mptrSrc.m_pData;
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        T& objData()
        {
            return *m_pData;
        }

        const T& objData() const
        {
            return *m_pData;
        }

        T* pobjData()
        {
            return m_pData;
        }

        const T* pobjData() const
        {
            return m_pData;
        }

        tCIDLib::TVoid SetPointer(T* const pToSet)
        {
            m_pData = pToSet;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pData
        //      This the pointer to the data
        // -------------------------------------------------------------------
        T*  m_pData;


        // -------------------------------------------------------------------
        //  Do any standard macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TMngPtr<T>,TObject)
        DefPolyDup(TMngPtr<T>)
};


// ---------------------------------------------------------------------------
//   CLASS: TUniquePtr
//  PREFIX: uptr
// ---------------------------------------------------------------------------
template <class T> class TUniquePtr
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TUniquePtr() :

            m_pData(nullptr)
        {
        }

        explicit TUniquePtr(T* const pAdopt) :

            m_pData(pAdopt)
        {
        }

        TUniquePtr(const TUniquePtr<T>&) = delete;

        TUniquePtr(TUniquePtr<T>&& uptrSrc) :

            m_pData(nullptr)
        {
            // Just swap the pointers, giving him our null
            tCIDLib::Swap(m_pData, uptrSrc.m_pData);
        }

        ~TUniquePtr()
        {
            try
            {
                delete m_pData;
                m_pData = nullptr;
            }

            catch(...)
            {
            }
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUniquePtr<T>& operator=(const TUniquePtr<T>&) = delete;

        T* operator->()
        {
            return m_pData;
        }

        const T* operator->() const
        {
            return m_pData;
        }

        const T& operator*() const
        {
            return *m_pData;
        }

        T& operator*()
        {
            return *m_pData;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        T* pData()
        {
            return m_pData;
        }

        const T* pData() const
        {
            return m_pData;
        }

        T* pOrphan()
        {
            T* pRet = m_pData;
            m_pData = nullptr;
            return pRet;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pData
        //      The data object we were given to handle. It can be null of course.
        // -------------------------------------------------------------------
        T*  m_pData;
};

#pragma CIDLIB_POPPACK

