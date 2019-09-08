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
#include    <unistd.h>


// ---------------------------------------------------------------------------
//  Local const data
//
//  m_c4Modulus
//      The modulus used in the hashing of the key strings. This is passed
//      along to the hash map object.
// ---------------------------------------------------------------------------
static const tCIDLib::TCard4    m_c4Modulus = 23;



// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::bInitTermEnvironment(const tCIDLib::EInitTerm eInitTerm)
{
    if (eInitTerm == tCIDLib::EInitTerm::Initialize)
    {
        // Allocate our critical section and our hash map objects.
        TKrnlEnvironment::s_pkcrsAccess = new TKrnlCritSec;
        TKrnlEnvironment::s_pkhshmEnv = new TKrnlHashMap(m_c4Modulus);

        //
        //  Now load up the environment with all of the environment
        //  strings that are set.
        //
        tCIDLib::TZStr512   szTmp;

        for (tCIDLib::TCard4 c4Count = 0; environ[c4Count]; c4Count++)
        {
            // Make a copy of the current element
            TRawStr::pszConvert(environ[c4Count], szTmp, c4MaxBufChars(szTmp));

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
            if (!TKrnlEnvironment::s_pkhshmEnv->bAdd(szTmp, pszSplit+1))
                return kCIDLib::False;
        }
    }
     else
    {
        // Delete our critical section and our hash map objects.
        delete TKrnlEnvironment::s_pkcrsAccess;
        delete TKrnlEnvironment::s_pkhshmEnv;
    }
    return kCIDLib::True;
}
