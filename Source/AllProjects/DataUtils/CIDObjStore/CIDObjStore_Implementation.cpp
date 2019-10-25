//
// FILE NAME: CIDObjStore_Implementation.cpp
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
//  This file implements the object store implementation. There are actually
//  a few files, since this is a complex class. This particular file provides
//  the public methods that the public engine (wrapper around this impl)
//  class calls, and any general private methods that aren't related to
//  manipulating either the store files or the free list/index data.
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
RTTIDecls(TCIDObjStoreImpl,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TCIDObjStoreImpl
//  PREFIX: ost
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDObjStoreImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDObjStoreImpl::TCIDObjStoreImpl( const   TString&                strPath
                                    , const TString&                strStoreName
                                    ,const  tCIDObjStore::EFlags    eFlags) :
    m_colStoreList
    (
        kCIDObjStore_::c4Modulus
        , TStringKeyOps(tCIDLib::bAllBitsOn(eFlags, tCIDObjStore::EFlags::CaseSensitive))
        , &TOSStoreItem::strKey
    )
    , m_bCaseSensitiveKeys(tCIDLib::bAllBitsOn(eFlags, tCIDObjStore::EFlags::CaseSensitive))
    , m_bRecoveryMode(tCIDLib::bAllBitsOn(eFlags, tCIDObjStore::EFlags::RecoveryMode))
    , m_c4IndexVersionNum(0)
    , m_strStoreName(strStoreName)
{
    // Build up the path to the store file that will make up this store
    TPathStr pathTmp = strPath;
    pathTmp.AddLevel(m_strStoreName);
    pathTmp.AppendExt(L"CIDObjStore");
    m_strStoreFile = pathTmp;
    m_flStore.strName(m_strStoreFile);
}

TCIDObjStoreImpl::~TCIDObjStoreImpl()
{
    //
    //  Flush any existing contents to disk. Hopefully it will be done
    //  by the client apps before destroying us, since we cannot do much
    //  about errors here, but worst case we need to try it.
    //
    try
    {
        Close();
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
//  TCIDObjStoreImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TCIDObjStoreImpl::AddObject(const   TString&        strKey
                            , const TMemBuf&        mbufKey
                            , const tCIDLib::TCard4 c4KeyBytes
                            , const TMemBuf&       mbufData
                            , const tCIDLib::TCard4 c4DataSize
                            , const tCIDLib::TCard4 c4Reserve)
{
    // See if this key already exists
    TOSStoreItem* posiFound = m_colStoreList.pobjFindByKey(strKey);
    if (posiFound)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcData_KeyAlreadyExists
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , strKey
            , m_strStoreName
        );
    }

    // Calc the bytes we'll need for this guy
    const tCIDLib::TCard4 c4Needed = c4DataSize + c4Reserve + c4KeyBytes
                                     + sizeof(TCIDObjStoreImpl::TStoreItemHdr);

    //
    //  Find a free slot to put us in. This will create a new free list item
    //  slow big enough to hold our item, though it may be slightly larger
    //  if the free item taken was just a little larger than needed, in which
    //  case it just takes the whole thing
    //
    tCIDLib::TCard4 c4Offset;
    tCIDLib::TCard4 c4ActualSz;
    tCIDLib::TCard4 c4FreeInd = c4FindFreeSpace(c4Needed, c4ActualSz, c4Offset);

    //
    //  If we didn't find any item to hold it, we have to either expand or
    //  try to compact.
    //
    if (c4FreeInd == kCIDLib::c4MaxCard)
    {
        ExpandOrCompact(c4Needed);
        c4FreeInd = c4FindFreeSpace(c4Needed, c4ActualSz, c4Offset);
    }

    //
    //  Ok, create a store item now which represents all of the info we've
    //  just got plus the item info. We have to recalculate the allocated
    //  size by taking the actual size we got and removing the 'overhead'
    //  stuff, the key and item header, from it. That's the amount that will
    //  be allocated to the actual data.
    //
    TOSStoreItem osiNew
    (
        c4Offset
        , c4DataSize
        , c4ActualSz - (c4KeyBytes + sizeof(TCIDObjStoreImpl::TStoreItemHdr))
        , 1
        , strKey
        , c4KeyBytes
    );

    //
    //  Try to write this object, which will overwrite the free list item
    //  that we are taking, which is what is supposed to happen. A free slot
    //  was created of the needed size for us to take over.
    //
    try
    {
        CommitStoreItem(m_flStore, osiNew, mbufData, mbufKey, m_strStoreName);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        //
        //  It failed, so put the free list item back again, since a partial
        //  write would have overwritten the free item header. This will get
        //  the store back to a stable state. We've not removed the free item
        //  from our free list, so everything will be back happy.
        //
        WriteFreeListItem(m_flStore, m_colFreeList[c4FreeInd], m_strStoreName);

        // And now we can rethrow it
        throw;
    }

    //
    //  Remove the free list item from our free list now, since we have
    //  taken it. And add the new object store item to the store list.
    //
    try
    {
        m_colFreeList.RemoveAt(c4FreeInd);
        m_colStoreList.objAdd(osiNew);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}


tCIDLib::TBoolean
TCIDObjStoreImpl::bAddOrUpdate( const   TString&            strKey
                                ,       tCIDLib::TCard4&    c4Version
                                , const TMemBuf&            mbufKey
                                , const tCIDLib::TCard4     c4KeySize
                                , const TMemBuf&            mbufData
                                , const tCIDLib::TCard4     c4DataSize
                                , const tCIDLib::TCard4     c4Reserve)
{
    tCIDLib::TBoolean bNew = kCIDLib::False;
    try
    {
        // Find the item with this key
        TOSStoreItem* posiFound = m_colStoreList.pobjFindByKey(strKey);

        // If we didn't find it, then add, else update
        bNew = (posiFound == 0);
        if (bNew)
        {
            AddObject(strKey, mbufKey, c4KeySize, mbufData, c4DataSize, c4Reserve);
            c4Version = 1;
        }
         else
        {
            c4Version = c4UpdateObject(*posiFound, mbufData, c4DataSize);
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return bNew;
}


//
//  Returns the paths to all objects under the start scope, recursively.
//
tCIDLib::TBoolean
TCIDObjStoreImpl::bAllObjectsUnder( const   TString&            strStartScope
                                    ,       tCIDLib::TStrList&  colPathsFound) const
{
    colPathsFound.RemoveAll();

    TStoreList::TCursor cursStore(&m_colStoreList);
    if (cursStore.bIsValid())
    {
        //
        //  Create a version of the scope that has the trailing slash, which the
        //  incoming might not. This insures we can't match something that partially
        //  matches into a scope name.
        //
        TString strToFind(strStartScope);
        if (strToFind.chLast() != kCIDLib::chForwardSlash)
            strToFind.Append(kCIDLib::chForwardSlash);

        for (; cursStore; ++cursStore)
        {
            const TOSStoreItem& osiCur = *cursStore;

            //
            //  If the scope starts wtih our start scope, then it has to be under
            //  the start.
            //
            if (osiCur.strScope().bStartsWithI(strToFind))
                colPathsFound.objAdd(osiCur.strKey());

        }
    }
    return !colPathsFound.bIsEmpty();
}


tCIDLib::TBoolean
TCIDObjStoreImpl::bDeleteObjectIfExists(const TString& strKey)
{
    try
    {
        // If we find it, then delete it and flush changes
        TOSStoreItem* posiDel = m_colStoreList.pobjFindByKey(strKey);
        if (posiDel)
        {
            c4GiveBackChunk(posiDel->c4Offset(), posiDel->c4StorageRequired());
            m_colStoreList.bRemoveKey(strKey);
            FlushStore();
            return kCIDLib::True;
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return kCIDLib::False;
}



//
//  Gets the object store either opened and loaded, or created and initalized.
//  Returns a boolean to indicate which one. True means we created it.
//
tCIDLib::TBoolean TCIDObjStoreImpl::bInitialize()
{
    const tCIDLib::TBoolean bStoreExists = m_flStore.bExists();

    //
    //  Get the files opened or created. If we opened the memory buffer,
    //  then the files have to be there, since another app has already
    //  created the store. Else, the files may or may not already exist.
    //
    //  If we open it, it the Open() method will build the index from the
    //  current store contents.
    //
    if (bStoreExists)
        Open();
    else
        Create();

    // And return whether we created it or not
    return !bStoreExists;
}


tCIDLib::TBoolean TCIDObjStoreImpl::bKeyExists(const TString& strKey) const
{
    // Find the item with this key
    const TOSStoreItem* posiCur = m_colStoreList.pobjFindByKey(strKey);
    if (!posiCur)
        return kCIDLib::False;
    return kCIDLib::True;
}

tCIDLib::TBoolean
TCIDObjStoreImpl::bKeyExists(const TString& strKey, tCIDLib::TCard4& c4Version) const
{
    // Find the item with this key
    const TOSStoreItem* posiCur = m_colStoreList.pobjFindByKey(strKey);
    if (!posiCur)
        return kCIDLib::False;

    c4Version = posiCur->c4Version();
    return kCIDLib::True;
}


//
//  Looks for the indicted key anywhere under the passed scope (looking for items,
//  not for sub-scopes.) We return all such keys found. There can't be more than one
//  such per-scope.
//
tCIDLib::TBoolean
TCIDObjStoreImpl::bFindNameUnder(const  TString&            strName
                                , const TString&            strStartScope
                                ,       tCIDLib::TStrList&  colPathsFound) const
{
    colPathsFound.RemoveAll();

    //
    //  Iterate the list and look for anything that starts with our indicated
    //  start scope.
    //
    TStoreList::TCursor cursStore(&m_colStoreList);
    if (cursStore.bIsValid())
    {
        //
        //  Create a version of the scope that has the trailing slash, which the
        //  incoming might not. This insures we can't match something that partially
        //  matches into a scope name.
        //
        TString strToFind(strStartScope);
        if (strToFind.chLast() != kCIDLib::chForwardSlash)
            strToFind.Append(kCIDLib::chForwardSlash);

        TString strTmp;
        for (; cursStore; ++cursStore)
        {
            const TOSStoreItem& osiCur = *cursStore;

            //
            //  If the scope starts wtih our start scope and the key equals our
            //  key, then take it.
            //
            if (osiCur.strScope().bStartsWithI(strToFind)
            &&  osiCur.strName().bCompareI(strName))
            {
                // Combine the scope and name for the full path to the find
                strTmp = osiCur.strScope();
                strTmp.Append(strName);

                // And add it to the return list.
                colPathsFound.objAdd(strTmp);
            }
        }
    }
    return !colPathsFound.bIsEmpty();
}


tCIDLib::TCard4 TCIDObjStoreImpl::c4ObjectsInStore() const
{
    return m_colStoreList.c4ElemCount();
}


//
//  Find all of the objects in the passed scope. We return the full paths to the
//  objects we find. The return is the number we found.
//
tCIDLib::TCard4
TCIDObjStoreImpl::c4QueryObjectsInScope(const   TString&              strScope
                                        ,       tCIDLib::TStrCollect& colToFill)
{
    //
    //  Our scopes are stored with the trailing slash, but the passed one might
    //  not. So we need to create one that we are sure has the slash. This way we
    //  are sure we don't partially match a scope.
    //
    TString strToFind(strScope);
    if (strToFind.chLast() != kCIDLib::chForwardSlash)
        strToFind.Append(kCIDLib::chForwardSlash);

    //
    //  Iterate the items, and for each one in the the passed scope, add
    //  its key to the passed collection.
    //
    colToFill.RemoveAll();
    TStoreList::TCursor cursStore(&m_colStoreList);
    for (; cursStore; ++cursStore)
    {
        const TOSStoreItem& osiCur = *cursStore;
        const TString& strCurPath = osiCur.strScope();

        // If this path is equal the one we are looking for, then take this guy
        if (strToFind.bCompareI(osiCur.strScope()))
            colToFill.objAdd(osiCur.strKey());
    }
    return colToFill.c4ElemCount();
}


//
//  Find all of the sub-scopes under the passed scope. We return the full paths to the
//  sub-scopes we find. The return is the number we found. We only return unique ones,
//  so no dups.
//
//  Since only actual objects are in the list, what we have to do is find any objects
//  whose path (just the path part) starts with the target scope, but is longer than
//  that, and has one more slash in it. So these:
//
//  /Scope1/Scope2/
//  /Scope1/Scope2/Scope3/Scope4/
//
//  will get ignored because the first one is the actual starting scope, and the
//  second ones will have more slash in it (it is a scope more than one level under
//  the start scope.)
//
//  The returned scopes will have a trailing slash, the same as what we store them
//  as.
//
tCIDLib::TCard4
TCIDObjStoreImpl::c4QuerySubScopes( const   TString&              strScope
                                    ,       tCIDLib::TStrCollect& colToFill)
{
    //
    //  Our scopes are stored with the trailing slash, but the passed one might
    //  not. So we need to create one that we are sure has the slash. This way we
    //  are sure we don't partially match a scope.
    //
    TString strToFind(strScope);
    if (strToFind.chLast() != kCIDLib::chForwardSlash)
        strToFind.Append(kCIDLib::chForwardSlash);
    const tCIDLib::TCard4 c4FindLen = strToFind.c4Length();

    //
    //  Iterate the items, and for each one in the the passed scope, add
    //  its key to the passed collection.
    //
    colToFill.RemoveAll();
    TStoreList::TCursor cursStore(&m_colStoreList);
    if (cursStore.bIsValid())
    {
        //
        //  To avoid dups we need a hashed set of strings. Depending on what we were told
        //  to do wrt to case sensitive keys, we set the key ops to treat keys case
        //  sensitively or not for dup testing.
        //
        tCIDLib::TStrHashSet colDups(109UL, TStringKeyOps(m_bCaseSensitiveKeys));

        for (; cursStore; ++cursStore)
        {
            const TOSStoreItem& osiCur = *cursStore;
            const TString& strCurScope = osiCur.strScope();
            const tCIDLib::TCard4 c4CurLen = strCurScope.c4Length();

            //
            //  If it starts with the search scope but is longer, and contains
            //  one more slash and that slash is at the end of it, then it's one
            //  of our guys.
            //
            tCIDLib::TCard4 c4At = c4FindLen - 1;
            if (strCurScope.bStartsWithI(strToFind)
            &&  (c4CurLen > c4FindLen)
            &&  strCurScope.bNextOccurrence(kCIDLib::chForwardSlash, c4At)
            &&  (c4At == c4CurLen - 1))
            {
                // Make sure we haven't already done this one
                if (!colDups.bHasElement(strCurScope))
                {
                    // Not yet, so add it to the dups list and the return list
                    colDups.objAdd(strCurScope);
                    colToFill.objAdd(strCurScope);
                }
            }
        }
    }
    return colToFill.c4ElemCount();
}



tCIDLib::TCard4
TCIDObjStoreImpl::c4UpdateObject(const  TString&        strKey
                                , const TMemBuf&        mbufData
                                , const tCIDLib::TCard4 c4Size)
{
    TOSStoreItem* posiToUpdate = m_colStoreList.pobjFindByKey(strKey);
    if (!posiToUpdate)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcData_KeyNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , strKey
            , m_strStoreName
        );
    }

    try
    {
        // Call the common helper that actually does the work
        CIDLib_Suppress(6011) // We null checked above
        UpdateItemData(*posiToUpdate, mbufData, c4Size);

        // Flush any changes to disk
        FlushStore();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // Return the version
    return posiToUpdate->c4Version();
}

tCIDLib::TCard4
TCIDObjStoreImpl::c4UpdateObject(       TOSStoreItem&   osiToUpdate
                                , const TMemBuf&        mbufData
                                , const tCIDLib::TCard4 c4Size)
{
    try
    {
        // Call the common helper that actually does the work
        UpdateItemData(osiToUpdate, mbufData, c4Size);

        // Flush any changes to disk
        FlushStore();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // Return the version
    return osiToUpdate.c4Version();
}



tCIDLib::TVoid TCIDObjStoreImpl::Close()
{
    // First flush any changes to disk and close the file, if it's open
    if (m_flStore.bIsOpen())
    {
        FlushStore();
        m_flStore.Close();
    }
}



tCIDLib::TVoid TCIDObjStoreImpl::DeleteObject(const TString& strKey)
{
    try
    {
        // Find the item with this key
        TOSStoreItem* posiDel = m_colStoreList.pobjFindByKey(strKey);
        if (!posiDel)
        {
            facCIDObjStore().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kObjSErrs::errcData_KeyNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , strKey
                , m_strStoreName
            );
        }

        // Give this object's chunk back and remove it from the list
        CIDLib_Suppress(6011) // We null checked above
        c4GiveBackChunk(posiDel->c4Offset(), posiDel->c4StorageRequired());
        m_colStoreList.bRemoveKey(strKey);

        // Flush everthing to disk
        FlushStore();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}



tCIDLib::TVoid TCIDObjStoreImpl::DeleteScope(const TString& strScopeName)
{
    try
    {
        //
        //  The passed value may or may not have a trailing slash. We need to insure that
        //  it does, so that we can't accidentally match something that's really an item
        //  that happens to have the name that the caller (incorrectly) passed us.
        //
        TString strCompVal(strScopeName, 1UL);
        if (strCompVal.chLast() != kCIDLib::chForwardSlash)
            strCompVal.Append(kCIDLib::chForwardSlash);
        const tCIDLib::TCard4 c4CompLen = strCompVal.c4Length();

        //
        //  Since we are deleting as we go, we can't do the while(bNext()) at the bottom of
        //  the loop as normal, since that would cause us to skip one after each remove.
        //
        TStoreList::TNCCursor cursStore(&m_colStoreList);
        while (cursStore)
        {
            //
            //  If the key of this one starts with the passed scope, then we want to
            //  remove it. Note that the RemoveAt() that takes a cursor will keep the
            //  passed cursor valid.
            //
            const TString& strCurPath = cursStore.objRCur().strKey();
            if (strCurPath.bCompareN(strCompVal, c4CompLen))
            {
                // Give this block's data back, and remove the item itself
                c4GiveBackChunk
                (
                    cursStore.objWCur().c4Offset()
                    , cursStore.objWCur().c4StorageRequired()
                );
                m_colStoreList.RemoveAt(cursStore);
            }
             else
            {
                ++cursStore;
            }
        }

        // Flush everthing to disk
        FlushStore();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}



tCIDLib::TEncodedTime TCIDObjStoreImpl::enctLastBackup() const
{
    return m_enctLastBackup;
}



tCIDLib::ELoadRes
TCIDObjStoreImpl::eReadObject(  const   TString&            strKey
                                ,       tCIDLib::TCard4&    c4Version
                                ,       TMemBuf&            mbufData
                                ,       tCIDLib::TCard4&    c4DataSize
                                ,       TMemBuf&            mbufKey
                                ,       tCIDLib::TCard4&    c4KeySize
                                , const tCIDLib::TBoolean   bThrowIfNot)
{
    //
    //  Find the item with this key. Either throw or return not found, if we don't
    //  find it.
    //
    TOSStoreItem* posiCur = m_colStoreList.pobjFindByKey(strKey);
    if (!posiCur)
    {
        if (bThrowIfNot)
        {
            facCIDObjStore().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kObjSErrs::errcData_KeyNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , strKey
                , m_strStoreName
            );
        }
        return tCIDLib::ELoadRes::NotFound;
    }

    //
    //  If the version number is the same, then its not changed, so return
    //  false with no data.
    //
    if (posiCur->c4Version() == c4Version)
        return tCIDLib::ELoadRes::NoNewData;

    try
    {
        // Load the data, in this case we get data and key data
        bLoadItemData(*posiCur, mbufData, mbufKey);

        // It worked, so set the caller's parms
        c4Version = posiCur->c4Version();
        c4DataSize = posiCur->c4CurUsed();
        c4KeySize = posiCur->c4KeyBytes();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return tCIDLib::ELoadRes::NewData;
}

tCIDLib::ELoadRes
TCIDObjStoreImpl::eReadObject(  const   TString&            strKey
                                ,       tCIDLib::TCard4&    c4Version
                                ,       TMemBuf&            mbufData
                                ,       tCIDLib::TCard4&    c4DataSize
                                , const tCIDLib::TBoolean   bThrowIfNot)
{
    // Find the item with this key, throw if not found
    TOSStoreItem* posiCur = m_colStoreList.pobjFindByKey(strKey);
    if (!posiCur)
    {
        if (bThrowIfNot)
        {
            facCIDObjStore().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kObjSErrs::errcData_KeyNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , strKey
                , m_strStoreName
            );
        }
         else
        {
            return tCIDLib::ELoadRes::NotFound;
        }
    }

    //
    //  If the version number is the same, then its not changed, so return
    //  false with no data.
    //
    CIDLib_Suppress(6011) // We null checked above
    if (posiCur->c4Version() == c4Version)
        return tCIDLib::ELoadRes::NoNewData;

    try
    {
        // Load up the data. We only care about the object data in this case
        bLoadItemData(*posiCur, mbufData);

        // It worked, so set the caller's parms
        c4Version = posiCur->c4Version();
        c4DataSize = posiCur->c4CurUsed();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return tCIDLib::ELoadRes::NewData;
}



tCIDLib::TVoid TCIDObjStoreImpl::FlushStore()
{
    m_flStore.Flush();
}


tCIDLib::TVoid
TCIDObjStoreImpl::QueryAllKeys(tCIDLib::TStrCollect& colToFill)
{
    // Just interate the list and grab each path
    TStoreList::TNCCursor cursStore(&m_colStoreList);
    for (; cursStore; ++cursStore)
        colToFill.objAdd(cursStore->strKey());
}

