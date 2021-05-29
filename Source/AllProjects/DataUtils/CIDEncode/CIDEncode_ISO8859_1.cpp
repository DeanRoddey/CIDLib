//
// FILE NAME: CIDEncode_ISO8859_1.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/29/1999
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
//  This file implements the TLatin1Converter class, which handles converting
//  the Latin-1 encoding to and from the internal Unicode format.
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
#include    "CIDEncode_.hpp"



// ---------------------------------------------------------------------------
//  Magic RTTI macros. We support advanced RTTI so that converters can be
//  created by class name.
// ---------------------------------------------------------------------------
AdvRTTIDecls(TLatin1Converter,TTextConverter)

namespace
{
    namespace CIDEncode_ISO8859_1
    {
        // -------------------------------------------------------------------
        //  Local const data
        //
        //  ac1Map
        //      This is an array of flags that represent the valid ISO-8859-1 code
        //      points. Since the Unicode code points for the valid 8859 points
        //      are the same values. We can just look up a flag here and, if its
        //      non-zero, the code point value is valid for both Unicode and 8859.
        // -------------------------------------------------------------------
        constexpr tCIDLib::TCard1 ac1Map[] =
        {
            1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 1 , 0 , 0
            , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
            , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
            , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
            , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
            , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
            , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
            , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0
            , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
            , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
            , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
            , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
            , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
            , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
            , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
            , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
        };
    }
}



// ---------------------------------------------------------------------------
//  TLatin1Converter: Constructors and Destructor
// ---------------------------------------------------------------------------
TLatin1Converter::TLatin1Converter() :

    TTextConverter(L"ISO-8859-1")
{
}


// ---------------------------------------------------------------------------
//  TLatinConverter: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::EBaseTextFmts TLatin1Converter::eBaseFmt() const
{
    return tCIDLib::EBaseTextFmts::SingleByte;
}


// ---------------------------------------------------------------------------
//  TLatinConverter: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TLatin1Converter::c4BlockFrom(  const   tCIDLib::TCard1* const  pc1Src
                                , const tCIDLib::TCard4         c4SrcBytes
                                ,       tCIDLib::TCh* const     pszToFill
                                , const tCIDLib::TCard4         c4MaxChars
                                ,       tCIDLib::TCard4&        c4OutChars
                                ,       tCIDLib::TBoolean&      bStop)
{
    const tCIDLib::TCh          chRep = chRepChar();
    const tCIDLib::ETCvtActs    eAct = eErrorAction();
    bStop = kCIDLib::False;

    //
    //  There is a one to one correspondence, so the bytes we convert is the
    //  smaller of the available bytes and the max chars.
    //
    const tCIDLib::TCard4 c4Actual = tCIDLib::MinVal(c4SrcBytes, c4MaxChars);

    //
    //  For each char we can do, just cast up the ISO char to the Unicode
    //  char, since all of the Latin1 characters have the same code point
    //  values in Unicode (just of a different data type.)
    //
    tCIDLib::TCard4 c4Index;
    for (c4Index = 0; c4Index < c4Actual; c4Index++)
    {
        // Just cast up the value to a native char
        const tCIDLib::TCh chCur = tCIDLib::TCh(pc1Src[c4Index]);

        //
        //  Make sure this character is valid. Since they almost always will
        //  be, do the positive attack first, leaving the error handling for
        //  the end.
        //
        if (CIDEncode_ISO8859_1::ac1Map[chCur])
        {
            pszToFill[c4Index] = chCur;
            continue;
        }

        if ((eAct == tCIDLib::ETCvtActs::StopThenThrow) && c4Index)
        {
            bStop = kCIDLib::True;
            break;
        }

        if (eAct == tCIDLib::ETCvtActs::Replace)
        {
            pszToFill[c4Index] = chRep;
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

    // Return the bytes we processed and chars we output
    c4OutChars = c4Index;
    return c4Index;
}


tCIDLib::TCard4
TLatin1Converter::c4BlockTo(const   tCIDLib::TCh* const     pszSrc
                            , const tCIDLib::TCard4         c4SrcChars
                            ,       tCIDLib::TCard1* const  pc1ToFill
                            , const tCIDLib::TCard4         c4MaxBytes
                            ,       tCIDLib::TCard4&        c4OutBytes
                            ,       tCIDLib::TBoolean&      bStop)
{
    //
    //  There is a one to one correspondence, so the bytes we convert is the
    //  smaller of the available chars and the max bytes.
    //
    const tCIDLib::TCard4 c4Actual = tCIDLib::MinVal(c4SrcChars, c4MaxBytes);

    //
    //  We know that the Unicode and Latin1 code points are all the same, for
    //  those points that are in the Latin1 set I mean. So all we have to do
    //  is make sure the character is less than 256 and that its flag is on
    //  in the map. If so we can just convert it to a short char. Else we
    //  either use the replacement or throw.
    //
    tCIDLib::TCard4 c4Index;
    for (c4Index = 0; c4Index < c4Actual; c4Index++)
    {
        // Get the current source char to convert
        const tCIDLib::TCh chCur = pszSrc[c4Index];

        //
        //  Make sure this character is valid. Since they almost always will
        //  be, do the positive attack first, leaving the error handling for
        //  the end.
        //
        if (chCur < 256)
        {
            if (CIDEncode_ISO8859_1::ac1Map[chCur])
            {
                pc1ToFill[c4Index] = tCIDLib::TCard1(chCur);
                continue;
            }
        }

        if ((eErrorAction() == tCIDLib::ETCvtActs::StopThenThrow) && c4Index)
        {
            bStop = kCIDLib::True;
            break;
        }

        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTCvt_Unrepresentable
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(tCIDLib::TCard4(chCur))
            , strEncodingName()
        );
    }

    // Return the chars we processed and the bytes processed
    c4OutBytes = c4Index;
    return c4Index;
}


