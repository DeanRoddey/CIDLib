//
// FILE NAME: CIDLib_StatsCache.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/07/2009
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
//  This is the definition file for the CIDLib_StatsCache.Cpp file. This file
//  implements a namespace that is used for global process statistics
//  collection and for flags, such as logging verbosity flags. Our ORB
//  utility facility defines a generic remote process control interface and
//  it provides methods for accessing this information remotely, allowing for
//  remote reading and writing of these values.
//
//  It's logically a tree structure, so each value is accessed via a path
//  style key, e.g. /CIDLib/FileSystem/Logging. Each terminal node holds a
//  TInt4 value, which can be used as a counter (positive or negative) or
//  as a flag using 0 and 1. Each one indicates (for the benefit of generic
//  access) whether its a counter or flag, and this also allows us to catch
//  incorrect access methods being used.
//
//  We don't actually create a tree structure. It's just a flat list, in a
//  hash style structure for fast access. It only needs to be assembled into
//  a tree structure for display purposes, if even desired then, so client
//  code is left to handle that as desired. Iteration of the keys under a given
//  scope is done by just finding any keys that start with the prefix
//  of that scope.
//
//  Internally we use no CIDLib level functionality, so as to avoid circular
//  dependency issues. We use all kernel services and only use fundamental
//  types in our interface, because CIDLib level code will use this guy to
//  check whether they are in verbose logging modes or to store stats. We do
//  use some higher level functionality in the methods that allow for the
//  query out of the values for presentation purposes.
//
//  If an item is accessed before it is created, then we have to be careful
//  how we deal with that. For reads we will just assume a zero value, which
//  is what it would have had it been created and not set yet. For writes,
//  we'll fault it in based on how it was accessed. If it is later explicitly
//  created, and the type is different, the type will be changed. But no
//  matter what we cannot throw any exceptions from here because it will
//  be used in very fundamental code.
//
//  Keys are never removed during the process run. So we can give out pointers
//  to nodes safely, which allows client code to pre-lookup (and/or fault in)
//  nodes and store a pointer for efficient subsequent access, without our
//  having to maintain a separate 'by index' type of mapping. We thinly
//  disguise it via a 'stat cache item' class. It actually contains a pointer
//  and an index value that is used as a sanity check. Each new item is given a
//  unique index and that is given back in the item along with the pointer.
//  So, with both a pointer and an index to check, we can be pretty sure we
//  aren't getting a bogus item.
//
//  It important for code that uses this scheme for things like whether
//  it should log something in highly traveled code or to store stats in
//  often accessed code to do this lookup for any stuff that is checked
//  often, and do subsequent checks via the pre-looked up item. The cache
//  item class is very simple so it will never represent any issue to define
//  them statically at class or file scope and then fault them in via first
//  use or on the first constructor of that class type.
//
//  The key of an item can be gotten to via the cache item class. Since the
//  items are never removed, this is safe and it can just provide access to
//  the constant key name pointer in the actual item it points to, so this is
//  very efficient. No sync is required for this since, once faulted in,
//  access to the key is read only.
//
//  Iteration of the lists can use CIDLib level functionality, because it's
//  only used for display purposes and such. So we can fill in a collection
//  of cache items that meet some sort of criteria. We provide a simple
//  item info class for one style of value query.
//
// CAVEATS/GOTCHAS:
//
//  1)  All in all this stuff has to be failsafe. It cannot throw exceptions
//      because it's used in exception handling very often. It can't use
//      CIDLib level functionality, so it's very simple and only uses basic
//      data types. The only way it can represent errors in its own usage is
//      via some items that it updates itself.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class TStatsCacheNode;


