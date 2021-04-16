//
// FILE NAME: CIDLogSrv_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/28/2001
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
//  This is the header for the the non-class types of this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

namespace tCIDLogSrv
{
    // -----------------------------------------------------------------------
    //  The fields available to create sorted keys on the key list
    // -----------------------------------------------------------------------
    enum class EKeyListKeys
    {
        LoggedTime
        , Severity
        , Offset

        , Count
    };


    // -----------------------------------------------------------------------
    //  The fields available to create sorted keys on the free list
    // -----------------------------------------------------------------------
    enum class EFreeListKeys
    {
        Offset

        , Count
    };


    // -----------------------------------------------------------------------
    //  The log file header structure
    //
    //  c4Marker
    //      A marker value to give some clue this is a valid file. It is the
    //      ASCII string "CIDLOG", with a null terminator.
    //
    //  c4FmtVersion
    //      A format version, so that we can automatically move forward old
    //      data, though we seldom need to, since log files are not moved
    //      forward after a new install.
    //
    //  c4LastSeq
    //      Each new logged event is assigned a sequence number. When we shut
    //      down, and restart again, we have to remember where we were.
    //
    //  c4FreesUsed
    //  c4KeysUsed
    //      This is where we store the number of entries used in the key and
    //      free lists. We could figure it out from scanning for all of them
    //      whose bUsed field is set, but this is more convenient.
    //
    //  ac1Reserved
    //      Allows us to add some more header keys in subsequent new program
    //      versions, without having to rewrite the file.
    // -----------------------------------------------------------------------
    struct TFileHeader
    {
        tCIDLib::TSCh           achMarker[7];
        tCIDLib::TCard4         c4FmtVersion;
        tCIDLib::TCard4         c4LastSeq;
        tCIDLib::TCard4         c4FreesUsed;
        tCIDLib::TCard4         c4KeysUsed;
        tCIDLib::TCard1         ac1Reserved[64];
    };


    // -----------------------------------------------------------------------
    //  In the store area of the log file, these structures represent a logged
    //  event. We store the offset and size of the streamed data, and a couple
    //  of fields copied out of the event object. These allow us to do fast in
    //  memory sorting on these fields. Sorts on other fields, if they are
    //  supported, will be much slower and require reading the whole file.
    //
    //  The list is not maintained sorted, we just create sorted keys as
    //  needed, using pointers to the list contents.
    //
    //  c4Ofs
    //  c4Size
    //      The size and offset within the free store of the flattened event.
    //
    //      NOTE:   The offset is relative to the start of the free store area,
    //              not the file origin. This makes them independent of where
    //              the free store area actually starts.
    //
    //  enctLogged
    //      The time the event was logged. We copy this out of the logged
    //      event before flattening it.
    //
    //  c1Class
    //  c1Sev
    //      The class and severity level of the event. We copy this out of
    //      the logged event. Since these are reasonably sized, and are the
    //      most common filter criteria, it can vastly speed up filtered
    //      log queries.
    //
    //      Note that we reduce them to TCard1 values since we know that they
    //      will never have values beyond 255, even close to that, and it
    //      significantly reduces the key list size.
    //
    //  c4Seq
    //      Each newly logged item is assigned a new sequence number. The only
    //      reason we need it in the live in memory key is so that, when we
    //      sort, events at the same time can be disambiguated using the
    //      sequence number to make sure that they are in the right order.
    // -----------------------------------------------------------------------
    struct TKeyItem
    {
        tCIDLib::TCard4         c4Ofs = 0;
        tCIDLib::TCard4         c4Size = 0;
        tCIDLib::TEncodedTime   enctLogged = 0;
        tCIDLib::TCard1         c1Sev = 0;
        tCIDLib::TCard4         c1Class = 0;
        tCIDLib::TCard4         c4Seq = 0;
    };


    // -----------------------------------------------------------------------
    //  In the free store area of the log file, these structures represent a
    //  contiguous block of free memory. We maintain this list compacted, but
    //  not sorted.
    //
    //  The list is not maintained sorted, we just create sorted keys as
    //  needed, using pointers to the list contents.
    //
    //  c4Ofs
    //  c4Size
    //      The size and offset within the free store of the free block.
    //
    //      NOTE:   The offset is relative to the start of the free store area,
    //              not the file origin. This makes them independent of where
    //              the free store area actually starts.
    // -----------------------------------------------------------------------
    struct TFreeItem
    {
        tCIDLib::TCard4         c4Ofs = 0;
        tCIDLib::TCard4         c4Size = 0;
    };


    // -----------------------------------------------------------------------
    //  Aliases for the comparator functions we'll use for passing to the
    //  standard sort
    // -----------------------------------------------------------------------
    using TFreeComparator = tCIDLib::ESortComps (*)(const TFreeItem&, const TFreeItem&);
    using TKeyComparator = tCIDLib::ESortComps (*)(const TKeyItem&, const TKeyItem&);


    // -----------------------------------------------------------------------
    //  Used for creating a combined sorted offset list
    // -----------------------------------------------------------------------
    enum class ETypes
    {
        Free
        , Key
    };

    struct TSortItem
    {
        ETypes              eType;
        tCIDLib::TCard4*    pc4This;
    };
}

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  Create comparator functions for the free and key items.
// ---------------------------------------------------------------------------
inline tCIDLib::ESortComps
eCompFreeItem(  const   tCIDLogSrv::TFreeItem& item1
                , const tCIDLogSrv::TFreeItem& item2)
{
    if (item1.c4Ofs < item2.c4Ofs)
        return tCIDLib::ESortComps::FirstLess;
    else if (item1.c4Ofs > item2.c4Ofs)
        return tCIDLib::ESortComps::FirstGreater;
    return tCIDLib::ESortComps::Equal;
}


inline tCIDLib::ESortComps
eKeyCompOfs(const   tCIDLogSrv::TKeyItem& item1
            , const tCIDLogSrv::TKeyItem& item2)
{
    if (item1.c4Ofs < item2.c4Ofs)
        return tCIDLib::ESortComps::FirstLess;
    else if (item1.c4Ofs > item2.c4Ofs)
        return tCIDLib::ESortComps::FirstGreater;
    return tCIDLib::ESortComps::Equal;
}

inline tCIDLib::ESortComps
eKeyCompTime(const  tCIDLogSrv::TKeyItem& item1
            , const tCIDLogSrv::TKeyItem& item2)
{
    if (item1.enctLogged < item2.enctLogged)
        return tCIDLib::ESortComps::FirstLess;
    else if (item1.enctLogged > item2.enctLogged)
        return tCIDLib::ESortComps::FirstGreater;

    // The time is the same, so use the sequence number
    if (item1.c4Seq < item2.c4Seq)
        return tCIDLib::ESortComps::FirstLess;
    else if (item1.c4Seq > item2.c4Seq)
        return tCIDLib::ESortComps::FirstGreater;
    return tCIDLib::ESortComps::Equal;
}


inline tCIDLib::ESortComps
eCompSortItem(  const   tCIDLogSrv::TSortItem& item1
                , const tCIDLogSrv::TSortItem& item2)
{
    if (*item1.pc4This < *item2.pc4This)
        return tCIDLib::ESortComps::FirstLess;
    else if (*item1.pc4This > *item2.pc4This)
        return tCIDLib::ESortComps::FirstGreater;
    return tCIDLib::ESortComps::Equal;
}
