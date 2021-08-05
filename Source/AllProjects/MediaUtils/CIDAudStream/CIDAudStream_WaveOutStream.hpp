//
// FILE NAME: CIDAudStream_WaveOutStream.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/01/2021
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2021
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is a derivative of our base audio target stream, which handles the common
//  scenario of output via a standard WAV style output source. Usually this will be
//  a standard system audio output.
//
//  This guy has per-platform implementation files to handle the platform specific
//  stuff. On Windows we use a standard WAVE output.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// Forward reference our internal per-platform structure
struct TCIDWaveOutStreamInfo;


// ---------------------------------------------------------------------------
//   CLASS: TCIDWaveOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDAUDSTREAMEXP TCIDWaveOutStream : public TCIDAudioTarStream
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDWaveOutStream();

        TCIDWaveOutStream(const TCIDWaveOutStream&) = delete;
        TCIDWaveOutStream(TCIDWaveOutStream&&) = delete;

        virtual ~TCIDWaveOutStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDWaveOutStream& operator=(const TCIDWaveOutStream&) = delete;
        TCIDWaveOutStream& operator=(TCIDWaveOutStream&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bWriteBytes
        (
            const   tCIDLib::TCard1* const  pc1Src
            , const tCIDLib::TCard4         c4SrcBytes
            , const tCIDLib::TCard4         c4WaitMSs
            , COP   tCIDLib::TCard4&        c4BytesWritten
        )   final;

        tCIDLib::TVoid FlushBufs() final;

        tCIDLib::TVoid QueryFormat
        (
            COP     tCIDLib::TCard4&        c4Channels
            , COP   tCIDLib::TCard4&        c4SamplesPerSec
            , COP   tCIDLib::TCard4&        c4BytesPerSample
        )   const final;

        tCIDLib::TVoid Initialize() final;

        tCIDLib::TVoid Terminate() final;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        [[noreturn]] tCIDLib::TVoid ThrowNotReady() const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_mtxSync
        //      We need a little sync here, so that the application can ask us to do
        //      things, without stepping on the toes of any thread that's reading the
        //      stream data.
        //
        //  m_pInfo
        //      An internal data structure we use to hide any OS specific stuff we
        //      need to deal with.
        // -------------------------------------------------------------------
        TMutex                  m_mtxSync;
        TCIDWaveOutStreamInfo*     m_pInfo;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDWaveOutStream,TCIDAudioTarStream)
};

#pragma CIDLIB_POPPACK
