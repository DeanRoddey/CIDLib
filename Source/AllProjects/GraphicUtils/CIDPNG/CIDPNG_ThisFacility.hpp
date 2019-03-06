//
// FILE NAME: CIDPNG_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/14/2004
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDPNG_ThisFacility.Cpp file. This
//  file implements the facility class for this facility.
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
//  CLASS: TFacCIDPNG
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDPNGEXP TFacCIDPNG : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TFacCIDPNG();

        TFacCIDPNG(const TFacCIDPNG&) = delete;

        ~TFacCIDPNG();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDPNG& operator=(const TFacCIDPNG&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDPNG,TFacility)
};

#pragma CIDLIB_POPPACK

