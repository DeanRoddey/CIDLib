//
// FILE NAME: CIDMacroEng_StringPool_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/17/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_StringPool.cpp file, which
//  implements the TMEngStringPool class. This is for internal use during
//  parsing. We need at various times and at various levels of recursion,
//  to parse text tokens. We don't want to create and destroy strings every
//  time we come in and out of these methods, and because of recursion the
//  complexity of trying to have some temp members and track how they are
//  used so as not to clash is just too much work.
//
//  So this guy provides a pool of strings, and a string pool janitor that
//  will get a string from the pool and keep it on a scoped basis. Each
//  parser instance will create it's own pool, so no sync is required.
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TMEngStrPoolJan;

// ---------------------------------------------------------------------------
//  CLASS: TMEngStringPool
// PREFIX: mspl
// ---------------------------------------------------------------------------
class TMEngStringPool : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngStringPool();

        TMEngStringPool(const TMEngStringPool&) = delete;

        ~TMEngStringPool();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngStringPool& operator=(const TMEngStringPool&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TMEngStrPoolJan;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        TString* pstrLease
        (
                    tCIDLib::TCard4&        c4Id
        );

        tCIDLib::TVoid Return
        (
            const   tCIDLib::TCard4         c4Id
        );


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        class TPoolItem
        {
            public :
                TPoolItem() : m_bInUse(kCIDLib::False)
                {
                }

                TPoolItem(const TPoolItem& piToCopy) :

                    m_bInUse(piToCopy.m_bInUse)
                    , m_strToUse(piToCopy.m_strToUse)
                {
                }

                ~TPoolItem()
                {
                }

                tCIDLib::TVoid operator=(const TPoolItem& piToAssign)
                {
                    m_bInUse = piToAssign.m_bInUse;
                    m_strToUse = piToAssign.m_strToUse;
                }

                tCIDLib::TBoolean   m_bInUse;
                TString             m_strToUse;
        };


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colPool
        //      A vector of pool items we manage. We use a vector so that
        //      we can give back an id for each string leased. When they give
        //      it back, it's just a direct index into the vector to find the
        //      guy we are giving up.
        // -------------------------------------------------------------------
        TVector<TPoolItem>  m_colPool;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngStringPool,TObject)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngStrPoolJan
// PREFIX: mspl
// ---------------------------------------------------------------------------
class TMEngStrPoolJan
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngStrPoolJan(TMEngStringPool* pmsplSrc) :

            m_c4Id(0)
            , m_pmsplSrc(pmsplSrc)
            , m_pstrLeased(0)
        {
            m_pstrLeased = pmsplSrc->pstrLease(m_c4Id);
        }

        TMEngStrPoolJan(const TMEngStrPoolJan&) = delete;

        ~TMEngStrPoolJan()
        {
            m_pmsplSrc->Return(m_c4Id);
        }

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngStrPoolJan& operator=(const TMEngStrPoolJan&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TString& strThis()
        {
            return *m_pstrLeased;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Id
        //      The pool id of the string we got.
        //
        //  m_pmsplSrc
        //      The string pool we got the string from, which we need in order
        //      to give it back.
        //
        //  m_pstrLeased
        //      The string we leased from the pool.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4Id;
        TMEngStringPool*    m_pmsplSrc;
        TString*            m_pstrLeased;
};

#pragma CIDLIB_POPPACK


