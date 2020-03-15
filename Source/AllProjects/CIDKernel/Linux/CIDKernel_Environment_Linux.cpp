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


// ---------------------------------------------------------------------------
//  TCIDKrnlModule: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlEnvironment::bAddToExePath(const   tCIDLib::TCh* const pszToAdd
                                , const tCIDLib::EStartEnd  eWhere)
{
    TKrnlString kstrTmp;
    if (!bFind(L"PATH", kstrTmp))
        return kCIDLib::False;

    if (eWhere == tCIDLib::EStartEnd::Start)
    {
        TKrnlString kstrPrep(pszToAdd, kCIDLib::szMultiPathSep, kstrTmp.pszValue());
        return bSetSysVar(L"PATH", kstrPrep.pszValue());
    }

    TKrnlString kstrPrep(kstrTmp.pszValue(), kCIDLib::szMultiPathSep, pszToAdd);
    return bSetSysVar(L"PATH", kstrPrep.pszValue());    
}


tCIDLib::TBoolean
TKrnlEnvironment::bAddToLibPath(const   tCIDLib::TCh* const pszToAdd
                                , const tCIDLib::EStartEnd  eWhere)
{
    TKrnlString kstrTmp;
    if (!bFind(L"LD_LIBRARY_PATH", kstrTmp))
        return kCIDLib::False;

    tCIDLib::TBoolean bRes = kCIDLib::False;
    if (eWhere == tCIDLib::EStartEnd::Start)
    {
        TKrnlString kstrPrepend(pszToAdd, kCIDLib::szMultiPathSep, kstrTmp.pszValue());
        bRes = bSetSysVar(L"LD_LIBRARY_PATH", kstrPrepend.pszValue());
        if (bRes)
            s_pkhshmEnv->bUpdateKey(L"LD_LIBRARY_PATH", kstrPrepend.pszValue());
    }
    else
    {
        TKrnlString kstrAppend(kstrTmp.pszValue(), kCIDLib::szMultiPathSep, pszToAdd);
        bRes = bSetSysVar(L"LD_LIBRARY_PATH", kstrAppend.pszValue());    
        if (bRes)
            s_pkhshmEnv->bUpdateKey(L"LD_LIBRARY_PATH", kstrAppend.pszValue());
    }
    return bRes;
}


tCIDLib::TBoolean TKrnlEnvironment::bFindExePath(TKrnlString& kstrToFill)
{
    return bFind(L"PATH", kstrToFill);
}


tCIDLib::TBoolean TKrnlEnvironment::bFindLibPath(TKrnlString& kstrToFill)
{
    return bFind(L"LD_LIBRARY_PATH", kstrToFill);
}


tCIDLib::TBoolean TKrnlEnvironment::bFindTempPath(TKrnlString& kstrToFill)
{
    if (!bFind(L"TMPDIR", kstrToFill))
        kstrToFill = L"/tmp";
    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TKrnlEnvironment: Private, static methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlEnvironment::bSetSysVar(const  tCIDLib::TCh* const pszKey
                            , const tCIDLib::TCh* const pszNewValue)
{
    // Build up the string
    const tCIDLib::TCard4 c4BufLen(TRawStr::c4StrLen(pszKey) + TRawStr::c4StrLen(pszNewValue));
    tCIDLib::TCh* apszBuf = nullptr;
    apszBuf = new tCIDLib::TCh[c4BufLen + 1];
    TArrayJanitor<tCIDLib::TCh> janBuf(apszBuf);
    TRawStr::CopyCatStr(apszBuf, c4BufLen, pszKey, L"=", pszNewValue);

    tCIDLib::TSCh* pszShortBuf = TRawStr::pszConvert(apszBuf);
    TArrayJanitor<tCIDLib::TSCh> janShortBuf(pszShortBuf);

    if (::putenv(pszShortBuf))
    {
        TKrnlError::SetLastHostError(errno);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}

