//
// FILE NAME: CIDGraphDev_WndDevice.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/24/2017
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TIntGraphWndDev class, which is a derative of
//  the basic device class used in window paint events.
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
#include    "CIDGraphDev_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TIntGraphWndDev,TGraphDrawDev)



// ---------------------------------------------------------------------------
//  CLASS: TIntGraphWndDev
// PREFIX: gdev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TIntGraphWndDev: Constructors and Destructor
// ---------------------------------------------------------------------------
TIntGraphWndDev::TIntGraphWndDev()
{
    // Create a compatible device based on the desktop
    tCIDGraphDev::TDeviceHandle hdevTmp = ::GetWindowDC(::GetDesktopWindow());
    if (!hdevTmp)
    {
        TKrnlError kerrToLog;
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CreateWndDev
            , kerrToLog
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    //
    //  And set the handle on our parent, remembering this one is adopted though we
    //  don't really need that since we always adopt ours. But we should make it clear
    //  by setting the flag right.
    //
    SetHandle(hdevTmp, tCIDLib::EAdoptOpts::Adopt);
}

TIntGraphWndDev::~TIntGraphWndDev()
{
    // WE always adopt the handles, so if it's valid, release it
    if (hdevThis() != kCIDGraphDev::hdevInvalid)
    {
        // Get the window associated with this device
        tCIDCtrls::TWndHandle hwndDev = ::WindowFromDC(hdevThis());
        if (!hwndDev)
        {
            TKrnlError kerrToLog;
            facCIDGraphDev().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_WndFromDev
                , kerrToLog
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::AppStatus
            );
        }

        // And release the DC
        if (!::ReleaseDC(hwndDev, hdevThis()))
        {
            TKrnlError kerrToLog;
            facCIDGraphDev().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_DestroyWndDev
                , kerrToLog
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::AppStatus
            );
        }
    }
}

