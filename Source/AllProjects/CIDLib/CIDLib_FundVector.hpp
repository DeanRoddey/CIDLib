//
// FILE NAME: CIDLib_FundVector.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/25/1999
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
//  This header defines a template class that is used to create vectors of
//  fundamental data types. This is a pretty rare need since CIDLib provides
//  an environment that discourages such things, but its used some internally
//  and might be needed outside in some rare circumstances.
//
//  There are a full set of these fundamental collections, providing arrays,
//  queues. stacks, deques, etc...
//
//  ONLY use it for fundamental types, not objects or structures.
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
//   CLASS: TFundVector
//  PREFIX: fcol
// ---------------------------------------------------------------------------
template <typename TElem, typename TIndex = tCIDLib::TCard4>
class TFundVector : public TFundColBase, public MDuplicable
{
    public :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TMyType = TFundVector<TElem, TIndex>;
        using TMyElemType = TElem;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFundVector() :

            m_c4AllocSize(32)
            , m_c4CurIndex(0)
            , m_ptElements(nullptr)
        {
            //
            //  Allocate the buffer. No need to initialize. We won't look at any
            //  that are not actually used.
            //
            m_ptElements = new TElem[m_c4AllocSize];
        }

        TFundVector(const TIndex tInitSize) :

            m_c4AllocSize(tCIDLib::c4EnumOrd(tInitSize))
            , m_c4CurIndex(0)
            , m_ptElements(nullptr)
        {
            // Force the init size to a default if its zero
            if (!m_c4AllocSize)
                m_c4AllocSize = 32;

            //
            //  Allocate the buffer. No need to initialize. We won't look at any
            //  that are not actually used.
            //
            m_ptElements = new TElem[m_c4AllocSize];
        }

        TFundVector(const TMyType& fcolSrc) :

            m_c4AllocSize(fcolSrc.m_c4AllocSize)
            , m_c4CurIndex(fcolSrc.m_c4CurIndex)
            , m_ptElements(nullptr)
        {
            m_ptElements = new TElem[m_c4AllocSize];
            TRawMem::CopyMemBuf
            (
                m_ptElements
                , fcolSrc.m_ptElements
                , m_c4CurIndex * sizeof(TElem)
            );
        }

        TFundVector(TMyType&& fcolSrc) :

            m_c4AllocSize(1)
            , m_c4CurIndex(0)
            , m_ptElements(new TElem[1])
        {
            *this = tCIDLib::ForceMove(fcolSrc);
        }

        ~TFundVector()
        {
            try
            {
                delete [] m_ptElements;
                m_ptElements = nullptr;
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
        TMyType& operator=(const TMyType& fcolSrc)
        {
            if (this == &fcolSrc)
                return *this;

            // Publish first a clear before we whack the current stuff
            this->PublishClear();

            // Reallocate if we aren't big enough
            if (m_c4AllocSize < fcolSrc.m_c4CurIndex)
            {
                delete [] m_ptElements;
                m_c4AllocSize = fcolSrc.m_c4CurIndex + 32;
                m_ptElements = new TElem[m_c4AllocSize];
            }

            m_c4CurIndex = fcolSrc.m_c4CurIndex;
            TRawMem::CopyMemBuf
            (
                m_ptElements
                , fcolSrc.m_ptElements
                , m_c4CurIndex * sizeof(TElem)
            );

            this->PublishBlockAdded(0, m_c4CurIndex);
            return *this;
        }

        TMyType& operator=(TMyType&& fcolSrc)
        {
            if (&fcolSrc != this)
            {
                this->PublishClear();

                tCIDLib::Swap(m_c4AllocSize, fcolSrc.m_c4AllocSize);
                tCIDLib::Swap(m_c4CurIndex, fcolSrc.m_c4CurIndex);
                tCIDLib::Swap(m_ptElements, fcolSrc.m_ptElements);

                this->PublishBlockAdded(0, m_c4CurIndex);
            }
            return *this;
        }

        TElem operator[](const TIndex tIndex) const
        {
            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);
            CheckIndex(c4Index, m_c4CurIndex, CID_FILE, CID_LINE);
            return m_ptElements[c4Index];
        }

        TElem& operator[](const TIndex tIndex)
        {
            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);
            CheckIndex(c4Index, m_c4CurIndex, CID_FILE, CID_LINE);
            return m_ptElements[c4Index];
        }

