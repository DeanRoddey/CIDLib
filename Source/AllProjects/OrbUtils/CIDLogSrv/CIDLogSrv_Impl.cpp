//
// FILE NAME: CIDLogSrv_Impl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/28/2001
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
//  This method implements the TCIDLogSrvImpl class, which is the
//  implementation of the log server IDL generated interface.
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
#include    "CIDLogSrv.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDLogServerImpl,TCIDLogSrvServerBase)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDLogSrv_Impl
{
    // -----------------------------------------------------------------------
    //  A magic marker value we use in the log file
    // -----------------------------------------------------------------------
    constexpr tCIDLib::TCard4       c4DeadBeef    = 0xDEADBEEF;


    // -----------------------------------------------------------------------
    //  Used to number sequential log events in the live data queue
    // -----------------------------------------------------------------------
    tCIDLib::TCard4             c4CurIndex = 0;


    // -----------------------------------------------------------------------
    //  The file flags to use on the log file
    // -----------------------------------------------------------------------
    const tCIDLib::EFileFlags   eLogFileFlags = tCIDLib::EFileFlags
    (
        tCIDLib::EFileFlags::WriteThrough
        | tCIDLib::EFileFlags::RandomAccess
    );
}



// ---------------------------------------------------------------------------
//   CLASS: TLogSrvQNode
//  PREFIX: node
//
//  We use a singly linked list to hold our live data queue. It is not a
//  templatized collection, but it does work in terms of a node derived from
//  its own based node. So we we create our own little node to hold the ptr
//  to the log event object.
// ---------------------------------------------------------------------------
class TLogSrvQNode : public TSLstNode
{
    public :
        TLogSrvQNode(       TLogEvent* const    plogevToAdopt
                    , const tCIDLib::TCard4     c4Index) :

            m_c4Index(c4Index)
            , m_plogevData(plogevToAdopt)
        {
        }

        TLogSrvQNode(const TLogSrvQNode&) = delete;
        TLogSrvQNode(TLogSrvQNode&&) = delete;

        ~TLogSrvQNode()
        {
            delete m_plogevData;
        }

        TLogSrvQNode& operator=(const TLogSrvQNode&) = delete;
        TLogSrvQNode& operator=(TLogSrvQNode&&) = delete;

        tCIDLib::TCard4 m_c4Index;
        TLogEvent*      m_plogevData;
};


// ---------------------------------------------------------------------------
//   CLASS: TCIDLogServerImpl
//  PREFIX: orbs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDLogServerImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDLogServerImpl::TCIDLogServerImpl() :

    m_aFreeList()
    , m_aKeyList()
    , m_c4FreesUsed(0)
    , m_c4LastFlushSeq(1)
    , m_c4KeysUsed(0)
    , m_c4Seq(1)
    , m_enctLastLogged(0)
    , m_strmBuf(8192)
    , m_thrFlusher
      (
        L"CIDLogSrvFlusherThread"
        , TMemberFunc<TCIDLogServerImpl>(this, &TCIDLogServerImpl::eFlusherThread)
      )
{
}

TCIDLogServerImpl::~TCIDLogServerImpl()
{
}


// ---------------------------------------------------------------------------
//  TCIDLogServerImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDLogServerImpl::bGetLiveEvents(  tCIDLib::TCard4&        c4NextEventId
                                    , TVector<TLogEvent>&   colToFill)
{
    // Lock the mutex first
    TLocker lockrSync(&m_mtxSync);

    //
    //  If the passed message id is the same as our current id, then we
    //  can just return false now.
    //
    if (c4NextEventId == CIDLogSrv_Impl::c4CurIndex)
        return kCIDLib::False;

    //
    //  Get the head. If it's zero, we have no data to give. Else, get the
    //  tail, so we can do our checks.
    //
    TLogSrvQNode* pnodeHead = static_cast<TLogSrvQNode*>
    (
        m_colLiveData.pnodeHead()
    );

    // If we have no data, then nothing to do
    if (!pnodeHead)
        return kCIDLib::False;

    TLogSrvQNode* pnodeTail = static_cast<TLogSrvQNode*>
    (
        m_colLiveData.pnodeTail()
    );

    //
    //  If his next event id is before our first one, then he is behind and
    //  we just need to give him what we've got. If his id isn't zero, then
    //  he has missed some messages so we'll fake an error into the list to
    //  that effect.
    //
    //  If his next even id is past our last one, then he has gotten out
    //  of sync, probably because we restarted. So we fake an error into
    //  the list and get him back into sync.
    //
    //  Else, search the list for where we need to start feeding items
    //  out, and give him those items.
    //
    TLogSrvQNode* pnodeCur = nullptr;
    if (c4NextEventId < pnodeHead->m_c4Index)
    {
        if (c4NextEventId)
        {
            colToFill.objPlace
            (
                facCIDLogSrv.strName()
                , CID_FILE
                , CID_LINE
                , facCIDLogSrv.strMsg(kLogSErrs::errcLive_EventsMissed)
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::OutOfSync
            );
        }

        // Just start at the head and give him what we have
        pnodeCur = pnodeHead;
    }
     else if (c4NextEventId > pnodeTail->m_c4Index)
    {
        // Fake an event into the list to tell him what happened
        colToFill.objPlace
        (
            facCIDLogSrv.strName()
            , CID_FILE
            , CID_LINE
            , facCIDLogSrv.strMsg(kLogSErrs::errcLive_OutOfSync)
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutOfSync
        );
        // Don't give him anything else back, but we'll resync him now
    }
     else
    {
        pnodeCur = pnodeHead;
        while (pnodeCur)
        {
            if (pnodeCur->m_c4Index == c4NextEventId)
                break;
            pnodeCur = static_cast<TLogSrvQNode*>(pnodeCur->pnodeNext());
        }

        //
        //  If we didn't find it, then reset his next event id, and return
        //  true without any items, to make him resync.
        //
        if (!pnodeCur)
        {
            colToFill.objPlace
            (
                facCIDLogSrv.strName()
                , CID_FILE
                , CID_LINE
                , facCIDLogSrv.strMsg
                (
                    kLogSErrs::errcLive_BadId, TCardinal(c4NextEventId)
                )
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
            );
        }
    }

    //
    //  If we have some valid data for him, give him the rest of the events
    //  from the current node forward.
    //
    while (pnodeCur)
    {
        colToFill.objAdd(*pnodeCur->m_plogevData);
        pnodeCur = static_cast<TLogSrvQNode*>(pnodeCur->pnodeNext());
    }

    // Update the next message id that the caller will want
    c4NextEventId = CIDLogSrv_Impl::c4CurIndex;

    // Return true if we got any events
    return !colToFill.bIsEmpty();
}


tCIDLib::TCard4
TCIDLogServerImpl::c4QueryEvents(       TVector<TLogEvent>& colToFill
                                , const tCIDLib::TCard4     c4MaxToReturn)
{
    // Lock the mutex first
    TLocker lockrSync(&m_mtxSync);

    // If we have no keys, then we are done now
    if (!m_c4KeysUsed)
        return 0;

    //
    //  Figure out the real max. If its larger than the number of events
    //  available, then clip back to that. If its zero or bigger than the
    //  hard max, then set it to the hard max, else take the value given.
    //
    tCIDLib::TCard4 c4RealMax = c4MaxToReturn;
    if (c4RealMax > m_c4KeysUsed)
        c4RealMax = m_c4KeysUsed;
    else if (!c4RealMax || (c4RealMax > kCIDLogSrv::c4MaxRetCount))
        c4RealMax = kCIDLogSrv::c4MaxRetCount;

    //
    //  Get a sorted key based on time. We'll work back until we get the
    //  number requested or the max, then start filling in the caller's
    //  collection from there.
    //
    tCIDLogSrv::TKeyItem** apitemSort = apitemMakeSortedKeyList
    (
        tCIDLogSrv::EKeyListKeys::LoggedTime
    );
    TArrayJanitor<tCIDLogSrv::TKeyItem*> janSort(apitemSort);

    //
    //  Create a pair of linked input and output memory buffer streams. We'll
    //  need these to selectively pull in the streamed out chunks and stream
    //  back in log event objects from them. Set the intial buffer size to
    //  1K, but let it grow pretty large. It almost never will but it costs
    //  us nothing to be prepared for that, in the worst case.
    //
    //  We also need a separate buffer to read into, which we then have to
    //  write to the output stream, which causes it to show up in the input
    //  stream, which we can then stream the object out of.
    //
    THeapBuf            mbufIn(1024, 128 * 1024);
    TBinMBufInStream    strmIn(&mbufIn);
    TLogEvent           logevTmp;
    tCIDLib::TCard4     c4RetCount = 0;
    for (tCIDLib::TCard4 c4Index = m_c4KeysUsed - c4RealMax;
                                            c4Index < m_c4KeysUsed; c4Index++)
    {
        tCIDLogSrv::TKeyItem& itemCur = *apitemSort[c4Index];

        // Reset the stream for the next round
        strmIn.Reset();

        // Seek to this item and read it into the stream's buffer
        m_flLog.SetFilePos(itemCur.c4Ofs + kCIDLogSrv::c4StoreOfs);
        if (m_flLog.c4ReadBuffer(mbufIn, itemCur.c4Size) != itemCur.c4Size)
        {
            facCIDLogSrv.ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kLogSErrs::errcFile_NotAllRead
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotAllRead
            );
        }

        // Tell the stream where it's end index is
        strmIn.SetEndIndex(itemCur.c4Size);

        // Now stream in our temp object from the input stream, and add it
        try
        {
            ReadOne(strmIn, logevTmp);
        }

        catch(const TError& errToCatch)
        {
            logevTmp = TLogEvent
            (
                facCIDLogSrv.strName()
                , CID_FILE
                , CID_LINE
                , L"!!!!Error in log entry, couldn't parsed"
                , errToCatch.strErrText()
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::Format
            );
        }

        catch(...)
        {
            logevTmp = TLogEvent
            (
                facCIDLogSrv.strName()
                , CID_FILE
                , CID_LINE
                , L"!!!!Error in log entry, couldn't parsed"
                , tCIDLib::ESeverities::Status
                , tCIDLib::EErrClasses::Format
            );
        }

        colToFill.objAdd(logevTmp);

        // Bump the count, and break out if we hit the max
        c4RetCount++;
        if (c4RetCount >= c4RealMax)
            break;
    }
    return c4RealMax;
}

