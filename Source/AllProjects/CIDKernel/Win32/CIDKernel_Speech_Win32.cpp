//
// FILE NAME: CIDKernel_Speech_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/17/2004
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
//  This file provides the Win32 specific implementation for the class
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
//  Force off deprecation warnings for the SD headers
// ---------------------------------------------------------------------------
#pragma     warning(push)
#pragma     warning(disable : 4996)
#include    <sapi.h>
#include    <sphelper.h>
#pragma     warning(pop)



// ---------------------------------------------------------------------------
//  Local helpers
// ---------------------------------------------------------------------------

//
//  Creates a voice object if the passed pointer is null, else it casts it
//  to the right type. We expose the pointer to the voice object in our
//  hpp file as just a void pointer because COM headers are not allowed
//  publically.
//
//  So the below methods call this guy any time they need to use their
//  voice object, either getting a new one or getting the one they already
//  previously created.
//
static ISpVoice* pcomMakeVoiceObj(tCIDLib::TVoid*& pData)
{
    ISpVoice* pcomRet;
    if (!pData)
    {
        // Try to create our instance
        HRESULT hRes = ::CoCreateInstance
        (
            CLSID_SpVoice
            , NULL
            , CLSCTX_INPROC_SERVER
            , IID_ISpVoice
            , (LPVOID*)&pcomRet
        );

        if (!SUCCEEDED(hRes))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            return 0;
        }

        // And give it back as a void pointer
        pData = pcomRet;
    }
     else
    {
        // We already had it, so cast it to the real type
        pcomRet = reinterpret_cast<ISpVoice*>(pData);
    }
    return pcomRet;
}



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
    // Flush both lists in preparation for filling them in
    kllstOutputs.RemoveAll();

    // Get the number of devices available
    IEnumSpObjectTokens* pEnum = nullptr;
    HRESULT hRes = ::SpEnumTokens(SPCAT_AUDIOOUT, NULL, NULL, &pEnum);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    TCOMJanitor<IEnumSpObjectTokens> janList(&pEnum);

    // WE got the list so iterate it. We need to get the count
    ULONG ulCount;
    hRes = pEnum->GetCount(&ulCount);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // If we got any, load them up
    for (ULONG ulIndex = 0; ulIndex < ulCount; ulIndex++)
    {
        ISpObjectToken* pToken;
        hRes = pEnum->Next(1, &pToken, NULL);
        if (SUCCEEDED(hRes))
        {
            tCIDLib::TCh* pszId = nullptr;
            tCIDLib::TCh* pszName = nullptr;
            if (SUCCEEDED(pToken->GetId(&pszId)))
                ::SpGetDescription(pToken, &pszName);

            if (pszId && pszName)
                kllstOutputs.pobjAddNew(new TKrnlKVPair(pszId, pszName));

            if (pszId)
                ::CoTaskMemFree(pszId);
            if (pszName)
                ::CoTaskMemFree(pszName);

            pToken->Release();
        }
    }

    return !kllstOutputs.bIsEmpty();
}


// Query the list of available voices
tCIDLib::TBoolean
TKrnlSpeech::bQueryVoiceList(TKrnlLList<TKrnlString>& kllstVoices)
{
    // Flush the incoming list
    kllstVoices.RemoveAll();

    // Try to create a voice instance
    ISpVoice* pcomVoice;
    HRESULT hRes = ::CoCreateInstance
    (
        CLSID_SpVoice
        , NULL
        , CLSCTX_INPROC_SERVER
        , IID_ISpVoice
        , (LPVOID*)&pcomVoice
    );

    if (!SUCCEEDED(hRes))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return 0;
    }
    TCOMJanitor<ISpVoice> janVoice(&pcomVoice);

    // Create an enumerator object
    IEnumSpObjectTokens* pcomEnum = 0;

    // Get the enumeration now
    hRes = ::SpEnumTokens(SPCAT_VOICES, 0, 0, &pcomEnum);
    if (!SUCCEEDED(hRes))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    TCOMJanitor<IEnumSpObjectTokens> janEnum(&pcomEnum);

    ISpObjectToken* pToken;
    tCIDLib::TCh*   pszName;
    while (pcomEnum->Next(1, &pToken, NULL) == S_OK)
    {
        ::SpGetDescription(pToken, &pszName);
        kllstVoices.pobjAddNew(new TKrnlString(pszName));
        ::CoTaskMemFree(pszName);
        pToken->Release();
        pToken = 0;
    }
    return kCIDLib::True;
}


