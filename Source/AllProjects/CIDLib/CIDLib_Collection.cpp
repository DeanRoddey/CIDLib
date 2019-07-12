//
// FILE NAME: CIDLib_Collection.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/23/2002
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
//  This file implements the bit of out of line stuff that collections have.
//  The most base collection class is non-templatized, so we can get some
//  stuff out of the header.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TColPubSubInfo,TObject)
RTTIDecls(TCollectionBase,TObject)
RTTIDecls(TFundColBase,TCollectionBase)



// ---------------------------------------------------------------------------
//   CLASS: TColPubSubInfo
//  PREFIX: colpsi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TColPubSubInfo: Constructors and destructor
// ---------------------------------------------------------------------------
TColPubSubInfo::TColPubSubInfo() :

    m_c4Index1(0)
    , m_c4Index2(0)
    , m_eEvent(EEvents::Count)
{
}

TColPubSubInfo::TColPubSubInfo(const EEvents eEvent, const tCIDLib::TCard4 c4Index) :

    m_c4Index1(c4Index)
    , m_c4Index2(0)
    , m_eEvent(eEvent)
{
}


TColPubSubInfo::TColPubSubInfo( const   EEvents         eEvent
                                , const tCIDLib::TCard4 c4Index1
                                , const tCIDLib::TCard4 c4Index2) :

    m_c4Index1(c4Index1)
    , m_c4Index2(c4Index2)
    , m_eEvent(eEvent)
{
}




// ---------------------------------------------------------------------------
//   CLASS: TCollectionBaseBase
//  PREFIX: col
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCollectionBase: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCollectionBase::BadStoredCount(const TClass& clsCol)
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcCol_BadStoredCount
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Format
        , clsCol
    );
}