tCIDLib::TCard4
TCIDLogServerImpl::c4QueryEvents(       TVector<TLogEvent>& colToFill
                                , const tCIDLib::TCard4     c4Minutes
                                , const tCIDLib::TCard4     c4MaxToReturn)
{
    // Lock the mutex first
    TLocker lockrSync(&m_mtxSync);

    // If we have no keys, then we are done now
    if (!m_c4KeysUsed)
        return 0;

    //
    //  Figure out the real max. If its larger than the number of events
    //  available, then clip back to that. If its zero or bigger than the
    //  hard max, then set it to the hard max, else take the value given.
    //
    tCIDLib::TCard4 c4RealMax = c4MaxToReturn;
    if (c4RealMax > m_c4KeysUsed)
        c4RealMax = m_c4KeysUsed;
    else if (!c4RealMax || (c4RealMax > kCIDLogSrv::c4MaxRetCount))
        c4RealMax = kCIDLogSrv::c4MaxRetCount;

    // Get a sorted key based on time and put a janitor on it
    tCIDLogSrv::TKeyItem** apitemSort = apitemMakeSortedKeyList
    (
        tCIDLogSrv::EKeyListKeys::LoggedTime
    );
    TArrayJanitor<tCIDLogSrv::TKeyItem*> janSort(apitemSort);

    // Calculate the cutoff point, that many minutes back from now.
    const tCIDLib::TEncodedTime enctCutoff =
    (
        TTime::enctNow()
        - (c4Minutes * kCIDLib::enctOneMinute)
    );

    //
    //  Now, start working backwards from the end. We go back until we hit
    //  the start of the list, we hit one that's past the cutoff, or we hit
    //  the max number we need to return.
    //
    tCIDLib::TCard4 c4Index = m_c4KeysUsed - 1;
    tCIDLib::TCard4 c4RetCount = 0;
    while ((c4Index > 0)
    &&     (apitemSort[c4Index]->enctLogged > enctCutoff)
    &&     (c4RetCount <= c4RealMax))
    {
        c4Index--;
        c4RetCount++;
    }

    if (c4RetCount)
    {
        THeapBuf            mbufIn(1024, 128 * 1024);
        TBinMBufInStream    strmIn(&mbufIn);
        TLogEvent           logevTmp;

        // Ok, add all the ones we found to the collection
        for (; c4Index < m_c4KeysUsed; c4Index++)
        {
            tCIDLogSrv::TKeyItem& itemCur = *apitemSort[c4Index];

            // Reset the stream for the next round
            strmIn.Reset();

            // Seek to this item and read it into a memory buffer
            m_flLog.SetFilePos(itemCur.c4Ofs + kCIDLogSrv::c4StoreOfs);
            if (m_flLog.c4ReadBuffer(mbufIn, itemCur.c4Size) != itemCur.c4Size)
            {
                facCIDLogSrv.ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kLogSErrs::errcFile_NotAllRead
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotAllRead
                );
            }

            // Tell the stream where it's end index is
            strmIn.SetEndIndex(itemCur.c4Size);

            // Now stream in our temp object from the input stream, and add it
            try
            {
                ReadOne(strmIn, logevTmp);
            }

            catch(const TError& errToCatch)
            {
                logevTmp = TLogEvent
                (
                    facCIDLogSrv.strName()
                    , CID_FILE
                    , CID_LINE
                    , L"!!!!Error in log entry, couldn't parsed"
                    , errToCatch.strErrText()
                    , tCIDLib::ESeverities::Status
                    , tCIDLib::EErrClasses::Format
                );
            }

            catch(...)
            {
                logevTmp = TLogEvent
                (
                    facCIDLogSrv.strName()
                    , CID_FILE
                    , CID_LINE
                    , L"!!!!Error in log entry, couldn't parsed"
                    , tCIDLib::ESeverities::Status
                    , tCIDLib::EErrClasses::Format
                );
            }
            colToFill.objAdd(logevTmp);
        }
    }
    return c4RetCount;
}


//
//  Note that this guy will return a list that is in reverse time order,
//  since that's the only practical way to do it. The caller must be aware
//  of this.
//
tCIDLib::TCard4
TCIDLogServerImpl::c4QueryEvents(       TVector<TLogEvent>& colToFill
                                , const tCIDLib::TCard4     c4MaxToReturn
                                , const TString&            strHostExpr
                                , const TString&            strProcExpr
                                , const TString&            strFacExpr
                                , const TString&            strThreadExpr
                                , const tCIDLib::TCard8     c8SevBits
                                , const tCIDLib::TCard8     c8ClassBits)
{
    //
    //  If we have no keys, then we are done now. We will make this check
    //  unlocked, to avoid the overhead of the regular expression stuff
    //  and locking, jut to find out that we have nothing to do.
    //
    if (!m_c4KeysUsed)
        return 0;

    //
    //  Optimize by setting flags that indicate whether each one was "*" or
    //  not. For any of them that are, we don't have to test that field
    //  since we know it will match anything, and we don't have to create
    //  the regular expression for that one.
    //
    const tCIDLib::TBoolean bHostAll = (strHostExpr == L"*");
    const tCIDLib::TBoolean bFacAll = (strFacExpr == L"*");
    const tCIDLib::TBoolean bProcAll = (strProcExpr == L"*");
    const tCIDLib::TBoolean bThreadAll = (strThreadExpr == L"*");

    // And now create those expressions that we need to
    TRegEx* pregxHostExpr = nullptr;
    if (!bHostAll)
        pregxHostExpr = new TRegEx(strHostExpr);
    TJanitor<TRegEx> janHost(pregxHostExpr);

    TRegEx* pregxFacExpr = nullptr;
    if (!bFacAll)
        pregxFacExpr = new TRegEx(strFacExpr);
    TJanitor<TRegEx> janFac(pregxFacExpr);

    TRegEx* pregxProcExpr = nullptr;
    if (!bProcAll)
        pregxProcExpr = new TRegEx(strProcExpr);
    TJanitor<TRegEx> janProc(pregxProcExpr);

    TRegEx* pregxThreadExpr = nullptr;
    if (!bThreadAll)
        pregxThreadExpr = new TRegEx(strThreadExpr);
    TJanitor<TRegEx> janThread(pregxThreadExpr);

    // Lock the mutex now before we go further
    TLocker lockrSync(&m_mtxSync);

    //
    //  Figure out the real max. If its larger than the number of events
    //  available, then clip back to that. If its zero or bigger than the
    //  hard max, then set it to the hard max, else take the value given.
    //
    tCIDLib::TCard4 c4RealMax = c4MaxToReturn;
    if (c4RealMax > m_c4KeysUsed)
        c4RealMax = m_c4KeysUsed;
    else if (!c4RealMax || (c4RealMax > kCIDLogSrv::c4MaxRetCount))
        c4RealMax = kCIDLogSrv::c4MaxRetCount;

    // Get a sorted key based on time and put a janitor on it
    tCIDLogSrv::TKeyItem** apitemSort = apitemMakeSortedKeyList
    (
        tCIDLogSrv::EKeyListKeys::LoggedTime
    );
    TArrayJanitor<tCIDLogSrv::TKeyItem*> janSort(apitemSort);

    //
    //  Create the streams we'll need to pull the objects in
    THeapBuf            mbufIn(1024, 128 * 1024);
    TBinMBufInStream    strmIn(&mbufIn);
    TLogEvent           logevCur;

    //
    //  And now work backwards through the list, until we run out of items
    //  or get as many matches as we can handle.
    //
    tCIDLib::TCard4 c4Index = m_c4KeysUsed - 1;
    tCIDLib::TCard4 c4RetCount = 0;
    while (c4RetCount <= c4RealMax)
    {
        // Get the current element from the key index
        tCIDLogSrv::TKeyItem& itemCur = *apitemSort[c4Index];

        //
        //  We can do the severity and class without streaming in the object,
        //  which can greatly speed things up if they aren't doing all
        //  severity levels or all classes, and they often aren't. These
        //  are in the key item, converted to TCard1 format.
        //
        //  If the indicated bit isn't on in the key field, then we can
        //  just reject this one and move on.
        //
        tCIDLib::TCard8 c8Sev = 1;
        c8Sev <<= itemCur.c1Sev;
        tCIDLib::TCard8 c8Class = 1;
        c8Class <<= itemCur.c1Class;

        if ((c8SevBits & c8Sev) && (c8ClassBits & c8Class))
        {
            // Ok, have to stream this object in
            strmIn.Reset();
            m_flLog.SetFilePos(itemCur.c4Ofs + kCIDLogSrv::c4StoreOfs);
            if (m_flLog.c4ReadBuffer(mbufIn, itemCur.c4Size) != itemCur.c4Size)
            {
                facCIDLogSrv.ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kLogSErrs::errcFile_NotAllRead
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotAllRead
                );
            }

            // Tell the stream where it's end index is
            strmIn.SetEndIndex(itemCur.c4Size);

            // Now stream in our temp object from the input stream, and add it
            try
            {
                ReadOne(strmIn, logevCur);
            }

            catch(const TError& errToCatch)
            {
                logevCur = TLogEvent
                (
                    facCIDLogSrv.strName()
                    , CID_FILE
                    , CID_LINE
                    , L"!!!!Error in log entry, couldn't parsed"
                    , errToCatch.strErrText()
                    , tCIDLib::ESeverities::Status
                    , tCIDLib::EErrClasses::Format
                );
            }

            catch(...)
            {
                logevCur = TLogEvent
                (
                    facCIDLogSrv.strName()
                    , CID_FILE
                    , CID_LINE
                    , L"!!!!Error in log entry, couldn't parsed"
                    , tCIDLib::ESeverities::Status
                    , tCIDLib::EErrClasses::Format
                );
            }

            //
            //  For any of the regular expression fields that are not just
            //  "*", test this object against them and reject them if no
            //  match.
            //
            tCIDLib::TBoolean bMatch = kCIDLib::True;

            // If host expression isn't *, then do it
            if (!bHostAll)
                bMatch =  pregxHostExpr->bFullyMatches(logevCur.strHostName());

            // If it matched that, then try the proc name
            if (bMatch)
            {
                if (!bProcAll)
                    bMatch =  pregxProcExpr->bFullyMatches(logevCur.strProcess());
            }

            // If that matched, try the facility name
            if (bMatch)
            {
                // If it's not *, then do it
                if (!bFacAll)
                    bMatch = pregxFacExpr->bFullyMatches(logevCur.strFacName());
            }

            // If those matched, do the thread name
            if (bMatch)
            {
                // if it's not *, then do it
                if (!bThreadAll)
                    bMatch = pregxThreadExpr->bFullyMatches(logevCur.strThread());
            }

            // If all matched, then add this one and bump the counter
            if (bMatch)
            {
                c4RetCount++;
                colToFill.objAdd(logevCur);
            }
        }

        // If we are on the 0th item, we are done, else move back
        if (!c4Index)
            break;
        c4Index--;
    }

    return c4RetCount;
}


