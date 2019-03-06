//
// FILE NAME: CIDUPnP_AsyncFinder.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/10/2012
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TUPnPAsyncFinder class.
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
#include    "CIDUPnP_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TUPnPAsyncFinder,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TUPnPAsyncFinderItem
// PREFIX: upnpafi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TUPnPAsyncFinderItem: Public, static methods
// ---------------------------------------------------------------------------
const TString&
TUPnPAsyncFinderItem::strKey(const TUPnPAsyncFinderItem& upnpafiSrc)
{
    return upnpafiSrc.m_strUID;
}


// ---------------------------------------------------------------------------
//  TUPnPAsyncFinderItem: Constructors and destructor
// ---------------------------------------------------------------------------
TUPnPAsyncFinderItem::
TUPnPAsyncFinderItem(const  tCIDLib::TCh* const     pszUID
                    , const tCIDLib::TCh* const     pszName
                    , const tCIDLib::TCh* const     pszModel
                    , const tCIDLib::TCh* const     pszType
                    ,       TKrnlUPnPDevice* const  pkupnpDev
                    , const tCIDLib::TCard4         c4SeqId) :

    m_c4SeqId(c4SeqId)
    , m_pkupnpdThis(pkupnpDev)
    , m_strModel(pszModel)
    , m_strName(pszName)
    , m_strType(pszType)
    , m_strUID(pszUID)
{
}


TUPnPAsyncFinderItem::~TUPnPAsyncFinderItem()
{
    // Clean up the kernel level object
    delete m_pkupnpdThis;
    m_pkupnpdThis = 0;
}




// ---------------------------------------------------------------------------
//  CLASS: TUPnPAsyncFinder
// PREFIX: upnpaf
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TUPnPAsyncFinder: Constructors and Destructor
// ---------------------------------------------------------------------------
TUPnPAsyncFinder::TUPnPAsyncFinder() :

    m_c4NextId(1)
    , m_c4SerialNum(1)
    , m_colDevList
      (
        tCIDLib::EAdoptOpts::Adopt
        , 109
        , new TStringKeyOps
        , &TUPnPAsyncFinderItem::strKey
        , tCIDLib::EMTStates::Safe
      )
    , m_kupnpafThis(this)
{
}

TUPnPAsyncFinder::~TUPnPAsyncFinder()
{
    // Make sure the kernel level object stops searchs and cleans up
    m_kupnpafThis.bCleanup();
}


// ---------------------------------------------------------------------------
//  TUPnPAsyncFinder: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Return whether the indicated unique id is in the list
tCIDLib::TBoolean TUPnPAsyncFinder::bDevIsAvailable(const TString& strUID) const
{
    // This is an atomic op, so no need for explicit locking
    return m_colDevList.bKeyExists(strUID);
}


// Let the caller know if the indicated search list is complete
tCIDLib::TBoolean
TUPnPAsyncFinder::bListComplete(const TString& strSearchName) const
{
    // Have to sync for this
    TMtxLocker mtxlSync(m_colDevList.pmtxLock());

    // Look up the search, throw if not found
    tCIDLib::TCard4 c4At;
    const TKeyValues* pkvalsCheck = pkvalsFindByName(strSearchName, c4At, kCIDLib::True);

    // If the second value is not empty, then it's completed
    return !pkvalsCheck->strVal2().bIsEmpty();
}


// Query the name and type URN of a device, if in the list
tCIDLib::TBoolean
TUPnPAsyncFinder::bQueryDevInfo(const   TString&            strUID
                                ,       TString&            strName
                                ,       TString&            strTypeURN
                                ,       tCIDLib::TCard4&    c4SeqId) const
{
    // We need to explicitly lock here since it's not a single op
    TMtxLocker mtxlSync(m_colDevList.pmtxLock());

    const TUPnPAsyncFinderItem* pkupnpafiRet = m_colDevList.pobjFindByKey
    (
        strUID, kCIDLib::False
    );

    if (pkupnpafiRet)
    {
        c4SeqId = pkupnpafiRet->m_c4SeqId;
        strName = pkupnpafiRet->m_strName;
        strTypeURN = pkupnpafiRet->m_strType;
        return kCIDLib::True;
    }
    return kCIDLib::False;

}


