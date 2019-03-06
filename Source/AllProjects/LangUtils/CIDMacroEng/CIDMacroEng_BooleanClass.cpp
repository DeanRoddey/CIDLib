//
// FILE NAME: CIDMacroEng_BooleanClasse.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a set of derivatives of the class info and class
//  value classes, which implement a boolean class.
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
RTTIDecls(TMEngBooleanVal,TMEngClassVal)
RTTIDecls(TMEngBooleanInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_BooleanClass
{
    // -----------------------------------------------------------------------
    //  The names for the types that we support here. Each derivative has to
    //  be able to return strings that contain its name and full name.
    // -----------------------------------------------------------------------
    const TString   strBoolean(L"Boolean");
    const TString   strBooleanClassPath(L"MEng.Boolean");
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngBooleanVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngBooleanVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngBooleanVal::TMEngBooleanVal(const  TString&                strName
                                , const tCIDMacroEng::EConstTypes  eConst
                                , const tCIDLib::TBoolean       bInit) :

    TMEngClassVal(strName, tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Boolean), eConst)
    , m_bValue(bInit)
{
}

TMEngBooleanVal::~TMEngBooleanVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngBooleanVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngBooleanVal::bDbgFormat(        TTextOutStream&     strmTarget
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices
                            , const TCIDMacroEngine&    ) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Short)
    {
        strmTarget << m_bValue;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
TMEngBooleanVal::bParseFromText( const  TString&        strValue
                                , const TMEngClassInfo& )
{
    tCIDLib::TBoolean bRes = kCIDLib::True;
    if ((strValue.bCompareI(L"True"))
    ||  (strValue == L"1"))
    {
        m_bValue = kCIDLib::True;
    }
     else if ((strValue.bCompareI(L"False"))
          ||  (strValue == L"0"))
    {
        m_bValue = kCIDLib::False;
    }
     else
    {
        bRes = kCIDLib::False;
    }
    return bRes;
}


tCIDLib::TVoid
TMEngBooleanVal::CopyFrom(const TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    m_bValue = static_cast<const TMEngBooleanVal&>(mecvToCopy).m_bValue;
}



// ---------------------------------------------------------------------------
//  TMEngBooleanVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngBooleanVal::And(const tCIDLib::TBoolean bAndWith)
{
    m_bValue &= bAndWith;
}


tCIDLib::TBoolean TMEngBooleanVal::bValue() const
{
    return m_bValue;
}

tCIDLib::TBoolean TMEngBooleanVal::bValue(const tCIDLib::TBoolean bToSet)
{
    m_bValue = bToSet;
    return m_bValue;
}


tCIDLib::TVoid TMEngBooleanVal::Negate()
{
    m_bValue = !m_bValue;
}


tCIDLib::TVoid TMEngBooleanVal::Or(const tCIDLib::TBoolean bOrWith)
{
    m_bValue |= bOrWith;
}


tCIDLib::TVoid TMEngBooleanVal::Xor(const tCIDLib::TBoolean bXorWith)
{
    m_bValue &= bXorWith;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngBooleanInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngBooleanInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngBooleanInfo::strPath()
{
    return CIDMacroEng_BooleanClass::strBooleanClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngBooleanInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngBooleanInfo::TMEngBooleanInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_BooleanClass::strBoolean
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2MethId_And(kMacroEng::c2BadId)
    , m_c2MethId_AndEq(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Equal(kMacroEng::c2BadId)
    , m_c2MethId_Negate(kMacroEng::c2BadId)
    , m_c2MethId_Or(kMacroEng::c2BadId)
    , m_c2MethId_OrEq(kMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kMacroEng::c2BadId)
    , m_c2MethId_Xor(kMacroEng::c2BadId)
    , m_c2MethId_XorEq(kMacroEng::c2BadId)
{
}

TMEngBooleanInfo::~TMEngBooleanInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngBooleanInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDMacroEng::ECastRes
TMEngBooleanInfo::eCastFrom(        TCIDMacroEngine&    meOwner
                            , const TMEngClassVal&      mecvSrc
                            ,       TMEngClassVal&      mecvTarget)
{
    // Get the source value
    const tCIDLib::TCard2 c2SrcId = mecvSrc.c2Id();

    tCIDLib::TBoolean bNew;
    switch(tCIDMacroEng::EIntrinsics(c2SrcId))
    {
        case tCIDMacroEng::EIntrinsics::Boolean :
            bNew = static_cast<const TMEngBooleanVal&>(mecvSrc).bValue();
            break;

        case tCIDMacroEng::EIntrinsics::Card1 :
            if (static_cast<const TMEngCard1Val&>(mecvSrc).c1Value())
                bNew = kCIDLib::True;
            else
                bNew = kCIDLib::False;
            break;


        case tCIDMacroEng::EIntrinsics::Card2 :
            if (static_cast<const TMEngCard2Val&>(mecvSrc).c2Value())
                bNew = kCIDLib::True;
            else
                bNew = kCIDLib::False;
            break;


        case tCIDMacroEng::EIntrinsics::Card4 :
            if (static_cast<const TMEngCard4Val&>(mecvSrc).c4Value())
                bNew = kCIDLib::True;
            else
                bNew = kCIDLib::False;
            break;


        case tCIDMacroEng::EIntrinsics::Int1 :
            if (static_cast<const TMEngInt1Val&>(mecvSrc).i1Value())
                bNew = kCIDLib::True;
            else
                bNew = kCIDLib::False;
            break;


        case tCIDMacroEng::EIntrinsics::Int2 :
            if (static_cast<const TMEngInt2Val&>(mecvSrc).i2Value())
                bNew = kCIDLib::True;
            else
                bNew = kCIDLib::False;
            break;


        case tCIDMacroEng::EIntrinsics::Int4 :
            if (static_cast<const TMEngInt4Val&>(mecvSrc).i4Value())
                bNew = kCIDLib::True;
            else
                bNew = kCIDLib::False;
            break;


        case tCIDMacroEng::EIntrinsics::Float4 :
            if (static_cast<const TMEngFloat4Val&>(mecvSrc).f4Value())
                bNew = kCIDLib::True;
            else
                bNew = kCIDLib::False;
            break;


        case tCIDMacroEng::EIntrinsics::Float8 :
            if (static_cast<const TMEngFloat8Val&>(mecvSrc).f8Value())
                bNew = kCIDLib::True;
            else
                bNew = kCIDLib::False;
            break;

        default :
            return tCIDMacroEng::ECastRes::Incompatible;
    };

    // Cast the target to our type, which it must be
    TMEngBooleanVal& mecvActual = static_cast<TMEngBooleanVal&>(mecvTarget);
    mecvActual.bValue(bNew);

    return tCIDMacroEng::ECastRes::Ok;
}


tCIDLib::TVoid TMEngBooleanInfo::Init(TCIDMacroEngine&)
{
    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.Boolean"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the constructor that takes an initial value
    {
        TMEngMethodInfo methiValCtor
        (
            L"ctor2_MEng.Boolean"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiValCtor.c2AddInParm(L"InitVal", tCIDMacroEng::EIntrinsics::Boolean);
        methiValCtor.bIsCtor(kCIDLib::True);
        m_c2MethId_ValCtor = c2AddMethodInfo(methiValCtor);
    }

    // And with parameter, one which returns and one which sets
    {
        TMEngMethodInfo methiNew
        (
            L"And"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"AndWith", tCIDMacroEng::EIntrinsics::Boolean);
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
        methiNew.c2AddInParm(L"AndWith", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_AndEq = c2AddMethodInfo(methiNew);
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
        methiNew.c2AddInParm(L"Val", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_Equal = c2AddMethodInfo(methiNew);
    }

    // Add the Negate() method
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

    // Or with parameter
    {
        TMEngMethodInfo methiNew
        (
            L"Or"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"OrWith", tCIDMacroEng::EIntrinsics::Boolean);
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
        methiNew.c2AddInParm(L"OrWith", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_OrEq = c2AddMethodInfo(methiNew);
    }

    // Xor with parameter
    {
        TMEngMethodInfo methiNew
        (
            L"Xor"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"XorWith", tCIDMacroEng::EIntrinsics::Boolean);
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
        methiNew.c2AddInParm(L"XorWith", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_XorEq = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngBooleanInfo::pmecvMakeStorage( const   TString&                strName
                                    ,       TCIDMacroEngine&
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngBooleanVal(strName, eConst);
}


// ---------------------------------------------------------------------------
//  TMacroBooleanInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngBooleanInfo::bInvokeMethod(        TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngBooleanVal& mecvActual = static_cast<TMEngBooleanVal&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_And)
    {
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(meOwner.bStackValAt(c4FirstInd) && mecvActual.bValue());
    }
     else if (c2MethId == m_c2MethId_AndEq)
    {
        mecvActual.And(meOwner.bStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Default ctor just resets the value
        mecvActual.bValue(kCIDLib::False);
    }
     else if (c2MethId == m_c2MethId_Equal)
    {
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.bValue() == meOwner.bStackValAt(c4FirstInd));
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner) << mecvActual.bValue();
    }
     else if (c2MethId == m_c2MethId_Negate)
    {
        // Just flip our own value
        mecvActual.Negate();
    }
     else if (c2MethId == m_c2MethId_Or)
    {
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(meOwner.bStackValAt(c4FirstInd) || mecvActual.bValue());
    }
     else if (c2MethId == m_c2MethId_OrEq)
    {
        mecvActual.Or(meOwner.bStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        mecvActual.bValue(meOwner.bStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Xor)
    {
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(meOwner.bStackValAt(c4FirstInd) ^ mecvActual.bValue());
    }
     else if (c2MethId == m_c2MethId_XorEq)
    {
        mecvActual.Xor(meOwner.bStackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



