//
// FILE NAME: CIDLib_SafeCounter.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/11/1996
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
            // Returns previous value, so add one to do prefix correctly
            return c4AddTo(1) + 1;
        }

        tCIDLib::TCard4 operator++(int)
        {
            return c4AddTo(1);
        }

        tCIDLib::TCard4 operator--()
        {
            // Returns previous value, so sub one to do prefix correctly
            return c4SubFrom(1) - 1;
        }

        tCIDLib::TCard4 operator--(int)
        {
            return c4SubFrom(1);
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
            // Returns previous value, so add one to do prefix correctly
            return i4AddTo(1) + 1;
        }

        tCIDLib::TInt4 operator++(int)
        {
            return i4AddTo(1);
        }

        tCIDLib::TInt4 operator--()
        {
            // Returns previous value, so sub one to do prefix correctly
            return i4SubFrom(1) - 1;
        }

        tCIDLib::TInt4 operator--(int)
        {
            return i4SubFrom(1);
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

        ~TSafeCard4Janitor()
        {
            (*m_pscntToSanitize)--;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSafeCard4Janitor& operator=(const TSafeCard4Janitor&) = delete;


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

        ~TSafeInt4Janitor()
        {
            (*m_pscntToSanitize)--;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSafeInt4Janitor& operator=(const TSafeInt4Janitor&) = delete;


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

