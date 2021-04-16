//
// FILE NAME: CIDMacroEng_Audio.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/16/2003
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
//  This file implements the two class derivatives required to create a new
//  macro engine class that provides basic audio support.
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
RTTIDecls(TMEngAudioInfo,TMacroEngClass)
RTTIDecls(TMEngAudioVal,TMEngClassVal)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_AudioClass
{
    namespace
    {
        const TString   strName(L"Audio");
        const TString   strClassPath(L"MEng.System.Runtime.Audio");
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngAudioVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngAudioVal: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngAudioVal::TMEngAudioVal(const  TString&                strName
                            , const tCIDLib::TCard2         c2Id
                            , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
{
}

TMEngAudioVal::~TMEngAudioVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngAudioVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngAudioVal::bDbgFormat(          TTextOutStream&     strmTarget
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices
                            , const TCIDMacroEngine&    ) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        strmTarget << L"Info = "  << TAudio::strDevInfo();
        return kCIDLib::True;
    }
    return kCIDLib::False;
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngAudioInfo
// PREFIX: meunit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngAudioInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngAudioInfo::strPath()
{
    return CIDMacroEng_AudioClass::strClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngAudioInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngAudioInfo::TMEngAudioInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_AudioClass::strName
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_Beep(kCIDMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_PlayWAV(kCIDMacroEng::c2BadId)
{
}

TMEngAudioInfo::~TMEngAudioInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngAudioInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngAudioInfo::Init(TCIDMacroEngine&)
{
    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.Audio"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the Beep method, which has two Card4 parameters
    {
        TMEngMethodInfo methiNew
        (
            L"Beep"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"Freq", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Millis", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_Beep = c2AddMethodInfo(methiNew);
    }

    // Add the PlayWAV method, which has one string parameter
    {
        TMEngMethodInfo methiNew
        (
            L"PlayWAV"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"FilePath", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_PlayWAV = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngAudioInfo::pmecvMakeStorage(const  TString&                strName
                                ,       TCIDMacroEngine&
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    TMEngAudioVal* pmecvNew = new TMEngAudioVal(strName, c2Id(), eConst);
    TJanitor<TMEngAudioVal> janNew(pmecvNew);
    return janNew.pobjOrphan();
}


// ---------------------------------------------------------------------------
//  TMEngAudioInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngAudioInfo::bInvokeMethod(          TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    //
    //  In our case, we have all external 'methods', so we just handle them
    //  all either right here, or call a private to do it.
    //
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    if (methiTarget.c2Id() == m_c2MethId_Beep)
    {
        // Get the two card4 parms off of the stack
        const tCIDLib::TCard4 c4Freq = meOwner.c4StackValAt(c4FirstInd);
        const tCIDLib::TCard4 c4Millis = meOwner.c4StackValAt(c4FirstInd + 1);
        TAudio::Beep(c4Freq, c4Millis);
    }
     else if (methiTarget.c2Id() == m_c2MethId_DefCtor)
    {
        // Nothing to do in this case
    }
     else if (methiTarget.c2Id() == m_c2MethId_PlayWAV)
    {
        // Get the value item that represents the parameter
//        const TMEngClassVal& mecvFile = meOwner.mecvStackAt(c4FirstInd);

        // <TBD> Finish this
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



