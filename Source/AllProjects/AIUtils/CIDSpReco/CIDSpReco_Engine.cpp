//
// FILE NAME: CIDSpReco_SpReco.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/28/2016
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
//  This file implements the SpReco wrapper
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
#include    "CIDSpReco_.hpp"

#include    <mmreg.h>
#include    <sapi.h>
#include    <sphelper.h>



// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDSpReco, TObject)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDSpReco_Engine
{
    // Max event queue entries we'll queue up before we start complaining
    const tCIDLib::TCard4   c4MaxEvQSize = 128;
}


// ---------------------------------------------------------------------------
//  We need to keep up with various things, which we don't want to make public,
//  so we define this
//
//  So we define this structure that he can create and pass to his helpers. We forward
//  reference it in the header so that we can declare private helper methods that will
//  take one of these.
// ---------------------------------------------------------------------------
struct TSpRecoInfo
{
    //
    //  The actual source of data, a derivative of our base source stream in CIDAudStream.
    //  We are given this by the application, or create a default if they don't provide
    //  one. We adopt it.
    //
    TCIDAudioSrcStream* pstrmSrc;

    //
    //  Our internal wrapper around the base stream. This derives from IStream so we
    //  can set it as the source on a speech stream. We pass the above stream to this
    //  guy to use as the actual source.
    //
    TSpWrapperStream*   pstrmWrapper;

    //
    //  And this is the actual speech stream that we pass to the recognizer. We set
    //  the above wrapper stream as the base stream on this guy.
    //
    ISpStream*          pstrmSpeech;


    // The core COM recognition objects we need
    ISpRecognizer*      pSpeechRecognizer;
    ISpRecoContext*     pSpeechContext;
    ISpRecoGrammar*     pSpeechGrammar;

    // Used to watch for speed events to occur
    HANDLE              hSpeechEvent;
};



// ---------------------------------------------------------------------------
//   CLASS: TCIDSpReco
//  PREFIX: SpReco
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDSpReco: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDSpReco::TCIDSpReco(const TString& strSpRecoName) :

    m_bDynGrammar(kCIDLib::False)
    , m_colEventQ(tCIDLib::EMTStates::Safe)
    , m_pInfo(nullptr)
    , m_strNoSemValue(L"RuleText")
    , m_strRoot(L"/")
    , m_thrEvMon
      (
          facCIDLib().strNextThreadName(TString(L"CIDSpRecoEvMon"))
          , TMemberFunc<TCIDSpReco>(this, &TCIDSpReco::eEvMonThread)
      )
{
}

TCIDSpReco::~TCIDSpReco()
{
    // If we weren't appropriately terminated, then do it now
    if (m_pInfo)
    {
        try
        {
            Terminate();
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
        }
    }
}


// ---------------------------------------------------------------------------
//  TCIDSpReco: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Wait for up to the indicated number of MSs for a speech recognition event to
//  show up. Return whether we got one or not. The queue is thread safe, so this
//  works the nicely without any sync.
//
//  We assume that the application won't call another method on us while we are in
//  here because he's blocked on this and can only call us on one thread a time.
//
tCIDLib::TBoolean
TCIDSpReco::bGetNextRecEvent(       TCIDSpeechRecoEv&           sprecevToFill
                            , const tCIDLib::TCard4             c4WaitMSs
                            ,       tCIDAudStream::EStrmStates& eSrcStrmState)
{
    CheckSrcStreamReady();

    const tCIDLib::TBoolean bRes = m_colEventQ.bGetNext(sprecevToFill, c4WaitMSs);

    // Give this guy back the current stream state either way
    eSrcStrmState = tCIDAudStream::EStrmStates::Ready; // m_pInfo->pstrmSrc->eState();

    return bRes;
}


//
//  Get or set the grammar recognition pause state. So TRUE means PAUSED, and false
//  means enabled.
//
//  If recogition is paused, we just tell our SpReco stream to pass silence to the
//  speech stream that wraps it. This is an effective way to do it, whereas various
//  other things like pausing the grammar don't because the audio is still there
//  waiting to be read when it is re-enabled.
//
tCIDLib::TBoolean TCIDSpReco::bRecoPause() const
{
    if (m_pInfo->pstrmWrapper)
        return m_pInfo->pstrmWrapper->bSuppressInput();

    // If the stream doesn't exist, obviously we are not doing recognition
    return kCIDLib::True;
}

tCIDLib::TBoolean TCIDSpReco::bRecoPause(const tCIDLib::TBoolean bToSet)
{
    // Have our wrapper stream pass silence
    if (m_pInfo->pstrmWrapper)
        m_pInfo->pstrmWrapper->bSuppressInput(bToSet);

    return bToSet;
}


// Make our list of rule names available
const tCIDLib::TStrList& TCIDSpReco::colRuleNames() const
{
    return m_colRuleNames;
}


