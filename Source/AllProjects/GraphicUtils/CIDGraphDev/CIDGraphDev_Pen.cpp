//
// FILE NAME: CIDGraphDev_Pen.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/24/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the various pen classes
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
RTTIDecls(TGUIPen,TObject)
RTTIDecls(TCosmeticPen,TGUIPen)
RTTIDecls(TGeomPen,TGUIPen)
RTTIDecls(TNullPen,TGUIPen)





// ---------------------------------------------------------------------------
//  CLASS: TGUIPen
// PREFIX: gpen
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGUIPen: Public, static methods
// ---------------------------------------------------------------------------
TGUIPen& TGUIPen::Nul_TGUIPen()
{
    static TGUIPen* pgpenNull = 0;
    if (!pgpenNull)
    {
        TBaseLock lockInit;
        if (!pgpenNull)
        {
            TRawMem::pExchangePtr<TGUIPen>
            (
                pgpenNull, new TCosmeticPen(facCIDGraphDev().rgbBlack)
            );
        }
    }
    return *pgpenNull;
}


// ---------------------------------------------------------------------------
//  TGUIPen: Constructors and Destructor
// ---------------------------------------------------------------------------
TGUIPen::~TGUIPen()
{
    // If we are selected into a device, this is illegal
    if (m_bSetOnDevice)
    {
        facCIDGraphDev().LogMsg
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_DtorWhileSelected
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TString(L"Pen")
        );
    }

    // Its ok, so if we have a pen, lets clean it up
    if (m_hpenThis != kCIDGraphDev::hpenInvalid)
        ::DeleteObject(m_hpenThis);
}


// ---------------------------------------------------------------------------
//  TGUIPen: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TGUIPen::bSetOnDevice() const
{
    return m_bSetOnDevice;
}

tCIDLib::TBoolean TGUIPen::bSetOnDevice(const tCIDLib::TBoolean bToSet)
{
    m_bSetOnDevice = bToSet;
    return m_bSetOnDevice;
}


tCIDGraphDev::TPenHandle TGUIPen::hpenThis() const
{
    // If we've not created the pen yet, then do it
    if (m_hpenThis == kCIDGraphDev::hpenInvalid)
        m_hpenThis = hpenCreatePen();
    return m_hpenThis;
}


// ---------------------------------------------------------------------------
//  TGUIPen: Hidden constructors and operators
// ---------------------------------------------------------------------------
TGUIPen::TGUIPen() :

    m_bSetOnDevice(kCIDLib::False)
    , m_hpenThis(kCIDGraphDev::hpenInvalid)
{
}





// ---------------------------------------------------------------------------
//  CLASS: TCosmeticPen
// PREFIX: gpen
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCosmeticPen: Constructors and Destructor
// ---------------------------------------------------------------------------
TCosmeticPen::TCosmeticPen( const   TRGBClr&                    rgbClr
                            , const tCIDGraphDev::ELineStyles   eStyle) :
    m_eStyle(eStyle)
    , m_rgbClr(rgbClr)
{
}

TCosmeticPen::~TCosmeticPen()
{
}


// ---------------------------------------------------------------------------
//  TCosmeticPen: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDGraphDev::TPenHandle TCosmeticPen::hpenCreatePen() const
{
    // Set up the fill pattern stuff
    LOGBRUSH Brush;
    Brush.lbStyle = BS_SOLID;
    Brush.lbColor = m_rgbClr.c4Color();
    Brush.lbHatch = 0;

    // And create the pen
    tCIDGraphDev::TPenHandle hpenRet = ::ExtCreatePen
    (
        TGrDevPlatform::c4XlatLineStyle(m_eStyle)
        | PS_COSMETIC
        , 1
        , &Brush
        , 0
        , 0
    );

    if (hpenRet == kCIDGraphDev::hpenInvalid)
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
            , TString(L"Pen")
        );
    }

    return hpenRet;
}





