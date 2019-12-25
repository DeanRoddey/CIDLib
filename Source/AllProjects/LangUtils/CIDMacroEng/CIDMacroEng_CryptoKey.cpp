//
// FILE NAME: CIDMacroEng_CryptoKey.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/08/2006
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
//  This file implements the value and info classes required to create a
//  crypto key CML class.
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
RTTIDecls(TMEngCryptoKeyVal,TMEngClassVal)
RTTIDecls(TMEngCryptoKeyInfo,TMacroEngClass)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_CryptoKey
{
    // -----------------------------------------------------------------------
    //  The names of our classes
    // -----------------------------------------------------------------------
    const TString   strKeyName(L"CryptoKey");
    const TString   strKeyClassPath(L"MEng.System.Runtime.CryptoKey");

}



// ---------------------------------------------------------------------------
//  CLASS: TMEngCryptoKeyVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCryptoKeyVal: Constuctors and Destructor
// ---------------------------------------------------------------------------
TMEngCryptoKeyVal::TMEngCryptoKeyVal(const  TString&                strName
                                    , const tCIDLib::TCard2         c2Id
                                    , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_pckeyValue(new TCryptoKey)
{
}

TMEngCryptoKeyVal::~TMEngCryptoKeyVal()
{
    delete m_pckeyValue;
}


// ---------------------------------------------------------------------------
//  TMEngCryptoKeyVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCryptoKeyVal::bDbgFormat(          TTextOutStream&     strmTarget
                                , const TMEngClassInfo&
                                , const tCIDMacroEng::EDbgFmts eFormat
                                , const tCIDLib::ERadices   eRadix
                                , const TCIDMacroEngine&    ) const
{
    // Save the stream state and set up for hex byte format
    TStreamJanitor janTarget(&strmTarget);
    strmTarget.c4Width(2);
    strmTarget.chFill(kCIDLib::chDigit0);
    strmTarget.eJustification(tCIDLib::EHJustify::Right);

    tCIDLib::TCard4 c4Count = m_pckeyValue->c4Bytes();
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        strmTarget << L"Key Size=" << TCardinal(c4Count, eRadix)
                   << L"\nKey Bytes:\n";

        const tCIDLib::TCard1* pc1Data = m_pckeyValue->pc1Buffer();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            if (!(c4Index % 8))
                strmTarget << kCIDLib::NewLn << TTextOutStream::Spaces(4);
            strmTarget  << TCardinal(pc1Data[c4Index], eRadix)
                        << TTextOutStream::Spaces(1);
        }
        return kCIDLib::True;
    }
     else
    {
        const tCIDLib::TCard1* pc1Data = m_pckeyValue->pc1Buffer();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            strmTarget << TCardinal(pc1Data[c4Index], eRadix) << TTextOutStream::Spaces(1);
    }
    return kCIDLib::False;
}


tCIDLib::TVoid
TMEngCryptoKeyVal::CopyFrom(  const   TMEngClassVal&      mecvToCopy
                            ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
        meOwner.CheckSameClasses(*this, mecvToCopy);
    *m_pckeyValue = *static_cast<const TMEngCryptoKeyVal&>(mecvToCopy).m_pckeyValue;
}


// ---------------------------------------------------------------------------
//  TMEngCryptoKeyVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TCryptoKey& TMEngCryptoKeyVal::ckeyValue() const
{
    return *m_pckeyValue;
}

TCryptoKey& TMEngCryptoKeyVal::ckeyValue()
{
    return *m_pckeyValue;
}


tCIDLib::TVoid TMEngCryptoKeyVal::Reset()
{
    m_pckeyValue->Reset();
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngCryptoKeyInfo
// PREFIX: meunit
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCryptoKeyInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngCryptoKeyInfo::strPath()
{
    return CIDMacroEng_CryptoKey::strKeyClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngCryptoKeyInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCryptoKeyInfo::TMEngCryptoKeyInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_CryptoKey::strKeyName
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_Equal(kCIDMacroEng::c2BadId)
    , m_c2MethId_FromHash(kCIDMacroEng::c2BadId)
    , m_c2MethId_FromString(kCIDMacroEng::c2BadId)
    , m_c2MethId_HashCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_Reset(kCIDMacroEng::c2BadId)
    , m_c2MethId_StrCtor(kCIDMacroEng::c2BadId)
{
    // Import the MD5 hash class
    bAddClassImport(TMEngMD5HashInfo::strPath());
}

TMEngCryptoKeyInfo::~TMEngCryptoKeyInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngCryptoKeyInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngCryptoKeyInfo::Init(TCIDMacroEngine& meOwner)
{
    // Look up the MD5 hash clash which we need in some parms
    const tCIDLib::TCard2 c2MD5Hash = meOwner.c2FindClassId(TMEngMD5HashInfo::strPath());

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.System.Runtime.CryptoKey"
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

    // Set the key from a hash
    {
        TMEngMethodInfo methiNew
        (
            L"FromHash"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"SrcHash", c2MD5Hash);
        m_c2MethId_FromHash = c2AddMethodInfo(methiNew);
    }

    // Set the key from a string
    {
        TMEngMethodInfo methiNew
        (
            L"FromString"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"SrcString", tCIDMacroEng::EIntrinsics::String);
        m_c2MethId_FromString = c2AddMethodInfo(methiNew);
    }

    // Reset the key
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
TMEngCryptoKeyInfo::pmecvMakeStorage( const   TString&                strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngCryptoKeyVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngCryptoKeyInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCryptoKeyInfo::bInvokeMethod(        TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngCryptoKeyVal& mecvActual = static_cast<TMEngCryptoKeyVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
        // Just do a reset on it
        mecvActual.Reset();
    }
     else if (c2MethId == m_c2MethId_Equal)
    {
        TMEngBooleanVal& mecvRet
                = meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1);
        const TMEngCryptoKeyVal& mecvComp = meOwner.mecvStackAtAs<TMEngCryptoKeyVal>(c4FirstInd);
        mecvRet.bValue(mecvActual.ckeyValue() == mecvComp.ckeyValue());
    }
     else if (c2MethId == m_c2MethId_FromHash)
    {
        const TMEngMD5HashVal& mecvToSet
                    = meOwner.mecvStackAtAs<TMEngMD5HashVal>(c4FirstInd);
        mecvActual.ckeyValue().Set(mecvToSet.mhashValue());
    }
     else if (c2MethId == m_c2MethId_FromString)
    {
        mecvActual.ckeyValue().Set(meOwner.strStackValAt(c4FirstInd));
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


