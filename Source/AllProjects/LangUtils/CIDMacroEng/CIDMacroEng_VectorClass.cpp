//
// FILE NAME: CIDMacroEng_VectorClass.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/14/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a set of derivatives of the class info and class
//  value classes, which implement the fundamental MEng.System.Runtime.Vector
//  class.
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
RTTIDecls(TMEngVectorVal,TMEngCollectVal)
RTTIDecls(TMEngVectorInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_VectorClasses
{
    // -----------------------------------------------------------------------
    //  The names for the types that we support here. Each derivative has to
    //  be able to return strings that contain its name and full name.
    // -----------------------------------------------------------------------
    const TString   strVector(L"Vector");
    const TString   strClassPath(L"MEng.System.Runtime.Vector");
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngVectorVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngVectorVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngVectorVal::TMEngVectorVal( const   TString&                    strName
                                , const tCIDLib::TCard2             c2Id
                                , const tCIDLib::TCard2             c2ElemId
                                , const tCIDMacroEng::EConstTypes   eConst
                                ,       TMEngCollectInfo*           pmeciBase) :

    TMEngCollectVal(strName, c2Id, c2ElemId, eConst)
    , m_colElems(tCIDLib::EAdoptOpts::Adopt, 8)
    , m_pmeciBase(pmeciBase)
{
}

TMEngVectorVal::~TMEngVectorVal()
{
}


// ---------------------------------------------------------------------------
//  TMEngVectorVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngVectorVal::bDbgFormat(         TTextOutStream&         strmTarget
                            , const TMEngClassInfo&         meciThis
                            , const tCIDMacroEng::EDbgFmts  eFormat
                            , const tCIDLib::ERadices       eRadix
                            , const TCIDMacroEngine&        meOwner) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        strmTarget  << L"Element Type = " << meOwner.strClassPathForId(c2ElemId())
                    << L"\nElements = " << TCardinal(m_colElems.c4ElemCount(), eRadix);

        // Display up to the first 128 elements
        tCIDLib::TCard4 c4Count = m_colElems.c4ElemCount();
        if (c4Count > 256)
        {
            strmTarget << L"  (Showing first 256 elements...)\n";
            c4Count = 256;
        }

        // Get the class info object for the element type
        TStreamFmt strmfIndex(3, 0, tCIDLib::EHJustify::Right, kCIDLib::chSpace);
        TStreamFmt strmfString(0, 0, tCIDLib::EHJustify::Left);

        const TMEngClassInfo& meciElem = meOwner.meciFind(c2ElemId());
        strmTarget << kCIDLib::DNewLn;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            strmTarget  << strmfIndex << c4Index
                        << strmfString << L". ";
            m_colElems[c4Index]->bDbgFormat
            (
                strmTarget
                , meciElem
                , tCIDMacroEng::EDbgFmts::Short
                , eRadix
                , meOwner
            );
            strmTarget << kCIDLib::NewLn;
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TVoid
TMEngVectorVal::CopyFrom(const  TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
    {
        //
        //  In our case, we just need to be equiv collections, not exactly the
        //  same type, though with the same element types.
        //
        if (!meOwner.bAreEquivCols(c2ClassId(), mecvToCopy.c2ClassId(), kCIDLib::True))
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcClass_Mismatch
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::TypeMatch
                , meOwner.strClassPathForId(c2ClassId())
                , meOwner.strClassPathForId(mecvToCopy.c2ClassId())
            );
        }
    }
    const TMEngVectorVal& mecvSrc = static_cast<const TMEngVectorVal&>(mecvToCopy);

    //
    //  If our vector isn't the same size as theirs, then we need to
    //  either expand or reduce ours to fit.
    //
    TMEngClassInfo& meciElem = meOwner.meciFind(c2ElemId());
    const tCIDLib::TCard4 c4SrcCnt = mecvSrc.m_colElems.c4ElemCount();
    tCIDLib::TCard4 c4OurCnt = m_colElems.c4ElemCount();

    if (c4OurCnt < c4SrcCnt)
    {
        // Expand our list out
        while (c4OurCnt < c4SrcCnt)
        {
            m_colElems.Add
            (
                meciElem.pmecvMakeStorage
                (
                    TString::strEmpty()
                    , meOwner
                    , eConst()
                )
            );
            c4OurCnt++;
        }
    }
     else if (c4OurCnt > c4SrcCnt)
    {
        //
        //  Remove items till we get down to the right size. Optimize by
        //  seeing if the source is empty, in which case we can just do a
        //  remove all.
        //
        if (!c4SrcCnt)
        {
            m_colElems.RemoveAll();
            c4OurCnt = 0;
        }
         else
        {
            while (c4OurCnt > c4SrcCnt)
            {
                c4OurCnt--;
                m_colElems.RemoveAt(c4OurCnt);
            }
        }
    }

    // Now just run through and have our elements copy from theirs
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4OurCnt; c4Index++)
        m_colElems[c4Index]->CopyFrom(*mecvSrc.m_colElems[c4Index], meOwner);
}


