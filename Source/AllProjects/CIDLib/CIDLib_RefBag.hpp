//
// FILE NAME: CIDLib_RefBag.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/06/2001
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
//  This is the header for the ref bag collection template. It implements the
//  TRefBag class template, which provides unordered storage of its elements
//  by pointer.
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
//   CLASS: TRefBag
//  PREFIX: col
// ---------------------------------------------------------------------------
template <typename TElem> class TRefBag : public TBasicDLinkedRefCol<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------
        using TMyType = TRefBag<TElem>;
        using TParType = TBasicDLinkedRefCol<TElem>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRefBag() = delete;

        TRefBag(const   tCIDLib::EAdoptOpts eAdopt
                , const tCIDLib::EMTStates  eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TParType(eAdopt, eMTSafe)
        {
        }

        TRefBag(const TRefBag&) = delete;
        TRefBag(TRefBag&& colSrc) = default;
        ~TRefBag() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TRefBag& operator=(const TRefBag&) = delete;
        TRefBag& operator=(TRefBag&&) = default;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Add(TElem* const pobjToAdd) override
        {
            // Delegate to our parent
            this->AddAtTop(pobjToAdd);
        }


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TRefBag<TElem>,TBasicDLinkedRefCol<TElem>)
};

#pragma CIDLIB_POPPACK
