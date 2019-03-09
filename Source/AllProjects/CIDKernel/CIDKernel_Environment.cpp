//
// FILE NAME: CIDKernel_Environment.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/15/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TKrnlEnvironment namespace's functions. Its all
//  platform independent except for the initialization, which must parse
//  out the process' environment and fill in our information.
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
//  TKrnlEnvironment: Static data members
// ---------------------------------------------------------------------------
TKrnlCritSec*   TKrnlEnvironment::s_pkcrsAccess = nullptr;
TKrnlHashMap*   TKrnlEnvironment::s_pkhshmEnv = nullptr;


// ---------------------------------------------------------------------------
//  TKrnlEnvironment: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlEnvironment::bAdd( const   tCIDLib::TCh* const pszKey
                        , const tCIDLib::TCh* const pszNewValue)
{
    //
    //  We need to create an uppercased version of the key string. This
    //  is not the way some host platforms work, but it makes things
    //  consistent across platforms.
    //
    tCIDLib::TCh* pszUpKey = TRawStr::pszReplicate(pszKey);
    TArrayJanitor<tCIDLib::TCh> janKey(pszUpKey);
    TRawStr::pszUpperCase(pszUpKey);

    // Lock the environment before we update it
    TKrnlCritSecLocker crslAccess(s_pkcrsAccess);

    // And now add the new element wth the uppercased key
    if (!s_pkhshmEnv->bAdd(pszUpKey, pszNewValue))
        return kCIDLib::False;

    // Update the system variable
    return bSetSysVar(pszKey, pszNewValue);
}


tCIDLib::TBoolean
TKrnlEnvironment::bAddOrUpdate( const   tCIDLib::TCh* const pszKey
                                , const tCIDLib::TCh* const pszNewValue
                                ,       tCIDLib::TBoolean&  bAdded)
{
    // Assume we won't add it
    bAdded = kCIDLib::False;

    //  Create an uppercased version of the key
    tCIDLib::TCh* pszUpKey = TRawStr::pszReplicate(pszKey);
    TArrayJanitor<tCIDLib::TCh> janKey(pszUpKey);
    TRawStr::pszUpperCase(pszUpKey);

    // Lock the environment before we update it
    TKrnlCritSecLocker crslAccess(s_pkcrsAccess);

    // Try to update it on the assumption its already there
    if (!s_pkhshmEnv->bUpdateKey(pszUpKey, pszNewValue))
    {
        // If anything other than not found, then return error
        if (TKrnlError::kerrLast().errcId() != kKrnlErrs::errcGen_NotFound)
            return kCIDLib::False;

        // To to add the new value
        bAdded = kCIDLib::True;
        return bAdd(pszKey, pszNewValue);
    }

    // Update the system variable
    return bSetSysVar(pszKey, pszNewValue);
}


tCIDLib::TBoolean
TKrnlEnvironment::bCharsInValue(const   tCIDLib::TCh* const pszKey
                                ,       tCIDLib::TCard4&    c4ToFill)
{
    //  Create an uppercased version of the key
    tCIDLib::TCh* pszUpKey = TRawStr::pszReplicate(pszKey);
    TArrayJanitor<tCIDLib::TCh> janKey(pszUpKey);
    TRawStr::pszUpperCase(pszUpKey);

    {
        // Lock the environment before we update it
        TKrnlCritSecLocker crslAccess(s_pkcrsAccess);

        //
        //  See if this element is in the map. If not, then a not found
        //  then an error is returned.
        //
        const tCIDLib::TCh* pszValue = s_pkhshmEnv->pszValueForKey(pszUpKey);
        if (!pszValue)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotFound);
            return kCIDLib::False;
        }
        c4ToFill = TRawStr::c4StrLen(pszValue);
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlEnvironment::bFind(const   tCIDLib::TCh* const pszKey
                        ,       tCIDLib::TCh* const pszToFill
                        , const tCIDLib::TCard4     c4MaxChars)
{
    //  Create an uppercased version of the key
    tCIDLib::TCh* pszUpKey = TRawStr::pszReplicate(pszKey);
    TArrayJanitor<tCIDLib::TCh> janKey(pszUpKey);
    TRawStr::pszUpperCase(pszUpKey);

    // Lock the environment before we update it
    TKrnlCritSecLocker crslAccess(s_pkcrsAccess);

    const tCIDLib::TCh* pszValue = s_pkhshmEnv->pszValueForKey(pszUpKey);
    if (!pszValue)
        return kCIDLib::False;

    TRawStr::CopyStr(pszToFill, pszValue, c4MaxChars);
    return kCIDLib::True;
}


