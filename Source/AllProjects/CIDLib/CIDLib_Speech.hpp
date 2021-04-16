//
// FILE NAME: CIDLib_Speech.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/18/2004
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
//  This header defines the TSpeech class, which supports speech related
//  system functionality.
//
//  For voices and audio outputs you can query them and use those to set a particular
//  voice or output. For voices we just use the name and go back later and try to find
//  if it is set by client code. For outputs, we reutrn a key/value pair where the key
//  is a unique id and the value is the display name. Pass back in the unique id later
//  to set that output.
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
//   CLASS: TSpeech
//  PREFIX: spch
// ---------------------------------------------------------------------------
class CIDLIBEXP TSpeech : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid QuerySpeechOutputs
        (
                    tCIDLib::TKVPCollect&   colToFill
        );

        static tCIDLib::TVoid QueryVoiceList
        (
                    tCIDLib::TStrCollect&   colToFill
        );

        static tCIDLib::TVoid SpeakToFile
        (
            const   TString&                strToSay
            , const TString&                strTarFile
            , const tCIDLib::EAudioFmts     eFormat
            , const TString&                strVoice
        );



        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSpeech();

        TSpeech(const TSpeech&) = delete;
        TSpeech(TSpeech&&) = delete;

        ~TSpeech();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSpeech& operator=(const TSpeech&) = delete;
        TSpeech& operator=(TSpeech&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bSpeechActive() const;

        tCIDLib::TBoolean bWaitTillDone
        (
            const   tCIDLib::TCard4         c4WaitFor
        );

        tCIDLib::TVoid SetOutput
        (
            const   TString&                strOutputId
        );

        tCIDLib::TVoid SetVoice
        (
            const   TString&                strVoiceName
        );

        tCIDLib::TVoid Speak
        (
            const   TString&                strToSay
            , const tCIDLib::ESpeechFlags   eFlags = tCIDLib::ESpeechFlags::None
        );


    protected   :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_kspchImpl
        //      Our kernel level speech services wrapper class that provides the
        //      actual functionality.
        //
        //  m_strOutputId
        //      This is empty until the client code calls SetOutput. If that is
        //      succcessful, we store the output id here. If any later calls to SetVoice
        //      are done, we'll try to re-apply this to the new voice so that the client
        //      code doesn't have to remember to re-apply it.
        // -------------------------------------------------------------------
        TKrnlSpeech m_kspchImpl;
        TString     m_strOutputId;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSpeech,TObject)
};


#pragma CIDLIB_POPPACK


