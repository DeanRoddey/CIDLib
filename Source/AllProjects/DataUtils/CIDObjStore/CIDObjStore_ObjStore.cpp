//
// FILE NAME: CIDObjStore_ObjStore.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/16/2002
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
//  This file implements the core class for the object store. It is just a
//  public wrapper for a private implementation object that does all of the
//  real work.
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
#include    "CIDObjStore_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDObjStore,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TCIDObjStore
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDObjStore: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDObjStore::TCIDObjStore(const tCIDObjStore::EFlags eFlags) :

    m_bReady(kCIDLib::False)
    , m_eFlags(eFlags)
    , m_mbufRead(8192, 0x10000, 8192)
    , m_mbufReadKey(1024, 0x10000)
    , m_postCache(nullptr)
    , m_strmKey(8192, 0x10000)
    , m_strmOut(8192, 0x10000)
{
}

TCIDObjStore::~TCIDObjStore()
{
    // If we ever got the object store allocated, then let's clean up
    if (m_postCache)
    {
        TLocker lockrStore(&m_mtxSync);

        // If its currently opened, then close it
        if (m_bReady)
            m_postCache->Close();
        delete m_postCache;
        m_postCache = nullptr;
    }
}


// ---------------------------------------------------------------------------
//  TCIDObjStore: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TCIDObjStore::AddObject(const   TString&        strKey
                        , const MStreamable&    strmblToWrite
                        , const tCIDLib::TCard4 c4Reserve)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);
    ValidatePath(strKey, kCIDLib::True, CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Flatten the object
    m_strmOut.Reset();
    m_strmOut << strmblToWrite;
    m_strmOut.Flush();

    // And flatten the key
    m_strmKey.Reset();
    m_strmKey << strKey;
    m_strmKey.Flush();

    // Delegate to the cache object
    m_postCache->AddObject
    (
        strKey
        , m_strmKey.mbufData()
        , m_strmKey.c4CurSize()
        , m_strmOut.mbufData()
        , m_strmOut.c4CurSize()
        , c4Reserve
    );
}

tCIDLib::TVoid
TCIDObjStore::AddObjectDirect(  const   TString&        strKey
                                , const THeapBuf&       mbufToWrite
                                , const tCIDLib::TCard4 c4Bytes
                                , const tCIDLib::TCard4 c4Reserve)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);
    ValidatePath(strKey, kCIDLib::True, CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // And flatten the key
    m_strmKey.Reset();
    m_strmKey << strKey;
    m_strmKey.Flush();

    // Delegate to the cache object
    m_postCache->AddObject
    (
        strKey
        , m_strmKey.mbufData()
        , m_strmKey.c4CurSize()
        , mbufToWrite
        , c4Bytes
        , c4Reserve
    );
}


tCIDLib::TBoolean
TCIDObjStore::bAddOrUpdate( const   TString&            strKey
                            ,       tCIDLib::TCard4&    c4Version
                            , const MStreamable&        strmblToWrite
                            , const tCIDLib::TCard4     c4Reserve)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);
    ValidatePath(strKey, kCIDLib::True, CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Flatten the object
    m_strmOut.Reset();
    m_strmOut << strmblToWrite;
    m_strmOut.Flush();

    // And flatten the key
    m_strmKey.Reset();
    m_strmKey << strKey;
    m_strmKey.Flush();

    // Delegate to the cache object
    return m_postCache->bAddOrUpdate
    (
        strKey
        , c4Version
        , m_strmKey.mbufData()
        , m_strmKey.c4CurSize()
        , m_strmOut.mbufData()
        , m_strmOut.c4CurSize()
        , c4Reserve
    );
}

tCIDLib::TBoolean
TCIDObjStore::bAddOrUpdateDirect(const  TString&            strKey
                                ,       tCIDLib::TCard4&    c4Version
                                , const THeapBuf&           mbufToWrite
                                , const tCIDLib::TCard4     c4Bytes
                                , const tCIDLib::TCard4     c4Reserve)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);
    ValidatePath(strKey, kCIDLib::True, CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // And flatten the key
    m_strmKey.Reset();
    m_strmKey << strKey;
    m_strmKey.Flush();

    // Delegate to the cache object
    return m_postCache->bAddOrUpdate
    (
        strKey
        , c4Version
        , m_strmKey.mbufData()
        , m_strmKey.c4CurSize()
        , mbufToWrite
        , c4Bytes
        , c4Reserve
    );
}


tCIDLib::TBoolean
TCIDObjStore::bAllObjectsUnder( const   TString&            strStartScope
                                ,       tCIDLib::TStrList&  colToFill) const
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);
    return m_postCache->bAllObjectsUnder(strStartScope, colToFill);
}


tCIDLib::TBoolean TCIDObjStore::bDeleteObjectIfExists(const TString& strKey)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);
    ValidatePath(strKey, kCIDLib::True, CID_LINE, kCIDLib::True);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Delegate to the cache object
    return m_postCache->bDeleteObjectIfExists(strKey);
}


