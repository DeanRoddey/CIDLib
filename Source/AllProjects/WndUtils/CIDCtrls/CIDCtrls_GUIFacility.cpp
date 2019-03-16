//
// FILE NAME: CIDCtrls_GUIFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/2000
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
//  This file implements the TGUIFacility class.
//
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
RTTIDecls(TGUIFacility,TFacility)



// ---------------------------------------------------------------------------
//  Local types and data
// ---------------------------------------------------------------------------
namespace CIDCtrls_GUIFacility
{
    //
    //  Our cache for bitmaps. It is thread safe, so we use it for locking
    //  during access.
    //
    class TBmpCacheItem
    {
        public :
            static const TString& strKey(const TBmpCacheItem& itemSrc)
            {
                return itemSrc.m_strName;
            }

            TBmpCacheItem(const TString& strName, TBitmap& bmpItem) :
                m_bmpItem(bmpItem)
                , m_strName(strName)
            {
            }

            TBitmap m_bmpItem;
            TString m_strName;
    };

    TKeyedHashSet<TBmpCacheItem, TString, TStringKeyOps> colBmpCache
    (
        29
        , new TStringKeyOps
        , &TBmpCacheItem::strKey
        , tCIDLib::EMTStates::Safe
    );



    // Save as above but for icons in this case
    class TIconCacheItem
    {
        public :
            static const TString& strKey(const TIconCacheItem& itemSrc)
            {
                return itemSrc.m_strName;
            }

            TIconCacheItem(const TString& strName, TIcon& icoItem) :
                m_icoItem(icoItem)
                , m_strName(strName)
            {
            }

            TIcon   m_icoItem;
            TString m_strName;
    };

    TKeyedHashSet<TIconCacheItem, TString, TStringKeyOps> colIconCache
    (
        29
        , new TStringKeyOps
        , &TIconCacheItem::strKey
        , tCIDLib::EMTStates::Safe
    );


    // And yet another for app images
    class TAppImgCacheItem
    {
        public :
            static const TString& strKey(const TAppImgCacheItem& itemSrc)
            {
                return itemSrc.m_strName;
            }

            TAppImgCacheItem(const TString& strName) :
                m_strName(strName)
            {
            }

            TPNGImage   m_imgItem;
            TString     m_strName;
    };

    static TKeyedHashSet<TAppImgCacheItem, TString, TStringKeyOps> colAppImgCache
    (
        29
        , new TStringKeyOps
        , &TAppImgCacheItem::strKey
        , tCIDLib::EMTStates::Safe
    );
}



// ---------------------------------------------------------------------------
//   CLASS: TGUIFacility
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGUIFacility: Public, static methods
// ---------------------------------------------------------------------------
const TArea& TGUIFacility::areaPrimaryMonitor()
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    return s_areaPrimaryMon;
}


const TArea& TGUIFacility::areaVirtualScreen()
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    return s_areaVirtualScr;
}


tCIDLib::TVoid TGUIFacility::AdjustDlgUnits(TArea& areaToAdjust)
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    areaToAdjust.Set
    (
        (areaToAdjust.i4X() * tCIDLib::TInt4(s_c4HorzDlgUnits)) / 4
        , (areaToAdjust.i4Y() * tCIDLib::TInt4(s_c4VertDlgUnits)) / 8
        , (areaToAdjust.c4Width() * s_c4HorzDlgUnits) / 4
        , (areaToAdjust.c4Height() * s_c4VertDlgUnits) / 8
    );
}

tCIDLib::TVoid TGUIFacility::AdjustDlgUnits(TSize& szToAdjust)
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    szToAdjust.Set
    (
        (szToAdjust.c4Width() * s_c4HorzDlgUnits) / 4
        , (szToAdjust.c4Height() * s_c4VertDlgUnits) / 8
    );
}

tCIDLib::TVoid TGUIFacility::AdjustDlgUnits(TPoint& pntToAdjust)
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    pntToAdjust.Set
    (
        (pntToAdjust.i4X() * tCIDLib::TInt4(s_c4HorzDlgUnits)) / 4
        , (pntToAdjust.i4Y() * tCIDLib::TInt4(s_c4VertDlgUnits)) / 8
    );
}


