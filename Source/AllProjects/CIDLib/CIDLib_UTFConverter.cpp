//
// FILE NAME: CIDLib_UTFConverter.cpp
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
//  This file implements the TUTFConverter class, which handles converting
//  the UTF-8/16 family of encodings to/from the internal UTF-16 LE format.
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
//  Magic RTTI macros. We support advanced RTTI so that converters can be
//  created by class name.
// ---------------------------------------------------------------------------
AdvRTTIDecls(TUTFConverter,TTextConverter)
AdvRTTIDecls(TUSASCIIConverter,TUTFConverter)
AdvRTTIDecls(TUTF16BEConverter,TUTFConverter)
AdvRTTIDecls(TUTF16LEConverter,TUTFConverter)
AdvRTTIDecls(TUTF8Converter,TUTFConverter)


namespace
{
    namespace CIDLib_UTFConverter
    {
        // -----------------------------------------------------------------------
        //  Local data
        //
        //  ac1FirstByteMark
        //      A mask to mask onto the first byte of an encoded UTF-8 sequence.
        //      Its indexed by the number of bytes required to encode it.
        //
        //  ac1UTFBytes
        //      This is an array of values that are indexed by the first encoded
        //      char of a UTF-8 encoded char, resulting in the number of bytes it
        //      uses to encode the UTF-16 char.
        //
        //  ac4UTFOffsets
        //      This array is indexed by the count of bytes required to encode
        //      the value. Its provides an amount to offset the decoded UTF-16
        //      value by.
        //
        //  eDefEncoding
        //      The default encoding of a TCh value for this host workstation.
        //      Byte knowing this value, we can optimize when the source and
        //      target encodings are the same. And its used in the default ctor,
        //      to create a converter with those characteristics.
        //
        //  apszEncodings
        //      Strings that represent the descriptions of the EEncodings type.
        // -----------------------------------------------------------------------
        constexpr const tCIDLib::TCard1 ac1FirstByteMark[7] =
        {
            0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
        };
        constexpr const tCIDLib::TCard4 ac4UTFOffsets[6] =
        {
            0, 0x3080, 0xE2080, 0x3C82080, 0xFA082080, 0x82082080
        };
        constexpr const tCIDLib::TCard1 ac1UTFBytes[256] =
        {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
            ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
            ,   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
            ,   3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
        };
        constexpr const tCIDLib::TCh* const apszEncodingVals[] =
        {
            L"US-ASCII"
            , L"UTF-8"
            , L"UTF-16LE"
            , L"UTF-16BE"
        };

        TEArray<const tCIDLib::TCh*, TUTFConverter::EEncodings, TUTFConverter::EEncodings::Count>
        apszEncodings(apszEncodingVals);
    }
}



// ---------------------------------------------------------------------------
//  TUTFConverter: Constructors and Destructor
// ---------------------------------------------------------------------------
TUTFConverter::TUTFConverter(const EEncodings eEncoding) :

    TTextConverter(CIDLib_UTFConverter::apszEncodings[eEncoding])
    , m_eEncoding(eEncoding)
{
}


// ---------------------------------------------------------------------------
//  TUTFConverter: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::EBaseTextFmts TUTFConverter::eBaseFmt() const
{
    if (m_eEncoding == EEncodings::USASCII)
        return tCIDLib::EBaseTextFmts::SingleByte;
    else if  (m_eEncoding == EEncodings::UTF8)
        return tCIDLib::EBaseTextFmts::MultiByte;
    else if (m_eEncoding == EEncodings::UTF16_LE)
        return tCIDLib::EBaseTextFmts::TwoByte_LE;
    else if (m_eEncoding == EEncodings::UTF16_BE)
        return tCIDLib::EBaseTextFmts::TwoByte_BE;


    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcGen_BadEnumValue
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::AppError
        , TInteger(tCIDLib::i4EnumOrd(m_eEncoding))
        , TString(L"TUTFConverter::EEncodings")
    );

    return tCIDLib::EBaseTextFmts::Count;
}


TUTFConverter::EEncodings TUTFConverter::eEncoding() const
{
    return m_eEncoding;
}


