//
// FILE NAME: CIDLib_Stack.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/05/1993
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
template <typename TElem> class TStack : public TBasicDLinkedCol<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStack(const tCIDLib::EMTStates eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TParent(eMTSafe)
        {
        }

        TStack(const TStack&) = default;
        TStack(TStack&&)  = default;

        ~TStack()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStack& operator=(const TStack&) = default;
        TStack& operator=(TStack&&) = default;


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
        TElem& objAddMove(TElem&& objNew)
        {
            return this->objAddAtBottom(tCIDLib::ForceMove(objNew));
        }

        const TElem& objPeek() const
        {
            return this->objPeekAtBottom();
        }

        TElem& objPeek()
        {
            return this->objPeekAtBottom();
        }

        template <typename... TArgs> TElem& objPlace(TArgs&&... Args)
        {
            return this->objPlaceAtBottom(tCIDLib::Forward<TArgs>(Args)...);
        }

        [[nodiscard]] TElem objPop()
        {
            return this->objGetFromBottom();
        }

        TElem& objPush(const TElem& objToPush)
        {
            return this->objAddAtBottom(objToPush);
        }

        TElem& objPush(TElem&& objToPush)
        {
            return this->objAddAtBottom(tCIDLib::ForceMove(objToPush));
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
template <typename TElem> class TStackJan
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

        TStackJan(TStack<TElem>* const pcolTarget, TElem&& objToPush) :

            m_pcolTarget(pcolTarget)
        {
            m_pcolTarget->objPush(tCIDLib::ForceMove(objToPush));
        }

        TStackJan(const TStackJan&) = delete;
        TStackJan(TStackJan&&) = delete;

        ~TStackJan()
        {
            // And pop it back off, if it wasn't orphaned
            if (m_pcolTarget)
                m_pcolTarget->TrashTop();
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStackJan& operator=(const TStackJan&) = delete;
        TStackJan& operator=(TStackJan&&) = delete;


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