//
//  The application should call this once it has loaded its grammar and has finished
//  making any changes if it is a dynamic grammar. This gives us a chance to do any
//  final bits. If the grammar was loaded dynamically, we do a commit. Then we iterate
//  the rule names and load them up into a list for later use.
//
tCIDLib::TVoid TCIDSpReco::GrammarLoadingDone()
{
    CheckGrammarReady();

    // If the last grammar loaded was dynamic, then do a commit
    HRESULT hRes;
    if (m_bDynGrammar)
    {
        hRes = m_pInfo->pSpeechGrammar->Commit(0);
        if (FAILED(hRes))
        {
            facCIDSpReco().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSpRecoErrs::errcGram_CommitFailed
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TCardinal(hRes, tCIDLib::ERadices::Hex)
            );
        }
    }

    // Load the rule names
    LoadRuleNames();

    // Enable the grammar
    SetGrammarState(kCIDLib::True);

    // Start recognition
    hRes = m_pInfo->pSpeechContext->Resume(NULL);

    // Enable the top level rules
    hRes = m_pInfo->pSpeechGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE);

}


//
//  This is called to start up speech recognition. The caller provides us with a
//  source stream to use for audio input. We adopt the stream.
//
tCIDLib::TVoid TCIDSpReco::Initialize(TCIDAudioSrcStream* const pstrmToAdopt)
{
    // Make sure it gets cleaned up if we don't get it stored away
    TJanitor<TCIDAudioSrcStream> janSteam(pstrmToAdopt);

    // Make sure we aren't already initialized
    if (m_pInfo)
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcEng_NotReady
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }

    try
    {
        HRESULT hRes;

        // Set up our internal data structure
        m_pInfo = new TSpRecoInfo;
        TRawMem::SetMemBuf(m_pInfo, kCIDLib::c1MinCard, sizeof(TSpRecoInfo));
        m_pInfo->hSpeechEvent = INVALID_HANDLE_VALUE;

        // Let's try to create the speech recognizer now
        ISpObjectToken* pEngineToken = nullptr;
        hRes = ::CoCreateInstance
        (
            CLSID_SpInprocRecognizer
            , NULL
            , CLSCTX_INPROC_SERVER
            , __uuidof(ISpRecognizer)
            , tCIDLib::pToVoidPP(&m_pInfo->pSpeechRecognizer)
        );

        if (FAILED(hRes))
        {
            facCIDSpReco().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSpRecoErrs::errcEng_CreateRecog
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Init
                , TCardinal(hRes, tCIDLib::ERadices::Hex)
            );
        }
        TCOMJanitor<ISpRecognizer> janRecog(&m_pInfo->pSpeechRecognizer);

        //
        //  Store the incoming application source stream. If null, then create
        //  a standard audio input based one, which will use the default audio
        //  input.
        //
        if (pstrmToAdopt)
            m_pInfo->pstrmSrc = janSteam.pobjOrphan();
        else
            m_pInfo->pstrmSrc = new TWaveInSrcStream();

        // Initialize the source stream
        m_pInfo->pstrmSrc->Initialize();

        // Create our wrapper stream and pass it the application stream
        m_pInfo->pstrmWrapper = new TSpWrapperStream(m_pInfo->pstrmSrc);

        // Create the speech specific stream
        hRes = ::CoCreateInstance
        (
            CLSID_SpStream
            , NULL
            , CLSCTX_INPROC_SERVER
            , __uuidof(ISpStream)
            , tCIDLib::pToVoidPP(&m_pInfo->pstrmSpeech)
        );

        if (FAILED(hRes))
        {
            facCIDSpReco().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSpRecoErrs::errcEng_CreateSpeechStream
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Init
                , TCardinal(hRes, tCIDLib::ERadices::Hex)
            );
        }
        TCOMJanitor<ISpStream> janStream(&m_pInfo->pstrmSpeech);

        // The format we expect our sources to provide us data in
        WAVEFORMATEX wfxOut = { WAVE_FORMAT_PCM, 1, 16000, 32000, 2, 16, 0 };

        // Set our translation stream as the input source for this
        hRes = m_pInfo->pstrmSpeech->SetBaseStream
        (
            m_pInfo->pstrmWrapper, SPDFID_WaveFormatEx, &wfxOut
        );
        if (FAILED(hRes))
        {
            facCIDSpReco().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSpRecoErrs::errcEng_SetBaseStream
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Init
                , TCardinal(hRes, tCIDLib::ERadices::Hex)
            );
        }

        // Set the final stream as the input on the recognizer
        m_pInfo->pSpeechRecognizer->SetInput(m_pInfo->pstrmSpeech, TRUE);

        // And now enable processing on the source stream
        m_pInfo->pstrmWrapper->bSpeechState(kCIDLib::True);

        //
        //  If this fails here, you have not installed any acoustic models that the
        //  reco engine understands, or no English ones.
        //
        hRes = ::SpFindBestToken(SPCAT_RECOGNIZERS, L"Language=409", NULL, &pEngineToken);
        if (FAILED(hRes))
        {
            facCIDSpReco().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSpRecoErrs::errcEng_LoadModel
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Init
                , TCardinal(hRes, tCIDLib::ERadices::Hex)
            );
        }
        TCOMJanitor<ISpObjectToken> janEngine(&pEngineToken);

        // We got it, so set it on the recognizer and create the context
        m_pInfo->pSpeechRecognizer->SetRecognizer(pEngineToken);
        hRes = m_pInfo->pSpeechRecognizer->CreateRecoContext(&m_pInfo->pSpeechContext);
        if (FAILED(hRes))
        {
            facCIDSpReco().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSpRecoErrs::errcEng_LoadModel
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Init
                , TCardinal(hRes, tCIDLib::ERadices::Hex)
            );
        }
        TCOMJanitor<ISpRecoContext> janContext(&m_pInfo->pSpeechContext);

        // Disable model adapation since we run continuously.
        hRes = m_pInfo->pSpeechRecognizer->SetPropertyNum(L"AdaptationOn", 0);
        if(FAILED(hRes))
        {
            facCIDSpReco().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSpRecoErrs::errcEng_NoAdaptation
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Init
                , TCardinal(hRes, tCIDLib::ERadices::Hex)
            );
        }

        // Pause it until the application gets the grammar set up and enables it
        hRes = m_pInfo->pSpeechContext->Pause(NULL);
        if(FAILED(hRes))
        {
            facCIDSpReco().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSpRecoErrs::errcEng_Pause
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Init
                , TCardinal(hRes, tCIDLib::ERadices::Hex)
            );
        }

        // Create the grammar for our context
        hRes = m_pInfo->pSpeechContext->CreateGrammar(1, &m_pInfo->pSpeechGrammar);
        if (FAILED(hRes))
        {
            facCIDSpReco().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSpRecoErrs::errcEng_CreateGrammar
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Init
                , TCardinal(hRes, tCIDLib::ERadices::Hex)
            );
        }
        TCOMJanitor<ISpRecoGrammar> janGrammar(&m_pInfo->pSpeechGrammar);

        // We only care about recognition events
        hRes = m_pInfo->pSpeechContext->SetInterest
        (
            SPFEI(SPEI_RECOGNITION),SPFEI(SPEI_RECOGNITION)
        );
        if(FAILED(hRes))
        {
            facCIDSpReco().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSpRecoErrs::errcEng_RegForEvents
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Init
                , TCardinal(hRes, tCIDLib::ERadices::Hex)
            );
        }

        // And get the notification handle
        m_pInfo->hSpeechEvent = m_pInfo->pSpeechContext->GetNotifyEventHandle();
        if (m_pInfo->hSpeechEvent == INVALID_HANDLE_VALUE)
        {
            facCIDSpReco().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSpRecoErrs::errcEng_GetNotifyHandle
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Init
                , TCardinal(hRes, tCIDLib::ERadices::Hex)
            );
        }

        // It all worked, so release any of the janitors for things we want to keep
        janGrammar.Orphan();
        janContext.Orphan();
        janRecog.Orphan();
        janStream.Orphan();

        //
        //  Release our ref count of the wrapper stream now. DON'T use SafeCOMFree
        //  because it will null the pointer!
        //
        m_pInfo->pstrmWrapper->Release();

        // Start up the event handler thread
        m_thrEvMon.Start();
    }

    catch(TError& errToCatch)
    {
        // Clean up anything we created
        Terminate();

        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

//
//  For the derived class to load a grammar from a pre-compiled grammar that it has
//  in memory.
//
tCIDLib::TVoid
TCIDSpReco::LoadGrammarFromBuf( const   TMemBuf&            mbufToLoad
                                , const tCIDLib::TCard4     c4Bytes
                                , const tCIDLib::TBoolean   bDynamic)
{
    // Make sure we are ready for this
    CheckGrammarReady();

    // Create a global memory buffer of the appropriate size
    HGLOBAL hGlob = ::GlobalAlloc(GMEM_MOVEABLE, c4Bytes);
    if (!hGlob)
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_BufAllocFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(::GetLastError())
        );
    }

    // Get a pointer to the memory
    void* pData = ::GlobalLock(hGlob);
    if (!pData)
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_BufLockFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Copy the data into it
    TRawMem::CopyMemBuf(pData, mbufToLoad.pc1Data(), c4Bytes);

    // Now let's try to load the grammar
    HRESULT hRes = m_pInfo->pSpeechGrammar->LoadCmdFromMemory
    (
        (SPBINARYGRAMMAR *)::GlobalLock(hGlob), bDynamic ? SPLO_DYNAMIC : SPLO_STATIC
    );

    // Regardless of outcome, free the memory
    ::GlobalFree(hGlob);

    if (FAILED(hRes))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_BufLoadFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }

    // Remember if was loaded dynamically or not
    m_bDynGrammar = bDynamic;
}


