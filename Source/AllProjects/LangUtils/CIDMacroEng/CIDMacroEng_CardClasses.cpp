//
// FILE NAME: CIDMacroEng_CardTypes.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2003
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
//  This file implements a set of derivatives of the type info and type
//  value classes, which implement the fundamental intrinsic types.
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
RTTIDecls(TMEngCard1Val,TMEngClassVal)
RTTIDecls(TMEngCard1Info,TMEngClassInfo)
RTTIDecls(TMEngCard2Val,TMEngClassVal)
RTTIDecls(TMEngCard2Info,TMEngClassInfo)
RTTIDecls(TMEngCard4Val,TMEngClassVal)
RTTIDecls(TMEngCard4Info,TMEngClassInfo)
RTTIDecls(TMEngCard8Val,TMEngClassVal)
RTTIDecls(TMEngCard8Info,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_CardClasses
{
    // -----------------------------------------------------------------------
    //  The names for the types that we support here. Each derivative has to
    //  be able to return strings that contain its name and full name.
    // -----------------------------------------------------------------------
    const TString   strCard1(L"Card1");
    const TString   strCard2(L"Card2");
    const TString   strCard4(L"Card4");
    const TString   strCard8(L"Card8");

    const TString   strCard1ClassPath(L"MEng.Card1");
    const TString   strCard2ClassPath(L"MEng.Card2");
    const TString   strCard4ClassPath(L"MEng.Card4");
    const TString   strCard8ClassPath(L"MEng.Card8");
}


// ---------------------------------------------------------------------------
//  CLASS: TMEngCard1Val
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCard1Val: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngCard1Val::TMEngCard1Val(const  TString&                strName
                            , const tCIDMacroEng::EConstTypes  eConst
                            , const tCIDLib::TCard1         c1Init) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card1), eConst)
    , m_c1Value(c1Init)
{
}

TMEngCard1Val::~TMEngCard1Val()
{
}


