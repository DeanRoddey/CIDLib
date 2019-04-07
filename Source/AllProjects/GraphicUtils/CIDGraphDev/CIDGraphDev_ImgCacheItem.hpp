//
// FILE NAME: CIDCtrls_ImgCacheItem.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/28/2005
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
//  This is the header for the CIDCtrls_ImgCacheItem.cpp file, which implements
//  a simple image cache item that various programs may want to use (after
//  extending) to create an image cache.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TImgCacheItem
//  PREFIX: ici
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TImgCacheItem : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strImgName
        (
            const   TImgCacheItem&          iciSrc
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TImgCacheItem
        (
            const   TString&                strImgName
            , const tCIDLib::TBoolean       bThumb
        );

        TImgCacheItem
        (
            const   TString&                strImgName
            , const tCIDLib::TBoolean       bThumb
            , const tCIDLib::TCard4         c4Size
            , const TBitmap&                bmpData
            , const tCIDLib::TBoolean       bDeepCopy
        );

        TImgCacheItem
        (
            const   TImgCacheItem&          I_NsClientBindSearch
        );

        ~TImgCacheItem();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TImgCacheItem& operator=
        (
            const   TImgCacheItem&          iciSrc
        );


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid Reset
        (
            const   TString&                strImgName
            , const tCIDLib::TBoolean       bThumb
        );

        virtual tCIDLib::TVoid Set
        (
            const   tCIDLib::TBoolean       bThumb
            , const tCIDLib::TCard4         c4Size
            , const TMemBuf&                mbufData
            , const tCIDLib::TCard4         c4SerialNum
            , const TGraphDrawDev&          gdevCompat
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bHasAlpha() const;

        tCIDLib::TBoolean bStillGood
        (
            const   TString&                strExpiresStamp
        )   const;

        tCIDLib::TBoolean bThumb() const;

        tCIDLib::TBoolean bTransparent() const;

        const TBitmap& bmpImage() const;

        const TBitmap& bmpMask() const;

        tCIDLib::TCard4 c4SerialNum() const;

        tCIDLib::TCard4 c4Size() const;

        tCIDLib::TCard4 c4TransClr() const;

        tCIDLib::TVoid DropImage();

        tCIDLib::TEncodedTime enctLastAccess() const;

        tCIDLib::TEncodedTime enctLastCheck() const;

        tCIDLib::TEncodedTime enctGoodTill() const;

        const TString& strExpiresStamp() const;

        const TString& strExpiresStamp
        (
            const   TString&                strToSet
        );

        const TString& strImageName() const;

        const TString& strTagData() const;

        const TString& strTagData
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid SetLastAccess();

        tCIDLib::TVoid SetLastCheck();

        tCIDLib::TVoid SetGoodTill
        (
            const   tCIDLib::TCard4         c4Seconds
        );

        const TSize& szImage() const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        TBitmap& bmpWriteable();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bThumb
        //      Indicatse if this is a thumbnail or the full size image.
        //
        //  m_bTrans
        //      Indicates if this a color based transparency image. If so, then the
        //      mask images are needed for drawing.
        //
        //  m_bmpImage
        //  m_bmpMask
        //      The image and the mask for the image if it is transparent.
        //
        //  m_c4Size
        //      The size of the original raw image data, so we'll know how much is
        //      required if we have to send it.
        //
        //  m_enctLastAccess
        //      Updated each time the image is accessed, and used to do a least recently
        //      used algorithm if our cache gets full.
        //
        //  m_enctLastCheck
        //      The last time that the client code checked to see if the image
        //      is still up to date with any external source. This can be
        //      used to prevent repeated checks when the image is accessed
        //      multipled times quickly.
        //
        //  m_enctGoodTill
        //      In some cases, it's known that an image will be valid for some length of
        //      time, in which case this can be set and used to avoid re-validating the
        //      cached image for that period. Defaults to zero, which means it will by
        //      default never be good.
        //
        //  m_c4TransClr
        //      If color transparency based (m_bTrans is true), then this is the color that
        //      was used to create the masks.
        //
        //  m_strExpiresStamp
        //      This is used to store an expiration stamp, generally for use with HTTP
        //      servers, which may provide such a stamp. If it provides a max-age that
        //      will be set via the SetGoodTill and will override this. If the good till
        //      is zero, then we try this guy.
        //
        //  m_strImageName
        //      The original name of the image, whihc is whatever is meaningful
        //      to the user of this class.
        //
        //  m_strTagData
        //      This is for the use of the application, to store some kind of tag info.
        //
        //  m_szImage
        //      Get the size out up front, since we have to return it fairly often, and
        //      the bitmap itself returns it by value, so we can be more efficient.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bThumb;
        tCIDLib::TBoolean       m_bTrans;
        TBitmap                 m_bmpImage;
        TBitmap                 m_bmpMask;
        tCIDLib::TCard4         m_c4Size;
        tCIDLib::TCard4         m_c4TransClr;
        tCIDLib::TEncodedTime   m_enctLastAccess;
        tCIDLib::TEncodedTime   m_enctLastCheck;
        tCIDLib::TEncodedTime   m_enctGoodTill;
        TString                 m_strExpiresStamp;
        TString                 m_strImageName;
        TString                 m_strTagData;
        TSize                   m_szImage;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TImgCacheItem,TObject)
};


// ---------------------------------------------------------------------------
//  Define a counter pointer to an image cache object. This is what normally would be
//  handed out to callers when they want to use an image. This insures that the image
//  will not be dropped from the cache as long as someone is holding it.
// ---------------------------------------------------------------------------
using TImgCacheItemPtr = TCntPtr<TImgCacheItem>;
inline const TString&
strExtractImgCacheItemPtrKey(const TImgCacheItemPtr& cptrSrc)
{
    return cptrSrc->strImageName();
}

#pragma CIDLIB_POPPACK