        tCIDLib::TBoolean operator==(const TMyType& fcolToTest) const
        {
            if (this == &fcolToTest)
                return kCIDLib::True;

            //
            //  NOTE: The current allocation size is an implementation detail
            //  and not counted in equality, since it will grow as needed.
            //
            if (m_c4CurIndex != fcolToTest.m_c4CurIndex)
                return kCIDLib::False;

            // Check the active elements for equality
            for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4CurIndex; c4Ind++)
            {
                if (m_ptElements[c4Ind] != fcolToTest.m_ptElements[c4Ind])
                    return kCIDLib::False;
            }
            return kCIDLib::True;
        }

        tCIDLib::TBoolean operator!=(const TMyType& fcolToTest) const
        {
            return !operator==(fcolToTest);
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsEmpty() const
        {
            return (m_c4CurIndex == 0);
        }

        tCIDLib::TCard4 c4ElemCount() const override
        {
            return m_c4CurIndex;
        }

        tCIDLib::TVoid RemoveAll() override
        {
            m_c4CurIndex = 0;
            this->PublishClear();
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddElements( const   TElem           tToAdd
                                    , const tCIDLib::TCard4 c4Count)
        {
            // Check for zero count
            if (!c4Count)
                return;

            // Check for a needed expansion to hold these elements
            CheckExpand(c4Count, kCIDLib::True);

            //
            //  We can't do a set here, because the type could be a floating point or
            //  something, and we have no type sensitive buffer set for those types.
            //  So this is just safer.
            //
            const tCIDLib::TCard4 c4OrgCount = m_c4CurIndex;
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                m_ptElements[m_c4CurIndex++] = tToAdd;

            this->PublishBlockAdded(c4OrgCount, c4Count);
        }

        tCIDLib::TVoid Append(  const   TMyType&        fcolToAppend
                                , const tCIDLib::TCard4 c4Count = 0)
        {
            //
            //  If they indicated a specific count, take that. Else take the number of
            //  elements in the source vector.
            //
            tCIDLib::TCard4 c4SrcCount = c4Count;
            if (!c4SrcCount)
                c4SrcCount = fcolToAppend.c4ElemCount();

            // If none, we are done
            if (!c4SrcCount)
                return;

            // See if we have to expand, keeping the old content
            CheckExpand(c4SrcCount, kCIDLib::True);

            // Copy the new elements onto the end
            const tCIDLib::TCard4 c4OrgCount = m_c4CurIndex;
            TRawMem::CopyMemBuf
            (
                &m_ptElements[m_c4CurIndex]
                , fcolToAppend.m_ptElements
                , sizeof(TElem) * c4SrcCount
            );

            // Bump the end index and publish the change if enabled
            m_c4CurIndex += c4SrcCount;
            this->PublishBlockAdded(c4OrgCount, c4Count);
        }

        tCIDLib::TBoolean
        bCheckCapacity( const   tCIDLib::TCard4     c4ToCheck
                        , const tCIDLib::TBoolean   bKeepOld = kCIDLib::True)
        {
            if (c4ToCheck > m_c4CurIndex)
            {
                // Expand by amount we can't handle
                CheckExpand(c4ToCheck - m_c4CurIndex, bKeepOld);
                return kCIDLib::True;
            }

            // We didn't have to expand
            return kCIDLib::False;
        }

        tCIDLib::TBoolean bDeleteIfPresent(const TElem tToDel)
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurIndex; c4Index++)
            {
                if (m_ptElements[c4Index] == tToDel)
                {
                    Delete(c4Index);
                    return kCIDLib::True;
                }
            }
            return kCIDLib::False;
        }

        tCIDLib::TBoolean bElementPresent(const TElem tToTest, TIndex& tAt) const
        {
            tCIDLib::TCard4 c4Index = 0;
            for (; c4Index < m_c4CurIndex; c4Index++)
            {
                if (m_ptElements[c4Index] == tToTest)
                {
                    tAt = TIndex(c4Index);
                    return kCIDLib::True;
                }
            }
            return kCIDLib::False;
        }

        tCIDLib::TBoolean bElementPresent(const TElem tToTest) const
        {
            // Call the other version and eat the returned index output
            TIndex tAt;
            return bElementPresent(tToTest, tAt);
        }

        tCIDLib::TCard4 c4AddElement(const TElem tToAdd)
        {
            // Check for a needed expansion to hold one more element
            CheckExpand(1, kCIDLib::True);

            // Add the new guy in and bump the index
            const tCIDLib::TCard4 c4NewIndex = m_c4CurIndex++;
            m_ptElements[c4NewIndex] = tToAdd;

            this->PublishAdd(c4NewIndex);
            return c4NewIndex;
        }

        tCIDLib::TVoid Delete(const TIndex tAt)
        {
            // Make sure its a legal index
            const tCIDLib::TCard4 c4At = tCIDLib::TCard4(tAt);
            CheckIndex(c4At, m_c4CurIndex, CID_FILE, CID_LINE);

            // Bump down the end index to reduce the count by 1
            m_c4CurIndex--;

            //
            //  If not deleting the last one, then we have to move the remaining ones
            //  down to fill that gap.
            //
            if (c4At < m_c4CurIndex)
            {
                // Move down everything to cover this one
                tCIDLib::TCard4 c4Index = c4At;
                for (; c4Index < m_c4CurIndex; c4Index++)
                    m_ptElements[c4Index] = m_ptElements[c4Index + 1];
            }

            this->PublishRemove(c4At);
        }

        // To is inclusive, so if from==to we remove one element
        tCIDLib::TVoid Delete(  const   TIndex  tFrom
                                , const TIndex  tTo)
        {
            // Make sure both indexes are legal
            const tCIDLib::TCard4 c4From = tCIDLib::TCard4(tFrom);
            const tCIDLib::TCard4 c4To = tCIDLib::TCard4(tTo);

            CheckIndex(c4From, m_c4CurIndex, CID_FILE, CID_LINE);
            CheckIndex(c4To, m_c4CurIndex, CID_FILE, CID_LINE);

            // If they are the same, do a single element remove
            if (c4From == c4To)
            {
                Delete(c4From);
                return;
            }

            // It's a range so flip them if out of order
            tCIDLib::TCard4 c4RealFrom  = c4From;
            tCIDLib::TCard4 c4RealTo    = c4To;
            if (c4To < c4From)
                tCIDLib::Swap(c4RealFrom, c4RealTo);

            //
            //  Special case it if To is the last one, so we can just cap at From and
            //  not have to move any data.
            //
            if (c4RealTo == m_c4CurIndex - 1)
            {
                this->PublishBlockRemoved(c4RealFrom, (c4RealTo - c4RealFrom) + 1);
                m_c4CurIndex = c4RealFrom;
                return;
            }

            //
            //  Oh well, we have to move down the trailing elements to fill
            //  in the emptied section.
            //
            tCIDLib::TCard4 c4TargetInd = c4RealFrom;
            tCIDLib::TCard4 c4SrcInd = c4To + 1;
            while (c4SrcInd < m_c4CurIndex)
                m_ptElements[c4TargetInd++] = m_ptElements[c4SrcInd++];

            // Adjust the current end index by the number removed
            m_c4CurIndex -= (c4RealTo - c4RealFrom) + 1;

            this->PublishBlockRemoved(c4RealFrom, (c4RealTo - c4RealFrom) + 1);
        }

        // Enable publish/subscribe on this collection
        tCIDLib::TVoid EnablePubSub(const TString& strDesiredPath, TString& strActualPath)
        {
            // Just pass to our parent
            this->EnablePublish(strDesiredPath, strActualPath);
        }

        tCIDLib::TVoid EnablePubSub(const TString& strTopicPath)
        {
            // Just pass to our parent
            this->EnablePublish(strTopicPath);
        }


        // Call back for each elements
        template <typename IterCB> tCIDLib::TVoid ForEachI(IterCB iterCB) const
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurIndex; c4Index++)
            {
                if (!iterCB(m_ptElements[c4Index], TIndex(c4Index)))
                    break;
            }
        }

        template <typename IterCB> tCIDLib::TVoid ForEach(IterCB iterCB) const
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurIndex; c4Index++)
            {
                if (!iterCB(m_ptElements[c4Index]))
                    break;
            }
        }

        template <typename IterCB> tCIDLib::TVoid ForEachNC(IterCB iterCB)
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurIndex; c4Index++)
            {
                if (!iterCB(m_ptElements[c4Index]))
                    break;
            }
        }




        tCIDLib::TVoid Insert(const TElem tToInsert, const TIndex tAt)
        {
            // Special case an insert at the end
            const tCIDLib::TCard4 c4At = tCIDLib::TCard4(tAt);

            if (c4At == m_c4CurIndex)
            {
                c4AddElement(tToInsert);
                return;
            }

            // Check for a needed expansion to hold one more element
            CheckExpand(1, kCIDLib::True);

            //
            //  We have to move the trailing elements up now, to make room
            //  for the new guy.
            //
            tCIDLib::TCard4 c4Index = m_c4CurIndex;
            while (c4Index > c4At)
            {
                m_ptElements[c4Index] = m_ptElements[c4Index - 1];
                c4Index--;
            }

            // Put the new one in and bump up the end index
            m_ptElements[c4At] = tToInsert;
            m_c4CurIndex++;

            this->PublishAdd(c4At);
        }

        tCIDLib::TVoid Insert(  const   TMyType&    fcolToInsert
                                , const TIndex      tAt)
        {
            // Get a copy of the count of source elements
            const tCIDLib::TCard4 c4At = tCIDLib::TCard4(tAt);
            const tCIDLib::TCard4 c4SrcCount = fcolToInsert.c4ElemCount();

            // If none, we are done
            if (!c4SrcCount)
                return;

            //
            //  Special case an insert at the end to an append, in which case at being
            //  past the last item is legal.
            //
            if (c4At == m_c4CurIndex)
            {
                Append(fcolToInsert);
                return;
            }

            // Make sure its a legal index
            CheckIndex(c4At, m_c4CurIndex, CID_FILE, CID_LINE);

            // See if we have to expand
            CheckExpand(c4SrcCount, kCIDLib::True);

            //
            //  Ok, lets move the elements that are going to pushed up. We
            //  copy them up to the end.
            //
            tCIDLib::TCard4 c4TargetInd = c4At + c4SrcCount;
            tCIDLib::TCard4 c4SrcInd    = c4At;
            while (c4SrcInd < m_c4CurIndex + c4SrcCount)
                m_ptElements[c4TargetInd++] = m_ptElements[c4SrcInd++];

            // Copy the new elements in at the insert point
            TRawMem::CopyMemBuf
            (
                &m_ptElements[c4At]
                , fcolToInsert.m_ptElements
                , sizeof(TElem) * c4SrcCount
            );

            // And bump the end index
            m_c4CurIndex += c4SrcCount;

            this->PublishBlockAdded(c4At, c4SrcCount);
        }

        const TElem* ptElements() const
        {
            return m_ptElements;
        }

        tCIDLib::TVoid SetAll(const TElem tVal)
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurIndex; c4Index++)
                m_ptElements[c4Index] = tVal;

            this->PublishBlockChanged(0, m_c4CurIndex);
        }

        tCIDLib::TVoid SetAt(const TIndex tAt, const TElem tVal)
        {
            // Make sure its a legal index, and if so, copy in the value
            const tCIDLib::TCard4 c4At = tCIDLib::TCard4(tAt);
            CheckIndex(c4At, m_c4CurIndex, CID_FILE, CID_LINE);
            m_ptElements[c4At] = tVal;

            this->PublishElemChanged(c4At);
        }

        tCIDLib::TVoid SetFrom(const TElem* const ptSrc, const tCIDLib::TCard4 c4Count)
        {
            //
            //  Just expand if required, throwing any old content, and copy
            //  the source data into our buffer, and update the end index.
            //
            CheckExpand(c4Count - m_c4CurIndex, kCIDLib::False);
            TRawMem::CopyMemBuf(m_ptElements, ptSrc, sizeof(TElem) * c4Count);
            m_c4CurIndex = c4Count;

            this->PublishBlockChanged(0, c4Count);
        }

        template <typename TCompFunc> tCIDLib::TVoid Sort(TCompFunc pfnComp)
        {
            // If 1 or 0 elements, nothing to do
            if (m_c4CurIndex < 2)
                return;

            TArrayOps::TSort<TElem>(m_ptElements, m_c4CurIndex, pfnComp);
            this->PublishReorder();
        }

        TElem tAt(const TIndex tIndex) const
        {
            const tCIDLib::TCard4 c4Index = tCIDLib::TCard4(tIndex);
            CheckIndex(c4Index, m_c4CurIndex, CID_FILE, CID_LINE);
            return m_ptElements[c4Index];
        }

        TElem tLast() const
        {
            if (!m_c4CurIndex)
                ColIsEmpty(CID_FILE, CID_LINE);
            return m_ptElements[m_c4CurIndex - 1];
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

			//
			//	We can allow the start index to be at the item past the end. We just
			//	don't enter the loop in that case and return not found. Since we
			//	don't have a checking helper for that, we'll do it ourself up front.
			//
			if (c4StartAt == m_c4CurIndex)
				return tNotFound;

			// Otherwise, it has to be a valid index
            CheckIndex(c4StartAt, m_c4CurIndex, CID_FILE, CID_LINE);
            for (tCIDLib::TCard4 c4Index = c4StartAt; c4Index < m_c4CurIndex; c4Index++)
            {
                if (compCB(m_ptElements[c4Index]))
                    return TIndex(c4Index);
            }
            return tNotFound;
        }


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend TBinOutStream& operator<< <TElem, TIndex>
        (
                    TBinOutStream&          strmOut
            , const TMyType&                colToStream
        );

        friend TBinInStream& operator>> <TElem, TIndex>
        (
                    TBinInStream&           strmIn
            ,       TMyType&                colToStream
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckExpand( const   tCIDLib::TCard4     c4NewNeeded
                                    , const tCIDLib::TBoolean   bKeepOld)
        {
            if (m_c4CurIndex + c4NewNeeded >= m_c4AllocSize)
            {
                // Calc the new size and alloc the new array
                const tCIDLib::TCard4 c4NewSize = tCIDLib::TCard4
                (
                    (m_c4AllocSize + c4NewNeeded) * 1.5
                );
                TElem* ptNewArray = new TElem[c4NewSize];

                // If told to, copy over the old stuff, and then delete it
                if (bKeepOld)
                {
                    TRawMem::CopyMemBuf
                    (
                        ptNewArray, m_ptElements, sizeof(TElem) * m_c4AllocSize
                    );
                }
                delete [] m_ptElements;

                // And store the new info
                m_ptElements = ptNewArray;
                m_c4AllocSize = c4NewSize;
            }
        }


        // -------------------------------------------------------------------
        //  Private data
        //
        //  m_c4AllocSize
        //      This is the current allocation size. When m_c4CurIndex equals
        //      this size, its time to reallocate.
        //
        //      NOTE:   This is an implementation detail and is not part of
        //              vector equality.
        //
        //  m_c4CurIndex
        //      The current index, i.e. where the next element is going to
        //      be placed (and also the current count of elements in the
        //      vector.)
        //
        //  m_ptElements
        //      This is the allocated array of m_c4AllocSize values.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4AllocSize;
        tCIDLib::TCard4     m_c4CurIndex;
        TElem*              m_ptElements;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TMyType,TFundColBase)
        DefPolyDup(TMyType)
};

