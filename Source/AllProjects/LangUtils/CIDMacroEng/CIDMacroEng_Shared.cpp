// ----------------------------------------------------------------------------
//  FILE: CIDMacroEng_Shared.cpp
//  DATE: Fri, Oct 01 19:41:29 2021 -0400
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------

#include "CIDMacroEng_.hpp"



static TEnumMap::TEnumValItem aeitemValues_EFlowTypes[15] = 
{
    {  tCIDLib::TInt4(tCIDMacroEng::EFlowTypes::None), 0, 0,  { L"", L"<None>", L"", L"None", L"EFlowTypes::None", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EFlowTypes::Case), 0, 0,  { L"", L"Case", L"", L"Case", L"EFlowTypes::Case", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EFlowTypes::Catch), 0, 0,  { L"", L"Catch", L"", L"Catch", L"EFlowTypes::Catch", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EFlowTypes::DoLoop), 0, 0,  { L"", L"DoLoop", L"", L"DoLoop", L"EFlowTypes::DoLoop", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EFlowTypes::DoWhile), 0, 0,  { L"", L"DoWhile", L"", L"DoWhile", L"EFlowTypes::DoWhile", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EFlowTypes::Else), 0, 0,  { L"", L"Else", L"", L"Else", L"EFlowTypes::Else", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EFlowTypes::ElseIf), 0, 0,  { L"", L"ElseIf", L"", L"ElseIf", L"EFlowTypes::ElseIf", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EFlowTypes::EndCase), 0, 0,  { L"", L"EndCase", L"", L"EndCase", L"EFlowTypes::EndCase", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EFlowTypes::EndTry), 0, 0,  { L"", L"EndTry", L"", L"EndTry", L"EFlowTypes::EndTry", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EFlowTypes::ForEach), 0, 0,  { L"", L"ForEach", L"", L"ForEach", L"EFlowTypes::ForEach", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EFlowTypes::FTCase), 0, 0,  { L"", L"FTCase", L"", L"FTCase", L"EFlowTypes::FTCase", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EFlowTypes::If), 0, 0,  { L"", L"If", L"", L"If", L"EFlowTypes::If", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EFlowTypes::Switch), 0, 0,  { L"", L"Switch", L"", L"Switch", L"EFlowTypes::Switch", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EFlowTypes::Try), 0, 0,  { L"", L"Try", L"", L"Try", L"EFlowTypes::Try", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EFlowTypes::While), 0, 0,  { L"", L"While", L"", L"While", L"EFlowTypes::While", L"" } }

};

static TEnumMap emapEFlowTypes
(
     L"EFlowTypes"
     , 15
     , kCIDLib::False
     , aeitemValues_EFlowTypes
     , nullptr
     , tCIDLib::TCard4(tCIDMacroEng::EFlowTypes::Count)
);

const TString& tCIDMacroEng::strXlatEFlowTypes(const tCIDMacroEng::EFlowTypes eVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return emapEFlowTypes.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::AltText, bThrowIfNot);
}

tCIDMacroEng::EFlowTypes tCIDMacroEng::eXlatEFlowTypes(const TString& strVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return tCIDMacroEng::EFlowTypes(emapEFlowTypes.i4MapEnumText(strVal, TEnumMap::ETextVals::AltText, bThrowIfNot));
}

TTextOutStream& tCIDMacroEng::operator<<(TTextOutStream& strmTar, const tCIDMacroEng::EFlowTypes eVal)
{
    strmTar << emapEFlowTypes.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::AltText, kCIDLib::False);
    return strmTar;
}
tCIDLib::TBoolean tCIDMacroEng::bIsValidEnum(const tCIDMacroEng::EFlowTypes eVal)
{
    return emapEFlowTypes.bIsValidEnum(tCIDLib::TCard4(eVal));

}



