//
// FILE NAME: CIDLib_String.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/16/1992
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
//  This file implements the standard string classes. These classes provide
//  all of the usual string manipulation services.
//
// CAVEATS/GOTCHAS:
//
//  1)  Token replacement is an important component of the string class. A
//      token has the following potential formats:
//
//          %(X)        - X is the token character
//          %(X,W)      - W is the field width, negative means left justify
//          %(X,W,F)    - F is the fill character for the field
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
//  Do our RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TString,TObject)


namespace
{
    namespace CIDLib_String
    {
        // -----------------------------------------------------------------------
        //  c1FmtVersion
        //      A format version that we stream out, so that we can upgrade the
        //      persistence format automatically later if needed.
        //
        //  c4ReallocBlock
        //      The steps that we try to alloc strings in.
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard1   c1FmtVersion    = 1;
        constexpr tCIDLib::TCard4   c4ReallocBlock  = 64;
    }
}



// ---------------------------------------------------------------------------
//  Global operators
// ---------------------------------------------------------------------------

// Concatenate two strings and return the result
TString operator+(const TStringView& strv1, const TStringView& strv2)
{
    TString strRet(strv1, strv2.c4Length());
    strRet.Append(strv2);
    return strRet;
}


// ---------------------------------------------------------------------------
//   CLASS: TString
//  PREFIX: str
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TString: Public, static methods
// ---------------------------------------------------------------------------

// These have to be out of line because bCompare takes a string view and it's after our header
tCIDLib::TBoolean TString::bComp(const TString& str1, const TString& str2) noexcept
{
    return str1.bCompare(str2);
}

tCIDLib::TBoolean TString::bCompI(const TString& str1, const TString& str2)  noexcept
{
    return str1.bCompareI(str2);
}

tCIDLib::ESortComps TString::eComp(const TString& str1, const TString& str2) noexcept
{
    return str1.eCompare(str2);
}

tCIDLib::ESortComps TString::eCompI(const TString& str1, const TString& str2) noexcept
{
    return str1.eCompareI(str2);
}

//
//  We have various scenarios where we need to find a given token, find all the tokens
//  or find the tokens and text that goes between them. This function provides the core
//  capabilities for that that. You pass in the start of the string and it returns the next
//  thing it finds, which is either a run of text or a token (or end of string or bad
//  format error.)
//
//  If it returns a token, then it returns the token info. In either case it returns the
//  start and end pointers of what it found. The end is the character after the last char,
//  which can be the null if at the end. To keep going, move up the pointer to the end
//  index (if End is not returned.)
//
//  The incoming pointer can be moved upwards by one character as well, so it is not
//  a const pointer. If we hit an escaped percent, we won't to only return the actual
//  percentage as text, so we have to move the start pointer forward.
//
//  This stuff is used in some low level scenarios, so we try to make use of as little
//  standard functionality as possible. So it's more complex than it otherwise would
//  be. We also want to be very efficient since this will be indirectly used all over
//  the place. So it's mostly for internal use and the general public would use things
//  like eReplaceToken() or the msg loading methods of this class and TModule and such,
//  which are very convenient to use.
//
TString::ETokenFind
TString::eFindToken( const  tCIDLib::TCh*&          pszStart
                    , COP   tCIDLib::TCh&           chToken
                    , COP   tCIDLib::EHJustify&     eJustify
                    , COP   tCIDLib::TCard4&        c4FldWidth
                    , COP   tCIDLib::TCh&           chFill
                    , COP   tCIDLib::TCard4&        c4Precision
                    , COP   const tCIDLib::TCh*&    pszEnd)
{
    // States for our state machine
    enum class ETokenStates
    {
        Start
        , StartToken
        , TokenChar
        , PostChar
        , WaitWidth
        , Width
        , Precision
        , PostWidth
        , FillChar
        , WaitEnd
        , WaitEndToken
        , TextRun

        // These are termination states, end text has to be the first!
        , EndText
        , EndToken
        , Error
    };

    // Set up deafult values
    chToken         = kCIDLib::chNull;
    c4FldWidth      = 0;
    c4Precision     = 0;
    chFill          = kCIDLib::chSpace;
    eJustify        = tCIDLib::EHJustify::Right;

    // Watch for psycho scenarios
    if (!pszStart)
    {
        pszEnd = nullptr;
        return ETokenFind::BadFormat;
    }

    if (*pszStart == kCIDLib::chNull)
    {
        pszEnd = nullptr;
        return ETokenFind::End;
    }

    // We need a pointer to run up to the end of whatever we find
    pszEnd = pszStart;

    // Some states need to remember a starting point
    const tCIDLib::TCh* pszStateFirst = nullptr;

    // And let's do our state machine
    ETokenStates eState = ETokenStates::Start;
    while (*pszEnd && (eState < ETokenStates::EndText))
    {
        const tCIDLib::TCh chCur = *pszEnd;
        switch(eState)
        {
            case ETokenStates::Start :
                //
                //  If it's a percent, then assume for the moment it's a token, else
                //  has to be a text run.
                //
                if (chCur == kCIDLib::chPercentSign)
                {
                    eState = ETokenStates::StartToken;
                    pszEnd++;
                }
                 else
                {
                    eState = ETokenStates::TextRun;
                }
                break;

            case ETokenStates::TextRun :
                //
                //  It's a text run, so we are just going to eat chars till we hit
                //  another percent or the end. Else just move forward to the next
                //  char.
                //
                if (!chCur || (chCur == kCIDLib::chPercentSign))
                    eState = ETokenStates::EndText;
                else
                    pszEnd++;
                break;

            case ETokenStates::StartToken :
                if (chCur == kCIDLib::chOpenParen)
                {
                    // Gotta be a token or bad format, so move forward with token
                    eState = ETokenStates::TokenChar;
                    pszEnd++;
                }
                 else
                {
                    //
                    //  A false start, so go to text run state.
                    //
                    //  If it's another percent, that means an escaped percent, we only
                    //  want to return the first one, so we move both start and end
                    //  forward. Else we don't move end forward.
                    //
                    eState = ETokenStates::TextRun;
                    if (chCur == kCIDLib::chPercentSign)
                    {
                        pszEnd++;
                        pszStart++;
                    }
                }
                break;

            case ETokenStates::TokenChar :
                if (TRawStr::bIsSpace(chCur))
                {
                    // Can't be a white space character
                    eState = ETokenStates::Error;
                }
                 else
                {
                    // Keep this character as the token and do post char stuff
                    chToken = chCur;
                    eState = ETokenStates::PostChar;
                    pszEnd++;
                }

                break;

            case ETokenStates::PostChar :
                // We have to see a comma or close paren
                if (chCur == kCIDLib::chComma)
                {
                    eState = ETokenStates::WaitWidth;
                    pszEnd++;
                }
                 else if (chCur == kCIDLib::chCloseParen)
                {
                    eState = ETokenStates::EndToken;
                    pszEnd++;
                }
                 else
                {
                    eState = ETokenStates::Error;
                }
                break;

            case ETokenStates::WaitWidth :
                //
                //  It has to be either a negative or positive sign  or the first
                //  digit of the width, then we move to the width, else it is bad.
                //
                if (chCur == kCIDLib::chPlusSign)
                {
                    eJustify = tCIDLib::EHJustify::Right;
                    pszEnd++;
                    eState = ETokenStates::Width;
                }
                 else if (chCur == kCIDLib::chHyphenMinus)
                {
                    eJustify = tCIDLib::EHJustify::Left;
                    pszEnd++;
                    eState = ETokenStates::Width;
                }
                 else if (TRawStr::bIsDigit(chCur))
                {
                    // Don't move forward just move to width state
                    eState = ETokenStates::Width;
                }
                 else
                {
                    eState = ETokenStates::Error;
                }

                // If the state is now width, save this position
                if (eState == ETokenStates::Width)
                    pszStateFirst = pszEnd;
                break;

            case ETokenStates::Width :
            case ETokenStates::Precision :
                //
                //  These are almost exactly the same, so we do them together. The
                //  only difference is what we store and the next state.
                //
                //  If a digit, just keep going, else process
                //
                if (TRawStr::bIsDigit(chCur))
                {
                    pszEnd++;

                    //
                    //  We will accept a zero width, but no leading zero in
                    //  an otherwise non-zero number.
                    //
                    if (((pszEnd - pszStateFirst) > 1)
                    &&  (*pszStateFirst == kCIDLib::chDigit0))
                    {
                        eState = ETokenStates::Error;
                    }
                }
                 else
                {
                    if (pszEnd == pszStateFirst)
                    {
                        // We got nothing
                        eState = ETokenStates::Error;
                    }
                     else
                    {
                        //
                        //  We need to convert what we have to a number, which has to be
                        //  decimal radix. We do this manually here for reasons discussed
                        //  above. We'll use a Card8 initially so we can catch an
                        //  overflow.
                        //
                        tCIDLib::TCard8 c8Width = 0;
                        const tCIDLib::TCh* pszCur = pszStateFirst;
                        while (pszCur < pszEnd)
                        {
                            // Skip leading zeros
                            if (*pszCur != kCIDLib::chDigit0)
                            {
                                // If not the first digit, move up to make room for new one
                                if (pszCur > pszStateFirst)
                                    c8Width *= 10;
                                c8Width += tCIDLib::TCard4(*pszCur - kCIDLib::chDigit0);
                            }
                            pszCur++;
                        }

                        if (c8Width > kCIDLib::c4MaxCard)
                        {
                            //
                            //  Move the end back to the start of this width, since we
                            //  want to report it as the problem.
                            //
                            pszEnd = pszStateFirst;
                            eState = ETokenStates::Error;
                        }
                         else
                        {
                            if (eState == ETokenStates::Width)
                            {
                                c4FldWidth = tCIDLib::TCard4(c8Width);
                                if (chCur == kCIDLib::chPeriod)
                                {
                                    //
                                    //  We need to move forward past the period and reset
                                    //  set the first of state pointer.
                                    //
                                    eState = ETokenStates::Precision;
                                    pszEnd++;
                                    pszStateFirst = pszEnd;
                                }
                                 else
                                {
                                    eState = ETokenStates::PostWidth;
                                }
                            }
                             else
                            {
                                c4Precision = tCIDLib::TCard4(c8Width);
                                eState = ETokenStates::PostWidth;
                            }
                        }
                    }
                }
                break;


            case ETokenStates::PostWidth :
                // We either see the end of token or a comma
                if (chCur == kCIDLib::chComma)
                {
                    eState = ETokenStates::FillChar;
                    pszEnd++;
                }
                 else if (chCur == kCIDLib::chCloseParen)
                {
                    eState = ETokenStates::EndToken;
                    pszEnd++;
                }
                 else
                {
                    eState = ETokenStates::Error;
                }
                break;

            case ETokenStates::FillChar :
                // Take the current char as the fill and move forward
                chFill = chCur;
                eState = ETokenStates::WaitEndToken;
                pszEnd++;
                break;

            case ETokenStates::WaitEndToken :
                // It has to be the close paren
                if (chCur == kCIDLib::chCloseParen)
                {
                    eState = ETokenStates::EndToken;
                    pszEnd++;
                }
                 else
                {
                    eState = ETokenStates::Error;
                }
                break;

            case ETokenStates::EndText :
            case ETokenStates::EndToken :
                // Just place holders so we have positive end states to use below
                break;

            default :
                CIDAssert2(L"Unknown token find state");
                break;
        };
    }

    // It's legal to be in a text run at the end, it just went to the end
    if ((eState == ETokenStates::EndText)
    ||  (eState == ETokenStates::TextRun))
    {
        return ETokenFind::TextRun;
    }
     else if (eState == ETokenStates::EndToken)
    {
        return ETokenFind::Token;
    }

    // It has to be a bad format error
    return ETokenFind::BadFormat;
}


//
//  A wrapper around the more fundamental eFindToken() method above, which lets
//  the caller find the first instance of a given token in the passed string.
//
const tCIDLib::TCh*
TString::pszFindToken(  const   tCIDLib::TCh* const     pszSrc
                        , const tCIDLib::TCh            chToFind
                        , COP   tCIDLib::EHJustify&     eJustify
                        , COP   tCIDLib::TCard4&        c4FldWidth
                        , COP   tCIDLib::TCh&           chFill
                        , COP   tCIDLib::TCard4&        c4Precision
                        , COP   tCIDLib::TCard4&        c4TokenChars
                        , COP   tCIDLib::TCard4&        c4TokenCnt)
{
    // Initialize the token count
    c4TokenCnt = 0;

    eJustify = tCIDLib::EHJustify::Left;
    c4FldWidth = 0;
    chFill = kCIDLib::chNull;
    c4Precision = 0;
    c4TokenChars = 0;
    c4TokenCnt = 0;

    if (pszSrc)
    {
        const tCIDLib::TCh* pszStart = pszSrc;
        const tCIDLib::TCh* pszEnd = nullptr;
        while (*pszStart)
        {
            tCIDLib::TCh chTokenChar;
            const ETokenFind eFindRes = eFindToken
            (
                pszStart
                , chTokenChar
                , eJustify
                , c4FldWidth
                , chFill
                , c4Precision
                , pszEnd
            );

            if (eFindRes == ETokenFind::End)
                break;

            if (eFindRes == ETokenFind::Token)
            {
                c4TokenCnt++;
                if (chTokenChar == chToFind)
                {
                    c4TokenChars = (pszEnd - pszStart);
                    return pszStart;
                }
            }

            // Not found yet, so move our pointer forward for next round
            pszStart = pszEnd;
        }
    }
    return nullptr;
}


// THESE PUT OUT UPPER CASE and some folks depend on that. So don't change the case
tCIDLib::TVoid TString::FromHex(const   tCIDLib::TCard1 c1ToXlat
                                , COP   tCIDLib::TSCh&  chOne
                                , COP   tCIDLib::TSCh&  chTwo)
{
    tCIDLib::TCard1 c1Tmp = c1ToXlat & 0xF;
    if (c1Tmp)
    {
        chTwo = (c1Tmp > 9) ? tCIDLib::TSCh(kCIDLib::chLatin_A + (c1Tmp - 10))
                            : tCIDLib::TSCh(kCIDLib::chDigit0 + c1Tmp);
    }
     else
    {
        chTwo = tCIDLib::TSCh(kCIDLib::chDigit0);
    }

    c1Tmp = c1ToXlat >> 4;
    if (c1Tmp & 0xF)
    {
        chOne = (c1Tmp > 9) ? tCIDLib::TSCh(kCIDLib::chLatin_A + (c1Tmp - 10))
                            : tCIDLib::TSCh(kCIDLib::chDigit0 + c1Tmp);
    }
     else
    {
        chOne = tCIDLib::TSCh(kCIDLib::chDigit0);
    }
}

tCIDLib::TVoid TString::FromHex(const   tCIDLib::TCard1 c1ToXlat
                                , COP   tCIDLib::TCh&   chOne
                                , COP   tCIDLib::TCh&   chTwo)
{
    tCIDLib::TCard1 c1Tmp = c1ToXlat & 0xF;
    if (c1Tmp)
    {
        chTwo = (c1Tmp > 9) ? tCIDLib::TCh(kCIDLib::chLatin_A+ (c1Tmp - 10))
                            : tCIDLib::TCh(kCIDLib::chDigit0 + c1Tmp);
    }
     else
    {
        chTwo = kCIDLib::chDigit0;
    }

    c1Tmp = c1ToXlat >> 4;
    if (c1Tmp & 0xF)
    {
        chOne = (c1Tmp > 9) ? tCIDLib::TCh(kCIDLib::chLatin_A + (c1Tmp - 10))
                            : tCIDLib::TCh(kCIDLib::chDigit0 + c1Tmp);
    }
     else
    {
        chOne = kCIDLib::chDigit0;
    }
}

const TString& TString::strEmpty() noexcept
{
    static TString strEmpty;
    return strEmpty;
}

const TStringView& TString::strvEmpty() noexcept
{
    static TStringView strvEmpty(strEmpty());
    return strvEmpty;
}

TString& TString::Nul_TString() noexcept
{
    static TString strNull;
    return strNull;
}


// We pre-instantiate some common variations on the concatenation template method
template CIDLIBEXP TString TString::strConcat(const TString&, const TString&);
template CIDLIBEXP TString TString::strConcat(const tCIDLib::TCh*&, const tCIDLib::TCh*&);
template CIDLIBEXP TString TString::strConcat(const TString&, const tCIDLib::TCh*&);
template CIDLIBEXP TString TString::strConcat(const tCIDLib::TCh*&, const TString&);


template CIDLIBEXP TString TString::strConcat(const TString&, const TString&, const TString&);
template CIDLIBEXP TString TString::strConcat(const tCIDLib::TCh*&, const tCIDLib::TCh*&, const tCIDLib::TCh*&);

template CIDLIBEXP TString TString::strConcat(const TString&, const tCIDLib::TCh&, const TString&);
template CIDLIBEXP TString TString::strConcat(const tCIDLib::TCh*&, const tCIDLib::TCh&, const tCIDLib::TCh*&);
template CIDLIBEXP TString TString::strConcat(const TString&, const tCIDLib::TCh&, const tCIDLib::TCh*&);
template CIDLIBEXP TString TString::strConcat(const tCIDLib::TCh*&, const tCIDLib::TCh&, const TString&);

template CIDLIBEXP TString TString::strConcat(const tCIDLib::TCh&, const TString&);
template CIDLIBEXP TString TString::strConcat(const tCIDLib::TCh&, const tCIDLib::TCh*&);



// ---------------------------------------------------------------------------
//  TString: Constructors and Destructor
// ---------------------------------------------------------------------------

TString::TString(const tCIDLib::TCard4 c4BufSize) :

    m_strbData(c4BufSize)
{
}

TString::TString(const tCIDLib::TCh chInit) :

    m_strbData(1UL)
{
    tCIDLib::TCh achTmp[2] = { chInit, kCIDLib::chNull };
    m_strbData.Append(achTmp, 1);
}

TString::TString(const TStringView& strvSrc, const tCIDLib::TCard4 c4ExtraChars) :

    m_strbData(strvSrc.pszBuffer(), strvSrc.c4Length(), c4ExtraChars)
{
}

TString::TString(tCIDLib::TCh* pszToAdopt, const tCIDLib::EAdoptOpts eAdopt) :

    m_strbData(pszToAdopt, eAdopt)
{
}

