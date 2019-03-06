//
// FILE NAME: CIDDAE_Ripper.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2006
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
RTTIDecls(TDAEBuf,TObject)
RTTIDecls(TCIDDAEEncoder,TObject)
RTTIDecls(TCIDDAERipper,TObject)

// Used to force on some jitter simulation code below
#define FAKEJITTER 1
#if defined(FAKEJITTER)
const tCIDLib::TInt4 i4FakeJitter = -512;
#endif


// ---------------------------------------------------------------------------
//  Local types and data
// ---------------------------------------------------------------------------
namespace CIDDAE_Ripper
{
    // -----------------------------------------------------------------------
    //  We need to be able to provide unique thread names for our read/write
    //  threads.
    // -----------------------------------------------------------------------
    TUniqueName unamInstance(L"CQCDAEThread%(1)");


    // -----------------------------------------------------------------------
    //  The maximum number of buffers we can have outstanding at once. This
    //  is how many buffers the buffer pool objects are set up for. The read
    //  thread will stop and wait for the write thread to catch up.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4MaxBufs(8);


    // -----------------------------------------------------------------------
    //  The number of sectors we read per pass. We may actually read more if
    //  jitter is enabled. The buffer size below is big enough to adjust for
    //  the jitter overlap.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4SectorsPerRead(22);


    // -----------------------------------------------------------------------
    //  The bytes we try to read per pass, and the size of the buffers. We
    //  make the buffer size larger by the jitter overlap we do.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4BytesPerRead(c4SectorsPerRead * TKrnlCDROM::c4RawSectorSz);
    const tCIDLib::TCard4   c4BufSz((c4SectorsPerRead + 3) * TKrnlCDROM::c4RawSectorSz);


    // -----------------------------------------------------------------------
    //  The bytes in our overlap area.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4 c4OverlapBytes(TKrnlCDROM::c4RawSectorSz * 2);


    // -----------------------------------------------------------------------
    //  The number of blocks we overlap during jitter correction mode.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4OverlapBlocks(2);
}



// ---------------------------------------------------------------------------
//  Local helper functions
// ---------------------------------------------------------------------------

//
//  The pool class requires a reset function pointer that it calls with each
//  object that it is about to give out from the pool.
//
static tCIDLib::TVoid DAEBufReset(TDAEBuf& daebToReset)
{
    // If this guy's buffer hasn't been allocated yet, then do it.
    if (!daebToReset.m_pc1Buf)
    {
        //
        //  If fake jitter is on, then we allocate a sector before and after
        //  the actual buffer.
        //
        #if defined(FAKEJITTER)
        daebToReset.m_pc1Buf
            = new tCIDLib::TCard1[CIDDAE_Ripper::c4BufSz + (TKrnlCDROM::c4RawSectorSz * 2)];
        daebToReset.m_pc1Buf += TKrnlCDROM::c4RawSectorSz;
        #else
        daebToReset.m_pc1Buf = new tCIDLib::TCard1[CIDDAE_Ripper::c4BufSz];
        #endif
    }

    daebToReset.m_c4BytesRead = 0;
    daebToReset.m_c4PoolCookie = kCIDLib::c4MaxCard;
    daebToReset.m_c4EndBlk = 0;
    daebToReset.m_c4StartBlk = 0;
    daebToReset.m_pc1StartPtr = daebToReset.m_pc1Buf;
}



// ---------------------------------------------------------------------------
//  CLASS: TDAEBuf
// PREFIX: daeb
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDAEBuf: Constructors and Destructor
// ---------------------------------------------------------------------------
TDAEBuf::TDAEBuf() :

    m_c4BytesRead(0)
    , m_c4EndBlk(0)
    , m_c4PoolCookie(0)
    , m_c4StartBlk(0)
    , m_pc1Buf(0)
    , m_pc1StartPtr(0)
{
}

TDAEBuf::~TDAEBuf()
{
    // Clean up the buffer
    if (m_pc1Buf)
    {
        #if defined(FAKEJITTER)
        delete [] (m_pc1Buf - TKrnlCDROM::c4RawSectorSz);
        #else
        delete [] m_pc1Buf;
        #endif
    }
}




// ---------------------------------------------------------------------------
//  CLASS: TCIDDAEEncoder
// PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDDAEEncoder: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDDAEEncoder::TCIDDAEEncoder()
{
}

TCIDDAEEncoder::~TCIDDAEEncoder()
{
    //
    //  If they didn't close the file, close it now. Do it without a cleanup
    //  call! That's a virtual and their version wouldn't get called at this
    //  point.
    //
    if (m_flTarget.bIsOpen())
        m_flTarget.Close();
}


