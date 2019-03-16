//
// FILE NAME: CIDObjStore_Implementation3.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/17/2002
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
//  This file implements most grunt work fundamental methods that deal
//  with getting data into and out of the files.
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
//  TCIDObjStoreImpl: Public, static methods
// ---------------------------------------------------------------------------

//
//  This method will do an emergency type of recovery attempt on a repo,
//  creating a new file that has as much as can be recovered. It also
//  provides a log of things it did.
//
tCIDLib::TVoid
TCIDObjStoreImpl::RecoverStore( const   TString&    strSrcRepo
                                , const TString&    strTarFile
                                ,       TString&    strActionLog)
{
    // <TBD>
}



// ---------------------------------------------------------------------------
//  TCIDObjStoreImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDObjStoreImpl::BackupStore()
{
    //
    //  We need to do the following operations:
    //
    //  1. Get rid of any previous temp file
    //  2. Generate a new object store to the temp file, fully compacted
    //     and verified.
    //  3. If successful, delete any backup2 file, then rename any backup1
    //     file to the backup2 file.
    //  4. Rename the temp file to the backup1 file.
    //
    TPathStr pathTmpFl = m_strStoreFile;
    pathTmpFl.bRemoveExt();
    pathTmpFl.AppendExt(L"TmpStore");

    TPathStr pathBack1 = m_strStoreFile;
    pathBack1.bRemoveExt();
    pathBack1.AppendExt(L"StoreBack1");

    TPathStr pathBack2 = m_strStoreFile;
    pathBack2.bRemoveExt();
    pathBack2.AppendExt(L"StoreBack2");

    //
    //  Do step 1 and remove any previous temp file, removing any file attrs
    //  that might prevent us from doing it first.
    //
    if (TFileSys::bExists(pathTmpFl))
    {
        TFileSys::SetPerms(pathTmpFl, tCIDLib::EFilePerms::AllWE);
        TFileSys::DeleteFile(pathTmpFl);
    }

    //
    //  Create a local temp file and write out all the current items to it.
    //  We don't even have to modify the item headers in the process, since
    //  they don't contain the storage offset when on disk (since that is
    //  implicit in where they are stored.)
    //
    {
        TBinaryFile flTmp(pathTmpFl);
        flTmp.Open
        (
            tCIDLib::EAccessModes::Excl_ReadWrite
            , tCIDLib::ECreateActs::CreateIfNew
            , tCIDLib::EFilePerms::AllOwnerAccess
            , tCIDLib::EFileFlags::SafeStream
        );

        // Create a store header and write it out
        TStoreHdr hdrNew = {0};
        hdrNew.m_c4MagicValue1 = kCIDObjStore_::c4UsedMagicVal;
        hdrNew.m_enctLastBackup = TTime::enctNow();
        hdrNew.m_c4MagicValue2 = kCIDObjStore_::c4UsedMagicVal;
        hdrNew.m_c4IndexVersionNum = m_c4IndexVersionNum;

        if (flTmp.c4WriteBuffer(&hdrNew, sizeof(hdrNew)) != sizeof(hdrNew))
        {
            facCIDObjStore().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kObjSErrs::errcIO_WriteStoreFlHdr
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotAllWritten
                , pathTmpFl
            );
        }

        //
        //  Create a cursor on the item store item collection, and run through
        //  them, writing each one out. We'll need buffers for reading in the
        //  key and data buffers of each item, and then to pass to the commit
        //  method to write it out.
        //
        TStoreList::TCursor cursItems(&m_colStoreList);
        if (cursItems.bIsValid())
        {
            THeapBuf mbufKey(1024);
            THeapBuf mbufData(32 * 1024);
            TStoreItemHdr hdrCur;

            for (; cursItems; ++cursItems)
            {
                // Get a copy of the current item
                TOSStoreItem osiCur = *cursItems;

                // Read this one in from the current store
                LoadItemData(osiCur, mbufData, mbufKey, hdrCur);

                //
                //  Validate that the sizes and version that we read in
                //  matches what is in the store item.
                //
                if ((osiCur.c4CurUsed() != hdrCur.m_c4CurUsed)
                ||  (osiCur.c4Version() != hdrCur.m_c4VersionNum)
                ||  (osiCur.c4Allocated() != hdrCur.m_c4Allocated)
                ||  (osiCur.c4KeyBytes() != hdrCur.m_c4KeyBytes))
                {
                    //
                    //  If it doesn't, then log a message and leave it out of
                    //  the backup.
                    //
                    if (facCIDObjStore().bLogWarnings())
                    {
                        facCIDObjStore().LogMsg
                        (
                            CID_FILE
                            , CID_LINE
                            , kObjSErrs::errcBack_BadKey
                            , tCIDLib::ESeverities::Warn
                            , tCIDLib::EErrClasses::Format
                            , osiCur.strKey()
                        );
                    }
                }
                 else
                {
                    // Update the copy with the current target file offset
                    osiCur.c4Offset(tCIDLib::TCard4(flTmp.c8CurPos()));

                    // And write the data to the new spot in the temp store
                    CommitStoreItem(flTmp, osiCur, mbufData, mbufKey, pathTmpFl);
                }
            }
        }
    }

    // If there is a second backup file, get rid of it
    if (TFileSys::bExists(pathBack2))
    {
        TFileSys::SetPerms(pathBack2, tCIDLib::EFilePerms::AllWE);
        TFileSys::DeleteFile(pathBack2);
    }

    // If there is a first backup, rename it to the second backup
    if (TFileSys::bExists(pathBack1))
        TFileSys::Rename(pathBack1, pathBack2);

    // And rename the temp file to the back1 file
    TFileSys::Rename(pathTmpFl, pathBack1);
}



