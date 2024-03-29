//
// FILE NAME: CIDLib_Boolean.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/26/2017
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
//  This file provides a simple wrapper class for a boolean value, for when we need
//  a boolean that acts like an object.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TBoolean,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TBoolean
//  PREFIX: b
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBoolean: Public, static methods
// ---------------------------------------------------------------------------
TBoolean& TBoolean::Nul_TBoolean() noexcept
{
    static TBoolean bNull;
    return bNull;
}


// ---------------------------------------------------------------------------
//  TBoolean: Constructors and Destructor
// ---------------------------------------------------------------------------
TBoolean::TBoolean(const tCIDLib::TBoolean bVal) noexcept :

    m_bVal(bVal)
{
}


// ---------------------------------------------------------------------------
//  TBoolean: Public operators
// ---------------------------------------------------------------------------
TBoolean& TBoolean::operator=(tCIDLib::TBoolean bSrc)
{
    m_bVal = bSrc;
    return *this;
}

tCIDLib::TBoolean TBoolean::operator!=(const TBoolean& bToTest) const
{
    return !operator==(bToTest);
}

tCIDLib::TBoolean TBoolean::operator==(const tCIDLib::TBoolean bToTest) const
{
    return (bToTest == m_bVal);
}

tCIDLib::TBoolean TBoolean::operator!=(const tCIDLib::TBoolean bToTest) const
{
    return (bToTest != m_bVal);
}

tCIDLib::TBoolean TBoolean::operator==(const TBoolean& bToTest) const
{
    if (this == &bToTest)
        return kCIDLib::True;

    return (m_bVal == bToTest.m_bVal);
}


// ---------------------------------------------------------------------------
//  TBoolean: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TBoolean::bVal(const tCIDLib::TBoolean bNewVal) noexcept
{
    m_bVal = bNewVal;
    return m_bVal;
}


// ---------------------------------------------------------------------------
//  TBoolean: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TBoolean::FormatTo(TTextOutStream& strmToWriteTo) const
{
    strmToWriteTo << m_bVal;
}


tCIDLib::TVoid TBoolean::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom  >> m_bVal;
}

tCIDLib::TVoid TBoolean::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo << m_bVal;
}
