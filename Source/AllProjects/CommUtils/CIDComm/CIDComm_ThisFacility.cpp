//
// FILE NAME: CIDComm_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/27/2000
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
//  This file implements TFacCIDComm class.
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
#include    "CIDComm_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDComm,TFacility)


// ---------------------------------------------------------------------------
//  Local types and data
// ---------------------------------------------------------------------------
namespace CIDComm_ThisFacility
{
    namespace
    {
        // -----------------------------------------------------------------------
        //  This is our port factory list
        // -----------------------------------------------------------------------
        using TFactoryList = TSafeRefVector<TComPortFactory>;
        TFactoryList colFList(tCIDLib::EAdoptOpts::Adopt, 4);
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TFacCIDComm
// PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDComm: Public, static data
// ---------------------------------------------------------------------------
const TString TFacCIDComm::strLocalFactoryId(L"LocalSerialPortFactory");
const TString TFacCIDComm::strLocalPortPrefix(L"/Local/COM");


// ---------------------------------------------------------------------------
//  TFacCIDComm: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDComm::TFacCIDComm() :

    TFacility
    (
        L"CIDComm"
        , tCIDLib::EModTypes::Dll
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}


// ---------------------------------------------------------------------------
//  TFacCIDComm:: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  In order for higher level code to atomically update our port factory list,
//  we implement the MLockable interface, just in terms of our internal safe
//  collection.
//
tCIDLib::TBoolean TFacCIDComm::bTryLock(const tCIDLib::TCard4 c4WaitMSs) const
{
    return CIDComm_ThisFacility::colFList.bTryLock(c4WaitMSs);
}

tCIDLib::TVoid TFacCIDComm::Lock(const tCIDLib::TCard4 c4WaitMSs) const
{
    CIDComm_ThisFacility::colFList.Lock(c4WaitMSs);
}

tCIDLib::TVoid TFacCIDComm::Unlock() const
{
    CIDComm_ThisFacility::colFList.Unlock();
}



// ---------------------------------------------------------------------------
//  TFacCIDComm: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TFacCIDComm::bCanConfigure(const TString& strToCheck) const
{
    // Lock the list while we do this
    TLocker lockrSync(&CIDComm_ThisFacility::colFList);

    //
    //  Iterate all of the installed port factories and ask each one if it owns this
    //  port. If so, return his can configure flag.
    //
    CIDComm_ThisFacility::TFactoryList::TCursor cursList(&CIDComm_ThisFacility::colFList);
    for (; cursList; ++cursList)
    {
        if (cursList->bOwnsThisPort(strToCheck))
            return cursList->bCanConfigure();
    }

    // Never found anyone who claims it, so just say we cannot
    return kCIDLib::False;
}


tCIDLib::TBoolean TFacCIDComm::bIsValidPortName(const TString& strToCheck) const
{
    // Lock the list while we do this
    TLocker lockrSync(&CIDComm_ThisFacility::colFList);

    //
    //  Iterate all of the installed port factories and ask each one if it
    //  owns this port. If none say yes, it's a bad path. If one says yes,
    //  then ask it to validate the name and return that result.
    //
    CIDComm_ThisFacility::TFactoryList::TCursor cursList(&CIDComm_ThisFacility::colFList);
    for (; cursList; ++cursList)
    {
        if (cursList->bOwnsThisPort(strToCheck))
            return cursList->bValidatePath(strToCheck);
    }

    // Never found anyone who claims it, so obviously bad
    return kCIDLib::False;
}


tCIDLib::TBoolean
TFacCIDComm::bQueryPorts(       tCIDLib::TStrCollect& colToFill
                        , const tCIDLib::TBoolean     bLocalOnly
                        , const tCIDLib::TBoolean     bAvailOnly) const
{
    // Lock the list while we do this
    TLocker lockrSync(&CIDComm_ThisFacility::colFList);

    //
    //  Iterate the installed factories and ask each of them to add their
    //  avialable ports to the list. If the bLocalOnly flag is set, then we
    //  only do the 0th entry.
    //
    const tCIDLib::TCard4 c4Count
    (
        bLocalOnly ? 1 : CIDComm_ThisFacility::colFList.c4ElemCount()
    );

    colToFill.RemoveAll();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TComPortFactory* pfactCur = CIDComm_ThisFacility::colFList.pobjAt(c4Index);
        pfactCur->bQueryPorts(colToFill, bAvailOnly);
    }

    // Return true if we endedup with any
    return !colToFill.bIsEmpty();
}


//
//  Looks up the factory with the indicated id and removes it from our
//  list.
//
tCIDLib::TVoid
TFacCIDComm::DeregisterFactory(const TString& strFactoryId)
{
    // Lock the list while we do this
    TLocker lockrSync(&CIDComm_ThisFacility::colFList);

    // Find this factory by its id, and remove it
    const tCIDLib::TCard4 c4Count = CIDComm_ThisFacility::colFList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TComPortFactory* pfactCur = CIDComm_ThisFacility::colFList.pobjAt(c4Index);
        if (pfactCur->strId() == strFactoryId)
        {
            CIDComm_ThisFacility::colFList.RemoveAt(c4Index);
            break;
        }
    }
}



//
//  Make a new comm port based on the passed path, if we do indeed support
//  the factory it indicates.
//
TCommPortBase* TFacCIDComm::pcommMakeNew(const TString& strPath)
{
    // Lock the list while we do this
    TLocker lockrSync(&CIDComm_ThisFacility::colFList);

    TCommPortBase* pcommRet = nullptr;
    const tCIDLib::TCard4 c4Count = CIDComm_ThisFacility::colFList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TComPortFactory* pfactCur = CIDComm_ThisFacility::colFList.pobjAt(c4Index);
        if (pfactCur->bOwnsThisPort(strPath))
        {
            pcommRet = pfactCur->pcommMakeNew(strPath);
            break;
        }
    }
    return pcommRet;
}


//
//  Provide access to particular com port factory by id. NOTE that the
//  caller should lock the list before they do this because the factory
//  could be accessed while they are accessing it otherwise. And they should
//  keep the access very short.
//
TComPortFactory* TFacCIDComm::pfactById(const TString& strId)
{
    // Lock the list while we do this
    TLocker lockrSync(&CIDComm_ThisFacility::colFList);

    const tCIDLib::TCard4 c4Count = CIDComm_ThisFacility::colFList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TComPortFactory* pfactCur = CIDComm_ThisFacility::colFList.pobjAt(c4Index);
        if (pfactCur->strId() == strId)
            return pfactCur;
    }
    return nullptr;
}


//
//  Adopts the passed factory. We make sure that no other factory has the
//  same id, if debugging.
//
tCIDLib::TVoid
TFacCIDComm::RegisterFactory(TComPortFactory* const pfactToAdopt)
{
    // Lock the list while we do this
    TLocker lockrSync(&CIDComm_ThisFacility::colFList);

    #if CID_DEBUG_ON
    const TString& strNew = pfactToAdopt->strId();
    const tCIDLib::TCard4 c4Count = CIDComm_ThisFacility::colFList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TComPortFactory* pfactCur = CIDComm_ThisFacility::colFList.pobjAt(c4Index);
        if (pfactCur->strId() == strNew)
        {
            facCIDComm().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCommErrs::errcFact_DupId
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Already
                , strNew
            );
        }
    }
    #endif

    // Looks ok so add it
    CIDComm_ThisFacility::colFList.Add(pfactToAdopt);
}


