//
// FILE NAME: CIDXML_NameIDPool.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/30/1999
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
//  This file implements the templatized class, TXMLNameIDPool, which is
//  used by validators to maintain pools of objects that are indexed by both
//  name and unique id.
//
//  The element class must implement the following methods:
//
//  1)  strKey()
//  2)  c4Id(const tCIDLib::TCard4 c4NewId)
//  3)  c4Id()
//
//  We also implement an iterator for the pool class.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  This class uses a hash table for fast access to the elements by
//      key (name.) For reasons of performance, we do not use a general
//      purpose hash table collection. We just implement a very simple one
//      for ourself.
//
// LOG:
//
//  $_CIDLib_Log_$
//


#pragma CIDLIB_PACK(CIDLIBPACK)


template <typename TElem> class TXMLNameIDPoolCursor;


// ---------------------------------------------------------------------------
//  CLASS: TXMLNameIDPool
// PREFIX: xnip
// ---------------------------------------------------------------------------
template <typename TElem> class TXMLNameIDPool : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public class types
        //
        //  TKeyExtract
        //      Since this class must work with any type of element, and
        //      requires access to the key field inside that element, it
        //      needs a function to extract it.
        // -------------------------------------------------------------------
        using TKeyExtract = const TString& (*)(const TElem&);


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLNameIDPool
        (
                    TKeyExtract             pfnKeyExtract
        );

        TXMLNameIDPool(const TXMLNameIDPool&) = delete;

        ~TXMLNameIDPool();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLNameIDPool& operator=(const TXMLNameIDPool&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bEmpty() const;

        tCIDLib::TBoolean bNext();

        tCIDLib::TBoolean bResetIter();

        tCIDLib::TCard4 c4AddNew
        (
                    TElem* const            pobjToAdd
        );

        tCIDLib::TCard4 c4SeqId() const;

        TElem* pobjById
        (
            const   tCIDLib::TCard4         c4Id
        );

        TElem* pobjByName
        (
            const   tCIDLib::TCh* const     pszName
        );

        TElem* pobjByName
        (
            const   TString&                strName
        );

        const TElem& objCur() const;

        TElem& objCur();

        tCIDLib::TVoid RemoveAll();


    protected :
        // -------------------------------------------------------------------
        //  The iterator class is our friend
        // -------------------------------------------------------------------
        friend class TXMLNameIDPoolCursor<TElem>;


    private :
        // -------------------------------------------------------------------
        //  Class constants
        // -------------------------------------------------------------------
        static const tCIDLib::TCard4 c4Modulus = 109;


        // -------------------------------------------------------------------
        //  Class specific data types
        // -------------------------------------------------------------------
        class TBucketElem
        {
            public :
                // -----------------------------------------------------------
                //  Constructors and Destructor
                // -----------------------------------------------------------
                TBucketElem(TElem* const            pobjToAdopt
                            , TBucketElem* const    pNextElem) :

                    pobjElem(pobjToAdopt)
                    , pNext(pNextElem)
                {
                }

                TBucketElem(const TBucketElem&) = delete;

                ~TBucketElem()
                {
                    delete pobjElem;
                }

                // -----------------------------------------------------------
                //  Public data members
                // -----------------------------------------------------------
                TElem*          pobjElem;
                TBucketElem*    pNext;
        };


        // -------------------------------------------------------------------
        //  Private non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4AddToIdArray
        (
                    TElem* const            pobjToAdd
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_aBuckets
        //      This is the list of buckets used to store the heads of the
        //      linked lists for each modulus value. Its a fixed size.
        //
        //  m_apobjById
        //      This is the 'by id' mapping structure. Its a second set of
        //      element pointers to the elements that are owned by the hash
        //      table member. This array is in id order, so by using an
        //      element's id to index this array, we get the element with
        //      that id. It grows as required.
        //
        //  m_c4IdCount
        //      This is the current size of the m_apobjById array. When the
        //      m_c4IdIndex value equals this value, its time to expand the
        //      array.
        //
        //  m_c4IdIndex
        //      This is the index into the m_apobjById array, which means
        //      that this is the value that is used to set ids on newly added
        //      elements.
        //
        //  m_c4IterBucket
        //      This is the index of the bucket list that the iterator is
        //      currently on.
        //
        //  m_c4SeqId
        //      This is the sequence id used to insure that out of date
        //      cursors are not used to access this pool.
        //
        //  m_pIter
        //      This is a pointer that represents an iterator. It points to
        //      the current bucket element that the iterator is on, or zero
        //      if at the end or not valid.
        //
        //  m_pfnKeyExtract
        //      This is the key extraction function that knows how to get
        //      the key field out of the element.
        // -------------------------------------------------------------------
        TBucketElem*    m_aBuckets[c4Modulus];
        TElem**         m_apobjById;
        tCIDLib::TCard4 m_c4IdCount;
        tCIDLib::TCard4 m_c4IdIndex;
        tCIDLib::TCard4 m_c4IterBucket;
        tCIDLib::TCard4 m_c4SeqId;
        TBucketElem*    m_pIter;
        TKeyExtract     m_pfnKeyExtract;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TXMLNameIDPool<TElem>,TObject)
};



// ---------------------------------------------------------------------------
//  CLASS: TXMLNameIDPoolCursor
// PREFIX: xnipc
// ---------------------------------------------------------------------------
template <typename TElem> class TXMLNameIDPoolCursor
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TXMLNameIDPoolCursor(const TXMLNameIDPool<TElem>* const pxnipToIter);

        TXMLNameIDPoolCursor(const TXMLNameIDPoolCursor& xnipcSrc);

        ~TXMLNameIDPoolCursor();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bNext();

        tCIDLib::TBoolean bReset();

        const TElem& objCur() const;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TXMLNameIDPoolCursor& operator=
        (
            const   TXMLNameIDPoolCursor<TElem>&    xnipcSrc
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4IterBucket
        //      The bucket within the pool's bucket list that we are currently
        //      iterating.
        //
        //  m_c4SeqId
        //      The sequence id of the pool we are iterating, as of the last
        //      reset.
        //
        //  m_pIter
        //      The pointer to the current element within the current bucket
        //      that we are on.
        //
        //  m_pxnipIter
        //      The name id pool we are iterating.
        // -------------------------------------------------------------------
        tCIDLib::TCard4                              m_c4IterBucket;
        tCIDLib::TCard4                              m_c4SeqId;
        typename TXMLNameIDPool<TElem>::TBucketElem* m_pIter;
        const TXMLNameIDPool<TElem>*                 m_pxnipIter;
};

#pragma CIDLIB_POPPACK


