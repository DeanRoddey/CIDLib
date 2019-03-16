//
// FILE NAME: CIDMacroEng_SHA1Classes_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/23/2009
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
//  This is the header file for the CIDMacroEng_SHA1Classes.cpp file,
//  which implements the info/value derivatives needed to create an SHA1
//  hash digester and an SHA1 hash value.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TSHA1Hasher;
class TSHA1Hash;


// ---------------------------------------------------------------------------
//  CLASS: TMEngSHA1HashVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngSHA1HashVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngSHA1HashVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngSHA1HashVal(const TMEngSHA1HashVal&) = delete;

        ~TMEngSHA1HashVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngSHA1HashVal& operator=(const TMEngSHA1HashVal&) = delete;


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

        tCIDLib::TVoid CopyFrom
        (
            const   TMEngClassVal&          mecvToCopy
            ,       TCIDMacroEngine&        meOwner
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TSHA1Hash& mhashValue() const;

        TSHA1Hash& mhashValue();

        tCIDLib::TVoid Reset();


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_phashValue
        //      The hash value, which is always the same number of bytes since
        //      it is defined by SHA1. We use a pointer so that our client
        //      code doesn't have to pull in the CIDCrypto facility just
        //      to get these classes.
        // -------------------------------------------------------------------
        TSHA1Hash*  m_pmhashValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngSHA1HashVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngSHA1HashInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngSHA1HashInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngSHA1HashInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngSHA1HashInfo(const TMEngSHA1HashInfo&) = delete;

        ~TMEngSHA1HashInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngSHA1HashInfo& operator=(const TMEngSHA1HashInfo&) = delete;


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
        tCIDLib::TCard2 m_c2MethId_CopyToMemBuf;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_GetAsString;
        tCIDLib::TCard2 m_c2MethId_GetByteCount;
        tCIDLib::TCard2 m_c2MethId_Reset;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngSHA1HashInfo,TMEngClassInfo)
};




// ---------------------------------------------------------------------------
//  CLASS: TMEngSHA1Val
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngSHA1Val : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngSHA1Val
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngSHA1Val(const TMEngSHA1Val&) = delete;

        ~TMEngSHA1Val();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngSHA1Val& operator=(const TMEngSHA1Val&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TSHA1Hasher& mdigValue() const;

        TSHA1Hasher& mdigValue();


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_mdigValue
        //      The SHA1 message digest object that is our value. We don't
        //      really expose it at the macro level. Since this is the only
        //      reason we have to bring in the CIDCrypto library, we make it
        //      a pointer and only import that library's header internally,
        //      so as not to foist it on clients.
        // -------------------------------------------------------------------
        TSHA1Hasher*    m_pmdigValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngSHA1Val,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngSHA1Info
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngSHA1Info : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngSHA1Info
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngSHA1Info(const TMEngSHA1Info&) = delete;

        ~TMEngSHA1Info();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngSHA1Info& operator=(const TMEngSHA1Info&) = delete;


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
        )   ;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatching, which we handle at the class level
        //      since this is a fully 'external' class, we store the ids of
        //      our methods as we register them.

        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_DigestBuf;
        tCIDLib::TCard2 m_c2MethId_DigestStr;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngSHA1Info,TMEngClassInfo)
};


#pragma CIDLIB_POPPACK




