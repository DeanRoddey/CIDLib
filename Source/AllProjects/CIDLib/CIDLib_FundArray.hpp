//
// FILE NAME: CIDLib_FundArray.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/22/1995
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
//  This header defines a template class that is used to create arrays of
//  fundamental data types. This is a pretty rare need since CIDLib provides
//  an environment that discourages such things, but its used some internally
//  and might be needed outside in some rare circumstances.
//
//  There are a full set of these fundamental collections, providing queues.
//  stacks, and deques.
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
//   CLASS: TFundArray
//  PREFIX: fcol
// ---------------------------------------------------------------------------
template <typename TElem, typename TIndex = tCIDLib::TCard4>
class TFundArray : public TFundColBase, public MDuplicable
{
    public :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TMyType = TFundArray<TElem, TIndex>;
        using TMyElemType = TElem;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFundArray() :

            m_c4ElemCount(32)
            , m_ptElements(nullptr)
        {
            if (!m_c4ElemCount)
                m_c4ElemCount = 32;

            // Allocate the buffer
            m_ptElements = new TElem[m_c4ElemCount];

            // We can only zero it in this case
            TRawMem::SetMemBuf
            (
                m_ptElements, tCIDLib::TCard1(0), sizeof(TElem) * m_c4ElemCount
            );
        }

        TFundArray(const TIndex tElemCount) :

            m_c4ElemCount(tCIDLib::c4EnumOrd(tElemCount))
            , m_ptElements(nullptr)
        {
            if (!m_c4ElemCount)
                ZeroSize(CID_FILE, CID_LINE);

            // Allocate the buffer
            m_ptElements = new TElem[m_c4ElemCount];

            // We can only zero it in this case
            TRawMem::SetMemBuf
            (
                m_ptElements, tCIDLib::TCard1(0), sizeof(TElem) * m_c4ElemCount
            );
        }

        TFundArray(const TIndex tElemCount, const TElem tInit) :

            m_c4ElemCount(tCIDLib::c4EnumOrd(tElemCount))
            , m_ptElements(nullptr)
        {
            if (!m_c4ElemCount)
                ZeroSize(CID_FILE, CID_LINE);

            // Allocate the buffer
            m_ptElements = new TElem[m_c4ElemCount];

            // And init the data values
            SetAll(tInit);
        }

        TFundArray(const TMyType& fcolSrc) :

            m_c4ElemCount(fcolSrc.m_c4ElemCount)
            , m_ptElements(nullptr)
        {
            m_ptElements = new TElem[m_c4ElemCount];
            TRawMem::CopyMemBuf
            (
                m_ptElements, fcolSrc.m_ptElements, m_c4ElemCount * sizeof(TElem)
            );
        }

        TFundArray(TMyType&& fcolSrc) :

            m_c4ElemCount(1)
            , m_ptElements(new TElem[1])
        {
            TRawMem::SetMemBuf(m_ptElements, tCIDLib::TCard1(0), sizeof(TElem));
            *this = tCIDLib::ForceMove(fcolSrc);
        }

