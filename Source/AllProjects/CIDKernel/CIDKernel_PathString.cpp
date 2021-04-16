//
// FILE NAME: CIDKernel_PathString.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/09/2020
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2020
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file provides the platform independent implementation of the
//  TKrnlPathStr class.
//
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



//
//  Adds two path components together, insuring that they have one and only one
//  separator between them. We can do this for both Windows and Linux since the
//  only difference is the path separator and that's platform defined.
//
tCIDLib::TBoolean
TKrnlPathStr::bCombinePath(         tCIDLib::TCh* const     pszToFill
                            , const tCIDLib::TCh* const     pszFirst
                            , const tCIDLib::TCh* const     pszSecond
                            , const tCIDLib::TCard4         c4MaxChars)
{
    if (!pszToFill)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_NullParm);
        return kCIDLib::False;
    }

    // Worst cast make sur ethe target ends up empty
    *pszToFill = kCIDLib::chNull;

    // If we have a first, then copy it over
    tCIDLib::TCh* pszTar = pszToFill;
    tCIDLib::TCh* pszTarEnd  = pszToFill + c4MaxChars;

    // Add the first part if we have any
    const tCIDLib::TCh* pszSrc = pszFirst;
    if (pszFirst && (*pszFirst != kCIDLib::chNull))
    {
        while ((pszTar < pszTarEnd) && *pszSrc)
            *pszTar++ = *pszSrc++;

        // If we ran out of space, then an error
        if (*pszSrc)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
            return kCIDLib::False;
        }

        // If the last character put in wasn't a separator, then add one
        if (*(pszTar - 1) != kCIDLib::chPathSep)
            *pszTar++ = kCIDLib::chPathSep;
    }

    // And add the second bit in if any
    if (pszSecond && (*pszSecond != kCIDLib::chNull))
    {
        // If the second value starts with a separator, move forward
        pszSrc = pszSecond;
        if (*pszSrc == kCIDLib::chPathSep)
            pszSrc++;

        // Now append this to the target as long as we have space
        while ((pszTar < pszTarEnd) && *pszSrc)
            *pszTar++ = *pszSrc++;

        // If we didn't get all of the src, then an error
        if (*pszSrc)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
            return kCIDLib::False;
        }
    }

    *pszTar = kCIDLib::chNull;
    return kCIDLib::True;
}
