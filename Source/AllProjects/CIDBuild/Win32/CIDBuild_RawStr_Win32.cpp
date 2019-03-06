//
// FILE NAME: CIDBuild_RawString.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/29/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides the Win32 specific parts of the TRawString namespace.
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
#include    "../CIDBuild.hpp"
#include    <windows.h>

// Supress a deprecation warning on wcstok, which is not a problem in this app
#pragma warning(disable : 4346 4996)


tCIDLib::TBoolean TRawStr::bIsAlpha(const tCIDLib::TCh chToCheck)
{
    if (::iswalpha(chToCheck))
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean TRawStr::bIsAlphaNum(const tCIDLib::TCh chToCheck)
{
    if (::iswalpha(chToCheck) || iswdigit(chToCheck))
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean TRawStr::bIsSpace(const tCIDLib::TCh chToCheck)
{
    if (::iswspace(chToCheck))
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean TRawStr::bIsDigit(const tCIDLib::TCh chToCheck)
{
    if (::iswdigit(chToCheck))
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean
TRawStr::bXlatCard4(const   tCIDLib::TCh* const pszValue
                    ,       tCIDLib::TCard4&    c4ToFill
                    , const tCIDLib::TCard4     c4Radix)
{
    // Attempt the conversion
    tCIDLib::TCh* pszEnd;
    tCIDLib::TCard4 c4Ret = wcstoul(pszValue, &pszEnd, c4Radix);

    // See if it failed
    if (*pszEnd)
        return kCIDLib::False;

    // It went ok, so fill in the param and return kCIDLib::True
    c4ToFill = c4Ret;
    return kCIDLib::True;
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


tCIDLib::TVoid
TRawStr::FormatVal( const   tCIDLib::TCard4     c4ToFormat
                    ,       tCIDLib::TCh* const pszToFill
                    , const tCIDLib::TCard4     c4MaxChars
                    , const tCIDLib::TCard4     c4Radix)
{
    _ultow_s(c4ToFormat, pszToFill, c4MaxChars, c4Radix);
}


tCIDLib::TVoid
TRawStr::FormatVal( const   tCIDLib::TInt4      i4ToFormat
                    ,       tCIDLib::TCh* const pszToFill
                    , const tCIDLib::TCard4     c4MaxChars)
{
    _ltow_s(i4ToFormat, pszToFill, c4MaxChars, 10);
}

tCIDLib::TSInt
TRawStr::iCompIStr( const   tCIDLib::TCh* const pszOne
                    , const tCIDLib::TCh* const pszTwo)
{
    return _wcsicoll(pszOne, pszTwo);
}

tCIDLib::TSInt
TRawStr::iCompIStrN(const   tCIDLib::TCh* const pszOne
                    , const tCIDLib::TCh* const pszTwo
                    , const tCIDLib::TCard4     c4Count)
{
    return _wcsnicoll(pszOne, pszTwo, c4Count);
}

tCIDLib::TCh*
TRawStr::pszStrTok(         tCIDLib::TCh* const pszToTok
                    , const tCIDLib::TCh* const pszSeps)
{
    return wcstok(pszToTok, pszSeps);
}

tCIDLib::TCh*
TRawStr::pszTranscode(  const   tCIDLib::TSCh* const    pszToTranscode
                        ,       tCIDLib::TCard4&        c4Transcoded
                        , const tCIDLib::TCard4         c4InChars)
{
    const tCIDLib::TCard4 c4Len = c4InChars ? c4InChars : _mbstrlen(pszToTranscode) + 1;
    tCIDLib::TCh* pszRet = new tCIDLib::TCh[c4Len];
    c4Transcoded = mbstowcs(pszRet, pszToTranscode, c4Len);
    return pszRet;
}

char* TRawStr::pszTranscode(const   tCIDLib::TCh* const pszToTranscode
                            ,       tCIDLib::TCard4&    c4Transcoded
                            , const tCIDLib::TCard4     c4InChars)
{
    const tCIDLib::TCard4 c4Len = c4InChars ? c4InChars : wcslen(pszToTranscode) + 1;
    char* pszRet = new char[(c4Len * 2)];
    c4Transcoded = wcstombs(pszRet, pszToTranscode, (c4Len * 2));
    return pszRet;
}


tCIDLib::TVoid TRawStr::UpperCase(tCIDLib::TCh* const pszToUpper)
{
    wcsupr(pszToUpper);
}
