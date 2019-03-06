//
// FILE NAME: CIDLib_SortedBag.hpp
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
template <typename TElem> class TSortedBag : public TBag<TElem>
{
    public :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------
        using TMyType = TSortedBag<TElem>;
        using TCompFunc = tCIDLib::ESortComps (*)(const TElem&, const TElem&);


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSortedBag(         TCompFunc           pfnComp
                    , const tCIDLib::ESortDirs  eDir = tCIDLib::ESortDirs::Ascending
                    , const tCIDLib::EMTStates  eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TBag<TElem>(eMTSafe)
            , m_eDir(eDir)
            , m_pfnComp(pfnComp)
        {
        }

        TSortedBag(const TMyType& colSrc) :

            TBag<TElem>(colSrc)
            , m_eDir(colSrc.m_eDir)
            , m_pfnComp(colSrc.m_pfnComp)
        {
        }

        ~TSortedBag()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMyType& operator=(const TMyType& colSrc)
        {
            if (&colSrc != this)
            {
                TParent::operator=(colSrc);
                m_eDir = colSrc.m_eDir;
                m_pfnComp = colSrc.m_pfnComp;
            }
            return *this;
        }

        TMyType& operator=(TMyType&&) = delete;


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
            typename TParent::TCursor cursAdd(this);
            while (cursAdd)
            {
                if (m_eDir == tCIDLib::ESortDirs::Ascending)
                {
                    // As soon as the bag element is >= the new
                    if (m_pfnComp(*cursAdd, objNew) != tCIDLib::ESortComps::FirstLess)
                        break;
                }
                 else
                {
                    // As soon as the new is >= a bag element
                    if (m_pfnComp(objNew, *cursAdd) != tCIDLib::ESortComps::FirstLess)
                        break;
                }
                ++cursAdd;
            }

            // If the cursor is invalid, it will go at the end
            TElem& objRet = this->objInsertAfter(objNew, cursAdd);

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


    private  :
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
        TemplateRTTIDefs(TMyType,TBag<TElem>)
};

#pragma CIDLIB_POPPACK

