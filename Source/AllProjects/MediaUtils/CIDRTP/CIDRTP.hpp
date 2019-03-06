//
// FILE NAME: CIDRTP.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/20/2014
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main public header for the facility. It is the single point
//  of contact for the outside world. By including it, they get what we
//  have to offer.
//
//  An RTP session is represented by a TCIDRTPRec or TCIDRTPSend object, which is
//  given ports for RTP/RTCP, and which is given either a data source or a data
//  target, depending on sender or receiver. It will spawn off three threads.
//
// CAVEATS/GOTCHAS:
//
//  1)  Our implementation of RTP is purely oriented towards the AV profile of the
//      RTP standards, and even there only with a small number of data formats,
//      those useful for voice, and currently only supports payload type 0 of the
//      AV profile, which is the only required one.
//
//      We also currently do not deal with more than one outgoing source identifier.
//      I.e. we only support a single stream of data, no mixing is supported.
//
//  2)  RTP does not support media format negotiation. That has to be done outside
//      of this system. So we assume that the data source/target we are given is
//      capable of consuming/providing the media data received/required, or can
//      convert as required. Given #1, we assume that the client code has negotiated
//      a type 0 AVP format.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Set up our import/export attributes
// ---------------------------------------------------------------------------
#if     defined(PROJ_CIDRTP)
#define CIDRTPEXP  CID_DLLEXPORT
#else
#define CIDRTPEXP  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers
// ---------------------------------------------------------------------------
#include    "CIDMath.hpp"
#include    "CIDLib.hpp"
#include    "CIDSock.hpp"



// ---------------------------------------------------------------------------
//  Facility constants namespace
// ---------------------------------------------------------------------------
namespace kCIDRTP
{
    // -----------------------------------------------------------------------
    //  Some protocol related constants
    // -----------------------------------------------------------------------
    const   tCIDLib::TCard4 c4FixedHdrSz    = 12;
    const   tCIDLib::TCard4 c4MaxHdrSz      = 12 + (16 * 32);


    // -----------------------------------------------------------------------
    //  The max RTP sessions that our RTSP server will support, at least per
    //  instance. You could always create more than one instance.
    //
    //  If debug mode, we only create one, so that we don't have to deal with
    //  multiple client service threads popping off on the breakpoints.
    // -----------------------------------------------------------------------
    #if CID_DEBUG_ON
    const   tCIDLib::TCard4 c4MaxSessions   = 1;
    #else
    const   tCIDLib::TCard4 c4MaxSessions   = 8;
    #endif


    // -----------------------------------------------------------------------
    //  The default iniital media buffer size, which should be big enough usually,
    //  for the common 20ms of our currently supported type 0 AVP payload plus the
    //  maximum header size (full complement of CSrcs.)
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4   c4DefMediaBufSz(kCIDRTP::c4MaxHdrSz + (160 * 2));
    const tCIDLib::TCard4   c4DefCtrlBufSz(128);


    // -----------------------------------------------------------------------
    //  A default initial port (of a range of c4MaxSessions * 2 that we will use.)
    //  This is just one that is likely to be available if it's not set by the
    //  client code to something else. It has to be an event number!
    // -----------------------------------------------------------------------
    const tCIDLib::TIPPortNum   ippnFirstRTP = 27302;


    // -----------------------------------------------------------------------
    //  The default RTSP port, though it will generally be already taken by the
    //  OS, and will have to be set explicitly by client code.
    // -----------------------------------------------------------------------
    const tCIDLib::TIPPortNum   ippnRTSPSrv = 554;

}


// ---------------------------------------------------------------------------
//  Subinclude our other headers, in the needed order.
// ---------------------------------------------------------------------------
#include    "CIDRTP_ErrorIds.hpp"
#include    "CIDRTP_Type.hpp"
#include    "CIDRTP_Bufs.hpp"
#include    "CIDRTP_SrcTar.hpp"
#include    "CIDRTP_WAVIO.hpp"
#include    "CIDRTP_Receiver.hpp"
#include    "CIDRTP_Transmitter.hpp"
#include    "CIDRTP_RTSP.hpp"
#include    "CIDRTP_ThisFacility.hpp"



// ---------------------------------------------------------------------------
//  Export the facility object lazy evaluator function. This guy is in the
//  main module, as usual.
// ---------------------------------------------------------------------------
extern CIDRTPEXP TFacCIDRTP& facCIDRTP();


