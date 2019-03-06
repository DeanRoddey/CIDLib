//
// FILE NAME: CIDZLib_Tree.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/18/2004
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements internal Huffman tree implementation.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDZLib_.hpp"



// ---------------------------------------------------------------------------
//  Local methods
// ---------------------------------------------------------------------------

//
//  An inline to flip the bits of a 16 bit value.
//
static inline tCIDLib::TCard2
c2RevBits(tCIDLib::TCard2 c2Val, tCIDLib::TCard4 c4Bits)
{
    tCIDLib::TCard2 c2RetVal = 0;
    do
    {
        c2RetVal |= c2Val & 1;
        c2Val >>= 1;
        c2RetVal <<= 1;
    }   while (--c4Bits > 0);
    return (c2RetVal >> 1);
}



// ---------------------------------------------------------------------------
//  CLASS: TZTree
// PREFIX: tree
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TZTree: Constructors and Destuctor
// ---------------------------------------------------------------------------
TZTree::TZTree( const   tCIDLib::TCard4             c4Size
                , const tCIDZLib_::TTreeVals* const ptvInit) :

    m_ac2D1(new tCIDLib::TCard2[c4Size])
    , m_ac2D2(new tCIDLib::TCard2[c4Size])
    , m_c4Size(c4Size)
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Size; c4Index++)
    {
        m_ac2D1[c4Index] = ptvInit[c4Index].c2Val1;
        m_ac2D2[c4Index] = ptvInit[c4Index].c2Val2;
    }
}


// ---------------------------------------------------------------------------
//  TZTree: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TZTree::c4Build(const   TStaticZTreeDescr&  stdStatic
                ,       tCIDLib::TCard4&    c4OptLen
                ,       tCIDLib::TCard4&    c4StaticLen)
{
    tCIDLib::TCard1 ac1Depth[kCIDZLib_::c4HeapSz];
    const TZTree*   ptreeStatic = stdStatic.ptreeData();
    tCIDLib::TCard4 c4Elems = stdStatic.c4MaxElems();

    tCIDLib::TCard4 ac4Heap[kCIDZLib_::c4HeapSz];
    tCIDLib::TCard4 c4HeapCnt = 0;

    //
    //  Construct the heap with all the non-zero frequencies. Note that
    //  the 0th element of the heap is not used.
    //
    tCIDLib::TCard4 c4Largest = kCIDLib::c4MaxCard;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Elems; c4Index++)
    {
        if (m_ac2D1[c4Index])
        {
            c4Largest = c4Index;
            ac4Heap[++c4HeapCnt] = c4Index;
            ac1Depth[c4Index] = 0;
        }
         else
        {
            m_ac2D2[c4Index] = 0;
        }
    }

    //
    //  The pkzip format requires that at least one distance code exists,
    //  and that at least one bit should be sent even if there is only one
    //  possible code. So to avoid special checks later on we force at least
    //  two codes of non zero frequency.
    //
    tCIDLib::TCard4 c4Elem;
    while (c4HeapCnt < 2)
    {
        if (c4Largest < 2)
        {
            c4Largest++;
            c4Elem = c4Largest;
        }
         else if (c4Largest == kCIDLib::c4MaxCard)
        {
            c4Largest = 0;
            c4Elem = 0;
        }
         else
        {
            c4Elem = 0;
        }
        ac4Heap[++c4HeapCnt] = c4Elem;
        m_ac2D1[c4Elem] = 1;
        ac1Depth[c4Elem] = 0;

        c4OptLen--;
        if (ptreeStatic)
            c4StaticLen -= ptreeStatic->c2Len(c4Elem);

        // Node is 0 or 1 so it does not have extra bits
    }

    //
    //  Establish sub-heaps of increasing lengths. We start in the center
    //  and work down. Since the children of node X are X*2 and X*2+1, this
    //  means we'll hit them all this way.
    //
    for (tCIDLib::TCard4 c4Index = c4HeapCnt / 2; c4Index >= 1; c4Index--)
        ProcessHeap(c4Index, ac4Heap, ac1Depth, c4HeapCnt);

    //
    //  Do the Huffman algorithm by pushing down the two of the smallest
    //  frequencies until we've got a single root node.
    //
    c4Elem = c4Elems;
    tCIDLib::TCard4 c4MaxHeap = kCIDZLib_::c4HeapSz;
    do
    {
        //
        //  Pull out the smallest frequency element (we keep them sorted, and
        //  the 0th element isn't used, so it's always the 1st element. Stick
        //  the last element into it's place and reprocess the heap to get it
        //  back into correct form.
        //
        const tCIDLib::TCard4 c4Smallest = ac4Heap[1];
        ac4Heap[1] = ac4Heap[c4HeapCnt--];
        ProcessHeap(1, ac4Heap, ac1Depth, c4HeapCnt);

        //
        //  And now the next to smallest is in the 1st. This gets us the
        //  two smallest frequency elements. We'll overwrite this one with
        //  the new node we create.
        //
        const tCIDLib::TCard4 c4Next = ac4Heap[1];

        //
        //  Put them in at the end of the heap, and create a new node in the
        //  tree with the sum of their frequencies.
        //
        ac4Heap[--c4MaxHeap] = c4Smallest;
        ac4Heap[--c4MaxHeap] = c4Next;
        m_ac2D1[c4Elem] = m_ac2D1[c4Smallest] + m_ac2D1[c4Next];

        ac1Depth[c4Elem] = (ac1Depth[c4Smallest] >= ac1Depth[c4Next]
                            ? ac1Depth[c4Smallest] : ac1Depth[c4Next]) + 1;
        m_ac2D2[c4Smallest] = m_ac2D2[c4Next] = tCIDLib::TCard2(c4Elem);

        ac4Heap[1] = c4Elem++;
        ProcessHeap(1, ac4Heap, ac1Depth, c4HeapCnt);

    }   while (c4HeapCnt >= 2);

    ac4Heap[--c4MaxHeap] = ac4Heap[1];

    //
    //  Generate the bit lengths. Put the result into the caller's output
    //  parm.
    //
    tCIDLib::TCard4 ac4BitCounts[kCIDZLib_::c4MaxBits + 1];
    GenBitLengths
    (
        stdStatic
        , c4OptLen
        , c4StaticLen
        , ac4Heap
        , c4MaxHeap
        , ac4BitCounts
        , tCIDLib::c4ArrayElems(ac4BitCounts)
        , c4Largest
    );

    // And generate the codes now that the lengths are in place
    GenCodes(ac4BitCounts, tCIDLib::c4ArrayElems(ac4BitCounts), c4Largest);
    return c4Largest;
}


