//
// FILE NAME: CIDRTP_WAVIO.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/12/2015
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
//  This class implements a derivative of the RTP I/O source class that handles
//  basic WAV files. Mainly we are interested in uLaw (for voice) and CD quality
//  for music.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TFileWaveIO
// PREFIX: rtpio
// ---------------------------------------------------------------------------
class CIDRTPEXP TFileWaveIO : public TCIDRTPIOSrc
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFileWaveIO() = delete;

        TFileWaveIO
        (
            const   TString&                strPath
            , const tCIDLib::TCard1         c1AVPFormat
        );

        TFileWaveIO(const TFileWaveIO&) = delete;
        TFileWaveIO(TFileWaveIO&&) = delete;

        ~TFileWaveIO();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFileWaveIO& operator=(const TFileWaveIO&);
        TFileWaveIO& operator=(TFileWaveIO&&);


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInitialized() const override;

        tCIDLib::TCard4 c4ReadSamples
        (
                    TMemBuf&                mbufToFill
            , COP   tCIDLib::TCard4&        c4AtSample
            , COP   tCIDLib::TCard4&        c4AtMillisec
        )   final;

        tCIDLib::TVoid Initialize
        (
            COP     tCIDLib::TCard1&        c1PayloadType
            , COP   tCIDLib::TCard4&        c4PayloadMSs
            , COP   tCIDLib::TCard4&        c4PayloadBytes
            , COP   tCIDLib::TCard4&        c4PayloadSamples
            , COP   tCIDLib::TCard4&        c4SampleBytes
            , COP   tCIDLib::TCard4&        c4TotalMSs
            , COP   tCIDLib::TCard4&        c4TotalBytes
        )   final;

        tCIDLib::TVoid Terminate() final;



    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ReadNext();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c1AVPPLType
        //      The AVP payload type that we were told that this file should be
        //      in. If it's not, we throw. Or if it's not one we support, we throw.
        //
        //  m_c4ClockInc
        //      The amount to move the RTP time by for each packet. It's not just
        //      the samples per packet, since apparently it has to take into
        //      account channels, so twice as many for stereo.
        //
        //  m_c4DataBytes
        //  m_c4DataPos
        //      The size of the audio data chunk and our current read position
        //      within it, which we need to know when we hit the end in the
        //      c4ReadSamples method.
        //
        //  m_c4PayloadBytes
        //  m_c4PayloadSamples
        //      This is calculated during init. We give this info back from the
        //      init, but we also want to keep a copy since these are needed by
        //      c4ReadSamples.
        //
        //  m_flSrc
        //      The file object that we use to read in the source wave.
        //
        //  m_strPath
        //      The path we were given for the file to read from.
        // -------------------------------------------------------------------
        tCIDLib::TCard1 m_c1AVPPLType;
        tCIDLib::TCard4 m_c4ClockInc;
        tCIDLib::TCard4 m_c4DataBytes;
        tCIDLib::TCard4 m_c4DataPos;
        tCIDLib::TCard4 m_c4PayloadBytes;
        tCIDLib::TCard4 m_c4PayloadSamples;
        TBinaryFile     m_flSrc;
        TString         m_strPath;
};


#pragma CIDLIB_POPPACK


