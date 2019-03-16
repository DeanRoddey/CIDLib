//
// FILE NAME: CIDLib_TextInStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/08/1999
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
//  This file implements the TTextInStream class, which provides text mode
//  input over a binary stream.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macros
// ---------------------------------------------------------------------------
RTTIDecls(TTextInStream,TObject)


// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------
static const tCIDLib::TCh* pszBool(const tCIDLib::TBoolean bVal)
{
    static volatile tCIDLib::TBoolean   bInitDone  = kCIDLib::False;
    static const tCIDLib::TCh*          pszFalse   = kCIDLib_::pszFalse;
    static const tCIDLib::TCh*          pszTrue    = kCIDLib_::pszTrue;

    if (!bInitDone)
    {
        TBaseLock lockInit;
        if (!bInitDone)
        {
            const tCIDLib::TCh* pszTmp;
            pszTmp = facCIDLib().pszLoadCIDMsg(kCIDMsgs::midGen_False);
            if (pszTmp)
                pszFalse = pszTmp;

            pszTmp = facCIDLib().pszLoadCIDMsg(kCIDMsgs::midGen_True);
            if (pszTmp)
                pszTrue = TRawStr::pszReplicate(pszTmp);

            bInitDone = kCIDLib::True;
        }
    }
    if (bVal)
        return pszTrue;
    return pszFalse;
}



// ---------------------------------------------------------------------------
//   CLASS: TTextInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTextInStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TTextInStream::TTextInStream(TBinInStream* const        pstrmToAdopt
                            ,   TTextConverter* const   ptcvtToAdopt) :
    m_c4CacheOfs(0)
    , m_c4CacheSize(0)
    , m_c4SpareBytes(0)
    , m_chUnGet(0)
    , m_eNewLineType(tCIDLib::ENewLineTypes::CRLF)
    , m_pstrmIn(pstrmToAdopt)
    , m_ptcvtThis(ptcvtToAdopt)
{
}

TTextInStream::~TTextInStream()
{
    delete m_pstrmIn;
    delete m_ptcvtThis;
}



// ---------------------------------------------------------------------------
//  TTextInStream: Public operators
// ---------------------------------------------------------------------------
TTextInStream& TTextInStream::operator>>(tCIDLib::TBoolean& bToFill)
{
    // Get the next line into a local buffer
    bToFill = bRead();
    return *this;
}

TTextInStream& TTextInStream::operator>>(tCIDLib::TCard1& c1ToFill)
{
    c1ToFill = c1Read(tCIDLib::ERadices::Dec);
    return *this;
}

TTextInStream& TTextInStream::operator>>(tCIDLib::TCard2& c2ToFill)
{
    c2ToFill = c2Read(tCIDLib::ERadices::Dec);
    return *this;
}

TTextInStream& TTextInStream::operator>>(tCIDLib::TCard4& c4ToFill)
{
    c4ToFill = c4Read(tCIDLib::ERadices::Dec);
    return *this;
}

TTextInStream& TTextInStream::operator>>(tCIDLib::TCard8& c8ToFill)
{
    c8ToFill = c8Read(tCIDLib::ERadices::Dec);
    return *this;
}

TTextInStream& TTextInStream::operator>>(tCIDLib::TFloat4& f4ToFill)
{
    f4ToFill = f4Read();
    return *this;
}

TTextInStream& TTextInStream::operator>>(tCIDLib::TFloat8& f8ToFill)
{
    f8ToFill = f8Read();
    return *this;
}

TTextInStream& TTextInStream::operator>>(tCIDLib::TInt1& i1ToFill)
{
    i1ToFill = i1Read(tCIDLib::ERadices::Dec);
    return *this;
}

TTextInStream& TTextInStream::operator>>(tCIDLib::TInt2& i2ToFill)
{
    i2ToFill = i2Read(tCIDLib::ERadices::Dec);
    return *this;
}

TTextInStream& TTextInStream::operator>>(tCIDLib::TInt4& i4ToFill)
{
    i4ToFill = i4Read(tCIDLib::ERadices::Dec);
    return *this;
}

TTextInStream& TTextInStream::operator>>(tCIDLib::TInt8& i8ToFill)
{
    i8ToFill = i8Read(tCIDLib::ERadices::Dec);
    return *this;
}

TTextInStream& TTextInStream::operator>>(tCIDLib::TUInt& uToFill)
{
    uToFill = c4Read(tCIDLib::ERadices::Dec);
    return *this;
}

TTextInStream& TTextInStream::operator>>(tCIDLib::TSInt& iToFill)
{
    iToFill = i4Read(tCIDLib::ERadices::Dec);
    return *this;
}

TTextInStream& TTextInStream::operator>>(tCIDLib::TCh& chToFill)
{
    // Tell it to throw if we can't read one
    chToFill = chNextChar(kCIDLib::True);
    return *this;
}

TTextInStream& TTextInStream::operator>>(TString& strToFill)
{
    c4GetLine(strToFill, 0, kCIDLib::False);
    return *this;
}


// ---------------------------------------------------------------------------
//  TTextInStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TTextInStream::bDataInCache() const
{
    if ((m_c4CacheOfs < m_c4CacheSize) || m_chUnGet)
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean TTextInStream::bEndOfStream() const
{
    // If we have data in our cache, then obviously not
    if (bDataInCache())
        return kCIDLib::False;

    //
    //  No data there, try to reload the cache. If that fails, then we are
    //  at the end.
    //
    if (!bReloadCache())
        return kCIDLib::True;

    // Else we got some more data and aren't at the end
    return kCIDLib::False;
}


tCIDLib::TBoolean TTextInStream::bRead()
{
    tCIDLib::TBoolean   bGotEnd;
    tCIDLib::TCh        szTmp[65];
    if (!c4GetRawBuf(szTmp, 64, bGotEnd))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_EndOfStream
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::UserInput
        );
    }

    // If we didn't get the end, then eat the rest of the line
    if (!bGotEnd)
        EatLineRemainder();

    if (TRawStr::bCompareStrI(szTmp, pszBool(kCIDLib::True))
    ||  TRawStr::bCompareStrI(szTmp, L"1"))
    {
         return kCIDLib::True;
    }

    if (TRawStr::bCompareStrI(szTmp, pszBool(kCIDLib::False))
    ||  TRawStr::bCompareStrI(szTmp, L"0"))
    {
        return kCIDLib::False;
    }

    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcStr_TextConvert
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::UserInput
        , TString(szTmp)
        , TString(L"TBoolean")
    );

    // Make compiler happy
    return kCIDLib::False;
}

tCIDLib::TCard4
TTextInStream::c4GetLine(       TString&            strToFill
                        , const tCIDLib::TCard4     c4MaxChars
                        , const tCIDLib::TBoolean   bStripWhitespace)
{
    //
    //  If the max chars is zero, then set it to c4MaxCard so that there is
    //  really no limit.
    //
    tCIDLib::TCard4 c4CountDown = c4MaxChars ? c4MaxChars : kCIDLib::c4MaxCard;

    // Empty the target string
    strToFill.Clear();

    //
    //  Use a local buffer to get text, to be more efficient. We keep loading
    //  up the local buffer until we've got a new line, hit the end of the
    //  stream, got as many chars as needed.
    //
    const tCIDLib::TCard4 c4BufSz = 4096;
    tCIDLib::TCh szTmp[c4BufSz + 1];
    tCIDLib::TBoolean bGotEnd = kCIDLib::False;
    while (c4CountDown)
    {
        //
        //  One each round, get up to either a full buffer's worth or as
        //  much as is required to handle what's left.
        //
        const tCIDLib::TCard4 c4ToGet = (c4BufSz < c4CountDown) ?
                                        c4BufSz : c4CountDown;

        // And get up to that many chars
        const tCIDLib::TCard4 c4Got = c4GetRawBuf(szTmp, c4ToGet, bGotEnd);

        // If we didn't get anything, then break out
        if (!c4Got)
            break;

        // Else, append what we got to the target char
        strToFill.Append(szTmp);

        // If we got the end, then break out
        if (bGotEnd)
            break;

        // Subtract what we got from the countdown
        c4CountDown -= c4Got;
    }

    //
    //  If we broke out without getting the end, then we didn't have enough
    //  chars to eat the line. So eat the line remainder.
    //
    if (!bGotEnd)
        EatLineRemainder();

    if (bStripWhitespace)
        strToFill.StripWhitespace();
    return strToFill.c4Length();
}