//
//  This method is called when the value of an item is updated. It first sees
//  if it can just write the new data into the old storage. If not, it sees
//  if there is an adjacent free list item that it can take space from. If not,
//  it tries to move the item to a free spot big enough. If not, it will expand
//  the store to make room.
//
//  NOTE: Make sure to keep osiToUpdate unchanged until the end, because we
//  need it at various places to reference info about the original size and
//  position of the item. We copy it to a temp and use that until the end,
//  when we copy it back.
//
tCIDLib::TVoid
TCIDObjStoreImpl::UpdateItemData(       TOSStoreItem&   osiToUpdate
                                , const TMemBuf&        mbufData
                                , const tCIDLib::TCard4 c4Size)
{
    const tCIDLib::TCard4 c4HdrSz = sizeof(TStoreItemHdr);

    // Seek to the item in the store and read in the header
    TStoreItemHdr hdrOldStore;
    m_flStore.SetFilePos(osiToUpdate.c4Offset());
    if (m_flStore.c4ReadBuffer(&hdrOldStore, c4HdrSz) != c4HdrSz)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_ReadStoreItemHdr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , osiToUpdate.strKey()
            , m_strStoreName
        );
    }

    // Insure that the key size matches the store item
    if (osiToUpdate.c4KeyBytes() != hdrOldStore.m_c4KeyBytes)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcData_KeySzMismatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(osiToUpdate.c4KeyBytes())
            , TCardinal(hdrOldStore.m_c4KeyBytes)
            , m_strStoreName
        );
    }

    // Read the key into a temp buffer
    THeapBuf mbufKey(hdrOldStore.m_c4KeyBytes);
    if (m_flStore.c4ReadBuffer( mbufKey
                                , hdrOldStore.m_c4KeyBytes) != hdrOldStore.m_c4KeyBytes)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_ReadItemKey
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , osiToUpdate.strKey()
            , m_strStoreName
        );
    }

    //
    //  And read in the original data, in case something goes wrong and
    //  we have to write it back out.
    //
    THeapBuf mbufOrgData(hdrOldStore.m_c4CurUsed);
    if (m_flStore.c4ReadBuffer( mbufOrgData
                                , hdrOldStore.m_c4CurUsed) != hdrOldStore.m_c4CurUsed)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_ReadStoreItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , osiToUpdate.strKey()
            , m_strStoreName
        );
    }

    //
    //  Make a copy of the item, and update that. We won't write it to the
    //  index until we know everything is done.
    //
    TOSStoreItem osiTmp(osiToUpdate);

    //
    //  See if we can fit the new contents into the old store item. If its
    //  larger, see if it has any reserve space. If it does, and that will
    //  handle our needs, then take it.
    //
    if (c4Size <= osiToUpdate.c4Allocated())
    {
        //
        //  Ok, it's the easy one, so adjust the size of the storage object
        //  and commit this data.
        //
        osiTmp.c4CurUsed(c4Size);

        //
        //  Bump the version number on this item since it's being modified,
        //  and commit it it back to the original data area.
        //
        osiTmp.IncVersion();
        CommitStoreItem(m_flStore, osiTmp, mbufData, mbufKey, m_strStoreName);

        // It worked, so update the original
        osiToUpdate = osiTmp;
    }
     else
    {
        //
        //  It's the hard scenario. We have to find a new spot for this item.
        //  Get the bytes needed for the data plus any overhead.
        //
        const tCIDLib::TCard4 c4Needed = osiTmp.c4NewStorageRequired(c4Size, 0);

        //
        //  Find a free slot to put us in. This will create a new free list
        //  item big enough to hold our item, though it may be slightly larger
        //  if the free item taken was just a little larger than needed, in
        //  which case it just takes the whole thing.
        //
        tCIDLib::TCard4 c4Offset;
        tCIDLib::TCard4 c4ActualSz;
        tCIDLib::TCard4 c4FreeInd = c4FindFreeSpace(c4Needed, c4ActualSz, c4Offset);

        //
        //  If we didn't find any item to hold it, we have to expand.
        //
        //  NOTE: We MUST expand, we cannot expand or compact. If we compact,
        //        then we invalidate the original item offset information and
        //        it will make recovery impossible if we mess up.
        //
        //        We'll do expand or compact during additions of new items,
        //        where these issues don't matter.
        //
        if (c4FreeInd == kCIDLib::c4MaxCard)
        {
            ExpandStore(c4Needed);
            c4FreeInd = c4FindFreeSpace(c4Needed, c4ActualSz, c4Offset);
        }

        // Update the temp item to represent the new size and offset
        osiTmp.c4Offset(c4Offset);
        osiTmp.c4Allocated
        (
            c4ActualSz
            - (hdrOldStore.m_c4KeyBytes + sizeof(TCIDObjStoreImpl::TStoreItemHdr))
        );
        osiTmp.c4CurUsed(c4Size);

        //
        //  Try to store this item in the new slot, which will overwrite the
        //  free list item we made for it. Get a pointer to the free list
        //  item, because when we give back the original chunk, the index
        //  could be affected. So we'll go back and find this item by address
        //  afterwards, to remove the free list item. We also need to get
        //  a copy, in case we need to undo.
        //
        TOSFreeListItem fliRemove = m_colFreeList[c4FreeInd];
        TOSFreeListItem* pfliRemove = &m_colFreeList[c4FreeInd];
        try
        {
            // Bump the version for this object and we write it back out
            osiTmp.IncVersion();
            CommitStoreItem(m_flStore, osiTmp, mbufData, mbufKey, m_strStoreName);

            //
            //  And give back the original chunk. Last parm is 'no adjacent'
            //  so that it won't try to combine it with an adjacent free item.
            //  If we do that, we won't be able to easily recover, and it could
            //  combine with the free list item that we took (since it's not
            //  removed yet.)
            //
            c4GiveBackChunk
            (
                osiToUpdate.c4Offset()
                , osiToUpdate.c4StorageRequired()
                , kCIDLib::True
            );
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);

            //
            //  If it failed, we still have the ability to go back to where
            //  we were. We write back out the free list item and write back
            //  out the original data object.
            //
            WriteFreeListItem(m_flStore, fliRemove, m_strStoreName);
            CommitStoreItem(m_flStore, osiToUpdate, mbufOrgData, mbufKey, m_strStoreName);

            // And now we can rethrow it
            throw;
        }

        //
        //  Remove the free list item that we took. We must find it by address
        //  because the index we used above could have been affected by the
        //  giving back of the original chunk.
        //
        const tCIDLib::TCard4 c4Count = m_colFreeList.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            if (&m_colFreeList[c4Index] == pfliRemove)
            {
                m_colFreeList.RemoveAt(c4Index);
                pfliRemove = 0;
                break;
            }
        }

        #if CID_DEBUG_ON
        // We didn't find it in the list to remove, which is really bad
        if (pfliRemove)
        {
            facCIDObjStore().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kObjSErrs::errcDbg_FreeItemNotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , m_strStoreName
            );
        }
        #endif

        //
        //  Ok, it's good, so copy the updated store item back to the store
        //  list object that we are modifying.
        //
        osiToUpdate = osiTmp;
    }

    // Bump the overall index version number to indicate changes have been made
    m_c4IndexVersionNum++;
}



