//
// FILE NAME: CIDZLib_CompImpl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/2004
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements ZLib compression and decompression algorithm.
//  Because of the size we have some extra implementation files to hold some
//  grunt work methods, so this one just holds the main logic.
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
#include    "CIDZLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TZLibCompImpl,TObject)




// ---------------------------------------------------------------------------
//  CLASS: TZLibCompImpl
// PREFIX: zlib
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TZLibCompImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TZLibCompImpl::TZLibCompImpl(const  tCIDZLib::ECompLevels   eLevel
                            , const tCIDZLib_::EStrategies  eStrategy) :
    m_bEndOfInput(kCIDLib::True)
    , m_bInitialized(kCIDLib::False)
    , m_c2BitBuf(0)
    , m_c4BitCount(0)
    , m_c2LastEOBLen(0)
    , m_c4Adler(1)
    , m_c4BytesAvail(0)
    , m_c4CurOfs(0)
    , m_c4GoodLen(0)
    , m_c4InputBytes(0)
    , m_c4MaxChainLen(0)
    , m_c4MaxLazyLen(0)
    , m_c4NiceLen(0)
    , m_c4TotalIn(0)
    , m_c4TotalOut(0)
    , m_eCompLevel(eLevel)
    , m_eDataType(tCIDZLib_::EDataTypes::Unknown)
    , m_eStrategy(eStrategy)
    , m_pc1LLAccum(nullptr)
    , m_pc1WndBuf(nullptr)
    , m_pc2DistAccum(nullptr)
    , m_pc2HashPrev(nullptr)
    , m_pc2HashTbl(nullptr)
    , m_tdDynBitLen
      (
        (kCIDZLib_::c4BitLenCodes * 2) + 1
        , &s_stdBitLen
      )
    , m_tdDynDist
      (
        (kCIDZLib_::c4DistCodes * 2) + 1
        , &s_stdDist
      )
    , m_tdDynLens
      (
        kCIDZLib_::c4HeapSz
        , &s_stdLens
      )
{
    // Set the tuning parameters that are controlled by the compression level
    const tCIDLib::TCard4 c4TblIndex = tCIDLib::c4EnumOrd(eLevel);
    m_c4GoodLen = kCIDZLib_::aStratTable[c4TblIndex].c4GoodLen;
    m_c4MaxChainLen = kCIDZLib_::aStratTable[c4TblIndex].c4MaxChainLen;
    m_c4MaxLazyLen = kCIDZLib_::aStratTable[c4TblIndex].c4MaxLazyLen;
    m_c4NiceLen = kCIDZLib_::aStratTable[c4TblIndex].c4NiceLen;

    // Allocate our major buffers
    m_pc1LLAccum   = new tCIDLib::TCard1[kCIDZLib_::c4WndSz];
    m_pc1WndBuf    = new tCIDLib::TCard1[kCIDZLib_::c4WndBufSz];
    m_pc2HashPrev  = new tCIDLib::TCard2[kCIDZLib_::c4HashSz];
    m_pc2HashTbl   = new tCIDLib::TCard2[kCIDZLib_::c4HashSz];
    m_pc2DistAccum = new tCIDLib::TCard2[kCIDZLib_::c4WndSz];

}

TZLibCompImpl::~TZLibCompImpl()
{
    // Release all our major buffesr
    delete [] m_pc1WndBuf;
    delete [] m_pc2HashPrev;
    delete [] m_pc2HashTbl;
    delete [] m_pc1LLAccum;
    delete [] m_pc2DistAccum;
}


// ---------------------------------------------------------------------------
//  TZLibCompImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TZLibCompImpl::c4Compress(          TBinInStream&   strmInput
                            ,       TBinOutStream&  strmOutput
                            , const tCIDLib::TCard4 c4InputBytes)
{
    // Set the mode
    m_eMode = tCIDZLib_::EModes::Compress;

    // Store the stream info for use by the various methods
    m_pstrmIn = &strmInput;
    m_pstrmOut = &strmOutput;
    m_c4InputBytes = c4InputBytes;

    // Reset for a new compression run and then do it
    Reset();
    Deflate(tCIDZLib_::ECompMethods::Deflated);
    strmOutput.Flush();
    return m_c4TotalOut;
}


