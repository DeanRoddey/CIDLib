//
// FILE NAME: CIDLib_FundDeque.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/30/1998
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
//  This header implements a template based deque collection for fundamental
//  data types, as apposed to the versions for objects.
//
//  ONLY use this class for fundamental types, not objects or structures.
//
//  The layout is an array with head and tail indexes to keep up with where
//  we are at any moment. Adding at the head will write where the head index
//  points, then bump it down, so it always points where the next write is.
//  The same for the tail, except it bumps up.
//
//  We use a circular buffer, so we have to have a max size, unlike most other
//  collection types.
//
//  In order to be able to tell the difference between a full deque and an
//  empty one, we allocate an extra element. When either the head or tail
//  hits this element the deque is full and the other one cannot come into
//  there.
//
//  c4MaxElements() returns (m_c4MaxElements-1) to make up for this fact.
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

// -----------------------------------------------------------------------------
//   CLASS: TFundDeque
//  PREFIX: fcol
// -----------------------------------------------------------------------------
template <class T> class TFundDeque : public TFundColBase, public MDuplicable
{
    public :
        // --------------------------------------------------------------------
        //  Constructors and Destructor
        // --------------------------------------------------------------------
        TFundDeque(const tCIDLib::TCard4 c4MaxElements) :

            m_c4MaxElements(c4MaxElements + 1)
            , m_c4Head(0)
            , m_c4Tail(0)
            , m_ptElements(nullptr)
        {
            //
            //  Allocate the buffer. We really allocate one extra element
            //  so that we can have an extra one to separate the head and
            //  tail when the deque is full.
            //
            m_ptElements = new T[m_c4MaxElements];
        }

        TFundDeque(const TFundDeque<T>& fcolSrc) :

            m_c4MaxElements(fcolSrc.m_c4MaxElements)
            , m_c4Head(fcolSrc.m_c4Head)
            , m_c4Tail(fcolSrc.m_c4Tail)
            , m_ptElements(nullptr)
        {
            m_ptElements = new T[m_c4MaxElements];
            TRawMem::CopyMemBuf
            (
                m_ptElements
                , fcolSrc.m_ptElements
                , m_c4MaxElements * sizeof(T)
            );
        }

        ~TFundDeque()
        {
            delete [] m_ptElements;
            m_ptElements = nullptr;
        }


        // --------------------------------------------------------------------
        //  Public operators
        // --------------------------------------------------------------------
        TFundDeque<T>& operator=(const TFundDeque<T>& fcolSrc)
        {
            if (this != &fcolSrc)
            {
                if (m_c4MaxElements != fcolSrc.m_c4MaxElements)
                {
                    delete [] m_ptElements;
                    m_ptElements = new T[fcolSrc.m_c4MaxElements];
                    m_c4MaxElements = fcolSrc.m_c4MaxElements;
                }

                m_c4Head = fcolSrc.m_c4Head;
                m_c4Tail = fcolSrc.m_c4Tail;
                TRawMem::CopyMemBuf
                (
                    m_ptElements, fcolSrc.m_ptElements, m_c4MaxElements * sizeof(T)
                );
            }
            return *this;
        }

        tCIDLib::TBoolean operator==(const TFundDeque<T>& fcolSrc)
        {
            if (this == &fcolSrc)
                return kCIDLib::True;

            //
            //  Check the obvious stuff first. Note that we don't compare
            //  the head/tail because these are implementation methods.
            //
            if ((m_c4MaxElements != fcolSrc.m_c4MaxElements)
            ||  (c4ElemCount() != fcolSrc.c4ElemCount()))
            {
                return kCIDLib::False;
            }

            //
            //  Oh well, lets check the actual content. They both have
            //  their own head/tail positions so we have to deal with them
            //  separately.
            //
            tCIDLib::TCard4 c4ThisInd = m_c4Head;
            tCIDLib::TCard4 c4SrcInd = fcolSrc.m_c4Head;
            const tCIDLib::TCard4 c4SrcMax = fcolSrc.m_c4MaxElements;
            const T* const ptSrc = fcolSrc.m_ptElements;

            while (c4ThisInd != m_c4Tail)
            {
                if (m_ptElements[c4ThisInd++] != ptSrc[c4SrcInd++])
                    return kCIDLib::False;

                // Wrap both indexes around if needed
                if (c4ThisInd == m_c4MaxElements)
                    c4ThisInd = 0;

                if (c4SrcInd == c4SrcMax)
                    c4SrcInd = 0;
            }

            return kCIDLib::True;
        }

        tCIDLib::TBoolean operator!=(const TFundDeque<T>& fcolSrc)
        {
            return !operator==(fcolSrc);
        }



        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsEmpty() const override
        {
            return (m_c4Head == m_c4Tail);
        }

        tCIDLib::TCard4 c4ElemCount() const override
        {
            if (m_c4Head == m_c4Tail)
                return 0;

            if (m_c4Head < m_c4Tail)
            {
                // We are not wrapped around right now
                return m_c4Tail - m_c4Head;
            }
             else
            {
                // We are wrapped around
                return (m_c4MaxElements - m_c4Head) + m_c4Tail;
            }
        }

        tCIDLib::TVoid RemoveAll() override
        {
            // Point the head and tail back at the zero'th position
            m_c4Head = 0;
            m_c4Tail = 0;
        }


        // --------------------------------------------------------------------
        //  Public, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid AddAtBack(const T tToAdd)
        {
            if (bIsFull())
                ColIsFull(CID_FILE, CID_LINE, m_c4MaxElements);

            // Stick the new element where the tail is pointing now
            m_ptElements[m_c4Tail] = tToAdd;

            // And bump up the tail, handing wraparound
            m_c4Tail++;
            if (m_c4Tail == m_c4MaxElements)
                m_c4Tail = 0;
        }

        tCIDLib::TVoid AddAtFront(const T tToAdd)
        {
            if (bIsFull())
                ColIsFull(CID_FILE, CID_LINE, m_c4MaxElements);

            //
            //  We have to move the head backwards to make room for the
            //  new element, accounting for wraparound.
            //
            if (!m_c4Head)
                m_c4Head = m_c4MaxElements - 1;
            else
                m_c4Head--;

            // And put the new element into the new head
            m_ptElements[m_c4Head] = tToAdd;
        }

        tCIDLib::TBoolean bAddAtBackIfNotAlready(const T tToAdd)
        {
            //
            //  See if this element is already in the deque. If so, then
            //  just return with a false.
            //
            if (bElementPresent(tToAdd))
                return kCIDLib::False;

            // Its not there, so add it and return true
            AddAtBack(tToAdd);
            return kCIDLib::True;
        }

        tCIDLib::TBoolean bAddAtFrontIfNotAlready(const T tToAdd)
        {
            //
            //  See if this element is already in the deque. If so, then
            //  just return with a false.
            //
            if (bElementPresent(tToAdd))
                return kCIDLib::False;

            // Its not there, so add it and return true
            AddAtFront(tToAdd);
            return kCIDLib::True;
        }

        tCIDLib::TBoolean bRemoveIfPresent(const T tToRemove)
        {
            //
            //  Go from the head to the tail, handling wrap, and looking
            //  for the indicated item.
            //
            tCIDLib::TCard4 c4RemInd = m_c4Head;
            while (c4RemInd != m_c4Tail)
            {
                if (m_ptElements[c4RemInd] == tToRemove)
                {
                    //
                    //  This is our guy. So move all the items from the next
                    //  index up back one to fill in the hole. Note that the
                    //  tail points BEYOND the last item, so we stop when
                    //  we move the index up and the next slot after that
                    //  would be the tail. This is complicated by the fact
                    //  that we have to deal with wrapping.
                    //
                    while (kCIDLib::True)
                    {
                        // See if we are at the end yet
                        tCIDLib::TCard4 c4ProbeInd = c4RemInd;
                        c4ProbeInd++;
                        if (c4ProbeInd == m_c4MaxElements)
                            c4ProbeInd = 0;

                        if (c4ProbeInd == m_c4Tail)
                            break;

                        // We aren't so copy down another element
                        if (c4RemInd + 1 == m_c4MaxElements)
                        {
                            //
                            //  We are going to wrap at the end of the array,
                            //  so we need to pull our source from the 0th
                            //  slot.
                            //
                            m_ptElements[c4RemInd] = m_ptElements[0];
                            c4RemInd = 0;
                        }
                         else
                        {
                            m_ptElements[c4RemInd] = m_ptElements[c4RemInd + 1];
                            c4RemInd++;
                        }
                    }

                    // Move the tail back now and return success
                    if (!m_c4Tail)
                        m_c4Tail = m_c4MaxElements - 1;
                    else
                        m_c4Tail--;
                    return kCIDLib::True;
                }

                // Not yet, move up and wrap if needed
                c4RemInd++;
                if (c4RemInd == m_c4MaxElements)
                    c4RemInd = 0;
            }
            return kCIDLib::False;
        }

        tCIDLib::TBoolean bElementPresent(const T tToTest) const
        {
            //
            //  Go from the head to the element before the tail (since
            //  the tail points to the next element to add!), handling
            //  wrap around.
            //
            tCIDLib::TCard4 c4CurInd = m_c4Head;
            while (c4CurInd != m_c4Tail)
            {
                if (m_ptElements[c4CurInd] == tToTest)
                    return kCIDLib::True;

                c4CurInd++;
                if (c4CurInd == m_c4MaxElements)
                    c4CurInd = 0;
            }
            return kCIDLib::False;
        }

        tCIDLib::TBoolean bIsFull() const
        {
            //
            //  If the tail is at the element before the head, then the
            //  deque is full (taking wraparound into account.)
            //
            if (m_c4Tail == m_c4MaxElements - 1)
            {
                //
                //  The tail is at the end, so if the head is at the start,
                //  then the tail is next and the deque is full.
                //
                if (m_c4Head == 0)
                    return kCIDLib::True;
            }
             else if (m_c4Tail + 1 == m_c4Head)
            {
                return kCIDLib::True;
            }
            return kCIDLib::False;
        }

        tCIDLib::TCard4 c4MaxElemCount() const
        {
            // Adjust for the fact that we allocate an extra one
            return m_c4MaxElements - 1;
        }

        T tGetFromBack()
        {
            if (m_c4Head == m_c4Tail)
                ColIsEmpty(CID_FILE, CID_LINE);

            // And bump down the head, handling wraparound
            if (!m_c4Tail)
                m_c4Tail = m_c4MaxElements - 1;
            else
                m_c4Tail--;

            //
            //  Return the element at the new position. Note that the
            //  tail always points past the last element so after returning
            //  this element, we are again 'past' the last element.
            //
            return m_ptElements[m_c4Tail];
        }

        T tGetFromFront()
        {
            if (m_c4Head == m_c4Tail)
                ColIsEmpty(CID_FILE, CID_LINE);

            // Save the element at the head
            const T tRet = m_ptElements[m_c4Head];

            // And bump up the head, handling wraparound
            m_c4Head++;
            if (m_c4Head == m_c4MaxElements)
                m_c4Head = 0;

            return tRet;
        }

        T tPeekAtBack()
        {
            if (m_c4Head == m_c4Tail)
                ColIsEmpty(CID_FILE, CID_LINE);

            // Calculate where the previous tail element was placed
            tCIDLib::TCard4 c4Pos = m_c4Tail;
            if (!c4Pos)
                c4Pos = m_c4MaxElements - 1;
            else
                c4Pos--;

            //
            //  Return the element at the new position. Note that the
            //  tail always points past the last element so after returning
            //  this element, we are again 'past' the last element.
            //
            return m_ptElements[c4Pos];
        }

        T tPeekAtFront()
        {
            if (m_c4Head == m_c4Tail)
                ColIsEmpty(CID_FILE, CID_LINE);

            // Return the element at the head
            return m_ptElements[m_c4Head];
        }


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend TBinOutStream& operator<< <T>
        (
                    TBinOutStream&          strmOut
            , const TFundDeque<T>&          fcolToStream
        );

        friend TBinInStream& operator>> <T>
        (
                    TBinInStream&           strmIn
            ,       TFundDeque<T>&          fcolToStream
        );


    private :
        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_ptElements
        //      The array of elements that make up the storage for the deque.
        //
        //  m_c4MaxElements
        //      The maximum number of elements in the deque, i.e. the size
        //      of m_ptElements.
        //
        //  m_c4Head
        //  m_c4Tail
        //      These are used to turn the element array into a circular
        //      deque that we can efficiently add and remove from without
        //      ever needing any movement of existing elements. The head
        //      chases the tail around. There is an extra element so that
        //      the tail can point to an empty element when the deque is
        //      full.
        // --------------------------------------------------------------------
        T*              m_ptElements;
        tCIDLib::TCard4 m_c4MaxElements;
        tCIDLib::TCard4 m_c4Head;
        tCIDLib::TCard4 m_c4Tail;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TFundDeque<T>,TFundColBase)
        DefPolyDup(TFundDeque<T>)
};

