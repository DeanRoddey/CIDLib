//
// FILE NAME: CIDCtrls_ImgCacheItem.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/21/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the image cache item class.
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
#include "CIDGraphDev_.hpp"


// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TImgCacheItem,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TImgCacheItem
//  PREFIX: ici
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Public, static methods
// ---------------------------------------------------------------------------
const TString& TImgCacheItem::strImgName(const TImgCacheItem& iciSrc)
{
    return iciSrc.m_strImageName;
}


// ---------------------------------------------------------------------------
//  TImgCacheItem: Constructors and Destructor
// ---------------------------------------------------------------------------
TImgCacheItem::TImgCacheItem(const  TString&            strImgName
                            , const tCIDLib::TBoolean   bThumb) :

    m_bThumb(bThumb)
    , m_bTrans(kCIDLib::False)
    , m_c4Size(0)
    , m_c4TransClr(0)
    , m_enctLastAccess(0)
    , m_enctLastCheck(0)
    , m_enctGoodTill(0)
    , m_strImageName(strImgName)
{
}

//
//  For when they already have the bitmap. We deep copy it if they ask us, else we assume
//  they are going to not reuse the object. For now this assumes non-color based transparency
//  images.
//
TImgCacheItem::TImgCacheItem(const  TString&                strImgName
                            , const tCIDLib::TBoolean       bThumb
                            , const tCIDLib::TCard4         c4Size
                            , const TBitmap&                bmpData
                            , const tCIDLib::TBoolean       bDeepCopy) :

    m_bThumb(bThumb)
    , m_bTrans(kCIDLib::False)
    , m_bmpImage(bmpData)
    , m_c4Size(c4Size)
    , m_c4TransClr(0)
    , m_enctLastAccess(0)
    , m_enctLastCheck(0)
    , m_enctGoodTill(0)
    , m_strImageName(strImgName)
    , m_szImage(bmpData.szBitmap())
{
    if (bDeepCopy)
        m_bmpImage.DeepCopy();
}

TImgCacheItem::TImgCacheItem(const TImgCacheItem& iciToCopy) :

    m_bThumb(iciToCopy.m_bThumb)
    , m_bTrans(iciToCopy.m_bTrans)
    , m_bmpImage(iciToCopy.m_bmpImage)
    , m_bmpMask(iciToCopy.m_bmpMask)
    , m_c4Size(iciToCopy.m_c4Size)
    , m_c4TransClr(iciToCopy.m_c4TransClr)
    , m_enctLastAccess(iciToCopy.m_enctLastAccess)
    , m_enctLastCheck(iciToCopy.m_enctLastCheck)
    , m_enctGoodTill(iciToCopy.m_enctGoodTill)
    , m_strExpiresStamp(iciToCopy.m_strExpiresStamp)
    , m_strImageName(iciToCopy.m_strImageName)
    , m_strTagData(iciToCopy.m_strTagData)
    , m_szImage(iciToCopy.m_szImage)
{
}

TImgCacheItem::~TImgCacheItem()
{
}