// ---------------------------------------------------------------------------
//   CLASS: TStatsCacheItem
//  PREFIX: sci
// ---------------------------------------------------------------------------
class CIDLIBEXP TStatsCacheItem
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStatsCacheItem();

        TStatsCacheItem
        (
            const   tCIDLib::TCard4         c4Id
            ,       TStatsCacheNode*        pscnData
        );

        TStatsCacheItem(const TStatsCacheItem&) = default;
        TStatsCacheItem(TStatsCacheItem&&) = default;

        ~TStatsCacheItem();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStatsCacheItem& operator=(const TStatsCacheItem&) = default;
        TStatsCacheItem& operator=(TStatsCacheItem&&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        [[nodiscard]] tCIDLib::TBoolean bHasValidData
        (
                    TStatsCacheNode*&       pscnToFill
            , COP   tCIDLib::TCard4&        c4Id
        );

        [[nodiscard]] tCIDLib::TBoolean bHasValidData
        (
            const   TStatsCacheNode*&       pscnToFill
            , COP   tCIDLib::TCard4&        c4Id
        )   const;

        [[nodiscard]] tCIDLib::TCard4 c4Id() const;

        [[nodiscard]] tCIDLib::TCard8 c8Stamp() const;

        [[nodiscard]] tCIDLib::EStatItemTypes eType() const;

        const tCIDLib::TCh* pszKey() const;

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard4         c4Id
            , CIP   TStatsCacheNode* const  pscnData
        );


    private :
        // -------------------------------------------------------------------
        //  Private data values
        //
        //  m_c4Id
        //      The unique ID of the node we hold, which is used by clients
        //      to relate polled values back to stat items they know about.
        //
        //  m_pscnData
        //      An opaque pointer to the node represented. Zero if this cache item
        //      hasn't been set yet. We don't own it, so it's ok to just hold a
        //      pointer and copy the pointer when we are copied or assigned.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4Id;
        TStatsCacheNode*    m_pscnData;
};



// ---------------------------------------------------------------------------
//   CLASS: TStatsCacheItemInfo
//  PREFIX: scii
// ---------------------------------------------------------------------------
class CIDLIBEXP TStatsCacheItemInfo : public TObject, public MStreamable
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::ESortComps eCompNames
        (
            const   TStatsCacheItemInfo&    scii1
            , const TStatsCacheItemInfo&    scii2
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStatsCacheItemInfo();

        TStatsCacheItemInfo
        (
            const   tCIDLib::TCh* const     pszName
            , const tCIDLib::TCard4         c4Id
            , const TStatsCacheNode&        scnSrc
        );

        TStatsCacheItemInfo(const TStatsCacheItemInfo&)  = default;
        TStatsCacheItemInfo(TStatsCacheItemInfo&&)  = default;

        ~TStatsCacheItemInfo();


        // -------------------------------------------------------------------
        //  Public oeprators
        // -------------------------------------------------------------------
        TStatsCacheItemInfo& operator=(const TStatsCacheItemInfo&) = default;
        TStatsCacheItemInfo& operator=(TStatsCacheItemInfo&&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TStatsCacheItemInfo&    sciiSrc
        )   const;

        tCIDLib::TBoolean operator!=(const TStatsCacheItemInfo&  sciiSrc) const
        {
            return !operator==(sciiSrc);
        }


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        [[nodiscard]] tCIDLib::TCard4 c4Id() const;

        [[nodiscard]] tCIDLib::TCard8 c8Stamp() const;

        tCIDLib::TCard8 c8Stamp
        (
            const   tCIDLib::TCard8         c8ToSet
        );

        [[nodiscard]] tCIDLib::TCard8 c8Value() const;

        [[nodiscard]] tCIDLib::EStatItemTypes eType() const;

        [[nodiscard]] const TString& strName() const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
            CIOP    TBinInStream&           strmToReadFrom
        )   final;

        tCIDLib::TVoid StreamTo
        (
            CIOP    TBinOutStream&          strmToWriteTo
        )   const final;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Id
        //      The unique id of the item we represent.
        //
        //  m_c8Stamp
        //      Used for polling purposes. It gets the caller the original
        //      stamp and he can update it to hold new stamps as he polls.
        //
        //  m_c8Value
        //      This is the value of the item we represent
        //
        //  m_strName
        //      The relative or fully qualified name of the item we represent.
        //      Whether it's relative or FQ, depends on how it was gotten.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4Id;
        tCIDLib::TCard8         m_c8Stamp;
        tCIDLib::TCard8         m_c8Value;
        tCIDLib::EStatItemTypes m_eType;
        TString                 m_strName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStatsCacheItemInfo,TObject)
};


