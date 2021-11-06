//
// FILE NAME: CIDGraphDev_Device.cpp
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
//  This file implements the TGraphicDevice class, which is a wrapper class
//  for the host OS' graphics output device handle.
//
//  A derivative is provided, TGraphDrawDev, that is the base for all devices
//  that can be drawn on.
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
#include    <commctrl.h>


// ---------------------------------------------------------------------------
//  We just need a small amount of DD functionality, so we avoid the whole
//  header issue by just bringing in what we need. Semi-dangerous, but it
//  avoids a lot of overhead. We dynamically load the DLL as required.
// ---------------------------------------------------------------------------
DEFINE_GUID( IID_IDirectDraw,
0x6C14DB80,0xA733,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60 );
#define DDWAITVB_BLOCKBEGIN                     0x00000001l
DECLARE_INTERFACE_( IDirectDraw, IUnknown )
{
  STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
  STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
  STDMETHOD_(ULONG,Release) (THIS) PURE;
  STDMETHOD(Compact)(THIS) PURE;
  STDMETHOD(CreateClipper)(THIS_ DWORD, void*, IUnknown FAR * ) PURE;
  STDMETHOD(CreatePalette)(THIS_ DWORD, void*, void*, IUnknown FAR *) PURE;
  STDMETHOD(CreateSurface)(THIS_  void*, void*, IUnknown FAR *) PURE;
  STDMETHOD(DuplicateSurface)( THIS_ void*, void*) PURE;
  STDMETHOD(EnumDisplayModes)( THIS_ DWORD, void*, LPVOID, void*) PURE;
  STDMETHOD(EnumSurfaces)(THIS_ DWORD, void*, LPVOID,void* ) PURE;
  STDMETHOD(FlipToGDISurface)(THIS) PURE;
  STDMETHOD(GetCaps)( THIS_ void*, void*) PURE;
  STDMETHOD(GetDisplayMode)( THIS_ void*) PURE;
  STDMETHOD(GetFourCCCodes)(THIS_  LPDWORD, LPDWORD ) PURE;
  STDMETHOD(GetGDISurface)(THIS_ void*) PURE;
  STDMETHOD(GetMonitorFrequency)(THIS_ LPDWORD) PURE;
  STDMETHOD(GetScanLine)(THIS_ LPDWORD) PURE;
  STDMETHOD(GetVerticalBlankStatus)(THIS_ LPBOOL ) PURE;
  STDMETHOD(Initialize)(THIS_ GUID FAR *) PURE;
  STDMETHOD(RestoreDisplayMode)(THIS) PURE;
  STDMETHOD(SetCooperativeLevel)(THIS_ HWND, DWORD) PURE;
  STDMETHOD(SetDisplayMode)(THIS_ DWORD, DWORD,DWORD) PURE;
  STDMETHOD(WaitForVerticalBlank)(THIS_ DWORD, HANDLE ) PURE;
};



// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TGraphicDevice,TObject)
RTTIDecls(TGraphDrawDev,TGraphicDevice)



// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDGraphDev_Device
    {
        // -----------------------------------------------------------------------
        //  We may fault in a DD object if the client code invokes any of our
        //  functionality that requires it. It may also not load, so we have a
        //  flag to remember if we already tried it.
        // -----------------------------------------------------------------------
        TAtomicFlag         atomDDLoad;
        HINSTANCE           hDDraw;
        IDirectDraw*        pDDraw = nullptr;
    }
}



// ---------------------------------------------------------------------------
//  Local static methods
// ---------------------------------------------------------------------------
static tCIDLib::TBoolean bDevErrToIgnore()
{
    tCIDLib::TCard4 c4LastErr = ::GetLastError();
    if (!c4LastErr
    ||  (c4LastErr == ERROR_ACCESS_DENIED)
    ||  (c4LastErr == ERROR_INVALID_HANDLE))
    {
        return kCIDLib::True;
    }

    TKrnlError::SetLastHostError(c4LastErr);
    return kCIDLib::False;
}


static tCIDLib::TBoolean bDevErrToIgnore(const tCIDLib::TCard4 c4ToCheck)
{
    return (!c4ToCheck
            || (c4ToCheck == ERROR_ACCESS_DENIED)
            || (c4ToCheck == ERROR_INVALID_HANDLE));
}


//
//  Calculate a gradient color given a particular angle around a circle,
//  assuming a light source in the upper left.
//
static tCIDLib::TCard4 c4AngleColor(const   tCIDLib::TFloat8    f8Angle
                                    , const tCIDLib::TCard4     c4Light
                                    , const tCIDLib::TCard4     c4Dark)
{
    static const tCIDLib::TFloat8 f8SrcAngle = -1.88;

    tCIDLib::TFloat8 f8Diff = f8SrcAngle - f8Angle;

    if (f8Diff < -kCIDLib::f8PI)
        f8Diff += 6.293;
    else if (f8Diff > kCIDLib::f8PI)
        f8Diff = 6.293 - f8Diff;

    const tCIDLib::TFloat8 f8Weight = 0.5
                                      * (TMathLib::f8Cosine(f8Diff) + 1.0);

    tCIDLib::TCodedRGB* rgbLight = (tCIDLib::TCodedRGB*)&c4Light;
    tCIDLib::TCodedRGB* rgbDark  = (tCIDLib::TCodedRGB*)&c4Dark;

    tCIDLib::TCard1 c1Red = tCIDLib::TCard1
    (
        (f8Weight * rgbLight->c1Red) + ((1.0 - f8Weight) * rgbDark->c1Red)
    );

    tCIDLib::TCard1 c1Green = tCIDLib::TCard1
    (
        (f8Weight * rgbLight->c1Green) + ((1.0 - f8Weight) * rgbDark->c1Green)
    );

    tCIDLib::TCard1 c1Blue = tCIDLib::TCard1
    (
        (f8Weight * rgbLight->c1Blue) + ((1.0 - f8Weight) * rgbDark->c1Blue)
    );
    return RGB(c1Red, c1Green, c1Blue);
}


// Faults in our DirectDraw support
static tCIDLib::TVoid LoadDDraw()
{
    // The caller already did one check, so assue we have to lock
    TBaseLock lockInit;
    if (!CIDGraphDev_Device::atomDDLoad)
    {
        // Try to load the DLL. If it fails, we are done
        CIDGraphDev_Device::hDDraw = ::LoadLibrary(L"ddraw.dll");
        if (!CIDGraphDev_Device::hDDraw)
        {
            // Don't try again
            CIDGraphDev_Device::atomDDLoad.Set();
            return;
        }

        // Alias the function pointer we'll extract
        using DIRECTDRAWCREATE = HRESULT (WINAPI *)
        (
            GUID FAR *lpGUID, IDirectDraw **lplpDD, IUnknown FAR *pUnkOuter
        );

        // And try to load an instance of that function
        DIRECTDRAWCREATE pfnDDCreate = (DIRECTDRAWCREATE)::GetProcAddress
        (
            CIDGraphDev_Device::hDDraw, "DirectDrawCreate"
        );

        // If it failed, then we give up
        if (!pfnDDCreate)
        {
            // Don't try again
            CIDGraphDev_Device::atomDDLoad.Set();
            return;
        }

        // Looks like it worked, so try to create our instance
        try
        {
            CIDGraphDev_Device::pDDraw = nullptr;
            pfnDDCreate(0, &CIDGraphDev_Device::pDDraw, 0);
        }

        catch(...)
        {
            // Eat it and make sure pointer is zero
            CIDGraphDev_Device::pDDraw = nullptr;
        }

        // Indicate last that we have tried it
        CIDGraphDev_Device::atomDDLoad.Set();
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TGraphicDevice
// PREFIX: gdev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGraphicDevice: Public, static methods
// ---------------------------------------------------------------------------
TGraphicDevice& TGraphicDevice::Nul_TGraphicDevice() noexcept
{
    static TGraphicDevice gdevNull;
    return gdevNull;
}


// ---------------------------------------------------------------------------
//  TGraphicDevice: Destructor
// ---------------------------------------------------------------------------
TGraphicDevice::~TGraphicDevice()
{
    //
    //  We don't destroy the device handle here because only the derived
    //  class knows how to do that!
    //
}


// ---------------------------------------------------------------------------
//  TGraphicDevice: Public, virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TGraphicDevice::bCapConstAlpha() const
{
    return (m_c4BlendCaps && SB_CONST_ALPHA) != 0;
}


tCIDLib::TBoolean TGraphicDevice::bCapSrcAlpha() const
{
    return (m_c4BlendCaps && SB_PIXEL_ALPHA) != 0;
}


tCIDLib::TBoolean TGraphicDevice::bIsPaletteBased() const
{
    if (::GetDeviceCaps(m_hdevThis, RASTERCAPS) & RC_PALETTE)
        return kCIDLib::True;
    return kCIDLib::False;
}

tCIDLib::TCard4 TGraphicDevice::c4BitsPerPixel() const
{
    return m_c4BitsPerPixel;
}


tCIDLib::EAdoptOpts TGraphicDevice::eAdopted() const
{
    return m_eAdopt;
}


tCIDGraphDev::TDeviceHandle TGraphicDevice::hdevThis() const
{
    return m_hdevThis;
}


TSize TGraphicDevice::szPixels() const
{
    return TSize
    (
        tCIDLib::TCard4(::GetDeviceCaps(m_hdevThis, HORZRES))
        , tCIDLib::TCard4(::GetDeviceCaps(m_hdevThis, VERTRES))
    );
}

TSize TGraphicDevice::szPixelsPerInch() const
{
    return TSize
    (
        tCIDLib::TCard4(::GetDeviceCaps(m_hdevThis, LOGPIXELSX))
        , tCIDLib::TCard4(::GetDeviceCaps(m_hdevThis, LOGPIXELSY))
    );
}


// ---------------------------------------------------------------------------
//  TGraphicDevice: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TGraphicDevice::SetHandle(  const   tCIDGraphDev::TDeviceHandle hdevToUse
                            , const tCIDLib::EAdoptOpts         eAdopt)
{
    m_eAdopt = eAdopt;
    m_hdevThis = hdevToUse;

    // And do any one time setup of the device thats required
    OneTimeDeviceInit();
}


// ---------------------------------------------------------------------------
//  TGraphicDevice: Hidden Constructors
// ---------------------------------------------------------------------------
TGraphicDevice::TGraphicDevice() :

    m_eAdopt(tCIDLib::EAdoptOpts::NoAdopt)
    , m_c4BitsPerPixel(0)
    , m_c4BlendCaps(0)
    , m_hdevThis(kCIDGraphDev::hdevInvalid)
{
}

TGraphicDevice::TGraphicDevice( const   tCIDGraphDev::TDeviceHandle hdevToUse
                                , const tCIDLib::EAdoptOpts         eAdopt) :
    m_eAdopt(eAdopt)
    , m_hdevThis(hdevToUse)
{
    // And do any one time setup of the device thats required
    OneTimeDeviceInit();
}


// ---------------------------------------------------------------------------
//  TGraphicDevice: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TGraphicDevice::OneTimeDeviceInit()
{
    ::SetMapMode(m_hdevThis, MM_TEXT);
    ::SetArcDirection(m_hdevThis, AD_CLOCKWISE);
    ::SetStretchBltMode(m_hdevThis, HALFTONE);
    ::SetBrushOrgEx(m_hdevThis, 0, 0, 0);

    // Store some flags for later use
    m_c4BitsPerPixel = tCIDLib::TCard4(::GetDeviceCaps(m_hdevThis, BITSPIXEL));
    m_c4BlendCaps = ::GetDeviceCaps(m_hdevThis, SHADEBLENDCAPS);
}




// ---------------------------------------------------------------------------
//  CLASS: TGraphDrawDev
// PREFIX: gdev
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGraphDrawDev: Public, static methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TGraphDrawDev::WaitRetrace()
{
    // Fault in the DirectShow stuff if not done yet
    if (!CIDGraphDev_Device::atomDDLoad)
        LoadDDraw();

    // If we got what we wanted, then do the wait, else we just don't wait
    if (CIDGraphDev_Device::pDDraw)
        CIDGraphDev_Device::pDDraw->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0);
}


// ---------------------------------------------------------------------------
//  TGraphDrawDev: Destructor
// ---------------------------------------------------------------------------
TGraphDrawDev::~TGraphDrawDev()
{
}


// ---------------------------------------------------------------------------
//  TGraphDrawDev: Public, virtual methods
// ---------------------------------------------------------------------------
//
//  Gets the bounds of all graphics drawn so far, optionally resetting the
//  accumulation in the process.
//
TArea TGraphDrawDev::areaBounds(const tCIDLib::TBoolean bReset) const
{
    tCIDLib::TCard4 c4Flags = 0;
    if (bReset)
        c4Flags |= DCB_RESET;

    tCIDLib::THostRectl rectlTmp;
    tCIDLib::TCard4 c4Res = ::GetBoundsRect
    (
        hdevThis(), (RECT*)&rectlTmp, c4Flags
    );

    if (!c4Res)
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_GetBounds
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }

    if ((c4Res & DCB_SET) == DCB_SET)
        return TArea(rectlTmp);

    return TArea();
}


//
//  Calculates the area required to draw a multi-line text string given
//  the indicated area width, so basically it sets the height required for
//  that widget.
//
TArea TGraphDrawDev::areaMLText(const   TString&        strText
                                , const tCIDLib::TCard4 c4Width
                                , const tCIDLib::TCard4 c4Start
                                , const tCIDLib::TCard4 c4Len)
{
    //
    //  Set up a starting rectangle, at the origin and with the indicated
    //  width.
    //
    tCIDLib::THostRectl rectlTmp = {0};
    rectlTmp.i4Right = c4Width - 1;

    // We need this structure for the DrawString call
    DRAWTEXTPARAMS Params = {0};
    Params.cbSize = sizeof(DRAWTEXTPARAMS);

    tCIDLib::TInt4 i4Len = -1;
    if (c4Len != kCIDLib::c4MaxCard)
        i4Len = tCIDLib::TInt4(c4Len);

    // This will update the rectangle to hold the new bottom
    ::DrawTextEx
    (
        hdevThis()
        , (tCIDLib::TCh*)strText.pszBufferAt(c4Start)
        , i4Len
        , (RECT*)&rectlTmp
        , DT_TOP | DT_LEFT | DT_WORDBREAK | DT_CALCRECT | DT_NOPREFIX
        , &Params
    );

    // Get the temp rectangle into an area for return
    return TArea(rectlTmp);
}


TArea
TGraphDrawDev::areaString(  const   TString&            strText
                            , const tCIDLib::TBoolean   bSkipMnemonic) const
{
    return areaString(strText, 0, kCIDLib::c4MaxCard, bSkipMnemonic);
}

TArea
TGraphDrawDev::areaString(  const   TString&            strText
                            , const tCIDLib::TCard4     c4Start
                            , const tCIDLib::TCard4     c4Len
                            , const tCIDLib::TBoolean   bSkipMnemonic) const
{
    //
    //  Pull in the indicated substring. Note that this guy will handle the
    //  c4Len parm being c4MaxCard or too long for the source or
    //  destination strings.
    //
    TString strTmp(kCIDLib::pszEmptyZStr, strText.c4Length());
    strTmp.CopyInSubStr(strText, c4Start, c4Len);

    //
    //  If we need to pull out the mnemonic char, then do that now before
    //  we pull out the substring.
    //
    if (bSkipMnemonic)
    {
        tCIDLib::TCard4 c4Ind;
        if (strTmp.bFirstOccurrence(kCIDGraphDev::chMnemonic, c4Ind))
            strTmp.Cut(c4Ind, 1);
    }

    SIZE SizeInfo;
    if (!::GetTextExtentPoint32W
    (
        hdevThis()
        , strTmp.pszBuffer()
        , strTmp.c4Length()
        , &SizeInfo))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_QueryTextExtent
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }

    return TArea(0, 0, SizeInfo.cx, SizeInfo.cy);
}


tCIDLib::TVoid
TGraphDrawDev::AlphaBlit(const  TBitmap&            bmpSrc
                        , const TArea&              areaDest
                        , const tCIDLib::TCard1     c1Alpha
                        , const tCIDLib::TBoolean   bSrcAlpha
                        , const tCIDLib::TBoolean   bWaitRetrace)
{
    //
    //  Set up a memory device over the bitmap, and call the other version
    //  with the source area.
    //
    TGraphMemDev gdevSrc(*this, bmpSrc);
    TArea areaSrc(TPoint::pntOrigin, bmpSrc.szBitmap());
    AlphaBlit(gdevSrc, areaSrc, areaDest, c1Alpha, bSrcAlpha, bWaitRetrace);
}

tCIDLib::TVoid
TGraphDrawDev::AlphaBlit(const  TBitmap&            bmpSrc
                        , const TPoint&             pntDestUL
                        , const tCIDLib::TCard1     c1Alpha
                        , const tCIDLib::TBoolean   bSrcAlpha
                        , const tCIDLib::TBoolean   bWaitRetrace)
{
    //
    //  Set up a memory device over the bitmap, and call the other version
    //  with the source area.
    //
    TGraphMemDev gdevSrc(*this, bmpSrc);
    TArea areaSrc(TPoint::pntOrigin, bmpSrc.szBitmap());
    TArea areaDest(pntDestUL, areaSrc.szArea());
    AlphaBlit(gdevSrc, areaSrc, areaDest, c1Alpha, bSrcAlpha, bWaitRetrace);
}

tCIDLib::TVoid
TGraphDrawDev::AlphaBlit(const  TBitmap&            bmpSrc
                        , const TArea&              areaSource
                        , const TArea&              areaDest
                        , const tCIDLib::TCard1     c1Alpha
                        , const tCIDLib::TBoolean   bSrcAlpha
                        , const tCIDLib::TBoolean   bWaitRetrace)
{
    // Set up a memory device over the bitmap, and call the other version
    TGraphMemDev gdevSrc(*this, bmpSrc);
    AlphaBlit(gdevSrc, areaSource, areaDest, c1Alpha, bSrcAlpha, bWaitRetrace);
}

tCIDLib::TVoid
TGraphDrawDev::AlphaBlit(const  TBitmap&            bmpSrc
                        , const TArea&              areaSource
                        , const TPoint&             pntDestUL
                        , const tCIDLib::TCard1     c1Alpha
                        , const tCIDLib::TBoolean   bSrcAlpha
                        , const tCIDLib::TBoolean   bWaitRetrace)
{
    //
    //  Set up a memory device over the bitmap, and call the other version
    //  with the source area.
    //
    TGraphMemDev gdevSrc(*this, bmpSrc);
    TArea areaDest(pntDestUL, areaSource.szArea());
    AlphaBlit(gdevSrc, areaSource, areaDest, c1Alpha, bSrcAlpha, bWaitRetrace);
}

