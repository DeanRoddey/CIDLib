//
// FILE NAME: CIDMacroEng_BaseInfoClass.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/24/2004
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
//  This file implements a class that just defines some miscellaneous types
//  and literals.
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
RTTIDecls(TMEngBaseInfoInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_BaseInfoClass
{
    namespace
    {
        // -----------------------------------------------------------------------
        //  The names for the types that we support here. Each derivative has to
        //  be able to return strings that contain its name and full name.
        // -----------------------------------------------------------------------
        const TString   strBaseInfo(L"BaseInfo");
        const TString   strBaseInfoClassPath(L"MEng.BaseInfo");
        const TString   strCreateActsPath(L"MEng.BaseInfo.CreateActs");
        const TString   strFindResPath(L"MEng.BaseInfo.FindRes");
        const TString   strFQTypesPath(L"MEng.BaseInfo.FQTypes");
        const TString   strHJustifyClassPath(L"MEng.BaseInfo.HorzJustify");
        const TString   strRadixClassPath(L"MEng.BaseInfo.Radices");
        const TString   strEngErrorClassPath(L"MEng.BaseInfo.MEngErrors");
        const TString   strUpDnClClassPath(L"MEng.BaseInfo.UpDnClosest");
    }
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngBaseInfoInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngBaseInfoInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngBaseInfoInfo::strCreateActsPath()
{
    return CIDMacroEng_BaseInfoClass::strCreateActsPath;
}

const TString& TMEngBaseInfoInfo::strEngineErrPath()
{
    return CIDMacroEng_BaseInfoClass::strEngErrorClassPath;
}

const TString& TMEngBaseInfoInfo::strFindResPath()
{
    return CIDMacroEng_BaseInfoClass::strFindResPath;
}

const TString& TMEngBaseInfoInfo::strFQTypesPath()
{
    return CIDMacroEng_BaseInfoClass::strFQTypesPath;
}

const TString& TMEngBaseInfoInfo::strHorzJustifyPath()
{
    return CIDMacroEng_BaseInfoClass::strHJustifyClassPath;
}

const TString& TMEngBaseInfoInfo::strPath()
{
    return CIDMacroEng_BaseInfoClass::strBaseInfoClassPath;
}

const TString& TMEngBaseInfoInfo::strRadixPath()
{
    return CIDMacroEng_BaseInfoClass::strRadixClassPath;
}

const TString& TMEngBaseInfoInfo::strUpDnClPath()
{
return CIDMacroEng_BaseInfoClass::strUpDnClClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngBaseInfoInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngBaseInfoInfo::TMEngBaseInfoInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_BaseInfoClass::strBaseInfo
        , TFacCIDMacroEng::strRootClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )

    // These are made public, so we don't use the m_ prefix
    , c4ErrIdBadEntryParm(0)
    , c4ErrIdNotOpened(0)

    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_IsInDebugMode(kCIDMacroEng::c2BadId)
    , m_pmeciCreateActs(0)
    , m_pmeciFindRes(0)
    , m_pmeciFQTypes(0)
    , m_pmeciJustify(0)
    , m_pmeciMEngErrors(0)
    , m_pmeciRadices(0)
    , m_pmeciUpDnCl(0)
{
}

TMEngBaseInfoInfo::~TMEngBaseInfoInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngBaseInfoInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngBaseInfoInfo::Init(TCIDMacroEngine& meOwner)
{
    //
    //  This class exists to just hold some core types and literals that don't
    //  belong necessarily to other classes, because they are used system
    //  wide.
    //

    //
    //  Add an enum that defines a commonly used 'create action' enum that
    //  is used in places where somethign is opened or created that might
    //  already exist.
    //
    {
        m_pmeciCreateActs = new TMEngEnumInfo
        (
            meOwner, L"CreateActs", strClassPath(), L"MEng.Enum", 5
        );
        m_pmeciCreateActs->c4AddEnumItem(L"CreateIfNew", L"Create If New", tCIDLib::ECreateActs::CreateIfNew);
        m_pmeciCreateActs->c4AddEnumItem(L"CreateAlways", L"Create Always", tCIDLib::ECreateActs::CreateAlways);
        m_pmeciCreateActs->c4AddEnumItem(L"OpenIfExists", L"Open If Exists", tCIDLib::ECreateActs::OpenIfExists);
        m_pmeciCreateActs->c4AddEnumItem(L"OpenOrCreate", L"Open or Create", tCIDLib::ECreateActs::OpenOrCreate);
        m_pmeciCreateActs->c4AddEnumItem(L"TruncateExisting", L"Truncate Existing", tCIDLib::ECreateActs::TruncateExisting);
        m_pmeciCreateActs->BaseClassInit(meOwner);
        meOwner.c2AddClass(m_pmeciCreateActs);
        bAddNestedType(m_pmeciCreateActs->strClassPath());
    }

    //
    //  An enum for a common find operating result.
    //
    {
        m_pmeciFindRes = new TMEngEnumInfo
        (
            meOwner, L"FindRes", strClassPath(), L"MEng.Enum", 3
        );
        m_pmeciFindRes->c4AddEnumItem(L"Found", L"Found", tCIDLib::EFindRes::Found);
        m_pmeciFindRes->c4AddEnumItem(L"NotFound", L"NotFound", tCIDLib::EFindRes::NotFound);
        m_pmeciFindRes->c4AddEnumItem(L"NoMore", L"NoMore", tCIDLib::EFindRes::NoMore);
        m_pmeciFindRes->BaseClassInit(meOwner);
        meOwner.c2AddClass(m_pmeciFindRes);
        bAddNestedType(m_pmeciFindRes->strClassPath());
    }

    //
    //  An enum for those cases where their needs to be an indication that
    //  a path or URL or whatever is fully qualified or partially qualified
    //  or that it doesn't matter.
    //
    {
        m_pmeciFQTypes = new TMEngEnumInfo
        (
            meOwner, L"FQTypes", strClassPath(), L"MEng.Enum", 3
        );
        m_pmeciFQTypes->c4AddEnumItem(L"Partial", L"Partial", tCIDSock::EQualified::Partial);
        m_pmeciFQTypes->c4AddEnumItem(L"Full", L"Full", tCIDSock::EQualified::Full);
        m_pmeciFQTypes->c4AddEnumItem(L"DontCare", L"DontCare", tCIDSock::EQualified::DontCare);
        m_pmeciFQTypes->BaseClassInit(meOwner);
        meOwner.c2AddClass(m_pmeciFQTypes);
        bAddNestedType(m_pmeciFQTypes->strClassPath());
    }


    //
    //  Add an enumeration for the numeric radices that we support. We set
    //  their mapping values to the C++ values, so that we can easily map
    //  back to those values internally.
    //
    {
        m_pmeciRadices = new TMEngEnumInfo
        (
            meOwner, L"Radices", strClassPath(), L"MEng.Enum", 4
        );
        m_pmeciRadices->c4AddEnumItem(L"Bin", L"Binary", tCIDLib::ERadices::Bin);
        m_pmeciRadices->c4AddEnumItem(L"Octal", L"Octal", tCIDLib::ERadices::Oct);
        m_pmeciRadices->c4AddEnumItem(L"Dec", L"Decimal", tCIDLib::ERadices::Dec);
        m_pmeciRadices->c4AddEnumItem(L"Hex", L"Hexadecimal", tCIDLib::ERadices::Hex);
        m_pmeciRadices->BaseClassInit(meOwner);
        meOwner.c2AddClass(m_pmeciRadices);
        bAddNestedType(m_pmeciRadices->strClassPath());
    }

    // Generate our nested justification class
    {
        m_pmeciJustify = new TMEngEnumInfo
        (
            meOwner, L"HorzJustify", strClassPath(), L"MEng.Enum", 3
        );

        m_pmeciJustify->c4AddEnumItem(L"Left", L"Left", tCIDLib::EHJustify::Left);
        m_pmeciJustify->c4AddEnumItem(L"Right", L"Right", tCIDLib::EHJustify::Right);
        m_pmeciJustify->c4AddEnumItem(L"Center", L"Center", tCIDLib::EHJustify::Center);
        m_pmeciJustify->BaseClassInit(meOwner);
        meOwner.c2AddClass(m_pmeciJustify);
        bAddNestedType(m_pmeciJustify->strClassPath());
    }

    //
    //  Add an enumeration for some general macro engine errors, that are
    //  thrown for stuff that doesn't have anything to do with the actual
    //  code, but because of doing bad things with the engine.
    //
    {
        m_pmeciMEngErrors = new TMEngEnumInfo
        (
            meOwner, L"MEngErrors", strClassPath(), L"MEng.Enum", 4
        );

        // This one is a place holder. The text will be loaded by the engine
        c4ErrIdBadEntryParm = m_pmeciMEngErrors->c4AddEnumItem(L"BadEPParam", L"");

        c4ErrIdNotOpened = m_pmeciMEngErrors->c4AddEnumItem
        (
            L"NotOpened", L"The %(1) object was accessed before being opened"
        );
        m_pmeciMEngErrors->BaseClassInit(meOwner);
        meOwner.c2AddClass(m_pmeciMEngErrors);
        bAddNestedType(m_pmeciMEngErrors->strClassPath());
    }

    // Create an up/down/closest general purpose enum
    {
        m_pmeciUpDnCl = new TMEngEnumInfo
        (
            meOwner, L"UpDnClosest", strClassPath(), L"MEng.Enum", 3
        );

        m_pmeciUpDnCl->c4AddEnumItem(L"Closest", L"Closest", tCIDLib::ERoundTypes::Closest);
        m_pmeciUpDnCl->c4AddEnumItem(L"Down", L"Down", tCIDLib::ERoundTypes::Down);
        m_pmeciUpDnCl->c4AddEnumItem(L"Up", L"Up", tCIDLib::ERoundTypes::Up);
        m_pmeciUpDnCl->BaseClassInit(meOwner);
        meOwner.c2AddClass(m_pmeciUpDnCl);
        bAddNestedType(m_pmeciUpDnCl->strClassPath());
    }


    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.BaseInfo"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Check whether the debug flag is on in the macro engine
    {
        TMEngMethodInfo methiNew
        (
            L"IsInDebugMode"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_IsInDebugMode = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngBaseInfoInfo::pmecvMakeStorage(const   TString&                strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    TMEngStdClassVal* pmecvNew = new TMEngStdClassVal(strName, c2Id(), eConst);
    TJanitor<TMEngStdClassVal> janNew(pmecvNew);
    LoadMemberVals(*pmecvNew, meOwner);
    return janNew.pobjOrphan();
}


// ---------------------------------------------------------------------------
//  TMEngBaseInfoInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
// Some getters for direct access to our enum types
const TMEngEnumInfo& TMEngBaseInfoInfo::meciCreateActs() const
{
    return *m_pmeciCreateActs;
}


const TMEngEnumInfo& TMEngBaseInfoInfo::meciFindRes() const
{
    return *m_pmeciFindRes;
}


const TMEngEnumInfo& TMEngBaseInfoInfo::meciFQTypes() const
{
    return *m_pmeciFQTypes;
}


const TMEngEnumInfo& TMEngBaseInfoInfo::meciMEngErrors() const
{
    return *m_pmeciMEngErrors;
}


const TMEngEnumInfo& TMEngBaseInfoInfo::meciUpDnCl() const
{
    return *m_pmeciUpDnCl;
}


tCIDLib::TVoid
TMEngBaseInfoInfo::ThrowErr(const   tCIDLib::TCard4     c4ToThrow
                            , const TString&            strClassPath
                            ,       TCIDMacroEngine&    meOwner)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_pmeciMEngErrors->c2Id()
        , strClassPath
        , c4ToThrow
        , m_pmeciMEngErrors->strPartialName(c4ToThrow)
        , m_pmeciMEngErrors->strTextValue(c4ToThrow)
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngBaseInfoInfo::ThrowErr1(const  tCIDLib::TCard4     c4ToThrow
                            , const TString&            strClassPath
                            ,       TCIDMacroEngine&    meOwner
                            , const MFormattable&       fmtblToken)
{
    TString strText(m_pmeciMEngErrors->strTextValue(c4ToThrow));
    strText.eReplaceToken(fmtblToken, kCIDLib::chDigit1);

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_pmeciMEngErrors->c2Id()
        , strClassPath
        , c4ToThrow
        , m_pmeciMEngErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}


// ---------------------------------------------------------------------------
//  TMEngBaseInfoInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngBaseInfoInfo::bInvokeMethod(       TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      )
{
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        // Nothing to do
    }
     else if (c2MethId == m_c2MethId_IsInDebugMode)
    {
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(meOwner.bDebugMode());
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


