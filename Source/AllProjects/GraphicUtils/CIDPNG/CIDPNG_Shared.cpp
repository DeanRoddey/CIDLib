// ----------------------------------------------------------------------------
//  FILE: CIDPNG_Shared.cpp
//  DATE: Tue, Jan 19 17:36:47 2021 -0500
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------

#include "CIDPNG_.hpp"



static TEnumMap::TEnumValItem aeitemValues_EChunks[8] = 
{
    {  tCIDLib::TInt4(tCIDPNG::EChunks::Header), 0, 0,  { L"", L"", L"", L"Header", L"EChunks::Header", L"" } }
  , {  tCIDLib::TInt4(tCIDPNG::EChunks::Palette), 0, 0,  { L"", L"", L"", L"Palette", L"EChunks::Palette", L"" } }
  , {  tCIDLib::TInt4(tCIDPNG::EChunks::Trans), 0, 0,  { L"", L"", L"", L"Trans", L"EChunks::Trans", L"" } }
  , {  tCIDLib::TInt4(tCIDPNG::EChunks::Data), 0, 0,  { L"", L"", L"", L"Data", L"EChunks::Data", L"" } }
  , {  tCIDLib::TInt4(tCIDPNG::EChunks::End), 0, 0,  { L"", L"", L"", L"End", L"EChunks::End", L"" } }
  , {  tCIDLib::TInt4(tCIDPNG::EChunks::Gamma), 0, 0,  { L"", L"", L"", L"Gamma", L"EChunks::Gamma", L"" } }
  , {  tCIDLib::TInt4(tCIDPNG::EChunks::BgnClr), 0, 0,  { L"", L"", L"", L"BgnClr", L"EChunks::BgnClr", L"" } }
  , {  tCIDLib::TInt4(tCIDPNG::EChunks::TransClr), 0, 0,  { L"", L"", L"", L"TransClr", L"EChunks::TransClr", L"" } }

};

static TEnumMap emapEChunks
(
     L"EChunks"
     , 8
     , kCIDLib::True
     , aeitemValues_EChunks
     , nullptr
     , tCIDLib::TCard4(tCIDPNG::EChunks::None)
);

tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDPNG::EChunks* const aeList, const tCIDLib::TCard4 c4Count)
{
    tCIDLib::TCard4 c4Cur;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        strmSrc >> c4Cur;
        aeList[c4Index] = tCIDPNG::EChunks(c4Cur);
    }
}
tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDPNG::EChunks* const aeList, const tCIDLib::TCard4 c4Count)
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        strmTar << tCIDLib::TCard4(aeList[c4Index]);
}
tCIDLib::TBoolean tCIDPNG::bIsValidEnum(const tCIDPNG::EChunks eVal)
{
    return emapEChunks.bIsValidEnum(tCIDLib::TCard4(eVal));

}



static TEnumMap::TEnumValItem aeitemValues_ECompTypes[1] = 
{
    {  tCIDLib::TInt4(tCIDPNG::ECompTypes::Deflate), 0, 0,  { L"", L"", L"", L"Deflate", L"ECompTypes::Deflate", L"" } }

};

static TEnumMap emapECompTypes
(
     L"ECompTypes"
     , 1
     , kCIDLib::False
     , aeitemValues_ECompTypes
     , nullptr
     , tCIDLib::TCard4(tCIDPNG::ECompTypes::Count)
);

tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDPNG::ECompTypes* const aeList, const tCIDLib::TCard4 c4Count)
{
    tCIDLib::TCard4 c4Cur;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        strmSrc >> c4Cur;
        aeList[c4Index] = tCIDPNG::ECompTypes(c4Cur);
    }
}
tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDPNG::ECompTypes* const aeList, const tCIDLib::TCard4 c4Count)
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        strmTar << tCIDLib::TCard4(aeList[c4Index]);
}
tCIDLib::TBoolean tCIDPNG::bIsValidEnum(const tCIDPNG::ECompTypes eVal)
{
    return emapECompTypes.bIsValidEnum(tCIDLib::TCard4(eVal));

}



static TEnumMap::TEnumValItem aeitemValues_EInterlaces[2] = 
{
    {  tCIDLib::TInt4(tCIDPNG::EInterlaces::None), 0, 0,  { L"", L"", L"", L"None", L"EInterlaces::None", L"" } }
  , {  tCIDLib::TInt4(tCIDPNG::EInterlaces::Adam7), 0, 0,  { L"", L"", L"", L"Adam7", L"EInterlaces::Adam7", L"" } }

};

static TEnumMap emapEInterlaces
(
     L"EInterlaces"
     , 2
     , kCIDLib::False
     , aeitemValues_EInterlaces
     , nullptr
     , tCIDLib::TCard4(tCIDPNG::EInterlaces::Count)
);

tCIDLib::TVoid TBinInStream_ReadArray(TBinInStream& strmSrc, tCIDPNG::EInterlaces* const aeList, const tCIDLib::TCard4 c4Count)
{
    tCIDLib::TCard4 c4Cur;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        strmSrc >> c4Cur;
        aeList[c4Index] = tCIDPNG::EInterlaces(c4Cur);
    }
}
tCIDLib::TVoid TBinOutStream_WriteArray(TBinOutStream& strmTar, const tCIDPNG::EInterlaces* const aeList, const tCIDLib::TCard4 c4Count)
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        strmTar << tCIDLib::TCard4(aeList[c4Index]);
}
tCIDLib::TBoolean tCIDPNG::bIsValidEnum(const tCIDPNG::EInterlaces eVal)
{
    return emapEInterlaces.bIsValidEnum(tCIDLib::TCard4(eVal));

}

