//
// FILE NAME: CIDMacroEng_AudioClass_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/16/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_AudioClass.cpp file,
//  which implements the info/value derivatives needed to create a runtime
//  class for doing basic audio (beeps and such.) This one is a completely
//  'external' class, meaning that all of it's methods are handled by C++
//  code. So we use the dummy method class just to put the method descriptions
//  into the class, but we handle the method invocation at the class level.
//
//  The following methods are provided
//
//      1. Void Beep([in] Card4 Freq, [in] Card4 Millis)
//      2. Void PlayWAV([in] String FileName)
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
//  CLASS: TMEngAudioVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngAudioVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngAudioVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst

        );

        TMEngAudioVal(const TMEngAudioVal&) = delete;

        ~TMEngAudioVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngAudioVal& operator=(const TMEngAudioVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDbgFormat
        (
                    TTextOutStream&         strmTarget
            , const TMEngClassInfo&         meciThis
            , const tCIDMacroEng::EDbgFmts     eFormat
            , const tCIDLib::ERadices       eRadix
            , const TCIDMacroEngine&        meOwner
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  We currently have no instance data, but we have to have a class
        //  to represent instance data in order to have a new class type.
        // -------------------------------------------------------------------


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngAudioVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngAudioInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngAudioInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngAudioInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngAudioInfo(const TMEngAudioInfo&) = delete;

        ~TMEngAudioInfo();


        // -------------------------------------------------------------------
        // Public operators
        // -------------------------------------------------------------------
        TMEngAudioInfo& operator=(const TMEngAudioInfo&) = delete;


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
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatching, which we handle at the class level
        //      since this is a fully 'external' class, we store the ids of
        //      our methods as we register them.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_Beep;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_PlayWAV;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngAudioInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


