//
// FILE NAME: CIDNet.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/2000
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
#define CIDNET_PREINST  1


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDNet_.hpp"


// ---------------------------------------------------------------------------
//  Pre generate some template types
// ---------------------------------------------------------------------------
template class CIDNETEXP TRefQueue<TEmailMsg>;
template class CIDNETEXP TRefVector<TEmailAttachment>;
template class CIDNETEXP TRefVector<TJSONValue>;



// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDNet,TFacility)


// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TFacCIDNet& facCIDNet()
{
    static TFacCIDNet* pfacCIDNet = nullptr;
    if (!pfacCIDNet)
    {
        TBaseLock lockInit;
        if (!pfacCIDNet)
            pfacCIDNet = new TFacCIDNet;
    }
    return *pfacCIDNet;
}


// If debugging, force some instantiations to catch errors earlier
#if CID_DEBUG_ON
static tCIDLib::TVoid DummyFunc()
{
    TString strDummy(L"{ true; }");
    TTextStringInStream strmDummy(&strDummy);
    TJSONParser jprsTest;
    jprsTest.bParse
    (
        strmDummy
        , [](const TString& strPath
             , const TString& strValue
             , const tCIDNet::EJSONVTypes eType)
          {
            return kCIDLib::True;
          }
    );
}
#endif