tCIDLib::TCard4 TGUIFacility::c4AdjustHDlgUnit(const tCIDLib::TCard4 c4ToAdjust)
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    return tCIDLib::TCard4((c4ToAdjust * s_c4HorzDlgUnits) / 4);
}

tCIDLib::TCard4 TGUIFacility::c4AdjustVDlgUnit(const tCIDLib::TCard4 c4ToAdjust)
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    return tCIDLib::TCard4((c4ToAdjust * s_c4VertDlgUnits) / 8);
}


tCIDLib::TVoid TGUIFacility::AdjustWndUnits(TArea& areaToAdjust)
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    areaToAdjust.Set
    (
        (areaToAdjust.i4X() * 4) / tCIDLib::TInt4(s_c4HorzDlgUnits)
        , (areaToAdjust.i4Y() * 8) / tCIDLib::TInt4(s_c4VertDlgUnits)
        , (areaToAdjust.c4Width() * 4) / s_c4HorzDlgUnits
        , (areaToAdjust.c4Height() * 8) / s_c4VertDlgUnits
    );
}

tCIDLib::TVoid TGUIFacility::AdjustWndUnits(TSize& szToAdjust)
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    szToAdjust.Set
    (
        (szToAdjust.c4Width() * 4) / s_c4HorzDlgUnits
        , (szToAdjust.c4Height() * 8) / s_c4VertDlgUnits
    );
}

tCIDLib::TVoid TGUIFacility::AdjustWndUnits(TPoint& pntToAdjust)
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    pntToAdjust.Set
    (
        (pntToAdjust.i4X() * 4) / tCIDLib::TInt4(s_c4HorzDlgUnits)
        , (pntToAdjust.i4Y() * 8) / tCIDLib::TInt4(s_c4VertDlgUnits)
    );
}

//
//  The default font height. It should ge used in EM height mode.
//
tCIDLib::TCard4 TGUIFacility::c4DefFontHeight()
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    return s_c4DefFontHeight;
}


//
//  Return our horizontal and vertical dialog units, which are based on our
//  default font.
//
tCIDLib::TCard4 TGUIFacility::c4HorzDlgUnit()
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    return s_c4VertDlgUnits;
}

tCIDLib::TCard4 TGUIFacility::c4VertDlgUnit()
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    return s_c4HorzDlgUnits;
}


//
//  This method returns our default font that we want all windows to use. There
//  is a normal and bold version. Each control will use the appropriate one for its
//  type and the theme set on its parent.
//
const TGUIFont& TGUIFacility::gfontDefBold()
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();
    return *s_pgfontDefBold;
}

const TGUIFont& TGUIFacility::gfontDefFixed()
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();
    return *s_pgfontDefFixed;
}

const TGUIFont& TGUIFacility::gfontDefLight()
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();
    return *s_pgfontDefLight;
}

const TGUIFont& TGUIFacility::gfontDefNorm()
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();
    return *s_pgfontDefNorm;
}

const TGUIFont& TGUIFacility::gfontDefSmall()
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();
    return *s_pgfontDefSmall;
}

const TGUIFont& TGUIFacility::gfontDefSmallLight()
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();
    return *s_pgfontDefSmallLight;
}

const TGUIFont& TGUIFacility::gfontDefTitle()
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();
    return *s_pgfontDefTitle;
}


tCIDLib::TInt4 TGUIFacility::i4AdjustHDlgUnit(const tCIDLib::TInt4 i4ToAdjust)
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    return (i4ToAdjust * tCIDLib::TInt4(s_c4HorzDlgUnits)) / 4;
}

tCIDLib::TInt4 TGUIFacility::i4AdjustHWndUnit(const tCIDLib::TInt4 i4ToAdjust)
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    return (i4ToAdjust * 4) / tCIDLib::TInt4(s_c4HorzDlgUnits);
}

tCIDLib::TInt4 TGUIFacility::i4AdjustVDlgUnit(const tCIDLib::TInt4 i4ToAdjust)
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    return (i4ToAdjust * tCIDLib::TInt4(s_c4VertDlgUnits)) / 8;
}

