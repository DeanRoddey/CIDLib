// ----------------------------------------------------------------------------
//  FILE: CIDNameSrv_NameSrvServerBase.cpp
//  DATE: Tue, Jan 19 17:36:47 2021 -0500
//    ID: DA7622D8FE9D1744-8BBD12DB0DD8ABF3
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "CIDNameSrv.hpp"

// ----------------------------------------------------------------------------
// Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TCIDNameSrvServerBase,TOrbServerBase)


// ----------------------------------------------------------------------------
// Public, static data
// ----------------------------------------------------------------------------
const TString TCIDNameSrvServerBase::strInterfaceId(L"DA7622D8FE9D1744-8BBD12DB0DD8ABF3");
const TString TCIDNameSrvServerBase::strAdminScope(L"/CIDLib/CIDNameSrv");
const TString TCIDNameSrvServerBase::strAdminBinding(L"/CIDLib/CIDNameSrv/CoreAdmin");


// ----------------------------------------------------------------------------
// TCIDNameSrvServerBase: Destructor
// ----------------------------------------------------------------------------
TCIDNameSrvServerBase::~TCIDNameSrvServerBase()
{
}

// ----------------------------------------------------------------------------
// TCIDNameSrvServerBase: Protected Constructors
// ----------------------------------------------------------------------------
TCIDNameSrvServerBase::TCIDNameSrvServerBase() :
    TOrbServerBase(strInterfaceId, L"TCIDNameSrvClientProxy" )
{
}

TCIDNameSrvServerBase::TCIDNameSrvServerBase(const TOrbObjId& ooidThis) :
    TOrbServerBase(ooidThis)
{
}

// ----------------------------------------------------------------------------
// TCIDNameSrvServerBase: Public, non-virtual methods
// ----------------------------------------------------------------------------
tCIDLib::TVoid TCIDNameSrvServerBase::Dispatch(const TString& strMethodName, TOrbCmd& orbcToDispatch)
{
    if (strMethodName == L"bBindingExists")
    {
        TString strToCheck;
        orbcToDispatch.strmIn() >> strToCheck;
        tCIDLib::TCard8 c8CurCookie = {};
        tCIDLib::TBoolean retVal = bBindingExists
        (
            strToCheck
          , c8CurCookie
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << c8CurCookie;
    }
     else if (strMethodName == L"bGetObject")
    {
        TString strBinding;
        orbcToDispatch.strmIn() >> strBinding;
        TOrbObjId ooidToFill;
        tCIDLib::TCard8 c8CurCookie = {};
        tCIDLib::TBoolean retVal = bGetObject
        (
            strBinding
          , ooidToFill
          , c8CurCookie
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << ooidToFill;
        orbcToDispatch.strmOut() << c8CurCookie;
    }
     else if (strMethodName == L"bQueryNameInfo")
    {
        TString strNameToQuery;
        orbcToDispatch.strmIn() >> strNameToQuery;
        TString strName;
        TString strDescr;
        tCIDLib::TBoolean retVal = bQueryNameInfo
        (
            strNameToQuery
          , strName
          , strDescr
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << strName;
        orbcToDispatch.strmOut() << strDescr;
    }
     else if (strMethodName == L"bQueryNameInfo2")
    {
        TString strNameToQuery;
        orbcToDispatch.strmIn() >> strNameToQuery;
        TString strName;
        TString strDescr;
        TString strExtra1;
        TString strExtra2;
        TString strExtra3;
        TString strExtra4;
        tCIDLib::TBoolean retVal = bQueryNameInfo2
        (
            strNameToQuery
          , strName
          , strDescr
          , strExtra1
          , strExtra2
          , strExtra3
          , strExtra4
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << strName;
        orbcToDispatch.strmOut() << strDescr;
        orbcToDispatch.strmOut() << strExtra1;
        orbcToDispatch.strmOut() << strExtra2;
        orbcToDispatch.strmOut() << strExtra3;
        orbcToDispatch.strmOut() << strExtra4;
    }
     else if (strMethodName == L"bQueryScopeKeys")
    {
        tCIDLib::TCard4 c4SerialNum = {};
        orbcToDispatch.strmIn() >> c4SerialNum;
        TString strScopeToEnum;
        orbcToDispatch.strmIn() >> strScopeToEnum;
        TVector<TString> colResults;
        tCIDLib::TBoolean bObjectsOnly;
        orbcToDispatch.strmIn() >> bObjectsOnly;
        tCIDLib::TBoolean bThrowIfNot;
        orbcToDispatch.strmIn() >> bThrowIfNot;
        tCIDLib::TBoolean retVal = bQueryScopeKeys
        (
            c4SerialNum
          , strScopeToEnum
          , colResults
          , bObjectsOnly
          , bThrowIfNot
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        if (retVal)
        {
            orbcToDispatch.strmOut() << c4SerialNum;
            orbcToDispatch.strmOut() << colResults;
        }
    }
     else if (strMethodName == L"bRebindObjs")
    {
        TVector<TNSRebindInfo> colBindings;
        orbcToDispatch.strmIn() >> colBindings;
        TFundVector<tCIDOrb::ERebindRes> fcolResults;
        tCIDLib::TCard8 c8CurCookie = {};
        orbcToDispatch.strmIn() >> c8CurCookie;
        tCIDLib::TBoolean retVal = bRebindObjs
        (
            colBindings
          , fcolResults
          , c8CurCookie
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << fcolResults;
        orbcToDispatch.strmOut() << c8CurCookie;
    }
     else if (strMethodName == L"bRenewLeases")
    {
        TVector<TString> colPaths;
        orbcToDispatch.strmIn() >> colPaths;
        TFundVector<tCIDLib::TBoolean> fcolResults;
        tCIDLib::TCard8 c8CurCookie = {};
        orbcToDispatch.strmIn() >> c8CurCookie;
        tCIDLib::TBoolean retVal = bRenewLeases
        (
            colPaths
          , fcolResults
          , c8CurCookie
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << fcolResults;
        orbcToDispatch.strmOut() << c8CurCookie;
    }
     else if (strMethodName == L"bScopeExists")
    {
        TString strToCheck;
        orbcToDispatch.strmIn() >> strToCheck;
        tCIDLib::TCard8 c8CurCookie = {};
        tCIDLib::TBoolean retVal = bScopeExists
        (
            strToCheck
          , c8CurCookie
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << c8CurCookie;
    }
     else if (strMethodName == L"c4EnumObjects")
    {
        TString strScopeToEnum;
        orbcToDispatch.strmIn() >> strScopeToEnum;
        TVector<TNameServerInfo> colResults;
        tCIDLib::TBoolean bThrowIfNot;
        orbcToDispatch.strmIn() >> bThrowIfNot;
        tCIDLib::TCard4 retVal = c4EnumObjects
        (
            strScopeToEnum
          , colResults
          , bThrowIfNot
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << colResults;
    }
     else if (strMethodName == L"c4EnumSubScopes")
    {
        TString strScopeToEnum;
        orbcToDispatch.strmIn() >> strScopeToEnum;
        TVector<TKeyValuePair> colResults;
        tCIDLib::TBoolean bThrowIfNot;
        orbcToDispatch.strmIn() >> bThrowIfNot;
        tCIDLib::TCard4 retVal = c4EnumSubScopes
        (
            strScopeToEnum
          , colResults
          , bThrowIfNot
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << colResults;
    }
     else if (strMethodName == L"c8QueryCookie")
    {
        tCIDLib::TCard8 retVal = c8QueryCookie
        (
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
    }
     else if (strMethodName == L"BindObj")
    {
        TString strBinding;
        orbcToDispatch.strmIn() >> strBinding;
        TString strDescription;
        orbcToDispatch.strmIn() >> strDescription;
        TString strExtra1;
        orbcToDispatch.strmIn() >> strExtra1;
        TString strExtra2;
        orbcToDispatch.strmIn() >> strExtra2;
        TString strExtra3;
        orbcToDispatch.strmIn() >> strExtra3;
        TString strExtra4;
        orbcToDispatch.strmIn() >> strExtra4;
        TOrbObjId ooidToStore;
        orbcToDispatch.strmIn() >> ooidToStore;
        BindObj
        (
            strBinding
          , strDescription
          , strExtra1
          , strExtra2
          , strExtra3
          , strExtra4
          , ooidToStore
        );
        orbcToDispatch.strmOut().Reset();
    }
     else if (strMethodName == L"BindObj2")
    {
        TString strParentScope;
        orbcToDispatch.strmIn() >> strParentScope;
        TString strNewName;
        orbcToDispatch.strmIn() >> strNewName;
        TString strDescription;
        orbcToDispatch.strmIn() >> strDescription;
        TString strExtra1;
        orbcToDispatch.strmIn() >> strExtra1;
        TString strExtra2;
        orbcToDispatch.strmIn() >> strExtra2;
        TString strExtra3;
        orbcToDispatch.strmIn() >> strExtra3;
        TString strExtra4;
        orbcToDispatch.strmIn() >> strExtra4;
        TOrbObjId ooidToStore;
        orbcToDispatch.strmIn() >> ooidToStore;
        BindObj2
        (
            strParentScope
          , strNewName
          , strDescription
          , strExtra1
          , strExtra2
          , strExtra3
          , strExtra4
          , ooidToStore
        );
        orbcToDispatch.strmOut().Reset();
    }
     else if (strMethodName == L"BindObj3")
    {
        TNSRebindInfo nsrbiToBind;
        orbcToDispatch.strmIn() >> nsrbiToBind;
        BindObj3
        (
            nsrbiToBind
        );
        orbcToDispatch.strmOut().Reset();
    }
     else if (strMethodName == L"ClearScope")
    {
        TString strToClear;
        orbcToDispatch.strmIn() >> strToClear;
        ClearScope
        (
            strToClear
        );
        orbcToDispatch.strmOut().Reset();
    }
     else if (strMethodName == L"CreateScope")
    {
        TString strParentScope;
        orbcToDispatch.strmIn() >> strParentScope;
        TString strNewScope;
        orbcToDispatch.strmIn() >> strNewScope;
        TString strDescription;
        orbcToDispatch.strmIn() >> strDescription;
        tCIDLib::TBoolean bThrowIfExists;
        orbcToDispatch.strmIn() >> bThrowIfExists;
        CreateScope
        (
            strParentScope
          , strNewScope
          , strDescription
          , bThrowIfExists
        );
        orbcToDispatch.strmOut().Reset();
    }
     else if (strMethodName == L"CreateScopePath")
    {
        TString strScopePath;
        orbcToDispatch.strmIn() >> strScopePath;
        TString strDescription;
        orbcToDispatch.strmIn() >> strDescription;
        CreateScopePath
        (
            strScopePath
          , strDescription
        );
        orbcToDispatch.strmOut().Reset();
    }
     else if (strMethodName == L"CreateScopePath1")
    {
        TVector<TKeyValuePair> colPathParts;
        orbcToDispatch.strmIn() >> colPathParts;
        CreateScopePath
        (
            colPathParts
        );
        orbcToDispatch.strmOut().Reset();
    }
     else if (strMethodName == L"Dump")
    {
        TString strToFill;
        tCIDOrbUC::EDumpFmts eFormat;
        orbcToDispatch.strmIn() >> eFormat;
        Dump
        (
            strToFill
          , eFormat
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << strToFill;
    }
     else if (strMethodName == L"QueryNameInfo")
    {
        TString strPath;
        orbcToDispatch.strmIn() >> strPath;
        TString strName;
        TString strDescr;
        QueryNameInfo
        (
            strPath
          , strName
          , strDescr
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << strName;
        orbcToDispatch.strmOut() << strDescr;
    }
     else if (strMethodName == L"QueryNameInfo2")
    {
        TString strPath;
        orbcToDispatch.strmIn() >> strPath;
        TString strName;
        TString strDescr;
        TString strExtra1;
        TString strExtra2;
        TString strExtra3;
        TString strExtra4;
        QueryNameInfo2
        (
            strPath
          , strName
          , strDescr
          , strExtra1
          , strExtra2
          , strExtra3
          , strExtra4
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << strName;
        orbcToDispatch.strmOut() << strDescr;
        orbcToDispatch.strmOut() << strExtra1;
        orbcToDispatch.strmOut() << strExtra2;
        orbcToDispatch.strmOut() << strExtra3;
        orbcToDispatch.strmOut() << strExtra4;
    }
     else if (strMethodName == L"RemoveBinding")
    {
        TString strToRemove;
        orbcToDispatch.strmIn() >> strToRemove;
        tCIDLib::TBoolean bThrowIfNot;
        orbcToDispatch.strmIn() >> bThrowIfNot;
        RemoveBinding
        (
            strToRemove
          , bThrowIfNot
        );
        orbcToDispatch.strmOut().Reset();
    }
     else if (strMethodName == L"RemoveScope")
    {
        TString strToRemove;
        orbcToDispatch.strmIn() >> strToRemove;
        RemoveScope
        (
            strToRemove
        );
        orbcToDispatch.strmOut().Reset();
    }
     else if (strMethodName == L"UpdateExtraVal")
    {
        TString strBinding;
        orbcToDispatch.strmIn() >> strBinding;
        tCIDLib::TCard4 c4Index;
        orbcToDispatch.strmIn() >> c4Index;
        TString strNewValue;
        orbcToDispatch.strmIn() >> strNewValue;
        UpdateExtraVal
        (
            strBinding
          , c4Index
          , strNewValue
        );
        orbcToDispatch.strmOut().Reset();
    }
     else
    {
         TParent::Dispatch(strMethodName, orbcToDispatch);
    }
}

