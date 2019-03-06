//
// FILE NAME: CIDMacroEng_DBaseClasses.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/08/2006
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the CML level classes for doing ODBC operations.
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
RTTIDecls(TMEngDBConnInfo,TMacroEngClass)
RTTIDecls(TMEngDBConnVal,TMEngClassVal)
RTTIDecls(TMEngDBStmtInfo,TMacroEngClass)
RTTIDecls(TMEngDBStmtVal,TMEngClassVal)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_DBaseClasses
{
    const TString   strConnClassPath(L"MEng.System.Runtime.DBConnect");
    const TString   strConnName(L"DBConnect");
    const TString   strStmtClassPath(L"MEng.System.Runtime.DBStatement");
    const TString   strStmtName(L"DBStatement");

    volatile tCIDLib::TBoolean bODBCInitDone = kCIDLib::False;
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngDBConnVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngDBConnVal: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngDBConnVal::TMEngDBConnVal( const   TString&                strName
                                , const tCIDLib::TCard2         c2Id
                                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_pdbconnVal(0)
{
    // Init the ODBC engine if not already.
    if (!CIDMacroEng_DBaseClasses::bODBCInitDone)
    {
        TBaseLock lockInit;
        if (!CIDMacroEng_DBaseClasses::bODBCInitDone)
        {
            facCIDDBase().Initialize();
            CIDMacroEng_DBaseClasses::bODBCInitDone = kCIDLib::True;
        }
    }
}

TMEngDBConnVal::~TMEngDBConnVal()
{
    try
    {
        delete m_pdbconnVal;
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }

    catch(...)
    {
        // Not much we can do
    }
}


// ---------------------------------------------------------------------------
//  TMEngDBConnVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngDBConnVal::bDbgFormat(         TTextOutStream&
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts
                            , const tCIDLib::ERadices
                            , const TCIDMacroEngine&) const
{
    // We have no displayable info
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TMEngDBConnVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngDBConnVal::bConnected() const
{
    return (m_pdbconnVal && m_pdbconnVal->bIsConnected());
}


const TDBConnection& TMEngDBConnVal::dbconnValue() const
{
    // If not created yet, then do it
    if (!m_pdbconnVal)
        m_pdbconnVal = new TDBConnection;
    return *m_pdbconnVal;
}

TDBConnection& TMEngDBConnVal::dbconnValue()
{
    // If not created yet, then do it
    if (!m_pdbconnVal)
        m_pdbconnVal = new TDBConnection;
    return *m_pdbconnVal;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngDBConnInfo
// PREFIX: meunit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngDBConnInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngDBConnInfo::strPath()
{
    return CIDMacroEng_DBaseClasses::strConnClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngDBConnInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngDBConnInfo::TMEngDBConnInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_DBaseClasses::strConnName
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2EnumId_Errors(kMacroEng::c2BadId)
    , m_c2MethId_Connect(kMacroEng::c2BadId)
    , m_c2MethId_Connect2(kMacroEng::c2BadId)
    , m_c2MethId_ConnectStr(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Disconnect(kMacroEng::c2BadId)
    , m_c2MethId_GetSrcList(kMacroEng::c2BadId)
    , m_c2MethId_GetSrcName(kMacroEng::c2BadId)
    , m_c2TypeId_KVPair(kMacroEng::c2BadId)
    , m_c2TypeId_SrcList(kMacroEng::c2BadId)
    , m_c4ErrAlreadyConnected(kCIDLib::c4MaxCard)
    , m_c4ErrConnFailed(kCIDLib::c4MaxCard)
    , m_c4ErrDisconnFailed(kCIDLib::c4MaxCard)
    , m_c4ErrGenError(kCIDLib::c4MaxCard)
    , m_c4ErrNotConnected(kCIDLib::c4MaxCard)
    , m_pmeciErrors(0)
{
    // Force the import of non-intrinsic classes we reference
    bAddClassImport(TMEngKVPairInfo::strPath());
}

TMEngDBConnInfo::~TMEngDBConnInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngDBConnInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngDBConnInfo::Init(TCIDMacroEngine& meOwner)
{
    // An enum to represent the exceptions that this class throws
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"DBConnErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 4
        );
        m_c4ErrAlreadyConnected = m_pmeciErrors->c4AddEnumItem(L"AlreadyConnected", L"The database connection has already been made");
        m_c4ErrConnFailed = m_pmeciErrors->c4AddEnumItem(L"ConnErr", TString::strEmpty());
        m_c4ErrDisconnFailed= m_pmeciErrors->c4AddEnumItem(L"DisconnErr", TString::strEmpty());
        m_c4ErrGenError = m_pmeciErrors->c4AddEnumItem(L"GenError", TString::strEmpty());
        m_c4ErrNotConnected = m_pmeciErrors->c4AddEnumItem(L"NotConnected", L"The database connection has not been made");
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // We need a vector of key/values for returning the list of sources
    m_c2TypeId_KVPair = meOwner.c2FindClassId(TMEngKVPairInfo::strPath());
    {
        TMEngVectorInfo* pmeciNew = new TMEngVectorInfo
        (
            meOwner
            , L"DBSrcList"
            , strClassPath()
            , TMEngVectorInfo::strPath()
            , m_c2TypeId_KVPair
        );
        TJanitor<TMEngVectorInfo> janNewClass(pmeciNew);
        pmeciNew->BaseClassInit(meOwner);
        bAddNestedType(pmeciNew->strClassPath());
        m_c2TypeId_SrcList = meOwner.c2AddClass(janNewClass.pobjOrphan());
    }


    // Connect to the data source using Windows authentication
    {
        TMEngMethodInfo methiNew
        (
            L"Connect"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Source", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_Connect = c2AddMethodInfo(methiNew);
    }

    // Connect to the data source using SQL Server authentication
    {
        TMEngMethodInfo methiNew
        (
            L"Connect2"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Source", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"UserName", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"Password", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_Connect2 = c2AddMethodInfo(methiNew);
    }

    // Connect using a connection string
    {
        TMEngMethodInfo methiNew
        (
            L"ConnectStr"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ConnName", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"ConnInStr", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"ConnOutStr", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_ConnectStr = c2AddMethodInfo(methiNew);
    }

    // Delete a file
    {
        TMEngMethodInfo methiNew
        (
            L"Disconnect"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Disconnect = c2AddMethodInfo(methiNew);
    }

    // The default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.DBConnect"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Query the list of available sources
    {
        TMEngMethodInfo methiNew
        (
            L"GetSrcList"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToFill", m_c2TypeId_SrcList);
        m_c2MethId_GetSrcList = c2AddMethodInfo(methiNew);
    }

    // Get the current data source name, if connected. If not, it'll throw
    {
        TMEngMethodInfo methiNew
        (
            L"GetSrcName"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetSrcName = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngDBConnInfo::pmecvMakeStorage(const  TString&                strName
                                ,       TCIDMacroEngine&
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngDBConnVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngDBConnInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngDBConnInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngDBConnVal& mecvActual = static_cast<TMEngDBConnVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);

    if (methiTarget.c2Id() == m_c2MethId_Connect)
    {
        if (mecvActual.bConnected())
            ThrowAnErr(meOwner, m_c4ErrAlreadyConnected);

        try
        {
            mecvActual.dbconnValue().Connect(meOwner.strStackValAt(c4FirstInd));
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrConnFailed, errToCatch.strErrText());
        }

        catch(...)
        {
            ThrowAnErr(meOwner, m_c4ErrConnFailed, L"A system exception occurred");
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_Connect2)
    {
        if (mecvActual.bConnected())
            ThrowAnErr(meOwner, m_c4ErrAlreadyConnected);

        try
        {
            mecvActual.dbconnValue().Connect
            (
                meOwner.strStackValAt(c4FirstInd)
                , meOwner.strStackValAt(c4FirstInd + 1)
                , meOwner.strStackValAt(c4FirstInd + 2)
                , kCIDLib::False
            );
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrConnFailed, errToCatch.strErrText());
        }

        catch(...)
        {
            ThrowAnErr(meOwner, m_c4ErrConnFailed, L"A system exception occurred");
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_ConnectStr)
    {
        if (mecvActual.bConnected())
            ThrowAnErr(meOwner, m_c4ErrAlreadyConnected);

        try
        {
            TMEngStringVal& mecvOut = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 2);

            mecvActual.dbconnValue().ConnectStr
            (
                meOwner.strStackValAt(c4FirstInd)
                , meOwner.strStackValAt(c4FirstInd + 1)
                , mecvOut.strValue()
            );
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrConnFailed, errToCatch.strErrText());
        }

        catch(...)
        {
            ThrowAnErr(meOwner, m_c4ErrConnFailed, L"A system exception occurred");
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_DefCtor)
    {
        // Nothing to do in this case
    }
     else if (methiTarget.c2Id() == m_c2MethId_Disconnect)
    {
        try
        {
            mecvActual.dbconnValue().Disconnect();
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrDisconnFailed, errToCatch.strErrText());
        }

        catch(...)
        {
            ThrowAnErr(meOwner, m_c4ErrDisconnFailed, L"A system exception occurred");
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_GetSrcList)
    {
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        try
        {
            // Ask the CIDDBase facility to get us the list of sources
            tCIDLib::TKVPList colSrcs;
            facCIDDBase().QuerySources(colSrcs, tCIDDBase::ESrcTypes::Both);

            // Load up our CML level vector
            const tCIDLib::TCard4 c4SrcCnt = colSrcs.c4ElemCount();
            TMEngVectorVal& mecvSrcs = meOwner.mecvStackAtAs<TMEngVectorVal>(c4FirstInd);
            mecvSrcs.RemoveAll();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SrcCnt; c4Index++)
            {
                const TKeyValuePair& kvalCur = colSrcs[c4Index];
                mecvSrcs.AddObject
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

            // Give back the count
            mecvRet.c4Value(c4SrcCnt);
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrGenError, errToCatch.strErrText());
        }

        catch(...)
        {
            ThrowAnErr(meOwner, m_c4ErrGenError, L"A system exception occurred");
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_GetSrcName)
    {
        if (!mecvActual.bConnected())
            ThrowAnErr(meOwner, m_c4ErrNotConnected);

        TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        try
        {
            mecvRet.strValue(mecvActual.dbconnValue().cptrHandle()->strDataSrc());
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrGenError, errToCatch.strErrText());
        }

        catch(...)
        {
            ThrowAnErr(meOwner, m_c4ErrGenError, L"A system exception occurred");
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngDBConnInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngDBConnInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , m_pmeciErrors->strTextValue(c4ToThrow)
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngDBConnInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
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





// ---------------------------------------------------------------------------
//  CLASS: TMEngDBStmtVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngDBStmtVal: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngDBStmtVal::TMEngDBStmtVal( const   TString&                strName
                                , const tCIDLib::TCard2         c2Id
                                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_pdbstmtVal(0)
{
}

TMEngDBStmtVal::~TMEngDBStmtVal()
{
    try
    {
        delete m_pdbstmtVal;
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }

    catch(...)
    {
        // Not much we can do
    }
}


// ---------------------------------------------------------------------------
//  TMEngDBStmtVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngDBStmtVal::bDbgFormat(         TTextOutStream&
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts
                            , const tCIDLib::ERadices
                            , const TCIDMacroEngine&) const
{
    // We have no displayable info
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TMEngDBStmtVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngDBStmtVal::bIsReady() const
{
    return (m_pdbstmtVal != 0);
}


const TDBDirStatement& TMEngDBStmtVal::dbstmtValue() const
{
    // If not created yet, then do it
    if (!m_pdbstmtVal)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcDbg_ValNotReady
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , CIDMacroEng_DBaseClasses::strStmtClassPath
        );
    }
    return *m_pdbstmtVal;
}

TDBDirStatement& TMEngDBStmtVal::dbstmtValue()
{
    // If not created yet, then do it
    if (!m_pdbstmtVal)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcDbg_ValNotReady
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , CIDMacroEng_DBaseClasses::strStmtClassPath
        );
    }
    return *m_pdbstmtVal;
}


tCIDLib::TVoid
TMEngDBStmtVal::Initialize( const   TDBConnection&  dbconTarget
                            , const TString&        strName)
{
    // Clean up any existing statement
    if (m_pdbstmtVal)
    {
        delete m_pdbstmtVal;
        m_pdbstmtVal = 0;
    }
    m_pdbstmtVal = new TDBDirStatement(dbconTarget, strName);
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngDBStmtInfo
// PREFIX: meunit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngDBStmtInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngDBStmtInfo::strPath()
{
    return CIDMacroEng_DBaseClasses::strStmtClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngDBStmtInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngDBStmtInfo::TMEngDBStmtInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_DBaseClasses::strStmtName
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2EnumId_ColTypes(kMacroEng::c2BadId)
    , m_c2EnumId_Errors(kMacroEng::c2BadId)
    , m_c2MethId_AddBinding(kMacroEng::c2BadId)
    , m_c2MethId_AddStrBinding(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Execute(kMacroEng::c2BadId)
    , m_c2MethId_Fetch(kMacroEng::c2BadId)
    , m_c2MethId_GetBoolCol(kMacroEng::c2BadId)
    , m_c2MethId_GetCard1Col(kMacroEng::c2BadId)
    , m_c2MethId_GetCard2Col(kMacroEng::c2BadId)
    , m_c2MethId_GetCard4Col(kMacroEng::c2BadId)
    , m_c2MethId_GetCard8Col(kMacroEng::c2BadId)
    , m_c2MethId_GetDateCol(kMacroEng::c2BadId)
    , m_c2MethId_GetInt1Col(kMacroEng::c2BadId)
    , m_c2MethId_GetInt2Col(kMacroEng::c2BadId)
    , m_c2MethId_GetInt4Col(kMacroEng::c2BadId)
    , m_c2MethId_GetName(kMacroEng::c2BadId)
    , m_c2MethId_GetStrCol(kMacroEng::c2BadId)
    , m_c2MethId_GetTimeCol(kMacroEng::c2BadId)
    , m_c2MethId_IsColNull(kMacroEng::c2BadId)
    , m_c2MethId_InitCtor(kMacroEng::c2BadId)
    , m_c2MethId_Reset(kMacroEng::c2BadId)
    , m_c4ErrAddBinding(kCIDLib::c4MaxCard)
    , m_c4ErrExecute(kCIDLib::c4MaxCard)
    , m_c4ErrFetch(kCIDLib::c4MaxCard)
    , m_c4ErrGetCol(kCIDLib::c4MaxCard)
    , m_c4ErrGetName(kCIDLib::c4MaxCard)
    , m_c4ErrNotReady(kCIDLib::c4MaxCard)
    , m_c4ErrReset(kCIDLib::c4MaxCard)
    , m_pmeciColTypes(0)
    , m_pmeciErrors(0)
{
}

TMEngDBStmtInfo::~TMEngDBStmtInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngDBStmtInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngDBStmtInfo::Init(TCIDMacroEngine& meOwner)
{
    // An enum to represent the exceptions that this class throws
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"DBStmtErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 8
        );
        m_c4ErrAddBinding = m_pmeciErrors->c4AddEnumItem(L"AddBindingErr", TString::strEmpty());
        m_c4ErrExecute = m_pmeciErrors->c4AddEnumItem(L"ExecErr", TString::strEmpty());
        m_c4ErrFetch = m_pmeciErrors->c4AddEnumItem(L"FetchErr", TString::strEmpty());
        m_c4ErrGetCol = m_pmeciErrors->c4AddEnumItem(L"GetColErr", TString::strEmpty());
        m_c4ErrGetName = m_pmeciErrors->c4AddEnumItem(L"GetNameErr", TString::strEmpty());
        m_c4ErrNotReady = m_pmeciErrors->c4AddEnumItem(L"NotReady", L"The statement has not been initialized yet");
        m_c4ErrReset = m_pmeciErrors->c4AddEnumItem(L"Reset", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // An enum to represent the column types that can be added
    {
        m_pmeciColTypes = new TMEngEnumInfo
        (
            meOwner
            , L"DBColTypes"
            , strClassPath()
            , L"MEng.Enum"
            , 13
        );
        m_pmeciColTypes->c4AddEnumItem(L"Boolean", L"Boolean", tCIDDBase::ECppTypes::Boolean);
        m_pmeciColTypes->c4AddEnumItem(L"Card1"  , L"Card1"  , tCIDDBase::ECppTypes::Card1);
        m_pmeciColTypes->c4AddEnumItem(L"Card2"  , L"Card2"  , tCIDDBase::ECppTypes::Card2);
        m_pmeciColTypes->c4AddEnumItem(L"Card4"  , L"Card4"  , tCIDDBase::ECppTypes::Card4);
        m_pmeciColTypes->c4AddEnumItem(L"Card8"  , L"Card8"  , tCIDDBase::ECppTypes::Card8);
        m_pmeciColTypes->c4AddEnumItem(L"Date"   , L"Date"   , tCIDDBase::ECppTypes::Date);
        m_pmeciColTypes->c4AddEnumItem(L"Int1"   , L"Int1"   , tCIDDBase::ECppTypes::Int1);
        m_pmeciColTypes->c4AddEnumItem(L"Int2"   , L"Int2"   , tCIDDBase::ECppTypes::Int2);
        m_pmeciColTypes->c4AddEnumItem(L"Int4"   , L"Int4"   , tCIDDBase::ECppTypes::Int4);
        m_pmeciColTypes->c4AddEnumItem(L"String" , L"String" , tCIDDBase::ECppTypes::String);
        m_pmeciColTypes->c4AddEnumItem(L"Time"   , L"Time"   , tCIDDBase::ECppTypes::Time);
        m_pmeciColTypes->BaseClassInit(meOwner);
        m_c2EnumId_ColTypes = meOwner.c2AddClass(m_pmeciColTypes);
        bAddNestedType(m_pmeciColTypes->strClassPath());
    }

    // Loop up the connection class
    const tCIDLib::TCard2 c2ConnType
    (
        meOwner.meciFind(CIDMacroEng_DBaseClasses::strConnClassPath).c2Id()
    );

    // Add a non-string binding
    {
        TMEngMethodInfo methiNew
        (
            L"AddBinding"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ColName", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"ColType", m_c2EnumId_ColTypes);
        m_c2MethId_AddBinding = c2AddMethodInfo(methiNew);
    }

    // Add a string binding
    {
        TMEngMethodInfo methiNew
        (
            L"AddStrBinding"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ColName", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"MaxChars", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_AddStrBinding = c2AddMethodInfo(methiNew);
    }

    // The default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.DBStatement"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Ctor that takes init values
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.System.Runtime.DBStatement"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Connection", c2ConnType);
        methiNew.c2AddInParm(L"Name", tCIDMacroEng::EIntrinsics::String);
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_InitCtor = c2AddMethodInfo(methiNew);
    }

    // Add a string binding
    {
        TMEngMethodInfo methiNew
        (
            L"Execute"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToExec", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_Execute = c2AddMethodInfo(methiNew);
    }

    // Fetch the next row
    {
        TMEngMethodInfo methiNew
        (
            L"Fetch"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Fetch = c2AddMethodInfo(methiNew);
    }

    // Pull out columns of various types
    {
        TMEngMethodInfo methiNew
        (
            L"GetBoolCol"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ColIndex", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetBoolCol = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetCard1Col"
            , tCIDMacroEng::EIntrinsics::Card1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ColIndex", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetCard1Col = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetCard2Col"
            , tCIDMacroEng::EIntrinsics::Card2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ColIndex", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetCard2Col = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetCard4Col"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ColIndex", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetCard4Col = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetCard8Col"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ColIndex", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetCard8Col = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetDateCol"
            , tCIDMacroEng::EIntrinsics::Time
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ColIndex", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetDateCol = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetInt1Col"
            , tCIDMacroEng::EIntrinsics::Int1
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ColIndex", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetInt1Col = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetInt2Col"
            , tCIDMacroEng::EIntrinsics::Int2
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ColIndex", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetInt2Col = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetInt4Col"
            , tCIDMacroEng::EIntrinsics::Int4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ColIndex", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetInt4Col = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetStrCol"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ColIndex", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetStrCol = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetTimeCol"
            , tCIDMacroEng::EIntrinsics::Time
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ColIndex", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetTimeCol = c2AddMethodInfo(methiNew);
    }

    // Get the data source name, if any
    {
        TMEngMethodInfo methiNew
        (
            L"GetName"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetName = c2AddMethodInfo(methiNew);
    }

    // Check if the last fetched value for a column is null
    {
        TMEngMethodInfo methiNew
        (
            L"IsColNull"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ColIndex", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_IsColNull = c2AddMethodInfo(methiNew);
    }

    // Reset the statement for a new use
    {
        TMEngMethodInfo methiNew
        (
            L"Reset"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Connection", c2ConnType);
        methiNew.c2AddInParm(L"Name", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_Reset = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngDBStmtInfo::pmecvMakeStorage(const  TString&                strName
                                ,       TCIDMacroEngine&
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngDBStmtVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngDBStmtInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngDBStmtInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngDBStmtVal& mecvActual = static_cast<TMEngDBStmtVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);

    if (methiTarget.c2Id() == m_c2MethId_AddBinding)
    {
        if (!mecvActual.bIsReady())
            ThrowAnErr(meOwner, m_c4ErrNotReady);
        try
        {
            TMEngEnumVal& mecvType = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1);
            mecvActual.dbstmtValue().AddColBinding
            (
                meOwner.strStackValAt(c4FirstInd)
                , tCIDDBase::ECppTypes(m_pmeciColTypes->c4MapValue(mecvType))
            );
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrAddBinding, errToCatch);
        }

        catch(...)
        {
            ThrowAnErr(meOwner, m_c4ErrAddBinding, L"A system exception occurred");
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_AddStrBinding)
    {
        if (!mecvActual.bIsReady())
            ThrowAnErr(meOwner, m_c4ErrNotReady);
        try
        {
            mecvActual.dbstmtValue().AddColStrBinding
            (
                meOwner.strStackValAt(c4FirstInd)
                , meOwner.c4StackValAt(c4FirstInd + 1)
            );
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrAddBinding, errToCatch);
        }

        catch(...)
        {
            ThrowAnErr(meOwner, m_c4ErrAddBinding, L"A system exception occurred");
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_DefCtor)
    {
        // Nothing to do in this case
    }
     else if (methiTarget.c2Id() == m_c2MethId_Execute)
    {
        if (!mecvActual.bIsReady())
            ThrowAnErr(meOwner, m_c4ErrNotReady);
        try
        {
            mecvActual.dbstmtValue().Execute(meOwner.strStackValAt(c4FirstInd));
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrExecute, errToCatch);
        }

        catch(...)
        {
            ThrowAnErr(meOwner, m_c4ErrExecute, L"A system exception occurred");
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_Fetch)
    {
        if (!mecvActual.bIsReady())
            ThrowAnErr(meOwner, m_c4ErrNotReady);

        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        try
        {
            mecvRet.bValue(mecvActual.dbstmtValue().bFetch());
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrFetch, errToCatch);
        }

        catch(...)
        {
            ThrowAnErr(meOwner, m_c4ErrFetch, L"A system exception occurred");
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_GetName)
    {
        if (!mecvActual.bIsReady())
            ThrowAnErr(meOwner, m_c4ErrNotReady);

        TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        try
        {
            mecvRet.strValue(mecvActual.dbstmtValue().strName());
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrGetName, errToCatch);
        }

        catch(...)
        {
            ThrowAnErr(meOwner, m_c4ErrGetName, L"A system exception occurred");
        }
    }
     else if ((methiTarget.c2Id() == m_c2MethId_GetBoolCol)
          ||  (methiTarget.c2Id() == m_c2MethId_GetCard1Col)
          ||  (methiTarget.c2Id() == m_c2MethId_GetCard2Col)
          ||  (methiTarget.c2Id() == m_c2MethId_GetCard4Col)
          ||  (methiTarget.c2Id() == m_c2MethId_GetCard8Col)
          ||  (methiTarget.c2Id() == m_c2MethId_GetDateCol)
          ||  (methiTarget.c2Id() == m_c2MethId_GetInt1Col)
          ||  (methiTarget.c2Id() == m_c2MethId_GetInt2Col)
          ||  (methiTarget.c2Id() == m_c2MethId_GetInt4Col)
          ||  (methiTarget.c2Id() == m_c2MethId_GetStrCol)
          ||  (methiTarget.c2Id() == m_c2MethId_GetTimeCol))
    {
        if (!mecvActual.bIsReady())
            ThrowAnErr(meOwner, m_c4ErrNotReady);

        try
        {
            if (methiTarget.c2Id() == m_c2MethId_GetBoolCol)
            {
                TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
                mecvRet.bValue
                (
                    mecvActual.dbstmtValue().bColVal(meOwner.c4StackValAt(c4FirstInd))
                );
            }
             else if (methiTarget.c2Id() == m_c2MethId_GetCard1Col)
            {
                TMEngCard1Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd - 1);
                mecvRet.c1Value
                (
                    mecvActual.dbstmtValue().c1ColVal(meOwner.c4StackValAt(c4FirstInd))
                );
            }
             else if (methiTarget.c2Id() == m_c2MethId_GetCard2Col)
            {
                TMEngCard2Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard2Val>(c4FirstInd - 1);
                mecvRet.c2Value
                (
                    mecvActual.dbstmtValue().c2ColVal(meOwner.c4StackValAt(c4FirstInd))
                );
            }
             else if (methiTarget.c2Id() == m_c2MethId_GetCard4Col)
            {
                TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
                mecvRet.c4Value
                (
                    mecvActual.dbstmtValue().c4ColVal(meOwner.c4StackValAt(c4FirstInd))
                );
            }
             else if (methiTarget.c2Id() == m_c2MethId_GetCard8Col)
            {
                TMEngCard8Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
                mecvRet.c8Value
                (
                    mecvActual.dbstmtValue().c8ColVal(meOwner.c4StackValAt(c4FirstInd))
                );
            }
             else if (methiTarget.c2Id() == m_c2MethId_GetDateCol)
            {
                TMEngTimeVal& mecvRet = meOwner.mecvStackAtAs<TMEngTimeVal>(c4FirstInd - 1);
                mecvRet.tmValue
                (
                    mecvActual.dbstmtValue().tmDateColVal(meOwner.c4StackValAt(c4FirstInd))
                );
            }
             else if (methiTarget.c2Id() == m_c2MethId_GetInt1Col)
            {
                TMEngInt1Val& mecvRet = meOwner.mecvStackAtAs<TMEngInt1Val>(c4FirstInd - 1);
                mecvRet.i1Value
                (
                    mecvActual.dbstmtValue().i1ColVal(meOwner.c4StackValAt(c4FirstInd))
                );
            }
             else if (methiTarget.c2Id() == m_c2MethId_GetInt2Col)
            {
                TMEngInt2Val& mecvRet = meOwner.mecvStackAtAs<TMEngInt2Val>(c4FirstInd - 1);
                mecvRet.i2Value
                (
                    mecvActual.dbstmtValue().i2ColVal(meOwner.c4StackValAt(c4FirstInd))
                );
            }
             else if (methiTarget.c2Id() == m_c2MethId_GetInt4Col)
            {
                TMEngInt4Val& mecvRet = meOwner.mecvStackAtAs<TMEngInt4Val>(c4FirstInd - 1);
                mecvRet.i4Value
                (
                    mecvActual.dbstmtValue().i4ColVal(meOwner.c4StackValAt(c4FirstInd))
                );
            }
             else if (methiTarget.c2Id() == m_c2MethId_GetStrCol)
            {
                TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
                mecvRet.strValue
                (
                    mecvActual.dbstmtValue().strColVal(meOwner.c4StackValAt(c4FirstInd))
                );
            }
             else if (methiTarget.c2Id() == m_c2MethId_GetTimeCol)
            {
                TMEngTimeVal& mecvRet = meOwner.mecvStackAtAs<TMEngTimeVal>(c4FirstInd - 1);
                mecvRet.tmValue
                (
                    mecvActual.dbstmtValue().tmTimeColVal(meOwner.c4StackValAt(c4FirstInd))
                );
            }
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrGetCol, errToCatch);
        }

        catch(...)
        {
            ThrowAnErr(meOwner, m_c4ErrGetCol, L"A system exception occurred");
        }
    }
     else if (methiTarget.c2Id() == m_c2MethId_IsColNull)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue
        (
            mecvActual.dbstmtValue().bIsColNull(meOwner.c4StackValAt(c4FirstInd))
        );
    }
     else if ((methiTarget.c2Id() == m_c2MethId_InitCtor)
          ||  (methiTarget.c2Id() == m_c2MethId_Reset))
    {
        try
        {
            mecvActual.Initialize
            (
                meOwner.mecvStackAtAs<TMEngDBConnVal>(c4FirstInd).dbconnValue()
                , meOwner.strStackValAt(c4FirstInd + 1)
            );
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrReset, errToCatch);
        }

        catch(...)
        {
            ThrowAnErr(meOwner, m_c4ErrReset, L"A system exception occurred");
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngDBStmtInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngDBStmtInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , m_pmeciErrors->strTextValue(c4ToThrow)
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngDBStmtInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
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

tCIDLib::TVoid
TMEngDBStmtInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            ,       TError&             errToThrow)
{
    // If verbose log mode, we'll log the C++ level exception
    if (!errToThrow.bLogged() && facCIDMacroEng().bLogWarnings())
    {
        errToThrow.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToThrow);
    }

    TString strErr(errToThrow.strErrText());
    if (errToThrow.bHasAuxText())
    {
        strErr.Append(L" [ODBC=");
        strErr.Append(errToThrow.strAuxText());
        strErr.Append(L"]");
    }

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strErr
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}