tCIDLib::TInt4 TGUIFacility::i4AdjustVWndUnit(const tCIDLib::TInt4 i4ToAdjust)
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    return (i4ToAdjust * 8) / tCIDLib::TInt4(s_c4VertDlgUnits);
}


const TString& TGUIFacility::strDefFontFace()
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    return s_strDefFontFace;
}


TSize TGUIFacility::szDefIcon()
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    return TSize(::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
}


const TSize& TGUIFacility::szPrimaryMonitor()
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    return s_szPrimaryMon;
}


const TSize& TGUIFacility::szVirtualScreen()
{
    if (!s_bMetricsLoaded)
        FaultInMetrics();

    return s_szVirtualScr;
}


// ---------------------------------------------------------------------------
//  TGUIFacility: Constructors and operators
// ---------------------------------------------------------------------------
TGUIFacility::TGUIFacility( const   TString&            strFacName
                            , const tCIDLib::EModTypes  eModType
                            , const tCIDLib::TCard4     c4MajVer
                            , const tCIDLib::TCard4     c4MinVer
                            , const tCIDLib::TCard4     c4Revision
                            , const tCIDLib::EModFlags  eFlags
                            , const tCIDLib::TBoolean   bLoad) :
    TFacility
    (
        strFacName
        , eModType
        , c4MajVer
        , c4MinVer
        , c4Revision
        , eFlags
        , bLoad
    )
    , m_strImageDir()
{
}

TGUIFacility::TGUIFacility( const   TString&            strFacName
                            , const tCIDLib::EModTypes  eModType
                            , const tCIDLib::TCard4     c4MajVer
                            , const tCIDLib::TCard4     c4MinVer
                            , const tCIDLib::TCard4     c4Revision
                            , const tCIDLib::EModFlags  eFlags
                            , const TString&            strImageDir
                            , const tCIDLib::TBoolean   bLoad) :
    TFacility
    (
        strFacName
        , eModType
        , c4MajVer
        , c4MinVer
        , c4Revision
        , eFlags
        , bLoad
    )
    , m_strImageDir(strImageDir)
{
}

TGUIFacility::~TGUIFacility()
{
}



// ---------------------------------------------------------------------------
//  TGUIFacility: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Test whether an app image name is valid. This is the standard type app image
//  associated with CIDCtrls, so they only provide just a base name and we know
//  where to find it.
//
tCIDLib::TBoolean
TGUIFacility::bAppImageExists(  const   TString&                strName
                                , const tCIDCtrls::EAppImgSz    eSize) const
{
    // Build up the full name to the image, and the cache name
    TPathStr pathActName;
    TString  strCacheName;
    MakeAppImgName(strName, eSize, pathActName, strCacheName);

    // See if it's in our cache first. If so, return true
    CIDCtrls_GUIFacility::TBmpCacheItem* pitemCache
    (
        CIDCtrls_GUIFacility::colBmpCache.pobjFindByKey(strCacheName)
    );
    if (pitemCache)
        return kCIDLib::True;

    // If not, try to load it up. If it fails, then just return false
    TCIDImage* pimgNew = pimgLoadRawAppImg(pathActName);
    if (!pimgNew)
        return kCIDLib::False;

    // We've got this far so might as well cache it for possible later use
    TJanitor<TCIDImage> janImg(pimgNew);

    // Create the bitmap now, then put it in a cache item and add to the cache
    TGraphDesktopDev gdevTmp;
    TBitmap bmpRet(gdevTmp, *pimgNew);
    CIDCtrls_GUIFacility::TBmpCacheItem itemNew(strCacheName, bmpRet);

    //
    //  On the various unlikely chance that someone else beat us to it, use a dup
    //  tolerant add.
    //
    tCIDLib::TBoolean bAdded;
    CIDCtrls_GUIFacility::colBmpCache.objAddOrUpdate(itemNew, bAdded);

    return kCIDLib::True;
}


