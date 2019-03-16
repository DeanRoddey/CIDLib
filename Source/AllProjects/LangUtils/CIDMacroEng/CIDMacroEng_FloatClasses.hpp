//
// FILE NAME: CIDMacroEng_FloatClasses.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/06/2003
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
//  This is the header file for the CIDMacroEng_FloatClasses.cpp file, which
//  implements some derivatives of the class info and class value classes that
//  are needed to create a new macro class. Each new class requires two
//  derivatives, one that describes the class (and can create new instance
//  data for that type) and another that provides those instance data objects
//  for classes of that type.
//
//  We provide these derivatives here for 4 and 8 byte floating point types.
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
//  CLASS: TMEngFloat4Val
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngFloat4Val : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngFloat4Val
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
            , const tCIDLib::TFloat4        f4Init = 0
        );

        TMEngFloat4Val(const TMEngFloat4Val&) = delete;

        ~TMEngFloat4Val();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngFloat4Val& operator=(const TMEngFloat4Val&) = delete;


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
        tCIDLib::TFloat4 f4Value() const;

        tCIDLib::TFloat4 f4Value
        (
            const   tCIDLib::TFloat4        f4ToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_f4Value
        //      This is the storage for values of this type.
        // -------------------------------------------------------------------
        tCIDLib::TFloat4 m_f4Value;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngFloat4Val,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngFloat4Info
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngFloat4Info : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngFloat4Info
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngFloat4Info(const TMEngFloat4Info&) = delete;

        ~TMEngFloat4Info();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngFloat4Info& operator=(const TMEngFloat4Info&) = delete;


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
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_Abs;
        tCIDLib::TCard2 m_c2MethId_Add;
        tCIDLib::TCard2 m_c2MethId_ArcCosine;
        tCIDLib::TCard2 m_c2MethId_ArcSine;
        tCIDLib::TCard2 m_c2MethId_ArcTangent;
        tCIDLib::TCard2 m_c2MethId_Ceiling;
        tCIDLib::TCard2 m_c2MethId_Cosine;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Div;
        tCIDLib::TCard2 m_c2MethId_DivEq;
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_Floor;
        tCIDLib::TCard2 m_c2MethId_GtThan;
        tCIDLib::TCard2 m_c2MethId_GtThanEq;
        tCIDLib::TCard2 m_c2MethId_LsThan;
        tCIDLib::TCard2 m_c2MethId_LsThanEq;
        tCIDLib::TCard2 m_c2MethId_MaxVal;
        tCIDLib::TCard2 m_c2MethId_MinVal;
        tCIDLib::TCard2 m_c2MethId_MinusEq;
        tCIDLib::TCard2 m_c2MethId_Mul;
        tCIDLib::TCard2 m_c2MethId_MulEq;
        tCIDLib::TCard2 m_c2MethId_Negate;
        tCIDLib::TCard2 m_c2MethId_NLog;
        tCIDLib::TCard2 m_c2MethId_PlusEq;
        tCIDLib::TCard2 m_c2MethId_Power;
        tCIDLib::TCard2 m_c2MethId_Round;
        tCIDLib::TCard2 m_c2MethId_Sine;
        tCIDLib::TCard2 m_c2MethId_Split;
        tCIDLib::TCard2 m_c2MethId_SqrRoot;
        tCIDLib::TCard2 m_c2MethId_Sub;
        tCIDLib::TCard2 m_c2MethId_ValCtor;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngFloat4Info,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngFloat8Val
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngFloat8Val : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngFloat8Val
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
            , const tCIDLib::TFloat8        f8Init = 0
        );

        TMEngFloat8Val(const TMEngFloat8Val&) = delete;

        ~TMEngFloat8Val();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngFloat8Val& operator=(const TMEngFloat8Val&) = delete;


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
        tCIDLib::TFloat8 f8Value() const;

        tCIDLib::TFloat8 f8Value
        (
            const   tCIDLib::TFloat8        f8ToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_f8Value
        //      This is the storage for values of this type.
        // -------------------------------------------------------------------
        tCIDLib::TFloat8    m_f8Value;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngFloat8Val,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngFloat8Info
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngFloat8Info : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngFloat8Info
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngFloat8Info(const TMEngFloat8Info&) = delete;

        ~TMEngFloat8Info();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngFloat8Info& operator=(const TMEngFloat8Info&) = delete;


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
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_Abs;
        tCIDLib::TCard2 m_c2MethId_Add;
        tCIDLib::TCard2 m_c2MethId_ArcCosine;
        tCIDLib::TCard2 m_c2MethId_ArcSine;
        tCIDLib::TCard2 m_c2MethId_ArcTangent;
        tCIDLib::TCard2 m_c2MethId_Ceiling;
        tCIDLib::TCard2 m_c2MethId_Cosine;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Div;
        tCIDLib::TCard2 m_c2MethId_DivEq;
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_Floor;
        tCIDLib::TCard2 m_c2MethId_GtThan;
        tCIDLib::TCard2 m_c2MethId_GtThanEq;
        tCIDLib::TCard2 m_c2MethId_LsThan;
        tCIDLib::TCard2 m_c2MethId_LsThanEq;
        tCIDLib::TCard2 m_c2MethId_MaxVal;
        tCIDLib::TCard2 m_c2MethId_MinVal;
        tCIDLib::TCard2 m_c2MethId_MinusEq;
        tCIDLib::TCard2 m_c2MethId_Mul;
        tCIDLib::TCard2 m_c2MethId_MulEq;
        tCIDLib::TCard2 m_c2MethId_Negate;
        tCIDLib::TCard2 m_c2MethId_NLog;
        tCIDLib::TCard2 m_c2MethId_PlusEq;
        tCIDLib::TCard2 m_c2MethId_Power;
        tCIDLib::TCard2 m_c2MethId_Round;
        tCIDLib::TCard2 m_c2MethId_Sine;
        tCIDLib::TCard2 m_c2MethId_Split;
        tCIDLib::TCard2 m_c2MethId_SqrRoot;
        tCIDLib::TCard2 m_c2MethId_Sub;
        tCIDLib::TCard2 m_c2MethId_ValCtor;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngFloat8Info,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


