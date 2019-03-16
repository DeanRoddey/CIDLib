//
// FILE NAME: CIDDAE_Codec.cpp
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
//  This file implements the core encoder/decoder base interfaces that all
//  actual codec implementations derive from.
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
#include    "CIDDAE_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TDAEBuf,TObject)
RTTIDecls(TCIDDAECodec,TObject)
RTTIDecls(TCIDDAEDecoder,TObject)
RTTIDecls(TCIDDAEEncoder,TObject)



// ---------------------------------------------------------------------------
//  CLASS: TDAEBuf
// PREFIX: daeb
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TDAEBuf: Constructors and Destructor
// ---------------------------------------------------------------------------
TDAEBuf::TDAEBuf() :

    m_c4BufSz(0)
    , m_c4BytesRead(0)
    , m_c4EndBlk(0)
    , m_c4StartBlk(0)
    , m_pc1Buf(nullptr)
    , m_pc1StartPtr(nullptr)
{
}

TDAEBuf::TDAEBuf(const tCIDLib::TCard4 c4BufSz) :

    m_c4BufSz(c4BufSz)
    , m_c4BytesRead(0)
    , m_c4EndBlk(0)
    , m_c4StartBlk(0)
    , m_pc1Buf(new tCIDLib::TCard1[c4BufSz])
{
    m_pc1StartPtr = m_pc1Buf;
}

TDAEBuf::~TDAEBuf()
{
    // Clean up the buffer
    if (m_pc1Buf)
        delete [] m_pc1Buf;
}


// ---------------------------------------------------------------------------
//  TDAEBuf: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TDAEBuf::Reset(const tCIDLib::TCard4 c4BufSz)
{
    // See if we already have a buffer and need to reallocate
    if (m_c4BufSz && (c4BufSz != m_c4BufSz))
    {
        delete [] m_pc1Buf;
        m_pc1Buf = nullptr;
        m_pc1StartPtr = nullptr;
        m_c4BufSz = 0;
    }

    // If no buffer yet, or we need to reallocate, then do that
    if (!m_pc1Buf)
        m_pc1Buf = new tCIDLib::TCard1[c4BufSz];
    m_pc1StartPtr = m_pc1Buf;
    m_c4BufSz = c4BufSz;
}



// ---------------------------------------------------------------------------
//  CLASS: TCIDDAECodec
// PREFIX: daec
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDDAECodec: Destructor
// ---------------------------------------------------------------------------
TCIDDAECodec::~TCIDDAECodec()
{
}


// ---------------------------------------------------------------------------
//  TCIDDAECodec: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TString& TCIDDAECodec::strGenericName() const
{
    return m_strGenericName;
}


// ---------------------------------------------------------------------------
//  TCIDDAECodec: Hidden constructors
// ---------------------------------------------------------------------------
TCIDDAECodec::TCIDDAECodec(const TString& strName) :

    m_strGenericName(strName)
{
}




// ---------------------------------------------------------------------------
//  CLASS: TCIDDAEDecoder
// PREFIX: daed
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDDAEDecoder: Destructor
// ---------------------------------------------------------------------------
TCIDDAEDecoder::~TCIDDAEDecoder()
{
}


// ---------------------------------------------------------------------------
//  TCIDDAEDecoder: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TCIDDAEDecoder::c4LoadChunk(        TMemBuf&        mbufToFill
                            , const tCIDLib::TCard4 c4StartInd)
{
    // For now just pass pass through to the derived class
    return c4LoadChunkImpl(mbufToFill, c4StartInd);
}


tCIDLib::TVoid TCIDDAEDecoder::Cleanup()
{
    // For now just pass pass through to the derived class
    CleanupImpl();
}


tCIDLib::TVoid TCIDDAEDecoder::EndDecode()
{
    // For now just pass pass through to the derived class
    EndDecodeImpl();
}


tCIDLib::TVoid TCIDDAEDecoder::StartDecode(const TString& strSrcFile)
{
    // For now just pass pass through to the derived class
    StartDecodeImpl(strSrcFile);
}


// ---------------------------------------------------------------------------
//  TCIDDAEDecoder: Hidden Constructors
// ---------------------------------------------------------------------------
TCIDDAEDecoder::TCIDDAEDecoder(const TString& strCodecName) :

    TCIDDAECodec(strCodecName)
{
}




// ---------------------------------------------------------------------------
//  CLASS: TCIDDAEEncoder
// PREFIX: daee
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCIDDAEEncoder: Destructor
// ---------------------------------------------------------------------------
TCIDDAEEncoder::~TCIDDAEEncoder()
{
}


// ---------------------------------------------------------------------------
//  TCIDDAEEncoder: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCIDDAEEncoder::Cleanup()
{
    // For now just pass pass through to the derived class
    CleanupImpl();
}


tCIDLib::TVoid TCIDDAEEncoder::EndEncode()
{
    // For now just pass through to the derived class
    EndEncodeImpl();
}


tCIDLib::TVoid
TCIDDAEEncoder::StartEncode(const   TString&            strTargetPath
                            , const TString&            strEncodingName
                            , const TString&            strFmtName
                            , const tCIDLib::TCard4     c4TrackNum
                            , const TString&            strAlbumTitle
                            , const TString&            strTrackTitle
                            , const TString&            strArtist
                            , const TString&            strLabel
                            , const TString&            strGenre
                            , const tCIDLib::TCard4     c4Year)
{
    // For now just pass through to the derived class
    StartEncodeImpl
    (
        strTargetPath
        , strEncodingName
        , strFmtName
        , c4TrackNum
        , strAlbumTitle
        , strTrackTitle
        , strArtist
        , strLabel
        , strGenre
        , c4Year
    );
}


// Called repeatedly to store the chunks
tCIDLib::TVoid
TCIDDAEEncoder::StoreChunk( const   tCIDLib::TCard1* const  pc1Data
                            , const tCIDLib::TCard4         c4Bytes)
{
    // For now just pass through to the derived class
    StoreChunkImpl(pc1Data, c4Bytes);
}


// ---------------------------------------------------------------------------
//  TCIDDAEEncoder: Hidden Constructors
// ---------------------------------------------------------------------------
TCIDDAEEncoder::TCIDDAEEncoder(const TString& strCodecName) :

    TCIDDAECodec(strCodecName)
{
}


