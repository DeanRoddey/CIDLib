//
// FILE NAME: CIDAudStream_StdSrcStream.hpp
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
//  scenario of input via a standard Windows audio input via the waveIn interface.
//
//  By default it works on the default audio input.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// Forward reference our internal structure
struct TWaveInSrcStreamInfo;


// ---------------------------------------------------------------------------
//   CLASS: TWaveInSrcStream
//  PREFIX: strm
// ---------------------------------------------------------------------------
class CIDAUDSTREAMEXP TWaveInSrcStream : public TCIDAudioSrcStream
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TWaveInSrcStream();

        TWaveInSrcStream(const TWaveInSrcStream&) = delete;

        virtual ~TWaveInSrcStream();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TWaveInSrcStream& operator=(const TWaveInSrcStream&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bReadBytes
        (
                    tCIDLib::TCard1* const  pc1ToFill
            , const tCIDLib::TCard4         c4MaxBytes
            ,       tCIDLib::TCard4&        c4BytesRead
            , const tCIDLib::TCard4         c4WaitMSs
        )   override;

        tCIDLib::TVoid FlushBufs() override;

        tCIDLib::TVoid QueryFormat
        (
                    tCIDLib::TCard4&        c4Channels
            ,       tCIDLib::TCard4&        c4SamplesPerSec
            ,       tCIDLib::TCard4&        c4BytesPerSample
        )   const override;

        tCIDLib::TVoid Initialize() override;

        tCIDLib::TVoid Terminate() override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowNotReady() const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4BufAvail
        //  m_c4BufIndex
        //      The current number of bytes available in the overflow buffer, and our
        //      current index into it.
        //
        //  m_c4NextBufInd
        //      Buffers are filled in the order they are added, so we use this go
        //      loop through them, so that we are sure to get them in the right order
        //      if more than one is ready. We just wrap it around to zero when we the
        //      the buffer count.
        //
        //  m_mtxSync
        //      We need a little sync here, so that the application can ask us to do
        //      things, without stepping on the toes of the reco engine, which will
        //      be (indirectly via the layered input streams) be reading data as well.
        //
        //  m_pac1Over
        //      An overflow buffer that we copy any data to that we can't return
        //      immediately, so that we can free the buffer back up. We'll give back
        //      data out of this until it's drained. m_c4BufAvail/Index above are
        //      used to track how much is left in this guy.
        //
        //  m_pInfo
        //      An internal data structure we use to hide any OS specific stuff we
        //      need to deal with.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4BufAvail;
        tCIDLib::TCard4         m_c4BufIndex;
        tCIDLib::TCard4         m_c4NextBufInd;
        TMutex                  m_mtxSync;
        tCIDLib::TCard1*        m_pac1Over;
        TWaveInSrcStreamInfo*   m_pInfo;
};

#pragma CIDLIB_POPPACK
