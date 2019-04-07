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


namespace CIDLib_String
{
    // -----------------------------------------------------------------------
    //  Local, constant data
    //
    //  c1FmtVersion
    //      A format version that we stream out, so that we can upgrade the
    //      persistence format automatically later if needed.
    //
    //  c4ReallocBlock
    //      The steps that we try to alloc strings in.
    // -----------------------------------------------------------------------
    const tCIDLib::TCard1   c1FmtVersion    = 1;
    const tCIDLib::TCard4   c4ReallocBlock  = 64;
}



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------

//
//  This method will find a token in the passed string, parse it out, and
//  give back the token information.
//
//  Returns a pointer to the % sign of the token found, or 0 if not found.
//
//  We also return whether the count of tokens we saw up until we found the
//  target one. This can allow for significant optimization by calling code
//  that is going through an replacing multiple tokens, if it knows no more
//  will be found. If the indicated token was not found, then we went all
//  the way to the end, and if there were no tokens seen, then there are
//  no more tokens in the source text.
//
static const tCIDLib::TCh*
pszFindToken(   const   tCIDLib::TCh* const pszSource
                , const tCIDLib::TCh        chToken
                ,       tCIDLib::EHJustify& eJustify
                ,       tCIDLib::TCard4&    c4FldWidth
                ,       tCIDLib::TCh&       chFill
                ,       tCIDLib::TCard4&    c4TokenChars
                ,       tCIDLib::TCard4&    c4TokenCnt)
{
    const tCIDLib::TCh* const pszSeparators = L" (,)%";

    tCIDLib::TCh        chTokenType;
    tCIDLib::TInt4      i4WidthVal;
    const tCIDLib::TCh* pszCur;
    const tCIDLib::TCh* pszClose;
    tCIDLib::TCh*       pszTok;
    tCIDLib::TCh        szTmp[64];

    // Set up the returns with correct default values
    eJustify        = tCIDLib::EHJustify::Right;
    c4FldWidth      = 0;
    c4TokenChars    = 0;
    chFill          = kCIDLib::chSpace;

    // Initialize the token count
    c4TokenCnt = 0;

    //
    //  Do an initial search for a percent sign. After we find one, check the
    //  next character to see what we are looking for.
    //
    pszCur = TRawStr::pszFindChar(pszSource, L'%');
    while (pszCur != 0)
    {
        // See if the next char is the end of the string. If so, then no match
        if (*(pszCur + 1) == 0)
            return 0;

        // If the next char is a (, then it is a token so check it out
        if (*(pszCur + 1) == L'(')
        {
            // Search for the closing paren
            pszClose = TRawStr::pszFindChar(pszCur + 1, L')');

            //
            //  There was no closing parm so we need to just search for the
            //  next %, because this was a false alarm.
            //
            if (!pszClose)
            {
                pszCur++;
                pszCur = TRawStr::pszFindChar(pszCur, L'%');
                continue;
            }

            //
            //  If the enclosed text is larger than the tmp buffer, then
            //  something is wrong.
            //
            if (pszClose - pszCur > c4MaxBufChars(szTmp))
            {
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcStr_InvalidToken
                    , tCIDLib::ESeverities::Warn
                    , tCIDLib::EErrClasses::BadParms
                );
                return 0;
            }

            // If it is empty, then can't be right
            if (pszClose-pszCur == 1)
            {
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcStr_InvalidToken
                    , tCIDLib::ESeverities::Warn
                    , tCIDLib::EErrClasses::BadParms
                );
                return 0;
            }

            //
            //  We need to parse the token contents. So copy the substring of
            //  the token to a temp buffer. Note that we multiply by the char
            //  size in order to convert chars to bytes.
            //
            TRawMem::CopyMemBuf
            (
                szTmp
                , pszCur
                , (pszClose - pszCur) * kCIDLib::c4CharBytes
            );
            szTmp[pszClose - pszCur] = kCIDLib::chNull;

            tCIDLib::TCh* pszCtx = 0;
            pszTok = TRawStr::pszStrTokenize(szTmp, pszSeparators, &pszCtx);

            //
            //  If no token or the more than a character, then invalid.
            //  Otherwise, pull out the token as the token character.
            //
            if (!pszTok || (TRawStr::c4StrLen(pszTok) > 1))
            {
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcStr_InvalidToken
                    , tCIDLib::ESeverities::Warn
                    , tCIDLib::EErrClasses::BadParms
                );
                return 0;
            }
            chTokenType = pszTok[0];

            // Bump the count of tokens seen so far
            c4TokenCnt++;

            //
            //  See if the char is the token character. If not, then start
            //  looking for the next character. It has to start after the
            //  close parent we already found, so we can skip up to there.
            //
            if (chToken != chTokenType)
            {
                pszCur = pszClose;
                pszCur = TRawStr::pszFindChar(pszCur, L'%');
                continue;
            }

            // We know the token width now
            c4TokenChars = (pszClose - pszCur) + 1;

            //
            //  Look for another token. If so, then it will be the field
            //  width. It must be a valid integral value. If it is negative,
            //  that means left justification.
            //
            pszTok = TRawStr::pszStrTokenize(0, pszSeparators, &pszCtx);
            if (pszTok)
            {
                tCIDLib::TBoolean  bValid;
                i4WidthVal = TRawStr::i4AsBinary
                (
                    pszTok
                    , bValid
                    , tCIDLib::ERadices::Dec
                );

                if (!bValid)
                {
                    facCIDLib().ThrowErr
                    (
                        CID_FILE
                        , CID_LINE
                        , kCIDErrs::errcStr_InvalidToken
                        , tCIDLib::ESeverities::Warn
                        , tCIDLib::EErrClasses::BadParms
                    );
                    return 0;
                }

                if (i4WidthVal < 0)
                {
                    eJustify = tCIDLib::EHJustify::Left;
                    if (i4WidthVal < 0)
                        c4FldWidth = i4WidthVal * -1;
                    else
                        c4FldWidth = i4WidthVal;
                }
                 else
                {
                    c4FldWidth = i4WidthVal;
                }

                pszTok = TRawStr::pszStrTokenize(0, pszSeparators, &pszCtx);
                if (pszTok)
                {
                    if (TRawStr::c4StrLen(pszTok) > 1)
                    {
                        facCIDLib().ThrowErr
                        (
                            CID_FILE
                            , CID_LINE
                            , kCIDErrs::errcStr_InvalidToken
                            , tCIDLib::ESeverities::Warn
                            , tCIDLib::EErrClasses::BadParms
                        );
                        return 0;
                    }
                    chFill = pszTok[0];
                }
            }

            // Break out to return
            break;
        }

        // Look for another percent sign
        pszCur++;
        pszCur = TRawStr::pszFindChar(pszCur, L'%');
    }
    return pszCur;
}



// ---------------------------------------------------------------------------
//  CLASS: TStrCat
// PREFIX: scat
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStrCat: Constructors and Destructor.
// ---------------------------------------------------------------------------

//
//  To be efficient, we don't call CatStr, because it has to run all the way up to
//  the end each time. So we take the hit here and do memory moves. That willl make
//  this fairly fundamental thing vastly more efficient.
//
//  We have some special ones that combine strings and characters, which is also
//  pretty common.
//
TStrCat::TStrCat(const TString& str1, const TString& str2) :

    m_pszBuf(nullptr)
{
    const tCIDLib::TCard4 c4Len1 = str1.c4Length();
    const tCIDLib::TCard4 c4Len2 = str2.c4Length();
    m_pszBuf = new tCIDLib::TCh[c4Len1 + c4Len2 + 1];

    TRawMem::CopyMemBuf(m_pszBuf, str1.pszBuffer(), c4Len1 *  kCIDLib::c4CharBytes);
    TRawMem::CopyMemBuf(&m_pszBuf[c4Len1], str2.pszBuffer(), c4Len2 * kCIDLib::c4CharBytes);

    m_c4Len = c4Len1 + c4Len2;
    m_pszBuf[m_c4Len] = kCIDLib::chNull;
}

TStrCat::TStrCat(const  TString& str1, const TString& str2, const TString& str3) :

    m_pszBuf(nullptr)
{
    const tCIDLib::TCard4 c4Len1 = str1.c4Length();
    const tCIDLib::TCard4 c4Len2 = str2.c4Length();
    const tCIDLib::TCard4 c4Len3 = str3.c4Length();
    m_pszBuf = new tCIDLib::TCh[c4Len1 + c4Len2 + c4Len3 + 1];

    TRawMem::CopyMemBuf(m_pszBuf, str1.pszBuffer(), c4Len1 *  kCIDLib::c4CharBytes);
    TRawMem::CopyMemBuf(&m_pszBuf[c4Len1], str2.pszBuffer(), c4Len2 * kCIDLib::c4CharBytes);
    TRawMem::CopyMemBuf
    (
        &m_pszBuf[c4Len1 + c4Len2], str3.pszBuffer(), c4Len3 * kCIDLib::c4CharBytes
    );

    m_c4Len = c4Len1 + c4Len2 + c4Len3;
    m_pszBuf[m_c4Len] = kCIDLib::chNull;
}


TStrCat::TStrCat(const  TString&    str1
                , const TString&    str2
                , const TString&    str3
                , const TString&    str4) :

    m_pszBuf(nullptr)
{
    const tCIDLib::TCard4 c4Len1 = str1.c4Length();
    const tCIDLib::TCard4 c4Len2 = str2.c4Length();
    const tCIDLib::TCard4 c4Len3 = str3.c4Length();
    const tCIDLib::TCard4 c4Len4 = str4.c4Length();
    m_pszBuf = new tCIDLib::TCh[c4Len1 + c4Len2 + c4Len3 + c4Len4 + 1];

    TRawMem::CopyMemBuf(m_pszBuf, str1.pszBuffer(), c4Len1 *  kCIDLib::c4CharBytes);
    TRawMem::CopyMemBuf(&m_pszBuf[c4Len1], str2.pszBuffer(), c4Len2 * kCIDLib::c4CharBytes);
    TRawMem::CopyMemBuf
    (
        &m_pszBuf[c4Len1 + c4Len2], str3.pszBuffer(), c4Len3 * kCIDLib::c4CharBytes
    );
    TRawMem::CopyMemBuf
    (
        &m_pszBuf[c4Len1 + c4Len2 + c4Len3], str4.pszBuffer(), c4Len4 * kCIDLib::c4CharBytes
    );


    m_c4Len = c4Len1 + c4Len2 + c4Len3 + c4Len4;
    m_pszBuf[m_c4Len] = kCIDLib::chNull;
}


TStrCat::TStrCat(const TString& str1, const tCIDLib::TCh chSep) :

    m_pszBuf(nullptr)
{
    const tCIDLib::TCard4 c4Len1 = str1.c4Length();
    m_pszBuf = new tCIDLib::TCh[c4Len1 + 2];

    TRawMem::CopyMemBuf(m_pszBuf, str1.pszBuffer(), c4Len1 *  kCIDLib::c4CharBytes);
    TRawMem::CopyMemBuf(&m_pszBuf[c4Len1], &chSep, kCIDLib::c4CharBytes);

    m_c4Len = c4Len1 + 1;
    m_pszBuf[m_c4Len] = kCIDLib::chNull;
}


TStrCat::TStrCat(const  TString&        str1
                , const tCIDLib::TCh    chSep
                , const TString&        str2) :


    m_pszBuf(nullptr)
{
    const tCIDLib::TCard4 c4Len1 = str1.c4Length();
    const tCIDLib::TCard4 c4Len2 = str2.c4Length();
    m_pszBuf = new tCIDLib::TCh[c4Len1 + c4Len2 + 2];

    TRawMem::CopyMemBuf(m_pszBuf, str1.pszBuffer(), c4Len1 *  kCIDLib::c4CharBytes);
    TRawMem::CopyMemBuf(&m_pszBuf[c4Len1], &chSep, kCIDLib::c4CharBytes);
    TRawMem::CopyMemBuf
    (
        &m_pszBuf[c4Len1 + 1], str2.pszBuffer(), c4Len2 *  kCIDLib::c4CharBytes
    );

    m_c4Len = c4Len1 + + c4Len2 + 1;
    m_pszBuf[m_c4Len] = kCIDLib::chNull;
}


TStrCat::TStrCat(const TString& str1, const tCIDLib::TCh* const psz2) :

    m_pszBuf(nullptr)
{
    const tCIDLib::TCard4 c4Len1 = str1.c4Length();
    const tCIDLib::TCard4 c4Len2 = (psz2 == nullptr) ? 0 : TRawStr::c4StrLen(psz2);
    m_pszBuf = new tCIDLib::TCh[c4Len1 + c4Len2 + 1];

    TRawMem::CopyMemBuf(m_pszBuf, str1.pszBuffer(), c4Len1 *  kCIDLib::c4CharBytes);
    TRawMem::CopyMemBuf(&m_pszBuf[c4Len1], psz2, c4Len2 * kCIDLib::c4CharBytes);

    m_c4Len = c4Len1 + c4Len2;
    m_pszBuf[m_c4Len] = kCIDLib::chNull;
}


TStrCat::TStrCat(const tCIDLib::TCh* const psz1, const TString& str2) :

    m_pszBuf(nullptr)
{
    const tCIDLib::TCard4 c4Len1 = (psz1 == nullptr) ? 0 : TRawStr::c4StrLen(psz1);
    const tCIDLib::TCard4 c4Len2 = str2.c4Length();
    m_pszBuf = new tCIDLib::TCh[c4Len1 + c4Len2 + 1];

    TRawMem::CopyMemBuf(m_pszBuf, psz1, c4Len1 *  kCIDLib::c4CharBytes);
    TRawMem::CopyMemBuf(&m_pszBuf[c4Len1], str2.pszBuffer(), c4Len2 * kCIDLib::c4CharBytes);

    m_c4Len = c4Len1 + c4Len2;
    m_pszBuf[m_c4Len] = kCIDLib::chNull;
}


TStrCat::TStrCat(const tCIDLib::TCh* const psz1, const tCIDLib::TCh* const psz2) :

    m_pszBuf(nullptr)
{
    const tCIDLib::TCard4 c4Len1 = (psz1 == nullptr) ? 0 : TRawStr::c4StrLen(psz1);
    const tCIDLib::TCard4 c4Len2 = (psz2 == nullptr) ? 0 : TRawStr::c4StrLen(psz2);
    m_pszBuf = new tCIDLib::TCh[c4Len1 + c4Len2 + 1];

    TRawMem::CopyMemBuf(m_pszBuf, psz1, c4Len1 * kCIDLib::c4CharBytes);
    TRawMem::CopyMemBuf(&m_pszBuf[c4Len1], psz2, c4Len2 * kCIDLib::c4CharBytes);

    m_c4Len = c4Len1 + c4Len2;
    m_pszBuf[m_c4Len] = kCIDLib::chNull;
}


