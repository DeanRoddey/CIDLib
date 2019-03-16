//
// FILE NAME: CIDNameSrv_Impl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/16/2001
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This method implements the TCIDNameSrvImpl class, which is the
//  implementation of the name server IDL generated interface.
//
// CAVEATS/GOTCHAS:
//
//  1)  Any time that we get an error and throw something or try to log
//      sometimes, we need to release the lock on the cache collection,
//      because if any of that code somehow tried to do some ORB operation,
//      we could deadlock!
//
//      So put the locker inside the try block where possible. Where not
//      possible, manually release the locker in the catch.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDNameSrv.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDNameServerImpl,TCIDNameSrvServerBase)


// ---------------------------------------------------------------------------
//   CLASS: TCIDNameServerImpl
//  PREFIX: orbs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDNameServerImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDNameServerImpl::TCIDNameServerImpl( const   TOrbObjId&          ooidToUse
                                        , const tCIDLib::TBoolean   bNoLease) :
    TCIDNameSrvServerBase(ooidToUse)
    , m_bNoLease(bNoLease)
    , m_c8Cookie(TTime::enctNow())
    , m_colCache()
    , m_thrLandlord
      (
        L"CIDNameSrvLandLordThread"
        , TMemberFunc<TCIDNameServerImpl>
          (
            this
            , &TCIDNameServerImpl::eLandlordThread
          )
      )
{
}

TCIDNameServerImpl::~TCIDNameServerImpl()
{
}


