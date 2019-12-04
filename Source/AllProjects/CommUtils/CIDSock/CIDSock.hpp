//
// FILE NAME: CIDSock.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/06/1998
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
//  This is the main public header for the facility. It is the single point
//  of contact for the outside world. By including it, they get what we
//  have to offer. It also insures consistent order of inclusion which
//  is needed for the precompiled headers stuff to work.
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
#if     defined(PROJ_CIDSOCK)
#define CIDSOCKEXP  CID_DLLEXPORT
#else
#define CIDSOCKEXP  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Include our underlying headers if not already included
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"


// ---------------------------------------------------------------------------
//  Include the fundamental headers first
// ---------------------------------------------------------------------------
#include    "CIDSock_ErrorIds.hpp"
#include    "CIDSock_Shared.hpp"


// ---------------------------------------------------------------------------
//  Facility non-class constants
// ---------------------------------------------------------------------------
namespace kCIDSock
{
    // -----------------------------------------------------------------------
    //  Statistics cache paths for any stats we maintain at this level
    // -----------------------------------------------------------------------
    const tCIDLib::TCh* const   pszStat_Scope_Net       = L"/Stats/Net/";
    const tCIDLib::TCh* const   pszStat_Net_OpenSockCnt = L"/Stats/Net/OpenSockCnt";
}


// ---------------------------------------------------------------------------
//  And subinclude our other headers
// ---------------------------------------------------------------------------
#include    "CIDSock_IPAddr.hpp"
#include    "CIDSock_IPHostInfo.hpp"
#include    "CIDSock_Socket.hpp"
#include    "CIDSock_StreamSocket.hpp"
#include    "CIDSock_DatagramSocket.hpp"
#include    "CIDSock_SocketListener.hpp"
#include    "CIDSock_ListenEngine.hpp"
#include    "CIDSock_Pinger.hpp"
#include    "CIDSock_URL.hpp"

#include    "CIDSock_SockStreamImpl.hpp"
#include    "CIDSock_BinarySockStream.hpp"
#include    "CIDSock_TextSockStream.hpp"
#include    "CIDSock_StreamDataSrc.hpp"

#include    "CIDSock_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the lazy evalution method for the facility object.
// ---------------------------------------------------------------------------
extern CIDSOCKEXP TFacCIDSock& facCIDSock();


// ---------------------------------------------------------------------------
//  For those collection types that we pre-generate in our main cpp file, we want to
//  insure that they don't get generated again in users of the DLL.
// ---------------------------------------------------------------------------
#if     !defined(CIDSOCK_PREINST)
extern template class TRefQueue<TSockLEngConn>;
extern template class TRefVector<TMSockSelItem>;
extern template class TVector<TIPAddress>;
#endif


// ---------------------------------------------------------------------------
//  Some magic macros for some enums of ours that have to be defined down in the
//  kernel.
// ---------------------------------------------------------------------------
EnumBinStreamMacros(tCIDSock::EAddrTypes)
EnumBinStreamMacros(tCIDSock::ESockProtos)

StdEnumTricks(tCIDSock::ESockProtos)

BmpEnumTricks(tCIDSock::EMSelFlags)
BmpEnumTricks(tCIDSock::ESockEvs)
