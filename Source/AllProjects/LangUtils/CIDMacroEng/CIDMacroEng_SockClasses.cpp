//
// FILE NAME: CIDMacroEng_SockClasses.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/2003
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
//  This file implements a set of derivatives of the class info and class
//  value classes, which implement the macro level socket classes and some
//  auxilary helper classes.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDMacroEng_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMEngIPEPVal,TMEngClassVal)
RTTIDecls(TMEngIPEPInfo,TMEngClassInfo)
RTTIDecls(TMEngSocketVal,TMEngClassVal)
RTTIDecls(TMEngSocketInfo,TMEngClassInfo)
RTTIDecls(TMEngStreamSocketVal,TMEngSocketVal)
RTTIDecls(TMEngStreamSocketInfo,TMEngClassInfo)
RTTIDecls(TMEngDGramSocketVal,TMEngSocketVal)
RTTIDecls(TMEngDGramSocketInfo,TMEngClassInfo)
RTTIDecls(TMEngSockPingerVal,TMEngClassVal)
RTTIDecls(TMEngSockPingerInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_SockClasses
{
    // -----------------------------------------------------------------------
    //  The names for the types that we support here. Each derivative has to
    //  be able to return strings that contain its name and full name.
    // -----------------------------------------------------------------------
    const TString   strIPEP(L"IPEndPoint");
    const TString   strIPEPClassPath(L"MEng.System.Runtime.IPEndPoint");
    const TString   strIPAddrTypesPath(L"MEng.System.Runtime.IPEndPoint.IPAddrTypes");

    const TString   strSocket(L"Socket");
    const TString   strSocketClassPath(L"MEng.System.Runtime.Socket");
    const TString   strProtoEnumClassPath(L"MEng.System.Runtime.Socket.SockProtos");

    const TString   strStreamSocket(L"StreamSocket");
    const TString   strStreamSocketClassPath(L"MEng.System.Runtime.StreamSocket");

    const TString   strDGramSocket(L"DGramSocket");
    const TString   strDGramSocketClassPath(L"MEng.System.Runtime.DGramSocket");

    const TString   strSockPinger(L"SockPinger");
    const TString   strSockPingerClassPath(L"MEng.System.Runtime.SockPinger");
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngIPEPVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngIPEPVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngIPEPVal::TMEngIPEPVal( const   TString&                strName
                            , const tCIDLib::TCard2         c2Id
                            , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_pipepValue(new TIPEndPoint)
{
}

TMEngIPEPVal::~TMEngIPEPVal()
{
    delete m_pipepValue;
}


// ---------------------------------------------------------------------------
//  TMEngIPEPVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngIPEPVal::bDbgFormat(       TTextOutStream&     strmTarget
                        , const TMEngClassInfo&     meciThis
                        , const tCIDMacroEng::EDbgFmts eFormat
                        , const tCIDLib::ERadices   eRadix
                        , const TCIDMacroEngine&    meOwner) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        strmTarget << L"PortNum: ";
        if (eRadix == tCIDLib::ERadices::Hex)
            strmTarget << L"0x";
        strmTarget  << TCardinal(m_pipepValue->ippnThis(), eRadix) << kCIDLib::NewLn
                    << L"Address: " << m_pipepValue->strAsText() << kCIDLib::EndLn;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TVoid
TMEngIPEPVal::CopyFrom(const   TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    *m_pipepValue = *static_cast<const TMEngIPEPVal&>(mecvToCopy).m_pipepValue;
}


// ---------------------------------------------------------------------------
//  TMEngIPEPVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TIPEndPoint& TMEngIPEPVal::ipepValue() const
{
    return *m_pipepValue;
}

const TIPEndPoint& TMEngIPEPVal::ipepValue(const TIPEndPoint& ipepToSet)
{
    *m_pipepValue = ipepToSet;
    return *m_pipepValue;
}

TIPEndPoint& TMEngIPEPVal::ipepValue()
{
    return *m_pipepValue;
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngIPEPInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngIPEPInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngIPEPInfo::strIPAddrTypesPath()
{
    return CIDMacroEng_SockClasses::strIPAddrTypesPath;
}

const TString& TMEngIPEPInfo::strPath()
{
    return CIDMacroEng_SockClasses::strIPEPClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngIPEPInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngIPEPInfo::TMEngIPEPInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_SockClasses::strIPEP
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_FromText(kMacroEng::c2BadId)
    , m_c2MethId_FromText2(kMacroEng::c2BadId)
    , m_c2MethId_GetAddrBytes(kMacroEng::c2BadId)
    , m_c2MethId_GetAddrType(kMacroEng::c2BadId)
    , m_c2MethId_GetAsText(kMacroEng::c2BadId)
    , m_c2MethId_GetHostName(kMacroEng::c2BadId)
    , m_c2MethId_GetOrgAddr(kMacroEng::c2BadId)
    , m_c2MethId_GetPortNum(kMacroEng::c2BadId)
    , m_c2MethId_SetPortNum(kMacroEng::c2BadId)
    , m_c2MethId_Set(kMacroEng::c2BadId)
    , m_c2MethId_Set2(kMacroEng::c2BadId)
    , m_c2MethId_TextValCtor(kMacroEng::c2BadId)
    , m_c2MethId_TextValCtor2(kMacroEng::c2BadId)
    , m_c2TypeId_Errors(kMacroEng::c2BadId)
    , m_c2TypeId_IPAddrTypes(kMacroEng::c2BadId)
    , m_pmeciErrors(0)
    , m_pmeciIPAddrTypes(0)

    // Deprecated
    , m_c2MethId_BinValCtor(kMacroEng::c2BadId)
    , m_c2MethId_GetAddr(kMacroEng::c2BadId)
    , m_c2MethId_GetNetAddr(kMacroEng::c2BadId)
{
}

TMEngIPEPInfo::~TMEngIPEPInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngIPEPInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngIPEPInfo::Init(TCIDMacroEngine& meOwner)
{
    // An enum to represent the IP address family
    {
        m_pmeciIPAddrTypes = new TMEngEnumInfo
        (
            meOwner
            , L"IPAddrTypes"
            , strClassPath()
            , L"MEng.Enum"
            , 3
        );
        m_pmeciIPAddrTypes->c4AddEnumItem(L"Unspec", L"Unspecified", tCIDSock::EAddrTypes::Unspec);
        m_pmeciIPAddrTypes->c4AddEnumItem(L"IPV4", L"IPV4", tCIDSock::EAddrTypes::IPV4);
        m_pmeciIPAddrTypes->c4AddEnumItem(L"IPV6", L"IPV6", tCIDSock::EAddrTypes::IPV6);
        m_pmeciIPAddrTypes->BaseClassInit(meOwner);
        m_c2TypeId_IPAddrTypes = meOwner.c2AddClass(m_pmeciIPAddrTypes);
        bAddNestedType(m_pmeciIPAddrTypes->strClassPath());
    }

    //
    //  An enum to represent the exceptions that this class throws. Note
    //  that we just use the text in the C++ exception in some cases, so there
    //  is no text for the enum value for those.
    //
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"IPEndPointErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 3
        );
        m_c4ErrId_CantConvert = m_pmeciErrors->c4AddEnumItem(L"CantConvert", L"");
        m_c4ErrId_GetRemoteName = m_pmeciErrors->c4AddEnumItem(L"GetRemoteName", L"");
        m_c4ErrId_DeprecatedIPAddr = m_pmeciErrors->c4AddEnumItem
        (
            L"DeprecatedIPAddr"
            , L"This IP address method is deprecated. It cannot handle IPV6 addresses."
        );
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2TypeId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }


    //
    //  Value constructor that takes binary address. This is now deprecated
    //  but we'll keep it around. It will create an IPV4 address.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.IPEndPoint"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"BinAddr", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"PortNum", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_BinValCtor = c2AddMethodInfo(methiNew);
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.System.Runtime.IPEndPoint"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Set from text value
    {
        TMEngMethodInfo methiNew
        (
            L"FromText"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToParse", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_FromText = c2AddMethodInfo(methiNew);
    }

    // Set from text value and require a paritcular type of address family
    {
        TMEngMethodInfo methiNew
        (
            L"FromText2"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToParse", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"AddrType", m_c2TypeId_IPAddrTypes);
        m_c2MethId_FromText2 = c2AddMethodInfo(methiNew);
    }

    //
    // Get the address value, in native and network endianness
    //
    //  NOTE:   These are deprecated due to IPV6 compatibility, but are still
    //          present for backwards comptibility. Use GetAddrBytes() instead.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"GetAddr"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetAddr = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetNetAddr"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetNetAddr = c2AddMethodInfo(methiNew);
    }

    //
    //  Get the raw bytes for the IP address. We return the count of bytes
    //  in the address, the family type, and the raw network order bytes in
    //  a provided memory buffer.
    //
    //  This is the correct method to use, since it is IPV4/6 agnostic.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"GetAddrBytes"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"AddrType", m_c2TypeId_IPAddrTypes);
        methiNew.c2AddOutParm(L"ByteData", tCIDMacroEng::EIntrinsics::MemBuf);
        m_c2MethId_GetAddrBytes = c2AddMethodInfo(methiNew);
    }

    // Get the address family
    {
        TMEngMethodInfo methiNew
        (
            L"GetAddrType"
            , m_c2TypeId_IPAddrTypes
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetAddrType = c2AddMethodInfo(methiNew);
    }


    // Get the end point formatted to text
    {
        TMEngMethodInfo methiNew
        (
            L"GetAsText"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetAsText = c2AddMethodInfo(methiNew);
    }

    //
    //  Get the host name, if possible. This may do reverse lookup if
    //  we were provided with a numeric address.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"GetHostName"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetHostName = c2AddMethodInfo(methiNew);
    }

    //
    //  GetHostName() above will do reverse lookup, which is often not what
    //  you want. And GetAsText() includes the port which we may not want
    //  sometimes. This will return the original address info, as it was
    //  provided (numeric or text), with no port.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"GetOrgAddr"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetOrgAddr = c2AddMethodInfo(methiNew);
    }

    // Get the port number
    {
        TMEngMethodInfo methiNew
        (
            L"GetPort"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetPortNum = c2AddMethodInfo(methiNew);
    }

    // Set the text address and port
    {
        TMEngMethodInfo methiNew
        (
            L"Set"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"TextAddr", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"PortNum", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_Set = c2AddMethodInfo(methiNew);
    }

    // Set the text address and port, with a required address type
    {
        TMEngMethodInfo methiNew
        (
            L"Set2"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"TextAddr", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"PortNum", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"AddrType", m_c2TypeId_IPAddrTypes);
        m_c2MethId_Set2 = c2AddMethodInfo(methiNew);
    }

    // Set the port number
    {
        TMEngMethodInfo methiNew
        (
            L"SetPort"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"PortNum", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SetPortNum = c2AddMethodInfo(methiNew);
    }

    // Value constructor that takes text address
    {
        TMEngMethodInfo methiNew
        (
            L"ctor3_MEng.System.Runtime.IPEndPoint"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"TextAddr", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"PortNum", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_TextValCtor = c2AddMethodInfo(methiNew);
    }

    // Value constructor that takes text address and required adddress type
    {
        TMEngMethodInfo methiNew
        (
            L"ctor4_MEng.System.Runtime.IPEndPoint"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"TextAddr", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"PortNum", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"AddrType", m_c2TypeId_IPAddrTypes);
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_TextValCtor2 = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngIPEPInfo::pmecvMakeStorage(const   TString&                strName
                                ,       TCIDMacroEngine&        meOwner
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngIPEPVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngIPEPInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngIPEPInfo::bInvokeMethod(       TCIDMacroEngine&    meOwner
                            , const TMEngMethodInfo&    methiTarget
                            ,       TMEngClassVal&      mecvInstance)
{
    TMEngIPEPVal& mecvActual = static_cast<TMEngIPEPVal&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_BinValCtor)
    {
        //
        //  This is now deprecated and only kept around for backwards
        //  compatibility. We'll create an IPV4 address in this case. We
        //  have to flip the address bytes since it will expect network
        //  order.
        //
        TKrnlIPAddr kipaTmp;
        tCIDLib::TCard4 c4NOAddr = TRawBits::c4SwapBytes(meOwner.c4StackValAt(c4FirstInd));

        if (!kipaTmp.bSet(&c4NOAddr, 4, tCIDSock::EAddrTypes::IPV4, 0, 0))
            ThrowAnErr(meOwner, m_c4ErrId_CantConvert, L"Could not convert to V4 IP address");

        mecvActual.ipepValue(TIPEndPoint(kipaTmp, meOwner.c4StackValAt(c4FirstInd + 1)));
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Nothing to do
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner) << mecvActual.ipepValue();
    }
     else if ((c2MethId == m_c2MethId_FromText)
          ||  (c2MethId == m_c2MethId_FromText2))
    {
        try
        {
            // Use IPV4 if unspecified, for backwards compability
            tCIDSock::EAddrTypes eType = tCIDSock::EAddrTypes::IPV4;

            if (c2MethId == m_c2MethId_FromText2)
            {
                TMEngEnumVal& mecvFamily = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1);
                eType = tCIDSock::EAddrTypes(m_pmeciIPAddrTypes->c4MapValue(mecvFamily));
            }

            TIPEndPoint ipepNew(meOwner.strStackValAt(c4FirstInd), eType);
            mecvActual.ipepValue(ipepNew);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_CantConvert, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetAddr)
    {
        //
        //  Deprecated. If we get an IPV6 address, this won't work, so we
        //  have to throw.
        //
        const TKrnlIPAddr& kipaSrc = mecvActual.ipepValue().kipaThis();
        if (kipaSrc.eType() != tCIDSock::EAddrTypes::IPV4)
            ThrowAnErr(meOwner, m_c4ErrId_DeprecatedIPAddr);

        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);

        // We hvae to swap byte order in this case
        mecvRet.c4Value
        (
            TRawBits::c4SwapBytes(*reinterpret_cast<const tCIDLib::TIPPortNum*>(kipaSrc.pc1Data()))
        );
    }
     else if (c2MethId == m_c2MethId_GetAddrBytes)
    {
        const TKrnlIPAddr& kipaSrc = mecvActual.ipepValue().kipaThis();
        const tCIDLib::TCard4 c4ByteCnt = kipaSrc.c4Count();

        // The return value is the byte count
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(c4ByteCnt);

        // Set the address family
        TMEngEnumVal& mecvFamily = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        mecvFamily.c4Ordinal(m_pmeciIPAddrTypes->c4FromMapValue(kipaSrc.eType()));

        // And fill in the bytes into the buffer
        if (c4ByteCnt)
        {
            TMEngMemBufVal& mecvToFill = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 1);
            mecvToFill.mbufValue().CopyIn(kipaSrc.pc1Data(), c4ByteCnt);
        }
    }
     else if (c2MethId == m_c2MethId_GetAddrType)
    {
        TMEngEnumVal& mecvFamily = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
        mecvFamily.c4Ordinal
        (
            m_pmeciIPAddrTypes->c4FromMapValue(mecvActual.ipepValue().kipaThis().eType())
        );
    }
     else if (c2MethId == m_c2MethId_GetNetAddr)
    {
        //
        //  Deprecated. If we get an IPV6 address, this won't work, so we
        //  have to throw.
        //
        const TKrnlIPAddr& kipaSrc = mecvActual.ipepValue().kipaThis();
        if (kipaSrc.eType() != tCIDSock::EAddrTypes::IPV4)
            ThrowAnErr(meOwner, m_c4ErrId_DeprecatedIPAddr);

        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value
        (
            *reinterpret_cast<const tCIDLib::TIPPortNum*>(kipaSrc.pc1Data())
        );
    }
     else if (c2MethId == m_c2MethId_GetAsText)
    {
        TMEngStringVal& mecvStr
                    = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        mecvStr.strValue(mecvActual.ipepValue().strAsText());

        if (mecvActual.ipepValue().eAddrType() == tCIDSock::EAddrTypes::IPV6)
        {
            mecvStr.strValue().Prepend(L"[");
            mecvStr.strValue().Append(L"]");
        }

        mecvStr.strValue().Append(kCIDLib::chColon);
        mecvStr.strValue().AppendFormatted(mecvActual.ipepValue().ippnThis());
    }
     else if (c2MethId == m_c2MethId_GetHostName)
    {
        // This could throw sinceit might do a reverse lookup
        try
        {
            TMEngStringVal& mecvStr
                        = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
            mecvStr.strValue(mecvActual.ipepValue().strHostName());
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_GetRemoteName, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetOrgAddr)
    {
        TMEngStringVal& mecvStr
                    = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        mecvStr.strValue(mecvActual.ipepValue().strAsText());
    }
     else if (c2MethId == m_c2MethId_GetPortNum)
    {
        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.ipepValue().ippnThis());
    }
     else if (c2MethId == m_c2MethId_SetPortNum)
    {
        mecvActual.ipepValue().ippnThis(meOwner.c4StackValAt(c4FirstInd));
    }
     else if ((c2MethId == m_c2MethId_Set) || (c2MethId == m_c2MethId_Set2))
    {
        try
        {
            // Use IPV4 if unspecified, for backwards compatibility
            tCIDSock::EAddrTypes eType = tCIDSock::EAddrTypes::IPV4;
            if (c2MethId == m_c2MethId_Set2)
            {
                TMEngEnumVal& mecvFamily = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 2);
                eType = tCIDSock::EAddrTypes(m_pmeciIPAddrTypes->c4MapValue(mecvFamily));
            }

            mecvActual.ipepValue
            (
                TIPEndPoint
                (
                    meOwner.strStackValAt(c4FirstInd)
                    , meOwner.c4StackValAt(c4FirstInd + 1)
                    , eType
                )
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_CantConvert, errToCatch);
        }
    }
     else if ((c2MethId == m_c2MethId_TextValCtor)
          ||  (c2MethId == m_c2MethId_TextValCtor2))
    {
        try
        {
            // Use IPV4 if unspecified, for backwards compatibility
            tCIDSock::EAddrTypes eType = tCIDSock::EAddrTypes::IPV4;
            if (c2MethId == m_c2MethId_TextValCtor2)
            {
                TMEngEnumVal& mecvFamily = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 2);
                eType = tCIDSock::EAddrTypes(m_pmeciIPAddrTypes->c4MapValue(mecvFamily));
            }

            TIPEndPoint ipepNew
            (
                meOwner.strStackValAt(c4FirstInd)
                , meOwner.c4StackValAt(c4FirstInd + 1)
                , eType
            );
            mecvActual.ipepValue(ipepNew);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_CantConvert, errToCatch);
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngIPEPInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngIPEPInfo::ThrowAnErr(          TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            ,       TError&             errCaught)
{
    // If verbose log mode, we'll log the C++ level exception
    if (!errCaught.bLogged() && facCIDMacroEng().bLogWarnings())
    {
        errCaught.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errCaught);
    }

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , errCaught.strErrText()
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngIPEPInfo::ThrowAnErr(          TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow)
{
    TString strText(m_pmeciErrors->strTextValue(c4ToThrow));

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngIPEPInfo::ThrowAnErr(          TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            , const tCIDLib::TCh* const pszText)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , pszText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngSocketVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngSocketVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngSocketVal::
TMEngSocketVal( const   TString&                strName
                , const tCIDLib::TCard2         c2ClassId
                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2ClassId, eConst)
{
}

TMEngSocketVal::~TMEngSocketVal()
{
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngSocketInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngSocketInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngSocketInfo::strPath()
{
    return CIDMacroEng_SockClasses::strSocketClassPath;
}

const TString& TMEngSocketInfo::strSockProtosPath()
{
    return CIDMacroEng_SockClasses::strProtoEnumClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngSocketInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngSocketInfo::TMEngSocketInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_SockClasses::strSocket
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Abstract
        , L"MEng.Object"
    )
    , m_c2TypeId_Errors(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_GetDataReady(kMacroEng::c2BadId)
    , m_c2MethId_GetLocalEndPoint(kMacroEng::c2BadId)
    , m_c2MethId_GetIsBound(kMacroEng::c2BadId)
    , m_c2MethId_GetIsOpen(kMacroEng::c2BadId)
    , m_c2MethId_GetStatus(kMacroEng::c2BadId)
    , m_c2MethId_SetKeepAlive(kMacroEng::c2BadId)
    , m_c2MethId_SetLinger(kMacroEng::c2BadId)
    , m_c2MethId_WaitForData(kMacroEng::c2BadId)
    , m_c4ErrId_AlreadyBound(kMacroEng::c2BadId)
    , m_c4ErrId_NotBound(kMacroEng::c2BadId)
    , m_c4ErrId_BindFailed(kMacroEng::c2BadId)
    , m_c4ErrId_AlreadyConn(kMacroEng::c2BadId)
    , m_c4ErrId_AlreadyOpen(kMacroEng::c2BadId)
    , m_c4ErrId_NotConn(kMacroEng::c2BadId)
    , m_c4ErrId_ConnFailed(kMacroEng::c2BadId)
    , m_c4ErrId_Unsupported(kMacroEng::c2BadId)
    , m_c4ErrId_ConnAborted(kMacroEng::c2BadId)
    , m_c4ErrId_Timeout(kMacroEng::c2BadId)
    , m_c4ErrId_NetworkUnreachable(kMacroEng::c2BadId)
    , m_c4ErrId_NetworkDown(kMacroEng::c2BadId)
    , m_c4ErrId_NotOpen(kMacroEng::c2BadId)
    , m_c4ErrId_ConnRefused(kMacroEng::c2BadId)
    , m_c4ErrId_DGBufTooSmall(kMacroEng::c2BadId)
    , m_c4ErrId_HostUnreachable(kMacroEng::c2BadId)
    , m_c4ErrId_NameTooLong(kMacroEng::c2BadId)
    , m_c4ErrId_ReadErr(kMacroEng::c2BadId)
    , m_c4ErrId_SetOptErr(kMacroEng::c2BadId)
    , m_c4ErrId_ShutdownErr(kMacroEng::c2BadId)
    , m_c4ErrId_UnknownErr(kMacroEng::c2BadId)
    , m_c4ErrId_WriteErr(kMacroEng::c2BadId)
    , m_pmeciErrors(0)
{
    // Add the end point class as one of our imports
    bAddClassImport(CIDMacroEng_SockClasses::strIPEPClassPath);
}

TMEngSocketInfo::~TMEngSocketInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngSocketInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngSocketInfo::Init(TCIDMacroEngine& meOwner)
{
    //
    //  We have a few enum types that represent various aspects of a port's
    //  configuration. Since this is a programmatically created class, we
    //  have to generate them on the fly.
    //
    //  They go into the namespace as sub-classes of our own classpath, so
    //  we pass our own class path as their root path.
    //

    // An enum to represent the exceptions that this class throws
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"SocketErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 32
        );
        m_c4ErrId_AlreadyBound = m_pmeciErrors->c4AddEnumItem(L"AlreadyBound", L"The socket is already bound");
        m_c4ErrId_NotBound = m_pmeciErrors->c4AddEnumItem(L"NotBound", L"The socket must be bound to do this");
        m_c4ErrId_BindFailed = m_pmeciErrors->c4AddEnumItem(L"BindFailed", L"The socket could not be bound");
        m_c4ErrId_AlreadyConn = m_pmeciErrors->c4AddEnumItem(L"AlreadyConn", L"The socket is already connected");
        m_c4ErrId_NotConn = m_pmeciErrors->c4AddEnumItem(L"NotConn", L"The socket is not connected");
        m_c4ErrId_AlreadyOpen = m_pmeciErrors->c4AddEnumItem(L"AlreadyOpen", L"The socket is already open");
        m_c4ErrId_NotOpen = m_pmeciErrors->c4AddEnumItem(L"NotOpen", L"The socket must be opened to do this");

        //
        //  These don't have text because we are just going to use the text that
        //  is in the C++ exception.
        //
        m_c4ErrId_ConnFailed = m_pmeciErrors->c4AddEnumItem(L"ConnFailed", TString::strEmpty());
        m_c4ErrId_Unsupported = m_pmeciErrors->c4AddEnumItem(L"Unsupported", TString::strEmpty());
        m_c4ErrId_ConnAborted = m_pmeciErrors->c4AddEnumItem(L"ConnAborted", TString::strEmpty());
        m_c4ErrId_Timeout = m_pmeciErrors->c4AddEnumItem(L"Timeout", TString::strEmpty());
        m_c4ErrId_NetworkUnreachable = m_pmeciErrors->c4AddEnumItem(L"NetworkUnreachable", TString::strEmpty());
        m_c4ErrId_NetworkDown = m_pmeciErrors->c4AddEnumItem(L"NetworkDown", TString::strEmpty());
        m_c4ErrId_ConnRefused = m_pmeciErrors->c4AddEnumItem(L"ConnRefused", TString::strEmpty());
        m_c4ErrId_DGBufTooSmall = m_pmeciErrors->c4AddEnumItem(L"DGramBufToSmall", TString::strEmpty());
        m_c4ErrId_HostUnreachable = m_pmeciErrors->c4AddEnumItem(L"HostUnreachable", TString::strEmpty());
        m_c4ErrId_NameTooLong = m_pmeciErrors->c4AddEnumItem(L"NameTooLong", TString::strEmpty());
        m_c4ErrId_ReadErr = m_pmeciErrors->c4AddEnumItem(L"ReadErr", TString::strEmpty());
        m_c4ErrId_SetOptErr = m_pmeciErrors->c4AddEnumItem(L"SetOptErr", TString::strEmpty());
        m_c4ErrId_ShutdownErr = m_pmeciErrors->c4AddEnumItem(L"ShutdownErr", TString::strEmpty());
        m_c4ErrId_UnknownErr = m_pmeciErrors->c4AddEnumItem(L"UnknownErr", TString::strEmpty());
        m_c4ErrId_WriteErr = m_pmeciErrors->c4AddEnumItem(L"WriteErr", TString::strEmpty());
        m_c4ErrId_OpenFailed = m_pmeciErrors->c4AddEnumItem(L"OpenFailed", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2TypeId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // And provide one for the socket protocol
    {
        TMEngEnumInfo* pmeciEnum = new TMEngEnumInfo
        (
            meOwner
            , L"SockProtos"
            , strClassPath()
            , L"MEng.Enum"
            , 8
        );
        pmeciEnum->c4AddEnumItem(L"IP", L"IP", tCIDSock::ESockProtos::IP);
        pmeciEnum->c4AddEnumItem(L"ICMP", L"ICMP", tCIDSock::ESockProtos::ICMP);
        pmeciEnum->c4AddEnumItem(L"IGMP", L"IGMP", tCIDSock::ESockProtos::IGMP);
        pmeciEnum->c4AddEnumItem(L"TCP", L"TCP", tCIDSock::ESockProtos::TCP);
        pmeciEnum->c4AddEnumItem(L"PUP", L"PUP", tCIDSock::ESockProtos::PUP);
        pmeciEnum->c4AddEnumItem(L"UDP", L"UDP", tCIDSock::ESockProtos::UDP);
        pmeciEnum->c4AddEnumItem(L"IDP", L"IDP", tCIDSock::ESockProtos::IDP);
        pmeciEnum->c4AddEnumItem(L"RawIP", L"RawIP", tCIDSock::ESockProtos::RawIP);
        pmeciEnum->BaseClassInit(meOwner);
        meOwner.c2AddClass(pmeciEnum);
        bAddNestedType(pmeciEnum->strClassPath());
    }

    // Look up the IP End point class id, since we use it in parms below
    const tCIDLib::TCard2 c2IPEPId = meOwner.meciFind
    (
        CIDMacroEng_SockClasses::strIPEPClassPath
    ).c2Id();


    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.Socket"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the 'data ready' method
    {
        TMEngMethodInfo methiNew
        (
            L"GetDataIsReady"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"Bytes", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetDataReady = c2AddMethodInfo(methiNew);
    }

    // Add the get is open method
    {
        TMEngMethodInfo methiNew
        (
            L"GetLocalEndPoint"
            , c2IPEPId
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetLocalEndPoint = c2AddMethodInfo(methiNew);
    }

    // Get whether we are bound
    {
        TMEngMethodInfo methiNew
        (
            L"GetIsBound"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetIsBound = c2AddMethodInfo(methiNew);
    }

    // Get whether we are open
    {
        TMEngMethodInfo methiNew
        (
            L"GetIsOpen"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetIsOpen = c2AddMethodInfo(methiNew);
    }

    // Add the 'get status' method
    {
        TMEngMethodInfo methiNew
        (
            L"GetStatus"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"Readable", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddOutParm(L"Writable", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddOutParm(L"Excepts", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddInParm(L"Millis", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_GetStatus = c2AddMethodInfo(methiNew);
    }

    // Add the set keepalive method
    {
        TMEngMethodInfo methiNew
        (
            L"SetKeepAlive"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"State", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_SetKeepAlive = c2AddMethodInfo(methiNew);
    }

    // Add the set linger method
    {
        TMEngMethodInfo methiNew
        (
            L"SetLinger"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"State", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddInParm(L"Millis", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SetLinger = c2AddMethodInfo(methiNew);
    }

    // Add the 'wait for data' method
    {
        TMEngMethodInfo methiNew
        (
            L"WaitForData"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_WaitForData = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngSocketInfo::pmecvMakeStorage(  const   TString&                strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    // These cannot be created directly, only derived classes
    facCIDMacroEng().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kMEngErrs::errcClass_ClassHasNoValue
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Internal
        , strClassPath()
    );

    // Make the compiler happy
    return 0;
}


// ---------------------------------------------------------------------------
//  TMEngSocketInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngSocketInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            , const TString&            strClassPath)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , m_pmeciErrors->strTextValue(c4ToThrow)
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngSocketInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            , const TString&            strClassPath
                            , const MFormattable&       fmtblToken)
{
    TString strText(m_pmeciErrors->strTextValue(c4ToThrow));
    strText.eReplaceToken(fmtblToken, kCIDLib::chDigit1);

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngSocketInfo::XlatSockErr(       TCIDMacroEngine&    meOwner
                            ,       TError&             errCaught
                            , const TString&            strClassPath
                            , const tCIDLib::TCard4     c4ElseErr)
{
    // If verbose log mode, we'll log the C++ level exception
    if (!errCaught.bLogged() && facCIDMacroEng().bLogWarnings())
    {
        errCaught.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errCaught);
    }


    tCIDLib::TCard4 c4Err = m_c4ErrId_UnknownErr;
    if (errCaught.strFacName() == facCIDSock().strName())
    {
        switch(errCaught.errcId())
        {
            case kSockErrs::errcSock_Unsupported :
                c4Err = m_c4ErrId_Unsupported;
                break;

            case kSockErrs::errcSock_ConnAborted :
                c4Err = m_c4ErrId_ConnAborted;
                break;

            case kSockErrs::errcSock_DGBufTooSmall :
                c4Err = m_c4ErrId_DGBufTooSmall;
                break;

            case kSockErrs::errcSock_Timeout :
                c4Err = m_c4ErrId_Timeout;
                break;

            case kSockErrs::errcSock_NetworkUnreachable :
                c4Err = m_c4ErrId_NetworkUnreachable;
                break;

            case kSockErrs::errcSock_NetworkDown :
                c4Err = m_c4ErrId_NetworkDown;
                break;

            case kSockErrs::errcSock_ConnRefused :
                c4Err = m_c4ErrId_ConnRefused;
                break;

            case kSockErrs::errcSock_NotConnected :
                c4Err = m_c4ErrId_NotConn;
                break;

            case kSockErrs::errcSock_HostUnreachable :
                c4Err = m_c4ErrId_HostUnreachable;
                break;

            case kSockErrs::errcSock_NameTooLong :
                c4Err = m_c4ErrId_NameTooLong;
                break;

            default :
                c4Err = c4ElseErr;
                break;
        };
    }

    // Set the exception info on the engine
    TString strErrText(errCaught.strErrText());
    if (errCaught.bHasAuxText())
    {
        strErrText.Append(L" [");
        strErrText.Append(errCaught.strAuxText());
        strErrText.Append(L"]");
    }
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath
        , c4Err
        , m_pmeciErrors->strPartialName(c4Err)
        , strErrText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}



// ---------------------------------------------------------------------------
//  TMEngSocketInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngSocketInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngSocketVal& mecvActual = static_cast<TMEngSocketVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        // Nothing to do here
    }
     else if (c2MethId == m_c2MethId_GetDataReady)
    {
        // Make sure it's bound
        if (!mecvActual.bIsOpen())
            ThrowAnErr(meOwner, m_c4ErrId_NotOpen, strClassPath());

        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        tCIDLib::TCard4 c4Bytes;
        if (mecvActual.sockValue(meOwner).bDataReady(c4Bytes))
        {
            TMEngCard4Val& mecvBytes = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd);
            mecvBytes.c4Value(c4Bytes);
            mecvRet.bValue(kCIDLib::True);
        }
         else
        {
            mecvRet.bValue(kCIDLib::False);
        }
    }
     else if (c2MethId == m_c2MethId_GetLocalEndPoint)
    {
        // Make sure it's bound
        if (!mecvActual.bIsBound())
            ThrowAnErr(meOwner, m_c4ErrId_NotBound, strClassPath());

        TMEngIPEPVal& mecvRet = meOwner.mecvStackAtAs<TMEngIPEPVal>(c4FirstInd - 1);
        mecvRet.ipepValue(mecvActual.sockValue(meOwner).ipepLocalEndPoint());
    }
     else if (c2MethId == m_c2MethId_GetIsBound)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.bIsBound());
    }
     else if (c2MethId == m_c2MethId_GetIsOpen)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.bIsOpen());
    }
     else if (c2MethId == m_c2MethId_GetStatus)
    {
        if (!mecvActual.bIsOpen())
            ThrowAnErr(meOwner, m_c4ErrId_NotOpen, strClassPath());

        tCIDLib::TBoolean bRead;
        tCIDLib::TBoolean bWrite;
        tCIDLib::TBoolean bExcept;

        const tCIDLib::TCard4 c4Wait = meOwner.c4StackValAt(c4FirstInd + 3);
        mecvActual.sockValue(meOwner).QuerySelStatus
        (
            bRead, bWrite, bExcept, c4Wait * kCIDLib::enctOneMilliSec
        );

        meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd).bValue(bRead);
        meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd + 1).bValue(bWrite);
        meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd + 2).bValue(bExcept);
    }
     else if (c2MethId == m_c2MethId_SetKeepAlive)
    {
        if (!mecvActual.bIsOpen())
            ThrowAnErr(meOwner, m_c4ErrId_NotOpen, strClassPath());
        try
        {
            mecvActual.sockValue(meOwner).bSendKeepAlive(meOwner.bStackValAt(c4FirstInd));
        }

        catch(TError& errToCatch)
        {
            XlatSockErr(meOwner, errToCatch, strClassPath(), m_c4ErrId_SetOptErr);
        }
    }
     else if (c2MethId == m_c2MethId_SetLinger)
    {
        if (!mecvActual.bIsOpen())
            ThrowAnErr(meOwner, m_c4ErrId_NotOpen, strClassPath());

        //
        //  The actual value set is seconds, so convert from millis. If less
        //  than a second, just go with a second.
        //
        tCIDLib::TCard4 c4Time = meOwner.c4StackValAt(c4FirstInd + 1) / 1000;
        if (!c4Time)
            c4Time = 1;

        try
        {
            mecvActual.sockValue(meOwner).bLinger
            (
                meOwner.bStackValAt(c4FirstInd), c4Time
            );
        }

        catch(TError& errToCatch)
        {
            XlatSockErr(meOwner, errToCatch, strClassPath(), m_c4ErrId_SetOptErr);
        }
    }
     else if (c2MethId == m_c2MethId_WaitForData)
    {
        // Make sure it's bound
        if (!mecvActual.bIsOpen())
            ThrowAnErr(meOwner, m_c4ErrId_NotOpen, strClassPath());

        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue
        (
            mecvActual.sockValue(meOwner).bWaitForDataReadyMS
            (
                meOwner.c4StackValAt(c4FirstInd)
            )
        );
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngStreamSocketVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngStreamSocketVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngStreamSocketVal::
TMEngStreamSocketVal(const  TString&                strName
                    , const tCIDLib::TCard2         c2ClassId
                    , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngSocketVal(strName, c2ClassId, eConst)
    , m_psockValue(0)
{
}

TMEngStreamSocketVal::~TMEngStreamSocketVal()
{
    // If not shutdown or closed or deleted, then do that
    try
    {
        Cleanup();
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  TMEngStreamSocketVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngStreamSocketVal::bDbgFormat(       TTextOutStream&     strmTarget
                                , const TMEngClassInfo&     meciThis
                                , const tCIDMacroEng::EDbgFmts eFormat
                                , const tCIDLib::ERadices   eRadix
                                , const TCIDMacroEngine&    meOwner) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        if (m_psockValue)
        {
            if (m_psockValue->bIsConnected())
                strmTarget << L"\nConnected To: " << m_psockValue->ipepRemoteEndPoint();

            if (m_psockValue->bIsBound())
                strmTarget  << L"Local End Point: " << m_psockValue->ipepLocalEndPoint();

            strmTarget  << kCIDLib::NewLn;
        }
         else
        {
            strmTarget << L"<Not Opened>";
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TMEngStreamSocketVal::bIsBound() const
{
    return (m_psockValue && m_psockValue->bIsBound());
}


tCIDLib::TBoolean TMEngStreamSocketVal::bIsOpen() const
{
    return (m_psockValue && m_psockValue->bIsOpen());
}


tCIDLib::TVoid TMEngStreamSocketVal::Close()
{
    // Clean up any existing socket
    Cleanup();
}


tCIDLib::TVoid
TMEngStreamSocketVal::Connect(  const   tCIDSock::ESockProtos eProtocol
                                , const TIPEndPoint&            ipepRemote)
{
    // If we have one and it's connected, then we need to clean it up
    if (m_psockValue && m_psockValue->bIsConnected())
        Cleanup();

    // Create a new one if not already (or we just cleaned it up)
    if (!m_psockValue)
        m_psockValue = new TClientStreamSocket(eProtocol, ipepRemote.eAddrType());
    m_psockValue->Connect(ipepRemote);
}


TSocket& TMEngStreamSocketVal::sockValue(TCIDMacroEngine& meOwner)
{
    if (!m_psockValue)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcRT_SockNotRead
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    return *m_psockValue;
}

const TSocket& TMEngStreamSocketVal::sockValue(TCIDMacroEngine& meOwner) const
{
    if (!m_psockValue)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcRT_SockNotRead
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    return *m_psockValue;
}



// ---------------------------------------------------------------------------
//  TMEngStreamSocketVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngStreamSocketVal::bIsConnected() const
{
    return (m_psockValue && m_psockValue->bIsConnected());
}


tCIDLib::TVoid TMEngStreamSocketVal::Cleanup()
{
    // If there's an existing one, then shut it down and close it if needed
    if (m_psockValue)
    {
        if (!m_psockValue->bIsShutdown())
            m_psockValue->c4Shutdown();

        delete m_psockValue;
        m_psockValue = 0;
    }
}


TClientStreamSocket&
TMEngStreamSocketVal::sockStreamValue(TCIDMacroEngine& meOwner)
{
    #if CID_DEBUG_ON
    if (!m_psockValue)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcRT_SockNotRead
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif
    return *m_psockValue;
}

const TClientStreamSocket&
TMEngStreamSocketVal::sockStreamValue(TCIDMacroEngine& meOwner) const
{
    #if CID_DEBUG_ON
    if (!m_psockValue)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcRT_SockNotRead
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif
    return *m_psockValue;
}


tCIDLib::TVoid TMEngStreamSocketVal::Shutdown()
{
    if (m_psockValue && !m_psockValue->bIsShutdown())
        m_psockValue->c4Shutdown();
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngStreamSocketInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngStreamSocketInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngStreamSocketInfo::strPath()
{
    return CIDMacroEng_SockClasses::strStreamSocketClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngStreamSocketInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngStreamSocketInfo::TMEngStreamSocketInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_SockClasses::strStreamSocket
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , CIDMacroEng_SockClasses::strSocketClassPath
    )
    , m_c2MethId_Close(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Connect(kMacroEng::c2BadId)
    , m_c2MethId_GetIsConnected(kMacroEng::c2BadId)
    , m_c2MethId_ReadBuffer(kMacroEng::c2BadId)
    , m_c2MethId_ReadBuffer2(kMacroEng::c2BadId)
    , m_c2MethId_ReadByte(kMacroEng::c2BadId)
    , m_c2MethId_ReadByte2(kMacroEng::c2BadId)
    , m_c2MethId_SetNagle(kMacroEng::c2BadId)
    , m_c2MethId_Shutdown(kMacroEng::c2BadId)
    , m_c2MethId_WriteBuffer(kMacroEng::c2BadId)
    , m_pmeciSockInfo(0)
{
    bAddClassImport(CIDMacroEng_SockClasses::strIPEPClassPath);
}

TMEngStreamSocketInfo::~TMEngStreamSocketInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngStreamSocketInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngStreamSocketInfo::Init(TCIDMacroEngine& meOwner)
{
    // Store a pointer to our macro level base class for later use
    m_pmeciSockInfo = meOwner.pmeciFindAs<TMEngSocketInfo>
    (
        CIDMacroEng_SockClasses::strSocketClassPath
    );

    // Get some class ids that we need for method defs below
    const tCIDLib::TCard2 c2ProtoId = meOwner.c2FindClassId(CIDMacroEng_SockClasses::strProtoEnumClassPath);
    const tCIDLib::TCard2 c2IPEPId = meOwner.c2FindClassId(CIDMacroEng_SockClasses::strIPEPClassPath);
    const tCIDLib::TCard2 c2IPAddrTypesId = meOwner.c2FindClassId(CIDMacroEng_SockClasses::strIPAddrTypesPath);


    // Close the socket
    {
        TMEngMethodInfo methiNew
        (
            L"Close"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Close = c2AddMethodInfo(methiNew);
    }

    // The default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.StreamSocket"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Connect to the indicated end point
    {
        TMEngMethodInfo methiNew
        (
            L"Connect"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Protocol", c2ProtoId);
        methiNew.c2AddInParm(L"RemoteEndPoint", c2IPEPId);
        m_c2MethId_Connect = c2AddMethodInfo(methiNew);
    }

    // Get whether we are connected or not
    {
        TMEngMethodInfo methiNew
        (
            L"GetIsConnected"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetIsConnected = c2AddMethodInfo(methiNew);
    }

    // Read a buffer of data with millisecond timeout
    {
        TMEngMethodInfo methiNew
        (
            L"ReadBuffer"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"WaitForMillis", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ToRead", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ReadBuffer = c2AddMethodInfo(methiNew);
    }

    // Read a buffer of data with end time
    {
        TMEngMethodInfo methiNew
        (
            L"ReadBuffer2"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"EndTime", tCIDMacroEng::EIntrinsics::Card8);
        methiNew.c2AddInParm(L"ToRead", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ReadBuffer2 = c2AddMethodInfo(methiNew);
    }

    // Read a byte of data with millisecond timeout
    {
        TMEngMethodInfo methiNew
        (
            L"ReadByte"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::Card1);
        methiNew.c2AddInParm(L"WaitForMillis", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ReadByte = c2AddMethodInfo(methiNew);
    }

    // Read a byte of data with end time
    {
        TMEngMethodInfo methiNew
        (
            L"ReadByte2"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::Card1);
        methiNew.c2AddInParm(L"EndTime", tCIDMacroEng::EIntrinsics::Card8);
        m_c2MethId_ReadByte2 = c2AddMethodInfo(methiNew);
    }

    // Enable/disable Nagle processing
    {
        TMEngMethodInfo methiNew
        (
            L"SetNagle"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_SetNagle = c2AddMethodInfo(methiNew);
    }

    // Do a polite shutdown
    {
        TMEngMethodInfo methiNew
        (
            L"Shutdown"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Shutdown = c2AddMethodInfo(methiNew);
    }

    // Write a buffer
    {
        TMEngMethodInfo methiNew
        (
            L"WriteBuffer"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"SrcData", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"ToWrite", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_WriteBuffer = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngStreamSocketInfo::pmecvMakeStorage(const   TString&                strName
                                        ,       TCIDMacroEngine&        meOwner
                                        , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngStreamSocketVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngStreamSocketInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngStreamSocketInfo::bInvokeMethod(       TCIDMacroEngine&    meOwner
                                    , const TMEngMethodInfo&    methiTarget
                                    ,       TMEngClassVal&      mecvInstance)
{
    TMEngStreamSocketVal& mecvActual = static_cast<TMEngStreamSocketVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_Close)
    {
        try
        {
            mecvActual.Close();
        }

        catch(TError& errToCatch)
        {
            if (!errToCatch.bLogged())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
        }
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Nothing to do here
    }
     else if (c2MethId == m_c2MethId_Connect)
    {
        // Make sure not already connected
        if (mecvActual.bIsConnected())
            m_pmeciSockInfo->ThrowAnErr(meOwner, m_pmeciSockInfo->m_c4ErrId_AlreadyConn, strClassPath());

        try
        {
            TMEngEnumVal& mecvProto = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
            TMEngEnumInfo& meciProto = static_cast<TMEngEnumInfo&>
            (
                meOwner.meciFind(mecvProto.c2ClassId())
            );
            TIPEndPoint ipepSrv = meOwner.mecvStackAtAs<TMEngIPEPVal>(c4FirstInd + 1).ipepValue();
            mecvActual.Connect
            (
                tCIDSock::ESockProtos(meciProto.c4MapValue(mecvProto)), ipepSrv
            );
        }

        catch(TError& errToCatch)
        {
            m_pmeciSockInfo->XlatSockErr(meOwner, errToCatch, strClassPath(), m_pmeciSockInfo->m_c4ErrId_ConnFailed);
        }
    }
     else if (c2MethId == m_c2MethId_GetIsConnected)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.bIsConnected());
    }
     else if (c2MethId == m_c2MethId_ReadBuffer)
    {
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        try
        {
            TMEngMemBufVal& mecvToFill = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd);
            TMEngCard4Val& mecvTimeOut = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 1);

            mecvRet.c4Value
            (
                mecvActual.sockStreamValue(meOwner).c4ReceiveMBufTOMS
                (
                    mecvToFill.mbufValue()
                    , mecvTimeOut.c4Value()
                    , meOwner.c4StackValAt(c4FirstInd + 2)
                    , tCIDLib::EAllData::OkIfNotAll
                )
            );
        }

        catch(TError& errToCatch)
        {
            m_pmeciSockInfo->XlatSockErr(meOwner, errToCatch, strClassPath(), m_pmeciSockInfo->m_c4ErrId_ReadErr);
        }
    }
     else if (c2MethId == m_c2MethId_ReadBuffer2)
    {
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        try
        {
            TMEngMemBufVal& mecvToFill = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd);
            mecvRet.c4Value
            (
                mecvActual.sockStreamValue(meOwner).c4ReceiveMBufTO
                (
                    mecvToFill.mbufValue()
                    , meOwner.c8StackValAt(c4FirstInd + 1)
                    , meOwner.c4StackValAt(c4FirstInd + 2)
                    , tCIDLib::EAllData::OkIfNotAll
                )
            );
        }

        catch(TError& errToCatch)
        {
            m_pmeciSockInfo->XlatSockErr(meOwner, errToCatch, strClassPath(), m_pmeciSockInfo->m_c4ErrId_ReadErr);
        }
    }
     else if (c2MethId == m_c2MethId_ReadByte)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        try
        {
            TMEngCard1Val& mecvToFill = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd);
            TMEngCard4Val& mecvTimeOut = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 1);

            tCIDLib::TCard1 c1Buf;
            const tCIDLib::TCard4 c4Res = mecvActual.sockStreamValue(meOwner).c4ReceiveRawTOMS
            (
                &c1Buf, mecvTimeOut.c4Value(), 1, tCIDLib::EAllData::OkIfNotAll
            );
            mecvRet.bValue(c4Res == 1);
            mecvToFill.c1Value(c1Buf);
        }

        catch(TError& errToCatch)
        {
            m_pmeciSockInfo->XlatSockErr(meOwner, errToCatch, strClassPath(), m_pmeciSockInfo->m_c4ErrId_ReadErr);
        }
    }
     else if (c2MethId == m_c2MethId_ReadByte2)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        try
        {
            TMEngCard1Val& mecvToFill = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd);
            tCIDLib::TCard1 c1Buf;
            const tCIDLib::TCard4 c4Res = mecvActual.sockStreamValue(meOwner).c4ReceiveRawTO
            (
                &c1Buf
                , meOwner.c8StackValAt(c4FirstInd + 1)
                , 1
                , tCIDLib::EAllData::OkIfNotAll
            );
            mecvRet.bValue(c4Res == 1);
            mecvToFill.c1Value(c1Buf);
        }

        catch(TError& errToCatch)
        {
            m_pmeciSockInfo->XlatSockErr(meOwner, errToCatch, strClassPath(), m_pmeciSockInfo->m_c4ErrId_ReadErr);
        }
    }
     else if (c2MethId == m_c2MethId_SetNagle)
    {
        try
        {
            mecvActual.sockStreamValue(meOwner).bNagleOn(meOwner.bStackValAt(c4FirstInd));
        }

        catch(TError& errToCatch)
        {
            m_pmeciSockInfo->XlatSockErr(meOwner, errToCatch, strClassPath(), m_pmeciSockInfo->m_c4ErrId_SetOptErr);
        }
    }
     else if (c2MethId == m_c2MethId_Shutdown)
    {
        try
        {
            mecvActual.Shutdown();
        }

        catch(TError& errToCatch)
        {
            m_pmeciSockInfo->XlatSockErr(meOwner, errToCatch, strClassPath(), m_pmeciSockInfo->m_c4ErrId_ShutdownErr);
        }
    }
     else if (c2MethId == m_c2MethId_WriteBuffer)
    {
        try
        {
            TMEngMemBufVal& mecvSend = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd);
            mecvActual.sockStreamValue(meOwner).Send
            (
                mecvSend.mbufValue()
                , meOwner.c4StackValAt(c4FirstInd + 1)
            );
        }

        catch(TError& errToCatch)
        {
            m_pmeciSockInfo->XlatSockErr(meOwner, errToCatch, strClassPath(), m_pmeciSockInfo->m_c4ErrId_WriteErr);
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngDGramSocketVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngDGramSocketVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngDGramSocketVal::
TMEngDGramSocketVal(const   TString&                strName
                    , const tCIDLib::TCard2         c2ClassId
                    , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngSocketVal(strName, c2ClassId, eConst)
    , m_psockValue(0)
{
}

TMEngDGramSocketVal::~TMEngDGramSocketVal()
{
    // If not closed or deleted, then do that
    try
    {
        Cleanup();
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  TMEngDGramSocketVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngDGramSocketVal::bDbgFormat(        TTextOutStream&     strmTarget
                                , const TMEngClassInfo&     meciThis
                                , const tCIDMacroEng::EDbgFmts eFormat
                                , const tCIDLib::ERadices   eRadix
                                , const TCIDMacroEngine&    meOwner) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        if (m_psockValue && m_psockValue->bIsBound())
        {
            strmTarget  << L"Local End Point: "
                        << m_psockValue->ipepLocalEndPoint() << kCIDLib::NewLn;
        }
         else
        {
            strmTarget << L"<Not Bound>";
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TMEngDGramSocketVal::bIsBound() const
{
    return (m_psockValue && m_psockValue->bIsBound());
}


tCIDLib::TBoolean TMEngDGramSocketVal::bIsOpen() const
{
    return (m_psockValue && m_psockValue->bIsOpen());
}


tCIDLib::TVoid TMEngDGramSocketVal::Close()
{
    // Clean up any existing socket
    Cleanup();
}


tCIDLib::TVoid
TMEngDGramSocketVal::Connect(const  tCIDSock::ESockProtos eProtocol
                            , const TIPEndPoint&            ipepRemote)
{
    // Not valid for this type of socket
    facCIDSock().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kSockErrs::errcSock_NotValidForType
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::TypeMatch
        , TString(L"Connect()")
        , TString(L"Datagram")
    );
}


TSocket& TMEngDGramSocketVal::sockValue(TCIDMacroEngine& meOwner)
{
    if (!m_psockValue)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcRT_SockNotRead
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    return *m_psockValue;
}

const TSocket& TMEngDGramSocketVal::sockValue(TCIDMacroEngine& meOwner) const
{
    if (!m_psockValue)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcRT_SockNotRead
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    return *m_psockValue;
}



// ---------------------------------------------------------------------------
//  TMEngDGramSocketVal: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TMEngDGramSocketVal::BindForRemote( const   tCIDSock::ESockProtos eProtocol
                                    , const TIPEndPoint&            ipepTar
                                    , const tCIDLib::TIPPortNum     ippnLocal)
{
    // Clean up any existing socket if it's already bound
    if (m_psockValue && m_psockValue->bIsBound())
        Cleanup();

    // If not created yet, do that
    if (!m_psockValue)
        m_psockValue = new TClientDatagramSocket(eProtocol, ipepTar.eAddrType());

    // And do the binding
    m_psockValue->BindForRemote(ipepTar, ippnLocal);
}

tCIDLib::TVoid
TMEngDGramSocketVal::BindListen(const   tCIDSock::ESockProtos eProtocol
                                , const tCIDSock::EAddrTypes    eAddrType
                                , const tCIDLib::TIPPortNum     ippnListen)
{
    // Clean up any existing socket if it's already bound
    if (m_psockValue && m_psockValue->bIsBound())
        Cleanup();

    // If not created yet, do that
    if (!m_psockValue)
        m_psockValue = new TClientDatagramSocket(eProtocol, eAddrType);

    // And do the binding
    TIPEndPoint ipepAll(tCIDSock::ESpecAddrs::Any, eAddrType, ippnListen);
    m_psockValue->BindListen(ipepAll);
}


tCIDLib::TVoid
TMEngDGramSocketVal::BindLocal( const   tCIDSock::ESockProtos eProtocol
                                , const TIPEndPoint&            ipepLocal)
{
    // Clean up any existing socket if it's already bound
    if (m_psockValue && m_psockValue->bIsBound())
        Cleanup();

    // If not created yet, do that
    if (!m_psockValue)
        m_psockValue = new TClientDatagramSocket(eProtocol, ipepLocal.eAddrType());

    // And do the binding
    m_psockValue->BindListen(ipepLocal);
}


tCIDLib::TVoid
TMEngDGramSocketVal::Open(  const   tCIDSock::ESockProtos eProtocol
                            , const tCIDSock::EAddrTypes    eAddrType)
{
    // Clean up any existing socket
    Cleanup();
    m_psockValue = new TClientDatagramSocket(eProtocol, eAddrType);
}


TClientDatagramSocket&
TMEngDGramSocketVal::sockDGramValue(TCIDMacroEngine& meOwner)
{
    if (!m_psockValue)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcRT_SockNotRead
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    return *m_psockValue;
}

const TClientDatagramSocket&
TMEngDGramSocketVal::sockDGramValue(TCIDMacroEngine& meOwner) const
{
    if (!m_psockValue)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcRT_SockNotRead
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    return *m_psockValue;
}


// ---------------------------------------------------------------------------
//  TMEngDGramSocketVal: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngDGramSocketVal::Cleanup()
{
    // If there's an existing one, then clean it up
    if (m_psockValue)
    {
        delete m_psockValue;
        m_psockValue = 0;
    }
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngDGramSocketInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngDGramSocketInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngDGramSocketInfo::strPath()
{
    return CIDMacroEng_SockClasses::strDGramSocketClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngDGramSocketInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngDGramSocketInfo::TMEngDGramSocketInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_SockClasses::strDGramSocket
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , CIDMacroEng_SockClasses::strSocketClassPath
    )
    , m_c2MethId_Close(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_BindListen(kMacroEng::c2BadId)
    , m_c2MethId_BindLocal(kMacroEng::c2BadId)
    , m_c2MethId_BindForRemote(kMacroEng::c2BadId)
    , m_c2MethId_Open(kMacroEng::c2BadId)
    , m_c2MethId_ReadFrom(kMacroEng::c2BadId)
    , m_c2MethId_SendTo(kMacroEng::c2BadId)
    , m_c2MethId_SendWOL(kMacroEng::c2BadId)
    , m_c2MethId_SetAllowBroadcasts(kMacroEng::c2BadId)
    , m_pmeciSockInfo(0)
{
    bAddClassImport(CIDMacroEng_SockClasses::strIPEPClassPath);
}

TMEngDGramSocketInfo::~TMEngDGramSocketInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngDGramSocketInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngDGramSocketInfo::Init(TCIDMacroEngine& meOwner)
{
    // Store a pointer to our macro level base class for later use
    m_pmeciSockInfo = meOwner.pmeciFindAs<TMEngSocketInfo>
    (
        CIDMacroEng_SockClasses::strSocketClassPath
    );

    // Get some class ids that we need for method defs below
    const tCIDLib::TCard2 c2ProtoId = meOwner.c2FindClassId(CIDMacroEng_SockClasses::strProtoEnumClassPath);
    const tCIDLib::TCard2 c2IPEPId = meOwner.c2FindClassId(CIDMacroEng_SockClasses::strIPEPClassPath);
    const tCIDLib::TCard2 c2IPAddrTypesId = meOwner.c2FindClassId(CIDMacroEng_SockClasses::strIPAddrTypesPath);


    // Bind to the any address on a specific port
    {
        TMEngMethodInfo methiNew
        (
            L"BindListen"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Protocol", c2ProtoId);
        methiNew.c2AddInParm(L"AddrType", c2IPAddrTypesId);
        methiNew.c2AddInParm(L"PortNum", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_BindListen = c2AddMethodInfo(methiNew);
    }

    // Bind to specific local address
    {
        TMEngMethodInfo methiNew
        (
            L"BindLocal"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Protocol", c2ProtoId);
        methiNew.c2AddInParm(L"LocalAddr", c2IPEPId);
        m_c2MethId_BindLocal = c2AddMethodInfo(methiNew);
    }

    // Bind locally to an appropriate address for the given remote address
    {
        TMEngMethodInfo methiNew
        (
            L"BindRemote"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Protocol", c2ProtoId);
        methiNew.c2AddInParm(L"RemAddr", c2IPEPId);
        methiNew.c2AddInParm(L"LocalPort", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_BindForRemote = c2AddMethodInfo(methiNew);
    }

    // Close the socket
    {
        TMEngMethodInfo methiNew
        (
            L"Close"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Close = c2AddMethodInfo(methiNew);
    }

    // Default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.DGramSocket"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Open the socket, no binding
    {
        TMEngMethodInfo methiNew
        (
            L"Open"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Protocol", c2ProtoId);
        methiNew.c2AddInParm(L"AddrType", c2IPAddrTypesId);
        m_c2MethId_Open = c2AddMethodInfo(methiNew);
    }

    // Read a datagram
    {
        TMEngMethodInfo methiNew
        (
            L"ReadFrom"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"SrcEndPoint", c2IPEPId);
        methiNew.c2AddInParm(L"WaitForMillis", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"ToRead", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ReadFrom = c2AddMethodInfo(methiNew);
    }

    // Send a datagram
    {
        TMEngMethodInfo methiNew
        (
            L"SendTo"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"TargetEndPoint", c2IPEPId);
        methiNew.c2AddInParm(L"SrcData", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"ToSend", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SendTo = c2AddMethodInfo(methiNew);
    }

    // Send a WOL message to the indicated MAC address
    {
        TMEngMethodInfo methiNew
        (
            L"SendWOL"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"MACAddr", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_SendWOL = c2AddMethodInfo(methiNew);
    }

    // Enable broadcast send/receive on this socket
    {
        TMEngMethodInfo methiNew
        (
            L"SetAllowBroadcasts"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_SetAllowBroadcasts = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngDGramSocketInfo::pmecvMakeStorage( const   TString&                strName
                                        ,       TCIDMacroEngine&        meOwner
                                        , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngDGramSocketVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngDGramSocketInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngDGramSocketInfo::bInvokeMethod(       TCIDMacroEngine&    meOwner
                                    , const TMEngMethodInfo&    methiTarget
                                    ,       TMEngClassVal&      mecvInstance)
{
    TMEngDGramSocketVal& mecvActual = static_cast<TMEngDGramSocketVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if ((c2MethId == m_c2MethId_BindListen) || (c2MethId == m_c2MethId_BindLocal))
    {
        // Make sure it's not already bound
        if (mecvActual.bIsBound())
            m_pmeciSockInfo->ThrowAnErr(meOwner, m_pmeciSockInfo->m_c4ErrId_AlreadyBound, strClassPath());

        try
        {
            // Find the protocol class type and convert the value to the C++enum
            TMEngEnumVal& mecvProto = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
            TMEngEnumInfo& meciProto = static_cast<TMEngEnumInfo&>
            (
                meOwner.meciFind(mecvProto.c2ClassId())
            );
            const tCIDSock::ESockProtos eProto = tCIDSock::ESockProtos
            (
                meciProto.c4MapValue(mecvProto)
            );

            if (c2MethId == m_c2MethId_BindListen)
            {
                // We get an address type and port number
                TMEngEnumVal& mecvAddrType = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1);
                TMEngEnumInfo& meciAddrType = static_cast<TMEngEnumInfo&>
                (
                    meOwner.meciFind(mecvAddrType.c2ClassId())
                );

                mecvActual.BindListen
                (
                    eProto
                    , tCIDSock::EAddrTypes(meciAddrType.c4MapValue(mecvAddrType))
                    , meOwner.c4StackValAt(c4FirstInd + 2)
                );
            }
             else
            {
                // We get an end point to bind to
                mecvActual.BindLocal
                (
                    eProto, meOwner.mecvStackAtAs<TMEngIPEPVal>(c4FirstInd + 1).ipepValue()
                );
            }
        }

        catch(TError& errToCatch)
        {
            m_pmeciSockInfo->XlatSockErr(meOwner, errToCatch, strClassPath(), m_pmeciSockInfo->m_c4ErrId_BindFailed);
        }
    }
     else if (c2MethId == m_c2MethId_BindForRemote)
    {
        // Make sure it's not already bound
        if (mecvActual.bIsBound())
            m_pmeciSockInfo->ThrowAnErr(meOwner, m_pmeciSockInfo->m_c4ErrId_AlreadyBound, strClassPath());

        try
        {
            // Find the protocol class type and convert the value to the C++enum
            TMEngEnumVal& mecvProto = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
            TMEngEnumInfo& meciProto = static_cast<TMEngEnumInfo&>
            (
                meOwner.meciFind(mecvProto.c2ClassId())
            );
            const tCIDSock::ESockProtos eProto = tCIDSock::ESockProtos
            (
                meciProto.c4MapValue(mecvProto)
            );

            mecvActual.BindForRemote
            (
                eProto
                , meOwner.mecvStackAtAs<TMEngIPEPVal>(c4FirstInd + 1).ipepValue()
                , meOwner.c4StackValAt(c4FirstInd + 2)
            );
        }

        catch(TError& errToCatch)
        {
            m_pmeciSockInfo->XlatSockErr(meOwner, errToCatch, strClassPath(), m_pmeciSockInfo->m_c4ErrId_BindFailed);
        }
    }
     else if (c2MethId == m_c2MethId_Close)
    {
        try
        {
            mecvActual.Close();
        }

        catch(TError& errToCatch)
        {
            if (!errToCatch.bLogged())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
        }
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Nothing to do here
    }
     else if (c2MethId == m_c2MethId_Open)
    {
        // Make sure it's not already open
        if (mecvActual.bIsOpen())
            m_pmeciSockInfo->ThrowAnErr(meOwner, m_pmeciSockInfo->m_c4ErrId_AlreadyOpen, strClassPath());

        try
        {
            TMEngEnumVal& mecvProto = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
            TMEngEnumInfo& meciProto = static_cast<TMEngEnumInfo&>
            (
                meOwner.meciFind(mecvProto.c2ClassId())
            );

            TMEngEnumVal& mecvAddrType = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1);
            TMEngEnumInfo& meciAddrType = static_cast<TMEngEnumInfo&>
            (
                meOwner.meciFind(mecvAddrType.c2ClassId())
            );

            mecvActual.Open
            (
                tCIDSock::ESockProtos(meciProto.c4MapValue(mecvProto))
                , tCIDSock::EAddrTypes(meciAddrType.c4MapValue(mecvAddrType))
            );
        }

        catch(TError& errToCatch)
        {
            m_pmeciSockInfo->XlatSockErr(meOwner, errToCatch, strClassPath(), m_pmeciSockInfo->m_c4ErrId_OpenFailed);
        }
    }
     else if (c2MethId == m_c2MethId_ReadFrom)
    {
        // Make sure we are bound
        if (!mecvActual.bIsOpen())
            m_pmeciSockInfo->ThrowAnErr(meOwner, m_pmeciSockInfo->m_c4ErrId_NotOpen, strClassPath());

        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        try
        {
            TMEngMemBufVal& mecvToFill = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 2);
            mecvRet.c4Value
            (
                mecvActual.sockDGramValue(meOwner).c4ReceiveMBufFrom
                (
                    meOwner.mecvStackAtAs<TMEngIPEPVal>(c4FirstInd).ipepValue()
                    , mecvToFill.mbufValue()
                    , meOwner.c4StackValAt(c4FirstInd + 1) * kCIDLib::enctOneMilliSec
                    , meOwner.c4StackValAt(c4FirstInd + 3)
                )
            );
        }

        catch(TError& errToCatch)
        {
            m_pmeciSockInfo->XlatSockErr(meOwner, errToCatch, strClassPath(), m_pmeciSockInfo->m_c4ErrId_ReadErr);
        }
    }
     else if (c2MethId == m_c2MethId_SendTo)
    {
        // Make sure we are bound
        if (!mecvActual.bIsOpen())
            m_pmeciSockInfo->ThrowAnErr(meOwner, m_pmeciSockInfo->m_c4ErrId_NotOpen, strClassPath());

        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        try
        {
            const TMEngMemBufVal& mecvSend = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 1);
            mecvRet.c4Value
            (
                mecvActual.sockDGramValue(meOwner).c4SendTo
                (
                    meOwner.mecvStackAtAs<TMEngIPEPVal>(c4FirstInd).ipepValue()
                    , mecvSend.mbufValue()
                    , meOwner.c4StackValAt(c4FirstInd + 2)
                )
            );
        }

        catch(TError& errToCatch)
        {
            m_pmeciSockInfo->XlatSockErr(meOwner, errToCatch, strClassPath(), m_pmeciSockInfo->m_c4ErrId_WriteErr);
        }
    }
     else if (c2MethId == m_c2MethId_SendWOL)
    {
        // Make sure we are bound
        if (!mecvActual.bIsOpen())
            m_pmeciSockInfo->ThrowAnErr(meOwner, m_pmeciSockInfo->m_c4ErrId_NotOpen, strClassPath());

        try
        {
            TClientDatagramSocket& sockTar = mecvActual.sockDGramValue(meOwner);
            sockTar.bAllowBroadcast(kCIDLib::True);

            sockTar.SendWOLBroadcast(meOwner.strStackValAt(c4FirstInd));
            TThread::Sleep(150);
            sockTar.SendWOLBroadcast(meOwner.strStackValAt(c4FirstInd));
            TThread::Sleep(150);
            sockTar.SendWOLBroadcast(meOwner.strStackValAt(c4FirstInd));
        }

        catch(TError& errToCatch)
        {
            m_pmeciSockInfo->XlatSockErr(meOwner, errToCatch, strClassPath(), m_pmeciSockInfo->m_c4ErrId_WriteErr);
        }
    }
     else if (c2MethId == m_c2MethId_SetAllowBroadcasts)
    {
        try
        {
            mecvActual.sockDGramValue(meOwner).bAllowBroadcast(meOwner.bStackValAt(c4FirstInd));
        }

        catch(TError& errToCatch)
        {
            m_pmeciSockInfo->XlatSockErr(meOwner, errToCatch, strClassPath(), m_pmeciSockInfo->m_c4ErrId_SetOptErr);
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngSockPingerVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngSockPingerVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngSockPingerVal::TMEngSockPingerVal( const   TString&                strName
                                        , const tCIDLib::TCard2         c2Id
                                        , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
{
}

TMEngSockPingerVal::~TMEngSockPingerVal()
{
    // Clean up the pinger if not already
    try
    {
        m_spingValue.EndPing();
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  TMEngSockPingerVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngSockPingerVal::bDbgFormat(         TTextOutStream&     strmTarget
                                , const TMEngClassInfo&     meciThis
                                , const tCIDMacroEng::EDbgFmts eFormat
                                , const tCIDLib::ERadices   eRadix
                                , const TCIDMacroEngine&    meOwner) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        if (m_spingValue.eState() != tCIDSock::EPingStates::WaitInit)
        {
            strmTarget << L"Remote Addr: "
                       << m_spingValue.strRemAddr() << kCIDLib::NewLn;
        }

        strmTarget << L"State: ";
        switch(m_spingValue.eState())
        {
            case tCIDSock::EPingStates::WaitInit :
                strmTarget << L"Waiting for Init";
                break;

            case tCIDSock::EPingStates::Idle :
                strmTarget  << L"Waiting to Send";
                break;

            case tCIDSock::EPingStates::WaitReply :
                strmTarget  << L"Waiting for Reply";
                break;

            default :
                strmTarget << L"[unknown]";
                break;
        };
        strmTarget << kCIDLib::EndLn;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TVoid
TMEngSockPingerVal::CopyFrom(   const   TMEngClassVal&      mecvToCopy
                                ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);

    // Don't copy anything. Just make sure its reset
    m_spingValue.EndPing();
}


// ---------------------------------------------------------------------------
//  TMEngSockPingerVal: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Provide access to the value
TSockPinger& TMEngSockPingerVal::spingValue()
{
    return m_spingValue;
}

const TSockPinger& TMEngSockPingerVal::spingValue() const
{
    return m_spingValue;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngSockPingerInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngSockPingerInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngSockPingerInfo::strPath()
{
    return CIDMacroEng_SockClasses::strSockPingerClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngSockPingerInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngSockPingerInfo::TMEngSockPingerInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_SockClasses::strSockPinger
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_CancelWait(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_EndPing(kMacroEng::c2BadId)
    , m_c2MethId_GetLastTime(kMacroEng::c2BadId)
    , m_c2MethId_GetRemAddr(kMacroEng::c2BadId)
    , m_c2MethId_GetState(kMacroEng::c2BadId)
    , m_c2MethId_SendRequest(kMacroEng::c2BadId)
    , m_c2MethId_StartPing(kMacroEng::c2BadId)
    , m_c2MethId_WaitReply(kMacroEng::c2BadId)
    , m_c4ErrId_CancelFailed(kCIDLib::c4MaxCard)
    , m_c4ErrId_EndFailed(kCIDLib::c4MaxCard)
    , m_c4ErrId_SendFailed(kCIDLib::c4MaxCard)
    , m_c4ErrId_StartFailed(kCIDLib::c4MaxCard)
    , m_c4ErrId_WaitFailed(kCIDLib::c4MaxCard)
    , m_c2TypeId_Errors(kMacroEng::c2BadId)
    , m_c2TypeId_States(kMacroEng::c2BadId)
    , m_pmeciErrors(0)
    , m_pmeciStates(0)
{
    // Add the end point class as one of our imports
    bAddClassImport(CIDMacroEng_SockClasses::strIPEPClassPath);
}

TMEngSockPingerInfo::~TMEngSockPingerInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngSockPingerInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngSockPingerInfo::Init(TCIDMacroEngine& meOwner)
{
    // An enum to represent the state
    {
        m_pmeciStates = new TMEngEnumInfo
        (
            meOwner
            , L"PingerStates"
            , strClassPath()
            , L"MEng.Enum"
            , 3
        );
        m_pmeciStates->c4AddEnumItem(L"WaitInit", L"WaitInit", tCIDSock::EPingStates::WaitInit);
        m_pmeciStates->c4AddEnumItem(L"WaitSend", L"WaitSend", tCIDSock::EPingStates::Idle);
        m_pmeciStates->c4AddEnumItem(L"WaitReply", L"WaitReply", tCIDSock::EPingStates::WaitReply);
        m_pmeciStates->BaseClassInit(meOwner);
        m_c2TypeId_States = meOwner.c2AddClass(m_pmeciStates);
        bAddNestedType(m_pmeciStates->strClassPath());
    }

    //
    //  An enum to represent the exceptions that this class throws. Note
    //  that we just use the text in the C++ exception in some cases, so there
    //  is no text for the enum value for those.
    //
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner, L"SockPingErrors", strClassPath(), L"MEng.Enum", 5
        );
        m_c4ErrId_CancelFailed = m_pmeciErrors->c4AddEnumItem(L"CancelFailed", L"");
        m_c4ErrId_EndFailed = m_pmeciErrors->c4AddEnumItem(L"EndFailed", L"");
        m_c4ErrId_SendFailed = m_pmeciErrors->c4AddEnumItem(L"SendFailed", L"");
        m_c4ErrId_StartFailed = m_pmeciErrors->c4AddEnumItem(L"StartFailed", L"");
        m_c4ErrId_WaitFailed = m_pmeciErrors->c4AddEnumItem(L"WaitFailed", L"");
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2TypeId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // Get some class ids that we need for method defs below
    const tCIDLib::TCard2 c2IPEPId = meOwner.c2FindClassId(CIDMacroEng_SockClasses::strIPEPClassPath);
    const tCIDLib::TCard2 c2IPAddrTypesId = meOwner.c2FindClassId(CIDMacroEng_SockClasses::strIPAddrTypesPath);

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.System.Runtime.SockPinger"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Cancel an outstanding wait
    {
        TMEngMethodInfo methiNew
        (
            L"CancelWait"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ResetStamp", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_CancelWait = c2AddMethodInfo(methiNew);
    }

    // Terminate a ping session
    {
        TMEngMethodInfo methiNew
        (
            L"EndPing"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_EndPing = c2AddMethodInfo(methiNew);
    }

    // Get the last time stamp
    {
        TMEngMethodInfo methiNew
        (
            L"GetLastTime"
            , tCIDMacroEng::EIntrinsics::Card8
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetLastTime = c2AddMethodInfo(methiNew);
    }

    // Get the remote address we are set up for, if any
    {
        TMEngMethodInfo methiNew
        (
            L"GetRemAddr"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetRemAddr = c2AddMethodInfo(methiNew);
    }

    // Get the current state
    {
        TMEngMethodInfo methiNew
        (
            L"GetState"
            , m_c2TypeId_States
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetState = c2AddMethodInfo(methiNew);
    }

    // Send a request
    {
        TMEngMethodInfo methiNew
        (
            L"SendRequest"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_SendRequest = c2AddMethodInfo(methiNew);
    }

    //
    //  Initialize the pinger for a target address. We added a second one to allow
    //  the caller to provide a local address to bind to.
    //
    {
        TMEngMethodInfo methiNew
        (
            L"StartPing"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"TarAddr", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"AddrType", c2IPAddrTypesId);
        methiNew.c2AddOutParm(L"ResAddr", c2IPEPId);
        m_c2MethId_StartPing = c2AddMethodInfo(methiNew);
    }

    // Wait for a reply
    {
        TMEngMethodInfo methiNew
        (
            L"WaitReply"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"WaitMillis", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"RepMillis", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_WaitReply = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngSockPingerInfo::pmecvMakeStorage(  const   TString&                strName
                                        ,       TCIDMacroEngine&        meOwner
                                        , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngSockPingerVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngSockPingerInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngSockPingerInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                    , const TMEngMethodInfo&    methiTarget
                                    ,       TMEngClassVal&      mecvInstance)
{
    TMEngSockPingerVal& mecvActual = static_cast<TMEngSockPingerVal&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_CancelWait)
    {
        try
        {
            mecvActual.spingValue().CancelReply(meOwner.bStackValAt(c4FirstInd));
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_CancelFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Make sure the object is reset
        mecvActual.spingValue().EndPing();
    }
     else if (c2MethId == m_c2MethId_EndPing)
    {
        try
        {
            mecvActual.spingValue().EndPing();
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_EndFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_GetLastTime)
    {
        TMEngCard8Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard8Val>(c4FirstInd - 1);
        mecvRet.c8Value(mecvActual.spingValue().enctLastTime());
    }
     else if (c2MethId == m_c2MethId_GetState)
    {
        TMEngEnumVal& mecvState = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
        mecvState.c4Ordinal
        (
            m_pmeciStates->c4FromMapValue(mecvActual.spingValue().eState())
        );
    }
     else if (c2MethId == m_c2MethId_GetRemAddr)
    {
        TMEngStringVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        mecvRet.strValue(mecvActual.spingValue().strRemAddr());
    }
     else if (c2MethId == m_c2MethId_StartPing)
    {
        try
        {
            TMEngEnumVal& mecvType = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1);
            TMEngEnumInfo& meciType = static_cast<TMEngEnumInfo&>
            (
                meOwner.meciFind(mecvType.c2ClassId())
            );
            const tCIDSock::EAddrTypes eAddrType
            (
                tCIDSock::EAddrTypes(meciType.c4MapValue(mecvType))
            );

            TIPAddress ipaTar;
            mecvActual.spingValue().StartPing
            (
                meOwner.strStackValAt(c4FirstInd), eAddrType, ipaTar
            );

            TMEngIPEPVal& mecvEP = meOwner.mecvStackAtAs<TMEngIPEPVal>(c4FirstInd + 2);
            mecvEP.ipepValue() = TIPEndPoint(ipaTar, 0);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_StartFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_SendRequest)
    {
        try
        {
            mecvActual.spingValue().SendRequest();
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_SendFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_WaitReply)
    {
        try
        {
            tCIDLib::TCard4 c4RepMillis;
            tCIDLib::TCard4 c4RepBytes;
            TIPAddress      ipaFrom;
            const tCIDLib::TBoolean bStatus = mecvActual.spingValue().bWaitReply
            (
                meOwner.c4StackValAt(c4FirstInd), c4RepMillis, c4RepBytes, ipaFrom
            );

            TMEngCard4Val& mecvRepMSs = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd + 1);
            mecvRepMSs.c4Value(c4RepMillis);

            TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
            mecvRet.bValue(bStatus);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_WaitFailed, errToCatch);
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngSockPingerInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngSockPingerInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                                , const tCIDLib::TCard4     c4ToThrow
                                ,       TError&             errCaught)
{
    // If verbose log mode, we'll log the C++ level exception
    if (!errCaught.bLogged() && facCIDMacroEng().bLogWarnings())
    {
        errCaught.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errCaught);
    }

    TString strErr(errCaught.strErrText());
    if (errCaught.bHasAuxText())
    {
        strErr.Append(L" [");
        strErr.Append(errCaught.strAuxText());
        strErr.Append(L"]");
    }

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2TypeId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strErr
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