//
//  Load the indicated dialog description, from this facility, well the derived
//  class facility.
//
tCIDLib::TBoolean
TGUIFacility::bCreateDlgDesc(const  tCIDLib::TResId ridToCreate
                            ,       TDlgDesc&       dlgdToFill) const
{
    // Find the memory of the resource indicated
    tCIDLib::TCard4         c4Len;
    const tCIDLib::TCard1*  pc1Start = pc1FindRes
    (
        c4Len, ridToCreate, tCIDLib::EResTypes::Dialog, kCIDLib::True
    );

    if (!pc1Start)
        return kCIDLib::False;

    // Look at it as it really is
    const tCIDCtrls::TRawDlgDesc& rdlgdSrc =
                        *reinterpret_cast<const tCIDCtrls::TRawDlgDesc*>(pc1Start);

    //
    //  And let it construct itself from this raw dialog description resource.
    //  We pass a ref to ourself along because it will need to load text.
    //
    dlgdToFill.FromRaw(rdlgdSrc, *this);
    return kCIDLib::True;
}


//
//  Load an app image and convert it to a bitmap and return the bitmap.
//
TBitmap
TGUIFacility::bmpLoadAppImg(const   TString&                strName
                            , const tCIDCtrls::EAppImgSz    eSize
                            , const tCIDLib::TBoolean       bThrowIfNot)
{
    // Build up the full name to the image, and the cache name.
    TPathStr pathActName;
    TString  strCacheName;
    MakeAppImgName(strName, eSize, pathActName, strCacheName);

    // See if it's in our cache first. If so, return that
    CIDCtrls_GUIFacility::TBmpCacheItem* pitemCache
    (
        CIDCtrls_GUIFacility::colBmpCache.pobjFindByKey(strCacheName)
    );

    if (pitemCache)
        return pitemCache->m_bmpItem;

    // And try to load it up
    TCIDImage* pimgNew = pimgLoadRawAppImg(pathActName);
    if (!pimgNew)
    {
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcFac_UnknownAppImg
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strName
            );
        }
        return TBitmap();
    }
    TJanitor<TCIDImage> janImg(pimgNew);

    // Create the bitmap now, then put it in a cache item and add to the cache
    TGraphDesktopDev gdevTmp;
    TBitmap bmpRet(gdevTmp, *pimgNew);
    CIDCtrls_GUIFacility::TBmpCacheItem itemNew(strCacheName, bmpRet);

    //
    //  On the various unlikely chance that someone else beat us to it, use a dup
    //  tolerant add.
    //
    tCIDLib::TBoolean bAdded;
    CIDCtrls_GUIFacility::colBmpCache.objAddOrUpdate(itemNew, bAdded);

    // And return the new bitmap
    return bmpRet;
}

TBitmap
TGUIFacility::bmpLoadLargeAppImg(const  TString&            strName
                                , const tCIDLib::TBoolean   bThrowIfNot)
{
    return bmpLoadAppImg(strName, tCIDCtrls::EAppImgSz::Large, bThrowIfNot);
}

TBitmap
TGUIFacility::bmpLoadSmallAppImg(const  TString&            strName
                                , const tCIDLib::TBoolean   bThrowIfNot)
{
    return bmpLoadAppImg(strName, tCIDCtrls::EAppImgSz::Small, bThrowIfNot);
}


// Load an image from an image resource attached to this facility
TBitmap
TGUIFacility::bmpLoadResourceImg(const  tCIDLib::TResId     ridToLoad
                                , const tCIDLib::TBoolean   bThrowIfNot)
{
    // Try to load the image based resource which gets us a raw bitmap handle
    TGraphWndDev gdevCompat(TWindow::wndDesktop());
    tCIDGraphDev::TBmpHandle hbmpRet = TBitmap::hbmpCreate(*this, ridToLoad, gdevCompat);

    // If not found, then return an empty bitmap or throw if asked
    if (!hbmpRet)
    {
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcRes_NotFound
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strName()
                , TCardinal(ridToLoad)
            );
        }
        return TBitmap();
    }

    // Else give it the handle, and tell it to adopt it
    return TBitmap(hbmpRet, tCIDLib::EAdoptOpts::Adopt);
}


