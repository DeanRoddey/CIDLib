//
// FILE NAME: CIDLib_PubSub.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/15/2018
//
// COPYRIGHT: 2018..2018, Charmed Quark Systems, Ltd
//
//  This file and its contents are the property of Charmed Quark Systems, Ltd. It
//  forms part of the internal implementation of the Charmed Quark Controller
//  product. It contains proprietary source code and is not to be distributed
//  publically in any way, shape, or form.
//
// DESCRIPTION:
//
//  This is the implementation file for the publish/subscribe framework.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macro
// ---------------------------------------------------------------------------
RTTIDecls(TPubSubMsg, TObject)
RTTIDecls(TPubSubAsyncSub,TObject)
RTTIDecls(TPubSubTopic, TObject)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDLib_PubSub
{
    using TTopicList    = TKeyedHashSet<TPubSubTopic, TString, TStringKeyOps>;
    using TSubList      = TRefVector<MPubSubscription>;

    //
    //  We need a mutex for sync, and we fault it in. Everyone accesses it via the
    //  method below which faults it in, along with some other stuff below.
    //
    TMutex* pmtxSync();

    //
    //  The lists are faulted in with the mutex above. We support a max of 32K
    //  subscribers, and 8192 topics (as set below when we fault in these lists.)
    //
    TAtomicFlag             atomInit;
    tCIDLib::TCard4         c4NextId = 1;
    TSubList*               pcolSubList;
    TTopicList*             pcolTopicList;
    tCIDLib::TEncodedTime   enctNextScavenge;

    // The max size we'll let the msg queue grow to before we start dropping them
    const tCIDLib::TCard4   c4MaxMsgQSz = 4192;
}


// This is used to access the sync mutex and fault some other bits in as well
TMutex* CIDLib_PubSub::pmtxSync()
{
    static TMutex mtxLock;

    if (!CIDLib_PubSub::atomInit)
    {
        TMtxLocker mtxlSync(&mtxLock);
        if (!CIDLib_PubSub::atomInit)
        {
            pcolSubList = new TSubList(tCIDLib::EAdoptOpts::NoAdopt, 256);
            pcolTopicList = new TTopicList
            (
                109, TStringKeyOps(kCIDLib::False), &TPubSubTopic::strKey
            );
            enctNextScavenge = TTime::enctNowPlusMins(5);

            CIDLib_PubSub::atomInit.Set();
        }
    }

    return &mtxLock;
}



// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------

// A comparator for the subscriptions, by subscriber id
tCIDLib::ESortComps eCompSubId( const   tCIDLib::TCard4&    c4Key
                                , const MPubSubscription&   mpsubComp)
{
    const tCIDLib::TCard4 c4Comp = mpsubComp.m_c4SubscriberId;
    if (c4Key < c4Comp)
        return tCIDLib::ESortComps::FirstLess;
    else if (c4Key > c4Comp)
        return tCIDLib::ESortComps::FirstGreater;

    return tCIDLib::ESortComps::Equal;
}


//
//  We run through the list of topics and remove any that have a ref count of 1, which
//  means we are the only ones still referencing them. We assume the caller has locked.
//  Subscribers remove themselves.
//
static tCIDLib::TVoid ScavengePubs()
{
    // Update the next scavenge time stamp first
    CIDLib_PubSub::enctNextScavenge = TTime::enctNowPlusMins(5);

    // For the topics we have to do a cursor
    CIDLib_PubSub::TTopicList::TNCCursor cursTopics(CIDLib_PubSub::pcolTopicList);
    while (cursTopics.bIsValid())
    {
        if (cursTopics->c4RefCount() < 2)
            CIDLib_PubSub::pcolTopicList->RemoveAt(cursTopics);
        else
            ++cursTopics;
    }
}