//
//  Allows for one character of read ahead. We use the same mechanism that
//  we use internally for single character unget. They can tell us to throw
//  if we are at the end of stream, or just return a null.
//
tCIDLib::TCh TTextInStream::chPeek(const tCIDLib::TBoolean bThrowIfEOS)
{
    // If our unget char is non-zero, then that's what we return
    if (m_chUnGet)
        return m_chUnGet;

    // Else we need to get one and then unget it
    const tCIDLib::TCh chRet = chNextChar(bThrowIfEOS);
    m_chUnGet = chRet;

    return chRet;
}


tCIDLib::TCh TTextInStream::chRead()
{
    // Tell it to throw if we can't read one
    return chNextChar(kCIDLib::True);
}


tCIDLib::TCard1 TTextInStream::c1Read(const tCIDLib::ERadices eRadix)
{
    TString strInput(64UL);
    if (!c4GetLine(strInput, 64, kCIDLib::True))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_EndOfStream
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::UserInput
        );
    }

    tCIDLib::TCard4 c4Value = strInput.c4Val(eRadix);
    if (c4Value > kCIDLib::c1MaxCard)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTStrm_InputRange
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::UserInput
            , TCardinal(c4Value)
            , TString(L"TCard1")
        );
    }
    return tCIDLib::TCard1(c4Value);
}

tCIDLib::TCard2 TTextInStream::c2Read(const tCIDLib::ERadices eRadix)
{
    TString strInput(64UL);
    if (!c4GetLine(strInput, 64, kCIDLib::True))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_EndOfStream
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::UserInput
        );
    }

    tCIDLib::TCard4 c4Value = strInput.c4Val(eRadix);

    if (c4Value > kCIDLib::c2MaxCard)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTStrm_InputRange
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Value)
            , TString(L"TCard2")
        );
    }
    return tCIDLib::TCard2(c4Value);
}

tCIDLib::TCard4 TTextInStream::c4Read(const tCIDLib::ERadices eRadix)
{
    TString strInput(64UL);
    if (!c4GetLine(strInput, 64, kCIDLib::True))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_EndOfStream
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::UserInput
        );
    }
    return strInput.c4Val(eRadix);
}

tCIDLib::TCard8 TTextInStream::c8Read(const tCIDLib::ERadices eRadix)
{
    TString strInput(128UL);
    if (!c4GetLine(strInput, 128, kCIDLib::True))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_EndOfStream
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::UserInput
        );
    }
    return strInput.c8Val(eRadix);
}


tCIDLib::ENewLineTypes TTextInStream::eNewLineType() const
{
    return m_eNewLineType;
}

tCIDLib::ENewLineTypes
TTextInStream::eNewLineType(const tCIDLib::ENewLineTypes eToSet)
{
    m_eNewLineType = eToSet;
    return m_eNewLineType;
}


tCIDLib::TFloat4 TTextInStream::f4Read()
{
    TString strInput(64UL);
    if (!c4GetLine(strInput, 64, kCIDLib::True))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_EndOfStream
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::UserInput
        );
    }

    tCIDLib::TFloat8 f8Value = strInput.f8Val();

    if ((f8Value < kCIDLib::f4MinFloat) || (f8Value > kCIDLib::f4MaxFloat))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTStrm_InputRange
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::UserInput
            , TFloat(f8Value)
            , TString(L"TFloat4")
        );
    }
    return tCIDLib::TFloat4(f8Value);
}

tCIDLib::TFloat8 TTextInStream::f8Read()
{
    TString strInput(64UL);
    if (!c4GetLine(strInput, 64, kCIDLib::True))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_EndOfStream
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::UserInput
        );
    }
    return strInput.f8Val();
}