// Load the predefined system bitmaps
TBitmap TGUIFacility::bmpLoadSysBmp(const tCIDCtrls::ESysBmps eToLoad)
{
    // Convert the enum to the system value
    LPCWSTR lpSysName;
    switch(eToLoad)
    {
        case tCIDCtrls::ESysBmps::Size :
            lpSysName = (LPCWSTR)OBM_SIZE;
            break;

        case tCIDCtrls::ESysBmps::Check :
            lpSysName = (LPCWSTR)OBM_CHECK;
            break;

        case tCIDCtrls::ESysBmps::Close :
            lpSysName = (LPCWSTR)OBM_CLOSE;
            break;

        case tCIDCtrls::ESysBmps::DnArrow :
            lpSysName = (LPCWSTR)OBM_COMBO;
            break;

        case tCIDCtrls::ESysBmps::DnArrowBoxed :
            lpSysName = (LPCWSTR)OBM_DNARROW;
            break;

        case tCIDCtrls::ESysBmps::LeftArrow :
            lpSysName = (LPCWSTR)OBM_LFARROW;
            break;

        case tCIDCtrls::ESysBmps::RightArrow :
            lpSysName = (LPCWSTR)OBM_RGARROW;
            break;

        case tCIDCtrls::ESysBmps::UpArrow :
            lpSysName = (LPCWSTR)OBM_UPARROW;
            break;

        default :
            lpSysName = (LPCWSTR)OBM_BTNCORNERS;
            break;
    };

    tCIDGraphDev::TBmpHandle hbmpRet = ::LoadBitmap(0, lpSysName);
    if (!hbmpRet)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcFac_UnknownSysBmp
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
        );
    }

    return TBitmap(hbmpRet, tCIDLib::EAdoptOpts::Adopt);
}


//
//  Load up an app image associated with this facility and convert it to an icon
//  and return the icon.
//
TIcon
TGUIFacility::icoLoadAppIcon(const  TString&                strName
                            , const tCIDCtrls::EAppImgSz    eSize
                            , const tCIDLib::TBoolean       bThrowIfNot)
{
    // Build up the full name and the cache name of the image
    TPathStr pathActName;
    TString  strCacheName;
    MakeAppImgName(strName, eSize, pathActName, strCacheName);

    // See if it's in our cache first. If so, return that
    CIDCtrls_GUIFacility::TIconCacheItem* pitemCache
    (
        CIDCtrls_GUIFacility::colIconCache.pobjFindByKey(strCacheName)
    );

    if (pitemCache)
        return pitemCache->m_icoItem;

    // And try to load it
    TCIDImage* pimgLoad = pimgLoadRawAppImg(pathActName);
    if (!pimgLoad)
    {
        if (bThrowIfNot)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcFac_UnknownAppImg
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strName
            );
        }
        return TIcon();
    }
    TJanitor<TCIDImage> janImg(pimgLoad);

    TIcon icoRet;
    try
    {
        // Create the icon and put it into our cache
        icoRet = TIcon(*pimgLoad, TPoint::pntOrigin);
        CIDCtrls_GUIFacility::TIconCacheItem itemNew(strCacheName, icoRet);

        //
        //  On the various unlikely chance that someone else beat us to it, use a dup
        //  tolerant add.
        //
        tCIDLib::TBoolean bAdded;
        CIDCtrls_GUIFacility::colIconCache.objAddOrUpdate(itemNew, bAdded);
    }

    catch(TError& errToCatch)
    {
        if (bThrowIfNot)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }
        return TIcon();
    }

    // And return the new item
    return icoRet;
}

TIcon
TGUIFacility::icoLoadSmallAppIcon(  const   TString&            strName
                                    , const tCIDLib::TBoolean   bThrowIfNot)
{
    return icoLoadAppIcon(strName, tCIDCtrls::EAppImgSz::Small, bThrowIfNot);
}