// ---------------------------------------------------------------------------
//  TCIDDAEEncoder: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDDAEEncoder::Close()
{
    // Give the derived class a chance to clean up
    Cleanup();

    // And now we can close the file
    m_flTarget.Close();
}


tCIDLib::TVoid TCIDDAEEncoder::OpenTarget(const TString& strPath)
{
    //
    //  Try to open the target file. We assume they want to overwrite any
    //  existing file and the derived encoder will need read/write access.
    //
    m_flTarget.strName(strPath);
    m_flTarget.Open
    (
        tCIDLib::EAccessModes::Excl_ReadWrite
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::None
    );

    //
    //  Call the prep method to allow the derived class to do any prep work
    //  before encoding starts.
    //
    DoPrep();
}


// ---------------------------------------------------------------------------
//  TCIDDAEEncoder: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TCIDDAEEncoder::ReadBytes(  const   tCIDLib::TCard4 c4Count
                            ,       TMemBuf&        mbufToFill)
{
    if (m_flTarget.c4ReadBuffer(mbufToFill, c4Count) != c4Count)
    {
        facCIDDAE().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDAEErrs::errcEnc_ReadErr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllRead
            , TCardinal(c4Count)
        );
    }
}


tCIDLib::TVoid TCIDDAEEncoder::SeekTo(const tCIDLib::TCard4 c4At)
{
    tCIDLib::TCard8 c8Pos = c4At;
    m_flTarget.SetFilePos(c8Pos);
}


tCIDLib::TVoid
TCIDDAEEncoder::WriteBytes( const   tCIDLib::TCard4 c4Count
                            , const TMemBuf&        mbufToWrite)
{
    if (m_flTarget.c4WriteBuffer(mbufToWrite, c4Count) != c4Count)
    {
        facCIDDAE().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDAEErrs::errcEnc_WriteErr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllWritten
            , TCardinal(c4Count)
        );
    }
}

tCIDLib::TVoid
TCIDDAEEncoder::WriteBytes( const   tCIDLib::TCard4         c4Count
                            , const tCIDLib::TVoid* const   pToWrite)
{
    if (m_flTarget.c4WriteBuffer(pToWrite, c4Count) != c4Count)
    {
        facCIDDAE().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDAEErrs::errcEnc_WriteErr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllWritten
            , TCardinal(c4Count)
        );
    }
}




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
    , m_c4PercentDone(0)
    , m_c4ReadErrs(0)
    , m_c4StitchErrs(0)
    , m_c4Stitches(0)
    , m_c4StartBlk(0)
    , m_colWorkQ(tCIDLib::EAdoptOpts::NoAdopt, tCIDLib::EMTStates::Safe)
    , m_eJitterOpt(tCIDDAE::EJitterOpts::None)
    , m_pbBreakFlag(0)
    , m_pcdriToUse(0)
    , m_pdaeeToUse(0)
    , m_splBuffers(CIDDAE_Ripper::c4MaxBufs, &DAEBufReset, tCIDLib::EMTStates::Safe)
    , m_thrRead
      (
        CIDDAE_Ripper::unamInstance.strQueryNewName()
        , TMemberFunc<TCIDDAERipper>(this, &TCIDDAERipper::eReadThread)
      )
    , m_thrWrite
      (
        CIDDAE_Ripper::unamInstance.strQueryNewName()
        , TMemberFunc<TCIDDAERipper>(this, &TCIDDAERipper::eWriteThread)
      )
{
}

