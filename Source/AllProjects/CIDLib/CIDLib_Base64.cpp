//
// FILE NAME: CIDLib_Base64.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/12/2005
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
//  This file implements the TBase64 class, which provides base 64 encoding
//  and decoding.
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
//  Do our standard RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TBase64,TObject)


// ---------------------------------------------------------------------------
//  Local types and constants
// ---------------------------------------------------------------------------
namespace CIDLib_Base64
{
    // -----------------------------------------------------------------------
    //  A table to translate the binary data into base 64 values. These
    //  represent the ASCII chars:
    //
    //    ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/
    //
    //  They are not done as a character string since we don't want to assume
    //  that a locally compiled constant string will generate ASCII codes.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard1 ac1Encode[] =
    {
        // A-Z
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C
      , 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58
      , 0x59, 0x5A

        // a-z
      , 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C
      , 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
      , 0x79, 0x7A

        // 0-9
      , 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39

        // +/
      , 0x2B, 0x2F
    };
    const tCIDLib::TCard4 c4EncodeSz = tCIDLib::c4ArrayElems(ac1Encode);


    // -----------------------------------------------------------------------
    //  And a table for decoding back the other way. Again, as binary bytes
    //  so we don't assume generation of ASCII codes.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard1 ac1Decode[] =
    {
        0x7C, 0x24, 0x24, 0x24, 0x7D, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78
      , 0x79, 0x7A, 0x7B, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x3E, 0x3F
      , 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B
      , 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57
      , 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D
      , 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69
      , 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71
    };
    const tCIDLib::TCard4 c4DecodeSz = tCIDLib::c4ArrayElems(ac1Decode);


    // -----------------------------------------------------------------------
    //  Some Base64 related constants
    // -----------------------------------------------------------------------
    const tCIDLib::TCard4 c4RawBlkSz = 3;
    const tCIDLib::TCard4 c4EncBlkSz = 4;
}



// ---------------------------------------------------------------------------
//   CLASS: TObject
//  PREFIX: b64
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBase64: Constructors and Destructor
// ---------------------------------------------------------------------------
TBase64::TBase64() :

    m_bEscapeForwardSlashes(kCIDLib::False)
    , m_c4LineWidth(72)
{
}


// ---------------------------------------------------------------------------
//  TBase64: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Get or set the forward slash escapement flag
tCIDLib::TBoolean TBase64::bEscapeForwardSlashes() const
{
    return m_bEscapeForwardSlashes;
}

tCIDLib::TBoolean TBase64::bEscapeForwardSlashes(const tCIDLib::TBoolean bToSet)
{
    m_bEscapeForwardSlashes = bToSet;
    return m_bEscapeForwardSlashes;
}