TStrCat::TStrCat(const  tCIDLib::TCh* const psz1
                , const tCIDLib::TCh        chSep
                , const tCIDLib::TCh* const psz2) :

    m_pszBuf(nullptr)
{
    const tCIDLib::TCard4 c4Len1 = (psz1 == nullptr) ? 0 : TRawStr::c4StrLen(psz1);
    const tCIDLib::TCard4 c4Len2 = (psz2 == nullptr) ? 0 : TRawStr::c4StrLen(psz2);
    m_pszBuf = new tCIDLib::TCh[c4Len1 + c4Len2 + 2];

    TRawMem::CopyMemBuf(m_pszBuf, psz1, c4Len1 * kCIDLib::c4CharBytes);
    TRawMem::CopyMemBuf(&m_pszBuf[c4Len1], &chSep, kCIDLib::c4CharBytes);
    TRawMem::CopyMemBuf(&m_pszBuf[c4Len1 + 1], psz2, c4Len2 * kCIDLib::c4CharBytes);

    m_c4Len = c4Len1 + c4Len2 + 1;
    m_pszBuf[m_c4Len] = kCIDLib::chNull;
}




// If by chance we never got to give it away, delete it
TStrCat::~TStrCat()
{
    delete [] m_pszBuf;
}


// ---------------------------------------------------------------------------
//  TStrCat: Public, non-virtual methods
// ---------------------------------------------------------------------------

// The string calls this to steal our buffer
tCIDLib::TCh* TStrCat::pszStealBuf(tCIDLib::TCard4& c4Len) const
{
    tCIDLib::TCh* pszRet = m_pszBuf;
    m_pszBuf = nullptr;
    c4Len = m_c4Len;
    m_c4Len = 0;
    return pszRet;
}



// ---------------------------------------------------------------------------
//   CLASS: TStrBuf
//  PREFIX: strb
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStrBuf: Constructors and Destructor
// ---------------------------------------------------------------------------
TString::TStrBuf::TStrBuf() :

    m_c4CurEnd(0)
    , m_c4BufChars(8)
    , m_pszBuffer(nullptr)
{
    m_pszBuffer = new tCIDLib::TCh[m_c4BufChars + 1];
    m_pszBuffer[0] = kCIDLib::chNull;
}

TString::TStrBuf::TStrBuf(const tCIDLib::TCard4 c4BufSize) :

    m_c4CurEnd(0)
    , m_c4BufChars(c4BufSize)
    , m_pszBuffer(nullptr)
{
    // Catch psycho scenario and bump it back to 8
    if (!m_c4BufChars)
        m_c4BufChars = 8;

    m_pszBuffer = new tCIDLib::TCh[m_c4BufChars + 1];
    m_pszBuffer[0] = kCIDLib::chNull;
}

TString::TStrBuf::TStrBuf(const tCIDLib::TCh chInit) :

    m_c4CurEnd(1)
    , m_c4BufChars(8)
    , m_pszBuffer(nullptr)
{
    m_pszBuffer = new tCIDLib::TCh[m_c4BufChars + 1];
    m_pszBuffer[0] = chInit;
    m_pszBuffer[1] = kCIDLib::chNull;
}

TString::TStrBuf::TStrBuf(  const   tCIDLib::TCh* const pszInit
                            , const tCIDLib::TCard4     c4ExtraChars) :
    m_c4CurEnd(0)
    , m_c4BufChars(0)
    , m_pszBuffer(nullptr)
{
    //
    //  Watch for a null value and format the magic value for a null string.
    //  Else, just process it normally.
    //
    const tCIDLib::TCh* pszSrc = pszInit;
    if (!pszInit)
        pszSrc = kCIDLib::pszNullStr;

    m_c4CurEnd = TRawStr::c4StrLen(pszSrc);
    m_c4BufChars = m_c4CurEnd + c4ExtraChars;
    m_pszBuffer = new tCIDLib::TCh[m_c4BufChars + 1];
    TRawMem::CopyMemBuf
    (
        m_pszBuffer, pszSrc, m_c4CurEnd * kCIDLib::c4CharBytes
    );
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
}

TString::TStrBuf::TStrBuf(  const   TStrBuf&            strbInit
                            , const tCIDLib::TCard4     c4ExtraChars) :
    m_c4CurEnd(0)
    , m_c4BufChars(0)
    , m_pszBuffer(nullptr)
{
    m_c4CurEnd = strbInit.m_c4CurEnd;
    m_c4BufChars = strbInit.m_c4CurEnd + c4ExtraChars;
    m_pszBuffer = new tCIDLib::TCh[m_c4BufChars + 1];
    TRawMem::CopyMemBuf
    (
        m_pszBuffer
        , strbInit.m_pszBuffer
        , m_c4CurEnd * kCIDLib::c4CharBytes
    );
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
}

TString::TStrBuf::TStrBuf(          tCIDLib::TCh* const pszToAdopt
                            , const tCIDLib::EAdoptOpts eAdopt) :
    m_c4CurEnd(0)
    , m_c4BufChars(0)
    , m_pszBuffer(nullptr)
{
    //
    //  If its a null pointer, then be tolerant and just create another buffer
    //  of our own, which will be empty. Otherwise, just accept the passed
    //  buffer and either adopt or copy it.
    //
    if (!pszToAdopt)
    {
        m_c4BufChars = 8;
        m_pszBuffer = new tCIDLib::TCh[m_c4BufChars + 1];
        m_c4CurEnd = 0;
        *m_pszBuffer = kCIDLib::chNull;
    }
     else
    {
        m_c4CurEnd = TRawStr::c4StrLen(pszToAdopt);
        m_c4BufChars = m_c4CurEnd;

        if (eAdopt == tCIDLib::EAdoptOpts::Adopt)
            m_pszBuffer = pszToAdopt;
        else
            m_pszBuffer = TRawStr::pszReplicate(pszToAdopt);
    }
}


//
//  In this case (copy ctor), we have to be careful. If someone is using a
//  big buffer and then making a copy of that buffer and storing it away and
//  reusing the big buffer, that could cause gross overuse of memory because
//  every copy will be allocated to the size of the big buffer.
//
//  So, we use the current length for our allocation, and take the hit for
//  growth if needed. We'll make it at least a small size.
//
//  If they really want a copy to be a certain size, there are other ctors
//  above to achieve that.
//
TString::TStrBuf::TStrBuf(const TStrBuf& strbSrc) :

    m_c4CurEnd(strbSrc.m_c4CurEnd)
    , m_c4BufChars(strbSrc.m_c4CurEnd)
    , m_pszBuffer(nullptr)
{
    if (m_c4BufChars < 8)
        m_c4BufChars = 8;
    m_pszBuffer = new tCIDLib::TCh[m_c4BufChars + 1];
    if (m_c4CurEnd)
    {
        TRawMem::CopyMemBuf
        (
            m_pszBuffer, strbSrc.m_pszBuffer, m_c4CurEnd * kCIDLib::c4CharBytes
        );
    }
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
}


// We steal the cat objects buffer for ourself
TString::TStrBuf::TStrBuf(const TStrCat& scatSrc) :

    m_c4CurEnd(0)
    , m_c4BufChars(0)
    , m_pszBuffer(nullptr)
{
    tCIDLib::TCard4 c4SrcLen;
    m_pszBuffer = scatSrc.pszStealBuf(c4SrcLen);

    // If we get a used one, then do a 1 char string, else take the src len
    if (!m_pszBuffer)
    {
        m_c4CurEnd = 1;
        m_c4BufChars = 1;
        m_pszBuffer = new tCIDLib::TCh[1];
        *m_pszBuffer = kCIDLib::chNull;
    }
     else
    {
        m_c4BufChars = c4SrcLen;
        m_c4CurEnd = c4SrcLen;
    }
}

TString::TStrBuf::TStrBuf(TStrBuf&& strbSrc) :

    TString::TStrBuf()
{
    *this = tCIDLib::ForceMove(strbSrc);
}


TString::TStrBuf::~TStrBuf()
{
    try
    {
        delete [] m_pszBuffer;
    }

    catch(...)
    {
        m_pszBuffer = nullptr;
    }
}


// ---------------------------------------------------------------------------
//  TStrBuf: Public operators
// ---------------------------------------------------------------------------
TString::TStrBuf& TString::TStrBuf::operator=(const TString::TStrBuf& strbSrc)
{
    if (this != &strbSrc)
    {
        TArrayJanitor<tCIDLib::TCh> janOld(m_pszBuffer);
        m_pszBuffer = nullptr;

        m_c4CurEnd = strbSrc.m_c4CurEnd;
        m_c4BufChars = strbSrc.m_c4CurEnd;
        m_pszBuffer = new tCIDLib::TCh[m_c4BufChars + 1];

        if (m_c4CurEnd)
        {
            TRawMem::CopyMemBuf
            (
                m_pszBuffer, strbSrc.m_pszBuffer, m_c4CurEnd * kCIDLib::c4CharBytes
            );
        }
        m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;

    }
    return *this;
}

TString::TStrBuf& TString::TStrBuf::operator=(TString::TStrBuf&& strbSrc)
{
    if (this != &strbSrc)
    {
        tCIDLib::Swap(m_c4BufChars, strbSrc.m_c4BufChars);
        tCIDLib::Swap(m_c4CurEnd, strbSrc.m_c4CurEnd);
        tCIDLib::Swap(m_pszBuffer, strbSrc.m_pszBuffer);
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TStrBuf: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TString::TStrBuf::Append(const tCIDLib::TCh chToAppend, const tCIDLib::TCard4 c4Count)
{
    // See if we have enough to hold it, else expand. Maintain the current text
    if (m_c4CurEnd + c4Count > m_c4BufChars)
        Reallocate(m_c4CurEnd + c4Count + 16, kCIDLib::True);

    // And append the char and update the length
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        m_pszBuffer[m_c4CurEnd++] = chToAppend;

    m_pszBuffer[m_c4CurEnd] = 0;
}


tCIDLib::TVoid
TString::TStrBuf::Append(const  tCIDLib::TCh* const psz1
                        , const tCIDLib::TCh* const psz2
                        , const tCIDLib::TCh* const psz3)
{
    // If the source string is a null pointer, then make it the special value
    const tCIDLib::TCh* pszActual = psz1 ? psz1 : kCIDLib::pszNullStr;

    // Get the length of the strings to append
    const tCIDLib::TCard4 c4Len1 = TRawStr::c4StrLen(pszActual);
    const tCIDLib::TCard4 c4Len2 = psz2 ? TRawStr::c4StrLen(psz2) : 0;
    const tCIDLib::TCard4 c4Len3 = psz3 ? TRawStr::c4StrLen(psz3) : 0;

    // See if we have enough to hold it, else expand. Maintain the current text
    const tCIDLib::TCard4 c4TotalNew = c4Len1 + c4Len2 + c4Len3;
    if (m_c4CurEnd + c4TotalNew > m_c4BufChars)
        Reallocate(m_c4CurEnd + c4TotalNew, kCIDLib::True);

    // And copy the new strings in
    TRawMem::CopyMemBuf
    (
        &m_pszBuffer[m_c4CurEnd], pszActual, c4Len1 * kCIDLib::c4CharBytes
    );
    m_c4CurEnd += c4Len1;

    if (psz2)
    {
        TRawMem::CopyMemBuf
        (
            &m_pszBuffer[m_c4CurEnd], psz2, c4Len2 * kCIDLib::c4CharBytes
        );
       m_c4CurEnd += c4Len2;
    }

    if (psz3)
    {
        TRawMem::CopyMemBuf
        (
            &m_pszBuffer[m_c4CurEnd], psz3, c4Len3 * kCIDLib::c4CharBytes
        );
       m_c4CurEnd += c4Len3;
    }

    m_pszBuffer[m_c4CurEnd] = 0;
}


tCIDLib::TVoid TString::TStrBuf::Append(const TStrBuf& strbToAppend)
{
    //
    //  Calculate the buffer size we'll need. If the passed string is empty,
    //  then just bail out now.
    //
    const tCIDLib::TCard4 c4Needed = m_c4CurEnd + strbToAppend.m_c4CurEnd;
    if (c4Needed == m_c4CurEnd)
        return;

    // Reallocate if needed. Maintain the current content
    if (c4Needed > m_c4BufChars)
        Reallocate(c4Needed, kCIDLib::True);

    // Copy the source onto the end, getting the null terminator
    TRawMem::CopyMemBuf
    (
        &m_pszBuffer[m_c4CurEnd]
        , strbToAppend.m_pszBuffer
        , strbToAppend.m_c4CurEnd * kCIDLib::c4CharBytes
    );

    m_c4CurEnd = c4Needed;

    m_pszBuffer[m_c4CurEnd] = 0;
}


tCIDLib::TVoid
TString::TStrBuf::AppendSubStr( const   TStrBuf&        strbAppend
                                , const tCIDLib::TCard4 c4Start
                                , const tCIDLib::TCard4 c4Len)
{
    // Is the source string is empty, then just return cause nothing to do
    if (!strbAppend.m_c4CurEnd)
        return;

    // Make sure the start is within the current length of the source
    if (c4Start >= strbAppend.m_c4CurEnd)
    {
        //
        //  If it'as at the end, and the length is max card, just do nothing, since basically
        //  they are just telling us to copy what's available, and that's just nothing in
        //  this case. Else throw.
        //
        if ((c4Start == strbAppend.m_c4CurEnd) && (c4Len == kCIDLib::c4MaxCard))
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
            , TCardinal(strbAppend.m_c4CurEnd)
        );
    }

    //
    //  If the len is c4MaxCard, then set to exactly the rest of the source
    //  string.
    //
    tCIDLib::TCard4 c4ActualLen = c4Len;
    if (c4ActualLen == kCIDLib::c4MaxCard)
    {
        c4ActualLen = strbAppend.m_c4CurEnd - c4Start;
    }
     else
    {
        if (c4Start + c4ActualLen > strbAppend.m_c4CurEnd)
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
                , TCardinal(strbAppend.m_c4CurEnd)
            );
        }
    }

    // If we ended up with nothing to do, then return
    if (!c4ActualLen)
        return;

    //
    //  See if we need to reallocate. If so it will reallocate to the
    //  nearest next realloc increment thats equal to or greater than
    //  c4Actual. Maintain the current text content in the new buffer.
    //
    if (m_c4CurEnd + c4ActualLen > m_c4BufChars)
        Reallocate(m_c4CurEnd + c4ActualLen, kCIDLib::True);

    //
    //  Copy over the data. We multiply by the character size to convert
    //  chars to bytes.
    //
    TRawMem::CopyMemBuf
    (
        &m_pszBuffer[m_c4CurEnd]
        , &strbAppend.m_pszBuffer[c4Start]
        , c4ActualLen * kCIDLib::c4CharBytes
    );

    // And update the current length
    m_c4CurEnd += c4ActualLen;

    m_pszBuffer[m_c4CurEnd] = 0;
}


