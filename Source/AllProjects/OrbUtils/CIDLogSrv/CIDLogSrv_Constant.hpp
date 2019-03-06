//
// FILE NAME: CIDLogSrv_Constant.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/28/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the the constants of this facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

namespace kCIDLogSrv
{
    // -----------------------------------------------------------------------
    //  Query interface constants
    //
    //  c4MaxRetCount
    //      This is the maximum number of events that will be returned in a
    //      single call. Even if they ask for more, it will be clipped to
    //      this amount.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       c4MaxRetCount = 256;


    // -----------------------------------------------------------------------
    //  A format version market that's written in the file header to let
    //  us deal with version changes if we need to.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       c4FmtVersion = 1;


    // -----------------------------------------------------------------------
    //  File related constants
    //
    //  c4ExpandK
    //      The number of K to expand the file if expansion is required. It
    //      MUST be a multile of 4K!
    //
    //  pszLogFileName
    //  pszTmpLogFileName
    //      This is the name of the file we log to. Its path can be changed,
    //      but the name is always the same. And we have the names for a temp
    //      file name we use to create a new file when we have to compact or
    //      expand the file and a backup file name we rename the current one
    //      to when we create a new one.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       c4ExpandK = 256;
    const tCIDLib::TCh* const   pszLogFileName      = L"CIDLogSrv.LogData";
    const tCIDLib::TCh* const   pszTmpLogFileName   = L"CIDLogSrv.TmpLogData";
    const tCIDLib::TCh* const   pszBackLogFileName  = L"CIDLogSrv.BackLogData";


    // -----------------------------------------------------------------------
    //  Limit and size related constants.
    //
    //  c4CutoffDays
    //      This is the length of time we'll keep around log events before
    //      we start throwing them away if the file gets full. I.e. we'll lean
    //      towards throwing away events older than this before we start to
    //      expand the file size.
    //
    //  c4MaxKeys
    //      The maximum number of key items we store, which means this is the
    //      maximum number of events we can store.
    //
    //  c4KeyListSize
    //      The precalculated size of the key list, which makes it more
    //      convenient do some calculations.
    //
    //  c4MaxFrees
    //      The maximum number of free list entries we keep. The absolute
    //      worst case is that the file is divided into c4MaxEntries items
    //      with space between (which would be "keys/2 + 1" free list
    //      entries.) But, because we never let that kind of thing happen,
    //      we keep it to 1/4 the number of keys. And even that is probably
    //      massive overkill.
    //
    //  c4FreeListSize
    //      The precalculated size of the free list, which makes it more
    //      convenient do some calculations.
    //
    //  c4StoreOfs
    //      This the offset where the free list starts. It adds up all of the
    //      header and list stuff. The extra TCard4 is for a separator value
    //      between the two lists, to make it easier to find in a hex editor.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4CutoffDays    = 7;
    const tCIDLib::TCard4   c4MaxKeys       = 8192;
    const tCIDLib::TCard4   c4KeyListSize   = sizeof(tCIDLogSrv::TKeyItem)
                                              * c4MaxKeys;
    const tCIDLib::TCard4   c4MaxFrees      = (c4MaxKeys / 4);
    const tCIDLib::TCard4   c4FreeListSize  = sizeof(tCIDLogSrv::TFreeItem)
                                              * c4MaxFrees;
    const tCIDLib::TCard4   c4StoreOfs      = sizeof(tCIDLogSrv::TFileHeader)
                                              + c4KeyListSize
                                              + c4FreeListSize
                                              + sizeof(tCIDLib::TCard4);
}

