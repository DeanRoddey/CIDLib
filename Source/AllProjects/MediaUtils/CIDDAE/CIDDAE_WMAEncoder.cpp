//
// FILE NAME: CIDDAE_WMAEncoder.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2006
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
//  This file implements WMA encoder derivative of the base encoder class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Force inclusion of system libraries we need
// ---------------------------------------------------------------------------
#pragma     comment(lib, "ole32.lib")
#pragma     comment(lib, "Wmvcore.lib")


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDDAE_.hpp"



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
RTTIDecls(TCIDDAEWMADec,TCIDDAEDecoder)
RTTIDecls(TCIDDAEWMAEnc,TCIDDAEEncoder)


// ---------------------------------------------------------------------------
//  Local types and data
// ---------------------------------------------------------------------------
namespace CIDDAE_WMAEncoder
{
    // -----------------------------------------------------------------------
    //  We use an internal structure to hold the WMA specific info for the
    //  encoding process, so that we don't expose the Windows headers to the
    //  world. WE also have one for the decoding process.
    // -----------------------------------------------------------------------
    struct TWMAWInfo
    {
        WM_MEDIA_TYPE   MediaType;
        WAVEFORMATEX    WaveFmt;
        IWMWriter*      pWriter;
        IWMProfile*     pProfile;
        tCIDLib::TCard8 c8CurTime;
    };

    struct TWMARInfo
    {
        IWMSyncReader*  pReader;
        tCIDLib::TCard2 c2StreamNum;
    };


    // -----------------------------------------------------------------------
    //  We fault in a single profile manager and just keep using it
    // -----------------------------------------------------------------------
    IWMProfileManager*  pProfMgr = 0;


    // -----------------------------------------------------------------------
    //  Our coded name
    // -----------------------------------------------------------------------
    const TString strCodecName(L"MS-WMA");
}



// ---------------------------------------------------------------------------
//  Local helper functions
// ---------------------------------------------------------------------------
static tCIDLib::TVoid CIDDAE_XlatSysErr(tCIDLib::TCard4 c4Err, TString& strToFill)
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
CIDDAE_ThrowEncError(const  tCIDLib::TErrCode   errcToThrow
                    , const tCIDLib::TCard4     c4HRes
                    , const tCIDLib::TCard4     c4Line)
{
    TString strSysErr;
    CIDDAE_XlatSysErr(c4HRes, strSysErr);
    facCIDDAE().ThrowErr
    (
        CID_FILE
        , c4Line
        , errcToThrow
        , strSysErr
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::CantDo
    );
}

static tCIDLib::TVoid
CIDDAE_ThrowEncError(const  tCIDLib::TErrCode   errcToThrow
                    , const tCIDLib::TCard4     c4HRes
                    , const tCIDLib::TCard4     c4Line
                    , const MFormattable&       mfmtblTok1
                    , const MFormattable&       mfmtblTok2)
{
    TString strSysErr;
    CIDDAE_XlatSysErr(c4HRes, strSysErr);
    facCIDDAE().ThrowErr
    (
        CID_FILE
        , c4Line
        , errcToThrow
        , strSysErr
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::CantDo
        , mfmtblTok1
        , mfmtblTok2
    );
}



