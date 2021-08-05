//
// FILE NAME: CIDMacroEng_USBHIDClass.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/11/2005
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
//  value classes, which implement the fundamental MEng.USBHID macro class.
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
RTTIDecls(TMEngUSBHIDVal,TMEngClassVal)
RTTIDecls(TMEngUSBHIDInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDMacroEng_USBHIDClasses
    {
        // -----------------------------------------------------------------------
        //  The names for the types that we support here. Each derivative has to
        //  be able to return strings that contain its name and full name.
        // -----------------------------------------------------------------------
        const TString   strUSBHID(L"USBHID");
        const TString   strUSBHIDClassPath(L"MEng.System.Runtime.USBHID");
    }
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngUSBHIDVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngUSBHIDVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngUSBHIDVal::TMEngUSBHIDVal( const   TString&                strName
                                , const tCIDLib::TCard2         c2Id
                                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_c2ProductId(0)
    , m_c2VendorId(0)
{
}

TMEngUSBHIDVal::~TMEngUSBHIDVal()
{
    // If they didn't close it, then we need to
    try
    {
        if (m_flDevice.bIsOpen())
            m_flDevice.Close();
    }

    catch(TError& errToCatch)
    {
        // If in verbose mode we'll log the C++ error
        if (!errToCatch.bLogged() && facCIDMacroEng().bLogWarnings())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  TMEngUSBHIDVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngUSBHIDVal::bDbgFormat(         TTextOutStream&     strmTarget
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices
                            , const TCIDMacroEngine&    ) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Short)
    {
        if (m_flDevice.bIsOpen())
            strmTarget << L"[Device is open]";
        else
            strmTarget << L"[Device is closed]";
        return kCIDLib::True;
    }
     else if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        if (m_flDevice.bIsOpen())
        {
            strmTarget  << L"Vendor Id = 0x"
                        << TTextOutStream::strmfHex4() << m_c2VendorId
                        << TTextOutStream::strmfDefault()
                        << L"\nProduct Id = 0x"
                        << TTextOutStream::strmfHex4() << m_c2ProductId
                        << TTextOutStream::strmfDefault();
        }
    }
    return kCIDLib::False;
}



// ---------------------------------------------------------------------------
//  TMEngUSBHIDVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngUSBHIDVal::bFindDevPath(const  tCIDLib::TCard2 c2VendorId
                            , const tCIDLib::TCard2 c2ProductId
                            ,       TString&        strToFill)
{
    // Try to find a device with the indicated ids
    if (TUSBDev::bFindHIDDev(c2VendorId, c2ProductId, strToFill))
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean
TMEngUSBHIDVal::bReadBytes(         TMemBuf&            mbufToFill
                            , const tCIDLib::TCard4     c4ToRead
                            , const tCIDLib::TCard4     c4WaitFor
                            , const tCIDLib::TBoolean   bThrowIfNot)
{
    return m_flDevice.c4ReadBufferTO
    (
        mbufToFill.pc1Data()
        , c4ToRead
        , c4WaitFor
        , m_evIO
        , bThrowIfNot
    ) == 9;
}


tCIDLib::TVoid
TMEngUSBHIDVal::Open(const  TString&        strDevPath
                    , const tCIDLib::TCard2 c2VendorId
                    , const tCIDLib::TCard2 c2ProductId)
{
    m_flDevice.strName(strDevPath);
    m_flDevice.Open
    (
        tCIDLib::EAccessModes::ReadWrite
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::USBDevice
    );

    // It worked, so set the ids
    m_c2VendorId = c2VendorId;
    m_c2ProductId = c2ProductId;
}


tCIDLib::TVoid TMEngUSBHIDVal::Close()
{
    // If the device is open, then close it
    if (m_flDevice.bIsOpen())
        m_flDevice.Close();
}


tCIDLib::TVoid TMEngUSBHIDVal::Reset()
{
    // Shouldn't be possible, but close it just in case
    if (m_flDevice.bIsOpen())
        m_flDevice.Close();
}


tCIDLib::TVoid
TMEngUSBHIDVal::WriteBytes( const   TMemBuf&        mbufToSend
                            , const tCIDLib::TCard4 c4ToSend
                            , const tCIDLib::TCard4 c4WaitFor)
{
    m_flDevice.c4WriteBufferTO
    (
        mbufToSend.pc1Data()
        , c4ToSend
        , c4WaitFor
        , m_evIO
    );
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngUSBHIDInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngUSBHIDInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngUSBHIDInfo::strPath()
{
    return CIDMacroEng_USBHIDClasses::strUSBHIDClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngUSBHIDInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngUSBHIDInfo::TMEngUSBHIDInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_USBHIDClasses::strUSBHID
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2EnumId_Errors(kCIDMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_Close(kCIDMacroEng::c2BadId)
    , m_c2MethId_FindDevice(kCIDMacroEng::c2BadId)
    , m_c2MethId_Open(kCIDMacroEng::c2BadId)
    , m_c2MethId_Read(kCIDMacroEng::c2BadId)
    , m_c2MethId_Write(kCIDMacroEng::c2BadId)
    , m_c4ErrCloseFailed(kCIDLib::c4MaxCard)
    , m_c4ErrDevNotFound(kCIDLib::c4MaxCard)
    , m_c4ErrOpenFailed(kCIDLib::c4MaxCard)
    , m_c4ErrReadFailed(kCIDLib::c4MaxCard)
    , m_c4ErrWriteFailed(kCIDLib::c4MaxCard)
    , m_pmeciErrors(nullptr)
{
}

TMEngUSBHIDInfo::~TMEngUSBHIDInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngUSBHIDInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngUSBHIDInfo::Init(TCIDMacroEngine& meOwner)
{
    //
    //  We need to add some nested enum types. First we do one for the days
    //  of the week, months, and then for errors.
    //
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"USBHIDErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 5
        );
        m_c4ErrCloseFailed = m_pmeciErrors->c4AddEnumItem(L"CloseFailed", TString::strEmpty());
        m_c4ErrDevNotFound = m_pmeciErrors->c4AddEnumItem(L"DevNotFound", L"No device with ids %(1)/%(2) was found");
        m_c4ErrOpenFailed = m_pmeciErrors->c4AddEnumItem(L"OpenFailed", TString::strEmpty());
        m_c4ErrReadFailed = m_pmeciErrors->c4AddEnumItem(L"ReadFailed", TString::strEmpty());
        m_c4ErrWriteFailed = m_pmeciErrors->c4AddEnumItem(L"WriteFailed", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.USBHID"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Close the device if open
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

    // Close the device if a device is present
    {
        TMEngMethodInfo methiNew
        (
            L"FindDevice"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"VendorId", tCIDMacroEng::EIntrinsics::Card2);
        methiNew.c2AddInParm(L"ProductId", tCIDMacroEng::EIntrinsics::Card2);
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_FindDevice = c2AddMethodInfo(methiNew);
    }

    // Open the device based on vendor/product id
    {
        TMEngMethodInfo methiNew
        (
            L"Open"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"VendorId", tCIDMacroEng::EIntrinsics::Card2);
        methiNew.c2AddInParm(L"ProductId", tCIDMacroEng::EIntrinsics::Card2);
        m_c2MethId_Open = c2AddMethodInfo(methiNew);
    }

    // Read bytes from the device
    {
        TMEngMethodInfo methiNew
        (
            L"Read"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"ThrowIfNot", tCIDMacroEng::EIntrinsics::Boolean);
        m_c2MethId_Read = c2AddMethodInfo(methiNew);
    }

    // Write bytes to the device
    {
        TMEngMethodInfo methiNew
        (
            L"Write"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"ToWrite", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"Count", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddInParm(L"WaitFor", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_Write = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngUSBHIDInfo::pmecvMakeStorage(const  TString&                strName
                                 ,       TCIDMacroEngine&        meOwner
                                 , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngUSBHIDVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngUSBHIDInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngUSBHIDInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngUSBHIDVal& mecvActual = static_cast<TMEngUSBHIDVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        try
        {
            mecvActual.Reset();
        }

        catch(TError& errToCatch)
        {
            // If in verbose mode we'll log the C++ error
            if (!errToCatch.bLogged() && facCIDMacroEng().bLogWarnings())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
            ThrowAnErr(meOwner, m_c4ErrCloseFailed, errToCatch.strErrText());
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
            // If in verbose mode we'll log the C++ error
            if (!errToCatch.bLogged() && facCIDMacroEng().bLogWarnings())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
            ThrowAnErr(meOwner, m_c4ErrCloseFailed, errToCatch.strErrText());
        }
    }
     else if (c2MethId == m_c2MethId_FindDevice)
    {
        const tCIDLib::TCard2 c2VendorId = meOwner.c2StackValAt(c4FirstInd);
        const tCIDLib::TCard2 c2ProductId = meOwner.c2StackValAt(c4FirstInd + 1);
        TMEngStringVal& mecvPath = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 2);
        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue
        (
            mecvActual.bFindDevPath(c2VendorId, c2ProductId, mecvPath.strValue())
        );
    }
     else if (c2MethId == m_c2MethId_Open)
    {
        // First see if there is such a device
        TString strPath;
        const tCIDLib::TCard2 c2VendorId = meOwner.c2StackValAt(c4FirstInd);
        const tCIDLib::TCard2 c2ProductId = meOwner.c2StackValAt(c4FirstInd + 1);
        if (!mecvActual.bFindDevPath(c2VendorId, c2ProductId, strPath))
        {
            ThrowAnErr
            (
                meOwner
                , m_c4ErrDevNotFound
                , TCardinal(c2VendorId)
                , TCardinal(c2ProductId)
            );
        }

        try
        {
            mecvActual.Open(strPath, c2VendorId, c2ProductId);
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrOpenFailed, errToCatch.strErrText());
        }
    }
     else if (c2MethId == m_c2MethId_Read)
    {
        tCIDLib::TBoolean bRet = kCIDLib::False;
        try
        {
            bRet = mecvActual.bReadBytes
            (
                meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd).mbufValue()
                , meOwner.c4StackValAt(c4FirstInd + 1)
                , meOwner.c4StackValAt(c4FirstInd + 2)
                , meOwner.bStackValAt(c4FirstInd + 3)
            );
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrReadFailed, errToCatch.strErrText());
        }

        TMEngBooleanVal& mecvRet = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        mecvRet.bValue(bRet);
    }
     else if (c2MethId == m_c2MethId_Write)
    {
        try
        {
            mecvActual.WriteBytes
            (
                meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd).mbufValue()
                , meOwner.c4StackValAt(c4FirstInd + 1)
                , meOwner.c4StackValAt(c4FirstInd + 2)
            );
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4ErrWriteFailed, errToCatch.strErrText());
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngUSBHIDInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngUSBHIDInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            , const TString&            strText)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
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
TMEngUSBHIDInfo::ThrowAnErr(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            , const MFormattable&       fmtblToken1
                            , const MFormattable&       fmtblToken2)
{
    TString strText(m_pmeciErrors->strTextValue(c4ToThrow));
    strText.eReplaceToken(fmtblToken1, kCIDLib::chDigit1);
    strText.eReplaceToken(fmtblToken2, kCIDLib::chDigit2);

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}


