//
// FILE NAME: CIDAudStream_AudioFmt.cpp
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
//  The out of line code for the TCIDAudStreamFmt class.
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
#include    "CIDAudStream_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDAudStreamFmt,TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDAudStream_AudioFmt
    {
        constexpr tCIDLib::TCard2   c2FmtVersion = 1;
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TCIDAudStreamFmt
//  PREFIX: cass
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDAudStreamFmt: Constructors
// ---------------------------------------------------------------------------
TCIDAudStreamFmt::TCIDAudStreamFmt( const   tCIDLib::TCard4 c4Channels
                                    , const tCIDLib::TCard4 c4SampleRate
                                    , const tCIDLib::TCard4 c4BytesPerSample)
{
    // Validate the values and calcualte any derived values
    ValidateFmt(c4Channels, c4SampleRate, c4BytesPerSample);
}


// ---------------------------------------------------------------------------
//  TCIDAudStreamFmt: Public, non-virtual methods
// ---------------------------------------------------------------------------

// If the values are not valid, then nothing will be changed
tCIDLib::TVoid TCIDAudStreamFmt::Set(const  tCIDLib::TCard4 c4Channels
                                    , const tCIDLib::TCard4 c4SampleRate
                                    , const tCIDLib::TCard4 c4BytesPerSample)
{
    ValidateFmt(c4Channels, c4SampleRate, c4BytesPerSample);
}


// ---------------------------------------------------------------------------
//  TCIDAudStreamFmt: protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDAudStreamFmt::FormatTo(TTextOutStream& strmDest) const
{
    strmDest    << L"{Channels=" << m_c4Channels
                << L", Rate=" << m_c4SampleRate
                << L", Sample Bytes=" << m_c4BytesPerSample << L'}';
}

tCIDLib::TVoid TCIDAudStreamFmt::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Make sure we get the start marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (c2FmtVersion != CIDAudStream_AudioFmt::c2FmtVersion)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , clsThis()
        );
    }

    // Looks ok, so stream our other stuff in
    strmToReadFrom  >> m_c4Channels
                    >> m_c4SampleRate
                    >> m_c4BytesPerSample;

    // And it should end with an end marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TCIDAudStreamFmt::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDAudStream_AudioFmt::c2FmtVersion
                    << m_c4Channels
                    << m_c4SampleRate
                    << m_c4BytesPerSample
                    << tCIDLib::EStreamMarkers::EndObject;
}


// ---------------------------------------------------------------------------
//  TCIDAudStreamFmt: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Validates the format values and, if valid, stores them and calculates any derivedc
//  members. Throws if bad. This doesn't guarantee any particular device or standard
//  will support these settings, just that they are reasonable settings likely to be
//  supported.
//
tCIDLib::TVoid TCIDAudStreamFmt::ValidateFmt(const  tCIDLib::TCard4 c4Channels
                                            , const tCIDLib::TCard4 c4SampleRate
                                            , const tCIDLib::TCard4 c4BytesPerSample)
{
    if ((c4Channels < 1) || (c4Channels > 2))
    {
        facCIDAudStream().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kAudStrErrs::errcStrm_BadChannels
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c4Channels)
        );
    }

    if ((c4BytesPerSample < 1) || (c4BytesPerSample > 4))
    {
        facCIDAudStream().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kAudStrErrs::errcStrm_BadBytesPerSample
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c4Channels)
        );
    }

    switch(c4SampleRate)
    {
        case 8000 :
        case 16000 :
        case 44100 :
        case 88200 :
        case 96000 :
            break;

        default :
        {
            facCIDAudStream().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kAudStrErrs::errcStrm_BadSampleRate
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(c4Channels)
            );
            break;
        }
    };

    m_c4Channels = c4Channels;
    m_c4SampleRate = c4SampleRate;
    m_c4BytesPerSample = c4BytesPerSample;
    m_c4BytesPerSecond = c4Channels * c4SampleRate * c4BytesPerSample;
}

