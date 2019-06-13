//
// FILE NAME: CIDEncode_UCS4.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/19/1999
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
//  This file implements the TUCS4Converter class, which handles converting
//  the UCS-4 encoding to and from the internal Unicode format. Since UCS-4
//  can hold chars up to 32 bits, we have to use Unicode's surrogate system
//  in some cases.
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
RTTIDecls(TUCS4Converter,TTextConverter)
AdvRTTIDecls(TUCS4BEConverter, TUCS4Converter)
AdvRTTIDecls(TUCS4LEConverter, TUCS4Converter)



// ---------------------------------------------------------------------------
//  TUCS4Converter: Constructors and Destructor
// ---------------------------------------------------------------------------
TUCS4Converter::TUCS4Converter(const tCIDLib::EEndianModes eMode) :

    TTextConverter
    (
        (eMode == tCIDLib::EEndianModes::Little)
        ? L"UCS-4(LE)" : L"UCS-4(BE)"
    )
    , m_bSwapped(kCIDLib::False)
    , m_eMode(eMode)
{
    #if defined(CIDLIB_LITTLEENDIAN)
    if (eMode == tCIDLib::EEndianModes::Big)
        m_bSwapped = kCIDLib::True;
    #else
    if (eMode == tCIDLib::EEndianModes::Little)
        m_bSwapped = kCIDLib::False;
    #endif
}

TUCS4Converter::TUCS4Converter(const TUCS4Converter& tcvtToCopy) :

    TTextConverter(tcvtToCopy)
    , m_bSwapped(tcvtToCopy.m_bSwapped)
    , m_eMode(tcvtToCopy.m_eMode)
{
}

TUCS4Converter::~TUCS4Converter()
{
}


// ---------------------------------------------------------------------------
//  TUCS4Converter: Constructors and Destructor
// ---------------------------------------------------------------------------
TUCS4Converter&
TUCS4Converter::operator=(const TUCS4Converter& tcvtToAssign)
{
    if (this == &tcvtToAssign)
        return *this;

    TParent::operator=(tcvtToAssign);
    m_bSwapped = tcvtToAssign.m_bSwapped;
    m_eMode = tcvtToAssign.m_eMode;
    return *this;
}


// ---------------------------------------------------------------------------
//  TUCS4Converter: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::EBaseTextFmts TUCS4Converter::eBaseFmt() const
{
    if (m_eMode == tCIDLib::EEndianModes::Little)
        return tCIDLib::EBaseTextFmts::FourByte_LE;
    return tCIDLib::EBaseTextFmts::FourByte_BE;
}


