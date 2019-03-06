//
// FILE NAME: CIDKernel_PathString_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/08/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides the Win32 specific implementation of the the
//  TKrnlPathStr namespace.
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
#include    "CIDKernel_.hpp"



// ---------------------------------------------------------------------------
//  TKrnlPathStr methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlPathStr::bFindPart(  const   tCIDLib::TCh* const pszSrc
                                            ,       tCIDLib::TCard4&    c4Start
                                            ,       tCIDLib::TCard4&    c4End
                                            , const tCIDLib::EPathParts ePart)
{
    // Init the current and last indexes to the start of the string
    tCIDLib::TBoolean   bLoop   = kCIDLib::True;
    tCIDLib::TBoolean   bGotIt;
    tCIDLib::TCard4     c4Cur, c4TmpInd;
    const tCIDLib::TCh* pszTmp;

    // Init the return offsets
    c4Start = kCIDLib::c4MaxCard;
    c4End   = kCIDLib::c4MaxCard;

    // If this string is empty, then just return
    const tCIDLib::TCard4 c4Chars = TRawStr::c4StrLen(pszSrc);
    if (!c4Chars)
        return kCIDLib::False;

    //
    //  See if there is a ':' in the string. If so, must be the second char
    //  and the first char must be a valid drive
    //
    if (c4Chars >= 2)
    {
        if (pszSrc[1] == kCIDLib::chColon)
        {
            // Make sure the 0th char is a legal letter
            if (!TRawStr::bIsAlpha(pszSrc[0]))
                return kCIDLib::False;
        }
    }

    //
    //  Check for a number of special cases that start with a slash. All
    //  of these can be handled specially to make the state machine code
    //  below a lot easier.
    //
    if (pszSrc[0] == L'\\')
    {
        // If the next char is null, then its just a single slash path
        if (!pszSrc[1])
        {
            if (ePart == tCIDLib::EPathParts::Path)
            {
                c4Start = 0;
                c4End = 0;
                return kCIDLib::True;
            }
            return kCIDLib::False;
        }

        //
        //  Check for the special case of \., which is the current directory
        //  of the root.
        //
        if ((pszSrc[1] == kCIDLib::chPeriod) && (pszSrc[2] == kCIDLib::chNull))
        {
            if (ePart == tCIDLib::EPathParts::Path)
            {
                c4Start = 0;
                c4End = 0;
                return kCIDLib::True;
            }
             else if (ePart == tCIDLib::EPathParts::Name)
            {
                c4Start = 1;
                c4End = 1;
                return kCIDLib::True;
            }
            return kCIDLib::False;
        }

        //
        //  Check for the special case of \.., which is the parent directory
        //  of the root
        //
        if ((pszSrc[1] == kCIDLib::chPeriod)
        &&  (pszSrc[2] == kCIDLib::chPeriod)
        &&  (pszSrc[3] == kCIDLib::chNull))
        {
            if (ePart == tCIDLib::EPathParts::Path)
            {
                c4Start = 0;
                c4End = 2;
                return kCIDLib::True;
            }
             else if (ePart == tCIDLib::EPathParts::Name)
            {
                c4Start = 1;
                c4End = 2;
                return kCIDLib::True;
            }
            return kCIDLib::False;
        }
    }

    if (pszSrc[0] == L'\\')
    {
        //
        //  Check for \\, which is a UNC name with no node name. Or \\\,
        //  which is just a screwed up path. These are just to simplify the
        //  state machine below.
        //
        if (pszSrc[1] == L'\\')
        {
            if (pszSrc[2] == kCIDLib::chNull)
                return kCIDLib::False;

            if ((pszSrc[2] == L'\\') && !pszSrc[3])
            {
                return kCIDLib::False;
            }
        }
    }

    // And check a couple that start with a period
    if (pszSrc[0] == L'.')
    {
        if (pszSrc[1] == kCIDLib::chNull)
        {
            if (ePart == tCIDLib::EPathParts::Name)
            {
                c4Start = 0;
                c4End = 0;
                return kCIDLib::True;
            }
            return kCIDLib::False;
        }

        if ((pszSrc[1] == L'.') && (pszSrc[2] == kCIDLib::chNull))
        {
            if (ePart == tCIDLib::EPathParts::Name)
            {
                c4Start = 0;
                c4End = 1;
                return kCIDLib::True;
            }
            return kCIDLib::False;
        }
    }

    // Create a state flag for the state machine
    tCIDLib::EPathParts eState  = tCIDLib::EPathParts::Volume;

    // Loop until we are done with the string
    c4Cur = 0;
    while (bLoop)
    {
        bGotIt = kCIDLib::False;
        switch(eState)
        {
            case tCIDLib::EPathParts::Volume :
                //
                //  It either has to be a drive type volume, or a UNC type
                //  volume.
                //
                if ((c4Chars > 3) && ((pszSrc[0] == L'\\') && (pszSrc[1] == L'\\')))
                {
                    //
                    //  Now just run the current character up two more
                    //  separators. One is the node, and the other is
                    //  the share name (which is not part of the path,
                    //  it's really part of the volume name.)
                    //
                    tCIDLib::TCard4 c4Count = 0;
                    c4Cur = 2;
                    while (pszSrc[c4Cur])
                    {
                        if (pszSrc[c4Cur] == L'\\')
                        {
                            c4Count++;
                            if (c4Count == 2)
                                break;
                        }
                        c4Cur++;
                    }

                    //
                    //  If we saw at least one separator. We don't have to
                    //  see the trailing one.
                    //
                    if (c4Count)
                    {
                        c4Start = 0;
                        bGotIt = kCIDLib::True;
                    }
                }
                 else if (c4Chars > 2)
                {
                    if ((((pszSrc[0] >= L'A') && (pszSrc[0] <= L'Z'))
                    ||   ((pszSrc[0] >= L'a') && (pszSrc[0] <= L'z')))
                    &&  (pszSrc[1] == L':'))
                    {
                        c4Cur = 2;
                        bGotIt = kCIDLib::True;
                    }
                }

                if (bGotIt)
                {
                    //
                    //  If this is what they wanted, then we are done. We
                    //  leave the current index after this part, so we want
                    //  to bring it back 1 for the end marker.
                    //
                    if (ePart == tCIDLib::EPathParts::Volume)
                    {
                        c4Start = 0;
                        c4End = c4Cur - 1;
                        return kCIDLib::True;
                    }
                }
                 else
                {
                    // If this is what they wanted, then we failed
                    if (ePart == tCIDLib::EPathParts::Volume)
                        return kCIDLib::False;
                }

                // Move on to find the path part
                eState = tCIDLib::EPathParts::Path;
                break;


            case tCIDLib::EPathParts::Path :
                //
                //  Find the last path seperator in the input.
                //
                pszTmp = TRawStr::pszFindLastChar(pszSrc, L'\\');

                //
                //  If we didn't find any separators, then there cannot be a
                //  path part.
                //
                if (!pszTmp)
                {
                    // If that's what they wanted, then we failed
                    if (ePart == tCIDLib::EPathParts::Path)
                        return kCIDLib::False;

                    // Else start looking for a name part
                    eState = tCIDLib::EPathParts::Name;
                    break;
                }

                if (pszTmp)
                {
                    // Calculate where we found it
                    c4TmpInd = pszTmp - pszSrc;

                    // If it was before c4Cur, then we failed
                    if (c4TmpInd < c4Cur)
                    {
                        // If that's what they wanted, then we failed
                        if (ePart == tCIDLib::EPathParts::Path)
                            return kCIDLib::False;

                        // Else start looking for a name part
                        eState = tCIDLib::EPathParts::Name;
                        break;
                    }

                    // If that's what they wanted, then we are done
                    if (ePart == tCIDLib::EPathParts::Path)
                    {
                        c4Start = c4Cur;
                        c4End = c4TmpInd;
                        return kCIDLib::True;
                    }

                    // Update the current character
                    c4Cur = c4TmpInd + 1;
                }

                // Move on to find a name part
                eState  = tCIDLib::EPathParts::Name;
                break;


            case tCIDLib::EPathParts::Name :
                //
                //  First check for some special cases that start with a
                //  period. Initialize the temp index to avoid warnings.
                //
                c4TmpInd = kCIDLib::c4MaxCard;
                if ((pszSrc[c4Cur] == kCIDLib::chPeriod) && !pszSrc[c4Cur+1])
                {
                    // It's a . type directory
                    c4TmpInd = c4Cur + 1;
                    bGotIt = kCIDLib::True;
                }
                 else if ((pszSrc[c4Cur] == kCIDLib::chPeriod)
                      &&  (pszSrc[c4Cur + 1] == kCIDLib::chPeriod)
                      &&  !pszSrc[c4Cur + 2])
                {
                    // It's a .. type directory
                    c4TmpInd = c4Cur + 2;
                    bGotIt = kCIDLib::True;
                }
                 else
                {
                    //
                    //  Look for the last ext sep in the remaining text. If
                    //  there is not one, then everthing else is the name;
                    //  else, from c4Cur up to (but not including) the period
                    //  is the name.
                    //
                    pszTmp = TRawStr::pszFindLastChar(&pszSrc[c4Cur], kCIDLib::chPeriod);

                    // Calculate where the period was if we got one
                    if (pszTmp)
                        c4TmpInd = (pszTmp - pszSrc);

                    //
                    //  If no period, or its at or before the current pos, then
                    //  there's no extension and name goes to the end. Else
                    //  back up one to the last char before the period.
                    //
                    if ((c4TmpInd != kCIDLib::c4MaxCard) && (c4TmpInd > c4Cur))
                        c4TmpInd--;
                    else
                        c4TmpInd = kCIDLib::c4MaxCard;

                    bGotIt = kCIDLib::True;
                }

                if (bGotIt)
                {
                    // If this is what they wanted, then we are done
                    if (ePart == tCIDLib::EPathParts::Name)
                    {
                        c4Start = c4Cur;
                        c4End = c4TmpInd;
                        return kCIDLib::True;
                    }

                    //
                    //  If c4TmpInd is c4MaxCard, then the name goes to the
                    //  end of the source, so we have failed at this point.
                    //  The only thing left to find is the extension, and it
                    //  cannot be present.
                    //
                    if (c4TmpInd == kCIDLib::c4MaxCard)
                        return kCIDLib::False;

                    // Move to the next char past the name
                    c4Cur = c4TmpInd + 1;
                }
                 else
                {
                    // If this is what they wanted, then we failed
                    if (ePart == tCIDLib::EPathParts::Name)
                        return kCIDLib::False;
                }
                eState = tCIDLib::EPathParts::Extension;
                break;


            case tCIDLib::EPathParts::Extension :
                //
                //  Everything else is the extension. If the current char is
                //  the period, then skip forward over it.
                //
                if (pszSrc[c4Cur] == kCIDLib::chPeriod)
                    c4Cur++;

                // If we are at the end, then we have failed
                if (!pszSrc[c4Cur])
                    return kCIDLib::False;

                //
                //  If this is what they were looking for, then set the start
                //  to the current index and leave the end at max card.
                //
                if (ePart == tCIDLib::EPathParts::Extension)
                {
                    c4Start = c4Cur;
                    return kCIDLib::True;
                }
                bLoop = kCIDLib::False;
                break;

            default :
                #if CID_DEBUG_ON
                kmodCIDKernel.MsgPopUp
                (
                    kmodCIDKernel.pszLoadCIDFacMsg(kKrnlMsgs::midPath_UnknownPart)
                    , CID_FILE
                    , CID_LINE
                );
                #endif
                break;
        }

        // If we hit the end, then we are obviously going to fail
        if (!pszSrc[c4Cur])
            return kCIDLib::False;
    }
    return kCIDLib::False;
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

tCIDLib::TBoolean TKrnlPathStr::bHasPath(const tCIDLib::TCh* const pszToCheck)
{
    tCIDLib::TCard4 c4Start, c4End;
    return bFindPart(pszToCheck, c4Start, c4End, tCIDLib::EPathParts::Path);
}

tCIDLib::TBoolean TKrnlPathStr::bHasVolume(const tCIDLib::TCh* const pszToCheck)
{
    tCIDLib::TCard4 c4Start, c4End;
    return bFindPart(pszToCheck, c4Start, c4End, tCIDLib::EPathParts::Volume);
}


tCIDLib::TBoolean
TKrnlPathStr::bIsFullyQualified(const tCIDLib::TCh* const pszToCheck)
{
    // It has to be at least 3 characters long to be fully qualified
    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszToCheck);
    if (c4Len < 3)
        return kCIDLib::False;

    // See if it's a UNC type name, it has to start with two slashes
    if ((pszToCheck[0] == L'\\') && (pszToCheck[1] == L'\\'))
    {
        return kCIDLib::True;
    }

    // It has to start with a drive if its to be fully qualified
    if (!TRawStr::bIsAlpha(pszToCheck[0]))
        return kCIDLib::False;

    if ((pszToCheck[1] != L':') || (pszToCheck[2] != L'\\'))
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
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
    tCIDLib::TCard4 c4Start = kCIDLib::c4MaxCard;
    tCIDLib::TCard4 c4End = kCIDLib::c4MaxCard;

    //
    //  Find the extent of the name. We really just care about the start
    //  of it, since the name and extension is anything past it.
    //
    if (!bFindPart(pszSrc, c4Start, c4End, tCIDLib::EPathParts::Name))
        return kCIDLib::False;

    // Make sure the buffer is large enough
    if (c4MaxChars < TRawStr::c4StrLen(&pszSrc[c4Start]))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
        return kCIDLib::False;
    }

    // Copy over the characters
    TRawStr::CopyStr(pszToFill, &pszSrc[c4Start], c4MaxChars);
    return kCIDLib::True;

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
        return kCIDLib::False;

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
    TRawMem::CopyMemBuf(pszToFill, &pszSrc[c4Start], c4Size * kCIDLib::c4UniBytes);
    pszToFill[c4Size] = kCIDLib::chNull;

    return kCIDLib::True;
}

