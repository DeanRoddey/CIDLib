//
// FILE NAME: CIDKernel_Audio.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/21/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDKernel_Audio.Cpp file. This file implements
//  the TKrnlAudio class, which encapsulates the system's very basic
//  audio capabilities, i.e. just the basic ability to make beeps, bonks, and
//  alarms. The implementation for this class just needs to do the best
//  it can do on its platform, given what support is available.
//
//  This class is fully static, but its not done as a namespace because it
//  needs to have private stuff shared between the platform dependent and
//  independent implementation files. So the constructors are not available so
//  that you have to treat it like a namespace.
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
//   CLASS: TKrnlAudio
//  PREFIX: kaudio
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlAudio
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bAudioCue
        (
            const   tCIDLib::EAudioCues     eCue
        );

        static tCIDLib::TBoolean bAudioSupport();

        static tCIDLib::TBoolean bEnumAudioDevices
        (
                    tCIDLib::TCh* const     pszToFill
            , const tCIDLib::TCard4         c4BufLen
        );

        static tCIDLib::TBoolean bEnumRecEndPoints
        (
                    TKrnlLList<TKrnlKVPair>& kllstToFill
        );

        static tCIDLib::TBoolean bMuteSystemAudio
        (
            const   tCIDLib::TBoolean       bMute
        );

        static tCIDLib::TBoolean bPlayWAVFile
        (
            const   tCIDLib::TCh* const     pszFileToPlay
            , const tCIDLib::EWaitModes     eWait = tCIDLib::EWaitModes::Wait
        );

        static tCIDLib::TBoolean bQuerySystemVolume
        (
                    tCIDLib::TCard4&        c4ToFill
        );

        static tCIDLib::TBoolean bSetSystemVolume
        (
            const   tCIDLib::TCard4         c4VolPercent
        );

        static tCIDLib::TBoolean bSetWAVForCue
        (
            const   tCIDLib::EAudioCues     eCue
            , const tCIDLib::TCh* const     pszFileForCue
        );

        static tCIDLib::TVoid Beep
        (
            const   tCIDLib::TCard4         c4Frequency
            , const tCIDLib::TCard4         c4Duration
        );

        static const tCIDLib::TCh* pszDevInfo();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlAudio() = delete;

        TKrnlAudio(const TKrnlAudio&) = delete;

        ~TKrnlAudio();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlAudio& operator=(const TKrnlAudio&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TCIDKrnlModule;


    private :
        // -------------------------------------------------------------------
        //  Private, static data members
        //
        //  s_apszCueFiles
        //      A list of file names that represent the Wav files that will
        //      be played for a particular audio cue.
        //
        //  s_bAudioSupport
        //      Set to indicate whether this workstation has any audio support
        //      (beyond just a system speaker, i.e. it can play wav files.)
        //
        //  s_pszDevInfo
        //      A string describing the device we selected, if any.
        // -------------------------------------------------------------------
        static tCIDLib::TCh*        s_apszCueFiles[tCIDLib::TCard4(tCIDLib::EAudioCues::Count)];
        static  tCIDLib::TBoolean   s_bAudioSupport;
        static  tCIDLib::TCh*       s_pszDevInfo;
};

#pragma CIDLIB_POPPACK