TString::TString(const tCIDLib::TErrCode midToLoad, const TFacility& facSrc) :

    m_strbData(facSrc.pszLoadCIDMsg(midToLoad), 0, 64)
{
}

TString::TString(const  tCIDLib::TMsgId     midToLoad
                , const TFacility&          facSrc
                , const MFormattable&       fmtblToken1
                , const MFormattable&       fmtblToken2
                , const MFormattable&       fmtblToken3
                , const MFormattable&       fmtblToken4) :

    m_strbData(0UL)
{
    tCIDLib::TBoolean bLoaded;
    const tCIDLib::TCh* pszMsg = facSrc.pszLoadCIDMsg(midToLoad, bLoaded);
    Set(pszMsg, 64);

    //
    //  If we loaded it ok, then do token replacement. If not, we got a default 'msg not
    //  found' msg.
    //
    if (bLoaded)
    {
        if (!MFormattable::bIsNullObject(fmtblToken1))
            eReplaceToken(fmtblToken1, L'1');
        if (!MFormattable::bIsNullObject(fmtblToken2))
            eReplaceToken(fmtblToken2, L'2');
        if (!MFormattable::bIsNullObject(fmtblToken3))
            eReplaceToken(fmtblToken3, L'3');
        if (!MFormattable::bIsNullObject(fmtblToken4))
            eReplaceToken(fmtblToken4, L'4');
    }
}

TString::TString(const  MFormattable& fmtblInitValue, const TStreamFmt& strmfToUse) :

    m_strbData(0UL)
{
    TTextStringOutStream strmTmp(1024UL, strmfToUse);
    strmTmp << fmtblInitValue << kCIDLib::FlushIt;
    Set(strmTmp.strData());
}

TString::TString(const MFormattable& fmtblInitValue) :

    m_strbData(0UL)
{
    TTextStringOutStream strmTmp(1024UL);
    strmTmp << fmtblInitValue << kCIDLib::FlushIt;
    Set(strmTmp.strData());
}


// Create a string from a pattern with replacement tokens

TString::TString(const  TString&        strPattern
                , const MFormattable&   fmtblToken1
                , const MFormattable&   fmtblToken2
                , const MFormattable&   fmtblToken3
                , const MFormattable&   fmtblToken4) :

    m_strbData(strPattern.pszBuffer(), strPattern.c4Length(), 64)
{
    // Handle the token replacement
    if (!MFormattable::bIsNullObject(fmtblToken1))
        eReplaceToken(fmtblToken1, L'1');
    if (!MFormattable::bIsNullObject(fmtblToken2))
        eReplaceToken(fmtblToken2, L'2');
    if (!MFormattable::bIsNullObject(fmtblToken3))
        eReplaceToken(fmtblToken3, L'3');
    if (!MFormattable::bIsNullObject(fmtblToken4))
        eReplaceToken(fmtblToken4, L'4');
}

TString::TString(const  tCIDLib::TCh* const pszPattern
                , const MFormattable&       fmtblToken1
                , const MFormattable&       fmtblToken2
                , const MFormattable&       fmtblToken3
                , const MFormattable&       fmtblToken4) :

    m_strbData(pszPattern, 0, 64)
{
    // Handle the token replacement
    if (!MFormattable::bIsNullObject(fmtblToken1))
        eReplaceToken(fmtblToken1, L'1');
    if (!MFormattable::bIsNullObject(fmtblToken2))
        eReplaceToken(fmtblToken2, L'2');
    if (!MFormattable::bIsNullObject(fmtblToken3))
        eReplaceToken(fmtblToken3, L'3');
    if (!MFormattable::bIsNullObject(fmtblToken4))
        eReplaceToken(fmtblToken4, L'4');
}


// Convert a short character string
TString::TString(const tCIDLib::TSCh* const pszInit) :

    m_strbData(0UL)
{
    if (pszInit)
        SetFromShort(pszInit);
}


// ---------------------------------------------------------------------------
//  TString: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TString::operator==(const TString& strSrc) const noexcept
{
    if (this == &strSrc)
        return kCIDLib::True;

    // Can't be equal if not the same length
    if (m_strbData.c4CurEnd() != strSrc.m_strbData.c4CurEnd())
        return kCIDLib::False;

    // We know the lengths are equal, so just do a memory compare now
    const tCIDLib::ESortComps eRes = TRawMem::eCompareMemBuf
    (
        m_strbData.pszBuffer(), strSrc.m_strbData.pszBuffer(), strSrc.m_strbData.c4CurEnd() * kCIDLib::c4CharBytes
    );
    return (eRes == tCIDLib::ESortComps::Equal);
}

tCIDLib::TBoolean TString::operator==(const TStringView& strvSrc) const noexcept
{
    //
    //  If we already have the length, use it. Else it's not worth getting the length,
    //  since it's a raw string (that the length hasn't been gotten on yet) and we'd
    //  likely end up having to scan it once for the length and then again for the
    //  comparison. We can lose either way, but this is probabyl the safer bet most of the
    //  time.
    //
    if (strvSrc.bHaveLength() && (strvSrc.c4Length() != c4Length()))
        return kCIDLib::False;
    return (TRawStr::eCompareStr(pszBuffer(), strvSrc.pszBuffer()) == tCIDLib::ESortComps::Equal);
}

tCIDLib::TBoolean TString::operator==(const tCIDLib::TSCh* const pszSrc) const noexcept
{
    // Convert the short string so we can compare it
    tCIDLib::TCh* pszConverted = TRawStr::pszConvert(pszSrc);
    TArrayJanitor<tCIDLib::TCh> janConverted(pszConverted);
    return (TRawStr::eCompareStr(pszBuffer(), pszConverted) == tCIDLib::ESortComps::Equal);
}

tCIDLib::TBoolean TString::operator<=(const tCIDLib::TCh* const pszSrc) const noexcept
{
    const tCIDLib::ESortComps eComp = TRawStr::eCompareStr(pszBuffer(), pszSrc);
    return (eComp != tCIDLib::ESortComps::FirstGreater);
}

tCIDLib::TBoolean TString::operator>=(const tCIDLib::TCh* const pszSrc) const noexcept
{
    const tCIDLib::ESortComps eComp = TRawStr::eCompareStr(pszBuffer(), pszSrc);
    return (eComp != tCIDLib::ESortComps::FirstLess);
}

tCIDLib::TBoolean TString::operator<(const tCIDLib::TCh* const pszSrc) const noexcept
{
    return TRawStr::eCompareStr(pszBuffer(), pszSrc) == tCIDLib::ESortComps::FirstLess;
}

tCIDLib::TBoolean TString::operator>(const tCIDLib::TCh* const pszSrc) const noexcept
{
    return TRawStr::eCompareStr(pszBuffer(), pszSrc) == tCIDLib::ESortComps::FirstGreater;
}

tCIDLib::TVoid TString::operator+=(const TStringView& strvSrc)
{
    m_strbData.Append(strvSrc.pszBuffer(), strvSrc.c4Length());
}



// ---------------------------------------------------------------------------
//  TString: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Append a count of characters
tCIDLib::TVoid TString::Append(const tCIDLib::TCh chSrc, const tCIDLib::TCard4 c4Count)
{
    if (!c4Count)
        return;

    // See if we have enough to hold it, else expand. Maintain the current text
    m_strbData.ExpandBy(c4Count + 16, kCIDLib::True);

    tCIDLib::TCh* pszTar = m_strbData.pszAtEnd();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        *pszTar++ = chSrc;

    m_strbData.IncEnd(c4Count);
}


tCIDLib::TVoid TString::Append(const TStringView& strvSrc)
{
    m_strbData.Append(strvSrc.pszBuffer(), strvSrc.c4Length());
}

tCIDLib::TVoid TString::Append(const TStringView& strvSrc1, const TStringView& strvSrc2)
{
    const tCIDLib::TCard4 c4SrcLen = strvSrc1.c4Length() + strvSrc2.c4Length();
    m_strbData.ExpandBy(c4SrcLen, kCIDLib::True);
    m_strbData.Append(strvSrc1.pszBuffer(), strvSrc1.c4Length());
    m_strbData.Append(strvSrc2.pszBuffer(), strvSrc2.c4Length());
}

tCIDLib::TVoid TString::Append( const   TStringView& strvSrc1
                                , const TStringView& strvSrc2
                                , const TStringView& strvSrc3)
{
    const tCIDLib::TCard4 c4SrcLen
    (
        strvSrc1.c4Length() + strvSrc2.c4Length() + strvSrc3.c4Length()
    );
    m_strbData.ExpandBy(c4SrcLen, kCIDLib::True);
    m_strbData.Append(strvSrc1.pszBuffer(), strvSrc1.c4Length());
    m_strbData.Append(strvSrc2.pszBuffer(), strvSrc2.c4Length());
    m_strbData.Append(strvSrc3.pszBuffer(), strvSrc3.c4Length());
}


tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TCard1     c1ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    tCIDLib::TZStr128 szTmp;
    if (!TRawStr::bFormatVal(c1ToFmt
                            , szTmp
                            , tCIDLib::c4MaxBufChars(szTmp)
                            , eRadix
                            , c4GroupSize
                            , chGroupSep))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_FormatBin
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    Append(szTmp);
}

tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TCard2     c2ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    tCIDLib::TZStr128 szTmp;
    if (!TRawStr::bFormatVal(c2ToFmt
                            , szTmp
                            , tCIDLib::c4MaxBufChars(szTmp)
                            , eRadix
                            , c4GroupSize
                            , chGroupSep))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_FormatBin
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    Append(szTmp);
}

tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TCard4     c4ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    tCIDLib::TZStr128 szTmp;
    if (!TRawStr::bFormatVal(c4ToFmt
                            , szTmp
                            , tCIDLib::c4MaxBufChars(szTmp)
                            , eRadix
                            , c4GroupSize
                            , chGroupSep))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_FormatBin
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    Append(szTmp);
}

tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TCard8     c8ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    tCIDLib::TZStr128 szTmp;
    if (!TRawStr::bFormatVal(c8ToFmt
                            , szTmp
                            , tCIDLib::c4MaxBufChars(szTmp)
                            , eRadix
                            , c4GroupSize
                            , chGroupSep))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_FormatBin
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    Append(szTmp);
}

tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TFloat8    f8ToFmt
                        , const tCIDLib::TCard4     c4DecPlaces)
{
    const tCIDLib::ETrailFmts eTrail
    (
        c4DecPlaces == 0 ? tCIDLib::ETrailFmts::Ignore : tCIDLib::ETrailFmts::Zeroes
    );
    tCIDLib::TZStr512 szTmp = L"";
    if (!TRawStr::bFormatVal(f8ToFmt
                            , szTmp
                            , c4DecPlaces
                            , tCIDLib::c4MaxBufChars(szTmp)
                            , eTrail))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_FormatBin
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    Append(szTmp);
}

tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TFloat8    f8ToFmt
                        , const tCIDLib::TCard4     c4DecPlaces
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCh        chDecimal
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    const tCIDLib::ETrailFmts eTrail
    (
        c4DecPlaces == 0 ? tCIDLib::ETrailFmts::Ignore : tCIDLib::ETrailFmts::Zeroes
    );
    tCIDLib::TZStr512 szTmp = L"";
    if (!TRawStr::bFormatVal(f8ToFmt
                            , szTmp
                            , c4DecPlaces
                            , tCIDLib::c4MaxBufChars(szTmp)
                            , eTrail
                            , chNegSign
                            , chDecimal
                            , c4GroupSize
                            , chGroupSep))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_FormatBin
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    Append(szTmp);
}


tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TInt1      i1ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    tCIDLib::TZStr128 szTmp = L"";
    if (!TRawStr::bFormatVal(i1ToFmt
                            , szTmp
                            , tCIDLib::c4MaxBufChars(szTmp)
                            , eRadix
                            , chNegSign
                            , c4GroupSize
                            , chGroupSep))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_FormatBin
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    Append(szTmp);
}

tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TInt2      i2ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    tCIDLib::TZStr128 szTmp = L"";
    if (!TRawStr::bFormatVal(i2ToFmt
                            , szTmp
                            , tCIDLib::c4MaxBufChars(szTmp)
                            , eRadix
                            , chNegSign
                            , c4GroupSize
                            , chGroupSep))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_FormatBin
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    Append(szTmp);
}

tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TInt4      i4ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    tCIDLib::TZStr128 szTmp = L"";
    if (!TRawStr::bFormatVal(i4ToFmt
                            , szTmp
                            , tCIDLib::c4MaxBufChars(szTmp)
                            , eRadix
                            , chNegSign
                            , c4GroupSize
                            , chGroupSep))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_FormatBin
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    Append(szTmp);
}


tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TInt8      i8ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    tCIDLib::TZStr128 szTmp = L"";
    if (!TRawStr::bFormatVal(i8ToFmt
                            , szTmp
                            , tCIDLib::c4MaxBufChars(szTmp)
                            , eRadix
                            , chNegSign
                            , c4GroupSize
                            , chGroupSep))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_FormatBin
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    Append(szTmp);
}

// Just force this one to the Int4 one
tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TSInt      iToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    AppendFormatted(tCIDLib::TInt4(iToFmt), eRadix, chNegSign, c4GroupSize, chGroupSep);
}

//
//  The trailing parameter is a dummy one that helps us deal with an otherwise
//  much more messy issue with the variadic recursion processing of Format().
//
tCIDLib::TVoid TString::AppendFormatted(const MFormattable& fmtblSrc, const tCIDLib::TCard4)
{
    TTextStringOutStream strmTar(1024UL);
    strmTar << fmtblSrc << kCIDLib::FlushIt;
    Append(strmTar.strData());
}


tCIDLib::TVoid
TString::AppendSubStr( const   TStringView&     strvSrc
                        , const tCIDLib::TCard4 c4Start
                        , const tCIDLib::TCard4 c4Len)
{
    // Is the source string is empty, then just return cause nothing to do
    if (strvSrc.bIsEmpty())
        return;

    // Make sure the start is within the current length of the source if not zero
    if (c4Start && (c4Start >= strvSrc.c4Length()))
    {
        //
        //  If it'as at the end, and the length is max card, just do nothing, since basically
        //  they are just telling us to copy what's available, and that's just nothing in
        //  this case. Else throw.
        //
        if ((c4Start == strvSrc.c4Length()) && (c4Len == kCIDLib::c4MaxCard))
            return;

        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Start)
            , clsThis()
            , TCardinal(strvSrc.c4Length())
        );
    }

    //
    //  If the len is c4MaxCard, then set to exactly the rest of the source
    //  string.
    //
    tCIDLib::TCard4 c4ActualLen = c4Len;
    if (c4ActualLen == kCIDLib::c4MaxCard)
    {
        c4ActualLen = strvSrc.c4Length() - c4Start;
    }
     else
    {
        if (c4Start + c4ActualLen > strvSrc.c4Length())
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_IndexError
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::BadParms
                , TCardinal(c4Start + c4Len)
                , clsThis()
                , TCardinal(strvSrc.c4Length())
            );
        }
    }

    // If we ended up with nothing to do, then return
    if (!c4ActualLen)
        return;

    m_strbData.Append(strvSrc.pszBufferAt(c4Start), c4ActualLen);
}


// Cap the string at the first instance of the indicated character
tCIDLib::TBoolean TString::bCapAtChar(const tCIDLib::TCh chAt)
{
    // If no match, then we are done
    tCIDLib::TCh* pszMatch = TRawStr::pszFindChar(m_strbData.pszBuffer(), chAt, 0, kCIDLib::True);
    if (!pszMatch)
        return kCIDLib::False;

    m_strbData.SetEnd(pszMatch);
    return kCIDLib::True;
}


//
//  Comparison methods for various schemes. We first do a check of lengths. If
//  they are different, we can quickly return false. Else we call the comparison
//  method. There's not really a better way to do it. They just have to be compared
//  until we get a different character or hit the end of both, which the
//  underlying methods do.
//
tCIDLib::TBoolean TString::bCompare(const TStringView& strvSrc) const noexcept
{
    if (strvSrc.bHaveLength() && (c4Length() != strvSrc.c4Length()))
        return kCIDLib::False;

    return (eCompare(strvSrc) == tCIDLib::ESortComps::Equal);
}


tCIDLib::TBoolean TString::bCompareI(const TStringView& strvSrc) const noexcept
{
    if (strvSrc.bHaveLength() && (c4Length() != strvSrc.c4Length()))
        return kCIDLib::False;

    return (eCompareI(strvSrc) == tCIDLib::ESortComps::Equal);
}

tCIDLib::TBoolean TString::bCompareN(const  TStringView&    strvSrc
                                    , const tCIDLib::TCard4 c4Count) const noexcept
{
    // If we are less than the min of the indicted count or source length, obviously can't be
    if (strvSrc.bHaveLength() && (c4Length() < tCIDLib::MinVal(c4Count, strvSrc.c4Length())))
        return kCIDLib::False;

    return (eCompareN(strvSrc, c4Count) == tCIDLib::ESortComps::Equal);
}

tCIDLib::TBoolean TString::bCompareNI(  const   TStringView&    strvSrc
                                        , const tCIDLib::TCard4 c4Count) const noexcept
{
    // If we are less than the min of the indicted count or source length, obviously can't be
    if (strvSrc.bHaveLength() && (c4Length() < tCIDLib::MinVal(c4Count, strvSrc.c4Length())))
        return kCIDLib::False;

    return (eCompareNI(strvSrc, c4Count) == tCIDLib::ESortComps::Equal);
}


