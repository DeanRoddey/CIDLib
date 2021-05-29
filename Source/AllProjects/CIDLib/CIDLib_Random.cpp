//
// FILE NAME: CIDLib_Random.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/20/2001
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
//  This file implements the TRandomNum class. This implementation is roughly
//  descended from the free BSD implementation. Its pretty commonly used and
//  probably well beaten on. Its also probably not ultra-random but it will
//  do for most applications.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TRandomNum,TObject)



// ---------------------------------------------------------------------------
//  Local constants
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDLib_Random
    {
        constexpr tCIDLib::TCard2 c2RandSeed0    = 0x330E;
        constexpr tCIDLib::TCard2 c2RandSeed1    = 0xABCD;
        constexpr tCIDLib::TCard2 c2RandSeed2    = 0x1234;
        constexpr tCIDLib::TCard2 c2RandMult0    = 0xE66D;
        constexpr tCIDLib::TCard2 c2RandMult1    = 0xDEEC;
        constexpr tCIDLib::TCard2 c2RandMult2    = 0x0005;
        constexpr tCIDLib::TCard4 c4RandAdd      = 0xB;
    }
}




// ---------------------------------------------------------------------------
//  CLASS: TRandomNum
// PREFIX: rand
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TRandomNum: Constructors and Destructor
// ---------------------------------------------------------------------------
TRandomNum::TRandomNum()
{
    // Set up the initial default state
    m_ac2Seed[0] = CIDLib_Random::c2RandSeed0;
    m_ac2Seed[1] = CIDLib_Random::c2RandSeed1;
    m_ac2Seed[2] = CIDLib_Random::c2RandSeed2;
    m_ac2Mult[0] = CIDLib_Random::c2RandMult0;
    m_ac2Mult[1] = CIDLib_Random::c2RandMult1;
    m_ac2Mult[2] = CIDLib_Random::c2RandMult2;
}

TRandomNum::~TRandomNum()
{
}


// ---------------------------------------------------------------------------
//  TRandomNum: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TRandomNum::c4GetNextNum()
{
    // Just run another round on the seed and create the value from the result
    DoRand48(m_ac2Seed);
    return (tCIDLib::TCard4(m_ac2Seed[2]) << 15)
           + (tCIDLib::TCard4(m_ac2Seed[1]) >> 1);
}


tCIDLib::TFloat8 TRandomNum::f8GetNextNum()
{
    // Run another round on our seed member
    DoRand48(m_ac2Seed);

    // And then convert the results to floating point format
    return  TMathLib::f8MakeReal(tCIDLib::TFloat8(m_ac2Seed[0]), -48)
            + TMathLib::f8MakeReal(tCIDLib::TFloat8(m_ac2Seed[1]), -32)
            + TMathLib::f8MakeReal(tCIDLib::TFloat8(m_ac2Seed[2]), -16);
}


tCIDLib::TVoid TRandomNum::Seed(const tCIDLib::TCard4 c4Seed)
{
    m_ac2Seed[0] = CIDLib_Random::c2RandSeed0;
    m_ac2Seed[1] = TRawBits::c2Low16From32(c4Seed);
    m_ac2Seed[2] = TRawBits::c2High16From32(c4Seed);;
    m_ac2Mult[0] = CIDLib_Random::c2RandMult0;
    m_ac2Mult[1] = CIDLib_Random::c2RandMult1;
    m_ac2Mult[2] = CIDLib_Random::c2RandMult2;
}


// ---------------------------------------------------------------------------
//  TRandomNum: Private, non-virtual methods
// ---------------------------------------------------------------------------
void TRandomNum::DoRand48(tCIDLib::TCard2 ac2Seed[3])
{
    tCIDLib::TCard4 c4Accum = 0;
    tCIDLib::TCard2 c2Temp[2] = {0};

    c4Accum = (tCIDLib::TCard4(m_ac2Mult[0]) * tCIDLib::TCard4(ac2Seed[0]))
              + CIDLib_Random::c4RandAdd;

    // Do the low half
    c2Temp[0] = tCIDLib::TCard2(c4Accum);
    c4Accum >>= sizeof(tCIDLib::TCard2) * 8;
    c4Accum += (tCIDLib::TCard4(m_ac2Mult[0]) * tCIDLib::TCard4(ac2Seed[1]))
               + (tCIDLib::TCard4(m_ac2Mult[1]) * tCIDLib::TCard4(ac2Seed[0]));

    // And the high half
    c2Temp[1] = tCIDLib::TCard2(c4Accum);
    c4Accum >>= sizeof(tCIDLib::TCard2) * 8;
    c4Accum += (tCIDLib::TCard4(m_ac2Mult[0]) * tCIDLib::TCard4(ac2Seed[2]))
               + (tCIDLib::TCard4(m_ac2Mult[1]) * tCIDLib::TCard4(ac2Seed[1]))
               + (tCIDLib::TCard4(m_ac2Mult[2]) * tCIDLib::TCard4(ac2Seed[0]));

    // Update the seed array
    ac2Seed[0] = c2Temp[0];
    ac2Seed[1] = c2Temp[1];
    ac2Seed[2] = tCIDLib::TCard2(c4Accum);
}

