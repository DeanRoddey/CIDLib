//
// FILE NAME: CIDKernel_RawString.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/10/1996
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
//  This file provides our blessed mechanisms for dealing with raw strings.
//  This is not done much at all outside of the kernel but we make them
//  available to the outside world.
//
//  There are some platform specific implementation parts that are done in
//  the CIDKernel_RawString_Win32.Cpp file in each per-platform directory.
//
// CAVEATS/GOTCHAS:
//
//  1)  Conditional compilation is used in here to use the correct versions
//      of string manipulation APIs, according to the character mode.
//
// LOG:
//
//  $_CIDLib_Log_$
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"
#include    "CIDKernel_PlatformStrOps.hpp"
#include    <math.h>


//
//  Supress a deprecation warning on wcstok, which is not a problem for us
//  because the caller verifies such things.
//
#pragma warning(disable : 4346 4996)


// ---------------------------------------------------------------------------
//  Local const data
//
//  achList
//      A list of characters used during conversion from a binary to text
//      formats.
// ---------------------------------------------------------------------------
namespace CIDKernel_RawString
{
    const tCIDLib::TCh achList[16] =
    {
          L'0'  , L'1'  , L'2'  , L'3'  , L'4'  , L'5'  , L'6'  , L'7'
        , L'8'  , L'9'  , L'A'  , L'B'  , L'C'  , L'D'  , L'E'  , L'F'
    };
}


// ---------------------------------------------------------------------------
//  A helper inline used by the hashing code
// ---------------------------------------------------------------------------
static inline tCIDLib::TCard4
c4AddToHash(const tCIDLib::TCard4 c4CurHash, const tCIDLib::TCard4 c4ToAdd)
{
    return c4CurHash + (c4CurHash * 37) + (c4CurHash >> 24) + c4ToAdd;
}


// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  Takes a raw formatted numeric value string, and group information, and
//  creates an output string with the group information applied.
//
static
tCIDLib::TBoolean bApplyGroupInfo(  const   tCIDLib::TCh* const pszInStr
                                    ,       tCIDLib::TCh* const pszOutStr
                                    , const tCIDLib::TCard4     c4MaxChars
                                    , const tCIDLib::TCard4     c4GroupSize
                                    , const tCIDLib::TCh        chGroupSep
                                    , const tCIDLib::TCh        chDecimal)
{
    tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszInStr);
    tCIDLib::TCard4 c4OutInd = 0;
    tCIDLib::TCard4 c4Index = 0;

    // The position depends on the decimal if present
    tCIDLib::TCard4 c4Pos = c4Len;
    const tCIDLib::TCh* pszDec = TRawStr::pszFindChar(pszInStr, chDecimal);
    if (pszDec)
        c4Pos = pszDec - pszInStr;

    for (; c4Index < c4Len; c4Index++, c4Pos--)
    {
        const tCIDLib::TCh chCur = pszInStr[c4Index];

        // If it's a floating point, then break out on the decimal
        if (chCur == chDecimal)
            break;

        //
        //  If this is not the left or right most digit, and the
        //  position is divisible by the group size, then we need to
        //  put in a separator.
        //
        if (c4Index && (c4Index != c4Len) && !(c4Pos % c4GroupSize))
            pszOutStr[c4OutInd++] = chGroupSep;

        if (c4OutInd == c4MaxChars)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_BufferOverflow);
            return kCIDLib::False;
        }

        pszOutStr[c4OutInd++] = chCur;

        if (c4OutInd == c4MaxChars)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_BufferOverflow);
            return kCIDLib::False;
        }
    }

    // If not at the end, we broke out on a decimal and need to do the rest
    while (c4Index < c4Len)
        pszOutStr[c4OutInd++] = pszInStr[c4Index++];

    // Cap it off
    pszOutStr[c4OutInd] = kCIDLib::chNull;
    return kCIDLib::True;
}


//
//  There are const/non-const versions of most of the search methods. These
//  are local functions which implement the actual guts of the methods. The
//  const and non-const public versions share these implementations.
//
//  Returns the find index or kCIDLib::c4MaxCard if not found
//
static tCIDLib::TCard4
c4FindChar( const   tCIDLib::TCh* const     pszToSearch
            , const tCIDLib::TCh            chToFind
            , const tCIDLib::TCard4         c4StartAt
            , const tCIDLib::TBoolean       bCase)
{
    // Assume no find
    tCIDLib::TCard4 c4Ret = kCIDLib::c4MaxCard;

    tCIDLib::TCard4 c4Ind = c4StartAt;

    if (bCase)
    {
        while (pszToSearch[c4Ind] != kCIDLib::chNull)
        {
            if (pszToSearch[c4Ind] == chToFind)
                return c4Ind;
            c4Ind++;
        }
    }
     else
    {
        const tCIDLib::TCh chActual = TRawStr::chUpper(chToFind);
        while (pszToSearch[c4Ind] != kCIDLib::chNull)
        {
            if (TRawStr::chUpper(pszToSearch[c4Ind]) == chActual)
                return c4Ind;
            c4Ind++;
        }
    }
    return c4Ret;
}


static tCIDLib::TCard4
c4FindChars(const  tCIDLib::TCh* const      pszToSearch
            , const tCIDLib::TCh* const     pszChars
            , const tCIDLib::TCard4         c4StartAt
            , const tCIDLib::TBoolean       bCase)
{
    tCIDLib::TCard4 c4Ret;

    //
    //  Loop through the search string, starting at the start index. For each
    //  character, look for it in the list of chars.
    //
    tCIDLib::TCard4     c4Index = c4StartAt;
    tCIDLib::TBoolean   bDone = kCIDLib::False;

    while (!bDone)
    {
        c4Ret = c4FindChar(pszChars, pszToSearch[c4Index], 0, bCase);
        if (c4Ret != kCIDLib::c4MaxCard)
            return c4Index;

        // Move up to the next char in the search string
        c4Index++;

        // If we are at the end of the search string, then done
        if (pszToSearch[c4Index] == kCIDLib::chNull)
            break;
    }
    return kCIDLib::c4MaxCard;
}


static tCIDLib::TCard4
c4FindPrevChar( const  tCIDLib::TCh* const      pszToSearch
                , const tCIDLib::TCh            chToFind
                , const tCIDLib::TCard4         c4StartAt
                , const tCIDLib::TBoolean       bCase)
{
    tCIDLib::TCard4 c4Index = c4StartAt;
    tCIDLib::TBoolean   bDone = kCIDLib::False;

    if (bCase)
    {
        while (!bDone)
        {
            if (pszToSearch[c4Index] == chToFind)
                return c4Index;

            if (!c4Index)
                break;

            c4Index--;
        }
    }
     else
    {
        const tCIDLib::TCh chActual = TRawStr::chUpper(chToFind);
        while (!bDone)
        {
            if (TRawStr::chUpper(pszToSearch[c4Index]) == chActual)
                return c4Index;

            if (!c4Index)
                break;

            c4Index--;
        }
    }

    // Convert the new find index
    return kCIDLib::c4MaxCard;
}


static tCIDLib::TCard4
c4FindPrevChars(const   tCIDLib::TCh* const     pszToSearch
                , const tCIDLib::TCh* const     pszChars
                , const tCIDLib::TCard4         c4StartAt
                , const tCIDLib::TBoolean       bCase)
{
    //
    //  Loop backwards through the search string, starting at the start index.
    //  For each character, look for it in the list of chars.
    //
    tCIDLib::TCard4 c4Index = c4StartAt;
    tCIDLib::TBoolean   bDone = kCIDLib::False;
    while (!bDone)
    {
        const tCIDLib::TCard4 c4Ret = c4FindChar
        (
            pszChars, pszToSearch[c4Index], 0, bCase
        );

        if (c4Ret != kCIDLib::c4MaxCard)
            return c4Index;

        if (!c4Index)
            break;

        c4Index--;
    }
    return kCIDLib::c4MaxCard;
}


//
//  Formats a 32 bit unsigned value into a textual format. We use this instead
//  of the RTL stuff in order to reduce our dependence upon the RTL and to
//  avoid issues regarding character mode.
//
static
tCIDLib::TBoolean bCard4ToText( const   tCIDLib::TCard4     c4ToConvert
                                ,       tCIDLib::TCh* const pszToFill
                                , const tCIDLib::TCard4     c4MaxChars
                                , const tCIDLib::ERadices   eRadix)
{
    // If the buffer is empty, return an error
    if (!c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_ZeroSizedBuffer);
        return kCIDLib::False;
    }

    // Handle special case
    if (!c4ToConvert)
    {
        pszToFill[0] = L'0';
        pszToFill[1] = kCIDLib::chNull;
        return kCIDLib::True;
    }

    // This is used to fill the temp buffer
    tCIDLib::TCard4 c4TmpIndex = 0;

    // A copy of the conversion value that we can modify
    tCIDLib::TCard4 c4TmpVal = c4ToConvert;

    //
    //  Convert into a temp buffer that we know is large enough. This avoids
    //  having to check for overflow in the inner loops, and we have to flip
    //  the resulting string anyway.
    //
    tCIDLib::TZStr128 szTmpBuf;

    //
    //  For each radix, do the optimal thing. For bin and hex, we can special
    //  case them and do shift and mask oriented stuff. For oct and decimal
    //  there isn't much to do but bull through it with divides.
    //
    if (eRadix == tCIDLib::ERadices::Bin)
    {
        //
        //  Put digits into the string in reverse order, until we get a zero
        //  value for the conversion value by successive shifts.
        //
        while (c4TmpVal)
        {
            if (c4TmpVal & 0x1UL)
                szTmpBuf[c4TmpIndex++] = L'1';
            else
                szTmpBuf[c4TmpIndex++] = L'0';

            c4TmpVal >>= 1;
        }
    }
     else if (eRadix == tCIDLib::ERadices::Hex)
    {
        while (c4TmpVal)
        {
            // Get the mask, which is from 0 to 0xF
            const tCIDLib::TCard4 c4CharInd = (c4TmpVal & 0xFUL);

            // And use it to index the list of characters
            szTmpBuf[c4TmpIndex++] = CIDKernel_RawString::achList[c4CharInd];

            c4TmpVal >>= 4;
        }
    }
     else if ((eRadix == tCIDLib::ERadices::Oct)
          ||  (eRadix == tCIDLib::ERadices::Dec))
    {
        while (c4TmpVal)
        {
            const tCIDLib::TCard4 c4CharInd = (c4TmpVal % tCIDLib::c4EnumOrd(eRadix));
            szTmpBuf[c4TmpIndex++] = CIDKernel_RawString::achList[c4CharInd];
            c4TmpVal /= tCIDLib::c4EnumOrd(eRadix);
        }
    }
     else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_BadRadix);
        return kCIDLib::False;
    }

    // See if have enough room in the caller's buffer
    if (c4TmpIndex > c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_BufferOverflow);
        return kCIDLib::False;
    }

    // Reverse the tmp buffer into the caller's buffer
    tCIDLib::TCard4 c4OutIndex = 0;
    for (; c4TmpIndex > 0; c4TmpIndex--)
        pszToFill[c4OutIndex++] = szTmpBuf[c4TmpIndex-1];

    // And cap off the caller's buffer
    pszToFill[c4OutIndex] = kCIDLib::chNull;

    return kCIDLib::True;
}


