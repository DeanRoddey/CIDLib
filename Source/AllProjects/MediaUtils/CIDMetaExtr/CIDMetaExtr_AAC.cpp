//
// FILE NAME: CIDMetaExtr_AAC.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/24/2007
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
//  This file implements a metadata extractor for AAC/MPEG4 files.
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
#include    "CIDMetaExtr_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDMetaExtrAAC,TCIDMetaExtror)


// ---------------------------------------------------------------------------
//  Local types and data
// ---------------------------------------------------------------------------
namespace
{
    namespace CIDMetaExtr_AAC
    {
        // Constants for the various box types we care about
        constexpr tCIDLib::TCard4   c4Box_DATA = 0x64617461;
        constexpr tCIDLib::TCard4   c4Box_FTYP = 0x66747970;
        constexpr tCIDLib::TCard4   c4Box_ILST = 0x696C7374;
        constexpr tCIDLib::TCard4   c4Box_META = 0X6D657461;
        constexpr tCIDLib::TCard4   c4Box_MOOV = 0x6D6F6F76;
        constexpr tCIDLib::TCard4   c4Box_UDTA = 0x75647461;
        constexpr tCIDLib::TCard4   c4Box_MVHD = 0x6d766864;
        constexpr tCIDLib::TCard4   c4Box_TKHD = 0x746B6864;
        constexpr tCIDLib::TCard4   c4Box_TRAK = 0x7472616B;

        constexpr tCIDLib::TCard4   c4Box_ALB = 0xA9616C62;
        constexpr tCIDLib::TCard4   c4Box_ART = 0xA9617274;
        constexpr tCIDLib::TCard4   c4Box_GEN = 0xA967656E;

        constexpr tCIDLib::TCard4   c4Box_DAY = 0xA9646179;
        constexpr tCIDLib::TCard4   c4Box_NAM = 0xA96E616D;

        constexpr tCIDLib::TCard4   c4Box_AART = 0x61415254;
        constexpr tCIDLib::TCard4   c4Box_COVR = 0x636F7672;
        constexpr tCIDLib::TCard4   c4Box_GNRE = 0x676E7265;
        constexpr tCIDLib::TCard4   c4Box_TRKN = 0x74726B6E;
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TCIDMetaExtrAAC
// PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDMetaExtrAAC: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDMetaExtrAAC::TCIDMetaExtrAAC() :

