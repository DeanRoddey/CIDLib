//
// FILE NAME: CIDKernel_SafeCounter.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/11/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_SafeCounter.Cpp file, which
//  implements some simple, atomic counters (i.e. thread safe counters.)
//  There is one version for TInt4 values and one for TCard4 values.
//
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
//   CLASS: TKrnlSafeCard4Counter
//  PREFIX: scnt
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlSafeCard4Counter
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlSafeCard4Counter();

        TKrnlSafeCard4Counter
        (
            const   tCIDLib::TCard4         c4InitVal
        );

        TKrnlSafeCard4Counter(const TKrnlSafeCard4Counter&) = delete;

        ~TKrnlSafeCard4Counter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlSafeCard4Counter& operator=(const TKrnlSafeCard4Counter&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDec();

        tCIDLib::TBoolean bInc();

        tCIDLib::TCard4 c4AddTo
        (
            const   tCIDLib::TCard4         c4ToAdd
        );

        tCIDLib::TCard4 c4CompareAndExchange
        (
            const   tCIDLib::TCard4         c4New
            , const tCIDLib::TCard4         c4Compare
        );

        tCIDLib::TCard4 c4Exchange
        (
            const   tCIDLib::TCard4         c4New
        );

        tCIDLib::TCard4 c4SetValue
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4SubFrom
        (
            const   tCIDLib::TCard4         c4ToSub
        );

        tCIDLib::TCard4 c4Value() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pc4Counter
        //      This is the counter member that is incremented and decremented.
        //      It is a pointer because it may have alignment requirements for a
        //      given platform.
        //
        //  m_pLockData
        //      This is the locking data for this counter. Its just an
        //      anonymous pointer here so that it can be anything required
        //      internally.
        // -------------------------------------------------------------------
        volatile tCIDLib::TCard4*   m_pc4Counter;
        tCIDLib::TVoid*             m_pLockData;
};


// ---------------------------------------------------------------------------
//   CLASS: TKrnlSafeInt4Counter
//  PREFIX: scnt
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlSafeInt4Counter
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlSafeInt4Counter();

        TKrnlSafeInt4Counter
        (
            const   tCIDLib::TInt4          i4InitVal
        );

        TKrnlSafeInt4Counter(const TKrnlSafeInt4Counter&) = delete;

        ~TKrnlSafeInt4Counter();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlSafeInt4Counter& operator=(const TKrnlSafeInt4Counter&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDec();

        tCIDLib::TBoolean bInc();

        tCIDLib::TInt4 i4AddTo
        (
            const   tCIDLib::TInt4          i4ToAdd
        );

        tCIDLib::TInt4 i4CompareAndExchange
        (
            const   tCIDLib::TInt4          i4New
            , const tCIDLib::TInt4          i4Compare
        );

        tCIDLib::TInt4 i4Exchange
        (
            const   tCIDLib::TInt4          i4New
        );

        tCIDLib::TInt4 i4SetValue
        (
            const   tCIDLib::TInt4          i4ToSet
        );

        tCIDLib::TInt4 i4SubFrom
        (
            const   tCIDLib::TInt4          i4ToSub
        );

        tCIDLib::TInt4 i4Value() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pi4Counter
        //      This is the counter member that is incremented and decremented. It
        //      is a pointer because a given platform may have alignment requirements.
        //
        //  m_pLockData
        //      This is the locking data for this counter. Its just an
        //      anonymous pointer here so that it can be anything required
        //      internally.
        // -------------------------------------------------------------------
        volatile tCIDLib::TInt4*    m_pi4Counter;
        tCIDLib::TVoid*             m_pLockData;
};

#pragma CIDLIB_POPPACK


