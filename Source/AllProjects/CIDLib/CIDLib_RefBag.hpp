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
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRefBag() = delete;

        TRefBag(const tCIDLib::EAdoptOpts eAdopt) :

            TParent(eAdopt)
        {
        }

        TRefBag(const TRefBag&) = delete;
        TRefBag(TRefBag&&) = delete;

        ~TRefBag()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TRefBag& operator=(const TRefBag&) = delete;
        TRefBag& operator=(TRefBag&&) = delete;


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


// ---------------------------------------------------------------------------
//   CLASS: TSafeRefBag
//  PREFIX: col
// ---------------------------------------------------------------------------
template <typename TElem> class TSafeRefBag : public TRefBag<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSafeRefBag() = delete;

        TSafeRefBag(const tCIDLib::EAdoptOpts eAdopt) :

            TParent(eAdopt)
        {
        }

        TSafeRefBag(const TSafeRefBag&) = delete;
        TSafeRefBag(TSafeRefBag&&) = delete;

        ~TSafeRefBag()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSafeRefBag& operator=(const TSafeRefBag&) = delete;
        TSafeRefBag& operator=(TSafeRefBag&&) = delete;


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
        TemplateRTTIDefs(TSafeRefBag<TElem>, TRefBag<TElem>)
};

#pragma CIDLIB_POPPACK
