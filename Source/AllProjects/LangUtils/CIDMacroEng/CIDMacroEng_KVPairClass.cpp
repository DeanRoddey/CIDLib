//
// FILE NAME: CIDMacroEng_KVPairClass.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/16/2003
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements a set of derivatives of the class info and class
//  value classes, which implement the MEng.System.Runtime.KVPair class.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDMacroEng_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMEngKVPairVal,TMEngClassVal)
RTTIDecls(TMEngKVPairInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_KVPairClass
{
    // -----------------------------------------------------------------------
    //  The names for the types that we support here. Each derivative has to
    //  be able to return strings that contain its name and full name.
    // -----------------------------------------------------------------------
    const TString   strKVPair(L"KVPair");
    const TString   strKVPairClassPath(L"MEng.System.Runtime.KVPair");
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngKVPairVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngKVPairVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngKVPairVal::TMEngKVPairVal( const   TString&                strName
                                , const tCIDLib::TCard2         c2Id
                                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
{
}

TMEngKVPairVal::TMEngKVPairVal( const   TString&                strName
                                , const tCIDLib::TCard2         c2Id
                                , const tCIDMacroEng::EConstTypes  eConst
                                , const TString&                strKey
                                , const TString&                strValue) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_strKey(strKey)
    , m_strValue(strValue)
{
}

TMEngKVPairVal::~TMEngKVPairVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngKVPairVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngKVPairVal::Reset()
{
    m_strKey.Clear();
    m_strValue.Clear();
}


const TString& TMEngKVPairVal::strKey() const
{
    return m_strKey;
}

const TString& TMEngKVPairVal::strKey(const TString& strToSet)
{
    m_strKey = strToSet;
    return m_strKey;
}


const TString& TMEngKVPairVal::strValue() const
{
    return m_strValue;
}

const TString& TMEngKVPairVal::strValue(const TString& strToSet)
{
    m_strValue = strToSet;
    return m_strValue;
}


tCIDLib::TVoid TMEngKVPairVal::Set( const   TString& strKey
                                    , const TString& strValue)
{
    m_strKey = strKey;
    m_strValue = strValue;
}

tCIDLib::TVoid TMEngKVPairVal::SetBoolean(const tCIDLib::TBoolean bValue)
{
    if (bValue)
        m_strValue = L"True";
    else
        m_strValue = L"False";
}

tCIDLib::TVoid TMEngKVPairVal::SetCard4(const tCIDLib::TCard4 c4Value)
{
    m_strValue.SetFormatted(c4Value);
}

tCIDLib::TVoid TMEngKVPairVal::SetFloat8(const tCIDLib::TFloat8 f8Value)
{
    m_strValue.SetFormatted(f8Value);
}

tCIDLib::TVoid TMEngKVPairVal::SetInt4( const tCIDLib::TInt4 i4Value)
{
    m_strValue.SetFormatted(i4Value);
}


// ---------------------------------------------------------------------------
//  TMEngKVPairVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngKVPairVal::bDbgFormat(         TTextOutStream&     strmTarget
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices
                            , const TCIDMacroEngine&    ) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Short)
        strmTarget << m_strKey;
    else
        strmTarget << L"Key=" << m_strKey << L"\nValue=" << m_strValue;
    return kCIDLib::True;
}


