//
// FILE NAME: CIDMacroEng_NetClasses_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/17/2005
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
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
//  This is the header file for the CIDMacroEng_NetClasses.cpp file, which
//  implements the info and value classes required to create new macro level
//  classes to wrap the C++ HTTPClient and URL classes.
//
//  - URL parsing and manipulation
//  - HTTP client support
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

class TURL;
class THTTPClient;


// ---------------------------------------------------------------------------
//  CLASS: TMEngDataSrcVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngDataSrcVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngDataSrcVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngDataSrcVal(const TMEngDataSrcVal&) = delete;

        ~TMEngDataSrcVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngDataSrcVal& operator=(const TMEngDataSrcVal&) = delete;


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

        tCIDLib::TBoolean bReady() const;

        tCIDLib::TVoid Close();

        TCIDDataSrc& cdsValue();

        const TCIDDataSrc& cdsValue() const;

        tCIDLib::TVoid SetSockTCP
        (
            const   TIPEndPoint&            ipepTar
            , const tCIDLib::TBoolean       bSecure
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pcdsSrc
        //      A pointer to some sort of data source derivative. Fow now, we are
        //      really only dealing with socket based sources for HTTP, but later
        //      others. Code that uses these need to validate that the actual type
        //      is valid, if that's relevant.
        // -------------------------------------------------------------------
        TCIDDataSrc*    m_pcdsSrc;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngDataSrcVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngDataSrcInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngDataSrcInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngDataSrcInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngDataSrcInfo(const TMEngDataSrcInfo&) = delete;

        ~TMEngDataSrcInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngDataSrcInfo& operator=(const TMEngDataSrcInfo&) = delete;


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
        tCIDLib::TVoid CheckIsReady
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngDataSrcVal&         mecvCheck
        )   const;


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
        )   const;

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
        )   const;


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
        tCIDLib::TCard2         m_c2MethId_Close;
        tCIDLib::TCard2         m_c2MethId_DataIsReady;
        tCIDLib::TCard2         m_c2MethId_DefCtor;
        tCIDLib::TCard2         m_c2MethId_GetIsConnected;
        tCIDLib::TCard2         m_c2MethId_GetLine;
        tCIDLib::TCard2         m_c2MethId_ReadBytes;
        tCIDLib::TCard2         m_c2MethId_TCPSetup;
        tCIDLib::TCard4         m_c4ErrId_Already;
        tCIDLib::TCard4         m_c4ErrId_Close;
        tCIDLib::TCard4         m_c4ErrId_NotReady;
        tCIDLib::TCard4         m_c4ErrId_Read;
        tCIDLib::TCard4         m_c4ErrId_Setup;
        tCIDLib::TCard2         m_c2TypeId_IPEndPoint;
        tCIDLib::TCard2         m_c2TypeId_TextXCoder;
        TMEngEnumInfo*          m_pmeciErrors;
        TMEngEnumInfo*          m_pmeciLineRes;
        const TMEngEnumInfo*    m_pmeciSockProtos;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngDataSrcInfo,TMEngClassInfo)
};




