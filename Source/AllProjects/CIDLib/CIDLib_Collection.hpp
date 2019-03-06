//
// FILE NAME: CIDLib_Collection.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/24/1995
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This header implements the base class for all template collections. All
//  of the collections derive from TCollectionBase and must implement its
//  virtual interface. This interface allows for basic things like checking
//  the number of elements, checking for empty, checking for full. I.e. things
//  that do not depend upon any particular ordering or storage style or
//  knowledge of the storage mechanism or element type.
//
//  By value collections also implement the MDuplicable interface, which allows
//  a collection to be duplicated polymorphically (with all its contents.) This is
//  a powerful concept. It means that all element classes must support copy
//  construction and assignment, but that was already a requirement anyway for a
//  'by value' collection of elements (in order to get elements in and out of the
//  collection.)
//
//  There are two derivatives of TCollectionBase, which represent the two main types
//  of by value collections, element based and map based. Element based collections
//  have self contained elements. Map based collections have a key and a value,
//  whether the key is conceptually part of the element or a separate thing. Either
//  way, they require another instantiation parameter, that of the key field. Hash
//  key based ones have self contained elements, but must provide a key extractor
//  so that the key can be known to the collection.
//
//  TRefCollection is the derivative of TCollectionBase that serves as the base for
//  all 'by reference' collections. The main difference is that the by value
//  derivatives use an objAdd(const TElem&)) method and the by ref one use an
//  Add(TElem* const) method, representing the two ways that they accept their elements.
//  The former of course copies the added element.
//
//  And finally we introduce two commonly used node classes, which are based on the
//  fundamental doubly linked list node. Many collections are based on this doubly
//  linked list, and so can use this common node literally or as the basis for their
//  nodes. We have one for by value and one for by reference nodes.
//
//  We also provide a base class for fundamental collections, which are a different
//  set of collections from the object based ones. They just hold fundamental types
//  and are much simpler. The base class is just to help get common code out of the
//  templatized stuff in order to make the generated code smaller, mostly error
//  checking/throwing stuff.
//
//  Publish/Subscribe
//
//  Collections can support the publish/subscribe frameweork, so that client code
//  can subscribe to changes. That makes it much easier to track changes in the
//  collection than trying to insure that they put checks at every point in their
//  own code where the collection might be changed.
//
//  So the base collection classes have a pointer to a published topic object, which
//  they will create if asked to by the derived class. We don't directly expose that
//  setup method since not all collections support pub/sub yet.
//
//  So the derived class calls EnablePublish() to enable it. It's assumed that, if pub/sub
//  is enabled that someone is listening, so posting events isn't a waste of time, so
//  no need to bother constantly checking if there are any subscribers. DisablePubSub()
//  can be called to close the topic.
//
//  We can only support one topic. So EnablePublish takes a desired topic path, but if
//  publishing is already enabled it returns the already set up path, so the caller can
//  use that instead. If it wasn't already set up, you get back the path you passed in.
//  If you care, you can compare them to see if your path was used. We have another
//  version that just takes the path to use and throws if it is already enabled under
//  another path. If it's already enabled under the same path it just returns since
//  you can just use your desired path already.
//
//  We provide a small set of PublishXXX() variations that the derived class should
//  call to report changes. There are only so many scenarios and we want to keep them
//  small. Most just take an event type plus an index or key. Others take a little more
//  info. We generate the correct topic object and publish it.
//
//  There is also a public PublishElemChange() method that we call if we can know that
//  an element was changed. Often we cannot know (because it was directly modified) so
//  this one is public, allowing the code that changed it to report the change.
//
//  * When we remove an element, we publish FIRST, then remove. This way, sync subscribers
//    get a chance to get info about the element before we kill it. This can be very
//    convenient, though they have to be careful to understand that is what is happening.
//
//  Bulk load with Pub/Sub
//
//  There is a HUGE gotcha here. Each item added generates a msg. If you spit out
//  100K items into a collection at once, that will cause 100K msgs, which isn't
//  practical (and will throw since the subcriber's queue isn't nearly that big.) Same
//  with a big remove of elements. There is no clean way to automagically deal with this,
//  because we cannot get back msgs already published and we don't know it's a bulk
//  load/remove starting if they are done one at a time, or when it ended. The client
//  code MUST cooperate or it's going to get full subscription queue errors.
//
//  So they have to call our BlockModeStart() method, do the load, then call our
//  BlockModeEnd() method with the start/count values of what it loaded/removed. Or, if
//  they are not contiguous they must pass c4MaxCard for the start value, which just
//  sends a Reordered event (effectively, hey, reload from this collection.)
//
//  There's just no way around it. We use a flag that is set while doing this.
//  Obviously they have to worry about exceptions that would leave the flag set, so
//  we provide a janitor. It will set the flag and remember the start index. It can
//  be ticked each time one is added/removed to bump a counter. When it dtors, it will
//  call BlockModeEnd.
//
//  If a copy/assign is done with the block mode flag set, it is just cleared. A
//  full report will be issued anyway with the new content.
//
//  Synchronization
//
//  Collections can be created thread safe. If so, a mutex is created. The derived
//  classes can lock on that as required, and we do the same at our level.
//
//  You would have to explicitly change those things.
//
//  Move/Assign
//
//  You have to be very careful with move/assign. This ONLY deals with element
//  content, it doesn't change thread safety, adoption status for by ref vectors,
//  publish topic, etc... So we provide empty move/assign stuff at our level so
//  that derived classes can call them or have defaults.
//
//  It does change anything required to represent the elements in the same order
//  (they should be considered equal by the criteria below.) So, for instance, a
//  hash collection would adopt the source hash modulus so that it will have the
//  same element ordering after the operation.
//
//  As with all changes, the derived class has to deal with pub/sub if enabled.
//  For assign, remove current elements, post a clear. Take on the new elements,
//  and post a block mode add.
//
//  *   For now we are still re-adding move support back after a huge re-working
//      of how collections work.
//
//  Equality
//
//  Equality for collections means that they have the same number of elements
//  and that those elements, in iteration order, are equal. This is only meaningful
//  for by value collections. Anything beyond that client code has to check themselves
//  because things mentioned above in the Move/Assign section that are not affected
//  by these operations are considered internal details.
//
//  So there is a global equality operator below that works generically for all
//  non-fundamental collections.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TThreadWaitList;
class TColBlockModeJan;