tCIDLib::TBoolean
TKrnlPathStr::bQueryPath(   const   tCIDLib::TCh* const pszSrc
                            ,       tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars)
{
    return bQueryPart(pszSrc, pszToFill, c4MaxChars, tCIDLib::EPathParts::Path);
}


tCIDLib::TBoolean
TKrnlPathStr::bQueryVolume( const   tCIDLib::TCh* const pszSrc
                            ,       tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars)
{
    if (!bQueryPart(pszSrc, pszToFill, c4MaxChars, tCIDLib::EPathParts::Volume))
        return kCIDLib::False;

    //
    //  These by convention must end with a slash, but the slash in the actual
    //  path is considered part of the path part, not of the volume part, so we
    //  have to add one.
    //
    TRawStr::CatStr(pszToFill, L"\\", c4MaxChars);
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlPathStr::bRemoveLevel(tCIDLib::TCh* const pszSrc)
{
    // If its empty, then we don't remove anything
    if (!*pszSrc)
        return kCIDLib::False;

    // Find the last path separator in the string
    tCIDLib::TCh* pszTmp = TRawStr::pszFindLastChar(pszSrc, L'\\');

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
    pszTmp = TRawStr::pszFindPrevChar(pszSrc, L'\\', (pszTmp - pszSrc) - 1);
    if (!pszTmp)
        return kCIDLib::False;

    // Cap it after this location and return success
    *(pszTmp + 1) = 0;
    return kCIDLib::True;
}