// ---------------------------------------------------------------------------
//  TCIDNameServerImpl: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  See if the indicated binding exists. This is bindings only, for scopes use bScopeExists
//  instead. Returns the name server cookie to allow for simultaneous checks for changes.
//
tCIDLib::TBoolean
TCIDNameServerImpl::bBindingExists( const   TString&            strToCheck
                                    ,       tCIDLib::TCard8&    c4CurNSCookie)
{
    tCIDLib::ETreeNodes eType;

    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    // Give back the current cookie. Do it NOW in case we return early
    c4CurNSCookie = m_c8Cookie;

    // If it doesn't exist at all, then obviously we failed
    if (!m_colCache.bNodeExists(strToCheck, eType))
        return kCIDLib::False;

    // And it has to be a terminal type
    if (eType != tCIDLib::ETreeNodes::Terminal)
        return kCIDLib::False;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TCIDNameServerImpl::bGetObject( const   TString&            strBinding
                                ,       TOrbObjId&          ooidToFill
                                ,       tCIDLib::TCard8&    c4CurNSCookie)
{
    try
    {
        // Lock the cache
        TMtxLocker lockCache(&m_mtxSync);

        // Give back the current cookie. Do it NOW in case we throw!
        c4CurNSCookie = m_c8Cookie;

        const TNSNode& nodeAt = m_colCache.objAt(strBinding);
        ooidToFill = nodeAt.ooidThis();
    }

    catch(TError& errToCatch)
    {
        //
        //  If its just "node not found", then return false. If it was the
        //  wrong type of node, then throw one of our NS specific errors for
        //  that. Else, just rethrow.
        //
        //  WE ARE NOT locked here, so don't access the cache object!
        //
        if (errToCatch.bCheckEvent( facCIDLib().strName()
                                    , kCIDErrs::errcCol_NamedNodeNotFound))
        {
            return kCIDLib::False;
        }
         else if (errToCatch.bCheckEvent(facCIDLib().strName()
                                        , kCIDErrs::errcCol_MustBeTerminal))
        {
            facCIDOrbUC().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbUCErrs::errcNSP_NotABinding
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::TypeMatch
                , strBinding
            );
        }
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TCIDNameServerImpl::bQueryNameInfo( const   TString&    strNameToQuery
                                    ,       TString&    strName
                                    ,       TString&    strDescr)
{
    try
    {
        // Lock the cache and do the query
        TMtxLocker lockCache(&m_mtxSync);
        m_colCache.QueryNameAndDesc(strNameToQuery, strName, strDescr);
    }

    catch(TError& errToCatch)
    {
        //
        //  If its just "node not found", then return false. Else, just
        //  rethrow.
        //
        //  WE ARE NOT locked here, so don't access the cache object!
        //
        if (errToCatch.bCheckEvent( facCIDLib().strName()
                                    , kCIDErrs::errcCol_NamedNodeNotFound))
        {
            if (facCIDNameSrv.bLogWarnings())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
            return kCIDLib::False;
        }
        throw;
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean
TCIDNameServerImpl::bQueryNameInfo2(const   TString&    strNameToQuery
                                    ,       TString&    strName
                                    ,       TString&    strDescr
                                    ,       TString&    strExtra1
                                    ,       TString&    strExtra2
                                    ,       TString&    strExtra3
                                    ,       TString&    strExtra4)
{
    try
    {
        // Lock the cache and do the query
        TMtxLocker lockCache(&m_mtxSync);
        const TNSNode& nodeInfo = m_colCache.objAt(strNameToQuery, strName, strDescr);

        // The extra user values are in the node data
        strExtra1 = nodeInfo.strExtra1();
        strExtra2 = nodeInfo.strExtra2();
        strExtra3 = nodeInfo.strExtra3();
        strExtra4 = nodeInfo.strExtra4();
    }

    catch(TError& errToCatch)
    {
        //
        //  If its just "node not found", then return false. Else, just
        //  rethrow.
        //
        //  WE ARE NOT locked here, so don't access the cache object!
        //
        if (errToCatch.bCheckEvent( facCIDLib().strName()
                                    , kCIDErrs::errcCol_NamedNodeNotFound))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            return kCIDLib::False;
        }
        throw;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TCIDNameServerImpl::bQueryScopeKeys(        tCIDLib::TCard4&    c4SerialNum
                                    , const TString&            strScopePath
                                    ,       tCIDLib::TStrList&  colResults
                                    , const tCIDLib::TBoolean   bObjectsOnly
                                    , const tCIDLib::TBoolean   bThrowIfNot)
{
    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    //
    //  This could throw if the node is not in the tree, so lets catch it and
    //  if that's the error, then just return zero.
    //
    try
    {
        //
        //  Make sure the passed name is a non-terminal. And, while we have the
        //  pointer, check the serial number. If the same as the one passed, then
        //  we don't need to do anything.
        //
        const TNSCache::TNode* pnodeSrc = m_colCache.pnodeAt(strScopePath);
        if (!pnodeSrc || (pnodeSrc->eType() != tCIDLib::ETreeNodes::NonTerminal))
        {
            facCIDOrbUC().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbUCErrs::errcNSP_NotAScope
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::TypeMatch
                , strScopePath
            );
        }

        const TNSCache::TNodeNT* pnodeNT = static_cast<const TNSCache::TNodeNT*>(pnodeSrc);
        if (pnodeNT->c4SerialNum() == c4SerialNum)
            return kCIDLib::False;
    }

    catch(TError& errToCatch)
    {
        //
        //  Release the lock before we do this, for efficiency. We don't
        //  need the lock anymore now, so others can get in while we are
        //  off doing error reporting.
        //
        lockCache.Release();

        //
        //  Watch for the node just not existing. If not, we either throw
        //  a scope not found, or just return zero, as told by the caller.
        //  For anything else, just rethrow.
        //
        if (errToCatch.bCheckEvent( facCIDLib().strName()
                                    , kCIDErrs::errcCol_NamedNodeNotFound))
        {
            if (bThrowIfNot)
            {
                facCIDOrbUC().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbUCErrs::errcNSP_ScopeNotFound
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotFound
                    , strScopePath
                );
            }
            return kCIDLib::False;
        }
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    //
    //  Ok, so lets get a cursor for this scope. We can't use a regular
    //  cursor, since we just want to enumerate this scope and nothing more.
    //  So we use a specialized scope cursor.
    //
    TBasicTreeCol<TNSNode>::TScopeCursor cursEnum(&m_colCache, strScopePath);
    if (!cursEnum.bReset())
        return kCIDLib::False;

    tCIDLib::TCard4 c4Count = 0;
    colResults.RemoveAll();
    do
    {
        //
        //  If it's a terminal, or it's ok to add non-objects, then we add this
        //  one.
        //
        if ((cursEnum.eCurType() == tCIDLib::ETreeNodes::Terminal) || !bObjectsOnly)
        {
            colResults.objAdd(cursEnum.strCurName());
            c4Count++;
        }
    }   while (cursEnum.bNext());

    return kCIDLib::True;
}


//
//  Given a list of rebind info objects, we rebind all of them. For each one
//  we return a list of true/false status results. The caller passes in a
//  cookie. If not the same as ours, we return false and give them back the
//  latest one.
//
tCIDLib::TBoolean TCIDNameServerImpl::
bRebindObjs(const   TVector<TNSRebindInfo>&             colBindings
            ,       TFundVector<tCIDOrb::ERebindRes>&   fcolResults
            ,       tCIDLib::TCard8&                    c8CurCookie)
{
    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    // Save the incoming cookie and store ours in the return
    tCIDLib::TCard8 c8InCookie = c8CurCookie;
    c8CurCookie = m_c8Cookie;

    //
    //  If there provided cookie isn't the same as ours, then we are out of
    //  sync, so we return false to force them to update.
    //
    if (c8InCookie != m_c8Cookie)
        return kCIDLib::False;

    fcolResults.RemoveAll();

    //
    //  Loop through the passed objects. If one already exists, we just update
    //  it. If not, we create it.
    //
    tCIDLib::ETreeNodes eType;
    const tCIDLib::TCard4 c4Count = colBindings.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TNSRebindInfo& nsrbiCur = colBindings[c4Index];

        tCIDOrb::ERebindRes eRes = tCIDOrb::ERebindRes::Success;
        try
        {
            if (m_colCache.bNodeExists(nsrbiCur.strFullPath(), eType))
            {
                // Make sure its a terminal node
                if (eType == tCIDLib::ETreeNodes::Terminal)
                {
                    m_colCache.RefreshTerminal
                    (
                        nsrbiCur.strFullPath()
                        , TNSNode
                          (
                            nsrbiCur.ooidSrvObject()
                            , nsrbiCur.strExtra1()
                            , nsrbiCur.strExtra2()
                            , nsrbiCur.strExtra3()
                            , nsrbiCur.strExtra4()
                          )
                        , nsrbiCur.strDescription()
                    );
                }
                 else
                {
                    eRes = tCIDOrb::ERebindRes::NotATerminal;
                }
            }
             else
            {
                //
                //  It doesn't already exist so add it. Make sure the
                //  path exists.
                //
                m_colCache.pnodeCreateNTPath(nsrbiCur.strParentPath(), TString::strEmpty());
                m_colCache.objAddTerminal
                (
                    nsrbiCur.strParentPath()
                    , nsrbiCur.strNodeName()
                    , TNSNode
                      (
                        nsrbiCur.ooidSrvObject()
                        , nsrbiCur.strExtra1()
                        , nsrbiCur.strExtra2()
                        , nsrbiCur.strExtra3()
                        , nsrbiCur.strExtra4()
                      )
                    , nsrbiCur.strDescription()
                );
            }
        }

        catch(TError& errToCatch)
        {
            if (facCIDNameSrv.bLogWarnings() && !errToCatch.bLogged())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
            eRes = tCIDOrb::ERebindRes::Exception;
        }

        catch(...)
        {
            eRes = tCIDOrb::ERebindRes::Exception;
        }

        // Add the result for this one
        fcolResults.c4AddElement(eRes);
    }
    return kCIDLib::True;
}



//
//  Given a list of paths to bindings, renew their leases. We return a list
//  of true/false flags, one per incoming path, to indicate the results of
//  each binding attempt.
//
tCIDLib::TBoolean
TCIDNameServerImpl::bRenewLeases(const  tCIDLib::TStrList&      colPaths
                                ,       tCIDLib::TBoolList&     fcolResults
                                ,       tCIDLib::TCard8&        c8CurCookie)
{
    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    // Save the incoming cookie and store ours in the return
    tCIDLib::TCard8 c8InCookie = c8CurCookie;
    c8CurCookie = m_c8Cookie;

    //
    //  If there provided cookie isn't the same as ours, then we are out of
    //  sync, so we return false to force them to update.
    //
    if (c8InCookie != m_c8Cookie)
        return kCIDLib::False;

    //
    //  Looks ok, so loop through all of the passed paths, look them up and
    //  renew them.
    //
    fcolResults.RemoveAll();

    const tCIDLib::TCard4 c4Count = colPaths.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TNSNode* pnodeRenew = m_colCache.pobjAt(colPaths[c4Index]);
        if (pnodeRenew)
        {
            pnodeRenew->RenewLease();
            fcolResults.c4AddElement(kCIDLib::True);
        }
         else
        {
            fcolResults.c4AddElement(kCIDLib::False);
        }
    }
    return kCIDLib::True;
}


//
//  Like binding exists above but checks for a scope in this case. Returns the name
//  server cookie to allow for simultaneous checks for changes.
//
tCIDLib::TBoolean
TCIDNameServerImpl::bScopeExists(const  TString&            strToCheck
                                ,       tCIDLib::TCard8&    c4CurNSCookie)
{
    tCIDLib::ETreeNodes eType;

    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    // Give back the current cookie. Do it NOW in case we return early
    c4CurNSCookie = m_c8Cookie;

    // If it doesn't exist at all, then obviously we failed
    if (!m_colCache.bNodeExists(strToCheck, eType))
        return kCIDLib::False;

    // And it has to be a non-terminal type
    if (eType != tCIDLib::ETreeNodes::NonTerminal)
        return kCIDLib::False;

    return kCIDLib::True;
}


//
//  Create a new binding. It must not already exist or an error will occur.
//  We have one that takes a full path and another that takes a parent path
//  and child node name. And another that takes a rebinder object.
//
tCIDLib::TVoid
TCIDNameServerImpl::BindObj(const   TString&    strBinding
                            , const TString&    strDescription
                            , const TString&    strExtra1
                            , const TString&    strExtra2
                            , const TString&    strExtra3
                            , const TString&    strExtra4
                            , const TOrbObjId&  ooidToStore)
{
    //
    //  This one accepts a whole path, with scope and binding together. So we
    //  have to pull it apart into a scope and binding name. So look for the
    //  last slash, which should be just before the binding name.
    //
    TString strScope;
    TString strName;
    facCIDOrbUC().ParseNSBindingPath(strBinding, strScope, strName);

    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    // Make sure the scope exists
    m_colCache.pnodeCreateNTPath(strScope, TString::strEmpty());

    // Just pass it on to the tree collection
    m_colCache.objAddTerminal
    (
        strScope
        , strName
        , TNSNode(ooidToStore, strExtra1, strExtra2, strExtra3, strExtra4)
        , strDescription
    );
}

tCIDLib::TVoid
TCIDNameServerImpl::BindObj2(const  TString&    strParentNode
                            , const TString&    strNodeName
                            , const TString&    strDescription
                            , const TString&    strExtra1
                            , const TString&    strExtra2
                            , const TString&    strExtra3
                            , const TString&    strExtra4
                            , const TOrbObjId&  ooidToStore)
{
    // Do basic validation of the scope
    facCIDOrbUC().ValidateNSScopePath(strParentNode);

    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    // Make sure the scope exists
    m_colCache.pnodeCreateNTPath(strParentNode, TString::strEmpty());

    // Just pass it on to the tree collection
    m_colCache.objAddTerminal
    (
        strParentNode
        , strNodeName
        , TNSNode(ooidToStore, strExtra1, strExtra2, strExtra3, strExtra4)
        , strDescription
    );
}

//
//  In this one we get one of the rebinder's objects, which includes within
//  it a name server info object.
//
tCIDLib::TVoid TCIDNameServerImpl::BindObj3(const TNSRebindInfo& nsrbiToBind)
{
    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    // Make sure the scope exists
    m_colCache.pnodeCreateNTPath(nsrbiToBind.strParentPath(), TString::strEmpty());

    // Just pass it on to the tree collection
    m_colCache.objAddTerminal
    (
        nsrbiToBind.strParentPath()
        , nsrbiToBind.strNodeName()
        , TNSNode
          (
            nsrbiToBind.ooidSrvObject()
            , nsrbiToBind.strExtra1()
            , nsrbiToBind.strExtra2()
            , nsrbiToBind.strExtra3()
            , nsrbiToBind.strExtra4()
          )
        , nsrbiToBind.strDescription()
    );
}


//
//  Enuermate bindings in the passed scope, i.e. the remote objects that are available
//  under the passed scope. Use bQueryScopeKeys if you only need the binding names.
//
tCIDLib::TCard4
TCIDNameServerImpl::c4EnumObjects(  const   TString&                strScopeToEnum
                                    ,       TVector<TNameServerInfo>& colResults
                                    , const tCIDLib::TBoolean       bThrowIfNot)
{
    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    //
    //  This could throw if the node is not in the tree, so lets catch it and
    //  if that's the error, then just return zero.
    //
    try
    {
        // Make sure the passed name is a non-terminal
        if (m_colCache.eTypeAt(strScopeToEnum) != tCIDLib::ETreeNodes::NonTerminal)
        {
            facCIDOrbUC().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbUCErrs::errcNSP_NotAScope
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::TypeMatch
                , strScopeToEnum
            );
        }
    }

    catch(TError& errToCatch)
    {
        //
        //  Release the lock before we do this, for efficiency. We don't
        //  need the lock anymore now, so others can get in while we are
        //  off doing error reporting.
        //
        lockCache.Release();

        //
        //  Watch for the node just not existing. If not, we either throw
        //  a scope not found, or just return zero, as told by the caller.
        //  For anything else, just rethrow.
        //
        if (errToCatch.bCheckEvent( facCIDLib().strName()
                                    , kCIDErrs::errcCol_NamedNodeNotFound))
        {
            if (bThrowIfNot)
            {
                facCIDOrbUC().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbUCErrs::errcNSP_ScopeNotFound
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotFound
                    , strScopeToEnum
                );
            }
            return 0;
        }
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    //
    //  Ok, so lets get a cursor for this scope. We can't use a regular
    //  cursor, since we just want to enumerate this scope and nothing more.
    //  So we use a specialized scope cursor.
    //
    TBasicTreeCol<TNSNode>::TScopeCursor cursEnum(&m_colCache, strScopeToEnum);
    if (!cursEnum.bReset())
        return 0;

    tCIDLib::TCard4 c4Count = 0;
    colResults.RemoveAll();
    do
    {
        // If its a terminal, add an item to the result collection for this guy
        if (cursEnum.eCurType() == tCIDLib::ETreeNodes::Terminal)
        {
            const TNSNode& nodeCur = cursEnum.objRCur();
            colResults.objAdd
            (
                TNameServerInfo
                (
                    nodeCur.ooidThis()
                    , cursEnum.strCurName()
                    , cursEnum.strCurDescription()
                    , nodeCur.strExtra1()
                    , nodeCur.strExtra2()
                    , nodeCur.strExtra3()
                    , nodeCur.strExtra4()
                )
            );
            c4Count++;
        }
    }   while (cursEnum.bNext());
    return c4Count;
}


//
//  Return a key/value pair list for all of the scopes under the indicated scope.
//  The hey is the name and the value is the description.
//
tCIDLib::TCard4
TCIDNameServerImpl::c4EnumSubScopes(const   TString&            strScopeToEnum
                                    ,       tCIDLib::TKVPList&  colResults
                                    , const tCIDLib::TBoolean   bThrowIfNot)
{
    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    // Make sure the passed name is a non-terminal
    try
    {
        if (m_colCache.eTypeAt(strScopeToEnum) != tCIDLib::ETreeNodes::NonTerminal)
        {
            facCIDOrbUC().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbUCErrs::errcNSP_NotAScope
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::TypeMatch
                , strScopeToEnum
            );
        }
    }

    catch(TError& errToCatch)
    {
        //
        //  Release the lock before we do this, for efficiency. We don't
        //  need it now, and others can get in while we are off doing
        //  error reporting.
        //
        lockCache.Release();

        //
        //  Watch for the node just not existing. If not, we either throw
        //  a scope not found, or just return zero, as told by the caller.
        //  For anything else, just rethrow.
        //
        if (errToCatch.bCheckEvent( facCIDLib().strName()
                                    , kCIDErrs::errcCol_NamedNodeNotFound))
        {
            if (bThrowIfNot)
            {
                facCIDOrbUC().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kOrbUCErrs::errcNSP_ScopeNotFound
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotFound
                    , strScopeToEnum
                );
            }
            return 0;
        }

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    //
    //  Ok, so lets get a cursor for this scope. We can't use a regular
    //  cursor, since we just want to enumerate this scope and nothing more.
    //  So we use a specialized scope cursor.
    //
    TBasicTreeCol<TNSNode>::TScopeCursor cursEnum(&m_colCache, strScopeToEnum);
    if (!cursEnum.bReset())
        return 0;

    tCIDLib::TCard4 c4Count = 0;
    do
    {
        //
        //  If its a non-terminal, add an item to the result collection for
        //  this guy.
        //
        if (cursEnum.eCurType() == tCIDLib::ETreeNodes::NonTerminal)
        {
            colResults.objAdd
            (
                TKeyValuePair(cursEnum.strCurName(), cursEnum.strCurDescription())
            );
            c4Count++;
        }
    }   while (cursEnum.bNext());

    return c4Count;
}


