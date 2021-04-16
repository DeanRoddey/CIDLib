//
// FILE NAME: CIDLib_DataSrc.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/26/2014
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
//  This file implements the TCIDDataSrc class, which is an abstract data source
//  wrapper from which various derivatives can be created.
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
#include    "CIDLib_.hpp"



// ---------------------------------------------------------------------------
//   CLASS: TCIDDataSrc
//  PREFIX: cns
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDDataSrc: Destructor
// ---------------------------------------------------------------------------
TCIDDataSrc::~TCIDDataSrc()
{
    if (m_bInitialized)
    {
        //
        //  They didn't terminate it. Nothing we can do now since it's a virtual
        //  method. Any underlying comm resource will be released, it just means
        //  that any stuff (like secure channel) that includes a formal termination
        //  won't get done.
        //
        if (facCIDLib().bTestLog(tCIDLib::ESeverities::Warn, tCIDLib::ELogFlags::DataSrc))
        {
            facCIDLib().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcDSrc_NotTerminated
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::AppError
            );
        }
    }
}


// ---------------------------------------------------------------------------
//  TCIDDataSrc: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Wait for up to a given time for data to be available
tCIDLib::TBoolean TCIDDataSrc::bDataAvailMS(const tCIDLib::TCard4 c4WaitMSs)
{
    // If we have data now, then we are done
    if (m_c4InBufIndex < m_c4InBufCount)
        return kCIDLib::True;

    // Else try to do a cache load
    ReloadInCache(TTime::enctNowPlusMSs(c4WaitMSs));

    // And test it again
    return (m_c4InBufCount != 0);
}

tCIDLib::TBoolean TCIDDataSrc::bDataAvail(const tCIDLib::TEncodedTime enctEnd)
{
    // If we have data now, then we are done
    if (m_c4InBufIndex < m_c4InBufCount)
        return kCIDLib::True;

    // Else try to do a cache load
    ReloadInCache(enctEnd);

    // And test it again
    return (m_c4InBufCount != 0);
}


//
//  Return whether we are initialized or not. Mostly for use by the data source janitor
//  class.
//
tCIDLib::TBoolean TCIDDataSrc::bIsInitialized() const
{
    return m_bInitialized;
}


// Read in a single byte. Return zero if we hit the end
tCIDLib::TCard1 TCIDDataSrc::c1ReadByte(const tCIDLib::TEncodedTime enctEnd)
{
    // If no cache data, then reload it first
    if (m_c4InBufIndex == m_c4InBufCount)
        ReloadInCache(enctEnd);

    // If none, then return zero now
    if (!m_c4InBufCount)
        return 0;

    // Return the next available character
    return m_mbufInData[m_c4InBufIndex++];
}


// Read in a buffer of data
tCIDLib::TCard4
TCIDDataSrc::c4ReadBytes(       TMemBuf&                mbufTar
                        , const tCIDLib::TCard4         c4MaxBytes
                        , const tCIDLib::TEncodedTime   enctEnd
                        , const tCIDLib::TBoolean       bReadAll)
{
    tCIDLib::TCard4 c4RetCnt = 0;
    while (c4RetCnt < c4MaxBytes)
    {
        // If the cache is empty, reload it
        if (m_c4InBufIndex == m_c4InBufCount)
        {
           ReloadInCache(enctEnd);

            // If we didn't get anything, then we are done
            if (!m_c4InBufCount)
                break;
        }

        // Get what we can from the cache
        const tCIDLib::TCard4 c4Avail = m_c4InBufCount - m_c4InBufIndex;
        const tCIDLib::TCard4 c4Needed = c4MaxBytes - c4RetCnt;

        // See how much we need/can get this time
        tCIDLib::TCard4 c4ThisTime = c4Needed;
        if (c4Avail < c4Needed)
            c4ThisTime = c4Avail;

        // Copy those bytes into our target buffer
        mbufTar.CopyIn(m_mbufInData.pc1DataAt(m_c4InBufIndex), c4ThisTime, c4RetCnt);

        // Add this amount to the buffer index and our count
        m_c4InBufIndex += c4ThisTime;
        c4RetCnt += c4ThisTime;

        // If not reading all, then just return whatever we got on the first round
        if (!bReadAll)
            break;
    }

    // If reading all and we didn't get them, then it's an error
    if (bReadAll && (c4RetCnt != c4MaxBytes))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcDSrc_Timeout
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Timeout
            , TCardinal(c4MaxBytes)
            , TCardinal(c4RetCnt)
        );
    }

    return c4RetCnt;
}


tCIDLib::TCard4
TCIDDataSrc::c4ReadBytes(       tCIDLib::TVoid* const   pToFill
                        , const tCIDLib::TCard4         c4MaxBytes
                        , const tCIDLib::TEncodedTime   enctEnd
                        , const tCIDLib::TBoolean       bReadAll)
{
    // Look at the buffer as byte array internally
    tCIDLib::TCard1* pc1Tar = static_cast<tCIDLib::TCard1*>(pToFill);

    tCIDLib::TCard4 c4RetCnt = 0;
    while (c4RetCnt < c4MaxBytes)
    {
        // If the cache is empty, reload it
        if (m_c4InBufIndex == m_c4InBufCount)
        {
           ReloadInCache(enctEnd);

            // If we didn't get anything, then we are done
            if (!m_c4InBufCount)
                break;
        }

        // Get what we can from the cache
        const tCIDLib::TCard4 c4Avail = m_c4InBufCount - m_c4InBufIndex;
        const tCIDLib::TCard4 c4Needed = c4MaxBytes - c4RetCnt;

        // See how much we need/can get this time
        tCIDLib::TCard4 c4ThisTime = c4Needed;
        if (c4Avail < c4Needed)
            c4ThisTime = c4Avail;

        // Copy those bytes out to the target
        m_mbufInData.CopyOut(&pc1Tar[c4RetCnt], c4ThisTime, m_c4InBufIndex);

        // Add this amount to the buffer index and our count
        m_c4InBufIndex += c4ThisTime;
        c4RetCnt += c4ThisTime;

        // If not reading all, then just return whatever we got on the first round
        if (!bReadAll)
            break;
    }

    // If reading all and we didn't get them, then it's an error
    if (bReadAll && (c4RetCnt != c4MaxBytes))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcDSrc_Timeout
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Timeout
            , TCardinal(c4MaxBytes)
            , TCardinal(c4RetCnt)
        );
    }
    return c4RetCnt;
}