tCIDLib::TVoid
TCIDObjStoreImpl::ValidateStore(TTextOutStream* const pstrmOut)
{
    // Get the sequence data
    tCIDLib::TCard4     c4IndexItems;
    tCIDLib::TCard4     c4FreeItems;
    TVector<TOSSeqData> colSeqData;
    c4CreateSeqData(colSeqData, c4IndexItems, c4FreeItems, kCIDLib::c4MaxCard);

    // If we have an output stream, then output some header info
    if (pstrmOut)
    {
        *pstrmOut << L"Object Store Debug Dump\n"
                     L"----------------------------------\n"
                  << L"  Free Count:" << c4FreeItems
                  << L", Used Count:" << c4IndexItems << kCIDLib::NewLn
                  << L"   File Size:" << m_flStore.c8CurSize() << kCIDLib::DNewLn;
    }

    //
    //  We'll use a hash set to make sure that we don't have any duplicate keys in the
    //  store, treating them case sensitively or not depending on how we were created.
    //
    tCIDLib::TStrHashSet colUnique(109, new TStringKeyOps(m_bCaseSensitiveKeys));

    // Read the header
    TStoreHdr hdrStore;
    m_flStore.SetFilePos(0);
    if (m_flStore.c4ReadBuffer( &hdrStore
                                , sizeof(hdrStore)) != sizeof(hdrStore))
    {
        if (pstrmOut)
            *pstrmOut << L"Failed to read object store header\n";

        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_WriteStoreFlHdr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strStoreName
        );
    }

    // Make sure it has the expected magic values
    if ((hdrStore.m_c4MagicValue1 != kCIDObjStore_::c4UsedMagicVal)
    ||  (hdrStore.m_c4MagicValue2 != kCIDObjStore_::c4UsedMagicVal))
    {
        if (pstrmOut)
            *pstrmOut << L"Failed header magic values were wrong\n";

        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcData_BadHdrMagicVal
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strStoreName
        );
    }

    // And the offset should now be that of the first sequence entry
    if (m_flStore.c8CurPos() != colSeqData[0].c4Offset())
    {
        if (pstrmOut)
            *pstrmOut << L"1st item not at end of store header\n";

        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcDbg_GapAfterHeader
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strStoreName
        );
    }

    //
    //  Ok, let's move through the list and, for each entry, we will check
    //  the store file and make sure that the data that's supposed to be
    //  there is there.
    //
    const tCIDLib::TCard4   c4ItemHdrSize = sizeof(TStoreItemHdr);
    const tCIDLib::TCard4   c4FreeHdrSize = sizeof(TStoreItemFreeHdr);
    const tCIDLib::TCard4   c4Count = colSeqData.c4ElemCount();
    tCIDLib::TCard4         c4CurOfs   = sizeof(TStoreHdr);
    tCIDLib::TCard4         c4MagicVal;
    THeapBuf                mbufData(8192);
    TBinMBufInStream        strmTmp(&mbufData, 0, tCIDLib::EAdoptOpts::NoAdopt);
    TString                 strKey;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TOSSeqData& osdCur = colSeqData[c4Index];

        // Check for a gap in the file that's not covered
        if (osdCur.c4Offset() != c4CurOfs)
        {
            //
            //  If we have an output stream, then it's a reporting pass, so
            //  we report it but keep going. Else, we throw because this
            //  store does not validate.
            //
            if (pstrmOut)
            {
                *pstrmOut << L"   GAP: Before ";

                if (osdCur.eType() == tCIDObjStore_::EItemTypes::Index)
                    *pstrmOut << L"object ";
                else
                    *pstrmOut << L"free item ";

                *pstrmOut << L" at offset " << osdCur.c4Offset()
                          << L". Previous only reached to "
                          << c4CurOfs << kCIDLib::NewLn;
            }
             else
            {
                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcData_GapInData
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            }
        }

        // Output the info on this one
        if (pstrmOut)
        {
            if (osdCur.eType() == tCIDObjStore_::EItemTypes::Index)
                *pstrmOut << L"OBJECT: ";
            else
                *pstrmOut << L"  FREE: ";

            *pstrmOut << L"Offset:" << osdCur.c4Offset()
                      << L", Size:" << osdCur.c4Size();

            if (osdCur.eType() == tCIDObjStore_::EItemTypes::FreeList)
                *pstrmOut << L", Index:" << osdCur.c4ListInd();
        }

        //
        //  Seek to the current offset, and check the contents according
        //  to the type of item.
        //
        m_flStore.SetFilePos(osdCur.c4Offset());

        // Read in the first TCard4 value, the magic value
        if (m_flStore.c4ReadBuffer(&c4MagicVal
                                   , sizeof(tCIDLib::TCard4)) != sizeof(tCIDLib::TCard4))
        {
            if (pstrmOut)
                *pstrmOut << L"\nCould not read magic value item separator\n";

            facCIDObjStore().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kObjSErrs::errcIO_ReadMagicVal
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(c4CurOfs)
                , m_strStoreName
            );
        }

        if (((osdCur.eType() == tCIDObjStore_::EItemTypes::Index)
        &&   (c4MagicVal != kCIDObjStore_::c4UsedMagicVal))
        ||  ((osdCur.eType() == tCIDObjStore_::EItemTypes::FreeList)
        &&   (c4MagicVal != kCIDObjStore_::c4FreeMagicVal)))
        {
            if (pstrmOut)
                *pstrmOut << L"\nMagic value item separator was bad\n";

            facCIDObjStore().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kObjSErrs::errcData_BadMagicVal
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(c4CurOfs)
                , m_strStoreName
            );
        }

        //
        //  Looks like we have the right type, so lets check the data.
        //  Seek back so we can read the whole header.
        //
        m_flStore.SetFilePos(osdCur.c4Offset());
        if (osdCur.eType() == tCIDObjStore_::EItemTypes::Index)
        {
            // Its a stored item, so read in the header
            TStoreItemHdr hdrItem;
            if (m_flStore.c4ReadBuffer( &hdrItem
                                        , c4ItemHdrSize) != c4ItemHdrSize)
            {
                if (pstrmOut)
                    *pstrmOut << L"\nCould not read header for item\n";

                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcIO_ReadStoreItemHdr
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::OutOfSync
                    , m_strStoreName
                );
            }

            // Make sure the size matches the size of our sequence item
            const tCIDLib::TCard4 c4Needed = c4ItemHdrSize
                                             + hdrItem.m_c4Allocated
                                             + hdrItem.m_c4KeyBytes;
            if (c4Needed != osdCur.c4Size())
            {
                if (pstrmOut)
                    *pstrmOut << L"\nItem's needed size did not compute\n";

                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcData_BadItemSlotSize
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , TCardinal(c4Needed)
                    , TCardinal(osdCur.c4Size())
                    , m_strStoreName
                );
            }


            //
            //  Read in the key data, and stream it into a string, so that
            //  we can see if it's a dup.
            //
            if (m_flStore.c4ReadBuffer(mbufData
                                      , hdrItem.m_c4KeyBytes) != hdrItem.m_c4KeyBytes)
            {
                if (pstrmOut)
                    *pstrmOut << L"\nCould not read in item data\n";

                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcIO_ReadItemKey
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , osdCur.osiSrc().strKey()
                    , m_strStoreName
                );
            }
            strmTmp.Reset();
            strmTmp.SetEndIndex(hdrItem.m_c4KeyBytes);
            strmTmp >> strKey;

            if (colUnique.bHasElement(strKey))
            {
                if (pstrmOut)
                    *pstrmOut << L"\nItem key " << strKey << L" used multiple times\n";

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
            colUnique.objAdd(strKey);

            // Read the object data and check the hash
            if (m_flStore.c4ReadBuffer(mbufData
                                      , hdrItem.m_c4CurUsed) != hdrItem.m_c4CurUsed)
            {
                if (pstrmOut)
                    *pstrmOut << L"\nThe object's hash was bad\n";

                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcIO_ReadStoreItem
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , strKey
                    , m_strStoreName
                );
            }

            //
            //  Hash the data and make sure it matches the has that was
            //  stored in the two headers.
            //
            const tCIDLib::THashVal hshTest = mbufData.hshCalcHash
            (
                kCIDObjStore_::c4Modulus
                , 0
                , hdrItem.m_c4CurUsed
            );

            if (hshTest != hdrItem.m_hshData)
            {
                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcData_HashMismatch
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::OutOfSync
                    , strKey
                    , m_strStoreName
                );
            }

            if (pstrmOut)
                *pstrmOut << L" (" << strKey << L")" << kCIDLib::NewLn;
        }
         else
        {
            if (pstrmOut)
                *pstrmOut << kCIDLib::NewLn;

            // Its a free list item, so read in the header
            TStoreItemFreeHdr hdrFree;
            if (m_flStore.c4ReadBuffer( &hdrFree
                                        , c4FreeHdrSize) != c4FreeHdrSize)
            {
                if (pstrmOut)
                    *pstrmOut << L"Could not read header for free item\n";

                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcIO_ReadFreeStoreHdr
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::OutOfSync
                    , m_strStoreName
                );
            }

            // Make sure the size matches the size of our sequence item
            if (hdrFree.m_c4Size != osdCur.c4Size())
            {
                if (pstrmOut)
                    *pstrmOut << L"Free item size count mismatch\n";

                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcData_BadFreeSlotSize
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , TCardinal(hdrFree.m_c4Size)
                    , TCardinal(osdCur.c4Size())
                    , m_strStoreName
                );
            }

            // Make sure that the xor'd size is valid
            if ((hdrFree.m_c4Size ^ kCIDObjStore_::c4FreeMagicVal)
                                                != hdrFree.m_c4XORSize)
            {
                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcData_BadFreeHdr
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , TCardinal(c4CurOfs)
                    , m_strStoreName
                );
            }
        }

        // And move our current offset forward by the size
        c4CurOfs += osdCur.c4Size();
    }

    // And the offset we ended up at should be the size of the file
    if (m_flStore.c8CurSize() != c4CurOfs)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcData_NotAllStoreUsed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strStoreName
        );
    }

    // Flush the stream if we have one
    if (pstrmOut)
        pstrmOut->Flush();
}