// ---------------------------------------------------------------------------
//  NAMESPACE: TStatsCache
// ---------------------------------------------------------------------------
namespace TStatsCache
{
    using TItemList  = TCollection<TStatsCacheItem>;
    using TInfoList  = TCollection<TStatsCacheItemInfo>;
    using TIDList    = TFundVector<tCIDLib::TCard4>;
    using TValueList = TFundVector<tCIDLib::TCard8>;


    // Methods to check a specific bit in bit flag types
    CIDLIBEXP tCIDLib::TBoolean bCheckBitFlag
    (
        const   TStatsCacheItem&        sciToUse
        , const tCIDLib::TCard4         c4BitNum
    );


    // Methods to check flag type items
    CIDLIBEXP tCIDLib::TBoolean bCheckFlag
    (
        const   tCIDLib::TCh* const     pszPath
        , CIOP  TStatsCacheItem&        sciToUse
    );

    CIDLIBEXP tCIDLib::TBoolean bCheckFlag
    (
        const   tCIDLib::TCh* const     pszPath
    );

    CIDLIBEXP tCIDLib::TBoolean bCheckFlag
    (
        const   TStatsCacheItem&        sciToUse
    );


    //
    //  Efficient polling of values in a scope, returning only those
    //  values that have changed. You have to first call c4QueryValuesInScope
    //  and get the list of items and a stamp. You have to pass that stamp
    //  back to this method. If the list has changed, you'll get false
    //  back and need to go back and start over. Otherwise, you will get
    //  a list of items in that scope which have changed since the last
    //  time you asked. The first time you call this, you will get back a
    //  poll stamp, which you pass in on subsequent queries, which will
    //  let it know which values have changed since you last queried. The
    //  first time, pass in zero.
    //
    //  It fills in two lists. One holds the ids of the items returned,
    //  and the other holds a parallel list of the values of those changed
    //  items.
    //
    CIDLIBEXP tCIDLib::TBoolean bPollValuesInScope
    (
        const   TString&                strScope
        , const tCIDLib::TBoolean       bDirectOnly
        , const tCIDLib::TCard8         c8QueryStamp
        , CIOP  tCIDLib::TCard8&        c8PollStamp
        , COP   TIDList&                fcolIdList
        , COP   TValueList&             fcolValueList
    );


    // Methods to set flag type items
    CIDLIBEXP tCIDLib::TBoolean bSetFlag
    (
        const   tCIDLib::TCh* const     pszPath
        , CIOP  TStatsCacheItem&        sciToUse
        , const tCIDLib::TBoolean       bNewState
    );

    CIDLIBEXP tCIDLib::TBoolean bSetFlag
    (
        const   tCIDLib::TCh* const     pszPath
        , const tCIDLib::TBoolean       bNewState
    );

    CIDLIBEXP tCIDLib::TBoolean bSetFlag
    (
        CIOP    TStatsCacheItem&        sciToUse
        , const tCIDLib::TBoolean       bNewState
    );


    // Methods to set high water mark type items
    CIDLIBEXP tCIDLib::TBoolean bSetIfHigher
    (
        const   tCIDLib::TCh* const     pszPath
        , CIOP  TStatsCacheItem&        sciToUse
        , const tCIDLib::TCard8         c8ToSet
    );

    CIDLIBEXP tCIDLib::TBoolean bSetIfHigher
    (
        CIOP    TStatsCacheItem&        sciToUse
        , const tCIDLib::TCard8         c8ToSet
    );


