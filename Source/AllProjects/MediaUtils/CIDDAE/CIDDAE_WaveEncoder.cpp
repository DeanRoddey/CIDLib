//
// FILE NAME: CIDDAE_WaveEncoder.cpp
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
//  This file implements wave encoder derivative of the base encoder class.
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
RTTIDecls(TCIDDAEWaveDec,TCIDDAEDecoder)
RTTIDecls(TCIDDAEWaveEnc,TCIDDAEEncoder)


// ---------------------------------------------------------------------------
//  Local types and data
// ---------------------------------------------------------------------------
namespace CIDDAE_WaveEncoder
{
    namespace
    {
        // -----------------------------------------------------------------------
        //  The names we return for our codec and format
        // -----------------------------------------------------------------------
        const TString strCodecName(L"DefaultWAV");
        const TString strFormatName(L"44Khz, 2Ch, 16Bit, PCM");


        // -----------------------------------------------------------------------
        //  The maximum amount of data we'll decode in a single chunk decode
        //  call.
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard4   c4MaxReadChunkSz = 16 * 1024;
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TCIDDAEWaveDec
// PREFIX: daed
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDDAEWMADec: Public, static methods
// ---------------------------------------------------------------------------
const TString& TCIDDAEWaveDec::strCodecName()
{
    return CIDDAE_WaveEncoder::strCodecName;
}

const TString& TCIDDAEWaveDec::strFormatName()
{
    return CIDDAE_WaveEncoder::strFormatName;
}


// ---------------------------------------------------------------------------
//  TCIDDAEWaveDec: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDDAEWaveDec::TCIDDAEWaveDec() :

