//
// FILE NAME: CIDMacroEng_DBaseClasses_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/08/2006
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
//  This is the header file for the CIDMacroEng_DBaseClass.cpp file, which
//  implements the info and value classes required to create CML level
//  classes for interfacing to an ODBC driver.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TDBConnection;
class TDBStatement;


// ---------------------------------------------------------------------------
//  CLASS: TMEngDBConnVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngDBConnVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngDBConnVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngDBConnVal(const TMEngDBConnVal&) = delete;

        ~TMEngDBConnVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngDBConnVal& operator=(const TMEngDBConnVal&) = delete;


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
        tCIDLib::TBoolean bConnected() const;

        const TDBConnection& dbconnValue() const;

        TDBConnection& dbconnValue();


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_pdbconnVal
        //      The underlying database connection object. It's a pointer
        //      because we don't want to force the underly CIDDBase headers
        //      on everyone downstream.
        // -------------------------------------------------------------------
        mutable TDBConnection*  m_pdbconnVal;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngDBConnVal,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngDBConnInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngDBConnInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngDBConnInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngDBConnInfo(const TMEngDBConnInfo&) = delete;

        ~TMEngDBConnInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngDBConnInfo& operator=(const TMEngDBConnInfo&) = delete;


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
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const TString&                strText
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        //
        //  m_c4ErrXXX
        //      We have an error enum, so we store the ordinals so that we
        //      can throw macro level errors internally if needed.
        //
        //  m_pmeciErrors
        //      A pointer the error enum we create for our errors.
        // -------------------------------------------------------------------
        tCIDLib::TCard2     m_c2EnumId_Errors;
        tCIDLib::TCard2     m_c2MethId_Connect;
        tCIDLib::TCard2     m_c2MethId_Connect2;
        tCIDLib::TCard2     m_c2MethId_ConnectStr;
        tCIDLib::TCard2     m_c2MethId_DefCtor;
        tCIDLib::TCard2     m_c2MethId_Disconnect;
        tCIDLib::TCard2     m_c2MethId_GetSrcList;
        tCIDLib::TCard2     m_c2MethId_GetSrcName;
        tCIDLib::TCard2     m_c2TypeId_KVPair;
        tCIDLib::TCard2     m_c2TypeId_SrcList;
        tCIDLib::TCard4     m_c4ErrAlreadyConnected;
        tCIDLib::TCard4     m_c4ErrConnFailed;
        tCIDLib::TCard4     m_c4ErrDisconnFailed;
        tCIDLib::TCard4     m_c4ErrNotConnected;
        tCIDLib::TCard4     m_c4ErrGenError;
        TMEngEnumInfo*      m_pmeciErrors;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngDBConnInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngDBStmtVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngDBStmtVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngDBStmtVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngDBStmtVal(const TMEngDBStmtVal&) = delete;

        ~TMEngDBStmtVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngDBStmtVal& operator=(const TMEngDBStmtVal&) = delete;


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
        tCIDLib::TBoolean bIsReady() const;

        const TDBDirStatement& dbstmtValue() const;

        TDBDirStatement& dbstmtValue();

        tCIDLib::TVoid Initialize
        (
            const   TDBConnection&          dbconTarget
            , const TString&                strName
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_pdbstmtThis
        //      The underlying database statement object. It's a pointer
        //      because we don't want to force the underly CIDDBase headers
        //      on everyone downstream.
        // -------------------------------------------------------------------
        TDBDirStatement*    m_pdbstmtVal;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngDBStmtVal,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngDBStmtInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngDBStmtInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngDBStmtInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngDBStmtInfo(const TMEngDBStmtInfo&) = delete;

        ~TMEngDBStmtInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngDBStmtInfo& operator=(const TMEngDBStmtInfo&) = delete;


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
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const TString&                strText
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            ,       TError&                 errToThrow
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2EnumId_XXX
        //      We have some nested enums, so we store the id for quick
        //      use later.
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        //
        //  m_c4ErrXXX
        //      We have an error enum, so we store the ordinals so that we
        //      can throw macro level errors internally if needed.
        //
        //  m_pmeciErrors
        //      A pointer the error enum we create for our errors.
        // -------------------------------------------------------------------
        tCIDLib::TCard2     m_c2EnumId_ColTypes;
        tCIDLib::TCard2     m_c2EnumId_Errors;
        tCIDLib::TCard2     m_c2MethId_AddBinding;
        tCIDLib::TCard2     m_c2MethId_AddStrBinding;
        tCIDLib::TCard2     m_c2MethId_DefCtor;
        tCIDLib::TCard2     m_c2MethId_Execute;
        tCIDLib::TCard2     m_c2MethId_Fetch;
        tCIDLib::TCard2     m_c2MethId_InitCtor;
        tCIDLib::TCard2     m_c2MethId_GetBoolCol;
        tCIDLib::TCard2     m_c2MethId_GetCard1Col;
        tCIDLib::TCard2     m_c2MethId_GetCard2Col;
        tCIDLib::TCard2     m_c2MethId_GetCard4Col;
        tCIDLib::TCard2     m_c2MethId_GetCard8Col;
        tCIDLib::TCard2     m_c2MethId_GetDateCol;
        tCIDLib::TCard2     m_c2MethId_GetInt1Col;
        tCIDLib::TCard2     m_c2MethId_GetInt2Col;
        tCIDLib::TCard2     m_c2MethId_GetInt4Col;
        tCIDLib::TCard2     m_c2MethId_GetInt8Col;
        tCIDLib::TCard2     m_c2MethId_GetName;
        tCIDLib::TCard2     m_c2MethId_GetStrCol;
        tCIDLib::TCard2     m_c2MethId_GetTimeCol;
        tCIDLib::TCard2     m_c2MethId_IsColNull;
        tCIDLib::TCard2     m_c2MethId_Reset;
        tCIDLib::TCard4     m_c4ErrAddBinding;
        tCIDLib::TCard4     m_c4ErrExecute;
        tCIDLib::TCard4     m_c4ErrFetch;
        tCIDLib::TCard4     m_c4ErrGetCol;
        tCIDLib::TCard4     m_c4ErrGetName;
        tCIDLib::TCard4     m_c4ErrNotReady;
        tCIDLib::TCard4     m_c4ErrReset;
        TMEngEnumInfo*      m_pmeciColTypes;
        TMEngEnumInfo*      m_pmeciErrors;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngDBStmtInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