// ---------------------------------------------------------------------------
//  TUCS4Converter: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TUCS4Converter::c4BlockFrom(const   tCIDLib::TCard1* const  pc1Src
                            , const tCIDLib::TCard4         c4SrcBytes
                            ,       tCIDLib::TCh* const     pszToFill
                            , const tCIDLib::TCard4         c4MaxChars
                            ,       tCIDLib::TCard4&        c4OutChars
                            ,       tCIDLib::TBoolean&      bStop)
{
    const tCIDLib::TCh          chRep = chRepChar();
    const tCIDLib::ETCvtActions eAct = eErrorAction();
    bStop = kCIDLib::False;

    //
    //  Get pointers to the start and end of both the input and output buffers.
    //  There is not a one to one relationship between UCS-4 chars and our
    //  internal Unicode chars, because of the problem of surrogate pairs.
    //
    const tCIDLib::TCard4*  pc4Start = reinterpret_cast<const tCIDLib::TCard4*>(pc1Src);
    const tCIDLib::TCard4*  pc4Src = pc4Start;
    const tCIDLib::TCard4*  pc4End = reinterpret_cast<const tCIDLib::TCard4*>(pc1Src + c4SrcBytes);
    tCIDLib::TCh*           pchOut = pszToFill;
    tCIDLib::TCh*           pchEnd = pszToFill + c4MaxChars;

    // Loop until we get all the source done
    while ((pc4Src < pc4End) && (pchOut < pchEnd))
    {
        // Get the next char out
        tCIDLib::TCard4 c4Cur = *pc4Src;

        // Swap it if required
        if (m_bSwapped)
            c4Cur = TRawBits::c4SwapBytes(c4Cur);

        //
        //  If it will fit as is, then store it, else we have to do a surrogate
        //  pair. If the pair won't fit, then just leave it for the next time.
        //
        if (c4Cur < 0x10000)
        {
            *pchOut++ = tCIDLib::TCh(c4Cur);
        }
         else if (c4Cur <= 0x10FFFF)
        {
            // Do we have room to store both? If not, don't do this one.
            if (pchOut + 2 > pchEnd)
                break;

            // Break out the values into surrogate pairs
            c4Cur -= 0x10000;
            *pchOut++ = tCIDLib::TCh((c4Cur >> 10) + 0xD800);
            *pchOut++ = tCIDLib::TCh((c4Cur & 0x3FF) + 0xDC00);
        }
         else
        {
            if ((eAct == tCIDLib::ETCvtActions::StopThenThrow)
            &&  (pchOut != pszToFill))
            {
                bStop = kCIDLib::True;
                break;
            }

            if (eAct == tCIDLib::ETCvtActions::Replace)
            {
                *pchOut++ = chRep;
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

        // Move up the source by one
        pc4Src++;
    }

    // Return the bytes we processed and chars we output
    c4OutChars = pchOut - pszToFill;
    return TRawMem::c4AdrDiff(pc1Src, pc4Src);
}


tCIDLib::TCard4
TUCS4Converter::c4BlockTo(  const   tCIDLib::TCh* const     pszSrc
                            , const tCIDLib::TCard4         c4SrcChars
                            ,       tCIDLib::TCard1* const  pc1ToFill
                            , const tCIDLib::TCard4         c4MaxBytes
                            ,       tCIDLib::TCard4&        c4OutBytes
                            ,       tCIDLib::TBoolean&      bStop)
{
    const tCIDLib::ETCvtActions eAct = eErrorAction();
    bStop = kCIDLib::False;

    //
    //  Get pointers to the beginning and end of the input and output buffers.
    //  There is not a one to one correspondence between UCS-4 and Unicode
    //  chars because of surrogates, so we have to track both buffer ends.
    //
    tCIDLib::TCard4* pc4Out = reinterpret_cast<tCIDLib::TCard4*>(pc1ToFill);
    tCIDLib::TCard4* pc4End = reinterpret_cast<tCIDLib::TCard4*>(pc1ToFill + c4MaxBytes);
    const tCIDLib::TCh* pchSrc = pszSrc;
    const tCIDLib::TCh* pchEnd = pszSrc + c4SrcChars;

    while ((pc4Out < pc4End) && (pchSrc < pchEnd))
    {
        // Get the next character from the source
        const tCIDLib::TCh chCur = *pchSrc;

        //
        //  If this character indicates that this is a leading character of a
        //  surrogate pair, see if there is another source char. If not, then
        //  give up where we are. If so, then get it out and create the single
        //  UCS-4 character.
        //
        if ((chCur >= 0xD800) && (chCur < 0xDC00))
        {
            if (pchSrc + 2 > pchEnd)
                break;

            // We have another one so get it and make sure its a legal surrogate
            const tCIDLib::TCh chTrail = *pchSrc++;

            // See if its a valid trailing surrogate
            if ((chCur < 0xDC00) && (chCur > 0xDFFF))
            {
                if ((eAct == tCIDLib::ETCvtActions::StopThenThrow)
                &&  (pchSrc != pszSrc))
                {
                    bStop = kCIDLib::True;
                    break;
                }

                tCIDLib::TCh szTmp[3];
                szTmp[0] = chCur;
                szTmp[1] = chTrail;
                szTmp[2] = kCIDLib::chNull;

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
                // Looks ok, so lets create the surrogate and store it
                *pc4Out++ = ((chCur - 0xD800) << 10) + (chTrail - 0xDC00) + 0x10000;
            }
        }
         else
        {
            // Its not a surrogate, so store it
            *pc4Out++ = tCIDLib::TCard4(chCur);
        }

        // Bump up the source pointer
        pchSrc++;
    }

    // Calculate the output bytes and chars eaten
    c4OutBytes = TRawMem::c4AdrDiff(pc1ToFill, pc4Out);
    return pchSrc - pszSrc;
}





// ---------------------------------------------------------------------------
//  TUCS4Converter: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::EEndianModes TUCS4Converter::eMode() const
{
    return m_eMode;
}


// ---------------------------------------------------------------------------
//  TUCS4BEConverter: Constructors and Destructor
// ---------------------------------------------------------------------------
TUCS4BEConverter::TUCS4BEConverter() :

    TUCS4Converter(tCIDLib::EEndianModes::Big)
{
}

TUCS4BEConverter::TUCS4BEConverter(const TUCS4BEConverter& tcvtToCopy) :

    TUCS4Converter(tcvtToCopy)
{
}

TUCS4BEConverter::~TUCS4BEConverter()
{
}


// ---------------------------------------------------------------------------
//  TUCS4BEConverter: Public operators
// ---------------------------------------------------------------------------
TUCS4BEConverter&
TUCS4BEConverter::operator=(const TUCS4BEConverter& tcvtToAssign)
{
    if (this == &tcvtToAssign)
        return *this;

    TParent::operator=(tcvtToAssign);
    return *this;
}



// ---------------------------------------------------------------------------
//  TUCS4LEConverter: Constructors and Destructor
// ---------------------------------------------------------------------------
TUCS4LEConverter::TUCS4LEConverter() :

    TUCS4Converter(tCIDLib::EEndianModes::Little)
{
}

TUCS4LEConverter::TUCS4LEConverter(const TUCS4LEConverter& tcvtToCopy) :

    TUCS4Converter(tcvtToCopy)
{
}

TUCS4LEConverter::~TUCS4LEConverter()
{
}


// ---------------------------------------------------------------------------
//  TUCS4LEConverter: Public operators
// ---------------------------------------------------------------------------
TUCS4LEConverter&
TUCS4LEConverter::operator=(const TUCS4LEConverter& tcvtToAssign)
{
    if (this == &tcvtToAssign)
        return *this;

    TParent::operator=(tcvtToAssign);
    return *this;
}


