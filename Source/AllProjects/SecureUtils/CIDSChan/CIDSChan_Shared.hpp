// ----------------------------------------------------------------------------
//  FILE: CIDSChan_Shared.hpp
//  DATE: Fri, Oct 01 19:41:29 2021 -0400
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

// ----------------------------------------------------------------------------
// Types namespace
// ----------------------------------------------------------------------------
namespace tCIDSChan
{
    
    // ------------------------------------------------------------------------
    //  Options for creating a secure channel connection
    //                  
    // ------------------------------------------------------------------------
    enum class EConnOpts : tCIDLib::TCard4
    {
        NoDefCreds = 0x00000001
        , AllowTLS1_1 = 0x01000000
        , Count = 2
        , None = 0
        , AllBits = 0x1000001
    };
    [[nodiscard]]  EConnOpts eXlatEConnOpts(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]]  const TString& strXlatEConnOpts(const EConnOpts eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
     tCIDLib::TVoid FormatEConnOpts
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::BaseName
    );
    [[nodiscard]]  tCIDLib::TBoolean bIsValidEnum(const EConnOpts eVal);

    [[nodiscard]] constexpr tCIDSChan::EConnOpts operator|(const tCIDSChan::EConnOpts eLHS, const tCIDSChan::EConnOpts eRHS)
    {    
        return tCIDSChan::EConnOpts(tCIDLib::TEnumMaskType(eLHS) | tCIDLib::TEnumMaskType(eRHS));
    }
    constexpr tCIDSChan::EConnOpts& operator|=(tCIDSChan::EConnOpts& eLHS, const tCIDSChan::EConnOpts eRHS)
    {    
        eLHS = tCIDSChan::EConnOpts(tCIDLib::TEnumMaskType(eLHS) | tCIDLib::TEnumMaskType(eRHS));
        return eLHS;
    }
    [[nodiscard]] constexpr tCIDSChan::EConnOpts operator&(const tCIDSChan::EConnOpts eLHS, const tCIDSChan::EConnOpts eRHS)
    {    
        return tCIDSChan::EConnOpts(tCIDLib::TEnumMaskType(eLHS) & tCIDLib::TEnumMaskType(eRHS));
    }
    constexpr tCIDSChan::EConnOpts& operator&=(tCIDSChan::EConnOpts& eLHS, const tCIDSChan::EConnOpts eRHS)
    {    
        eLHS = tCIDSChan::EConnOpts(tCIDLib::TEnumMaskType(eLHS) & tCIDLib::TEnumMaskType(eRHS));
        return eLHS;
    }
     TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDSChan::EConnOpts eToStream);
}

#pragma CIDLIB_POPPACK


