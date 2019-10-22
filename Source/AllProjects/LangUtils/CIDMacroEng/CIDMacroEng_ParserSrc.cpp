//
// FILE NAME: CIDMacroEng_ParserSrc.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/27/2003
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
//  This file implements some internal helper classes for the parser.
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
#include    "CIDMacroEng_.hpp"
#include    "CIDMacroEng_ParserSrc_.hpp"


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroEng_ParserSrc
{
    //
    //  The list of special chars that always, if not in a quoted string,
    //  will stop parsing of the current token, beceause these are always
    //  considered tokens themselves otherwise.
    //
    const tCIDLib::TCh aszSpecialChars[] =
    {
        kCIDLib::chAmpersand
        , kCIDLib::chApostrophe
        , kCIDLib::chAsterisk
        , kCIDLib::chCircumflex
        , kCIDLib::chCloseBracket
        , kCIDLib::chCloseParen
        , kCIDLib::chColon
        , kCIDLib::chComma
        , kCIDLib::chEquals
        , kCIDLib::chExclamation
        , kCIDLib::chForwardSlash
        , kCIDLib::chGreaterThan
        , kCIDLib::chHyphenMinus
        , kCIDLib::chLessThan
        , kCIDLib::chOpenBracket
        , kCIDLib::chOpenParen
        , kCIDLib::chPercentSign
        , kCIDLib::chPeriod
        , kCIDLib::chPlusSign
        , kCIDLib::chQuotation
        , kCIDLib::chSemiColon
        , kCIDLib::chVerticalBar
        , kCIDLib::chNull
    };

    // Used to do the one time calc of the hashes of the lookup table
    TAtomicFlag                 atomInitDone;

    // The modulus that we use for hashing the strings
    tCIDLib::TCard4             c4Modulus = 19;

    // We have an array of these items to do token matching
    struct TMapItem
    {
        const   tCIDLib::TCh*   pszName;
        tCIDLib::THashVal       hshName;
        tCIDMacroEng::ETokens   eToken;
    };

    //
    //  This maps to the ETokens enum value. We look up the string in here and,
    //  if found, cast that index to the enum value.
    //
    //  NOTE:   Keep this in sync with the enum of course!
    //
    //  Also note that, because of it's size, we put it in a separate header
    //  and include it here.
    //
    #include "CIDMacroEng_ParserTokenMap_.hpp"
}



// ---------------------------------------------------------------------------
//   CLASS: TParserSrc
//  PREFIX: psrc
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TParserSrc: Constructors and destructor
// ---------------------------------------------------------------------------
TParserSrc::TParserSrc(         TTextInStream* const    pstrmToAdopt
                        , const TString&                strClassPath
                        , const tCIDLib::TBoolean       bDebugMode) :

    m_bDebugMode(bDebugMode)
    , m_c4CondCount(0)
    , m_c4CurCol(1)
    , m_c4CurLine(1)
    , m_c4LastLen(0)
    , m_c4SaveCol(1)
    , m_c4SaveLine(1)
    , m_fcolCharStack(8192)
    , m_pstrmSrc(pstrmToAdopt)
    , m_strClassPath(strClassPath)
{
    //
    //  If we haven't initialized the hash table, do it. We have to calc the
    //  hashes for the strings in the map.
    //
    if (!CIDMacroEng_ParserSrc::atomInitDone)
    {
        TBaseLock lockInit;
        if (!CIDMacroEng_ParserSrc::atomInitDone)
        {
            tCIDLib::TInt4 i4Index = 0;
            for (; i4Index < tCIDLib::i4EnumOrd(tCIDMacroEng::ETokens::Count); i4Index++)
            {
                CIDMacroEng_ParserSrc::aitemMap[i4Index].hshName =
                (
                    TRawStr::hshHashStr
                    (
                        CIDMacroEng_ParserSrc::aitemMap[i4Index].pszName
                        , CIDMacroEng_ParserSrc::c4Modulus
                    )
                );
            }

            // And mark us done now
            CIDMacroEng_ParserSrc::atomInitDone.Set();
        }
    }
}

TParserSrc::~TParserSrc()
{
    delete m_pstrmSrc;
}


// ---------------------------------------------------------------------------
//  TParserSrc: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TParserSrc::bIsNameChar(const tCIDLib::TCh chToCheck) const
{
    return TRawStr::bIsAlphaNum(chToCheck) || (chToCheck == kCIDLib::chUnderscore);
}


tCIDLib::TBoolean
TParserSrc::bIsSpecialChar(const tCIDLib::TCh chToCheck) const
{
    return TRawStr::pszFindChar
    (
        CIDMacroEng_ParserSrc::aszSpecialChars
        , chToCheck
    ) != 0;
}