tCIDLib::TVoid
TString::TStrBuf::AppendSubStr( const   tCIDLib::TCh* const pszAppend
                                , const tCIDLib::TCard4     c4Start
                                , const tCIDLib::TCard4     c4Len)
{
    // The source cannot be a null pointer in this case
    if (!pszAppend)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_NullSrc
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
        );
    }

    //
    //  If the len is c4MaxCard, then set to exactly the rest of the source
    //  string.
    //
    tCIDLib::TCard4 c4ActualLen = c4Len;
    if (c4ActualLen == kCIDLib::c4MaxCard)
    {
        // Get the length of the text we are appending from
        const tCIDLib::TCard4 c4SrcLen = TRawStr::c4StrLen(pszAppend);
        c4ActualLen = c4SrcLen - c4Start;
    }

    // If we ended up with nothing to do, then return
    if (!c4ActualLen)
        return;

    //
    //  See if we need to reallocate. If so it will reallocate to the
    //  nearest next realloc increment thats equal to or greater than
    //  c4Actual. Maintain the current text content in the new buffer.
    //
    if (m_c4CurEnd + c4ActualLen > m_c4BufChars)
        Reallocate(m_c4CurEnd + c4ActualLen, kCIDLib::True);

    //
    //  Copy over the data. We multiply by the character size to convert
    //  chars to bytes.
    //
    TRawMem::CopyMemBuf
    (
        &m_pszBuffer[m_c4CurEnd]
        , &pszAppend[c4Start]
        , c4ActualLen * kCIDLib::c4CharBytes
    );

    // And update the current length
    m_c4CurEnd += c4ActualLen;

    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
}


tCIDLib::TBoolean TString::TStrBuf::bCapAtChar(const tCIDLib::TCh chAt)
{
    // Make sure we are null terminated first, then search for the string
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
    tCIDLib::TCh* pszMatch = TRawStr::pszFindChar
    (
        m_pszBuffer, chAt, 0, kCIDLib::True
    );

    // If not, then we are done
    if (!pszMatch)
        return kCIDLib::False;

    // Cap it at the match point
    *pszMatch = kCIDLib::chNull;

    // Update the new end point
    m_c4CurEnd = pszMatch - m_pszBuffer;

    return kCIDLib::True;
}


tCIDLib::TBoolean TString::TStrBuf::bIsAlpha() const
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurEnd; c4Index++)
    {
        if (!TRawStr::bIsAlpha(m_pszBuffer[c4Index]))
            return kCIDLib::False;
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean TString::TStrBuf::bIsAlphaNum() const
{
    for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurEnd; c4Index++)
    {
        if (!TRawStr::bIsAlphaNum(m_pszBuffer[c4Index]))
            return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TString::TStrBuf::bEqual(const TStrBuf& strbToComp) const
{
    // Can't be equal if not the same length
    if (m_c4CurEnd != strbToComp.m_c4CurEnd)
        return kCIDLib::False;

    // We know the lengths, so just do a memory compare now
    const tCIDLib::ESortComps eRes = TRawMem::eCompareMemBuf
    (
        m_pszBuffer
        , strbToComp.m_pszBuffer
        , m_c4CurEnd * kCIDLib::c4CharBytes
    );
    return (eRes == tCIDLib::ESortComps::Equal);
}


//
//  Check whether this string starts with the passed string, with optional
//  case sensitivity or insensitivity. We have two versions so that when
//  string objects are passed as the prefix to check we can do the fast
//  check for length and reject it immediately if too long.
//
tCIDLib::TBoolean
TString::TStrBuf::bCheckPrefix( const  tCIDLib::TCh* const  pszToCheck
                                , const tCIDLib::TBoolean   bCaseSensitive) const
{
    m_pszBuffer[m_c4CurEnd] = 0;
    const tCIDLib::TCh* pszUs = m_pszBuffer;
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
TString::TStrBuf::bCheckPrefix( const   TString::TStrBuf&   strbToCheck
                                , const tCIDLib::TBoolean   bCaseSensitive) const
{
    const tCIDLib::TCard4 c4SrcLen = strbToCheck.m_c4CurEnd;

    // Do the fast check, where this string is shorter
    if (m_c4CurEnd < c4SrcLen)
        return kCIDLib::False;

    // Oh well, check the strings
    m_pszBuffer[m_c4CurEnd] = 0;
    const tCIDLib::TCh* pszUs = m_pszBuffer;
    const tCIDLib::TCh* pszThem = strbToCheck.m_pszBuffer;

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
TString::TStrBuf::bCheckSuffix( const  tCIDLib::TCh* const  pszToCheck
                                , const tCIDLib::TBoolean   bCaseSensitive) const
{
    //
    //  Calc where we need to start comparing in our buffer. And since we
    //  have to do the source length, do the quick check to see if he is
    //  longer than us and therefore it could never be true, or where
    //  the suffix is empty.
    //
    const tCIDLib::TCard4 c4SrcLen = TRawStr::c4StrLen(pszToCheck);
    if (!c4SrcLen || (m_c4CurEnd < c4SrcLen))
        return kCIDLib::False;

    // Set up pointers to run up
    m_pszBuffer[m_c4CurEnd] = 0;
    const tCIDLib::TCh* pszUs = m_pszBuffer + (m_c4CurEnd - c4SrcLen);
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
TString::TStrBuf::bCheckSuffix( const   TString::TStrBuf&   strbToCheck
                                , const tCIDLib::TBoolean   bCaseSensitive) const
{
    const tCIDLib::TCard4 c4SrcLen = strbToCheck.m_c4CurEnd;

    //
    //  Do the fast check, where this string is shorter, or the suffix is
    //  empty.
    //
    if (!c4SrcLen || (m_c4CurEnd < c4SrcLen))
        return kCIDLib::False;

    // Oh well, check the strings
    m_pszBuffer[m_c4CurEnd] = 0;
    const tCIDLib::TCh* pszUs = m_pszBuffer + (m_c4CurEnd - c4SrcLen);
    const tCIDLib::TCh* pszThem = strbToCheck.m_pszBuffer;

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


// Replace the char and return whether we made any changes
tCIDLib::TBoolean
TString::TStrBuf::bReplaceChar( const   tCIDLib::TCh    chToReplace
                                , const tCIDLib::TCh    chReplaceWith)
{
    if (chToReplace)
    {
        //
        //  Loop through the string and replace any instances of the replacement
        //  character with the replacement character.
        //
        tCIDLib::TCard4 c4Count = 0;
        for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4CurEnd; c4Index++)
        {
            if (m_pszBuffer[c4Index] == chToReplace)
            {
                m_pszBuffer[c4Index] = chReplaceWith;
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


//
//  The const version of this is inlined for speed and only calls the
//  TestIndex method if in debug mode. This one always tests, because it
//  is a writeable return value and the results of bad index are much
//  worse if not caught.
//
tCIDLib::TCh& TString::TStrBuf::chAt(const tCIDLib::TCard4 c4Ind)
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
            , clsThis()
            , TCardinal(m_c4CurEnd)
        );
    }
    return m_pszBuffer[c4Ind];
}


tCIDLib::TCh TString::TStrBuf::chLast() const
{
    if (!m_c4CurEnd)
        return kCIDLib::chNull;
    return m_pszBuffer[m_c4CurEnd - 1];
}


tCIDLib::TVoid TString::TStrBuf::CapAt(const tCIDLib::TCard4 c4Index)
{
    //
    //  We'll let it be at the current end, which basically becomes a no-op,
    //  since its already capped there, but it makes things easier for client
    //  code, so they don't have to special case this.
    //
    if (c4Index <= m_c4CurEnd)
    {
        m_c4CurEnd = c4Index;
        m_pszBuffer[m_c4CurEnd] = 0;
        return;
    }

    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcGen_IndexError
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::BadParms
        , TCardinal(c4Index)
        , clsThis()
        , TCardinal(m_c4CurEnd)
    );
}


tCIDLib::TVoid TString::TStrBuf::Clear()
{
    m_c4CurEnd = 0;
    m_pszBuffer[m_c4CurEnd] = 0;
}


//
//  Cut from the indicated index for the indicated number of chars. If the length
//  is max card, then we go to the end of the string.
//
tCIDLib::TVoid
TString::TStrBuf::Cut(const tCIDLib::TCard4 c4Start, const tCIDLib::TCard4 c4Len)
{
    if (!c4Len)
        return;

    // We cannot start beyond the end
    if (c4Start >= m_c4CurEnd)
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
            , TCardinal(m_c4CurEnd)
        );
    }

    tCIDLib::TCard4 c4ActualLen = c4Len;
    if ((c4ActualLen == kCIDLib::c4MaxCard)
    ||  ((c4Start + c4ActualLen) == m_c4CurEnd))
    {
        m_c4CurEnd = c4Start;
        return;
    }
     else if ((c4Start + c4ActualLen) > m_c4CurEnd)
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
            , TCardinal(m_c4CurEnd)
        );
    }

    // Copy the text at c4Start+c4ActualLen down to c4Start
    tCIDLib::TCh* pszDest = &m_pszBuffer[c4Start];
    tCIDLib::TCh* pszSrc  = &m_pszBuffer[c4Start + c4ActualLen];
    tCIDLib::TCh* pszEnd  = &m_pszBuffer[m_c4CurEnd];
    while (pszSrc < pszEnd)
        *pszDest++ = *pszSrc++;

    // Adjust the end by the actual length
    m_c4CurEnd -= c4ActualLen;

    // And cap the string
    m_pszBuffer[m_c4CurEnd] = 0;
}


//
//  Look for the first instance of the indicated character and, if found, cut
//  from the start o the string to there.
//
tCIDLib::TVoid TString::TStrBuf::CutUpTo(const tCIDLib::TCh chFind)
{
    // Make sure we are capped and then search
    m_pszBuffer[m_c4CurEnd] = 0;
    tCIDLib::TCh* pszMatch = TRawStr::pszFindChar
    (
        m_pszBuffer, chFind, 0, kCIDLib::True
    );

    // If we got a match, then copy down the remainder
    if (pszMatch)
    {
        //
        //  We end the cut on the found character, so we subtract the two
        //  pointers and add one. Then just call the cut method.
        //
        const tCIDLib::TCard4 c4Len((pszMatch - m_pszBuffer) + 1);
        Cut(0, c4Len);
    }
}


// Delete the last character if we have one
tCIDLib::TVoid TString::TStrBuf::DeleteLast()
{
    // If already empty, then we are happy
    if (!m_c4CurEnd)
        return;

    // Just put a 0 in the last character and dec the length
    m_c4CurEnd--;

    m_pszBuffer[m_c4CurEnd] = 0;
}


//
//  Copyies in characters from a raw source string. It must be a null termed
//  string. It can be a null though, and a special string representing a
//  null will be used as the value. If the count is 0, then the actual length
//  of the string is used as the count.
//
tCIDLib::TVoid
TString::TStrBuf::FromZStr( const   tCIDLib::TCh* const pszSrc
                            , const tCIDLib::TCard4     c4Count)
{
    // If the source is a null pointer, then set it to the special value
    const tCIDLib::TCh* pszActual = pszSrc ? pszSrc : kCIDLib::pszNullStr;

    // Get the length of the actual source string
    const tCIDLib::TCard4 c4SrcLen = TRawStr::c4StrLen(pszActual);

    // Get the actual count, which if zero means the actual source length
    tCIDLib::TCard4 c4ActualCount = c4Count ? c4Count : c4SrcLen;

    // If the actual count is longer than the source string, an error
    if (c4ActualCount > c4SrcLen)
    {
        facCIDLib().ThrowErr
        (
             CID_FILE
             , CID_LINE
             , kCIDErrs::errcStr_CountOverLen
             , tCIDLib::ESeverities::Failed
             , tCIDLib::EErrClasses::AppError
             , TCardinal(c4ActualCount)
             , TCardinal(c4SrcLen)
        );
    }

    //
    //  If the actual count is bigger than our current buffer, reallocate.
    //  Tell it not to bother retaining the current contents, since we are
    //  going to just set a new value below.
    //
    if (c4ActualCount > m_c4BufChars)
        Reallocate(c4ActualCount + 16, kCIDLib::False);

    TRawMem::CopyMemBuf
    (
        m_pszBuffer
        , pszActual
        , c4ActualCount * kCIDLib::c4CharBytes
    );
    m_c4CurEnd = c4ActualCount;

    m_pszBuffer[m_c4CurEnd] = 0;
}


tCIDLib::TVoid
TString::TStrBuf::FormatToFld(  const   TStrBuf&            strbSrc
                                , const tCIDLib::TCard4     c4FldWidth
                                , const tCIDLib::EHJustify  eJustify
                                , const tCIDLib::TCh        chFill
                                , const tCIDLib::TCard4     c4TrailingSp)
{
    //
    //  If the field width is zero, then we will just make it fit the
    //  source string, so its an easy one.
    //
    if (!c4FldWidth)
    {
        Set(strbSrc);
        return;
    }

    //
    //  See if we need to reallocate. If so, it will reallocate up to the
    //  next realloc block size that is >= the passed actual. The second
    //  parm says don't bother preserving the text content.
    //
    if (c4FldWidth + c4TrailingSp > m_c4BufChars)
        Reallocate(c4FldWidth + c4TrailingSp, kCIDLib::False);

    strbSrc.m_pszBuffer[strbSrc.m_c4CurEnd] = kCIDLib::chNull;
    if (!TRawStr::bFormatStr(strbSrc.m_pszBuffer
                            , m_pszBuffer
                            , c4FldWidth
                            , chFill
                            , eJustify))
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

    // Update the current length to the width of the field
    m_c4CurEnd = c4FldWidth;

    // Make sure we got the expansion right
    CIDAssert
    (
        m_c4CurEnd + c4TrailingSp <= m_c4BufChars, L"Not enough room for trailing spaces"
    );

    // Add any trailing spaces
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4TrailingSp; c4Index++)
        m_pszBuffer[m_c4CurEnd++] = kCIDLib::chSpace;

    m_pszBuffer[m_c4CurEnd] = 0;
}


//
//  Unlike FromZStr above, this one doesn't assume any sort of null term
//  on the source. So it takes the source count as a literal value and
//  won't take 0 as a 'use all the source chars' indicator like FromZStr.
//
tCIDLib::TVoid
TString::TStrBuf::ImportChars(  const   tCIDLib::TCh* const pszSrc
                                , const tCIDLib::TCard4     c4SrcCount)
{
    //
    //  If the actual count is bigger than our current buffer, reallocate.
    //  Tell it not to bother retaining the current contents, since we are
    //  going to just set a new value below.
    //
    if (c4SrcCount > m_c4BufChars)
        Reallocate(c4SrcCount + 16, kCIDLib::False);

    TRawMem::CopyMemBuf
    (
        m_pszBuffer, pszSrc, c4SrcCount * kCIDLib::c4CharBytes
    );
    m_c4CurEnd = c4SrcCount;

    m_pszBuffer[m_c4CurEnd] = 0;
}


