//
// FILE NAME: CIDMacroEng_NetClasses.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/17/2005
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a set of derivatives of the class info and class
//  value classes, which implement the macro level HTTP and URL classes.
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
RTTIDecls(TMEngDataSrcVal,TMEngClassVal)
RTTIDecls(TMEngDataSrcInfo,TMEngClassInfo)
RTTIDecls(TMEngHTTPClientVal,TMEngClassVal)
RTTIDecls(TMEngHTTPClientInfo,TMEngClassInfo)
RTTIDecls(TMEngURLVal,TMEngClassVal)
RTTIDecls(TMEngURLInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_NetClasses
{
    // -----------------------------------------------------------------------
    //  The names for the types that we support here. Each derivative has to
    //  be able to return strings that contain its name and full name.
    // -----------------------------------------------------------------------
    const TString   strDataSrc(L"DataSrc");
    const TString   strDataSrcClassPath(L"MEng.System.Runtime.DataSrc");

    const TString   strHTTPClient(L"HTTPClient");
    const TString   strHTTPClientClassPath(L"MEng.System.Runtime.HTTPClient");

    const TString   strURL(L"URL");
    const TString   strURLClassPath(L"MEng.System.Runtime.URL");
    const TString   strURLProtosClassPath(L"MEng.System.Runtime.URL.URLProtos");
}


// ---------------------------------------------------------------------------
//  CLASS: TMEngDataSrcVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngDataSrcVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngDataSrcVal::TMEngDataSrcVal( const TString&                strName
                                , const tCIDLib::TCard2         c2Id
                                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_pcdsSrc(nullptr)
{
}

TMEngDataSrcVal::~TMEngDataSrcVal()
{
    try
    {
        delete m_pcdsSrc;
    }

    catch(...)
    {
    }
    m_pcdsSrc = nullptr;
}


// ---------------------------------------------------------------------------
//  TMEngDataSrcVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngDataSrcVal::bDbgFormat(        TTextOutStream&     strmTarget
                            , const TMEngClassInfo&     meciThis
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices   eRadix
                            , const TCIDMacroEngine&    meOwner) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TMEngDataSrcVal: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Return true if we hae a source and it's connected
tCIDLib::TBoolean TMEngDataSrcVal::bConnected() const
{
    return (m_pcdsSrc && m_pcdsSrc->bConnected());
}


// Return true if our data source object is already set up
tCIDLib::TBoolean TMEngDataSrcVal::bReady() const
{
    return (m_pcdsSrc != nullptr);
}


TCIDDataSrc& TMEngDataSrcVal::cdsValue()
{
    CIDAssert(m_pcdsSrc != nullptr, L"Net data source is not set yet");
    return *m_pcdsSrc;
}

const TCIDDataSrc& TMEngDataSrcVal::cdsValue() const
{
    CIDAssert(m_pcdsSrc != nullptr, L"Net data source is not set yet");
    return *m_pcdsSrc;
}