        ~TFundArray()
        {
            delete [] m_ptElements;
            m_ptElements = nullptr;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMyType& operator=(const TMyType& fcolSrc)
        {
            if (this != &fcolSrc)
            {
                if (m_c4ElemCount != fcolSrc.m_c4ElemCount)
                {
                    delete [] m_ptElements;
                    m_c4ElemCount = fcolSrc.m_c4ElemCount;
                    m_ptElements = new TElem[m_c4ElemCount];
                }

                TRawMem::CopyMemBuf
                (
                    m_ptElements, fcolSrc.m_ptElements, m_c4ElemCount * sizeof(TElem)
                );
            }
            return *this;
        }

        TMyType& operator=(TMyType&& fcolSrc)
        {
            if (this != &fcolSrc)
            {
                tCIDLib::Swap(m_c4ElemCount, fcolSrc.m_c4ElemCount);
                tCIDLib::Swap(m_ptElements, fcolSrc.m_ptElements);
            }
            return *this;
        }

        TElem operator[](const TIndex tIndex) const
        {
            CheckIndex(tCIDLib::TCard4(tIndex), m_c4ElemCount, CID_FILE, CID_LINE);
            return m_ptElements[tCIDLib::TCard4(tIndex)];
        }

        TElem& operator[](const TIndex tIndex)
        {
            CheckIndex(tCIDLib::TCard4(tIndex), m_c4ElemCount, CID_FILE, CID_LINE);
            return m_ptElements[tCIDLib::TCard4(tIndex)];
        }

        tCIDLib::TBoolean operator==(const TMyType& fcolToTest) const
        {
            if (this == &fcolToTest)
                return kCIDLib::True;

            if (m_c4ElemCount != fcolToTest.m_c4ElemCount)
                return kCIDLib::False;

            for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4ElemCount; c4Ind++)
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
        tCIDLib::TBoolean bIsEmpty() const override
        {
            // Arrays are never empty
            return kCIDLib::False;
        }

        tCIDLib::TCard4 c4ElemCount() const override
        {
            return m_c4ElemCount;
        }

        tCIDLib::TVoid RemoveAll() override
        {
            // Has no meaning for an array
            Unsupported(CID_FILE, CID_LINE, L"RemoveAll");
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean
        bExpand(const   tCIDLib::TCard4     c4NewSize
                , const TElem               tInitVal
                , const tCIDLib::TBoolean   bKeepOld = kCIDLib::True)
        {
            // If the new size larger, then reallocate
            if (c4NewSize > m_c4ElemCount)
            {
                // Allocate the new array
                TElem* ptNewArray = new TElem[c4NewSize];

                // If told to, copy over the old stuff, and then delete it.
                if (bKeepOld)
                {
                    TRawMem::CopyMemBuf
                    (
                        ptNewArray, m_ptElements, sizeof(TElem) * m_c4ElemCount
                    );
                }
                delete [] m_ptElements;

                // Initialize the new items
                for (tCIDLib::TCard4 c4Index = m_c4ElemCount; c4Index < c4NewSize; c4Index++)
                    ptNewArray[c4Index] = tInitVal;

                // And store the new array info
                m_c4ElemCount = c4NewSize;
                m_ptElements = ptNewArray;

                // Indicate we did reallocate the array
                return kCIDLib::True;
            }

            // We didn't have to reallocate
            return kCIDLib::False;
        }

        tCIDLib::TVoid Exchange(const   tCIDLib::TCard4 c4First
                                , const tCIDLib::TCard4 c4Second)
        {
            CheckIndex(c4First, m_c4ElemCount, CID_FILE, CID_LINE);
            CheckIndex(c4Second, m_c4ElemCount, CID_FILE, CID_LINE);

            TElem TTmp = m_ptElements[c4First];
            m_ptElements[c4First] = m_ptElements[c4Second];
            m_ptElements[c4Second] = TTmp;
        }


        // Call back for each element
        template <typename IterCB> tCIDLib::TBoolean bForEach(IterCB iterCB) const
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4ElemCount; c4Index++)
            {
                if (!iterCB(m_ptElements[c4Index]))
                    return kCIDLib::False;
            }
            return kCIDLib::True;
        }