tCIDLib::TVoid
TGraphDrawDev::AlphaBlit(const  TGraphDrawDev&      gdevSrc
                        , const TArea&              areaSource
                        , const TArea&              areaDest
                        , const tCIDLib::TCard1     c1Alpha
                        , const tCIDLib::TBoolean   bSrcAlpha
                        , const tCIDLib::TBoolean   bWaitRetrace)
{
    tCIDGraphDev::TDeviceHandle hdevSrc = gdevSrc.hdevThis();
    tCIDGraphDev::TDeviceHandle hdevUs = hdevThis();

    // Create a blend function based on the passed alpha
    BLENDFUNCTION Blend;
    Blend.BlendOp = AC_SRC_OVER;
    Blend.BlendFlags = 0;
    Blend.SourceConstantAlpha = c1Alpha;
    Blend.AlphaFormat = bSrcAlpha ? AC_SRC_ALPHA : 0;

    //
    //  Get all the values out that we'll use. because if we are doing a
    //  vertical retrace wait, we want nothing but the invocation of the blit
    //  to be next.
    //
    const tCIDLib::TInt4 i4DX = areaDest.i4X();
    const tCIDLib::TInt4 i4DY = areaDest.i4Y();
    const tCIDLib::TCard4 c4DCX = areaDest.c4Width();
    const tCIDLib::TCard4 c4DCY = areaDest.c4Height();

    const tCIDLib::TInt4 i4SX = areaSource.i4X();
    const tCIDLib::TInt4 i4SY = areaSource.i4Y();
    const tCIDLib::TCard4 c4SCX = areaSource.c4Width();
    const tCIDLib::TCard4 c4SCY = areaSource.c4Height();

    //
    //  If they asked us to wait for the vertical retrace then let's do that.
    //  We may have to fault in the DD API that we use to do this if it's
    //  not been already.
    //
    if (bWaitRetrace)
        WaitRetrace();

    // And now do the blit and check the result
    const tCIDLib::TCard4 c4Res = ::AlphaBlend
    (
        hdevUs, i4DX, i4DY, c4DCX, c4DCY, hdevSrc, i4SX, i4SY, c4SCX, c4SCY, Blend
    );
    if (!c4Res)
    {
        if (!bDevErrToIgnore())
        {
            TKrnlError kerrCheck = TKrnlError::kerrLast();

            // <TBD>
            // We are getting some bogus errors here, despite the fact it works
            #if 0
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_BitBlit
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
            #endif
        }
    }
}


tCIDLib::TBoolean TGraphDrawDev::bBoundsEnabled() const
{
    tCIDLib::THostRectl rectTmp;
    tCIDLib::TCard4 c4Res = ::GetBoundsRect(hdevThis(), (RECT*)&rectTmp, 0);
    if (!c4Res)
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_GetBounds
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
    return ((c4Res & DCB_ENABLE) != 0);
}


//
//  Exclude the passed area from our current clip region. Returns true if the regions is
//  non-empty. And we have another that intersects with our current region.
//
tCIDLib::TBoolean TGraphDrawDev::bExcludeClipArea(const TArea& areaEx)
{
    int iRes = ::ExcludeClipRect
    (
        hdevThis(), areaEx.i4Left(), areaEx.i4Top(), areaEx.i4Right(), areaEx.i4Bottom()
    );

    if (iRes == ERROR)
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_ExcludeRegion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return (iRes != NULLREGION);
}

tCIDLib::TBoolean TGraphDrawDev::bIntersectClipArea(const TArea& areaInt)
{
    int iRes = ::IntersectClipRect
    (
        hdevThis(), areaInt.i4Left(), areaInt.i4Top(), areaInt.i4Right(), areaInt.i4Bottom()
    );

    if (iRes == ERROR)
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_IntersectRegion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return (iRes != NULLREGION);
}


// Start accumulating a path based on line, polygon, etc... operations
tCIDLib::TVoid TGraphDrawDev::BeginPath()
{
    if (!::BeginPath(hdevThis()))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_BeginPath
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


tCIDLib::TCard4 TGraphDrawDev::c4Extra() const
{
    const tCIDLib::TCard4 c4Ret = ::GetTextCharacterExtra(hdevThis());
    if (c4Ret == 0x8000000)
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_GetTextExtra
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
    return c4Ret;
}

tCIDLib::TCard4 TGraphDrawDev::c4Extra(const tCIDLib::TCard4 c4ToSet)
{
    const tCIDLib::TCard4 c4Ret = ::SetTextCharacterExtra(hdevThis(), c4ToSet);
    if (c4Ret == 0x8000000)
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_SetTextExtra
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
    return c4Ret;
}


tCIDLib::TCard4 TGraphDrawDev::c4HPelsPerInch() const
{
    return tCIDLib::TCard4(::GetDeviceCaps(hdevThis(), LOGPIXELSX));
}


tCIDLib::TCard4 TGraphDrawDev::c4VPelsPerInch() const
{
    return tCIDLib::TCard4(::GetDeviceCaps(hdevThis(), LOGPIXELSY));
}


tCIDLib::TVoid TGraphDrawDev::ClearClipRegion()
{
    if (!::ExtSelectClipRgn(hdevThis(), 0, RGN_COPY))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_ClearClipRegion
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


tCIDLib::TVoid
TGraphDrawDev::CopyBits(       TGraphDrawDev&           gdevSrc
                        , const TPoint&                 pntSrc
                        , const TArea&                  areaTarget
                        , const tCIDGraphDev::EBmpModes eMode
                        , const tCIDLib::TBoolean       bWaitRetrace)
{
    //
    //  Get all the info ready first. If we are waiting for the vertical
    //  retrace, then we want to get our blit done as quickly as possible
    //  once it arrives. So we want to get everything out up front.
    //
    const tCIDLib::TInt4 i4DX = areaTarget.i4X();
    const tCIDLib::TInt4 i4DY = areaTarget.i4Y();
    const tCIDLib::TCard4 c4DCX = areaTarget.c4Width();
    const tCIDLib::TCard4 c4DCY = areaTarget.c4Height();
    const tCIDLib::TInt4 i4SX = pntSrc.i4X();
    const tCIDLib::TInt4 i4SY = pntSrc.i4Y();
    const tCIDLib::TCard4 c4Mode = TGrDevPlatform::c4XlatBmpMode(eMode);

    //  If they asked us to wait for the vertical retrace then let's do that.
    if (bWaitRetrace)
        WaitRetrace();

    if (!::BitBlt(  hdevThis()
                    , i4DX
                    , i4DY
                    , c4DCX
                    , c4DCY
                    , gdevSrc.hdevThis()
                    , i4SX
                    , i4SY
                    , c4Mode))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_BitBlit
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}

tCIDLib::TVoid
TGraphDrawDev::CopyBits(        TGraphDrawDev&          gdevSrc
                        , const TArea&                  areaSrc
                        , const TArea&                  areaTarget
                        , const tCIDGraphDev::EBmpModes eMode
                        , const tCIDLib::TBoolean       bWaitRetrace)
{
    //
    //  Get all the info ready first. If we are waiting for the vertical
    //  retrace, then we want to get our blit done as quickly as possible
    //  once it arrives. So we want to get everything out up front.
    //
    const tCIDLib::TInt4 i4DX = areaTarget.i4X();
    const tCIDLib::TInt4 i4DY = areaTarget.i4Y();
    const tCIDLib::TCard4 c4DCX = areaTarget.c4Width();
    const tCIDLib::TCard4 c4DCY = areaTarget.c4Height();
    const tCIDLib::TInt4 i4SX = areaSrc.i4X();
    const tCIDLib::TInt4 i4SY = areaSrc.i4Y();
    const tCIDLib::TCard4 c4SCX = areaSrc.c4Width();
    const tCIDLib::TCard4 c4SCY = areaSrc.c4Height();
    const tCIDLib::TCard4 c4Mode = TGrDevPlatform::c4XlatBmpMode(eMode);

    // If they asked us to wait for the vertical retrace then let's do that.
    if (bWaitRetrace)
        WaitRetrace();

    if (!::StretchBlt(  hdevThis()
                        , i4DX
                        , i4DY
                        , c4DCX
                        , c4DCY
                        , gdevSrc.hdevThis()
                        , i4SX
                        , i4SY
                        , c4SCX
                        , c4SCY
                        , c4Mode))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_BitBlit
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


tCIDLib::TVoid TGraphDrawDev::DisableBoundsAccumulation()
{
    if (!(::SetBoundsRect(hdevThis(), 0, DCB_DISABLE) & DCB_DISABLE))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_EnableBounds
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


tCIDLib::TVoid TGraphDrawDev::DrawArc(  const   TPoint&         pntCenter
                                        , const tCIDLib::TCard4 c4Radius
                                        , const tCIDLib::TCard4 c4StartDegree
                                        , const tCIDLib::TCard4 c4EndDegree)
{
    //
    //  Sanity check the degrees, and adjust them if necessary. If it turns
    //  out that its effectively a circle, that's fine, it'll work ok. But
    //  we want to normalize the degrees so its at worst 360 degrees.
    //
    tCIDLib::TCard4 c4ActualStart = c4StartDegree;
    if (c4ActualStart > 360)
        c4ActualStart %= 360;

    tCIDLib::TCard4 c4ActualEnd = c4EndDegree;
    if (c4ActualEnd > 360)
        c4ActualEnd %= 360;

    //
    //  Now convert these degrees, and the passed radius, into cartesian
    //  coordinates, which is the way the kind of kludgey Win32 arc API
    //  works.
    //
    TPoint pntStart;
    pntStart.FromPolarDegrees(c4ActualStart, c4Radius);
    pntStart += pntCenter;

    TPoint pntEnd;
    pntEnd.FromPolarDegrees(c4ActualEnd, c4Radius);
    pntEnd += pntCenter;

    // Calc the sides of the bounding box
    const tCIDLib::TInt4 i4Left = tCIDLib::TInt4(pntCenter.i4X() - c4Radius);
    const tCIDLib::TInt4 i4Top = tCIDLib::TInt4(pntCenter.i4Y() - c4Radius);
    const tCIDLib::TInt4 i4Right = tCIDLib::TInt4(pntCenter.i4X() + c4Radius);
    const tCIDLib::TInt4 i4Bottom = tCIDLib::TInt4(pntCenter.i4Y() + c4Radius);

    if (!::Arc( hdevThis()
                , i4Left
                , i4Top
                , i4Right
                , i4Bottom
                , pntStart.i4X()
                , pntStart.i4Y()
                , pntEnd.i4X()
                , pntEnd.i4Y()))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_DrawCircle
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , pntCenter
                , TCardinal(c4Radius)
            );
        }
    }
}


//
//  Various variations to draw a bitmap
//
tCIDLib::TVoid
TGraphDrawDev::DrawBitmap(  const  TBitmap&                 bmpToDraw
                            , const TPoint&                 pntUL
                            , const tCIDGraphDev::EBmpModes eMode)
{
    // Just call the other one, with a source point at the origin
    DrawBitmap(bmpToDraw, TPoint::pntOrigin, pntUL, eMode);
}

tCIDLib::TVoid
TGraphDrawDev::DrawBitmap(  const   TBitmap&                bmpToDraw
                            , const TPoint&                 pntSrcUL
                            , const TPoint&                 pntDestUL
                            , const tCIDGraphDev::EBmpModes eMode)
{
    //
    //  Create a memory device from this device and select the bitmap
    //  into it.
    //
    TGraphMemDev gdevTmp(*this, bmpToDraw);

    // Ok, now we can blit from the memory device to this device
    tCIDLib::TCard4 c4Height;
    tCIDLib::TCard4 c4Width;
    bmpToDraw.QuerySize(c4Width, c4Height);

    if (!::BitBlt
    (
        hdevThis()
        , pntDestUL.i4X()
        , pntDestUL.i4Y()
        , c4Width
        , c4Height
        , gdevTmp.hdevThis()
        , pntSrcUL.i4X()
        , pntSrcUL.i4Y()
        , TGrDevPlatform::c4XlatBmpMode(eMode)))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_BitBlit
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}

tCIDLib::TVoid
TGraphDrawDev::DrawBitmap(  const   TBitmap&                bmpToDraw
                            , const TArea&                  areaSrc
                            , const TPoint&                 pntDestUL
                            , const tCIDGraphDev::EBmpModes eMode)
{
    //
    //  Create a memory device from this device and select the bitmap
    //  into it.
    //
    TGraphMemDev gdevTmp(*this, bmpToDraw);

    if (!::BitBlt
    (
        hdevThis()
        , pntDestUL.i4X()
        , pntDestUL.i4Y()
        , areaSrc.c4Width()
        , areaSrc.c4Height()
        , gdevTmp.hdevThis()
        , areaSrc.i4X()
        , areaSrc.i4Y()
        , TGrDevPlatform::c4XlatBmpMode(eMode)))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_BitBlit
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}

tCIDLib::TVoid
TGraphDrawDev::DrawBitmap(  const   TBitmap&                bmpToDraw
                            , const TArea&                  areaDest
                            , const tCIDGraphDev::EBmpModes eMode)

{
    //
    //  Just call the other version with a source area of the whole bitmap
    //  so that it draw the whole bitmap.
    //
    tCIDLib::TCard4 c4Height;
    tCIDLib::TCard4 c4Width;
    bmpToDraw.QuerySize(c4Width, c4Height);
    DrawBitmap
    (
        bmpToDraw
        , TArea(TPoint::pntOrigin, TSize(c4Width, c4Height))
        , areaDest
        , eMode
    );
}

tCIDLib::TVoid
TGraphDrawDev::DrawBitmap(  const   TBitmap&                bmpToDraw
                            , const TArea&                  areaSource
                            , const TArea&                  areaDest
                            , const tCIDGraphDev::EBmpModes eMode)
{
    //
    //  Create a memory device from this device and select the bitmap
    //  into it.
    //
    TGraphMemDev gdevTmp(*this, bmpToDraw);

    //
    //  Ok, now we can blit from the memory device to this device. IF the
    //  source and destination sizes are different, use the stretch version,
    //  else use the standard version.
    //
    if (areaSource.szArea() == areaDest.szArea())
    {
        if (!::BitBlt
        (
            hdevThis()
            , areaDest.i4X()
            , areaDest.i4Y()
            , areaDest.c4Width()
            , areaDest.c4Height()
            , gdevTmp.hdevThis()
            , areaSource.i4X()
            , areaSource.i4Y()
            , TGrDevPlatform::c4XlatBmpMode(eMode)))
        {
            if (!bDevErrToIgnore())
            {
                facCIDGraphDev().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kGrDevErrs::errcDev_BitBlit
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }
        }
    }
     else
    {
        if (!::StretchBlt
        (
            hdevThis()
            , areaDest.i4Left()
            , areaDest.i4Top()
            , areaDest.c4Width()
            , areaDest.c4Height()
            , gdevTmp.hdevThis()
            , areaSource.i4Left()
            , areaSource.i4Top()
            , areaSource.c4Width()
            , areaSource.c4Height()
            , TGrDevPlatform::c4XlatBmpMode(eMode)))
        {
            if (!bDevErrToIgnore())
            {
                facCIDGraphDev().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kGrDevErrs::errcDev_BitBlit
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }
        }
    }
}


//
//  This is for when we need to draw a bitmap that doesn't include alpha
//  but we need to insure that the target gets the alpha channel set up
//  correctly.
//
tCIDLib::TVoid
TGraphDrawDev::DrawBitmapAlpha( const   TBitmap&                bmpToDraw
                                , const TArea&                  areaSource
                                , const TArea&                  areaDest
                                , const tCIDGraphDev::EBmpModes eMode)
{
    // Draw it into a temp buffer the size of the image
    TBitmap bmpBuf
    (
        areaDest.szArea()
        , tCIDImage::EPixFmts::TrueAlpha
        , tCIDImage::EBitDepths::Eight
        , kCIDLib::True

    );

    // Set up zero based versions of the areas
    TArea areaZSrc(areaSource);
    TArea areaZDest(areaDest);
    areaZSrc.ZeroOrg();
    areaZDest.ZeroOrg();

    {
        TGraphMemDev gdevBuf(*this, bmpBuf);
        gdevBuf.DrawBitmap(bmpToDraw, areaZSrc, areaZDest, eMode);
    }
    bmpBuf.pixaData().SetAllAlpha(0xFF);
    TGraphMemDev gdevBuf(*this,bmpBuf);

    AlphaBlit(gdevBuf, areaZDest, areaDest, 0xFF, kCIDLib::False);
}


//
//  Does the work to draw a bitmap in a sprite style, i.e. masking it
//  into the background contents of this device's output.
//
//  We assume here that the image itself has been pre-masked with the
//  inverse mask, so that this only needs to be done once, and not repeated
//  every time.
//
//  Note that, unless forcing alpha, we don't need the transparency color
//  if we are getting the masks. But if graphics operations are being
//  remoted, then that needs to be passed to the far side, since we only
//  want them to have to get the image, and create their own masks.
//
//  If forcing alpha, which won't be done in a remoting scenario, we use
//  the transparency color to create our own alpha channel, which we force
//  into a temporary version of the image in a memory bitmap, then we
//  alpha blit that to the target. This insures that the alpha channel
//  info is kept correct.
//
tCIDLib::TVoid
TGraphDrawDev::DrawBitmapMasked(const   TBitmap&            bmpToDraw
                                , const TBitmap&            bmpMask
                                , const TArea&              areaSrc
                                , const TArea&              areaDest
                                , const tCIDLib::TCard4     c4TransClr
                                , const tCIDLib::TBoolean   bForceAlpha)
{
    if (bForceAlpha)
    {
        TBitmap bmpBuf
        (
            areaSrc.szArea()
            , tCIDImage::EPixFmts::TrueAlpha
            , tCIDImage::EBitDepths::Eight
            , kCIDLib::True
        );
        TGraphMemDev gdevBuf(*this, bmpBuf);

        // Draw the image at source size into a temp bitmap
        TArea areaZSrc(areaSrc);
        areaZSrc.ZeroOrg();
        gdevBuf.DrawBitmap(bmpToDraw, areaSrc, areaZSrc);

        //
        //  Use the mask to extract an alpha channel. In this case, it's
        //  going to be a 1 bit off or on type channel, which will get
        //  converted to 0 or 0xFF. We have to draw it to a new bitmap
        //  since the one we get probably doesn't have direct access, and
        //  we can convert it in the process, and we invert it which is
        //  the form we need for our purposes.
        //
        TBitmap bmpAlphaMask
        (
            areaSrc.szArea()
            , tCIDImage::EPixFmts::GrayScale
            , tCIDImage::EBitDepths::Eight
            , kCIDLib::True
        );
        TGraphMemDev gdevAlphaMask(*this, bmpAlphaMask);
        gdevAlphaMask.DrawBitmap
        (
            bmpMask, areaSrc, areaZSrc, tCIDGraphDev::EBmpModes::NotSrcCopy
        );

        //
        //  We can directly use it's pixel array as the alpha channel to
        //  apply.
        //
        bmpBuf.pixaData().ApplyAlphaChannel
        (
            bmpAlphaMask.pixaData()
            , areaSrc
            , TPoint::pntOrigin
            , kCIDLib::True
            , kCIDLib::False
        );

        // Now alpha blit this to the target
        AlphaBlit(gdevBuf, areaZSrc, areaDest, 0xFF, kCIDLib::True);
    }
     else
    {
        DrawBitmap(bmpMask, areaSrc, areaDest, tCIDGraphDev::EBmpModes::SrcAnd);
        DrawBitmap(bmpToDraw, areaSrc, areaDest, tCIDGraphDev::EBmpModes::SrcPaint);
    }
}

tCIDLib::TVoid
TGraphDrawDev::DrawBitmapMasked(const   TBitmap&            bmpToDraw
                                , const TBitmap&            bmpMask
                                , const TArea&              areaSrc
                                , const TPoint&             pntDestUL
                                , const tCIDLib::TCard4     c4TransClr
                                , const tCIDLib::TBoolean   bForceAlpha)
{
    // Just set up the target area and call the other version
    TArea areaDest(pntDestUL, areaSrc.szArea());
    DrawBitmapMasked
    (
        bmpToDraw, bmpMask, areaSrc, areaDest, c4TransClr, bForceAlpha
    );
}

tCIDLib::TVoid
TGraphDrawDev::DrawBitmapMasked(const   TBitmap&            bmpToDraw
                                , const TBitmap&            bmpMask
                                , const TPoint&             pntDestUL
                                , const tCIDLib::TCard4     c4TransClr
                                , const tCIDLib::TBoolean   bForceAlpha)
{
    // Just set up both areas and call the other version
    TArea areaSrc(TPoint::pntOrigin, bmpToDraw.szBitmap());
    TArea areaDest(pntDestUL, areaSrc.szArea());
    DrawBitmapMasked
    (
        bmpToDraw, bmpMask, areaSrc, areaDest, c4TransClr, bForceAlpha
    );
}


//
//  These typically wouldn't be used since they are inefficient, but
//  for one shot deals it's convenient. It does all the masking of the
//  image using temporaries.
//
tCIDLib::TVoid
TGraphDrawDev::DrawBitmapMasked(const   TBitmap&            bmpToDraw
                                , const TArea&              areaSrc
                                , const TArea&              areaDest
                                , const tCIDLib::TCard4     c4TransClr
                                , const tCIDLib::TBoolean   bForceAlpha)
{
    //
    //  Create both mask images and a temp of the image to draw, then
    //  mask off the copy. Then we can do the usual thing.
    //
    TBitmap bmpTmp(bmpToDraw);
    bmpTmp.DeepCopy();

    TBitmap bmpMask(bmpTmp.bmpMakeMask(*this, c4TransClr, kCIDLib::False));
    TBitmap bmpInvMask(bmpTmp.bmpMakeMask(*this, c4TransClr, kCIDLib::True));
    bmpTmp.MaskWith(*this, bmpInvMask);

    if (bForceAlpha)
    {
        // Call other version, which does the alpha thing
        DrawBitmapMasked
        (
            bmpTmp, bmpMask, areaSrc, areaDest, c4TransClr, bForceAlpha
        );
    }
     else
    {
        DrawBitmap(bmpMask, areaSrc, areaDest, tCIDGraphDev::EBmpModes::SrcAnd);
        DrawBitmap(bmpTmp, areaSrc, areaDest, tCIDGraphDev::EBmpModes::SrcPaint);
    }
}

tCIDLib::TVoid
TGraphDrawDev::DrawBitmapMasked(const   TBitmap&            bmpToDraw
                                , const TArea&              areaSrc
                                , const TPoint&             pntDestUL
                                , const tCIDLib::TCard4     c4TransClr
                                , const tCIDLib::TBoolean   bForceAlpha)
{
    // Create the target area and call the other one
    TArea areaDest(pntDestUL, areaSrc.szArea());
    DrawBitmapMasked(bmpToDraw, areaSrc, areaDest, c4TransClr, bForceAlpha);
}

tCIDLib::TVoid
TGraphDrawDev::DrawBitmapMasked(const   TBitmap&            bmpToDraw
                                , const TPoint&             pntDestUL
                                , const tCIDLib::TCard4     c4TransClr
                                , const tCIDLib::TBoolean   bForceAlpha)
{
    // Set up both areas and call the other version
    TArea areaSrc(TPoint::pntOrigin, bmpToDraw.szBitmap());
    TArea areaDest(pntDestUL, areaSrc.szArea());
    DrawBitmapMasked(bmpToDraw, pntDestUL, c4TransClr, bForceAlpha);
}


//
//  Draw a box between the two points (upper left/lower right) with the indicated
//  drawing radius. We assume the passed points are non-inclusive.
//
tCIDLib::TVoid
TGraphDrawDev::DrawBox( const   TPoint&         pntFrom
                        , const TPoint&         pntTo
                        , const tCIDLib::TCard4 c4Rounding)
{
    tCIDLib::TBoolean bRes = kCIDLib::True;
    if (c4Rounding)
    {
        if (!::RoundRect
        (
            hdevThis()
            , pntFrom.i4X()
            , pntFrom.i4Y()
            , pntTo.i4X()
            , pntTo.i4Y()
            , c4Rounding
            , c4Rounding))
        {
            bRes = kCIDLib::False;
        }
    }
     else
    {
        if (!::Rectangle
        (
            hdevThis()
            , pntFrom.i4X()
            , pntFrom.i4Y()
            , pntTo.i4X()
            , pntTo.i4Y()))
        {
            bRes = kCIDLib::False;
        }
    }

    if (!bRes)
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_Box
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}

tCIDLib::TVoid
TGraphDrawDev::DrawBox(const TArea& areaBox, const tCIDLib::TCard4 c4Rounding)
{
    TPoint pntUL, pntLR;
    areaBox.ToPoints(pntUL, pntLR, tCIDLib::ERectlTypes::NonInclusive);
    DrawBox(pntUL, pntLR, c4Rounding);
}


tCIDLib::TVoid TGraphDrawDev::DrawFocusArea(const TArea& areaToDraw)
{
    //
    //  Use a pale gray color to insure good XOR contrast. Save the old one
    //  so we can put it back.
    //
    TCosmeticPen gpenLine(facCIDGraphDev().rgbPaleGrey);
    TPenJanitor janLine(this, &gpenLine);

    tCIDLib::THostRectl rectlDraw;
    areaToDraw.ToRectl(rectlDraw);
    if (!::DrawFocusRect(hdevThis(), (RECT*)&rectlDraw))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_DrawFocus
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , areaToDraw
            );
        }
    }
}


