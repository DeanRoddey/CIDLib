//
// FILE NAME: CIDMacroEng_ZLib.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/31/2021
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2021
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
//  value classes, which wrap the underlying C++ ZLib interface.
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
#include    "CIDZLib.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMEngZLibVal,TMEngClassVal)
RTTIDecls(TMEngZLibInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDMacroEng_ZLibClasses
    {
        // -----------------------------------------------------------------------
        //  The names for the types that we support here. Each derivative has to
        //  be able to return strings that contain its name and full name.
        // -----------------------------------------------------------------------
        const TString   strZLib(L"ZLib");
        const TString   strZLibClassPath(L"MEng.System.Runtime.ZLib");
    }
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngZLibVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngZLibVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngZLibVal::TMEngZLibVal( const   TString&                    strName
                            , const tCIDLib::TCard2             c2Id
                            , const tCIDMacroEng::EConstTypes   eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_pzlibImpl(nullptr)
{
    m_pzlibImpl = new TZLibCompressor;
}

TMEngZLibVal::~TMEngZLibVal()
{
    delete m_pzlibImpl;
}


// ---------------------------------------------------------------------------
//  TMEngZLibVal: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TCard4
TMEngZLibVal::c4Compress(const  TMemBuf&            mbufSrc
                        ,       TMemBuf&            mbufTar
                        , const tCIDLib::TCard4     c4SrcCnt)
{
    TBinMBufInStream strmSrc(&mbufSrc, c4SrcCnt);
    TBinMBufOutStream strmTar(&mbufTar);
    return m_pzlibImpl->c4Compress(strmSrc, strmTar);
}


tCIDLib::TCard4
TMEngZLibVal::c4Decompress( const   TMemBuf&            mbufSrc
                            ,       TMemBuf&            mbufTar
                            , const tCIDLib::TCard4     c4SrcCnt)
{
    TBinMBufInStream strmSrc(&mbufSrc, c4SrcCnt);
    TBinMBufOutStream strmTar(&mbufTar);
    return m_pzlibImpl->c4Decompress(strmSrc, strmTar);
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngZLibInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngZLibInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngZLibInfo::strPath()
{
    return CIDMacroEng_ZLibClasses::strZLibClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngZLibInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngZLibInfo::TMEngZLibInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_ZLibClasses::strZLib
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::False
        , tCIDMacroEng::EClassExt::Final
        , L"MEng.Object"
    )
    , m_c2EnumId_Errors(kCIDMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kCIDMacroEng::c2BadId)
    , m_c2MethId_Compress(kCIDMacroEng::c2BadId)
    , m_c2MethId_Decompress(kCIDMacroEng::c2BadId)
    , m_c4ErrCompFailed(kCIDLib::c4MaxCard)
    , m_c4ErrDecompFailed(kCIDLib::c4MaxCard)
    , m_pmeciErrors(nullptr)
{
}

TMEngZLibInfo::~TMEngZLibInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngZLibInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngZLibInfo::Init(TCIDMacroEngine& meOwner)
{
    //
    //  We need to add some nested enum types. First we do one for the days
    //  of the week, months, and then for errors.
    //
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"ZLibErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 5
        );
        m_c4ErrCompFailed = m_pmeciErrors->c4AddEnumItem(L"CompFailed", TString::strEmpty());
        m_c4ErrDecompFailed = m_pmeciErrors->c4AddEnumItem(L"DecompFailed", TString::strEmpty());
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2EnumId_Errors = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }

    // Add the default constructor
    {
        TMEngMethodInfo methiNew
        (
            L"ctor1_MEng.ZLib"
            , tCIDMacroEng::EIntrinsics::Void
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.bIsCtor(kCIDLib::True);
        m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
    }

    // Compress a buffer to another buffer
    {
        TMEngMethodInfo methiNew
        (
            L"Compress"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Src", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddOutParm(L"Tar", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"SrcCount", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_Compress = c2AddMethodInfo(methiNew);
    }

    // Deompress a buffer to another buffer
    {
        TMEngMethodInfo methiNew
        (
            L"Decompress"
            , tCIDMacroEng::EIntrinsics::Card4
            , tCIDMacroEng::EVisTypes::Public
            , tCIDMacroEng::EMethExt::Final
        );
        methiNew.c2AddInParm(L"Src", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddOutParm(L"Tar", tCIDMacroEng::EIntrinsics::MemBuf);
        methiNew.c2AddInParm(L"SrcCount", tCIDMacroEng::EIntrinsics::Card4);
        m_c2MethId_Decompress = c2AddMethodInfo(methiNew);
    }

}


TMEngClassVal*
TMEngZLibInfo::pmecvMakeStorage(const   TString&                    strName
                                ,       TCIDMacroEngine&            meOwner
                                , const tCIDMacroEng::EConstTypes   eConst) const
{
    return new TMEngZLibVal(strName, c2Id(), eConst);
}


// ---------------------------------------------------------------------------
//  TMEngZLibInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngZLibInfo::bInvokeMethod(       TCIDMacroEngine&    meOwner
                            , const TMEngMethodInfo&    methiTarget
                            ,       TMEngClassVal&      mecvInstance)
{
    TMEngZLibVal& mecvActual = static_cast<TMEngZLibVal&>(mecvInstance);
    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_DefCtor)
    {
    }
     else if (c2MethId == m_c2MethId_Compress)
    {
        try
        {
            const tCIDLib::TCard4 c4Ret = mecvActual.c4Compress
            (
                meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd).mbufValue()
                , meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 1).mbufValue()
                , meOwner.c4StackValAt(c4FirstInd + 2)
            );
            meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1).c4Value(c4Ret);
        }

        catch(TError& errToCatch)
        {
            // If in verbose mode we'll log the C++ error
            if (!errToCatch.bLogged() && facCIDMacroEng().bLogWarnings())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
            ThrowAnErr(meOwner, m_c4ErrCompFailed, errToCatch);
        }
    }
     else if (c2MethId == m_c2MethId_Decompress)
    {
        try
        {
            const tCIDLib::TCard4 c4Ret = mecvActual.c4Decompress
            (
                meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd).mbufValue()
                , meOwner.mecvStackAtAs<TMEngMemBufVal>(c4FirstInd + 1).mbufValue()
                , meOwner.c4StackValAt(c4FirstInd + 2)
            );

            meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1).c4Value(c4Ret);
        }

        catch(TError& errToCatch)
        {
            // If in verbose mode we'll log the C++ error
            if (!errToCatch.bLogged() && facCIDMacroEng().bLogWarnings())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
            ThrowAnErr(meOwner, m_c4ErrDecompFailed, errToCatch);
        }
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TMEngZLibInfo: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngZLibInfo::ThrowAnErr(          TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4ToThrow
                            ,       TError&             errCaught)
{
    // If in verbose mode we'll log the C++ error
    if (!errCaught.bLogged() && facCIDMacroEng().bLogWarnings())
    {
        errCaught.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errCaught);
    }

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2EnumId_Errors
        , strClassPath()
        , c4ToThrow
        , m_pmeciErrors->strPartialName(c4ToThrow)
        , errCaught.strErrText()
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}