tCIDLib::ELineRes
TCIDDataSrc::eGetLine(TString& strToFill, const tCIDLib::TEncodedTime enctEnd)
{
    strToFill.Clear();

    // If we never set the return to anything else, then assume a timeout
    tCIDLib::ELineRes eRet = tCIDLib::ELineRes::Timeout;
    TThread* pthrCaller = 0;
    try
    {
        tCIDLib::TEncodedTime enctCur = TTime::enctNow();
        while (enctCur < enctEnd)
        {
            // If no bytes in the cache, then try to get some more
            if (m_c4InBufIndex == m_c4InBufCount)
            {
                // If asked to stop, then just fake a timeout
                if (!pthrCaller)
                    pthrCaller = TThread::pthrCaller();
                if (pthrCaller->bCheckShutdownRequest())
                    return tCIDLib::ELineRes::Timeout;

                // Reset our index and count
                ReloadInCache(enctEnd);

                // If no more, break out
                if (!m_c4InBufCount)
                    break;

                //
                //  Update our current time. we only do it inside here since, other
                //  wise we are just pulling data form the buffer and it will take no
                //  appreciable time. If we get the EOL, we'll break out out there,
                //  else we have to come bakc in here to get more data.
                //
                enctCur = TTime::enctNow();
            }

            // Get the next byte from the spool buffer
            tCIDLib::TCard1 c1Cur = m_mbufInData[m_c4InBufIndex++];

            //
            //  If we get a CR, then ignore it, and wait for the LF. If we get
            //  the LF, then we have a line. Else, just take it as the next
            //  line char.
            //
            if ((c1Cur == 0x0D) || (c1Cur == 0x0A))
            {
                // Break out on an LF
                if (c1Cur == 0x0A)
                {
                    eRet = tCIDLib::ELineRes::GotLine;
                    break;
                }
            }
             else
            {
                strToFill.Append(tCIDLib::TCh(c1Cur));
            }
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // If we saw an endline, but no content, return empty line
    if ((eRet == tCIDLib::ELineRes::GotLine) && strToFill.bIsEmpty())
        eRet = tCIDLib::ELineRes::EmptyLine;

    // If we are still at timeout, but have some content, it's a partial line
    if ((eRet == tCIDLib::ELineRes::Timeout) && !strToFill.bIsEmpty())
        eRet = tCIDLib::ELineRes::Partial;

    return eRet;
}


//
//  Similar to above but uses a text converter. As a practical matter we have to
//  transcode one char at a time.
//
tCIDLib::ELineRes
TCIDDataSrc::eGetLine(          TString&                strToFill
                        , const tCIDLib::TEncodedTime   enctEnd
                        ,       TTextConverter&         tcvtToUse)
{
    strToFill.Clear();

    // If we never set the return to anything else, then assume a timeout
    tCIDLib::ELineRes eRet = tCIDLib::ELineRes::Timeout;
    TThread* pthrCaller = nullptr;
    try
    {
        tCIDLib::TCard4 c4Eaten = 0;
        tCIDLib::TEncodedTime enctCur = TTime::enctNow();
        while (enctCur < enctEnd)
        {
            // If no bytes in the cache, then try to get some more
            if (m_c4InBufIndex == m_c4InBufCount)
            {
                // If asked to stop, then just fake a timeout
                if (!pthrCaller)
                    pthrCaller = TThread::pthrCaller();
                if (pthrCaller->bCheckShutdownRequest())
                    return tCIDLib::ELineRes::Timeout;

                // Reset our index and count
                ReloadInCache(enctEnd);

                // If no more, break out
                if (!m_c4InBufCount)
                    break;

                //
                //  Update our current time. we only do it inside here since, other
                //  wise we are just pulling data form the buffer and it will take no
                //  appreciable time. If we get the EOL, we'll break out out there,
                //  else we have to come bakc in here to get more data.
                //
                enctCur = TTime::enctNow();
            }

            // Transcode one char from the source buffer
            const tCIDLib::TCh chCur = tcvtToUse.chConvertOneFrom
            (
                m_mbufInData.pc1DataAt(m_c4InBufIndex)
                , m_c4InBufCount - m_c4InBufIndex
                , c4Eaten
            );

            //
            //  If any bytes were eaten, then we got a chracter, so move up. If
            //  not, then we have the annoying problem of a partial char, which we
            //  can't consume, and we cannot call ReloadInCache unless the cache is
            //  empty, which it's not.
            //
            if (c4Eaten)
            {
                m_c4InBufIndex += c4Eaten;

                //
                //  If we get a CR, then ignore it, and wait for the LF. If we get
                //  the LF, then we have a line. Else, just take it as the next
                //  line char.
                //
                if ((chCur == 0x0D) || (chCur == 0x0A))
                {
                    // Break out on an LF
                    if (chCur == 0x0A)
                    {
                        eRet = tCIDLib::ELineRes::GotLine;
                        break;
                    }
                }
                 else
                {
                    strToFill.Append(chCur);
                }
            }
             else
            {
                //
                //  Try a reload, which won't happen above because the buffer isn't
                //  empty. Check fora shutdown request first.
                //
                if (!pthrCaller)
                    pthrCaller = TThread::pthrCaller();
                if (pthrCaller->bCheckShutdownRequest())
                    return tCIDLib::ELineRes::Timeout;

                ReloadInCache(enctCur);
            }
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // If we saw an endline, but no content, return empty line
    if ((eRet == tCIDLib::ELineRes::GotLine) && strToFill.bIsEmpty())
        eRet = tCIDLib::ELineRes::EmptyLine;

    // If we are still at timeout, but have some content, it's a partial line
    if ((eRet == tCIDLib::ELineRes::Timeout) && !strToFill.bIsEmpty())
        eRet = tCIDLib::ELineRes::Partial;

    return eRet;
}



//
//  This is where any sending of data is done. We buffer output data until this is
//  called, then we send it. This allows us to be more efficient.
//
//  We must be initialized by now or an exception is thrown
//
tCIDLib::TVoid TCIDDataSrc::FlushOut(const tCIDLib::TEncodedTime enctEnd)
{
    if (!m_bInitialized)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcDSrc_NotInitilized
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }

    // If there's any data, then send it and zero out the counter again
    if (m_c4OutBufCount)
    {
        PutData(m_mbufOutData, m_c4OutBufCount, enctEnd);
        m_c4OutBufCount = 0;
    }
}


//
//  This must be called before the source can be used. We call a protected virtual to
//  let the derived class do any required init. If that works, we set the init flag.
//
tCIDLib::TVoid TCIDDataSrc::Initialize(const tCIDLib::TEncodedTime enctEnd)
{
    // Cannot be already initialized
    if (m_bInitialized)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcDSrc_AlreadyInit
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }

    SetupSrc(enctEnd);
    m_bInitialized = kCIDLib::True;
}


//
//  Sometimes, if the client code must do one type of exchange then another, it's
//  possible they may read more than they need, and the remainder really must be seen
//  as input later in the process. So we allow them to push back data.
//
//  This is a little tricky since our input buffer may already have some data that's
//  been read from it. So we have to put this new stuff into the input buffer first,
//  then move the remaining original input buffer content after this, resetting the
//  the input index to zero again.
//
tCIDLib::TVoid
TCIDDataSrc::PushBack(  const   tCIDLib::TVoid* const   pToPush
                        , const tCIDLib::TCard4         c4Bytes)
{
    //
    //  Watch for scenarios where we can optimize
    //
    //  1. If we are empty it's easy
    //  2. If (unlikely) the amount read so far is the size to push back.
    //  3. The amount to push is less than what we've read so far, so we can
    //      sneak it in before the unread stuff.
    //
    //  Else we have to do it the hard way.
    //
    if (m_c4InBufIndex == m_c4InBufCount)
    {
        m_mbufInData.CopyIn(pToPush, c4Bytes);
        m_c4InBufIndex = 0;
        m_c4InBufCount = c4Bytes;
    }
     else if (m_c4InBufIndex == c4Bytes)
    {
        //
        //  We can just copy the bytes into the start of the buffer and zero the
        //  index and increase the count.
        //
        m_mbufInData.CopyIn(pToPush, c4Bytes);
        m_c4InBufIndex = 0;
        m_c4InBufCount += c4Bytes;
    }
     else if (c4Bytes < m_c4InBufIndex)
    {
        m_c4InBufIndex -= c4Bytes;
        m_mbufInData.CopyIn(pToPush, c4Bytes, m_c4InBufIndex);
    }
     else
    {
        // Store away the stuff we have not yet read
        const tCIDLib::TCard4 c4Unread(m_c4InBufCount - m_c4InBufIndex);
        THeapBuf mbufSave(c4Unread, c4Unread);
        m_mbufInData.CopyOut(mbufSave, c4Unread, m_c4InBufIndex);

        m_mbufInData.CopyIn(pToPush, c4Bytes);
        m_mbufInData.CopyIn(mbufSave, c4Unread, c4Bytes);

        m_c4InBufIndex = 0;
        m_c4InBufCount = c4Bytes + c4Unread;
    }
}


//
//  This will reset our buffers. The primary purpose is for a redirection without
//  connection close. We need to get rid any previously returned data so that the new
//  read will start fresh, else we'd just accumulate it all.
//
tCIDLib::TVoid TCIDDataSrc::ResetBufs()
{
    m_c4InBufCount = 0;
    m_c4InBufIndex = 0;
    m_c4OutBufCount = 0;
}


//
//  This must be called when a data source is done, to clean up any resources or
//  do any final exchange before it is closed. This may optionally close the underlying
//  comm resource as well, or it can leave it open to be re-initialized for another
//  session with different communication parameters.
//
tCIDLib::TVoid
TCIDDataSrc::Terminate(const tCIDLib::TEncodedTime enctEnd, const tCIDLib::TBoolean bClose)
{
    // If initialized, then do a termination
    if (m_bInitialized)
    {
        //
        //  Clear the flag first. If it fails, we still consider it having beeen
        //  terminated.
        //
        m_bInitialized = kCIDLib::False;
        TerminateSrc(enctEnd, bClose);
    }
}


//
//  Send out bytes to the target.
//
tCIDLib::TVoid
TCIDDataSrc::WriteBytes(const TMemBuf& mbufData, const tCIDLib::TCard4 c4ToWrite)
{
    // Add it to the output buffer
    m_mbufOutData.CopyIn(mbufData, c4ToWrite, m_c4OutBufCount);
    m_c4OutBufCount += c4ToWrite;
}


tCIDLib::TVoid
TCIDDataSrc::WriteRawBytes( const   tCIDLib::TVoid* const   pData
                            , const tCIDLib::TCard4         c4ToWrite)
{
    // Add it to the output buffer
    m_mbufOutData.CopyIn(pData, c4ToWrite, m_c4OutBufCount);
    m_c4OutBufCount += c4ToWrite;
}


//
//  Convert the passed string to ASCII and send it. If ASCII isn't the desired encoding,
//  then transcode to binary first and write it that way. We assume the character
//  provided are valid ASCII, since this is very common in various network protocols.
//  So we do a very simple truncation to byte conversion.
//
tCIDLib::TVoid TCIDDataSrc::WriteString(const TString& strToWrite)
{
    const tCIDLib::TCard4 c4Count = strToWrite.c4Length();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        m_mbufOutData.PutCard1(tCIDLib::TCard1(strToWrite[c4Index]), m_c4OutBufCount++);
}


// ---------------------------------------------------------------------------
//  TCIDDataSrc: Hidden Constructors
// ---------------------------------------------------------------------------
TCIDDataSrc::TCIDDataSrc() :

    m_bInitialized(kCIDLib::False)
    , m_c4InBufCount(0)
    , m_c4InBufIndex(0)
    , m_c4OutBufCount(0)
    , m_c4ReadSz(4096)
    , m_mbufOutData(kCIDLib::c4Sz_32K)
{
}


// ---------------------------------------------------------------------------
//  TCIDDataSrc: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is called to reload the input cache. This is only called when the input
//  buffer is empty.
//
//  We must be initilized before use, and since this must be called upon any reading
//  of data, we do a check here for init.
//
tCIDLib::TVoid TCIDDataSrc::ReloadInCache(const tCIDLib::TEncodedTime enctEnd)
{
    if (!m_bInitialized)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcDSrc_NotInitilized
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }

    // Reset the input cache index for the next round
    if (m_c4InBufIndex == m_c4InBufCount)
    {
        // It's empty, so we can read directly into the input buffer
        m_c4InBufCount = c4GetData(m_mbufInData, m_c4ReadSz, enctEnd);
    }
     else
    {
        // We have to carry over data
        const tCIDLib::TCard4 c4Carry(m_c4InBufCount - m_c4InBufIndex);

        // Copy down the left over data
        m_mbufInData.MoveToStart(m_c4InBufIndex, c4Carry);

        //
        //  Load into the temp buffer. We only read what we can fit into the
        //  input buffer.
        //
        m_c4InBufCount = c4GetData(m_mbufTmpSpool, m_c4ReadSz - c4Carry, enctEnd);

        // Copy it in after the carry over
        m_mbufInData.CopyIn(m_mbufTmpSpool, m_c4InBufCount, c4Carry);

        // And add in the carryover bytes to our count
        m_c4InBufCount += c4Carry;
    }

    //
    //  The input buffer index gets zeroed either way, since the new data is at the
    //  start of the input buffer.
    //
    m_c4InBufIndex = 0;
}





// ---------------------------------------------------------------------------
//   CLASS: TCIDDataSrcJan
//  PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDDataSrcJan: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDDataSrcJan::TCIDDataSrcJan() :

    m_bClose(kCIDLib::False)
    , m_eAdopted(tCIDLib::EAdoptOpts::NoAdopt)
    , m_c4CleanupMSs(0)
    , m_pcdsToSanitize(nullptr)
{
    if (!m_c4CleanupMSs)
        m_c4CleanupMSs = 4000;
}

TCIDDataSrcJan::TCIDDataSrcJan(         TCIDDataSrc* const      pcdsToSanitize
                                , const tCIDLib::EAdoptOpts     eAdopt
                                , const tCIDLib::TBoolean       bClose
                                , const tCIDLib::TEncodedTime   enctEnd
                                , const tCIDLib::TCard4         c4CleanupMSs) :
    m_bClose(bClose)
    , m_eAdopted(eAdopt)
    , m_c4CleanupMSs(c4CleanupMSs)
    , m_pcdsToSanitize(nullptr)
{
    if (!m_c4CleanupMSs)
        m_c4CleanupMSs = 4000;

    //
    //  If not initialized yet, then do that. If this fails, we have to clean up
    //  the data source if we were asked to adopt.
    //
    if (pcdsToSanitize && !pcdsToSanitize->bIsInitialized())
    {
        tCIDLib::TEncodedTime enctRealEnd = enctEnd;
        if (!enctRealEnd)
            enctRealEnd = TTime::enctNowPlusSecs(5);

        try
        {
            pcdsToSanitize->Initialize(enctRealEnd);
        }

        catch(TError& errToCatch)
        {
            try
            {
                delete pcdsToSanitize;
            }

            catch(...)
            {
            }

            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }
    }

    // Looks, ok, so save the pointer
    m_pcdsToSanitize = pcdsToSanitize;
}

TCIDDataSrcJan::~TCIDDataSrcJan()
{
    if (m_pcdsToSanitize)
    {
        try
        {
            m_pcdsToSanitize->Terminate(TTime::enctNowPlusMSs(m_c4CleanupMSs), m_bClose);
        }

        catch(TError& errToCatch)
        {
            if (facCIDLib().bTestLog(errToCatch, tCIDLib::ELogFlags::DataSrc))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
            }
        }

        if (m_eAdopted == tCIDLib::EAdoptOpts::Adopt)
        {
            try
            {
                delete m_pcdsToSanitize;
                m_pcdsToSanitize = nullptr;
            }

            catch(TError& errToCatch)
            {
                if (facCIDLib().bTestLog(errToCatch, tCIDLib::ELogFlags::DataSrc))
                {
                    errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                    TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
                }
            }
        }
    }
}


