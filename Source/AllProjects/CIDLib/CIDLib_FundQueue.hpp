//
// FILE NAME: CIDLib_FundQueue.hpp
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
//  This header implements a template based queue collection for fundamental
//  data types, as apposed to the versions for objects.
//
//  ONLY use it for fundamental types, not objects or structures.
//
// CAVEATS/GOTCHAS:
//
//  1)  In order to have X number of elements, we have to allocate X+1 actual
//      array slots. This is because head=tail means its empty. If you put
//      X elements in, then the tail will again equal the head, which would
//      make it look empty. So we have to always have a spare one. So we
//      set m_c4MaxElements to the requested count plus one, and just adjust
//      it when the c4MaxElements() method is called.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// -----------------------------------------------------------------------------
//   CLASS: TFundQueue
//  PREFIX: fcol
// -----------------------------------------------------------------------------
template <class T> class TFundQueue : public TFundColBase, public MDuplicable
{
    public :
        // --------------------------------------------------------------------
        //  Constructors and Destructor
        // --------------------------------------------------------------------
        TFundQueue(const tCIDLib::TCard4 c4MaxElements) :

            m_c4MaxElements(c4MaxElements + 1)
            , m_c4Head(0)
            , m_c4Tail(0)
            , m_ptElements(nullptr)
        {
            //
            //  Allocate the buffer. We really allocate one extra element
            //  so that we can have an extra one to separate the head and
            //  tail when the queue is full.
            //
            m_ptElements = new T[m_c4MaxElements];
        }

        TFundQueue(const TFundQueue<T>& fcolSrc) :

            m_c4MaxElements(fcolSrc.m_c4MaxElements)
            , m_c4Head(fcolSrc.m_c4Head)
            , m_c4Tail(fcolSrc.m_c4Tail)
            , m_ptElements(0)
        {
            m_ptElements = new T[m_c4MaxElements];
            TRawMem::CopyMemBuf
            (
                m_ptElements
                , fcolSrc.m_ptElements
                , m_c4MaxElements * sizeof(T)
            );
        }

        ~TFundQueue()
        {
            delete [] m_ptElements;
            m_ptElements = nullptr;
        }


        // --------------------------------------------------------------------
        //  Public operators
        // --------------------------------------------------------------------
        TFundQueue<T>& operator=(const TFundQueue<T>& fcolSrc)
        {
            if (this == &fcolSrc)
                return *this;

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
                m_ptElements
                , fcolSrc.m_ptElements
                , m_c4MaxElements * sizeof(T)
            );

            return *this;
        }

        tCIDLib::TBoolean operator==(const TFundQueue<T>& fcolSrc)
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

        tCIDLib::TBoolean operator!=(const TFundQueue<T>& fcolSrc)
        {
            return !operator==(fcolSrc);
        }


        // --------------------------------------------------------------------
        //  Public, inherited methods
        // --------------------------------------------------------------------
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

        tCIDLib::TVoid RemoveAll()
        {
            // Point the head and tail back at the zero'th position
            m_c4Head = 0;
            m_c4Tail = 0;
        }


        // --------------------------------------------------------------------
        //  Public, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid Add(const T tToAdd)
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

        // Add a new element if not already in the list
        tCIDLib::TBoolean bAddIfNew(const T tToAdd)
        {
            if (bIsFull())
                ColIsFull(CID_FILE, CID_LINE, m_c4MaxElements);

            // See if this value already exists
            tCIDLib::TCard4 c4CurInd = m_c4Head;
            while (c4CurInd != m_c4Tail)
            {
                if (m_ptElements[c4CurInd] == tToAdd)
                    return kCIDLib::False;

                // Bump the index and wrap around if required
                c4CurInd++;
                if (c4CurInd == m_c4MaxElements)
                    c4CurInd = 0;
            }

            // It's new, so stick the new one where the tail is pointing now
            m_ptElements[m_c4Tail] = tToAdd;

            // And bump up the tail, handing wraparound
            m_c4Tail++;
            if (m_c4Tail == m_c4MaxElements)
                m_c4Tail = 0;

            return kCIDLib::True;
        }

        tCIDLib::TBoolean bIsFull() const
        {
            //
            //  If the tail is at the element before the head, then the
            //  queue is full (taking wraparound into account.)
            //
            if (m_c4Tail == m_c4MaxElements - 1)
            {
                //
                //  The tail is at the end, so if the head is at the start,
                //  then the tail is next and the queue is full.
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


        T tPeek() const
        {
            if (m_c4Head == m_c4Tail)
                ColIsEmpty(CID_FILE, CID_LINE);

            // Return the element at the head
            return m_ptElements[m_c4Head];
        }

        T tGet()
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


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend TBinOutStream& operator<< <T>
        (
                    TBinOutStream&          strmOut
            , const TFundQueue<T>&          colToStream
        );

        friend TBinInStream& operator>> <T>
        (
                    TBinInStream&           strmIn
            ,       TFundQueue<T>&          colToStream
        );


    private :
        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_ptElements
        //      The array of elements that make up the storage for the queue.
        //
        //  m_c4MaxElements
        //      The maximum number of elements in the queue. This is actually
        //      one more than the client asked for, see the Caveats section.
        //
        //  m_c4Head
        //  m_c4Tail
        //      These are used to turn the element array into a circular
        //      queue that we can efficiently add and remove from without
        //      ever needing any movement of existing elements. The head
        //      chases the tail around.
        // --------------------------------------------------------------------
        T*              m_ptElements;
        tCIDLib::TCard4 m_c4MaxElements;
        tCIDLib::TCard4 m_c4Head;
        tCIDLib::TCard4 m_c4Tail;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TFundQueue<T>,TFundColBase)
        DefPolyDup(TFundQueue<T>)
};

#pragma CIDLIB_POPPACK


template <class T> TBinInStream&
operator>>(TBinInStream& strmToReadFrom, TFundQueue<T>& colToStream)
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
    if (colToStream.m_c4MaxElements != c4NewMax)
    {
        delete [] colToStream.m_ptElements;
        colToStream.m_ptElements = 0;
        colToStream.m_ptElements = new T[c4NewMax];
        colToStream.m_c4MaxElements = c4NewMax;
    }

    // Set the head to zero and the tail to the new count
    colToStream.m_c4Head = 0;
    colToStream.m_c4Tail = c4NewCount;

    // And read in the data
    strmToReadFrom.ReadArray(colToStream.m_ptElements, c4NewCount);

    return strmToReadFrom;
}


template <class T> TBinOutStream&
operator<<(TBinOutStream& strmToWriteTo, const TFundQueue<T>& colToStream)
{
    // Store a stream marker for safety
    strmToWriteTo << tCIDLib::EStreamMarkers::StartObject;

    // Store the maximum element size
    strmToWriteTo << colToStream.m_c4MaxElements;

    // Write out the current number of elements
    const tCIDLib::TCard4 c4CurCount = colToStream.c4ElemCount();
    strmToWriteTo << c4CurCount;

    //
    //  NOTE: Because of the wraparound, we cannot do a bulk write
    //  here, even if the elements are byte sized.
    //
    tCIDLib::TCard4 c4CurInd = colToStream.m_c4Head;
    while (c4CurInd != colToStream.m_c4Tail)
    {
        // Write out the current element
        strmToWriteTo << colToStream.m_ptElements[c4CurInd];

        // Bump the index and wrap around if required
        c4CurInd++;
        if (c4CurInd == colToStream.m_c4MaxElements)
            c4CurInd = 0;
    }

    return strmToWriteTo;
}


