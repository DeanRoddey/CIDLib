//
// FILE NAME: CIDSpReco_WrapperStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/27/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the wrapper stream class that we use to feed audio into
//  the speech recognition engine, and which in turn wraps a provided application
//  specific stream that gets the actual data from wherever it might be coming from.
//  See our header for details on the streaming issues.
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
#include    "CIDSpReco_.hpp"
#include    <mmreg.h>


// This will create a WAV file with the audio, for debugging purposes
#if CID_DEBUG_ON
// #define WRITEOUTAUDIO 1
#if defined(WRITEOUTAUDIO)
TBinaryFile bflAudio(L"RecoAudio.wav");
#endif
#endif




// ---------------------------------------------------------------------------
//   CLASS: TSpWrapperStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSpWrapperStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TSpWrapperStream::TSpWrapperStream(TCIDAudioSrcStream* const pstrmSrc) :

    m_bSpeechActive(kCIDLib::False)
    , m_bSuppressInput(kCIDLib::False)
    , m_c4RefCnt(1)
    , m_pstrmSrc(pstrmSrc)
{
    #if CID_DEBUG_ON
    #if defined(WRITEOUTAUDIO)

    // Make sure the file is closed and re-create it
    if (bflAudio.bIsOpen())
        bflAudio.Close();

    bflAudio.Open
    (
        tCIDLib::EAccessModes::Excl_Write
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
    );

    //
    //  Set up the WAVE header, initially with an empty length since we don't know it yet,
    //  and with space for the id/size of the data chunk that will come after it.
    //
    tCIDMedia::TWaveHdr Hdr = {0};
    TRawMem::CopyMemBuf(Hdr.szRiff, "RIFF", 4);
    Hdr.c4Len = 0;
    TRawMem::CopyMemBuf(Hdr.szWavFmt, "WAVEfmt ", 8);
    Hdr.c4HdrLen = 16;
    Hdr.c2Format = 1;
    Hdr.c2NumChannels = 1;
    Hdr.c4SampleRate = 16000;
    Hdr.c4BytesPerSec = 16000 * 2;
    Hdr.c2BlockAlign = 2;
    Hdr.c2BitsPerSample = 16;
    bflAudio.c4WriteBuffer(&Hdr, sizeof(tCIDMedia::TWaveHdr));

    tCIDLib::TCard4 c4Val = 0;
    bflAudio.c4WriteBuffer(&c4Val, 4);
    bflAudio.c4WriteBuffer(&c4Val, 4);

    // We are now positioned at the right place to start writing

    #endif
    #endif

}

TSpWrapperStream::~TSpWrapperStream()
{
    #if CID_DEBUG_ON
    #if defined(WRITEOUTAUDIO)

    // Seek back to the length position and write it out, minus the 8 ASCII header bytes
    tCIDLib::TCard4 c4Len = tCIDLib::TCard4(bflAudio.c8CurSize());
    bflAudio.SetFilePos(4);
    c4Len -= 8;
    bflAudio.c4WriteBuffer(&c4Len, 4);

    //
    //  The actual number of data bytes is needed next. So subtract the header size. We
    //  already removed 8, so that covers the id/length bytes themselves.
    //
    c4Len -= sizeof(tCIDMedia::TWaveHdr);
    bflAudio.SetFilePos(sizeof(tCIDMedia::TWaveHdr));
    bflAudio.c4WriteBuffer("data", 4);
    bflAudio.c4WriteBuffer(&c4Len, 4);

    bflAudio.Flush();
    bflAudio.Close();

    #endif
    #endif
}


// ---------------------------------------------------------------------------
//  TSpWrapperStream: Public, inherited methods
// ---------------------------------------------------------------------------

STDMETHODIMP_(ULONG) TSpWrapperStream::AddRef()
{
    return InterlockedIncrement(&m_c4RefCnt);
}


STDMETHODIMP TSpWrapperStream::Clone(IStream**)
{
    return E_NOTIMPL;
}


STDMETHODIMP TSpWrapperStream::Commit(DWORD)
{
    return E_NOTIMPL;
}


STDMETHODIMP
TSpWrapperStream::CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER*)
{
    return E_NOTIMPL;
}


STDMETHODIMP TSpWrapperStream::LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
{
    return E_NOTIMPL;
}


STDMETHODIMP TSpWrapperStream::QueryInterface(REFIID riid, void **ppv)
{
    if (riid == IID_IUnknown)
    {
        AddRef();
        *ppv = (IUnknown*)this;
        return S_OK;
    }
    else if (riid == IID_IStream)
    {
        AddRef();
        *ppv = (IStream*)this;
        return S_OK;
    }
    else
    {
        return E_NOINTERFACE;
    }
}


