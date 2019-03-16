//
// FILE NAME: CIDKernel_Speech.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/17/2004
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
//  This is the header for the CIDKernel_Speech.Cpp file. This file
//  implements the TKrnlSpeech class, which wraps the operating system's text
//  to speech and speech recognition type functionality.
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
//   CLASS: TKrnlSpeech
//  PREFIX: kspch
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlSpeech
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bQueryOutputList
        (
                    TKrnlLList<TKrnlKVPair>& kllstOutputs
        );

        static tCIDLib::TBoolean bQueryVoiceList
        (
                    TKrnlLList<TKrnlString>& kllstVoices
        );

        static tCIDLib::TBoolean bSpeakToFile
        (
            const   tCIDLib::TCh* const     pszToSay
            , const tCIDLib::TCh* const     pszTarFile
            , const tCIDLib::EAudioFmts     eFormat = tCIDLib::EAudioFmts::F16Khz16BitMono
            , const tCIDLib::TCh* const     pszVoice = 0
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlSpeech();

        ~TKrnlSpeech();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bSetOutput
        (
            const   tCIDLib::TCh* const     pszOutputId
        );

        tCIDLib::TBoolean bSetVoice
        (
            const   tCIDLib::TCh* const     pszVoiceToSet
        );

        tCIDLib::TBoolean bSpeak
        (
            const   tCIDLib::TCh* const     pszToSay
            , const tCIDLib::ESpeechFlags   eFlags
        );

        tCIDLib::TBoolean bSpeechActive() const;

        tCIDLib::TBoolean bWaitTillDone
        (
            const   tCIDLib::TCard4         c4WaitFor
            ,       tCIDLib::TBoolean&      bTimedOut
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pData
        //      Each platform will allocate some type of instance data for
        //      each speech object. We look at is as a generic void pointer
        //      here.
        // -------------------------------------------------------------------
        tCIDLib::TVoid* m_pData;
};

#pragma CIDLIB_POPPACK

