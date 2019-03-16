//
// FILE NAME: CIDLib_Audio.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/08/1993
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  TAudio is a namespace that does various beeps, bonks, zings, and
//  honks all of which are given enumerated values. It also supports basic
//  playing of Wav files.
//
//  This stuff is built on top of TKrnlAudio, which encapsulates the system
//  audio services. He will sense whether the workstation has audio services
//  or not and use them if there. If not there, it will use the system speaker
//  where possible, basically just doing the best it can given the issues
//  involved.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace TAudio
{
    CIDLIBEXP tCIDLib::TVoid AudioCue
    (
        const   tCIDLib::EAudioCues     eCue
    );

    CIDLIBEXP tCIDLib::TVoid Beep
    (
        const   tCIDLib::TCard4         c4Frequency
        , const tCIDLib::TCard4         c4Duration
    );

    CIDLIBEXP tCIDLib::TCard4 c4EnumAudioDevices
    (
                tCIDLib::TKVPCollect&   colToFill
    );

    CIDLIBEXP tCIDLib::TVoid MuteSystemAudio
    (
        const   tCIDLib::TBoolean       bToSet
    );

    CIDLIBEXP tCIDLib::TVoid PlayWAVFile
    (
        const   TString&                strFileToPlay
        , const tCIDLib::EWaitModes     eWait = tCIDLib::EWaitModes::Wait
    );

    CIDLIBEXP tCIDLib::TVoid QuerySystemVolume
    (
                tCIDLib::TCard4&        c4ToFill
    );

    CIDLIBEXP tCIDLib::TVoid SetWAVForCue
    (
        const   tCIDLib::EAudioCues     eCue
        , const TString&                strFileForCue
    );

    CIDLIBEXP TString strDevInfo();

    CIDLIBEXP tCIDLib::TVoid SetSystemVolume
    (
        const   tCIDLib::TCard4         c4Percent
    );
}
