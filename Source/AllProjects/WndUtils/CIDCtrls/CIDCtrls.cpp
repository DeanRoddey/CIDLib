//
// FILE NAME: CIDCtrls.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/02/1997
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
#define CIDCTRLS_PREINST  1


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDCtrls_.hpp"


// ---------------------------------------------------------------------------
//  Force in any libraries we need
// ---------------------------------------------------------------------------
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "ComCtl32.lib")
#pragma comment(lib, "Shell32.lib")



// ---------------------------------------------------------------------------
//  Pre-generate some templates
// ---------------------------------------------------------------------------
template class CIDCTRLSEXP TVector<TAttrData>;
template class CIDCTRLSEXP TVector<TDlgItem>;
template class CIDCTRLSEXP TVector<TWndState>;
template class CIDCTRLSEXP TRefVector<TWindow>;
template class CIDCTRLSEXP TRefKeyedHashSet<TWindow,TString,TStringKeyOps>;


// ---------------------------------------------------------------------------
//  The lazy eval method for our facility object
// ---------------------------------------------------------------------------
TFacCIDCtrls& facCIDCtrls()
{
    static TFacCIDCtrls* pfacCIDCtrls = nullptr;
    if (!pfacCIDCtrls)
    {
        TBaseLock lockInit;
        if (!pfacCIDCtrls)
            TRawMem::pExchangePtr(pfacCIDCtrls, new TFacCIDCtrls);
    }
    return *pfacCIDCtrls;
}


// If debugging, force some stuff so we can catch it quickly
#if CID_DEBUG_ON
static tCIDLib::TVoid DummyFunc()
{
    TListBox wndLB;
    XlatEnumToListBox(tCIDCtrls,EMouseButts,wndLB)

    TComboBox wndCB;
    XlatEnumToCombo(tCIDCtrls,EMouseButts,wndCB)
}

#endif
