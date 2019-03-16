//
// FILE NAME: CIDMacroEng_MD5Class.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/16/2003
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
//  This file implements the value and info classes required to create an
//  MD5 hasher class.
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
RTTIDecls(TMEngMD5HashVal,TMEngClassVal)
RTTIDecls(TMEngMD5HashInfo,TMacroEngClass)
RTTIDecls(TMEngMD5Val,TMEngClassVal)
RTTIDecls(TMEngMD5Info,TMacroEngClass)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_MD5Class
{
    // -----------------------------------------------------------------------
    //  The names of our classes
    // -----------------------------------------------------------------------
    const TString   strDigestName(L"MD5Digest");
    const TString   strDigestPath(L"MEng.System.Runtime.MD5Digest");
    const TString   strHashName(L"MD5Hash");
    const TString   strHashClassPath(L"MEng.System.Runtime.MD5Hash");
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngMD5HashVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngMD5HashVal: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngMD5HashVal::TMEngMD5HashVal(const  TString&                strName
                                , const tCIDLib::TCard2         c2Id
                                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_pmhashValue(new TMD5Hash)
{
}

TMEngMD5HashVal::~TMEngMD5HashVal()
{
    delete m_pmhashValue;
}


// ---------------------------------------------------------------------------
//  TMEngMD5HashVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngMD5HashVal::bDbgFormat(        TTextOutStream&     strmTarget
                            , const TMEngClassInfo&     meciThis
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices   eRadix
                            , const TCIDMacroEngine&    meOwner) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        strmTarget << *m_pmhashValue;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TVoid
TMEngMD5HashVal::CopyFrom(  const   TMEngClassVal&      mecvToCopy
                            ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    *m_pmhashValue = *static_cast<const TMEngMD5HashVal&>(mecvToCopy).m_pmhashValue;
}


// ---------------------------------------------------------------------------
//  TMEngMD5HashVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TMD5Hash& TMEngMD5HashVal::mhashValue() const
{
    return *m_pmhashValue;
}

TMD5Hash& TMEngMD5HashVal::mhashValue()
{
    return *m_pmhashValue;
}


tCIDLib::TVoid TMEngMD5HashVal::Reset()
{
    m_pmhashValue->Zero();
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngMD5HashInfo
// PREFIX: meunit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngMD5HashInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngMD5HashInfo::strPath()
{
    return CIDMacroEng_MD5Class::strHashClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngMD5HashInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngMD5HashInfo::TMEngMD5HashInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_MD5Class::strHashName
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Formattable"
    )
    , m_c2MethId_CopyToMemBuf(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_Equal(kMacroEng::c2BadId)
    , m_c2MethId_GetAsString(kMacroEng::c2BadId)
    , m_c2MethId_GetByteCount(kMacroEng::c2BadId)
    , m_c2MethId_Reset(kMacroEng::c2BadId)
{
}

TMEngMD5HashInfo::~TMEngMD5HashInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngMD5HashInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngMD5HashInfo::Init(TCIDMacroEngine& meOwner)
{
    // Copy the bytes to a memory buffer
    {
        TMEngMethodInfo methiNew
        (
            L"CopyToMemBuf"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddOutParm(L"ToFill", tCIDMacroEng::EIntrinsics::MemBuf);
        m_c2MethId_CopyToMemBuf = c2AddMethodInfo(methiNew);
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.MD5Hash"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Check equality
    {
        TMEngMethodInfo methiNew
        (
            L"Equal"
            , tCIDMacroEng::EIntrinsics::Boolean
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        methiNew.c2AddInParm(L"ToComp", c2Id());
        m_c2MethId_Equal = c2AddMethodInfo(methiNew);
    }

    // Get the count of bytes in the hash
    {
        TMEngMethodInfo methiNew
        (
            L"GetAsString"
            , tCIDMacroEng::EIntrinsics::String
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetAsString = c2AddMethodInfo(methiNew);
    }

    // Get the count of bytes in the hash
    {
        TMEngMethodInfo methiNew
        (
            L"GetByteCount"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
            , tCIDMacroEng::EConstTypes::Const
        );
        m_c2MethId_GetByteCount = c2AddMethodInfo(methiNew);
    }

    // Reset the hash bytes to zero
    {
        TMEngMethodInfo methiNew
        (
            L"Reset"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        m_c2MethId_Reset = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngMD5HashInfo::pmecvMakeStorage( const   TString&                strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngMD5HashVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngMD5HashInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngMD5HashInfo::bInvokeMethod(        TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngMD5HashVal& mecvActual = static_cast<TMEngMD5HashVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_CopyToMemBuf)
    {
        // Get the target memory buffer and return value
        TMEngCard4Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        TMEngMemBufVal& mecvToFill
                        = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd);

        mecvRet.c4Value(mecvActual.mhashValue().c4Bytes());
        mecvToFill.mbufValue().CopyIn
        (
            mecvActual.mhashValue().pc1Buffer(), mecvRet.c4Value()
        );
    }
     else if (c2MethId == m_c2MethId_DefCtor)
    {
        // Just do a reset on it
        mecvActual.Reset();
    }
     else if (c2MethId == m_c2MethId_Equal)
    {
        TMEngBooleanVal& mecvRet
                = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        const TMEngMD5HashVal& mecvComp = meOwner.mecvStackAtAs<TMEngMD5HashVal>(c4FirstInd);

        mecvRet.bValue(mecvActual.mhashValue() == mecvComp.mhashValue());
    }
     else if (c2MethId == TMEngFormattableInfo::c2FormatToId())
    {
        //
        //  The parm is a text stream object. In our case, we can kind of
        //  cheat and just call to it directly.
        //
        TMEngTextOutStreamVal& mecvTarget
                = meOwner.mecvStackAtAs<TMEngTextOutStreamVal>(c4FirstInd);
        mecvTarget.strmTarget(meOwner) << mecvActual.mhashValue();
    }
     else if (c2MethId == m_c2MethId_GetAsString)
    {
        TMEngStringVal& mecvRet
                    = meOwner.mecvStackAtAs<TMEngStringVal>(c4FirstInd - 1);
        mecvActual.mhashValue().FormatToStr(mecvRet.strValue());
    }
     else if (c2MethId == m_c2MethId_GetByteCount)
    {
        TMEngCard4Val& mecvRet
                        = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.mhashValue().c4Bytes());
    }
     else if (c2MethId == m_c2MethId_Reset)
    {
        mecvActual.Reset();
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngMD5Val
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngMD5Val: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngMD5Val::TMEngMD5Val(const  TString&                strName
                        , const tCIDLib::TCard2         c2Id
                        , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_pmdigValue(new TMessageDigest5)
{
}

TMEngMD5Val::~TMEngMD5Val()
{
    delete m_pmdigValue;
}


// ---------------------------------------------------------------------------
//  TMEngMD5Val: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TMessageDigest5& TMEngMD5Val::mdigValue() const
{
    return *m_pmdigValue;
}

TMessageDigest5& TMEngMD5Val::mdigValue()
{
    return *m_pmdigValue;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngMD5Info
// PREFIX: meunit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngMD5Info: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngMD5Info::strPath()
{
    return CIDMacroEng_MD5Class::strDigestPath;
}


// ---------------------------------------------------------------------------
//  TMEngMD5Info: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngMD5Info::TMEngMD5Info(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_MD5Class::strDigestName
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_DigestBuf(kMacroEng::c2BadId)
    , m_c2MethId_DigestStr(kMacroEng::c2BadId)
{
    //
    //  Add the hash class as an import of ours, so that it will get loaded
    //  if we do, and before we are initialized.
    //
    bAddClassImport(CIDMacroEng_MD5Class::strHashClassPath);
}

TMEngMD5Info::~TMEngMD5Info()
{
}


// ---------------------------------------------------------------------------
//  TMEngMD5Info: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngMD5Info::Init(TCIDMacroEngine& meOwner)
{
    // Look up some other classes that we need as parameters below
    const tCIDLib::TCard2 c2HashId = meOwner.c2FindClassId(CIDMacroEng_MD5Class::strHashClassPath);

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.MD5Digest"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Digest bytes from a memory buffer
    {
        TMEngMethodInfo methiNew
        (
            L"DigestBuf"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"ToFill", c2HashId);
        methiNew.c2AddInParm(L"SrcBuf", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"ByteCount", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_DigestBuf = c2AddMethodInfo(methiNew);
    }

    // Digest chars from a string
    {
        TMEngMethodInfo methiNew
        (
            L"DigestStr"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddOutParm(L"ToFill", c2HashId);
        methiNew.c2AddInParm(L"SrcStr", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_DigestStr = c2AddMethodInfo(methiNew);
    }
}


TMEngClassVal*
TMEngMD5Info::pmecvMakeStorage( const   TString&                strName
                                ,       TCIDMacroEngine&        meOwner
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngMD5Val(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngMD5Info: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngMD5Info::bInvokeMethod(        TCIDMacroEngine&    meOwner
                            , const TMEngMethodInfo&    methiTarget
                            ,       TMEngClassVal&      mecvInstance)
{
    TMEngMD5Val& mecvActual = static_cast<TMEngMD5Val&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        // Nothing to do in this case
    }
     else if (c2MethId == m_c2MethId_DigestBuf)
    {
        // Get the output hash object
        TMEngMD5HashVal& mecvOut
                        = meOwner.mecvStackAtAs<TMEngMD5HashVal>(c4FirstInd);
        TMEngMemBufVal& mecvBuf = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 1);

        mecvActual.mdigValue().StartNew();
        mecvActual.mdigValue().DigestBuf
        (
            mecvBuf.mbufValue(), meOwner.c4StackValAt(c4FirstInd + 2)
        );
        mecvActual.mdigValue().Complete(mecvOut.mhashValue());
    }
     else if (c2MethId == m_c2MethId_DigestStr)
    {
        // Get the output hash object
        TMEngMD5HashVal& mecvOut
                        = meOwner.mecvStackAtAs<TMEngMD5HashVal>(c4FirstInd);

        mecvActual.mdigValue().StartNew();
        mecvActual.mdigValue().DigestStr(meOwner.strStackValAt(c4FirstInd + 1));
        mecvActual.mdigValue().Complete(mecvOut.mhashValue());
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


