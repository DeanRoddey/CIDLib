//
// FILE NAME: CIDUPnP_Sonos.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/18/2012
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDUPnP_Sonos.cpp file, which implements
//  some services related to Sonos Zone Player control.
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
//  CLASS: TUPnPAudInService
// PREFIX: upnps
// ---------------------------------------------------------------------------
class CIDUPNPEXP TUPnPAudInService : public TUPnPService
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUPnPAudInService();

        TUPnPAudInService(const TUPnPAudInService&) = delete;

        ~TUPnPAudInService();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUPnPAudInService& operator=(const TUPnPAudInService&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckServiceType
        (
            const   TString&                strTypeID
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bLineInConnected();

        tCIDLib::TVoid QueryLineLevels
        (
                    tCIDLib::TCard4&        c4LeftLevel
            ,       tCIDLib::TCard4&        c4RightLevel
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUPnPAudInService,TUPnPService)
};



// ---------------------------------------------------------------------------
//  CLASS: TUPnPDevPropsService
// PREFIX: upnps
// ---------------------------------------------------------------------------
class CIDUPNPEXP TUPnPDevPropsService : public TUPnPService
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUPnPDevPropsService();

        TUPnPDevPropsService(const TUPnPDevPropsService&) = delete;

        ~TUPnPDevPropsService();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUPnPDevPropsService& operator=(const TUPnPDevPropsService&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckServiceType
        (
            const   TString&                strTypeID
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsBridge() const;

        tCIDLib::TBoolean bLEDState() const;

        tCIDLib::TBoolean bLEDState
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TVoid QueryAutoPlay
        (
                    tCIDLib::TBoolean&      bLinkedZones
            ,       TString&                strRoom
            ,       tCIDLib::TCard4&        c4Volume
        );

        tCIDLib::TVoid QueryZoneAttrs
        (
                    TString&                strZoneName
            ,       TString&                strCurIcon
        );

        tCIDLib::TVoid QueryZoneInfo
        (
                    TString&                strSerialNum
            ,       TString&                strIPAddress
            ,       TString&                strMacAddress
            ,       TString&                strSoftwareVer
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUPnPDevPropsService,TUPnPService)
};



// ---------------------------------------------------------------------------
//  CLASS: TUPnPGrpMgmtService
// PREFIX: upnps
// ---------------------------------------------------------------------------
class CIDUPNPEXP TUPnPGrpMgmtService : public TUPnPService
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUPnPGrpMgmtService();

        TUPnPGrpMgmtService(const TUPnPGrpMgmtService&) = delete;

        ~TUPnPGrpMgmtService();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TUPnPGrpMgmtService& operator=(const TUPnPGrpMgmtService&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckServiceType
        (
            const   TString&                strTypeID
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCoordIsLocal() const;

        tCIDLib::TVoid QueryGrpCoordinator
        (
                    TString&                strCoordUID
            ,       TString&                strGroupUID
        )   const;

        tCIDLib::TVoid QueryVolAVTranportURI
        (
                    TString&                strToFill
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUPnPGrpMgmtService,TUPnPService)
};


#pragma CIDLIB_POPPACK