    //
    //  An alternative setter for low frequency sets by outside code, for things
    //  like setting logging levels or other control flags. The return indicates
    //  whether the item indicated by the path exists or not and could be set or
    //  not. No item is created if it doesn't exist and no stats cache items info
    //  is returned.
    //
    //  We have another one to set specific bits in the value, which is often
    //  desired for control flag type stuff.
    //
    //  For reading, it's assumed that the normal value reading methods will be
    //  used, where the caller provides the stats cache item, for efficiency.
    //
    CIDLIBEXP tCIDLib::TBoolean bSetValue
    (
        const   tCIDLib::TCh* const     pszPath
        , const tCIDLib::TCard8         c8ToSet
    );

    CIDLIBEXP tCIDLib::TBoolean bSetFlagBit
    (
        const   tCIDLib::TCh* const     pszPath
        , const tCIDLib::TCard4         c4BitNum
        , const tCIDLib::TBoolean       bState
    );


    //
    //  Get a list of items that meet some criteria.
    //
    CIDLIBEXP tCIDLib::TCard4 c4QueryItemsInScope
    (
        const   TString&                strScope
        , COP   TItemList&              colToFill
    );


    //
    //  Get the names (just the relative part) and values of each of the items
    //  under a given parent scope. They can ask for only direct children, or
    //  all children. They also get a stamp which is the time stamp of the
    //  last addition of a new item (not necessarily under the indicated
    //  scope just changes to the list.) They can pass that stamp back to
    //  bPollValuesInScope().
    //
    //  If you want all items, set the stamp to zero on the way in. If you
    //  want items that have been added since a previous query, then pass
    //  back in the previous stamp. If getting this from a remote app and it
    //  has restarted, that will get you everything effectively (as long as
    //  clocks are reasonably in sync.)
    //
    CIDLIBEXP tCIDLib::TCard4 c4QueryValuesInScope
    (
        const   TString&                strScope
        , CIOP  TInfoList&              colToFill
        , const tCIDLib::TBoolean       bDirectOnly
        , COP   tCIDLib::TCard8&        c8Stamp
        , const tCIDLib::TBoolean       bClearFirst = kCIDLib::True
    );


    // Return the count of bad item refs we've gotten
    CIDLIBEXP tCIDLib::TCard8 c8BadItemRefs();


    // Methods to check value/counter type items
    CIDLIBEXP tCIDLib::TCard8 c8CheckValue
    (
        const   tCIDLib::TCh* const     pszPath
        , CIOP  TStatsCacheItem&        sciToUse
    );

    CIDLIBEXP tCIDLib::TCard8 c8CheckValue
    (
        const   tCIDLib::TCh* const     pszPath
    );

    CIDLIBEXP tCIDLib::TCard8 c8CheckValue
    (
        const   TStatsCacheItem&        sciToUse
    );


    // Methods to inc/dec counter type items
    CIDLIBEXP tCIDLib::TCard8 c8DecCounter
    (
        const   tCIDLib::TCh* const     pszPath
        , CIOP  TStatsCacheItem&        sciToUse
    );

    CIDLIBEXP tCIDLib::TCard8 c8DecCounter
    (
        CIOP    TStatsCacheItem&        sciToUse
    );

    CIDLIBEXP tCIDLib::TCard8 c8IncCounter
    (
        const   tCIDLib::TCh* const     pszPath
        , CIOP  TStatsCacheItem&        sciToUse
    );

    CIDLIBEXP tCIDLib::TCard8 c8IncCounter
    (
        CIOP    TStatsCacheItem&        sciToUse
    );


    // Explicitly initialize an item
    CIDLIBEXP tCIDLib::TVoid RegisterItem
    (
        const   tCIDLib::TCh* const     pszPath
        , const tCIDLib::EStatItemTypes eType
        , COP   TStatsCacheItem&        sciToFill
    );


    // Methods to set value type items
    CIDLIBEXP tCIDLib::TVoid SetValue
    (
        const   tCIDLib::TCh* const     pszPath
        , CIOP  TStatsCacheItem&        sciToUse
        , const tCIDLib::TCard8         c8ToSet
    );

    CIDLIBEXP tCIDLib::TVoid SetValue
    (
        CIOP    TStatsCacheItem&        sciToUse
        , const tCIDLib::TCard8         c8ToSet
    );
};

#pragma CIDLIB_POPPACK

