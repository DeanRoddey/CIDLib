//
// FILE NAME: CIDDAE_Ripper.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2006
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
//  This file implements core ripper class and the non-virtual parts of
//  the base encoder interface.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDDAE_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDDAERipper,TObject)


// ---------------------------------------------------------------------------
//  Local types and data
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDDAE_Ripper
    {
        // -----------------------------------------------------------------------
        //  We need to be able to provide unique thread names for our read/write
        //  threads.
        // -----------------------------------------------------------------------
        TUniqueName unamInstance(L"CIDDAEThread%(1)");


        // -----------------------------------------------------------------------
        //  The maximum number of buffers we can have outstanding at once. This
        //  is how many buffers the buffer pool objects are set up for. The read
        //  thread will stop and wait for the write thread to catch up.
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard4   c4MaxBufs = 12;


        // -----------------------------------------------------------------------
        //  The number of sectors we read per pass. We may actually read more if
        //  jitter is enabled. The buffer size below is big enough to adjust for
        //  the jitter overlap.
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard4   c4SectorsPerRead = 20;


        // -----------------------------------------------------------------------
        //  The bytes we try to read per pass, and the size of the buffers. We
        //  make the buffer size larger by the jitter overlap we do.
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard4   c4BytesPerRead
        (
            c4SectorsPerRead * TKrnlRemMedia::c4CDRawSectorSz
        );
        constexpr tCIDLib::TCard4   c4BufSz
        (
            (c4SectorsPerRead + 2) * TKrnlRemMedia::c4CDRawSectorSz
        );


        // -----------------------------------------------------------------------
        //  The bytes and blocks in our overlap area.
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard4   c4OverlapBlocks(2);
        constexpr tCIDLib::TCard4   c4OverlapBytes(TKrnlRemMedia::c4CDRawSectorSz * 2);
    }
}



// ---------------------------------------------------------------------------
//  Our derivative of simple pool for DAE buffers
// ---------------------------------------------------------------------------
class TDAEBufPool : public TFixedSizePool<TDAEBuf>
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TDAEBufPool() :

            TFixedSizePool
            (
                CIDDAE_Ripper::c4MaxBufs * 2, L"DAE Buffer Pool", tCIDLib::EMTStates::Safe
            )
        {
        }

        TDAEBufPool(const TDAEBufPool&) = delete;
        TDAEBufPool(TDAEBufPool&&) = delete;

        ~TDAEBufPool() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TDAEBufPool& operator=(const TDAEBufPool&) = delete;
        TDAEBufPool& operator=(TDAEBufPool&&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        [[nodiscard]] TDAEBuf* pelemMakeNew() override
        {
            return new TDAEBuf(CIDDAE_Ripper::c4BufSz);
        }

        //
        //  If smaller the size, then expand it up to that plus the expansion
        //  increment. If not, we just clear the string to get it ready for the
        //  next use.
        //
        tCIDLib::TVoid PrepElement(TDAEBuf& daebTar) override
        {
            daebTar.Reset(CIDDAE_Ripper::c4BufSz);
        }
};




// ---------------------------------------------------------------------------
//  CLASS: TCIDDAERipper
// PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDDAERipper: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDDAERipper::TCIDDAERipper() :

    m_bReadComplete(kCIDLib::False)
    , m_bReadErr(kCIDLib::False)
    , m_bWriteErr(kCIDLib::False)
    , m_c4BlkCount(0)
    , m_c4EndBlk(0)
    , m_c4PercentDone(0)
    , m_c4ReadErrs(0)
    , m_c4StitchErrs(0)
    , m_c4Stitches(0)
    , m_c4StartBlk(0)
    , m_c4TrackNum(0)
    , m_colWorkQ(tCIDLib::EAdoptOpts::NoAdopt)
    , m_eJitterOpt(tCIDDAE::EJitterOpts::None)
    , m_pbBreakFlag(nullptr)
    , m_pdaeeToUse(nullptr)
    , m_prmmdToUse(nullptr)
    , m_psplBuffers(new TDAEBufPool)
    , m_pthrRead(nullptr)
    , m_pthrWrite(nullptr)
    , m_strCodecName()
    , m_strFmtName()
    , m_strOutFile()
    , m_TOCData()
    , m_c4Year(0)
{
    m_pthrRead = new TThread
    (
        CIDDAE_Ripper::unamInstance.strQueryNewName()
        , TMemberFunc<TCIDDAERipper>(this, &TCIDDAERipper::eReadThread)
    );

    m_pthrWrite = new TThread
    (
        CIDDAE_Ripper::unamInstance.strQueryNewName()
        , TMemberFunc<TCIDDAERipper>(this, &TCIDDAERipper::eWriteThread)
    );
}

