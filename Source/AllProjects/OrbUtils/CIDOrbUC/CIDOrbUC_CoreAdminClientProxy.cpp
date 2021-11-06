// ----------------------------------------------------------------------------
//  FILE: CIDOrbUC_CoreAdminClientProxy.cpp
//  DATE: Fri, Nov 05 19:32:39 2021 -0400
//    ID: 2117FF72EE8BD81D-F320AE6BEB1B2F0C
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
AdvRTTIDecls(TCIDCoreAdminClientProxy,TOrbClientBase)


// ----------------------------------------------------------------------------
// TCIDCoreAdminClientProxy: Public, static data
// ----------------------------------------------------------------------------
const TString TCIDCoreAdminClientProxy::strInterfaceId(L"2117FF72EE8BD81D-F320AE6BEB1B2F0C");

// ----------------------------------------------------------------------------
// TCIDCoreAdminClientProxy: Constructors and Destructor
// ----------------------------------------------------------------------------
TCIDCoreAdminClientProxy::TCIDCoreAdminClientProxy()
{
}

TCIDCoreAdminClientProxy::
TCIDCoreAdminClientProxy(const TOrbObjId& ooidSrc, const TString& strNSBinding)
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

TCIDCoreAdminClientProxy::~TCIDCoreAdminClientProxy()
{
}

// ----------------------------------------------------------------------------
// TCIDCoreAdminClientProxy: Public, non-virtual methods
// ----------------------------------------------------------------------------
tCIDLib::TVoid TCIDCoreAdminClientProxy::AdminStop()
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"AdminStop");
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

tCIDLib::TBoolean TCIDCoreAdminClientProxy::bPollValuesInScope
(
    const TString& strParScope
    , const tCIDLib::TBoolean bDirectOnly
    , const tCIDLib::TCard8 c8QueryStamp
    , CIOP tCIDLib::TCard8& c8PollStamp
    , COP TFundVector<tCIDLib::TCard4>& fcolIds
    , COP TFundVector<tCIDLib::TCard8>& fcolValues)
{
    #pragma warning(suppress : 26494)
    tCIDLib::TBoolean retVal;
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"bPollValuesInScope");
        ocmdToUse.strmOut() << strParScope;
        ocmdToUse.strmOut() << bDirectOnly;
        ocmdToUse.strmOut() << c8QueryStamp;
        ocmdToUse.strmOut() << c8PollStamp;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> retVal;
        ocmdToUse.strmIn() >> c8PollStamp;
        ocmdToUse.strmIn() >> fcolIds;
        ocmdToUse.strmIn() >> fcolValues;
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

tCIDLib::TCard4 TCIDCoreAdminClientProxy::c4QueryStats
(
    const TString& strParScope
    , COP TVector<TStatsCacheItemInfo>& colItems
    , const tCIDLib::TBoolean bDirectOnly
    , CIOP tCIDLib::TCard8& c8Stamp)
{
    #pragma warning(suppress : 26494)
    tCIDLib::TCard4 retVal;
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"c4QueryStats");
        ocmdToUse.strmOut() << strParScope;
        ocmdToUse.strmOut() << bDirectOnly;
        ocmdToUse.strmOut() << c8Stamp;
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> retVal;
        ocmdToUse.strmIn() >> colItems;
        ocmdToUse.strmIn() >> c8Stamp;
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

tCIDLib::TVoid TCIDCoreAdminClientProxy::QueryBasicInfo
(
    COP TString& strHostName
    , COP TString& strProcName
    , COP tCIDLib::TCard4& c4CPUCount
    , COP TString& strOSInfo
    , COP tCIDLib::TCard8& c8StartTime)
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"QueryBasicInfo");
        Dispatch(30000, pcqiToUse);
        ocmdToUse.strmIn().Reset();
        ocmdToUse.strmIn() >> strHostName;
        ocmdToUse.strmIn() >> strProcName;
        ocmdToUse.strmIn() >> c4CPUCount;
        ocmdToUse.strmIn() >> strOSInfo;
        ocmdToUse.strmIn() >> c8StartTime;
        GiveBackCmdItem(pcqiToUse);
    }
    catch(TError& errToCatch)
    {
        GiveBackCmdItem(pcqiToUse);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

tCIDLib::TVoid TCIDCoreAdminClientProxy::SetModLogLevel
(
    const TString& strModule
    , const TString& strLevel)
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"SetModLogLevel");
        ocmdToUse.strmOut() << strModule;
        ocmdToUse.strmOut() << strLevel;
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

tCIDLib::TVoid TCIDCoreAdminClientProxy::SetModLogTrace
(
    const TString& strModule
    , const tCIDLib::TBoolean bState)
{
    TCmdQItem* pcqiToUse = pcqiGetCmdItem(ooidThis().oidKey());
    TOrbCmd& ocmdToUse = pcqiToUse->ocmdData();
    try
    {
        ocmdToUse.strmOut() << TString(L"SetModLogTrace");
        ocmdToUse.strmOut() << strModule;
        ocmdToUse.strmOut() << bState;
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

