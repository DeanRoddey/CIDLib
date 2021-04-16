//
// FILE NAME: CIDGraphDev_Brush.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/24/2002
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
//  This file implements the various brush classes
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
RTTIDecls(TGUIBrush,TObject)
RTTIDecls(TSolidBrush,TGUIBrush)
RTTIDecls(THatchedBrush,TGUIBrush)
RTTIDecls(TBitmapBrush,TGUIBrush)
RTTIDecls(TNullBrush,TGUIBrush)


// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
static tCIDGraphDev::TBrushHandle hbrCreateNewBrush(const LOGBRUSH& Brush)
{
    tCIDGraphDev::TBrushHandle hbrRet = ::CreateBrushIndirect(&Brush);
    if (!hbrRet)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CantCreate
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"Brush")
        );
    }
    return hbrRet;
}



// ---------------------------------------------------------------------------
//  CLASS: TGUIBrush
// PREFIX: gbr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGUIBrush: Public, static methods
// ---------------------------------------------------------------------------
TGUIBrush& TGUIBrush::Nul_TGUIBrush()
{
    static TSolidBrush gbrNull(TRGBClr(0, 0, 0));
    return gbrNull;
}


// ---------------------------------------------------------------------------
//  TGUIBrush: Constructors and Destructor
// ---------------------------------------------------------------------------
TGUIBrush::~TGUIBrush()
{
    try
    {
        Reset();
    }

    catch(TError& errToCatch)
    {
        // Can't throw out of here, so log it
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  TGUIBrush: Publish, non-virtual methods
// ---------------------------------------------------------------------------
tCIDGraphDev::TBrushHandle TGUIBrush::hbrThis() const
{
    // If we've not created the brush yet, then do it
    if (m_hbrThis == kCIDGraphDev::hbrInvalid)
        m_hbrThis = hbrCreateBrush();
    return m_hbrThis;
}


tCIDGraphDev::TBrushHandle
TGUIBrush::hbrSetOnDevice(TGraphDrawDev& gdevTarget) const
{
    //
    //  If we are currently selected into a device, then this is an illegal
    //  double selection.
    //
    if (m_bSetOnDevice)
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_AlreadySelected
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TString(L"Brush")
        );
    }

    // If we've not created the brush yet, then do it
    if (m_hbrThis == kCIDGraphDev::hbrInvalid)
        m_hbrThis = hbrCreateBrush();

    tCIDGraphDev::TBrushHandle hRet = reinterpret_cast<tCIDGraphDev::TBrushHandle>
    (
        ::SelectObject(gdevTarget.hdevThis(), m_hbrThis)
    );

    if (!hRet)
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
            , TString(L"Brush")
        );
    }

    // It worked, so mark us as selected
    m_bSetOnDevice = kCIDLib::True;

    return hRet;
}

tCIDLib::TVoid
TGUIBrush::Unset(       TGraphDrawDev&              gdevTarget
                , const tCIDGraphDev::TBrushHandle  hbrToPutBack) const
{
    // If we are not set on a device, then this is a bogus call
    if (!m_bSetOnDevice)
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_NotSelected
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TString(L"Brush")
        );
    }

    // Looks ok, so select the old one back
    tCIDGraphDev::TBrushHandle hbrCur = reinterpret_cast<tCIDGraphDev::TBrushHandle>
    (
        ::SelectObject(gdevTarget.hdevThis(), hbrToPutBack)
    );

    if (!hbrCur)
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
            , TString(L"Brush")
        );
    }

    // Make sure the old one is us
    if (hbrCur != m_hbrThis)
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_DeselectWrongObj
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TString(L"Brush")
        );
    }

    // Ok, went ok, so lets mark us as not selected
    m_bSetOnDevice = kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TGUIBrush: Hidden constructors and operators
// ---------------------------------------------------------------------------
TGUIBrush::TGUIBrush() :

    m_bSetOnDevice(kCIDLib::False)
    , m_hbrThis(kCIDGraphDev::hbrInvalid)
{
}

//
//  Note that we don't copy anything at this level. We are really just
//  concerned with dup'ing the derived class' attributes. But we want to
//  create our own brush.
//
TGUIBrush::TGUIBrush(const TGUIBrush&) :

    m_bSetOnDevice(kCIDLib::False)
    , m_hbrThis(kCIDGraphDev::hbrInvalid)
{
}