//
//  For the derived class to load a file based grammar into us. We'll throw an
//  exception it fails.
//
tCIDLib::TVoid
TCIDSpReco::LoadGrammarFromFile(const TString& strToLoad, const tCIDLib::TBoolean bDynamic)
{
    // Make sure we are ready for this
    CheckGrammarReady();

    HRESULT hRes = m_pInfo->pSpeechGrammar->LoadCmdFromFile
    (
        strToLoad.pszBuffer(), bDynamic ? SPLO_DYNAMIC : SPLO_STATIC
    );

    if (FAILED(hRes))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_FileLoadFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }

    // Remember if was loaded dynamically or not
    m_bDynGrammar = bDynamic;
}


//
//  This is allows for the common scenario where place holder rules are included, with a
//  single 'one of' item in it, which will be dynamically updated with the actual
//  available values passed.
//
//  We get the name of the rule, and we get a list of string values that basically
//  become the items in a <one-of> type rule.
//
//  There is another one that gets a list of key/value pairs. The the key is the actual
//  item text. The value is the semantic value. We also get the name of the semantic
//  value to set the value to. If empty, it's just assigned to 'out', i.e. the rule
//  result.
//
//  And another that allows for two semantic values. As above, the key is the actual item
//  text. Value 1 and value 2 are the two semantic values for each item. We get two
//  semantic names to use of course, the first applying to value 1 and the second applying
//  to value 2. In this case both semantic names must be provided.
//
//  We have another that takes a list of values for a <one-of> and a list of semantic values
//  to set, if any of the values match.
//
//  And we have another where they just set one text value, but multiple key/value pairs
//  to indicate semantic values for that rule.
//
tCIDLib::TVoid
TCIDSpReco::ReplaceOneOfRule(const  TString&            strRuleName
                            , const tCIDLib::TStrList&  colNewList)
{
    CheckGrammarReady();

    // Try to find the rule
    SPSTATEHANDLE hRule;
    HRESULT hRes = m_pInfo->pSpeechGrammar->GetRule
    (
        strRuleName.pszBuffer(), NULL, SPRAF_Dynamic, FALSE, &hRule
    );

    if (FAILED(hRes))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_RuleNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strRuleName
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }

    // Let's clear out any dummy content
    hRes = m_pInfo->pSpeechGrammar->ClearRule(hRule);
    if (FAILED(hRes))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_ClearRule
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strRuleName
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }

    // And let's loop through the provided values and add them
    const tCIDLib::TCard4 c4Count = colNewList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TString& strCur = colNewList[c4Index];
        hRes = m_pInfo->pSpeechGrammar->AddWordTransition
        (
            hRule, NULL, strCur.pszBuffer(), L" ", SPWT_LEXICAL, 1, NULL
        );

        if (FAILED(hRes))
        {
            facCIDSpReco().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSpRecoErrs::errcGram_WordTrans
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , strRuleName
                , TCardinal(hRes, tCIDLib::ERadices::Hex)
            );
        }
    }
}


