//
// FILE NAME: CIDGraphDev_Bitmap.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TBitmap class, which is the platform independent
//  abstraction for a host bitmap.
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
#include    "CIDImage_RawPalettes_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TBitmap,TObject)
RTTIDecls(TSysBitmapInfo,TObject)


namespace CIDGraphDev_Bitmap
{
    // ---------------------------------------------------------------------------
    //  A little array of mapping structures that let us turn our generic system
    //  bitmap enum into a system specific one.
    // ---------------------------------------------------------------------------
    struct TBmpMap
    {
        tCIDGraphDev::ESysBmps    eBmp;
        tCIDLib::TResId         ridThis;
        const tCIDLib::TCh*     pszName;
    };

    static TBmpMap  amapVals[] =
    {
        // These are Win32 bitmaps
            { tCIDGraphDev::ESysBmps::None         , 0                            , L"None" }
        ,   { tCIDGraphDev::ESysBmps::Close        , 32754                        , L"Close" }
        ,   { tCIDGraphDev::ESysBmps::UpArrow      , 32753                        , L"UpArrow" }
        ,   { tCIDGraphDev::ESysBmps::DownArrow    , 32752                        , L"DownArrow" }
        ,   { tCIDGraphDev::ESysBmps::RightArrow   , 32751                        , L"RightArrow" }
        ,   { tCIDGraphDev::ESysBmps::LeftArrow    , 32750                        , L"LeftArrow" }
        ,   { tCIDGraphDev::ESysBmps::Reduce       , 32749                        , L"Reduce" }
        ,   { tCIDGraphDev::ESysBmps::Zoom         , 32748                        , L"Zoom" }
        ,   { tCIDGraphDev::ESysBmps::Restore      , 32747                        , L"Restore" }
        ,   { tCIDGraphDev::ESysBmps::Reduced      , 32746                        , L"Reduced" }
        ,   { tCIDGraphDev::ESysBmps::Zoomed       , 32745                        , L"Zoomed" }
        ,   { tCIDGraphDev::ESysBmps::Restored     , 32744                        , L"Restored" }
        ,   { tCIDGraphDev::ESysBmps::UpArrowD     , 32743                        , L"UpArrowD" }
        ,   { tCIDGraphDev::ESysBmps::DownArrowD   , 32742                        , L"DownArrowD" }
        ,   { tCIDGraphDev::ESysBmps::RightArrowD  , 32741                        , L"RightArrowD" }
        ,   { tCIDGraphDev::ESysBmps::LeftArrowD   , 32740                        , L"LeftArrowD" }
        ,   { tCIDGraphDev::ESysBmps::MnArrow      , 32739                        , L"MnArrow" }
        ,   { tCIDGraphDev::ESysBmps::Combo        , 32738                        , L"Combo" }
        ,   { tCIDGraphDev::ESysBmps::UpArrowI     , 32737                        , L"UpArrowI" }
        ,   { tCIDGraphDev::ESysBmps::DownArrowI   , 32736                        , L"DownArrowI" }
        ,   { tCIDGraphDev::ESysBmps::RightArrowI  , 32735                        , L"RightArrowI" }
        ,   { tCIDGraphDev::ESysBmps::LeftArrowI   , 32734                        , L"LeftArrowI" }
        ,   { tCIDGraphDev::ESysBmps::Size         , 32766                        , L"Size" }
        ,   { tCIDGraphDev::ESysBmps::BtSize       , 32761                        , L"BtSize" }
        ,   { tCIDGraphDev::ESysBmps::Check        , 32760                        , L"Check" }

        // These are our own bitmaps
        ,   { tCIDGraphDev::ESysBmps::Halftone1    , kCIDGraphDev::ridBmp_Halftone1 , L"Halftone1" }
        ,   { tCIDGraphDev::ESysBmps::Halftone2    , kCIDGraphDev::ridBmp_Halftone2 , L"Halftone2" }
        ,   { tCIDGraphDev::ESysBmps::Halftone3    , kCIDGraphDev::ridBmp_Halftone3 , L"Halftone3" }
        ,   { tCIDGraphDev::ESysBmps::Halftone4    , kCIDGraphDev::ridBmp_Halftone4 , L"Halftone4" }
        ,   { tCIDGraphDev::ESysBmps::Halftone5    , kCIDGraphDev::ridBmp_Halftone5 , L"Halftone5" }
        ,   { tCIDGraphDev::ESysBmps::Halftone6    , kCIDGraphDev::ridBmp_Halftone6 , L"Halftone6" }
        ,   { tCIDGraphDev::ESysBmps::Halftone7    , kCIDGraphDev::ridBmp_Halftone7 , L"Halftone7" }
        ,   { tCIDGraphDev::ESysBmps::Halftone8    , kCIDGraphDev::ridBmp_Halftone8 , L"Halftone8" }
        ,   { tCIDGraphDev::ESysBmps::Halftone9    , kCIDGraphDev::ridBmp_Halftone9 , L"Halftone9" }
    };

    TEArray<TBmpMap, tCIDGraphDev::ESysBmps, tCIDGraphDev::ESysBmps::Count> amapList
    (
        amapVals
    );


    // -----------------------------------------------------------------------
    //  Our persistent format versions
    // -----------------------------------------------------------------------
    const tCIDLib::TCard2   c2FCBFmtVersion = 1;
}


// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------
static tCIDLib::TResId ridXlatSysBmp(const tCIDGraphDev::ESysBmps eBitmap)
{
    //
    //  Check the passed enum.
    //
    if ((eBitmap < tCIDGraphDev::ESysBmps::Min)
    ||  (eBitmap > tCIDGraphDev::ESysBmps::Max))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(tCIDLib::c4EnumOrd(eBitmap))
            , TString(L"tCIDGraphDev::ESysBmps")
        );
    }
    return CIDGraphDev_Bitmap::amapList[eBitmap].ridThis;
}


static tCIDImage::EBmpComp
eXlatCompression(const  tCIDLib::TCard4 c4Compression
                , const tCIDLib::TCard4 c4BitsPerPixel)
{
    switch(c4Compression)
    {
        case 0 :
            return tCIDImage::EBmpComp::None;

        case 1 :
            return tCIDImage::EBmpComp::RLE8;

        case 2 :
            return tCIDImage::EBmpComp::RLE4;

        case 3 :
            //
            //  Do a little magic here. A 40 byte OS/2 header and a Win 3.x
            //  header could both look the same but have a different
            //  interpretation of the compression value of 3. If its an
            //  OS/2 bmp with a compression of 3, then it must be a 1 bit
            //  format (i.e. a fax style image.) If its a Win 3.x with
            //  that compression value it cannot be 1, so we can tell them
            //  apart.
            //
            if (c4BitsPerPixel == 1)
                return tCIDImage::EBmpComp::Huffman1D;
            return tCIDImage::EBmpComp::BitFields;

        case 4 :
            return tCIDImage::EBmpComp::RLE24;

        default :
            facCIDGraphDev().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcBmp_UnknownCompression
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Unknown
                , TCardinal(c4Compression)
            );
            break;
    }

    // Make the compiler happy
    return tCIDImage::EBmpComp::None;
}



// ---------------------------------------------------------------------------
//   CLASS: TRefBmpHandle
//  PREFIX: cnted
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TRefBmpHandle: Ctors and Dtor
// ---------------------------------------------------------------------------
TBitmap::TRefBmpHandle::TRefBmpHandle(const tCIDLib::TBoolean bSrcAlpha) :

    m_bHasSrcAlpha(bSrcAlpha)
    , m_c4SerialNum(0)
    , m_hbmpThis(kCIDGraphDev::hbmpInvalid)
    , m_ppixaData(0)
{
    // Put some default info into the header
    m_bhiThis.eType = tCIDImage::EBmpTypes::Win_3x;
    m_bhiThis.eCompression = tCIDImage::EBmpComp::None;
    m_bhiThis.c4BitDepth = 1;
    m_bhiThis.c4ClrsImportant = 2;
    m_bhiThis.c4ClrsUsed = 2;
    m_bhiThis.c4Height = 0;
    m_bhiThis.c4Width = 0;
    m_bhiThis.c4HorzRes = 72;
    m_bhiThis.c4VertRes = 72;
    m_bhiThis.c4LineByteRounding = 4;
    m_bhiThis.c4CompressedSize = 0;
    m_bhiThis.c4AlphaMask = 0;
    m_bhiThis.c4BlueMask = 0;
    m_bhiThis.c4GreenMask = 0;
    m_bhiThis.c4RedMask = 0;
}

TBitmap::TRefBmpHandle::TRefBmpHandle(  const   tCIDGraphDev::TBmpHandle    hbmpToAdopt
                                        , const tCIDLib::TBoolean           bSrcAlpha
                                        , const tCIDGraphDev::TDeviceHandle hdevToUse
                                        ,       TPixelArray* const          ppixaData) :
    m_bHasSrcAlpha(bSrcAlpha)
    , m_c4SerialNum(0)
    , m_hbmpThis(hbmpToAdopt)
    , m_ppixaData(ppixaData)
{
    try
    {
        // We can't accept an invalid handle
        if (hbmpToAdopt == kCIDGraphDev::hbmpInvalid)
        {
            facCIDGraphDev().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcBmp_InvalidHandle
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::BadParms
            );
        }

        // Store the new bitmap handle and update our header info
        m_hbmpThis = hbmpToAdopt;
        UpdateInfo(hdevToUse);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // We own the handle and pixel array, so clean them up and rethrow
        if (m_hbmpThis != kCIDGraphDev::hbmpInvalid)
            ::DeleteObject(m_hbmpThis);
        delete m_ppixaData;
        throw;
    }
}

TBitmap::TRefBmpHandle::~TRefBmpHandle()
{
    // Delete the handle if its set
    if (m_hbmpThis != kCIDGraphDev::hbmpInvalid)
    {
        if (!::DeleteObject(m_hbmpThis))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDGraphDev().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcBmp_Delete
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::Internal
            );
        }
    }

    //
    //  Delete the pixel array if we have one. It may or may not own the
    //  underlying pixel data. It will delete it if so.
    //
    delete m_ppixaData;
    m_ppixaData = nullptr;
}


// ---------------------------------------------------------------------------
//  TRefBmpHandle: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TBitmap::TRefBmpHandle::bSrcAlpha() const
{
    return m_bHasSrcAlpha;
}

tCIDLib::TBoolean
TBitmap::TRefBmpHandle::bSrcAlpha(const tCIDLib::TBoolean bToSet)
{
    m_bHasSrcAlpha = bToSet;
    return m_bHasSrcAlpha;
}


const TBitmapImage::TBmpHdrInfo& TBitmap::TRefBmpHandle::bhiThis() const
{
    return m_bhiThis;
}


// Get or set the serial number
tCIDLib::TCard4 TBitmap::TRefBmpHandle::c4SerialNum() const
{
    return m_c4SerialNum;
}

tCIDLib::TCard4
TBitmap::TRefBmpHandle::c4SerialNum(const tCIDLib::TCard4 c4ToSet)
{
    m_c4SerialNum = c4ToSet;
    return m_c4SerialNum;
}


// Create a copy of our raw bitmap handle
tCIDGraphDev::TBmpHandle TBitmap::TRefBmpHandle::hbmpDup() const
{
    return TBitmap::hbmpDupHandle(m_hbmpThis);
}


// Provide direct access to our raw bitmap handle
tCIDGraphDev::TBmpHandle TBitmap::TRefBmpHandle::hbmpThis() const
{
    return m_hbmpThis;
}


// Bump this guy's serial number
tCIDLib::TVoid TBitmap::TRefBmpHandle::IncSerialNum()
{
    m_c4SerialNum++;
}


// Provide direct access to our raw pixel data
const TPixelArray* TBitmap::TRefBmpHandle::ppixaData() const
{
    //
    //  If we have one, make sure all drawing is completed to the bitmap
    //  before we let the caller mess with it.
    //
    if (m_ppixaData && (m_hbmpThis != kCIDGraphDev::hbmpInvalid))
        ::GdiFlush();
    return m_ppixaData;
}

TPixelArray* TBitmap::TRefBmpHandle::ppixaData()
{
    //
    //  If we have one, make sure all drawing is completed to the bitmap
    //  before we let the caller mess with it.
    //
    if (m_ppixaData && (m_hbmpThis != kCIDGraphDev::hbmpInvalid))
        ::GdiFlush();
    return m_ppixaData;
}


// Get or set the user data value
const TString& TBitmap::TRefBmpHandle::strUserData() const
{
    return m_strUserData;
}