// ---------------------------------------------------------------------------
//  TCIDObjStoreImpl: Private, static methods
// ---------------------------------------------------------------------------

//
//  Writes out the contents for a new empty repository. The caller can ask us
//  to create an initial free list and the space for it, in K.
//
tCIDLib::TVoid
TCIDObjStoreImpl::InitRepoFile(         TBinaryFile&    flToInit
                                , const TString&        strRepoName
                                , const tCIDLib::TCard4 c4InitFreeK)
{
    // Fill in a new store file header and write it out to the file
    TStoreHdr hdrNew = {0};
    hdrNew.m_c4MagicValue1 = kCIDObjStore_::c4UsedMagicVal;
    hdrNew.m_enctLastBackup = TTime::enctNow();
    hdrNew.m_c4MagicValue2 = kCIDObjStore_::c4UsedMagicVal;

    if (flToInit.c4WriteBuffer(&hdrNew, sizeof(hdrNew)) != sizeof(hdrNew))
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_WriteStoreFlHdr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllWritten
            , strRepoName
        );
    }

    // And create an initial free store, if any was requested
    if (c4InitFreeK)
    {
        const tCIDLib::TCard4 c4BufSz = 1024;
        tCIDLib::TCard1 ac1Buf[c4BufSz];
        TRawMem::SetMemBuf(ac1Buf, tCIDLib::TCard1(0), c4BufSz);
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4InitFreeK; c4Index++)
        {
            if (flToInit.c4WriteBuffer(ac1Buf, c4BufSz) != c4BufSz)
            {
                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcIO_CreateFreeSpace
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotAllWritten
                    , strRepoName
                );
            }
        }

        //
        //  Write out an initial free list item. It goes out just after
        //  the header, where the initial free list item was written.
        //
        TOSFreeListItem fliInit(1024 * c4InitFreeK, sizeof(hdrNew));
        WriteFreeListItem(flToInit, fliInit, strRepoName);
    }
}


//
//  These must be independent of the current file, because they are used
//  while doing backup or compaction to a new file. So we make them static
//  as a safety measure, since they cannot then refer to this.
//
tCIDLib::TVoid
TCIDObjStoreImpl::CommitStoreItem(          TBinaryFile&    flTarget
                                    , const TOSStoreItem&   osiToCommit
                                    , const TMemBuf&        mbufData
                                    , const TMemBuf&        mbufKey
                                    , const TString&        strRepoName)
{
    tCIDLib::TCard4 c4Written;

    //
    //  First fill in a store header and commit that to the store. All
    //  the info we need is in the index item we were passed.
    //
    TStoreItemHdr hdrItem;
    hdrItem.m_c4MagicValue = kCIDObjStore_::c4UsedMagicVal;
    hdrItem.m_c4Allocated = osiToCommit.c4Allocated();
    hdrItem.m_c4CurUsed = osiToCommit.c4CurUsed();
    hdrItem.m_c4VersionNum = osiToCommit.c4Version();
    hdrItem.m_c4KeyBytes = osiToCommit.c4KeyBytes();

    // Calc the hash of the data and put into the header
    hdrItem.m_hshData = mbufData.hshCalcHash
    (
        kCIDObjStore_::c4Modulus, 0, osiToCommit.c4CurUsed()
    );

    // Write the header
    flTarget.SetFilePos(osiToCommit.c4Offset());
    c4Written = flTarget.c4WriteBuffer(&hdrItem, sizeof(hdrItem));
    if (c4Written != sizeof(hdrItem))
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_WriteStoreItemHdr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , osiToCommit.strKey()
            , strRepoName
        );
    }

    // Write the key bytes
    c4Written = flTarget.c4WriteBuffer(mbufKey, osiToCommit.c4KeyBytes());
    if (c4Written != osiToCommit.c4KeyBytes())
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_WriteStoreItemKey
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , osiToCommit.strKey()
            , strRepoName
        );
    }

    // And now write the actual data
    c4Written = flTarget.c4WriteBuffer(mbufData, hdrItem.m_c4CurUsed);
    if (c4Written != hdrItem.m_c4CurUsed)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_WriteStoreItemData
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , osiToCommit.strKey()
            , strRepoName
        );
    }

    // And seek foward to the allocated size if larger
    if (hdrItem.m_c4Allocated > hdrItem.m_c4CurUsed)
        flTarget.c8OffsetFilePos(hdrItem.m_c4Allocated - hdrItem.m_c4CurUsed);
}


