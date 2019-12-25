//
// FILE NAME: CIDMacroEng_ArrayClass.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/10/2005
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
//  value classes, which implement the MEng.System.Runtime.Array CML class.
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
RTTIDecls(TMEngArrayVal,TMEngCollectVal)
RTTIDecls(TMEngArrayInfo,TMEngClassInfo)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_ArrayClasses
{
    // -----------------------------------------------------------------------
    //  The names for the types that we support here. Each derivative has to
    //  be able to return strings that contain its name and full name.
    // -----------------------------------------------------------------------
    const TString   strArray(L"Array");
    const TString   strClassPath(L"MEng.System.Runtime.Array");
}




// ---------------------------------------------------------------------------
//  CLASS: TMEngArrayVal
// PREFIX: mecv
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngArrayVal: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngArrayVal::TMEngArrayVal(const  TString&                strName
                            , const tCIDLib::TCard2         c2Id
                            , const tCIDLib::TCard2         c2ElemId
                            , const tCIDMacroEng::EConstTypes  eConst
                            ,       TMEngCollectInfo*       pmeciBase) :

    TMEngCollectVal(strName, c2Id, c2ElemId, eConst)
    , m_c4ElemCount(0)
    , m_pElems(nullptr)
    , m_pmeciBase(pmeciBase)
{
}

TMEngArrayVal::~TMEngArrayVal()
{
    CleanUpArray();
}


