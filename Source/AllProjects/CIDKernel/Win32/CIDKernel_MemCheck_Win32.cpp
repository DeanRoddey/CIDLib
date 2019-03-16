//
// FILE NAME: CIDKernel_MemCheck_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/13/1997
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
//  This file provides the Win32 specific implementation of the TKrnlMemCheck
//  class, which is a simple abstraction for the host development environment
//  or OS heap debugging services.
//
//  We completely remove the contents of this file and of the header file
//  when not in debug mode, to insure we've not left any references anywhere
//  in production code.
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
#include    <crtdbg.h>


// AFTER the header above, since this is defined in there
#if CID_DEBUG_ON



// ---------------------------------------------------------------------------
//   CLASS: TKrnlMemCheck
//  PREFIX: kmchk
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlMemCheck: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlMemCheck::TKrnlMemCheck() :

    m_pSnapShot(0)
{
    _CrtSetDbgFlag
    (
        _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)
        | _CRTDBG_CHECK_EVERY_128_DF | _CRTDBG_CHECK_CRT_DF
    );

    // Allocate a memory state structure and take a snapshot
    m_pSnapShot = new _CrtMemState;
    _CrtMemCheckpoint((_CrtMemState*)m_pSnapShot);
}

TKrnlMemCheck::~TKrnlMemCheck()
{
    // Clean up the snapshot buffer
    delete m_pSnapShot;
}


// ---------------------------------------------------------------------------
//  TKrnlMemCheck: Public, static methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid TKrnlMemCheck::DumpLeaks()
{
    _CrtDumpMemoryLeaks();
}


tCIDLib::TVoid TKrnlMemCheck::ReportToFile(const tCIDLib::TCh* const pszFile)
{
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);

    HANDLE hflDump = ::CreateFile
    (
        pszFile
        , GENERIC_WRITE
        , 0
        , 0
        , OPEN_ALWAYS
        , FILE_ATTRIBUTE_NORMAL
          | FILE_FLAG_SEQUENTIAL_SCAN
        , 0
    );

    _CrtSetReportFile(_CRT_WARN, hflDump);
    _CrtSetReportFile(_CRT_ERROR, hflDump);
}


tCIDLib::TVoid TKrnlMemCheck::ReportToStdOut()
{
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
}


//
//  Set the level of heap checking. We can also turn on the non-freeing
//  heap, so it keeps old blocks around and checks for accesses to them,
//  instead of letting them go back to the heap.
//

tCIDLib::TVoid
TKrnlMemCheck::SetHeapCheckLevel(const  tCIDLib::EHeapChkLevels eToSet
                                , const tCIDLib::TBoolean       bNonFreeing)
{
    //
    //  Get the old flags and clear out the high 16 bits and the other
    //  bits we control here. We'll set any thing need to be turned back
    //  on.
    //
    int iFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    iFlags &= 0xFFFF;
    iFlags &= ~_CRTDBG_DELAY_FREE_MEM_DF;
    iFlags &= ~_CRTDBG_CHECK_ALWAYS_DF;

    if (eToSet == tCIDLib::EHeapChkLevels::Off)
        iFlags |= _CRTDBG_CHECK_DEFAULT_DF;
    else if (eToSet == tCIDLib::EHeapChkLevels::Low)
        iFlags |= _CRTDBG_CHECK_EVERY_1024_DF;
    else if (eToSet == tCIDLib::EHeapChkLevels::Medium)
        iFlags |= _CRTDBG_CHECK_EVERY_128_DF;
    else if (eToSet == tCIDLib::EHeapChkLevels::High)
        iFlags |= _CRTDBG_CHECK_EVERY_16_DF;
    else
        iFlags |= _CRTDBG_CHECK_ALWAYS_DF;

    if (bNonFreeing)
        iFlags |= _CRTDBG_DELAY_FREE_MEM_DF;

    _CrtSetDbgFlag(iFlags);
}


tCIDLib::TVoid TKrnlMemCheck::ValidateHeap()
{
    _CrtCheckMemory();
}



// ---------------------------------------------------------------------------
//  TKrnlMemCheck: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TKrnlMemCheck::DumpSnapDiffs()
{
    // Take another snap shot now
    _CrtMemState SnapShot;
    _CrtMemCheckpoint(&SnapShot);

    // If there are differences, dump them out
    _CrtMemState Diffs;
    if (_CrtMemDifference(&Diffs, (_CrtMemState*)m_pSnapShot, &SnapShot))
        _CrtMemDumpStatistics(&Diffs);

    _CrtMemDumpAllObjectsSince((_CrtMemState*)m_pSnapShot);

    // Copy the new snapshot to make it the current one
    memcpy(m_pSnapShot, &SnapShot, sizeof(_CrtMemState));
}


tCIDLib::TVoid TKrnlMemCheck::TakeSnapshot()
{
    // Replace any previous snapshot with a new one
    _CrtMemCheckpoint((_CrtMemState*)m_pSnapShot);
}



#endif