tCIDLib::TBoolean TParserSrc::bIfPeeked(const tCIDMacroEng::ETokens eToCheck)
{
    //
    //  Get the next token. If its the one indicated, then eat it and
    //  return true, else put it back return false.
    //
    TString strText;
    tCIDMacroEng::ETokens eTok = eGetNextToken(strText, kCIDLib::True);
    if (eToCheck != eTok)
    {
        PushBack(eTok, strText);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TCard4 TParserSrc::c4Col() const
{
    return m_c4SaveCol;
}


tCIDLib::TCard4 TParserSrc::c4Line() const
{
    return m_c4SaveLine;
}


//
//  Given a character literal token (either a single char or a \x type of
//  special character), we return the literal character.
//
tCIDLib::TCh TParserSrc::chEscape(const TString& strToEscape)
{
    const tCIDLib::TCard4 c4Len = strToEscape.c4Length();

    tCIDLib::TCh chRet = 0;
    if (c4Len == 1)
    {
        // It's just the one literal character
        chRet = strToEscape[0];
    }
     else if (c4Len == 2)
    {
        // Must have a leading slash
        if (strToEscape[0] != kCIDLib::chBackSlash)
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcPrs_BadCharLiteral
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
        }

        // And the next character must be one of the legal special characters
        chRet = strToEscape[1];

        switch(chRet)
        {
            case kCIDLib::chLatin_a :
                chRet = kCIDLib::chBell;
                break;

            case kCIDLib::chLatin_b :
                chRet = kCIDLib::chBS;
                break;

            case kCIDLib::chLatin_f :
                chRet = kCIDLib::chFF;
                break;

            case kCIDLib::chLatin_n :
                chRet = kCIDLib::chLF;
                break;

            case kCIDLib::chApostrophe :
                // It's an escaped apostrophe
                chRet = kCIDLib::chApostrophe;
                break;

            case kCIDLib::chLatin_r :
                chRet = kCIDLib::chCR;
                break;

            case kCIDLib::chLatin_t :
                chRet = kCIDLib::chTab;
                break;

            case kCIDLib::chLatin_v :
                chRet = kCIDLib::chVTab;
                break;

            case kCIDLib::chBackSlash :
                chRet = kCIDLib::chBackSlash;
                break;

            default :
                // It's not a valid escape character
                facCIDMacroEng().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kMEngErrs::errcPrs_BadCharLiteral
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Internal
                );
                break;
        };
    }
     else if (c4Len == 6)
    {
        //
        //  It's got to be a character literal in the '\x0000' type of
        //  form. So we skip the first two chars and convert the four
        //  trailing chars into a hex number, and cast that to a char.
        //
        tCIDLib::TBoolean bValid;
        const tCIDLib::TCard4 c4Ordinal = TRawStr::c4AsBinary
        (
            strToEscape.pszBufferAt(2)
            , bValid
            , tCIDLib::ERadices::Hex
        );

        if (!bValid)
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcPrs_BadCharOrdinal
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Format
            );
        }

        chRet = tCIDLib::TCh(c4Ordinal);
    }
     else
    {
        // This shouldn't happen
        facCIDMacroEng().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kMEngErrs::errcPrs_BadCharLiteral
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    return chRet;
}


//
//  Second most primitve char spooling methods or get or peek the next
//  available character. At this level, we handle new line processing
//  and current position updating.
//
tCIDLib::TCh TParserSrc::chGetNextChar()
{
    // Spool in the next char available
    tCIDLib::TCh chRet = chSpoolAChar();

    //
    //  We convert all newline combinations to a single LF, andwe have to
    //  bump the line number and reset the column.
    //
    if ((chRet == kCIDLib::chCR) || (chRet == kCIDLib::chLF))
    {
        //
        //  If a CR, and the next char is LF, then eat the CR and return LF.
        //  Else, just replace it with LF.
        //
        if (chRet == kCIDLib::chCR)
        {
            if (chPeekNextChar() == kCIDLib::chLF)
                chRet = chSpoolAChar();
            else
                chRet = kCIDLib::chLF;
        }

        //
        //  Move us to the start of the next line, position-wise. And we need
        //  to save the current column in case of an unget of this newline.
        //
        m_c4LastLen = m_c4CurCol;
        m_c4CurLine++;
        m_c4CurCol = 1;
    }
     else
    {
        // Must another char on the current line
        m_c4CurCol++;
    }
    return chRet;
}


tCIDLib::TCh TParserSrc::chPeekNextChar()
{
    //
    //  If we have something on the pushback stack, just peek at the top
    //  and return that. Else we have to go to the stream. If its at the
    //  end, then return null (we check because we never want to throw an
    //  end of stream from a peek), else read the next char and then
    //  immediately push it back (which doesn't affect current position.)
    //
    tCIDLib::TCh chRet = kCIDLib::chNull;
    if (!m_fcolCharStack.bIsEmpty())
    {
        chRet = m_fcolCharStack.tPeek();
    }
     else
    {
        if (m_pstrmSrc->bEndOfStream())
        {
            chRet = kCIDLib::chNull;
        }
         else
        {
            chRet = m_pstrmSrc->chRead();
            m_fcolCharStack.Push(chRet);
        }
    }
    return chRet;
}


