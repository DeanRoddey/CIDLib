//
// FILE NAME: CIDMacroEng_Base64Class_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/28/2007
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_Base64.cpp file, which
//  implements the info/value derivatives needed to create a standard
//  CML class. This one provides simple Base64 support. It allows you to
//  send in a string and get it base64'd into a memory buffer. And it
//  allows you to take a buffer and get it un-Base64'd back into a string.
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
//  CLASS: TMEngBase64Val
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngBase64Val : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngBase64Val
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngBase64Val(const TMEngBase64Val&) = delete;

        ~TMEngBase64Val();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngBase64Val& operator=(const TMEngBase64Val&) = delete;


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
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TBase64& b64Value() const;

        TBase64& b64Value();


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_b64Value
        //      The C++ Base64 support object that we use to implement our
        //      functionality.
        // -------------------------------------------------------------------
        TBase64 m_b64Value;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngBase64Val,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngBase64Info
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngBase64Info : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngBase64Info
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngBase64Info(const TMEngBase64Info&) = delete;

        ~TMEngBase64Info();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngBase64Info& operator=(const TMEngBase64Info&) = delete;


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
            , const TString&                strThrowingClass
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const TString&                strText
            , const TString&                strThrowingClass
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2EnumId_XXX
        //      We create some enum nested types, so we store the ids here
        //      for quick access later.
        //
        //  m_c2MethId_XXX
        //      To speed up dispatching, which we handle at the class level
        //      since this is a fully 'external' class, we store the ids of
        //      our methods as we register them.
        //
        //  m_pmeciErrors
        //      A pointer the error enum we create for our errors.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2EnumId_Errors;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_DecodeToBuf;
        tCIDLib::TCard2 m_c2MethId_EncodeBuf;
        tCIDLib::TCard2 m_c2MethId_EncodeStr;
        tCIDLib::TCard2 m_c2MethId_SetLineWidth;
        tCIDLib::TCard4 m_c4Err_Decode;
        tCIDLib::TCard4 m_c4Err_Encode;
        TMEngEnumInfo*  m_pmeciErrors;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngBase64Info,TMEngClassInfo)
};


#pragma CIDLIB_POPPACK

