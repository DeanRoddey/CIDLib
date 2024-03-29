// ----------------------------------------------------------------------------
//  FILE: CIDMath_Shared.cpp
//  DATE: Fri, Oct 01 19:41:28 2021 -0400
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------

#include "CIDMath_.hpp"



static TEnumMap::TEnumValItem aeitemValues_EGraphScales[2] = 
{
    {  tCIDLib::TInt4(tCIDMath::EGraphScales::Linear), 0, 0,  { L"", L"", L"", L"Linear", L"EGraphScales::Linear", L"" } }
  , {  tCIDLib::TInt4(tCIDMath::EGraphScales::Log10), 0, 0,  { L"", L"", L"", L"Log10", L"EGraphScales::Log10", L"" } }

};

static TEnumMap emapEGraphScales
(
     L"EGraphScales"
     , 2
     , kCIDLib::False
     , aeitemValues_EGraphScales
     , nullptr
     , tCIDLib::TCard4(tCIDMath::EGraphScales::Count)
);

tCIDLib::TVoid tCIDMath::FormatEGraphScales(TString& strToFill, const TString& strPrefix, const tCIDLib::TCh chSepChar, const TEnumMap::ETextVals eVal)
{
    emapEGraphScales.FormatValues(strToFill, strPrefix, chSepChar, eVal);
}
const TString& tCIDMath::strXlatEGraphScales(const tCIDMath::EGraphScales eVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return emapEGraphScales.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::BaseName, bThrowIfNot);
}

tCIDMath::EGraphScales tCIDMath::eXlatEGraphScales(const TString& strVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return tCIDMath::EGraphScales(emapEGraphScales.i4MapEnumText(strVal, TEnumMap::ETextVals::BaseName, bThrowIfNot));
}

TTextOutStream& tCIDMath::operator<<(TTextOutStream& strmTar, const tCIDMath::EGraphScales eVal)
{
    strmTar << emapEGraphScales.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::BaseName, kCIDLib::False);
    return strmTar;
}
tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDMath::EGraphScales* const aeList, const tCIDLib::TCard4 c4Count)
{
    tCIDLib::TCard4 c4Cur = 0;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        strmSrc >> c4Cur;
        aeList[c4Index] = tCIDMath::EGraphScales(c4Cur);
    }
}
tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDMath::EGraphScales* const aeList, const tCIDLib::TCard4 c4Count)
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        strmTar << tCIDLib::TCard4(aeList[c4Index]);
}
tCIDLib::TBoolean tCIDMath::bIsValidEnum(const tCIDMath::EGraphScales eVal)
{
    return emapEGraphScales.bIsValidEnum(tCIDLib::TCard4(eVal));

}



static TEnumMap::TEnumValItem aeitemValues_ELogTypes[2] = 
{
    {  tCIDLib::TInt4(tCIDMath::ELogTypes::Natural), 0, 0,  { L"", L"", L"", L"Natural", L"ELogTypes::Natural", L"Natural" } }
  , {  tCIDLib::TInt4(tCIDMath::ELogTypes::Base10), 0, 0,  { L"", L"", L"", L"Base10", L"ELogTypes::Base10", L"Base 10" } }

};

static TEnumMap emapELogTypes
(
     L"ELogTypes"
     , 2
     , kCIDLib::False
     , aeitemValues_ELogTypes
     , nullptr
     , tCIDLib::TCard4(tCIDMath::ELogTypes::Count)
);

const TString& tCIDMath::strXlatELogTypes(const tCIDMath::ELogTypes eVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return emapELogTypes.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::Text, bThrowIfNot);
}

tCIDMath::ELogTypes tCIDMath::eXlatELogTypes(const TString& strVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return tCIDMath::ELogTypes(emapELogTypes.i4MapEnumText(strVal, TEnumMap::ETextVals::Text, bThrowIfNot));
}

TTextOutStream& tCIDMath::operator<<(TTextOutStream& strmTar, const tCIDMath::ELogTypes eVal)
{
    strmTar << emapELogTypes.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::Text, kCIDLib::False);
    return strmTar;
}
tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDMath::ELogTypes* const aeList, const tCIDLib::TCard4 c4Count)
{
    tCIDLib::TCard4 c4Cur = 0;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        strmSrc >> c4Cur;
        aeList[c4Index] = tCIDMath::ELogTypes(c4Cur);
    }
}
tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDMath::ELogTypes* const aeList, const tCIDLib::TCard4 c4Count)
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        strmTar << tCIDLib::TCard4(aeList[c4Index]);
}
tCIDLib::TBoolean tCIDMath::bIsValidEnum(const tCIDMath::ELogTypes eVal)
{
    return emapELogTypes.bIsValidEnum(tCIDLib::TCard4(eVal));

}