// ---------------------------------------------------------------------------
//  CLASS: TMEngHTTPClientVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngHTTPClientVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngHTTPClientVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngHTTPClientVal(const TMEngHTTPClientVal&) = delete;

        ~TMEngHTTPClientVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngHTTPClientVal& operator=(const TMEngHTTPClientVal&) = delete;


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
        THTTPClient& httpcValue();

        tCIDLib::TVoid Reset();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_phttpcValue
        //      The HTTP client object that provides our value. We use a pointer
        //      so that we don't have to expose the underlying Net headers
        //      to our users.
        // -------------------------------------------------------------------
        THTTPClient*    m_phttpcValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngHTTPClientVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngHTTPClientInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngHTTPClientInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngHTTPClientInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngHTTPClientInfo(const TMEngHTTPClientInfo&) = delete;

        ~TMEngHTTPClientInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngHTTPClientInfo& operator=(const TMEngHTTPClientInfo&) = delete;


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
        tCIDLib::TCard2         m_c2MethId_CreateBasicAuthStr;
        tCIDLib::TCard2         m_c2MethId_DefCtor;
        tCIDLib::TCard2         m_c2MethId_DoSSLGET;
        tCIDLib::TCard2         m_c2MethId_DoSSLPOST;
        tCIDLib::TCard2         m_c2MethId_EscapeBodyText;
        tCIDLib::TCard2         m_c2MethId_ExpandBodyText;
        tCIDLib::TCard2         m_c2MethId_FindHdrLine;
        tCIDLib::TCard2         m_c2MethId_FindTextEncoding;
        tCIDLib::TCard2         m_c2MethId_GetClientMsg;
        tCIDLib::TCard2         m_c2MethId_GetSrvReply;
        tCIDLib::TCard2         m_c2MethId_ParseAuthReq;
        tCIDLib::TCard2         m_c2MethId_ParseMPartMIME;
        tCIDLib::TCard2         m_c2MethId_ParseQParms;
        tCIDLib::TCard2         m_c2MethId_ParseTextEncoding;
        tCIDLib::TCard2         m_c2MethId_SendGET;
        tCIDLib::TCard2         m_c2MethId_SendHEAD;
        tCIDLib::TCard2         m_c2MethId_SendPOST;
        tCIDLib::TCard2         m_c2MethId_SendPOST2;
        tCIDLib::TCard2         m_c2MethId_SendPUT;
        tCIDLib::TCard2         m_c2MethId_SendRUGET;
        tCIDLib::TCard2         m_c2MethId_SendRUHEAD;
        tCIDLib::TCard2         m_c2MethId_SendRUPOST;
        tCIDLib::TCard2         m_c2MethId_SendRUPOST2;
        tCIDLib::TCard2         m_c2MethId_SendRUPUT;
        tCIDLib::TCard2         m_c2MethId_SetAddrType;
        tCIDLib::TCard2         m_c2MethId_SetAutoAuth;
        tCIDLib::TCard2         m_c2MethId_SetAuthInfo;
        tCIDLib::TCard2         m_c2MethId_SetProxy;
        tCIDLib::TCard2         m_c2MethId_Reset;
        tCIDLib::TCard2         m_c2TypeId_KVPair;
        tCIDLib::TCard2         m_c2TypeId_CardList;
        tCIDLib::TCard2         m_c2TypeId_BufList;
        tCIDLib::TCard2         m_c2TypeId_ItemList;
        tCIDLib::TCard2         m_c2TypeId_StrList;
        tCIDLib::TCard4         m_c4ErrId_Get;
        tCIDLib::TCard4         m_c4ErrId_Escape;
        tCIDLib::TCard4         m_c4ErrId_Expand;
        tCIDLib::TCard4         m_c4ErrId_Parse;
        tCIDLib::TCard4         m_c4ErrId_Post;
        tCIDLib::TCard4         m_c4ErrId_RecvMsg;
        tCIDLib::TCard4         m_c4ErrId_SetProxy;
        TMEngEnumInfo*          m_pmeciAuthTypes;
        TMEngEnumInfo*          m_pmeciErrors;
        const TMEngEnumInfo*    m_pmeciIPAddrTypes;
        const TMEngDataSrcInfo* m_pmeciNetSrc;
        TMEngEnumInfo*          m_pmeciReadRes;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngHTTPClientInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngURLVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngURLVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngURLVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngURLVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
            , const TURL&                   urlInit
        );

        TMEngURLVal(const TMEngURLVal&) = delete;

        ~TMEngURLVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngURLVal& operator=(const TMEngURLVal&) = delete;


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
        TURL& urlValue();

        const TURL& urlValue() const;

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid Set
        (
            const   TURL&                   urlToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_purlValue
        //      The URL object that provides our value. We use a pointer
        //      so that we don't have to expose the underlying C++ net headers
        //      to our users.
        // -------------------------------------------------------------------
        TURL*   m_purlValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngURLVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngURLInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngURLInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString& strPath();
        static const TString& strProtosPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngURLInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngURLInfo(const TMEngURLInfo&) = delete;

        ~TMEngURLInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngURLInfo& operator=(const TMEngURLInfo&) = delete;


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


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_BuildURL;
        tCIDLib::TCard2 m_c2MethId_BuildURL2;
        tCIDLib::TCard2 m_c2MethId_Clear;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Encode;
        tCIDLib::TCard2 m_c2MethId_Equal;
        tCIDLib::TCard2 m_c2MethId_Expand;
        tCIDLib::TCard2 m_c2MethId_GetEncoded;
        tCIDLib::TCard2 m_c2MethId_GetEncodedParts;
        tCIDLib::TCard2 m_c2MethId_GetFragment;
        tCIDLib::TCard2 m_c2MethId_GetFullText;
        tCIDLib::TCard2 m_c2MethId_GetHost;
        tCIDLib::TCard2 m_c2MethId_GetIPEndPoint;
        tCIDLib::TCard2 m_c2MethId_GetParams;
        tCIDLib::TCard2 m_c2MethId_GetPassword;
        tCIDLib::TCard2 m_c2MethId_GetPath;
        tCIDLib::TCard2 m_c2MethId_GetPort;
        tCIDLib::TCard2 m_c2MethId_GetProto;
        tCIDLib::TCard2 m_c2MethId_GetQParms;
        tCIDLib::TCard2 m_c2MethId_GetQParmsFmt;
        tCIDLib::TCard2 m_c2MethId_GetResource;
        tCIDLib::TCard2 m_c2MethId_GetUserName;
        tCIDLib::TCard2 m_c2MethId_Set;
        tCIDLib::TCard2 m_c2MethId_Set2;
        tCIDLib::TCard2 m_c2MethId_Set3;
        tCIDLib::TCard2 m_c2MethId_Set4;
        tCIDLib::TCard2 m_c2TypeId_Comps;
        tCIDLib::TCard2 m_c2TypeId_Errors;
        tCIDLib::TCard2 m_c2TypeId_ItemList;
        tCIDLib::TCard2 m_c2TypeId_IPEP;
        tCIDLib::TCard2 m_c2TypeId_KVPair;
        tCIDLib::TCard2 m_c2TypeId_Protos;
        tCIDLib::TCard4 m_c4ErrId_Encoding;
        tCIDLib::TCard4 m_c4ErrId_NoIPEP;
        tCIDLib::TCard4 m_c4ErrId_Query;
        tCIDLib::TCard4 m_c4ErrId_Set;
        TMEngEnumInfo*  m_pmeciIPAddrTypes;
        TMEngEnumInfo*  m_pmeciComps;
        TMEngEnumInfo*  m_pmeciErrors;
        TMEngEnumInfo*  m_pmeciProtos;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngURLInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK

