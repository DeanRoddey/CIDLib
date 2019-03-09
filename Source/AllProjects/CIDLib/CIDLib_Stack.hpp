//
// FILE NAME: CIDLib_Stack.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/05/1993
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the public header for the stack collection template. It implements
//  the TStack class template, which provides LIFO storage of its elements by
//  value.
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
//   CLASS: TStack
//  PREFIX: col
// ---------------------------------------------------------------------------
template <class TElem> class TStack : public TBasicDLinkedCol<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TCursor   = TBasicDLinkedCol::TConstCursor<TElem>;
        using TNCCursor = TBasicDLinkedCol::TNonConstCursor<TElem>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStack(const tCIDLib::EMTStates eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TParent(eMTSafe)
        {
        }

        TStack(const TStack& colSrc) :

            TBasicDLinkedCol<TElem>(colSrc)
        {
        }

        TStack(TStack&& colSrc) :

            TStack()
        {
            *this = tCIDLib::ForceMove(colSrc);
        }

        ~TStack()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStack& operator=(const TStack& colSrc)
        {
            if (this != &colSrc)
                TParent::operator=(colSrc);
            return *this;
        }

        TStack& operator=(TStack&& colSrc)
        {
            TParent::operator=(tCIDLib::ForceMove(colSrc));
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        TElem& objAdd(const TElem& objNew) override
        {
            return this->objAddAtBottom(objNew);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TElem& objPeek() const
        {
            return this->objPeekAtBottom();
        }

        TElem& objPeek()
        {
            return this->objPeekAtBottom();
        }

        TElem objPop()
        {
            return this->objGetFromBottom();
        }

        TElem& objPush(const TElem& objToPush)
        {
            return this->objAddAtBottom(objToPush);
        }

        tCIDLib::TVoid SwapTop()
        {
            // Our parent class can do this safely and efficiently
            this->ExchangeBottom();
        }

        tCIDLib::TVoid TrashTop()
        {
            // Get the top but don't return it
            this->objGetFromBottom();
        }


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        DefPolyDup(TStack<TElem>)
        TemplateRTTIDefs(TStack<TElem>,TBasicDLinkedCol<TElem>)
};


// ---------------------------------------------------------------------------
//   CLASS: TStackJan
//  PREFIX: jan
// ---------------------------------------------------------------------------
template <class TElem> class TStackJan
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStackJan() = delete;

        TStackJan(TStack<TElem>* const pcolTarget, const TElem& objToPush) :

            m_pcolTarget(pcolTarget)
        {
            m_pcolTarget->objPush(objToPush);
        }

        TStackJan(const TStackJan&) = delete;

        ~TStackJan()
        {
            // And pop it back off, if it wasn't orphaned
            if (m_pcolTarget)
                m_pcolTarget->objPop();
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStackJan& operator=(const TStackJan&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Orphan()
        {
            m_pcolTarget = nullptr;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pcolTarget
        //      This is a pointer to the stack we are to pop when we exit. If
        //      it gets orphaned, this is zeroed.
        // -------------------------------------------------------------------
        TStack<TElem>*  m_pcolTarget;
};

#pragma CIDLIB_POPPACK