tCIDMacroEng::ETokens
TParserSrc::eGetNextToken(          TString&            strToFill
                            , const tCIDLib::TBoolean   bEOFOk)
{
    tCIDMacroEng::ETokens eRet = eExtractToken(strToFill, bEOFOk);

    //
    //  If we get a begin debug and we are not in debug mode, then we have
    //  to enter a loop until we see the end debug. If we hit the EOF,
    //  that's obviously an error while in this loop.
    //
    //  If we aren't in debug mode, we just ignore it and get the next
    //  available token, and bump the conditional nesting counter, so
    //  we'll know when we hit the matching end debug.
    //
    if (eRet == tCIDMacroEng::ETokens::BeginDebug)
    {
        if (m_bDebugMode)
        {
            m_c4CondCount++;
            eRet = eProcessBeginDebug(strToFill, bEOFOk);
        }
         else
        {
            // While we see begin debug blocks, keep eating them
            while (eRet == tCIDMacroEng::ETokens::BeginDebug)
            {
                m_c4CondCount++;

                // Loop until we get back out of this conditional block
                while (m_c4CondCount)
                {
                    eRet = eExtractToken(strToFill, kCIDLib::False);

                    if (eRet == tCIDMacroEng::ETokens::BeginDebug)
                    {
                        // We've entered another nested block
                        m_c4CondCount++;
                    }
                     else if (eRet == tCIDMacroEng::ETokens::EndDebug)
                    {
                        // We've come out of a block
                        m_c4CondCount--;
                        if (!m_c4CondCount)
                            break;
                    }
                }

                //
                //  Now get another one. If it's another begin debug, we'll
                //  stay in the loop and go again, else we return with this
                //  one. Else, we'll fall out with something to return.
                //
                eRet = eExtractToken(strToFill, bEOFOk);
            }
        }
    }
     else
    {
        while (eRet == tCIDMacroEng::ETokens::EndDebug)
        {
            // If already zeroed on the depth count, we underflowed
            if (!m_c4CondCount)
            {
                facCIDMacroEng().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kMEngErrs::errcPrs_CondUnderflow
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Internal
                );
            }
            m_c4CondCount--;
            eRet = eExtractToken(strToFill, bEOFOk);
        }

        // If it should be a begin debug, then process that
        if (eRet == tCIDMacroEng::ETokens::BeginDebug)
        {
            m_c4CondCount++;
            eRet = eProcessBeginDebug(strToFill, bEOFOk);
        }
    }

    // And return the next token
    return eRet;
}


tCIDMacroEng::ETokens TParserSrc::eGetNextToken(const tCIDLib::TBoolean bEOFOk)
{
    return eExtractToken(m_strTmp, bEOFOk);
}


tCIDMacroEng::ETokens
TParserSrc::eMapToken(const tCIDLib::TCh* const pszToMap) const
{
    // Hash the passed string
    const tCIDLib::THashVal hshFind = TRawStr::hshHashStr
    (
        pszToMap
        , CIDMacroEng_ParserSrc::c4Modulus
    );

    //
    //  Look through the map. If we get a hash match, then check the whole
    //  string.
    //
    tCIDMacroEng::ETokens  eRet = tCIDMacroEng::ETokens::NoMatch;
    tCIDLib::TInt4      i4Index = 0;
    for (; i4Index < tCIDLib::i4EnumOrd(tCIDMacroEng::ETokens::Count); i4Index++)
    {
        if (hshFind != CIDMacroEng_ParserSrc::aitemMap[i4Index].hshName)
            continue;

        const tCIDLib::TCh* pszCur = CIDMacroEng_ParserSrc::aitemMap[i4Index].pszName;
        if (TRawStr::bCompareStr(pszCur, pszToMap))
        {
            eRet = CIDMacroEng_ParserSrc::aitemMap[i4Index].eToken;
            break;
        }
    }
    return eRet;
}


//
//  Peak the next token in the input. We have one that just returns the
//  token type and one that returns the type and the text.
//
tCIDMacroEng::ETokens TParserSrc::ePeekNextToken()
{
    //
    //  Get the next token, then push the text back onto the pushback stack
    //  so it's like we didn't read it.
    //
    tCIDMacroEng::ETokens eNext = eGetNextToken(m_strTmp, kCIDLib::True);
    PushBack(eNext, m_strTmp);
    return eNext;
}

tCIDMacroEng::ETokens TParserSrc::ePeekNextToken(TString& strText)
{
    //
    //  Get the next token, then push the text back onto the pushback stack
    //  so it's like we didn't read it.
    //
    tCIDMacroEng::ETokens eNext = eGetNextToken(strText, kCIDLib::True);
    PushBack(eNext, strText);
    return eNext;
}


// Eat the rest of the current line
tCIDLib::TVoid TParserSrc::EatLineRemainder()
{
    // Eat until we it a new line or the end of the input
    while (kCIDLib::True)
    {
        if ((chGetNextChar() == kCIDLib::chLF) || m_pstrmSrc->bEndOfStream())
            break;
    }
}


// Eat any whitespace starting at the current position
tCIDLib::TVoid TParserSrc::EatWhitespace()
{
    tCIDLib::TCh chCur;
    while (kCIDLib::True)
    {
        chCur = chGetNextChar();
        while (TRawStr::bIsSpace(chCur)
        ||     (chCur == kCIDLib::chLF)
        ||     (chCur == kCIDLib::chLF)
        ||     (chCur == kCIDLib::chTab))
        {
            chCur = chGetNextChar();
        }

        //
        //  Before we push the break out char back, see if its a forward
        //  slash character. If so, check the next char. If its one too,
        //  then this is a quote and we want to eat it and try again.
        //
        if ((chCur == kCIDLib::chForwardSlash)
        &&  (chPeekNextChar() == kCIDLib::chForwardSlash))
        {
            EatLineRemainder();
        }
         else
        {
            // Looks like something we want, so push it back and break out
            PushBack(chCur);

            //
            //  In this case, we want to pull the save line/col up to
            //  the current line/col.
            //
            SavePos();
            break;
        }
    }
}


