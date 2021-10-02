// ----------------------------------------------------------------------------
//  FILE: CIDMath_Shared.hpp
//  DATE: Fri, Oct 01 19:41:28 2021 -0400
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

// ----------------------------------------------------------------------------
// Types namespace
// ----------------------------------------------------------------------------
namespace tCIDMath
{
    
    // ------------------------------------------------------------------------
    //  The available graph vertical scaling types
    //                  
    // ------------------------------------------------------------------------
    enum class EGraphScales
    {
        Linear
        , Log10
        , Count
        , Min = Linear
        , Max = Log10
    };
    [[nodiscard]] CIDMATHEXP EGraphScales eXlatEGraphScales(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDMATHEXP const TString& strXlatEGraphScales(const EGraphScales eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    CIDMATHEXP tCIDLib::TVoid FormatEGraphScales
    (
                   TString&            strTarget
        , const    TString&            strPrefix
        , const    tCIDLib::TCh        chSepChar
        , const    TEnumMap::ETextVals eVal = TEnumMap::ETextVals::BaseName
    );
    [[nodiscard]] CIDMATHEXP tCIDLib::TBoolean bIsValidEnum(const EGraphScales eVal);

    inline tCIDMath::EGraphScales operator++(CIOP tCIDMath::EGraphScales& eVal, int)
    {
        const tCIDMath::EGraphScales eTmp = eVal;
        eVal = tCIDMath::EGraphScales(int(eVal)+1);
        return eTmp;
    }

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDMath::EGraphScales eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDMath::EGraphScales& eToFill)
    {
        eToFill = tCIDMath::EGraphScales(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDMATHEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDMath::EGraphScales eToStream);
    
    // ------------------------------------------------------------------------
    //  The types of logarithmic algorithms. Used by TFLogMap, but it is
    //  generally useful.
    //                  
    // ------------------------------------------------------------------------
    enum class ELogTypes
    {
        Natural
        , Base10
        , Count
        , Min = Natural
        , Max = Base10
    };
    [[nodiscard]] CIDMATHEXP ELogTypes eXlatELogTypes(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDMATHEXP const TString& strXlatELogTypes(const ELogTypes eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDMATHEXP tCIDLib::TBoolean bIsValidEnum(const ELogTypes eVal);

    inline tCIDMath::ELogTypes operator++(CIOP tCIDMath::ELogTypes& eVal, int)
    {
        const tCIDMath::ELogTypes eTmp = eVal;
        eVal = tCIDMath::ELogTypes(int(eVal)+1);
        return eTmp;
    }

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDMath::ELogTypes eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDMath::ELogTypes& eToFill)
    {
        eToFill = tCIDMath::ELogTypes(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDMATHEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDMath::ELogTypes eToStream);
    
    // ------------------------------------------------------------------------
    //  This enum is used to index matrices via the subscript operator.
    //                  
    // ------------------------------------------------------------------------
    enum class EMatrixElems
    {
        M11
        , M12
        , M13
        , M14
        , M21
        , M22
        , M23
        , M24
        , M31
        , M32
        , M33
        , M34
        , MXt
        , MYt
        , MZt
        , M44
        , Count
        , Min = M11
        , Max = M44
    };
    [[nodiscard]] CIDMATHEXP EMatrixElems eXlatEMatrixElems(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDMATHEXP const TString& strXlatEMatrixElems(const EMatrixElems eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDMATHEXP const TString& strAltXlatEMatrixElems(const EMatrixElems eToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::True);
    [[nodiscard]] CIDMATHEXP EMatrixElems eAltXlatEMatrixElems(const TString& strToXlat, const tCIDLib::TBoolean bThrowIfNot = kCIDLib::False);
    [[nodiscard]] CIDMATHEXP tCIDLib::TBoolean bIsValidEnum(const EMatrixElems eVal);

    inline tCIDMath::EMatrixElems operator++(CIOP tCIDMath::EMatrixElems& eVal, int)
    {
        const tCIDMath::EMatrixElems eTmp = eVal;
        eVal = tCIDMath::EMatrixElems(int(eVal)+1);
        return eTmp;
    }

    inline TBinOutStream& operator<<(TBinOutStream& strmTar, const tCIDMath::EMatrixElems eVal)
    {
        strmTar.WriteEnum(tCIDLib::TCard4(eVal));
        return strmTar;
    }
    inline TBinInStream& operator>>(TBinInStream& strmSrc, COP tCIDMath::EMatrixElems& eToFill)
    {
        eToFill = tCIDMath::EMatrixElems(strmSrc.c4ReadEnum());
        return strmSrc;
    }
    CIDMATHEXP TTextOutStream& operator<<(TTextOutStream& strmTar, const tCIDMath::EMatrixElems eToStream);
}

#pragma CIDLIB_POPPACK

CIDMATHEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDMath::EGraphScales* const aeList, const tCIDLib::TCard4 c4Count);
CIDMATHEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDMath::EGraphScales* const aeList, const tCIDLib::TCard4 c4Count);

CIDMATHEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDMath::ELogTypes* const aeList, const tCIDLib::TCard4 c4Count);
CIDMATHEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDMath::ELogTypes* const aeList, const tCIDLib::TCard4 c4Count);

CIDMATHEXP tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDMath::EMatrixElems* const aeList, const tCIDLib::TCard4 c4Count);
CIDMATHEXP tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDMath::EMatrixElems* const aeList, const tCIDLib::TCard4 c4Count);