static TEnumMap::TEnumValItem aeitemValues_EOpCodes[56] = 
{
    {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::None), 0, 0,  { L"", L"", L"", L"None", L"EOpCodes::None", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::CallExcept), 0, 0,  { L"", L"", L"", L"CallExcept", L"EOpCodes::CallExcept", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::CallLocal), 0, 0,  { L"", L"", L"", L"CallLocal", L"EOpCodes::CallLocal", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::CallMember), 0, 0,  { L"", L"", L"", L"CallMember", L"EOpCodes::CallMember", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::CallParent), 0, 0,  { L"", L"", L"", L"CallParent", L"EOpCodes::CallParent", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::CallParm), 0, 0,  { L"", L"", L"", L"CallParm", L"EOpCodes::CallParm", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::CallStack), 0, 0,  { L"", L"", L"", L"CallStack", L"EOpCodes::CallStack", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::CallThis), 0, 0,  { L"", L"", L"", L"CallThis", L"EOpCodes::CallThis", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::ColIndex), 0, 0,  { L"", L"", L"", L"ColIndex", L"EOpCodes::ColIndex", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::Copy), 0, 0,  { L"", L"", L"", L"Copy", L"EOpCodes::Copy", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::CondEnumInc), 0, 0,  { L"", L"", L"", L"CondEnumInc", L"EOpCodes::CondEnumInc", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::CondJump), 0, 0,  { L"", L"", L"", L"CondJump", L"EOpCodes::CondJump", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::CondJumpNP), 0, 0,  { L"", L"", L"", L"CondJumpNP", L"EOpCodes::CondJumpNP", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::CurLine), 0, 0,  { L"", L"", L"", L"CurLine", L"EOpCodes::CurLine", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::EndTry), 0, 0,  { L"", L"", L"", L"EndTry", L"EOpCodes::EndTry", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::FlipTop), 0, 0,  { L"", L"", L"", L"FlipTop", L"EOpCodes::FlipTop", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::Jump), 0, 0,  { L"", L"", L"", L"Jump", L"EOpCodes::Jump", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::LogicalAnd), 0, 0,  { L"", L"", L"", L"LogicalAnd", L"EOpCodes::LogicalAnd", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::LogicalOr), 0, 0,  { L"", L"", L"", L"LogicalOr", L"EOpCodes::LogicalOr", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::LogicalXor), 0, 0,  { L"", L"", L"", L"LogicalXor", L"EOpCodes::LogicalXor", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::MultiPop), 0, 0,  { L"", L"", L"", L"MultiPop", L"EOpCodes::MultiPop", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::Negate), 0, 0,  { L"", L"", L"", L"Negate", L"EOpCodes::Negate", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::NoOp), 0, 0,  { L"", L"", L"", L"NoOp", L"EOpCodes::NoOp", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::NotCondJumpNP), 0, 0,  { L"", L"", L"", L"NotCondJumpNP", L"EOpCodes::NotCondJumpNP", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::NotCondJump), 0, 0,  { L"", L"", L"", L"NotCondJump", L"EOpCodes::NotCondJump", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PopTop), 0, 0,  { L"", L"", L"", L"PopTop", L"EOpCodes::PopTop", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PopToReturn), 0, 0,  { L"", L"", L"", L"PopToReturn", L"EOpCodes::PopToReturn", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushCurLine), 0, 0,  { L"", L"", L"", L"PushCurLine", L"EOpCodes::PushCurLine", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushEnum), 0, 0,  { L"", L"", L"", L"PushEnum", L"EOpCodes::PushEnum", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushException), 0, 0,  { L"", L"", L"", L"PushException", L"EOpCodes::PushException", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushImBoolean), 0, 0,  { L"", L"", L"", L"PushImBoolean", L"EOpCodes::PushImBoolean", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushImCard1), 0, 0,  { L"", L"", L"", L"PushImCard1", L"EOpCodes::PushImCard1", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushImCard2), 0, 0,  { L"", L"", L"", L"PushImCard2", L"EOpCodes::PushImCard2", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushImCard4), 0, 0,  { L"", L"", L"", L"PushImCard4", L"EOpCodes::PushImCard4", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushImCard8), 0, 0,  { L"", L"", L"", L"PushImCard8", L"EOpCodes::PushImCard8", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushImChar), 0, 0,  { L"", L"", L"", L"PushImChar", L"EOpCodes::PushImChar", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushImFloat4), 0, 0,  { L"", L"", L"", L"PushImFloat4", L"EOpCodes::PushImFloat4", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushImFloat8), 0, 0,  { L"", L"", L"", L"PushImFloat8", L"EOpCodes::PushImFloat8", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushImInt1), 0, 0,  { L"", L"", L"", L"PushImInt1", L"EOpCodes::PushImInt1", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushImInt2), 0, 0,  { L"", L"", L"", L"PushImInt2", L"EOpCodes::PushImInt2", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushImInt4), 0, 0,  { L"", L"", L"", L"PushImInt4", L"EOpCodes::PushImInt4", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushLocal), 0, 0,  { L"", L"", L"", L"PushLocal", L"EOpCodes::PushLocal", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushMember), 0, 0,  { L"", L"", L"", L"PushMember", L"EOpCodes::PushMember", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushParm), 0, 0,  { L"", L"", L"", L"PushParm", L"EOpCodes::PushParm", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushStrPoolItem), 0, 0,  { L"", L"", L"", L"PushStrPoolItem", L"EOpCodes::PushStrPoolItem", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushTempConst), 0, 0,  { L"", L"", L"", L"PushTempConst", L"EOpCodes::PushTempConst", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushTempVar), 0, 0,  { L"", L"", L"", L"PushTempVar", L"EOpCodes::PushTempVar", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::PushThis), 0, 0,  { L"", L"", L"", L"PushThis", L"EOpCodes::PushThis", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::Repush), 0, 0,  { L"", L"", L"", L"Repush", L"EOpCodes::Repush", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::ResetEnum), 0, 0,  { L"", L"", L"", L"ResetEnum", L"EOpCodes::ResetEnum", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::Return), 0, 0,  { L"", L"", L"", L"Return", L"EOpCodes::Return", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::TableJump), 0, 0,  { L"", L"", L"", L"TableJump", L"EOpCodes::TableJump", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::Throw), 0, 0,  { L"", L"", L"", L"Throw", L"EOpCodes::Throw", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::ThrowFmt), 0, 0,  { L"", L"", L"", L"ThrowFmt", L"EOpCodes::ThrowFmt", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::Try), 0, 0,  { L"", L"", L"", L"Try", L"EOpCodes::Try", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EOpCodes::TypeCast), 0, 0,  { L"", L"", L"", L"TypeCast", L"EOpCodes::TypeCast", L"" } }

};