#pragma CIDLIB_POPPACK


template <class T> TBinInStream&
operator>>(TBinInStream& strmToReadFrom, TFundDeque<T>& fcolToStream)
{
    // First we should get a stream marker
    strmToReadFrom.CheckForMarker
    (
        tCIDLib::EStreamMarkers::StartObject
        , CID_FILE
        , CID_LINE
    );

    // Get out the new max and count
    tCIDLib::TCard4 c4NewCount;
    tCIDLib::TCard4 c4NewMax;
    strmToReadFrom >> c4NewMax >> c4NewCount;

    // Validate these
    if ((c4NewCount > c4NewMax) || !c4NewMax)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFData_BadExtData
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    //
    //  If the current max size and the new max size are not the
    //  same, then reallocate.
    //
    if (fcolToStream.m_c4MaxElements != c4NewMax)
    {
        delete [] fcolToStream.m_ptElements;
        fcolToStream.m_ptElements = 0;
        fcolToStream.m_ptElements = new T[c4NewMax];
        fcolToStream.m_c4MaxElements = c4NewMax;
    }

    // Set the head to zero and the tail to the new count
    fcolToStream.m_c4Head = 0;
    fcolToStream.m_c4Tail = c4NewCount;

    // And load up the data
    strmToReadFrom.ReadArray(fcolToStream.m_ptElements, c4NewCount);
    return strmToReadFrom;
}


template <class T> TBinOutStream&
operator<<(TBinOutStream& strmToWriteTo, const TFundDeque<T>& fcolToStream)
{
    // Store a stream marker for safety, then the max and current sizes
    strmToWriteTo << tCIDLib::EStreamMarkers::StartObject
                  << fcolToStream.m_c4MaxElements
                  << fcolToStream.c4ElemCount();

    //
    //  NOTE: Because of the wraparound, we cannot do a bulk write
    //  here, even if the elements are byte sized.
    //
    tCIDLib::TCard4 c4CurInd = fcolToStream.m_c4Head;
    while (c4CurInd != fcolToStream.m_c4Tail)
    {
        // Write out the current element
        strmToWriteTo << fcolToStream.m_ptElements[c4CurInd];

        // Bump the index and wrap around if required
        c4CurInd++;
        if (c4CurInd == fcolToStream.m_c4MaxElements)
            c4CurInd = 0;
    }
    return strmToWriteTo;
}

