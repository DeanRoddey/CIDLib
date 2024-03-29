// ----------------------------------------------------------------------------
//  FILE: CIDOrbUC_NameSrvClientProxy.cpp
//  DATE: Fri, Nov 05 19:32:39 2021 -0400
//    ID: DA7622D8FE9D1744-8BBD12DB0DD8ABF3
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "CIDOrbUC_.hpp"

// ----------------------------------------------------------------------------
// Magic macros
// ----------------------------------------------------------------------------
AdvRTTIDecls(TCIDNameSrvClientProxy,TOrbClientBase)


// ----------------------------------------------------------------------------
// TCIDNameSrvClientProxy: Public, static data
// ----------------------------------------------------------------------------
const TString TCIDNameSrvClientProxy::strInterfaceId(L"DA7622D8FE9D1744-8BBD12DB0DD8ABF3");
const TString TCIDNameSrvClientProxy::strAdminScope(L"/CIDLib/CIDNameSrv");
const TString TCIDNameSrvClientProxy::strAdminBinding(L"/CIDLib/CIDNameSrv/CoreAdmin");



// ----------------------------------------------------------------------------
// TCIDNameSrvClientProxy: Constructors and Destructor
// ----------------------------------------------------------------------------
TCIDNameSrvClientProxy::TCIDNameSrvClientProxy()
{
}

TCIDNameSrvClientProxy::
TCIDNameSrvClientProxy(const TOrbObjId& ooidSrc, const TString& strNSBinding)
{
    #if CID_DEBUG_ON
    if (!ooidSrc.oidKey().bIsInterfaceId(strInterfaceId))
    {
        facCIDOrb().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbErrs::errcClient_BadIntfId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , ooidSrc.oidKey().mhashInterface()
            , strInterfaceId
        );
    }
    #endif
    SetObjId(ooidSrc, strNSBinding, kCIDLib::False);
}

TCIDNameSrvClientProxy::~TCIDNameSrvClientProxy()
{
}

// ----------------------------------------------------------------------------
// TCIDNameSrvClientProxy: Public, non-virtual methods
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TCIDNameSrvClientProxy::bBindingExists
(
    const TString& strToCheck
    , COP tCIDLib::TCard8& c8CurCookie)
{
    #pragma warning(suppress : 26494)
    tCIDLib::TBoolean retVal;
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"bBindingExists");
        ocmdToUse.strmOut() << strToCheck;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> retVal;
        ocmdToUse.strmIn() >> c8CurCookie;
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return retVal;
}

tCIDLib::TBoolean TCIDNameSrvClientProxy::bGetObject
(
    const TString& strBinding
    , COP TOrbObjId& ooidToFill
    , COP tCIDLib::TCard8& c8CurCookie)
{
    #pragma warning(suppress : 26494)
    tCIDLib::TBoolean retVal;
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"bGetObject");
        ocmdToUse.strmOut() << strBinding;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> retVal;
        ocmdToUse.strmIn() >> ooidToFill;
        ocmdToUse.strmIn() >> c8CurCookie;
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return retVal;
}

tCIDLib::TBoolean TCIDNameSrvClientProxy::bQueryNameInfo
(
    const TString& strNameToQuery
    , COP TString& strName
    , COP TString& strDescr)
{
    #pragma warning(suppress : 26494)
    tCIDLib::TBoolean retVal;
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"bQueryNameInfo");
        ocmdToUse.strmOut() << strNameToQuery;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> retVal;
        ocmdToUse.strmIn() >> strName;
        ocmdToUse.strmIn() >> strDescr;
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return retVal;
}

tCIDLib::TBoolean TCIDNameSrvClientProxy::bQueryNameInfo2
(
    const TString& strNameToQuery
    , COP TString& strName
    , COP TString& strDescr
    , COP TString& strExtra1
    , COP TString& strExtra2
    , COP TString& strExtra3
    , COP TString& strExtra4)
{
    #pragma warning(suppress : 26494)
    tCIDLib::TBoolean retVal;
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"bQueryNameInfo2");
        ocmdToUse.strmOut() << strNameToQuery;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> retVal;
        ocmdToUse.strmIn() >> strName;
        ocmdToUse.strmIn() >> strDescr;
        ocmdToUse.strmIn() >> strExtra1;
        ocmdToUse.strmIn() >> strExtra2;
        ocmdToUse.strmIn() >> strExtra3;
        ocmdToUse.strmIn() >> strExtra4;
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return retVal;
}

