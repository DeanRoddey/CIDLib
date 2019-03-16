//
// FILE NAME: CIDLogSrv_Impl.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/28/2001
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
//  This is the header for the CIDLogSrv_Impl.cpp file, which provides an
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
class TCIDLogServerImpl : public TCIDLogSrvServerBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDLogServerImpl();

        TCIDLogServerImpl(const TCIDLogServerImpl&) = delete;

        ~TCIDLogServerImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDLogServerImpl& operator=(const TCIDLogServerImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bGetLiveEvents
        (
                    tCIDLib::TCard4&        c4NextEventId
            ,       TVector<TLogEvent>&     colToFill
        )   override;

        tCIDLib::TCard4 c4QueryEvents
        (
                    TVector<TLogEvent>&     colToFill
            , const tCIDLib::TCard4         c4MaxToReturn
        )   override;

        tCIDLib::TCard4 c4QueryEvents
        (
                    TVector<TLogEvent>&     colToFill
            , const tCIDLib::TCard4         c4Minutes
            , const tCIDLib::TCard4         c4MaxToReturn
        )   override;

        tCIDLib::TCard4 c4QueryEvents
        (
                    TVector<TLogEvent>&     colToFill
            , const tCIDLib::TCard4         c4MaxToReturn
            , const TString&                strHostExpr
            , const TString&                strProcExpr
            , const TString&                strFacExrp
            , const TString&                strThreadExpr
            , const tCIDLib::TCard8         c8SevLevels
            , const tCIDLib::TCard8         c8ErrClasses
        )   override;

        tCIDLib::TVoid DebugDump
        (
                    tCIDLib::TCard4&        c4BufSz
            ,       THeapBuf&               mbufData
        )   override;

        tCIDLib::TVoid LogSingle
        (
            const   TLogEvent&              logevToSend
        )   override;

        tCIDLib::TVoid LogMultiple
        (
            const   TBag<TLogEvent>&        colToSend
        )   override;

        tCIDLib::TVoid RemoveAll() override;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Initialize() override;

        tCIDLib::TVoid Terminate() override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLogSrv::TKeyItem** apitemMakeSortedKeyList
        (
            const   tCIDLogSrv::EKeyListKeys eKey
        );

        tCIDLib::TVoid AddToLiveQueue
        (
            const   TLogEvent&              logevToAdd
        );

        tCIDLib::TCard4 c4CompactFreeList
        (
            const   tCIDLib::TCard4         c4Needed
        );

        tCIDLib::TCard4 c4Expand();

        tCIDLib::TCard4 c4TossOldest
        (
            const   tCIDLib::TCard4         c4ToToss
        );

        tCIDLib::TVoid CompactFile();

        tCIDLib::TVoid DumpDebugInfo
        (
                    TTextOutStream&         strmOut
        );

        tCIDLib::EExitCodes eFlusherThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::TVoid FreeKeyItem
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid InitializeLogFile
        (
            const   TString&                strFileName
        );

        tCIDLib::TVoid PanicReset();

        tCIDLib::TVoid ReadHeader();

        tCIDLib::TVoid ReadOne
        (
                    TBinInStream&           strmSrc
            ,       TLogEvent&              logevToFill
        );

        tCIDLib::TVoid WriteHeader
        (
                    TBinaryFile&            flTarget
            , const tCIDLib::TCard4         c4Seq
            , const tCIDLib::TCard4         c4KeysUsed
            , const tCIDLib::TCard4         c4FreesUsed
            , const tCIDLogSrv::TKeyItem*   paKeyList
            , const tCIDLogSrv::TFreeItem*  paFreeList
        );

        tCIDLib::TVoid WriteOne
        (
            const   TLogEvent&              logevToWrite
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_aFreeList
        //      This is the in memory version of the free list array, which
        //      is loaded from the file on startup and periodically flushed
        //      back out. This one maintained compacted, but not sorted. It
        //      will be sorted periodically as needed, but during normal use
        //      new items are not added in any sorted way. They are just stuck
        //      onto the end.
        //
        //  m_aKeyList
        //      This is the in memory version of the key array, which is
        //      loaded from the file on startup and periodically flushed back
        //      out. This one is always compacted, and m_c4KeysUsed indicates
        //      where the next entry would go (assuming the new entry has a
        //      later time stamp, otherwise we have to make room and push the
        //      newer entries up.)
        //
        //  m_c4FreesUsed
        //  m_c4KeysUsed
        //      The number of entries in the key and free arrays that are in
        //      use. These lists are maintained compacted, so this is also
        //      the index at which new entries would be appended.
        //
        //  m_c4Seq
        //      The sequence number that we write to each new event that we
        //      log. Its saved in the log file header, so that we can restart
        //      it correctly when we restart.
        //
        //  m_colLiveData
        //      We retain a linked list of the most recently logged events,
        //      so that live viewer apps can come get it. We will keep up to
        //      15 seconds worth of events. When we add a new one, we check
        //      the last one and if its over 15 seconds old, we push it out.
        //
        //  m_enctLastLogged
        //      This is updated each time we log, with the time stamp of the
        //      last event logged. This lets us drastically optimize when
        //      we get a query in the form 'any events after X'. If X is at
        //      or beyond this time, then we don't have to do anything.
        //      Otherwise, we'd have to do a time sorted key just to find
        //      this out.
        //
        //  m_enctLastPrune
        //      The last time we check for old items we can prune. We don't
        //      both to do this more than once every couple hours, and only
        //      then if we are hurting for key space.
        //
        //  m_flLog
        //      The log file we keep opened so that we can read/write it.
        //
        //  m_mtxSync
        //      The mutex we use to synchronize access to the log file. We
        //      will have multiple remote clients calling into us at once,
        //      each of which has its own thread, so we have to synchronize
        //      our output.
        //
        //  m_strmBuf
        //      We format objects into a temp memory buffer first, then we
        //      dump it to the file all at once, with control info stuck
        //      around it.
        //
        //  m_thrFlusher
        //      This thread periodically wakes up and flushes the header info
        //      out to file if anything new has been written since it last
        //      flushed, in case of emergency. We will always flush it
        //      upon exit, but this way we worst case might lose a handful of
        //      entries.
        // -------------------------------------------------------------------
        tCIDLogSrv::TFreeItem   m_aFreeList[kCIDLogSrv::c4MaxFrees];
        tCIDLogSrv::TKeyItem    m_aKeyList[kCIDLogSrv::c4MaxKeys];
        tCIDLib::TCard4         m_c4FreesUsed;
        tCIDLib::TCard4         m_c4KeysUsed;
        tCIDLib::TCard4         m_c4LastFlushSeq;
        tCIDLib::TCard4         m_c4Seq;
        TSLinkedList            m_colLiveData;
        tCIDLib::TEncodedTime   m_enctLastLogged;
        TBinaryFile             m_flLog;
        TMutex                  m_mtxSync;
        TBinMBufOutStream       m_strmBuf;
        TThread                 m_thrFlusher;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDLogServerImpl,TCIDLogSrvServerBase)
};

