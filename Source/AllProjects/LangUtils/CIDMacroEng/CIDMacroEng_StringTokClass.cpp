//
// FILE NAME: CIDMacroEng_StringTok.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/05/2003
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
//  macro engine class that provides string tokenization functionality.
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
RTTIDecls(TMEngStringTokInfo,TMacroEngClass)
RTTIDecls(TMEngStringTokVal,TMEngClassVal)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_StringTokClass
{
    namespace
    {
        const TString   strClassPath(L"MEng.System.Runtime.StringTokenizer");
        const TString   strName(L"StringTokenizer");
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngStringTokVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngStringTokVal: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngStringTokVal::TMEngStringTokVal(const  TString&                strName
                                    , const tCIDLib::TCard2         c2Id
                                    , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_stokValue(&m_strValue, kCIDLib::szWhitespace)
{
}

TMEngStringTokVal::~TMEngStringTokVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngStringTokVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngStringTokVal::bDbgFormat(          TTextOutStream&
                                , const TMEngClassInfo&
                                , const tCIDMacroEng::EDbgFmts
                                , const tCIDLib::ERadices
                                , const TCIDMacroEngine&) const
{
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TMEngStringTokVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngStringTokVal::Init()
{
    m_strValue.Clear();
    m_stokValue.Reset();
}


const TStringTokenizer& TMEngStringTokVal::stokValue() const
{
    return m_stokValue;
}

TStringTokenizer& TMEngStringTokVal::stokValue()
{
    return m_stokValue;
}


tCIDLib::TVoid
TMEngStringTokVal::Setup(const TString& strToTok, const TString& strWhitespace)
{
    m_strValue = strToTok;
    m_stokValue.Reset(&m_strValue, strWhitespace);
}


tCIDLib::TVoid TMEngStringTokVal::SetWhitespace(const TString& strToSet)
{
    m_stokValue.strWhitespace(strToSet);
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngStringTokInfo
// PREFIX: meunit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCharInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngStringTokInfo::strPath()
{
    return CIDMacroEng_StringTokClass::strClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngStringTokInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngStringTokInfo::TMEngStringTokInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_StringTokClass::strName
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_BuildQuotedCommaList(kCIDMacroEng::c2BadId)
    , m_c2MethId_BuildQuotedCommaList2(kCIDMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_EatNextToken(kCIDMacroEng::c2BadId)
    , m_c2MethId_FormatQuotedList(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetNextToken(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetRestOfLine(kCIDMacroEng::c2BadId)
    , m_c2MethId_MoreTokens(kCIDMacroEng::c2BadId)
    , m_c2MethId_PeekRestOfLine(kCIDMacroEng::c2BadId)
    , m_c2MethId_ParseCmdLine(kCIDMacroEng::c2BadId)
    , m_c2MethId_ParseCSVLine(kCIDMacroEng::c2BadId)
    , m_c2MethId_ParseQuotedCommaList(kCIDMacroEng::c2BadId)
    , m_c2MethId_Reset(kCIDMacroEng::c2BadId)
    , m_c2MethId_Set(kCIDMacroEng::c2BadId)
    , m_c2MethId_SetWhitespace(kCIDMacroEng::c2BadId)
    , m_c2MethId_Tokenize(kCIDMacroEng::c2BadId)
    , m_c2TypeId_StrVec(kCIDMacroEng::c2BadId)
    , m_c4ErrBadCmdLine(kCIDLib::c4MaxCard)
    , m_c4ErrBuild(kCIDLib::c4MaxCard)
    , m_c4ErrParse(kCIDLib::c4MaxCard)
    , m_c4ErrTokenize(kCIDLib::c4MaxCard)
{
}

TMEngStringTokInfo::~TMEngStringTokInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngStringTokInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngStringTokInfo::Init(TCIDMacroEngine& meOwner)
{
    // We need to generate an enum class for our errors
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"StrTokErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 4
        );
        m_c4ErrBadCmdLine = m_pmeciErrors->c4AddEnumItem(L"BadCmdLine", TString::strEmpty());
        m_c4ErrBuild = m_pmeciErrors->c4AddEnumItem(L"Build", TString::strEmpty());
        m_c4ErrParse = m_pmeciErrors->c4AddEnumItem(L"ParseErr", TString::strEmpty());
        m_c4ErrTokenize = m_pmeciErrors->c4AddEnumItem(L"TokenizeErr", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // Add a literal for default whitespace chars
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kDefWhitespace"
            , tCIDMacroEng::EIntrinsics::String
            , new TMEngStringVal
              (
                L"kDefWhitespace"
                , tCIDMacroEng::EConstTypes::Const
                , kCIDLib::szWhitespace
              )
        )
    );

    //
    //  Make sure that there is a vector of strings defined. The actual
    //  one we get may be under a different name, but it'll be considered
    //  the same if they have the same collection types and element type.
    //
    {
        TMEngVectorInfo* pmeciNew = new TMEngVectorInfo
        (
            meOwner
            , L"ParmStrList"
            , strClassPath()
            , TMEngVectorInfo::strPath()
            , tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::String)
        );
        TJanitor<TMEngVectorInfo> janNewClass(pmeciNew);
        pmeciNew->BaseClassInit(meOwner);
        bAddNestedType(pmeciNew->strClassPath());
        m_c2TypeId_StrVec = meOwner.c2AddClass(janNewClass.pobjOrphan());
    }


    // Builds up quoted comma list type strings, one string at a time
    {
        TMEngMethodInfo methiNew
        (
            L"BuildQuotedCommaList"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToAppend", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Accum", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_BuildQuotedCommaList = c2AddMethodInfo(methiNew);
    }

    // Builds up quoted comma list type strings from a stringlist object
    {
        TMEngMethodInfo methiNew
        (
            L"BuildQuotedCommaList2"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"SrcList", tCIDMacroEng::EIntrinsics::StringList);
        methiNew.c2AddOutParm(L"Accum", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_BuildQuotedCommaList2 = c2AddMethodInfo(methiNew);
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.StringTokenizer"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Destructively throw away the next token
    {
        TMEngMethodInfo methiNew
        (
            L"EatNextToken"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_EatNextToken = c2AddMethodInfo(methiNew);
    }

    // Format out a list of strings as a quoted comma string
    {
        TMEngMethodInfo methiNew
        (
            L"FormatQuotedList"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToFmt", m_c2TypeId_StrVec);
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_FormatQuotedList = c2AddMethodInfo(methiNew);
    }

    // Destructively get the next token
    {
        TMEngMethodInfo methiNew
        (
            L"GetNextToken"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetNextToken = c2AddMethodInfo(methiNew);
    }

    // Get the rest of the text
    {
        TMEngMethodInfo methiNew
        (
            L"GetRestOfLine"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetRestOfLine = c2AddMethodInfo(methiNew);
    }

    // Check for more tokens available
    {
        TMEngMethodInfo methiNew
        (
            L"MoreTokens"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_MoreTokens = c2AddMethodInfo(methiNew);
    }

    // Peek the rest of the text
    {
        TMEngMethodInfo methiNew
        (
            L"PeekRestOfLine"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_PeekRestOfLine = c2AddMethodInfo(methiNew);
    }

    // Do a standard command line type parse
    {
        TMEngMethodInfo methiNew
        (
            L"ParseCmdLine"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToParse", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"ToFill", m_c2TypeId_StrVec);
        m_c2MethId_ParseCmdLine = c2AddMethodInfo(methiNew);
    }

    // Break out a standard CSV line
    {
        TMEngMethodInfo methiNew
        (
            L"ParseCSVLine"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToParse", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"ToFill", m_c2TypeId_StrVec);
        methiNew.c2AddOutParm(L"ErrIndex", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ParseCSVLine = c2AddMethodInfo(methiNew);
    }

    // Break out a comma separated list of quoted values
    {
        TMEngMethodInfo methiNew
        (
            L"ParseQuotedCommaList"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToParse", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"ToFill", m_c2TypeId_StrVec);
        methiNew.c2AddOutParm(L"ErrIndex", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ParseQuotedCommaList = c2AddMethodInfo(methiNew);
    }

    // Add the reset method
    {
        TMEngMethodInfo methiNew
        (
            L"Reset"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Reset = c2AddMethodInfo(methiNew);
    }

    // Add the set method
    {
        TMEngMethodInfo methiNew
        (
            L"Set"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToIterate", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Whitespace", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_Set = c2AddMethodInfo(methiNew);
    }

    // Add one to just change the whitespace used
    {
        TMEngMethodInfo methiNew
        (
            L"SetWhitespace"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Whitespace", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_SetWhitespace = c2AddMethodInfo(methiNew);
    }

    // Do a whole tokenize loop in one shot, filling in a vector of strings
    {
        TMEngMethodInfo methiNew
        (
            L"Tokenize"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToTok", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Whitespace", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"ToFill", m_c2TypeId_StrVec);
        m_c2MethId_Tokenize = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngStringTokInfo::pmecvMakeStorage(  const   TString&                strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    TMEngStringTokVal* pmecvNew = new TMEngStringTokVal(strName, c2Id(), eConst);
    TJanitor<TMEngStringTokVal> janNew(pmecvNew);
    return janNew.pobjOrphan();
}


// ---------------------------------------------------------------------------
//  TMEngStringTokInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngStringTokInfo::bInvokeMethod(          TCIDMacroEngine&    meOwner
                                    , const TMEngMethodInfo&    methiTarget
                                    ,       TMEngClassVal&      mecvInstance)
{
    TMEngStringTokVal& mecvActual = static_cast<TMEngStringTokVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_BuildQuotedCommaList)
    {
        TMEngStringVal& mecvToFill
                        = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 1);

        try
        {
            mecvActual.stokValue().BuildQuotedCommaList
            (
                meOwner.strStackValAt(c4FirstInd), mecvToFill.strValue()
            );
        }

        catch(TError& errToCatch)
        {
            // If verbose log mode, we'll log the C++ level exception
            if (!errToCatch.bLogged() && facCIDMacroEng().bLogWarnings())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            // Throw a macro level exception
            ThrowAnErr(meOwner, m_c4ErrBuild, errToCatch.strErrText());
        }
    }
     else if (c2MethId == m_c2MethId_BuildQuotedCommaList2)
    {
        // Get the string to format into and empty it
        TMEngStringVal& mecvToFill
                        = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 1);
        mecvToFill.strValue().Clear();

        // Get the source string list
        const TMEngStrListVal& mecvList = meOwner.mecvStackAtAs<TMEngStrListVal>(c4FirstInd);

        // Iterate the stringlist values and add them to the target string
        try
        {
            const tCIDLib::TCard4 c4Count = mecvList.c4ElemCount();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                mecvActual.stokValue().BuildQuotedCommaList
                (
                    mecvList.strAt(c4Index), mecvToFill.strValue()
                );
            }
        }

        catch(TError& errToCatch)
        {
            // If verbose log mode, we'll log the C++ level exception
            if (!errToCatch.bLogged() && facCIDMacroEng().bLogWarnings())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            // Throw a macro level exception
            ThrowAnErr(meOwner, m_c4ErrBuild, errToCatch.strErrText());
        }
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Just reinitialize it
        mecvActual.Init();
    }
     else if (c2MethId == m_c2MethId_EatNextToken)
    {
        TMEngBooleanVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.stokValue().bEatNextToken());
    }
     else if (c2MethId == m_c2MethId_FormatQuotedList)
    {
        // Get the srce list of values to format and the target string
        const TMEngVectorVal& mecvList(meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd));
        TMEngStringVal& mecvToFill(meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 1));
        TString& strToFill = mecvToFill.strValue();

        // Clear the string then iterate the list and format it out
        strToFill.Clear();

        const tCIDLib::TCard4 c4Count = mecvList.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            // Cast the vector element to its actual type
            const TMEngStringVal& mecvCur = static_cast<const TMEngStringVal&>
            (
                mecvList.mecvAt(meOwner, c4Index)
            );
            mecvActual.stokValue().BuildQuotedCommaList(mecvCur.strValue(), strToFill);
        }
    }
     else if (c2MethId == m_c2MethId_GetNextToken)
    {
        TMEngBooleanVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngStringVal& mecvToFill
                        = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
        mecvRet.bValue(mecvActual.stokValue().bGetNextToken(mecvToFill.strValue()));
    }
     else if (c2MethId == m_c2MethId_GetRestOfLine)
    {
        TMEngBooleanVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngStringVal& mecvToFill
                        = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
        mecvRet.bValue(mecvActual.stokValue().bGetRestOfLine(mecvToFill.strValue()));
    }
     else if (c2MethId == m_c2MethId_MoreTokens)
    {
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.stokValue().bMoreTokens());
    }
     else if (c2MethId == m_c2MethId_PeekRestOfLine)
    {
        TMEngBooleanVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngStringVal& mecvToFill
                        = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
        mecvRet.bValue(mecvActual.stokValue().bPeekRestOfLine(mecvToFill.strValue()));
    }
     else if (c2MethId == m_c2MethId_ParseCmdLine)
    {
        // Get the target vector and clear it
        TMEngVectorVal& mecvToFill = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 1);
        mecvToFill.RemoveAll();

        // We'll have to use a local temp collection
        try
        {
            TVector<TString> colParms;
            const tCIDLib::TCard4 c4Count = TExternalProcess::c4BreakOutParms
            (
                meOwner.strStackValAt(c4FirstInd)
                , colParms
            );

            // If we got any, then fill the macro level vector
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                mecvToFill.AddObject
                (
                    new TMEngStringVal
                    (
                        TString::strEmpty()
                        , tCIDMacroEng::EConstTypes::NonConst
                        , colParms[c4Index]
                    )
                );
            }
        }

        catch(TError& errToCatch)
        {
            // If verbose log mode, we'll log the C++ level exception
            if (!errToCatch.bLogged() && facCIDMacroEng().bLogWarnings())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            // Throw a macro level exception
            ThrowAnErr(meOwner, m_c4ErrBadCmdLine, errToCatch.strErrText());
        }
    }
     else if ((c2MethId == m_c2MethId_ParseCSVLine)
          ||  (c2MethId == m_c2MethId_ParseQuotedCommaList))
    {
        //
        //  These are the same, so it just matters which unerlying method we
        //  call. They have the same input and output parameters.
        //

        // Get the target vector and clear it
        TMEngVectorVal& mecvToFill = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 1);
        mecvToFill.RemoveAll();

        // We'll have to use a local temp collection
        try
        {
            TVector<TString> colVals;
            const TString& strSrc = meOwner.strStackValAt(c4FirstInd);
            TMEngBooleanVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
            TMEngCard4Val& mecvErrInd
                        = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 2);

            tCIDLib::TBoolean   bRes;
            tCIDLib::TCard4     c4ErrInd;

            if (c2MethId == m_c2MethId_ParseCSVLine)
                bRes = TStringTokenizer::bParseCSVLine(strSrc, colVals, c4ErrInd);
            else
                bRes = TStringTokenizer::bParseQuotedCommaList(strSrc, colVals, c4ErrInd);

            if (bRes)
            {
                // If we got any, then fill the macro level vector
                const tCIDLib::TCard4 c4Count = colVals.c4ElemCount();
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                {
                    mecvToFill.AddObject
                    (
                        new TMEngStringVal
                        (
                            TString::strEmpty()
                            , tCIDMacroEng::EConstTypes::NonConst
                            , colVals[c4Index]
                        )
                    );
                }
                mecvRet.bValue(kCIDLib::True);
            }
             else
            {
                mecvErrInd.c4Value(c4ErrInd);
                mecvRet.bValue(kCIDLib::False);
            }
        }

        catch(TError& errToCatch)
        {
            // If verbose log mode, we'll log the C++ level exception
            if (!errToCatch.bLogged() && facCIDMacroEng().bLogWarnings())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            // Throw a macro level exception
            ThrowAnErr(meOwner, m_c4ErrParse, errToCatch.strErrText());
        }

    }
     else if (c2MethId == m_c2MethId_Reset)
    {
        mecvActual.stokValue().Reset();
    }
     else if (c2MethId == m_c2MethId_Set)
    {
        mecvActual.Setup
        (
            meOwner.strStackValAt(c4FirstInd)
            , meOwner.strStackValAt(c4FirstInd + 1)
        );
    }
     else if (c2MethId == m_c2MethId_SetWhitespace)
    {
        mecvActual.SetWhitespace(meOwner.strStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Tokenize)
    {
        // Get the target vector and clear it
        TMEngVectorVal& mecvToFill = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 2);
        mecvToFill.RemoveAll();

        // Set up the tokenizer object with the provided values
        mecvActual.Setup
        (
            meOwner.strStackValAt(c4FirstInd), meOwner.strStackValAt(c4FirstInd + 1)
        );

        // Now get the tokenizer out and do the loop
        TStringTokenizer& stokThis = mecvActual.stokValue();
        TString strTmp;
        try
        {
            while (stokThis.bGetNextToken(strTmp))
            {
                mecvToFill.AddObject
                (
                    new TMEngStringVal
                    (
                        TString::strEmpty(), tCIDMacroEng::EConstTypes::NonConst, strTmp
                    )
                );
            }
        }

        catch(TError& errToCatch)
        {
            // If verbose log mode, we'll log the C++ level exception
            if (!errToCatch.bLogged() && facCIDMacroEng().bLogWarnings())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            // Throw a macro level exception
            ThrowAnErr(meOwner, m_c4ErrTokenize, errToCatch.strErrText());
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngStringTokInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngStringTokInfo::ThrowAnErr(         TCIDMacroEngine&    meOwner
                                , const tCIDLib::TCard4     c4ToThrow
                                , const TString&            strText)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}