tCIDLib::TBoolean TCIDNameSrvClientProxy::bQueryScopeKeys
(
    CIOP tCIDLib::TCard4& c4SerialNum
    , const TString& strScopeToEnum
    , TVector<TString>& colResults
    , const tCIDLib::TBoolean bObjectsOnly
    , const tCIDLib::TBoolean bThrowIfNot)
{
    #pragma warning(suppress : 26494)
    tCIDLib::TBoolean retVal;
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"bQueryScopeKeys");
        ocmdToUse.strmOut() << c4SerialNum;
        ocmdToUse.strmOut() << strScopeToEnum;
        ocmdToUse.strmOut() << bObjectsOnly;
        ocmdToUse.strmOut() << bThrowIfNot;
        Dispatch(10000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> retVal;
        if (retVal)
        {
            ocmdToUse.strmIn() >> c4SerialNum;
            ocmdToUse.strmIn() >> colResults;
        }
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return retVal;
}

tCIDLib::TBoolean TCIDNameSrvClientProxy::bRebindObjs
(
    const TVector<TNSRebindInfo>& colBindings
    , COP TFundVector<tCIDOrb::ERebindRes>& fcolResults
    , CIOP tCIDLib::TCard8& c8CurCookie)
{
    #pragma warning(suppress : 26494)
    tCIDLib::TBoolean retVal;
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"bRebindObjs");
        ocmdToUse.strmOut() << colBindings;
        ocmdToUse.strmOut() << c8CurCookie;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> retVal;
        ocmdToUse.strmIn() >> fcolResults;
        ocmdToUse.strmIn() >> c8CurCookie;
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return retVal;
}

tCIDLib::TBoolean TCIDNameSrvClientProxy::bRenewLeases
(
    const TVector<TString>& colPaths
    , COP TFundVector<tCIDLib::TBoolean>& fcolResults
    , CIOP tCIDLib::TCard8& c8CurCookie)
{
    #pragma warning(suppress : 26494)
    tCIDLib::TBoolean retVal;
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"bRenewLeases");
        ocmdToUse.strmOut() << colPaths;
        ocmdToUse.strmOut() << c8CurCookie;
        Dispatch(10000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> retVal;
        ocmdToUse.strmIn() >> fcolResults;
        ocmdToUse.strmIn() >> c8CurCookie;
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return retVal;
}

tCIDLib::TBoolean TCIDNameSrvClientProxy::bScopeExists
(
    const TString& strToCheck
    , COP tCIDLib::TCard8& c8CurCookie)
{
    #pragma warning(suppress : 26494)
    tCIDLib::TBoolean retVal;
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"bScopeExists");
        ocmdToUse.strmOut() << strToCheck;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> retVal;
        ocmdToUse.strmIn() >> c8CurCookie;
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return retVal;
}

tCIDLib::TCard4 TCIDNameSrvClientProxy::c4EnumObjects
(
    const TString& strScopeToEnum
    , COP TVector<TNameServerInfo>& colResults
    , const tCIDLib::TBoolean bThrowIfNot)
{
    #pragma warning(suppress : 26494)
    tCIDLib::TCard4 retVal;
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"c4EnumObjects");
        ocmdToUse.strmOut() << strScopeToEnum;
        ocmdToUse.strmOut() << bThrowIfNot;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> retVal;
        ocmdToUse.strmIn() >> colResults;
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return retVal;
}

tCIDLib::TCard4 TCIDNameSrvClientProxy::c4EnumSubScopes
(
    const TString& strScopeToEnum
    , COP TVector<TKeyValuePair>& colResults
    , const tCIDLib::TBoolean bThrowIfNot)
{
    #pragma warning(suppress : 26494)
    tCIDLib::TCard4 retVal;
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"c4EnumSubScopes");
        ocmdToUse.strmOut() << strScopeToEnum;
        ocmdToUse.strmOut() << bThrowIfNot;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> retVal;
        ocmdToUse.strmIn() >> colResults;
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return retVal;
}

tCIDLib::TCard8 TCIDNameSrvClientProxy::c8QueryCookie()
{
    #pragma warning(suppress : 26494)
    tCIDLib::TCard8 retVal;
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"c8QueryCookie");
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> retVal;
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return retVal;
}

tCIDLib::TVoid TCIDNameSrvClientProxy::BindObj
(
    const TString& strBinding
    , const TString& strDescription
    , const TString& strExtra1
    , const TString& strExtra2
    , const TString& strExtra3
    , const TString& strExtra4
    , const TOrbObjId& ooidToStore)
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"BindObj");
        ocmdToUse.strmOut() << strBinding;
        ocmdToUse.strmOut() << strDescription;
        ocmdToUse.strmOut() << strExtra1;
        ocmdToUse.strmOut() << strExtra2;
        ocmdToUse.strmOut() << strExtra3;
        ocmdToUse.strmOut() << strExtra4;
        ocmdToUse.strmOut() << ooidToStore;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

