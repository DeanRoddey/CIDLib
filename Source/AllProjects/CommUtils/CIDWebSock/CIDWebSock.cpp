//
// FILE NAME: CIDWebSock.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/24/2017
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
#define CIDWEBSOCK_PREINST  1


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDWebSock_.hpp"


// ---------------------------------------------------------------------------
//  Pre generate some template types
// ---------------------------------------------------------------------------
// template class CIDWEBSOCKEXP TRefQueue<TEmailMsg>;




// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDWebSock,TFacility)


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDWebSock& facCIDWebSock()
{
    static TFacCIDWebSock* pfacCIDWebSock = 0;
    if (!pfacCIDWebSock)
    {
        TBaseLock lockInit;
        if (!pfacCIDWebSock)
            TRawMem::pExchangePtr(pfacCIDWebSock, new TFacCIDWebSock);
    }
    return *pfacCIDWebSock;
}