tCIDLib::TVoid TString::TStrBuf::Insert(const   tCIDLib::TCh* const pszInsert
                                        , const tCIDLib::TCard4     c4Ind)
{
    //
    //  Make sure that the insert index is valid. Note that, in this case, we
    //  allow it to fall on the terminating null, so that an insert at the end
    //  will be like an append. Otherwise, you could not insert after the last
    //  char, because the index is the 'insert after' index.
    //
    if (c4Ind > m_c4CurEnd)
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
            , TCardinal(m_c4CurEnd)
        );
    }

    // If the source is a null pointer, then set it to the special Null value
    const tCIDLib::TCh* pszActual = pszInsert ? pszInsert : kCIDLib::pszNullStr;

    // Get the length of the string we are going to insert
    const tCIDLib::TCard4 c4InsertLen = TRawStr::c4StrLen(pszActual);

    // Do a quick sanity check to see if we need to do anything
    if (!c4InsertLen)
        return;

    // If the insert point is at the end, then we just append
    if (c4Ind == m_c4CurEnd)
    {
        Append(pszActual, 0, 0);
        return;
    }

    // Extend the string if we are going to need to. Retain current content
    if (m_c4CurEnd + c4InsertLen > m_c4BufChars)
        Reallocate(m_c4CurEnd + c4InsertLen, kCIDLib::True);

    //
    //  Lets move the text after the insert point upwards to make room for
    //  the insert text.
    //
    tCIDLib::TCh* pszSrc = &m_pszBuffer[m_c4CurEnd - 1];
    tCIDLib::TCh* pszDest = &m_pszBuffer[(m_c4CurEnd - 1) + c4InsertLen];
    while (pszSrc >= &m_pszBuffer[c4Ind])
        *pszDest-- = *pszSrc--;

    // And now move the insert text into the section left open
    TRawMem::CopyMemBuf
    (
        &m_pszBuffer[c4Ind]
        , pszActual
        , c4InsertLen * kCIDLib::c4CharBytes
    );

    // Adjust the current end up by the insert length
    m_c4CurEnd += c4InsertLen;

    m_pszBuffer[m_c4CurEnd] = 0;
}


tCIDLib::TVoid TString::TStrBuf::Insert(const   TStrBuf&        strbSrc
                                        , const tCIDLib::TCard4 c4Ind)
{
    //
    //  Make sure that the insert index is valid. Note that, in this case, we
    //  allow it to fall on the terminating null, so that an insert at the end
    //  will be like an append. Otherwise, you could not insert after the last
    //  char, because the index is the 'insert after' index.
    //
    if (c4Ind > m_c4CurEnd)
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
            , TCardinal(m_c4CurEnd)
        );
    }

    // Do a quick sanity check to see if we need to do anything
    if (!strbSrc.m_c4CurEnd)
        return;

    // If the insert point is at the end, then we just append
    if (c4Ind == m_c4CurEnd)
    {
        Append(strbSrc);
        return;
    }

    // Extend the string if we are going to need to. Retain current content
    if (m_c4CurEnd + strbSrc.m_c4CurEnd > m_c4BufChars)
        Reallocate(m_c4CurEnd + strbSrc.m_c4CurEnd, kCIDLib::True);

    //
    //  Lets move the text after the insert point upwards to make room for
    //  the insert text.
    //
    tCIDLib::TCh* pszSrc = &m_pszBuffer[m_c4CurEnd - 1];
    tCIDLib::TCh* pszDest = &m_pszBuffer[(m_c4CurEnd - 1) + strbSrc.m_c4CurEnd];
    while (pszSrc >= &m_pszBuffer[c4Ind])
        *pszDest-- = *pszSrc--;

    // And now move the insert text into the section left open
    TRawMem::CopyMemBuf
    (
        &m_pszBuffer[c4Ind]
        , strbSrc.m_pszBuffer
        , strbSrc.m_c4CurEnd * kCIDLib::c4CharBytes
    );

    // Adjust the current end up by the insert length
    m_c4CurEnd += strbSrc.m_c4CurEnd;

    m_pszBuffer[m_c4CurEnd] = 0;
}


tCIDLib::TCh* TString::TStrBuf::pszDupBuffer() const
{
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
    tCIDLib::TCh* pszRet = new tCIDLib::TCh[m_c4CurEnd + 1];
    TRawMem::CopyMemBuf
    (
        pszRet
        , m_pszBuffer
        , (m_c4CurEnd + 1) * kCIDLib::c4CharBytes
    );
    return pszRet;
}


// Provide access to the buffer at the start or a given offset
const tCIDLib::TCh* TString::TStrBuf::pszBuffer() const
{
    // Make sure its null terminated
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
    return m_pszBuffer;
}

const tCIDLib::TCh*
TString::TStrBuf::pszBufferAt(const tCIDLib::TCard4 c4At) const
{
    // We do allow them to point at the null terminator, so its lt or equal
    if (c4At <= m_c4CurEnd)
    {
        // Make sure its null terminated
        m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
        return &m_pszBuffer[c4At];
    }

    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcGen_IndexError
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::BadParms
        , TCardinal(c4At)
        , clsThis()
        , TCardinal(m_c4CurEnd)
    );
    return 0;
}


tCIDLib::TVoid TString::TStrBuf::PutAt( const   tCIDLib::TCard4 c4Index
                                        , const tCIDLib::TCh    chToPut)
{
    if (c4Index < m_c4CurEnd)
    {
        //
        //  If the character is a null, then this is a truncation operation, so we
        //  must adjust our current end.
        //
        if (!chToPut)
            m_c4CurEnd = c4Index;

        // Stick the character in
        m_pszBuffer[c4Index] = chToPut;
        return;
    }

    facCIDLib().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCIDErrs::errcGen_IndexError
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::BadParms
        , TCardinal(c4Index)
        , clsThis()
        , TCardinal(m_c4CurEnd)
    );
}


tCIDLib::TVoid
TString::TStrBuf::Reallocate(const  tCIDLib::TCard4     c4NewSize
                            , const tCIDLib::TBoolean   bPreserve)
{
    // If the same, then nothing to do
    if (c4NewSize == m_c4BufChars)
        return;

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

    //
    //  Reallocate the buffer. If the new size is not at least one realloc
    //  block larger, then make it one realloc block larger anyway.
    //
    tCIDLib::TCard4 c4Actual = c4NewSize;
    if (m_c4BufChars + CIDLib_String::c4ReallocBlock > c4NewSize)
        c4Actual = m_c4BufChars + CIDLib_String::c4ReallocBlock;

    //
    //  Save the old buffer pointer and put a janitor on it so that it will
    //  get cleaned up. zero the current buffer so we give up control.
    //
    tCIDLib::TCh* pszOldBuffer = m_pszBuffer;
    TArrayJanitor<tCIDLib::TCh> janOld(pszOldBuffer);
    m_pszBuffer = 0;

    // Now reallocate the new buffer
    m_pszBuffer = new tCIDLib::TCh[c4Actual + 1];

    //
    //  If we are preserving, then copy the old content. Else, just set the
    //  end to zero.
    //
    if (bPreserve && m_c4CurEnd)
    {
        TRawMem::CopyMemBuf
        (
            m_pszBuffer
            , pszOldBuffer
            , m_c4CurEnd * kCIDLib::c4CharBytes
        );
    }
     else
    {
        m_c4CurEnd = 0;
    }

    // Save the new current buffer size
    m_c4BufChars = c4Actual;

    // And cap it off
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
}


tCIDLib::TVoid
TString::TStrBuf::Replace(  const   tCIDLib::TCard4     c4RepStart
                            , const tCIDLib::TCard4     c4RepEnd
                            , const tCIDLib::TCh* const pszRepText
                            , const tCIDLib::TCard4     c4RepLen)
{
    //
    //  Get the rep text len. If the len is zero, then the caller doesn't know
    //  so we figure it out.
    //
    const tCIDLib::TCard4 c4ActualLen = c4RepLen ? c4RepLen
                                                 : TRawStr::c4StrLen(pszRepText);

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

    if (c4RepEnd > m_c4CurEnd)
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
            , TCardinal(m_c4CurEnd)
        );
    }

    const tCIDLib::TCard4 c4TargetLen = (c4RepEnd - c4RepStart) + 1;

    //
    //  Lets first test for the optimum scenario, where the replacement token
    //  takes exactly the same chars as what is being replaced. In this case,
    //  we can just copy it directly over.
    //
    //  The other two scenarios are that the replacement text is bigger than
    //  the target area, and when its smaller than the target area.
    //
    if (c4TargetLen == c4ActualLen)
    {
        TRawMem::CopyMemBuf
        (
            &m_pszBuffer[c4RepStart]
            , pszRepText
            , c4ActualLen * kCIDLib::c4CharBytes
        );
    }
     else if (c4TargetLen > c4ActualLen)
    {
        //
        //  The target is bigger that the replacement text. So we can copy
        //  the replace in, then copy down the remainder.
        //
        TRawMem::CopyMemBuf
        (
            &m_pszBuffer[c4RepStart]
            , pszRepText
            , c4ActualLen * kCIDLib::c4CharBytes
        );

        //
        //  Copy down any text from the replacement end point to the end
        //  of the replacement text we just copied in.
        //
        m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
        tCIDLib::TCh* pszTarget = &m_pszBuffer[c4RepStart + c4ActualLen];
        tCIDLib::TCh* pszSrc = &m_pszBuffer[c4RepStart + c4TargetLen];

        while (*pszSrc)
            *pszTarget++ = *pszSrc++;

        // Recalc the length of the string
        m_c4CurEnd -= c4TargetLen - c4ActualLen;
    }
     else
    {
        //
        //  The replacement text is bigger than the target. So we need to
        //  move up the text from the end to make room for it. We have to
        //  deal with expanding the buffer if it won't hold the text.
        //
        if ((m_c4CurEnd - c4TargetLen) + c4ActualLen > m_c4BufChars)
        {
            //
            //  We do have to expand. So we take advantage of that to just
            //  steal the current buffer, allocate a new one, then copy over
            //  form the old to the new.
            //
            tCIDLib::TCh* pszOld = m_pszBuffer;
            TArrayJanitor<tCIDLib::TCh> janOld(pszOld);
            m_c4BufChars = m_c4CurEnd + c4ActualLen;
            m_pszBuffer = new tCIDLib::TCh[m_c4BufChars + 1];

            TRawMem::CopyMemBuf
            (
                m_pszBuffer
                , pszOld
                , c4RepStart * kCIDLib::c4CharBytes
            );

            TRawMem::CopyMemBuf
            (
                &m_pszBuffer[c4RepStart]
                , pszRepText
                , c4ActualLen * kCIDLib::c4CharBytes
            );

            TRawMem::CopyMemBuf
            (
                &m_pszBuffer[c4RepStart + c4ActualLen]
                , &pszOld[c4RepEnd + 1]
                , (m_c4CurEnd - c4RepEnd) * kCIDLib::c4CharBytes
            );
        }
         else
        {
            //
            //  The buffer is big enough, so we need to move up the trailing
            //  text to make room for the replacement text. First we calc how
            //  many chars we have to move it up. Its the rep text length
            //  minus the target length.
            //
            const tCIDLib::TCard4 c4ExpCount = c4ActualLen - c4TargetLen;
            tCIDLib::TCh* pszTarget = &m_pszBuffer[m_c4CurEnd + c4ExpCount];
            tCIDLib::TCh* pszSrc = &m_pszBuffer[m_c4CurEnd];
            while (pszSrc > (m_pszBuffer + c4RepEnd))
                *pszTarget-- = *pszSrc--;

            // Copy the replacement text into place now
            TRawMem::CopyMemBuf
            (
                &m_pszBuffer[c4RepStart]
                , pszRepText
                , c4ActualLen * kCIDLib::c4CharBytes
            );
        }

        // Recalculate the length
        m_c4CurEnd += c4ActualLen - c4TargetLen;
    }

    m_pszBuffer[m_c4CurEnd] = 0;
}


tCIDLib::TVoid TString::TStrBuf::Set(const tCIDLib::TCh* const pszSrc)
{
    // If the source string is the same address as our buffer, then return
    if (pszSrc == m_pszBuffer)
        return;

    // If the source string is a null pointer, then set it to the special value
    const tCIDLib::TCh* pszActual = pszSrc ? pszSrc : kCIDLib::pszNullStr;

    // If the source string is within our buffer, then an error
    if ((pszActual > m_pszBuffer) && (pszActual < m_pszBuffer+m_c4BufChars))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_SourceInDest
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // Get the new length
    const tCIDLib::TCard4 c4SrcLen = TRawStr::c4StrLen(pszActual);

    //
    //  If its larger than our current buffer, then reallocate. The last parm
    //  says don't maintain current content
    //
    if (c4SrcLen > m_c4BufChars)
        Reallocate(c4SrcLen, kCIDLib::False);

    TRawMem::CopyMemBuf
    (
        m_pszBuffer, pszActual, c4SrcLen * kCIDLib::c4CharBytes
    );

    // And be sure to update the new end
    m_c4CurEnd = c4SrcLen;

    m_pszBuffer[m_c4CurEnd] = 0;
}


tCIDLib::TVoid
TString::TStrBuf::Set(const TStrBuf& strbNewValue)
{
    // If the source buffer is the same address as our buffer, then return
    if (this == &strbNewValue)
        return;

    //
    //  If its larger than our current buffer, then reallocate. Don't maintain
    //  previous contents.
    //
    if (strbNewValue.m_c4CurEnd > m_c4BufChars)
        Reallocate(strbNewValue.m_c4CurEnd, kCIDLib::False);

    TRawMem::CopyMemBuf
    (
        m_pszBuffer
        , strbNewValue.m_pszBuffer
        , strbNewValue.m_c4CurEnd * kCIDLib::c4CharBytes
    );

    // And be sure to update the new end
    m_c4CurEnd = strbNewValue.m_c4CurEnd;

    m_pszBuffer[m_c4CurEnd] = 0;
}


// Set us from a short string, which we have to convert
tCIDLib::TVoid TString::TStrBuf::SetFromShort(const tCIDLib::TSCh* const pszSrc)
{
    if (m_pszBuffer)
    {
        delete [] m_pszBuffer;
        m_pszBuffer = 0;
    }
    m_pszBuffer = TRawStr::pszConvert(pszSrc);
    m_c4CurEnd = TRawStr::c4StrLen(m_pszBuffer);
    m_c4BufChars = m_c4CurEnd;
}


tCIDLib::TVoid TString::TStrBuf::SetLast(const tCIDLib::TCh chNew)
{
    if (!m_c4CurEnd)
        return;

    // Just put the new char in the last character
    m_pszBuffer[m_c4CurEnd - 1] = chNew;

    // If the new character is a null, then adjust the length
    if (!chNew)
        m_c4CurEnd--;
}


tCIDLib::TVoid TString::TStrBuf::StreamFrom(TBinInStream& strmToReadFrom)
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
        //  If the cur buffer size is smaller than the stored one, then we
        //  have to reallocate. The second parm says  don't preserve current
        //  content. This will set m_c4BufChars to the new buffer size, and
        //  will avoid possibly lots of reallocations of the buffer.
        //
        if (c4CharsStored > m_c4BufChars)
            Reallocate(c4CharsStored, kCIDLib::False);

        //
        //  Only stream in new chars if we actually stored any chars during
        //  the out, which we might not have. We call the standard stream
        //  method to do this, since we use its outgoing counterpart to write
        //  the data out. It converts it from the 'on the wire' canonical
        //  format to the internal wide char format.
        //
        if (c4CharsStored)
        {
            strmToReadFrom.ReadArray(m_pszBuffer, c4CharsStored);

            // We don't really need this. Its just for debuggin
            m_pszBuffer[c4CharsStored] = kCIDLib::chNull;
        }

        // And it should end with an end object marker
        strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);

        // And now update our current char count since it seemed to have worked
        m_c4CurEnd = c4CharsStored;
        m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);

        // Don't leave junk in the string
        m_c4CurEnd = 0;
        m_pszBuffer[0] = kCIDLib::chNull;
        throw;
    }
}