// If we have a source object, clean it up
tCIDLib::TVoid TMEngDataSrcVal::Close()
{
    if (m_pcdsSrc)
    {
        // Terminate it first, to giveit a chance for a clean shutdown
        try
        {
            m_pcdsSrc->Terminate(TTime::enctNowPlusSecs(5), kCIDLib::True);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        catch(...)
        {
        }

        try
        {
            delete m_pcdsSrc;
        }

        catch(...)
        {
        }

        m_pcdsSrc = nullptr;
    }
}


tCIDLib::TVoid
TMEngDataSrcVal::SetSockTCP(const TIPEndPoint& ipepTar, const tCIDLib::TBoolean bSecure)
{
    //
    //  If we have current stuff, clean it up. If it was already terminated this will
    //  just do nothing. We set the close parameter, though it doesn't really matter
    //  since we are going to delete this source anyway. At the CML level, we don't want
    //  to trust users to manage the connection state correctly. So we just create
    //  and destroy them.
    //
    if (m_pcdsSrc)
    {
        try
        {
            m_pcdsSrc->Terminate(TTime::enctNowPlusSecs(5), kCIDLib::True);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        catch(...)
        {
        }
    }

    try
    {
        delete m_pcdsSrc;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    catch(...)
    {
    }

    // Now clear the pointer until we manage to set a new one
    m_pcdsSrc = nullptr;


    TCIDDataSrc* pcdsNew = nullptr;
    try
    {
        // Create ourselves a new source
        if (bSecure)
        {
            //
            //  Create the socket in this case and directly and pass it to the
            //  secure channel data source, who will adopt it.
            //
            tCIDLib::TStrList colALPN;
            pcdsNew = new TCIDSChanClDataSrc
            (
                new TClientStreamSocket(tCIDSock::ESockProtos::TCP, ipepTar)
                , tCIDLib::EAdoptOpts::Adopt
                , ipepTar.strHostName()
                , colALPN
            );
        }
         else
        {
            pcdsNew = new TCIDSockStreamDataSrc(ipepTar);
        }

        // Try to initialize it
        pcdsNew->Initialize(TTime::enctNowPlusSecs(5));

        // It worked, so store it
        m_pcdsSrc = pcdsNew;
    }

    catch(TError& errToCatch)
    {
        // It failed so dclean it up and rethrow
        try
        {
            delete pcdsNew;
        }
        catch(...)
        {
        }

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngDataSrcInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngDataSrcInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngDataSrcInfo::strPath()
{
    return CIDMacroEng_NetClasses::strDataSrcClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngDataSrcInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngDataSrcInfo::TMEngDataSrcInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_NetClasses::strDataSrc
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_DataIsReady(kMacroEng::c2BadId)
    , m_c2MethId_Close(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_GetIsConnected(kMacroEng::c2BadId)
    , m_c2MethId_GetLine(kMacroEng::c2BadId)
    , m_c2MethId_ReadBytes(kMacroEng::c2BadId)
    , m_c2MethId_TCPSetup(kMacroEng::c2BadId)
    , m_c4ErrId_Already(kCIDLib::c4MaxCard)
    , m_c4ErrId_Close(kCIDLib::c4MaxCard)
    , m_c4ErrId_NotReady(kCIDLib::c4MaxCard)
    , m_c4ErrId_Read(kCIDLib::c4MaxCard)
    , m_c4ErrId_Setup(kCIDLib::c4MaxCard)
    , m_c2TypeId_IPEndPoint(kMacroEng::c2BadId)
    , m_c2TypeId_TextXCoder(kMacroEng::c2BadId)
    , m_pmeciErrors(0)
    , m_pmeciLineRes(0)
    , m_pmeciSockProtos(0)
{
    // Force the import of non-intrinsic classes we reference
    bAddClassImport(TMEngStreamSocketInfo::strPath());
    bAddClassImport(TMEngTextXCoderInfo::strPath());
}

TMEngDataSrcInfo::~TMEngDataSrcInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngDataSrcInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngDataSrcInfo::Init(TCIDMacroEngine& meOwner)
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
            , L"DataSrcErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 5
        );
        m_c4ErrId_Already = m_pmeciErrors->c4AddEnumItem(L"Already", TString::strEmpty());
        m_c4ErrId_Close = m_pmeciErrors->c4AddEnumItem(L"Close", TString::strEmpty());
        m_c4ErrId_NotReady = m_pmeciErrors->c4AddEnumItem(L"NotReady", L"The net data source is not ready for use");
        m_c4ErrId_Read = m_pmeciErrors->c4AddEnumItem(L"Read", TString::strEmpty());
        m_c4ErrId_Setup = m_pmeciErrors->c4AddEnumItem(L"Setup", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // An enum wrapper for the C++ line result enum.
    {
        m_pmeciLineRes = new TMEngEnumInfo
        (
            meOwner
            , L"LineReadRes"
            , strClassPath()
            , L"MEng.Enum"
            , 4
        );
        m_pmeciLineRes->c4AddEnumItem(L"GotLine", L"Got a line", tCIDLib::ELineRes::GotLine);
        m_pmeciLineRes->c4AddEnumItem(L"EmptyLine", L"Got empty line" , tCIDLib::ELineRes::EmptyLine);
        m_pmeciLineRes->c4AddEnumItem(L"Timeout", L"Timed out", tCIDLib::ELineRes::Timeout);
        m_pmeciLineRes->c4AddEnumItem(L"Partial", L"Got partial Line", tCIDLib::ELineRes::Partial);
        m_pmeciLineRes->BaseClassInit(meOwner);
        meOwner.c2AddClass(m_pmeciLineRes);
        bAddNestedType(m_pmeciLineRes->strClassPath());
    }

    //
    //  Look up some classes that we need to use.
    //
    m_c2TypeId_IPEndPoint = meOwner.c2FindClassId(TMEngIPEPInfo::strPath());
    m_c2TypeId_TextXCoder = meOwner.c2FindClassId(TMEngTextXCoderInfo::strPath());
    m_pmeciSockProtos = meOwner.pmeciFindAs<TMEngEnumInfo>
    (
        TMEngSocketInfo::strSockProtosPath(), kCIDLib::True
    );


    // The only constructor, which takes a title string
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.DataSrc"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Close this object. It can later be reused
    {
        TMEngMethodInfo methiNew
        (
            L"Close"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Close = c2AddMethodInfo(methiNew);
    }

    // Return true if data is available to be read
    {
        TMEngMethodInfo methiNew
        (
            L"DataIsReady"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"WaitMS", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_DataIsReady = c2AddMethodInfo(methiNew);
    }

    // Return true if the data source is conencted
    {
        TMEngMethodInfo methiNew
        (
            L"GetIsConnected"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetIsConnected = c2AddMethodInfo(methiNew);
    }

    // Get a line, or try to, returning the status
    {
        TMEngMethodInfo methiNew
        (
            L"GetLine"
            , m_pmeciLineRes->c2Id()
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"WaitMS", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"TextCvt", m_c2TypeId_TextXCoder);
        m_c2MethId_GetLine = c2AddMethodInfo(methiNew);
    }

    // Read up to the indicated number of bytes
    {
        TMEngMethodInfo methiNew
        (
            L"ReadBytes"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"MaxBytes", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"WaitMS", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ReadBytes = c2AddMethodInfo(methiNew);
    }

    // Set up this object
    {
        TMEngMethodInfo methiNew
        (
            L"TCPSetup"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"EndPoint", m_c2TypeId_IPEndPoint);
        methiNew.c2AddInParm(L"Secure", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_TCPSetup = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngDataSrcInfo::pmecvMakeStorage(  const   TString&                strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngDataSrcVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngDataSrcInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  These objects have to be set up before they can be used. The HTTP client class
//  accesses the value object's C++ data source object, but he does it at the C++
//  level. So he doesn't call a method of ours which could throw an exception if
//  the value wasn't ready. So we provide this method for him to do that check.
//
tCIDLib::TVoid
TMEngDataSrcInfo::CheckIsReady(          TCIDMacroEngine&    meOwner
                                , const TMEngDataSrcVal&     mecvCheck) const
{
    if (!mecvCheck.bReady())
        ThrowAnErr(meOwner, m_c4ErrId_NotReady);
}


// ---------------------------------------------------------------------------
//  TMEngDataSrcInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngDataSrcInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngDataSrcVal& mecvActual = static_cast<TMEngDataSrcVal&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_Close)
    {
        // Clean up the source object inside our value object
        try
        {
            mecvActual.Close();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            ThrowAnErr(meOwner, m_c4ErrId_Close, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_DataIsReady)
    {
        //
        //  The objects isn't set up, then obviously not. Else, let's get the wait
        //  time and make the call.
        //
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        if (!mecvActual.bReady())
        {
            mecvRet.bValue(kCIDLib::False);
        }
         else
        {
            try
            {
                mecvRet.bValue
                (
                    mecvActual.cdsValue().bDataAvailMS(meOwner.c4StackValAt(c4FirstInd))
                );
            }

            catch(TError& errToCatch)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                ThrowAnErr(meOwner, m_c4ErrId_Read, errToCatch);
            }
        }
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
    }
     else if (c2MethId == m_c2MethId_GetIsConnected)
    {
        // Just a return value
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.bConnected());
    }
     else if (c2MethId == m_c2MethId_GetLine)
    {
        try
        {
            // Get the text converter object, for which there is not a convenience call
            tCIDLib::ELineRes eRes = mecvActual.cdsValue().eGetLine
            (
                meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd).strValue()
                , TTime::enctNowPlusMSs(meOwner.c4StackValAt(c4FirstInd + 1))
                , meOwner.mecvStackAtAs<TMEngTextXCoderVal>(c4FirstInd + 2).tcvtValue()
            );

            //
            //  Convert the line result to the CML enum. They use the same values
            //  so we can just set the C++ value as the ordinal.
            //
            meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1).c4Ordinal
            (
                tCIDLib::c4EnumOrd(eRes)
            );
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            ThrowAnErr(meOwner, m_c4ErrId_Read, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_ReadBytes)
    {
        try
        {
            const tCIDLib::TEncodedTime enctEnd = TTime::enctNowPlusMSs
            (
                meOwner.c4StackValAt(c4FirstInd + 2)
            );
            TMEngMemBufVal& mecvToFill = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd);
            const tCIDLib::TCard4 c4Ret = mecvActual.cdsValue().c4ReadBytes
            (
                mecvToFill.mbufValue()
                , meOwner.c4StackValAt(c4FirstInd + 1)
                , enctEnd
                , kCIDLib::False
            );

            // Give back the count of bytes read
            meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1).c4Value(c4Ret);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            ThrowAnErr(meOwner, m_c4ErrId_Read, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_TCPSetup)
    {
        if (mecvActual.bReady())
            ThrowAnErr(meOwner, m_c4ErrId_Already);

        try
        {
            mecvActual.SetSockTCP
            (
                meOwner.mecvStackAtAs<TMEngIPEPVal>(c4FirstInd).ipepValue()
                , meOwner.bStackValAt(c4FirstInd + 1)
            );
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            ThrowAnErr(meOwner, m_c4ErrId_Setup, errToCatch);
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngDataSrcInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngDataSrcInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            ,       TError&             errCaught)  const
{
    // If verbose log mode, we'll log the C++ level exception
    if (!errCaught.bLogged() && facCIDMacroEng().bLogWarnings())
    {
        errCaught.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errCaught);
    }

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_pmeciErrors->c2Id()
        , strPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , errCaught.strErrText()
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}


tCIDLib::TVoid
TMEngDataSrcInfo::ThrowAnErr(TCIDMacroEngine& meOwner, const tCIDLib::TCard4 c4ToThrow) const
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_pmeciErrors->c2Id()
        , strPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , m_pmeciErrors->strTextValue(c4ToThrow)
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}






// ---------------------------------------------------------------------------
//  CLASS: TMEngHTTPClientVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngHTTPClientVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngHTTPClientVal::TMEngHTTPClientVal( const   TString&                strName
                                        , const tCIDLib::TCard2         c2Id
                                        , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_phttpcValue(new THTTPClient)
{
}

TMEngHTTPClientVal::~TMEngHTTPClientVal()
{
    delete m_phttpcValue;
}


// ---------------------------------------------------------------------------
//  TMEngHTTPClientVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngHTTPClientVal::bDbgFormat(         TTextOutStream&     strmTarget
                                , const TMEngClassInfo&     meciThis
                                , const tCIDMacroEng::EDbgFmts eFormat
                                , const tCIDLib::ERadices   eRadix
                                , const TCIDMacroEngine&    meOwner) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        strmTarget << L"Proxy Server: ";
        if (m_phttpcValue->strProxy().bIsEmpty())
        {
            strmTarget << L"[None]";
        }
         else
        {
            strmTarget  << m_phttpcValue->strProxy()
                        << L':' << m_phttpcValue->ippnProxy();
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TMEngHTTPClientVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
THTTPClient& TMEngHTTPClientVal::httpcValue()
{
    return *m_phttpcValue;
}


tCIDLib::TVoid TMEngHTTPClientVal::Reset()
{
    m_phttpcValue->Reset();
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngHTTPClientInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngHTTPClientInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngHTTPClientInfo::strPath()
{
    return CIDMacroEng_NetClasses::strHTTPClientClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngHTTPClientInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngHTTPClientInfo::TMEngHTTPClientInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_NetClasses::strHTTPClient
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_CreateBasicAuthStr(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_DoSSLGET(kMacroEng::c2BadId)
    , m_c2MethId_DoSSLPOST(kMacroEng::c2BadId)
    , m_c2MethId_EscapeBodyText(kMacroEng::c2BadId)
    , m_c2MethId_ExpandBodyText(kMacroEng::c2BadId)
    , m_c2MethId_FindHdrLine(kMacroEng::c2BadId)
    , m_c2MethId_FindTextEncoding(kMacroEng::c2BadId)
    , m_c2MethId_GetClientMsg(kMacroEng::c2BadId)
    , m_c2MethId_GetSrvReply(kMacroEng::c2BadId)
    , m_c2MethId_ParseAuthReq(kMacroEng::c2BadId)
    , m_c2MethId_ParseMPartMIME(kMacroEng::c2BadId)
    , m_c2MethId_ParseQParms(kMacroEng::c2BadId)
    , m_c2MethId_ParseTextEncoding(kMacroEng::c2BadId)
    , m_c2MethId_SendGET(kMacroEng::c2BadId)
    , m_c2MethId_SendHEAD(kMacroEng::c2BadId)
    , m_c2MethId_SendPOST(kMacroEng::c2BadId)
    , m_c2MethId_SendPOST2(kMacroEng::c2BadId)
    , m_c2MethId_SendPUT(kMacroEng::c2BadId)
    , m_c2MethId_SendRUGET(kMacroEng::c2BadId)
    , m_c2MethId_SendRUHEAD(kMacroEng::c2BadId)
    , m_c2MethId_SendRUPOST(kMacroEng::c2BadId)
    , m_c2MethId_SendRUPOST2(kMacroEng::c2BadId)
    , m_c2MethId_SendRUPUT(kMacroEng::c2BadId)
    , m_c2MethId_SetAddrType(kMacroEng::c2BadId)
    , m_c2MethId_SetAutoAuth(kMacroEng::c2BadId)
    , m_c2MethId_SetProxy(kMacroEng::c2BadId)
    , m_c2MethId_Reset(kMacroEng::c2BadId)
    , m_c2TypeId_KVPair(kMacroEng::c2BadId)
    , m_c2TypeId_CardList(kMacroEng::c2BadId)
    , m_c2TypeId_BufList(kMacroEng::c2BadId)
    , m_c2TypeId_ItemList(kMacroEng::c2BadId)
    , m_c2TypeId_StrList(kMacroEng::c2BadId)
    , m_c4ErrId_Get(kCIDLib::c4MaxCard)
    , m_c4ErrId_Escape(kCIDLib::c4MaxCard)
    , m_c4ErrId_Expand(kCIDLib::c4MaxCard)
    , m_c4ErrId_Parse(kCIDLib::c4MaxCard)
    , m_c4ErrId_Post(kCIDLib::c4MaxCard)
    , m_c4ErrId_RecvMsg(kCIDLib::c4MaxCard)
    , m_c4ErrId_SetProxy(kCIDLib::c4MaxCard)
    , m_pmeciAuthTypes(0)
    , m_pmeciErrors(0)
    , m_pmeciIPAddrTypes(0)
    , m_pmeciNetSrc(0)
    , m_pmeciReadRes(0)
{
    // Force the import of non-intrinsic classes we reference
    bAddClassImport(TMEngVectorInfo::strPath());
    bAddClassImport(TMEngIPEPInfo::strPath());
    bAddClassImport(CIDMacroEng_NetClasses::strURLClassPath);
    bAddClassImport(TMEngKVPairInfo::strPath());
    bAddClassImport(TMEngStreamSocketInfo::strPath());
    bAddClassImport(CIDMacroEng_NetClasses::strDataSrcClassPath);
}

TMEngHTTPClientInfo::~TMEngHTTPClientInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngHTTPClientInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngHTTPClientInfo::Init(TCIDMacroEngine& meOwner)
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
            , L"HTTPClientErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 7
        );
        m_c4ErrId_Get = m_pmeciErrors->c4AddEnumItem(L"GetError", TString::strEmpty());
        m_c4ErrId_Escape = m_pmeciErrors->c4AddEnumItem(L"EscapeError", TString::strEmpty());
        m_c4ErrId_Expand = m_pmeciErrors->c4AddEnumItem(L"ExpandError", TString::strEmpty());
        m_c4ErrId_Parse = m_pmeciErrors->c4AddEnumItem(L"ParseError", TString::strEmpty());
        m_c4ErrId_Post = m_pmeciErrors->c4AddEnumItem(L"PostError", TString::strEmpty());
        m_c4ErrId_RecvMsg = m_pmeciErrors->c4AddEnumItem(L"ReceiveMsg", TString::strEmpty());
        m_c4ErrId_SetProxy = m_pmeciErrors->c4AddEnumItem(L"SetProxy", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    //
    //  An enum to represent the HTTP authentication types
    //
    {
        m_pmeciAuthTypes = new TMEngEnumInfo
        (
            meOwner
            , L"HTTPAuthTypes"
            , strClassPath()
            , L"MEng.Enum"
            , 2
        );
        m_pmeciAuthTypes->c4AddEnumItem(L"Basic", L"Basic Authentication", tCIDNet::EHTTPAuthTypes::Basic);
        m_pmeciAuthTypes->c4AddEnumItem(L"Digest", L"Digest Authentication", tCIDNet::EHTTPAuthTypes::Digest);
        m_pmeciAuthTypes->BaseClassInit(meOwner);
        meOwner.c2AddClass(m_pmeciAuthTypes);
        bAddNestedType(m_pmeciAuthTypes->strClassPath());
    }

    //
    //  An enum to represent the read result from GetClientMsg().
    //
    {
        m_pmeciReadRes = new TMEngEnumInfo
        (
            meOwner
            , L"HTTPReadRes"
            , strClassPath()
            , L"MEng.Enum"
            , 11
        );
        m_pmeciReadRes->c4AddEnumItem(L"Success", L"Successful read", tCIDNet::ENetPReadRes::Success);
        m_pmeciReadRes->c4AddEnumItem(L"BadFirstLn", TString::strEmpty(), tCIDNet::ENetPReadRes::BadFirstLine);
        m_pmeciReadRes->c4AddEnumItem(L"BadHdrLn", TString::strEmpty(), tCIDNet::ENetPReadRes::BadHdrLine);
        m_pmeciReadRes->c4AddEnumItem(L"BadLineCont", TString::strEmpty(), tCIDNet::ENetPReadRes::BadLineCont);
        m_pmeciReadRes->c4AddEnumItem(L"BadProtoVer", TString::strEmpty(), tCIDNet::ENetPReadRes::BadProtoVer);
        m_pmeciReadRes->c4AddEnumItem(L"BufTooSmall", TString::strEmpty(), tCIDNet::ENetPReadRes::BufTooSmall);
        m_pmeciReadRes->c4AddEnumItem(L"ContLen", TString::strEmpty(), tCIDNet::ENetPReadRes::ContLen);
        m_pmeciReadRes->c4AddEnumItem(L"InvalidMsg", TString::strEmpty(), tCIDNet::ENetPReadRes::InvalidMsg);
        m_pmeciReadRes->c4AddEnumItem(L"PartialMsg", TString::strEmpty(), tCIDNet::ENetPReadRes::PartialMsg);
        m_pmeciReadRes->c4AddEnumItem(L"Timeout", TString::strEmpty(), tCIDNet::ENetPReadRes::Timeout);
        m_pmeciReadRes->BaseClassInit(meOwner);
        meOwner.c2AddClass(m_pmeciReadRes);
        bAddNestedType(m_pmeciReadRes->strClassPath());
    }

    // Add some constants for common HTTP return codes
    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kHTTPCode_OK"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kHTTPCode_OK", tCIDMacroEng::EConstTypes::Const, 200)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kHTTPCode_MovedPerm"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kHTTPCode_MovedPerm", tCIDMacroEng::EConstTypes::Const, 301)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kHTTPCode_MovedTemp"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kHTTPCode_MovedTemp", tCIDMacroEng::EConstTypes::Const, 302)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kHTTPCode_SeeOther"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kHTTPCode_SeeOther", tCIDMacroEng::EConstTypes::Const, 303)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kHTTPCode_NotChanged"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kHTTPCode_NotChanged", tCIDMacroEng::EConstTypes::Const, 304)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kHTTPCode_TempRedir"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kHTTPCode_TempRedir", tCIDMacroEng::EConstTypes::Const, 307)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kHTTPCode_PermRedir"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kHTTPCode_PermRedir", tCIDMacroEng::EConstTypes::Const, 308)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kHTTPCode_BadRequest"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kHTTPCode_BadRequest", tCIDMacroEng::EConstTypes::Const, 400)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kHTTPCode_AuthRequired"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kHTTPCode_AuthRequired", tCIDMacroEng::EConstTypes::Const, 401)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kHTTPCode_NotFound"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kHTTPCode_NotFound", tCIDMacroEng::EConstTypes::Const, 404)
        )
    );

    AddLiteral
    (
        new TMEngLiteralVal
        (
            L"kHTTPCode_ServerErr"
            , tCIDMacroEng::EIntrinsics::Card4
            , new TMEngCard4Val(L"kHTTPCode_ServerErr", tCIDMacroEng::EConstTypes::Const, 500)
        )
    );

    //
    //  Look up some classes that we need to use. We need the URL class just
    //  for methods declarations below. We need the type of if the KVPair class
    //  for later. And we need the IP address types class, which is nested within
    //  the IP end point class. This one we get the class info object for since
    //  we need to do translations during runtime.
    //
    const tCIDLib::TCard2 c2URLId
    (
        meOwner.meciFind(CIDMacroEng_NetClasses::strURLClassPath).c2Id()
    );
    m_c2TypeId_KVPair = meOwner.c2FindClassId(TMEngKVPairInfo::strPath());
    m_pmeciIPAddrTypes = meOwner.pmeciFindAs<TMEngEnumInfo>
    (
        TMEngIPEPInfo::strIPAddrTypesPath(), kCIDLib::True
    );
    m_pmeciNetSrc = meOwner.pmeciFindAs<TMEngDataSrcInfo>
    (
        CIDMacroEng_NetClasses::strDataSrcClassPath, kCIDLib::True
    );


    //
    //  We need vectors of a number of things.
    //
    {
        TMEngVectorInfo* pmeciNew = new TMEngVectorInfo
        (
            meOwner
            , L"BufList"
            , strClassPath()
            , TMEngVectorInfo::strPath()
            , tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::MemBuf)
        );
        TJanitor<TMEngVectorInfo> janNewClass(pmeciNew);
        pmeciNew->BaseClassInit(meOwner);
        bAddNestedType(pmeciNew->strClassPath());
        m_c2TypeId_BufList = meOwner.c2AddClass(janNewClass.pobjOrphan());
    }

    {
        TMEngVectorInfo* pmeciNew = new TMEngVectorInfo
        (
            meOwner
            , L"CardList"
            , strClassPath()
            , TMEngVectorInfo::strPath()
            , tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card4)
        );
        TJanitor<TMEngVectorInfo> janNewClass(pmeciNew);
        pmeciNew->BaseClassInit(meOwner);
        bAddNestedType(pmeciNew->strClassPath());
        m_c2TypeId_CardList = meOwner.c2AddClass(janNewClass.pobjOrphan());
    }

    {
        TMEngVectorInfo* pmeciNew = new TMEngVectorInfo
        (
            meOwner
            , L"LinesList"
            , strClassPath()
            , TMEngVectorInfo::strPath()
            , m_c2TypeId_KVPair
        );
        TJanitor<TMEngVectorInfo> janNewClass(pmeciNew);
        pmeciNew->BaseClassInit(meOwner);
        bAddNestedType(pmeciNew->strClassPath());
        m_c2TypeId_ItemList = meOwner.c2AddClass(janNewClass.pobjOrphan());
    }

    {
        TMEngVectorInfo* pmeciNew = new TMEngVectorInfo
        (
            meOwner
            , L"StrList"
            , strClassPath()
            , TMEngVectorInfo::strPath()
            , tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::String)
        );
        TJanitor<TMEngVectorInfo> janNewClass(pmeciNew);
        pmeciNew->BaseClassInit(meOwner);
        bAddNestedType(pmeciNew->strClassPath());
        m_c2TypeId_StrList = meOwner.c2AddClass(janNewClass.pobjOrphan());
    }



    // Create a basic authentication value, given a name and password
    {
        TMEngMethodInfo methiNew
        (
            L"CreateBasicAuthStr"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"UserName", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Password", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_CreateBasicAuthStr = c2AddMethodInfo(methiNew);
    }

    // Default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.HTTPClient"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    //
    //  Do an SSL based GET or POST. They are really the same except for the
    //  the parameter directions.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"DoSSLGET"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"URL", c2URLId);
        methiNew.c2AddInParm(L"Agent", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Accept", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"ContType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Content", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddOutParm(L"ContLen", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"InHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddOutParm(L"OutHdrLines", m_c2TypeId_ItemList);
        m_c2MethId_DoSSLGET = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"DoSSLPOST"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"URL", c2URLId);
        methiNew.c2AddInParm(L"Agent", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Accept", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInOutParm(L"ContType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInOutParm(L"Content", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInOutParm(L"ContLen", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"InHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddOutParm(L"OutHdrLines", m_c2TypeId_ItemList);
        m_c2MethId_DoSSLPOST = c2AddMethodInfo(methiNew);
    }

    // Escape HTML body text from a string to a stream
    {
        TMEngMethodInfo methiNew
        (
            L"EscapeBodyText"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"SrcText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"TarStream", tCIDMacroEng::EIntrinsics::TextOutStream);
        m_c2MethId_EscapeBodyText = c2AddMethodInfo(methiNew);
    }

    // Expand HTML body text from a string to a stream
    {
        TMEngMethodInfo methiNew
        (
            L"ExpandBodyText"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"SrcText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"TarStream", tCIDMacroEng::EIntrinsics::TextOutStream);
        m_c2MethId_ExpandBodyText = c2AddMethodInfo(methiNew);
    }

    // Look up a particular value in a header lines list
    {
        TMEngMethodInfo methiNew
        (
            L"FindHdrLine"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"HdrList", m_c2TypeId_ItemList);
        methiNew.c2AddInParm(L"ToFind", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"ValToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_FindHdrLine = c2AddMethodInfo(methiNew);
    }

    // Look at header lines and content body to try to determine encoding
    {
        TMEngMethodInfo methiNew
        (
            L"FindTextEncoding"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"HdrList", m_c2TypeId_ItemList);
        methiNew.c2AddInParm(L"Content", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"ContLen", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"Encoding", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_FindTextEncoding = c2AddMethodInfo(methiNew);
    }

    //
    //  When reusing the socket and expecting the server to send ongoing unsolicted
    //  replies, this can be used to parse them out. And the other way, to read in
    //  ongoing client type msgs.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"GetClientMsg"
            , m_pmeciReadRes->c2Id()
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"DataSrc", m_pmeciNetSrc->c2Id());
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"ReqType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"HdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddOutParm(L"ContType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"URLText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"ProtoVer", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"Content", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddOutParm(L"ContLen", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetClientMsg = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetSrvReply"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"DataSrc", m_pmeciNetSrc->c2Id());
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"NoContent", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddOutParm(L"RepText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"OutHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddOutParm(L"ContType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Content", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddOutParm(L"ContLen", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetSrvReply = c2AddMethodInfo(methiNew);
    }


    // Break out the authenticate request header line from the server
    {
        TMEngMethodInfo methiNew
        (
            L"ParseAuthReq"
            , m_pmeciAuthTypes->c2Id()
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"SrcText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Realm", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Nonce", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Opaque", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Domain", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Algorithm", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ParseAuthReq = c2AddMethodInfo(methiNew);
    }

    // Parse a multi-part MIME content body
    {
        TMEngMethodInfo methiNew
        (
            L"ParseMultiPartMIME"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"SrcBuf", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"SrcBytes", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ContType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"PartBufs", m_c2TypeId_BufList);
        methiNew.c2AddOutParm(L"Sizes", m_c2TypeId_CardList);
        methiNew.c2AddOutParm(L"ContTypes", m_c2TypeId_StrList);
        methiNew.c2AddOutParm(L"ContDispositions", m_c2TypeId_StrList);
        methiNew.c2AddOutParm(L"ContTransEncodings", m_c2TypeId_StrList);
        m_c2MethId_ParseMPartMIME = c2AddMethodInfo(methiNew);
    }

    // Break out a string of URL encoded query parameters
    {
        TMEngMethodInfo methiNew
        (
            L"ParseQueryParms"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"SrcText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"ToFill", m_c2TypeId_ItemList);
        methiNew.c2AddInParm(L"Expand", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_ParseQParms = c2AddMethodInfo(methiNew);
    }

    // Parse the ContType return and see if it's text and if so what encoding
    {
        TMEngMethodInfo methiNew
        (
            L"ParseTextEncoding"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"SrcText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ParseTextEncoding = c2AddMethodInfo(methiNew);
    }

    // Send a GET request
    {
        TMEngMethodInfo methiNew
        (
            L"SendGET"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"URL", c2URLId);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Agent", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Accept", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"RepText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"OutHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddInOutParm(L"ContType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInOutParm(L"Content", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInOutParm(L"ContLen", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"OutBody", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddInParm(L"InHdrLines", m_c2TypeId_ItemList);
        m_c2MethId_SendGET = c2AddMethodInfo(methiNew);
    }

    // Sends out a HEAD, which never returns a body
    {
        TMEngMethodInfo methiNew
        (
            L"SendHEAD"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"URL", c2URLId);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Agent", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Accept", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"RepText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"OutHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddInOutParm(L"ContType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInOutParm(L"ContLen", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"InHdrLines", m_c2TypeId_ItemList);
        m_c2MethId_SendHEAD = c2AddMethodInfo(methiNew);
    }

    //
    //  Send a POST request, providing the body content as a set of k/v pairs,
    //  which we format out appropriately.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"SendPOST"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"URL", c2URLId);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Agent", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Accept", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"PostVals", m_c2TypeId_ItemList);
        methiNew.c2AddOutParm(L"RepText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"OutHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddOutParm(L"ContType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Content", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddOutParm(L"ContLen", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"InHdrLines", m_c2TypeId_ItemList);
        m_c2MethId_SendPOST = c2AddMethodInfo(methiNew);
    }

    // Same as above, but with pre-created body content instead of a k/v pair list
    {
        TMEngMethodInfo methiNew
        (
            L"SendPOST2"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"URL", c2URLId);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Agent", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Accept", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"RepText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"OutHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddInOutParm(L"ContType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInOutParm(L"Content", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInOutParm(L"ContLen", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"OutBody", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddInParm(L"InHdrLines", m_c2TypeId_ItemList);
        m_c2MethId_SendPOST2 = c2AddMethodInfo(methiNew);
    }

    // Send a PUT
    {
        TMEngMethodInfo methiNew
        (
            L"SendPUT"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"URL", c2URLId);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Agent", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Accept", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"RepText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"OutHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddInOutParm(L"ContType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInOutParm(L"Content", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInOutParm(L"ContLen", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"OutBody", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddInParm(L"InHdrLines", m_c2TypeId_ItemList);
        m_c2MethId_SendPUT = c2AddMethodInfo(methiNew);
    }


    // Send a GET request, with a reusable socket
    {
        TMEngMethodInfo methiNew
        (
            L"SendRUGET"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"URL", c2URLId);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Agent", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Accept", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"RepText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"OutHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddInOutParm(L"ContType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInOutParm(L"Content", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInOutParm(L"ContLen", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"OutBody", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddInParm(L"InHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddInParm(L"DataSrc", m_pmeciNetSrc->c2Id());
        m_c2MethId_SendRUGET = c2AddMethodInfo(methiNew);
    }

    // Sends out a HEAD, with a reusable socket
    {
        TMEngMethodInfo methiNew
        (
            L"SendRUHEAD"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"URL", c2URLId);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Agent", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Accept", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"RepText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"OutHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddInOutParm(L"ContType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInOutParm(L"ContLen", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"InHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddInParm(L"DataSrc", m_pmeciNetSrc->c2Id());
        m_c2MethId_SendRUHEAD = c2AddMethodInfo(methiNew);
    }

    //
    //  Send a POST request, providing the body content as a set of k/v pairs,
    //  which we format out appropriately.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"SendRUPOST"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"URL", c2URLId);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Agent", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Accept", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"PostVals", m_c2TypeId_ItemList);
        methiNew.c2AddOutParm(L"RepText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"OutHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddOutParm(L"ContType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Content", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddOutParm(L"ContLen", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"InHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddInParm(L"DataSrc", m_pmeciNetSrc->c2Id());
        m_c2MethId_SendRUPOST = c2AddMethodInfo(methiNew);
    }

    // Same as above, but with pre-created body content instead of a k/v pair list
    {
        TMEngMethodInfo methiNew
        (
            L"SendRUPOST2"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"URL", c2URLId);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Agent", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Accept", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"RepText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"OutHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddInOutParm(L"ContType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInOutParm(L"Content", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInOutParm(L"ContLen", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"OutBody", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddInParm(L"InHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddInParm(L"DataSrc", m_pmeciNetSrc->c2Id());
        m_c2MethId_SendRUPOST2 = c2AddMethodInfo(methiNew);
    }

    // Send a PUT
    {
        TMEngMethodInfo methiNew
        (
            L"SendRUPUT"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"URL", c2URLId);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Agent", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Accept", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"RepText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"OutHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddInOutParm(L"ContType", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInOutParm(L"Content", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInOutParm(L"ContLen", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"OutBody", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddInParm(L"InHdrLines", m_c2TypeId_ItemList);
        methiNew.c2AddInParm(L"DataSrv", m_pmeciNetSrc->c2Id());
        m_c2MethId_SendRUPUT = c2AddMethodInfo(methiNew);
    }

    // Set the desired address family type
    {
        TMEngMethodInfo methiNew
        (
            L"SetAddrType"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Type", m_pmeciIPAddrTypes->c2Id());
        m_c2MethId_SetAddrType = c2AddMethodInfo(methiNew);
    }

    // Set the auto-authorization flag
    {
        TMEngMethodInfo methiNew
        (
            L"SetAutoAuth"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_SetAutoAuth = c2AddMethodInfo(methiNew);
    }

    // Set the auto-authorization info
    {
        TMEngMethodInfo methiNew
        (
            L"SetAuthInfo"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"UserName", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Password", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_SetAuthInfo = c2AddMethodInfo(methiNew);
    }

    // Set the proxy addr/port to use
    {
        TMEngMethodInfo methiNew
        (
            L"SetProxy"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"SrvAddr", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"PortNum", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SetProxy = c2AddMethodInfo(methiNew);
    }

    // Reset the object to get rid of any partial input or state
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
}


TMEngClassVal*
TMEngHTTPClientInfo::pmecvMakeStorage(  const   TString&                strName
                                        ,       TCIDMacroEngine&        meOwner
                                        , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngHTTPClientVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngHTTPClientInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngHTTPClientInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                    , const TMEngMethodInfo&    methiTarget
                                    ,       TMEngClassVal&      mecvInstance)
{
    TMEngHTTPClientVal& mecvActual = static_cast<TMEngHTTPClientVal&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_CreateBasicAuthStr)
    {
        // The HTTP client has a static helper to do this, just pass in the parms
        THTTPClient::MakeBasicAuthStr
        (
            meOwner.strStackValAt(c4FirstInd)
            , meOwner.strStackValAt(c4FirstInd + 1)
            , meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 2).strValue()
        );
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Reset the value object
        mecvActual.Reset();
    }
     else if ((c2MethId == m_c2MethId_DoSSLGET)
          || (c2MethId == m_c2MethId_DoSSLPOST))
    {
        try
        {
            const TMEngURLVal& mecvURL = meOwner.mecvStackAtAs<TMEngURLVal>(c4FirstInd);
            const TString& strAgent = meOwner.strStackValAt(c4FirstInd + 1);
            const TString& strAccept = meOwner.strStackValAt(c4FirstInd + 2);

            // The In and/or out parms
            TMEngStringVal& mecvContType = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 3);
            TMEngMemBufVal& mecvCont = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 4);
            TMEngCard4Val& mecvContLen = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 5);
            const TMEngVectorVal& mecvInLines
            (
                meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 6)
            );

            //
            //  Load up any incoming header lines. Use +1 for the max, in case of
            //  zero count.
            //
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

            // If a POST, we have an outgoing length
            tCIDLib::TCard4 c4SendLen = 0;
            if (c2MethId == m_c2MethId_DoSSLPOST)
                c4SendLen = mecvContLen.c4Value();

            tCIDLib::TCard4 c4ResCode;
            tCIDLib::TKVPList colOutHdrLines;
            tCIDLib::TCard4 c4ContLen = THTTPClient::c4DoSSLPageOp
            (
                (c2MethId == m_c2MethId_DoSSLGET) ? tCIDNet::ESSLOps::GET
                                                  : tCIDNet::ESSLOps::POST
                , mecvURL.urlValue()
                , strAgent
                , strAccept
                , mecvContType.strValue()
                , c4ResCode
                , mecvCont.mbufValue()
                , colInHdrLines
                , colOutHdrLines
                , c4SendLen
            );

            // Give back the content length. The data is already in the buffer
            mecvContLen.c4Value(c4ContLen);

            // Give back any output header lines
            TMEngVectorVal& mecvOutHdrLines = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 7);
            mecvOutHdrLines.RemoveAll();
            c4Count = colOutHdrLines.c4ElemCount();
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

            // Give back the result code as the return
            TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
            mecvRet.c4Value(c4ResCode);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr
            (
                meOwner
                , (c2MethId == m_c2MethId_DoSSLGET) ? m_c4ErrId_Get : m_c4ErrId_Post
                , errToCatch
            );
        }

    }
     else if ((c2MethId == m_c2MethId_EscapeBodyText)
          ||  (c2MethId == m_c2MethId_ExpandBodyText))
    {
        //
        //  These both take an input string and output text stream, they just
        //  call different underyling C++ methods (with also take those same
        //  parameters.
        //
        const TString& strSrc = meOwner.strStackValAt(c4FirstInd);
        TMEngTextOutStreamVal& mecvStrm
        (
            meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd + 1)
        );

        try
        {
            if (c2MethId == m_c2MethId_EscapeBodyText)
                THTTPClient::EscapeBodyText(strSrc, mecvStrm.strmTarget(meOwner));
            else
                THTTPClient::ExpandBodyText(strSrc, mecvStrm.strmTarget(meOwner));
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr
            (
                meOwner
                , (c2MethId == m_c2MethId_EscapeBodyText) ? m_c4ErrId_Escape
                                                          : m_c4ErrId_Expand
                , errToCatch
            );
        }
    }
     else if (c2MethId == m_c2MethId_FindHdrLine)
    {
        //
        //  Get the vector and search it. There's actually a helper in the
        //  C++ object, but this is so simple it's not worth the overhead
        //  of copying to a C++ collection just to find an element.
        //
        TMEngVectorVal& mecvInLines = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd);
        tCIDLib::TCard4 c4Count = mecvInLines.c4ElemCount();

        const TString& strToFind = meOwner.strStackValAt(c4FirstInd + 1);
        tCIDLib::TCard4 c4Index = 0;
        for (; c4Index < c4Count; c4Index++)
        {
            // Get the element and cast to its real type
            const TMEngKVPairVal& kvalCur
            (
                static_cast<const TMEngKVPairVal&>
                (
                    mecvInLines.mecvAt(meOwner, c4Index)
                )
            );

            if (kvalCur.strKey() == strToFind)
            {
                TMEngStringVal& mecvVal = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 2);
                mecvVal.strValue(kvalCur.strValue());
                break;
            }
        }

        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(c4Index < c4Count);
    }
     else if (c2MethId == m_c2MethId_FindTextEncoding)
    {
        try
        {
            // Load up any incoming header lines to a C++ vector
            const TMEngVectorVal& mecvInLines
            (
                meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd)
            );
            tCIDLib::TCard4 c4Count = mecvInLines.c4ElemCount();

            // Use +1 for max in case of zero count
            tCIDLib::TKVPList colHdrLines(c4Count);
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
                colHdrLines.objAdd
                (
                    TKeyValuePair(mecvCur.strKey(), mecvCur.strValue())
                );
            }

            // The output string to fill if successful
            TMEngStringVal& mecvToFill = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 3);

            // Now make the call and set up the return value
            TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
            mecvRet.bValue
            (
                TNetCoreParser::bFindTextEncoding
                (
                    colHdrLines
                    , meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 1).mbufValue()
                    , meOwner.c4StackValAt(c4FirstInd + 2)
                    , mecvToFill.strValue()
                )
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Parse, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetClientMsg)
    {
        try
        {
            TMEngDataSrcVal& mecvSrc = meOwner.mecvStackAtAs<TMEngDataSrcVal>(c4FirstInd);
            m_pmeciNetSrc->CheckIsReady(meOwner, mecvSrc);

            const tCIDLib::TCard4 c4WaitFor = meOwner.c4StackValAt(c4FirstInd + 1);
            TMEngStringVal& mecvReqText = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 2);
            TMEngVectorVal& mecvOutHdrLines = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 3);
            TMEngStringVal& mecvContType = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 4);
            TMEngStringVal& mecvURL = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 5);
            TMEngCard4Val& mecvProtoVer = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 6);
            TMEngMemBufVal& mecvCont = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 7);
            TMEngCard4Val& mecvContLen = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 8);

            tCIDLib::TKVPList colOutHdrLines;
            tCIDLib::TCard4 c4ContLen = 0;
            tCIDLib::TCard4 c4ProtoVer = 0;
            const tCIDNet::ENetPReadRes eRes = mecvActual.httpcValue().eGetClientMsg
            (
                mecvSrc.cdsValue()
                , TTime::enctNowPlusMSs(c4WaitFor)
                , mecvReqText.strValue()
                , colOutHdrLines
                , mecvContType.strValue()
                , mecvURL.strValue()
                , c4ProtoVer
                , mecvCont.mbufValue()
                , c4ContLen
                , kCIDLib::False
            );

            mecvContLen.c4Value(c4ContLen);
            mecvProtoVer.c4Value(c4ProtoVer);

            TMEngEnumVal& mecvRet = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
            mecvRet.c4Ordinal(m_pmeciReadRes->c4FromMapValue(eRes));

            // If it worked
            if (eRes == tCIDNet::ENetPReadRes::Success)
            {
                // Get the outgoing header lines to CML format
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
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_RecvMsg, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetSrvReply)
    {
        try
        {
            TMEngDataSrcVal& mecvSrc = meOwner.mecvStackAtAs<TMEngDataSrcVal>(c4FirstInd);
            m_pmeciNetSrc->CheckIsReady(meOwner, mecvSrc);

            const tCIDLib::TCard4 c4WaitFor = meOwner.c4StackValAt(c4FirstInd + 1);
            const tCIDLib::TBoolean bNoContent = meOwner.bStackValAt(c4FirstInd + 2);

            // Output parms
            TMEngStringVal& mecvRepText = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 3);
            TMEngVectorVal& mecvOutHdrLines = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 4);
            TMEngStringVal& mecvContType = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 5);
            TMEngMemBufVal& mecvCont = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 6);
            TMEngCard4Val& mecvContLen = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 7);

            tCIDNet::EHTTPCodes eCodeType;
            TString strRepType;
            tCIDLib::TKVPList colOutHdrLines;
            tCIDLib::TCard4 c4ContLen = 0;
            tCIDLib::TCard4 c4Ret = mecvActual.httpcValue().c4GetSrvReply
            (
                mecvSrc.cdsValue()
                , TTime::enctNowPlusMSs(c4WaitFor)
                , bNoContent
                , strRepType
                , eCodeType
                , mecvRepText.strValue()
                , colOutHdrLines
                , mecvContType.strValue()
                , mecvCont.mbufValue()
                , c4ContLen
            );

            mecvContLen.c4Value(c4ContLen);
            TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
            mecvRet.c4Value(c4Ret);

            //
            //  We need to get the header lines out of our local vector and
            //  into the CML level vector.
            //
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

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_RecvMsg, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_ParseMPartMIME)
    {
        // We need to do the call and get the stuff into locals first
        try
        {
            TVector<THeapBuf>   colParts;
            tCIDLib::TCardList  fcolSizes;
            tCIDLib::TStrList   colTypes;
            tCIDLib::TStrList   colDispos;
            tCIDLib::TStrList   colEncodings;

            TMEngMemBufVal& mecvSrcBuf = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd);
            facCIDNet().ParseMultiPartMIME
            (
                mecvSrcBuf.mbufValue()
                , meOwner.c4StackValAt(c4FirstInd + 1)
                , meOwner.strStackValAt(c4FirstInd + 2)
                , colParts
                , fcolSizes
                , colTypes
                , colDispos
                , colEncodings
            );

            //
            //  Now we have to load up our CML level vectors from the local
            //  data.
            //
            TMEngVectorVal& mecvParts = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 3);
            TMEngVectorVal& mecvSizes = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 4);
            TMEngVectorVal& mecvTypes = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 5);
            TMEngVectorVal& mecvDispos = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 6);
            TMEngVectorVal& mecvEncodings = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 7);

            tCIDLib::TCard4 c4Count;

            // Do the parts
            c4Count = colParts.c4ElemCount();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                TMEngMemBufVal* pmecvNew = new TMEngMemBufVal
                (
                    TString::strEmpty()
                    , tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::MemBuf)
                    , tCIDMacroEng::EConstTypes::NonConst
                );
                pmecvNew->mbufValue() = colParts[c4Index];
                mecvParts.AddObject(pmecvNew);
            }

            // Do the sizes
            c4Count = fcolSizes.c4ElemCount();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                mecvSizes.AddObject
                (
                    new TMEngCard4Val
                    (
                        TString::strEmpty(), tCIDMacroEng::EConstTypes::NonConst, fcolSizes[c4Index]
                    )
                );
            }

            // Do the content types
            c4Count = colTypes.c4ElemCount();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                mecvTypes.AddObject
                (
                    new TMEngStringVal
                    (
                        TString::strEmpty(), tCIDMacroEng::EConstTypes::NonConst, colTypes[c4Index]
                    )
                );
            }

            // Do the content dispositions
            c4Count = colDispos.c4ElemCount();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                mecvDispos.AddObject
                (
                    new TMEngStringVal
                    (
                        TString::strEmpty(), tCIDMacroEng::EConstTypes::NonConst, colDispos[c4Index]
                    )
                );
            }

            // Do the encodings
            c4Count = colEncodings.c4ElemCount();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                mecvEncodings.AddObject
                (
                    new TMEngStringVal
                    (
                        TString::strEmpty(), tCIDMacroEng::EConstTypes::NonConst, colEncodings[c4Index]
                    )
                );
            }
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Parse, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_ParseQParms)
    {
        try
        {
            // We have to use a temp local C++ collection first
            tCIDLib::TKVPList colItems;
            tCIDLib::TCard4 c4Ret = TURL::c4ParseQueryParms
            (
                meOwner.strStackValAt(c4FirstInd)
                , colItems
                , meOwner.bStackValAt(c4FirstInd + 2)
                , TURL::EExpTypes::Query
            );

            // Now copy over to the CML level collection
            TMEngVectorVal& mecvItems = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 1);
            mecvItems.RemoveAll();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Ret; c4Index++)
            {
                const TKeyValuePair& kvalCur = colItems[c4Index];
                mecvItems.AddObject
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

            // Give back the count return value
            TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
            mecvRet.c4Value(c4Ret);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Parse, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_ParseAuthReq)
    {
        try
        {
            tCIDNet::EHTTPAuthTypes eType;
            THTTPClient::ParseAuthReq
            (
                meOwner.strStackValAt(c4FirstInd)
                , eType
                , meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 1).strValue()
                , meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 2).strValue()
                , meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 3).strValue()
                , meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 4).strValue()
                , meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 5).strValue()
            );

            // Store the authorization type
            TMEngEnumVal& mecvRet = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
            mecvRet.c4Ordinal(m_pmeciAuthTypes->c4FromMapValue(eType));
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Parse, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_ParseTextEncoding)
    {
        try
        {
            TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
            TMEngStringVal& mecvToFill = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 1);
            mecvRet.bValue
            (
                mecvActual.httpcValue().bParseTextEncoding
                (
                    meOwner.strStackValAt(c4FirstInd)
                    , mecvToFill.strValue()
                )
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Parse, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_Reset)
    {
        mecvActual.httpcValue().Reset();
    }
     else if ((c2MethId == m_c2MethId_SendGET)
          ||  (c2MethId == m_c2MethId_SendRUGET)
          ||  (c2MethId == m_c2MethId_SendPUT)
          ||  (c2MethId == m_c2MethId_SendRUPUT))
    {
        // These two are the same but for the method called
        try
        {
            // Input parms
            const TMEngURLVal& mecvURL = meOwner.mecvStackAtAs<TMEngURLVal>(c4FirstInd);
            const tCIDLib::TCard4 c4WaitFor = meOwner.c4StackValAt(c4FirstInd + 1);
            const TString& strAgent = meOwner.strStackValAt(c4FirstInd + 2);
            const TString& strAccept = meOwner.strStackValAt(c4FirstInd + 3);

            // Output parms
            TMEngStringVal& mecvRepText = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 4);
            TMEngVectorVal& mecvOutHdrLines = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 5);

            // These are in/out, though possibly only used for out
            TMEngStringVal& mecvContType = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 6);
            TMEngMemBufVal& mecvCont = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 7);
            TMEngCard4Val& mecvContLen = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 8);

            // And this tells us if we have outgoing content
            const tCIDLib::TBoolean bOutBody = meOwner.bStackValAt(c4FirstInd + 9);

            // Load up any incoming header lines
            const TMEngVectorVal& mecvInLines
            (
                meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 10)
            );
            tCIDLib::TCard4 c4Count = mecvInLines.c4ElemCount();

            // Use +1 for max in case of zero count
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

            // If outgoing body content, then set the content length
            tCIDLib::TCard4 c4ContLen = 0;
            if (bOutBody)
                c4ContLen = mecvContLen.c4Value();

            //
            //  If there's an incoming data src , get a pointer to it, else pass in
            //  zero and it will create one for us.
            //
            TCIDDataSrc* pcdsToUse = 0;
            if ((c2MethId == m_c2MethId_SendRUGET) || (c2MethId == m_c2MethId_SendRUPUT))
            {
                TMEngDataSrcVal& mecvSrc
                (
                    meOwner.mecvStackAtAs<TMEngDataSrcVal>(c4FirstInd + 11)
                );
                m_pmeciNetSrc->CheckIsReady(meOwner, mecvSrc);
                pcdsToUse = &mecvSrc.cdsValue();
            }

            tCIDNet::EHTTPCodes eCodeType;
            tCIDLib::TKVPList colOutHdrLines;
            tCIDLib::TCard4 c4Ret;

            if ((c2MethId == m_c2MethId_SendGET) || (c2MethId == m_c2MethId_SendRUGET))
            {
                c4Ret = mecvActual.httpcValue().c4SendGet
                (
                    pcdsToUse
                    , mecvURL.urlValue()
                    , TTime::enctNowPlusMSs(c4WaitFor)
                    , strAgent
                    , strAccept
                    , eCodeType
                    , mecvRepText.strValue()
                    , colOutHdrLines
                    , mecvContType.strValue()
                    , mecvCont.mbufValue()
                    , c4ContLen
                    , bOutBody
                    , colInHdrLines
                );
            }
             else
            {
                c4Ret = mecvActual.httpcValue().c4SendPut
                (
                    pcdsToUse
                    , mecvURL.urlValue()
                    , TTime::enctNowPlusMSs(c4WaitFor)
                    , strAgent
                    , strAccept
                    , eCodeType
                    , mecvRepText.strValue()
                    , colOutHdrLines
                    , mecvContType.strValue()
                    , mecvCont.mbufValue()
                    , c4ContLen
                    , bOutBody
                    , colInHdrLines
                );
            }

            mecvContLen.c4Value(c4ContLen);
            TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
            mecvRet.c4Value(c4Ret);

            //
            //  We need to get the header lines out of our local vector and
            //  into the CML level vector.
            //
            mecvOutHdrLines.RemoveAll();
            c4Count = colOutHdrLines.c4ElemCount();
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

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Get, errToCatch);
        }
    }
     else if ((c2MethId == m_c2MethId_SendHEAD)
          ||  (c2MethId == m_c2MethId_SendRUHEAD))
    {
        try
        {
            // Input parms
            const TMEngURLVal& mecvURL = meOwner.mecvStackAtAs<TMEngURLVal>(c4FirstInd);
            const tCIDLib::TCard4 c4WaitFor = meOwner.c4StackValAt(c4FirstInd + 1);
            const TString& strAgent = meOwner.strStackValAt(c4FirstInd + 2);
            const TString& strAccept = meOwner.strStackValAt(c4FirstInd + 3);

            // Output parms
            TMEngStringVal& mecvRepText = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 4);
            TMEngVectorVal& mecvOutHdrLines = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 5);

            // These are in/out, though possibly only used for out
            TMEngStringVal& mecvContType = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 6);
            TMEngCard4Val& mecvContLen = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 7);

            // Load up any incoming header lines
            const TMEngVectorVal& mecvInLines
            (
                meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 8)
            );
            tCIDLib::TCard4 c4Count = mecvInLines.c4ElemCount();

            // Use +1 for max in case of zero count
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

            //
            //  If there's an incoming socket, get a pointer to it, else pass in
            //  zero and it will create one for us.
            //
            TCIDDataSrc* pcdsToUse = 0;
            if (c2MethId == m_c2MethId_SendRUHEAD)
            {
                TMEngDataSrcVal& mecvSrc
                (
                    meOwner.mecvStackAtAs<TMEngDataSrcVal>(c4FirstInd + 11)
                );
                m_pmeciNetSrc->CheckIsReady(meOwner, mecvSrc);
                pcdsToUse = &mecvSrc.cdsValue();
            }

            tCIDNet::EHTTPCodes eCodeType;
            tCIDLib::TCard4 c4ContLen;
            tCIDLib::TKVPList colOutHdrLines;
            tCIDLib::TCard4 c4Ret = mecvActual.httpcValue().c4SendHead
            (
                pcdsToUse
                , mecvURL.urlValue()
                , TTime::enctNowPlusMSs(c4WaitFor)
                , strAgent
                , strAccept
                , eCodeType
                , mecvRepText.strValue()
                , colOutHdrLines
                , mecvContType.strValue()
                , c4ContLen
                , colInHdrLines
            );

            mecvContLen.c4Value(c4ContLen);
            TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
            mecvRet.c4Value(c4Ret);

            //
            //  We need to get the header lines out of our local vector and
            //  into the CML level vector.
            //
            mecvOutHdrLines.RemoveAll();
            c4Count = colOutHdrLines.c4ElemCount();
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

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Get, errToCatch);
        }
    }
     else if ((c2MethId == m_c2MethId_SendPOST)
          ||  (c2MethId == m_c2MethId_SendRUPOST))
    {
        try
        {
            // Input parms
            const TMEngURLVal& mecvURL = meOwner.mecvStackAtAs<TMEngURLVal>(c4FirstInd);
            const tCIDLib::TCard4 c4WaitFor = meOwner.c4StackValAt(c4FirstInd + 1);
            const TString& strAgent = meOwner.strStackValAt(c4FirstInd + 2);
            const TString& strAccept = meOwner.strStackValAt(c4FirstInd + 3);

            //
            //  Get the incoming k/v pairs that represent the post values out of
            //  the CML level vector and into a local one.
            //
            tCIDLib::TKVPList colPostVals;
            TMEngVectorVal& mecvVals = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 4);
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
                meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 10)
            );
            tCIDLib::TCard4 c4Count = mecvInLines.c4ElemCount();

            // Use +1 for max in case of zer count
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

            // Output parms
            TMEngStringVal& mecvRepText = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 5);
            TMEngVectorVal& mecvOutHdrLines = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 6);
            TMEngStringVal& mecvContType = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 7);
            TMEngMemBufVal& mecvCont = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 8);
            TMEngCard4Val& mecvContLen = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 9);

            //
            //  If there's an incoming socket, get a pointer to it, else pass in
            //  zero and it will create one for us.
            //
            TCIDDataSrc* pcdsToUse = 0;
            if (c2MethId == m_c2MethId_SendRUPOST)
            {
                TMEngDataSrcVal& mecvSrc
                (
                    meOwner.mecvStackAtAs<TMEngDataSrcVal>(c4FirstInd + 11)
                );
                m_pmeciNetSrc->CheckIsReady(meOwner, mecvSrc);
                pcdsToUse = &mecvSrc.cdsValue();
            }

            tCIDNet::EHTTPCodes eCodeType;
            tCIDLib::TKVPList colOutHdrLines;
            tCIDLib::TCard4 c4ContLen = 0;
            tCIDLib::TCard4 c4Ret = mecvActual.httpcValue().c4SendPost
            (
                pcdsToUse
                , mecvURL.urlValue()
                , TTime::enctNowPlusMSs(c4WaitFor)
                , strAgent
                , strAccept
                , colPostVals
                , eCodeType
                , mecvRepText.strValue()
                , colOutHdrLines
                , mecvContType.strValue()
                , mecvCont.mbufValue()
                , c4ContLen
                , colInHdrLines
            );

            mecvContLen.c4Value(c4ContLen);
            TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
            mecvRet.c4Value(c4Ret);

            //
            //  We need to get the header lines out of our local vector and
            //  into the CML level vector.
            //
            mecvOutHdrLines.RemoveAll();

            c4Count = colOutHdrLines.c4ElemCount();
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

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Post, errToCatch);
        }
    }
     else if ((c2MethId == m_c2MethId_SendPOST2)
          ||  (c2MethId == m_c2MethId_SendRUPOST2))
    {
        try
        {
            // Input parms
            const TMEngURLVal& mecvURL = meOwner.mecvStackAtAs<TMEngURLVal>(c4FirstInd);
            const tCIDLib::TCard4 c4WaitFor = meOwner.c4StackValAt(c4FirstInd + 1);
            const TString& strAgent = meOwner.strStackValAt(c4FirstInd + 2);
            const TString& strAccept = meOwner.strStackValAt(c4FirstInd + 3);

            // Output parms
            TMEngStringVal& mecvRepText = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 4);
            TMEngVectorVal& mecvOutHdrLines = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 5);

            // These are in/out, though possibly only used for out
            TMEngStringVal& mecvContType = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 6);
            TMEngMemBufVal& mecvCont = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 7);
            TMEngCard4Val& mecvContLen = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 8);

            // And this tells us if we have outgoing content
            const tCIDLib::TBoolean bOutBody = meOwner.bStackValAt(c4FirstInd + 9);

            // And the outgong header lines
            const TMEngVectorVal& mecvInLines
            (
                meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 10)
            );
            tCIDLib::TCard4 c4Count = mecvInLines.c4ElemCount();

            // Use +1 for max in case of zero count
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

            //
            //  Get a copy of the incoming content length to pass in. If the
            //  bOutBody flag is false, it will be ignored so it doesn't matter
            //  if there's really no outgoing, but be a good boy and only pass
            //  non-zero if we need to.
            //
            //  For the content buffer and the content type string, we are
            //  passing the passed values directly, so they already are going
            //  in with whatever value they had on the way in.
            //
            tCIDLib::TCard4 c4ContLen = 0;
            if (bOutBody)
                c4ContLen = mecvContLen.c4Value();

            //
            //  If there's an incoming socket, get a pointer to it, else pass in
            //  zero and it will create one for us.
            //
            TCIDDataSrc* pcdsToUse = 0;
            if (c2MethId == m_c2MethId_SendRUPOST2)
            {
                TMEngDataSrcVal& mecvSrc
                (
                    meOwner.mecvStackAtAs<TMEngDataSrcVal>(c4FirstInd + 11)
                );
                m_pmeciNetSrc->CheckIsReady(meOwner, mecvSrc);
                pcdsToUse = &mecvSrc.cdsValue();
            }

            tCIDNet::EHTTPCodes eCodeType;
            tCIDLib::TKVPList colOutHdrLines;
            tCIDLib::TCard4 c4Ret = mecvActual.httpcValue().c4SendPost
            (
                pcdsToUse
                , mecvURL.urlValue()
                , TTime::enctNowPlusMSs(c4WaitFor)
                , strAgent
                , strAccept
                , eCodeType
                , mecvRepText.strValue()
                , colOutHdrLines
                , mecvContType.strValue()
                , mecvCont.mbufValue()
                , c4ContLen
                , bOutBody
                , colInHdrLines
            );

            mecvContLen.c4Value(c4ContLen);
            TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
            mecvRet.c4Value(c4Ret);

            //
            //  We need to get the header lines out of our local vector and
            //  into the CML level vector.
            //
            mecvOutHdrLines.RemoveAll();
            c4Count = colOutHdrLines.c4ElemCount();
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

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Post, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_SetAddrType)
    {
        // Translate the value
        const TMEngEnumVal& mecvType = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);

        tCIDSock::EAddrTypes eType = tCIDSock::EAddrTypes
        (
            m_pmeciIPAddrTypes->c4MapValue(mecvType)
        );
        mecvActual.httpcValue().eAddrType(eType);
    }
     else if (c2MethId == m_c2MethId_SetAutoAuth)
    {
        mecvActual.httpcValue().bAutoAuth(meOwner.bStackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_SetAuthInfo)
    {
        mecvActual.httpcValue().SetAuthInfo
        (
            meOwner.strStackValAt(c4FirstInd)
            , meOwner.strStackValAt(c4FirstInd + 1)
        );
    }
     else if (c2MethId == m_c2MethId_SetProxy)
    {
        try
        {
            mecvActual.httpcValue().SetProxy
            (
                meOwner.strStackValAt(c4FirstInd)
                , meOwner.c4StackValAt(c4FirstInd + 1)
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_SetProxy, errToCatch);
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngHTTPClientInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngHTTPClientInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                                , const tCIDLib::TCard4     c4ToThrow
                                ,       TError&             errCaught)
{
    // If verbose log mode, we'll log the C++ level exception
    if (!errCaught.bLogged() && facCIDMacroEng().bLogWarnings())
    {
        errCaught.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errCaught);
    }

    // Set the exception info on the engine
    TString strText(errCaught.strErrText(), 128UL);
    if (errCaught.bHasAuxText())
    {
        strText.Append(L". ");
        strText.Append(errCaught.strAuxText());
    }
    meOwner.SetException
    (
        m_pmeciErrors->c2Id()
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngURLVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngURLVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngURLVal::TMEngURLVal(const  TString&                strName
                        , const tCIDLib::TCard2         c2Id
                        , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_purlValue(new TURL)
{
}

TMEngURLVal::TMEngURLVal(const  TString&                strName
                        , const tCIDLib::TCard2         c2Id
                        , const tCIDMacroEng::EConstTypes  eConst
                        , const TURL&                   urlInit) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_purlValue(new TURL(urlInit))
{
}

TMEngURLVal::~TMEngURLVal()
{
    delete m_purlValue;
}


// ---------------------------------------------------------------------------
//  TMEngURLVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngURLVal::bDbgFormat(        TTextOutStream&     strmTar
                        , const TMEngClassInfo&     meciThis
                        , const tCIDMacroEng::EDbgFmts eFormat
                        , const tCIDLib::ERadices   eRadix
                        , const TCIDMacroEngine&    meOwner) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Short)
    {
        strmTar << *m_purlValue;
    }
     else if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        TString             strProto;
        TString             strUser;
        TString             strPassword;
        TString             strHost;
        tCIDLib::TIPPortNum ippnHost;
        TString             strPath;
        TString             strParams;
        TString             strFragment;
        TString             strQuery;
        m_purlValue->QueryEncodedForm
        (
            strProto
            , strUser
            , strPassword
            , strHost
            , ippnHost
            , strPath
            , strParams
            , strFragment
            , strQuery
        );

        strmTar << L"Proto: " << strProto << kCIDLib::NewLn
                << L"User: " << strUser << kCIDLib::NewLn
                << L"Password: " << strPassword << kCIDLib::NewLn
                << L"Host: " << strHost << kCIDLib::NewLn
                << L"Port: " << ippnHost << kCIDLib::NewLn
                << L"Path: " << strPath << kCIDLib::NewLn
                << L"Params: " << strParams << kCIDLib::NewLn
                << L"Fragment: " << strFragment << kCIDLib::NewLn
                << L"Query: " << strQuery<< kCIDLib::NewLn;
    }
    return kCIDLib::True;
}


