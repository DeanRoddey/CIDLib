//
// FILE NAME: CIDUPnP_MediaServices.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/16/2011
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
//  This file implements some media oriented UPnP service derivatives
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
RTTIDecls(TUPnPAVTransService, TUPnPService)
RTTIDecls(TUPnPContDirService, TUPnPService)
RTTIDecls(TUPnPRendCtrlService, TUPnPService)
RTTIDecls(TUPnPTrackItem, TObject)



// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDUPnP_MediaServices
{
    // -----------------------------------------------------------------------
    //  This is some standard metadata that we just do some search and replace
    //  in with the info provided to us in the SetAVTransportURI method. We
    //  replace these tokens:
    //
    //  %(a)  - The artist
    //  %(t)  - Item title
    //  %(l)  - Album title
    //  %(n)  - The track number
    //  $(d)  - The release date
    // -----------------------------------------------------------------------
    const TString strDefMeta
    (
        L"<DIDL-Lite xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\">\n"
        L"    <item id=\"{2B1A8947-E988-44B1-84F3-F7C1B86375C7}.0.4\" restricted=\"0\" parentID=\"4\">\n"
        L"        <dc:title>%(t)</dc:title>\n"
        //L"        <dc:creator>%(a)</dc:creator>\n"
        //L"        <upnp:class>object.item.audioItem</upnp:class>\n"
        L"        <upnp:genre>%(g)</upnp:genre>\n"
        L"        <upnp:artist role=\"AlbumArtist\">%(a)</upnp:artist>\n"
        L"        <upnp:artist role=\"Performer\">%(a)Acri</upnp:artist>\n"
        //L"        <upnp:author role=\"Composer\">Robert R. Acri</upnp:author>\n"
        L"        <upnp:album>%(l)</upnp:album>\n"
        L"        <upnp:originalTrackNumber>%(n)</upnp:originalTrackNumber>\n"
        L"        <dc:date>%(d)</dc:date>\n"
        //L"        <upnp:actor>Bob Acri</upnp:actor>\n"
        //L"        <desc id=\"artist\" nameSpace=\"urn:schemas-microsoft-com:WMPNSS-1-0/\" xmlns:microsoft=\"urn:schemas-microsoft-com:WMPNSS-1-0/\">\n"
        //L"            <microsoft:artistAlbumArtist>Bob Acri</microsoft:artistAlbumArtist>\n"
        //L"            <microsoft:artistPerformer>Bob Acri</microsoft:artistPerformer>\n"
        //L"        </desc>\n"
        //L"        <desc id=\"author\" nameSpace=\"urn:schemas-microsoft-com:WMPNSS-1-0/\" xmlns:microsoft=\"urn:schemas-microsoft-com:WMPNSS-1-0/\">\n"
        //L"            <microsoft:authorComposer>Robert R. Acri</microsoft:authorComposer>\n"
        //L"        </desc>\n"
        //L"        <desc id=\"Year\" nameSpace=\"urn:schemas-microsoft-com:WMPNSS-1-0/\" xmlns:microsoft=\"urn:schemas-microsoft-com:WMPNSS-1-0/\">\n"
        //L"            <microsoft:year>2004</microsoft:year>\n"
        //L"        </desc>\n"
        //L"        <desc id=\"UserRating\" nameSpace=\"urn:schemas-microsoft-com:WMPNSS-1-0/\" xmlns:microsoft=\"urn:schemas-microsoft-com:WMPNSS-1-0/\">\n"
        //L"            <microsoft:userEffectiveRatingInStars>4</microsoft:userEffectiveRatingInStars>\n"
        //L"            <microsoft:userEffectiveRating>75</microsoft:userEffectiveRating>\n"
        //L"        </desc>\n"
        L"    </item>\n"
        L"</DIDL-Lite>\n"
    );


    // -----------------------------------------------------------------------
    //  The commands and variables we use
    // -----------------------------------------------------------------------
    const TString   strAVCmd_AddURIToQueue(L"AddURIToQueue");
    const TString   strAVCmd_BecomeLocalGroup(L"BecomeCoordinatorOfStandaloneGroup");
    const TString   strAVCmd_ClearQueue(L"RemoveAllTracksFromQueue");
    const TString   strAVCmd_GetCrossfadeMode(L"GetCrossfadeMode");
    const TString   strAVCmd_Next(L"Next");
    const TString   strAVCmd_Pause(L"Pause");
    const TString   strAVCmd_Play(L"Play");
    const TString   strAVCmd_Previous(L"Previous");
    const TString   strAVCmd_QueryPosInfo(L"GetPositionInfo");
    const TString   strAVCmd_QueryTransInfo(L"GetTransportInfo");
    const TString   strAVCmd_QueryTransSet(L"GetTransportSettings");
    const TString   strAVCmd_SaveQueue(L"SaveQueue");
    const TString   strAVCmd_SetAVTransURI(L"SetAVTransportURI");
    const TString   strAVCmd_SetCrossfadeMode(L"SetCrossfadeMode");
    const TString   strAVCmd_SetPlayMode(L"SetPlayMode");
    const TString   strAVCmd_StartAutoPlay(L"StartAutoPlay");
    const TString   strAVCmd_Stop(L"Stop");

    const TString   strCDCmd_Browse(L"Browse");
    const TString   strCDCmd_DestroyObject(L"DestroyObject");

    const TString   strRCCmd_QueryMute(L"GetMute");
    const TString   strRCCmd_QueryVolume(L"GetVolume");
    const TString   strRCCmd_QueryVolumeDB(L"GetVolumeDB");
    const TString   strRCCmd_QueryVolRange(L"GetVolumeDBRange");
    const TString   strRCCmd_SetMute(L"SetMute");
    const TString   strRCCmd_SetVolume(L"SetVolume");
    const TString   strRCCmd_SetVolumeDB(L"SetVolumeDB");


    // -----------------------------------------------------------------------
    //  Some other command action invocation strings we check for a lot or
    //  strings we use in lots of commands and such.
    // -----------------------------------------------------------------------
    const TString   strVal_EOM(L"END_OF_MEDIA");
    const TString   strVal_InstID(L"0");
    const TString   strVal_LastChange(L"LastChange");
    const TString   strVal_Untitled(L"[Untitled Track]");


    // -----------------------------------------------------------------------
    //  Some elements and attr names in the returned XML, ones that are not
    //  the names of variables be reported in composite LastChange type
    //  evented values.
    // -----------------------------------------------------------------------
    const TString   strXML_Album(L"upnp:album");
    const TString   strXML_Artist(L"dc:creator");
    const TString   strXML_ArtURL(L"upnp:albumArtURI");
    const TString   strXML_Item(L"item");
    const TString   strXML_ID(L"id");
    const TString   strXML_Master(L"Master");
    const TString   strXML_MetaData(L"r:resMD");
    const TString   strXML_ProtoInfo(L"protocolInfo");
    const TString   strXML_Res(L"res");
    const TString   strXML_Title(L"dc:title");
    const TString   strXML_TrackNum(L"upnp:originalTrackNumber");
    const TString   strXML_Val(L"val");


    // -----------------------------------------------------------------------
    //  State variable names
    // -----------------------------------------------------------------------
    const TString   strVar_AbsTimePos(L"AbsoluteTimePosition");
    const TString   strVar_AVTransURI(L"AVTransportURI");
    const TString   strVar_ContUpdateIDs(L"ContainerUpdateIDs");
    const TString   strVar_CrossfadeMode(L"CurrentCrossfadeMode");
    const TString   strVar_CurPlayMode(L"CurrentPlayMode");
    const TString   strVar_CurTrackDur(L"CurrentTrackDuration");
    const TString   strVar_CurrentTrackMetaData(L"CurrentTrackMetaData");
    const TString   strVar_CurTrackNum(L"CurrentTrack");
    const TString   strVar_CurTrackURI(L"CurrentTrackURI");
    const TString   strVar_Mute(L"Mute");
    const TString   strVar_NumTracks(L"NumberOfTracks");
    const TString   strVar_RelTimePos(L"RelativeTimePosition");
    const TString   strVar_TransSpeed(L"TransportPlaySpeed");
    const TString   strVar_TransState(L"TransportState");
    const TString   strVar_TransStatus(L"TransportStatus");
    const TString   strVar_Volume(L"Volume");
}