tCIDLib::TVoid
TCIDLogServerImpl::DebugDump(tCIDLib::TCard4& c4BufSz, THeapBuf& mbufData)
{
    //
    //  Create an output stream over the caller's buffer. Use UTF-8 to
    //  minimize data size to stream back.
    //
    TTextMBufOutStream strmOut
    (
        &mbufData, tCIDLib::EAdoptOpts::NoAdopt, new TUTF8Converter
    );

    // Run through all of the items and make sure we can read them in
    {
        // Lock the mutex during this
        TLocker lockrSync(&m_mtxSync);

        // This is just for debugging purposes, so leave it out otherwise
        // DumpDebugInfo(strmOut);

        // Get a sorted list to use for the dump
        tCIDLogSrv::TKeyItem** apitemSort = apitemMakeSortedKeyList
        (
            tCIDLogSrv::EKeyListKeys::LoggedTime
        );
        TArrayJanitor<tCIDLogSrv::TKeyItem*> janSort(apitemSort);

        strmOut << L"\n------------------------" << kCIDLib::EndLn;

        THeapBuf            mbufIn(1024, 128 * 1024);
        TBinMBufInStream    strmIn(&mbufIn);
        TLogEvent           logevTmp;

        TTime               tmLogged;
        tmLogged.strDefaultFormat(L"%(M,2,0)/%(D,2,0) %(H,2,0):%(u,2,0):%(s,2,0)");
        for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4KeysUsed; c4Index++)
        {
            tCIDLogSrv::TKeyItem& itemCur = *apitemSort[c4Index];

            // Reset the stream for the next round
            strmIn.Reset();

            // Seek to this item and read it into the stream's buffer
            strmOut << L"Seq#" << itemCur.c4Seq << L" - ";

            try
            {
                m_flLog.SetFilePos(itemCur.c4Ofs + kCIDLogSrv::c4StoreOfs);
                if (m_flLog.c4ReadBuffer(mbufIn, itemCur.c4Size) != itemCur.c4Size)
                {
                    strmOut << L"Could not read item bytes\n";
                }
                 else
                {
                    try
                    {
                        // Tell the stream where it's end index is
                        strmIn.SetEndIndex(itemCur.c4Size);
                        ReadOne(strmIn, logevTmp);
                        tmLogged = logevTmp.enctLogged();

                        // Put out the header line
                        strmOut << tmLogged << L"-"
                                << logevTmp.strHostName() << kCIDLib::chComma
                                << logevTmp.strProcess() << kCIDLib::chComma
                                << logevTmp.strThread();

                        // Now the other info goes inside braces, indented
                        strmOut << L"\n{\n    "
                                << logevTmp.strFacName() << kCIDLib::chComma
                                << logevTmp.strFileName()
                                << kCIDLib::chPeriod << logevTmp.c4LineNum()
                                << kCIDLib::chComma << logevTmp.eSeverity()
                                << L"/" << logevTmp.eClass();

                        // Only do the error stuff if there are any non-zero codes
                        if (logevTmp.bAnyError())
                        {
                            strmOut << kCIDLib::chComma
                                    << facCIDLib().strMsg(kCIDMsgs::midGen_Error)
                                    << L":" << logevTmp.errcId()
                                    << L"/" << logevTmp.errcKrnlId()
                                    << L"/" << logevTmp.errcHostId();
                        }

                        // Now append the error text itself, indenting it over
                        strmOut << L"\n    " << logevTmp.strErrText();

                        // If there is aux text, log it next and indent it also.
                        if (logevTmp.bHasAuxText())
                            strmOut << L"\n    " << logevTmp.strAuxText();

                        // Add the closing brace, add new line, and flush the stream
                        strmOut << L"\n}\n" << kCIDLib::FlushIt;

                    }

                    catch(const TError& errToCatch)
                    {
                        strmOut << L"StreamErr: " << errToCatch.strErrText()
                                << kCIDLib::NewLn;
                    }
                }
            }

            catch(const TError& errToCatch)
            {
                strmOut << L"ReadErr: " << errToCatch.strErrText() << kCIDLib::NewLn;
            }
        }
    }

    // Flush the stream and indicate the return size
    strmOut.Flush();
    c4BufSz = strmOut.c4CurSize();
}


tCIDLib::TVoid TCIDLogServerImpl::LogSingle(const TLogEvent& logevToSend)
{
    // Lock the mutex first
    TLocker lockrSync(&m_mtxSync);
    WriteOne(logevToSend);
}


tCIDLib::TVoid TCIDLogServerImpl::LogMultiple(const TBag<TLogEvent>& colToSend)
{
    // Lock the mutex first
    TLocker lockrSync(&m_mtxSync);
    TBag<TLogEvent>::TCursor cursFmt(&colToSend);
    if (cursFmt.bReset())
    {
        do
        {
            WriteOne(cursFmt.objRCur());
        }   while (cursFmt.bNext());
    }
}


tCIDLib::TVoid TCIDLogServerImpl::RemoveAll()
{
    // Lock the mutex first
    TLocker lockrSync(&m_mtxSync);

    // Get the current file size
    const tCIDLib::TCard4 c4CurSize = tCIDLib::TCard4(m_flLog.c8CurSize());

    // Zero out our key and free list arrays.
    TRawMem::SetMemBuf(m_aFreeList, kCIDLib::c1MinCard, kCIDLogSrv::c4FreeListSize);
    TRawMem::SetMemBuf(m_aKeyList, kCIDLib::c1MinCard, kCIDLogSrv::c4KeyListSize);

    //
    //  Then set up a single entry in the free list which holds all of
    //  the currently available free list data. Its the size of the file
    //  minus the header stuff
    //
    m_aFreeList[0].c4Ofs = 0;
    m_aFreeList[0].c4Size = c4CurSize - kCIDLogSrv::c4StoreOfs;

    // Initialize our members that go into the header to defaults
    m_c4Seq = 1;
    m_c4LastFlushSeq = 1;
    m_c4KeysUsed = 0;
    m_c4FreesUsed = 1;

    // And now write out the header data
    WriteHeader(m_flLog, m_c4Seq, m_c4KeysUsed, m_c4FreesUsed, m_aKeyList, m_aFreeList);

    // Flush the live data queue
    m_colLiveData.RemoveAll();
}


// ---------------------------------------------------------------------------
//  TCIDLogServerImpl: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDLogServerImpl::Initialize()
{
    //
    //  Open our log file. If its not there, then this is our first run and
    //  we need to initialize it. So build up the path to the file so we can
    //  check for its existence.
    //
    TPathStr pathSearch(facCIDLogSrv.strLogPath());
    pathSearch.AddLevel(kCIDLogSrv::pszLogFileName);

    // If we don't find it, then initialize it
    if (!TFileSys::bExists(pathSearch))
        InitializeLogFile(pathSearch);

    // Now open it for exclusive read/write
    m_flLog.strName(pathSearch);
    m_flLog.Open
    (
        tCIDLib::EAccessModes::Excl_ReadWrite
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::AllOwnerAccess
        , CIDLogSrv_Impl::eLogFileFlags
    );

    //
    //  Read in the header information. This contains the keys and some
    //  other state info, into our members.
    //
    ReadHeader();

    #if CID_DEBUG_ON
    //    DebugDump(kCIDLib::True);
    #endif

    // And now start up the flusher thread
    m_thrFlusher.Start();
}


tCIDLib::TVoid TCIDLogServerImpl::Terminate()
{
    // Ask the flusher thread to stop
    m_thrFlusher.ReqShutdownSync();

    // Make sure the flusher is totally down
    try
    {
        m_thrFlusher.eWaitForDeath(10000);
    }

    catch(const TError&)
    {
        // <TBD> Not much we can do
    }

    // Flush out the header info, just in case
    WriteHeader(m_flLog, m_c4Seq, m_c4KeysUsed, m_c4FreesUsed, m_aKeyList, m_aFreeList);

    // And close it to make sure its all flushed out
    m_flLog.Close();
}