    TCIDDAEDecoder(CIDDAE_WaveEncoder::strCodecName)
    , m_c4SoFar(0)
    , m_c4DataBytes(0)
{
}

TCIDDAEWaveDec::~TCIDDAEWaveDec()
{
}



// -------------------------------------------------------------------
//  Protected, virtual methods
// -------------------------------------------------------------------
tCIDLib::TCard4
TCIDDAEWaveDec::c4LoadChunkImpl(        TMemBuf&        mbufToFill
                                , const tCIDLib::TCard4 c4StartInd)
{
    // Read up to our max chunk size or what we have left in the data chunk.
    const tCIDLib::TCard4 c4ToRead = tCIDLib::MinVal
    (
        CIDDAE_WaveEncoder::c4MaxReadChunkSz, m_c4DataBytes - m_c4SoFar
    );

    //
    //  If not loading into the 0th position, we have to play some tricks
    //  to append the new data.
    //
    tCIDLib::TCard4 c4Read = 0;
    if (c4StartInd)
    {
        //
        //  We need to make sure the buffer is large enough and do a read
        //  directly into it's buffer at the right place.
        //
        if (mbufToFill.c4Size() < c4StartInd + c4ToRead)
            mbufToFill.Reallocate(c4StartInd + c4ToRead, kCIDLib::False);

        c4Read = m_flSrc.c4ReadBuffer
        (
            mbufToFill.pc1DataAt(c4StartInd), c4ToRead
        );
    }
     else
    {
        c4Read = m_flSrc.c4ReadBuffer(mbufToFill, c4ToRead);
    }
    return c4Read;
}


tCIDLib::TVoid TCIDDAEWaveDec::CleanupImpl()
{
    // Nothing to do
}


tCIDLib::TVoid TCIDDAEWaveDec::EndDecodeImpl()
{
    // Close the source file if we got it open
    if (m_flSrc.bIsOpen())
        m_flSrc.Close();
}


tCIDLib::TVoid TCIDDAEWaveDec::StartDecodeImpl(const TString& strSrcFile)
{
    // Try to open the source file
    m_flSrc.strName(strSrcFile);
    m_flSrc.Open
    (
        tCIDLib::EAccessModes::Excl_Read
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::None
    );

    //
    //  If it's not at least large enough for a header, it can't be valid,
    //  so we just read a header's worth and throw if we can't read that
    //  much.
    //
    tCIDMedia::TWaveHdr Hdr = {0};
    m_flSrc.c4ReadBuffer(&Hdr, sizeof(Hdr), tCIDLib::EAllData::FailIfNotAll);

    // Make sure it's a RIFF Wave file
    if ((TRawStr::eCompareStrN(Hdr.szRiff, "RIFF", 4) != tCIDLib::ESortComps::Equal)
    ||  (TRawStr::eCompareStrN(Hdr.szWavFmt, "WAVEfmt ", 8) != tCIDLib::ESortComps::Equal))
    {
        facCIDDAE().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDAEErrs::errcWAV_NotWaveFile
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // If there's extra data after the standard header, skip it
    if (Hdr.c4HdrLen > 16)
        m_flSrc.c8OffsetFilePos(Hdr.c4HdrLen - 16);

    //
    //  Find the data chunk, which is all that we will want to return. We'll
    //  save the size and zero out our counter so that we know when we've
    //  hit the end of that chunk.
    //
    m_c4SoFar = 0;
    m_c4DataBytes = 0;

    // Search forward for the data chunk. We only support one data chunk
    tCIDLib::TCard4 c4ID = 0;
    tCIDLib::TCard4 c4Sz = 0;
    while (kCIDLib::True)
    {
        // Get the chunk id and size
        c4ID = c4ReadNext();
        c4Sz = c4ReadNext();

        //
        //  If it's the data chunk, then we can break out. Store the chunk
        //  size for later user and reset the so far counter. Else, jump
        //  forward over it
        //
        if (c4ID == 0x61746164)
        {
            m_c4SoFar = 0;
            m_c4DataBytes = c4Sz;
            break;
        }

        //
        //  The next chunk will be word aligned. So if we ended up on an
        //  odd byte, increment the offset and size.
        //
        if (m_c4SoFar & 0x1)
        {
            m_c4SoFar++;
            c4Sz++;
        }
        m_flSrc.c8OffsetFilePos(c4Sz);
    }

    // If we never found one, it's not a valid WAV file
    if (c4ID != 0x61746164)
    {
        facCIDDAE().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDAEErrs::errcWAV_NotWaveFile
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }
}


// ---------------------------------------------------------------------------
//  TCIDDAEWMADec: Private, non-virtula methods
// ---------------------------------------------------------------------------
tCIDLib::TCard1 TCIDDAEWaveDec::c1ReadNext()
{
    tCIDLib::TCard1 c1Ret;
    m_flSrc.c4ReadBuffer(&c1Ret, 1, tCIDLib::EAllData::FailIfNotAll);
    m_c4SoFar++;
    return c1Ret;
}

tCIDLib::TCard2 TCIDDAEWaveDec::c2ReadNext()
{
    tCIDLib::TCard2 c2Ret;
    m_flSrc.c4ReadBuffer(&c2Ret, 2, tCIDLib::EAllData::FailIfNotAll);
    m_c4SoFar += 2;
    #if CIDLIB_BIGENDIAN
    c2Ret = TRawBits::c2SwapBytes(c2Ret);
    #endif
    return c2Ret;
}

tCIDLib::TCard4 TCIDDAEWaveDec::c4ReadNext()
{
    tCIDLib::TCard4 c4Ret;
    m_flSrc.c4ReadBuffer(&c4Ret, 4, tCIDLib::EAllData::FailIfNotAll);
    m_c4SoFar += 4;
    #if CIDLIB_BIGENDIAN
    c4Ret = TRawBits::c4SwapBytes(c4Ret);
    #endif
    return c4Ret;
}




// ---------------------------------------------------------------------------
//  CLASS: TCIDDAEWaveEnc
// PREFIX: daee
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDDAEWMAEnc: Public, static methods
// ---------------------------------------------------------------------------
const TString& TCIDDAEWaveEnc::strCodecName()
{
    return CIDDAE_WaveEncoder::strCodecName;
}

const TString& TCIDDAEWaveEnc::strFormatName()
{
    return CIDDAE_WaveEncoder::strFormatName;
}


// ---------------------------------------------------------------------------
//  TCIDDAEWaveEnc: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDDAEWaveEnc::TCIDDAEWaveEnc() :

    TCIDDAEEncoder(CIDDAE_WaveEncoder::strCodecName)
    , m_c4Total(0)
{
}

TCIDDAEWaveEnc::~TCIDDAEWaveEnc()
{
}


// ---------------------------------------------------------------------------
//  TCIDDAEWaveEnc: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDDAEWaveEnc::bCodecInfoValid(const   TString&    strCodec
                                , const TString&    strFormat) const
{
    return ((strCodec == CIDDAE_WaveEncoder::strCodecName)
           && (strFormat == CIDDAE_WaveEncoder::strFormatName));
}


tCIDLib::TCard4 TCIDDAEWaveEnc::c4QueryCodecNames(TVector<TString>& colToFill)
{
    colToFill.RemoveAll();
    colToFill.objAdd(CIDDAE_WaveEncoder::strCodecName);
    return 1;
}


tCIDLib::TCard4
TCIDDAEWaveEnc::c4QueryCodecFmtNames(const  TString&            strCodec
                                    ,       TVector<TString>&   colToFill)
{
    if (strCodec != CIDDAE_WaveEncoder::strCodecName)
        return 0;

    colToFill.RemoveAll();
    colToFill.objAdd(CIDDAE_WaveEncoder::strFormatName);
    return 1;
}


// ---------------------------------------------------------------------------
//  TCIDDAEWaveEnc: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  This is called when the encoder is no longer needed, and gives up a
//  chance to clean up any resources. For us it's a no-op.
//
tCIDLib::TVoid TCIDDAEWaveEnc::CleanupImpl()
{
}


//
//  Handles any post-encoding cleanup we need to do. We have to go back and
//  create the wave header and write it out to the space we left at the start
//  of the file.
//
tCIDLib::TVoid TCIDDAEWaveEnc::EndEncodeImpl()
{
    // Fill in the real wave header now
    tCIDMedia::TWaveHdr Hdr = {0};

    TRawMem::CopyMemBuf(Hdr.szRiff, "RIFF", 4);
    Hdr.c4Len = m_c4Total + 44 - 8;
    TRawMem::CopyMemBuf(Hdr.szWavFmt, "WAVEfmt ", 8);
    Hdr.c4HdrLen = 16;
    Hdr.c2Format = 1;
    Hdr.c2NumChannels = 2;
    Hdr.c4SampleRate = 44100;
    Hdr.c4BytesPerSec = 44100 * 2 * 2;
    Hdr.c2BlockAlign = 4;
    Hdr.c2BitsPerSample = 16;

    //
    //  Seek back to the start of the file and write the header, the
    //  data chunk id and the size.
    //
    m_flTarget.SetFilePos(0);
    tCIDLib::TCard4 c4Id = 0x61746164;
    if ((m_flTarget.c4WriteBuffer(&Hdr, sizeof(Hdr)) != sizeof(Hdr))
    ||  (m_flTarget.c4WriteBuffer(&c4Id, 4) != 4)
    ||  (m_flTarget.c4WriteBuffer(&m_c4Total, 4) != 4))
    {
        facCIDDAE().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDAEErrs::errcEnc_WriteErr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllWritten
            , TCardinal(sizeof(Hdr) + 8)
        );
    }

