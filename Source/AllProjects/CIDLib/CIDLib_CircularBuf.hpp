//
// FILE NAME: CIDLib_CircularBuf.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/11/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the circular buffer class. Though not a formal
//  collection class, this is sort of a combination of the efficiency and indexed
//  access of a vector with the LIFO aspects of a deque. It is appropriate when
//  fast and efficient management of a cicular buffer, of up to some maximum number,
//  is required and where direct indexing is necesary.
//
//  A vector is not efficient for LIFO purposes, since it has to shift the whole
//  list to throw out the oldest one. And a deque is quite heavy weight and does
//  not support indexed access.
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
//   CLASS: TCircularBuf
//  PREFIX: cbuf
// ---------------------------------------------------------------------------
template <class TElem> class TCircularBuf
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructors
        // -------------------------------------------------------------------
        TCircularBuf(const tCIDLib::TCard4 c4MaxElements) :

            m_c4MaxElements(c4MaxElements + 1)
            , m_c4Head(0)
            , m_c4Tail(0)
            , m_ptElements(nullptr)
        {
            if (m_c4MaxElements < 4)
                m_c4MaxElements = 5;
            else if (m_c4MaxElements > 512)
                m_c4MaxElements = 513;

            //
            //  Allocate the buffer. We really allocate one extra element
            //  so that we can have an extra one to separate the head and
            //  tail when the queue is full.
            //
            m_ptElements = new TElem[m_c4MaxElements];
        }

        TCircularBuf(const TCircularBuf<TElem>& cbufSrc) :

            m_c4MaxElements(cbufSrc.m_c4MaxElements)
            , m_c4Head(cbufSrc.m_c4Head)
            , m_c4Tail(cbufSrc.m_c4Tail)
            , m_ptElements(nullptr)
        {
            m_ptElements = new TElem[m_c4MaxElements];
            TRawMem::CopyMemBuf
            (
                m_ptElements, cbufSrc.m_ptElements, m_c4MaxElements * sizeof(TElem)
            );
        }

        ~TCircularBuf()
        {
            delete [] m_ptElements;
            m_ptElements = nullptr;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCircularBuf<TElem>& operator=(const TCircularBuf<TElem>& cbufSrc)
        {
            if (this != &cbufSrc)
            {
                if (m_c4MaxElements != cbufSrc.m_c4MaxElements)
                {
                    delete [] m_ptElements;
                    m_ptElements = nullptr;
                    m_ptElements = new TElem[cbufSrc.m_c4MaxElements];
                    m_c4MaxElements = cbufSrc.m_c4MaxElements;
                }

                m_c4Head = cbufSrc.m_c4Head;
                m_c4Tail = cbufSrc.m_c4Tail;
                TRawMem::CopyMemBuf
                (
                    m_ptElements, cbufSrc.m_ptElements, m_c4MaxElements * sizeof(TElem)
                );
            }
            return *this;
        }

        const TElem& operator[](const tCIDLib::TCard4 c4Index) const
        {
            return tAt(c4Index);
        }

        TElem& operator[](const tCIDLib::TCard4 c4Index)
        {
            return tAt(c4Index);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bAdd(const TElem tToAdd)
        {
            //
            //  If full, then throw out the oldest one. We return whether we threw
            //  out an old one or not.
            //
            tCIDLib::TBoolean bRet = bCheckFull();

            // Stick the new element where the tail is pointing now
            m_ptElements[m_c4Tail] = tToAdd;

            // And bump up the tail, handing wraparound
            m_c4Tail++;
            if (m_c4Tail == m_c4MaxElements)
                m_c4Tail = 0;

            return bRet;
        }

        tCIDLib::TBoolean bIsEmpty() const
        {
            return (m_c4Head == m_c4Tail);
        }

        tCIDLib::TBoolean bIsFull()
        {
            //
            //  Check the special case of the tail at the end and the head
            //  wrapped around. Else if the head is at tail+1 it's full.
            //
            if ((m_c4Tail == m_c4MaxElements - 1) && (m_c4Head == 0))
                return kCIDLib::True;
            else if (m_c4Tail + 1 == m_c4Head)
                return kCIDLib::True;

            return kCIDLib::False;
        }

        tCIDLib::TCard4 c4ElemCount() const
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

        constexpr tCIDLib::TCard4 c4MaxElemCount() const
        {
            // Adjust for the fact that we allocate an extra one
            return m_c4MaxElements - 1;
        }

        const TElem& tAt(const tCIDLib::TCard4 c4At) const
        {
            return m_ptElements[(m_c4Head + c4At) % m_c4MaxElements];
        }

        TElem& tAt(const tCIDLib::TCard4 c4At)
        {
            return m_ptElements[(m_c4Head + c4At) % m_c4MaxElements];
        }

        tCIDLib::TVoid RemoveAll()
        {
            // Point the head and tail back at the zero'th position
            m_c4Head = 0;
            m_c4Tail = 0;
        }

        tCIDLib::TVoid SetSize(const tCIDLib::TCard4 c4New)
        {

        }


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------

        // If full, we throw out the oldest one
        tCIDLib::TBoolean bCheckFull()
        {
            if (m_c4Tail == m_c4MaxElements - 1)
            {
                //
                //  The tail is at the end, so if the head is at the start,
                //  the queue is full. In this case, we want to move head up
                //  to discard the oldest one.
                //
                if (m_c4Head == 0)
                {
                    m_c4Head++;
                    return kCIDLib::True;
                }
            }
             else if (m_c4Tail + 1 == m_c4Head)
            {
                // It's full so we need to move head up, wrapping it if needed.
                m_c4Head++;
                if (m_c4Head == m_c4MaxElements)
                    m_c4Head = 0;
                return kCIDLib::True;
            }

            // Weren't full, so nothing discarded
            return kCIDLib::False;
        }


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4MaxElements
        //  m_c4MaxElements
        //      The maximum number of elements in the queue. This is actually
        //      one more than the client asked for, see m_ptElements below.
        //
        //  m_c4Head
        //  m_c4Tail
        //      The first and last element indices. Last is where the next one will
        //      go. So if head=tail means empty. Once we start putting elements in,
        //      tail always points one beyond the last one. If tail is less than
        //      head, it is wrapped around.
        //
        //  m_paElememnts
        //      The array of elements, which is allocated to the max size. NOTE THAT
        //      because of the way head/tail work, we have to allocate it one larger
        //      than the max. Since head=tail means empty, if we filled it up, that
        //      would wrap the head back around to the tail, and make it look empty.
        //      So we have to have the extra one.
        // -------------------------------------------------------------------
        TElem*          m_ptElements;
        tCIDLib::TCard4 m_c4MaxElements;
        tCIDLib::TCard4 m_c4Head;
        tCIDLib::TCard4 m_c4Tail;
};

#pragma CIDLIB_POPPACK



