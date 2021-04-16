//
// FILE NAME: CIDImgFact_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/02/2013
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
//  This file implements the facility class for this facility. It is a
//  derivative (as all facility objects are) of TFacility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDImgFact_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDImgFact,TFacility)



// ---------------------------------------------------------------------------
//   CLASS: TFacCIDImgFact
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDImgFact: Constructors and Destructor
// ---------------------------------------------------------------------------
TFacCIDImgFact::TFacCIDImgFact() :

    TFacility
    (
        L"CIDImgFact"
        , tCIDLib::EModTypes::SharedLib
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
    , m_strExtBmp(L"bmp")
    , m_strExtJPG(L"jpg")
    , m_strExtPNG(L"png")
{
}

TFacCIDImgFact::~TFacCIDImgFact()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDImgFact: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Given a memory buffer with, presumably, image data, we probe the buffer
//  to figure out what type of image, create the correct type of image object
//  and return a pointer to it. If successful, we return true, else false.
//
tCIDLib::TBoolean
TFacCIDImgFact::bDecodeImg( const   TMemBuf&            mbufImg
                            , const tCIDLib::TCard4     c4ImgSz
                            ,       TCIDImage*&         pimgToSet
                            , const tCIDLib::TBoolean   bThrowIfNot) const
{
    // Just call the other and pass along the buffer point
    return bDecodeImg
    (
        mbufImg.pc1Data(), c4ImgSz, pimgToSet, bThrowIfNot
    );
}

tCIDLib::TBoolean
TFacCIDImgFact::bDecodeImg( const   tCIDLib::TVoid* const   pImgData
                            , const tCIDLib::TCard4         c4ImgSz
                            ,       TCIDImage*&             pimgToSet
                            , const tCIDLib::TBoolean       bThrowIfNot) const
{
    // Assume we fail until proven otherwise
    pimgToSet = 0;

    // Probe the image data to see what type of image
    const tCIDImage::EImgTypes eType = eProbeImg(pImgData, c4ImgSz);

    // Create an input stream over the data
    TBinMBufInStream strmIn
    (
        (const tCIDLib::TCard1*)pImgData, c4ImgSz
    );

    // And according to image type, stream it in
    try
    {
        switch(eType)
        {
            case tCIDImage::EImgTypes::Bitmap :
            {
                TBitmapImage* pimgBmp = new TBitmapImage();
                TJanitor<TBitmapImage> janBmp(pimgBmp);

                strmIn >> *pimgBmp;
                pimgToSet = janBmp.pobjOrphan();
                break;
            }

            case tCIDImage::EImgTypes::JPEG :
            {
                TJPEGImage* pimgJPEG = new TJPEGImage();
                TJanitor<TJPEGImage> janJPG(pimgJPEG);

                strmIn >> *pimgJPEG;
                pimgToSet = janJPG.pobjOrphan();
                break;
            }

            case tCIDImage::EImgTypes::PNG :
            {
                TPNGImage* pimgPNG = new TPNGImage();
                TJanitor<TPNGImage> janPNG(pimgPNG);

                strmIn >> *pimgPNG;
                pimgToSet = janPNG.pobjOrphan();
                break;
            }

            default :
            case tCIDImage::EImgTypes::Unknown :
                break;
        };
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // If we didn't get one
    if (!pimgToSet)
    {
        if (bThrowIfNot)
        {
            facCIDImage().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kImgErrs::errcGen_UnknownImgType
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  Like the other version above, but in this case we will convert to a
//  target preferred format.
//
tCIDLib::TBoolean
TFacCIDImgFact::bDecodeImgTo(const  TMemBuf&                mbufImg
                            , const tCIDLib::TCard4         c4ImgSz
                            , const tCIDImage::EImgTypes    eTarType
                            ,       TCIDImage*&             pimgToSet
                            , const tCIDLib::TBoolean       bThrowIfNot) const
{
    // Call the other version and pass along the buffer point
    return bDecodeImgTo
    (
        mbufImg.pc1Data()
        , c4ImgSz
        , eTarType
        , pimgToSet
        , bThrowIfNot
    );
}

tCIDLib::TBoolean
TFacCIDImgFact::bDecodeImgTo(const  tCIDLib::TVoid* const   pImgData
                            , const tCIDLib::TCard4         c4ImgSz
                            , const tCIDImage::EImgTypes    eTarType
                            ,       TCIDImage*&             pimgToSet
                            , const tCIDLib::TBoolean       bThrowIfNot) const
{
    // Make sure the target type isn't the unknown type
    CIDAssert
    (
        eTarType != tCIDImage::EImgTypes::Unknown
        , L"Can't create unknown image type"
    );

    //
    //  First call the other version. If throwing and it fails, it throws.
    //  If not throwing, then we just return its failure.
    //
    try
    {
        if (!bDecodeImg(pImgData, c4ImgSz, pimgToSet, bThrowIfNot))
            return kCIDLib::False;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }

    // If it's not already the target format, convert it
    if (pimgToSet->eType() != eTarType)
    {
        try
        {
            // We are not keeping the original, make sure it gets cleaned up
            TCIDImage* pimgOrg = pimgToSet;
            TJanitor<TCIDImage> janOrg(pimgOrg);

            pimgToSet = 0;
            switch(eTarType)
            {
                case tCIDImage::EImgTypes::Bitmap :
                {
                    TBitmapImage* pimgBmp = new TBitmapImage();
                    TJanitor<TBitmapImage> janBmp(pimgBmp);
                    *pimgBmp = *pimgOrg;
                    pimgToSet = janBmp.pobjOrphan();
                    break;
                }

                case tCIDImage::EImgTypes::JPEG :
                {
                    TJPEGImage* pimgJPEG = new TJPEGImage();
                    TJanitor<TJPEGImage> janJPG(pimgJPEG);
                    *pimgJPEG = *pimgOrg;
                    pimgToSet = janJPG.pobjOrphan();
                    break;
                }

                case tCIDImage::EImgTypes::PNG :
                {
                    TPNGImage* pimgPNG = new TPNGImage();
                    TJanitor<TPNGImage> janPNG(pimgPNG);
                    *pimgPNG = *pimgOrg;
                    pimgToSet = janPNG.pobjOrphan();
                    break;
                }

                default :
                case tCIDImage::EImgTypes::Unknown :
                    CIDAssert2(L"Should not have gotten here");
                    break;
            };
        }

        catch(TError& errToCatch)
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            throw;
        }
    }

    // If we didn't get one
    if (!pimgToSet)
    {
        if (bThrowIfNot)
        {
            facCIDImage().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kImgErrs::errcGen_UnknownImgType
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }
        return kCIDLib::False;
    }
    return kCIDLib::True;
}



//
//  Sometimes programs deal only with JPEGs and need to load images, converting
//  them to JPEG if they aren't already. These helpers handle that work,
//  keeping that grunt work out of client code.
//
tCIDLib::TBoolean
TFacCIDImgFact::bLoadToBitmap(  const   TString&            strPath
                                ,       TBitmapImage&       imgToLoad
                                , const tCIDLib::TBoolean   bThrowIfNot) const
{
    tCIDLib::TBoolean bRet = kCIDLib::False;
    try
    {
        TBinaryFile flImg(strPath);
        flImg.Open
        (
            tCIDLib::EAccessModes::Read
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );
        const tCIDLib::TCard4 c4Size = tCIDLib::TCard4(flImg.c8CurSize());
        THeapBuf mbufImg(c4Size, c4Size);
        flImg.c4ReadBuffer(mbufImg, c4Size);

        // Call the other version that loads from a memory buffer
        bRet = bLoadToBitmap(mbufImg, c4Size, imgToLoad, bThrowIfNot);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        if (bShouldLog(errToCatch))
            TModule::LogEventObj(errToCatch);

        // If throwing, then rethrow, else fall through with false
        if (bThrowIfNot)
            throw;
    }
    return bRet;
}

tCIDLib::TBoolean
TFacCIDImgFact::bLoadToBitmap(  const   TMemBuf&            mbufImgData
                                , const tCIDLib::TCard4     c4ImgSz
                                ,       TBitmapImage&       imgToLoad
                                , const tCIDLib::TBoolean   bThrowIfNot) const
{
    try
    {
        // See what type it is. If not supported, throw or return
        const tCIDImage::EImgTypes eType = eProbeImg(mbufImgData.pc1Data(), c4ImgSz);
        if (eType == tCIDImage::EImgTypes::Unknown)
        {
            if (bThrowIfNot)
            {
                facCIDImage().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kImgErrs::errcGen_UnknownImgType
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            }
            return kCIDLib::False;
        }

        //
        //  If it's a bitmap, we can just directly load it. Else we load it
        //  to the actual type and convert.
        //
        TBinMBufInStream strmIn(&mbufImgData, c4ImgSz);
        if (eType == tCIDImage::EImgTypes::Bitmap)
        {
            strmIn >> imgToLoad;
        }
         else if (eType == tCIDImage::EImgTypes::JPEG)
        {
            TJPEGImage imgJPEG;
            strmIn >> imgJPEG;
            imgToLoad = imgJPEG;
        }
         else if (eType == tCIDImage::EImgTypes::PNG)
        {
            TPNGImage imgPNG;
            strmIn >> imgPNG;
            imgToLoad = imgPNG;
        }
         else
        {
            CIDAssert2(L"Should not have gotten here");
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        if (bShouldLog(errToCatch))
            TModule::LogEventObj(errToCatch);

        // If throwing, then rethrow
        if (bThrowIfNot)
            throw;

        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TFacCIDImgFact::bLoadToJPEG(const   TString&            strPath
                            ,       TJPEGImage&         imgToLoad
                            , const tCIDLib::TBoolean   bThrowIfNot) const
{
    tCIDLib::TBoolean bRet = kCIDLib::False;
    try
    {
        TBinaryFile flImg(strPath);
        flImg.Open
        (
            tCIDLib::EAccessModes::Read
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );
        const tCIDLib::TCard4 c4Size = tCIDLib::TCard4(flImg.c8CurSize());
        THeapBuf mbufImg(c4Size, c4Size);
        flImg.c4ReadBuffer(mbufImg, c4Size);

        // Call the other version that loads from a memory buffer
        bRet = bLoadToJPEG(mbufImg.pc1Data(), c4Size, imgToLoad, bThrowIfNot);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        if (bShouldLog(errToCatch))
            TModule::LogEventObj(errToCatch);

        // If throwing, then rethrow, else fall through with false
        if (bThrowIfNot)
            throw;
    }
    return bRet;
}

tCIDLib::TBoolean
TFacCIDImgFact::bLoadToJPEG(const   TMemBuf&            mbufImgData
                            , const tCIDLib::TCard4     c4ImgSz
                            ,       TJPEGImage&         imgToLoad
                            , const tCIDLib::TBoolean   bThrowIfNot) const
{
    // Call the other version, passing along the buffer point
    return bLoadToJPEG(mbufImgData.pc1Data(), c4ImgSz, imgToLoad, bThrowIfNot);
}

tCIDLib::TBoolean
TFacCIDImgFact::bLoadToJPEG(const   tCIDLib::TVoid* const   pImgData
                            , const tCIDLib::TCard4         c4ImgSz
                            ,       TJPEGImage&             imgToLoad
                            , const tCIDLib::TBoolean       bThrowIfNot) const
{
    try
    {
        // See what type it is. If not supported, throw or return
        const tCIDImage::EImgTypes eType = eProbeImg(pImgData, c4ImgSz);
        if (eType == tCIDImage::EImgTypes::Unknown)
        {
            if (bThrowIfNot)
            {
                facCIDImage().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kImgErrs::errcGen_UnknownImgType
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            }
            return kCIDLib::False;
        }

        //
        //  If it's a JPEG, we can just directly load it. Else we load it
        //  to the actual type and convert.
        //
        TBinMBufInStream strmIn((const tCIDLib::TCard1*)pImgData, c4ImgSz);
        if (eType == tCIDImage::EImgTypes::Bitmap)
        {
            TBitmapImage imgBmp;
            strmIn >> imgBmp;
            imgToLoad = imgBmp;
        }
         else if (eType == tCIDImage::EImgTypes::JPEG)
        {
            strmIn >> imgToLoad;
        }
         else if (eType == tCIDImage::EImgTypes::PNG)
        {
            TPNGImage imgPNG;
            strmIn >> imgPNG;
            imgToLoad = imgPNG;
        }
         else
        {
            CIDAssert2(L"Should not have gotten here");
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        if (bShouldLog(errToCatch))
            TModule::LogEventObj(errToCatch);

        // If throwing, then rethrow
        if (bThrowIfNot)
            throw;

        return kCIDLib::False;
    }
    return kCIDLib::True;
}



//
//  PNGs are explictly used a lot, particularly at the CQC level. So there's
//  often a need to load a file into a PNG. So we provide this helper to do
//  that.
//
tCIDLib::TBoolean
TFacCIDImgFact::bLoadToPNG( const   TString&            strPath
                            ,       TPNGImage&          imgToLoad
                            , const tCIDLib::TBoolean   bThrowIfNot) const
{
    tCIDLib::TBoolean bRet = kCIDLib::False;
    try
    {
        TBinaryFile flImg(strPath);
        flImg.Open
        (
            tCIDLib::EAccessModes::Read
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );
        const tCIDLib::TCard4 c4Size = tCIDLib::TCard4(flImg.c8CurSize());
        THeapBuf mbufImg(c4Size, c4Size);
        flImg.c4ReadBuffer(mbufImg, c4Size);

        // Call the other version that loads from a memory buffer
        bRet = bLoadToPNG(mbufImg.pc1Data(), c4Size, imgToLoad, bThrowIfNot);
    }

    catch(TError& errToCatch)
    {
        if (bShouldLog(errToCatch))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        // If throwing, then rethrow, else fall through with false
        if (bThrowIfNot)
            throw;
    }
    return bRet;
}

tCIDLib::TBoolean
TFacCIDImgFact::bLoadToPNG( const   TMemBuf&            mbufImgData
                            , const tCIDLib::TCard4     c4ImgSz
                            ,       TPNGImage&          imgToLoad
                            , const tCIDLib::TBoolean   bThrowIfNot) const
{
    // Call the other version, passing along the buffer point
    return bLoadToPNG(mbufImgData.pc1Data(), c4ImgSz, imgToLoad, bThrowIfNot);
}

tCIDLib::TBoolean
TFacCIDImgFact::bLoadToPNG( const   tCIDLib::TVoid* const   pImgData
                            , const tCIDLib::TCard4         c4ImgSz
                            ,       TPNGImage&              imgToLoad
                            , const tCIDLib::TBoolean       bThrowIfNot) const
{
    try
    {
        // See what type it is. If not supported, throw or return
        const tCIDImage::EImgTypes eType = eProbeImg(pImgData, c4ImgSz);
        if (eType == tCIDImage::EImgTypes::Unknown)
        {
            if (bThrowIfNot)
            {
                facCIDImage().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kImgErrs::errcGen_UnknownImgType
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            }
            return kCIDLib::False;
        }

        //
        //  If it's a PNG, we can just directly load it. Else we load it
        //  to the actual type and convert.
        //
        TBinMBufInStream strmIn((const tCIDLib::TCard1*)pImgData, c4ImgSz);
        if (eType == tCIDImage::EImgTypes::Bitmap)
        {
            TBitmapImage imgBmp;
            strmIn >> imgBmp;
            imgToLoad = imgBmp;
        }
         else if (eType == tCIDImage::EImgTypes::JPEG)
        {
            TJPEGImage imgJPEG;
            strmIn >> imgJPEG;
            imgToLoad = imgJPEG;
        }
         else if (eType == tCIDImage::EImgTypes::PNG)
        {
            strmIn >> imgToLoad;
        }
         else
        {
            CIDAssert2(L"Should not have gotten here");
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        if (bShouldLog(errToCatch))
            TModule::LogEventObj(errToCatch);

        // If throwing, then rethrow
        if (bThrowIfNot)
            throw;

        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  This is a standard method to rescale an image an image, converting
//  it also to JPEG at reasonable quality. This is a commonly desired
//  operation where images of arbitrary type and size are imported and
//  need to be reduced to reasonable size/quality for storage and
//  subsequent use.
//
tCIDLib::TCard4
TFacCIDImgFact::c4CompactToJPEG(        TMemBuf&            mbufInOut
                                , const tCIDLib::TCard4     c4OrgSize
                                , const TSize&              szMaxRes) const
{
    tCIDLib::TCard4 c4Ret = 0;
    try
    {
        TCIDImage* pimgNew;
        bDecodeImg(mbufInOut, c4OrgSize, pimgNew, kCIDLib::True);
        TJanitor<TCIDImage> janImg(pimgNew);

        // See if we need to rescale it and adjust the size if so
        tCIDLib::TBoolean bScale = kCIDLib::False;
        TSize szImg = pimgNew->szImage();
        if (szImg.c4Width() > szImg.c4Height())
        {
            if (szImg.c4Width() > szMaxRes.c4Width())
            {
                bScale = kCIDLib::True;
                szImg.ScaleToWidthAR(szMaxRes.c4Width());
            }
        }
         else
        {
            if (szImg.c4Height() > szMaxRes.c4Height())
            {
                bScale = kCIDLib::True;
                szImg.ScaleToHeightAR(szMaxRes.c4Height());
            }
        }

        //
        //  If we need to scale it, then do that and we have to then write it
        //  back out to the buffer. Otherwise, we can just keep the original
        //  data and return the original size.
        //
        c4Ret = c4OrgSize;
        if (bScale)
        {
            pimgNew->Scale(szImg, 2);

            //
            //  And let's write it back out, using a moderate image quality and
            //  encoding settings that will minimize file size. We have to
            //  convert it to jPEG format if not already. If it's already JPG
            //  just replace our existing one.
            //
            TJPEGImage* pimgJPEG;
            if (pimgNew->bIsA(TJPEGImage::clsThis()))
                pimgJPEG = static_cast<TJPEGImage*>(janImg.pobjOrphan());
            else
                pimgJPEG = new TJPEGImage(*pimgNew);

            //
            //  And make sure this new one get's cleaned up. If it was a JPEG,
            //  we orphaned it from the other janitor. If it wasn't we created
            //  a new one, so either way we know will need to clean this up. If
            //  it wasn't JPEG, then the original janitor still owns the initial
            //  image.
            //
            TJanitor<TJPEGImage> janJPEG(pimgJPEG);

            pimgJPEG->bOptimalEncoding(kCIDLib::True);
            pimgJPEG->c4CompQuality(80);
            pimgJPEG->eOutSample(tCIDJPEG::EOutSamples::F4_2_2);
            {
                TBinMBufOutStream strmTar(&mbufInOut);
                strmTar << *pimgJPEG << kCIDLib::FlushIt;
                c4Ret = strmTar.c4CurSize();
            }
        }
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        throw;
    }
    return c4Ret;
}


//
//  Probe an image buffer to see what type of image it is. We look for
//  known markers used by the supported image types.
//
tCIDImage::EImgTypes
TFacCIDImgFact::eProbeImg(  const   TMemBuf&        mbufData
                            , const tCIDLib::TCard4 c4Bytes) const
{
    // Call the other version, passing the buffer pointer
    return eProbeImg(mbufData.pc1Data(), c4Bytes);
}


tCIDImage::EImgTypes
TFacCIDImgFact::eProbeImg(  const   tCIDLib::TVoid* const   pImgData
                            , const tCIDLib::TCard4         c4Bytes) const
{
    // If it's less than 16 bytes, it cannot be any kind of legal image
    if (c4Bytes < 16)
        return tCIDImage::EImgTypes::Unknown;

    // Get a byte pointer to it
    const tCIDLib::TCard1* pc1Buf = (const tCIDLib::TCard1*)pImgData;

    tCIDLib::TCard2 c2Marker;
    tCIDLib::TCard8 c8Marker;

    // Look for the bitmap marker. It's little endian so flip if big
    c2Marker = *(tCIDLib::TCard2*)pc1Buf;
    #if defined(CIDLIB_BIGENDIAN)
    c2Marker = TRawBits::c2SwapBytes(c2Marker);
    #endif
    if ((c2Marker == 0x4142) || (c2Marker == 0x4D42))
        return tCIDImage::EImgTypes::Bitmap;

    // Check for a PNG image marker. It's little endian, so flip if big
    c8Marker = *(tCIDLib::TCard8*)pc1Buf;
    #if defined(CIDLIB_BIGENDIAN)
    c8Marker = TRawBits::c8SwapBytes(c8Marker);
    #endif
    if (c8Marker == 0x0A1A0A0D474E5089)
        return tCIDImage::EImgTypes::PNG;

    // And check for a JPEG marker
    if ((pc1Buf[0] == 0xFF) && (pc1Buf[1] == 0xD8))
        return tCIDImage::EImgTypes::JPEG;

    // Dunno what it is
    return tCIDImage::EImgTypes::Unknown;
}


//
//  Given a file extension, try to match it to one of our supported image
//  types and return the type.
//
tCIDImage::EImgTypes TFacCIDImgFact::eTypeFromExt(const TString& strExt) const
{
    if (strExt.bCompareI(L"bmp"))
        return tCIDImage::EImgTypes::Bitmap;

    if (strExt.bCompareI(L"jpg") || strExt.bCompareI(L"jpeg"))
        return tCIDImage::EImgTypes::JPEG;

    if (strExt.bCompareI(L"png"))
        return tCIDImage::EImgTypes::PNG;

    return tCIDImage::EImgTypes::Unknown;
}


//
//  Format out the pixel format and bit depth in a standard format
//
tCIDLib::TVoid
TFacCIDImgFact::FormatImgFormat(        TString&                strToFill
                                , const tCIDImage::EPixFmts     eFmt
                                , const tCIDImage::EBitDepths   eDepth
                                , const tCIDLib::TBoolean       bTwoLines) const
{
    if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Palette))
    {
        bLoadCIDMsg
        (
            kImgFactMsgs::midFmt_Palette
            , strToFill
            , TInteger(tCIDLib::i4EnumOrd(eDepth))
        );
    }
     else
    {
        TString strTmp;
        bLoadCIDMsg
        (
            kImgFactMsgs::midFmt_BitsPerClr
            , strTmp
            , TInteger(tCIDLib::i4EnumOrd(eDepth))
        );

        if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Color))
        {
            if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Alpha))
                bLoadCIDMsg(kImgFactMsgs::midFmt_TrueAlpha, strToFill);
            else
                bLoadCIDMsg(kImgFactMsgs::midFmt_TrueColor, strToFill);
        }
         else
        {
            if (tCIDLib::bAllBitsOn(eFmt, tCIDImage::EPixFmts::Alpha))
                bLoadCIDMsg(kImgFactMsgs::midFmt_GrayAlpha, strToFill);
            else
                bLoadCIDMsg(kImgFactMsgs::midFmt_GrayScale, strToFill);
        }
        if (bTwoLines)
            strToFill.Append(L"\n");
        else
            strToFill.Append(kCIDLib::chSpace);
        strToFill.Append(strTmp);
    }
}


// Create a new, default constructed, image of the indicated type
TCIDImage*
TFacCIDImgFact::pimgCreateNew(const tCIDImage::EImgTypes eType) const
{
    TCIDImage* pimgRet = 0;
    switch(eType)
    {
        case tCIDImage::EImgTypes::Bitmap :
            pimgRet = new TBitmapImage();
            break;

        case tCIDImage::EImgTypes::JPEG :
            pimgRet = new TJPEGImage();
            break;

        case tCIDImage::EImgTypes::PNG :
            pimgRet = new TPNGImage();
            break;

        default :
        case tCIDImage::EImgTypes::Unknown :
            break;
    };

    if (!pimgRet)
    {
        facCIDImage().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kImgErrs::errcGen_UnknownImgType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // We got one, so return it
    return pimgRet;
}



//
//  Loads up an image from a file. We load the raw data, see what type of
//  image it is by probing the buffer, create the right type of image object,
//  and stream the image data in. If it works we return the new image object,
//  which the caller is responsible for. We also give back the image type
//  we got.
//
TCIDImage*
TFacCIDImgFact::pimgLoadImgFile(const   TString&                strPath
                                ,       tCIDImage::EImgTypes&   eType
                                , const tCIDLib::TBoolean       bThrowIfNot) const
{
    TCIDImage* pimgRet = 0;
    try
    {
        TBinaryFile flImg(strPath);
        flImg.Open
        (
            tCIDLib::EAccessModes::Read
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
        );
        const tCIDLib::TCard4 c4Size = tCIDLib::TCard4(flImg.c8CurSize());
        THeapBuf mbufImg(c4Size, c4Size);
        flImg.c4ReadBuffer(mbufImg, c4Size);

        //
        //  Try to load it up. Use a temp pointer. If it works, store the
        //  temp into the return object, else leave it null. Pass in the
        //  throw flag that we got.
        //
        TCIDImage* pimgNew = 0;
        if (bDecodeImg(mbufImg, c4Size, pimgNew, bThrowIfNot))
            pimgRet = pimgNew;
    }

    catch(TError& errToCatch)
    {
        if (bShouldLog(errToCatch))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }

        // If throwing, then rethrow now. Else fall through
        if (bThrowIfNot)
            throw;
    }

    return pimgRet;
}


//
//  Return a const ref to a string with the correct extension for the indicated
//  image type.
//
const TString&
TFacCIDImgFact::strImgTypeExt(const tCIDImage::EImgTypes eType) const
{
    const TString* pstrRet = &TString::strEmpty();
    switch(eType)
    {
        case tCIDImage::EImgTypes::Bitmap :
            pstrRet = &m_strExtBmp;
            break;

        case tCIDImage::EImgTypes::JPEG :
            pstrRet = &m_strExtJPG;
            break;

        case tCIDImage::EImgTypes::PNG :
            pstrRet = &m_strExtPNG;
            break;

        default :
        case tCIDImage::EImgTypes::Unknown :
            break;
    };

    return *pstrRet;
}