const TString& TBitmap::TRefBmpHandle::strUserData(const TString& strToSet)
{
    m_strUserData = strToSet;
    return m_strUserData;
}


// Return the size of the bitmap in pixels
TSize TBitmap::TRefBmpHandle::szBitmap() const
{
    return TSize(m_bhiThis.c4Width, m_bhiThis.c4Height);
}


// ---------------------------------------------------------------------------
//  TRefBmpHandle: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Reload our bitmap information from the bitmap
tCIDLib::TVoid
TBitmap::TRefBmpHandle::UpdateInfo(const tCIDGraphDev::TDeviceHandle hdevToUse)
{
    // Get the basic info we need to query the more complex info
    BITMAP BmpInfo = {0};
    if (!::GetObject(m_hbmpThis, sizeof(BITMAP), &BmpInfo))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcBmp_QueryInfo
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    BITMAPINFO Info = {0};
    Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    Info.bmiHeader.biPlanes = 1;
    Info.bmiHeader.biWidth = BmpInfo.bmWidth;
    Info.bmiHeader.biHeight = BmpInfo.bmHeight;
    Info.bmiHeader.biCompression = BI_RGB;

    tCIDGraphDev::TDeviceHandle hdevLocal = hdevToUse;
    HWND hwndTmp = 0;
    if (!hdevToUse)
    {
        // Get a temp DC to use
        hwndTmp = ::GetDesktopWindow();
        hdevLocal = ::GetWindowDC(hwndTmp);
    }

    //
    //  DO NOT set the biBitCount field or it will return pallete data and
    //  overflow the buffer and corrupt the stack.
    //
    if (!::GetDIBits(hdevLocal, m_hbmpThis, 0, 0, 0, (BITMAPINFO*)&Info, DIB_RGB_COLORS))
    {
        if (!hdevToUse)
            ::ReleaseDC(hwndTmp, hdevLocal);

        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcBmp_QueryInfo
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Release the temp device if we need to
    if (!hdevToUse)
        ::ReleaseDC(hwndTmp, hdevLocal);

    //
    //  We need to translate the information over into a raw bitmap
    //  info structure we keep around.
    //
    m_bhiThis.eType = tCIDImage::EBmpTypes::Win_3x;
    m_bhiThis.eCompression = eXlatCompression
    (
        Info.bmiHeader.biCompression
        , Info.bmiHeader.biBitCount
    );
    m_bhiThis.c4BitDepth = Info.bmiHeader.biBitCount;
    m_bhiThis.c4ClrsImportant = Info.bmiHeader.biClrImportant;
    m_bhiThis.c4ClrsUsed = Info.bmiHeader.biClrUsed;
    m_bhiThis.c4Height = Info.bmiHeader.biHeight;
    m_bhiThis.c4Width = Info.bmiHeader.biWidth;
    m_bhiThis.c4HorzRes = Info.bmiHeader.biXPelsPerMeter;
    m_bhiThis.c4VertRes = Info.bmiHeader.biYPelsPerMeter;
    m_bhiThis.c4LineByteRounding = 4;
    m_bhiThis.c4CompressedSize = Info.bmiHeader.biSizeImage;
    m_bhiThis.c4AlphaMask = 0;
    m_bhiThis.c4BlueMask = 0;
    m_bhiThis.c4GreenMask = 0;
    m_bhiThis.c4RedMask = 0;
}



// ---------------------------------------------------------------------------
//   CLASS: TBitmap
//  PREFIX: bmp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBitmap: Public, static methods
// ---------------------------------------------------------------------------
tCIDGraphDev::TBmpHandle
TBitmap::hbmpFromImage( const   TGraphicDevice&     gdevTarget
                        , const TPixelArray&        pixaSrc
                        , const TClrPalette&        palSrc
                        , const tCIDLib::TFloat8    f8Gamma
                        , const tCIDLib::TBoolean   bTransClr
                        , const tCIDLib::TCard4     c4TransClr)
{
    tCIDGraphDev::TBmpHandle hbmpRet = kCIDGraphDev::hbmpInvalid;
    const tCIDLib::TCard4       c4CX = pixaSrc.szImage().c4Width();
    const tCIDLib::TCard4       c4CY = pixaSrc.szImage().c4Height();
    const tCIDImage::EPixFmts   eFmt = pixaSrc.ePixFmt();
    const tCIDImage::EBitDepths eDepth = pixaSrc.eBitDepth();

    //
    //  Set up a pointer to the bit data. If it's not one of the special
    //  cases where we have to convert to a Windows compatible format, then
    //  we'll point it directly at the incoming pixel data.
    //
    tCIDLib::TCard4         c4NewDataSz = pixaSrc.c4ImageSz();
    tCIDLib::TCard4         c4NewPerLn = pixaSrc.c4LineWidth();
    tCIDLib::TCard4         c4PalColors = palSrc.c4ClrCount();
    tCIDImage::EBitDepths   eNewDepth = eDepth;
    tCIDImage::EPixFmts     eNewFmt = eFmt;

    // If palette based, there has to be palette colors
    if (tCIDLib::bAllBitsOn(eNewFmt, tCIDImage::EPixFmts::Palette) && !c4PalColors)
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcBmp_NoPalColors
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    tCIDLib::TCard1* pc1BitPtr = nullptr;
    try
    {
        tCIDLib::TCard1 c1Cur;
        tCIDLib::TCard4 c4Cur;
        tCIDLib::TCard1* pc1Cur;
        TPixelArray::TRGBQuad* pqCur;

        if (eDepth == tCIDImage::EBitDepths::Two)
        {
            //
            //  Windows does not have a 2 bit format, so we have to convert
            //  to a 4 bit palette format.
            //
            eNewFmt = tCIDImage::EPixFmts::ClrPal;
            eNewDepth = tCIDImage::EBitDepths::Four;

            c4NewPerLn = TPixelArray::c4CalcLineWidth(c4CX, eNewFmt, eNewDepth, 4);
            c4NewDataSz = c4NewPerLn * c4CY;
            pc1BitPtr = new tCIDLib::TCard1[c4NewDataSz];

            // Convert over the data now
            for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4CY; c4YInd++)
            {
                pc1Cur = pc1BitPtr + (c4YInd * c4NewPerLn);
                for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4CX; c4XInd += 2)
                {
                    // We'll do two source pixels per round, for 1 out byte
                    c1Cur = tCIDLib::TCard1(pixaSrc.c4At(c4XInd, c4YInd)) << 4;
                    if (c4XInd + 1 < c4CX)
                        c1Cur |= tCIDLib::TCard1(pixaSrc.c4At(c4XInd + 1, c4YInd));
                    *pc1Cur++ = c1Cur;
                }
            }

            // We'll only use 4 of the palette entries
            c4PalColors = 4;
        }
         else if ((eFmt == tCIDImage::EPixFmts::GrayAlpha)
              ||  ((eFmt == tCIDImage::EPixFmts::GrayScale)
              &&   (eDepth == tCIDImage::EBitDepths::Sixteen)))
        {
            //
            //  Windows doesn't support gray scale bitmaps per se. We handle
            //  the 8 bit and lower ones by making up a palette for them.
            //  For 8 bit with alpha and 16 bit w/wo alpha, we have to convert
            //  to true color.
            //
            eNewDepth = tCIDImage::EBitDepths::Eight;
            if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Alpha))
                eNewFmt = tCIDImage::EPixFmts::TrueAlpha;
            else
                eNewFmt = tCIDImage::EPixFmts::TrueClr;

            c4NewPerLn = TPixelArray::c4CalcLineWidth(c4CX, eNewFmt, eNewDepth, 4);
            c4NewDataSz = c4NewPerLn * c4CY;
            pc1BitPtr = new tCIDLib::TCard1[c4NewDataSz];

            if (eDepth == tCIDImage::EBitDepths::Sixteen)
            {
                //
                //  Take the top 8 bits of the color and if we have alpha
                //  then of the alpha if present.
                //
                if (tCIDLib::bAllBitsOn(eNewFmt, tCIDImage::EPixFmts::Alpha))
                {
                    for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4CY; c4YInd++)
                    {
                        pc1Cur = pc1BitPtr + (c4YInd * c4NewPerLn);
                        for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4CX; c4XInd++)
                        {
                            c4Cur = pixaSrc.c4At(c4XInd, c4YInd);
                            c1Cur = tCIDLib::TCard1(c4Cur >> 24);
                            *pc1Cur++ = c1Cur;
                            *pc1Cur++ = c1Cur;
                            *pc1Cur++ = c1Cur;
                            *pc1Cur++ = tCIDLib::TCard1((c4Cur >> 8) & 0xFF);
                        }
                    }
                }
                 else
                {
                    for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4CY; c4YInd++)
                    {
                        pc1Cur = pc1BitPtr + (c4YInd * c4NewPerLn);
                        for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4CX; c4XInd++)
                        {
                            c4Cur = pixaSrc.c4At(c4XInd, c4YInd);
                            c1Cur = tCIDLib::TCard1(c4Cur >> 8);
                            *pc1Cur++ = c1Cur;
                            *pc1Cur++ = c1Cur;
                            *pc1Cur++ = c1Cur;
                        }
                    }
                }
            }
             else
            {
                for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4CY; c4YInd++)
                {
                    pqCur = reinterpret_cast<TPixelArray::TRGBQuad*>
                    (
                        pc1BitPtr + (c4YInd * c4NewPerLn)
                    );
                    for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4CX; c4XInd++)
                    {
                        c4Cur = pixaSrc.c4At(c4XInd, c4YInd);
                        pqCur->c1Alpha = tCIDLib::TCard1(c4Cur & 0xFF);
                        c1Cur = tCIDLib::TCard1((c4Cur >> 8) & 0xFF);
                        pqCur->c1Red = c1Cur;
                        pqCur->c1Green = c1Cur;
                        pqCur->c1Blue = c1Cur;
                        pqCur++;
                    }
                }
            }
        }
         else if (eFmt == tCIDImage::EPixFmts::GrayScale)
        {
            //
            //  It's gray scale, but the source image will already have
            //  a gray scale palette for it, so we can treat it like a
            //  palette scheme here.
            //
            eNewFmt = tCIDImage::EPixFmts::ClrPal;
            eNewDepth = eDepth;
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // Clean up the buffer if we got it allocated
        delete [] pc1BitPtr;
        throw;
    }

    //
    //  If the source gamma isn't 2.2 (which would cause nothing to happen),
    //  and we are more than 1 bit depth, then do gamma. We have to deal with
    //  a possible non-exact floating point, so multiply it up and take the
    //  integral part.
    //
    const tCIDLib::TBoolean bGamma = kCIDLib::False;
    (
        (tCIDLib::c4EnumOrd(eDepth) > 1) && (tCIDLib::TCard4(f8Gamma * 10) != 22)
    );

    //
    //  If there are more pallete colors than this image can use, reduce them.
    //  It's ok if there are fewer, since we'll tell the system how many colors
    //  in the palette are actually used.
    //
    const tCIDLib::TCard4 c4BitDepthClrs = (0x1U << tCIDLib::c4EnumOrd(eNewDepth));
    if (c4PalColors > c4BitDepthClrs)
        c4PalColors = c4BitDepthClrs;

    // And there cannot be more than 256 coors in a palette based image
    if (c4PalColors > 256)
        c4PalColors = 256;

    //
    //  There are some scenarios where we have to allocate a new buffer, if
    //  we didn't already above due to conversion.
    //
    //  -   If it's alpha based, we have to pre-multiply the samples we pass
    //      in
    //  -   If we are doing gamma and it's not a palette based image. If a
    //      palette, then we gamma the palette values.
    //
    if (!pc1BitPtr
    &&  ((bGamma && !tCIDLib::bAllBitsOn(eNewFmt, tCIDImage::EPixFmts::Palette))
    ||   tCIDLib::bAllBitsOn(eNewFmt, tCIDImage::EPixFmts::Alpha)))
    {
        pc1BitPtr = new tCIDLib::TCard1[c4NewDataSz];
        TRawMem::CopyMemBuf(pc1BitPtr, pixaSrc.pc1Buffer(), c4NewDataSz);
    }

    //
    //  Put a janitor on the buffer now. If it never got allocated, then we
    //  won't end up dealocating anything, but if it did, then we will. This
    //  way, we can insure it get's cleaned up, but still substitute our
    //  incoming pixel array buffer bellow if no special case is required.
    //
    TArrayJanitor<tCIDLib::TCard1> janPixBuf(pc1BitPtr);

    //
    //  Calc the new per-pixel storage, which is what we need to pass to
    //  the bitmap creation function, and we'll need during gamma correction.
    //
    const tCIDLib::TCard4 c4NewPerPix
    (
        TPixelArray::c4CalcBitsPerPixel(eNewFmt, eNewDepth)
    );

    //
    //  If we are doign to be doing gamma, then pre-calculate the gamma
    //  correction table.
    //
    tCIDLib::TCard1 ac1GTbl[256];
    if (bGamma)
    {
        //
        //  We pre-build a table for the passed gamma, so that we can just
        //  do a lookup on each color component.
        //
        //  Note that we build the table for the target format that are going
        //  to build, which means always 256 or less values, since the only
        //  > 8 bit values (16 bit gray scale) get down converted anyway.
        //
        tCIDLib::TCard4 c4Count;
        if (tCIDLib::bAllBitsOn(eNewFmt, tCIDImage::EPixFmts::Palette))
            c4Count = c4PalColors;
        else
            c4Count = (0x1U << tCIDLib::c4EnumOrd(eNewDepth));
        const tCIDLib::TCard1 c1Max(tCIDLib::TCard1(c4Count - 1));
        const tCIDLib::TFloat8 f8Max(c1Max);
        const tCIDLib::TFloat8 f8Exp(1.0 / ((1.0 / f8Gamma) * 2.2));
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            tCIDLib::TFloat8 f8Cur = tCIDLib::TFloat8(c4Index) / f8Max;
            f8Cur = TMathLib::f8Power(f8Cur, f8Exp);
            f8Cur *= f8Max;
            TMathLib::Round(f8Cur);
            if (f8Cur >= c1Max)
                ac1GTbl[c4Index] = c1Max;
            else
                ac1GTbl[c4Index] = tCIDLib::TCard1(f8Cur);
        }
    }

    //
    //  Now if doing gamma and this is not a palette based image, let's loop
    //  through all of the samples and replace them with the equiv value in
    //  the table.
    //
    //  Note that we only have to do non-palette based formats here. For
    //  palette based, we'll adjust the generated palette below.
    //
    //  We won't see either 1 or 2 bits since we converted 2 to 4 and
    //  we don't do gamma for 1.
    //
    if (bGamma && !tCIDLib::bAllBitsOn(eNewFmt, tCIDImage::EPixFmts::Palette))
    {
        CIDAssert(pc1BitPtr != nullptr, L"The gamma buffer was not allocated");

        tCIDLib::TCard1 c1Cur;
        tCIDLib::TCard1 c1Tmp;
        tCIDLib::TCard1 c1Tmp2;
        tCIDLib::TCard1 c1Tmp3;
        tCIDLib::TCard4 c4XInd;
        tCIDLib::TCard4 c4YInd;
        tCIDLib::TCard1* pc1Cur;
        switch(eNewDepth)
        {
            case tCIDImage::EBitDepths::Four :
            {
                for (c4YInd = 0; c4YInd < c4CY; c4YInd++)
                {
                    pc1Cur = pc1BitPtr + (c4YInd * c4NewPerLn);
                    if (bTransClr)
                    {
                        for (c4XInd = 0; c4XInd < c4CX; c4XInd += 2)
                        {
                            c1Cur = *pc1Cur;
                            c1Tmp = ac1GTbl[c1Cur >> 4];
                            c1Tmp2 = ac1GTbl[c1Cur & 0xF];
                            *pc1Cur++ = (c1Tmp << 4) | c1Tmp2;
                        }
                    }
                     else
                    {
                        for (c4XInd = 0; c4XInd < c4CX; c4XInd += 2)
                        {
                            c1Cur = *pc1Cur;
                            c1Tmp = c1Cur >> 4;
                            c1Tmp2 = c1Cur & 0xF;

                            if (c1Tmp != c4TransClr)
                                c1Tmp = ac1GTbl[c1Tmp];

                            if (c1Tmp2 != c4TransClr)
                                c1Tmp2 = ac1GTbl[c1Tmp2];

                            *pc1Cur++ = (c1Tmp << 4) | c1Tmp2;
                        }
                    }
                }
                break;
            }

            case tCIDImage::EBitDepths::Five :
            {
                tCIDLib::TCard2 c2Cur;
                for (c4YInd = 0; c4YInd < c4CY; c4YInd++)
                {
                    pc1Cur = pc1BitPtr + (c4YInd * c4NewPerLn);
                    if (bTransClr)
                    {
                        for (c4XInd = 0; c4XInd < c4CX; c4XInd++)
                        {
                            c2Cur = *pc1Cur;
                            c2Cur <<= 8;
                            c2Cur |= *(pc1Cur + 1);

                            if (c2Cur == c4TransClr)
                            {
                                pc1Cur += 2;
                            }
                             else
                            {
                                c1Tmp = tCIDLib::TCard1(c2Cur >> 10);
                                c1Tmp2 = tCIDLib::TCard1((c2Cur >> 5) & 0x1F);
                                c1Tmp3 = tCIDLib::TCard1(c2Cur & 0x1F);

                                c1Tmp = ac1GTbl[c1Tmp];
                                c1Tmp2 = ac1GTbl[c1Tmp2];
                                c1Tmp3 = ac1GTbl[c1Tmp3];

                                c2Cur = c1Tmp;
                                c2Cur <<= 5;
                                c2Cur |= c1Tmp2;
                                c2Cur <<= 5;
                                c2Cur |= c1Tmp3;
                                *pc1Cur++ = tCIDLib::TCard1(c2Cur >> 8);
                                *pc1Cur++ = tCIDLib::TCard1(c2Cur & 0xFF);
                            }
                        }
                    }
                     else
                    {
                        for (c4XInd = 0; c4XInd < c4CX; c4XInd++)
                        {
                            c2Cur = *pc1Cur;
                            c2Cur <<= 8;
                            c2Cur |= *(pc1Cur + 1);
                            c1Tmp = ac1GTbl[c2Cur >> 10];
                            c1Tmp2 = ac1GTbl[(c2Cur >> 5) & 0x1F];
                            c1Tmp3 = ac1GTbl[c2Cur & 0x1F];

                            c2Cur = c1Tmp;
                            c2Cur <<= 5;
                            c2Cur |= c1Tmp2;
                            c2Cur <<= 5;
                            c2Cur |= c1Tmp3;
                            *pc1Cur++ = tCIDLib::TCard1(c2Cur >> 8);
                            *pc1Cur++ = tCIDLib::TCard1(c2Cur & 0xFF);
                        }
                    }
                }
                break;
            }

            case tCIDImage::EBitDepths::Eight :
            {
                if (tCIDLib::bAllBitsOn(eNewFmt, tCIDImage::EPixFmts::Color))
                {
                    // It's RGB or RGBA, so 3 or 4 bytes per
                    if (bTransClr)
                    {
                        const tCIDLib::TCard1 c1TB = tCIDLib::TCard1(c4TransClr & 0xFF);
                        const tCIDLib::TCard1 c1TG = tCIDLib::TCard1((c4TransClr >> 8) & 0xFF);
                        const tCIDLib::TCard1 c1TR = tCIDLib::TCard1((c4TransClr >> 16) & 0xFF);

                        for (c4YInd = 0; c4YInd < c4CY; c4YInd++)
                        {
                            pc1Cur = pc1BitPtr + (c4YInd * c4NewPerLn);
                            for (c4XInd = 0; c4XInd < c4CX; c4XInd++)
                            {
                                c1Tmp = *pc1Cur;
                                c1Tmp2 = *(pc1Cur + 1);
                                c1Tmp3 = *(pc1Cur + 2);

                                if ((c1Tmp == c1TR)
                                &&  (c1Tmp2 == c1TG)
                                &&  (c1Tmp3 == c1TB))
                                {
                                    pc1Cur += 3;
                                }
                                 else
                                {
                                    *pc1Cur++ = ac1GTbl[c1Tmp];
                                    *pc1Cur++ = ac1GTbl[c1Tmp2];
                                    *pc1Cur++ = ac1GTbl[c1Tmp3];
                                }

                                if (tCIDLib::bAllBitsOn(eNewFmt, tCIDImage::EPixFmts::Alpha))
                                    pc1Cur++;
                            }
                        }
                    }
                     else
                    {
                        for (c4YInd = 0; c4YInd < c4CY; c4YInd++)
                        {
                            pc1Cur = pc1BitPtr + (c4YInd * c4NewPerLn);
                            for (c4XInd = 0; c4XInd < c4CX; c4XInd++)
                            {
                                *pc1Cur = ac1GTbl[*pc1Cur];
                                *pc1Cur++;
                                *pc1Cur = ac1GTbl[*pc1Cur];
                                *pc1Cur++;
                                *pc1Cur = ac1GTbl[*pc1Cur];
                                *pc1Cur++;

                                if (tCIDLib::bAllBitsOn(eNewFmt, tCIDImage::EPixFmts::Alpha))
                                    pc1Cur++;
                            }
                        }
                    }
                }
                 else
                {
                    //
                    //  It's gray scale, so one byte per. If it had alpha
                    //  it got converted to RGBA.
                    //
                    for (c4YInd = 0; c4YInd < c4CY; c4YInd++)
                    {
                        pc1Cur = pc1BitPtr + (c4YInd * c4NewPerLn);
                        for (c4XInd = 0; c4XInd < c4CX; c4XInd++)
                        {
                            c1Cur = *pc1Cur;
                            if (c1Cur != c4TransClr)
                                *pc1Cur = ac1GTbl[c1Cur];
                            *pc1Cur++;
                        }
                    }
                }
                break;
            }
        };
    }

    //
    //  If it has source alpha then we need to pre-blend the values with
    //  their alpha values now that any gamma correction is done.
    //
    //  Since we converted any gray scale with alpha to RBG with alpha, we
    //  only have one format to deal with here, which is 32 bit per pixel
    //  color.
    //
    //  We allocated a new buffer above if we are alpha based, so we aren't
    //  trying to update the original here.
    //
    if (tCIDLib::bAllBitsOn(eNewFmt, tCIDImage::EPixFmts::Alpha))
    {
        CIDAssert(pc1BitPtr != nullptr, L"The pre-mul buffer was not allocated");

        tCIDLib::TCard4 c4Tmp;
        for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4CY; c4YInd++)
        {
            tCIDLib::TCard1* pc1Cur = pc1BitPtr + (c4YInd * c4NewPerLn);
            for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4CX; c4XInd++)
            {
                const tCIDLib::TCard1 c1Alpha = *(pc1Cur + 3);

                //
                //  Don't bother if fully opaque. We can skip a lot of
                //  overhead in this case and often it will be the case.
                //
                if (c1Alpha == 0xFF)
                {
                    pc1Cur += 4;
                    continue;
                }

                c4Tmp = *pc1Cur;
                c4Tmp *= c1Alpha;
                c4Tmp /= 0xFF;
                *pc1Cur++ = tCIDLib::TCard1(c4Tmp);

                c4Tmp = *pc1Cur;
                c4Tmp *= c1Alpha;
                c4Tmp /= 0xFF;
                *pc1Cur++ = tCIDLib::TCard1(c4Tmp);

                c4Tmp = *pc1Cur;
                c4Tmp *= c1Alpha;
                c4Tmp /= 0xFF;
                *pc1Cur = tCIDLib::TCard1(c4Tmp);

                // Skip the last color and the alpha byte
                pc1Cur += 2;
            }
        }
    }

    // If no special case, we can use our incoming pixel data as is
    if (!pc1BitPtr)
        pc1BitPtr = const_cast<tCIDLib::TCard1*>(pixaSrc.pc1Buffer());

    //
    //  Allocate a buffer big enough for the info header and the color palette
    //  if any and point a bitmap info structure at it.
    //
    tCIDLib::TCard4 c4Size =  sizeof(BITMAPINFOHEADER);
    if (tCIDLib::bAllBitsOn(eNewFmt, tCIDImage::EPixFmts::Palette))
        c4Size += 1024;

    tCIDLib::TVoid* pBuf = new tCIDLib::TCard1[c4Size];
    TArrayJanitor<tCIDLib::TVoid> janInfoBuf(pBuf);
    TRawMem::SetMemBuf(pBuf, tCIDLib::TCard1(0), c4Size);
    BITMAPINFO* pSourceFmt = reinterpret_cast<BITMAPINFO*>(pBuf);

    pSourceFmt->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pSourceFmt->bmiHeader.biWidth = c4CX;
    pSourceFmt->bmiHeader.biHeight = c4CY;
    if ((pixaSrc.eRowOrder() == tCIDImage::ERowOrders::TopDown) && (c4CY > 1))
        pSourceFmt->bmiHeader.biHeight *= -1;
    pSourceFmt->bmiHeader.biPlanes = 1;
    pSourceFmt->bmiHeader.biBitCount = tCIDLib::TCard1(c4NewPerPix);
    pSourceFmt->bmiHeader.biCompression = BI_RGB;

    if (tCIDLib::bAllBitsOn(eNewFmt, tCIDImage::EPixFmts::Palette))
    {
        // If our palette isn't full sized, tell him how many are relevant
        if (c4PalColors != c4BitDepthClrs)
        {
            pSourceFmt->bmiHeader.biClrUsed = c4PalColors;
            pSourceFmt->bmiHeader.biClrImportant = c4PalColors;
        }

        tCIDLib::TCard4 c4Index = 0;
        for (; c4Index < c4PalColors; c4Index++)
        {
            const TRGBClr& rgbCur = palSrc.rgbAt(c4Index);
            if (bGamma)
            {
                pSourceFmt->bmiColors[c4Index].rgbRed = ac1GTbl[rgbCur.c1Red()];
                pSourceFmt->bmiColors[c4Index].rgbGreen = ac1GTbl[rgbCur.c1Green()];
                pSourceFmt->bmiColors[c4Index].rgbBlue = ac1GTbl[rgbCur.c1Blue()];
            }
             else
            {
                pSourceFmt->bmiColors[c4Index].rgbRed = rgbCur.c1Red();
                pSourceFmt->bmiColors[c4Index].rgbGreen = rgbCur.c1Green();
                pSourceFmt->bmiColors[c4Index].rgbBlue = rgbCur.c1Blue();
            }
        }
    }

    //
    //  If the target device is capable of handling the bit depth of the
    //  image, then create a device dependent bitmap. Else, we have to create
    //  a DIB section so that we can make it the required depth.
    //
    if (c4NewPerPix > gdevTarget.c4BitsPerPixel())
    {
        //
        //  <TBD>   Note that we could make this a lot more efficient if we
        //          when ahead and allocated the DIB section bit buffer above
        //          and built the image directly into it. This would complicate
        //          things a lot since it's a completely different type of
        //          buffer (memory mapped), but it would avoid building into
        //          one buffer then creating another one for the DIB and
        //          copying into it.
        //
        //  In this case, we let Windows create the storage, and then we
        //  copy our source data into it. We know our data is of the correct
        //  format, since we are creating a DIB of the same format as the
        //  source data.
        //
        tCIDLib::TVoid* pBits;
        hbmpRet = ::CreateDIBSection
        (
            gdevTarget.hdevThis()
            , pSourceFmt
            , DIB_RGB_COLORS
            , &pBits
            , 0
            , 0
        );

        if (hbmpRet != kCIDGraphDev::hbmpInvalid)
        {
            GdiFlush();
            TRawMem::CopyMemBuf(pBits, pc1BitPtr, c4NewDataSz);
        }
    }
     else
    {
        hbmpRet = ::CreateDIBitmap
        (
            gdevTarget.hdevThis()
            , &pSourceFmt->bmiHeader
            , CBM_INIT
            , pc1BitPtr
            , pSourceFmt
            , DIB_RGB_COLORS
        );
    }

    if (hbmpRet == kCIDGraphDev::hbmpInvalid)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcBmp_CreateFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return hbmpRet;
}


