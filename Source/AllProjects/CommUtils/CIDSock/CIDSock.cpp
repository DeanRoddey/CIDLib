//
// FILE NAME: CIDSock.cpp
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
//  This is the main file of the facility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  For this file we want to pre instantiate some collections, but not for any
//  other files. Only this file defines this toke, which prevents the use of the
//  extern declaration.
// ---------------------------------------------------------------------------
#define CIDSOCK_PREINST  1


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDSock_.hpp"


// ---------------------------------------------------------------------------
//  Pre generate some template types
// ---------------------------------------------------------------------------
template class CIDSOCKEXP TRefQueue<TSockLEngConn>;
template class CIDSOCKEXP TRefVector<TMSockSelItem>;
template class CIDSOCKEXP TVector<TIPAddress>;



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDSock& facCIDSock()
{
    static TFacCIDSock facCIDSock;
    return facCIDSock;
}
