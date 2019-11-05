//
// FILE NAME: CIDGraphDev_Icon.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/1998
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
//  This file implements the TIcon class, which is the platform independent
//  abstraction for a host icon.
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
RTTIDecls(TIcon,TObject)


// ---------------------------------------------------------------------------
//  Some platform structures that we just do manually
// ---------------------------------------------------------------------------
#pragma CIDLIB_PACK(1)
struct NEWHEADER
{
    WORD Reserved;
    WORD ResType;
    WORD ResCount;
};

struct ICONRESDIR
{
    BYTE Width;
    BYTE Height;
    BYTE ColorCount;
    BYTE Reserved;
};

struct CURSORDIR
{
    WORD Width;
    WORD Height;
};

// As extracted
struct RESDIR
{
    union
    {
        ICONRESDIR Icon;
        CURSORDIR  Cursor;
    }   ResInfo;
    WORD Planes;
    WORD BitCount;
    DWORD BytesInRes;
    WORD IconCursorId;
};

// In the file
struct FRESDIR
{
    union
    {
        ICONRESDIR Icon;
        CURSORDIR  Cursor;
    }   ResInfo;
    WORD Planes;
    WORD BitCount;
    DWORD BytesInRes;
    DWORD FileOfs;
};


// ---------------------------------------------------------------------------
//  These structures define the format of icon files, which are stored in
//  the .CIDRes file as is. These structures are not in any system headers
//  so we have to define them ourself.
// ---------------------------------------------------------------------------
struct TIconDirEntry
{
    tCIDLib::TCard1 c1Width;
    tCIDLib::TCard1 c1Height;
    tCIDLib::TCard1 c1ColorCount;
    tCIDLib::TCard1 c1Reserved;
    tCIDLib::TCard2 c2Planes;
    tCIDLib::TCard2 c2BitCount;
    tCIDLib::TCard4 c4BytesInRes;
    tCIDLib::TCard4 c4ImageOffset;
};

struct TIconDir
{
    tCIDLib::TCard2 c2Reserved;
    tCIDLib::TCard2 c2Type;
    tCIDLib::TCard2 c2Count;
    TIconDirEntry   ideEntries[1];
};

#pragma CIDLIB_POPPACK



// ---------------------------------------------------------------------------
//   CLASS: TRefIconHandle
//  PREFIX: rih
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TIcon::TRefIconHandle: Ctors and Dtor
// ---------------------------------------------------------------------------
TIcon::TRefIconHandle::TRefIconHandle() :

    m_c4Height(0)
    , m_c4Width(0)
    , m_hicoThis(kCIDGraphDev::hicoInvalid)
{
}

TIcon::TRefIconHandle::TRefIconHandle(tCIDGraphDev::TIconHandle hicoToAdopt) :

    m_hicoThis(hicoToAdopt)
{
    // We can't accept an invalid handle
    if (hicoToAdopt == kCIDGraphDev::hicoInvalid)
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcIcon_InvalidHandle
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }

    // Update our info
    UpdateInfo();
}

TIcon::TRefIconHandle::~TRefIconHandle()
{
    // Clean up the icon we manage
    if (m_hicoThis != kCIDGraphDev::hicoInvalid)
    {
        if (!::DestroyIcon(m_hicoThis))
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDGraphDev().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcIcon_Delete
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::Internal
            );
        }
        m_hicoThis = kCIDGraphDev::hicoInvalid;
    }
}


// ---------------------------------------------------------------------------
//  TRefIconHandle: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDGraphDev::TIconHandle TIcon::TRefIconHandle::hicoThis() const
{
    return m_hicoThis;
}

TSize TIcon::TRefIconHandle::szBitmap() const
{
    return TSize(m_c4Width, m_c4Height);
}