//
//  Builds an audio profile for the passed codec name and format. We throw
//  if these specific names are not found.
//
//  We assume that the code below that calls us has initialized COM and
//  created a profile manager already.
//
static tCIDLib::TVoid
CIDDAE_BuildProfile(        CIDDAE_WMAEncoder::TWMAWInfo&    Info
                    , const TCIDDAEWMAEnc::TCodecInfo&      cdeciCodec
                    , const TCIDDAEWMAEnc::TCodecInfo&      cdeciFmt)
{
    HRESULT hRes;

    // Get a codec info object
    IWMCodecInfo3* pCodecInfo = 0;
    hRes = CIDDAE_WMAEncoder::pProfMgr->QueryInterface
    (
        IID_IWMCodecInfo3, (void**)&pCodecInfo
    );
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_QueryCodecInfo, hRes, CID_LINE);
    TCOMJanitor<IWMCodecInfo3> janCodecInfo(&pCodecInfo);

    // Set the VBR and passes info for the incoming codec/format
    BOOL bFlag = cdeciFmt.m_bIsVBR ? TRUE : FALSE;
    hRes = pCodecInfo->SetCodecEnumerationSetting
    (
        WMMEDIATYPE_Audio
        , cdeciCodec.m_c4Index
        , g_wszVBREnabled
        , WMT_TYPE_BOOL
        , (BYTE*)&bFlag
        , sizeof(BOOL)
    );
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_SetEnumType, hRes, CID_LINE);

    // If VBR, set the passes
    if (cdeciFmt.m_bIsVBR)
    {
        hRes = pCodecInfo->SetCodecEnumerationSetting
        (
            WMMEDIATYPE_Audio
            , cdeciCodec.m_c4Index
            , g_wszNumPasses
            , WMT_TYPE_DWORD
            , (BYTE*)&cdeciFmt.m_c4Passes
            , sizeof(DWORD)
        );
        if (FAILED(hRes))
            CIDDAE_ThrowEncError(kDAEErrs::errcWMA_SetEnumType, hRes, CID_LINE);
    }

    // Get the indicated codec and format
    IWMStreamConfig* pStreamCfg = 0;
    hRes = pCodecInfo->GetCodecFormat
    (
        WMMEDIATYPE_Audio, cdeciCodec.m_c4Index, cdeciFmt.m_c4Index, &pStreamCfg
    );
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_QueryCodecFmt, hRes, CID_LINE);
    TCOMJanitor<IWMStreamConfig> janConfig(&pStreamCfg);

    // Create an empty profile object that we can start setting up
    IWMProfile* pProfile = 0;
    hRes = CIDDAE_WMAEncoder::pProfMgr->CreateEmptyProfile(WMT_VER_9_0, &pProfile);
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_CreateProfile, hRes, CID_LINE);
    TCOMJanitor<IWMProfile> janProfile(&pProfile);

    //
    //  Create a new stream, get the stream number out, then dump it. We'll
    //  use the one we already have, and just set the stream number on it.
    //
    {
        IWMStreamConfig* pStreamCfg2 = 0;
        hRes = pProfile->CreateNewStream(WMMEDIATYPE_Audio, &pStreamCfg2);
        if (FAILED(hRes))
            CIDDAE_ThrowEncError(kDAEErrs::errcWMA_CreateStream, hRes, CID_LINE);

        tCIDLib::TCard2 c2StreamNum;
        pStreamCfg2->GetStreamNumber(&c2StreamNum);
        pStreamCfg->SetStreamNumber(c2StreamNum);
        pStreamCfg2->Release();
    }

    hRes = pStreamCfg->SetStreamName((WCHAR*)L"Audio Stream");
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_SetStreamName, hRes, CID_LINE);

    hRes = pStreamCfg->SetConnectionName((WCHAR*)L"Audio");
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_SetConnectionName, hRes, CID_LINE);

    // Get the media properties interface of the original format
    IWMMediaProps* pProps = 0;
    hRes = pStreamCfg->QueryInterface(IID_IWMMediaProps, (void**)&pProps);
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_GetMediaProps, hRes, CID_LINE);
    TCOMJanitor<IWMMediaProps> janProps(&pProps);

    // And use that to query the media type info
    tCIDLib::TCard4 c4Size = 0;
    pProps->GetMediaType(0, &c4Size);
    tCIDLib::TCard1* pc1FmtBuf = new tCIDLib::TCard1[c4Size];
    TArrayJanitor<tCIDLib::TCard1> janFmtBuf(pc1FmtBuf);
    WM_MEDIA_TYPE* pMType = (WM_MEDIA_TYPE*)pc1FmtBuf;
    hRes = pProps->GetMediaType(pMType, &c4Size);
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_GetMediaType, hRes, CID_LINE);
    WAVEFORMATEX* pWaveFmt = (WAVEFORMATEX*)pMType->pbFormat;

    //
    //  Set the buffer window. It seems to be -1 in the actual retrieved
    //  profiles. We just use a fixed value of 3 seconds.
    //
    pStreamCfg->SetBufferWindow(3000);

    //
    //  If we are VBR or quality based, then we have to set a few more
    //  things. Note that we only do unconstrained VBR. We don't use any
    //  multi-pass VBR modes
    //
    IWMPropertyVault* pVault = 0;
    hRes = pStreamCfg->QueryInterface(IID_IWMPropertyVault, (void**)&pVault);
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_QueryPropVault, hRes, CID_LINE);

    if (cdeciFmt.m_bIsVBR)
    {
        // Set the VBR setting on it
        bFlag = TRUE;
        pVault->SetProperty
        (
            g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE*)&bFlag, sizeof(BOOL)
        );
    }

    // And now add the stream to the profile
    hRes = pProfile->AddStream(pStreamCfg);
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_AddStreamToProfile, hRes, CID_LINE);

    // We survived, so orphan the profile out its janitor into the return
    Info.pProfile = janProfile.pobjOrphan();
}


