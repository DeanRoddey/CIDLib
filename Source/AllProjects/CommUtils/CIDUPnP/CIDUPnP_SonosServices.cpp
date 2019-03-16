//
// FILE NAME: CIDUPnP_SonosServices.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/16/2012
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
//  This file implements some services that are specific to the Sonos
//  Zone Player device type.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDUPnP_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TUPnPAudInService, TUPnPService)
RTTIDecls(TUPnPDevPropsService, TUPnPService)
RTTIDecls(TUPnPGrpMgmtService, TUPnPService)



// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDUPnP_SonosServices
{
    // -----------------------------------------------------------------------
    //  The commands we use
    // -----------------------------------------------------------------------
    const TString   strAudInCmd_GetInLevel(L"GetLineInLevel");
    const TString   strAudInCmd_SelAudio(L"SelectAudio");

    const TString   strDevPropsCmd_GetAPLinked(L"GetAutoplayLinkedZones");
    const TString   strDevPropsCmd_GetAPRoomID(L"GetAutoplayRoomUUID");
    const TString   strDevPropsCmd_GetAPVolume(L"GetAutoplayVolume");

    const TString   strDevPropsCmd_GetLEDState(L"GetLEDState");
    const TString   strDevPropsCmd_GetZoneAttrs(L"GetZoneAttributes");
    const TString   strDevPropsCmd_GetZoneInfo(L"GetZoneInfo");
    const TString   strDevPropsCmd_SetLEDState(L"SetLEDState");


    // -----------------------------------------------------------------------
    //  State variables
    // -----------------------------------------------------------------------

    // Device properties service
    const TString   strVar_Icon(L"Icon");
    const TString   strVar_IsZoneBridge(L"IsZoneBridge");
    const TString   strVar_LineInConn(L"LineInConnected");
    const TString   strVar_ZoneName(L"ZoneName");

    // Group coordinator service
    const TString   strVar_GrpCoordIsLocal(L"GroupCoordinatorIsLocal");
    const TString   strVar_LocalGrpUID(L"LocalGroupUUID");
    const TString   strVar_VolAVTransUIRT(L"VolumeAVTransportURI");
}



// ---------------------------------------------------------------------------
//  CLASS: TUPnPAudInService
// PREFIX: upnps
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TUPnPAudInService: Constructors and Destructor
// -------------------------------------------------------------------
TUPnPAudInService::TUPnPAudInService()
{
}

TUPnPAudInService::~TUPnPAudInService()
{
}


// -------------------------------------------------------------------
//  TUPnPAudInService: Public, inherited methods
// -------------------------------------------------------------------

// We only support audio in services
tCIDLib::TBoolean
TUPnPAudInService::bCheckServiceType(const TString& strTypeID) const
{
    return (strTypeID == kCIDUPnP::strSvcType_AudioIn);
}


// -------------------------------------------------------------------
//  TUPnPAudInService: Public, non-virtual methods
// -------------------------------------------------------------------

tCIDLib::TBoolean TUPnPAudInService::bLineInConnected()
{
    QueryStateVar(CIDUPnP_SonosServices::strVar_LineInConn, m_strTmp);
    return (m_strTmp.bCompareI(TFacCIDUPnP::strUPnP_True));
}


tCIDLib::TVoid
TUPnPAudInService::QueryLineLevels( tCIDLib::TCard4&    c4LeftLevel
                                    , tCIDLib::TCard4&  c4RightLevel)
{
    const TString& strCmd = CIDUPnP_SonosServices::strAudInCmd_GetInLevel;

    m_colInParms.RemoveAll();
    InvokeAction(strCmd, m_colInParms, m_colOutParms);
    CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 2);

    if (!m_colOutParms[0].bToCard4(c4LeftLevel, tCIDLib::ERadices::Dec))
        ThrowBadOutActParms(strCmd, CID_LINE);

    if (!m_colOutParms[1].bToCard4(c4RightLevel, tCIDLib::ERadices::Dec))
        ThrowBadOutActParms(strCmd, CID_LINE);
}




