//
// FILE NAME: CIDAudStream_WaveInStream_Win32.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/01/2019
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
//  This file provides the Win32 specific implementation of the Wave audio input
//  stream class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes. We use some kernel helpers here.
// ---------------------------------------------------------------------------
#include    "CIDAudStream_.hpp"

#pragma warning(push)
#pragma warning(disable : 26461 26473 26812)
#include    <windows.h>
#include    <mmsystem.h>
#pragma warning(pop)

#include    "CIDKernel_InternalHelpers_.hpp"
#include    "CIDKernel_RegistryWin32.hpp"


// ---------------------------------------------------------------------------
//  Force some system libraries to be linked in
// ---------------------------------------------------------------------------
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Ole32.lib")


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDAudStream_WaveInStream_Win32
    {
        //
        //  Constants related to our most common audio format, which is 16K at 16 bits
        //  per sample PCM, mono. For now, it's the only format supported. This stuff needs
        //  to be expanded to support format setting from the outside.
        //
        constexpr tCIDLib::TCard4   c4Channels      = 1;

        // 16000 samples per second, two bytes per sample
        constexpr tCIDLib::TCard4   c4SampleRate    = 16000;
        constexpr tCIDLib::TCard4   c4SampleBytes   = 2;

        // An 16th of a second of audio samples
        constexpr tCIDLib::TCard4   c4BufSamples    = (c4SampleRate / 16);

        // Bytes required to hold that number of samples
        constexpr tCIDLib::TCard4   c4BufBytes      = c4BufSamples * c4SampleBytes;

        // Number of buffers we have (which will get us three seconds of buffering)
        constexpr tCIDLib::TCard4   c4BufCount      = 48;
    }
}



// ---------------------------------------------------------------------------
//  Our internal structure that we use to track any OS specific resources, so as not
//  to expose them.
// ---------------------------------------------------------------------------
struct TCIDWaveInStreamInfo
{
    HWAVEIN             hWAVEIn;
    HANDLE              hNotEvent;

    // Info about which buffer we are working on, it's size, where we are in it
    tCIDLib::TCard4     c4BufAvail;
    tCIDLib::TCard4     c4BufIndex;
    tCIDLib::TCard4     c4NextBufInd;

    // We keep a set of buffers available
    WAVEHDR             hdrWAVEs[CIDAudStream_WaveInStream_Win32::c4BufCount];

    //
    //  An overflow buffer that we copy any data to that we can't return
    //  immediately, so that we can free the buffer back up. We'll give back
    //  data out of this until it's drained. m_c4BufAvail/Index above are
    //  used to track how much is left in this guy.
    //
    tCIDLib::TCard1*    pac1Over;
};