//
//  The topic path is only used in debug mode, to make sure that the subscriber
//  we find at the indicated id really is a subcriber for the indicated topic. The
//  calling topic must lock across the whole send operation, so that no one else
//  is modifying the subscriber list.
//
static tCIDLib::TVoid SendMsg(  const   tCIDLib::TCard4 c4SubId
                                , const TPubSubMsg&     psmsgToSend)
{
    //
    //  If it's been a while since we last scavenged, then do that. He assumes the
    //  caller has locked, but we do as well, so we should be ok.
    //
    if (TTime::enctNow() > CIDLib_PubSub::enctNextScavenge)
        ScavengePubs();

    // Find the indicated subcriber
    tCIDLib::TCard4 c4At;
    MPubSubscription* pmpsubTar = CIDLib_PubSub::pcolSubList->pobjKeyedBinarySearch
    (
        c4SubId, eCompSubId, c4At
    );

    if (!pmpsubTar)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPubSub_IdNotFound
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , TString(L"subscription")
            , TCardinal(c4SubId)
        );
    }

    // In debug mode, make sure ths subscriber is subscribed to this topic
    #if CID_DEBUG_ON
    if (!pmpsubTar->bIsSubscribedToTopic(psmsgToSend.strSrcTopicPath()))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPubSub_WrongTopicId
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , TCardinal(c4SubId)
            , psmsgToSend.strSrcTopicPath()
        );
    }
    #endif

    pmpsubTar->ProcessPubMsg(psmsgToSend);
}