tCIDLib::TCard8 TCIDNameServerImpl::c8QueryCookie()
{
    return m_c8Cookie;
}


tCIDLib::TVoid TCIDNameServerImpl::ClearScope(const TString& strToClear)
{
    // If its the root node, then that's an error
    if (strToClear == kCIDLib::pszTreeSepChar)
    {
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcNSP_CantRemoveRoot
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    // See if exists, and what type it is
    tCIDLib::ETreeNodes eType;
    if (!m_colCache.bNodeExists(strToClear, eType))
    {
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcNSP_ScopeNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
            , strToClear
        );
    }

    // Make sure its a non-terminal node
    if (eType != tCIDLib::ETreeNodes::NonTerminal)
    {
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcNSP_NotAScope
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
            , strToClear
        );
    }

    // Looks reasonable, so ask the cache to clear its contents
    m_colCache.RemoveNode(strToClear);
}


tCIDLib::TVoid
TCIDNameServerImpl::CreateScope(const   TString&    strParentNode
                                , const TString&    strNodeName
                                , const TString&    strDescription)
{
    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    // Just pass it on to the tree collection
    m_colCache.pnodeAddNonTerminal(strParentNode, strNodeName, strDescription);
}


tCIDLib::TVoid
TCIDNameServerImpl::CreateScopePath(const   TString&    strScopePath
                                    , const TString&    strDescription)
{
    // Make sure the path is basically well formed
    facCIDOrbUC().ValidateNSScopePath(strScopePath);

    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    // Just pass this on to the cache
    m_colCache.pnodeCreateNTPath(strScopePath, strDescription);
}

