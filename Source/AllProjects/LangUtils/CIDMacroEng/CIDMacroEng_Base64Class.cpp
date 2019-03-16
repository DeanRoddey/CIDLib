//
// FILE NAME: CIDMacroEng_Base64Class.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/08/2006
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
//  This file implements the value and info classes required to do Base64
//  encoding/decoding.
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
RTTIDecls(TMEngBase64Val,TMEngClassVal)
RTTIDecls(TMEngBase64Info,TMacroEngClass)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_Base64
{
    // -----------------------------------------------------------------------
    //  The names of our classes
    // -----------------------------------------------------------------------
    const TString   strB64Name(L"Base64");
    const TString   strB64ClassPath(L"MEng.System.Runtime.Base64");

}



// ---------------------------------------------------------------------------
//  CLASS: TMEngBase64Val
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngBase64Val: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngBase64Val::TMEngBase64Val( const   TString&                strName
                                , const tCIDLib::TCard2         c2Id
                                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
{
}

TMEngBase64Val::~TMEngBase64Val()
{
}


// ---------------------------------------------------------------------------
//  TMEngBase64Val: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngBase64Val::bDbgFormat(         TTextOutStream&
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts
                            , const tCIDLib::ERadices
                            , const TCIDMacroEngine&) const
{
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TMEngBase64Val: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TBase64& TMEngBase64Val::b64Value() const
{
    return m_b64Value;
}

TBase64& TMEngBase64Val::b64Value()
{
    return m_b64Value;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngBase64Info
// PREFIX: meunit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngBase64Info: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngBase64Info::strPath()
{
    return CIDMacroEng_Base64::strB64ClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngBase64Info: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngBase64Info::TMEngBase64Info(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_Base64::strB64Name
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2EnumId_Errors(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_DecodeToBuf(kMacroEng::c2BadId)
    , m_c2MethId_EncodeBuf(kMacroEng::c2BadId)
    , m_c2MethId_EncodeStr(kMacroEng::c2BadId)
    , m_c2MethId_SetLineWidth(kMacroEng::c2BadId)
    , m_c4Err_Decode(kCIDLib::c4MaxCard)
    , m_c4Err_Encode(kCIDLib::c4MaxCard)
    , m_pmeciErrors(0)
{
}

TMEngBase64Info::~TMEngBase64Info()
{
}


// ---------------------------------------------------------------------------
//  TMEngBase64Info: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngBase64Info::Init(TCIDMacroEngine& meOwner)
{
    // An enum to represent the exceptions that this class throws
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"B64Errors"
            , strClassPath()
            , L"MEng.Enum"
            , 2
        );
        m_c4Err_Decode = m_pmeciErrors->c4AddEnumItem(L"Decode", TString::strEmpty());
        m_c4Err_Encode = m_pmeciErrors->c4AddEnumItem(L"Encode", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.Base64"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Decode to a binary buffer
    {
        TMEngMethodInfo methiNew
        (
            L"DecodeToBuf"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"InString", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"OutBuf", tCIDMacroEng::EIntrinsics::MemBuf);
        m_c2MethId_DecodeToBuf = c2AddMethodInfo(methiNew);
    }

    // Encode a binary buffer
    {
        TMEngMethodInfo methiNew
        (
            L"EncodeBuf"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"InBuf", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"InBytes", tCIDMacroEng::EIntrinsics::Card4);
        methiNew.c2AddOutParm(L"OutString", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_EncodeBuf = c2AddMethodInfo(methiNew);
    }

    // Encode a string
    {
        TMEngMethodInfo methiNew
        (
            L"EncodeStr"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"InString", tCIDMacroEng::EIntrinsics::String);
        methiNew.c2AddOutParm(L"OutString", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_EncodeStr = c2AddMethodInfo(methiNew);
    }

    // Set the encoding line width
    {
        TMEngMethodInfo methiNew
        (
            L"SetLineWidth"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Columns", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_SetLineWidth = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngBase64Info::pmecvMakeStorage( const   TString&                strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngBase64Val(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngBase64Info: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngBase64Info::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngBase64Val& mecvActual = static_cast<TMEngBase64Val&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        // Nothing to do
    }
     else if (c2MethId == m_c2MethId_DecodeToBuf)
    {
        try
        {
            // Set up the input string stream
            const TString& strSrc = meOwner.strStackValAt(c4FirstInd);
            TTextStringInStream strmSrc(&strSrc);

            // And the output binary stream
            TMemBuf& mbufTar
            (
                meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 1).mbufValue()
            );
            TBinMBufOutStream strmTar(&mbufTar);

            // Do the decode from one to the other
            mecvActual.b64Value().Decode(strmSrc, strmTar);

            // Flush the output and store the number of bytes decoded
            strmTar.Flush();
            meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1).c4Value
            (
                strmTar.c4CurSize()
            );
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4Err_Decode, errToCatch.strErrText());
        }
    }
     else if (c2MethId == m_c2MethId_EncodeBuf)
    {
        try
        {
            // Set up the input stream (they tell us how many bytes are in it)
            TBinMBufInStream strmSrc
            (
                &meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd).mbufValue()
                , meOwner.c4StackValAt(c4FirstInd + 1)
            );

            // Set up the output stream
            TMEngStringVal& mecvTar = meOwner.mecvStackAtAs<TMEngStringVal>
            (
                c4FirstInd + 2
            );
            TTextStringOutStream strmTar(&mecvTar.strValue());

            // And do the encode from one to the other
            mecvActual.b64Value().Encode(strmSrc, strmTar);
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4Err_Encode, errToCatch.strErrText());
        }
    }
     else if (c2MethId == m_c2MethId_EncodeStr)
    {
        try
        {
            // Set up the input stream
            const TString& strSrc = meOwner.strStackValAt(c4FirstInd);
            TTextStringInStream strmSrc(&strSrc);

            // Set up the output stream
            TTextStringOutStream strmTar
            (
                &meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd + 1).strValue()
            );

            // And encode from one to the other
            mecvActual.b64Value().Encode(strmSrc, strmTar);
        }

        catch(const TError& errToCatch)
        {
            ThrowAnErr(meOwner, m_c4Err_Encode, errToCatch.strErrText());
        }
    }
     else if (c2MethId == m_c2MethId_SetLineWidth)
    {
        mecvActual.b64Value().c4LineWidth(meOwner.c4StackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngBase64Info: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngBase64Info::ThrowAnErr(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            , const TString&            strThrowingClass)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strThrowingClass
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , m_pmeciErrors->strTextValue(c4ToThrow)
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

tCIDLib::TVoid
TMEngBase64Info::ThrowAnErr(        TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            , const TString&            strText
                            , const TString&            strThrowingClass)
{
    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strThrowingClass
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}