    m_mbufData(1024, 0x1000000)
    , m_strTypeName(L"AAC/MPEG4")
{
}

TCIDMetaExtrAAC::~TCIDMetaExtrAAC()
{
}


// ---------------------------------------------------------------------------
//  TCIDMetaExtrAAC: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  We override this to provide the core metadata extraction functionality.
//  We run through the boxes to find the metadata box and then pull out all
//  of the sub-boxes and get the info out that we want.
//
tCIDLib::TBoolean
TCIDMetaExtrAAC::bExtract(  const   TString&            strFilePath
                            ,       TString&            strGenres
                            ,       TString&            strDescr
                            ,       TString&            strAlbum
                            ,       TString&            strAlbumArtist
                            ,       TString&            strArtist
                            ,       TString&            strTrack
                            ,       tCIDLib::TCard4&    c4TrackNum
                            ,       tCIDLib::TCard4&    c4Duration
                            ,       tCIDLib::TCard4&    c4Year
                            ,       tCIDLib::TCard4&    c4Bitrate
                            ,       tCIDLib::TCard4&    c4Rating
                            , const tCIDLib::TBoolean   bWantArt
                            ,       TMemBuf&            mbufArt
                            ,       tCIDLib::TCard4&    c4ArtSize)
{
    try
    {
        // Create an input stream over the passed file path
        TBinFileInStream strmSrc
        (
            strFilePath
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
            , tCIDLib::EAccessModes::Read
        );

        tCIDLib::TCard4 c4Tmp;
        tCIDLib::TCard4 c4Size;
        tCIDLib::TCard4 c4Type;

        // Default stuff in case we don't get them
        c4ArtSize = 0;
        c4Bitrate = 0;
        c4Duration = 0;
        c4Rating = 1;
        c4TrackNum = 0;
        c4Year = 0;
        strGenres.Clear();
        strDescr.Clear();
        strAlbum.Clear();
        strAlbumArtist.Clear();
        strArtist.Clear();
        strTrack.Clear();

        // Store the file name for use in error messages
        m_strSrcName = strFilePath;


        //
        //  Search for the start of the metadata. If we don't find it, then
        //  return false, else fall through and iterate them It will also return
        //  us the movie header block if it finds it while doing this.
        //
        THeapBuf mbufMHdr(128, 512);
        tCIDLib::TCard4 c4MHdrSz = 0;
        tCIDLib::TCard4 c4ILSTEnd;
        if (!bSkipToAttrs(strmSrc, c4ILSTEnd, mbufMHdr, c4MHdrSz))
            return kCIDLib::False;

        // If we got the movie header, lets calculate the duration
        if (c4MHdrSz)
        {
            // Get an input stream and skip one 32 bit value
            TBinMBufInStream strmMHdr(&mbufMHdr, c4MHdrSz);
            strmMHdr.SkipForwardBy(4);

            //
            //  Get out the next 32 which has a version and some flags. WE make sure
            //  that the version agrees with the size (which in our case is minus the
            //  8 bytes of the header.
            //
            tCIDLib::TCard4 c4MHVer = 0;
            const tCIDLib::TCard4 c4TestVer = c4Extract(strmMHdr) >> 24;
            if (c4TestVer == 1)
            {
                if (c4MHdrSz == 100)
                    c4MHVer = 1;
            }
             else if (c4TestVer)
            {
                c4MHVer = 2;
            }

            if (c4MHVer)
            {
                if (c4MHVer == 1)
                    strmMHdr.SkipForwardBy(16);
                else
                    strmMHdr.SkipForwardBy(8);

                tCIDLib::TCard4 c4TimeScale = c4Extract(strmMHdr);
                tCIDLib::TCard8 c8Duration;

                if (c4MHVer == 1)
                    c8Duration = c8Extract(strmMHdr);
                else
                    c8Duration = c4Extract(strmMHdr);

                if (c8Duration && c4TimeScale && (c8Duration > tCIDLib::TCard8(c4TimeScale)))
                    c4Duration = tCIDLib::TCard4(c8Duration / c4TimeScale);
            }
        }

        while (strmSrc.c4CurPos() < c4ILSTEnd)
        {
            tCIDLib::TCard4 c4BoxEnd;
            if (!bNextBox(strmSrc, c4Type, c4Size, c4BoxEnd))
                break;

            switch(c4Type)
            {
                case CIDMetaExtr_AAC::c4Box_ALB :
                    GetTextAttr(strmSrc, strAlbum, c4Size);
                    break;

                case CIDMetaExtr_AAC::c4Box_ART :
                    GetTextAttr(strmSrc, strArtist, c4Size);
                    break;

                case CIDMetaExtr_AAC::c4Box_AART :
                    GetTextAttr(strmSrc, strAlbumArtist, c4Size);
                    break;

                case CIDMetaExtr_AAC::c4Box_COVR :
                    // They may not want the cover art
                    if (bWantArt)
                        c4ArtSize = c4GetImageAttr(strmSrc, mbufArt, c4Size);
                    else
                        strmSrc.SkipForwardBy(c4Size - 8);
                    break;

                case CIDMetaExtr_AAC::c4Box_DAY :
                    GetTextAttr(strmSrc, m_strTmp, c4Size);
                    m_strTmp.CapAt(4);
                    m_strTmp.bToCard4(c4Year, tCIDLib::ERadices::Dec);
                    break;

                case CIDMetaExtr_AAC::c4Box_GEN :
                    GetTextAttr(strmSrc, m_strTmp, c4Size);
                    if (!strGenres.bIsEmpty())
                        strGenres.Append(L", ");
                    strGenres.Append(m_strTmp);
                    break;

                case CIDMetaExtr_AAC::c4Box_GNRE :
                    c4Tmp = c4GetUnsignedAttr(strmSrc, c4Size);
                    if (c4Tmp < kCIDMetaExtr::c4ID3GenreCnt)
                    {
                        if (!strGenres.bIsEmpty())
                            strGenres.Append(L", ");
                        strGenres.Append(kCIDMetaExtr::apszID3Genres[c4Tmp]);
                    }
                    break;

                case CIDMetaExtr_AAC::c4Box_NAM :
                    GetTextAttr(strmSrc, strTrack, c4Size);
                    break;

                case CIDMetaExtr_AAC::c4Box_TRKN :
                    //
                    //  If it's 4 bytes, it's a track number. If 8, it's a track number plus
                    //  total tracks. Either way, we just want the first card4.
                    //
                    if (c4Size == 32)
                        Get2UnsignedAttrs(strmSrc, c4Size, c4TrackNum, c4Tmp);
                    else if (c4Size == 24)
                        c4TrackNum = c4GetUnsignedAttr(strmSrc, c4Size);

                    // If some stupid value, ignore it
                    if (c4TrackNum > 128)
                        c4TrackNum = 0;
                    break;

                default :
                    // Don't care about this one, just skip forward
                    strmSrc.SkipForwardBy(c4Size - 8);
                    break;
            };
        }

        // If the artist is empty, but album artist is not, then use that
        if (strArtist.bIsEmpty() && !strAlbumArtist.bIsEmpty())
            strArtist = strAlbumArtist;
    }

    catch(TError& errToCatch)
    {
        if (facCIDMetaExtr().bShouldLog(errToCatch))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Just extracts an image if present
tCIDLib::TBoolean
TCIDMetaExtrAAC::bExtractImage( const   TString&            strFilePath
                                ,       TMemBuf&            mbufArt
                                ,       tCIDLib::TCard4&    c4ArtSize)
{
    c4ArtSize = 0;
    try
    {
        // Create an input stream over the passed file path
        TBinFileInStream strmSrc
        (
            strFilePath
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
            , tCIDLib::EAccessModes::Read
        );

        tCIDLib::TCard4 c4Size;
        tCIDLib::TCard4 c4Type;

        // Store the file name for use in error messages
        m_strSrcName = strFilePath;

        // Skip to the attributes and then run through them looking for the art
        tCIDLib::TCard4 c4ILSTEnd;
        THeapBuf mbufMHdr(128, 512);
        tCIDLib::TCard4 c4MHdrSz;
        if (bSkipToAttrs(strmSrc, c4ILSTEnd, mbufMHdr, c4MHdrSz))
        {
            while (strmSrc.c4CurPos() < c4ILSTEnd)
            {
                tCIDLib::TCard4 c4BoxEnd;
                if (!bNextBox(strmSrc, c4Type, c4Size, c4BoxEnd))
                    break;

                if (c4Type == CIDMetaExtr_AAC::c4Box_COVR)
                {
                    c4ArtSize = c4GetImageAttr(strmSrc, mbufArt, c4Size);
                    break;
                }
                strmSrc.SkipForwardBy(c4Size - 8);
            }
        }
    }

    catch(TError& errToCatch)
    {
        if (facCIDMetaExtr().bShouldLog(errToCatch))
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }
    return (c4ArtSize != 0);
}


//
//  We override this to provide AAC oriented probing. We just check to see
//  if it starts with the exected initial 8 bytes that a MPEG4 file should.
//
tCIDLib::TBoolean TCIDMetaExtrAAC::bProbe(const TString& strFilePath)
{
    // Create an input file over the input file
    TBinFileInStream strmSrc
    (
        strFilePath
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Read
    );

    tCIDLib::TCard4 c4End;
    tCIDLib::TCard4 c4Size;
    tCIDLib::TCard4 c4Type;

    try
    {
        if (!bNextBox(strmSrc, c4Type, c4Size, c4End))
            return kCIDLib::False;
    }

    catch(...)
    {
        //
        //  If it fails, then just say no. It would mean either it's not
        //  even 8 bytes in size, or it's not readable or something like
        //  that.
        //
        return kCIDLib::False;
    }
    return ((c4Size == 0x20) && (c4Type == CIDMetaExtr_AAC::c4Box_FTYP));
}


// Just returns the name of the file type we handle
const TString& TCIDMetaExtrAAC::strTypeName() const
{
    return m_strTypeName;
}


// ---------------------------------------------------------------------------
//  TCIDMetaExtrAAC: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Pulls out the next box. Assumes that we left off a the end of the previous
//  one, so we should be on the first byte of the next one.
//
tCIDLib::TBoolean
TCIDMetaExtrAAC::bNextBox(  TBinInStream&       strmSrc
                            , tCIDLib::TCard4&  c4Type
                            , tCIDLib::TCard4&  c4Size
                            , tCIDLib::TCard4&  c4End)
{
    if (strmSrc.bEndOfStream())
        return kCIDLib::False;

    c4Size = c4Extract(strmSrc);
    c4Type = c4Extract(strmSrc);

#if CID_DEBUG_ON
#define XLATTHETAGS 1
#if XLATTHETAGS
TString strTmp;
strTmp.Append(tCIDLib::TCh((c4Type >> 24) & 0xFF));
strTmp.Append(tCIDLib::TCh((c4Type >> 16) & 0xFF));
strTmp.Append(tCIDLib::TCh((c4Type >> 8) & 0xFF));
strTmp.Append(tCIDLib::TCh(c4Type & 0xFF));
#endif
#endif

    // The size cannot be less than 8
    if (c4Size < 8)
    {
        facCIDMetaExtr().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMExtrErrs::errcFmt_InvalidBoxSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c4Size)
        );
    }

    // Calculate the end point. We just ate 8 bytes of it, so remove that
    c4End = strmSrc.c4CurPos() + (c4Size - 8);

    return kCIDLib::True;
}


//
//  This is called to skip over (and verify) the goop that gets us down to
//  where the metadata attributes begin.
//
tCIDLib::TBoolean
TCIDMetaExtrAAC::bSkipToAttrs(  TBinInStream&       strmSrc
                                , tCIDLib::TCard4&  c4ILSTEnd
                                , TMemBuf&          mbufMHdr
                                , tCIDLib::TCard4&  c4MHdrSz)
{
    // Insure this is zero if we don't find the movie header atom
    c4MHdrSz = 0;

    tCIDLib::TCard4 c4Tmp;
    tCIDLib::TCard4 c4Size;
    tCIDLib::TCard4 c4Type;

    //
    //  Make sure it's actually a file of the type we support. We should
    //  see the file type box of the correct size.
    //
    if (!bNextBox(strmSrc, c4Type, c4Size, c4Tmp)
    ||  (c4Type != CIDMetaExtr_AAC::c4Box_FTYP)
    ||  (c4Size != 0x20))
    {
        ThrowNotOfType();
    }
    strmSrc.SkipForwardBy(c4Size - 8);

    //
    //  Search forward for the moov block. It contains the information we
    //  are interested in.
    //
    tCIDLib::TCard4 c4MOOVEnd;
    tCIDLib::TBoolean bRes = kCIDLib::False;
    while (bNextBox(strmSrc, c4Type, c4Size, c4MOOVEnd))
    {
        if (c4Type == CIDMetaExtr_AAC::c4Box_MOOV)
        {
            bRes = kCIDLib::True;
            break;
        }
        strmSrc.SkipForwardBy(c4Size - 8);
    }

    //
    //  If we found MOOV, then we need to search within it for the user data
    //  box.
    //
    tCIDLib::TCard4 c4UDTAEnd;
    if (bRes)
    {
        bRes = kCIDLib::False;
        while ((strmSrc.c4CurPos() < c4MOOVEnd)
        &&     bNextBox(strmSrc, c4Type, c4Size, c4UDTAEnd))
        {
            if (c4Type == CIDMetaExtr_AAC::c4Box_UDTA)
            {
                bRes = kCIDLib::True;
                break;
            }
             else if (c4Type == CIDMetaExtr_AAC::c4Box_MVHD)
            {
                // Return this data to the caller
                strmSrc.c4ReadBuffer(mbufMHdr, c4Size - 8);
                c4MHdrSz = c4Size - 8;
            }
             else
            {
                strmSrc.SkipForwardBy(c4Size - 8);
            }
        }
    }

    // If we found UDTA, then now find the META box
    tCIDLib::TCard4 c4METAEnd;
    if (bRes)
    {
        bRes = kCIDLib::False;
        while ((strmSrc.c4CurPos() < c4UDTAEnd)
        &&     bNextBox(strmSrc, c4Type, c4Size, c4METAEnd))
        {
            if (c4Type == CIDMetaExtr_AAC::c4Box_META)
            {
                //
                //  We found it. Note that the META box is inconsistent with
                //  the format. It contains first some data, then nested
                //  child boxes. So skip a flag word, then get the next word
                //  which is a data size, and skip over that data.
                //
                strmSrc.SkipForwardBy(4);
                c4Tmp = c4Extract(strmSrc);
                strmSrc.SkipForwardBy(c4Tmp - 4);

                bRes = kCIDLib::True;
                break;
            }
            strmSrc.SkipForwardBy(c4Size - 8);
        }
    }

    // If we found META, then now find the ILST box
    if (bRes)
    {
        bRes = kCIDLib::False;
        while ((strmSrc.c4CurPos() < c4METAEnd)
        &&     bNextBox(strmSrc, c4Type, c4Size, c4ILSTEnd))
        {
            if (c4Type == CIDMetaExtr_AAC::c4Box_ILST)
            {
                //
                //  Nothing to skip, the child tags start directly after this,
                //  being the actual content of this tag.
                //
                bRes = kCIDLib::True;
                break;
            }
            strmSrc.SkipForwardBy(c4Size - 8);
        }
    }
    return kCIDLib::True;
}


//
//  This is called when an image attr is seen. We pull out the data, probe
//  it to see if it's a JPEG or PNG. If a PNG, we convert to JPEG. We return
//  the raw data in a buffer.
//
tCIDLib::TCard4
TCIDMetaExtrAAC::c4GetImageAttr(        TBinInStream&   strmSrc
                                ,       TMemBuf&        mbufToFill
                                , const tCIDLib::TCard4 c4DataSz)
{
    //
    //  If it's less than 40 bytes, then it just couldn't be a good
    //  image. We need 24 for the box and embedded box overhead, plus
    //  the image has to more than another 32.
    //
    if (c4DataSz < 40)
    {
        facCIDMetaExtr().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMExtrErrs::errcFmt_InvalidDataSize
            , m_strSrcName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c4DataSz)
        );
    }