tCIDLib::TVoid
TCIDNameServerImpl::CreateScopePath(const tCIDLib::TKVPList& colPathParts)
{
    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    // Just pass this on to the cache
    m_colCache.pnodeCreateNTPath(colPathParts);
}


tCIDLib::TVoid
TCIDNameServerImpl::Dump(       TString&                strToFill
                        , const tCIDOrbUC::EDumpFmts eFormat)
{
    // Create a text string stream for the passed string
    strToFill.Clear();
    TTextStringOutStream strmFill(&strToFill);

    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    if (eFormat == tCIDOrbUC::EDumpFmts::PlainText)
    {
        RecursiveDumpText(m_colCache.pnodeRoot(), strmFill, 0);
    }
     else if (eFormat == tCIDOrbUC::EDumpFmts::XML)
    {
        // Output the XMLDecl, then do the recursive generation
        tCIDLib::TCh szTmp[2];
        szTmp[0] = kCIDLib::chUniBOM;
        szTmp[1] = kCIDLib::chNull;
        strmFill    << szTmp
                    << L"<?xml version='1.0'?>\n";

        RecursiveDumpXML(m_colCache.pnodeRoot(), strmFill, 0);
    }
     else
    {
        #if CID_DEBUG_ON
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcNSP_UnknownDumpFormat
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
            , TInteger(tCIDLib::i4EnumOrd(eFormat))
        );
        #endif
    }

    //
    //  Flush it to insure the data is in the string before we close
    //  down the stream.
    //
    strmFill << kCIDLib::FlushIt;
}