//
//  We have a set of high res icon type images which we don't want to load
//  as resources since they take up a good bit of memory. So they are just
//  loaded as required and are in the same directory as the CIDWnd facility
//  executable.
//
//  These are all PNG images, but callers don't provide any path or extension,
//  just the basic name, so that we can change the actual mapping/loading
//  methods as required over time.
//
//  We cache them so that we don't end up just reloading them over and
//  over.
//
TCIDImage&
TGUIFacility::imgLoadAppImg(const   TString&                strName
                            , const tCIDCtrls::EAppImgSz    eSize)
{
    // Build the actual name and the cache name of the image
    TPathStr pathActName;
    TString  strCacheName;
    MakeAppImgName(strName, eSize, pathActName, strCacheName);

    // Lock the cache while we do this
    TMtxLocker mtxlCache(CIDCtrls_GUIFacility::colAppImgCache.pmtxLock());

    // See if it's in our cache first. If so, return that
    CIDCtrls_GUIFacility::TAppImgCacheItem* pitemCache
    (
        CIDCtrls_GUIFacility::colAppImgCache.pobjFindByKey(strCacheName)
    );

    if (pitemCache)
        return pitemCache->m_imgItem;

    //
    //  It's not cached, so load it up using the raw name. Provisionally
    //  add an item to the cache so that we can load directly into it and
    //  avoid a copy of the image data.
    //
    CIDCtrls_GUIFacility::TAppImgCacheItem& itemNew
    (
        CIDCtrls_GUIFacility::colAppImgCache.objAdd(strCacheName)
    );

    try
    {
        if (!bLoadRawAppImg(pathActName, itemNew.m_imgItem))
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcFac_UnknownAppImg
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::NotFound
                , strName
            );
        }

        // Make sure it's a true/alpha image, since they all should be
        if (itemNew.m_imgItem.ePixFmt() != tCIDImage::EPixFmts::TrueAlpha)
        {
            facCIDCtrls().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCtrlsErrs::errcFac_BadAppImgFmt
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , strName
            );
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // Remove the provisional cache item
        CIDCtrls_GUIFacility::colAppImgCache.bRemoveKey(strCacheName);
        throw;
    }
    return itemNew.m_imgItem;
}


//
//  Loads up a raw menu description resource
//
const tCIDCtrls::TRawMenuDesc*
TGUIFacility::prmdLoad(const tCIDLib::TResId ridToLoad) const
{
    tCIDLib::TCard4         c4Len;
    const tCIDLib::TCard1*  pc1Start = pc1FindRes
    (
        c4Len, ridToLoad, tCIDLib::EResTypes::Menu, kCIDLib::True
    );

    // Cast it to what its supposed to be
    const tCIDCtrls::TRawMenuDesc* prmdRet =
                        reinterpret_cast<const tCIDCtrls::TRawMenuDesc*>(pc1Start);

    //
    //  If debugging, then run through the data structure and kind of make
    //  sure that it looks reasonable.
    //
    #if CID_DEBUG_ON
    // <TBD>
    #endif

    // Return the data
    return prmdRet;
}


//
//  The caller gives us a window and the base name of an icon to set on it. We build
//  up the path to the icon and set it.
//
tCIDLib::TVoid TGUIFacility::SetFrameIcon(TWindow& wndTar, const TString& strName)
{
    // Build up the file path and load the icon
    TPathStr pathIcon(strPath());
    pathIcon.AddLevel(L"AppIcons");
    pathIcon.AddLevel(strName);
    pathIcon.AppendExt(L"ico");

    try
    {
        TIcon icoLarge(pathIcon, TSize(32, 32), 24);
        wndTar.SetIcon(icoLarge, kCIDLib::True);

        TIcon icoSmall(pathIcon, TSize(16, 16), 24);
        wndTar.SetIcon(icoSmall, kCIDLib::False);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        TModule::LogEventObj(errToCatch);
    }
}


// ---------------------------------------------------------------------------
//  TGUIFacility: Private, static data
// ---------------------------------------------------------------------------
tCIDLib::TBoolean   TGUIFacility::s_bMetricsLoaded = kCIDLib::False;
TArea               TGUIFacility::s_areaPrimaryMon;
TArea               TGUIFacility::s_areaVirtualScr;
tCIDLib::TCard4     TGUIFacility::s_c4DefFontHeight;
tCIDLib::TCard4     TGUIFacility::s_c4HorzDlgUnits;
tCIDLib::TCard4     TGUIFacility::s_c4VertDlgUnits;
TGUIFont*           TGUIFacility::s_pgfontDefBold;
TGUIFont*           TGUIFacility::s_pgfontDefFixed;
TGUIFont*           TGUIFacility::s_pgfontDefLight;
TGUIFont*           TGUIFacility::s_pgfontDefNorm;
TGUIFont*           TGUIFacility::s_pgfontDefSmall;
TGUIFont*           TGUIFacility::s_pgfontDefSmallLight;
TGUIFont*           TGUIFacility::s_pgfontDefTitle;
TString             TGUIFacility::s_strBoldFontFace;
TString             TGUIFacility::s_strDefFontFace;
TString             TGUIFacility::s_strLightFontFace;
TSize               TGUIFacility::s_szPrimaryMon;
TSize               TGUIFacility::s_szVirtualScr;



