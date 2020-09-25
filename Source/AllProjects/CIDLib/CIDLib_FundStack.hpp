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

template <typename T> class TFundStack;

template <typename T> TBinInStream&
operator>>(TBinInStream& strmToReadFrom, TFundStack<T>& colToStream);

template <typename T> TBinOutStream&
operator<<(TBinOutStream& strmToWriteTo, const TFundStack<T>& colToStream);

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

        ~TFundStack()
        {
            delete [] m_ptElements;
            m_ptElements = nullptr;
        }


        // --------------------------------------------------------------------
        //  Public operators
        // --------------------------------------------------------------------
        TFundStack<T>& operator=(TFundStack<T>&& fcolSrc) = delete;

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

        ~TFundStackJan()
        {
            if (m_pfcolStack)
                m_pfcolStack->Pop();
        }

        tCIDLib::TVoid Orphan()
        {
            m_pfcolStack = 0;
        }

        T tValue() const
        {
            return m_pfcolStack->tPeek();
        }

    private :
        TFundStackJan(const TFundStackJan&);
        tCIDLib::TVoid operator=(const TFundStackJan&);
        TFundStack<T>* m_pfcolStack;
};

#pragma CIDLIB_POPPACK