tCIDLib::TCard4 TZTree::c4Size() const
{
    return m_c4Size;
}


tCIDLib::TVoid TZTree::Reset()
{
    TRawMem::SetMemBuf(m_ac2D1, tCIDLib::TCard2(0), m_c4Size);
    TRawMem::SetMemBuf(m_ac2D2, tCIDLib::TCard2(0), m_c4Size);
}



// ---------------------------------------------------------------------------
//  TZTree: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TZTree::CheckIndex(const tCIDLib::TCard4 c4ToCheck) const
{
    if (c4ToCheck >= m_c4Size)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4ToCheck)
            , TString(L"TZTree")
            , TCardinal(m_c4Size)
        );
    }
}


//
//  This guy computes the optimal and static bit lengths for the tree, which
//  tells us which tree to use to compress with.
//
tCIDLib::TVoid
TZTree::GenBitLengths(  const   TStaticZTreeDescr&      stdStatic
                        ,       tCIDLib::TCard4&        c4OptLen
                        ,       tCIDLib::TCard4&        c4StaticLen
                        , const tCIDLib::TCard4* const  ac4Heap
                        , const tCIDLib::TCard4         c4HeapCnt
                        ,       tCIDLib::TCard4* const  ac4BitLens
                        , const tCIDLib::TCard4         c4LenCount
                        , const tCIDLib::TCard4         c4Largest)
{
    const tCIDLib::TCard4 c4ExtraBase = stdStatic.c4ExtraBase();
    const tCIDLib::TCard4 c4MaxBitLen = stdStatic.c4MaxBitLen();
    const TZTree* const ptreeStatic = stdStatic.ptreeData();

    // Init the bit length accumulators
    for (tCIDLib::TCard4 c4Bits = 0; c4Bits < c4LenCount; c4Bits++)
        ac4BitLens[c4Bits] = 0;

    m_ac2D2[ac4Heap[c4HeapCnt]] = 0;

    //
    //  Compute the optimal bit lengths first, i.e. the lengths using this
    //  tree. There can be overflows, in which case we'll adjust afterwards.
    //
    tCIDLib::TCard4 c4Overflow = 0;
    tCIDLib::TCard4 c4HeapInd = c4HeapCnt + 1;
    for (; c4HeapInd < kCIDZLib_::c4HeapSz; c4HeapInd++)
    {
        const tCIDLib::TCard4 c4Cur = ac4Heap[c4HeapInd];

        tCIDLib::TCard4 c4Bits = m_ac2D2[m_ac2D2[c4Cur]] + 1;
        if (c4Bits > c4MaxBitLen)
        {
            c4Bits = c4MaxBitLen;
            c4Overflow++;
        }

        // This overwrites the dad value, which is not used from here out
        m_ac2D2[c4Cur] = tCIDLib::TCard2(c4Bits);

        //
        //  Largest might not have gotten set, in which case it's max card
        //  and we want to treat that as though it is a negative number so
        //  it would come out less than c4Cur.
        //
        if ((c4Cur > c4Largest) || (c4Largest == kCIDLib::c4MaxCard))
            continue;

        ac4BitLens[c4Bits]++;
        tCIDLib::TCard4 c4XBits = 0;

        if (c4Cur >= c4ExtraBase)
            c4XBits = stdStatic.c1ExtraBitsAt(c4Cur - c4ExtraBase);

        const tCIDLib::TCard4 c4Freq = m_ac2D1[c4Cur];
        c4OptLen += c4Freq * (c4Bits+ c4XBits);
        if (ptreeStatic)
            c4StaticLen += c4Freq * (ptreeStatic->c2Len(c4Cur) + c4XBits);
    }

    // If any overflow, then do fixups
    if (c4Overflow)
    {
        do
        {
            // Find the first big length that could increase
            tCIDLib::TCard4 c4Bits = c4MaxBitLen - 1;
            while (ac4BitLens[c4Bits] == 0)
                c4Bits--;

            ac4BitLens[c4Bits]--;
            ac4BitLens[c4Bits + 1] += 2;
            ac4BitLens[c4MaxBitLen]--;

            c4Overflow -= 2;
        }   while (c4Overflow > 0);

        // And recompute all the lengths
        for (tCIDLib::TCard4 c4Bits = c4MaxBitLen; c4Bits != 0; c4Bits--)
        {
            tCIDLib::TCard4 c4Cur = ac4BitLens[c4Bits];
            while (c4Cur != 0)
            {
                const tCIDLib::TCard4 c4CurHeap = ac4Heap[--c4HeapInd];

                // As above, if largest wasn't set, treat it as smaller
                if ((c4CurHeap > c4Largest) || (c4Largest == kCIDLib::c4MaxCard))
                    continue;
                if (m_ac2D2[c4CurHeap] != tCIDLib::TCard2(c4Bits))
                {
                    c4OptLen += (c4Bits - m_ac2D2[c4CurHeap])
                                * m_ac2D1[c4CurHeap];
                    m_ac2D2[c4CurHeap] = tCIDLib::TCard2(c4Bits);
                }
                c4Cur--;
            }
        }
    }
}