// ---------------------------------------------------------------------------
//  CLASS: TPSMsg
// PREFIX: psmsg
//
//  Our internal wrapper around msgs. The public message class creates a counted pointer
//  around one of these, which will clean us up once the last msg object that refs this
//  msg is released. We need this extra layer so we can store some other info that is
//  not part of the msg object.
// ---------------------------------------------------------------------------
class TPSMsg
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TPSMsg() = delete;

        TPSMsg( const   TObject* const      pobjToAdopt
                , const tCIDLib::TCard4     c4MsgId
                , const TString&            strTopicPath) :

            m_c4MsgId(c4MsgId)
            , m_pobjMsg(pobjToAdopt)
            , m_strTopicPath(strTopicPath)
        {
            CIDAssert(pobjToAdopt != nullptr, L"Pub/Sub msg object cannot be null");
        }

        TPSMsg(const TPSMsg&) = delete;

        ~TPSMsg()
        {
            // When we go we take our object with us
            try
            {
                delete m_pobjMsg;
            }

            catch(TError& errToCatch)
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            catch(...)
            {
            }
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPSMsg& operator=(const TPSMsg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4MsgId() const
        {
            return m_c4MsgId;
        }

        const TObject& objMsg() const
        {
            return *m_pobjMsg;
        }

        const TString& strTopicPath() const
        {
            return m_strTopicPath;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4MsgId
        //      Our topic assigns each msg an id from a monotonic counter.
        //
        //  m_pobjMsg
        //      The actual msg object. We own it and delete it when we are deleted.
        //
        //  m_strTopicPath
        //      The id of the topic that issued the msg
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4MsgId;
        const TObject*      m_pobjMsg;
        TString             m_strTopicPath;
};



// ---------------------------------------------------------------------------
//  CLASS: TPSTopic
// PREFIX: pstopic
//
//  The public topic class keeps a counted pointer for one of these. Then the last
//  public reference has been dropped the counted point will kill us.
// ---------------------------------------------------------------------------
class TPSTopic
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TPSTopic() = delete;

        TPSTopic(const  TString&            strPath
                , const TClass&             clsMsgType
                , const tCIDLib::TCard4     c4TopicId) :

            m_c4NextMsgId(1)
            , m_c4TopicId(c4TopicId)
            , m_clsMsgType(clsMsgType)
            , m_strPath(strPath)
        {
        }

        TPSTopic(const TPSTopic&) = delete;

        ~TPSTopic()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPSTopic& operator=(const TPSTopic&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddSubscriber(const tCIDLib::TCard4 c4SubId)
        {
            if (!m_fcolSubList.bElementPresent(c4SubId))
                m_fcolSubList.c4AddElement(c4SubId);
        }

        tCIDLib::TCard4 c4GetNextMsgId()
        {
            return m_c4NextMsgId++;
        }

        tCIDLib::TCard4 c4SubCount() const
        {
            return m_fcolSubList.c4ElemCount();
        }

        tCIDLib::TCard4 c4TopicId() const
        {
            return m_c4TopicId;
        }

        tCIDLib::TVoid RemoveSubscriber(const tCIDLib::TCard4 c4SubId)
        {
            m_fcolSubList.bDeleteIfPresent(c4SubId);
        }

        const TString& strPath() const
        {
            return m_strPath;
        }

        // We assume the caller has locked, and it really is only called from one place
        tCIDLib::TVoid SendMsg(TObject* const pobjToAdopt)
        {
            // Make sure it's the right msg type
            if (!pobjToAdopt->bIsDescendantOf(m_clsMsgType))
            {
                // Clean up the object since we own it
                delete pobjToAdopt;
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcPubSub_MsgCast
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::TypeMatch
                    , pobjToAdopt->clsIsA().strClassName()
                    , m_clsMsgType.strClassName()
                );
            }

            // Optimize for no subscribers
            const tCIDLib::TCard4 c4Count = m_fcolSubList.c4ElemCount();
            if (!c4Count)
            {
                // We own the object, so clean it up
                delete pobjToAdopt;
                return;
            }

            // Create a msg wrapper, giving it our next available msg id
            TPubSubMsg psmsgNew(new TPSMsg(pobjToAdopt, m_c4NextMsgId++, m_strPath));

            //
            //  The object is owned now so if we fail it will get cleaned up, so let's
            //  add to each subscriber's queue.
            //
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                ::SendMsg(m_fcolSubList[c4Index], psmsgNew);
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4NextMsgId
        //      We provide for a monotonic sequence number for any msgs sent via this
        //      topic, so that the subscriber can knwo if he ever misses any.
        //
        //  m_c4TopicId
        //      A unique id assigned to us by the facility object when he created
        //      us.
        //
        //  m_clsMsgType
        //      The class type of the messages this topic provides
        //
        //  m_fcolSubList
        //      A list of the subscriber ids of all our subscribers. The facility
        //      class will use this to know what subscribers to pass our msgs to.
        //
        //  m_strPath
        //      The unique path for the topic.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4NextMsgId;
        const tCIDLib::TCard4   m_c4TopicId;
        TClass                  m_clsMsgType;
        tCIDLib::TCardList      m_fcolSubList;
        TString                 m_strPath;
};





// ---------------------------------------------------------------------------
//  CLASS: TPubSubMsg
// PREFIX: psmsg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPubSubMsg: Constructors and Destructor.
// ---------------------------------------------------------------------------
TPubSubMsg::TPubSubMsg(TPSMsg* const psmsgImpl) :

    m_cptrMsgImpl(psmsgImpl)
{
}

TPubSubMsg::TPubSubMsg(const TPubSubMsg& psmsgSrc) :

    m_cptrMsgImpl(psmsgSrc.m_cptrMsgImpl)
{
}

TPubSubMsg::~TPubSubMsg()
{
    // Our counted pointer will be destroyed and will clean up the msg if appropriate
}


// ---------------------------------------------------------------------------
//  TPubSubMsg: Public operators
// ---------------------------------------------------------------------------
TPubSubMsg& TPubSubMsg::operator=(const TPubSubMsg& psmsgSrc)
{
    if (this != &psmsgSrc)
    {
        m_cptrMsgImpl = psmsgSrc.m_cptrMsgImpl;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TPubSubMsg: Public, non-virutal methods
// ---------------------------------------------------------------------------
const TObject& TPubSubMsg::objMsg() const
{
    if (!m_cptrMsgImpl)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPubSub_NotReady
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , TString(L"message")
        );
    }
    return m_cptrMsgImpl->objMsg();
}

const TString& TPubSubMsg::strSrcTopicPath() const
{
    if (!m_cptrMsgImpl)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPubSub_NotReady
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , TString(L"message")
        );
    }
    return m_cptrMsgImpl->strTopicPath();
}


