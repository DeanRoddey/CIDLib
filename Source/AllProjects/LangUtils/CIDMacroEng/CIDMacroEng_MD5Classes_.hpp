//
// FILE NAME: CIDMacroEng_MD5Classes.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/16/2003
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
//  This is the header file for the CIDMacroEng_MD5Classes.cpp file,
//  which implements the info/value derivatives needed to create an MD5
//  hash digester and an MD5 hash value.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TMessageDigest5;
class TMD5Hash;


// ---------------------------------------------------------------------------
//  CLASS: TMEngMD5HashVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngMD5HashVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngMD5HashVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngMD5HashVal(const TMEngMD5HashVal&) = delete;

        ~TMEngMD5HashVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngMD5HashVal& operator=(const TMEngMD5HashVal&) = delete;


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
        const TMD5Hash& mhashValue() const;

        TMD5Hash& mhashValue();

        tCIDLib::TVoid Reset();


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_phashValue
        //      The hash value, which is always the same number of bytes since
        //      it is defined by MD5. We use a pointer so that our client
        //      codes doesn't have to pull in the CIDCrypto facility just
        //      to get these classes.
        // -------------------------------------------------------------------
        TMD5Hash*   m_pmhashValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngMD5HashVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngMD5HashInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngMD5HashInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngMD5HashInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngMD5HashInfo(const TMEngMD5HashInfo&) = delete;

        ~TMEngMD5HashInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngMD5HashInfo& operator=(const TMEngMD5HashInfo&) = delete;


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
        RTTIDefs(TMEngMD5HashInfo,TMEngClassInfo)
};




// ---------------------------------------------------------------------------
//  CLASS: TMEngMD5Val
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngMD5Val : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngMD5Val
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngMD5Val(const TMEngMD5Val&) = delete;

        ~TMEngMD5Val();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngMD5Val& operator=(const TMEngMD5Val&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TMessageDigest5& mdigValue() const;

        TMessageDigest5& mdigValue();


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_mdigValue
        //      The MD5 message digest object that is our value. We don't
        //      really expose it at the macro level. Since this is the only
        //      reason we have to bring in the CIDCrypto library, we make it
        //      a pointer and only import that library's header internally,
        //      so as not to foist it on clients.
        // -------------------------------------------------------------------
        TMessageDigest5* m_pmdigValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngMD5Val,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngMD5Info
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngMD5Info : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngMD5Info
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngMD5Info(const TMEngMD5Info&) = delete;

        ~TMEngMD5Info();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngMD5Info& operator=(const TMEngMD5Info&) = delete;


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
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_DigestBuf;
        tCIDLib::TCard2 m_c2MethId_DigestStr;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngMD5Info,TMEngClassInfo)
};


#pragma CIDLIB_POPPACK