// ---------------------------------------------------------------------------
//  TRefIconHandle: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TIcon::TRefIconHandle::UpdateInfo()
{
    //
    //  Query the icon information, which will get us access to the bitmaps
    //  that map up the icon.
    //
    ICONINFO Info;
    if (!::GetIconInfo(m_hicoThis, &Info))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcIcon_QueryInfo
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    // If there is a color bitmap, then get that, else get the mask
    HBITMAP hbmpToUse = Info.hbmColor ? Info.hbmColor : Info.hbmMask;

    // Now query the bitmap info for the mask bitmap
    BITMAP BmpInfo;
    if (!::GetObject(hbmpToUse, sizeof(BITMAP), &BmpInfo))
    {
        // Free the bitmaps
        ::DeleteObject(Info.hbmColor);
        ::DeleteObject(Info.hbmMask);

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

    //
    //  Store the image size info. If it was from the mask bitmap, then half
    //  the height, else keep the whole thing.
    //
    m_c4Width = BmpInfo.bmWidth;

    if (Info.hbmColor)
        m_c4Height = BmpInfo.bmHeight;
    else
        m_c4Height = BmpInfo.bmHeight >> 1;

    // Free the bitmaps
    ::DeleteObject(Info.hbmColor);
    ::DeleteObject(Info.hbmMask);
}





// ---------------------------------------------------------------------------
//   CLASS: TIcon
//  PREFIX: bmp
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TIcon: Public, static methods
// ---------------------------------------------------------------------------
tCIDGraphDev::TIconHandle
TIcon::hicoCreate(  const   TFacility&                  facSrc
                    , const tCIDLib::TResId             ridToCreate
                    , const tCIDGraphDev::EIconSizes    eSize)
{
    // Find the memory of the resource indicated
    tCIDLib::TCard4         c4Len;
    const tCIDLib::TCard1*  pc1Start = facSrc.pc1FindRes
    (
        c4Len, ridToCreate, tCIDLib::EResTypes::Icon, kCIDLib::True
    );

    if (!pc1Start)
        return kCIDGraphDev::hicoInvalid;

    //
    //  Look for the one that is the requested size. If there is only one,
    //  then we take that one. If there isn't a match, we take the 0th
    //  one.
    //
    const TIconDir* pDir = reinterpret_cast<const TIconDir*>(pc1Start);
    tCIDLib::TCard4 c4Best;
    for (c4Best = 0; c4Best < pDir->c2Count; c4Best++)
    {
        if (eSize == tCIDGraphDev::EIconSizes::Small)
        {
            if (pDir->ideEntries[c4Best].c1Width == 16)
                break;
        }
         else if (eSize == tCIDGraphDev::EIconSizes::Normal)
        {
            if (pDir->ideEntries[c4Best].c1Width == 32)
                break;
        }
         else if (eSize == tCIDGraphDev::EIconSizes::Large)
        {
            if (pDir->ideEntries[c4Best].c1Width == 48)
                break;
        }
        #if CID_DEBUG_ON
         else
        {
            facCIDGraphDev().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcIcon_UnknownSize
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Internal
                , TInteger(tCIDLib::i4EnumOrd(eSize))
            );
        }
        #endif
    }

    // If we didn't find an exact match, then take the zeroth one
    if (c4Best == pDir->c2Count)
        c4Best = 0;

    // Load up the best
    tCIDGraphDev::TIconHandle hicoRes = ::CreateIconFromResourceEx
    (
        (unsigned char*)(pc1Start + pDir->ideEntries[c4Best].c4ImageOffset)
        , c4Len
        , 1
        , 0x30000
        , 0
        , 0
        , LR_DEFAULTCOLOR
    );
    if (!hicoRes)
        return kCIDGraphDev::hicoInvalid;

    return hicoRes;
}


tCIDGraphDev::TIconHandle
TIcon::hicoLoadRaw( const   TString&        strLibName
                    , const tCIDLib::TResId ridToLoad
                    , const TSize&          szPreferred
                    , const tCIDLib::TCard4 c4PrefDepth)
{
    return hicoLoadRaw
    (
        strLibName.pszBuffer()
        , MAKEINTRESOURCE(ridToLoad)
        , szPreferred
        , c4PrefDepth
    );
}

tCIDGraphDev::TIconHandle
TIcon::hicoLoadRaw( const   TString&        strLibName
                    , const TString&        strId
                    , const TSize&          szPreferred
                    , const tCIDLib::TCard4 c4PrefDepth)
{
    return hicoLoadRaw
    (
        strLibName.pszBuffer()
        , strId.pszBuffer()
        , szPreferred
        , c4PrefDepth
    );
}