// ---------------------------------------------------------------------------
//  TCollectionBase: Constructors and Destructor
// ---------------------------------------------------------------------------
TCollectionBase::~TCollectionBase()
{
    if (m_pmtxLock)
        delete m_pmtxLock;

    // If we registered a pub/sub topic, then clean that up
    if (m_ppstopReport)
    {
        try
        {
            delete m_ppstopReport;
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  TCollectionBase: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCollectionBase::bIsMTSafe() const
{
    return (m_pmtxLock != 0);
}

tCIDLib::TCard4 TCollectionBase::c4SerialNum() const
{
    if (m_pmtxLock)
    {
        TMtxLocker lockCol(m_pmtxLock);
        tCIDLib::TCard4 c4Ret = m_c4SerialNum;
        return c4Ret;
    }
    return m_c4SerialNum;
}


//
//  If we are at or beyond the limit, throw. Unlike the protected ones, this
//  is for client code to do checks.
//
tCIDLib::TVoid
TCollectionBase::CheckIsFull(   const   tCIDLib::TCard4     c4Limit
                                , const tCIDLib::TCh* const pszDescr)
{
    if (c4ElemCount() >= c4Limit)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCol_Full
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TString(pszDescr)
        );
    }
}


tCIDLib::EMTStates TCollectionBase::eMTState() const
{
    if (m_pmtxLock)
        return tCIDLib::EMTStates::Safe;
    return tCIDLib::EMTStates::Unsafe;
}


//
//  Sometimes we cannot know when the containing code has modified an element, since
//  it did so directly on the object. So this is public, allowing that containing code
//  to force a publish msg. If the collection knows it can call these itself.
//
tCIDLib::TVoid
TCollectionBase::PublishBlockChanged(const  tCIDLib::TCard4 c4At, const tCIDLib::TCard4 c4Count)
{
    if (m_ppstopReport)
    {
        m_ppstopReport->Publish
        (
            new TColPubSubInfo(TColPubSubInfo::EEvents::BlockChanged, c4At, c4Count)
        );
    }
}

tCIDLib::TVoid TCollectionBase::PublishElemChanged(const  tCIDLib::TCard4 c4At)
{
    if (m_ppstopReport)
        m_ppstopReport->Publish(new TColPubSubInfo(TColPubSubInfo::EEvents::ElemChanged, c4At));
}


//
//  Sometimes the containing code may make direct changes en masse and then want to
//  force nay subscribers to do a full reload, so we make this public. We just send
//  a reorder event.
//
tCIDLib::TVoid TCollectionBase::PublishReload()
{
    if (m_ppstopReport)
        m_ppstopReport->Publish(new TColPubSubInfo(TColPubSubInfo::EEvents::Reordered));
}




// ---------------------------------------------------------------------------
//  TCollectionBase: Public, static methods
// ---------------------------------------------------------------------------
//
//  A common helper to wait for data to show up in a collection, which uses
//  a wait list to manage waiters. This is something that is done by some
//  types of collections, so we want to get it out of the templatized code
//  as much as possible, since it's a good bit of code.
//
//  They will have locked themselves before they call us, and they pass us
//  a mutex locker. Upon return, we will have gotten lock back. We return
//  true if we got out because something was available, else false. If we
//  return true, the caller has the lock at that point and can take the
//  available one. If they tell us to throw if we don't get one, then we
//  won't return False, we'll throw.
//
//  They pass us a ref to themselves, so that we can check whether they are
//  empty or not. This is base collection functionality so we can work for
//  any type of collection.
//
tCIDLib::TBoolean
TCollectionBase::bWaitForData(          TMtxLocker&         mtxlQueue
                                , const TCollectionBase&    colSrc
                                , const tCIDLib::TCard4     c4WaitMSs
                                ,       TThreadWaitList&    twlWaitList
                                , const tCIDLib::TBoolean   bThrowIfNot)
{
    //
    //  Loop until we get some data. We have to do this because we can never guarantee
    //  that when we are awakened, that some other thread hasn't beat us to the punch
    //  and got the data already.
    //
    //  If they indicated wait forever, set the end time to zero, so we can quickly
    //  check for that below.
    //
    tCIDLib::TEncodedTime enctCur = TTime::enctNow();
    const tCIDLib::TEncodedTime enctEnd
    (
        (c4WaitMSs == kCIDLib::c4MaxCard) ? 0
                                          : (enctCur + (kCIDLib::enctOneMilliSec * c4WaitMSs))
    );

    //
    //  Loop while the collection is empty. Get the count first before we enter the
    //  loop, since we can avoid doing any work at all if not.
    //
    tCIDLib::TCard4 c4Count = colSrc.c4ElemCount();
    while (!c4Count)
    {
        //
        //  Assume wait forever, but if the end time is set, let's
        //  calc how many millis left to wait. It might be zero,
        //  and that's fine.
        //
        tCIDLib::TCard4 c4MillisLeft = kCIDLib::c4MaxCard;
        if (enctEnd)
        {
            c4MillisLeft = tCIDLib::TCard4
            (
                (enctEnd - enctCur) / kCIDLib::enctOneMilliSec
            );
        }

        //
        //  Put ourselves on the thread wait list. We use version of
        //  the method that takes a lock. It will release the lock
        //  after this thread is put on the wait list, but before this
        //  thread blocks. And it will reaquire the list upon wake up
        //  before it returns.
        //
        //  When we wake up, if the list isn't empty, then we can
        //  take it.
        //
        twlWaitList.bWaitOnList
        (
            mtxlQueue, kCIDLib::c4TWLReason_WaitData, c4MillisLeft
        );

        //
        //  If there's nothing in the queue, and we aren't just waiting
        //  forever, then we need to see if we are out of time and
        //  update the end time. If we have one, then we'll berak out
        //  above. If we don't have one yet, but we are waiting for
        //  ever, then we don't need to check for timeout.
        //
        c4Count = colSrc.c4ElemCount();
        if (!c4Count && enctEnd)
        {
            // If we are out of time, then give up
            enctCur = TTime::enctNow();
            if (enctCur >= enctEnd)
            {
                if (bThrowIfNot)
                {
                    facCIDLib().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kCIDErrs::errcCol_IsEmpty
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Timeout
                        , colSrc.clsIsA()
                    );
                }
                return kCIDLib::False;
            }
        }
    }
    return kCIDLib::True;
}



// ---------------------------------------------------------------------------
//  TCollectionBase: Hidden constructors and operators
// ---------------------------------------------------------------------------
TCollectionBase::TCollectionBase(const tCIDLib::EMTStates eMTSafe) :

    m_bInBlockMode(kCIDLib::False)
    , m_c4SerialNum(1)
    , m_pmtxLock(nullptr)
    , m_ppstopReport(nullptr)
{
    if (eMTSafe == tCIDLib::EMTStates::Safe)
        m_pmtxLock = new TMutex;
}


