//
// FILE NAME: CIDDAE_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2006
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDDAE_ThisFacility.cpp file. This
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
//  CLASS: TFacCIDDAE
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDDAEEXP TFacCIDDAE : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDDAE();

        TFacCIDDAE(const TFacCIDDAE&) = delete;

        ~TFacCIDDAE();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDDAE& operator=(const TFacCIDDAE&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatRuntime
        (
            const   tCIDLib::TCard4         c4Seconds
            ,       TString&                strFmt
        );

        tCIDLib::TVoid MungeTrackName
        (
                    TString&                strToMunge
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDDAE,TFacility)
};

#pragma CIDLIB_POPPACK