// ---------------------------------------------------------------------------
//  CLASS: TUPnPDevPropsService
// PREFIX: upnps
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TUPnPDevPropsService: Constructors and Destructor
// -------------------------------------------------------------------
TUPnPDevPropsService::TUPnPDevPropsService()
{
}

TUPnPDevPropsService::~TUPnPDevPropsService()
{
}



// -------------------------------------------------------------------
//  TUPnPAudInService: Public, inherited methods
// -------------------------------------------------------------------

// We only support the device propertie sservices
tCIDLib::TBoolean
TUPnPDevPropsService::bCheckServiceType(const TString& strTypeID) const
{
    return (strTypeID == kCIDUPnP::strSvcType_DeviceProperties);
}


// -------------------------------------------------------------------
//  TUPnPDevPropsService: Public, non-virtual methods
// -------------------------------------------------------------------

//
//  Query whether this unit is a bridge ornot. This can be evented, so we
//  return that if available, else we tell the state query method to
//  do the direct query.
//
tCIDLib::TBoolean TUPnPDevPropsService::bIsBridge() const
{
    QueryStateVar
    (
        CIDUPnP_SonosServices::strVar_IsZoneBridge, m_strTmp, kCIDLib::True
    );
    return (m_strTmp.bCompareI(TFacCIDUPnP::strUPnP_True));
}


//
//  Get or set the front panel LED state. This isn't evented, so we have
//  to poll it if we want to keep an up to date status.
//
tCIDLib::TBoolean TUPnPDevPropsService::bLEDState() const
{
    const TString& strCmd = CIDUPnP_SonosServices::strDevPropsCmd_GetLEDState;

    m_colInParms.RemoveAll();
    InvokeAction(strCmd, m_colInParms, m_colOutParms);
    CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 1);

    return m_colOutParms[0].bCompare(L"On");
}

tCIDLib::TBoolean TUPnPDevPropsService::bLEDState(const tCIDLib::TBoolean bToSet)
{
    const TString& strCmd = CIDUPnP_SonosServices::strDevPropsCmd_SetLEDState;

    m_colInParms.RemoveAll();
    if (bToSet)
        m_colInParms.objAdd(TString(L"On"));
    else
        m_colInParms.objAdd(TString(L"Off"));

    InvokeAction(strCmd, m_colInParms, m_colOutParms);
    CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 0);

    return bToSet;
}


//
//  Query some auto-play info. This stuff is not evented, so we always have
//  to do the query.
//
tCIDLib::TVoid
TUPnPDevPropsService::QueryAutoPlay(tCIDLib::TBoolean&  bLinkedZones
                                    , TString&          strRoomID
                                    , tCIDLib::TCard4&  c4Volume)
{
    {
        const TString& strCmd = CIDUPnP_SonosServices::strDevPropsCmd_GetAPLinked;

        m_colInParms.RemoveAll();
        InvokeAction(strCmd, m_colInParms, m_colOutParms);
        CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 1);
        bLinkedZones = m_colOutParms[0].bCompareI(TFacCIDUPnP::strUPnP_True);
    }

    {
        const TString& strCmd = CIDUPnP_SonosServices::strDevPropsCmd_GetAPRoomID;

        m_colInParms.RemoveAll();
        InvokeAction(strCmd, m_colInParms, m_colOutParms);
        CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 1);
        strRoomID = m_colOutParms[0];
    }

    {
        const TString& strCmd = CIDUPnP_SonosServices::strDevPropsCmd_GetAPVolume;

        m_colInParms.RemoveAll();
        InvokeAction(strCmd, m_colInParms, m_colOutParms);
        CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 1);

        if (!m_colOutParms[0].bToCard4(c4Volume, tCIDLib::ERadices::Dec))
            ThrowBadOutActParms(strCmd, CID_LINE);
    }
}


