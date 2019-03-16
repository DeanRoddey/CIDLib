//
// FILE NAME: CIDKernel_Audio.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/21/1998
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
//  This file provides the platform independent implementation of the
//  TKrnlAudio class, which provides host abstraction services for audio
//  capabilities. The platform dependent parts are in the per-platform
//  directories in the CIDKernel_Audio_Win32.Cpp file.
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
#include    "CIDKernel_.hpp"


// ---------------------------------------------------------------------------
//  TKrnlAudio: Static data
// ---------------------------------------------------------------------------
tCIDLib::TCh*       TKrnlAudio::s_apszCueFiles[tCIDLib::c4EnumOrd(tCIDLib::EAudioCues::Count)];
tCIDLib::TBoolean   TKrnlAudio::s_bAudioSupport;
tCIDLib::TCh*       TKrnlAudio::s_pszDevInfo;


// ---------------------------------------------------------------------------
//  TKrnlAudio: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlAudio::bAudioCue(const tCIDLib::EAudioCues eCue)
{
    #if CID_DEBUG_ON
    if ((eCue < tCIDLib::EAudioCues::Min) || (eCue > tCIDLib::EAudioCues::Max))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcAudio_BadCue);
        return kCIDLib::False;
    }
    #endif

    //
    //  If we have audio support and there is a Wav file for this cue, then
    //  play the Wav file. Otherwise, do the speaker.
    //
    if (s_bAudioSupport && s_apszCueFiles[tCIDLib::c4EnumOrd(eCue)])
        return bPlayWAVFile(s_apszCueFiles[tCIDLib::c4EnumOrd(eCue)]);

    switch(eCue)
    {
        case tCIDLib::EAudioCues::Alert :
            //
            //  Do a high-low pulse kind of like a british siren
            //
            Beep(880, 50);
            Beep(440, 50);
            break;

        case tCIDLib::EAudioCues::Bell :
            //
            //  Do a high tone followed by a fast series of slightly
            //  lower oscillating frequencies.
            //
            Beep(1000, 10);
            Beep(990, 50);
            Beep(980, 50);
            break;

        case tCIDLib::EAudioCues::Chime :
            //
            //  Do a short medium high tone followed by a longer
            //  slightly lower tone.
            //
            Beep(880, 10);
            Beep(850, 50);
            break;

        case tCIDLib::EAudioCues::Error :
            //
            //  Do two hard pulses with a small pause. Put them at a
            //  medium high frequency so that system speakers will
            //  carry them well.
            //
            Beep(500, 100);
            TKrnlThread::Sleep(25);
            Beep(500, 100);
            break;

        case tCIDLib::EAudioCues::HiPulse :
        case tCIDLib::EAudioCues::Information :
            //
            //  Do a short pulse at 880
            //
            Beep(880, 50);
            break;

        case tCIDLib::EAudioCues::LowPulse :
            Beep(440, 75);
            break;

        case tCIDLib::EAudioCues::MedPulse :
            Beep(660, 60);
            break;

        case tCIDLib::EAudioCues::Rising :
            Beep(400, 50);
            Beep(440, 50);
            Beep(480, 50);
            Beep(520, 50);
            Beep(580, 50);
            break;

        case tCIDLib::EAudioCues::Sinking :
            Beep(580, 50);
            Beep(520, 50);
            Beep(480, 50);
            Beep(440, 50);
            Beep(400, 50);
            break;

        case tCIDLib::EAudioCues::Question :
            //
            //  Do three pulses moving upward in frequency
            //
            Beep(880, 25);
            Beep(900, 25);
            Beep(920, 25);
            break;

        case tCIDLib::EAudioCues::Click :
            //
            //  Do a very short tone at a high frequency
            //
            Beep(880,10);
            break;

        case tCIDLib::EAudioCues::Warning :
            //
            //  Do two short pulses at 880 with a small pause between.
            //
            Beep(880, 25);
            TKrnlThread::Sleep(25);
            Beep(880, 25);
            break;


        default :
            #if CID_DEBUG_ON
            kmodCIDKernel.ErrorPopUp
            (
                kKrnlErrs::errcAudio_BadCue
                , CID_FILE
                , CID_LINE
            );
            #endif
            return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlAudio::bAudioSupport()
{
    return s_bAudioSupport;
}


tCIDLib::TBoolean
TKrnlAudio::bSetWAVForCue(  const   tCIDLib::EAudioCues eCue
                            , const tCIDLib::TCh* const pszFileForCue)
{
    #if CID_DEBUG_ON
    if ((eCue < tCIDLib::EAudioCues::Min) || (eCue > tCIDLib::EAudioCues::Max))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcAudio_BadCue);
        return kCIDLib::False;
    }
    #endif

    // Free any existing name in this slot
    const tCIDLib::TCard4 c4Index = tCIDLib::c4EnumOrd(eCue);
    delete [] s_apszCueFiles[c4Index];

    // Store a copy of the file name in the local list
    if (pszFileForCue)
        s_apszCueFiles[c4Index] = TRawStr::pszReplicate(pszFileForCue);
    else
        s_apszCueFiles[c4Index] = nullptr;

    return kCIDLib::True;
}


const tCIDLib::TCh* TKrnlAudio::pszDevInfo()
{
    return s_pszDevInfo;
}