// ---------------------------------------------------------------------------
//  TImgCacheItem: Public operators
// ---------------------------------------------------------------------------
TImgCacheItem& TImgCacheItem::operator=(const TImgCacheItem& iciToAssign)
{
    if (this != &iciToAssign)
    {
        m_bThumb            = iciToAssign.m_bThumb;
        m_bTrans            = iciToAssign.m_bTrans;
        m_bmpImage          = iciToAssign.m_bmpImage;
        m_bmpMask           = iciToAssign.m_bmpMask;
        m_c4Size            = iciToAssign.m_c4Size;
        m_c4TransClr        = iciToAssign.m_c4TransClr;
        m_enctLastAccess    = iciToAssign.m_enctLastAccess;
        m_enctLastCheck     = iciToAssign.m_enctLastCheck;
        m_enctGoodTill      = iciToAssign.m_enctGoodTill;
        m_strExpiresStamp   = iciToAssign.m_strExpiresStamp;
        m_strImageName      = iciToAssign.m_strImageName;
        m_strTagData        = iciToAssign.m_strTagData;
        m_szImage           = iciToAssign.m_szImage;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TImgCacheItem: Public, virtual methods
// ---------------------------------------------------------------------------

//
//  Resets the cache item back to default state. If the derived class
//  overrides, it should call us first.
//
tCIDLib::TVoid TImgCacheItem::Reset(const   TString&            strImgName
                                    , const tCIDLib::TBoolean   bThumb)
{
    m_bThumb = bThumb;
    m_bTrans = kCIDLib::False;
    m_c4Size = 0;
    m_c4TransClr = 0;
    m_enctLastAccess = 0;
    m_enctLastCheck = 0;
    m_enctGoodTill = 0;
    m_strExpiresStamp.Clear();
    m_strImageName = strImgName;
    m_strTagData.Clear();
    m_szImage.Zero();

    m_bmpImage.Reset();
    m_bmpMask.Reset();
}


// Set up the image cache item with all the info it needs
tCIDLib::TVoid
TImgCacheItem::Set( const   tCIDLib::TBoolean   bThumb
                    , const tCIDLib::TCard4     c4Size
                    , const TMemBuf&            mbufData
                    , const tCIDLib::TCard4     c4SerialNum
                    , const TGraphDrawDev&      gdevCompat)
{
    //
    //  Let's create the bitmap first. We need to probe it and figure out
    //  what type of iamge it is.
    //
    TCIDImage* pimgNew = 0;
    if (!facCIDImgFact().bDecodeImg(mbufData, c4Size, pimgNew, kCIDLib::True))
    {
        // It's not valid data, so just go back an empty image and return
        DropImage();
        return;
    }
    TJanitor<TCIDImage> janImg(pimgNew);

    // See if it has source alpha
    const tCIDLib::TBoolean bSrcAlpha
    (
        tCIDLib::bAllBitsOn(pimgNew->ePixFmt(), tCIDImage::EPixFmts::Alpha)
    );

    //
    //  Just in case it's a palette based image, we call this to get the
    //  actual color. If not, it'll just return the color converted to
    //  RGB.
    //
    TRGBClr rgbTransClr;
    TCIDImage::CvtTransClr
    (
        pimgNew->ePixFmt()
        , pimgNew->eBitDepth()
        , pimgNew->c4TransClr()
        , pimgNew->palClrs()
        , rgbTransClr
    );

    // Store away the basic info
    m_bThumb = bThumb;
    m_bTrans = bSrcAlpha ? kCIDLib::False : pimgNew->bTransClr();
    m_c4Size = c4Size;
    m_c4TransClr = rgbTransClr.c4Color();
    m_szImage = pimgNew->szImage();

    // Update the last accessed and last checked time stamps
    m_enctLastAccess = TTime::enctNow();
    m_enctLastCheck = m_enctLastAccess;

    // OK, now create the bitmap
    m_bmpImage = TBitmap(gdevCompat, *pimgNew);

    //
    //  If color transparency based, create the mask and pre-mask the image,
    //  unless it's a thumbnail.
    //
    if (m_bTrans && !bThumb)
    {
        m_bmpMask = m_bmpImage.bmpMakeMask
        (
            gdevCompat, m_c4TransClr, kCIDLib::False
        );

        // Create a temp inverse mask and pre-mask the image
        TBitmap bmpInvMask
        (
            m_bmpImage.bmpMakeMask(gdevCompat, m_c4TransClr, kCIDLib::True)
        );
        m_bmpImage.MaskWith(gdevCompat, bmpInvMask);
    }

    // Set the serial number last, after we have the final image
    m_bmpImage.c4SerialNum(c4SerialNum);

    //
    //  And clear the good till and expires stamps. The image has changed,
    //  so these should be set again.
    //
    m_enctGoodTill = 0;
    m_strExpiresStamp.Clear();
}


// ---------------------------------------------------------------------------
//  TImgCacheItem: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TImgCacheItem::bHasAlpha() const
{
    return m_bmpImage.bHasAlpha();
}


tCIDLib::TBoolean TImgCacheItem::bThumb() const
{
    return m_bThumb;
}


tCIDLib::TBoolean TImgCacheItem::bTransparent() const
{
    return m_bTrans;
}


//
//  Check the good till stamp and see if it's still valid. If good till is
//  non-zero, that is used. If it's zero, we'll compare the passed stamp to
//  our stored expires stamp.
//
tCIDLib::TBoolean
TImgCacheItem::bStillGood(const TString& strExpiresStamp) const
{
    if (m_enctGoodTill)
        return (TTime::enctNow() <= m_enctGoodTill);
    return (strExpiresStamp == m_strExpiresStamp);
}

const TBitmap& TImgCacheItem::bmpImage() const
{
    return m_bmpImage;
}


const TBitmap& TImgCacheItem::bmpMask() const
{
    return m_bmpMask;
}


// Get the serial number. It's only changed when setting a new image
tCIDLib::TCard4 TImgCacheItem::c4SerialNum() const
{
    return m_bmpImage.c4SerialNum();
}


// Return the raw image size that was downloaded to create this image
tCIDLib::TCard4 TImgCacheItem::c4Size() const
{
    return m_c4Size;
}


// Return the transparency color, zero if this is not a color transparent image
tCIDLib::TCard4 TImgCacheItem::c4TransClr() const
{
    return m_c4TransClr;
}


// Drop our image references
tCIDLib::TVoid TImgCacheItem::DropImage()
{
    m_bmpImage.Reset();
    m_bmpMask.Reset();
}


// Provide access to the time stamp values
tCIDLib::TEncodedTime TImgCacheItem::enctLastAccess() const
{
    return m_enctLastAccess;
}

tCIDLib::TEncodedTime TImgCacheItem::enctLastCheck() const
{
    return m_enctLastCheck;
}

tCIDLib::TEncodedTime TImgCacheItem::enctGoodTill() const
{
    return m_enctGoodTill;
}


// Get/set the expires stamp
const TString& TImgCacheItem::strExpiresStamp() const
{
    return m_strExpiresStamp;
}

const TString& TImgCacheItem::strExpiresStamp(const TString& strToSet)
{
    m_strExpiresStamp = strToSet;
    return m_strExpiresStamp;
}



// Provide access to the image name
const TString& TImgCacheItem::strImageName() const
{
    return m_strImageName;
}


// Get/set the generic tag data string
const TString& TImgCacheItem::strTagData() const
{
    return m_strTagData;
}

const TString& TImgCacheItem::strTagData(const TString& strToSet)
{
    m_strTagData = strToSet;
    return m_strTagData;
}


// Update the time stamps
tCIDLib::TVoid TImgCacheItem::SetLastAccess()
{
    m_enctLastAccess = TTime::enctNow();
}

tCIDLib::TVoid TImgCacheItem::SetLastCheck()
{
    m_enctLastCheck = TTime::enctNow();
}


//
//  Set the good for time stamp to now plus the indicated seconds. If zero,
//  we assume it's not used, and zero out the good till. That will allow
//  any expires stamp to be used if present.
//
tCIDLib::TVoid TImgCacheItem::SetGoodTill(const tCIDLib::TCard4 c4Seconds)
{
    if (c4Seconds)
        m_enctGoodTill = TTime::enctNowPlusSecs(c4Seconds);
    else
        m_enctGoodTill = 0;
}


const TSize& TImgCacheItem::szImage() const
{
    return m_szImage;
}



// ---------------------------------------------------------------------------
//  TImgCacheItem: Protected, non-virtual methods
// ---------------------------------------------------------------------------

// Proivde non-const access to the bitmap for our derived class
TBitmap& TImgCacheItem::bmpWriteable()
{
    return m_bmpImage;
}