//
//  Query info about the passed terminal or non-terminal node. It must exist
//  or an exception occurs.
//
tCIDLib::TVoid
TCIDNameServerImpl::QueryNameInfo(  const   TString&    strNameToQuery
                                    ,       TString&    strName
                                    ,       TString&    strDescr)
{
    try
    {
        // Lock the cache and do the query
        TMtxLocker lockCache(&m_mtxSync);
        m_colCache.QueryNameAndDesc(strNameToQuery, strName, strDescr);
    }

    catch(TError& errToCatch)
    {
        //
        //  If its just "node not found", then translate into a more
        //  meaningful name server error.
        //
        //  WE ARE NOT locked here, so don't access the tree collection!
        //
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        if (errToCatch.bCheckEvent( facCIDLib().strName()
                                    , kCIDErrs::errcCol_NamedNodeNotFound))
        {
            if (facCIDNameSrv.bLogWarnings() && !errToCatch.bLogged())
                TModule::LogEventObj(errToCatch);

            facCIDOrbUC().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbUCErrs::errcNSP_NoPathInfo
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strNameToQuery
            );
        }
        throw;
    }
}

tCIDLib::TVoid
TCIDNameServerImpl::QueryNameInfo2( const   TString&    strNameToQuery
                                    ,       TString&    strName
                                    ,       TString&    strDescr
                                    ,       TString&    strExtra1
                                    ,       TString&    strExtra2
                                    ,       TString&    strExtra3
                                    ,       TString&    strExtra4)
{
    try
    {
        // Lock the cache and do the query
        TMtxLocker lockCache(&m_mtxSync);
        const TNSNode& nodeInfo = m_colCache.objAt(strNameToQuery, strName, strDescr);

        // The extra user values are in the node data
        strExtra1 = nodeInfo.strExtra1();
        strExtra2 = nodeInfo.strExtra2();
        strExtra3 = nodeInfo.strExtra3();
        strExtra4 = nodeInfo.strExtra4();
    }

    catch(TError& errToCatch)
    {
        //
        //  If its just "node not found", then translate into a more
        //  meaningful name server error.
        //
        //  WE ARE NOT locked here, so don't access the tree collection!
        //
        if (errToCatch.bCheckEvent( facCIDLib().strName()
                                    , kCIDErrs::errcCol_NamedNodeNotFound))
        {
            facCIDOrbUC().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbUCErrs::errcNSP_NoPathInfo
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strNameToQuery
            );
        }
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}


