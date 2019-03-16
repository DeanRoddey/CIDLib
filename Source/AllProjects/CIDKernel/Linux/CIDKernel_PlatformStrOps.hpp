//
// FILE NAME: CIDKernel_PlatformStrOps.Hpp
//
// AUTHOR: Will Mason
//
// CREATED: 04/27/1999
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
//  This header allows each platform to provides its own mappings of the
//  standard CIDLib raw string methods to the correct underlying C RTL
//  methods, or to its own if it wants to handle them.
//
//  A macro is provided for each operation that TRawStr needs the platform to
//  provide. Each platform just defines them to map to what it needs.
//
//  A second set are provided for a small set of short character ops that
//  we support.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

#if defined(__KCC)
#define CIDStrOp_ChToLower(c)       ::towctrans(c, static_cast<wctrans_t>(__ctype_tolower))
#else
#define CIDStrOp_ChToLower(c)       towlower(c)
#endif

#if defined(__KCC)
#define CIDStrOp_ChToUpper(c)       ::towctrans(c, static_cast<wctrans_t>(__ctype_toupper))
#else
#define CIDStrOp_ChToUpper(c)       towupper(c)
#endif

#define CIDStrOp_CnvtFloat(s,ep)    ::wcstod(s,ep)
#define CIDStrOp_FmtFloat(v,p,d,s)  ::fcvt(v,p,d,s)
#define CIDStrOp_IsAlpha(c)         ::iswalpha(c)
#define CIDStrOp_IsAlphaNum(c)      ::iswalnum(c)
#define CIDStrOp_IsDigit(c)         ::iswdigit(c)
#define CIDStrOp_IsLower(c)         ::iswlower(c)
#define CIDStrOp_IsPunct(c)         ::iswpunct(c)
#define CIDStrOp_IsSpace(c)         ::iswspace(c)
#define CIDStrOp_IsUpper(c)         ::iswupper(c)
#define CIDStrOp_IsXDigit(c)        ::iswxdigit(c)
#define CIDStrOp_MBToWC(t,s,c)      ::mbstowcs(t,s,c)

inline tCIDLib::TCh* __CIDStrOp_StrTok(tCIDLib::TCh* const          pszSource
                                       , const tCIDLib::TCh* const  pszWhiteSpace)
{
    static tCIDLib::TCh* pszContext;
    return ::wcstok(pszSource, pszWhiteSpace, &pszContext);
}
#define CIDStrOp_StrTok(s,ws)       __CIDStrOp_StrTok(s, ws)

inline tCIDLib::TVoid __CIDStrOp_StrToLower(tCIDLib::TCh* pszToLower)
{
    if (pszToLower)
    {
        while (*pszToLower)
        {
            *pszToLower = CIDStrOp_ChToLower(*pszToLower);
            ++pszToLower;
        }
    }
}
#define CIDStrOp_StrToLower(s)      __CIDStrOp_StrToLower(s)

inline tCIDLib::TVoid __CIDStrOp_StrToUpper(tCIDLib::TCh* pszToUpper)
{
    if (pszToUpper)
    {
        while (*pszToUpper)
        {
            *pszToUpper = CIDStrOp_ChToUpper(*pszToUpper);
            ++pszToUpper;
        }
    }
}
#define CIDStrOp_StrToUpper(s)      __CIDStrOp_StrToUpper(s)
#define CIDStrOp_WCToMB(t,s,c)      ::wcstombs(t,s,(c == 0 ? 1 : c))
#define CIDStrOp_CalcMBSize(us)     ::wcstombs(0,us,0)

#define CIDStrOp_Compare(s1,s2)     ::wcscmp(s1,s2)

inline tCIDLib::TSInt __CIDStrOp_CompareI(const tCIDLib::TCh* const    pszStr1
                                          , const tCIDLib::TCh* const  pszStr2)
{
    tCIDLib::TCh* pszCopy1 = TRawStr::pszReplicate(pszStr1);
    tCIDLib::TCh* pszCopy2 = TRawStr::pszReplicate(pszStr2);
    CIDStrOp_StrToLower(pszCopy1);
    CIDStrOp_StrToLower(pszCopy2);
    tCIDLib::TSInt iReturn = ::wcscmp(pszCopy1, pszCopy2);
    delete [] pszCopy1;
    delete [] pszCopy2;
    return iReturn;
}
#define CIDStrOp_CompareI(s1,s2)    __CIDStrOp_CompareI(s1, s2)
#define CIDStrOp_CompareN(s1,s2,n)  ::wcsncmp(s1,s2,n)

inline tCIDLib::TSInt __CIDStrOp_CompareNI(const tCIDLib::TCh* const    pszStr1
                                           , const tCIDLib::TCh* const  pszStr2
                                           , const tCIDLib::TCard4      c4Count)
{
    tCIDLib::TCh* pszCopy1 = TRawStr::pszReplicate(pszStr1);
    tCIDLib::TCh* pszCopy2 = TRawStr::pszReplicate(pszStr2);
    CIDStrOp_StrToLower(pszCopy1);
    CIDStrOp_StrToLower(pszCopy2);
    tCIDLib::TSInt iReturn = ::wcsncmp(pszCopy1, pszCopy2, c4Count);
    delete [] pszCopy1;
    delete [] pszCopy2;
    return iReturn;
}
#define CIDStrOp_CompareNI(s1,s2,n) __CIDStrOp_CompareNI(s1, s2, n)

#define CIDSStrOp_Compare(s1,s2)        ::strcmp(s1,s2)
#define CIDSStrOp_CompareI(s1,s2)       ::strcasecmp(s1,s2)
#define CIDSStrOp_CompareN(s1,s2,n)     ::strncmp(s1,s2,n)
#define CIDSStrOp_CompareNI(s1,s2,n)    ::strncasecmp(s1,s2,n)
