//
// FILE NAME: CIDKernel_PathString_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 02/28/1999
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
//  This file provides the Linux specific implementation of the the
//  TKrnlPathStr namespace.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"



// ---------------------------------------------------------------------------
//  TKrnlPathStr methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlPathStr::bFindPart(  const   tCIDLib::TCh* const pszSrc
                                            ,       tCIDLib::TCard4&    c4Start
                                            ,       tCIDLib::TCard4&    c4End
                                            , const tCIDLib::EPathParts ePart)
{
    // This one's easy
    if (pszSrc[0] == L'\000')
        return kCIDLib::False;

    // There's no concept of node parts around here
    if (ePart == tCIDLib::EPathParts::Node)
        return kCIDLib::False;

    // First exclude special cases.
    if (ePart == tCIDLib::EPathParts::FullPath)
    {
        c4Start = 0;
        c4End = kCIDLib::c4MaxCard;
        return kCIDLib::True;
    }

    const tCIDLib::TCh* pszLastPathSep = TRawStr::pszFindLastChar(pszSrc, kCIDLib::chPathSeparator);
    const tCIDLib::TCh* pszLastExtSep = TRawStr::pszFindLastChar(pszSrc, L'.');

    // Another special case. If the path separator is last,
    // then the whole thing is a path.
    if (pszLastPathSep && pszLastPathSep[1] == L'\000')
    {
        if (ePart == tCIDLib::EPathParts::Path)
        {
            c4Start = 0;
            c4End = kCIDLib::c4MaxCard;
            return kCIDLib::True;
        }
        return kCIDLib::False;
    }

    // If there's no path separator, then there's no path
    if (ePart == tCIDLib::EPathParts::Path && !pszLastPathSep)
        return kCIDLib::False;

    // If there's no ext separator or the ext separator is first or
    // last in the source string or it comes before the last path
    // separator, then we know there's no extension.
    if ((ePart == tCIDLib::EPathParts::Extension)
    &&  (!pszLastExtSep
         || pszLastExtSep[1] == L'\000'
         || pszLastPathSep > pszLastExtSep
         || pszLastExtSep == pszSrc))
    {
        return kCIDLib::False;
    }

    // Same as above, but for the benefit of finding the other
    // parts.
    if (pszLastPathSep > pszLastExtSep || pszLastExtSep == pszSrc)
        pszLastExtSep = 0;

    // Now get the start value
    if (ePart == tCIDLib::EPathParts::Path)
    {
        c4Start = 0;
    }
    else if (ePart == tCIDLib::EPathParts::Name
         ||  ePart == tCIDLib::EPathParts::NameExt)
    {
        c4Start = pszLastPathSep ? pszLastPathSep - pszSrc + 1 : 0;
    }
    else
    {
        c4Start = pszLastExtSep - pszSrc + 1;
    }

    // Now get the end value
    if (ePart == tCIDLib::EPathParts::Extension
    ||  ePart == tCIDLib::EPathParts::NameExt)
    {
        c4End = kCIDLib::c4MaxCard;
    }
    else if (ePart & tCIDLib::EPathParts::Name)
    {
        const tCIDLib::TCh* const pszTmpName = pszSrc + c4Start;
        if (pszLastExtSep
        &&  TRawStr::eCompareStr(pszTmpName, L".") != tCIDLib::ESortComps::Equal
        &&  TRawStr::eCompareStr(pszTmpName, L"..") != tCIDLib::ESortComps::Equal)
        {
            c4End = pszLastExtSep - pszSrc - 1;
        }
        else
        {
            c4End = kCIDLib::c4MaxCard;
        }
    }
    else
    {
        c4End = pszLastPathSep - pszSrc;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlPathStr::bHasExt(const tCIDLib::TCh* const pszToCheck)
{
    tCIDLib::TCard4 c4Start, c4End;
    return bFindPart(pszToCheck, c4Start, c4End, tCIDLib::EPathParts::Extension);
}

tCIDLib::TBoolean TKrnlPathStr::bHasName(const tCIDLib::TCh* const pszToCheck)
{
    tCIDLib::TCard4 c4Start, c4End;
    return bFindPart(pszToCheck, c4Start, c4End, tCIDLib::EPathParts::Name);
}

tCIDLib::TBoolean TKrnlPathStr::bHasNode(const tCIDLib::TCh* const pszToCheck)
{
    tCIDLib::TCard4 c4Start, c4End;
    return bFindPart(pszToCheck, c4Start, c4End, tCIDLib::EPathParts::Node);
}

tCIDLib::TBoolean TKrnlPathStr::bHasPath(const tCIDLib::TCh* const pszToCheck)
{
    tCIDLib::TCard4 c4Start, c4End;
    return bFindPart(pszToCheck, c4Start, c4End, tCIDLib::EPathParts::Path);
}

tCIDLib::TBoolean
TKrnlPathStr::bIsFullyQualified(const tCIDLib::TCh* const pszToCheck)
{
    return pszToCheck[0] == kCIDLib::chPathSeparator;
}

tCIDLib::TBoolean
TKrnlPathStr::bQueryExt(const   tCIDLib::TCh* const pszSrc
                        ,       tCIDLib::TCh* const pszToFill
                        , const tCIDLib::TCard4     c4MaxChars)
{
    return bQueryPart(pszSrc, pszToFill, c4MaxChars, tCIDLib::EPathParts::Extension);
}

tCIDLib::TBoolean
TKrnlPathStr::bQueryName(   const   tCIDLib::TCh* const pszSrc
                            ,       tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars)
{
    return bQueryPart(pszSrc, pszToFill, c4MaxChars, tCIDLib::EPathParts::Name);
}

tCIDLib::TBoolean
TKrnlPathStr::bQueryNameExt(const   tCIDLib::TCh* const pszSrc
                            ,       tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars)
{
    return bQueryPart(pszSrc, pszToFill, c4MaxChars, tCIDLib::EPathParts::NameExt);
}

tCIDLib::TBoolean
TKrnlPathStr::bQueryNode(   const   tCIDLib::TCh* const pszSrc
                            ,       tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars)
{
    return bQueryPart(pszSrc, pszToFill, c4MaxChars, tCIDLib::EPathParts::Node);
}

tCIDLib::TBoolean
TKrnlPathStr::bQueryPart(const  tCIDLib::TCh* const pszSrc
                        ,       tCIDLib::TCh* const pszToFill
                        , const tCIDLib::TCard4     c4MaxChars
                        , const tCIDLib::EPathParts ePart)
{
    tCIDLib::TCard4 c4Start, c4End;

    // Find the extent of the extension, if any
    if (!bFindPart(pszSrc, c4Start, c4End, ePart))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotFound);
        return kCIDLib::False;
    }

    // Calculate the size
    const tCIDLib::TCard4 c4Size = (c4End == kCIDLib::c4MaxCard) ?
                                   TRawStr::c4StrLen(&pszSrc[c4Start])
                                   : (c4End - c4Start) + 1;

    // Make sure the buffer is large enough
    if (c4MaxChars < c4Size)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
        return kCIDLib::False;
    }

    // Copy over the characters and terminate it
    TRawMem::CopyMemBuf(pszToFill, &pszSrc[c4Start], c4Size * kCIDLib::c4CharBytes);
    pszToFill[c4Size] = kCIDLib::chNull;

    return kCIDLib::True;
}