TCIDDAERipper::~TCIDDAERipper()
{
    // Clean up the threads
    try
    {
        delete m_pthrRead;
    }

    catch(...)
    {
    }

    try
    {
        delete m_pthrWrite;
    }

    catch(...)
    {
    }

    //
    //  Release all the pool objects and the pool itself, which will delete
    //  the objects.
    //
    try
    {
        if (m_psplBuffers)
        {
            m_psplBuffers->ReleaseAll();
            delete m_psplBuffers;
        }
    }

    catch(...)
    {
        if (facCIDDAE().bLogFailures())
        {
            facCIDDAE().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kDAEErrs::errcData_PoolCleanup
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppStatus
            );
        }
    }
}


// ---------------------------------------------------------------------------
//  TCIDDAERipper: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Returns the current percent done value
tCIDLib::TCard4 TCIDDAERipper::c4PercentDone() const
{
    return m_c4PercentDone;
}


// Returns the current read error count
tCIDLib::TCard4 TCIDDAERipper::c4ReadErrs() const
{
    return m_c4ReadErrs;
}


// Returns the current stitch error count
tCIDLib::TCard4 TCIDDAERipper::c4StitchErrs() const
{
    return m_c4StitchErrs;
}


// Returns the current stitch count
tCIDLib::TCard4 TCIDDAERipper::c4Stitches() const
{
    return m_c4Stitches;
}


// Set or get the jitter option
tCIDDAE::EJitterOpts TCIDDAERipper::eJitterOpt() const
{
    return m_eJitterOpt;
}

tCIDDAE::EJitterOpts
TCIDDAERipper::eJitterOpt(const tCIDDAE::EJitterOpts eToSet)
{
    m_eJitterOpt = eToSet;
    return m_eJitterOpt;
}


//
//  This is the main method of the engine. This does the ripping of a single
//  track to a target file. We have one that takes metadata, and another that
//  doesn't. The one that doesn't just calls the other one with empty data.
//

tCIDLib::TVoid
TCIDDAERipper::RipTrack(        TKrnlRemMediaDrv&   rmmdToUse
                        , const TString&            strCodecName
                        , const TString&            strFmtName
                        , const tCIDLib::TCard4     c4TrackNum
                        ,       TCIDDAEEncoder&     daeeToUse
                        , const TString&            strTrackName
                        , CIOP  tCIDLib::TBoolean&  bBreakFlag)
{
    RipTrack
    (
        rmmdToUse
        , strCodecName
        , strFmtName
        , c4TrackNum
        , daeeToUse
        , strTrackName
        , bBreakFlag
        , TString::strEmpty()
        , TString::strEmpty()
        , TString::strEmpty()
        , TString::strEmpty()
        , TString::strEmpty()
        , 0
    );
}