// ---------------------------------------------------------------------------
//   CLASS: TColPubSubInfo
//  PREFIX: colpsi
// ---------------------------------------------------------------------------
class CIDLIBEXP TColPubSubInfo : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public types
        //
        //  These events use these members, else they don't use any:
        //
        //      BlockAdded
        //          Index1  - Index of first item added
        //          Index2  - Count of items added after that
        //
        //      BlockChanged
        //          Index1  - Index of first item changed
        //          Index2  - Count of items changed after that
        //
        //      BlockRemoved
        //          Index1  - Index of first item removed
        //          Index2  - Count of items removed after that
        //
        //      ElemAdded
        //          Index1  - Index where it was added
        //
        //      ElemChanged
        //          Index1 - Index of element changed
        //
        //      ElemMoved
        //          Index1  - Where it was originally
        //          Index2  - Where it ended up
        //
        //      ElemChanged
        //      ElemRemoved
        //          Index1  - Index of element modified or removed
        //
        //      Loaded
        //          The previous contents has been replaced with new contents
        //          Index1 - The count of elements newly added
        //
        //      Reordered
        //          The elements are the same but the order has been affected
        //
        //      Swapped
        //          Index1  - Index of element moved to index 2
        //          Index2  - Index of element moved to index 1
        // -------------------------------------------------------------------
        enum class EEvents
        {
            BlockAdded
            , BlockChanged
            , BlockRemoved
            , Cleared
            , ElemAdded
            , ElemChanged
            , ElemMoved
            , ElemRemoved
            , Reordered
            , Swapped

            , Count
        };


        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TColPubSubInfo();

        TColPubSubInfo
        (
            const   EEvents                 eEvent
            , const tCIDLib::TCard4         c4Index = 0
        );

        TColPubSubInfo
        (
            const   EEvents                 eEvent
            , const TString&                stRKey
        );

        TColPubSubInfo
        (
            const   EEvents                 eEvent
            , const tCIDLib::TCard4         c4Index1
            , const tCIDLib::TCard4         c4Index2
        );

        TColPubSubInfo(const TColPubSubInfo&) = delete;

        ~TColPubSubInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TColPubSubInfo& operator=
        (
            const   TColPubSubInfo&         colpsiSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Index1() const
        {
            return m_c4Index1;
        }

        tCIDLib::TCard4 c4Index2() const
        {
            return m_c4Index2;
        }

        EEvents eEvent() const
        {
            return m_eEvent;
        }

        const TString& strKey() const
        {
            return m_strKey;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Index1
        //  m_c4Index2
        //      Some indices that mean different things based on the event. See the
        //      class comments above.
        //
        //  m_eEvent
        //      The event being reported
        //
        //  m_strKey
        //      Used for add/remove in key based collections.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Index1;
        tCIDLib::TCard4 m_c4Index2;
        EEvents         m_eEvent;
        TString         m_strKey;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TColPubSubInfo,TObject)
};