//
//  Formats a 64 bit unsigned value into a textual format. We use this instead
//  of the RTL stuff in order to reduce our dependence upon the RTL and to
//  avoid issues regarding character mode.
//
static
tCIDLib::TBoolean bCard8ToText( const   tCIDLib::TCard8&    c8ToConvert
                                ,       tCIDLib::TCh* const pszToFill
                                , const tCIDLib::TCard4     c4MaxChars
                                , const tCIDLib::ERadices   eRadix)
{
    // If the buffer is empty, return an error
    if (!c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_ZeroSizedBuffer);
        return kCIDLib::False;
    }

    // Handle special case
    if (!c8ToConvert)
    {
        pszToFill[0] = L'0';
        pszToFill[1] = kCIDLib::chNull;
        return kCIDLib::True;
    }

    // This is used to fill the temp buffer
    tCIDLib::TCard4 c4TmpIndex = 0;

    // A copy of the conversion value that we can modify
    tCIDLib::TCard8 c8TmpVal = c8ToConvert;

    //
    //  Convert into a temp buffer that we know is large enough. This avoids
    //  having to check for overflow in the inner loops, and we have to flip
    //  the resulting string anyway.
    //
    tCIDLib::TZStr128 szTmpBuf;

    //
    //  For each radix, do the optimal thing. For bin and hex, we can special
    //  case them and do shift and mask oriented stuff. For oct and decimal
    //  there isn't much to do but bull through it with divides.
    //
    if (eRadix == tCIDLib::ERadices::Bin)
    {
        //
        //  Put digits into the string in reverse order, until we get a zero
        //  value for the conversion value by successive shifts.
        //
        while (c8TmpVal)
        {
            if (c8TmpVal & 0x1UL)
                szTmpBuf[c4TmpIndex++] = L'1';
            else
                szTmpBuf[c4TmpIndex++] = L'0';

            c8TmpVal >>= 1;
        }
    }
     else if (eRadix == tCIDLib::ERadices::Hex)
    {
        while (c8TmpVal)
        {
            // Get the mask, which is from 0 to 0xF
            const tCIDLib::TCard4 c4CharInd = tCIDLib::TCard4(c8TmpVal & 0xFUL);

            // And use it to index the list of characters
            szTmpBuf[c4TmpIndex++] = CIDKernel_RawString::achList[c4CharInd];

            c8TmpVal >>= 4;
        }
    }
     else if ((eRadix == tCIDLib::ERadices::Oct)
          ||  (eRadix == tCIDLib::ERadices::Dec))
    {
        while (c8TmpVal)
        {
            const tCIDLib::TCard4 c4CharInd = tCIDLib::TCard4
            (
                c8TmpVal % tCIDLib::c4EnumOrd(eRadix)
            );
            szTmpBuf[c4TmpIndex++] = CIDKernel_RawString::achList[c4CharInd];
            c8TmpVal /= tCIDLib::c4EnumOrd(eRadix);
        }
    }
     else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_BadRadix);
        return kCIDLib::False;
    }

    // See if have enough room in the caller's buffer
    if (c4TmpIndex > c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_BufferOverflow);
        return kCIDLib::False;
    }

    // Reverse the tmp buffer into the caller's buffer
    tCIDLib::TCard4 c4OutIndex = 0;
    for (; c4TmpIndex > 0; c4TmpIndex--)
        pszToFill[c4OutIndex++] = szTmpBuf[c4TmpIndex-1];

    // And cap off the caller's buffer
    pszToFill[c4OutIndex] = kCIDLib::chNull;

    return kCIDLib::True;
}


//
//  Used locally to translate raw system collation result values into our
//  more restrictive ESortComps enum.
//
static inline tCIDLib::ESortComps eXlatSortRes(const tCIDLib::TSInt sRes) noexcept
{
    if (sRes < 0)
        return tCIDLib::ESortComps::FirstLess;
    else if (sRes > 0)
        return tCIDLib::ESortComps::FirstGreater;
    return tCIDLib::ESortComps::Equal;
}


//
//  This method will convert the passed string to a binary TCard4 value,
//  taking into account the radix. It will return either -1 or the offset
//  of the offending character.
//
static tCIDLib::TInt4 i4TextToCard4( const  tCIDLib::TCh* const pszSrcText
                                    , const tCIDLib::TCard4     c4Radix
                                    ,       tCIDLib::TCard4&    c4ToFill
                                    ,       tCIDLib::TBoolean&  bNegative) noexcept
{
    // A temp local value to accumulate into
    tCIDLib::TCard4 c4Accum = 0;

    // Assume its not negative
    bNegative = kCIDLib::False;

    // Get a pointer that we can run up through the string
    const tCIDLib::TCh* pszSrc = pszSrcText;

    // Skip over any whitespace, returning an error at 0 if its all space
    while (TRawStr::bIsSpace(*pszSrc))
        pszSrc++;

    // If it's empty, it cannot be valid
    if (!*pszSrc)
    {
        bNegative = kCIDLib::True;
        return -1;
    }

    // If its a sign, then eat it and set the caller's flag accordingly
    if (*pszSrc == L'-')
    {
        bNegative = kCIDLib::True;
        pszSrc++;
    }
     else if (*pszSrc == L'+')
    {
        pszSrc++;
    }

    //
    //  See if we need to figure out the radix ourself. If the passed radix
    //  is zero, we have to try to figure it out.
    //
    tCIDLib::TCard4 c4ActualRadix = c4Radix;
    if (!c4Radix)
    {
        if (*pszSrc == L'0')
        {
            // Its either hex or octal
            if (TRawStr::chUpper(*(pszSrc+1)) == L'X')
            {
                c4ActualRadix = 16;

                // Skip over the 0x
                pszSrc += 2;
            }
             else
            {
                c4ActualRadix = 8;
            }
        }
         else if (TRawStr::bIsDigit(*pszSrc))
        {
            c4ActualRadix = 10;
        }
         else
        {
            // Return an error at this index
            return tCIDLib::TInt4(pszSrc - pszSrcText);
        }
    }
     else if (c4Radix == 16)
    {
        // Skip over any leading 0x
        if ((*pszSrc == L'0') && (TRawStr::chUpper(*(pszSrc+1)) == L'X'))
            pszSrc += 2;
    }

    //
    //  This value is used below to know whether we are going to overflow,
    //  without doing any sticky calculations. Once we go over this, then
    //  we have to be careful because the next digit can push it over.
    //
    //  The other value is maximum value that the 'ones' position digit
    //  can be.
    //
    const tCIDLib::TCard4 c4OverVal = kCIDLib::c4MaxCard / c4ActualRadix;
    const tCIDLib::TCard4 c4MaxOnesDig = kCIDLib::c4MaxCard % c4ActualRadix;

    while (*pszSrc)
    {
        // Figure out the numeric value of the current digit
        tCIDLib::TCard4 c4CurDigit;
        if ((*pszSrc >= L'0') && (*pszSrc <= L'9'))
        {
            c4CurDigit = tCIDLib::TCard4(*pszSrc - L'0');
        }
         else if ((*pszSrc >= L'A') && (*pszSrc <= L'F'))
        {
            c4CurDigit = tCIDLib::TCard4(*pszSrc - L'A') + 10;
        }
         else if ((*pszSrc >= L'a') && (*pszSrc <= L'f'))
        {
            c4CurDigit = tCIDLib::TCard4(*pszSrc - L'a') + 10;
        }
         else
        {
            // Break out on invalid character
            break;
        }

        // Not a valid digit for this radix
        if (c4CurDigit >= c4ActualRadix)
            return tCIDLib::TInt4(pszSrc - pszSrcText);

        // Make sure that we are not going to overflow
        if ((c4Accum < c4OverVal)
        ||  ((c4Accum == c4OverVal) && (c4CurDigit <= c4MaxOnesDig)))
        {
            c4Accum = (c4Accum * c4ActualRadix) + c4CurDigit;
        }
         else
        {
            // Return a failure at this index
            return tCIDLib::TInt4(pszSrc - pszSrcText);
        }

        // Move to the next character
        pszSrc++;
    }

    //
    //  If we broke out on something besides the end of the string, then
    //  check that the rest of the string is all whitespace. If not, then
    //  its not valid.
    //
    if (*pszSrc)
    {
        while (*pszSrc)
        {
            if (!TRawStr::bIsSpace(*pszSrc))
                return tCIDLib::TInt4(pszSrc - pszSrcText);
            pszSrc++;
        }
    }

    // Copy to the caller's buffer and return success
    c4ToFill = c4Accum;
    return -1;
}


//
//  This method will convert the passed string to a binary TCard8 value,
//  taking into account the radix. It will return either -1 or the offset
//  of the offending character.
//
static tCIDLib::TInt4 i4TextToCard8( const  tCIDLib::TCh* const pszSrcText
                                    , const tCIDLib::TCard4     c4Radix
                                    ,       tCIDLib::TCard8&    c8ToFill
                                    ,       tCIDLib::TBoolean&  bNegative)
{
    // A temp local value to accumulate into
    tCIDLib::TCard8 c8Accum = 0;

    // Assume its not negative
    bNegative = kCIDLib::False;

    // Get a pointer that we can run up through the string
    const tCIDLib::TCh* pszSrc = pszSrcText;

    // Skip over any whitespace, returning an error at 0 if its all space
    while (TRawStr::bIsSpace(*pszSrc))
        pszSrc++;

    // If it's empty, it cannot be valid
    if (!*pszSrc)
        return -1;

    // If its a sign, then eat it and set the caller's flag accordingly
    if (*pszSrc == L'-')
    {
        bNegative = kCIDLib::True;
        pszSrc++;
    }
     else if (*pszSrc == L'+')
    {
        pszSrc++;
    }

    //
    //  See if we need to figure out the radix ourself. If the passed radix
    //  is zero, we have to try to figure it out.
    //
    tCIDLib::TCard4 c4ActualRadix = c4Radix;
    if (!c4Radix)
    {
        if (*pszSrc == L'0')
        {
            // Its either hex or octal
            if (TRawStr::chUpper(*(pszSrc+1)) == L'X')
            {
                c4ActualRadix = 16;

                // Skip over the 0x
                pszSrc += 2;
            }
             else
            {
                c4ActualRadix = 8;
            }
        }
         else if (TRawStr::bIsDigit(*pszSrc))
        {
            c4ActualRadix = 10;
        }
         else
        {
            // Return an error at this index
            return tCIDLib::TInt4(pszSrc - pszSrcText);
        }
    }
     else if (c4Radix == 16)
    {
        // Skip over any leading 0x
        if ((*pszSrc == L'0') && (TRawStr::chUpper(*(pszSrc+1)) == L'X'))
            pszSrc += 2;
    }

    //
    //  This value is used below to know whether we are going to overflow,
    //  without doing any sticky calculations. Once we go over this, then
    //  we have to be careful because the next digit can push it over.
    //
    //  The other value is maximum value that the 'ones' position digit
    //  can be.
    //
    const tCIDLib::TCard8 c8OverVal = kCIDLib::c8MaxCard / c4ActualRadix;
    const tCIDLib::TCard4 c4MaxOnesDig = tCIDLib::TCard4
    (
        kCIDLib::c8MaxCard % c4ActualRadix
    );

    while (*pszSrc)
    {
        // Figure out the numeric value of the current digit
        tCIDLib::TCard4 c4CurDigit;
        if ((*pszSrc >= L'0') && (*pszSrc <= L'9'))
        {
            c4CurDigit = tCIDLib::TCard4(*pszSrc - L'0');
        }
         else if ((*pszSrc >= L'A') && (*pszSrc <= L'F'))
        {
            c4CurDigit = tCIDLib::TCard4(*pszSrc - L'A') + 10;
        }
         else if ((*pszSrc >= L'a') && (*pszSrc <= L'f'))
        {
            c4CurDigit = tCIDLib::TCard4(*pszSrc - L'a') + 10;
        }
         else
        {
            // Break out on invalid character
            break;
        }

        // Not a valid digit for this radix
        if (c4CurDigit >= c4ActualRadix)
            return tCIDLib::TInt4(pszSrc - pszSrcText);

        // Make sure that we are not going to overflow
        if ((c8Accum < c8OverVal)
        ||  ((c8Accum == c8OverVal) && (c4CurDigit <= c4MaxOnesDig)))
        {
            c8Accum = (c8Accum * c4ActualRadix) + c4CurDigit;
        }
         else
        {
            // Return a failure at this index
            return tCIDLib::TInt4(pszSrc - pszSrcText);
        }

        // Move to the next character
        pszSrc++;
    }

    //
    //  If we broke out on something besides the end of the string, then
    //  check that the rest of the string is all whitespace. If not, then
    //  its not valid.
    //
    if (*pszSrc)
    {
        while (*pszSrc)
        {
            if (!TRawStr::bIsSpace(*pszSrc))
                return tCIDLib::TInt4(pszSrc - pszSrcText);
            pszSrc++;
        }
    }

    // Copy to the caller's buffer and return success
    c8ToFill = c8Accum;
    return -1;
}



// ---------------------------------------------------------------------------
//  TRawStr Methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TRawStr::bFormatStr(const   tCIDLib::TCh* const pszVal
                    ,       tCIDLib::TCh* const pszBuf
                    , const tCIDLib::TCard4     c4FldWidth
                    , const tCIDLib::TCh        chFill
                    , const tCIDLib::EHJustify  eJustify)
{
    //
    //  If no field width is indicated, then just copy as much of the
    //  source string as possible.
    //
    if (c4FldWidth == 0)
    {
        CopyStr(pszBuf, pszVal, c4FldWidth);
        return kCIDLib::True;
    }

    // Remember the current length of the value string and the buffer
    tCIDLib::TCard4  c4ValLen = c4StrLen(pszVal);
    tCIDLib::TCard4  c4Tmp;

    //
    //  Regardless of what route we take below, we want to fill the
    //  destination buffer with the fill character and cap if off at
    //  the field width, so go ahead and do that.
    //
    FillString(pszBuf, chFill, c4FldWidth);
    pszBuf[c4FldWidth] = 0;

    //
    //  If there is no text in the source buffer, then we have nothing to
    //  do. Otherwise, we need to do the justification thing.
    //
    if (pszVal[0])
    {
        if (eJustify == tCIDLib::EHJustify::Left)
        {
            //
            //  Copy the value into the field, minus the null terminator.
            //  If the value string is longer than the field, then clip
            //  it.
            //
            c4Tmp = (c4ValLen > c4FldWidth) ? c4FldWidth : c4ValLen;
            memcpy(pszBuf, pszVal, c4Tmp * kCIDLib::c4CharBytes);
        }
         else if (eJustify == tCIDLib::EHJustify::Right)
        {
            //
            //  Copy the value into the field so that it ends at the end
            //  of the field. If the value string is longer than the
            //  field, then we have to start copying past the 0th char.
            //  Make sure that the terminating null on the tmp buffer
            //  falls onto the null of the result buffer.
            //
            if (c4ValLen > c4FldWidth)
                CopyStr(pszBuf, &pszVal[c4ValLen - c4FldWidth], c4FldWidth);
             else
                CopyStr(&pszBuf[c4FldWidth - c4ValLen], pszVal, c4ValLen);
        }
         else if (eJustify == tCIDLib::EHJustify::Center)
        {
            //
            //  Copy the value into the field so that it is centered.
            //  If an even number of chars are not available on both
            //  sides of the centered field, then it tends toward the
            //  left since the integer divide truncates and loses the
            //  partial remainder. If the value string is longer than
            //  the field width, then it is clipped on both ends so
            //  that the center portion of it gets into the result
            //  buffer.
            //
            if (c4ValLen > c4FldWidth)
            {
                memcpy
                (
                    pszBuf
                    , &pszVal[(c4ValLen - c4FldWidth) >> 1]
                    , ((c4ValLen - c4FldWidth) >> 1) * kCIDLib::c4CharBytes
                );
            }
             else
            {
                memcpy
                (
                    &pszBuf[(c4FldWidth >> 1) - (c4ValLen >> 1)]
                    , pszVal
                    , c4StrLen(pszVal) * kCIDLib::c4CharBytes
                );
            }
        }
         else
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcFmt_InvalidJustification);
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TRawStr::bFormatVal(const   tCIDLib::TCard4     c4Val
                    ,       tCIDLib::TCh* const pszBuf
                    , const tCIDLib::TCard4     c4MaxChars
                    , const tCIDLib::ERadices   eRadix
                    , const tCIDLib::TCard4     c4GroupSize
                    , const tCIDLib::TCh        chGroupSep)
{
    // Watch for pathological case of zero sized buffer
    if (!c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_ZeroSizedBuffer);
        return kCIDLib::False;
    }

    //
    //  If the group size is non-zero, that means put in separators. So
    //  make sure the separator is not nul.
    //
    if (c4GroupSize && (chGroupSep == kCIDLib::chNull))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcFmt_InvalidGroupSep);
        return kCIDLib::False;
    }

    // Init the target string to empty just in case
    pszBuf[0] = kCIDLib::chNull;

    // Do a basic format to a local buffer
    tCIDLib::TZStr128 szTmp;
    if (!bCard4ToText(c4Val, szTmp, 128, eRadix))
        return kCIDLib::False;

    //
    //  Handle the special case of zero, which we don't have to do any
    //  thing special for. This makes the code below simpler to deal with
    //  this case first.
    //
    if (!c4Val)
    {
        CopyStr(pszBuf, szTmp, c4MaxChars);
        return kCIDLib::True;
    }

    if ((eRadix == tCIDLib::ERadices::Dec) && c4GroupSize)
    {
        if (!bApplyGroupInfo(szTmp, pszBuf, c4MaxChars, c4GroupSize, chGroupSep, 0))
            return kCIDLib::False;
    }
     else
    {
        // The octal and hex radices don't honor the group stuff
        CopyStr(pszBuf, szTmp, c4MaxChars);
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean
TRawStr::bFormatVal(const   tCIDLib::TCard8&    c8Val
                    ,       tCIDLib::TCh* const pszBuf
                    , const tCIDLib::TCard4     c4MaxChars
                    , const tCIDLib::ERadices   eRadix
                    , const tCIDLib::TCard4     c4GroupSize
                    , const tCIDLib::TCh        chGroupSep)
{
    // Watch for pathological case of zero sized buffer
    if (!c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_ZeroSizedBuffer);
        return kCIDLib::False;
    }

    //
    //  If the group size is non-zero, that means put in separators. So
    //  make sure the separator is not nul.
    //
    if (c4GroupSize && (chGroupSep == kCIDLib::chNull))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcFmt_InvalidGroupSep);
        return kCIDLib::False;
    }

    // Init the target string to empty just in case
    pszBuf[0] = kCIDLib::chNull;

    // Do a basic format to a local buffer
    tCIDLib::TZStr128 szTmp;
    if (!bCard8ToText(c8Val, szTmp, 128, eRadix))
        return kCIDLib::False;

    //
    //  Handle the special case of zero, which we don't have to do any
    //  thing special for. This makes the code below simpler to deal with
    //  this case first.
    //
    if (!c8Val)
    {
        CopyStr(pszBuf, szTmp, c4MaxChars);
        return kCIDLib::True;
    }

    if ((eRadix == tCIDLib::ERadices::Dec) && c4GroupSize)
    {
        if (!bApplyGroupInfo(szTmp, pszBuf, c4MaxChars, c4GroupSize, chGroupSep, 0))
            return kCIDLib::False;
    }
     else
    {
        // The octal and hex radices don't honor the group stuff
        CopyStr(pszBuf, szTmp, c4MaxChars);
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean
TRawStr::bFormatVal(const   tCIDLib::TInt4      i4Val
                    ,       tCIDLib::TCh* const pszBuf
                    , const tCIDLib::TCard4     c4MaxChars
                    , const tCIDLib::ERadices   eRadix
                    , const tCIDLib::TCh        chNegSign
                    , const tCIDLib::TCard4     c4GroupSize
                    , const tCIDLib::TCh        chGroupSep)
{
    // Watch for pathological case of zero sized buffer
    if (!c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_ZeroSizedBuffer);
        return kCIDLib::False;
    }

    //
    //  If the group size is non-zero, that means put in separators. So
    //  make sure the separator is not nul.
    //
    if (c4GroupSize && (chGroupSep == kCIDLib::chNull))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcFmt_InvalidGroupSep);
        return kCIDLib::False;
    }

    //
    //  We don't want to deal with any locale specific sign stuff here,
    //  since we are doing it ourself. So we get a temp copy of the value
    //  and make sure its positive.
    //
    const tCIDLib::TCard4 c4PosVal = tCIDLib::TCard4(i4Val < 0 ? -i4Val : i4Val);

    tCIDLib::TZStr128 szTmp;
    if (!bCard4ToText(c4PosVal, szTmp, 128, eRadix))
        return kCIDLib::False;

    // Handle the sign separately in order to make the below code simpler.
    tCIDLib::TCard4 c4OutInd = 0;
    if (i4Val < 0)
        pszBuf[c4OutInd++] = chNegSign;

    //
    //  If the radix is decimal and we have a group size, then apply the group
    //  info the string. Otherwise, just copy over as is.
    //
    if ((eRadix == tCIDLib::ERadices::Dec) && c4GroupSize)
    {
        if (!bApplyGroupInfo(szTmp, &pszBuf[c4OutInd], c4MaxChars, c4GroupSize, chGroupSep, 0))
            return kCIDLib::False;
    }
     else
    {
        CopyStr(&pszBuf[c4OutInd], szTmp, c4MaxChars);
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean
TRawStr::bFormatVal(const   tCIDLib::TInt8&     i8Val
                    ,       tCIDLib::TCh* const pszBuf
                    , const tCIDLib::TCard4     c4MaxChars
                    , const tCIDLib::ERadices   eRadix
                    , const tCIDLib::TCh        chNegSign
                    , const tCIDLib::TCard4     c4GroupSize
                    , const tCIDLib::TCh        chGroupSep)
{
    // Watch for pathological case of zero sized buffer
    if (!c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_ZeroSizedBuffer);
        return kCIDLib::False;
    }

    //
    //  If the group size is non-zero, that means put in separators. So
    //  make sure the separator is not nul.
    //
    if (c4GroupSize && (chGroupSep == kCIDLib::chNull))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcFmt_InvalidGroupSep);
        return kCIDLib::False;
    }

    //
    //  We don't want to deal with any locale specific sign stuff here,
    //  since we are doing it ourself. So we get a temp copy of the value
    //  and make sure its positive.
    //
    const tCIDLib::TCard8 c8PosVal = tCIDLib::TCard8(i8Val < 0 ? -i8Val : i8Val);

    tCIDLib::TZStr128 szTmp;
    if (!bCard8ToText(c8PosVal, szTmp, 128, eRadix))
        return kCIDLib::False;

    // Handle the sign separately in order to make the below code simpler.
    tCIDLib::TCard4 c4OutInd = 0;
    if (i8Val < 0)
        pszBuf[c4OutInd++] = chNegSign;

    //
    //  If the radix is decimal and we have a group size, then apply the group
    //  info the string. Otherwise, just copy over as is.
    //
    if ((eRadix == tCIDLib::ERadices::Dec) && c4GroupSize)
    {
        if (!bApplyGroupInfo(szTmp, &pszBuf[c4OutInd], c4MaxChars, c4GroupSize, chGroupSep, 0))
            return kCIDLib::False;
    }
     else
    {
        CopyStr(&pszBuf[c4OutInd], szTmp, c4MaxChars);
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean
TRawStr::bFormatVal(const   tCIDLib::TUInt      uVal
                    ,       tCIDLib::TCh* const pszBuf
                    , const tCIDLib::TCard4     c4MaxChars
                    , const tCIDLib::ERadices   eRadix
                    , const tCIDLib::TCard4     c4GroupSize
                    , const tCIDLib::TCh        chGroupSep)
{
    // Just call the TCard4 version
    return bFormatVal
    (
        tCIDLib::TCard4(uVal)
        , pszBuf
        , c4MaxChars
        , eRadix
        , c4GroupSize
        , chGroupSep
    );
}


tCIDLib::TBoolean
TRawStr::bFormatVal(const   tCIDLib::TFloat8&       f8Val
                    ,       tCIDLib::TCh* const     pszBuf
                    , const tCIDLib::TCard4         c4Precision
                    , const tCIDLib::TCard4         c4MaxChars
                    , const tCIDLib::ETrailFmts  eTrail
                    , const tCIDLib::TCh            chNegSign
                    , const tCIDLib::TCh            chDecimal
                    , const tCIDLib::TCard4         c4GroupSize
                    , const tCIDLib::TCh            chGroupSep)
{
    // Watch for pathological case of zero sized buffer
    if (!c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_ZeroSizedBuffer);
        return kCIDLib::False;
    }

    // Init the target string to empty just in case
    pszBuf[0] = kCIDLib::chNull;

    // And create some locals for temp formatting
    tCIDLib::TCh    chFill = kCIDLib::chSpace;
    tCIDLib::TCard4 c4Len;
    tCIDLib::TSInt  iSign, iDec;
    tCIDLib::TCh*   pszCur;
    tCIDLib::TCh    szTmp[512];
    tCIDLib::TCh    szFmt[512];

    // Set up the trailing character
    if (eTrail == tCIDLib::ETrailFmts::Spaces)
        chFill = kCIDLib::chSpace;
    else if (eTrail == tCIDLib::ETrailFmts::Zeroes)
        chFill = kCIDLib::chDigit0;
    else if (eTrail == tCIDLib::ETrailFmts::Ignore)
        chFill = kCIDLib::chNull;

    //
    //  Handle the special case of 0 up front since its easy. We just put
    //  in a 0, then enough fill chars according to the fill style.
    //
    if (!f8Val)
    {
        tCIDLib::TCard4 c4Ind = 0;

        pszBuf[c4Ind++] = kCIDLib::chDigit0;

        if (c4MaxChars == c4Ind)
        {
            pszBuf[c4Ind++] = 0;
            return kCIDLib::True;
        }

        if (c4Precision)
        {
            pszBuf[c4Ind++] = chDecimal;
            if (c4MaxChars == c4Ind)
            {
                pszBuf[c4Ind] = 0;
                return kCIDLib::True;
            }

            for (tCIDLib::TCard4 c4Ind2 = 0; c4Ind2 < c4Precision; c4Ind2++)
            {
                if (c4Ind == c4MaxChars)
                    break;
                pszBuf[c4Ind++] = kCIDLib::chDigit0;
            }
        }
        pszBuf[c4Ind] = 0;
        return kCIDLib::True;
    }

    //
    //  Convert the value into a raw string, using 2 more decimals of
    //  precision than the user wants so as to avoid rounding error. This
    //  string has no sign or decimal place and only contains significant
    //  trailing digits. We also then convert it to a wide char string.
    //
    tCIDLib::TSCh szRaw[512];
    CIDStrOp_FmtFloat
    (
        szRaw, 512, f8Val, c4Precision + 2, &iDec, &iSign
    );
    pszConvert(szRaw, szTmp, c4MaxBufChars(szTmp));

    // Get the length of the string
    c4Len = c4StrLen(szTmp);

    //
    //  First we will do a rough conversion, to just get the sign and decimal
    //  point into the output buffer. Start with getting the sign in if it is
    //  negative.
    //
    pszCur = szFmt;
    if (iSign && chNegSign)
    {
        *pszCur = chNegSign;
        pszCur++;
    }

    //
    //  If the decimal is to the right, then we can do it easily by just
    //  putting the leading zeros in and then appending the result from
    //  above. Else we copy over chars stick in the decimal and then copy
    //  the rest.
    //
    //  Remember where we put the decimal
    //
    tCIDLib::TCard4 c4DecOfs(0);
    if (iDec < 0)
    {
        // Put out the leading zero and decimal
        *pszCur++ = kCIDLib::chDigit0;
        c4DecOfs  = (pszCur - szFmt);
        *pszCur++ = chDecimal;

        // And then the required leading zeros
        while (iDec < 0)
        {
            *pszCur++ = kCIDLib::chDigit0;
            iDec++;
        }

        // And append the rest
        TRawStr::CopyStr(pszCur, szTmp);
    }
     else
    {
        if (iDec == 0)
        {
            *pszCur++ = kCIDLib::chDigit0;
            c4DecOfs  = (pszCur - szFmt);
            *pszCur++ = chDecimal;
        }

        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Len; c4Index++)
        {
            if ((c4Index == tCIDLib::TCard4(iDec)) && (iDec != 0))
            {
                c4DecOfs  = (pszCur - szFmt);
                *pszCur++ = chDecimal;
            }
            *pszCur++ = szTmp[c4Index];
        }
        *pszCur = 0;
    }

    //
    //  If the decimal offset plus one, plus the requested precision
    //  cannot fit in the max chars, we return an overflow error.
    //
    if (c4DecOfs + 1 + c4Precision > c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
        return kCIDLib::False;
    }

    //
    //  Ok, now lets deal with the trailing issues. We have to either add
    //  trailing chars, or remove them, according to the settings. First
    //  though, see if we are already past the output max char size. If
    //  so, then clip it back to that first.
    //
    c4Len = c4StrLen(szFmt);
    if (c4Len > c4MaxChars)
    {
        szFmt[c4MaxChars] = kCIDLib::chNull;
        c4Len = c4MaxChars;
    }
    pszCur = szFmt + c4Len;

    tCIDLib::TCard4 c4CurPrec = (c4Len - c4DecOfs) - 1;
    if (c4CurPrec < c4Precision)
    {
        //
        //  We don't have enough to meet the precision, so append enough
        //  trailing chars to get enough.
        //
        if (eTrail != tCIDLib::ETrailFmts::Ignore)
        {
            while (c4CurPrec < c4Precision)
            {
                *pszCur++ = chFill;
                c4CurPrec++;
            }
        }

        // Cal it off and move back to last char
        *pszCur = kCIDLib::chNull;
        pszCur--;
    }
     else if (c4CurPrec > c4Precision)
    {
        //
        //  We have more precision chars than they asked for. So clip them
        //  off and move back to the new last char.
        //
        pszCur = szFmt + (c4DecOfs + c4Precision + 1);
        *pszCur = kCIDLib::chNull;
        pszCur--;
    }

    //
    //  If the trail type is not zeros, then work backwards either get rid
    //  of any trailing zeros or replace them with spaces.
    //
    if ((pszCur > szFmt) && (eTrail != tCIDLib::ETrailFmts::Zeroes))
    {
        while (*pszCur == kCIDLib::chDigit0)
            *pszCur-- = chFill;
    }

    // If the last char ended up being the decimal, then get rid of it
    if (*pszCur == chDecimal)
        *pszCur = kCIDLib::chNull;

    // Now copy back as much as we can to the caller's buffer
    if (c4GroupSize)
    {
        tCIDLib::TCard4 c4Start = 0;
        if (szFmt[0] == chNegSign)
        {
            pszBuf[0] = szFmt[0];
            c4Start = 1;
        }

        if (!bApplyGroupInfo(&szFmt[c4Start]
                            , &pszBuf[c4Start]
                            , c4MaxChars
                            , c4GroupSize
                            , chGroupSep
                            , chDecimal))
        {
            return kCIDLib::False;
        }
    }
     else
    {
        CopyStr(pszBuf, szFmt, c4MaxChars);
    }
    return kCIDLib::True;
}