//
//  Remove the passed binding. We can optionally throw if it doesn't exist or
//  just do nothing.
//
tCIDLib::TVoid
TCIDNameServerImpl::RemoveBinding(  const   TString&            strToRemove
                                    , const tCIDLib::TBoolean   bThrowIfNot)
{
    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    // See if exists, and what type it is
    tCIDLib::ETreeNodes eType;
    if (!m_colCache.bNodeExists(strToRemove, eType))
    {
        if (bThrowIfNot)
        {
            facCIDOrbUC().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kOrbUCErrs::errcNSP_BindingNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::TypeMatch
                , strToRemove
            );
        }
        return;
    }

    // Make sure its a terminal node
    if (eType != tCIDLib::ETreeNodes::Terminal)
    {
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcNSP_NotABinding
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
            , strToRemove
        );
    }

    // Looks reasonable, so ask the cach to remove it
    m_colCache.RemoveNode(strToRemove);
}


tCIDLib::TVoid TCIDNameServerImpl::RemoveScope(const TString& strToRemove)
{
    // If its the root node, then that's an error
    if (strToRemove == kCIDLib::pszTreeSepChar)
    {
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcNSP_CantRemoveRoot
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    // See if exists, and what type it is
    tCIDLib::ETreeNodes eType;
    if (!m_colCache.bNodeExists(strToRemove, eType))
    {
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcNSP_ScopeNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
            , strToRemove
        );
    }

    // Make sure its a non-terminal node
    if (eType != tCIDLib::ETreeNodes::NonTerminal)
    {
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcNSP_NotAScope
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
            , strToRemove
        );
    }

    // Looks reasonable, so ask the cach to remove it
    m_colCache.RemoveNode(strToRemove);
}


//
//  Allows a client to update one of the extra values associated with a
//  binding, without having to send the whole thing.
//
tCIDLib::TVoid
TCIDNameServerImpl::UpdateExtraVal( const   TString&        strBinding
                                    , const tCIDLib::TCard4 c4Index
                                    , const TString&        strNewValue)
{
    // Lock the cache
    TMtxLocker lockCache(&m_mtxSync);

    //
    //  This will return us a pointer to the node if it exists. It will
    //  throw if not a terminal, which is what we want.
    //
    TNSNode* pnodeUpdate = m_colCache.pobjAt(strBinding);
    if (!pnodeUpdate)
    {
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcNSP_ScopeNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
            , strBinding
        );
    }

    // See which value we need to update
    if (c4Index == 1)
        pnodeUpdate->strExtra1(strNewValue);
    else if (c4Index == 2)
        pnodeUpdate->strExtra2(strNewValue);
    else if (c4Index == 3)
        pnodeUpdate->strExtra3(strNewValue);
    else if (c4Index == 4)
        pnodeUpdate->strExtra4(strNewValue);
    else
    {
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcNSP_BadExtraValInd
            , strBinding
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4Index)
        );
    }
}


// ---------------------------------------------------------------------------
//  TCIDNameServerImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is for the name server's internal use. If it wants to put any special
//  nodes in the cache and insure that they stay there without having to have
//  their lease constantly renewed, it can mark them as permanent and the
//  lease checker will skip it.
//
//  It's only called at startup before any clients can get in, so no need
//  for sync.
//
//  Note that it only works for bindings, not scopes, since scopes don't have
//  a node object, only the actual registered binding objects. So objAt below
//  will throw if it's not a binding.
//
tCIDLib::TVoid TCIDNameServerImpl::MarkPermanent(const TString& strBinding)
{
    TNSNode& nodeAt = m_colCache.objAt(strBinding);
    nodeAt.bPermanent(kCIDLib::True);
}


// ---------------------------------------------------------------------------
//  TCIDNameServerImpl: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDNameServerImpl::Initialize()
{
    //
    //  Since almost everyone will use or look for a certain core set of
    //  scopes, we'll pre-add them, so that every app doesn't have to make
    //  sure that they are already created.
    //
    //  They are:
    //
    //      /CIDLib
    //      /CIDLib/CIDLogSrv
    //
    TMtxLocker lockCache(&m_mtxSync);

    m_colCache.pnodeAddNonTerminal
    (
        kCIDLib::pszTreeSepChar, L"CIDLib", L"CIDLib Scope"
    );

    m_colCache.pnodeAddNonTerminal
    (
        L"/CIDLib", L"CIDLogSrv", L"CIDLib Log Server Scope"
    );

    //
    //  Start the landlord thread unless leasing was supressed on the
    //  command line.
    //
    if (!m_bNoLease)
        m_thrLandlord.Start();
}