//
//  We don't copy any pub/sub topic if the source has one. The derived class has to
//  enable it for this instance if he wants reporting. If the source is thread safe
//  we make this one thread safe.
//
TCollectionBase::TCollectionBase(const TCollectionBase& colSrc) :

    m_bInBlockMode(kCIDLib::False)
    , m_c4SerialNum(1)
    , m_pmtxLock(nullptr)
    , m_ppstopReport(nullptr)
{
    if (colSrc.m_pmtxLock)
        m_pmtxLock = new TMutex;
}

TCollectionBase::TCollectionBase(TCollectionBase&& colSrc) :

    m_bInBlockMode(kCIDLib::False)
    , m_c4SerialNum(1)
    , m_pmtxLock(nullptr)
    , m_ppstopReport(nullptr)
{
    if (colSrc.m_pmtxLock)
        m_pmtxLock = new TMutex;
}


//
//  The caller should have locked. We bump our serial number. We prevent any messing with
//  the lock or topic stuff, otherwise we'd get just a copy and it would be a mess.
//
TCollectionBase& TCollectionBase::operator=(const TCollectionBase& colSrc)
{
    if (this != &colSrc)
    {
        //
        //  If we are in block mode, the have to just clear it. This shouldn't
        //  happen, but if it does there's not much else we can do.
        //
        if (m_bInBlockMode)
            m_bInBlockMode = kCIDLib::False;

        m_c4SerialNum++;
    }
    return *this;
}


// The caller should have locked. We bump both guy's serial numbers
TCollectionBase& TCollectionBase::operator=(TCollectionBase&& colSrc)
{
    if (this != &colSrc)
    {
        m_c4SerialNum++;
        colSrc.m_c4SerialNum++;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TCollectionBase: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCollectionBase::bCheckNewSize( const   tCIDLib::TCard4     c4New
                                , const tCIDLib::TCard4     c4Current
                                , const tCIDLib::TCh* const pszFile
                                , const tCIDLib::TCard4     c4Line) const
{
    if (c4New < c4Current)
    {
        facCIDLib().ThrowErr
        (
            pszFile
            , c4Line
            , kCIDErrs::errcCol_NewSizeTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(c4New)
            , TCardinal(c4Current)
        );
    }
    return (c4New > c4Current);
}


//
//  Our block mode janitor calls this on its dtor. We check that the block mode
//  flag is set. If not, that's an error. If it is, then we send a block event
//  if pub/sub is enabled, which is should be if they are using this stuff.
//
tCIDLib::TVoid
TCollectionBase::BlockModeEnd(  const   tCIDLib::TBoolean   bAddMode
                                , const tCIDLib::TCard4     c4Index
                                , const tCIDLib::TCard4     c4Count)
{
    // If not in a block mode, that's an error
    if (!m_bInBlockMode)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCol_NotInBlockMode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }

    // Clear the flag first, then report if pub/sub is enabled
    m_bInBlockMode = kCIDLib::False;
    if (m_ppstopReport)
    {
        // If index is max card, it's a non-contiguous one
        if (c4Index == kCIDLib::c4MaxCard)
        {
            m_ppstopReport->Publish(new TColPubSubInfo(TColPubSubInfo::EEvents::Reordered));
        }
         else
        {
            if (bAddMode)
                PublishBlockAdded(c4Index, c4Count);
            else
                PublishBlockRemoved(c4Index, c4Count);
        }
    }

}

tCIDLib::TVoid TCollectionBase::BlockModeStart()
{
    // If already in a block mode, that's an error
    if (m_bInBlockMode)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCol_InBlockMode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
        );
    }

    m_bInBlockMode = kCIDLib::True;
}


//
//  Bumps our serial number after locking, returns the new serial number. This is
//  called a lot, since we have to bump it on any change in our collection.
//
tCIDLib::TCard4 TCollectionBase::c4IncSerialNum()
{
    TMtxLocker lockCol(m_pmtxLock);
    m_c4SerialNum++;
    return m_c4SerialNum;
}


tCIDLib::TVoid
TCollectionBase::CheckAllocAndCount(const   tCIDLib::TCard4     c4Count
                                    , const tCIDLib::TCard4     c4Alloc
                                    , const tCIDLib::TCh* const pszFile
                                    , const tCIDLib::TCard4     c4Line)
{
    if (c4Count > c4Alloc)
    {
        facCIDLib().ThrowErr
        (
            pszFile
            , c4Line
            , kCIDErrs::errcCol_CurBeyondAlloc
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::ObjState
            , TCardinal(c4Count)
            , TCardinal(c4Alloc)
            , clsIsA()
        );
    }
}