tCIDGraphDev::TIconHandle
TIcon::hicoLoadRaw( const   tCIDLib::TCh* const pszLibName
                    , const tCIDLib::TCh* const pszId
                    , const TSize&              szPreferred
                    , const tCIDLib::TCard4     c4PrefDepth)
{
    // Load the library without any executtion, just to look up resources
    HINSTANCE hMod = ::LoadLibraryEx(pszLibName, 0, LOAD_LIBRARY_AS_DATAFILE);
    if (!hMod)
        return kCIDGraphDev::hicoInvalid;

    // We'll break out (if successful) with the single icon resource id
    tCIDLib::TResId ridFind = kCIDLib::c4MaxCard;
    tCIDLib::TCard4 c4Size = 0;

    // Try to find the resource id in this module
    HRSRC hRes = ::FindResource(hMod, pszId, RT_GROUP_ICON);
    if (hRes)
    {
        HGLOBAL hMem = ::LoadResource(hMod, hRes);
        if (hMem)
        {
            const tCIDLib::TCard1* pc1Res
            (
                (const tCIDLib::TCard1*)::LockResource(hMem)
            );

            if (pc1Res)
            {
                const tCIDLib::TCard4 c4Height = szPreferred.c4Height();
                const tCIDLib::TCard4 c4Width = szPreferred.c4Width();

                //
                //  Ok, we got it, so get the overall header, which will tell us
                //  how many individual icons we have to work with.
                //
                NEWHEADER* pIcoHdr = (NEWHEADER*)(pc1Res);

                //
                //  The header indicates how many versions of this icon we
                //  have following the header.
                //
                RESDIR* pRes = (RESDIR*)(pc1Res + sizeof(NEWHEADER));
                for (tCIDLib::TCard4 c4Index = 0; c4Index < pIcoHdr->ResCount; c4Index++)
                {
                    // If we find the one we want, break out
                    if ((pRes->BitCount == c4PrefDepth)
                    &&  (pRes->ResInfo.Icon.Width == c4Width)
                    &&  (pRes->ResInfo.Icon.Height == c4Height))
                    {
                        ridFind = pRes->IconCursorId;
                        c4Size = pRes->BytesInRes;
                        break;
                    }

                    // Move up to the next one
                    pRes++;
                }
            }
        }
    }

    tCIDGraphDev::TIconHandle hicoRet = kCIDGraphDev::hicoInvalid;

    // If we found an id, then look up the icon info
    if (ridFind != kCIDLib::c4MaxCard)
    {
        hRes = ::FindResource(hMod, MAKEINTRESOURCE(ridFind), RT_ICON);
        if (hRes != nullptr)
        {
            HGLOBAL hMem = ::LoadResource(hMod, hRes);
            if (hMem)
            {
                const tCIDLib::TCard1* pc1Res
                (
                    (const tCIDLib::TCard1*)::LockResource(hMem)
                );

                if (pc1Res != nullptr)
                {
                    // We got it so let's create the icon from this data
                    hicoRet = ::CreateIconFromResourceEx
                    (
                        (PBYTE)pc1Res, c4Size, 1, 0x00030000, 0, 0, 0
                    );
                }
            }
        }
    }

    ::FreeLibrary(hMod);
    return hicoRet;
}


// ---------------------------------------------------------------------------
//  TIcon: Constructors and Destructor
// ---------------------------------------------------------------------------
TIcon::TIcon()
{
    // Load a default system icon
    tCIDGraphDev::TIconHandle hicoTmp = ::LoadIcon(0, IDI_APPLICATION);
    if (hicoTmp == kCIDGraphDev::hicoInvalid)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcIcon_LoadSysIcon
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"default app")
        );
    }

    // Looks ok so store it on the counted pointer
    m_cptrHandle.SetPointer(new TRefIconHandle(hicoTmp));
}

TIcon::TIcon(const tCIDGraphDev::TIconHandle hicoToAdopt)
{
    m_cptrHandle.SetPointer(new TRefIconHandle(hicoToAdopt));
}