//
//  This is called multiple times, once with each VBR/Pass setting, to get
//  the various types of encodings.
//
static tCIDLib::TVoid
CIDDAE_EnumCodecFmts(       IWMCodecInfo3*                          pCodecInfo
                    , const tCIDLib::TCard4                         c4CodecInd
                    ,       TCollection<TCIDDAEWMAEnc::TCodecInfo>& colToFill
                    , const tCIDLib::TBoolean                       bVBR
                    , const tCIDLib::TCard4                         c4Passes)
{
    HRESULT hRes;

    // Set the passed VBR setting
    BOOL bFlag = bVBR ? TRUE : FALSE;
    hRes = pCodecInfo->SetCodecEnumerationSetting
    (
        WMMEDIATYPE_Audio
        , c4CodecInd
        , g_wszVBREnabled
        , WMT_TYPE_BOOL
        , (BYTE*)&bFlag
        , sizeof(BOOL)
    );
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_SetEnumType, hRes, CID_LINE);

    // If VBR, set the passes
    if (bVBR)
    {
        hRes = pCodecInfo->SetCodecEnumerationSetting
        (
            WMMEDIATYPE_Audio
            , c4CodecInd
            , g_wszNumPasses
            , WMT_TYPE_DWORD
            , (BYTE*)&c4Passes
            , sizeof(c4Passes)
        );
        if (FAILED(hRes))
            CIDDAE_ThrowEncError(kDAEErrs::errcWMA_SetEnumType, hRes, CID_LINE);
    }

    // Get the count of formats provided by the incoming codec
    tCIDLib::TCard4 c4FmtCount = 0;
    hRes = pCodecInfo->GetCodecFormatCount
    (
        WMMEDIATYPE_Audio, c4CodecInd, &c4FmtCount
    );
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_QueryCodecFmtCnt, hRes, CID_LINE);

    for (tCIDLib::TCard4 c4FmtIndex = 0; c4FmtIndex < c4FmtCount; c4FmtIndex++)
    {
        tCIDLib::TCard4 c4Size = 0;

        // Get the description of this format
        IWMStreamConfig* pStreamCfg = 0;
        hRes = pCodecInfo->GetCodecFormatDesc
        (
            WMMEDIATYPE_Audio, c4CodecInd, c4FmtIndex, 0, 0, &c4Size
        );
        if (FAILED(hRes))
            CIDDAE_ThrowEncError(kDAEErrs::errcWMA_QueryCodecFmtName, hRes, CID_LINE);

        tCIDLib::TCh* pszBuf = new tCIDLib::TCh[c4Size + 1];
        TArrayJanitor<tCIDLib::TCh> janBuf(pszBuf);

        // Ok, get the name and format info and see if it's one of our guys
        {
            hRes = pCodecInfo->GetCodecFormatDesc
            (
                WMMEDIATYPE_Audio
                , c4CodecInd
                , c4FmtIndex
                , &pStreamCfg
                , pszBuf
                , &c4Size
            );
            if (FAILED(hRes))
                CIDDAE_ThrowEncError(kDAEErrs::errcWMA_QueryCodecFmtName, hRes, CID_LINE);
            TCOMJanitor<IWMStreamConfig> janFormat(&pStreamCfg);

            //
            //  If it contains the sub-string (A/V), then it's a mixed type
            //  of codec, and we don't want it.
            //
            if (TRawStr::pszFindSubStr(pszBuf, L"(A/V)"))
                continue;

            // Get the media properties interface
            IWMMediaProps* pProps = 0;
            hRes = pStreamCfg->QueryInterface(IID_IWMMediaProps, (void**)&pProps);
            if (FAILED(hRes))
                CIDDAE_ThrowEncError(kDAEErrs::errcWMA_GetMediaProps, hRes, CID_LINE);
            TCOMJanitor<IWMMediaProps> janProps(&pProps);

            // And use that to query the media type info
            c4Size = 0;
            pProps->GetMediaType(0, &c4Size);
            tCIDLib::TCard1* pc1FmtBuf = new tCIDLib::TCard1[c4Size];
            TArrayJanitor<tCIDLib::TCard1> janFmtBuf(pc1FmtBuf);
            WM_MEDIA_TYPE* pMType = (WM_MEDIA_TYPE*)pc1FmtBuf;
            hRes = pProps->GetMediaType(pMType, &c4Size);
            if (FAILED(hRes))
                CIDDAE_ThrowEncError(kDAEErrs::errcWMA_GetMediaType, hRes, CID_LINE);

            // If not 2-channel, 16 bit, CD PCM, then skip it
            WAVEFORMATEX* pWaveFmt = (WAVEFORMATEX*)pMType->pbFormat;
            if ((pMType->formattype != WMFORMAT_WaveFormatEx)
            ||  (pWaveFmt->nChannels != 2)
            ||  (pWaveFmt->wBitsPerSample != 16)
            ||  (pWaveFmt->nSamplesPerSec != 44100))
            {
                continue;
            }

            // It passed, so keep it
            colToFill.objAdd
            (
                TCIDDAEWMAEnc::TCodecInfo
                (
                    TString(janBuf.paOrphan(), tCIDLib::EAdoptOpts::Adopt)
                    , c4FmtIndex
                    , bVBR
                    , c4Passes
                )
            );
        }
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TCIDDAEWMADec
// PREFIX: daed
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDDAEWMAEnc: Public, static data
// ---------------------------------------------------------------------------
const TString& TCIDDAEWMADec::strCodecName()
{
    return CIDDAE_WMAEncoder::strCodecName;
}


// ---------------------------------------------------------------------------
//  TCIDDAEWMAEnc: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDDAEWMADec::TCIDDAEWMADec() :

    TCIDDAEDecoder(CIDDAE_WMAEncoder::strCodecName)
    , m_pc1Data(0)
{
    // Allocate and initialize our internal data structure
    CIDDAE_WMAEncoder::TWMARInfo* pWMAInfo = new CIDDAE_WMAEncoder::TWMARInfo;
    TRawMem::SetMemBuf
    (
        pWMAInfo, tCIDLib::TCard1(0), sizeof(CIDDAE_WMAEncoder::TWMARInfo)
    );
    m_pc1Data = reinterpret_cast<tCIDLib::TCard1*>(pWMAInfo);
}

TCIDDAEWMADec::~TCIDDAEWMADec()
{
    // Look at our opaque data as its actual type
    CIDDAE_WMAEncoder::TWMAWInfo* pWMAInfo
                = reinterpret_cast<CIDDAE_WMAEncoder::TWMAWInfo*>(m_pc1Data);

    // Clean up any stuff we might need to

    // And then free the backing data
    if (m_pc1Data)
    {
        delete [] m_pc1Data;
        m_pc1Data = 0;
    }
}



// -------------------------------------------------------------------
//  Protected, virtual methods
// -------------------------------------------------------------------
tCIDLib::TCard4
TCIDDAEWMADec::c4LoadChunkImpl(         TMemBuf&        mbufToFill
                                , const tCIDLib::TCard4 c4StartInd)
{
    // Look at our opaque data as its actual type
    CIDDAE_WMAEncoder::TWMARInfo* pWMAInfo
                = reinterpret_cast<CIDDAE_WMAEncoder::TWMARInfo*>(m_pc1Data);

    INSSBuffer* pSample;
    HRESULT hRes;

    tCIDLib::TCard8 c8SampleLen;
    tCIDLib::TCard8 c8SampleTime;
    tCIDLib::TCard4 c4Flags;

    hRes = pWMAInfo->pReader->GetNextSample
    (
        pWMAInfo->c2StreamNum
        , &pSample
        , &c8SampleTime
        , &c8SampleLen
        , &c4Flags
        , 0
        , 0
    );

    // If at the end, just return zero samples
    if (hRes == NS_E_NO_MORE_SAMPLES)
        return 0;

    // Otherwise throw if an error
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_GetSample, hRes, CID_LINE);

    // Get the buffer and length and copy to the caller's buffer
    TCOMJanitor<INSSBuffer> janBuf(&pSample);
    tCIDLib::TCard4 c4Len;
    tCIDLib::TCard1* pc1BufPtr;
    hRes = pSample->GetBufferAndLength(&pc1BufPtr, &c4Len);
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_GetBufferInfo, hRes, CID_LINE);

    mbufToFill.CopyIn(pc1BufPtr, c4Len, c4StartInd);
    return c4Len;
}


