// ----------------------------------------------------------------------------
//  FILE: CIDUPnP_Shared.cpp
//  DATE: Fri, Feb 12 21:10:39 2021 -0500
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------

#include "CIDUPnP_.hpp"

const TString kCIDUPnP::strDevType_MediaRenderer(L"urn:schemas-upnp-org:device:MediaRenderer:1");
const TString kCIDUPnP::strDevType_MediaServer(L"urn:schemas-upnp-org:device:MediaServer:1");
const TString kCIDUPnP::strDevType_RootDev(L"upnp:rootdevice");
const TString kCIDUPnP::strDevType_ZonePlayer(L"urn:schemas-upnp-org:device:ZonePlayer:1");
const TString kCIDUPnP::strSvcType_AVTransport(L"urn:schemas-upnp-org:service:AVTransport:1");
const TString kCIDUPnP::strSvcType_AlarmClock(L"urn:schemas-upnp-org:service:AlarmClock:1");
const TString kCIDUPnP::strSvcType_AudioIn(L"urn:schemas-upnp-org:service:AudioIn:1");
const TString kCIDUPnP::strSvcType_ConnectionMgr(L"urn:schemas-upnp-org:service:ConnectionManager:1");
const TString kCIDUPnP::strSvcType_ContentDirectory(L"urn:schemas-upnp-org:service:ContentDirectory:1");
const TString kCIDUPnP::strSvcType_DeviceProperties(L"urn:schemas-upnp-org:service:DeviceProperties:1");
const TString kCIDUPnP::strSvcType_GroupManagement(L"urn:schemas-upnp-org:service:GroupManagement:1");
const TString kCIDUPnP::strSvcType_MusicServices(L"urn:schemas-upnp-org:service:MusicServices:1");
const TString kCIDUPnP::strSvcType_RendControl(L"urn:schemas-upnp-org:service:RenderingControl:1");
const TString kCIDUPnP::strSvcType_ZoneGroupTopology(L"urn:schemas-upnp-org:service:ZoneGroupTopology:1");


static TEnumMap::TEnumValItem aeitemValues_EPlayModes[8] = 
{
    {  tCIDLib::TInt4(tCIDUPnP::EPlayModes::UNKNOWN), 0, 0,  { L"", L"", L"", L"UNKNOWN", L"EPlayModes::UNKNOWN", L"Unknown" } }
  , {  tCIDLib::TInt4(tCIDUPnP::EPlayModes::NORMAL), 0, 0,  { L"", L"", L"", L"NORMAL", L"EPlayModes::NORMAL", L"Normal" } }
  , {  tCIDLib::TInt4(tCIDUPnP::EPlayModes::SHUFFLE), 0, 0,  { L"", L"", L"", L"SHUFFLE", L"EPlayModes::SHUFFLE", L"Shuffle" } }
  , {  tCIDLib::TInt4(tCIDUPnP::EPlayModes::REPEAT), 0, 0,  { L"", L"", L"", L"REPEAT", L"EPlayModes::REPEAT", L"Repeat" } }
  , {  tCIDLib::TInt4(tCIDUPnP::EPlayModes::REPEATONE), 0, 0,  { L"", L"", L"", L"REPEATONE", L"EPlayModes::REPEATONE", L"RepeatOne" } }
  , {  tCIDLib::TInt4(tCIDUPnP::EPlayModes::RANDOM), 0, 0,  { L"", L"", L"", L"RANDOM", L"EPlayModes::RANDOM", L"Random" } }
  , {  tCIDLib::TInt4(tCIDUPnP::EPlayModes::DIRECT), 0, 0,  { L"", L"", L"", L"DIRECT", L"EPlayModes::DIRECT", L"Direct" } }
  , {  tCIDLib::TInt4(tCIDUPnP::EPlayModes::INTRO), 0, 0,  { L"", L"", L"", L"INTRO", L"EPlayModes::INTRO", L"Intro" } }

};

static TEnumMap emapEPlayModes
(
     L"EPlayModes"
     , 8
     , kCIDLib::False
     , aeitemValues_EPlayModes
     , nullptr
     , tCIDLib::TCard4(tCIDUPnP::EPlayModes::Count)
);

const TString& tCIDUPnP::strAltXlatEPlayModes(const tCIDUPnP::EPlayModes eVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return emapEPlayModes.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::BaseName, bThrowIfNot);
}

tCIDUPnP::EPlayModes tCIDUPnP::eAltXlatEPlayModes(const TString& strVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return tCIDUPnP::EPlayModes(emapEPlayModes.i4MapEnumText(strVal, TEnumMap::ETextVals::BaseName, bThrowIfNot));
}

tCIDLib::TVoid tCIDUPnP::FormatEPlayModes(TString& strToFill, const TString& strPrefix, const tCIDLib::TCh chSepChar, const TEnumMap::ETextVals eVal)
{
    emapEPlayModes.FormatValues(strToFill, strPrefix, chSepChar, eVal);
}
const TString& tCIDUPnP::strXlatEPlayModes(const tCIDUPnP::EPlayModes eVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return emapEPlayModes.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::Text, bThrowIfNot);
}

tCIDUPnP::EPlayModes tCIDUPnP::eXlatEPlayModes(const TString& strVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return tCIDUPnP::EPlayModes(emapEPlayModes.i4MapEnumText(strVal, TEnumMap::ETextVals::Text, bThrowIfNot));
}

