//
// FILE NAME: CIDLib_Deque.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/06/1996
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
//  This is the header for the deque template collection. It implements the
//  TDeque class template, which provides LIFO and FIFO storage of its elements
//  by value.
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
//   CLASS: TDeque
//  PREFIX: deq
// ---------------------------------------------------------------------------
template <typename TElem> class TDeque : public TBasicDLinkedCol<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDeque() : TParent()
        {
        }

        TDeque(const TDeque& colSrc) :

            TParent(colSrc)
        {
        }

        TDeque(TDeque&&) = delete;

        ~TDeque()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDeque& operator=(const TDeque& colSrc)
        {
            if (this != &colSrc)
                TParent::operator=(colSrc);
            return *this;
        }

        TDeque& operator=(TDeque&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        TElem& objAdd(const TElem& objNew) final
        {
            // Delegate to our parent
            return this->objAddAtBottom(objNew);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bPopBottom(TElem& objToFill)
        {
            // Delegate to our parent
            return this->bGetFromBottom(objToFill);
        }

        tCIDLib::TBoolean bPopTop(TElem& objToFill)
        {
            // Delegate to our parent
            return this->bGetFromTop(objToFill);
        }

        const TElem& objPeekBottom() const
        {
            // Delegate to our parent
            return this->objPeekAtBottom();
        }

        const TElem& objPeekTop() const
        {
            // Delegate to our parent
            return this->objPeekAtTop();
        }

        template <typename... TArgs> TElem& objPlaceBottom(TArgs&&... Args)
        {
            return this->objPlaceAtBottom(tCIDLib::Forward<TArgs>(Args)...);
        }

        template <typename... TArgs> TElem& objPlaceTop(TArgs&&... Args)
        {
            return this->objPlaceAtTop(tCIDLib::Forward<TArgs>(Args)...);
        }

        TElem objPopBottom()
        {
            // Delegate to our parent
            return this->objGetFromBottom();
        }

        TElem objPopTop()
        {
            // Delegate to our parent
            return this->objGetFromTop();
        }

        TElem& objPushBottom(const TElem& objToAdd)
        {
            // Delegate to our parent
            return this->objAddAtBottom(objToAdd);
        }

        TElem& objPushTop(const TElem& objToAdd)
        {
            // Delegate to our parent
            return this->objAddAtTop(objToAdd);
        }

        tCIDLib::TVoid PopBottom()
        {
            this->DiscardBottom();
        }

        tCIDLib::TVoid PopTop()
        {
            this->DiscardTop();
        }


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        DefPolyDup(TDeque<TElem>)
        TemplateRTTIDefs(TDeque<TElem>,TBasicDLinkedCol<TElem>)
};


// ---------------------------------------------------------------------------
//   CLASS: TSafeDeque
//  PREFIX: col
// ---------------------------------------------------------------------------
template <typename TElem> class TSafeDeque : public TDeque<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSafeDeque() : TParent()
        {
        }

        TSafeDeque(const TSafeDeque& colSrc) : TParent(colSrc)
        {
        }

        TSafeDeque(TSafeDeque&&)  = delete;

        ~TSafeDeque()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSafeDeque& operator=(const TSafeDeque& colSrc)
        {
            if (this != &colSrc)
                TParent::operator=(colSrc);
            return *this;
        }

        TSafeDeque& operator=(TSafeDeque&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTryLock(const tCIDLib::TCard4 c4WaitMS) const final
        {
            return m_mtxSync.bTryLock(c4WaitMS);
        }

        tCIDLib::TVoid Lock(const tCIDLib::TCard4 c4WaitMSs) const final
        {
            m_mtxSync.Lock(c4WaitMSs);
        }

        tCIDLib::EMTStates eMTSafe() const final
        {
            return tCIDLib::EMTStates::Safe;
        }

        tCIDLib::TVoid Unlock() const final
        {
            m_mtxSync.Unlock();
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_mtxSync
        //      We override the MLockable interface and implement it in terms
        //      of this guy.
        // -------------------------------------------------------------------
        TMutex  m_mtxSync;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        DefPolyDup(TSafeDeque<TElem>)
        TemplateRTTIDefs(TSafeDeque<TElem>,TDeque<TElem>)
};


#pragma CIDLIB_POPPACK