tCIDLib::TVoid TGraphDrawDev::DrawCircle(const  tCIDLib::TCard4 c4Radius
                                        , const TPoint&         pntCenter)
{
    if (c4Radius < 2)
        return;

    const tCIDLib::TInt4 i4Ofs(c4Radius);

    // The bottom/right aren't included, so we go one further there
    const tCIDLib::TInt4 i4Left = pntCenter.i4X() - i4Ofs;
    const tCIDLib::TInt4 i4Top = pntCenter.i4Y() -  i4Ofs;
    const tCIDLib::TInt4 i4Right = pntCenter.i4X() + i4Ofs + 1;
    const tCIDLib::TInt4 i4Bottom = pntCenter.i4Y() + i4Ofs + 1;
    const tCIDLib::TInt4 i4XCenter = pntCenter.i4X();

    if (!::Arc( hdevThis()
                , i4Left
                , i4Top
                , i4Right
                , i4Bottom
                , i4XCenter
                , i4Bottom
                , i4XCenter
                , i4Bottom))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_DrawCircle
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , pntCenter
                , TCardinal(c4Radius)
            );
        }
    }
}


tCIDLib::TVoid
TGraphDrawDev::DrawFilledCircle(const   tCIDLib::TCard4 c4Radius
                                , const TPoint&         pntCenter)
{
    if (c4Radius < 2)
        return;

    const tCIDLib::TInt4 i4Ofs(c4Radius);

    // The bottom/right aren't included, so we go one further there
    const tCIDLib::TInt4 i4Left = pntCenter.i4X() - i4Ofs;
    const tCIDLib::TInt4 i4Top = pntCenter.i4Y() - i4Ofs;
    const tCIDLib::TInt4 i4Right = pntCenter.i4X() + i4Ofs + 1;
    const tCIDLib::TInt4 i4Bottom = pntCenter.i4Y() + i4Ofs + 1;

    if (!::Ellipse( hdevThis()
                    , i4Left
                    , i4Top
                    , i4Right
                    , i4Bottom))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_DrawCircle
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , pntCenter
                , TCardinal(c4Radius)
            );
        }
    }
}


tCIDLib::TVoid
TGraphDrawDev::DrawGradientCircle(  const   tCIDLib::TCard4 c4Radius
                                    , const TPoint&         pntCenter
                                    , const TRGBClr&        rgbLight
                                    , const TRGBClr&        rgbDark)
{
    tCIDGraphDev::TDeviceHandle hdevUs = hdevThis();

    GradientCircleUL(hdevUs, pntCenter, c4Radius, rgbLight.c4Color(), rgbDark.c4Color());
    GradientCircleLR(hdevUs, pntCenter, c4Radius, rgbLight.c4Color(), rgbDark.c4Color());
}


tCIDLib::TVoid TGraphDrawDev::DrawIcon( const   TIcon&  icoToDraw
                                        , const TPoint& pntDestUL)
{
    // Just call the other one, with a default size
    DrawIcon(icoToDraw, pntDestUL, icoToDraw.szBitmap());
}

tCIDLib::TVoid TGraphDrawDev::DrawIcon( const   TIcon&  icoToDraw
                                        , const TPoint& pntDestUL
                                        , const TSize&  szStretchTo)
{
    if (!::DrawIconEx(  hdevThis()
                        , pntDestUL.i4X()
                        , pntDestUL.i4Y()
                        , icoToDraw.hicoThis()
                        , szStretchTo.c4Width()
                        , szStretchTo.c4Height()
                        , 0
                        , 0
                        , DI_NORMAL))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_DrawIcon
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


tCIDLib::TVoid TGraphDrawDev::DrawIcon( const   TIcon&  icoToDraw
                                        , const TArea&  areaToFill)
{
    // Just call the other one, with the org, size passed
    DrawIcon(icoToDraw, areaToFill.pntOrg(), areaToFill.szArea());
}


tCIDLib::TVoid
TGraphDrawDev::DrawIconPart(const   TIcon&              icoToDraw
                            , const TPoint&             pntDestUL
                            , const tCIDGraphDev::EBmpModes  eMode
                            , const tCIDLib::TBoolean   bMask)
{
    DrawIconPart(icoToDraw, pntDestUL, TSize(0,0), eMode, bMask);
}

tCIDLib::TVoid
TGraphDrawDev::DrawIconPart(const   TIcon&              icoToDraw
                            , const TArea&              areaToFill
                            , const tCIDGraphDev::EBmpModes  eMode
                            , const tCIDLib::TBoolean   bMask)
{
    DrawIconPart(icoToDraw, areaToFill.pntOrg(), areaToFill.szArea(), eMode, bMask);
}

tCIDLib::TVoid
TGraphDrawDev::DrawIconPart(const   TIcon&                  icoToDraw
                            , const TPoint&                 pntDestUL
                            , const TSize&                  szStretchTo
                            , const tCIDGraphDev::EBmpModes eMode
                            , const tCIDLib::TBoolean       bMask)
{
    tCIDGraphDev::TDeviceHandle hdevUs = hdevThis();

    ICONINFO Info;
    ::GetIconInfo(icoToDraw.hicoThis(), &Info);

    tCIDGraphDev::TBmpHandle hbmpDraw;
    if (bMask)
    {
        hbmpDraw = Info.hbmMask;
    }
     else
    {
        hbmpDraw = Info.hbmColor;

        //
        //  If there is no color bmp, then its a monochrome with the
        //  two masks in one. So we need to use the mask and use just
        //  the second half of it.
        //
        if (!hbmpDraw)
            hbmpDraw = Info.hbmMask;
    }

    //
    //  Create a memory device from this device and select the bitmap
    //  into it.
    //
    tCIDGraphDev::TDeviceHandle hdevTmp = ::CreateCompatibleDC(hdevUs);
    if (!hdevTmp)
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_CreateMemDev
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );

            // Won't happen, but makes analyzer happy
            return;
        }
    }

    //
    //  Try to select the bitmap into the device. We save the original for
    //  later.
    //
    tCIDGraphDev::TBmpHandle hbmpOld = reinterpret_cast<HBITMAP__*>
    (
        ::SelectObject(hdevTmp, hbmpDraw)
    );

    // Get the bitmap size data
    BITMAPINFO bmiSrc = {0};
    bmiSrc.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    ::GetDIBits(hdevUs, hbmpDraw, 0, 0, 0, &bmiSrc, DIB_RGB_COLORS);

    //
    //  If its a mono, then we have to adjust our origins and size.
    //
    tCIDLib::TInt4 i4OrgX = 0;
    tCIDLib::TInt4 i4OrgY = 0;
    tCIDLib::TInt4 i4CX = bmiSrc.bmiHeader.biWidth;
    tCIDLib::TInt4 i4CY = bmiSrc.bmiHeader.biHeight;

    if (!Info.hbmColor)
    {
        i4CY /= 2;

        if (bMask)
            i4OrgY += i4CY;
    }

    // Ok, now we can blit from the memory device to this device
    BOOL bRes;
    if (szStretchTo.bAllZero())
    {
        bRes = ::BitBlt
        (
            hdevUs
            , pntDestUL.i4X()
            , pntDestUL.i4Y()
            , i4CX
            , i4CY
            , hdevTmp
            , i4OrgX
            , i4OrgY
            , TGrDevPlatform::c4XlatBmpMode(eMode)
        );
    }
     else
    {
        bRes = ::StretchBlt
        (
            hdevUs
            , pntDestUL.i4X()
            , pntDestUL.i4Y()
            , szStretchTo.c4Width()
            , szStretchTo.c4Height()
            , hdevTmp
            , i4OrgX
            , i4OrgY
            , i4CX
            , i4CY
            , TGrDevPlatform::c4XlatBmpMode(eMode)
        );
    }

    // Put back the original bitmap first, to deselect ours
    if (!::SelectObject(hdevTmp, hbmpOld))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_DestroyMemDev
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::CantDo
            );

            // Won't happen, but makes analyzer happy
            return;
        }
    }

    // And now we can delete our tmp device handle
    if (!::DeleteDC(hdevTmp))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_DeleteObj
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::CantDo
                , TString(L"Memory Device")
            );

            // Won't happen, but makes analyzer happy
            return;
        }
    }

    if (!bRes)
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_BitBlit
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


tCIDLib::TVoid
TGraphDrawDev::DrawLine(const   TPoint& pntFrom
                        , const TPoint& pntTo)
{
    MoveTo(pntFrom);
    DrawLine(pntTo);
}

tCIDLib::TVoid
TGraphDrawDev::DrawLine(const   TPoint&     pntFrom
                        , const TPoint&     pntTo
                        , const TRGBClr&    rgbClr)
{
    MoveTo(pntFrom);
    DrawLine(pntTo, rgbClr);
}

tCIDLib::TVoid TGraphDrawDev::DrawLine(const TPoint& pntTo)
{
    // Just call the other version
    DrawLine(pntTo.i4X(), pntTo.i4Y());
}

tCIDLib::TVoid
TGraphDrawDev::DrawLine(const TPoint& pntTo, const TRGBClr& rgbClr)
{
    // Just call the other version
    DrawLine(pntTo.i4X(), pntTo.i4Y(), rgbClr);
}

tCIDLib::TVoid
TGraphDrawDev::DrawLine(const   tCIDLib::TInt4  i4X
                        , const tCIDLib::TInt4  i4Y
                        , const TRGBClr&        rgbClr)
{
    // Set a pen and call the other version
    TCosmeticPen gpenClr(rgbClr);
    TPenJanitor janPen(this, &gpenClr);
    DrawLine(i4X, i4Y);
}

tCIDLib::TVoid
TGraphDrawDev::DrawLine(const   tCIDLib::TInt4  i4X
                        , const tCIDLib::TInt4  i4Y)
{
    // Get a quick ref to the device for efficiency
    tCIDGraphDev::TDeviceHandle hdevTarget = hdevThis();

    if (!::LineTo(hdevTarget, i4X, i4Y))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_DrawLine
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}



