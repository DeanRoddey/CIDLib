//
// FILE NAME: CIDMacroEng_MPartMIME.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/01/2017
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
//  This file implements a class for parsing out the contents of multi-part MIME
//  messages.
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
RTTIDecls(TMEngMPartMIMEVal,TMEngClassVal)
RTTIDecls(TMEngMPartMIMEInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_MPartMIME
{
    // -----------------------------------------------------------------------
    //  The names for the types that we support here. Each derivative has to
    //  be able to return strings that contain its name and full name.
    // -----------------------------------------------------------------------
    const TString   strMPartMIME(L"MPartMIME");
    const TString   strMPartMIMEPath(L"MEng.System.Runtime.MPartMIME");
}


// ---------------------------------------------------------------------------
//  CLASS: TMEngMPartMIMEVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngMPartMIMEVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngMPartMIMEVal::TMEngMPartMIMEVal(const  TString&                strName
                                    , const tCIDLib::TCard2         c2Id
                                    , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
{
}

TMEngMPartMIMEVal::~TMEngMPartMIMEVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngMPartMIMEVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngMPartMIMEVal::bDbgFormat(          TTextOutStream&
                                , const TMEngClassInfo&
                                , const tCIDMacroEng::EDbgFmts
                                , const tCIDLib::ERadices
                                , const TCIDMacroEngine&    ) const
{
    // We have no state currently
    return kCIDLib::False;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngMPartMIMEInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngMPartMIMEInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngMPartMIMEInfo::strPath()
{
    return CIDMacroEng_MPartMIME::strMPartMIMEPath;
}


// ---------------------------------------------------------------------------
//  TMEngMPartMIMEInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngMPartMIMEInfo::TMEngMPartMIMEInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_MPartMIME::strMPartMIME
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_ParseBuf(kMacroEng::c2BadId)
    , m_c4ErrId_Parse(kCIDLib::c4MaxCard)
    , m_pmeciErrors(0)
{
}

TMEngMPartMIMEInfo::~TMEngMPartMIMEInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngMPartMIMEInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngMPartMIMEInfo::Init(TCIDMacroEngine& meOwner)
{
    //
    //  An enum to represent the exceptions that this class throws. Note
    //  that we just use the text in the C++ exception in some cases, so there
    //  is no text for the enum value for those.
    //
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"MPartMIMEErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 5
        );
        m_c4ErrId_Parse = m_pmeciErrors->c4AddEnumItem(L"ParseErr", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2TypeId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }


    // The only constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.MPartMIME"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    //
    //  Parse an incoming buffer and return a list of sizes, a list of encodings, a
    //  list of MIME types, and a list of memory buffers.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"ParseBuf"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_ParseBuf = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngMPartMIMEInfo::pmecvMakeStorage(  const   TString&                strName
                                        ,       TCIDMacroEngine&        meOwner
                                        , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngMPartMIMEVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngMPartMIMEInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngMPartMIMEInfo::bInvokeMethod(        TCIDMacroEngine&    meOwner
                                    , const TMEngMethodInfo&    methiTarget
                                    ,       TMEngClassVal&      mecvInstance)
{
    TMEngMPartMIMEVal& mecvActual = static_cast<TMEngMPartMIMEVal&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
    }
     else if (c2MethId == m_c2MethId_ParseBuf)
    {
        try
        {
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            ThrowAnErr(meOwner, m_c4ErrId_Parse, errToCatch);
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngMPartMIMEInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngMPartMIMEInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
                                , const tCIDLib::TCard4     c4ToThrow
                                , const TError&             errCaught)
{
    // If verbose log mode, we'll log the C++ level exception
    if (!errCaught.bLogged() && facCIDMacroEng().bLogWarnings())
        TModule::LogEventObj(errCaught);

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , errCaught.strErrText()
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}


tCIDLib::TVoid
TMEngMPartMIMEInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
                                , const tCIDLib::TCard4     c4ToThrow)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , m_pmeciErrors->strTextValue(c4ToThrow)
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}