tCIDLib::TVoid
TCIDDAERipper::RipTrack(        TKrnlRemMediaDrv&   rmmdToUse
                        , const TString&            strCodecName
                        , const TString&            strFmtName
                        , const tCIDLib::TCard4     c4TrackNum
                        ,       TCIDDAEEncoder&     daeeToUse
                        , const TString&            strTrackName
                        , CIOP  tCIDLib::TBoolean&  bBreakFlag
                        , const TString&            strAlbumTitle
                        , const TString&            strTrackTitle
                        , const TString&            strArtist
                        , const TString&            strLabel
                        , const TString&            strGenre
                        , const tCIDLib::TCard4     c4Year)
{
    //
    //  We need to store some pointers to parms for the scope of this call,
    //  so we use janitors to do that.
    //
    TGFJanitor<TKrnlRemMediaDrv*> janCD(&m_prmmdToUse, &rmmdToUse);
    TGFJanitor<TCIDDAEEncoder*> janEncoder(&m_pdaeeToUse, &daeeToUse);
    TGFJanitor<tCIDLib::TBoolean*> janBreak(&m_pbBreakFlag, &bBreakFlag);

    // And store the other incoming stuff
    m_c4TrackNum = c4TrackNum;
    m_strOutFile = strTrackName;
    m_strAlbumTitle  = strAlbumTitle;
    m_strTrackTitle  = strTrackTitle;
    m_strArtist = strArtist;
    m_strCodecName = strCodecName;
    m_strFmtName = strFmtName;
    m_strLabel  = strLabel;
    m_strGenre  = strGenre;
    m_c4Year = c4Year;

    // Query the table of contents info from the CD
    if (!rmmdToUse.bQueryCDTOC(m_TOCData))
    {
        facCIDDAE().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kDAEErrs::errcData_GetTOC
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    //
    //  Store the information that the threads will use to know the sectors
    //  to rip. We call a helper method to break out the block info.
    //
    tCIDLib::TCard4 c4Minutes;
    tCIDLib::TCard4 c4Seconds;
    const tCIDLib::TBoolean bRes = TKrnlRemMedia::bExtractCDTrackInfo
    (
        m_TOCData, c4TrackNum, m_c4StartBlk, m_c4BlkCount, c4Minutes, c4Seconds
    );

    if (!bRes)
    {
        facCIDDAE().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kDAEErrs::errcData_ExtractTrack
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4TrackNum)
        );
    }

    // We precalc the end track, to avoid going it over and over
    m_c4EndBlk = m_c4StartBlk + (m_c4BlkCount - 1);

    //
    //  Set the break flag to false. This can be set by the containing app
    //  to force the read/write threads to stop. We wake up when they stop
    //  for whatever reason.
    //
    bBreakFlag = kCIDLib::False;

    // Reset any per-run stuff
    m_bReadErr = kCIDLib::False;
    m_bWriteErr = kCIDLib::False;
    m_c4PercentDone = 0;
    m_c4ReadErrs = 0;
    m_c4StitchErrs = 0;
    m_c4Stitches = 0;
    m_bReadComplete = kCIDLib::False;

    //
    //  In case we got stopped prematurely last time, release all the
    //  pool objects. If the write thread got stopped with items in the
    //  queue, or there was an error and the ones that the read or write
    //  thread was working on didn't get released, then there will be
    //  some that are still reserved.
    //
    m_psplBuffers->ReleaseAll();

    // Flush the work queue for the same reason
    m_colWorkQ.RemoveAll();

    // Now start up the two threads
    m_pthrRead->Start();
    m_pthrWrite->Start();

    //
    //  The read thread drives the process, and will get to the end first.
    //  So we block first on it. Once we see it end, then we wait for the
    //  write thread to end.
    //
    TThread* pthrUs = TThread::pthrCaller();
    tCIDLib::EExitCodes eExit = tCIDLib::EExitCodes::Normal;
    while (kCIDLib::True)
    {
        // Wait a while for it to end
        if (m_pthrRead->bWaitForDeath(eExit, 1000))
            break;

        // Not yet, check for shutdown request
        if (pthrUs->bCheckShutdownRequest())
        {
            // Force them down
            bBreakFlag = kCIDLib::True;
            break;
        }
    }

    //
    //  Once we get here, either the read thread has completed normally,
    //  and the write thread will end soon thereafter, or they were forcedc
    //  down via the break flag, in which case they will both end also.
    //
    if (!m_pthrRead->bWaitForDeath(eExit, 15000))
    {
        // It didn't exit, so try to make it exit
        try
        {
            if (m_pthrRead->bIsRunning())
                m_pthrRead->ReqShutdownSync(10000);
            m_pthrRead->eWaitForDeath(5000);
        }

        catch(const TError& errToCatch)
        {
            m_errRead = errToCatch;
            m_bReadErr = kCIDLib::True;
        }
    }

    if (!m_pthrWrite->bWaitForDeath(eExit, 15000))
    {
        // It didn't exit, so try to make it exit
        try
        {
            if (m_pthrWrite->bIsRunning())
                m_pthrWrite->ReqShutdownSync(10000);
            m_pthrWrite->eWaitForDeath(5000);
        }

        catch(const TError& errToCatch)
        {
            m_errWrite = errToCatch;
            m_bWriteErr = kCIDLib::True;
        }
    }

    // If we got read or write errors, then we throw them.
    if (m_bReadErr)
        throw m_errRead;

    if (m_bWriteErr)
        throw m_errWrite;
}