// ---------------------------------------------------------------------------
//  TCIDLogServerImpl: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLogSrv::TKeyItem**
TCIDLogServerImpl::apitemMakeSortedKeyList(const tCIDLogSrv::EKeyListKeys eKey)
{
    // Allocate an array large enough to hold the used keys
    tCIDLogSrv::TKeyItem** apitemSort = new tCIDLogSrv::TKeyItem*[m_c4KeysUsed];
    TArrayJanitor<tCIDLogSrv::TKeyItem*> janRet(apitemSort);

    // Point our sorted view at the original data
    for (tCIDLib::TCard4 c4SortInd = 0; c4SortInd < m_c4KeysUsed; c4SortInd++)
        apitemSort[c4SortInd] = &m_aKeyList[c4SortInd];

    // And now sort our view by the indicted field and return it
    TArrayOps::TSort<tCIDLogSrv::TKeyItem*>
    (
        apitemSort
        , m_c4KeysUsed
        , [eKey](const tCIDLogSrv::TKeyItem* pitem1, const tCIDLogSrv::TKeyItem* pitem2)
          {
            if (eKey == tCIDLogSrv::EKeyListKeys::LoggedTime)
            {
                if (pitem1->enctLogged < pitem2->enctLogged)
                    return tCIDLib::ESortComps::FirstLess;
                else if (pitem1->enctLogged > pitem2->enctLogged)
                    return tCIDLib::ESortComps::FirstGreater;

                // The time is the same, so use the sequence number
                if (pitem1->c4Seq < pitem2->c4Seq)
                    return tCIDLib::ESortComps::FirstLess;
                else if (pitem1->c4Seq > pitem2->c4Seq)
                    return tCIDLib::ESortComps::FirstGreater;
            }
             else if (eKey == tCIDLogSrv::EKeyListKeys::Severity)
            {
                if (pitem1->c1Sev < pitem2->c1Sev)
                    return tCIDLib::ESortComps::FirstLess;
                else if (pitem1->c1Sev > pitem2->c1Sev)
                    return tCIDLib::ESortComps::FirstGreater;
            }
             else if (eKey == tCIDLogSrv::EKeyListKeys::Offset)
            {
                if (pitem1->c4Ofs < pitem2->c4Ofs)
                    return tCIDLib::ESortComps::FirstLess;
                else if (pitem1->c4Ofs > pitem2->c4Ofs)
                    return tCIDLib::ESortComps::FirstGreater;
            }
            return tCIDLib::ESortComps::Equal;
          }
    );
    return janRet.paOrphan();
}


tCIDLib::TVoid
TCIDLogServerImpl::AddToLiveQueue(const TLogEvent& logevToAdd)
{
    static const tCIDLib::TEncodedTime enctCutoffRange
    (
        kCIDLib::enctOneSecond * 15
    );

    //
    //  We have a few scenarios:
    //
    //  1. Our live queue is less than 32 entries. If so, we just add this
    //  new guy as a new node without bothering to check the time.
    //
    //  2. We have some items, but the last one is less than 15 seconds
    //  old. So we just add a new node.
    //
    //  3. The last item is older than 15 seconds. So we move it to the
    //  end of the list and assign our new guy to this old object, so that
    //  we can reuse it.
    //
    if (m_colLiveData.c4ElemCount() < 32)
    {
        // Scenario #1, so add a new node
        m_colLiveData.AppendNode
        (
            new TLogSrvQNode
            (
                new TLogEvent(logevToAdd), CIDLogSrv_Impl::c4CurIndex++
            )
        );
    }
     else
    {
        // Calc the cutoff time
        const tCIDLib::TEncodedTime enctCutoff = TTime::enctNow() - enctCutoffRange;

        // Get the head node
        TLogSrvQNode* pnodeTmp = static_cast<TLogSrvQNode*>(m_colLiveData.pnodeHead());
        if (pnodeTmp->m_plogevData->enctLogged() < enctCutoff)
        {
            // Move the head node to the tail
            m_colLiveData.MoveToTail(pnodeTmp);

            //
            //  Now get the tail node and assign our new guy to it, and
            //  update its sequence id.
            //
            pnodeTmp = static_cast<TLogSrvQNode*>
            (
                m_colLiveData.pnodeTail()
            );
            *(pnodeTmp->m_plogevData) = logevToAdd;
            pnodeTmp->m_c4Index = CIDLogSrv_Impl::c4CurIndex++;
        }
         else
        {
            // We don't have 15 second's worth yet, so just add a node
            m_colLiveData.AppendNode
            (
                new TLogSrvQNode
                (
                    new TLogEvent(logevToAdd), CIDLogSrv_Impl::c4CurIndex++
                )
            );
        }
    }
}


//
//  This guy will try to compact the free list by combining adjacent free
//  list items, which happens quite a lot. It will try to get one big enough
//  to hold the passed byte count, but it will do the whole list.
//
tCIDLib::TCard4 TCIDLogServerImpl::c4CompactFreeList(const tCIDLib::TCard4 c4Needed)
{
    //
    //  If at this point we have less than 64 free entries, don't bother
    //  since it's unlikely to do much for us. We assumed the caller already
    //  searched the free list and didn't find an item big enough.
    //
    if (m_c4FreesUsed < 64)
        return m_c4FreesUsed;

    //
    //  Because of the destructive nature of some stuff below, we need to
    //  create a temp copy of the free list, so that we only write out the
    //  changes when we know we have everything in hand.
    //
    tCIDLogSrv::TFreeItem* pFreeList = new tCIDLogSrv::TFreeItem[kCIDLogSrv::c4MaxFrees];
    TArrayJanitor<tCIDLogSrv::TFreeItem> janFree(pFreeList);
    TRawMem::CopyMemBuf(pFreeList, m_aFreeList, kCIDLogSrv::c4FreeListSize);
    tCIDLib::TCard4 c4FreeCount = m_c4FreesUsed;

    //  Sort the free list by offset, so we can find adjacent ones easily.
    TArrayOps::TSort<tCIDLogSrv::TFreeItem>
    (
        pFreeList, c4FreeCount, eCompFreeItem
    );

    //
    //  Use the original frees count as te loop limit so that we can run
    //  c4FreeCount as we remove items.
    //
    tCIDLib::TCard4 c4FIndex = 0;
    while (c4FIndex < m_c4FreesUsed)
    {
        //
        //  Get the current free item. While subsequent ones are
        //  contiguous, add them to this guy and let them go.
        //
        tCIDLogSrv::TFreeItem& itemCur = pFreeList[c4FIndex++];
        tCIDLib::TCard4 c4NextOfs = itemCur.c4Ofs + itemCur.c4Size;
        while ((c4FIndex < m_c4FreesUsed)
        &&     (pFreeList[c4FIndex].c4Ofs == c4NextOfs))
        {
            c4NextOfs += pFreeList[c4FIndex].c4Size;
            itemCur.c4Size += pFreeList[c4FIndex].c4Size;
            pFreeList[c4FIndex].c4Ofs = kCIDLib::c4MaxCard;
            c4FIndex++;

            // We removed one so bump down the counter
            c4FreeCount--;
        }
    }

    //
    //  If we removed any, then sort again to push freed ones up.
    //
    //  NOTE: It's impossible to remove them all, so we don't have to check
    //  for that. We can get down to one. We have to sort no matter what,
    //  to push even that one down to the start of the list.
    //
    if (c4FreeCount != m_c4FreesUsed)
    {
        //
        //  Sort the original number of items, to push up the removed ones
        //  to the end of the original range. We don't want to sort the
        //  whole thing since all the trailing ones have an offset of zero.
        //
        TArrayOps::TSort<tCIDLogSrv::TFreeItem>(pFreeList, m_c4FreesUsed, eCompFreeItem);

        // And zero out the unused parts now and flush out the header
        TRawMem::SetMemBuf
        (
            &pFreeList[c4FreeCount]
            , kCIDLib::c1MinCard
            , (kCIDLogSrv::c4MaxFrees - c4FreeCount)
                * sizeof(tCIDLogSrv::TFreeItem)
        );

        WriteHeader
        (
            m_flLog, m_c4Seq, m_c4KeysUsed, c4FreeCount, m_aKeyList, pFreeList
        );

        // Ok, update our actual values with the stuff we just wrote
        m_c4FreesUsed = c4FreeCount;
        TRawMem::CopyMemBuf
        (
            m_aFreeList, pFreeList, kCIDLogSrv::c4FreeListSize
        );

        //
        //  If we created one that's big enough, don't bother with the full
        //  compaction.
        //
        c4FIndex = 0;
        while (c4FIndex < m_c4FreesUsed)
        {
            if (pFreeList[c4FIndex].c4Size >= c4Needed)
                return c4FIndex;
            c4FIndex++;
        }
    }


    // Couldn't find or create space
    return m_c4FreesUsed;
}