//
//  Draws a masked progress bar. This is a high level one, mainly here
//  to support the RIVA system in the CQC layer. Normally a program would
//  do this more efficiently by pre-creating the masked image used, but
//  for RIVA we need to send the high level command to the RIVA client.
//
//  It takes a mask image, a fill color, a percentage, and a fill direction.
//  It will create a temporary bitmap that contains the fill color for
//  the indicated percentage of the bitmap width or height (depending on
//  the fill direction.) It will then blit that image to the indicated
//  target area using the indicated blit operation.
//
//  The fill color is actually two colors, and if they are different we
//  have to do a graduated fill in the indicated direction. There is a
//  third color used to fill any unused area, usually black but not
//  necessarily.
//
//  They also provide a constant opacity to apply if desired.
//
//  So we end up with an image that can be blitted over a background, such
//  that the fill color only shows in the areas desired and to the degree
//  desired (via the alpha channel transparency.) It's used for things
//  like progress bars or thermometers and things like, which need a
//  complex shape fill, often with blurred out edges and whatnot.
//
tCIDLib::TVoid
TGraphDrawDev::DrawPBar(const   TBitmap&                bmpMask
                        , const tCIDLib::TFloat4        f4Percent
                        , const TArea&                  areaSrc
                        , const TArea&                  areaTar
                        , const tCIDGraphDev::EBltTypes eBlt
                        , const tCIDLib::EDirs          eDir
                        , const TRGBClr&                rgbStart
                        , const TRGBClr&                rgbEnd
                        , const TRGBClr&                rgbFill
                        , const tCIDLib::TCard1         c1Opacity)
{
    // We have one so get the size out
    TSize szImg = bmpMask.szBitmap();
    const tCIDLib::TCard4 c4CX = szImg.c4Width();
    const tCIDLib::TCard4 c4CY = szImg.c4Height();

    tCIDLib::TBoolean bValid = kCIDLib::True;
    switch(eDir)
    {
        case tCIDLib::EDirs::Left :
        case tCIDLib::EDirs::Right :
            if (c4CX < 4)
                bValid = kCIDLib::False;
            break;

        case tCIDLib::EDirs::Up :
        case tCIDLib::EDirs::Down :
            if (c4CY < 4)
                bValid = kCIDLib::False;
            break;

        default :
            CIDAssert(0, L"Unknown direction for PBar draw");
            break;
    };

    // It has to be an alpha based image
    if (!bmpMask.bHasAlpha())
        return;

    //
    //  Create a temp bitmap to build our image into. Force it to be 32 bit
    //  and in memory so that we can directly get to the bits. We need this
    //  for efficiency.
    //
    TBitmap bmpFill
    (
        szImg
        , tCIDImage::EPixFmts::TrueAlpha
        , tCIDImage::EBitDepths::Eight
        , kCIDLib::True
    );

    //
    //  Now steal the alpha channel from the original image. It is actually
    //  not guaranteed to be an in-memory, so we need to query the image
    //  data out into a pixel array, then extract the alpha channel into
    //  another.
    //
    TClrPalette palDummy;
    TPixelArray pixaImg;
    bmpMask.QueryImgData(*this, pixaImg, palDummy);
    TPixelArray pixaMask
    (
        tCIDImage::EPixFmts::GrayScale
        , tCIDImage::EBitDepths::Eight
        , tCIDImage::ERowOrders::TopDown
        , szImg
    );
    pixaImg.ExtractAlphaChannel(pixaMask);


    //
    //  OK, now let's build up the fill bar image. Based on the direction of
    //  the fill, we may need to reverse the two colors, and remember if we
    //  are doing vertical or horz. And we need to figure out the two fill
    //  areas.
    //
    tCIDLib::TBoolean bIsVert = kCIDLib::False;
    TArea   areaBar(TPoint::pntOrigin, szImg);
    TArea   areaFill(areaBar);
    TRGBClr rgbStartFill;
    TRGBClr rgbEndFill;
    switch(eDir)
    {
        case tCIDLib::EDirs::Left :
            rgbStartFill = rgbStart;
            rgbEndFill = rgbEnd;
            areaBar.ScaleSize(f4Percent, 1.0);
            areaFill.ScaleSize(1.0 - f4Percent, 1.0);
            areaFill.i4X(areaBar.i4Right());
            break;

        case tCIDLib::EDirs::Right :
            rgbStartFill = rgbEnd;
            rgbEndFill = rgbStart;
            bIsVert = kCIDLib::True;
            areaFill.ScaleSize(1.0 - f4Percent, 1.0);
            areaBar.ScaleSize(f4Percent, 1.0);
            areaBar.i4X(areaFill.i4Right());
            break;

        case tCIDLib::EDirs::Down :
            rgbStartFill = rgbStart;
            rgbEndFill = rgbEnd;
            areaBar.ScaleSize(1.0, f4Percent);
            areaFill.ScaleSize(1.0, 1.0 - f4Percent);
            areaFill.i4Y(areaBar.i4Bottom());
            bIsVert = kCIDLib::True;
            break;

        case tCIDLib::EDirs::Up :
            rgbStartFill = rgbEnd;
            rgbEndFill = rgbStart;
            bIsVert = kCIDLib::True;
            areaFill.ScaleSize(1.0, 1.0 - f4Percent);
            areaBar.ScaleSize(1.0, f4Percent);
            areaBar.i4Y(areaFill.i4Bottom());
            bIsVert = kCIDLib::True;
            break;

        default :
            CIDAssert(0, L"Unknown direction in PBar draw");
            break;
    };

    // If the F1 and F2 colors are not the same, then we have to interpolate
    if (rgbStartFill != rgbEndFill)
    {
        //
        //  We need to interpolate, and do the same sort of flipping as
        //  done above if needed. To interpolate we convert to HSV format
        //  since that makes it easy to maintain the same hue and just
        //  adjust the saturation.
        //
        THSVClr hsvStart(rgbStartFill);
        THSVClr hsvEnd(rgbEndFill);
        const tCIDLib::TFloat4 f4StartSat = hsvStart.f4Saturation();
        tCIDLib::TFloat4 f4EndSat = hsvEnd.f4Saturation();

        if (f4EndSat > f4StartSat)
            f4EndSat = f4StartSat + ((f4EndSat - f4StartSat) * f4Percent);
        else
            f4EndSat = f4StartSat - ((f4StartSat - f4EndSat) * f4Percent);
        hsvEnd.f4Saturation(f4EndSat);

        // Now copy the result back
        hsvEnd.ToRGB(rgbEndFill);
    }

    //
    //  Now we need to do two fills, one with the bar colors and the rest
    //  with the unused portion color, so create a device over our mask
    //  bitmap and do the fills. We need the faux scope because we have
    //  to release the memory device before we try to access the pixel
    //  data.
    //
    //  We have to use a faux scope here since we have to release the
    //  bitmap from us before continuing.
    //
    {
        TGraphMemDev gdevMask(*this, bmpFill);
        gdevMask.GradientFill
        (
            areaBar
            , rgbStartFill
            , rgbEndFill
            , bIsVert ? tCIDGraphDev::EGradDirs::Vertical
                      : tCIDGraphDev::EGradDirs::Horizontal
        );
        gdevMask.Fill(areaFill, rgbFill);
    }

    //
    //  Now we need to reapply the alpha channel we saved earlier, because
    //  the fills above will have whacked it. The last parm tells ut to
    //  pre-multiply the pixels as well, which is required if we are going
    //  to alpha blit the image, which we are.
    //
    TPixelArray& pixaData = bmpFill.pixaData();
    pixaData.CreateAlphaChannelFrom
    (
        pixaMask, TArea(TPoint::pntOrigin, szImg), TPoint::pntOrigin, kCIDLib::True
    );

    //
    //  And, finally, we can draw it. We may only be redrawing a part of it, so we
    //  use the source and target areas. This method handles masked images as well,
    //  and these never are, so we pass a dummy bitmap which will never actually be
    //  accessed since these are full color images, and indicate not color transparent
    //  and pass zero for the transparency color.
    //
    TBitmap bmpDummy;
    bmpFill.AdvDrawOn
    (
        *this
        , areaSrc
        , areaTar
        , kCIDLib::False
        , 0
        , bmpDummy
        , eBlt
        , c1Opacity
    );
}


//
//  Draws a list of connected lines from point to point. So each point is
//  taken as the 'to' point from the previous point (other than the 0th one
//  of course.)
//
tCIDLib::TVoid
TGraphDrawDev::DrawPolyLine(const   TPointArray&    pntaToDraw
                            , const TRGBClr&        rgbClr
                            , const tCIDLib::TCard4 c4Count)
{
    // If 0 or 1 count, do nothing
    if (c4Count < 2)
        return;

    // Set a pen and call the other version
    TCosmeticPen gpenClr(rgbClr);
    TPenJanitor janPen(this, &gpenClr);
    DrawPolyLine(pntaToDraw, c4Count);
}

tCIDLib::TVoid
TGraphDrawDev::DrawPolyLine(const   TPointArray&    pntaToDraw
                            , const tCIDLib::TCard4 c4Count)
{
    // If the count is max card, then use the array count
    tCIDLib::TCard4 c4ActCount = c4Count;
    if (c4ActCount == kCIDLib::c4MaxCard)
        c4ActCount = pntaToDraw.c4Count();

    // If 0 or 1 count, do nothing
    if (c4Count < 2)
        return;

    const POINT* pTmp = reinterpret_cast<const POINT*>(pntaToDraw.aptList());
    if (!::Polyline(hdevThis(), pTmp, c4ActCount))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_PolyLine
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TCardinal(pntaToDraw.c4Count())
            );
        }
    }
}



//
//  Draws a ilst of disconnected line segements. So the array of points has
//  to be an even number, each of which represents a start/end point.
//
tCIDLib::TVoid
TGraphDrawDev::DrawPolySegments(const   TPointArray&    pntaToDraw
                                , const TRGBClr&        rgbClr
                                , const tCIDLib::TCard4 c4Count)
{
    // Set a pen and call the other version
    TCosmeticPen gpenClr(rgbClr);
    TPenJanitor janPen(this, &gpenClr);
    DrawPolySegments(pntaToDraw, c4Count);
}

tCIDLib::TVoid
TGraphDrawDev::DrawPolySegments(const   TPointArray&    pntaToDraw
                                , const tCIDLib::TCard4 c4Count)
{
    // If the count is max card, then use the array count
    tCIDLib::TCard4 c4ActCount = c4Count;
    if (c4ActCount == kCIDLib::c4MaxCard)
        c4ActCount = pntaToDraw.c4Count();

    // We want each segment to be 2 of the points
    const tCIDLib::TCard4 c4SegCount = c4ActCount / 2;
    tCIDLib::TCard4* pc4Counts = new tCIDLib::TCard4[c4SegCount];
    TArrayJanitor<tCIDLib::TCard4> janCounts(pc4Counts);
    TRawMem::SetMemBuf(pc4Counts, tCIDLib::TCard4(2), c4SegCount);

    const POINT* pTmp = reinterpret_cast<const POINT*>(pntaToDraw.aptList());
    if (!::PolyPolyline(hdevThis(), pTmp, pc4Counts, c4SegCount))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_PolyLine
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TCardinal(pntaToDraw.c4Count())
            );
        }
    }
}


//
//  Draw multi-line text with a given justification, within a given area. We
//  have one regular version, and one that is alpha channel aware.
//
tCIDLib::TVoid
TGraphDrawDev::DrawMText(const  TString&            strText
                        , const TArea&              areaFormat
                        , const tCIDLib::EHJustify  eHJustify
                        , const tCIDLib::EVJustify  eVJustify
                        , const tCIDLib::TBoolean   bWordBreak)
{
    // We need this structure for the DrawString call
    DRAWTEXTPARAMS Params = {0};
    Params.cbSize = sizeof(DRAWTEXTPARAMS);

    // Translate the parms into Win32 flags
    DWORD dwFlags = DT_NOPREFIX;
    if (bWordBreak)
        dwFlags |= DT_WORDBREAK;

    //
    //  Create a native rect from the format area. Multi text doesn't support
    //  vertical justification, so we have to do it ourself.
    //
    tCIDLib::THostRectl rectDraw;

    if (eVJustify != tCIDLib::EVJustify::Top)
    {
        TArea areaText = areaMLText(strText, areaFormat.c4Width());
        areaText.JustifyIn(areaFormat, eHJustify, eVJustify);
        areaText.ToRectl(rectDraw);
    }
     else
    {
        areaFormat.ToRectl(rectDraw);
    }

    switch(eHJustify)
    {
        case tCIDLib::EHJustify::Left :
            dwFlags |= DT_LEFT;
            break;

        case tCIDLib::EHJustify::Right :
            dwFlags |= DT_RIGHT;
            break;

        case tCIDLib::EHJustify::Center :
            dwFlags |= DT_CENTER;
            break;
    }

    //
    //  Clip the output to the format area. In the top justified scenario this is a
    //  bit redundant, but it's not worth doing them separately.
    //
    TRegionJanitor janClip(this, areaFormat, tCIDGraphDev::EClipModes::And);

    ::DrawTextEx
    (
        hdevThis()
        , (tCIDLib::TCh*)strText.pszBuffer()
        , -1
        , (RECT*)&rectDraw
        , dwFlags
        , &Params
    );
}

tCIDLib::TVoid
TGraphDrawDev::DrawMTextAlpha(  const   TString&            strText
                                , const TArea&              areaFormat
                                ,       TBitmap&            bmpBuffer
                                ,       TBitmap&            bmpMask
                                , const tCIDLib::EHJustify  eHJustify
                                , const tCIDLib::EVJustify  eVJustify
                                , const tCIDLib::TBoolean   bWordBreak
                                , const tCIDLib::TCard1     c1ConstAlpha)
{
    //
    //  Get the font info currently set on us, so that we can apply the
    //  same font to the local temp devices.
    //
    TFontSelAttrs fselToUse(*this);
    TGUIFont gfontToUse(fselToUse);

    // Get the area at the origin
    TArea areaZFmt = areaFormat;
    areaZFmt.ZeroOrg();

    // Set the mask content
    {
        TGraphMemDev gdevMask(*this, bmpMask);
        gdevMask.Fill(areaZFmt, facCIDGraphDev().rgbBlack);
        gdevMask.SetTextColor(facCIDGraphDev().rgbWhite);
        gdevMask.eBackMixMode(tCIDGraphDev::EBackMixModes::Transparent);
        TFontJanitor janFont(&gdevMask, &gfontToUse);
        gdevMask.DrawMText
        (
            strText, areaZFmt, eHJustify, eVJustify, bWordBreak
        );
    }

    // Now draw the actual text into the buffer
    {
        TGraphMemDev gdevDraw(*this, bmpBuffer);
        gdevDraw.Fill(areaZFmt, facCIDGraphDev().rgbBlack);

        gdevDraw.eBackMixMode(eBackMixMode());
        gdevDraw.SetTextColor(rgbTextColor());
        TFontJanitor janFont(&gdevDraw, &gfontToUse);
        gdevDraw.DrawMText
        (
            strText, areaZFmt, eHJustify, eVJustify, bWordBreak
        );
    }

    // Now apply the alpha mask to the drawn text and blit the result out
    TPixelArray& pixaBuf = bmpBuffer.pixaData();
    TPixelArray& pixaMask = bmpMask.pixaData();

    pixaBuf.CreateAlphaChannelFrom
    (
        pixaMask, areaZFmt, TPoint::pntOrigin, kCIDLib::True
    );
    AlphaBlit(bmpBuffer, areaZFmt, areaFormat, c1ConstAlpha, kCIDLib::True);
}


//
//  Draws text with a drop shadow. This is the built in stuff, which is limited
//  becasue it only does one style of blur for the shadow. We can do better with
//  our own DrawTextFX method, but it's a lot heavier, and mostly only for graphics
//  output oriented programs. This is for use in standard control windows and
//  such.
//
tCIDLib::TVoid
TGraphDrawDev::DrawShadowText(const TString&            strText
                            , const TArea&              areaFormat
                            , const TPoint&             pntShadowOfs
                            , const TRGBClr&            rgbShadowClr
                            , const tCIDLib::EHJustify  eHJustify
                            , const tCIDLib::EVJustify  eVJustify)
{
    #if 0
    static tCIDLib::TBoolean bLoad = kCIDLib::False;
    if (!bLoad)
    {
        bLoad = kCIDLib::True;
        INITCOMMONCONTROLSEX InitCC = {};
        InitCC.dwSize = sizeof(INITCOMMONCONTROLSEX);
        InitCC.dwICC = ICC_STANDARD_CLASSES;
        InitCommonControlsEx(&InitCC);
    }
    #endif

    tCIDLib::THostRectl rectlText;
    areaFormat.ToRectl(rectlText);
    ::DrawShadowText
    (
        hdevThis()
        , strText.pszBuffer()
        , strText.c4Length()
        , (RECT*)&rectlText
        , 0
        , rgbTextColor().c4Color()
        , rgbShadowClr.c4Color()
        , pntShadowOfs.i4X()
        , pntShadowOfs.i4Y()
    );
}



//
//  Draw a single line of text, no wrapping supported. Uses the currently
//  set font. It can be justified within a passed area, and clipped to
//  that area, or just drawn from a starting point. It can provide a color
//  or use the currently set text color.
//
tCIDLib::TVoid
TGraphDrawDev::DrawString(const TString& strText, const TPoint& pntAlign)
{
    DrawString(strText, 0, pntAlign);
}


tCIDLib::TVoid
TGraphDrawDev::DrawString(  const   TString&        strText
                            , const tCIDLib::TCard4 c4StartAt
                            , const TPoint&         pntAlign)
{
    DrawString(strText, c4StartAt, strText.c4Length(), pntAlign);
}

tCIDLib::TVoid
TGraphDrawDev::DrawString(  const   TString&        strText
                            , const tCIDLib::TCard4 c4StartAt
                            , const tCIDLib::TCard4 c4Len
                            , const TPoint&         pntAlign)
{
    // If its empty, or the start is at the end, then we are done
    if (!c4Len || (c4StartAt == c4Len))
        return;

    // Sanity check the start index
    if (c4StartAt > strText.c4Length())
    {
        // NOTE: We are throwing a CIDLib error, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TCardinal(c4StartAt)
            , clsThis()
            , TCardinal(strText.c4Length())
        );
    }

    //
    //  This simple version just draws at the current position, using the
    //  current text attributes.
    //
    if (!::TextOut(hdevThis(), pntAlign.i4X(), pntAlign.i4Y(), strText.pszBufferAt(c4StartAt), c4Len))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_DrawString
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


tCIDLib::TVoid
TGraphDrawDev::DrawString(const TString&                strText
                        , const TArea&                  areaFormat
                        , const tCIDLib::EHJustify      eHJustify
                        , const tCIDLib::EVJustify      eVJustify
                        , const tCIDGraphDev::ETextFmts eFormat
                        , const TRGBClr&                rgbBgn)
{
    DrawString
    (
        strText
        , 0
        , kCIDLib::c4MaxCard
        , areaFormat
        , eHJustify
        , eVJustify
        , eFormat
        , rgbBgn
    );
}

tCIDLib::TVoid
TGraphDrawDev::DrawString(  const   TString&                strText
                            , const tCIDLib::TCard4         c4StartAt
                            , const tCIDLib::TCard4         c4Len
                            , const TArea&                  areaFormat
                            , const tCIDLib::EHJustify      eHJustify
                            , const tCIDLib::EVJustify      eVJustify
                            , const tCIDGraphDev::ETextFmts eFormat
                            , const TRGBClr&                rgbBgn)
{
    //
    //  Get the part of the string out that we need to display. If the
    //  start or length are bad, this will catch it. After this, we deal
    //  totally with the copy.
    //
    TString strCopy;
    strCopy.CopyInSubStr(strText, c4StartAt, c4Len);

    // Save the device state so we can modify internally as needed
    TGraphicDevJanitor janSave(this);

    //
    //  Find the mnemonic character if any, remember the index of it, and
    //  then remove it from the string copy. If there is not one, then set
    //  the index to c4MaxCard to indicate there is not one.
    //
    //  !!Be sure to do this before getting the area of the string! Otherwise
    //  the area will include the mnemonic character!!
    //
    tCIDLib::TCard4 c4MnemonicIndex = kCIDLib::c4MaxCard;
    if (tCIDLib::bAllBitsOn(eFormat, tCIDGraphDev::ETextFmts::Mnemonic))
    {
        if (strCopy.bFirstOccurrence(kCIDGraphDev::chMnemonic, c4MnemonicIndex))
            strCopy.Cut(c4MnemonicIndex, 1);
    }

    //
    //  Find out the area that we need for the text. Note that we MUST
    //  do this before we put in the host specific mnemonic character,
    //  which is done below!
    //
    TArea areaOfText(areaString(strCopy));

    // Now justify this in the format area as requested
    areaOfText.JustifyIn(areaFormat, eHJustify, eVJustify);

    // If we got a background fill color, then do the fill
    if (!TRGBClr::bIsNullObject(rgbBgn))
        Fill(areaFormat, rgbBgn);

    //
    //  If we have a mnemonic character, then calculate the points of the
    //  underline line that we will use.
    //
    TPoint pntMnemonic1;
    TPoint pntMnemonic2;
    if (c4MnemonicIndex != kCIDLib::c4MaxCard)
    {
        tCIDLib::TInt4  i4Ofs;
        tCIDLib::TCard4 c4Width;
        QueryCharOffsets(strCopy.pszBuffer(), c4MnemonicIndex, i4Ofs, c4Width);
        pntMnemonic1.Set(areaOfText.i4X() + i4Ofs, areaOfText.i4Bottom());
        pntMnemonic2.Set
        (
            areaOfText.i4X()
            + i4Ofs
            + tCIDLib::TInt4(c4Width)
            , areaOfText.i4Bottom()
        );
    }

    //
    //  If disabled, then we need to do a preliminary draw using white as the
    //  backing color, and over and down one pixel.
    //
    if (tCIDLib::bAllBitsOn(eFormat, tCIDGraphDev::ETextFmts::Disabled))
    {
        SetTextColor(facCIDGraphDev().rgbWhite);
        areaOfText.AdjustOrg(1, 1);
    }

    //
    //  Use the 'conditional janitor' version of the region janitor. It will
    //  do nothing if its last parameter is false. Else it will create a
    //  region from the passed area and set it.
    //
    //  We use the AND mode, so it will be in addition to any other clipping
    //  that is currently set.
    //
    TRegionJanitor janClip
    (
        this
        , areaFormat
        , tCIDGraphDev::EClipModes::And
        , !tCIDLib::bAnyBitsOn(eFormat, tCIDGraphDev::ETextFmts::NoClip)
    );

    // And now finally draw the text
    if (!::TextOutW
    (
        hdevThis()
        , areaOfText.i4X()
        , areaOfText.i4Y()
        , strCopy.pszBuffer()
        , strCopy.c4Length()))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_DrawString
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }

    // If we have a mnemonic char, then draw the underline
    if (c4MnemonicIndex != kCIDLib::c4MaxCard)
    {
        TCosmeticPen gpenLine(rgbTextColor());
        TPenJanitor janLine(this, &gpenLine);
        DrawLine(pntMnemonic1, pntMnemonic2);
    }

    //
    //  If we are disabled, then we can now redraw the whole thing using a dark gray,
    //  up and left by one pixel to give a double drop effect.
    //
    if (tCIDLib::bAllBitsOn(eFormat, tCIDGraphDev::ETextFmts::Disabled))
    {
        // Update the text color
        SetTextColor(facCIDGraphDev().rgbDarkGrey);

        // And draw the text again, back in the original position
        areaOfText.AdjustOrg(-1, -1);
        if (!::TextOutW
        (
            hdevThis()
            , areaOfText.i4X()
            , areaOfText.i4Y()
            , strCopy.pszBuffer()
            , strCopy.c4Length()))
        {
            if (!bDevErrToIgnore())
            {
                facCIDGraphDev().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kGrDevErrs::errcDev_DrawString
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }
        }

        //
        //  And, once again, draw the underline using the same color as
        //  the text
        //
        if (c4MnemonicIndex != kCIDLib::c4MaxCard)
        {
            TCosmeticPen gpenLine(rgbTextColor());
            TPenJanitor janLine(this, &gpenLine);
            DrawLine(pntMnemonic1 - TPoint(1,1), pntMnemonic2 - TPoint(1,1));
        }
    }
}