tCIDGraphDev::TBmpHandle
TBitmap::hbmpCreate(const   TFacility&      facSrc
                    , const tCIDLib::TResId ridToCreate
                    , const TGraphicDevice& gdevCompat)
{
    // Find the memory of the resource indicated
    tCIDLib::TCard4     c4Len;
    tCIDLib::EResTypes  eType;
    const tCIDLib::TCard1*  pc1Start = facSrc.pc1FindRes
    (
        eType, c4Len, ridToCreate, kCIDLib::True
    );

    if (!pc1Start)
        return kCIDGraphDev::hbmpInvalid;


    tCIDGraphDev::TBmpHandle hbmpRes = 0;
    if (eType == tCIDLib::EResTypes::Bitmap)
    {
        // Get the file header data so we can skip to the bits
        const BITMAPFILEHEADER* pbfhCreate = reinterpret_cast<const BITMAPFILEHEADER*>
        (
            pc1Start
        );

        const BITMAPINFO* pbmpCreate = reinterpret_cast<const BITMAPINFO*>
        (
            pc1Start + sizeof(BITMAPFILEHEADER)
        );

        hbmpRes = ::CreateDIBitmap
        (
            gdevCompat.hdevThis()
            , (BITMAPINFOHEADER*)pbmpCreate
            , CBM_INIT
            , pc1Start + pbfhCreate->bfOffBits
            , pbmpCreate
            , DIB_RGB_COLORS
        );
    }
     else if ((eType == tCIDLib::EResTypes::JPEG)
          ||  (eType == tCIDLib::EResTypes::PNG))
    {
        //
        //  It's not a bitmap, so do the rest generically. Ask the image
        //  factory to create the image for us, and we'll create the bitmap
        //  from the image data.
        //
        TCIDImage* pimgNew = nullptr;
        facCIDImgFact().bDecodeImg(pc1Start, c4Len, pimgNew, kCIDLib::True);
        TJanitor<TCIDImage> janImg(pimgNew);

        hbmpRes = TBitmap::hbmpFromImage
        (
            gdevCompat
            , pimgNew->pixaBits()
            , pimgNew->palClrs()
            , pimgNew->f8Gamma()
            , pimgNew->bTransClr()
            , pimgNew->c4TransClr()
        );
    }
     else
    {
        // Not a resource type we can create an image from
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcRes_BadResType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::TypeMatch
            , facSrc.strName()
            , TCardinal(ridToCreate)
        );
    }
    return hbmpRes;
}


