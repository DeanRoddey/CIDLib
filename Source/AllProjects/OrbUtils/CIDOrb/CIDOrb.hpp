//
// FILE NAME: CIDOrb.hpp
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
//  This is the main public header for the facility. It is the single point
//  of contact for the outside world. By including it, they get what we
//  have to offer. It also insures consistent order of inclusion which
//  is needed for the precompiled headers stuff to work.
//
//  This is not a large facility, but it's complex and highly multi-threaded
//  and async in nature. There are two sides, client and server, each of which
//  is implement in a few classes apiece.
//
//  We define base classes for client side proxies classes and for server
//  side stub classes. These represent the core end points of remote object
//  connections. The IDL compiler will generate derivatives of these that
//  do the right thing for a specific remote object interface, but the ORB
//  only undersands the base interfaces.
//
//  Both sides share some common sending/receiving methods in the facility
//  class, which do the grunt work of sending and receiving our command
//  packets. These are composed of a header and a following packet body.
//
//  The client side works like this:
//
//      1. The ClientBase file provides the client side smarts. Every
//      proxy object created (a class derived from the client side base
//      class), needs to talk to a particular server. So the ClientBaseHelpers
//      file provides a TSrvTarget class that provides that connection.
//      the base client side class reference counts server target objects, so
//      that as long as there is someone using a server target, it will be
//      kept alive.
//
//      2. When a proxy object wants to make a remote call, he calls the
//      Dispatch method of the base client side class. That guy finds the
//      target object for the proxy (or creates one doesn't exist) and queues
//      up a TCmdQItem with the info, on an outgoing queue, and marks it
//      as being ready to send. The calling thread then blocks on an
//      event that is in the queue item object.
//
//      3. The server target object has a spooling thread that sends out
//      queued calls and gets back replies. The CmdQ objects have a status
//      and as it makes it through each step, that status is updated. It's
//      first sent, and then moved to a reply queue to wait for a reply.
//      Then whe the reply comes back, the status is updated again and the
//      event is triggered, and the item is removed from queue and marked
//      ready. The event trigger wakes up the waiting thread, which sees
//      the item is ready and gets the info out and returns, marking the
//      item free again for use by another thread.
//
//      4. If the waiting thread gives up waiting (it has a timeout value
//      set by the IDL compiler on each method), then it will mark the item
//      as orphaned. The spooler thread will see this eventually and free
//      the item again.
//
//      5. The server target object has a socket which is managed by the
//      spooler thread also. The target object is always in either reconn
//      mode, meaning he's trying to connect to his server, or he is connected
//      and is ready to go.
//
//      The spooler thread has two event objects, one it associates with
//      the socket, so that it will be triggered when data is ready. The other
//      is triggered by the incoming threads from #2 when it calls in to
//      queue up an outgoing CmdQ item. So the thread does a multi-block
//      call, and each time he wakes up, he looks to see which one of them
//      was triggered, and either reads available replies and wakes up the
//      waiting threads, or it sends out outgoing commands.
//
//      6. If the connection is lost, then the spooler thread will just
//      put an error return into each command in his outgoing or incoming
//      queue, and wake up the threads. They will see this and return the
//      error to their caller (by throwing.)
//
//      7. The server targets will periodically send a ping to the server
//      if no outgoing activity to the server has occured in over half
//      the maximum idle time (the length of time the server side will wait
//      before assuming a client is lost and dropping the connection.)
//
//      8. When the last proxy referencing a server target drops it, that
//      target is moved to a scavenger queue for a short time. This way,
//      if another proxy accesses it shortly thereafter, we don't have to
//      rebuild it. They can stay their for some number of seconds, using
//      a time stamp in the server target object to keep up with how long
//      it's been there. The scavenger thread checks periodically and destroys
//      those that have been in the scanveger queue for too long. This adds
//      complexity, but it's a very important optimization.
//
//      9. There are a few levels of locking here, so care has to be taken
//      not to deadlock. There is an overall lock, reuired to change/search
//      the list of targets. The is a cache of command queue objects, which
//      is a thread safe collection, locked separately due to a deadlock
//      issue (and performance issue) of using the main lock for getting
//      items from the poll for use. The main lock is also used to coordinate
//      access to the scavenger queue. And finally each command queue object
//      must be locked during updates to its status, since both the waiting
//      client thread and the ORB threads processing it have to access it
//      potentially at the same time. It woudl be a big bottleneck to have
//      to use the main lock just to update each command queue object's
//      status.
//
//      So the rule is that the main lock must be done first, if it is going
//      to be done, and then the others. This must be enforced down the call
//      tree as well, so a called method cannot lock the main lock if an
//      upstream call has locked one of the others. But generally this is
//      not an issue since the other locks are very localized.
//
//      And the main lock must not be held across any long operations, since
//      it will hold up all threads.
//
//  So that that's the client side in a nutshell. The server side works like
//  this:
//
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

// ---------------------------------------------------------------------------
//  Set up our import/export attributes
// ---------------------------------------------------------------------------
#if     defined(PROJ_CIDORB)
#define CIDORBEXP   CID_DLLEXPORT
#else
#define CIDORBEXP   CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers if not already included
// ---------------------------------------------------------------------------
#include    "CIDSock.hpp"
#include    "CIDCrypto.hpp"


// ---------------------------------------------------------------------------
//  Include the fundamental headers first
// ---------------------------------------------------------------------------
#include    "CIDOrb_ErrorIds.hpp"
#include    "CIDOrb_MessageIds.hpp"
#include    "CIDOrb_Constant.hpp"
#include    "CIDOrb_Type.hpp"

#include    "CIDOrb_Id.hpp"
#include    "CIDOrb_ObjId.hpp"
#include    "CIDOrb_Reply.hpp"
#include    "CIDOrb_Cmd.hpp"
#include    "CIDOrb_ClientBase.hpp"
#include    "CIDOrb_ServerBase.hpp"
#include    "CIDOrb_ObjList.hpp"
#include    "CIDOrb_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the lazy evalution method for the facility object
// ---------------------------------------------------------------------------
extern CIDORBEXP TFacCIDOrb& facCIDOrb();

