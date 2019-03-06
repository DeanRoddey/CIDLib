//
// FILE NAME: CIDCtrls_WndDevice.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TGraphWndDev class, which is a derivative of
//  the basic device class used to do ad hoc painting on windows.
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
#include    "CIDCtrls_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TGraphWndDev,TGraphDrawDev)
RTTIDecls(TGraphDesktopDev,TGraphWndDev)



// ---------------------------------------------------------------------------
//  CLASS: TGraphWndDev
// PREFIX: gdev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGraphWndDev: Constructors and destructor
// ---------------------------------------------------------------------------
TGraphWndDev::TGraphWndDev( const   tCIDGraphDev::TDeviceHandle hdevPaint
                            , const tCIDLib::EAdoptOpts         eAdopt) :

    TGraphDrawDev(hdevPaint, eAdopt)
{
}

TGraphWndDev::TGraphWndDev(const TWindow& wndSource)
{
//    tCIDCtrls::TDeviceHandle hdevTmp = ::GetWindowDC(wndSource.hwndThis());
    tCIDGraphDev::TDeviceHandle hdevTmp = ::GetDC(wndSource.hwndSafe());
    if (!hdevTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CreateWndDev
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Store the handle
    SetHandle(hdevTmp, tCIDLib::EAdoptOpts::Adopt);
}

TGraphWndDev::~TGraphWndDev()
{
    if (hdevThis() != kCIDGraphDev::hdevInvalid)
    {
        if (eAdopted() == tCIDLib::EAdoptOpts::Adopt)
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
}


// ---------------------------------------------------------------------------
//  TGraphWndDev: Protected Constructors
// ---------------------------------------------------------------------------
TGraphWndDev::TGraphWndDev()
{
}



// ---------------------------------------------------------------------------
//  CLASS: TGraphDesktopDev
// PREFIX: gdev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGraphDesktopDev: Constructors and destructor
// ---------------------------------------------------------------------------
TGraphDesktopDev::TGraphDesktopDev()
{
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

    // Store the handle
    SetHandle(hdevTmp, tCIDLib::EAdoptOpts::Adopt);
}


TGraphDesktopDev::~TGraphDesktopDev()
{
}

