//
// FILE NAME: CIDMacroEng_AsyncHTTP_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/17/2005
//
// COPYRIGHT: $_CIDLib_CopyRight_$
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
//  This is the header file for the CIDMacroEng_AsyncHTTP.cpp file, which implements
//  the info and value classes required to create a CML class for doing async HTTP
//  operations.
//
//  We use the underlying C++ classes via pointers so that we don't have to
//  impose the CIDNet headers on everyone who uses this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TAsyncHTTPQ;


// ---------------------------------------------------------------------------
//  CLASS: TMEngAsyncHTTPClVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngAsyncHTTPClVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngAsyncHTTPClVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes eConst
        );

        TMEngAsyncHTTPClVal(const TMEngAsyncHTTPClVal&) = delete;

        ~TMEngAsyncHTTPClVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngAsyncHTTPClVal& operator=(const TMEngAsyncHTTPClVal&) = delete;


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
        TAsyncHTTPQ& ahttpqValue();

        tCIDLib::EAsyncWaitRes eState() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pahttpqValue
        //      The async HTTP query object that is our value. We use a pointer
        //      so as to not force the CIDNet headers on our downstreamd clients.
        // -------------------------------------------------------------------
        TAsyncHTTPQ*    m_pahttpqValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngAsyncHTTPClVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngAsyncHTTPClInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngAsyncHTTPClInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngAsyncHTTPClInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngAsyncHTTPClInfo(const TMEngAsyncHTTPClInfo&) = delete;

        ~TMEngAsyncHTTPClInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngAsyncHTTPClInfo& operator=(const TMEngAsyncHTTPClInfo&) = delete;


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
        tCIDLib::TCard2         m_c2MethId_CancelOp;
        tCIDLib::TCard2         m_c2MethId_DefCtor;
        tCIDLib::TCard2         m_c2MethId_GetOutput;
        tCIDLib::TCard2         m_c2MethId_GetStatus;
        tCIDLib::TCard2         m_c2MethId_StartGETRedir;
        tCIDLib::TCard2         m_c2MethId_StartPOST;
        tCIDLib::TCard2         m_c2TypeId_HdrList;
        tCIDLib::TCard2         m_c2TypeId_KVPair;
        tCIDLib::TCard2         m_c2TypeId_Errors;
        tCIDLib::TCard2         m_c2TypeId_States;
        tCIDLib::TCard4         m_c4ErrId_Busy;
        tCIDLib::TCard4         m_c4ErrId_Cancel;
        tCIDLib::TCard4         m_c4ErrId_Output;
        tCIDLib::TCard4         m_c4ErrId_Start;
        tCIDLib::TCard4         m_c4ErrId_Status;
        TMEngEnumInfo*          m_pmeciErrors;
        TMEngEnumInfo*          m_pmeciStates;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngAsyncHTTPClInfo,TMEngClassInfo)
};


#pragma CIDLIB_POPPACK