//
//  Query the basic zone player attributes. This can be evented, so use the
//  evented values if available.
//
//  Note that this one changed at some point, and it now returns three values
//  but we only still use the first two. So we tell the return parm checker
//  to just make sure it's at least 2.
//
tCIDLib::TVoid
TUPnPDevPropsService::QueryZoneAttrs(TString& strZoneName, TString& strCurIcon)
{
    const TString& strCmd = CIDUPnP_SonosServices::strDevPropsCmd_GetZoneAttrs;

    // If evented and we have both values, return those. Else query
    if (!m_bIsEvented
    ||  !bQueryStateVar(CIDUPnP_SonosServices::strVar_Icon, strCurIcon)
    ||  !bQueryStateVar(CIDUPnP_SonosServices::strVar_ZoneName, strZoneName))
    {
        m_colInParms.RemoveAll();
        InvokeAction(strCmd, m_colInParms, m_colOutParms);
        CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 2, kCIDLib::False);

        strZoneName = m_colOutParms[0];
        strCurIcon = m_colOutParms[1];
    }
}


//
//  Query some more static type info. This stuff is not evented, so we
//  always query it.
//
//  Note that this one changed at some point, and it now returns more values
//  but we only still use the first 8. So we tell the return parm checker
//  to just make sure it's at least 8.
//
tCIDLib::TVoid
TUPnPDevPropsService::QueryZoneInfo(TString&                strSerialNum
                                    , TString&              strIPAddress
                                    , TString&              strMacAddress
                                    , TString&              strSoftwareVer)
{
    const TString& strCmd = CIDUPnP_SonosServices::strDevPropsCmd_GetZoneInfo;

    m_colInParms.RemoveAll();
    InvokeAction(strCmd, m_colInParms, m_colOutParms);
    CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 8, kCIDLib::False);

    strSerialNum = m_colOutParms[0];
    strSoftwareVer = m_colOutParms[1];
    strIPAddress = m_colOutParms[4];
    strMacAddress = m_colOutParms[5];
}





// ---------------------------------------------------------------------------
//  CLASS: TUPnPGrpMgmtService
// PREFIX: upnps
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TUPnPGrpMgmtService: Constructors and Destructor
// -------------------------------------------------------------------
TUPnPGrpMgmtService::TUPnPGrpMgmtService()
{
}

TUPnPGrpMgmtService::~TUPnPGrpMgmtService()
{
}



// -------------------------------------------------------------------
//  TUPnPAudInService: Public, inherited methods
// -------------------------------------------------------------------

// We only support the device propertie sservices
tCIDLib::TBoolean
TUPnPGrpMgmtService::bCheckServiceType(const TString& strTypeID) const
{
    return (strTypeID == kCIDUPnP::strSvcType_GroupManagement);
}



// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------

// Return whether we are the coordinator of our group or not
tCIDLib::TBoolean TUPnPGrpMgmtService::bCoordIsLocal() const
{
    // Tell it to query if not in the list
    QueryStateVar
    (
        CIDUPnP_SonosServices::strVar_GrpCoordIsLocal, m_strTmp, kCIDLib::True
    );
    return m_strTmp.bCompareI(TFacCIDUPnP::strUPnP_True);
}


// Return the unique id of the group coordinator
tCIDLib::TVoid
TUPnPGrpMgmtService::QueryGrpCoordinator(TString&   strCoordUID
                                        , TString&  strGroupUID) const
{
    // Tell it to query if not in the list
    QueryStateVar
    (
        CIDUPnP_SonosServices::strVar_LocalGrpUID, strGroupUID, kCIDLib::True
    );

    //
    //  The coordinator is the group with the trailing group number removed,
    //  which is in the form :x. So we look for the last colon and cap it
    //  there.
    //
    strCoordUID = strGroupUID;
    strCoordUID.bCapAtChar(kCIDLib::chColon);
}


// Query the group volume AV Transport URI
tCIDLib::TVoid
TUPnPGrpMgmtService::QueryVolAVTranportURI(TString& strToFill) const
{
    // Tell it to query if not in the list
    QueryStateVar
    (
        CIDUPnP_SonosServices::strVar_VolAVTransUIRT, strToFill, kCIDLib::True
    );
}



