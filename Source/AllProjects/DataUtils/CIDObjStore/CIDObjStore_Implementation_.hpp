//
// FILE NAME: CIDObjStore_Implementation_.hpp
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
//  This is the header for the CIDObjStore_Implementation.cpp file (and it's
//  overflow files), which provides the internal implementation of the object
//  store. The public class just provides synchronization, and delegates
//  everything to us. This keeps all of the details completely out of public
//  view and not exported.
//
//  This class implements a kind of 'bento box' storage. So, in the actual
//  store file, we use a couple raw structures to hold the house keeping
//  data, which is fixed in size for sanity's sake. But the data itself is
//  just an MStreamable object that's been flattened.
//
//  The index is never written. It's just determined from the store itself
//  when the store is opened, and is maintained in memory. So it can be
//  purely object oriented.
//
// CAVEATS/GOTCHAS:
//
//  1)  We assume the public interface always locks before calling any
//      methods on this class.
//
//  2)  We assume that a store is never cross platform directly. The user
//      data stored might come in from various clients via an ORB interface,
//      but the store itself is not platform portable, because of the fact
//      that the housekeeping data is not flattened, but just written as is
//      from structures.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: TCIDObjStoreImpl
//  PREFIX: ost
// ---------------------------------------------------------------------------
class TCIDObjStoreImpl : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Class types
        //
        //  These must be specifically packed, since they represent direct
        //  formats on disk. The index list at the end of the index file
        //  structure is open ended, so we just indicate one item.
        // -------------------------------------------------------------------
        #pragma CIDLIB_PACK(1)

        //
        //  A small header for the overall store file. We put out a magic value
        //  to help try to identify attempts to open a non-store file. The last
        //  time that this store was backed up is store here so that it can
        //  be reloaded when the store is opened.
        //
        //  We reserve some bytes for later expansion of this without having
        //  to rewrite the store.
        //
        struct TStoreHdr
        {
            tCIDLib::TCard4         m_c4MagicValue1;
            tCIDLib::TEncodedTime   m_enctLastBackup;
            tCIDLib::TCard4         m_c4IndexVersionNum;
            tCIDLib::TCard1         m_ac1Reserved[512];
            tCIDLib::TCard4         m_c4MagicValue2;
        };


        //
        //  This is stored before each item in the store. The data we put in
        //  here is driven by the requirement that, if necessary, we can
        //  rebuild the whole index by tracing through the store file.
        //
        //  The magic value for a free list item is 0xFEDCCDEF and the value
        //  for a stored object is 0xDEADBEEF.
        //
        //  The key is always streamed out first (a TString), and the last
        //  field, m_c4KeyBytes, indicates how many bytes are in that part of
        //  the object, so that we can scan through the store and read in
        //  the headers and the keys, so that we can build the index.
        //
        struct TStoreItemHdr
        {
            tCIDLib::TCard4         m_c4MagicValue;
            tCIDLib::TCard4         m_c4Allocated;
            tCIDLib::TCard4         m_c4CurUsed;
            tCIDLib::TCard4         m_c4VersionNum;
            tCIDLib::THashVal       m_hshData;
            tCIDLib::TCard4         m_c4KeyBytes;
        };

        //
        //  For a free list item, we write out just the magic value and
        //  size fields, which gives us enough info to recognize the free
        //  item, and to know how long it is. For actual items in the store,
        //  we write the store item header, above. To avoid accidently
        //  seeing junk data, we also store the size XORd with the magic
        //  value.
        //
        struct TStoreItemFreeHdr
        {
            tCIDLib::TCard4         m_c4MagicValue;
            tCIDLib::TCard4         m_c4Size;
            tCIDLib::TCard4         m_c4XORSize;
        };

        #pragma CIDLIB_POPPACK


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDObjStoreImpl
        (
            const   TString&                strPath
            , const TString&                strStoreName
            , const tCIDObjStore::EFlags    eFlags
        );

        ~TCIDObjStoreImpl();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddObject
        (
            const   TString&                strKey
            , const TMemBuf&                mbufKey
            , const tCIDLib::TCard4         c4KeySize
            , const TMemBuf&                mbufData
            , const tCIDLib::TCard4         c4DataSize
            , const tCIDLib::TCard4         c4Reserve
        );

        tCIDLib::TBoolean bAddOrUpdate
        (
            const   TString&                strKey
            ,       tCIDLib::TCard4&        c4Version
            , const TMemBuf&                mbufKey
            , const tCIDLib::TCard4         c4KeySize
            , const TMemBuf&                mbufData
            , const tCIDLib::TCard4         c4DataSize
            , const tCIDLib::TCard4         c4Reserve
        );

        tCIDLib::TBoolean bAllObjectsUnder
        (
            const   TString&                strStartScope
            ,       tCIDLib::TStrList&      colPathsFound
        )   const;

        tCIDLib::TBoolean bDeleteObjectIfExists
        (
            const   TString&                strKey
        );

        tCIDLib::TBoolean bFindNameUnder
        (
            const   TString&                strName
            , const TString&                strStartScope
            ,       tCIDLib::TStrList&      colPathsFound
        )   const;

        tCIDLib::TBoolean bInitialize();

        tCIDLib::TBoolean bKeyExists
        (
            const   TString&                strKey
        )   const;

        tCIDLib::TBoolean bKeyExists
        (
            const   TString&                strKey
            ,       tCIDLib::TCard4&        c4Version
        )   const;

        tCIDLib::TVoid BackupStore();

        tCIDLib::TCard4 c4ObjectsInStore() const;

        tCIDLib::TCard4 c4QueryKeysInScope
        (
            const   TString&                strScope
            ,       tCIDLib::TStrCollect&   colToFill
        );

        tCIDLib::TCard4 c4QueryObjectsInScope
        (
            const   TString&                strScope
            ,       tCIDLib::TStrCollect&   colToFill
        );

        tCIDLib::TCard4 c4QuerySubScopes
        (
            const   TString&                strScope
            ,       tCIDLib::TStrCollect&   colToFill
        );

        tCIDLib::TCard4 c4UpdateObject
        (
            const   TString&                strKey
            , const TMemBuf&                mbufData
            , const tCIDLib::TCard4         c4Size
        );

        tCIDLib::TCard4 c4UpdateObject
        (
                    TOSStoreItem&           osiToUpdate
            , const TMemBuf&                mbufData
            , const tCIDLib::TCard4         c4Size
        );

        tCIDLib::TVoid Close();

        tCIDLib::TVoid DeleteObject
        (
            const   TString&                strKey
        );

        tCIDLib::TVoid DeleteScope
        (
            const   TString&                strScopeName
        );

        tCIDLib::ELoadRes eReadObject
        (
            const   TString&                strKey
            ,       tCIDLib::TCard4&        c4Version
            ,       TMemBuf&                mbufData
            ,       tCIDLib::TCard4&        c4DataSize
            ,       TMemBuf&                mbufKey
            ,       tCIDLib::TCard4&        c4KeySize
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        tCIDLib::ELoadRes eReadObject
        (
            const   TString&                strKey
            ,       tCIDLib::TCard4&        c4Version
            ,       TMemBuf&                mbufData
            ,       tCIDLib::TCard4&        c4DataSize
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        tCIDLib::TEncodedTime enctLastBackup() const;

        tCIDLib::TVoid FlushStore();

        tCIDLib::TVoid QueryAllKeys
        (
                    tCIDLib::TStrCollect&   colToFill
        );

        tCIDLib::TVoid UpdateItemData
        (
                    TOSStoreItem&           osiToUpdate
            , const TMemBuf&                mbufData
            , const tCIDLib::TCard4         c4Size
        );

        tCIDLib::TVoid ValidateStore
        (
                    TTextOutStream* const   pstrmOut
        );


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        using TFreeList = TVector<TOSFreeListItem>;
        using TStoreList = TKeyedHashSet<TOSStoreItem,TString,TStringKeyOps>;


        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid CommitStoreItem
        (
                    TBinaryFile&            flTarget
            , const TOSStoreItem&           osiToCommit
            , const TMemBuf&                mbufData
            , const TMemBuf&                mbufKey
            , const TString&                strRepoName
        );

        static tCIDLib::TVoid InitRepoFile
        (
                    TBinaryFile&            flToInit
            , const TString&                strRepoName
            , const tCIDLib::TCard4         c4InitFreeK
        );

        static tCIDLib::TVoid WriteFreeListItem
        (
                    TBinaryFile&            flTarget
            , const TOSFreeListItem&        fliToWrite
            , const TString&                strRepoName
        );


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bLoadItemData
        (
            const   TOSStoreItem&           osiToLoad
            ,       TMemBuf&                mbufDataFill
            ,       TMemBuf&                mbufKeyFill
        );

        tCIDLib::TBoolean bLoadItemData
        (
            const   TOSStoreItem&           osiToLoad
            ,       TMemBuf&                mbufDataFill
        );

        tCIDLib::TBoolean bLoadItemData
        (
            const   TOSStoreItem&           osiToLoad
            ,       TMemBuf&                mbufToFill
            ,       TMemBuf&                mbufKeyFill
            ,       TStoreItemHdr&          hdrToFill
        );

        tCIDLib::TBoolean bLoadItemData
        (
            const   TOSStoreItem&           osiToLoad
            ,       TMemBuf&                mbufToFill
            ,       TStoreItemHdr&          hdrToFill
        );

        tCIDLib::TVoid BuildIndex();

        tCIDLib::TCard4 c4CreateSeqData
        (
                    TVector<TOSSeqData>&    colToFill
            ,       tCIDLib::TCard4&        c4IndexItems
            ,       tCIDLib::TCard4&        c4FreeItems
            , const tCIDLib::TCard4         c4TargetOfs
        );

        tCIDLib::TCard4 c4GiveBackChunk
        (
            const   tCIDLib::TCard4         c4Offset
            , const tCIDLib::TCard4         c4Size
            , const tCIDLib::TBoolean       bNoAdjacent = kCIDLib::False
        );

        tCIDLib::TCard4 c4FindFreeSpace
        (
            const   tCIDLib::TCard4         c4Needed
            ,       tCIDLib::TCard4&        c4ActualSz
            ,       tCIDLib::TCard4&        c4Offset
        );

        tCIDLib::TVoid Compact
        (
            const   tCIDLib::TCard4         c4Needed
        );

        tCIDLib::TVoid Create();

        tCIDLib::TVoid ExpandOrCompact
        (
            const   tCIDLib::TCard4         c4Needed
        );

        tCIDLib::TVoid ExpandStore
        (
            const   tCIDLib::TCard4         c4Needed
        );

        tCIDLib::TVoid Open();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bCaseSensitiveKeys
        //      When we are created the caller tells us whether we should treat keys as
        //      case sensitive or not. If case sensitive, you could have two keys that are
        //      the same except for case, which would normally not be desirable.
        //
        //  m_bRecoveryMode
        //      Makes us try to ignore errors that we can reasonably ignore and load as
        //      much data as possible.
        //
        //  m_c4IndexVersionNum
        //      This is bumped every time the index is modified, so that the
        //      outside world can do a quick check for any changes in the
        //      store as a whole.
        //
        //  m_colFreeList
        //      A collection of free list items that contains an item for each
        //      range of unused bytes in the store file.
        //
        //  m_colStoreList
        //      A collection of store items, each of which represents one stored
        //      item in the store. This one is a keyed hash set, since we find
        //      items by their path string.
        //
        //  m_enctLastBackup
        //      This is read in from the store on startup, and we update it and
        //      write it back out every time we back up the store. This allows
        //      the outside world to know how long it's been since a backup.
        //
        //  m_flStore
        //      The store file object. This is what we use to read/write the
        //      store data.
        //
        //  m_strStoreFile
        //      The path to the store file. It's set up during init.
        //
        //  m_strStoreName
        //      The name of the store. This is used to create the file name
        //      and in error messages and the creation of some named
        //      resources. It should be alphanumeric, no spaces.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bCaseSensitiveKeys;
        tCIDLib::TBoolean       m_bRecoveryMode;
        tCIDLib::TCard4         m_c4IndexVersionNum;
        TFreeList               m_colFreeList;
        TStoreList              m_colStoreList;
        tCIDLib::TEncodedTime   m_enctLastBackup;
        TBinaryFile             m_flStore;
        TString                 m_strStoreFile;
        TString                 m_strStoreName;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDObjStoreImpl,TObject)
};

#pragma CIDLIB_POPPACK


