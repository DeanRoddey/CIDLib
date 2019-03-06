//
// FILE NAME: CIDSIP_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/05/2013
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDSIP_ThisFacility.cpp file. This
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
//  CLASS: TFacCIDSIP
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDSIPEXP TFacCIDSIP : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString    strMeth_Ack;
        static const TString    strMeth_Bye;
        static const TString    strMeth_Invite;
        static const TString    strMeth_OK;
        static const TString    strMeth_Ringing;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDSIP();

        ~TFacCIDSIP();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDSIP,TFacility)
};

#pragma CIDLIB_POPPACK


