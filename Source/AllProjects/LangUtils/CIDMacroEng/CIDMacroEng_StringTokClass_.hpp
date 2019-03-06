//
// FILE NAME: CIDMacroEng_StringTokClass_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/05/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_StringTokClass.cpp file,
//  which implements the info/value derivatives needed to create a runtime
//  class for doing string tokenization, i.e. to break out tokens from a
//  string based on indicated delimiters.
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
//  CLASS: TMEngStringTokVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngStringTokVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngStringTokVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngStringTokVal(const TMEngStringTokVal&) = delete;

        ~TMEngStringTokVal();


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


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngStringTokVal& operator=(const TMEngStringTokVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Init();

        const TStringTokenizer& stokValue() const;

        TStringTokenizer& stokValue();

        tCIDLib::TVoid Setup
        (
            const   TString&                strToTokenize
            , const TString&                strWhitespace
        );

        tCIDLib::TVoid SetWhitespace
        (
            const   TString&                strWhitespace
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_strValue
        //      The tokenizer is designed for maximum efficiency, so it just
        //      works on a pointer to a TString object. But, in the macro
        //      world, we don't want to create that sort of by ref connection.
        //      So we copy the string value here, and give the tokenizer our
        //      string to work on.
        //
        //      NOTE:   Order is important, since we pass a ref to the
        //              tokenizer, so we want this guy initialized first!
        //
        //  m_stokValue
        //      This is the string tokenize that provides our value.
        // -------------------------------------------------------------------
        TString             m_strValue;
        TStringTokenizer    m_stokValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngStringTokVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngStringTokInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngStringTokInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngStringTokInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngStringTokInfo(const TMEngStringTokInfo&) = delete;

        ~TMEngStringTokInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngStringTokInfo& operator=(const TMEngStringTokInfo&) = delete;


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
        //  m_c2EnumId_XXX
        //      We have to create some nested classes, so we cache the class
        //      ids for performance.
        //
        //  m_c2MethId_XXX
        //      To speed up dispatching, which we handle at the class level
        //      since this is a fully 'external' class, we store the ids of
        //      our methods as we register them.
        //
        //  m_c4ErrXXX
        //      We have a nested enum for the exceptions we throw, so we store
        //      the ordinals as we register the values, which are used later
        //      to throw them.
        //
        //  m_pmeciErrors
        //      We hang onto a copy of the error enum class that we create
        //      since we need it when we throw a macro level exception. But
        //      we DONT own it.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2EnumId_Errors;
        tCIDLib::TCard2 m_c2MethId_BuildQuotedCommaList;
        tCIDLib::TCard2 m_c2MethId_BuildQuotedCommaList2;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_EatNextToken;
        tCIDLib::TCard2 m_c2MethId_FormatQuotedList;
        tCIDLib::TCard2 m_c2MethId_GetNextToken;
        tCIDLib::TCard2 m_c2MethId_GetRestOfLine;
        tCIDLib::TCard2 m_c2MethId_MoreTokens;
        tCIDLib::TCard2 m_c2MethId_PeekRestOfLine;
        tCIDLib::TCard2 m_c2MethId_ParseCmdLine;
        tCIDLib::TCard2 m_c2MethId_ParseCSVLine;
        tCIDLib::TCard2 m_c2MethId_ParseQuotedCommaList;
        tCIDLib::TCard2 m_c2MethId_Reset;
        tCIDLib::TCard2 m_c2MethId_Set;
        tCIDLib::TCard2 m_c2MethId_SetWhitespace;
        tCIDLib::TCard2 m_c2MethId_Tokenize;
        tCIDLib::TCard2 m_c2TypeId_StrVec;
        tCIDLib::TCard4 m_c4ErrBadCmdLine;
        tCIDLib::TCard4 m_c4ErrBuild;
        tCIDLib::TCard4 m_c4ErrParse;
        tCIDLib::TCard4 m_c4ErrTokenize;
        TMEngEnumInfo*  m_pmeciErrors;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngStringTokInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK



