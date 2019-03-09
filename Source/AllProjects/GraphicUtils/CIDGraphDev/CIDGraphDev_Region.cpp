//
// FILE NAME: CIDGraphDev_Region.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/25/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the region class
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
RTTIDecls(TGUIRegion,TObject)


// ---------------------------------------------------------------------------
//  CLASS: TGUIRegion
// PREFIX: grgn
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGUIRegion: Constructors and Destructor
// ---------------------------------------------------------------------------
TGUIRegion::TGUIRegion() :

    m_c4Rounding(0)
    , m_hrgnThis(kCIDGraphDev::hrgnInvalid)
{
    Set(TArea::areaEmpty);
}

TGUIRegion::TGUIRegion(const TArea& areaRegion) :

    m_c4Rounding(0)
    , m_hrgnThis(kCIDGraphDev::hrgnInvalid)
{
    Set(areaRegion);
}

TGUIRegion::TGUIRegion( const   TArea&          areaRegion
                        , const tCIDLib::TCard4 c4Rounding) :

    m_c4Rounding(c4Rounding)
    , m_hrgnThis(kCIDGraphDev::hrgnInvalid)
{
    Set(areaRegion, c4Rounding);
}

TGUIRegion::TGUIRegion(const TGUIRegion& grgnSrc) :

    m_c4Rounding(grgnSrc.m_c4Rounding)
    , m_hrgnThis(kCIDGraphDev::hrgnInvalid)
{
    m_hrgnThis = hrgnMakeCopy(grgnSrc.m_hrgnThis);
}


TGUIRegion::~TGUIRegion()
{
    if (m_hrgnThis != kCIDGraphDev::hrgnInvalid)
        ::DeleteObject(m_hrgnThis);
}


// ---------------------------------------------------------------------------
//  TGUIRegion: Public operators
// ---------------------------------------------------------------------------
TGUIRegion& TGUIRegion::operator=(const TGUIRegion& grgnSrc)
{
    if (this != &grgnSrc)
    {
        // Try to copy the source
        tCIDGraphDev::TRegionHandle hrgnNew = hrgnMakeCopy(grgnSrc.m_hrgnThis);

        // Get rid of the current region, if any
        if (m_hrgnThis != kCIDGraphDev::hrgnInvalid)
        {
            ::DeleteObject(m_hrgnThis);
            m_hrgnThis = kCIDGraphDev::hrgnInvalid;
        }

        // Now store the new one
        m_c4Rounding = grgnSrc.m_c4Rounding;
        m_hrgnThis = hrgnNew;
    }
    return *this;
}



// ---------------------------------------------------------------------------
//  TGUIRegion: Public, non-virtual methods
// ---------------------------------------------------------------------------
TArea TGUIRegion::areaBounds() const
{
    RECT rectLog;
    if (!::GetRgnBox(m_hrgnThis, &rectLog))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_GetRgnBounds
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    //
    //  Convert to an area for return. Indicate that it is non-inclusive,
    //  which will make it come out correctly.
    //
    TArea areaRet;
    areaRet.FromRectl
    (
        *reinterpret_cast<tCIDLib::THostRectl*>(&rectLog)
        , tCIDLib::ERectlTypes::NonInclusive
    );
    return areaRet;
}