// ---------------------------------------------------------------------------
//  TPubSubMsg: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Used to type check that the msg object we have is of the type that the caller is
//  expecting. It's used by a templatized method we provide to get to the msg object.
//
tCIDLib::TVoid TPubSubMsg::CheckMsgType(const TClass& clsToCheck) const
{
    if (!m_cptrMsgImpl
    ||  !m_cptrMsgImpl->objMsg().bIsDescendantOf(clsToCheck))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPubSub_MsgCast
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
            , m_cptrMsgImpl ? m_cptrMsgImpl->objMsg().clsIsA().strClassName()
                            : TString(L"???")
            , clsToCheck.strClassName()
        );
    }
}



// ---------------------------------------------------------------------------
//  CLASS: MPubSubscription
// PREFIX: mpsub
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  MPubSubscription: Constructors and destructor
// ---------------------------------------------------------------------------

//
//  Unsubscribe from any topics we are subscribed to.
//
//  DON'T zero the subscriber id until after we search for ourself in the subscriber list,
//  since otherwise we won't find and remove ourself.
//
MPubSubscription::~MPubSubscription()
{
    TMtxLocker mtxlSync(CIDLib_PubSub::pmtxSync());
    if (m_c4SubscriberId)
    {
        try
        {
            // Remove us from the subscriber list of any topics we are subscribed to
            TPubSubTopic* pstopFind;
            if (m_kllstTopicPaths.bResetCursor())
            {
                TString* pstrCurTopic = nullptr;
                while (m_kllstTopicPaths.bNext(pstrCurTopic))
                {
                    try
                    {
                        // Find the topic. If active, then let's remove ourself from his list
                        pstopFind = CIDLib_PubSub::pcolTopicList->pobjFindByKey(*pstrCurTopic);
                        if (pstopFind)
                            pstopFind->RemoveSubscriber(m_c4SubscriberId);
                    }

                    catch(TError& errToCatch)
                    {
                        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                        TModule::LogEventObj(errToCatch);
                    }
                }
            }

            // Now find us in the subscriber list and remove us if found
            tCIDLib::TCard4 c4At;
            if (CIDLib_PubSub::pcolSubList->pobjKeyedBinarySearch(m_c4SubscriberId, eCompSubId, c4At))
                CIDLib_PubSub::pcolSubList->RemoveAt(c4At);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        catch(...)
        {
        }

        m_c4SubscriberId = 0;
    }
}


// ---------------------------------------------------------------------------
//  MPubSubscription: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Returns whether we are subscribed to the indicated topic
tCIDLib::TBoolean MPubSubscription::bIsSubscribedToTopic(const TString& strTopicPath) const
{
    if (!m_kllstTopicPaths.bResetCursor())
        return kCIDLib::False;

    TString* pstrCurTopic;
    while (m_kllstTopicPaths.bNext(pstrCurTopic))
    {
        if (pstrCurTopic->bCompareI(strTopicPath))
            return kCIDLib::True;
    }
    return kCIDLib::False;
}

// No need to lock to just return our id. If zero we are not subscribed to anything yet
tCIDLib::TCard4 MPubSubscription::c4SubscriberId() const
{
    return m_c4SubscriberId;
}


// ---------------------------------------------------------------------------
//  MPubSubscription: Hidden constructors and operators
// ---------------------------------------------------------------------------
MPubSubscription::MPubSubscription() :

    m_c4SubscriberId(0)
{
}

// We don't duplicate the subscription. The new object has to do its own
MPubSubscription::MPubSubscription(const MPubSubscription&) :

    m_c4SubscriberId(0)
{
}

// We don't duplicate the subscriptions. The new object has to do its own
MPubSubscription& MPubSubscription::operator=(const MPubSubscription&)
{
    return *this;
}



// ---------------------------------------------------------------------------
//  MPubSubscription: Protected, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This will subscribe us to the passed topic. If bMustExist is true, and the topic is
//  not already published, then we will throw.
//
tCIDLib::TVoid
MPubSubscription::SubscribeToTopic( const   TString&            strTopicPath
                                    , const tCIDLib::TBoolean   bMustExist)
{
    TMtxLocker mtxlSync(CIDLib_PubSub::pmtxSync());

    //
    //  Invoke a scavenger pass. This isn't something that happens very often, so it's
    //  fine to do one each time. This way we won't see any dead topics below and end
    //  up subscribing to one.
    //
    ScavengePubs();

    //
    //  Find the topic if it already exists. Since we did a scavenge above, we won't
    //  find any abandoned ones left in the list.
    //
    TPubSubTopic* pstopFind = CIDLib_PubSub::pcolTopicList->pobjFindByKey(strTopicPath);

    // If not and they say it must, then we give up
    if (!pstopFind && bMustExist)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPubSub_TopicNotFound
            , strTopicPath
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strTopicPath
        );
    }

    //
    //  Give this guy a subscription id if not done yet. And add us to the subscriber
    //  list.
    //
    if (!m_c4SubscriberId)
    {
        m_c4SubscriberId = CIDLib_PubSub::c4NextId++;
        CIDLib_PubSub::pcolSubList->Add(this);
    }

    // If not already subscribed to this topic, then add it to our path list
    if (!bIsSubscribedToTopic(strTopicPath))
        m_kllstTopicPaths.pobjAddNew(new TString(strTopicPath));

    // If the topic already existed, add us to his subscriber list. He'll ignore dups
    if (pstopFind)
        pstopFind->AddSubscriber(m_c4SubscriberId);
}