tCIDLib::TVoid TCIDDAEWMADec::CleanupImpl()
{
    // Look at our opaque data as its actual type
    CIDDAE_WMAEncoder::TWMARInfo* pWMAInfo
                = reinterpret_cast<CIDDAE_WMAEncoder::TWMARInfo*>(m_pc1Data);

    // Free up the stuff we may have gotten allocated
    SafeCOMFree(pWMAInfo->pReader);
}


tCIDLib::TVoid TCIDDAEWMADec::EndDecodeImpl()
{
    // Look at our opaque data as its actual type
    CIDDAE_WMAEncoder::TWMARInfo* pWMAInfo
                = reinterpret_cast<CIDDAE_WMAEncoder::TWMARInfo*>(m_pc1Data);

}


tCIDLib::TVoid TCIDDAEWMADec::StartDecodeImpl(const TString& strSrcFile)
{
    // Look at our opaque data as its actual type
    CIDDAE_WMAEncoder::TWMARInfo* pWMAInfo
                = reinterpret_cast<CIDDAE_WMAEncoder::TWMARInfo*>(m_pc1Data);

    //
    //  Just in case we failed to clean up, get rid of the stuff from the
    //  previous round.
    //
    SafeCOMFree(pWMAInfo->pReader);

    // Create our read object
    HRESULT hRes = ::WMCreateSyncReader(NULL, WMT_RIGHT_PLAYBACK, &pWMAInfo->pReader);
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_CreateReader, hRes, CID_LINE);

    // Open the source file
    hRes = pWMAInfo->pReader->Open(strSrcFile.pszBuffer());
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_OpenSource, hRes, CID_LINE);

    // Get the count of outputs, which should be 1 for us
    tCIDLib::TCard4 c4OutCnt;
    hRes = pWMAInfo->pReader->GetOutputCount(&c4OutCnt);

    // Get the stream for output 0
    hRes = pWMAInfo->pReader->GetStreamNumberForOutput(0, &pWMAInfo->c2StreamNum);

    // Tell it we want uncomperssed samples from this stream
    hRes = pWMAInfo->pReader->SetReadStreamSamples(pWMAInfo->c2StreamNum, FALSE);
}



// ---------------------------------------------------------------------------
//  CLASS: TCIDDAEWMAEnc
// PREFIX: daee
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDDAEWMAEnc: Public, static methods
// ---------------------------------------------------------------------------

//
//  Given the descriptions of a codec and a format from that codec, looks up
//  the index of the codec and format so that they can be used again after
//  being persisted. The indices cannot be persisted since they could change
//  while the host program is away.
//
tCIDLib::TBoolean
TCIDDAEWMAEnc::bFindCodecInfo(  const   TString&    strCodecDescr
                                , const TString&    strFmtDescr
                                ,       TCodecInfo& cdeciCodec
                                ,       TCodecInfo& cdeciFmt)
{
    // Enum the codecs. If none, then we obviously failed
    TVector<TCodecInfo> colList;
    const tCIDLib::TCard4 c4CodecCnt = c4EnumAudioCodecs(colList);
    if (!c4CodecCnt)
        return kCIDLib::False;

    // Search the list for the passed description
    tCIDLib::TCard4 c4CInd = 0;
    for (; c4CInd < c4CodecCnt; c4CInd++)
    {
        const TCodecInfo& cdeciCur = colList[c4CInd];
        if (cdeciCur.m_strName == strCodecDescr)
        {
            cdeciCodec = cdeciCur;
            break;
        }
    }

    // If we didn't find it, then we failed
    if (c4CInd == c4CodecCnt)
        return kCIDLib::False;

    // See if the format is present
    const tCIDLib::TCard4 c4FmtCnt = c4EnumCodecFmts(cdeciCodec, colList);
    if (!c4FmtCnt)
        return kCIDLib::False;

    // See if our fmt is in this codec's list
    tCIDLib::TCard4 c4FInd = 0;
    for (; c4FInd < c4FmtCnt; c4FInd++)
    {
        const TCodecInfo& cdeciCur = colList[c4FInd];
        if (colList[c4FInd].m_strName == strFmtDescr)
        {
            cdeciFmt = cdeciCur;
            break;
        }
    }
    return (c4FInd < c4FmtCnt);
}


