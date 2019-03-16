//
// FILE NAME: CIDMacroEng_KVPairClass.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/16/2003
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
//  This is the header file for the CIDMacroEng_KVPairClass.cpp file, which
//  implements a key/value pair class, each instance of which holds two
//  strings, one the key and the other a value associated with that key.
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
//  CLASS: TMEngKVPairVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngKVPairVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngKVPairVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngKVPairVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
            , const TString&                strKey
            , const TString&                strValue
        );

        TMEngKVPairVal(const TMEngKVPairVal&) = delete;

        ~TMEngKVPairVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngKVPairVal& operator=(const TMEngKVPairVal&) = delete;


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
        tCIDLib::TVoid Reset();

        const TString& strKey() const;

        const TString& strKey
        (
            const   TString&                strToSet
        );

        const TString& strValue() const;

        const TString& strValue
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid Set
        (
            const   TString&                strKey
            , const TString&                strValue
        );

        tCIDLib::TVoid SetBoolean
        (
            const   tCIDLib::TBoolean       bValue
        );

        tCIDLib::TVoid SetCard4
        (
            const   tCIDLib::TCard4         c4Value
        );

        tCIDLib::TVoid SetFloat8
        (
            const   tCIDLib::TFloat8        f8Value
        );

        tCIDLib::TVoid SetInt4
        (
            const   tCIDLib::TInt4          i4Value
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_strKey
        //  m_strValue
        //      The two values of this object
        // -------------------------------------------------------------------
        TString m_strKey;
        TString m_strValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngKVPairVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngKVPairInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngKVPairInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngKVPairInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngKVPairInfo(const TMEngKVPairInfo&) = delete;

        ~TMEngKVPairInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngKVPairInfo& operator=(const TMEngKVPairInfo&) = delete;


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
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_CopyKey;
        tCIDLib::TCard2 m_c2MethId_CopyValue;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_GetKey;
        tCIDLib::TCard2 m_c2MethId_GetValue;
        tCIDLib::TCard2 m_c2MethId_MatchesKey;
        tCIDLib::TCard2 m_c2MethId_MatchesValue;
        tCIDLib::TCard2 m_c2MethId_SameKey;
        tCIDLib::TCard2 m_c2MethId_SameValue;
        tCIDLib::TCard2 m_c2MethId_SetKey;
        tCIDLib::TCard2 m_c2MethId_SetValue;
        tCIDLib::TCard2 m_c2MethId_Set;
        tCIDLib::TCard2 m_c2MethId_SetBoolean;
        tCIDLib::TCard2 m_c2MethId_SetCard4;
        tCIDLib::TCard2 m_c2MethId_SetFloat8;
        tCIDLib::TCard2 m_c2MethId_SetInt4;
        tCIDLib::TCard2 m_c2MethId_ValCtor;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngKVPairInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