tCIDLib::TVoid
MPubSubscription::UnsubscribeFromTopic(const TString& strTopicPath)
{
    TMtxLocker mtxlSync(CIDLib_PubSub::pmtxSync());
    try
    {
        //
        //  Find this topic in our list. If not in there, then nothing to do. If in
        //  there, remove it.
        //
        if (m_kllstTopicPaths.bResetCursor())
        {
            TString* pstrCur = nullptr;
            while (m_kllstTopicPaths.bNext(pstrCur))
            {
                if (pstrCur->bCompareI(strTopicPath))
                    break;
            }
        }

        // If we never found it, and debugging, then throw
        if (m_kllstTopicPaths.bAtEnd())
        {
            #if CID_DEBUG_ON
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcPubSub_WrongTopicId
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
                , TCardinal(m_c4SubscriberId)
                , strTopicPath
            );
            #endif
        }

        // Remove this entry
        m_kllstTopicPaths.RemoveCurrent();

        // Now find the topic and remove us from its subscriber list if found
        TPubSubTopic* pstopFind = CIDLib_PubSub::pcolTopicList->pobjFindByKey(strTopicPath);
        if (pstopFind)
            pstopFind->RemoveSubscriber(m_c4SubscriberId);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }
}


// ---------------------------------------------------------------------------
//  MPubSubscription: Private, non-virtual methods
// ---------------------------------------------------------------------------

// See if we have ever been set up
tCIDLib::TVoid MPubSubscription::CheckSubIsReady() const
{
    if (!m_c4SubscriberId)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPubSub_NotReady
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , TString(L"subscription")
        );
    }
}


// ---------------------------------------------------------------------------
//  CLASS: TPubSubAsyncSub
// PREFIX: psasub
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPubSubAsyncSub: Constructors and destructor
// ---------------------------------------------------------------------------
TPubSubAsyncSub::TPubSubAsyncSub(const tCIDLib::TBoolean bCreateEvent) :

    m_c4BlockingCnt(0)
    , m_colMsgs(tCIDLib::EMTStates::Safe)
    , m_pevWait(nullptr)
{
    //
    //  If an event was requested, create it. Defaults of triggered initial
    //  state and manual mode are what we want.
    //
    if (bCreateEvent)
        m_pevWait = new TEvent();
}