TTextOutStream& tCIDUPnP::operator<<(TTextOutStream& strmTar, const tCIDUPnP::EPlayModes eVal)
{
    strmTar << emapEPlayModes.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::Text, kCIDLib::False);
    return strmTar;
}
tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDUPnP::EPlayModes* const aeList, const tCIDLib::TCard4 c4Count)
{
    tCIDLib::TCard4 c4Cur;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        strmSrc >> c4Cur;
        aeList[c4Index] = tCIDUPnP::EPlayModes(c4Cur);
    }
}
tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDUPnP::EPlayModes* const aeList, const tCIDLib::TCard4 c4Count)
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        strmTar << tCIDLib::TCard4(aeList[c4Index]);
}
tCIDLib::TBoolean tCIDUPnP::bIsValidEnum(const tCIDUPnP::EPlayModes eVal)
{
    return emapEPlayModes.bIsValidEnum(tCIDLib::TCard4(eVal));

}



static TEnumMap::TEnumValItem aeitemValues_ETransStates[8] = 
{
    {  tCIDLib::TInt4(tCIDUPnP::ETransStates::UNKNOWN), 0, 0,  { L"", L"", L"", L"UNKNOWN", L"ETransStates::UNKNOWN", L"Unknown" } }
  , {  tCIDLib::TInt4(tCIDUPnP::ETransStates::STOPPED), 0, 0,  { L"", L"", L"", L"STOPPED", L"ETransStates::STOPPED", L"Stopped" } }
  , {  tCIDLib::TInt4(tCIDUPnP::ETransStates::PAUSED_PLAYBACK), 0, 0,  { L"", L"", L"", L"PAUSED_PLAYBACK", L"ETransStates::PAUSED_PLAYBACK", L"Paused PB" } }
  , {  tCIDLib::TInt4(tCIDUPnP::ETransStates::PAUSED_RECORDING), 0, 0,  { L"", L"", L"", L"PAUSED_RECORDING", L"ETransStates::PAUSED_RECORDING", L"Paused Rec" } }
  , {  tCIDLib::TInt4(tCIDUPnP::ETransStates::PLAYING), 0, 0,  { L"", L"", L"", L"PLAYING", L"ETransStates::PLAYING", L"Playing" } }
  , {  tCIDLib::TInt4(tCIDUPnP::ETransStates::RECORDING), 0, 0,  { L"", L"", L"", L"RECORDING", L"ETransStates::RECORDING", L"Recording" } }
  , {  tCIDLib::TInt4(tCIDUPnP::ETransStates::TRANSITIONING), 0, 0,  { L"", L"", L"", L"TRANSITIONING", L"ETransStates::TRANSITIONING", L"Transitioning" } }
  , {  tCIDLib::TInt4(tCIDUPnP::ETransStates::NO_MEDIA_PRESENT), 0, 0,  { L"", L"", L"", L"NO_MEDIA_PRESENT", L"ETransStates::NO_MEDIA_PRESENT", L"No Media" } }

};

static TEnumMap emapETransStates
(
     L"ETransStates"
     , 8
     , kCIDLib::False
     , aeitemValues_ETransStates
     , nullptr
     , tCIDLib::TCard4(tCIDUPnP::ETransStates::Count)
);

const TString& tCIDUPnP::strAltXlatETransStates(const tCIDUPnP::ETransStates eVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return emapETransStates.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::BaseName, bThrowIfNot);
}

tCIDUPnP::ETransStates tCIDUPnP::eAltXlatETransStates(const TString& strVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return tCIDUPnP::ETransStates(emapETransStates.i4MapEnumText(strVal, TEnumMap::ETextVals::BaseName, bThrowIfNot));
}

tCIDLib::TVoid tCIDUPnP::FormatETransStates(TString& strToFill, const TString& strPrefix, const tCIDLib::TCh chSepChar, const TEnumMap::ETextVals eVal)
{
    emapETransStates.FormatValues(strToFill, strPrefix, chSepChar, eVal);
}
const TString& tCIDUPnP::strXlatETransStates(const tCIDUPnP::ETransStates eVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return emapETransStates.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::Text, bThrowIfNot);
}

tCIDUPnP::ETransStates tCIDUPnP::eXlatETransStates(const TString& strVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return tCIDUPnP::ETransStates(emapETransStates.i4MapEnumText(strVal, TEnumMap::ETextVals::Text, bThrowIfNot));
}

TTextOutStream& tCIDUPnP::operator<<(TTextOutStream& strmTar, const tCIDUPnP::ETransStates eVal)
{
    strmTar << emapETransStates.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::Text, kCIDLib::False);
    return strmTar;
}
tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDUPnP::ETransStates* const aeList, const tCIDLib::TCard4 c4Count)
{
    tCIDLib::TCard4 c4Cur;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        strmSrc >> c4Cur;
        aeList[c4Index] = tCIDUPnP::ETransStates(c4Cur);
    }
}
tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDUPnP::ETransStates* const aeList, const tCIDLib::TCard4 c4Count)
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        strmTar << tCIDLib::TCard4(aeList[c4Index]);
}
tCIDLib::TBoolean tCIDUPnP::bIsValidEnum(const tCIDUPnP::ETransStates eVal)
{
    return emapETransStates.bIsValidEnum(tCIDLib::TCard4(eVal));

}