tCIDLib::TBoolean TRawStr::bIsAlpha(const tCIDLib::TCh chToTest)
{
    if (CIDStrOp_IsAlpha(chToTest))
        return kCIDLib::True;
    return kCIDLib::False;
}

tCIDLib::TBoolean TRawStr::bIsAlphaNum(const tCIDLib::TCh chToTest)
{
    if (CIDStrOp_IsAlphaNum(chToTest))
        return kCIDLib::True;
    return kCIDLib::False;
}

tCIDLib::TBoolean TRawStr::bIsDigit(const tCIDLib::TCh chToTest)
{
    if (CIDStrOp_IsDigit(chToTest))
        return kCIDLib::True;
    return kCIDLib::False;
}

tCIDLib::TBoolean TRawStr::bIsHexDigit(const tCIDLib::TCh chToTest)
{
    if (CIDStrOp_IsXDigit(chToTest))
        return kCIDLib::True;
    return kCIDLib::False;
}

tCIDLib::TBoolean TRawStr::bIsPunct(const tCIDLib::TCh chToTest)
{
    if (CIDStrOp_IsPunct(chToTest))
        return kCIDLib::True;
    return kCIDLib::False;
}

tCIDLib::TBoolean TRawStr::bIsLower(const tCIDLib::TCh chToTest)
{
    if (CIDStrOp_IsLower(chToTest))
        return kCIDLib::True;
    return kCIDLib::False;
}

tCIDLib::TBoolean TRawStr::bIsSpace(const tCIDLib::TCh chToTest)
{
    if (CIDStrOp_IsSpace(chToTest))
        return kCIDLib::True;
    return kCIDLib::False;
}

tCIDLib::TBoolean TRawStr::bIsUpper(const tCIDLib::TCh chToTest)
{
    if (CIDStrOp_IsUpper(chToTest))
        return kCIDLib::True;
    return kCIDLib::False;
}


//
//  Given two ASCII hex digits, create the corresponding binary byte that
//  the chars represent.
//
tCIDLib::TBoolean TRawStr::bMakeHexByte(const   tCIDLib::TCh        ch1
                                        , const tCIDLib::TCh        ch2
                                        ,       tCIDLib::TCard1&    c1ToFill)
{
    if ((ch1 >= kCIDLib::chLatin_A) && (ch1 <= kCIDLib::chLatin_F))
        c1ToFill = tCIDLib::TCard1((ch1 - kCIDLib::chLatin_A) + 10);
    else if ((ch1 >= kCIDLib::chLatin_a) && (ch1 <= kCIDLib::chLatin_f))
        c1ToFill = tCIDLib::TCard1((ch1 - kCIDLib::chLatin_a) + 10);
    else if ((ch1 >= kCIDLib::chDigit0) && (ch1 <= kCIDLib::chDigit9))
        c1ToFill = tCIDLib::TCard1(ch1 - kCIDLib::chDigit0);
    else
        return kCIDLib::False;

    c1ToFill <<= 4;

    if ((ch2 >= kCIDLib::chLatin_A) && (ch2 <= kCIDLib::chLatin_F))
        c1ToFill |= tCIDLib::TCard1((ch2 - kCIDLib::chLatin_A) + 10);
    else if ((ch2 >= kCIDLib::chLatin_a) && (ch2 <= kCIDLib::chLatin_f))
        c1ToFill |= tCIDLib::TCard1((ch2 - kCIDLib::chLatin_a) + 10);
    else if ((ch2 >= kCIDLib::chDigit0) && (ch2 <= kCIDLib::chDigit9))
        c1ToFill |= tCIDLib::TCard1(ch2 - kCIDLib::chDigit0);
    else
        return kCIDLib::False;

    return kCIDLib::True;
}


tCIDLib::TCard4
TRawStr::c4AsBinary(const   tCIDLib::TCh* const pszToConvert
                    ,       tCIDLib::TBoolean&  bValid
                    , const tCIDLib::ERadices   eRadix) noexcept
{
    tCIDLib::TBoolean   bNeg;
    tCIDLib::TCard4     c4Ret = 0;
    bValid = (i4TextToCard4(pszToConvert, tCIDLib::c4EnumOrd(eRadix), c4Ret, bNeg) == -1);
    if (bNeg)
        bValid = kCIDLib::False;
    return c4Ret;
}


KRNLEXPORT tCIDLib::TCard4 TRawStr::c4ToHex(const tCIDLib::TCh ch1)
{
    tCIDLib::TCard4 c4Ret = 0;
    if ((ch1 >= kCIDLib::chLatin_A) && (ch1 <= kCIDLib::chLatin_F))
        c4Ret = tCIDLib::TCard4((ch1 - kCIDLib::chLatin_A) + 10);
    else if ((ch1 >= kCIDLib::chLatin_a) && (ch1 <= kCIDLib::chLatin_f))
        c4Ret = tCIDLib::TCard4((ch1 - kCIDLib::chLatin_a) + 10);
    else if ((ch1 >= kCIDLib::chDigit0) && (ch1 <= kCIDLib::chDigit9))
        c4Ret = tCIDLib::TCard4(ch1 - kCIDLib::chDigit0);
    return c4Ret;
}


tCIDLib::TCard8
TRawStr::c8AsBinary(const   tCIDLib::TCh* const pszToConvert
                    ,       tCIDLib::TBoolean&  bValid
                    , const tCIDLib::ERadices   eRadix) noexcept
{
    tCIDLib::TBoolean   bNeg;
    tCIDLib::TCard8     c8Ret = 0;
    bValid = (i4TextToCard8(pszToConvert, tCIDLib::c4EnumOrd(eRadix), c8Ret, bNeg) == -1);
    if (bNeg)
        bValid = kCIDLib::False;
    return c8Ret;
}


tCIDLib::TCh TRawStr::chLower(const tCIDLib::TCh chToLower) noexcept
{
    return CIDStrOp_ChToLower(chToLower);
}


tCIDLib::TCh TRawStr::chUpper(const tCIDLib::TCh chToUpper) noexcept
{
    return CIDStrOp_ChToUpper(chToUpper);
}


tCIDLib::TVoid TRawStr::CatStr(         tCIDLib::TCh* const pszTarget
                                , const tCIDLib::TCh* const pszSrc
                                , const tCIDLib::TCard4     c4MaxChars)
{
    // Watch for null source and do nothing
    if (!pszSrc)
        return;

    // Get the current length of the target
    const tCIDLib::TCard4 c4TargetLen = c4StrLen(pszTarget);

    //
    //  Get temp pointers we can run upwards. The target one starts at the
    //  current end of the target string.
    //
    const tCIDLib::TCh* pszS = pszSrc;
    tCIDLib::TCh* pszT = &pszTarget[c4TargetLen];

    //
    //  If the max chars is zero, then we can do the more optimized blind
    //  cat. Else we have to do the limited cat.
    //
    if (c4MaxChars)
    {
        // If the string is already full, then nothing to do
        if (c4TargetLen >= c4MaxChars)
            return;

        // Start counting at the current len
        tCIDLib::TCard4 c4Count = c4TargetLen;

        // And copy over until end of the source or we max out on chars
        while (*pszS && (c4Count < c4MaxChars))
        {
            *pszT++ = *pszS++;
            c4Count++;
        }
    }
     else
    {
        // And copy over until end of the source
        while (*pszS)
            *pszT++ = *pszS++;
    }

    // Cap off the target
    *pszT = kCIDLib::chNull;
}


tCIDLib::TVoid
TRawStr::CopyCatStr(        tCIDLib::TCh* const     pszTarget
                    , const tCIDLib::TCard4         c4MaxChars
                    , const tCIDLib::TCh* const     pszCopy
                    , const tCIDLib::TCh* const     pszCat)
{
    // Empty the target regardless of what happens
    pszTarget[0] = kCIDLib::chNull;

    CopyStr(pszTarget, pszCopy, c4MaxChars);
    CatStr(pszTarget, pszCat, c4MaxChars);
}

tCIDLib::TVoid
TRawStr::CopyCatStr(        tCIDLib::TCh* const     pszTarget
                    , const tCIDLib::TCard4         c4MaxChars
                    , const tCIDLib::TCh* const     pszCopy
                    , const tCIDLib::TCh* const     pszCat1
                    , const tCIDLib::TCh* const     pszCat2)
{
    // Empty the target regardless of what happens
    pszTarget[0] = kCIDLib::chNull;

    CopyStr(pszTarget, pszCopy, c4MaxChars);
    CatStr(pszTarget, pszCat1, c4MaxChars);
    CatStr(pszTarget, pszCat2, c4MaxChars);
}

tCIDLib::TVoid
TRawStr::CopyCatStr(        tCIDLib::TCh* const     pszTarget
                    , const tCIDLib::TCard4         c4MaxChars
                    , const tCIDLib::TCh* const     pszCopy
                    , const tCIDLib::TCh* const     pszCat1
                    , const tCIDLib::TCh* const     pszCat2
                    , const tCIDLib::TCh* const     pszCat3)
{
    // Empty the target regardless of what happens
    pszTarget[0] = kCIDLib::chNull;

    CopyStr(pszTarget, pszCopy, c4MaxChars);
    CatStr(pszTarget, pszCat1, c4MaxChars);
    CatStr(pszTarget, pszCat2, c4MaxChars);
    CatStr(pszTarget, pszCat3, c4MaxChars);
}


tCIDLib::TVoid
TRawStr::CopyStr(           tCIDLib::TSCh* const    pszTarget
                    , const tCIDLib::TSCh* const    pszSrc
                    , const tCIDLib::TCard4         c4MaxChars)
{
    // If a nul source, then obviously nothing to do
    if (!pszSrc)
    {
        *pszTarget = 0;
        return;
    }

    //
    //  If the max chars is zero, then its just a plain string copy, so
    //  do the optimized loop. Otherwise do the limited copy.
    //
    tCIDLib::TSCh* pszT = pszTarget;
    const tCIDLib::TSCh* pszS = pszSrc;
    if (c4MaxChars)
    {
        tCIDLib::TCard4 c4Count = 0;
        while (*pszS && (c4Count < c4MaxChars))
        {
            *pszT++ = *pszS++;
            c4Count++;
        }
    }
     else
    {
        while (*pszS)
            *pszT++ = *pszS++;
    }

    // Cap it off
    *pszT = 0;
}

tCIDLib::TVoid
TRawStr::CopyStr(           tCIDLib::TCh* const pszTarget
                    , const tCIDLib::TCh* const pszSrc
                    , const tCIDLib::TCard4     c4MaxChars)
{
    // If a nul source, then obviously nothing to do
    if (!pszSrc)
    {
        *pszTarget = 0;
        return;
    }

    //
    //  If the max chars is zero, then its just a plain string copy, so
    //  do the optimized loop. Otherwise do the limited copy.
    //
    tCIDLib::TCh* pszT = pszTarget;
    const tCIDLib::TCh* pszS = pszSrc;
    if (c4MaxChars)
    {
        tCIDLib::TCard4 c4Count = 0;
        while (*pszS && (c4Count < c4MaxChars))
        {
            *pszT++ = *pszS++;
            c4Count++;
        }
    }
     else
    {
        while (*pszS)
            *pszT++ = *pszS++;
    }

    // Cap it off
    *pszT = kCIDLib::chNull;
}


tCIDLib::ESortComps
TRawStr::eCompareStr(   const   tCIDLib::TCh* const pszStr1
                        , const tCIDLib::TCh* const pszStr2) noexcept
{
    return eXlatSortRes(CIDStrOp_Compare(pszStr1, pszStr2));
}

tCIDLib::ESortComps
TRawStr::eCompareStrN(  const   tCIDLib::TCh* const pszStr1
                        , const tCIDLib::TCh* const pszStr2
                        , const tCIDLib::TCard4     c4Count) noexcept
{
    return eXlatSortRes(CIDStrOp_CompareN(pszStr1, pszStr2, c4Count));
}


tCIDLib::ESortComps
TRawStr::eCompareStrI(  const   tCIDLib::TCh* const pszStr1
                        , const tCIDLib::TCh* const pszStr2) noexcept
{
    return eXlatSortRes(CIDStrOp_CompareI(pszStr1, pszStr2));
}


tCIDLib::ESortComps
TRawStr::eCompareStrNI( const   tCIDLib::TCh* const pszStr1
                        , const tCIDLib::TCh* const pszStr2
                        , const tCIDLib::TCard4     c4Count) noexcept
{
    return eXlatSortRes(CIDStrOp_CompareNI(pszStr1, pszStr2, c4Count));
}


tCIDLib::ESortComps
TRawStr::eCompareStr(   const   tCIDLib::TSCh* const pszStr1
                        , const tCIDLib::TSCh* const pszStr2) noexcept
{
    return eXlatSortRes(CIDSStrOp_Compare(pszStr1, pszStr2));
}

