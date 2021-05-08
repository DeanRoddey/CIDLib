// ----------------------------------------------------------------------------
//  FILE: CIDCfgSrv_CfgServerBase.cpp
//  DATE: Sat, May 01 11:54:20 2021 -0400
//    ID: 9BEA088A1BD33D3A-FB3EA0B995167E0E
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "CIDCfgSrv.hpp"

// ----------------------------------------------------------------------------
// Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TCIDCfgSrvServerBase,TOrbServerBase)


// ----------------------------------------------------------------------------
// Public, static data
// ----------------------------------------------------------------------------
const TString TCIDCfgSrvServerBase::strInterfaceId(L"9BEA088A1BD33D3A-FB3EA0B995167E0E");
const TString TCIDCfgSrvServerBase::strScope(L"/CIDLib/CfgSrv/%(h)");
const TString TCIDCfgSrvServerBase::strAdminBinding(L"/CIDLib/CfgSrv/%(h)/CoreAdmin");
const TString TCIDCfgSrvServerBase::strBinding(L"/CIDLib/CfgSrv/%(h)/RepoObj");


// ----------------------------------------------------------------------------
// TCIDCfgSrvServerBase: Destructor
// ----------------------------------------------------------------------------
TCIDCfgSrvServerBase::~TCIDCfgSrvServerBase()
{
}

// ----------------------------------------------------------------------------
// TCIDCfgSrvServerBase: Protected Constructors
// ----------------------------------------------------------------------------
TCIDCfgSrvServerBase::TCIDCfgSrvServerBase() :
    TOrbServerBase(strInterfaceId, L"TCIDCfgSrvClientProxy" )
{
}

TCIDCfgSrvServerBase::TCIDCfgSrvServerBase(const TOrbObjId& ooidThis) :
    TOrbServerBase(ooidThis)
{
}

