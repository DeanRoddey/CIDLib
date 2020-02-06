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
        TDeque(const tCIDLib::EMTStates eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TParent(eMTSafe)
        {
        }

        TDeque(const TDeque&) = default;
        TDeque(TDeque&&) = default;

        ~TDeque()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDeque& operator=(const TDeque&) = default;
        TDeque& operator=(TDeque&&) = default;


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

        TElem& objAddMove(TElem&& objNew)
        {
            // Delegate to our parent
            return this->objAddAtBottom(tCIDLib::ForceMove(objNew));
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
            return this->objGetFromBottom();
        }

        TElem objPopTop()
        {
            return this->objGetFromTop();
        }

        TElem& objPushBottom(const TElem& objToAdd)
        {
            return this->objAddAtBottom(objToAdd);
        }

        TElem& objPushBottom(TElem&& objToAdd)
        {
            return this->objAddAtBottom(tCIDLib::ForceMove(objToAdd));
        }

        TElem& objPushTop(const TElem& objToAdd)
        {
            return this->objAddAtTop(objToAdd);
        }

        TElem& objPushTop(TElem&& objToAdd)
        {
            return this->objAddAtTop(tCIDLib::ForceMove(objToAdd));
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

