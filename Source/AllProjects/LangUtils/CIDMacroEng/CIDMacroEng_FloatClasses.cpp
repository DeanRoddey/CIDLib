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
RTTIDecls(TMEngFloat4Val,TMEngClassVal)
RTTIDecls(TMEngFloat4Info,TMEngClassInfo)
RTTIDecls(TMEngFloat8Val,TMEngClassVal)
RTTIDecls(TMEngFloat8Info,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_FloatClasses
{
    namespace
    {
        // -----------------------------------------------------------------------
        //  The names for the types that we support here. Each derivative has to
        //  be able to return strings that contain its name and full name.
        // -----------------------------------------------------------------------
        const TString   strFloat4(L"Float4");
        const TString   strFloat8(L"Float8");

        const TString   strFloat4ClassPath(L"MEng.Float4");
        const TString   strFloat8ClassPath(L"MEng.Float8");
    }
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngFloat4Val
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngFloat4Val: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngFloat4Val::TMEngFloat4Val( const   TString&                strName
                                , const tCIDMacroEng::EConstTypes  eConst
                                , const tCIDLib::TFloat4        f4Init) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Float4), eConst)
    , m_f4Value(f4Init)
{
}

TMEngFloat4Val::~TMEngFloat4Val()
{
}


// ---------------------------------------------------------------------------
//  TMEngFloat4Val: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TFloat4 TMEngFloat4Val::f4Value() const
{
    return m_f4Value;
}

tCIDLib::TFloat4 TMEngFloat4Val::f4Value(const tCIDLib::TFloat4 f4ToSet)
{
    m_f4Value = f4ToSet;
    return m_f4Value;
}