//
//  Find's the Nth token (based on the passed separator character, and puts
//  it into the passed string.
//
tCIDLib::TBoolean
TString::bExtractNthToken(  const   tCIDLib::TCard4     c4TokInd
                            , const tCIDLib::TCh        chSepChar
                            , COP   TString&            strToFill
                            , const tCIDLib::TBoolean   bStripWS) const noexcept
{
    strToFill.Clear();

    //
    //  Ok, find the start of the token. We do a loop here. Each one is
    //  finding the start of the indicated token. We do < toknum here. If
    //  tok number is 0, we fall straight through with the start of the
    //  buffer as the start of the token. Else, we keep moving forward until
    //  we count through toknum sep chars.
    //
    const tCIDLib::TCh* pchStart = m_strbData.pszBuffer();
    const tCIDLib::TCh* pchEnd = m_strbData.pszAtEnd();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TokInd; c4Index++)
    {
        // Find the next sep char
        while (pchStart < pchEnd)
        {
            if (*pchStart == chSepChar)
                break;
            pchStart++;
        }

        if (pchStart == pchEnd)
            break;

        // Move forward to start searching for the next one
        pchStart++;
    }

    if (pchStart == pchEnd)
    {
        strToFill.Clear();
        return kCIDLib::False;
    }

    //
    //  If we got here, then we've found the start offset of the token,
    //  and we need to now find the end, which may be the end of the buffer.
    //
    const tCIDLib::TCh* pchTokEnd = pchStart + 1;
    while (pchTokEnd < pchEnd)
    {
        if (*pchTokEnd == chSepChar)
                break;
        pchTokEnd++;
    }

    // If it's empty, then optimize. We return an empty token
    if (pchTokEnd == pchStart)
    {
        strToFill.Clear();
        return kCIDLib::True;
    }

    //
    //  If stripping whitespace, then skip any leading whitespace. It's
    //  way more efficient to do it now than to put it into the target
    //  and then strip it there.
    //
    if (bStripWS)
    {
        while ((pchStart < pchTokEnd) && TRawStr::bIsSpace(*pchStart))
            pchStart++;
    }

    //
    //  If we stripped it all out, that's ok. It's still at token that
    //  we found, it's just empty.
    //
    if (pchStart == pchTokEnd)
        return kCIDLib::True;

    // We have one, so copy out to the target string
    strToFill.FromZStr(pchStart, m_strbData.c4CalcBufDiff(pchTokEnd, pchStart));

    // And strip trailing now, if asked
    if (bStripWS)
        strToFill.Strip(kCIDLib::szWhitespace, tCIDLib::EStripModes::Trailing);
    return kCIDLib::True;
}


//
//  Returns pointers to the text inside any leading/trailing text. If none, then returns False,
//  else returns true and sets the pointers.
//
tCIDLib::TBoolean TString::bFindTextBody(const  tCIDLib::TCh*&  pszStart
                                        , const tCIDLib::TCh*&  pszEnd) const
{
    tCIDLib::TCard4 c4Start;
    tCIDLib::TCard4 c4End;
    const tCIDLib::TBoolean bRes = TRawStr::bFindTextBody(pszBuffer(), c4Start, c4End, c4Length());

    if (bRes)
    {
        pszStart = pszBufferAt(c4Start);
        pszEnd = pszBufferAt(c4End);
    }
     else
    {
        pszStart = nullptr;
        pszEnd = nullptr;
    }
    return bRes;
}



//
//  This method will return a list of all of the token characters found
//  in the passed source string. It returns a string of the characters
//  found. If a token is found more than once, it's in the list more than
//  once. They are returned in the order found.
//
tCIDLib::TBoolean TString::bFindTokenList(COP TString& strToFill) const
{
    strToFill.Clear();

    const tCIDLib::TCh* pszStart = m_strbData.pszBuffer();
    const tCIDLib::TCh* pszEnd = nullptr;
    while (*pszStart)
    {
        tCIDLib::TCh        chFill;
        tCIDLib::TCard4     c4FldWidth;
        tCIDLib::TCard4     c4Precision;
        tCIDLib::EHJustify  eJustify;

        tCIDLib::TCh chTokenChar;
        const ETokenFind eFindRes = eFindToken
        (
            pszStart
            , chTokenChar
            , eJustify
            , c4FldWidth
            , chFill
            , c4Precision
            , pszEnd
        );

        if (eFindRes == ETokenFind::End)
            break;

		if (eFindRes == ETokenFind::Token)
            strToFill.Append(chTokenChar);

        pszStart = pszEnd;
    }
    return !strToFill.bIsEmpty();
}



tCIDLib::TBoolean
TString::bFirstOccurrence(  const   tCIDLib::TCh            chTarget
                            , COP   tCIDLib::TCard4&        c4Pos
                            , const tCIDLib::TBoolean       bCaseSensitive) const noexcept
{
    //
    //  See if there is a match.
    //
    //  NOTE: We don't pass in the position here, since its just an output
    //          parameter here. We pass in zero to start at the beginning!
    //
    const tCIDLib::TCh* pszMatch = TRawStr::pszFindChar(pszBuffer(), chTarget, 0, bCaseSensitive);

    // If not, then get out
    if (!pszMatch)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // Calc the position
    c4Pos = pszMatch - m_strbData.pszBuffer();
    return kCIDLib::True;
}

tCIDLib::TBoolean
TString::bFirstOccurrence(  const   TString&                strSubStr
                            , COP   tCIDLib::TCard4&        c4Pos
                            , const tCIDLib::TBoolean       bAnyChar
                            , const tCIDLib::TBoolean       bCaseSensitive) const
{
    // See if there is a match, starting at 0
    const tCIDLib::TCh* pszMatch = nullptr;
    if (bAnyChar)
        pszMatch = TRawStr::pszFindChars(pszBuffer(), strSubStr.pszBuffer(), 0, bCaseSensitive);
    else
        pszMatch = TRawStr::pszFindSubStr(pszBuffer(), strSubStr.pszBuffer(), 0, bCaseSensitive);

    // If not, then get out
    if (!pszMatch)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // Calc the position and return true
    c4Pos = m_strbData.c4CalcBufDiff(pszMatch);
    return kCIDLib::True;
}


tCIDLib::TBoolean TString::bIsAlpha() const noexcept
{
    const tCIDLib::TCh* pszCur = pszBuffer();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_strbData.c4CurEnd(); c4Index++)
    {
        if (!TRawStr::bIsAlpha(pszCur[c4Index]))
            return kCIDLib::False;
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean TString::bIsAlphaNum() const noexcept
{
    const tCIDLib::TCh* pszCur = pszBuffer();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_strbData.c4CurEnd(); c4Index++)
    {
        if (!TRawStr::bIsAlphaNum(pszCur[c4Index]))
            return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TString::bLastOccurrence(   const   tCIDLib::TCh            chTarget
                            , COP   tCIDLib::TCard4&        c4Pos
                            , const tCIDLib::TBoolean       bCaseSensitive) const
{
    // See if there is a match
    const tCIDLib::TCh* pszMatch = TRawStr::pszFindLastChar
    (
        pszBuffer(), chTarget, bCaseSensitive
    );

    // If not, then get out
    if (!pszMatch)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // Calc the position and return true
    c4Pos = m_strbData.c4CalcBufDiff(pszMatch);
    return kCIDLib::True;
}

tCIDLib::TBoolean
TString::bLastOccurrence(   const   TString&                strSubStr
                            , COP   tCIDLib::TCard4&        c4Pos
                            , const tCIDLib::TBoolean       bAnyChar
                            , const tCIDLib::TBoolean       bCaseSensitive) const
{
    // See if there is a match
    const tCIDLib::TCh* pszMatch = nullptr;
    if (bAnyChar)
        pszMatch = TRawStr::pszFindLastChars(pszBuffer(), strSubStr.pszBuffer(), bCaseSensitive);
    else
        pszMatch = TRawStr::pszFindLastSubStr(pszBuffer(), strSubStr.pszBuffer(), bCaseSensitive);

    // If not, then get out
    if (!pszMatch)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // Calc the position of the match and return true
    c4Pos = m_strbData.c4CalcBufDiff(pszMatch);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TString::bNextOccurrence(   const   tCIDLib::TCh            chTarget
                            , CIOP  tCIDLib::TCard4&        c4Pos
                            , const tCIDLib::TBoolean       bCaseSensitive) const
{
    // If over the length, then an error
    if (c4Pos >= c4Length())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Pos)
            , clsThis()
            , TCardinal(c4Length())
        );
    }

    // See if there is a match. If not, we are done
    const tCIDLib::TCh* pszMatch = TRawStr::pszFindChar(pszBuffer(), chTarget, c4Pos + 1, bCaseSensitive);
    if (!pszMatch)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // Calc the position
    c4Pos = m_strbData.c4CalcBufDiff(pszMatch);
    return kCIDLib::True;
}

tCIDLib::TBoolean
TString::bNextOccurrence(   const   TString&                strSubStr
                            , CIOP  tCIDLib::TCard4&        c4Pos
                            , const tCIDLib::TBoolean       bAnyChar
                            , const tCIDLib::TBoolean       bCaseSensitive) const
{
    // If position is beyond the end, then an error
    if (c4Pos >= c4Length())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Pos)
            , clsThis()
            , TCardinal(c4Length())
        );
    }

    // See if there is a match
    const tCIDLib::TCh* pszMatch = nullptr;
    if (bAnyChar)
        pszMatch = TRawStr::pszFindChars(pszBuffer(), strSubStr.pszBuffer(), c4Pos + 1, bCaseSensitive);
    else
        pszMatch = TRawStr::pszFindSubStr(pszBuffer(), strSubStr.pszBuffer(), c4Pos + 1, bCaseSensitive);
    const tCIDLib::TCh* pszLast = pszMatch;

    // If not, then get out with negative result
    if (!pszMatch)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // Calc the position of the match and return true
    c4Pos = m_strbData.c4CalcBufDiff(pszLast);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TString::bPrevOccurrence(   const   tCIDLib::TCh            chTarget
                            , CIOP  tCIDLib::TCard4&        c4Pos
                            , const tCIDLib::TBoolean       bCaseSensitive) const
{
    // If the position is zero, then no further to look
    if (!c4Pos)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // If starting point is beyond the string length, then an error
    if (c4Pos > c4Length())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Pos)
            , clsThis()
            , TCardinal(c4Length())
        );
    }

    const tCIDLib::TCh* pszMatch = nullptr;
    pszMatch = TRawStr::pszFindPrevChar(pszBuffer(), chTarget, c4Pos - 1, bCaseSensitive);
    if (!pszMatch)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // Calc the position of the match and return true
    c4Pos = m_strbData.c4CalcBufDiff(pszMatch);
    return kCIDLib::True;
}

tCIDLib::TBoolean
TString::bPrevOccurrence(   const   TString&                strSubStr
                            , CIOP  tCIDLib::TCard4&        c4Pos
                            , const tCIDLib::TBoolean       bAnyChar
                            , const tCIDLib::TBoolean       bCaseSensitive) const
{
    // If the position is zero, then no further to look
    if (!c4Pos)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // In debug mode, make sure start is within string
    if (c4Pos >= c4Length())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Pos)
            , clsThis()
            , TCardinal(c4Length())
        );
    }

    const tCIDLib::TCh* pszFind = nullptr;

    //
    //  If we are looking for any character, then we can do a simpler
    //  search. If we are looking for the previous substring, then we
    //  want to do a smarter search.
    //
    if (bAnyChar)
    {
        //
        //  All we have to do is find the the previous char that shows up
        //  in the substring anywhere.
        //
        pszFind = TRawStr::pszFindPrevChars(pszBuffer(), strSubStr.pszBuffer(), c4Pos - 1, bCaseSensitive);
    }
     else
    {
        pszFind = TRawStr::pszFindPrevSubStr(pszBuffer(), strSubStr.pszBuffer(), c4Pos - 1, bCaseSensitive);
    }

    if (pszFind)
    {
        c4Pos = m_strbData.c4CalcBufDiff(pszFind);
        return kCIDLib::True;
    }

    c4Pos = kCIDLib::c4MaxCard;
    return kCIDLib::False;
}


// Replace the char and return whether we made any changes
tCIDLib::TBoolean
TString::bReplaceChar(  const   tCIDLib::TCh    chToReplace
                        , const tCIDLib::TCh    chReplaceWith)
{
    // Ignore nulls in either
    if (chToReplace && chReplaceWith)
    {
        //
        //  Loop through the string and replace any instances of the replacement
        //  character with the replacement character.
        //
        tCIDLib::TCh* pszTar = m_strbData.pszBuffer();
        tCIDLib::TCard4 c4Count = 0;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < m_strbData.c4CurEnd(); c4Index++)
        {
            if (pszTar[c4Index] == chToReplace)
            {
                pszTar[c4Index] = chReplaceWith;
                c4Count++;
            }
        }
        return c4Count != 0;
    }

    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcStr_CannotBeNul
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::BadParms
    );
    return kCIDLib::False;
}


tCIDLib::TBoolean
TString::bReplaceSubStr(const   TString&            strToReplace
                        , const TString&            strReplaceWith
                        , CIOP  tCIDLib::TCard4&    c4StartAt
                        , const tCIDLib::TBoolean   bReplaceAll
                        , const tCIDLib::TBoolean   bCaseSensitive)
{
    //
    //  We'll use the length of the replacement string a few times. Note
    //  that, even if it's zero, we still move forward since we are just
    //  replacing with nothing in that case. And we'll also use the length
    //  of the target text being replaced.
    //
    const tCIDLib::TCh* const pszRepText = strReplaceWith.pszBuffer();
    const tCIDLib::TCh* const pszTarText = strToReplace.pszBuffer();
    const tCIDLib::TCard4 c4RepLen = strReplaceWith.c4Length();
    const tCIDLib::TCard4 c4TarLen = strToReplace.c4Length();

    //
    //  If the target replacement string is empty, we can't do that. Or
    //  if we are shorter than the string to replace, then clearly it can't
    //  be present to replace. Or if the start at is at the end of the
    //  string (on the null), then obviously nothing to find.
    //
    if (!c4TarLen || (c4Length() < c4TarLen) || (c4StartAt == c4Length()))
    {
        c4StartAt = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // If start position is beyond the end, then an error
    if (c4StartAt > c4Length())
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4StartAt)
            , clsThis()
            , TCardinal(c4Length())
        );
    }

    tCIDLib::TBoolean bRet = kCIDLib::False;
    while(kCIDLib::True)
    {
        //
        //  See if there is a match. We call pszBuffer on each round since the below
        //  could expand the buffer.
        //
        tCIDLib::TCh* pszBase = m_strbData.pszBuffer();
        const tCIDLib::TCh* pszMatch = TRawStr::pszFindSubStr(pszBase, pszTarText, c4StartAt, bCaseSensitive);

        // If no match, we are done
        if (!pszMatch)
        {
            // Set the next index to max card in case it accidentally gets used
            c4StartAt = kCIDLib::c4MaxCard;
            break;
        }

        // We have at least replaced one thing, so return true
        bRet = kCIDLib::True;

        // Calc the position of the match
        c4StartAt = pszMatch - pszBase;

        // Replace the substring we found, just cutting if no rep text
        if (c4RepLen)
            Replace(c4StartAt, c4StartAt + (c4TarLen - 1), pszRepText, c4RepLen);
        else
            Cut(c4StartAt, c4TarLen);

        //
        //  Now move the start at up past the replacement text. We don't
        //  want to look for substrings in the substring text. That might
        //  lead to infinite recursion.
        //
        //  Note that this cannot take us past the end, since what we
        //  replaced was clearly within the string and we put this many
        //  chars back into the string at the same location. It could
        //  though take us to the end, in which case the next one will
        //  fail.
        //
        //  If we break out below because we are only doing one, this
        //  leaves it at the point where searching would commence again
        //  next time.
        //
        c4StartAt += c4RepLen;

        // If not replacing all, then one is all we do
        if (!bReplaceAll)
            break;
    }

    return bRet;
}


//
//  Splits the string into two parts, based on the passed divider char. It
//  will find the first instance of the divider, and copy out the string
//  content after that to the second half parm, cap this one at the divider
//  and strip leading/trailing whitespace on both.
//
tCIDLib::TBoolean
TString::bSplit(COP     TString&                strSecondHalf
                , const tCIDLib::TCh            chDivider
                , const tCIDLib::TBoolean       bCaseSensitive)
{
    const tCIDLib::TCard4 c4OurLen = c4Length();
    strSecondHalf.Clear();

    // Find the first divider character. If none, then we are done
    const tCIDLib::TCh* pszMatch = TRawStr::pszFindChar(pszBuffer(), chDivider, 0, bCaseSensitive);
    if (!pszMatch)
        return kCIDLib::False;

    // Calc the offset
    const tCIDLib::TCard4 c4Ofs = m_strbData.c4CalcBufDiff(pszMatch);

    //
    //  We want to skip over the divider, and we want to make sure we don't
    //  try to go beyond the end of this string. So if the char past the
    //  divider isn't beyond the last char, then we have some second half to
    //  get.
    //
    if (c4Ofs + 1 < c4OurLen)
    {
        strSecondHalf.AppendSubStr(*this, c4Ofs + 1, kCIDLib::c4MaxCard);
        strSecondHalf.Strip(kCIDLib::szWhitespace, tCIDLib::EStripModes::LeadTrail);
    }

    CapAt(c4Ofs);
    Strip(kCIDLib::szWhitespace, tCIDLib::EStripModes::LeadTrail);
    return kCIDLib::True;
}


//
//  These methods are like the xVal() methods, but these just return a
//  boolean that indicates whether the translation worked or not, whereas
//  the others throw an exception and are therefore better for situations
//  where we expect the value to always be correct. These are better for
//  when we are probing a user entered value, without having to install an
//  exception handler.
//
tCIDLib::TBoolean TString::bToBoolean(COP tCIDLib::TBoolean& bToFill) const noexcept
{
    tCIDLib::TBoolean bValid = kCIDLib::True;

    const tCIDLib::TCh* pszBuf = m_strbData.pszBuffer();
    if (TRawStr::bCompareStrI(pszBuf, L"yes")
    ||  TRawStr::bCompareStrI(pszBuf, L"on")
    ||  TRawStr::bCompareStrI(pszBuf, L"true")
    ||  TRawStr::bCompareStrI(pszBuf, L"1"))
    {
        bToFill = kCIDLib::True;
    }
     else if (TRawStr::bCompareStrI(pszBuf, L"no")
          ||  TRawStr::bCompareStrI(pszBuf, L"off")
          ||  TRawStr::bCompareStrI(pszBuf, L"false")
          ||  TRawStr::bCompareStrI(pszBuf, L"0"))
    {
        bToFill = kCIDLib::False;
    }
     else
    {
        // It might be something like "  1", so try to convert to a number
        const tCIDLib::TInt4 i4Val = TRawStr::i4AsBinary(pszBuf, bValid, tCIDLib::ERadices::Dec);

        if (bValid)
        {
            bToFill = (i4Val != 0);
        }
        else
        {
            bValid = kCIDLib::False;
            bToFill = kCIDLib::False;
        }
    }
    return bValid;
}