//
//  The caller has to provide bitmaps to use for this one. We don't provide a
//  version that doesn't take them (and calls this one) because we'd have to
//  calculate the text area twice to do that. They have to be true alpha and
//  memory based.
//
tCIDLib::TVoid
TGraphDrawDev::DrawStringAlpha( const   TString&            strText
                                , const tCIDLib::TCard4     c4StartAt
                                , const TPoint&             pntAlign
                                ,       TBitmap&            bmpBuffer
                                ,       TBitmap&            bmpMask
                                , const tCIDLib::TCard1     c1ConstAlpha)
{
    //
    //  Get the font info currently set on us, so that we can apply the
    //  same font to the local temp devices.
    //
    TFontSelAttrs fselToUse(*this);
    TGUIFont gfontToUse(fselToUse);

    // Get the area of the text in our font, and move to the target point
    TArea areaText(areaString(strText, c4StartAt));
    areaText.SetOrg(pntAlign);

    // Get the area at the origin
    TArea areaZText= areaText;
    areaZText.ZeroOrg();

    // Draw the text into the mask in white
    {
        TGraphMemDev gdevMask(*this, bmpMask);
        gdevMask.eBackMixMode(tCIDGraphDev::EBackMixModes::Transparent);
        gdevMask.Fill(areaZText, facCIDGraphDev().rgbBlack);
        gdevMask.SetTextColor(facCIDGraphDev().rgbWhite);
        TFontJanitor janFont(&gdevMask, &gfontToUse);
        gdevMask.DrawString(strText, c4StartAt, TPoint::pntOrigin);
    }

    // Draw the text into the buffer, using our settings
    {
        TGraphMemDev gdevDraw(*this, bmpBuffer);
        gdevDraw.eBackMixMode(eBackMixMode());
        gdevDraw.SetTextColor(rgbTextColor());
        gdevDraw.Fill(areaZText, facCIDGraphDev().rgbBlack);
        TFontJanitor janFont(&gdevDraw, &gfontToUse);
        gdevDraw.DrawString(strText, c4StartAt, TPoint::pntOrigin);
    }

    // now apply the mask to the buffer and premultiply as we do it
    TPixelArray& pixaBuf = bmpBuffer.pixaData();
    TPixelArray& pixaMask = bmpMask.pixaData();
    pixaBuf.CreateAlphaChannelFrom
    (
        pixaMask, areaZText, TPoint::pntOrigin, kCIDLib::True
    );

    // And now alpha blit this to the correct location on us
    AlphaBlit(bmpBuffer, areaZText, areaText, c1ConstAlpha, kCIDLib::True);
}


tCIDLib::TVoid
TGraphDrawDev::DrawStringAlpha( const   TString&                strText
                                , const TArea&                  areaFormat
                                ,       TBitmap&                bmpBuffer
                                ,       TBitmap&                bmpMask
                                , const tCIDLib::EHJustify      eHJustify
                                , const tCIDLib::EVJustify      eVJustify
                                , const tCIDGraphDev::ETextFmts eFormat
                                , const tCIDLib::TCard1         c1ConstAlpha
                                , const TRGBClr&                rgbBgn)
{
    DrawStringAlpha
    (
        strText
        , 0
        , kCIDLib::c4MaxCard
        , areaFormat
        , bmpBuffer
        , bmpMask
        , eHJustify
        , eVJustify
        , eFormat
        , c1ConstAlpha
        , rgbBgn
    );
}

tCIDLib::TVoid
TGraphDrawDev::DrawStringAlpha( const   TString&                strText
                                , const tCIDLib::TCard4         c4StartAt
                                , const tCIDLib::TCard4         c4Len
                                , const TArea&                  areaFormat
                                ,       TBitmap&                bmpBuffer
                                ,       TBitmap&                bmpMask
                                , const tCIDLib::EHJustify      eHJustify
                                , const tCIDLib::EVJustify      eVJustify
                                , const tCIDGraphDev::ETextFmts eFormat
                                , const tCIDLib::TCard1         c1ConstAlpha
                                , const TRGBClr&                rgbBgn)
{
    //
    //  Get the font info currently set on us, so that we can apply the
    //  same font to the local temp devices.
    //
    TFontSelAttrs fselToUse(*this);
    TGUIFont gfontToUse(fselToUse);

    // Get the area at the origin
    TArea areaZFmt = areaFormat;
    areaZFmt.ZeroOrg();

    // Draw the text into the mask in white
    {
        TGraphMemDev gdevMask(*this, bmpMask);
        gdevMask.eBackMixMode(tCIDGraphDev::EBackMixModes::Transparent);
        gdevMask.Fill(areaZFmt, facCIDGraphDev().rgbBlack);
        gdevMask.SetTextColor(facCIDGraphDev().rgbWhite);
        TFontJanitor janFont(&gdevMask, &gfontToUse);
        gdevMask.DrawString
        (
            strText
            , c4StartAt
            , c4Len
            , areaZFmt
            , eHJustify
            , eVJustify
            , eFormat
            , rgbBgn
        );
    }

    // Draw the text into the buffer, using our settings
    {
        TGraphMemDev gdevDraw(*this, bmpBuffer);
        gdevDraw.eBackMixMode(eBackMixMode());
        gdevDraw.SetTextColor(rgbTextColor());
        gdevDraw.Fill(areaZFmt, facCIDGraphDev().rgbBlack);
        TFontJanitor janFont(&gdevDraw, &gfontToUse);
        gdevDraw.DrawString
        (
            strText
            , c4StartAt
            , c4Len
            , areaZFmt
            , eHJustify
            , eVJustify
            , eFormat
            , rgbBgn
        );
    }

    // now apply the mask to the buffer and premultiply as we do it
    TPixelArray& pixaBuf = bmpBuffer.pixaData();
    TPixelArray& pixaMask = bmpMask.pixaData();
    pixaBuf.CreateAlphaChannelFrom
    (
        pixaMask, areaZFmt, TPoint::pntOrigin, kCIDLib::True
    );

    // And now alpha blit this to the correct location on us
    AlphaBlit(bmpBuffer, areaZFmt, areaFormat, c1ConstAlpha, kCIDLib::True);

}


//
//  This method provides the support some some fancy text drawing effects.
//  We assume the desired font has been set on this device before we get
//  called.
//
tCIDLib::TVoid
TGraphDrawDev::DrawStringFX(const   TString&                strText
                            , const TArea&                  areaDraw
                            , const tCIDGraphDev::ETextFX  eEffect
                            , const TRGBClr&                rgbClr1
                            , const TRGBClr&                rgbClr2
                            , const tCIDLib::EHJustify      eHJustify
                            , const tCIDLib::EVJustify      eVJustify
                            , const tCIDLib::TBoolean       bNoTextWrap
                            , const TPoint&                 pntOffset
                            ,       TBitmap&                bmpBuffer
                            ,       TBitmap&                bmpMask
                            ,       TPixelArray&            pixaBlur
                            , const tCIDLib::TCard1         c1ConstAlpha)
{
    // If no text, then nothing to do
    if (strText.bIsEmpty())
        return;

    // Remember if this is a blur type effect
    const tCIDLib::TBoolean bBlurType
    (
        (eEffect == tCIDGraphDev::ETextFX::GaussianBlur)
        || (eEffect == tCIDGraphDev::ETextFX::DropShadow)
    );

    //
    //  Calculate the size of the text, in the case of multi-line, given the
    //  the width of the drawing area.
    //
    TArea areaText;
    if (bNoTextWrap)
        areaText = areaString(strText);
    else
        areaText = areaMLText(strText, areaDraw.c4Width());

    //
    //  Decide how big our bitmap will be. It's the text smaller of the text area
    //  and the drawing area, plus 8 to allow for any blurring if we are doing a
    //  blurring type effect.
    //
    TArea areaBmp(areaText);
    areaBmp.TakeSmaller(areaDraw);
    areaBmp.ZeroOrg();
    if (bBlurType)
        areaBmp.AdjustSize(10, 10);

    //
    //  Our zero based target area is the smaller of the drawing area and the text
    //  centered in the bitmap. This is the area we'll draw to, and this leaves
    //  room on all sides for blurring.
    //
    TArea areaZTar(areaDraw);
    areaZTar.TakeSmaller(areaText);
    areaZTar.CenterIn(areaBmp);

    //
    //  Then use the passed jusifications to position the text relative to our
    //  zero target. This will make the correct part of the text fall into the
    //  target area. The rest will be clippe away.
    //
    TArea areaZText(areaText);
    areaZText.JustifyIn(areaZTar, eHJustify, eVJustify);

    //
    //  If we have to do a reflection, this is how many rows in the reflection.
    //  It's a percentage of the height of the text, so that some portion of the
    //  text height is reflected.
    //
    const tCIDLib::TCard4 c4ReflRows = tCIDLib::TCard4
    (
        areaText.c4Height() * 0.65
    );

    //
    //  We have a further complication if a drop shadow or reflection and
    //  there's an offset. If the offset is in the direction of the
    //  justification, then we cannot push the drop shadow in that direction.
    //  It would go out of the area. Instead we have to move the regular text
    //  in the inverse direction.
    //
    //  If the offset is in the opposite direction of the justification (or
    //  the justification is centered), then we can just move the regular
    //  text. So we set up an offset for each of them, either of which might
    //  be zero in one or both directions.
    //
    //  Note that for the reflection scenarios, negative vertical offsets are
    //  not an issue, since the reflection is always under the text and can
    //  only move downwards. And we use the opposite points in that case.
    //
    //  And, in the reflection scenario, if bottom jusified, we'll always
    //  move the text upwards by the number of reflection lines so that the
    //  reflection ends up bottom justifed (on top of any offset.)
    //
    TPoint pntShadowOfs;
    TPoint pntShadowTxtOfs;
    if (eEffect == tCIDGraphDev::ETextFX::DropShadow)
    {
        if (((pntOffset.i4X() < 0) && (eHJustify == tCIDLib::EHJustify::Left))
        ||  ((pntOffset.i4X() > 0) && (eHJustify == tCIDLib::EHJustify::Right)))
        {
            // Move the text the other way
            pntShadowTxtOfs.i4X(pntOffset.i4X() * -1);
        }
         else
        {
            // No special case, so the shadow can move
            pntShadowOfs.i4X(pntOffset.i4X());
        }

        if (((pntOffset.i4Y() < 0) && (eVJustify == tCIDLib::EVJustify::Top))
        ||  ((pntOffset.i4Y() > 0) && (eVJustify == tCIDLib::EVJustify::Bottom)))
        {
            pntShadowTxtOfs.i4Y(pntOffset.i4Y() * -1);
        }
         else
        {
            // No special case, so the shadow can move
            pntShadowOfs.i4Y(pntOffset.i4Y());
        }
    }
     else if ((eEffect == tCIDGraphDev::ETextFX::Reflected)
          ||  (eEffect == tCIDGraphDev::ETextFX::GradientRefl))
    {
        if (((pntOffset.i4X() < 0) && (eHJustify == tCIDLib::EHJustify::Left))
        ||  ((pntOffset.i4X() > 0) && (eHJustify == tCIDLib::EHJustify::Right)))
        {
            //
            //  Move the 'shadow' the other way, actually the real text in
            //  this case.
            //
            pntShadowOfs.i4X(pntOffset.i4X() * -1);
        }
         else
        {
            // No special case, so the reflection can move
            pntShadowTxtOfs.i4X(pntOffset.i4X());
        }

        if (eVJustify == tCIDLib::EVJustify::Bottom)
        {
            //
            //  Move the 'shadow' the other way, actually the real text
            //  in this case.
            //
            pntShadowOfs.i4Y(pntOffset.i4Y() * -1);
            pntShadowOfs.AdjustY(tCIDLib::TInt4(c4ReflRows) * -1);
        }
         else
        {
            // No special case, so the reflection can move
            pntShadowTxtOfs.i4Y(pntOffset.i4Y());
        }
    }

    TGraphMemDev gdevDraw(*this, bmpBuffer);
    TGraphMemDev gdevMask(*this, bmpMask);

    //
    //  Clean out the part of the bitmap we'll use. We clear out the bitmap
    //  area, which is a bit larger than the usable part of the text if doing
    //  a blurring effect, so we'll clear out space for that as well.
    //
    {
        TArea areaClear(areaBmp);
        gdevDraw.Fill(areaClear, facCIDGraphDev().rgbBlack);
        gdevMask.Fill(areaClear, facCIDGraphDev().rgbBlack);
    }

    //
    //  Get a copy of the font handle of the our device so that we can set
    //  the same font on our mask device.
    //
    TFontSelAttrs fselToUse(*this);
    TGUIFont gfontToUse(fselToUse);

    //
    //  Let's create our alpha mask now. We draw the text into the mask, clipping
    //  the output to the Z target area. This insures that only the text we care
    //  about ends up in the bitmap, the bit that falls in the center of the the
    //  bitmap area.
    //
    {
        // Set our font on the mask and set up for transparent bgn
        TFontJanitor janFont(&gdevMask, &gfontToUse);

        gdevMask.eBackMixMode(tCIDGraphDev::EBackMixModes::Transparent);
        gdevMask.SetTextColor(facCIDGraphDev().rgbWhite);

        //
        //  And draw the text into the bitmap. For the no text wrap, the default
        //  left/center justification is fine. The area is the calculated text
        //  size, so it will fit exactly. For multi-line, take the horz just
        //  because that controls how line ends are lined up, but center it
        //  vertically though it should make no difference sine the target area
        //  is the calculated text height.
        //
        TRegionJanitor janClip(this, areaZTar, tCIDGraphDev::EClipModes::And);
        if (bNoTextWrap)
        {
            gdevMask.DrawString(strText, areaZText);
        }
         else
        {
            gdevMask.DrawMText
            (
                strText
                , areaZText
                , eHJustify
                , tCIDLib::EVJustify::Center
                , kCIDLib::True
            );
        }
    }

    //
    //  Now, fill with the appropriate colors. If doing a gradient, we only fill
    //  the actual text area, since we want to get the most gradient across the
    //  actual text, and in this case there's no blurring anyway, so the text is
    //  in the z target area completely.
    //
    //  For drop shadow or gausian blur, fill the whole  bitmap area, to insure
    //  that it's underneath any blurring.
    //
    if ((eEffect == tCIDGraphDev::ETextFX::Gradient)
    ||  (eEffect == tCIDGraphDev::ETextFX::GradientRefl))
    {
        gdevDraw.GradientFill
        (
            areaZTar, rgbClr1, rgbClr2, tCIDGraphDev::EGradDirs::Vertical
        );
    }
     else if (eEffect == tCIDGraphDev::ETextFX::DropShadow)
    {
        gdevDraw.Fill(areaBmp, rgbClr2);
    }
     else if (eEffect == tCIDGraphDev::ETextFX::GaussianBlur)
    {
        gdevDraw.Fill(areaBmp, rgbClr1);
    }

    //
    //  And apply the mask we created to the drawing area. We do the bitmap area.
    //  This is the area that was cleared and that was filled in above. The mask
    //  is centered in this.
    //
    TPixelArray& pixaDraw = bmpBuffer.pixaData();
    TPixelArray& pixaMask = bmpMask.pixaData();
    pixaDraw.ApplyAlphaChannel
    (
        pixaMask, areaBmp, TPoint::pntOrigin, kCIDLib::True, kCIDLib::False
    );

    //
    //  Blur the bitmap if it's a blur type. We blur the bitmap area. The caller
    //  has to pass us a blur object.
    //
    //  Since the text we kept is centered in the bitmap area, we insure that we
    //  get good blurring all around.
    //
    if (bBlurType)
    {
        //
        //  We use one of the special blur orders that are used for creating faster
        //  blurs of a reasonable size, either 1 or 2. We try to adjust this based
        //  on font size, so that we use 1 for fonts under a certain size. This keeps
        //  the blur more in proportion to the font.
        //
        tCIDLib::TCard4 c4Order = 2;
        TEXTMETRICW OurMetrics;
        if (::GetTextMetrics(hdevThis(), &OurMetrics))
        {
            if (OurMetrics.tmHeight < 20)
                c4Order = 1;
        }

        pixaDraw.GaussianBlur
        (
            c4Order, areaBmp.c4Width(), areaBmp.c4Height(), &pixaBlur
        );
    }

    //
    //  Set a clip region so that we don't draw outside our drawing area from
    //  here on out.
    //
    TRegionJanitor janClip(this, areaDraw, tCIDGraphDev::EClipModes::And);

    //
    //  Create a source area we'll use below. This is either the drawing area, or
    //  the bitmap area if the drawing area is larger, centered on the bitmap
    //  area. This insures that we get our blur info, even if it is clipped in
    //  the end.
    //
    TArea areaSrc = areaBmp;

    TArea areaTar(areaSrc);
    areaTar.JustifyIn(areaDraw, eHJustify, eVJustify);

    //
    //  Note that we have one special case here. If bottom justifed, and it's a
    //  reflection scenario, we have to include the reflected text, which we will
    //  draw below, in the vertical justification. It wasn't of course included
    //  in the original text height calculation.
    //
    const tCIDLib::TBoolean bReflect
    (
        ((eEffect == tCIDGraphDev::ETextFX::GradientRefl)
        || (eEffect == tCIDGraphDev::ETextFX::Reflected))
        && (c4ReflRows > 4)
    );

    if (bReflect && (eVJustify == tCIDLib::EVJustify::Bottom))
    {
        tCIDLib::TInt4 i4RefAdjust = tCIDLib::TInt4(c4ReflRows);
        areaTar.AdjustOrg(0, -(i4RefAdjust + pntOffset.i4Y()));
    }

    //
    //  Blit the image to the target area, adjusted by the shadow offset if this
    //  is a blur type. If a blur type, lower the constant opacity a bit to cover
    //  the fact that we are doing a fast and sort of rough blur.
    //
    {
        TArea areaFX(areaTar);
        if (bBlurType)
            areaFX += pntShadowOfs;

        AlphaBlit
        (
            gdevDraw
            , areaSrc
            , areaFX
            , bBlurType ? 0xB0 : 0xFF
            , kCIDLib::True
        );
    }

    //
    //  If they want it reflected, then we need to flip the image in the
    //  horizontal axis (i.e. make it upside down), and then apply an
    //  alpha gradient to it to make it go to completely transparent by
    //  halfway down.
    //
    if (bReflect)
    {
        pixaDraw.FlipVertically(areaZTar.i4Y(), areaZTar.i4Bottom());

        // Tell it to pre-multiply as it goes
        pixaDraw.ScaleAlpha
        (
            tCIDLib::EDirs::Down, areaZTar.i4Y(), c4ReflRows, kCIDLib::True
        );

        TArea areaReflSrc(areaZTar);
        areaReflSrc.c4Height(c4ReflRows);

        TArea areaReflTar(areaReflSrc);
        areaReflTar.i4X(areaTar.i4X() + areaZTar.i4X());

        // Put it at the bottom of the main text
        areaReflTar.i4Y(areaTar.i4Y() + areaZTar.i4Bottom());

        // And adjust for shadow offset
        areaReflTar += pntOffset;

        AlphaBlit(gdevDraw, areaReflSrc, areaReflTar, 0xFF, kCIDLib::True);
    }

    //
    //  If doing a drop shadow, we can now just draw the text normally over
    //  the top of the blur.
    //
    if (eEffect == tCIDGraphDev::ETextFX::DropShadow)
    {
        gdevDraw.Fill(areaBmp, facCIDGraphDev().rgbBlack);
        pixaDraw.SetAllAlpha(0);

        TGUIFont gfontDraw(fselToUse);
        TFontJanitor janDFont(&gdevDraw, &gfontDraw);

        gdevDraw.eBackMixMode(tCIDGraphDev::EBackMixModes::Transparent);
        gdevDraw.SetTextColor(rgbClr1);

        {
            TRegionJanitor janClip2(this, areaZTar, tCIDGraphDev::EClipModes::And);
            if (bNoTextWrap)
            {
                gdevDraw.DrawString(strText, areaZText);
            }
             else
            {
                gdevDraw.DrawMText
                (
                    strText
                    , areaZText
                    , eHJustify
                    , tCIDLib::EVJustify::Center
                    , kCIDLib::True
                );
            }
        }

        pixaDraw.ApplyAlphaChannel
        (
            pixaMask, areaBmp, TPoint::pntOrigin, kCIDLib::True, kCIDLib::False
        );

        // If there's an offset, then apply that
        areaTar.AdjustOrg(pntShadowTxtOfs);

        // We seem to work best with a slight extra
        areaTar.AdjustOrg(-1, -1);

        AlphaBlit(gdevDraw, areaSrc, areaTar, 0xFF, kCIDLib::True);
    }
}