//
//  Tries to expand the file and returns the index of a new free list item
//  to use. If it cannot expand the file, it returns the current free list
//  item count to indicate this.
//
tCIDLib::TCard4 TCIDLogServerImpl::c4Expand()
{
    // Remember the current size
    m_flLog.Flush();
    const tCIDLib::TCard8 c8Org = m_flLog.c8CurSize();

    //
    //  Ok, lets seek to the end of the file and expand it out by writing
    //  zeroed data to it.
    //
    m_flLog.SetFilePos(c8Org);

    // We'll expand by a pretermined K at a time
    try
    {
        constexpr tCIDLib::TCard4 c4BufSz = 4096;
        tCIDLib::TCard1 ac1Dummy[c4BufSz];
        TRawMem::SetMemBuf(ac1Dummy, kCIDLib::c1MinCard, c4BufSz);
        for (tCIDLib::TCard4 c4Ks = 0; c4Ks < kCIDLogSrv::c4ExpandK; c4Ks += 4)
            m_flLog.c4WriteBuffer(ac1Dummy, c4BufSz);
    }

    catch(...)
    {
        // Just truncate back to original and return failure
        m_flLog.TruncateAt(c8Org);
        return m_c4FreesUsed;
    }

    // Shouldn't ever happen, but make sure we don't overflow the free list
    if (m_c4FreesUsed + 1 >= kCIDLogSrv::c4MaxFrees)
    {
        facCIDLogSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kLogSErrs::errcFree_Overflow
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::OutResource
        );
    }

    //
    //  A very common scenario is that there is a free list item at the end
    //  of the file, which isn't big enough. Instead of orphaning that small
    //  bit, we'll see if that guy exists. If so, we'll just expand it instead
    //  of adding another.
    //
    //  So calculate where the new storate would start (in our store-relative
    //  positioning scheme. Then search for a free list item that would end
    //  just before that. If we don't find one, the index ends up at the first
    //  available empty slot and we create a new item.
    //
    const tCIDLib::TCard4 c4NewOfs = tCIDLib::TCard4(c8Org) - kCIDLogSrv::c4StoreOfs;
    tCIDLib::TCard4 c4Ret = 0;
    while (c4Ret < m_c4FreesUsed)
    {
        if ((m_aFreeList[c4Ret].c4Ofs + m_aFreeList[c4Ret].c4Size) == c4NewOfs)
            break;
        c4Ret++;
    }

    try
    {
        //
        //  If we created a new one, then bump the count and set the new entry.
        //  Else, add the new stuff to the old size.
        //
        if (c4Ret == m_c4FreesUsed)
        {
            m_c4FreesUsed++;
            m_aFreeList[c4Ret].c4Size = kCIDLogSrv::c4ExpandK * 1024;
            m_aFreeList[c4Ret].c4Ofs = c4NewOfs;
        }
         else
        {
            // Here we keep the original offset and just adjust the size
            m_aFreeList[c4Ret].c4Size += kCIDLogSrv::c4ExpandK * 1024;
        }

        // And write this new stuff out
        WriteHeader
        (
            m_flLog, m_c4Seq, m_c4KeysUsed, m_c4FreesUsed, m_aKeyList, m_aFreeList
        );
    }

    catch(...)
    {
        // Truncate back to original size and return an error
        m_flLog.TruncateAt(c8Org);
        return m_c4FreesUsed;
    }

    // And return the index of the new one
    return c4Ret;
}


//
//  This guy is called when we've maxed out on keys, so we have to toss
//  something, expansion isn't going to help.
//
//  What we do is make a copy of the free and key lists (since this is
//  a destructive operation and we want to be sure that we've got it all
//  straight before we commit), both of them sorted by offset.
//
//  Then we start at the oldest item in the key list and put it on the
//  next free list item. As long as we are hitting continuous keys (which
//  is very common), we'll keep adding them to that free list item. If we
//  hit a gap, we'll start another free list item and start over again.
//
//  Each item we remove, we set it's offset to c4MaxCard. At the end, we
//  sort the temp list by offset and that naturally compacts the list,
//  pushing the removed ones to the end, then we can copy it back to the
//  real list, zeroing the real list and only copying the valid ones from
//  the temp, so that we get a clean new compacted list.
//
//  Finally, we sort the temp free list by offset, and make a run through
//  it, seeing if we can coallesce adjacent items. If so, we do it and
//  again set the freed ones to a max card offset and resort to compact
//  the list. And again, we zero the real free list and only copy over the
//  used temp ones, to get a clean compacted free list.
//
tCIDLib::TCard4 TCIDLogServerImpl::c4TossOldest(const tCIDLib::TCard4 c4ToToss)
{
    // Shouldn't happen, but just in case
    if (!m_c4KeysUsed)
        return 0;

    //
    //  If its >= the keys we have, then we are throwing them all away and
    //  can massively optimize.
    //
    if (c4ToToss > m_c4KeysUsed)
    {
        // Remember the current count, which is the number we're tossing
        const tCIDLib::TCard4 c4Ret = m_c4KeysUsed;
        RemoveAll();
        return c4Ret;
    }

    //
    //  Because of the destructive nature of this, we need to create a
    //  temp copy of the key and free lists, so that we only write out
    //  the changes when we know we have everything in hand.
    //
    tCIDLogSrv::TFreeItem* pFreeList = new tCIDLogSrv::TFreeItem[kCIDLogSrv::c4MaxFrees];
    TArrayJanitor<tCIDLogSrv::TFreeItem> janFree(pFreeList);
    tCIDLogSrv::TKeyItem* pKeyList = new tCIDLogSrv::TKeyItem[kCIDLogSrv::c4MaxKeys];
    TArrayJanitor<tCIDLogSrv::TKeyItem> janKey(pKeyList);

    // Copy over the current lists into our temp lists
    TRawMem::CopyMemBuf(pFreeList, m_aFreeList, kCIDLogSrv::c4FreeListSize);
    TRawMem::CopyMemBuf(pKeyList, m_aKeyList, kCIDLogSrv::c4KeyListSize);

    // And we need temp counters for the frees that we can adjust
    tCIDLib::TCard4 c4KeyCount = m_c4KeysUsed;
    tCIDLib::TCard4 c4FreeCount = m_c4FreesUsed;

    //
    //  Sort the temp key list by time so we can work through them in time
    //  order.
    //
    TArrayOps::TSort<tCIDLogSrv::TKeyItem>
    (
        pKeyList, c4KeyCount, eKeyCompTime
    );

    //
    //  Throw away events until we hit the number requested or run out.
    //  If any of them are contiguous we will create only one free list
    //  entry for them. But we not be able to go all the way through, since
    //  it might create more free entries than we have slots available. But
    //  that's very unlikely since it would have to mean that a large percent
    //  of entries are over the cuttoff and logged non-contiguously.
    //
    tCIDLib::TCard4 c4NextOfs;
    tCIDLib::TCard4 c4Removed = 0;
    tCIDLib::TCard4 c4EndIndex;
    tCIDLib::TCard4 c4Index = 0;
    while ((c4Index < c4KeyCount) && (c4Removed < c4ToToss))
    {
        // If we max out on free entries, then fall out
        if (c4FreeCount == kCIDLogSrv::c4MaxFrees)
            break;

        // Get the next key item
        tCIDLogSrv::TKeyItem& itemCur = pKeyList[c4Index];

        //
        //  Take the next free list item and start loading it up. Start off
        //  by stting its offset to the current item offset.
        //
        tCIDLogSrv::TFreeItem& itemFree = pFreeList[c4FreeCount];
        itemFree.c4Ofs = itemCur.c4Ofs;
        itemFree.c4Size = 0;

        //
        //  Start accumulating contiguous keys to this free list item.
        //
        //  DO NOT start the end index at index+1. We want to include the
        //  current one in the items to check!
        //
        c4EndIndex = c4Index;
        c4NextOfs = itemCur.c4Ofs;
        tCIDLib::TBoolean bFoundSome = kCIDLib::False;
        while ((c4EndIndex < c4KeyCount) && (c4Removed < c4ToToss))
        {
            // Get a ref to the current item
            tCIDLogSrv::TKeyItem& itemEnd = pKeyList[c4EndIndex];

            //
            //  If not contiguous with the previous one, then done. Note
            //  that we'll always do one, since we above set the next
            //  offset to offset of the first one.
            //
            if (c4NextOfs != itemEnd.c4Ofs)
                break;

            //
            //  Add this guy to the free list item, and then set its offset
            //  to max card so that it'll sort to the end of the last later.
            //
            itemFree.c4Size += itemEnd.c4Size;
            itemEnd.c4Ofs = kCIDLib::c4MaxCard;

            // And update to the next offset we'll look for
            c4NextOfs += itemEnd.c4Size;

            // Keep count of how many removed and bump our end index for next
            c4Removed++;
            c4EndIndex++;

            // Let the code below know we got some
            bFoundSome = kCIDLib::True;
        }

        //
        //  If we got anything, find a spot on the free list that we can
        //  use. We'll put this whole contiguous range in there.
        //
        if (bFoundSome)
        {
            // Bump the free list counter to keep the one we just did
            c4FreeCount++;
        }
         else
        {
            // Just move up one and try again, clean out the free list item
            itemFree.c4Ofs = 0;
            itemFree.c4Size = 0;
        }

        // Move up to the next one after the last we removed/tried
        c4Index = c4EndIndex + 1;
    }

    //
    //  If we removed any, we have to recompact the key list. We just
    //  run through it and copy down entries from the end to cover the
    //  ones we freed (with the c4MaxCard in the offset.)
    //
    if (c4Removed)
    {
        //
        //  If we removed them all, then do the optimized scenario and just
        //  zero out the used counts. Since we keep the list compacted, this
        //  just cleans out the list.
        //
        //  Else, we have to do the compaction.
        //
        if (c4Removed == c4KeyCount)
        {
            RemoveAll();
        }
         else
        {
            // Resort the list by ofs to push the freed up ones to the end
            TArrayOps::TSort<tCIDLogSrv::TKeyItem>(pKeyList, c4KeyCount, eKeyCompOfs);

            // Adjust the keys by the number we've removed
            c4KeyCount -= c4Removed;

            //
            //  Take a whack at conglomerating adjacent items free list items.
            //  if we have a good number of them. So sort it by offset and
            //  then work through it. As above, if we take an item and put it
            //  into a previous item, we set it's offset to max card, so we
            //  can sort again at the end to compact.
            //
            if (c4FreeCount > 256)
            {
                TArrayOps::TSort<tCIDLogSrv::TFreeItem>(pFreeList, c4FreeCount, eCompFreeItem);

                c4Index = 0;
                c4Removed = 0;
                while (c4Index < c4FreeCount)
                {
                    //
                    //  Get the current free item. Ans then while subsequent
                    //  ones are contiguous, add them to this guy and let them
                    //  go.
                    //
                    tCIDLogSrv::TFreeItem& itemCur = pFreeList[c4Index++];
                    c4NextOfs = itemCur.c4Ofs + itemCur.c4Size;
                    while ((c4Index < c4FreeCount)
                    &&     (pFreeList[c4Index].c4Ofs == c4NextOfs))
                    {
                        c4NextOfs += pFreeList[c4Index].c4Size;
                        itemCur.c4Size += pFreeList[c4Index].c4Size;
                        pFreeList[c4Index].c4Ofs = kCIDLib::c4MaxCard;
                        c4Index++;
                        c4Removed++;
                    }
                }

                //
                //  If we removed any, then sort again to push freed ones up.
                //  We reduce the free count by the number we removed, and that
                //  will cause only the remaining ones to get copied back
                //  below.
                //
                //  NOTE: It's impossible to remove them all, so we don't have
                //  to check for that. We can get down to one. We have to
                //  sort no matter what, to push even that one down to the
                //  start of the list.
                //
                if (c4Removed)
                {
                    TArrayOps::TSort<tCIDLogSrv::TFreeItem>(pFreeList, c4FreeCount, eCompFreeItem);
                    c4FreeCount -= c4Removed;
                }
            }

            //
            //  Ok, update our actual values with the stuff we just wrote. To
            //  get clean data, we zero out lists out first, then copy only
            //  the used items from the temp lists.
            //
            m_c4KeysUsed = c4KeyCount;
            m_c4FreesUsed = c4FreeCount;

            TRawMem::SetMemBuf(m_aKeyList, kCIDLib::c1MinCard, kCIDLogSrv::c4KeyListSize);
            TRawMem::SetMemBuf(m_aFreeList, kCIDLib::c1MinCard, kCIDLogSrv::c4FreeListSize);
            TRawMem::CopyMemBuf(m_aFreeList, pFreeList, m_c4FreesUsed * sizeof(tCIDLogSrv::TFreeItem));
            TRawMem::CopyMemBuf(m_aKeyList, pKeyList, m_c4KeysUsed * sizeof(tCIDLogSrv::TKeyItem));

            // Ok, we can write the info out now
            WriteHeader(m_flLog, m_c4Seq, m_c4KeysUsed, m_c4FreesUsed, m_aKeyList, m_aFreeList);
        }
    }
    return c4Removed;
}