tCIDLib::TBoolean
TString::bToCard1(  COP     tCIDLib::TCard1&    c1ToFill
                    , const tCIDLib::ERadices   eRadix) const noexcept
{
    tCIDLib::TBoolean bValid = kCIDLib::False;
    c1ToFill = 0;
    const tCIDLib::TCard4 c4Val = TRawStr::c4AsBinary(pszBuffer(), bValid, eRadix);
    if (bValid)
    {
        if (c4Val > kCIDLib::c1MaxCard)
            bValid = kCIDLib::False;
        else
            c1ToFill = tCIDLib::TCard1(c4Val);
    }
    return bValid;
}

tCIDLib::TBoolean
TString::bToCard2(  COP     tCIDLib::TCard2&    c2ToFill
                    , const tCIDLib::ERadices   eRadix) const noexcept
{
    tCIDLib::TBoolean bValid = kCIDLib::False;
    const tCIDLib::TCard4 c4Val = TRawStr::c4AsBinary
    (
        pszBuffer(), bValid, eRadix
    );
    c2ToFill = 0;
    if (bValid)
    {
        if (c4Val > kCIDLib::c2MaxCard)
            bValid = kCIDLib::False;
        else
            c2ToFill = tCIDLib::TCard2(c4Val);
    }
    return bValid;
}

tCIDLib::TBoolean
TString::bToCard4(  COP     tCIDLib::TCard4&    c4ToFill
                    , const tCIDLib::ERadices   eRadix) const noexcept
{
    tCIDLib::TBoolean bValid = kCIDLib::False;
    c4ToFill = TRawStr::c4AsBinary(pszBuffer(), bValid, eRadix);
    return bValid;
}

tCIDLib::TBoolean
TString::bToCard8(  COP     tCIDLib::TCard8&    c8ToFill
                    , const tCIDLib::ERadices   eRadix) const noexcept
{
    tCIDLib::TBoolean bValid = kCIDLib::False;
    c8ToFill = TRawStr::c8AsBinary(pszBuffer(), bValid, eRadix);
    return bValid;
}

tCIDLib::TBoolean TString::bToFloat4(COP tCIDLib::TFloat4& f4ToFill) const noexcept
{
    tCIDLib::TBoolean bValid = kCIDLib::False;
    const tCIDLib::TFloat8 f8Val = TRawStr::f8AsBinary(pszBuffer(), bValid);
    f4ToFill = 0;
    if (bValid)
    {
        if ((f8Val < kCIDLib::f4MinFloat) && (f8Val > kCIDLib::f4MaxFloat))
            bValid = kCIDLib::False;
        else
            f4ToFill = tCIDLib::TFloat4(f8Val);
    }
    return bValid;
}

tCIDLib::TBoolean TString::bToFloat8(COP tCIDLib::TFloat8& f8ToFill) const noexcept
{
    tCIDLib::TBoolean bValid = kCIDLib::False;
    f8ToFill = TRawStr::f8AsBinary(pszBuffer(), bValid);
    return bValid;
}

tCIDLib::TBoolean
TString::bToInt1(COP tCIDLib::TInt1& i1ToFill, const tCIDLib::ERadices eRadix) const noexcept
{
    tCIDLib::TBoolean bValid = kCIDLib::False;
    const tCIDLib::TInt4 i4Val = TRawStr::i4AsBinary(pszBuffer(), bValid, eRadix);
    i1ToFill = 0;
    if (bValid)
    {
        if ((i4Val < kCIDLib::i1MinInt) || (i4Val > kCIDLib::i1MaxInt))
            bValid = kCIDLib::False;
        else
            i1ToFill = tCIDLib::TInt1(i4Val);
    }
    return bValid;
}

tCIDLib::TBoolean
TString::bToInt2(COP tCIDLib::TInt2& i2ToFill, const tCIDLib::ERadices eRadix) const noexcept
{
    tCIDLib::TBoolean bValid = kCIDLib::False;
    const tCIDLib::TInt4 i4Val = TRawStr::i4AsBinary(pszBuffer(), bValid, eRadix);
    i2ToFill = 0;
    if (bValid)
    {
        if ((i4Val < kCIDLib::i2MinInt) || (i4Val > kCIDLib::i2MaxInt))
            bValid = kCIDLib::False;
        else
            i2ToFill = tCIDLib::TInt2(i4Val);
    }
    return bValid;
}

tCIDLib::TBoolean
TString::bToInt4(COP tCIDLib::TInt4& i4ToFill, const tCIDLib::ERadices eRadix) const noexcept
{
    tCIDLib::TBoolean bValid = kCIDLib::False;
    i4ToFill = TRawStr::i4AsBinary(pszBuffer(), bValid, eRadix);
    return bValid;
}

tCIDLib::TBoolean
TString::bToInt8(COP tCIDLib::TInt8& i8ToFill, const tCIDLib::ERadices eRadix) const noexcept
{
    tCIDLib::TBoolean bValid;
    i8ToFill = TRawStr::i8AsBinary(pszBuffer(), bValid, eRadix);
    return bValid;
}



// Checks to see if the indicated token is in this string
tCIDLib::TBoolean TString::bTokenExists(const tCIDLib::TCh chToken) const
{
    //  Some dummy params to get output values we are goign to discard
    tCIDLib::TCh        chDummy = kCIDLib::chNull;
    tCIDLib::EHJustify  eDummy = tCIDLib::EHJustify::Count;
    tCIDLib::TCard4     c4Dummy = 0;

    // And look for the token.
    if (pszFindToken(pszBuffer(), chToken, eDummy, c4Dummy, chDummy, c4Dummy, c4Dummy, c4Dummy))
        return kCIDLib::True;
    return kCIDLib::False;
}


// Tries to convert this string to a Card4 value. Assumes the indicated radix
tCIDLib::TCard4 TString::c4Val(const tCIDLib::ERadices eRadix) const
{
    tCIDLib::TBoolean bValid = kCIDLib::False;
    const tCIDLib::TCard4 c4Ret = TRawStr::c4AsBinary(pszBuffer(), bValid, eRadix);
    if (!bValid)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_TextConvert
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , *this
            , TString(L"tCIDLib::TCard4")
        );
    }
    return c4Ret;
}


// Tries to convert this string to a Card8 value. Assumes the indicated radix
tCIDLib::TCard8 TString::c8Val(const tCIDLib::ERadices eRadix) const
{
    tCIDLib::TBoolean bValid = kCIDLib::False;
    const tCIDLib::TCard8 c8Ret = TRawStr::c8AsBinary(pszBuffer(), bValid, eRadix);
    if (!bValid)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_TextConvert
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , *this
            , TString(L"tCIDLib::TCard8")
        );
    }
    return c8Ret;
}


tCIDLib::TVoid
TString::CopyInSubStr(  const   TStringView&    strvSource
                        , const tCIDLib::TCard4 c4Start
                        , const tCIDLib::TCard4 c4Len)
{
    Clear();
    AppendSubStr(strvSource, c4Start, c4Len);
}

tCIDLib::TVoid
TString::CopyOutSubStr( COP     TString&        strTarget
                        , const tCIDLib::TCard4 c4Start
                        , const tCIDLib::TCard4 c4Len) const
{
    strTarget.Clear();
    strTarget.AppendSubStr(*this, c4Start, c4Len);
}


tCIDLib::TVoid
TString::Cut(const tCIDLib::TCard4 c4Start, const tCIDLib::TCard4 c4Len)
{
    if (!c4Len)
        return;

    // We cannot start beyond the end
    const tCIDLib::TCard4 c4OurLen = m_strbData.c4CurEnd();
    if (c4Start >= c4OurLen)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Start)
            , clsThis()
            , TCardinal(m_strbData.c4CurEnd())
        );
    }

    //
    //  They can pass max card in which case we ur our own length. That effectively
    //  becomes a cap at, but we don't want to reject such a thing.
    //
    tCIDLib::TCard4 c4ActualLen = c4Len;
    if (c4ActualLen == kCIDLib::c4MaxCard)
        c4ActualLen = c4OurLen;

    // If the start plus actual length would eat the whole thing, then cap and done
    if ((c4Start + c4ActualLen) >= c4OurLen)
    {
        // If this would eat the whole string, then just cap at the start
        m_strbData.SetEnd(c4Start);
        return;
    }


    // Copy the text at c4Start+c4ActualLen down to c4Start
    tCIDLib::TCh* pszDest = m_strbData.pszBufferAt(c4Start);
    tCIDLib::TCh* pszSrc  = m_strbData.pszBufferAt(c4Start + c4ActualLen);
    tCIDLib::TCh* pszEnd  = m_strbData.pszAtEnd();
    while (pszSrc < pszEnd)
        *pszDest++ = *pszSrc++;

    m_strbData.DecEnd(c4ActualLen);
}


//
//  Look for the first instance of the indicated character and, if found, cut
//  from the start o the string to there.
//
tCIDLib::TVoid TString::CutUpTo(const tCIDLib::TCh chFind)
{
    // If we got a match, then copy down the remainder
    tCIDLib::TCh* pszMatch = TRawStr::pszFindChar(m_strbData.pszBuffer(), chFind, 0, kCIDLib::True);
    if (pszMatch)
    {
        //
        //  We end the cut on the found character, so we subtract the two
        //  pointers and add one. Then just call the cut method.
        //
        Cut(0, m_strbData.c4CalcBufDiff(pszMatch) + 1);
    }
}


// Compares this string to a passed string
tCIDLib::ESortComps TString::eCompare(const TStringView& strvSrc) const
{
    return TRawStr::eCompareStr(pszBuffer(), strvSrc.pszBuffer());
}


// Compares N chars of this string to a passed string
tCIDLib::ESortComps
TString::eCompareN(const TStringView& strvSrc, const tCIDLib::TCard4 c4MaxComp) const
{
    if (!c4MaxComp)
        return tCIDLib::ESortComps::Equal;
    return TRawStr::eCompareStrN(pszBuffer(), strvSrc.pszBuffer(), c4MaxComp);
}


// Compares this string to a passed string, insensitive to case
tCIDLib::ESortComps TString::eCompareI(const TStringView& strvSrc) const
{
    return TRawStr::eCompareStrI(pszBuffer(), strvSrc.pszBuffer());
}


// Compares N chars of this string to a passed string, insensitive to case
tCIDLib::ESortComps
TString::eCompareNI(const TStringView& strvSrc, const tCIDLib::TCard4 c4MaxComp) const
{
    if (!c4MaxComp)
        return tCIDLib::ESortComps::Equal;
    return TRawStr::eCompareStrNI(pszBuffer(), strvSrc.pszBuffer(), c4MaxComp);
}


// Tries to convert this string into a Float8 value, throwing if bad
tCIDLib::TFloat8 TString::f8Val() const
{
    tCIDLib::TBoolean bValid = kCIDLib::False;
    tCIDLib::TFloat8 f8Ret = TRawStr::f8AsBinary(pszBuffer(), bValid);
    if (!bValid)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_TextConvert
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , *this
            , TString(L"tCIDLib::TFloat8")
        );
    }
    return f8Ret;
}



//
//  These all replace tokens in the string with passed values. We have
//  specialized versions for all of the fundamental data types, plus a
//  generic one that takes any formattable object. We can either throw
//  on failure to replace or just return a status. The status indicates
//  whether we found it or not and if not found whether there are any
//  more tokens in the string to be found.
//
tCIDLib::EFindRes
TString::eReplaceToken(  const  tCIDLib::TCh    chVal
                        , const tCIDLib::TCh    chToken)
{
    tCIDLib::TCh szTmp[2] = { chVal, kCIDLib::chNull };
    return eReplaceToken(szTmp, chToken);
}


tCIDLib::EFindRes
TString::eReplaceToken(  const  tCIDLib::TCh* const pszVal
                        , const tCIDLib::TCh        chToken)
{
    tCIDLib::TCh        chFill = kCIDLib::chNull;
    tCIDLib::TCard4     c4Chars = 0;
    tCIDLib::TCard4     c4FldWidth = 0;
    tCIDLib::TCard4     c4Index = 0;
    tCIDLib::TCard4     c4Precision = 0;
    tCIDLib::TCard4     c4TokenCnt = 0;
    tCIDLib::EHJustify  eJustify = tCIDLib::EHJustify::Count;

    // Find the first valid replacement token
    const tCIDLib::TCh* pszTmp = pszFindToken
    (
        pszBuffer(), chToken, eJustify, c4FldWidth, chFill, c4Precision, c4Chars, c4TokenCnt
    );

    // If no token, we are done
    if (!pszTmp)
    {
        if (facCIDLib().bLogWarnings())
        {
            facCIDLib().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_TokenNotFound
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::NotFound
                , TString(chToken)
            );
        }
        return c4TokenCnt ? tCIDLib::EFindRes::NotFound : tCIDLib::EFindRes::NoMore;
    }

    // Calc the index where we found it
    c4Index = m_strbData.c4CalcBufDiff(pszTmp);

    //
    //  If the field width is zero, then it's just going to be the source
    //  string in this case, so we can save some overhead.
    //
    if (!c4FldWidth)
    {
        Replace(c4Index, c4Index + (c4Chars - 1), pszVal, c4FldWidth);
    }
     else
    {
        // Format the source into the field
        tCIDLib::TCh* pszFmt = new tCIDLib::TCh[c4FldWidth + 1];
        TArrayJanitor<tCIDLib::TCh> janFmt(pszFmt);
        if (!TRawStr::bFormatStr(pszVal, pszFmt, c4FldWidth, chFill, eJustify))
        {
            if (facCIDLib().bLogFailures())
            {
                facCIDLib().LogKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcStr_FormatToken
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Internal
                );
            }
            TRawStr::CopyStr(pszFmt, L"???", c4FldWidth);
        }

        Replace(c4Index, c4Index + (c4Chars - 1), pszFmt, c4FldWidth);
    }
    return tCIDLib::EFindRes::Found;
}


tCIDLib::EFindRes
TString::eReplaceToken( const   MFormattable&   fmtblVal
                        , const tCIDLib::TCh    chToken)
{
    tCIDLib::TCh        chFill = kCIDLib::chNull;
    tCIDLib::TCard4     c4Chars = 0;
    tCIDLib::TCard4     c4FldWidth = 0;
    tCIDLib::TCard4     c4Index = 0;
    tCIDLib::TCard4     c4Precision = 0;
    tCIDLib::TCard4     c4TokenCnt = 0;
    tCIDLib::EHJustify  eJustify = tCIDLib::EHJustify::Count;

    const tCIDLib::TCh* pszTmp = pszFindToken
    (
        pszBuffer(), chToken, eJustify, c4FldWidth, chFill, c4Precision, c4Chars, c4TokenCnt
    );

    // If no token, log a message and return
    if (!pszTmp)
    {
        if (facCIDLib().bLogWarnings())
        {
            facCIDLib().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_TokenNotFound
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::NotFound
                , TString(chToken)
            );
        }
        return c4TokenCnt ? tCIDLib::EFindRes::NotFound : tCIDLib::EFindRes::NoMore;
    }

    // Calc the index where we found it
    c4Index = m_strbData.c4CalcBufDiff(pszTmp);

    //
    //  Format the replacement value to a string, being sure to flush it
    //  so that the cache is forced out to the string buffer.
    //
    TTextStringOutStream strmTmp(256, TStreamFmt(c4FldWidth, 2, eJustify, chFill));
    strmTmp << fmtblVal << kCIDLib::FlushIt;

    Replace(c4Index, c4Index + (c4Chars - 1), strmTmp.strData().pszBuffer(), c4FldWidth);
    return tCIDLib::EFindRes::Found;
}


tCIDLib::EFindRes
TString::eReplaceToken( const   tCIDLib::TBoolean   bVal
                        , const tCIDLib::TCh        chToken)
{
    tCIDLib::TCh        chFill = kCIDLib::chNull;
    tCIDLib::TCard4     c4Chars = 0;
    tCIDLib::TCard4     c4FldWidth = 0;
    tCIDLib::TCard4     c4Index = 0;
    tCIDLib::TCard4     c4Precision = 0;
    tCIDLib::TCard4     c4TokenCnt = 0;
    tCIDLib::EHJustify  eJustify = tCIDLib::EHJustify::Count;

    //  Find the first valid replacement token.
    const tCIDLib::TCh* pszTmp = pszFindToken
    (
        pszBuffer(), chToken, eJustify, c4FldWidth, chFill, c4Precision, c4Chars, c4TokenCnt
    );

    // If no token, log a message and return
    if (!pszTmp)
    {
        if (facCIDLib().bLogWarnings())
        {
            facCIDLib().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_TokenNotFound
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::NotFound
                , TString(chToken)
            );
        }
        return c4TokenCnt ? tCIDLib::EFindRes::NotFound : tCIDLib::EFindRes::NoMore;
    }

    // Calc the index where we found it
    c4Index = m_strbData.c4CalcBufDiff(pszTmp);

    // Get the value we are going to format out
    const tCIDLib::TCh* pszVal
    (
         bVal ? facCIDLib().pszLoadCIDMsg(kCIDMsgs::midGen_True)
              : facCIDLib().pszLoadCIDMsg(kCIDMsgs::midGen_False)
    );

    //
    //  If they provided no field width, then we just use the source value
    //  because none of the formatting stuff could make any difference for
    //  us here.
    //
    if (!c4FldWidth)
    {
        Replace(c4Index, c4Index + (c4Chars - 1), pszVal, c4FldWidth);
    }
     else
    {
        // Format it into the indicated field width
        tCIDLib::TCh* pszFmt = new tCIDLib::TCh[c4FldWidth + 1];
        TArrayJanitor<tCIDLib::TCh> janFmt(pszFmt);
        if (!TRawStr::bFormatStr(pszVal, pszFmt, c4FldWidth, chFill, eJustify))
        {
            if (facCIDLib().bLogFailures())
            {
                facCIDLib().LogKrnlErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcStr_FormatToken
                    , TKrnlError::kerrLast()
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Internal
                );
            }
            TRawStr::CopyStr(pszFmt, L"???", c4FldWidth);
        }
        Replace(c4Index, c4Index + (c4Chars - 1), pszFmt, c4FldWidth);
    }
    return tCIDLib::EFindRes::Found;
}

