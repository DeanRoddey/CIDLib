//
// FILE NAME: CIDSpReco_SpReco.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/30/2016
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
//  This is the header for a wrapper class that encapsulates the functionality of the
//  speech recognition engine. This class is not thread safe, so the containing app
//  should only call it on one thread at a time.
//
//  The calling application must call Initialize() to set us up. Then it must load a
//  grammar in some way, and then do any dynamic grammar updating it wants.
//
//  It must call GrammarLoadDone() when it is finished with any grammar loading and
//  dynamic grammar modifications, to let us know that it is ready to start. We will
//  enable the grammar and start recognition. The engine will have been started up
//  paused so that nothing happens until the grammar is ready.
//
//  At that point it must regularly call bGetNextEvent() to look for new speech
//  recognition events. In most cases presumably it will have a background thread that
//  is responsible for doing this and processing the resulting events. It can use this
//  to throttle itself since it takes a wait time.
//
//  We also define a janitor class for enabling/disabling grammar rules on a scoped
//  basis.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// Forward reference our internal data structure
struct TSpRecoInfo;

// ---------------------------------------------------------------------------
//   CLASS: TCIDSpReco
//  PREFIX: spreco
// ---------------------------------------------------------------------------
class CIDSPRECOEXP TCIDSpReco : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCIDSpReco() = delete;

        TCIDSpReco
        (
            const   TString&                strSpRecoName
        );

        TCIDSpReco(const TCIDSpReco&) = delete;

        ~TCIDSpReco();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCIDSpReco& operator=(const TCIDSpReco&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bGetNextRecEvent
        (
                    TCIDSpeechRecoEv&       sprecevToFill
            , const tCIDLib::TCard4         c4WaitMSs
            ,       tCIDAudStream::EStrmStates& eSrcStrmState
        );

        tCIDLib::TBoolean bRecoPause() const;

        tCIDLib::TBoolean bRecoPause
        (
            const   tCIDLib::TBoolean       bToSet
        );

        const tCIDLib::TStrList& colRuleNames() const;

        tCIDLib::TVoid GrammarLoadingDone();

        tCIDLib::TVoid Initialize
        (
                    TCIDAudioSrcStream* const pstrmSrc
        );

        tCIDLib::TVoid LoadGrammarFromBuf
        (
            const   TMemBuf&                mbufData
            , const tCIDLib::TCard4         c4Bytes
            , const tCIDLib::TBoolean       bDynamic
        );

        tCIDLib::TVoid LoadGrammarFromFile
        (
            const   TString&                strToLoad
            , const tCIDLib::TBoolean       bDynamic
        );

        tCIDLib::TVoid ReplaceOneOfRule
        (
            const   TString&                strRuleName
            , const tCIDLib::TStrList&      colNewList
        );

        tCIDLib::TVoid ReplaceOneOfRule
        (
            const   TString&                strRuleName
            , const TString&                strSemName
            , const tCIDLib::TKVPList&      colNewList
        );

        tCIDLib::TVoid ReplaceOneOfRule
        (
            const   TString&                strRuleName
            , const TString&                strSemName1
            , const TString&                strSemName2
            , const tCIDLib::TKValsList&    colNewList
        );

        tCIDLib::TVoid ReplaceOneOfRule
        (
            const   TString&                strRuleName
            , const tCIDLib::TStrList&      colRuleVals
            , const tCIDLib::TKVPList&      colSemVals
        );

        tCIDLib::TVoid ReplaceOneValRule
        (
            const   TString&                strRuleName
            , const TString&                strItemText
            , const tCIDLib::TKVPList&      colSemVals
        );

        tCIDLib::TVoid SetGrammarState
        (
            const   tCIDLib::TBoolean       bEnabled
        );

        tCIDLib::TVoid SetRuleState
        (
            const   TString&                strRuleName
            , const tCIDLib::TBoolean       bEnabled
        );

        tCIDLib::TVoid Terminate();


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckGrammarReady() const;

        tCIDLib::TVoid CheckSrcStreamReady() const;

        tCIDLib::EExitCodes eEvMonThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid* const   pData
        );

        tCIDLib::TVoid LoadRuleNames();

        tCIDLib::TVoid ProcessEvents();

        tCIDLib::TVoid ResetSpeechEngine();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bDynGrammar
        //      We remember whether the last grammar was loaded in dynamic mode or
        //      static.
        //
        //  m_colEventQ
        //      We queue up recognition events here for the application to process
        //      as soon as he is able. Though we have a separate mutex, we make this
        //      one thread safe so as to efficiently allow calls to get the next
        //      event block while waiting for data in a thread safe way.
        //
        //  m_colRuleNames
        //      For efficiency we get all of the rule names once the grammar is
        //      loaded, so that we don't have to query the whole list every time
        //      client ewants them, which may be often if he uses this list to
        //      enable/disable rules and such.
        //
        //  m_mtxSync
        //      A mutex to sync access to our members.
        //
        //  m_pInfo
        //      A pointer to our internal info structure.
        //
        //  m_strXXX
        //      Some special values we use or look for.
        //
        //  m_thrEvMon
        //      We use a background thread to watch for events.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bDynGrammar;
        tCIDSpReco::TSpEventQ   m_colEventQ;
        tCIDLib::TStrList       m_colRuleNames;
        TMutex                  m_mtxSync;
        TSpRecoInfo*            m_pInfo;
        const TString           m_strNoSemValue;
        const TString           m_strRoot;
        TThread                 m_thrEvMon;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCIDSpReco,TObject)
};


// ---------------------------------------------------------------------------
//   CLASS: TRecoGramRuleJan
//  PREFIX: jan
// ---------------------------------------------------------------------------
class CIDSPRECOEXP TRecoGramRuleJan
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRecoGramRuleJan
        (
                    TCIDSpReco* const       pSpRecoTar
            , const TString&                strRuleName
            , const tCIDLib::TBoolean       bNewState
        );

        TRecoGramRuleJan(const TRecoGramRuleJan&) = delete;

        ~TRecoGramRuleJan();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TRecoGramRuleJan& operator=(const TRecoGramRuleJan&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Orphan();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bNewState
        //      The new requested state. Upon dtor, we will set the state to the
        //      opposite of this. There's no way to get the current state of the
        //      rule, so we can't just insure that it gets back to the previous
        //      state no matter what.
        //
        //  m_pSpRecoTar
        //      The target SpReco that we are operating on.
        //
        //  m_strRuleName
        //      The name of the rule we are operating on.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bNewState;
        TCIDSpReco*         m_pSpRecoTar;
        TString             m_strRuleName;
};

#pragma CIDLIB_POPPACK
