//
// FILE NAME: CIDMetaExtr_WMA.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/27/2007
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
//  This file implements a metadata extractor for WMA files.
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
//  Force inclusion of system libraries we need
// ---------------------------------------------------------------------------
#pragma     comment(lib, "Wmvcore.lib")


// ---------------------------------------------------------------------------
//  We need some windows stuff and some of our platform helpers
// ---------------------------------------------------------------------------
#include    <windows.h>
#include    <wmsdk.h>
#include    <vfw.h>
#include    "CIDKernel_InternalHelpers_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCIDMetaExtrWMA,TCIDMetaExtror)


// ---------------------------------------------------------------------------
//  CLASS: TReaderStatusHandler
// PREFIX: rsh
//
//  We need a class that implements the IWMStatusCallback class, and we can't
//  make our main class implement it because that would require public
//  exposure of the WMF headers. So we do it internally and it takes a pointer
//  to our main extractor class that it can call back on. A bit overly self-
//  referential but it works.
// ---------------------------------------------------------------------------
struct TReaderStatusHandler : public IWMStatusCallback
{
    TReaderStatusHandler(TCIDMetaExtrWMA* const pmetexOwner) :

        m_c4RefCnt(1)
        , m_pmetexOwner(pmetexOwner)
    {
    }

    ~TReaderStatusHandler()
    {
    }

    HRESULT __stdcall QueryInterface(REFIID riid, void **ppv)
    {
        if ((riid == IID_IUnknown)
        ||  (riid == IID_IWMStatusCallback))
        {
            *ppv = this;
            AddRef();
            return NOERROR;
        }
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    unsigned long __stdcall AddRef(void)
    {
        return m_c4RefCnt++;
    }

    unsigned long __stdcall Release(void)
    {
        if (m_c4RefCnt)
        {
            m_c4RefCnt--;
            return m_c4RefCnt;
        }
        delete this;
        return 0;
    }

    HRESULT __stdcall OnStatus( WMT_STATUS          Status
                                , HRESULT           hRes
                                , WMT_ATTR_DATATYPE dwType
                                , BYTE*             pValue
                                , void*             pvContext)
    {
        if (FAILED(hRes))
            m_pmetexOwner->StatusCallback(hRes);
        else if (Status == WMT_OPENED)
            m_pmetexOwner->StatusCallback(0);
        return 0;
    }

    tCIDLib::TCard4     m_c4RefCnt;
    TCIDMetaExtrWMA*    m_pmetexOwner;
};




// ---------------------------------------------------------------------------
//  Local helper functions
// ---------------------------------------------------------------------------
static tCIDLib::TVoid
CIDMetaExtr_XlatSysErr(tCIDLib::TCard4 c4Err, TString& strToFill)
{
    const tCIDLib::TCard4 c4BufSz = 1023;
    tCIDLib::TCh achBuf[c4BufSz + 1];

    achBuf[0] = kCIDLib::chNull;
    ::FormatMessage
    (
        FORMAT_MESSAGE_FROM_SYSTEM
        , NULL
        , c4Err
        , MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
        , achBuf
        , c4BufSz
        , NULL
    );
    achBuf[c4BufSz] = kCIDLib::chNull;

    //
    //  If it came out empty, then format the actual error code into the
    //  string.
    //
    strToFill = achBuf;
    strToFill.StripWhitespace();
    if (strToFill.bIsEmpty())
    {
        strToFill = L"WMA error: ";
        strToFill.AppendFormatted(c4Err, tCIDLib::ERadices::Hex);
    }
}


//
//  All of the COM APIs has to be checked for errors and the code is the
//  same for all of them here, so they just call this method and provide the
//  facility error and the underlying COM error.
//
static tCIDLib::TVoid
CIDMetaExtr_ThrowEncError(  const   tCIDLib::TErrCode   errcToThrow
                            , const tCIDLib::TCard4     c4HRes
                            , const tCIDLib::TCard4     c4Line)
{
    TString strSysErr;
    CIDMetaExtr_XlatSysErr(c4HRes, strSysErr);
    facCIDMetaExtr().ThrowErr
    (
        CID_FILE
        , c4Line
        , errcToThrow
        , strSysErr
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::CantDo
    );
}




// ---------------------------------------------------------------------------
//  CLASS: TCIDMetaExtrWMA
// PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDMetaExtrWMA: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDMetaExtrWMA::TCIDMetaExtrWMA() :