tCIDLib::TVoid
TCollectionBase::CheckCursorValid(  const   TCursorBase&        cursToCheck
                                    , const tCIDLib::TCh* const pszFile
                                    , const tCIDLib::TCard4     c4Line) const
{
    if (!cursToCheck.bIsValid())
    {
        facCIDLib().ThrowErr
        (
            pszFile
            , c4Line
            , kCIDErrs::errcCol_CursorNotValid
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , clsIsA()
        );
    }
}


tCIDLib::TVoid
TCollectionBase::CheckIndex(const   tCIDLib::TCard4     c4ToCheck
                            , const tCIDLib::TCard4     c4CurCount
                            , const tCIDLib::TCh* const pszFile
                            , const tCIDLib::TCard4     c4Line) const
{
    if (c4ToCheck >= c4CurCount)
    {
        facCIDLib().ThrowErr
        (
            pszFile
            , c4Line
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4ToCheck)
            , clsIsA()
            , TCardinal(c4CurCount)
        );
    }
}


tCIDLib::TVoid
TCollectionBase::CheckOneNonAdopting(const  tCIDLib::EAdoptOpts eAdoptSrc
                                    , const tCIDLib::EAdoptOpts eAdoptTar
                                    , const tCIDLib::TCh* const pszFile
                                    , const tCIDLib::TCard4     c4Line) const
{
    if ((eAdoptSrc != tCIDLib::EAdoptOpts::NoAdopt)
    &&  (eAdoptTar != tCIDLib::EAdoptOpts::NoAdopt))
    {
        facCIDLib().ThrowErr
        (
            pszFile
            , c4Line
            , kCIDErrs::errcCol_NeitherNonAdopt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }
}


tCIDLib::TVoid
TCollectionBase::CheckReallocParms( const   tCIDLib::TCard4     c4NewSize
                                    , const tCIDLib::TCard4     c4CurCount
                                    , const tCIDLib::TBoolean   bKeepOld
                                    , const tCIDLib::TCh* const pszFile
                                    , const tCIDLib::TCard4     c4Line) const
{
    // If keeping old, new can't be less than current count
    if (bKeepOld && (c4NewSize < c4CurCount))
    {
        facCIDLib().ThrowErr
        (
            pszFile
            , c4Line
            , kCIDErrs::errcCol_CantKeepOld
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4NewSize)
            , TCardinal(c4CurCount)
        );
    }
}


tCIDLib::TVoid
TCollectionBase::ColIsEmpty(const   tCIDLib::TCh* const pszFile
                            , const tCIDLib::TCard4     c4Line) const
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_IsEmpty
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::AppError
        , clsIsA()
    );
}

tCIDLib::TVoid
TCollectionBase::ColIsFull( const tCIDLib::TCh* const   pszFile
                            , const tCIDLib::TCard4     c4Line
                            , const tCIDLib::TCard4     c4Max) const
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_TooManyNodes
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::OutResource
        , TCardinal(c4Max)
    );
}

tCIDLib::TVoid
TCollectionBase::DuplicateElem( const tCIDLib::TCh* const   pszFile
                                , const tCIDLib::TCard4     c4Line) const
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_DuplicateElem
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Already
    );
}


tCIDLib::TVoid
TCollectionBase::DuplicateKey(  const   TObject&            objKey
                                , const tCIDLib::TCh* const pszFile
                                , const tCIDLib::TCard4     c4Line) const
{
    // See if we can format it
    TString strKey;
    const MFormattable* pmftblKey = dynamic_cast<const MFormattable*>(&objKey);
    if (pmftblKey)
    {
        TTextStringOutStream strmOut(&strKey);
        strmOut << *pmftblKey;
    }
     else
    {
        strKey = L"???";
    }

    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_DuplicateKey
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Already
        , strKey
        , clsIsA()
    );
}


tCIDLib::EMTStates TCollectionBase::eMTState(const tCIDLib::EMTStates eState)
{
    if (eState == tCIDLib::EMTStates::Safe)
    {
        if (!m_pmtxLock)
        {
            m_pmtxLock = new TMutex;
            m_c4SerialNum++;
        }
    }
     else
    {
        if (m_pmtxLock)
        {
            delete m_pmtxLock;
            m_pmtxLock = nullptr;
            m_c4SerialNum++;
        }
    }
    return eState;
}