TIcon::TIcon(const TFacility& facSource, const tCIDLib::TResId ridIcon)
{
    //
    //  We use the passed module object to load up the resource based
    //  icon and get its handle.
    //
    tCIDGraphDev::TIconHandle hicoTmp = hicoCreate(facSource, ridIcon);
    if (hicoTmp == kCIDGraphDev::hicoInvalid)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcIcon_LoadFromModule
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(ridIcon)
            , facSource.strName()
        );
    }

    // Looks ok so store it on the counted pointer
    m_cptrHandle.SetPointer(new TRefIconHandle(hicoTmp));
}

TIcon::TIcon(const TString& strIconFile)
{
    // Ask the system to load up the icon and get us a handle.
    tCIDGraphDev::TIconHandle hicoRes = ::ExtractIconW(0, strIconFile.pszBuffer(), 0);

    if (!hicoRes || (hicoRes == tCIDGraphDev::TIconHandle(1)))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcIcon_LoadFromFile
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strIconFile
        );
    }

    // Looks ok so store it on the counted pointer
    m_cptrHandle.SetPointer(new TRefIconHandle(hicoRes));
}

TIcon::TIcon(const  TString&        strIconFile
            , const TSize&          szPreferredSz
            , const tCIDLib::TCard4 c4PrefDepth)
{
    TBinaryFile flSrc(strIconFile);
    flSrc.Open
    (
        tCIDLib::EAccessModes::Read
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
    );

    //
    //  Read in the header so that we know how much to read in order to
    //  get all the lookup table.
    //
    NEWHEADER FlHdr;
    flSrc.c4ReadBuffer(&FlHdr, sizeof(NEWHEADER), tCIDLib::EAllData::FailIfNotAll);

    // Now read in the table
    const tCIDLib::TCard4 c4TblSz = (sizeof(FRESDIR) * FlHdr.ResCount);
    tCIDLib::TCard1* pc1Table = new tCIDLib::TCard1[c4TblSz];
    TArrayJanitor<tCIDLib::TCard1> janTable(pc1Table);
    flSrc.c4ReadBuffer(pc1Table, c4TblSz, tCIDLib::EAllData::FailIfNotAll);

    const tCIDLib::TCard4 c4Height = szPreferredSz.c4Height();
    const tCIDLib::TCard4 c4Width = szPreferredSz.c4Width();
    tCIDLib::TCard4 c4Index = 0;

    // The table starts just after the header
    FRESDIR* pRes = (FRESDIR*)pc1Table;
    for (; c4Index < FlHdr.ResCount; c4Index++)
    {
        // If we find the one we want, break out
        if ((pRes->BitCount == c4PrefDepth)
        &&  (pRes->ResInfo.Icon.Width == c4Width)
        &&  (pRes->ResInfo.Icon.Height == c4Height))
        {
            break;
        }

        // Move up to the next one
        pRes++;
    }

    // If didn't find the preferred one, take the first one
    if (c4Index == FlHdr.ResCount)
    {
        c4Index = 0;
        pRes = (FRESDIR*)pc1Table;
    }

    flSrc.SetFilePos(pRes->FileOfs);
    TSysBuf mbufData(pRes->BytesInRes, pRes->BytesInRes);
    flSrc.c4ReadBuffer(mbufData, pRes->BytesInRes, tCIDLib::EAllData::FailIfNotAll);

    // Now we can try to create an icon from that data
    HICON hicoRes = ::CreateIconFromResourceEx
    (
        (PBYTE)mbufData.pc1Data(), pRes->BytesInRes, 1, 0x00030000, 0, 0, 0
    );

    // If it failed, we bail
    if (!hicoRes)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcIcon_LoadFromFile
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strIconFile
        );
    }

    // Looks ok so store it on the counted pointer
    m_cptrHandle.SetPointer(new TRefIconHandle(hicoRes));
}


TIcon::TIcon(   const   TPoint&     pntHotspot
                , const TBitmap&    bmpMask
                , const TBitmap&    bmpColor)
{
    ICONINFO Info;
    Info.fIcon = 0;
    Info.xHotspot = pntHotspot.i4X();
    Info.yHotspot = pntHotspot.i4Y();
    Info.hbmMask  = bmpMask.hbmpThis();
    Info.hbmColor = bmpColor.hbmpThis();

    tCIDGraphDev::TIconHandle hicoRes = ::CreateIconIndirect(&Info);
    if (hicoRes == kCIDGraphDev::hicoInvalid)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcIcon_CreateFromBmp
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Looks ok so store it on the counted pointer
    m_cptrHandle.SetPointer(new TRefIconHandle(hicoRes));
}


