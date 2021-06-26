//
// FILE NAME: CIDLib_StringTokenizer.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/23/1998
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
//  This file implements the TStringTokenizer class.
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
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TStringTokenizer,TObject)


namespace
{
    namespace CIDLib_StringTokenizer
    {
        // -----------------------------------------------------------------------
        //  Local const data
        //
        //  c4MaxTokenLen
        //      The maximum length that we can return as a token. This should way
        //      more than sufficient.
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard4    c4MaxTokenLen = kCIDLib::c4Sz_64K;
    }
}


// ---------------------------------------------------------------------------
//   CLASS: TStringTokenizer
//  PREFIX: stok
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//  TStringTokenizer: Public, static methods
// ---------------------------------------------------------------------------

//
//  This method will break out a comma separated list of quoted values into
//  a collection of strings. The values cannot have double quotes inside of
//  them unless they are escaped (with a slash, i.e. \").
//
tCIDLib::TBoolean TStringTokenizer::
bParseQuotedCommaList(  const   TStringView&            strvText
                        ,       tCIDLib::TStrCollect&   colToFill
                        ,       tCIDLib::TCard4&        c4ErrIndex)
{
    //
    //  A little enum that we use as a state machine. Note that, in order to
    //  catch a trailing comma, which we don't allow, we have a 'post comma'
    //  state, so that we know we are looking for the start of a new token,
    //  the same as StartQ, but that lets us know if we get to the end of
    //  the source and are in that state, that it was invalid. They are the
    //  same state otherwise for processing purposes.
    //
    enum class EStates
    {
        StartQ
        , EndQ
        , Comma
        , PostComma
    };


    // Clear the incoming just in case
    colToFill.RemoveAll();

    const tCIDLib::TCh* pszSrc = strvText.pszBuffer();
    const tCIDLib::TCh* pszEnd = strvText.pszEnd();

    EStates eCurState = EStates::StartQ;
    TString strAccum;
    while (pszSrc < pszEnd)
    {
        // Get the text char
        const tCIDLib::TCh chCur = *pszSrc++;

        switch(eCurState)
        {
            case EStates::StartQ :
            case EStates::PostComma :
                //
                //  It has to be either white space or a quote. If whitespace
                //  ignore it. If a quote, move to the next state and clear
                //  the accumulator.
                //
                if (chCur == kCIDLib::chQuotation)
                {
                    strAccum.Clear();
                    eCurState = EStates::EndQ;
                }
                 else if (!TRawStr::bIsSpace(chCur))
                {
                    // It's bad
                    c4ErrIndex = (pszSrc - strvText.pszBuffer());
                    return kCIDLib::False;
                }
                break;

            case EStates::EndQ :
                //
                //  It's either the ending quote, or a character in the current
                //  value.
                //
                if (chCur == kCIDLib::chQuotation)
                {
                    colToFill.objAdd(strAccum);
                    eCurState = EStates::Comma;
                }
                 else
                {
                    //
                    //  If we see a backslash, then the next character must
                    //  be another backslash (an escaped slash) or a quote
                    //  character (an escaped quote.) In either case we
                    //  toss the first slash and keep the next character.
                    //
                    //  If it's not one of those, it is in error.
                    //
                    //  Else it's just another char to accumulate
                    //
                    if (chCur == kCIDLib::chBackSlash)
                    {
                        if (*pszSrc == kCIDLib::chQuotation)
                        {
                            strAccum.Append(kCIDLib::chQuotation);
                            pszSrc++;
                        }
                         else if (*pszSrc == kCIDLib::chBackSlash)
                        {
                            strAccum.Append(kCIDLib::chBackSlash);
                            pszSrc++;
                        }
                         else
                        {
                            c4ErrIndex = (pszSrc - strvText.pszBuffer());
                            return kCIDLib::False;
                        }
                    }
                     else
                    {
                        strAccum.Append(chCur);
                    }
                }
                break;

            case EStates::Comma :
                //
                //  We have to see either whitespace, which we ignore, or
                //  a comma.
                //
                if (chCur == kCIDLib::chComma)
                {
                    eCurState = EStates::PostComma;
                }
                 else if (!TRawStr::bIsSpace(chCur))
                {
                    // It's bad
                    c4ErrIndex = (pszSrc - strvText.pszBuffer());
                    return kCIDLib::False;
                }

                break;

            default :
                break;
        };
    }

    // We have to end up on StartQ or Comma
    if ((eCurState != EStates::StartQ) && (eCurState != EStates::Comma))
    {
        c4ErrIndex = (pszSrc - strvText.pszBuffer());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


//
//  This method will break out a standard CSV type of line into a collection
//  of strings. It's a comma separated list, but any one of the values may
//  be quoted. The values cannot have commas inside of them.
//
tCIDLib::TBoolean
TStringTokenizer::bParseCSVLine(const   TStringView&            strvText
                                ,       tCIDLib::TStrCollect&   colToFill
                                ,       tCIDLib::TCard4&        c4ErrIndex)
{
    //
    //  A little enum that we use as a state machine. At any given point, we
    //  are either waiting for a token to start (whcih will either be a quote
    //  for a quoted token or non-whitespace, non-comma if it is not quoted,
    //  or we are waiting for the quote that terminates the token or the comma
    //  that terminates it.
    //
    //  We have two comma states, so that we can remember if we are working on
    //  a quoted token or a non-quoted one. If we get to the comma and it's
    //  non-quoted, we have to store the new toke. If we get to the comma after
    //  a quoted one, then we've already seen the terminating quote and stored
    //  the value.
    //
    //  We also have to be sure to clear the accumulator once we see a token
    //  end and store it, because we can get to the end without seeing any
    //  terminator if the last one isn't quoted, so we hvae to store any last
    //  token, and we know there's one if it's non-empty.
    //
    //  To catch an invalid trailng comma, we have a post-comma state thatwe
    //  treat exactly the same as WaitToken, but it lets us know if we saw
    //  a comma last when we hit the end of source.
    //
    enum class EStates
    {
        WaitToken
        , WaitCommaTerm
        , WaitTermQ
        , WaitComma
        , PostComma
    };


    // Clear the incoming just in case
    colToFill.RemoveAll();

    const tCIDLib::TCh* pszSrc = strvText.pszBuffer();
    const tCIDLib::TCh* pszEnd = strvText.pszEnd();

    EStates eCurState = EStates::WaitToken;
    TString strAccum;
    while (pszSrc < pszEnd)
    {
        // Get the text char
        const tCIDLib::TCh chCur = *pszSrc++;

        switch(eCurState)
        {
            case EStates::WaitToken :
            case EStates::PostComma :
                //
                //  If it's a quote, se start looking for the end quote. If
                //  it's not a whitespace, we start looking for the comma term
                //  since it's a non-quoted token. Otherwise, ignore it.
                //
                //  Note that we can see a comma here also, which means we
                //  saw an empty element, so we add an empty one and move to
                //  wait token.
                //
                if (chCur == kCIDLib::chQuotation)
                {
                    strAccum.Clear();
                    eCurState = EStates::WaitTermQ;
                }
                 else if (chCur == kCIDLib::chComma)
                {
                    colToFill.objAdd(strAccum);
                    strAccum.Clear();
                    eCurState = EStates::PostComma;
                }
                 else if (!TRawStr::bIsSpace(chCur))
                {
                    strAccum.Clear();
                    strAccum.Append(chCur);
                    eCurState = EStates::WaitCommaTerm;
                }
                break;

            case EStates::WaitCommaTerm :
                //
                //  If it's a comma, then we are at the end of a non-quoted
                //  token, so store it and clear the accumulator. If it's
                //  anything else, store it.
                //
                if (chCur == kCIDLib::chComma)
                {
                    //
                    //  Store this one and clear the accumulator, then we
                    //  are back to waiting for a token.
                    //
                    colToFill.objAdd(strAccum);
                    strAccum.Clear();
                    eCurState = EStates::PostComma;
                }
                 else
                {
                    // Nothing special, just store it
                    strAccum.Append(chCur);
                }
                break;

            case EStates::WaitTermQ :
                //
                //  It's either the ending quote, or a character in the current
                //  value.
                //
                if (chCur == kCIDLib::chQuotation)
                {
                    //
                    //  Store it and clear the accumulator. Then we want to
                    //  wait on a non-terminating comma.
                    //
                    colToFill.objAdd(strAccum);
                    strAccum.Clear();
                    eCurState = EStates::WaitComma;
                }
                 else
                {
                    strAccum.Append(chCur);
                }
                break;

            case EStates::WaitComma :
                //
                //  We have to see either whitespace, which we ignore, or
                //  a comma. Anything else is an error.
                //
                if (chCur == kCIDLib::chComma)
                {
                    eCurState = EStates::PostComma;
                }
                 else if (!TRawStr::bIsSpace(chCur))
                {
                    // It's bad
                    c4ErrIndex = (pszSrc - strvText.pszBuffer());
                    return kCIDLib::False;
                }
                break;

            default :
                break;
        };
    }

    //
    //  We cannot end when waiting for a terminating quote. All the other
    //  ones are possible legal scenarios
    //
    if (eCurState == EStates::WaitTermQ)
    {
        c4ErrIndex = (pszSrc - strvText.pszBuffer());
        return kCIDLib::False;
    }

    //
    //  If the accumulator isn't empty, then there was a trailing token.
    //  If we ended on PostComma, then there's a trailing, empty value
    //
    if (!strAccum.bIsEmpty() || (eCurState == EStates::PostComma))
        colToFill.objAdd(strAccum);

    return kCIDLib::True;
}


//
//  This method will break out whitespace space separated tokens. It can optionally
//  append strings to the collection or reset it first, defaults to reset.
//
tCIDLib::TBoolean
TStringTokenizer::bParseSpacedTokens(const  TStringView&            strvText
                                    ,       tCIDLib::TStrCollect&   colToFill
                                    , const tCIDLib::TBoolean       bAppend)
{
    if (!bAppend)
        colToFill.RemoveAll();
    if (strvText.bIsEmpty())
        return kCIDLib::True;

    tCIDLib::TBoolean bInWS = TRawStr::bIsSpace(strvText[0]);
    TString strTmp;
    const tCIDLib::TCard4 c4Len = strvText.c4Length();
    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < c4Len; c4Ind++)
    {
        const tCIDLib::TCh chCur = strvText[c4Ind];
        if (bInWS)
        {
            // Move forward till we hit a non-space
            if (!TRawStr::bIsSpace(chCur))
            {
                strTmp.Clear();
                strTmp += chCur;
                bInWS = kCIDLib::False;
            }
        }
         else
        {
            // Move forward until we hit a space
            if (TRawStr::bIsSpace(chCur))
            {
                colToFill.objAdd(strTmp);
                bInWS = kCIDLib::False;
                strTmp.Clear();
            }
             else
            {
                strTmp += chCur;
            }
        }
    }

    // If we ended on non-WS and the temp string isn't empty, it's a trailing one
    if (!bInWS && !strTmp.bIsEmpty())
        colToFill.objAdd(strTmp);

    return kCIDLib::True;
}


//
//  A helper for building up quoted comma list type values. Basically we
//  add the quotes and commas and escape any internal quotes. We have another
//  that takes a list of values and just calls the first one repeatedly.
//
//  Note that since the \ character is the escapement character, we have to
//  escape those as well, with a double \\.
//
tCIDLib::TVoid
TStringTokenizer::BuildQuotedCommaList( const   TStringView&    strvToAdd
                                        ,       TString&        strAccum)
{
    // If not the first value, then append a command to the end
    if (!strAccum.bIsEmpty())
        strAccum.Append(L", ");

    // Add the starting quote of this value
    strAccum.Append(kCIDLib::chQuotation);

    // How run through the src chars, copying over and escaping quotes
    const tCIDLib::TCh* pszSrc = strvToAdd.pszBuffer();
    const tCIDLib::TCh* pszEnd = strvToAdd.pszEnd();
    while (pszSrc < pszEnd)
    {
        const tCIDLib::TCh chCur = *pszSrc++;
        if (chCur == kCIDLib::chQuotation)
            strAccum.Append(L"\\\"");
        else if (chCur == kCIDLib::chBackSlash)
            strAccum.Append(L"\\\\");
        else
            strAccum.Append(chCur);
    }

    // Put the terminating quote on
    strAccum.Append(kCIDLib::chQuotation);
}


tCIDLib::TVoid
TStringTokenizer::BuildQuotedCommaList( const   tCIDLib::TStrCollect&   colValues
                                        ,       TString&                strAccum)
{
    TColCursor<TString>* pcursVals = colValues.pcursNew();
    TJanitor<TColCursor<TString>> janCurs(pcursVals);

    for (; pcursVals->bIsValid(); pcursVals->bNext())
        BuildQuotedCommaList(pcursVals->objRCur(), strAccum);
}


//
//  Splits the passed text into a sequence of sub-strings,separated by the passed split
//  character.
//
tCIDLib::TVoid TStringTokenizer::SplitOnChar(const  TStringView&            strvText
                                            ,       tCIDLib::TStrCollect&   colToFill
                                            , const tCIDLib::TCh            chSplitChar)
{
    tCIDLib::TCh achSplit[1];
    achSplit[0] = chSplitChar;
    TStringTokenizer stokSplit(strvText, TStringView(achSplit, 1));

    colToFill.RemoveAll();
    TString strToken;
    while (stokSplit.bGetNextToken(strToken))
    {
        colToFill.objAdd(strToken);
    }
}


// ---------------------------------------------------------------------------
//  TStringTokenizer: Constructors and Destructor
// ---------------------------------------------------------------------------
TStringTokenizer::TStringTokenizer() :

    m_c4CurOffset(0)
    , m_c4CurToken(0)
    , m_c4SourceLen(0)
    , m_strvSrc(TString::strvEmpty())
    , m_strWhitespace(kCIDLib::szWhitespace)
{
    Reset();
}

TStringTokenizer::TStringTokenizer( const   TStringView     strvToTokenize
                                    , const TStringView&    strvWhitespace) :

    m_c4CurOffset(0)
    , m_c4CurToken(0)
    , m_c4SourceLen(0)
    , m_strWhitespace(strvWhitespace)
    , m_strvSrc(strvToTokenize)
{
    Reset();
}


// ---------------------------------------------------------------------------
//  TStringTokenizer: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Destructively throw away the next token
tCIDLib::TBoolean TStringTokenizer::bEatNextToken()
{
    // If already at the end, just return false
    if (m_c4CurOffset >= m_c4SourceLen)
        return kCIDLib::False;

    //
    //  Get the length of the current token. It cannot be zero since
    //  we saw above that we are not at the end of the string yet. And
    //  getting the previous token would have put it at the end if there
    //  was nothing left but whitespace.
    //
    const tCIDLib::TCard4 c4TokenLen = c4CurTokenLen();

    // If debugging, check it anyway
    #if CID_DEBUG_ON
    if (!c4TokenLen)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_TokenizerError
            , tCIDLib::ESeverities::SysFatal
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif

    //
    //  Now reset the current offset to the end of this token and move to
    //  the next non-whitespace character.
    //
    m_c4CurOffset += c4TokenLen;
    FindNext();

    // Bump up the index of the next token to get
    m_c4CurToken++;
    return kCIDLib::True;
}


// Destructively get the next token
tCIDLib::TBoolean TStringTokenizer::bGetNextToken(TString& strToFill)
{
    // If already at the end, just return an empty string
    if (m_c4CurOffset >= m_c4SourceLen)
    {
        strToFill.Clear();
        return kCIDLib::False;
    }

    //
    //  Get the length of the current token. It cannot be zero because
    //  we are at the end, because we saw above that we are not at the end
    //  of the string yet. And getting the previous token would have put
    //  it at the end if there was nothing left but whitespace. So it has
    //  to be a legitimately empty token.
    //
    const tCIDLib::TCard4 c4TokenLen = c4CurTokenLen();
    if (!c4TokenLen)
    {
        strToFill.Clear();
    }
     else
    {
        // Copy this token text to caller's string
        strToFill.CopyInSubStr(m_strvSrc, m_c4CurOffset, c4TokenLen);
    }

    //
    //  Now reset the current offset to the end of this token and move to
    //  the next non-whitespace character.
    //
    m_c4CurOffset += c4TokenLen;
    FindNext();

    // Bump up the index of the next token to get, and return success
    m_c4CurToken++;

    return kCIDLib::True;
}


//
//  Destructively get the rest of the line. There will be no more tokens
//  available after this, since we'll set the current offset ot the end of
//  the string.
//
tCIDLib::TBoolean TStringTokenizer::bGetRestOfLine(TString& strToFill)
{
    // If at the end of the line, just clear the return string and return false
    if (m_c4CurOffset >= m_c4SourceLen)
    {
        strToFill.Clear();
        return kCIDLib::False;
    }

    // Otherwise, just copy the rest of the source string to the token
    strToFill.CopyInSubStr(m_strvSrc, m_c4CurOffset);

    // And set our position to the end of the string
    m_c4CurOffset = m_c4SourceLen;
    return kCIDLib::True;
}


// Checks to see if more tokens are potentially available
tCIDLib::TBoolean TStringTokenizer::bMoreTokens() const
{
    #if CID_DEBUG_ON
    if (m_strvSrc.c4Length() != m_c4SourceLen)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_SourceModified
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }
    #endif
    return (m_c4CurOffset < m_c4SourceLen);
}


// Non-destructively returns the next token, if any
tCIDLib::TBoolean TStringTokenizer::bPeekNextToken(TString& strToFill) const
{
    // If already at the end, just return an empty string
    if (m_c4CurOffset >= m_c4SourceLen)
    {
        strToFill.Clear();
        return kCIDLib::False;
    }

    //
    //  Get the length of the current token. It cannot be zero because
    //  we are at the end, because we saw above that we are not at the end
    //  of the string yet. And getting the previous token would have put
    //  it at the end if there was nothing left but whitespace. So it has
    //  to be a legitimately empty token.
    //
    const tCIDLib::TCard4 c4TokenLen = c4CurTokenLen();
    if (!c4TokenLen)
    {
        strToFill.Clear();
    }
     else
    {
        // Copy this token text to caller's string
        strToFill.CopyInSubStr(m_strvSrc, m_c4CurOffset, c4TokenLen);
    }
    return kCIDLib::True;
}


// Non-destructively returns the rest of the line
tCIDLib::TBoolean TStringTokenizer::bPeekRestOfLine(TString& strToFill) const
{
    // If at the end of the line, just clear the current token and return that
    if (m_c4CurOffset >= m_c4SourceLen)
    {
        strToFill.Clear();
        return kCIDLib::False;
    }

    // Otherwise, just copy the rest of the source string to the token
    strToFill.CopyInSubStr(m_strvSrc, m_c4CurOffset);
    return kCIDLib::True;
}