tCIDLib::TVoid
TCIDSpReco::ReplaceOneOfRule(const  TString&            strRuleName
                            , const TString&            strSemName
                            , const tCIDLib::TKVPList&  colNewList)
{
    CheckGrammarReady();

    // Try to find the rule
    SPSTATEHANDLE hRule;
    HRESULT hRes = m_pInfo->pSpeechGrammar->GetRule
    (
        strRuleName.pszBuffer(), NULL, SPRAF_Dynamic, FALSE, &hRule
    );

    if (FAILED(hRes))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_RuleNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strRuleName
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }

    // Let's clear out any dummy content
    hRes = m_pInfo->pSpeechGrammar->ClearRule(hRule);
    if (FAILED(hRes))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_ClearRule
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strRuleName
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }

    // Set up the property info structure with the stuff that won't change
    SPPROPERTYINFO PropInfo = {0};
    PropInfo.pszName = L"Property";
    PropInfo.vValue.vt = VT_UI4;

    // And let's loop through the provided values and add them
    TString strSemVal;
    const tCIDLib::TCard4 c4Count = colNewList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyValuePair& kvalCur = colNewList[c4Index];

        PropInfo.vValue.ulVal = c4Index + 1;

        strSemVal = L"out";
        if (!strSemName.bIsEmpty())
        {
            strSemVal.Append(kCIDLib::chPeriod);
            strSemVal.Append(strSemName);
        }
        strSemVal.Append(L" = \"");
        strSemVal.Append(kvalCur.strValue());
        strSemVal.Append(L"\";");
        PropInfo.pszValue = strSemVal.pszBuffer();

        hRes = m_pInfo->pSpeechGrammar->AddWordTransition
        (
            hRule, NULL, kvalCur.strKey().pszBuffer(), NULL, SPWT_LEXICAL, 1, &PropInfo
        );

        if (FAILED(hRes))
        {
            facCIDSpReco().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSpRecoErrs::errcGram_WordTrans
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , strRuleName
                , TCardinal(hRes, tCIDLib::ERadices::Hex)
            );
        }
    }
}