//
//  This guy takes an image object and creates an icon from it. It either
//  must be true color with a transparency color, or be true-alpha based,
//  in which case the apha channel is used to make the mask.
//
TIcon::TIcon(const TCIDImage& imgSrc, const TPoint& pntHotspot)
{
    if ((imgSrc.ePixFmt() != tCIDImage::EPixFmts::TrueClr)
    &&  (imgSrc.ePixFmt() != tCIDImage::EPixFmts::TrueAlpha))
    {
        facCIDGraphDev().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcIcon_BadSrcFmt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    //
    //  Let's create a mask based on the alpha channel of the image. Anything
    //  that isn't 255 in the mask, we assume should be transparent in the
    //  masked icon version of the image.
    //
    //  So create a monochrome pixel array the same size as the original
    //  image.
    //
    TPixelArray pixaMask
    (
        tCIDImage::EPixFmts::GrayScale
        , tCIDImage::EBitDepths::One
        , imgSrc.eRowOrder()
        , imgSrc.szImage()
    );

    const TPixelArray& pixaImg = imgSrc.pixaBits();
    const tCIDLib::TCard4 c4Height = pixaImg.c4Height();
    const tCIDLib::TCard4 c4Width = pixaImg.c4Width();
    if (imgSrc.ePixFmt() == tCIDImage::EPixFmts::TrueAlpha)
    {
        tCIDLib::TCard4 c4Cur;
        tCIDLib::TCodedRGB* prgbCur = (tCIDLib::TCodedRGB*)&c4Cur;

        // Set the mask pixel for each highly opaque src pixel
        for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4Height; c4YInd++)
        {
            for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4Width; c4XInd++)
            {
                c4Cur = pixaImg.c4At(c4XInd,c4YInd);
                if (prgbCur->c1Alpha == 0xFF)
                    pixaMask.PutAt(1, c4XInd, c4YInd);
            }
        }
    }
     else
    {
        //
        //  If a transparency color, set the mask pixel where the trans color
        //  doesn't match, i.e. opaque colors.
        //
        if (imgSrc.bTransClr())
        {
            const tCIDLib::TCard4 c4Trans = imgSrc.c4TransClr();
            for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4Height; c4YInd++)
            {
                for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4Width; c4XInd++)
                {
                    const tCIDLib::TCard4 c4Cur = pixaImg.c4At(c4XInd,c4YInd);
                    if (c4Cur != c4Trans)
                        pixaMask.PutAt(1, c4XInd, c4YInd);
                }
            }
        }
         else
        {
            for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4Height; c4YInd++)
            {
                for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4Width; c4XInd++)
                    pixaMask.PutAt(1, c4XInd, c4YInd);
            }
        }
    }

    // Create a compatible device, based on the desktop
    TIntGraphWndDev gdevCompat;

    // Create a bitmap for the mask we just made
    TClrPalette palMask(2);
    palMask[0].Set(0, 0, 0);
    palMask[1].Set(0xFF, 0xFF, 0xFF);
    TBitmap bmpMask(gdevCompat, pixaMask, palMask);

    // Create the main color image now
    TBitmap bmpClr(gdevCompat, imgSrc);

    // And set ourselves based on the image and mask
    ICONINFO Info;
    Info.fIcon = 0;
    Info.xHotspot = pntHotspot.i4X();
    Info.yHotspot = pntHotspot.i4Y();
    Info.hbmMask  = bmpMask.hbmpThis();
    Info.hbmColor = bmpClr.hbmpThis();

    tCIDGraphDev::TIconHandle hicoRes = ::CreateIconIndirect(&Info);
    if (hicoRes == kCIDGraphDev::hicoInvalid)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcIcon_CreateFromBmp
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Looks ok so store it on the counted pointer
    m_cptrHandle.SetPointer(new TRefIconHandle(hicoRes));
}