tCIDLib::TBoolean
TCIDObjStore::bInitialize(const TString& strPath, const TString& strStoreName)
{
    // Lock while we initialize
    TLocker lockrStore(&m_mtxSync);

    // Make sure we aren't already initialized
    if (m_bReady)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcInit_AlreadyInitialized
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
        );
    }

    //
    //  Create the new object store manager object if we haven't done so
    //  already.
    //
    if (!m_postCache)
        m_postCache = new TCIDObjStoreImpl(strPath, strStoreName, m_eFlags);

    //
    //  And initialize it. This will either create a new store, or load up
    //  an existing one. The return indicates which one, with true meaning
    //  it created a new store.
    //
    const tCIDLib::TBoolean bRet = m_postCache->bInitialize();

    // Indicate that we are now ready
    m_bReady = kCIDLib::True;
    return bRet;
}


tCIDLib::TBoolean TCIDObjStore::bKeyExists(const TString& strKey) const
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    return m_postCache->bKeyExists(strKey);
}

tCIDLib::TBoolean
TCIDObjStore::bKeyExists(const TString& strKey, tCIDLib::TCard4& c4Version) const
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    return m_postCache->bKeyExists(strKey);
}


tCIDLib::TBoolean
TCIDObjStore::bFindNameUnder(const  TString&            strName
                            , const TString&            strStartScope
                            ,       tCIDLib::TStrList&  colToFill) const
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);
    return m_postCache->bFindNameUnder(strName, strStartScope, colToFill);
}


tCIDLib::TBoolean
TCIDObjStore::bReadObject(  const   TString&            strKey
                            ,       tCIDLib::TCard4&    c4Version
                            ,       MStreamable&        strmblToFill)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);
    ValidatePath(strKey, kCIDLib::True, CID_LINE, kCIDLib::True);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Delegate to the cache object
    tCIDLib::TCard4 c4Size;
    const tCIDLib::ELoadRes eRes = m_postCache->eReadObject
    (
        strKey, c4Version, m_mbufRead, c4Size, kCIDLib::True
    );

    if (eRes == tCIDLib::ELoadRes::NewData)
    {
        TBinMBufInStream strmTmp(&m_mbufRead, c4Size);
        strmTmp >> strmblToFill;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TVoid TCIDObjStore::BackupStore()
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    m_postCache->BackupStore();
}


tCIDLib::TCard4 TCIDObjStore::c4ObjectsInStore() const
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Delegate to the cache object
    return m_postCache->c4ObjectsInStore();
}


tCIDLib::TCard4
TCIDObjStore::c4QueryKeysInScope(const  TString&                strScope
                                ,       tCIDLib::TStrCollect&   colToFill)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);
    ValidatePath(strScope, kCIDLib::False, CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Delegate to the cache object
    return m_postCache->c4QueryKeysInScope(strScope, colToFill);
}


tCIDLib::TCard4
TCIDObjStore::c4QueryObjectsInScope(const   TString&                strScope
                                    ,       tCIDLib::TStrCollect&   colToFill)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);
    ValidatePath(strScope, kCIDLib::False, CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Delegate to the cache object
    return m_postCache->c4QueryObjectsInScope(strScope, colToFill);
}


tCIDLib::TCard4
TCIDObjStore::c4QuerySubScopes( const   TString&                strScope
                                ,       tCIDLib::TStrCollect&   colToFill)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);
    ValidatePath(strScope, kCIDLib::False, CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Delegate to the cache object
    return m_postCache->c4QuerySubScopes(strScope, colToFill);
}


tCIDLib::TCard4
TCIDObjStore::c4UpdateObject(const  TString&        strKey
                            , const MStreamable&    strmblToWrite)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);
    ValidatePath(strKey, kCIDLib::True, CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Flatten the object
    m_strmOut.Reset();
    m_strmOut << strmblToWrite;
    m_strmOut.Flush();

    // Delegate to the cache object
    return m_postCache->c4UpdateObject
    (
        strKey
        , m_strmOut.mbufData()
        , m_strmOut.c4CurSize()
    );
}


// Writes raw data bytes to the given key, as apposed to streaming out an object
tCIDLib::TCard4
TCIDObjStore::c4UpdateObjectDirect( const   TString&        strKey
                                    , const THeapBuf&       mbufToWrite
                                    , const tCIDLib::TCard4 c4Bytes)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);
    ValidatePath(strKey, kCIDLib::True, CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Delegate to the cache object
    return m_postCache->c4UpdateObject(strKey, mbufToWrite, c4Bytes);
}


tCIDLib::TVoid TCIDObjStore::Close()
{
    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // If initialized, then close it
    if (m_bReady)
        m_postCache->Close();

    // Mark us as not ready now
    m_bReady = kCIDLib::False;
}


tCIDLib::TVoid TCIDObjStore::DebugDump(TTextOutStream& strmOut)
{
    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Delegate to the implementation
    m_postCache->ValidateStore(&strmOut);
}


