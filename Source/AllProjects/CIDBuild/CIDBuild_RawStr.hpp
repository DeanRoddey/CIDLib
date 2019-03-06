//
// FILE NAME: CIDBuild_RawString.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/03/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This header provides a simple set of Unicode string manipulation methods.
//  Since we have to deal with Unicode chars even if we are not on a native
//  Unicode platform, these help deal with that. They are very simplistic
//  since they only have to serve this build utility. A couple are required
//  to be provided by the platform, the transcoding methods.
//
// LOG:
//
//  $_CIDLib_Log_$
//


class TBldStr;

namespace TRawStr
{
    // -----------------------------------------------------------------------
    //  These must be provided per-platform. They are out of line.
    // -----------------------------------------------------------------------
    tCIDLib::TBoolean bIsAlpha(const tCIDLib::TCh chToCheck);

    tCIDLib::TBoolean bIsAlphaNum(const tCIDLib::TCh chToCheck);

    tCIDLib::TBoolean bIsSpace(const tCIDLib::TCh chToCheck);

    tCIDLib::TBoolean bIsDigit(const tCIDLib::TCh chToCheck);

    tCIDLib::TSInt iCompIStr
    (
        const   tCIDLib::TCh* const         pszOne
        , const tCIDLib::TCh* const         pszTwo
    );

    tCIDLib::TSInt iCompIStrN
    (
        const   tCIDLib::TCh* const         pszOne
        , const tCIDLib::TCh* const         pszTwo
        , const tCIDLib::TCard4             c4Count
    );

    tCIDLib::TBoolean bXlatCard4
    (
        const   tCIDLib::TCh* const         pszValue
        ,       tCIDLib::TCard4&            c4ToFill
        , const tCIDLib::TCard4             c4Radix = 10
    );

    tCIDLib::TBoolean bXlatInt4
    (
        const   tCIDLib::TCh* const         pszValue
        ,       tCIDLib::TInt4&             i4ToFill
    );

    tCIDLib::TVoid FormatVal
    (
        const   tCIDLib::TCard4             c4ToFormat
        ,       tCIDLib::TCh* const         pszToFill
        , const tCIDLib::TCard4             c4MaxCars
        , const tCIDLib::TCard4             c4Radix
    );

    tCIDLib::TVoid FormatVal
    (
        const   tCIDLib::TInt4              i4ToFormat
        ,       tCIDLib::TCh* const         pszToFill
        , const tCIDLib::TCard4             c4MaxCars
    );

    tCIDLib::TCh* pszStrTok
    (
                tCIDLib::TCh* const         pszToTok
        , const tCIDLib::TCh* const         pszSeps
    );

    tCIDLib::TCh* pszTranscode
    (
        const   tCIDLib::TSCh* const        pszToTranscode
        ,       tCIDLib::TCard4&            c4Transcoded
        , const tCIDLib::TCard4             c4InChars = 0
    );

    inline tCIDLib::TCh*
    pszTranscode(   const   char* const     pszToTranscode
                    , const tCIDLib::TCard4 c4InChars = 0)
    {
        tCIDLib::TCard4 c4Dummy;
        return pszTranscode(pszToTranscode, c4Dummy, c4InChars);
    }

    char* pszTranscode
    (
        const   tCIDLib::TCh* const         pszToTranscode
        ,       tCIDLib::TCard4&            c4Transcoded
        , const tCIDLib::TCard4             c4InChars = 0
    );

    inline char*
    pszTranscode(   const   tCIDLib::TCh* const pszToTranscode
                    , const tCIDLib::TCard4     c4InChars = 0)
    {
        tCIDLib::TCard4 ulDummy;
        return pszTranscode(pszToTranscode, ulDummy, c4InChars);
    }

    tCIDLib::TVoid UpperCase
    (
                tCIDLib::TCh* const         pszToUpper
    );


    // -----------------------------------------------------------------------
    //  These are just all done for all platforms, mostly inline
    // -----------------------------------------------------------------------
    tCIDLib::TBoolean bXlatBoolean
    (
        const   TBldStr&                    strValue
        ,       tCIDLib::TBoolean&          bToFill
    );

    tCIDLib::TBoolean bXlatBoolean
    (
        const   tCIDLib::TCh* const         pszValue
        ,       tCIDLib::TBoolean&          bToFill
    );

    inline tCIDLib::TCard4 c4StrLen(const tCIDLib::TCh* const pszSource)
    {
        if (!pszSource)
            return 0;
        tCIDLib::TCard4 c4Len = 0;
        const tCIDLib::TCh* pszSrc = pszSource;
        while (*pszSrc++)
            c4Len++;
        return c4Len;
    }

    inline tCIDLib::TVoid CopyStr(          tCIDLib::TCh* const pszTarget
                                    , const tCIDLib::TCh* const pszSource)
    {
        tCIDLib::TCh* pszTar = pszTarget;
        const tCIDLib::TCh* pszSrc = pszSource;
        while (*pszSrc)
            *pszTar++ = *pszSrc++;
        *pszTar = 0;
    }