tCIDLib::ESortComps
TRawStr::eCompareStrN(  const   tCIDLib::TSCh* const    pszStr1
                        , const tCIDLib::TSCh* const    pszStr2
                        , const tCIDLib::TCard4         c4Count) noexcept
{
    return eXlatSortRes(CIDSStrOp_CompareN(pszStr1, pszStr2, c4Count));
}


tCIDLib::ESortComps
TRawStr::eCompareStrI(  const   tCIDLib::TSCh* const pszStr1
                        , const tCIDLib::TSCh* const pszStr2) noexcept
{
    return eXlatSortRes(CIDSStrOp_CompareI(pszStr1, pszStr2));
}


tCIDLib::ESortComps
TRawStr::eCompareStrNI( const   tCIDLib::TSCh* const    pszStr1
                        , const tCIDLib::TSCh* const    pszStr2
                        , const tCIDLib::TCard4         c4Count) noexcept
{
    return eXlatSortRes(CIDSStrOp_CompareNI(pszStr1, pszStr2, c4Count));
}


tCIDLib::TFloat8
TRawStr::f8AsBinary(const   tCIDLib::TCh* const pszToConvert
                    ,       tCIDLib::TBoolean&  bValid) noexcept
{
    // Try to convert it
    tCIDLib::TCh* pszEnd;
    tCIDLib::TFloat8 f8Ret = CIDStrOp_CnvtFloat(pszToConvert, &pszEnd);

    if ((f8Ret == -HUGE_VAL) || (f8Ret == HUGE_VAL))
    {
        bValid = kCIDLib::False;
        return 0.0;
    }

    //
    //  If we didn't convert the whole string, other than perhaps some
    //  trailing whitespace, then it failed.
    //
    if (*pszEnd)
    {
        while (*pszEnd)
        {
            if (!TRawStr::bIsSpace(*pszEnd))
            {
                bValid = kCIDLib::False;
                return 0.0;
            }
            pszEnd++;
        }
    }

    bValid = kCIDLib::True;
    return f8Ret;
}


tCIDLib::TVoid
TRawStr::FillString(        tCIDLib::TCh* const pszBuf
                    , const tCIDLib::TCh        chFill
                    , const tCIDLib::TCard4     c4Count)
{
    //
    //  Optimize if the high and low bytes are the same, since we can then
    //  just do a memset.
    //
    if (((chFill & 0xFF00) >> 8) == (chFill & 0x00FF))
    {
        memset(pszBuf, tCIDLib::TCard1(chFill & 0x00FF), c4Count * 2);
    }
     else
    {
        tCIDLib::TCh* pchBuf = pszBuf;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            *pchBuf++ = chFill;
    }
    pszBuf[c4Count] = kCIDLib::chNull;
}


//
//  We will never return zero as a legal hash. So we return zero if the src
//  string is badly formed, to indicate an error. It's the only error we can
//  return.
//
tCIDLib::THashVal
TRawStr::hshHashStr(const   tCIDLib::TCh* const pszSrc
                    , const tCIDLib::TCard4     c4Modulus)
{
    static const tCIDLib::TCard1 ac1FirstByteMark[7] =
    {
        0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
    };

    // Start the value off as zero
    tCIDLib::TCard4 c4Ret = 0;

    //
    //  In order to maintain platform independent, consistent hashes, we
    //  will convert the string to UTF-8 form to hash it. We don't store any
    //  conversion results, we just add each generated byte to the hash as
    //  we generate it.
    //
    const tCIDLib::TCh* pchSrcPtr   = pszSrc;
    const tCIDLib::TCh* pchSrcEnd   = pszSrc + c4StrLen(pszSrc);

    while (pchSrcPtr < pchSrcEnd)
    {
        //
        //  Get the next char out into a 32 bit value. If its a leading char,
        //  get the trailing char and put that in. If the next char is not in
        //  there, then we can't do anything but ignore the whole last char.
        //
        tCIDLib::TCard4 c4Val = *pchSrcPtr;

        tCIDLib::TCard4 c4SrcUsed = 1;
        if ((c4Val >= 0xD800) && (c4Val <= 0xDBFF))
        {
            if (pchSrcPtr + 1 >= pchSrcEnd)
                break;

            // Create the composite surrogate pair
            c4Val = ((c4Val - 0xD800) << 10)
                    + ((*(pchSrcPtr + 1) - 0xDC00) + 0x10000);
            c4SrcUsed++;
        }

        // Figure out how many bytes we need
        tCIDLib::TCard4 c4EncBytes;
        if (c4Val < 0x80)
            c4EncBytes = 1;
        else if (c4Val < 0x800)
            c4EncBytes = 2;
        else if (c4Val < 0x10000)
            c4EncBytes = 3;
        else if (c4Val < 0x200000)
            c4EncBytes = 4;
        else if (c4Val < 0x4000000)
            c4EncBytes = 5;
        else if (c4Val <= 0x7FFFFFFF)
            c4EncBytes = 6;
        else
        {
            // Return zero to indicate an error
            return 0;
        }

        // We can do it, so update the source index
        pchSrcPtr += c4SrcUsed;

        // And spit out the bytes
        switch(c4EncBytes)
        {
            case 6 : c4Ret = c4AddToHash(c4Ret, (c4Val | 0x80UL) & 0xBFUL);
                     c4Val >>= 6;
            case 5 : c4Ret = c4AddToHash(c4Ret, (c4Val | 0x80UL) & 0xBFUL);
                     c4Val >>= 6;
            case 4 : c4Ret = c4AddToHash(c4Ret, (c4Val | 0x80UL) & 0xBFUL);
                     c4Val >>= 6;
            case 3 : c4Ret = c4AddToHash(c4Ret, (c4Val | 0x80UL) & 0xBFUL);
                     c4Val >>= 6;
            case 2 : c4Ret = c4AddToHash(c4Ret, (c4Val | 0x80UL) & 0xBFUL);
                     c4Val >>= 6;
            case 1 : c4Ret = c4AddToHash(c4Ret, c4Val | ac1FirstByteMark[c4EncBytes]);
        }
    }

    // If the hash is zero, set it to 1. We never return zero as a hash
    c4Ret %= c4Modulus;
    if (!c4Ret)
        c4Ret = 1;

    return c4Ret;
}


tCIDLib::TInt4
TRawStr::i4AsBinary(const   tCIDLib::TCh* const pszToConvert
                    ,       tCIDLib::TBoolean&  bValid
                    , const tCIDLib::ERadices   eRadix) noexcept
{
    tCIDLib::TBoolean   bNeg;
    tCIDLib::TCard4     c4Ret = 0;
    bValid = (i4TextToCard4(pszToConvert, tCIDLib::c4EnumOrd(eRadix), c4Ret, bNeg) == -1);

    if (bValid)
    {
        if (c4Ret > tCIDLib::TCard4(kCIDLib::i4MaxInt))
        {
            bValid = kCIDLib::False;
            return 0;
        }

        if (bNeg)
            return tCIDLib::TInt4(c4Ret) * -1;
    }
    return tCIDLib::TInt4(c4Ret);
}


tCIDLib::TInt8
TRawStr::i8AsBinary(const   tCIDLib::TCh* const pszToConvert
                    ,       tCIDLib::TBoolean&  bValid
                    , const tCIDLib::ERadices   eRadix) noexcept
{
    tCIDLib::TBoolean   bNeg;
    tCIDLib::TCard8     c8Ret = 0;
    bValid = (i4TextToCard8(pszToConvert, tCIDLib::c4EnumOrd(eRadix), c8Ret, bNeg) == -1);

    if (bValid)
    {
        if (c8Ret > tCIDLib::TCard4(kCIDLib::i8MaxInt))
        {
            bValid = kCIDLib::False;
            return 0;
        }

        if (bNeg)
            return tCIDLib::TInt8(c8Ret) * -1;
    }
    return tCIDLib::TInt8(c8Ret);
}


tCIDLib::TCh*
TRawStr::pszConvert(const   tCIDLib::TSCh* const    pszToConvert
                    ,       tCIDLib::TCh* const     pszTarget
                    , const tCIDLib::TCard4         c4MaxChars)
{
    if (!pszToConvert)
    {
        pszTarget[0] = kCIDLib::chNull;
        return pszTarget;
    }

    pszTarget[0] = 0;
    size_t OutChars = size_t(-1);
    if (!CIDStrOp_MBToWC(pszTarget, c4MaxChars + 1, pszToConvert, OutChars))
        pszTarget[c4MaxChars] = 0;
    return pszTarget;
}

tCIDLib::TCh* TRawStr::pszConvert(const tCIDLib::TSCh* const pszToConvert)
{
    tCIDLib::TCh* pszNew = 0;

    if (pszToConvert)
    {
        // Allocate a buffer big enough
        tCIDLib::TCard4 c4Len = strlen(pszToConvert);
        pszNew = new tCIDLib::TCh[c4Len + 1];

        // Convert into the new string and cap it off
        size_t OutChars = size_t(-1);
        if (CIDStrOp_MBToWC(pszNew, c4Len + 1, pszToConvert, OutChars))
            pszNew[0] = 0;
        else
            pszNew[c4Len] = 0;
    }
     else
    {
        pszNew = new tCIDLib::TCh[1];
        pszNew[0] = kCIDLib::chNull;
    }
    return pszNew;
}

tCIDLib::TSCh*
TRawStr::pszConvert(const   tCIDLib::TCh* const     pszToConvert
                    ,       tCIDLib::TSCh* const    pszTarget
                    , const tCIDLib::TCard4         c4MaxChars)
{
    if (!pszToConvert)
    {
        pszTarget[0] = 0;
        return pszTarget;
    }

    CIDStrOp_WCToMB(pszTarget, pszToConvert, c4MaxChars + 1);
    pszTarget[c4MaxChars] = 0;
    return pszTarget;
}

tCIDLib::TSCh* TRawStr::pszConvert(const tCIDLib::TCh* const pszToConvert)
{
    tCIDLib::TSCh* pszNew = 0;

    if (pszToConvert)
    {
        // Calc the size required for this string
        size_t NeededBytes;
        CIDStrOp_CalcMBSize(pszToConvert, NeededBytes);

        // Allocate a buffer big enough
        pszNew = new tCIDLib::TSCh[NeededBytes + 1];

        // Convert into the new string and cap it off
        if (NeededBytes)
            CIDStrOp_WCToMB(pszNew, pszToConvert, NeededBytes);
        pszNew[NeededBytes] = 0;
    }
     else
    {
        pszNew = new tCIDLib::TSCh[1];
        pszNew[0] = 0;
    }
    return pszNew;
}


tCIDLib::TCh*
TRawStr::pszFindChar(       tCIDLib::TCh* const     pszToSearch
                    , const tCIDLib::TCh            chToFind
                    , const tCIDLib::TCard4         c4StartAt
                    , const tCIDLib::TBoolean       bCase)
{
    const tCIDLib::TCard4 c4At = ::c4FindChar(pszToSearch, chToFind, c4StartAt, bCase);
    if (c4At == kCIDLib::c4MaxCard)
        return 0;
    return &pszToSearch[c4At];
}