STDMETHODIMP
TSpWrapperStream::Read(void *pBuffer, ULONG cbBuffer, ULONG *pcbRead)
{
    if (!pBuffer || !pcbRead || (cbBuffer == 0))
        return E_INVALIDARG;

    // If the stream is not in ready state, just return silence
    if (m_pstrmSrc->eState() != tCIDAudStream::EStrmStates::Ready)
    {
        TRawMem::SetMemBuf(pBuffer, tCIDLib::TCard1(0), cbBuffer);
        return S_OK;
    }

    //
    //  Speech Service isn't tolerant of partial reads, so read till we get all the
    //  samples requested.
    //
    tCIDLib::TCard4 c4BytesLeft = cbBuffer;
    tCIDLib::TCard1* pc1Cur = reinterpret_cast<tCIDLib::TCard1*>(pBuffer);
    while (c4BytesLeft)
    {
        // Stop returning Audio data if Speech isn't active
        if (!m_bSpeechActive)
        {
            *pcbRead = 0;
            return S_FALSE;
        }

        //
        //  Read up to the number of samples left
        //
        //  We MUST provide some throttling or we'd end up in a run away loop. So wait
        //  for a bit for data to arrive.
        //
        tCIDLib::TCard4 c4CurBytes;
        if (m_pstrmSrc->bReadBytes(pc1Cur, c4BytesLeft, c4CurBytes, 50))
        {
            // Move our buffer pointer forward and subtract current samples from total
            pc1Cur += c4CurBytes;
            c4BytesLeft -= c4CurBytes;
        }
    }

    //
    //  If suppressing input, give him bogus silence. This can be used to prevent
    //  TTS from trigging bogus input while it's speaking.
    //
    //  WE STILL need to read the data above, so that it doesn't end up showing
    //  later.
    //
    if (m_bSuppressInput)
        TRawMem::SetMemBuf(pBuffer, tCIDLib::TCard1(0), cbBuffer);

    #if CID_DEBUG_ON
    #if defined(WRITEOUTAUDIO)
    bflAudio.c4WriteBuffer(pBuffer, cbBuffer);
    #endif
    #endif

    // We always return the requested amount
    *pcbRead = cbBuffer;
    return S_OK;
}



STDMETHODIMP_(ULONG) TSpWrapperStream::Release()
{
    tCIDLib::TCard4 c4Ref = InterlockedDecrement(&m_c4RefCnt);
    if (!c4Ref)
        delete this;
    return c4Ref;
}


STDMETHODIMP TSpWrapperStream::Revert()
{
    return E_NOTIMPL;
}


STDMETHODIMP
TSpWrapperStream::Seek(LARGE_INTEGER, DWORD, ULARGE_INTEGER* plibNewPosition)
{
    //
    //  Speech seeks and expects a seek implementation - but the NUIAudio stream
    //  doesn't support seeking
    //
    if (plibNewPosition != nullptr)
        plibNewPosition->QuadPart = 0;

    return S_OK;
}

STDMETHODIMP TSpWrapperStream::SetSize(ULARGE_INTEGER)
{
    return E_NOTIMPL;
}


STDMETHODIMP TSpWrapperStream::Stat(STATSTG*, DWORD)
{
    return E_NOTIMPL;
}


STDMETHODIMP TSpWrapperStream::UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
{
    return E_NOTIMPL;
}


STDMETHODIMP
TSpWrapperStream::Write(const void*, ULONG cb, ULONG*)
{
    return E_NOTIMPL;
}


// ---------------------------------------------------------------------------
//  TSpWrapperStream: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get/set the speech active flag
tCIDLib::TBoolean TSpWrapperStream::bSpeechState() const
{
    return m_bSpeechActive;
}

tCIDLib::TBoolean TSpWrapperStream::bSpeechState(const tCIDLib::TBoolean bToSet)
{
    m_bSpeechActive = bToSet;
    return m_bSpeechActive;
}


// Get/set the input suppression flag
tCIDLib::TBoolean TSpWrapperStream::bSuppressInput() const
{
    return m_bSuppressInput;
}

tCIDLib::TBoolean TSpWrapperStream::bSuppressInput(const tCIDLib::TBoolean bToSet)
{
    //
    //  If we are turning back on input, then flush any currently cached buffers, to
    //  insure that content doesn't now suddenly show up. Anything seen before this
    //  should be discarded.
    //
    if (!bToSet && m_pstrmSrc)
        m_pstrmSrc->FlushBufs();

    m_bSuppressInput = bToSet;
    return m_bSuppressInput;
}