// ---------------------------------------------------------------------------
//  TMEngArrayVal: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngArrayVal::bDbgFormat(          TTextOutStream&     strmTarget
                            , const TMEngClassInfo&
                            , const tCIDMacroEng::EDbgFmts eFormat
                            , const tCIDLib::ERadices   eRadix
                            , const TCIDMacroEngine&    meOwner) const
{
    if (eFormat == tCIDMacroEng::EDbgFmts::Long)
    {
        strmTarget  << L"Element Type = " << meOwner.strClassPathForId(c2ElemId())
                    << L"\nElement Count = " << TCardinal(m_c4ElemCount, eRadix);


        // Display up to the first 128 elements
        tCIDLib::TCard4 c4Count = m_c4ElemCount;
        if (c4Count > 256)
        {
            strmTarget << L" (Showing first 256 entries...)\n";
            c4Count = 256;
        }

        // Get the class info object for the element type
        TStreamFmt strmfIndex(3, 0, tCIDLib::EHJustify::Right, kCIDLib::chSpace);
        TStreamFmt strmfString(0, 0, tCIDLib::EHJustify::Left);

        const TMEngClassInfo& meciElem = meOwner.meciFind(c2ElemId());
        strmTarget << kCIDLib::DNewLn;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            strmTarget << strmfIndex << c4Index
                       << strmfString << L". ";

            // They are faulted in, so make sure there's something in it
            if (m_pElems[c4Index])
            {
                m_pElems[c4Index]->bDbgFormat
                (
                    strmTarget
                    , meciElem
                    , tCIDMacroEng::EDbgFmts::Short
                    , eRadix
                    , meOwner
                );
            }
             else
            {
                strmTarget << L"[Not Set]";
            }
            strmTarget << kCIDLib::NewLn;
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TVoid
TMEngArrayVal::CopyFrom(const   TMEngClassVal&      mecvToCopy
                        ,       TCIDMacroEngine&    meOwner)
{
    if (meOwner.bValidation())
    {
        //
        //  In our case, we just need to be equiv collections, not exactly the
        //  same type. The element types though have to be the same.
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
    const TMEngArrayVal& mecvSrc = static_cast<const TMEngArrayVal&>(mecvToCopy);

    //
    //  If the sizes are different, re-allocate our array, but keep as
    //  many of the elements as we can.
    //
    if (m_c4ElemCount != mecvSrc.m_c4ElemCount)
    {
        TMEngClassVal** pNew = new TMEngClassVal*[mecvSrc.m_c4ElemCount];
        TArrayJanitor<TMEngClassVal*> janNew(pNew);
        TRawMem::SetMemBuf
        (
            pNew
            , tCIDLib::TCard1(0)
            , mecvSrc.m_c4ElemCount * sizeof(TMEngClassVal*)
        );

        //
        //  Copy as many elements from the current array as we can make
        //  use of into the new one. We won't keep any in our list that are
        //  not in the source list. So there's no point taking any more
        //  than required to match the available slots in the source.
        //
        tCIDLib::TCard4 c4OurInd = 0;
        tCIDLib::TCard4 c4OldInd = 0;

        while ((c4OurInd < mecvSrc.m_c4ElemCount) && (c4OldInd < m_c4ElemCount))
        {
            //
            //  If there is an object in the source at this slot, find the
            //  next available one in the old array, and put it here in the
            //  new array.
            //
            if (mecvSrc.m_pElems[c4OurInd])
            {
                TMEngClassVal* pmecvTaken = nullptr;
                while (c4OldInd < m_c4ElemCount)
                {
                    if (m_pElems[c4OldInd])
                    {
                        pmecvTaken = m_pElems[c4OldInd];
                        m_pElems[c4OldInd] = nullptr;
                        break;
                    }
                    c4OldInd++;
                }

                //
                //  If we got one, then store it. If we didn't, then we are
                //  out of old elements, so break out.
                //
                if (pmecvTaken)
                    pNew[c4OurInd] = pmecvTaken;
                else
                    break;
            }
            c4OurInd++;
        }


        //
        //  Ok, we've taken anything we can take, so now clean up any remaining
        //  elements in our current array and release it.
        //
        CleanUpArray();

        // Ok, we can commit, so orphan out the new guy
        m_pElems = janNew.paOrphan();
        m_c4ElemCount = mecvSrc.m_c4ElemCount;
    }

    //
    //  Now we have to go through and copy the contents of each element that
    //  is valid in the source.
    //
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4ElemCount; c4Index++)
    {
        //
        //  If the source doesn't have this element, then we have to deal with
        //  that specially. If we do have one, we delete ours, else we just
        //  skip this one and go to the next one.
        //
        if (!mecvSrc.m_pElems[c4Index])
        {
            if (m_pElems[c4Index])
            {
                delete m_pElems[c4Index];
                m_pElems[c4Index] = nullptr;
            }
            continue;
        }

        //  If we don't have one at this slot, then make one.
        if (!m_pElems[c4Index])
            FaultInElem(meOwner, c4Index);

        // And copy from the source
        m_pElems[c4Index]->CopyFrom(*mecvSrc.m_pElems[c4Index], meOwner);
    }
}


TMEngClassVal*
TMEngArrayVal::pmecvGetAt(          TCIDMacroEngine&    meOwner
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
    if (c4Index >= m_c4ElemCount)
        m_pmeciBase->IndexErr(meOwner, c4Index, meOwner.strClassPathForId(c2ClassId()));

    // If we haven't faulted in an object for this slot yet, then do it
    if (!m_pElems[c4Index])
    if (!m_pElems[c4Index])
        FaultInElem(meOwner, c4Index);

    // Else, it looks ok so return the value object
    return m_pElems[c4Index];
}



// ---------------------------------------------------------------------------
//  TMEngArrayVal: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TMEngArrayVal::c4ElemCount() const
{
    return m_c4ElemCount;
}


tCIDLib::TVoid TMEngArrayVal::Initialize(const tCIDLib::TCard4 c4ElemCount)
{
    // If we have any existing contents, clean it up
    CleanUpArray();

    //
    //  Allocate the new array then zero out the pointers. We fault the
    //  value objects in upon use, to minimize overhead.
    //
    m_pElems = new TMEngClassVal*[c4ElemCount];
    m_c4ElemCount = c4ElemCount;
    TRawMem::SetMemBuf
    (
        m_pElems
        , tCIDLib::TCard1(0)
        , c4ElemCount * sizeof(TMEngClassVal*)
    );
}


const TMEngClassVal&
TMEngArrayVal::mecvAt(TCIDMacroEngine& meOwner, const tCIDLib::TCard4 c4At)
{
    if (c4At >= m_c4ElemCount)
        m_pmeciBase->IndexErr(meOwner, c4At, meOwner.strClassPathForId(c2ClassId()));

    // If we've not faulted this one in yet, then do it
    if (!m_pElems[c4At])
        FaultInElem(meOwner, c4At);
    return *m_pElems[c4At];
}


tCIDLib::TVoid
TMEngArrayVal::Reallocate(const tCIDLib::TCard4 c4ElemCount)
{
    // If the element count is the same, then we don't do anything
    if (c4ElemCount == m_c4ElemCount)
        return;

    // Allocate a new array and put a janitor on it till we are done
    TMEngClassVal** pNew = new TMEngClassVal*[c4ElemCount];
    TArrayJanitor<TMEngClassVal*> janNew(pNew);
    TRawMem::SetMemBuf
    (
        pNew
        , tCIDLib::TCard1(0)
        , c4ElemCount * sizeof(TMEngClassVal*)
    );

    //
    //  Now go through and for all the elements in the old array that
    //  fall within the new size, steal them.
    //
    const tCIDLib::TCard4 c4MaxCopy = tCIDLib::MinVal(c4ElemCount, m_c4ElemCount);
    tCIDLib::TCard4 c4Index = 0;
    while (c4Index < c4MaxCopy)
    {
        if (m_pElems[c4Index])
        {
            pNew[c4Index] = m_pElems[c4Index];
            m_pElems[c4Index] = nullptr;
        }
        c4Index++;
    }

    //
    //  Ok, we've taken anything we can take, so now clean up any remaining
    //  elements in our current array and release it.
    //
    CleanUpArray();

    // Ok, we can commit, so orphan out the new guy
    m_pElems = janNew.paOrphan();
    m_c4ElemCount = c4ElemCount;
}


// ---------------------------------------------------------------------------
//  TMEngArrayVal: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngArrayVal::CleanUpArray()
{
    // Clean up the elements in our array first
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4ElemCount; c4Index++)
    {
        if (m_pElems[c4Index])
        {
            try
            {
                // <TBD> If we ever support dtors, we'd invoke them here

                delete m_pElems[c4Index];
            }

            catch(...)
            {
                // Eat it
            }
        }
    }

    // Now clean up the array itself
    delete [] m_pElems;
    m_pElems = nullptr;
    m_c4ElemCount = 0;
}


tCIDLib::TVoid
TMEngArrayVal::FaultInElem(TCIDMacroEngine& meOwner, const tCIDLib::TCard4 c4At)
{
    TMEngClassInfo& meciElem = meOwner.meciFind(c2ElemId());
    TMEngClassVal* pmecvNew = meciElem.pmecvMakeStorage
    (
        TString::strEmpty()
        , meOwner
        , eConst()
    );
    TJanitor<TMEngClassVal> janNew(pmecvNew);

    // Invoke it's default constructor if not an intrinsic class
    if (!meOwner.bIsIntrinsicClass(meciElem.c2Id())
    &&  !meOwner.bInvokeDefCtor(*pmecvNew, meOwner.pcuctxRights()))
    {
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcClass_DefCtorFailed
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , meOwner.strClassPathForId(c2ClassId())
        );
    }

    // It worked, so store it
    m_pElems[c4At] = janNew.pobjOrphan();
}





