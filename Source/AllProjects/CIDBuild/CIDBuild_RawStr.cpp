//
// FILE NAME: CIDBuild_RawStr.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/1998
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
//  Local utilities
// ---------------------------------------------------------------------------
static tCIDLib::TCard4
c4FormatVal(const   tCIDLib::TCard8     c8Val
            ,       tCIDLib::TCh* const pszToFill
            , const tCIDLib::TCard4     c4MaxChars
            , const tCIDLib::TCard4     c4Radix)
{
    static const tCIDLib::TCh achList[16] =
    {
          L'0'  , L'1'  , L'2'  , L'3'  , L'4'  , L'5'  , L'6'  , L'7'
        , L'8'  , L'9'  , L'A'  , L'B'  , L'C'  , L'D'  , L'E'  , L'F'
    };

    if (!c4MaxChars)
        return 0;

    // Handle special case
    if (!c8Val)
    {
        pszToFill[0] = L'0';
        pszToFill[1] = kCIDLib::chNull;
        return 1;
    }

    // This is used to fill the temp buffer
    tCIDLib::TCard4 c4TmpIndex = 0;

    // A copy of the conversion value that we can modify
    tCIDLib::TCard8 c8TmpVal = c8Val;

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
    if (c4Radix == 2)
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
     else if (c4Radix == 16)
    {
        while (c8TmpVal)
        {
            // Get the mask, which is from 0 to 0xF
            const tCIDLib::TCard4 c4CharInd = (c8TmpVal & 0xFUL);

            // And use it to index the list of characters
            szTmpBuf[c4TmpIndex++] = achList[c4CharInd];

            c8TmpVal >>= 4;
        }
    }
     else if ((c4Radix == 8) || (c4Radix == 10))
    {
        while (c8TmpVal)
        {
            const tCIDLib::TCard4 c4CharInd = (c8TmpVal % c4Radix);
            szTmpBuf[c4TmpIndex++] = achList[c4CharInd];
            c8TmpVal /= c4Radix;
        }
    }
     else
    {
        return kCIDLib::False;
    }

    //
    //  See if have enough room in the caller's buffer. He has to make
    //  sure there's room for the null.
    //
    if (c4TmpIndex > c4MaxChars)
    {
        // Return an empty string
        c4TmpIndex = 0;
    }

    // Reverse the tmp buffer into the caller's buffer
    tCIDLib::TCard4 c4OutIndex = 0;
    for (; c4TmpIndex > 0; c4TmpIndex--)
        pszToFill[c4OutIndex++] = szTmpBuf[c4TmpIndex-1];

    // And cap off the caller's buffer
    pszToFill[c4OutIndex] = kCIDLib::chNull;

    return c4OutIndex;
}        



// ---------------------------------------------------------------------------
//  TRawStr: Stuff we can do portably or in terms of per-paltform stuff
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
    FormatVal(c4ToFormat, szTmp, 127, c4Radix);
    strToFill = szTmp;
}


tCIDLib::TVoid
TRawStr::FormatVal( const   tCIDLib::TCard4     c4ToFormat
                    ,       tCIDLib::TCh* const pszToFill
                    , const tCIDLib::TCard4     c4MaxChars
                    , const tCIDLib::TCard4     c4Radix)
{
    c4FormatVal(c4ToFormat, pszToFill, c4MaxChars, c4Radix);
}                    


tCIDLib::TVoid TRawStr::FormatVal(  const   tCIDLib::TInt4      i4Val
                                    ,       tCIDLib::TCh* const pszToFill
                                    , const tCIDLib::TCard4     c4MaxChars)
{

    // Remember if it's negative or positive and convert to positive
    const tCIDLib::TCard4 c4PosVal = tCIDLib::TCard4(i4Val < 0 ? -i4Val : i4Val);

    // If the max is zero, nothing we can do
    if (!c4MaxChars)
        return;

    //
    //  Reserve one extra character for the sign if negative. Check for
    //  zero max again.
    //
    tCIDLib::TCard4 c4ActualMax = c4MaxChars;    
    if (i4Val < 0)
        c4ActualMax--;
    if (!c4ActualMax)
        return;        

    // And now we can just use the generic unsigned helper, with decimal radix
    tCIDLib::TCard4 c4OutChars = c4FormatVal(c4PosVal, pszToFill, c4ActualMax, 10);
    if (c4OutChars && (i4Val < 0))
        pszToFill[c4OutChars++] = L'-';
}                                    


    tCIDLib::TSInt TRawStr::iCompIStr(  const   tCIDLib::TCh* const pszOne
                                        , const tCIDLib::TCh* const pszTwo)
    {
        const tCIDLib::TCh* psz1 = pszOne;
        const tCIDLib::TCh* psz2 = pszTwo;

        //
        //  While they are equal, keep running up the pointer. If either of
        //  them goes null within the loop, then they are equal.
        //
        while (chToUpper(*psz1) == chToUpper(*psz2))
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

    tCIDLib::TSInt TRawStr::iCompIStrN( const   tCIDLib::TCh* const pszOne
                                        , const tCIDLib::TCh* const pszTwo
                                        , const tCIDLib::TCard4     c4Count)
    {
        const tCIDLib::TCh* psz1 = pszOne;
        const tCIDLib::TCh* psz2 = pszTwo;
        tCIDLib::TCard4 c4Index = 0;
        while (*psz1 && (c4Index < c4Count))
        {
            if (chToUpper(*psz1) != chToUpper(*psz2))
                return (tCIDLib::TSInt(*psz1) - tCIDLib::TSInt(*psz2));
            psz1++;
            psz2++;
            c4Index++;
        }
        return 0;
    }
    