TMEngClassVal*
TMEngVectorVal::pmecvGetAt(         TCIDMacroEngine&    meOwner
                            , const TMEngClassVal&      mecvIndex)
{
    //
    //  The passed value object should represent a Card4 value, or an enum.
    //  This should have been checked by the parser, so if not, we throw an
    //  exception.
    //
    tCIDLib::TCard4 c4Index = kCIDLib::c4MaxCard;
    TMEngClassInfo& meciIndex = meOwner.meciFind(mecvIndex.c2ClassId());

    if (mecvIndex.c2ClassId() == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Card4))
    {
        c4Index = static_cast<const TMEngCard4Val&>(mecvIndex).c4Value();
    }
     else if (meciIndex.c2ParentClassId() == tCIDLib::TCard2(tCIDMacroEng::EIntrinsics::Enum))
    {
        c4Index = static_cast<const TMEngEnumVal&>(mecvIndex).c4Ordinal();
    }
     else
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcRT_NumericIndexType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    // If it's not a valid index, then throw a macro level index error
    if (c4Index >= m_colElems.c4ElemCount())
        m_pmeciBase->IndexErr(meOwner, c4Index, meOwner.strClassPathForId(c2ClassId()));

    // Else, it looks ok so return the value object
    return m_colElems[c4Index];
}


// ---------------------------------------------------------------------------
//  TMEngVectorVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngVectorVal::AddObject(TMEngClassVal* const pmecvToAdd)
{
    m_colElems.Add(pmecvToAdd);
}


tCIDLib::TCard4 TMEngVectorVal::c4ElemCount() const
{
    return m_colElems.c4ElemCount();
}


tCIDLib::TVoid
TMEngVectorVal::InsertObject(       TCIDMacroEngine&        meOwner
                            ,       TMEngClassVal* const    pmecvToAdd
                            , const tCIDLib::TCard4         c4InsertAt)
{
    // In this case, insert at the end is ok, so just check for greater than
    if (c4InsertAt > m_colElems.c4ElemCount())
    {
        // We own the passed object, so clean it up before we throw
        delete pmecvToAdd;
        m_pmeciBase->IndexErr(meOwner, c4InsertAt, meOwner.strClassPathForId(c2ClassId()));
    }
    m_colElems.InsertAt(pmecvToAdd, c4InsertAt);
}


const TMEngClassVal&
TMEngVectorVal::mecvAt(TCIDMacroEngine& meOwner, const tCIDLib::TCard4 c4At) const
{
    if (c4At >= m_colElems.c4ElemCount())
        m_pmeciBase->IndexErr(meOwner, c4At, meOwner.strClassPathForId(c2ClassId()));
    return *m_colElems[c4At];
}

TMEngClassVal&
TMEngVectorVal::mecvAt(TCIDMacroEngine& meOwner, const tCIDLib::TCard4 c4At)
{
    if (c4At >= m_colElems.c4ElemCount())
        m_pmeciBase->IndexErr(meOwner, c4At, meOwner.strClassPathForId(c2ClassId()));
    return *m_colElems[c4At];
}


tCIDLib::TVoid TMEngVectorVal::RemoveAll()
{
    m_colElems.RemoveAll();
}