tCIDLib::TVoid
TCIDSpReco::ReplaceOneOfRule(const  TString&                strRuleName
                            , const TString&                strSemName1
                            , const TString&                strSemName2
                            , const tCIDLib::TKValsList&    colNewList)
{
    CheckGrammarReady();

    // Try to find the rule
    SPSTATEHANDLE hRule;
    HRESULT hRes = m_pInfo->pSpeechGrammar->GetRule
    (
        strRuleName.pszBuffer(), NULL, SPRAF_Dynamic, FALSE, &hRule
    );

    if (FAILED(hRes))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_RuleNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strRuleName
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }

    // Let's clear out any dummy content
    hRes = m_pInfo->pSpeechGrammar->ClearRule(hRule);
    if (FAILED(hRes))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_ClearRule
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strRuleName
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }

    // Set up the property info structure with the stuff that won't change
    SPPROPERTYINFO PropInfo = {0};
    PropInfo.pszName = L"Property";
    PropInfo.vValue.vt = VT_UI4;

    // And let's loop through the provided values and add them
    TString strSemVal;
    const tCIDLib::TCard4 c4Count = colNewList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyValues& kvalsCur = colNewList[c4Index];

        PropInfo.vValue.ulVal = c4Index + 1;

        strSemVal = L"out.";
        strSemVal.Append(strSemName1);
        strSemVal.Append(L" = \"");
        strSemVal.Append(kvalsCur.strVal1());
        strSemVal.Append(L"\"; out.");
        strSemVal.Append(strSemName2);
        strSemVal.Append(L" = \"");
        strSemVal.Append(kvalsCur.strVal2());
        strSemVal.Append(L"\";");
        PropInfo.pszValue = strSemVal.pszBuffer();

        hRes = m_pInfo->pSpeechGrammar->AddWordTransition
        (
            hRule, NULL, kvalsCur.strKey().pszBuffer(), NULL, SPWT_LEXICAL, 1, &PropInfo
        );

        if (FAILED(hRes))
        {
            facCIDSpReco().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSpRecoErrs::errcGram_WordTrans
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , strRuleName
                , TCardinal(hRes, tCIDLib::ERadices::Hex)
            );
        }
    }
}


//
//  This one takes a list of text values (for a <one-of>) and a list of multiple
//  semantic values to return for the rule if any of the values match, i.e. we aren't
//  assigning individual semantic values to the values. We set the same semantic values
//  on all of them.
//
tCIDLib::TVoid
TCIDSpReco::ReplaceOneOfRule(const  TString&            strRuleName
                            , const tCIDLib::TStrList&  colRuleVals
                            , const tCIDLib::TKVPList&  colSemVals)
{
    CheckGrammarReady();

    // Try to find the rule
    SPSTATEHANDLE hRule;
    HRESULT hRes = m_pInfo->pSpeechGrammar->GetRule
    (
        strRuleName.pszBuffer(), NULL, SPRAF_Dynamic, FALSE, &hRule
    );

    if (FAILED(hRes))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_RuleNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strRuleName
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }

    // Let's clear out any dummy content
    hRes = m_pInfo->pSpeechGrammar->ClearRule(hRule);
    if (FAILED(hRes))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_ClearRule
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strRuleName
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }

    // Set up the property info structure with the stuff that won't change
    SPPROPERTYINFO PropInfo = {0};
    PropInfo.pszName = L"Property";
    PropInfo.vValue.vt = VT_UI4;
    PropInfo.vValue.ulVal = 1;

    // Build up the semantic value
    const TString strEq(L" = \"");
    const TString strOut(L"out.");
    const TString strSemi(L"\"; ");

    tCIDLib::TCard4 c4Count = colSemVals.c4ElemCount();
    TString strSemVal(c4Count * 32);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyValuePair& kvalCur = colSemVals[c4Index];
        strSemVal.Append(strOut);
        strSemVal.Append(kvalCur.strKey());
        strSemVal.Append(strEq);
        strSemVal.Append(kvalCur.strValue());
        strSemVal.Append(strSemi);
    }

    // Set it on the property
    PropInfo.pszValue = strSemVal.pszBuffer();

    // And set the transition text values
    c4Count = colRuleVals.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        hRes = m_pInfo->pSpeechGrammar->AddWordTransition
        (
            hRule, NULL, colRuleVals[c4Index].pszBuffer(), NULL, SPWT_LEXICAL, 1, &PropInfo
        );

        if (FAILED(hRes))
        {
            facCIDSpReco().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSpRecoErrs::errcGram_WordTrans
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , strRuleName
                , TCardinal(hRes, tCIDLib::ERadices::Hex)
            );
        }
    }
}


