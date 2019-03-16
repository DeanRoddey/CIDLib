//
// FILE NAME: CIDGraphDev_InfoDevice.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
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
//  This file implements the TGraphInfoDeve class, which is a derivative of
//  the basic graphics device class which is used for quering information
//  from a device.
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
RTTIDecls(TGraphInfoDev,TGraphInfoDevice)



// ---------------------------------------------------------------------------
//  CLASS: TGraphInfoDevice
// PREFIX: gdev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGraphInfoDevice: Constructors and Destructor
// ---------------------------------------------------------------------------
TGraphInfoDev::TGraphInfoDev(   const   TString&    strDriverName
                                , const TString&    strDeviceName)
{
    tCIDGraphDev::TDeviceHandle hdevTmp = ::CreateICW
    (
        strDriverName.pszBuffer()
        , strDeviceName.pszBuffer()
        , 0
        , 0
    );

    if (hdevTmp == kCIDGraphDev::hdevInvalid)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CreateInfoDev
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strDriverName
            , strDeviceName
        );
    }

    // Set the handle
    SetHandle(hdevTmp, tCIDLib::EAdoptOpts::Adopt);
}


TGraphInfoDev::~TGraphInfoDev()
{
    if ((eAdopted() == tCIDLib::EAdoptOpts::Adopt)
    &&  (hdevThis() != kCIDGraphDev::hdevInvalid))
    {
        if (!::DeleteDC(hdevThis()))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_DestroyInfoDev
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}
