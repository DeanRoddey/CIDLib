//
// FILE NAME: CIDKernel_ResourceName_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 01/02/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file provides the Linux specific implementation of the TKrnlRscName
//  class.
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



// ---------------------------------------------------------------------------
//  TKrnlRscName: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TKrnlRscName::bBuildFullName(       tCIDLib::TCh* const     pszToFill
                            , const tCIDLib::TCard4         c4MaxChars
                            , const tCIDLib::ENamedRscTypes eType)  const
{
    // If its not valid, then return an empty string
    if (!bValid())
    {
        pszToFill[0] = kCIDLib::chNull;
        return kCIDLib::True;
    }

    //
    //  Start by formatting in the prefix that we force into all names. Under
    //  OS/2 there is a prefix defined, but Win32 does not have one. So we
    //  insure that there is no clash among resources of different types.
    //
    if (eType == tCIDLib::ENamedRscTypes::Event)
        TRawStr::CopyStr(pszToFill, L"Event", c4MaxChars);
    else if (eType == tCIDLib::ENamedRscTypes::Memory)
        TRawStr::CopyStr(pszToFill, L"Memory", c4MaxChars);
    else if (eType == tCIDLib::ENamedRscTypes::Mutex)
        TRawStr::CopyStr(pszToFill, L"Mutex", c4MaxChars);
    else if (eType == tCIDLib::ENamedRscTypes::Semaphore)
        TRawStr::CopyStr(pszToFill, L"Semaphore", c4MaxChars);
    else
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcRsc_InvalidType);
        return kCIDLib::False;
    }

    TRawStr::CatStr(pszToFill, L".", c4MaxChars);

    if (TRawStr::c4StrLen(pszToFill) == c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
        return kCIDLib::False;
    }

    //
    //  If there is a process id to format in, then we need to format it
    //  to a hex value and cat it on.
    //
    if (__pidOfName != kCIDLib::pidInvalid)
    {
        tCIDLib::TZStr64 szTmp;
        TRawStr::bFormatVal(tCIDLib::TCard4(__pidOfName), szTmp, 64, tCIDLib::ERadices::Hex);
        TRawStr::CatStr(pszToFill, szTmp, c4MaxChars);
        TRawStr::CatStr(pszToFill, L".", c4MaxChars);
        if (TRawStr::c4StrLen(pszToFill) == c4MaxChars)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
            return kCIDLib::False;
        }
    }

    // Now put on the three name parts
    TRawStr::CatStr(pszToFill, __pszCompany, c4MaxChars);
    TRawStr::CatStr(pszToFill, L".", c4MaxChars);
    if (TRawStr::c4StrLen(pszToFill) == c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
        return kCIDLib::False;
    }

    TRawStr::CatStr(pszToFill, __pszSubsystem, c4MaxChars);
    TRawStr::CatStr(pszToFill, L".", c4MaxChars);
    if (TRawStr::c4StrLen(pszToFill) == c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
        return kCIDLib::False;
    }

    TRawStr::CatStr(pszToFill, __pszResource, c4MaxChars);
    if (TRawStr::c4StrLen(pszToFill) == c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}
