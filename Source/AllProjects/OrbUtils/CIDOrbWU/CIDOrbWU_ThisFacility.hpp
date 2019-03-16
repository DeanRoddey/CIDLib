//
// FILE NAME: CIDOrbWU_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/28/2004
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
//  Implements the facility class for this facility.
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
//   CLASS: TFacCIDOrbWU
//  PREFIX: fac
// ---------------------------------------------------------------------------
class CIDORBWUEXP TFacCIDOrbWU : public TFacility
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDOrbWU();

        TFacCIDOrbWU(const TFacCIDOrbWU&) = delete;

        ~TFacCIDOrbWU();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDOrbWU& operator=(const TFacCIDOrbWU&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDOrbWU,TFacility)
};

#pragma CIDLIB_POPPACK