// ---------------------------------------------------------------------------
//  TCIDDataSrcJan: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Provide access to our data source object
//
const TCIDDataSrc& TCIDDataSrcJan::cdsData() const
{
    if (!m_pcdsToSanitize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcDSrc_JanDataNotSet
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
    return *m_pcdsToSanitize;
}


TCIDDataSrc& TCIDDataSrcJan::cdsData()
{
    if (!m_pcdsToSanitize)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcDSrc_JanDataNotSet
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
    return *m_pcdsToSanitize;
}


TCIDDataSrc* TCIDDataSrcJan::pcdsData()
{
    return m_pcdsToSanitize;
}



// Orphan our data source out, leaving us with none
TCIDDataSrc* TCIDDataSrcJan::pcdsOrphan()
{
    TCIDDataSrc* pcdsRet = m_pcdsToSanitize;
    m_pcdsToSanitize = nullptr;
    return pcdsRet;
}

tCIDLib::TVoid TCIDDataSrcJan::Orphan()
{
    m_pcdsToSanitize = nullptr;
}


//
//  Set a new data source on us. We'll drop any current one, terminating it first,
//  and then deleting it if we adopted it.
//
tCIDLib::TVoid
TCIDDataSrcJan::SetNew(         TCIDDataSrc* const      pcdsToSanitize
                        , const tCIDLib::EAdoptOpts     eAdopt
                        , const tCIDLib::TBoolean       bClose
                        , const tCIDLib::TEncodedTime   enctEnd)
{
    //
    //  If we have an existing one and we own it, then clean it up. Use the currently
    //  stored close flag.
    //
    if (m_pcdsToSanitize)
    {
        try
        {
            m_pcdsToSanitize->Terminate(TTime::enctNowPlusMSs(m_c4CleanupMSs), m_bClose);
        }

        catch(TError& errToCatch)
        {
            if (facCIDLib().bTestLog(errToCatch, tCIDLib::ELogFlags::DataSrc))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
            }
        }

        if (m_eAdopted == tCIDLib::EAdoptOpts::Adopt)
            delete m_pcdsToSanitize;

        m_pcdsToSanitize = nullptr;
    }

    // Store the new one and the adoption info
    m_bClose = bClose;
    m_eAdopted = eAdopt;
    m_pcdsToSanitize = pcdsToSanitize;

    // If not initialized, then do that
    if (m_pcdsToSanitize && !m_pcdsToSanitize->bIsInitialized())
    {
        tCIDLib::TEncodedTime enctRealEnd = enctEnd;
        if (!enctRealEnd)
            enctRealEnd = TTime::enctNowPlusSecs(5);

        m_pcdsToSanitize->Initialize(enctRealEnd);
    }
}