tCIDLib::TVoid
TMEngVectorVal::RemoveAt(TCIDMacroEngine& meOwner, const tCIDLib::TCard4 c4At)
{
    if (c4At >= m_colElems.c4ElemCount())
        m_pmeciBase->IndexErr(meOwner, c4At, meOwner.strClassPathForId(c2ClassId()));
    return m_colElems.RemoveAt(c4At);
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngVectorInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngVectorInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngVectorInfo::strPath()
{
    return CIDMacroEng_VectorClasses::strClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngVectorInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngVectorInfo::TMEngVectorInfo(       TCIDMacroEngine&    meOwner
                                , const TString&            strName
                                , const TString&            strBasePath
                                , const TString&            strParentClassPath
                                , const tCIDLib::TCard2     c2ElemId) :
    TMEngColBaseInfo
    (
        strName
        , strBasePath
        , meOwner
        , kCIDLib::True
        , tCIDMacroEng::EClassExt::Abstract
        , strParentClassPath
        , c2ElemId
    )
    , m_c2MethId_AddObject(kMacroEng::c2BadId)
    , m_c2MethId_DefCtor(kMacroEng::c2BadId)
    , m_c2MethId_GetElemCount(kMacroEng::c2BadId)
    , m_c2MethId_InsertObject(kMacroEng::c2BadId)
    , m_c2MethId_IsEmpty(kMacroEng::c2BadId)
    , m_c2MethId_RemoveAll(kMacroEng::c2BadId)
    , m_c2MethId_RemoveAt(kMacroEng::c2BadId)
    , m_pmeciBase(0)
{
    //
    //  If not the vector class itself, i.e. we are being used to instantiate
    //  some vector over a type, then set it to final.
    //
    if (strClassPath() != CIDMacroEng_VectorClasses::strClassPath)
        eExtend(tCIDMacroEng::EClassExt::Final);

    //
    //  Insure that the element class, if provided, is a copyable class. When
    //  the base class is being initialized, it will be zero. This should be
    //  check by the parser, so we just throw a C++ exception.
    //
    if (c2ElemId)
    {
        const TMEngClassInfo& meciElem = meOwner.meciFind(c2ElemId);
        if (!meciElem.bIsCopyable())
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcPrs_NotCopyable
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , meciElem.strClassPath()
            );
        }
    }
}

TMEngVectorInfo::~TMEngVectorInfo()
{
    //
    //  Since we own the objects we contain, which is a special case because
    //  otherwise, all objects are members of something or from the temp
    //  pool, we need to invoke destructors on them if we ever support
    //  macro level destructors.
    //
    //  <TBD>
}


// ---------------------------------------------------------------------------
//  TMEngVectorInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngVectorInfo::Init(TCIDMacroEngine& meOwner)
{
    // Store a pointer to our macro level base collection class for later use
    m_pmeciBase = meOwner.pmeciFindAs<TMEngCollectInfo>
    (
        TMEngCollectInfo::strPath()
    );

    //
    //  If we are doing the base MEng.System.Runtime.Vector class, then we
    //  do the stuff that's valid at that level. Else, we do the stuff for
    //  the derived class, which has all of the type specific methods. In the
    //  case of the base class, the m_c2ElemId member will be zero, else it
    //  will be the class id of the type we should store.
    //
    if (strClassPath() == CIDMacroEng_VectorClasses::strClassPath)
    {
        // Add the default constructor for this level
        {
            TMEngMethodInfo methiNew
            (
                L"ctor1_MEng.System.Runtime.Vector"
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::NonFinal
            );
            methiNew.bIsCtor(kCIDLib::True);
            m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
        }

        // Get the element count
        {
            TMEngMethodInfo methiNew
            (
                L"GetElemCount"
                , tCIDMacroEng::EIntrinsics::Card4
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
                , tCIDMacroEng::EConstTypes::Const
            );
            m_c2MethId_GetElemCount = c2AddMethodInfo(methiNew);
        }

        // Check if the vector is empty
        {
            TMEngMethodInfo methiNew
            (
                L"IsEmpty"
                , tCIDMacroEng::EIntrinsics::Boolean
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
                , tCIDMacroEng::EConstTypes::Const
            );
            m_c2MethId_IsEmpty = c2AddMethodInfo(methiNew);
        }

        // Remove all elements
        {
            TMEngMethodInfo methiNew
            (
                L"RemoveAll"
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
            );
            m_c2MethId_RemoveAll = c2AddMethodInfo(methiNew);
        }

        // Remove element at
        {
            TMEngMethodInfo methiNew
            (
                L"RemoveAt"
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
            );
            methiNew.c2AddInParm(L"AtIndex", tCIDMacroEng::EIntrinsics::Card4);
            m_c2MethId_RemoveAt = c2AddMethodInfo(methiNew);
        }
    }
     else
    {
        // Add the default constructor for this level
        {
            TMEngMethodInfo methiNew
            (
                TString(L"ctor1_") + strClassPath()
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
            );
            methiNew.bIsCtor(kCIDLib::True);
            m_c2MethId_DefCtor = c2AddMethodInfo(methiNew);
        }

        // Add an object
        {
            TMEngMethodInfo methiNew
            (
                L"AddObject"
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
            );
            methiNew.c2AddInParm(L"ToAdd", c2ElemId());
            m_c2MethId_AddObject = c2AddMethodInfo(methiNew);
        }

        // Add an object
        {
            TMEngMethodInfo methiNew
            (
                L"InsertObject"
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
            );
            methiNew.c2AddInParm(L"ToAdd", c2ElemId());
            methiNew.c2AddInParm(L"AtIndex", tCIDMacroEng::EIntrinsics::Card4);
            m_c2MethId_InsertObject = c2AddMethodInfo(methiNew);
        }
    }
}