tCIDLib::EFindRes
TString::eReplaceToken( const   tCIDLib::TCard1     c1Val
                        , const tCIDLib::TCh        chToken
                        , const tCIDLib::ERadices   eRadix)
{
    // Just call the TCard4 version
    return eReplaceToken(tCIDLib::TCard4(c1Val), chToken, eRadix);
}

tCIDLib::EFindRes
TString::eReplaceToken( const   tCIDLib::TCard2     c2Val
                        , const tCIDLib::TCh        chToken
                        , const tCIDLib::ERadices   eRadix)
{
    // Just call the TCard4 version
    return eReplaceToken(tCIDLib::TCard4(c2Val), chToken, eRadix);
}

tCIDLib::EFindRes
TString::eReplaceToken( const   tCIDLib::TCard4     c4Val
                        , const tCIDLib::TCh        chToken
                        , const tCIDLib::ERadices   eRadix)
{
    tCIDLib::TCh        chFill = kCIDLib::chNull;
    tCIDLib::TCard4     c4Chars = 0;
    tCIDLib::TCard4     c4FldWidth = 0;
    tCIDLib::TCard4     c4Index = 0;
    tCIDLib::TCard4     c4Precision = 0;
    tCIDLib::TCard4     c4TokenCnt = 0;
    tCIDLib::EHJustify  eJustify = tCIDLib::EHJustify::Count;

    //  Find the first valid replacement token.
    const tCIDLib::TCh* pszTmp = pszFindToken
    (
        pszBuffer(), chToken, eJustify, c4FldWidth, chFill, c4Precision, c4Chars, c4TokenCnt
    );

    // If no token, log a message and return
    if (!pszTmp)
    {
        if (facCIDLib().bLogWarnings())
        {
            facCIDLib().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_TokenNotFound
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::NotFound
                , TString(chToken)
            );
        }
        return c4TokenCnt ? tCIDLib::EFindRes::NotFound : tCIDLib::EFindRes::NoMore;
    }

    // Calc the index where we found it
    c4Index = m_strbData.c4CalcBufDiff(pszTmp);

    tCIDLib::TZStr64 szTmp;
    if (!TRawStr::bFormatVal(c4Val, szTmp, 64, eRadix))
    {
        if (facCIDLib().bLogFailures())
        {
            facCIDLib().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_FormatToken
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
        }
        TRawStr::CopyStr(szTmp, L"???", 64);
    }

    if (!c4FldWidth)
        c4FldWidth = TRawStr::c4StrLen(szTmp);

    tCIDLib::TCh* pszFmt = new tCIDLib::TCh[c4FldWidth + 1];
    TArrayJanitor<tCIDLib::TCh> janFmt(pszFmt);
    if (!TRawStr::bFormatStr(szTmp, pszFmt, c4FldWidth, chFill, eJustify))
    {
        if (facCIDLib().bLogFailures())
        {
            facCIDLib().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_FormatToken
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
        }
        TRawStr::CopyStr(pszFmt, L"???", c4FldWidth);
    }

    Replace(c4Index, c4Index + (c4Chars - 1), pszFmt, c4FldWidth);
    return tCIDLib::EFindRes::Found;
}

tCIDLib::EFindRes
TString::eReplaceToken( const   tCIDLib::TCard8     c8Val
                        , const tCIDLib::TCh        chToken
                        , const tCIDLib::ERadices   eRadix)
{
    tCIDLib::TCh        chFill = kCIDLib::chNull;
    tCIDLib::TCard4     c4Chars = 0;
    tCIDLib::TCard4     c4FldWidth = 0;
    tCIDLib::TCard4     c4Index = 0;
    tCIDLib::TCard4     c4Precision = 0;
    tCIDLib::TCard4     c4TokenCnt = 0;
    tCIDLib::EHJustify  eJustify = tCIDLib::EHJustify::Count;

    //  Find the first valid replacement token.
    const tCIDLib::TCh* pszTmp = pszFindToken
    (
        pszBuffer(), chToken, eJustify, c4FldWidth, chFill, c4Precision, c4Chars, c4TokenCnt
    );

    // If no token, log a message and return
    if (!pszTmp)
    {
        if (facCIDLib().bLogWarnings())
        {
            facCIDLib().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_TokenNotFound
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::NotFound
                , TString(chToken)
            );
        }
        return c4TokenCnt ? tCIDLib::EFindRes::NotFound : tCIDLib::EFindRes::NoMore;
    }

    // Calc the index where we found it
    c4Index = m_strbData.c4CalcBufDiff(pszTmp);

    tCIDLib::TZStr128 szTmp;
    if (!TRawStr::bFormatVal(c8Val, szTmp, 128, eRadix))
    {
        if (facCIDLib().bLogFailures())
        {
            facCIDLib().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_FormatToken
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
        }
        TRawStr::CopyStr(szTmp, L"???", 128);
    }

    if (!c4FldWidth)
        c4FldWidth = TRawStr::c4StrLen(szTmp);

    tCIDLib::TCh* pszFmt = new tCIDLib::TCh[c4FldWidth + 1];
    TArrayJanitor<tCIDLib::TCh> janFmt(pszFmt);
    if (!TRawStr::bFormatStr(szTmp, pszFmt, c4FldWidth, chFill, eJustify))
    {
        if (facCIDLib().bLogFailures())
        {
            facCIDLib().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_FormatToken
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
            TRawStr::CopyStr(pszFmt, L"???", c4FldWidth);
        }
    }

    // And ask the buffer object to replace the token with the text
    Replace(c4Index, c4Index + (c4Chars - 1), pszFmt, c4FldWidth);
    return tCIDLib::EFindRes::Found;
}


tCIDLib::EFindRes
TString::eReplaceToken( const   tCIDLib::TFloat8&   f8Val
                        , const tCIDLib::TCh        chToken
                        , const tCIDLib::TCard1     c1Precision)
{
    tCIDLib::TCh        chFill = kCIDLib::chNull;
    tCIDLib::TCard4     c4Chars = 0;
    tCIDLib::TCard4     c4FldWidth = 0;
    tCIDLib::TCard4     c4Index = 0;
    tCIDLib::TCard4     c4Precision = 0;
    tCIDLib::TCard4     c4TokenCnt = 0;
    tCIDLib::EHJustify  eJustify = tCIDLib::EHJustify::Count;

    // Find the first valid replacement token
    const tCIDLib::TCh* pszTmp = pszFindToken
    (
        pszBuffer(), chToken, eJustify, c4FldWidth, chFill, c4Precision, c4Chars, c4TokenCnt
    );

    // If no token, log a message and return
    if (!pszTmp)
    {
        if (facCIDLib().bLogWarnings())
        {
            facCIDLib().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_TokenNotFound
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::NotFound
                , TString(chToken)
            );
        }
        return c4TokenCnt ? tCIDLib::EFindRes::NotFound : tCIDLib::EFindRes::NoMore;
    }

    // Calc the index where we found it
    c4Index = m_strbData.c4CalcBufDiff(pszTmp);

    tCIDLib::TZStr128 szTmp;
    if (!TRawStr::bFormatVal(f8Val, szTmp, c1Precision, 128, tCIDLib::ETrailFmts::Zeroes))
    {
        if (facCIDLib().bLogFailures())
        {
            facCIDLib().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_FormatToken
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
        }
        TRawStr::CopyStr(szTmp, L"???", 128);
    }

    if (!c4FldWidth)
        c4FldWidth = TRawStr::c4StrLen(szTmp);

    tCIDLib::TCh* pszFmt = new tCIDLib::TCh[c4FldWidth + 1];
    TArrayJanitor<tCIDLib::TCh> janFmt(pszFmt);
    if (!TRawStr::bFormatStr(szTmp, pszFmt, c4FldWidth, chFill, eJustify))
    {
        if (facCIDLib().bLogFailures())
        {
            facCIDLib().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_FormatToken
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
        }
        TRawStr::CopyStr(pszFmt, L"???", c4FldWidth);
    }

    Replace(c4Index, c4Index + (c4Chars - 1), pszFmt, c4FldWidth);
    return tCIDLib::EFindRes::Found;
}

tCIDLib::EFindRes
TString::eReplaceToken( const   tCIDLib::TInt1      i1Val
                        , const tCIDLib::TCh        chToken
                        , const tCIDLib::ERadices   eRadix)
{
    // Just call the TInt4 version
    return eReplaceToken(tCIDLib::TInt4(i1Val), chToken, eRadix);
}

tCIDLib::EFindRes
TString::eReplaceToken( const   tCIDLib::TInt2      i2Val
                        , const tCIDLib::TCh        chToken
                        , const tCIDLib::ERadices   eRadix)
{
    // Just call the TInt4 version
    return eReplaceToken(tCIDLib::TInt4(i2Val), chToken, eRadix);
}

tCIDLib::EFindRes
TString::eReplaceToken( const   tCIDLib::TInt4      i4Val
                        , const tCIDLib::TCh        chToken
                        , const tCIDLib::ERadices   eRadix)
{
    tCIDLib::TCh        chFill = kCIDLib::chNull;
    tCIDLib::TCard4     c4Chars = 0;
    tCIDLib::TCard4     c4FldWidth = 0;
    tCIDLib::TCard4     c4Index = 0;
    tCIDLib::TCard4     c4Precision = 0;
    tCIDLib::TCard4     c4TokenCnt = 0;
    tCIDLib::EHJustify  eJustify = tCIDLib::EHJustify::Count;

    //  Find the first valid replacement token.
    const tCIDLib::TCh* pszTmp = pszFindToken
    (
        pszBuffer(), chToken, eJustify, c4FldWidth, chFill, c4Precision, c4Chars, c4TokenCnt
    );

    // If no token, log a message and return
    if (!pszTmp)
    {
        if (facCIDLib().bLogWarnings())
        {
            facCIDLib().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_TokenNotFound
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::NotFound
                , TString(chToken)
            );
        }
        return c4TokenCnt ? tCIDLib::EFindRes::NotFound : tCIDLib::EFindRes::NoMore;
    }

    // Calc the index where we found it
    c4Index = m_strbData.c4CalcBufDiff(pszTmp);

    tCIDLib::TZStr64 szTmp;
    if (!TRawStr::bFormatVal(i4Val, szTmp, 64, eRadix))
    {
        if (facCIDLib().bLogFailures())
        {
            facCIDLib().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_FormatToken
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
        }
        TRawStr::CopyStr(szTmp, L"???", 64);
    }

    if (!c4FldWidth)
        c4FldWidth = TRawStr::c4StrLen(szTmp);

    // Could get an exception here and leak the buffer
    tCIDLib::TCh* pszFmt = new tCIDLib::TCh[c4FldWidth + 1];
    TArrayJanitor<tCIDLib::TCh> janFmt(pszFmt);
    if (!TRawStr::bFormatStr(szTmp, pszFmt, c4FldWidth, chFill, eJustify))
    {
        if (facCIDLib().bLogFailures())
        {
            facCIDLib().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_FormatToken
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
        }
        TRawStr::CopyStr(pszFmt, L"???", c4FldWidth);
    }

    Replace(c4Index, c4Index + (c4Chars - 1), pszFmt, c4FldWidth);
    return tCIDLib::EFindRes::Found;
}

tCIDLib::EFindRes
TString::eReplaceToken( const   tCIDLib::TInt8      i8Val
                        , const tCIDLib::TCh        chToken
                        , const tCIDLib::ERadices   eRadix)
{
    tCIDLib::TCh        chFill = kCIDLib::chNull;
    tCIDLib::TCard4     c4Chars = 0;
    tCIDLib::TCard4     c4FldWidth = 0;
    tCIDLib::TCard4     c4Index = 0;
    tCIDLib::TCard4     c4Precision = 0;
    tCIDLib::TCard4     c4TokenCnt = 0;
    tCIDLib::EHJustify  eJustify = tCIDLib::EHJustify::Count;

    //  Find the first valid replacement token.
    const tCIDLib::TCh* pszTmp = pszFindToken
    (
        pszBuffer(), chToken, eJustify, c4FldWidth, chFill, c4Precision, c4Chars, c4TokenCnt
    );

    // If no token, log a message and return
    if (!pszTmp)
    {
        if (facCIDLib().bLogWarnings())
        {
            facCIDLib().LogMsg
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_TokenNotFound
                , tCIDLib::ESeverities::Warn
                , tCIDLib::EErrClasses::NotFound
                , TString(chToken)
            );
        }
        return c4TokenCnt ? tCIDLib::EFindRes::NotFound : tCIDLib::EFindRes::NoMore;
    }

    // Calc the index where we found it
    c4Index = m_strbData.c4CalcBufDiff(pszTmp);

    tCIDLib::TZStr128  szTmp = L"";
    if (!TRawStr::bFormatVal(i8Val, szTmp, 128, eRadix))
    {
        if (facCIDLib().bLogFailures())
        {
            facCIDLib().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_FormatToken
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
        }
        TRawStr::CopyStr(szTmp, L"???", 64);
    }

    if (!c4FldWidth)
        c4FldWidth = TRawStr::c4StrLen(szTmp);

    // Could get an exception here and leak the buffer
    tCIDLib::TCh* pszFmt = new tCIDLib::TCh[c4FldWidth + 1];
    TArrayJanitor<tCIDLib::TCh> janFmt(pszFmt);
    if (!TRawStr::bFormatStr(szTmp, pszFmt, c4FldWidth, chFill, eJustify))
    {
        if (facCIDLib().bLogFailures())
        {
            facCIDLib().LogKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_FormatToken
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
        }
        TRawStr::CopyStr(pszFmt, L"???", c4FldWidth);
    }

    Replace(c4Index, c4Index + (c4Chars - 1), pszFmt, c4FldWidth);
    return tCIDLib::EFindRes::Found;
}


//
//  Checks to see if the indicated token is in this string. Unlike the
//  bTokenExists above, this one returns an enumerated value that indicates
//  whether it's found, or not found, or not found and no other tokens were
//  seen during the scan.
//
tCIDLib::EFindRes TString::eTokenExists(const tCIDLib::TCh chToken) const
{
    //
    //  We don't care about these, but the searching function returns
    //  them, so...
    //
    tCIDLib::TCh        chFill = kCIDLib::chNull;
    tCIDLib::TCard4     c4Chars = 0;
    tCIDLib::TCard4     c4Precision = 0;
    tCIDLib::TCard4     c4TokenCnt = 0;
    tCIDLib::TCard4     c4Width = 0;
    tCIDLib::EHJustify  eJustify = tCIDLib::EHJustify::Count;

    if (pszFindToken(pszBuffer(), chToken, eJustify, c4Width, chFill, c4Precision, c4Chars, c4TokenCnt))
        return tCIDLib::EFindRes::Found;

    // Not found, but there are still tokens present
    if (c4TokenCnt)
        return tCIDLib::EFindRes::NotFound;

    // Not found and no more tokens are present
    return tCIDLib::EFindRes::NoMore;
}


tCIDLib::TVoid TString::ExportChars(        tCIDLib::TCh* const pszTarget
                                    , const tCIDLib::TCard4     c4MaxChars
                                    , const tCIDLib::TCard4     c4StartInd) const
{
    // Tell it not to add any null terminator
    ToZStr(pszTarget, c4MaxChars, c4StartInd, kCIDLib::False);
}


//
//  Formats the indicate string into a field of the indicated format and
//  stores the result as our contents.
//
tCIDLib::TVoid
TString::FormatToFld(const  TStringView&            strvSrc
                    , const tCIDLib::TCard4         c4FldWidth
                    , const tCIDLib::EHJustify      eJustify
                    , const tCIDLib::TCh            chFill
                    , const tCIDLib::TCard4         c4TrailingSp)
{
    //
    //  If the field width is zero, then we will just make it fit the
    //  source string, so its an easy one.
    //
    if (!c4FldWidth)
    {
        Set(strvSrc);
        return;
    }

    //  See if we need to expand. Don't need to preserve existing content
    m_strbData.ExpandTo(c4FldWidth + c4TrailingSp, kCIDLib::False);

    if (!TRawStr::bFormatStr(strvSrc.pszBuffer(), m_strbData.pszBuffer(), c4FldWidth, chFill, eJustify))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_FormatFld
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }

    // Update the current length to the width of the field, plus any trailing spaces
    m_strbData.SetEnd(c4FldWidth + c4TrailingSp);

    // Append any trailing spaces. We expanded already so we have room
    if (c4TrailingSp)
        Append(kCIDLib::chSpace, c4TrailingSp);
}


//
//  Copies in characters from a raw source string. It must be a null termed
//  string. It can be a null though, and a special string representing a
//  null will be used as the value. If the count is 0, then the actual length
//  of the string is used as the count.
//
tCIDLib::TVoid TString::FromShortZStr(const tCIDLib::TSCh* const pszNewValue)
{
    SetFromShort(pszNewValue);
}


tCIDLib::TVoid TString::FromZStr(const  tCIDLib::TCh* const pszSrc
                                , const tCIDLib::TCard4     c4Count)
{
    // If the source is null, nothing to do
    if (!pszSrc)
        return;

    // Get the length of the actual source string
    const tCIDLib::TCard4 c4SrcLen = c4Count ? c4Count : TRawStr::c4StrLen(pszSrc);
    m_strbData.Clear();
    m_strbData.Append(pszSrc, c4SrcLen);
}


// Does a standard hash calcualtion on the contents of this string
tCIDLib::THashVal
TString::hshCalcHash(const tCIDLib::TCard4 c4Modulus) const
{
    tCIDLib::THashVal hshRet = TRawStr::hshHashStr(pszBuffer(), c4Modulus);
    if (!hshRet)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_NotValidUnicode
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }
    return hshRet;
}


// Tries to convert this string into a Int4 value, throwing if bad
tCIDLib::TInt4 TString::i4Val(const tCIDLib::ERadices eRadix) const
{
    tCIDLib::TBoolean bValid;
    const tCIDLib::TInt4 i4Ret = TRawStr::i4AsBinary(pszBuffer(), bValid, eRadix);
    if (!bValid)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_TextConvert
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , *this
            , TString(L"tCIDLib::TInt4")
        );
    }
    return i4Ret;
}


// Tries to convert this string into an Int8 value, throwing if bad
tCIDLib::TInt8 TString::i8Val(const tCIDLib::ERadices eRadix) const
{
    tCIDLib::TBoolean bValid;
    const tCIDLib::TInt8 i8Ret = TRawStr::i8AsBinary(pszBuffer(), bValid, eRadix);
    if (!bValid)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_TextConvert
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , *this
            , TString(L"tCIDLib::TInt8")
        );
    }
    return i8Ret;
}