// Deletes a single object from the store
tCIDLib::TVoid TCIDObjStore::DeleteObject(const TString& strKey)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);
    ValidatePath(strKey, kCIDLib::True, CID_LINE, kCIDLib::True);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Delegate to the cache object
    return m_postCache->DeleteObject(strKey);
}


// Deletes an entire scope from the store
tCIDLib::TVoid TCIDObjStore::DeleteScope(const TString& strScopeName)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);
    ValidatePath(strScopeName, kCIDLib::False, CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Delegate to the cache object
    return m_postCache->DeleteScope(strScopeName);
}


//
//  Streams in an object at the indicated key.
//
tCIDLib::ELoadRes
TCIDObjStore::eReadObject(  const   TString&            strKey
                            , CIOP  tCIDLib::TCard4&    c4Version
                            ,       MStreamable&        strmblToFill
                            , const tCIDLib::TBoolean   bThrowIfNot)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);
    ValidatePath(strKey, kCIDLib::True, CID_LINE, kCIDLib::True);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Delegate to the cache object
    tCIDLib::TCard4 c4Size;
    const tCIDLib::ELoadRes eRes = m_postCache->eReadObject
    (
        strKey, c4Version, m_mbufRead, c4Size, bThrowIfNot
    );

    if (eRes == tCIDLib::ELoadRes::NewData)
    {
        TBinMBufInStream strmTmp(&m_mbufRead, c4Size);
        strmTmp >> strmblToFill;
    }
    return eRes;
}


//
//  Reads the raw bytes of an object, as apposed to streaming in an object.
//
tCIDLib::ELoadRes
TCIDObjStore::eReadObjectDirect(const   TString&            strKey
                                , CIOP  tCIDLib::TCard4&    c4Version
                                ,       THeapBuf&           mbufToFill
                                , COP   tCIDLib::TCard4&    c4BytesRead
                                , const tCIDLib::TBoolean   bThrowIfNot)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);
    ValidatePath(strKey, kCIDLib::True, CID_LINE, kCIDLib::True);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Delegate to the cache object
    const tCIDLib::ELoadRes eRes = m_postCache->eReadObject
    (
        strKey
        , c4Version
        , mbufToFill
        , c4BytesRead
        , bThrowIfNot
    );
    return eRes;
}


// Makes sure all store data is flushed out to the disk
tCIDLib::TVoid TCIDObjStore::FlushToDisk()
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Delegate to the cache object
    m_postCache->FlushStore();
}


TMutex& TCIDObjStore::mtxSync()
{
    return m_mtxSync;
}


//
//  This method will load up the passed collection with all of the keys to
//  all of the objects in the store.
//
tCIDLib::TVoid TCIDObjStore::QueryAllKeys(tCIDLib::TStrCollect& colToFill)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Delegate to the cache object
    m_postCache->QueryAllKeys(colToFill);
}


// Returns the time stamp of the last backup invocation
TTime TCIDObjStore::tmLastBackup() const
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    return TTime(m_postCache->enctLastBackup());
}


// Do a validation pass of the store
tCIDLib::TVoid TCIDObjStore::ValidateStore()
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Delegate to the cache object
    m_postCache->ValidateStore(nullptr);
}

tCIDLib::TVoid TCIDObjStore::ValidateStore(TTextOutStream& strmTar)
{
    if (!m_bReady)
        ThrowNotReady(CID_LINE);

    // Lock while we do this
    TLocker lockrStore(&m_mtxSync);

    // Delegate to the cache object
    m_postCache->ValidateStore(&strmTar);
}



// ---------------------------------------------------------------------------
//  TCIDObjStore: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TCIDObjStore::ThrowNotReady(const tCIDLib::TCard4 c4Line) const
{
    facCIDObjStore().ThrowErr
    (
        CID_FILE
        , c4Line
        , kObjSErrs::errcInit_NotInitialized
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotReady
    );
}


//
//  NOTE that we allow a trailing slash optionally, because there was an
//  error in an earlier version that allowed keys of this type to get into
//  the repository. They are being fixed as we go, but in the meantime we
//  have to allow reading these keys in, so that they can be fixed and then
//  written back out. So the reading methods above, for how, call this with
//  the last parm enabled. We can remove this when all of the data has been
//  fixed. The delete method allows it also so that the bad keys can be
//  deleted if needed.
//
tCIDLib::TVoid
TCIDObjStore::ValidatePath( const   TString&            strPath
                            , const tCIDLib::TBoolean   bTerminal
                            , const tCIDLib::TCard4     c4Line
                            , const tCIDLib::TBoolean   bAllowTrailing) const
{
    if (strPath.bIsEmpty()
    ||  (strPath.chFirst() != kCIDLib::chForwardSlash))
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , c4Line
            , kObjSErrs::errcData_BadKey
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , strPath
        );
    }

    // If a terminal node, then it cannot end with a slash
    if (!bAllowTrailing
    &&  bTerminal
    &&  (strPath.chLast() == kCIDLib::chForwardSlash))
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , c4Line
            , kObjSErrs::errcData_BadBindingKey
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , strPath
        );
    }
}

