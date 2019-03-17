//
// FILE NAME: CIDWebSock.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/24/2017
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
//  This is the main public header for this facility, which implements the guts of a
//  server side WebSockets engine. It provides a TCIDWebSockThread class, which is
//  derived from the standard TThread class. A server would do the usual thing of waiting
//  for connections and then handing off that connection to a class derived from our
//  base web socket thread class.
//
//  The derived class provides us with a data source, so it may be anything that allows
//  for appropriate reading and writing of a stream of data. It may be encrypted via
//  TLS or whatever. We don't care, we just have a data source to work with.
//
//  It is not assumed that these objects will be part of a thread pool. Websockets are
//  for persistent, ongoing connections, so they don't need to be kept around and spun
//  up like many server side threads are when they are often dealing with just a single
//  quick quiery and disconnect.
//
//  We handle web socket negotation, but the containing program and/or derived class
//  has to handle any preliminary work required, such as an upgrade from HTTP for a
//  web server. Once the required work is done, and a data source is created, create an
//  instance of the derived class, passing the data source down to our level, and then
//  start the thread.
//
//  For debugging or simulation purposes, the thread just calls a 'service client'
//  method, which is protected so that derived classes can call it directly in their
//  own thread context if needed.
//
//  We can adopt the data source or not. Generally it's easiest to just let us adopt it
//  and clean it up when the connection is lost. But in debugging or simulation modes
//  it may be easier for the containing application to own the data source.
//
//  Obviously web sockets are event driven on the client side, so they can never block
//  while waiting for the server to reply to something. That means that the server side
//  logic also has to typically work the same, never entering any modal loops, but just
//  being state driven, and so we also work in terms of callbacks to report events.
//
//  If the server cannot handle incoming messages very quickly, it should just queue them
//  up and process them in another thread. The event driven nature of WebSockets means
//  that this doesn't in any way affect the flow of things. It's all already completely
//  async.
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
#if     defined(PROJ_CIDWEBSOCK)
#define CIDWEBSOCKEXP   CID_DLLEXPORT
#else
#define CIDWEBSOCKEXP   CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers if not already included
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"
#include    "CIDSock.hpp"



// ---------------------------------------------------------------------------
//  Public facility types
// ---------------------------------------------------------------------------
namespace tCIDWebSock
{
    // --------------------------------------------------------------------
    //  We need a current state, because the WebSocket system is all event driven and state
    //  based. So each WebSockets thread has a current state based on this enum.
    //
    //  WaitStart
    //      We've been constructed but not started yet.
    //
    //  Connecting
    //      We need to call the virtual method that lets the derived class know we are now
    //      connected and let him initialize as required.
    //
    //  Idle
    //      We are waiting for a message. We call the CML handler's idle time callback
    //      while waiting.
    //
    //  InMsg
    //      We are processing a non-final message, i.e. during Idle we got a non-final
    //      frame, and we need to now wait for the remaining fragments.
    //
    //  WaitClientEnd
    //      Either we or a derivative has asked to shutdown. So we will have sent our close
    //      and need to wait for the client to respond with his own close before we exit.
    //      We will only wait so long.
    //
    //  End
    //      The sessions has been ended. This will cause the thread's client servicing loop
    //      to end.
    // --------------------------------------------------------------------
    enum class EStates
    {
        WaitStart
        , Connecting
        , Idle
        , InMsg
        , WaitClientEnd
        , End

        , Count
    };
}


// ---------------------------------------------------------------------------
//  Pubilc facilty constants
// ---------------------------------------------------------------------------
namespace kCIDWebSock
{
    // -----------------------------------------------------------------------
    //  On the server side, these are the msg type codes
    // -----------------------------------------------------------------------
    const tCIDLib::TCard1       c1WSockMsg_Cont     = 0;
    const tCIDLib::TCard1       c1WSockMsg_Text     = 1;
    const tCIDLib::TCard1       c1WSockMsg_Bin      = 2;
    const tCIDLib::TCard1       c1WSockMsg_Close    = 8;
    const tCIDLib::TCard1       c1WSockMsg_Ping     = 9;
    const tCIDLib::TCard1       c1WSockMsg_Pong     = 0xA;


    // -----------------------------------------------------------------------
    //  Websocket errors we return. First are standard ones, and then some that we
    //  return for more specific errors, then a value at which derived classes can
    //  start their own.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard2       c2WSockErr_Normal       = 1000;
    const tCIDLib::TCard2       c2WSockErr_Exiting      = 1001;
    const tCIDLib::TCard2       c2WSockErr_Proto        = 1002;
    const tCIDLib::TCard2       c2WSockErr_BadData      = 1003;
    const tCIDLib::TCard2       c2WSockErr_TooBig       = 1009;
    const tCIDLib::TCard2       c2WSockErr_CantDo       = 1011;

    const tCIDLib::TCard2       c2WSockErr_BadFragMsg   = 4001;
    const tCIDLib::TCard2       c2WSockErr_UnstartedCont= 4002;
    const tCIDLib::TCard2       c2WSockErr_NonFinalCtrl = 4003;
    const tCIDLib::TCard2       c2WSockErr_HandlerErr   = 4004;
    const tCIDLib::TCard2       c2WSockErr_BadFragSz    = 4005;
    const tCIDLib::TCard2       c2WSockErr_ResrvBits    = 4006;
    const tCIDLib::TCard2       c2WSockErr_FragTooBig   = 4007;
    const tCIDLib::TCard2       c2WSockErr_Timeout      = 4008;
    const tCIDLib::TCard2       c2WSockErr_Nesting      = 4009;

    const tCIDLib::TCard2       c2WSockErr_CustBase     = 5000;


    // -----------------------------------------------------------------------
    //  The maximum size we'll send or accept in a single fragment
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       c4MaxWebsockFragSz  = 0x20000;


    // -----------------------------------------------------------------------
    //  The maximum total msg size we'll accept
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4       c4MaxWebsockMsgSz   = 0x800000;
}



// ---------------------------------------------------------------------------
//  Include the fundamental headers first
// ---------------------------------------------------------------------------
#include    "CIDWebSock_ErrorIds.hpp"
#include    "CIDWebSock_Shared.hpp"
#include    "CIDWebSock_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the lazy evalution method for the facility object.
// ---------------------------------------------------------------------------
extern CIDWEBSOCKEXP TFacCIDWebSock& facCIDWebSock();


// ---------------------------------------------------------------------------
//  And subinclude our other headers
// ---------------------------------------------------------------------------
#include    "CIDWebSock_WSEngine.hpp"