tCIDLib::TVoid TCIDNameServerImpl::Terminate()
{
    // Stop the landlord thread
    try
    {
        m_thrLandlord.ReqShutdownSync(4000);
        m_thrLandlord.eWaitForDeath(5000);
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  TCIDNameServerImpl: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This thread just periodically runs through all of the name server bindings
//  and sees if any have exceeded their lease, i.e. haven't been renewed
//  within the required period of time. If so, they are removed.
//
//  Note that it won't remove any of our own name server objects since we
//  marked them permanent when we put them into the cache.
//
tCIDLib::EExitCodes
TCIDNameServerImpl::eLandlordThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let the starting thread go
    thrThis.Sync();

    //
    //  If by some horrible chance our mutex gets stuck, we don't want
    //  this thread to hang. So we use a timeout on the lock below. If
    //  it fails we want to log something. But we aren't blocking for long
    //  so we can't log every time it failed, since we'd probably just
    //  sit there in a loop logging eveyr second or two. So we us a time
    //  stamp to throttle that. We want it to log the first time, so we
    //  start it at zero.
    //
    //  We could use the fact that we are sleeping each round, but don't
    //  want to tie the rate of logging to that, in case it's later
    //  changed and because it's a bit too fast now.
    //
    const tCIDLib::TEncodedTime enctLogSecsThresh(45 * kCIDLib::enctOneSecond);
    tCIDLib::TEncodedTime enctNextLog = 0;

    while (kCIDLib::True)
    {
        //
        //  Sleep for up to 15 seconds. We use the form of sleep that will
        //  wake up on a shutdown request, so we won't hold up shutdown due
        //  to such a long sleep. If it returns false, we didn't sleep the
        //  whole time because we were asked to shutdown.
        //
        if (!thrThis.bSleep(15000))
            break;

        //
        //  Lock and run through all the bindings, tossing out those whose
        //  lease is up.
        //
        try
        {
            //
            //  We use a locker with a timeout. If, by some horrible chance,
            //  the lock get's hung up, we'll not hang. In the normal course
            //  of events the cache is never locked more than fractions of
            //  a millisecond.
            //
            TMtxLocker lockCache(&m_mtxSync, 2000UL);

            //
            //  Calculate the cutoff time, which would be now minus the
            //  binding lease time. Anything older than that is outa here.
            //  Then kick off the cursive operation that does the work.
            //
            const tCIDLib::TEncodedTime enctNow = TTime::enctNow();
            RecursiveLeaseCheck(m_colCache.pnodeRoot(), enctNow);
        }

        catch(TError& errToCatch)
        {
            // See if it's time to do another log message
            const tCIDLib::TEncodedTime enctCur = TTime::enctNow();
            if (enctCur > enctNextLog)
            {
                // Reset for the next time
                enctNextLog = enctCur + enctLogSecsThresh;

                // If its not been logged, then log it
                if (!errToCatch.bLogged())
                {
                    errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                    TModule::LogEventObj(errToCatch);
                }

                facCIDNameSrv.LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kNSMsgs::midStatus_CIDLibExcept
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::AppStatus
                    , facCIDNameSrv.strMsg(kNSMsgs::midStatus_RestartingLL)
                );
            }
        }

        catch(...)
        {
            // See if it's time to do another log message
            const tCIDLib::TEncodedTime enctCur = TTime::enctNow();
            if (enctCur > enctNextLog)
            {
                // Reset for the next time
                enctNextLog = enctCur + enctLogSecsThresh;

                facCIDNameSrv.LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kNSMsgs::midStatus_UnknownExcept
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::AppStatus
                    , facCIDNameSrv.strMsg(kNSMsgs::midStatus_RestartingLL)
                );
            }
        }
    }
    return tCIDLib::EExitCodes::Normal;
}


tCIDLib::TVoid
TCIDNameServerImpl::RecursiveDumpText(  TBasicTreeCol<TNSNode>::TNodeNT* const pnodeParent
                                        ,       TTextOutStream&         strmOut
                                        , const tCIDLib::TCard4         c4Depth)
{
    // Output this scope's start tab
    strmOut << TTextOutStream::Spaces(c4Depth * 3)
            << L"Scope=" << pnodeParent->strName()
            << L"  '" << pnodeParent->strDescription() << L"'\n"
            << TTextOutStream::Spaces(c4Depth * 3)
            << L"{\n";

    // Do this node's children, if any
    TBasicTreeCol<TNSNode>::TNode* pnodeChild = pnodeParent->pnodeFirstChild();
    while (pnodeChild)
    {
        if (pnodeChild->eType() == tCIDLib::ETreeNodes::NonTerminal)
        {
            // Just recurse on this non-terminal node, with a depth of one more
            RecursiveDumpText
            (
                static_cast<TBasicTreeCol<TNSNode>::TNodeNT*>(pnodeChild)
                , strmOut
                , c4Depth + 1
            );
        }
         else
        {
            const tCIDLib::TCard4 c4Indent = (c4Depth + 1) * 3;

            // Just dump this binding
            strmOut << TTextOutStream::Spaces(c4Indent)
                    << L"Binding=" << pnodeChild->strName()
                    << L"  '" << pnodeChild->strDescription() << L"'\n";

            const TNSNode& nodeCur
            (
                static_cast<TBasicTreeCol<TNSNode>::TNodeT*>(pnodeChild)->objData()
            );

            // If any extra values, display them
            if (!nodeCur.strExtra1().bIsEmpty())
            {
                strmOut << TTextOutStream::Spaces(c4Indent + 3)
                        << L"Extra1=" << nodeCur.strExtra1() << kCIDLib::NewLn;
            }

            if (!nodeCur.strExtra2().bIsEmpty())
            {
                strmOut << TTextOutStream::Spaces(c4Indent + 3)
                        << L"Extra2=" << nodeCur.strExtra2() << kCIDLib::NewLn;
            }

            if (!nodeCur.strExtra3().bIsEmpty())
            {
                strmOut << TTextOutStream::Spaces(c4Indent + 3)
                        << L"Extra3=" << nodeCur.strExtra3() << kCIDLib::NewLn;
            }

            if (!nodeCur.strExtra4().bIsEmpty())
            {
                strmOut << TTextOutStream::Spaces(c4Indent + 3)
                        << L"Extra4=" << nodeCur.strExtra4() << kCIDLib::NewLn;
            }
        }
        pnodeChild = pnodeChild->pnodeNext();
    }

    strmOut << TTextOutStream::Spaces(c4Depth * 3)
            << L"}\n";
}


