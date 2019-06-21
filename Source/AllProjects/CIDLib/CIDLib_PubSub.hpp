//
// FILE NAME: CIDLib_PubSub.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/15/2018
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
//  This is the header for the CIDLib_PubSub.Cpp file. This file implements a framework
//  for intra-program publish and subscribe. We define some public classes which are
//  mostly just reference counters around a matching set of internal classes.
//  Ids
//
//  Each mesage, topic, and subscriber is given a unique numerical id. The id for each
//  subscriber is added to the topic it subscribes to. That's how we link topics to
//  subscribers.
//
//  Msg ids are monotonic for each topic, so subscribers can know if they have somehow
//  missed msgs if they want, by remembering the id of the last msg they read. This of
//  course is only meaningful for async subscribers, where there's a chance that
//  the queue may overflow.
//
//
//  Locking
//
//  We have an internal mutex we use for sync when updating the subscriber or publisher
//  lists. For topics the public wrapper does any required locking, so the internal
//  implementation doesn't need to. For subscribers the mixin class does any needed
//  locking.
//
//  Publisher objects should be treated as non-thread safe. If multiple threads need to
//  publish to the same topic, create the topic then let each thread join the topic and
//  get their own publishing object. Or, create it first and then copy construct one for
//  each thread, which will all see the same internal object, but the internal object is
//  thread safe.
//
//  For subscriptions, in theory an async subscriber could be read from by multiple
//  threads. If you want to insure that no msgs are seen twice, that would be necessary.
//  Otherwise, let each one create a subscription object if you want each one to see all
//  msgs.
//
//  When you implement the subscription mixin yourself, it's up to you. You need to deal
//  with the fact that the ProcessSubMsg() call can be made at any time. For GUI code that
//  is typically all on one thread so it's just a callback. If bgn threads are involved,
//  you have to be sure you can handle these incoming calls cleanly, providing your own
//  locking.
//
//  Of course you could also implement your own async queuing if that is more convenient
//  than using the async class. If you already had a queue for queing up things from multiple
//  sources you could also throw the incoming msgs into one. As long as the queue is thread
//  safe, then there are no issues.
//
//  Our internal msg class (which wraps your published objects) implements MRefCounted and
//  we manage the reference counting internally. Each subscriber is giving a copy of this
//  ref counted object, so they all see the same one. When the last subscriber has read
//  it, it is dropped.
//
//  To avoid potential deadlocks, we use a separate internal critical section to handle
//  the reference counting.
//
//  KEEP IN MIND that anything you do in the dtor of the published objects will happen at
//  some fairly random time, when the last subscriber of that topic has read it and let it
//  go. So this could also be an issue with deadlocking if that caused locking of some sort.
//  Generally speaking, published objects should be simple data carriers and just avoid
//  these types of complexities.
//
//  Async Polling vs. Event Driven
//
//  A common scenario with async subscribers is for the client code to either use a GUI
//  timer or a separate thread to monitor the subscription object for msgs to process.
//  In the former case it will just do a 0 timeout read to get anything already avaialble.
//  In the latter it can do a longer blocking read to just wait for stuff to be available.
//
//  But, in some cases, if some client code needed to subscribe to multiple topics, or to
//  wait for either topic msgs or socket input or some such, then it would want to use an
//  event driven scheme.
//
//  So you can request the subscription to create an event, which can either be blocked on
//  itself (not really needed sinc you can just do a blocking read) or added to to a multi-
//  event wait list. It will be posted when msgs are available and reset when not.
//
// CAVEATS/GOTCHAS:
//
//  1)  The async subscriber class is in a separate header, to avoid order of inclusion
//      issues. It needs to use a queue, but our header has to be before the collection
//      headers since the base collection class implements publishing.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// Forward reference our internal implementation classes
class TPSMsg;
class TPSTopic;



// ---------------------------------------------------------------------------
//  CLASS: TPubSubMsg
// PREFIX: pssub
// ---------------------------------------------------------------------------
class CIDLIBEXP TPubSubMsg : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TPubSubMsg
        (
                    TPSMsg* const       psmsgToAdopt = nullptr
        );

        TPubSubMsg
        (
            const   TPubSubMsg&         psmsgSrc
        );

        ~TPubSubMsg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPubSubMsg& operator=
        (
            const   TPubSubMsg&         psmsgSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TObject& objMsg() const;

        template <class T> const T& objMsgAs() const
        {
            CheckMsgType(T::clsThis());
            return static_cast<const T&>(objMsg());
        }

        const TString& strSrcTopicPath() const;

    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckMsgType
        (
            const   TClass&             clCheck
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_cptrMsgImpl
        //      A counter pointer to the internal implementation object we are wrapping.
        //      It can be null in our case since we have to support a default ctor. So
        //      this would only get set later, when a msg is read into us.
        // -------------------------------------------------------------------
        TCntPtr<TPSMsg>     m_cptrMsgImpl;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TPubSubMsg,TObject)
};