tCIDLib::TBoolean
TKrnlPathStr::bQueryPath(   const  tCIDLib::TCh* const pszSrc
                            ,       tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars)
{
    return bQueryPart(pszSrc, pszToFill, c4MaxChars, tCIDLib::EPathParts::Path);
}


tCIDLib::TBoolean TKrnlPathStr::bRemoveLevel(tCIDLib::TCh* const pszSrc)
{
    // If its empty, then we don't remove anything
    if (!*pszSrc)
        return kCIDLib::False;

    // Find the last path separator in the string
    tCIDLib::TCh* pszTmp = TRawStr::pszFindLastChar(pszSrc, kCIDLib::chPathSeparator);

    // If there isn't any, then we empty out the whole string
    if (!pszTmp)
    {
        *pszSrc = 0;
        return kCIDLib::True;
    }

    //
    //  If this is not the last char, then we have found it, so put a null
    //  after this char and return.
    //
    if (*(pszTmp + 1))
    {
        *(pszTmp + 1) = 0;
        return kCIDLib::True;
    }

    // If its only one char long, we are done
    if (pszTmp == pszSrc)
        return kCIDLib::False;

    //
    //  We have to find another one before this. If we don't find one,
    //  then there's nothing to do.
    //
    pszTmp = TRawStr::pszFindPrevChar
    (
        pszSrc
        , kCIDLib::chPathSeparator
        , (pszTmp - pszSrc) - 1
    );
    if (!pszTmp)
        return kCIDLib::False;

    // Cap it after this location and return success
    *(pszTmp + 1) = 0;
    return kCIDLib::True;
}