tCIDLib::TVoid
TCIDObjStoreImpl::WriteFreeListItem(        TBinaryFile&        flTarget
                                    , const TOSFreeListItem&    fliToWrite
                                    , const TString&            strRepoName)
{
    // If debugging, confirm that its not smaller now than the minimum
    #if CID_DEBUG_ON
    if (fliToWrite.c4Size() < kCIDObjStore_::c4MinFreeItemSize)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcDbg_SubMinFreeSlot
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppStatus
            , TCardinal(fliToWrite.c4Size())
            , strRepoName
            , TCardinal(kCIDObjStore_::c4MinFreeItemSize)
        );
    }
    #endif

    // Create a header that we write to this slot
    TStoreItemFreeHdr hdrWrite;
    hdrWrite.m_c4MagicValue = kCIDObjStore_::c4FreeMagicVal;
    hdrWrite.m_c4Size       = fliToWrite.c4Size();
    hdrWrite.m_c4XORSize    = hdrWrite.m_c4Size ^ kCIDObjStore_::c4FreeMagicVal;

    // Seek to the offset of this item, and write the header
    flTarget.SetFilePos(fliToWrite.c4Offset());
    const tCIDLib::TCard4 c4Written = flTarget.c4WriteBuffer
    (
        &hdrWrite, sizeof(hdrWrite)
    );

    if (c4Written != sizeof(hdrWrite))
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_WriteFreeStoreHdr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllWritten
            , strRepoName
        );
    }
}


// ---------------------------------------------------------------------------
//  TCIDObjStoreImpl: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is called when an existing store is opened. It will scan the store,
//  validating the format, and building the free and item lists as it goes.
//  When we are done, if it validates ok, then we have a valid index build
//  for the store.
//
tCIDLib::TVoid TCIDObjStoreImpl::BuildIndex()
{
    // Normally this would never be called again, but flush first in case
    m_colFreeList.RemoveAll();
    m_colStoreList.RemoveAll();

    // Get the file size, which we'll need a couple times
    const tCIDLib::TCard4 c4FlSize = tCIDLib::TCard4(m_flStore.c8CurSize());

    // Read the file in first
    TStoreHdr hdrStore;
    m_flStore.SetFilePos(0);
    if (m_flStore.c4ReadBuffer(&hdrStore, sizeof(hdrStore)) != sizeof(hdrStore))
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_ReadStoreFlHdr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strStoreName
        );
    }

    // Make sure it has the expected magic values
    if ((hdrStore.m_c4MagicValue1 != kCIDObjStore_::c4UsedMagicVal)
    ||  (hdrStore.m_c4MagicValue2 != kCIDObjStore_::c4UsedMagicVal))
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcData_BadHdrMagicVal
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , m_strStoreName
        );
    }

    // Looks ok, so copy out the fields that we need to maintain live
    m_c4IndexVersionNum = hdrStore.m_c4IndexVersionNum;

    //
    //  To reduce verbiage below, get the sizes of the headers we have
    //  to read.
    //
    const tCIDLib::TCard4 c4ItemHdrSz = sizeof(TStoreItemHdr);
    const tCIDLib::TCard4 c4FreeHdrSz = sizeof(TStoreItemFreeHdr);

    //
    //  Ok, let's move through the list and, for each entry, we will check
    //  the store file and make sure that the data that's supposed to be
    //  there is there. If it's good, we'll add the appropriate item to
    //  the appropriate list.
    //
    tCIDLib::TCard4         c4MagicVal;
    TStoreItemHdr           hdrItem;
    TStoreItemFreeHdr       hdrFree;
    THeapBuf                mbufData(8192);
    TBinMBufInStream        strmTmp(&mbufData, 0, tCIDLib::EAdoptOpts::NoAdopt);
    TString                 strKey;

    // We have to keep up with the offset on each round, before we change it
    tCIDLib::TCard4 c4CurOfs = tCIDLib::TCard4(m_flStore.c8CurPos());

    // Loop until we run out of file, or get an error
    tCIDLib::TCard4 c4DupBreak = 1;
    while (c4CurOfs < c4FlSize)
    {
        //
        //  Read in the first TCard4 value, which is the magic value of the
        //  housekeeping structure that must be here. It's value will tell us
        //  what type of item this will be.
        //
        if (m_flStore.c4ReadBuffer(&c4MagicVal, sizeof(tCIDLib::TCard4)) != sizeof(tCIDLib::TCard4))
        {
            facCIDObjStore().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kObjSErrs::errcIO_ReadMagicVal
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(c4CurOfs)
                , m_strStoreName
            );
        }

        //
        //  According to the value, read in one type or the other. We then
        //  have to read in the rest of the appropriate structure. Seek back
        //  so that we conveniently read the whole header.
        //
        m_flStore.c8OffsetFilePos(-tCIDLib::TInt4(sizeof(tCIDLib::TCard4)));
        if (c4MagicVal == kCIDObjStore_::c4UsedMagicVal)
        {
            // It's a store item, so read in the header
            if (m_flStore.c4ReadBuffer(&hdrItem, c4ItemHdrSz) != c4ItemHdrSz)
            {
                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcIO_ReadStoreItemHdr
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , m_strStoreName
                );
            }

            //
            //  Read in the key data, and stream it into a local string. We
            //  need this to create the index item later.
            //
            if (m_flStore.c4ReadBuffer(mbufData
                                      , hdrItem.m_c4KeyBytes) != hdrItem.m_c4KeyBytes)
            {
                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcIO_ReadItemKey
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , TString(L"Unknown")
                    , m_strStoreName
                );
            }
            strmTmp.Reset();
            strmTmp.SetEndIndex(hdrItem.m_c4KeyBytes);
            strmTmp >> strKey;

            // Read the object data and check the hash
            if (m_flStore.c4ReadBuffer(mbufData
                                      , hdrItem.m_c4CurUsed) != hdrItem.m_c4CurUsed)
            {
                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcIO_ReadStoreItem
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , strKey
                    , m_strStoreName
                );
            }

            //
            //  Hash the data and make sure it matches the hash that was
            //  stored in the two headers.
            //
            const tCIDLib::THashVal hshTest = mbufData.hshCalcHash
            (
                kCIDObjStore_::c4Modulus
                , 0
                , hdrItem.m_c4CurUsed
            );

            if (hshTest != hdrItem.m_hshData)
            {
                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcData_HashMismatch
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::OutOfSync
                    , strKey
                    , m_strStoreName
                );
            }

            TOSStoreItem& osiNew = m_colStoreList.objAdd
            (
                TOSStoreItem
                (
                    c4CurOfs
                    , hdrItem.m_c4CurUsed
                    , hdrItem.m_c4Allocated
                    , hdrItem.m_c4VersionNum
                    , strKey
                    , hdrItem.m_c4KeyBytes
                )
            );


            //
            //  If the allocation is larger than the used, then we have to
            //  move forward by the reserve space in order to get to the next
            //  item.
            //
            if (osiNew.c4Reserve())
                m_flStore.c8OffsetFilePos(osiNew.c4Reserve());
        }
         else if (c4MagicVal == kCIDObjStore_::c4FreeMagicVal)
        {
            // It's a free item, so read in the header
            if (m_flStore.c4ReadBuffer(&hdrFree, c4FreeHdrSz) != c4FreeHdrSz)
            {
                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcIO_ReadFreeStoreHdr
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::OutOfSync
                    , m_strStoreName
                );
            }

            // Make sure that the xor'd size is valid
            if ((hdrFree.m_c4Size ^ kCIDObjStore_::c4FreeMagicVal) != hdrFree.m_c4XORSize)
            {
                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcData_BadFreeHdr
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                    , TCardinal(c4CurOfs)
                    , m_strStoreName
                );
            }

            // Looks ok, so add an item to the free list
            m_colFreeList.objAdd(TOSFreeListItem(hdrFree.m_c4Size, c4CurOfs));

            // Move forward by this item's size, minuse the header we read
            m_flStore.c8OffsetFilePos(hdrFree.m_c4Size - c4FreeHdrSz);
        }
         else
        {
            facCIDObjStore().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kObjSErrs::errcData_UnknownStoreMarker
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(c4CurOfs)
                , m_strStoreName
            );
        }

        // Save the new current position
        c4CurOfs = tCIDLib::TCard4(m_flStore.c8CurPos());
    }
}