tCIDLib::TVoid TCIDNameSrvClientProxy::BindObj2
(
    const TString& strParentScope
    , const TString& strNewName
    , const TString& strDescription
    , const TString& strExtra1
    , const TString& strExtra2
    , const TString& strExtra3
    , const TString& strExtra4
    , const TOrbObjId& ooidToStore)
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"BindObj2");
        ocmdToUse.strmOut() << strParentScope;
        ocmdToUse.strmOut() << strNewName;
        ocmdToUse.strmOut() << strDescription;
        ocmdToUse.strmOut() << strExtra1;
        ocmdToUse.strmOut() << strExtra2;
        ocmdToUse.strmOut() << strExtra3;
        ocmdToUse.strmOut() << strExtra4;
        ocmdToUse.strmOut() << ooidToStore;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

tCIDLib::TVoid TCIDNameSrvClientProxy::BindObj3
(
    const TNSRebindInfo& nsrbiToBind)
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"BindObj3");
        ocmdToUse.strmOut() << nsrbiToBind;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

tCIDLib::TVoid TCIDNameSrvClientProxy::ClearScope
(
    const TString& strToClear)
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"ClearScope");
        ocmdToUse.strmOut() << strToClear;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

tCIDLib::TVoid TCIDNameSrvClientProxy::CreateScope
(
    const TString& strParentScope
    , const TString& strNewScope
    , const TString& strDescription
    , const tCIDLib::TBoolean bThrowIfExists)
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"CreateScope");
        ocmdToUse.strmOut() << strParentScope;
        ocmdToUse.strmOut() << strNewScope;
        ocmdToUse.strmOut() << strDescription;
        ocmdToUse.strmOut() << bThrowIfExists;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

tCIDLib::TVoid TCIDNameSrvClientProxy::CreateScopePath
(
    const TString& strScopePath
    , const TString& strDescription)
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"CreateScopePath");
        ocmdToUse.strmOut() << strScopePath;
        ocmdToUse.strmOut() << strDescription;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

tCIDLib::TVoid TCIDNameSrvClientProxy::CreateScopePath
(
    const TVector<TKeyValuePair>& colPathParts)
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"CreateScopePath1");
        ocmdToUse.strmOut() << colPathParts;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

tCIDLib::TVoid TCIDNameSrvClientProxy::Dump
(
    COP TString& strToFill
    , const tCIDOrbUC::EDumpFmts eFormat)
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"Dump");
        ocmdToUse.strmOut() << eFormat;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> strToFill;
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

tCIDLib::TVoid TCIDNameSrvClientProxy::QueryNameInfo
(
    const TString& strPath
    , COP TString& strName
    , COP TString& strDescr)
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"QueryNameInfo");
        ocmdToUse.strmOut() << strPath;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> strName;
        ocmdToUse.strmIn() >> strDescr;
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

tCIDLib::TVoid TCIDNameSrvClientProxy::QueryNameInfo2
(
    const TString& strPath
    , COP TString& strName
    , COP TString& strDescr
    , COP TString& strExtra1
    , COP TString& strExtra2
    , COP TString& strExtra3
    , COP TString& strExtra4)
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"QueryNameInfo2");
        ocmdToUse.strmOut() << strPath;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> strName;
        ocmdToUse.strmIn() >> strDescr;
        ocmdToUse.strmIn() >> strExtra1;
        ocmdToUse.strmIn() >> strExtra2;
        ocmdToUse.strmIn() >> strExtra3;
        ocmdToUse.strmIn() >> strExtra4;
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

tCIDLib::TVoid TCIDNameSrvClientProxy::RemoveBinding
(
    const TString& strToRemove
    , const tCIDLib::TBoolean bThrowIfNot)
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"RemoveBinding");
        ocmdToUse.strmOut() << strToRemove;
        ocmdToUse.strmOut() << bThrowIfNot;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

tCIDLib::TVoid TCIDNameSrvClientProxy::RemoveScope
(
    const TString& strToRemove)
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"RemoveScope");
        ocmdToUse.strmOut() << strToRemove;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

tCIDLib::TVoid TCIDNameSrvClientProxy::UpdateExtraVal
(
    const TString& strBinding
    , const tCIDLib::TCard4 c4Index
    , const TString& strNewValue)
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"UpdateExtraVal");
        ocmdToUse.strmOut() << strBinding;
        ocmdToUse.strmOut() << c4Index;
        ocmdToUse.strmOut() << strNewValue;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