//
//  Unlike FromZStr above, this one doesn't assume any sort of null term
//  on the source. So it takes the source count as a literal value and
//  won't take 0 as a 'use all the source chars' indicator like FromZStr.
//
tCIDLib::TVoid
TString::ImportChars(const  tCIDLib::TCh* const pszSrc
                    , const tCIDLib::TCard4     c4SrcCount)
{
    // Expand if needed, no need to preserve
    m_strbData.ExpandTo(c4SrcCount, kCIDLib::False);
    m_strbData.Append(pszSrc, c4SrcCount);
}


// Inserts the indicated character into us at the indicated index
tCIDLib::TVoid TString::Insert(const tCIDLib::TCh chInsert, const tCIDLib::TCard4 c4Ind)
{
    tCIDLib::TCh szTmp[2] = { chInsert, kCIDLib::chNull };
    Insert(szTmp, c4Ind);
}


tCIDLib::TVoid TString::Insert( const   tCIDLib::TCh* const pszInsert
                                , const tCIDLib::TCard4     c4Ind
                                , const tCIDLib::TCard4     c4InsertLen)
{
    // If source to insert is null or empty, do nothing
    if (!pszInsert || (pszInsert[0] == kCIDLib::chNull))
        return;

    //
    //  Make sure that the insert index is valid. Note that, in this case, we
    //  allow it to fall on the terminating null, so that an insert at the end
    //  will be like an append. Otherwise, you could not insert after the last
    //  char, because the index is the 'insert after' index.
    //
    const tCIDLib::TCard4 c4OurLen = m_strbData.c4CurEnd();
    if (c4Ind > c4OurLen)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Ind)
            , clsThis()
            , TCardinal(c4OurLen)
        );
    }

    const tCIDLib::TCard4 c4SrcLen = c4InsertLen ? c4InsertLen : TRawStr::c4StrLen(pszInsert);

    // If the insert point is at the end, then we just append
    if (c4Ind == c4OurLen)
    {
        m_strbData.Append(pszInsert, c4SrcLen);
        return;
    }

    // Extend the string if we are going to need to. Retain current content
    m_strbData.ExpandBy(c4SrcLen, kCIDLib::True);

    //
    //  Lets move the text after the insert point upwards to make room for
    //  the insert text.
    //
    tCIDLib::TCh* pszSrc = m_strbData.pszBufferAt(c4OurLen - 1, kCIDLib::False);
    tCIDLib::TCh* pszStop = m_strbData.pszBufferAt(c4Ind, kCIDLib::False);
    tCIDLib::TCh* pszDest = m_strbData.pszBufferAt((c4OurLen - 1) + c4SrcLen, kCIDLib::False);
    while (pszSrc >= pszStop)
        *pszDest-- = *pszSrc--;

    // And now move the insert text into the section left open
    TRawMem::CopyMemBuf(pszStop, pszInsert, c4SrcLen * kCIDLib::c4CharBytes);

    // And move the end up by the length we inserted
    m_strbData.IncEnd(c4SrcLen);
}


//
//  Sets our contents to a loadable text string, optionally replacing
//  tokens in the loaded text with passed formattable objects.
//
tCIDLib::TVoid
TString::LoadFromMsg(   const   tCIDLib::TErrCode   errcToLoad
                        , const TFacility&          facSrc)
{
    Set(facSrc.pszLoadCIDMsg(errcToLoad));
}

tCIDLib::TVoid
TString::LoadFromMsg(   const   tCIDLib::TErrCode   errcToLoad
                        , const TFacility&          facSrc
                        , const MFormattable&       fmtblToken1
                        , const MFormattable&       fmtblToken2
                        , const MFormattable&       fmtblToken3
                        , const MFormattable&       fmtblToken4)
{
    //
    //  Call the other version first. This will get the string allocated
    //  and we just have to handle the token replacement.
    //
    LoadFromMsg(errcToLoad, facSrc);

    // Handle the token replacement
    if (!MFormattable::bIsNullObject(fmtblToken1))
        eReplaceToken(fmtblToken1, L'1');
    if (!MFormattable::bIsNullObject(fmtblToken2))
        eReplaceToken(fmtblToken2, L'2');
    if (!MFormattable::bIsNullObject(fmtblToken3))
        eReplaceToken(fmtblToken3, L'3');
    if (!MFormattable::bIsNullObject(fmtblToken4))
        eReplaceToken(fmtblToken4, L'4');
}


// Prepends the indicated character to us
tCIDLib::TVoid TString::Prepend(const tCIDLib::TCh chPrepend)
{
    // Put it into a temporary ascii z-string and insert at 0
    tCIDLib::TCh pszTmp[2] = { chPrepend, kCIDLib::chNull };
    Insert(pszTmp, 0);
}

tCIDLib::TVoid TString::Prepend(const TString& strPrepend)
{
    Insert(strPrepend, 0);
}

tCIDLib::TVoid TString::Prepend(const tCIDLib::TCh* const pszPrepend)
{
    Insert(pszPrepend, 0);
}


tCIDLib::TCh* TString::pszDupBuffer() const
{
    //
    //  We know here we have a buffer, so we can access it directly, be sure to copy
    //  over the null as well!
    //
    const tCIDLib::TCard4 c4OurLen = m_strbData.c4CurEnd();
    tCIDLib::TCh* pszRet = new tCIDLib::TCh[c4OurLen + 1];
    TRawMem::CopyMemBuf(pszRet, m_strbData.pszBuffer(), (c4OurLen + 1) * kCIDLib::c4CharBytes);
    return pszRet;
}


tCIDLib::TVoid
TString::Replace(const  tCIDLib::TCard4     c4RepStart
                , const tCIDLib::TCard4     c4RepEnd
                , const tCIDLib::TCh* const pszRepText
                , const tCIDLib::TCard4     c4RepLen)
{
    //
    //  Get the rep text len. If the len is zero, then the caller doesn't know
    //  so we figure it out.
    //
    const tCIDLib::TCard4 c4ActualLen(c4RepLen ? c4RepLen : TRawStr::c4StrLen(pszRepText));

    //
    //  Calculate the size of the substring we are replacing. Check for the
    //  problems of bad start/end values.
    //
    if (c4RepStart > c4RepEnd)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_StartPastEnd
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4RepStart)
            , TCardinal(c4RepEnd)
        );
    }

    const tCIDLib::TCard4 c4OurLen = m_strbData.c4CurEnd();
    if (c4RepEnd > c4OurLen)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4RepEnd)
            , clsThis()
            , TCardinal(c4OurLen)
        );
    }

    //
    //  Lets first test for the optimum scenario, where the replacement token
    //  takes exactly the same chars as what is being replaced. In this case,
    //  we can just copy it directly over.
    //
    //  The other two scenarios are that the replacement text is bigger than
    //  the target area, and when its smaller than the target area.
    //
    const tCIDLib::TCard4 c4TargetLen = (c4RepEnd - c4RepStart) + 1;
    if (c4TargetLen == c4ActualLen)
    {
        TRawMem::CopyMemBuf
        (
            m_strbData.pszBufferAt(c4RepStart), pszRepText, c4ActualLen * kCIDLib::c4CharBytes
        );
    }
     else if (c4TargetLen > c4ActualLen)
    {
        //
        //  The target is bigger that the replacement text. So we can copy
        //  the replace in, then copy down the remainder. Get the buffer out and
        //  use it for any offsets, since calling buffer at will cap at the current
        //  length, which will mess us up.
        //
        tCIDLib::TCh* pszBase = m_strbData.pszBuffer();
        TRawMem::CopyMemBuf
        (
            pszBase + c4RepStart, pszRepText, c4ActualLen * kCIDLib::c4CharBytes
        );

        //
        //  Copy down any text from the replacement end point to the end
        //  of the replacement text we just copied in.
        //
        tCIDLib::TCh* pszTarget = pszBase + (c4RepStart + c4ActualLen);
        tCIDLib::TCh* pszSrc = pszBase + (c4RepStart + c4TargetLen);
        const tCIDLib::TCh* pszEnd = pszBase + m_strbData.c4CurEnd();
        while (pszSrc < pszEnd)
            *pszTarget++ = *pszSrc++;

        // Recalc the length of the string
        m_strbData.DecEnd(c4TargetLen - c4ActualLen);
    }
     else
    {
        //
        //  The replacement text is bigger than the target. So we need to
        //  move up the text from the end to make room for it. We have to
        //  deal with expanding the buffer if it won't hold the text.
        //
        if ((c4OurLen - c4TargetLen) + c4ActualLen > m_strbData.c4BufSz())
        {
            //
            //  We do have to expand. We have to just make a copy of the existing content
            //  and then expand, destroying previous content.
            //
            TArrayJanitor<tCIDLib::TCh> janOld(pszDupBuffer());
            m_strbData.ExpandBy(c4ActualLen, kCIDLib::False);

            tCIDLib::TCh* pszBase = m_strbData.pszBuffer();
            TRawMem::CopyMemBuf(pszBase, janOld.paThis(), c4RepStart * kCIDLib::c4CharBytes);
            TRawMem::CopyMemBuf
            (
                pszBase + c4RepStart, pszRepText, c4ActualLen * kCIDLib::c4CharBytes
            );

            CIDLib_Suppress(6011 28182)  // We null checked above
            TRawMem::CopyMemBuf
            (
                pszBase + (c4RepStart + c4ActualLen)
                , &janOld.paThis()[c4RepEnd + 1]
                , (c4OurLen - c4RepEnd) * kCIDLib::c4CharBytes
            );

            m_strbData.SetEnd(c4OurLen + (c4ActualLen - c4TargetLen));
        }
         else
        {
            //
            //  The buffer is big enough, so we need to move up the trailing
            //  text to make room for the replacement text. First we calc how
            //  many chars we have to move it up. Its the rep text length
            //  minus the target length.
            //
            tCIDLib::TCh* pszBase = m_strbData.pszBuffer(kCIDLib::False);
            const tCIDLib::TCard4 c4ExpCount = c4ActualLen - c4TargetLen;
            tCIDLib::TCh* pszTarget = pszBase + (c4OurLen + c4ExpCount);
            tCIDLib::TCh* pszSrc = pszBase + c4OurLen;
            while (pszSrc > (pszBase  + c4RepEnd))
                *pszTarget-- = *pszSrc--;

            // Copy the replacement text into place now
            TRawMem::CopyMemBuf
            (
                m_strbData.pszBufferAt(c4RepStart, kCIDLib::False)
                , pszRepText
                , c4ActualLen * kCIDLib::c4CharBytes
            );

            m_strbData.IncEnd(c4ActualLen - c4TargetLen);
        }
    }
}


// Can't be inline because TStringView isn't complete there
tCIDLib::TVoid TString::Set(const TStringView& strvSrc, const tCIDLib::TCard4 c4Extra)
{
    m_strbData.Clear();
    m_strbData.Append(strvSrc.pszBuffer(), strvSrc.c4Length(), c4Extra);
}


//
//  Sets our contents to a formatted version of the passed value, using the
//  indicated formatting information.
//
//  The trailing parameter is a dummy one that helps us deal with an otherwise
//  much more messy issue with the variac recursion processing of Format().
//
tCIDLib::TVoid TString::SetFormatted(const MFormattable& fmtblSrc, const tCIDLib::TCard4)
{
    // Create a temporary stream over ourself to stream to, telling it not to append
    TTextStringOutStream strmTar
    (
        this, tCIDLib::EAdoptOpts::NoAdopt, kCIDLib::False
    );
    strmTar << fmtblSrc << kCIDLib::FlushIt;
}

tCIDLib::TVoid
TString::SetFormatted(  const   tCIDLib::TCard1     c1ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    Clear();
    AppendFormatted(c1ToFmt, eRadix, c4GroupSize, chGroupSep);
}

tCIDLib::TVoid
TString::SetFormatted(  const   tCIDLib::TCard2     c2ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    Clear();
    AppendFormatted(c2ToFmt, eRadix, c4GroupSize, chGroupSep);
}

tCIDLib::TVoid
TString::SetFormatted(  const   tCIDLib::TCard4     c4ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    Clear();
    AppendFormatted(c4ToFmt, eRadix, c4GroupSize, chGroupSep);
}

tCIDLib::TVoid
TString::SetFormatted(  const   tCIDLib::TCard8     c8ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    Clear();
    AppendFormatted(c8ToFmt, eRadix, c4GroupSize, chGroupSep);
}

tCIDLib::TVoid
TString::SetFormatted(  const   tCIDLib::TFloat8    f8ToFmt
                        , const tCIDLib::TCard4     c4DecPlaces)
{
    Clear();
    AppendFormatted(f8ToFmt, c4DecPlaces);
}

tCIDLib::TVoid
TString::SetFormatted(  const   tCIDLib::TFloat8    f8ToFmt
                        , const tCIDLib::TCard4     c4DecPlaces
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCh        chDecimal
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    Clear();
    AppendFormatted
    (
        f8ToFmt, c4DecPlaces, chNegSign, chDecimal, c4GroupSize, chGroupSep
    );
}

tCIDLib::TVoid
TString::SetFormatted(  const   tCIDLib::TInt1      i1ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    Clear();
    AppendFormatted(i1ToFmt, eRadix, chNegSign, c4GroupSize, chGroupSep);
}

tCIDLib::TVoid
TString::SetFormatted(  const   tCIDLib::TInt2      i2ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    Clear();
    AppendFormatted(i2ToFmt, eRadix, chNegSign, c4GroupSize, chGroupSep);
}

tCIDLib::TVoid
TString::SetFormatted(  const   tCIDLib::TInt4      i4ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    Clear();
    AppendFormatted(i4ToFmt, eRadix, chNegSign, c4GroupSize, chGroupSep);
}

tCIDLib::TVoid
TString::SetFormatted(  const   tCIDLib::TInt8      i8ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    Clear();
    AppendFormatted(i8ToFmt, eRadix, chNegSign, c4GroupSize, chGroupSep);
}

// Just force this one to the TInt4 version
tCIDLib::TVoid
TString::SetFormatted(  const   tCIDLib::TSInt      iToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    Clear();
    AppendFormatted
    (
        tCIDLib::TInt4(iToFmt), eRadix, chNegSign, c4GroupSize, chGroupSep
    );
}


//
//  Set the last character in this string to the passed value. If empty, this
//  does nothing.
//
tCIDLib::TVoid TString::SetLast(const tCIDLib::TCh chNew)
{
    if (m_strbData.bIsEmpty())
        return;

    // Just put the new char in the last character
    m_strbData.PutAt(m_strbData.c4CurEnd() - 1, chNew);
}

tCIDLib::TVoid TString::Strip(  const   tCIDLib::TCh* const     pszStripChars
                                , const tCIDLib::EStripModes    eMode
                                , const tCIDLib::TCh            chRepChar)
{
    TRawStr::StripStr(m_strbData.pszBuffer(), pszStripChars, eMode, chRepChar);
    m_strbData.ResetEnd();
}

tCIDLib::TVoid TString::Strip(  const   TString&                strStripChars
                                , const tCIDLib::EStripModes    eMode
                                , const tCIDLib::TCh            chRepChar)
{
    TRawStr::StripStr(m_strbData.pszBuffer(), strStripChars.pszBuffer(), eMode, chRepChar);
    m_strbData.ResetEnd();
}


//
//  Strips the standard set of whitespace chars from us, using the passed
//  strip mode.
//
tCIDLib::TVoid
TString::StripWhitespace(const  tCIDLib::EStripModes    eStripMode
                        , const tCIDLib::TCh            chRepChar)
{
    Strip(kCIDLib::szWhitespace, eStripMode, chRepChar);
}


tCIDLib::TVoid TString::ToLower(const   tCIDLib::TCard4 c4StartInd
                                , const tCIDLib::TCard4 c4Len)
{
    tCIDLib::TCard4 c4Actual;

    //
    //  If the length is max card, then we want to just make it long enough
    //  to get to the end of the string.
    //
    c4Actual = c4Len;
    const tCIDLib::TCard4 c4OurLen = m_strbData.c4CurEnd();
    if (c4Actual == kCIDLib::c4MaxCard)
    {
        // This can't be right
        if (c4StartInd > c4OurLen)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_StartPastEnd
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::BadParms
                , TCardinal(c4StartInd)
                , TCardinal(c4OurLen)
            );
        }
        c4Actual = c4OurLen - c4StartInd;
    }

    // If no length, then simplify below by just stopping
    if (!c4Actual)
        return;

    if ((tCIDLib::TCard8(c4StartInd) + tCIDLib::TCard8(c4Actual) > kCIDLib::c4MaxCard)
    ||  (c4StartInd + c4Actual > c4OurLen))
    {
        //
        // The start plus the length can't go beyond the end. Be sure we don't get
        //  caught by some wrap around issue first though.
        //
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_Range
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(c4StartInd)
            , TCardinal(c4Actual)
            , TCardinal(c4OurLen)
        );
    }

    // If the length would take us to the end of the string, then do it directly
    if (c4StartInd + c4Actual >= c4OurLen)
    {
        TRawStr::pszLowerCase(m_strbData.pszBufferAt(c4StartInd));
        return;
    }
     else
    {
        // Else, we have to use a loop
        tCIDLib::TCh* pszTar = m_strbData.pszBufferAt(c4StartInd);
        tCIDLib::TCh* pszEnd = m_strbData.pszBufferAt(c4StartInd + c4Actual);
        while (pszTar < pszEnd)
        {
            *pszTar = TRawStr::chLower(*pszTar);
            pszTar++;
        }
    }
}