tCIDGraphDev::ESysBmps TBitmap::eFromName(const TString& strName)
{
    for (tCIDGraphDev::ESysBmps eIndex = tCIDGraphDev::ESysBmps::Min;
                            eIndex <= tCIDGraphDev::ESysBmps::Max; eIndex++)
    {
        if (strName == CIDGraphDev_Bitmap::amapList[eIndex].pszName)
            return eIndex;
    }
    return tCIDGraphDev::ESysBmps::None;
}

tCIDGraphDev::ESysBmps TBitmap::eFromName(const tCIDLib::TCh* const pszName)
{
    for (tCIDGraphDev::ESysBmps eIndex = tCIDGraphDev::ESysBmps::Min;
                            eIndex <= tCIDGraphDev::ESysBmps::Max; eIndex++)
    {
        if (TRawStr::bCompareStr(CIDGraphDev_Bitmap::amapList[eIndex].pszName, pszName))
            return eIndex;
    }
    return tCIDGraphDev::ESysBmps::None;
}


tCIDGraphDev::TBmpHandle TBitmap::hbmpDupHandle(const tCIDGraphDev::TBmpHandle hbmpToDup)
{
    tCIDGraphDev::TBmpHandle hbmpRet = tCIDGraphDev::TBmpHandle
    (
        ::CopyImage(hbmpToDup, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION)
    );

    if (!hbmpRet)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().LogKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcBmp_CopyFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    return hbmpRet;
}


const tCIDLib::TCh* TBitmap::pszToName(const tCIDGraphDev::ESysBmps eBmp)
{
    if ((eBmp < tCIDGraphDev::ESysBmps::Min) || (eBmp > tCIDGraphDev::ESysBmps::Max))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(tCIDLib::c4EnumOrd(eBmp))
            , TString(L"tCIDGraphDev::ESysBmps")
        );
    }
    return CIDGraphDev_Bitmap::amapList[eBmp].pszName;
}


// ---------------------------------------------------------------------------
//  TBitmap: Constructors and Destructor
// ---------------------------------------------------------------------------
TBitmap::TBitmap()
{
    // Just create a 1x1 mono bitmap
    m_cptrHandle.SetPointer
    (
        new TRefBmpHandle(::CreateBitmap(0, 0, 0, 1, 0))
    );
}

// Create a bitmap from a bitmap resource attached to the indicated facility
TBitmap::TBitmap(const  TFacility&      facSource
                , const tCIDLib::TResId ridBitmap)
{
    // Create a compatible device based on the desktop
    TIntGraphWndDev gdevCompat;

    //
    //  We use the passed module object to load up the resource based image, loaded
    //  into a bitmap, and get its handle.
    //
    tCIDGraphDev::TBmpHandle hbmpTmp = hbmpCreate(facSource, ridBitmap, gdevCompat);
    if (hbmpTmp == kCIDGraphDev::hbmpInvalid)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcBmp_LoadFromModule
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(ridBitmap)
            , facSource.strName()
        );
    }

    // Looks ok so store it on the counted pointer
    m_cptrHandle.SetPointer(new TRefBmpHandle(hbmpTmp));
}

// Load a bitmap from one of the available system bitmaps
TBitmap::TBitmap(const tCIDGraphDev::ESysBmps eBitmap)
{
    LoadSysBmp(eBitmap);
}

// Load a bitmap from a file
TBitmap::TBitmap(const TGraphicDevice& gdevTarget, const TString& strFileName)
{
    // Create an input stream over the source file
    TBinFileInStream strmSource
    (
        strFileName
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::None
        , tCIDLib::EFileFlags::SequentialScan
    );

    // Create a bitmap image object and stream it in from the stream
    TBitmapImage imgSrc;
    strmSource >> imgSrc;

    // Load up the bitmap and store the returned handle
    try
    {
        tCIDGraphDev::TBmpHandle hbmpTmp = hbmpFromImage
        (
            gdevTarget
            , imgSrc.pixaBits()
            , imgSrc.palClrs()
            , imgSrc.f8Gamma()
            , imgSrc.bTransClr()
            , imgSrc.c4TransClr()
        );

        // Looks ok so store it on the counted pointer
        m_cptrHandle.SetPointer
        (
            new TRefBmpHandle
            (
                hbmpTmp
                , tCIDLib::bAllBitsOn(imgSrc.ePixFmt(), tCIDImage::EPixFmts::Alpha)
                , 0
            )
        );
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}


//
//  Create a bitmap with a given size and format. It can be forced to be a
//  memory based bitmap instead of device based, so that the caller can be
//  sure he can get to the bitmap bits when desired.
//
TBitmap::TBitmap(const  TSize&                  szSize
                , const tCIDImage::EPixFmts     eFmt
                , const tCIDImage::EBitDepths   eDepth
                , const tCIDLib::TBoolean       bForceMem)
{
    //
    //  Calc the raw bits per pixel for the passed fmt/depth, and the overall
    //  image size, including padding.
    //
    const tCIDLib::TCard4 c4BitsPer = TPixelArray::c4CalcBitsPerPixel(eFmt, eDepth);
    const tCIDLib::TCard4 c4ImgSz = TPixelArray::c4CalcBytesForImage
    (
        szSize.c4Width()
        , szSize.c4Height()
        , eFmt
        , eDepth
    );

    //
    //  If the bits per pixel are greater than the desktop's bit depth,
    //  then we create a DIB section. Else, we create a device dependent
    //  bitmap for efficiency (unless forced noft to.)
    //
    tCIDGraphDev::TBmpHandle    hbmpNew;
    TJanitor<TPixelArray>       janArray;

    // Create a compatible device based on the desktop
    TIntGraphWndDev gdevCompat;
    if (bForceMem || (c4BitsPer > gdevCompat.c4BitsPerPixel()))
    {
        //
        //  Allocate a buffer big enough for the bitmap info and the colors.
        //  Just allocated it large enough for 256 colors, since that's the
        //  max and it doesn't matter if the buffer is larger than it needs to
        //  be.
        //
        const tCIDLib::TCard4 c4BufSz = sizeof(BITMAPINFO) + 1024;
        tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[c4BufSz];
        TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Buf);
        TRawMem::SetMemBuf(pc1Buf, tCIDLib::TCard1(0), c4BufSz);
        BITMAPINFO& SrcFmt = *reinterpret_cast<BITMAPINFO*>(pc1Buf);

        SrcFmt.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        SrcFmt.bmiHeader.biWidth = szSize.c4Width();
        SrcFmt.bmiHeader.biHeight = szSize.c4Height();
        SrcFmt.bmiHeader.biPlanes = 1;
        SrcFmt.bmiHeader.biBitCount = tCIDLib::TCard1(c4BitsPer);

        // Make it top down, since it's easier for us
        SrcFmt.bmiHeader.biHeight *= -1;

        // If it's a gray scale or palette format, then create a palette
        if (eFmt == tCIDImage::EPixFmts::GrayScale)
        {
            // Just generate a simple palette
            SrcFmt.bmiHeader.biClrUsed = 0x1UL << tCIDLib::c4EnumOrd(eDepth);
            SrcFmt.bmiHeader.biClrImportant = 0;

            for (tCIDLib::TCard4 c4CInd = 0; c4CInd < SrcFmt.bmiHeader.biClrUsed; c4CInd++)
            {
                RGBQUAD& QClr = SrcFmt.bmiColors[c4CInd];
                QClr.rgbBlue = tCIDLib::TCard1(c4CInd);
                QClr.rgbGreen = tCIDLib::TCard1(c4CInd);
                QClr.rgbRed = tCIDLib::TCard1(c4CInd);
            }
        }
         else if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Palette))
        {
            // Use the default palette
            SrcFmt.bmiHeader.biClrUsed = 0x1UL << tCIDLib::c4EnumOrd(eDepth);
            SrcFmt.bmiHeader.biClrImportant = 0;

            for (tCIDLib::TCard4 c4CInd = 0; c4CInd < SrcFmt.bmiHeader.biClrUsed; c4CInd++)
            {
                RGBQUAD& QClr = SrcFmt.bmiColors[c4CInd];
                const tCIDLib::TRawRGB& rgbCur = CIDImage_RawPalettes::argbDefault256[c4CInd];

                QClr.rgbBlue = rgbCur.c1Blue;
                QClr.rgbGreen = rgbCur.c1Green;
                QClr.rgbRed = rgbCur.c1Red;
            }
        }
         else
        {
            SrcFmt.bmiHeader.biClrUsed = 0;
            SrcFmt.bmiHeader.biClrImportant = 0;
        }

        tCIDLib::TVoid* pBits;
        hbmpNew = ::CreateDIBSection
        (
            gdevCompat.hdevThis()
            , &SrcFmt
            , DIB_RGB_COLORS
            , &pBits
            , 0
            , 0
        );

        //
        //  Create a non-adopting pixel array to manage the memory of the
        //  DIB section. It will allow us to come back later and easily
        //  access the data. The DIB owns the memory, we just want our
        //  pixel array to reference it for convenient access.
        //
        janArray.Set
        (
            new TPixelArray
            (
                eFmt
                , eDepth
                , tCIDImage::ERowOrders::TopDown
                , szSize
                , reinterpret_cast<tCIDLib::TCard1*>(pBits)
                , c4ImgSz
                , tCIDLib::EAdoptOpts::NoAdopt
            )
        );
    }
     else
    {
        //
        //  For performance reasons, we create monochrome bitmaps with
        //  CreateBitmap and color ones with CreateCompatibleBitmap.
        //
        if (c4BitsPer == 1)
        {
            hbmpNew = ::CreateBitmap
            (
                szSize.c4Width(), szSize.c4Height(), 1, c4BitsPer, 0
            );
        }
         else
        {
            hbmpNew = ::CreateCompatibleBitmap
            (
                gdevCompat.hdevThis()
                , szSize.c4Width()
                , szSize.c4Height()
            );
        }
    }

    if (hbmpNew == kCIDGraphDev::hbmpInvalid)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcBmp_CreateFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Looks ok so store it on the counted pointer
    m_cptrHandle.SetPointer
    (
        new TRefBmpHandle
        (
            hbmpNew
            , tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Alpha)
            , gdevCompat.hdevThis()
            , janArray.pobjOrphan()
        )
    );
}