// ---------------------------------------------------------------------------
//  TMEngCard1Val: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCard1Val::bDbgFormat(          TTextOutStream&     strmTarget
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices   eRadix
                            , const TCIDMacroEngine&    ) const
{
    if (eRadix == tCIDLib::ERadices::Hex)
        strmTarget << L"0x";
    strmTarget << TCardinal(m_c1Value, eRadix);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMEngCard1Val::bParseFromText(  const   TString&        strValue
                                , const TMEngClassInfo&)
{
    tCIDLib::TBoolean bRes;
    tCIDLib::TCard4 c4Val = TRawStr::c4AsBinary(strValue.pszBuffer(), bRes);
    if (bRes)
        bRes = (c4Val <= kCIDLib::c1MaxCard);
    if (bRes)
        m_c1Value = tCIDLib::TCard1(c4Val);
    return bRes;
}


tCIDLib::TVoid
TMEngCard1Val::CopyFrom(const   TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    m_c1Value = static_cast<const TMEngCard1Val&>(mecvToCopy).m_c1Value;
}



// ---------------------------------------------------------------------------
//  TMEngCard1Val: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard1 TMEngCard1Val::c1Value() const
{
    return m_c1Value;
}

tCIDLib::TCard1 TMEngCard1Val::c1Value(const tCIDLib::TCard1 c1ToSet)
{
    m_c1Value = c1ToSet;
    return m_c1Value;
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngCard1Info
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCard1Info: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngCard1Info::strPath()
{
    return CIDMacroEng_CardClasses::strCard1ClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngCard1Info: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCard1Info::TMEngCard1Info(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_CardClasses::strCard1
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2MethId_Add(kMacroEng::c2BadId)
    , m_c2MethId_And(kMacroEng::c2BadId)
    , m_c2MethId_AndEq(kMacroEng::c2BadId)
    , m_c2MethId_Dec(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Div(kMacroEng::c2BadId)
    , m_c2MethId_DivEq(kMacroEng::c2BadId)
    , m_c2MethId_Equal(kMacroEng::c2BadId)
    , m_c2MethId_GetBit(kMacroEng::c2BadId)
    , m_c2MethId_GetHighNibble(kMacroEng::c2BadId)
    , m_c2MethId_GetLowNibble(kMacroEng::c2BadId)
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
    , m_c2MethId_Or(kMacroEng::c2BadId)
    , m_c2MethId_OrEq(kMacroEng::c2BadId)
    , m_c2MethId_PlusEq(kMacroEng::c2BadId)
    , m_c2MethId_SetBit(kMacroEng::c2BadId)
    , m_c2MethId_ShiftL(kMacroEng::c2BadId)
    , m_c2MethId_ShiftR(kMacroEng::c2BadId)
    , m_c2MethId_Sub(kMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kMacroEng::c2BadId)
    , m_c2MethId_Xor(kMacroEng::c2BadId)
    , m_c2MethId_XorEq(kMacroEng::c2BadId)
{
}

TMEngCard1Info::~TMEngCard1Info()
{
}


// ---------------------------------------------------------------------------
//  TMEngCard1Info: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngCard1Info::bCanCastFrom(const tCIDLib::TCard2 c2SrcId)
{
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
        case tCIDMacroEng::EIntrinsics::Card1 :
        case tCIDMacroEng::EIntrinsics::Card2 :
        case tCIDMacroEng::EIntrinsics::Card4 :
        case tCIDMacroEng::EIntrinsics::Card8 :
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
TMEngCard1Info::eCastFrom(          TCIDMacroEngine&
                            , const TMEngClassVal&      mecvSrc
                            ,       TMEngClassVal&      mecvTarget)
{
    // Get the source value
    const tCIDLib::TCard2 c2SrcId = mecvSrc.c2ClassId();

    tCIDLib::TCard1 c1New;
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
            if (static_cast<const TMEngBooleanVal&>(mecvSrc).bValue())
                c1New = 1;
            else
                c1New = 0;
            break;

        case tCIDMacroEng::EIntrinsics::Card1 :
            c1New = static_cast<const TMEngCard1Val&>(mecvSrc).c1Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card2 :
            c1New = tCIDLib::TCard1(static_cast<const TMEngCard2Val&>(mecvSrc).c2Value());
            break;

        case tCIDMacroEng::EIntrinsics::Card4 :
            c1New = tCIDLib::TCard1(static_cast<const TMEngCard4Val&>(mecvSrc).c4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Card8 :
            c1New = tCIDLib::TCard1(static_cast<const TMEngCard8Val&>(mecvSrc).c8Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int1 :
            c1New = tCIDLib::TCard1(static_cast<const TMEngInt1Val&>(mecvSrc).i1Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int2 :
            c1New = tCIDLib::TCard1(static_cast<const TMEngInt2Val&>(mecvSrc).i2Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int4 :
            c1New = tCIDLib::TCard1(static_cast<const TMEngInt4Val&>(mecvSrc).i4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Float4 :
            c1New = tCIDLib::TCard1(static_cast<const TMEngFloat4Val&>(mecvSrc).f4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Float8 :
            c1New = tCIDLib::TCard1(static_cast<const TMEngFloat8Val&>(mecvSrc).f8Value());
            break;

        default :
            return tCIDMacroEng::ECastRes::Incompatible;
    };

    // Cast the target to our type, which it must be
    TMEngCard1Val& mecvActual = static_cast<TMEngCard1Val&>(mecvTarget);
    mecvActual.c1Value(c1New);
    return tCIDMacroEng::ECastRes::Ok;
}


tCIDLib::TVoid TMEngCard1Info::Init(TCIDMacroEngine&)
{
    // Add a literal for our max value
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kMaxValue"
            , tCIDMacroEng::EIntrinsics::Card1
            , new TMEngCard1Val(L"kMaxValue", tCIDMacroEng::EConstTypes::Const, kCIDLib::c1MaxCard)
        )
    );

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.Card1"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that takes an initial value
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.Card1"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"InitVal", tCIDMacroEng::EIntrinsics::Card1);
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_ValCtor = c2AddMethodInfo(methiNew);
    }

    // Binary addition
    {
        TMEngMethodInfo methiNew
        (
            L"Add"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_Add = c2AddMethodInfo(methiNew);
    }

    // Unary and binary bitwise AND methods
    {
        TMEngMethodInfo methiNew
        (
            L"And"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToAnd", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_And = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"AndEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAnd", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_AndEq = c2AddMethodInfo(methiNew);
    }

    // Add the decrement method
    {
        TMEngMethodInfo methiNew
        (
            L"Dec"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Dec = c2AddMethodInfo(methiNew);
    }

    // Add the div and diveq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Div"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Card1);
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
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Card1);
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
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_Equal = c2AddMethodInfo(methiNew);
    }

    // Get a bit state
    {
        TMEngMethodInfo methiNew
        (
            L"GetBit"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"BitNum", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetBit = c2AddMethodInfo(methiNew);
    }

    // Get the low or high nibble
    {
        TMEngMethodInfo methiNew
        (
            L"GetHighNibble"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetHighNibble = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetLowNibble"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetLowNibble = c2AddMethodInfo(methiNew);
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
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Card1);
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
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_GtThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the increment method
    {
        TMEngMethodInfo methiNew
        (
            L"Inc"
            , tCIDMacroEng::EIntrinsics::Card1
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
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Card1);
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
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_LsThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the max/min value methods
    {
        TMEngMethodInfo methiNew
        (
            L"MaxVal"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_MaxVal = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"MinVal"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_MinVal = c2AddMethodInfo(methiNew);
    }

    // Unary subtraction
    {
        TMEngMethodInfo methiNew
        (
            L"MinusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_MinusEq = c2AddMethodInfo(methiNew);
    }

    // Add the moddiv and moddiveq methods
    {
        TMEngMethodInfo methiNew
        (
            L"ModDiv"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Divisor", tCIDMacroEng::EIntrinsics::Card1);
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
        methiNew.c2AddInParm(L"Divisor", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_ModDivEq = c2AddMethodInfo(methiNew);
    }

    // Add the mul and muleq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Mul"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Card1);
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
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_MulEq = c2AddMethodInfo(methiNew);
    }

    // Unary and binary bitwise OR methods
    {
        TMEngMethodInfo methiNew
        (
            L"Or"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToOr", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_Or = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"OrEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToOr", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_OrEq = c2AddMethodInfo(methiNew);
    }

    // Unary addition
    {
        TMEngMethodInfo methiNew
        (
            L"PlusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_PlusEq = c2AddMethodInfo(methiNew);
    }

    // Set/clear a bit
    {
        TMEngMethodInfo methiNew
        (
            L"SetBit"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"BitNum", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"NewState", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_SetBit = c2AddMethodInfo(methiNew);
    }

    // Shift left and right
    {
        TMEngMethodInfo methiNew
        (
            L"ShiftLeft"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ShiftCount", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ShiftL = c2AddMethodInfo(methiNew);
    }
    {
        TMEngMethodInfo methiNew
        (
            L"ShiftRight"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ShiftCount", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ShiftR = c2AddMethodInfo(methiNew);
    }

    // Binary addition
    {
        TMEngMethodInfo methiNew
        (
            L"Sub"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_Sub = c2AddMethodInfo(methiNew);
    }

    // Unary and binary bitwise AND methods
    {
        TMEngMethodInfo methiNew
        (
            L"Xor"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToXor", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_Xor = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"XorEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToXor", tCIDMacroEng::EIntrinsics::Card1);
        m_c2MethId_XorEq = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngCard1Info::pmecvMakeStorage(const  TString&                strName
                                ,       TCIDMacroEngine&
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngCard1Val(strName, eConst);
}



// ---------------------------------------------------------------------------
//  TMEngCard1Info: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCard1Info::bInvokeMethod(          TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngCard1Val& mecvActual = static_cast<TMEngCard1Val&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_Add)
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        mecvRet.c1Value(mecvActual.c1Value() + meOwner.c1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_And)
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        mecvRet.c1Value(mecvActual.c1Value() & meOwner.c1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_AndEq)
    {
        mecvActual.c1Value(mecvActual.c1Value() & meOwner.c1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Dec)
    {
        const tCIDLib::TCard1 c1Cur = mecvActual.c1Value();
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        mecvRet.c1Value(c1Cur);
        if (c1Cur)
            mecvActual.c1Value(c1Cur - 1);
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Default ctor just zeros the value
        mecvActual.c1Value(0);
    }
     else if (c2MethId == m_c2MethId_Div)
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        mecvRet.c1Value(mecvActual.c1Value() / meOwner.c1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_DivEq)
    {
        mecvActual.c1Value(mecvActual.c1Value() / meOwner.c1StackValAt(c4FirstInd));
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner) << mecvActual.c1Value();
    }
     else if (c2MethId == m_c2MethId_GetBit)
    {
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        // Create the mask for the bit they wanted
        tCIDLib::TCard1 c1Mask = 0x1;
        c1Mask <<= meOwner.c4StackValAt(c4FirstInd);
        mecvRet.bValue((mecvActual.c1Value() & c1Mask) != 0);
    }
     else if (c2MethId == m_c2MethId_GetLowNibble)
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        mecvRet.c1Value(mecvActual.c1Value() & 0xF);
    }
     else if (c2MethId == m_c2MethId_GetHighNibble)
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        mecvRet.c1Value(mecvActual.c1Value() >> 4);
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
        const tCIDLib::TCard1 c1Parm = meOwner.c1StackValAt(c4FirstInd);

        if (c2MethId == m_c2MethId_GtThan)
            mecvRet.bValue(mecvActual.c1Value() > c1Parm);
        else if (c2MethId == m_c2MethId_GtThanEq)
            mecvRet.bValue(mecvActual.c1Value() >= c1Parm);
        else if (c2MethId == m_c2MethId_LsThan)
            mecvRet.bValue(mecvActual.c1Value() < c1Parm);
        else if (c2MethId == m_c2MethId_LsThanEq)
            mecvRet.bValue(mecvActual.c1Value() <= c1Parm);
        else
            mecvRet.bValue(mecvActual.c1Value() == c1Parm);
    }
     else if (c2MethId == m_c2MethId_Inc)
    {
        const tCIDLib::TCard1 c1Cur = mecvActual.c1Value();
        TMEngCard1Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        mecvRet.c1Value(c1Cur);
        if (c1Cur < kCIDLib::c1MaxCard)
            mecvActual.c1Value(c1Cur + 1);
    }
     else if (c2MethId == m_c2MethId_MaxVal)
    {
        const tCIDLib::TCard1 c1Comp = meOwner.c1StackValAt(c4FirstInd);
        TMEngCard1Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        if (c1Comp > mecvActual.c1Value())
            mecvRet.c1Value(c1Comp);
        else
            mecvRet.c1Value(mecvActual.c1Value());
    }
     else if (c2MethId == m_c2MethId_MinVal)
    {
        const tCIDLib::TCard1 c1Comp = meOwner.c1StackValAt(c4FirstInd);
        TMEngCard1Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        if (c1Comp < mecvActual.c1Value())
            mecvRet.c1Value(c1Comp);
        else
            mecvRet.c1Value(mecvActual.c1Value());
    }
     else if (c2MethId == m_c2MethId_MinusEq)
    {
        const tCIDLib::TCard1 c1Sub = meOwner.c1StackValAt(c4FirstInd);
        mecvActual.c1Value(mecvActual.c1Value() - c1Sub);
    }
     else if (c2MethId == m_c2MethId_ModDiv)
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        mecvRet.c1Value(mecvActual.c1Value() % meOwner.c1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ModDivEq)
    {
        mecvActual.c1Value(mecvActual.c1Value() % meOwner.c1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Mul)
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        mecvRet.c1Value(mecvActual.c1Value() * meOwner.c1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_MulEq)
    {
        mecvActual.c1Value(mecvActual.c1Value() * meOwner.c1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Or)
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        mecvRet.c1Value(mecvActual.c1Value() | meOwner.c1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_OrEq)
    {
        mecvActual.c1Value(mecvActual.c1Value() | meOwner.c1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_PlusEq)
    {
        const tCIDLib::TCard1 c1Add = meOwner.c1StackValAt(c4FirstInd);
        mecvActual.c1Value(mecvActual.c1Value() + c1Add);
    }
     else if (c2MethId == m_c2MethId_SetBit)
    {
        const tCIDLib::TCard4 c4Bit = meOwner.c4StackValAt(c4FirstInd);
        tCIDLib::TCard4 c4Val = mecvActual.c1Value();
        if (meOwner.bStackValAt(c4FirstInd + 1))
            c4Val |= (0x1UL << c4Bit);
        else
            c4Val &= ~(0x1UL << c4Bit);
        mecvActual.c1Value(tCIDLib::TCard1(c4Val));
    }
     else if ((c2MethId == m_c2MethId_ShiftL)
          ||  (c2MethId == m_c2MethId_ShiftR))
    {
        tCIDLib::TCard1 c1Tmp = mecvActual.c1Value();
        tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd);

        if (c2MethId == m_c2MethId_ShiftR)
            mecvActual.c1Value(tCIDLib::TCard1(c1Tmp >> c4Count));
        else
            mecvActual.c1Value(tCIDLib::TCard1(c1Tmp << c4Count));
    }
     else if (c2MethId == m_c2MethId_Sub)
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        mecvRet.c1Value(mecvActual.c1Value() - meOwner.c1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        mecvActual.c1Value(meOwner.c1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Xor)
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        mecvRet.c1Value(mecvActual.c1Value() ^ meOwner.c1StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_XorEq)
    {
        mecvActual.c1Value(mecvActual.c1Value() ^ meOwner.c1StackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngCard2Val
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCard2Val: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCard2Val::TMEngCard2Val(const  TString&                strName
                            , const tCIDMacroEng::EConstTypes  eConst
                            , const tCIDLib::TCard2         c2Init) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card2), eConst)
    , m_c2Value(c2Init)
{
}

TMEngCard2Val::~TMEngCard2Val()
{
}


// ---------------------------------------------------------------------------
//  TMEngCard2Val: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCard2Val::bDbgFormat(          TTextOutStream&     strmTarget
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices   eRadix
                            , const TCIDMacroEngine&    ) const
{
    if (eRadix == tCIDLib::ERadices::Hex)
        strmTarget << L"0x";
    strmTarget << TCardinal(m_c2Value, eRadix);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMEngCard2Val::bParseFromText(  const   TString&        strValue
                                , const TMEngClassInfo& )
{
    tCIDLib::TBoolean bRes;
    tCIDLib::TCard4 c4Val = TRawStr::c4AsBinary(strValue.pszBuffer(), bRes);
    if (bRes)
        bRes = (c4Val <= kCIDLib::c2MaxCard);
    if (bRes)
        m_c2Value = tCIDLib::TCard2(c4Val);
    return bRes;
}


tCIDLib::TVoid
TMEngCard2Val::CopyFrom(const   TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    m_c2Value = static_cast<const TMEngCard2Val&>(mecvToCopy).m_c2Value;
}



// ---------------------------------------------------------------------------
//  TMEngCard2Val: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard2 TMEngCard2Val::c2Value() const
{
    return m_c2Value;
}

tCIDLib::TCard2 TMEngCard2Val::c2Value(const tCIDLib::TCard2 c2ToSet)
{
    m_c2Value = c2ToSet;
    return m_c2Value;
}


tCIDLib::TVoid TMEngCard2Val::SwapBytes()
{
    m_c2Value = TRawBits::c2SwapBytes(m_c2Value);
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngCard2Info
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCard2Info: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngCard2Info::strPath()
{
    return CIDMacroEng_CardClasses::strCard2ClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngCard2Info: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCard2Info::TMEngCard2Info(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_CardClasses::strCard2
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2MethId_Add(kMacroEng::c2BadId)
    , m_c2MethId_And(kMacroEng::c2BadId)
    , m_c2MethId_AndEq(kMacroEng::c2BadId)
    , m_c2MethId_Dec(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Div(kMacroEng::c2BadId)
    , m_c2MethId_DivEq(kMacroEng::c2BadId)
    , m_c2MethId_Equal(kMacroEng::c2BadId)
    , m_c2MethId_GetBit(kMacroEng::c2BadId)
    , m_c2MethId_GetHighByte(kMacroEng::c2BadId)
    , m_c2MethId_GetLowByte(kMacroEng::c2BadId)
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
    , m_c2MethId_Or(kMacroEng::c2BadId)
    , m_c2MethId_OrEq(kMacroEng::c2BadId)
    , m_c2MethId_PlusEq(kMacroEng::c2BadId)
    , m_c2MethId_SetBit(kMacroEng::c2BadId)
    , m_c2MethId_ShiftL(kMacroEng::c2BadId)
    , m_c2MethId_ShiftR(kMacroEng::c2BadId)
    , m_c2MethId_Sub(kMacroEng::c2BadId)
    , m_c2MethId_SwapBytes(kMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kMacroEng::c2BadId)
    , m_c2MethId_Xor(kMacroEng::c2BadId)
    , m_c2MethId_XorEq(kMacroEng::c2BadId)
{
}

TMEngCard2Info::~TMEngCard2Info()
{
}


// ---------------------------------------------------------------------------
//  TMEngCard2Info: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngCard2Info::bCanCastFrom(const tCIDLib::TCard2 c2SrcId)
{
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
        case tCIDMacroEng::EIntrinsics::Card1 :
        case tCIDMacroEng::EIntrinsics::Card2 :
        case tCIDMacroEng::EIntrinsics::Card4 :
        case tCIDMacroEng::EIntrinsics::Card8 :
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
TMEngCard2Info::eCastFrom(          TCIDMacroEngine&
                            , const TMEngClassVal&      mecvSrc
                            ,       TMEngClassVal&      mecvTarget)
{
    // Get the source value
    const tCIDLib::TCard2 c2SrcId = mecvSrc.c2ClassId();

    tCIDLib::TCard2 c2New;
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
            if (static_cast<const TMEngBooleanVal&>(mecvSrc).bValue())
                c2New = 1;
            else
                c2New = 0;
            break;

        case tCIDMacroEng::EIntrinsics::Card1 :
            c2New = static_cast<const TMEngCard1Val&>(mecvSrc).c1Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card2 :
            c2New = static_cast<const TMEngCard2Val&>(mecvSrc).c2Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card4 :
            c2New = tCIDLib::TCard2(static_cast<const TMEngCard4Val&>(mecvSrc).c4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Card8 :
            c2New = tCIDLib::TCard2(static_cast<const TMEngCard8Val&>(mecvSrc).c8Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int1 :
            c2New = tCIDLib::TCard2(static_cast<const TMEngInt1Val&>(mecvSrc).i1Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int2 :
            c2New = tCIDLib::TCard2(static_cast<const TMEngInt2Val&>(mecvSrc).i2Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int4 :
            c2New = tCIDLib::TCard2(static_cast<const TMEngInt4Val&>(mecvSrc).i4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Float4 :
            c2New = tCIDLib::TCard2(static_cast<const TMEngFloat4Val&>(mecvSrc).f4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Float8 :
            c2New = tCIDLib::TCard2(static_cast<const TMEngFloat8Val&>(mecvSrc).f8Value());
            break;

        default:
            return tCIDMacroEng::ECastRes::Incompatible;
    };

    // Cast the target to our type, which it must be
    TMEngCard2Val& mecvActual = static_cast<TMEngCard2Val&>(mecvTarget);
    mecvActual.c2Value(c2New);
    return tCIDMacroEng::ECastRes::Ok;
}


tCIDLib::TVoid TMEngCard2Info::Init(TCIDMacroEngine&)
{
    // Add a literal for our max value
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kMaxValue"
            , tCIDMacroEng::EIntrinsics::Card2
            , new TMEngCard2Val(L"kMaxValue", tCIDMacroEng::EConstTypes::Const, kCIDLib::c2MaxCard)
        )
    );

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.Card2"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that takes an initial value
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.Card2"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"InitVal", tCIDMacroEng::EIntrinsics::Card2);
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_ValCtor = c2AddMethodInfo(methiNew);
    }

    // Binary addition
    {
        TMEngMethodInfo methiNew
        (
            L"Add"
            , tCIDMacroEng::EIntrinsics::Card2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_Add = c2AddMethodInfo(methiNew);
    }

    // Unary and binary bitwise AND methods
    {
        TMEngMethodInfo methiNew
        (
            L"And"
            , tCIDMacroEng::EIntrinsics::Card2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToAnd", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_And = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"AndEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAnd", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_AndEq = c2AddMethodInfo(methiNew);
    }

    // Add the decrement method
    {
        TMEngMethodInfo methiNew
        (
            L"Dec"
            , tCIDMacroEng::EIntrinsics::Card2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Dec = c2AddMethodInfo(methiNew);
    }

    // Add the div and diveq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Div"
            , tCIDMacroEng::EIntrinsics::Card2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Card2);
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
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Card2);
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
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_Equal = c2AddMethodInfo(methiNew);
    }

    // Get a bit state
    {
        TMEngMethodInfo methiNew
        (
            L"GetBit"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"BitNum", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetBit = c2AddMethodInfo(methiNew);
    }

    // Get the low and high byte
    {
        TMEngMethodInfo methiNew
        (
            L"GetHighByte"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetHighByte = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetLowByte"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetLowByte = c2AddMethodInfo(methiNew);
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
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Card2);
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
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_GtThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the increment method
    {
        TMEngMethodInfo methiNew
        (
            L"Inc"
            , tCIDMacroEng::EIntrinsics::Card2
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
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Card2);
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
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_LsThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the max/min value methods
    {
        TMEngMethodInfo methiNew
        (
            L"MaxVal"
            , tCIDMacroEng::EIntrinsics::Card2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_MaxVal = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"MinVal"
            , tCIDMacroEng::EIntrinsics::Card2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_MinVal = c2AddMethodInfo(methiNew);
    }

    // Unary subtraction
    {
        TMEngMethodInfo methiNew
        (
            L"MinusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_MinusEq = c2AddMethodInfo(methiNew);
    }

    // Add the moddiv and moddiveq methods
    {
        TMEngMethodInfo methiNew
        (
            L"ModDiv"
            , tCIDMacroEng::EIntrinsics::Card2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Divisor", tCIDMacroEng::EIntrinsics::Card2);
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
        methiNew.c2AddInParm(L"Divisor", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_ModDivEq = c2AddMethodInfo(methiNew);
    }

    // Add the mul and muleq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Mul"
            , tCIDMacroEng::EIntrinsics::Card2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Card2);
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
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_MulEq = c2AddMethodInfo(methiNew);
    }

    // Unary and binary bitwise OR methods
    {
        TMEngMethodInfo methiNew
        (
            L"Or"
            , tCIDMacroEng::EIntrinsics::Card2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToOr", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_Or = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"OrEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToOr", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_OrEq = c2AddMethodInfo(methiNew);
    }

    // Unary addition
    {
        TMEngMethodInfo methiNew
        (
            L"PlusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_PlusEq = c2AddMethodInfo(methiNew);
    }

    // Set/clear a bit
    {
        TMEngMethodInfo methiNew
        (
            L"SetBit"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"BitNum", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"NewState", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_SetBit = c2AddMethodInfo(methiNew);
    }

    // Shift left and right
    {
        TMEngMethodInfo methiNew
        (
            L"ShiftLeft"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ShiftCount", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ShiftL = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ShiftRight"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ShiftCount", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ShiftR = c2AddMethodInfo(methiNew);
    }

    // Binary addition
    {
        TMEngMethodInfo methiNew
        (
            L"Sub"
            , tCIDMacroEng::EIntrinsics::Card2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_Sub = c2AddMethodInfo(methiNew);
    }

    // Swap the byte order
    {
        TMEngMethodInfo methiNew
        (
            L"SwapBytes"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_SwapBytes = c2AddMethodInfo(methiNew);
    }

    // Unary and binary bitwise AND methods
    {
        TMEngMethodInfo methiNew
        (
            L"Xor"
            , tCIDMacroEng::EIntrinsics::Card2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToXor", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_Xor = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"XorEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToXor", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_XorEq = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngCard2Info::pmecvMakeStorage(const  TString&                strName
                                ,       TCIDMacroEngine&
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngCard2Val(strName, eConst);
}


// ---------------------------------------------------------------------------
//  TMEngCard2Info: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCard2Info::bInvokeMethod(          TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngCard2Val& mecvActual = static_cast<TMEngCard2Val&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_Add)
    {
        TMEngCard2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);
        mecvRet.c2Value(mecvActual.c2Value() + meOwner.c2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_And)
    {
        TMEngCard2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);
        mecvRet.c2Value(mecvActual.c2Value() & meOwner.c2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_AndEq)
    {
        mecvActual.c2Value(mecvActual.c2Value() & meOwner.c2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Dec)
    {
        const tCIDLib::TCard2 c2Cur = mecvActual.c2Value();
        TMEngCard2Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);
        mecvRet.c2Value(c2Cur);
        if (c2Cur)
            mecvActual.c2Value(c2Cur - 1);
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Default ctor just zeros the value
        mecvActual.c2Value(0);
    }
     else if (c2MethId == m_c2MethId_Div)
    {
        TMEngCard2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);
        mecvRet.c2Value(mecvActual.c2Value() / meOwner.c2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_DivEq)
    {
        mecvActual.c2Value(mecvActual.c2Value() / meOwner.c2StackValAt(c4FirstInd));
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner) << mecvActual.c2Value();
    }
     else if (c2MethId == m_c2MethId_GetBit)
    {
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);

        // Create the mask for the bit they wanted
        tCIDLib::TCard2 c2Mask = 0x1;
        c2Mask <<= meOwner.c4StackValAt(c4FirstInd);
        mecvRet.bValue((mecvActual.c2Value() & c2Mask) != 0);
    }
     else if (c2MethId == m_c2MethId_GetHighByte)
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        mecvRet.c1Value(tCIDLib::TCard1(mecvActual.c2Value() >> 8));
    }
     else if (c2MethId == m_c2MethId_GetLowByte)
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
        mecvRet.c1Value(tCIDLib::TCard1(mecvActual.c2Value() & 0xFF));
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
        const tCIDLib::TCard2 c2Parm = meOwner.c2StackValAt(c4FirstInd);

        if (c2MethId == m_c2MethId_GtThan)
            mecvRet.bValue(mecvActual.c2Value() > c2Parm);
        else if (c2MethId == m_c2MethId_GtThanEq)
            mecvRet.bValue(mecvActual.c2Value() >= c2Parm);
        else if (c2MethId == m_c2MethId_LsThan)
            mecvRet.bValue(mecvActual.c2Value() < c2Parm);
        else if (c2MethId == m_c2MethId_LsThanEq)
            mecvRet.bValue(mecvActual.c2Value() <= c2Parm);
        else
            mecvRet.bValue(mecvActual.c2Value() == c2Parm);
    }
     else if (c2MethId == m_c2MethId_Inc)
    {
        const tCIDLib::TCard2 c2Cur = mecvActual.c2Value();
        TMEngCard2Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);
        mecvRet.c2Value(c2Cur);
        if (c2Cur < kCIDLib::c2MaxCard)
            mecvActual.c2Value(c2Cur + 1);
    }
     else if (c2MethId == m_c2MethId_MaxVal)
    {
        const tCIDLib::TCard2 c2Comp = meOwner.c2StackValAt(c4FirstInd);
        TMEngCard2Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);
        if (c2Comp > mecvActual.c2Value())
            mecvRet.c2Value(c2Comp);
        else
            mecvRet.c2Value(mecvActual.c2Value());
    }
     else if (c2MethId == m_c2MethId_MinVal)
    {
        const tCIDLib::TCard2 c2Comp = meOwner.c2StackValAt(c4FirstInd);
        TMEngCard2Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);
        if (c2Comp < mecvActual.c2Value())
            mecvRet.c2Value(c2Comp);
        else
            mecvRet.c2Value(mecvActual.c2Value());
    }
     else if (c2MethId == m_c2MethId_MinusEq)
    {
        mecvActual.c2Value(mecvActual.c2Value() - meOwner.c2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ModDiv)
    {
        TMEngCard2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);
        mecvRet.c2Value(mecvActual.c2Value() % meOwner.c2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ModDivEq)
    {
        mecvActual.c2Value(mecvActual.c2Value() % meOwner.c2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Mul)
    {
        TMEngCard2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);
        mecvRet.c2Value(mecvActual.c2Value() * meOwner.c2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_MulEq)
    {
        mecvActual.c2Value(mecvActual.c2Value() * meOwner.c2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Or)
    {
        TMEngCard2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);
        mecvRet.c2Value(mecvActual.c2Value() | meOwner.c2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_OrEq)
    {
        mecvActual.c2Value(mecvActual.c2Value() | meOwner.c2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_PlusEq)
    {
        mecvActual.c2Value(mecvActual.c2Value() + meOwner.c2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_SetBit)
    {
        const tCIDLib::TCard4 c4Bit = meOwner.c4StackValAt(c4FirstInd);
        tCIDLib::TCard4 c4Val = mecvActual.c2Value();
        if (meOwner.bStackValAt(c4FirstInd + 1))
            c4Val |= (0x1UL << c4Bit);
        else
            c4Val &= ~(0x1UL << c4Bit);
        mecvActual.c2Value(tCIDLib::TCard2(c4Val));
    }
     else if ((c2MethId == m_c2MethId_ShiftL)
          ||  (c2MethId == m_c2MethId_ShiftR))
    {
        tCIDLib::TCard2 c2Tmp = mecvActual.c2Value();
        tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd);

        if (c2MethId == m_c2MethId_ShiftR)
            mecvActual.c2Value(tCIDLib::TCard2(c2Tmp >> c4Count));
        else
            mecvActual.c2Value(tCIDLib::TCard2(c2Tmp << c4Count));
    }
     else if (c2MethId == m_c2MethId_Sub)
    {
        TMEngCard2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);
        mecvRet.c2Value(mecvActual.c2Value() - meOwner.c2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_SwapBytes)
    {
        mecvActual.SwapBytes();
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        mecvActual.c2Value(meOwner.c2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Xor)
    {
        TMEngCard2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);
        mecvRet.c2Value(mecvActual.c2Value() ^ meOwner.c2StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_XorEq)
    {
        mecvActual.c2Value(mecvActual.c2Value() ^ meOwner.c2StackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngCard4Val
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCard4Val: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCard4Val::TMEngCard4Val(const  TString&                strName
                            , const tCIDMacroEng::EConstTypes  eConst
                            , const tCIDLib::TCard4         c4Init) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card4), eConst)
    , m_c4Value(c4Init)
{
}

TMEngCard4Val::~TMEngCard4Val()
{
}


// ---------------------------------------------------------------------------
//  TMEngCard4Val: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCard4Val::bDbgFormat(          TTextOutStream&     strmTarget
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices   eRadix
                            , const TCIDMacroEngine&    ) const
{
    if (eRadix == tCIDLib::ERadices::Hex)
        strmTarget << L"0x";
    strmTarget << TCardinal(m_c4Value, eRadix);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMEngCard4Val::bParseFromText(  const   TString&        strValue
                                , const TMEngClassInfo& )
{
    tCIDLib::TBoolean bRes;
    tCIDLib::TCard4 c4Val = TRawStr::c4AsBinary(strValue.pszBuffer(), bRes);
    if (bRes)
        m_c4Value = c4Val;
    return bRes;
}


tCIDLib::TVoid
TMEngCard4Val::CopyFrom(const   TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    m_c4Value = static_cast<const TMEngCard4Val&>(mecvToCopy).m_c4Value;
}



// ---------------------------------------------------------------------------
//  TMEngCard4Val: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TMEngCard4Val::c4Value() const
{
    return m_c4Value;
}

tCIDLib::TCard4 TMEngCard4Val::c4Value(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Value = c4ToSet;
    return m_c4Value;
}


tCIDLib::TVoid TMEngCard4Val::SwapBytes()
{
    m_c4Value = TRawBits::c4SwapBytes(m_c4Value);
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngCard4Info
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCard4Info: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngCard4Info::strPath()
{
    return CIDMacroEng_CardClasses::strCard4ClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngCard4Info: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCard4Info::TMEngCard4Info(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_CardClasses::strCard4
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2MethId_Add(kMacroEng::c2BadId)
    , m_c2MethId_And(kMacroEng::c2BadId)
    , m_c2MethId_AndEq(kMacroEng::c2BadId)
    , m_c2MethId_Dec(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Div(kMacroEng::c2BadId)
    , m_c2MethId_DivEq(kMacroEng::c2BadId)
    , m_c2MethId_Equal(kMacroEng::c2BadId)
    , m_c2MethId_GetBit(kMacroEng::c2BadId)
    , m_c2MethId_GetByte1(kMacroEng::c2BadId)
    , m_c2MethId_GetByte2(kMacroEng::c2BadId)
    , m_c2MethId_GetByte3(kMacroEng::c2BadId)
    , m_c2MethId_GetByte4(kMacroEng::c2BadId)
    , m_c2MethId_GetHighWord(kMacroEng::c2BadId)
    , m_c2MethId_GetLowWord(kMacroEng::c2BadId)
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
    , m_c2MethId_Or(kMacroEng::c2BadId)
    , m_c2MethId_OrEq(kMacroEng::c2BadId)
    , m_c2MethId_PercentOf(kMacroEng::c2BadId)
    , m_c2MethId_PlusEq(kMacroEng::c2BadId)
    , m_c2MethId_SetBit(kMacroEng::c2BadId)
    , m_c2MethId_ShiftL(kMacroEng::c2BadId)
    , m_c2MethId_ShiftR(kMacroEng::c2BadId)
    , m_c2MethId_Sub(kMacroEng::c2BadId)
    , m_c2MethId_SwapBytes(kMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kMacroEng::c2BadId)
    , m_c2MethId_Xor(kMacroEng::c2BadId)
    , m_c2MethId_XorEq(kMacroEng::c2BadId)
{
}

TMEngCard4Info::~TMEngCard4Info()
{
}


// ---------------------------------------------------------------------------
//  TMEngCard4Info: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngCard4Info::bCanCastFrom(const tCIDLib::TCard2 c2SrcId)
{
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
        case tCIDMacroEng::EIntrinsics::Card1 :
        case tCIDMacroEng::EIntrinsics::Card2 :
        case tCIDMacroEng::EIntrinsics::Card4 :
        case tCIDMacroEng::EIntrinsics::Card8 :
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
TMEngCard4Info::eCastFrom(          TCIDMacroEngine&
                            , const TMEngClassVal&      mecvSrc
                            ,       TMEngClassVal&      mecvTarget)
{
    // Get the source value
    const tCIDLib::TCard2 c2SrcId = mecvSrc.c2ClassId();

    tCIDLib::TCard4 c4New;
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
            if (static_cast<const TMEngBooleanVal&>(mecvSrc).bValue())
                c4New = 1;
            else
                c4New = 0;
            break;

        case tCIDMacroEng::EIntrinsics::Card1 :
            c4New = static_cast<const TMEngCard1Val&>(mecvSrc).c1Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card2 :
            c4New = static_cast<const TMEngCard2Val&>(mecvSrc).c2Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card4 :
            c4New = static_cast<const TMEngCard4Val&>(mecvSrc).c4Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card8 :
            c4New = tCIDLib::TCard4(static_cast<const TMEngCard8Val&>(mecvSrc).c8Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int1 :
            c4New = tCIDLib::TCard4(static_cast<const TMEngInt1Val&>(mecvSrc).i1Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int2 :
            c4New = tCIDLib::TCard4(static_cast<const TMEngInt2Val&>(mecvSrc).i2Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int4 :
            c4New = tCIDLib::TCard4(static_cast<const TMEngInt4Val&>(mecvSrc).i4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Float4 :
            c4New = tCIDLib::TCard4(static_cast<const TMEngFloat4Val&>(mecvSrc).f4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Float8 :
            c4New = tCIDLib::TCard4(static_cast<const TMEngFloat8Val&>(mecvSrc).f8Value());
            break;

        default :
            return tCIDMacroEng::ECastRes::Incompatible;
    };

    // Cast the target to our type, which it must be
    TMEngCard4Val& mecvActual = static_cast<TMEngCard4Val&>(mecvTarget);
    mecvActual.c4Value(c4New);
    return tCIDMacroEng::ECastRes::Ok;
}


tCIDLib::TVoid TMEngCard4Info::Init(TCIDMacroEngine&)
{
    // Add a literal for our max value
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kMaxValue"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kMaxValue", tCIDMacroEng::EConstTypes::Const, kCIDLib::c4MaxCard)
        )
    );

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.Card4"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that takes an initial value
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.Card4"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"InitVal", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_ValCtor = c2AddMethodInfo(methiNew);
    }

    // Binary addition
    {
        TMEngMethodInfo methiNew
        (
            L"Add"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_Add = c2AddMethodInfo(methiNew);
    }

    // Unary and binary bitwise AND methods
    {
        TMEngMethodInfo methiNew
        (
            L"And"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToAnd", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_And = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"AndEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAnd", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_AndEq = c2AddMethodInfo(methiNew);
    }

    // Add the decrement method
    {
        TMEngMethodInfo methiNew
        (
            L"Dec"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Dec = c2AddMethodInfo(methiNew);
    }

    // Add the div and diveq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Div"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Card4);
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
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Card4);
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
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_Equal = c2AddMethodInfo(methiNew);
    }

    // Get a bit state
    {
        TMEngMethodInfo methiNew
        (
            L"GetBit"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"BitNum", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetBit = c2AddMethodInfo(methiNew);
    }

    // Get one of the bytes of the Card4
    {
        TMEngMethodInfo methiNew
        (
            L"GetByte1"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetByte1 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetByte2"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetByte2 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetByte3"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetByte3 = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetByte4"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetByte4 = c2AddMethodInfo(methiNew);
    }

    // Get the low and high byte
    {
        TMEngMethodInfo methiNew
        (
            L"GetHighWord"
            , tCIDMacroEng::EIntrinsics::Card2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetHighWord = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetLowWord"
            , tCIDMacroEng::EIntrinsics::Card2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetLowWord = c2AddMethodInfo(methiNew);
    }

    // Add the greater than and greater/equal method
    {
        TMEngMethodInfo methiNew
        (
            L"GtThan"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Card4);
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
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GtThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the increment method
    {
        TMEngMethodInfo methiNew
        (
            L"Inc"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Inc = c2AddMethodInfo(methiNew);
    }

    // Add the less than and less/equal method
    {
        TMEngMethodInfo methiNew
        (
            L"LsThan"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Card4);
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
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_LsThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the max/min value methods
    {
        TMEngMethodInfo methiNew
        (
            L"MaxVal"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_MaxVal = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"MinVal"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_MinVal = c2AddMethodInfo(methiNew);
    }

    // Unary subtraction
    {
        TMEngMethodInfo methiNew
        (
            L"MinusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_MinusEq = c2AddMethodInfo(methiNew);
    }

    // Add the moddiv and moddiveq methods
    {
        TMEngMethodInfo methiNew
        (
            L"ModDiv"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Divisor", tCIDMacroEng::EIntrinsics::Card4);
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
        methiNew.c2AddInParm(L"Divisor", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ModDivEq = c2AddMethodInfo(methiNew);
    }

    // Add the mul and muleq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Mul"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Card4);
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
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_MulEq = c2AddMethodInfo(methiNew);
    }

    // Unary and binary bitwise OR methods
    {
        TMEngMethodInfo methiNew
        (
            L"Or"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToOr", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_Or = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"OrEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToOr", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_OrEq = c2AddMethodInfo(methiNew);
    }

    // Calc the percent of us that the passed value represents
    {
        TMEngMethodInfo methiNew
        (
            L"PercentOf"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"PerVal", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_PercentOf = c2AddMethodInfo(methiNew);
    }

    // Unary addition
    {
        TMEngMethodInfo methiNew
        (
            L"PlusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_PlusEq = c2AddMethodInfo(methiNew);
    }

    // Set/clear a bit
    {
        TMEngMethodInfo methiNew
        (
            L"SetBit"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"BitNum", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"NewState", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_SetBit = c2AddMethodInfo(methiNew);
    }

    // Shift left and right
    {
        TMEngMethodInfo methiNew
        (
            L"ShiftLeft"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ShiftCount", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ShiftL = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ShiftRight"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ShiftCount", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ShiftR = c2AddMethodInfo(methiNew);
    }

    // Binary addition
    {
        TMEngMethodInfo methiNew
        (
            L"Sub"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_Sub = c2AddMethodInfo(methiNew);
    }

    // Swap byte order
    {
        TMEngMethodInfo methiNew
        (
            L"SwapBytes"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_SwapBytes = c2AddMethodInfo(methiNew);
    }

    // Unary and binary bitwise AND methods
    {
        TMEngMethodInfo methiNew
        (
            L"Xor"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToXor", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_Xor = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"XorEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToXor", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_XorEq = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngCard4Info::pmecvMakeStorage(const  TString&                strName
                                ,       TCIDMacroEngine&
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngCard4Val(strName, eConst);
}


// ---------------------------------------------------------------------------
//  TMEngCard4Info: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCard4Info::bInvokeMethod(          TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngCard4Val& mecvActual = static_cast<TMEngCard4Val&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_Add)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.c4Value() + meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_And)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.c4Value() & meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_AndEq)
    {
        mecvActual.c4Value(mecvActual.c4Value() & meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Dec)
    {
        const tCIDLib::TCard4 c4Cur = mecvActual.c4Value();
        TMEngCard4Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(c4Cur);
        if (c4Cur)
            mecvActual.c4Value(c4Cur - 1);
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Default ctor just zeros the value
        mecvActual.c4Value(0);
    }
     else if (c2MethId == m_c2MethId_Div)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.c4Value() / meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_DivEq)
    {
        mecvActual.c4Value(mecvActual.c4Value() / meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner) << mecvActual.c4Value();
    }
     else if (c2MethId == m_c2MethId_GetBit)
    {
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        // Create the mask for the bit they wanted
        tCIDLib::TCard4 c4Mask = 0x1;
        c4Mask <<= meOwner.c4StackValAt(c4FirstInd);
        mecvRet.bValue((mecvActual.c4Value() & c4Mask) != 0);
    }
     else if ((c2MethId == m_c2MethId_GetByte1)
          ||  (c2MethId == m_c2MethId_GetByte2)
          ||  (c2MethId == m_c2MethId_GetByte3)
          ||  (c2MethId == m_c2MethId_GetByte4))
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);

        if (c2MethId == m_c2MethId_GetByte1)
            mecvRet.c1Value(TRawBits::c1Low8From32(mecvActual.c4Value()));
        else if (c2MethId == m_c2MethId_GetByte2)
            mecvRet.c1Value(TRawBits::c1Sec8From32(mecvActual.c4Value()));
        else if (c2MethId == m_c2MethId_GetByte3)
            mecvRet.c1Value(TRawBits::c1Third8From32(mecvActual.c4Value()));
        else
            mecvRet.c1Value(TRawBits::c1High8From32(mecvActual.c4Value()));
    }
     else if (c2MethId == m_c2MethId_GetHighWord)
    {
        TMEngCard2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);
        mecvRet.c2Value(tCIDLib::TCard2(mecvActual.c4Value() >> 16));
    }
     else if (c2MethId == m_c2MethId_GetLowWord)
    {
        TMEngCard2Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);
        mecvRet.c2Value(tCIDLib::TCard2(mecvActual.c4Value() & 0xFFFF));
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
        const tCIDLib::TCard4 c4Parm = meOwner.c4StackValAt(c4FirstInd);

        if (c2MethId == m_c2MethId_GtThan)
            mecvRet.bValue(mecvActual.c4Value() > c4Parm);
        else if (c2MethId == m_c2MethId_GtThanEq)
            mecvRet.bValue(mecvActual.c4Value() >= c4Parm);
        else if (c2MethId == m_c2MethId_LsThan)
            mecvRet.bValue(mecvActual.c4Value() < c4Parm);
        else if (c2MethId == m_c2MethId_LsThanEq)
            mecvRet.bValue(mecvActual.c4Value() <= c4Parm);
        else
            mecvRet.bValue(mecvActual.c4Value() == c4Parm);
    }
     else if (c2MethId == m_c2MethId_Inc)
    {
        const tCIDLib::TCard4 c4Cur = mecvActual.c4Value();
        TMEngCard4Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(c4Cur);
        if (c4Cur < kCIDLib::c4MaxCard)
            mecvActual.c4Value(c4Cur + 1);
    }
     else if (c2MethId == m_c2MethId_MaxVal)
    {
        const tCIDLib::TCard4 c4Comp = meOwner.c4StackValAt(c4FirstInd);
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        if (c4Comp > mecvActual.c4Value())
            mecvRet.c4Value(c4Comp);
        else
            mecvRet.c4Value(mecvActual.c4Value());
    }
     else if (c2MethId == m_c2MethId_MinVal)
    {
        const tCIDLib::TCard4 c4Comp = meOwner.c4StackValAt(c4FirstInd);
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        if (c4Comp < mecvActual.c4Value())
            mecvRet.c4Value(c4Comp);
        else
            mecvRet.c4Value(mecvActual.c4Value());
    }
     else if (c2MethId == m_c2MethId_MinusEq)
    {
        const tCIDLib::TCard4 c4Sub = meOwner.c4StackValAt(c4FirstInd);
        mecvActual.c4Value(mecvActual.c4Value() - c4Sub);
    }
     else if (c2MethId == m_c2MethId_ModDiv)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.c4Value() % meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ModDivEq)
    {
        mecvActual.c4Value(mecvActual.c4Value() % meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Mul)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.c4Value() * meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_MulEq)
    {
        mecvActual.c4Value(mecvActual.c4Value() * meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Or)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.c4Value() | meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_OrEq)
    {
        mecvActual.c4Value(mecvActual.c4Value() | meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_PercentOf)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        const tCIDLib::TCard4 c4SrcVal = meOwner.c4StackValAt(c4FirstInd);

        if (!c4SrcVal)
        {
            mecvRet.c4Value(0);
        }
         else
        {
            mecvRet.c4Value
            (
                tCIDLib::TCard4
                (
                    (tCIDLib::TFloat4(c4SrcVal)
                     / tCIDLib::TFloat4(mecvActual.c4Value())) * 100.0
                )
            );
        }
    }
     else if (c2MethId == m_c2MethId_PlusEq)
    {
        const tCIDLib::TCard4 c4Add = meOwner.c4StackValAt(c4FirstInd);
        mecvActual.c4Value(mecvActual.c4Value() + c4Add);
    }
     else if (c2MethId == m_c2MethId_SetBit)
    {
        const tCIDLib::TCard4 c4Bit = meOwner.c4StackValAt(c4FirstInd);
        tCIDLib::TCard4 c4Val = mecvActual.c4Value();
        if (meOwner.bStackValAt(c4FirstInd + 1))
            c4Val |= (0x1UL << c4Bit);
        else
            c4Val &= ~(0x1UL << c4Bit);
        mecvActual.c4Value(c4Val);
    }
     else if ((c2MethId == m_c2MethId_ShiftL)
          ||  (c2MethId == m_c2MethId_ShiftR))
    {
        tCIDLib::TCard4 c4Tmp = mecvActual.c4Value();
        tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd);

        if (c2MethId == m_c2MethId_ShiftR)
            mecvActual.c4Value(c4Tmp >> c4Count);
        else
            mecvActual.c4Value(c4Tmp << c4Count);
    }
     else if (c2MethId == m_c2MethId_Sub)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.c4Value() - meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_SwapBytes)
    {
        mecvActual.SwapBytes();
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        mecvActual.c4Value(meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Xor)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.c4Value() ^ meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_XorEq)
    {
        mecvActual.c4Value(mecvActual.c4Value() ^ meOwner.c4StackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngCard8Val
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCard8Val: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCard8Val::TMEngCard8Val(const  TString&                strName
                            , const tCIDMacroEng::EConstTypes  eConst
                            , const tCIDLib::TCard8         c8Init) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card8), eConst)
    , m_c8Value(c8Init)
{
}

TMEngCard8Val::~TMEngCard8Val()
{
}


// ---------------------------------------------------------------------------
//  TMEngCard8Val: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCard8Val::bDbgFormat(          TTextOutStream&     strmTarget
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices   eRadix
                            , const TCIDMacroEngine&    ) const
{
    if (eRadix == tCIDLib::ERadices::Hex)
        strmTarget << L"0x";
    strmTarget << TCardinal64(m_c8Value, eRadix);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMEngCard8Val::bParseFromText(  const   TString&        strValue
                                , const TMEngClassInfo& )
{
    tCIDLib::TBoolean bRes;
    tCIDLib::TCard8 c8Val = TRawStr::c8AsBinary(strValue.pszBuffer(), bRes);
    if (bRes)
        m_c8Value = c8Val;
    return bRes;
}


tCIDLib::TVoid
TMEngCard8Val::CopyFrom(const   TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    m_c8Value = static_cast<const TMEngCard8Val&>(mecvToCopy).m_c8Value;
}



// ---------------------------------------------------------------------------
//  TMEngCard8Val: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngCard8Val::Add(const tCIDLib::TCard8 c8ToAdd)
{
    m_c8Value += c8ToAdd;
}


tCIDLib::TCard8 TMEngCard8Val::c8Value() const
{
    return m_c8Value;
}

tCIDLib::TCard8 TMEngCard8Val::c8Value(const tCIDLib::TCard8 c8ToSet)
{
    m_c8Value = c8ToSet;
    return m_c8Value;
}


tCIDLib::TVoid TMEngCard8Val::SwapBytes()
{
    m_c8Value = TRawBits::c8SwapBytes(m_c8Value);
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngCard8Info
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCard8Info: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngCard8Info::strPath()
{
    return CIDMacroEng_CardClasses::strCard8ClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngCard8Info: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCard8Info::TMEngCard8Info(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_CardClasses::strCard8
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2MethId_Add(kMacroEng::c2BadId)
    , m_c2MethId_And(kMacroEng::c2BadId)
    , m_c2MethId_AndEq(kMacroEng::c2BadId)
    , m_c2MethId_Dec(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Div(kMacroEng::c2BadId)
    , m_c2MethId_DivEq(kMacroEng::c2BadId)
    , m_c2MethId_Equal(kMacroEng::c2BadId)
    , m_c2MethId_GetBit(kMacroEng::c2BadId)
    , m_c2MethId_GetByte(kMacroEng::c2BadId)
    , m_c2MethId_GetHighDWord(kMacroEng::c2BadId)
    , m_c2MethId_GetLowDWord(kMacroEng::c2BadId)
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
    , m_c2MethId_Or(kMacroEng::c2BadId)
    , m_c2MethId_OrEq(kMacroEng::c2BadId)
    , m_c2MethId_PercentOf(kMacroEng::c2BadId)
    , m_c2MethId_PlusEq(kMacroEng::c2BadId)
    , m_c2MethId_SetBit(kMacroEng::c2BadId)
    , m_c2MethId_ShiftL(kMacroEng::c2BadId)
    , m_c2MethId_ShiftR(kMacroEng::c2BadId)
    , m_c2MethId_Sub(kMacroEng::c2BadId)
    , m_c2MethId_SwapBytes(kMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kMacroEng::c2BadId)
    , m_c2MethId_Xor(kMacroEng::c2BadId)
    , m_c2MethId_XorEq(kMacroEng::c2BadId)
{
}

TMEngCard8Info::~TMEngCard8Info()
{
}


// ---------------------------------------------------------------------------
//  TMEngCard8Info: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngCard8Info::bCanCastFrom(const tCIDLib::TCard2 c2SrcId)
{
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
        case tCIDMacroEng::EIntrinsics::Card1 :
        case tCIDMacroEng::EIntrinsics::Card2 :
        case tCIDMacroEng::EIntrinsics::Card4 :
        case tCIDMacroEng::EIntrinsics::Card8 :
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
TMEngCard8Info::eCastFrom(          TCIDMacroEngine&
                            , const TMEngClassVal&      mecvSrc
                            ,       TMEngClassVal&      mecvTarget)
{
    // Get the source value
    const tCIDLib::TCard2 c2SrcId = mecvSrc.c2ClassId();

    tCIDLib::TCard8 c8New;
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
            if (static_cast<const TMEngBooleanVal&>(mecvSrc).bValue())
                c8New = 1;
            else
                c8New = 0;
            break;

        case tCIDMacroEng::EIntrinsics::Card1 :
            c8New = static_cast<const TMEngCard1Val&>(mecvSrc).c1Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card2 :
            c8New = static_cast<const TMEngCard2Val&>(mecvSrc).c2Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card4 :
            c8New = static_cast<const TMEngCard4Val&>(mecvSrc).c4Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card8 :
            c8New = static_cast<const TMEngCard8Val&>(mecvSrc).c8Value();
            break;

        case tCIDMacroEng::EIntrinsics::Int1 :
            c8New = tCIDLib::TCard8(static_cast<const TMEngInt1Val&>(mecvSrc).i1Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int2 :
            c8New = tCIDLib::TCard8(static_cast<const TMEngInt2Val&>(mecvSrc).i2Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int4 :
            c8New = tCIDLib::TCard8(static_cast<const TMEngInt4Val&>(mecvSrc).i4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Float4 :
            c8New = tCIDLib::TCard8(static_cast<const TMEngFloat4Val&>(mecvSrc).f4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Float8 :
            c8New = tCIDLib::TCard8(static_cast<const TMEngFloat8Val&>(mecvSrc).f8Value());
            break;

        default :
            return tCIDMacroEng::ECastRes::Incompatible;
    };

    // Cast the target to our type, which it must be
    TMEngCard8Val& mecvActual = static_cast<TMEngCard8Val&>(mecvTarget);
    mecvActual.c8Value(c8New);
    return tCIDMacroEng::ECastRes::Ok;
}


tCIDLib::TVoid TMEngCard8Info::Init(TCIDMacroEngine&)
{
    // Add a literal for our max value
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kMaxValue"
            , tCIDMacroEng::EIntrinsics::Card8
            , new TMEngCard8Val(L"kMaxValue", tCIDMacroEng::EConstTypes::Const, kCIDLib::c8MaxCard)
        )
    );

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.Card8"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that takes an initial value
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.Card8"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"InitVal", tCIDMacroEng::EIntrinsics::Card8);
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_ValCtor = c2AddMethodInfo(methiNew);
    }

    // Binary addition
    {
        TMEngMethodInfo methiNew
        (
            L"Add"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_Add = c2AddMethodInfo(methiNew);
    }

    // Unary and binary bitwise AND methods
    {
        TMEngMethodInfo methiNew
        (
            L"And"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToAnd", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_And = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"AndEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAnd", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_AndEq = c2AddMethodInfo(methiNew);
    }

    // Add the decrement method
    {
        TMEngMethodInfo methiNew
        (
            L"Dec"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Dec = c2AddMethodInfo(methiNew);
    }

    // Add the div and diveq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Div"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Card8);
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
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Card8);
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
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_Equal = c2AddMethodInfo(methiNew);
    }

    // Get a bit state
    {
        TMEngMethodInfo methiNew
        (
            L"GetBit"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"BitNum", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetBit = c2AddMethodInfo(methiNew);
    }

    // Get one of the bytes of the Card8
    {
        TMEngMethodInfo methiNew
        (
            L"GetByte"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Index", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetByte = c2AddMethodInfo(methiNew);
    }

    // Get the low and high byte
    {
        TMEngMethodInfo methiNew
        (
            L"GetHighDWord"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetHighDWord = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetLowDWord"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetLowDWord = c2AddMethodInfo(methiNew);
    }

    // Add the greater than and greater/equal method
    {
        TMEngMethodInfo methiNew
        (
            L"GtThan"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Card8);
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
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_GtThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the increment method
    {
        TMEngMethodInfo methiNew
        (
            L"Inc"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Inc = c2AddMethodInfo(methiNew);
    }

    // Add the less than and less/equal method
    {
        TMEngMethodInfo methiNew
        (
            L"LsThan"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Card8);
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
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_LsThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the max/min value methods
    {
        TMEngMethodInfo methiNew
        (
            L"MaxVal"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_MaxVal = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"MinVal"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_MinVal = c2AddMethodInfo(methiNew);
    }

    // Unary subtraction
    {
        TMEngMethodInfo methiNew
        (
            L"MinusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_MinusEq = c2AddMethodInfo(methiNew);
    }

    // Add the moddiv and moddiveq methods
    {
        TMEngMethodInfo methiNew
        (
            L"ModDiv"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Divisor", tCIDMacroEng::EIntrinsics::Card8);
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
        methiNew.c2AddInParm(L"Divisor", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_ModDivEq = c2AddMethodInfo(methiNew);
    }

    // Add the mul and muleq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Mul"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Card8);
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
        methiNew.c2AddInParm(L"Value", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_MulEq = c2AddMethodInfo(methiNew);
    }

    // Unary and binary bitwise OR methods
    {
        TMEngMethodInfo methiNew
        (
            L"Or"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToOr", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_Or = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"OrEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToOr", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_OrEq = c2AddMethodInfo(methiNew);
    }

    // Calc the percent of us that the passed value represents
    {
        TMEngMethodInfo methiNew
        (
            L"PercentOf"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"PerVal", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_PercentOf = c2AddMethodInfo(methiNew);
    }

    // Unary addition
    {
        TMEngMethodInfo methiNew
        (
            L"PlusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_PlusEq = c2AddMethodInfo(methiNew);
    }

    // Set/clear a bit
    {
        TMEngMethodInfo methiNew
        (
            L"SetBit"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"BitNum", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"NewState", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_SetBit = c2AddMethodInfo(methiNew);
    }

    // Shift left and right
    {
        TMEngMethodInfo methiNew
        (
            L"ShiftLeft"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ShiftCount", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ShiftL = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ShiftRight"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ShiftCount", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ShiftR = c2AddMethodInfo(methiNew);
    }

    // Binary addition
    {
        TMEngMethodInfo methiNew
        (
            L"Sub"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_Sub = c2AddMethodInfo(methiNew);
    }

    // Swap byte order
    {
        TMEngMethodInfo methiNew
        (
            L"SwapBytes"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_SwapBytes = c2AddMethodInfo(methiNew);
    }

    // Unary and binary bitwise AND methods
    {
        TMEngMethodInfo methiNew
        (
            L"Xor"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToXor", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_Xor = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"XorEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToXor", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_XorEq = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngCard8Info::pmecvMakeStorage(const  TString&                strName
                                ,       TCIDMacroEngine&
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngCard8Val(strName, eConst);
}


// ---------------------------------------------------------------------------
//  TMEngCard8Info: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCard8Info::bInvokeMethod(          TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngCard8Val& mecvActual = static_cast<TMEngCard8Val&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_Add)
    {
        TMEngCard8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(mecvActual.c8Value() + meOwner.c8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_And)
    {
        TMEngCard8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(mecvActual.c8Value() & meOwner.c8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_AndEq)
    {
        mecvActual.c8Value(mecvActual.c8Value() & meOwner.c8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Dec)
    {
        const tCIDLib::TCard8 c4Cur = mecvActual.c8Value();
        TMEngCard8Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(c4Cur);
        if (c4Cur)
            mecvActual.c8Value(c4Cur - 1);
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Default ctor just zeros the value
        mecvActual.c8Value(0);
    }
     else if (c2MethId == m_c2MethId_Div)
    {
        TMEngCard8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(mecvActual.c8Value() / meOwner.c8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_DivEq)
    {
        mecvActual.c8Value(mecvActual.c8Value() / meOwner.c8StackValAt(c4FirstInd));
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner) << mecvActual.c8Value();
    }
     else if (c2MethId == m_c2MethId_GetBit)
    {
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        // Create the mask for the bit they wanted
        tCIDLib::TCard8 c4Mask = 0x1;
        c4Mask <<= meOwner.c4StackValAt(c4FirstInd);
        mecvRet.bValue((mecvActual.c8Value() & c4Mask) != 0);
    }
     else if (c2MethId == m_c2MethId_GetByte)
    {
        TMEngCard1Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);

        // Get the byte index and do modulo 8
        tCIDLib::TCard4 c4Byte = meOwner.c4StackValAt(c4FirstInd);
        c4Byte &= 0x7;

        //
        //  Get the value and shift it down by the byte times 8 bits, and
        //  then clip it to the low byte.
        //
        tCIDLib::TCard8 c8Ret = mecvActual.c8Value();
        //
        //  The analyzer is confused here. We aren't assigning the shipped
        //  value to the 64 bit value. We are creating a shift count.
        //
        #pragma warning(suppress : 6297)
        c8Ret >>= (c4Byte << 3);
        c8Ret &= 0xFF;
        mecvRet.c1Value(tCIDLib::TCard1(c8Ret));
    }
     else if (c2MethId == m_c2MethId_GetHighDWord)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(tCIDLib::TCard4(mecvActual.c8Value() >> 32));
    }
     else if (c2MethId == m_c2MethId_GetLowDWord)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(tCIDLib::TCard4(mecvActual.c8Value() & 0xFFFFFFFF));
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
        const tCIDLib::TCard8 c4Parm = meOwner.c8StackValAt(c4FirstInd);

        if (c2MethId == m_c2MethId_GtThan)
            mecvRet.bValue(mecvActual.c8Value() > c4Parm);
        else if (c2MethId == m_c2MethId_GtThanEq)
            mecvRet.bValue(mecvActual.c8Value() >= c4Parm);
        else if (c2MethId == m_c2MethId_LsThan)
            mecvRet.bValue(mecvActual.c8Value() < c4Parm);
        else if (c2MethId == m_c2MethId_LsThanEq)
            mecvRet.bValue(mecvActual.c8Value() <= c4Parm);
        else
            mecvRet.bValue(mecvActual.c8Value() == c4Parm);
    }
     else if (c2MethId == m_c2MethId_Inc)
    {
        const tCIDLib::TCard8 c4Cur = mecvActual.c8Value();
        TMEngCard8Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(c4Cur);
        if (c4Cur < kCIDLib::c4MaxCard)
            mecvActual.c8Value(c4Cur + 1);
    }
     else if (c2MethId == m_c2MethId_MaxVal)
    {
        const tCIDLib::TCard8 c8Comp = meOwner.c8StackValAt(c4FirstInd);
        TMEngCard8Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        if (c8Comp > mecvActual.c8Value())
            mecvRet.c8Value(c8Comp);
        else
            mecvRet.c8Value(mecvActual.c8Value());
    }
     else if (c2MethId == m_c2MethId_MinVal)
    {
        const tCIDLib::TCard8 c8Comp = meOwner.c8StackValAt(c4FirstInd);
        TMEngCard8Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        if (c8Comp < mecvActual.c8Value())
            mecvRet.c8Value(c8Comp);
        else
            mecvRet.c8Value(mecvActual.c8Value());
    }
     else if (c2MethId == m_c2MethId_MinusEq)
    {
        const tCIDLib::TCard8 c8Sub = meOwner.c8StackValAt(c4FirstInd);
        mecvActual.c8Value(mecvActual.c8Value() - c8Sub);
    }
     else if (c2MethId == m_c2MethId_ModDiv)
    {
        TMEngCard8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(mecvActual.c8Value() % meOwner.c8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ModDivEq)
    {
        mecvActual.c8Value(mecvActual.c8Value() % meOwner.c8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Mul)
    {
        TMEngCard8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(mecvActual.c8Value() * meOwner.c8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_MulEq)
    {
        mecvActual.c8Value(mecvActual.c8Value() * meOwner.c8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Or)
    {
        TMEngCard8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(mecvActual.c8Value() | meOwner.c8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_OrEq)
    {
        mecvActual.c8Value(mecvActual.c8Value() | meOwner.c8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_PercentOf)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        const tCIDLib::TCard8 c8SrcVal = meOwner.c8StackValAt(c4FirstInd);

        if (!c8SrcVal)
        {
            mecvRet.c4Value(0);
        }
         else
        {
            mecvRet.c4Value
            (
                tCIDLib::TCard4
                (
                    (tCIDLib::TFloat8(c8SrcVal)
                     / tCIDLib::TFloat8(mecvActual.c8Value())) * 100.0
                )
            );
        }
    }
     else if (c2MethId == m_c2MethId_PlusEq)
    {
        const tCIDLib::TCard8 c8Add = meOwner.c8StackValAt(c4FirstInd);
        mecvActual.Add(c8Add);
    }
     else if (c2MethId == m_c2MethId_SetBit)
    {
        // Get the bit and insure it's modulo 64
        tCIDLib::TCard4 c4Bit = meOwner.c4StackValAt(c4FirstInd);
        c4Bit %= 64;
        tCIDLib::TCard8 c8Val = mecvActual.c8Value();
        tCIDLib::TCard8 c8Mask = 0x1;
        c8Mask <<= c4Bit;
        if (meOwner.bStackValAt(c4FirstInd + 1))
            c8Val |= c8Mask;
        else
            c8Val &= ~c8Mask;
        mecvActual.c8Value(c8Val);
    }
     else if ((c2MethId == m_c2MethId_ShiftL)
          ||  (c2MethId == m_c2MethId_ShiftR))
    {
        tCIDLib::TCard8 c8Tmp = mecvActual.c8Value();
        tCIDLib::TCard4 c4Count = meOwner.c4StackValAt(c4FirstInd);

        if (c2MethId == m_c2MethId_ShiftR)
            mecvActual.c8Value(c8Tmp >> c4Count);
        else
            mecvActual.c8Value(c8Tmp << c4Count);
    }
     else if (c2MethId == m_c2MethId_Sub)
    {
        TMEngCard8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(mecvActual.c8Value() - meOwner.c8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_SwapBytes)
    {
        mecvActual.SwapBytes();
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        mecvActual.c8Value(meOwner.c8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Xor)
    {
        TMEngCard8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(mecvActual.c8Value() ^ meOwner.c8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_XorEq)
    {
        mecvActual.c8Value(mecvActual.c8Value() ^ meOwner.c8StackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