TMEngClassVal*
TMEngVectorInfo::pmecvMakeStorage(  const   TString&                strName
                                    ,       TCIDMacroEngine&        meOwner
                                    , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngVectorVal(strName, c2Id(), c2ElemId(), eConst, m_pmeciBase);
}


// ---------------------------------------------------------------------------
//  TMacroEngVectorInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngVectorInfo::bInvokeMethod(         TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngVectorVal& mecvActual = static_cast<TMEngVectorVal&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_AddObject)
    {
        //
        //  Create another instance of the element type. We have to get the
        //  class info object, which is the factory for values of it's type.
        //
        //  These are not named items, so we just given them bogus names.
        //  Make the new instance const or not according to our constness.
        //
        TMEngClassInfo& meciElem = meOwner.meciFind(c2ElemId());
        TMEngClassVal* pmecvNew = meciElem.pmecvMakeStorage
        (
            TString::strEmpty()
            , meOwner
            , mecvActual.eConst()
        );
        TJanitor<TMEngClassVal> janNew(pmecvNew);

        // Copy from the first parm
        pmecvNew->CopyFrom(meOwner.mecvStackAt(c4FirstInd), meOwner);

        // It survived, so orphan it from the janitor into the vector
        mecvActual.AddObject(janNew.pobjOrphan());
    }
     else if (c2MethId == m_c2MethId_GetElemCount)
    {
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.c4ElemCount());
    }
     else if (c2MethId == m_c2MethId_InsertObject)
    {
        //
        //  Check the insert index for validity. We check up front this time
        //  instead of letting the value object do it, so that we can avoid
        //  creating an object just to throw it back away.
        //
        //  Note that insert at the end is ok, so we just check for greater
        //  than.
        //
        const tCIDLib::TCard4 c4InsertAt = meOwner.c4StackValAt(c4FirstInd + 1);
        if (c4InsertAt > mecvActual.c4ElemCount())
            m_pmeciBase->IndexErr(meOwner, c4InsertAt, strClassPath());

        //
        //  Create another instance of the element type. We have to get the
        //  class info object, which is the factory for values of it's type.
        //
        //  These are not named items, so we just given them bogus empty names.
        //  Make the new instance const or not according to our constness.
        //
        TMEngClassInfo& meciElem = meOwner.meciFind(c2ElemId());
        TMEngClassVal* pmecvNew = meciElem.pmecvMakeStorage
        (
            TString::strEmpty()
            , meOwner
            , mecvActual.eConst()
        );
        TJanitor<TMEngClassVal> janNew(pmecvNew);

        // Copy from the first parm
        pmecvNew->CopyFrom(meOwner.mecvStackAt(c4FirstInd), meOwner);

        //
        //  It survived, so orphan it from the janitor into the vector, getting
        //  out the insert index and passing it along. The value object will
        //  throw an index error if appropriate.
        //
        mecvActual.InsertObject(meOwner, janNew.pobjOrphan(), c4InsertAt);
    }
     else if (c2MethId == m_c2MethId_IsEmpty)
    {
        meOwner.mecvStackAtAs<TMEngBooleanVal>(c4FirstInd - 1).bValue
        (
            mecvActual.c4ElemCount() == 0
        );
    }
     else if ((c2MethId == m_c2MethId_DefCtor)
          ||  (c2MethId == m_c2MethId_RemoveAll))
    {
        mecvActual.RemoveAll();
    }
     else if (c2MethId == m_c2MethId_RemoveAt)
    {
        mecvActual.RemoveAt(meOwner, meOwner.c4StackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


