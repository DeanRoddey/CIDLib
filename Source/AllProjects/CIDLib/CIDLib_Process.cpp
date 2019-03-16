//
// FILE NAME: CIDLib_Process.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/31/1998
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
//  This file implements the TProcess namespace. We mostly just pass things
//  on to the kernel level process class.
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
#include    "CIDLib_.hpp"




// ---------------------------------------------------------------------------
//  TProcess functions
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TProcess::bActivateProcess(const TExternalProcess& extpTarget)
{
    // He doesn't throw, it he just returns a status
    return TKrnlProcess::bActivateProcess(extpTarget.pidThis());
}


//
//  Just checks to see if a single instance info resoure exists or not. If
//  we fail we return false. If don't fail, we return true, and set bCreated
//  to whether we created it or found an existing one.
//
//  They can provide an optional external process object to be set up with
//  the found process' information, so that the caller can interact with it.
//  If an error or we don't find the single instance info, then it's not
//  modified.
//
tCIDLib::TBoolean
TProcess::bCheckSingleInstanceInfo( const   TResourceName&          rsnToUse
                                    ,       tCIDLib::TBoolean&      bFound
                                    ,       TExternalProcess* const pextpTarget)
{
    // Get the full resource name to pass in
    const TString strName = rsnToUse.strFullName(tCIDLib::ENamedRscTypes::Memory);
    return TKrnlProcess::bCheckSingleInstanceInfo
    (
        strName.pszBuffer(), bFound, pextpTarget ? &pextpTarget->kextpThis() : 0
    );
}


//
//  We return true if a previous instance was found. If bBringOldForward is
//  true we forced that old one forward if it was found.
//
//  If anything goes wrong, we can either throw or just return true that we
//  did find it, so that another one won't try to start.
//
tCIDLib::TBoolean
TProcess::bSetSingleInstanceInfo(const  TResourceName&      rsnToUse
                                , const tCIDLib::TBoolean   bBringOldForward
                                , const tCIDLib::TBoolean   bThrowIfFail)
{
    // Get the full resource name to pass in
    const TString strName = rsnToUse.strFullName(tCIDLib::ENamedRscTypes::Memory);

    tCIDLib::TBoolean bFoundPrev;
    if (!TKrnlProcess::bSetSingleInstanceInfo(  strName.pszBuffer()
                                                , bBringOldForward
                                                , bFoundPrev))
    {
        if (bThrowIfFail)
        {
            facCIDLib().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcPrc_SetSingleInstInfo
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
        return kCIDLib::True;
    }
    return bFoundPrev;
}


tCIDLib::TVoid TProcess::ExitProcess(const tCIDLib::EExitCodes eExit)
{
    TKrnlProcess::ExitProcess(eExit);
}


TProcessHandle TProcess::hprocThis()
{
    return TKrnlProcess::hprocThis();
}


tCIDLib::TProcessId TProcess::pidThis()
{
    return TKrnlProcess::pidThis();
}


TString TProcess::strProcessName()
{
    return TString(TKrnlSysInfo::pszProcessName());
}


tCIDLib::TVoid TProcess::SetPriorityClass(const tCIDLib::EPrioClasses eClass)
{
    if (!TKrnlProcess::bSetPriorityClass(eClass))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcPrc_SetProcessPriority
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TInteger(tCIDLib::i4EnumOrd(eClass))
        );
    }
}