tCIDLib::TInt1 TTextInStream::i1Read(const tCIDLib::ERadices eRadix)
{
    TString strInput(64UL);
    if (!c4GetLine(strInput, 64, kCIDLib::True))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_EndOfStream
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::UserInput
        );
    }

    tCIDLib::TInt4 i4Value = strInput.i4Val(eRadix);

    if ((i4Value < kCIDLib::i1MinInt)
    ||  (i4Value > kCIDLib::i1MaxInt))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTStrm_InputRange
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::UserInput
            , TInteger(i4Value)
            , TString(L"TInt1")
        );
    }
    return tCIDLib::TInt1(i4Value);
}

tCIDLib::TInt2 TTextInStream::i2Read(const tCIDLib::ERadices eRadix)
{
    TString strInput(64UL);
    if (!c4GetLine(strInput, 64, kCIDLib::True))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_EndOfStream
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::UserInput
        );
    }

    tCIDLib::TInt4 i4Value = strInput.i4Val(eRadix);

    if ((i4Value < kCIDLib::i2MinInt)
    ||  (i4Value > kCIDLib::i2MaxInt))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTStrm_InputRange
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::UserInput
            , TInteger(i4Value)
            , TString(L"TInt2")
        );
    }
    return tCIDLib::TInt2(i4Value);
}

tCIDLib::TInt4 TTextInStream::i4Read(const tCIDLib::ERadices eRadix)
{
    TString strInput(64UL);
    if (!c4GetLine(strInput, 64, kCIDLib::True))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_EndOfStream
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::UserInput
        );
    }
    return strInput.i4Val(eRadix);
}

tCIDLib::TInt8 TTextInStream::i8Read(const tCIDLib::ERadices eRadix)
{
    TString strInput(128UL);
    if (!c4GetLine(strInput, 128, kCIDLib::True))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_EndOfStream
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::UserInput
        );
    }
    return strInput.i8Val(eRadix);
}


tCIDLib::TVoid TTextInStream::Reset()
{
    // Reset all of our fields
    m_c4CacheOfs = 0;
    m_c4CacheSize = 0;
    m_c4SpareBytes = 0;
    m_chUnGet = 0;

    // And reset the implementation amd the text converter if any
    m_pstrmIn->Reset();

    // And reset the text converter if any
    if (m_ptcvtThis)
        m_ptcvtThis->Reset();
}


const TString& TTextInStream::strEncodingName() const
{
    return m_ptcvtThis->strEncodingName();
}



TTextConverter& TTextInStream::tcvtThis()
{
    return *m_ptcvtThis;
}

const TTextConverter& TTextInStream::tcvtThis() const
{
    return *m_ptcvtThis;
}


// ---------------------------------------------------------------------------
//  TTextInStream: Hidden Constructors
// ---------------------------------------------------------------------------
TTextInStream::TTextInStream(TTextConverter* const ptcvtToAdopt) :

    m_c4CacheOfs(0)
    , m_c4CacheSize(0)
    , m_c4SpareBytes(0)
    , m_chUnGet(0)
    , m_eNewLineType(tCIDLib::ENewLineTypes::CRLF)
    , m_pstrmIn(0)
    , m_ptcvtThis(ptcvtToAdopt)
{
    // The stream must be set later, before use of this stream
}


// ---------------------------------------------------------------------------
//  TTextInStream: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TTextInStream::AdoptStream(TBinInStream* const pstrmToAdopt)
{
    // If the stream is already set, then that's an error
    if (m_pstrmIn)
    {
        // Clean up passed stream since we are responsible for it
        delete pstrmToAdopt;

        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTStrm_StrmAlreadySet
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , clsIsA()
        );
    }

    // We don't already have a stream so store it away
    m_pstrmIn = pstrmToAdopt;
}