// ---------------------------------------------------------------------------
//  TGUIFacility: Private, static methods
// ---------------------------------------------------------------------------

//
//  Called from any methods that deal with some system metrics that we fault in
//  upon use.
//
tCIDLib::TVoid TGUIFacility::FaultInMetrics()
{
    if (!s_bMetricsLoaded)
    {
        TBaseLock lockInit;
        if (!s_bMetricsLoaded)
        {
            //
            //  Calculate our default font point size in pixels. We use a 9 point
            //  font as our default and everything else is off of that. We need the
            //  actual pixels to drive our dialog units calculations that are used
            //  to drive the size of stuff in dialogs that we create.
            //
            TGraphDesktopDev gdevTmp;
            tCIDLib::TCard4 c4PixelsPerInchY;
            c4PixelsPerInchY = gdevTmp.szPixelsPerInch().c4Height();

            tCIDLib::TCard4 c4Points = 9;
            s_c4DefFontHeight = MulDiv(c4Points, c4PixelsPerInchY, 72);
            s_strBoldFontFace = L"Segoe UI Bold";
            s_strDefFontFace = L"Segoe UI";
            s_strLightFontFace = L"Segoe UI Light";

            // Default is EM height which is what we want for a points based size
            TFontSelAttrs fselDraw;
            fselDraw.strFaceName(s_strDefFontFace);
            fselDraw.c4Height(s_c4DefFontHeight);
            fselDraw.eWeight(tCIDGraphDev::EFontWeights::Normal);
            s_pgfontDefNorm = new TGUIFont(fselDraw);

            {
                TFontJanitor janFonts(&gdevTmp, s_pgfontDefNorm);
                TFontMetrics fmtrFont(gdevTmp);

                s_c4HorzDlgUnits = fmtrFont.c4AverageWidth();
                if (s_c4HorzDlgUnits % 4)
                    s_c4HorzDlgUnits++;

                s_c4VertDlgUnits =  fmtrFont.c4Height();
                if (s_c4VertDlgUnits % 8)
                    s_c4VertDlgUnits++;
            }

            // Create a bold version of the default
            fselDraw.eWeight(tCIDGraphDev::EFontWeights::Bold);
            fselDraw.strFaceName(s_strBoldFontFace);
            s_pgfontDefBold = new TGUIFont(fselDraw);

            // A light version of the default
            fselDraw.eWeight(tCIDGraphDev::EFontWeights::Light);
            fselDraw.strFaceName(s_strLightFontFace);
            s_pgfontDefLight = new TGUIFont(fselDraw);

            // And a slightly larger one for titles, also bold
            fselDraw.eWeight(tCIDGraphDev::EFontWeights::Bold);
            fselDraw.c4Height(s_c4DefFontHeight + 2);
            fselDraw.strFaceName(s_strDefFontFace);
            s_pgfontDefTitle= new TGUIFont(fselDraw);

            // A small one at normal weight
            fselDraw.eWeight(tCIDGraphDev::EFontWeights::Normal);
            fselDraw.c4Height(s_c4DefFontHeight - 2);
            fselDraw.strFaceName(s_strDefFontFace);
            s_pgfontDefSmall = new TGUIFont(fselDraw);

            // A small one at light for fine print stuff
            fselDraw.eWeight(tCIDGraphDev::EFontWeights::Light);
            fselDraw.c4Height(s_c4DefFontHeight - 2);
            fselDraw.strFaceName(s_strLightFontFace);
            s_pgfontDefSmallLight = new TGUIFont(fselDraw);

            // A normal sized, fixed pitch font
            fselDraw.eWeight(tCIDGraphDev::EFontWeights::Normal);
            fselDraw.c4Height(s_c4DefFontHeight - 1);
            fselDraw.ePitch(tCIDGraphDev::EFontPitches::Fixed);
            fselDraw.strFaceName(L"Courier New");
            s_pgfontDefFixed = new TGUIFont(fselDraw);


            // Get the full virtual area and primary monitor area
            TVector<TGUIMonInfo> colMons;
            facCIDCtrls().QueryMonitors(colMons);
            const tCIDLib::TCard4 c4Count = colMons.c4ElemCount();
            s_areaPrimaryMon.ZeroAll();
            s_areaVirtualScr.ZeroAll();
            if (c4Count)
            {
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
                {
                    // Add this guy's physical area to the virtual screen size
                    s_areaVirtualScr |= colMons[c4Index].areaPhysical();

                    // Remember the physical area of the primary monitor
                    if (colMons[c4Index].bIsPrimary())
                        s_areaPrimaryMon = colMons[c4Index].areaPhysical();
                }
            }

            // Save the sizes of the above
            s_szVirtualScr = s_areaVirtualScr.szArea();
            s_szPrimaryMon = s_areaPrimaryMon.szArea();

            s_bMetricsLoaded = kCIDLib::True;
        }
    }
}