//
//  If not already enabled, we enable our publishing topic. They provide a topic path
//  they want us to use.
//
//  If we have already been enabled, we don't use their desired path, we give them
//  back the already set path. Else we use their path and give them back their path
//  as the actual path. If they care about the difference they can compare the two.
//
//  We have another variation that will throw if already set up.
//
tCIDLib::TVoid
TCollectionBase::EnablePublish(const TString& strDesired, TString& strActual)
{
    // Lock if we have a mutex
    TMtxLocker lockCol(m_pmtxLock);

    if (m_ppstopReport)
    {
        // Give them back the actual path so that they can use that
        strActual = m_ppstopReport->strPath();
        return;
    }

    //
    //  Looks ok so create the topic object. We store a pointer, so we have to actually
    //  copy construct a dynamic one from the returned one. This is fine since it is a
    //  reference counted object.
    //
    m_ppstopReport = new TPubSubTopic
    (
        TPubSubTopic::pstopCreateTopic(strDesired, TColPubSubInfo::clsThis())
    );

    // Given them back their desired path since we used that
    strActual = strDesired;
}

tCIDLib::TVoid TCollectionBase::EnablePublish(const TString& strTopicPath)
{
    TString strActual;
    EnablePublish(strTopicPath, strActual);

    if (!strTopicPath.bCompareI(strActual))
    {
        // It was already set up with some other path
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcCol_AlreadyPub
            , strActual
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
        );
    }
}


tCIDLib::TVoid
TCollectionBase::HashChanged(const  tCIDLib::TCh* const     pszFile
                            , const tCIDLib::TCard4         c4Line) const
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_HashChanged
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotSupported
    );
}


tCIDLib::TVoid
TCollectionBase::MustClearFirst(const   tCIDLib::TCh* const pszFile
                                , const tCIDLib::TCard4     c4Line) const
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_MustClearFirst
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotSupported
    );
}


tCIDLib::TVoid
TCollectionBase::KeyNotFound(const  tCIDLib::TCh* const pszFile
                            , const tCIDLib::TCard4     c4Line) const
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_KeyNotFound
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotFound
        , clsIsA()
    );
}


tCIDLib::TVoid
TCollectionBase::NodeNotFound(  const   tCIDLib::TCh* const pszFile
                                , const tCIDLib::TCard4     c4Line) const
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_NodeNotFound
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotFound
    );
}


tCIDLib::TVoid
TCollectionBase::NotMemberNode( const   tCIDLib::TCh* const pszFile
                                , const tCIDLib::TCard4     c4Line) const
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_NotMemberNode
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotFound
    );
}


tCIDLib::TVoid
TCollectionBase::NotMyCursor(const  TClass&             clsCursor
                            , const TClass&             clsCol
                            , const tCIDLib::TCh* const pszFile
                            , const tCIDLib::TCard4     c4Line) const
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_NotMyCursor
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::BadParms
        , clsCursor
        , clsCol
    );
}


tCIDLib::TVoid
TCollectionBase::NullNodeAdded( const   tCIDLib::TCh* const     pszFile
                                , const tCIDLib::TCard4         c4Line) const
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_NulNodeAdded
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::BadParms
    );
}


//
//  Various helpers the derived classes call to publish change events. If pub/sub
//  is not enabled on this collection, they will do nothing. This way the templatized
//  derived code has to do the least work possible.
//
//  The add/remove ones don't do anything if we are in block mode. See the class
//  header comments.
//
tCIDLib::TVoid TCollectionBase::PublishAdd(const  tCIDLib::TCard4 c4At)
{
    if (m_ppstopReport && !m_bInBlockMode)
        m_ppstopReport->Publish(new TColPubSubInfo(TColPubSubInfo::EEvents::ElemAdded, c4At));
}

tCIDLib::TVoid
TCollectionBase::PublishBlockAdded(const  tCIDLib::TCard4 c4At, const tCIDLib::TCard4 c4Count)
{
    if (m_ppstopReport)
    {
        m_ppstopReport->Publish
        (
            new TColPubSubInfo(TColPubSubInfo::EEvents::BlockAdded, c4At, c4Count)
        );
    }
}