// ---------------------------------------------------------------------------
//  TCIDWaveInStream: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDWaveInStream::bEnumWaveInDevs(          TCIDAudioStreamList&    colToFill
                                    , const tCIDLib::TBoolean       bThrowIfNot)
{
    colToFill.RemoveAll();

    //
    //  Get the  number of devices available. There's no specific error return. If we
    //  get zero, so we have to just see if there's a last error.
    //
    const UINT uCnt =  waveInGetNumDevs();
    if (uCnt == 0)
    {
        const DWORD dwErr = ::GetLastError();
        if (dwErr)
        {
            TKrnlError::SetLastHostError(dwErr);
            if (bThrowIfNot)
            {
                facCIDAudStream().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kAudStrErrs::errcStrm_EnumDevices
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::NotFound
                );
            }
            return kCIDLib::False;
        }

        // Not an error, just no devices
        return kCIDLib::True;
    }

    // Assume generally they'll all be gotten successfully
    colToFill.CheckExpansion(uCnt);

    // Try to open the key where full names are found
    tCIDKernel::TWRegHandle hkeyNames = TKrnlWin32Registry::hkeyOpenSubKey
    (
        tCIDKernel::ERootKeys::LocalMachine
        , L"System\\CurrentControlSet\\Control\\"
        , L"MediaCategories"
        , tCIDKernel::ERegAccFlags::StdOwned
    );

    //
    //  Iterate through them and get their info. We have to look them up in the registry
    //  to get around a shortcoming in the name length that can be reported. If not in the
    //  registry, we assume the iterated name is the full one.
    //
    tCIDLib::TZStr512 szName;
    TString strName;
    TString strId;
    WAVEINCAPS2 wicCur;
    for (UINT uIndex = 0; uIndex < uCnt; uIndex++)
    {
        MMRESULT res = ::waveInGetDevCaps(uIndex, reinterpret_cast<WAVEINCAPS*>(&wicCur), sizeof(WAVEINCAPS2));
        if (res != MMSYSERR_NOERROR)
            continue;

        constexpr tCIDLib::TCard4 c4BufSz = 127;
        tCIDLib::TCh achBuf[c4BufSz + 1];
        if (!::StringFromGUID2(wicCur.ProductGuid, achBuf, c4BufSz))
            continue;
        strId = achBuf;

        if (TKrnlWin32Registry::bQueryStrValue(hkeyNames, achBuf, szName, 512))
        {
            strName = szName;
        }
         else
        {
            strName = wicCur.szPname;
        }
        colToFill.objPlace(strName, strId);
    }

    // Close the registery key if we opened it
    if (hkeyNames != nullptr)
        ::TKrnlWin32Registry::bCloseKey(hkeyNames);

    // If none of them worked, treat that as an error
    if (colToFill.bIsEmpty() && bThrowIfNot)
    {
        facCIDAudStream().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kAudStrErrs::errcStrm_EnumDevices
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TCIDWaveInStream: Public, virtual methods
// ---------------------------------------------------------------------------

//
//  Reads up to the indicated number of bytes. The count could be zero if nothing is
//  available in the short time that we wait for data to show up.
//
tCIDLib::TBoolean
TCIDWaveInStream::bReadBytes(          tCIDLib::TCard1* const  pc1ToFill
                            , const tCIDLib::TCard4         c4MaxBytes
                            , COP   tCIDLib::TCard4&        c4BytesRead
                            , const tCIDLib::TCard4         c4WaitMSs)
{
    if (!m_pInfo)
        ThrowNotReady();

    // Lock while we do this
    TLocker lockrSync(&m_mtxSync);

    //
    //  If we have some left over data from last time, return it first. It's even
    //  possible we still cannot return it all, so be careful of that.
    //
    if (m_pInfo->c4BufAvail)
    {
        c4BytesRead = m_pInfo->c4BufAvail - m_pInfo->c4BufIndex;

        if (c4BytesRead <= c4MaxBytes)
        {
            // We can return it all
            TRawMem::CopyMemBuf(pc1ToFill, &m_pInfo->pac1Over[m_pInfo->c4BufIndex], c4BytesRead);
            m_pInfo->c4BufAvail = 0;
            m_pInfo->c4BufIndex = 0;
        }
         else
        {
            c4BytesRead = c4MaxBytes;
            TRawMem::CopyMemBuf(pc1ToFill, &m_pInfo->pac1Over[m_pInfo->c4BufIndex], c4BytesRead);

            // Move the index up for the next time
            m_pInfo->c4BufIndex += c4BytesRead;
        }
        return kCIDLib::True;
    }


    // Wait for the indicated time for something to show up. If not, return false now
    c4BytesRead = 0;
    const tCIDLib::EWaitRes eRes = TKrnlWin32::eWaitHandle(m_pInfo->hNotEvent, c4WaitMSs);
    if (eRes != tCIDLib::EWaitRes::Success)
        return kCIDLib::False;

    //
    //  Loop through the buffers until we hit one not ready or we cannot give
    //  back any more. When we get to that point, copy the rest of that buffer
    //  to the overflow to be handed out in subsequent calls.
    //
    while (c4BytesRead < c4MaxBytes)
    {
        // If this one isn't ready, break out
        if (!(m_pInfo->hdrWAVEs[m_pInfo->c4NextBufInd].dwFlags & WHDR_DONE))
            break;

        //
        //  Get a convenience reference, then we can bump the buffer for the next round,
        //  now, before any potential error could cause us to fail to do this.
        //
        WAVEHDR& hdrCur = m_pInfo->hdrWAVEs[m_pInfo->c4NextBufInd];
        m_pInfo->c4NextBufInd++;
        if (m_pInfo->c4NextBufInd == CIDAudStream_WaveInStream_Win32::c4BufCount)
            m_pInfo->c4NextBufInd = 0;

        // Calc how much room left
        const tCIDLib::TCard4 c4SpaceAvail = c4MaxBytes - c4BytesRead;

        //
        //  See how much of it we can copy over. If not all, we have to save some for
        //  the next call.
        //
        if (hdrCur.dwBytesRecorded <= c4SpaceAvail)
        {
            // We can take all of this one
            TRawMem::CopyMemBuf(&pc1ToFill[c4BytesRead], hdrCur.lpData, hdrCur.dwBytesRecorded);
            c4BytesRead += hdrCur.dwBytesRecorded;
        }
         else
        {
            // We can only return part of it
            TRawMem::CopyMemBuf(&pc1ToFill[c4BytesRead], hdrCur.lpData, c4SpaceAvail);
            c4BytesRead += c4SpaceAvail;

            // Get the rest into the overflow buffer
            m_pInfo->c4BufAvail = hdrCur.dwBytesRecorded - c4SpaceAvail;
            m_pInfo->c4BufIndex = 0;

            TRawMem::CopyMemBuf
            (
                m_pInfo->pac1Over
                , &(reinterpret_cast<tCIDLib::TCard1*>(hdrCur.lpData))[c4SpaceAvail]
                , m_pInfo->c4BufAvail
            );
        }

        // And give this one back
        ::waveInAddBuffer(m_pInfo->hWAVEIn, &hdrCur, sizeof(WAVEHDR));
    }

    return kCIDLib::True;
}


//
//  If the application stops listening for a while, it will need to flush any currently
//  queued up data  before it starts again, else that will show up before the new info
//  it really cares about.
//
tCIDLib::TVoid TCIDWaveInStream::FlushBufs()
{
    if (!m_pInfo)
        ThrowNotReady();

    TLocker lockrSync(&m_mtxSync);

    // Just move through the buffers until we find one not ready
    while (kCIDLib::True)
    {
        // Get a ref to the next available buffer. If not marked done, we are finished
        WAVEHDR& hdrCur = m_pInfo->hdrWAVEs[m_pInfo->c4NextBufInd];
        if (!(hdrCur.dwFlags & WHDR_DONE))
            break;

        // Give this one back to the OS
        ::waveInAddBuffer(m_pInfo->hWAVEIn, &hdrCur, sizeof(WAVEHDR));

        // And move forward, wrapping if needed
        m_pInfo->c4NextBufInd++;
        if (m_pInfo->c4NextBufInd == CIDAudStream_WaveInStream_Win32::c4BufCount)
            m_pInfo->c4NextBufInd = 0;
    }

    // Make sure the overflow buffer is seen as empty as well
    m_pInfo->c4BufAvail = 0;
    m_pInfo->c4BufIndex = 0;
}


//
//  This method must be called to set us up. We create our round buffers and get all
//  of the waveIn stuff set up. We set up our internal structure for tracking state.
//
tCIDLib::TVoid TCIDWaveInStream::Initialize()
{
    if (m_pInfo)
    {
        facCIDAudStream().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kAudStrErrs::errcStrm_AlreadyInit
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
        );
    }

    try
    {
        m_pInfo = new TCIDWaveInStreamInfo;
        TRawMem::SetMemBuf(m_pInfo, kCIDLib::c1MinCard, sizeof(TCIDWaveInStreamInfo));

        //
        //  Allocate our overflow buffer. It's the same size as one of our regular
        //  buffers.
        //
        m_pInfo->pac1Over = new tCIDLib::TCard1[CIDAudStream_WaveInStream_Win32::c4BufBytes];

        // Create the event we'll register, an auto-reset one
        m_pInfo->hNotEvent = ::CreateEvent(0, 0, 0, 0);

        // Open the default wave, asking for our desired audio format
        WAVEFORMATEX wfxWanted =
        {
            WAVE_FORMAT_PCM
            , 1
            , CIDAudStream_WaveInStream_Win32::c4SampleRate
            , CIDAudStream_WaveInStream_Win32::c4SampleRate
              * CIDAudStream_WaveInStream_Win32::c4SampleBytes
            , CIDAudStream_WaveInStream_Win32::c4SampleBytes
            , CIDAudStream_WaveInStream_Win32::c4SampleBytes * 8
            , 0
        };
        MMRESULT mRes = ::waveInOpen
        (
            &m_pInfo->hWAVEIn
            , WAVE_MAPPER
            , &wfxWanted
            , (DWORD_PTR)m_pInfo->hNotEvent
            , 0
            , WAVE_MAPPED_DEFAULT_COMMUNICATION_DEVICE | CALLBACK_EVENT
        );

        if (mRes != MMSYSERR_NOERROR)
        {
            facCIDAudStream().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kAudStrErrs::errcStrm_OpenDefWavIn
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TInteger(mRes, tCIDLib::ERadices::Hex)
            );
        }

        // Set up the buffers
        for (tCIDLib::TCard4 c4Index = 0; c4Index < CIDAudStream_WaveInStream_Win32::c4BufCount; c4Index++)
        {
            WAVEHDR& hdrCur = m_pInfo->hdrWAVEs[c4Index];

            hdrCur.lpData = (LPSTR)new tCIDLib::TCard1
            [
                CIDAudStream_WaveInStream_Win32::c4BufBytes
            ];
            hdrCur.dwFlags = 0;
            hdrCur.dwBufferLength = CIDAudStream_WaveInStream_Win32::c4BufBytes;

            mRes = ::waveInPrepareHeader(m_pInfo->hWAVEIn, &hdrCur, sizeof(WAVEHDR));
            if (mRes != MMSYSERR_NOERROR)
            {
                facCIDAudStream().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kAudStrErrs::errcStrm_InitWAVEBuf
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                    , TInteger(mRes, tCIDLib::ERadices::Hex)
                );
            }
            mRes = ::waveInAddBuffer(m_pInfo->hWAVEIn, &hdrCur, sizeof(WAVEHDR));
            if (mRes != MMSYSERR_NOERROR)
            {
                facCIDAudStream().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kAudStrErrs::errcStrm_AddWAVEBuf
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                    , TInteger(mRes, tCIDLib::ERadices::Hex)
                );
            }
        }

        // Now start processing
        waveInStart(m_pInfo->hWAVEIn);

        // Set our parent class' stream state to ready
        eState(tCIDAudStream::EStrmStates::Ready);
    }

    catch(TError& errToCatch)
    {
        // Clean us back up since it didn't go well
        try
        {
            Terminate();
        }

        catch(TError& errNested)
        {
            errNested.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errNested);
        }

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}


