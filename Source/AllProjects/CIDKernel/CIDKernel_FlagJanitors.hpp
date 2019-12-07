//
// FILE NAME: CIDKernel_FlagJanitors.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/11/1994
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
//  This header implements a couple of small, simple janitor classes that
//  allow the client to modify a fundamental type flag temporarily and then
//  to set it back upon exit of the scope. There is also one for enums which
//  is a template.
//
//  And we have another templated one that is a sort of magic one that takes
//  a lambda expression, which it will call when it destructs. The lambda can
//  do whatever you want, including having captures of data it can operate
//  on.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TBoolJanitor
//  PREFIX: jan
// ---------------------------------------------------------------------------
class KRNLEXPORT TBoolJanitor
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBoolJanitor() = delete;

        TBoolJanitor(       tCIDLib::TBoolean* const pbToRestore
                    , const tCIDLib::TBoolean        bNewValue) :

            m_bOldValue(kCIDLib::False)
            , m_pbToSanitize(pbToRestore)
        {
            if (m_pbToSanitize)
            {
                m_bOldValue = *m_pbToSanitize;
                *m_pbToSanitize = bNewValue;
            }
        }

        TBoolJanitor(const TBoolJanitor&) = delete;
        TBoolJanitor(TBoolJanitor&&) = delete;

        ~TBoolJanitor()
        {
            if (m_pbToSanitize)
                *m_pbToSanitize = m_bOldValue;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBoolJanitor& operator=(const TBoolJanitor&) = delete;
        TBoolJanitor& operator=(TBoolJanitor&&) = delete;
        tCIDLib::TVoid* operator new(const size_t)  = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Release()
        {
            m_pbToSanitize = nullptr;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data
        //
        //  m_bOldValue
        //      This is the stored previous value to restore.
        //
        //  m_pbToSanitize
        //      This is the stored address of the boolean flag that we are
        //      to sanitize.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bOldValue;
        tCIDLib::TBoolean*  m_pbToSanitize;
};


// ---------------------------------------------------------------------------
//   CLASS: TCardJanitor
//  PREFIX: jan
// ---------------------------------------------------------------------------
class KRNLEXPORT TCardJanitor
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCardJanitor() = delete;

        TCardJanitor(       tCIDLib::TCard4* const  pc4ToRestore
                    , const tCIDLib::TCard4         c4NewValue) :

            m_c4OldValue(0)
            , m_pc4ToSanitize(pc4ToRestore)
        {
            if (m_pc4ToSanitize)
            {
                m_c4OldValue = *m_pc4ToSanitize;
                *m_pc4ToSanitize = c4NewValue;
            }
        }

        TCardJanitor(const TCardJanitor&) = delete;
        TCardJanitor(TCardJanitor&&) = delete;

        ~TCardJanitor()
        {
            if (m_pc4ToSanitize)
                *m_pc4ToSanitize = m_c4OldValue;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCardJanitor& operator=(const TCardJanitor&) = delete;
        TCardJanitor& operator=(TCardJanitor&&) = delete;
        tCIDLib::TVoid* operator new(const size_t) = delete;

        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Release()
        {
            m_pc4ToSanitize = nullptr;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data
        //
        //  m_c4OldValue
        //      This is the stored previous value to restore.
        //
        //  m_pc4ToSanitize
        //      This is the stored address of the integer flag that we are
        //      to sanitize.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4OldValue;
        tCIDLib::TCard4*    m_pc4ToSanitize;
};



// ---------------------------------------------------------------------------
//   CLASS: TIntJanitor
//  PREFIX: jan
// ---------------------------------------------------------------------------
class KRNLEXPORT TIntJanitor
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TIntJanitor() = delete;

        TIntJanitor(        tCIDLib::TInt4* const   pi4ToRestore
                    , const tCIDLib::TInt4          i4NewValue) :

            m_i4OldValue(0)
            , m_pi4ToSanitize(pi4ToRestore)
        {
            if (m_pi4ToSanitize)
            {
                m_i4OldValue = *m_pi4ToSanitize;
                *m_pi4ToSanitize = i4NewValue;
            }
        }

        TIntJanitor(const TIntJanitor&) = delete;
        TIntJanitor(TIntJanitor&&) = delete;

        ~TIntJanitor()
        {
            if (m_pi4ToSanitize)
                *m_pi4ToSanitize = m_i4OldValue;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TIntJanitor& operator=(const TIntJanitor&) = delete;
        TIntJanitor& operator=(TIntJanitor&&) = delete;
        tCIDLib::TVoid* operator new(const size_t) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Release()
        {
            m_pi4ToSanitize = nullptr;
        }

    private :
        // -------------------------------------------------------------------
        //  Private data
        //
        //  m_i4OldValue
        //      This is the stored previous value to restore.
        //
        //  m_pi4ToSanitize
        //      This is the stored address of the integer flag that we are
        //      to sanitize.
        // -------------------------------------------------------------------
        tCIDLib::TInt4  m_i4OldValue;
        tCIDLib::TInt4* m_pi4ToSanitize;
};



// ---------------------------------------------------------------------------
//  CLASS: TLambdaJan
// PREFIX: jan
// ---------------------------------------------------------------------------
template <typename CB> class TLambdaJan
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TLambdaJan() = delete;

        TLambdaJan(CB pfnCallback) :

            m_bOrphaned(kCIDLib::False)
            , m_pfnCallback(pfnCallback)
        { }

        TLambdaJan(const TLambdaJan&) = delete;
        TLambdaJan(TLambdaJan&&) = delete;

        ~TLambdaJan()
        {
            // If we still have the callback, invoke it
            if (!m_bOrphaned)
            {
                //
                //  We can't force these to be no-except, we just have to suppress
                //  this and if they cause an exception it's going to be fatal.
                //
                #pragma warning(suppress : 26447)
                m_pfnCallback();
            }
        }

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLambdaJan& operator=(const TLambdaJan&) = delete;
        TLambdaJan& operator=(TLambdaJan&&) = delete;
        tCIDLib::TVoid* operator new(const size_t) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOrphaned() const
        {
            return m_bOrphaned;
        }

        tCIDLib::TVoid Orphan()
        {
            m_bOrphaned = kCIDLib::True;
        }

    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bOrphaned;
        CB                  m_pfnCallback;
};


// ---------------------------------------------------------------------------
//   CLASS: TPtrJanitor
//  PREFIX: jan
//
//  This one will set a pointer on a scoped basis and then put back the original
//  value. The parameter must in include the constness of the pointer.
// ---------------------------------------------------------------------------
template <class T> class TPtrJanitor
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPtrJanitor() = delete;

        TPtrJanitor(T*& ptToRestore, T* const ptNewValue) :

            m_ptOldValue(ptToRestore)
            , m_pptToSanitize(&ptToRestore)
        {
            *m_pptToSanitize = ptNewValue;
        }

        TPtrJanitor(const TPtrJanitor&) = delete;
        TPtrJanitor(TPtrJanitor&&) = delete;

        ~TPtrJanitor()
        {
            if (m_pptToSanitize)
                *m_pptToSanitize = m_ptOldValue;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPtrJanitor& operator=(const TPtrJanitor&) = delete;
        TPtrJanitor& operator=(TPtrJanitor&&) = delete;
        tCIDLib::TVoid* operator new(const size_t) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        T& operator*()
        {
            return **m_pptToSanitize;
        }

        tCIDLib::TVoid Release()
        {
            m_pptToSanitize = nullptr;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data
        //
        //  m_ptOldValue
        //      This is the previous value that was in the pointer, so it's a pointer
        //      to a T.
        //
        //  m_pptToSanitize
        //      A pointer to the pointer we are sanitizing, so that we can put back
        //      the previous value.
        // -------------------------------------------------------------------
        T*  m_ptOldValue;
        T** m_pptToSanitize;
};



// ---------------------------------------------------------------------------
//   CLASS: TGFJanitor
//  PREFIX: jan
//
//  This generic flag janitor allows for arbitrary type to be sanitized. It's
//  for things like pointers to things that we want to set on a scoped basis
//  or enumvalues that need to be set on a scoped basis, that kind of thing.
// ---------------------------------------------------------------------------
template <class T> class TGFJanitor
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TGFJanitor() = delete;

        TGFJanitor(T* const ptToRestore, const T tNewValue) :

            m_tOldValue()
            , m_ptToSanitize(ptToRestore)
        {
            if (m_ptToSanitize)
            {
                m_tOldValue = *m_ptToSanitize;
                *m_ptToSanitize = tNewValue;
            }
        }

        TGFJanitor(const TGFJanitor&) = delete;
        TGFJanitor(TGFJanitor&&) = delete;

        ~TGFJanitor()
        {
            if (m_ptToSanitize)
                *m_ptToSanitize = m_tOldValue;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGFJanitor& operator=(const TGFJanitor&) = delete;
        TGFJanitor& operator=(TGFJanitor&&) = delete;
        tCIDLib::TVoid* operator new(const size_t) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Release()
        {
            m_ptToSanitize = nullptr;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data
        //
        //  m_tOldValue
        //      This is the stored previous value to restore.
        //
        //  m_ptToSanitize
        //      This is the stored address of the value that we are to
        //      sanitize.
        // -------------------------------------------------------------------
        T   m_tOldValue;
        T*  m_ptToSanitize;
};


#pragma CIDLIB_POPPACK
