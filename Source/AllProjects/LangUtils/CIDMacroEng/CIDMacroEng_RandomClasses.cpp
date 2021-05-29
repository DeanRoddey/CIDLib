//
// FILE NAME: CIDMacroEng_RandomClass.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/02/2007
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
//  This file implements the value and info classes required to create a
//  random number generator CML class.
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
RTTIDecls(TMEngRandomNumVal,TMEngClassVal)
RTTIDecls(TMEngRandomNumInfo,TMacroEngClass)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDMacroEng_RandomClass
    {
        // -----------------------------------------------------------------------
        //  The names of our classes and their paths.
        // -----------------------------------------------------------------------
        const TString   strName(L"RandomNum");
        const TString   strPath(L"MEng.System.Runtime.RandomNum");
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngRandomNumVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngRandomNumVal: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngRandomNumVal::TMEngRandomNumVal(const  TString&                    strName
                                    , const tCIDLib::TCard2             c2Id
                                    , const tCIDMacroEng::EConstTypes   eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_prandValue(new TRandomNum)
{
}

TMEngRandomNumVal::~TMEngRandomNumVal()
{
    delete m_prandValue;
}


// ---------------------------------------------------------------------------
//  TMEngRandomNumVal: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TCard4 TMEngRandomNumVal::c4NextVal(const tCIDLib::TCard4 c4Modulus)
{
    return m_prandValue->c4GetNextNum() % c4Modulus;
}

tCIDLib::TVoid TMEngRandomNumVal::Seed(const tCIDLib::TCard4 c4Seed)
{
    m_prandValue->Seed(c4Seed);
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngRandomNumInfo
// PREFIX: meunit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngRandomNumInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngRandomNumInfo::strPath()
{
    return CIDMacroEng_RandomClass::strPath;
}


// ---------------------------------------------------------------------------
//  TMEngRandomNumInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngRandomNumInfo::TMEngRandomNumInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_RandomClass::strName
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetNextVal(kCIDMacroEng::c2BadId)
    , m_c2MethId_Seed(kCIDMacroEng::c2BadId)
{
}

TMEngRandomNumInfo::~TMEngRandomNumInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngRandomNumInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngRandomNumInfo::Init(TCIDMacroEngine& meOwner)
{
    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.RandomNum"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Get the next random number
    {
        TMEngMethodInfo methiNew
        (
            L"GetNextVal"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Modulus", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetNextVal = c2AddMethodInfo(methiNew);
    }

    // Seed the random number generator
    {
        TMEngMethodInfo methiNew
        (
            L"Seed"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"SeedValue", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_Seed = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngRandomNumInfo::pmecvMakeStorage(const  TString&                strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngRandomNumVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngRandomNumInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngRandomNumInfo::bInvokeMethod(        TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngRandomNumVal& mecvActual = static_cast<TMEngRandomNumVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        // Nothing to do
    }
     else if (c2MethId == m_c2MethId_GetNextVal)
    {
        TMEngCard4Val& mecvRet
                = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.c4NextVal(meOwner.c4StackValAt(c4FirstInd)));
    }
     else if (c2MethId == m_c2MethId_Seed)
    {
        mecvActual.Seed(meOwner.c4StackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