// ---------------------------------------------------------------------------
//  TMEngFloat4Val: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngFloat4Val::bDbgFormat(         TTextOutStream&     strmTarget
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices
                            , const TCIDMacroEngine&    ) const
{
    strmTarget << TFloat(m_f4Value, 4);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMEngFloat4Val::bParseFromText(const TString& strValue, const TMEngClassInfo&)
{
    tCIDLib::TBoolean bRes;
    tCIDLib::TFloat8 f8Val = TRawStr::f8AsBinary(strValue.pszBuffer(), bRes);
    if (bRes)
        bRes = (f8Val >= -kCIDLib::f4MaxFloat) && (f8Val <= kCIDLib::f4MaxFloat);
    if (bRes)
        m_f4Value = tCIDLib::TFloat4(f8Val);
    return bRes;
}


tCIDLib::TVoid
TMEngFloat4Val::CopyFrom(const  TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    m_f4Value = static_cast<const TMEngFloat4Val&>(mecvToCopy).m_f4Value;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngFloat4Info
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngFloat4Info: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngFloat4Info::strPath()
{
    return CIDMacroEng_FloatClasses::strFloat4ClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngFloat4Info: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngFloat4Info::TMEngFloat4Info(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_FloatClasses::strFloat4
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2MethId_Abs(kCIDMacroEng::c2BadId)
    , m_c2MethId_Add(kCIDMacroEng::c2BadId)
    , m_c2MethId_ArcCosine(kCIDMacroEng::c2BadId)
    , m_c2MethId_ArcSine(kCIDMacroEng::c2BadId)
    , m_c2MethId_ArcTangent(kCIDMacroEng::c2BadId)
    , m_c2MethId_Ceiling(kCIDMacroEng::c2BadId)
    , m_c2MethId_Cosine(kCIDMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_Div(kCIDMacroEng::c2BadId)
    , m_c2MethId_DivEq(kCIDMacroEng::c2BadId)
    , m_c2MethId_Equal(kCIDMacroEng::c2BadId)
    , m_c2MethId_Floor(kCIDMacroEng::c2BadId)
    , m_c2MethId_GtThan(kCIDMacroEng::c2BadId)
    , m_c2MethId_GtThanEq(kCIDMacroEng::c2BadId)
    , m_c2MethId_LsThan(kCIDMacroEng::c2BadId)
    , m_c2MethId_LsThanEq(kCIDMacroEng::c2BadId)
    , m_c2MethId_MaxVal(kCIDMacroEng::c2BadId)
    , m_c2MethId_MinVal(kCIDMacroEng::c2BadId)
    , m_c2MethId_MinusEq(kCIDMacroEng::c2BadId)
    , m_c2MethId_Mul(kCIDMacroEng::c2BadId)
    , m_c2MethId_MulEq(kCIDMacroEng::c2BadId)
    , m_c2MethId_Negate(kCIDMacroEng::c2BadId)
    , m_c2MethId_NLog(kCIDMacroEng::c2BadId)
    , m_c2MethId_PlusEq(kCIDMacroEng::c2BadId)
    , m_c2MethId_Power(kCIDMacroEng::c2BadId)
    , m_c2MethId_Round(kCIDMacroEng::c2BadId)
    , m_c2MethId_Sine(kCIDMacroEng::c2BadId)
    , m_c2MethId_Sub(kCIDMacroEng::c2BadId)
    , m_c2MethId_Split(kCIDMacroEng::c2BadId)
    , m_c2MethId_SqrRoot(kCIDMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kCIDMacroEng::c2BadId)
{
    // Add imports for any non-intrinsic classes we use in our signatures
    bAddClassImport(TMEngBaseInfoInfo::strPath());
}

TMEngFloat4Info::~TMEngFloat4Info()
{
}


// ---------------------------------------------------------------------------
//  TMEngFloat4Info: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngFloat4Info::bCanCastFrom(const tCIDLib::TCard2 c2SrcId)
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
TMEngFloat4Info::eCastFrom(         TCIDMacroEngine&
                            , const TMEngClassVal&      mecvSrc
                            ,       TMEngClassVal&      mecvTarget)
{
    // Get the source value
    const tCIDLib::TCard2 c2SrcId = mecvSrc.c2ClassId();

    tCIDLib::TFloat4 f4New = 0;
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
            if (static_cast<const TMEngBooleanVal&>(mecvSrc).bValue())
                f4New = 1;
            else
                f4New = 0;
            break;

        case tCIDMacroEng::EIntrinsics::Card1 :
            f4New = static_cast<const TMEngCard1Val&>(mecvSrc).c1Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card2 :
            f4New = static_cast<const TMEngCard2Val&>(mecvSrc).c2Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card4 :
            f4New = tCIDLib::TFloat4(static_cast<const TMEngCard4Val&>(mecvSrc).c4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Card8 :
            f4New = tCIDLib::TFloat4(static_cast<const TMEngCard8Val&>(mecvSrc).c8Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int1 :
            f4New = static_cast<const TMEngInt1Val&>(mecvSrc).i1Value();
            break;

        case tCIDMacroEng::EIntrinsics::Int2 :
            f4New = static_cast<const TMEngInt2Val&>(mecvSrc).i2Value();
            break;

        case tCIDMacroEng::EIntrinsics::Int4 :
            f4New = tCIDLib::TFloat4(static_cast<const TMEngInt4Val&>(mecvSrc).i4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Float4 :
            f4New = static_cast<const TMEngFloat4Val&>(mecvSrc).f4Value();
            break;

        case tCIDMacroEng::EIntrinsics::Float8 :
            f4New = tCIDLib::TFloat4(static_cast<const TMEngFloat8Val&>(mecvSrc).f8Value());
            break;

        default :
            return tCIDMacroEng::ECastRes::Incompatible;
    };

    // Cast the target to our type, which it must be
    TMEngFloat4Val& mecvActual = static_cast<TMEngFloat4Val&>(mecvTarget);
    mecvActual.f4Value(f4New);
    return tCIDMacroEng::ECastRes::Ok;
}


tCIDLib::TVoid TMEngFloat4Info::Init(TCIDMacroEngine& meOwner)
{
    // Add a literal for PI
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kPI"
            , tCIDMacroEng::EIntrinsics::Float4
            , new TMEngFloat4Val(L"kPI", tCIDMacroEng::EConstTypes::Const, kCIDLib::f4PI)
        )
    );

    // Add a literal for PI*2
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"k2PI"
            , tCIDMacroEng::EIntrinsics::Float4
            , new TMEngFloat4Val(L"k2PI", tCIDMacroEng::EConstTypes::Const, kCIDLib::f4PIx2)
        )
    );


    const tCIDLib::TCard2 c2RndTypes = meOwner.c2FindClassId
    (
        TMEngBaseInfoInfo::strUpDnClPath()
    );



    // Return the absolute value of this object
    {
        TMEngMethodInfo methiNew
        (
            L"Abs"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_Abs = c2AddMethodInfo(methiNew);
    }

    // Add this value and another and return the result
    {
        TMEngMethodInfo methiNew
        (
            L"Add"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Float4);
        m_c2MethId_Add = c2AddMethodInfo(methiNew);
    }

    // Calculate the ArcCosine, ArcSine, and ArcTangent
    {
        TMEngMethodInfo methiNew
        (
            L"ArcCosine"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_ArcCosine = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ArcSine"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_ArcSine = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ArcTangent"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_ArcTangent = c2AddMethodInfo(methiNew);
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.Float4"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Do a standard ceiling operation
    {
        TMEngMethodInfo methiNew
        (
            L"Ceiling"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_Ceiling = c2AddMethodInfo(methiNew);
    }

    // Return the cosine of this value
    {
        TMEngMethodInfo methiNew
        (
            L"Cosine"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_Cosine = c2AddMethodInfo(methiNew);
    }

    // Divide this value by another and return the result
    {
        TMEngMethodInfo methiNew
        (
            L"Div"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"DivBy", tCIDMacroEng::EIntrinsics::Float4);
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
        methiNew.c2AddInParm(L"DivBy", tCIDMacroEng::EIntrinsics::Float4);
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
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Float4);
        m_c2MethId_Equal = c2AddMethodInfo(methiNew);
    }

    // Do a standard floor operation
    {
        TMEngMethodInfo methiNew
        (
            L"Floor"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_Floor = c2AddMethodInfo(methiNew);
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
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Float4);
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
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Float4);
        m_c2MethId_GtThanEq = c2AddMethodInfo(methiNew);
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
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Float4);
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
        methiNew.c2AddInParm(L"ToComp", tCIDMacroEng::EIntrinsics::Float4);
        m_c2MethId_LsThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the max/min value methods
    {
        TMEngMethodInfo methiNew
        (
            L"MaxVal"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Float4);
        m_c2MethId_MaxVal = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"MinVal"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Float4);
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
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Float4);
        m_c2MethId_MinusEq = c2AddMethodInfo(methiNew);
    }

    // Add the mul and muleq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Mul"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"MulBy", tCIDMacroEng::EIntrinsics::Float4);
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
        methiNew.c2AddInParm(L"MulBy", tCIDMacroEng::EIntrinsics::Float4);
        m_c2MethId_MulEq = c2AddMethodInfo(methiNew);
    }

    // Negate our value
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

    // Return the natural log of this value
    {
        TMEngMethodInfo methiNew
        (
            L"NLog"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_NLog = c2AddMethodInfo(methiNew);
    }

    // Add the pluseq method
    {
        TMEngMethodInfo methiNew
        (
            L"PlusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Float4);
        m_c2MethId_PlusEq = c2AddMethodInfo(methiNew);
    }

    // Raise this value to the indicated power
    {
        TMEngMethodInfo methiNew
        (
            L"Power"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Power", tCIDMacroEng::EIntrinsics::Float4);
        m_c2MethId_Power = c2AddMethodInfo(methiNew);
    }

    // Round this value
    {
        TMEngMethodInfo methiNew
        (
            L"Round"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"RoundType", c2RndTypes);
        m_c2MethId_Round = c2AddMethodInfo(methiNew);
    }

    // Return the sine of this value
    {
        TMEngMethodInfo methiNew
        (
            L"Sine"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_Sine = c2AddMethodInfo(methiNew);
    }

    // Split out the fractional and integral parts
    {
        TMEngMethodInfo methiNew
        (
            L"Split"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"Integral", tCIDMacroEng::EIntrinsics::Float4);
        m_c2MethId_Split = c2AddMethodInfo(methiNew);
    }

    // Return the square root of this value
    {
        TMEngMethodInfo methiNew
        (
            L"SqrRoot"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_SqrRoot = c2AddMethodInfo(methiNew);
    }

    // Sub a value from this value and return the result
    {
        TMEngMethodInfo methiNew
        (
            L"Sub"
            , tCIDMacroEng::EIntrinsics::Float4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Float4);
        m_c2MethId_Sub = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that takes an initial value
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.Float4"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"InitVal", tCIDMacroEng::EIntrinsics::Float4);
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_ValCtor = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngFloat4Info::pmecvMakeStorage(  const   TString&                strName
                                    ,       TCIDMacroEngine&
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngFloat4Val(strName, eConst);
}


// ---------------------------------------------------------------------------
//  TMEngFloat4Info: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngFloat4Info::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngFloat4Val& mecvActual = static_cast<TMEngFloat4Val&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_Abs)
    {
        TMEngFloat4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        mecvRet.f4Value(TMathLib::f4Abs(mecvActual.f4Value()));
    }
     else if (c2MethId == m_c2MethId_ArcCosine)
    {
        TMEngFloat4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        mecvRet.f4Value(TMathLib::f4ArcCosine(mecvActual.f4Value()));
    }
     else if (c2MethId == m_c2MethId_ArcSine)
    {
        TMEngFloat4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        mecvRet.f4Value(TMathLib::f4ArcSine(mecvActual.f4Value()));
    }
     else if (c2MethId == m_c2MethId_ArcTangent)
    {
        TMEngFloat4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        mecvRet.f4Value(TMathLib::f4ArcTangent(mecvActual.f4Value()));
    }
     else if (c2MethId == m_c2MethId_Add)
    {
        TMEngFloat4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        mecvRet.f4Value(mecvActual.f4Value() + meOwner.f4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Ceiling)
    {
        TMEngFloat4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        mecvRet.f4Value(TMathLib::f4Ceil(mecvActual.f4Value()));
    }
     else if (c2MethId == m_c2MethId_Cosine)
    {
        TMEngFloat4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        mecvRet.f4Value(TMathLib::f4Cosine(mecvActual.f4Value()));
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Default ctor just zeros the value
        mecvActual.f4Value(0);
    }
     else if (c2MethId == m_c2MethId_Div)
    {
        TMEngFloat4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        mecvRet.f4Value(mecvActual.f4Value() / meOwner.f4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_DivEq)
    {
        mecvActual.f4Value(mecvActual.f4Value() / meOwner.f4StackValAt(c4FirstInd));
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner) << mecvActual.f4Value();
    }
     else if (c2MethId == m_c2MethId_Floor)
    {
        TMEngFloat4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        mecvRet.f4Value(TMathLib::f4Floor(mecvActual.f4Value()));
    }
     else if ((c2MethId == m_c2MethId_Equal)
          ||  (c2MethId == m_c2MethId_GtThan)
          ||  (c2MethId == m_c2MethId_GtThanEq)
          ||  (c2MethId == m_c2MethId_LsThanEq)
          ||  (c2MethId == m_c2MethId_LsThan))
    {
        //
        //  These set the boolean return value, based on our relation
        //  to a passed float4 value.
        //
        TMEngBooleanVal& mecvRet
                = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        const tCIDLib::TFloat4 f4Parm = meOwner.f4StackValAt(c4FirstInd);

        if (c2MethId == m_c2MethId_GtThan)
            mecvRet.bValue(mecvActual.f4Value() > f4Parm);
        else if (c2MethId == m_c2MethId_GtThanEq)
            mecvRet.bValue(mecvActual.f4Value() >= f4Parm);
        else if (c2MethId == m_c2MethId_LsThan)
            mecvRet.bValue(mecvActual.f4Value() < f4Parm);
        else if (c2MethId == m_c2MethId_LsThanEq)
            mecvRet.bValue(mecvActual.f4Value() <= f4Parm);
        else
            mecvRet.bValue(mecvActual.f4Value() == f4Parm);
    }
     else if (c2MethId == m_c2MethId_MaxVal)
    {
        const tCIDLib::TFloat4 f4Comp = meOwner.f4StackValAt(c4FirstInd);
        TMEngFloat4Val& mecvRet = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        if (f4Comp > mecvActual.f4Value())
            mecvRet.f4Value(f4Comp);
        else
            mecvRet.f4Value(mecvActual.f4Value());
    }
     else if (c2MethId == m_c2MethId_MinVal)
    {
        const tCIDLib::TFloat4 f4Comp = meOwner.f4StackValAt(c4FirstInd);
        TMEngFloat4Val& mecvRet = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        if (f4Comp < mecvActual.f4Value())
            mecvRet.f4Value(f4Comp);
        else
            mecvRet.f4Value(mecvActual.f4Value());
    }
     else if (c2MethId == m_c2MethId_MinusEq)
    {
        mecvActual.f4Value(mecvActual.f4Value() - meOwner.f4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Mul)
    {
        TMEngFloat4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        mecvRet.f4Value(mecvActual.f4Value() * meOwner.f4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_MulEq)
    {
        mecvActual.f4Value(mecvActual.f4Value() * meOwner.f4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Negate)
    {
        mecvActual.f4Value(mecvActual.f4Value() * -1);
    }
     else if (c2MethId == m_c2MethId_NLog)
    {
        TMEngFloat4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        mecvRet.f4Value(TMathLib::f4Log(mecvActual.f4Value()));
    }
     else if (c2MethId == m_c2MethId_PlusEq)
    {
        mecvActual.f4Value(mecvActual.f4Value() + meOwner.f4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Power)
    {
        mecvActual.f4Value
        (
            TMathLib::f4Power(mecvActual.f4Value(), meOwner.f4StackValAt(c4FirstInd))
        );
    }
     else if (c2MethId == m_c2MethId_Round)
    {
        TMEngEnumVal& mecvType = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        tCIDLib::TFloat4 f4Val = mecvActual.f4Value();
        const TMEngEnumInfo& meciType = meOwner.meciFindAs<TMEngEnumInfo>
        (
            mecvType.c2ClassId()
        );
        TMathLib::Round(f4Val, tCIDLib::ERoundTypes(meciType.c4MapValue(mecvType)));
        mecvActual.f4Value(f4Val);
    }
     else if (c2MethId == m_c2MethId_Sine)
    {
        TMEngFloat4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        mecvRet.f4Value(TMathLib::f4Sine(mecvActual.f4Value()));
    }
     else if (c2MethId == m_c2MethId_Split)
    {
        TMEngFloat4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        TMEngFloat4Val& mecvInt
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd);

        tCIDLib::TFloat4 f4Int;
        mecvRet.f4Value(TMathLib::f4Split(mecvActual.f4Value(), f4Int));
        mecvInt.f4Value(f4Int);
    }
     else if (c2MethId == m_c2MethId_SqrRoot)
    {
        TMEngFloat4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        mecvRet.f4Value(TMathLib::f4SqrRoot(mecvActual.f4Value()));
    }
     else if (c2MethId == m_c2MethId_Sub)
    {
        TMEngFloat4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat4Val>(c4FirstInd - 1);
        mecvRet.f4Value(mecvActual.f4Value() - meOwner.f4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        mecvActual.f4Value(meOwner.f4StackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngFloat8Val
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngFloat8Val: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngFloat8Val::TMEngFloat8Val( const   TString&                strName
                                , const tCIDMacroEng::EConstTypes  eConst
                                , const tCIDLib::TFloat8        f8Init) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Float8), eConst)
    , m_f8Value(f8Init)
{
}

TMEngFloat8Val::~TMEngFloat8Val()
{
}


// ---------------------------------------------------------------------------
//  TMEngFloat8Val: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TFloat8 TMEngFloat8Val::f8Value() const
{
    return m_f8Value;
}

tCIDLib::TFloat8 TMEngFloat8Val::f8Value(const tCIDLib::TFloat8 f8ToSet)
{
    m_f8Value = f8ToSet;
    return m_f8Value;
}


// ---------------------------------------------------------------------------
//  TMEngFloat8Val: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngFloat8Val::bDbgFormat(         TTextOutStream&     strmTarget
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices
                            , const TCIDMacroEngine&    ) const
{
    strmTarget << TFloat(m_f8Value, 4);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TMEngFloat8Val::bParseFromText(const TString& strValue, const TMEngClassInfo&)
{
    tCIDLib::TBoolean bRes;
    tCIDLib::TFloat8 f8Val = TRawStr::f8AsBinary(strValue.pszBuffer(), bRes);
    if (bRes)
        m_f8Value = f8Val;
    return bRes;
}


tCIDLib::TVoid
TMEngFloat8Val::CopyFrom(const  TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    m_f8Value = static_cast<const TMEngFloat8Val&>(mecvToCopy).m_f8Value;
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngFloat8Info
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngFloat8Info: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngFloat8Info::strPath()
{
    return CIDMacroEng_FloatClasses::strFloat8ClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngFloat8Info: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngFloat8Info::TMEngFloat8Info(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_FloatClasses::strFloat8
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2MethId_Abs(kCIDMacroEng::c2BadId)
    , m_c2MethId_Add(kCIDMacroEng::c2BadId)
    , m_c2MethId_ArcCosine(kCIDMacroEng::c2BadId)
    , m_c2MethId_ArcSine(kCIDMacroEng::c2BadId)
    , m_c2MethId_ArcTangent(kCIDMacroEng::c2BadId)
    , m_c2MethId_Ceiling(kCIDMacroEng::c2BadId)
    , m_c2MethId_Cosine(kCIDMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_Div(kCIDMacroEng::c2BadId)
    , m_c2MethId_DivEq(kCIDMacroEng::c2BadId)
    , m_c2MethId_Equal(kCIDMacroEng::c2BadId)
    , m_c2MethId_Floor(kCIDMacroEng::c2BadId)
    , m_c2MethId_GtThan(kCIDMacroEng::c2BadId)
    , m_c2MethId_GtThanEq(kCIDMacroEng::c2BadId)
    , m_c2MethId_LsThan(kCIDMacroEng::c2BadId)
    , m_c2MethId_LsThanEq(kCIDMacroEng::c2BadId)
    , m_c2MethId_MaxVal(kCIDMacroEng::c2BadId)
    , m_c2MethId_MinVal(kCIDMacroEng::c2BadId)
    , m_c2MethId_MinusEq(kCIDMacroEng::c2BadId)
    , m_c2MethId_Mul(kCIDMacroEng::c2BadId)
    , m_c2MethId_MulEq(kCIDMacroEng::c2BadId)
    , m_c2MethId_Negate(kCIDMacroEng::c2BadId)
    , m_c2MethId_NLog(kCIDMacroEng::c2BadId)
    , m_c2MethId_PlusEq(kCIDMacroEng::c2BadId)
    , m_c2MethId_Power(kCIDMacroEng::c2BadId)
    , m_c2MethId_Round(kCIDMacroEng::c2BadId)
    , m_c2MethId_Sine(kCIDMacroEng::c2BadId)
    , m_c2MethId_SqrRoot(kCIDMacroEng::c2BadId)
    , m_c2MethId_Split(kCIDMacroEng::c2BadId)
    , m_c2MethId_Sub(kCIDMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kCIDMacroEng::c2BadId)
{
    // Add imports for any non-intrinsic classes we use in our signatures
    bAddClassImport(TMEngBaseInfoInfo::strPath());
}

TMEngFloat8Info::~TMEngFloat8Info()
{
}


// ---------------------------------------------------------------------------
//  TMEngFloat8Info: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngFloat8Info::bCanCastFrom(const tCIDLib::TCard2 c2SrcId)
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
TMEngFloat8Info::eCastFrom(         TCIDMacroEngine&
                            , const TMEngClassVal&      mecvSrc
                            ,       TMEngClassVal&      mecvTarget)
{
    // Get the source value
    const tCIDLib::TCard2 c2SrcId = mecvSrc.c2ClassId();

    tCIDLib::TFloat8 f8New = 0;
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
            if (static_cast<const TMEngBooleanVal&>(mecvSrc).bValue())
                f8New = 1;
            else
                f8New = 0;
            break;

        case tCIDMacroEng::EIntrinsics::Card1 :
            f8New = static_cast<const TMEngCard1Val&>(mecvSrc).c1Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card2 :
            f8New = static_cast<const TMEngCard2Val&>(mecvSrc).c2Value();
            break;

        case tCIDMacroEng::EIntrinsics::Card4 :
            f8New = tCIDLib::TFloat8(static_cast<const TMEngCard4Val&>(mecvSrc).c4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Card8 :
            f8New = tCIDLib::TFloat8(static_cast<const TMEngCard8Val&>(mecvSrc).c8Value());
            break;

        case tCIDMacroEng::EIntrinsics::Int1 :
            f8New = static_cast<const TMEngInt1Val&>(mecvSrc).i1Value();
            break;

        case tCIDMacroEng::EIntrinsics::Int2 :
            f8New = static_cast<const TMEngInt2Val&>(mecvSrc).i2Value();
            break;

        case tCIDMacroEng::EIntrinsics::Int4 :
            f8New = tCIDLib::TFloat8(static_cast<const TMEngInt4Val&>(mecvSrc).i4Value());
            break;

        case tCIDMacroEng::EIntrinsics::Float4 :
            f8New = static_cast<const TMEngFloat4Val&>(mecvSrc).f4Value();
            break;

        case tCIDMacroEng::EIntrinsics::Float8 :
            f8New = static_cast<const TMEngFloat8Val&>(mecvSrc).f8Value();
            break;

        default :
            return tCIDMacroEng::ECastRes::Incompatible;
    };

    // Cast the target to our type, which it must be
    TMEngFloat8Val& mecvActual = static_cast<TMEngFloat8Val&>(mecvTarget);
    mecvActual.f8Value(f8New);
    return tCIDMacroEng::ECastRes::Ok;
}


tCIDLib::TVoid TMEngFloat8Info::Init(TCIDMacroEngine& meOwner)
{
    // Add a literal for PI
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kPI"
            , tCIDMacroEng::EIntrinsics::Float8
            , new TMEngFloat8Val(L"kPI", tCIDMacroEng::EConstTypes::Const, kCIDLib::f8PI)
        )
    );

    // Add a literal for PI*2
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"k2PI"
            , tCIDMacroEng::EIntrinsics::Float8
            , new TMEngFloat8Val(L"k2PI", tCIDMacroEng::EConstTypes::Const, kCIDLib::f8PIx2)
        )
    );

    const tCIDLib::TCard2 c2RndTypes = meOwner.c2FindClassId
    (
        TMEngBaseInfoInfo::strUpDnClPath()
    );


    // Return the absolute value of this object
    {
        TMEngMethodInfo methiNew
        (
            L"Abs"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_Abs = c2AddMethodInfo(methiNew);
    }

    // Add the add method
    {
        TMEngMethodInfo methiNew
        (
            L"Add"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Float8);
        m_c2MethId_Add = c2AddMethodInfo(methiNew);
    }

    // Calculate the ArcCosine, ArcSine, and ArcTangent
    {
        TMEngMethodInfo methiNew
        (
            L"ArcCosine"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_ArcCosine = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ArcSine"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_ArcSine = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"ArcTangent"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_ArcTangent = c2AddMethodInfo(methiNew);
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.Float8"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Do a standard ceiling operation
    {
        TMEngMethodInfo methiNew
        (
            L"Ceiling"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_Ceiling = c2AddMethodInfo(methiNew);
    }

    // Return the cosine of this value
    {
        TMEngMethodInfo methiNew
        (
            L"Cosine"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_Cosine = c2AddMethodInfo(methiNew);
    }

    // Add the div and diveq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Div"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"DivBy", tCIDMacroEng::EIntrinsics::Float8);
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
        methiNew.c2AddInParm(L"DivBy", tCIDMacroEng::EIntrinsics::Float8);
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
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Float8);
        m_c2MethId_Equal = c2AddMethodInfo(methiNew);
    }

    // Do a standard floor operation
    {
        TMEngMethodInfo methiNew
        (
            L"Floor"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_Floor = c2AddMethodInfo(methiNew);
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
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Float8);
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
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Float8);
        m_c2MethId_GtThanEq = c2AddMethodInfo(methiNew);
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
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Float8);
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
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Float8);
        m_c2MethId_LsThanEq = c2AddMethodInfo(methiNew);
    }

    // Add the max/min value methods
    {
        TMEngMethodInfo methiNew
        (
            L"MaxVal"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Float8);
        m_c2MethId_MaxVal = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"MinVal"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToCompare", tCIDMacroEng::EIntrinsics::Float8);
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
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Float8);
        m_c2MethId_MinusEq = c2AddMethodInfo(methiNew);
    }

    // Add the mul and muleq methods
    {
        TMEngMethodInfo methiNew
        (
            L"Mul"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"MulBy", tCIDMacroEng::EIntrinsics::Float8);
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
        methiNew.c2AddInParm(L"MulBy", tCIDMacroEng::EIntrinsics::Float8);
        m_c2MethId_MulEq = c2AddMethodInfo(methiNew);
    }

    // Negate our value
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

    // Return the natural log of this value
    {
        TMEngMethodInfo methiNew
        (
            L"NLog"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_NLog = c2AddMethodInfo(methiNew);
    }

    // Add the pluseq method
    {
        TMEngMethodInfo methiNew
        (
            L"PlusEq"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToAdd", tCIDMacroEng::EIntrinsics::Float8);
        m_c2MethId_PlusEq = c2AddMethodInfo(methiNew);
    }


    // Raise this value to the indicated power
    {
        TMEngMethodInfo methiNew
        (
            L"Power"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Power", tCIDMacroEng::EIntrinsics::Float8);
        m_c2MethId_Power = c2AddMethodInfo(methiNew);
    }

    // Round this value
    {
        TMEngMethodInfo methiNew
        (
            L"Round"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"RoundType", c2RndTypes);
        m_c2MethId_Round = c2AddMethodInfo(methiNew);
    }

    // Return the sine of this value
    {
        TMEngMethodInfo methiNew
        (
            L"Sine"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_Sine = c2AddMethodInfo(methiNew);
    }

    // Split out the fractional and integral parts
    {
        TMEngMethodInfo methiNew
        (
            L"Split"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"Integral", tCIDMacroEng::EIntrinsics::Float8);
        m_c2MethId_Split = c2AddMethodInfo(methiNew);
    }

    // Return the square root of this value
    {
        TMEngMethodInfo methiNew
        (
            L"SqrRoot"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_SqrRoot = c2AddMethodInfo(methiNew);
    }

    // Sub a value from this value and return the result
    {
        TMEngMethodInfo methiNew
        (
            L"Sub"
            , tCIDMacroEng::EIntrinsics::Float8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToSub", tCIDMacroEng::EIntrinsics::Float8);
        m_c2MethId_Sub = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that takes an initial value
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.Float8"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"InitVal", tCIDMacroEng::EIntrinsics::Float8);
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_ValCtor = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngFloat8Info::pmecvMakeStorage(  const   TString&                strName
                                    ,       TCIDMacroEngine&
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngFloat8Val(strName, eConst);
}


// ---------------------------------------------------------------------------
//  TMEngFloat8Info: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngFloat8Info::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngFloat8Val& mecvActual = static_cast<TMEngFloat8Val&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_Abs)
    {
        TMEngFloat8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        mecvRet.f8Value(TMathLib::f8Abs(mecvActual.f8Value()));
    }
     else if (c2MethId == m_c2MethId_Add)
    {
        TMEngFloat8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        mecvRet.f8Value(mecvActual.f8Value() + meOwner.f8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ArcCosine)
    {
        TMEngFloat8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        mecvRet.f8Value(TMathLib::f8ArcCosine(mecvActual.f8Value()));
    }
     else if (c2MethId == m_c2MethId_ArcSine)
    {
        TMEngFloat8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        mecvRet.f8Value(TMathLib::f8ArcSine(mecvActual.f8Value()));
    }
     else if (c2MethId == m_c2MethId_ArcTangent)
    {
        TMEngFloat8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        mecvRet.f8Value(TMathLib::f8ArcTangent(mecvActual.f8Value()));
    }
     else if (c2MethId == m_c2MethId_Ceiling)
    {
        TMEngFloat8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        mecvRet.f8Value(TMathLib::f8Ceil(mecvActual.f8Value()));
    }
     else if (c2MethId == m_c2MethId_Cosine)
    {
        TMEngFloat8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        mecvRet.f8Value(TMathLib::f8Cosine(mecvActual.f8Value()));
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Default ctor just zeros the value
        mecvActual.f8Value(0);
    }
     else if (c2MethId == m_c2MethId_Div)
    {
        TMEngFloat8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        mecvRet.f8Value(mecvActual.f8Value() / meOwner.f8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_DivEq)
    {
        mecvActual.f8Value(mecvActual.f8Value() / meOwner.f8StackValAt(c4FirstInd));
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner) << mecvActual.f8Value();
    }
     else if (c2MethId == m_c2MethId_Floor)
    {
        TMEngFloat8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        mecvRet.f8Value(TMathLib::f8Floor(mecvActual.f8Value()));
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
        const tCIDLib::TFloat8 f8Parm = meOwner.f8StackValAt(c4FirstInd);

        if (c2MethId == m_c2MethId_GtThan)
            mecvRet.bValue(mecvActual.f8Value() > f8Parm);
        else if (c2MethId == m_c2MethId_GtThanEq)
            mecvRet.bValue(mecvActual.f8Value() >= f8Parm);
        else if (c2MethId == m_c2MethId_LsThan)
            mecvRet.bValue(mecvActual.f8Value() < f8Parm);
        else if (c2MethId == m_c2MethId_LsThanEq)
            mecvRet.bValue(mecvActual.f8Value() <= f8Parm);
        else
            mecvRet.bValue(mecvActual.f8Value() == f8Parm);
    }
     else if (c2MethId == m_c2MethId_MaxVal)
    {
        const tCIDLib::TFloat8 f8Comp = meOwner.f8StackValAt(c4FirstInd);
        TMEngFloat8Val& mecvRet = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        if (f8Comp > mecvActual.f8Value())
            mecvRet.f8Value(f8Comp);
        else
            mecvRet.f8Value(mecvActual.f8Value());
    }
     else if (c2MethId == m_c2MethId_MinVal)
    {
        const tCIDLib::TFloat8 f8Comp = meOwner.f8StackValAt(c4FirstInd);
        TMEngFloat8Val& mecvRet = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        if (f8Comp < mecvActual.f8Value())
            mecvRet.f8Value(f8Comp);
        else
            mecvRet.f8Value(mecvActual.f8Value());
    }
     else if (c2MethId == m_c2MethId_MinusEq)
    {
        mecvActual.f8Value(mecvActual.f8Value() - meOwner.f8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Mul)
    {
        TMEngFloat8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        mecvRet.f8Value(mecvActual.f8Value() * meOwner.f8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_MulEq)
    {
        mecvActual.f8Value(mecvActual.f8Value() * meOwner.f8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Negate)
    {
        mecvActual.f8Value(mecvActual.f8Value() * -1);
    }
     else if (c2MethId == m_c2MethId_NLog)
    {
        TMEngFloat8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        mecvRet.f8Value(TMathLib::f8Log(mecvActual.f8Value()));
    }
     else if (c2MethId == m_c2MethId_PlusEq)
    {
        mecvActual.f8Value(mecvActual.f8Value() + meOwner.f8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Power)
    {
        mecvActual.f8Value
        (
            TMathLib::f8Power(mecvActual.f8Value(), meOwner.f8StackValAt(c4FirstInd))
        );
    }
     else if (c2MethId == m_c2MethId_Round)
    {
        TMEngEnumVal& mecvType = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        tCIDLib::TFloat8 f8Val = mecvActual.f8Value();
        const TMEngEnumInfo& meciType = meOwner.meciFindAs<TMEngEnumInfo>
        (
            mecvType.c2ClassId()
        );
        TMathLib::Round(f8Val, tCIDLib::ERoundTypes(meciType.c4MapValue(mecvType)));
        mecvActual.f8Value(f8Val);
    }
     else if (c2MethId == m_c2MethId_Sine)
    {
        TMEngFloat8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        mecvRet.f8Value(TMathLib::f8Sine(mecvActual.f8Value()));
    }
     else if (c2MethId == m_c2MethId_Split)
    {
        TMEngFloat8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        TMEngFloat8Val& mecvInt
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd);

        tCIDLib::TFloat8 f8Int;
        mecvRet.f8Value(TMathLib::f8Split(mecvActual.f8Value(), f8Int));
        mecvInt.f8Value(f8Int);
    }

     else if (c2MethId == m_c2MethId_SqrRoot)
    {
        TMEngFloat8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        mecvRet.f8Value(TMathLib::f8SqrRoot(mecvActual.f8Value()));
    }
     else if (c2MethId == m_c2MethId_Sub)
    {
        TMEngFloat8Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngFloat8Val>(c4FirstInd - 1);
        mecvRet.f8Value(mecvActual.f8Value() - meOwner.f8StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        mecvActual.f8Value(meOwner.f8StackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


