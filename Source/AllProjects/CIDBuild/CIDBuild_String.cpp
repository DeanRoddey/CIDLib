//
// FILE NAME: CIDBuild_String.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/29/1998
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
//  This file implements the TCBStr class
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDBuild.hpp"



// ---------------------------------------------------------------------------
//  Global operators
// ---------------------------------------------------------------------------
TTextFile& operator<<(TTextFile& strmOut, const TBldStr& strOut)
{
    strmOut << strOut.pszBuffer();
    return strmOut;
}


// ---------------------------------------------------------------------------
//   CLASS: TBldStr
//  PREFIX: str
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBldStr: Constructors and Destructor
// ---------------------------------------------------------------------------
TBldStr::TBldStr() :

    m_c4BufSz(64)
    , m_pszBuf(nullptr)
{
    m_pszBuf = new tCIDLib::TCh[m_c4BufSz + 1];
    m_pszBuf[0] = kCIDLib::chNull;
}

TBldStr::TBldStr(const TBldStr& strSrc) :

    m_c4BufSz(strSrc.m_c4BufSz)
    , m_pszBuf(nullptr)
{
    m_pszBuf = new tCIDLib::TCh[m_c4BufSz + 1];
    TRawStr::MoveChars(m_pszBuf, strSrc.m_pszBuf, m_c4BufSz + 1);
}

TBldStr::TBldStr(const TBldStr& strFirst, const TBldStr& strSec) :

    m_c4BufSz(strFirst.m_c4BufSz + strSec.m_c4BufSz + 16)
    , m_pszBuf(nullptr)
{
    m_pszBuf = new tCIDLib::TCh[m_c4BufSz + 1];
    TRawStr::CopyStr(m_pszBuf, strFirst.m_pszBuf);
    TRawStr::CopyStr(&m_pszBuf[strFirst.m_c4BufSz], strSec.m_pszBuf);
}

TBldStr::TBldStr(const tCIDLib::TCh* const pszText) :

    m_c4BufSz(0)
    , m_pszBuf(nullptr)
{
    m_c4BufSz = TRawStr::c4StrLen(pszText) + 64;
    m_pszBuf = new tCIDLib::TCh[m_c4BufSz + 1];
    TRawStr::CopyStr(m_pszBuf, pszText);
}

TBldStr::TBldStr(const tCIDLib::TSCh* const pszName) :

    m_c4BufSz(0)
    , m_pszBuf(nullptr)
{
    m_pszBuf = TRawStr::pszTranscode(pszName);
    m_c4BufSz = TRawStr::c4StrLen(m_pszBuf) + 64;
}

TBldStr::TBldStr(const tCIDLib::TCh chText) :

    m_c4BufSz(1)
    , m_pszBuf(nullptr)
{
    m_pszBuf = new tCIDLib::TCh[m_c4BufSz + 1];
    m_pszBuf[0] = chText;
    m_pszBuf[1] = kCIDLib::chNull;
}

TBldStr::~TBldStr()
{
    delete [] m_pszBuf;
}


// ---------------------------------------------------------------------------
//  TBldStr: Public operators
// ---------------------------------------------------------------------------
TBldStr& TBldStr::operator=(const TBldStr& strToSet)
{
    if (this == &strToSet)
        return *this;

    // If our buffer isn't large enough, then reallocate it
    if (strToSet.m_c4BufSz > m_c4BufSz)
    {
        delete [] m_pszBuf;
        m_c4BufSz = strToSet.m_c4BufSz;
        m_pszBuf = new tCIDLib::TCh[m_c4BufSz + 1];
    }

    TRawStr::CopyStr(m_pszBuf, strToSet.m_pszBuf);
    return *this;
}

TBldStr& TBldStr::operator=(const tCIDLib::TCh* const pszToSet)
{
    // If our buffer isn't large enough, then reallocate it.
    const tCIDLib::TCard4 c4SrcLen = TRawStr::c4StrLen(pszToSet);
    if (c4SrcLen > m_c4BufSz)
    {
        delete [] m_pszBuf;
        m_c4BufSz = c4SrcLen + 64;
        m_pszBuf = new tCIDLib::TCh[m_c4BufSz + 1];
    }

    TRawStr::CopyStr(m_pszBuf, pszToSet);
    return *this;
}


