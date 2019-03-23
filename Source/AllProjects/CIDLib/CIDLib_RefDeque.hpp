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
template <class TElem> class TRefDeque :  public TBasicDLinkedRefCol<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRefDeque() = delete;

        TRefDeque(  const   tCIDLib::EAdoptOpts eAdopt
                    , const tCIDLib::EMTStates  eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TParent(eAdopt, eMTSafe)
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
        tCIDLib::TVoid Add(TElem* const pobjNew) override
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

        TElem* pobjPopBottom()
        {
            // Delegate to our parent
            return this->pobjGetFromBottom();
        }

        TElem* pobjPopTop()
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

#pragma CIDLIB_POPPACK

