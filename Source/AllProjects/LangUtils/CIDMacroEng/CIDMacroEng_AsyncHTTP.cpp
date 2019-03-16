//
// FILE NAME: CIDMacroEng_AsyncHTTP.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/17/2005
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
//  This file implements a simple wrapper around the HTTP stuff, to support async
//  HTTP queries. Since CML is single threaded, this allows CML classes to get some
//  async downloading capabilities.
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
RTTIDecls(TMEngAsyncHTTPClVal,TMEngClassVal)
RTTIDecls(TMEngAsyncHTTPClInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_NetClasses
{
    // -----------------------------------------------------------------------
    //  The names for the types that we support here. Each derivative has to
    //  be able to return strings that contain its name and full name.
    // -----------------------------------------------------------------------
    const TString   strAsyncHTTP(L"AsyncHTTPCl");
    const TString   strAsyncHTTPPath(L"MEng.System.Runtime.AsyncHTTPCl");
}


// ---------------------------------------------------------------------------
//  CLASS: TMEngAsyncHTTPClVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngAsyncHTTPClVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngAsyncHTTPClVal::TMEngAsyncHTTPClVal(const  TString&                strName
                                        , const tCIDLib::TCard2         c2Id
                                        , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_pahttpqValue(new TAsyncHTTPQ())
{
}

TMEngAsyncHTTPClVal::~TMEngAsyncHTTPClVal()
{
    delete m_pahttpqValue;
}


// ---------------------------------------------------------------------------
//  TMEngAsyncHTTPClVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngAsyncHTTPClVal::bDbgFormat(         TTextOutStream&     strmTarget
                                , const TMEngClassInfo&     meciThis
                                , const tCIDMacroEng::EDbgFmts eFormat
                                , const tCIDLib::ERadices   eRadix
                                , const TCIDMacroEngine&    meOwner) const
{
    if (m_pahttpqValue)
    {
        if (eFormat == tCIDMacroEng::EDbgFmts::Long)
        {
            strmTarget  << L"Title: " << m_pahttpqValue->strTitle() << kCIDLib::NewLn
                        << L"Status: ";

            switch(m_pahttpqValue->eState())
            {
                case tCIDLib::EAsyncWaitRes::Complete :
                    strmTarget << L"Complete";
                    break;

                case tCIDLib::EAsyncWaitRes::Error :
                    strmTarget << L"Error";
                    break;

                case tCIDLib::EAsyncWaitRes::Waiting :
                    strmTarget << L"Waiting";
                    break;

                default :
                    strmTarget << L"Unknown";
                    break;
            };
            strmTarget << kCIDLib::NewLn;

            if (m_pahttpqValue->eState() == tCIDLib::EAsyncWaitRes::Error)
            {
                strmTarget  << L"\nError=\n" << m_pahttpqValue->errLast()
                            << kCIDLib::DNewLn;
            }
            return kCIDLib::True;
        }
    }
     else
    {
        strmTarget << L"Not initialized yet";
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TMEngAsyncHTTPClVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
TAsyncHTTPQ& TMEngAsyncHTTPClVal::ahttpqValue()
{
    return *m_pahttpqValue;
}


tCIDLib::EAsyncWaitRes TMEngAsyncHTTPClVal::eState() const
{
    return m_pahttpqValue->eState();
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngAsyncHTTPClInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngAsyncHTTPClInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngAsyncHTTPClInfo::strPath()
{
    return CIDMacroEng_NetClasses::strAsyncHTTPPath;
}


// ---------------------------------------------------------------------------
//  TMEngAsyncHTTPClInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngAsyncHTTPClInfo::TMEngAsyncHTTPClInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_NetClasses::strAsyncHTTP
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_CancelOp(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_GetOutput(kMacroEng::c2BadId)
    , m_c2MethId_GetStatus(kMacroEng::c2BadId)
    , m_c2MethId_StartGETRedir(kMacroEng::c2BadId)
    , m_c2MethId_StartPOST(kMacroEng::c2BadId)
    , m_c2TypeId_HdrList(kMacroEng::c2BadId)
    , m_c2TypeId_KVPair(kMacroEng::c2BadId)
    , m_c2TypeId_Errors(kMacroEng::c2BadId)
    , m_c2TypeId_States(kMacroEng::c2BadId)
    , m_c4ErrId_Cancel(kCIDLib::c4MaxCard)
    , m_c4ErrId_Output(kCIDLib::c4MaxCard)
    , m_c4ErrId_Start(kCIDLib::c4MaxCard)
    , m_c4ErrId_Status(kCIDLib::c4MaxCard)
    , m_pmeciErrors(0)
    , m_pmeciStates(0)
{
    // Force the import of non-intrinsic classes we reference
    bAddClassImport(TMEngHTTPClientInfo::strPath());
    bAddClassImport(TMEngURLInfo::strPath());
    bAddClassImport(TMEngKVPairInfo::strPath());
}

TMEngAsyncHTTPClInfo::~TMEngAsyncHTTPClInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngAsyncHTTPClInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngAsyncHTTPClInfo::Init(TCIDMacroEngine& meOwner)
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
            , L"AsyncHTTPErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 5
        );
        m_c4ErrId_Busy = m_pmeciErrors->c4AddEnumItem(L"Busy", L"An operation is still in progress");
        m_c4ErrId_Cancel = m_pmeciErrors->c4AddEnumItem(L"Cancel", TString::strEmpty());
        m_c4ErrId_Output = m_pmeciErrors->c4AddEnumItem(L"Output", TString::strEmpty());
        m_c4ErrId_Start = m_pmeciErrors->c4AddEnumItem(L"Start", TString::strEmpty());
        m_c4ErrId_Status = m_pmeciErrors->c4AddEnumItem(L"Status", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2TypeId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // And one for our status values
    {
        m_pmeciStates = new TMEngEnumInfo
        (
            meOwner
            , L"AsyncHTTPStates"
            , strClassPath()
            , L"MEng.Enum"
            , 4
        );
        m_pmeciStates->c4AddEnumItem(L"Waiting", L"Waiting", tCIDLib::EAsyncWaitRes::Waiting);
        m_pmeciStates->c4AddEnumItem(L"Complete", L"Complete", tCIDLib::EAsyncWaitRes::Complete);
        m_pmeciStates->c4AddEnumItem(L"Error", L"Error", tCIDLib::EAsyncWaitRes::Error);
        m_pmeciStates->BaseClassInit(meOwner);
        m_c2TypeId_States = meOwner.c2AddClass(m_pmeciStates);
        bAddNestedType(m_pmeciStates->strClassPath());
    }


    //
    //  Look up some classes that we need to use. We need the URL class just
    //  for methods declarations below. We need the type of if the KVPair class
    //  for later. Also look up the HTTP client's vector of KV pairs, so that
    //  we can use the same type.
    //
    const tCIDLib::TCard2 c2URLId
    (
        meOwner.meciFind(TMEngURLInfo::strPath()).c2Id()
    );
    m_c2TypeId_KVPair = meOwner.c2FindClassId(TMEngKVPairInfo::strPath());
    m_c2TypeId_HdrList = meOwner.c2FindClassId
    (
        TMEngHTTPClientInfo::strPath() + TString(L".LinesList")
    );


    // The only constructor, which takes a title string
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.AsyncHTTPCl"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Cancel any outstanding operation
    {
        TMEngMethodInfo methiNew
        (
            L"CancelOp"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_CancelOp = c2AddMethodInfo(methiNew);
    }

    // Get the results of the last completed operation
    {
        TMEngMethodInfo methiNew
        (
            L"GetOutput"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );

        methiNew.c2AddOutParm(L"RepText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"OutHdrLines", m_c2TypeId_HdrList);
        methiNew.c2AddOutParm(L"ContType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Content", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddOutParm(L"ContLen", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"FinalURL", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetOutput = c2AddMethodInfo(methiNew);
    }

    // Cancel any outstanding operation
    {
        TMEngMethodInfo methiNew
        (
            L"GetStatus"
            , m_c2TypeId_States
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ThrowIfErr", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_GetStatus = c2AddMethodInfo(methiNew);
    }

    // Start a GET operation (with auto-redirect), if not already in one
    {
        TMEngMethodInfo methiNew
        (
            L"StartGETRedir"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Title", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"URL", c2URLId);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Agent", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Accept", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"InHdrLines", m_c2TypeId_HdrList);
        m_c2MethId_StartGETRedir = c2AddMethodInfo(methiNew);
    }

    // Start a POST operation, if no already in one
    {
        TMEngMethodInfo methiNew
        (
            L"StartPOST"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Title", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"URL", c2URLId);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Agent", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Accept", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"PostVals", m_c2TypeId_HdrList);
        methiNew.c2AddInParm(L"InHdrLines", m_c2TypeId_HdrList);
        m_c2MethId_StartPOST = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngAsyncHTTPClInfo::pmecvMakeStorage(  const   TString&                strName
                                        ,       TCIDMacroEngine&        meOwner
                                        , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngAsyncHTTPClVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngAsyncHTTPClInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngAsyncHTTPClInfo::bInvokeMethod(        TCIDMacroEngine&    meOwner
                                    , const TMEngMethodInfo&    methiTarget
                                    ,       TMEngClassVal&      mecvInstance)
{
    TMEngAsyncHTTPClVal& mecvActual = static_cast<TMEngAsyncHTTPClVal&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
    }
     else if (c2MethId == m_c2MethId_CancelOp)
    {
        try
        {
            mecvActual.ahttpqValue().CancelOp();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            ThrowAnErr(meOwner, m_c4ErrId_Cancel, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetOutput)
    {
        // If we are in error state, throw
        if (mecvActual.ahttpqValue().eState() == tCIDLib::EAsyncWaitRes::Error)
            ThrowAnErr(meOwner, m_c4ErrId_Status, mecvActual.ahttpqValue().errLast());

        if (mecvActual.ahttpqValue().eState() == tCIDLib::EAsyncWaitRes::Waiting)
            ThrowAnErr(meOwner, m_c4ErrId_Busy);

        tCIDLib::TCard4     c4ContLen;
        tCIDLib::TCard4     c4ResCode;
        tCIDNet::EHTTPCodes eCodeType;
        tCIDLib::TKVPList   colOutHdrLines;

        TMEngStringVal& mecvRepText = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
        TMEngStringVal& mecvContType = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 2);
        TMEngMemBufVal& mecvCont = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 3);
        TMEngStringVal& mecvFinalURL = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 5);

        try
        {
            c4ResCode = mecvActual.ahttpqValue().c4GetOutput
            (
                eCodeType
                , mecvRepText.strValue()
                , colOutHdrLines
                , mecvContType.strValue()
                , mecvCont.mbufValue()
                , c4ContLen
                , mecvFinalURL.strValue()
            );
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            ThrowAnErr(meOwner, m_c4ErrId_Output, errToCatch);
        }

        // Fill in the rest of the values we couldn't go straight into
        TMEngCard4Val& mecvContLen = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 4);
        mecvContLen.c4Value(c4ContLen);

        // The return value if the result code
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(c4ResCode);

        //
        //  We need to get the header lines out of our local vector and
        //  into the CML level vector.
        //
        TMEngVectorVal& mecvOutHdrLines = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 1);
        mecvOutHdrLines.RemoveAll();
        const tCIDLib::TCard4 c4Count = colOutHdrLines.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            const TKeyValuePair& kvalCur = colOutHdrLines[c4Index];
            mecvOutHdrLines.AddObject
            (
                new TMEngKVPairVal
                (
                    TString::strEmpty()
                    , m_c2TypeId_KVPair
                    , tCIDMacroEng::EConstTypes::NonConst
                    , kvalCur.strKey()
                    , kvalCur.strValue()
                )
            );
        }
    }
     else if (c2MethId == m_c2MethId_GetStatus)
    {
        // If they want to throw on error and it's in error, let's do that
        if (meOwner.bStackValAt(c4FirstInd)
        &&  (mecvActual.ahttpqValue().eState() == tCIDLib::EAsyncWaitRes::Error))
        {
            ThrowAnErr(meOwner, m_c4ErrId_Status, mecvActual.ahttpqValue().errLast());
        }

        // Else return the status
        TMEngEnumVal& mecvRet = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
        mecvRet.c4Ordinal(m_pmeciStates->c4FromMapValue(mecvActual.eState()));
    }
     else if (c2MethId == m_c2MethId_StartGETRedir)
    {
        // Can't have an active operation
        if (mecvActual.ahttpqValue().eState() == tCIDLib::EAsyncWaitRes::Waiting)
            ThrowAnErr(meOwner, m_c4ErrId_Busy);

        try
        {
            // Input parms
            const TString& strTitle= meOwner.strStackValAt(c4FirstInd);
            const TMEngURLVal& mecvURL = meOwner.mecvStackAtAs<TMEngURLVal>(c4FirstInd + 1);
            const tCIDLib::TCard4 c4WaitFor = meOwner.c4StackValAt(c4FirstInd + 2);
            const TString& strAgent = meOwner.strStackValAt(c4FirstInd + 3);
            const TString& strAccept = meOwner.strStackValAt(c4FirstInd + 4);

            // Load up any incoming header lines
            const TMEngVectorVal& mecvInLines
            (
                meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 5)
            );
            tCIDLib::TCard4 c4Count = mecvInLines.c4ElemCount();

            tCIDLib::TKVPList colInHdrLines(c4Count);
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                // Get the element and cast to its real type
                const TMEngKVPairVal& mecvCur
                (
                    static_cast<const TMEngKVPairVal&>
                    (
                        mecvInLines.mecvAt(meOwner, c4Index)
                    )
                );
                colInHdrLines.objAdd
                (
                    TKeyValuePair(mecvCur.strKey(), mecvCur.strValue())
                );
            }

            mecvActual.ahttpqValue().StartGETRedir
            (
                strTitle
                , mecvURL.urlValue()
                , c4WaitFor
                , strAgent
                , strAccept
                , colInHdrLines
            );
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            ThrowAnErr(meOwner, m_c4ErrId_Start, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_StartPOST)
    {
        // Can't have an active operation
        if (mecvActual.ahttpqValue().eState() == tCIDLib::EAsyncWaitRes::Waiting)
            ThrowAnErr(meOwner, m_c4ErrId_Busy);

        try
        {
            // Input parms
            const TString& strTitle= meOwner.strStackValAt(c4FirstInd);
            const TMEngURLVal& mecvURL = meOwner.mecvStackAtAs<TMEngURLVal>(c4FirstInd + 1);
            const tCIDLib::TCard4 c4WaitFor = meOwner.c4StackValAt(c4FirstInd + 2);
            const TString& strAgent = meOwner.strStackValAt(c4FirstInd + 3);
            const TString& strAccept = meOwner.strStackValAt(c4FirstInd + 4);

            //
            //  Get the incoming k/v pairs that represent the post values out of
            //  the CML level vector and into a local one.
            //
            tCIDLib::TKVPList colPostVals;
            TMEngVectorVal& mecvVals = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 5);
            const tCIDLib::TCard4 c4ValCount = mecvVals.c4ElemCount();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4ValCount; c4Index++)
            {
                const TMEngKVPairVal& mecvCur
                (
                    static_cast<const TMEngKVPairVal&>
                    (
                        mecvVals.mecvAt(meOwner, c4Index)
                    )
                );
                colPostVals.objAdd
                (
                    TKeyValuePair(mecvCur.strKey(), mecvCur.strValue())
                );
            }

            const TMEngVectorVal& mecvInLines
            (
                meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 6)
            );
            tCIDLib::TCard4 c4Count = mecvInLines.c4ElemCount();

            tCIDLib::TKVPList colInHdrLines(c4Count);
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                // Get the element and cast to its real type
                const TMEngKVPairVal& mecvCur
                (
                    static_cast<const TMEngKVPairVal&>
                    (
                        mecvInLines.mecvAt(meOwner, c4Index)
                    )
                );
                colInHdrLines.objAdd
                (
                    TKeyValuePair(mecvCur.strKey(), mecvCur.strValue())
                );
            }

            mecvActual.ahttpqValue().StartPOST
            (
                strTitle
                , mecvURL.urlValue()
                , c4WaitFor
                , strAgent
                , strAccept
                , colPostVals
                , colInHdrLines
            );
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            ThrowAnErr(meOwner, m_c4ErrId_Start, errToCatch);
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngAsyncHTTPClInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngAsyncHTTPClInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
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
TMEngAsyncHTTPClInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
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
