//
// FILE NAME: CIDMacroEng_CommClasses.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/26/2003
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
//  This file implements a set of derivatives of the class info and class
//  value classes, which implement the macro level comm port class and the
//  comm port configuration info class.
//
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
RTTIDecls(TMEngCommCfgVal,TMEngClassVal)
RTTIDecls(TMEngCommCfgInfo,TMEngClassInfo)
RTTIDecls(TMEngCommPortVal,TMEngClassVal)
RTTIDecls(TMEngCommPortInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_CommClasses
{
    // -----------------------------------------------------------------------
    //  The names for the types that we support here. Each derivative has to
    //  be able to return strings that contain its name and full name.
    // -----------------------------------------------------------------------
    const TString   strCommCfg(L"CommCfg");
    const TString   strCommCfgClassPath(L"MEng.System.Runtime.CommCfg");

    const TString   strDataBitsClassPath(L"MEng.System.Runtime.CommCfg.DataBits");
    const TString   strDTRFlagsClassPath(L"MEng.System.Runtime.CommCfg.DTRFlags");
    const TString   strParitiesClassPath(L"MEng.System.Runtime.CommCfg.Parities");
    const TString   strCommPortFlagsClassPath(L"MEng.System.Runtime.CommCfg.CommPortFlags");
    const TString   strRTSFlagsClassPath(L"MEng.System.Runtime.CommCfg.RTSFlags");
    const TString   strStopBitsClassPath(L"MEng.System.Runtime.CommCfg.StopBits");

    const TString   strCommPort(L"CommPort");
    const TString   strCommPortClassPath(L"MEng.System.Runtime.CommPort");
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngCommCfgVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCommCfgVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCommCfgVal::TMEngCommCfgVal(const  TString&                strName
                                , const tCIDLib::TCard2         c2ClassId
                                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2ClassId, eConst)
    , m_pcpcfgValue(new TCommPortCfg)
{
}

TMEngCommCfgVal::~TMEngCommCfgVal()
{
    try
    {
        delete m_pcpcfgValue;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TMEngCommCfgVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCommCfgVal::bDbgFormat(        TTextOutStream&     strmTarget
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices
                            , const TCIDMacroEngine&    ) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        strmTarget << L"Baud: " << m_pcpcfgValue->c4Baud() << kCIDLib::NewLn
                   << L"Data Bits: " << m_pcpcfgValue->eDataBits() << kCIDLib::NewLn
                   << L"Parity: " << m_pcpcfgValue->eParity() << kCIDLib::NewLn
                   << L"Stop Bits: " << m_pcpcfgValue->eStopBits() << kCIDLib::NewLn;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TVoid
TMEngCommCfgVal::CopyFrom(  const   TMEngClassVal&      mecvToCopy
                            ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    *m_pcpcfgValue = *static_cast<const TMEngCommCfgVal&>(mecvToCopy).m_pcpcfgValue;
}


// ---------------------------------------------------------------------------
//  TMEngCommCfgVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
TCommPortCfg& TMEngCommCfgVal::cpcfgValue()
{
    return *m_pcpcfgValue;
}

TCommPortCfg& TMEngCommCfgVal::cpcfgValue(const TCommPortCfg& cpcfgToSet)
{
    *m_pcpcfgValue = cpcfgToSet;
    return *m_pcpcfgValue;
}

const TCommPortCfg& TMEngCommCfgVal::cpcfgValue() const
{
    return *m_pcpcfgValue;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngCommCfgInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCommCfgInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngCommCfgInfo::strPath()
{
    return CIDMacroEng_CommClasses::strCommCfgClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngCommCfgInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCommCfgInfo::TMEngCommCfgInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_CommClasses::strCommCfg
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_ClearPortFlag(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_GetBaud(kMacroEng::c2BadId)
    , m_c2MethId_GetDataBits(kMacroEng::c2BadId)
    , m_c2MethId_GetDTRFlag(kMacroEng::c2BadId)
    , m_c2MethId_GetParity(kMacroEng::c2BadId)
    , m_c2MethId_GetPortFlag(kMacroEng::c2BadId)
    , m_c2MethId_GetRTSFlag(kMacroEng::c2BadId)
    , m_c2MethId_GetStopBits(kMacroEng::c2BadId)
    , m_c2MethId_SetBaud(kMacroEng::c2BadId)
    , m_c2MethId_SetDataBits(kMacroEng::c2BadId)
    , m_c2MethId_SetDTRFlag(kMacroEng::c2BadId)
    , m_c2MethId_SetParity(kMacroEng::c2BadId)
    , m_c2MethId_SetPortFlag(kMacroEng::c2BadId)
    , m_c2MethId_SetRTSFlag(kMacroEng::c2BadId)
    , m_c2MethId_SetStopBits(kMacroEng::c2BadId)
    , m_c2MethId_ValCtor(kMacroEng::c2BadId)
    , m_c2TypeId_DataBits(kMacroEng::c2BadId)
    , m_c2TypeId_DTRFlags(kMacroEng::c2BadId)
    , m_c2TypeId_Parities(kMacroEng::c2BadId)
    , m_c2TypeId_PortFlags(kMacroEng::c2BadId)
    , m_c2TypeId_RTSFlags(kMacroEng::c2BadId)
    , m_c2TypeId_StopBits(kMacroEng::c2BadId)
    , m_pmeciDataBits(nullptr)
    , m_pmeciDTRFlags(nullptr)
    , m_pmeciParities(nullptr)
    , m_pmeciPortFlags(nullptr)
    , m_pmeciRTSFlags(nullptr)
    , m_pmeciStopBits(nullptr)
{
}

TMEngCommCfgInfo::~TMEngCommCfgInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngCommCfgInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngCommCfgInfo::Init(TCIDMacroEngine& meOwner)
{
    //
    //  We have a few enum types that represent various aspects of a port's
    //  configuration. Since this is a programmatically created class, we
    //  have to generate them on the fly.
    //
    //  They go into the namespace as sub-classes of our own classpath, so
    //  we pass our own class path as their root path.
    //
    //  NOTE:   The order is important in these, so that they can be cast
    //          directly to their associated underlying C++ enums.
    //

    //
    //  Do a data bits enum, which indicatse how many bits each data byte
    //  in the transfer will have.
    //
    {
        m_pmeciDataBits = new TMEngEnumInfo
        (
            meOwner, L"DataBits", strClassPath(), L"MEng.Enum", 5
        );
        m_pmeciDataBits->c4AddEnumItem(L"Four", L"4 Data Bits", tCIDComm::EDataBits::Four);
        m_pmeciDataBits->c4AddEnumItem(L"Five", L"5 Data Bits", tCIDComm::EDataBits::Five);
        m_pmeciDataBits->c4AddEnumItem(L"Six", L"6 Data Bits", tCIDComm::EDataBits::Six);
        m_pmeciDataBits->c4AddEnumItem(L"Seven", L"7 Data Bits", tCIDComm::EDataBits::Seven);
        m_pmeciDataBits->c4AddEnumItem(L"Eight", L"8 Data Bits", tCIDComm::EDataBits::Eight);

        m_pmeciDataBits->BaseClassInit(meOwner);
        m_c2TypeId_DataBits = meOwner.c2AddClass(m_pmeciDataBits);
        bAddNestedType(m_pmeciDataBits->strClassPath());
    }

    //
    //  Do a DTR flags enum, which indicates how the DTR line should be
    //  used by the comm driver.
    //
    {
        m_pmeciDTRFlags = new TMEngEnumInfo
        (
            meOwner, L"DTRFlags", strClassPath(), L"MEng.Enum", 5
        );
        m_pmeciDTRFlags->c4AddEnumItem(L"Disable", L"Disable DTR", tCIDComm::EPortDTR::Disable);
        m_pmeciDTRFlags->c4AddEnumItem(L"Enable", L"Enable DTR", tCIDComm::EPortDTR::Enable);
        m_pmeciDTRFlags->c4AddEnumItem(L"Handshake", L"Handshake DTR", tCIDComm::EPortDTR::Handshake);

        m_pmeciDTRFlags->BaseClassInit(meOwner);
        m_c2TypeId_DTRFlags = meOwner.c2AddClass(m_pmeciDTRFlags);
        bAddNestedType(m_pmeciDTRFlags->strClassPath());
    }

    //
    //  An enum to represent the possible parity types that a port can
    //  use. Whether it's enforced or not is dependent on the parity
    //  checking flag.
    //
    {
        m_pmeciParities = new TMEngEnumInfo
        (
            meOwner, L"Parities", strClassPath(), L"MEng.Enum", 5
        );
        m_pmeciParities->c4AddEnumItem(L"None", L"No Parity", tCIDComm::EParities::None);
        m_pmeciParities->c4AddEnumItem(L"Odd", L"Odd Parity", tCIDComm::EParities::Odd);
        m_pmeciParities->c4AddEnumItem(L"Even", L"Even Parity", tCIDComm::EParities::Even);
        m_pmeciParities->c4AddEnumItem(L"Mark", L"Mark Parity", tCIDComm::EParities::Mark);
        m_pmeciParities->c4AddEnumItem(L"Space", L"Space Parity", tCIDComm::EParities::Space);

        m_pmeciParities->BaseClassInit(meOwner);
        m_c2TypeId_Parities = meOwner.c2AddClass(m_pmeciParities);
        bAddNestedType(m_pmeciParities->strClassPath());
    }

    //
    //  General port flags. These allow various protocol options to be turned
    //  on or off.
    //
    {
        m_pmeciPortFlags = new TMEngEnumInfo
        (
            meOwner, L"CommPortFlags", strClassPath(), L"MEng.Enum", 5
        );
        m_pmeciPortFlags->c4AddEnumItem(L"Parity", L"Control parity checking", tCIDComm::EFlags::ParityChecking);
        m_pmeciPortFlags->c4AddEnumItem(L"CTSOut", L"Control CTS line assertion", tCIDComm::EFlags::CTSOut);
        m_pmeciPortFlags->c4AddEnumItem(L"DSROut", L"Control DSR line assertion", tCIDComm::EFlags::DSROut);
        m_pmeciPortFlags->c4AddEnumItem(L"XOut", L"Control XOut usage", tCIDComm::EFlags::XOut);
        m_pmeciPortFlags->c4AddEnumItem(L"XIn", L"Control XIn usage", tCIDComm::EFlags::XIn);

        m_pmeciPortFlags->BaseClassInit(meOwner);
        m_c2TypeId_PortFlags = meOwner.c2AddClass(m_pmeciPortFlags);
        bAddNestedType(m_pmeciPortFlags->strClassPath());
    }

    //
    //  Do a DTR flags enum, which indicates how the DTR line should be
    //  used by the comm driver.
    //
    {
        m_pmeciRTSFlags = new TMEngEnumInfo
        (
            meOwner, L"RTSFlags", strClassPath(), L"MEng.Enum", 5
        );
        m_pmeciRTSFlags->c4AddEnumItem(L"Disable", L"Disable RTS", tCIDComm::EPortRTS::Disable);
        m_pmeciRTSFlags->c4AddEnumItem(L"Enable", L"Enable RTS", tCIDComm::EPortRTS::Enable);
        m_pmeciRTSFlags->c4AddEnumItem(L"Handshake", L"Handshake RTS", tCIDComm::EPortRTS::Handshake);
        m_pmeciRTSFlags->c4AddEnumItem(L"Toggle", L"Toggle RTS", tCIDComm::EPortRTS::Toggle);

        m_pmeciRTSFlags->BaseClassInit(meOwner);
        m_c2TypeId_RTSFlags = meOwner.c2AddClass(m_pmeciRTSFlags);
        bAddNestedType(m_pmeciRTSFlags->strClassPath());
    }

    //
    //  Now create one for the stop bits. It just has 3 values for the
    //  three standard settings.
    //
    {
        m_pmeciStopBits = new TMEngEnumInfo
        (
            meOwner, L"StopBits", strClassPath(), L"MEng.Enum", 3
        );

        m_pmeciStopBits->c4AddEnumItem(L"One", L"1 StopBits", tCIDComm::EStopBits::One);
        m_pmeciStopBits->c4AddEnumItem(L"OnePtFive", L"1.5 StopBits", tCIDComm::EStopBits::OnePointFive);
        m_pmeciStopBits->c4AddEnumItem(L"Two", L"2 StopBits", tCIDComm::EStopBits::Two);

        m_pmeciStopBits->BaseClassInit(meOwner);
        m_c2TypeId_StopBits = meOwner.c2AddClass(m_pmeciStopBits);
        bAddNestedType(m_pmeciStopBits->strClassPath());
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.CommCfg"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Clear a port flag
    {
        TMEngMethodInfo methiNew
        (
            L"ClearPortFlag"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToChange", m_c2TypeId_PortFlags);
        m_c2MethId_SetPortFlag = c2AddMethodInfo(methiNew);
    }

    // Add getters for all the fields
    {
        TMEngMethodInfo methiNew
        (
            L"GetBaud"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetBaud = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetDataBits"
            , m_c2TypeId_DataBits
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetDataBits = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetDTRFlag"
            , m_c2TypeId_DTRFlags
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetDTRFlag = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetParity"
            , m_c2TypeId_Parities
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetParity = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetPortFlag"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToGet", m_c2TypeId_PortFlags);
        m_c2MethId_GetPortFlag = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetRTSFlag"
            , m_c2TypeId_RTSFlags
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetRTSFlag = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetStopBits"
            , m_c2TypeId_StopBits
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetStopBits = c2AddMethodInfo(methiNew);
    }


    // Add setters for all the fields
    {
        TMEngMethodInfo methiNew
        (
            L"SetBaud"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SetBaud = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"SetDataBits"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", m_c2TypeId_DataBits);
        m_c2MethId_SetDataBits = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"SetDTRFlag"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", m_c2TypeId_DTRFlags);
        m_c2MethId_SetDTRFlag = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"SetParity"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", m_c2TypeId_Parities);
        m_c2MethId_SetParity = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"SetPortFlag"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToChange", m_c2TypeId_PortFlags);
        m_c2MethId_SetPortFlag = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"SetRTSFlag"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", m_c2TypeId_RTSFlags);
        m_c2MethId_SetRTSFlag = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"SetStopBits"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", m_c2TypeId_StopBits);
        m_c2MethId_SetStopBits = c2AddMethodInfo(methiNew);
    }

    // Add a constructor that takes all the values
    {
        TMEngMethodInfo methiNew
        (
            L"ctor2_MEng.System.Runtime.CommCfg"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Baud", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"Parity", m_c2TypeId_Parities);
        methiNew.c2AddInParm(L"DataBits", m_c2TypeId_DataBits);
        methiNew.c2AddInParm(L"StopBits", m_c2TypeId_StopBits);
        methiNew.c2AddInParm(L"DTRFlag", m_c2TypeId_DTRFlags);
        methiNew.c2AddInParm(L"RTSFlag", m_c2TypeId_RTSFlags);
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_ValCtor = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngCommCfgInfo::pmecvMakeStorage( const   TString&                strName
                                    ,       TCIDMacroEngine&
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngCommCfgVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngCommCfgInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCommCfgInfo::bInvokeMethod(       TCIDMacroEngine&    meOwner
                            , const TMEngMethodInfo&    methiTarget
                            ,       TMEngClassVal&      mecvInstance)
{
    TMEngCommCfgVal& mecvActual = static_cast<TMEngCommCfgVal&>(mecvInstance);
    TCommPortCfg& cpcfgVal = mecvActual.cpcfgValue();

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_ClearPortFlag)
    {
        TMEngEnumVal& mecvVal = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        cpcfgVal.eFlags
        (
            tCIDComm::EFlags
            (
                tCIDLib::TCard4(cpcfgVal.eFlags())
                & ~m_pmeciPortFlags->c4MapValue(mecvVal)
            )
        );
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Just reset the object
        cpcfgVal.Reset();
    }
     else if (c2MethId == m_c2MethId_GetBaud)
    {
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(cpcfgVal.c4Baud());
    }
     else if (c2MethId == m_c2MethId_GetDataBits)
    {
        TMEngEnumVal& mecvRet = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
        mecvRet.c4Ordinal(m_pmeciDataBits->c4FromMapValue(cpcfgVal.eDataBits()));
    }
     else if (c2MethId == m_c2MethId_GetDTRFlag)
    {
        TMEngEnumVal& mecvRet = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
        mecvRet.c4Ordinal(m_pmeciDTRFlags->c4FromMapValue(cpcfgVal.eDTR()));
    }
     else if (c2MethId == m_c2MethId_GetParity)
    {
        TMEngEnumVal& mecvRet = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
        mecvRet.c4Ordinal(m_pmeciParities->c4FromMapValue(cpcfgVal.eParity()));
    }
     else if (c2MethId == m_c2MethId_GetPortFlag)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue
        (
            (m_pmeciPortFlags->c4MapValue(meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd))
             & tCIDLib::TCard4(cpcfgVal.eFlags())) != 0
        );
    }
     else if (c2MethId == m_c2MethId_GetRTSFlag)
    {
        TMEngEnumVal& mecvRet = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
        mecvRet.c4Ordinal(m_pmeciRTSFlags->c4FromMapValue(cpcfgVal.eRTS()));
    }
     else if (c2MethId == m_c2MethId_GetStopBits)
    {
        TMEngEnumVal& mecvRet = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd - 1);
        mecvRet.c4Ordinal(m_pmeciStopBits->c4FromMapValue(cpcfgVal.eStopBits()));
    }
     else if (c2MethId == m_c2MethId_SetBaud)
    {
        cpcfgVal.c4Baud(meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_SetDataBits)
    {
        TMEngEnumVal& mecvVal = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        cpcfgVal.eDataBits(tCIDComm::EDataBits(m_pmeciDataBits->c4MapValue(mecvVal)));
    }
     else if (c2MethId == m_c2MethId_SetDTRFlag)
    {
        TMEngEnumVal& mecvVal = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        cpcfgVal.eDTR(tCIDComm::EPortDTR(m_pmeciDTRFlags->c4MapValue(mecvVal)));
    }
     else if (c2MethId == m_c2MethId_SetParity)
    {
        TMEngEnumVal& mecvVal = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        cpcfgVal.eParity(tCIDComm::EParities(m_pmeciParities->c4MapValue(mecvVal)));
    }
     else if (c2MethId == m_c2MethId_SetPortFlag)
    {
        TMEngEnumVal& mecvVal = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        cpcfgVal.eFlags
        (
            tCIDComm::EFlags
            (
                tCIDLib::TCard4(cpcfgVal.eFlags()) | m_pmeciPortFlags->c4MapValue(mecvVal)
            )
        );
    }
     else if (c2MethId == m_c2MethId_SetRTSFlag)
    {
        TMEngEnumVal& mecvVal = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        cpcfgVal.eRTS(tCIDComm::EPortRTS(m_pmeciRTSFlags->c4MapValue(mecvVal)));
    }
     else if (c2MethId == m_c2MethId_SetStopBits)
    {
        TMEngEnumVal& mecvVal = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        cpcfgVal.eStopBits(tCIDComm::EStopBits(m_pmeciStopBits->c4MapValue(mecvVal)));
    }
     else if (c2MethId == m_c2MethId_ValCtor)
    {
        //
        //  Set the value with new data. We can't do the flags, since we don't
        //  have a bitmapped capability, so we just default the flags.
        //
        cpcfgVal.SetAll
        (
            meOwner.c4StackValAt(c4FirstInd)
            , tCIDComm::EFlags::None
            , tCIDComm::EParities(m_pmeciParities->c4MapValue(meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 1)))
            , tCIDComm::EDataBits(m_pmeciDataBits->c4MapValue(meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 2)))
            , tCIDComm::EStopBits(m_pmeciStopBits->c4MapValue(meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 3)))
            , tCIDComm::EPortDTR(m_pmeciDTRFlags->c4MapValue(meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 4)))
            , tCIDComm::EPortRTS(m_pmeciRTSFlags->c4MapValue(meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd + 5)))
        );
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngCommPortVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCommPortVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCommPortVal::TMEngCommPortVal( const   TString&                strName
                                    , const tCIDLib::TCard2         c2ClassId
                                    , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2ClassId, eConst)
    , m_pcommValue(0)
{
}

TMEngCommPortVal::~TMEngCommPortVal()
{
    // Clean up the comm port if not already
    try
    {
        // Do a close on it if it exists, just to be sure
        if (m_pcommValue)
            m_pcommValue->Close();
    }

    catch(...)
    {
    }

    try
    {
        // And delete it if it got created
        delete m_pcommValue;
    }

    catch(...)
    {
    }
}


// ---------------------------------------------------------------------------
//  TMEngCommPortVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCommPortVal::bDbgFormat(       TTextOutStream&     strmTarget
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices
                            , const TCIDMacroEngine&) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        strmTarget << L"Port: ";
        if (m_pcommValue && m_pcommValue->bIsOpen())
            strmTarget << m_pcommValue->strPath();
        else
            strmTarget << L"[not opened yet]";
        strmTarget << kCIDLib::NewLn;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TMEngCommPortVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TMEngCommPortVal::bIsOpen() const
{
    // If we've not created it yet, then obviously not open
    if (!m_pcommValue)
        return kCIDLib::False;
    return m_pcommValue->bIsOpen();
}


TCommPortBase& TMEngCommPortVal::commValue()
{
    // Should already exists, but check just to be sure, if debugging
    #if CID_DEBUG_ON
    if (!m_pcommValue)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcDbg_NotCreated
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , TString(L"serial port")
        );
    }
    #endif
    return *m_pcommValue;
}

const TCommPortBase& TMEngCommPortVal::commValue() const
{
    // Should already exists, but check just to be sure, if debugging
    #if CID_DEBUG_ON
    if (!m_pcommValue)
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcDbg_NotCreated
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , TString(L"serial port")
        );
    }
    #endif
    return *m_pcommValue;
}


tCIDLib::TVoid TMEngCommPortVal::Open(const TString& strPath)
{
    // If we've not created it yet, then do so, then try to open
    if (!m_pcommValue)
    {
        m_pcommValue = facCIDComm().pcommMakeNew(strPath);
        if (!m_pcommValue)
        {
            facCIDComm().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCommErrs::errcPort_Open
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , strPath
            );
        }
    }
    m_pcommValue->Open(tCIDComm::EOpenFlags::WriteThrough);
}

tCIDLib::TVoid
TMEngCommPortVal::Open( const   TString&        strPath
                        , const TCommPortCfg&   ccfgToUse
                        , const tCIDLib::TCard4 c4ReadSz
                        , const tCIDLib::TCard4 c4WriteSz)
{
    // If we've not created it yet, then do so, then try to open
    if (!m_pcommValue)
    {
        m_pcommValue = facCIDComm().pcommMakeNew(strPath);
        if (!m_pcommValue)
        {
            facCIDComm().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCommErrs::errcPort_Open
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , strPath
            );
        }
    }
    m_pcommValue->Open(tCIDComm::EOpenFlags::WriteThrough, c4ReadSz, c4WriteSz);
    m_pcommValue->SetCfg(ccfgToUse);
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngCommPortInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCommPortInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngCommPortInfo::strPath()
{
    return CIDMacroEng_CommClasses::strCommPortClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngCommPortInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCommPortInfo::TMEngCommPortInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_CommClasses::strCommPort
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_AssertCTS(kMacroEng::c2BadId)
    , m_c2MethId_AssertDSR(kMacroEng::c2BadId)
    , m_c2MethId_ClearPortFlag(kMacroEng::c2BadId)
    , m_c2MethId_Close(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_GetIsOpen(kMacroEng::c2BadId)
    , m_c2MethId_GetPort(kMacroEng::c2BadId)
    , m_c2MethId_Open(kMacroEng::c2BadId)
    , m_c2MethId_OpenCfg(kMacroEng::c2BadId)
    , m_c2MethId_OpenCfgBufs(kMacroEng::c2BadId)
    , m_c2MethId_GetCTSOn(kMacroEng::c2BadId)
    , m_c2MethId_GetDSROn(kMacroEng::c2BadId)
    , m_c2MethId_GetLineStates(kMacroEng::c2BadId)
    , m_c2MethId_PurgeReadBuf(kMacroEng::c2BadId)
    , m_c2MethId_PurgeWriteBuf(kMacroEng::c2BadId)
    , m_c2MethId_ReadBuf(kMacroEng::c2BadId)
    , m_c2MethId_ReadByte(kMacroEng::c2BadId)
    , m_c2MethId_SetBreak(kMacroEng::c2BadId)
    , m_c2MethId_SetCfg(kMacroEng::c2BadId)
    , m_c2MethId_SetDTRFlag(kMacroEng::c2BadId)
    , m_c2MethId_SetPortFlag(kMacroEng::c2BadId)
    , m_c2MethId_SetRTSFlag(kMacroEng::c2BadId)
    , m_c2MethId_WriteBuf(kMacroEng::c2BadId)
    , m_c2MethId_WriteByte(kMacroEng::c2BadId)
    , m_c2TypeId_Errors(kMacroEng::c2BadId)
    , m_c4ErrId_AlreadyOpen(kCIDLib::c4MaxCard)
    , m_c4ErrId_CfgFailed(kCIDLib::c4MaxCard)
    , m_c4ErrId_CloseFailed(kCIDLib::c4MaxCard)
    , m_c4ErrId_NotOpen(kCIDLib::c4MaxCard)
    , m_c4ErrId_OpenFailed(kCIDLib::c4MaxCard)
    , m_c4ErrId_QueryLineStateErr(kCIDLib::c4MaxCard)
    , m_c4ErrId_PurgeErr(kCIDLib::c4MaxCard)
    , m_c4ErrId_ReadErr(kCIDLib::c4MaxCard)
    , m_c4ErrId_SetBreakErr(kCIDLib::c4MaxCard)
    , m_c4ErrId_SetFlagErr(kCIDLib::c4MaxCard)
    , m_c4ErrId_SetLineStateErr(kCIDLib::c4MaxCard)
    , m_c4ErrId_WriteErr(kCIDLib::c4MaxCard)
    , m_pmeciErrors(0)
{
    //
    //  Make sure the comm cfg class is imported for us and memory buffer
    //  class are imported for us.
    //
    bAddClassImport(CIDMacroEng_CommClasses::strCommCfgClassPath);
}

TMEngCommPortInfo::~TMEngCommPortInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngCommPortInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngCommPortInfo::Init(TCIDMacroEngine& meOwner)
{
    // An enum to represent the exceptions that this class throws
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"ComPortErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 12
        );
        m_c4ErrId_AlreadyOpen = m_pmeciErrors->c4AddEnumItem(L"AlreadyOpen", L"Port %(1) is already open");
        m_c4ErrId_CfgFailed = m_pmeciErrors->c4AddEnumItem(L"CfgFailed", TString::strEmpty());
        m_c4ErrId_CloseFailed = m_pmeciErrors->c4AddEnumItem(L"CloseFailed", TString::strEmpty());
        m_c4ErrId_NotOpen = m_pmeciErrors->c4AddEnumItem(L"NotOpen", L"The comm port must be open to do this");
        m_c4ErrId_OpenFailed = m_pmeciErrors->c4AddEnumItem(L"OpenFailed", TString::strEmpty());
        m_c4ErrId_PurgeErr = m_pmeciErrors->c4AddEnumItem(L"PurgeFailed", TString::strEmpty());
        m_c4ErrId_QueryLineStateErr = m_pmeciErrors->c4AddEnumItem(L"QueryLineStateErr", TString::strEmpty());
        m_c4ErrId_ReadErr = m_pmeciErrors->c4AddEnumItem(L"ReadErr", TString::strEmpty());
        m_c4ErrId_SetBreakErr = m_pmeciErrors->c4AddEnumItem(L"SetBreakErr", TString::strEmpty());
        m_c4ErrId_SetFlagErr = m_pmeciErrors->c4AddEnumItem(L"SetFlagErr", TString::strEmpty());
        m_c4ErrId_SetLineStateErr = m_pmeciErrors->c4AddEnumItem(L"SetLineStateErr", TString::strEmpty());
        m_c4ErrId_WriteErr = m_pmeciErrors->c4AddEnumItem(L"WriteErr", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2TypeId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    //
    //  We use the CommCfg class and some of its nested types, so we need to
    //  look them up first.
    //
    const tCIDLib::TCard2 c2CommCfg = meOwner.c2FindClassId(CIDMacroEng_CommClasses::strCommCfgClassPath);
    const tCIDLib::TCard2 c2DTRFlags = meOwner.c2FindClassId(CIDMacroEng_CommClasses::strDTRFlagsClassPath);
    const tCIDLib::TCard2 c2PortFlags = meOwner.c2FindClassId(CIDMacroEng_CommClasses::strCommPortFlagsClassPath);
    const tCIDLib::TCard2 c2RTSFlags = meOwner.c2FindClassId(CIDMacroEng_CommClasses::strRTSFlagsClassPath);

    // Assert or don't assert the CTS line
    {
        TMEngMethodInfo methiNew
        (
            L"AssertCTS"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"State", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_AssertCTS = c2AddMethodInfo(methiNew);
    }

    // Assert or don't assert the DSR line
    {
        TMEngMethodInfo methiNew
        (
            L"AssertDSR"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"State", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_AssertDSR = c2AddMethodInfo(methiNew);
    }

    // Clear one of the port flags
    {
        TMEngMethodInfo methiNew
        (
            L"ClearPortFlag"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"FlagToClear", c2PortFlags);
        m_c2MethId_ClearPortFlag = c2AddMethodInfo(methiNew);
    }

    // Add the close method
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

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.CommPort"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Add the get is open method
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

    // Add the get port
    {
        TMEngMethodInfo methiNew
        (
            L"GetPort"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetPort = c2AddMethodInfo(methiNew);
    }

    // Open the port
    {
        TMEngMethodInfo methiNew
        (
            L"Open"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Port", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_Open = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"OpenCfg"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Port", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"ToSet", c2CommCfg);
        m_c2MethId_OpenCfg = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"OpenCfgBufs"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Port", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddInParm(L"ToSet", c2CommCfg);
        methiNew.c2AddInParm(L"ReadBufSz", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"WriteBufSz", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_OpenCfgBufs = c2AddMethodInfo(methiNew);
    }

    // Query the line state flags, some individually, and all together
    {
        TMEngMethodInfo methiNew
        (
            L"GetCTSOn"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetCTSOn = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetDSROn"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetDSROn = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"GetLineStates"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"CTSOn", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddOutParm(L"DSROn", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddOutParm(L"RingOn", tCIDMacroEng::EIntrinsics::Boolean);
        methiNew.c2AddOutParm(L"RLSDOn", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_GetLineStates = c2AddMethodInfo(methiNew);
    }

    // Purge the read or write buffer
    {
        TMEngMethodInfo methiNew
        (
            L"PurgeReadBuf"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_PurgeReadBuf = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"PurgeWriteBuf"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_PurgeWriteBuf = c2AddMethodInfo(methiNew);
    }

    // Read a buffer from the port
    {
        TMEngMethodInfo methiNew
        (
            L"ReadBuffer"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"MaxBytes", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ReadBuf = c2AddMethodInfo(methiNew);
    }

    // Read a byte from the port
    {
        TMEngMethodInfo methiNew
        (
            L"ReadByte"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"BufToFill", tCIDMacroEng::EIntrinsics::Card1);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_ReadByte = c2AddMethodInfo(methiNew);
    }

    // Set or clear the break state on this port
    {
        TMEngMethodInfo methiNew
        (
            L"SetBreak"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"State", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_SetBreak = c2AddMethodInfo(methiNew);
    }

    // Set a comm config value on the port
    {
        TMEngMethodInfo methiNew
        (
            L"SetCfg"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", c2CommCfg);
        m_c2MethId_SetCfg = c2AddMethodInfo(methiNew);
    }

    // Set the DTR flag
    {
        TMEngMethodInfo methiNew
        (
            L"SetDTRFlag"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", c2DTRFlags);
        m_c2MethId_SetDTRFlag = c2AddMethodInfo(methiNew);
    }

    // Set one of the port flags
    {
        TMEngMethodInfo methiNew
        (
            L"SetPortFlag"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"FlagToSet", c2PortFlags);
        m_c2MethId_SetPortFlag = c2AddMethodInfo(methiNew);
    }

    // Set the RTS flag
    {
        TMEngMethodInfo methiNew
        (
            L"SetRTSFlag"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToSet", c2RTSFlags);
        m_c2MethId_SetRTSFlag = c2AddMethodInfo(methiNew);
    }

    // Write a buffer to the port
    {
        TMEngMethodInfo methiNew
        (
            L"WriteBuffer"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"BufToWrite", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"ToWrite", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_WriteBuf = c2AddMethodInfo(methiNew);
    }

    {
        TMEngMethodInfo methiNew
        (
            L"WriteByte"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ByteToWrite", tCIDMacroEng::EIntrinsics::Card1);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_WriteByte = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngCommPortInfo::pmecvMakeStorage(const   TString&                strName
                                    ,       TCIDMacroEngine&
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngCommPortVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMacroEngCommPortInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCommPortInfo::bInvokeMethod(       TCIDMacroEngine&    meOwner
                            , const TMEngMethodInfo&    methiTarget
                            ,       TMEngClassVal&      mecvInstance)
{
    TMEngCommPortVal& mecvActual = static_cast<TMEngCommPortVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if ((c2MethId == m_c2MethId_AssertCTS)
    ||  (c2MethId == m_c2MethId_AssertDSR))
    {
        // These are the same except for the flag asserted
        try
        {
            // Set the on/off flag appropriately
            tCIDComm::EFlags eToSet;
            if (meOwner.bStackValAt(c4FirstInd))
            {
                if (c2MethId == m_c2MethId_AssertCTS)
                    eToSet = tCIDComm::EFlags::CTSOut;
                else
                    eToSet = tCIDComm::EFlags::DSROut;
            }
             else
            {
                eToSet = tCIDComm::EFlags::None;
            }

            // And the mask to indicate which flag we are affecting
            tCIDComm::EFlags eMask;
            if (c2MethId == m_c2MethId_AssertCTS)
                eMask = tCIDComm::EFlags::CTSOut;
            else
                eMask = tCIDComm::EFlags::DSROut;

            mecvActual.commValue().SetFlags(eToSet, eMask);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_SetLineStateErr, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_ClearPortFlag)
    {
        // Make sure it's open, else throw
        CheckIsOpen(meOwner, mecvActual);

        // Get the actual enum type, and use it to get the actual flag value
        TMEngEnumVal& mecvFlag = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        TMEngEnumInfo& meciEnum = meOwner.meciFindAs<TMEngEnumInfo>(mecvFlag.c2ClassId());
        tCIDComm::EFlags eToSet = tCIDComm::EFlags(meciEnum.c4MapValue(mecvFlag));

        try
        {
            mecvActual.commValue().SetFlags(eToSet, tCIDComm::EFlags::None);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_SetFlagErr, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_Close)
    {
        try
        {
            mecvActual.commValue().Close();
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_CloseFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Nothing to do at this time. The Open() method does the work
    }
     else if (c2MethId == m_c2MethId_GetIsOpen)
    {
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(mecvActual.bIsOpen());
    }
     else if ((c2MethId == m_c2MethId_GetCTSOn)
          ||  (c2MethId == m_c2MethId_GetDSROn)
          ||  (c2MethId == m_c2MethId_GetLineStates))
    {
        // Make sure it's open, else throw
        CheckIsOpen(meOwner, mecvActual);

        tCIDLib::TBoolean bCTSOn = kCIDLib::False;
        tCIDLib::TBoolean bDSROn = kCIDLib::False;
        tCIDLib::TBoolean bRingOn = kCIDLib::False;
        tCIDLib::TBoolean bRLSDOn = kCIDLib::False;
        try
        {
            mecvActual.commValue().QueryLineStates(bCTSOn, bDSROn, bRingOn, bRLSDOn);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_QueryLineStateErr, errToCatch);
        }

        if (c2MethId == m_c2MethId_GetLineStates)
        {
            // This one returns all of the states as parms
            TMEngBooleanVal& mecvCTSOn
                = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd);
            TMEngBooleanVal& mecvDSROn
                = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd + 1);
            TMEngBooleanVal& mecvRingOn
                = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd + 2);
            TMEngBooleanVal& mecvRLSDOn
                = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd + 3);

            mecvCTSOn.bValue(bCTSOn);
            mecvDSROn.bValue(bDSROn);
            mecvRingOn.bValue(bRingOn);
            mecvRLSDOn.bValue(bRLSDOn);
        }
         else if (c2MethId == m_c2MethId_GetCTSOn)
        {
            // The CTS flag as a return boolean value
            TMEngBooleanVal& mecvRet
                = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
            mecvRet.bValue(bCTSOn);
        }
         else if (c2MethId == m_c2MethId_GetDSROn)
        {
            // The DSR flag as a return boolean value
            TMEngBooleanVal& mecvRet
                = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
            mecvRet.bValue(bDSROn);
        }
    }
     else if (c2MethId == m_c2MethId_GetPort)
    {
        // Make sure it's open, else throw
        CheckIsOpen(meOwner, mecvActual);

        TMEngStringVal& mecvRet = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        mecvRet.strValue(mecvActual.commValue().strPath());
    }
     else if (c2MethId == m_c2MethId_Open)
    {
        // Make sure it's not already open
        if (mecvActual.bIsOpen())
            ThrowAnErr(meOwner, m_c4ErrId_AlreadyOpen, mecvActual.commValue().strPath());

        const TString& strPath = meOwner.strStackValAt(c4FirstInd);
        try
        {
            mecvActual.Open(strPath);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_OpenFailed, errToCatch);
        }
    }
     else if ((c2MethId == m_c2MethId_OpenCfg)
          ||  (c2MethId == m_c2MethId_OpenCfgBufs))
    {
        // Make sure it's not already open
        if (mecvActual.bIsOpen())
            ThrowAnErr(meOwner, m_c4ErrId_AlreadyOpen, mecvActual.commValue().strPath());

        //
        //  And set the buffer sizes. Default them to zero, which means let
        //  the system size them, then if this is a Bufs version, get those
        //  values and use them.
        //
        tCIDLib::TCard4 c4ReadSz = 0;
        tCIDLib::TCard4 c4WriteSz = 0;
        if (c2MethId == m_c2MethId_OpenCfgBufs)
        {
            c4ReadSz = meOwner.c4StackValAt(c4FirstInd + 2);
            c4WriteSz = meOwner.c4StackValAt(c4FirstInd + 3);
        }

        const TString& strPath = meOwner.strStackValAt(c4FirstInd);
        try
        {
            mecvActual.Open
            (
                strPath
                , meOwner.mecvStackAtAs<TMEngCommCfgVal>(c4FirstInd + 1).cpcfgValue()
                , c4ReadSz
                , c4WriteSz
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_OpenFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_PurgeReadBuf)
    {
        try
        {
            mecvActual.commValue().PurgeReadData();
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_PurgeErr, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_PurgeWriteBuf)
    {
        try
        {
            mecvActual.commValue().PurgeWriteData();
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_PurgeErr, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_ReadBuf)
    {
        // Make sure it's open, else throw
        CheckIsOpen(meOwner, mecvActual);

        TMEngCard4Val& mecvRet
                    = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        try
        {
            mecvRet.c4Value
            (
                mecvActual.commValue().c4ReadMBufMS
                (
                    meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd).mbufValue()
                    , meOwner.c4StackValAt(c4FirstInd + 1)
                    , meOwner.c4StackValAt(c4FirstInd + 2)
                )
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_ReadErr, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_ReadByte)
    {
        // Make sure it's open, else throw
        CheckIsOpen(meOwner, mecvActual);

        TMEngBooleanVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        TMEngCard1Val& mecvToFill
                    = meOwner.mecvStackAtAs<TMEngCard1Val>(c4FirstInd);

        tCIDLib::TCard1 c1Buf;
        try
        {
            const tCIDLib::TCard4 c4Res = mecvActual.commValue().c4ReadRawBufMS
            (
                &c1Buf, 1, meOwner.c4StackValAt(c4FirstInd + 1)
            );
            mecvRet.bValue(c4Res == 1);
            mecvToFill.c1Value(c1Buf);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_ReadErr, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_SetBreak)
    {
        // Make sure it's open, else throw
        CheckIsOpen(meOwner, mecvActual);

        try
        {
            mecvActual.commValue().SetBreak(meOwner.bStackValAt(c4FirstInd));
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_SetBreakErr, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_SetCfg)
    {
        // Make sure it's open, else throw
        CheckIsOpen(meOwner, mecvActual);

        try
        {
            mecvActual.commValue().SetCfg
            (
                meOwner.mecvStackAtAs<TMEngCommCfgVal>(c4FirstInd).cpcfgValue()
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_CfgFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_SetDTRFlag)
    {
        // Make sure it's open, else throw
        CheckIsOpen(meOwner, mecvActual);

        // Get the actual enum type, and use it to get the actual flag value
        TMEngEnumVal& mecvFlag = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        TMEngEnumInfo& meciEnum = meOwner.meciFindAs<TMEngEnumInfo>(mecvFlag.c2ClassId());

        try
        {
            mecvActual.commValue().SetDTR(tCIDComm::EPortDTR(meciEnum.c4MapValue(mecvFlag)));
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_SetFlagErr, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_SetPortFlag)
    {
        // Make sure it's open, else throw
        CheckIsOpen(meOwner, mecvActual);

        // Get the actual enum type, and use it to get the actual flag value
        TMEngEnumVal& mecvFlag = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        TMEngEnumInfo& meciEnum = meOwner.meciFindAs<TMEngEnumInfo>(mecvFlag.c2ClassId());
        tCIDComm::EFlags eToSet = tCIDComm::EFlags(meciEnum.c4MapValue(mecvFlag));

        try
        {
            mecvActual.commValue().SetFlags(eToSet, eToSet);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_SetFlagErr, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_SetRTSFlag)
    {
        // Make sure it's open, else throw
        CheckIsOpen(meOwner, mecvActual);

        // Get the actual enum type, and use it to get the actual flag value
        TMEngEnumVal& mecvFlag = meOwner.mecvStackAtAs<TMEngEnumVal>(c4FirstInd);
        TMEngEnumInfo& meciEnum = meOwner.meciFindAs<TMEngEnumInfo>(mecvFlag.c2ClassId());

        try
        {
            mecvActual.commValue().SetRTS(tCIDComm::EPortRTS(meciEnum.c4MapValue(mecvFlag)));
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_SetFlagErr, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_WriteBuf)
    {
        // Make sure it's open, else throw
        CheckIsOpen(meOwner, mecvActual);

        try
        {
            TMEngCard4Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
            mecvRet.c4Value
            (
                mecvActual.commValue().c4WriteMBufMS
                (
                    meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd).mbufValue()
                    , meOwner.c4StackValAt(c4FirstInd + 1)
                    , meOwner.c4StackValAt(c4FirstInd + 2)
                )
            );
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_WriteErr, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_WriteByte)
    {
        // Make sure it's open, else throw
        CheckIsOpen(meOwner, mecvActual);

        try
        {
            TMEngBooleanVal& mecvRet
                        = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);

            const tCIDLib::TCard1 c1Val = meOwner.c1StackValAt(c4FirstInd);
            const tCIDLib::TCard4 c4Count = mecvActual.commValue().c4WriteRawBufMS
            (
                &c1Val, 1, meOwner.c4StackValAt(c4FirstInd + 1)
            );
            mecvRet.bValue(c4Count == 1);
        }

        catch(TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrId_WriteErr, errToCatch);
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMacroEngCommPortInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngCommPortInfo::CheckIsOpen(         TCIDMacroEngine&    meOwner
                                , const TMEngCommPortVal&   mecvToCheck)
{
    if (!mecvToCheck.bIsOpen())
    {
        meOwner.SetException
        (
            m_c2TypeId_Errors
            , strClassPath()
            , m_c4ErrId_NotOpen
            , m_pmeciErrors->strPartialName(m_c4ErrId_NotOpen)
            , m_pmeciErrors->strTextValue(m_c4ErrId_NotOpen)
            , meOwner.c4CurLine()
        );
        throw TExceptException();
    }
}

tCIDLib::TVoid
TMEngCommPortInfo::ThrowAnErr(          TCIDMacroEngine&    meOwner
                                , const tCIDLib::TCard4     c4ToThrow
                                , const TString&            strText)
{
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
TMEngCommPortInfo::ThrowAnErr(          TCIDMacroEngine&    meOwner
                                , const tCIDLib::TCard4     c4ToThrow
                                , const MFormattable&       fmtblToken)
{
    TString strText(m_pmeciErrors->strTextValue(c4ToThrow));
    strText.eReplaceToken(fmtblToken, kCIDLib::chDigit1);

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
TMEngCommPortInfo::ThrowAnErr(          TCIDMacroEngine&    meOwner
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