TPubSubAsyncSub::~TPubSubAsyncSub()
{
    try
    {
        m_colMsgs.RemoveAll();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }

    catch(...)
    {
    }

    // If an event was created, destroy it
    if (m_pevWait)
    {
        try
        {
            delete m_pevWait;
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        catch(...)
        {
        }

        m_pevWait = nullptr;
    }
}


// ---------------------------------------------------------------------------
//  TPubSubAsyncSub: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TPubSubAsyncSub::bGetNextMsg(TPubSubMsg& psmsgToFill, const tCIDLib::TCard4 c4WaitMSs)
{
    if (m_colMsgs.bGetNext(psmsgToFill, c4WaitMSs))
    {
        //
        //  If we have an event, lock and update the event appropriately. We can't
        //  lock before the read since we want to let multiple threads potentially
        //  get into waiting call (the queue is thread safe so that's fine.)
        //
        //  So we have to do this after the fact. Even if the above emptied the queue,
        //  it's possible that by the time we get here and lock, another msg could
        //  have been added.
        //
        if (m_pevWait)
        {
            TMtxLocker mtxlSync(m_colMsgs.pmtxLock());
            if (m_colMsgs.bIsEmpty())
                m_pevWait->Reset();
        }
        return kCIDLib::True;
    }

    //
    //  Nothing gotten so the our state couldn't have changed so no need to do any
    //  event stuff.
    //
    return kCIDLib::False;
}


tCIDLib::TCard4 TPubSubAsyncSub::c4MsgCount() const
{
    return m_colMsgs.c4ElemCount();
}


TEvent& TPubSubAsyncSub::evWait()
{
    if (!m_pevWait)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPubSub_NoEvent
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
        );
    }
    return *m_pevWait;
}

tCIDLib::TVoid TPubSubAsyncSub::FlushMsgs()
{
    //
    //  Though the queue is thread safe, we have to deal with the event if we have
    //  one. So lock, remove all, then reset the event. That way no one can get another
    //  msg into the queue before we reset.
    //
    TMtxLocker mtxlSync(m_colMsgs.pmtxLock());
    m_colMsgs.RemoveAll();

    if (m_pevWait)
        m_pevWait->Reset();
}


tCIDLib::TVoid
TPubSubAsyncSub::SubscribeTo(const  TString&            strTopicPath
                            , const tCIDLib::TBoolean   bMustExist)
{
    // Pass to our mixin class
    SubscribeToTopic(strTopicPath, bMustExist);
}


tCIDLib::TVoid TPubSubAsyncSub::UnsubscribeFrom(const TString& strTopicPath)
{
    // Pass to our mixin class
    UnsubscribeFromTopic(strTopicPath);
}


// ---------------------------------------------------------------------------
//  TPubSubAsyncSub: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TPubSubAsyncSub::ProcessPubMsg(const TPubSubMsg& psmsgIn)
{
    //
    //  This one is tricky if we have an event. If we go from empty to having
    //  content msgs, then we need to trigger the event. So even though the collection
    //  is thread safe, we have to lock first, get the count, add, then trigger if
    //  it was initially empty.
    //
    //  We also need to deal with the queue being full. We just drop the incoming
    //  msg. The client can know this if desired by watching the msg sequence numbers,
    //  which will now have a gap.
    //
    TMtxLocker mtxlSync(m_colMsgs.pmtxLock());
    const tCIDLib::TCard4 c4OrgCount = m_colMsgs.c4ElemCount();
    if (c4OrgCount >= CIDLib_PubSub::c4MaxMsgQSz)
    {
        facCIDLib().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPubSub_SubQFull
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
        );
        return;
    }

    // Add it to our queue
    m_colMsgs.objAdd(psmsgIn);

    // If we were empty and now we aren't, then trigger the event if we have one
    if (!c4OrgCount && m_pevWait)
        m_pevWait->Trigger();
}