// ---------------------------------------------------------------------------
//  CLASS: TUPnPAVTransService
// PREFIX: upnps
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TUPnPAVTransService: Constructors and Destructor
// ---------------------------------------------------------------------------
TUPnPAVTransService::TUPnPAVTransService() :

    m_bDonePosInfo(kCIDLib::False)
    , m_bDoneTransInfo(kCIDLib::False)
{
}

TUPnPAVTransService::~TUPnPAVTransService()
{
}


// ---------------------------------------------------------------------------
//  TUPnPAVTransService: Public, inherited methods
// ---------------------------------------------------------------------------

// We only accept AV transport types
tCIDLib::TBoolean
TUPnPAVTransService::bCheckServiceType(const TString& strTypeID) const
{
    return (strTypeID == kCIDUPnP::strSvcType_AVTransport);
}


//
//  We override this one so that we can watch for our last change evented
//  variable, which is one of those composite ones transmitted as XML. So we
//  don't want the underlying service base class to see this one. We'll eat
//  it and parse the content and store it. Others we pass on.
//
//  What we do is call our parent version with the broken out values, so that
//  he will store them as normal cached evented state variable values that
//  will be seen later when we are queried.
//
tCIDLib::TVoid
TUPnPAVTransService::UPnPVarChanged(const   tCIDLib::TCh* const pszVarName
                                    , const tCIDLib::TCh* const pszVarVal)
{
    // If not the special case just pass to our parent class
    if (!CIDUPnP_MediaServices::strVal_LastChange.bCompareI(pszVarName))
    {
        TParent::UPnPVarChanged(pszVarName, pszVarVal);
        return;
    }

    // Else it's our guy so parse the XML content
    TXMLTreeParser xtprsItems;
    if (bParseXML(xtprsItems, TString(pszVarVal), L"AVTrans lastchage"))
    {
        // Get the root node, and then the InstanceID child, which is the first one
        const TXMLTreeElement& xtnodeRoot = xtprsItems.xtdocThis().xtnodeRoot();
        const TXMLTreeElement& xtnodeInst = xtnodeRoot.xtnodeChildAtAsElement(0);

        const tCIDLib::TCard4 c4Count = xtnodeInst.c4ChildCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TXMLTreeElement& xtnodeCur = xtnodeInst.xtnodeChildAtAsElement(c4Index);

            //
            //  For these we can just use a flag to indicate we want to store the
            //  value of the 'val' attribute. We just set a pointer to the name
            //  string and we can handle the storage for all of them simply below.
            //
            const TString* pstrName = 0;

            if (xtnodeCur.strQName() == CIDUPnP_MediaServices::strVar_AVTransURI)
                pstrName = &CIDUPnP_MediaServices::strVar_AVTransURI;
            else if (xtnodeCur.strQName() == CIDUPnP_MediaServices::strVar_CrossfadeMode)
                pstrName = &CIDUPnP_MediaServices::strVar_CrossfadeMode;
            else if (xtnodeCur.strQName() == CIDUPnP_MediaServices::strVar_CurPlayMode)
                pstrName = &CIDUPnP_MediaServices::strVar_CurPlayMode;
            else if (xtnodeCur.strQName() == CIDUPnP_MediaServices::strVar_CurTrackNum)
                pstrName = &CIDUPnP_MediaServices::strVar_CurTrackNum;
            else if (xtnodeCur.strQName() == CIDUPnP_MediaServices::strVar_CurTrackURI)
                pstrName = &CIDUPnP_MediaServices::strVar_CurTrackURI;
            else if (xtnodeCur.strQName() == CIDUPnP_MediaServices::strVar_CurTrackDur)
                pstrName = &CIDUPnP_MediaServices::strVar_CurTrackDur;
            else if (xtnodeCur.strQName() ==  CIDUPnP_MediaServices::strVar_CurrentTrackMetaData)
                pstrName = &CIDUPnP_MediaServices::strVar_CurrentTrackMetaData;
            else if (xtnodeCur.strQName() == CIDUPnP_MediaServices::strVar_NumTracks)
                pstrName = &CIDUPnP_MediaServices::strVar_NumTracks;
            else if (xtnodeCur.strQName() == CIDUPnP_MediaServices::strVar_TransSpeed)
                pstrName = &CIDUPnP_MediaServices::strVar_TransSpeed;
            else if (xtnodeCur.strQName() == CIDUPnP_MediaServices::strVar_TransState)
                pstrName = &CIDUPnP_MediaServices::strVar_TransState;
            else if (xtnodeCur.strQName() == CIDUPnP_MediaServices::strVar_TransStatus)
                pstrName = &CIDUPnP_MediaServices::strVar_TransStatus;

            // If they want us to store a value one, do that
            if (pstrName)
                SetStateVar(*pstrName, xtnodeCur.xtattrNamed(L"val").strValue());
        }
    }

    // Indicate that we have gotten all the evented stuff
    m_bDonePosInfo = kCIDLib::True;
    m_bDoneTransInfo = kCIDLib::True;

    // Bump our parent class' serial number now and mark use ready
    BumpSerialNum();
}


// ---------------------------------------------------------------------------
//  TUPnPAVTransService: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Given a playlist id, load it up to the player's queue. Has to be sent to
//  the group coordinator.
//
tCIDLib::TVoid
TUPnPAVTransService::AddURIToQueue( const   TString&            strURI
                                    , const TString&            strMetaData
                                    ,       tCIDLib::TCard4&    c4QueuedAtInd
                                    ,       tCIDLib::TCard4&    c4NewTrackCnt
                                    , const tCIDLib::TCard4     c4FirstTrInd
                                    , const tCIDLib::TBoolean   bAsNext)
{
    const TString& strCmd = CIDUPnP_MediaServices::strAVCmd_AddURIToQueue;

    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    m_colInParms.objAdd(strURI);
    m_colInParms.objAdd(strMetaData);
    m_colInParms.objPlace(TCardinal(c4FirstTrInd));
    if (bAsNext)
        m_colInParms.objAdd(kCIDUPnP_::strVal_One);
    else
        m_colInParms.objAdd(kCIDUPnP_::strVal_Zero);
    InvokeAction(strCmd, m_colInParms, m_colOutParms);

    CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 3);

    if (!m_colOutParms[0].bToCard4(c4QueuedAtInd, tCIDLib::ERadices::Dec))
        ThrowBadOutActParms(strCmd, CID_LINE);

    if (!m_colOutParms[2].bToCard4(c4NewTrackCnt, tCIDLib::ERadices::Dec))
        ThrowBadOutActParms(strCmd, CID_LINE);
}


// Get/set the cross fade mode
tCIDLib::TBoolean TUPnPAVTransService::bCrossfadeMode() const
{
    // If evented, check for the state variable. If not set, we need to query
    m_colOutParms.RemoveAll();
    tCIDLib::TBoolean bDoQ = kCIDLib::True;
    if (bIsEvented())
    {
        TString strVal;
        bDoQ = !bQueryStateVar(CIDUPnP_MediaServices::strVar_CrossfadeMode, strVal);

        // If no need to query, then we have the variable and just push that into the list
        if (!bDoQ)
            m_colOutParms.objAdd(strVal);
    }

    // If we need to do an active query, then do that
    if (bDoQ)
    {
        m_colInParms.RemoveAll();
        m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
        InvokeAction
        (
            CIDUPnP_MediaServices::strAVCmd_GetCrossfadeMode, m_colInParms, m_colOutParms
        );

        // If we got a valid answer, force this into the state variable
        if (m_colOutParms[0] != kCIDUPnP_::strVal_NotImpl)
        {
            SetStateVar(CIDUPnP_MediaServices::strVar_CrossfadeMode, m_colOutParms[0]);
            BumpSerialNum();
        }
    }

    //
    //  This is totally inconsistent, with the evented value returning True/False and
    //  the active query returning 0/1, so we need to check box.
    //
    if (m_colOutParms[0].bCompareI(kCIDUPnP_::strVal_One)
    ||  m_colOutParms[0].bCompareI(kCIDUPnP_::strVal_True))
    {
        return kCIDLib::True;
    }
    return kCIDLib::False;
}

tCIDLib::TBoolean
TUPnPAVTransService::bCrossfadeMode(const tCIDLib::TBoolean bToSet)
{
    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    m_colInParms.objAdd(bToSet ? kCIDUPnP_::strVal_One : kCIDUPnP_::strVal_Zero);
    InvokeAction
    (
        CIDUPnP_MediaServices::strAVCmd_SetCrossfadeMode, m_colInParms, m_colOutParms
    );

    return bToSet;
}


