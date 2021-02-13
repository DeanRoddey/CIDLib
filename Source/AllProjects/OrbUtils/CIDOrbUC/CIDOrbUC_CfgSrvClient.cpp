//
// FILE NAME: CIDOrbUC_CfgServerClient.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/03/2003
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
//  This file implements the TOrbAutoRebind class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------
#include "CIDOrbUC_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCfgServerClient,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TCfgServerClient
//  PREFIX: cfgsc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCfgServerClient: Constructors and Destructor
// ---------------------------------------------------------------------------
TCfgServerClient::TCfgServerClient(const tCIDLib::TCard4 c4ConnWait) :

    m_c4ConnWait(c4ConnWait)
    , m_mbufIO(8192, 0x100000)
    , m_mbufIO2(8192, 0x100000)
    , m_pcrypToUse(nullptr)
    , m_porbcProxy(nullptr)
    , m_strmOut(&m_mbufIO, tCIDLib::EAdoptOpts::NoAdopt)
    , m_strmIn(m_strmOut)
    , m_strNSScope(TCIDCfgSrvClientProxy::strBinding)
{
    // Put our host name into the local host cfg server binding template
    m_strNSScope.eReplaceToken(facCIDSock().strIPHostName(), L'h');
}

TCfgServerClient::TCfgServerClient( const   TString&        strNSScope
                                    , const tCIDLib::TCard4 c4ConnWait) :

    m_c4ConnWait(c4ConnWait)
    , m_mbufIO(8192, 0x100000)
    , m_mbufIO2(8192, 0x100000)
    , m_pcrypToUse(nullptr)
    , m_porbcProxy(nullptr)
    , m_strmOut(&m_mbufIO, tCIDLib::EAdoptOpts::NoAdopt)
    , m_strmIn(m_strmOut)
    , m_strNSScope(strNSScope)
{
}

TCfgServerClient::~TCfgServerClient()
{
    try
    {
        delete m_porbcProxy;
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }

    // Clean up the encrypter if we got one
    delete m_pcrypToUse;
    m_pcrypToUse = nullptr;
}


// ---------------------------------------------------------------------------
//  TCfgServerClient: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TCfgServerClient::AddEncryptedScope(const TString& strToAdd)
{
    m_colEncryptedScopes.objAdd(strToAdd);
}