// ---------------------------------------------------------------------------
//  CLASS: TPubSubTopic
// PREFIX: pstop
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TPubSubTopic: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TPubSubTopic::bTopicExists(const TString& strTopicPath)
{
    TMtxLocker mtxlSync(CIDLib_PubSub::pmtxSync());
    TPubSubTopic* pstopFind = CIDLib_PubSub::pcolTopicList->pobjFindByKey(strTopicPath);

    //
    //  If we found it, but the ref count is less than 2, then it has abandoned
    //  so remove it.
    //
    if (pstopFind && (pstopFind->c4RefCount() < 2))
    {
        pstopFind = nullptr;
        CIDLib_PubSub::pcolTopicList->bRemoveKey(strTopicPath);
    }

    return (pstopFind != nullptr);
}


//
//  This is called to create a topic. Only one topic can exist for a given path, so
//  we throw if it already exists. That means that they have yet to destroy the one
//  already gotten. We return by value since this is a ref counting wrapper around
//  the actual topic object. We add it to our lists of topics, which leave the count
//  at 1 until the caller stores a copy.
//
TPubSubTopic
TPubSubTopic::pstopCreateTopic(const TString& strTopicPath, const TClass& clsMsgType)
{
    TMtxLocker mtxlSync(CIDLib_PubSub::pmtxSync());

    //
    //  Invoke a scavenger pass. This isn't something that happens very often, so it's
    //  fine to do one each time.
    //
    ScavengePubs();

    //
    //  See if we already have this guy. We removed any above that were abandoned,
    //  so if it's in the list it's still active and a dup.
    //
    TPubSubTopic* pstopFind = CIDLib_PubSub::pcolTopicList->pobjFindByKey(strTopicPath);
    if (pstopFind)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPubSub_TopicExists
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , strTopicPath
        );
    }

    //
    //  Create a new one and add it to our list and return it. Of course adding it to
    //  our list creates a new copy that goes into the list, but the ref counted bit
    //  inside is the same, so we can just return our local copy. It will be assigned
    //  the next available unique topic id.
    //
    TPubSubTopic pstopRet(strTopicPath, clsMsgType);
    CIDLib_PubSub::pcolTopicList->objAdd(pstopRet);

    //
    //  Go through our current list of subscribers and see if any are for this
    //  topic, and just got registered before him. If so, we need to add them to
    //  his list of subscribers.
    //
    const tCIDLib::TCard4 c4Count = CIDLib_PubSub::pcolSubList->c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        MPubSubscription* pmpsubCur = CIDLib_PubSub::pcolSubList->pobjAt(c4Index);
        if (pmpsubCur->bIsSubscribedToTopic(strTopicPath))
            pstopRet.AddSubscriber(pmpsubCur->c4SubscriberId());
    }

    return pstopRet;
}


const TString& TPubSubTopic::strKey(const TPubSubTopic& pstopSrc)
{
    return pstopSrc.strPath();
}


// ---------------------------------------------------------------------------
//  TPubSubTopic: Constructors and Destructor.
// ---------------------------------------------------------------------------
TPubSubTopic::TPubSubTopic()
{
}

TPubSubTopic::TPubSubTopic(const TString& strPath, const TClass& clsMsgType) :

    m_cptrImpl(new TPSTopic(strPath, clsMsgType, CIDLib_PubSub::c4NextId++))
    , m_strTopicPath(strPath)
{
}

TPubSubTopic::TPubSubTopic(const TPubSubTopic& pstopSrc) :

    m_cptrImpl(pstopSrc.m_cptrImpl)
{
    // Make sure this is after our impl has been updated
    if (m_cptrImpl)
        m_strTopicPath = m_cptrImpl->strPath();
}

TPubSubTopic::~TPubSubTopic()
{
    // Our counted pointer will clean up the impl object here if appropriate
}


