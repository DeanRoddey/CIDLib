//
// FILE NAME: CIDLib_ObjectArray.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/21/1995
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
//  This is the header for the CIDLib_ObjArray.Cpp file. This file
//  implements the TObjArray class, which is a templatize array of objects.
//  TObjArray is not a derivative of TCollection because it does not fit
//  well into the standard collection scheme.
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TObjArray
//  PREFIX: obja
// ---------------------------------------------------------------------------
template <typename TElem, typename TIndex = tCIDLib::TCard4>
class TObjArray : public TObject, public MDuplicable, public MLockable
{
    public  :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TMyType = TObjArray<TElem, TIndex>;


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TClass& clsThis()
        {
            static const TClass clsRet(L"TObjArrary<TElem,TIndex>");
            return clsRet;
        }


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TObjArray() = delete;

        TObjArray(  const   TIndex              tElemCount
                    , const tCIDLib::EMTStates  eMTSafe = tCIDLib::EMTStates::Unsafe) :

            m_c4ElemCount(0)
            , m_c4SerialNum(0)
            , m_paobjList(nullptr)
            , m_pmtxLock(nullptr)
        {
            const tCIDLib::TCard4 c4ElemCount = tCIDLib::TCard4(tElemCount);

            // Can't be zero size
            if (!c4ElemCount)
            {
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcCol_ZeroSize
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::BadParms
                );
            }

            // Allocate the object array
            m_c4ElemCount = c4ElemCount;
            m_paobjList = new TElem[c4ElemCount];