// Exit any current group and go back to being a standalone group
tCIDLib::TVoid TUPnPAVTransService::BecomeStandaloneGrp()
{
    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    InvokeAction(CIDUPnP_MediaServices::strAVCmd_BecomeLocalGroup, m_colInParms, m_colOutParms);
}


// Clear all tracks from the player's queue
tCIDLib::TVoid TUPnPAVTransService::ClearQueue()
{
    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    InvokeAction(CIDUPnP_MediaServices::strAVCmd_ClearQueue, m_colInParms, m_colOutParms);
}


//
//  Return the currently stored play mode if we have it. So if it is evented or it has
//  been queried.
//
tCIDUPnP::EPlayModes TUPnPAVTransService::ePlayMode() const
{
    const TString& strCmd = CIDUPnP_MediaServices::strAVCmd_QueryTransSet;

    tCIDUPnP::EPlayModes eRet = tCIDUPnP::EPlayModes::UNKNOWN;
    TString strVal;
    if (bQueryStateVar(CIDUPnP_MediaServices::strVar_CurPlayMode, strVal))
    {
        // We have it so convert to enum format
        eRet = tCIDUPnP::eAltXlatEPlayModes(strVal);
    }
     else
    {
        // Not current stored so query it
        m_colInParms.RemoveAll();
        m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
        InvokeAction(strCmd, m_colInParms, m_colOutParms);
        CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 2);

        // And force that to enum format
        eRet = tCIDUPnP::eAltXlatEPlayModes(m_colOutParms[0]);

        // Force the value into a state variable
        SetStateVar(CIDUPnP_MediaServices::strVar_CurPlayMode, m_colOutParms[0]);
    }

    // If we couldn't translate it, set to unknown
    if (eRet == tCIDUPnP::EPlayModes::Count)
        eRet = tCIDUPnP::EPlayModes::UNKNOWN;

    return eRet;
}


// Move to the next available media
tCIDLib::TVoid TUPnPAVTransService::Next()
{
    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    InvokeAction(CIDUPnP_MediaServices::strAVCmd_Next, m_colInParms, m_colOutParms);
}


// Pause the media playback
tCIDLib::TVoid TUPnPAVTransService::Pause()
{
    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    InvokeAction(CIDUPnP_MediaServices::strAVCmd_Pause, m_colInParms, m_colOutParms);
}


// Start the media playback
tCIDLib::TVoid TUPnPAVTransService::Play()
{
    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    m_colInParms.objAdd(kCIDUPnP_::strVal_One);
    InvokeAction(CIDUPnP_MediaServices::strAVCmd_Play, m_colInParms, m_colOutParms);
}


// Move to the previous available media
tCIDLib::TVoid TUPnPAVTransService::Previous()
{
    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    InvokeAction(CIDUPnP_MediaServices::strAVCmd_Previous, m_colInParms, m_colOutParms);
}


//
//  Get the current AV Transport URI. This is only available via the evented
//  XML, so we return what's available or empty value.
//
tCIDLib::TVoid TUPnPAVTransService::QueryAVTransportURI(TString& strToFill)
{
    if (!bQueryStateVar(CIDUPnP_MediaServices::strVar_AVTransURI, strToFill))
        strToFill.Clear();
}


//
//  Query current position info. We have two versions. One can provide info that is all
//  evented. The other requires that we actually make the full call even if evented.
//
tCIDLib::TVoid
TUPnPAVTransService::QueryPositionInfo( TString&                    strTrackURI
                                        , tCIDLib::TEncodedTime&    enctDuration
                                        , tCIDLib::TCard4&          c4TrackNum
                                        , TUPnPTrackItem* const     pupnptiToFill)
{
    const TString& strCmd = CIDUPnP_MediaServices::strAVCmd_QueryPosInfo;

    // If not evented, or we haven't stored pos info yet, then do a query
    if (!bIsEvented() || !m_bDonePosInfo)
    {
        m_colInParms.RemoveAll();
        m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
        InvokeAction(strCmd, m_colInParms, m_colOutParms);
        CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 8);

        // If evented, we can forced these values into variables and remember we have them
        if (bIsEvented())
        {
            m_bDonePosInfo = kCIDLib::True;
            SetStateVar(CIDUPnP_MediaServices::strVar_CurTrackNum, m_colOutParms[0]);
            SetStateVar(CIDUPnP_MediaServices::strVar_CurTrackDur, m_colOutParms[1]);
            SetStateVar(CIDUPnP_MediaServices::strVar_CurrentTrackMetaData, m_colOutParms[2]);
            SetStateVar(CIDUPnP_MediaServices::strVar_CurTrackURI, m_colOutParms[3]);

            // Bump the serial number
            BumpSerialNum();
        }
    }

    //
    //  If we have done the pos info, fake the stored values into the parameters.
    //  This is a little redundant the first time, but not worth the complications
    //  to avoid.
    //
    if (m_bDonePosInfo)
    {
        m_colOutParms.RemoveAll();
        QueryStateVar(CIDUPnP_MediaServices::strVar_CurTrackNum, m_colOutParms);
        QueryStateVar(CIDUPnP_MediaServices::strVar_CurTrackDur, m_colOutParms);
        QueryStateVar(CIDUPnP_MediaServices::strVar_CurrentTrackMetaData, m_colOutParms);
        QueryStateVar(CIDUPnP_MediaServices::strVar_CurTrackURI, m_colOutParms);
        m_colOutParms.objAdd(TString::strEmpty());
        m_colOutParms.objAdd(TString::strEmpty());
        m_colOutParms.objAdd(TString::strEmpty());
        m_colOutParms.objAdd(TString::strEmpty());
    }

    // Get the track URI. If not implemented return empty string
    if (m_colOutParms[3] == kCIDUPnP_::strVal_NotImpl)
        strTrackURI.Clear();
    else
        strTrackURI = m_colOutParms[3];

    //
    //  Do the track duration. If not implemented return zero. It's also sometimes
    //  empty, in which case we return zero as well. If we can't parse it, we
    //  also just set it to zero.
    //
    if ((m_colOutParms[1] == kCIDUPnP_::strVal_NotImpl)
    ||  m_colOutParms[1].bIsEmpty()
    ||  !bParseTimeVal(m_colOutParms[1], enctDuration, strCmd, CID_LINE))
    {
        enctDuration = 0;
    }

    // If track number is implemented or invalid, set it to zero
    if ((m_colOutParms[0] == kCIDUPnP_::strVal_NotImpl)
    ||  !m_colOutParms[0].bToCard4(c4TrackNum, tCIDLib::ERadices::Dec))
    {
        c4TrackNum = 0;
    }

    // If we got metadata and they want it, then parse that
    if (pupnptiToFill)
        ParseMetaData(m_colOutParms[2], *pupnptiToFill);
}