//
//  To compact the store, we don't both to try to move things around, which
//  has too much possibility for getting things messed up. Instead, we just
//  write out a new file, which only contains the used parts of the original
//  file. It's quick, easy, and we can ensure that we get it written out and
//  validate it before we toss the old one, so we never have to worry about
//  what will happen if we get killed in the process.
//
//  We'll then add a single new free item at the end of the file which has
//  enough space for the passed need, plus a minimum expansion block size.
//
//  Since we are going through the trouble, we treat this like a backup
//  operation, leaving the current store as the first backup.
//
tCIDLib::TVoid TCIDObjStoreImpl::Compact(const tCIDLib::TCard4 c4Needed)
{
    // Set up the names of the files's we'll use
    TPathStr pathTmpFl = m_strStoreFile;
    pathTmpFl.bRemoveExt();
    pathTmpFl.AppendExt(L"TmpStore");

    TPathStr pathBack1 = m_strStoreFile;
    pathBack1.bRemoveExt();
    pathBack1.AppendExt(L"StoreBack1");

    TPathStr pathBack2 = m_strStoreFile;
    pathBack2.bRemoveExt();
    pathBack2.AppendExt(L"StoreBack2");

    //
    //  Remove any previous temp file, removing any file attrs that might
    //  prevent us from doing it first.
    //
    if (TFileSys::bExists(pathTmpFl))
    {
        TFileSys::SetPerms(pathTmpFl, tCIDLib::EFilePerms::AllWE);
        TFileSys::DeleteFile(pathTmpFl);
    }

    //
    //  Create a local temp file and write out all the current items to it.
    //  We don't even have to modify the item headers in the process, since
    //  they don't contain the storage offset when on disk (since that is
    //  implicit in where they are stored.)
    //
    {
        TBinaryFile flTmp(pathTmpFl);
        flTmp.Open
        (
            tCIDLib::EAccessModes::Excl_ReadWrite
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::AllOwnerAccess
            , tCIDLib::EFileFlags::SafeStream
        );

        // Create initial new contents for this new file, with no initial free
        InitRepoFile(flTmp, pathTmpFl, 0);

        //
        //  Create a cursor on the item store item collection, and run through
        //  them, writing each one out. We'll need buffers for reading in the
        //  key and data buffers of each item, and then to pass to the commit
        //  method to write it out.
        //
        TStoreList::TCursor cursItems(&m_colStoreList);
        THeapBuf mbufData(32 * 1024);
        if (cursItems.bIsValid())
        {
            THeapBuf mbufKey(1024);
            TStoreItemHdr hdrCur;

            for (; cursItems; ++cursItems)
            {
                // Get a copy of the current item
                TOSStoreItem osiCur = *cursItems;

                // Read this one in from the current store
                LoadItemData(osiCur, mbufData, mbufKey, hdrCur);

                //
                //  Validate that the sizes and version that we read in
                //  matches what is in the store item.
                //
                if ((osiCur.c4CurUsed() != hdrCur.m_c4CurUsed)
                ||  (osiCur.c4Version() != hdrCur.m_c4VersionNum)
                ||  (osiCur.c4Allocated() != hdrCur.m_c4Allocated)
                ||  (osiCur.c4KeyBytes() != hdrCur.m_c4KeyBytes))
                {
                    //
                    //  If it doesn't, then log a message and leave it out of
                    //  the backup.
                    //
                    if (facCIDObjStore().bLogWarnings())
                    {
                        facCIDObjStore().LogMsg
                        (
                            CID_FILE
                            , CID_LINE
                            , kObjSErrs::errcBack_BadKey
                            , tCIDLib::ESeverities::Warn
                            , tCIDLib::EErrClasses::Format
                            , osiCur.strKey()
                        );
                    }
                }
                 else
                {
                    // Update the copy with the current target file offset
                    osiCur.c4Offset(tCIDLib::TCard4(flTmp.c8CurPos()));

                    // And write the data to the new spot in the temp store
                    CommitStoreItem(flTmp, osiCur, mbufData, mbufKey, pathTmpFl);
                }
            }
        }

        //
        //  Now it's just big enough to hold the actual data. So we need to
        //  write out a free list item at the end that is the needed size
        //  plus the minimum expansion size. So remember the current offset,
        //  and then write out the free K we need.
        //
        const tCIDLib::TCard4 c4FreeStart = tCIDLib::TCard4(flTmp.c8CurPos());
        const tCIDLib::TCard4 c4New4K
        (
            ((kCIDObjStore_::c4MinExpandSize + c4Needed) / 4096) + 1
        );
        mbufData.Set(0, 0, 4096);
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4New4K; c4Index++)
            flTmp.c4WriteBuffer(mbufData, 4096);

        // Go back to the start of the free area and write a header
        TOSFreeListItem fliFree(c4New4K * 4096, c4FreeStart);
        WriteFreeListItem(flTmp, fliFree, pathTmpFl);

        // And flush it all out. It will close when we exist this scope
        flTmp.Flush();
    }

    // If there is a second backup file, get rid of it
    if (TFileSys::bExists(pathBack2))
    {
        TFileSys::SetPerms(pathBack2, tCIDLib::EFilePerms::AllWE);
        TFileSys::DeleteFile(pathBack2);
    }

    // If there is a first backup, rename it to the second backup
    if (TFileSys::bExists(pathBack1))
        TFileSys::Rename(pathBack1, pathBack2);

    //
    //  Copy the current file to the backup1, and move the new file in. We
    //  need to close the current file first.
    //
    m_flStore.Close();
    TFileSys::ReplaceFile(m_strStoreFile, pathTmpFl, pathBack1);

    //
    //  Open up the new file as the current file. If this fails, then
    //  put the old one back.
    //
    try
    {
        Open();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        if (facCIDObjStore().bLogFailures())
        {
            facCIDObjStore().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kObjSErrs::errcBack_NewCompFailed
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppStatus
            );
        }

        //
        //  Try to open the original one back up again. If that fails, then
        //  just start over.
        //
        try
        {
            // Close the new file and delete it, and copy the old one back
            m_flStore.Close();
            TFileSys::DeleteFile(m_strStoreFile);
            TFileSys::Rename(pathBack1, m_strStoreFile);
            Open();
        }

        catch(TError& errNested)
        {
            if (facCIDObjStore().bLogFailures())
            {
                if (!errNested.bLogged())
                {
                    errNested.AddStackLevel(CID_FILE, CID_LINE);
                    TModule::LogEventObj(errNested);
                }

                facCIDObjStore().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcBack_RollbackFailed
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::AppStatus
                );
            }

            m_flStore.Close();
            TPathStr pathSave = m_strStoreFile;
            pathSave.bRemoveExt();
            pathSave.AppendExt(L"SavedStore");
            TFileSys::Rename(m_strStoreFile, pathSave);

            // Just create a new one
            Create();
        }

        // And rethrow now
        throw;
    }
}