tCIDLib::TCh TBldStr::operator[](const tCIDLib::TCard4 c4Index) const
{
    // If its past the end, then an error
    if (c4Index > TRawStr::c4StrLen(m_pszBuf))
    {
        stdOut << L"Index is past end of string" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::IndexError;
    }
    return m_pszBuf[c4Index];
}

tCIDLib::TCh& TBldStr::operator[](const tCIDLib::TCard4 c4Index)
{
    // If its past the end, then an error
    if (c4Index > TRawStr::c4StrLen(m_pszBuf))
    {
        stdOut << L"Index is past end of string" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::IndexError;
    }
    return m_pszBuf[c4Index];
}


// ---------------------------------------------------------------------------
//  TBldStr: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TBldStr::Append(const tCIDLib::TCh* const pszToAppend)
{
    // If we don't enough spare space, the reallocate
    const tCIDLib::TCard4 c4AppendLen = TRawStr::c4StrLen(pszToAppend);
    const tCIDLib::TCard4 c4OurLen = TRawStr::c4StrLen(m_pszBuf);

    if (c4OurLen + c4AppendLen > m_c4BufSz)
    {
        m_c4BufSz = c4OurLen + c4AppendLen + 64;
        tCIDLib::TCh* pszTmp = new tCIDLib::TCh[m_c4BufSz + 1];
        TRawStr::CopyStr(pszTmp, m_pszBuf);
        delete [] m_pszBuf;
        m_pszBuf = pszTmp;
    }

    TRawStr::CopyStr(&m_pszBuf[c4OurLen], pszToAppend);
}

tCIDLib::TVoid
TBldStr::Append(const   tCIDLib::TCh* const psz1
                , const tCIDLib::TCh* const psz2)
{
    // If we don't enough spare space, the reallocate
    const tCIDLib::TCard4 c4AppendLen1 = TRawStr::c4StrLen(psz1);
    const tCIDLib::TCard4 c4AppendLen2 = TRawStr::c4StrLen(psz2);
    const tCIDLib::TCard4 c4OurLen = TRawStr::c4StrLen(m_pszBuf);

    if (c4OurLen + c4AppendLen1 + c4AppendLen2 > m_c4BufSz)
    {
        m_c4BufSz = c4OurLen + c4AppendLen1 + c4AppendLen2 + 64;
        tCIDLib::TCh* pszTmp = new tCIDLib::TCh[m_c4BufSz + 1];
        TRawStr::CopyStr(pszTmp, m_pszBuf);
        delete [] m_pszBuf;
        m_pszBuf = pszTmp;
    }

    TRawStr::CopyStr(&m_pszBuf[c4OurLen], psz1);
    TRawStr::CopyStr(&m_pszBuf[c4OurLen + c4AppendLen1], psz2);
}

tCIDLib::TVoid TBldStr::Append(const    tCIDLib::TCard4   c4ToAppend
                                , const tCIDLib::TCard4   c4Radix)
{
    tCIDLib::TCh szTmp[128];
    TRawStr::FormatVal(c4ToAppend, szTmp, c4Radix);
    Append(szTmp);
}


tCIDLib::TVoid TBldStr::AppendAt(const  TBldStr&        strToAppend
                                , const tCIDLib::TCard4 c4StartAt)
{
    if (c4StartAt >= strToAppend.c4Length())
    {
        stdOut << L"Start index for Append is beyond source string's length"
               << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::IndexError;
    }

    // And just call the other version with the part we want
    Append(&strToAppend.pszBuffer()[c4StartAt]);
}


tCIDLib::TVoid TBldStr::Append(const tCIDLib::TCh chToAppend)
{
    tCIDLib::TCh szTmp[2];
    szTmp[0] = chToAppend;
    szTmp[1] = kCIDLib::chNull;
    Append(szTmp);
}


tCIDLib::TBoolean TBldStr::bAsBoolean() const
{
    return (TRawStr::iCompIStr(m_pszBuf, L"True") == 0);
}