// ---------------------------------------------------------------------------
//  TTextInStream: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  All of the cache members are mutable and this method is const, because
//  it is sometimes required to call this during methods that are theoretically
//  const.
//
tCIDLib::TBoolean TTextInStream::bReloadCache() const
{
    //
    //  Read into a temporary buffer, since we have to convert it before
    //  we can put it into our cache.
    //
    //  Note that its the same count of bytes as chars in the cache. This
    //  insure that, worst case, we get c4CacheBufChars ASCII chars into
    //  this temp buf. So we will never read more bytes than we can get
    //  into the cache.
    //
    tCIDLib::TCard1 ac1Tmp[c4CacheBufChars];

    //
    //  First though, we have to copy over any spare bytes left over from
    //  the previous load.
    //
    if (m_c4SpareBytes)
        TRawMem::CopyMemBuf(ac1Tmp, m_ac1SpareBytes, m_c4SpareBytes);

    //
    //  Read a temp buffer's worth, starting past the spare bytes we
    //  copied in above. Add the spare bytes back to the bytes read
    //  to get the actual bytes we got in the temp buffer now.
    //
    const tCIDLib::TCard4 c4BytesRead = m_pstrmIn->c4ReadRawBuffer
    (
        &ac1Tmp[m_c4SpareBytes]
        , c4CacheBufChars - m_c4SpareBytes
        , tCIDLib::EAllData::OkIfNotAll
    ) + m_c4SpareBytes;

    // We can zero out the spare bytes count now
    m_c4SpareBytes = 0;

    //
    //  If we did not get any text, then clear out our cache pointers amd
    //  return a failure.
    //
    if (!c4BytesRead)
    {
        m_c4CacheOfs = 0;
        m_c4CacheSize = 0;
        return kCIDLib::False;
    }

    //
    //  If we don't have a converter yet, then none was provided and we need
    //  to try to create one.
    //
    if (!m_ptcvtThis)
    {
        TUTFConverter::EEncodings eEncoding = TUTFConverter::EEncodings::UTF8;

        //
        //  Look at the first two bytes of the cache. If there aren't two
        //  bytes, then don't bother. If there are, then see if its the BOM
        //  or its flipped version.
        //
        if (c4BytesRead > 1)
        {
            if ((ac1Tmp[0] == 0xFE) && (ac1Tmp[1] == 0xFF))
                eEncoding = TUTFConverter::EEncodings::UTF16_LE;
            else if ((ac1Tmp[0] == 0xFF) && (ac1Tmp[1] == 0xFE))
                eEncoding = TUTFConverter::EEncodings::UTF16_BE;
        }
        m_ptcvtThis = new TUTFConverter(eEncoding);
    }

    //
    //  Ok, lets convert over as many characters as we can from the temp
    //  buffer. We'll never overflow the buffer, since the temp buffer is
    //  the same size as the cache. Worst case, its single byte and it becomes
    //  a one to one map. Any multi/variable byte encoding will create even
    //  fewer output chars to the cache.
    //
    tCIDLib::TCard4 c4BytesConverted;
    c4BytesConverted = m_ptcvtThis->c4ConvertFrom
    (
        ac1Tmp, c4BytesRead, m_achCache, c4CacheBufChars, m_c4CacheSize
    );

    //
    //  If this left any spare bytes over, then we need to get them into
    //  the spare bytes array for the next round.
    //
    m_c4SpareBytes = c4BytesRead - c4BytesConverted;
    if (m_c4SpareBytes)
    {
        if (m_c4SpareBytes > c4SpareByteCnt)
        {
            m_c4SpareBytes = 0;

            //
            //  This should never happen, since it indicates that over our limit
            //  of bytes of data did not get converted. That would mean that the
            //  encoding requires more than that many bytes to encode a character,
            //  which just isn't going to happen.
            //
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcTStrm_TooManySpareBytes
                , tCIDLib::ESeverities::ProcFatal
                , tCIDLib::EErrClasses::Format
            );
        }

        // Copy the bytes over to the spare bytes array
        TRawMem::CopyMemBuf
        (
            m_ac1SpareBytes, &ac1Tmp[c4BytesRead - m_c4SpareBytes], m_c4SpareBytes
        );
    }

    //
    //  Ok, we now have chars in the cache, so reset the current cache
    //  offset so that we start reading at the start of the buffer.
    //
    m_c4CacheOfs = 0;

    // And return success
    return kCIDLib::True;
}


