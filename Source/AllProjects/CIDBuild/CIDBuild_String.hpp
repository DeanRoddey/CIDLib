//
// FILE NAME: CIDBuild_String.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/03/1998
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
//  This header implements a very simple string class that makes it much
//  simpler and safer to deal with strings. It just makes sure that they get
//  cleaned up and are easy to copy and compare.
//
// LOG:
//
//  $_CIDLib_Log_$
//


class TTextFile;


// ---------------------------------------------------------------------------
//   CLASS: TBldStr
//  PREFIX: str
// ---------------------------------------------------------------------------
class TBldStr
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBldStr();

        TBldStr
        (
            const   TBldStr&                strSrc
        );

        TBldStr
        (
            const   TBldStr&                strFirst
            , const TBldStr&                strSec
        );

        TBldStr
        (
            const   tCIDLib::TCh* const     pszSrc
        );

        TBldStr
        (
            const   tCIDLib::TCh            chSrc
        );

        TBldStr
        (
            const   tCIDLib::TSCh* const    pszSrc
        );

        ~TBldStr();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        tCIDLib::TBoolean operator==
        (
            const   TBldStr&                strToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TBldStr&                strToCompare
        )   const;

        TBldStr& operator=
        (
            const   TBldStr&                strToSet
        );

        TBldStr& operator=
        (
            const   tCIDLib::TCh* const     pszToSet
        );

        tCIDLib::TCh operator[]
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TCh& operator[]
        (
            const   tCIDLib::TCard4         c4Index
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Append
        (
            const   TBldStr&                strToAppend
        );

        tCIDLib::TVoid Append
        (
            const   TBldStr&                str1
            , const TBldStr&                str2
        );

        tCIDLib::TVoid Append
        (
            const   tCIDLib::TCh* const     pszToAppend
        );

        tCIDLib::TVoid Append
        (
            const   tCIDLib::TCh* const     pszToAppend1
            , const tCIDLib::TCh* const     pszToAppend2
        );

        tCIDLib::TVoid Append
        (
            const   tCIDLib::TCard4         c4ToAppend
            , const tCIDLib::TCard4         c4Radix = 10
        );

        tCIDLib::TVoid AppendAt
        (
            const   TBldStr&                strToAppend
            , const tCIDLib::TCard4         c4StartAt
        );

        tCIDLib::TVoid Append
        (
            const   tCIDLib::TCh            chToAppend
        );

        tCIDLib::TBoolean bAsBoolean() const;

        tCIDLib::TBoolean bContainsChar
        (
            const   tCIDLib::TCh            chToCheck
        )   const;

        tCIDLib::TBoolean bIContainsChar
        (
            const   tCIDLib::TCh            chToCheck
        )   const;

        tCIDLib::TBoolean bEmpty() const;

        tCIDLib::TBoolean bIEquals
        (
            const   TBldStr&                strToCompare
        )   const;

        tCIDLib::TBoolean bIEquals
        (
            const   tCIDLib::TCh* const     pszToCompare
        )   const;

        tCIDLib::TBoolean bStartsWith
        (
            const   TBldStr&                strToCheck
        )   const;

        tCIDLib::TBoolean bStartsWith
        (
            const   tCIDLib::TCh* const     pszToCheck
        )   const;

        tCIDLib::TBoolean bIStartsWithN
        (
            const   TBldStr&                strToCheck
            , const tCIDLib::TCard4         c4Count
        )   const;

        tCIDLib::TBoolean bIStartsWithN
        (
            const   tCIDLib::TCh* const     pszToCheck
            , const tCIDLib::TCard4         c4Count
        )   const;

        tCIDLib::TCh chFirst() const;

        tCIDLib::TCh chLast() const;

        tCIDLib::TCard4 c4AsCard
        (
            const   tCIDLib::TCard4         c4Radix = 10
        )   const;

        tCIDLib::TCard4 c4Length() const;

        tCIDLib::TVoid DeleteLast();

        tCIDLib::TVoid CapAt
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid Clear();

        tCIDLib::TVoid CopyAt
        (
            const   TBldStr&                strSrc
            , const tCIDLib::TCard4         c4StartAt
        );

        tCIDLib::TVoid CopyAt
        (
            const   tCIDLib::TCh* const     pszSrc
            , const tCIDLib::TCard4         c4StartAt
        );

        tCIDLib::TVoid Cut
        (
            const   tCIDLib::TCard4         c4At
        );

        tCIDLib::TVoid Format
        (
            const   tCIDLib::TCard4         c4ToFormat
            , const tCIDLib::TCard4         c4Radix = 10
        );

        tCIDLib::TSInt iFindFirst
        (
            const   tCIDLib::TCh            chToFind
        )   const;

        tCIDLib::TSInt iFindLast
        (
            const   tCIDLib::TCh            chToFind
        )   const;

        tCIDLib::TInt4 i4AsInt() const;

        tCIDLib::TCh* pszDupBuffer() const;

        const tCIDLib::TCh* pszBuffer() const;

        tCIDLib::TCh* pszBuffer();

        tCIDLib::TCh* pszOrphanBuffer();

        tCIDLib::TVoid ReplaceChar
        (
            const   tCIDLib::TCh            chToReplace
            , const tCIDLib::TCh            chNew
        );

        tCIDLib::TVoid ReplaceExt
        (
            const   TBldStr&                strNewExt
        );

        tCIDLib::TVoid ReplaceExt
        (
            const   tCIDLib::TCh* const     pszNewExt
        );

        tCIDLib::TVoid StripWhitespace();

        void UpperCase();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4BufSz
        //      This is the current allocation size of the buffer. We expand
        //      the buffer as required, and always allocate extra, to avoid
        //      to many reallocations. The actual buffer is of course one
        //      larger than this, to hold the nul.
        //
        //  m_pszBuf
        //      The string buffer. Its always converted to Unicode internally
        //      regardless of what it came in as, and stored in the local
        //      wide character type.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4BufSz;
        tCIDLib::TCh*       m_pszBuf;
};