tCIDLib::TVoid
TCollectionBase::PublishBlockRemoved(const  tCIDLib::TCard4 c4At
                                    , const tCIDLib::TCard4 c4Count)
{
    if (m_ppstopReport)
    {
        m_ppstopReport->Publish
        (
            new TColPubSubInfo(TColPubSubInfo::EEvents::BlockRemoved, c4At, c4Count)
        );
    }
}

tCIDLib::TVoid TCollectionBase::PublishClear()
{
    if (m_ppstopReport)
        m_ppstopReport->Publish(new TColPubSubInfo(TColPubSubInfo::EEvents::Cleared));
}

tCIDLib::TVoid
TCollectionBase::PublishMoved(  const   tCIDLib::TCard4 c4From
                                , const tCIDLib::TCard4 c4To)
{
    if (m_ppstopReport)
    {
        m_ppstopReport->Publish
        (
            new TColPubSubInfo(TColPubSubInfo::EEvents::ElemMoved, c4From, c4To)
        );
    }
}

tCIDLib::TVoid TCollectionBase::PublishRemove(const tCIDLib::TCard4 c4At)
{
    if (m_ppstopReport && !m_bInBlockMode)
        m_ppstopReport->Publish(new TColPubSubInfo(TColPubSubInfo::EEvents::ElemRemoved, c4At));
}

tCIDLib::TVoid TCollectionBase::PublishReorder()
{
    if (m_ppstopReport)
        m_ppstopReport->Publish(new TColPubSubInfo(TColPubSubInfo::EEvents::Reordered));
}

tCIDLib::TVoid
TCollectionBase::PublishSwap(const  tCIDLib::TCard4 c4At1
                            , const tCIDLib::TCard4 c4At2)
{
    if (m_ppstopReport)
    {
        m_ppstopReport->Publish
        (
            new TColPubSubInfo(TColPubSubInfo::EEvents::Swapped, c4At1, c4At2)
        );
    }
}


// A debug helper that will throw a source too big exception
tCIDLib::TVoid
TCollectionBase::SrcTooBig( const   tCIDLib::TCh* const pszFile
                            , const tCIDLib::TCard4     c4Line
                            , const tCIDLib::TCard4     c4SrcElems
                            , const tCIDLib::TCard4     c4TarMax) const
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_SrcTooBig
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::OutResource
        , TCardinal(c4SrcElems)
        , TCardinal(c4TarMax)
    );
}


tCIDLib::TVoid
TCollectionBase::Unsupported(const  tCIDLib::TCh* const pszFile
                            , const tCIDLib::TCard4     c4Line
                            , const tCIDLib::TCh* const pszOpName) const
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_Unsupported
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotSupported
        , TString(pszOpName)
    );
}


tCIDLib::TVoid
TCollectionBase::ZeroSize(  const   tCIDLib::TCh* const pszFile
                            , const tCIDLib::TCard4     c4Line) const
{
    facCIDLib().ThrowErr
    (
        pszFile
        , c4Line
        , kCIDErrs::errcCol_ZeroSize
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::AppError
    );
}


// ---------------------------------------------------------------------------
//   CLASS: TFundColBase
//  PREFIX: col
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFundColBase: Constructors and Destructor
// ---------------------------------------------------------------------------
TFundColBase::~TFundColBase()
{
}


// ---------------------------------------------------------------------------
//  TFundColBase: Hidden constructors and operators
// ---------------------------------------------------------------------------
TFundColBase::TFundColBase(const tCIDLib::EMTStates eMTSafe) :

    TCollectionBase(eMTSafe)
{
}



// ---------------------------------------------------------------------------
//   CLASS: TColBlockModeJan
//  PREFIX: jan
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TColBlockModeJan: Constructors and destructor
// -------------------------------------------------------------------
TColBlockModeJan::TColBlockModeJan(         TCollectionBase* const  pcolTar
                                    , const tCIDLib::TBoolean       bLoad
                                    , const tCIDLib::TCard4         c4Start) :

    m_bLoad(bLoad)
    , m_c4Count(0)
    , m_c4Start(c4Start)
    , m_pcolTar(pcolTar)
{
    m_pcolTar->BlockModeStart();
}

TColBlockModeJan::~TColBlockModeJan()
{
    if (m_pcolTar)
        m_pcolTar->BlockModeEnd(m_bLoad, m_c4Start, m_c4Count);
}


// -------------------------------------------------------------------
//  TColBlockModeJan: Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TVoid TColBlockModeJan::IncCount()
{
    m_c4Count++;
}