// ---------------------------------------------------------------------------
//  TPubSubTopic: Public operators
// ---------------------------------------------------------------------------
TPubSubTopic& TPubSubTopic::operator=(const TPubSubTopic& pstopSrc)
{
    if (this != &pstopSrc)
    {
        m_cptrImpl = pstopSrc.m_cptrImpl;

        //
        //  If we got a new impl (not null), then get a copy of the topic path out
        //  else clear our copy.
        //
        if (m_cptrImpl)
            m_strTopicPath = m_cptrImpl->strPath();
        else
            m_strTopicPath.Clear();
    }
    return *this;
}


// -------------------------------------------------------------------
//  TPubSubTopic: Public, non-virtual methods
// -------------------------------------------------------------------

// See if we have an implementation object set, which means we are published
tCIDLib::TBoolean TPubSubTopic::bIsPublished() const
{
    return tCIDLib::TBoolean(m_cptrImpl);
}


// This is used by the scavenger thread
tCIDLib::TCard4 TPubSubTopic::c4RefCount() const
{
    CheckReady();
    return m_cptrImpl.c4StrongCount();
}


// This will access the subscriber id collection in the impl, so we need to lock
tCIDLib::TCard4 TPubSubTopic::c4SubCount() const
{
    CheckReady();

    TMtxLocker mtxlSync(CIDLib_PubSub::pmtxSync());
    return m_cptrImpl->c4SubCount();
}


//
//  No need to lock. The impl can't go away since we have a ref, and it's just returning
//  an unchanging simple value. Since this is accessed a lot, it's worth the opt.
//
tCIDLib::TCard4 TPubSubTopic::c4TopicId() const
{
    CheckReady();
    return m_cptrImpl->c4TopicId();
}


//
//  We just drop our reference to the internal object. If we are the last owner
//  it will get cleaned up.
//
tCIDLib::TVoid TPubSubTopic::DropTopic()
{
    TMtxLocker mtxlSync(CIDLib_PubSub::pmtxSync());
    m_cptrImpl.DropRef();
}


//
//  This is used to send a message to any current subscribers. We pass it on to the
//  impl. He'll create a msg wrapper and put it into any subscriber queues.
//
//  We need to lock across this whole operation so that the subscription list cannot
//  change while we are doing this.
//
tCIDLib::TVoid TPubSubTopic::Publish(TObject* const pobjToAdopt)
{
    TMtxLocker mtxlSync(CIDLib_PubSub::pmtxSync());
    CheckReady();
    m_cptrImpl->SendMsg(pobjToAdopt);
}


// -------------------------------------------------------------------
//  TPubSubTopic: Protected, non-virtual methods
// -------------------------------------------------------------------

//
//  We assume the caller locks. This is called from the topic and subscription
//  static creation methods.
//
tCIDLib::TVoid TPubSubTopic::AddSubscriber(const tCIDLib::TCard4 c4SubId)
{
    // Should always be an impl object, so just a debug mode check
    CIDAssertX
    (
        tCIDLib::TBoolean(m_cptrImpl)
        , L"Topic %(1) has no impl at add subscriber"
        , m_strTopicPath
    );

    TMtxLocker mtxlSync(CIDLib_PubSub::pmtxSync());
    return m_cptrImpl->AddSubscriber(c4SubId);
}


//
//  We assum the caller locks. This is called from our scaveger local function when
//  it drops a subscription.
//
tCIDLib::TVoid TPubSubTopic::RemoveSubscriber(const tCIDLib::TCard4 c4SubId)
{
    // Should always be an impl object, so just a debug mode check
    CIDAssertX
    (
        tCIDLib::TBoolean(m_cptrImpl)
        , L"Topic %(1) has no impl at remove subscriber"
        , m_strTopicPath
    );

    TMtxLocker mtxlSync(CIDLib_PubSub::pmtxSync());
    return m_cptrImpl->RemoveSubscriber(c4SubId);
}


// ---------------------------------------------------------------------------
//  TPubSubTopic: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TPubSubTopic::CheckReady() const
{
    if (!m_cptrImpl)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPubSub_NotReady
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotReady
            , TString(L"topic")
        );
    }
}
