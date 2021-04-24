//
// FILE NAME: CIDLib_StreamLexer.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/20/1993
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
//  This file implements the TStreamLexer class. The stream lexer is a
//  means of doing complex syntax based lexical analisys of any input source
//  available via the TTextInStream interface.
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
RTTIDecls(TStreamLexer,TObject)


namespace CIDLib_StreamLexer
{
    namespace
    {
        // -----------------------------------------------------------------------
        //  Local constant data
        //
        //  c4MaxTokens
        //      This is the maximum number of predefined tokens.
        //
        //  c4MaxUnget
        //      This is the maximum size of the character unget stack.
        // -----------------------------------------------------------------------
        constexpr tCIDLib::TCard4   c4MaxTokens  = 64;
        constexpr tCIDLib::TCard4   c4MaxUnget   = 4096;
    }
}


// ---------------------------------------------------------------------------
//   CLASS: TStreamLexer
//  PREFIX: lexr
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStreamLexer: Constructors and Destructor
// ---------------------------------------------------------------------------
TStreamLexer::TStreamLexer(         TTextInStream* const    pstrmToReadFrom
                            , const tCIDLib::EAdoptOpts     eAdopt) :
    m_c4CurLine(1)
    , m_c4TokenIndex(0)
    , m_eAdopt(eAdopt)
    , m_i4StackIndex(-1)
    , m_pac4Tokens(nullptr)
    , m_pchCharStack(nullptr)
    , m_pstrmSrc(pstrmToReadFrom)
{
    //
    //  Just on the bizarre chance that we cannot allocate any memory
    //  catch and cleanup. Though its probably not worth it since we are
    //  asking for such a small amount of memory here that its unlikey
    //  anything is going to survive after this.
    //
    try
    {
        m_pac4Tokens = new tCIDLib::TCard4[CIDLib_StreamLexer::c4MaxTokens];
        m_pchCharStack = new tCIDLib::TCh[CIDLib_StreamLexer::c4MaxUnget];
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        delete [] m_pac4Tokens;
        delete [] m_pchCharStack;

        if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
            delete m_pstrmSrc;

        throw;
    }
}

TStreamLexer::~TStreamLexer()
{
    delete [] m_pac4Tokens;
    delete [] m_pchCharStack;

    // Delete the stream if we adopted it
    if (m_eAdopt == tCIDLib::EAdoptOpts::Adopt)
        delete m_pstrmSrc;
}


// ---------------------------------------------------------------------------
//  TStreamLexer: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TStreamLexer::AddSyntaxToken(const TString& strBuf)
{
    if (m_c4TokenIndex >= CIDLib_StreamLexer::c4MaxTokens)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcLex_MaxTokens
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(CIDLib_StreamLexer::c4MaxTokens)
        );
    }

    tCIDLib::TCard4 c4Len = strBuf.c4Length();

    #if CID_DEBUG_ON
    if (!c4Len || (c4Len > 2))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcLex_TokenLength
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , strBuf
        );
    }
    #endif

    // Add a new node to the list
    m_pac4Tokens[m_c4TokenIndex] = 0;
    if (c4Len == 2)
    {
        m_pac4Tokens[m_c4TokenIndex] = strBuf[1];
        m_pac4Tokens[m_c4TokenIndex] <<= 16;
    }
    m_pac4Tokens[m_c4TokenIndex] += strBuf[0];

    // Bump up the token index
    m_c4TokenIndex++;
}


tCIDLib::TCard4 TStreamLexer::c4CurLine() const
{
    return m_c4CurLine;
}


tCIDLib::TCh TStreamLexer::chGetNextChar()
{
    return chGetNext();
}


tCIDLib::TVoid TStreamLexer::FlushLine()
{
    tCIDLib::TBoolean   bWaiting = kCIDLib::True;
    tCIDLib::TCh        chNext = chGetNext();

    while (chNext)
    {
        if (bWaiting)
       {
            if ((chNext == kCIDLib::chLF) || (chNext == kCIDLib::chCR))
                bWaiting = kCIDLib::False;
        }
         else
        {
            if ((chNext != kCIDLib::chLF) && (chNext != kCIDLib::chCR))
            {
                UnGet(chNext);
                break;
            }
        }
        chNext = chGetNext();
    }
}