tCIDLib::TVoid
TUPnPAVTransService::QueryPositionInfo( TString&                    strTrackURI
                                        , tCIDLib::TEncodedTime&    enctDuration
                                        , tCIDLib::TCard4&          c4TrackNum
                                        , tCIDLib::TEncodedTime&    enctCurTime
                                        , tCIDLib::TCard4&          c4PerComplete
                                        , TUPnPTrackItem* const     pupnptiToFill)
{
    const TString& strCmd = CIDUPnP_MediaServices::strAVCmd_QueryPosInfo;

    //
    //  For this version we just do the call since we have to get the current
    //  time and this is the only way to get it. So we've already paid the cost
    //  of getting the other stuff. We'll still update our state variables if
    //  we are evented though.
    //
    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    InvokeAction(strCmd, m_colInParms, m_colOutParms);
    CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 8);

    //
    //  If evented and we've not stored pos info yet, then we can at least optimized and
    //  store that data so that it's available in the first version above without
    //  another query.
    //
    if (bIsEvented() && !m_bDonePosInfo)
    {
        m_bDonePosInfo = kCIDLib::True;
        SetStateVar(CIDUPnP_MediaServices::strVar_CurTrackNum, m_colOutParms[0]);
        SetStateVar(CIDUPnP_MediaServices::strVar_CurTrackDur, m_colOutParms[1]);
        SetStateVar(CIDUPnP_MediaServices::strVar_CurrentTrackMetaData, m_colOutParms[2]);
        SetStateVar(CIDUPnP_MediaServices::strVar_CurTrackURI, m_colOutParms[3]);

        // Bump the serial number
        BumpSerialNum();
    }

    // Get the track URI. If not implemented return empty string
    if (m_colOutParms[3] == kCIDUPnP_::strVal_NotImpl)
        strTrackURI.Clear();
    else
        strTrackURI = m_colOutParms[3];

    //
    //  Do the track duration. If not implemented return zero. Sometimes it
    //  returns an empty value also. If it seems ok but we can't parse it,
    //  then also return zero.
    //
    if ((m_colOutParms[1] == kCIDUPnP_::strVal_NotImpl)
    ||  m_colOutParms[1].bIsEmpty()
    ||  !bParseTimeVal(m_colOutParms[1], enctDuration, strCmd, CID_LINE))
    {
        enctDuration = 0;
    }


    // If track number is implemented, set it to zero
    if (m_colOutParms[0] == kCIDUPnP_::strVal_NotImpl)
        c4TrackNum = 0;
    else if (!m_colOutParms[0].bToCard4(c4TrackNum, tCIDLib::ERadices::Dec))
        ThrowBadOutActParms(strCmd, CID_LINE);

    //
    //  Get the end time if available. If it shows end of media, just set it to the
    //  duration time.
    //
    if (m_colOutParms[4] == CIDUPnP_MediaServices::strVal_EOM)
    {
        enctCurTime = enctDuration;
    }
     else if ((m_colOutParms[4] == kCIDUPnP_::strVal_NotImpl)
          ||  !bParseTimeVal(m_colOutParms[4], enctCurTime, strCmd, CID_LINE))
    {
        enctCurTime = 0;
    }

    //
    //  Calculate the percent complete, watch for possible divide by zero. If
    //  there is no duration, then there's no media loaded so we set the percent
    //  complete to 0.
    //
    if (!enctDuration)
    {
        c4PerComplete = 0;
    }
     else
    {
        c4PerComplete = tCIDLib::TCard4
        (
            (tCIDLib::TFloat8(enctCurTime) / tCIDLib::TFloat8(enctDuration))
            * 100.0
        );
        if (c4PerComplete > 100)
            c4PerComplete = 100;
    }

    // If we got metadata, and they want it, then parse that
    if (pupnptiToFill)
        ParseMetaData(m_colOutParms[2], *pupnptiToFill);
}


// Query transport status info
tCIDLib::TVoid
TUPnPAVTransService::QueryTransportInfo(tCIDUPnP::ETransStates& eState
                                        , TString&              strStatus
                                        , tCIDLib::TCard4&      c4Speed)
{
    const TString& strCmd = CIDUPnP_MediaServices::strAVCmd_QueryTransInfo;

    // If not evented or we haven't loaded this yet, do then query it
    if (!bIsEvented() || !m_bDoneTransInfo)
    {
        m_colInParms.RemoveAll();
        m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
        InvokeAction(strCmd, m_colInParms, m_colOutParms);
        CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 3);

        // If evented, remember we've done it and force into variables
        if (bIsEvented())
        {
            m_bDoneTransInfo = kCIDLib::True;
            SetStateVar(CIDUPnP_MediaServices::strVar_TransState, m_colOutParms[0]);
            SetStateVar(CIDUPnP_MediaServices::strVar_TransStatus, m_colOutParms[1]);
            SetStateVar(CIDUPnP_MediaServices::strVar_TransSpeed, m_colOutParms[2]);

            // Bump the serial number
            BumpSerialNum();
        }
    }

    // If evented, fake current variables into the output parms
    if (bIsEvented())
    {
        m_colOutParms.RemoveAll();
        QueryStateVar(CIDUPnP_MediaServices::strVar_TransState, m_colOutParms);
        QueryStateVar(CIDUPnP_MediaServices::strVar_TransStatus, m_colOutParms);
        QueryStateVar(CIDUPnP_MediaServices::strVar_TransSpeed, m_colOutParms);
    }

    if (m_colOutParms[0] == kCIDUPnP_::strVal_NotImpl)
    {
        eState = tCIDUPnP::ETransStates::UNKNOWN;
    }
     else
    {
        eState = tCIDUPnP::eAltXlatETransStates(m_colOutParms[0]);
        if (eState == tCIDUPnP::ETransStates::Count)
            eState = tCIDUPnP::ETransStates::UNKNOWN;
    }

    if (m_colOutParms[1] == kCIDUPnP_::strVal_NotImpl)
        strStatus.Clear();
    else
        strStatus = m_colOutParms[1];

    // If speed isn't implemented, just return 1
    if (m_colOutParms[2] == kCIDUPnP_::strVal_NotImpl)
        c4Speed = 1;
    else if (!m_colOutParms[2].bToCard4(c4Speed, tCIDLib::ERadices::Dec))
        ThrowBadOutActParms(strCmd, CID_LINE);
}


//
//  Query the transport settings. These are not evented values, so we have to just
//  query them. Well the play mode is, but the recording quality isn't.
//
tCIDLib::TVoid
TUPnPAVTransService::QueryTransportSettings(tCIDUPnP::EPlayModes&   ePlayMode
                                            , TString&              strRecQuality)
{
    const TString& strCmd = CIDUPnP_MediaServices::strAVCmd_QueryTransSet;

    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    InvokeAction(strCmd, m_colInParms, m_colOutParms);
    CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 2);

    if (m_colOutParms[1] == kCIDUPnP_::strVal_NotImpl)
        strRecQuality.Clear();
    else
        strRecQuality = m_colOutParms[1];

    ePlayMode = tCIDUPnP::eAltXlatEPlayModes(m_colOutParms[0]);
    if (ePlayMode == tCIDUPnP::EPlayModes::Count)
        ePlayMode = tCIDUPnP::EPlayModes::UNKNOWN;
}


// Saves the current queue to a playlist with the indicated name
tCIDLib::TVoid
TUPnPAVTransService::SaveQAsPlaylist(const  TString&    strName
                                    ,       TString&    strAssignedID)
{
    const TString& strCmd = CIDUPnP_MediaServices::strAVCmd_SaveQueue;

    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    m_colInParms.objAdd(strName);
    m_colInParms.objAdd(TString::strEmpty());
    InvokeAction(strCmd, m_colInParms, m_colOutParms);

    CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 1);
    strAssignedID = m_colOutParms[0];
}


//
//  Set a new AV transport URI (which may actually point to media or be
//  some special value to queue media or whatever.) We can take some
//  metadata or not.
//
tCIDLib::TVoid
TUPnPAVTransService::SetAVTransportURI( const   TString&        strURI
                                        , const TString&        strArtist
                                        , const TString&        strTitle
                                        , const TString&        strItemName
                                        , const tCIDLib::TCard4 c4TrackNum
                                        , const tCIDLib::TCard4 c4Year)
{
    // Get a copy of the default metadata and search and replace the tokens
    TString strMeta(CIDUPnP_MediaServices::strDefMeta);
    strMeta.eReplaceToken(strArtist, L'a');
    strMeta.eReplaceToken(strItemName, L't');
    strMeta.eReplaceToken(strTitle, L'l');
    strMeta.eReplaceToken(c4TrackNum, L'n');
    strMeta.eReplaceToken(c4Year, L'd');

    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    m_colInParms.objAdd(strURI);
    m_colInParms.objAdd(strMeta);
    InvokeAction
    (
        CIDUPnP_MediaServices::strAVCmd_SetAVTransURI, m_colInParms, m_colOutParms
    );
}

tCIDLib::TVoid
TUPnPAVTransService::SetAVTransportURI(const TString& strURI, const TString& strMeta)
{
    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    m_colInParms.objAdd(strURI);
    m_colInParms.objAdd(strMeta);
    InvokeAction
    (
        CIDUPnP_MediaServices::strAVCmd_SetAVTransURI, m_colInParms, m_colOutParms
    );
}


// Set the current play mode
tCIDLib::TVoid TUPnPAVTransService::SetPlayMode(const tCIDUPnP::EPlayModes eToSet)
{
    // Get the alt text which is the base name and the upper cased version we need
    TString strPlayMode = tCIDUPnP::strAltXlatEPlayModes(eToSet);

    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    m_colInParms.objAdd(strPlayMode);
    InvokeAction
    (
        CIDUPnP_MediaServices::strAVCmd_SetPlayMode, m_colInParms, m_colOutParms
    );
}