tCIDLib::TCh TTextInStream::chNextChar(const tCIDLib::TBoolean bThrowIfEnd)
{
    // Check the unget buffer first
    if (m_chUnGet)
    {
        const tCIDLib::TCh chRet = m_chUnGet;
        m_chUnGet = kCIDLib::chNull;
        return chRet;
    }

    //
    //  See if we need to do an initial cache fill from the underlying
    //  binary stream.
    //
    if (m_c4CacheOfs == m_c4CacheSize)
    {
        //
        //  If we get nothing, then throw an end of stream exception if they
        //  wanted one. Else just return a null char.
        //
        if (!bReloadCache())
        {
            if (bThrowIfEnd)
            {
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcStrm_EndOfStream
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::OutResource
                );
            }

            return kCIDLib::chNull;
        }
    }

    // Return the next char and bump up the offset
    return m_achCache[m_c4CacheOfs++];
}


tCIDLib::TCard4
TTextInStream::c4GetRawBuf(         tCIDLib::TCh* const pszToFill
                            , const tCIDLib::TCard4     c4MaxChars
                            ,       tCIDLib::TBoolean&  bGotEnd)
{
    // Assume we won't hit the end of line or stream
    bGotEnd = kCIDLib::False;

    // Check for pathological scenario
    if (!c4MaxChars)
        return 0;

    //
    //  Loop until we either get a new line, the end of the stream, or
    //  fill the requested number of chars.
    //
    tCIDLib::TCard4 c4Index = 0;

    tCIDLib::TBoolean bDone = kCIDLib::False;
    while (!bDone)
    {
        // Get the next character from the cache
        const tCIDLib::TCh chCur = chNextChar(kCIDLib::False);

        // Add the new char to the buffer at the current index and bump it
        pszToFill[c4Index++] = chCur;

        //
        //  If we've:
        //
        //  1)  Filled the buffer
        //  2)  Hit the end of the stream
        //  3)  Got a newline
        //
        //  Then cap off the buffer and break out of the loop
        //
        if ((c4Index == c4MaxChars)
        ||  !chCur
        ||  (chCur == kCIDLib::chLF)
        ||  (chCur == kCIDLib::chCR))
        {
            // If its a new line, then back down the index
            if ((chCur == kCIDLib::chLF) || (chCur == kCIDLib::chCR))
                c4Index--;

            //
            //  Cap off the string overwriting the new line character that we
            //  broke out on.
            //
            pszToFill[c4Index] = kCIDLib::chNull;

            //
            //  If this char is a CR, and the new line type is set to
            //  CRLF, then see if the next one is an LF. If so, then
            //  eat the LF.
            //
            if ((chCur == kCIDLib::chCR)
            &&  (m_eNewLineType == tCIDLib::ENewLineTypes::CRLF))
            {
                // Get the next character to check it
                const tCIDLib::TCh chPeek = chNextChar(kCIDLib::False);

                // If its not an kCIDLib::chLF, then unget it
                if (chPeek != kCIDLib::chLF)
                    m_chUnGet = chPeek;
            }

            // If we got out because of hitting the end, then set the flag
            if (!chCur || (chCur == kCIDLib::chLF) || (chCur == kCIDLib::chCR))
                bGotEnd = kCIDLib::True;

            // And break out
            break;
        }
    }
    return c4Index;
}


tCIDLib::TVoid TTextInStream::EatLineRemainder()
{
    // Just loop until we hit the end or get a new line
    tCIDLib::TBoolean bDone = kCIDLib::False;
    while (!bDone)
    {
        const tCIDLib::TCh chEat = chNextChar(kCIDLib::False);

        // Break out if end of input
        if (!chEat)
            break;

        if ((chEat == kCIDLib::chLF) || (chEat == kCIDLib::chCR))
        {
            //
            //  If this char is a CR, and the new line type is set to
            //  CRLF, then see if the next one is an LF. If so, then
            //  eat the LF.
            //
            if ((chEat == kCIDLib::chCR)
            &&  (m_eNewLineType == tCIDLib::ENewLineTypes::CRLF))
            {
                // Get the next character to check it
                const tCIDLib::TCh chPeek = chNextChar(kCIDLib::False);

                // If its not an kCIDLib::chLF, then unget it
                if (chPeek != kCIDLib::chLF)
                    m_chUnGet = chPeek;
            }
            break;
        }
    }
}

