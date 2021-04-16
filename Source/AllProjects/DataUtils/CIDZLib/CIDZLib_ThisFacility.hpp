//
// FILE NAME: CIDZLib_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/13/2004
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
//  This is the header file for the CIDZLib_ThisFacility.Cpp file. This
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
//  CLASS: TFacCIDZLib
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDZLIBEXP TFacCIDZLib : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TFacCIDZLib();

        TFacCIDZLib(const TFacCIDZLib&) = delete;
        TFacCIDZLib(TFacCIDZLib&&) = delete;

        ~TFacCIDZLib();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDZLib& operator=(const TFacCIDZLib&) = delete;
        TFacCIDZLib& operator=(TFacCIDZLib&&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDZLib,TFacility)
};

#pragma CIDLIB_POPPACK
