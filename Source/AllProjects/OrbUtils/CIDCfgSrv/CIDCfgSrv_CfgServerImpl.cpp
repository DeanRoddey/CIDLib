//
// FILE NAME: CIDCfgSrv_CfgServerImpl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/05/2001
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
//  This method implements the TCIDCfgServerImpl class, which is the
//  implementation of the cfg server IDL generated interface. Note that the
//  object store provides it's own synchronziation, so we don't have to do
//  any.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDCfgSrv.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDCfgServerImpl,TCIDCfgSrvServerBase)



// ---------------------------------------------------------------------------
//   CLASS: TCIDCfgServerImpl
//  PREFIX: orbs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDCfgServerImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDCfgServerImpl::TCIDCfgServerImpl()
{
}

TCIDCfgServerImpl::~TCIDCfgServerImpl()
{
}


// ---------------------------------------------------------------------------
//  TCIDCfgServerImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TCIDCfgServerImpl::AddObject(const  TString&        strKey
                            , const tCIDLib::TCard4 c4DataBytes
                            , const THeapBuf&       mbufObjData
                            , const tCIDLib::TCard4 c4Reserve)
{
    m_oseRepo.AddObjectDirect(strKey, mbufObjData, c4DataBytes, c4Reserve);
}


tCIDLib::TBoolean
TCIDCfgServerImpl::bAllObjectsUnder(const   TString&            strStartScope
                                    ,       tCIDLib::TStrList&  colToFill)
{
    return m_oseRepo.bAllObjectsUnder(strStartScope, colToFill);
}


tCIDLib::TBoolean
TCIDCfgServerImpl::bAddOrUpdate(const   TString&            strKey
                                ,       tCIDLib::TCard4&    c4Version
                                , const tCIDLib::TCard4     c4DataBytes
                                , const THeapBuf&           mbufObjData
                                , const tCIDLib::TCard4     c4Reserve)
{
    return m_oseRepo.bAddOrUpdateDirect
    (
        strKey, c4Version, mbufObjData, c4DataBytes, c4Reserve
    );
}


tCIDLib::TBoolean TCIDCfgServerImpl::bDeleteObjectIfExists(const TString& strKey)
{
    return m_oseRepo.bDeleteObjectIfExists(strKey);
}


tCIDLib::TBoolean
TCIDCfgServerImpl::bFindNameUnder(  const   TString&            strName
                                    , const TString&            strStartScope
                                    ,       tCIDLib::TStrList&  colToFill)
{
    return m_oseRepo.bFindNameUnder(strName, strStartScope, colToFill);
}


tCIDLib::TBoolean TCIDCfgServerImpl::bKeyExists(const TString& strKey)
{
    return m_oseRepo.bKeyExists(strKey);
}

tCIDLib::TBoolean
TCIDCfgServerImpl::bKeyExists2(const TString& strKey, tCIDLib::TCard4& c4Version)
{
    return m_oseRepo.bKeyExists(strKey, c4Version);
}


//
//  Always throws if the key path doesn't exist. The return indicates new data or
//  not.
//
tCIDLib::TBoolean
TCIDCfgServerImpl::bReadObject( const   TString&            strKey
                                ,       tCIDLib::TCard4&    c4BytesRead
                                ,       THeapBuf&           mbufToFill
                                ,       tCIDLib::TCard4&    c4Version)
{
    const tCIDLib::ELoadRes eRes = m_oseRepo.eReadObjectDirect
    (
        strKey, c4Version, mbufToFill, c4BytesRead, kCIDLib::True
    );

    if (eRes != tCIDLib::ELoadRes::NewData)
    {
        c4BytesRead = 0;
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TCard4
TCIDCfgServerImpl::c4QueryObjectsInScope(const  TString&            strParScope
                                        ,       TVector<TString>&   colToFill)
{
    return m_oseRepo.c4QueryObjectsInScope(strParScope, colToFill);
}


tCIDLib::TCard4
TCIDCfgServerImpl::c4QuerySubScopes(const   TString&            strParScope
                                    ,       TVector<TString>&   colToFill)
{
    return m_oseRepo.c4QuerySubScopes(strParScope, colToFill);
}


tCIDLib::TCard4
TCIDCfgServerImpl::c4UpdateObject(  const   TString&        strKey
                                    , const tCIDLib::TCard4 c4DataBytes
                                    , const THeapBuf&       mbufObjData)
{
    return m_oseRepo.c4UpdateObjectDirect(strKey, mbufObjData, c4DataBytes);
}


tCIDLib::TVoid TCIDCfgServerImpl::DeleteObject(const TString& strKey)
{
    m_oseRepo.DeleteObject(strKey);
}


tCIDLib::TVoid TCIDCfgServerImpl::DeleteScope(const TString& strScopeKey)
{
    m_oseRepo.DeleteScope(strScopeKey);
}


//
//  Similar to bReadObject but is the lower level call, and can return an indicator
//  of new data, no new data, or not found (or optionally throw if not found.)
//
tCIDLib::ELoadRes
TCIDCfgServerImpl::eReadObject( const   TString&            strKey
                                ,       tCIDLib::TCard4&    c4BytesRead
                                ,       THeapBuf&           mbufToFill
                                ,       tCIDLib::TCard4&    c4Version
                                , const tCIDLib::TBoolean   bThrowIfNot)
{
    const tCIDLib::ELoadRes eRes = m_oseRepo.eReadObjectDirect
    (
        strKey, c4Version, mbufToFill, c4BytesRead, bThrowIfNot
    );

    if (eRes != tCIDLib::ELoadRes::NewData)
        c4BytesRead = 0;
    return eRes;
}


// ---------------------------------------------------------------------------
//  TCIDCfgServerImpl: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDCfgServerImpl::Initialize()
{
    //
    //  If the repo path doesn't exist, let's try to create it. Tell it not
    //  to throw if the path already exists, but it will if the path cannot
    //  be created.
    //
    if (!TFileSys::bIsDirectory(facCIDCfgSrv.strRepoPath()))
        TFileSys::MakeDirectory(facCIDCfgSrv.strRepoPath(), kCIDLib::False);

    // Let's initialize the object store
    try
    {
        m_oseRepo.bInitialize(facCIDCfgSrv.strRepoPath(), kCIDCfgSrv::pszRepFileName);
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        facCIDCfgSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCfgSrvErrs::errcStore_InitFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::InitFailed
        );
    }
}


tCIDLib::TVoid TCIDCfgServerImpl::Terminate()
{
    // Close the object store
    try
    {
        m_oseRepo.Close();
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        facCIDCfgSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCfgSrvErrs::errcStore_CloseFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
}


