//
// FILE NAME: CIDKernel_Environment_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/12/1998
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
//  This file privdes the Win32 specific implemention of the
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
#include    "CIDKernel_InternalHelpers_.hpp"

namespace
{
    namespace CIDKernel_Environment_Win32
    {
        // -----------------------------------------------------------------------
        //  Local const data
        //
        //  c4Modulus
        //      The modulus used in the hashing of the key strings. This is
        //      passed along to the hash map object.
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard4    c4Modulus = 23;
    }
}


// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::bInitTermEnvironment(const tCIDLib::EInitTerm eState)
{
    tCIDLib::TBoolean bRet = kCIDLib::True;
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
        // Allocate our critical section and our hash map objects.
        TKrnlEnvironment::s_pkcrsAccess = new TKrnlCritSec;
        TKrnlEnvironment::s_pkhshmEnv = new TKrnlHashMap
        (
            CIDKernel_Environment_Win32::c4Modulus
        );

        //
        //  In order to avoid having to create arbitrarily large local
        //  buffers to deal with the values, we want to pull the values
        //  right out of the source buffer, so we make a copy of the
        //  system buffer and then free the system buffer, keeping around
        //  our copy that we can mangle.
        //
        //  In order to duplicate it, we have to figure out how big it is
        //  which is complicated by the fact that we aren't told and it's
        //  a block of multiple zero terminated strings, so the first thing
        //  we do is quickly validate the format and count the size and
        //  number of items in it.
        //
        TArrayJanitor<tCIDLib::TCard1> janRawBuf;
        const tCIDLib::TCh* pszEnd = nullptr;
        tCIDLib::TCh* pszKey = nullptr;
        {
            tCIDLib::TCh* pszSysEnv = ::GetEnvironmentStringsW();
            tCIDLib::TCh* pszRawEnv = pszSysEnv;
            tCIDLib::TBoolean bFirstNul = kCIDLib::False;
            while (kCIDLib::True)
            {
                if (*pszRawEnv == kCIDLib::chNull)
                {
                    // If we just saw one nul, then this is the end
                    if (bFirstNul)
                    {
                        pszRawEnv++;
                        break;
                    }

                    // Else just set the first null flag
                    bFirstNul = kCIDLib::True;
                }
                 else
                {
                    // Not a null so make sure first nul flag is cleared
                    bFirstNul = kCIDLib::False;
                }
                pszRawEnv++;
            }

            // Ok, now allocate the buffer and give it to our janitor.
            tCIDLib::TCard4 c4BufSz = TRawMem::c4AdrDiff(pszSysEnv, pszRawEnv);
            tCIDLib::TCard1* pc1EnvBuf = new tCIDLib::TCard1[c4BufSz];
            janRawBuf.Set(pc1EnvBuf);

            // Copy over the original system buffer and free it
            TRawMem::CopyMemBuf(pc1EnvBuf, pszSysEnv, c4BufSz);
            ::FreeEnvironmentStrings(pszSysEnv);

            // And point the key pointer at the raw buffer as a string
            pszKey = reinterpret_cast<tCIDLib::TCh*>(pc1EnvBuf);

            // And set the end pointer now before we lose the count
            pszEnd = pszKey + (c4BufSz / kCIDLib::c4CharBytes);
        }

        //
        //  Now load up the environment with all of the environment strings
        //  that are set.
        //
        tCIDLib::TCh* pszVal;
        tCIDLib::TCh* pszValE;
        tCIDLib::TCard4 c4Count = 0;
        while (pszKey < pszEnd)
        {
            // If we hit the end, we are done
            if (!*pszKey)
                break;

            // Skip any leading whitespace by running the key pointer up
            while (*pszKey && TRawStr::bIsSpace(*pszKey))
                pszKey++;

            //
            //  Now run the value pointer up to the equals sign or the
            //  first space.
            //
            pszVal = pszKey;
            while (*pszVal
            &&     (*pszVal != kCIDLib::chEquals)
            &&     !TRawStr::bIsSpace(*pszVal))
            {
                pszVal++;
            }

            //
            //  Put a null here to terminate the value, and move the value
            //  pointer up past it to get it ready to find the value.
            //
            *pszVal++ = kCIDLib::chNull;

            // And uppercase the key value
            TRawStr::pszUpperCase(pszKey);

            // Now move the value up to the first non-space
            while (*pszVal && TRawStr::bIsSpace(*pszVal))
                pszVal++;

            // And now find the end of this value and terminate it
            pszValE = pszVal;
            while (*pszValE)
                pszValE++;

            // If we got a key, then we have something to add
            if (*pszKey)
            {
                // Strip trailing space from the value
                TRawStr::StripStr
                (
                    pszVal
                    , kCIDLib::szWhitespace
                    , tCIDLib::EStripModes::Trailing
                );

                //
                //  And add to the list. In theory we cannot get any dups,
                //  but if we do, there's little we can do.
                //
                if (!TKrnlEnvironment::s_pkhshmEnv->bAdd(pszKey, pszVal))
                {
                    // Not much we can do
                }
            }

            // Now set the key pointer to past the last value to start again
            pszKey = pszValE + 1;

            // Keep up with how many we found
            c4Count++;
        }
    }
     else
    {
        // Delete our critical section and our hash map objects.
        delete TKrnlEnvironment::s_pkcrsAccess;
        delete TKrnlEnvironment::s_pkhshmEnv;
    }
    return bRet;
}



// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlEnvironment::bAddToExePath(const   tCIDLib::TCh* const pszToAdd
                                , const tCIDLib::EStartEnd  eWhere)
{
    static const tCIDLib::TCh* pszVar = L"Path";
    TKrnlCritSecLocker crslAccess(s_pkcrsAccess);

    //
    //  First get the length of the PATH variable, and then the whole buffer.
    //  Add one for the possible separate we might have to add, and create
    //  a string to build up the new value in.
    //
    const tCIDLib::TCard4 c4VarLen = ::GetEnvironmentVariable(pszVar, 0, 0);
    tCIDLib::TCh* pszVarVal = new tCIDLib::TCh[c4VarLen];
    ::GetEnvironmentVariable(pszVar, pszVarVal, c4VarLen);
    TArrayJanitor<tCIDLib::TCh> janVarValue(pszVarVal);

    // Create a string with enough to hold it all
    const tCIDLib::TCard4 c4AddLen = TRawStr::c4StrLen(pszToAdd);
    TKrnlString kstrNew(c4VarLen + 1 + c4AddLen);

    //
    //  If adding before, then put the add value in first and make sure it
    //  ends with a separator.
    //
    if (eWhere == tCIDLib::EStartEnd::Start)
    {
        kstrNew.Append(pszToAdd);
        kstrNew.AppendIfNotAlready(kCIDLib::chMultiPathSep);
    }

    // Append the current value
    kstrNew.Append(pszVarVal);

    // And if doing the end, put in a sep char and the value to add
    if (eWhere == tCIDLib::EStartEnd::End)
    {
        kstrNew.AppendIfNotAlready(kCIDLib::chMultiPathSep);
        kstrNew.Append(pszToAdd);
    }

    // Update the system's copy of this one
    if (!::SetEnvironmentVariable(pszVar, kstrNew.pszValue()))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Update our copy of this information
    tCIDLib::TBoolean bDummy;
    if (!bAddOrUpdate(pszVar, kstrNew.pszValue(), bDummy))
        return kCIDLib::False;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlEnvironment::bAddToLibPath(const   tCIDLib::TCh* const pszToAdd
                                , const tCIDLib::EStartEnd  eWhere)
{
    //
    //  Just call the other version, since Windows uses the same PATH variable
    //  for both libs and exes.
    //
    return bAddToExePath(pszToAdd, eWhere);
}



tCIDLib::TBoolean TKrnlEnvironment::bFindExePath(TKrnlString& kstrToFill)
{
    // Just call the public version to get the Path var
    return bFind(L"Path", kstrToFill);
}

tCIDLib::TBoolean TKrnlEnvironment::bFindLibPath(TKrnlString& kstrToFill)
{
    // Just call the public version to get the Path var
    return bFind(L"Path", kstrToFill);
}


tCIDLib::TBoolean TKrnlEnvironment::bFindTempPath(TKrnlString& kstrToFill)
{
    //
    //  Look for the TEMP and TMP variables. If we can't find either of
    //  those, then we fail.
    //
    return (bFind(L"TEMP", kstrToFill) || bFind(L"TMP", kstrToFill));
}



// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Private, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlEnvironment::bSetSysVar(const  tCIDLib::TCh* const pszKey
                            , const tCIDLib::TCh* const pszNewValue)
{
    if (!::SetEnvironmentVariable(pszKey, pszNewValue))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}