//
//  Does the whole reset, next, next, .... sequence internally and fills in
//  the passed collection with the resulting tokens.
//
tCIDLib::TCard4
TStringTokenizer::c4BreakApart(tCIDLib::TStrCollect& colToFill)
{
    colToFill.RemoveAll();

    Reset();
    TString strToken;
    while (bGetNextToken(strToken))
        colToFill.objAdd(strToken);
    return colToFill.c4ElemCount();
}


// Returns the number of the current token, zero based
tCIDLib::TCard4 TStringTokenizer::c4CurToken() const
{
    return m_c4CurToken;
}


// Resets us on the original input string, to start tokeninizing it again
tCIDLib::TVoid TStringTokenizer::Reset()
{
    // Reset our two counters
    m_c4CurOffset = 0;
    m_c4CurToken = 0;

    // Reset the length of the string
    m_c4SourceLen = m_strvSrc.c4Length();

    //
    //  And find the next token. This will get the internal state ready
    //  return the first token available.
    //
    FindNext();
}


// Same as above but allows a new whitespace list ot be set in the process
tCIDLib::TVoid
TStringTokenizer::Reset(const   TStringView strvToTokenize
                        , const TString&    strWhitespace)
{
    m_strvSrc = strvToTokenize;
    m_strWhitespace = strWhitespace;
    Reset();
}