tCIDLib::TCard4
TZLibCompImpl::c4Decompress(        TBinInStream&   strmInput
                            ,       TBinOutStream&  strmOutput
                            , const tCIDLib::TCard4 c4InputBytes)
{
    // Set the mode
    m_eMode = tCIDZLib_::EModes::Decompress;

    // Store the stream info for use by the various methods
    m_pstrmIn = &strmInput;
    m_pstrmOut = &strmOutput;
    m_c4InputBytes = c4InputBytes;

    // Reset for a new decompression run, and then do it
    Reset();

    // Indicate no dictionary
    Inflate(kCIDLib::False);
    strmOutput.Flush();
    return m_c4TotalOut;
}



// ---------------------------------------------------------------------------
//  TZLibCompImpl: Private, static data members
// ---------------------------------------------------------------------------
const TStaticZTreeDescr TZLibCompImpl::s_stdBitLen
(
    kCIDZLib_::c4MaxLenBits
    , 0
    , kCIDZLib_::ac1ExtraLenBits
    , kCIDZLib_::c4BitLenCodes
    , kCIDZLib_::c4BitLenCodes
);

const TStaticZTreeDescr TZLibCompImpl::s_stdDist
(
    kCIDZLib_::c4MaxBits
    , 0
    , kCIDZLib_::ac1ExtraDBits
    , kCIDZLib_::c4DistCodes
    , atvStaticDTree
    , tCIDLib::c4ArrayElems(atvStaticDTree)
    , kCIDZLib_::c4DistCodes
);

const TStaticZTreeDescr TZLibCompImpl::s_stdLens
(
    kCIDZLib_::c4MaxBits
    , kCIDZLib_::c4Literals + 1
    , kCIDZLib_::ac1ExtraLBits
    , kCIDZLib_::c4LengthCodes
    , atvStaticLTree
    , tCIDLib::c4ArrayElems(atvStaticLTree)
    , kCIDZLib_::c4LLCodes
);


// ---------------------------------------------------------------------------
//  TZLibCompImpl: Private, non-virtual methods
// ---------------------------------------------------------------------------

// Returns the next byte from the read buffer
tCIDLib::TCard4
TZLibCompImpl::c4ReadBuf(       tCIDLib::TCard1* const  pc1ToFill
                        , const tCIDLib::TCard4         c4ReadUpTo)
{
    // If already at the end, then return zero
    if (m_bEndOfInput)
        return 0;

    //
    //  We can either read to the end of stream, or up to a count given to
    //  us by the client code.
    //
    tCIDLib::TCard4 c4Actual = c4ReadUpTo;
    if (m_c4InputBytes == kCIDLib::c4MaxCard)
    {
        // If we are at the end of stream, we are done
        if (m_pstrmIn->bEndOfStream())
        {
            c4Actual = 0;
            m_bEndOfInput = kCIDLib::True;
        }
    }
     else
    {
        // Don't read further than the input bytes we were told to
        if (m_c4TotalIn + c4ReadUpTo > m_c4InputBytes)
            c4Actual = m_c4InputBytes - m_c4TotalIn;
    }

    // Read up to the requested bytes
    tCIDLib::TCard4 c4Ret = 0;
    if (c4Actual)
    {
        c4Ret = m_pstrmIn->c4ReadRawBuffer
        (
            pc1ToFill
            , c4Actual
            , tCIDLib::EAllData::OkIfNotAll
        );
    }

    //
    //  If we got more data, so update the adler hash which is kept for
    //  the whole input stream, and stored at the end so that the decomp
    //  can validate the resulting decompressed data. And bump the count
    //  of bytes eaten.
    //
    if (c4Ret)
    {
        m_c4Adler = TRawMem::hshHashBufferAdler32(m_c4Adler, pc1ToFill, c4Ret);
        m_c4TotalIn += c4Ret;
    }

    //
    //  If we got no bytes, or the total input bytes read in so far is equal
    //  to how many we were told to read, then set the end of input flag.
    //
    if (!c4Ret || (m_c4TotalIn == m_c4InputBytes))
        m_bEndOfInput = kCIDLib::True;

    return c4Ret;
}