    // Next should be a data child box
    tCIDLib::TCard4 c4DataEnd;
    tCIDLib::TCard4 c4Size;
    tCIDLib::TCard4 c4Type;
    if (!bNextBox(strmSrc, c4Type, c4Size, c4DataEnd)
    ||  (c4Type != CIDMetaExtr_AAC::c4Box_DATA))
    {
        facCIDMetaExtr().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMExtrErrs::errcFmt_ExpectedDataBox
            , m_strSrcName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Skip forward over the version, flags, and null space
    strmSrc.SkipForwardBy(8);

    // Pull in the data to the caller's buffer
    c4Size -= 16;
    strmSrc.c4ReadBuffer(mbufToFill, c4Size);

    //
    //  See what type it is. If it's not a JPEG, but it's a supported type,
    //  then we need to covert it. Else return what we have.
    //
    const tCIDImage::EImgTypes eType = facCIDImgFact().eProbeImg(mbufToFill, c4Size);
    if ((eType != tCIDImage::EImgTypes::JPEG)
    &&  (eType != tCIDImage::EImgTypes::Unknown))
    {
        try
        {
            //
            //  Use the image factory to decode it and convert to the desired
            //  JPEG format if not already.
            //
            TCIDImage* pimgTar;
            facCIDImgFact().bDecodeImgTo
            (
                mbufToFill
                , c4Size
                , tCIDImage::EImgTypes::JPEG
                , pimgTar
                , kCIDLib::True
            );
            TJanitor<TCIDImage> janImg(pimgTar);

            // Stream this temp image back out now
            {
                TBinMBufOutStream strmTar(&mbufToFill);
                strmTar << *pimgTar << kCIDLib::FlushIt;
                c4Size = strmTar.c4CurPos();
            }
        }

        catch(TError& errToCatch)
        {
            if (facCIDMetaExtr().bShouldLog(errToCatch))
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }

            if (facCIDMetaExtr().bLogFailures())
            {
                facCIDMetaExtr().LogMsg
                (
                    CID_FILE
                    , CID_LINE
                    , kMExtrErrs::errcFmt_ImageExtract
                    , m_strSrcName
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            }
            c4Size = 0;
        }
    }
    return c4Size;
}


//
//  This is called when an unsigned attr is seen. We pull out the data
//  and just return the value.
//
tCIDLib::TCard4
TCIDMetaExtrAAC::c4GetUnsignedAttr(         TBinInStream&   strmSrc
                                    , const tCIDLib::TCard4 c4DataSz)
{
    // It should be at least 17 bytes
    if (c4DataSz < 17)
    {
        facCIDMetaExtr().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMExtrErrs::errcFmt_InvalidDataSize
            , m_strSrcName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c4DataSz)
        );
    }

        // Next should be a data child box
    tCIDLib::TCard4 c4DataEnd;
    tCIDLib::TCard4 c4Size;
    tCIDLib::TCard4 c4Type;
    if (!bNextBox(strmSrc, c4Type, c4Size, c4DataEnd)
    ||  (c4Type != CIDMetaExtr_AAC::c4Box_DATA))
    {
        facCIDMetaExtr().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMExtrErrs::errcFmt_ExpectedDataBox
            , m_strSrcName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Skip forward over the version, flags, and null space
    strmSrc.SkipForwardBy(8);

    // And extract the value
    return c2Extract(strmSrc);
}


