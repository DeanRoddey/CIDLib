//
// FILE NAME: CIDMacroEng_CommClasses_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/22/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_CommClass.cpp file, which
//  implements the info and value classes required to create new macro level
//  classes for a comm port and a class that contains the info required to
//  configure a comm port.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TCommPortCfg;
class TCommPortBase;


// ---------------------------------------------------------------------------
//  CLASS: TMEngCommCfgVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCommCfgVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCommCfgVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngCommCfgVal(const TMEngCommCfgVal&) = delete;

        ~TMEngCommCfgVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCommCfgVal& operator=(const TMEngCommCfgVal&) = delete;


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
        TCommPortCfg& cpcfgValue();

        TCommPortCfg& cpcfgValue
        (
            const   TCommPortCfg&           cpcfgToSet
        );

        const TCommPortCfg& cpcfgValue() const;


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_pcpcfgValue
        //      This is the underlying C++ com port configuration object that
        //      provides the value for this class. We use a pointer so that
        //      we don't have to expose the comm headers to our clients.
        // -------------------------------------------------------------------
        TCommPortCfg*   m_pcpcfgValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCommCfgVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngCommCfgInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCommCfgInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCommCfgInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngCommCfgInfo(const TMEngCommCfgInfo&) = delete;

        ~TMEngCommCfgInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCommCfgInfo& operator=(const TMEngCommCfgInfo&) = delete;


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
        //
        //  m_c2TypeId_XXX
        //      We create a few nested enum types that represent various
        //      aspects of a COM port's configuration.
        //
        //  m_pmeciXXX
        //      We keep pointers to our registered enum types, because we'll
        //      need them during method handling. We don't own them.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_ClearPortFlag;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_GetBaud;
        tCIDLib::TCard2 m_c2MethId_GetDataBits;
        tCIDLib::TCard2 m_c2MethId_GetDTRFlag;
        tCIDLib::TCard2 m_c2MethId_GetParity;
        tCIDLib::TCard2 m_c2MethId_GetPortFlag;
        tCIDLib::TCard2 m_c2MethId_GetRTSFlag;
        tCIDLib::TCard2 m_c2MethId_GetStopBits;
        tCIDLib::TCard2 m_c2MethId_SetBaud;
        tCIDLib::TCard2 m_c2MethId_SetDataBits;
        tCIDLib::TCard2 m_c2MethId_SetDTRFlag;
        tCIDLib::TCard2 m_c2MethId_SetParity;
        tCIDLib::TCard2 m_c2MethId_SetPortFlag;
        tCIDLib::TCard2 m_c2MethId_SetRTSFlag;
        tCIDLib::TCard2 m_c2MethId_SetStopBits;
        tCIDLib::TCard2 m_c2MethId_ValCtor;
        tCIDLib::TCard2 m_c2TypeId_DataBits;
        tCIDLib::TCard2 m_c2TypeId_DTRFlags;
        tCIDLib::TCard2 m_c2TypeId_Parities;
        tCIDLib::TCard2 m_c2TypeId_PortFlags;
        tCIDLib::TCard2 m_c2TypeId_RTSFlags;
        tCIDLib::TCard2 m_c2TypeId_StopBits;
        TMEngEnumInfo*  m_pmeciDataBits;
        TMEngEnumInfo*  m_pmeciDTRFlags;
        TMEngEnumInfo*  m_pmeciParities;
        TMEngEnumInfo*  m_pmeciPortFlags;
        TMEngEnumInfo*  m_pmeciRTSFlags;
        TMEngEnumInfo*  m_pmeciStopBits;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCommCfgInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngCommPortVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCommPortVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCommPortVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngCommPortVal(const TMEngCommPortVal&) = delete;

        ~TMEngCommPortVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCommPortVal& operator=(const TMEngCommPortVal&) = delete;


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
        tCIDLib::TBoolean bIsOpen() const;

        TCommPortBase& commValue();

        const TCommPortBase& commValue() const;

        tCIDLib::TVoid Open
        (
            const   TString&                strPath
        );

        tCIDLib::TVoid Open
        (
            const   TString&                strPath
            , const TCommPortCfg&           ccfgToUse
            , const tCIDLib::TCard4         c4ReadSz
            , const tCIDLib::TCard4         c4WriteSz
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_pcommValue
        //      This is the comm port represnted by this object. It is not
        //      actually opened nor is the port set until the macro level
        //      constructor is called. We use a pointer so that we don't have
        //      to expose the comm headers to our clients.
        // -------------------------------------------------------------------
        TCommPortBase*  m_pcommValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCommPortVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngCommPortInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngCommPortInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngCommPortInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngCommPortInfo(const TMEngCommPortInfo&) = delete;

        ~TMEngCommPortInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngCommPortInfo& operator=(const TMEngCommPortInfo&) = delete;


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
        tCIDLib::TVoid CheckIsOpen
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngCommPortVal&       mecvToCheck
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
            , const MFormattable&           fmtblToken
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            ,       TError&                 errCaught
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        //
        //  m_c2TypeId_XXX
        //      We create a few nested enum types that represent various
        //      aspects of a COM port's configuration.
        //
        //  m_c4ErrId_XXXX
        //      To aid in throwing exceptions, we store the ids of the error
        //      enum values as we create them. This way, we don't have to
        //      do some kind of by name lookup each time.
        //
        //  m_pmeciErrors
        //      We need to keep around a pointer to the error enum that we
        //      create, because we need it when throwing an exception. We
        //      don't own it, we just keep a pointer around.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_AssertCTS;
        tCIDLib::TCard2 m_c2MethId_AssertDSR;
        tCIDLib::TCard2 m_c2MethId_ClearPortFlag;
        tCIDLib::TCard2 m_c2MethId_Close;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_GetIsOpen;
        tCIDLib::TCard2 m_c2MethId_GetPort;
        tCIDLib::TCard2 m_c2MethId_Open;
        tCIDLib::TCard2 m_c2MethId_OpenCfg;
        tCIDLib::TCard2 m_c2MethId_OpenCfgBufs;
        tCIDLib::TCard2 m_c2MethId_GetCTSOn;
        tCIDLib::TCard2 m_c2MethId_GetDSROn;
        tCIDLib::TCard2 m_c2MethId_GetLineStates;
        tCIDLib::TCard2 m_c2MethId_PurgeReadBuf;
        tCIDLib::TCard2 m_c2MethId_PurgeWriteBuf;
        tCIDLib::TCard2 m_c2MethId_ReadBuf;
        tCIDLib::TCard2 m_c2MethId_ReadByte;
        tCIDLib::TCard2 m_c2MethId_SetBreak;
        tCIDLib::TCard2 m_c2MethId_SetCfg;
        tCIDLib::TCard2 m_c2MethId_SetDTRFlag;
        tCIDLib::TCard2 m_c2MethId_SetPortFlag;
        tCIDLib::TCard2 m_c2MethId_SetRTSFlag;
        tCIDLib::TCard2 m_c2MethId_WriteBuf;
        tCIDLib::TCard2 m_c2MethId_WriteByte;
        tCIDLib::TCard2 m_c2TypeId_Errors;
        tCIDLib::TCard4 m_c4ErrId_AlreadyOpen;
        tCIDLib::TCard4 m_c4ErrId_CfgFailed;
        tCIDLib::TCard4 m_c4ErrId_CloseFailed;
        tCIDLib::TCard4 m_c4ErrId_NotOpen;
        tCIDLib::TCard4 m_c4ErrId_OpenFailed;
        tCIDLib::TCard4 m_c4ErrId_PurgeErr;
        tCIDLib::TCard4 m_c4ErrId_QueryLineStateErr;
        tCIDLib::TCard4 m_c4ErrId_ReadErr;
        tCIDLib::TCard4 m_c4ErrId_SetBreakErr;
        tCIDLib::TCard4 m_c4ErrId_SetFlagErr;
        tCIDLib::TCard4 m_c4ErrId_SetLineStateErr;
        tCIDLib::TCard4 m_c4ErrId_WriteErr;
        TMEngEnumInfo*  m_pmeciErrors;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngCommPortInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


