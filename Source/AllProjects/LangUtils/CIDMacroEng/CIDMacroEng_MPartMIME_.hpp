//
// FILE NAME: CIDMacroEng_MPartMIME_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/01/2017
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2019
//
//  This file and its contents are the property of Charmed Quark Software. It
//  may not be redistributed in any modified form, or as part of any product
//  other than the original CIDLib C++ Frameworks distribution file. Its
//  contents may not be compiled into any other product, except when required
//  by use of the CIDLib C++ Frameworks product. Such use in any Sockercial
//  product is subject to any relevant licensing agreements.
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_MPartMIME.cpp file, which implements
//  the info and value classes required to create a CML class that can parse out
//  the contents of a multi-part MIME packet. It will return an array of objects that
//  represent each of the chunks.
//
//  Our volume object has no contents at this time, since this class exists just to
//  parse an incoming memory buffer and give back a set of buffers. So it doesn't
//  currently need any state of its own.
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
//  CLASS: TMEngMPartMIMEVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngMPartMIMEVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngMPartMIMEVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngMPartMIMEVal(const TMEngMPartMIMEVal&) = delete;

        ~TMEngMPartMIMEVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngMPartMIMEVal& operator=(const TMEngMPartMIMEVal&) = delete;


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


    private :
        // -------------------------------------------------------------------
        //  Private data members

        // -------------------------------------------------------------------


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngMPartMIMEVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngMPartMIMEInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngMPartMIMEInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngMPartMIMEInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngMPartMIMEInfo(const TMEngMPartMIMEInfo&) = delete;

        ~TMEngMPartMIMEInfo();


        // -------------------------------------------------------------------
        //  Public operator
        // -------------------------------------------------------------------
        TMEngMPartMIMEInfo& operator=(const TMEngMPartMIMEInfo&) = delete;


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
            , const TError&                 errCaught
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        //
        //  m_c2TypeId_XXX
        //      Some nested class ids and ids of other classes that we need
        //      to reference so we look them up and keep the id around.
        //
        //  m_c4ErrId_XXX
        //      The ids for our errors that we throw, for efficiency we save
        //      them away for later throwing.
        //
        //  m_pmeciXXX
        //      The class info for any types we want to keep up with (our
        //      nested or external.)
        // -------------------------------------------------------------------
        tCIDLib::TCard2         m_c2MethId_DefCtor;
        tCIDLib::TCard2         m_c2MethId_ParseBuf;
        tCIDLib::TCard2         m_c2TypeId_Errors;
        tCIDLib::TCard4         m_c4ErrId_Parse;
        TMEngEnumInfo*          m_pmeciErrors;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngMPartMIMEInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK

