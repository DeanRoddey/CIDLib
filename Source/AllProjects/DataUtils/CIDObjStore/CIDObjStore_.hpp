//
// FILE NAME: CIDObjStore_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/16/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main internal header for the facility. It provides any
//  internal non-class types, constants, enums, etc...
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Include our own public header so folks internally can see it
// ---------------------------------------------------------------------------
#include    "CIDObjStore.hpp"


// ---------------------------------------------------------------------------
//  Internal constants namespace
// ---------------------------------------------------------------------------
namespace kCIDObjStore_
{
    // -----------------------------------------------------------------------
    //  Our current format version
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4FmtVersion    = 1;


    // -----------------------------------------------------------------------
    //  These bits are used in the store, to mark items with certain
    //  characteristics.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard1   c1Flag_Used     = 0x01;


    // -----------------------------------------------------------------------
    //  The modulus used when hashing index data
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4Modulus       = 109;


    // -----------------------------------------------------------------------
    //  The magic values we write at the start of free item or used item
    //  entries in the store.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4FreeMagicVal  = 0xFEDCCDEF;
    const tCIDLib::TCard4   c4UsedMagicVal  = 0xDEADBEEF;
};


// ---------------------------------------------------------------------------
//  Internal types namespace
// ---------------------------------------------------------------------------
namespace tCIDObjStore_
{
    // -----------------------------------------------------------------------
    //  Used by the TOSSeqData class
    // -----------------------------------------------------------------------
    enum class EItemTypes
    {
        None
        , Index
        , FreeList
    };
};


// ---------------------------------------------------------------------------
//  Include any internal headers we need
// ---------------------------------------------------------------------------
#include    "CIDObjStore_Index_.hpp"
#include    "CIDObjStore_SeqData_.hpp"
#include    "CIDObjStore_Implementation_.hpp"


// ---------------------------------------------------------------------------
//  And more internal constants namespace stuff that depends on info in the
//  internal headers above.
// ---------------------------------------------------------------------------
namespace kCIDObjStore_
{
    // -----------------------------------------------------------------------
    //  We never create a free list item with fewer bytes than this, since
    //  no user value could be stored in it. If it would happen, we just give
    //  the extra bytes as reserved bytes for the new item being stored or
    //  expanded. This prevents fragmentation, which is important since we
    //  have a fixed set of free list item slots.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4MinFreeItemSize = sizeof(TCIDObjStoreImpl::TStoreItemHdr);


    // -----------------------------------------------------------------------
    //  The minimum store expansion chunk size
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4MinExpandSize   = 64 * 1024;
}