    // Close the source file if we got it open
    if (m_flTarget.bIsOpen())
        m_flTarget.Close();
}


//
//  Handles any startup work we need to do. We open up the target file,
//  then write out a black wave header as a place holder. We'll come back
//  at the end and write out the real header.
//
//  The encoding has no meaning for us. We are inherently a lossless
//  format. Nor does the metadata since WAV files have no way to support
//  tagging.
//
tCIDLib::TVoid
TCIDDAEWaveEnc::StartEncodeImpl(const   TString&        strTargetFile
                                , const TString&
                                , const TString&
                                , const tCIDLib::TCard4
                                , const TString&
                                , const TString&
                                , const TString&
                                , const TString&
                                , const TString&
                                , const tCIDLib::TCard4)
{
    //
    //  Try to open the target file. We assume they want to overwrite any
    //  existing file and the derived encoder will need read/write access.
    //
    m_flTarget.strName(strTargetFile);
    m_flTarget.Open
    (
        tCIDLib::EAccessModes::Excl_ReadWrite
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::None
    );

    //
    //  Write out empty bytes for the header plus room for the data chunk
    //  id/size that we'll have to write after it.
    //
    tCIDMedia::TWaveHdr Hdr = {0};
    const tCIDLib::TCard4 c4Count = sizeof(Hdr) + 8;
    if (m_flTarget.c4WriteBuffer(&Hdr, c4Count) != c4Count)
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

    // Reset any of our per-session stuff
    m_c4Total = 0;
}


//
//  This is called repeatedly to store the incoming data . For WAV
//  content, we just write the data as is since it's just raw samples.
//
tCIDLib::TVoid
TCIDDAEWaveEnc::StoreChunkImpl( const   tCIDLib::TCard1* const  pc1Data
                                , const tCIDLib::TCard4         c4Bytes)
{
    if (m_flTarget.c4WriteBuffer(pc1Data, c4Bytes) != c4Bytes)
    {
        facCIDDAE().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDAEErrs::errcEnc_WriteErr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotAllWritten
            , TCardinal(c4Bytes)
        );
    }

    // Keep up with the data bytes written
    m_c4Total += c4Bytes;
}