//
//  These decode/encode methods work in terms of binary streams, for those
//  scenarios where the Base64 data is available as a chunk of binary data and
//  can be easily treated as such via a binary stream. This is the most
//  efficient way to do it, but there are versions below that work in terms
//  of text streams for the Base64 side of the equation.
//
tCIDLib::TCard4 TBase64::c4Decode(TBinInStream& strmIn, TBinOutStream& strmOut)
{
    tCIDLib::TCard4 c4BlkCnt = 0;
    tCIDLib::TCard4 c4Count = 0;
    tCIDLib::TCard1 ac1In[CIDLib_Base64::c4EncBlkSz];
    tCIDLib::TCard1 ac1Out[CIDLib_Base64::c4RawBlkSz];

    try
    {
        while (kCIDLib::True)
        {
            // Try to get a block's worth out, tossing junk
            c4BlkCnt = 0;
            while (c4BlkCnt < CIDLib_Base64::c4EncBlkSz)
            {
                //
                //  Loop until we get a non-junk character
                tCIDLib::TCard1 c1Next = 0;
                while (!c1Next)
                {
                    strmIn >> c1Next;
                    if ((c1Next > 42) && (c1Next < 123))
                    {
                        c1Next = CIDLib_Base64::ac1Decode[c1Next - 43];
                        if (c1Next)
                        {
                            if (c1Next == 0x24)
                                c1Next = 0;
                            else
                                c1Next -= 61;
                        }
                    }
                     else
                    {
                        c1Next = 0;
                    }
                }

                // We got a character so put it into the block
                if (c1Next)
                    ac1In[c4BlkCnt++] = c1Next - 1;
                else
                    ac1In[c4BlkCnt++] = 0;
            }

            // Decode the block, we know it's a whole block
            DecodeBlock(ac1In, ac1Out);

            // Write the output bytes and bump the overall count
            strmOut.c4WriteRawBuffer(ac1Out, CIDLib_Base64::c4RawBlkSz);
            c4Count += CIDLib_Base64::c4RawBlkSz;
        }
    }

    catch(const TError& errToCatch)
    {
        //
        //  If it's an end of stream error, then handle any partial last
        //  block. Else, rethrow it.
        //
        if (!errToCatch.bCheckEvent(facCIDLib().strName()
                                    , kCIDErrs::errcStrm_EndOfStream))
        {
            throw;
        }

        // Handle the last partial block if any
        if (c4BlkCnt)
        {
            for (tCIDLib::TCard4 c4Ind = c4BlkCnt; c4Ind < CIDLib_Base64::c4EncBlkSz; c4Ind++)
                ac1In[c4Ind] = 0;
            DecodeBlock(ac1In, ac1Out);

            //
            //  Write the output bytes and bump the overall count. Only write
            //  the count that we actually got, not the zero fill.
            //
            strmOut.c4WriteRawBuffer(ac1Out, c4BlkCnt - 1);
            c4Count += (c4BlkCnt - 1);
        }
    }

    // Flush the output stream and return the bytes we wrote to it
    strmOut.Flush();
    return c4Count;
}


tCIDLib::TCard4 TBase64::c4Encode(TBinInStream& strmIn, TBinOutStream& strmOut)
{
    tCIDLib::TCard4 c4LnWidth = 0;
    tCIDLib::TCard4 c4Count = 0;
    tCIDLib::TCard1 ac1In[CIDLib_Base64::c4RawBlkSz];

    // Twice as big to allow for escapes
    tCIDLib::TCard1 ac1Out[CIDLib_Base64::c4EncBlkSz * 2];
    while (kCIDLib::True)
    {
        //
        //  Read up to a full input block from the input stream. Tell it not
        //  to throw if not all bytes were read, but to just return the bytes
        //  it could get.
        //
        const tCIDLib::TCard4 c4In = strmIn.c4ReadRawBuffer
        (
            ac1In
            , CIDLib_Base64::c4RawBlkSz
            , tCIDLib::EAllData::OkIfNotAll
        );

        // If we got no bytes this time, we are done
        if (!c4In)
            break;

        // Zero pad the block if not full
        for (tCIDLib::TCard4 c4Ind = c4In; c4Ind < CIDLib_Base64::c4RawBlkSz; c4Ind++)
            ac1In[c4Ind] = 0;

        // Convert this block to an output block, possibly escaped
        const tCIDLib::TCard4 c4OutBytes = c4EncodeBlock(ac1In, ac1Out, c4In);
        for (tCIDLib::TCard4 c4OInd = 0; c4OInd < c4OutBytes; c4OInd++)
        {
            strmOut << ac1Out[c4OInd];
            c4Count++;
            c4LnWidth++;

            // If this line is full write out a new line
            if (c4LnWidth == m_c4LineWidth)
            {
                static const tCIDLib::TCard1 ac1NL[2] = { 0xD, 0xA };
                strmOut.c4WriteRawBuffer(ac1NL, 2);
                c4Count += 2;
                c4LnWidth = 0;
            }
        }

        // If we didn't get a full input block this time, we are done
        if (c4In < CIDLib_Base64::c4RawBlkSz)
            break;
    }

    // Flush the output stream and return the bytes we wrote to it
    strmOut.Flush();
    return c4Count;
}


tCIDLib::TCard4 TBase64::c4LineWidth() const
{
    return m_c4LineWidth;
}

