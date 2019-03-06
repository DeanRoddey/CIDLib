//
// FILE NAME: CIDMath_ValueMap2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/08/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a couple of simple derivatives of the abstract
//  TValueMap class. TLogMap does a logarithmic mapping. TModuloMap does a
//  modulo division mapping.
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
#include    "CIDMath_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macros
// ---------------------------------------------------------------------------
RTTIDecls(TLogMap,TValueMap)
RTTIDecls(TModMap,TValueMap)



// ---------------------------------------------------------------------------
//  CLASS: TLogMap
// PREFIX: vmap
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TLogMap: Constructors and Destructor
// ---------------------------------------------------------------------------
TLogMap::TLogMap() :

    TValueMap()
    , m_ac1LogTable(0)
    , m_c4ElemSize(1)
    , m_eMode(tCIDLib::ELogMapModes::Spread)
{
    AllocTable();
    InitTable();
}

TLogMap::TLogMap(   const   tCIDLib::TCard4         c4MaxIndex1
                    , const tCIDLib::TCard4         c4MaxIndex2
                    , const tCIDLib::ELogMapModes   eMode) :

    TValueMap(c4MaxIndex1, c4MaxIndex2)
    , m_ac1LogTable(0)
    , m_c4ElemSize(0)
    , m_eMode(eMode)
{
    if (c4MaxIndex1 <= 0xFF)
        m_c4ElemSize = 1;
    else if (c4MaxIndex1 <= 0xFFFF)
        m_c4ElemSize = 2;
    else
        m_c4ElemSize = 4;

    AllocTable();
    InitTable();
}

TLogMap::TLogMap(const TLogMap& vmapSrc) :

    TValueMap(vmapSrc)
    , m_ac1LogTable(0)
    , m_c4ElemSize(vmapSrc.m_c4ElemSize)
    , m_eMode(vmapSrc.m_eMode)
{
    AllocTable();
    InitTable();
}

TLogMap::~TLogMap()
{
    delete [] m_ac1LogTable;
}


// ---------------------------------------------------------------------------
//  TLogMap: Public operators
// ---------------------------------------------------------------------------
TLogMap& TLogMap::operator=(const TLogMap& vmapSrc)
{
    if (&vmapSrc != this)
    {
        // Remember the current max indexes
        tCIDLib::TCard4 c4Max1 = c4MaxIndex1();
        tCIDLib::TCard4 c4Max2 = c4MaxIndex2();

        // Call our parent, which will update the new maximums
        TParent::operator=(vmapSrc);

        // Store our map mode and element size
        m_c4ElemSize    = vmapSrc.m_c4ElemSize;
        m_eMode         = vmapSrc.m_eMode;

        // If the indexes changed, then redo the look up table
        if ((c4Max1 != c4MaxIndex1()) || (c4Max2 != c4MaxIndex2()))
        {
            delete [] m_ac1LogTable;
            m_ac1LogTable = 0;
            AllocTable();
            InitTable();
        }
    }
    return *this;
}