//
//  For assignment, we get rid of our current brush so we'll have to recreate
//  it again using the derived class' attributes. We cannot be selected into a
//  device at this point.
//
tCIDLib::TVoid TGUIBrush::operator=(const TGUIBrush&)
{
    Reset();
}


// ---------------------------------------------------------------------------
//  TGUIBrush: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TGUIBrush::Reset()
{
    // Can't do this while set on a device
    if (m_bSetOnDevice)
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_AlreadySelected
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TString(L"Brush")
        );
    }

    if (m_hbrThis != kCIDGraphDev::hbrInvalid)
    {
        ::DeleteObject(m_hbrThis);
        m_hbrThis = kCIDGraphDev::hbrInvalid;
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TSolidBrush
// PREFIX: gbr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSolidBrush: Constructors and Destructor
// ---------------------------------------------------------------------------
TSolidBrush::TSolidBrush() :

    m_rgbClr(0, 0, 0)
{
}

TSolidBrush::TSolidBrush(const TRGBClr& rgbClr) :

    m_rgbClr(rgbClr)
{
}

TSolidBrush::TSolidBrush(const TSolidBrush& gbrSrc) :

    TGUIBrush(gbrSrc)
    , m_rgbClr(gbrSrc.m_rgbClr)
{
}

TSolidBrush::~TSolidBrush()
{
}


// ---------------------------------------------------------------------------
//  TSolidBrush: Public operators
// ---------------------------------------------------------------------------
TSolidBrush& TSolidBrush::operator=(const TSolidBrush& gbrSrc)
{
    if (this != &gbrSrc)
    {
        // Do our parent first. If we are selected, he will throw
        TParent::operator=(gbrSrc);
        m_rgbClr = gbrSrc.m_rgbClr;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TSolidBrush: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TRGBClr& TSolidBrush::rgbColor() const
{
    return m_rgbClr;
}

tCIDLib::TVoid TSolidBrush::SetColor(const TRGBClr& rgbToSet)
{
    Reset();
    m_rgbClr = rgbToSet;
}


// ---------------------------------------------------------------------------
//  TSolidBrush: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDGraphDev::TBrushHandle TSolidBrush::hbrCreateBrush() const
{
    // Just set up a structure and create and return the brush
    LOGBRUSH Brush;
    Brush.lbStyle = BS_SOLID;
    Brush.lbColor = RGB(m_rgbClr.c1Red(), m_rgbClr.c1Green(), m_rgbClr.c1Blue());
    Brush.lbHatch = 0;
    return ::hbrCreateNewBrush(Brush);
}



// ---------------------------------------------------------------------------
//  CLASS: THatchedBrush
// PREFIX: gbr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  THatchedBrush: Constructors and Destructor
// ---------------------------------------------------------------------------
THatchedBrush::THatchedBrush(const  TRGBClr&                rgbClr
                            , const tCIDGraphDev::EPatterns ePattern) :
    m_ePattern(ePattern)
    , m_rgbClr(rgbClr)
{
}

THatchedBrush::THatchedBrush(const THatchedBrush& gbrSrc) :

    TGUIBrush(gbrSrc)
    , m_ePattern(gbrSrc.m_ePattern)
    , m_rgbClr(gbrSrc.m_rgbClr)
{
}

THatchedBrush::~THatchedBrush()
{
}


// ---------------------------------------------------------------------------
//  THatchedBrush: Public operators
// ---------------------------------------------------------------------------
THatchedBrush& THatchedBrush::operator=(const THatchedBrush& gbrSrc)
{
    if (this != &gbrSrc)
    {
        // Do our parent first. If we are selected, he will throw
        TParent::operator=(gbrSrc);
        m_ePattern = gbrSrc.m_ePattern;
        m_rgbClr = gbrSrc.m_rgbClr;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  THatchedBrush: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid THatchedBrush::Set(  const   TRGBClr&                rgbClr
                                    , const tCIDGraphDev::EPatterns ePattern)
{
    Reset();
    m_ePattern  = ePattern;
    m_rgbClr    = rgbClr;
}


// ---------------------------------------------------------------------------
//  THatchedBrush: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDGraphDev::TBrushHandle THatchedBrush::hbrCreateBrush() const
{
    // Just set up a structure and create and return the brush
    LOGBRUSH Brush;
    Brush.lbStyle = BS_HATCHED;
    Brush.lbColor = RGB(m_rgbClr.c1Red(), m_rgbClr.c1Green(), m_rgbClr.c1Blue());
    Brush.lbHatch = TGrDevPlatform::c4XlatPattern(m_ePattern);
    return ::hbrCreateNewBrush(Brush);
}



// ---------------------------------------------------------------------------
//  CLASS: TBitmapBrush
// PREFIX: gbr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBitmapBrush: Constructors and Destructor
// ---------------------------------------------------------------------------

// We are taking a shallow copy of the bitmap
TBitmapBrush::TBitmapBrush(const TBitmap& bmpToUse) :

    m_pbmpToUse(new TBitmap(bmpToUse))
{
}

TBitmapBrush::TBitmapBrush(const TBitmapBrush& gbrToCopy) :

    TGUIBrush(gbrToCopy)
    , m_pbmpToUse(new TBitmap(*gbrToCopy.m_pbmpToUse))
{
}

TBitmapBrush::~TBitmapBrush()
{
    // Clean up our bitmap
    delete m_pbmpToUse;
}


// ---------------------------------------------------------------------------
//  TBitmapBrush: Public operators
// ---------------------------------------------------------------------------
TBitmapBrush& TBitmapBrush::operator=(const TBitmapBrush& gbrToAssign)
{
    if (this != &gbrToAssign)
    {
        // Do our parent first. If we are selected, he will throw
        TParent::operator=(gbrToAssign);
        delete m_pbmpToUse;
        m_pbmpToUse = new TBitmap(*gbrToAssign.m_pbmpToUse);
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TBitmapBrush: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDGraphDev::TBrushHandle TBitmapBrush::hbrCreateBrush() const
{
    //
    //  In order to avoid having to keep the bitmap around, we just create
    //  it up front, and store the handle for later return to our parent
    //  class.
    //
    tCIDGraphDev::TBrushHandle hbrRet = ::CreatePatternBrush(m_pbmpToUse->hbmpThis());
    if (hbrRet == kCIDGraphDev::hbrInvalid)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CantCreate
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"Bitmap Brush")
        );
    }
    return hbrRet;
}




// ---------------------------------------------------------------------------
//  CLASS: TNullBrush
// PREFIX: gbr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TNullBrush: Constructors and Destructor
// ---------------------------------------------------------------------------
TNullBrush::TNullBrush()
{
}

TNullBrush::TNullBrush(const TNullBrush& gbrSrc) :

    TGUIBrush(gbrSrc)
{
}

TNullBrush::~TNullBrush()
{
}


// ---------------------------------------------------------------------------
//  TNullBrush: Public operators
// ---------------------------------------------------------------------------
TNullBrush& TNullBrush::operator=(const TNullBrush& gbrSrc)
{
    if (this != &gbrSrc)
    {
        TParent::operator=(gbrSrc);
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TNullBrush: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDGraphDev::TBrushHandle TNullBrush::hbrCreateBrush() const
{
    return (tCIDGraphDev::TBrushHandle)::GetStockObject(NULL_BRUSH);
}




// ---------------------------------------------------------------------------
//  CLASS: TBrushJanitor
// PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Constructors and Destructor
// ---------------------------------------------------------------------------
TBrushJanitor::TBrushJanitor(       TGraphDrawDev* const    pgdevTarget
                            , const TGUIBrush* const        pgbrToSet
                            , const tCIDLib::EAdoptOpts     eAdopt) :

    m_eAdopt(eAdopt)
    , m_hbrRestore(kCIDGraphDev::hbrInvalid)
    , m_pgdevTarget(pgdevTarget)
    , m_pgbrSet(pgbrToSet)
{
    //
    //  Set the passed brush and store the old handle. If we got no target
    //  device, then do nothing.
    //
    if (m_pgdevTarget)
        m_hbrRestore = pgbrToSet->hbrSetOnDevice(*pgdevTarget);
}

TBrushJanitor::~TBrushJanitor()
{
    // If we had a target device, then put the old brush back
    if (m_pgdevTarget)
        m_pgbrSet->Unset(*m_pgdevTarget, m_hbrRestore);

    // If we adopted teh brush, then clean it up
    if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
        delete m_pgbrSet;
}