tCIDLib::TVoid
TCIDSpReco::ReplaceOneValRule(  const   TString&            strRuleName
                                , const TString&            strItemText
                                , const tCIDLib::TKVPList&  colSemVals)
{
    CheckGrammarReady();

    // Try to find the rule
    SPSTATEHANDLE hRule;
    HRESULT hRes = m_pInfo->pSpeechGrammar->GetRule
    (
        strRuleName.pszBuffer(), NULL, SPRAF_Dynamic, FALSE, &hRule
    );

    if (FAILED(hRes))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_RuleNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strRuleName
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }

    // Let's clear out any dummy content
    hRes = m_pInfo->pSpeechGrammar->ClearRule(hRule);
    if (FAILED(hRes))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_ClearRule
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strRuleName
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }

    // Set up the property info structure with the stuff that won't change
    SPPROPERTYINFO PropInfo = {0};
    PropInfo.pszName = L"Property";
    PropInfo.vValue.vt = VT_UI4;
    PropInfo.vValue.ulVal = 1;

    // Build up the semantic value
    const TString strEq(L" = \"");
    const TString strOut(L"out.");
    const TString strSemi(L"\"; ");

    const tCIDLib::TCard4 c4Count = colSemVals.c4ElemCount();
    TString strSemVal(c4Count * 32);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const TKeyValuePair& kvalCur = colSemVals[c4Index];
        strSemVal.Append(strOut);
        strSemVal.Append(kvalCur.strKey());
        strSemVal.Append(strEq);
        strSemVal.Append(kvalCur.strValue());
        strSemVal.Append(strSemi);
    }

    // Set it on the property
    PropInfo.pszValue = strSemVal.pszBuffer();

    // And set the rule
    hRes = m_pInfo->pSpeechGrammar->AddWordTransition
    (
        hRule, NULL, strItemText.pszBuffer(), NULL, SPWT_LEXICAL, 1, &PropInfo
    );

    if (FAILED(hRes))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_WordTrans
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strRuleName
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }
}


// Enable or disable the grammar, usually to disable while manipulating it
tCIDLib::TVoid TCIDSpReco::SetGrammarState(const tCIDLib::TBoolean bEnable)
{
    HRESULT hRes = m_pInfo->pSpeechGrammar->SetGrammarState
    (
        bEnable ? SPGS_ENABLED : SPGS_DISABLED
    );

    if (FAILED(hRes))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_SetGrammarState
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }
}


//
//  Enable or disable a rule based on its name in the grammar.
//
tCIDLib::TVoid
TCIDSpReco::SetRuleState(const TString& strRuleName, const tCIDLib::TBoolean bEnable)
{
    // Don't do anything if not ready
    if (m_pInfo->pSpeechGrammar)
    {
        HRESULT hRes = m_pInfo->pSpeechGrammar->SetRuleState
        (
            strRuleName.pszBuffer(), NULL, bEnable ? SPRS_ACTIVE : SPRS_INACTIVE
        );

        if (FAILED(hRes))
        {
            facCIDSpReco().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kSpRecoErrs::errcGram_SetRuleState
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , strRuleName
                , TCardinal(hRes, tCIDLib::ERadices::Hex)
            );
        }
    }
}

//
//  This is called to clean up everything and put us back to inactive mode. We will
//  require another call to init before we can be used again.
//
tCIDLib::TVoid TCIDSpReco::Terminate()
{
    // If never initialized, then do nothing
    if (!m_pInfo)
        return;

    // First of all stop the event monitor thread
    if (m_thrEvMon.bIsRunning())
    {
        try
        {
            m_thrEvMon.ReqShutdownSync(5000);
            m_thrEvMon.eWaitForDeath(10000);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
        }
    }

    // Zero out other stuff that doesn't need releasing
    m_pInfo->hSpeechEvent = INVALID_HANDLE_VALUE;

    // Free the main reco stuff
    if (m_pInfo->pSpeechGrammar)
        SafeCOMFree(m_pInfo->pSpeechGrammar);

    if (m_pInfo->pSpeechContext)
        SafeCOMFree(m_pInfo->pSpeechContext);

    if (m_pInfo->pSpeechRecognizer)
        SafeCOMFree(m_pInfo->pSpeechRecognizer);

    // Stop our wrapper stream using the underlying stream
    if (m_pInfo->pstrmWrapper)
        m_pInfo->pstrmWrapper->bSpeechState(kCIDLib::False);

    //
    //  Now clean up the speech stream first. This will also destroy the wrapper
    //  stream, which is a COM object owned by the speech stream.
    //
    if (m_pInfo->pstrmSpeech)
        SafeCOMFree(m_pInfo->pstrmSpeech);

    // And finally do the source stream
    if (m_pInfo->pstrmSrc)
    {
        try
        {
            m_pInfo->pstrmSrc->Terminate();
            delete m_pInfo->pstrmSrc;
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
        }
    }

    // Now clean up the info structure pointer to indicate we are terminated
    delete m_pInfo;
    m_pInfo = nullptr;
}


// ---------------------------------------------------------------------------
//  TCIDSpReco: Private, non-virtual methods
// ---------------------------------------------------------------------------

// We have to check this in a lot of methods so split it out
tCIDLib::TVoid TCIDSpReco::CheckGrammarReady() const
{
    if (!m_pInfo || !m_pInfo->pSpeechGrammar)
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcEng_GrammarNotReady
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
}


// Throw if our source stream is not in place
tCIDLib::TVoid TCIDSpReco::CheckSrcStreamReady() const
{
//    if (!m_pInfo || !m_pInfo->pstrmSrc)
//    {
//        facCIDSpReco().ThrowErr
//        (
//            CID_FILE
//            , CID_LINE
//            , kSpRecoErrs::errcEng_SrcStrmNotReady
//            , tCIDLib::ESeverities::Failed
//            , tCIDLib::EErrClasses::NotReady
//        );
//    }
}