tCIDLib::TBoolean TGUIRegion::bContainsPnt(const TPoint& pntToTest) const
{
    if (::PtInRegion(m_hrgnThis, pntToTest.i4X(), pntToTest.i4Y()))
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean TGUIRegion::bIntersects(const TArea& areaToTest) const
{
    //
    //  The function does not check the l/r edges, so say our area is
    //  inclusive, so that the rectl will be one larger. This will make
    //  it check the whole area.
    //
    tCIDLib::THostRectl rectToTest;
    areaToTest.ToRectl(rectToTest, tCIDLib::ERectlTypes::Inclusive);

    if (::RectInRegion(m_hrgnThis, reinterpret_cast<RECT*>(&rectToTest)))
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean TGUIRegion::bIsEmpty() const
{
    RECT rectBox;
    return (::GetRgnBox(m_hrgnThis, &rectBox) == NULLREGION);
}


tCIDLib::TBoolean TGUIRegion::bSetFromDev(TGraphDrawDev& gdevSrc)
{
    // Create an empty temp region
    tCIDGraphDev::TRegionHandle hrgnTmp = ::CreateRectRgn(0, 0, 0, 0);
    if (hrgnTmp == kCIDGraphDev::hrgnInvalid)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CreateRegion
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Try to get the current clip
    int iRes = ::GetClipRgn(gdevSrc.hdevThis(), hrgnTmp);
    if (iRes == -1)
    {
        // Clean up the region since we aren't going to set it
        ::DeleteObject(hrgnTmp);
        hrgnTmp = kCIDGraphDev::hrgnInvalid;

        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_GetClipRegion
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // If we have one already, then get rid of it
    if (m_hrgnThis != kCIDGraphDev::hrgnInvalid)
    {
        ::DeleteObject(m_hrgnThis);
        m_hrgnThis = kCIDGraphDev::hrgnInvalid;
    }

    // And store the new one
    m_hrgnThis = hrgnTmp;

    // If there wasn't a clip region, then we stored our new empty one
    return (iRes != 0);
}


// Return the rounding for this region
tCIDLib::TCard4 TGUIRegion::c4Rounding() const
{
    return m_c4Rounding;
}


tCIDLib::TVoid
TGUIRegion::CombineWith(const   TArea&                      areaWith
                        , const tCIDGraphDev::EClipModes    eMode)
{
    // Just make a temp region for the area, and call the other one
    TGUIRegion grgnTmp(areaWith);
    CombineWith(grgnTmp, eMode);
}

tCIDLib::TVoid
TGUIRegion::CombineWith(const   TGUIRegion&                 grgnWith
                        , const tCIDGraphDev::EClipModes    eMode)
{
    const tCIDLib::TCard4 c4Mode = TGrDevPlatform::c4XlatClipMode(eMode);

    if (::CombineRgn(m_hrgnThis, m_hrgnThis, grgnWith.m_hrgnThis, c4Mode) == ERROR)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CombineRegion
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


// Deflate the size of the region in each axis
tCIDLib::TVoid
TGUIRegion::Deflate(const tCIDLib::TCard4 c4Horz, const tCIDLib::TCard4 c4Vert)
{
    // Query the area, deflate it, and set us back up with a new region
    TArea areaRgn = areaBounds();
    areaRgn.Deflate(c4Horz, c4Vert);
    Set(areaRgn, m_c4Rounding);
}


// Make a copy of our region and return it. The caller is responsible fo rit
tCIDGraphDev::TRegionHandle TGUIRegion::hrgnCopy() const
{
    return hrgnMakeCopy(m_hrgnThis);
}


// Provide access to our raw ergion handle
tCIDGraphDev::TRegionHandle TGUIRegion::hrgnThis() const
{
    return m_hrgnThis;
}


// Inflate the size of the region in each axis
tCIDLib::TVoid
TGUIRegion::Inflate(const tCIDLib::TCard4 c4Horz, const tCIDLib::TCard4 c4Vert)
{
    // Query the area, inflate it, and set us back up with a new region
    TArea areaRgn = areaBounds();
    areaRgn.Inflate(c4Horz, c4Vert);
    Set(areaRgn, m_c4Rounding);
}


// Move this region's origin
tCIDLib::TVoid TGUIRegion::Offset(const TPoint& pntBy)
{
    if (::OffsetRgn(m_hrgnThis, pntBy.i4X(), pntBy.i4Y()) == ERROR)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_OffsetRegion
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , pntBy
        );
    }
}

tCIDLib::TVoid
TGUIRegion::Offset(const tCIDLib::TInt4 i4XOfs, const tCIDLib::TInt4 i4YOfs)
{
    if (::OffsetRgn(m_hrgnThis, i4XOfs, i4YOfs) == ERROR)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_OffsetRegion
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TPoint(i4XOfs, i4YOfs)
        );
    }
}


// Set us to a new area
tCIDLib::TVoid TGUIRegion::Set(const TArea& areaToSet)
{
    //
    //  Create the region. The area is assumed to be exclusive, so the bottom/right
    //  edges aren't included. So we have to bump ours by 1.
    //
    tCIDGraphDev::TRegionHandle hrgnTmp = ::CreateRectRgn
    (
        areaToSet.i4X()
        , areaToSet.i4Y()
        , areaToSet.i4Right() + 1
        , areaToSet.i4Bottom() + 1
    );

    if (!hrgnTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        TTextStringOutStream strmOut(256UL);
        strmOut << L"rectangular region (" << areaToSet << L")"
                << kCIDLib::FlushIt;;

        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CantCreate
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strmOut.strData()
        );
    }


    // If we have one already, then get rid of it
    if (m_hrgnThis != kCIDGraphDev::hrgnInvalid)
    {
        ::DeleteObject(m_hrgnThis);
        m_hrgnThis = kCIDGraphDev::hrgnInvalid;
    }

    // And store the new one
    m_hrgnThis = hrgnTmp;
}