            if (eMTSafe == tCIDLib::EMTStates::Safe)
                m_pmtxLock = new TMutex;
        }

        TObjArray(const TMyType& objaSrc) :

            m_c4ElemCount(objaSrc.m_c4ElemCount)
            , m_c4SerialNum(0)
            , m_paobjList(nullptr)
            , m_pmtxLock(nullptr)
        {
            // Lock the source
            TLocker lockrSource(&objaSrc);

            // Allocate the node array
            m_paobjList = new TElem[m_c4ElemCount];

            for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4ElemCount; c4Ind++)
                m_paobjList[c4Ind] = objaSrc.m_paobjList[c4Ind];

           // Create a mutex if the source has one
            if (objaSrc.m_pmtxLock)
                m_pmtxLock = new TMutex;
        }

        // Set up a with 1 element
        TObjArray(TMyType&& objaSrc) :

            TObjArray(static_cast<TIndex>(1), objaSrc.eMTState())
        {
            operator=(tCIDLib::ForceMove(objaSrc));
        }

        ~TObjArray()
        {
            try
            {
                delete [] m_paobjList;
                m_paobjList = nullptr;

                delete m_pmtxLock;
                m_pmtxLock = nullptr;
            }

            catch(TError& errToCatch)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        const TElem& operator[](const TIndex tIndex) const
        {
            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);

            // Lock this object
            TLocker lockrSource(this);
            VerifyIndex(c4Index, CID_LINE);
            return m_paobjList[c4Index];
        }

        TElem& operator[](const TIndex tIndex)
        {
            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);

            // Lock this object
            TLocker lockrSource(this);
            VerifyIndex(c4Index, CID_LINE);
            return m_paobjList[c4Index];
        }

        tCIDLib::TBoolean operator==(const TMyType& objaSrc) const
        {
            if (this == &objaSrc)
                return kCIDLib::True;

            // Lock this object and the source
            TLocker lockrThis(this);
            TLocker lockrSource(&objaSrc);

            if (objaSrc.m_c4ElemCount != m_c4ElemCount)
                return kCIDLib::False;

            // Compare all the objects
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4ElemCount; c4Index++)
            {
                if (m_paobjList[c4Index] != objaSrc.m_paobjList[c4Index])
                    return kCIDLib::False;
            }
            return kCIDLib::True;
        }

        tCIDLib::TBoolean operator!=(const TMyType& objaSrc) const
        {
            return !operator==(objaSrc);
        }

        TMyType& operator=(const TMyType& objaSrc)
        {
            if (this != &objaSrc)
            {
                TLocker lockrUs(this);
                TLocker lockrSource(&objaSrc);

                // Delete and reallocate and if not the same size
                if (m_c4ElemCount != objaSrc.m_c4ElemCount)
                {
                    delete [] m_paobjList;
                    m_paobjList = new TElem[objaSrc.m_c4ElemCount];
                }

                // Copy over the members
                m_c4ElemCount = objaSrc.m_c4ElemCount;
                for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4ElemCount; c4Ind++)
                    m_paobjList[c4Ind] = objaSrc.m_paobjList[c4Ind];

                // Bump the serial number
                m_c4SerialNum++;
            }
            return *this;
        }

        // As always only swap content, not the lock. Both serial numbers are bumped
        TMyType& operator=(TMyType&& objaSrc)
        {
            if (this != &objaSrc)
            {
                TLocker lockrUs(this);
                TLocker lockrSource(&objaSrc);

                tCIDLib::Swap(objaSrc.m_c4ElemCount, m_c4ElemCount);
                tCIDLib::Swap(objaSrc.m_paobjList, m_paobjList);

                m_c4SerialNum++;
                objaSrc.m_c4SerialNum++;
            }
            return *this;
        }


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::EMTStates eMTSafe() const
        {
            return tCIDLib::EMTStates::Unsafe;
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsDescendantOf(const TClass& clsTarget) const override
        {
            if (clsTarget == clsThis())
                return kCIDLib::True;
            return TObject::bIsDescendantOf(clsTarget);
        }

        tCIDLib::TBoolean bTryLock(const tCIDLib::TCard4 c4WaitMSs) const override
        {
            // WE aren't lockable, so just say it worked
            return kCIDLib::True;
        }

        const TClass& clsIsA() const final
        {
            return clsThis();
        }

        const TClass& clsParent() const final
        {
            return TObject::clsThis();
        }

        tCIDLib::TVoid Lock(const tCIDLib::TCard4) const override
        {
            // A no-op for us
        }

        tCIDLib::TVoid Unlock() const override
        {
            // A no-op for us
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------

        // Call back for each element. Can be a lambda or function
        template <typename IterCB> tCIDLib::TBoolean bForEach(IterCB iterCB) const
        {
            TLocker lockrThis(this);
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4ElemCount; c4Index++)
            {
                if (!iterCB(m_paobjList[c4Index]))
                    return kCIDLib::False;
            }
            return kCIDLib::True;
        }

        template <typename IterCB> tCIDLib::TBoolean bForEachNC(IterCB iterCB)
        {
            TLocker lockrThis(this);
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4ElemCount; c4Index++)
            {
                if (!iterCB(m_paobjList[c4Index]))
                    return kCIDLib::False;
            }
            return kCIDLib::True;
        }

        template <typename IterCB> tCIDLib::TBoolean bForEachI(IterCB iterCB) const
        {
            TLocker lockrThis(this);
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4ElemCount; c4Index++)
            {
                if (!iterCB(m_paobjList[c4Index], c4Index))
                    return kCIDLib::False;
            }
            return kCIDLib::True;
        }

        template <typename IterCB> tCIDLib::TBoolean bForEachNCI(IterCB iterCB)
        {
            TLocker lockrThis(this);
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4ElemCount; c4Index++)
            {
                if (!iterCB(m_paobjList[c4Index], c4Index))
                    return kCIDLib::False;
            }
            return kCIDLib::True;
        }

        tCIDLib::TBoolean bIsMTSafe() const
        {
            return (m_pmtxLock != nullptr);
        }

        tCIDLib::TCard4 c4SerialNum() const
        {
            // Lock this object
            tCIDLib::TCard4 c4Ret;
            {
                TLocker lockrThis(this);
                c4Ret = m_c4SerialNum;
            }
            return c4Ret;
        }

        tCIDLib::EMTStates eMTState() const
        {
            if (m_pmtxLock)
                return tCIDLib::EMTStates::Safe;
            return tCIDLib::EMTStates::Unsafe;
        }

        tCIDLib::TVoid ExchangeElems(   const   TIndex  tFirst
                                        , const TIndex  tSecond)
        {
            const tCIDLib::TCard4 c4First = tCIDLib::TCard4(tFirst);
            const tCIDLib::TCard4 c4Second = tCIDLib::TCard4(tSecond);

            // Lock this object
            TLocker lockrThis(this);

            // Sanity check both element indexes
            VerifyIndex(c4First, CID_LINE);
            VerifyIndex(c4Second, CID_LINE);

            // If they are the same, then nothing to do
            if (c4First == c4Second)
                return;

            // Ok, we can switch them puppies
            TElem objTmp(m_paobjList[c4First]);

            m_paobjList[c4First] = m_paobjList[c4Second];
            m_paobjList[c4Second] = objTmp;

            // Bump the serial number
            m_c4SerialNum++;
        }


        TIndex tElemCount() const
        {
            // Lock this object
            tCIDLib::TCard4 c4Ret;
            {
                TLocker lockrThis(this);
                c4Ret = m_c4ElemCount;
            }
            return TIndex(c4Ret);
        }


        const TElem& objAt(const TIndex tIndex) const
        {
            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);

            // Lock this object and get the reference out
            TElem* pobjRet = nullptr;
            {
                TLocker lockrThis(this);
                VerifyIndex(c4Index, CID_LINE);
                pobjRet = &m_paobjList[c4Index];
            }
            return *pobjRet;
        }

        TElem& objAt(const TIndex tIndex)
        {
            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);

            // Lock this object and get the reference out
            TElem* pobjRet = nullptr;
            {
                TLocker lockrThis(this);
                VerifyIndex(c4Index, CID_LINE);
                pobjRet = &m_paobjList[c4Index];
            }
            return *pobjRet;
        }

        tCIDLib::TVoid Realloc(const TIndex tNewSz)
        {
            const tCIDLib::TCard4 c4NewSz = tCIDLib::TCard4(tNewSz);

            // Lock this object
            TLocker lockrThis(this);

            // If our current size is different, the delete and reallocate
            if (m_c4ElemCount != c4NewSz)
            {
                delete [] m_paobjList;
                m_c4ElemCount = c4NewSz;
                m_paobjList = new TElem[m_c4ElemCount];
            }
        }

        tCIDLib::TVoid SetAll(const TElem& objToSet)
        {
            // Lock this object
            TLocker lockrThis(this);

            for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4ElemCount; c4Ind++)
                m_paobjList[c4Ind] = objToSet;
        }


        // This makes reasonable sense for this class
        tCIDLib::TVoid TakeElements(TMyType& objaSrc)
        {
            if (this != &objaSrc)
            {
                // Lock both
                TLocker lockrThis(this);
                TLocker lockrSource(&objaSrc);

                // Swap the content
                tCIDLib::Swap(m_c4ElemCount, objaSrc.m_c4ElemCount);
                tCIDLib::Swap(m_paobjList, objaSrc.m_paobjList);

                // And bump both serial numbers
                m_c4SerialNum++;
                objaSrc.m_c4SerialNum++;
            }
        }


        //
        //  Find something in the list. Depends on a lambda with capture to avoid having
        //  to pass in the value to look for. Not found value defaults to most likely
        //  scenario of a Card4 value with max card meaning not found.
        //
        template <typename TCompCB>
        TIndex tFind(       TCompCB     compCB
                    , const TIndex      tNotFound = TIndex(kCIDLib::c4MaxCard)
                    , const TIndex      tStartAt = TIndex(0)) const
        {
            const tCIDLib::TCard4 c4StartAt = tCIDLib::TCard4(tStartAt);

            TLocker lockrThis(this);

			//
			//	We can allow the start index to be at the item past the end. We just
			//	don't enter the loop in that case and return not found. Since we
			//	don't have a checking helper for that, we'll do it ourself up front.
			//
			if (c4StartAt == m_c4ElemCount)
				return tNotFound;

			// Otherwise, it has to be a valid index
            VerifyIndex(c4StartAt, CID_LINE);
            for (tCIDLib::TCard4 c4Ind = c4StartAt; c4Ind < m_c4ElemCount; c4Ind++)
            {
                if (compCB(m_paobjList[c4Ind]))
                    return TIndex(c4Ind);
            }
            return tNotFound;
        }


    protected   :
        // -------------------------------------------------------------------
        //  A friend so he can call reset on us
        // -------------------------------------------------------------------
        template <typename T, typename I>
        friend TBinInStream& operator>>
        (
            TBinInStream& strmIn, TObjArray<T,I>& colToStream
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------

        //
        //  Needed by our input streamer friend, so that he can reset us if the
        //  stored object is different.
        //
        tCIDLib::TVoid Reset(const  tCIDLib::TCard4     c4Count
                            , const tCIDLib::EMTStates  eMTState)
        {
            if (c4Count != m_c4ElemCount)
            {
                delete [] m_paobjList;
                m_c4ElemCount = c4Count;
                m_paobjList = new TElem[m_c4ElemCount];
            }

            if (eMTState == tCIDLib::EMTStates::Safe)
            {
                if (!m_pmtxLock)
                    m_pmtxLock = new TMutex;
            }
             else
            {
                if (!m_pmtxLock)
                {
                    delete m_pmtxLock;
                    m_pmtxLock = nullptr;
                }
            }
        }


        tCIDLib::TVoid VerifyIndex( const   tCIDLib::TCard4 c4Index
                                    , const tCIDLib::TCard4 c4Line) const
        {
            if (c4Index >= m_c4ElemCount)
            {
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , c4Line
                    , kCIDErrs::errcGen_IndexError
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::BadParms
                    , TCardinal(c4Index)
                    , clsIsA()
                    , TCardinal(m_c4ElemCount)
                );
            }
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4ElemCount
        //      The number of elements the array has.
        //
        //  m_c4SerialNum
        //      This is the array serial number. It is bumped up each time
        //      that something happens that would invalid a cursor of this
        //      array. For this class, that only happens when the array is
        //      reallocated.
        //
        //  m_paobjList
        //      This is a pointer to the the array of objects. It is
        //      allocated in the constructor to be as big as the user wants.
        //
        //  m_pmtxLock
        //      This is the optional mutex that allows this array to
        //      be lockable. The derived class indicates to our constructor
        //      whether this collection should be mutlti-thread safe.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4ElemCount;
        tCIDLib::TCard4     m_c4SerialNum;
        TElem*              m_paobjList;
        TMutex*             m_pmtxLock;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        DefPolyDup(TMyType)
};


