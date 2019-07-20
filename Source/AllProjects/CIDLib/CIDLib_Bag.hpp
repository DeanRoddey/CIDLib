//
// FILE NAME: CIDLib_Bag.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/30/1996
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
//  This is the header for the bag collection template. It implements the TBag
//  class template, which provides unordered storage of its elements by value.
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
//   CLASS: TBag
//  PREFIX: col
// ---------------------------------------------------------------------------
template <class TElem> class TBag : public TBasicDLinkedCol<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Nested class type aliases
        // -------------------------------------------------------------------
        using TMyElemType = TElem;
        using TMyType = TBag<TElem>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBag(const tCIDLib::EMTStates eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TParent(eMTSafe)
        {
        }

        TBag(const TMyType&) = default;

        TBag(TMyType&& colSrc) : TParent(colSrc.eMTState())
        {
            *this = operator=(tCIDLib::ForceMove(colSrc));
        }

        ~TBag() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBag& operator=(const TBag& colSrc)
        {
            if (&colSrc != this)
                TParent::operator=(colSrc);
            return *this;
        }

        TBag& operator=(TBag&& colSrc)
        {
            TParent::operator=(tCIDLib::ForceMove(colSrc));
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        TElem& objAdd(const TElem& objNew)
        {
            // Delegate to our parent
            return TParent::objAddAtTop(objNew);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        template <typename... TArgs> TElem& objPlace(TArgs&&... Args)
        {
            return this->objPlaceAtTop(tCIDLib::Forward<TArgs>(Args)...);
        }


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        DefPolyDup(TBag<TElem>)
        TemplateRTTIDefs(TBag<TElem>,TBasicDLinkedCol<TElem>)
};

#pragma CIDLIB_POPPACK