tCIDLib::TVoid TString::ToUpper(const   tCIDLib::TCard4 c4StartInd
                                , const tCIDLib::TCard4 c4Len)
{
    tCIDLib::TCard4 c4Actual;

    //
    //  If the length is max card, then we want to just make it long enough
    //  to get to the end of the string.
    //
    c4Actual = c4Len;
    const tCIDLib::TCard4 c4OurLen = m_strbData.c4CurEnd();
    if (c4Actual == kCIDLib::c4MaxCard)
    {
        // This can't be right
        if (c4StartInd > c4OurLen)
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcStr_StartPastEnd
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::BadParms
                , TCardinal(c4StartInd)
                , TCardinal(c4OurLen)
            );
        }
        c4Actual = c4OurLen - c4StartInd;
    }

    // If no length, then simplify below by just stopping
    if (!c4Actual)
        return;

    if ((tCIDLib::TCard8(c4StartInd) + tCIDLib::TCard8(c4Actual) > kCIDLib::c4MaxCard)
    ||  (c4StartInd + c4Actual > c4OurLen))
    {
        //
        // The start plus the length can't go beyond the end. Be sure we don't get
        //  caught by some wrap around issue first though.
        //
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_Range
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(c4StartInd)
            , TCardinal(c4Actual)
            , TCardinal(c4OurLen)
        );
    }

    //
    //  If the length would take us to the end of the string, then do it directly. Note
    //  that our buffer not being allocated would have been caught above by an index
    //  error or the requested count being zero.
    //
    if (c4StartInd + c4Actual >= c4OurLen)
    {
        TRawStr::pszUpperCase(m_strbData.pszBufferAt(c4StartInd));
        return;
    }
     else
    {
        tCIDLib::TCh* pszTar = m_strbData.pszBufferAt(c4StartInd);
        tCIDLib::TCh* pszEnd = m_strbData.pszBufferAt(c4StartInd + c4Actual);
        while (pszTar < pszEnd)
        {
            *pszTar = TRawStr::chUpper(*pszTar);
            pszTar++;
        }
    }
}

// Just call the generalized one
tCIDLib::TVoid
TString::ToZStr(        tCIDLib::TCh* const pszTarget
                , const tCIDLib::TCard4     c4MaxChars
                , const tCIDLib::TCard4     c4StartInd) const
{
    ToZStr(pszTarget, c4MaxChars, c4StartInd, kCIDLib::True);
}


// ---------------------------------------------------------------------------
//  TString: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TString::FormatTo(CIOP TTextOutStream& strmToWriteTo) const
{
    strmToWriteTo << *this;
}


tCIDLib::TVoid TString::StreamFrom(CIOP TBinInStream& strmToReadFrom)
{
    try
    {
        //
        //  The first thing has to be a start object marker. We do this in
        //  order to minimize the possibility of reading some random value
        //  and trying to allocate a huge buffer.
        //
        strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

        // Check the format version
        tCIDLib::TCard1 c1FmtVersion;
        strmToReadFrom  >> c1FmtVersion;
        if (!c1FmtVersion || (c1FmtVersion > CIDLib_String::c1FmtVersion))
        {
            facCIDLib().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kCIDErrs::errcGen_UnknownFmtVersion
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
                , TCardinal(c1FmtVersion)
                , clsThis()
            );
        }

        // Read the number of chars we are gong to read in
        tCIDLib::TCard4 c4CharsStored;
        strmToReadFrom >> c4CharsStored;

        //
        //  If the cur buffer size is smaller than the stored one, then we have
        //  to reallocate, don't have to preserve
        //
        m_strbData.ExpandTo(c4CharsStored, kCIDLib::False);

        //
        //  Only stream in new chars if we actually stored any chars during
        //  the out, which we might not have. We call the standard stream
        //  method to do this, since we use its outgoing counterpart to write
        //  the data out. It converts it from the 'on the wire' canonical
        //  format to the internal wide char format.
        //
        if (c4CharsStored)
            strmToReadFrom.ReadArray(m_strbData.pszBuffer(), c4CharsStored);

        // And it should end with an end object marker
        strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);

        // And now update our current char count since it seemed to have worked
        m_strbData.SetEnd(c4CharsStored);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // Don't leave junk in the string
        m_strbData.Clear();
        throw;
    }
}


tCIDLib::TVoid TString::StreamTo(CIOP TBinOutStream& strmToWriteTo) const
{
    //
    //  Write a start object marker to start the object, and write out the
    //  number of chars that have now, to use as a sanity check on the way back
    //  in. And it will allow us to pre-expand the target string to hold all
    //  the chars we are going to stream in, to avoid lots of re-allocations of
    //  the buffer. We also put out a format version, so that we can upgrade
    //  this format automatically later.
    //
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDLib_String::c1FmtVersion
                    << m_strbData.c4CurEnd();

    //
    //  If any chars to write, then stream them out. The output stream provides
    //  a method to do this. It converts them to a canonical format for
    //  streaming, so always use this to write char arrays.
    //
    //  If we don't have a buffer, then current end is zero and we won't access
    //  it anyway.
    //
    if (m_strbData.c4CurEnd())
        strmToWriteTo.WriteArray(m_strbData.pszBuffer(), m_strbData.c4CurEnd());

    // And finish off with an end object marker
    strmToWriteTo << tCIDLib::EStreamMarkers::EndObject;
}


// ---------------------------------------------------------------------------
//  TString: Private, static methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TString::BadHexChar(const tCIDLib::TCh chToXlat)
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcStr_BadHexChar
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::Format
        , TCardinal(tCIDLib::TCard4(chToXlat))
    );
}


// ---------------------------------------------------------------------------
//  TString: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Check whether this string starts with the passed string, with optional
//  case sensitivity or insensitivity. We have two versions so that when
//  string objects are passed as the prefix to check we can do the fast
//  check for length and reject it immediately if too long.
//
tCIDLib::TBoolean
TString::bCheckPrefix(  const   tCIDLib::TCh* const pszToCheck
                        , const tCIDLib::TBoolean   bCaseSensitive) const noexcept
{
    // if the check string is null, just say no
    if (!pszToCheck)
        return kCIDLib::False;

    const tCIDLib::TCh* pszUs = pszBuffer();
    const tCIDLib::TCh* pszThem = pszToCheck;

    //
    //  NOTE that we'll get a difference if we hit the end of either of
    //  them first, because of the null terminator, so we don't need an
    //  explicit end of string check for both strings. We just go to the
    //  end of the src string.
    //
    //  And we need to do this in order to only break out on a change.
    //  If we broke out when we hit either end, we'd think that they
    //  matched.
    //
    if (bCaseSensitive)
    {
        do
        {
            if (*pszUs++ != *pszThem++)
                return kCIDLib::False;
        }   while (*pszThem);
    }
     else
    {
        do
        {
            if (TRawStr::chUpper(*pszUs++) != TRawStr::chUpper(*pszThem++))
                return kCIDLib::False;
        }   while (*pszThem);
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean
TString::bCheckPrefix(  const   TString&            strToCheck
                        , const tCIDLib::TBoolean   bCaseSensitive) const noexcept
{
    const tCIDLib::TCard4 c4SrcLen = strToCheck.m_strbData.c4CurEnd();

    // Do the fast check, where this string is shorter
    if (m_strbData.c4CurEnd() < c4SrcLen)
        return kCIDLib::False;

    // Oh well, check the strings
    const tCIDLib::TCh* pszUs = m_strbData.pszBuffer();
    const tCIDLib::TCh* pszThem = strToCheck.m_strbData.pszBuffer();

    if (bCaseSensitive)
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SrcLen; c4Index++)
        {
            if (*pszUs++ != *pszThem++)
                return kCIDLib::False;
        }
    }
     else
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SrcLen; c4Index++)
        {
            if (TRawStr::chUpper(*pszUs++) != TRawStr::chUpper(*pszThem++))
                return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


//
//  Check whether this string ends with the passed string, with optional
//  case sensitivity or insensitivity. We have two versions so that when
//  string objects are passed as the suffxi to check we can do the fast
//  check for length and reject it immediately if too long.
//
tCIDLib::TBoolean
TString::bCheckSuffix(  const  tCIDLib::TCh* const  pszToCheck
                        , const tCIDLib::TBoolean   bCaseSensitive) const noexcept
{
    // if the check string is null, just say no
    if (!pszToCheck)
        return kCIDLib::False;

    //
    //  Calc where we need to start comparing in our buffer. And since we
    //  have to do the source length, do the quick check to see if he is
    //  longer than us and therefore it could never be true, or where
    //  the suffix is empty.
    //
    const tCIDLib::TCard4 c4SrcLen = TRawStr::c4StrLen(pszToCheck);
    if (!c4SrcLen || (m_strbData.c4CurEnd() < c4SrcLen))
        return kCIDLib::False;

    //
    //  Set up pointers to run up. Buffer at could throw, but we are insuring above
    //  that we won't get past the end.
    //
    const tCIDLib::TCh* pszUs = m_strbData.pszAtEnd() - c4SrcLen;
    const tCIDLib::TCh* pszThem = pszToCheck;

    // In this case, we had to get the length so we can use a for loop
    if (bCaseSensitive)
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SrcLen; c4Index++)
        {
            if (*pszUs++ != *pszThem++)
                return kCIDLib::False;
        }
    }
     else
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SrcLen; c4Index++)
        {
            if (TRawStr::chUpper(*pszUs++) != TRawStr::chUpper(*pszThem++))
                return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean
TString::bCheckSuffix(  const   TString&            strToCheck
                        , const tCIDLib::TBoolean   bCaseSensitive) const noexcept
{
    const tCIDLib::TCard4 c4SrcLen = strToCheck.m_strbData.c4CurEnd();

    //
    //  Do the fast check, where this string is shorter, or the suffix is
    //  empty.
    //
    if (!c4SrcLen || (m_strbData.c4CurEnd() < c4SrcLen))
        return kCIDLib::False;

    // Oh well, check the strings
    const tCIDLib::TCh* pszUs = m_strbData.pszAtEnd() - c4SrcLen;
    const tCIDLib::TCh* pszThem = strToCheck.pszBuffer();

    if (bCaseSensitive)
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SrcLen; c4Index++)
        {
            if (*pszUs++ != *pszThem++)
                return kCIDLib::False;
        }
    }
     else
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4SrcLen; c4Index++)
        {
            if (TRawStr::chUpper(*pszUs++) != TRawStr::chUpper(*pszThem++))
                return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}


tCIDLib::TVoid TString::SetFromShort(const tCIDLib::TSCh* const pszSrc)
{
    m_strbData.AdoptBuffer(TRawStr::pszConvert(pszSrc), 0);
}


//
//  This method will copy chars from this string to the target buffer. It
//  copy up to the indicated max chars (i.e. the most the target can hold.
//  It can optionally start at a specific index within this string.
//
//  It optionally will add a null terminator to the target or not. This
//  allows it to be used as the implemention of more than one public method.
//
//  If you tell it to, then the buffer has to be at least one char larger
//  than c4MaxChars!
//
tCIDLib::TVoid
TString::ToZStr(        tCIDLib::TCh* const pszTarget
                , const tCIDLib::TCard4     c4MaxChars
                , const tCIDLib::TCard4     c4StartInd
                , const tCIDLib::TBoolean   bAddNull) const
{
    if (!pszTarget)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_NullTarget
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }

    // Clear the target string
    CIDLib_Suppress(6011)  // We null checked above
    pszTarget[0] = kCIDLib::chNull;

    // If no chars in this string, then we are done
    if (m_strbData.bIsEmpty())
        return;

    // If start index is at end of string, then we are done
    const tCIDLib::TCard4 c4OurLen = m_strbData.c4CurEnd();
    if (c4StartInd == c4OurLen)
        return;

    // If the start index is >= than the length of this string, then an error
    if (c4StartInd >= c4OurLen)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4StartInd)
            , clsThis()
            , TCardinal(c4OurLen)
        );
    }

    //
    //  Copy as many chars as will fit or are available, whichever is
    //  smaller. We have to do this as a raw memory op, because of the
    //  optional null terminator. The TRawStr::CopyStr method will add
    //  a null.
    //
    const tCIDLib::TCard4 c4ToCopy
    (
        tCIDLib::MinVal(c4MaxChars, c4OurLen - c4StartInd)
    );
    TRawMem::CopyMemBuf(pszTarget, m_strbData.pszBufferAt(c4StartInd), c4ToCopy * kCIDLib::c4CharBytes);

    // Cap off the target appropriately if asked to
    if (bAddNull)
        pszTarget[c4ToCopy] = kCIDLib::chNull;
}



// ---------------------------------------------------------------------------
//   CLASS: TString::TStrBuf
//  PREFIX: strb
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TString::TStrBuf: Constructors and destructor
// ---------------------------------------------------------------------------

//
//  Call this if the init size is larger than small buffer. Nothing will
//  go wrong if you do otherwise, but the default ctor will handle that
//  already.
//
TString::TStrBuf::TStrBuf(const tCIDLib::TCard4 c4InitSz) :

    m_c4BufSz(c4InitSz)
    , m_c4CurEnd(0)
{
    m_szSmallBuf[0] = kCIDLib::chNull;

    if (m_c4BufSz < c4SmallBufSz)
        m_c4BufSz = c4SmallBufSz;

    if (m_c4BufSz > c4SmallBufSz)
        m_pszBuffer = new tCIDLib::TCh[m_c4BufSz + 1];
    else
        m_pszBuffer = m_szSmallBuf;
    m_pszBuffer[0] = kCIDLib::chNull;
}


// Construct from an existing string, optionally allocating extra chars for subsequent use
TString::TStrBuf::TStrBuf(  const   tCIDLib::TCh* const pszSrc
                            , const tCIDLib::TCard4     c4SrcLen
                            , const tCIDLib::TCard4     c4ExtraChars) :
    m_szSmallBuf(L"")
{
    if (pszSrc)
    {
        // We have a source string. Get the length. If zero, we get the size ourself
        m_c4CurEnd = c4SrcLen;
        if (!m_c4CurEnd)
            m_c4CurEnd = TRawStr::c4StrLen(pszSrc);
    }

    const tCIDLib::TCard4 c4NewSz = m_c4CurEnd + c4ExtraChars;
    if (c4NewSz > c4SmallBufSz)
    {
        m_c4BufSz = c4NewSz;
        m_pszBuffer = new tCIDLib::TCh[m_c4BufSz + 1];
        TRawMem::CopyMemBuf(m_pszBuffer, pszSrc, m_c4CurEnd * kCIDLib::c4CharBytes);
    }
     else
    {
        // We can just copy into the small buffer
        m_pszBuffer = m_szSmallBuf;
        m_c4BufSz = c4SmallBufSz;
        TRawMem::CopyMemBuf(m_pszBuffer, pszSrc, m_c4CurEnd * kCIDLib::c4CharBytes);
    }
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
}


// Construct from an existing string, which we may be asked to adopt
TString::TStrBuf::TStrBuf(          tCIDLib::TCh* const     pszToAdopt
                            , const tCIDLib::EAdoptOpts     eAdopt) :

    m_szSmallBuf(L"")
{
    if (!pszToAdopt)
    {
        // If null, just set us up as empty in the short string
        m_pszBuffer = m_szSmallBuf;
        m_pszBuffer[0] = kCIDLib::chNull;
        m_c4CurEnd = 0;
        m_c4BufSz = c4SmallBufSz;
    }
     else
    {
        m_c4CurEnd = TRawStr::c4StrLen(pszToAdopt);

        if ((eAdopt == tCIDLib::EAdoptOpts::NoAdopt) && (m_c4CurEnd <= c4SmallBufSz))
        {
            // Copy to small buffer
            m_pszBuffer = m_szSmallBuf;
            TRawMem::CopyMemBuf(m_pszBuffer, pszToAdopt, m_c4CurEnd * kCIDLib::c4CharBytes);
            m_c4BufSz = c4SmallBufSz;
            m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
        }
         else
        {
            m_c4BufSz = m_c4CurEnd;
            if (eAdopt == tCIDLib::EAdoptOpts::Adopt)
                m_pszBuffer = pszToAdopt;
            else
                m_pszBuffer = TRawStr::pszReplicate(pszToAdopt);
        }
    }
}

TString::TStrBuf::TStrBuf(const TStrBuf& strbSrc) :

    m_c4BufSz(strbSrc.m_c4BufSz)
    , m_c4CurEnd(strbSrc.m_c4CurEnd)
    , m_szSmallBuf(L"")
{
    m_szSmallBuf[0] = kCIDLib::chNull;
    if (strbSrc.m_pszBuffer == strbSrc.m_szSmallBuf)
    {
        CIDAssert(strbSrc.m_c4CurEnd <= c4SmallBufSz, L"Small buffer string is too big");

        // We just copy the small buffer including null
        m_pszBuffer = m_szSmallBuf;
        m_c4BufSz = c4SmallBufSz;
        TRawMem::CopyMemBuf
        (
            m_pszBuffer
            , strbSrc.m_szSmallBuf
            , tCIDLib::c4ArrayElems(m_szSmallBuf) * kCIDLib::c4CharBytes
        );
    }
     else
    {
        //
        //  We need to allocate and copy. It's not necessarily null terminated, since
        //  we only do that on access, so we term our copy.
        //
        m_pszBuffer = new tCIDLib::TCh[m_c4BufSz + 1];
        TRawMem::CopyMemBuf(m_pszBuffer, strbSrc.m_pszBuffer, m_c4CurEnd * kCIDLib::c4CharBytes);
        m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
    }
}

// Set up empty small buffer and then call move operator
TString::TStrBuf::TStrBuf(TStrBuf&& strbSrc) :

    m_c4BufSz(c4SmallBufSz)
    , m_pszBuffer(m_szSmallBuf)
    , m_szSmallBuf(L"")
{
    *this = tCIDLib::ForceMove(strbSrc);
}

TString::TStrBuf::TStrBuf(const TKrnlString& kstrSrc) :

    m_c4BufSz(kstrSrc.c4Length())
    , m_c4CurEnd(kstrSrc.c4Length())
    , m_szSmallBuf(L"")
{
    // If it will fit in the small string do that, else allocate
    if (m_c4BufSz <= c4SmallBufSz)
    {
        m_pszBuffer = m_szSmallBuf;
        m_c4BufSz = c4SmallBufSz;
    }
     else
    {
        m_pszBuffer = new tCIDLib::TCh[m_c4BufSz + 1];
        m_szSmallBuf[0] = kCIDLib::chNull;
    }

    TRawMem::CopyMemBuf(m_pszBuffer, kstrSrc.pszValue(), m_c4CurEnd * kCIDLib::c4CharBytes);
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
}

// We just steal his buffer
TString::TStrBuf::TStrBuf(TKrnlString&& kstrSrc) :

    m_szSmallBuf(L"")
{
    m_pszBuffer = kstrSrc.pszOrphanBuffer(m_c4BufSz, m_c4CurEnd);
}

