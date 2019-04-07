//
// FILE NAME: CIDMacroEng_KVPairClass.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/18/2004
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
//  This file implements value/info classes to provide a macro level class
//  that wraps the CIDLib speech features.
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
RTTIDecls(TMEngSpeechVal,TMEngClassVal)
RTTIDecls(TMEngSpeechInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_SpeechClass
{
    // -----------------------------------------------------------------------
    //  The names for the types that we support here. Each derivative has to
    //  be able to return strings that contain its name and full name.
    // -----------------------------------------------------------------------
    const TString   strSpeech(L"Speech");
    const TString   strSpeechClassPath(L"MEng.System.Runtime.Speech");
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngSpeechVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngSpeechVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngSpeechVal::TMEngSpeechVal( const   TString&                strName
                                , const tCIDLib::TCard2         c2Id
                                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
{
}

TMEngSpeechVal::~TMEngSpeechVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngSpeechVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngSpeechVal::bWaitTillDone(const tCIDLib::TCard4 c4ToWait)
{
    return m_spchImpl.bWaitTillDone(c4ToWait);
}


tCIDLib::TVoid TMEngSpeechVal::SetVoice(const TString& strToSet)
{
    m_spchImpl.SetVoice(strToSet);
}


tCIDLib::TVoid
TMEngSpeechVal::Speak(  const   TString&                strToSay
                        , const tCIDLib::ESpeechFlags   eFlags)
{
    m_spchImpl.Speak(strToSay, eFlags);
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngSpeechInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngSpeechInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngSpeechInfo::strPath()
{
    return CIDMacroEng_SpeechClass::strSpeechClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngSpeechInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngSpeechInfo::TMEngSpeechInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_SpeechClass::strSpeech
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2TypeId_Errors(kMacroEng::c2BadId)
    , m_c2TypeId_Flags(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_SetVoice(kMacroEng::c2BadId)
    , m_c2MethId_Speak(kMacroEng::c2BadId)
    , m_c2MethId_WaitTillDone(kMacroEng::c2BadId)
    , m_c4ErrId_SetVoice(kCIDLib::c4MaxCard)
    , m_c4ErrId_Speak(kCIDLib::c4MaxCard)
    , m_c4ErrId_WaitTillDone(kCIDLib::c4MaxCard)
    , m_pmeciErrors(0)
    , m_pmeciFlags(0)
{
}

TMEngSpeechInfo::~TMEngSpeechInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngSpeechInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngSpeechInfo::Init(TCIDMacroEngine& meOwner)
{
    // Create an enum for the flags to the Speak method
    {
        m_pmeciFlags = new TMEngEnumInfo
        (
            meOwner
            , L"SpeechFlags"
            , strClassPath()
            , L"MEng.Enum"
            , 4
        );
        m_pmeciFlags->c4AddEnumItem(L"Default", L"Async", tCIDLib::ESpeechFlags::None);
        m_pmeciFlags->c4AddEnumItem(L"Async", L"Async", tCIDLib::ESpeechFlags::Async);
        m_pmeciFlags->c4AddEnumItem(L"Markup", L"Markup", tCIDLib::ESpeechFlags::Markup);
        m_pmeciFlags->c4AddEnumItem(L"AsyncMarkup", L"AsyncMarkup", tCIDLib::ESpeechFlags::AsyncMarkup);
        m_pmeciFlags->BaseClassInit(meOwner);
        m_c2TypeId_Flags = meOwner.c2AddClass(m_pmeciFlags);
        bAddNestedType(m_pmeciFlags->strClassPath());
    }

    // Create an enum for the errors we throw
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"SpeechErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 3
        );
        m_c4ErrId_SetVoice = m_pmeciErrors->c4AddEnumItem(L"SetVoice", kCIDLib::pszEmptyZStr);
        m_c4ErrId_Speak = m_pmeciErrors->c4AddEnumItem(L"Speak", kCIDLib::pszEmptyZStr);
        m_c4ErrId_WaitTillDone = m_pmeciErrors->c4AddEnumItem(L"WaitTillDone", kCIDLib::pszEmptyZStr);
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2TypeId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.Speech"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Set the voice
    {
        TMEngMethodInfo methiNew
        (
            L"SetVoice"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"SetVoice", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_SetVoice = c2AddMethodInfo(methiNew);
    }

    // Speak a line of text
    {
        TMEngMethodInfo methiNew
        (
            L"Speak"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSay", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Flags", m_c2TypeId_Flags);
        m_c2MethId_Speak = c2AddMethodInfo(methiNew);
    }

    // Wait for any outstanding events to end
    {
        TMEngMethodInfo methiNew
        (
            L"WaitTillDone"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_WaitTillDone = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngSpeechInfo::pmecvMakeStorage(  const   TString&                strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngSpeechVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngSpeechInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngSpeechInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngSpeechVal& mecvActual = static_cast<TMEngSpeechVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
    }
     else if (c2MethId == m_c2MethId_SetVoice)
    {
        try
        {
            mecvActual.SetVoice(meOwner.strStackValAt(c4FirstInd));
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_SetVoice, errToCatch.strErrText());
        }
    }
     else if (c2MethId == m_c2MethId_Speak)
    {
        // Get the string and flags enum
        const TString& strToSay = meOwner.strStackValAt(c4FirstInd);
        TMEngEnumVal& mecvFlags = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1);

        try
        {
            mecvActual.Speak
            (
                strToSay
                , tCIDLib::ESpeechFlags(m_pmeciFlags->c4MapValue(mecvFlags))
            );
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Speak, errToCatch.strErrText());
        }
    }
     else if (c2MethId == m_c2MethId_WaitTillDone)
    {
        try
        {
            const tCIDLib::TBoolean bRes = mecvActual.bWaitTillDone
            (
                meOwner.c4StackValAt(c4FirstInd)
            );

            TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
            mecvRet.bValue(bRes);
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_WaitTillDone, errToCatch.strErrText());
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngSpeechInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngSpeechInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            , const TString&            strText)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

