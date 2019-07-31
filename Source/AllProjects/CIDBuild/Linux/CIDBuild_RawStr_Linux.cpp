//
// FILE NAME: CIDBuild_RawStr_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 02/23/1999
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
//  This file provides the Linux specific parts of the TRawString namespace.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "../CIDBuild.hpp"
#include    <iostream>
#include    <string.h>


tCIDLib::TBoolean TRawStr::bIsAlpha(const tCIDLib::TCh chToCheck)
{
    return (iswalpha(chToCheck) != 0);
}


tCIDLib::TBoolean TRawStr::bIsAlphaNum(const tCIDLib::TCh chToCheck)
{
    return (iswalnum(chToCheck) != 0);
}


tCIDLib::TBoolean TRawStr::bIsSpace(const tCIDLib::TCh chToCheck)
{
    return (iswspace(chToCheck) != 0);
}


tCIDLib::TBoolean TRawStr::bIsDigit(const tCIDLib::TCh chToCheck)
{
    return (iswdigit(chToCheck) != 0);
}


tCIDLib::TBoolean
TRawStr::bXlatCard4(const  tCIDLib::TCh* const  pszValue
                   ,       tCIDLib::TCard4&     c4ToFill
                   , const tCIDLib::TCard4      c4Radix)
{
    // Attempt the conversion
    tCIDLib::TCh* pszEnd;
    tCIDLib::TCard4 c4Ret = ::wcstoul(pszValue, &pszEnd, c4Radix);

    // See if it failed
    if (*pszEnd)
        return false;

    // It went ok, so fill in the param and return true
    c4ToFill = c4Ret;

    return true;
}


tCIDLib::TBoolean
TRawStr::bXlatInt4( const   tCIDLib::TCh* const pszValue
                    ,       tCIDLib::TInt4&     i4ToFill)
{
    // Attempt the conversion
    tCIDLib::TCh* pszEnd;
    tCIDLib::TInt4 i4Ret = wcstoul(pszValue, &pszEnd, 10);

    // See if it failed
    if (*pszEnd)
        return kCIDLib::False;

    // It went ok, so fill in the param and return kCIDLib::True
    i4ToFill = i4Ret;
    return kCIDLib::True;
}


tCIDLib::TCh TRawStr::chToUpper(const tCIDLib::TCh chSrc)
{
    return towupper(chSrc);
}


tCIDLib::TCh*
TRawStr::pszStrTok(         tCIDLib::TCh* const pszToTok
                    , const tCIDLib::TCh* const pszSeps)
{
    static tCIDLib::TCh* pszSave;
    return ::wcstok(pszToTok, pszSeps, &pszSave);
}

tCIDLib::TCh*
TRawStr::pszTranscode(  const   tCIDLib::TSCh* const    pszToTranscode
                        ,       tCIDLib::TCard4&        c4Transcoded
                        , const tCIDLib::TCard4         c4InChars)
{
    const tCIDLib::TUInt c4Len = c4InChars ? c4InChars : ::strlen(pszToTranscode) + 1;
    tCIDLib::TCh* pszRet = new tCIDLib::TCh[c4Len];
    c4Transcoded = ::mbstowcs(pszRet, pszToTranscode, c4Len);
    return pszRet;
}

tCIDLib::TSCh* TRawStr::pszTranscode(const  tCIDLib::TCh* const pszToTranscode
                                    ,       tCIDLib::TCard4&    c4Transcoded
                                    , const tCIDLib::TCard4     c4InChars)
{
    const tCIDLib::TUInt c4Len = c4InChars ? c4InChars : ::wcslen(pszToTranscode) + 1;
    char* pszRet = new tCIDLib::TSCh[c4Len * 2];
    c4Transcoded = ::wcstombs(pszRet, pszToTranscode, (c4Len * 2));
    return pszRet;
}

tCIDLib::TVoid TRawStr::UpperCase(tCIDLib::TCh* const pszToUpper)
{
    tCIDLib::TCh* pszSource = pszToUpper;
    while (*pszSource)
    {
        *pszSource = towupper(*pszSource);
        ++pszSource;
    }
}