tCIDLib::TBoolean
TBldStr::bIEquals(const tCIDLib::TCh* const pszToCompare) const
{
    if (!TRawStr::iCompIStr(pszToCompare, m_pszBuf))
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean
TBldStr::bStartsWith(const tCIDLib::TCh* const pszToCheck) const
{
    if (!TRawStr::iCompStrN(m_pszBuf, pszToCheck, TRawStr::c4StrLen(pszToCheck)))
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean
TBldStr::bIStartsWithN(const tCIDLib::TCh* const pszToCheck, const tCIDLib::TCard4 c4Count) const
{
    if (!TRawStr::iCompIStrN(m_pszBuf, pszToCheck, c4Count))
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TCard4 TBldStr::c4AsCard(const tCIDLib::TCard4 c4Radix) const
{
    tCIDLib::TCard4 c4Ret;
    if (!TRawStr::bXlatCard4(m_pszBuf, c4Ret, c4Radix))
    {
        stdOut  << L"'" << m_pszBuf << L"' cannot be converted to a cardinal value"
                << kCIDBuild::EndLn;

        throw tCIDBuild::EErrors::FileFormat;
    }
    return c4Ret;
}


tCIDLib::TVoid TBldStr::CapAt(const tCIDLib::TCard4 c4Index)
{
    if (c4Index >= TRawStr::c4StrLen(m_pszBuf))
    {
        stdOut << L"CapAt index is beyond the length of the string" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::IndexError;
    }
    m_pszBuf[c4Index] = kCIDLib::chNull;
}


tCIDLib::TVoid TBldStr::CopyAt( const   tCIDLib::TCh* const pszSrc
                                , const tCIDLib::TCard4     c4StartAt)
{
    if (c4StartAt >= TRawStr::c4StrLen(pszSrc))
    {
        stdOut << L"Start index for CopyAt is beyond source string length"
               << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::IndexError;
    }
    *this = &pszSrc[c4StartAt];
}


tCIDLib::TVoid TBldStr::Cut(const tCIDLib::TCard4 c4At)
{
    // If its at 0, then nothing to do
    if (!c4At)
        return;

    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(m_pszBuf);

    // If its past the end, then an error
    if (c4At > c4Len)
    {
        stdOut << L"Cut index is past end of string" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::IndexError;
    }

    // If its at the end, then just empty the string
    if (c4At == c4Len)
    {
        m_pszBuf[0] = kCIDLib::chNull;
        return;
    }

    // Else copy down from the cut position
    tCIDLib::TCh* pszSrc = &m_pszBuf[c4At];
    tCIDLib::TCh* pszTarget = m_pszBuf;
    while (*pszSrc)
    {
        *pszTarget = *pszSrc;
        pszTarget++;
        pszSrc++;
    }

    // And make sure to get the null copied
    *pszTarget = *pszSrc;
}


tCIDLib::TVoid TBldStr::DeleteLast()
{
    // If empty, nothing to do
    if (!*m_pszBuf)
        return;

    // Run up to the null, then back up and cap
    tCIDLib::TCh* pszTarget = m_pszBuf;
    while (*pszTarget)
        pszTarget++;

    pszTarget--;
    *pszTarget = kCIDLib::chNull;
}


tCIDLib::TVoid TBldStr::Format( const   tCIDLib::TCard4   c4ToFormat
                                , const tCIDLib::TCard4   c4Radix)
{
    tCIDLib::TCh szTmp[128];
    TRawStr::FormatVal(c4ToFormat, szTmp, c4Radix);
    *this = szTmp;
}


tCIDLib::TSInt TBldStr::iFindFirst(const tCIDLib::TCh chToFind) const
{
    tCIDLib::TCh* pszTarget = m_pszBuf;
    while (*pszTarget)
    {
        if (*pszTarget == chToFind)
            break;
        pszTarget++;
    }

    if (!*pszTarget)
        return -1;

    // Return the offset
    return tCIDLib::TSInt(pszTarget - m_pszBuf);
}


tCIDLib::TSInt TBldStr::iFindLast(const tCIDLib::TCh chToFind) const
{
    tCIDLib::TCard4 c4Len =  TRawStr::c4StrLen(m_pszBuf);
    if (!c4Len)
        return -1;

    tCIDLib::TCh* pszTarget = &m_pszBuf[c4Len - 1];
    while ((*pszTarget != chToFind) && (pszTarget >= m_pszBuf))
        pszTarget--;

    if (pszTarget < m_pszBuf)
        return -1;

    return tCIDLib::TSInt(pszTarget - m_pszBuf);
}


tCIDLib::TInt4 TBldStr::i4AsInt() const
{
    tCIDLib::TInt4 i4Ret;
    if (!TRawStr::bXlatInt4(m_pszBuf, i4Ret))
    {
        stdOut  << L"'" << m_pszBuf << L"' cannot be converted to an integer value"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileFormat;
    }
    return i4Ret;
}


tCIDLib::TVoid TBldStr::ReplaceChar(  const   tCIDLib::TCh  chToReplace
                                        , const tCIDLib::TCh  chNew)
{
    tCIDLib::TCh* pszCur = m_pszBuf;
    while (*pszCur)
    {
        if (*pszCur == chToReplace)
            *pszCur = chNew;
        pszCur++;
    }
}


tCIDLib::TVoid TBldStr::ReplaceExt(const tCIDLib::TCh* const pszNewExt)
{
    // Strip off the extension and replace it with the passed
    tCIDLib::TCh* pszExt = TRawStr::pszFindLastChar(m_pszBuf, kCIDBuild::chExtSep);
    if (!pszExt)
    {
        stdOut  << L"Cound not find the extension in file name: "
                << m_pszBuf << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::NotFound;
    }

    // Cap it off where it is and get the new length, and of the new extension
    *pszExt = kCIDLib::chNull;
    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(m_pszBuf);
    const tCIDLib::TCard4 c4ExtLen = TRawStr::c4StrLen(pszNewExt);

    //
    //  If the new length plus the length of the extension will fit in the
    //  current buffer, then just copy in. Else, we have to reallocate.
    //
    if (c4Len + c4ExtLen <= m_c4BufSz)
    {
        TRawStr::CopyStr(pszExt, pszNewExt);
    }
     else
    {
        // Save the old buffer temporarily
        tCIDLib::TCh* pszOld = m_pszBuf;

        // Alloc a new buffer and build it back up
        m_c4BufSz += c4ExtLen + 64;
        m_pszBuf = new tCIDLib::TCh[m_c4BufSz + 1];
        TRawStr::CopyStr(m_pszBuf, pszOld);
        TRawStr::CopyStr(&m_pszBuf[c4Len], pszNewExt);

        // And now we can drop the old buffer
        delete [] pszOld;
    }
}


tCIDLib::TVoid TBldStr::StripWhitespace()
{
    //
    //  Get a pointer to the buffer and run up until we find the first non
    //  whitespace.
    //
    const tCIDLib::TCh* pszNonWS = m_pszBuf;
    while (TRawStr::bIsSpace(*pszNonWS))
        pszNonWS++;

    // If we hit the null, then just empty our buffer
    if (!*pszNonWS)
    {
        m_pszBuf[0] = kCIDLib::chNull;
        return;
    }

    // If we moved up at all, then we have to copy down the stuff to keep
    if (pszNonWS != m_pszBuf)
    {
        tCIDLib::TCh* pszTarget = m_pszBuf;
        while (*pszNonWS)
        {
            *pszTarget = *pszNonWS;
            pszTarget++;
            pszNonWS++;
        }

        // And be sure to copy the null terminator
        *pszTarget = *pszNonWS;
    }

    //
    //  And now lets get rid any trailing whitespace. We start at the end
    //  and run backwards until we find some non-whitespace. We know that
    //  there is some content in the string at this point.
    //
    tCIDLib::TCh* pszTarget = &m_pszBuf[TRawStr::c4StrLen(m_pszBuf)-1];
    while (TRawStr::bIsSpace(*pszTarget) && (pszTarget >= m_pszBuf))
        pszTarget--;

    // Put a null in the next position past the non-WS that broke us out
    *(pszTarget+1) = kCIDLib::chNull;
}


tCIDLib::TVoid TBldStr::UpperCase()
{
    TRawStr::UpperCase(m_pszBuf);
}


tCIDLib::TCard4 TBldStr::c4Length() const
{
    return TRawStr::c4StrLen(m_pszBuf);
}
