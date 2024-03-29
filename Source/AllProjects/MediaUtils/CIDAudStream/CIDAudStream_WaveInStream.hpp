//
// FILE NAME: CIDAudStream_WaveInStream.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/11/2017
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
//  This is a derivative of our base audio source stream, which handles the common
//  scenario of input via a standard WAV style input source. Usually this will be
//  a standard system audio input.
//
//  This guy has per-platform implementation files to handle the platform specific
//  stuff. On Windows we use a standard WAVE input.
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
struct TCIDWaveInStreamInfo;


// ---------------------------------------------------------------------------
//   CLASS: TCIDWaveInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDAUDSTREAMEXP TCIDWaveInStream : public TCIDAudioSrcStream
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bEnumWaveInDevs
        (
                    TCIDAudioStreamList&    colToFill
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDWaveInStream();

        TCIDWaveInStream(const TCIDWaveInStream&) = delete;
        TCIDWaveInStream(TCIDWaveInStream&&) = delete;

        virtual ~TCIDWaveInStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDWaveInStream& operator=(const TCIDWaveInStream&) = delete;
        TCIDWaveInStream& operator=(TCIDWaveInStream&&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bReadBytes
        (
                    tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4MaxBytes
            , COP   tCIDLib::TCard4&        c4BytesRead
            , const tCIDLib::TCard4         c4WaitMSs
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
        TCIDWaveInStreamInfo*   m_pInfo;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDWaveInStream,TCIDAudioSrcStream)
};

#pragma CIDLIB_POPPACK