tCIDLib::TVoid TString::TStrBuf::StreamTo(TBinOutStream& strmToWriteTo) const
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
                    << m_c4CurEnd;

    //
    //  If any chars to write, then stream them out. The output stream provides
    //  a method to do this. It converts them to a canonical format for
    //  streaming, so always use this to write char arrays.
    //
    if (m_c4CurEnd)
        strmToWriteTo.WriteArray(m_pszBuffer, m_c4CurEnd);

    // And finish off with an end object marker
    strmToWriteTo << tCIDLib::EStreamMarkers::EndObject;
}


tCIDLib::TVoid
TString::TStrBuf::Strip(const   tCIDLib::TCh* const     pszStripChars
                        , const tCIDLib::EStripModes    eMode
                        , const tCIDLib::TCh            chRepChar)
{
    m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
    TRawStr::StripStr(m_pszBuffer, pszStripChars, eMode, chRepChar);
    m_c4CurEnd = TRawStr::c4StrLen(m_pszBuffer);

    m_pszBuffer[m_c4CurEnd] = 0;
}


//
//  We have this separate method so that some inlined methods can do index
//  checking in debug builds (without imposing this code on every call site.)
//
tCIDLib::TVoid TString::TStrBuf::TestIndex(const tCIDLib::TCard4 c4ToTest) const
{
    if (c4ToTest >= m_c4CurEnd)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_IndexError
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(c4ToTest)
            , clsThis()
            , TCardinal(m_c4CurEnd)
        );
    }
}


tCIDLib::TVoid TString::TStrBuf::ToLower(const  tCIDLib::TCard4 c4StartInd
                                        , const tCIDLib::TCard4 c4Len)
{
    tCIDLib::TCard4 c4Actual;

    //
    //  If the length is max card, then we want to just make it long enough
    //  to get to the end of the string.
    //
    c4Actual = c4Len;
    if (c4Actual == kCIDLib::c4MaxCard)
        c4Actual = m_c4CurEnd - c4StartInd;

    // If the start plus the actual is too big, then an error
    if (c4StartInd + c4Actual > m_c4CurEnd)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_Range
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(c4StartInd)
            , TCardinal(c4Actual)
            , TCardinal(m_c4CurEnd)
        );
    }

    // If the length would take us to the end of the string, then do it directly
    if (c4StartInd + c4Actual >= m_c4CurEnd)
    {
        m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
        TRawStr::pszLowerCase(&m_pszBuffer[c4StartInd]);
        return;
    }
     else
    {
        // Else, we have to use a loop
        tCIDLib::TCard4 c4Ind;
        for (c4Ind = c4StartInd; c4Ind < c4StartInd + c4Actual; c4Ind++)
            m_pszBuffer[c4Ind] = TRawStr::chLower(m_pszBuffer[c4Ind]);
    }
}


tCIDLib::TVoid TString::TStrBuf::ToUpper(const  tCIDLib::TCard4 c4StartInd
                                        , const tCIDLib::TCard4 c4Len)
{
    tCIDLib::TCard4 c4Actual;

    //
    //  If the length is max card, then we want to just make it long enough
    //  to get to the end of the string.
    //
    c4Actual = c4Len;
    if (c4Actual == kCIDLib::c4MaxCard)
        c4Actual = m_c4CurEnd - c4StartInd;

    // If the start plus the actual is too big, then an error
    if (c4StartInd + c4Actual > m_c4CurEnd)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStr_Range
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::OutResource
            , TCardinal(c4StartInd)
            , TCardinal(c4Actual)
            , TCardinal(m_c4CurEnd)
        );
    }

    // If the length would take us to the end of the string, then do it directly
    if (c4StartInd + c4Actual >= m_c4CurEnd)
    {
        m_pszBuffer[m_c4CurEnd] = kCIDLib::chNull;
        TRawStr::pszUpperCase(&m_pszBuffer[c4StartInd]);
        return;
    }
     else
    {
        // Else, we have to use a loop
        tCIDLib::TCard4 c4Ind;
        for (c4Ind = c4StartInd; c4Ind < c4StartInd + c4Actual; c4Ind++)
            m_pszBuffer[c4Ind] = TRawStr::chUpper(m_pszBuffer[c4Ind]);
    }
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
TString::TStrBuf::ToZStr(       tCIDLib::TCh* const pszTarget
                        , const tCIDLib::TCard4     c4MaxChars
                        , const tCIDLib::TCard4     c4StartInd
                        , const tCIDLib::TBoolean   bAddNull) const
{
    #if CID_DEBUG_ON
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
    #endif

    // Clear the target zstring
    pszTarget[0] = kCIDLib::chNull;

    // If no chars in this string, then we are done
    if (!m_c4CurEnd)
        return;

    // If start index is at end of string, then we are done
    if (c4StartInd == m_c4CurEnd)
        return;

    // If the start index is >= than the length of this string, then an error
    if (c4StartInd >= m_c4CurEnd)
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
            , TCardinal(m_c4CurEnd)
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
        tCIDLib::MinVal(c4MaxChars, m_c4CurEnd - c4StartInd)
    );
    TRawMem::CopyMemBuf
    (
        pszTarget
        , &m_pszBuffer[c4StartInd]
        , c4ToCopy * kCIDLib::c4CharBytes
    );

    // Cap off the target appropriately if asked to
    if (bAddNull)
        pszTarget[c4ToCopy] = kCIDLib::chNull;
}




// ---------------------------------------------------------------------------
//   CLASS: TString
//  PREFIX: str
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TString: Public, static data
// ---------------------------------------------------------------------------
const TString& TString::strEmpty()
{
    static TString* pstrEmpty = 0;
    if (!pstrEmpty)
    {
        TBaseLock lockInit;
        if (!pstrEmpty)
            TRawMem::pExchangePtr(&pstrEmpty, new TString);
    }
    return *pstrEmpty;
}


// ---------------------------------------------------------------------------
//  TString: Public, static methods
// ---------------------------------------------------------------------------
TString& TString::Nul_TString()
{
    static TString* pstrNull = nullptr;

    if (!pstrNull)
    {
        TBaseLock lockInit;
        if (!pstrNull)
            TRawMem::pExchangePtr(&pstrNull, new TString);
    }
    return *pstrNull;
}


// ---------------------------------------------------------------------------
//  TString: Constructors and Destructor
// ---------------------------------------------------------------------------
TString::TString()
{
}

TString::TString(const tCIDLib::TCard4 c4BufSize) :

    m_strbData(c4BufSize)
{
}

TString::TString(const tCIDLib::TCh chInit) :

    m_strbData(chInit)
{
}

TString::TString(tCIDLib::TCh* pszToAdopt, const tCIDLib::EAdoptOpts eAdopt) :

    m_strbData(pszToAdopt, eAdopt)
{
}

TString::TString(const  tCIDLib::TCh*   pszSrc
                , const tCIDLib::TCard4 c4ExtraChars) :

    m_strbData(pszSrc, c4ExtraChars)
{
}

TString::TString(const tCIDLib::TCh* const pszSrc) :

    m_strbData(pszSrc, 8)
{
}

TString::TString(const  TString&        strSrc
                , const tCIDLib::TCard4 c4ExtraChars) :

    m_strbData(strSrc.m_strbData, c4ExtraChars)
{
}

TString::TString(const tCIDLib::TErrCode errcToLoad, const TFacility& facSrc) :

    m_strbData(facSrc.pszLoadCIDMsg(errcToLoad), 64)
{
}

