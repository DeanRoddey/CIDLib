//
// FILE NAME: CIDKernel_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/13/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file contains the Win32 specific parts of the main module,
//  CIDKernel.Cpp. In this case we provide the module init for the main
//  module which just does any process wide fundamental setup.
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
//  TCIDKrnlModule: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCIDKrnlModule::bInitTermCore(const tCIDLib::EInitTerm eState)
{
    if (eState == tCIDLib::EInitTerm::Initialize)
    {
        // Disable system popups
        ::SetErrorMode
        (
            SEM_FAILCRITICALERRORS
            | SEM_NOGPFAULTERRORBOX
            | SEM_NOOPENFILEERRORBOX
        );


        #if CID_DEBUG_MEM
        //
        //  If memory debugging is on, then crank up the heap check frequency
        //  by default.
        //
        _CrtSetDbgFlag
        (
            (_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) & 0x0000FFFF)
            | _CRTDBG_CHECK_EVERY_1024_DF
        );


//        _CrtSetDbgFlag
//        (
//            _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)
//            | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_DELAY_FREE_MEM_DF
//        );
        #endif
    }
     else
    {
        // Nothing to do at this time
    }
    return kCIDLib::True;
}
