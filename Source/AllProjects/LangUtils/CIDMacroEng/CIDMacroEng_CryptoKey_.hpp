//
// FILE NAME: CIDMacroEng_CryptoKey_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/08/2006
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
//  This is the header file for the CIDMacroEng_CryptoKey.cpp file, which
//  implements the info/value derivatives needed to create a standard CIDLib
//  cryptogaphic key. It's just a wrapper around the TCryptoKey C++ class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TCryptoKey;


// ---------------------------------------------------------------------------
//  CLASS: TMEngCryptoKeyVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCryptoKeyVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCryptoKeyVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngCryptoKeyVal(const TMEngCryptoKeyVal&) = delete;

        ~TMEngCryptoKeyVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCryptoKeyVal& operator=(const TMEngCryptoKeyVal&) = delete;


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
        const TCryptoKey& ckeyValue() const;

        TCryptoKey& ckeyValue();

        tCIDLib::TVoid Reset();


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_pckeyValue
        //      The hash value, which we do via a pointer so that we don't
        //      have to force the underlying crypto classes on everyone.
        // -------------------------------------------------------------------
        TCryptoKey* m_pckeyValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCryptoKeyVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngCryptoKeyInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCryptoKeyInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCryptoKeyInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngCryptoKeyInfo(const TMEngCryptoKeyInfo&) = delete;

        ~TMEngCryptoKeyInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCryptoKeyInfo& operator=(const TMEngCryptoKeyInfo&) = delete;


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
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_FromHash;
        tCIDLib::TCard2 m_c2MethId_FromString;
        tCIDLib::TCard2 m_c2MethId_HashCtor;
        tCIDLib::TCard2 m_c2MethId_Reset;
        tCIDLib::TCard2 m_c2MethId_StrCtor;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCryptoKeyInfo,TMEngClassInfo)
};


#pragma CIDLIB_POPPACK