    m_strTypeName(L"WMA/MP3")
{
}

TCIDMetaExtrWMA::~TCIDMetaExtrWMA()
{
}


// ---------------------------------------------------------------------------
//  TCIDMetaExtrWMA: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  We override this to provide the core metadata extraction functionality.
//  We run through the boxes to find the metadata box and then pull out all
//  of the sub-boxes and get the info out that we want.
//
//  We can't really provide rating, so we just set it to 1 and leave it at
//  that.
//
tCIDLib::TBoolean
TCIDMetaExtrWMA::bExtract(  const   TString&            strFilePath
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
    // Assume no art until proven otherwise
    c4ArtSize = 0;

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

    // Store the source name for use in error messages
    m_strSrcName = strFilePath;

    // Create a reader object to do our work, we don't need any rights
    IWMReader* pReader = 0;
    HRESULT hRes = ::WMCreateReader(0, 0, &pReader);
    if (FAILED(hRes))
        CIDMetaExtr_ThrowEncError(kMExtrErrs::errcWMA_Reader, hRes, CID_LINE);
    TCOMJanitor<IWMReader> janReader(&pReader);

    //
    //  We have to asynchronously wait for the stream to open. Create an
    //  instance of our status callback object and point it at us, then
    //  then try the open and block for status.
    //
    m_c4Result = 0;
    m_evStatus.Reset();
    {
        TReaderStatusHandler* prshWait = new TReaderStatusHandler(this);
        TCOMJanitor<TReaderStatusHandler> janStatus(&prshWait);

        pReader->Open(strFilePath.pszBuffer(), (IWMReaderCallback*)prshWait, 0);
        m_evStatus.WaitFor(15000);
    }

    // If it failed, then give up now
    if (m_c4Result != 0)
        return kCIDLib::False;

    // Create the info interface we use to query the attributes
    IWMHeaderInfo3* pHdrInfo;
    hRes = pReader->QueryInterface(&pHdrInfo);
    if (FAILED(hRes))
        CIDMetaExtr_ThrowEncError(kMExtrErrs::errcWMA_HdrInfo, hRes, CID_LINE);
    TCOMJanitor<IWMHeaderInfo3> janHdrInfo(&pHdrInfo);

    // Ask now many attributes there are and iterate through them
    WORD AttrCnt;
    hRes = pHdrInfo->GetAttributeCountEx(0xFFFF, &AttrCnt);
    if (FAILED(hRes))
        CIDMetaExtr_ThrowEncError(kMExtrErrs::errcWMA_GetAttrCnt, hRes, CID_LINE);

    const tCIDLib::TCard2 c4BufSz = 2048;
    const tCIDLib::TCard2 c2NameSz = 1024;
    tCIDLib::TBoolean bOk;
    tCIDLib::TCard1 ac1Buf[c4BufSz];
    tCIDLib::TCh achName[c2NameSz];
    for (tCIDLib::TCard2 c2Index = 0; c2Index < AttrCnt; c2Index++)
    {
        tCIDLib::TCard4 c4DataLen = c4BufSz;
        tCIDLib::TCard2 c2NameLen = c2NameSz;
        tCIDLib::TCard2 c2Lang;
        WMT_ATTR_DATATYPE DType;
        hRes = pHdrInfo->GetAttributeByIndexEx
        (
            0xFFFF
            , c2Index
            , achName
            , &c2NameLen
            , &DType
            , &c2Lang
            , ac1Buf
            , &c4DataLen
        );

        if (!FAILED(hRes) && c2NameLen && c4DataLen)
        {
            // Get the data pointer. If it's an empty string, skip this one
            const tCIDLib::TCh* pszText = reinterpret_cast<const tCIDLib::TCh*>(ac1Buf);
            if (!pszText || !TRawStr::c4StrLen(pszText))
                continue;

            // If this one worked, then see if it's one we care about
            if (TRawStr::bCompareStr(achName, L"Artist"))
            {
                strArtist = pszText;
            }
             else if (TRawStr::bCompareStr(achName, L"Author"))
            {
                // If we don't get an artist, we'll take the Author tag
                if (strArtist.bIsEmpty())
                    strArtist = pszText;
            }
             else if (TRawStr::bCompareStr(achName, L"Bitrate"))
            {
                if (DType == WMT_TYPE_DWORD)
                    c4Bitrate = *reinterpret_cast<const tCIDLib::TCard4*>(ac1Buf);
            }
             else if (TRawStr::bCompareStr(achName, L"Duration"))
            {
                c4Duration = tCIDLib::TCard4
                (
                    *reinterpret_cast<tCIDLib::TCard8*>(ac1Buf)
                    / kCIDLib::enctOneSecond
                );
            }
             else if (TRawStr::bCompareStr(achName, L"WM/AlbumTitle"))
            {
                strAlbum = pszText;
            }
             else if (TRawStr::bCompareStr(achName, L"Title"))
            {
                strTrack = pszText;
            }
             else if (TRawStr::bCompareStr(achName, L"WM/AlbumArtist"))
            {
                strAlbumArtist = pszText;
            }
             else if (TRawStr::bCompareStr(achName, L"WM/Genre"))
            {
                if (!strGenres.bIsEmpty())
                    strGenres.Append(L", ");
                strGenres.Append(pszText);
            }
             else if (TRawStr::bCompareStr(achName, L"WM/TrackNumber")
                  ||  TRawStr::bCompareStr(achName, L"WM/Track"))
            {
                if (DType == WMT_TYPE_DWORD)
                {
                    c4TrackNum = *reinterpret_cast<const tCIDLib::TCard4*>(ac1Buf);
                }
                 else if (DType == WMT_TYPE_STRING)
                {
                    c4TrackNum = TRawStr::c4AsBinary
                    (
                        pszText, bOk, tCIDLib::ERadices::Dec
                    );
                }

                // It's one based and we want an index for internal use
                if (c4TrackNum)
                    c4TrackNum--;
            }
             else if (TRawStr::bCompareStr(achName, L"WM/Year"))
            {
                c4Year = TRawStr::c4AsBinary
                (
                    pszText, bOk, tCIDLib::ERadices::Dec
                );
            }
        }
    }

    //
    //  If they want art, then check for that. We don't care about the
    //  return. We return true at this point to indicate we didn't fail
    //  to get metadata of some sort.
    //
    if (bWantArt)
        bLoadImage(pHdrInfo, c4ArtSize, mbufArt);

    // Strip any leading/trailing white space we may have gotten
    strGenres.StripWhitespace();
    strDescr.StripWhitespace();
    strAlbum.StripWhitespace();
    strAlbumArtist.StripWhitespace();
    strArtist.StripWhitespace();
    strTrack.StripWhitespace();

    return kCIDLib::True;
}


//
//  This one just extracts cover art only, so instead of iteration it just
//  does a by name query for the attribute we want.
//
tCIDLib::TBoolean
TCIDMetaExtrWMA::bExtractImage( const   TString&            strFilePath
                                ,       TMemBuf&            mbufArt
                                ,       tCIDLib::TCard4&    c4ArtSize)
{
    // Assume no art until proven otherwise
    c4ArtSize = 0;

    // Store the source name for use in error messages
    m_strSrcName = strFilePath;

    // Create a reader object to do our work, we don't need any rights
    IWMReader* pReader = 0;
    HRESULT hRes = ::WMCreateReader(0, 0, &pReader);
    if (FAILED(hRes))
        CIDMetaExtr_ThrowEncError(kMExtrErrs::errcWMA_Reader, hRes, CID_LINE);
    TCOMJanitor<IWMReader> janReader(&pReader);

    //
    //  We have to asynchronously wait for the stream to open. Create an
    //  instance of our status callback object and point it at us, then
    //  then try the open.
    //
    m_c4Result = 0;
    m_evStatus.Reset();

    {
        TReaderStatusHandler* prshWait = new TReaderStatusHandler(this);
        TCOMJanitor<TReaderStatusHandler> janStatus(&prshWait);

        pReader->Open(strFilePath.pszBuffer(), (IWMReaderCallback*)prshWait, 0);
        m_evStatus.WaitFor(15000);
    }

    // If it failed, then give up now
    if (m_c4Result != 0)
        return kCIDLib::False;

    // Create the info interface we use to query the attributes
    IWMHeaderInfo3* pHdrInfo = 0;
    hRes = pReader->QueryInterface(&pHdrInfo);
    if (FAILED(hRes))
        CIDMetaExtr_ThrowEncError(kMExtrErrs::errcWMA_HdrInfo, hRes, CID_LINE);
    TCOMJanitor<IWMHeaderInfo3> janHdrInfo(&pHdrInfo);

    // And call the commaon image loader helper method
    return bLoadImage(pHdrInfo, c4ArtSize, mbufArt);
}


//
//  We override this to provide WMA oriented probing. We just probe the
//  file to see if it looks like it is really a WMA file.
//
tCIDLib::TBoolean TCIDMetaExtrWMA::bProbe(const TString& strFilePath)
{
    tCIDLib::TBoolean bRes = kCIDLib::False;
    try
    {
        // Create a reader object to do our work, we don't need any rights
        IWMReader* pReader = nullptr;
        HRESULT hRes = ::WMCreateReader(0, 0, &pReader);
        if (FAILED(hRes))
            CIDMetaExtr_ThrowEncError(kMExtrErrs::errcWMA_Reader, hRes, CID_LINE);
        TCOMJanitor<IWMReader> janCodecInfo(&pReader);

        //
        //  We have to asynchronously wait for the stream to open. Create an
        //  instance of our status callback object and point it at us, then
        //  then try the open.
        //
        m_c4Result = 0;
        m_evStatus.Reset();
        {
            TReaderStatusHandler* prshWait = new TReaderStatusHandler(this);
            TCOMJanitor<TReaderStatusHandler> janStatus(&prshWait);

            pReader->Open(strFilePath.pszBuffer(), (IWMReaderCallback*)prshWait, 0);
            m_evStatus.WaitFor(15000);
        }

        // If we didn't get an error, assume it's really an ASF file
        bRes = (m_c4Result == 0);
    }

    catch(...)
    {
        // If it fails, then just say no
        bRes = kCIDLib::False;
    }
    return bRes;
}


// Just returns the name of the file type we handle
const TString& TCIDMetaExtrWMA::strTypeName() const
{
    return m_strTypeName;
}


// ---------------------------------------------------------------------------
//  TCIDMetaExtrWMA: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TCIDMetaExtrWMA::bLoadImage(IWMHeaderInfo3* const   pHdrInfo
                            , tCIDLib::TCard4&      c4ArtSize
                            , TMemBuf&              mbufToFill)
{
    // Assume no art
    c4ArtSize = 0;

    // See if we have a picture attribute, and if so what size it is
    WMT_ATTR_DATATYPE AttrType;
    tCIDLib::TCard2 c2Stream = 0;
    tCIDLib::TCard2 c2AttrSz;
    HRESULT hRes = pHdrInfo->GetAttributeByName
    (
        &c2Stream, L"WM/Picture", &AttrType, 0, &c2AttrSz
    );

    // If it failed or returned zero for the size, say no image
    if (FAILED(hRes) || !c2AttrSz)
        return kCIDLib::False;

    // Double check that it returns the right type
    if (AttrType != WMT_TYPE_BINARY)
        return kCIDLib::False;

    // Allocate a buffer and put a janitor on it so it gets cleane dup
    tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[c2AttrSz];
    TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Buf);

    // And now call again to get the actual data
    hRes = pHdrInfo->GetAttributeByName
    (
        &c2Stream, L"WM/Picture", &AttrType, pc1Buf, &c2AttrSz
    );

    // Just in case, check the return again
    if (FAILED(hRes) || !c2AttrSz)
        return kCIDLib::False;

    // Cast the data to the picture structure
    WM_PICTURE* pPic = reinterpret_cast<WM_PICTURE*>(pc1Buf);

    // We only care about type 3, the front album cover
    if (pPic->bPictureType == 3)
    {
        //
        //  It's our guy. Check the mime type. If it's JPEG, take it as
        //  is. Else see if we can understand it and convert it to JPEG.
        //
        mbufToFill.CopyIn(pPic->pbData, pPic->dwDataLen);

        if (TRawStr::bCompareStrI(pPic->pwszMIMEType, L"image/jpeg"))
        {
            // Take it as is
            c4ArtSize = pPic->dwDataLen;
        }
         else
        {
            try
            {
                //
                //  Use the image factory to decode it and convert to the
                //  desired JPEG format.
                //
                TCIDImage* pimgTar;
                facCIDImgFact().bDecodeImgTo
                (
                    mbufToFill
                    , pPic->dwDataLen
                    , tCIDImage::EImgTypes::JPEG
                    , pimgTar
                    , kCIDLib::True
                );
                TJanitor<TCIDImage> janImg(pimgTar);

                // Stream this temp image back out now
                {
                    TBinMBufOutStream strmTar(&mbufToFill);
                    strmTar << *pimgTar << kCIDLib::FlushIt;
                    c4ArtSize = strmTar.c4CurPos();
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
                c4ArtSize = 0;
                return kCIDLib::False;
            }
        }
    }

    // Return true if we got any art data
    return (c4ArtSize != 0);
}


//
//  We have to wait for a callback to let us know that the reader has opened
//  the stream up successfully. We get a callback here from the callback object
//  we pass to the Open() method. It will store status info and trigger an
//  event that wakes up the waiting main thread.
//
//  BE CAREFUL here because we are not on a CIDLib thread! So just do the
//  minimum necessary.
//
tCIDLib::TVoid
TCIDMetaExtrWMA::StatusCallback(const tCIDLib::TCard4 c4SysErr)
{
    m_c4Result = c4SysErr;
    m_evStatus.Trigger();
}