// Get/set the background mix mode
tCIDGraphDev::EBackMixModes TGraphDrawDev::eBackMixMode() const
{
    return TGrDevPlatform::eXlatBackMixMode(::GetBkMode(hdevThis()));
}

tCIDGraphDev::EBackMixModes
TGraphDrawDev::eBackMixMode(const tCIDGraphDev::EBackMixModes eToSet)
{
    if (!::SetBkMode(hdevThis(), TGrDevPlatform::c4XlatBackMixMode(eToSet)))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_SetBackMixMode
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TCardinal(tCIDLib::c4EnumOrd(eToSet))
            );
        }
    }
    return eToSet;
}


// Get/set the foreground mix mode
tCIDGraphDev::EMixModes TGraphDrawDev::eMixMode() const
{
    return TGrDevPlatform::eXlatMixMode(::GetROP2(hdevThis()));
}

tCIDGraphDev::EMixModes TGraphDrawDev::eMixMode(const tCIDGraphDev::EMixModes eToSet)
{
    if (!::SetROP2(hdevThis(), TGrDevPlatform::c4XlatMixMode(eToSet)))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_SetMixMode
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TCardinal(tCIDLib::c4EnumOrd(eToSet))
            );
        }
    }
    return eToSet;
}


tCIDLib::TVoid TGraphDrawDev::EnableBoundsAccumulation()
{
    if (!(::SetBoundsRect(hdevThis(), 0, DCB_ENABLE) & DCB_ENABLE))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_EnableBounds
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


// End a path and set it as the current path on the device
tCIDLib::TVoid TGraphDrawDev::EndPath()
{
    if (!::EndPath(hdevThis()))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_EndPath
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


// Push a new font onto this device
tCIDGraphDev::TFontHandle TGraphDrawDev::hfontSetFont(const TGUIFont& gfontToSet)
{
    // Select our font and get the old one back
    tCIDGraphDev::TFontHandle hfontOld = reinterpret_cast<tCIDGraphDev::TFontHandle>
    (
        ::SelectObject(hdevThis(), gfontToSet.hfontThis())
    );

    if (!hfontOld)
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
            , TString(L"Font")
        );
    }

    return hfontOld;
}


// Push a new pen onto this device
tCIDGraphDev::TPenHandle TGraphDrawDev::hpenSetPen(TGUIPen& gpenNew)
{
    //
    //  If we are currently selected into a device, then this is an illegal
    //  double selection.
    //
    if (gpenNew.bSetOnDevice())
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_AlreadySelected
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TString(L"Pen")
        );
    }

    tCIDGraphDev::TPenHandle hpenOld = reinterpret_cast<tCIDGraphDev::TPenHandle>
    (
        ::SelectObject(hdevThis(), gpenNew.hpenThis())
    );

    if (!hpenOld)
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
            , TString(L"Pen")
        );
    }

    // It worked, so mark it as selected and return the old pen
    gpenNew.bSetOnDevice(kCIDLib::True);
    return hpenOld;
}



//
//  Set a clipping area or region on this device. Returns any previous
//  clipping region. If there was none, it returns hte invalid handle
//  value.
//
tCIDGraphDev::TRegionHandle
TGraphDrawDev::hrgnSetClipArea( const   tCIDGraphDev::EClipModes    eMode
                                , const TArea&                      areaToSet
                                , const tCIDLib::TCard4             c4Rounding)
{
    // Get the current region so that we can return it to the caller
    tCIDGraphDev::TRegionHandle hrgnOld = ::CreateRectRgn(0, 0, 0, 0);
    if (hrgnOld == kCIDGraphDev::hrgnInvalid)
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

    tCIDGraphDev::TDeviceHandle hdevTarget = hdevThis();
    int iRes = ::GetClipRgn(hdevTarget, hrgnOld);

    // IT failed, so clean up the region and throw
    if (iRes == -1)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        ::DeleteObject(hrgnOld);
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

    // If there was no clip region, then return an invalid handle
    if (iRes == 0)
    {
        ::DeleteObject(hrgnOld);
        hrgnOld = kCIDGraphDev::hrgnInvalid;
    }

    // Create a temp region based on the passed info
    tCIDGraphDev::TRegionHandle hrgnTmp = ::CreateRoundRectRgn
    (
        areaToSet.i4X()
        , areaToSet.i4Y()
        , areaToSet.i4Right() + 1
        , areaToSet.i4Bottom() + 1
        , c4Rounding
        , c4Rounding
    );

    // If we can't do that, clean up the old region and throw
    if (!hrgnTmp)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        if (hrgnOld != kCIDGraphDev::hrgnInvalid)
            ::DeleteObject(hrgnOld);

        TString strMsg(L"rounded rect region (");
        strMsg.AppendFormatted(areaToSet);
        strMsg.Append(L")");
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_CantCreate
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strMsg
        );
    }

    // Now set it on us
    const tCIDLib::TCard4 c4Mode = TGrDevPlatform::c4XlatClipMode(eMode);
    if (::ExtSelectClipRgn(hdevTarget, hrgnTmp, c4Mode) == ERROR)
    {
        TKrnlError::SetLastHostError(::GetLastError());

        ::DeleteObject(hrgnTmp);
        if (hrgnOld != kCIDGraphDev::hrgnInvalid)
            ::DeleteObject(hrgnOld);

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

    // Clean up the temp region now that it's set
    ::DeleteObject(hrgnTmp);

    // And return the old one
    return hrgnOld;
}


tCIDGraphDev::TRegionHandle
TGraphDrawDev::hrgnSetClipRegion(const  tCIDGraphDev::EClipModes    eMode
                                , const TGUIRegion&                 grgnToSet)
{
    // Get the current region so that we can return it to the caller
    tCIDGraphDev::TRegionHandle hrgnOld = ::CreateRectRgn(0, 0, 0, 0);
    if (hrgnOld == kCIDGraphDev::hrgnInvalid)
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

        // Won't happen, but makes analyzer happy
        return kCIDGraphDev::hrgnInvalid;
    }

    tCIDGraphDev::TDeviceHandle hdevTarget = hdevThis();
    int iRes = ::GetClipRgn(hdevTarget, hrgnOld);

    // IT failed, so clean up the region and throw
    if (iRes == -1)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        ::DeleteObject(hrgnOld);
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_GetClipRegion
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );

        // Won't happen, but makes analyzer happy
        return kCIDGraphDev::hrgnInvalid;
    }

    // If there was no clip region, then return an invalid handle
    if (iRes == 0)
    {
        ::DeleteObject(hrgnOld);
        hrgnOld = kCIDGraphDev::hrgnInvalid;
    }

    // Now set the new region on us
    const tCIDLib::TCard4 c4Mode = TGrDevPlatform::c4XlatClipMode(eMode);
    if (::ExtSelectClipRgn(hdevTarget, grgnToSet.hrgnThis(), c4Mode) == ERROR)
    {
        TKrnlError::SetLastHostError(::GetLastError());

        if (hrgnOld != kCIDGraphDev::hrgnInvalid)
            ::DeleteObject(hrgnOld);

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

        // Won't happen, but makes analyzer happy
        return kCIDGraphDev::hrgnInvalid;
    }

    // And return the old one
    return hrgnOld;
}


tCIDLib::TVoid TGraphDrawDev::InvertArea(const TArea& areaToInvert)
{
    tCIDLib::THostRectl  rectlInvert;
    areaToInvert.ToRectl(rectlInvert);

    if (!::InvertRect(hdevThis(), reinterpret_cast<const RECT*>(&rectlInvert)))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_InvertArea
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , areaToInvert
            );
        }
    }
}


tCIDLib::TVoid
TGraphDrawDev::Fill(const TArea& areaToFill, const TGUIBrush& gbrToUse)
{
    // Convert the area to a host rectangle
    tCIDLib::THostRectl  rectlFill;
    areaToFill.ToRectl(rectlFill);

    tCIDGraphDev::TDeviceHandle hdevTarget = hdevThis();
    if (!::FillRect(hdevTarget, reinterpret_cast<RECT*>(&rectlFill), gbrToUse.hbrThis()))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_FillArea
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , areaToFill
            );
        }
    }
}


tCIDLib::TVoid
TGraphDrawDev::Fill(const TArea& areaToFill, const TRGBClr& rgbToUse)
{
    // Convert the area to a host rectangle
    tCIDLib::THostRectl  rectlFill;
    areaToFill.ToRectl(rectlFill);

    TSolidBrush gbrToUse(rgbToUse);
    tCIDGraphDev::TDeviceHandle hdevTarget = hdevThis();
    if (!::FillRect(hdevTarget, reinterpret_cast<RECT*>(&rectlFill), gbrToUse.hbrThis()))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_FillArea
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , areaToFill
            );
        }
    }
}

tCIDLib::TVoid
TGraphDrawDev::Fill(const TGUIRegion& grgnToFill, const TRGBClr& rgbClr)
{
    TSolidBrush gbrToUse(rgbClr);
    if (!::FillRgn(hdevThis(), grgnToFill.hrgnThis(), gbrToUse.hbrThis()))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_FillRegion
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}

// WE asume the points are non-inclusive
tCIDLib::TVoid
TGraphDrawDev::Fill(const   TPoint&     pntFrom
                    , const TPoint&     pntTo
                    , const TRGBClr&    rgbToUse)
{
    Fill(TArea(pntFrom, pntTo), rgbToUse);
}


tCIDLib::TVoid TGraphDrawDev::FillPath()
{
    if (!::FillPath(hdevThis()))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_FillPath
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


tCIDLib::TVoid
TGraphDrawDev::FillWithBmpPattern(  const   TArea&                  areaToFill
                                    , const TBitmap&                bmpToUse
                                    , const tCIDGraphDev::EBmpModes eMode
                                    , const TPoint&                 pntPatOrg)
{
    tCIDGraphDev::TDeviceHandle hdevUs = hdevThis();

    // Save the current brush offset and set the new one
    POINT ptSaveOrg;
    if (!::SetBrushOrgEx(hdevUs, pntPatOrg.i4X(), pntPatOrg.i4Y(), &ptSaveOrg))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_SetBrushOrg
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }

    //
    //  Create a pattern brush using the passed bitmap, and select it into
    //  this device with a janitor.
    //
    TBitmapBrush gbrFill(bmpToUse);
    TBrushJanitor janBrush(this, &gbrFill);

    // If the width/height are zero, use the size of the image
    tCIDLib::TCard4 c4W = areaToFill.c4Width();
    tCIDLib::TCard4 c4H = areaToFill.c4Height();

    if (!c4W || !c4H)
    {
        tCIDLib::TCard4 c4Height;
        tCIDLib::TCard4 c4Width;
        bmpToUse.QuerySize(c4Width, c4Height);
        if (!c4W)
            c4W = c4Width;
        if (!c4H)
            c4H = c4Height;
    }

    // And do a fill with the pattern brush
    BOOL bRes = ::PatBlt
    (
        hdevUs
        , areaToFill.i4X()
        , areaToFill.i4Y()
        , c4W
        , c4H
        , TGrDevPlatform::c4XlatBmpMode(eMode)
    );

    // If it failed, save the error before we do anything that might override it
    tCIDLib::TCard4 c4Err = 0;
    if (!bRes)
    {
        c4Err = ::GetLastError();
        bRes = !bDevErrToIgnore(c4Err);
    }

    if (!::SetBrushOrgEx(hdevUs, ptSaveOrg.x, ptSaveOrg.y, 0))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_SetBrushOrg
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }

    // If the fill failed, then throw
    if (!bRes)
    {
        TKrnlError::SetLastHostError(c4Err);
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_FillArea
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , areaToFill
        );
    }
}

tCIDLib::TVoid
TGraphDrawDev::FillWithBmpPattern(  const   TPoint&                 pntFrom
                                    , const TPoint&                 pntTo
                                    , const TBitmap&                bmpToUse
                                    , const tCIDGraphDev::EBmpModes eMode
                                    , const TPoint&                 pntPatOrg)
{
    // Just call the other version
    TArea areaTmp(pntFrom, pntTo);
    FillWithBmpPattern(areaTmp, bmpToUse, eMode, pntPatOrg);
}


tCIDLib::TVoid
TGraphDrawDev::FillWithBmpPattern(  const   TPoint&                 pntFrom
                                    , const TPoint&                 pntTo
                                    , const TBitmap&                bmpToUse
                                    , const tCIDGraphDev::EBmpModes eMode)
{
    // Just call the other version
    TArea areaTmp(pntFrom, pntTo);
    FillWithBmpPattern(areaTmp, bmpToUse, eMode, TPoint::pntOrigin);
}

tCIDLib::TVoid
TGraphDrawDev::FillWithBmpPattern(  const   TArea&                  areaToFill
                                    , const TBitmap&                bmpToUse
                                    , const tCIDGraphDev::EBmpModes eMode)
{
    // Just call the other version
    FillWithBmpPattern(areaToFill, bmpToUse, eMode, TPoint::pntOrigin);
}


// Flushes any accumulated commands
tCIDLib::TVoid TGraphDrawDev::Flush()
{
    ::GdiFlush();
}


//
//  A few variations of gradient fields.
//
tCIDLib::TVoid
TGraphDrawDev::Gradient3DFill(  const   TArea&      areaFill
                                , const TRGBClr&    rgbLight
                                , const TRGBClr&    rgbDark)
{
    //
    //  We do a small gradient fill from each edge (dark color) inwards
    //  towards the center (light color.)
    //
    TArea areaTmp(areaFill);
    areaTmp.c4Width(3);
    GradientFill(areaTmp, rgbDark, rgbLight);
    areaTmp.RightJustifyIn(areaFill);
    GradientFill(areaTmp, rgbLight, rgbDark);

    areaTmp = areaFill;
    areaTmp.c4Height(3);
    GradientFill(areaTmp, rgbDark, rgbLight, tCIDGraphDev::EGradDirs::Vertical);
    areaTmp.BottomJustifyIn(areaFill);
    GradientFill(areaTmp, rgbLight, rgbDark, tCIDGraphDev::EGradDirs::Vertical);
}

tCIDLib::TVoid
TGraphDrawDev::Gradient3DFill2( const   TArea&      areaFill
                                , const TRGBClr&    rgbFill)
{
    // Create darker and lighter version
    TRGBClr rgbDark(rgbFill);
    rgbDark.AdjustHue(15);
    rgbDark.AdjustSaturation(0.16F);
    rgbDark.AdjustValue(0.04F);

    TRGBClr rgbLight(rgbFill);
    rgbLight.AdjustSaturation(-0.16F);

    TRGBClr rgbLight2(rgbFill);
    rgbLight2.SetValue(1);
    rgbLight2.AdjustSaturation(-0.2F);

    // Draw the light/dark lines above and below
    TArea areaWorking(areaFill);
    {
        TCosmeticPen gpenL(rgbLight2);
        TPenJanitor  janPen(this, &gpenL);
        DrawLine(areaWorking.pntUL(), areaWorking.pntUR());
    }

    {
        TCosmeticPen gpenD(rgbDark);
        TPenJanitor  janPen(this, &gpenD);
        DrawLine(areaWorking.pntLL(), areaWorking.pntLR());
    }

    // Draw the inner lines at the top and bottom
    areaWorking.Deflate(0, 1);
    {
        TCosmeticPen gpenL(rgbLight);
        TPenJanitor  janPen(this, &gpenL);
        DrawLine
        (
            areaFill.pntUL() + TPoint(0, 1)
            , areaFill.pntUR() + TPoint(0, 1)
        );
    }

    {
        TCosmeticPen gpenF(rgbFill);
        TPenJanitor  janPen(this, &gpenF);
        DrawLine
        (
            areaFill.pntLL() + TPoint(0, -1)
            , areaFill.pntLR() + TPoint(0, -1)
        );
    }

    // Do gradient lines on the two sides
    areaWorking.Deflate(0, 1);
    areaWorking.c4Width(2);
    GradientFill(areaWorking, rgbLight, rgbFill, tCIDGraphDev::EGradDirs::Vertical);
    areaWorking.RightJustifyIn(areaFill);
    GradientFill(areaWorking, rgbLight, rgbFill, tCIDGraphDev::EGradDirs::Vertical);
}