//
//  Queries all the names of the available WMA codecs.
//
tCIDLib::TCard4
TCIDDAEWMAEnc::c4EnumAudioCodecs(TCollection<TCodecInfo>& colToFill)
{
    HRESULT hRes;

    // Empty the passed collection in preperation for reloading
    colToFill.RemoveAll();

    // Get a profile manager object, if not already created
    if (!CIDDAE_WMAEncoder::pProfMgr)
    {
        hRes = ::WMCreateProfileManager(&CIDDAE_WMAEncoder::pProfMgr);
        if (FAILED(hRes))
            CIDDAE_ThrowEncError(kDAEErrs::errcWMA_CreateProfMgr, hRes, CID_LINE);
    }

    // Use that to get a codec info object
    IWMCodecInfo3* pCodecInfo = 0;
    hRes = CIDDAE_WMAEncoder::pProfMgr->QueryInterface
    (
        IID_IWMCodecInfo3, (void**)&pCodecInfo
    );
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_QueryCodecInfo, hRes, CID_LINE);
    TCOMJanitor<IWMCodecInfo3> janCodecInfo(&pCodecInfo);

    // Retrieve the number of supported audio codecs on the system.
    tCIDLib::TCard4 c4CodecCnt;
    hRes = pCodecInfo->GetCodecInfoCount(WMMEDIATYPE_Audio, &c4CodecCnt);
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_QueryCodecCnt, hRes, CID_LINE);

    // Loop through them and get all the names
    TString strName;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4CodecCnt; c4Index++)
    {
        // First, get the size of the name
        tCIDLib::TCard4 c4NameLen = 0;
        hRes = pCodecInfo->GetCodecName
        (
            WMMEDIATYPE_Audio, c4Index, NULL, &c4NameLen
        );
        if (FAILED(hRes))
            CIDDAE_ThrowEncError(kDAEErrs::errcWMA_QueryCodecNameLen, hRes, CID_LINE);

        // Allocate a buffer and query again
        {
            tCIDLib::TCh* pszBuf = new tCIDLib::TCh[c4NameLen + 1];
            TArrayJanitor<tCIDLib::TCh> janBuf(pszBuf);

            hRes = pCodecInfo->GetCodecName
            (
                WMMEDIATYPE_Audio, c4Index, pszBuf, &c4NameLen
            );
            if (FAILED(hRes))
                CIDDAE_ThrowEncError(kDAEErrs::errcWMA_QueryCodecName, hRes, CID_LINE);

            //
            //  If this guy starts with "Windows Media Audio", then add it
            //  to the list. Also skip anything with Voice in it
            //
            tCIDLib::TCard4 c4Pos;
            strName = pszBuf;
            if (strName.bStartsWith(L"Windows Media Audio")
            &&  !strName.bFirstOccurrence(L"Voice", c4Pos))
            {
                colToFill.objAdd(TCodecInfo(strName, c4Index));
            }
        }
    }
    return colToFill.c4ElemCount();
}



//
//  Queries all the names of the available formats for a passed WMA codec and
//  loads up the passed collection with the formats that match our criteria.
//
tCIDLib::TCard4
TCIDDAEWMAEnc::c4EnumCodecFmts( const   TCodecInfo&                 cdeciSrc
                                ,       TCollection<TCodecInfo>&    colToFill)
{
    HRESULT hRes;

    // Empty the passed collection in preperation for reloading
    colToFill.RemoveAll();

    // Get a profile manager object, if not already created
    if (!CIDDAE_WMAEncoder::pProfMgr)
    {
        hRes = ::WMCreateProfileManager(&CIDDAE_WMAEncoder::pProfMgr);
        if (FAILED(hRes))
            CIDDAE_ThrowEncError(kDAEErrs::errcWMA_CreateProfMgr, hRes, CID_LINE);
    }

    // Use that to get a codec info object
    IWMCodecInfo3* pCodecInfo = 0;
    hRes = CIDDAE_WMAEncoder::pProfMgr->QueryInterface
    (
        IID_IWMCodecInfo3, (void**)&pCodecInfo
    );
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_QueryCodecInfo, hRes, CID_LINE);
    TCOMJanitor<IWMCodecInfo3> janCodecInfo(&pCodecInfo);

    // Do non-VBR
    CIDDAE_EnumCodecFmts(pCodecInfo, cdeciSrc.m_c4Index, colToFill, kCIDLib::False, 0);

    // Do VBR, 1 pass
    CIDDAE_EnumCodecFmts(pCodecInfo, cdeciSrc.m_c4Index, colToFill, kCIDLib::True, 1);

    // Do VBR, 2 pass (For now we don't support these)
//    CIDDAE_EnumCodecFmts(pCodecInfo, cdeciSrc.m_c4Index, colToFill, kCIDLib::True, 2);

    return colToFill.c4ElemCount();
}


