//
// FILE NAME: CIDKernel_PlatformStrOps.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/08/2019
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
//  This file provides the Win32 specific implementation of some of the more
//  complex string processing wrappers.
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


tCIDLib::TBoolean
CIDStrOp_MBToWC(		tCIDLib::TCh* const     pszTarget
				, const tCIDLib::TCard4         c4MaxOutChars
				, const tCIDLib::TSCh* const    pschSrc
				,		tCIDLib::TCard4&        c4OutChars)
{
	size_t outCnt = size_t(-1);
	const int iRes = ::mbstowcs_s(&outCnt, pszTarget, c4MaxOutChars, pschSrc, _TRUNCATE);
	if (iRes && (iRes != STRUNCATE))
	{
        *pszTarget = kCIDLib::chNull;
		return kCIDLib::False;
	}


    c4OutChars = outCnt;
    return kCIDLib::True;
}


tCIDLib::TBoolean
CIDStrOp_WCToMB(        tCIDLib::TSCh* const    pschTarget
                , const tCIDLib::TCard4         c4MaxOutBytes
                , const tCIDLib::TCh* const     pszSrc
                ,       tCIDLib::TCard4&        c4OutBytes)
{
    size_t outCnt = size_t(-1);
	const int iRes = ::wcstombs_s(&outCnt, pschTarget, c4MaxOutBytes, pszSrc, _TRUNCATE);
	if (iRes && (iRes != STRUNCATE))
        return kCIDLib::False;

    c4OutBytes = outCnt;
    return kCIDLib::True;
}

tCIDLib::TBoolean
CIDStrOp_CalcMBSize(const tCIDLib::TCh* const pszSrc, tCIDLib::TCard4& c4OutBytes)
{
    size_t outCnt = size_t(-1);
	const int iRes = ::wcstombs_s(&outCnt, 0, 0, pszSrc, _TRUNCATE);
	if (iRes && (iRes != STRUNCATE))
        return kCIDLib::False;

    c4OutBytes = outCnt;
    return kCIDLib::True;
}


tCIDLib::TBoolean
CIDStrOp_CalcWCSize(const tCIDLib::TSCh* const pszSrc, tCIDLib::TCard4& c4OutBytes)
{
    size_t outCnt = size_t(-1);
	const int iRes = ::mbstowcs_s(&outCnt, 0, 0, pszSrc, _TRUNCATE);
	if (iRes && (iRes != STRUNCATE))
		return kCIDLib::False;

    c4OutBytes = outCnt;
    return kCIDLib::True;
}

