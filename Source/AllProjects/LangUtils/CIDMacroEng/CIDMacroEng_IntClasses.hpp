//
// FILE NAME: CIDMacroEng_BaseClasses.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2003
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
//  This is the header file for the CIDMacroEng_IntClasses.cpp file, which
//  implements some derivatives of the classinfo and class value classes that
//  are needed to create a new type. Each new type requires two derivatives,
//  one that describes the class (and can create new value object for that
//  class) and another that provides the instance data storage for class of
//  that type.
//
//  We provide these derivatives here for 1, 2, 4 byte integral types. These
//  are fundamental intrinsic types.
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
//  CLASS: TMEngInt1Val
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngInt1Val : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TMEngInt1Val
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
            , const tCIDLib::TInt1          i1Init = 0
        );

        TMEngInt1Val(const TMEngInt1Val&) = delete;

        ~TMEngInt1Val();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngInt1Val& operator=(const TMEngInt1Val&) = delete;


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

        tCIDLib::TBoolean bParseFromText
        (
            const   TString&                strValue
            , const TMEngClassInfo&         mecvThis
        )   override;

        tCIDLib::TVoid CopyFrom
        (
            const   TMEngClassVal&          mecvToCopy
            ,       TCIDMacroEngine&        meOwner
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TInt1 i1Value() const;

        tCIDLib::TInt1 i1Value
        (
            const   tCIDLib::TInt1          i1ToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_i1Value
        //      This is the storage for values of this type.
        // -------------------------------------------------------------------
        tCIDLib::TInt1 m_i1Value;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngInt1Val,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngInt1Info
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngInt1Info : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngInt1Info
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngInt1Info(const TMEngInt1Info&) = delete;

        ~TMEngInt1Info();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngInt1Info& operator=(const TMEngInt1Info&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCanCastFrom
        (
            const   tCIDLib::TCard2         c2SrcId
        )   override;

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
        //  Public data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_AbsValue;
        tCIDLib::TCard2 m_c2MethId_Add;
        tCIDLib::TCard2 m_c2MethId_Dec;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Div;
        tCIDLib::TCard2 m_c2MethId_DivEq;
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_GetNegated;
        tCIDLib::TCard2 m_c2MethId_GtThan;
        tCIDLib::TCard2 m_c2MethId_GtThanEq;
        tCIDLib::TCard2 m_c2MethId_Inc;
        tCIDLib::TCard2 m_c2MethId_LsThan;
        tCIDLib::TCard2 m_c2MethId_LsThanEq;
        tCIDLib::TCard2 m_c2MethId_MaxVal;
        tCIDLib::TCard2 m_c2MethId_MinVal;
        tCIDLib::TCard2 m_c2MethId_MinusEq;
        tCIDLib::TCard2 m_c2MethId_ModDiv;
        tCIDLib::TCard2 m_c2MethId_ModDivEq;
        tCIDLib::TCard2 m_c2MethId_Mul;
        tCIDLib::TCard2 m_c2MethId_MulEq;
        tCIDLib::TCard2 m_c2MethId_Negate;
        tCIDLib::TCard2 m_c2MethId_PlusEq;
        tCIDLib::TCard2 m_c2MethId_Sub;
        tCIDLib::TCard2 m_c2MethId_ValCtor;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngInt1Info,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngInt2Val
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngInt2Val : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngInt2Val
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
            , const tCIDLib::TInt2          i2Init = 0
        );

        TMEngInt2Val(const TMEngInt2Val&) = delete;

        ~TMEngInt2Val();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngInt2Val& operator=(const TMEngInt2Val&) = delete;


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

        tCIDLib::TBoolean bParseFromText
        (
            const   TString&                strValue
            , const TMEngClassInfo&         mecvThis
        )   override;

        tCIDLib::TVoid CopyFrom
        (
            const   TMEngClassVal&          mecvToCopy
            ,       TCIDMacroEngine&        meOwner
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TInt2 i2Value() const;

        tCIDLib::TInt2 i2Value
        (
            const   tCIDLib::TInt2          i2ToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_i2Value
        //      This is the storage for values of this type.
        // -------------------------------------------------------------------
        tCIDLib::TInt2 m_i2Value;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngInt2Val,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngInt2Info
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngInt2Info : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngInt2Info
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngInt2Info(const TMEngInt2Info&) = delete;

        ~TMEngInt2Info();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngInt2Info& operator=(const TMEngInt2Info&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCanCastFrom
        (
            const   tCIDLib::TCard2         c2SrcId
        )   override;

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
        //  Public data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_AbsValue;
        tCIDLib::TCard2 m_c2MethId_Add;
        tCIDLib::TCard2 m_c2MethId_Dec;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Div;
        tCIDLib::TCard2 m_c2MethId_DivEq;
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_GetNegated;
        tCIDLib::TCard2 m_c2MethId_GtThan;
        tCIDLib::TCard2 m_c2MethId_GtThanEq;
        tCIDLib::TCard2 m_c2MethId_Inc;
        tCIDLib::TCard2 m_c2MethId_LsThan;
        tCIDLib::TCard2 m_c2MethId_LsThanEq;
        tCIDLib::TCard2 m_c2MethId_MaxVal;
        tCIDLib::TCard2 m_c2MethId_MinVal;
        tCIDLib::TCard2 m_c2MethId_MinusEq;
        tCIDLib::TCard2 m_c2MethId_ModDiv;
        tCIDLib::TCard2 m_c2MethId_ModDivEq;
        tCIDLib::TCard2 m_c2MethId_Mul;
        tCIDLib::TCard2 m_c2MethId_MulEq;
        tCIDLib::TCard2 m_c2MethId_Negate;
        tCIDLib::TCard2 m_c2MethId_PlusEq;
        tCIDLib::TCard2 m_c2MethId_Sub;
        tCIDLib::TCard2 m_c2MethId_ValCtor;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngInt2Info,TMEngClassInfo)
};




// ---------------------------------------------------------------------------
//  CLASS: TMEngInt4Val
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngInt4Val : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngInt4Val
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
            , const tCIDLib::TInt4          i4Init = 0
        );

        TMEngInt4Val(const TMEngInt4Val&) = delete;

        ~TMEngInt4Val();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngInt4Val& operator=(const TMEngInt4Val&) = delete;


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

        tCIDLib::TBoolean bParseFromText
        (
            const   TString&                strValue
            , const TMEngClassInfo&         mecvThis
        )   override;

        tCIDLib::TVoid CopyFrom
        (
            const   TMEngClassVal&          mecvToCopy
            ,       TCIDMacroEngine&        meOwner
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TInt4 i4Value() const;

        tCIDLib::TInt4 i4Value
        (
            const   tCIDLib::TInt4          i4ToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_i4Value
        //      This is the storage for values of this type.
        // -------------------------------------------------------------------
        tCIDLib::TInt4 m_i4Value;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngInt4Val,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngInt4Info
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngInt4Info : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngInt4Info
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngInt4Info(const TMEngInt4Info&) = delete;

        ~TMEngInt4Info();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngInt4Info& operator=(const TMEngInt4Info&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCanCastFrom
        (
            const   tCIDLib::TCard2         c2SrcId
        )   override;

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
        //  Public data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_AbsValue;
        tCIDLib::TCard2 m_c2MethId_Add;
        tCIDLib::TCard2 m_c2MethId_Dec;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Div;
        tCIDLib::TCard2 m_c2MethId_DivEq;
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_GetNegated;
        tCIDLib::TCard2 m_c2MethId_GtThan;
        tCIDLib::TCard2 m_c2MethId_GtThanEq;
        tCIDLib::TCard2 m_c2MethId_Inc;
        tCIDLib::TCard2 m_c2MethId_LsThan;
        tCIDLib::TCard2 m_c2MethId_LsThanEq;
        tCIDLib::TCard2 m_c2MethId_MaxVal;
        tCIDLib::TCard2 m_c2MethId_MinVal;
        tCIDLib::TCard2 m_c2MethId_MinusEq;
        tCIDLib::TCard2 m_c2MethId_ModDiv;
        tCIDLib::TCard2 m_c2MethId_ModDivEq;
        tCIDLib::TCard2 m_c2MethId_Mul;
        tCIDLib::TCard2 m_c2MethId_MulEq;
        tCIDLib::TCard2 m_c2MethId_Negate;
        tCIDLib::TCard2 m_c2MethId_PlusEq;
        tCIDLib::TCard2 m_c2MethId_Sub;
        tCIDLib::TCard2 m_c2MethId_ValCtor;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngInt4Info,TMEngClassInfo)
};


#pragma CIDLIB_POPPACK