//
//  Given the bit lengths, sets the code fields for the non-zero length
//  codes.
//
tCIDLib::TVoid
TZTree::GenCodes(const  tCIDLib::TCard4* const  ac4BitLens
                , const tCIDLib::TCard4         c4LenCount
                , const tCIDLib::TCard4         c4Largest)
{
    tCIDLib::TCard2 ac2NextCode[kCIDZLib_::c4MaxBits + 1];
    tCIDLib::TCard2 c2Code = 0;

    for (tCIDLib::TCard4 c4Bits = 1; c4Bits <= kCIDZLib_::c4MaxBits; c4Bits++)
    {
        c2Code = tCIDLib::TCard2((c2Code + ac4BitLens[c4Bits - 1]) << 1);
        ac2NextCode[c4Bits] = c2Code;
    }

    //
    //  Largest may have never gotten set, so we have to check for that. If
    //  not, we act like it's a negative number where the loop would never
    //  be executed.
    //
    if (c4Largest != kCIDLib::c4MaxCard)
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index <= c4Largest; c4Index++)
        {
            const tCIDLib::TCard4 c4Len = m_ac2D2[c4Index];
            if (!c4Len)
                continue;
            m_ac2D1[c4Index] = c2RevBits(ac2NextCode[c4Len]++, c4Len);
        }
    }
}