tCIDLib::TVoid
TMEngURLVal::CopyFrom(  const   TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    *m_purlValue = *static_cast<const TMEngURLVal&>(mecvToCopy).m_purlValue;
}


// ---------------------------------------------------------------------------
//  TMEngURLVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
TURL& TMEngURLVal::urlValue()
{
    return *m_purlValue;
}

const TURL& TMEngURLVal::urlValue() const
{
    return *m_purlValue;
}


tCIDLib::TVoid TMEngURLVal::Reset()
{
    m_purlValue->Clear();
}


tCIDLib::TVoid TMEngURLVal::Set(const TURL& urlToSet)
{
    *m_purlValue = urlToSet;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngURLInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngURLInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngURLInfo::strPath()
{
    return CIDMacroEng_NetClasses::strURLClassPath;
}

const TString& TMEngURLInfo::strProtosPath()
{
    return CIDMacroEng_NetClasses::strURLProtosClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngURLInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngURLInfo::TMEngURLInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_NetClasses::strURL
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2MethId_BuildURL(kMacroEng::c2BadId)
    , m_c2MethId_Clear(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Encode(kMacroEng::c2BadId)
    , m_c2MethId_Equal(kMacroEng::c2BadId)
    , m_c2MethId_Expand(kMacroEng::c2BadId)
    , m_c2MethId_GetEncoded(kMacroEng::c2BadId)
    , m_c2MethId_GetEncodedParts(kMacroEng::c2BadId)
    , m_c2MethId_GetFragment(kMacroEng::c2BadId)
    , m_c2MethId_GetFullText(kMacroEng::c2BadId)
    , m_c2MethId_GetHost(kMacroEng::c2BadId)
    , m_c2MethId_GetIPEndPoint(kMacroEng::c2BadId)
    , m_c2MethId_GetParams(kMacroEng::c2BadId)
    , m_c2MethId_GetPassword(kMacroEng::c2BadId)
    , m_c2MethId_GetPath(kMacroEng::c2BadId)
    , m_c2MethId_GetPort(kMacroEng::c2BadId)
    , m_c2MethId_GetProto(kMacroEng::c2BadId)
    , m_c2MethId_GetQParms(kMacroEng::c2BadId)
    , m_c2MethId_GetQParmsFmt(kMacroEng::c2BadId)
    , m_c2MethId_GetResource(kMacroEng::c2BadId)
    , m_c2MethId_GetUserName(kMacroEng::c2BadId)
    , m_c2MethId_Set(kMacroEng::c2BadId)
    , m_c2MethId_Set2(kMacroEng::c2BadId)
    , m_c2MethId_Set3(kMacroEng::c2BadId)
    , m_c2MethId_Set4(kMacroEng::c2BadId)
    , m_c2TypeId_Comps(kMacroEng::c2BadId)
    , m_c2TypeId_Errors(kMacroEng::c2BadId)
    , m_c2TypeId_ItemList(kMacroEng::c2BadId)
    , m_c2TypeId_IPEP(kMacroEng::c2BadId)
    , m_c2TypeId_KVPair(kMacroEng::c2BadId)
    , m_c2TypeId_Protos(kMacroEng::c2BadId)
    , m_c4ErrId_Encoding(kCIDLib::c4MaxCard)
    , m_c4ErrId_NoIPEP(kMacroEng::c2BadId)
    , m_c4ErrId_Query(kCIDLib::c4MaxCard)
    , m_c4ErrId_Set(kCIDLib::c4MaxCard)
    , m_pmeciIPAddrTypes(0)
    , m_pmeciComps(0)
    , m_pmeciErrors(0)
    , m_pmeciProtos(0)
{
    // Add imports for any non-intrinsic classes we use in our signatures
    bAddClassImport(TMEngBaseInfoInfo::strPath());
    bAddClassImport(TMEngKVPairInfo::strPath());
    bAddClassImport(TMEngIPEPInfo::strPath());
}

TMEngURLInfo::~TMEngURLInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngURLInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngURLInfo::Init(TCIDMacroEngine& meOwner)
{
    // An enum for the URL component areas used in escaping
    {
        m_pmeciComps = new TMEngEnumInfo
        (
            meOwner
            , L"URLComps"
            , strClassPath()
            , L"MEng.Enum"
            , 3
        );
        m_pmeciComps->c4AddEnumItem(L"Path", L"Path", TURL::EExpTypes::Path);
        m_pmeciComps->c4AddEnumItem(L"Query", L"Query", TURL::EExpTypes::Query);
        m_pmeciComps->c4AddEnumItem(L"Fragment", L"Fragment", TURL::EExpTypes::Fragment);
        m_pmeciComps->BaseClassInit(meOwner);
        m_c2TypeId_Comps = meOwner.c2AddClass(m_pmeciComps);
        bAddNestedType(m_pmeciComps->strClassPath());
    }

    //
    //  An enum to represent the exceptions that this class throws. Note
    //  that we just use the text in the C++ exception in some cases, so there
    //  is no text for the enum value for those.
    //
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"URLErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 4
        );
        m_c4ErrId_Encoding = m_pmeciErrors->c4AddEnumItem(L"EncodingError", TString::strEmpty());
        m_c4ErrId_NoIPEP  = m_pmeciErrors->c4AddEnumItem(L"NoIPEP", TString::strEmpty());
        m_c4ErrId_Query = m_pmeciErrors->c4AddEnumItem(L"QueryError", TString::strEmpty());
        m_c4ErrId_Set = m_pmeciErrors->c4AddEnumItem(L"SetError", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2TypeId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // An enum for the protocols this class recognizes
    {
        m_pmeciProtos = new TMEngEnumInfo
        (
            meOwner
            , L"URLProtos"
            , strClassPath()
            , L"MEng.Enum"
            , 10
        );
        m_pmeciProtos->c4AddEnumItem(L"None", L"None", tCIDSock::EProtos::None);
        m_pmeciProtos->c4AddEnumItem(L"File", L"file", tCIDSock::EProtos::File);
        m_pmeciProtos->c4AddEnumItem(L"HTTP", L"http", tCIDSock::EProtos::HTTP);
        m_pmeciProtos->c4AddEnumItem(L"FTP", L"ftp", tCIDSock::EProtos::FTP);
        m_pmeciProtos->c4AddEnumItem(L"MailTo", L"mailto", tCIDSock::EProtos::MailTo);
        m_pmeciProtos->c4AddEnumItem(L"News", L"news", tCIDSock::EProtos::News);
        m_pmeciProtos->c4AddEnumItem(L"HTTPS", L"https", tCIDSock::EProtos::HTTPS);
        m_pmeciProtos->c4AddEnumItem(L"SIP", L"sip", tCIDSock::EProtos::SIP);
        m_pmeciProtos->c4AddEnumItem(L"WS", L"ws", tCIDSock::EProtos::WS);
        m_pmeciProtos->c4AddEnumItem(L"WSS", L"wss", tCIDSock::EProtos::WSS);
        m_pmeciProtos->BaseClassInit(meOwner);
        m_c2TypeId_Protos = meOwner.c2AddClass(m_pmeciProtos);
        bAddNestedType(m_pmeciProtos->strClassPath());
    }

    // Look up some types we need
    m_c2TypeId_IPEP = meOwner.c2FindClassId(TMEngIPEPInfo::strPath());
    m_c2TypeId_KVPair = meOwner.c2FindClassId(TMEngKVPairInfo::strPath());
    m_pmeciIPAddrTypes = meOwner.pmeciFindAs<TMEngEnumInfo>
    (
        TMEngIPEPInfo::strIPAddrTypesPath(), kCIDLib::True
    );
    const tCIDLib::TCard2 c2FQTypes = meOwner.c2FindClassId
    (
        TMEngBaseInfoInfo::strFQTypesPath()
    );

    //
    //  Create a vector of kvpairs, since we have parameters of that type
    //  below. Any equiv type can be actually be passed in.
    //
    {
        TMEngVectorInfo* pmeciNew = new TMEngVectorInfo
        (
            meOwner
            , L"QPairs"
            , strClassPath()
            , TMEngVectorInfo::strPath()
            , m_c2TypeId_KVPair
        );
        TJanitor<TMEngVectorInfo> janNewClass(pmeciNew);
        pmeciNew->BaseClassInit(meOwner);
        bAddNestedType(pmeciNew->strClassPath());
        m_c2TypeId_ItemList = meOwner.c2AddClass(janNewClass.pobjOrphan());
    }

    //
    //  Build up a URL out of constituent parts. We have two. The second
    //  takes a list of key/value pairs for query parameters.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"BuildURL"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Protocol", m_c2TypeId_Protos);
        methiNew.c2AddInParm(L"User", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Password", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Host", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"PortNum", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Path", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Params", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Fragment", tCIDMacroEng::EIntrinsics::String);

        m_c2MethId_BuildURL = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"BuildURL2"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Protocol", m_c2TypeId_Protos);
        methiNew.c2AddInParm(L"User", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Password", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Host", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"PortNum", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Path", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Params", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Fragment", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"QParms", m_c2TypeId_ItemList);

        m_c2MethId_BuildURL2 = c2AddMethodInfo(methiNew);
    }


    // Clear the URL
    {
        TMEngMethodInfo methiNew
        (
            L"Clear"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Clear = c2AddMethodInfo(methiNew);
    }

    // Default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.URL"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Encode a string based on component type
    {
        TMEngMethodInfo methiNew
        (
            L"Encode"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToEncode", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Component", m_c2TypeId_Comps);
        methiNew.c2AddInParm(L"Append", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_Encode = c2AddMethodInfo(methiNew);
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
        methiNew.c2AddInParm(L"Val", c2Id());
        m_c2MethId_Equal = c2AddMethodInfo(methiNew);
    }

    // Expand a string based on component type
    {
        TMEngMethodInfo methiNew
        (
            L"Expand"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToExpand", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Component", m_c2TypeId_Comps);
        methiNew.c2AddInParm(L"Append", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_Expand = c2AddMethodInfo(methiNew);
    }

    // Get the full URL encoded
    {
        TMEngMethodInfo methiNew
        (
            L"GetEncoded"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetEncoded = c2AddMethodInfo(methiNew);
    }

    // Get the individual encoded parts
    {
        TMEngMethodInfo methiNew
        (
            L"GetEncodedParts"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"Proto", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"User", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Host", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Path", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"Suffix", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetEncodedParts = c2AddMethodInfo(methiNew);
    }

    // Get the fragment
    {
        TMEngMethodInfo methiNew
        (
            L"GetFragment"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetFragment = c2AddMethodInfo(methiNew);
    }

    // Get the full URL unencoded
    {
        TMEngMethodInfo methiNew
        (
            L"GetFullText"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetFullText = c2AddMethodInfo(methiNew);
    }

    // Get the host name part
    {
        TMEngMethodInfo methiNew
        (
            L"GetHost"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetHost = c2AddMethodInfo(methiNew);
    }

    // Get an IP end point that reflects the info in the URL, if available
    {
        TMEngMethodInfo methiNew
        (
            L"GetIPEndPoint"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", m_c2TypeId_IPEP);
        methiNew.c2AddInParm(L"AddrType", m_pmeciIPAddrTypes->c2Id());
        methiNew.c2AddInParm(L"ThrowIfNot", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_GetIPEndPoint = c2AddMethodInfo(methiNew);
    }

    // Get the params part
    {
        TMEngMethodInfo methiNew
        (
            L"GetParams"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetParams = c2AddMethodInfo(methiNew);
    }

    // Get the password part
    {
        TMEngMethodInfo methiNew
        (
            L"GetPassword"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetPassword = c2AddMethodInfo(methiNew);
    }

    // Get the path part
    {
        TMEngMethodInfo methiNew
        (
            L"GetPath"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetPath = c2AddMethodInfo(methiNew);
    }

    // Get the port number
    {
        TMEngMethodInfo methiNew
        (
            L"GetPort"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetPort = c2AddMethodInfo(methiNew);
    }

    // Get the protocol
    {
        TMEngMethodInfo methiNew
        (
            L"GetProto"
            , m_c2TypeId_Protos
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetProto = c2AddMethodInfo(methiNew);
    }

    // Get the query parameter list as K/V pairs
    {
        TMEngMethodInfo methiNew
        (
            L"GetQParms"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", m_c2TypeId_ItemList);
        m_c2MethId_GetQParms = c2AddMethodInfo(methiNew);
    }

    // Get the query parameter list formatted out
    {
        TMEngMethodInfo methiNew
        (
            L"GetQParmsFmt"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetQParmsFmt = c2AddMethodInfo(methiNew);
    }

    // Get the URL resource, optionally expanded and optionally with suffix
    {
        TMEngMethodInfo methiNew
        (
            L"GetResource"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Expanded", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddInParm(L"WithSuffix", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_GetResource = c2AddMethodInfo(methiNew);
    }

    // Get the user name part
    {
        TMEngMethodInfo methiNew
        (
            L"GetUserName"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_GetUserName = c2AddMethodInfo(methiNew);
    }

    // Set from text
    {
        TMEngMethodInfo methiNew
        (
            L"Set"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Text", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"FullyQual", c2FQTypes);
        m_c2MethId_Set = c2AddMethodInfo(methiNew);
    }

    // Set from Base text + relative text
    {
        TMEngMethodInfo methiNew
        (
            L"Set2"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"BaseText", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"RelativeText", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_Set2 = c2AddMethodInfo(methiNew);
    }

    // Set from base URL + relative text
    {
        TMEngMethodInfo methiNew
        (
            L"Set3"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"BaseURL", c2Id());
        methiNew.c2AddInParm(L"RelativeText", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_Set3 = c2AddMethodInfo(methiNew);
    }

    // Set from all of the info
    {
        TMEngMethodInfo methiNew
        (
            L"Set4"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Protocol", m_c2TypeId_Protos);
        methiNew.c2AddInParm(L"User", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Password", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Host", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"PortNum", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Path", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Params", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Fragment", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"QParms", m_c2TypeId_ItemList);
        m_c2MethId_Set4 = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngURLInfo::pmecvMakeStorage( const   TString&                strName
                                ,       TCIDMacroEngine&        meOwner
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngURLVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngURLInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngURLInfo::bInvokeMethod(        TCIDMacroEngine&    meOwner
                            , const TMEngMethodInfo&    methiTarget
                            ,       TMEngClassVal&      mecvInstance)
{
    TMEngURLVal& mecvActual = static_cast<TMEngURLVal&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_Clear)
    {
        mecvActual.Reset();
    }
     else if ((c2MethId == m_c2MethId_BuildURL)
          ||  (c2MethId == m_c2MethId_BuildURL2))
    {
        //
        //  If it is the 2 version we have to get out the KV pairs into
        //  a C++ list to pass in, else we pass a null pointer.
        //
        tCIDLib::TKVPList* pcolQPairs = nullptr;
        if (c2MethId == m_c2MethId_BuildURL2)
        {
            const TMEngVectorVal& mecvQPs
            (
                meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 8)
            );
            tCIDLib::TCard4 c4Count = mecvQPs.c4ElemCount();

            // Use +1 for max in case of zero count
            pcolQPairs = new tCIDLib::TKVPList(c4Count);
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                // Get the element and cast to its real type
                const TMEngKVPairVal& mecvCur
                (
                    static_cast<const TMEngKVPairVal&>
                    (
                        mecvQPs.mecvAt(meOwner, c4Index)
                    )
                );
                pcolQPairs->objAdd
                (
                    TKeyValuePair(mecvCur.strKey(), mecvCur.strValue())
                );
            }
        }
        TJanitor<tCIDLib::TKVPList> janQPs(pcolQPairs);

        // We have to ranslate the protocol enum to text
        TMEngEnumVal& mecvProto = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);

        // Build the URL into the return string
        TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        TURL::BuildURL
        (
            m_pmeciProtos->strTextValue(mecvProto)
            , meOwner.strStackValAt(c4FirstInd + 1)
            , meOwner.strStackValAt(c4FirstInd + 2)
            , meOwner.strStackValAt(c4FirstInd + 3)
            , meOwner.c4StackValAt(c4FirstInd + 4)
            , meOwner.strStackValAt(c4FirstInd + 5)
            , meOwner.strStackValAt(c4FirstInd + 6)
            , meOwner.strStackValAt(c4FirstInd + 7)
            , pcolQPairs
            , mecvRet.strValue()
            , tCIDSock::EExpOpts::Encode
        );
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Reset the value object
        mecvActual.Reset();
    }
     else if (c2MethId == m_c2MethId_Encode)
    {
        try
        {
            TMEngStringVal& mecvTar = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 1);
            TMEngEnumVal& mecvComp = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 2);

            TURL::EncodeTo
            (
                meOwner.strStackValAt(c4FirstInd)
                , mecvTar.strValue()
                , TURL::EExpTypes(m_pmeciComps->c4MapValue(mecvComp))
                , meOwner.bStackValAt(c4FirstInd + 3) ?
                    tCIDLib::EAppendOver::Append : tCIDLib::EAppendOver::Overwrite
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Encoding, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_Equal)
    {
        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue
        (
            meOwner.mecvStackAtAs<TMEngURLVal>(c4FirstInd).urlValue()
            == mecvActual.urlValue()
        );
    }
     else if (c2MethId == m_c2MethId_Expand)
    {
        try
        {
            TMEngStringVal& mecvTar = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 1);
            TMEngEnumVal& mecvComp = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 2);

            TURL::ExpandTo
            (
                meOwner.strStackValAt(c4FirstInd)
                , mecvTar.strValue()
                , TURL::EExpTypes(m_pmeciComps->c4MapValue(mecvComp))
                , meOwner.bStackValAt(c4FirstInd + 3) ?
                    tCIDLib::EAppendOver::Append : tCIDLib::EAppendOver::Overwrite
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Encoding, errToCatch);
        }
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner) << mecvActual.urlValue();
    }
     else if (c2MethId == m_c2MethId_GetEncoded)
    {
        try
        {
            TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
            mecvActual.urlValue().QueryText
            (
                mecvRet.strValue(), kCIDLib::False, kCIDLib::True
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Query, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetEncodedParts)
    {
        try
        {
            TMEngStringVal& mecvProto = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
            TMEngStringVal& mecvUser = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 1);
            TMEngStringVal& mecvHost = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 2);
            TMEngStringVal& mecvPath = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 3);
            TMEngStringVal& mecvSuffix = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 4);
            mecvActual.urlValue().QueryEncodedForm
            (
                mecvProto.strValue()
                , mecvUser.strValue()
                , mecvHost.strValue()
                , mecvPath.strValue()
                , mecvSuffix.strValue()
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Query, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetFragment)
    {
        try
        {
            TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
            mecvRet.strValue(mecvActual.urlValue().strFragment());
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Query, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetFullText)
    {
        try
        {
            TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
            mecvActual.urlValue().QueryText
            (
                mecvRet.strValue(), kCIDLib::True, kCIDLib::True
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Query, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetHost)
    {
        try
        {
            TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
            mecvRet.strValue(mecvActual.urlValue().strHost());
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Query, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetIPEndPoint)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        try
        {
            // Get the IP address type value
            TMEngEnumVal& mecvType = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1);
            TIPEndPoint ipepOut = mecvActual.urlValue().ipepHost
            (
                tCIDSock::EAddrTypes(m_pmeciIPAddrTypes->c4MapValue(mecvType))
            );

            // It worked, so set it on the caller's output parameter
            meOwner.mecvStackAtAs<TMEngIPEPVal>(c4FirstInd).ipepValue(ipepOut);

            mecvRet.bValue(kCIDLib::True);
        }

        catch(TError& errToCatch)
        {
            // Either throw or return false
            if (meOwner.bStackValAt(c4FirstInd + 2))
                ThrowAnErr(meOwner, m_c4ErrId_NoIPEP, errToCatch);

            mecvRet.bValue(kCIDLib::False);
        }
    }
     else if (c2MethId == m_c2MethId_GetParams)
    {
        try
        {
            TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
            mecvRet.strValue(mecvActual.urlValue().strParams());
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Query, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetPassword)
    {
        try
        {
            TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
            mecvRet.strValue(mecvActual.urlValue().strPassword());
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Query, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetPath)
    {
        try
        {
            TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
            mecvRet.strValue(mecvActual.urlValue().strPath());
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Query, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetPort)
    {
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.urlValue().ippnHost());
    }
     else if (c2MethId == m_c2MethId_GetProto)
    {
        TMEngEnumVal& mecvRet = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
        mecvRet.c4Ordinal(m_pmeciProtos->c4FromMapValue(mecvActual.urlValue().eProto()));
    }
     else if (c2MethId == m_c2MethId_GetQParms)
    {
        try
        {
            // Get the output list and clear it
            TMEngVectorVal& mecvQPs
            (
                meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd)
            );
            mecvQPs.RemoveAll();

            // Loop through the query parms list from the incoming URL
            const tCIDLib::TKVPList& colQPs = mecvActual.urlValue().colQParms();
            tCIDLib::TCard4 c4Count = colQPs.c4ElemCount();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                // Add a CML level KVPair into the caller's list for this one
                const TKeyValuePair& kvalCur = colQPs[c4Index];
                mecvQPs.AddObject
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

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Query, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetQParmsFmt)
    {
        try
        {
            // Get the parms from our URL
            const tCIDLib::TKVPList& colQPs = mecvActual.urlValue().colQParms();

            // Format them out to the caller's string
            TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
            TURL::FormatQParms
            (
                colQPs
                , mecvRet.strValue()
                , tCIDSock::EExpOpts::None
                , tCIDLib::EAppendOver::Overwrite
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Query, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetResource)
    {
        try
        {
            TMEngStringVal& mecvToFill = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
            mecvActual.urlValue().QueryResource
            (
                mecvToFill.strValue()
                , meOwner.bStackValAt(c4FirstInd + 1)
                , meOwner.bStackValAt(c4FirstInd + 2)
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Query, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetUserName)
    {
        try
        {
            TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd);
            mecvRet.strValue(mecvActual.urlValue().strUser());
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Query, errToCatch);
        }
    }
     else if ((c2MethId == m_c2MethId_Set) || (c2MethId == m_c2MethId_Set2))
    {
        try
        {
            if (c2MethId == m_c2MethId_Set)
            {
                TMEngEnumVal& mecvQual = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1);
                mecvActual.urlValue().Reset
                (
                    meOwner.strStackValAt(c4FirstInd)
                    , tCIDSock::EQualified(mecvQual.c4Ordinal())
                );
            }
             else if (c2MethId == m_c2MethId_Set2)
            {
                mecvActual.urlValue().Reset
                (
                    meOwner.strStackValAt(c4FirstInd)
                    , meOwner.strStackValAt(c4FirstInd + 1)
                );
            }
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Set, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_Set3)
    {
        try
        {
            mecvActual.urlValue().Reset
            (
                meOwner.mecvStackAtAs<TMEngURLVal>(c4FirstInd).urlValue()
                , meOwner.strStackValAt(c4FirstInd + 1)
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Set, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_Set4)
    {
        // Get out any CML level query parameters into a C++ version of the list
        const TMEngVectorVal& mecvQPs
        (
            meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd + 8)
        );
        tCIDLib::TCard4 c4Count = mecvQPs.c4ElemCount();

        // Use +1 for max in case of zero count
        tCIDLib::TKVPList colQPairs(c4Count);
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            // Get the element and cast to its real type
            const TMEngKVPairVal& mecvCur
            (
                static_cast<const TMEngKVPairVal&>
                (
                    mecvQPs.mecvAt(meOwner, c4Index)
                )
            );
            colQPairs.objAdd(TKeyValuePair(mecvCur.strKey(), mecvCur.strValue()));
        }

        try
        {
            const TMEngEnumVal& mecvProto(meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd));
            mecvActual.urlValue().Set
            (
                tCIDSock::EProtos(m_pmeciProtos->c4MapValue(mecvProto))
                , meOwner.strStackValAt(c4FirstInd + 1)
                , meOwner.strStackValAt(c4FirstInd + 2)
                , meOwner.strStackValAt(c4FirstInd + 3)
                , meOwner.c4StackValAt(c4FirstInd + 4)
                , meOwner.strStackValAt(c4FirstInd + 5)
                , meOwner.strStackValAt(c4FirstInd + 6)
                , meOwner.strStackValAt(c4FirstInd + 7)
                , colQPairs
                , kCIDLib::True
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_Set, errToCatch);
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngURLInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngURLInfo::ThrowAnErr(       TCIDMacroEngine&    meOwner
                        , const tCIDLib::TCard4     c4ToThrow
                        ,       TError&             errCaught)
{
    // If verbose log mode, we'll log the C++ level exception
    if (!errCaught.bLogged() && facCIDMacroEng().bLogWarnings())
    {
        errCaught.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errCaught);
    }

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