// Load a URI and start it playing
tCIDLib::TVoid
TUPnPAVTransService::StartAutoplay( const   TString&            strURI
                                    , const tCIDLib::TBoolean   bIncLinked
                                    , const tCIDLib::TBoolean   bResetVol
                                    , const tCIDLib::TCard4     c4Volume)
{
    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    m_colInParms.objAdd(strURI);
    m_colInParms.objAdd(TString::strEmpty());
    m_colInParms.objPlace(TCardinal(c4Volume));
    m_colInParms.objAdd(bIncLinked ? kCIDUPnP_::strVal_One : kCIDUPnP_::strVal_Zero);
    m_colInParms.objAdd(bResetVol ? kCIDUPnP_::strVal_One : kCIDUPnP_::strVal_Zero);
    InvokeAction
    (
        CIDUPnP_MediaServices::strAVCmd_StartAutoPlay, m_colInParms, m_colOutParms
    );
}


// Stop media playback
tCIDLib::TVoid TUPnPAVTransService::Stop()
{
    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    InvokeAction
    (
        CIDUPnP_MediaServices::strAVCmd_Stop, m_colInParms, m_colOutParms
    );
}


// ---------------------------------------------------------------------------
//  TUPnPAVTransService: Protected, inherited methods
// ---------------------------------------------------------------------------

// When the service is being set up or reset, we can reset stuff
tCIDLib::TVoid TUPnPAVTransService::SvcReset()
{
    m_bDonePosInfo = kCIDLib::False;
    m_bDoneTransInfo = kCIDLib::False;
}



// ---------------------------------------------------------------------------
//  TUPnPAVTransService: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We get some time fields in the format HH:MM:SS, which we have to parse
//  into time objects.
//
tCIDLib::TBoolean
TUPnPAVTransService::bParseTimeVal( const   TString&                strTmVal
                                    ,       tCIDLib::TEncodedTime&  enctToSet
                                    , const TString&                strCmdName
                                    , const tCIDLib::TCard4         c4Line
                                    , const tCIDLib::TBoolean       bThrowIfBad) const
{
    // If not implemented set it to zero
    if (strTmVal == kCIDUPnP_::strVal_NotImpl)
    {
        enctToSet = 0;
        return kCIDLib::False;
    }

    try
    {
        TString strHrs(strTmVal);
        TString strMins;
        TString strSecs;
        if (!strHrs.bSplit(strMins, kCIDLib::chColon)
        ||  !strMins.bSplit(strSecs, kCIDLib::chColon))
        {
            ThrowBadOutActParms(strCmdName, c4Line);
        }

        // Toss any fractional seconds
        tCIDLib::TCard4 c4At;
        if (strSecs.bLastOccurrence(kCIDLib::chPeriod, c4At))
            strSecs.CapAt(c4At);

        tCIDLib::TCard4 c4Hours;
        tCIDLib::TCard4 c4Mins;
        tCIDLib::TCard4 c4Secs;

        if (!strHrs.bToCard4(c4Hours, tCIDLib::ERadices::Dec)
        ||  !strMins.bToCard4(c4Mins, tCIDLib::ERadices::Dec)
        ||  !strSecs.bToCard4(c4Secs, tCIDLib::ERadices::Dec))
        {
            ThrowBadOutActParms(strCmdName, CID_LINE);
        }

        enctToSet = (c4Hours * kCIDLib::enctOneHour)
                    + (c4Mins * kCIDLib::enctOneMinute)
                    + (c4Secs * kCIDLib::enctOneSecond);
    }

    catch(TError& errToCatch)
    {
        if (bLogErrs())
        {
            errToCatch.AddStackLevel(CID_FILE, c4Line);
            TModule::LogEventObj(errToCatch);
        }

        if (bThrowIfBad)
            throw;
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



//
//  Called to parse out the current metadata info that we get in the evented
//  data. Sometimes there's nothing there of interest. We currently just try
//  to get the current track and artist name.
//
tCIDLib::TVoid
TUPnPAVTransService::ParseMetaData( const   TString&        strData
                                    ,       TUPnPTrackItem& upnptiToFill)
{
    // If empty, do nothing, but clear out the track item
    if (strData.bIsEmpty())
    {
        upnptiToFill.Reset();
        return;
    }

    // Otherwise let's try to parse it
    TXMLTreeParser xtprsMeta;
    if (!bParseXML(xtprsMeta, strData, L"AVTrans metadata"))
    {
        upnptiToFill.Reset();
        return;
    }

    try
    {
        const TXMLTreeElement& xtnodeRoot = xtprsMeta.xtdocThis().xtnodeRoot();

        //
        //  The root should be the DIDL node, then under that the item node.
        //  The track item knows how to parse itself out of that.
        //
        const TXMLTreeElement& xtnodeItem = xtnodeRoot.xtnodeChildAtAsElement(0);
        if (xtnodeItem.strQName() == CIDUPnP_MediaServices::strXML_Item)
            upnptiToFill.SetFromXML(xtnodeItem);
    }

    catch(TError& errToCatch)
    {
        if (bLogErrs())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
        upnptiToFill.Reset();
    }
}




// ---------------------------------------------------------------------------
//  CLASS: TUPnPContDirService
// PREFIX: upnps
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TUPnPContDirService: Constructors and Destructor
// ---------------------------------------------------------------------------
TUPnPContDirService::TUPnPContDirService()
{
}

TUPnPContDirService::~TUPnPContDirService()
{
}


// ---------------------------------------------------------------------------
//  TUPnPContDirService: Public, inherited methods
// ---------------------------------------------------------------------------

// We only accept content directory types
tCIDLib::TBoolean
TUPnPContDirService::bCheckServiceType(const TString& strTypeID) const
{
    return (strTypeID == kCIDUPnP::strSvcType_ContentDirectory);
}


//
//  We override this in order to special case the container update ids
//  variable. We want to just append to it, and we'll ask to have it
//  cleared when read from the evented variable list. This is necessary
//  to avoid either missing or redundantly processing changes.
//
tCIDLib::TVoid
TUPnPContDirService::UPnPVarChanged(const   tCIDLib::TCh* const pszVarName
                                    , const tCIDLib::TCh* const pszVarVal)
{
    if (CIDUPnP_MediaServices::strVar_ContUpdateIDs.bCompareI(pszVarName))
    {
        TString strRealVal;
        bQueryStateVar(pszVarName, strRealVal);
        if (!strRealVal.bIsEmpty())
            strRealVal.Append(kCIDLib::chComma);
        strRealVal.Append(pszVarVal);

        SetStateVar(pszVarName, strRealVal);
        BumpSerialNum();
        return;
    }

    // Just pass it on for regular handling
    TParent::UPnPVarChanged(pszVarName, pszVarVal);
}


// ---------------------------------------------------------------------------
//  TUPnPContDirService: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Browse the favorites container. We have two versions.
//
//  In the first we get back a list of TKeyValues. The key is the title. The first value
//  is the protocol info (what we need to write back to the transport URI to play it,
//  and the second value is the metadata we need to pass back in to play it.
//
//  In the other version we just get back a list of names.
//
tCIDLib::TCard4
TUPnPContDirService::c4BrowseFavorites( const   tCIDLib::TCard4         c4StartAt
                                        , const tCIDLib::TCard4         c4ToGet
                                        ,       tCIDLib::TCard4&        c4Total
                                        ,       tCIDLib::TKValsCollect& colToFill
                                        ,       tCIDLib::TInt4&         i4UpdateID
                                        , const tCIDLib::TBoolean       bAppend)
{
    // If not appending to the list, then clear it first
    if (!bAppend)
        colToFill.RemoveAll();

    tCIDLib::TCard4 c4RetCnt = 0;
    DoBrowse(L"FV:2", kCIDLib::True, c4StartAt, c4ToGet, c4RetCnt, c4Total, i4UpdateID);

    TXMLTreeParser xtprsItems;
    if (bParseXML(xtprsItems, m_colOutParms[0], L"favorites list"))
    {
        // Loop through the returned elements and load up the caller's list
        const TXMLTreeElement& xtnodeRoot = xtprsItems.xtdocThis().xtnodeRoot();
        CIDAssert
        (
            c4RetCnt == xtnodeRoot.c4ChildCount()
            , L"Returned list count != actual returned XML elements"
        );

        // Loop through then and fill in the caller's list
        TKeyValues kvalsNew;
        const TXMLTreeElement* pxtnodeMeta;
        const TXMLTreeElement* pxtnodeName;
        const TXMLTreeElement* pxtnodeRes;
        TString strProtoInfo;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4RetCnt; c4Index++)
        {
            const TXMLTreeElement& xtnodeCur = xtnodeRoot.xtnodeChildAtAsElement(c4Index);

            // Look up the child elements we need
            tCIDLib::TCard4 c4ElemInd;
            pxtnodeMeta = xtnodeCur.pxtnodeFindElement
            (
                CIDUPnP_MediaServices::strXML_MetaData, 0, c4ElemInd
            );
            pxtnodeName = xtnodeCur.pxtnodeFindElement
            (
                CIDUPnP_MediaServices::strXML_Title, 0, c4ElemInd
            );
            pxtnodeRes = xtnodeCur.pxtnodeFindElement
            (
                CIDUPnP_MediaServices::strXML_Res, 0, c4ElemInd
            );

            if (pxtnodeName
            &&  pxtnodeMeta
            &&  pxtnodeRes
            &&  pxtnodeName->c4ChildCount()
            &&  pxtnodeRes->c4ChildCount())
            {
                // We can live with meta not being set
                const TString& strMeta
                (
                    pxtnodeMeta->c4ChildCount()
                    ? pxtnodeMeta->xtnodeChildAtAsText(0).strText()
                    : TString::strEmpty()
                );

                kvalsNew.Set
                (
                    pxtnodeName->xtnodeChildAtAsText(0).strText()
                    , pxtnodeRes->xtnodeChildAtAsText(0).strText()
                    , strMeta
                );
                colToFill.objAdd(kvalsNew);
            }
        }
    }
    return c4RetCnt;
}


//
//  Browse a container that returns track type info. The caller is responsible
//  for pssing in the appropriate type of container id.
//
tCIDLib::TCard4
TUPnPContDirService::c4BrowseTracks(const   TString&            strContID
                                    , const tCIDLib::TCard4     c4StartAt
                                    , const tCIDLib::TCard4     c4ToGet
                                    ,       tCIDLib::TCard4&    c4Total
                                    ,       TTrackList&         colToFill
                                    ,       tCIDLib::TInt4&     i4UpdateID
                                    , const tCIDLib::TBoolean   bAppend)
{
    tCIDLib::TCard4 c4RetCnt = 0;
    DoBrowse(strContID, kCIDLib::True, c4StartAt, c4ToGet, c4RetCnt, c4Total, i4UpdateID);
    TXMLTreeParser xtprsItems;
    if (bParseXML(xtprsItems, m_colOutParms[0], L"track list"))
    {
        // Loop through the returned elements and load up the caller's list
        const TXMLTreeElement& xtnodeRoot = xtprsItems.xtdocThis().xtnodeRoot();
        CIDAssert
        (
            c4RetCnt == xtnodeRoot.c4ChildCount()
            , L"Returned track count != actual returned XML elements"
        );

        if (!bAppend)
            colToFill.RemoveAll();

        TUPnPTrackItem upnptiCur;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4RetCnt; c4Index++)
        {
            upnptiCur.SetFromXML(xtnodeRoot.xtnodeChildAtAsElement(c4Index));
            colToFill.objAdd(upnptiCur);
        }
    }
    return c4RetCnt;
}


