//
// FILE NAME: CIDLib_FundStack.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/29/1998
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
//  This header implements a template based stack collection for fundamental
//  data types, as apposed to the versions for objects.
//
//  ONLY use it for fundamental types, not objects or structures.
//
//  We also define a simple janitor to push/pop it on a scoped basis.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// -----------------------------------------------------------------------------
//   CLASS: TFundStack
//  PREFIX: fcol
// -----------------------------------------------------------------------------
template <typename T> class TFundStack : public TFundColBase, public MDuplicable
{
    public :
        // --------------------------------------------------------------------
        //  Constructors and Destructor
        // --------------------------------------------------------------------
        TFundStack(const tCIDLib::TCard4 c4MaxElements) :

            m_c4MaxElements(c4MaxElements)
            , m_c4Top(0)
            , m_ptElements(nullptr)
        {
            // Allocate the buffer
            if (m_c4MaxElements == 0)
                m_c4MaxElements = 1;
            m_ptElements = new T[m_c4MaxElements];
        }

        TFundStack(const TFundStack<T>& fcolSrc) :

            m_c4MaxElements(fcolSrc.m_c4MaxElements)
            , m_c4Top(fcolSrc.m_c4Top)
            , m_ptElements(nullptr)
        {
            // Allocate the buffer and copy over the source contents
            m_ptElements = new T[m_c4MaxElements];
            TRawMem::CopyMemBuf(m_ptElements, fcolSrc.m_ptElements, m_c4Top * sizeof(T));
        }

        TFundStack(TFundStack<T>&& fcolSrc) :

            TFundStack(1)
        {
            *this = tCIDLib::ForceMove(fcolSrc);
        }

        ~TFundStack()
        {
            delete [] m_ptElements;
            m_ptElements = nullptr;
        }


        // --------------------------------------------------------------------
        //  Public operators
        // --------------------------------------------------------------------
        TFundStack<T>& operator=(const TFundStack<T>& fcolSrc)
        {
            if (this == &fcolSrc)
                return *this;

            // If different max sizes, then reallocate
            if (m_c4MaxElements != fcolSrc.m_c4MaxElements)
            {
                delete [] m_ptElements;
                m_ptElements = new T[m_c4MaxElements];
                m_c4MaxElements = fcolSrc.m_c4MaxElements;
            }

            // Set the new top and copy over the source elements
            m_c4Top = fcolSrc.m_c4Top;
            TRawMem::CopyMemBuf(m_ptElements, fcolSrc.m_ptElements, m_c4Top * sizeof(T));

            return *this;
        }

        TFundStack<T>& operator=(TFundStack<T>&& fcolSrc)
        {
            if (&fcolSrc != this)
            {
                TParent::operator=(tCIDLib::ForceMove(fcolSrc));
                tCIDLib::Swap(m_c4MaxElements, fcolSrc.m_c4MaxElements);
                tCIDLib::Swap(m_c4Top, fcolSrc.m_c4Top);
                tCIDLib::Swap(m_ptElements, fcolSrc.m_ptElements);
            }
            return *this;
        }

        tCIDLib::TBoolean operator==(const TFundStack<T>& fcolToCompare)
        {
            if (this == &fcolToCompare)
                return kCIDLib::True;

            // Check the obvious stuff first
            if ((m_c4Top != fcolToCompare.m_c4Top)
            ||  (m_c4MaxElements != fcolToCompare.m_c4MaxElements))
            {
                return kCIDLib::False;
            }

            // Oh well, lets check the actual content
            const T* const ptSrc = fcolToCompare.m_ptElements;
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Top; c4Index++)
            {
                if (m_ptElements[c4Index] != ptSrc[c4Index])
                    return kCIDLib::False;
            }

            return kCIDLib::True;
        }

        tCIDLib::TBoolean operator!=(const TFundStack<T>& fcolToCompare)
        {
            return !operator==(fcolToCompare);
        }


        // --------------------------------------------------------------------
        //  Public, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::TBoolean bIsEmpty() const override
        {
            return (!m_c4Top);
        }

        tCIDLib::TCard4 c4ElemCount() const override
        {
            return m_c4Top;
        }

        tCIDLib::TVoid RemoveAll() override
        {
            m_c4Top = 0;
        }


        // --------------------------------------------------------------------
        //  Public, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TBoolean bIsFull() const
        {
            return (m_c4Top == m_c4MaxElements);
        }

        tCIDLib::TCard4 c4MaxElemCount() const
        {
            return m_c4MaxElements;
        }

        const T* ptElements() const
        {
            return m_ptElements;
        }

        T tPeek() const
        {
            if (!m_c4Top)
                ColIsEmpty(CID_FILE, CID_LINE);

            // Return the element just below the top
            return m_ptElements[m_c4Top-1];
        }

        T tPop()
        {
            if (!m_c4Top)
                ColIsEmpty(CID_FILE, CID_LINE);

            // Bump down the top and return the element there
            return m_ptElements[--m_c4Top];
        }

        tCIDLib::TVoid Push(const T tToPush)
        {
            if (m_c4Top == m_c4MaxElements)
                ColIsFull(CID_FILE, CID_LINE, m_c4MaxElements);

            // Stick the new element onto the top of stack and bump the top
            m_ptElements[m_c4Top++] = tToPush;
        }


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend TBinOutStream& operator<< <T>
        (
                    TBinOutStream&          strmOut
            , const TFundStack<T>&          fcolToStream
        );

        friend TBinInStream& operator>> <T>
        (
                    TBinInStream&           strmIn
            ,       TFundStack<T>&          fcolToStream
        );


    private :
        // --------------------------------------------------------------------
        //  Private data members
        //
        //  m_ptElements
        //      The array of elements that make up the storage for the queue.
        //
        //  m_c4MaxElements
        //      The maximum number of elements in the queue.
        //
        //  m_c4CurTop
        //      The current top of the stack. It is zero if the stack is
        //      empty, so it always points to where the next element will be
        //      pushed.
        // --------------------------------------------------------------------
        T*              m_ptElements;
        tCIDLib::TCard4 m_c4MaxElements;
        tCIDLib::TCard4 m_c4Top;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TFundStack<T>,TFundColBase)
        DefPolyDup(TFundStack<T>)
};