//
//  Same as above but also provides a palette for those cases where the
//  format requested is palette based.
//
TBitmap::TBitmap(const  TSize&                  szSize
                , const tCIDImage::EPixFmts     eFmt
                , const tCIDImage::EBitDepths   eDepth
                , const TClrPalette&            palToUse
                , const tCIDLib::TBoolean       bForceMem)
{
    //
    //  Calc the raw bits per pixel for the passed fmt/depth, and the overall
    //  image size, including padding.
    //
    const tCIDLib::TCard4 c4BitsPer = TPixelArray::c4CalcBitsPerPixel(eFmt, eDepth);
    const tCIDLib::TCard4 c4ImgSz = TPixelArray::c4CalcBytesForImage
    (
        szSize.c4Width(), szSize.c4Height(), eFmt, eDepth
    );

    // Create a compatible device based on the desktop
    TIntGraphWndDev gdevCompat;

    //
    //  If the bits per pixel are greater than the desktop's bit depth,
    //  then we create a DIB section. Else, we create a device dependent
    //  bitmap for efficiency (unless forced not to.)
    //
    tCIDGraphDev::TBmpHandle    hbmpNew;
    TJanitor<TPixelArray>       janArray;
    if (bForceMem || (c4BitsPer > gdevCompat.c4BitsPerPixel()))
    {
        //
        //  Allocate a buffer big enough for the bitmap info and the colors.
        //  Just allocated it large enough for 256 colors, since that's the
        //  max and it doesn't matter if the buffer is larger than it needs to
        //  be.
        //
        const tCIDLib::TCard4 c4BufSz = sizeof(BITMAPINFO) + 1024;
        tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[c4BufSz];
        TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Buf);
        TRawMem::SetMemBuf(pc1Buf, tCIDLib::TCard1(0), c4BufSz);
        BITMAPINFO& SrcFmt = *reinterpret_cast<BITMAPINFO*>(pc1Buf);

        SrcFmt.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        SrcFmt.bmiHeader.biWidth = szSize.c4Width();
        SrcFmt.bmiHeader.biHeight = szSize.c4Height();
        SrcFmt.bmiHeader.biPlanes = 1;
        SrcFmt.bmiHeader.biBitCount = tCIDLib::TCard1(c4BitsPer);
        SrcFmt.bmiHeader.biClrUsed = palToUse.c4ClrCount();
        SrcFmt.bmiHeader.biClrImportant = palToUse.c4ClrCount();

        // Set up the palette colors
        for (tCIDLib::TCard4 c4CInd = 0; c4CInd < SrcFmt.bmiHeader.biClrUsed; c4CInd++)
        {
            const TRGBClr& rgbCur = palToUse[c4CInd];
            RGBQUAD& QClr = SrcFmt.bmiColors[c4CInd];
            QClr.rgbBlue = rgbCur.c1Blue();
            QClr.rgbGreen = rgbCur.c1Green();
            QClr.rgbRed = rgbCur.c1Red();
        }

        tCIDLib::TVoid* pBits;
        hbmpNew = ::CreateDIBSection
        (
            gdevCompat.hdevThis(), &SrcFmt, DIB_RGB_COLORS, &pBits, 0
            , 0
        );

        //
        //  Create a non-adopting pixel array to manage the memory of the
        //  DIB section. It will allow us to come back later and easily
        //  access the data. The DIB owns the memory, we just want our
        //  pixel array to reference it for convenient access.
        //
        janArray.Set
        (
            new TPixelArray
            (
                eFmt
                , eDepth
                , tCIDImage::ERowOrders::TopDown
                , szSize
                , reinterpret_cast<tCIDLib::TCard1*>(pBits)
                , c4ImgSz
                , tCIDLib::EAdoptOpts::NoAdopt
            )
        );
    }
     else
    {
        //
        //  For performance reasons, we create monochrome bitmaps with
        //  CreateBitmap and color ones with CreateCompatibleBitmap.
        //
        if (c4BitsPer == 1)
        {
            hbmpNew = ::CreateBitmap
            (
                szSize.c4Width(), szSize.c4Height(), 1, c4BitsPer, 0
            );
        }
         else
        {
            hbmpNew = ::CreateCompatibleBitmap
            (
                gdevCompat.hdevThis(), szSize.c4Width(), szSize.c4Height()
            );
        }
    }

    if (hbmpNew == kCIDGraphDev::hbmpInvalid)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcBmp_CreateFailed
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Looks ok so store it on the counted pointer
    m_cptrHandle.SetPointer
    (
        new TRefBmpHandle
        (
            hbmpNew
            , tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Alpha)
            , gdevCompat.hdevThis()
            , janArray.pobjOrphan()
        )
    );
}


