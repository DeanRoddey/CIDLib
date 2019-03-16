//
// FILE NAME: CIDObjStore_Implementation2.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/17/2002
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the grunt work methods that are related to searching
//  and manipulating the free list or item index. These methods in turn use
//  methods in the 3rd file which manipulate the actual files.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDObjStore_.hpp"



// ---------------------------------------------------------------------------
//  TCIDObjStoreImpl: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Creates a combined index of free and used items, sorted by their offsets,
//  so that this list should represent a complete, linear coverage of the
//  whole store file.
//
//  As an optimization, the caller can also ask to get back the index of an
//  item at a particular target offset.
//
tCIDLib::TCard4
TCIDObjStoreImpl::c4CreateSeqData(          TVector<TOSSeqData>& colToFill
                                    ,       tCIDLib::TCard4&     c4IndexItems
                                    ,       tCIDLib::TCard4&     c4FreeItems
                                    , const tCIDLib::TCard4      c4TargetOfs)
{
    // Clear the vector out first and init the counter parms
    colToFill.RemoveAll();
    c4IndexItems = 0;
    c4FreeItems = 0;

    // First add the store items
    tCIDLib::TCard4 c4Ret = kCIDLib::c4MaxCard;
    TStoreList::TNCCursor cursStore(&m_colStoreList);
    for (; cursStore; ++cursStore)
    {
        TOSStoreItem& osiCur = *cursStore;

        // If it's the target item, return this index
        if (osiCur.c4Offset() == c4TargetOfs)
            c4Ret = c4IndexItems;

        colToFill.objAdd(TOSSeqData(&osiCur));
        c4IndexItems++;
    }

    // And add the free list items that are in use
    const tCIDLib::TCard4 c4Count = m_colFreeList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TOSFreeListItem& fliCur = m_colFreeList[c4Index];

        //
        //  We are never supposed to have a free list item of less than
        //  a minimum sizebytes. So if debugging, then check it.
        //
        #if CID_DEBUG_ON
        if (fliCur.c4Size() < kCIDObjStore_::c4MinFreeItemSize)
        {
            facCIDObjStore().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kObjSErrs::errcDbg_SubMinFreeSlot
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppStatus
                , TCardinal(fliCur.c4Size())
                , m_strStoreName
                , TCardinal(kCIDObjStore_::c4MinFreeItemSize)
            );
        }
        #endif

        colToFill.objAdd(TOSSeqData(fliCur, c4Index));
        c4FreeItems++;
    }

    // And now sort the list by their offsets
    colToFill.Sort
    (
        [](const TOSSeqData& osd1, const TOSSeqData& osd2)
        {
            if (osd1.c4Offset() < osd2.c4Offset())
                return tCIDLib::ESortComps::FirstLess;
            else if (osd1.c4Offset() > osd2.c4Offset())
                return tCIDLib::ESortComps::FirstGreater;
            return tCIDLib::ESortComps::Equal;
        }
    );
    return c4Ret;
}


//
//  When we need to write a new object out, this method is called in order to
//  make a hole to write it into. It will look for a free list item with the
//  space to hold the new item. The rules are:
//
//  1.  Prefers a free item that can be completely used. So if the free item
//      is the same size or just a little larger than the requested bytes,
//      it'll take that one, in order to reduce fragementation.
//
//  2.  If it can't take a whole one, takes part of one. In order to be
//      failsafe, it will actually split the item and create a new free list
//      item of just the size required, and return that. This way, we get
//      back to a completely coherent object store before return, so if we
//      should fail before the new item get's written there, there won't be
//      any danger of having a stranded bit of the store.
//
tCIDLib::TCard4
TCIDObjStoreImpl::c4FindFreeSpace(  const   tCIDLib::TCard4     c4Needed
                                    ,       tCIDLib::TCard4&    c4ActualSz
                                    ,       tCIDLib::TCard4&    c4Offset)
{
    //
    //  Make one run through the list and see if we can find that we can
    //  take outright. While looking, we'll keep up with the smallest one
    //  that is big enough, which we'll use otherwise.
    //
    const tCIDLib::TCard4 c4Count = m_colFreeList.c4ElemCount();
    tCIDLib::TCard4 c4Index = 0;

    tCIDLib::TCard4 c4SmallestInd = kCIDLib::c4MaxCard;
    tCIDLib::TCard4 c4SmallestSz = kCIDLib::c4MaxCard;
    for (; c4Index < c4Count; c4Index++)
    {
        TOSFreeListItem& fliCur = m_colFreeList[c4Index];
        const tCIDLib::TCard4 c4Size = fliCur.c4Size();

        //
        //  If it's bigger than our needed size, but not bigger than our
        //  size plus the min free item size, we can take the whole thing.
        //
        if ((c4Size >= c4Needed)
        &&  (c4Size <= c4Needed + kCIDObjStore_::c4MinFreeItemSize))
        {
            // Give bakc this guy'ssize and offset
            c4Offset = fliCur.c4Offset();
            c4ActualSz = c4Size;
            return c4Index;
        }

        if ((c4Size > c4Needed) && (c4Size < c4SmallestSz))
        {
            c4SmallestSz = c4Size;
            c4SmallestInd = c4Index;
        }
    }

    //
    //  That won't work, so let's use the smallest useable one we found
    //  above, if any. If we didn't find one, then there is not a slot
    //  big enough, so we return max card to indicate that.
    //
    if (c4SmallestInd == kCIDLib::c4MaxCard)
        return kCIDLib::c4MaxCard;

    //
    //  We need to chop off a chunk from this free list item, creating
    //  two items, one of which is the one we'll give back. We take it out
    //  of the end of the item, so that we can write a new header wihout
    //  affecting the original one until we know we've got the new stuff
    //  taken. That way, if we die after writing the new header, it won't
    //  make any difference since the original header is still there and
    //  holds the whole chunk still until we update it.
    //
    TOSFreeListItem& fliSrc = m_colFreeList[c4SmallestInd];

    //
    //  Create a new free list item and give it a chunk at the end of
    //  the source item, and write it out. We've not done anything
    //  destructive yet.
    //
    //  NOTE:   This slot cannot be so small that our new offset overwrites
    //          the source slot's header, because we would have taken the
    //          whole slot above! So we know the original free list slot
    //          header remains unchanged so far.
    //
    TOSFreeListItem fliNew(c4Needed, fliSrc.c4AfterIt() - c4Needed);
    WriteFreeListItem(m_flStore, fliNew, m_strStoreName);

    // Now update the original one on disk, after reducing it's size
    fliSrc.ReduceSize(c4Needed);
    WriteFreeListItem(m_flStore, fliSrc, m_strStoreName);

    //
    //  Now we have to add the new free list item to the free list. It has
    //  to go in sorted, so we have to insert it after the source item, since
    //  we took it from the end of the source. The InsertAt() method inserts
    //  before, so we have to insert it after the next one, unless the source
    //  was the last one, in which case we append.
    //
    if (c4SmallestInd + 1 == c4Count)
        m_colFreeList.objAdd(fliNew);
    else
        m_colFreeList.InsertAt(fliNew, c4SmallestInd + 1);

    // Give back what we took and where
    c4ActualSz = fliNew.c4Size();
    c4Offset   = fliNew.c4Offset();

    // Return the index of the new guy
    return c4SmallestInd + 1;
}