//
//  Browse a lists type of container, which retursn key/value pairs, which
//  are the names and the ids of the things browsed.
//
tCIDLib::TCard4
TUPnPContDirService::c4BrowseLists( const   TString&                strContID
                                    , const tCIDLib::TCard4         c4StartAt
                                    , const tCIDLib::TCard4         c4ToGet
                                    ,       tCIDLib::TCard4&        c4Total
                                    ,       tCIDLib::TKVPCollect&   colToFill
                                    ,       tCIDLib::TInt4&         i4UpdateID
                                    , const tCIDLib::TBoolean       bAppend)
{
    // If not appending to the list, then clear it first
    if (!bAppend)
        colToFill.RemoveAll();

    tCIDLib::TCard4 c4RetCnt = 0;
    DoBrowse(strContID, kCIDLib::True, c4StartAt, c4ToGet, c4RetCnt, c4Total, i4UpdateID);
    TXMLTreeParser xtprsItems;
    if (bParseXML(xtprsItems, m_colOutParms[0], L"playlists list"))
    {
        // Loop through the returned elements and load up the caller's list
        const TXMLTreeElement& xtnodeRoot = xtprsItems.xtdocThis().xtnodeRoot();
        CIDAssert
        (
            c4RetCnt == xtnodeRoot.c4ChildCount()
            , L"Returned list count != actual returned XML elements"
        );

        // Loop through then and fill in the caller's list
        TKeyValuePair kvalNew;
        const TXMLTreeElement* pxtnodeName;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4RetCnt; c4Index++)
        {
            const TXMLTreeElement& xtnodeCur = xtnodeRoot.xtnodeChildAtAsElement(c4Index);

            // See if the id attribute is present
            tCIDLib::TCard4 c4AttrInd;
            if (xtnodeCur.bAttrExists(CIDUPnP_MediaServices::strXML_ID, c4AttrInd))
            {
                // It is so see if the title child element is present
                tCIDLib::TCard4 c4ElemInd;
                pxtnodeName = xtnodeCur.pxtnodeFindElement
                (
                    CIDUPnP_MediaServices::strXML_Title, 0, c4ElemInd
                );

                if (pxtnodeName)
                {
                    kvalNew.Set
                    (
                        xtnodeCur.xtattrAt(c4AttrInd).strValue()
                        , pxtnodeName->xtnodeChildAtAsText(0).strText()
                    );
                    colToFill.objAdd(kvalNew);
                }
            }
        }
    }
    return c4RetCnt;
}


//
//  Similar to above but this one returns the raw DIDL XML to the caller who
//  can parse it for whatever info he wants. In this case we let him tell us if
//  he wants to get children or metadata, since it's all the same to use, just
//  a blob of XML.
//
tCIDLib::TCard4
TUPnPContDirService::c4Browse(  const   TString&            strContID
                                , const tCIDLib::TBoolean   bChildren
                                , const tCIDLib::TCard4     c4StartAt
                                , const tCIDLib::TCard4     c4ToGet
                                ,       tCIDLib::TCard4&    c4Total
                                ,       tCIDLib::TInt4&     i4UpdateID
                                ,       TString&            strDIDL)
{
    tCIDLib::TCard4 c4RetCnt = 0;
    DoBrowse(strContID, bChildren, c4StartAt, c4ToGet, c4RetCnt, c4Total, i4UpdateID);
    strDIDL = m_colOutParms[0];
    return c4RetCnt;
}


// Delete an object by its id
tCIDLib::TVoid
TUPnPContDirService::DeleteObject(const TString& strObjectID)
{
    m_colInParms.RemoveAll();
    m_colInParms.objAdd(strObjectID);
    InvokeAction
    (
        CIDUPnP_MediaServices::strCDCmd_DestroyObject, m_colInParms, m_colOutParms
    );
}


//
//  Query the list of container update ids that have changed since we
//  last checked. This is one where we read and empty it when we read it.
//  And we also override the change handler above and allow the values
//  to accumulate until this is called to read it again. This one can't
//  be queried, it's only evented. So we call the version that just cheks
//  the list and if it says it's not there, we return an empty value.
//
tCIDLib::TVoid TUPnPContDirService::QueryContUpdateID(TString& strToFill)
{
    if (!bQueryStateVar(CIDUPnP_MediaServices::strVar_ContUpdateIDs
                        , strToFill
                        , kCIDLib::True))
    {
        strToFill.Clear();
    }
}



// ---------------------------------------------------------------------------
//  TUPnPContDirService: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Do the basic parse operation and return the info. We leave the XML text
//  in the 0th parameter. It might be large so it makes no sense to try to
//  make a copy of it to give back.
//
tCIDLib::TVoid
TUPnPContDirService::DoBrowse(  const   TString&            strContID
                                , const tCIDLib::TBoolean   bChildren
                                , const tCIDLib::TCard4     c4StartAt
                                , const tCIDLib::TCard4     c4ToGet
                                ,       tCIDLib::TCard4&    c4RetCnt
                                ,       tCIDLib::TCard4&    c4Total
                                ,       tCIDLib::TInt4&     i4UpdateID)
{
    const TString& strCmd = CIDUPnP_MediaServices::strCDCmd_Browse;

    m_colInParms.RemoveAll();
    m_colInParms.objAdd(strContID);
    if (bChildren)
        m_colInParms.objAdd(L"BrowseDirectChildren");
    else
        m_colInParms.objAdd(L"BrowseMetadata");
    m_colInParms.objAdd(L"*");
    m_colInParms.objPlace(TCardinal(c4StartAt));
    m_colInParms.objPlace(TCardinal(c4ToGet));
    m_colInParms.objAdd(L"*");
    InvokeAction(strCmd, m_colInParms, m_colOutParms);

    CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 4);

    if (!m_colOutParms[1].bToCard4(c4RetCnt, tCIDLib::ERadices::Dec))
        ThrowBadOutActParms(strCmd, CID_LINE);

    if (!m_colOutParms[2].bToCard4(c4Total, tCIDLib::ERadices::Dec))
        ThrowBadOutActParms(strCmd, CID_LINE);

    if (!m_colOutParms[3].bToInt4(i4UpdateID, tCIDLib::ERadices::Dec))
        ThrowBadOutActParms(strCmd, CID_LINE);
}




