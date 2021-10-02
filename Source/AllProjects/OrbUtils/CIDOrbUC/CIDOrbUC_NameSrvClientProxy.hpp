// ----------------------------------------------------------------------------
//  FILE: CIDOrbUC_NameSrvClientProxy.hpp
//  DATE: Fri, Oct 01 19:41:29 2021 -0400
//    ID: DA7622D8FE9D1744-8BBD12DB0DD8ABF3
//
//  This file was generated by the Charmed Quark CIDIDL compiler. Do not make
//  changes by hand, because they will be lost if the file is regenerated.
// ----------------------------------------------------------------------------
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

class CIDORBUCEXP TCIDNameSrvClientProxy : public TOrbClientBase
{
    public :
        // --------------------------------------------------------------------
        // Public, static data
        // --------------------------------------------------------------------
        static const TString strInterfaceId;
        static const TString strAdminScope;
        static const TString strAdminBinding;



        // --------------------------------------------------------------------
        // Public Constructors and Destructor
        // --------------------------------------------------------------------
        TCIDNameSrvClientProxy();
        TCIDNameSrvClientProxy(const TOrbObjId& ooidSrc, const TString& strNSBinding);
        TCIDNameSrvClientProxy(const TCIDNameSrvClientProxy&) = delete;
        TCIDNameSrvClientProxy(TCIDNameSrvClientProxy&&) = delete;
        ~TCIDNameSrvClientProxy();

        // --------------------------------------------------------------------
        // Public operators
        // --------------------------------------------------------------------
        TCIDNameSrvClientProxy& operator=(const TCIDNameSrvClientProxy&) = delete;
        TCIDNameSrvClientProxy& operator=(TCIDNameSrvClientProxy&&) = delete;

        // --------------------------------------------------------------------
        // Public, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid SetObjId
        (
            const   TOrbObjId& ooidToSet
            , const TString& strNSBinding
            , const tCIDLib::TBoolean bCheck = kCIDLib::True
        )   override;

        // --------------------------------------------------------------------
        // Public, non-virtual methods
        // --------------------------------------------------------------------
        tCIDLib::TBoolean bBindingExists
        (
            const TString& strToCheck
            , COP tCIDLib::TCard8& c8CurCookie
        );

        tCIDLib::TBoolean bGetObject
        (
            const TString& strBinding
            , COP TOrbObjId& ooidToFill
            , COP tCIDLib::TCard8& c8CurCookie
        );

        tCIDLib::TBoolean bQueryNameInfo
        (
            const TString& strNameToQuery
            , COP TString& strName
            , COP TString& strDescr
        );

        tCIDLib::TBoolean bQueryNameInfo2
        (
            const TString& strNameToQuery
            , COP TString& strName
            , COP TString& strDescr
            , COP TString& strExtra1
            , COP TString& strExtra2
            , COP TString& strExtra3
            , COP TString& strExtra4
        );

        tCIDLib::TBoolean bQueryScopeKeys
        (
            CIOP tCIDLib::TCard4& c4SerialNum
            , const TString& strScopeToEnum
            , TVector<TString>& colResults
            , const tCIDLib::TBoolean bObjectsOnly
            , const tCIDLib::TBoolean bThrowIfNot
        );

        tCIDLib::TBoolean bRebindObjs
        (
            const TVector<TNSRebindInfo>& colBindings
            , COP TFundVector<tCIDOrb::ERebindRes>& fcolResults
            , CIOP tCIDLib::TCard8& c8CurCookie
        );

        tCIDLib::TBoolean bRenewLeases
        (
            const TVector<TString>& colPaths
            , COP TFundVector<tCIDLib::TBoolean>& fcolResults
            , CIOP tCIDLib::TCard8& c8CurCookie
        );

        tCIDLib::TBoolean bScopeExists
        (
            const TString& strToCheck
            , COP tCIDLib::TCard8& c8CurCookie
        );

        tCIDLib::TCard4 c4EnumObjects
        (
            const TString& strScopeToEnum
            , COP TVector<TNameServerInfo>& colResults
            , const tCIDLib::TBoolean bThrowIfNot
        );

        tCIDLib::TCard4 c4EnumSubScopes
        (
            const TString& strScopeToEnum
            , COP TVector<TKeyValuePair>& colResults
            , const tCIDLib::TBoolean bThrowIfNot
        );

        tCIDLib::TCard8 c8QueryCookie();

        tCIDLib::TVoid BindObj
        (
            const TString& strBinding
            , const TString& strDescription
            , const TString& strExtra1
            , const TString& strExtra2
            , const TString& strExtra3
            , const TString& strExtra4
            , const TOrbObjId& ooidToStore
        );

        tCIDLib::TVoid BindObj2
        (
            const TString& strParentScope
            , const TString& strNewName
            , const TString& strDescription
            , const TString& strExtra1
            , const TString& strExtra2
            , const TString& strExtra3
            , const TString& strExtra4
            , const TOrbObjId& ooidToStore
        );

        tCIDLib::TVoid BindObj3
        (
            const TNSRebindInfo& nsrbiToBind
        );

        tCIDLib::TVoid ClearScope
        (
            const TString& strToClear
        );

        tCIDLib::TVoid CreateScope
        (
            const TString& strParentScope
            , const TString& strNewScope
            , const TString& strDescription
            , const tCIDLib::TBoolean bThrowIfExists = kCIDLib::True
        );

        tCIDLib::TVoid CreateScopePath
        (
            const TString& strScopePath
            , const TString& strDescription
        );

        tCIDLib::TVoid CreateScopePath
        (
            const TVector<TKeyValuePair>& colPathParts
        );

        tCIDLib::TVoid Dump
        (
            COP TString& strToFill
            , const tCIDOrbUC::EDumpFmts eFormat
        );

        tCIDLib::TVoid QueryNameInfo
        (
            const TString& strPath
            , COP TString& strName
            , COP TString& strDescr
        );

        tCIDLib::TVoid QueryNameInfo2
        (
            const TString& strPath
            , COP TString& strName
            , COP TString& strDescr
            , COP TString& strExtra1
            , COP TString& strExtra2
            , COP TString& strExtra3
            , COP TString& strExtra4
        );

        tCIDLib::TVoid RemoveBinding
        (
            const TString& strToRemove
            , const tCIDLib::TBoolean bThrowIfNot
        );

        tCIDLib::TVoid RemoveScope
        (
            const TString& strToRemove
        );

        tCIDLib::TVoid UpdateExtraVal
        (
            const TString& strBinding
            , const tCIDLib::TCard4 c4Index
            , const TString& strNewValue
        );

    protected :
        // --------------------------------------------------------------------
        // Declare our friends
        // --------------------------------------------------------------------
        friend class TFacCIDOrb;

    private :
        // --------------------------------------------------------------------
        // Magic macros
        // --------------------------------------------------------------------
        RTTIDefs(TCIDNameSrvClientProxy,TOrbClientBase)
};

#pragma CIDLIB_POPPACK

inline tCIDLib::TVoid
TCIDNameSrvClientProxy::SetObjId(const  TOrbObjId& ooidToSet
              , const TString& strNSBinding
              , const tCIDLib::TBoolean bCheck)
{
    TParent::SetObjId(ooidToSet, strNSBinding, bCheck);
}

