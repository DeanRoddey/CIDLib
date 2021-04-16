//
// FILE NAME: CIDKernel_Speech_Linux.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/19/2019
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
//  This file provides the Linux specific implementation for the class
//  TKrnlSpeech.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"
#include    "CIDKernel_InternalHelpers_.hpp"




// ---------------------------------------------------------------------------
//   CLASS: TKrnlSpeech
//  PREFIX: kspch
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlSpeech: Public, static methods
// ---------------------------------------------------------------------------

//
//  Query the list of available speech outputs. We return a list of key/value pairs.
//  The key is the unique id and the value is the display name.
//
tCIDLib::TBoolean
TKrnlSpeech::bQueryOutputList(TKrnlLList<TKrnlKVPair>& kllstOutputs)
{
    // Flush the list in preparation for filling it in
    kllstOutputs.RemoveAll();
    return !kllstOutputs.bIsEmpty();
}


// Query the list of available voices
tCIDLib::TBoolean
TKrnlSpeech::bQueryVoiceList(TKrnlLList<TKrnlString>& kllstVoices)
{
    // Flush the incoming list
    kllstVoices.RemoveAll();
    return kCIDLib::True;
}


//
//  This one is a special case and doesn't use the actual void, it instead
//  speaks it to a file as audio.
//
tCIDLib::TBoolean
TKrnlSpeech::bSpeakToFile(  const   tCIDLib::TCh* const     pszToSay
                            , const tCIDLib::TCh* const     pszTarFile
                            , const tCIDLib::EAudioFmts     eFormat
                            , const tCIDLib::TCh* const     pszVoice)
{
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TKrnlSpeech: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSpeech::TKrnlSpeech() :

    m_pData(nullptr)
{
}

TKrnlSpeech::~TKrnlSpeech()
{
    // If we allocated our per-platform object, clean it up
    if (m_pData)
    {
        m_pData = nullptr;
    }
}


// ---------------------------------------------------------------------------
//  TKrnlSpeech: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We iterate the list of available outputs. If one matches this id, then we set that
//  as the output for the current voice.
//
tCIDLib::TBoolean TKrnlSpeech::bSetOutput(const tCIDLib::TCh* const pszOutputId)
{
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


//
//  Set's the voice to use for speech output. If not called, it will be the default
//
tCIDLib::TBoolean TKrnlSpeech::bSetVoice(const tCIDLib::TCh* const pszToSet)
{
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlSpeech::bSpeak(const   tCIDLib::TCh* const     pszToSay
                    , const tCIDLib::ESpeechFlags   eFlags)
{
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


// Returns true if there speech going on on this object
tCIDLib::TBoolean TKrnlSpeech::bSpeechActive() const
{
    return kCIDLib::False;
}


// Wait for any TTS queued up by bSpeak above to finish
tCIDLib::TBoolean
TKrnlSpeech::bWaitTillDone( const   tCIDLib::TCard4     c4WaitFor
                            ,       tCIDLib::TBoolean&  bTimedOut)
{
    return kCIDLib::True;
}


