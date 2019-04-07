//
// FILE NAME: CIDMacroEng_SHA1Class.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/23/2009
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
//  This file implements the value and info classes required to create an
//  SHA1 hasher class.
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
RTTIDecls(TMEngSHA1HashVal,TMEngClassVal)
RTTIDecls(TMEngSHA1HashInfo,TMacroEngClass)
RTTIDecls(TMEngSHA1Val,TMEngClassVal)
RTTIDecls(TMEngSHA1Info,TMacroEngClass)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_SHA1Class
{
    // -----------------------------------------------------------------------
    //  The names of our classes
    // -----------------------------------------------------------------------
    const TString   strDigestName(L"SHA1Digest");
    const TString   strDigestPath(L"MEng.System.Runtime.SHA1Digest");
    const TString   strHashName(L"SHA1Hash");
    const TString   strHashClassPath(L"MEng.System.Runtime.SHA1Hash");
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngSHA1HashVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngSHA1HashVal: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngSHA1HashVal::TMEngSHA1HashVal( const   TString&                strName
                                    , const tCIDLib::TCard2         c2Id
                                    , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_pmhashValue(new TSHA1Hash)
{
}

TMEngSHA1HashVal::~TMEngSHA1HashVal()
{
    delete m_pmhashValue;
}


// ---------------------------------------------------------------------------
//  TMEngSHA1HashVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngSHA1HashVal::bDbgFormat(       TTextOutStream&     strmTarget
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
TMEngSHA1HashVal::CopyFrom(  const   TMEngClassVal&      mecvToCopy
                            ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    *m_pmhashValue = *static_cast<const TMEngSHA1HashVal&>(mecvToCopy).m_pmhashValue;
}


// ---------------------------------------------------------------------------
//  TMEngSHA1HashVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TSHA1Hash& TMEngSHA1HashVal::mhashValue() const
{
    return *m_pmhashValue;
}

TSHA1Hash& TMEngSHA1HashVal::mhashValue()
{
    return *m_pmhashValue;
}


tCIDLib::TVoid TMEngSHA1HashVal::Reset()
{
    m_pmhashValue->Zero();
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngSHA1HashInfo
// PREFIX: meunit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngSHA1HashInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngSHA1HashInfo::strPath()
{
    return CIDMacroEng_SHA1Class::strHashClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngSHA1HashInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngSHA1HashInfo::TMEngSHA1HashInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_SHA1Class::strHashName
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

TMEngSHA1HashInfo::~TMEngSHA1HashInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngSHA1HashInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngSHA1HashInfo::Init(TCIDMacroEngine& meOwner)
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
            L"ctor1_MEng.System.Runtime.SHA1Hash"
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
TMEngSHA1HashInfo::pmecvMakeStorage( const   TString&               strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngSHA1HashVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngSHA1HashInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngSHA1HashInfo::bInvokeMethod(        TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngSHA1HashVal& mecvActual = static_cast<TMEngSHA1HashVal&>(mecvInstance);
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
        const TMEngSHA1HashVal& mecvComp = meOwner.mecvStackAtAs<TMEngSHA1HashVal>(c4FirstInd);

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
//  CLASS: TMEngSHA1Val
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngSHA1Val: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngSHA1Val::TMEngSHA1Val( const   TString&                strName
                            , const tCIDLib::TCard2         c2Id
                            , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_pmdigValue(new TSHA1Hasher)
{
}

TMEngSHA1Val::~TMEngSHA1Val()
{
    delete m_pmdigValue;
}


// ---------------------------------------------------------------------------
//  TMEngSHA1Val: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TSHA1Hasher& TMEngSHA1Val::mdigValue() const
{
    return *m_pmdigValue;
}

TSHA1Hasher& TMEngSHA1Val::mdigValue()
{
    return *m_pmdigValue;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngSHA1Info
// PREFIX: meunit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngSHA1Info: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngSHA1Info::strPath()
{
    return CIDMacroEng_SHA1Class::strDigestPath;
}


// ---------------------------------------------------------------------------
//  TMEngSHA1Info: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngSHA1Info::TMEngSHA1Info(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_SHA1Class::strDigestName
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
    bAddClassImport(CIDMacroEng_SHA1Class::strHashClassPath);
}

TMEngSHA1Info::~TMEngSHA1Info()
{
}


// ---------------------------------------------------------------------------
//  TMEngSHA1Info: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngSHA1Info::Init(TCIDMacroEngine& meOwner)
{
    // Look up some other classes that we need as parameters below
    const tCIDLib::TCard2 c2HashId = meOwner.c2FindClassId(CIDMacroEng_SHA1Class::strHashClassPath);

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.SHA1Digest"
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
TMEngSHA1Info::pmecvMakeStorage(const   TString&                strName
                                ,       TCIDMacroEngine&        meOwner
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngSHA1Val(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngSHA1Info: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngSHA1Info::bInvokeMethod(       TCIDMacroEngine&    meOwner
                            , const TMEngMethodInfo&    methiTarget
                            ,       TMEngClassVal&      mecvInstance)
{
    TMEngSHA1Val& mecvActual = static_cast<TMEngSHA1Val&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        // Nothing to do in this case
    }
     else if (c2MethId == m_c2MethId_DigestBuf)
    {
        // Get the output hash object
        TMEngSHA1HashVal& mecvOut
                        = meOwner.mecvStackAtAs<TMEngSHA1HashVal>(c4FirstInd);
        TMEngMemBufVal& mecvBuf = meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 1);

        mecvActual.mdigValue().StartNew();
        mecvActual.mdigValue().DigestBuf
        (
            mecvBuf.mbufValue()
            , meOwner.c4StackValAt(c4FirstInd + 2)
        );
        mecvActual.mdigValue().Complete(mecvOut.mhashValue());
    }
     else if (c2MethId == m_c2MethId_DigestStr)
    {
        // Get the output hash object
        TMEngSHA1HashVal& mecvOut
                        = meOwner.mecvStackAtAs<TMEngSHA1HashVal>(c4FirstInd);

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