tCIDLib::TBoolean TLogMap::operator==(const TLogMap& vmapSrc) const
{
    if (this != &vmapSrc)
    {
        // Compare our parent's stuff first
        if (!TParent::operator==(vmapSrc))
            return kCIDLib::False;

        // Compare our members
        if ((m_c4ElemSize != vmapSrc.m_c4ElemSize)
        ||  (m_eMode != vmapSrc.m_eMode))
        {
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean TLogMap::operator!=(const TLogMap& vmapSrc) const
{
    return !operator==(vmapSrc);
}



// ---------------------------------------------------------------------------
//  TLogMap: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TLogMap::c4MapIndex(const tCIDLib::TCard4 c4Index1) const
{
    // Sanity check the index
    if (c4Index1 > c4MaxIndex1())
    {
        facCIDMath().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMathErrs::errcVMap_RangeIndexError
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TCardinal(c4Index1)
            , TCardinal(c4MaxIndex1())
        );
    }

    //
    //  Return the element at that index. This method handles the different
    //  element sizes we can have.
    //
    return c4ElemAt(c4Index1);
}


tCIDLib::TVoid
TLogMap::SetNewRanges(  const   tCIDLib::TCard4 c4NewMax1
                        , const tCIDLib::TCard4 c4NewMax2)
{
    // If they changed, then we need to reallocate
    tCIDLib::TCard4 c4Max1 = c4MaxIndex1();
    tCIDLib::TCard4 c4Max2 = c4MaxIndex2();

    // Call our parent
    TParent::SetNewRanges(c4NewMax1, c4NewMax2);

    // Redo the table if we need to
    if ((c4NewMax1 != c4Max1) || (c4NewMax2 != c4Max2))
    {
        delete [] m_ac1LogTable;
        m_ac1LogTable = 0;
        AllocTable();
        InitTable();
    }
}

// ---------------------------------------------------------------------------
//  TLogMap: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TLogMap::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Save the old versions
    tCIDLib::TCard4 c4Max1 = c4MaxIndex1();
    tCIDLib::TCard4 c4Max2 = c4MaxIndex2();

    // Call our parent
    TParent::StreamFrom(strmToReadFrom);

    // Pull in the map mode
    strmToReadFrom >> m_eMode;

    // If the indices changed, then reinitialize the table
    if ((c4MaxIndex1() != c4Max1) || (c4MaxIndex2() != c4Max2))
    {
        if (c4MaxIndex1() < 0xFF)
            m_c4ElemSize = 1;
        else if (c4MaxIndex1() < 0xFFFF)
            m_c4ElemSize = 2;
        else
            m_c4ElemSize = 4;

        delete [] m_ac1LogTable;
        m_ac1LogTable = 0;
        AllocTable();
    }
    InitTable();
}

tCIDLib::TVoid TLogMap::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Call our parent first
    TParent::StreamTo(strmToWriteTo);

    // And write out our map mode
    strmToWriteTo << m_eMode;
}


// ---------------------------------------------------------------------------
//  TLogMap: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This method allocates the table the correct size according to the first
//  max index and the element size.
//
tCIDLib::TVoid TLogMap::AllocTable()
{
    m_ac1LogTable = new tCIDLib::TCard1[(c4MaxIndex1()+1) * m_c4ElemSize];
}

//
//  This method will return the indicated element, taking into account the
//  element size.
//
tCIDLib::TCard4 TLogMap::c4ElemAt(const tCIDLib::TCard4 c4Index) const
{
    #if CID_DEBUG_ON
    if (c4Index > c4MaxIndex1())
    {
        facCIDMath().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMathErrs::errcVMap_RangeIndexError
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TCardinal(c4Index)
            , TCardinal(c4MaxIndex1())
        );
    }
    #endif

    tCIDLib::TCard4 c4Ret = 0;
    if (m_c4ElemSize == 1)
    {
        c4Ret = tCIDLib::TCard4(m_ac1LogTable[c4Index]);
    }
     else if (m_c4ElemSize == 2)
    {
        c4Ret = *reinterpret_cast<tCIDLib::TCard2*>(m_ac1LogTable + (c4Index*2));
    }
     else if (m_c4ElemSize == 4)
    {
        c4Ret = *reinterpret_cast<tCIDLib::TCard4*>(m_ac1LogTable + (c4Index*4));
    }
     else
    {
        facCIDMath().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMathErrs::errcVMap_BadElemSize
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TCardinal(m_c4ElemSize)
        );
    }
    return c4Ret;
}