// ---------------------------------------------------------------------------
//  CLASS: TUPnPRendCtrlService
// PREFIX: upnps
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TUPnPRendCtrlService: Constructors and Destructor
// ---------------------------------------------------------------------------
TUPnPRendCtrlService::TUPnPRendCtrlService() :

    m_bGotMute(kCIDLib::False)
    , m_bGotVolume(kCIDLib::False)
{
}

TUPnPRendCtrlService::~TUPnPRendCtrlService()
{
}


// ---------------------------------------------------------------------------
//  TUPnPRendCtrlService: Public, inherited methods
// ---------------------------------------------------------------------------

// We only accept AV transport types
tCIDLib::TBoolean
TUPnPRendCtrlService::bCheckServiceType(const TString& strTypeID) const
{
    return (strTypeID == kCIDUPnP::strSvcType_RendControl);
}


//
//  We override this one so that we can watch for our last change evented
//  variable, which is one of those composite ones transmitted as XML. So we
//  don't want the underlying service base class to see this one. We'll eat
//  it and parse the content and store it. Others we pass on.
//
//  What we do is call our parent version with the broken out values, so that
//  he will store them as normal cached evented state variable values that
//  will be seen later when we are queried.
//
tCIDLib::TVoid
TUPnPRendCtrlService::UPnPVarChanged(const  tCIDLib::TCh* const pszVarName
                                    , const tCIDLib::TCh* const pszVarVal)
{
    // If not our special case, let the parent class handle it
    if (!CIDUPnP_MediaServices::strVal_LastChange.bCompareI(pszVarName))
    {
        TParent::UPnPVarChanged(pszVarName, pszVarVal);
        return;
    }

    TXMLTreeParser xtprsItems;
    if (bParseXML(xtprsItems, TString(pszVarVal), L"rend control lastchage"))
    {
        // Get the root node, and then the InstanceID child, which is the first one
        const TXMLTreeElement& xtnodeRoot = xtprsItems.xtdocThis().xtnodeRoot();
        const TXMLTreeElement& xtnodeInst = xtnodeRoot.xtnodeChildAtAsElement(0);

        const tCIDLib::TCard4 c4Count = xtnodeInst.c4ChildCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TXMLTreeElement& xtnodeCur = xtnodeInst.xtnodeChildAtAsElement(c4Index);
            if ((xtnodeCur.strQName() == CIDUPnP_MediaServices::strVar_Volume)
            &&  (xtnodeCur.xtattrNamed(L"channel").strValue() == CIDUPnP_MediaServices::strXML_Master))
            {
                SetStateVar
                (
                    CIDUPnP_MediaServices::strVar_Volume
                    , xtnodeCur.xtattrNamed(L"val").strValue()
                );
                m_bGotVolume = kCIDLib::True;
            }
             else if ((xtnodeCur.strQName() == CIDUPnP_MediaServices::strVar_Mute)
                  &&  (xtnodeCur.xtattrNamed(L"channel").strValue() == CIDUPnP_MediaServices::strXML_Master))
            {
                // We have to pass true or false as the value
                SetStateVar
                (
                    CIDUPnP_MediaServices::strVar_Mute
                    , (xtnodeCur.xtattrNamed(L"val").strValue() == kCIDUPnP_::strVal_Zero)
                         ? kCIDUPnP_::strVal_False : kCIDUPnP_::strVal_True
                );
                m_bGotMute = kCIDLib::True;
            }
        }

        // Bump our parent class' serial number
        BumpSerialNum();
    }
}


// ---------------------------------------------------------------------------
//  TUPnPRendCtrlService: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get the current mute state
tCIDLib::TBoolean TUPnPRendCtrlService::bQueryMute()
{
    const TString& strCmd = CIDUPnP_MediaServices::strRCCmd_QueryMute;

    // If not evented or we haven't gotten mute yet, then query it
    if (!bIsEvented() || !m_bGotMute)
    {
        m_colInParms.RemoveAll();
        m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
        m_colInParms.objAdd(kCIDUPnP_::strVal_Master);
        InvokeAction(strCmd, m_colInParms, m_colOutParms);
        CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 1);

        // If evented, then store the state variable
        if (bIsEvented())
        {
            m_bGotMute = kCIDLib::True;
            SetStateVar(CIDUPnP_MediaServices::strVar_Mute, m_colOutParms[0]);
            BumpSerialNum();
        }
    }

    // If we have the values stored, then fake them into the output parameters
    if (m_bGotMute)
    {
        m_colOutParms.RemoveAll();
        QueryStateVar(CIDUPnP_MediaServices::strVar_Mute, m_colOutParms);
    }


    // Return the appropriate value
    tCIDLib::TBoolean bRet = kCIDLib::False;
    if (m_colOutParms[0] == kCIDUPnP_::strVal_False)
        bRet = kCIDLib::False;
    else if (m_colOutParms[0] == kCIDUPnP_::strVal_True)
        bRet = kCIDLib::True;
    else if (m_colOutParms[0] == kCIDUPnP_::strVal_NotImpl)
        bRet = kCIDLib::False;
    else
        ThrowBadOutActParms(strCmd, CID_LINE);

    return bRet;
}


// Query the current non-dB based volume of the renderer
tCIDLib::TCard4 TUPnPRendCtrlService::c4QueryVolume()
{
    const TString& strCmd = CIDUPnP_MediaServices::strRCCmd_QueryVolume;

    // If not evented or we haven't gotten volume yet, then query it
    if (!bIsEvented() || !m_bGotVolume)
    {
        m_colInParms.RemoveAll();
        m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
        m_colInParms.objAdd(kCIDUPnP_::strVal_Master);
        InvokeAction(strCmd, m_colInParms, m_colOutParms);
        CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 1);

        // If evented, store to state vars
        if (bIsEvented())
        {
            m_bGotVolume = kCIDLib::True;
            SetStateVar(CIDUPnP_MediaServices::strVar_Volume, m_colOutParms[0]);
            BumpSerialNum();
        }
    }

    // If we have stored values, fake them into the output parameters
    if (m_bGotVolume)
    {
        m_colOutParms.RemoveAll();
        QueryStateVar(CIDUPnP_MediaServices::strVar_Volume, m_colOutParms);
    }

    // And now convert the value appropriately
    tCIDLib::TCard4 c4Ret;
    if (m_colOutParms[0] == kCIDUPnP_::strVal_NotImpl)
        c4Ret = 0;
    else if (!m_colOutParms[0].bToCard4(c4Ret, tCIDLib::ERadices::Dec))
        ThrowBadOutActParms(strCmd, CID_LINE);

    return c4Ret;
}



// Query the current dB volume of the renderer
tCIDLib::TInt4 TUPnPRendCtrlService::i4QueryVolumeDB()
{
    const TString& strCmd = CIDUPnP_MediaServices::strRCCmd_QueryVolumeDB;

    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    m_colInParms.objAdd(kCIDUPnP_::strVal_Master);
    InvokeAction(strCmd, m_colInParms, m_colOutParms);
    CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 1);

    tCIDLib::TInt4 i4Ret;
    if (m_colOutParms[0] == kCIDUPnP_::strVal_NotImpl)
        i4Ret = 0;
    else if (!m_colOutParms[0].bToInt4(i4Ret, tCIDLib::ERadices::Dec))
        ThrowBadOutActParms(strCmd, CID_LINE);

    return i4Ret;
}