tCIDLib::TVoid
TGraphDrawDev::GradientFill(const   TArea&                  areaFill
                            , const TRGBClr&                rgbLeft
                            , const TRGBClr&                rgbRight
                            , const tCIDGraphDev::EGradDirs eDir)
{
    GradientFill(areaFill.pntOrg(), areaFill.pntLR(), rgbLeft, rgbRight, eDir);
}

tCIDLib::TVoid
TGraphDrawDev::GradientFill(const   TPoint&                 pntUL
                            , const TPoint&                 pntLR
                            , const TRGBClr&                rgbLeft
                            , const TRGBClr&                rgbRight
                            , const tCIDGraphDev::EGradDirs eDir)
{
    //
    //  Set up the vertex and gradient stuff according to the gradient
    //  direction and areas.
    //
    TRIVERTEX VertInfo[4];

    tCIDLib::TCard4 c4VertCount(0);
    tCIDLib::TCard4 c4GradCount(0);
    tCIDLib::TVoid* pMesh(0);
    GRADIENT_TRIANGLE TGradInfo[2];
    GRADIENT_RECT RGradInfo;
    if ((eDir == tCIDGraphDev::EGradDirs::Horizontal)
    ||  (eDir == tCIDGraphDev::EGradDirs::Vertical))
    {
        VertInfo[0].x       = pntUL.i4X();
        VertInfo[0].y       = pntUL.i4Y();
        VertInfo[0].Red     = tCIDLib::TCard2(rgbLeft.c1Red()) << 8;
        VertInfo[0].Green   = tCIDLib::TCard2(rgbLeft.c1Green()) << 8;
        VertInfo[0].Blue    = tCIDLib::TCard2(rgbLeft.c1Blue()) << 8;
        VertInfo[0].Alpha   = 0;

        VertInfo[1].x       = pntLR.i4X() + 1;
        VertInfo[1].y       = pntLR.i4Y() + 1;
        VertInfo[1].Red     = tCIDLib::TCard2(rgbRight.c1Red()) << 8;
        VertInfo[1].Green   = tCIDLib::TCard2(rgbRight.c1Green()) << 8;
        VertInfo[1].Blue    = tCIDLib::TCard2(rgbRight.c1Blue()) << 8;
        VertInfo[1].Alpha   = 0;

        RGradInfo.UpperLeft = 0;
        RGradInfo.LowerRight = 1;
        pMesh = &RGradInfo;

        c4VertCount = 2;
        c4GradCount = 1;
    }
     else if (eDir == tCIDGraphDev::EGradDirs::DownDiagonal)
    {
        // Figure out the color half way between the two colors
        TRGBClr rgbHalf;
        rgbLeft.Interpolate(rgbRight, rgbHalf);

        VertInfo[0].x       = pntUL.i4X();
        VertInfo[0].y       = pntUL.i4Y();
        VertInfo[0].Red     = tCIDLib::TCard2(rgbLeft.c1Red()) << 8;
        VertInfo[0].Green   = tCIDLib::TCard2(rgbLeft.c1Green()) << 8;
        VertInfo[0].Blue    = tCIDLib::TCard2(rgbLeft.c1Blue()) << 8;
        VertInfo[0].Alpha   = 0;

        VertInfo[1].x       = pntLR.i4X();
        VertInfo[1].y       = pntUL.i4Y();
        VertInfo[1].Red     = tCIDLib::TCard2(rgbHalf.c1Red()) << 8;
        VertInfo[1].Green   = tCIDLib::TCard2(rgbHalf.c1Green()) << 8;
        VertInfo[1].Blue    = tCIDLib::TCard2(rgbHalf.c1Blue()) << 8;
        VertInfo[1].Alpha   = 0;

        VertInfo[2].x       = pntLR.i4X();
        VertInfo[2].y       = pntLR.i4Y();
        VertInfo[2].Red     = tCIDLib::TCard2(rgbRight.c1Red()) << 8;
        VertInfo[2].Green   = tCIDLib::TCard2(rgbRight.c1Green()) << 8;
        VertInfo[2].Blue    = tCIDLib::TCard2(rgbRight.c1Blue()) << 8;
        VertInfo[2].Alpha   = 0;

        VertInfo[3].x       = pntUL.i4X();
        VertInfo[3].y       = pntLR.i4Y();
        VertInfo[3].Red     = tCIDLib::TCard2(rgbHalf.c1Red()) << 8;
        VertInfo[3].Green   = tCIDLib::TCard2(rgbHalf.c1Green()) << 8;
        VertInfo[3].Blue    = tCIDLib::TCard2(rgbHalf.c1Blue()) << 8;
        VertInfo[3].Alpha   = 0;

        if (eDir == tCIDGraphDev::EGradDirs::DownDiagonal)
        {
            TGradInfo[0].Vertex1 = 0;
            TGradInfo[0].Vertex2 = 1;
            TGradInfo[0].Vertex3 = 3;
            TGradInfo[1].Vertex1 = 1;
            TGradInfo[1].Vertex2 = 2;
            TGradInfo[1].Vertex3 = 3;
        }
         else
        {
            TGradInfo[0].Vertex1 = 0;
            TGradInfo[0].Vertex2 = 3;
            TGradInfo[0].Vertex3 = 2;
            TGradInfo[1].Vertex1 = 0;
            TGradInfo[1].Vertex2 = 1;
            TGradInfo[1].Vertex3 = 2;
        }
        pMesh = TGradInfo;

        c4VertCount = 4;
        c4GradCount = 2;
    }

    // And translate the orientation
    tCIDLib::TCard4 c4Orient;
    if (eDir == tCIDGraphDev::EGradDirs::Horizontal)
        c4Orient = GRADIENT_FILL_RECT_H;
    else if (eDir == tCIDGraphDev::EGradDirs::Vertical)
        c4Orient = GRADIENT_FILL_RECT_V;
    else
        c4Orient = GRADIENT_FILL_TRIANGLE;

    if (!::GradientFill
    (
        hdevThis()
        , VertInfo
        , c4VertCount
        , pMesh
        , c4GradCount
        , c4Orient))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_GradientFill
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


tCIDLib::TVoid
TGraphDrawDev::GradientTriangleFill(const   TPoint&     pnt1
                                    , const TRGBClr&    rgb1
                                    , const TPoint&     pnt2
                                    , const TRGBClr&    rgb2
                                    , const TPoint&     pnt3
                                    , const TRGBClr&    rgb3)
{
    TRIVERTEX VertInfo[3];
    VertInfo[0].x       = pnt1.i4X();
    VertInfo[0].y       = pnt1.i4Y();
    VertInfo[0].Red     = tCIDLib::TCard2(rgb1.c1Red()) << 8;
    VertInfo[0].Green   = tCIDLib::TCard2(rgb1.c1Green()) << 8;
    VertInfo[0].Blue    = tCIDLib::TCard2(rgb1.c1Blue()) << 8;
    VertInfo[0].Alpha   = 0;

    VertInfo[1].x       = pnt2.i4X();
    VertInfo[1].y       = pnt2.i4Y();
    VertInfo[1].Red     = tCIDLib::TCard2(rgb2.c1Red()) << 8;
    VertInfo[1].Green   = tCIDLib::TCard2(rgb2.c1Green()) << 8;
    VertInfo[1].Blue    = tCIDLib::TCard2(rgb2.c1Blue()) << 8;
    VertInfo[1].Alpha   = 0;

    VertInfo[2].x       = pnt3.i4X();
    VertInfo[2].y       = pnt3.i4Y();
    VertInfo[2].Red     = tCIDLib::TCard2(rgb3.c1Red()) << 8;
    VertInfo[2].Green   = tCIDLib::TCard2(rgb3.c1Green()) << 8;
    VertInfo[2].Blue    = tCIDLib::TCard2(rgb3.c1Blue()) << 8;
    VertInfo[2].Alpha   = 0;

    GRADIENT_TRIANGLE GradInfo;
    GradInfo.Vertex1 = 0;
    GradInfo.Vertex2 = 1;
    GradInfo.Vertex3 = 2;

    if (!::GradientFill
    (
        hdevThis()
        , VertInfo
        , 3
        , &GradInfo
        , 1
        , GRADIENT_FILL_TRIANGLE))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_GradientFill
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


//
//  This is a special case helper. It's for when non-alpha based images
//  are to be blitted with some constant alpha. So we have to do an alpha
//  based operation, to get the constant alpha. But we also need to do the
//  masking for color based trnaspraency.
//
//  The incoming image must be pre-masked with the inverse mask.
//
//  We don't need the transparency color, but it's passed so that if this
//  method is being intercepted for remoting purposes, that the other
//  side (which will only have the main image) can create the necessary
//  masks.
//
tCIDLib::TVoid
TGraphDrawDev::MaskedAlphaBlit( const   TBitmap&            bmpSrc
                                , const TBitmap&            bmpMask
                                , const TArea&              areaSource
                                , const TPoint&             pntTarget
                                , const tCIDLib::TCard1     c1Alpha
                                , const tCIDLib::TBoolean   bSrcAlpha
                                , const tCIDLib::TCard4     c4TransClr)
{
    //
    //  Set up a memory device over the bitmap, set up the target area to
    //  be the same size as the source, and call the other version.
    //
    TGraphMemDev gdevSrc(*this, bmpSrc);
    TArea areaTarget(pntTarget, areaSource.szArea());
    MaskedAlphaBlit
    (
        gdevSrc, bmpMask, areaSource, areaTarget, c1Alpha, bSrcAlpha, c4TransClr
    );
}

tCIDLib::TVoid
TGraphDrawDev::MaskedAlphaBlit( const   TBitmap&            bmpSrc
                                , const TBitmap&            bmpMask
                                , const TPoint&             pntTarget
                                , const tCIDLib::TCard1     c1Alpha
                                , const tCIDLib::TBoolean   bSrcAlpha
                                , const tCIDLib::TCard4     c4TransClr)
{
    //
    //  Set up a memory device over the bitmap, set up the source and target
    //  areas, then call the other version.
    //
    TGraphMemDev gdevSrc(*this, bmpSrc);
    TArea areaSource(TPoint::pntOrigin, bmpSrc.szBitmap());
    TArea areaTarget(pntTarget, areaSource.szArea());
    MaskedAlphaBlit
    (
        gdevSrc, bmpMask, areaSource, areaTarget, c1Alpha, bSrcAlpha, c4TransClr
    );
}

tCIDLib::TVoid
TGraphDrawDev::MaskedAlphaBlit( const   TBitmap&            bmpSrc
                                , const TBitmap&            bmpMask
                                , const TArea&              areaSource
                                , const TArea&              areaTarget
                                , const tCIDLib::TCard1     c1Alpha
                                , const tCIDLib::TBoolean   bSrcAlpha
                                , const tCIDLib::TCard4     c4TransClr)
{
    // Set up a memory device over the bitmap, then call the other version
    TGraphMemDev gdevSrc(*this, bmpSrc);
    MaskedAlphaBlit
    (
        gdevSrc, bmpMask, areaSource, areaTarget, c1Alpha, bSrcAlpha, c4TransClr
    );
}

tCIDLib::TVoid
TGraphDrawDev::MaskedAlphaBlit( const   TGraphDrawDev&      gdevSrc
                                , const TBitmap&            bmpMask
                                , const TArea&              areaSource
                                , const TArea&              areaTarget
                                , const tCIDLib::TCard1     c1Alpha
                                , const tCIDLib::TBoolean   bSrcAlpha
                                , const tCIDLib::TCard4     c4TransClr)
{
    //
    //  If the source and target are not the same size, we have to do
    //  stretch verions of blits, while otherwise we can in some cases do
    //  the more efficient non-stretching versions.
    //
    const tCIDLib::TBoolean bStretch = areaSource.szArea() != areaTarget.szArea();

    //
    //  Get a memory device compatible with us, and the size of the target
    //  area we are going to draw, and blit the current contents of our
    //  surface from the destination location into it. We never stretch here
    //  because the target area is what we are drawing into.
    //
    TGraphMemDev gdevTmp(*this, areaTarget.szArea());
    if (!::BitBlt
    (
        gdevTmp.hdevThis()
        , 0
        , 0
        , areaTarget.c4Width()
        , areaTarget.c4Height()
        , hdevThis()
        , areaTarget.i4X()
        , areaTarget.i4Y()
        , SRCCOPY))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_BitBlit
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }

    // Create a blend function based on the passed alpha
    BLENDFUNCTION Blend;
    Blend.BlendOp = 0;
    Blend.BlendFlags = 0;
    Blend.SourceConstantAlpha = c1Alpha;
    Blend.AlphaFormat = bSrcAlpha ? AC_SRC_ALPHA : 0;

    //
    //  Now do the alpha blit the source image onto the memory image, so
    //  we now have a blended versions in memory. We will stretch the source
    //  image if the source/dest sizes are different. We use the source x/y
    //  because we could be clipping it, in which case the source is not the
    //  origin of the image.
    //
    if (!::AlphaBlend
    (
        gdevTmp.hdevThis()
        , 0
        , 0
        , areaTarget.c4Width()
        , areaTarget.c4Height()
        , gdevSrc.hdevThis()
        , areaSource.i4X()
        , areaSource.i4Y()
        , areaSource.c4Width()
        , areaSource.c4Height()
        , Blend))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_BitBlit
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }

    //
    //  Use the mask to mask off the target area, which will leave the hole
    //  into which we'll draw. We have to stretch/shrink the mask if
    //  necessary.
    //
    {
        TGraphMemDev gdevMask(*this, bmpMask);
        if (bStretch)
        {
            if (!::StretchBlt
            (
                hdevThis()
                , areaTarget.i4X()
                , areaTarget.i4Y()
                , areaTarget.c4Width()
                , areaTarget.c4Height()
                , gdevMask.hdevThis()
                , areaSource.i4X()
                , areaSource.i4Y()
                , areaSource.c4Width()
                , areaSource.c4Height()
                , SRCAND))
            {
                if (!bDevErrToIgnore())
                {
                    facCIDGraphDev().ThrowKrnlErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kGrDevErrs::errcDev_BitBlit
                        , TKrnlError::kerrLast()
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::CantDo
                    );
                }
            }
        }
         else
        {
            if (!::BitBlt
            (
                hdevThis()
                , areaTarget.i4X()
                , areaTarget.i4Y()
                , areaSource.c4Width()
                , areaSource.c4Height()
                , gdevMask.hdevThis()
                , areaSource.i4X()
                , areaSource.i4Y()
                , SRCAND))
            {
                if (!bDevErrToIgnore())
                {
                    facCIDGraphDev().ThrowKrnlErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kGrDevErrs::errcDev_BitBlit
                        , TKrnlError::kerrLast()
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::CantDo
                    );
                }
            }
        }
    }

    //
    //  Mask off the memory image using an inverted mask, so that any pixels
    //  not part of the image part are black. We have to create this inverted
    //  mask on the fly from the regular mask.
    //
    {
        TBitmap bmpInvMask(bmpMask);
        bmpInvMask.DeepCopy();
        bmpInvMask.Invert(gdevSrc);
        TGraphMemDev gdevMask(*this, bmpInvMask);
        if (bStretch)
        {
            if (!::StretchBlt
            (
                gdevTmp.hdevThis()
                , 0
                , 0
                , areaTarget.c4Width()
                , areaTarget.c4Height()
                , gdevMask.hdevThis()
                , areaSource.i4X()
                , areaSource.i4Y()
                , areaSource.c4Width()
                , areaSource.c4Height()
                , SRCAND))
            {
                if (!bDevErrToIgnore())
                {
                    facCIDGraphDev().ThrowKrnlErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kGrDevErrs::errcDev_BitBlit
                        , TKrnlError::kerrLast()
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::CantDo
                    );
                }
            }
        }
         else
        {
            if (!::BitBlt
            (
                gdevTmp.hdevThis()
                , 0
                , 0
                , areaSource.c4Width()
                , areaSource.c4Height()
                , gdevMask.hdevThis()
                , areaSource.i4X()
                , areaSource.i4Y()
                , SRCAND))
            {
                if (!bDevErrToIgnore())
                {
                    facCIDGraphDev().ThrowKrnlErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kGrDevErrs::errcDev_BitBlit
                        , TKrnlError::kerrLast()
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::CantDo
                    );
                }
            }
        }
    }

    //
    //  And finally blit the temporary image, which is now masked off, into
    //  the area we just masked on the target. In this case, our memory
    //  image and the target area always the same, so just a straight blit
    //  will always work.
    //
    if (!::BitBlt
    (
        hdevThis()
        , areaTarget.i4X()
        , areaTarget.i4Y()
        , areaTarget.c4Width()
        , areaTarget.c4Height()
        , gdevTmp.hdevThis()
        , 0
        , 0
        , SRCPAINT))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_BitBlit
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


//
//  Does a relative move of the current position, i.e. it just adjusts the
//  current position.
//
tCIDLib::TVoid TGraphDrawDev::MoveRelative(const TPoint& pntOfs)
{
    TPoint pntNew(pntCurPosition());
    pntNew += pntOfs;

    if (!::MoveToEx(hdevThis(), pntNew.i4X(), pntNew.i4Y(), 0))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_MoveRelative
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , pntOfs
            );
        }
    }
}

tCIDLib::TVoid TGraphDrawDev::MoveRelative( const   tCIDLib::TInt4  i4X
                                            , const tCIDLib::TInt4  i4Y)
{
    TPoint pntNew(pntCurPosition());
    pntNew.Adjust(i4X, i4Y);

    if (!::MoveToEx(hdevThis(), pntNew.i4X(), pntNew.i4Y(), 0))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_MoveRelative
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TPoint(i4X, i4Y)
            );
        }
    }
}


//
//  Does an absolute move of the current position to a given position.
//
tCIDLib::TVoid TGraphDrawDev::MoveTo(const TPoint& pntNewPos)
{
    if (!::MoveToEx(hdevThis(), pntNewPos.i4X(), pntNewPos.i4Y(), 0))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_MoveTo
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , pntNewPos
            );
        }
    }
}

