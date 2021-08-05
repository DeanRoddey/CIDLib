//
// FILE NAME: CIDAudStream_Audiofmt.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/05/2021
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
//  This file defines the TCIDAudStreamFmt class, which is used to report and request
//  particular audio formats on audio streams, files, etc...
//
//  !!Bytes per sample is PER CHANNEL, so that it's consistent whether stereo or mono.
//  For stereo, the actual bytes per sample time will be twice as much.
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
//   CLASS: TCIDAudStreamFmt
//  PREFIX: asfmt
// ---------------------------------------------------------------------------
class CIDAUDSTREAMEXP TCIDAudStreamFmt

    : public TObject, public MStreamable, public MFormattable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDAudStreamFmt
        (
            const   tCIDLib::TCard4         c4Channels
            , const tCIDLib::TCard4         c4SampleRate
            , const tCIDLib::TCard4         c4BytesPerSample    // PER CHANNEL!
        );

        TCIDAudStreamFmt(const TCIDAudStreamFmt&) = default;
        TCIDAudStreamFmt(TCIDAudStreamFmt&&) = default;

        ~TCIDAudStreamFmt() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDAudStreamFmt& operator=(const TCIDAudStreamFmt&) = default;
        TCIDAudStreamFmt& operator=(TCIDAudStreamFmt&&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4BytesPerSample() const
        {
            return m_c4BytesPerSample;
        }

        tCIDLib::TCard4 c4BytesPerSampleTime() const
        {
            return m_c4BytesPerSample * m_c4Channels;
        }

        tCIDLib::TCard4 c4BytesPerSecond() const
        {
            return m_c4BytesPerSecond;
        }

        tCIDLib::TCard4 c4Channels() const
        {
            return m_c4Channels;
        }

        tCIDLib::TCard4 c4SampleRate() const
        {
            return m_c4SampleRate;
        }

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard4         c4Channels
            , const tCIDLib::TCard4         c4SampleRate
            , const tCIDLib::TCard4         c4BytesPerSample    // PER CHANNEL!
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid FormatTo
        (
                    TTextOutStream&         strmDest
        )   const final;

        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   final;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const final;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ValidateFmt
        (
            const   tCIDLib::TCard4         c4Channels
            , const tCIDLib::TCard4         c4SampleRate
            , const tCIDLib::TCard4         c4BytesPerSample
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4BytesPerSample
        //      The number of bytes per channel for a single channel. For a stereo
        //      format, the actual number of bytes per sample time is twice this.
        //
        //  m_c4BytesPerSecond
        //      The bytes per second (for all channels.) This is pre-calculated when our
        //      format is set.
        //
        //  m_c4Channels
        //      The number of channels, either 1 or 2 for now.
        //
        //  m_c4SampleRate
        //      The number of samples per second.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4BytesPerSample = 1;
        tCIDLib::TCard4     m_c4BytesPerSecond = kCIDLib::c4Sz_8K;
        tCIDLib::TCard4     m_c4Channels = 1;
        tCIDLib::TCard4     m_c4SampleRate = kCIDLib::c4Sz_8K;


        // -------------------------------------------------------------------
        //  Only available to stream factory for streaming purposes
        // -------------------------------------------------------------------
        TCIDAudStreamFmt();


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDAudStreamFmt, TObject)
        BefriendFactory(TCIDAudioStrmFmt)
};


#pragma CIDLIB_POPPACK