// Returns our codec name
const TString& TCIDDAEWMAEnc::strCodecName()
{
    return CIDDAE_WMAEncoder::strCodecName;
}



// ---------------------------------------------------------------------------
//  TCIDDAEWMAEnc: Constructors and Destructor
// ---------------------------------------------------------------------------
TCIDDAEWMAEnc::TCIDDAEWMAEnc() :

    TCIDDAEEncoder(CIDDAE_WMAEncoder::strCodecName)
    , m_pc1Data(nullptr)
{
    // Allocate and initialize our internal data structure
    CIDDAE_WMAEncoder::TWMAWInfo* pWMAInfo = new CIDDAE_WMAEncoder::TWMAWInfo;
    TRawMem::SetMemBuf
    (
        pWMAInfo, tCIDLib::TCard1(0), sizeof(CIDDAE_WMAEncoder::TWMAWInfo)
    );
    m_pc1Data = reinterpret_cast<tCIDLib::TCard1*>(pWMAInfo);

    //
    //  Pre-fill in the input format info since we always use the same
    //  source format which is CD style PCM data.
    //
    pWMAInfo->WaveFmt.wFormatTag = WAVE_FORMAT_PCM;
    pWMAInfo->WaveFmt.nChannels = 2;
    pWMAInfo->WaveFmt.nSamplesPerSec = 44100;
    pWMAInfo->WaveFmt.wBitsPerSample = 16;
    pWMAInfo->WaveFmt.cbSize = 0;

    // Some are calculated based on others
    pWMAInfo->WaveFmt.nBlockAlign =
    (
        (pWMAInfo->WaveFmt.nChannels * pWMAInfo->WaveFmt.wBitsPerSample) / 8
    );

    pWMAInfo->WaveFmt.nAvgBytesPerSec =
    (
        pWMAInfo->WaveFmt.nBlockAlign * pWMAInfo->WaveFmt.nSamplesPerSec
    );

    pWMAInfo->MediaType.majortype            = WMMEDIATYPE_Audio;
    pWMAInfo->MediaType.subtype              = WMMEDIASUBTYPE_PCM;
    pWMAInfo->MediaType.bFixedSizeSamples    = TRUE;
    pWMAInfo->MediaType.bTemporalCompression = FALSE;
    pWMAInfo->MediaType.lSampleSize          = 4;
    pWMAInfo->MediaType.formattype           = WMFORMAT_WaveFormatEx;
    pWMAInfo->MediaType.pUnk                 = NULL;

    pWMAInfo->MediaType.cbFormat             = sizeof(WAVEFORMATEX);
    pWMAInfo->MediaType.pbFormat             = (BYTE*)&pWMAInfo->WaveFmt;
}

TCIDDAEWMAEnc::~TCIDDAEWMAEnc()
{
    // Look at our opaque data as its actual type
    CIDDAE_WMAEncoder::TWMAWInfo* pWMAInfo
                = reinterpret_cast<CIDDAE_WMAEncoder::TWMAWInfo*>(m_pc1Data);

    // Clean up any of its members that we need to

    // And free the backing memory
    if (m_pc1Data)
    {
        delete [] m_pc1Data;
        m_pc1Data = nullptr;
    }
}


// ---------------------------------------------------------------------------
//  TCIDDAEWMAEnc: Public, inherited methods
// ---------------------------------------------------------------------------

//
//  Checks to see if the passed codec name and format are valid. We just
//  call our static lookup method and return its return, and eat the
//  output information, which is trivial so it's not great inefficiency
//  to do it this way.
//
tCIDLib::TBoolean
TCIDDAEWMAEnc::bCodecInfoValid( const   TString&    strCodec
                                , const TString&    strFormat) const
{
    TCodecInfo cdeciCodec;
    TCodecInfo cdeciFmt;
    return bFindCodecInfo(strCodec, strFormat, cdeciCodec, cdeciFmt);
}


// We return a list of the names for all our supported codecs
tCIDLib::TCard4 TCIDDAEWMAEnc::c4QueryCodecNames(TVector<TString>& colToFill)
{
    colToFill.RemoveAll();

    TVector<TCodecInfo> colCodecs;
    const tCIDLib::TCard4 c4Count = c4EnumAudioCodecs(colCodecs);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        colToFill.objAdd(colCodecs[c4Index].m_strName);
    return c4Count;
}


//
//  We return a list of the available formats for the indicated codec. If
//  we cannot find the codec, or it doesn't have any formats, we return
//  zero, which effectively means it's an unusuable codec.
//
tCIDLib::TCard4
TCIDDAEWMAEnc::c4QueryCodecFmtNames(const   TString&            strCodec
                                    ,       TVector<TString>&   colToFill)
{
    colToFill.RemoveAll();

    //
    //  See if we can find the passed codec. If not, then we return max
    //  card to indicate we couldn't do anything.
    //
    TVector<TCodecInfo> colInfo;
    const tCIDLib::TCard4 c4Count = c4EnumAudioCodecs(colInfo);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TCodecInfo& cdeciCur = colInfo[c4Index];

        if (cdeciCur.m_strName == strCodec)
        {
            // This is our guy, so query the formats for him
            TCodecInfo cdeciCodec = cdeciCur;
            colInfo.RemoveAll();
            try
            {
                const tCIDLib::TCard4 c4FmtCnt = c4EnumCodecFmts(cdeciCodec, colInfo);
                for (tCIDLib::TCard4 c4FmtInd = 0; c4FmtInd < c4FmtCnt; c4FmtInd++)
                    colToFill.objAdd(colInfo[c4FmtInd].m_strName);
            }

            catch(TError& errToCatch)
            {
                if (!errToCatch.bLogged())
                {
                    errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                    TModule::LogEventObj(errToCatch);
                }
            }
            break;
        }
    }

    // If we found nothing, we'll return zero
    return colToFill.c4ElemCount();
}