tCIDLib::TVoid
TMEngKVPairVal::CopyFrom(const  TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    m_strKey = static_cast<const TMEngKVPairVal&>(mecvToCopy).m_strKey;
    m_strValue = static_cast<const TMEngKVPairVal&>(mecvToCopy).m_strValue;
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngKVPairInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngKVPairInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngKVPairInfo::strPath()
{
    return CIDMacroEng_KVPairClass::strKVPairClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngKVPairInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngKVPairInfo::TMEngKVPairInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_KVPairClass::strKVPair
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2MethId_CopyKey(kMacroEng::c2BadId)
    , m_c2MethId_CopyValue(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Equal(kMacroEng::c2BadId)
    , m_c2MethId_GetKey(kMacroEng::c2BadId)
    , m_c2MethId_GetValue(kMacroEng::c2BadId)
    , m_c2MethId_MatchesKey(kMacroEng::c2BadId)
    , m_c2MethId_MatchesValue(kMacroEng::c2BadId)
    , m_c2MethId_SameKey(kMacroEng::c2BadId)
    , m_c2MethId_SameValue(kMacroEng::c2BadId)
    , m_c2MethId_SetKey(kMacroEng::c2BadId)
    , m_c2MethId_SetValue(kMacroEng::c2BadId)
    , m_c2MethId_Set(kMacroEng::c2BadId)
    , m_c2MethId_SetBoolean(kMacroEng::c2BadId)
    , m_c2MethId_SetCard4(kMacroEng::c2BadId)
    , m_c2MethId_SetFloat8(kMacroEng::c2BadId)
    , m_c2MethId_SetInt4(kMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kMacroEng::c2BadId)
{
}

TMEngKVPairInfo::~TMEngKVPairInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngKVPairInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngKVPairInfo::Init(TCIDMacroEngine&)
{
    // Get a copy of the key
    {
        TMEngMethodInfo methiNew
        (
            L"CopyKey"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_CopyKey = c2AddMethodInfo(methiNew);
    }

    // Get a copy of the value
    {
        TMEngMethodInfo methiNew
        (
            L"CopyValue"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_CopyValue = c2AddMethodInfo(methiNew);
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.KVPair"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // We support equality testing
    {
        TMEngMethodInfo methiNew
        (
            L"Equal"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", c2Id());
        m_c2MethId_Equal = c2AddMethodInfo(methiNew);
    }

    // Get the key
    {
        TMEngMethodInfo methiNew
        (
            L"GetKey"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetKey = c2AddMethodInfo(methiNew);
    }

    // Get the value
    {
        TMEngMethodInfo methiNew
        (
            L"GetValue"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetValue = c2AddMethodInfo(methiNew);
    }

    // Check for a key match
    {
        TMEngMethodInfo methiNew
        (
            L"MatchesKey"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCheck", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_MatchesKey = c2AddMethodInfo(methiNew);
    }

    // Check for a value match
    {
        TMEngMethodInfo methiNew
        (
            L"MatchesValue"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCheck", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_MatchesValue = c2AddMethodInfo(methiNew);
    }

    // See if two pairs have the same key or same value
    {
        TMEngMethodInfo methiNew
        (
            L"SameKey"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCheck", c2Id());
        m_c2MethId_SameKey = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"SameValue"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCheck", c2Id());
        m_c2MethId_SameValue = c2AddMethodInfo(methiNew);
    }

    // Set the key field
    {
        TMEngMethodInfo methiNew
        (
            L"SetKey"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_SetKey = c2AddMethodInfo(methiNew);
    }

    // Set the value field
    {
        TMEngMethodInfo methiNew
        (
            L"SetValue"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_SetValue = c2AddMethodInfo(methiNew);
    }

    // Set both values
    {
        TMEngMethodInfo methiNew
        (
            L"Set"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"KeyToSet", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"ValueToSet", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_Set = c2AddMethodInfo(methiNew);
    }

    // Set both values, with specialized value setters for convenience
    {
        TMEngMethodInfo methiNew
        (
            L"SetBoolean"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"KeyToSet", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"ValueToSet", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_SetBoolean = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"SetCard4"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"KeyToSet", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"ValueToSet", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SetCard4 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"SetFloat8"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"KeyToSet", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"ValueToSet", tCIDMacroEng::EIntrinsics::Float8);
        m_c2MethId_SetFloat8 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"SetInt4"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"KeyToSet", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"ValueToSet", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_SetInt4 = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that takes an initial value
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.System.Runtime.KVPair"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"InitKey", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"InitValue", tCIDMacroEng::EIntrinsics::String);
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_ValCtor = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngKVPairInfo::pmecvMakeStorage(  const   TString&                strName
                                    ,       TCIDMacroEngine&
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngKVPairVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngKVPairInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngKVPairInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngKVPairVal& mecvActual = static_cast<TMEngKVPairVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_CopyKey)
    {
        TMEngStringVal& mecvFill
                        = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
        mecvFill.strValue(mecvActual.strKey());
    }
     else if (c2MethId == m_c2MethId_CopyValue)
    {
        TMEngStringVal& mecvFill
                        = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
        mecvFill.strValue(mecvActual.strValue());
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        mecvActual.Reset();
    }
     else if (c2MethId == m_c2MethId_Equal)
    {
        // Get the return and the comparison parm
        TMEngBooleanVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngKVPairVal& mecvComp
                        = meOwner.mecvStackAtAs<TMEngKVPairVal>(c4FirstInd);
        mecvRet.bValue
        (
            (mecvActual.strKey() == mecvComp.strKey())
            && (mecvActual.strValue() == mecvComp.strValue())
        );
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner)  << mecvActual.strKey()
                                        << kCIDLib::chEquals << mecvActual.strValue();
    }
     else if (c2MethId == m_c2MethId_GetKey)
    {
        TMEngStringVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        mecvRet.strValue(mecvActual.strKey());
    }
     else if (c2MethId == m_c2MethId_GetValue)
    {
        TMEngStringVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        mecvRet.strValue(mecvActual.strValue());
    }
     else if (c2MethId == m_c2MethId_MatchesKey)
    {
        TMEngBooleanVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.strKey() == meOwner.strStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_MatchesValue)
    {
        TMEngBooleanVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.strValue() == meOwner.strStackValAt(c4FirstInd));
    }
     else if ((c2MethId == m_c2MethId_SameKey)
          ||  (c2MethId == m_c2MethId_SameValue))
    {
        // Get the return and the comparison parm
        TMEngBooleanVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngKVPairVal& mecvComp
                        = meOwner.mecvStackAtAs<TMEngKVPairVal>(c4FirstInd);

        if (c2MethId == m_c2MethId_SameKey)
            mecvRet.bValue(mecvActual.strKey() == mecvComp.strKey());
        else
            mecvRet.bValue(mecvActual.strValue() == mecvComp.strValue());
    }

     else if (c2MethId == m_c2MethId_SetKey)
    {
        mecvActual.strKey(meOwner.strStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_SetValue)
    {
        mecvActual.strValue(meOwner.strStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Set)
    {
        mecvActual.strKey(meOwner.strStackValAt(c4FirstInd));
        mecvActual.strValue(meOwner.strStackValAt(c4FirstInd + 1));
    }
     else if (c2MethId == m_c2MethId_SetBoolean)
    {
        mecvActual.strKey(meOwner.strStackValAt(c4FirstInd));
        mecvActual.SetBoolean(meOwner.bStackValAt(c4FirstInd + 1));
    }
     else if (c2MethId == m_c2MethId_SetCard4)
    {
        mecvActual.strKey(meOwner.strStackValAt(c4FirstInd));
        mecvActual.SetCard4(meOwner.c4StackValAt(c4FirstInd + 1));
    }
     else if (c2MethId == m_c2MethId_SetFloat8)
    {
        mecvActual.strKey(meOwner.strStackValAt(c4FirstInd));
        mecvActual.SetFloat8(meOwner.f8StackValAt(c4FirstInd + 1));
    }
     else if (c2MethId == m_c2MethId_SetInt4)
    {
        mecvActual.strKey(meOwner.strStackValAt(c4FirstInd));
        mecvActual.SetInt4(meOwner.i4StackValAt(c4FirstInd + 1));
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        mecvActual.strKey(meOwner.strStackValAt(c4FirstInd));
        mecvActual.strValue(meOwner.strStackValAt(c4FirstInd + 1));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


