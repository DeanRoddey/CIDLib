//
// FILE NAME: CIDMacroEng_CardClasses.hpp
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
//  This is the header file for the CIDMacroEng_CardClasses.cpp file, which
//  implements some derivatives of the class info and class value classes that
//  are needed to create a new macro class. Each new class requires two
//  derivatives, one that describes the class (and can create new instance
//  data for that type) and another that provides those instance data objects
//  for classes of that type.
//
//  We provide these derivatives here for 1, 2, 4 byte cardinal types.
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
//  CLASS: TMEngCard1Val
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCard1Val : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCard1Val
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
            , const tCIDLib::TCard1         c1Init = 0
        );

        TMEngCard1Val(const TMEngCard1Val&) = delete;

        ~TMEngCard1Val();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCard1Val& operator=(const TMEngCard1Val&) = delete;


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
        tCIDLib::TCard1 c1Value() const;

        tCIDLib::TCard1 c1Value
        (
            const   tCIDLib::TCard1         c1ToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_c1Value
        //      This is the storage for values of this type.
        // -------------------------------------------------------------------
        tCIDLib::TCard1 m_c1Value;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCard1Val,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngCard1Info
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCard1Info : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCard1Info
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngCard1Info(const TMEngCard1Info&) = delete;

        ~TMEngCard1Info();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCard1Info& operator=(const TMEngCard1Info&) = delete;


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
        tCIDLib::TCard2 m_c2MethId_Add;
        tCIDLib::TCard2 m_c2MethId_And;
        tCIDLib::TCard2 m_c2MethId_AndEq;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Dec;
        tCIDLib::TCard2 m_c2MethId_Div;
        tCIDLib::TCard2 m_c2MethId_DivEq;
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_GetBit;
        tCIDLib::TCard2 m_c2MethId_GetHighNibble;
        tCIDLib::TCard2 m_c2MethId_GetLowNibble;
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
        tCIDLib::TCard2 m_c2MethId_Or;
        tCIDLib::TCard2 m_c2MethId_OrEq;
        tCIDLib::TCard2 m_c2MethId_PlusEq;
        tCIDLib::TCard2 m_c2MethId_SetBit;
        tCIDLib::TCard2 m_c2MethId_ShiftL;
        tCIDLib::TCard2 m_c2MethId_ShiftR;
        tCIDLib::TCard2 m_c2MethId_Sub;
        tCIDLib::TCard2 m_c2MethId_ValCtor;
        tCIDLib::TCard2 m_c2MethId_Xor;
        tCIDLib::TCard2 m_c2MethId_XorEq;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCard1Info,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngCard2Val
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCard2Val : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCard2Val
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
            , const tCIDLib::TCard2         c2Init = 0
        );

        TMEngCard2Val(const TMEngCard2Val&) = delete;

        ~TMEngCard2Val();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCard2Val& operator=(const TMEngCard2Val&) = delete;


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
        tCIDLib::TCard2 c2Value() const;

        tCIDLib::TCard2 c2Value
        (
            const   tCIDLib::TCard2         c2ToSet
        );

        tCIDLib::TVoid SwapBytes();



    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_c2Value
        //      This is the storage for values of this type.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2Value;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCard2Val,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngCard2Info
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCard2Info : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCard2Info
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngCard2Info(const TMEngCard2Info&) = delete;

        ~TMEngCard2Info();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCard2Info& operator=(const TMEngCard2Info&) = delete;


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
        tCIDLib::TCard2 m_c2MethId_Add;
        tCIDLib::TCard2 m_c2MethId_And;
        tCIDLib::TCard2 m_c2MethId_AndEq;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Dec;
        tCIDLib::TCard2 m_c2MethId_Div;
        tCIDLib::TCard2 m_c2MethId_DivEq;
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_GetBit;
        tCIDLib::TCard2 m_c2MethId_GetHighByte;
        tCIDLib::TCard2 m_c2MethId_GetLowByte;
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
        tCIDLib::TCard2 m_c2MethId_Or;
        tCIDLib::TCard2 m_c2MethId_OrEq;
        tCIDLib::TCard2 m_c2MethId_PlusEq;
        tCIDLib::TCard2 m_c2MethId_SetBit;
        tCIDLib::TCard2 m_c2MethId_ShiftL;
        tCIDLib::TCard2 m_c2MethId_ShiftR;
        tCIDLib::TCard2 m_c2MethId_Sub;
        tCIDLib::TCard2 m_c2MethId_SwapBytes;
        tCIDLib::TCard2 m_c2MethId_ValCtor;
        tCIDLib::TCard2 m_c2MethId_Xor;
        tCIDLib::TCard2 m_c2MethId_XorEq;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCard2Info,TMEngClassInfo)
};