//
//  Initializes the table, given the current state. It assumes that the
//  other members are set and the table is allocated.
//
tCIDLib::TVoid TLogMap::InitTable()
{
    tCIDLib::TFloat8    f8Spread;
    tCIDLib::TCard4     c4Index;
    tCIDLib::TCard4     c4Limit;
    tCIDLib::TCard4     c4Prev;

    // For speed and convenience, get the max indexes out
    tCIDLib::TCard4     c4Max1 = c4MaxIndex1();
    tCIDLib::TCard4     c4Max2 = c4MaxIndex2();

    tCIDLib::TCard4     c4Mode = 0;
    if (m_eMode == tCIDLib::ELogMapModes::Spread)
        c4Mode++;

    if (c4Mode >= c4Max1)
        c4Mode = c4Max1 - 1;

    f8Spread = TMathLib::f8Log(tCIDLib::TFloat8(c4Max1 - c4Mode));
    f8Spread /= tCIDLib::TFloat8(c4Max2 - (c4Mode ? 2.0 : 1.0));
    for (c4Prev = 1; c4Prev <= c4Mode; c4Prev++)
        PutAt(c4Prev, 1);

    for (c4Index = (c4Mode ? 2 : 1); c4Index < c4Max2; c4Index++)
    {
        c4Limit = tCIDLib::TCard4
        (
            TMathLib::f8Exponent(tCIDLib::TFloat8(c4Index) * f8Spread)
        );

        c4Limit += c4Mode;
        if ((c4Limit > c4Max1) || (c4Index == c4Max2 - 1))
            c4Limit = c4Max1-1;

        while (c4Prev <= c4Limit)
        {
            PutAt(c4Prev, c4Index);
            c4Prev++;
        }
    }

    PutAt(0, 0);
    for (c4Index = 1; c4Index < c4Max1; c4Index++)
    {
        if (c4ElemAt(c4Index) > c4ElemAt(c4Index-1))
            PutAt(c4Index, c4ElemAt(c4Index - 1) + 1);
    }
}


//
//  This method will put a new value into the table. It will cut down the
//  value to fit into the element size. It is assumed that our internal
//  callers will not call us with a value too big. If they do, then it will
//  get sliced.
//
tCIDLib::TVoid TLogMap::PutAt(const   tCIDLib::TCard4 c4Index
                                , const tCIDLib::TCard4 c4Val)
{
    #if CID_DEBUG_ON
    if (c4Index > c4MaxIndex1())
    {
        facCIDMath().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMathErrs::errcVMap_RangeIndexError
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TCardinal(c4Index)
            , TCardinal(c4MaxIndex1())
        );
    }
    #endif

    if (m_c4ElemSize == 1)
    {
        m_ac1LogTable[c4Index] = tCIDLib::TCard1(c4Val);
    }
     else if (m_c4ElemSize == 2)
    {
        *((tCIDLib::TCard2*)(m_ac1LogTable + (c4Index*2))) = tCIDLib::TCard2(c4Val);
    }
     else if (m_c4ElemSize == 4)
    {
        *((tCIDLib::TCard4*)(m_ac1LogTable + (c4Index*4))) = c4Val;
    }
     else
    {
        facCIDMath().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMathErrs::errcVMap_BadElemSize
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Internal
            , TCardinal(m_c4ElemSize)
        );
    }
}


// ---------------------------------------------------------------------------
//  CLASS: TModMap
// PREFIX: vmap
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TModMap: Constructors and Destructor
// ---------------------------------------------------------------------------
TModMap::TModMap() :

    TValueMap()
{
}

TModMap::TModMap(const  tCIDLib::TCard4 c4MaxIndex1
                , const tCIDLib::TCard4 c4MaxIndex2) :

    TValueMap(c4MaxIndex1, c4MaxIndex2)
{
}

TModMap::~TModMap()
{
}


// ---------------------------------------------------------------------------
//  TModMap: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TModMap::operator==(const TModMap& vmapToCompare) const
{
    return TParent::operator==(vmapToCompare);
}

tCIDLib::TBoolean TModMap::operator!=(const TModMap& vmapToCompare) const
{
    return !TParent::operator==(vmapToCompare);
}


// ---------------------------------------------------------------------------
//  TModMap: Public non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TModMap::c4MapIndex(const tCIDLib::TCard4 c4ToMap) const
{
    return c4ToMap % c4MaxIndex2();
}


// ---------------------------------------------------------------------------
//  TModMap: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TModMap::StreamFrom(TBinInStream& strmToReadFrom)
{
    TParent::StreamFrom(strmToReadFrom);
}

tCIDLib::TVoid TModMap::StreamTo(TBinOutStream& strmToWriteTo) const
{
    TParent::StreamTo(strmToWriteTo);
}


