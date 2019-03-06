//
// FILE NAME: CIDLib_Bag.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/30/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
        //  Class types
        // -------------------------------------------------------------------
        using TCursor = TBasicDLinkedCol::TConstCursor<TElem>;
        using TNCCursor = TBasicDLinkedCol::TNonConstCursor<TElem>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBag(const tCIDLib::EMTStates eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TParent(eMTSafe)
        {
        }

        TBag(const TBag& colSrc) : TParent(colSrc)
        {
        }

        TBag(TBag&&) = delete;

        ~TBag()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBag& operator=(const TBag& colSrc)
        {
            if (&colSrc != this)
                TParent::operator=(colSrc);
            return *this;
        }

        TBag& operator=(TBag&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        TElem& objAdd(const TElem& objNew)
        {
            // Delegate to our parent
            return TParent::objAddAtTop(objNew);
        }


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        DefPolyDup(TBag<TElem>)
        TemplateRTTIDefs(TBag<TElem>,TBasicDLinkedCol<TElem>)
};

#pragma CIDLIB_POPPACK