tCIDLib::TVoid
TStringTokenizer::Reset(const   TStringView         strvToTokenize
                        , const tCIDLib::TCh* const pszWhitespace)
{
    m_strvSrc = strvToTokenize;
    m_strWhitespace = pszWhitespace;
    Reset();
}


// Returns a ref to the original text we are tokenizing
const TStringView& TStringTokenizer::strvSrcText() const
{
    return m_strvSrc;
}


// Get or set the whitespace list
const TString& TStringTokenizer::strWhitespace() const
{
    return m_strWhitespace;
}

const TString& TStringTokenizer::strWhitespace(const TStringView& strvToSet)
{
    m_strWhitespace = strvToSet;
    return m_strWhitespace;
}


// ---------------------------------------------------------------------------
//  TStringTokenizer: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TStringTokenizer::c4CurTokenLen() const
{
    //
    //  Look forward from the start of the current token to find the first
    //  whitespace character. The difference is the length of the current
    //  token.
    //
    const tCIDLib::TCh* const pszSrc = m_strvSrc.pszBuffer();

    tCIDLib::TCard4 c4EndInd = m_c4CurOffset;
    while (c4EndInd < m_c4SourceLen)
    {
        if (m_strWhitespace.bContainsChar(pszSrc[c4EndInd]))
            break;
        c4EndInd++;
    }

    //
    //  We actually went to the next char beyond the end of the token.
    //  But, because of the fact that the first character is part of it,
    //  this subtraction comes out right.
    //
    return (c4EndInd - m_c4CurOffset);
}


tCIDLib::TVoid TStringTokenizer::FindNext()
{
    // Get fast access to raw string buffers
    const tCIDLib::TCh* const pszSrc = m_strvSrc.pszBuffer();

    // Move up to the first non-whitespace character
    while (m_c4CurOffset < m_c4SourceLen)
    {
        if (!m_strWhitespace.bContainsChar(pszSrc[m_c4CurOffset]))
            break;
        m_c4CurOffset++;
    }
}
