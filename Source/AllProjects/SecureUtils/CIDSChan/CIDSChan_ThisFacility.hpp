//
// FILE NAME: CIDSChan_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/11/2014
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDSChan_ThisFacility.cpp file. This
//  file implements the facility class for this facility. An object of this
//  class is created and exported to the outside world to give them access to
//  this facility's global functionality and resources. The actual object is
//  lazily evaluated by way of an exported access method.
//
// CAVEATS/GOTCHAS:
//
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TFacCIDSChan
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDSCHANEXP TFacCIDSChan : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructors.
        // -------------------------------------------------------------------
        TFacCIDSChan();

        TFacCIDSChan(const TFacCIDSChan&) = delete;

        ~TFacCIDSChan();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDSChan& operator=(const TFacCIDSChan&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bParseCertInfo
        (
            const   TString&                strToParse
            ,       TString&                strType
            ,       TString&                strStore
            ,       TString&                strName
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDSChan,TFacility)
};

#pragma CIDLIB_POPPACK