//
//  We'll want up to the indicated amount of time for the list to become
//  complete. We just do a simple spin, with a sleep since this isn't a quick
//  operation typically and a half second extra delay isn't going to make any
//  difference.
//
tCIDLib::TBoolean
TUPnPAsyncFinder::bWaitListComplete(const   TString&        strSearchName
                                    , const tCIDLib::TCard4 c4WaitMillis) const
{
    //
    //  We have to do some fancy sync here. Lock initially to find the
    //  the search (or throw if not found.) Then let it go while we wait.
    //  We reaquire each time before checking. Though we can do a quick
    //  check before we release to see if it's already set.
    //
    TMtxLocker mtxlSync(m_colDevList.pmtxLock());

    tCIDLib::TCard4 c4At;
    const TKeyValues* pkvalsCheck = pkvalsFindByName(strSearchName, c4At, kCIDLib::True);

    if (!pkvalsCheck->strVal2().bIsEmpty())
        return kCIDLib::True;

    // Not set yet, so release and start waiting
    mtxlSync.Release();

    const tCIDLib::TEncodedTime enctEnd = TTime::enctNowPlusMSs(c4WaitMillis);
    tCIDLib::TEncodedTime enctNow = TTime::enctNow();

    tCIDLib::TBoolean bRet = kCIDLib::False;
    TThread* pthrCaller = TThread::pthrCaller();
    while (enctNow < enctEnd)
    {
        // Do an interruptable sleep, so we'll come back if asked to shut down
        if (!pthrCaller->bSleep(500))
            break;

        //
        //  Reaquire the lock and check again. We give a second timeout, but
        //  it should never be even fractionally that long in the worst case.
        //
        mtxlSync.Lock(1000);
        if (!pkvalsCheck->strVal2().bIsEmpty())
        {
            bRet = kCIDLib::True;
            break;
        }

        // Still not, so release again
        mtxlSync.Release();

        // Get the current time and try again
        enctNow = TTime::enctNow();
    }

    return bRet;
}


// Return our finder serial number
tCIDLib::TCard4 TUPnPAsyncFinder::c4SerialNum() const
{
    return m_c4SerialNum;
}


//
//  If we have the indicated device in our list, set up the passed device
//  object for it, creating for it a copy of the kernel object. If we get
//  any updates for or removal of the device, we won't affect this created
//  device. He'll just get the new one next time.
//
tCIDLib::ELoadRes
TUPnPAsyncFinder::eSetupDevice( const   TString&            strUID
                                ,       TUPnPDevice&        upnpdSetup
                                ,       tCIDLib::TCard4&    c4SerialNum)
{
    // We need to explicitly lock here since it's not a single op
    TMtxLocker mtxlSync(m_colDevList.pmtxLock());

    // Give back the latest serial number regardless
    c4SerialNum = m_c4SerialNum;

    // see if we have this guy in our list
    TUPnPAsyncFinderItem* pkupnpafiRet = m_colDevList.pobjFindByKey
    (
        strUID, kCIDLib::False
    );

    // if not found, just return that now
    if (!pkupnpafiRet)
        return tCIDLib::ELoadRes::NotFound;

    //
    //  If found, and the sequence id the same, then we don't need to do anything.
    //  Else, set it up.
    //
    if (pkupnpafiRet->m_c4SeqId != upnpdSetup.c4SeqId())
    {
        //
        //  NOTE We are giving him a copy of our cached kernel object, so that
        //  he doesn't require any sync to use it. The sequence number will
        //  be used to know if the device has changed.
        //
        upnpdSetup.SetKrnlObj
        (
            pkupnpafiRet->m_pkupnpdThis->pkupnpdClone()
            , pkupnpafiRet->m_strUID.pszBuffer()
            , pkupnpafiRet->m_strName.pszBuffer()
            , pkupnpafiRet->m_strModel.pszBuffer()
            , pkupnpafiRet->m_strType.pszBuffer()
            , pkupnpafiRet->m_c4SeqId
        );
        return tCIDLib::ELoadRes::NewData;
    }

    // It was found, but nothing has changed
    return tCIDLib::ELoadRes::NoNewData;
}