tCIDLib::TVoid TGUIRegion::Set( const   TArea&          areaToSet
                                , const tCIDLib::TCard4 c4Rounding)
{
    //
    //  Create the region. There is a known stupidity in Windows in which the area
    //  of a rounded rect region is not the same as a non-rounded. It ends up being
    //  doubly exclusive, so we have to add 2 to get the same results, where we only
    //  add 1 for the non-rounded.
    //
    tCIDGraphDev::TRegionHandle hrgnTmp = ::CreateRoundRectRgn
    (
        areaToSet.i4X()
        , areaToSet.i4Y()
        , areaToSet.i4Right() + 2
        , areaToSet.i4Bottom() + 2
        , c4Rounding
        , c4Rounding
    );

    //
    //  This can cause an issue in some pathological cases. So we only throw if in
    //  debug mode. In production mode, we will log instead and just create an empty
    //  region and return that. Otherwise it could create a situation where they just
    //  get caught in a loop of error popups.
    //
    if (!hrgnTmp)
    {
        TString strArea(areaToSet);
        strArea.Prepend(L"rect region at ");
        strArea.Append(L", rnd=");
        strArea.AppendFormatted(c4Rounding);

        #if CID_DEBUG_ON
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CantCreate
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strArea
        );
        #else
        facCIDGraphDev().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CantCreate
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strArea
        );
        hrgnTmp = ::CreateRectRgn(0, 0, 0, 0);
        #endif
    }

    // If we have one already, then get rid of it
    if (m_hrgnThis != kCIDGraphDev::hrgnInvalid)
    {
        ::DeleteObject(m_hrgnThis);
        m_hrgnThis = kCIDGraphDev::hrgnInvalid;
    }

    // And store the new one
    m_c4Rounding = c4Rounding;
    m_hrgnThis = hrgnTmp;
}


tCIDLib::TVoid
TGUIRegion::SetOnDevice(        TGraphDrawDev&              gdevTarget
                        , const tCIDGraphDev::EClipModes    eMode)
{
    //
    //  This guy does NOT steal our region, he just makes a copy of it, so
    //  we can keep our handle.
    //
    const tCIDLib::TCard4 c4Mode = TGrDevPlatform::c4XlatClipMode(eMode);
    if (::ExtSelectClipRgn(gdevTarget.hdevThis(), m_hrgnThis, c4Mode) == ERROR)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_Select
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"Region")
        );
    }
}


// ---------------------------------------------------------------------------
//  TGUIRegion: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDGraphDev::TRegionHandle
TGUIRegion::hrgnMakeCopy(const tCIDGraphDev::TRegionHandle hrgnSrc) const
{
    // Get the region data and create a new region from it
    const tCIDLib::TCard4 c4DataSz = ::GetRegionData(hrgnSrc, 0, 0);
    tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[c4DataSz];
    TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Buf);
    if (!::GetRegionData(hrgnSrc
                        , c4DataSz
                        , reinterpret_cast<RGNDATA*>(pc1Buf)))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_QueryRgnData
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // And try to create a region from it
    HRGN hrgnNew = ::ExtCreateRegion
    (
        0, c4DataSz, reinterpret_cast<RGNDATA*>(pc1Buf)
    );

    if (hrgnNew == kCIDGraphDev::hrgnInvalid)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CreateRegion
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return hrgnNew;
}




// ---------------------------------------------------------------------------
//  CLASS: TRegionJanitor
// PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TRegionJanitor: Constructors and Destructor
// ---------------------------------------------------------------------------
TRegionJanitor::TRegionJanitor(         TGraphDrawDev* const        pgdevTarget
                                ,       TGUIRegion&                 grgnToSet
                                , const tCIDGraphDev::EClipModes    eMode
                                , const tCIDLib::TBoolean           bClip) :

    m_hrgnOld(kCIDGraphDev::hrgnInvalid)
    , m_pgdevTarget(pgdevTarget)
{
    if (bClip)
        m_hrgnOld = pgdevTarget->hrgnSetClipRegion(eMode, grgnToSet);
    else
        m_pgdevTarget = nullptr;
}

TRegionJanitor::TRegionJanitor(         TGraphDrawDev* const        pgdevTarget
                                , const TArea&                      areaRegion
                                , const tCIDGraphDev::EClipModes    eMode
                                , const tCIDLib::TBoolean           bClip) :

    m_hrgnOld(kCIDGraphDev::hrgnInvalid)
    , m_pgdevTarget(pgdevTarget)
{
    if (bClip)
        m_hrgnOld = pgdevTarget->hrgnSetClipArea(eMode, areaRegion, 0);
    else
        m_pgdevTarget = nullptr;
}


TRegionJanitor::TRegionJanitor(         TGraphDrawDev* const        pgdevTarget
                                , const TArea&                      areaRegion
                                , const tCIDGraphDev::EClipModes    eMode
                                , const tCIDLib::TCard4             c4Rounding
                                , const tCIDLib::TBoolean           bClip) :

    m_hrgnOld(kCIDGraphDev::hrgnInvalid)
    , m_pgdevTarget(pgdevTarget)
{
    if (bClip)
        m_hrgnOld = pgdevTarget->hrgnSetClipArea(eMode, areaRegion, c4Rounding);
    else
        m_pgdevTarget = nullptr;
}


TRegionJanitor::~TRegionJanitor()
{
    if (m_pgdevTarget)
    {
        //
        //  The handle may be null. If there was no previous clipping region, then the
        //  set clip calls above may return null, in which case that's what we want to
        //  put back since that removes the clipping region.
        //
        try
        {
            m_pgdevTarget->PopClipArea(m_hrgnOld);
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);

            // Fall through so we get the old regions cleaned up
        }

        // He copies the region so we have to delete ours
        if (m_hrgnOld != kCIDGraphDev::hrgnInvalid)
            ::DeleteObject(m_hrgnOld);
    }
}