// Returns the sample format for for this stream.
tCIDLib::TVoid
TCIDWaveInStream::QueryFormat( COP     tCIDLib::TCard4&    c4Channels
                            , COP   tCIDLib::TCard4&    c4SamplesPerSec
                            , COP   tCIDLib::TCard4&    c4BytesPerSample) const
{
    // Make sure we are ready
    if (!m_pInfo)
        ThrowNotReady();

    TLocker lockrSync(&m_mtxSync);
    c4Channels = CIDAudStream_WaveInStream_Win32::c4Channels;
    c4SamplesPerSec = CIDAudStream_WaveInStream_Win32::c4SampleRate;
    c4BytesPerSample = CIDAudStream_WaveInStream_Win32::c4SampleBytes;
}


//
//  This will clean up our resources. We have to make sure we leave ourself in a
//  state where multiple calls to this method will not do anything wrong.
//
tCIDLib::TVoid TCIDWaveInStream::Terminate()
{
    TLocker lockrSync(&m_mtxSync);

    if (m_pInfo)
    {
        // Most of this could have only gotten set up if we opened the wave device
        if (m_pInfo->hWAVEIn)
        {
            // Stop processing
            waveInStop(m_pInfo->hWAVEIn);

            // Unprep our buffers and release them
            for (tCIDLib::TCard4 c4Index = 0;
                            c4Index < CIDAudStream_WaveInStream_Win32::c4BufCount; c4Index++)
            {
                WAVEHDR& hdrCur = m_pInfo->hdrWAVEs[c4Index];

                // If we never allocated the buffer, couldn't have added it
                if (hdrCur.lpData)
                {
                    ::waveInUnprepareHeader(m_pInfo->hWAVEIn, &hdrCur, sizeof(WAVEHDR));
                    delete [] reinterpret_cast<tCIDLib::TCard1*>(hdrCur.lpData);
                }
            }

            // And now we can close the device
            ::waveInClose(m_pInfo->hWAVEIn);
        }

        // Clean up our overflow buffer if we allocated it
        if (m_pInfo->pac1Over)
        {
            delete [] m_pInfo->pac1Over;
            m_pInfo->pac1Over = nullptr;
        }

        // And finally clean up our info structure which marks us as terminated
        delete m_pInfo;
        m_pInfo = nullptr;
    }
}