TCIDDAERipper::~TCIDDAERipper()
{
    // Release all the pool objects
    try
    {
        m_splBuffers.Flush();
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
                , tCIDLib::ESeverities::Fail
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
//  track to a target file.
//
tCIDLib::TVoid
TCIDDAERipper::RipTrack(        TKrnlCDROMInfo&         cdriToUse
                        , const tCIDLib::TCard4         c4TrackNum
                        ,       TCIDDAEEncoder&         daeeToUse
                        , const TString&                strTrackName
                        ,       tCIDLib::TBoolean&      bBreakFlag)
{
    //
    //  We need to store some pointers to parms for the scope of this call,
    //  so we use janitors to do that.
    //
    TGFJanitor<TKrnlCDROMInfo*> janCD(&m_pcdriToUse, &cdriToUse);
    TGFJanitor<TCIDDAEEncoder*> janEncoder(&m_pdaeeToUse, &daeeToUse);
    TGFJanitor<tCIDLib::TBoolean*> janBreak(&m_pbBreakFlag, &bBreakFlag);

    // Query the table of contents info from the CD
    if (!cdriToUse.bQueryTOC(m_TOCData))
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
    const tCIDLib::TBoolean bRes = TKrnlCDROM::bExtractTrackInfo
    (
        m_TOCData
        , c4TrackNum
        , m_c4StartBlk
        , m_c4BlkCount
        , c4Minutes
        , c4Seconds
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
    //  Set up the encoder for the passed track name. This will open the
    //  target file, and will do a prep call to the encoder to let him set
    //  up for a new encoding sessions.
    //
    daeeToUse.OpenTarget(strTrackName);

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
    //  pool objects. If the write thread go stopped with items in the
    //  queue, they will still be marked reseroved.
    //
    m_splBuffers.ReleaseAll();

    // Flush the work queue for the same reason
    m_colWorkQ.RemoveAll();

    // Now start up the two threads
    m_thrRead.Start();
    m_thrWrite.Start();

    //
    //  The read thread drives the process, and will get to the end first.
    //  So we block first on it. Once we see it end, then we wait for the
    //  write thread to end.
    //
    TThread* pthrUs = TThread::pthrCaller();
    tCIDLib::EExitCodes eExit;
    while (kCIDLib::True)
    {
        // Wait a while for it to end
        if (m_thrRead.bWaitForDeath(eExit, 1000))
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
    if (!m_thrRead.bWaitForDeath(eExit, 5000))
    {
        // It didn't exit, so try to make it exit
        try
        {
            m_thrRead.ReqShutdownSync(5000);
            m_thrRead.eWaitForDeath(5000);
        }

        catch(const TError& errToCatch)
        {
            m_errRead = errToCatch;
            m_bReadErr = kCIDLib::True;
        }
    }

    if (!m_thrWrite.bWaitForDeath(eExit, 5000))
    {
        // It didn't exit, so try to make it exit
        try
        {
            m_thrWrite.ReqShutdownSync(5000);
            m_thrWrite.eWaitForDeath(5000);
        }

        catch(const TError& errToCatch)
        {
            m_errWrite = errToCatch;
            m_bWriteErr = kCIDLib::True;
        }
    }

    // Do a close on the encoder, which will give the encoder time to clean up
    daeeToUse.Close();

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
    for (tCIDLib::TCard4 c4Ofs = 1; c4Ofs < TKrnlCDROM::c4RawSectorSz; c4Ofs++)
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
                                     + 1) * TKrnlCDROM::c4RawSectorSz;
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
    for (tCIDLib::TCard4 c4Ofs = 1; c4Ofs < TKrnlCDROM::c4RawSectorSz; c4Ofs++)
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
    TDAEBuf* pdaebPrev = 0;
    TDAEBuf* pdaebCur = 0;
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
                    pdaebPrev = 0;
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
            while (kCIDLib::True)
            {
                if (m_colWorkQ.c4ElemCount() < CIDDAE_Ripper::c4MaxBufs - 2)
                {
                    // There's space so put this one in
                    m_colWorkQ.Put(pdaebPrev);

                    // Now swap the current buffer to be the old one
                    pdaebPrev = pdaebCur;
                    pdaebCur = 0;
                    break;
                }

                if (!thrThis.bSleep(5))
                    break;
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

    // Release any buffers we have back to the pool
    if (pdaebPrev)
        m_splBuffers.ReleaseElem(pdaebPrev, pdaebPrev->m_c4PoolCookie);
    if (pdaebCur)
        m_splBuffers.ReleaseElem(pdaebCur, pdaebPrev->m_c4PoolCookie);

    return tCIDLib::EExitCodes::Normal;
}


// The write thread object is started up on this method
tCIDLib::EExitCodes TCIDDAERipper::eWriteThread(TThread& thrThis, tCIDLib::TVoid*)
{
    // Let the calling thread go
    thrThis.Sync();

    //
    //  Calculate the number of blocks that will be ripped, so that we can
    //  do percent done calculations as we go.
    //
    const tCIDLib::TFloat8 f8Total = (m_c4EndBlk - m_c4StartBlk) + 1;

    TDAEBuf* pdaebCur = 0;
    try
    {
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

            // Ok, we got one, so let the encoder encode it
            m_pdaeeToUse->StoreChunk(pdaebCur->m_pc1StartPtr, pdaebCur->m_c4BytesRead);

            // Update the percent done
            m_c4PercentDone = tCIDLib::TCard4
            (
                (((pdaebCur->m_c4EndBlk - m_c4StartBlk) + 1) / f8Total)
                * 100.0
            );

            // Release the buffer back to the pool now
            m_splBuffers.ReleaseElem(pdaebCur, pdaebCur->m_c4PoolCookie);
            pdaebCur = 0;
        }
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

    // Release any final buffer we have
    if (pdaebCur)
        m_splBuffers.ReleaseElem(pdaebCur, pdaebCur->m_c4PoolCookie);

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
TDAEBuf* TCIDDAERipper::pdaebGetChunk(TDAEBuf* const pdaebPrev)
{
    //
    //  If we are at the end, then done so return null pointer. This
    //  will cause the read thread to exit.
    //
    if (pdaebPrev && (pdaebPrev->m_c4EndBlk >= m_c4EndBlk))
        return 0;

    //
    //  Get a buffer out of the pool. We put the pool cookie in the
    //  buffer object so that it can be correctly released later.
    //
    tCIDLib::TCard4 c4Cookie;
    TDAEBuf* pdaebNew = m_splBuffers.pobjReserveElem(c4Cookie);
    pdaebNew->m_c4PoolCookie = c4Cookie;

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
     else
    {
        //
        //  Ask for an extra block so that, if we have a jitter error, we can
        //  still get the last black. Else, we can just get stuck forever.
        //  We know 1 more will fit since we have fewer than the sectors per
        //  read. We'll know below to get rid of the extra sector if it
        //  survives, because our chunk end will be one past the real end.
        //
        if (m_eJitterOpt != tCIDDAE::EJitterOpts::None)
            c4BlksToRead++;
    }

    // So set up the end block and bytes to read
    pdaebNew->m_c4EndBlk    = pdaebNew->m_c4StartBlk + (c4BlksToRead - 1);
    pdaebNew->m_c4BytesRead = c4BlksToRead * TKrnlCDROM::c4RawSectorSz;

    //
    //  If we are doing jitter, and this isn't the first chunk, then add the
    //  jitter blocks.
    //
    tCIDLib::TCard4 c4ActualStart = pdaebNew->m_c4StartBlk;
    if ((m_eJitterOpt != tCIDDAE::EJitterOpts::None)
    &&  (pdaebNew->m_c4StartBlk != m_c4StartBlk))
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
    tCIDLib::TCard1* pc1ToFill = pdaebNew->m_pc1Buf;

#if defined(FAKEJITTER)
if (pdaebPrev && (m_eJitterOpt != tCIDDAE::EJitterOpts::None))
{
    // And adjust the read pointer to force the misalignment
    c4BlksToRead++;
    if (i4FakeJitter >= 0)
    {
        c4ActualStart--;
        pc1ToFill -= TKrnlCDROM::c4RawSectorSz;
        pc1ToFill += i4FakeJitter;
    }
     else
    {
        pc1ToFill += i4FakeJitter;
    }
    c4MaxToRead += TKrnlCDROM::c4RawSectorSz;
}
#endif

    tCIDLib::TBoolean bRes = m_pcdriToUse->bReadSectors
    (
        c4ActualStart
        , c4BlksToRead
        , pc1ToFill
        , c4MaxToRead
    );

#if defined(FAKEJITTER)
// Hide the extra sector we asked for
if (pdaebPrev && (m_eJitterOpt != tCIDDAE::EJitterOpts::None))
{
    c4BlksToRead--;
    c4MaxToRead -= TKrnlCDROM::c4RawSectorSz;
}
#endif

    // If we failed, or didn't get the reuqested bytes, try again
    if (!bRes || (c4MaxToRead != pdaebNew->m_c4BytesRead))
    {
        m_c4ReadErrs++;

        // We'll try it 3 times
        for (tCIDLib::TCard4 c4Retry = 0; c4Retry < 3; c4Retry++)
        {
            // Back off a little bit before we try again
            TThread::Sleep(25);

            c4MaxToRead = CIDDAE_Ripper::c4BufSz;
            bRes = m_pcdriToUse->bReadSectors
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
            // Oh well, give up. Release the buffer back to the pool
            m_splBuffers.ReleaseElem(pdaebNew, c4Cookie);
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

    // If doing jitter corection and we have a previous buffer, then do it
    if ((m_eJitterOpt != tCIDDAE::EJitterOpts::None) && pdaebPrev)
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
            pdaebNew->m_c4BytesRead -= TKrnlCDROM::c4RawSectorSz;
        }
    }
    return pdaebNew;
}