tCIDLib::TVoid
TCIDNameServerImpl::RecursiveDumpXML(TBasicTreeCol<TNSNode>::TNodeNT* const pnodeParent
                                    ,       TTextOutStream&         strmOut
                                    , const tCIDLib::TCard4         c4Depth)
{
    // Output this scope's start tab
    strmOut << TTextOutStream::Spaces(c4Depth * 3)
            << L"<CIDNS:Scope Name='" << pnodeParent->strName() << L"'";

    // If this is the root, then we have to define our namespace
    if (!c4Depth)
    {
        strmOut << L" xmlns:CIDNS='http://www.charmedquark.com"
                << TCIDLogSrvClientProxy::strScope << L"'";
    }

    const tCIDLib::TCard4 c4Indent = (c4Depth + 1) * 3;
    strmOut << L">\n"
            << TTextOutStream::Spaces(c4Indent)
            << L"<CIDNS:Desc>\n"
            << TTextOutStream::Spaces(c4Indent + 3)
            << pnodeParent->strDescription() << kCIDLib::NewLn
            << TTextOutStream::Spaces(c4Indent)
            << L"</CIDNS:Desc>\n";

    // Do this node's children, if any
    TBasicTreeCol<TNSNode>::TNode* pnodeChild = pnodeParent->pnodeFirstChild();
    while (pnodeChild)
    {
        if (pnodeChild->eType() == tCIDLib::ETreeNodes::NonTerminal)
        {
            // Just recurse on this non-terminal node, with a depth of one more
            RecursiveDumpXML
            (
                static_cast<TBasicTreeCol<TNSNode>::TNodeNT*>(pnodeChild)
                , strmOut
                , c4Depth + 1
            );
        }
         else
        {
            // Just dump this binding
            strmOut << TTextOutStream::Spaces(c4Indent)
                    << L"<CIDNS:Binding Name='"
                    << pnodeChild->strName() << L"'>\n"
                    << TTextOutStream::Spaces(c4Indent + 3)
                    << L"<CIDNS:Desc>\n"
                    << TTextOutStream::Spaces(c4Indent + 6)
                    << pnodeChild->strDescription() << kCIDLib::NewLn
                    << TTextOutStream::Spaces(c4Indent + 3)
                    << L"</CIDNS:Desc>\n"
                    << TTextOutStream::Spaces(c4Indent)
                    << L"</CIDNS:Binding>\n";
        }
        pnodeChild = pnodeChild->pnodeNext();
    }

    // And close out this scope node
    strmOut << TTextOutStream::Spaces(c4Depth * 3) << L"</CIDNS:Scope>\n";
}


tCIDLib::TVoid TCIDNameServerImpl::
RecursiveLeaseCheck(        TBasicTreeCol<TNSNode>::TNodeNT* const  pnodeParent
                    , const tCIDLib::TEncodedTime                   enctNow)
{
    // Do this node's children, if any
    TBasicTreeCol<TNSNode>::TNode* pnodeChild = pnodeParent->pnodeFirstChild();
    while (pnodeChild)
    {
        if (pnodeChild->eType() == tCIDLib::ETreeNodes::NonTerminal)
        {
            RecursiveLeaseCheck
            (
                static_cast<TBasicTreeCol<TNSNode>::TNodeNT*>(pnodeChild), enctNow
            );

            // And move forward
            pnodeChild = pnodeChild->pnodeNext();
        }
         else
        {
            // Get a ref to our own node object out of the tree node
            TNSNode& nodeCur = static_cast<TBasicTreeCol<TNSNode>::TNodeT*>
            (
                pnodeChild
            )->objData();

            //
            //  Note that, if this node is marked as permanent, this will
            //  always return false and we'll never remove it.
            //
            if (nodeCur.bLeaseIsUp(enctNow))
            {
                // Log that we are ejecting this one
                TString strPath;
                pnodeChild->BuildFullName(strPath);

                if (facCIDNameSrv.bLogWarnings())
                {
                    facCIDNameSrv.LogMsg
                    (
                        CID_FILE
                        , CID_LINE
                        , kNSMsgs::midStatus_EjectedBinding
                        , tCIDLib::ESeverities::Warn
                        , tCIDLib::EErrClasses::Timeout
                        , strPath
                    );
                }

                TBasicTreeCol<TNSNode>::TNode* pnodeTmp = pnodeChild->pnodeNext();
                pnodeParent->RemoveChild(pnodeChild);
                pnodeChild = pnodeTmp;
            }
             else
            {
                pnodeChild = pnodeChild->pnodeNext();
            }
        }
    }
}

