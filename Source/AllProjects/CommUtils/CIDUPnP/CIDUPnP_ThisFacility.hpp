//
// FILE NAME: CIDUPnP_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/15/2011
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
//  This is the header file for the CIDUPnP_ThisFacility.cpp file. This
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
//  CLASS: TFacCIDUPnP
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDUPNPEXP TFacCIDUPnP : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString strUPnP_False;

        static const TString strUPnP_True;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDUPnP();

        ~TFacCIDUPnP();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bSetDeviceByType
        (
            const   tCIDLib::TKValsList&    colDevs
            ,       TUPnPDevice&            upnpdTar
            , const TString&                strType
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        tCIDLib::TBoolean bSetDeviceFromUID
        (
            const   TString&                strUID
            ,       TUPnPDevice&            upnpdToSet
        );

        tCIDLib::TBoolean bFindDevsByType
        (
            const   TString&                strType
            ,       tCIDLib::TKValsCollect& colFound
        );

        tCIDLib::TBoolean bSetServiceByType
        (
            const   tCIDLib::TKVPList&      colServices
            ,       TUPnPDevice&            upnpdPar
            ,       TUPnPService&           upnpsTar
            , const TString&                strType
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        tCIDLib::TVoid SetDeviceFromUID
        (
            const   TString&                strUID
            ,       TUPnPDevice&            upnpdToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDUPnP,TFacility)
};

#pragma CIDLIB_POPPACK