const tCIDLib::TCh*
TRawStr::pszFindChar(const  tCIDLib::TCh* const     pszToSearch
                    , const tCIDLib::TCh            chToFind
                    , const tCIDLib::TCard4         c4StartAt
                    , const tCIDLib::TBoolean       bCase)
{
    const tCIDLib::TCard4 c4At = ::c4FindChar(pszToSearch, chToFind, c4StartAt, bCase);
    if (c4At == kCIDLib::c4MaxCard)
        return 0;
    return &pszToSearch[c4At];
}


tCIDLib::TCh*
TRawStr::pszFindChars(          tCIDLib::TCh* const     pszToSearch
                        , const tCIDLib::TCh* const     pszChars
                        , const tCIDLib::TCard4         c4StartAt
                        , const tCIDLib::TBoolean       bCase)
{
    const tCIDLib::TCard4 c4At = ::c4FindChars(pszToSearch, pszChars, c4StartAt, bCase);
    if (c4At == kCIDLib::c4MaxCard)
        return 0;
    return &pszToSearch[c4At];
}

const tCIDLib::TCh*
TRawStr::pszFindChars(  const   tCIDLib::TCh* const     pszToSearch
                        , const tCIDLib::TCh* const     pszChars
                        , const tCIDLib::TCard4         c4StartAt
                        , const tCIDLib::TBoolean       bCase)
{
    const tCIDLib::TCard4 c4At = ::c4FindChars(pszToSearch, pszChars, c4StartAt, bCase);
    if (c4At == kCIDLib::c4MaxCard)
        return 0;
    return &pszToSearch[c4At];
}


tCIDLib::TCh*
TRawStr::pszFindLastChar(       tCIDLib::TCh* const     pszToSearch
                        , const tCIDLib::TCh            chToFind
                        , const tCIDLib::TBoolean       bCase)
{
    const tCIDLib::TCard4 c4Len = c4StrLen(pszToSearch);
    if (!c4Len)
        return nullptr;
    const tCIDLib::TCard4 c4At = ::c4FindPrevChar
    (
        pszToSearch, chToFind, c4Len - 1, bCase
    );
    if (c4At == kCIDLib::c4MaxCard)
        return 0;
    return &pszToSearch[c4At];
}

const tCIDLib::TCh*
TRawStr::pszFindLastChar(   const   tCIDLib::TCh* const     pszToSearch
                            , const tCIDLib::TCh            chToFind
                            , const tCIDLib::TBoolean       bCase)
{
    const tCIDLib::TCard4 c4Len = c4StrLen(pszToSearch);
    if (!c4Len)
        return nullptr;
    const tCIDLib::TCard4 c4At = ::c4FindPrevChar
    (
        pszToSearch, chToFind, c4Len - 1, bCase
    );
    if (c4At == kCIDLib::c4MaxCard)
        return 0;
    return &pszToSearch[c4At];
}


tCIDLib::TCh*
TRawStr::pszFindLastChars(          tCIDLib::TCh* const     pszToSearch
                            , const tCIDLib::TCh* const     pszChars
                            , const tCIDLib::TBoolean       bCase)
{
    const tCIDLib::TCard4 c4Len = c4StrLen(pszToSearch);
    if (!c4Len)
        return nullptr;
    const tCIDLib::TCard4 c4At = ::c4FindPrevChars
    (
        pszToSearch, pszChars, c4Len - 1, bCase
    );
    if (c4At == kCIDLib::c4MaxCard)
        return 0;
    return &pszToSearch[c4At];
}

const tCIDLib::TCh*
TRawStr::pszFindLastChars(  const   tCIDLib::TCh* const     pszToSearch
                            , const tCIDLib::TCh* const     pszChars
                            , const tCIDLib::TBoolean       bCase)
{
    const tCIDLib::TCard4 c4Len = c4StrLen(pszToSearch);
    if (!c4Len)
        return nullptr;
    const tCIDLib::TCard4 c4At = ::c4FindPrevChars
    (
        pszToSearch, pszChars, c4Len - 1, bCase
    );
    if (c4At == kCIDLib::c4MaxCard)
        return 0;
    return &pszToSearch[c4At];
}


tCIDLib::TCh*
TRawStr::pszFindPrevChar(       tCIDLib::TCh* const     pszToSearch
                        , const tCIDLib::TCh            chToFind
                        , const tCIDLib::TCard4         c4StartAt
                        , const tCIDLib::TBoolean       bCase)
{
    const tCIDLib::TCard4 c4At = ::c4FindPrevChar
    (
        pszToSearch, chToFind, c4StartAt, bCase
    );
    if (c4At == kCIDLib::c4MaxCard)
        return 0;
    return &pszToSearch[c4At];
}

const tCIDLib::TCh*
TRawStr::pszFindPrevChar(const  tCIDLib::TCh* const     pszToSearch
                        , const tCIDLib::TCh            chToFind
                        , const tCIDLib::TCard4         c4StartAt
                        , const tCIDLib::TBoolean       bCase)
{
    const tCIDLib::TCard4 c4At = ::c4FindPrevChar
    (
        pszToSearch, chToFind, c4StartAt, bCase
    );
    if (c4At == kCIDLib::c4MaxCard)
        return 0;
    return &pszToSearch[c4At];
}


tCIDLib::TCh*
TRawStr::pszFindPrevChars(          tCIDLib::TCh* const     pszToSearch
                            , const tCIDLib::TCh* const     pszChars
                            , const tCIDLib::TCard4         c4StartAt
                            , const tCIDLib::TBoolean       bCase)
{
    const tCIDLib::TCard4 c4At = ::c4FindPrevChars
    (
        pszToSearch, pszChars, c4StartAt, bCase
    );
    if (c4At == kCIDLib::c4MaxCard)
        return 0;
    return &pszToSearch[c4At];
}

const tCIDLib::TCh*
TRawStr::pszFindPrevChars(  const   tCIDLib::TCh* const     pszToSearch
                            , const tCIDLib::TCh* const     pszChars
                            , const tCIDLib::TCard4         c4StartAt
                            , const tCIDLib::TBoolean       bCase)
{
    const tCIDLib::TCard4 c4At = ::c4FindPrevChars
    (
        pszToSearch, pszChars, c4StartAt, bCase
    );
    if (c4At == kCIDLib::c4MaxCard)
        return 0;
    return &pszToSearch[c4At];
}


tCIDLib::TCh*
TRawStr::pszFindPrevSubStr(         tCIDLib::TCh* const     pszToSearch
                            , const tCIDLib::TCh* const     pszSubStr
                            , const tCIDLib::TCard4         c4StartAt
                            , const tCIDLib::TBoolean       bCase)
{
    //
    //  To avoid alot of duplicated code, we call the const version and
    //  cast on the way out. That is ok, since we are calling a more
    //  restrictive version.
    //
    return const_cast<tCIDLib::TCh*>
    (
        pszFindPrevSubStr
        (
            static_cast<const tCIDLib::TCh*>(pszToSearch)
            , pszSubStr
            , c4StartAt
            , bCase
        )
    );
}

const tCIDLib::TCh*
TRawStr::pszFindPrevSubStr( const   tCIDLib::TCh* const     pszToSearch
                            , const tCIDLib::TCh* const     pszSubStr
                            , const tCIDLib::TCard4         c4StartAt
                            , const tCIDLib::TBoolean       bCase)
{
    // If either string is null, then nothing to do
    if (!pszToSearch || !pszSubStr)
        return 0;

    // Get the length of the string to search and the substring
    const tCIDLib::TCard4 c4SearchLen = c4StrLen(pszToSearch);
    const tCIDLib::TCard4 c4SubLen = c4StrLen(pszSubStr);

    // If either length is zero, then nothing to do
    if (!c4SearchLen || !c4SubLen)
        return 0;

    // If the sub length is greater than the search length, then done
    if (c4SubLen > c4SearchLen)
        return 0;

    //
    //  If the substring is only one char long, then just call the single
    //  character search method.
    //
    if (c4SubLen == 1)
        return pszFindPrevChar(pszToSearch, pszSubStr[0], c4StartAt, bCase);

    //
    //  Get the first and last character of the substring. This will be used
    //  to do fast checks before comparing the whole substring.
    //
    const tCIDLib::TCh chFirst = pszSubStr[0];
    const tCIDLib::TCh chLast = pszSubStr[c4SubLen-1];

    //
    //  Now start looping through the search string. We start at the last index
    //  that could possibly match, which is the first index below the start
    //  index that the substring could fit into.
    //
    tCIDLib::TInt4 i4Index;
    if ((c4StartAt == kCIDLib::c4MaxCard) || (c4StartAt + c4SubLen >= c4SearchLen))
        i4Index = tCIDLib::TInt4(c4SearchLen - c4SubLen);
    else
        i4Index = tCIDLib::TInt4(c4StartAt);

    if (bCase)
    {
        for (; i4Index >= 0; i4Index--)
        {
            //
            //  If the first and last char of the substring line up here,
            //  then we need to compare the whole substring. Else, just
            //  continue and try the previous character.
            //
            if ((pszToSearch[i4Index] != chFirst)
            ||  (pszToSearch[i4Index + (c4SubLen-1)] != chLast))
            {
                continue;
            }

            //
            //  Ok, the first and last char match up. So lets compare the whole
            //  substring.
            //
            tCIDLib::TCard4 c4SubInd = 1;
            for (; c4SubInd < c4SubLen; c4SubInd++)
            {
                if (pszToSearch[i4Index + c4SubInd] != pszSubStr[c4SubInd])
                    break;
            }

            //
            //  If we went to the sub length, then we found it. Else try
            //  next char
            //
            if (c4SubInd == c4SubLen)
                return &pszToSearch[i4Index];
        }
    }
     else
    {
        // Same as loop above but case insensitive
        for (; i4Index >= 0; i4Index--)
        {
            if ((chUpper(pszToSearch[i4Index]) != chUpper(chFirst))
            ||  (chUpper(pszToSearch[i4Index + (c4SubLen-1)]) != chUpper(chLast)))
            {
                continue;
            }

            tCIDLib::TCard4 c4SubInd = 1;
            for (; c4SubInd < c4SubLen; c4SubInd++)
            {
                if (chUpper(pszToSearch[i4Index + c4SubInd]) != chUpper(pszSubStr[c4SubInd]))
                    break;
            }

            if (c4SubInd == c4SubLen)
                return &pszToSearch[i4Index];
        }
    }

    // Did not find a match
    return 0;
}


tCIDLib::TCh*
TRawStr::pszFindSubStr(         tCIDLib::TCh* const     pszToSearch
                        , const tCIDLib::TCh* const     pszSubStr
                        , const tCIDLib::TCard4         c4StartAt
                        , const tCIDLib::TBoolean       bCase)
{
    //
    //  To avoid alot of duplicated code, we call the const version and
    //  cast on the way out. That is ok, since we are calling a more
    //  restrictive version.
    //
    return const_cast<tCIDLib::TCh*>
    (
        pszFindSubStr
        (
            static_cast<const tCIDLib::TCh*>(pszToSearch)
            , pszSubStr
            , c4StartAt
            , bCase
        )
    );
}

