//
// FILE NAME: CIDMacroDbg_StateInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/19/2003
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
//  This is the header for the file CIDMacroDbg_StateInfo.cpp, which implements
//  a small class that we can give back to the outside world. It will ultimately
//  be used to give them last state info that they can give back to us so that we can
//  re-create the size/positions of things, previously opened calsses, and such.
//
//  But, for now, it's just empty so that they can at least go through the motions
//  and be ready for when we get this implemented.
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
//   CLASS: TMacroDbgStateInfo
//  PREFIX: sti
// ---------------------------------------------------------------------------
class CIDMACRODBGEXP TMacroDbgStateInfo : public TObject, public MStreamable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMacroDbgStateInfo();

        TMacroDbgStateInfo(const TMacroDbgStateInfo&) = default;

        ~TMacroDbgStateInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMacroDbgStateInfo& operator=(const TMacroDbgStateInfo&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Reset();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetDefaults();


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMacroDbgStateInfo,TObject)
};

#pragma CIDLIB_POPPACK