//
//  This one is a special case and doesn't use the voice object of this
//  speech object. It's a one shot deal that creates a voice object, binds
//  it to a file stream, and optionally sets a voice on it. It then
//  synchronously does the output to the file then cleans everything up.
//
//  The caller then can do what it wants with the file.
//
tCIDLib::TBoolean
TKrnlSpeech::bSpeakToFile(  const   tCIDLib::TCh* const     pszToSay
                            , const tCIDLib::TCh* const     pszTarFile
                            , const tCIDLib::EAudioFmts     eFormat
                            , const tCIDLib::TCh* const     pszVoice)
{
    ISpVoice* pcomTar;

    // Try to create our voice instance
    HRESULT hRes = ::CoCreateInstance
    (
        CLSID_SpVoice, NULL, CLSCTX_INPROC_SERVER, IID_ISpVoice, (LPVOID*)&pcomTar
    );

    if (!SUCCEEDED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcTTS_Create, ::GetLastError());
        return kCIDLib::False;
    }

    // It workd so put a janitor on the voice object
    TCOMJanitor<ISpVoice> janVoice(&pcomTar);

    //
    //  If there is a non-default voice, try to set that. If this fails, we just keep
    //  going and they should get the default voice.
    //
    if (pszVoice != 0)
    {
        // Create an enumerator object
        HRESULT hRes;
        IEnumSpObjectTokens* pcomEnum = 0;

        // Get the enumerator for voices
        hRes = ::SpEnumTokens(SPCAT_VOICES, 0, 0, &pcomEnum);
        if (SUCCEEDED(hRes))
        {
            TCOMJanitor<IEnumSpObjectTokens> janEnum(&pcomEnum);

            // Search for one with the passed name
            tCIDLib::TBoolean   bDone = kCIDLib::False;
            ISpObjectToken*     pToken;
            tCIDLib::TCh*       pszName;
            while (!bDone && (pcomEnum->Next(1, &pToken, NULL) == S_OK))
            {
                // Get the name of this one
                ::SpGetDescription(pToken, &pszName);

                //
                //  If this is our guy, set the voice and remember result. If it fails
                //  we don't care, just continue. They should get the default voice.
                //
                if (TRawStr::bCompareStr(pszVoice, pszName))
                {
                    pcomTar->SetVoice(pToken);
                    bDone = kCIDLib::True;
                }

                ::CoTaskMemFree(pszName);
                pToken->Release();
                pToken = 0;
            }
        }
    }

    //
    //  OK, let's now set up the output stream and audio format, which
    //  we set to be just good enough for audio purposes. Map the incoming
    //  audio format to the platform type.
    //
    SPSTREAMFORMAT W32Fmt;
    switch(eFormat)
    {
        case tCIDLib::EAudioFmts::F8Khz16BitMono :
            W32Fmt = SPSF_8kHz16BitMono;
            break;

        case tCIDLib::EAudioFmts::F8Khz16BitStereo :
            W32Fmt = SPSF_8kHz16BitStereo;
            break;

        case tCIDLib::EAudioFmts::F12Khz16BitMono :
            W32Fmt = SPSF_12kHz16BitMono;
            break;

        case tCIDLib::EAudioFmts::F12Khz16BitStereo :
            W32Fmt = SPSF_12kHz16BitStereo;
            break;

        case tCIDLib::EAudioFmts::F16Khz16BitMono :
            W32Fmt = SPSF_16kHz16BitMono;
            break;

        case tCIDLib::EAudioFmts::F16Khz16BitStereo :
            W32Fmt = SPSF_16kHz16BitStereo;
            break;

        case tCIDLib::EAudioFmts::F22Khz16BitMono :
            W32Fmt = SPSF_16kHz16BitStereo;
            break;

        case tCIDLib::EAudioFmts::F22Khz16BitStereo :
            W32Fmt = SPSF_16kHz16BitStereo;
            break;

        case tCIDLib::EAudioFmts::F44Khz16BitMono :
            W32Fmt = SPSF_16kHz16BitStereo;
            break;

        case tCIDLib::EAudioFmts::F44Khz16BitStereo :
            W32Fmt = SPSF_16kHz16BitStereo;
            break;

        case tCIDLib::EAudioFmts::F48Khz16BitMono :
            W32Fmt = SPSF_16kHz16BitStereo;
            break;

        case tCIDLib::EAudioFmts::F48Khz16BitStereo :
            W32Fmt = SPSF_16kHz16BitStereo;
            break;

        case tCIDLib::EAudioFmts::Default :
        default :
            W32Fmt = SPSF_Default;
            break;
    };

    // OK, set up the stream format object
    CSpStreamFormat Fmt;
    Fmt.AssignFormat(W32Fmt);

    // Try to bind a stream to the file
    ISpStream* pStream = 0;
    hRes = SPBindToFile
    (
        pszTarFile, SPFM_CREATE_ALWAYS, &pStream, &Fmt.FormatId(), Fmt.WaveFormatExPtr()
    );
    if (!SUCCEEDED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcTTS_CreateStream, GetLastError());
        return kCIDLib::False;
    }

    TCOMJanitor<ISpStream> janStream(&pStream);

    // Try to set the stream on the voice object
    hRes = pcomTar->SetOutput(pStream, TRUE);
    if (!SUCCEEDED(hRes))
    {
        TKrnlError::SetLastHostError(GetLastError());
        return kCIDLib::False;
    }

    //
    //  OK, let's try to do the actual speech now. We do it synchronously,
    //  since we are just doing it to a file not actually speaking it.
    //
    hRes = pcomTar->Speak(pszToSay, SPF_DEFAULT, 0);
    if (!SUCCEEDED(hRes))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcTTS_Speak, GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlSpeech: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlSpeech::TKrnlSpeech() :

    m_pData(0)
{
}

