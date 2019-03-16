//
// FILE NAME: CIDMacroEng_IntClasses.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2003
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
//  value classes, which implement the integral fundamental intrinsic types.
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
RTTIDecls(TMEngInt1Val,TMEngClassVal)
RTTIDecls(TMEngInt1Info,TMEngClassInfo)
RTTIDecls(TMEngInt2Val,TMEngClassVal)
RTTIDecls(TMEngInt2Info,TMEngClassInfo)
RTTIDecls(TMEngInt4Val,TMEngClassVal)
RTTIDecls(TMEngInt4Info,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_IntClasses
{
    // -----------------------------------------------------------------------
    //  The names for the types that we support here. Each derivative has to
    //  be able to return strings that contain its name and full name.
    // -----------------------------------------------------------------------
    const TString   strInt1(L"Int1");
    const TString   strInt2(L"Int2");
    const TString   strInt4(L"Int4");

    const TString   strInt1ClassPath(L"MEng.Int1");
    const TString   strInt2ClassPath(L"MEng.Int2");
    const TString   strInt4ClassPath(L"MEng.Int4");
}


// ---------------------------------------------------------------------------
//  CLASS: TMEngInt1Val
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngInt1Val: Destructor
// ---------------------------------------------------------------------------
TMEngInt1Val::TMEngInt1Val( const   TString&                strName
                            , const tCIDMacroEng::EConstTypes  eConst
                            , const tCIDLib::TInt1          i1Init) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int1), eConst)
    , m_i1Value(i1Init)
{
}

TMEngInt1Val::~TMEngInt1Val()
{
}


// ---------------------------------------------------------------------------
//  TMEngInt1Val: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TInt1 TMEngInt1Val::i1Value() const
{
    return m_i1Value;
}

tCIDLib::TInt1 TMEngInt1Val::i1Value(const tCIDLib::TInt1 i1ToSet)
{
    m_i1Value = i1ToSet;
    return m_i1Value;
}