static TEnumMap::TEnumValItem aeitemValues_EMatrixElems[16] = 
{
    {  tCIDLib::TInt4(tCIDMath::EMatrixElems::M11), 0, 0,  { L"", L"", L"", L"M11", L"EMatrixElems::M11", L"1-1" } }
  , {  tCIDLib::TInt4(tCIDMath::EMatrixElems::M12), 0, 0,  { L"", L"", L"", L"M12", L"EMatrixElems::M12", L"1-2" } }
  , {  tCIDLib::TInt4(tCIDMath::EMatrixElems::M13), 0, 0,  { L"", L"", L"", L"M13", L"EMatrixElems::M13", L"1-3" } }
  , {  tCIDLib::TInt4(tCIDMath::EMatrixElems::M14), 0, 0,  { L"", L"", L"", L"M14", L"EMatrixElems::M14", L"1-4" } }
  , {  tCIDLib::TInt4(tCIDMath::EMatrixElems::M21), 0, 0,  { L"", L"", L"", L"M21", L"EMatrixElems::M21", L"2-1" } }
  , {  tCIDLib::TInt4(tCIDMath::EMatrixElems::M22), 0, 0,  { L"", L"", L"", L"M22", L"EMatrixElems::M22", L"2-2" } }
  , {  tCIDLib::TInt4(tCIDMath::EMatrixElems::M23), 0, 0,  { L"", L"", L"", L"M23", L"EMatrixElems::M23", L"2-3" } }
  , {  tCIDLib::TInt4(tCIDMath::EMatrixElems::M24), 0, 0,  { L"", L"", L"", L"M24", L"EMatrixElems::M24", L"2-4" } }
  , {  tCIDLib::TInt4(tCIDMath::EMatrixElems::M31), 0, 0,  { L"", L"", L"", L"M31", L"EMatrixElems::M31", L"3-1" } }
  , {  tCIDLib::TInt4(tCIDMath::EMatrixElems::M32), 0, 0,  { L"", L"", L"", L"M32", L"EMatrixElems::M32", L"3-2" } }
  , {  tCIDLib::TInt4(tCIDMath::EMatrixElems::M33), 0, 0,  { L"", L"", L"", L"M33", L"EMatrixElems::M33", L"3-3" } }
  , {  tCIDLib::TInt4(tCIDMath::EMatrixElems::M34), 0, 0,  { L"", L"", L"", L"M34", L"EMatrixElems::M34", L"3-4" } }
  , {  tCIDLib::TInt4(tCIDMath::EMatrixElems::MXt), 0, 0,  { L"", L"", L"", L"MXt", L"EMatrixElems::MXt", L"Xt" } }
  , {  tCIDLib::TInt4(tCIDMath::EMatrixElems::MYt), 0, 0,  { L"", L"", L"", L"MYt", L"EMatrixElems::MYt", L"Yt" } }
  , {  tCIDLib::TInt4(tCIDMath::EMatrixElems::MZt), 0, 0,  { L"", L"", L"", L"MZt", L"EMatrixElems::MZt", L"Zt" } }
  , {  tCIDLib::TInt4(tCIDMath::EMatrixElems::M44), 0, 0,  { L"", L"", L"", L"M44", L"EMatrixElems::M44", L"4-4" } }

};

static TEnumMap emapEMatrixElems
(
     L"EMatrixElems"
     , 16
     , kCIDLib::False
     , aeitemValues_EMatrixElems
     , nullptr
     , tCIDLib::TCard4(tCIDMath::EMatrixElems::Count)
);

const TString& tCIDMath::strAltXlatEMatrixElems(const tCIDMath::EMatrixElems eVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return emapEMatrixElems.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::BaseName, bThrowIfNot);
}

tCIDMath::EMatrixElems tCIDMath::eAltXlatEMatrixElems(const TString& strVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return tCIDMath::EMatrixElems(emapEMatrixElems.i4MapEnumText(strVal, TEnumMap::ETextVals::BaseName, bThrowIfNot));
}

const TString& tCIDMath::strXlatEMatrixElems(const tCIDMath::EMatrixElems eVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return emapEMatrixElems.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::Text, bThrowIfNot);
}

tCIDMath::EMatrixElems tCIDMath::eXlatEMatrixElems(const TString& strVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return tCIDMath::EMatrixElems(emapEMatrixElems.i4MapEnumText(strVal, TEnumMap::ETextVals::Text, bThrowIfNot));
}

TTextOutStream& tCIDMath::operator<<(TTextOutStream& strmTar, const tCIDMath::EMatrixElems eVal)
{
    strmTar << emapEMatrixElems.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::Text, kCIDLib::False);
    return strmTar;
}
tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDMath::EMatrixElems* const aeList, const tCIDLib::TCard4 c4Count)
{
    tCIDLib::TCard4 c4Cur = 0;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        strmSrc >> c4Cur;
        aeList[c4Index] = tCIDMath::EMatrixElems(c4Cur);
    }
}
tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDMath::EMatrixElems* const aeList, const tCIDLib::TCard4 c4Count)
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        strmTar << tCIDLib::TCard4(aeList[c4Index]);
}
tCIDLib::TBoolean tCIDMath::bIsValidEnum(const tCIDMath::EMatrixElems eVal)
{
    return emapEMatrixElems.bIsValidEnum(tCIDLib::TCard4(eVal));

}