//
//  This gets called when our fragmentation gets really high, which we
//  take as our free list being 80% full.
//
//  Instead of doing the tricky and time consuming job of physically
//  compacting the current file, we will create a new file, write all of
//  the exisitng items out contiguously, which will naturally compact it,
//  and then create a new single free item at the end of the new file. We
//  will then close the current file, make it a backup, and switch over to
//  the new one.
//
tCIDLib::TVoid TCIDLogServerImpl::CompactFile()
{
    // Create it with a temp name first
    TPathStr pathTempLog(facCIDLogSrv.strLogPath());
    pathTempLog.AddLevel(kCIDLogSrv::pszTmpLogFileName);

    try
    {
        //
        //  Sort the current key list. We want to do this so that we can
        //  pull items out of the source file in order. Note that, even if
        //  we have to back out, this is not a destructive operation, since
        //  they each have their own offset/size info, and it doesn't really
        //  matter what the order is.
        //
        TArrayOps::TSort<tCIDLogSrv::TKeyItem>(m_aKeyList, m_c4KeysUsed, eKeyCompOfs);

        // Create and zero out new free and key lists
        tCIDLogSrv::TFreeItem* pFreeList = new tCIDLogSrv::TFreeItem[kCIDLogSrv::c4MaxFrees];
        TArrayJanitor<tCIDLogSrv::TFreeItem> janFree(pFreeList);
        TRawMem::SetMemBuf(pFreeList, kCIDLib::c1MinCard, kCIDLogSrv::c4FreeListSize);

        tCIDLogSrv::TKeyItem* pKeyList = new tCIDLogSrv::TKeyItem[kCIDLogSrv::c4MaxKeys];
        TArrayJanitor<tCIDLogSrv::TKeyItem> janKey(pKeyList);
        TRawMem::SetMemBuf(pKeyList, kCIDLib::c1MinCard, kCIDLogSrv::c4KeyListSize);

        // Now create the temp file
        TBinaryFile flTmp;
        flTmp.strName(pathTempLog);
        flTmp.Open
        (
            tCIDLib::EAccessModes::Excl_ReadWrite
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::AllOwnerAccess
            , tCIDLib::EFileFlags::SequentialScan
        );

        // Write out a place holder header until we know the real info
        WriteHeader(flTmp, m_c4Seq, 0, 0, pKeyList, pFreeList);

        //
        //  Now go through and copy items from the source file to the target
        //  file, in the key order. Use a buffer big enough to hold pretty
        //  much any item, but we have to be prepared to do an item in
        //  sections if needed.
        //
        //  The target file is at the correct position now to start writing
        //  out contiguous items. We put in a new item in our temp key list
        //  for each new item, at their new location.
        //
        constexpr tCIDLib::TCard4 c4IOBufSz = 8192;
        THeapBuf mbufIO(c4IOBufSz, c4IOBufSz);
        tCIDLib::TCard4 c4NextOfs = 0;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4KeysUsed; c4Index++)
        {
            // Seek to the src file pos
            const tCIDLib::TCard4 c4Ofs = m_aKeyList[c4Index].c4Ofs;
            m_flLog.SetFilePos(c4Ofs + kCIDLogSrv::c4StoreOfs);

            tCIDLib::TCard4 c4Left = m_aKeyList[c4Index].c4Size;
            while (c4Left)
            {

                tCIDLib::TCard4 c4ThisTime = tCIDLib::MinVal(c4Left, c4IOBufSz);
                m_flLog.c4ReadBuffer(mbufIO, c4ThisTime);
                flTmp.c4WriteBuffer(mbufIO, c4ThisTime);
                c4Left -= c4ThisTime;
            }

            // Set the outgoing key item with the new offset and existing size
            pKeyList[c4Index].c4Ofs = c4NextOfs;
            pKeyList[c4Index].c4Size = m_aKeyList[c4Index].c4Size;

            // Move up to the next target offset
            c4NextOfs += m_aKeyList[c4Index].c4Size;
        }

        //
        //  Initialize a buffer the size of the expansion, and write it out
        //  now at the end.
        //
        const tCIDLib::TCard4 c4ExpSz = kCIDLogSrv::c4ExpandK * 1024;
        THeapBuf mbufInit(c4ExpSz, c4ExpSz);
        mbufInit.Set(0, 0, c4ExpSz);
        flTmp.c4WriteBuffer(mbufInit, c4ExpSz);

        //
        //  Set up a single free list item that holds all of the new space
        //  in the file, which will also include some space that we got by
        //  the compaction plus the expansion. It will be at the last offset
        //  we left off from above.
        //
        //  The size is the full size, minus the start of the store part of
        //  the file, minus the now used part.
        //
        pFreeList[0].c4Ofs = c4NextOfs;
        pFreeList[0].c4Size = tCIDLib::TCard4(flTmp.c8CurSize()) - kCIDLogSrv::c4StoreOfs;
        pFreeList[0].c4Size -= c4NextOfs;

        // Write out the correct header data now
        WriteHeader(flTmp, m_c4Seq, m_c4KeysUsed, 1, pKeyList, pFreeList);

        // And update our lists now
        m_c4FreesUsed = 1;
        TRawMem::CopyMemBuf(m_aFreeList, pFreeList, kCIDLogSrv::c4FreeListSize);
        TRawMem::CopyMemBuf(m_aKeyList, pKeyList, kCIDLogSrv::c4KeyListSize);

        // Close the file to flush everything out
        flTmp.Close();
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        //
        //  Do a panic reset. We will close and delete the current file
        //  and creae a new one from scratch.
        //
        PanicReset();
        return;
    }

    catch(...)
    {
        //
        //  Do a panic reset. We will close and delete the current file
        //  and creae a new one from scratch.
        //
        PanicReset();
        return;
    }

    //
    //  Now we need to close the current file, rename it as the backup file,
    //  after deleting any previous backup, rename the temp file as the
    //  official file, and open it back up as the current log file.
    //
    try
    {
        //
        //  Get rid of any old backup. Be paranoid and remove any file
        //  attributes, which might prevent us from removing it.
        //
        TPathStr pathBackLog(facCIDLogSrv.strLogPath());
        pathBackLog.AddLevel(kCIDLogSrv::pszBackLogFileName);

        if (TFileSys::bExists(pathBackLog))
        {
            TFileSys::SetPerms(pathBackLog, tCIDLib::EFilePerms::AllWE);
            TFileSys::DeleteFile(pathBackLog);
        }

        // Close the current file and rename
        m_flLog.Close();
        TFileSys::Rename(m_flLog.strName(), pathBackLog);

        // Rename the temp to the current
        TFileSys::Rename(pathTempLog, m_flLog.strName());

        // Now open the new file back up as the new file
        m_flLog.Open
        (
            tCIDLib::EAccessModes::Excl_ReadWrite
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::AllOwnerAccess
            , CIDLogSrv_Impl::eLogFileFlags
        );
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        //
        //  Do a panic reset. We will close and delete the current file
        //  and creae a new one from scratch.
        //
        PanicReset();
    }

    catch(...)
    {
        //
        //  Do a panic reset. We will close and delete the current file
        //  and creae a new one from scratch.
        //
        PanicReset();
    }
}