    inline tCIDLib::TVoid CopyStrN(         tCIDLib::TCh* const pszTarget
                                    , const tCIDLib::TCh* const pszSource
                                    , const tCIDLib::TCard4     c4Count)
    {
        tCIDLib::TCh* pszTar = pszTarget;
        const tCIDLib::TCh* pszSrc = pszSource;
        tCIDLib::TCard4 c4Index = 0;
        while (*pszSrc && (c4Index < c4Count))
        {
            *pszTar++ = *pszSrc++;
            c4Index++;
        }
        *pszTar = 0;
    }

    inline tCIDLib::TVoid CatStr(       tCIDLib::TCh* const pszTarget
                                , const tCIDLib::TCh* const pszSource)
    {
        tCIDLib::TCh* pszTar = pszTarget + c4StrLen(pszTarget);
        const tCIDLib::TCh* pszSrc = pszSource;
        while (*pszSrc)
            *pszTar++ = *pszSrc++;
        *pszTar = 0;
    }

    inline tCIDLib::TSInt iCompStr( const   tCIDLib::TCh* const pszOne
                                    , const tCIDLib::TCh* const pszTwo)
    {
        const tCIDLib::TCh* psz1 = pszOne;
        const tCIDLib::TCh* psz2 = pszTwo;

        //
        //  While they are equal, keep running up the pointer. If either of
        //  them goes null within the loop, then they are equal.
        //
        while (*psz1 == *psz2)
        {
            // If 1 is zero, then two is too, so we've reached the end
            if (!*psz1)
                return 0;

            // Not there yet, so bump both pointers
            psz1++;
            psz2++;
        }

        //
        //  They are not equal. Either we hit the end of one before the other
        //  or a different character.
        //
        return (tCIDLib::TSInt(*psz1) - tCIDLib::TSInt(*psz2));
    }

    inline tCIDLib::TSInt iCompStrN(const   tCIDLib::TCh* const pszOne
                                    , const tCIDLib::TCh* const pszTwo
                                    , const tCIDLib::TCard4     c4Count)
    {
        const tCIDLib::TCh* psz1 = pszOne;
        const tCIDLib::TCh* psz2 = pszTwo;
        tCIDLib::TCard4 c4Index = 0;
        while (*psz1 && (c4Index < c4Count))
        {
            if (*psz1 != *psz2)
                return (tCIDLib::TSInt(*psz1) - tCIDLib::TSInt(*psz2));
            psz1++;
            psz2++;
            c4Index++;
        }
        return 0;
    }

    tCIDLib::TVoid FormatVal
    (
        const   tCIDLib::TCard4     c4ToFormat
        ,       TBldStr&            strToFill
        , const tCIDLib::TCard4     c4Radix
    );

    inline tCIDLib::TVoid MoveChars(        tCIDLib::TCh* const pszTarget
                                    , const tCIDLib::TCh* const pszSource
                                    , const tCIDLib::TCard4     c4Count)
    {
        tCIDLib::TCh* pszTar = pszTarget;
        const tCIDLib::TCh* pszSrc = pszSource;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            *pszTar++ = *pszSrc++;
    }

    inline tCIDLib::TCh*
    pszDupString(const  tCIDLib::TCh* const pszToDup
                , const tCIDLib::TCard4     c4ExtraSpace = 0)
    {
        tCIDLib::TCh* pszRet
                = new tCIDLib::TCh[c4StrLen(pszToDup) + c4ExtraSpace + 1];
        CopyStr(pszRet, pszToDup);
        return pszRet;
    }

    inline tCIDLib::TCh*
    pszDupString(const  tCIDLib::TCh* const pszToDup
                , const tCIDLib::TCh* const pszExtra)
    {
        tCIDLib::TCh* pszRet
            = new tCIDLib::TCh[c4StrLen(pszToDup) + c4StrLen(pszExtra) + 1];
        CopyStr(pszRet, pszToDup);
        CatStr(pszRet, pszExtra);
        return pszRet;
    }

    inline const tCIDLib::TCh*
    pszFindChar(const   tCIDLib::TCh* const pszSource
                , const tCIDLib::TCh        chToFind)
    {
        const tCIDLib::TCh* pszSrc = pszSource;
        while (*pszSrc)
        {
            if (*pszSrc == chToFind)
                return pszSrc;
            pszSrc++;
        }
        return 0;
    }

    inline tCIDLib::TCh*
    pszFindChar(        tCIDLib::TCh* const pszSource
                , const tCIDLib::TCh        chToFind)
    {
        tCIDLib::TCh* pszSrc = pszSource;
        while (*pszSrc)
        {
            if (*pszSrc == chToFind)
                return pszSrc;
            pszSrc++;
        }
        return 0;
    }

    inline const tCIDLib::TCh*
    pszFindLastChar(const   tCIDLib::TCh* const pszSource
                    , const tCIDLib::TCh        chToFind)
    {
        const tCIDLib::TCh* pszSrc = pszSource + c4StrLen(pszSource);
        while (pszSrc >= pszSource)
        {
            if (*pszSrc == chToFind)
                return pszSrc;
            pszSrc--;
        }
        return 0;
    }

    inline tCIDLib::TCh*
    pszFindLastChar(        tCIDLib::TCh* const pszSource
                    , const tCIDLib::TCh        chToFind)
    {
        tCIDLib::TCh* pszSrc = pszSource + c4StrLen(pszSource);
        while (pszSrc >= pszSource)
        {
            if (*pszSrc == chToFind)
                return pszSrc;
            pszSrc--;
        }
        return 0;
    }
}

