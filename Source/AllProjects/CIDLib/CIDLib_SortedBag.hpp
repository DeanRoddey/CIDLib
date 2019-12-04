//
// FILE NAME: CIDLib_SortedBag.hpp
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
//  This header defines a very simple derivative of TBag, TSortedBag, which
//  just adds the capability to insert new objects in sorted order. All it
//  requires, beyond the usual RTTI and constructor/destructor stuff, is to
//  override the objAdd() method and find the correct sorted add position.
//
//  The comp function cannot be a capturing lambda because we have no way to
//  specialize a template on a lambda. So we have to define it as a regular
//  function pointer.
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
//   CLASS: TSortedBag
//  PREFIX: bag
// ---------------------------------------------------------------------------
template <typename TElem> class TSortedBag : public TBasicDLinkedCol<TElem>
{
    public :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------
        using TMyElemType = TElem;
        using TMyType = TSortedBag<TElem>;
        using TCompFunc = tCIDLib::ESortComps (*)(const TElem&, const TElem&);


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSortedBag(         TCompFunc           pfnComp
                    , const tCIDLib::ESortDirs  eDir = tCIDLib::ESortDirs::Ascending
                    , const tCIDLib::EMTStates  eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TParent(eMTSafe)
            , m_c4UserData(0)
            , m_eDir(eDir)
            , m_pfnComp(pfnComp)
        {
        }

        TSortedBag(const TMyType& colSrc) :

            TParent(colSrc)
            , m_c4UserData(colSrc.m_c4UserData)
            , m_eDir(colSrc.m_eDir)
            , m_pfnComp(colSrc.m_pfnComp)
        {
        }

        TSortedBag(TMyType&& colSrc) :

            TParent(colSrc.eMTState())
            , m_c4UserData(0)
            , m_eDir(colSrc.m_eDir)
            , m_pfnComp(nullptr)
        {
            *this = operator=(tCIDLib::ForceMove(colSrc));
        }

        ~TSortedBag() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMyType& operator=(const TMyType& colSrc)
        {
            if (&colSrc != this)
            {
                TParent::operator=(colSrc);
                m_c4UserData = colSrc.m_c4UserData;
                m_eDir = colSrc.m_eDir;
                m_pfnComp = colSrc.m_pfnComp;
            }
            return *this;
        }

        TMyType& operator=(TMyType&& colSrc)
        {
            TParent::operator=(tCIDLib::ForceMove(colSrc));
            tCIDLib::Swap(m_c4UserData, colSrc.m_c4UserData);
            tCIDLib::Swap(m_eDir, colSrc.m_eDir);
            tCIDLib::Swap(m_pfnComp, colSrc.m_pfnComp);
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        TElem& objAdd(const TElem& objNew) override
        {
            // Lock the collection
            TMtxLocker lockCol(this->pmtxLock());

            // Optimize if the bag is empty
            if (this->bIsEmpty())
                return this->objAddAtTop(objNew);

            // If the cursor is invalid, it will go at the end
            TParent::TCursor cursAt = cursFindInsert(objNew);
            TElem& objRet = this->objInsertAfter(objNew, cursAt);

            // Invalidate any cursors
            this->c4IncSerialNum();

            return objRet;
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::ESortDirs eDir() const
        {
            return m_eDir;
        }

        tCIDLib::ESortDirs eDir(const tCIDLib::ESortDirs eNew)
        {
            m_eDir = eNew;
            return m_eDir;
        }

        //
        //  In this case, this is only a convenience, no improvement in performance
        //  since we have to create the object first in order to find out where to
        //  put it. Only when we are empty can we just forward the arguments and
        //  add it directly.
        //
        template <typename... TArgs> TElem& objPlace(TArgs&&... Args)
        {
            // Lock the collection
            TMtxLocker lockCol(this->pmtxLock());

            // Optimize if the bag is empty
            if (this->bIsEmpty())
                return this->objPlaceAtTop(tCIDLib::Forward<TArgs>(Args)...);

            // If the cursor is invalid, it will go at the end
            TElem objNew(tCIDLib::Forward<TArgs>(Args)...);
            TParent::TCursor cursAt(cursFindInsert(objNew));
            TElem& objRet = this->objInsertAfter(objNew, cursAt);

            // Invalidate any cursors
            this->c4IncSerialNum();

            return objRet;
        }


    private  :
        // -------------------------------------------------------------------
        //  Find the insertion point for a new object
        // -------------------------------------------------------------------
        TBasicDLinkedCol<TElem>::TCursor cursFindInsert(const TElem& objNew)
        {
            TBasicDLinkedCol<TElem>::TCursor cursRet(this);

            //
            //  Find the position to put the new element at. We just iterate
            //  the nodes until we find one that is greater than (or less
            //  than if descending) the new object. We then insert the new
            //  element before it.
            //
            //  Note that we the comp func returns true if the first is less,
            //  so false means greater than or equal, so we not the return
            //  and arrange the two compared elements so that we won't compare
            //  the new element against a bunch of elements in the list that
            //  are equal.
            //
            while (cursRet)
            {
                if (m_eDir == tCIDLib::ESortDirs::Ascending)
                {
                    // As soon as the bag element is >= the new
                    if (m_pfnComp(*cursRet, objNew) != tCIDLib::ESortComps::FirstLess)
                        break;
                }
                 else
                {
                    // As soon as the new is >= a bag element
                    if (m_pfnComp(objNew, *cursRet) != tCIDLib::ESortComps::FirstLess)
                        break;
                }
                ++cursRet;
            }
            return cursRet;
        }


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4UserData
        //      Given to us to pass on to the comparitor function, if needed.
        //
        //  m_eDir
        //      Indicates whether we are to sort ascending or descending.
        //
        //  m_pfnComp
        //      The comparator function to use for comparing objects to find
        //      their insert position.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4UserData;
        tCIDLib::ESortDirs  m_eDir;
        TCompFunc           m_pfnComp;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        DefPolyDup(TMyType)
        TemplateRTTIDefs(TMyType, TBasicDLinkedCol<TElem>)
};

#pragma CIDLIB_POPPACK