tCIDLib::TVoid TCIDObjStoreImpl::Create()
{
    try
    {
        m_flStore.Open
        (
            tCIDLib::EAccessModes::Excl_ReadWrite
            , tCIDLib::ECreateActs::CreateIfNew
            , tCIDLib::EFilePerms::AllOwnerAccess
            , tCIDLib::eOREnumBits
              (
                tCIDLib::EFileFlags::WriteThrough, tCIDLib::EFileFlags::RandomAccess
              )
        );
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_CreateStore
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::InitFailed
            , m_strStoreName
        );
    }

    try
    {
        // Create an empty repositroy with a 128K initial free list
        InitRepoFile(m_flStore, m_strStoreName, 128);

        //
        //  And put a free list entry into our free list to represent the init
        //  free space.
        //
        m_colFreeList.objAdd(TOSFreeListItem(128 * 1024, sizeof(TStoreHdr)));
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // Get rid of the file we created and rethrow
        m_flStore.Close();
        TFileSys::DeleteFile(m_strStoreFile);

        throw;
    }

    //
    //  And flush the everything out to disk now, so a good initial state
    //  is on disk.
    //
    FlushStore();
}


//
//  This method will compact the store if it can get the indicated needed
//  bytes and the needed bytes are less than 64K (or, if the needed bytes
//  are less than 64K, if it can free up 64K, since it's probably not worth
//  doing otherwise.)
//
//  NOTE: This can only be called before the addition of a new object, not
//        during updating an object, since it can invalidate existing
//        information if it compacts.
//
tCIDLib::TVoid
TCIDObjStoreImpl::ExpandOrCompact(const tCIDLib::TCard4 c4Needed)
{
    //
    //  Though we make an attempt to conglomerate free items as we go, we
    //  can inevitably get fragementation such that there are free items
    //  beside each other that can be put together to provide the space
    //  needed.
    //
    tCIDLib::TCard4 c4Count = m_colFreeList.c4ElemCount();
    if (c4Count > 1)
    {
        tCIDLib::TCard4 c4Index = 0;
        TOSFreeListItem fliTmp;
        while (c4Index < c4Count - 1)
        {
            TOSFreeListItem& fliCur  = m_colFreeList[c4Index];
            TOSFreeListItem& fliNext = m_colFreeList[c4Index + 1];

            //
            //  If the end of the current is the start of the next, we can
            //  conglomerate them, so remove the next item and reduce the
            //  count value. Else, just move up to the next one.
            //
            if (fliCur.c4AfterIt() == fliNext.c4Offset())
            {
                //
                //  Write this guy out again, so that we own this new free
                //  chunk. Use a temp, so that if it fails for some reason,
                //  we've done nothing dangerous.
                //
                fliTmp.Set(fliCur.c4Size() + fliNext.c4Size(), fliCur.c4Offset());
                WriteFreeListItem(m_flStore, fliTmp, m_strStoreName);

                //
                //  It worked, so update our index. We give the current item
                //  the size of the next one, remove the next one, and reduce
                //  the local loop target count.
                //
                fliCur.AddToSize(fliNext);
                m_colFreeList.RemoveAt(c4Index + 1);
                c4Count--;
            }
             else
            {
                // Not adjacent, so just move up
                c4Index++;
            }
        }
    }

    //
    //  Now see if we have one big enough for the needed new item. If so,
    //  we are done so return now. Else, keep up with the amount of free
    //  item space we have as we go.
    //
    tCIDLib::TCard4 c4FreeAvail = 0;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TOSFreeListItem& fliCur = m_colFreeList[c4Index];

        if (fliCur.c4Size() >= c4Needed)
            return;
        c4FreeAvail += fliCur.c4Size();
    }

    //
    //  No item available to meet the need. If we have more than 64K of
    //  free space and the needed is less than that, then compact the
    //  repository, else expand the store.
    //
    if ((c4FreeAvail > 0x10000) && (c4Needed <= 0x10000))
        Compact(c4Needed);
    else
        ExpandStore(c4Needed);
}


//
//  This method writes a new chunk of data to the end of the file, and creates
//  a new free list item for it. Note that if we have to expand, we always
//  expand by at least the min size, above what the caller indicate he needs.
//
//  NOTE: This method must never invalidate any existing content. It can only
//        add new space to the end of the store. It's called during the update
//        of objects and if it changed any existing stuff it could cause the
//        update code to make a mess of things.
//
tCIDLib::TVoid TCIDObjStoreImpl::ExpandStore(const tCIDLib::TCard4 c4Needed)
{
    const tCIDLib::TCard8 c8End = m_flStore.c8CurSize();
    const tCIDLib::TCard8 c8NewEnd = c8End + c4Needed + kCIDObjStore_::c4MinExpandSize;

    // Write out up to 8K chunks at a time
    const tCIDLib::TCard8 c8ChunkSize = 8192;
    tCIDLib::TCard1 ac1Buf[c8ChunkSize];
    TRawMem::SetMemBuf(ac1Buf, tCIDLib::TCard1(0), c8ChunkSize);

    try
    {
        tCIDLib::TCard8 c8Inc = c8End;
        m_flStore.SetFilePos(c8End);
        while (c8Inc < c8NewEnd)
        {
            // Either write a whole chunk or a partial
            tCIDLib::TCard8 c8CurSz;
            if (c8NewEnd - c8Inc < c8ChunkSize)
                c8CurSz = c8NewEnd - c8Inc;
            else
                c8CurSz = c8ChunkSize;

            const tCIDLib::TCard4 c4Write = tCIDLib::TCard4(c8CurSz);
            if (m_flStore.c4WriteBuffer(ac1Buf, c4Write) != c4Write)
            {
                facCIDObjStore().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kObjSErrs::errcIO_CreateFreeSpace
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotAllWritten
                    , m_strStoreName
                );
            }
            c8Inc += c8CurSz;
        }

        // Ok, lets add a free list item for this new chunk
        c4GiveBackChunk
        (
            tCIDLib::TCard4(c8End)
            , tCIDLib::TCard4(c8NewEnd - c8End)
        );
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // Truncate the file back to the original
        m_flStore.TruncateAt(c8End);
        throw;
    }
}