        template <typename IterCB> tCIDLib::TBoolean bForEachI(IterCB iterCB) const
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4ElemCount; c4Index++)
            {
                if (!iterCB(m_ptElements[c4Index], TIndex(c4Index)))
                    return kCIDLib::False;
            }
            return kCIDLib::True;
        }

        template <typename IterCB> tCIDLib::TBoolean bForEachNC(IterCB iterCB)
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4ElemCount; c4Index++)
            {
                if (!iterCB(m_ptElements[c4Index]))
                    return kCIDLib::False;
            }
            return kCIDLib::True;
        }


        const TElem* ptElements() const
        {
            return m_ptElements;
        }

        tCIDLib::TVoid
        Reallocate( const   tCIDLib::TCard4 c4NewSize
                    , const TElem           tInitVal)
        {
            // If the new size is different, then reallocate
            if (m_c4ElemCount != c4NewSize)
            {
                delete [] m_ptElements;
                m_c4ElemCount = c4NewSize;
                m_ptElements = new TElem[m_c4ElemCount];
            }

            // And init the new values
            SetAll(tInitVal);
        }

        tCIDLib::TVoid RotateDown(const tCIDLib::TCard4 c4Count)
        {
            tCIDLib::TCard4  c4Actual = c4Count % m_c4ElemCount;

            if (!c4Actual || (m_c4ElemCount == 1))
                return;

            // Make c4Actual passes through the array
            for (tCIDLib::TCard4 c4Rotate = 0; c4Rotate < c4Actual; c4Rotate++)
            {
                TElem TSave = m_ptElements[0];

                for (tCIDLib::TCard4 c4Ind = 1; c4Ind < m_c4ElemCount; c4Ind++)
                    m_ptElements[c4Ind-1] = m_ptElements[c4Ind];

                m_ptElements[m_c4ElemCount-1] = TSave;
            }
        }

        tCIDLib::TVoid RotateUp(const tCIDLib::TCard4 c4Count)
        {
            tCIDLib::TCard4  c4Actual = c4Count % m_c4ElemCount;

            if (!c4Actual || (m_c4ElemCount == 1))
                return;

            // Make c4Actual passes through the array
            for (tCIDLib::TCard4 c4Rotate = 0; c4Rotate < c4Actual; c4Rotate++)
            {
                TElem TSave = m_ptElements[m_c4ElemCount-1];

                tCIDLib::TCard4 c4Ind;
                for (c4Ind = m_c4ElemCount-1; c4Ind >= 1; c4Ind--)
                    m_ptElements[c4Ind] = m_ptElements[c4Ind-1];

                m_ptElements[0] = TSave;
            }
        }

        tCIDLib::TVoid SetAt(const tCIDLib::TCard4 c4At, const TElem tVal)
        {
            CheckIndex(c4At, m_c4ElemCount, CID_FILE, CID_LINE);
            m_ptElements[c4At] = tVal;
        }

        tCIDLib::TVoid SetAll(const TElem tInitVal)
        {
            tCIDLib::TCard4 c4Ind;
            for (c4Ind = 0; c4Ind < m_c4ElemCount; c4Ind++)
                m_ptElements[c4Ind] = tInitVal;
        }

        template <typename TCompFunc> tCIDLib::TVoid Sort(TCompFunc pfnComp)
        {
            // If 1 or 0 elements, nothing to do
            if (m_c4ElemCount < 2)
                return;
            TArrayOps::TSort<TElem,TCompFunc>(m_ptElements, m_c4ElemCount, pfnComp);
        }

        TElem tAt(const tCIDLib::TCard4 c4At)
        {
            CheckIndex(c4At, m_c4ElemCount, CID_FILE, CID_LINE);
            return m_ptElements[c4At];
        }

        tCIDLib::TVoid ZeroAll()
        {
            TRawMem::SetMemBuf
            (
                m_ptElements
                , tCIDLib::TCard1(0)
                , m_c4ElemCount * sizeof(TElem)
            );
        }


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend TBinOutStream& operator<< <TElem, TIndex>
        (
                    TBinOutStream&          strmOut
            , const TMyType&                fcolToStream
        );

        friend TBinInStream& operator>> <TElem, TIndex>
        (
                    TBinInStream&           strmIn
            ,       TMyType&                fcolToStream
        );


    private :
        // -------------------------------------------------------------------
        //  Private data
        //
        //  m_c4ElemCount
        //      This is the count of elements in this array.
        //
        //  m_ptElements
        //      This is the allocated array of c4Count values. The count is
        //      provided in the constructor.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4ElemCount;
        TElem*              m_ptElements;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TMyType, TFundColBase)
        DefPolyDup(TMyType)
};

#pragma CIDLIB_POPPACK

template <typename TElem, typename TIndex> TBinOutStream&
operator<<(TBinOutStream& strmToWriteTo, const TFundArray<TElem, TIndex>& colToStream)
{
    //
    //  Store the element count and then the elements. We stream out
    //  a stream maker to add a little safety.
    //
    strmToWriteTo << tCIDLib::EStreamMarkers::StartObject << colToStream.m_c4ElemCount;
    TBinOutStream_WriteArray(strmToWriteTo, colToStream.m_ptElements, colToStream.m_c4ElemCount);
    return strmToWriteTo;
}

template <typename TElem, typename TIndex> TBinInStream&
operator>>(TBinInStream& strmToReadFrom, TFundArray<TElem, TIndex>& colToStream)
{
    strmToReadFrom.CheckForMarker
    (
        tCIDLib::EStreamMarkers::StartObject
        , CID_FILE
        , CID_LINE
    );

    // Pull the element count out
    tCIDLib::TCard4 c4NewCount;
    strmToReadFrom >> c4NewCount;

    if (colToStream.m_c4ElemCount != c4NewCount)
    {
        colToStream.m_c4ElemCount = c4NewCount;
        delete [] colToStream.m_ptElements;
        colToStream.m_ptElements = nullptr;
        colToStream.m_ptElements = new TElem[colToStream.m_c4ElemCount];
    }
    TBinInStream_ReadArray(strmToReadFrom, colToStream.m_ptElements, colToStream.m_c4ElemCount);
    return strmToReadFrom;
}