tCIDLib::TVoid TGraphDrawDev::MoveTo(   const   tCIDLib::TInt4  i4X
                                        , const tCIDLib::TInt4  i4Y)
{
    if (!::MoveToEx(hdevThis(), i4X, i4Y, 0))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_MoveTo
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TPoint(i4X, i4Y)
            );
        }
    }
}


// Returns the current output position
TPoint TGraphDrawDev::pntCurPosition() const
{
    POINT Pos;
    if (!::GetCurrentPositionEx(hdevThis(), &Pos))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_GetCurPos
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
    return TPoint(Pos.x, Pos.y);
}


//
//  Fills the given area of this device's drawing surface with currently set
//  brush and the passed mode.
//
tCIDLib::TVoid
TGraphDrawDev::PatternFill( const   TArea&                  areaToFill
                            , const tCIDGraphDev::EBmpModes eMode)
{
    if (!::PatBlt(  hdevThis()
                    , areaToFill.i4X()
                    , areaToFill.i4Y()
                    , areaToFill.c4Width()
                    , areaToFill.c4Height()
                    , TGrDevPlatform::c4XlatBmpMode(eMode)))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_FillArea
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , areaToFill
            );
        }
    }
}


//
//  Pops back off the last saved context. The caller is responsible for
//  keeping pushes and pops balanced.
//
tCIDLib::TVoid TGraphDrawDev::PopContext()
{
    if (!::RestoreDC(hdevThis(), -1))
    {
        //
        //  This one sometimes returns a bogus error return and no
        //  error code, in which case we assume we can ignore it.
        //
        const tCIDLib::TCard4 c4Err = ::GetLastError();
        if (c4Err)
        {
            TKrnlError::SetLastHostError(c4Err);
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_RestoreState
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


//
//  Put back the passed clip region. This just sets a copy of the passed region. The caller
//  is still responsible for it.
//
tCIDLib::TVoid
TGraphDrawDev::PopClipArea(tCIDGraphDev::TRegionHandle hrgnToPutBack)
{
    if (!::ExtSelectClipRgn(hdevThis(), hrgnToPutBack, RGN_COPY))
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


tCIDLib::TVoid
TGraphDrawDev::PopFont( const   TGUIFont&                   gfontCur
                        , const tCIDGraphDev::TFontHandle   hfontPutBack)
{
    // Put the passed one back and get the current back out
    tCIDGraphDev::TFontHandle hfontPopped = reinterpret_cast<tCIDGraphDev::TFontHandle>
    (
        ::SelectObject(hdevThis(), hfontPutBack)
    );

    if (!hfontPopped)
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
            , TString(L"Font")
        );
    }

    // Make sure the font we got back is the one passed
    if (hfontPopped != gfontCur.hfontThis())
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_DeselectWrongObj
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TString(L"Font")
        );
    }
}


//
//  Pop the passed pen and go back to the previous one. If it fails, the
//  caller is still responsible for the handle.
//
tCIDLib::TVoid
TGraphDrawDev::PopPen(TGUIPen& gpenCur, tCIDGraphDev::TPenHandle hpenOld)
{
    // If it's not set on a device, then this is a bogus call
    if (!gpenCur.bSetOnDevice())
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_NotSelected
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TString(L"Pen")
        );
    }

    // Looks ok, so select the old one back
    tCIDGraphDev::TPenHandle hpenPopped = reinterpret_cast<tCIDGraphDev::TPenHandle>
    (
        ::SelectObject(hdevThis(), hpenOld)
    );

    if (!hpenPopped)
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
            , TString(L"Pen")
        );
    }

    // Make sure the old one is the one we got
    if (hpenPopped != gpenCur.hpenThis())
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_DeselectWrongObj
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , TString(L"Pen")
        );
    }

    // Ok, went ok, so lets mark it as not selected
    gpenCur.bSetOnDevice(kCIDLib::False);
}


//
//  Pushes the current device context on the device's internal stack, to
//  later be restored with PopContext().
//
tCIDLib::TVoid TGraphDrawDev::PushContext()
{
    if (!::SaveDC(hdevThis()))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_SaveState
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


tCIDLib::TVoid
TGraphDrawDev::QueryCharOffsets(const   TString&            strSource
                                , const tCIDLib::TCard4     c4Index
                                ,       tCIDLib::TInt4&     i4Offset
                                ,       tCIDLib::TCard4&    c4Width)
{
    //
    //  Validate the index. If its not valid, then this will throw an
    //  exception right off the bat.
    //
    tCIDLib::TCh chTest = strSource[c4Index];

    // Get a copy of the device handle
    const tCIDGraphDev::TDeviceHandle hdevTarget = hdevThis();

    //
    //  Get the size info before the character. This gives us the left
    //  side of the character. Only do it if the index is not 0.
    //
    SIZE SizeInfoBefore = {0};

    if (c4Index)
    {
        if (!::GetTextExtentPoint32W
        (
            hdevTarget
            , strSource.pszBuffer()
            , c4Index
            , &SizeInfoBefore))
        {
            if (!bDevErrToIgnore())
            {
                facCIDGraphDev().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kGrDevErrs::errcDev_QueryTextExtent
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }
        }
    }

    // And now get the width of the particular character
    int Width;
    if (!::GetCharWidth32(hdevTarget, chTest, chTest, &Width))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_QueryTextExtent
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }

    // Now we have the info that we need
    i4Offset = SizeInfoBefore.cx ? SizeInfoBefore.cx - 1 : 0;
    c4Width = Width;
}


tCIDLib::TVoid
TGraphDrawDev::QueryCharWidths( const   TString&                        strSource
                                ,       TFundVector<tCIDLib::TCard4>&   fcolToFill)
{
    // Flush teh target and exit if the src is empty
    fcolToFill.RemoveAll();

    const tCIDLib::TCard4 c4SrcLen = strSource.c4Length();
    if (!c4SrcLen)
        return;

    // For efficiency, make sure the target is ready to hold this much
    fcolToFill.bCheckCapacity(c4SrcLen);

    //  Get the width of each char and store it in the caller's vector
    tCIDLib::TSInt iWidth;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SrcLen; c4Index++)
    {
        const tCIDLib::TCh chChar = strSource[c4Index];

        if (!::GetCharWidth32(hdevThis(), chChar, chChar, &iWidth))
        {
            if (!bDevErrToIgnore())
            {
                facCIDGraphDev().ThrowKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kGrDevErrs::errcDev_QueryTextExtent
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::CantDo
                );
            }
        }
        fcolToFill.c4AddElement(tCIDLib::TCard4(iWidth));
    }
}


tCIDLib::TVoid TGraphDrawDev::QueryFontMetrics(TFontMetrics& fmtrToFill)
{
    fmtrToFill.UpdateFrom(*this);
}


TRGBClr TGraphDrawDev::rgbAt(const TPoint& pntAt) const
{
    COLORREF clrRef = ::GetPixel(hdevThis(), pntAt.i4X(), pntAt.i4Y());

    if (clrRef == CLR_INVALID)
        return TRGBClr(0, 0, 0);
    return TRGBClr(clrRef);
}


TRGBClr TGraphDrawDev::rgbBgnColor() const
{
    return TRGBClr(::GetBkColor(hdevThis()));
}

TRGBClr TGraphDrawDev::rgbTextColor() const
{
    return TRGBClr(::GetTextColor(hdevThis()));
}

tCIDLib::TVoid TGraphDrawDev::ResetBoundsArea()
{
    if ((::SetBoundsRect(hdevThis(), 0, DCB_RESET) & DCB_RESET) == 0)
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_ResetBounds
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


TSize TGraphDrawDev::szPelsPerInch() const
{
    return TSize
    (
        tCIDLib::TCard4(::GetDeviceCaps(hdevThis(), LOGPIXELSX))
        , tCIDLib::TCard4(::GetDeviceCaps(hdevThis(), LOGPIXELSY))
    );
}


//
//  This method will shift bits internally within the DC, dealing with overlap and
//  and whatnot that BitBlt may not handle well. And it may be more efficient as well
//  since it knows it's always dealing with a single DC.
//
tCIDLib::TVoid
TGraphDrawDev::ScrollBits(  const   TArea&              areaToScroll
                            , const TArea&              areaClip
                            , const tCIDLib::TInt4      i4XOfs
                            , const tCIDLib::TInt4      i4YOfs
                            ,       TArea&              areaUpdate
                            , const tCIDLib::TBoolean   bWaitRetrace)
{
    // Convert the areas to RECTs
    tCIDLib::THostRectl rectlScroll;
    areaToScroll.ToRectl(rectlScroll);

    tCIDLib::THostRectl rectlClip;
    areaClip.ToRectl(rectlClip);

    if (bWaitRetrace)
        WaitRetrace();

    tCIDLib::THostRectl rectlUpdate;
    BOOL bRes = ::ScrollDC
    (
        hdevThis()
        , i4XOfs
        , i4YOfs
        , (RECT*)&rectlScroll
        , (RECT*)&rectlClip
        , 0
        , (RECT*)&rectlUpdate
    );

    if (!bRes)
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_ScrollBits
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }

    // Convert the update rectangle back
    areaUpdate.FromRectl(rectlUpdate);
}


//
//  Set the background color for those operations that use it, drawing
//  text, and so forth.
//
tCIDLib::TVoid TGraphDrawDev::SetBgnColor(const TRGBClr& rgbToSet)
{
    if (::SetBkColor(hdevThis(), rgbToSet.c4Color()) == CLR_INVALID)
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_SetBackColor
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , rgbToSet
            );
        }
    }
}


//
//  Sets the path currently set in this device as the clip path for this
//  device.
//
tCIDLib::TVoid TGraphDrawDev::SetClipPath(const tCIDGraphDev::EClipModes eMode)
{
    if (!::SelectClipPath(hdevThis(), TGrDevPlatform::c4XlatClipMode(eMode)))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_SetClipPath
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


tCIDLib::TVoid TGraphDrawDev::SetPixel( const   TPoint&     pntAt
                                        , const TRGBClr&    rgbToSet)
{
    ::SetPixel(hdevThis(), pntAt.i4X(), pntAt.i4Y(), rgbToSet.c4Color());
}


// Sets the text output color
tCIDLib::TVoid TGraphDrawDev::SetTextColor(const TRGBClr& rgbToSet)
{
    if (::SetTextColor(hdevThis(), rgbToSet.c4Color()) == CLR_INVALID)
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_SetTextColor
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , rgbToSet
            );
        }
    }
}


//
//  Sets the world transform's translation component. If this device has not
//  been put into advanced mode, yet, it does so.
//
tCIDLib::TVoid
TGraphDrawDev::SetWorldTranslation( const   tCIDLib::TFloat4    f4XOfs
                                    , const tCIDLib::TFloat4    f4YOfs)
{
    tCIDGraphDev::TDeviceHandle hdevTar = hdevThis();
    if (!m_bAdvModeSet)
    {
        ::SetGraphicsMode(hdevTar, GM_ADVANCED);
        m_bAdvModeSet = kCIDLib::True;
    }

    XFORM XForm = {0};
    XForm.eDx = f4XOfs;
    XForm.eDy = f4YOfs;
    ::SetWorldTransform(hdevTar, &XForm);
}


//
//  Stroke the indicated area with the currently set pen.
//
tCIDLib::TVoid TGraphDrawDev::Stroke(const TArea& areaToStroke)
{
    // Set a null brush so we don't affect the interior
    TNullBrush gbrNull;
    TBrushJanitor janBrush(this, &gbrNull);

    if (!::Rectangle(hdevThis()
                    , areaToStroke.i4X()
                    , areaToStroke.i4Y()
                    , areaToStroke.i4Right()
                    , areaToStroke.i4Bottom()))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_StrokeArea
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


//
//  Stroke the indicated area using a solid line of the indicated color, with
//  optional rounding (which can be zero if none is desired.)
//
tCIDLib::TVoid
TGraphDrawDev::Stroke(  const   TArea&          areaToStroke
                        , const TRGBClr&        rgbClr
                        , const tCIDLib::TCard4 c4Rounding)
{
    // Set a null brush so we don't affect the interior
    TNullBrush gbrNull;
    TBrushJanitor janBrush(this, &gbrNull);

    TCosmeticPen gpenBorder(rgbClr);
    TPenJanitor janPen(this, &gpenBorder);

    //
    //  These guys, though not documented as such, are not inclusive, so add
    //  one to the right and bottom.
    //
    BOOL bRes;
    if (c4Rounding)
    {
        bRes = ::RoundRect
        (
            hdevThis()
            , areaToStroke.i4X()
            , areaToStroke.i4Y()
            , areaToStroke.i4Right()
            , areaToStroke.i4Bottom()
            , c4Rounding
            , c4Rounding
        );
    }
     else
    {
        bRes = ::Rectangle
        (
            hdevThis()
            , areaToStroke.i4X()
            , areaToStroke.i4Y()
            , areaToStroke.i4Right()
            , areaToStroke.i4Bottom()
        );
    }

    if (!bRes)
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_StrokeArea
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


//
//  Strokes a regions in the passed color. Note that this centers the brush
//  output on the region if it is wider than 1 pixel.
//
tCIDLib::TVoid TGraphDrawDev::Stroke(const  TGUIRegion&     grgnToStroke
                                    , const TRGBClr&        rgbToUse
                                    , const tCIDLib::TCard4 c4Width)
{
    TSolidBrush gbrToUse(rgbToUse);
    if (!::FrameRgn(hdevThis()
                    , grgnToStroke.hrgnThis()
                    , gbrToUse.hbrThis()
                    , c4Width
                    , c4Width))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_StrokeRegion
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


tCIDLib::TVoid TGraphDrawDev::StrokeAndFillPath()
{
    if (!::StrokeAndFillPath(hdevThis()))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_StrokeAndFillPath
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


tCIDLib::TVoid TGraphDrawDev::StrokePath()
{
    if (!::StrokePath(hdevThis()))
    {
        if (!bDevErrToIgnore())
        {
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_StrokePath
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }
    }
}


// ---------------------------------------------------------------------------
//  TGraphDrawDev: Hidden constructors
// ---------------------------------------------------------------------------
TGraphDrawDev::TGraphDrawDev() :

    m_bAdvModeSet(kCIDLib::False)
{
}

TGraphDrawDev::TGraphDrawDev(   const   tCIDGraphDev::TDeviceHandle hdevToUse
                                , const tCIDLib::EAdoptOpts         eAdopt) :

    TGraphicDevice(hdevToUse, eAdopt)
    , m_bAdvModeSet(kCIDLib::False)
{
}


// ---------------------------------------------------------------------------
//  TGraphDrawDev: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TGraphDrawDev::GradientCircleLR(        tCIDGraphDev::TDeviceHandle hdevTar
                                , const TPoint&                     pntCenter
                                , const tCIDLib::TCard4             c4Radius
                                , const tCIDLib::TCard4             c4Light
                                , const tCIDLib::TCard4             c4Dark)
{
    tCIDLib::TInt4 i4Error;
    tCIDLib::TInt4 i4XOff;
    tCIDLib::TInt4 i4YOff;

    //Set starting values
    i4XOff  = tCIDLib::TInt4(c4Radius);
    i4YOff  = 0;
    i4Error = -tCIDLib::TInt4(c4Radius);

    const tCIDLib::TFloat8 f8HalfPI = kCIDLib::f8PI * 0.5;
    tCIDLib::TCard4 c4Color;
    do
    {
        const tCIDLib::TFloat8 f8Angle = TMathLib::f8ArcTangentXY(i4XOff, i4YOff);

        const tCIDLib::TInt4 i4YPlusY  = pntCenter.i4Y() + i4YOff;
        const tCIDLib::TInt4 i4YMinusY = pntCenter.i4Y() - i4YOff;
        const tCIDLib::TInt4 i4YPlusX  = pntCenter.i4Y() + i4XOff;
        const tCIDLib::TInt4 i4YMinusX = pntCenter.i4Y() - i4XOff;
        const tCIDLib::TInt4 i4XPlusX  = pntCenter.i4X() + i4XOff;
        const tCIDLib::TInt4 i4XPlusY  = pntCenter.i4X() + i4YOff;

        c4Color = c4AngleColor(f8Angle, c4Light, c4Dark);
        ::SetPixel(hdevTar, i4XPlusX, i4YPlusY, c4Color);

        c4Color = c4AngleColor(f8HalfPI - f8Angle, c4Light, c4Dark);
        ::SetPixel(hdevTar, i4XPlusY, i4YPlusX, c4Color);

        c4Color = c4AngleColor(-f8HalfPI + f8Angle, c4Light, c4Dark);
        ::SetPixel(hdevTar, i4XPlusY, i4YMinusX, c4Color);

        c4Color = c4AngleColor(-f8Angle, c4Light, c4Dark);
        ::SetPixel(hdevTar, i4XPlusX, i4YMinusY, c4Color);

        i4Error += i4YOff++;
        if ((i4Error += i4YOff) >= 0)
            i4Error -= --i4XOff * 2;
    }   while (i4YOff <= i4XOff);
}


tCIDLib::TVoid
TGraphDrawDev::GradientCircleUL(        tCIDGraphDev::TDeviceHandle hdevTar
                                , const TPoint&                     pntCenter
                                , const tCIDLib::TCard4             c4Radius
                                , const tCIDLib::TCard4             c4Light
                                , const tCIDLib::TCard4             c4Dark)
{
    tCIDLib::TInt4 i4Error;
    tCIDLib::TInt4 i4XOff;
    tCIDLib::TInt4 i4YOff;

    //Set starting values
    i4XOff  = tCIDLib::TInt4(c4Radius);
    i4YOff  = 0;
    i4Error = -tCIDLib::TInt4(c4Radius);

    const tCIDLib::TFloat8 f8HalfPI = kCIDLib::f8PI * 0.5;
    tCIDLib::TCard4 c4Color;

    do
    {
        const tCIDLib::TFloat8 f8Angle = TMathLib::f8ArcTangentXY(i4XOff, i4YOff);

        c4Color = c4AngleColor(f8HalfPI + f8Angle, c4Light, c4Dark);
        ::SetPixel(hdevTar, pntCenter.i4X() - i4YOff, pntCenter.i4Y() + i4XOff, c4Color);

        c4Color = c4AngleColor(kCIDLib::f8PI - f8Angle, c4Light, c4Dark);
        ::SetPixel(hdevTar, pntCenter.i4X() - i4XOff, pntCenter.i4Y() + i4YOff, c4Color);

        c4Color = c4AngleColor(-kCIDLib::f8PI + f8Angle, c4Light, c4Dark);
        ::SetPixel(hdevTar, pntCenter.i4X() - i4XOff, pntCenter.i4Y() - i4YOff, c4Color);

        c4Color = c4AngleColor(-f8HalfPI - f8Angle, c4Light, c4Dark);
        ::SetPixel(hdevTar, pntCenter.i4X() - i4YOff, pntCenter.i4Y() - i4XOff, c4Color);

        i4Error += i4YOff++;
        if ((i4Error += i4YOff) >= 0)
            i4Error -= --i4XOff * 2;

    }   while (i4YOff <= i4XOff);
}

