//
// FILE NAME: CIDBuild_RawStr.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/1998
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
//  This module implements the platform specific, out of line methods of the
//  TRawStr namespace.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDBuild.hpp"



// ---------------------------------------------------------------------------
//  TRawStr:
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TRawStr::bXlatBoolean(const TBldStr& strValue, tCIDLib::TBoolean& bToFill)
{
    return bXlatBoolean(strValue.pszBuffer(), bToFill);
}

tCIDLib::TBoolean
TRawStr::bXlatBoolean(  const   tCIDLib::TCh* const   pszValue
                        ,       tCIDLib::TBoolean&    bToFill)
{
    if (!iCompIStr(pszValue, L"Yes"))
        bToFill = kCIDLib::True;
    else if (!iCompIStr(pszValue, L"True"))
        bToFill = kCIDLib::True;
    else if (!iCompIStr(pszValue, L"No"))
        bToFill = kCIDLib::False;
    else if (!iCompIStr(pszValue, L"False"))
        bToFill = kCIDLib::False;
    else
        return kCIDLib::False;
    return kCIDLib::True;
}


tCIDLib::TVoid TRawStr::FormatVal(  const   tCIDLib::TCard4 c4ToFormat
                                    ,       TBldStr&        strToFill
                                    , const tCIDLib::TCard4 c4Radix)
{
    tCIDLib::TCh szTmp[128];
    FormatVal(c4ToFormat, szTmp, c4Radix);
    strToFill = szTmp;
}