//
//  Return a copy of the device list. The caller must work in terms of a copy
//  that is captured while synchronized. We can't provide an iteration scheme
//  or anything. We also give back our current list serial number so the caller
//  can easily check to see if changes have been made.
//
tCIDLib::TVoid
TUPnPAsyncFinder::QueryDevList(tCIDLib::TKValsCollect&  colToFill
                                , tCIDLib::TCard4&      c4SerialNum) const
{
    colToFill.RemoveAll();

    TMtxLocker mtxlSync(m_colDevList.pmtxLock());
    TDevList::TCursor cursList(&m_colDevList);
    for (; cursList; ++cursList)
    {
        const TUPnPAsyncFinderItem& kupnpafiCur = *cursList;
        colToFill.objAdd
        (
            TKeyValues
            (
                kupnpafiCur.m_strUID
                , kupnpafiCur.m_strName
                , kupnpafiCur.m_strModel
                , kupnpafiCur.m_strType
            )
        );
    }

    c4SerialNum = m_c4SerialNum;
}



//
//  Start up a search for the indicated type, and give it the indicated
//  name.
//
tCIDLib::TVoid
TUPnPAsyncFinder::StartSearch(  const   TString&    strFindType
                                , const TString&    strSearchName)
{
    //
    //  Make sure that this search name is not in our list already. We have
    //  to lock during this, but release before we do the start.
    //
    tCIDLib::TCard4 c4At;
    {
        TMtxLocker mtxlSync(m_colDevList.pmtxLock());
        if (pkvalsFindByName(strSearchName, c4At, kCIDLib::False))
        {
            facCIDUPnP().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kUPnPErrs::errcFnd_DupSearchName
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Init
                , m_strName
                , strSearchName
            );
        }
    }

    // Try to start the search on the kernel object
    TKrnlString kstrSearchID;
    if (!m_kupnpafThis.bListenFor(strFindType.pszBuffer(), kstrSearchID))
    {
        facCIDUPnP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcFnd_StartAsync
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , m_strName
            , strSearchName
        );
    }

    //
    //  It worked so update our search list. We have to lock again. The second
    //  value we leave empty. That's the search complete flag.
    //
    TMtxLocker mtxlSync(m_colDevList.pmtxLock());
    m_colSearches.objAdd
    (
        TKeyValues(strSearchName, kstrSearchID.pszValue(), TString::strEmpty())
    );
}


//
//  If the finder thread is running, we call the kernel object's stop method
//  to make it break out of its message pumping loop, then wait for it to
//  end. We then clean up any per-run data. At that point we can be reused
//  again.
//
tCIDLib::TVoid TUPnPAsyncFinder::StopSearch(const TString& strSearchName)
{
    //
    //  Have to sync for this, but release again before we do the actual
    //  operation.
    //
    const TKeyValues* pkvalsStop = 0;
    tCIDLib::TCard4 c4At;
    {
        TMtxLocker mtxlSync(m_colDevList.pmtxLock());
        pkvalsStop = pkvalsFindByName(strSearchName, c4At, kCIDLib::False);

        // If not found, return now
        if (!pkvalsStop)
            return;
    }

    // Ask the kernel object to stop this one
    if (!m_kupnpafThis.bStopListeningFor(strSearchName.pszBuffer()))
    {
        facCIDUPnP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcFnd_StopSearch
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , m_strName
            , strSearchName
        );
    }

    // It worked, so lock again and remove this search from our search list
    TMtxLocker mtxlSync(m_colDevList.pmtxLock());
    m_colSearches.RemoveAt(c4At);
}


// Get/set the name of this finder
const TString& TUPnPAsyncFinder::strName() const
{
    return m_strName;
}

const TString& TUPnPAsyncFinder::strName(const TString& strToSet)
{
    m_strName = strToSet;
    return m_strName;
}