// Just checks to see if the passed key exists
tCIDLib::TBoolean
TKrnlEnvironment::bKeyExists(const tCIDLib::TCh* const pszKey)
{
    //  Create an uppercased version of the key
    tCIDLib::TCh* pszUpKey = TRawStr::pszReplicate(pszKey);
    TArrayJanitor<tCIDLib::TCh> janKey(pszUpKey);
    TRawStr::pszUpperCase(pszUpKey);

    TKrnlCritSecLocker crslAccess(s_pkcrsAccess);
    return (s_pkhshmEnv->pszValueForKey(pszUpKey) != 0);
}


tCIDLib::TBoolean
TKrnlEnvironment::bQueryState(TElem*& pelemToFill, tCIDLib::TCard4& c4ToFill)
{
    TKrnlCritSecLocker crslAccess(s_pkcrsAccess);

    //
    //  Allocate the array of environment elements, large enough to hold
    //  all our entries.
    //
    tCIDLib::TCard4 c4Count = s_pkhshmEnv->c4ElemCount();
    if (!c4Count)
    {
        c4ToFill = 0;
        return kCIDLib::True;
    }

    pelemToFill = new TElem[c4Count];

    //
    //  We have at least one element so iterate the map and fill in
    //  a record for each one.
    //
    s_pkhshmEnv->bResetIter();
    tCIDLib::TCard4 c4ActualCount = 0;
    do
    {
        const tCIDLib::TCh* pszKey;
        const tCIDLib::TCh* pszValue;
        if (!s_pkhshmEnv->bCurKeyAndValue(pszKey, pszValue))
            return kCIDLib::False;

        pelemToFill[c4ActualCount].Set(pszKey, pszValue);
        c4ActualCount++;
    }   while (s_pkhshmEnv->bNext());

    if (c4ActualCount != c4Count)
    {
        delete [] pelemToFill;
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcEnv_InvalidEnvironment);
        return kCIDLib::False;
    }
    c4ToFill = c4Count;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlEnvironment::bUpdate(  const   tCIDLib::TCh* const pszKey
                            , const tCIDLib::TCh* const pszNewValue)
{
    //  Create an uppercased version of the key
    tCIDLib::TCh* pszUpKey = TRawStr::pszReplicate(pszKey);
    TArrayJanitor<tCIDLib::TCh> janKey(pszUpKey);
    TRawStr::pszUpperCase(pszUpKey);

    // Lock the environment before we search it
    TKrnlCritSecLocker crslAccess(s_pkcrsAccess);

    //
    //  See if this element is in the map. If not, a not found error
    //  is returned.
    //
    if (!s_pkhshmEnv->bUpdateKey(pszUpKey, pszNewValue))
        return kCIDLib::False;

    // Update the system variable
    return bSetSysVar(pszUpKey, pszNewValue);
}


tCIDLib::TCard4 TKrnlEnvironment::c4Entries()
{
    tCIDLib::TCard4 c4Count;
    {
        TKrnlCritSecLocker crslAccess(s_pkcrsAccess);
        c4Count = s_pkhshmEnv->c4ElemCount();
    }
    return c4Count;
}


tCIDLib::TVoid TKrnlEnvironment::Lock()
{
    s_pkcrsAccess->Enter();
}


const tCIDLib::TCh* TKrnlEnvironment::pszFind(const tCIDLib::TCh* const pszKey)
{
    //  Create an uppercased version of the key
    tCIDLib::TCh* pszUpKey = TRawStr::pszReplicate(pszKey);
    TArrayJanitor<tCIDLib::TCh> janKey(pszUpKey);
    TRawStr::pszUpperCase(pszUpKey);

    // Lock the environment before we search it
    TKrnlCritSecLocker crslAccess(s_pkcrsAccess);

    const tCIDLib::TCh* pszValue = s_pkhshmEnv->pszValueForKey(pszUpKey);
    if (!pszValue)
        return 0;

    return pszValue;
}


tCIDLib::TVoid TKrnlEnvironment::Unlock()
{
    s_pkcrsAccess->Exit();
}



// ---------------------------------------------------------------------------
//  TKrnlEnvironment: Hidden default constructor
// ---------------------------------------------------------------------------
TKrnlEnvironment::TKrnlEnvironment()
{
}