//
//  Query the volume range, which is device dependent, so any client code must
//  adapt to this range.
//
tCIDLib::TVoid
TUPnPRendCtrlService::QueryVolDBRange(  tCIDLib::TInt4&     i4MinVol
                                        , tCIDLib::TInt4&   i4MaxVol)
{
    const TString& strCmd = CIDUPnP_MediaServices::strRCCmd_QueryVolRange;

    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    m_colInParms.objAdd(kCIDUPnP_::strVal_Master);
    InvokeAction(strCmd, m_colInParms, m_colOutParms);
    CheckRetParmCnt(strCmd, m_colOutParms.c4ElemCount(), 2);

    // Get the parms out and convert them
    if (!m_colOutParms[0].bToInt4(i4MinVol, tCIDLib::ERadices::Dec)
    ||  !m_colOutParms[1].bToInt4(i4MaxVol, tCIDLib::ERadices::Dec))
    {
        ThrowBadOutActParms(strCmd, CID_LINE);
    }
}


// Turn the mute off/on
tCIDLib::TVoid TUPnPRendCtrlService::SetMute(const tCIDLib::TBoolean bToSet)
{
    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    m_colInParms.objAdd(kCIDUPnP_::strVal_Master);
    if (bToSet)
        m_colInParms.objAdd(kCIDUPnP_::strVal_One);
    else
        m_colInParms.objAdd(kCIDUPnP_::strVal_Zero);

    InvokeAction
    (
        CIDUPnP_MediaServices::strRCCmd_SetMute, m_colInParms, m_colOutParms
    );
}


//
//  Set the volume in the DB range. The new value has to be between the reported
//  min/max DB volume values of the device.
//
tCIDLib::TVoid TUPnPRendCtrlService::SetVolumeDB(const tCIDLib::TInt4 i4ToSet)
{
    TString strVol;
    strVol.SetFormatted(i4ToSet);

    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    m_colInParms.objAdd(kCIDUPnP_::strVal_Master);
    m_colInParms.objAdd(strVol);

    InvokeAction
    (
        CIDUPnP_MediaServices::strRCCmd_SetVolumeDB, m_colInParms, m_colOutParms
    );

}


//
//  Set the volume in the arbitrary player range.
//
tCIDLib::TVoid TUPnPRendCtrlService::SetVolume(const tCIDLib::TCard4 c4ToSet)
{
    TString strVol;
    strVol.SetFormatted(c4ToSet);

    m_colInParms.RemoveAll();
    m_colInParms.objAdd(CIDUPnP_MediaServices::strVal_InstID);
    m_colInParms.objAdd(kCIDUPnP_::strVal_Master);
    m_colInParms.objAdd(strVol);

    InvokeAction
    (
        CIDUPnP_MediaServices::strRCCmd_SetVolume, m_colInParms, m_colOutParms
    );
}



// ---------------------------------------------------------------------------
//  TUPnPRendCtrlService: Protected, inherited methods
// ---------------------------------------------------------------------------

// When the service is being set up or reset, we need to clear some things
tCIDLib::TVoid TUPnPRendCtrlService::SvcReset()
{
    m_bGotMute = kCIDLib::False;
    m_bGotVolume = kCIDLib::False;
}




// ---------------------------------------------------------------------------
//  CLASS: TUPnPTrackItem
// PREFIX: upnpti
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TUPnPTrackItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TUPnPTrackItem::TUPnPTrackItem() :

    m_c4TrackNum(0)
{
}

TUPnPTrackItem::TUPnPTrackItem( const   tCIDLib::TCard4 c4TrackNum
                                , const TString&        strAlbum
                                , const TString&        strArtist
                                , const TString&        strTitle
                                , const TString&        strArtURL) :
    m_c4TrackNum(c4TrackNum)
    , m_strAlbum(strAlbum)
    , m_strArtist(strArtist)
    , m_strArtURL(strArtURL)
    , m_strTitle(strTitle)
{
}

TUPnPTrackItem::TUPnPTrackItem(const TUPnPTrackItem& upnptiToCopy) :

    m_c4TrackNum(upnptiToCopy.m_c4TrackNum)
    , m_strAlbum(upnptiToCopy.m_strAlbum)
    , m_strArtist(upnptiToCopy.m_strArtist)
    , m_strArtURL(upnptiToCopy.m_strArtURL)
    , m_strTitle(upnptiToCopy.m_strTitle)
{
}

TUPnPTrackItem::~TUPnPTrackItem()
{
}


// ---------------------------------------------------------------------------
//  TUPnPTrackItem: Public operators
// ---------------------------------------------------------------------------
TUPnPTrackItem& TUPnPTrackItem::operator=(const TUPnPTrackItem& upnptiToAssign)
{
    if (this != &upnptiToAssign)
    {
        m_c4TrackNum = upnptiToAssign.m_c4TrackNum;
        m_strAlbum   = upnptiToAssign.m_strAlbum;
        m_strArtist  = upnptiToAssign.m_strArtist;
        m_strArtURL  = upnptiToAssign.m_strArtURL;
        m_strTitle   = upnptiToAssign.m_strTitle;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TUPnPTrackItem: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TCard4 TUPnPTrackItem::c4TrackNum() const
{
    return m_c4TrackNum;
}


// Clear all our fields out
tCIDLib::TVoid TUPnPTrackItem::Reset()
{
    m_c4TrackNum = 0;
    m_strAlbum.Clear();
    m_strArtist.Clear();
    m_strArtURL.Clear();
    m_strTitle.Clear();
}


const TString& TUPnPTrackItem::strAlbum() const
{
    return m_strAlbum;
}


const TString& TUPnPTrackItem::strArtist() const
{
    return m_strArtist;
}


const TString& TUPnPTrackItem::strArtURL() const
{
    return m_strArtURL;
}


const TString& TUPnPTrackItem::strTitle() const
{
    return m_strTitle;
}


tCIDLib::TVoid
TUPnPTrackItem::Set(const   tCIDLib::TCard4 c4TrackNum
                    , const TString&        strAlbum
                    , const TString&        strArtist
                    , const TString&        strTitle
                    , const TString&        strArtURL)
{
    m_c4TrackNum = c4TrackNum;
    m_strAlbum   = strAlbum;
    m_strArtist  = strArtist;
    m_strArtURL  = strArtURL;
    m_strTitle   = strTitle;
}


//
//  Normally we will be set from the XML returned from the browse query to
//  the content directory. The content directory service class will pass us
//  each item in turn and we parse out the info.
//
//  WE assume the caller set the parse flags such that unimportant text
//  content is not present.
//
tCIDLib::TVoid TUPnPTrackItem::SetFromXML(const TXMLTreeElement& xtnodeSrc)
{
    CIDAssert
    (
        xtnodeSrc.strQName() == CIDUPnP_MediaServices::strXML_Item
        , L"Expected an item element"
    );

    tCIDLib::TCard4 c4At;
    const TXMLTreeElement* pxtnodeCur;

    pxtnodeCur = xtnodeSrc.pxtnodeFindElement(CIDUPnP_MediaServices::strXML_Title, 0, c4At);
    if (pxtnodeCur)
        m_strTitle = pxtnodeCur->xtnodeChildAtAsText(0).strText();
    else
        m_strTitle = CIDUPnP_MediaServices::strVal_Untitled;


    pxtnodeCur = xtnodeSrc.pxtnodeFindElement(CIDUPnP_MediaServices::strXML_Artist, 0, c4At);
    if (pxtnodeCur)
        m_strArtist = pxtnodeCur->xtnodeChildAtAsText(0).strText();
    else
        m_strArtist.Clear();


    pxtnodeCur = xtnodeSrc.pxtnodeFindElement(CIDUPnP_MediaServices::strXML_Album, 0, c4At);
    if (pxtnodeCur)
        m_strAlbum = pxtnodeCur->xtnodeChildAtAsText(0).strText();
    else
        m_strAlbum.Clear();


    pxtnodeCur = xtnodeSrc.pxtnodeFindElement(CIDUPnP_MediaServices::strXML_ArtURL, 0, c4At);
    if (pxtnodeCur)
        m_strArtURL = pxtnodeCur->xtnodeChildAtAsText(0).strText();
    else
        m_strArtURL.Clear();


    m_c4TrackNum = 0;
    pxtnodeCur = xtnodeSrc.pxtnodeFindElement(CIDUPnP_MediaServices::strXML_TrackNum, 0, c4At);
    if (pxtnodeCur)
    {
        const TString& strNum = pxtnodeCur->xtnodeChildAtAsText(0).strText();
        strNum.bToCard4(m_c4TrackNum, tCIDLib::ERadices::Dec);
    }
}