TIcon::TIcon(const TIcon& icoToCopy) :

    m_cptrHandle(icoToCopy.m_cptrHandle)
{
}

TIcon::~TIcon()
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
//  TIcon: Public operators
// ---------------------------------------------------------------------------
TIcon& TIcon::operator=(const TIcon& icoToAssign)
{
    if (this == &icoToAssign)
        return *this;

    // Just assign the counted pointer, which will do the right thing
    m_cptrHandle = icoToAssign.m_cptrHandle;
    return *this;
}


// ---------------------------------------------------------------------------
//  TIcon: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TIcon::DrawOn(       TGraphDrawDev&  gdevTarget
                            , const TPoint&         pntOrg) const
{
    gdevTarget.DrawIcon(*this, pntOrg);
}


tCIDLib::TVoid TIcon::DrawOn(       TGraphDrawDev&  gdevTarget
                            , const TArea&          areaToFill) const
{
    gdevTarget.DrawIcon(*this, areaToFill);
}


// ---------------------------------------------------------------------------
//  TIcon: Public, non-virtual methods
// ---------------------------------------------------------------------------
TBitmap TIcon::bmpImageCopy()
{
    const TRefIconHandle* prihTmp = m_cptrHandle.pobjData();

    ICONINFO Info;
    if (!::GetIconInfo(prihTmp->hicoThis(), &Info))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcIcon_QueryInfo
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    //
    //  If there is a color bitmap, then that's our guy, so just return it
    //  now. Otherwise, we have to do more work.
    //
    if (Info.hbmColor)
    {
        //
        //  Return a bitmap object with it. We tell it not to adopt, so it will
        //  make a copy of the passed handle.
        //
        return TBitmap(Info.hbmColor, tCIDLib::EAdoptOpts::NoAdopt);
    }


    //
    //  Its a monochrone icon, so the whole thing is in the mask, which is
    //  double height. So we've got to get the image part out, which is in
    //  the lower half.
    //
    const TSize& szFull = prihTmp->szBitmap();
    TSize szImage(szFull.c4Width(), szFull.c4Height() / 2);

    // Create a compatible device
    TIntGraphWndDev gdevCompat;

    //
    //  Create a memory device which has the mask bitmap selected into it as
    //  the surface. Note that this is going to copy the mask bitmap! But
    //  icons aren't going to tend to be big.
    //
    TBitmap bmpSrc(Info.hbmMask, tCIDLib::EAdoptOpts::NoAdopt);
    TGraphMemDev gdevSrc(gdevCompat, bmpSrc);

    // And create a blank one of half the height
    TBitmap bmpRet(gdevCompat, szImage);
    TGraphMemDev gdevTarget(gdevCompat, bmpRet);

    // Copy over the first half into the target
    TArea areaSrc(0, 0, szImage.c4Width(), szImage.c4Height());
    gdevTarget.CopyBits(gdevSrc, areaSrc, areaSrc);

    // Ok, the data is in the return bitmap
    return bmpRet;
}


TBitmap TIcon::bmpMaskCopy()
{
    const TRefIconHandle* prihTmp = m_cptrHandle.pobjData();

    ICONINFO Info;
    if (!::GetIconInfo(prihTmp->hicoThis(), &Info))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcIcon_QueryInfo
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    //
    //  If there is a color bitmap, then we can return the mask as is, since
    //  its a regular, single height mask.
    //
    if (Info.hbmColor)
    {
        //
        //  Return a bitmap object with it. We tell it not to adopt, so it will
        //  make a copy of the passed handle.
        //
        return TBitmap(Info.hbmColor, tCIDLib::EAdoptOpts::NoAdopt);
    }


    //
    //  Its a monochrone icon, so the whole thing is in the mask, which is
    //  double height. So we've got to get the mask part out, which is in
    //  the upper half.
    //
    const TSize& szFull = prihTmp->szBitmap();
    TSize szImage(szFull.c4Width(), szFull.c4Height() / 2);

    TIntGraphWndDev gdevCompat;

    //
    //  Create a memory device which has the mask bitmap selected into it as
    //  the surface. Note that this is going to copy the mask bitmap! But
    //  icons aren't going to tend to be big.
    //
    TBitmap bmpSrc(Info.hbmMask, tCIDLib::EAdoptOpts::NoAdopt);
    TGraphMemDev gdevSrc(gdevCompat, bmpSrc);

    // And create a blank one of half the height
    TBitmap bmpRet(gdevCompat, szImage);
    TGraphMemDev gdevTarget(gdevCompat, bmpRet);

    // Copy over the second half into the target
    TArea areaTarget(0, 0, szImage.c4Width(), szImage.c4Height());
    TArea areaSrc(areaTarget);
    areaSrc.Move(tCIDLib::EDirs::Down);
    gdevTarget.CopyBits(gdevSrc, areaSrc, areaSrc);

    // Ok, the data is in the return bitmap
    return bmpRet;
}