//
//  Process any escape characters in the passed string. Leaving it
//  in final processed form.
//
//  NOTE: We do such escaping after the fact (instead of in the low level
//        token extractor method), because if the token ends up being pushed
//        back, the caller must have the original text to push back, not
//        the escaped version. So it's not as efficient, but it's neeeded
//        to support push back. Without it, the line and column positions
//        would be wrong after reparsing a pushed back token.
//
tCIDLib::TVoid TParserSrc::EscapeStr(TString& strToEscape)
{
    const tCIDLib::TCard4 c4Len = strToEscape.c4Length();

    // If empty, the we are done
    if (!c4Len)
        return;

    tCIDLib::TBoolean bOweEscape = kCIDLib::False;
    tCIDLib::TCard4 c4SrcIndex = 0;

    //
    //  Do a preliminary scan for backslashes. If we find nne, then
    //  we are done and don't have to do anymore. If we do, then start
    //  our indices there. This will much more efficient in the bulk of
    //  cases.
    //
    if (!strToEscape.bFirstOccurrence(kCIDLib::chBackSlash, c4SrcIndex))
        return;

    // Start the target index at the same point
    tCIDLib::TCard4 c4TarIndex = c4SrcIndex;
    while (c4SrcIndex < c4Len)
    {
        tCIDLib::TCh chCur = strToEscape[c4SrcIndex];

        if (bOweEscape)
        {
            //
            //  We owe an escape. So this char must be one of the legal
            //  escape chars.
            //
            switch(chCur)
            {
                case kCIDLib::chLatin_a :
                    chCur = kCIDLib::chBell;
                    break;

                case kCIDLib::chLatin_b :
                    chCur = kCIDLib::chBS;
                    break;

                case kCIDLib::chLatin_f :
                    chCur = kCIDLib::chFF;
                    break;

                case kCIDLib::chLatin_n :
                    chCur = kCIDLib::chLF;
                    break;

                case kCIDLib::chQuotation :
                    // It's an escaped quote
                    chCur = kCIDLib::chQuotation;
                    break;

                case kCIDLib::chLatin_r :
                    chCur = kCIDLib::chCR;
                    break;

                case kCIDLib::chLatin_t :
                    chCur = kCIDLib::chTab;
                    break;

                case kCIDLib::chLatin_v :
                    chCur = kCIDLib::chVTab;
                    break;

                case kCIDLib::chBackSlash :
                    chCur = kCIDLib::chBackSlash;
                    break;

                default :
                    // It's not a valid escape char, so put both back in
                    bOweEscape = kCIDLib::False;
                    break;
            };

            //
            //  If the owe flag got cleared, then it wasn't a real
            //  escape, so we owe both. Else we jsut own the created
            //  character.
            //
            if (bOweEscape)
            {
                strToEscape.PutAt(c4TarIndex++, chCur);
                bOweEscape = kCIDLib::False;
            }
             else
            {
                strToEscape.PutAt(c4TarIndex++, kCIDLib::chBackSlash);
                strToEscape.PutAt(c4TarIndex++, chCur);
            }
        }
         else
        {
            if (chCur == kCIDLib::chBackSlash)
            {
                // Remember we own an escape
                bOweEscape = kCIDLib::True;
            }
             else
            {
                if (c4SrcIndex != c4TarIndex)
                    strToEscape.PutAt(c4TarIndex, strToEscape[c4SrcIndex]);
                c4TarIndex++;
            }
        }

        // Bump the src for the next round
        c4SrcIndex++;
    }

    //
    //  If we get here and own an escape, then there was a trailing
    //  slash, so put it on.
    //
    if (bOweEscape)
        strToEscape.PutAt(c4TarIndex++, kCIDLib::chBackSlash);

    // And cap us on the target index
    strToEscape.CapAt(c4TarIndex);
}


// Eat the remainder of the current line and return the result
tCIDLib::TVoid TParserSrc::GetLineRemainder(TString& strToFill)
{
    // Collect until a new line or EOF
    strToFill.Clear();
    while (!m_pstrmSrc->bEndOfStream())
    {
        const tCIDLib::TCh chCur = chGetNextChar();
        if (chCur == kCIDLib::chLF)
            break;
        strToFill.Append(chCur);
    }
    SavePos();
}


// Push a single character back into the pushback stack
tCIDLib::TVoid TParserSrc::PushBack(const tCIDLib::TCh chToPush)
{
    //
    //  If they push back a newline, then we have to move back to the end of
    //  the previous line. We store the previous line end when the new line
    //  is read out. If that previous line end is zero, then they've tried to
    //  push back past two lines, which isn't allowed. Also, if we are already
    //  on the first line, that can't be right.
    //
    if (chToPush == kCIDLib::chLF)
    {
        if (!m_c4LastLen || (m_c4CurLine == 1))
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcPrs_PushBackNL
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
        }

        //
        //  Move back to the previous line and reset the column and then clear
        //  the last line length because we've used it.
        //
        m_c4CurLine--;
        m_c4CurCol = m_c4LastLen;
        m_c4LastLen = 0;
    }
     else
    {
        m_c4CurCol--;
    }
    m_fcolCharStack.Push(chToPush);
}