// ---------------------------------------------------------------------------
//  TGUIFacility: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TGUIFacility::bLoadRawAppImg(const TString& strRawName, TPNGImage& imgToLoad)
{
    try
    {
        TBinFileInStream strmImg
        (
            strRawName
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );
        strmImg >> imgToLoad;
    }

    catch(...)
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Builds a name to an app image. It's relative to our facility path and
//  is based on the passed name and the size indicator. We also build up a
//  name that the caller will use as a cache key, which needs to include the
//  base name and the size indicator.
//
tCIDLib::TVoid
TGUIFacility::MakeAppImgName(const  TString&                strName
                            , const tCIDCtrls::EAppImgSz    eSize
                            ,       TPathStr&               pathActName
                            ,       TString&                strCacheName) const
{
    // Start the actual path with our library path and add the AppImage subdir
    pathActName = strPath();
    pathActName.AddLevel(L"AppImages");

    // If a per-facility sub-dir, then add that
    if (!m_strImageDir.bIsEmpty())
        pathActName.AddLevel(m_strImageDir);

    // Store the size prefix in the cache name string
    switch(eSize)
    {
        case tCIDCtrls::EAppImgSz::Default :
        {
            TSize szIcon = szDefIcon();
            if (szIcon.c4Height() == 16)
                strCacheName = L"Sml_";
            else if (szIcon.c4Height() == 48)
                strCacheName = L"Lrg_";
            else
                strCacheName = L"Med_";
            break;
        }

        case tCIDCtrls::EAppImgSz::Small :
        {
            strCacheName = L"Sml_";
            break;
        }

        case tCIDCtrls::EAppImgSz::Medium :
        {
            strCacheName = L"Med_";
            break;
        }

        case tCIDCtrls::EAppImgSz::Large :
        {
            strCacheName = L"Lrg_";
            break;
        }

        default :
        {
            strCacheName = L"Med_";
            break;
        }
    };

    // Add this size prefix to the actual path name
    pathActName.AddLevel(strCacheName);

    // And now append the basic name and the extension
    pathActName.Append(strName);
    pathActName.AppendExt(L"png");

    // And finish up the cache name
    strCacheName.Append(strName);
}



//
//  Given the full raw path to one of our app images, load it up and
//  return the caller the image object, or a null pointer if not found. They
//  become responsible for cleaning it up.
//
//  In this case, we return a generic image pointer, so we can just pass
//  this through to the image factory facility.
//
TCIDImage* TGUIFacility::pimgLoadRawAppImg(const TString& strRawName) const
{
    // Tell it not to throw, so it'll just return null if this fails
    tCIDImage::EImgTypes eType;
    return facCIDImgFact().pimgLoadImgFile(strRawName, eType, kCIDLib::False);
}


