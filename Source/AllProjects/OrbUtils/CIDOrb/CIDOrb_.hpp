//
// FILE NAME: CIDOrb_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/03/2000
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
//  Include our own public header and any of our internal headers that need to
//  be seen by the name spaces below.
// ---------------------------------------------------------------------------
#include    "CIDOrb.hpp"
#include    "CIDOrb_WorkQItem_.hpp"




// ---------------------------------------------------------------------------
//  Internal constants namespace
// ---------------------------------------------------------------------------
namespace kCIDOrb_
{
    // -----------------------------------------------------------------------
    //  Magic values used in the packet header as brackets around the other
    //  fields, as a sanity check.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4MagicVal  = 0xDEADBEEF;
    const tCIDLib::TCard4   c4MagicVal2 = 0xEADABEBA;

    // -----------------------------------------------------------------------
    //  The modulus for hashing the packet data
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4PacketHashMod = 109;


    // -----------------------------------------------------------------------
    //  The size of the I/O buffer used in some cases for reading in and
    //  formatting out data, mostly on the server side. If it's bigger than
    //  this a temp buffer is allocated.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4SmallIOBufSz = 1024 * 32;
};


// ---------------------------------------------------------------------------
//  Internal types namespace
// ---------------------------------------------------------------------------
namespace tCIDOrb_
{
    // -----------------------------------------------------------------------
    //  We need a queue of work items. One of these is used by each connection
    //  object (for replies) and by the connection manager for the connections
    //  to queue up incoming work to be processed.
    //
    //  It holds instances of our work item pointer, which is a counted pointer
    //  that will release the item back to the facility class' pool when the last
    //  use of it is done.
    // -----------------------------------------------------------------------
    using TWorkQ = TQueue<TWorkQItemPtr>;
}


// ---------------------------------------------------------------------------
//  Include some of the headers
// ---------------------------------------------------------------------------
#include    "CIDOrb_ClientConn_.hpp"


// ---------------------------------------------------------------------------
//  And more headers that need to see the second namespace above
// ---------------------------------------------------------------------------
#include    "CIDOrb_ClientConnMgr_.hpp"