tCIDLib::TCard4 TBase64::c4LineWidth(const tCIDLib::TCard4 c4ToSet)
{
    m_c4LineWidth = c4ToSet;

    // Don't let it go under 1 block's worth of output
    if (m_c4LineWidth < 4)
        m_c4LineWidth = 4;
    return m_c4LineWidth;
}


//
//  The above encode/decode methods take binary streams in and out. These
//  do the same thing, but take a text in stream (for decoding) and a text
//  out stream (for encoding), since the Base64 stuff is text and will
//  often need to be pulled out of a text stream or written to on.
//
//  This isn't the most efficient thing to do, since Base64 is effectively
//  ASCII stuff, and we have to pass it through Unicode form here because
//  text streams work in Unicode format, so it makes a couple round trips
//  from ASCII to Unicode (or vice versa) on the way.
//
tCIDLib::TVoid TBase64::Decode(TTextInStream& strmIn, TBinOutStream& strmOut)
{
    tCIDLib::TCard4 c4BlkCnt = 0;
    tCIDLib::TCard1 ac1In[CIDLib_Base64::c4EncBlkSz];
    tCIDLib::TCard1 ac1Out[CIDLib_Base64::c4RawBlkSz];

    try
    {
        while (kCIDLib::True)
        {
            //
            //  Try to get a block's worth out, tossing junk. We'll convert
            //  the incoming Unicode chars to ASCII bytes as we go.
            //
            c4BlkCnt = 0;
            while (c4BlkCnt < CIDLib_Base64::c4EncBlkSz)
            {
                //
                //  Loop until we get a non-junk character
                tCIDLib::TCard1 c1Next = 0;
                while (!c1Next)
                {
                    // Get the next char in
                    tCIDLib::TCh chNext;
                    strmIn >> chNext;

                    if ((chNext > 42) && (chNext < 123))
                    {
                        c1Next = CIDLib_Base64::ac1Decode[chNext - 43];
                        if (c1Next)
                        {
                            if (c1Next == 0x24)
                                c1Next = 0;
                            else
                                c1Next -= 61;
                        }
                    }
                     else
                    {
                        c1Next = 0;
                    }
                }

                // We got a byte so put it into the block
                if (c1Next)
                    ac1In[c4BlkCnt++] = c1Next - 1;
                else
                    ac1In[c4BlkCnt++] = 0;
            }

            // Decode the block, we know it's a whole block
            DecodeBlock(ac1In, ac1Out);

            // Write the output bytes
            strmOut.c4WriteRawBuffer(ac1Out, CIDLib_Base64::c4RawBlkSz);
        }
    }

    catch(const TError& errToCatch)
    {
        //
        //  If it's an end of stream error, then handle any partial last
        //  block. Else, rethrow it.
        //
        if (!errToCatch.bCheckEvent(facCIDLib().strName()
                                    , kCIDErrs::errcStrm_EndOfStream))
        {
            throw;
        }

        // Handle the last partial block if any
        if (c4BlkCnt)
        {
            for (tCIDLib::TCard4 c4Ind = c4BlkCnt; c4Ind < CIDLib_Base64::c4EncBlkSz; c4Ind++)
                ac1In[c4Ind] = 0;
            DecodeBlock(ac1In, ac1Out);

            //
            //  Write the output bytes. Only write the count that we actually got, not the
            //  zero fill.
            //
            strmOut.c4WriteRawBuffer(ac1Out, c4BlkCnt - 1);
        }
    }

    // Flush the output stream
    strmOut.Flush();
}