// ---------------------------------------------------------------------------
//  TCIDDAERipper: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This method is used to do stitching. We get a previous buffer, and a
//  current buffer. the current buffer is in the initial state, where the
//  bytes read reflects the full bytes read including any partial block and
//  overlap sectors.
//
//  What we have to do is find out where in the previous buffer the current
//  (raw, including overlap) buffer starts. We'll first do the optimisic
//  check that would happen if no error occurred. If that fails, we'll
//  do the exhaustive search.
//
//  Note that we overlap 2 sectors, but only ever check backwards of forwards
//  by 1, so we can always do full 2 sector compares in one direction and
//  1 plus a continually shrinking partial second one in the other.
//
tCIDLib::TBoolean
TCIDDAERipper::bCompareBlocks(const TDAEBuf& daebPrev, TDAEBuf& daebCur)
{
    //
    //  Set up a pointer in the previous buffer that is overlap bytes back
    //  from the end.
    //
    const tCIDLib::TCard1* pc1Prev
    (
        (daebPrev.m_pc1StartPtr + daebPrev.m_c4BytesRead)
        - CIDDAE_Ripper::c4OverlapBytes
    );

    // And get a pointer to the raw start of the new buffer
    const tCIDLib::TCard1* pc1New = daebCur.m_pc1Buf;

    //
    //  Do the optimistic comparison. If it works, then we move the start
    //  point up past the start of the buffer by overlap bytes, and remove
    //  the overlap bytes from the count, and we are done.
    //
    if (TRawMem::bCompareMemBuf(pc1Prev, pc1New, CIDDAE_Ripper::c4OverlapBytes))
    {
        daebCur.m_pc1StartPtr = daebCur.m_pc1Buf + CIDDAE_Ripper::c4OverlapBytes;
        daebCur.m_c4BytesRead -= CIDDAE_Ripper::c4OverlapBytes;
        return kCIDLib::True;
    }

    // There was a jitter error, so bump the stitch count
    m_c4Stitches++;

    //
    //  Oh well, didn't work. So let's start the search. First, we'll search
    //  one way up to 1 block. If that fails, then we'll start going the
    //  other way for up to 1 block.
    //
    //  So first we starting sliding the new buffer to the left (by moving
    //  the offset within the new buffer up one each time.
    //
    //  Note that we can start with 1, since the optimistic check above did
    //  zero.
    //
    pc1New++;
    for (tCIDLib::TCard4 c4Ofs = 1; c4Ofs < TKrnlRemMedia::c4CDRawSectorSz; c4Ofs++)
    {
        if (TRawMem::bCompareMemBuf(pc1Prev, pc1New, CIDDAE_Ripper::c4OverlapBytes))
        {
            //
            //  We got a match. This means that we we actually read earlier
            //  (before the requested start block), so there are now fewer
            //  bytes that available. So we lose the trailing sector and bump
            //  down the end block and bytes read.
            //
            daebCur.m_pc1StartPtr = daebCur.m_pc1Buf
                                    + (CIDDAE_Ripper::c4OverlapBytes + c4Ofs);
            daebCur.m_c4EndBlk--;
            daebCur.m_c4BytesRead = ((daebCur.m_c4EndBlk - daebCur.m_c4StartBlk)
                                     + 1) * TKrnlRemMedia::c4CDRawSectorSz;
            return kCIDLib::True;
        }

        //
        //  Move up the new buffer, so we are effectively shifting it left
        //  and sliding it further back into the previous buffer.
        //
        pc1New++;
    }

    //
    //  No luck there, so start the other direction. In this case, we start
    //  sliding the previous buffer to the left similar to above, but with
    //  the other buffer.
    //
    //  Note that we can start with 1, since the optimistic check above did
    //  zero.
    //
    pc1New = daebCur.m_pc1Buf;
    pc1Prev++;
    for (tCIDLib::TCard4 c4Ofs = 1; c4Ofs < TKrnlRemMedia::c4CDRawSectorSz; c4Ofs++)
    {
        //
        //  Here we have to compare fewer and fewer bytes, down to a single
        //  sector in the worst case, beause we started 2 sectors back from
        //  the end of the previous buffer, and are movign forward through it.
        //
        if (TRawMem::bCompareMemBuf(pc1Prev
                                    , pc1New
                                    , CIDDAE_Ripper::c4OverlapBytes - c4Ofs))
        {
            //
            //  We got a match. In this case, we read later (beyond the
            //  requested start point.) So we move up the start point c4Ofs
            //  fewer bytes. In this case, we don't lose any blocks so
            //  we just need to subtract the overlap bytes from the bytes
            //  read.
            //
            daebCur.m_pc1StartPtr = daebCur.m_pc1Buf
                                    + (CIDDAE_Ripper::c4OverlapBytes - c4Ofs);
            daebCur.m_c4BytesRead -= CIDDAE_Ripper::c4OverlapBytes;
            return kCIDLib::True;
        }
        pc1Prev++;
    }

    // We failed to fix the error
    return kCIDLib::False;
}



