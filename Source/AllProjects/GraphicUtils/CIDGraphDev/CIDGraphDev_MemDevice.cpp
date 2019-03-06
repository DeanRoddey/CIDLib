//
// FILE NAME: CIDGraphDev_MemDevice.cpp
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
//  This file implements the TGraphMemDev class, which is a derivative of
//  the basic device class used to paint to memory.
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
RTTIDecls(TGraphMemDev,TGraphDrawDev)


// ---------------------------------------------------------------------------
//  CLASS: TGraphMemDev
// PREFIX: gdev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGraphMemDev: Constructors and Destructor
// ---------------------------------------------------------------------------
TGraphMemDev::TGraphMemDev( const   TGraphDrawDev&  gdevCompatible
                            , const TBitmap&        bmpSurface) :

    m_hbmpOld(kCIDGraphDev::hbmpInvalid)
    , m_bmpSurface(bmpSurface)
{
    tCIDGraphDev::TDeviceHandle hdevTmp = ::CreateCompatibleDC(gdevCompatible.hdevThis());
    if (!hdevTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CreateMemDev
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    //
    //  Try to select the bitmap into the device. We save the original for
    //  later.
    //
    m_hbmpOld = reinterpret_cast<HBITMAP__*>(::SelectObject(hdevTmp, bmpSurface.hbmpThis()));
    if (!m_hbmpOld)
    {
        ::DeleteDC(hdevTmp);

        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CreateMemDev
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Store the handle
    SetHandle(hdevTmp, tCIDLib::EAdoptOpts::Adopt);
}


TGraphMemDev::TGraphMemDev( const   TGraphDrawDev&  gdevCompatible
                            , const TSize&          szBitmap) :

    m_hbmpOld(kCIDGraphDev::hbmpInvalid)
    , m_bmpSurface(gdevCompatible, szBitmap)
{
    tCIDGraphDev::TDeviceHandle hdevTmp = ::CreateCompatibleDC(gdevCompatible.hdevThis());
    if (!hdevTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CreateMemDev
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    //
    //  Try to select the bitmap into the device. We save the original for
    //  later.
    //
    m_hbmpOld = reinterpret_cast<HBITMAP__*>(::SelectObject(hdevTmp, m_bmpSurface.hbmpThis()));
    if (!m_hbmpOld)
    {
        ::DeleteDC(hdevTmp);

        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CreateMemDev
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Store the handle
    SetHandle(hdevTmp, tCIDLib::EAdoptOpts::Adopt);
}


TGraphMemDev::TGraphMemDev( const   TSize&                  szMax
                            , const tCIDImage::EBitDepths   eDepth
                            , const tCIDImage::EPixFmts     eFormat) :

    m_hbmpOld(kCIDGraphDev::hbmpInvalid)
    , m_bmpSurface(szMax, eFormat, eDepth, kCIDLib::True)
{
    tCIDGraphDev::TDeviceHandle hdevTmp = ::CreateCompatibleDC(0);
    if (!hdevTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CreateMemDev
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    //
    //  Try to select the bitmap into the device. We save the original for
    //  later.
    //
    m_hbmpOld = reinterpret_cast<HBITMAP__*>(::SelectObject(hdevTmp, m_bmpSurface.hbmpThis()));
    if (!m_hbmpOld)
    {
        ::DeleteDC(hdevTmp);

        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CreateMemDev
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Store the handle
    SetHandle(hdevTmp, tCIDLib::EAdoptOpts::Adopt);
}


TGraphMemDev::~TGraphMemDev()
{
    tCIDGraphDev::TDeviceHandle hdevTarget = hdevThis();
    if (hdevTarget != kCIDGraphDev::hdevInvalid)
    {
        // Put back the original bitmap first, to deselect ours
        if (!::SelectObject(hdevTarget, m_hbmpOld))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDGraphDev().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_DeselBitmap
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::CantDo
            );
        }

        // And now we can delete our device handle
        if (!::DeleteDC(hdevTarget))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDGraphDev().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_DestroyMemDev
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


// ---------------------------------------------------------------------------
//  TGraphMemDev: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TBitmap& TGraphMemDev::bmpCurrent() const
{
    return m_bmpSurface;
}


tCIDLib::TVoid
TGraphMemDev::Resize(const TGraphDrawDev& gdevCompatible, const TSize& szNew)
{
    tCIDGraphDev::TDeviceHandle hdevTarget = hdevThis();
    if (hdevTarget != kCIDGraphDev::hdevInvalid)
    {
        //
        //  Release the current bitmap by restoring the original. Our bitmap
        //  object owns this guy, so it's not lost.
        //
        if (!::SelectObject(hdevTarget, m_hbmpOld))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_DeselBitmap
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::CantDo
            );
        }
        m_hbmpOld = kCIDGraphDev::hbmpInvalid;

        //
        //  Now, resize the bitmap object by assigning a new bitmap to it, of
        //  the new size.
        //
        m_bmpSurface = TBitmap(gdevCompatible, szNew);

        // And now reselect this new bitmap into the device
        m_hbmpOld = reinterpret_cast<HBITMAP__*>
        (
            ::SelectObject(hdevTarget, m_bmpSurface.hbmpThis())
        );

        if (!m_hbmpOld)
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_CreateMemDev
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}

