//
// FILE NAME: CIDLib_SafeCounter.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/11/1996
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
//  This header implements some simple, atomic counters (i.e. its thread
//  safe) classes that can be incremented, decremented, queried, added to,
//  subtracted from, and exchanged.
//
//  We also provide simple inc/dec janitors for these class, which make them
//  much safer and more efficient to use.
//
// CAVEATS/GOTCHAS:
//
//  1)  The underlying kernel wrapper class provides all of the public
//      interface we need and does not expose any APIs that these classes
//      would need to hide. So these class just derive from the kernel class
//      directly and let it provide the public API without any passthrough
//      wrappers.
//
//  2)  Since these classes are all inline, their RTTI macros are in
//      CIDLib.Cpp.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TSafeCard4Counter
//  PREFIX: scnt
// ---------------------------------------------------------------------------
class CIDLIBEXP TSafeCard4Counter : public TObject, public TKrnlSafeCard4Counter
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSafeCard4Counter()
        {
        }

        TSafeCard4Counter(const tCIDLib::TCard4 c4InitVal) :

            TKrnlSafeCard4Counter(c4InitVal)
        {
        }

        TSafeCard4Counter(const TSafeCard4Counter&) = delete;

        ~TSafeCard4Counter()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSafeCard4Counter& operator=(const TSafeCard4Counter&) = delete;

        tCIDLib::TCard4 operator++()
        {
            return c4Inc();
        }

        tCIDLib::TCard4 operator++(int)
        {
            // We have to sub one for the return for post semantics
            return c4Inc() - 1;
        }

        tCIDLib::TCard4 operator--()
        {
            return c4Dec();
        }

        tCIDLib::TCard4 operator--(int)
        {
            // We have to add one fpr the return for post semantics
            return c4Dec() + 1;
        }

        tCIDLib::TCard4 operator*() const
        {
            return c4Value();
        }

        tCIDLib::TBoolean operator==(const tCIDLib::TCard4 c4Val) const
        {
            return(c4Value() == c4Val);
        }

        tCIDLib::TBoolean operator!=(const tCIDLib::TCard4 c4Val) const
        {
            return !operator==(c4Val);
        }

        tCIDLib::TBoolean operator==(const TSafeCard4Counter& scntSrc) const
        {
            return(c4Value() == scntSrc.c4Value());
        }

        tCIDLib::TBoolean operator!=(const TSafeCard4Counter& scntSrc) const
        {
            return !operator==(scntSrc);
        }


    private :
        // -------------------------------------------------------------------
        //  Do any needed macros
        // -------------------------------------------------------------------
        RTTIDefs(TSafeCard4Counter,TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TSafeInt4Counter
//  PREFIX: scnt
// ---------------------------------------------------------------------------
class CIDLIBEXP TSafeInt4Counter : public TObject, public TKrnlSafeInt4Counter
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSafeInt4Counter()
        {
        }

        TSafeInt4Counter(const tCIDLib::TInt4 i4InitVal) :

            TKrnlSafeInt4Counter(i4InitVal)
        {
        }

        TSafeInt4Counter(const TSafeInt4Counter&) = delete;

        ~TSafeInt4Counter()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSafeInt4Counter& operator=(const TSafeInt4Counter&) = delete;

        tCIDLib::TInt4 operator++()
        {
            return i4Inc();
        }

        tCIDLib::TInt4 operator++(int)
        {
            // Sub one for the return value for post-inc semantics
            return i4Inc() - 1;
        }

        tCIDLib::TInt4 operator--()
        {
            return i4Dec();
        }

        tCIDLib::TInt4 operator--(int)
        {
            // Add one tp the return value for post-inc semantics
            return i4Dec() + 1;
        }

        tCIDLib::TInt4 operator*() const
        {
            return i4Value();
        }

        tCIDLib::TBoolean operator==(const tCIDLib::TInt4 i4Val) const
        {
            return(i4Value() == i4Val);
        }

        tCIDLib::TBoolean operator!=(const tCIDLib::TInt4 i4Val) const
        {
            return !operator==(i4Val);
        }

        tCIDLib::TBoolean operator==(const TSafeInt4Counter& scntSrc) const
        {
            return(i4Value() == scntSrc.i4Value());
        }

        tCIDLib::TBoolean operator!=(const TSafeInt4Counter& scntSrc) const
        {
            return !operator==(scntSrc);
        }


    private :
        // -------------------------------------------------------------------
        //  Do any needed macros
        // -------------------------------------------------------------------
        RTTIDefs(TSafeInt4Counter,TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TSafeCard4Janitor
//  PREFIX: jan
// ---------------------------------------------------------------------------
class CIDLIBEXP TSafeCard4Janitor
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSafeCard4Janitor() = delete;

        TSafeCard4Janitor(TSafeCard4Counter* const pscntToSanitize) :

            m_pscntToSanitize(pscntToSanitize)
        {
            (*m_pscntToSanitize)++;
        }

        TSafeCard4Janitor(const TSafeCard4Janitor&) = delete;
        TSafeCard4Janitor(TSafeCard4Janitor&&) = delete;

        ~TSafeCard4Janitor()
        {
            (*m_pscntToSanitize)--;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSafeCard4Janitor& operator=(const TSafeCard4Janitor&) = delete;
        TSafeCard4Janitor& operator=(TSafeCard4Janitor&&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pscntToSanitize
        //      This is the safe counter we sanitize. We increment it during
        //      construction and decrement it on destruction.
        // -------------------------------------------------------------------
        TSafeCard4Counter*  m_pscntToSanitize;
};


// ---------------------------------------------------------------------------
//   CLASS: TSafeInt4Janitor
//  PREFIX: jan
// ---------------------------------------------------------------------------
class CIDLIBEXP TSafeInt4Janitor
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSafeInt4Janitor() = delete;

        TSafeInt4Janitor(TSafeInt4Counter* const pscntToSanitize) :

            m_pscntToSanitize(pscntToSanitize)
        {
            (*m_pscntToSanitize)++;
        }

        TSafeInt4Janitor(const TSafeInt4Janitor&) = delete;
        TSafeInt4Janitor(TSafeInt4Janitor&&) = delete;

        ~TSafeInt4Janitor()
        {
            (*m_pscntToSanitize)--;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSafeInt4Janitor& operator=(const TSafeInt4Janitor&) = delete;
        TSafeInt4Janitor& operator=(TSafeInt4Janitor&&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pscntToSanitize
        //      This is the safe counter we sanitize. We increment it during
        //      construction and decrement it on destruction.
        // -------------------------------------------------------------------
        TSafeInt4Counter*  m_pscntToSanitize;
};

#pragma CIDLIB_POPPACK

