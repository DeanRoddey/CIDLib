//
// FILE NAME: CIDCfgSrv_CfgServerImpl.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/05/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDCfgSrv_Impl.cpp file, which provides an
//  implementation of the log server IDL interface. That interface is from
//  the CIDOrbUC facility, so others can implement it and provide their own
//  log server if required. We just provide a canned implementation that will
//  serve most folks.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//   CLASS: TCIDNameServerImpl
//  PREFIX: orbs
// ---------------------------------------------------------------------------
class TCIDCfgServerImpl : public TCIDCfgSrvServerBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDCfgServerImpl();

        TCIDCfgServerImpl(const TCIDCfgServerImpl&) = delete;

        ~TCIDCfgServerImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDCfgServerImpl& operator=(const TCIDCfgServerImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddObject
        (
            const   TString&                strKey
            , const tCIDLib::TCard4         c4DataBytes
            , const THeapBuf&               mbufObjData
            , const tCIDLib::TCard4         c4Reserve
        )   override;

        tCIDLib::TBoolean bAddOrUpdate
        (
            const   TString&                strKey
            ,       tCIDLib::TCard4&        c4Version
            , const tCIDLib::TCard4         c4DataBytes
            , const THeapBuf&               mbufObjData
            , const tCIDLib::TCard4         c4Reserve
        )   override;

        tCIDLib::TBoolean bAllObjectsUnder
        (
            const   TString&                strStartPath
            ,       tCIDLib::TStrList&      colToFill
        )   override;

        tCIDLib::TBoolean bDeleteObjectIfExists
        (
            const   TString&                strKey
        )   override;

        tCIDLib::TBoolean bFindNameUnder
        (
            const   TString&                strName
            , const TString&                strStartPath
            ,       tCIDLib::TStrList&      colToFill
        )   override;

        tCIDLib::TBoolean bKeyExists
        (
            const   TString&                strKey
        )   override;

        tCIDLib::TBoolean bKeyExists2
        (
            const   TString&                strKey
            ,       tCIDLib::TCard4&        c4Version
        )   override;

        tCIDLib::TBoolean bReadObject
        (
            const   TString&                strKey
            ,       tCIDLib::TCard4&        c4BytesRead
            ,       THeapBuf&               mbufToFill
            ,       tCIDLib::TCard4&        c4Version
        )   override;

        tCIDLib::TCard4 c4QueryObjectsInScope
        (
            const   TString&                strParScope
            ,       tCIDLib::TStrList&      colToFill
        )   override;

        tCIDLib::TCard4 c4QuerySubScopes
        (
            const   TString&                strParScope
            ,       tCIDLib::TStrList&      colToFill
        )   override;

        tCIDLib::TCard4 c4UpdateObject
        (
            const   TString&                strKey
            , const tCIDLib::TCard4         c4DataBytes
            , const THeapBuf&               mbufObjData
        )   override;

        tCIDLib::TVoid DeleteObject
        (
            const   TString&                strKey
        )   override;

        tCIDLib::TVoid DeleteScope
        (
            const   TString&                strScopeKey
        )   override;

        tCIDLib::ELoadRes eReadObject
        (
            const   TString&                strKey
            ,       tCIDLib::TCard4&        c4BytesRead
            ,       THeapBuf&               mbufToFill
            ,       tCIDLib::TCard4&        c4Version
            , const tCIDLib::TBoolean       bThrowIfNot
        )   override;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Initialize() override;

        tCIDLib::TVoid Terminate() override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_oseRepo
        //      The object store that manages our repository. It has a fixed
        //      file name, but it's path is in m_strRepoPath. Note that the
        //      object store provides it's own synchronization, so we don't
        //      have to provide any.
        // -------------------------------------------------------------------
        TCIDObjStore    m_oseRepo;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDCfgServerImpl,TCIDCfgSrvServerBase)
};