static TEnumMap emapEOpCodes
(
     L"EOpCodes"
     , 56
     , kCIDLib::False
     , aeitemValues_EOpCodes
     , nullptr
     , tCIDLib::TCard4(tCIDMacroEng::EOpCodes::Count)
);

const TString& tCIDMacroEng::strXlatEOpCodes(const tCIDMacroEng::EOpCodes eVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return emapEOpCodes.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::BaseName, bThrowIfNot);
}

tCIDMacroEng::EOpCodes tCIDMacroEng::eXlatEOpCodes(const TString& strVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return tCIDMacroEng::EOpCodes(emapEOpCodes.i4MapEnumText(strVal, TEnumMap::ETextVals::BaseName, bThrowIfNot));
}

TTextOutStream& tCIDMacroEng::operator<<(TTextOutStream& strmTar, const tCIDMacroEng::EOpCodes eVal)
{
    strmTar << emapEOpCodes.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::BaseName, kCIDLib::False);
    return strmTar;
}
tCIDLib::TBoolean tCIDMacroEng::bIsValidEnum(const tCIDMacroEng::EOpCodes eVal)
{
    return emapEOpCodes.bIsValidEnum(tCIDLib::TCard4(eVal));

}



static TEnumMap::TEnumValItem aeitemValues_EParmDirs[3] = 
{
    {  tCIDLib::TInt4(tCIDMacroEng::EParmDirs::In), 0, 0,  { L"", L"[In]", L"", L"In", L"EParmDirs::In", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EParmDirs::Out), 0, 0,  { L"", L"[Out]", L"", L"Out", L"EParmDirs::Out", L"" } }
  , {  tCIDLib::TInt4(tCIDMacroEng::EParmDirs::InOut), 0, 0,  { L"", L"[InOut]", L"", L"InOut", L"EParmDirs::InOut", L"" } }

};

static TEnumMap emapEParmDirs
(
     L"EParmDirs"
     , 3
     , kCIDLib::False
     , aeitemValues_EParmDirs
     , nullptr
     , tCIDLib::TCard4(tCIDMacroEng::EParmDirs::Count)
);

const TString& tCIDMacroEng::strXlatEParmDirs(const tCIDMacroEng::EParmDirs eVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return emapEParmDirs.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::AltText, bThrowIfNot);
}

tCIDMacroEng::EParmDirs tCIDMacroEng::eXlatEParmDirs(const TString& strVal, const tCIDLib::TBoolean bThrowIfNot)
{
    return tCIDMacroEng::EParmDirs(emapEParmDirs.i4MapEnumText(strVal, TEnumMap::ETextVals::AltText, bThrowIfNot));
}

TTextOutStream& tCIDMacroEng::operator<<(TTextOutStream& strmTar, const tCIDMacroEng::EParmDirs eVal)
{
    strmTar << emapEParmDirs.strMapEnumVal(tCIDLib::TCard4(eVal), TEnumMap::ETextVals::AltText, kCIDLib::False);
    return strmTar;
}
tCIDLib::TBoolean tCIDMacroEng::bIsValidEnum(const tCIDMacroEng::EParmDirs eVal)
{
    return emapEParmDirs.bIsValidEnum(tCIDLib::TCard4(eVal));

}