TString::TString(const  tCIDLib::TErrCode   errcToLoad
                , const TFacility&          facSrc
                , const MFormattable&       fmtblToken1
                , const MFormattable&       fmtblToken2
                , const MFormattable&       fmtblToken3
                , const MFormattable&       fmtblToken4) :

    m_strbData(1024UL)
{
    tCIDLib::TBoolean bLoaded;
    m_strbData.Set(facSrc.pszLoadCIDMsg(errcToLoad, bLoaded));

    // If we loaded it ok, then do token replacement
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

TString::TString(const  MFormattable&   fmtblInitValue
                , const TStreamFmt&     strmfToUse) :

    m_strbData(1024UL)
{
    //
    //  Create a string based text stream that we can use to format the
    //  passed object into. We tell it to format into us, but not to adopt
    //  us. We pass on the stream format object. If its a nul object,
    //  that is ok because the string stream will just then ignore it.
    //
    TTextStringOutStream strmTmp(this, strmfToUse, tCIDLib::EAdoptOpts::NoAdopt);

    //
    //  Now we just format the value into ourself, being sure to flush it
    //  before we exit so that the stream will dump its cache into our
    //  buffer.
    //
    strmTmp << fmtblInitValue << kCIDLib::FlushIt;
}

TString::TString(const MFormattable& fmtblInitValue) :

    m_strbData(1024UL)
{
    //
    //  Create a string base text stream that we can use to format the
    //  passed object into. We tell it to format into us, but not to adopt
    //  us. We pass on the stream format object. If its a nul reference,
    //  that is ok because the string stream will just then ignore it.
    //
    TTextStringOutStream strmTmp(this, tCIDLib::EAdoptOpts::NoAdopt);

    //
    //  Now we just format the value into ourself, being sure to flush it
    //  before we exit so that the stream will dump its cache into our
    //  buffer.
    //
    strmTmp << fmtblInitValue << kCIDLib::FlushIt;
}


// Create a string from a pattern with replacement tokens

TString::TString(const  TString&        strPattern
                , const MFormattable&   fmtblToken1
                , const MFormattable&   fmtblToken2
                , const MFormattable&   fmtblToken3
                , const MFormattable&   fmtblToken4) :

    m_strbData(strPattern.m_strbData, 32UL)
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

    m_strbData(pszPattern, 32UL)
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

    m_strbData(TRawStr::c4StrLen(pszInit))
{
    //
    //  Check for a null initial value. If it is, then set it to the special
    //  null string. Otherwise, convert the init string to internal format and
    //  make the new copy the buffer.
    //
    if (!pszInit)
        m_strbData.Set(kCIDLib::pszNullStr);
    else
        m_strbData.SetFromShort(pszInit);
}


//
//  We steal his buffer and adopt it. The string buffer object will deal with the
//  possibility that we get a used one (already nulled.)
//
TString::TString(const TStrCat& scatSrc) :

    m_strbData(scatSrc)
{
}


TString::TString(const TString& strSrc) :

    m_strbData(strSrc.m_strbData)
{
}

// We have no members, other than that the buffer, so just do a forced move on that
TString::TString(TString&& strSrc) :

    m_strbData(tCIDLib::ForceMove(strSrc.m_strbData))
{
}

TString::~TString()
{
}


// ---------------------------------------------------------------------------
//  TString: Public operators
// ---------------------------------------------------------------------------
TString& TString::operator=(const tCIDLib::TCh* const pszSrc)
{
    m_strbData.Set(pszSrc);
    return *this;
}

TString& TString::operator=(const TString& strSrc)
{
    if (this != &strSrc)
        m_strbData.Set(strSrc.m_strbData);
    return *this;
}

TString& TString::operator=(TString&& strSrc)
{
    if (this != &strSrc)
        m_strbData = tCIDLib::ForceMove(strSrc.m_strbData);
    return *this;
}


tCIDLib::TBoolean TString::operator==(const TString& strToCompare) const
{
    if (&m_strbData == &strToCompare.m_strbData)
        return kCIDLib::True;
    return m_strbData.bEqual(strToCompare.m_strbData);
}

tCIDLib::TBoolean
TString::operator==(const tCIDLib::TCh* const pszToCompare) const
{
    return (TRawStr::eCompareStr(m_strbData.pszBuffer(), pszToCompare) == tCIDLib::ESortComps::Equal);
}

tCIDLib::TBoolean TString::operator!=(const TString& strToCompare) const
{
    if (&m_strbData == &strToCompare.m_strbData)
        return kCIDLib::False;
    return !m_strbData.bEqual(strToCompare.m_strbData);
}

tCIDLib::TBoolean
TString::operator!=(const tCIDLib::TCh* const pszToCompare) const
{
    return (TRawStr::eCompareStr(m_strbData.pszBuffer(), pszToCompare) != tCIDLib::ESortComps::Equal);
}

tCIDLib::TBoolean
TString::operator!=(const tCIDLib::TSCh* const pszToCompare) const
{
    return !operator==(pszToCompare);
}

tCIDLib::TCh& TString::operator[](const tCIDLib::TCard4 c4Ind)
{
    return m_strbData.chAt(c4Ind);
}

tCIDLib::TCh TString::operator[](const tCIDLib::TCard4 c4Ind) const
{
    return m_strbData.chAt(c4Ind);
}


tCIDLib::TBoolean
TString::operator==(const tCIDLib::TSCh* const pszToCompare) const
{
    // Convert the short string so we can compare it
    tCIDLib::TCh* pszConverted = TRawStr::pszConvert(pszToCompare);
    TArrayJanitor<tCIDLib::TCh> janConverted(pszConverted);
    return (TRawStr::eCompareStr(m_strbData.pszBuffer(), pszConverted) == tCIDLib::ESortComps::Equal);
}


tCIDLib::TBoolean TString::operator<=(const TString& strToCompare) const
{
    const tCIDLib::ESortComps eComp = TRawStr::eCompareStr
    (
        m_strbData.pszBuffer(), strToCompare.m_strbData.pszBuffer()
    );
    return (eComp == tCIDLib::ESortComps::FirstLess) ||(eComp == tCIDLib::ESortComps::Equal);
}

tCIDLib::TBoolean TString::operator>=(const TString& strToCompare) const
{
    const tCIDLib::ESortComps eComp = TRawStr::eCompareStr
    (
        m_strbData.pszBuffer(), strToCompare.m_strbData.pszBuffer()
    );
    return (eComp == tCIDLib::ESortComps::FirstGreater)
            || (eComp == tCIDLib::ESortComps::Equal);
}


tCIDLib::TBoolean
TString::operator<=(const tCIDLib::TCh* const pszToCompare) const
{
    const tCIDLib::ESortComps eComp = TRawStr::eCompareStr
    (
        m_strbData.pszBuffer(), pszToCompare
    );
    return (eComp == tCIDLib::ESortComps::FirstLess) ||(eComp == tCIDLib::ESortComps::Equal);
}

tCIDLib::TBoolean
TString::operator>=(const tCIDLib::TCh* const pszToCompare) const
{
    const tCIDLib::ESortComps eComp = TRawStr::eCompareStr
    (
        m_strbData.pszBuffer(), pszToCompare
    );
    return (eComp == tCIDLib::ESortComps::FirstGreater)
            || (eComp == tCIDLib::ESortComps::Equal);
}


tCIDLib::TBoolean TString::operator<(const TString& strToCompare) const
{
    return TRawStr::eCompareStr
    (
        m_strbData.pszBuffer(), strToCompare.m_strbData.pszBuffer()
    ) == tCIDLib::ESortComps::FirstLess;
}

tCIDLib::TBoolean TString::operator>(const TString& strToCompare) const
{
    return TRawStr::eCompareStr
    (
        m_strbData.pszBuffer(), strToCompare.m_strbData.pszBuffer()
    ) == tCIDLib::ESortComps::FirstGreater;
}

tCIDLib::TBoolean
TString::operator<(const tCIDLib::TCh* const pszToCompare) const
{
    return TRawStr::eCompareStr
    (
        m_strbData.pszBuffer(), pszToCompare
    ) == tCIDLib::ESortComps::FirstLess;
}

tCIDLib::TBoolean
TString::operator>(const tCIDLib::TCh* const pszToCompare) const
{
    return TRawStr::eCompareStr
    (
        m_strbData.pszBuffer(), pszToCompare
    ) == tCIDLib::ESortComps::FirstGreater;
}


// ---------------------------------------------------------------------------
//  TString: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TString::Append(const TString&  strSrc1, const TString&  strSrc2)
{
    m_strbData.Append(strSrc1.m_strbData);
    m_strbData.Append(strSrc2.m_strbData);
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
                            , c4MaxBufChars(szTmp)
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
    m_strbData.Append(szTmp, 0, 0);
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
                            , c4MaxBufChars(szTmp)
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
    m_strbData.Append(szTmp, 0, 0);
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
                            , c4MaxBufChars(szTmp)
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
    m_strbData.Append(szTmp, 0, 0);
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
                            , c4MaxBufChars(szTmp)
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
    m_strbData.Append(szTmp, 0, 0);
}

tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TFloat8    f8ToFmt
                        , const tCIDLib::TCard4     c4DecPlaces)
{
    tCIDLib::ETrailFmts eTrail;
    if (c4DecPlaces)
        eTrail = tCIDLib::ETrailFmts::Zeroes;
    else
        eTrail = tCIDLib::ETrailFmts::Ignore;
    tCIDLib::TZStr512 szTmp;
    if (!TRawStr::bFormatVal(f8ToFmt
                            , szTmp
                            , c4DecPlaces
                            , c4MaxBufChars(szTmp)
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
    m_strbData.Append(szTmp, 0, 0);
}

tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TFloat8    f8ToFmt
                        , const tCIDLib::TCard4     c4DecPlaces
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCh        chDecimal
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    tCIDLib::ETrailFmts eTrail;
    if (c4DecPlaces)
        eTrail = tCIDLib::ETrailFmts::Zeroes;
    else
        eTrail = tCIDLib::ETrailFmts::Ignore;

    tCIDLib::TZStr512 szTmp;
    if (!TRawStr::bFormatVal(f8ToFmt
                            , szTmp
                            , c4DecPlaces
                            , c4MaxBufChars(szTmp)
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
    m_strbData.Append(szTmp, 0, 0);
}


tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TInt1      i1ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    tCIDLib::TZStr128 szTmp;
    if (!TRawStr::bFormatVal(i1ToFmt
                            , szTmp
                            , c4MaxBufChars(szTmp)
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
    m_strbData.Append(szTmp, 0, 0);
}

tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TInt2      i2ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    tCIDLib::TZStr128 szTmp;
    if (!TRawStr::bFormatVal(i2ToFmt
                            , szTmp
                            , c4MaxBufChars(szTmp)
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
    m_strbData.Append(szTmp, 0, 0);
}

tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TInt4      i4ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    tCIDLib::TZStr128 szTmp;
    if (!TRawStr::bFormatVal(i4ToFmt
                            , szTmp
                            , c4MaxBufChars(szTmp)
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
    m_strbData.Append(szTmp, 0, 0);
}


tCIDLib::TVoid
TString::AppendFormatted(const  tCIDLib::TInt8      i8ToFmt
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCh        chNegSign
                        , const tCIDLib::TCard4     c4GroupSize
                        , const tCIDLib::TCh        chGroupSep)
{
    tCIDLib::TZStr128 szTmp;
    if (!TRawStr::bFormatVal(i8ToFmt
                            , szTmp
                            , c4MaxBufChars(szTmp)
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
    m_strbData.Append(szTmp, 0, 0);
}


tCIDLib::TVoid TString::AppendFormatted(const MFormattable& fmtblSrc)
{
    // Create a temporary stream over ourself to stream to. Tell it to append
    TTextStringOutStream strmTar
    (
        this, tCIDLib::EAdoptOpts::NoAdopt, kCIDLib::True
    );
    strmTar << fmtblSrc << kCIDLib::FlushIt;
}


// Cap the string at the first instance of the indicated character
tCIDLib::TBoolean TString::bCapAtChar(const tCIDLib::TCh chAt)
{
    return m_strbData.bCapAtChar(chAt);
}


//
//  Comparison methods for various schemes. We first do a check of lengths. If
//  they are different, we can quickly return false. Else we call the comparison
//  method. There's not really a better way to do it. They just have to be compared
//  until we get a different character or hit the end of both, which the
//  underlying methods do.
//
tCIDLib::TBoolean TString::bCompare(const TString& strSrc) const
{
    if (m_strbData.c4Length() != strSrc.m_strbData.c4Length())
        return kCIDLib::False;

    return (eCompare(strSrc) == tCIDLib::ESortComps::Equal);
}

tCIDLib::TBoolean TString::bCompare(const tCIDLib::TCh* const pszSrc) const
{
    return (eCompare(pszSrc) == tCIDLib::ESortComps::Equal);
}


tCIDLib::TBoolean TString::bCompareI(const TString& strSrc) const
{
    if (m_strbData.c4Length() != strSrc.m_strbData.c4Length())
        return kCIDLib::False;

    return (eCompareI(strSrc) == tCIDLib::ESortComps::Equal);
}

tCIDLib::TBoolean TString::bCompareI(const tCIDLib::TCh* const pszSrc) const
{
    return (eCompareI(pszSrc) == tCIDLib::ESortComps::Equal);
}


tCIDLib::TBoolean TString::bCompareN(const  TString&        strSrc
                                    , const tCIDLib::TCard4 c4Count) const
{
    // If we are less than the indicted count, obviously can't be
    if (m_strbData.c4Length() < strSrc.m_strbData.c4Length())
        return kCIDLib::False;

    return (eCompareN(strSrc, c4Count) == tCIDLib::ESortComps::Equal);
}

tCIDLib::TBoolean
TString::bCompareN( const   tCIDLib::TCh* const     pszSrc
                    , const tCIDLib::TCard4         c4Count) const
{
    return (eCompareN(pszSrc, c4Count) == tCIDLib::ESortComps::Equal);
}


tCIDLib::TBoolean TString::bCompareNI(  const   TString&        strSrc
                                        , const tCIDLib::TCard4 c4Count) const
{
    // If we are less than the indicted count, obviously can't be
    if (m_strbData.c4Length() < strSrc.m_strbData.c4Length())
        return kCIDLib::False;

    return (eCompareNI(strSrc, c4Count) == tCIDLib::ESortComps::Equal);
}

tCIDLib::TBoolean
TString::bCompareNI(const   tCIDLib::TCh* const     pszSrc
                    , const tCIDLib::TCard4         c4Count) const
{
    return (eCompareNI(pszSrc, c4Count) == tCIDLib::ESortComps::Equal);
}


//
//  Just a convenience method to see if this string contains the indicated
//  character or not.
//
tCIDLib::TBoolean TString::bContainsChar(const tCIDLib::TCh chToFind) const noexcept
{
    return TRawStr::pszFindChar(m_strbData.pszBuffer(), chToFind) != 0;
}


// Just call the first occurrence check
tCIDLib::TBoolean
TString::bContainsSubStr(const  TString&            strToFind
                        , const tCIDLib::TBoolean   bCaseSensitive) const
{
    tCIDLib::TCard4 c4At = 0;
    return bFirstOccurrence(strToFind, c4At, kCIDLib::False, bCaseSensitive);
}


//
//  Find's the Nth token (based on the passed separator character, and puts
//  it into the passed string.
//
tCIDLib::TBoolean
TString::bExtractNthToken(  const   tCIDLib::TCard4     c4TokInd
                            , const tCIDLib::TCh        chSepChar
                            ,       TString&            strToFill
                            , const tCIDLib::TBoolean   bStripWS) const
{
    //
    //  Ok, find the start of the token. We do a loop here. Each one is
    //  finding the start of the indicated token. We do < toknum here. If
    //  tok number is 0, we fall straight through with the start of the
    //  buffer as the start of the token. Else, we keep moving forward until
    //  we count through toknum sep chars.
    //
    const tCIDLib::TCh* pchStart = m_strbData.pszBuffer();
    const tCIDLib::TCh* pchEnd = pchStart + m_strbData.c4Length();
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
    {
        strToFill.Clear();
        return kCIDLib::True;
    }

    // We have one, so copy out to the target string
    strToFill.m_strbData.FromZStr(pchStart, pchTokEnd - pchStart);

    // And strip trailing now, if asked
    if (bStripWS)
        strToFill.m_strbData.Strip(kCIDLib::szWhitespace, tCIDLib::EStripModes::Trailing);
    return kCIDLib::True;
}


//
//  This method will return a list of all of the token characters found
//  in the passed source string. It returns a string of the characters
//  found. If a token is found more than once, it's in the list more than
//  once. They are returned in the order found.
//
tCIDLib::TBoolean TString::bFindTokenList(TString& strToFill) const
{
    const tCIDLib::TCh* const pszSeparators = L" (,)%";

    const tCIDLib::TCh* pszCur;
    const tCIDLib::TCh* pszClose;
    tCIDLib::TCh*       pszTok;
    tCIDLib::TCh        szTmp[64];

    // Clear the incoming list
    strToFill.Clear();

    //
    //  Do an initial search for a percent sign. After we find one, check the
    //  next character to see what we are looking for.
    //
    const tCIDLib::TCh* pszSource = m_strbData.pszBuffer();
    pszCur = TRawStr::pszFindChar(pszSource, L'%');
    while (pszCur != 0)
    {
        // See if the next char is the end of the string. If so, then no match
        if (*(pszCur + 1) == 0)
            return 0;

        // If the next char is a (, then it is a token so check it out
        if (*(pszCur + 1) == L'(')
        {
            // Search for the closing paren
            pszClose = TRawStr::pszFindChar(pszCur + 1, L')');

            //
            //  There was no closing parm so we need to just search for the
            //  next %, because this was a false alarm.
            //
            if (!pszClose)
            {
                pszCur++;
                pszCur = TRawStr::pszFindChar(pszCur, L'%');
                continue;
            }

            // If it is empty, then can't be right
            if (pszClose - pszCur == 1)
            {
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcStr_InvalidToken
                    , tCIDLib::ESeverities::Warn
                    , tCIDLib::EErrClasses::BadParms
                );
            }

            //
            //  We need to parse the token contents. So copy the substring of
            //  the token to a temp buffer. Note that we multiply by the char
            //  size in order to convert chars to bytes.
            //
            TRawMem::CopyMemBuf
            (
                szTmp, pszCur, (pszClose - pszCur) * kCIDLib::c4CharBytes
            );
            szTmp[pszClose - pszCur] = kCIDLib::chNull;

            tCIDLib::TCh* pszCtx = 0;
            pszTok = TRawStr::pszStrTokenize(szTmp, pszSeparators, &pszCtx);

            //
            //  If no token or the more than a character, then invalid.
            //  Otherwise, pull out the token as the token character.
            //
            if (!pszTok || (TRawStr::c4StrLen(pszTok) > 1))
            {
                facCIDLib().ThrowErr
                (
                    CID_FILE
                    , CID_LINE
                    , kCIDErrs::errcStr_InvalidToken
                    , tCIDLib::ESeverities::Warn
                    , tCIDLib::EErrClasses::BadParms
                );
                return 0;
            }

            // Store the token character
            strToFill.Append(pszTok[0]);

            // Move up to the end of this token for the next round
            pszCur = pszClose;
        }

        // Look for another percent sign
        pszCur++;
        pszCur = TRawStr::pszFindChar(pszCur, L'%');
    }

    return !strToFill.bIsEmpty();
}



tCIDLib::TBoolean
TString::bFirstOccurrence(  const   tCIDLib::TCh            chTarget
                            ,       tCIDLib::TCard4&        c4Pos
                            , const tCIDLib::TBoolean       bCaseSensitive) const
{
    //
    //  See if there is a match.
    //
    //  NOTE: We don't pass in the position here, since its just an output
    //          parameter here. We pass in zero to start at the beginning!
    //
    const tCIDLib::TCh* pszMatch = TRawStr::pszFindChar
    (
        m_strbData.pszBuffer(), chTarget, 0, bCaseSensitive
    );

    // If not, then get out
    if (!pszMatch)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // Calc the position
    c4Pos = tCIDLib::TCard4(pszMatch - m_strbData.pszBuffer());
    return kCIDLib::True;
}

tCIDLib::TBoolean
TString::bFirstOccurrence(  const   TString&                strSubStr
                            ,       tCIDLib::TCard4&        c4Pos
                            , const tCIDLib::TBoolean       bAnyChar
                            , const tCIDLib::TBoolean       bCaseSensitive) const
{
    //
    //  See if there is a match.
    //
    //  NOTE: We don't pass the position in on these, its just an output
    //          parameter. We pass in zero to start at the beginning!
    //
    const tCIDLib::TCh* pszMatch;
    if (bAnyChar)
    {
        pszMatch = TRawStr::pszFindChars
        (
            m_strbData.pszBuffer()
            , strSubStr.m_strbData.pszBuffer()
            , 0
            , bCaseSensitive
        );
    }
     else
    {
        pszMatch = TRawStr::pszFindSubStr
        (
            m_strbData.pszBuffer()
            , strSubStr.m_strbData.pszBuffer()
            , 0
            , bCaseSensitive
        );
    }

    // If not, then get out
    if (!pszMatch)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // Calc the position and return true
    c4Pos = tCIDLib::TCard4(pszMatch - m_strbData.pszBuffer());
    return kCIDLib::True;
}



tCIDLib::TBoolean
TString::bLastOccurrence(   const   tCIDLib::TCh            chTarget
                            ,       tCIDLib::TCard4&        c4Pos
                            , const tCIDLib::TBoolean       bCaseSensitive) const
{
    // See if there is a match
    const tCIDLib::TCh* pszMatch = TRawStr::pszFindLastChar
    (
        m_strbData.pszBuffer(), chTarget, bCaseSensitive
    );

    // If not, then get out
    if (!pszMatch)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // Calc the position and return true
    c4Pos = tCIDLib::TCard4(pszMatch - m_strbData.pszBuffer());
    return kCIDLib::True;
}

tCIDLib::TBoolean
TString::bLastOccurrence(   const   TString&                strSubStr
                            ,       tCIDLib::TCard4&        c4Pos
                            , const tCIDLib::TBoolean       bAnyChar
                            , const tCIDLib::TBoolean       bCaseSensitive) const
{
    // See if there is a match
    const tCIDLib::TCh* pszMatch;
    if (bAnyChar)
    {
        pszMatch = TRawStr::pszFindLastChars
        (
            m_strbData.pszBuffer()
            , strSubStr.m_strbData.pszBuffer()
            , bCaseSensitive
        );
    }
     else
    {
        pszMatch = TRawStr::pszFindLastSubStr
        (
            m_strbData.pszBuffer()
            , strSubStr.m_strbData.pszBuffer()
            , bCaseSensitive
        );
    }

    // If not, then get out
    if (!pszMatch)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // Calc the position of the match and return true
    c4Pos = tCIDLib::TCard4(pszMatch - m_strbData.pszBuffer());
    return kCIDLib::True;
}


tCIDLib::TBoolean
TString::bNextOccurrence(   const   tCIDLib::TCh            chTarget
                            ,       tCIDLib::TCard4&        c4Pos
                            , const tCIDLib::TBoolean       bCaseSensitive) const
{
    // If over the length, then an error
    if (c4Pos >= m_strbData.c4Length())
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
            , TCardinal(m_strbData.c4Length())
        );
    }

    // See if there is a match
    const tCIDLib::TCh* pszMatch = TRawStr::pszFindChar
    (
        m_strbData.pszBuffer(), chTarget, c4Pos + 1, bCaseSensitive
    );

    // If not, then get out
    if (!pszMatch)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // Calc the position
    c4Pos = tCIDLib::TCard4(pszMatch - m_strbData.pszBuffer());
    return kCIDLib::True;
}

tCIDLib::TBoolean
TString::bNextOccurrence(   const   TString&                strSubStr
                            ,       tCIDLib::TCard4&        c4Pos
                            , const tCIDLib::TBoolean       bAnyChar
                            , const tCIDLib::TBoolean       bCaseSensitive) const
{
    // If position is beyond the end, then an error
    if (c4Pos >= m_strbData.c4Length())
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
            , TCardinal(m_strbData.c4Length())
        );
    }

    // See if there is a match
    const tCIDLib::TCh* pszMatch;

    if (bAnyChar)
    {
        pszMatch = TRawStr::pszFindChars
        (
            m_strbData.pszBuffer()
            , strSubStr.m_strbData.pszBuffer()
            , c4Pos + 1
            , bCaseSensitive
        );
    }
     else
    {
        pszMatch = TRawStr::pszFindSubStr
        (
            m_strbData.pszBuffer()
            , strSubStr.m_strbData.pszBuffer()
            , c4Pos + 1
            , bCaseSensitive
        );
    }
    const tCIDLib::TCh* pszLast = pszMatch;

    // If not, then get out with negative result
    if (!pszMatch)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // Calc the position of the match and return true
    c4Pos = tCIDLib::TCard4(pszLast - m_strbData.pszBuffer());
    return kCIDLib::True;
}


tCIDLib::TBoolean
TString::bPrevOccurrence(   const   tCIDLib::TCh            chTarget
                            ,       tCIDLib::TCard4&        c4Pos
                            , const tCIDLib::TBoolean       bCaseSensitive) const
{
    // If the position is zero, then no further to look
    if (!c4Pos)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // If starting point is beyond the string length, then an error
    if (c4Pos > m_strbData.c4Length())
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
            , TCardinal(m_strbData.c4Length())
        );
    }

    const tCIDLib::TCh* pszMatch;
    pszMatch = TRawStr::pszFindPrevChar
    (
        m_strbData.pszBuffer(), chTarget, c4Pos - 1, bCaseSensitive
    );

    if (!pszMatch)
    {
        c4Pos = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // Calc the position of the match and return true
    c4Pos = tCIDLib::TCard4(pszMatch - m_strbData.pszBuffer());
    return kCIDLib::True;
}

tCIDLib::TBoolean
TString::bPrevOccurrence(   const   TString&                strSubStr
                            ,       tCIDLib::TCard4&        c4Pos
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
    if (c4Pos >= m_strbData.c4Length())
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
            , TCardinal(m_strbData.c4Length())
        );
    }

    const tCIDLib::TCh* pszFind = 0;

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
        pszFind = TRawStr::pszFindPrevChars
        (
            m_strbData.pszBuffer()
            , strSubStr.m_strbData.pszBuffer()
            , c4Pos - 1
            , bCaseSensitive
        );
    }
     else
    {
        pszFind = TRawStr::pszFindPrevSubStr
        (
            m_strbData.pszBuffer()
            , strSubStr.m_strbData.pszBuffer()
            , c4Pos - 1
            , bCaseSensitive
        );
    }

    if (pszFind)
    {
        c4Pos = tCIDLib::TCard4(pszFind - m_strbData.pszBuffer());
        return kCIDLib::True;
    }

    c4Pos = kCIDLib::c4MaxCard;
    return kCIDLib::False;
}