//
//  When we are told that there's a recognition event this is called. We check the
//  event, pull the info out, package it up, and call a virtual method for the derived
//  class to handle the event.
//
static tCIDLib::TVoid
IterateRules(       tCIDSpReco::TRuleList&  colRuleList
            , const SPPHRASERULE&           ParRule
            , const TString&                strParPath
            ,       TKeyNumPair&            knumTmp)
{
    const SPPHRASERULE* pruleCur = &ParRule;
    while (pruleCur)
    {
        // Create the path for this new entry
        TString strNewPath = strParPath;
        if (strNewPath != kCIDSpReco::strPath_Root)
            strNewPath.Append(kCIDLib::chForwardSlash);
        strNewPath.Append(pruleCur->pszName);

        if (pruleCur->SREngineConfidence != -1.0)
            knumTmp.Set(strNewPath, pruleCur->SREngineConfidence);
        else
            knumTmp.Set(strNewPath, pruleCur->Confidence);
        colRuleList.objAdd(knumTmp);

        // If any children, iterate those
        if (pruleCur->pFirstChild)
            IterateRules(colRuleList, *pruleCur->pFirstChild, strNewPath, knumTmp);

        pruleCur = pruleCur->pNextSibling;
    }
}

static tCIDLib::TVoid
IterateSemantics(       tCIDSpReco::TSemList&   colSemList
                , const SPPHRASEPROPERTY&       propStart
                ,       TKeyNumPair&            knumTmp)
{
    const SPPHRASEPROPERTY* ppropCur = &propStart;
    while (ppropCur)
    {
        // If it has children recurse
        if (ppropCur->pFirstChild)
        {
            IterateSemantics(colSemList, *ppropCur->pFirstChild, knumTmp);
        }
         else
        {
            // No children, so it's a terminal node.
            if (ppropCur->SREngineConfidence != -1.0)
                knumTmp.Set(ppropCur->pszName, ppropCur->SREngineConfidence, ppropCur->pszValue);
            else
                knumTmp.Set(ppropCur->pszName, ppropCur->Confidence, ppropCur->pszValue);
            colSemList.objAdd(knumTmp);
        }
        ppropCur = ppropCur->pNextSibling;
    }
}


//
//  We start up our event monitor thread on this method. It just waits for events
//  to be available, grabs them, packages each one up in our reco event class, and
//  queues it up for the application to come get when it has time.
//
tCIDLib::EExitCodes
TCIDSpReco::eEvMonThread(TThread& thrThis, tCIDLib::TVoid* const pData)
{
    // Let the calling thread go
    thrThis.Sync();

    SPEVENT CurEvent = { SPEI_UNDEFINED, SPET_LPARAM_IS_UNDEFINED, 0, 0, 0, 0};
    ULONG ulFetched = 0;
    HRESULT hRes;

    // Set up our event handles to wait for. We just have the one speech event handle
    HANDLE aEvents[1];
    aEvents[0] = m_pInfo->hSpeechEvent;

    // Process until asked to shut down
    tCIDSpReco::TRuleList   colRuleList;
    tCIDSpReco::TSemList    colSemList;
    TCIDSpeechRecoEv        sprecevCur;
    TString                 strRuleName;
    TString                 strRulePath;
    while (!thrThis.bCheckShutdownRequest())
    {
        //
        //  Now wait a while for something to show up. We use a helper method to do
        //  this so that we get appropriate message pumping enough to make COM stuff
        //  work.
        //
        tCIDLib::TCard4 c4Signaled;
        const tCIDLib::EWaitRes eRes = TKrnlWin32::eWaitHandles(aEvents, 1, c4Signaled, 250);

        // If nothing, just go back to the top
        if (eRes != tCIDLib::EWaitRes::Success)
            continue;

        // While events are available, process the next available one
        while (kCIDLib::True)
        {
            hRes = m_pInfo->pSpeechContext->GetEvents(1, &CurEvent, &ulFetched);
            if (FAILED(hRes) || !ulFetched)
                break;

            // If it's a recognition event, let's process it
            if ((CurEvent.eEventId == SPEI_RECOGNITION)
            &&  (CurEvent.elParamType == SPET_LPARAM_IS_OBJECT))
            {
                // This is an ISpRecoResult
                ISpRecoResult* pResult = reinterpret_cast<ISpRecoResult*>(CurEvent.lParam);
                SPPHRASE* pPhrase = nullptr;
                hRes = pResult->GetPhrase(&pPhrase);

                // If it has no children, then skip it
                if (!pPhrase->Rule.pFirstChild)
                    continue;

                //
                //  If it worked, then make sure we have properties, else there's nothing
                //  to do.
                //
                if (SUCCEEDED(hRes) && pPhrase->pProperties)
                {
                    TKeyNumPair knumTmp;

                    //
                    //  Sometimes the top level rule name is just TopRule, which is
                    //  totally useless. In that case, the first child should have the
                    //  actual rule name. Else, just take the top level name.
                    //
                    //  We also set up the starting path for rule iteration. If it has
                    //  top rule as a name, we start with a / because the first child
                    //  will have the actual top level rule name. Else we start with /xxx
                    //  since the rule itself has the top level rule name.
                    //
                    strRulePath = kCIDSpReco::strPath_Root;
                    tCIDLib::TFloat4 f4TopConf;
                    if (kCIDSpReco::strRule_TopRule.bCompareI(pPhrase->Rule.pszName))
                    {
                        strRuleName = pPhrase->Rule.pFirstChild->pszName;
                        f4TopConf = pPhrase->Rule.pFirstChild->SREngineConfidence;
                    }
                     else
                    {
                        strRuleName = pPhrase->Rule.pszName;
                        strRulePath.Append(strRuleName);
                        f4TopConf = pPhrase->Rule.SREngineConfidence;
                    }


                    // Fill in the semantic tree for this event
                    colSemList.RemoveAll();
                    if (pPhrase->pProperties->pFirstChild)
                    {
                        IterateSemantics
                        (
                            colSemList, *pPhrase->pProperties->pFirstChild, knumTmp
                        );
                    }
                     else if (pPhrase->pProperties->pszValue)
                    {
                        knumTmp.Set
                        (
                            m_strNoSemValue, -1, pPhrase->pProperties->pszValue
                        );
                        colSemList.objAdd(knumTmp);
                    }

                    colRuleList.RemoveAll();
                    if (pPhrase->Rule.pFirstChild)
                    {
                        IterateRules
                        (
                            colRuleList
                            , *pPhrase->Rule.pFirstChild
                            , strRulePath
                            , knumTmp
                        );
                    }


                    //
                    //  And now we can set up the event. We give it the rules so that it
                    //  can update the semantic values as it stores them, if rule to semantic
                    //  mapping values are set on this rule. In debug mode is also saves
                    //  the rules for debugging purposes.
                    //
                    //  This could fail if mapping semantics are bad.
                    //
                    try
                    {
                        sprecevCur.Set(strRuleName, f4TopConf, colSemList, colRuleList);

                        //
                        //  Queue it up for the application to process, as long as the
                        //  queue isn't full. If it is, log an error.
                        //
                        if (m_colEventQ.bIsFull(CIDSpReco_Engine::c4MaxEvQSize))
                        {
                            facCIDSpReco().LogMsg
                            (
                                CID_FILE
                                , CID_LINE
                                , kSpRecoMsgs::midStatus_EvQFull
                                , tCIDLib::ESeverities::Failed
                                , tCIDLib::EErrClasses::OutResource
                            );
                        }
                         else
                        {
                            m_colEventQ.objAdd(sprecevCur);
                        }
                    }

                    catch(TError& errToCatch)
                    {
                        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                        TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
                    }
                }
            }
        }
    }

    return tCIDLib::EExitCodes::Normal;
}