// ---------------------------------------------------------------------------
//  TUTFConverter: Private, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TUTFConverter::c4BlockFrom( const   tCIDLib::TCard1* const  pc1Src
                            , const tCIDLib::TCard4         c4SrcBytes
                            ,       tCIDLib::TCh* const     pszToFill
                            , const tCIDLib::TCard4         c4MaxChars
                            ,       tCIDLib::TCard4&        c4OutChars
                            ,       tCIDLib::TBoolean&      bStop)
{
    const tCIDLib::TCh          chRep = chRepChar();
    const tCIDLib::ETCvtActs    eAct = eErrorAction();

    //
    //  Do the required work according to the specific encoding. Initialize
    //  output chars parm to zero so we can use it as an index below, and
    //  just run it up as we go
    //
    bStop = kCIDLib::False;
    tCIDLib::TCard4 c4Bytes = 0;
    c4OutChars = 0;
    switch(m_eEncoding)
    {
        case EEncodings::USASCII :
        {
            //
            //  This one just requires a tight loop to clip the high bytes of
            //  each char and put it into a byte in the output array. The
            //  chars consumed and bytes output are the same, so we use the chars
            //  as the index and set the bytes to the same when done.
            //
            const tCIDLib::TCard4 c4MaxCnt = tCIDLib::MinVal(c4SrcBytes, c4MaxChars);
            for (; c4OutChars < c4MaxCnt; c4OutChars++)
            {
                if (!pc1Src[c4OutChars] || (pc1Src[c4OutChars] > 0x7F))
                {
                    // It's a bad char
                    if ((eAct == tCIDLib::ETCvtActs::StopThenThrow) && c4OutChars)
                    {
                        bStop = kCIDLib::True;
                        break;
                    }

                    if (eAct == tCIDLib::ETCvtActs::Replace)
                    {
                        pszToFill[c4OutChars] = chRep;
                    }
                     else
                    {
                        facCIDLib().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kCIDErrs::errcTCvt_BadSource
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Format
                            , strEncodingName()
                        );
                    }
                }
                 else
                {
                    pszToFill[c4OutChars] = tCIDLib::TCh(pc1Src[c4OutChars]);
                }
            }
            c4Bytes = c4OutChars;
            break;
        }

        case EEncodings::UTF8 :
        {
            const tCIDLib::TCard1*  pc1SrcPtr   = pc1Src;
            const tCIDLib::TCard1*  pc1SrcEnd   = pc1Src + c4SrcBytes;

            tCIDLib::TCh*           pchOutPtr   = pszToFill;
            tCIDLib::TCh*           pchOutEnd   = pszToFill + c4MaxChars;

            while ((pc1SrcPtr < pc1SrcEnd) && (pchOutPtr < pchOutEnd))
            {
                //
                //  Get the first byte and optimize if its < 0x80, since it
                //  can be taken as is.
                //
                const tCIDLib::TCard1 c1First = *pc1SrcPtr;
                if (c1First <= 0x7F)
                {
                    *pchOutPtr++ = tCIDLib::TCh(c1First);
                    pc1SrcPtr++;
                    continue;
                }

                // Not the common case, so see how many encoded bytes we need
                const tCIDLib::TCard4 c4EncBytes
                            = CIDLib_UTFConverter::ac1UTFBytes[c1First];

                //
                //  See if we have enough bytes in the source to do a whole
                //  car. If not, then break out and leave it for the next
                //  time.
                //
                if (pc1SrcPtr + c4EncBytes >= pc1SrcEnd)
                    break;

                // Looks ok, so lets build up the value
                tCIDLib::TCard4 c4Val = 0;
                const tCIDLib::TCard1* pc1Tmp = pc1SrcPtr;
                switch(c4EncBytes)
                {
                    case 5 : c4Val += *pc1Tmp++; c4Val <<= 6;
                    case 4 : c4Val += *pc1Tmp++; c4Val <<= 6;
                    case 3 : c4Val += *pc1Tmp++; c4Val <<= 6;
                    case 2 : c4Val += *pc1Tmp++; c4Val <<= 6;
                    case 1 : c4Val += *pc1Tmp++; c4Val <<= 6;
                    case 0 : c4Val += *pc1Tmp++;
                }
                c4Val -= CIDLib_UTFConverter::ac4UTFOffsets[c4EncBytes];

                //
                //  If it will fit into a single char, then put it in.
                //  Otherwise encode it as a surrogate pair.
                //
                if (!(c4Val & 0xFFFF0000))
                {
                    // Not invalid, so move up the source pointer
                    pc1SrcPtr += (c4EncBytes + 1);
                    *pchOutPtr++ = tCIDLib::TCh(c4Val);
                }
                 else if (c4Val > 0x10FFFF)
                {
                    // Its a bad char. We don't move up the source pointer here
                    if ((eAct == tCIDLib::ETCvtActs::StopThenThrow)
                    &&  (pc1SrcPtr != pc1Src))
                    {
                        bStop = kCIDLib::True;
                        break;
                    }

                    if (eAct == tCIDLib::ETCvtActs::Replace)
                    {
                        // We are recoverings, so eat the src bytes
                        pc1SrcPtr += c4EncBytes + 1;
                        *pchOutPtr++ = chRep;
                    }
                     else
                    {
                        facCIDLib().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kCIDErrs::errcTCvt_BadSource
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Format
                            , strEncodingName()
                        );
                    }
                }
                 else
                {
                    //
                    //  The pre-loop above didn't check for this, since we our
                    //  outputting two chars instead of one, so see if we have
                    //  the extra space.
                    //
                    if (pchOutPtr + 1 >= pchOutEnd)
                        break;

                    // Not invalid, so move up the source pointer
                    pc1SrcPtr += (c4EncBytes + 1);

                    c4Val -= 0x10000;
                    *pchOutPtr++ = tCIDLib::TCh((c4Val >> 10) + 0xD800);
                    *pchOutPtr++ = tCIDLib::TCh((c4Val & 0x3FF) + 0xDC00);
                }
            }

            c4OutChars = pchOutPtr - pszToFill;
            c4Bytes = pc1SrcPtr - pc1Src;
            break;
        }

        case EEncodings::UTF16_LE :
        case EEncodings::UTF16_BE :
        {
            //
            //  Calculate the max chars we can process. Its the smaller of the
            //  target length and the max input bytes divided by the char size.
            //
            c4OutChars = tCIDLib::MinVal
            (
                c4MaxChars
                , c4SrcBytes / kCIDLib::c4UniBytes
            );

            // And calculate the bytes that represents
            c4Bytes = c4OutChars * kCIDLib::c4UniBytes;

            //
            //  Determine if we need to flip the byte order up front, to
            //  simplify the logic below.
            //
            tCIDLib::TBoolean bFlip = kCIDLib::False;
            #if defined(CIDLIB_LITTLEENDIAN)
            if (m_eEncoding == EEncodings::UTF16_BE)
                bFlip = kCIDLib::True;
            #else
            if (m_eEncoding == EEncodings::UTF16_LE)
                bFlip = kCIDLib::True;
            #endif

            // Look at the source as UTF-16 chars
            const tCIDLib::TUniCh* pszTmp =
                            reinterpret_cast<const tCIDLib::TUniCh*>(pc1Src);

            #if defined(CIDLIB_WCISUTF16)

            //
            //  The char sizes are the same internally and externally, so just
            //  copy over the required number of bytes. Flip them if required.
            //
            if (bFlip)
            {
                // We need to flip them as we store them
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4OutChars; c4Index++)
                    pszToFill[c4Index] = TRawBits::c2SwapBytes(*pszTmp++);
            }
             else
            {
                // We can do the optimal buffer copy in this case
                TRawMem::CopyMemBuf(pszToFill, pc1Src, c4Bytes);
            }

            #else

            //
            //  The internal char size is 32 bit, so we have to up cast the char
            //  data and potentially byte swap it. We have to be sure to do it
            //  in the correct order. It must be swapped to native order first,
            //  then upcast. If no flip is required, we just have to upconvert
            //  the bytes as is.
            //
            if (bFlip)
            {
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4OutChars; c4Index++)
                {
                    const tCIDLib::TUniCh uchTmp = TRawBits::c2SwapBytes(*pszTmp++);
                    pszToFill[c4Index] = tCIDLib::TCh(uchTmp);
                }
            }
             else
            {
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4OutChars; c4Index++)
                    pszToFill[c4Index] = tCIDLib::TCh(*pszTmp++);
            }

            #endif

            break;
        }

        default :
            // This is way bad
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_BadEnumValue
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
                , TInteger(tCIDLib::i4EnumOrd(m_eEncoding))
                , TString(L"TUTFConverter::EEncodings")
            );
    }
    return c4Bytes;
}


