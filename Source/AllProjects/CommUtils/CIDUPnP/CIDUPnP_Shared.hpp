// ----------------------------------------------------------------------------
//  FILE: CIDUPnP_Shared.hpp
//  DATE: Fri, Feb 12 21:10:39 2021 -0500
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

// ----------------------------------------------------------------------------
// Constants namespace
// ----------------------------------------------------------------------------
namespace kCIDUPnP
{
    
    // ------------------------------------------------------------------------
    //  Some well known UPnP device type URNs
    //                  
    // ------------------------------------------------------------------------
    CIDUPNPEXP const extern TString strDevType_MediaRenderer;
    CIDUPNPEXP const extern TString strDevType_MediaServer;
    CIDUPNPEXP const extern TString strDevType_RootDev;
    CIDUPNPEXP const extern TString strDevType_ZonePlayer;
    
    // ------------------------------------------------------------------------
    //  Some well known UPnP service type URNs
    //                  
    // ------------------------------------------------------------------------
    CIDUPNPEXP const extern TString strSvcType_AVTransport;
    CIDUPNPEXP const extern TString strSvcType_AlarmClock;
    CIDUPNPEXP const extern TString strSvcType_AudioIn;
    CIDUPNPEXP const extern TString strSvcType_ConnectionMgr;
    CIDUPNPEXP const extern TString strSvcType_ContentDirectory;
    CIDUPNPEXP const extern TString strSvcType_DeviceProperties;
    CIDUPNPEXP const extern TString strSvcType_GroupManagement;
    CIDUPNPEXP const extern TString strSvcType_MusicServices;
    CIDUPNPEXP const extern TString strSvcType_RendControl;
    CIDUPNPEXP const extern TString strSvcType_ZoneGroupTopology;
}

// ----------------------------------------------------------------------------
// Types namespace
// ----------------------------------------------------------------------------
namespace tCIDUPnP
{
    
    // ------------------------------------------------------------------------
    //  The supported UPnP play modes
    //                  
    // ------------------------------------------------------------------------
    enum class EPlayModes
    {
        UNKNOWN
        , NORMAL
        , SHUFFLE
        , REPEAT
        , REPEATONE
        , RANDOM
        , DIRECT
        , INTRO
        , Count
        , Min = UNKNOWN
        , Max = INTRO
    };
    [[nodiscard]] CIDUPNPEXP EPlayModes eXlatEPlayModes(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDUPNPEXP const TString& strXlatEPlayModes(const EPlayModes eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDUPNPEXP const TString& strAltXlatEPlayModes(const EPlayModes eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDUPNPEXP EPlayModes eAltXlatEPlayModes(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    CIDUPNPEXP tCIDLib::TVoid FormatEPlayModes
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::Text
    );
    [[nodiscard]] CIDUPNPEXP tCIDLib::TBoolean bIsValidEnum(const EPlayModes eVal);

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDUPnP::EPlayModes eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDUPnP::EPlayModes& eToFill)
    {
        eToFill = tCIDUPnP::EPlayModes(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDUPNPEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDUPnP::EPlayModes eToStream);
    
    // ------------------------------------------------------------------------
    //  The supported UPnP transport modes
    //                  
    // ------------------------------------------------------------------------
    enum class ETransStates
    {
        UNKNOWN
        , STOPPED
        , PAUSED_PLAYBACK
        , PAUSED_RECORDING
        , PLAYING
        , RECORDING
        , TRANSITIONING
        , NO_MEDIA_PRESENT
        , Count
        , Min = UNKNOWN
        , Max = NO_MEDIA_PRESENT
    };
    [[nodiscard]] CIDUPNPEXP ETransStates eXlatETransStates(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDUPNPEXP const TString& strXlatETransStates(const ETransStates eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDUPNPEXP const TString& strAltXlatETransStates(const ETransStates eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDUPNPEXP ETransStates eAltXlatETransStates(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    CIDUPNPEXP tCIDLib::TVoid FormatETransStates
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::Text
    );
    [[nodiscard]] CIDUPNPEXP tCIDLib::TBoolean bIsValidEnum(const ETransStates eVal);

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDUPnP::ETransStates eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDUPnP::ETransStates& eToFill)
    {
        eToFill = tCIDUPnP::ETransStates(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDUPNPEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDUPnP::ETransStates eToStream);
}

#pragma CIDLIB_POPPACK

CIDUPNPEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDUPnP::EPlayModes* const aeList, const tCIDLib::TCard4 c4Count);
CIDUPNPEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDUPnP::EPlayModes* const aeList, const tCIDLib::TCard4 c4Count);

CIDUPNPEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDUPnP::ETransStates* const aeList, const tCIDLib::TCard4 c4Count);
CIDUPNPEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDUPnP::ETransStates* const aeList, const tCIDLib::TCard4 c4Count);