// ---------------------------------------------------------------------------
//  CLASS: MPubSubscription
// PREFIX: mpsub
// ---------------------------------------------------------------------------
class CIDLIBEXP MPubSubscription
{
    public  :
        // -------------------------------------------------------------------
        //  Public destructor, consructors are hidden
        // -------------------------------------------------------------------
        virtual ~MPubSubscription();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsSubscribedToTopic
        (
            const   TString&                strTopicPath
        )   const;

        tCIDLib::TCard4 c4SubscriberId() const;


    protected :
        // -------------------------------------------------------------------
        //  Declare friends
        // -------------------------------------------------------------------
        friend static tCIDLib::TVoid SendMsg
        (
            const   tCIDLib::TCard4         c4SubId
            , const TPubSubMsg&             psmsgToSend
        );

        friend static tCIDLib::ESortComps eCompSubId
        (
            const   tCIDLib::TCard4&        c4ToComp
            , const MPubSubscription&       mpsubToComp
        );



        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        MPubSubscription();

        MPubSubscription
        (
            const   MPubSubscription&       mpsubSrc
        );

        MPubSubscription& operator=
        (
            const   MPubSubscription&       mpsubSrc
        );


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid ProcessPubMsg
        (
            const   TPubSubMsg&             psmsgToPass
        ) = 0;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SubscribeToTopic
        (
            const   TString&                strTopicPath
            , const tCIDLib::TBoolean       bMustExist
        );

        tCIDLib::TVoid UnsubscribeFromTopic
        (
            const   TString&                strTopicPath
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckSubIsReady() const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4SubscriberId
        //      We get assigned a subscriber id when the including class calls our
        //      SubscribeToTopic() method the first time. If zero we have not subscribed
        //      to anything yet.
        //
        //  m_kllstTopicPaths
        //      The ids of the topics we are subscribed to. We can't use any of our own
        //      collections since we want them to be able to support pub/sub, and that
        //      would make a circular dependency. So we use a kernel level linked list.
        // -------------------------------------------------------------------
        tCIDLib::TCard4             m_c4SubscriberId;
        mutable TKrnlLList<TString> m_kllstTopicPaths;
};



// ---------------------------------------------------------------------------
//  CLASS: TPubSubTopic
// PREFIX: pstop
// ---------------------------------------------------------------------------
class CIDLIBEXP TPubSubTopic : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bTopicExists
        (
            const   TString&                strTopicPath
        );

        static TPubSubTopic pstopCreateTopic
        (
            const   TString&                strTopicPath
            , const TClass&                 clsMsgType
        );

        static const TString& strKey
        (
            const   TPubSubTopic&           pstopSrc
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor.
        // -------------------------------------------------------------------
        TPubSubTopic();

        TPubSubTopic
        (
            const   TPubSubTopic&           pstopSrc
        );

        ~TPubSubTopic();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPubSubTopic& operator=
        (
            const   TPubSubTopic&           pstopSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsPublished() const;

        tCIDLib::TCard4 c4RefCount() const;

        tCIDLib::TCard4 c4SubCount() const;

        tCIDLib::TCard4 c4TopicId() const;

        tCIDLib::TVoid DropTopic();

        const TString& strPath() const
        {
            return m_strTopicPath;
        }

        tCIDLib::TVoid Publish
        (
                    TObject* const          pobjToSend
        );


    protected :
        // -------------------------------------------------------------------
        //  We need to make the add/remove subscriber calls below accessible
        //  internally, and let the internal scavenger call have access to our
        //  our subscription object.
        // -------------------------------------------------------------------
        friend static tCIDLib::TVoid DoScavenge();
        friend class MPubSubscription;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TPubSubTopic
        (
            const   TString&                strPath
            , const TClass&                 clsMsgType
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddSubscriber
        (
            const   tCIDLib::TCard4         c4SubId
        );

        tCIDLib::TVoid RemoveSubscriber
        (
            const   tCIDLib::TCard4         c4SubId
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckReady() const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_cptrImpl
        //      A counted pointer around the impl object we are managing. It can be
        //      null if default ctor. We throw if any access before it is set.
        //
        //  m_strTopicPath
        //      To avoid possible issues with the topic path getting method, we get a
        //      redundant copy of the topic out of the impl object set on us.
        // -------------------------------------------------------------------
        TCntPtr<TPSTopic>   m_cptrImpl;
        TString             m_strTopicPath;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TPubSubTopic,TObject)
};

#pragma CIDLIB_POPPACK