tCIDLib::TVoid TIcon::DeepCopy()
{
    const TRefIconHandle* prihTmp = m_cptrHandle.pobjData();

    // Try to make a copy of our icon
    tCIDGraphDev::TIconHandle hicoNew = ::CopyIcon(prihTmp->hicoThis());
    if (hicoNew == kCIDGraphDev::hicoInvalid)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcIcon_DupIcon
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Store a new counted handle
    m_cptrHandle.SetPointer(new TRefIconHandle(hicoNew));
}


tCIDImage::EBitDepths TIcon::eBitDepth() const
{
    const TRefIconHandle* prihTmp = m_cptrHandle.pobjData();
    ICONINFO Info;
    if (!::GetIconInfo(prihTmp->hicoThis(), &Info))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcIcon_QueryInfo
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    // If no color bitmap, its a monochrome icon
    if (!Info.hbmColor)
        return tCIDImage::EBitDepths::One;

    // Get the basic info we need to query the more complex info
    BITMAP BmpInfo = {0};
    if (!::GetObject(Info.hbmColor, sizeof(BITMAP), &BmpInfo))
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
    return tCIDImage::EBitDepths(BmpInfo.bmPlanes * BmpInfo.bmBitsPixel);
}


tCIDGraphDev::TIconHandle TIcon::hicoThis() const
{
    return m_cptrHandle.pobjData()->hicoThis();
}


TIcon TIcon::icoMakeGrayScale() const
{
    const TRefIconHandle* prihTmp = m_cptrHandle.pobjData();

    ICONINFO Info;
    if (!::GetIconInfo(prihTmp->hicoThis(), &Info))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcIcon_QueryInfo
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    // If this is a monochrome icon, then its just a copy operation
    if (!Info.hbmColor)
    {
        TIcon icoRet(*this);
        icoRet.DeepCopy();
        return icoRet;
    }

    //
    //  Its a color one, so we'll have to do a bit more work. We need to
    //  get copies of the color and mask bitmaps, then create a gray scale
    //  version of the color bitmap, and create an new icon again.
    //
    TBitmap bmpMask(Info.hbmMask, tCIDLib::EAdoptOpts::NoAdopt);
    TBitmap bmpColor(Info.hbmColor, tCIDLib::EAdoptOpts::NoAdopt);
    TBitmap bmpGS = bmpColor.bmpMakeGrayScale();

    return TIcon(TPoint(Info.xHotspot, Info.yHotspot), bmpMask, bmpGS);
}


TSize TIcon::szBitmap() const
{
    return m_cptrHandle.pobjData()->szBitmap();
}


tCIDLib::TVoid
TIcon::Set(const TFacility& facSource, const tCIDLib::TResId ridIcon)
{
    //
    //  We use the passed module object to load up the resource based
    //  icon and get its handle.
    //
    tCIDGraphDev::TIconHandle hicoTmp = hicoCreate(facSource, ridIcon);
    if (hicoTmp == kCIDGraphDev::hicoInvalid)
    {
        TKrnlError::SetLastHostError(::GetLastError());
        facCIDGraphDev().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kGrDevErrs::errcIcon_LoadFromModule
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(ridIcon)
            , facSource.strName()
        );
    }

    // Looks ok so store it on the counted pointer
    m_cptrHandle.SetPointer(new TRefIconHandle(hicoTmp));
}