// ---------------------------------------------------------------------------
//  TCIDDAEWMAEnc: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  This is called when the encoder is no longer needed, and gives up a
//  chance to clean up any resources. We clean up any system resources that
//  we keep around for the life of the encoder.
//
tCIDLib::TVoid TCIDDAEWMAEnc::CleanupImpl()
{
    // Look at our opaque data as its actual type
    CIDDAE_WMAEncoder::TWMAWInfo* pWMAInfo
                = reinterpret_cast<CIDDAE_WMAEncoder::TWMAWInfo*>(m_pc1Data);

    // Free up the stuff we allocated
    SafeCOMFree(pWMAInfo->pWriter);
    SafeCOMFree(pWMAInfo->pProfile);
}


//
//  Handles any post-encoding cleanup we need to do. We close out the
//  writer object.
//
tCIDLib::TVoid TCIDDAEWMAEnc::EndEncodeImpl()
{
    // Look at our opaque data as its actual type
    CIDDAE_WMAEncoder::TWMAWInfo* pWMAInfo
                = reinterpret_cast<CIDDAE_WMAEncoder::TWMAWInfo*>(m_pc1Data);

    // Tell the writer we are done
    pWMAInfo->pWriter->EndWriting();
}


//
//  Handles any startup work we need to do. We do the work to set up the
//  writer object and the profile and get it ready to start accepting
//  samples. If we get metadata, we'll apply those as tags to the outgoing
//  file.
//
tCIDLib::TVoid
TCIDDAEWMAEnc::StartEncodeImpl( const   TString&                strTargetFile
                                , const TString&                strCodecName
                                , const TString&                strFmtName
                                , const tCIDLib::TCard4         c4TrackNum
                                , const TString&                strAlbumTitle
                                , const TString&                strTrackTitle
                                , const TString&                strArtist
                                , const TString&                strLabel
                                , const TString&                strGenre
                                , const tCIDLib::TCard4         c4Year)
{
    // Look at our opaque data as its actual type
    CIDDAE_WMAEncoder::TWMAWInfo* pWMAInfo
                = reinterpret_cast<CIDDAE_WMAEncoder::TWMAWInfo*>(m_pc1Data);

    //
    //  Just in case we failed to clean up, get rid of the stuff from the
    //  previous round.
    //
    SafeCOMFree(pWMAInfo->pWriter);
    SafeCOMFree(pWMAInfo->pProfile);

    // Create our writer object
    HRESULT hRes = ::WMCreateWriter(NULL, &pWMAInfo->pWriter);
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_CreateWriter, hRes, CID_LINE);

    // Create a profile manager if it's not been created yet
    if (!CIDDAE_WMAEncoder::pProfMgr)
    {
        hRes = ::WMCreateProfileManager(&CIDDAE_WMAEncoder::pProfMgr);
        if (FAILED(hRes))
            CIDDAE_ThrowEncError(kDAEErrs::errcWMA_CreateProfMgr, hRes, CID_LINE);
    }

    // Try to find the passed codec indices
    TCodecInfo cdeciCodec;
    TCodecInfo cdeciFmt;
    if (!bFindCodecInfo(strCodecName, strFmtName, cdeciCodec, cdeciFmt))
    {
        facCIDDAE().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDAEErrs::errcWMA_UnknownEncodeFmt
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::NotFound
            , strFmtName
            , strCodecName
        );
    }

    // Create a WMA audio profile for the indicated codec and format
    CIDDAE_BuildProfile(*pWMAInfo, cdeciCodec, cdeciFmt);

    // Set the profile on the writer
    hRes = pWMAInfo->pWriter->SetProfile(pWMAInfo->pProfile);
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_SetProfile, hRes, CID_LINE);

    //
    //  Get the current input properties of the input. Just use input
    //  zero on the assumption that we only ever have one.
    //
    IWMInputMediaProps* pInpProps2 = nullptr;
    hRes = pWMAInfo->pWriter->GetInputProps(0, &pInpProps2);
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_GetMediaProps, hRes, CID_LINE);
    TCOMJanitor<IWMInputMediaProps> janInpProps2(&pInpProps2);

    // Set the prefab media type info on it that we set up earlier
    hRes = pInpProps2->SetMediaType(&pWMAInfo->MediaType);
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_SetMediaType, hRes, CID_LINE);

    // And now set the input properties back on the writer
    hRes = pWMAInfo->pWriter->SetInputProps(0, pInpProps2);
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_SetInputProps, hRes, CID_LINE);

    // Tell the writer the output file name
    pWMAInfo->pWriter->SetOutputFilename(strTargetFile.pszBuffer());

    // Apply any metadata we got
    IWMHeaderInfo3* pHdrInfo = nullptr;
    hRes = pWMAInfo->pWriter->QueryInterface(IID_IWMHeaderInfo3, (void**)&pHdrInfo);
    TCOMJanitor<IWMHeaderInfo3> janHdrInfo(&pHdrInfo);
    AddStringTag(pHdrInfo, g_wszWMAlbumTitle, strAlbumTitle);
    AddStringTag(pHdrInfo, g_wszWMAuthor, strArtist);
    AddStringTag(pHdrInfo, g_wszWMContentDistributor, strLabel);
    AddStringTag(pHdrInfo, g_wszWMToolVersion, L"CIDLib WMA Encoder");
    AddCardTag(pHdrInfo, g_wszWMTrackNumber, c4TrackNum + 1);
    AddCardTag(pHdrInfo, g_wszWMYear, c4Year);
    AddStringTag(pHdrInfo, g_wszWMGenre, strGenre);
    AddStringTag(pHdrInfo, g_wszWMTitle, strTrackTitle);

    // Reset any remaining per-session stuff
    pWMAInfo->c8CurTime = 0;

    // Tell the writer to begin writing
    hRes = pWMAInfo->pWriter->BeginWriting();
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_BeginWrite, hRes, CID_LINE);
}


