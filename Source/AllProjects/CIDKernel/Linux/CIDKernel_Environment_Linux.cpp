//
// FILE NAME: CIDKernel_Environment_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 12/09/1998
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
//  This file privdes the Linux specific implemention of the
//  TKrnlEnvironment namespace.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDKernel_.hpp"


// ---------------------------------------------------------------------------
//  Local const data
//
//  __c4Modulus
//      The modulus used in the hashing of the key strings. This is passed
//      along to the hash map object.
// ---------------------------------------------------------------------------
static const tCIDLib::TCard4    __c4Modulus = 23;



// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::__bInitTermEnvironment(const tCIDLib::EInitTerm eInitTerm)
{
    if (eInitTerm == tCIDLib::EInitTerm::Initialize)
    {
        // Allocate our critical section and our hash map objects.
        TKrnlEnvironment::__pkcrsAccess = new TKrnlCritSec;
        TKrnlEnvironment::__pkhshmEnv = new TKrnlHashMap(__c4Modulus);

        //
        //  Now load up the environment with all of the environment
        //  strings that are set.
        //
        tCIDLib::TZStr512   szTmp;

        for (tCIDLib::TCard4 c4Count = 0; __environ[c4Count]; c4Count++)
        {
            // Make a copy of the current element
            TRawStr::pszConvert(__environ[c4Count], szTmp, c4MaxBufChars(szTmp));

            //
            //  We need to find the two parts of the string and pull
            //  it into its key and value sections.
            //
            tCIDLib::TCh* pszSplit = TRawStr::pszFindLastChar
            (
                szTmp
                , kCIDLib::chEquals
            );

            // This is bad, and should not be able to happen
            if (!pszSplit)
            {
                TKrnlError::SetLastKrnlError(kKrnlErrs::errcInit_InvalidEnvironment);
                return kCIDLib::False;
            }

            // Put a null at the split to create two strings
            *pszSplit = kCIDLib::chNull;

            // Uppercase the key
            TRawStr::pszUpperCase(szTmp);

            // And add them now to the hash map
            if (!TKrnlEnvironment::__pkhshmEnv->bAdd(szTmp, pszSplit+1))
                return kCIDLib::False;
        }
    }
    else
    {
        // Delete our critical section and our hash map objects.
        delete TKrnlEnvironment::__pkcrsAccess;
        delete TKrnlEnvironment::__pkhshmEnv;
    }
    return kCIDLib::True;
}