// ---------------------------------------------------------------------------
//  CLASS: TGeomPen
// PREFIX: gpen
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGeomPen: Constructors and Destructor
// ---------------------------------------------------------------------------
TGeomPen::TGeomPen( const   TRGBClr&        rgbClr
                    , const tCIDLib::TCard4 c4Width) :

    m_c4Width(c4Width)
    , m_eEndType(tCIDGraphDev::ELineEnds::Flat)
    , m_eJoinType(tCIDGraphDev::ELineJoins::Bevel)
    , m_ePattern(tCIDGraphDev::EPatterns::Solid)
    , m_eStyle(tCIDGraphDev::ELineStyles::Solid)
    , m_rgbClr(rgbClr)
{
}

TGeomPen::TGeomPen( const   TRGBClr&                    rgbClr
                    , const tCIDLib::TCard4             c4Width
                    , const tCIDGraphDev::ELineEnds     eEndType
                    , const tCIDGraphDev::ELineJoins    eJoinType
                    , const tCIDGraphDev::ELineStyles   eStyle) :

    m_c4Width(c4Width)
    , m_eEndType(eEndType)
    , m_eJoinType(eJoinType)
    , m_ePattern(tCIDGraphDev::EPatterns::Solid)
    , m_eStyle(eStyle)
    , m_rgbClr(rgbClr)
{
}

TGeomPen::TGeomPen( const   TRGBClr&                    rgbClr
                    , const tCIDLib::TCard4             c4Width
                    , const tCIDGraphDev::ELineEnds     eEndType
                    , const tCIDGraphDev::ELineJoins    eJoinType
                    , const tCIDGraphDev::EPatterns     ePattern) :

    m_c4Width(c4Width)
    , m_eEndType(eEndType)
    , m_eJoinType(eJoinType)
    , m_ePattern(ePattern)
    , m_eStyle(tCIDGraphDev::ELineStyles::Solid)
    , m_rgbClr(rgbClr)
{
}

TGeomPen::~TGeomPen()
{
}


// ---------------------------------------------------------------------------
//  TGeomPen: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDGraphDev::TPenHandle TGeomPen::hpenCreatePen() const
{
    // Set up the fill pattern stuff
    LOGBRUSH Brush;
    if (m_ePattern == tCIDGraphDev::EPatterns::Solid)
        Brush.lbStyle = BS_SOLID;
    else
        Brush.lbStyle = BS_HATCHED;
    Brush.lbColor = m_rgbClr.c4Color();
    Brush.lbHatch = TGrDevPlatform::c4XlatPattern(m_ePattern);

    // And create the pen
    tCIDGraphDev::TPenHandle hpenRet = ::ExtCreatePen
    (
        TGrDevPlatform::c4XlatLineStyle(m_eStyle)
        | TGrDevPlatform::c4XlatJoinStyle(m_eJoinType)
        | TGrDevPlatform::c4XlatEndStyle(m_eEndType)
        | PS_GEOMETRIC
        , m_c4Width
        , &Brush
        , 0
        , 0
    );

    if (hpenRet == kCIDGraphDev::hpenInvalid)
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
            , TString(L"Pen")
        );
    }

    return hpenRet;
}




// ---------------------------------------------------------------------------
//  CLASS: TNullPen
// PREFIX: gpen
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TNullPen: Constructors and Destructor
// ---------------------------------------------------------------------------
TNullPen::TNullPen()
{
}

TNullPen::~TNullPen()
{
}


// ---------------------------------------------------------------------------
//  TNullPen: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDGraphDev::TPenHandle TNullPen::hpenCreatePen() const
{
    return (tCIDGraphDev::TPenHandle)::GetStockObject(NULL_PEN);
}




// ---------------------------------------------------------------------------
//  CLASS: TPenJanitor
// PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Constructors and Destructor
// ---------------------------------------------------------------------------
TPenJanitor::TPenJanitor(TGraphDrawDev* const   pgdevTarget
                        , TGUIPen* const        pgpenToSet) :

    m_hpenRestore(kCIDGraphDev::hpenInvalid)
    , m_pgdevTarget(pgdevTarget)
    , m_pgpenSet(pgpenToSet)
{
    // Set the passed brush and store the old handle
    m_hpenRestore = pgdevTarget->hpenSetPen(*pgpenToSet);
}

TPenJanitor::~TPenJanitor()
{
    m_pgdevTarget->PopPen(*m_pgpenSet, m_hpenRestore);
}