// ---------------------------------------------------------------------------
//  CLASS: TMEngCard4Val
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCard4Val : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCard4Val
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
            , const tCIDLib::TCard4         c4Init = 0
        );

        TMEngCard4Val(const TMEngCard4Val&) = delete;

        ~TMEngCard4Val();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCard4Val& operator=(const TMEngCard4Val&) = delete;


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
        tCIDLib::TCard4 c4Value() const;

        tCIDLib::TCard4 c4Value
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid SwapBytes();


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_c4Value
        //      This is the storage for values of this type.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Value;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCard4Val,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngCard4Info
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCard4Info : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCard4Info
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngCard4Info(const TMEngCard4Info&) = delete;

        ~TMEngCard4Info();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCard4Info& operator=(const TMEngCard4Info&) = delete;


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
        tCIDLib::TCard2 m_c2MethId_Add;
        tCIDLib::TCard2 m_c2MethId_And;
        tCIDLib::TCard2 m_c2MethId_AndEq;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Dec;
        tCIDLib::TCard2 m_c2MethId_Div;
        tCIDLib::TCard2 m_c2MethId_DivEq;
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_GetBit;
        tCIDLib::TCard2 m_c2MethId_GetByte1;
        tCIDLib::TCard2 m_c2MethId_GetByte2;
        tCIDLib::TCard2 m_c2MethId_GetByte3;
        tCIDLib::TCard2 m_c2MethId_GetByte4;
        tCIDLib::TCard2 m_c2MethId_GetHighWord;
        tCIDLib::TCard2 m_c2MethId_GetLowWord;
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
        tCIDLib::TCard2 m_c2MethId_Or;
        tCIDLib::TCard2 m_c2MethId_OrEq;
        tCIDLib::TCard2 m_c2MethId_PercentOf;
        tCIDLib::TCard2 m_c2MethId_PlusEq;
        tCIDLib::TCard2 m_c2MethId_SetBit;
        tCIDLib::TCard2 m_c2MethId_ShiftL;
        tCIDLib::TCard2 m_c2MethId_ShiftR;
        tCIDLib::TCard2 m_c2MethId_Sub;
        tCIDLib::TCard2 m_c2MethId_SwapBytes;
        tCIDLib::TCard2 m_c2MethId_ValCtor;
        tCIDLib::TCard2 m_c2MethId_Xor;
        tCIDLib::TCard2 m_c2MethId_XorEq;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCard4Info,TMEngClassInfo)
};




// ---------------------------------------------------------------------------
//  CLASS: TMEngCard8Val
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCard8Val : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCard8Val
        (
            const   TString&                strName
            , const tCIDMacroEng::EConstTypes  eConst
            , const tCIDLib::TCard8         c8Init = 0
        );

        TMEngCard8Val(const TMEngCard8Val&) = delete;

        ~TMEngCard8Val();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCard8Val& operator=(const TMEngCard8Val&) = delete;


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
        tCIDLib::TVoid Add
        (
            const   tCIDLib::TCard8         c8ToSet
        );

        tCIDLib::TCard8 c8Value() const;

        tCIDLib::TCard8 c8Value
        (
            const   tCIDLib::TCard8         c8ToSet
        );

        tCIDLib::TVoid SwapBytes();


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_c8Value
        //      This is the storage for values of this type.
        // -------------------------------------------------------------------
        tCIDLib::TCard8 m_c8Value;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCard8Val,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngCard8Info
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCard8Info : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCard8Info
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngCard8Info(const TMEngCard8Info&) = delete;

        ~TMEngCard8Info();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCard8Info& operator=(const TMEngCard8Info&) = delete;


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
        tCIDLib::TCard2 m_c2MethId_Add;
        tCIDLib::TCard2 m_c2MethId_And;
        tCIDLib::TCard2 m_c2MethId_AndEq;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Dec;
        tCIDLib::TCard2 m_c2MethId_Div;
        tCIDLib::TCard2 m_c2MethId_DivEq;
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_GetBit;
        tCIDLib::TCard2 m_c2MethId_GetByte;
        tCIDLib::TCard2 m_c2MethId_GetHighDWord;
        tCIDLib::TCard2 m_c2MethId_GetLowDWord;
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
        tCIDLib::TCard2 m_c2MethId_Or;
        tCIDLib::TCard2 m_c2MethId_OrEq;
        tCIDLib::TCard2 m_c2MethId_PercentOf;
        tCIDLib::TCard2 m_c2MethId_PlusEq;
        tCIDLib::TCard2 m_c2MethId_SetBit;
        tCIDLib::TCard2 m_c2MethId_ShiftL;
        tCIDLib::TCard2 m_c2MethId_ShiftR;
        tCIDLib::TCard2 m_c2MethId_Sub;
        tCIDLib::TCard2 m_c2MethId_SwapBytes;
        tCIDLib::TCard2 m_c2MethId_ValCtor;
        tCIDLib::TCard2 m_c2MethId_Xor;
        tCIDLib::TCard2 m_c2MethId_XorEq;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCard8Info,TMEngClassInfo)
};


#pragma CIDLIB_POPPACK


