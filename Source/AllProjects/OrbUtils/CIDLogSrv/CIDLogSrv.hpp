//
// FILE NAME: CIDLogSrv.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/28/2001
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
//  This is the main header of the CIDLib logging service. This program is
//  a backend program which implements a simple CIDIDL interface. This
//  interface allows other programs to log their errors and messages to a
//  central server, an important aspect of any distributed processing system.
//
//  The IDL interface is from CIDOrbUC, so anyone else could make their own
//  log server, but this implementation will likely be good enough for most
//  systems. The API provides both for clients to log their errors and messages,
//  and a query API via which other clients can get live access to them as
//  they happen, or to query currently stored stuff via a few different
//  criteria.
//
//  The program maintains the logged data in a binary file, which is keyed
//  on the time stamp of when the event was logged. The format of the file
//  looks like this:
//
//      HeaderInfo
//      KeyArray
//      FreeListArray
//      Data/FreeData
//
//  So it starts with a small header that saves some state data, such as the
//  sequence number applied to the last logged event, followed by two arrays.
//  The first is the key array, which contains the ofs/size entries for each
//  log event in the file, in sorted order by the time stamp. Next is the
//  free list that holds the size/ofs of each free chunk.
//
//  Following that is a pool of data in which logged events and free space
//  are intermixed, all of which is kept track of by the two arrays.
//
//  A scavenger/compactor thread wakes up every 10 minutes and gets rid of
//  any items which are over 7 days old. Otherwise, the file will expand
//  to hold up to 7 days worth of events. It just writes another chunk of
//  data at the end of the file and creates a new free list entry for it.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDOrbUC.hpp"
#include    "CIDRegx.hpp"


// <TBD> Remove when done with debugging
//#define CIDLOGSRV_DEBUG 1


// ---------------------------------------------------------------------------
//  And sub-include our core headers in the needed order
// ---------------------------------------------------------------------------
#include    "CIDLogSrv_ErrorIds.hpp"
#include    "CIDLogSrv_MessageIds.hpp"


// ---------------------------------------------------------------------------
//  And sub-include our higher level headers in the needed order
// ---------------------------------------------------------------------------
#include    "CIDLogSrv_Type.hpp"
#include    "CIDLogSrv_Constant.hpp"
#include    "CIDLogSrv_CoreAdminImpl.hpp"
#include    "CIDLogSrv_Impl.hpp"
#include    "CIDLogSrv_ThisFacility.hpp"



// ---------------------------------------------------------------------------
//  Make the facility object visible to all the program files. It is actually
//  declared in the main cpp file.
// ---------------------------------------------------------------------------
extern TFacCIDLogSrv   facCIDLogSrv;