#pragma CIDLIB_POPPACK


//
//  Unlike the usual way that classes support binary streaming, i.e. through
//  the MStreamable mixin interface, we cannot do that here because that would
//  require that all our element types are streamable. So we provide a global
//  operator for those folks who want to use it. This means that collections
//  cannot be streamed polymorphically via the base classes.
//
template <typename T, typename I>
TBinOutStream& operator<<(TBinOutStream& strmOut, const TObjArray<T, I>& colToStream)
{
    // Don't let it change during this
    TLocker lockrThis(&colToStream);

    //
    //  Stream out a leading stream marker, then the element count, and the
    //  thread safety setting.
    //
    strmOut     <<  tCIDLib::EStreamMarkers::StartObject
                <<  tCIDLib::TCard4(colToStream.tElemCount())
                <<  colToStream.eMTSafe();

    // If there were any elements, then stream them
    const tCIDLib::TCard4 c4Count = tCIDLib::c4EnumOrd(colToStream.tElemCount());
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        strmOut << colToStream[I(c4Index)];

    // And do an end marker after all elements
    strmOut << tCIDLib::EStreamMarkers::EndObject;
    return strmOut;
}


// We cannot lock the collection for this, since we might delete the mutex!
template <typename T, typename I>
TBinInStream& operator>>(TBinInStream& strmIn, TObjArray<T,I>& colToStream)
{
    // Make sure we see the stream marker
    strmIn.CheckForMarker(tCIDLib::EStreamMarkers::StartObject, CID_FILE, CID_LINE);

    // Stream in the count and MT state
    tCIDLib::TCard4     c4Count;
    tCIDLib::EMTStates  eMTSafe;
    strmIn  >> c4Count >> eMTSafe;

    // We are a friend so we can call the reset method
    colToStream.Reset(c4Count, eMTSafe);
    if (c4Count)
    {
        T objTmp;

        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            strmIn >> objTmp;
            colToStream[I(c4Index)] = objTmp;
        }
    }

    strmIn.CheckForMarker(tCIDLib::EStreamMarkers::EndObject, CID_FILE, CID_LINE);
    return strmIn;
}