tCIDLib::TVoid
TStreamLexer::GetNextToken( const   TString&    strSeparators
                            ,       TString&    strTarget
                            , const TString&    strNoSyntax)
{
    // Make sure to start off the target empty
    strTarget.Clear();

    //
    //  First we need to read until we get a non-separator character. Then
    //  we unget that character and fall into the next loop.
    //
    tCIDLib::TCard4 c4Dummy     = 0;
    tCIDLib::TCard4 c4Ind       = 0;
    tCIDLib::TCh    chNext      = chGetNext();

    while (chNext)
    {
        if (!strSeparators.bFirstOccurrence(chNext, c4Dummy))
        {
            UnGet(chNext);
            break;
        }
        chNext = chGetNext();
    }

    if (!chNext)
        return;

    //
    //  Ok, start copying characters to the target until we come to a
    //  separator or syntax character. chGetNext() will return 0 if it
    //  cannot get another character.
    //
    chNext = chGetNext();
    while (chNext)
    {
        ETokenMatches eMatch = eTokenMatch(chNext, 0, strNoSyntax);

        if (eMatch == ETokenMatches::Full)
        {
            //
            //  If current index is 0, then this is the token itself.
            //  Otherwise, it just means we unget the character for next
            //  time and break out with current token text.
            //
            if (c4Ind == 0)
            {
                strTarget.Append(chNext);
                c4Ind++;
                break;
            }
             else
            {
                UnGet(chNext);
                break;
            }
        }
         else if (eMatch == ETokenMatches::First)
        {
            tCIDLib::TCh chSecond = chGetNext();

            if (eTokenMatch(chNext, chSecond, strNoSyntax) == ETokenMatches::Full)
            {
                // It is a 2 char token, same rules as above
                if (c4Ind == 0)
                {
                    strTarget.Append(chNext);
                    c4Ind++;
                    strTarget.Append(chSecond);
                    c4Ind++;
                }
                 else
                {
                    // Unget the first and second char
                    UnGet(chSecond);
                    UnGet(chNext);
                }
                break;
            }
             else
            {
                // Unget the second char
                UnGet(chSecond);
            }
        }

        if (!strSeparators.bFirstOccurrence(chNext, c4Dummy))
        {
            // Not in the list so copy over
            strTarget.Append(chNext);
            c4Ind++;
        }
         else
        {
            // We hit a separator so unget and break
            UnGet(chNext);
            break;
        }

        chNext = chGetNext();
    }
}


//
//  <TBD> This could be more efficient by getting into a temp buffer then
//  putting into the target string. The appends are costly since the end of
//  the string must be found. However, proposed changes to TString to maintain
//  the end of string index would make this a non-issue!
//
tCIDLib::TVoid TStreamLexer::GetLineRemainder(TString& strTarget)
{
    // Clear the string
    strTarget.Clear();

    //
    //  Now read until we hit the end of a line, appending each character
    //  that we get to the string.
    //
    tCIDLib::TBoolean bDone = kCIDLib::False;
    while (!bDone)
    {
        tCIDLib::TCh chNext = chGetNext();

        if (!chNext)
            return;

        if ((chNext == kCIDLib::chLF) || (chNext == kCIDLib::chCR))
        {
            UnGet(chNext);
            return;
        }

        strTarget.Append(chNext);
    }
}


tCIDLib::TVoid TStreamLexer::UnGetChar(const tCIDLib::TCh chPush)
{
    UnGet(chPush);
}


tCIDLib::TVoid TStreamLexer::UnGetToken(const TString& strTokenText)
{
    tCIDLib::TCard4 c4Len = strTokenText.c4Length();
    if (!c4Len)
        return;

    const tCIDLib::TCh* pszText = strTokenText.pszBuffer();
    tCIDLib::TCard4     c4Index = c4Len-1;

    tCIDLib::TBoolean bDone = kCIDLib::False;
    while (!bDone)
    {
        UnGet(pszText[c4Index]);
        if (!c4Index)
            break;
        c4Index--;
    }
}