//
//  We put the passed chunk back onto the free list, by either giving it to
//  an existing adjacent free list item, or adding a new one.
//
tCIDLib::TCard4
TCIDObjStoreImpl::c4GiveBackChunk(  const   tCIDLib::TCard4     c4Offset
                                    , const tCIDLib::TCard4     c4Size
                                    , const tCIDLib::TBoolean   bNoAdjacent)
{
    // If debugging, make sure its not smaller than the minimum
    #if CID_DEBUG_ON
    if (c4Size < kCIDObjStore_::c4MinFreeItemSize)
    {
        facCIDObjStore().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kObjSErrs::errcDbg_CreatedMinFreeSlot
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
            , TCardinal(c4Size)
            , m_strStoreName
            , TCardinal(kCIDObjStore_::c4MinFreeItemSize)
        );
    }
    #endif

    //
    //  First of all, see if we can find another free item which is adjacent
    //  to this one, i.e. either the end of this one butts up against the
    //  start of another, or the end of another butts up against the start
    //  of this one.
    //
    //  If we find one that has a higher offset than the end of the chunk
    //  we are dropping, then we aren't going to find an adjacent, so we
    //  break out and add a new item.
    //
    const tCIDLib::TCard4 c4AfterIt = c4Offset + c4Size;
    const tCIDLib::TCard4 c4Count = m_colFreeList.c4ElemCount();
    tCIDLib::TCard4 c4Index = 0;
    for (; c4Index < c4Count; c4Index++)
    {
        TOSFreeListItem& fliCur = m_colFreeList[c4Index];

        // If we are beyond hope, break out
        if (fliCur.c4Offset() > c4AfterIt)
            break;

        //
        //  If it starts after our chunk or, if it ends at our chunk, then
        //  give it our chunk, unless we were told not to. If we aren't
        //  doing adjacent coallescing, then we are just finding the correct
        //  insert position.
        //
        if (!bNoAdjacent)
        {
            if (fliCur.c4Offset() == c4AfterIt)
            {
                // Give it our offset and add our size to it's size
                fliCur.Set(fliCur.c4Size() + c4Size, c4Offset);
                WriteFreeListItem(m_flStore, fliCur, m_strStoreName);
                m_c4IndexVersionNum++;
                return c4Index;
            }
             else if (fliCur.c4AfterIt() == c4Offset)
            {
                // Add our size to it's size
                fliCur.AddToSize(c4Size);
                WriteFreeListItem(m_flStore, fliCur, m_strStoreName);
                m_c4IndexVersionNum++;
                return c4Index;
            }
        }
    }

    //
    //  We aren't adjacent to another one, so write a new free list item
    //  header at the appropriate offset in the store
    //
    TOSFreeListItem fliNew(c4Size, c4Offset);
    WriteFreeListItem(m_flStore, fliNew, m_strStoreName);

    //
    //  And put the new item into the free list at the appropriate spot. If
    //  we went to the end without finding beyond our new chunk, then add a
    //  new one at the end. Else, insert before the one that was after us.
    //
    if (c4Index == c4Count)
        m_colFreeList.objAdd(fliNew);
    else
        m_colFreeList.InsertAt(fliNew, c4Index);

    // Bump the index version number and return the index of our guy
    m_c4IndexVersionNum++;
    return c4Index;
}