//
//  These methods load up the data for a given item in various ways,
//  getting more or less information, and doing appropriate validity
//  checks.
//
tCIDLib::TVoid
TCIDObjStoreImpl::LoadItemData( const   TOSStoreItem&   osiToLoad
                                ,       TMemBuf&        mbufData
                                ,       TMemBuf&        mbufKey)
{
    // Just use a local store item header and call the other version
    TStoreItemHdr hdrTmp;
    LoadItemData(osiToLoad, mbufData, mbufKey, hdrTmp);
}

tCIDLib::TVoid
TCIDObjStoreImpl::LoadItemData( const   TOSStoreItem&   osiToLoad
                                ,       TMemBuf&        mbufData)
{
    // Just use a local store item header and call the other version
    TStoreItemHdr hdrTmp;
    LoadItemData(osiToLoad, mbufData, hdrTmp);
}

tCIDLib::TVoid
TCIDObjStoreImpl::LoadItemData( const   TOSStoreItem&   osiToLoad
                                ,       TMemBuf&        mbufData
                                ,       TMemBuf&        mbufKey
                                ,       TStoreItemHdr&  hdrToFill)
{
    //
    //  Seek to the item's offset, and read in the store header for a
    //  sanity check.
    //
    m_flStore.SetFilePos(osiToLoad.c4Offset());
    tCIDLib::TCard4 c4Read = m_flStore.c4ReadBuffer
    (
        &hdrToFill
        , sizeof(TStoreItemHdr)
    );

    if (c4Read != sizeof(TStoreItemHdr))
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_ReadStoreItemHdr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , osiToLoad.strKey()
            , m_strStoreName
        );
    }

    if (osiToLoad.c4Allocated() != hdrToFill.m_c4Allocated)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcData_AllocMismatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(osiToLoad.c4Allocated())
            , TCardinal(hdrToFill.m_c4Allocated)
            , m_strStoreName
            , osiToLoad.strKey()
        );
    }

    if (osiToLoad.c4CurUsed() != hdrToFill.m_c4CurUsed)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcData_UsedMismatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(osiToLoad.c4CurUsed())
            , TCardinal(hdrToFill.m_c4CurUsed)
            , m_strStoreName
            , osiToLoad.strKey()
        );
    }

    // Read in the key data, and make sure we agree on it's size
    if (osiToLoad.c4KeyBytes() != hdrToFill.m_c4KeyBytes)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcData_KeySzMismatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(osiToLoad.c4KeyBytes())
            , TCardinal(hdrToFill.m_c4KeyBytes)
            , m_strStoreName
        );
    }

    c4Read = m_flStore.c4ReadBuffer(mbufKey, hdrToFill.m_c4KeyBytes);
    if (c4Read != hdrToFill.m_c4KeyBytes)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_ReadItemKey
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , osiToLoad.strKey()
            , m_strStoreName
        );
    }

    // Ok, it all checks out, so read in the data
    c4Read = m_flStore.c4ReadBuffer(mbufData, hdrToFill.m_c4CurUsed);
    if (c4Read != hdrToFill.m_c4CurUsed)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_ReadStoreItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , osiToLoad.strKey()
            , m_strStoreName
        );
    }

    //
    //  Hash the data and make sure it matches the has that was stored
    //  in the store header.
    //
    const tCIDLib::THashVal hshTest = mbufData.hshCalcHash
    (
        kCIDObjStore_::c4Modulus
        , 0
        , hdrToFill.m_c4CurUsed
    );

    if (hshTest != hdrToFill.m_hshData)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcData_HashMismatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , osiToLoad.strKey()
            , m_strStoreName
        );
    }
}

tCIDLib::TVoid
TCIDObjStoreImpl::LoadItemData( const   TOSStoreItem&   osiToLoad
                                ,       TMemBuf&        mbufData
                                ,       TStoreItemHdr&  hdrToFill)
{
    //
    //  Seek to the item's offset, and read in the store header for a
    //  sanity check.
    //
    m_flStore.SetFilePos(osiToLoad.c4Offset());
    tCIDLib::TCard4 c4Read = m_flStore.c4ReadBuffer
    (
        &hdrToFill
        , sizeof(TStoreItemHdr)
    );

    if (c4Read != sizeof(TStoreItemHdr))
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_ReadStoreItemHdr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , osiToLoad.strKey()
            , m_strStoreName
        );
    }

    if (osiToLoad.c4Allocated() != hdrToFill.m_c4Allocated)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcData_AllocMismatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(osiToLoad.c4Allocated())
            , TCardinal(hdrToFill.m_c4Allocated)
            , m_strStoreName
            , osiToLoad.strKey()
        );
    }

    if (osiToLoad.c4CurUsed() != hdrToFill.m_c4CurUsed)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcData_UsedMismatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(osiToLoad.c4CurUsed())
            , TCardinal(hdrToFill.m_c4CurUsed)
            , m_strStoreName
            , osiToLoad.strKey()
        );
    }

    // Ok, skip over the key data, but make sure we agree on it's size
    if (osiToLoad.c4KeyBytes() != hdrToFill.m_c4KeyBytes)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcData_KeySzMismatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(osiToLoad.c4KeyBytes())
            , TCardinal(hdrToFill.m_c4KeyBytes)
            , m_strStoreName
        );
    }
    m_flStore.c8OffsetFilePos(tCIDLib::TInt4(hdrToFill.m_c4KeyBytes));

    // Ok, it all checks out, so read in the data
    c4Read = m_flStore.c4ReadBuffer(mbufData, hdrToFill.m_c4CurUsed);
    if (c4Read != hdrToFill.m_c4CurUsed)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_ReadStoreItem
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , osiToLoad.strKey()
            , m_strStoreName
        );
    }

    //
    //  Hash the data and make sure it matches the has that was stored
    //  in the store header.
    //
    const tCIDLib::THashVal hshTest = mbufData.hshCalcHash
    (
        kCIDObjStore_::c4Modulus
        , 0
        , hdrToFill.m_c4CurUsed
    );

    if (hshTest != hdrToFill.m_hshData)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcData_HashMismatch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , osiToLoad.strKey()
            , m_strStoreName
        );
    }
}



//
//  In this case, a pre-existing store file has been found, so we need to
//  open it and load it up, and build an index based on what we find there.
//
tCIDLib::TVoid TCIDObjStoreImpl::Open()
{
    try
    {
        m_flStore.Open
        (
            tCIDLib::EAccessModes::Excl_ReadWrite
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::AllOwnerAccess
            , tCIDLib::eOREnumBits
              (
                tCIDLib::EFileFlags::WriteThrough, tCIDLib::EFileFlags::RandomAccess
              )
        );
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        // Close the index  file back down
        m_flStore.Close();

        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcIO_OpenStore
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::InitFailed
            , m_strStoreName
        );
    }

    // Now build the index from the store data
    BuildIndex();
}


