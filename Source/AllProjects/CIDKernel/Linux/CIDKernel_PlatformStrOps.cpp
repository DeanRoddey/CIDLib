//
// FILE NAME: CIDKernel_PlatformStrOps.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 08/31/2019
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
//  Most of this stuff is inlined and just directly included into the raw string
//  public class. But a few things we need to outboard.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"
#include    "CIDKernel_PlatformStrOps.hpp"

#define __STDC_LIB_EXT1__ 1
#include    <stdlib.h>
#include    <wchar.h>


tCIDLib::TBoolean
CIDStrOp_MBToWC(        tCIDLib::TCh* const     pszTarget
                , const tCIDLib::TCard4         c4MaxOutChars
                , const tCIDLib::TSCh* const    pschSrc
                ,       tCIDLib::TCard4&        c4OutChars)
{
    mbstate_t state;
    ::memset(&state, 0, sizeof state);
    const int iRes = ::mbsrtowcs
    (
        pszTarget, const_cast<const tCIDLib::TSCh**>(&pschSrc), c4MaxOutChars, &state
    );
    if (iRes >= 0)
    {
        c4OutChars = tCIDLib::TCard4(iRes);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}                


tCIDLib::TBoolean
CIDStrOp_WCToMB(        tCIDLib::TSCh* const    pschTarget
                , const tCIDLib::TCard4         c4MaxOutBytes            
                , const tCIDLib::TCh* const     pszSrc
                ,       tCIDLib::TCard4&        c4OutBytes)
{
    mbstate_t state;
    ::memset(&state, 0, sizeof state);
    const int iRes = ::wcsrtombs
    (
        pschTarget
        , const_cast<const tCIDLib::TCh**>(&pszSrc)
        , c4MaxOutBytes
        , &state
    );
    if (iRes >= 0)
    {
        c4OutBytes = tCIDLib::TCard4(iRes);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}                

tCIDLib::TBoolean
CIDStrOp_CalcMBSize(const tCIDLib::TCh* const pszSrc, tCIDLib::TCard4& c4OutBytes)
{
    mbstate_t state;
    ::memset(&state, 0, sizeof state);
    const int iRes = ::wcsrtombs(0, const_cast<const tCIDLib::TCh**>(&pszSrc), 0, &state);
    if (iRes >= 0)
    {
        c4OutBytes = tCIDLib::TCard4(iRes);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
CIDStrOp_CalcWCSize(const tCIDLib::TSCh* const pszSrc, tCIDLib::TCard4& c4OutBytes)
{
    mbstate_t state;
    ::memset(&state, 0, sizeof state);
    const int iRes = ::mbsrtowcs(0, const_cast<const tCIDLib::TSCh**>(&pszSrc), 0, &state);
    if (iRes >= 0)
    {
        c4OutBytes = tCIDLib::TCard4(iRes);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}

