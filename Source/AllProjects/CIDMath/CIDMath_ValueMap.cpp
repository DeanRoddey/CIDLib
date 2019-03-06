//
// FILE NAME: CIDMath_ValueMap.cpp
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
//  This file implements the abstract TValueMap class. This class is the
//  base class of a family of value mapping objects that can be dealt with
//  polymorphically.
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
RTTIDecls(TValueMap,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TValueMap
// PREFIX: vmap
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TValueMap: Constructors and Destructor
// ---------------------------------------------------------------------------
TValueMap::TValueMap() :

    m_c4MaxIndex1(1)
    , m_c4MaxIndex2(1)
{
}

TValueMap::TValueMap(   const   tCIDLib::TCard4 c4MaxIndex1
                        , const tCIDLib::TCard4 c4MaxIndex2) :
    m_c4MaxIndex1(c4MaxIndex1)
    , m_c4MaxIndex2(c4MaxIndex2)
{
}

TValueMap::~TValueMap()
{
}


// ---------------------------------------------------------------------------
//  TValueMap: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TValueMap::operator==(const TValueMap& vmapSrc) const
{
    if (this != &vmapSrc)
    {
        if ((m_c4MaxIndex1 != vmapSrc.m_c4MaxIndex1)
        ||  (m_c4MaxIndex2 != vmapSrc.m_c4MaxIndex2))
        {
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean TValueMap::operator!=(const TValueMap& vmapSrc) const
{
    return !operator==(vmapSrc);
}


// ---------------------------------------------------------------------------
//  TValueMap: Public, virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TValueMap::c4MaxIndex1() const
{
    return m_c4MaxIndex1;
}


tCIDLib::TCard4 TValueMap::c4MaxIndex2() const
{
    return m_c4MaxIndex2;
}


tCIDLib::TVoid
TValueMap::SetNewRanges(const   tCIDLib::TCard4 c4MaxIndex1
                        , const tCIDLib::TCard4 c4MaxIndex2)
{
    // Store the new ones
    m_c4MaxIndex1 = c4MaxIndex1;
    m_c4MaxIndex2 = c4MaxIndex2;
}


// ---------------------------------------------------------------------------
//  TValueMap: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TValueMap::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Copy over the matrix values
    strmToReadFrom  >> m_c4MaxIndex1
                    >> m_c4MaxIndex2;
}

tCIDLib::TVoid TValueMap::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << m_c4MaxIndex1
                    << m_c4MaxIndex2;
}
