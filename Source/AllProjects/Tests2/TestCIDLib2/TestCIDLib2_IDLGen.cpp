// ----------------------------------------------------------------------------
//  FILE: TestCIDLib2_IDLGen.cpp
//  DATE: Fri, Oct 01 19:41:29 2021 -0400
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------

#include "TestCIDLib2.hpp"



static TEnumMap::TEnumValItem aeitemValues_ETest1[5] = 
{
    {  tCIDLib::TInt4(tTestCIDLib2::ETest1::Value1), 0, 0,  { L"", L"", L"", L"Value1", L"ETest1::Value1", L"" } }
  , {  tCIDLib::TInt4(tTestCIDLib2::ETest1::Value2), 0, 0,  { L"", L"", L"", L"Value2", L"ETest1::Value2", L"" } }
  , {  tCIDLib::TInt4(tTestCIDLib2::ETest1::Value3), 0, 0,  { L"", L"", L"", L"Value3", L"ETest1::Value3", L"" } }
  , {  tCIDLib::TInt4(tTestCIDLib2::ETest1::Value4), 0, 0,  { L"", L"", L"", L"Value4", L"ETest1::Value4", L"" } }
  , {  tCIDLib::TInt4(tTestCIDLib2::ETest1::Value5), 0, 0,  { L"", L"", L"", L"Value5", L"ETest1::Value5", L"" } }

};

static TEnumMap emapETest1
(
     L"ETest1"
     , 5
     , kCIDLib::False
     , aeitemValues_ETest1
     , nullptr
     , tCIDLib::TCard4(tTestCIDLib2::ETest1::Count)
);

TTextOutStream& tTestCIDLib2::operator<<(TTextOutStream& strmTar, const tTestCIDLib2::ETest1 eVal)
{
    strmTar << emapETest1.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::BaseName, kCIDLib::False);
    return strmTar;
}
tCIDLib::TBoolean tTestCIDLib2::bIsValidEnum(const tTestCIDLib2::ETest1 eVal)
{
    return emapETest1.bIsValidEnum(tCIDLib::TCard4(eVal));

}

