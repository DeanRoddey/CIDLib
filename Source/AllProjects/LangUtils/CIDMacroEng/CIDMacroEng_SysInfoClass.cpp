//
// FILE NAME: CIDMacroEng_SysInfoClass.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/20/2003
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
//  This file implements the macro level MEng.System.Runtime.SysInfo class.
//  It just provides some global information of interest, and defines a
//  commonly used error enum.
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
RTTIDecls(TMEngSysInfoInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_SysInfoClass
{
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngSysInfoInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngSysInfoInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngSysInfoInfo::TMEngSysInfoInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        L"SysInfo"
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetCPUCount(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetNodeName(kCIDMacroEng::c2BadId)
{
}

TMEngSysInfoInfo::~TMEngSysInfoInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngSysInfoInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngSysInfoInfo::Init(TCIDMacroEngine&)
{
    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.SysInfo"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Get the local node name
    {
        TMEngMethodInfo methiNew
        (
            L"GetNodeName"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetNodeName = c2AddMethodInfo(methiNew);
    }

    // Get the number of CPUs
    {
        TMEngMethodInfo methiNew
        (
            L"GetCPUCount"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetCPUCount = c2AddMethodInfo(methiNew);
    }

}


TMEngClassVal*
TMEngSysInfoInfo::pmecvMakeStorage( const   TString&                strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngStdClassVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMacroEngSysInfoInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngSysInfoInfo::bInvokeMethod(        TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        // Nothing to do
    }
     else if (c2MethId == m_c2MethId_GetCPUCount)
    {
        // Get the return value and set it with the indicated char
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(TSysInfo::c4CPUCount());
    }
     else if (c2MethId == m_c2MethId_GetNodeName)
    {
        // Get the return value and set it with the indicated char
        TMEngStringVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        mecvRet.strValue(TSysInfo::strIPHostName());
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}