tCIDLib::TVoid
TStreamLexer::UnGetToken(const tCIDLib::TCh* const pszTokenText)
{
    tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszTokenText);
    if (!c4Len)
        return;

    tCIDLib::TCard4 c4Index = c4Len-1;

    tCIDLib::TBoolean bDone = kCIDLib::False;
    while (!bDone)
    {
        UnGet(pszTokenText[c4Index]);
        if (!c4Index)
            break;
        c4Index--;
    }
}


// ---------------------------------------------------------------------------
//  TStreamLexer: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This method will return the next character in the buffer. If we have
//  read all of the characters in this buffer, then a new buffer's worth
//  will be read in. When the end of the file is hit, then a 0 is returned.
//
tCIDLib::TCh TStreamLexer::chGetNext()
{
    // If the unget stack is not empty, then return the stack top
    if (m_i4StackIndex != -1)
    {
        const tCIDLib::TCh chRet = m_pchCharStack[m_i4StackIndex];
        m_i4StackIndex--;
        return chRet;
    }

    // If we are at the end of stream, return a nul char
    if (m_pstrmSrc->bEndOfStream())
        return kCIDLib::chNull;

    // Else, get the next char from the stream
    const tCIDLib::TCh chRet = m_pstrmSrc->chRead();

    // If it is a new line, then bump up line count
    if (chRet == kCIDLib::chLF)
        m_c4CurLine++;

    // Return the next character and bump up the index
    return chRet;
}


//
// DESCRIPTION:
//
//  This method will look for the passed token text in the token list. A
//  token is either 1 or 2 characters. If the chSecond parm is 0, then a
//  search is made for a single character token that matches the first char.

//  RETURN: ETokenMatches::Full if there was a full match, ETokenMatches::First if there
//              was no single character token that matched chFirst, but it
//              did match the first character of at least one token, and
//              ETokenMatches::None if no match at all.
//
TStreamLexer::ETokenMatches
TStreamLexer::eTokenMatch(  const   tCIDLib::TCh    chFirst
                            , const tCIDLib::TCh    chSecond
                            , const TString&        strNoSyntax)
{
    if (!TString::bIsNullObject(strNoSyntax))
    {
        tCIDLib::TCard4 c4Dummy;

        if (strNoSyntax.bFirstOccurrence(chFirst, c4Dummy))
            return ETokenMatches::None;

        if (chSecond)
        {
            if (strNoSyntax.bFirstOccurrence(chSecond, c4Dummy))
                return ETokenMatches::None;
        }
    }

    tCIDLib::TBoolean   bPartial = kCIDLib::False;
    tCIDLib::TCard4     c4Test = tCIDLib::TCard4
    (
        (tCIDLib::TCard4(chSecond) << 16) | chFirst
    );

    for (tCIDLib::TCard4 c4Ind = 0; c4Ind < m_c4TokenIndex; c4Ind++)
    {
        if (c4Test == m_pac4Tokens[c4Ind])
            return ETokenMatches::Full;

        if (chFirst == tCIDLib::TCh(m_pac4Tokens[c4Ind] & 0xFFFF))
            bPartial = kCIDLib::True;
    }
    if (bPartial)
        return ETokenMatches::First;

    return ETokenMatches::None;
}


//
//  This method will unget the last character read. It just sets the unget
//  flag.
//
tCIDLib::TVoid TStreamLexer::UnGet(const tCIDLib::TCh chPush)
{
    if ((m_i4StackIndex >= CIDLib_StreamLexer::c4MaxUnget) && (m_i4StackIndex != -1))
    {
        // Can't unget but once
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcLex_UngetFull
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
        );
    }

    m_i4StackIndex++;
    m_pchCharStack[m_i4StackIndex] = chPush;
}
