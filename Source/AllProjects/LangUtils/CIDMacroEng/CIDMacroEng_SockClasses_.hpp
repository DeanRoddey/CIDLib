//
// FILE NAME: CIDMacroEng_SockClasses_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/2003
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
//  This is the header file for the CIDMacroEng_SockClass.cpp file, which
//  implements the info and value classes required to create new macro level
//  classes to support IP sockets.
//
//  NOTE:   We have a C++ level inheritance here between the base socket value
//          class and the specific socket value classes. This let's us
//          get correct inheritance of the underlying CIDLib socket without
//          having to make a socket wrapper value object and use a macro
//          level data member to store the actual socket.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TSocket;
class TIPEndPoint;
class TClientStreamSocket;
class TClientDatagramSocket;
class TMEngStreamSocketInfo;
class TMEngDGramSocketInfo;


// ---------------------------------------------------------------------------
//  CLASS: TMEngIPEPVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngIPEPVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngIPEPVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngIPEPVal(const TMEngIPEPVal&) = delete;

        ~TMEngIPEPVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngIPEPVal& operator=(const TMEngIPEPVal&) = delete;


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
        const TIPEndPoint& ipepValue() const;

        const TIPEndPoint& ipepValue
        (
            const   TIPEndPoint&            ipepToSet
        );

        TIPEndPoint& ipepValue();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pipepValue
        //      The IP end point that provides our value. We use a pointer so
        //      that we don't have to expose the underlying socket headers
        //      to our users.
        // -------------------------------------------------------------------
        TIPEndPoint*    m_pipepValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngIPEPVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngIPEPInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngIPEPInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strIPAddrTypesPath();
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngIPEPInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngIPEPInfo(const TMEngIPEPInfo&) = delete;

        ~TMEngIPEPInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngIPEPInfo& operator=(const TMEngIPEPInfo&) = delete;


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
            ,       TError&                 errCaught
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const tCIDLib::TCh* const     pszText
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_FromText;
        tCIDLib::TCard2 m_c2MethId_FromText2;
        tCIDLib::TCard2 m_c2MethId_GetAddrBytes;
        tCIDLib::TCard2 m_c2MethId_GetAddrType;
        tCIDLib::TCard2 m_c2MethId_GetAsText;
        tCIDLib::TCard2 m_c2MethId_GetHostName;
        tCIDLib::TCard2 m_c2MethId_GetOrgAddr;
        tCIDLib::TCard2 m_c2MethId_GetPortNum;
        tCIDLib::TCard2 m_c2MethId_Set;
        tCIDLib::TCard2 m_c2MethId_Set2;
        tCIDLib::TCard2 m_c2MethId_SetPortNum;
        tCIDLib::TCard2 m_c2MethId_TextValCtor;
        tCIDLib::TCard2 m_c2MethId_TextValCtor2;
        tCIDLib::TCard2 m_c2TypeId_Errors;
        tCIDLib::TCard2 m_c2TypeId_IPAddrTypes;
        tCIDLib::TCard4 m_c4ErrId_DeprecatedIPAddr;
        tCIDLib::TCard4 m_c4ErrId_CantConvert;
        tCIDLib::TCard4 m_c4ErrId_GetRemoteName;
        TMEngEnumInfo*  m_pmeciErrors;
        TMEngEnumInfo*  m_pmeciIPAddrTypes;


        // Deprecated methods
        tCIDLib::TCard2 m_c2MethId_BinValCtor;
        tCIDLib::TCard2 m_c2MethId_GetAddr;
        tCIDLib::TCard2 m_c2MethId_GetNetAddr;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngIPEPInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngSocketVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngSocketVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngSocketVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngSocketVal(const TMEngSocketVal&) = delete;

        ~TMEngSocketVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngSocketVal& operator=(const TMEngSocketVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bIsBound() const = 0;

        virtual tCIDLib::TBoolean bIsOpen() const = 0;

        virtual tCIDLib::TVoid Close() = 0;

        virtual tCIDLib::TVoid Connect
        (
            const   tCIDSock::ESockProtos   eProtocol
            , const TIPEndPoint&            ipepRemote
        ) = 0;

        virtual TSocket& sockValue
        (
                    TCIDMacroEngine&        meOwner
        ) = 0;

        virtual const TSocket& sockValue
        (
                    TCIDMacroEngine&        meOwner
        )   const = 0;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngSocketVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngSocketInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngSocketInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();
        static const TString& strSockProtosPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngSocketInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngSocketInfo(const TMEngSocketInfo&) = delete;

        ~TMEngSocketInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngSocketInfo& operator=(const TMEngSocketInfo&) = delete;


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


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const TString&                strClassPath
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const TString&                strClassPath
            , const MFormattable&           fmtblToken
        );

        tCIDLib::TVoid XlatSockErr
        (
                    TCIDMacroEngine&        meOwner
            ,       TError&                 errCaught
            , const TString&                strClassPath
            , const tCIDLib::TCard4         c4ElseErr
        );


    protected :
        // -------------------------------------------------------------------
        //  Our (macro level) derivatives are our friends so that they can
        //  see the error ids.
        // -------------------------------------------------------------------
        friend class TMEngStreamSocketInfo;
        friend class TMEngDGramSocketInfo;


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
        //      aspects of a socket's configuration and our errors.
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
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_GetDataReady;
        tCIDLib::TCard2 m_c2MethId_GetIsBound;
        tCIDLib::TCard2 m_c2MethId_GetIsOpen;
        tCIDLib::TCard2 m_c2MethId_GetLocalEndPoint;
        tCIDLib::TCard2 m_c2MethId_GetStatus;
        tCIDLib::TCard2 m_c2MethId_SetKeepAlive;
        tCIDLib::TCard2 m_c2MethId_SetLinger;
        tCIDLib::TCard2 m_c2MethId_WaitForData;
        tCIDLib::TCard2 m_c2TypeId_Errors;
        tCIDLib::TCard4 m_c4ErrId_AlreadyBound;
        tCIDLib::TCard4 m_c4ErrId_NotBound;
        tCIDLib::TCard4 m_c4ErrId_BindFailed;
        tCIDLib::TCard4 m_c4ErrId_AlreadyConn;
        tCIDLib::TCard4 m_c4ErrId_AlreadyOpen;
        tCIDLib::TCard4 m_c4ErrId_NotConn;
        tCIDLib::TCard4 m_c4ErrId_ConnFailed;
        tCIDLib::TCard4 m_c4ErrId_Unsupported;
        tCIDLib::TCard4 m_c4ErrId_ConnAborted;
        tCIDLib::TCard4 m_c4ErrId_NotOpen;
        tCIDLib::TCard4 m_c4ErrId_Timeout;
        tCIDLib::TCard4 m_c4ErrId_NetworkUnreachable;
        tCIDLib::TCard4 m_c4ErrId_NetworkDown;
        tCIDLib::TCard4 m_c4ErrId_ConnRefused;
        tCIDLib::TCard4 m_c4ErrId_NotConnected;
        tCIDLib::TCard4 m_c4ErrId_DGBufTooSmall;
        tCIDLib::TCard4 m_c4ErrId_HostUnreachable;
        tCIDLib::TCard4 m_c4ErrId_NameTooLong;
        tCIDLib::TCard4 m_c4ErrId_OpenFailed;
        tCIDLib::TCard4 m_c4ErrId_ReadErr;
        tCIDLib::TCard4 m_c4ErrId_SetOptErr;
        tCIDLib::TCard4 m_c4ErrId_ShutdownErr;
        tCIDLib::TCard4 m_c4ErrId_UnknownErr;
        tCIDLib::TCard4 m_c4ErrId_WriteErr;
        TMEngEnumInfo*  m_pmeciErrors;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngSocketInfo,TMEngClassInfo)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngStreamSocketVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngStreamSocketVal : public TMEngSocketVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngStreamSocketVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngStreamSocketVal(const TMEngStreamSocketVal&) = delete;

        ~TMEngStreamSocketVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngStreamSocketVal& operator=(const TMEngStreamSocketVal&) = delete;


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

        tCIDLib::TBoolean bIsBound() const override;

        tCIDLib::TBoolean bIsOpen() const override;

        tCIDLib::TVoid Close() override;

        tCIDLib::TVoid Connect
        (
            const   tCIDSock::ESockProtos   eProtocol
            , const TIPEndPoint&            ipepSrv
        )   override;

        TSocket& sockValue
        (
                    TCIDMacroEngine&        meOwner
        )   override;

        const TSocket& sockValue
        (
                    TCIDMacroEngine&        meOwner
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsConnected() const;

        tCIDLib::TVoid Cleanup();

        TClientStreamSocket& sockStreamValue
        (
                    TCIDMacroEngine&        meOwner
        );

        const TClientStreamSocket& sockStreamValue
        (
                    TCIDMacroEngine&        meOwner
        )   const;

        tCIDLib::TVoid Shutdown();


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_psockValue
        //      This is the socket used by this class.
        // -------------------------------------------------------------------
        TClientStreamSocket*    m_psockValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngStreamSocketVal,TMEngSocketVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngStreamSocketInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngStreamSocketInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngStreamSocketInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngStreamSocketInfo(const TMEngStreamSocketInfo&) = delete;

        ~TMEngStreamSocketInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngStreamSocketInfo& operator=(const TMEngStreamSocketInfo&) = delete;


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
        //  m_pmeciSockInfo
        //      We have to throw macro level exceptions through our base
        //      class, so we look up a pointer to his class info object durin
        //      our init, to avoid having to do it in many places.
        // -------------------------------------------------------------------
        tCIDLib::TCard2     m_c2MethId_Close;
        tCIDLib::TCard2     m_c2MethId_DefCtor;
        tCIDLib::TCard2     m_c2MethId_Connect;
        tCIDLib::TCard2     m_c2MethId_GetIsConnected;
        tCIDLib::TCard2     m_c2MethId_ReadBuffer;
        tCIDLib::TCard2     m_c2MethId_ReadBuffer2;
        tCIDLib::TCard2     m_c2MethId_ReadByte;
        tCIDLib::TCard2     m_c2MethId_ReadByte2;
        tCIDLib::TCard2     m_c2MethId_SetNagle;
        tCIDLib::TCard2     m_c2MethId_Shutdown;
        tCIDLib::TCard2     m_c2MethId_WriteBuffer;
        TMEngSocketInfo*    m_pmeciSockInfo;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngStreamSocketInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngDGramSocketVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngDGramSocketVal : public TMEngSocketVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngDGramSocketVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2ClassId
            , const tCIDMacroEng::EConstTypes eConst
        );

        TMEngDGramSocketVal(const TMEngDGramSocketVal&) = delete;

        ~TMEngDGramSocketVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngDGramSocketVal& operator=(const TMEngDGramSocketVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDbgFormat
        (
                    TTextOutStream&         strmTarget
            , const TMEngClassInfo&         meciThis
            , const tCIDMacroEng::EDbgFmts  eFormat
            , const tCIDLib::ERadices       eRadix
            , const TCIDMacroEngine&        meOwner
        )   const override;

        tCIDLib::TBoolean bIsBound() const override;

        tCIDLib::TBoolean bIsOpen() const override;

        tCIDLib::TVoid Close() override;

        tCIDLib::TVoid Connect
        (
            const   tCIDSock::ESockProtos   eProtocol
            , const TIPEndPoint&            ipepSrv
        )   override;

        TSocket& sockValue
        (
                    TCIDMacroEngine&        meOwner
        )   override;

        const TSocket& sockValue
        (
                    TCIDMacroEngine&        meOwner
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BindForRemote
        (
            const   tCIDSock::ESockProtos   eProtocol
            , const TIPEndPoint&            ipepTar
            , const tCIDLib::TIPPortNum     ippnLocal
        );

        tCIDLib::TVoid BindLocal
        (
            const   tCIDSock::ESockProtos   eProtocol
            , const TIPEndPoint&            ipepLocal
        );

        tCIDLib::TVoid BindListen
        (
            const   tCIDSock::ESockProtos   eProtocol
            , const tCIDSock::EAddrTypes    eAddrType
            , const tCIDLib::TIPPortNum     ippnListen
        );

        tCIDLib::TVoid Open
        (
            const   tCIDSock::ESockProtos   eProtocol
            , const tCIDSock::EAddrTypes    eAddrType
        );

        TClientDatagramSocket& sockDGramValue
        (
                    TCIDMacroEngine&        meOwner
        );

        const TClientDatagramSocket& sockDGramValue
        (
                    TCIDMacroEngine&        meOwner
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Cleanup();


        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_psockValue
        //      This is the client side datagram socket used by this class.
        // -------------------------------------------------------------------
        TClientDatagramSocket*  m_psockValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngDGramSocketVal,TMEngSocketVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngDGramSocketInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngDGramSocketInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngDGramSocketInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngDGramSocketInfo(const TMEngDGramSocketInfo&) = delete;

        ~TMEngDGramSocketInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngDGramSocketInfo& operator=(const TMEngDGramSocketInfo&) = delete;


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
        //  m_pmeciSockInfo
        //      We have to throw macro level exceptions through our base
        //      class, so we look up a pointer to his class info object durin
        //      our init, to avoid having to do it in many places.
        // -------------------------------------------------------------------
        tCIDLib::TCard2     m_c2MethId_Close;
        tCIDLib::TCard2     m_c2MethId_DefCtor;
        tCIDLib::TCard2     m_c2MethId_BindListen;
        tCIDLib::TCard2     m_c2MethId_BindLocal;
        tCIDLib::TCard2     m_c2MethId_BindForRemote;
        tCIDLib::TCard2     m_c2MethId_Open;
        tCIDLib::TCard2     m_c2MethId_ReadFrom;
        tCIDLib::TCard2     m_c2MethId_SendTo;
        tCIDLib::TCard2     m_c2MethId_SetAllowBroadcasts;
        tCIDLib::TCard2     m_c2MethId_SendWOL;
        TMEngSocketInfo*    m_pmeciSockInfo;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngDGramSocketInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngSockPingerVal
// PREFIX: mecv
//
//  NOTE:   We play some trick shere. These values really wouldn't be copyable,
//          but we need to allow the creation of collections of them for
//          practical purposes in the CQC layer. So the CopyFrom class doesn't
//          actually copy anything.
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngSockPingerVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngSockPingerVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngSockPingerVal(const TMEngSockPingerVal&) = delete;

        ~TMEngSockPingerVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngSockPingerVal& operator=(const TMEngSockPingerVal&) = delete;


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
        TSockPinger& spingValue();

        const TSockPinger& spingValue() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_spingValue
        //      The socket pinger object
        // -------------------------------------------------------------------
        TSockPinger m_spingValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngSockPingerVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngSockPingerInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngSockPingerInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngSockPingerInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngSockPingerInfo(const TMEngSockPingerInfo&) = delete;

        ~TMEngSockPingerInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngSockPingerInfo& operator=(const TMEngSockPingerInfo&) = delete;


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
            ,       TError&                 errCaught
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const tCIDLib::TCh* const     pszText
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_CancelWait;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_EndPing;
        tCIDLib::TCard2 m_c2MethId_GetLastTime;
        tCIDLib::TCard2 m_c2MethId_GetRemAddr;
        tCIDLib::TCard2 m_c2MethId_GetState;
        tCIDLib::TCard2 m_c2MethId_SendRequest;
        tCIDLib::TCard2 m_c2MethId_StartPing;
        tCIDLib::TCard2 m_c2MethId_WaitReply;
        tCIDLib::TCard4 m_c4ErrId_CancelFailed;
        tCIDLib::TCard4 m_c4ErrId_EndFailed;
        tCIDLib::TCard4 m_c4ErrId_SendFailed;
        tCIDLib::TCard4 m_c4ErrId_StartFailed;
        tCIDLib::TCard4 m_c4ErrId_WaitFailed;
        tCIDLib::TCard2 m_c2TypeId_Errors;
        tCIDLib::TCard2 m_c2TypeId_States;
        TMEngEnumInfo*  m_pmeciErrors;
        TMEngEnumInfo*  m_pmeciStates;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngSockPingerInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


