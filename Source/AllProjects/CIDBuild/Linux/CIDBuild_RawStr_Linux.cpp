//
// FILE NAME: CIDBuild_RawStr_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 02/23/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
#include    <strstream>


tCIDBuild::TBoolean TRawStr::bIsSpace(const tCIDBuild::TCh chToCheck)
{
    return (iswspace(chToCheck) != 0);
}


tCIDBuild::TBoolean TRawStr::bIsDigit(const tCIDBuild::TCh chToCheck)
{
    return (iswdigit(chToCheck) != 0);
}


tCIDBuild::TBoolean
TRawStr::bXlatUInt(const   tCIDBuild::TCh* const        pszValue
                   ,       tCIDBuild::TUInt&            uiToFill
                   , const tCIDBuild::TUInt             uiRadix)
{
    // Attempt the conversion
    tCIDBuild::TCh* pszEnd;
    tCIDBuild::TUInt uiRet = ::wcstoul(pszValue, &pszEnd, uiRadix);

    // See if it failed
    if (*pszEnd)
        return false;

    // It went ok, so fill in the param and return true
    uiToFill = uiRet;

    return true;
}

tCIDBuild::TVoid
TRawStr::FormatVal( const   tCIDBuild::TUInt            uiToFormat
                    ,       tCIDBuild::TCh* const       pszToFill
                    , const tCIDBuild::TUInt            uiRadix)
{
    if (uiRadix != 8 && uiRadix != 10 && uiRadix != 16)
    {
        stdOut << NStr("Invalid radix") << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Internal;
    }

    std::ostrstream Strm;

    if (uiRadix == 8)
        Strm.setf(std::ios_base::oct);
    else if (uiRadix == 10)
        Strm.setf(std::ios_base::dec);
    else if (uiRadix == 16)
        Strm.setf(std::ios_base::hex);

    Strm << uiToFormat << std::ends;

    ::mbstowcs(pszToFill, Strm.str(), Strm.tellp());
}

tCIDBuild::TInt
TRawStr::iCompIStr( const   tCIDBuild::TCh* const   pszOne
                    , const tCIDBuild::TCh* const   pszTwo)
{
    tCIDBuild::TCh* pszOneCopy = TRawStr::pszDupString(pszOne);
    TRawStr::UpperCase(pszOneCopy);
    tCIDBuild::TCh* pszTwoCopy = TRawStr::pszDupString(pszTwo);
    TRawStr::UpperCase(pszTwoCopy);

    tCIDBuild::TInt iResult = TRawStr::iCompStr(pszOneCopy, pszTwoCopy);

    delete [] pszOneCopy;
    delete [] pszTwoCopy;

    return iResult;
}

tCIDBuild::TInt
TRawStr::iCompIStrN(const   tCIDBuild::TCh* const       pszOne
                    , const tCIDBuild::TCh* const       pszTwo
                    , const tCIDBuild::TUInt            uiCount)
{
    tCIDBuild::TCh* pszOneCopy = TRawStr::pszDupString(pszOne);
    TRawStr::UpperCase(pszOneCopy);
    tCIDBuild::TCh* pszTwoCopy = TRawStr::pszDupString(pszTwo);
    TRawStr::UpperCase(pszTwoCopy);

    tCIDBuild::TInt iResult = TRawStr::iCompStrN(pszOneCopy, pszTwoCopy, uiCount);

    delete [] pszOneCopy;
    delete [] pszTwoCopy;

    return iResult;
}

tCIDBuild::TCh*
TRawStr::pszStrTok(         tCIDBuild::TCh* const pszToTok
                    , const tCIDBuild::TCh* const pszSeps)
{
    static tCIDBuild::TCh* pszSave;
    return ::wcstok(pszToTok, pszSeps, &pszSave);
}

tCIDBuild::TCh*
TRawStr::pszTranscode(  const   tCIDBuild::TSCh* const  pszToTranscode
                        ,       tCIDBuild::TUInt&       uiTranscoded
                        , const tCIDBuild::TUInt        uiInChars)
{
    const tCIDBuild::TUInt uiLen = uiInChars ? uiInChars : ::strlen(pszToTranscode) + 1;
    tCIDBuild::TCh* pszRet = new tCIDBuild::TCh[uiLen];
    uiTranscoded = ::mbstowcs(pszRet, pszToTranscode, uiLen);
    return pszRet;
}

char* TRawStr::pszTranscode(const   tCIDBuild::TCh* const       pszToTranscode
                            ,       tCIDBuild::TUInt&           uiTranscoded
                            , const tCIDBuild::TUInt            uiInChars)
{
    const tCIDBuild::TUInt uiLen = uiInChars ? uiInChars : ::wcslen(pszToTranscode) + 1;
    char* pszRet = new tCIDBuild::TSCh[(uiLen * 2)];
    uiTranscoded = ::wcstombs(pszRet, pszToTranscode, (uiLen * 2));
    return pszRet;
}

tCIDBuild::TVoid TRawStr::UpperCase(tCIDBuild::TCh* const pszToUpper)
{
    tCIDBuild::TCh* pszSource = pszToUpper;
    while (*pszSource)
    {
        *pszSource = ::towctrans(*pszSource, reinterpret_cast<wctrans_t>(__ctype_toupper));
        ++pszSource;
    }
}