// This handles two 32 bit values stored in a single attribute
tCIDLib::TVoid
TCIDMetaExtrAAC::Get2UnsignedAttrs(         TBinInStream&       strmSrc
                                    , const tCIDLib::TCard4     c4DataSz
                                    ,       tCIDLib::TCard4&    c4Val1
                                    ,       tCIDLib::TCard4&    c4Val2)
{
    // It should be at least 16 bytes
    if (c4DataSz < 16)
    {
        facCIDMetaExtr().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMExtrErrs::errcFmt_InvalidDataSize
            , m_strSrcName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c4DataSz)
        );
    }

    // Next should be a data child box
    tCIDLib::TCard4 c4DataEnd;
    tCIDLib::TCard4 c4Size;
    tCIDLib::TCard4 c4Type;
    if (!bNextBox(strmSrc, c4Type, c4Size, c4DataEnd)
    ||  (c4Type != CIDMetaExtr_AAC::c4Box_DATA))
    {
        facCIDMetaExtr().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMExtrErrs::errcFmt_ExpectedDataBox
            , m_strSrcName
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Skip forward over the version, flags, and null space
    strmSrc.SkipForwardBy(8);

    // Now extract the two values
    c4Val1 = c4Extract(strmSrc);
    c4Val2 = c4Extract(strmSrc);
}