TKrnlSpeech::~TKrnlSpeech()
{
    // If we allocated the COM object, then clean it up
    if (m_pData)
    {
        ISpVoice* pcomThis = reinterpret_cast<ISpVoice*>(m_pData);
        pcomThis->Release();
        m_pData = 0;
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
    // Get or fault in the voice object
    ISpVoice* pcomThis = pcomMakeVoiceObj(m_pData);

    // If not, then probably no local support so return failure
    if (!pcomThis)
        return kCIDLib::False;

    // Use a helper to get the token from the id we were given, so we don't have to search
    ISpObjectToken* pToken;
    HRESULT hRes = ::SpGetTokenFromId(pszOutputId, &pToken, FALSE);
    if (FAILED(hRes))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    TCOMJanitor<ISpObjectToken> janToken(&pToken);

    if (SUCCEEDED(pcomThis->SetOutput(pToken, TRUE)))
        return kCIDLib::True;

    return kCIDLib::False;
}


//
//  Set's the voice to use for speech output. If not called, it will be the default
//
tCIDLib::TBoolean TKrnlSpeech::bSetVoice(const tCIDLib::TCh* const pszToSet)
{
    // Get or fault in the voice object
    ISpVoice* pcomThis = pcomMakeVoiceObj(m_pData);

    // If not, then probably no local support so return failure
    if (!pcomThis)
        return kCIDLib::False;

    // Create an enumerator object
    HRESULT hRes;
    IEnumSpObjectTokens* pcomEnum = 0;

    // Get the enumeration now
    hRes = ::SpEnumTokens(SPCAT_VOICES, 0, 0, &pcomEnum);
    if (!SUCCEEDED(hRes))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Search for one with the passed name
    ISpObjectToken*     pToken;
    tCIDLib::TCh*       pszName;
    tCIDLib::TBoolean   bRes = kCIDLib::False;
    tCIDLib::TBoolean   bFound = kCIDLib::False;
    while (!bFound && (pcomEnum->Next(1, &pToken, NULL) == S_OK))
    {
        // Get the name of this one
        ::SpGetDescription(pToken, &pszName);

        // If this is our guy, set the voice and remember result
        if (TRawStr::bCompareStr(pszToSet, pszName))
        {
            bRes = (pcomThis->SetVoice(pToken) == S_OK);
            bFound = kCIDLib::True;
        }
        ::CoTaskMemFree(pszName);
        pToken->Release();
        pToken = 0;
    }
    pcomEnum->Release();
    return bRes;
}


tCIDLib::TBoolean
TKrnlSpeech::bSpeak(const   tCIDLib::TCh* const     pszToSay
                    , const tCIDLib::ESpeechFlags   eFlags)
{
    HRESULT hRes;

    // Get or fault in the voice object
    ISpVoice* pcomThis = pcomMakeVoiceObj(m_pData);

    // If not, then probably no local support so return failure
    if (!pcomThis)
        return kCIDLib::False;

    // Set up the flags
    tCIDLib::TCard4 c4Flags = SPF_DEFAULT;
    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::ESpeechFlags::Async))
        c4Flags |= SPF_ASYNC;
    if (tCIDLib::bAllBitsOn(eFlags, tCIDLib::ESpeechFlags::Markup))
        c4Flags |= SPF_IS_XML;

    hRes = pcomThis->Speak(pszToSay, c4Flags, 0);
    if (!SUCCEEDED(hRes))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Returns true if there speech going on on this object
tCIDLib::TBoolean TKrnlSpeech::bSpeechActive() const
{
    // If not used yet, then obviously not
    if (!m_pData)
        return kCIDLib::False;

    // Cast it to the actual type
    ISpVoice* pcomRet = reinterpret_cast<ISpVoice*>(m_pData);

    // We just use the wait until done method with a zero timeout
    return (pcomRet->WaitUntilDone(0) == S_FALSE);
}


// Wait for any TTS queued up by bSpeak above to finish
tCIDLib::TBoolean
TKrnlSpeech::bWaitTillDone( const   tCIDLib::TCard4     c4WaitFor
                            ,       tCIDLib::TBoolean&  bTimedOut)
{
    //
    //  If the object isn't allocated, we cannot have any pending, so just
    //  return immediately with success.
    //
    if (!m_pData)
    {
        bTimedOut = kCIDLib::False;
        return kCIDLib::True;
    }

    // Cast it to the actual type and do the call
    ISpVoice* pcomThis = reinterpret_cast<ISpVoice*>(m_pData);
    bTimedOut = (pcomThis->WaitUntilDone(c4WaitFor) != S_OK);
    return kCIDLib::True;
}