tCIDLib::TVoid
TCfgServerClient::AddObject(const   TString&        strKey
                            , const MStreamable&    strmblToAdd
                            , const tCIDLib::TCard4 c4Reserve)
{
    try
    {
        m_strmOut.Reset();
        m_strmOut << strmblToAdd << kCIDLib::FlushIt;

        //
        //  If we have an encrypter and this key is in the list, then we have
        //  to encrypt it.
        //
        if (m_pcrypToUse && bIsEncryptedScope(strKey))
        {
            const tCIDLib::TCard4 c4Count = m_pcrypToUse->c4Encrypt
            (
                m_mbufIO, m_mbufIO2, m_strmOut.c4CurPos()
            );
            orbcProxy().AddObject(strKey, c4Count, m_mbufIO2, c4Reserve);
        }
         else
        {
            orbcProxy().AddObject
            (
                strKey
                , m_strmOut.c4CurPos()
                , m_mbufIO
                , c4Reserve
            );
        }
    }

    catch(TError& errToCatch)
    {
        CheckLostProxy(errToCatch);

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}


tCIDLib::TBoolean
TCfgServerClient::bAddOrUpdate( const   TString&            strKey
                                ,       tCIDLib::TCard4&    c4Version
                                , const MStreamable&        strmblToAdd
                                , const tCIDLib::TCard4     c4Reserve)
{
    tCIDLib::TBoolean bRet = kCIDLib::False;
    try
    {
        m_strmOut.Reset();
        m_strmOut << strmblToAdd << kCIDLib::FlushIt;

        //
        //  If we have an encrypter and this key is in the list, then we have
        //  to encrypt it.
        //
        if (m_pcrypToUse && bIsEncryptedScope(strKey))
        {
            const tCIDLib::TCard4 c4Count = m_pcrypToUse->c4Encrypt
            (
                m_mbufIO, m_mbufIO2, m_strmOut.c4CurPos()
            );
            bRet = orbcProxy().bAddOrUpdate(strKey, c4Version, c4Count, m_mbufIO2, c4Reserve);
        }
         else
        {
            bRet = orbcProxy().bAddOrUpdate
            (
                strKey, c4Version, m_strmOut.c4CurPos(), m_mbufIO, c4Reserve
            );
        }
    }

    catch(TError& errToCatch)
    {
        CheckLostProxy(errToCatch);

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return bRet;
}


tCIDLib::TBoolean
TCfgServerClient::bAllObjectsUnder( const   TString&            strStartScope
                                    ,       tCIDLib::TStrList&  colToFill)
{
    tCIDLib::TBoolean bRet;
    colToFill.RemoveAll();
    try
    {
        bRet = orbcProxy().bAllObjectsUnder(strStartScope, colToFill);
    }

    catch(TError& errToCatch)
    {
        CheckLostProxy(errToCatch);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return bRet;
}


tCIDLib::TBoolean TCfgServerClient::bDeleteObjectIfExists(const TString& strKey)
{
    tCIDLib::TBoolean bRet;
    try
    {
        bRet = orbcProxy().bDeleteObjectIfExists(strKey);
    }

    catch(TError& errToCatch)
    {
        CheckLostProxy(errToCatch);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return bRet;
}


tCIDLib::TBoolean TCfgServerClient::bIsConnected()
{
    //
    //  If we have no proxy, or we do but it reports the connection is lost,
    //  then say we are not connected.
    //
    if (!m_porbcProxy || m_porbcProxy->bCheckForLostConnection())
        return kCIDLib::False;

    return kCIDLib::True;
}


tCIDLib::TBoolean TCfgServerClient::bKeyExists(const TString& strKey)
{
    tCIDLib::TBoolean bRet;
    try
    {
        bRet = orbcProxy().bKeyExists(strKey);
    }

    catch(TError& errToCatch)
    {
        CheckLostProxy(errToCatch);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return bRet;
}


tCIDLib::TBoolean
TCfgServerClient::bFindNameUnder(const  TString&            strName
                                , const TString&            strStartScope
                                ,       tCIDLib::TStrList&  colToFill)
{
    colToFill.RemoveAll();
    tCIDLib::TBoolean bRet;
    try
    {
        bRet = orbcProxy().bFindNameUnder(strName, strStartScope, colToFill);
    }

    catch(TError& errToCatch)
    {
        CheckLostProxy(errToCatch);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return bRet;
}


tCIDLib::TBoolean
TCfgServerClient::bReadObject(  const   TString&            strKey
                                ,       MStreamable&        strmblToFill
                                ,       tCIDLib::TCard4&    c4Version)
{
    tCIDLib::TBoolean bNewData = kCIDLib::False;
    try
    {
        //
        //  If we have an encrypter and this key is in the encrypted scopes,
        //  then we have to read it into a temp buffer first and then decode
        //  it out.
        //
        tCIDLib::TCard4 c4BytesRet = 0;
        m_strmIn.Reset();
        if (m_pcrypToUse && bIsEncryptedScope(strKey))
        {
            tCIDLib::TCard4 c4Count;
            bNewData = orbcProxy().bReadObject(strKey, c4Count, m_mbufIO2, c4Version);
            if (bNewData)
                c4BytesRet = m_pcrypToUse->c4Decrypt(m_mbufIO2, m_mbufIO, c4Count);
        }
         else
        {
            bNewData = orbcProxy().bReadObject(strKey, c4BytesRet, m_mbufIO, c4Version);
        }

        // If we got data, then stream it in
        if (bNewData)
        {
            m_strmIn.SetEndIndex(c4BytesRet);
            m_strmIn >> strmblToFill;
        }
    }

    catch(TError& errToCatch)
    {
        CheckLostProxy(errToCatch);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return bNewData;
}


tCIDLib::TCard4
TCfgServerClient::c4QueryObjectsInScope(const   TString&            strParScope
                                        ,       TVector<TString>&   colToFill)
{
    tCIDLib::TCard4 c4Ret;
    try
    {
        c4Ret = orbcProxy().c4QueryObjectsInScope(strParScope, colToFill);
    }

    catch(TError& errToCatch)
    {
        CheckLostProxy(errToCatch);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return c4Ret;
}


tCIDLib::TCard4
TCfgServerClient::c4QuerySubScopes( const   TString&            strParScope
                                    ,       TVector<TString>&   colToFill)
{
    tCIDLib::TCard4 c4Ret;
    try
    {
        c4Ret = orbcProxy().c4QuerySubScopes(strParScope, colToFill);
    }

    catch(TError& errToCatch)
    {
        CheckLostProxy(errToCatch);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return c4Ret;
}


tCIDLib::TCard4
TCfgServerClient::c4UpdateObject(const  TString&        strKey
                                , const MStreamable&    strmblToUpdate)
{
    tCIDLib::TCard4 c4Ret = 0;
    try
    {
        m_strmOut.Reset();
        m_strmOut << strmblToUpdate << kCIDLib::FlushIt;

        //
        //  If we have an encrypter and this key is in the list, then we have
        //  to encrypt it.
        //
        if (m_pcrypToUse && bIsEncryptedScope(strKey))
        {
            const tCIDLib::TCard4 c4Count = m_pcrypToUse->c4Encrypt
            (
                m_mbufIO, m_mbufIO2, m_strmOut.c4CurPos()
            );
            c4Ret = orbcProxy().c4UpdateObject(strKey, c4Count, m_mbufIO2);
        }
         else
        {
            c4Ret = orbcProxy().c4UpdateObject(strKey, m_strmOut.c4CurPos(), m_mbufIO);
        }

    }

    catch(TError& errToCatch)
    {
        CheckLostProxy(errToCatch);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return c4Ret;
}


tCIDLib::TVoid TCfgServerClient::DeleteObject(const TString& strKey)
{
    try
    {
        orbcProxy().DeleteObject(strKey);
    }

    catch(TError& errToCatch)
    {
        CheckLostProxy(errToCatch);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}


tCIDLib::TVoid TCfgServerClient::DeleteScope(const TString& strScope)
{
    try
    {
        orbcProxy().DeleteScope(strScope);
    }

    catch(TError& errToCatch)
    {
        CheckLostProxy(errToCatch);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}


tCIDLib::TVoid TCfgServerClient::DropProxy()
{
    try
    {
        delete m_porbcProxy;
        m_porbcProxy = 0;
    }

    catch(TError& errToCatch)
    {
        m_porbcProxy = 0;
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}


tCIDLib::ELoadRes
TCfgServerClient::eReadObject(  const   TString&            strKey
                                ,       MStreamable&        strmblToFill
                                ,       tCIDLib::TCard4&    c4Version
                                , const tCIDLib::TBoolean   bThrowIfNot)
{
    tCIDLib::ELoadRes eRes = tCIDLib::ELoadRes::NotFound;
    try
    {
        //
        //  If we have an encrypter and this key is in the encrypted scopes,
        //  then we have to read it into a temp buffer first and then decode
        //  it out.
        //
        tCIDLib::TCard4 c4BytesRet = 0;
        m_strmIn.Reset();
        if (m_pcrypToUse && bIsEncryptedScope(strKey))
        {
            tCIDLib::TCard4 c4Count;
            eRes = orbcProxy().eReadObject(strKey, c4Count, m_mbufIO2, c4Version, bThrowIfNot);

            // If we got data, then decrypt it
            if (eRes == tCIDLib::ELoadRes::NewData)
                c4BytesRet = m_pcrypToUse->c4Decrypt(m_mbufIO2, m_mbufIO, c4Count);
        }
         else
        {
            eRes = orbcProxy().eReadObject(strKey, c4BytesRet, m_mbufIO, c4Version, bThrowIfNot);
        }

        // If we got data, then stream it in
        if (eRes == tCIDLib::ELoadRes::NewData)
        {
            m_strmIn.SetEndIndex(c4BytesRet);
            m_strmIn >> strmblToFill;
        }
    }

    catch(TError& errToCatch)
    {
        CheckLostProxy(errToCatch);
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return eRes;
}


// Set an encrypter to handle encrypted scopes
tCIDLib::TVoid
TCfgServerClient::SetEncrypter(TBlockEncrypter* const pcrypToAdopt)
{
    delete m_pcrypToUse;
    m_pcrypToUse = pcrypToAdopt;
}



// ---------------------------------------------------------------------------
//  TCfgServerClient: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  A helper to tell us if the passed scope is one that is in our list of
//  encrypted scopes.
//
tCIDLib::TBoolean
TCfgServerClient::bIsEncryptedScope(const TString& strToCheck) const
{
    const tCIDLib::TCard4 c4Count = m_colEncryptedScopes.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (strToCheck.bStartsWith(m_colEncryptedScopes[c4Index]))
            return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  If we get an error back in any of the public methods, this is called to
//  see if we've lost our proxy connection. If so, we delete the proxy and
//  zero it out.
//
tCIDLib::TVoid TCfgServerClient::CheckLostProxy(const TError& errToCheck)
{
    if (m_porbcProxy && m_porbcProxy->bCheckForLostConnection(errToCheck))
    {
        delete m_porbcProxy;
        m_porbcProxy = 0;
    }
}


//
//  This method is used internally to get access to the client proxy. This guy
//  handles faulting in the client proxy, and also catching errors and watching
//  for the connection being lost, in which case it will try to reconnect before
//  giving up. We deal with object ids stranded in the name server by keeping
//  the last failed object id and not bothering if we get it again.
//
TCIDCfgSrvClientProxy& TCfgServerClient::orbcProxy()
{
    // If we already have the proxy, return it
    if (m_porbcProxy)
        return *m_porbcProxy;

    m_porbcProxy = facCIDOrbUC().porbcMakeClient<TCIDCfgSrvClientProxy>
    (
        m_strNSScope, m_c4ConnWait
    );

    if (!m_porbcProxy)
    {
        facCIDOrbUC().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kOrbUCErrs::errcSrv_CSNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }
    return *m_porbcProxy;
}


