//
// FILE NAME: CIDObjStore_ObjStore.hpp
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
//  This is the header for the CIDObjStore_ObjStore.cpp file, which implements
//  the core class for the object store. This class is usually  derived from
//  and some functionality specialized, but it provides the bulk of the grunt
//  work.
//
//  This file works through an internal TObjStoreImpl object, which manages
//  the files. At this level we just provide some public APIs that are not
//  specific to the types stored, and we provide the synchronization of access
//  to the underlying implementation object.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TCIDObjStoreImpl;


// ---------------------------------------------------------------------------
//   CLASS: TObjStoreEngine
//  PREFIX: ose
// ---------------------------------------------------------------------------
class CIDOBJSTOREEXP TCIDObjStore : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDObjStore
        (
            const   tCIDObjStore::EFlags    eFlags = tCIDObjStore::EFlags::None
        );

        TCIDObjStore(const TCIDObjStore&) = delete;
        TCIDObjStore(TCIDObjStore&&) = delete;

        ~TCIDObjStore();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDObjStore& operator=(const TCIDObjStore&) = delete;
        TCIDObjStore& operator=(TCIDObjStore&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddObject
        (
            const   TString&                strKey
            , const MStreamable&            strmblToWrite
            , const tCIDLib::TCard4         c4Reserve = 0
        );

        tCIDLib::TVoid AddObjectDirect
        (
            const   TString&                strKey
            , const THeapBuf&               mbufToWrite
            , const tCIDLib::TCard4         c4Bytes
            , const tCIDLib::TCard4         c4Reserve = 0
        );

        tCIDLib::TBoolean bAddOrUpdate
        (
            const   TString&                strKey
            ,       tCIDLib::TCard4&        c4Version
            , const MStreamable&            strmblToWrite
            , const tCIDLib::TCard4         c4Reserve = 0
        );

        tCIDLib::TBoolean bAddOrUpdateDirect
        (
            const   TString&                strKey
            ,       tCIDLib::TCard4&        c4Version
            , const THeapBuf&               mbufToWrite
            , const tCIDLib::TCard4         c4Bytes
            , const tCIDLib::TCard4         c4Reserve = 0
        );

        tCIDLib::TBoolean bAllObjectsUnder
        (
            const   TString&                strStartScope
            ,       tCIDLib::TStrList&      colToFill
        )   const;

        tCIDLib::TBoolean bDeleteObjectIfExists
        (
            const   TString&                strKey
        );

        tCIDLib::TBoolean bFindNameUnder
        (
            const   TString&                strName
            , const TString&                strStartScope
            ,       tCIDLib::TStrList&      colToFill
        )   const;

        tCIDLib::TBoolean bInitialize
        (
            const   TString&                strPath
            , const TString&                strStoreName
        );

        tCIDLib::TBoolean bKeyExists
        (
            const   TString&                strKey
        )   const;

        tCIDLib::TBoolean bKeyExists
        (
            const   TString&                strKey
            ,       tCIDLib::TCard4&        c4Version
        )   const;

        tCIDLib::TBoolean bReadObject
        (
            const   TString&                strKey
            ,       tCIDLib::TCard4&        c4Version
            ,       MStreamable&            strmblToFill
        );

        tCIDLib::TVoid BackupStore();

        tCIDLib::TCard4 c4ObjectsInStore() const;

        tCIDLib::TCard4 c4QueryKeysInScope
        (
            const   TString&                strParScope
            ,       tCIDLib::TStrCollect&   colToFill
        );

        tCIDLib::TCard4 c4QueryObjectsInScope
        (
            const   TString&                strParScope
            ,       tCIDLib::TStrCollect&   colToFill
        );

        tCIDLib::TCard4 c4QuerySubScopes
        (
            const   TString&                strParScope
            ,       tCIDLib::TStrCollect&   colToFill
        );

        tCIDLib::TCard4 c4UpdateObject
        (
            const   TString&                strKey
            , const MStreamable&            strmblToWrite
        );

        tCIDLib::TCard4 c4UpdateObjectDirect
        (
            const   TString&                strKey
            , const THeapBuf&               mbufToWrite
            , const tCIDLib::TCard4         c4Bytes
        );

        tCIDLib::TVoid Close();

        tCIDLib::TVoid DebugDump
        (
                    TTextOutStream&         strmOut
        );

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
            , CIOP  tCIDLib::TCard4&        c4Version
            ,       MStreamable&            strmblToFill
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        tCIDLib::ELoadRes eReadObjectDirect
        (
            const   TString&                strKey
            , CIOP  tCIDLib::TCard4&        c4Version
            ,       THeapBuf&               mbufToFill
            , COP   tCIDLib::TCard4&        c4BytesRead
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        tCIDLib::TVoid FlushToDisk();

        TMutex& mtxSync();

        tCIDLib::TVoid QueryAllKeys
        (
                    tCIDLib::TStrCollect&   colToFill
        );

        TTime tmLastBackup() const;

        tCIDLib::TVoid ValidateStore();

        tCIDLib::TVoid ValidateStore
        (
                    TTextOutStream&         strmTar
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowNotReady
        (
            const   tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid ValidatePath
        (
            const   TString&                strPath
            , const tCIDLib::TBoolean       bTerminal
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TBoolean       bAllowTrail = kCIDLib::False
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bReady
        //      Keeps track of the Initialize/Close calls, so that we can catch an attempt
        //      to use the store when its not ready.
        //
        //  m_eFlags
        //      The flags provided in the ctor that allows the caller to set options on
        //      how this store instance works.
        //
        //  m_mbufRead
        //  m_mbufRealKey
        //      The memory buffer that we use to read the contents of objects into when they
        //      are being read. Then we create a temp input stream over it, and stream the
        //      object in. We also have one for the key data, which the internal calls
        //      always return, though we really don't given this flattened form back.
        //
        //  m_mtxSync
        //      This is a mutex that is used to syncrhonize access to this store by multiple
        //      threads. In most cases, those threads are server side proxies for remote
        //      process, via the ORB.
        //
        //  m_postCache
        //      A pointer to the object store cache object. It is a pointer because we don't
        //      export them or expose them.
        //
        //  m_strmKey
        //  m_strmOut
        //      Memory stream that we use to flatten objects when they are being stored, and
        //      one for flattening the keys also, when that is required (i.e. for the calls
        //      that write an object to the store, which requires storing the flattened key
        //      as well.)
        //
        //      Since all operations are synchronized, this is not a problem to have a
        //      per-store set of streams to use.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bReady;
        tCIDObjStore::EFlags    m_eFlags;
        THeapBuf                m_mbufRead;
        THeapBuf                m_mbufReadKey;
        TMutex                  m_mtxSync;
        TCIDObjStoreImpl*       m_postCache;
        TBinMBufOutStream       m_strmKey;
        TBinMBufOutStream       m_strmOut;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDObjStore,TObject)
};

#pragma CIDLIB_POPPACK