tCIDLib::TVoid TBase64::Encode(TBinInStream& strmIn, TTextOutStream& strmOut)
{
    tCIDLib::TCard4 c4LnWidth = 0;
    tCIDLib::TCard1 ac1In[CIDLib_Base64::c4RawBlkSz];

    // Possibly twice as big if escaping
    tCIDLib::TCard1 ac1Out[CIDLib_Base64::c4EncBlkSz * 2];
    while (kCIDLib::True)
    {
        //
        //  Read up to a full input block from the input stream. Tell it not
        //  to throw if not all bytes were read, but to just return the bytes
        //  it could get.
        //
        const tCIDLib::TCard4 c4In = strmIn.c4ReadRawBuffer
        (
            ac1In, CIDLib_Base64::c4RawBlkSz, tCIDLib::EAllData::OkIfNotAll
        );

        // If we got no bytes this time, we are done
        if (!c4In)
            break;

        // Zero pad it if needed
        for (tCIDLib::TCard4 c4Ind = c4In; c4Ind < CIDLib_Base64::c4RawBlkSz; c4Ind++)
            ac1In[c4Ind] = 0;

        // Convert this block to an output block
        const tCIDLib::TCard4 c4OutBytes = c4EncodeBlock(ac1In, ac1Out, c4In);
        for (tCIDLib::TCard4 c4OInd = 0; c4OInd < c4OutBytes; c4OInd++)
        {
            strmOut << tCIDLib::TCh(ac1Out[c4OInd]);
            c4LnWidth++;

            if (c4LnWidth == m_c4LineWidth)
            {
                c4LnWidth = 0;
                strmOut << kCIDLib::NewLn;
            }
        }

        // If we didn't get a full input block this time, we are done
        if (c4In < CIDLib_Base64::c4RawBlkSz)
            break;
    }

    // Flush the output stream
    strmOut.Flush();
}



//
//  This one converts from a string to a binary output stream. We assume the
//  text is all ASCII, so we do a cheap transcode by just grabbing the low
//  byte of each character.
//
tCIDLib::TVoid TBase64::Encode(const TString& strSrc, TBinOutStream& strmOut)
{
    tCIDLib::TCard4 c4LnWidth = 0;
    tCIDLib::TCard1 ac1In[CIDLib_Base64::c4RawBlkSz];

    // Could be twice as big if escaping
    tCIDLib::TCard1 ac1Out[CIDLib_Base64::c4EncBlkSz * 2];

    const tCIDLib::TCh*   pszSrc = strSrc.pszBuffer();
    const tCIDLib::TCh*   pszEnd = pszSrc + strSrc.c4Length();

    while (kCIDLib::True)
    {
        // Get up a full block of source bytes
        tCIDLib::TCard4 c4In = 0;
        while ((c4In < CIDLib_Base64::c4RawBlkSz) && (pszSrc < pszEnd))
            ac1In[c4In++] = tCIDLib::TCard1(*pszSrc++);

        // If we got no bytes this time, we are done
        if (!c4In)
            break;

        // Zero pad any partial last block
        for (tCIDLib::TCard4 c4Ind = c4In; c4Ind < CIDLib_Base64::c4RawBlkSz; c4Ind++)
            ac1In[c4Ind] = 0;

        // Convert this block to an output block
        const tCIDLib::TCard4 c4OutBytes = c4EncodeBlock(ac1In, ac1Out, c4In);

        // And output it
        for (tCIDLib::TCard4 c4OInd = 0; c4OInd < c4OutBytes; c4OInd++)
        {
            strmOut << ac1Out[c4OInd];
            c4LnWidth++;

            // If this line is full write out a new line
            if (c4LnWidth == m_c4LineWidth)
            {
                static const tCIDLib::TCard1 ac1NL[2] = { 0xD, 0xA };
                strmOut.c4WriteRawBuffer(ac1NL, 2);
                c4LnWidth = 0;
            }
        }

        // If we didn't get a full input block this time, we are done
        if (c4In < CIDLib_Base64::c4RawBlkSz)
            break;
    }

    // Flush the output stream
    strmOut.Flush();
}