TString::TStrBuf::~TStrBuf()
{
    if (m_pszBuffer != m_szSmallBuf)
    {
        delete [] m_pszBuffer;
        m_pszBuffer = nullptr;
    }
}

// ---------------------------------------------------------------------------
//  TString::TStrBuf: Public operators
// ---------------------------------------------------------------------------

//
//  These are unusual in that we have to deal with the four variations of
//  him and us being in small buffer mode or not.
//
TString::TStrBuf& TString::TStrBuf::operator=(const TStrBuf& strbSrc)
{
    if (&strbSrc != this)
    {
        const tCIDLib::TBoolean bSmallSrc = strbSrc.m_pszBuffer == strbSrc.m_szSmallBuf;
        const tCIDLib::TBoolean bSmallTar = m_pszBuffer == m_szSmallBuf;
        if (bSmallSrc && bSmallTar)
        {
            // We are both small, so just copy his small buffer
            m_c4CurEnd = strbSrc.m_c4CurEnd;
            m_c4BufSz = strbSrc.m_c4BufSz;
            TRawMem::CopyMemBuf(m_pszBuffer, strbSrc.m_pszBuffer, m_c4CurEnd * kCIDLib::c4CharBytes);
        }
         else if (!bSmallSrc && bSmallTar)
        {
            // He's not small and we are, so we have to allocate
            m_c4CurEnd = strbSrc.m_c4CurEnd;
            m_c4BufSz = strbSrc.m_c4BufSz;
            m_pszBuffer = new tCIDLib::TCh[m_c4BufSz + 1];
            TRawMem::CopyMemBuf(m_pszBuffer, strbSrc.m_pszBuffer, m_c4CurEnd * kCIDLib::c4CharBytes);

            m_szSmallBuf[0] = kCIDLib::chNull;
        }
         else if (bSmallSrc && !bSmallTar)
        {
            //
            //  He is small and we are not. If our buffer is big enough, just copy it over. Else drop
            //  our buffer and use the small buffer.
            //
            if (m_c4BufSz < strbSrc.m_c4CurEnd)
            {
                delete [] m_pszBuffer;
                m_pszBuffer = nullptr;
                m_pszBuffer = m_szSmallBuf;
                m_c4BufSz = c4SmallBufSz;
            }

            m_c4CurEnd = strbSrc.m_c4CurEnd;
            TRawMem::CopyMemBuf(m_pszBuffer, strbSrc.m_pszBuffer, m_c4CurEnd * kCIDLib::c4CharBytes);
        }
         else
        {
            //  Neither is amall, so reallocate if we need to.
            if (strbSrc.m_c4CurEnd > m_c4BufSz)
            {
                delete [] m_pszBuffer;
                m_pszBuffer = nullptr;
                m_pszBuffer = new tCIDLib::TCh[strbSrc.m_c4CurEnd + 1];
            }

            m_c4CurEnd = strbSrc.m_c4CurEnd;
            m_c4BufSz = m_c4CurEnd;
            TRawMem::CopyMemBuf(m_pszBuffer, strbSrc.m_pszBuffer, m_c4CurEnd * kCIDLib::c4CharBytes);
        }
        m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
    }
    return *this;
}

TString::TStrBuf& TString::TStrBuf::operator=(TStrBuf&& strbSrc)
{
    if (&strbSrc != this)
    {
        const tCIDLib::TBoolean bSmallSrc = strbSrc.m_pszBuffer == strbSrc.m_szSmallBuf;
        const tCIDLib::TBoolean bSmallTar = m_pszBuffer == m_szSmallBuf;

        if (bSmallSrc && bSmallTar)
        {
            // We have to just swap the small buffers, including null
            tCIDLib::TCh achTmp[c4SmallBufSz + 1];
            TRawMem::CopyMemBuf
            (
                achTmp
                , m_szSmallBuf
                , tCIDLib::c4ArrayElems(m_szSmallBuf) * kCIDLib::c4CharBytes
            );
            TRawMem::CopyMemBuf
            (
                m_szSmallBuf
                , strbSrc.m_szSmallBuf
                , tCIDLib::c4ArrayElems(m_szSmallBuf) * kCIDLib::c4CharBytes
            );
            TRawMem::CopyMemBuf
            (
                strbSrc.m_szSmallBuf
                , achTmp
                , tCIDLib::c4ArrayElems(m_szSmallBuf) * kCIDLib::c4CharBytes
            );

            // Swap current ends, buffers are teh same size already
            tCIDLib::Swap(m_c4CurEnd, strbSrc.m_c4CurEnd);
        }
         else if (!bSmallSrc && bSmallTar)
        {
            // Copy our small buffer to his, and steal his buffer
            TRawMem::CopyMemBuf
            (
                strbSrc.m_szSmallBuf
                , m_szSmallBuf
                , tCIDLib::c4ArrayElems(m_szSmallBuf) * kCIDLib::c4CharBytes
            );
            m_pszBuffer = strbSrc.m_pszBuffer;
            m_szSmallBuf[0] = kCIDLib::chNull;

            // Now we can point him at his small buffer
            strbSrc.m_pszBuffer = strbSrc.m_szSmallBuf;

            // And we can swap the size/end values
            tCIDLib::Swap(m_c4BufSz, strbSrc.m_c4BufSz);
            tCIDLib::Swap(m_c4CurEnd, strbSrc.m_c4CurEnd);
        }
         else if (bSmallSrc && !bSmallTar)
        {
            //
            //  The opposite of above. Copy his small buffer to ours and give him
            //  our buffer pointer.
            //
            TRawMem::CopyMemBuf
            (
                m_szSmallBuf
                , strbSrc.m_szSmallBuf
                , tCIDLib::c4ArrayElems(m_szSmallBuf) * kCIDLib::c4CharBytes
            );
            strbSrc.m_pszBuffer = m_pszBuffer;
            strbSrc.m_szSmallBuf[0] = kCIDLib::chNull;

            // Now point us to our small buffer and swap the size/end
            m_pszBuffer = m_szSmallBuf;
            tCIDLib::Swap(m_c4BufSz, strbSrc.m_c4BufSz);
            tCIDLib::Swap(m_c4CurEnd, strbSrc.m_c4CurEnd);
        }
        else
        {
            // We can just swap buffers
            tCIDLib::Swap(m_c4BufSz, strbSrc.m_c4BufSz);
            tCIDLib::Swap(m_c4CurEnd, strbSrc.m_c4CurEnd);
            tCIDLib::Swap(m_pszBuffer, strbSrc.m_pszBuffer);
        }
    }
    return *this;
}

TString::TStrBuf& TString::TStrBuf::operator=(const TStringView& strvSrc)
{
    if (strvSrc.c4Length() > m_c4BufSz)
        ExpandTo(strvSrc.c4Length(), kCIDLib::False);

    m_c4CurEnd = strvSrc.c4Length();
    TRawMem::CopyMemBuf(m_pszBuffer, strvSrc.pszBuffer(), m_c4CurEnd * kCIDLib::c4CharBytes);
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;

    return *this;
}


//
//  We just steal his buffer
//
TString::TStrBuf& TString::TStrBuf::operator=(TKrnlString&& kstrSrc)
{
    if (m_pszBuffer != m_szSmallBuf)
    {
        delete [] m_pszBuffer;
        m_pszBuffer = nullptr;
    }
    m_pszBuffer = kstrSrc.pszOrphanBuffer(m_c4BufSz, m_c4CurEnd);
    m_szSmallBuf[0] = kCIDLib::chNull;
    return *this;
}


// ---------------------------------------------------------------------------
//  TString::TStrBuf: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  We can be given a buffer to adopt. If length is zero we get the length. If c4Len is zero, the
//  it damn well better be null terminated.
//
tCIDLib::TVoid TString::TStrBuf::AdoptBuffer(tCIDLib::TCh* const pszToAdopt, const tCIDLib::TCard4 c4Len)
{
    if (m_pszBuffer != m_szSmallBuf)
    {
        delete [] m_pszBuffer;
        m_pszBuffer = nullptr;
    }

    if (pszToAdopt == nullptr)
    {
        m_pszBuffer = m_szSmallBuf;
        m_c4CurEnd = 0;
        m_c4BufSz = c4SmallBufSz;
        m_szSmallBuf[0] = kCIDLib::chNull;
    }
     else
    {
        m_pszBuffer = pszToAdopt;
        m_c4CurEnd = c4Len;
        if (!m_c4CurEnd)
            m_c4CurEnd = TRawStr::c4StrLen(pszToAdopt);
        m_c4BufSz = m_c4CurEnd;
        m_szSmallBuf[0] = kCIDLib::chNull;
    }
}


//
//  If the caller is going to do multiple appends, he should calculate the required
//  capacity and call Expand() first if he wants to be efficient.
//
tCIDLib::TVoid
TString::TStrBuf::Append(const  tCIDLib::TCh* const     pszSrc
                        , const tCIDLib::TCard4         c4Count
                        , const tCIDLib::TCard4         c4Extra)
{
    if ((m_c4CurEnd + c4Count + c4Extra) > m_c4BufSz)
        ExpandBy(c4Count + c4Extra, kCIDLib::True);

    TRawMem::CopyMemBuf(&m_pszBuffer[m_c4CurEnd], pszSrc, c4Count * kCIDLib::c4CharBytes);
    m_c4CurEnd += c4Count;
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
}


tCIDLib::TCh TString::TStrBuf::chAt(const tCIDLib::TCard4 c4Ind) const
{
    if (c4Ind >= m_c4CurEnd)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4Ind)
            , TString(L"TString")
            , TCardinal(m_c4CurEnd)
        );
    }
    return m_pszBuffer[c4Ind];
}


tCIDLib::TCh TString::TStrBuf::chLast() const noexcept
{
    if (m_c4CurEnd == 0)
        return kCIDLib::chNull;
    return m_pszBuffer[m_c4CurEnd - 1];
}


//
//  Just to be safe, we call this to calculate the difference in characters between
//  two places in our buffer. This insures that they are indeed both within our
//  buffer if in debug mode, just in case. With buffers being resized or faulted
//  in there's a danger we could sub from an old pointer or something. We also make
//  sure neither is null.
//
//  We have an inlined second that only takes an upper calls us with our own buffer
//  as the lower.
//
//  We don't do any calls that would use the current length and cap it, since this
//  may be called by functions that are putting new content in and haven't updated
//  the end yet.
//
tCIDLib::TCard4
TString::TStrBuf::c4CalcBufDiff(const   tCIDLib::TCh* const pszUpper
                                , const tCIDLib::TCh* const pszLower) const
{
    #if CID_DEBUG_ON
    const tCIDLib::TCh* const pszEnd = m_pszBuffer + m_c4BufSz;
    CIDAssert
    (
        (pszUpper != nullptr)
        && (pszLower != nullptr)
        && (pszUpper >= pszLower)
        && (pszUpper >= m_pszBuffer)
        && (pszUpper <= pszEnd)
        && (pszLower >= m_pszBuffer)
        && (pszLower <= pszEnd)
        , L"There is an issue with one or both string buffer pointers"
    );
    #endif
    return pszUpper - pszLower;
}


tCIDLib::TVoid TString::TStrBuf::Clear() noexcept
{
    m_c4CurEnd = 0;
    m_pszBuffer[0] = kCIDLib::chNull;
}


//
//  Copy in the source content without any leading or trailing space. This can be
//  an append or overwrite.
//
tCIDLib::TVoid TString::TStrBuf::CopyWithoutSpace(  const   TStringView&        strvSrc
                                                    , const tCIDLib::TBoolean   bAppend)
{
    // if not appending, then clear our contents no matter what
    if (!bAppend)
        Clear();

    // Find the start and end indicies
    tCIDLib::TCard4 c4Start = 0;
    tCIDLib::TCard4 c4End = 0;
    const tCIDLib::TBoolean bRes = TRawStr::bFindTextBody
    (
        strvSrc.pszBuffer(), c4Start, c4End, strvSrc.bHaveLength() ? strvSrc.c4Length() : kCIDLib::c4MaxCard
    );

    if (bRes)
    {
        // Make sure we have the needed capacity, keeping any previous content
        const tCIDLib::TCard4 c4AddedCnt = c4End - c4Start;
        ExpandBy(c4AddedCnt, kCIDLib::True);

        // We start out at the end of our content
        tCIDLib::TCh* pszTar = pszAtEnd();
        const tCIDLib::TCh* pszStart = strvSrc.pszBufferAt(c4Start);
        const tCIDLib::TCh* pszEnd = strvSrc.pszBufferAt(c4End);
        while (pszStart < pszEnd)
            *pszTar++ = *pszStart++;

        // Cap it off again
        *pszTar = kCIDLib::chNull;

        // And bump our current end
        m_c4CurEnd += c4AddedCnt;
    }
}


// Decrement  the end by the amount indicated
tCIDLib::TVoid TString::TStrBuf::DecEnd(const tCIDLib::TCard4 c4DecBy)
{
    if (c4DecBy > m_c4CurEnd)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexUnderFlow
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4DecBy)
        );
    }

    m_c4CurEnd -= c4DecBy;
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
}


// Delete the last character if we have one
tCIDLib::TVoid TString::TStrBuf::DeleteLast() noexcept
{
    // If already empty, then we are happy
    if (!m_c4CurEnd)
        return;

    // Just put a null in the last character and dec the length
    m_c4CurEnd--;
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
}



//
//  If the new size is larger than our current size, we'll expand. If we are already
//  out of small string world, we'll add an extra bit on relative to the current size.
//
//  If not preserving, then any existing content is dumped even if we don't reallocate.
//
tCIDLib::TVoid
TString::TStrBuf::ExpandTo(const tCIDLib::TCard4 c4NewSize, const tCIDLib::TBoolean bPreserve)
{
    // The new size cannot be less than the current end of the string if preserving
    if (bPreserve && (c4NewSize < m_c4CurEnd))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_Reallocate
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
            , facCIDLib().strMsg(kCIDMsgs::midStr_CantPreserve)
        );
    }

    // If not preserving, zero current size now to make the below simpler
    if (!bPreserve)
        m_c4CurEnd = 0;

    if (c4NewSize > m_c4BufSz)
    {
        tCIDLib::TCard4 c4RealNewSz = c4NewSize;
        if (c4RealNewSz > c4SmallBufSz)
            c4RealNewSz = c4NewSize + (c4NewSize >> 4);
        if (m_pszBuffer == m_szSmallBuf)
        {
            // We only need to do something if they go beyond the small buffer size
            if (c4RealNewSz > c4SmallBufSz)
            {
                // Allocate a new buffer. Copy over any old content if we have any
                m_pszBuffer = new tCIDLib::TCh[c4RealNewSz  + 1];
                if (m_c4CurEnd)
                    TRawMem::CopyMemBuf(m_pszBuffer, m_szSmallBuf, m_c4CurEnd * kCIDLib::c4CharBytes);

                m_szSmallBuf[0] = kCIDLib::chNull;
            }
        }
        else
        {
            // Expand the buffer and copy any old content
            TArrayJanitor<tCIDLib::TCh> janOld(m_pszBuffer);
            m_pszBuffer = new tCIDLib::TCh[c4RealNewSz + 1];
            if (m_c4CurEnd)
                TRawMem::CopyMemBuf(m_pszBuffer, janOld.paThis(), m_c4CurEnd * kCIDLib::c4CharBytes);
        }

        // Store the new size
        m_c4BufSz = c4RealNewSz;
    }

    // Make sure we are terminated. Even if no realloc, we may have changed the end
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
}

// Increment  the end by the amount indicated. It can go up to the current end
tCIDLib::TVoid TString::TStrBuf::IncEnd(const tCIDLib::TCard4 c4IncBy)
{
    CheckIndex(CID_FILE, CID_LINE, m_c4CurEnd + c4IncBy, kCIDLib::True);
    m_c4CurEnd += c4IncBy;
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
}


tCIDLib::TVoid
TString::TStrBuf::PutAt(const   tCIDLib::TCard4 c4Index
                        , const tCIDLib::TCh    chToPut)
{
    // We let them put it at the end
    CheckIndex(CID_FILE, CID_LINE, c4Index, kCIDLib::True);

    if (c4Index == m_c4CurEnd)
    {
        // They are putting it at the end, so just make it an append
        tCIDLib::TCh achTmp[2] = { chToPut, kCIDLib::chNull };
        Append(achTmp, 1);
    }
     else if (!chToPut)
    {
        // This is really a cap at operation
        m_c4CurEnd = c4Index;
        m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
    }
     else
    {
        // Stick the character in
        m_pszBuffer[c4Index] = chToPut;
    }
}


//
//  Called by the string class when it cuts out content in a way where it doesn't
//  have the new length, but knows it is null terminated. DON"T call anything here
//  that would use the currently set length of course!
//
tCIDLib::TVoid TString::TStrBuf::ResetEnd() noexcept
{
    m_c4CurEnd = TRawStr::c4StrLen(m_pszBuffer);
    CIDAssert(m_c4CurEnd <= m_c4BufSz, L"Null term len > buffer size");
}


// Called by the string class when it change the string end
tCIDLib::TVoid TString::TStrBuf::SetEnd(const tCIDLib::TCard4 c4At)
{
    //  We allow it to be the at the null
    CheckIndex(CID_FILE, CID_LINE, c4At, kCIDLib::True);
    m_c4CurEnd = c4At;
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
}

tCIDLib::TVoid TString::TStrBuf::SetEnd(const tCIDLib::TCh* const pszAt)
{
    m_c4CurEnd = c4CalcBufDiff(pszAt);
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
}

// A helper for the string class to add a termination at the current end
tCIDLib::TVoid TString::TStrBuf::Terminate()
{
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
}


// ---------------------------------------------------------------------------
//  TString::TStrBuf: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TString::TStrBuf::CheckIndex(const  tCIDLib::TCh* const pszFile
                            , const tCIDLib::TCard4     c4Line
                            , const tCIDLib::TCard4     c4ToCheck
                            , const tCIDLib::TBoolean   bEndOk) const
{
    const tCIDLib::TBoolean bOk
    (
        bEndOk ? (c4ToCheck <= m_c4BufSz) : (c4ToCheck < m_c4BufSz)
    );
    if (!bOk)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Index
            , TCardinal(c4ToCheck)
            , TString(L"TStrBuf")
            , TCardinal(m_c4BufSz)
        );
    }
}