//
//  Push a token back onto the pushback stack. They have to tell the type
//  of token since that matters how we push it back. We push the characters
//  backwards, so that if they are read out again, they will be read in the
//  correct order.
//
tCIDLib::TVoid TParserSrc::PushBack(const   tCIDMacroEng::ETokens  eTypeToPush
                                    ,       TString&            strToPush)
{
    //
    //  If the type is a quoted string, then put quotes around it. If a char
    //  literal, put the single quotes.
    //
    if (eTypeToPush == tCIDMacroEng::ETokens::CharLiteral)
    {
        strToPush.Insert(kCIDLib::chApostrophe, 0);
        strToPush.Append(kCIDLib::chApostrophe);
    }
     else if (eTypeToPush == tCIDMacroEng::ETokens::QuotedString)
    {
        strToPush.Insert(kCIDLib::chQuotation, 0);
        strToPush.Append(kCIDLib::chQuotation);
    }

    // We have to push them backwards, so start at the end
    tCIDLib::TCard4 c4Index = strToPush.c4Length();
    if (c4Index)
    {
        do
        {
            c4Index--;
            m_fcolCharStack.Push(strToPush[c4Index]);

            //
            //  We don't allow a push back past the start of a line in this mode
            //  since no token text should ever have a new line, so if we hit
            //  column 1, that's an error.
            //
            if (!m_c4CurCol)
            {
                facCIDMacroEng().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kMEngErrs::errcPrs_PushBackNL
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Internal
                );
            }
            m_c4CurCol--;
        }   while (c4Index > 0);
    }

    // And again, if a char or string literal, push the closing quote
    if (eTypeToPush == tCIDMacroEng::ETokens::CharLiteral)
    {
        strToPush.Insert(kCIDLib::chApostrophe, 0);
        strToPush.Append(kCIDLib::chApostrophe);
    }
     else if (eTypeToPush == tCIDMacroEng::ETokens::QuotedString)
    {
        strToPush.Insert(kCIDLib::chQuotation, 0);
        strToPush.Append(kCIDLib::chQuotation);
    }
}


//
//  Push back a string of text. It is assumed here that the text has not
//  been massaged in any way and doesn't include any quoted string or
//  char values since those will have been escaped. Use the version above
//  for that type of thing.
//
tCIDLib::TVoid TParserSrc::PushBack(const TString& strToPush)
{
    if (strToPush.bIsEmpty())
        return;

    // We have to push them backwards, so start at the end
    tCIDLib::TCard4 c4Index = strToPush.c4Length();
    do
    {
        //
        //  We don't allow a push back past the start of a line in this mode
        //  since no token text should ever have a new line, so if we hit
        //  column 1, that's an error.
        //
        if (m_c4CurCol == 1)
        {
            facCIDMacroEng().ThrowErr
            (
                CID_FILE
                , CID_LINE
                , kMEngErrs::errcPrs_PushBackNL
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::Internal
            );
        }
        c4Index--;
        m_fcolCharStack.Push(strToPush[c4Index]);
        m_c4CurCol--;

    }   while (c4Index > 0);
}


// Return the class path of the class we are parsing
const TString& TParserSrc::strPath() const
{
    return m_strClassPath;
}


// Returns the text for the indicated type of token
TString TParserSrc::strTokenText(const tCIDMacroEng::ETokens eToken)
{
    return TString(CIDMacroEng_ParserSrc::aitemMap[tCIDLib::c4EnumOrd(eToken)].pszName);
}


// Save the current line/col info so that it can be gotten back to
tCIDLib::TVoid TParserSrc::SavePos()
{
    m_c4SaveCol  = m_c4CurCol;
    m_c4SaveLine = m_c4CurLine;
}



// ---------------------------------------------------------------------------
//  TParserSrc: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  The most primitive char spooling method. Everything gets sucked through
//  here, and we just handle pulling out of the pushback stack first, then
//  going to the stream.
//
tCIDLib::TCh TParserSrc::chSpoolAChar()
{
    //
    //  First see if there are chars on the pushback stack. If so, use those.
    //  Else, get a new one from the stream.
    //
    tCIDLib::TCh chRet = kCIDLib::chNull;
    if (!m_fcolCharStack.bIsEmpty())
        chRet = m_fcolCharStack.tPop();
    else
        chRet = m_pstrmSrc->chRead();
    return chRet;
}