//
//  This one converts from a text input stream to an text output stream. So
//  we pull in chars and assume they are ASCII content in Unicode form, and
//  just do a cheap transcode by grabbing the low byte of each char.
//
tCIDLib::TVoid TBase64::Encode(TTextInStream& strmIn, TTextOutStream& strmOut)
{
    tCIDLib::TCard4 c4LnWidth = 0;
    tCIDLib::TCard1 ac1In[CIDLib_Base64::c4RawBlkSz];

    // Possibly twice as large if padding
    tCIDLib::TCard1 ac1Out[CIDLib_Base64::c4EncBlkSz * 2];
    while (kCIDLib::True)
    {
        // Get up a full block of source bytes
        tCIDLib::TCard4 c4In = 0;
        while ((c4In < CIDLib_Base64::c4RawBlkSz) && !strmIn.bEndOfStream())
            ac1In[c4In++] = tCIDLib::TCard1(strmIn.chRead());

        // If we got no bytes this time, we are done
        if (!c4In)
            break;

        // Zero pad the input block if needed
        for (tCIDLib::TCard4 c4Ind = c4In; c4Ind < CIDLib_Base64::c4RawBlkSz; c4Ind++)
            ac1In[c4Ind] = 0;

        // Convert this block to an output block
        const tCIDLib::TCard4 c4OutBytes = c4EncodeBlock(ac1In, ac1Out, c4In);

        // And output it
        for (tCIDLib::TCard4 c4OInd = 0; c4OInd < c4OutBytes; c4OInd++)
        {
            strmOut << tCIDLib::TCh(ac1Out[c4OInd]);
            c4LnWidth++;

            if (c4LnWidth == m_c4LineWidth)
            {
                c4LnWidth = 0;
                strmOut << kCIDLib::NewLn;
            }
        }

        // If we didn't get a full input block this time, we are done
        if (c4In < CIDLib_Base64::c4RawBlkSz)
            break;
    }

    // Flush the output stream
    strmOut.Flush();
}




// ---------------------------------------------------------------------------
//  TBase64: Private, non-virtual methods
// ---------------------------------------------------------------------------

// The output can be up to twice the actual block size if we are escaping slashes
tCIDLib::TCard4
TBase64::c4EncodeBlock( const   tCIDLib::TCard1* const  pc1In
                        ,       tCIDLib::TCard1* const  pc1Out
                        , const tCIDLib::TCard4         c4Len)
{
    tCIDLib::TCard4 c4Ret = 0;
    tCIDLib::TCard1 c1Cur;

    c1Cur = CIDLib_Base64::ac1Encode[pc1In[0] >> 2];
    if (m_bEscapeForwardSlashes && (c1Cur == 0x2F))
        pc1Out[c4Ret++] = 0x5C;
    pc1Out[c4Ret++] = c1Cur;

    c1Cur = CIDLib_Base64::ac1Encode
    [
        ((pc1In[0] & 0x03) << 4) | ((pc1In[1] & 0xF0) >> 4)
    ];
    if (m_bEscapeForwardSlashes && (c1Cur == 0x2F))
        pc1Out[c4Ret++] = 0x5C;
    pc1Out[c4Ret++] = c1Cur;

    if (c4Len > 1)
    {
        c1Cur =  CIDLib_Base64::ac1Encode[((pc1In[1] & 0x0F) << 2) | ((pc1In[2] & 0xC0) >> 6)];
        if (m_bEscapeForwardSlashes && (c1Cur == 0x2F))
            pc1Out[c4Ret++] = 0x5C;
        pc1Out[c4Ret++] = c1Cur;
    }
     else
    {
        pc1Out[c4Ret++] = 0x3D;
    }

    if (c4Len > 2)
    {
        c1Cur = CIDLib_Base64::ac1Encode[pc1In[2] & 0x3F];
        if (m_bEscapeForwardSlashes && (c1Cur == 0x2F))
            pc1Out[c4Ret++] = 0x5C;
        pc1Out[c4Ret++] = c1Cur;
    }
     else
    {
        pc1Out[c4Ret++] = 0x3D;
    }
    return c4Ret;
}



tCIDLib::TVoid
TBase64::DecodeBlock(const  tCIDLib::TCard1* const  pc1In
                    ,       tCIDLib::TCard1* const  pc1Out)
{
    pc1Out[0] = (pc1In[0] << 2) | (pc1In[1] >> 4);
    pc1Out[1] = (pc1In[1] << 4) | (pc1In[2] >> 2);
    pc1Out[2] = ((pc1In[2] << 6) & 0xC0) | pc1In[3];
}