const tCIDLib::TCh*
TRawStr::pszFindSubStr( const   tCIDLib::TCh* const     pszToSearch
                        , const tCIDLib::TCh* const     pszSubStr
                        , const tCIDLib::TCard4         c4StartAt
                        , const tCIDLib::TBoolean       bCase)
{
    // If either string is null, then nothing to do
    if (!pszToSearch || !pszSubStr)
        return nullptr;

    // Get the length of the string to search and the substring
    const tCIDLib::TCard4 c4SearchLen = c4StrLen(pszToSearch);
    const tCIDLib::TCard4 c4SubLen = c4StrLen(pszSubStr);

    // If either length is zero, then nothing to do
    if (!c4SearchLen || !c4SubLen)
        return nullptr;

    //
    //  In debug mode, make sure start is within string. It can be at the
    //  end, and we'll just fail in the next step below, now matter how
    //  short the search string.
    //
    #if CID_DEBUG_ON
    if (c4StartAt > c4SearchLen)
    {
        kmodCIDKernel.ErrorPopUp
        (
            kKrnlErrs::errcStr_StartPastEnd
            , CID_FILE
            , CID_LINE
        );
    }
    #endif

    //
    //  If the sub length + start is greater than the search length, then
    //  done. Note that we test for > not >= because if it's equal there
    //  could be a match at the start position.
    //
    if (c4SubLen + c4StartAt > c4SearchLen)
        return nullptr;

    //
    //  If the substring is only one char long, then just call the single
    //  character search method.
    //
    if (c4SubLen == 1)
        return pszFindChar(pszToSearch, pszSubStr[0], c4StartAt, bCase);

    //
    //  Get the first and last character of the substring. This will be used
    //  to do fast checks before comparing the whole substring.
    //
    const tCIDLib::TCh chFirst = pszSubStr[0];
    const tCIDLib::TCh chLast = pszSubStr[c4SubLen-1];

    //
    //  Now start looping through the search string. We start at the start
    //  index but only have to search up to (c4SearchLen-c4SubLen) because
    //  anything after that could not match the substring.
    //
    tCIDLib::TCard4 c4Index = c4StartAt;
    if (bCase)
    {
        for (; c4Index <= c4SearchLen - c4SubLen; c4Index++)
        {
            //
            //  If the first and last char of the substring line up here,
            //  then we need to compare the whole substring. Else, just
            //  continue and try the next character.
            //
            if (pszToSearch[c4Index] != chFirst)
                continue;

            if (pszToSearch[c4Index + (c4SubLen-1)] != chLast)
                continue;

            //
            //  Ok, the first and last char match up. So lets compare the
            //  whole substring.
            //
            tCIDLib::TCard4 c4SubInd = 1;
            for (; c4SubInd < c4SubLen; c4SubInd++)
            {
                if (pszToSearch[c4Index+c4SubInd] != pszSubStr[c4SubInd])
                    break;
            }

            //
            //  If we went to the sub length, then we found it. Else try
            //  next char.
            //
            if (c4SubInd == c4SubLen)
                return &pszToSearch[c4Index];
        }
    }
     else
    {
        // Save as above but case insensitive
        for (; c4Index <= c4SearchLen - c4SubLen; c4Index++)
        {
            if (chUpper(pszToSearch[c4Index]) != chUpper(chFirst))
                continue;
            if (chUpper(pszToSearch[c4Index + (c4SubLen-1)]) != chUpper(chLast))
                continue;

            tCIDLib::TCard4 c4SubInd = 1;
            for (; c4SubInd < c4SubLen; c4SubInd++)
            {
                if (chUpper(pszToSearch[c4Index+c4SubInd]) != chUpper(pszSubStr[c4SubInd]))
                    break;
            }

            if (c4SubInd == c4SubLen)
                return &pszToSearch[c4Index];
        }
    }

    // Did not find a match
    return nullptr;
}


tCIDLib::TCh*
TRawStr::pszStrTokenize(        tCIDLib::TCh* const pszSource
                        , const tCIDLib::TCh* const pszWhitespace
                        ,       tCIDLib::TCh**      pszContext)
{
    return CIDStrOp_StrTok(pszSource, pszWhitespace, pszContext);
}


tCIDLib::TCh* TRawStr::pszLowerCase(tCIDLib::TCh* pszToLower) noexcept
{
    CIDStrOp_StrToLower(pszToLower);
    return pszToLower;
}


tCIDLib::TCh* TRawStr::pszUpperCase(tCIDLib::TCh* pszToUpper) noexcept
{
    CIDStrOp_StrToUpper(pszToUpper);
    return pszToUpper;
}


//
//  Strips the indicated charactres from the passed buffer. We support a set of mode
//  flags, for stripping leading, trailing, and/or middle content, to limit where it
//  happens.
//
//  If stipping in the middle, it will normally replace the sub-stripng with an
//  indicated replacement char, or you can set the 'Total' flag in which case it will
//  remove them altogether. The former is very convenient in many cases to, say,
//  consolidate multiple consequtive characters to a single one and that kind of thing,
//  so that we can re-use this tricky method for various things.
//
tCIDLib::TVoid
TRawStr::StripStr(          tCIDLib::TCh* const     pszStripBuf
                    , const tCIDLib::TCh* const     pszStripChars
                    , const tCIDLib::EStripModes    eMode
                    , const tCIDLib::TCh            chRepChar)
{
    // If the string is already empty, then do nothing
    if (!(*pszStripBuf) || !(*pszStripChars))
        return;

    // If no stripping options set if flags then do nothing
    if (eMode == tCIDLib::EStripModes::None)
        return;

    //
    //  Handle special case of single char. We either kill the char and return
    //  an empty string or we return with no action.
    //
    if (!(*(pszStripBuf+1)))
    {
        if (pszFindChar(pszStripChars, *pszStripBuf))
            *pszStripBuf = kCIDLib::chNull;

        return;
    }

    // Get the current size, which we use as the max in string ops
    tCIDLib::TCard4 c4MaxChars = TRawStr::c4StrLen(pszStripBuf);

    // Start off the start pointer at the base of the passed buffer
    tCIDLib::TCh* pszStart = pszStripBuf;

    // Start the end pointer at the nul at the end
    tCIDLib::TCh* pszEnd = pszStripBuf + c4StrLen(pszStripBuf);

    if (tCIDLib::bAllBitsOn(eMode, tCIDLib::EStripModes::Leading))
    {
        //
        //  If leading character stripping is on, then handle that first.
        //  We will just run a pointer up until we hit the first non-strip
        //  char.
        //
        while (*pszStart)
        {
            // If the character is not in the strip list, then break
            if (!pszFindChar(pszStripChars, *pszStart))
                break;

            // Move up a character
            pszStart++;
        }

        // We stripped it away, so zero the passed string and return
        if (!(*pszStart))
        {
            *pszStripBuf = 0;
            return;
        }
    }

    //
    //  Ok, the pszStart pointer points to the first non-strip char. So now
    //  lets do the trailing stripping if needed. We know that there is
    //  text there, at least 1 char, between the start and end.
    //
    if (tCIDLib::bAllBitsOn(eMode, tCIDLib::EStripModes::Trailing))
    {
        while (pszEnd > pszStart)
        {
            // If previous char is not in the strip char list, then done
            if (!pszFindChar(pszStripChars, *(pszEnd-1)))
                break;
            pszEnd--;
        }

        // If we hit the leading edge, then we are done
        if (pszEnd == pszStart)
        {
            pszStripBuf[0] = 0;
            return;
        }

        // Otherwise, put a nul in the end
        *pszEnd = 0;
    }

    //
    //  Ok we now have the starting and ending pointers, which we know are
    //  not the same so something is in there. If middle stripping is on,
    //  the we have to handle it. If the string that is left at this point
    //  is 1 or 2 chars, then nothing to do because there cannot be any
    //  middle.
    //
    //  !!!!! We initially reference pszEnd, but once we start stripping
    //  !!!!! in the middle, it is no longer valid and is not used anymore
    //
    if (tCIDLib::bAllBitsOn(eMode, tCIDLib::EStripModes::Middle)
    &&  (c4StrLen(pszStart) > 2))
    {
        tCIDLib::TBoolean bTotComp = kCIDLib::False;

        // Test for total compaction flag. If on, then set the bTotComp flag.
        if (tCIDLib::bAllBitsOn(eMode, tCIDLib::EStripModes::Total))
            bTotComp = kCIDLib::True;

        //
        //  We have to start at the first non-strip char. If leading stripping
        //  was done, then the loop will fall out immediately. If we go all
        //  the way to the end, then strip the whole string clean and return.
        //
        while (pszStart < pszEnd)
        {
            if (!pszFindChar(pszStripChars, *pszStart))
                break;

            pszStart++;
        }

        // If we stripped the whole thing, then we are done
        if (pszStart == pszEnd)
        {
            *pszStripBuf = 0;
            return;
        }

        //
        //  If there is only 1 char left, we are done. We just have to copy
        //  the 1 char down and nul terminate it.
        //
        if (!(*(pszStart+1)))
        {
            if (pszStart != pszStripBuf)
            {
                *pszStripBuf = *pszStart;
                *(pszStripBuf+1) = 0;
            }
            return;
        }

        // !!! pszEnd is no longer valid from here on potentially

        //
        //  We use two pointers here, one of which is anchor that is dropped
        //  when we find a strip char, and the other run up to the next
        //  non-strip char. Then we can just copy the string downward to get
        //  rid of the chars in between.
        //
        //  Start running the search up at the next char after the start
        //  char which cannot be changed by middle stripping if leading
        //  stripping was done.
        //
        tCIDLib::TCh*   pszAnchor = pszStart+1;
        tCIDLib::TCh*   pszSearch = pszAnchor;

        while (*pszSearch)
        {
            // Look for a strip char via the search pointer
            while (*pszSearch)
            {
                if (pszFindChar(pszStripChars, *pszSearch))
                    break;
                pszSearch++;
            }

            //
            //  If we hit the end then done, because they are trailing,
            //  not middle, strip chars.
            //
            if (!(*pszSearch))
                break;

            //
            //  Drop the anchor pointer here and start looking for a non
            //  strip char at the next location.
            //
            pszAnchor = pszSearch;
            pszSearch++;
            while (*pszSearch)
            {
                if (!pszFindChar(pszStripChars, *pszSearch))
                    break;

                pszSearch++;
            }

            //
            //  If we hit the end then done, because they are trailing,
            //  not middle, strip chars.
            //
            if (!(*pszSearch))
                break;

            //
            //  Ok, we can copy down from the char where the search hit a
            //  non-strip, to the anchor pointer. We have to deal with the
            //  special case of total compaction vs. non-total. If not
            //  total, then put a rep char in at the anchor and bump it up
            //  before doing the copy.
            //
            if (!bTotComp)
            {
                *pszAnchor = chRepChar;
                pszAnchor++;
            }

            //
            //  If we did not bump the anchor back up to the search, then copy
            //  the contents back down.
            //
            if (pszAnchor != pszSearch)
                CopyStr(pszAnchor, pszSearch, c4MaxChars);

            // Start the search at the anchor point
            pszSearch = pszAnchor;
        }
    }

    //
    //  Ok, we need to copy from pszStart to the start of the buffer. These
    //  ranges overlap but pszStart is guaranteed to be greater than the
    //  buffer start (if they are not equal.) If they are equal, then we are
    //  done.
    //
    if (pszStart == pszStripBuf)
        return;

    CopyStr(pszStripBuf, pszStart);
}
