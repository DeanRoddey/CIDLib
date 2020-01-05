//
// FILE NAME: CIDLib_RefDeque.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/17/2008
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
//  TRefDeque class template, which provides LIFO and FIFO storage of its
//  elements by reference.
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
//   CLASS: TRefDeque
//  PREFIX: col
// ---------------------------------------------------------------------------
template <typename TElem> class TRefDeque : public TBasicDLinkedRefCol<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRefDeque() = delete;

        TRefDeque(const tCIDLib::EAdoptOpts eAdopt) :

            TParent(eAdopt)
        {
        }

        TRefDeque(const TRefDeque&) = delete;
        TRefDeque(TRefDeque&&) = delete;

        ~TRefDeque()
        {
        }

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TRefDeque& operator=(const TRefDeque&) = delete;
        TRefDeque& operator=(TRefDeque&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Add(TElem* const pobjNew) final
        {
            this->AddAtBottom(pobjNew);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TElem* pobjPeekBottom() const
        {
            // Delegate to our parent
            return this->pobjPeekAtBottom();
        }

        TElem* pobjPeekBottom()
        {
            // Delegate to our parent
            return this->pobjPeekAtBottom();
        }

        const TElem* pobjPeekTop() const
        {
            // Delegate to our parent
            return this->pobjPeekAtTop();
        }

        TElem* pobjPeekTop()
        {
            // Delegate to our parent
            return this->pobjPeekAtTop();
        }

        [[nodiscard]] TElem* pobjPopBottom()
        {
            // Delegate to our parent
            return this->pobjGetFromBottom();
        }

        [[nodiscard]] TElem* pobjPopTop()
        {
            // Delegate to our parent
            return this->pobjGetFromTop();
        }

        tCIDLib::TVoid PushBottom(TElem* const pobjToPush)
        {
            // Delegate to our parent
            this->AddAtBottom(pobjToPush);
        }

        tCIDLib::TVoid PushTop(TElem* const pobjToAdd)
        {
            // Delegate to our parent
            this->AddAtTop(pobjToAdd);
        }

        tCIDLib::TVoid PopBottom()
        {
            TElem* pobjPop = this->pobjGetFromBottom();
            if (this->eAdopt() == tCIDLib::EAdoptOpts::Adopt)
                delete pobjPop;
        }

        tCIDLib::TVoid PopTop()
        {
            TElem* pobjPop = this->pobjGetFromTop();
            if (this->eAdopt() == tCIDLib::EAdoptOpts::Adopt)
                delete pobjPop;
        }


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TRefDeque<TElem>,TBasicDLinkedRefCol<TElem>)
};


// ---------------------------------------------------------------------------
//   CLASS: TSafeRefDeque
//  PREFIX: col
// ---------------------------------------------------------------------------
template <typename TElem> class TSafeRefDeque : public TRefDeque<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSafeRefDeque() = delete;

        TSafeRefDeque(const tCIDLib::EAdoptOpts eAdopt) : TParent(eAdopt)
        {
        }

        TSafeRefDeque(const TSafeRefDeque&) = delete;
        TSafeRefDeque(TSafeRefDeque&&) = delete;

        ~TSafeRefDeque()
        {
        }

        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSafeRefDeque& operator=(const TSafeRefDeque&) = delete;
        TSafeRefDeque& operator=(TSafeRefDeque&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTryLock(const tCIDLib::TCard4 c4WaitMS) const final
        {
            return m_mtxSync.bTryLock(c4WaitMS);
        }

        tCIDLib::EMTStates eMTSafe() const final
        {
            return tCIDLib::EMTStates::Safe;
        }

        tCIDLib::TVoid Lock(const tCIDLib::TCard4 c4WaitMSs) const final
        {
            m_mtxSync.Lock(c4WaitMSs);
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
        //      We override the MLockable interface and implement them in terms
        //      of this guy.
        // -------------------------------------------------------------------
        TMutex  m_mtxSync;

        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TSafeRefDeque<TElem>,TRefDeque<TElem>)
};

#pragma CIDLIB_POPPACK