// ---------------------------------------------------------------------------
//   CLASS: TCollectionBase
//  PREFIX: col
// ---------------------------------------------------------------------------
class CIDLIBEXP TCollectionBase : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bWaitForData
        (
                    TMtxLocker&             lockQueue
            , const TCollectionBase&        colSrc
            , const tCIDLib::TCard4         c4WaitMSs
            ,       TThreadWaitList&        twlWaitList
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        static tCIDLib::TVoid BadStoredCount
        (
            const   TClass&                 clsCol
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TCollectionBase();


        // -------------------------------------------------------------------
        //  Public, pure virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bIsEmpty() const = 0;

        virtual tCIDLib::TCard4 c4ElemCount() const = 0;

        virtual tCIDLib::TVoid RemoveAll() = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsFull(const tCIDLib::TCard4 c4Limit) const
        {
            return (c4ElemCount() >= c4Limit);
        }

        tCIDLib::TBoolean bIsMTSafe() const;

        tCIDLib::TBoolean bPublishEnabled() const
        {
            return (m_ppstopReport != nullptr);
        }

        tCIDLib::TCard4 c4SerialNum() const;

        tCIDLib::TVoid CheckIsFull
        (
            const   tCIDLib::TCard4         c4Limit
            , const tCIDLib::TCh* const     pszDescr
        );

        tCIDLib::EMTStates eMTState() const;

        TMutex* pmtxLock() const
        {
            return m_pmtxLock;
        }

        tCIDLib::TVoid Lock(const tCIDLib::TCard4 c4Timeout = kCIDLib::c4MaxWait) const
        {
            // If this one is lockable, then do the lock
            if (m_pmtxLock)
                m_pmtxLock->Lock(c4Timeout);
        }

        tCIDLib::TVoid PublishBlockChanged
        (
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid PublishElemChanged
        (
            const   tCIDLib::TCard4         c4At
        );

        tCIDLib::TVoid PublishReload();

        const TString& strTopicPath() const
        {
            if (!m_ppstopReport)
                return TString::strEmpty();
            return m_ppstopReport->strPath();
        }

        tCIDLib::TVoid Unlock() const
        {
            // If this one is lockable, then do the unlock
            if (m_pmtxLock)
                m_pmtxLock->Unlock();
        }


    protected   :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TColBlockModeJan;


        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TCollectionBase
        (
            const   tCIDLib::EMTStates      eMTSafe = tCIDLib::EMTStates::Unsafe
        );

        TCollectionBase
        (
            const   TCollectionBase&        colSrc
        );

        TCollectionBase& operator=
        (
            const   TCollectionBase&        colSrc
        );

        TCollectionBase
        (
                    TCollectionBase&&       colSrc
        );

        TCollectionBase& operator=
        (
                    TCollectionBase&&       colSrc
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckNewSize
        (
            const   tCIDLib::TCard4         c4New
            , const tCIDLib::TCard4         c4Current
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid BlockModeEnd
        (
            const   tCIDLib::TBoolean       bAddMode
            , const tCIDLib::TCard4         c4Index
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid BlockModeStart();

        tCIDLib::TCard4 c4IncSerialNum();

        tCIDLib::TVoid CheckAllocAndCount
        (
            const   tCIDLib::TCard4         c4Count
            , const tCIDLib::TCard4         c4Alloc
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        );

        tCIDLib::TVoid CheckCursorValid
        (
            const   TCursorBase&            cursToCheck
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid CheckIndex
        (
            const   tCIDLib::TCard4         c4ToCheck
            , const tCIDLib::TCard4         c4CurCount
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid CheckOneNonAdopting
        (
            const   tCIDLib::EAdoptOpts     eAdoptSrc
            , const tCIDLib::EAdoptOpts     eAdoptTar
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid CheckReallocParms
        (
            const   tCIDLib::TCard4         c4NewSize
            , const tCIDLib::TCard4         c4CurCount
            , const tCIDLib::TBoolean       bKeepOld
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid ColIsEmpty
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid ColIsFull
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TCard4         c4Max
        )   const;

        tCIDLib::TVoid DuplicateElem
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid DuplicateKey
        (
            const   TObject&                objKey
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::EMTStates eMTState
        (
            const   tCIDLib::EMTStates      eState
        );

        tCIDLib::TVoid EnablePublish
        (
            const   TString&                strDesiredPath
            ,       TString&                strActualPath
        );

        tCIDLib::TVoid EnablePublish
        (
            const   TString&                strTopicPath
        );

        tCIDLib::TVoid KeyNotFound
        (
            const   TObject&                objKey
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid MustClearFirst
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid NodeNotFound
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid NotMemberNode
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        ) const;

        tCIDLib::TVoid NotMyCursor
        (
            const   TClass&                 clsCursor
            , const TClass&                 clsCol
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid NullNodeAdded
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid PublishAdd
        (
            const   tCIDLib::TCard4         c4At
        );

        tCIDLib::TVoid PublishAdd
        (
            const   TString&                strKey
        );

        tCIDLib::TVoid PublishBlockAdded
        (
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid PublishBlockRemoved
        (
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TCard4         c4Count
        );

        tCIDLib::TVoid PublishClear();

        tCIDLib::TVoid PublishMoved
        (
            const   tCIDLib::TCard4         c4From
            , const tCIDLib::TCard4         c4To
        );

        tCIDLib::TVoid PublishRemove
        (
            const   tCIDLib::TCard4         c4At
        );

        tCIDLib::TVoid PublishRemove
        (
            const   TString&                strRemove
        );

        tCIDLib::TVoid PublishReorder();

        tCIDLib::TVoid PublishSwap
        (
            const   tCIDLib::TCard4         c4At1
            , const tCIDLib::TCard4         c4At2
        );

        tCIDLib::TVoid SrcTooBig
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TCard4         c4SrcElems
            , const tCIDLib::TCard4         c4TarMax
        )   const;

        tCIDLib::TVoid Unsupported
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TCh* const     pszOpName
        )   const;

        tCIDLib::TVoid ZeroSize
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bInBlockMode
        //      This indicates we are in a block mode and we will not send individual
        //      add/remove reports if pub/sub is enabled.
        //
        //  m_c4SerialNum
        //      This is a serial number from the collection. It is bumped up every
        //      time a node is added or removed. Cursors watch it to know if they are
        //      out of date.
        //
        //      We start the serial number at 1! This makes it easier for code that
        //      needs to watch for changes. They can set their last serial number to
        //      zero, and it will always trigger an initial inequality.
        //
        //  m_pmtxLock
        //      This is the optional mutex that allows this collection to be lockable.
        //      The derived class indicates to our constructor whether this collection
        //      should be mutlti-thread safe.
        //
        //  m_ppstopReport
        //      A topic to report changes to subscribers. See the header comments above
        //      for details. Only created if the derived class sets it up.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bInBlockMode;
        tCIDLib::TCard4     m_c4SerialNum;
        TMutex*             m_pmtxLock;
        TPubSubTopic*       m_ppstopReport;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCollectionBase,TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TFundColBase
//  PREFIX: col
// ---------------------------------------------------------------------------
class CIDLIBEXP TFundColBase : public TCollectionBase
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TFundColBase();


    protected   :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TFundColBase
        (
            const   tCIDLib::EMTStates      eMTSafe = tCIDLib::EMTStates::Unsafe
        );

        TFundColBase(const TFundColBase& colSrc) :

            TCollectionBase(colSrc)
        {
        }

        tCIDLib::TVoid operator=(const TFundColBase&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFundColBase, TCollectionBase)
};




// ---------------------------------------------------------------------------
//   CLASS: TCollection
//  PREFIX: col
// ---------------------------------------------------------------------------
template <class TElem>
class TCollection : public TCollectionBase, public MDuplicable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TCollection()
        {
        }


        // -------------------------------------------------------------------
        //  Public, pure virtual methods
        // -------------------------------------------------------------------
        virtual TElem& objAdd
        (
            const   TElem&                  objNew
        ) = 0;

        virtual TColCursor<TElem>* pcursNew() const = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddXCopies(  const   TElem&          objNew
                                    , const tCIDLib::TCard4 c4Count)
        {
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                objAdd(objNew);
        }


        //
        //  Call back each element, breaking out on false return. Can be a function
        //  pointer or lambda. We use a cursor so it will work for any of our
        //  collection derivatives.
        //
        template <typename IterCB> tCIDLib::TVoid ForEach(IterCB iterCB) const
        {
            TMtxLocker lockThis(this->pmtxLock());
            TColCursor<TElem>* pcursEach = pcursNew();
            TJanitor<TColCursor<TElem>> janCursor(pcursEach);
            while (pcursEach->bIsValid())
            {
                if (!iterCB(pcursEach->objRCur()))
                    break;
                pcursEach->bNext();
            }
        }


    protected   :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TCollection(const tCIDLib::EMTStates eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TCollectionBase(eMTSafe)
        {
        }

        TCollection(const TCollection<TElem>& colSrc) :

            TCollectionBase(colSrc)
        {
        }

        TCollection(TCollection<TElem>&& colSrc) :

            TCollectionBase(tCIDLib::ForceMove(colSrc))
        {
        }

        TCollection<TElem>& operator=(const TCollection<TElem>& colSrc)
        {
            TCollectionBase::operator=(colSrc);
            return *this;
        }

        TCollection<TElem>& operator=(TCollection<TElem>&&  colSrc)
        {
            TCollectionBase::operator=(tCIDLib::ForceMove(colSrc));
            return *this;
        }


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TCollection<TElem>,TCollectionBase)
};



// ---------------------------------------------------------------------------
//   CLASS: TMapCollection
//  PREFIX: col
// ---------------------------------------------------------------------------
template <class TElem, class TKey> class TMapCollection

    : public TCollection<TKeyObjPair<TKey, TElem>>
{
    public  :
        // -------------------------------------------------------------------
        //  Public class types
        // -------------------------------------------------------------------
        using TMyType = TMapCollection<TElem, TKey>;
        using TParent = TCollection<TKeyObjPair<TKey, TElem>>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TMapCollection()
        {
        }


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsDescendantOf(const TClass& clsTarget) const override
        {
            if (clsTarget == this->clsThis())
                return kCIDLib::True;
            return TParent::bIsDescendantOf(clsTarget);
        }

        const TClass& clsParent() const override
        {
            return TMyType::clsThis();
        }


    protected   :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TMapCollection(const tCIDLib::EMTStates eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TParent(eMTSafe)
        {
        }

        TMapCollection(const TMyType& colSrc) :

            TParent(colSrc)
        {
        }

        TMapCollection& operator=(const TMyType& colSrc)
        {
            if (&colSrc != this)
                TParent::operator=(colSrc);
            return *this;
        }

        TMapCollection(TMyType&& colSrc) :

            TParent(tCIDLib::ForceMove(colSrc))
        {
        }

        TMapCollection& operator=(TMyType&& colSrc)
        {
            TParent::operator=(tCIDLib::ForceMove(colSrc));
            return *this;
        }
};



// ---------------------------------------------------------------------------
//   CLASS: TBasicColNode
//  PREFIX: node
// ---------------------------------------------------------------------------
template <class TElem> class TBasicColNode : public TDLstNode
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBasicColNode() = delete;

        TBasicColNode(const TElem& objData) :

            m_objData(objData)
        {
        }

        TBasicColNode(const TBasicColNode<TElem>&) = delete;

        ~TBasicColNode() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBasicColNode<TElem>& operator=(const TBasicColNode<TElem>&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TElem& objData() const
        {
            return m_objData;
        }

        TElem& objData()
        {
            return m_objData;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_objData
        //      The user data object.
        // -------------------------------------------------------------------
        TElem   m_objData;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TBasicColNode<TElem>,TDLstNode)
};



// ---------------------------------------------------------------------------
//   CLASS: TRefCollection
//  PREFIX: curs
// ---------------------------------------------------------------------------
template <class TElem> class TRefCollection : public TCollectionBase
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TRefCollection()
        {
        }

        // -------------------------------------------------------------------
        //  Public, pure virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid Add
        (
                    TElem* const            pobjToAdd
        ) = 0;

        virtual tCIDLib::EAdoptOpts eAdopt() const = 0;

        virtual tCIDLib::TVoid GiveAllTo
        (
                    TRefCollection<TElem>&  colTarget
        ) = 0;

        virtual tCIDLib::TVoid OrphanElem
        (
                    TElem* const            pobjToRemove
        ) = 0;

        virtual TColCursor<TElem>* pcursNew() const = 0;

        virtual tCIDLib::TVoid RemoveElem
        (
                    TElem* const            pobjToRemove
        ) = 0;


        //
        //  Call back each element, breaking out on false return. Can be a function
        //  pointer or lambda. We use a cursor so it will work for any of our
        //  collection derivatives.
        //
        template <typename IterCB> tCIDLib::TVoid ForEach(IterCB iterCB) const
        {
            TMtxLocker lockThis(this->pmtxLock());
            TColCursor<TElem>* pcursEach = pcursNew();
            TJanitor<TColCursor<TElem>> janCursor(pcursEach);
            while (pcursEach->bIsValid())
            {
                if (!iterCB(pcursEach->objRCur()))
                    break;
                pcursEach->bNext();
            }
        }


    protected   :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TRefCollection(const tCIDLib::EMTStates eMTSafe = tCIDLib::EMTStates::Unsafe) :

            TCollectionBase(eMTSafe)
        {
        }

        TRefCollection(TRefCollection&&  colSrc) :

            TCollectionBase(tCIDLib::ForceMove(colSrc))
        {
        }

        TRefCollection<TElem>& operator=(TRefCollection&& colSrc)
        {
             TCollectionBase::operator=(tCIDLib::ForceMove(colSrc));
             return *this;
        }

        TRefCollection(const TRefCollection&) = delete;
        TRefCollection<TElem>& operator=(const TRefCollection&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TRefCollection<TElem>,TCollectionBase)
};



// ---------------------------------------------------------------------------
//   CLASS: TBasicRefColNode
//  PREFIX: node
// ---------------------------------------------------------------------------
template <class TElem> class TBasicColRefNode : public TDLstNode
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBasicColRefNode() = delete;
        TBasicColRefNode(       TElem* const        pobjData
                        , const tCIDLib::EAdoptOpts eAdopt) :

            m_bOwns(eAdopt == tCIDLib::EAdoptOpts::Adopt)
            , m_pobjData(pobjData)
        {
        }

        TBasicColRefNode(const TBasicColRefNode<TElem>&) = delete;

        ~TBasicColRefNode()
        {
            // Delete the data if we own it
            if (m_bOwns && m_pobjData)
                delete m_pobjData;
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBasicColRefNode<TElem>& operator=(const TBasicColRefNode<TElem>&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid OrphanData()
        {
            m_pobjData = 0;
        }

        TElem* pobjOrphanData()
        {
            TElem* pobjRet = m_pobjData;
            m_pobjData = 0;
            return pobjRet;
        }

        const TElem* pobjData() const
        {
            return m_pobjData;
        }

        TElem* pobjData()
        {
            return m_pobjData;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bOwns
        //      A flag that indicates whether we own our data or not.
        //
        //  m_pobjData
        //      The pointer to the user data object.
        // -------------------------------------------------------------------
        tCIDLib::TCard1 m_bOwns;
        TElem*          m_pobjData;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        TemplateRTTIDefs(TBasicColRefNode<TElem>,TDLstNode)
};


// ---------------------------------------------------------------------------
//   CLASS: TColBlockModeJan
//  PREFIX: jan
//
//  Our ctor sets the m_bBlockMode flag on the collection. Our dtor calls the
//  BlockAddDone() method on it.
// ---------------------------------------------------------------------------
class CIDLIBEXP TColBlockModeJan
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TColBlockModeJan() = delete;

        TColBlockModeJan
        (
                    TCollectionBase* const  pcolTar
            , const tCIDLib::TBoolean       bLoad
            , const tCIDLib::TCard4         c4Start = kCIDLib::c4MaxCard
        );

        TColBlockModeJan(const TColBlockModeJan&) = delete;

        ~TColBlockModeJan();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TColBlockModeJan& operator=(const TColBlockModeJan&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid IncCount();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bLoad
        //      Remembers if this is a block add or block remove
        //
        //  m_c4Count
        //      The count of elements added at start. Initially zero and bumped by
        //      the IncCount() method.
        //
        //  m_c4Start
        //      The starting index of the block load. If max card, then this is a
        //      non-contiguous (maybe sorted) load.
        //
        //  m_pcolTar
        //      A pointer to the collection we are handling.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bLoad;
        tCIDLib::TCard4     m_c4Count;
        tCIDLib::TCard4     m_c4Start;
        TCollectionBase*    m_pcolTar;
};


//
//  Global equality comparisons that will work for all by value and by ref
//  collections if the elements have an equality operator.
//
template <typename TElem>
tCIDLib::TBoolean operator==(const TCollection<TElem>& col1, const TCollection<TElem>& col2)
{
    if (&col1 == &col2)
        return kCIDLib::True;

    // Lock both of the collections while we do this
    TMtxLocker lockThis(col1.pmtxLock());
    TMtxLocker lockSrc(col2.pmtxLock());

    if (col1.c4ElemCount() != col2.c4ElemCount())
        return kCIDLib::False;

    TColCursor<TElem>* pcurs1 = col1.pcursNew();
    TColCursor<TElem>* pcurs2 = col2.pcursNew();
    TJanitor<TColCursor<TElem>> janCurs1(pcurs1);
    TJanitor<TColCursor<TElem>> janCurs2(pcurs2);

    while (pcurs1->bIsValid())
    {
        if (!(pcurs1->objRCur() == pcurs2->objRCur()))
            return kCIDLib::False;
        pcurs1->bNext();
        pcurs2->bNext();
    }
    return kCIDLib::True;
}

template <typename TElem>
tCIDLib::TBoolean operator!=(const TCollection<TElem>& col1, const TCollection<TElem>& col2)
{
    return !operator==(col1, col2);
}


template <typename TElem> tCIDLib::TBoolean
operator==(const TRefCollection<TElem>& col1, const TRefCollection<TElem>& col2)
{
    if (&col1 == &col2)
        return kCIDLib::True;

    // Lock both of the collections while we do this
    TMtxLocker lockThis(col1.pmtxLock());
    TMtxLocker lockSrc(col2.pmtxLock());

    if (col1.c4ElemCount() != col2.c4ElemCount())
        return kCIDLib::False;

    TColCursor<TElem>* pcurs1 = col1.pcursNew();
    TColCursor<TElem>* pcurs2 = col2.pcursNew();
    TJanitor<TColCursor<TElem>> janCurs1(pcurs1);
    TJanitor<TColCursor<TElem>> janCurs2(pcurs2);

    while (pcurs1->bIsValid())
    {
        if (!(pcurs1->objRCur() == pcurs2->objRCur()))
            return kCIDLib::False;
        pcurs1->bNext();
        pcurs2->bNext();
    }
    return kCIDLib::True;
}

template <typename TElem> tCIDLib::TBoolean
operator!=(const TRefCollection<TElem>& col1, const TRefCollection<TElem>& col2)
{
    return !operator==(col1, col2);
}


//
//  And here are some methods that also compare two collections, but they
//  allow for a custom comparator.
//
namespace tCIDLib
{
    template <typename TElem, typename TCompFunc>
    tCIDLib::TBoolean
    bCompareElems(  const   TCollection<TElem>& col1
                    , const TCollection<TElem>& col2
                    ,       TCompFunc           pfnComp)
    {
        if (&col1 == &col2)
            return kCIDLib::True;

        // Lock both of the collections while we do this
        TMtxLocker lockThis(col1.pmtxLock());
        TMtxLocker lockSrc(col2.pmtxLock());

        if (col1.c4ElemCount() != col2.c4ElemCount())
            return kCIDLib::False;

        TColCursor<TElem>* pcurs1 = col1.pcursNew();
        TColCursor<TElem>* pcurs2 = col2.pcursNew();
        TJanitor<TColCursor<TElem>> janCurs1(pcurs1);
        TJanitor<TColCursor<TElem>> janCurs2(pcurs2);

        while (pcurs1->bIsValid())
        {
            if (!pfnComp(pcurs1->objRCur(), pcurs2->objRCur()))
                return kCIDLib::False;

            pcurs1->bNext();
            pcurs2->bNext();
        }
        return kCIDLib::True;
    }


    template <typename TElem, typename TCompFunc>
    tCIDLib::TBoolean
    bCompareElems(  const   TRefCollection<TElem>&  col1
                    , const TRefCollection<TElem>&  col2
                    ,       TCompFunc               pfnComp)
    {
        if (&col1 == &col2)
            return kCIDLib::True;

        // Lock both of the collections while we do this
        TMtxLocker lockThis(col1.pmtxLock());
        TMtxLocker lockSrc(col2.pmtxLock());

        if (col1.c4ElemCount() != col2.c4ElemCount())
            return kCIDLib::False;

        TColCursor<TElem>* pcurs1 = col1.pcursNew();
        TColCursor<TElem>* pcurs2 = col2.pcursNew();
        TJanitor<TColCursor<TElem>> janCurs1(pcurs1);
        TJanitor<TColCursor<TElem>> janCurs2(pcurs2);

        while (pcurs1->bIsValid())
        {
            if (!pfnComp(pcurs1->objRCur(), pcurs2->objRCur()))
                return kCIDLib::False;

            pcurs1->bNext();
            pcurs2->bNext();
        }
        return kCIDLib::True;
    }
}

#pragma CIDLIB_POPPACK