//
//  Gets the heap into the required format, starting at the c4Start node,
//  so that each father has a lower frequency than it's two children.
//
tCIDLib::TVoid
TZTree::ProcessHeap(const   tCIDLib::TCard4         c4Start
                    ,       tCIDLib::TCard4* const  ac4Heap
                    , const tCIDLib::TCard1* const  ac1Depth
                    , const tCIDLib::TCard4         c4HeapCnt)
{
    const tCIDLib::TCard4 c4Par = ac4Heap[c4Start];
    tCIDLib::TCard4 c4Ind = c4Start;

    tCIDLib::TCard4 c4Left = c4Start << 1;
    while (c4Left <= c4HeapCnt)
    {
        //
        //  Set left to the smallest of the two sons. Since the children
        //  are X*2 and X*2+1, by incrementing it, we move to the right
        //  child.
        //
        tCIDLib::TCard4 c41 = ac4Heap[c4Left + 1];
        tCIDLib::TCard4 c42 = ac4Heap[c4Left];
        if ((c4Left < c4HeapCnt)
        &&  ((m_ac2D1[c41] < m_ac2D1[c42])
        ||   ((m_ac2D1[c41] == m_ac2D1[c42])
        &&    (ac1Depth[c41] <= ac1Depth[c42]))))
        {
            c4Left++;
        }

        // Exit if par is smaller than both sons
        c42 = ac4Heap[c4Left];
        if ((m_ac2D1[c4Par] < m_ac2D1[c42])
        ||  ((m_ac2D1[c4Par] == m_ac2D1[c42])
        &&   (ac1Depth[c4Par] <= ac1Depth[c42])))
        {
            break;
        }

        // Exchange parent with the smallest son
        ac4Heap[c4Ind] = ac4Heap[c4Left];
        c4Ind = c4Left;

        // And continue down the tree, setting par to the left son of start
        c4Left <<= 1;
    }
    ac4Heap[c4Ind] = c4Par;
}



// ---------------------------------------------------------------------------
//  CLASS: TStaticZTreeDescr
// PREFIX: std
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStaticZTreeDescr: Constructors and Destructor
// ---------------------------------------------------------------------------
TStaticZTreeDescr::
TStaticZTreeDescr(  const   tCIDLib::TCard4             c4MaxBitLen
                    , const tCIDLib::TCard4             c4ExtraBase
                    , const tCIDLib::TCard1* const      pc1ExtraBits
                    , const tCIDLib::TCard4             c4ExtraBits
                    , const tCIDZLib_::TTreeVals* const ptvInit
                    , const tCIDLib::TCard4             c4InitCount
                    , const tCIDLib::TCard4             c4MaxElems) :

    m_c4ExtraBits(c4ExtraBits)
    , m_c4ExtraBase(c4ExtraBase)
    , m_c4MaxBitLen(c4MaxBitLen)
    , m_c4MaxElems(c4MaxElems)
    , m_pc1ExtraBits(nullptr)
    , m_ptreeData(new TZTree(c4InitCount, ptvInit))
{
    if (m_c4ExtraBits)
    {
        m_pc1ExtraBits = new tCIDLib::TCard1[m_c4ExtraBits];
        TRawMem::CopyMemBuf(m_pc1ExtraBits, pc1ExtraBits, m_c4ExtraBits);
    }
}

TStaticZTreeDescr::
TStaticZTreeDescr(  const   tCIDLib::TCard4         c4MaxBitLen
                    , const tCIDLib::TCard4         c4ExtraBase
                    , const tCIDLib::TCard1* const  pc1ExtraBits
                    , const tCIDLib::TCard4         c4ExtraBits
                    , const tCIDLib::TCard4         c4MaxElems) :

    m_c4ExtraBits(c4ExtraBits)
    , m_c4ExtraBase(c4ExtraBase)
    , m_c4MaxBitLen(c4MaxBitLen)
    , m_c4MaxElems(c4MaxElems)
    , m_pc1ExtraBits(nullptr)
    , m_ptreeData(nullptr)
{
    if (m_c4ExtraBits)
    {
        m_pc1ExtraBits = new tCIDLib::TCard1[m_c4ExtraBits];
        TRawMem::CopyMemBuf(m_pc1ExtraBits, pc1ExtraBits, m_c4ExtraBits);
    }
}

TStaticZTreeDescr::~TStaticZTreeDescr()
{
    try
    {
        delete m_ptreeData;
        delete [] m_pc1ExtraBits;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }
}


// ---------------------------------------------------------------------------
//  TStaticZTreeDescr: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard1
TStaticZTreeDescr::c1ExtraBitsAt(const tCIDLib::TCard4 c4At) const
{
    #if CID_DEBUG_ON
    if (!m_pc1ExtraBits || (c4At >= m_c4ExtraBits))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4At)
            , TString(L"TStaticZTreeDescr")
            , TCardinal(m_c4ExtraBits)
        );
    }
    #endif
    return m_pc1ExtraBits[c4At];
}




// ---------------------------------------------------------------------------
//  CLASS: TZTreeDescr
// PREFIX: td
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TZTreeDescr: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TZTreeDescr::Build( tCIDLib::TCard4&    c4OptLen
                    , tCIDLib::TCard4&  c4StaticLen)
{
    //
    //  Tell the tree to build it's representation. It returns to us the
    //  maximium code len, and the lengths for the optimum tree and the
    //  static tree.
    //
    m_c4MaxCode = m_treeData.c4Build(*m_pstdStatic, c4OptLen, c4StaticLen);
}