// ---------------------------------------------------------------------------
//  CLASS: TMEngArrayInfo
// PREFIX: meci
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMEngArrayInfo: Public, static methods
// ---------------------------------------------------------------------------
const TString& TMEngArrayInfo::strPath()
{
    return CIDMacroEng_ArrayClasses::strClassPath;
}


// ---------------------------------------------------------------------------
//  TMEngArrayInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMEngArrayInfo::TMEngArrayInfo(         TCIDMacroEngine&    meOwner
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
    , m_c2MethId_Ctor(kCIDMacroEng::c2BadId)
    , m_c2MethId_GetElemCount(kCIDMacroEng::c2BadId)
    , m_c2MethId_Reallocate(kCIDMacroEng::c2BadId)
    , m_pmeciBase(0)
{
    //
    //  If not the vector class itself, i.e. we are being used to instantiate
    //  some vector over a type, then set it to final.
    //
    if (strClassPath() != CIDMacroEng_ArrayClasses::strClassPath)
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

TMEngArrayInfo::~TMEngArrayInfo()
{
}


// ---------------------------------------------------------------------------
//  TMEngArrayInfo: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMEngArrayInfo::Init(TCIDMacroEngine& meOwner)
{
    // Store a pointer to our macro level base collection class for later use
    m_pmeciBase = meOwner.pmeciFindAs<TMEngCollectInfo>
    (
        TMEngCollectInfo::strPath()
    );

    //
    //  If we are doing the base MEng.System.Runtime.Array class, then we
    //  do the stuff that's valid at that level. Else, we do the stuff for
    //  the derived class, which has all of the type specific methods. In the
    //  case of the base class, the m_c2ElemId member will be zero, else it
    //  will be the class id of the type we should store.
    //
    if (strClassPath() == CIDMacroEng_ArrayClasses::strClassPath)
    {
        // Add the only constructor
        {
            TMEngMethodInfo methiNew
            (
                L"ctor1_MEng.System.Runtime.Array"
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
            );
            methiNew.bIsCtor(kCIDLib::True);
            methiNew.c2AddInParm(L"ElemCount", tCIDMacroEng::EIntrinsics::Card4);
            m_c2MethId_Ctor = c2AddMethodInfo(methiNew);
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

        // Reallocate the array
        {
            TMEngMethodInfo methiNew
            (
                L"Reallocate"
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
            );
            methiNew.c2AddInParm(L"NewSize", tCIDMacroEng::EIntrinsics::Card4);
            m_c2MethId_Reallocate = c2AddMethodInfo(methiNew);
        }
    }
     else
    {
        // Add the only constructor for this level
        {
            TMEngMethodInfo methiNew
            (
                TString(L"ctor1_") + strClassPath()
                , tCIDMacroEng::EIntrinsics::Void
                , tCIDMacroEng::EVisTypes::Public
                , tCIDMacroEng::EMethExt::Final
            );
            methiNew.bIsCtor(kCIDLib::True);
            methiNew.c2AddInParm(L"ElemCount", tCIDMacroEng::EIntrinsics::Card4);
            m_c2MethId_Ctor = c2AddMethodInfo(methiNew);
        }
    }
}


TMEngClassVal*
TMEngArrayInfo::pmecvMakeStorage(const  TString&                strName
                                ,       TCIDMacroEngine&
                                , const tCIDMacroEng::EConstTypes  eConst) const
{
    return new TMEngArrayVal(strName, c2Id(), c2ElemId(), eConst, m_pmeciBase);
}


// ---------------------------------------------------------------------------
//  TMacroEngArrayInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMEngArrayInfo::bInvokeMethod(          TCIDMacroEngine&    meOwner
                                , const TMEngMethodInfo&    methiTarget
                                ,       TMEngClassVal&      mecvInstance)
{
    TMEngArrayVal& mecvActual = static_cast<TMEngArrayVal&>(mecvInstance);

    const tCIDLib::TCard4 c4FirstInd = meOwner.c4FirstParmInd(methiTarget);
    const tCIDLib::TCard2 c2MethId   = methiTarget.c2Id();

    if (c2MethId == m_c2MethId_GetElemCount)
    {
        TMEngCard4Val& mecvRet = meOwner.mecvStackAtAs<TMEngCard4Val>(c4FirstInd - 1);
        mecvRet.c4Value(mecvActual.c4ElemCount());
    }
     else if (c2MethId == m_c2MethId_Ctor)
    {
        mecvActual.Initialize(meOwner.c4StackValAt(c4FirstInd));
    }
     else if (c2MethId == m_c2MethId_Reallocate)
    {
        mecvActual.Reallocate(meOwner.c4StackValAt(c4FirstInd));
    }
     else
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


