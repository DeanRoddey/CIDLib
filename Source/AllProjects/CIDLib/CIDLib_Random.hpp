//
// FILE NAME: CIDLib_Random.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/20/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_Random.cpp file, which implements the TRandomNum
//  class. This class provides a way to generate sequences of floating point or cardinal
//  random numbers. This is sort of math'y but it can't go into CIDMath because
//  CIDCrypto uses CIDMath and the IDL compiler uses CIDCrypto, and that would mean
//  that CIDMath would be yet another that can't use the IDL compiler. This is such
//  a small class that it's worth having it down here were it's available to everyone.
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
//  CLASS: TRandomNum
// PREFIX: rand
// ---------------------------------------------------------------------------
class CIDLIBEXP TRandomNum : public TObject
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TRandomNum();

        TRandomNum(const TRandomNum&) = delete;

        ~TRandomNum();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TRandomNum& operator=(const TRandomNum&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4GetNextNum();

        tCIDLib::TFloat8 f8GetNextNum();

        tCIDLib::TVoid Seed(const tCIDLib::TCard4 c4Seed);


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        void DoRand48
        (
                    tCIDLib::TCard2         ac2Seed[3]
        );

        double f8DoRand48
        (
                    tCIDLib::TCard2         ac2Seed[3]
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ac2Mult
        //      This is a temp array used internally.
        //
        //  m_ac2Seed
        //      This is the seed info that maintains the state over multiple
        //      calls.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_ac2Mult[3];
        tCIDLib::TCard2 m_ac2Seed[3];


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TRandomNum,TObject)
};

#pragma CIDLIB_POPPACK