//
//  This is called repeatedly to store the chunks. For WMA content we have
//  to send it to the writer for processing.
//
tCIDLib::TVoid
TCIDDAEWMAEnc::StoreChunkImpl( const    tCIDLib::TCard1* const  pc1Data
                                , const tCIDLib::TCard4         c4Bytes)
{
    // Look at our opaque data as its actual type
    CIDDAE_WMAEncoder::TWMAWInfo* pWMAInfo
                = reinterpret_cast<CIDDAE_WMAEncoder::TWMAWInfo*>(m_pc1Data);

    //
    //  We need to write the samples to the writer. We have to allocate
    //  a new buffer from the writer every time.
    //
    INSSBuffer* pBuf = 0;
    HRESULT hRes = pWMAInfo->pWriter->AllocateSample(c4Bytes, &pBuf);
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_AllocSampBuf, hRes, CID_LINE);
    TCOMJanitor<INSSBuffer> janBuf(&pBuf);

    // Get the raw buffer and buffer size
    tCIDLib::TCard4 c4BufSz;
    tCIDLib::TCard1* pc1RawBuf;
    hRes = pBuf->GetBufferAndLength(&pc1RawBuf, &c4BufSz);

    // Sanity check it so we don't overwrite a buffer
    if (c4BufSz < c4Bytes)
    {
        facCIDDAE().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kDAEErrs::errcWMA_BufTooSmall
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4Bytes)
            , TCardinal(c4BufSz)
        );
    }

    // Copy over our data to the target buffer
    TRawMem::CopyMemBuf(pc1RawBuf, pc1Data, c4Bytes);
    pBuf->SetLength(c4Bytes);

    // Now we can give the bytes to the writer
    hRes = pWMAInfo->pWriter->WriteSample(0, pWMAInfo->c8CurTime, 0, pBuf);
    if (FAILED(hRes))
        CIDDAE_ThrowEncError(kDAEErrs::errcWMA_WriteSample, hRes, CID_LINE);

    // Update our current time
    pWMAInfo->c8CurTime +=  10000000 * tCIDLib::TCard8(c4Bytes)
                            / pWMAInfo->WaveFmt.nAvgBytesPerSec;
}


// ---------------------------------------------------------------------------
//  TCIDDAEWMAEnc: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TCIDDAEWMAEnc::AddCardTag(          IWMHeaderInfo3* const   pHdrInfo
                            , const tCIDLib::TCh* const     pszAttr
                            , const tCIDLib::TCard4         c4Value)
{
    // Don't do it if zero
    if (!c4Value)
        return;

    WORD wIndex;
    pHdrInfo->AddAttribute
    (
        0
        , pszAttr
        , &wIndex
        , WMT_TYPE_DWORD
        , 0
        , (BYTE*)&c4Value
        , sizeof(c4Value)
    );
}


tCIDLib::TVoid
TCIDDAEWMAEnc::AddCard8Tag(         IWMHeaderInfo3* const   pHdrInfo
                            , const tCIDLib::TCh* const     pszAttr
                            , const tCIDLib::TCard8         c8Value)
{
    // Don't do it if zero
    if (!c8Value)
        return;

    WORD wIndex;
    pHdrInfo->AddAttribute
    (
        0
        , pszAttr
        , &wIndex
        , WMT_TYPE_QWORD
        , 0
        , (BYTE*)&c8Value
        , sizeof(c8Value)
    );
}


tCIDLib::TVoid
TCIDDAEWMAEnc::AddStringTag(        IWMHeaderInfo3* const   pHdrInfo
                            , const tCIDLib::TCh* const     pszAttr
                            , const TString&                strValue)
{
    // Don't do it if empty
    if (strValue.bIsEmpty())
        return;

    // Add one to the length so he gets the null terminator
    WORD wIndex;
    pHdrInfo->AddAttribute
    (
        0
        , pszAttr
        , &wIndex
        , WMT_TYPE_STRING
        , 0
        , (BYTE*)strValue.pszBuffer()
        , (strValue.c4Length() + 1) * kCIDLib::c4CharBytes
    );
}


