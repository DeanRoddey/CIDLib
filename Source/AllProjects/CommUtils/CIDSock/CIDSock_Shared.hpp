// ----------------------------------------------------------------------------
//  FILE: CIDSock_Shared.hpp
//  DATE: Fri, Feb 12 21:10:39 2021 -0500
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

// ----------------------------------------------------------------------------
// Types namespace
// ----------------------------------------------------------------------------
namespace tCIDSock
{
    
    // ------------------------------------------------------------------------
    //  URL encoding/expansion flags
    //                  
    // ------------------------------------------------------------------------
    enum class EExpOpts
    {
        None
        , Encode
        , Expand
        , Count
        , Min = None
        , Max = Expand
    };
    [[nodiscard]] CIDSOCKEXP EExpOpts eXlatEExpOpts(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDSOCKEXP const TString& strXlatEExpOpts(const EExpOpts eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDSOCKEXP tCIDLib::TBoolean bIsValidEnum(const EExpOpts eVal);

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDSock::EExpOpts eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDSock::EExpOpts& eToFill)
    {
        eToFill = tCIDSock::EExpOpts(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDSOCKEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDSock::EExpOpts eToStream);
    
    // ------------------------------------------------------------------------
    //  Our known supported internet protocols
    //                  
    // ------------------------------------------------------------------------
    enum class EProtos
    {
        None
        , File
        , HTTP
        , FTP
        , MailTo
        , News
        , HTTPS
        , RTSP
        , RTSPS
        , SIP
        , WS
        , WSS
        , Count
        , Min = None
        , Max = WSS
    };
    [[nodiscard]] CIDSOCKEXP EProtos eXlatEProtos(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDSOCKEXP const TString& strXlatEProtos(const EProtos eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDSOCKEXP const TString& strAltXlatEProtos(const EProtos eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDSOCKEXP EProtos eAltXlatEProtos(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDSOCKEXP tCIDLib::TBoolean bIsValidEnum(const EProtos eVal);

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDSock::EProtos eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDSock::EProtos& eToFill)
    {
        eToFill = tCIDSock::EProtos(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDSOCKEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDSock::EProtos eToStream);
    
    // ------------------------------------------------------------------------
    //  Flags for URL qualification
    //                  
    // ------------------------------------------------------------------------
    enum class EQualified
    {
        Partial
        , Full
        , DontCare
        , Count
        , Min = Partial
        , Max = DontCare
    };
    [[nodiscard]] CIDSOCKEXP EQualified eXlatEQualified(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDSOCKEXP const TString& strXlatEQualified(const EQualified eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDSOCKEXP tCIDLib::TBoolean bIsValidEnum(const EQualified eVal);

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDSock::EQualified eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDSock::EQualified& eToFill)
    {
        eToFill = tCIDSock::EQualified(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDSOCKEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDSock::EQualified eToStream);
    
    // ------------------------------------------------------------------------
    //  For servers that accept either secure or insecure or both types of connections
    //  this is a convenient way to store those options.
    //                  
    // ------------------------------------------------------------------------
    enum class ESecureOpts : tCIDLib::TCard4
    {
        Insecure = 0x00000001
        , Secure = 0x00000002
        , Both = 0x00000003
        , Count = 3
        , None = 0
        , AllBits = 0x3
    };
    [[nodiscard]] CIDSOCKEXP ESecureOpts eXlatESecureOpts(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDSOCKEXP const TString& strXlatESecureOpts(const ESecureOpts eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDSOCKEXP tCIDLib::TBoolean bIsValidEnum(const ESecureOpts eVal);

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDSock::ESecureOpts eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDSock::ESecureOpts& eToFill)
    {
        eToFill = tCIDSock::ESecureOpts(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    [[nodiscard]] constexpr tCIDSock::ESecureOpts operator|(const tCIDSock::ESecureOpts eLHS, const tCIDSock::ESecureOpts eRHS)
    {    
        return tCIDSock::ESecureOpts(tCIDLib::TEnumMaskType(eLHS) | tCIDLib::TEnumMaskType(eRHS));
    }
    constexpr tCIDSock::ESecureOpts& operator|=(tCIDSock::ESecureOpts& eLHS, const tCIDSock::ESecureOpts eRHS)
    {    
        eLHS = tCIDSock::ESecureOpts(tCIDLib::TEnumMaskType(eLHS) | tCIDLib::TEnumMaskType(eRHS));
        return eLHS;
    }
    [[nodiscard]] constexpr tCIDSock::ESecureOpts operator&(const tCIDSock::ESecureOpts eLHS, const tCIDSock::ESecureOpts eRHS)
    {    
        return tCIDSock::ESecureOpts(tCIDLib::TEnumMaskType(eLHS) & tCIDLib::TEnumMaskType(eRHS));
    }
    constexpr tCIDSock::ESecureOpts& operator&=(tCIDSock::ESecureOpts& eLHS, const tCIDSock::ESecureOpts eRHS)
    {    
        eLHS = tCIDSock::ESecureOpts(tCIDLib::TEnumMaskType(eLHS) & tCIDLib::TEnumMaskType(eRHS));
        return eLHS;
    }
    CIDSOCKEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDSock::ESecureOpts eToStream);
}

#pragma CIDLIB_POPPACK

CIDSOCKEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDSock::EExpOpts* const aeList, const tCIDLib::TCard4 c4Count);
CIDSOCKEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDSock::EExpOpts* const aeList, const tCIDLib::TCard4 c4Count);

CIDSOCKEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDSock::EProtos* const aeList, const tCIDLib::TCard4 c4Count);
CIDSOCKEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDSock::EProtos* const aeList, const tCIDLib::TCard4 c4Count);

CIDSOCKEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDSock::EQualified* const aeList, const tCIDLib::TCard4 c4Count);
CIDSOCKEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDSock::EQualified* const aeList, const tCIDLib::TCard4 c4Count);

CIDSOCKEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDSock::ESecureOpts* const aeList, const tCIDLib::TCard4 c4Count);
CIDSOCKEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDSock::ESecureOpts* const aeList, const tCIDLib::TCard4 c4Count);

