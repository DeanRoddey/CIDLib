//
// FILE NAME: CIDLib_CollectionBase.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/24/1995
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
//  We also define the base class for cursors, which is also the non-templatized
//  base that they all derive from.
//
//  Pub/Sub
//
//  Collections support publish/subscribe, which complicates things. We
//  provide the basic support here for setting a topic if pub/sub is requested
//  and defining the events, but the derived classes have to actually do the
//  event publishing since only they know what is going on.
//
//  It also raises a lot of complications for things like fast loading of a
//  collection one at a time. We can't know if that's going to happen, and we
//  could spit out as massive number of events, which would be really bad. So
//  we support a bulk mode, which we can be set into, and that will suppress
//  events until it is cleared then a bulk change event can be sent.
//
//  A block mode janitor is provided to do this on a scoped basis.
//
//  Move Semantics
//
//  Move semantics are complicated for collections. The issues of adoption of
//  elements and lockability have be dealt with. The approach we take is that
//  ONLY those things that are related teo the element content are moved. It's
//  important that the thread safety or adoption status of a collection don't
//  get changed just because we want to move elements into them from another
//  collection.
//
//  However, this raises potential concerns for by reference collections. If
//  did a move from an owning to a non-owning collection, those elements could
//  get leaked if this is not a purposeful choice. Therefore, a move from an
//  adopting to a non-adopting collection will throw.
//
//  Complications also arise from pub/sub block mode (see above.) If a or
//  copy is done while in block mode, an exception is thrown because it means
//  something has really gone wrong. The block operation must be completed first.
//
//
//  The actual code is in CIDLib_Collection.cpp, this header is split out for
//  order of inclusion purposes.
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
//   CLASS: TCursorBase
//  PREFIX: curs
// ---------------------------------------------------------------------------
class CIDLIBEXP TCursorBase : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TCursorBase() = default;


        // -------------------------------------------------------------------
        //  Public, pure virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bIsEmpty() const = 0;

        virtual tCIDLib::TBoolean bIsValid() const = 0;

        virtual tCIDLib::TBoolean bNext() = 0;

        virtual tCIDLib::TBoolean bReset() = 0;

        virtual tCIDLib::TCard4 c4ElemCount() const = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4SerialNum() const
        {
            return m_c4SerialNum;
        }


    protected   :
        // -------------------------------------------------------------------
        //  Hidden constructors and operators
        // -------------------------------------------------------------------
        TCursorBase() = default;

        TCursorBase
        (
            const   tCIDLib::TCard4         c4SerialNum
        );

        TCursorBase(const TCursorBase&) = default;
        TCursorBase& operator=(const TCursorBase&) = default;

        // Can't actually delete them since that causes problems
        // TCursorBase(TCursorBase&&) = delete;
        // TCursorBase& operator=(TCursorBase&&) = delete;

        tCIDLib::TBoolean operator==
        (
            const   TCursorBase&            cursSrc
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TCursorBase&            cursSrc
        )   const;



        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckSerialNum
        (
            const   tCIDLib::TCard4         c4ToCheck
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid CheckValid
        (
            const   tCIDLib::TVoid* const   pcursTest
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid CheckValid
        (
            const   tCIDLib::TBoolean       bState
            , const tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TCard4 c4SerialNum
        (
            const   tCIDLib::TCard4         c4New
        );

        tCIDLib::TVoid ThrowNotInitialized
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4SerialNum
        //      This is a serial number from the collection. Its stored when
        //      this collection is first created or synced back up by a
        //      bReset() call. This lets the derived cursor class know if
        //      it is out of date with respect to the collection it is
        //      cursoring.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4SerialNum = 0;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCursorBase,TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TColPubSubInfo
//  PREFIX: colpsi
// ---------------------------------------------------------------------------
class CIDLIBEXP TColPubSubInfo : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Public types
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
            , Reloaded

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
            , const tCIDLib::TCard4         c4Index1
            , const tCIDLib::TCard4         c4Index2
        );

        TColPubSubInfo(const TColPubSubInfo&) = default;
        TColPubSubInfo(TColPubSubInfo&&) = default;

        ~TColPubSubInfo() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TColPubSubInfo& operator=(const TColPubSubInfo&) = default;
        TColPubSubInfo& operator=(TColPubSubInfo&&) = default;


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
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Index1;
        tCIDLib::TCard4 m_c4Index2;
        EEvents         m_eEvent;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TColPubSubInfo,TObject)
};