// ---------------------------------------------------------------------------
//  TUPnPAsyncFinder: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  A new device has arrived. It may actually be new, or an update of an
//  existing one. So we have to see if it's there first. If so, we can just
//  update it with new info. Else add a new one.
//
tCIDLib::TBoolean
TUPnPAsyncFinder::bDeviceArrived(const  tCIDLib::TCh* const     pszUID
                                , const tCIDLib::TCh* const     pszName
                                , const tCIDLib::TCh* const     pszModel
                                , const tCIDLib::TCh* const     pszType
                                ,       TKrnlUPnPDevice* const  pkupnpDev)
{
    TMtxLocker mtxlSync(m_colDevList.pmtxLock());
    TString strUID(pszUID);

    //
    //  Bump the id we will give this object, skipping zero in the very unlikely
    //  chance we ever wrap
    //
    m_c4NextId++;
    if (!m_c4NextId)
        m_c4NextId++;

    TUPnPAsyncFinderItem* pupnpafiTar = m_colDevList.pobjFindByKey
    (
        strUID, kCIDLib::False
    );

    if (pupnpafiTar)
    {
        // Free the existing device object
        delete pupnpafiTar->m_pkupnpdThis;
        pupnpafiTar->m_pkupnpdThis = 0;

        pupnpafiTar->m_c4SeqId = m_c4NextId;
        pupnpafiTar->m_strName = pszName;
        pupnpafiTar->m_strModel = pszModel;
        pupnpafiTar->m_strType = pszType;
        pupnpafiTar->m_pkupnpdThis = pkupnpDev;
    }
     else
    {
        m_colDevList.Add
        (
            new TUPnPAsyncFinderItem
            (
                pszUID, pszName, pszModel, pszType, pkupnpDev, m_c4NextId
            )
        );
    }

    // Bump the serial number, wrapping if needed
    m_c4SerialNum++;
    if (!m_c4SerialNum)
        m_c4SerialNum++;

    // Indicate that we are keeping the device object
    return kCIDLib::True;
}


//
//  We can just do a remove of the passed unique id.
//
tCIDLib::TVoid TUPnPAsyncFinder::DeviceRemoved(const tCIDLib::TCh* const pszUID)
{
    TMtxLocker mtxlSync(m_colDevList.pmtxLock());
    const TString strKey(pszUID);
    if (m_colDevList.bRemoveKeyIfExists(strKey))
    {
        // Bump the serial number, wrapping if needed
        m_c4SerialNum++;
        if (!m_c4SerialNum)
            m_c4SerialNum++;
    }
}


//
//  We look up the indicated search by its low level id and set its complete
//  flag to indicate its complete. We have to lock while doing this.
//
tCIDLib::TVoid
TUPnPAsyncFinder::ListComplete(const tCIDLib::TCh* const pszSearchID)
{
    TMtxLocker mtxlSync(m_colDevList.pmtxLock());

    const TString strID(pszSearchID);

    const tCIDLib::TCard4 c4Count = m_colSearches.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TKeyValues& kvalsCur = m_colSearches[c4Index];

        // If this is our guy, set it's 2nd value and break out
        if (kvalsCur.strVal1() == strID)
        {
            kvalsCur.strVal2(L"1");
            break;
        }
    }
}


// ---------------------------------------------------------------------------
//  TUPnPAsyncFinder: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  See if a given search is in our search list. The caller must have locked
//  already because he'll have to already have a lock to insure what we return
//  will be valid by the time we get it back to him.
//
//  Actually, it wouldn't go away since that would require us being used by
//  multiple threads without sync. But the caller would just have to lock after
//  we locked, so best he just does it.
//
const TKeyValues*
TUPnPAsyncFinder::pkvalsFindByName( const   TString&            strName
                                    ,       tCIDLib::TCard4&    c4FoundAt
                                    , const tCIDLib::TBoolean   bThrowIfNot) const
{
    const TKeyValues* pkvalsRet = 0;

    // Search the list for this search name
    const tCIDLib::TCard4 c4Count = m_colSearches.c4ElemCount();
    for (c4FoundAt = 0; c4FoundAt < c4Count; c4FoundAt++)
    {
        const TKeyValues& kvalsCur = m_colSearches[c4FoundAt];

        // If this is our guy, see if value 2 is empty. If not, then it's complete
        if (kvalsCur.strKey() == strName)
        {
            pkvalsRet = &kvalsCur;
            break;
        }
    }

    // If not find, and they said throw, then throw
    if (!pkvalsRet && bThrowIfNot)
    {
        facCIDUPnP().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kUPnPErrs::errcFnd_SeachNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , m_strName
            , strName
        );
    }

    return pkvalsRet;
}