tCIDMacroEng::ETokens
TParserSrc::eExtractToken(TString& strText, const tCIDLib::TBoolean bEOFOk)
{
    tCIDLib::TBoolean       bInEscape = kCIDLib::False;
    tCIDLib::TBoolean       bQuoted = kCIDLib::False;
    tCIDMacroEng::ETokens      eRet = tCIDMacroEng::ETokens::NoMatch;
    tCIDLib::TCh            chCur;
    try
    {
        // Empty the string
        strText.Clear();

        // Skip any white space. We eat whitespace and don't report it
        while (kCIDLib::True)
        {
            chCur = chGetNextChar();
            while (TRawStr::bIsSpace(chCur))
                chCur = chGetNextChar();

            //
            //  Before we push the break out char back, see if its a forward
            //  slash character. If so, check the next char. If its one too,
            //  then this is a comment and we want to eat it and try again.
            //
            if ((chCur == kCIDLib::chForwardSlash)
            &&  (chPeekNextChar() == kCIDLib::chForwardSlash))
            {
                EatLineRemainder();
            }
             else
            {
                // Looks like something we want, so push it back and break out
                PushBack(chCur);
                break;
            }
        }

        // Set the current position as the save position
        SavePos();

        //
        //  We can handle one easy special case up front, which will also
        //  simplify the code below. If we see a single quote, then it has
        //  to be a char literal.
        //
        if (chPeekNextChar() == kCIDLib::chApostrophe)
        {
            // Eat the apostrophe and get the next char
            chGetNextChar();
            chCur = chGetNextChar();

            //
            //  If it was a slash, then it's either an escaped character
            //  or a hex number that represents the ordinal of the
            //  character (starting with an x.)
            //
            if (chCur == kCIDLib::chBackSlash)
            {
                tCIDLib::TCh chTmp = chGetNextChar();
                if (chTmp == kCIDLib::chLatin_x)
                {
                    //
                    //  It must be four digit hex value. We just get
                    //  them blindly. If they don't have enough chars,
                    //  then we'll fail because one of them one be a
                    //  digit. If they have too many, we'll fail below
                    //  because we won't end on the closeing apostrophe.
                    //
                    strText = L"\\x";
                    strText.Append(chGetNextChar());
                    strText.Append(chGetNextChar());
                    strText.Append(chGetNextChar());
                    strText.Append(chGetNextChar());
                }
                 else
                {
                    strText.Append(kCIDLib::chBackSlash);
                    strText.Append(chTmp);
                }
            }
             else
            {
                // Nothing special, just store the char
                strText.Append(chCur);
            }

            // And the next must be the closing apostophe
            if (chGetNextChar() != kCIDLib::chApostrophe)
            {
                facCIDMacroEng().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kMEngErrs::errcPrs_BadCharLiteral
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            }
            return tCIDMacroEng::ETokens::CharLiteral;
        }

        //
        //  First peek the next char and see if its a quote. If so, then we
        //  are in a quoted string and will ignore special chars other than
        //  the end quote. If it's a quote, then eat the character after we
        //  set the flag.
        //
        bQuoted = (chPeekNextChar() == kCIDLib::chQuotation);
        if (bQuoted)
            chGetNextChar();

        //
        //  Ok, we now read chars until we hit one that forces a token. This
        //  can be a whitespace, or one of the special chars such as equal
        //  sign, semi-colon, etc..., or the ending quote if in a quoted
        //  string.
        //
        while (kCIDLib::True)
        {
            // Get the next char
            chCur = chGetNextChar();

            if (bQuoted)
            {
                //
                //  We cannot see a CR or LF in a string unless we are in
                //  escape mode, which means the last character was a slash.
                //  In that case, we will eat any whitespace and then pick
                //  up again from there (throwing away the trailing slash
                //  and exiting escape mode.)
                //
                if ((chCur == kCIDLib::chCR) || (chCur == kCIDLib::chLF))
                {
                    if (!bInEscape)
                    {
                        facCIDMacroEng().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kMEngErrs::errcPrs_NewlineInString
                            , tCIDLib::ESeverities::Failed
                            , tCIDLib::EErrClasses::Format
                        );
                    }

                    bInEscape = kCIDLib::False;
                    EatWhitespace();
                    strText.DeleteLast();
                    continue;
                }

                //
                //  If we see a quote, and we are not in escape mode,
                //  then it has to be the end of the quoted string.
                //
                if (chCur == kCIDLib::chQuotation)
                {
                    if (!bInEscape)
                    {
                        eRet = tCIDMacroEng::ETokens::QuotedString;
                        break;
                    }
                }

                //
                //  DO THIS AFTER any checks that depend on whether we
                //  are in escape mode or not.
                //
                if (bInEscape)
                {
                    //
                    //  We've now eaten the escaped character that was
                    //  escaped by the previously seen slash, so toggle
                    //  out of escape mode.
                    //
                    bInEscape = kCIDLib::False;
                }
                 else if (chCur == kCIDLib::chBackSlash)
                {
                    // We need to enter escape mode
                    bInEscape = kCIDLib::True;
                }

                // Just add it to the quoted text since we aren't done yet
                strText.Append(chCur);
            }
             else if (TRawStr::bIsSpace(chCur))
            {
                //
                //  Break out if we hit a whitespace, since its not in a
                //  quoted string at this point. But push it back since
                //  if there is only one space between two tokens, it would
                //  make them look like one.
                //
                PushBack(chCur);
                break;
            }
             else
            {
                //
                //  Check for a special char. If we get one, and this is the
                //  first char of this token, then it is the token itself. Else,
                //  it terminates the token and we push it back. Either way,
                //  we break out.
                //
                if (bIsSpecialChar(chCur))
                {
                    tCIDLib::TBoolean bBreakOut = kCIDLib::True;

                    if (strText.bIsEmpty())
                    {
                        switch(chCur)
                        {
                            case kCIDLib::chAmpersand :
                                eRet = tCIDMacroEng::ETokens::And;
                                break;

                            case kCIDLib::chAsterisk :
                                eRet = tCIDMacroEng::ETokens::Multiply;
                                break;

                            case kCIDLib::chCloseBracket :
                                eRet = tCIDMacroEng::ETokens::CloseBracket;
                                break;

                            case kCIDLib::chCircumflex :
                                eRet = tCIDMacroEng::ETokens::Xor;
                                break;

                            case kCIDLib::chCloseParen :
                                eRet = tCIDMacroEng::ETokens::CloseParen;
                                break;

                            case kCIDLib::chColon :
                                eRet = tCIDMacroEng::ETokens::Colon;
                                break;

                            case kCIDLib::chComma :
                                eRet = tCIDMacroEng::ETokens::Comma;
                                break;

                            case kCIDLib::chEquals :
                                eRet = tCIDMacroEng::ETokens::EqualSign;
                                break;

                            case kCIDLib::chExclamation :
                                eRet = tCIDMacroEng::ETokens::Exclaim;
                                break;

                            case kCIDLib::chForwardSlash :
                                eRet = tCIDMacroEng::ETokens::Divide;
                                break;

                            case kCIDLib::chGreaterThan :
                                eRet = tCIDMacroEng::ETokens::GtThan;
                                break;

                            case kCIDLib::chHyphenMinus :
                                //
                                //  Could be a numeric literal, so see if the
                                //  next char is a digit. If so, don't break
                                //  out.
                                //
                                if (TRawStr::bIsDigit(chPeekNextChar()))
                                    bBreakOut = kCIDLib::False;
                                else
                                    eRet = tCIDMacroEng::ETokens::Subtract;
                                break;

                            case kCIDLib::chLessThan :
                                eRet = tCIDMacroEng::ETokens::LsThan;
                                break;

                            case kCIDLib::chOpenBracket :
                                eRet = tCIDMacroEng::ETokens::OpenBracket;
                                break;

                            case kCIDLib::chOpenParen :
                                eRet = tCIDMacroEng::ETokens::OpenParen;
                                break;

                            case kCIDLib::chPercentSign :
                                eRet = tCIDMacroEng::ETokens::ModDiv;
                                break;

                            case kCIDLib::chPeriod :
                                eRet = tCIDMacroEng::ETokens::Period;
                                break;

                            case kCIDLib::chPlusSign :
                                //
                                //  Could be a numeric literal, so see if the
                                //  next char is a digit. If so, don't break
                                //  out.
                                //
                                if (TRawStr::bIsDigit(chPeekNextChar()))
                                    bBreakOut = kCIDLib::False;
                                else
                                    eRet = tCIDMacroEng::ETokens::Add;
                                break;

                            case kCIDLib::chQuotation :
                                // A quote in the middle of something else
                                facCIDMacroEng().ThrowErr
                                (
                                    CID_FILE
                                    , CID_LINE
                                    , kMEngErrs::errcPrs_BadQuoteHere
                                    , tCIDLib::ESeverities::Failed
                                    , tCIDLib::EErrClasses::Format
                                );
                                break;

                            case kCIDLib::chSemiColon :
                                eRet = tCIDMacroEng::ETokens::SemiColon;
                                break;

                            case kCIDLib::chVerticalBar :
                                eRet = tCIDMacroEng::ETokens::Or;
                                break;

                            default :
                            {
                                facCIDMacroEng().ThrowErr
                                (
                                    CID_FILE
                                    , CID_LINE
                                    , kMEngErrs::errcDbg_UnhandledSpecChar
                                    , tCIDLib::ESeverities::Failed
                                    , tCIDLib::EErrClasses::Internal
                                    , TCardinal(tCIDLib::TCard4(chCur), tCIDLib::ERadices::Hex)
                                );
                                break;
                            }
                        };
                        strText.Append(chCur);
                    }
                     else
                    {
                        //
                        //  If the first char is a hyphen minus or digit or
                        //  plus sign, assume a floating point and don't
                        //  stop.
                        //
                        if ((chCur == kCIDLib::chPeriod)
                        &&  ((strText.chFirst() == kCIDLib::chPlusSign)
                        ||   (strText.chFirst() == kCIDLib::chHyphenMinus)
                        ||   TRawStr::bIsDigit(strText.chFirst())))
                        {
                            bBreakOut = kCIDLib::False;
                            strText.Append(chCur);
                        }
                         else
                        {
                            PushBack(chCur);
                        }
                    }

                    // And break out out of the loop, if told to
                    if (bBreakOut)
                        break;
                }
                 else
                {
                    strText.Append(chCur);
                }
            }
        }
    }

    catch(const TError& errToCatch)
    {
        //
        //  If its an EOF, then see if we can legally get one here. If not,
        //  or its some other error, then just throw. Even if they say EOF
        //  is ok, if we were in a quoted string, then this is not legal, so
        //  check that also.
        //
        const tCIDLib::TBoolean bEOF = errToCatch.bCheckEvent
        (
            facCIDLib().strName()
            , kCIDErrs::errcStrm_EndOfStream
        );

        if (bEOF)
        {
            // Not legal to see EOF in a conditional block
            if (m_c4CondCount)
            {
                facCIDMacroEng().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kMEngErrs::errcPrs_EOFInConditional
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            }

            // Or in a quoted string
            if (bQuoted)
            {
                facCIDMacroEng().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kMEngErrs::errcPrs_UnterminatedStr
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            };

            // If if the caller says it's not
            if (!bEOFOk)
            {
                facCIDMacroEng().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kMEngErrs::errcPrs_UnexpectedEOF
                    , tCIDLib::ESeverities::Failed
                    , tCIDLib::EErrClasses::Format
                );
            };
        }
         else
        {
            // Just rethrow original
            throw;
        }
    }

    //
    //  If it wasn't a special case one, then we need to try to match it
    //  using the lookup table.
    //
    if (eRet == tCIDMacroEng::ETokens::NoMatch)
    {
        if (strText.bIsEmpty())
            eRet = tCIDMacroEng::ETokens::EOF;
        else
            eRet = eMapToken(strText.pszBuffer());

        //
        //  If still no match, see if it looks like a legal numeric
        //  constant.
        //
        if (eRet == tCIDMacroEng::ETokens::NoMatch)
        {
            //
            //  In order to be a numeric constant. The first character must
            //  be a minus sign, or a decimal digit. So we can do a quick
            //  check that will prevent us from wasting time trying to convert
            //  many name tokens.
            //
            const tCIDLib::TCh chFirst = strText.chFirst();
            if ((chFirst == kCIDLib::chHyphenMinus)
            ||  (chFirst == kCIDLib::chPlusSign)
            ||  TRawStr::bIsDigit(chFirst))
            {
                eRet = tCIDMacroEng::ETokens::NumericLiteral;
            }
        }
    }
     else
    {
        //
        //  There are a few ambiguous symbolic tokens, so we need to look at
        //  the next character to see if it could be a different token.
        //
        chCur = chPeekNextChar();
        tCIDMacroEng::ETokens eOrg = eRet;
        switch(eRet)
        {
            case tCIDMacroEng::ETokens::And :
            {
                if (chCur == kCIDLib::chAmpersand)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::LogAnd;
                }
                 else if (chCur == kCIDLib::chEquals)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::AndEq;
                }
                break;
            }

            case tCIDMacroEng::ETokens::Add :
            {
                if (chCur == kCIDLib::chPlusSign)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::Inc;
                }
                 else if (chCur == kCIDLib::chEquals)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::PlusEq;
                }
                break;
            }

            case tCIDMacroEng::ETokens::Colon :
            {
                if (chCur == kCIDLib::chEquals)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::Assign;
                }
                break;
            }

            case tCIDMacroEng::ETokens::Divide :
            {
                if (chCur == kCIDLib::chEquals)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::DivideEq;
                }
                break;
            }

            case tCIDMacroEng::ETokens::Exclaim :
            {
                if (chCur == kCIDLib::chEquals)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::NotEqual;
                }
                break;
            }

            case tCIDMacroEng::ETokens::GtThan :
            {
                if (chCur == kCIDLib::chEquals)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::GtThanEq;
                }
                break;
            }

            case tCIDMacroEng::ETokens::LsThan :
            {
                if (chCur == kCIDLib::chEquals)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::LsThanEq;
                }
                break;
            }

            case tCIDMacroEng::ETokens::ModDiv :
            {
                if (chCur == kCIDLib::chEquals)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::ModDivEq;
                }
                break;
            }

            case tCIDMacroEng::ETokens::Multiply :
            {
                if (chCur == kCIDLib::chEquals)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::MultiplyEq;
                }
                break;
            }

            case tCIDMacroEng::ETokens::Or :
            {
                if (chCur == kCIDLib::chVerticalBar)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::LogOr;
                }
                 else if (chCur == kCIDLib::chEquals)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::OrEq;
                }
                break;
            }

            case tCIDMacroEng::ETokens::Subtract :
            {
                if (chCur == kCIDLib::chHyphenMinus)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::Dec;
                }
                 else if (chCur == kCIDLib::chEquals)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::MinusEq;
                }
                break;
            }

            case tCIDMacroEng::ETokens::Xor :
            {
                if (chCur == kCIDLib::chCircumflex)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::LogXor;
                }
                 else if (chCur == kCIDLib::chEquals)
                {
                    strText.Append(chCur);
                    eRet = tCIDMacroEng::ETokens::XorEq;
                }
                break;
            }

            default :
                break;
        };

        // If we changed return, then we need to eat another char
        if (eRet != eOrg)
            chGetNextChar();
    }
    return eRet;
}


