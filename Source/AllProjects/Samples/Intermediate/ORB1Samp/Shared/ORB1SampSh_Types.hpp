// ----------------------------------------------------------------------------
//  FILE: ORB1SampSh_Types.hpp
//  DATE: Tue, Jan 19 17:36:48 2021 -0500
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

// ----------------------------------------------------------------------------
// Types namespace
// ----------------------------------------------------------------------------
namespace tORB1SampSh
{
    
    // ------------------------------------------------------------------------
    //  The status marital status of a patient
    //                  
    // ------------------------------------------------------------------------
    enum class EStatus
    {
        Divorced
        , Married
        , Single
        , Widowed
        , Count
        , Min = Divorced
        , Max = Widowed
    };
    [[nodiscard]] ORB1SAMPSHEXP EStatus eXlatEStatus(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] ORB1SAMPSHEXP const TString& strXlatEStatus(const EStatus eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] ORB1SAMPSHEXP tCIDLib::TBoolean bIsValidEnum(const EStatus eVal);

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tORB1SampSh::EStatus eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tORB1SampSh::EStatus& eToFill)
    {
        eToFill = tORB1SampSh::EStatus(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    ORB1SAMPSHEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tORB1SampSh::EStatus eToStream);
}

#pragma CIDLIB_POPPACK

ORB1SAMPSHEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tORB1SampSh::EStatus* const aeList, const tCIDLib::TCard4 c4Count);
ORB1SAMPSHEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tORB1SampSh::EStatus* const aeList, const tCIDLib::TCard4 c4Count);