tCIDLib::TVoid TCIDLogServerImpl::DumpDebugInfo(TTextOutStream& strmOut)
{
    tCIDLib::TCard4 c4Count = 0;
    tCIDLogSrv::TSortItem aitemList[kCIDLogSrv::c4MaxFrees + kCIDLogSrv::c4MaxKeys];

    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4FreesUsed; c4Index++)
    {
        aitemList[c4Count].eType = tCIDLogSrv::ETypes::Free;
        aitemList[c4Count].pc4This = (tCIDLib::TCard4*)&m_aFreeList[c4Index];
        c4Count++;
    }

    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4KeysUsed; c4Index++)
    {
        aitemList[c4Count].eType = tCIDLogSrv::ETypes::Key;
        aitemList[c4Count].pc4This = (tCIDLib::TCard4*)&m_aKeyList[c4Index];
        c4Count++;
    }

    // Sort this list now
    if (c4Count > 1)
    {
        TArrayOps::TSort<tCIDLogSrv::TSortItem>
        (
            aitemList, c4Count, eCompSortItem
        );
    }

    strmOut << L"------------------------" << kCIDLib::EndLn;

    //
    //  Ok, lets run through the list. On each loop through, either the
    //  free list or the key list must cover the next chunk in the file.
    //  If there is any gap not covered by either list, or there is any
    //  overlap, that's an error.
    //
    tCIDLib::TCard4         c4NextOfs = 0;
    tCIDLib::TCard4         c4CurSize = 0;
    tCIDLogSrv::TKeyItem*   pitemKey = nullptr;
    tCIDLogSrv::TFreeItem*  pitemFree = nullptr;

    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        tCIDLogSrv::TSortItem& itemCur = aitemList[c4Index];

        if (itemCur.eType == tCIDLogSrv::ETypes::Free)
        {
            pitemFree = reinterpret_cast<tCIDLogSrv::TFreeItem*>(itemCur.pc4This);
            strmOut << L"FREE - "
                    << L"Ofs:" << pitemFree->c4Ofs
                    << L" Size:" << pitemFree->c4Size
                    << L" EndAt:" << (pitemFree->c4Ofs + pitemFree->c4Size - 1);
            c4CurSize = pitemFree ->c4Size;
        }
         else
        {
            pitemKey = reinterpret_cast<tCIDLogSrv::TKeyItem*>(itemCur.pc4This);

            strmOut << L"KEY  - "
                    << L"Ofs:" << pitemKey->c4Ofs
                    << L" Size:" << pitemKey->c4Size
                    << L" EndAt:" << (pitemKey->c4Ofs + pitemKey->c4Size - 1);
            c4CurSize = pitemKey->c4Size;
        }

        if (*itemCur.pc4This < c4NextOfs)
            strmOut << L"  (Overlaps previous block)";
        else if (*itemCur.pc4This > c4NextOfs)
            strmOut << L"  (Gap before block)";
        strmOut << kCIDLib::NewLn;

        // Set what we think the next offset should be
        c4NextOfs = *itemCur.pc4This + c4CurSize;
    }

    strmOut << L"------------------------" << kCIDLib::EndLn;
}


tCIDLib::EExitCodes
TCIDLogServerImpl::eFlusherThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let our calling thread go
    thrThis.Sync();

    // Loop until we told to shutdown
    while(kCIDLib::True)
    {
        try
        {
            // If we get an exit signal, then exit
            if (thrThis.bCheckShutdownRequest())
                break;

            // Sleep for a second
            if (!thrThis.bSleep(1000))
                break;

            // Lock the mutex and see if we need to save the header
            {
                TLocker lockrSync(&m_mtxSync);

                // If new stuff has arrived, then store it out
                if (m_c4LastFlushSeq != m_c4Seq)
                {
                    //
                    //  Save the new sequence first so that if it fails
                    //  we won't just keep banging away.
                    //
                    m_c4LastFlushSeq = m_c4Seq;
                    WriteHeader
                    (
                        m_flLog
                        , m_c4Seq
                        , m_c4KeysUsed
                        , m_c4FreesUsed
                        , m_aKeyList
                        , m_aFreeList
                    );
                    m_flLog.Flush();
                }
            }
        }

        catch(const TError& errToCatch)
        {
            if (facCIDLogSrv.bShouldLog(errToCatch))
                TModule::LogEventObj(errToCatch);

            if (facCIDLogSrv.bLogFailures())
            {
                facCIDLogSrv.LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kLogSErrs::errcFile_FlusherWriteFailed
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }
        }

        catch(...)
        {
            if (facCIDLogSrv.bLogFailures())
            {
                facCIDLogSrv.LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kLogSErrs::errcFile_FlusherWriteFailed
                    , tCIDLib::ESeverities::Status
                    , tCIDLib::EErrClasses::CantDo
                );
            }
        }
    }
    return tCIDLib::EExitCodes::Normal;
}


tCIDLib::TVoid TCIDLogServerImpl::FreeKeyItem(const tCIDLib::TCard4 c4Index)
{
    // Make sure its within the current count of used items
    if (c4Index >= m_c4KeysUsed)
    {
        facCIDLogSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kLogSErrs::errcKeys_BadIndex
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4Index)
            , TCardinal(m_c4KeysUsed)
        );
    }

    // Bump down the count of keys used now
    m_c4KeysUsed--;

    //
    //  If more than one existed and the one being removed is not the last one
    //  in the list, then copy the last one in the list down into this freed
    //  slot, since we have to keep it compacted.
    //
    if (m_c4KeysUsed && (c4Index != m_c4KeysUsed))
        m_aKeyList[c4Index] = m_aKeyList[m_c4KeysUsed];
}


//
//  This is called to initialize a new log file. We just write out a default
//  file header and initial free list storage.
//
tCIDLib::TVoid TCIDLogServerImpl::InitializeLogFile(const TString& strFileName)
{
   tCIDLib::TBoolean bOk = kCIDLib::True;
    try
    {
        // Create the new file
        m_flLog.strName(strFileName);
        m_flLog.Open
        (
            tCIDLib::EAccessModes::Excl_ReadWrite
            , tCIDLib::ECreateActs::OpenOrCreate
            , tCIDLib::EFilePerms::AllOwnerAccess
            , CIDLogSrv_Impl::eLogFileFlags
        );

        // Zero out our key and free list arrays.
        TRawMem::SetMemBuf(m_aFreeList, kCIDLib::c1MinCard, kCIDLogSrv::c4FreeListSize);
        TRawMem::SetMemBuf(m_aKeyList, kCIDLib::c1MinCard, kCIDLogSrv::c4KeyListSize);

        //
        //  Then set up a single entry in the free list which holds all of
        //  the data we'll initially write out for the file.
        //
        const tCIDLib::TCard4 c4InitStore = (kCIDLogSrv::c4ExpandK * 2) * 1024;
        m_aFreeList[0].c4Ofs = 0;
        m_aFreeList[0].c4Size = c4InitStore;

        // Initialize our members that go into the header to defaults
        m_c4Seq = 1;
        m_c4LastFlushSeq = 1;
        m_c4KeysUsed = 0;
        m_c4FreesUsed = 1;

        // And now write out the header data
        WriteHeader(m_flLog, m_c4Seq, m_c4KeysUsed, m_c4FreesUsed, m_aKeyList, m_aFreeList);

        //
        //  Create a zeroed dummy buffer and write out the initial chunk of
        //  store space.
        //
        THeapBuf mbufInit(c4InitStore, c4InitStore);
        mbufInit.Set(0, 0, c4InitStore);
        m_flLog.c4WriteBuffer(mbufInit, c4InitStore);

        m_flLog.Close();
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
        bOk = kCIDLib::False;
    }

    catch(...)
    {
        bOk = kCIDLib::False;
    }

    // If anything goes wrong, delete it
    if (!bOk)
    {
        if (m_flLog.bIsOpen())
            m_flLog.Close();
        TFileSys::DeleteFile(strFileName);

        // Log an error and exit
        facCIDLogSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kLogSErrs::errcInit_FileInitFailed
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


tCIDLib::TVoid TCIDLogServerImpl::PanicReset()
{
    //
    //  Something went badly wrong, so we are going to start over, so that
    //  we can keep going.
    //
    if (m_flLog.bIsOpen())
        m_flLog.Close();

    TPathStr pathLogFile(facCIDLogSrv.strLogPath());
    pathLogFile.AddLevel(kCIDLogSrv::pszLogFileName);

    // If we don't find it, then initialize it
    if (TFileSys::bExists(pathLogFile))
        TFileSys::DeleteFile(pathLogFile);

    // Call the initialize function
    InitializeLogFile(pathLogFile);
}


tCIDLib::TVoid TCIDLogServerImpl::ReadHeader()
{
    // Seek it back to zero first
    m_flLog.SetFilePos(0);

    // Read in the header first into a temp
    tCIDLogSrv::TFileHeader hdrTmp;
    if (m_flLog.c4ReadBuffer(&hdrTmp, sizeof(hdrTmp)) != sizeof(hdrTmp))
    {
        facCIDLogSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kLogSErrs::errcFile_HdrReadFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllRead
        );
    }

    // Check the header marker
    if (!TRawStr::bCompareStr(hdrTmp.achMarker, "CIDLOG"))
    {
        facCIDLogSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kLogSErrs::errcFile_BadHdrMarker
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    //
    //  <TBD> Check the format version if we have a format changes
    //  So far we've not had to change the file format.
    //

    // And update out members from this data
    m_c4Seq = hdrTmp.c4LastSeq;
    m_c4LastFlushSeq = m_c4Seq;
    m_c4FreesUsed = hdrTmp.c4FreesUsed;
    m_c4KeysUsed = hdrTmp.c4KeysUsed;

    //
    //  Now read in the key and free lists. For debugging purposes, we write
    //  the value 0xDEADBEEF between the two, to make it easy to find the
    //  split point manually, when spelunking through the files with a hex
    //  editor.
    //
    if (m_flLog.c4ReadBuffer(m_aKeyList, kCIDLogSrv::c4KeyListSize) != kCIDLogSrv::c4KeyListSize)
    {
        facCIDLogSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kLogSErrs::errcFile_KeyReadFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllRead
        );
    }
    tCIDLib::TCard4 c4DeadBeef;
    if (m_flLog.c4ReadBuffer(&c4DeadBeef, sizeof(c4DeadBeef)) != sizeof(c4DeadBeef))
    {
        facCIDLogSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kLogSErrs::errcFile_BeefReadFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllRead
        );
    }
    if (c4DeadBeef != CIDLogSrv_Impl::c4DeadBeef)
    {
        facCIDLogSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kLogSErrs::errcFile_NoDeadBeef
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::Format
        );
    }
    if (m_flLog.c4ReadBuffer(m_aFreeList, kCIDLogSrv::c4FreeListSize) != kCIDLogSrv::c4FreeListSize)
    {
        facCIDLogSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kLogSErrs::errcFile_FreeReadFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllRead
        );
    }

    // And we should now be at the precalculated store offset
    #if CID_DEBUG_ON
    if (m_flLog.c8CurPos() != kCIDLogSrv::c4StoreOfs)
    {
        facCIDLogSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kLogSErrs::errcFile_NotAtStoreOfs
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::CantDo
        );
    }
    #endif
}


