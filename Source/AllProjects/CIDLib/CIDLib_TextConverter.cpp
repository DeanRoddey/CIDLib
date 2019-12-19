//
// FILE NAME: CIDLib_TextConverter.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/17/1999
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
//  This file implements the TTextConverter class, the base class from which
//  all converters are derived.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TTextConverter,TObject)


// ---------------------------------------------------------------------------
//  TTextConverter: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  In this case we get in and out streams, so we just read blocks from the
//  input stream, convert them, and write them to the output stream.
//
tCIDLib::TCard4
TTextConverter::c4ConvertFrom(  TBinInStream&       strmSrc
                                , TTextOutStream&   strmTar
                                , tCIDLib::TCard4&  c4OutChars)
{
    //
    //  We use local byte and character buffers to convert chunks at a
    //  time until the source stream runs out of data.
    //
    const tCIDLib::TCard4   c4InBufSz = 4096;
    const tCIDLib::TCard4   c4OutBufSz = 2048;
    tCIDLib::TCard1         ac1SrcBuf[c4InBufSz];
    tCIDLib::TCh            achOutBuf[c4OutBufSz + 1];
    tCIDLib::TBoolean       bStop = kCIDLib::False;
    tCIDLib::TCard4         c4CurChars;
    tCIDLib::TCard4         c4BytesEaten = 0;
    while (!strmSrc.bEndOfStream())
    {
        const tCIDLib::TCard4 c4SrcBytes = strmSrc.c4ReadRawBuffer
        (
            ac1SrcBuf, c4InBufSz, tCIDLib::EAllData::OkIfNotAll
        );

        const tCIDLib::TCard4 c4CurBytes = c4BlockFrom
        (
            ac1SrcBuf, c4SrcBytes, achOutBuf, c4OutBufSz, c4CurChars, bStop
        );

        // Update the number of bytes we've eaten
        c4BytesEaten += c4CurBytes;

        //
        //  If we didn't eat all the bytes this time, push the rest back for the
        //  next time around.
        //
        if (c4CurBytes < c4SrcBytes)
            strmSrc.c4Pushback(&ac1SrcBuf[c4CurBytes], c4SrcBytes - c4CurBytes);

        //  If we did any chars, then put those in the target.
        if (c4CurChars)
            strmTar.WriteChars(achOutBuf, c4CurChars);

        // If we were asked to stop or got no chars, then break out
        if (bStop || !c4CurChars)
            break;
    }

    // Make sure it's flushed out to the target
    strmTar.Flush();

    // Return the bytes eaten
    return c4BytesEaten;
}


//
//  This one is just a straight callthrough to the protected, virtual block
//  transcode method that each derivative implements.
//
tCIDLib::TCard4
TTextConverter::c4ConvertFrom(  const   tCIDLib::TCard1* const  pc1Src
                                , const tCIDLib::TCard4         c4SrcBytes
                                ,       tCIDLib::TCh* const     pszToFill
                                , const tCIDLib::TCard4         c4MaxChars
                                ,       tCIDLib::TCard4&        c4OutChars)
{
    tCIDLib::TBoolean bDummy;
    return c4BlockFrom
    (
        pc1Src, c4SrcBytes, pszToFill, c4MaxChars, c4OutChars, bDummy
    );
}


tCIDLib::TCard4
TTextConverter::c4ConvertFrom(  const   tCIDLib::TCard1* const  pc1Src
                                , const tCIDLib::TCard4         c4SrcBytes
                                ,       TString&                strToFill
                                , const tCIDLib::EAppendOver    eAppend)
{
    // Clear the output string if not appending
    if (eAppend == tCIDLib::EAppendOver::Overwrite)
        strToFill.Clear();

    //
    //  We just use a local TCh buffer, and convert into that buffer using
    //  the protected since block converter method. For each round, we just
    //  append them to the passed string object, then use it again until
    //  we've filled the buffer or used up the source.
    //
    const tCIDLib::TCard4   c4BufSz = 2048;
    tCIDLib::TCh            achBuf[c4BufSz + 1];
    tCIDLib::TBoolean       bStop = kCIDLib::False;
    tCIDLib::TCard4         c4CurChars;
    tCIDLib::TCard4         c4SrcDone = 0;
    while (c4SrcDone < c4SrcBytes)
    {
        // Convert up to another full local buffer of chars
        const tCIDLib::TCard4 c4CurBytes = c4BlockFrom
        (
            &pc1Src[c4SrcDone]
            , c4SrcBytes - c4SrcDone
            , achBuf
            , c4BufSz
            , c4CurChars
            , bStop
        );

        // Update the number of bytes we've eaten
        c4SrcDone += c4CurBytes;

        // If we did any chars, then put those in the target
        if (c4CurChars)
            strToFill.AppendSubStr(achBuf, 0, c4CurChars);

        // If we got no chars or we were asked to stop, then break out
        if (!c4CurChars || bStop)
            break;
    }

    // Return the bytes eaten
    return c4SrcDone;
}


// We call the other version and pass along the raw buffer
tCIDLib::TCard4
TTextConverter::c4ConvertFrom(  const   TMemBuf&                mbufSrc
                                , const tCIDLib::TCard4         c4SrcBytes
                                ,       TString&                strToFill
                                , const tCIDLib::EAppendOver    eAppend)
{
    if (c4SrcBytes > mbufSrc.c4Size())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcMBuf_RangeTop
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TInteger(0)
            , TCardinal(c4SrcBytes)
            , TCardinal(mbufSrc.c4Size())
        );
    }
    return c4ConvertFrom(mbufSrc.pc1Data(), c4SrcBytes, strToFill, eAppend);
}


tCIDLib::TCard4
TTextConverter::c4ConvertTo(TTextInStream&      strmSrc
                            , TBinOutStream&    strmTar
                            , tCIDLib::TCard4&  c4OutBytes)
{
    const tCIDLib::TCard4   c4InBufSz = 2048;
    tCIDLib::TCh            achInBuf[c4InBufSz];
    const tCIDLib::TCard4   c4OutBufSz = c4InBufSz * kCIDLib::c4MaxUTF8Bytes;
    tCIDLib::TCard1         ac1OutBuf[c4OutBufSz];
    tCIDLib::TBoolean       bStop = kCIDLib::False;
    tCIDLib::TCard4         c4SrcDone = 0;

    c4OutBytes = 0;
    while (!strmSrc.bEndOfStream())
    {
        const tCIDLib::TCard4 c4SrcChars = strmSrc.c4ReadChars(achInBuf, c4InBufSz);
        CIDAssert(c4SrcChars != 0, L"Got no chars but stream is not at end");

        // Convert this block to bytes
        tCIDLib::TCard4 c4CurBytes;
        const tCIDLib::TCard4 c4CurChars = c4BlockTo
        (
            achInBuf
            , c4SrcChars
            , ac1OutBuf
            , c4OutBufSz
            , c4CurBytes
            , bStop
        );
        CIDAssert(c4CurBytes != 0, L"No source chars were converter to target encoding");

        // Update the number of chars we've eaten and write to the output
        c4SrcDone += c4CurChars;

        //  If we got any bytes, then write them to the output stream
        if (c4CurBytes)
        {
            strmTar.c4WriteRawBuffer(ac1OutBuf, c4CurBytes);
            c4OutBytes += c4CurBytes;
        }

        // If we were asked to stop or got no bytes, then break out
        if (bStop || !c4CurBytes)
            break;
    }

    // Make sure the stream is flushed
    strmTar.Flush();

    // Return the chars we ate
    return c4SrcDone;
}


//
//  This one is just a straight callthrough to the protected, virtual block
//  transcode method that each derivative implements.
//
tCIDLib::TCard4
TTextConverter::c4ConvertTo(const   tCIDLib::TCh* const     pszSrc
                            , const tCIDLib::TCard4         c4SrcChars
                            ,       tCIDLib::TCard1* const  pc1ToFill
                            , const tCIDLib::TCard4         c4MaxBytes
                            ,       tCIDLib::TCard4&        c4OutBytes)
{
    tCIDLib::TBoolean bDummy;
    return c4BlockTo(pszSrc, c4SrcChars, pc1ToFill, c4MaxBytes, c4OutBytes, bDummy);
}


tCIDLib::TCard4
TTextConverter::c4ConvertTo(const   tCIDLib::TCh* const pszSrc
                            , const tCIDLib::TCard4     c4SrcChars
                            ,       TExpByteBuf&        expbToFill)
{
    // Clear the output buffer
    expbToFill.Reset();

    const tCIDLib::TCard4   c4BufSz = 2048;
    tCIDLib::TCard1         ac1Buf[c4BufSz];
    tCIDLib::TBoolean       bStop = kCIDLib::False;
    tCIDLib::TCard4         c4CurBytes;
    tCIDLib::TCard4         c4SrcDone = 0;
    while (c4SrcDone < c4SrcChars)
    {
        // Convert up to another full local buffer of bytes
        const tCIDLib::TCard4 c4CurChars = c4BlockTo
        (
            &pszSrc[c4SrcDone]
            , c4SrcChars - c4SrcDone
            , ac1Buf
            , c4BufSz
            , c4CurBytes
            , bStop
        );

        // Update the number of chars we've eaten
        c4SrcDone += c4CurChars;

        // If we got some bytes, add to the buffer
        if (c4CurBytes)
            expbToFill.Append(ac1Buf, c4CurBytes);

        // If we got no bytes or were asked to stop, then break out
        if (!c4CurBytes || bStop)
            break;
    }

    // Return the chars we ate
    return c4SrcDone;
}


tCIDLib::TCard4
TTextConverter::c4ConvertTo(const   tCIDLib::TCh* const     pszSrc
                            , const tCIDLib::TCard4         c4SrcChars
                            ,       TMemBuf&                mbufToFill
                            ,       tCIDLib::TCard4&        c4OutBytes)
{
    const tCIDLib::TCard4   c4BufSz = 2048;
    tCIDLib::TCard1         ac1Buf[c4BufSz];
    tCIDLib::TBoolean       bStop = kCIDLib::False;
    tCIDLib::TCard4         c4CurBytes;
    tCIDLib::TCard4         c4SrcDone = 0;

    c4OutBytes = 0;
    while (c4SrcDone < c4SrcChars)
    {
        // Convert up to another full local buffer of bytes
        const tCIDLib::TCard4 c4CurChars = c4BlockTo
        (
            &pszSrc[c4SrcDone]
            , c4SrcChars - c4SrcDone
            , ac1Buf
            , c4BufSz
            , c4CurBytes
            , bStop
        );

        // Update the number of chars we've eaten
        c4SrcDone += c4CurChars;

        //
        //  If we got any bytes, then append them to the buffer and up
        //  the output bytes count.
        //
        if (c4CurBytes)
        {
            mbufToFill.CopyIn(ac1Buf, c4CurBytes, c4OutBytes);
            c4OutBytes += c4CurBytes;
        }

        // If we got no bytes or were asked to stop, then break out
        if (!c4CurBytes || bStop)
            break;
    }

    // Return the chars we ate
    return c4SrcDone;
}


// Just call the version above, breaking out the raw buffer and length
tCIDLib::TCard4
TTextConverter::c4ConvertTo(const   TString&            strSrc
                            ,       TMemBuf&            mbufToFill
                            ,       tCIDLib::TCard4&    c4OutBytes)
{
    return c4ConvertTo
    (
        strSrc.pszBuffer(), strSrc.c4Length(), mbufToFill, c4OutBytes
    );
}


//
//  Convert one char to the target encoding. We return the number of bytes
//  that we spit out.
//
tCIDLib::TCard4
TTextConverter::c4ConvertOneTo( const   tCIDLib::TCh            chSrc
                                ,       tCIDLib::TCard1* const  pc1ToFill
                                , const tCIDLib::TCard4         c4MaxBytes)
{
    tCIDLib::TBoolean bDummy;
    tCIDLib::TCard4 c4SrcDone;
    const tCIDLib::TCard4 c4Bytes = c4BlockTo
    (
        &chSrc, 1, pc1ToFill, c4MaxBytes, c4SrcDone, bDummy
    );

    if (!c4Bytes || !c4SrcDone)
        return 0;

    // Return the bytes we formatted out
    return c4Bytes;
}


//
//  Convert one character from an input source buffer. We return the char
//  nad give back the number of bytes we ate.
//
tCIDLib::TCh
TTextConverter::chConvertOneFrom(const  tCIDLib::TCard1* const  pc1Src
                                , const tCIDLib::TCard4         c4SrcBytes
                                ,       tCIDLib::TCard4&        c4OutBytes)
{
    tCIDLib::TBoolean   bDummy = kCIDLib::False;
    tCIDLib::TCh        szTmp[2];

    if (!c4BlockFrom(pc1Src, c4SrcBytes, szTmp, 1, c4OutBytes, bDummy))
        return kCIDLib::chNull;

    // Return the one char we did
    return szTmp[0];
}


// Get or set the replacement character
tCIDLib::TCh TTextConverter::chRepChar() const
{
    return m_chRepChar;
}

tCIDLib::TCh TTextConverter::chRepChar(const tCIDLib::TCh chToSet)
{
    m_chRepChar = chToSet;
    return m_chRepChar;
}


//
//  Get or set the error action to take when bad data is seen during
//  internalization.
//
tCIDLib::ETCvtActs TTextConverter::eErrorAction() const
{
    return m_eErrAction;
}

tCIDLib::ETCvtActs TTextConverter::eErrorAction(const tCIDLib::ETCvtActs eAction)
{
    m_eErrAction = eAction;
    return m_eErrAction;
}


// Get the encoding name that was set in the ctor
const TString& TTextConverter::strEncodingName() const
{
    return m_strEncodingName;
}


// ---------------------------------------------------------------------------
//  TTextConverter: Hidden constructors and operators
// ---------------------------------------------------------------------------
TTextConverter::TTextConverter(const TString& strEncodingName) :

    m_chRepChar(kCIDLib::chSpace)
    , m_eErrAction(tCIDLib::ETCvtActs::Throw)
    , m_strEncodingName(strEncodingName)
{
}


// ---------------------------------------------------------------------------
//  TTextConverter: Protected, non-virtual methods
// ---------------------------------------------------------------------------
const TString& TTextConverter::strEncodingName(const TString& strNew)
{
    m_strEncodingName = strNew;
    return m_strEncodingName;
}