//
//  This is called when we get to a metadata attribute that is textual. We
//  read in the text and load it into the passed string. We leave the stream
//  on the first byte of the next box.
//
tCIDLib::TVoid
TCIDMetaExtrAAC::GetTextAttr(       TBinInStream&   strmSrc
                            ,       TString&        strToFill
                            , const tCIDLib::TCard4 c4DataSz)
{
    //
    //  It has to be at least 24 bytes, the 8 of the size/type, then
    //  another 16 for the type/size, and flags, nulls, of the embedded box.
    //
    if (c4DataSz < 24)
    {
        facCIDMetaExtr().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMExtrErrs::errcFmt_InvalidDataSize
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c4DataSz)
        );
    }

    // Next should be a data child box
    tCIDLib::TCard4 c4DataEnd;
    tCIDLib::TCard4 c4Size;
    tCIDLib::TCard4 c4Type;
    if (!bNextBox(strmSrc, c4Type, c4Size, c4DataEnd)
    ||  (c4Type != CIDMetaExtr_AAC::c4Box_DATA))
    {
        facCIDMetaExtr().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMExtrErrs::errcFmt_ExpectedDataBox
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Skip forward over the version, flags, and null space
    strmSrc.SkipForwardBy(8);

    // And now use the text converter to pull out the remaining bytes
    strmSrc.c4ReadBuffer(m_mbufData, c4Size - 16);

    m_tcvtText.c4ConvertFrom(m_mbufData.pc1Data(), c4Size - 16, strToFill);
}

