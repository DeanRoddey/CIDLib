//
// FILE NAME: CIDKernel_TaskScheduler.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/06/2012
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
//  This file provides the kernel wrapper around local system task
//  scheduling functionality. The functionality is per-platform.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  NAMESPACE: TKrnlTaskSched
// ---------------------------------------------------------------------------
namespace TKrnlTaskSched
{
    enum class EStartTypes
    {
        OnLogon
        , OnBoot
    };

    KRNLEXPORT tCIDLib::TBoolean bCreateSchedTask
    (
        const   tCIDLib::TCh* const     pszName
        , const tCIDLib::TCh* const     pszAuthor
        , const tCIDLib::TCh* const     pszPath
        , const tCIDLib::TCh* const     pszArgs
        , const tCIDLib::TBoolean       bAdminRights
        , const EStartTypes             eStartType
        , const tCIDLib::TCard4         c4DelaySecs
    );


    KRNLEXPORT tCIDLib::TBoolean bDeleteSchedTask
    (
        const   tCIDLib::TCh* const     pszName
    );

    KRNLEXPORT tCIDLib::TBoolean bSupportAvail();
}