//
//  The read thread object is started up on this method. This guy reads
//  chunks and posts them until it runs out of input, retrying if needed
//  and doing stitching as needed/asked for.
//
tCIDLib::EExitCodes TCIDDAERipper::eReadThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let the calling thread go
    thrThis.Sync();

    //
    //  Lower our priority, so that we and the encoder thread don't eat up
    //  the CPU on lower powered systems.
    //
    thrThis.SetPriority(tCIDLib::EPrioLevels::BelowNormal);

    //
    //  We want to do stitching here instead of pushing it onto the encoding
    //  thread, which is already doing most of the work in some cases. This
    //  way, we get more overlapped and take better advantage of multi-CPU
    //  systems.
    //
    //  So, we always lag by one. When we get the second buffer, we do the
    //  stitching, and then post the first one, and make the new one the
    //  first one, and keep jumping forward till the last one.
    //
    //  NOTE that when we get a chunk buffer, the pool we get it out of
    //  calls our reset call. That will set the start pointer to be equal
    //  to the buffer origin. So unless we are doing jitter, there's no
    //  need to worry about setting the start pointer.
    //
    TDAEBuf* pdaebPrev = nullptr;
    TDAEBuf* pdaebCur = nullptr;
    try
    {
        //
        //  Prime the pump by setting up an initial previous block.
        //  So we get a chunk with no previous block passed.
        //
        pdaebPrev = pdaebGetChunk(0);

        // Loop while we aren't forced to break or asked to stop
        while(!(*m_pbBreakFlag))
        {
            // Do a check for a non-break shutdown request
            if (thrThis.bCheckShutdownRequest())
                break;

            // Get our current chunk
            pdaebCur = pdaebGetChunk(pdaebPrev);

            //
            //  If we get back a null pointer, there's no more, so we just
            //  post the previous buffer (if any), set the read complete
            //  flag, and break out.
            //
            if (!pdaebCur)
            {
                if (pdaebPrev)
                {
                    m_colWorkQ.Put(pdaebPrev);
                    pdaebPrev = nullptr;
                }
                m_bReadComplete = kCIDLib::True;
                break;
            }

            //
            //  We can post the previous buffer now and move forward. We
            //  have to throttle a bit if the queue has the max number of
            //  read ahead buffers - 2 in it (we have two of them here so
            //  if there are max-2 in the queue, all the pool buffers are
            //  used.
            //
            while (!(*m_pbBreakFlag))
            {
                if (m_colWorkQ.c4ElemCount() < CIDDAE_Ripper::c4MaxBufs - 2)
                {
                    // There's space so put this one in
                    m_colWorkQ.Put(pdaebPrev);

                    // Now swap the current buffer to be the old one
                    pdaebPrev = pdaebCur;
                    pdaebCur = nullptr;
                    break;
                }
                 else
                {
                    // Wait a while to see if some space becomes available
                    m_colWorkQ.bWaitForSpace(100, CIDDAE_Ripper::c4MaxBufs - 2);
                }
            }
        }
    }

    catch(TError& errToCatch)
    {
        // Indicate a read thread error
        m_bReadErr = kCIDLib::True;
        m_errRead = errToCatch;

        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        // Force the other thread down
        *m_pbBreakFlag = kCIDLib::True;
    }

    catch(...)
    {
        // Indicate a read thread error
        m_bReadErr = kCIDLib::True;
        m_errRead = TLogEvent
        (
            facCIDDAE().strName()
            , CID_FILE
            , CID_LINE
            , kDAEErrs::errcData_Unknown
            , facCIDDAE().strMsg(kDAEErrs::errcData_Unknown)
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
        );

        // Force the other thread down
        *m_pbBreakFlag = kCIDLib::True;
    }
    return tCIDLib::EExitCodes::Normal;
}