tCIDLib::TCard4
TUTFConverter::c4BlockTo(const  tCIDLib::TCh* const     pszSrc
                        , const tCIDLib::TCard4         c4SrcChars
                        ,       tCIDLib::TCard1* const  pc1ToFill
                        , const tCIDLib::TCard4         c4MaxBytes
                        ,       tCIDLib::TCard4&        c4OutBytes
                        ,       tCIDLib::TBoolean&      bStop)
{
    //
    //  Do the required work according to the specific encoding. Some of
    //  these can be done very quickly as tight loops.
    //
    tCIDLib::TCard4 c4Chars = 0;
    tCIDLib::TCard4 c4Bytes = 0;
    bStop = kCIDLib::False;

    switch(m_eEncoding)
    {
        case EEncodings::USASCII :
        {
            //
            //  This one just requires a tight loop to clip the high bytes of
            //  each char and put it into a byte in the output array. The
            //  chars consumed and bytes output are the same.
            //
            const tCIDLib::TCard4 c4Max = tCIDLib::MinVal(c4SrcChars, c4MaxBytes);

            for (; c4Chars < c4Max; c4Chars++)
            {
                const tCIDLib::TCh chCur = pszSrc[c4Chars];
                if (chCur > 0x7F)
                {
                    if ((eErrorAction() == tCIDLib::ETCvtActs::StopThenThrow)
                    &&  c4Chars)
                    {
                        bStop = kCIDLib::True;
                        break;
                    }

                    tCIDLib::TCh szTmp[2];
                    szTmp[0] = chCur;
                    szTmp[1] = kCIDLib::chNull;

                    facCIDLib().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kCIDErrs::errcTCvt_Unrepresentable
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::CantDo
                        , TString(szTmp)
                        , strEncodingName()
                    );
                }
                 else
                {
                    pc1ToFill[c4Chars] = tCIDLib::TCard1(chCur);
                }
            }

            // Bytes and chars are the same here
            c4Bytes = c4Chars;
            break;
        }

        case EEncodings::UTF8 :
        {
            //
            //  This one requires a more complex loop, to encode the chars as
            //  UTF-8 byte sequences.
            //
            const tCIDLib::TCh* pchSrcPtr   = pszSrc;
            const tCIDLib::TCh* pchSrcEnd   = pszSrc + c4SrcChars;
            tCIDLib::TCard1*    pc1OutPtr   = pc1ToFill;
            tCIDLib::TCard1*    pc1OutEnd   = pc1ToFill + c4MaxBytes;

            while (pchSrcPtr < pchSrcEnd)
            {
                //
                //  Get the next char out into a 32 bit value. If its a
                //  leading char, get the trailing char and put that in. If
                //  the next char is not in there, then save it for the next
                //  time.
                //
                tCIDLib::TCard4 c4Val = *pchSrcPtr;

                tCIDLib::TCard4 c4SrcUsed = 1;
                if ((c4Val >= 0xD800) && (c4Val <= 0xDBFF))
                {
                    if (pchSrcPtr + 1 >= pchSrcEnd)
                        break;

                    // Create the composite surrogate pair
                    c4Val = ((c4Val - 0xD800) << 10)
                            + ((*(pchSrcPtr + 1) - 0xDC00) + 0x10000);
                    c4SrcUsed++;
                }

                // Figure out how many bytes we need
                tCIDLib::TCard4 c4EncBytes = 0;
                if (c4Val < 0x80)
                    c4EncBytes = 1;
                else if (c4Val < 0x800)
                    c4EncBytes = 2;
                else if (c4Val < 0x10000)
                    c4EncBytes = 3;
                else if (c4Val < 0x200000)
                    c4EncBytes = 4;
                else if (c4Val < 0x4000000)
                    c4EncBytes = 5;
                else if (c4Val <= 0x7FFFFFFF)
                    c4EncBytes = 6;
                else
                {
                    if ((eErrorAction() == tCIDLib::ETCvtActs::StopThenThrow)
                    &&  (pc1OutPtr != pc1ToFill))
                    {
                        break;
                    }

                    facCIDLib().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kCIDErrs::errcTCvt_Unrepresentable
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::CantDo
                        , TCardinal(c4Val)
                        , strEncodingName()
                    );
                    break;
                }

                //
                //  If we cannot fully get this char into the output buffer,
                //  then leave it for the next time.
                //
                if (pc1OutPtr + c4EncBytes > pc1OutEnd)
                    break;

                // We can do it, so update the source index
                pchSrcPtr += c4SrcUsed;

                // And spit out the bytes
                pc1OutPtr += c4EncBytes;
                switch(c4EncBytes)
                {
                    case 6 : *--pc1OutPtr = tCIDLib::TCard1((c4Val | 0x80UL) & 0xBFUL);
                             c4Val >>= 6;
                    case 5 : *--pc1OutPtr = tCIDLib::TCard1((c4Val | 0x80UL) & 0xBFUL);
                             c4Val >>= 6;
                    case 4 : *--pc1OutPtr = tCIDLib::TCard1((c4Val | 0x80UL) & 0xBFUL);
                             c4Val >>= 6;
                    case 3 : *--pc1OutPtr = tCIDLib::TCard1((c4Val | 0x80UL) & 0xBFUL);
                             c4Val >>= 6;
                    case 2 : *--pc1OutPtr = tCIDLib::TCard1((c4Val | 0x80UL) & 0xBFUL);
                             c4Val >>= 6;
                    case 1 : *--pc1OutPtr = tCIDLib::TCard1
                             (
                                c4Val | CIDLib_UTFConverter::ac1FirstByteMark[c4EncBytes]
                             );
                }

                // Add the encoded bytes back in again
                pc1OutPtr += c4EncBytes;
            }

            c4Chars = pchSrcPtr - pszSrc;
            c4Bytes = pc1OutPtr - pc1ToFill;
            break;
        }

        case EEncodings::UTF16_LE :
        case EEncodings::UTF16_BE :
        {
            //
            //  Calculate the max chars we can process. Its the smaller of
            //  the source length and the UTF-16 chars we can fit in the
            //  output buffer.
            //
            c4Chars = tCIDLib::MinVal
            (
                c4SrcChars
                , c4MaxBytes / kCIDLib::c4UniBytes
            );

            // And calculate the bytes that represents
            c4Bytes = c4Chars * kCIDLib::c4UniBytes;

            //
            //  Determine if we need to flip the byte order up front, to
            //  simplify the logic below.
            //
            tCIDLib::TBoolean bFlip = kCIDLib::False;
            #if defined(CIDLIB_LITTLEENDIAN)
                if (m_eEncoding == EEncodings::UTF16_BE)
                    bFlip = kCIDLib::True;
            #else
                if (m_eEncoding == EEncodings::UTF16_LE)
                    bFlip = kCIDLib::True;
            #endif

            // Look at the target as UTF-16 chars
            tCIDLib::TUniCh* pszTmp = reinterpret_cast<tCIDLib::TUniCh*>(pc1ToFill);

            #if defined(CIDLIB_WCISUTF16)

            //
            //  The char sizes are the same internally and externally, so just
            //  copy over the required number of bytes. Flip them if required.
            //
            if (bFlip)
            {
                // We have to flip each char as we store it.
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Chars; c4Index++)
                    *pszTmp++ = TRawBits::c2SwapBytes(pszSrc[c4Index]);
            }
             else
            {
                // We can do the optimal memory buffer copy as is
                TRawMem::CopyMemBuf(pc1ToFill, pszSrc, c4Bytes);
            }

            #else

            //
            //  The internal char size is 32 bit, so we have to down cast the
            //  char data and potentially byte swap it. This is a little
            //  tricky. We have to get the relevant 16 bits out by downcasting
            //  it first. Then we flip if required.
            //
            if (bFlip)
            {
                //
                //  For this one, we have to first get the 16 bits that we care
                //  about out of the wide char, then we swap that.
                //
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Chars; c4Index++)
                {
                    *pszTmp++ = TRawBits::c2SwapBytes
                    (
                        tCIDLib::TUniCh(pszSrc[c4Index])
                    );
                }
            }
             else
            {
                // In this case, we just need to down cast the chars
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Chars; c4Index++)
                    *pszTmp++ = tCIDLib::TUniCh(pszSrc[c4Index]);
            }

            #endif

            break;
        }

        default :
            // This is obviously bad
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_BadEnumValue
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::AppError
                , TInteger(tCIDLib::i4EnumOrd(m_eEncoding))
                , TString(L"TUTFConverter::EEncodings")
            );
    }

    c4OutBytes = c4Bytes;
    return c4Chars;
}