// Stream in a bitmap from any provided binary stream deriative
TBitmap::TBitmap(const TGraphicDevice& gdevCompat, TBinInStream& strmSource)
{
    // Create a bitmap image object and stream it in from the stream
    TBitmapImage imgSrc;
    strmSource >> imgSrc;

    // Load up the bitmap and store the returned handle
    try
    {
        tCIDGraphDev::TBmpHandle hbmpTmp = hbmpFromImage
        (
            gdevCompat
            , imgSrc.pixaBits()
            , imgSrc.palClrs()
            , imgSrc.f8Gamma()
            , imgSrc.bTransClr()
            , imgSrc.c4TransClr()
        );

        // Looks ok so store it on the counted pointer
        m_cptrHandle.SetPointer
        (
            new TRefBmpHandle
            (
                hbmpTmp
                , tCIDLib::bAllBitsOn(imgSrc.ePixFmt(), tCIDImage::EPixFmts::Alpha)
                , gdevCompat.hdevThis()
            )
        );
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

// Create a bitmap from any image format deriative
TBitmap::TBitmap(const  TGraphicDevice&     gdevCompat
                , const TCIDImage&          imgSrc)
{
    // Load up the bitmap and store the returned handle
    try
    {
        tCIDGraphDev::TBmpHandle hbmpTmp = hbmpFromImage
        (
            gdevCompat
            , imgSrc.pixaBits()
            , imgSrc.palClrs()
            , imgSrc.f8Gamma()
            , imgSrc.bTransClr()
            , imgSrc.c4TransClr()
        );

        // Looks ok so store it on the counted pointer
        m_cptrHandle.SetPointer
        (
            new TRefBmpHandle
            (
                hbmpTmp
                , tCIDLib::bAllBitsOn(imgSrc.ePixFmt(), tCIDImage::EPixFmts::Alpha)
                , gdevCompat.hdevThis()
            )
        );
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

//
//  Create a bitmap from a raw pixel array. A palette must be provided in
//  case the array is a pixel based format.
//
TBitmap::TBitmap(const  TGraphicDevice&     gdevCompat
                , const TPixelArray&        pixaFrom
                , const TClrPalette&        palToUse)
{
    // Load up the bitmap and store the returned handle
    try
    {
        tCIDGraphDev::TBmpHandle hbmpTmp = hbmpFromImage
        (
            gdevCompat, pixaFrom, palToUse, 2.2, kCIDLib::False, 0
        );

        // Looks ok so store it on the counted pointer
        m_cptrHandle.SetPointer
        (
            new TRefBmpHandle
            (
                hbmpTmp
                , tCIDLib::bAllBitsOn(pixaFrom.ePixFmt(), tCIDImage::EPixFmts::Alpha)
                , gdevCompat.hdevThis()
            )
        );
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

//
//  Create a bitmap of the indicated size, compatible with the passed
//  graphics device.
//
TBitmap::TBitmap(   const   TGraphicDevice& gdevCompatible
                    , const TSize&          szSize)
{
    // Create a compatible bitmap
    try
    {
        tCIDGraphDev::TBmpHandle hbmpCompat = ::CreateCompatibleBitmap
        (
            gdevCompatible.hdevThis()
            , szSize.c4Width()
            , szSize.c4Height()
        );

        if (hbmpCompat == kCIDGraphDev::hbmpInvalid)
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcBmp_CreateCompat
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , szSize
            );
        }

        // Looks ok so store it on the counted pointer
        m_cptrHandle.SetPointer(new TRefBmpHandle(hbmpCompat));
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}

//
//  Either adopt or copy the passed raw bitmap handle. Generally only for
//  internal use.
//
TBitmap::TBitmap(const  tCIDGraphDev::TBmpHandle    hbmpSrc
                , const tCIDLib::EAdoptOpts         eAdopt)
{
    //
    //  Either duplicate this bitmap handle, or adopt it, according to the
    //  adopt options flag.
    //
    if (eAdopt == tCIDLib::EAdoptOpts::Adopt)
        m_cptrHandle.SetPointer(new TRefBmpHandle(hbmpSrc));
     else
        m_cptrHandle.SetPointer(new TRefBmpHandle(hbmpDupHandle(hbmpSrc)));
}

//
//  This copy constructor just copies the counted handle pointer, so it
//  still refers to the same underlying bitmap as the original. If you want
//  a copy of your own, call DeepCopy().
//
TBitmap::TBitmap(const TBitmap& bmpToCopy) :

    m_cptrHandle(bmpToCopy.m_cptrHandle)
{
}

TBitmap::~TBitmap()
{
    //
    //  The counted pointer will destruct and clean up if needed. But, to
    //  avoid exceptions propogating out of here, we will explicitly drop the
    //  reference within a try/catch, so that if we are the one who cleans it
    //  up, we'll catch any errors.
    //
    try
    {
        m_cptrHandle.DropRef();
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
}


// ---------------------------------------------------------------------------
//  TBitmap: Public operators
// ---------------------------------------------------------------------------
TBitmap& TBitmap::operator=(const TBitmap& bmpToAssign)
{
    if (this != &bmpToAssign)
        m_cptrHandle  = bmpToAssign.m_cptrHandle;
    return *this;
}


// ---------------------------------------------------------------------------
//  TBitmap: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TBitmap::DrawOn(         TGraphDrawDev&  gdevTarget
                                , const TPoint&         pntOrg) const
{
    gdevTarget.DrawBitmap(*this, pntOrg);
}

tCIDLib::TVoid TBitmap::DrawOn(         TGraphDrawDev&  gdevTarget
                                , const TArea&          areaToFill) const
{
    gdevTarget.DrawBitmap(*this, areaToFill);
}



// ---------------------------------------------------------------------------
//  TBitmap: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This method does an advanced drawing algorithm that allows a lot of standard
//  drawing logic to be done in one place. It deals with various blit types,
//  transparency, and alpha stuff.
//
//  A lot of the info needed to call this can be gotten via the CIDGraphDev facility
//  class' eCalcPlacement() method. It will figure out correct placement and
//  blitting options, given a target area and placement info.
//
//  If source alpha is present, that is used and any color based transparency
//  is ignored (though opacity, i.e. constant alpha is still used.) It is
//  assumed that the image is pre-blended.
//
//  If no source alpha is present, then both or either color based and and
//  constant alpha can be present. If both, then the image must be pre-masked
//  as it would be for color transparency.
//
//  If color transparency is used, then the image must be pre-masked with
//  the inverse mask.
//
//  NOTE that we don't need to set up any clip region since the algorithm will
//  naturally handle clipping by only blitting into the target area.
//
//  NOTE that we have to do everything here such that it would work if
//  the graphics output is being remoted, i.e. don't use methods that won't
//  provide a remote client with the info he heeds.
//
//
//  We also have a special case, force alpha, that is used by some programs
//  which need to build up an alpha blittable image of content. Since the
//  standard drawing operations just clear the alpha channel, we have to get
//  it back right, in a way that won't wrongly affect anything that this is
//  being drawn over. So basically we are forcing an alpha type display for
//  an image that doesn't otherwise have any.
//
tCIDLib::TVoid
TBitmap::AdvDrawOn(         TGraphDrawDev&          gdevTarget
                    , const TArea&                  areaSource
                    , const TArea&                  areaTarget
                    , const tCIDLib::TBoolean       bTransparent
                    , const tCIDLib::TCard4         c4TransClr
                    , const TBitmap&                bmpMask
                    , const tCIDGraphDev::EBltTypes eBltType
                    , const tCIDLib::TCard1         c1Opacity
                    , const tCIDLib::TBoolean       bForceAlpha) const
{
    try
    {
        // Get the bitmap handle reference out for use below
        const TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();

        // Force convenience and efficiency, get the source alpha flag out
        const tCIDLib::TBoolean bSrcAlpha = prbhTmp->bSrcAlpha();

        //
        //  We have three major scenarios.
        //
        //  1.  If a tiled blit, then we just fill the target area with the pattern
        //      and that's that.
        //
        //  2.  If any opacity or source alpha, we have to do special case stuff
        //      to deal with that. May or may not have a mask based transparency.
        //
        //  3.  Else a pretty straightforward blit of the correct type, and it
        //      may or may not have a mask based transparency.
        //
        if (eBltType == tCIDGraphDev::EBltTypes::Tile)
        {
            gdevTarget.FillWithBmpPattern
            (
                areaTarget
                , *this
                , tCIDGraphDev::EBmpModes::PatCopy
                , areaTarget.pntOrg()
            );
        }
         else if (bSrcAlpha || ((c1Opacity < 0xFF) && !bTransparent))
        {
            //
            //  Source alpha is present so we only do alpha blitting here,
            //  and ignore any color transparency. We also do this one if
            //  there is opacity but not color transparency.
            //
            if ((eBltType == tCIDGraphDev::EBltTypes::Stretch)
            ||  (eBltType == tCIDGraphDev::EBltTypes::Both))
            {
                // Use both source and destination areas in an alpha blit
                gdevTarget.AlphaBlit
                (
                    *this
                    , areaSource
                    , areaTarget
                    , c1Opacity
                    , bSrcAlpha
                );
            }
             else if (eBltType == tCIDGraphDev::EBltTypes::Clip)
            {
                //
                //  Alpha blit the selected source section to the target
                //  origin.
                //
                gdevTarget.AlphaBlit
                (
                    *this, areaSource, areaTarget.pntOrg(), c1Opacity, bSrcAlpha
                );
            }
             else
            {
                // Just alpha blit the whole thing at the target origin
                gdevTarget.AlphaBlit
                (
                    *this, areaTarget.pntOrg(), c1Opacity, bSrcAlpha
                );
            }
        }
         else if (bTransparent && (c1Opacity < 0xFF))
        {
            //
            //  We have const alpha and color based transparency, so we use
            //  the special masked alpha blit method which handles the
            //  masking and stretching appropriately.
            //
            if ((eBltType == tCIDGraphDev::EBltTypes::Stretch)
            ||  (eBltType == tCIDGraphDev::EBltTypes::Both))
            {
                // Use both source and destination areas
                gdevTarget.MaskedAlphaBlit
                (
                    *this
                    , bmpMask
                    , areaSource
                    , areaTarget
                    , c1Opacity
                    , kCIDLib::False
                    , c4TransClr
                );
            }
             else if (eBltType == tCIDGraphDev::EBltTypes::Clip)
            {
                // Blit the selected source section to the target origin
                gdevTarget.MaskedAlphaBlit
                (
                    *this
                    , bmpMask
                    , areaSource
                    , areaTarget.pntOrg()
                    , c1Opacity
                    , kCIDLib::False
                    , c4TransClr
                );
            }
             else
            {
                // Just blit the whole thing to the target origin
                gdevTarget.MaskedAlphaBlit
                (
                    *this
                    , bmpMask
                    , areaTarget.pntOrg()
                    , c1Opacity
                    , kCIDLib::False
                    , c4TransClr
                );
            }
        }
         else if (bTransparent)
        {
            // Color based transparency only
            if ((eBltType == tCIDGraphDev::EBltTypes::Stretch)
            ||  (eBltType == tCIDGraphDev::EBltTypes::Both))
            {
                gdevTarget.DrawBitmapMasked
                (
                    *this, bmpMask, areaSource, areaTarget, c4TransClr, bForceAlpha
                );
            }
             else if (eBltType == tCIDGraphDev::EBltTypes::Clip)
            {
                gdevTarget.DrawBitmapMasked
                (
                    *this
                    , bmpMask
                    , areaSource
                    , areaTarget.pntOrg()
                    , c4TransClr
                    , bForceAlpha
                );
            }
             else
            {
                // Just blit the whole thing to the target origin
                gdevTarget.DrawBitmapMasked
                (
                    *this, bmpMask, areaTarget.pntOrg(), c4TransClr, bForceAlpha
                );
            }
        }
         else
        {
            //
            //  Nothing special at all, just do a standard draw of the
            //  bit map with 'source copy' blit mode, same blit variations
            //  as above.
            //
            const tCIDGraphDev::EBmpModes eMode = tCIDGraphDev::EBmpModes::SrcCopy;
            if ((eBltType == tCIDGraphDev::EBltTypes::Stretch)
            ||  (eBltType == tCIDGraphDev::EBltTypes::Both)
            ||  (eBltType == tCIDGraphDev::EBltTypes::Clip))
            {
                if (bForceAlpha)
                    gdevTarget.DrawBitmapAlpha(*this, areaSource, areaTarget, eMode);
                else
                    gdevTarget.DrawBitmap(*this, areaSource, areaTarget, eMode);
            }
             else
            {
                if (bForceAlpha)
                {
                    //
                    //  The alpha forcing one we have requires source and target
                    //  areas, so query the size of the image and set up source
                    //  and targets to use.
                    //
                    tCIDLib::TCard4 c4Height;
                    tCIDLib::TCard4 c4Width;
                    QuerySize(c4Width, c4Height);
                    TArea areaTar(areaTarget.pntOrg(), TSize(c4Width, c4Height));
                    TArea areaSrc(0, 0, c4Width, c4Height);
                    gdevTarget.DrawBitmapAlpha(*this, areaSrc, areaTar, eMode);
                }
                 else
                {
                    gdevTarget.DrawBitmap(*this, areaTarget.pntOrg(), eMode);
                }
            }
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}


TBitmap TBitmap::bmpMakeGrayScale() const
{
    // Create a compatible device based on the desktop
    TIntGraphWndDev gdevCompat;

    //
    //  Create a bitmap image object of the required size and format,
    //  then load up our pixel data into it.
    //
    TPixelArray pixaData;
    TClrPalette palData;
    QueryImgData(gdevCompat, pixaData, palData);

    //
    //  Ask the pixel array to gray scale itself. If it is palette based,
    //  it'll update the palette to be a gray scale palette, else it will
    //  update itself. If it's already gray scale data, it won't do
    //  aything.
    //
    pixaData.CvtToGrayScale(palData);

    // And create a new bitmap from this data
    return TBitmap
    (
        hbmpFromImage(gdevCompat, pixaData, palData, 2.2, kCIDLib::False, 0)
        , tCIDLib::EAdoptOpts::Adopt
    );
}


//
//  Create a mask or inverted mask for a color based tranparency image.
//  We return a monochrome bitmap for the mask.
//
TBitmap
TBitmap::bmpMakeMask(const  TGraphDrawDev&      gdevCompat
                    , const TPoint&             pntAt
                    , const tCIDLib::TBoolean   bInvert) const
{
    // Call the other version and just eat the returned color
    tCIDLib::TCard4 c4Dummy;
    return bmpMakeMask(gdevCompat, pntAt, bInvert, c4Dummy);
}

TBitmap
TBitmap::bmpMakeMask(const  TGraphDrawDev&      gdevCompat
                    , const TPoint&             pntAt
                    , const tCIDLib::TBoolean   bInvert
                    ,       tCIDLib::TCard4&    c4TransClr) const
{
    const TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();
    const tCIDGraphDev::TBmpHandle hbmpUs = prbhTmp->hbmpThis();

    // Set it on a draw surface so we can get the pixel
    tCIDGraphDev::TDeviceHandle hdevTmp = ::CreateCompatibleDC(gdevCompat.hdevThis());
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
    HBITMAP hbmpOld = reinterpret_cast<HBITMAP__*>(::SelectObject(hdevTmp, hbmpUs));
    if (!hbmpOld)
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

    // Get the color out at the pixel
    c4TransClr = ::GetPixel(hdevTmp, pntAt.i4X(), pntAt.i4Y());
    if (c4TransClr == CLR_INVALID)
        c4TransClr = 0;

    // Put the old bitmap back and destroy the temp device
    if (!::SelectObject(hdevTmp, hbmpOld))
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

    // And now we can delete our device handle
    if (!::DeleteDC(hdevTmp))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcDev_DestroyMemDev
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Warn
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Now we can call the other version
    return bmpMakeMask(gdevCompat, c4TransClr, bInvert);
}


TBitmap
TBitmap::bmpMakeMask(const  TGraphDrawDev&      gdevCompat
                    , const tCIDLib::TCard4     c4TransClr
                    , const tCIDLib::TBoolean   bInvert) const
{
    // Get a temp pointer to the data in the counted pointer
    const TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();
    tCIDGraphDev::TBmpHandle hbmpMe = prbhTmp->hbmpThis();

    // Create a target bitmap, monochrome
    TSize szBmp = szBitmap();
    HBITMAP hbmpNew = ::CreateBitmap(szBmp.c4Width(), szBmp.c4Height(), 1, 1, 0);

    // Create two compatible DCs, one for us and one for the target
    HDC hdcSrc = ::CreateCompatibleDC(gdevCompat.hdevThis());
    HDC hdcTar = ::CreateCompatibleDC(gdevCompat.hdevThis());

    // Select the two bitmaps into them
    HBITMAP hbmpSrcOld = (HBITMAP)::SelectObject(hdcSrc, hbmpMe);
    HBITMAP hbmpTarOld = (HBITMAP)::SelectObject(hdcTar, hbmpNew);

    // Set the background color on the source
    COLORREF cSave = ::SetBkColor(hdcSrc, c4TransClr);

    //
    //  And blit from source to target, inverting the source if asked to do
    //  so.
    //
    ::BitBlt(hdcTar, 0, 0, szBmp.c4Width(), szBmp.c4Height(), hdcSrc, 0, 0, SRCCOPY);

    // Put the old color back
    ::SetBkColor(hdcSrc, cSave);

    // If they wanted an inverted mask, then invert
    if (bInvert)
        ::PatBlt(hdcTar, 0, 0, szBmp.c4Width(), szBmp.c4Height(), DSTINVERT);

    // Unselect the bitmaps, and release the src and dest stuff
    ::SelectObject(hdcSrc, hbmpSrcOld);
    ::SelectObject(hdcTar, hbmpTarOld);
    ::DeleteDC(hdcSrc);
    ::DeleteDC(hdcTar);

    // And create a bitmap with the new bitmap handle
    return TBitmap(hbmpNew, tCIDLib::EAdoptOpts::Adopt);
}


// Indicate if this image has an alpha channel
tCIDLib::TBoolean TBitmap::bHasAlpha() const
{
    const TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();
    return prbhTmp->bSrcAlpha();
}


// Return the ref count of this image's handle
tCIDLib::TCard4 TBitmap::c4RefCount() const
{
    return m_cptrHandle.c4RefCount();
}


// Get or set the serial numer
tCIDLib::TCard4 TBitmap::c4SerialNum() const
{
    return m_cptrHandle->c4SerialNum();
}

tCIDLib::TCard4 TBitmap::c4SerialNum(const tCIDLib::TCard4 c4ToSet)
{
    return m_cptrHandle->c4SerialNum(c4ToSet);
}


//
//  Does a deep copy of this bitmap, creating a copy of the original one
//  and releasing our hold on it. So we now have a new one.
//
tCIDLib::TVoid TBitmap::DeepCopy()
{
    // Get a temp pointer to the data in the counted pointer
    const TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();

    // Get copies of info we need to put back
    const tCIDLib::TBoolean bAlpha = prbhTmp->bSrcAlpha();
    const tCIDLib::TCard4 c4SerialNum = m_cptrHandle->c4SerialNum();
    const TString strUserData = m_cptrHandle->strUserData();

    // Make a copy of the raw bitmap
    tCIDGraphDev::TBmpHandle hbmpTmp = prbhTmp->hbmpDup();

    // And store a new ref handle for this new bitmap
    m_cptrHandle.SetPointer(new TRefBmpHandle(hbmpTmp, bAlpha));

    //
    //  Put the serial number and user data back. It's the same image
    //  so far, we just have our own copy. The client code may choose
    //  to change them if that suits its purposes.
    //
    m_cptrHandle->c4SerialNum(c4SerialNum);
    m_cptrHandle->strUserData(strUserData);
}


//
//  Assumes that we are a pre-masked image, and that the inverted mask (which
//  will be used to make the hole) is passed to us. The blit type tells us
//  how to interpret the source and target area values.
//
tCIDLib::TVoid
TBitmap::DrawAsMask(        TGraphDrawDev&          gdevTarget
                    , const TBitmap&                bmpMask
                    , const TArea&                  areaSource
                    , const TArea&                  areaTarget
                    , const tCIDGraphDev::EBltTypes eBltType) const
{
    // If we've gone smaller than 4 pixels, don' bother
    if ((areaTarget.c4Width() < 4) || (areaTarget.c4Height() < 4))
        return;

    // Clip if asked to
    TGUIRegion grgnTarget(areaTarget);
    TRegionJanitor janDraw
    (
        &gdevTarget
        , grgnTarget
        , tCIDGraphDev::EClipModes::And
        , eBltType == tCIDGraphDev::EBltTypes::Clip
    );

    if ((eBltType == tCIDGraphDev::EBltTypes::Stretch)
    ||  (eBltType == tCIDGraphDev::EBltTypes::Both))
    {
        gdevTarget.DrawBitmap
        (
            bmpMask
            , areaSource
            , areaTarget
            , tCIDGraphDev::EBmpModes::SrcAnd
        );

        gdevTarget.DrawBitmap
        (
            *this
            , areaSource
            , areaTarget
            , tCIDGraphDev::EBmpModes::SrcPaint
        );
    }
     else if (eBltType == tCIDGraphDev::EBltTypes::Clip)
    {
        gdevTarget.DrawBitmap
        (
            bmpMask
            , areaSource
            , areaTarget.pntOrg()
            , tCIDGraphDev::EBmpModes::SrcAnd
        );

        gdevTarget.DrawBitmap
        (
            *this
            , areaSource
            , areaTarget.pntOrg()
            , tCIDGraphDev::EBmpModes::SrcPaint
        );
    }
     else
    {
        gdevTarget.DrawBitmap
        (
            bmpMask
            , areaTarget.pntOrg()
            , tCIDGraphDev::EBmpModes::SrcAnd
        );

        gdevTarget.DrawBitmap
        (
            *this
            , areaTarget.pntOrg()
            , tCIDGraphDev::EBmpModes::SrcPaint
        );
    }
}


//
//  Some variations on doing a transparent blit. They all eventually call
//  the last version, with the full source/target areas.
//
tCIDLib::TVoid
TBitmap::DrawTransparent(       TGraphDrawDev&  gdevTarget
                        , const TPoint&         pntTarget
                        , const tCIDLib::TCard4 c4TransClr) const
{
    // Get our bitmap handle out so we can get the image size
    const TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();
    TArea areaSrc(TPoint::pntOrigin, prbhTmp->szBitmap());
    TArea areaTarget(pntTarget, areaSrc.szArea());
    DrawTransparent(gdevTarget, areaSrc, areaTarget, c4TransClr);
}

tCIDLib::TVoid
TBitmap::DrawTransparent(       TGraphDrawDev&  gdevTarget
                        , const TArea&          areaTarget
                        , const tCIDLib::TCard4 c4TransClr) const
{
    // Get our bitmap handle out so we can get the image size
    const TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();
    TArea areaSrc(TPoint::pntOrigin, prbhTmp->szBitmap());
    DrawTransparent(gdevTarget, areaSrc, areaTarget, c4TransClr);
}

tCIDLib::TVoid
TBitmap::DrawTransparent(       TGraphDrawDev&  gdevTarget
                        , const TArea&          areaSource
                        , const TArea&          areaTarget
                        , const tCIDLib::TCard4 c4TransClr) const
{
    // Get our bitmap handle out
    const TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();
    tCIDGraphDev::TBmpHandle hbmpMe = prbhTmp->hbmpThis();

    // Set up a memory device over the bitmap
    HDC hdcSrc = ::CreateCompatibleDC(gdevTarget.hdevThis());
    HBITMAP hbmpOld = (HBITMAP)::SelectObject(hdcSrc, hbmpMe);

    // And do the transparent blit
    const tCIDLib::TCard4 c4Res = ::TransparentBlt
    (
        gdevTarget.hdevThis()
        , areaTarget.i4X()
        , areaTarget.i4Y()
        , areaTarget.c4Width()
        , areaTarget.c4Height()
        , hdcSrc
        , areaSource.i4X()
        , areaSource.i4Y()
        , areaSource.c4Width()
        , areaSource.c4Height()
        , c4TransClr
    );

    // Get the error out before we do something to affect it
    tCIDLib::TCard4 c4LastErr = 0;
    if (!c4Res)
        c4LastErr = ::GetLastError();

    // Undo the the source device stuff
    ::SelectObject(hdcSrc, hbmpOld);
    ::DeleteDC(hdcSrc);

    // And if an error, now throw
    if (!c4Res)
    {
        TKrnlError::SetLastHostError(::GetLastError());
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
//  Lets the client increment the serial number (for their own purposes.)
//  Note that the serial number is in the ref counted handle object, so we
//  change it for any other bitmap handles that reference the image. But that
//  makes sense because if this image was changed, all of them now point to
//  different image contents.
//
tCIDLib::TVoid TBitmap::IncSerialNum()
{
    m_cptrHandle->IncSerialNum();
}



tCIDGraphDev::TBmpHandle TBitmap::hbmpThis() const
{
    // Get a temp pointer to the data in the counted pointer
    const TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();
    return prbhTmp->hbmpThis();
}


tCIDLib::TVoid TBitmap::Invert(const TGraphDrawDev& gdevCompat)
{
    // Make us the drawing surface of a memory device
    const TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();
    tCIDGraphDev::TBmpHandle hbmpMe = prbhTmp->hbmpThis();

    HDC hdcTar = ::CreateCompatibleDC(gdevCompat.hdevThis());
    HBITMAP hbmpTarOld = (HBITMAP)::SelectObject(hdcTar, hbmpMe);

    // And just do a pattern blit with destination invert
    const TSize& szBmp = prbhTmp->szBitmap();
    ::PatBlt(hdcTar, 0, 0, szBmp.c4Width(), szBmp.c4Height(), DSTINVERT);

    ::SelectObject(hdcTar, hbmpTarOld);
    ::DeleteDC(hdcTar);
}


//
//  Pre-masks this image with an inverse mask, So that it's ready to use
//  for sprite type masking (for color based transparency of course.)
//
tCIDLib::TVoid
TBitmap::MaskWith(const TGraphDrawDev& gdevCompat, const TBitmap& bmpInvMask)
{
    TGraphMemDev gdevMe(gdevCompat, *this);
    gdevMe.DrawBitmap(bmpInvMask, TPoint::pntOrigin, tCIDGraphDev::EBmpModes::SrcAnd);
}


tCIDLib::TVoid
TBitmap::MaskWith(const TGraphDrawDev& gdevCompat, const tCIDLib::TCard4 c4TransClr)
{
    // In this one we create a temporary inverse mask
    TBitmap bmpInvMask(bmpMakeMask(gdevCompat, c4TransClr, kCIDLib::True));

    TGraphMemDev gdevMe(gdevCompat, *this);
    gdevMe.DrawBitmap(bmpInvMask, TPoint::pntOrigin, tCIDGraphDev::EBmpModes::SrcAnd);
}


//
//  If we created a DIB section, then the pixel array data is available to
//  directly update. We have one version that returns a pointer that can be
//  null if it's not a DIB section. And we have another that will throw if
//  if it isn't and returns a ref.
//
//  NOTE:   This is data in the referenced counted object. Be sure that you
//          don't access this after you let the bitmap go since it could go
//          away.
//
//  NOTE:   If you change the image directly via the pixel data, you should
//          bump the serial number to indicate the image has changed, since
//          we have no way of knowing that here.
//
const TPixelArray& TBitmap::pixaData() const
{
    const TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();
    if (!prbhTmp->ppixaData())
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcBmp_NoDirectAccess
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotSupported
        );
    }
    return *prbhTmp->ppixaData();
}

TPixelArray& TBitmap::pixaData()
{
    TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();
    if (!prbhTmp->ppixaData())
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcBmp_NoDirectAccess
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotSupported
        );
    }
    return *prbhTmp->ppixaData();
}

const TPixelArray* TBitmap::ppixaData() const
{
    const TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();
    return prbhTmp->ppixaData();
}

TPixelArray* TBitmap::ppixaData()
{
    TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();
    return prbhTmp->ppixaData();
}



tCIDLib::TVoid
TBitmap::QueryImgData(  const   TGraphDrawDev&      gdevCompat
                        ,       TPixelArray&        pixaToFill
                        ,       TClrPalette&        palToFill
                        , const tCIDLib::TBoolean   bForceAlphaIf32) const
{
    try
    {
        // Get a temp pointer to the data in the counted pointer
        const TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();
        tCIDGraphDev::TBmpHandle hbmpMe = prbhTmp->hbmpThis();

        //
        //  First query the bitmap info, which won't get back any pixel
        //  data. This gives us the info we need to set up to query that
        //  information. We also provide enough space for up to the maximum
        //  256 colors for the palette in case it has a palette.
        //
        const tCIDLib::TCard4 c4HdrSz = sizeof(BITMAPINFOHEADER) + (4 * 256);
        tCIDLib::TCard1* pc1HdrBuf = new tCIDLib::TCard1[c4HdrSz];
        TRawMem::SetMemBuf(pc1HdrBuf, tCIDLib::TCard1(0), c4HdrSz);
        TArrayJanitor<tCIDLib::TCard1> janHdr(pc1HdrBuf);

        BITMAPINFO* pbiSrc = (BITMAPINFO*)pc1HdrBuf;
        pbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        tCIDLib::TInt4 i4Res = ::GetDIBits
        (
            gdevCompat.hdevThis(), hbmpMe, 0, 0, 0, pbiSrc, DIB_RGB_COLORS
        );

        if (!i4Res)
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcBmp_GetBits
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }

        // We want to get the bits in the pixel array's requested row order
        if (((pbiSrc->bmiHeader.biHeight < 0)
        &&   (pixaToFill.eRowOrder() == tCIDImage::ERowOrders::BottomUp))
        ||  ((pbiSrc->bmiHeader.biHeight > 0)
        &&   (pixaToFill.eRowOrder() == tCIDImage::ERowOrders::TopDown)))
        {
            pbiSrc->bmiHeader.biHeight *= -1;
        }

        // Set up a size for the bitmap
        TSize szThis
        (
            pbiSrc->bmiHeader.biWidth
            , pbiSrc->bmiHeader.biHeight > 0
              ? pbiSrc->bmiHeader.biHeight : (pbiSrc->bmiHeader.biHeight * -1)
        );

        // Set up a pixel format/depth set based on our bit depth
        tCIDImage::EPixFmts eFmt;
        tCIDImage::EBitDepths eDepth;
        switch(pbiSrc->bmiHeader.biBitCount)
        {
            case 1 :
            case 2 :
            case 4 :
            case 8 :
                // These are always palette based
                eFmt = tCIDImage::EPixFmts::ClrPal;
                eDepth = tCIDImage::EBitDepths(pbiSrc->bmiHeader.biBitCount);
                break;

            case 16 :
                eFmt = tCIDImage::EPixFmts::TrueClr;
                eDepth = tCIDImage::EBitDepths::Five;
                break;

            case 24 :
                eFmt = tCIDImage::EPixFmts::TrueClr;
                eDepth = tCIDImage::EBitDepths::Eight;
                break;

            case 32 :
                //
                //  It'll always be 32 bit on hardware that supports it, but that
                //  doesn't mean it really has alpha. Check the flag in the handle
                //  for that. If it's really just true color, then change the
                //  header bit count to indicate we want only 24 bit info and
                //  set our targe tpixel array format to true color without alpha.
                //
                //  They can also force us to assume alpha is present.
                //
                eDepth = tCIDImage::EBitDepths::Eight;
                if (prbhTmp->bSrcAlpha() || bForceAlphaIf32)
                {
                    eFmt = tCIDImage::EPixFmts::TrueAlpha;
                }
                 else
                {
                    eFmt = tCIDImage::EPixFmts::TrueClr;
                    pbiSrc->bmiHeader.biBitCount = 24;
                }
                break;

            default :
                // Make compiler happy
                eFmt = tCIDImage::EPixFmts::ClrPal;
                eDepth = tCIDImage::EBitDepths::Two;

                facCIDGraphDev().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kGrDevErrs::errcBmp_BadDepth
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::BadParms
                    , TCardinal(pbiSrc->bmiHeader.biBitCount)
                );
                break;
        };

        //
        //  See if the passed pixel array is of the correct size or bit depth. If
        //  not, then we have to reallocate.
        //
        if ((pixaToFill.szImage() != szThis)
        ||  (pixaToFill.ePixFmt() != eFmt)
        ||  (pixaToFill.eBitDepth() != eDepth))
        {
            pixaToFill.Reset(eFmt, eDepth, pixaToFill.eRowOrder(), szThis);
        }

        pbiSrc->bmiHeader.biCompression = BI_RGB;
        pbiSrc->bmiHeader.biSizeImage = pixaToFill.c4ImageSz();
        i4Res = ::GetDIBits
        (
            gdevCompat.hdevThis()
            , hbmpMe
            , 0
            , szThis.c4Height()
            , pixaToFill.pc1Buffer()
            , pbiSrc
            , DIB_RGB_COLORS
        );

        if (!i4Res)
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcBmp_GetBits
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
            );
        }

        // If there's a palette, then set up the palette with them
        if (eFmt == tCIDImage::EPixFmts::ClrPal)
        {
            palToFill.ResetSize(0x1UL << pbiSrc->bmiHeader.biBitCount);

            tCIDLib::TCard4 c4Count = pbiSrc->bmiHeader.biClrUsed;
            if (!c4Count)
                c4Count = palToFill.c4ClrCount();

            TRGBClr rgbCur;
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                RGBQUAD& qCur = pbiSrc->bmiColors[c4Index];
                rgbCur.Set(qCur.rgbRed, qCur.rgbGreen, qCur.rgbBlue);
            }
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
}


// Give them back a copy of the bitmap header info we have
tCIDLib::TVoid
TBitmap::QueryHdrInfo(TBitmapImage::TBmpHdrInfo& bhiToFill) const
{
    const TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();
    bhiToFill = prbhTmp->bhiThis();
}


// Give them the image size info
tCIDLib::TVoid
TBitmap::QuerySize(tCIDLib::TCard4& c4Width, tCIDLib::TCard4& c4Height) const
{
    const TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();
    const TBitmapImage::TBmpHdrInfo& bhiCur = prbhTmp->bhiThis();

    c4Height = bhiCur.c4Height;
    c4Width = bhiCur.c4Width;
}


//
//  When we draw into a memory device (into a bitmap), only alpha blits really
//  really retain alpha channel info. Any regular graphics operations don't
//  do that, they just set it fully opaque.
//
//  So generally if we are going to draw into a bitmap and then use that to
//  blit back into another bitmap (for animated layering purposes) we have to
//  go back and get the alpha channel out of the original image and reapply
//  it back to the now drawn in data.
//
//  So this method assumes that this bitmap contains the passed bitmap within
//  it (in the area specified), and will do this operation. I.e. this bitmap
//  maybe larger than the original and we just affect the area where the
//  original image was placed (and then probably drawn on top of with other
//  graphics operations.)
//
//  We can optionally set the alpha of this whole bitmap to fully transparent
//  first, before reapplying the image's alpha channel. This is often desired
//  since the image is supposed to be against a transparent background.
//
//  This bitmap must be a DIBSection, it cannot be a regular bitmap. It would
//  be far more efficient if the source is, but it doesn't have to be.
//
tCIDLib::TVoid
TBitmap::ReapplyAlphaMask(  const   TBitmap&            bmpOriginal
                            ,       TGraphDrawDev&      gdevCompat
                            , const TPoint&             pntSrc
                            , const TPoint&             pntTar
                            , const tCIDLib::TBoolean   bResetAllAlpha)
{
    // Get a ref to our handle and the original
    TRefBmpHandle* prbhThis = m_cptrHandle.pobjData();
    const TRefBmpHandle* prbhOrg = bmpOriginal.m_cptrHandle.pobjData();

    // This bitmap must be an in memory one
    if (!prbhThis->ppixaData())
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcBmp_NoDirectAccess
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotSupported
        );
    }

    //
    //  Create an 8 bit gray scale pixel array of the same size as the
    //  original. which we can put the alpha channel info into.
    //
    TPixelArray pixaAlpha
    (
        tCIDImage::EPixFmts::GrayScale
        , tCIDImage::EBitDepths::Eight
        , tCIDImage::ERowOrders::TopDown
        , prbhOrg->szBitmap()
    );


    //  If we can get direct access to the original pixel array, we do
    //  that instead of copying it. Otherwise we have to copy it out.
    //
    if (prbhOrg->ppixaData())
    {
        // We can get it straight out
        prbhOrg->ppixaData()->ExtractAlphaChannel(pixaAlpha);
    }
     else
    {
        // We have to make a temp copy
        TClrPalette palDummy;
        TPixelArray pixaTmp;
        bmpOriginal.QueryImgData(gdevCompat, pixaTmp, palDummy, kCIDLib::True);
        pixaTmp.ExtractAlphaChannel(pixaAlpha);
    }

    //
    //  Ok, now we want to go back and apply this alpha channel to ourself.
    //  In this case we know we are a DIBsection since it was checked above.
    //
    TArea areaSrc(pntSrc, pixaAlpha.szImage());
    if (bResetAllAlpha)
    {
        // This one will zero the alpha of any pixels outside the target area
        prbhThis->ppixaData()->CreateAlphaChannelFrom
        (
            pixaAlpha, areaSrc, pntTar, kCIDLib::False
        );
    }
     else
    {
        // Just apply alpha within the target area
        prbhThis->ppixaData()->ApplyAlphaChannel
        (
            pixaAlpha, areaSrc, pntTar, kCIDLib::False, kCIDLib::False
        );
    }
}


