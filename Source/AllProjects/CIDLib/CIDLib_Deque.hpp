//
// FILE NAME: CIDLib_Deque.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/06/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
template <class TElem> class TDeque : public TBasicDLinkedCol<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TCursor = TBasicDLinkedCol::TConstCursor<TElem>;
        using TNCCursor = TBasicDLinkedCol::TNonConstCursor<TElem>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDeque(const tCIDLib::EMTStates eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TParent(eMTSafe)
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
        TElem& objAdd(const TElem& objNew) override
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

#pragma CIDLIB_POPPACK

