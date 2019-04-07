//
// FILE NAME: CIDMacroEng_SpeechClass_.hpp
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
//  This is the header file for the CIDMacroEng_SpeechClas.cpp file, which
//  implements a simple wrapper around the CIDLib speech support.
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
//  CLASS: TMEngSpeechVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngSpeechVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngSpeechVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngSpeechVal(const TMEngSpeechVal&) = delete;

        ~TMEngSpeechVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngSpeechVal& operator=(const TMEngSpeechVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bWaitTillDone
        (
            const   tCIDLib::TCard4         c4ToWait
        );

        tCIDLib::TVoid SetVoice
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid Speak
        (
            const   TString&                strToSay
            , const tCIDLib::ESpeechFlags   eFlags
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_spchImpl
        //      The CIDLib speech class that we use to provide our
        //      implementation.
        // -------------------------------------------------------------------
        TSpeech m_spchImpl;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngSpeechVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngSpeechInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngSpeechInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngSpeechInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngSpeechInfo(const TMEngSpeechInfo&) = delete;

        ~TMEngSpeechInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngSpeechInfo& operator=(const TMEngSpeechInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Init
        (
                    TCIDMacroEngine&        meOwner
        )   override;

        TMEngClassVal* pmecvMakeStorage
        (
            const   TString&                strName
            ,       TCIDMacroEngine&        meOwner
            , const tCIDMacroEng::EConstTypes  eConst
        )   const override;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInvokeMethod
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngMethodInfo&        methiTarget
            ,       TMEngClassVal&          mecvInstance
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const TString&                strText
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2TypeId_XXX
        //      Type ids for any nested types we create.
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        //
        //  m_pmeciErrors
        //  m_pmeciFlags
        //      We keep around a pointer to our flags nested enum classes, to
        //      speed up things.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2TypeId_Errors;
        tCIDLib::TCard2 m_c2TypeId_Flags;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_SetVoice;
        tCIDLib::TCard2 m_c2MethId_Speak;
        tCIDLib::TCard2 m_c2MethId_WaitTillDone;
        tCIDLib::TCard4 m_c4ErrId_SetVoice;
        tCIDLib::TCard4 m_c4ErrId_Speak;
        tCIDLib::TCard4 m_c4ErrId_WaitTillDone;
        TMEngEnumInfo*  m_pmeciErrors;
        TMEngEnumInfo*  m_pmeciFlags;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngSpeechInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK



