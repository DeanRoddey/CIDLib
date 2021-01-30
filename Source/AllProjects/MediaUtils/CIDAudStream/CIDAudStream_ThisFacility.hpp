//
// FILE NAME: CIDAudStream_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/11/2017
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
//  This is the header file for the CIDAudStream_ThisFacility.cpp file. This
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
//  CLASS: TFacCIDAudStream
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDAUDSTREAMEXP TFacCIDAudStream : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDAudStream();

        TFacCIDAudStream(const TFacCIDAudStream&) = delete;
        TFacCIDAudStream(TFacCIDAudStream&&) = delete;

        ~TFacCIDAudStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDAudStream& operator=(const TFacCIDAudStream&) = delete;
        TFacCIDAudStream& operator=(TFacCIDAudStream&&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDAudStream,TFacility)
};

#pragma CIDLIB_POPPACK