// ----------------------------------------------------------------------------
// TCIDCfgSrvServerBase: Public, non-virtual methods
// ----------------------------------------------------------------------------
tCIDLib::TVoid TCIDCfgSrvServerBase::Dispatch(const TString& strMethodName, TOrbCmd& orbcToDispatch)
{
    if (strMethodName == L"AddObject")
    {
        TString strKey;
        orbcToDispatch.strmIn() >> strKey;
        tCIDLib::TCard4 c4BufSz_mbufObjData;
        orbcToDispatch.strmIn() >> c4BufSz_mbufObjData;
        THeapBuf mbufObjData(c4BufSz_mbufObjData + 1);
        orbcToDispatch.strmIn().c4ReadBuffer(mbufObjData, c4BufSz_mbufObjData);
        tCIDLib::TCard4 c4Reserve;
        orbcToDispatch.strmIn() >> c4Reserve;
        AddObject
        (
            strKey
          , c4BufSz_mbufObjData
          , mbufObjData
          , c4Reserve
        );
        orbcToDispatch.strmOut().Reset();
    }
     else if (strMethodName == L"bAddOrUpdate")
    {
        TString strKey;
        orbcToDispatch.strmIn() >> strKey;
        tCIDLib::TCard4 c4Version = {};
        tCIDLib::TCard4 c4BufSz_mbufObjData;
        orbcToDispatch.strmIn() >> c4BufSz_mbufObjData;
        THeapBuf mbufObjData(c4BufSz_mbufObjData + 1);
        orbcToDispatch.strmIn().c4ReadBuffer(mbufObjData, c4BufSz_mbufObjData);
        tCIDLib::TCard4 c4Reserve;
        orbcToDispatch.strmIn() >> c4Reserve;
        tCIDLib::TBoolean retVal = bAddOrUpdate
        (
            strKey
          , c4Version
          , c4BufSz_mbufObjData
          , mbufObjData
          , c4Reserve
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << c4Version;
    }
     else if (strMethodName == L"bAllObjectsUnder")
    {
        TString strStartScope;
        orbcToDispatch.strmIn() >> strStartScope;
        TVector<TString> colPathsFound;
        tCIDLib::TBoolean retVal = bAllObjectsUnder
        (
            strStartScope
          , colPathsFound
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << colPathsFound;
    }
     else if (strMethodName == L"bDeleteObjectIfExists")
    {
        TString strKey;
        orbcToDispatch.strmIn() >> strKey;
        tCIDLib::TBoolean retVal = bDeleteObjectIfExists
        (
            strKey
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
    }
     else if (strMethodName == L"bFindNameUnder")
    {
        TString strName;
        orbcToDispatch.strmIn() >> strName;
        TString strStartScope;
        orbcToDispatch.strmIn() >> strStartScope;
        TVector<TString> colPathsFound;
        tCIDLib::TBoolean retVal = bFindNameUnder
        (
            strName
          , strStartScope
          , colPathsFound
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << colPathsFound;
    }
     else if (strMethodName == L"bKeyExists")
    {
        TString strKey;
        orbcToDispatch.strmIn() >> strKey;
        tCIDLib::TBoolean retVal = bKeyExists
        (
            strKey
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
    }
     else if (strMethodName == L"bKeyExists2")
    {
        TString strKey;
        orbcToDispatch.strmIn() >> strKey;
        tCIDLib::TCard4 c4Version = {};
        tCIDLib::TBoolean retVal = bKeyExists2
        (
            strKey
          , c4Version
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << c4Version;
    }
     else if (strMethodName == L"bReadObject")
    {
        TString strKey;
        orbcToDispatch.strmIn() >> strKey;
        tCIDLib::TCard4 c4BufSz_mbufToFill = 0;
        THeapBuf mbufToFill;
        tCIDLib::TCard4 c4Version = {};
        orbcToDispatch.strmIn() >> c4Version;
        tCIDLib::TBoolean retVal = bReadObject
        (
            strKey
          , c4BufSz_mbufToFill
          , mbufToFill
          , c4Version
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << c4BufSz_mbufToFill;
        orbcToDispatch.strmOut().c4WriteBuffer(mbufToFill, c4BufSz_mbufToFill);
        orbcToDispatch.strmOut() << c4Version;
    }
     else if (strMethodName == L"c4QueryObjectsInScope")
    {
        TString strParScope;
        orbcToDispatch.strmIn() >> strParScope;
        TVector<TString> colToFill;
        tCIDLib::TCard4 retVal = c4QueryObjectsInScope
        (
            strParScope
          , colToFill
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << colToFill;
    }
     else if (strMethodName == L"c4QuerySubScopes")
    {
        TString strParScope;
        orbcToDispatch.strmIn() >> strParScope;
        TVector<TString> colToFill;
        tCIDLib::TCard4 retVal = c4QuerySubScopes
        (
            strParScope
          , colToFill
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << colToFill;
    }
     else if (strMethodName == L"c4UpdateObject")
    {
        TString strKey;
        orbcToDispatch.strmIn() >> strKey;
        tCIDLib::TCard4 c4BufSz_mbufObjData;
        orbcToDispatch.strmIn() >> c4BufSz_mbufObjData;
        THeapBuf mbufObjData(c4BufSz_mbufObjData + 1);
        orbcToDispatch.strmIn().c4ReadBuffer(mbufObjData, c4BufSz_mbufObjData);
        tCIDLib::TCard4 retVal = c4UpdateObject
        (
            strKey
          , c4BufSz_mbufObjData
          , mbufObjData
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
    }
     else if (strMethodName == L"DeleteObject")
    {
        TString strKey;
        orbcToDispatch.strmIn() >> strKey;
        DeleteObject
        (
            strKey
        );
        orbcToDispatch.strmOut().Reset();
    }
     else if (strMethodName == L"DeleteScope")
    {
        TString strScopeKey;
        orbcToDispatch.strmIn() >> strScopeKey;
        DeleteScope
        (
            strScopeKey
        );
        orbcToDispatch.strmOut().Reset();
    }
     else if (strMethodName == L"eReadObject")
    {
        TString strKey;
        orbcToDispatch.strmIn() >> strKey;
        tCIDLib::TCard4 c4BufSz_mbufToFill = 0;
        THeapBuf mbufToFill;
        tCIDLib::TCard4 c4Version = {};
        orbcToDispatch.strmIn() >> c4Version;
        tCIDLib::TBoolean bThrowIfNot;
        orbcToDispatch.strmIn() >> bThrowIfNot;
        tCIDLib::ELoadRes retVal = eReadObject
        (
            strKey
          , c4BufSz_mbufToFill
          , mbufToFill
          , c4Version
          , bThrowIfNot
        );
        orbcToDispatch.strmOut().Reset();
        orbcToDispatch.strmOut() << retVal;
        orbcToDispatch.strmOut() << c4BufSz_mbufToFill;
        orbcToDispatch.strmOut().c4WriteBuffer(mbufToFill, c4BufSz_mbufToFill);
        orbcToDispatch.strmOut() << c4Version;
    }
     else
    {
         TParent::Dispatch(strMethodName, orbcToDispatch);
    }
}