#pragma CIDLIB_POPPACK


template <typename TElem, typename TIndex> TBinInStream&
operator>>(TBinInStream& strmToReadFrom, TFundVector<TElem, TIndex>& colToStream)
{
    // First we should get a stream marker
    strmToReadFrom.CheckForMarker
    (
        tCIDLib::EStreamMarkers::StartObject, CID_FILE, CID_LINE
    );

    // Stream in the stored element count
    strmToReadFrom >> colToStream.m_c4CurIndex;

    //
    //  If the new count is bigger than our current allocation size,
    //  we have to reallocate. We don't have to retain old content
    //  here.
    //
    if (colToStream.m_c4CurIndex > colToStream.m_c4AllocSize)
    {
        colToStream.m_c4AllocSize = colToStream.m_c4CurIndex + 32;
        delete [] colToStream.m_ptElements;
        colToStream.m_ptElements = new TElem[colToStream.m_c4AllocSize];
    }

    // And read in the stored elements
    TBinInStream_ReadArray(strmToReadFrom, colToStream.m_ptElements, colToStream.m_c4CurIndex);
    return strmToReadFrom;
}

template <typename TElem, typename TIndex> TBinOutStream&
operator<<(TBinOutStream& strmToWriteTo, const TFundVector<TElem, TIndex>& colToStream)
{
    // Store a stream marker for safety
    strmToWriteTo << tCIDLib::EStreamMarkers::StartObject;

    // Stream out the active element count and then the elements
    strmToWriteTo << colToStream.m_c4CurIndex;
    TBinOutStream_WriteArray(strmToWriteTo, colToStream.m_ptElements, colToStream.m_c4CurIndex);
    return strmToWriteTo;
}

