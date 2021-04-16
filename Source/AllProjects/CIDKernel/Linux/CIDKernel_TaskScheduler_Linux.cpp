//
// FILE NAME: CIDKernel_TaskSched_Linux.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/12/2019
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
//  This file provides the Linux specific implementation of the the
//  TKrnlTaskSched namespace.
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
#include    "CIDKernel_InternalHelpers_.hpp"



// ---------------------------------------------------------------------------
//  TKrnlTaskSched functions
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TKrnlTaskSched::bCreateSchedTask(const  tCIDLib::TCh* const pszName
                                , const tCIDLib::TCh* const pszAuthor
                                , const tCIDLib::TCh* const pszPath
                                , const tCIDLib::TCh* const pszArgs
                                , const tCIDLib::TBoolean   bAdminRights
                                , const EStartTypes         eStartType
                                , const tCIDLib::TCard4     c4DelaySecs)
{
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlTaskSched::bDeleteSchedTask(const tCIDLib::TCh* const pszName)
{
    TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


//
//  See if support is available. We just try to creat the task service.
//
tCIDLib::TBoolean TKrnlTaskSched::bSupportAvail()
{
    return kCIDLib::False;
}

