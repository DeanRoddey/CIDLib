//
// FILE NAME: CIDMacroEng_BooleanClass.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_BooleanClass.cpp file, which
//  implements derivatives of the class info and class value classes that
//  are needed to create a new macro class. We provide these derivatives here
//  for a boolean (true/false) type.
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
//  CLASS: TMEngBooleanVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngBooleanVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngBooleanVal
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
            , const tCIDLib::TBoolean       bInit = kCIDLib::False
        );

        TMEngBooleanVal(const TMEngBooleanVal&) = delete;

        ~TMEngBooleanVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngBooleanVal& operator=(const TMEngBooleanVal&) = delete;


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

        )   const  override;

        tCIDLib::TBoolean bParseFromText
        (
            const  TString&                 strValue
            , const TMEngClassInfo&         meciClass
        )   override;

        tCIDLib::TVoid CopyFrom
        (
            const   TMEngClassVal&          mecvToCopy
            ,       TCIDMacroEngine&        meOwner
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid And
        (
            const   tCIDLib::TBoolean       bAndWith
        );

        tCIDLib::TBoolean bValue() const;

        tCIDLib::TBoolean bValue
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TVoid Negate();

        tCIDLib::TVoid Or
        (
            const   tCIDLib::TBoolean       bOrWith
        );

        tCIDLib::TVoid Xor
        (
            const   tCIDLib::TBoolean       bXorWith
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_bValue
        //      This is the storage for values of this type.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngBooleanVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngBooleanInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngBooleanInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngBooleanInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        ~TMEngBooleanInfo();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDMacroEng::ECastRes eCastFrom
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngClassVal&          mecvSrc
            ,       TMEngClassVal&          mecvTarget
        )   override;

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
        //  Public operators
        // -------------------------------------------------------------------
        TMEngBooleanInfo(const TMEngBooleanInfo&) = delete;
        TMEngBooleanInfo& operator=(const TMEngBooleanInfo&) = delete;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_And;
        tCIDLib::TCard2 m_c2MethId_AndEq;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_Negate;
        tCIDLib::TCard2 m_c2MethId_Or;
        tCIDLib::TCard2 m_c2MethId_OrEq;
        tCIDLib::TCard2 m_c2MethId_Set;
        tCIDLib::TCard2 m_c2MethId_ValCtor;
        tCIDLib::TCard2 m_c2MethId_Xor;
        tCIDLib::TCard2 m_c2MethId_XorEq;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngBooleanInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


