//
// FILE NAME: CIDLib_Speech.cpp
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
//  This file implements the speech support class. It just delegates everything
//  to the kernel level implementation object.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TSpeech,TObject)


// ---------------------------------------------------------------------------
//   CLASS: TSpeech
//  PREFIX: spch
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSpeech: Public, static methods
// ---------------------------------------------------------------------------

// Query the list of available speech outputs
tCIDLib::TVoid TSpeech::QuerySpeechOutputs(tCIDLib::TKVPCollect& colToFill)
{
    // Get a kernel level list of outputs
    TKrnlLList<TKrnlKVPair> kllstOutputs;
    if (!TKrnlSpeech::bQueryOutputList(kllstOutputs))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcSpeech_QueryOutputs
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // We got them so copy them over to the caller's CIDLib level collection
    colToFill.RemoveAll();

    TKrnlKVPair* pkkvpCur = nullptr;
    TKeyValuePair kvalCur;
    if (kllstOutputs.bResetCursor())
    {
        while (kllstOutputs.bNext(pkkvpCur))
        {
            kvalCur.Set(pkkvpCur->pszKey(), pkkvpCur->pszValue());
            colToFill.objAdd(kvalCur);
        }
    }
}


// Query the list of available voice names
tCIDLib::TVoid TSpeech::QueryVoiceList(tCIDLib::TStrCollect& colToFill)
{
    // Get a kernel level list of voices
    TKrnlLList<TKrnlString> kllstVoices;
    if (!TKrnlSpeech::bQueryVoiceList(kllstVoices))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcSpeech_QueryVoices
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Copy the values over the caller's CIDLib level string collection
    colToFill.RemoveAll();

    if (kllstVoices.bResetCursor())
    {
        TKrnlString* pkstrCur = nullptr;
        while (kllstVoices.bNext(pkstrCur))
            colToFill.objAdd(TString(pkstrCur->pszValue()));
    }
}


//
//  Speaks the text to a WAV file. This doesn't use the same settings as
//  the regular speach and wait operations do. It just does a one shot
//  output event to the file.
//
tCIDLib::TVoid
TSpeech::SpeakToFile(const  TString&            strToSay
                    , const TString&            strTarFile
                    , const tCIDLib::EAudioFmts eFormat
                    , const TString&            strVoice)
{
    const tCIDLib::TCh* pszVoice = nullptr;
    if (!strVoice.bIsEmpty())
        pszVoice = strVoice.pszBuffer();

    if (!TKrnlSpeech::bSpeakToFile( strToSay.pszBuffer()
                                    , strTarFile.pszBuffer()
                                    , eFormat
                                    , pszVoice))
    {
        facCIDLib().LogKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcSpeech_SpeakToFileFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


// ---------------------------------------------------------------------------
//  TSpeech: Constructors and Destructor
// ---------------------------------------------------------------------------
TSpeech::TSpeech()
{
}

TSpeech::~TSpeech()
{
}


// ---------------------------------------------------------------------------
//  TSpeech: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TSpeech::bSpeechActive() const
{
    return m_kspchImpl.bSpeechActive();
}


tCIDLib::TBoolean TSpeech::bWaitTillDone(const tCIDLib::TCard4 c4WaitFor)
{
    tCIDLib::TBoolean bTimeout;
    if (!m_kspchImpl.bWaitTillDone(c4WaitFor, bTimeout))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcSpeech_WaitTillDone
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return !bTimeout;
}


//
//  Set up the current voice to use a specific output. If it works, we remember this
//  output, and will put it back on any subsequently set voices.
//
tCIDLib::TVoid TSpeech::SetOutput(const TString& strOutputId)
{
    if (!m_kspchImpl.bSetOutput(strOutputId.pszBuffer()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcSpeech_SetOutput
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strOutputId
        );
    }

    // It worked so store the set voice
    m_strOutputId = strOutputId;
}


tCIDLib::TVoid TSpeech::SetVoice(const TString& strVoiceName)
{
    if (!m_kspchImpl.bSetVoice(strVoiceName.pszBuffer()))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcSpeech_SetVoice
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strVoiceName
        );
    }

    //
    //  If we've been given a specific output, then re-apply that. If it fails, just
    //  log it, don't rethrow.
    //
    if (!m_strOutputId.bIsEmpty())
    {
        try
        {
            SetOutput(m_strOutputId);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }

}


tCIDLib::TVoid TSpeech::Speak(  const   TString&                strToSay
                                , const tCIDLib::ESpeechFlags   eFlags)
{
    //
    //  If the indicate a synchronous operation, we really do it async,
    //  and watch for a shutdown request, since these can take a while
    //  sometimes.
    //
    const tCIDLib::ESpeechFlags eActual = eFlags | tCIDLib::ESpeechFlags::Async;
    if (!m_kspchImpl.bSpeak(strToSay.pszBuffer(), eActual))
    {
        facCIDLib().LogKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcSpeech_SpeakFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );

        // It failed, no need to do the wait below potentially
        return;
    }

    if (!tCIDLib::bAllBitsOn(eFlags, tCIDLib::ESpeechFlags::Async))
    {
        TThread* pthrMe = TThread::pthrCaller();
        tCIDLib::TCard4 c4Count = 0;
        while (c4Count < 150)
        {
            // If it's done, break out
            if (bWaitTillDone(200))
                break;

            // Not yet, bump the counter and check for a shutdown request
            c4Count++;
            if (pthrMe->bCheckShutdownRequest())
                break;
        }
    }
}

