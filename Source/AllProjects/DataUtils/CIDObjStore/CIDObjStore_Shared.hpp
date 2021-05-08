// ----------------------------------------------------------------------------
//  FILE: CIDObjStore_Shared.hpp
//  DATE: Sat, May 01 11:54:20 2021 -0400
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

// ----------------------------------------------------------------------------
// Types namespace
// ----------------------------------------------------------------------------
namespace tCIDObjStore
{
    
    // ------------------------------------------------------------------------
    //  Options for creating a secure channel connection
    //                  
    // ------------------------------------------------------------------------
    enum class EFlags : tCIDLib::TCard4
    {
        CaseSensitive = 0x00000001
        , RecoveryMode = 0x00000002
        , Count = 2
        , None = 0
        , AllBits = 0x3
    };
    [[nodiscard]] CIDOBJSTOREEXP EFlags eXlatEFlags(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDOBJSTOREEXP const TString& strXlatEFlags(const EFlags eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    CIDOBJSTOREEXP tCIDLib::TVoid FormatEFlags
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::BaseName
    );
    [[nodiscard]] CIDOBJSTOREEXP tCIDLib::TBoolean bIsValidEnum(const EFlags eVal);

    [[nodiscard]] constexpr tCIDObjStore::EFlags operator|(const tCIDObjStore::EFlags eLHS, const tCIDObjStore::EFlags eRHS)
    {    
        return tCIDObjStore::EFlags(tCIDLib::TEnumMaskType(eLHS) | tCIDLib::TEnumMaskType(eRHS));
    }
    constexpr tCIDObjStore::EFlags& operator|=(tCIDObjStore::EFlags& eLHS, const tCIDObjStore::EFlags eRHS)
    {    
        eLHS = tCIDObjStore::EFlags(tCIDLib::TEnumMaskType(eLHS) | tCIDLib::TEnumMaskType(eRHS));
        return eLHS;
    }
    [[nodiscard]] constexpr tCIDObjStore::EFlags operator&(const tCIDObjStore::EFlags eLHS, const tCIDObjStore::EFlags eRHS)
    {    
        return tCIDObjStore::EFlags(tCIDLib::TEnumMaskType(eLHS) & tCIDLib::TEnumMaskType(eRHS));
    }
    constexpr tCIDObjStore::EFlags& operator&=(tCIDObjStore::EFlags& eLHS, const tCIDObjStore::EFlags eRHS)
    {    
        eLHS = tCIDObjStore::EFlags(tCIDLib::TEnumMaskType(eLHS) & tCIDLib::TEnumMaskType(eRHS));
        return eLHS;
    }
    CIDOBJSTOREEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDObjStore::EFlags eToStream);
}

#pragma CIDLIB_POPPACK


