//
// FILE NAME: CIDKernel_ResourceName_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 01/02/1999
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
    if (!bIsValid())
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
    if (m_pidOfName != kCIDLib::pidInvalid)
    {
        tCIDLib::TZStr64 szTmp;
        TRawStr::bFormatVal(tCIDLib::TCard4(m_pidOfName), szTmp, 64, tCIDLib::ERadices::Hex);
        TRawStr::CatStr(pszToFill, szTmp, c4MaxChars);
        TRawStr::CatStr(pszToFill, L".", c4MaxChars);
        if (TRawStr::c4StrLen(pszToFill) == c4MaxChars)
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
            return kCIDLib::False;
        }
    }

    // Now put on the three name parts
    TRawStr::CatStr(pszToFill, m_pszCompany, c4MaxChars);
    TRawStr::CatStr(pszToFill, L".", c4MaxChars);
    if (TRawStr::c4StrLen(pszToFill) == c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
        return kCIDLib::False;
    }

    TRawStr::CatStr(pszToFill, m_pszSubsystem, c4MaxChars);
    TRawStr::CatStr(pszToFill, L".", c4MaxChars);
    if (TRawStr::c4StrLen(pszToFill) == c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
        return kCIDLib::False;
    }

    TRawStr::CatStr(pszToFill, m_pszResource, c4MaxChars);
    if (TRawStr::c4StrLen(pszToFill) == c4MaxChars)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcData_InsufficientBuffer);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}
