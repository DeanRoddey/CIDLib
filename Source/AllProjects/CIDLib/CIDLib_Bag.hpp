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
template <typename TElem> class TBag : public TBasicDLinkedCol<TElem>
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
        TBag() : TParent()
        {
        }

        TBag(const TMyType&) = default;
        TBag(TMyType&&) = delete;

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

        TBag& operator=(TBag&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        TElem& objAdd(const TElem& objNew) override
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



// ---------------------------------------------------------------------------
//   CLASS: TSafeBag
//  PREFIX: col
// ---------------------------------------------------------------------------
template <typename TElem> class TSafeBag : public TBag<TElem>
{
    public  :
        // -------------------------------------------------------------------
        //  Nested class type aliases
        // -------------------------------------------------------------------
        using TMyType = TSafeBag<TElem>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSafeBag() : TParent()
        {
        }

        TSafeBag(const TMyType&) = default;
        TSafeBag(TMyType&&) = delete;

        ~TSafeBag() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSafeBag& operator=(const TSafeBag& colSrc)
        {
            if (&colSrc != this)
                TParent::operator=(colSrc);
            return *this;
        }

        TSafeBag& operator=(TSafeBag&&) = delete;


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

        TElem& objAdd(const TElem& objNew) override
        {
            // Delegate to our parent
            return TParent::objAddAtTop(objNew);
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
        DefPolyDup(TSafeBag<TElem>)
        TemplateRTTIDefs(TSafeBag<TElem>,TBag<TElem>)
};
#pragma CIDLIB_POPPACK


