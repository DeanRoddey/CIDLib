//
// FILE NAME: CIDGraphDev_PaintDevice.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
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
//  This file implements the TGraphPaintDev class, which is a derative of
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
RTTIDecls(TGraphPaintDev,TGraphDrawDev)



// ---------------------------------------------------------------------------
//  CLASS: TGraphPaintDev
// PREFIX: gdev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGraphPaintDev: Constructors and Destructor
// ---------------------------------------------------------------------------
TGraphPaintDev::TGraphPaintDev( const   tCIDGraphDev::TDeviceHandle hdevToUse
                                , const THostPaintInfo&             hpiToUse) :

    TGraphDrawDev(hdevToUse, tCIDLib::EAdoptOpts::Adopt)
    , m_phpiThis(nullptr)
{
    // Duplication the paint information
    m_phpiThis = new THostPaintInfo;
    *m_phpiThis = hpiToUse;
}

TGraphPaintDev::~TGraphPaintDev()
{
    if ((eAdopted() == tCIDLib::EAdoptOpts::Adopt)
    &&  (hdevThis() != kCIDGraphDev::hdevInvalid))
    {
        // Get the window associated with this paint DC
        tCIDCtrls::TWndHandle hwndDev = ::WindowFromDC(hdevThis());
        if (!hwndDev)
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_GetWindow
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::CantDo
            );
        }

        // And pass it to the end paint
        if (!::EndPaint(hwndDev, m_phpiThis))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_EndPaint
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
    delete m_phpiThis;
}