//
//  Reset us back to a default 1x1 image. This will drop our reference to any
//  previous image image we were holding.
//
tCIDLib::TVoid TBitmap::Reset()
{
    //
    //  Just reset us to a single bit mono bitmap, which will let go any
    //  bitmap we were referencing. Note that this will leave us with
    //  empty user data and zero serial number, which is appropriate since
    //  we aren't whatever image we were before.
    //
    m_cptrHandle.SetPointer
    (
        new TRefBmpHandle(::CreateBitmap(0, 0, 0, 1, 0))
    );
}


//
//  Give them back the size of the image. We have to do a copy return here
//  since we don't want them holding a reference to data in the referenced
//  counted handle object.
//
TSize TBitmap::szBitmap() const
{
    // Get a temp pointer to the data in the counted pointer
    const TRefBmpHandle* prbhTmp = m_cptrHandle.pobjData();
    return prbhTmp->szBitmap();
}


// Get or set the user data value
const TString& TBitmap::strUserData() const
{
    return m_cptrHandle->strUserData();
}

const TString& TBitmap::strUserData(const TString& strToSet)
{
    m_cptrHandle->strUserData(strToSet);
    return m_cptrHandle->strUserData();
}


tCIDLib::TVoid TBitmap::Set(const tCIDGraphDev::ESysBmps eBitmap)
{
    LoadSysBmp(eBitmap);
}