// ---------------------------------------------------------------------------
//  TMEngInt1Val: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngInt1Val::bDbgFormat(       TTextOutStream&     strmTarget
                        , const TMEngClassInfo&
                        , const tCIDMacroEng::EDbgFmts eFormat
                        , const tCIDLib::ERadices   eRadix
                        , const TCIDMacroEngine&    ) const
{
    strmTarget << TInteger(m_i1Value, eRadix);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMEngInt1Val::bParseFromText(const  TString&        strValue
                            , const TMEngClassInfo& )
{
    tCIDLib::TBoolean bRes;
    tCIDLib::TInt4 i4Val = TRawStr::i4AsBinary(strValue.pszBuffer(), bRes);
    if (bRes)
        bRes = (i4Val >= kCIDLib::i1MinInt) && (i4Val <= kCIDLib::i1MaxInt);
    if (bRes)
        m_i1Value = tCIDLib::TInt1(i4Val);
    return bRes;
}


tCIDLib::TVoid
TMEngInt1Val::CopyFrom( const   TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    m_i1Value = static_cast<const TMEngInt1Val&>(mecvToCopy).m_i1Value;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngInt1Info
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngInt1Info: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngInt1Info::strPath()
{
    return CIDMacroEng_IntClasses::strInt1ClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngInt1Info: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngInt1Info::TMEngInt1Info(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_IntClasses::strInt1
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2MethId_AbsValue(kMacroEng::c2BadId)
    , m_c2MethId_Add(kMacroEng::c2BadId)
    , m_c2MethId_Dec(kMacroEng::c2BadId)
    , m_c2MethId_Div(kMacroEng::c2BadId)
    , m_c2MethId_DivEq(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Equal(kMacroEng::c2BadId)
    , m_c2MethId_GetNegated(kMacroEng::c2BadId)
    , m_c2MethId_GtThan(kMacroEng::c2BadId)
    , m_c2MethId_GtThanEq(kMacroEng::c2BadId)
    , m_c2MethId_Inc(kMacroEng::c2BadId)
    , m_c2MethId_LsThan(kMacroEng::c2BadId)
    , m_c2MethId_LsThanEq(kMacroEng::c2BadId)
    , m_c2MethId_MaxVal(kMacroEng::c2BadId)
    , m_c2MethId_MinVal(kMacroEng::c2BadId)
    , m_c2MethId_MinusEq(kMacroEng::c2BadId)
    , m_c2MethId_ModDiv(kMacroEng::c2BadId)
    , m_c2MethId_ModDivEq(kMacroEng::c2BadId)
    , m_c2MethId_Mul(kMacroEng::c2BadId)
    , m_c2MethId_MulEq(kMacroEng::c2BadId)
    , m_c2MethId_Negate(kMacroEng::c2BadId)
    , m_c2MethId_PlusEq(kMacroEng::c2BadId)
    , m_c2MethId_Sub(kMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kMacroEng::c2BadId)
{
}

TMEngInt1Info::~TMEngInt1Info()
{
}


// ---------------------------------------------------------------------------
//  TMEngInt1Info: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngInt1Info::bCanCastFrom(const tCIDLib::TCard2 c2SrcId)
{
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
        case tCIDMacroEng::EIntrinsics::Card1 :
        case tCIDMacroEng::EIntrinsics::Card2 :
        case tCIDMacroEng::EIntrinsics::Card4 :
        case tCIDMacroEng::EIntrinsics::Int1 :
        case tCIDMacroEng::EIntrinsics::Int2 :
        case tCIDMacroEng::EIntrinsics::Int4 :
        case tCIDMacroEng::EIntrinsics::Float4 :
        case tCIDMacroEng::EIntrinsics::Float8 :
            break;

        default :
            return kCIDLib::False;
    };
    return kCIDLib::True;
}


tCIDMacroEng::ECastRes
TMEngInt1Info::eCastFrom(       TCIDMacroEngine&
                        , const TMEngClassVal&      mecvSrc
                        ,       TMEngClassVal&      mecvTarget)
{
    // Get the source value
    const tCIDLib::TCard2 c2SrcId = mecvSrc.c2ClassId();

    tCIDLib::TInt1 i1New;
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
            if (static_cast<const TMEngBooleanVal&>(mecvSrc).bValue())
                i1New = 1;
            else
                i1New = 0;
            break;

        case tCIDMacroEng::EIntrinsics::Card1 :
            i1New = tCIDLib::TInt1(static_cast<const TMEngCard1Val&>(mecvSrc).c1Value());
            break;

        case tCIDMacroEng::EIntrinsics::Card2 :
            i1New = tCIDLib::TInt1(static_cast<const TMEngCard2Val&>(mecvSrc).c2Value());
            break;

        case tCIDMacroEng::EIntrinsics::Card4 :
            i1New = tCIDLib::TInt1(static_cast<const TMEngCard4Val&>(mecvSrc).c4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int1 :
            i1New = static_cast<const TMEngInt1Val&>(mecvSrc).i1Value();
            break;

        case tCIDMacroEng::EIntrinsics::Int2 :
            i1New = tCIDLib::TInt1(static_cast<const TMEngInt2Val&>(mecvSrc).i2Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int4 :
            i1New = tCIDLib::TInt1(static_cast<const TMEngInt4Val&>(mecvSrc).i4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Float4 :
            i1New = tCIDLib::TInt1(static_cast<const TMEngFloat4Val&>(mecvSrc).f4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Float8 :
            i1New = tCIDLib::TInt1(static_cast<const TMEngFloat8Val&>(mecvSrc).f8Value());
            break;

        default :
            return tCIDMacroEng::ECastRes::Incompatible;
    };

    // Cast the target to our type, which it must be
    TMEngInt1Val& mecvActual = static_cast<TMEngInt1Val&>(mecvTarget);
    mecvActual.i1Value(i1New);
    return tCIDMacroEng::ECastRes::Ok;
}


tCIDLib::TVoid TMEngInt1Info::Init(TCIDMacroEngine&)
{
    // Add a literal for our max value
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kMaxValue"
            , tCIDMacroEng::EIntrinsics::Int1
            , new TMEngInt1Val(L"kMaxValue", tCIDMacroEng::EConstTypes::Const, kCIDLib::i1MaxInt)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kMinValue"
            , tCIDMacroEng::EIntrinsics::Int1
            , new TMEngInt1Val(L"kMinValue", tCIDMacroEng::EConstTypes::Const, kCIDLib::i1MinInt)
        )
    );

    // Add the AbsVal method
    {
        TMEngMethodInfo methiNew
        (
            L"AbsValue"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_AbsValue = c2AddMethodInfo(methiNew);
    }

    // Add the add method
    {
        TMEngMethodInfo methiNew
        (
            L"Add"
            , tCIDMacroEng::EIntrinsics::Int1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_Add = c2AddMethodInfo(methiNew);
    }

    // Add the decrement method
    {
        TMEngMethodInfo methiNew
        (
            L"Dec"
            , tCIDMacroEng::EIntrinsics::Int1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Dec = c2AddMethodInfo(methiNew);
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.Int1"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the div and diveq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Div"
            , tCIDMacroEng::EIntrinsics::Int1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"DivBy", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_Div = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"DivEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_DivEq = c2AddMethodInfo(methiNew);
    }

    // Add the equal method
    {
        TMEngMethodInfo methiNew
        (
            L"Equal"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_Equal = c2AddMethodInfo(methiNew);
    }

    // Return a negated version of the value
    {
        TMEngMethodInfo methiNew
        (
            L"GetNegated"
            , tCIDMacroEng::EIntrinsics::Int1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetNegated = c2AddMethodInfo(methiNew);
    }

    // Add the greater than and greater/equal methods
    {
        TMEngMethodInfo methiNew
        (
            L"GtThan"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_GtThan = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GtThanEq"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_GtThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the increment method
    {
        TMEngMethodInfo methiNew
        (
            L"Inc"
            , tCIDMacroEng::EIntrinsics::Int1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Inc = c2AddMethodInfo(methiNew);
    }

    // Add the less than and less/equal methods
    {
        TMEngMethodInfo methiNew
        (
            L"LsThan"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_LsThan = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"LsThanEq"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_LsThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the max/min value methods
    {
        TMEngMethodInfo methiNew
        (
            L"MaxVal"
            , tCIDMacroEng::EIntrinsics::Int1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_MaxVal = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"MinVal"
            , tCIDMacroEng::EIntrinsics::Int1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_MinVal = c2AddMethodInfo(methiNew);
    }

    // Add the minuseq method
    {
        TMEngMethodInfo methiNew
        (
            L"MinusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_MinusEq = c2AddMethodInfo(methiNew);
    }

    // Add the moddiv and moddiveq methods
    {
        TMEngMethodInfo methiNew
        (
            L"ModDiv"
            , tCIDMacroEng::EIntrinsics::Int1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Divisor", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_ModDiv = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ModDivEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Divisor", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_ModDivEq = c2AddMethodInfo(methiNew);
    }

    // Add the mul and muleq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Mul"
            , tCIDMacroEng::EIntrinsics::Int1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"MulBy", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_Mul = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"MulEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_MulEq = c2AddMethodInfo(methiNew);
    }

    // The Negate method
    {
        TMEngMethodInfo methiNew
        (
            L"Negate"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Negate = c2AddMethodInfo(methiNew);
    }

    // The pluseq method
    {
        TMEngMethodInfo methiNew
        (
            L"PlusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_PlusEq = c2AddMethodInfo(methiNew);
    }

    // Add the subtract method
    {
        TMEngMethodInfo methiNew
        (
            L"Sub"
            , tCIDMacroEng::EIntrinsics::Int1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_Sub = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that takes an initial value
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.Int1"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        methiNew.c2AddInParm(L"InitVal", tCIDMacroEng::EIntrinsics::Int1);
        m_c2MethId_ValCtor = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngInt1Info::pmecvMakeStorage(const   TString&                strName
                                ,       TCIDMacroEngine&
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngInt1Val(strName, eConst);
}


// ---------------------------------------------------------------------------
//  TMEngInt1Info: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngInt1Info::bInvokeMethod(       TCIDMacroEngine&    meOwner
                            , const TMEngMethodInfo&    methiTarget
                            ,       TMEngClassVal&      mecvInstance)
{
    TMEngInt1Val& mecvActual = static_cast<TMEngInt1Val&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_AbsValue)
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);

        tCIDLib::TInt1 i1Val = mecvActual.i1Value();
        if (i1Val < 0)
            i1Val *= -1;
        mecvRet.c1Value(tCIDLib::TCard1(i1Val));
    }
     else if (c2MethId == m_c2MethId_Add)
    {
        TMEngInt1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt1Val>(c4FirstInd - 1);
        mecvRet.i1Value(mecvActual.i1Value() + meOwner.i1StackValAt(c4FirstInd));
    }
     else if (methiTarget.c2Id() == m_c2MethId_DefCtor)
    {
        mecvActual.i1Value(0);
    }
     else if (c2MethId == m_c2MethId_Dec)
    {
        const tCIDLib::TInt1 i1Cur = mecvActual.i1Value();
        TMEngInt1Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngInt1Val>(c4FirstInd - 1);
        mecvRet.i1Value(i1Cur);
        if (i1Cur > kCIDLib::i1MinInt)
            mecvActual.i1Value(i1Cur - 1);
    }
     else if (c2MethId == m_c2MethId_Div)
    {
        TMEngInt1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt1Val>(c4FirstInd - 1);
        mecvRet.i1Value(mecvActual.i1Value() / meOwner.i1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_DivEq)
    {
        mecvActual.i1Value(mecvActual.i1Value() / meOwner.i1StackValAt(c4FirstInd));
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner) << mecvActual.i1Value();
    }
     else if (c2MethId == m_c2MethId_GetNegated)
    {
        TMEngInt1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt1Val>(c4FirstInd - 1);
        mecvRet.i1Value(mecvActual.i1Value() * -1);
    }
     else if ((c2MethId == m_c2MethId_Equal)
          ||  (c2MethId == m_c2MethId_GtThan)
          ||  (c2MethId == m_c2MethId_GtThanEq)
          ||  (c2MethId == m_c2MethId_LsThan)
          ||  (c2MethId == m_c2MethId_LsThanEq))
    {
        //
        //  These set the boolean return value, based on our relation
        //  to a passed card4 value.
        //
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        const tCIDLib::TInt1 i1Parm = meOwner.i1StackValAt(c4FirstInd);

        if (c2MethId == m_c2MethId_GtThan)
            mecvRet.bValue(mecvActual.i1Value() > i1Parm);
        else if (c2MethId == m_c2MethId_GtThanEq)
            mecvRet.bValue(mecvActual.i1Value() >= i1Parm);
        else if (c2MethId == m_c2MethId_LsThan)
            mecvRet.bValue(mecvActual.i1Value() < i1Parm);
        else if (c2MethId == m_c2MethId_LsThanEq)
            mecvRet.bValue(mecvActual.i1Value() <= i1Parm);
        else
            mecvRet.bValue(mecvActual.i1Value() == i1Parm);
    }
     else if (c2MethId == m_c2MethId_Inc)
    {
        const tCIDLib::TInt1 i1Cur = mecvActual.i1Value();
        TMEngInt1Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngInt1Val>(c4FirstInd - 1);
        mecvRet.i1Value(i1Cur);
        if (i1Cur < kCIDLib::i1MaxInt)
            mecvActual.i1Value(i1Cur + 1);
    }
     else if (c2MethId == m_c2MethId_MaxVal)
    {
        const tCIDLib::TInt1 i1Comp = meOwner.i1StackValAt(c4FirstInd);
        TMEngInt1Val& mecvRet = meOwner.mecvStackAtAs<TMEngInt1Val>(c4FirstInd - 1);
        if (i1Comp > mecvActual.i1Value())
            mecvRet.i1Value(i1Comp);
        else
            mecvRet.i1Value(mecvActual.i1Value());
    }
     else if (c2MethId == m_c2MethId_MinVal)
    {
        const tCIDLib::TInt1 i1Comp = meOwner.i1StackValAt(c4FirstInd);
        TMEngInt1Val& mecvRet = meOwner.mecvStackAtAs<TMEngInt1Val>(c4FirstInd - 1);
        if (i1Comp < mecvActual.i1Value())
            mecvRet.i1Value(i1Comp);
        else
            mecvRet.i1Value(mecvActual.i1Value());
    }
     else if (c2MethId == m_c2MethId_MinusEq)
    {
        const tCIDLib::TInt1 i1Sub = meOwner.i1StackValAt(c4FirstInd);
        mecvActual.i1Value(mecvActual.i1Value() - i1Sub);
    }
     else if (c2MethId == m_c2MethId_ModDiv)
    {
        TMEngInt1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt1Val>(c4FirstInd - 1);
        mecvRet.i1Value(mecvActual.i1Value() % meOwner.i1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ModDivEq)
    {
        mecvActual.i1Value(mecvActual.i1Value() % meOwner.i1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Mul)
    {
        TMEngInt1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt1Val>(c4FirstInd - 1);
        mecvRet.i1Value(mecvActual.i1Value() * meOwner.i1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_MulEq)
    {
        mecvActual.i1Value(mecvActual.i1Value() * meOwner.i1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Negate)
    {
        mecvActual.i1Value(mecvActual.i1Value() * -1);
    }
     else if (c2MethId == m_c2MethId_PlusEq)
    {
        const tCIDLib::TInt1 i1Add = meOwner.i1StackValAt(c4FirstInd);
        mecvActual.i1Value(mecvActual.i1Value() + i1Add);
    }
     else if (c2MethId == m_c2MethId_Sub)
    {
        TMEngInt1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt1Val>(c4FirstInd - 1);
        mecvRet.i1Value(mecvActual.i1Value() - meOwner.i1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        mecvActual.i1Value(meOwner.i1StackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngInt2Val
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngInt2Val: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngInt2Val::TMEngInt2Val( const   TString&                strName
                            , const tCIDMacroEng::EConstTypes  eConst
                            , const tCIDLib::TInt2          i2Init) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int2), eConst)
    , m_i2Value(i2Init)
{
}

TMEngInt2Val::~TMEngInt2Val()
{
}


// ---------------------------------------------------------------------------
//  TMEngInt2Val: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TInt2 TMEngInt2Val::i2Value() const
{
    return m_i2Value;
}

tCIDLib::TInt2 TMEngInt2Val::i2Value(const tCIDLib::TInt2 i2ToSet)
{
    m_i2Value = i2ToSet;
    return m_i2Value;
}


// ---------------------------------------------------------------------------
//  TMEngInt2Val: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngInt2Val::bDbgFormat(       TTextOutStream&     strmTarget
                        , const TMEngClassInfo&
                        , const tCIDMacroEng::EDbgFmts eFormat
                        , const tCIDLib::ERadices   eRadix
                        , const TCIDMacroEngine&    ) const
{
    strmTarget << TInteger(m_i2Value, eRadix);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMEngInt2Val::bParseFromText(const TString& strValue, const TMEngClassInfo&)
{
    tCIDLib::TBoolean bRes;
    tCIDLib::TInt4 i4Val = TRawStr::i4AsBinary(strValue.pszBuffer(), bRes);
    if (bRes)
        bRes = (i4Val >= kCIDLib::i2MinInt) && (i4Val <= kCIDLib::i2MaxInt);
    if (bRes)
        m_i2Value = tCIDLib::TInt2(i4Val);
    return bRes;
}


tCIDLib::TVoid
TMEngInt2Val::CopyFrom( const   TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    m_i2Value = static_cast<const TMEngInt2Val&>(mecvToCopy).m_i2Value;
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngInt2Info
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngInt2Info: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngInt2Info::strPath()
{
    return CIDMacroEng_IntClasses::strInt2ClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngInt2Info: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngInt2Info::TMEngInt2Info(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_IntClasses::strInt2
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2MethId_AbsValue(kMacroEng::c2BadId)
    , m_c2MethId_Add(kMacroEng::c2BadId)
    , m_c2MethId_Dec(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Div(kMacroEng::c2BadId)
    , m_c2MethId_DivEq(kMacroEng::c2BadId)
    , m_c2MethId_Equal(kMacroEng::c2BadId)
    , m_c2MethId_GetNegated(kMacroEng::c2BadId)
    , m_c2MethId_GtThan(kMacroEng::c2BadId)
    , m_c2MethId_GtThanEq(kMacroEng::c2BadId)
    , m_c2MethId_Inc(kMacroEng::c2BadId)
    , m_c2MethId_LsThan(kMacroEng::c2BadId)
    , m_c2MethId_LsThanEq(kMacroEng::c2BadId)
    , m_c2MethId_MaxVal(kMacroEng::c2BadId)
    , m_c2MethId_MinVal(kMacroEng::c2BadId)
    , m_c2MethId_MinusEq(kMacroEng::c2BadId)
    , m_c2MethId_ModDiv(kMacroEng::c2BadId)
    , m_c2MethId_ModDivEq(kMacroEng::c2BadId)
    , m_c2MethId_Mul(kMacroEng::c2BadId)
    , m_c2MethId_MulEq(kMacroEng::c2BadId)
    , m_c2MethId_Negate(kMacroEng::c2BadId)
    , m_c2MethId_PlusEq(kMacroEng::c2BadId)
    , m_c2MethId_Sub(kMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kMacroEng::c2BadId)
{
}

TMEngInt2Info::~TMEngInt2Info()
{
}


// ---------------------------------------------------------------------------
//  TMEngInt2Info: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngInt2Info::bCanCastFrom(const tCIDLib::TCard2 c2SrcId)
{
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
        case tCIDMacroEng::EIntrinsics::Card1 :
        case tCIDMacroEng::EIntrinsics::Card2 :
        case tCIDMacroEng::EIntrinsics::Card4 :
        case tCIDMacroEng::EIntrinsics::Int1 :
        case tCIDMacroEng::EIntrinsics::Int2 :
        case tCIDMacroEng::EIntrinsics::Int4 :
        case tCIDMacroEng::EIntrinsics::Float4 :
        case tCIDMacroEng::EIntrinsics::Float8 :
            break;

        default :
            return kCIDLib::False;
    };
    return kCIDLib::True;
}


tCIDMacroEng::ECastRes
TMEngInt2Info::eCastFrom(       TCIDMacroEngine&
                        , const TMEngClassVal&      mecvSrc
                        ,       TMEngClassVal&      mecvTarget)
{
    // Get the source value
    const tCIDLib::TCard2 c2SrcId = mecvSrc.c2ClassId();

    tCIDLib::TInt2 i2New;
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
            if (static_cast<const TMEngBooleanVal&>(mecvSrc).bValue())
                i2New = 1;
            else
                i2New = 0;
            break;

        case tCIDMacroEng::EIntrinsics::Card1 :
            i2New = static_cast<const TMEngCard1Val&>(mecvSrc).c1Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card2 :
            i2New = tCIDLib::TInt2(static_cast<const TMEngCard2Val&>(mecvSrc).c2Value());
            break;

        case tCIDMacroEng::EIntrinsics::Card4 :
            i2New = tCIDLib::TInt2(static_cast<const TMEngCard4Val&>(mecvSrc).c4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int1 :
            i2New = static_cast<const TMEngInt1Val&>(mecvSrc).i1Value();
            break;

        case tCIDMacroEng::EIntrinsics::Int2 :
            i2New = static_cast<const TMEngInt2Val&>(mecvSrc).i2Value();
            break;

        case tCIDMacroEng::EIntrinsics::Int4 :
            i2New = tCIDLib::TInt2(static_cast<const TMEngInt4Val&>(mecvSrc).i4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Float4 :
            i2New = tCIDLib::TInt2(static_cast<const TMEngFloat4Val&>(mecvSrc).f4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Float8 :
            i2New = tCIDLib::TInt2(static_cast<const TMEngFloat8Val&>(mecvSrc).f8Value());
            break;

        default :
            return tCIDMacroEng::ECastRes::Incompatible;
    };

    // Cast the target to our type, which it must be
    TMEngInt2Val& mecvActual = static_cast<TMEngInt2Val&>(mecvTarget);
    mecvActual.i2Value(i2New);
    return tCIDMacroEng::ECastRes::Ok;
}


tCIDLib::TVoid TMEngInt2Info::Init(TCIDMacroEngine&)
{
    // Add a literal for our max value
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kMaxValue"
            , tCIDMacroEng::EIntrinsics::Int2
            , new TMEngInt2Val(L"kMaxValue", tCIDMacroEng::EConstTypes::Const, kCIDLib::i2MaxInt)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kMinValue"
            , tCIDMacroEng::EIntrinsics::Int2
            , new TMEngInt2Val(L"kMinValue", tCIDMacroEng::EConstTypes::Const, kCIDLib::i2MinInt)
        )
    );

    // Add the AbsVal method
    {
        TMEngMethodInfo methiNew
        (
            L"AbsValue"
            , tCIDMacroEng::EIntrinsics::Card2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_AbsValue = c2AddMethodInfo(methiNew);
    }

    // Add the add method
    {
        TMEngMethodInfo methiNew
        (
            L"Add"
            , tCIDMacroEng::EIntrinsics::Int2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_Add = c2AddMethodInfo(methiNew);
    }

    // Add the decrement method
    {
        TMEngMethodInfo methiNew
        (
            L"Dec"
            , tCIDMacroEng::EIntrinsics::Int2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Dec = c2AddMethodInfo(methiNew);
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.Int2"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the div and diveq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Div"
            , tCIDMacroEng::EIntrinsics::Int2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_Div = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"DivEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_DivEq = c2AddMethodInfo(methiNew);
    }

    // Add the equal method
    {
        TMEngMethodInfo methiNew
        (
            L"Equal"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_Equal = c2AddMethodInfo(methiNew);
    }

    // Return a negated version of the value
    {
        TMEngMethodInfo methiNew
        (
            L"GetNegated"
            , tCIDMacroEng::EIntrinsics::Int2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetNegated = c2AddMethodInfo(methiNew);
    }

    // Add the greater than and greater/equal methods
    {
        TMEngMethodInfo methiNew
        (
            L"GtThan"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_GtThan = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GtThanEq"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_GtThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the increment method
    {
        TMEngMethodInfo methiNew
        (
            L"Inc"
            , tCIDMacroEng::EIntrinsics::Int2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Inc = c2AddMethodInfo(methiNew);
    }

    // Add the less than and less/equal methods
    {
        TMEngMethodInfo methiNew
        (
            L"LsThan"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_LsThan = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"LsThanEq"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_LsThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the max/min value methods
    {
        TMEngMethodInfo methiNew
        (
            L"MaxVal"
            , tCIDMacroEng::EIntrinsics::Int2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_MaxVal = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"MinVal"
            , tCIDMacroEng::EIntrinsics::Int2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_MinVal = c2AddMethodInfo(methiNew);
    }

    // Add the minuseq method
    {
        TMEngMethodInfo methiNew
        (
            L"MinusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_MinusEq = c2AddMethodInfo(methiNew);
    }

    // Add the moddiv and moddiveq methods
    {
        TMEngMethodInfo methiNew
        (
            L"ModDiv"
            , tCIDMacroEng::EIntrinsics::Int2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Divisor", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_ModDiv = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ModDivEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Divisor", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_ModDivEq = c2AddMethodInfo(methiNew);
    }

    // Add the mul and muleq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Mul"
            , tCIDMacroEng::EIntrinsics::Int2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_Mul = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"MulEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_MulEq = c2AddMethodInfo(methiNew);
    }

    // The Negate method
    {
        TMEngMethodInfo methiNew
        (
            L"Negate"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Negate = c2AddMethodInfo(methiNew);
    }

    // The pluseq method
    {
        TMEngMethodInfo methiNew
        (
            L"PlusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_PlusEq = c2AddMethodInfo(methiNew);
    }

    // Add the subtract method
    {
        TMEngMethodInfo methiNew
        (
            L"Sub"
            , tCIDMacroEng::EIntrinsics::Int2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_Sub = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that takes an initial value
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.Int2"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        methiNew.c2AddInParm(L"InitVal", tCIDMacroEng::EIntrinsics::Int2);
        m_c2MethId_ValCtor = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngInt2Info::pmecvMakeStorage(const   TString&                strName
                                ,       TCIDMacroEngine&
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngInt2Val(strName, eConst);
}


// ---------------------------------------------------------------------------
//  TMEngInt2Info: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngInt2Info::bInvokeMethod(       TCIDMacroEngine&    meOwner
                            , const TMEngMethodInfo&    methiTarget
                            ,       TMEngClassVal&      mecvInstance)
{
    TMEngInt2Val& mecvActual = static_cast<TMEngInt2Val&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_AbsValue)
    {
        TMEngCard2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);

        tCIDLib::TInt2 i2Val = mecvActual.i2Value();
        if (i2Val < 0)
            i2Val *= -1;
        mecvRet.c2Value(tCIDLib::TCard2(i2Val));
    }
     else if (c2MethId == m_c2MethId_Add)
    {
        TMEngInt2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt2Val>(c4FirstInd - 1);
        mecvRet.i2Value(mecvActual.i2Value() + meOwner.i2StackValAt(c4FirstInd));
    }
     else if (methiTarget.c2Id() == m_c2MethId_DefCtor)
    {
        mecvActual.i2Value(0);
    }
     else if (c2MethId == m_c2MethId_Dec)
    {
        const tCIDLib::TInt2 i2Cur = mecvActual.i2Value();
        TMEngInt2Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngInt2Val>(c4FirstInd - 1);
        mecvRet.i2Value(i2Cur);
        if (i2Cur > kCIDLib::i2MinInt)
            mecvActual.i2Value(i2Cur - 1);
    }
     else if (c2MethId == m_c2MethId_Div)
    {
        TMEngInt2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt2Val>(c4FirstInd - 1);
        mecvRet.i2Value(mecvActual.i2Value() / meOwner.i2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_DivEq)
    {
        mecvActual.i2Value(mecvActual.i2Value() / meOwner.i2StackValAt(c4FirstInd));
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner) << mecvActual.i2Value();
    }
     else if (c2MethId == m_c2MethId_GetNegated)
    {
        TMEngInt2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt2Val>(c4FirstInd - 1);
        mecvRet.i2Value(mecvActual.i2Value() * -1);
    }
     else if ((c2MethId == m_c2MethId_Equal)
          ||  (c2MethId == m_c2MethId_GtThan)
          ||  (c2MethId == m_c2MethId_GtThanEq)
          ||  (c2MethId == m_c2MethId_LsThan)
          ||  (c2MethId == m_c2MethId_LsThanEq))
    {
        //
        //  These set the boolean return value, based on our relation
        //  to a passed card4 value.
        //
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        const tCIDLib::TInt2 i2Parm = meOwner.i2StackValAt(c4FirstInd);

        if (c2MethId == m_c2MethId_GtThan)
            mecvRet.bValue(mecvActual.i2Value() > i2Parm);
        else if (c2MethId == m_c2MethId_GtThanEq)
            mecvRet.bValue(mecvActual.i2Value() >= i2Parm);
        else if (c2MethId == m_c2MethId_LsThan)
            mecvRet.bValue(mecvActual.i2Value() < i2Parm);
        else if (c2MethId == m_c2MethId_LsThanEq)
            mecvRet.bValue(mecvActual.i2Value() <= i2Parm);
        else
            mecvRet.bValue(mecvActual.i2Value() == i2Parm);
    }
     else if (c2MethId == m_c2MethId_Inc)
    {
        const tCIDLib::TInt2 i2Cur = mecvActual.i2Value();
        TMEngInt2Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngInt2Val>(c4FirstInd - 1);
        mecvRet.i2Value(i2Cur);
        if (i2Cur < kCIDLib::i2MaxInt)
            mecvActual.i2Value(i2Cur + 1);
    }
     else if (c2MethId == m_c2MethId_MaxVal)
    {
        const tCIDLib::TInt2 i2Comp = meOwner.i2StackValAt(c4FirstInd);
        TMEngInt2Val& mecvRet = meOwner.mecvStackAtAs<TMEngInt2Val>(c4FirstInd - 1);
        if (i2Comp > mecvActual.i2Value())
            mecvRet.i2Value(i2Comp);
        else
            mecvRet.i2Value(mecvActual.i2Value());
    }
     else if (c2MethId == m_c2MethId_MinVal)
    {
        const tCIDLib::TInt2 i2Comp = meOwner.i2StackValAt(c4FirstInd);
        TMEngInt2Val& mecvRet = meOwner.mecvStackAtAs<TMEngInt2Val>(c4FirstInd - 1);
        if (i2Comp < mecvActual.i2Value())
            mecvRet.i2Value(i2Comp);
        else
            mecvRet.i2Value(mecvActual.i2Value());
    }
     else if (c2MethId == m_c2MethId_MinusEq)
    {
        const tCIDLib::TInt2 i2Sub = meOwner.i2StackValAt(c4FirstInd);
        mecvActual.i2Value(mecvActual.i2Value() - i2Sub);
    }
     else if (c2MethId == m_c2MethId_ModDiv)
    {
        TMEngInt2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt2Val>(c4FirstInd - 1);
        mecvRet.i2Value(mecvActual.i2Value() % meOwner.i2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ModDivEq)
    {
        mecvActual.i2Value(mecvActual.i2Value() % meOwner.i2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Mul)
    {
        TMEngInt2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt2Val>(c4FirstInd - 1);
        mecvRet.i2Value(mecvActual.i2Value() * meOwner.i2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_MulEq)
    {
        mecvActual.i2Value(mecvActual.i2Value() * meOwner.i2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Negate)
    {
        mecvActual.i2Value(mecvActual.i2Value() * -1);
    }
     else if (c2MethId == m_c2MethId_PlusEq)
    {
        const tCIDLib::TInt2 i2Add = meOwner.i2StackValAt(c4FirstInd);
        mecvActual.i2Value(mecvActual.i2Value() + i2Add);
    }
     else if (c2MethId == m_c2MethId_Sub)
    {
        TMEngInt2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt2Val>(c4FirstInd - 1);
        mecvRet.i2Value(mecvActual.i2Value() - meOwner.i2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        mecvActual.i2Value(meOwner.i2StackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngInt4Val
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngInt4Val: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngInt4Val::TMEngInt4Val( const   TString&                strName
                            , const tCIDMacroEng::EConstTypes  eConst
                            , const tCIDLib::TInt4          i4Init) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Int4), eConst)
    , m_i4Value(i4Init)
{
}

TMEngInt4Val::~TMEngInt4Val()
{
}


// ---------------------------------------------------------------------------
//  TMEngInt4Val: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TInt4 TMEngInt4Val::i4Value() const
{
    return m_i4Value;
}

tCIDLib::TInt4 TMEngInt4Val::i4Value(const tCIDLib::TInt4 i4ToSet)
{
    m_i4Value = i4ToSet;
    return m_i4Value;
}


// ---------------------------------------------------------------------------
//  TMEngInt4Val: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngInt4Val::bDbgFormat(       TTextOutStream&     strmTarget
                        , const TMEngClassInfo&
                        , const tCIDMacroEng::EDbgFmts eFormat
                        , const tCIDLib::ERadices   eRadix
                        , const TCIDMacroEngine&    ) const
{
    strmTarget << TInteger(m_i4Value, eRadix);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMEngInt4Val::bParseFromText(const TString& strValue, const TMEngClassInfo&)
{
    tCIDLib::TBoolean bRes;
    tCIDLib::TInt4 i4Val = TRawStr::i4AsBinary(strValue.pszBuffer(), bRes);
    if (bRes)
        m_i4Value = i4Val;
    return bRes;
}


tCIDLib::TVoid
TMEngInt4Val::CopyFrom( const   TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    m_i4Value = static_cast<const TMEngInt4Val&>(mecvToCopy).m_i4Value;
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngInt4Info
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngInt4Info: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngInt4Info::strPath()
{
    return CIDMacroEng_IntClasses::strInt4ClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngInt4Info: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngInt4Info::TMEngInt4Info(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_IntClasses::strInt4
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2MethId_AbsValue(kMacroEng::c2BadId)
    , m_c2MethId_Add(kMacroEng::c2BadId)
    , m_c2MethId_Dec(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Div(kMacroEng::c2BadId)
    , m_c2MethId_DivEq(kMacroEng::c2BadId)
    , m_c2MethId_Equal(kMacroEng::c2BadId)
    , m_c2MethId_GetNegated(kMacroEng::c2BadId)
    , m_c2MethId_GtThan(kMacroEng::c2BadId)
    , m_c2MethId_GtThanEq(kMacroEng::c2BadId)
    , m_c2MethId_Inc(kMacroEng::c2BadId)
    , m_c2MethId_LsThan(kMacroEng::c2BadId)
    , m_c2MethId_LsThanEq(kMacroEng::c2BadId)
    , m_c2MethId_MaxVal(kMacroEng::c2BadId)
    , m_c2MethId_MinVal(kMacroEng::c2BadId)
    , m_c2MethId_MinusEq(kMacroEng::c2BadId)
    , m_c2MethId_ModDiv(kMacroEng::c2BadId)
    , m_c2MethId_ModDivEq(kMacroEng::c2BadId)
    , m_c2MethId_Mul(kMacroEng::c2BadId)
    , m_c2MethId_MulEq(kMacroEng::c2BadId)
    , m_c2MethId_Negate(kMacroEng::c2BadId)
    , m_c2MethId_PlusEq(kMacroEng::c2BadId)
    , m_c2MethId_Sub(kMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kMacroEng::c2BadId)
{
}

TMEngInt4Info::~TMEngInt4Info()
{
}


// ---------------------------------------------------------------------------
//  Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngInt4Info::bCanCastFrom(const tCIDLib::TCard2 c2SrcId)
{
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
        case tCIDMacroEng::EIntrinsics::Card1 :
        case tCIDMacroEng::EIntrinsics::Card2 :
        case tCIDMacroEng::EIntrinsics::Card4 :
        case tCIDMacroEng::EIntrinsics::Int1 :
        case tCIDMacroEng::EIntrinsics::Int2 :
        case tCIDMacroEng::EIntrinsics::Int4 :
        case tCIDMacroEng::EIntrinsics::Float4 :
        case tCIDMacroEng::EIntrinsics::Float8 :
            break;

        default :
            return kCIDLib::False;
    };
    return kCIDLib::True;
}


tCIDMacroEng::ECastRes
TMEngInt4Info::eCastFrom(       TCIDMacroEngine&    meOwner
                        , const TMEngClassVal&      mecvSrc
                        ,       TMEngClassVal&      mecvTarget)
{
    // Get the source value
    const tCIDLib::TCard2 c2SrcId = mecvSrc.c2ClassId();

    tCIDLib::TInt4 i4New;
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
            if (static_cast<const TMEngBooleanVal&>(mecvSrc).bValue())
                i4New = 1;
            else
                i4New = 0;
            break;

        case tCIDMacroEng::EIntrinsics::Card1 :
            i4New = static_cast<const TMEngCard1Val&>(mecvSrc).c1Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card2 :
            i4New = static_cast<const TMEngCard2Val&>(mecvSrc).c2Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card4 :
            i4New = tCIDLib::TInt4(static_cast<const TMEngCard4Val&>(mecvSrc).c4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int1 :
            i4New = static_cast<const TMEngInt1Val&>(mecvSrc).i1Value();
            break;

        case tCIDMacroEng::EIntrinsics::Int2 :
            i4New = static_cast<const TMEngInt2Val&>(mecvSrc).i2Value();
            break;

        case tCIDMacroEng::EIntrinsics::Int4 :
            i4New = static_cast<const TMEngInt4Val&>(mecvSrc).i4Value();
            break;

        case tCIDMacroEng::EIntrinsics::Float4 :
            i4New = tCIDLib::TInt4(static_cast<const TMEngFloat4Val&>(mecvSrc).f4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Float8 :
            i4New = tCIDLib::TInt4(static_cast<const TMEngFloat8Val&>(mecvSrc).f8Value());
            break;

        default :
            return tCIDMacroEng::ECastRes::Incompatible;
    };

    // Cast the target to our type, which it must be
    TMEngInt4Val& mecvActual = static_cast<TMEngInt4Val&>(mecvTarget);
    mecvActual.i4Value(i4New);
    return tCIDMacroEng::ECastRes::Ok;
}


tCIDLib::TVoid TMEngInt4Info::Init(TCIDMacroEngine&)
{
    // Add a literal for our max value
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kMaxValue"
            , tCIDMacroEng::EIntrinsics::Int4
            , new TMEngInt4Val(L"kMaxValue", tCIDMacroEng::EConstTypes::Const, kCIDLib::i4MaxInt)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kMinValue"
            , tCIDMacroEng::EIntrinsics::Int4
            , new TMEngInt4Val(L"kMinValue", tCIDMacroEng::EConstTypes::Const, kCIDLib::i4MinInt)
        )
    );


    // Add the AbsVal method
    {
        TMEngMethodInfo methiNew
        (
            L"AbsValue"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_AbsValue = c2AddMethodInfo(methiNew);
    }

    // Add the add method
    {
        TMEngMethodInfo methiNew
        (
            L"Add"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_Add = c2AddMethodInfo(methiNew);
    }

    // Add the decrement method
    {
        TMEngMethodInfo methiNew
        (
            L"Dec"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Dec = c2AddMethodInfo(methiNew);
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.Int4"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the div and diveq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Div"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_Div = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"DivEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_DivEq = c2AddMethodInfo(methiNew);
    }

    // Add the equal method
    {
        TMEngMethodInfo methiNew
        (
            L"Equal"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_Equal = c2AddMethodInfo(methiNew);
    }

    // Return a negated version of the value
    {
        TMEngMethodInfo methiNew
        (
            L"GetNegated"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetNegated = c2AddMethodInfo(methiNew);
    }

    // Add the greater than and greater/equal methods
    {
        TMEngMethodInfo methiNew
        (
            L"GtThan"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_GtThan = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GtThanEq"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_GtThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the increment method
    {
        TMEngMethodInfo methiNew
        (
            L"Inc"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Inc = c2AddMethodInfo(methiNew);
    }

    // Add the less than and less/equal methods
    {
        TMEngMethodInfo methiNew
        (
            L"LsThan"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_LsThan = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"LsThanEq"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_LsThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the max/min value methods
    {
        TMEngMethodInfo methiNew
        (
            L"MaxVal"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_MaxVal = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"MinVal"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_MinVal = c2AddMethodInfo(methiNew);
    }

    // Add the minueq method
    {
        TMEngMethodInfo methiNew
        (
            L"MinusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_MinusEq = c2AddMethodInfo(methiNew);
    }

    // Add the moddiv and moddiveq methods
    {
        TMEngMethodInfo methiNew
        (
            L"ModDiv"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Divisor", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_ModDiv = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ModDivEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Divisor", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_ModDivEq = c2AddMethodInfo(methiNew);
    }

    // Add the mul and muleq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Mul"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_Mul = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"MulEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_MulEq = c2AddMethodInfo(methiNew);
    }

    // The Negate method
    {
        TMEngMethodInfo methiNew
        (
            L"Negate"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Negate = c2AddMethodInfo(methiNew);
    }

    // The plus equal method
    {
        TMEngMethodInfo methiNew
        (
            L"PlusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_PlusEq = c2AddMethodInfo(methiNew);
    }

    // Add the subtract method
    {
        TMEngMethodInfo methiNew
        (
            L"Sub"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_Sub = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that takes an initial value
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.Int4"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        methiNew.c2AddInParm(L"InitVal", tCIDMacroEng::EIntrinsics::Int4);
        m_c2MethId_ValCtor = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngInt4Info::pmecvMakeStorage(const   TString&                strName
                                ,       TCIDMacroEngine&
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngInt4Val(strName, eConst);
}


// ---------------------------------------------------------------------------
//  TMEngInt4Info: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngInt4Info::bInvokeMethod(       TCIDMacroEngine&    meOwner
                            , const TMEngMethodInfo&    methiTarget
                            ,       TMEngClassVal&      mecvInstance)
{
    TMEngInt4Val& mecvActual = static_cast<TMEngInt4Val&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_AbsValue)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);

        tCIDLib::TInt4 i4Val = mecvActual.i4Value();
        if (i4Val < 0)
            i4Val *= -1;
        mecvRet.c4Value(tCIDLib::TCard4(i4Val));
    }
     else if (c2MethId == m_c2MethId_Add)
    {
        TMEngInt4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1);
        mecvRet.i4Value(mecvActual.i4Value() + meOwner.i4StackValAt(c4FirstInd));
    }
     else if (methiTarget.c2Id() == m_c2MethId_DefCtor)
    {
        mecvActual.i4Value(0);
    }
     else if (c2MethId == m_c2MethId_Dec)
    {
        const tCIDLib::TInt4 i4Cur = mecvActual.i4Value();
        TMEngInt4Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1);
        mecvRet.i4Value(i4Cur);
        if (i4Cur > kCIDLib::i4MinInt)
            mecvActual.i4Value(i4Cur - 1);
    }
     else if (c2MethId == m_c2MethId_Div)
    {
        TMEngInt4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1);
        mecvRet.i4Value(mecvActual.i4Value() / meOwner.i4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_DivEq)
    {
        mecvActual.i4Value(mecvActual.i4Value() / meOwner.i4StackValAt(c4FirstInd));
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner) << mecvActual.i4Value();
    }
     else if (c2MethId == m_c2MethId_GetNegated)
    {
        TMEngInt4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1);
        mecvRet.i4Value(mecvActual.i4Value() * -1);
    }
     else if ((c2MethId == m_c2MethId_Equal)
          ||  (c2MethId == m_c2MethId_GtThan)
          ||  (c2MethId == m_c2MethId_GtThanEq)
          ||  (c2MethId == m_c2MethId_LsThan)
          ||  (c2MethId == m_c2MethId_LsThanEq))
    {
        //
        //  These set the boolean return value, based on our relation
        //  to a passed card4 value.
        //
        TMEngBooleanVal& mecvRet
                = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        const tCIDLib::TInt4 i4Parm = meOwner.i4StackValAt(c4FirstInd);

        if (c2MethId == m_c2MethId_GtThan)
            mecvRet.bValue(mecvActual.i4Value() > i4Parm);
        else if (c2MethId == m_c2MethId_GtThanEq)
            mecvRet.bValue(mecvActual.i4Value() >= i4Parm);
        else if (c2MethId == m_c2MethId_LsThan)
            mecvRet.bValue(mecvActual.i4Value() < i4Parm);
        else if (c2MethId == m_c2MethId_LsThanEq)
            mecvRet.bValue(mecvActual.i4Value() <= i4Parm);
        else
            mecvRet.bValue(mecvActual.i4Value() == i4Parm);
    }
     else if (c2MethId == m_c2MethId_Inc)
    {
        const tCIDLib::TInt4 i4Cur = mecvActual.i4Value();
        TMEngInt4Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1);
        mecvRet.i4Value(i4Cur);
        if (i4Cur < kCIDLib::i4MaxInt)
            mecvActual.i4Value(i4Cur + 1);
    }
     else if (c2MethId == m_c2MethId_MaxVal)
    {
        const tCIDLib::TInt4 i4Comp = meOwner.i4StackValAt(c4FirstInd);
        TMEngInt4Val& mecvRet = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1);
        if (i4Comp > mecvActual.i4Value())
            mecvRet.i4Value(i4Comp);
        else
            mecvRet.i4Value(mecvActual.i4Value());
    }
     else if (c2MethId == m_c2MethId_MinVal)
    {
        const tCIDLib::TInt4 i4Comp = meOwner.i4StackValAt(c4FirstInd);
        TMEngInt4Val& mecvRet = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1);
        if (i4Comp < mecvActual.i4Value())
            mecvRet.i4Value(i4Comp);
        else
            mecvRet.i4Value(mecvActual.i4Value());
    }
     else if (c2MethId == m_c2MethId_MinusEq)
    {
        const tCIDLib::TInt4 i4Sub = meOwner.i4StackValAt(c4FirstInd);
        mecvActual.i4Value(mecvActual.i4Value() - i4Sub);
    }
     else if (c2MethId == m_c2MethId_ModDiv)
    {
        TMEngInt4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1);
        mecvRet.i4Value(mecvActual.i4Value() % meOwner.i4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ModDivEq)
    {
        mecvActual.i4Value(mecvActual.i4Value() % meOwner.i4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Mul)
    {
        TMEngInt4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1);
        mecvRet.i4Value(mecvActual.i4Value() * meOwner.i4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_MulEq)
    {
        mecvActual.i4Value(mecvActual.i4Value() * meOwner.i4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Negate)
    {
        mecvActual.i4Value(mecvActual.i4Value() * -1);
    }
     else if (c2MethId == m_c2MethId_PlusEq)
    {
        const tCIDLib::TInt4 i4Add = meOwner.i4StackValAt(c4FirstInd);
        mecvActual.i4Value(mecvActual.i4Value() + i4Add);
    }
     else if (c2MethId == m_c2MethId_Sub)
    {
        TMEngInt4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1);
        mecvRet.i4Value(mecvActual.i4Value() - meOwner.i4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        mecvActual.i4Value(meOwner.i4StackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