// ---------------------------------------------------------------------------
//  Global operators
// ---------------------------------------------------------------------------
TTextFile& operator<<(TTextFile& strmOut, const TBldStr& strOut);


// ---------------------------------------------------------------------------
//  TBldStr: Public operators
// ---------------------------------------------------------------------------
inline tCIDLib::TBoolean TBldStr::operator==(const TBldStr& strToCompare) const
{
    return !TRawStr::iCompStr(m_pszBuf, strToCompare.m_pszBuf);
}

inline tCIDLib::TBoolean TBldStr::operator!=(const TBldStr& strToCompare) const
{
    return !operator==(strToCompare);
}



// ---------------------------------------------------------------------------
//  TBldStr: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TVoid TBldStr::Append(const TBldStr& strToAppend)
{
    Append(strToAppend.m_pszBuf);
}

inline tCIDLib::TVoid TBldStr::Append(const TBldStr& str1, const TBldStr& str2)
{
    Append(str1.m_pszBuf, str2.m_pszBuf);
}

inline tCIDLib::TBoolean TBldStr::bEmpty() const
{
    return (m_pszBuf[0] == 0);
}

inline tCIDLib::TBoolean TBldStr::bIEquals(const TBldStr& strToCompare) const
{
    return bIEquals(strToCompare.m_pszBuf);
}

inline tCIDLib::TBoolean TBldStr::bStartsWith(const TBldStr& strToCheck) const
{
    return bStartsWith(strToCheck.m_pszBuf);
}

inline tCIDLib::TBoolean
TBldStr::bIStartsWithN(const TBldStr& strToCheck, const tCIDLib::TCard4 c4Count) const
{
    return bIStartsWithN(strToCheck.m_pszBuf, c4Count);
}

inline tCIDLib::TCh TBldStr::chFirst() const
{
    return m_pszBuf[0];
}

inline tCIDLib::TCh TBldStr::chLast() const
{
    if (!m_pszBuf[0])
        return 0;

    return m_pszBuf[TRawStr::c4StrLen(m_pszBuf)-1];
}

inline tCIDLib::TVoid TBldStr::Clear()
{
    m_pszBuf[0] = 0;
}

inline tCIDLib::TVoid TBldStr::CopyAt(  const   TBldStr&        strSrc
                                        , const tCIDLib::TCard4 c4StartAt)
{
    CopyAt(strSrc.m_pszBuf, c4StartAt);
}

inline tCIDLib::TCh* TBldStr::pszDupBuffer() const
{
    tCIDLib::TCh* pszRet = new tCIDLib::TCh[TRawStr::c4StrLen(m_pszBuf) + 1];
    *pszRet = 0;
    TRawStr::CopyStr(pszRet, m_pszBuf);
    return pszRet;
}

inline const tCIDLib::TCh* TBldStr::pszBuffer() const
{
    return m_pszBuf;
}

inline tCIDLib::TCh* TBldStr::pszBuffer()
{
    return m_pszBuf;
}

inline tCIDLib::TCh* TBldStr::pszOrphanBuffer()
{
    tCIDLib::TCh* pszRet = m_pszBuf;
    m_pszBuf = new tCIDLib::TCh[2];
    m_pszBuf[0] = 0;
    return pszRet;
}

inline tCIDLib::TVoid TBldStr::ReplaceExt(const TBldStr& strNewExt)
{
    ReplaceExt(strNewExt.m_pszBuf);
}

