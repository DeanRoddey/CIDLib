//
// FILE NAME: CIDRegX_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDRegX_ThisFacility.cpp file, which
//  implements the facility class for this facility. An object of this type
//  is lazily evaluated by a method exported from the main file of this
//  facility.
//
//  This class derives from the TFacility class, from which it inherits a lot
//  of functionality that is module handle oriented.
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
//  CLASS: TFacCIDRegX
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDREGXEXP TFacCIDRegX : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TFacCIDRegX();

        ~TFacCIDRegX();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TFacCIDRegX(const TFacCIDRegX&);
        tCIDLib::TVoid operator=(const TFacCIDRegX&);


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDRegX,TFacility)
};

#pragma CIDLIB_POPPACK