tCIDLib::TVoid
TCIDLogServerImpl::ReadOne(TBinInStream& strmSrc, TLogEvent& logevToFill)
{
    // Check for the frame marker
    strmSrc.CheckForFrameMarker(CID_FILE, CID_LINE);

    // Now we should get the log event object
    strmSrc >> logevToFill;
}


//
//  Because we need to write headers with other than the current key/free
//  list info, we take the values to write. DON'T change this to access
//  any member.
//
tCIDLib::TVoid
TCIDLogServerImpl::WriteHeader(         TBinaryFile&            flTarget
                                , const tCIDLib::TCard4         c4Seq
                                , const tCIDLib::TCard4         c4KeysUsed
                                , const tCIDLib::TCard4         c4FreesUsed
                                , const tCIDLogSrv::TKeyItem*   paKeyList
                                , const tCIDLogSrv::TFreeItem*  paFreeList)
{
    // Seek it back to zero first
    flTarget.SetFilePos(0);

    // Fill in a temp header and write it out
    tCIDLogSrv::TFileHeader hdrTmp = {0};
    TRawStr::CopyStr(hdrTmp.achMarker, "CIDLOG", 7);
    hdrTmp.c4FmtVersion = kCIDLogSrv::c4FmtVersion;
    hdrTmp.c4LastSeq    = c4Seq;
    hdrTmp.c4FreesUsed  = c4FreesUsed;
    hdrTmp.c4KeysUsed   = c4KeysUsed;
    if (flTarget.c4WriteBuffer(&hdrTmp, sizeof(hdrTmp)) != sizeof(hdrTmp))
    {
        facCIDLogSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kLogSErrs::errcFile_HdrWriteFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllWritten
        );
    }

    // Now write out the key and free lists, with the separator char between
    if (flTarget.c4WriteBuffer(paKeyList, kCIDLogSrv::c4KeyListSize) != kCIDLogSrv::c4KeyListSize)
    {
        facCIDLogSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kLogSErrs::errcFile_KeyWriteFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllWritten
        );
    }

    tCIDLib::TCard4 c4DeadBeef = CIDLogSrv_Impl::c4DeadBeef;
    if (flTarget.c4WriteBuffer(&c4DeadBeef, sizeof(c4DeadBeef)) != sizeof(c4DeadBeef))
    {
        facCIDLogSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kLogSErrs::errcFile_BeefWriteFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllWritten
        );
    }

    if (flTarget.c4WriteBuffer(paFreeList, kCIDLogSrv::c4FreeListSize) != kCIDLogSrv::c4FreeListSize)
    {
        facCIDLogSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kLogSErrs::errcFile_FreeWriteFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllWritten
        );
    }

    // And we should now be at the precalculated store offset
    #if CID_DEBUG_ON
    if (flTarget.c8CurPos() != kCIDLogSrv::c4StoreOfs)
    {
        facCIDLogSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kLogSErrs::errcFile_NotAtStoreOfs
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::CantDo
        );
    }
    #endif
}


tCIDLib::TVoid TCIDLogServerImpl::WriteOne(const TLogEvent& logevToWrite)
{
    //
    //  Now write a frame marker and the error object itself. Flush it to
    //  get the data out into the memory buffer.
    //
    m_strmBuf.TruncateAt(0);
    m_strmBuf   << tCIDLib::EStreamMarkers::Frame
                << logevToWrite
                << kCIDLib::FlushIt;

    // Get the current position as the size of the buffer we need to write
    const tCIDLib::TCard4 c4SizeNeeded = m_strmBuf.c4CurPos();

    //
    //  If the item is bigger than 2K, ignore it
    //
    //  <TBD> We should probably provide some sort of diagnostic counter
    //  for this.
    //
    if (c4SizeNeeded > 2048)
        return;

    // Store this guy's time stamp as the last logged time
    m_enctLastLogged = logevToWrite.enctLogged();

    //
    //  If our key list is full, then we don't have any choices here. There
    //  isn't any place to put this new guy, even if we have space. So we
    //  throw away the 512 oldest items.
    //
    //  If we do this, then we'll find a free list item big enough below
    //  and take it.
    //
    if (m_c4KeysUsed >= kCIDLogSrv::c4MaxKeys)
        c4TossOldest(512);

    //
    //  And now lets find a place to store it. See if there is a freelist
    //  entry with enough space. If so, take it and be happy. If we dumped
    //  old items above, we'll be sure to find one.
    //
    tCIDLib::TCard4 c4FreeIndex = 0;
    for (; c4FreeIndex < m_c4FreesUsed; c4FreeIndex++)
    {
        const tCIDLogSrv::TFreeItem& itemCur = m_aFreeList[c4FreeIndex];

        // See if it has enough space
        if (itemCur.c4Size >= c4SizeNeeded)
            break;
    }

    //
    //  If we didn't find one, and the key list is not full, then we can
    //  expand the file. We'll let it grow up to large enough to hold a
    //  full whack of key values. Note that we won't just toss old items
    //  below and then expand, because we will find a free list item if
    //  we tossed old items. So we'll only do this if we had available
    //  keys but no free space big enough.
    //
    if ((c4FreeIndex == m_c4FreesUsed) && (m_c4KeysUsed < kCIDLogSrv::c4MaxKeys))
        c4FreeIndex = c4Expand();

    //
    //  If we couldn't or didn't expand, and we have a large amount of
    //  fragmentation, i.e. a lot of free list items, then let's try to
    //  compact.
    //
    if (c4FreeIndex == m_c4FreesUsed)
    {
        //
        //  First try the free list compaction. It'll just coallesce adjacent
        //  free list items.
        //
        c4FreeIndex = c4CompactFreeList(c4SizeNeeded);

        //
        //  If that still didn't, then we rewrite the file. This will write
        //  out all the current values contiguously to a new file, to get
        //  rid of any fragmentation, expand the file at the end, and swap
        //  it in as the new file.
        //
        if (c4FreeIndex == m_c4FreesUsed)
        {
            CompactFile();

            //
            //  This will create one big free index item, at the 0th
            //  index. So we'll take that one.
            //
            c4FreeIndex = 0;
        }
    }

    //
    //  Worst case, if we still couldn't get space, then toss old items
    //  even though the key list is not full
    //
    if (c4FreeIndex == m_c4FreesUsed)
    {
        c4TossOldest(512);
        c4FreeIndex = 0;
        for (; c4FreeIndex < m_c4FreesUsed; c4FreeIndex++)
        {
            const tCIDLogSrv::TFreeItem& itemCur = m_aFreeList[c4FreeIndex];
            if (itemCur.c4Size >= c4SizeNeeded)
                break;
        }
    }

    // If still not, then give up. Something is really wrong
    if (c4FreeIndex == m_c4FreesUsed)
    {
        facCIDLogSrv.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kLogSErrs::errcFile_CantFreeSpace
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::CantDo
        );
    }

    //
    //  Ok, finally, we can do it. So lets write our data at the start of
    //  the free list block we found. If its successful, then we'll update
    //  the block ofs. The offsets are relative to the store offset, so add
    //  that to the free block offset.
    //
    tCIDLogSrv::TFreeItem& itemFree = m_aFreeList[c4FreeIndex];
    m_flLog.SetFilePos(itemFree.c4Ofs + kCIDLogSrv::c4StoreOfs);
    m_flLog.c4WriteBuffer(m_strmBuf.mbufData(), c4SizeNeeded);

    // Fill in the next available item, then bump the index and sequence
    tCIDLogSrv::TKeyItem& itemKey = m_aKeyList[m_c4KeysUsed];
    itemKey.c4Ofs = m_aFreeList[c4FreeIndex].c4Ofs;
    itemKey.c4Size = c4SizeNeeded;
    itemKey.enctLogged = logevToWrite.enctLogged();
    itemKey.c1Sev = tCIDLib::TCard1(logevToWrite.eSeverity());
    itemKey.c1Class = tCIDLib::TCard1(logevToWrite.eClass());
    itemKey.c4Seq = m_c4Seq++;
    m_c4KeysUsed++;

    //
    //  And adjust the free list entry we stole from. Don't create a tiny
    //  sliver of a free list item. So if this would only leave 128 bytes or
    //  less in this entry, just give that to the new item.
    //
    itemFree.c4Size -= c4SizeNeeded;
    itemFree.c4Ofs += c4SizeNeeded;
    if (itemFree.c4Size < 128)
    {
        itemKey.c4Size += itemFree.c4Size;
        itemFree.c4Size = 0;
    }

    //
    //  If its gone zero size, then lets free it. We'll copy the last item in
    //  the free list down to overwrite this one. If we only have one, or its
    //  already the last one, then we don't do any move.
    //
    if (!itemFree.c4Size)
    {
        m_c4FreesUsed--;
        if (m_c4FreesUsed && (c4FreeIndex != m_c4FreesUsed))
            m_aFreeList[c4FreeIndex] = m_aFreeList[m_c4FreesUsed];
    }

    // Add this one to the live queue
    AddToLiveQueue(logevToWrite);
}

