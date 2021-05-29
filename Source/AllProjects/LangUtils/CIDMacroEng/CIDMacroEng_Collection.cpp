//
// FILE NAME: CIDMacroEng_Collection.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/14/2003
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
//  This file implements the base classes from which all collection classes
//  derive their value and info classes.
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
RTTIDecls(TMEngCollectVal,TMEngClassVal)
RTTIDecls(TMEngCollectInfo,TMEngClassInfo)
RTTIDecls(TMEngColBaseInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDMacroEng_Collection
    {
        // -----------------------------------------------------------------------
        //  The names for the types that we support here. Each derivative has to
        //  be able to return strings that contain its name and full name.
        // -----------------------------------------------------------------------
        const TString   strName(L"Collection");
        const TString   strClassPath(L"MEng.System.Runtime.Collection");
    }
}


// ---------------------------------------------------------------------------
//  CLASS: TMEngCollectVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCollectVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCollectVal::~TMEngCollectVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngCollectVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard2 TMEngCollectVal::c2ElemId() const
{
    return m_c2ElemId;
}


// ---------------------------------------------------------------------------
//  TMEngCollectVal: Hidden constructors
// ---------------------------------------------------------------------------
TMEngCollectVal::TMEngCollectVal(const  TString&                strName
                                , const tCIDLib::TCard2         c2Id
                                , const tCIDLib::TCard2         c2ElemId
                                , const tCIDMacroEng::EConstTypes  eConst) :

    TMEngClassVal(strName, c2Id, eConst)
    , m_c2ElemId(c2ElemId)
{
}



// ---------------------------------------------------------------------------
//  CLASS: TMEngCollectInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngCollectInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngCollectInfo::strPath()
{
    return CIDMacroEng_Collection::strClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngCollectInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngCollectInfo::TMEngCollectInfo(TCIDMacroEngine& meOwner) :

    TMEngClassInfo
    (
        CIDMacroEng_Collection::strName
        , TFacCIDMacroEng::strRuntimeClassPath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Abstract
        , L"MEng.Object"
    )
    , m_c2ErrId(kCIDMacroEng::c2BadId)
    , m_c4ErrBadInd(kCIDLib::c4MaxCard)
    , m_pmeciErrors(0)
{
}

TMEngCollectInfo::~TMEngCollectInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngCollectInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngCollectInfo::Init(TCIDMacroEngine& meOwner)
{
    // We need to generate an enum class for our errors
    {
        m_pmeciErrors = new TMEngEnumInfo
        (
            meOwner
            , L"CollectErrors"
            , strClassPath()
            , L"MEng.Enum"
            , 4
        );
        m_c4ErrBadInd = m_pmeciErrors->c4AddEnumItem(L"BadIndex", L"Index %(1) is invalid for this collection object");
        m_pmeciErrors->BaseClassInit(meOwner);
        m_c2ErrId = meOwner.c2AddClass(m_pmeciErrors);
        bAddNestedType(m_pmeciErrors->strClassPath());
    }
}


//
//  This isn't used, but we have to provide it so that this class can be
//  instantiated.
//
TMEngClassVal* TMEngCollectInfo::
pmecvMakeStorage(const TString&, TCIDMacroEngine&, const tCIDMacroEng::EConstTypes) const
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
//  TMEngCollectInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMEngCollectInfo::IndexErr(         TCIDMacroEngine&    meOwner
                            , const tCIDLib::TCard4     c4Index
                            , const TString&            strClassPath)
{
    TString strText(m_pmeciErrors->strTextValue(m_c4ErrBadInd));
    strText.eReplaceToken(c4Index, kCIDLib::chDigit1);

    // Set the exception info on the engine
    meOwner.SetException
    (
        m_c2ErrId
        , strClassPath
        , m_c4ErrBadInd
        , m_pmeciErrors->strPartialName(m_c4ErrBadInd)
        , strText
        , meOwner.c4CurLine()
    );

    // And throw the excpetion that represents a macro level exception
    throw TExceptException();
}


// ---------------------------------------------------------------------------
//  TMEngCollectInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngCollectInfo::bInvokeMethod(        TCIDMacroEngine&
                                , const TMEngMethodInfo&
                                ,       TMEngClassVal&)
{
    // We got no methods of our own right now
    return kCIDLib::False;
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngColBaseInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngColBaseInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngColBaseInfo::~TMEngColBaseInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngColBaseInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard2 TMEngColBaseInfo::c2ElemId() const
{
    return m_c2ElemId;
}


// ---------------------------------------------------------------------------
//  TMEngColBaseInfo: Hidden Constructors
// ---------------------------------------------------------------------------
TMEngColBaseInfo::TMEngColBaseInfo( const   TString&             strName
                                    , const TString&             strBasePath
                                    ,       TCIDMacroEngine&     meOwner
                                    , const tCIDLib::TBoolean    bCopyable
                                    , const tCIDMacroEng::EClassExt eExtend
                                    , const TString&             strParentClass
                                    , const tCIDLib::TCard2      c2ElemId) :
    TMEngClassInfo
    (
        strName
        , strBasePath
        , meOwner
        , bCopyable
        , eExtend
        , strParentClass
    )
    , m_c2ElemId(c2ElemId)
{
}

