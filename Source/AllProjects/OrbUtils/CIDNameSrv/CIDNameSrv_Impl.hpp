//
// FILE NAME: CIDNameSrv_Impl.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/16/2001
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
//  This is the header for the CIDNameSrv_Impl.cpp file, which provides an
//  implementation of the name server IDL interface.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TCIDNameServerImpl
//  PREFIX: orbs
// ---------------------------------------------------------------------------
class TCIDNameServerImpl : public TCIDNameSrvServerBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDNameServerImpl() = delete;

        TCIDNameServerImpl
        (
            const   TOrbObjId&              ooidToUse
            , const tCIDLib::TBoolean       bNoLease
        );

        TCIDNameServerImpl(const TCIDNameServerImpl&) = delete;

        ~TCIDNameServerImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDNameServerImpl& operator=(const TCIDNameServerImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBindingExists
        (
            const   TString&                strToCheck
            ,       tCIDLib::TCard8&        c4CurNSCookie
        );

        tCIDLib::TBoolean bGetObject
        (
            const   TString&                strBinding
            ,       TOrbObjId&              ooidToFill
            ,       tCIDLib::TCard8&        c4CurNSCookie
        );

        tCIDLib::TBoolean bQueryNameInfo
        (
            const   TString&                strNameToQuery
            ,       TString&                strName
            ,       TString&                strDescr
        );

        tCIDLib::TBoolean bQueryNameInfo2
        (
            const   TString&                strNameToQuery
            ,       TString&                strName
            ,       TString&                strDescr
            ,       TString&                strExtra1
            ,       TString&                strExtra2
            ,       TString&                strExtra3
            ,       TString&                strExtra4
        );

        tCIDLib::TBoolean bQueryScopeKeys
        (
                    tCIDLib::TCard4&        c4SerialNum
            , const TString&                strScopePath
            ,       tCIDLib::TStrList&      colResults
            , const tCIDLib::TBoolean       bObjectsOnly
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        tCIDLib::TBoolean bRebindObjs
        (
            const   TVector<TNSRebindInfo>& colBindings
            ,       TFundVector<tCIDOrb::ERebindRes>& fcolResults
            ,       tCIDLib::TCard8&        c8CurCookie
        );

        tCIDLib::TBoolean bRenewLeases
        (
            const   tCIDLib::TStrList&      colPaths
            ,       tCIDLib::TBoolList&     fcolResults
            ,       tCIDLib::TCard8&        c8CurCookie
        );

        tCIDLib::TBoolean bScopeExists
        (
            const   TString&                strToCheck
            ,       tCIDLib::TCard8&        c4CurNSCookie
        );

        tCIDLib::TVoid BindObj
        (
            const   TString&                strBinding
            , const TString&                strDescription
            , const TString&                strExtra1
            , const TString&                strExtra2
            , const TString&                strExtra3
            , const TString&                strExtra4
            , const TOrbObjId&              ooidToStore
        );

        tCIDLib::TVoid BindObj2
        (
            const   TString&                strParentNode
            , const TString&                strNodeName
            , const TString&                strDescription
            , const TString&                strExtra1
            , const TString&                strExtra2
            , const TString&                strExtra3
            , const TString&                strExtra4
            , const TOrbObjId&              ooidToStore
        );

        tCIDLib::TVoid BindObj3
        (
            const   TNSRebindInfo&          nsrbiToBind
        );

        tCIDLib::TCard4 c4EnumObjects
        (
            const   TString&                strScopeToEnum
            ,       TVector<TNameServerInfo>& colResults
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        tCIDLib::TCard4 c4EnumSubScopes
        (
            const   TString&                strScopeToEnum
            ,       tCIDLib::TKVPList&      colResults
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        tCIDLib::TCard8 c8QueryCookie();

        tCIDLib::TVoid ClearScope
        (
            const   TString&                strToClear
        );

        tCIDLib::TVoid CreateScope
        (
            const   TString&                strParentNode
            , const TString&                strNodeName
            , const TString&                strDescription
        );

        tCIDLib::TVoid CreateScopePath
        (
            const   TString&                strScopePath
            , const TString&                strDescription
        );

        tCIDLib::TVoid CreateScopePath
        (
            const   tCIDLib::TKVPList&      colPathParts
        );

        tCIDLib::TVoid Dump
        (
                    TString&                strToFill
            , const tCIDOrbUC::EDumpFmts eFormat
        );

        tCIDLib::TVoid QueryNameInfo
        (
            const   TString&                strNameToQuery
            ,       TString&                strName
            ,       TString&                strDescr
        );

        tCIDLib::TVoid QueryNameInfo2
        (
            const   TString&                strNameToQuery
            ,       TString&                strName
            ,       TString&                strDescr
            ,       TString&                strExtra1
            ,       TString&                strExtra2
            ,       TString&                strExtra3
            ,       TString&                strExtra4
        );

        tCIDLib::TVoid RemoveBinding
        (
            const   TString&                strToRemove
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        tCIDLib::TVoid RemoveScope
        (
            const   TString&                strToRemove
        );

        tCIDLib::TVoid UpdateExtraVal
        (
            const   TString&                strBinding
            , const tCIDLib::TCard4         c4Index
            , const TString&                strNewValue
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid MarkPermanent
        (
            const   TString&                strScope
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Initialize() override;

        tCIDLib::TVoid Terminate() override;


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        using TNSCache = TBasicTreeCol<TNSNode>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EExitCodes eLandlordThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::TVoid RecursiveDumpText
        (
                    TBasicTreeCol<TNSNode>::TNodeNT* const pnodeParent
            ,       TTextOutStream&         strmOut
            , const tCIDLib::TCard4         c4Depth
        );

        tCIDLib::TVoid RecursiveDumpXML
        (
                    TBasicTreeCol<TNSNode>::TNodeNT* const pnodeParent
            ,       TTextOutStream&         strmOut
            , const tCIDLib::TCard4         c4Depth
        );

        tCIDLib::TVoid RecursiveLeaseCheck
        (
                    TBasicTreeCol<TNSNode>::TNodeNT* const pnodeParent
            , const tCIDLib::TEncodedTime   enctNow
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bNoLease
        //      Indicates that we shouldn't enforce binding leases. Mostly
        //      for development purposes, to keep bindings from timeing out
        //      when programs are blocked for debugging purposes.
        //
        //  m_c8Cookie
        //      This is set to the encoded time when we first come up. Clients
        //      can poll this periodically to know if we've cycled and re-
        //      bind their objects with us, and also flush their object id
        //      caches.
        //
        //  m_colCache
        //      This is the tree collection that stores our hierarchical
        //      data structure.
        //
        //  m_mtxSync
        //      Though we could create the cache collection thread safe, many
        //      ops will require multiple calls, so it's more efficient to
        //      just provide it externally. We use a mutex instead of a
        //      critical section, because it could be pretty contentious at
        //      times.
        //
        //  m_thrLandlord
        //      All lease bindings must be renewed periodically, so we need
        //      a landlord thread to periodically collect the rent. Any bindings
        //      which have not been renewed within the lease period will be
        //      removed.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bNoLease;
        tCIDLib::TCard8     m_c8Cookie;
        TNSCache            m_colCache;
        TMutex              m_mtxSync;
        TThread             m_thrLandlord;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDNameServerImpl,TCIDNameSrvServerBase)
};

#pragma CIDLIB_POPPACK