// The write thread object is started up on this method
tCIDLib::EExitCodes TCIDDAERipper::eWriteThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let the calling thread go
    thrThis.Sync();

    //
    //  Lower our priority, so that we and the reader thread don't eat up
    //  the CPU on lower powered systems.
    //
    thrThis.SetPriority(tCIDLib::EPrioLevels::BelowNormal);

    //
    //  Calculate the number of blocks that will be ripped, so that we can
    //  do percent done calculations as we go.
    //
    const tCIDLib::TFloat8 f8Total = (m_c4EndBlk - m_c4StartBlk) + 1;

    TDAEBuf* pdaebCur = nullptr;
    try
    {
        //
        //  Set up the encoder for the passed track name. This will open the
        //  target file, and will do a prep call to the encoder to let him set
        //  up for a new encoding sessions.
        //
        m_pdaeeToUse->StartEncode
        (
            m_strOutFile
            , m_strCodecName
            , m_strFmtName
            , m_c4TrackNum
            , m_strAlbumTitle
            , m_strTrackTitle
            , m_strArtist
            , m_strLabel
            , m_strGenre
            , m_c4Year
        );

        // Loop until we are broken, or asked to stop
        while(!(*m_pbBreakFlag))
        {
            // Do a check for a non-break shutdown request
            if (thrThis.bCheckShutdownRequest())
                break;

            //
            //  Wait a bit for new data to show up in the queue. If we get
            //  some, then process it. Tell it not to throw if nothing shows
            //  up. We want to wake up periodically and check for shutdown
            //  requests.
            //
            pdaebCur = m_colWorkQ.pobjGetNext(250, kCIDLib::False);
            if (!pdaebCur)
            {
                //
                //  There's nothing to do. If the read complete flag is set,
                //  then we are done. Else go back and get another.
                //
                if (m_bReadComplete)
                    break;
                continue;
            }
            TFixedSizePoolJan<TDAEBuf> janBuf(m_psplBuffers, pdaebCur);

            // Ok, we got one, so let the encoder encode it
            m_pdaeeToUse->StoreChunk(pdaebCur->m_pc1StartPtr, pdaebCur->m_c4BytesRead);

            // Update the percent done
            m_c4PercentDone = tCIDLib::TCard4
            (
                (((pdaebCur->m_c4EndBlk - m_c4StartBlk) + 1) / f8Total)
                * 100.0
            );

            // The janitor will release it, we just need to clear our pointer
            pdaebCur = nullptr;
        }

        // Do end encode and cleanup calls on the encoder
        m_pdaeeToUse->EndEncode();
        m_pdaeeToUse->Cleanup();
    }

    catch(TError& errToCatch)
    {
        // Force the other thread down
        *m_pbBreakFlag = kCIDLib::True;

        m_bWriteErr = kCIDLib::True;
        m_errWrite = errToCatch;

        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }

    catch(...)
    {
        // Force the other thread down
        *m_pbBreakFlag = kCIDLib::True;

        m_bWriteErr = kCIDLib::True;
        m_errWrite = TLogEvent
        (
            facCIDDAE().strName()
            , CID_FILE
            , CID_LINE
            , kDAEErrs::errcData_Unknown
            , facCIDDAE().strMsg(kDAEErrs::errcData_Unknown)
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Unknown
        );
    }
    return tCIDLib::EExitCodes::Normal;
}