// ---------------------------------------------------------------------------
//  TBitmap: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TBitmap::LoadSysBmp(const tCIDGraphDev::ESysBmps eToLoad)
{
    // Translate the enum to a system resource id
    const tCIDLib::TResId ridBmp = ridXlatSysBmp(eToLoad);

    //
    //  Try to load up a bitmap with this id. If its a system bitmap, then
    //  call LoadBitmap(). Otherwise, use our own bitmap resource creation
    //  code.
    //
    tCIDGraphDev::TBmpHandle hbmpTmp;
    if (eToLoad > tCIDGraphDev::ESysBmps::LastSys)
    {
        // Create a compatible device based on the desktop
        TIntGraphWndDev gdevCompat;

        // Load and create the bitmap now
        hbmpTmp = hbmpCreate(facCIDGraphDev(), ridBmp, gdevCompat);
        if (hbmpTmp == kCIDGraphDev::hbmpInvalid)
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcBmp_LoadFromModule
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TCardinal(ridBmp)
                , facCIDGraphDev().strName()
            );
        }
    }
     else
    {
        hbmpTmp = ::LoadBitmapW(0, MAKEINTRESOURCE(ridBmp));
        if (hbmpTmp == kCIDGraphDev::hbmpInvalid)
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcBmp_LoadSysBitmap
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , TCardinal(tCIDLib::c4EnumOrd(eToLoad))
            );
        }
    }

    //
    //  Looks ok so store it on the counted pointer. If there is an old one,
    //  it will be dropped if this is the last reference.
    //
    m_cptrHandle.SetPointer(new TRefBmpHandle(hbmpTmp));
}


// ---------------------------------------------------------------------------
//   CLASS: TSysBitmapInfo
//  PREFIX: sbmpi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSysBitmapInfo: Constuctors and Destructor
// ---------------------------------------------------------------------------
TSysBitmapInfo::TSysBitmapInfo() :

    m_bmpImage()
{
}

TSysBitmapInfo::TSysBitmapInfo( const   TString& strName
                                , const TBitmap& bmpImage) :
    m_bmpImage(bmpImage)
    , m_strName(strName)
{
}

TSysBitmapInfo::TSysBitmapInfo(const TSysBitmapInfo& sbmpiToCopy) :

    m_bmpImage(sbmpiToCopy.m_bmpImage)
    , m_strName(sbmpiToCopy.m_strName)
{
}

TSysBitmapInfo::~TSysBitmapInfo()
{
}


// ---------------------------------------------------------------------------
//  TSysBitmapInfo: Public operators
// ---------------------------------------------------------------------------
TSysBitmapInfo& TSysBitmapInfo::operator=(const TSysBitmapInfo& sbmpiToAssign)
{
    if (this != &sbmpiToAssign)
    {
        m_bmpImage  = sbmpiToAssign.m_bmpImage;
        m_strName   = sbmpiToAssign.m_strName;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TSysBitmapInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TBitmap& TSysBitmapInfo::bmpImage() const
{
    return m_bmpImage;
}


const TString& TSysBitmapInfo::strName() const
{
    return m_strName;
}