//
//  After the grammar is loaded and committed, this is called to store a list of
//  the names of all of the available rules.
//
tCIDLib::TVoid TCIDSpReco::LoadRuleNames()
{
    #if CID_DEBUG_ON
    CheckGrammarReady();
    #endif

    // We need to get an extra interface for this.
    ISpRecoGrammar2* pGrammar2;
    HRESULT hRes = m_pInfo->pSpeechGrammar->QueryInterface(&pGrammar2);
    if (FAILED(hRes))
    {
        facCIDSpReco().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kSpRecoErrs::errcGram_GetGrammar2
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(hRes, tCIDLib::ERadices::Hex)
        );
    }

    // Make sure the extra interface gets cleaned up
    TCOMJanitor<ISpRecoGrammar2> janGrammar2(&pGrammar2);

    // Loop through the rules and fill in the list
    m_colRuleNames.RemoveAll();

    SPRULE* pRules;
    UINT Count;
    pGrammar2->GetRules(&pRules, &Count);
    for (UINT i = 0; i < Count; i++)
        m_colRuleNames.objAdd(pRules[i].pszRuleName);
}




// ---------------------------------------------------------------------------
//   CLASS: TRecoGramRuleJan
//  PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TRecoGramRuleJan: Constructors and Destructor
// ---------------------------------------------------------------------------
TRecoGramRuleJan::TRecoGramRuleJan(         TCIDSpReco* const       pSpRecoTar
                                    , const TString&                strRuleName
                                    , const tCIDLib::TBoolean       bNewState) :

    m_bNewState(bNewState)
    , m_pSpRecoTar(pSpRecoTar)
    , m_strRuleName(strRuleName)
{
    CIDAssert(pSpRecoTar, L"The target SpReco object pointer is null");

    // Set the state
    pSpRecoTar->SetRuleState(strRuleName, bNewState);
}

TRecoGramRuleJan::~TRecoGramRuleJan()
{
    // If we weren't orphaned, then put the state back the other way
    if (m_pSpRecoTar)
    {
        try
        {
            m_pSpRecoTar->SetRuleState(m_strRuleName, !m_bNewState);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(tCIDLib::ForceMove(errToCatch));
        }
    }
}



// ---------------------------------------------------------------------------
//  TRecoGramRuleJan: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Makes us not do anything on dtor
tCIDLib::TVoid TRecoGramRuleJan::Orphan()
{
    m_pSpRecoTar = nullptr;
}

