//
// FILE NAME: CIDObjStore_SeqData_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/24/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDObjStore_SeqData.cpp file, which implements
//  a small class that is used when we need to create a sequential view of
//  the store. We put in records for each item and each free item and then
//  sort by offset. This should give us a sequential record of how the store
//  is laid out, and should not leave any gaps.
//
//  So we have a class that can hold an enum indicating whether its a free
//  item or an index item, and which holds the offset and size of that item.
//  It also holds the index of the item within its respective list, so that
//  we can get back to them quickly.
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
//   CLASS: TOSSeqData
//  PREFIX: osd
// ---------------------------------------------------------------------------
class TOSSeqData : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TOSSeqData();

        TOSSeqData
        (
                    TOSFreeListItem&        fliSrc
            , const tCIDLib::TCard4         c4ListInd
        );

        TOSSeqData
        (
                    TOSStoreItem* const     posiSrc
        );

        TOSSeqData(const TOSSeqData&) = default;

        ~TOSSeqData();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOSSeqData& operator=(const TOSSeqData&) = default;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ListInd() const
        {
            return m_c4ListInd;
        }

        tCIDLib::TCard4 c4Offset() const
        {
            return m_c4Ofs;
        }

        tCIDLib::TCard4 c4Size() const
        {
            return m_c4Size;
        }

        tCIDLib::TVoid Combine
        (
            const   TOSSeqData&             osdToAdd
        );

        tCIDObjStore_::EItemTypes eType() const
        {
            return m_eType;
        }

        TOSStoreItem& osiSrc() const
        {
            return *m_posiSrc;
        }


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  m_c4ListInd
        //      The index of this item into its respective list, i.e. into
        //      the index item array or the free list array. This is only
        //      used for the free list items, since the store items are not
        //      stored in an indexed collection.
        //
        //  m_c4Ofs
        //  m_c4Size
        //      The size and offset of this particular item.
        //
        //  m_eType
        //      Tells us whether this is a free list item or an index item.
        //
        //  m_posiSrc
        //      If its a store item, then this is the pointer to it. Unlike
        //      with free list items, where we store the list index, store
        //      items aren't in an indexed collection. We don't own it of
        //      course, we just reference it, so no ref counting is required
        //      and we can still use default generated copy/assign.
        // -------------------------------------------------------------------
        tCIDLib::TCard4             m_c4ListInd;
        tCIDLib::TCard4             m_c4Ofs;
        tCIDLib::TCard4             m_c4Size;
        tCIDObjStore_::EItemTypes   m_eType;
        TOSStoreItem*               m_posiSrc;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TOSSeqData,TObject)
};

#pragma CIDLIB_POPPACK

