//
// FILE NAME: CIDOrbWU_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/28/2004
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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