// ---------------------------------------------------------------------------
//   CLASS: TCollectionBase
//  PREFIX: col
// ---------------------------------------------------------------------------
class CIDLIBEXP TCollectionBase : public TObject, public MLockable
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid BadStoredCount
        (
            const   TClass&                 clsCol
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TCollectionBase();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        //
        //  At this level, we just always say the lock worked, because there is
        //  no lock. Derived classes that provide locking will override these.
        //  This lets all collections (internally and externally) be locked via
        //  the standard MLockable interface and TLocker lock janitor.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTryLock(const tCIDLib::TCard4 c4WaitMS) const final
        {
            if (m_pmtxLock)
                return m_pmtxLock->bTryLock(c4WaitMS);

            return kCIDLib::True;
        }

        tCIDLib::TVoid Lock(const tCIDLib::TCard4 c4WaitMS) const final
        {
            if (m_pmtxLock)
                m_pmtxLock->Lock(c4WaitMS);
        }

        tCIDLib::TVoid Unlock() const override
        {
            if (m_pmtxLock)
                m_pmtxLock->Unlock();
        }


        // -------------------------------------------------------------------
        //  Public, virtual methods
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

        tCIDLib::TBoolean bIsMTSafe() const
        {
            return (m_pmtxLock != nullptr);
        }

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

        tCIDLib::EMTStates eMTSafe() const
        {
            if (m_pmtxLock == nullptr)
                return tCIDLib::EMTStates::Unsafe;
            return tCIDLib::EMTStates::Safe;
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

        tCIDLib::TVoid PublishReloaded();

        const TString& strTopicPath() const
        {
            if (!m_ppstopReport)
                return TString::strEmpty();
            return m_ppstopReport->strPath();
        }


    protected   :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TColBlockModeJan;


        // -------------------------------------------------------------------
        //  Protected, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bWaitForData
        (
                    TLocker&                lockrQueue
            , const TCollectionBase&        colSrc
            , const tCIDLib::TCard4         c4WaitMSs
            ,       TThreadWaitList&        twlWaitList
            , const tCIDLib::TBoolean       bThrowIfNot
        );


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

        // We only support move assignment here
        TCollectionBase(TCollectionBase&&) = delete;

        TCollectionBase& operator=
        (
            const   TCollectionBase&        colSrc
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

        tCIDLib::TVoid EnablePublish
        (
            const   TString&                strDesiredPath
            ,       TString&                strActualPath
        );

        tCIDLib::TVoid EnablePublish
        (
            const   TString&                strTopicPath
        );

        tCIDLib::TVoid HashChanged
        (
            const   tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid KeyNotFound
        (
             const  tCIDLib::TCh* const     pszFile
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid MovedAdopted
        (
            const   tCIDLib::TCh* const     pszFile
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

        tCIDLib::TVoid PublishReorder();

        tCIDLib::TVoid PublishSwap
        (
            const   tCIDLib::TCard4         c4At1
            , const tCIDLib::TCard4         c4At2
        );

        tCIDLib::TVoid SetMTState
        (
            const   tCIDLib::EMTStates      eToSet
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
        //      This is a serial number from the collection. It is bumped up every time
        //      changes are made to the element content. Cursors watch it to know if they
        //      are out of date.
        //
        //      We start the serial number at 1! This makes it easier for code that
        //      needs to watch for changes. They can set their last serial number to
        //      zero, and it will always trigger an initial inequality.
        //
        //  m_pmtxLock
        //      If this collection is thread safe, this is allocated, else null.
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

        TFundColBase(const TFundColBase&)  = default;
        TFundColBase& operator=(const TFundColBase&) = default;

        // No move support, it's too complicated
        TFundColBase(TFundColBase&&)  = delete;
        TFundColBase& operator=(TFundColBase&&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFundColBase, TCollectionBase)
};



#pragma CIDLIB_POPPACK