tCIDMacroEng::ETokens
TParserSrc::eProcessBeginDebug(         TString&            strToFill
                                , const tCIDLib::TBoolean   bEOFOk)
{
    // While we see begin debug blocks, keep eating them
    tCIDMacroEng::ETokens eRet = tCIDMacroEng::ETokens::BeginDebug;
    while (eRet == tCIDMacroEng::ETokens::BeginDebug)
    {
        //
        //  Loop until we either get ourselves back out of the
        //  conditional, which means it was empty basically, or
        //  we get something besides conditionals.
        //
        while (m_c4CondCount)
        {
            eRet = eExtractToken(strToFill, bEOFOk);

            //
            //  For each nested begin, bump the nesting count, and
            //  for each end, reduce it. If we get back to zero, then
            //  it was just one or more empty conditionals.
            //
            if (eRet == tCIDMacroEng::ETokens::BeginDebug)
                m_c4CondCount++;
            else if (eRet == tCIDMacroEng::ETokens::EndDebug)
                m_c4CondCount--;
            else
                break;
        }

        //
        //  If we broke out because the debug block was empty, then
        //  get another token. Else, we broke out because we got
        //  something to return already.
        //
        if (!m_c4CondCount)
            eRet = eExtractToken(strToFill, bEOFOk);
    }
    return eRet;
}