tCIDLib::TBoolean
TString::bReplaceSubStr(const   TString&            strToReplace
                        , const TString&            strReplaceWith
                        ,       tCIDLib::TCard4&    c4StartAt
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
    if (!c4TarLen
    ||  (m_strbData.c4Length() < c4TarLen)
    ||  (c4StartAt == m_strbData.c4Length()))
    {
        c4StartAt = kCIDLib::c4MaxCard;
        return kCIDLib::False;
    }

    // If start position is beyond the end, then an error
    if (c4StartAt > m_strbData.c4Length())
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
            , TCardinal(m_strbData.c4Length())
        );
    }

    tCIDLib::TBoolean bRet = kCIDLib::False;
    while(kCIDLib::True)
    {
        // See if there is a match
        const tCIDLib::TCh* pszMatch = TRawStr::pszFindSubStr
        (
            m_strbData.pszBuffer(), pszTarText, c4StartAt, bCaseSensitive
        );

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
        c4StartAt = tCIDLib::TCard4(pszMatch - m_strbData.pszBuffer());

        // Replace the substring we found, just cutting if no rep text
        if (c4RepLen)
            m_strbData.Replace(c4StartAt, c4StartAt + (c4TarLen - 1), pszRepText, c4RepLen);
        else
            m_strbData.Cut(c4StartAt, c4TarLen);

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
TString::bSplit(        TString&                strSecondHalf
                , const tCIDLib::TCh            chDivider
                , const tCIDLib::TBoolean       bCaseSensitive)
{
    const tCIDLib::TCard4 c4OurLen = m_strbData.c4Length();
    strSecondHalf.m_strbData.Clear();

    // Find the first divider character
    const tCIDLib::TCh* pszMatch = TRawStr::pszFindChar
    (
        m_strbData.pszBuffer(), chDivider, 0, bCaseSensitive
    );

    // If no match, then we are done
    if (!pszMatch)
        return kCIDLib::False;

    // Calc the offset
    const tCIDLib::TCard4 c4Ofs = pszMatch - m_strbData.pszBuffer();

    //
    //  We want to skip over the divider, and we want to make sure we don't
    //  try to go beyond the end of this string. So if the char past the
    //  divider isn't beyond the last char, then we have some second half to
    //  get.
    //
    if (c4Ofs + 1 < c4OurLen)
    {
        strSecondHalf.m_strbData.AppendSubStr(m_strbData, c4Ofs + 1, kCIDLib::c4MaxCard);
        strSecondHalf.m_strbData.Strip(kCIDLib::szWhitespace, tCIDLib::EStripModes::LeadTrail);
    }

    m_strbData.CapAt(c4Ofs);
    m_strbData.Strip(kCIDLib::szWhitespace, tCIDLib::EStripModes::LeadTrail);
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
tCIDLib::TBoolean TString::bToBoolean(tCIDLib::TBoolean& bToFill) const noexcept
{
    tCIDLib::TBoolean bValid = kCIDLib::True;

    if (TRawStr::bCompareStrI(m_strbData.pszBuffer(), L"yes")
    ||  TRawStr::bCompareStrI(m_strbData.pszBuffer(), L"on")
    ||  TRawStr::bCompareStrI(m_strbData.pszBuffer(), L"true")
    ||  TRawStr::bCompareStrI(m_strbData.pszBuffer(), L"1"))
    {
        bToFill = kCIDLib::True;
    }
     else if (TRawStr::bCompareStrI(m_strbData.pszBuffer(), L"no")
          ||  TRawStr::bCompareStrI(m_strbData.pszBuffer(), L"off")
          ||  TRawStr::bCompareStrI(m_strbData.pszBuffer(), L"false")
          ||  TRawStr::bCompareStrI(m_strbData.pszBuffer(), L"0"))
    {
        bToFill = kCIDLib::False;
    }
     else
    {
        // It might be something like "  1", so try to convert to a number
        tCIDLib::TBoolean bValid;
        const tCIDLib::TInt4 i4Val = TRawStr::i4AsBinary
        (
            m_strbData.pszBuffer(), bValid, tCIDLib::ERadices::Dec
        );

        if (bValid)
            bToFill = (i4Val != 0);
        else
            bValid = kCIDLib::False;
    }
    return bValid;
}

tCIDLib::TBoolean
TString::bToCard1(          tCIDLib::TCard1&    c1ToFill
                    , const tCIDLib::ERadices   eRadix) const noexcept
{
    tCIDLib::TBoolean bValid;
    const tCIDLib::TCard4 c4Val = TRawStr::c4AsBinary
    (
        m_strbData.pszBuffer(), bValid, eRadix
    );
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
TString::bToCard2(          tCIDLib::TCard2&    c2ToFill
                    , const tCIDLib::ERadices   eRadix) const noexcept
{
    tCIDLib::TBoolean bValid;
    const tCIDLib::TCard4 c4Val = TRawStr::c4AsBinary
    (
        m_strbData.pszBuffer(), bValid, eRadix
    );
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
TString::bToCard4(          tCIDLib::TCard4&    c4ToFill
                    , const tCIDLib::ERadices   eRadix) const noexcept
{
    tCIDLib::TBoolean bValid;
    c4ToFill = TRawStr::c4AsBinary(m_strbData.pszBuffer(), bValid, eRadix);
    return bValid;
}

tCIDLib::TBoolean
TString::bToCard8(          tCIDLib::TCard8&    c8ToFill
                    , const tCIDLib::ERadices   eRadix) const noexcept
{
    tCIDLib::TBoolean bValid;
    c8ToFill = TRawStr::c8AsBinary(m_strbData.pszBuffer(), bValid, eRadix);
    return bValid;
}

tCIDLib::TBoolean TString::bToFloat4(tCIDLib::TFloat4& f4ToFill) const noexcept
{
    tCIDLib::TBoolean bValid;
    const tCIDLib::TFloat8 f8Val = TRawStr::f8AsBinary
    (
        m_strbData.pszBuffer(), bValid
    );

    if (bValid)
    {
        if ((f8Val < kCIDLib::f4MinFloat) && (f8Val > kCIDLib::f4MaxFloat))
            bValid = kCIDLib::False;
        else
            f4ToFill = tCIDLib::TFloat4(f8Val);
    }
    return bValid;
}

tCIDLib::TBoolean TString::bToFloat8(tCIDLib::TFloat8& f8ToFill) const noexcept
{
    tCIDLib::TBoolean bValid;
    f8ToFill = TRawStr::f8AsBinary(m_strbData.pszBuffer(), bValid);
    return bValid;
}

tCIDLib::TBoolean
TString::bToInt1(tCIDLib::TInt1& i1ToFill, const tCIDLib::ERadices eRadix) const noexcept
{
    tCIDLib::TBoolean bValid;
    const tCIDLib::TInt4 i4Val = TRawStr::i4AsBinary
    (
        m_strbData.pszBuffer(), bValid, eRadix
    );
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
TString::bToInt2(tCIDLib::TInt2& i2ToFill, const tCIDLib::ERadices eRadix) const noexcept
{
    tCIDLib::TBoolean bValid;
    const tCIDLib::TInt4 i4Val = TRawStr::i4AsBinary
    (
        m_strbData.pszBuffer(), bValid, eRadix
    );
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
TString::bToInt4(tCIDLib::TInt4& i4ToFill, const tCIDLib::ERadices eRadix) const noexcept
{
    tCIDLib::TBoolean bValid;
    i4ToFill = TRawStr::i4AsBinary(m_strbData.pszBuffer(), bValid, eRadix);
    return bValid;
}

tCIDLib::TBoolean
TString::bToInt8(tCIDLib::TInt8& i8ToFill, const tCIDLib::ERadices eRadix) const noexcept
{
    tCIDLib::TBoolean bValid;
    i8ToFill = TRawStr::i8AsBinary(m_strbData.pszBuffer(), bValid, eRadix);
    return bValid;
}



// Checks to see if the indicated token is in this string
tCIDLib::TBoolean TString::bTokenExists(const tCIDLib::TCh chToken) const
{
    //
    //  We don't care about these, but the searching function returns
    //  them, so...
    //
    tCIDLib::EHJustify  eJustify;
    tCIDLib::TCard4     c4Chars;
    tCIDLib::TCard4     c4TokenCnt;
    tCIDLib::TCard4     c4Width;
    tCIDLib::TCh        chFill;

    // And look for the token.
    if (pszFindToken
    (
        m_strbData.pszBuffer()
        , chToken
        , eJustify
        , c4Width
        , chFill
        , c4Chars
        , c4TokenCnt))
    {
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// Tries to convert this string to a Card4 value. Assumes the indicated radix
tCIDLib::TCard4 TString::c4Val(const tCIDLib::ERadices eRadix) const
{
    tCIDLib::TBoolean bValid;
    const tCIDLib::TCard4 c4Ret = TRawStr::c4AsBinary
    (
        m_strbData.pszBuffer(), bValid, eRadix
    );

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
    tCIDLib::TBoolean bValid;
    const tCIDLib::TCard8 c8Ret = TRawStr::c8AsBinary
    (
        m_strbData.pszBuffer(), bValid, eRadix
    );

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


// Compares this string to a passed string
tCIDLib::ESortComps TString::eCompare(const tCIDLib::TCh* const pszToCompare) const
{
    if (!pszToCompare)
        return tCIDLib::ESortComps::FirstLess;
    return TRawStr::eCompareStr(m_strbData.pszBuffer(), pszToCompare);
}

tCIDLib::ESortComps TString::eCompare(const TString& strToCompare) const
{
    return TRawStr::eCompareStr(m_strbData.pszBuffer(), strToCompare.m_strbData.pszBuffer());
}


// Compares N chars of this string to a passed string
tCIDLib::ESortComps
TString::eCompareN( const   tCIDLib::TCh* const pszToCompare
                    , const tCIDLib::TCard4     c4MaxComp) const
{
    if (!pszToCompare)
        return tCIDLib::ESortComps::FirstLess;
    if (!c4MaxComp)
        return tCIDLib::ESortComps::Equal;
    return TRawStr::eCompareStrN(m_strbData.pszBuffer(), pszToCompare, c4MaxComp);
}

tCIDLib::ESortComps
TString::eCompareN( const   TString&        strToCompare
                    , const tCIDLib::TCard4 c4MaxComp) const
{
    if (!c4MaxComp)
        return tCIDLib::ESortComps::Equal;

    return TRawStr::eCompareStrN
    (
        m_strbData.pszBuffer(), strToCompare.m_strbData.pszBuffer(), c4MaxComp
    );
}


// Compares this string to a passed string, insensitive to case
tCIDLib::ESortComps TString::eCompareI(const tCIDLib::TCh* const pszToCompare) const
{
    if (!pszToCompare)
        return tCIDLib::ESortComps::FirstLess;

    return TRawStr::eCompareStrI(m_strbData.pszBuffer(), pszToCompare);
}

tCIDLib::ESortComps TString::eCompareI(const TString& strToCompare) const
{
    return TRawStr::eCompareStrI
    (
        m_strbData.pszBuffer(), strToCompare.m_strbData.pszBuffer()
    );
}


// Compares N chars of this string to a passed string, insensitive to case
tCIDLib::ESortComps
TString::eCompareNI(const   tCIDLib::TCh* const pszToCompare
                    , const tCIDLib::TCard4     c4MaxComp) const
{
    if (!pszToCompare)
        return tCIDLib::ESortComps::FirstLess;
    if (!c4MaxComp)
        return tCIDLib::ESortComps::Equal;
    return TRawStr::eCompareStrNI(m_strbData.pszBuffer(), pszToCompare, c4MaxComp);
}

tCIDLib::ESortComps
TString::eCompareNI(const   TString&        strToCompare
                    , const tCIDLib::TCard4 c4MaxComp) const
{
    if (!c4MaxComp)
        return tCIDLib::ESortComps::Equal;

    return TRawStr::eCompareStrNI
    (
        m_strbData.pszBuffer()
        , strToCompare.m_strbData.pszBuffer()
        , c4MaxComp
    );
}


// Tries to convert this string into a Float8 value, throwing if bad
tCIDLib::TFloat8 TString::f8Val() const
{
    tCIDLib::TBoolean bValid;
    tCIDLib::TFloat8 f8Ret = TRawStr::f8AsBinary(m_strbData.pszBuffer(), bValid);
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
    tCIDLib::TCh szTmp[2];
    szTmp[0] = chVal;
    szTmp[1] = kCIDLib::chNull;
    return eReplaceToken(szTmp, chToken);
}


tCIDLib::EFindRes
TString::eReplaceToken(  const  tCIDLib::TCh* const pszVal
                        , const tCIDLib::TCh        chToken)
{
    tCIDLib::TCh        chFill;
    tCIDLib::TCard4     c4Chars;
    tCIDLib::TCard4     c4FldWidth;
    tCIDLib::TCard4     c4Index;
    tCIDLib::TCard4     c4TokenCnt;
    tCIDLib::EHJustify  eJustify;

    // Find the first valid replacement token
    const tCIDLib::TCh* pszTmp = pszFindToken
    (
        m_strbData.pszBuffer()
        , chToken
        , eJustify
        , c4FldWidth
        , chFill
        , c4Chars
        , c4TokenCnt
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
    c4Index = pszTmp - m_strbData.pszBuffer();

    //
    //  If the field width is zero, then it's just going to be the source
    //  string in this case, so we can save some overhead.
    //
    if (!c4FldWidth)
    {
        m_strbData.Replace(c4Index, c4Index + (c4Chars - 1), pszVal, c4FldWidth);
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

        // And ask the buffer object to replace the token with the text
        m_strbData.Replace(c4Index, c4Index + (c4Chars - 1), pszFmt, c4FldWidth);
    }
    return tCIDLib::EFindRes::Found;
}


tCIDLib::EFindRes
TString::eReplaceToken( const   MFormattable&   fmtblVal
                        , const tCIDLib::TCh    chToken)
{
    tCIDLib::TCh        chFill;
    tCIDLib::TCard4     c4Chars;
    tCIDLib::TCard4     c4FldWidth;
    tCIDLib::TCard4     c4Index;
    tCIDLib::TCard4     c4TokenCnt;
    tCIDLib::EHJustify  eJustify;

    const tCIDLib::TCh* pszTmp = pszFindToken
    (
        m_strbData.pszBuffer()
        , chToken
        , eJustify
        , c4FldWidth
        , chFill
        , c4Chars
        , c4TokenCnt
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
    c4Index = pszTmp - m_strbData.pszBuffer();

    //
    //  Format the replacement value to a string, being sure to flush it
    //  so that the cache is forced out to the string buffer.
    //
    TTextStringOutStream strmTmp
    (
        256, TStreamFmt(c4FldWidth, 2, eJustify, chFill)
    );
    strmTmp << fmtblVal << kCIDLib::FlushIt;

    // And ask the buffer object to replace the token with the text
    m_strbData.Replace
    (
        c4Index
        , c4Index + (c4Chars - 1)
        , strmTmp.strData().pszBuffer()
        , c4FldWidth
    );
    return tCIDLib::EFindRes::Found;
}


tCIDLib::EFindRes
TString::eReplaceToken( const   tCIDLib::TBoolean   bVal
                        , const tCIDLib::TCh        chToken)
{
    tCIDLib::TCh        chFill;
    tCIDLib::TCard4     c4Chars;
    tCIDLib::TCard4     c4FldWidth;
    tCIDLib::TCard4     c4Index;
    tCIDLib::TCard4     c4TokenCnt;
    tCIDLib::EHJustify  eJustify;

    //  Find the first valid replacement token.
    const tCIDLib::TCh* pszTmp = pszFindToken
    (
        m_strbData.pszBuffer()
        , chToken
        , eJustify
        , c4FldWidth
        , chFill
        , c4Chars
        , c4TokenCnt
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
    c4Index = pszTmp - m_strbData.pszBuffer();

    // And ask the buffer object to replace the token with the text
    const tCIDLib::TCh* pszVal = bVal
                                 ? facCIDLib().pszLoadCIDMsg(kCIDMsgs::midGen_True)
                                 : facCIDLib().pszLoadCIDMsg(kCIDMsgs::midGen_False);

    //
    //  If they provided no field width, then we just use the source value
    //  because none of the formatting stuff could make any difference for
    //  us here.
    //
    if (!c4FldWidth)
    {
        m_strbData.Replace(c4Index, c4Index + (c4Chars - 1), pszVal, c4FldWidth);
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

        // And ask the buffer object to replace the token with the text
        m_strbData.Replace(c4Index, c4Index + (c4Chars - 1), pszFmt, c4FldWidth);
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
    tCIDLib::TCh        chFill;
    tCIDLib::TCard4     c4Chars;
    tCIDLib::TCard4     c4FldWidth;
    tCIDLib::TCard4     c4Index;
    tCIDLib::TCard4     c4TokenCnt;
    tCIDLib::EHJustify  eJustify;

    //  Find the first valid replacement token.
    const tCIDLib::TCh* pszTmp = pszFindToken
    (
        m_strbData.pszBuffer()
        , chToken
        , eJustify
        , c4FldWidth
        , chFill
        , c4Chars
        , c4TokenCnt
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
    c4Index = pszTmp - m_strbData.pszBuffer();

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

    // And ask the buffer object to replace the token with the text
    m_strbData.Replace(c4Index, c4Index + (c4Chars - 1), pszFmt, c4FldWidth);
    return tCIDLib::EFindRes::Found;
}

tCIDLib::EFindRes
TString::eReplaceToken( const   tCIDLib::TCard8     c8Val
                        , const tCIDLib::TCh        chToken
                        , const tCIDLib::ERadices   eRadix)
{
    tCIDLib::TCh        chFill;
    tCIDLib::TCard4     c4Chars;
    tCIDLib::TCard4     c4FldWidth;
    tCIDLib::TCard4     c4Index;
    tCIDLib::TCard4     c4TokenCnt;
    tCIDLib::EHJustify  eJustify;

    //  Find the first valid replacement token.
    const tCIDLib::TCh* pszTmp = pszFindToken
    (
        m_strbData.pszBuffer()
        , chToken
        , eJustify
        , c4FldWidth
        , chFill
        , c4Chars
        , c4TokenCnt
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
    c4Index = pszTmp - m_strbData.pszBuffer();

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
    m_strbData.Replace(c4Index, c4Index + (c4Chars - 1), pszFmt, c4FldWidth);
    return tCIDLib::EFindRes::Found;
}


tCIDLib::EFindRes
TString::eReplaceToken( const   tCIDLib::TFloat8&   f8Val
                        , const tCIDLib::TCh        chToken
                        , const tCIDLib::TCard1     c1Precision)
{
    tCIDLib::TCh        chFill;
    tCIDLib::TCard4     c4Chars;
    tCIDLib::TCard4     c4FldWidth;
    tCIDLib::TCard4     c4Index;
    tCIDLib::TCard4     c4TokenCnt;
    tCIDLib::EHJustify  eJustify;

    // Find the first valid replacement token
    const tCIDLib::TCh* pszTmp = pszFindToken
    (
        m_strbData.pszBuffer()
        , chToken
        , eJustify
        , c4FldWidth
        , chFill
        , c4Chars
        , c4TokenCnt
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
    c4Index = pszTmp - m_strbData.pszBuffer();

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

    // And ask the buffer object to replace the token with the text
    m_strbData.Replace(c4Index, c4Index + (c4Chars - 1), pszFmt, c4FldWidth);
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
    tCIDLib::TCh        chFill;
    tCIDLib::TCard4     c4Chars;
    tCIDLib::TCard4     c4FldWidth;
    tCIDLib::TCard4     c4Index;
    tCIDLib::TCard4     c4TokenCnt;
    tCIDLib::EHJustify  eJustify;

    //  Find the first valid replacement token.
    const tCIDLib::TCh* pszTmp = pszFindToken
    (
        m_strbData.pszBuffer()
        , chToken
        , eJustify
        , c4FldWidth
        , chFill
        , c4Chars
        , c4TokenCnt
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
    c4Index = pszTmp - m_strbData.pszBuffer();

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

    // And ask the buffer object to replace the token with the text
    m_strbData.Replace(c4Index, c4Index + (c4Chars - 1), pszFmt, c4FldWidth);
    return tCIDLib::EFindRes::Found;
}

tCIDLib::EFindRes
TString::eReplaceToken( const   tCIDLib::TInt8      i8Val
                        , const tCIDLib::TCh        chToken
                        , const tCIDLib::ERadices   eRadix)
{
    tCIDLib::TCh        chFill;
    tCIDLib::TCard4     c4Chars;
    tCIDLib::TCard4     c4FldWidth;
    tCIDLib::TCard4     c4Index;
    tCIDLib::TCard4     c4TokenCnt;
    tCIDLib::EHJustify  eJustify;

    //  Find the first valid replacement token.
    const tCIDLib::TCh* pszTmp = pszFindToken
    (
        m_strbData.pszBuffer()
        , chToken
        , eJustify
        , c4FldWidth
        , chFill
        , c4Chars
        , c4TokenCnt
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
    c4Index = pszTmp - m_strbData.pszBuffer();

    tCIDLib::TZStr128  szTmp;
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

    // And ask the buffer object to replace the token with the text
    m_strbData.Replace(c4Index, c4Index + (c4Chars - 1), pszFmt, c4FldWidth);
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
    tCIDLib::EHJustify  eJustify;
    tCIDLib::TCard4     c4Width;
    tCIDLib::TCard4     c4Chars;
    tCIDLib::TCard4     c4TokenCnt;
    tCIDLib::TCh        chFill;

    if (pszFindToken
    (
        m_strbData.pszBuffer()
        , chToken
        , eJustify
        , c4Width
        , chFill
        , c4Chars
        , c4TokenCnt))
    {
        return tCIDLib::EFindRes::Found;
    }

    // Not found, but there are still tokens present
    if (c4TokenCnt)
        return tCIDLib::EFindRes::NotFound;

    // Not found and no more tokens are present
    return tCIDLib::EFindRes::NoMore;
}



//
//  Formats the indicate string into a field of the indicated format and
//  stores the result as our contents.
//
tCIDLib::TVoid
TString::FormatToFld(const  TString&                strToFormat
                    , const tCIDLib::TCard4         c4FldWidth
                    , const tCIDLib::EHJustify      eJustify
                    , const tCIDLib::TCh            chFill
                    , const tCIDLib::TCard4         c4TrailingSp)
{
    // Just delegate to the buffer object
    m_strbData.FormatToFld(strToFormat.m_strbData, c4FldWidth, eJustify, chFill, c4TrailingSp);
}


// Does a standard hash calcualtion on the contents of this string
tCIDLib::THashVal
TString::hshCalcHash(const tCIDLib::TCard4 c4Modulus) const
{
    tCIDLib::THashVal hshRet = TRawStr::hshHashStr
    (
        m_strbData.pszBuffer(), c4Modulus
    );

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
    const tCIDLib::TInt4 i4Ret = TRawStr::i4AsBinary
    (
        m_strbData.pszBuffer(), bValid, eRadix
    );

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
    const tCIDLib::TInt8 i8Ret = TRawStr::i8AsBinary
    (
        m_strbData.pszBuffer(), bValid, eRadix
    );

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


// Inserts the indicated character into us at the indicated index
tCIDLib::TVoid
TString::Insert(const tCIDLib::TCh chInsert, const tCIDLib::TCard4 c4Ind)
{
    tCIDLib::TCh szTmp[2];
    szTmp[0] = chInsert;
    szTmp[1] = kCIDLib::chNull;
    m_strbData.Insert(szTmp, c4Ind);
}


//
//  Sets our contents to a loadable text string, optionally replacing
//  tokens in the loaded text with passed formattable objects.
//
tCIDLib::TVoid
TString::LoadFromMsg(   const   tCIDLib::TErrCode   errcToLoad
                        , const TFacility&          facSrc)
{
    const tCIDLib::TCh* pszTmp = facSrc.pszLoadCIDMsg(errcToLoad);
    m_strbData.Set(pszTmp);
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
    // Put it into a temporary asciiz string
    tCIDLib::TCh pszTmp[2];
    pszTmp[0] = chPrepend;
    pszTmp[1] = kCIDLib::chNull;

    // And insert it at 0
    Insert(pszTmp, 0);
}


//
//  Sets our contents to a formatted version of the passed value, using the
//  indicated formatting information.
//
tCIDLib::TVoid TString::SetFormatted(const MFormattable& fmtblSrc)
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


// ---------------------------------------------------------------------------
//  TString: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TString::FormatTo(TTextOutStream& strmToWriteTo) const
{
    //
    //  Dump the raw string pointer to the string. DO NOT dump 'this'
    //  to the stream! This will kick off a recursive circle jerk.
    //
    strmToWriteTo << m_strbData.pszBuffer();
}


tCIDLib::TVoid TString::StreamFrom(TBinInStream& strmToReadFrom)
{
    m_strbData.StreamFrom(strmToReadFrom);
}


tCIDLib::TVoid TString::StreamTo(TBinOutStream& strmToWriteTo) const
{
    m_strbData.StreamTo(strmToWriteTo);
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