// -----------------------------------------------------------------------------
//   CLASS: TFundStackJan
//  PREFIX: jan
// -----------------------------------------------------------------------------
template <typename T> class TFundStackJan
{
    public :
        TFundStackJan(const T tToPush, TFundStack<T>* const pfcolStack) :

            m_pfcolStack(pfcolStack)
        {
            m_pfcolStack->Push(tToPush);
        }

        TFundStackJan(const TFundStackJan&) = delete;
        TFundStackJan(TFundStackJan&&) = delete;

        ~TFundStackJan()
        {
            if (m_pfcolStack)
                m_pfcolStack->Pop();
        }

        TFundStackJan& operator=(const TFundStackJan&) = delete;
        TFundStackJan& operator=(TFundStackJan&&) = delete;

        tCIDLib::TVoid Orphan()
        {
            m_pfcolStack = nullptr;
        }

        T tValue() const
        {
            return m_pfcolStack->tPeek();
        }

    private :
        TFundStack<T>*  m_pfcolStack;
};

#pragma CIDLIB_POPPACK


template <typename T> TBinInStream&
operator>>(TBinInStream& strmToReadFrom, TFundStack<T>& colToStream)
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
        colToStream.m_ptElements = nullptr;
        colToStream.m_ptElements = new T[c4NewMax];
        colToStream.m_c4MaxElements = c4NewMax;
    }

    // Store the new top
    colToStream.m_c4Top = c4NewCount;

    // And read in the elements themselves.
    TBinInStream_ReadArray(strmToReadFrom, colToStream.m_ptElements, colToStream.m_c4Top);
    return strmToReadFrom;
}

template <typename T> TBinOutStream&
operator<<(TBinOutStream& strmToWriteTo, const TFundStack<T>& colToStream)
{
    // Stream out our start marker, max elements and current element count
    strmToWriteTo << tCIDLib::EStreamMarkers::StartObject
                  << colToStream.m_c4MaxElements
                  << colToStream.m_c4Top;

    // And write out the elements themselves
    TBinOutStream_WriteArray(strmToWriteTo, colToStream.m_ptElements, colToStream.m_c4Top);
    return strmToWriteTo;
}