//
//  This method reads chunks from the CD. This is called by the read thread,
//  to buffer up chunks for the write thread to deal with.
//
//  This guy also handles jitter if that is enabled. We get a pointer to the
//  previous buffer (if any, will be zero on the first read) so we can handle
//  finding overlap.
//
//  The next block to read is based on the previous block. If the incoming
//  is null, we start at the start block.
//
//  We return a null pointer when the end is reached, which will tell the
//  read thread he's done and he'll exit.
//
TDAEBuf* TCIDDAERipper::pdaebGetChunk(const TDAEBuf* const pdaebPrev)
{
    //
    //  If we are at the end, then done so return null pointer. This
    //  will cause the read thread to exit.
    //
    if (pdaebPrev && (pdaebPrev->m_c4EndBlk >= m_c4EndBlk))
        return nullptr;

    //
    //  Get a buffer out of the pool. The size value doesn't matter since
    //  they are all the same and our pool ignores it.
    //
    TDAEBuf* pdaebNew = m_psplBuffers->pobjReserveElem();
    if (!pdaebNew)
        return nullptr;

    TFixedSizePoolJan<TDAEBuf> janBuf(m_psplBuffers, pdaebNew);

    //
    //  Ok, start setting up this guy for the new read. We'll fill it
    //  in for the non-overlapped info. If we are doing an overlapped
    //  read, we'll read extra blocks below.
    //
    pdaebNew->m_c4StartBlk  = pdaebPrev ? (pdaebPrev->m_c4EndBlk + 1)
                              : m_c4StartBlk;

    //
    //  Calculate the number of blocks we can do. If we can do a full
    //  chunk, then do that. If not, then we are on the last chunk.
    //
    tCIDLib::TCard4 c4BlksToRead = (m_c4EndBlk - pdaebNew->m_c4StartBlk) + 1;
    if (c4BlksToRead >= CIDDAE_Ripper::c4SectorsPerRead)
    {
        c4BlksToRead = CIDDAE_Ripper::c4SectorsPerRead;
    }

    // So set up the end block and bytes to read
    pdaebNew->m_c4EndBlk    = pdaebNew->m_c4StartBlk + (c4BlksToRead - 1);
    pdaebNew->m_c4BytesRead = c4BlksToRead * TKrnlRemMedia::c4CDRawSectorSz;

    //
    //  If we are doing jitter, and this isn't the first or last chunk, then
    //  add the jitter blocks.
    //
    tCIDLib::TCard4 c4ActualStart = pdaebNew->m_c4StartBlk;
    if ((m_eJitterOpt != tCIDDAE::EJitterOpts::None)
    &&  (pdaebNew->m_c4StartBlk != m_c4StartBlk)
    &&  (pdaebNew->m_c4EndBlk != m_c4EndBlk))
    {
        c4BlksToRead += CIDDAE_Ripper::c4OverlapBlocks;
        c4ActualStart -= CIDDAE_Ripper::c4OverlapBlocks;
        pdaebNew->m_c4BytesRead += CIDDAE_Ripper::c4OverlapBytes;
    }

    //
    //  We pass in the max bytes we can handle, and get back out the bytes we
    //  actually got.
    //
    tCIDLib::TCard4 c4MaxToRead = CIDDAE_Ripper::c4BufSz;
    tCIDLib::TBoolean bRes = m_prmmdToUse->bReadSectors
    (
        c4ActualStart, c4BlksToRead, pdaebNew->m_pc1Buf, c4MaxToRead
    );

    //
    //  If we failed, or didn't get the requested bytes, try again. If we
    //  got an 'invalid function' error, assume that this is a bogus TOC
    //  that is reporting an invalid size as an attempt at copy protection,
    //  and just stop here.
    //
    if (!bRes || (c4MaxToRead != pdaebNew->m_c4BytesRead))
    {
        m_c4ReadErrs++;

        // We'll try it 4 times
        for (tCIDLib::TCard4 c4Retry = 0; c4Retry < 4; c4Retry++)
        {
            // Back off a little bit before we try again
            TThread::Sleep(25);

            c4MaxToRead = CIDDAE_Ripper::c4BufSz;
            bRes = m_prmmdToUse->bReadSectors
            (
                c4ActualStart
                , c4BlksToRead
                , pdaebNew->m_pc1Buf
                , c4MaxToRead
            );

            // If we got a good one, break out
            if (bRes && (c4MaxToRead == pdaebNew->m_c4BytesRead))
                break;
        }

        if (!bRes)
        {
            //
            //  Oh well, give up. If the error was an invalid function
            //  error, then assume that the size is purposefully mis-
            //  reported as a copy protection measure, and just stop here.
            //  Else, throw.
            //
            const TKrnlError& kerrLast = TKrnlError::kerrLast();
            if ((kerrLast.errcId() == kKrnlErrs::errcGen_NotSupported)
            &&  (kerrLast.errcHostId() == 1))
            {
                // Return without releasing the buffer so it gets returned to the pool
                return nullptr;
            }
             else
            {
                facCIDDAE().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kDAEErrs::errcData_CantReadBlks
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                    , TCardinal(pdaebNew->m_c4StartBlk)
                );
            }
        }
    }

    //
    //  If doing jitter corection and it's not the first block (no previous)
    //  or last block, then do the correction.
    //
    if ((m_eJitterOpt != tCIDDAE::EJitterOpts::None)
    &&  (pdaebNew->m_c4EndBlk != m_c4EndBlk)
    &&  pdaebPrev)
    {
        if (!bCompareBlocks(*pdaebPrev, *pdaebNew))
            m_c4StitchErrs++;

        //
        //  If this was a last, partial buffer, then we asked for an extra
        //  block, to allow for a jitter error. So, if we ended up with all
        //  the requested blocks, get rid of the last one.
        //
        if (pdaebNew->m_c4EndBlk > m_c4EndBlk)
        {
            pdaebNew->m_c4EndBlk--;
            pdaebNew->m_c4BytesRead -= TKrnlRemMedia::c4CDRawSectorSz;
        }
    }

    // Orphan the buffer out of the janitor and the caller is responsible
    return janBuf.pobjOrphan();
}

