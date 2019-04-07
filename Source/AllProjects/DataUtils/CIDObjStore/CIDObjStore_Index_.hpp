//
// FILE NAME: CIDObjStore_Index_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/01/2003
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
//  This is the header for the CIDObjStore_Index.cpp file which implements the
//  classes that make up the in memory index data. This data is never written
//  to disk. It is inferred from the store when the store is opened, and then
//  maintained in memory during runtime.
//
//  Each item in the index represents a stored object or a free slot in
//  the store. If sorted by offset, the items should account for every byte
//  in the store.
//
//  The free list items just need to keep up with offset and size of the
//  free slot. The store items need to keep up with the offset and size as
//  well, but also info about the object stored.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: TOSFreeListItem
//  PREFIX: fli
// ---------------------------------------------------------------------------
class TOSFreeListItem : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TOSFreeListItem();

        TOSFreeListItem
        (
            const   tCIDLib::TCard4         c4Size
            , const tCIDLib::TCard4         c4Offset
        );

        TOSFreeListItem
        (
            const   TOSFreeListItem&        fliToCopy
        );

        ~TOSFreeListItem();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOSFreeListItem& operator=
        (
            const   TOSFreeListItem&        fliToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddToSize
        (
            const   TOSFreeListItem&        fliToTake
        );

        tCIDLib::TVoid AddToSize
        (
            const   tCIDLib::TCard4         c4ToAdd
        );

        tCIDLib::TCard4 c4AfterIt() const;

        tCIDLib::TCard4 c4Offset() const;

        tCIDLib::TCard4 c4Offset
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4Size() const;

        tCIDLib::TCard4 c4Size
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid MarkUnused();

        tCIDLib::TVoid ReduceSize
        (
            const   tCIDLib::TCard4         c4ToSub
        );

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard4         c4Size
            , const tCIDLib::TCard4         c4Offset
        );

        tCIDLib::TVoid TakeFromBottom
        (
            const   tCIDLib::TCard4         c4ToTake
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Offset
        //  m_c4Size
        //      The offset of this free list item in the store, and it's size
        //      in bytes.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Offset;
        tCIDLib::TCard4 m_c4Size;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TOSFreeListItem,TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TOSStoreItem
//  PREFIX: osi
// ---------------------------------------------------------------------------
class TOSStoreItem : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strKey
        (
            const   TOSStoreItem&           osiSrc
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TOSStoreItem();

        TOSStoreItem
        (
            const   tCIDLib::TCard4         c4Offset
            , const tCIDLib::TCard4         c4CurUsed
            , const tCIDLib::TCard4         c4Allocated
            , const tCIDLib::TCard4         c4Version
            , const TString&                strKey
            , const tCIDLib::TCard4         c4KeyBytes
        );

        TOSStoreItem
        (
            const   TOSStoreItem&           osiToCopy
        );

        ~TOSStoreItem();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOSStoreItem& operator=
        (
            const   TOSStoreItem&           osiToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddToAllocated
        (
            const   TOSFreeListItem&        fliToTake
        );

        tCIDLib::TVoid AddToAllocated
        (
            const   tCIDLib::TCard4         c4ToAdd
        );

        tCIDLib::TCard4 c4Allocated() const;

        tCIDLib::TCard4 c4Allocated
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4CurUsed() const;

        tCIDLib::TCard4 c4CurUsed
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4KeyBytes() const;

        tCIDLib::TCard4 c4Offset() const;

        tCIDLib::TCard4 c4Offset
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4NewStorageRequired
        (
            const   tCIDLib::TCard4         c4NewSize
            , const tCIDLib::TCard4         c4NewReserve
        )   const;

        tCIDLib::TCard4 c4Reserve() const;

        tCIDLib::TCard4 c4StorageRequired() const;

        tCIDLib::TCard4 c4Version() const;

        tCIDLib::TCard4 c4Version
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid IncVersion();

        tCIDLib::TVoid MarkUnused();

        const TString& strKey() const;

        const TString& strName() const;

        const TString& strScope() const;

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard4         c4Offset
            , const tCIDLib::TCard4         c4CurUsed
            , const tCIDLib::TCard4         c4Allocated
            , const tCIDLib::TCard4         c4Version
            , const TString&                strKey
            , const tCIDLib::TCard4         c4KeyBytes
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BreakOutScope();

        tCIDLib::TVoid CheckAllocUsed
        (
            const   tCIDLib::TCard4         c4Allocated
            , const tCIDLib::TCard4         c4Used
            , const tCIDLib::TCard4         c4Line
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Allocated
        //      The bytes available in the object's current store slot. If it
        //      is equal to the m_c4CurUsed, then no more expansion space is
        //      available.
        //
        //  m_c4CurUsed
        //      The amount of this object's storage slot used. If it is equal
        //      to m_c4Allocated, then there is no more expansion space
        //      available for this object. If it grows, it has to be moved.
        //
        //  m_c4KeyBytes
        //      The bytes required by the flattened key data. We were given
        //      this in the ctor, or by the Set() method.
        //
        //  m_c4Offset
        //      The offset of this object in the store file.
        //
        //  m_c4VersionNum
        //      The current version number of this object. It is bumped each
        //      time the object is written to. This allows for quick 'has it
        //      changed' checks.
        //
        //  m_strKey
        //      The key for this object, i.e. it's 'store path'.
        //
        //  m_strName
        //      We store the full key in m_strKey, but we also break out the
        //      name part here, to speed up certain operations.
        //
        //  m_strScope
        //      We store the full key in m_strKey, but we also break out the
        //      scope here, to speed up certain operations.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4Allocated;
        tCIDLib::TCard4     m_c4CurUsed;
        tCIDLib::TCard4     m_c4KeyBytes;
        tCIDLib::TCard4     m_c4Offset;
        tCIDLib::TCard4     m_c4VersionNum;
        TString             m_strKey;
        TString             m_strName;
        TString             m_strScope;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TOSFreeListItem,TObject)
};


#pragma CIDLIB_POPPACK

