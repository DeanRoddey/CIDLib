//
// FILE NAME: CIDBuild_TextFile.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/1999
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This file implements the TTextFile class.
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
#include    <string.h>



// ---------------------------------------------------------------------------
//   CLASS: TTextFile
//  PREFIX: tfl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTextFile: Constructors and Destructor
// ---------------------------------------------------------------------------
TTextFile::TTextFile() :

    m_eEncoding(tCIDBuild::EEncodings::ASCII)
{
}

TTextFile::TTextFile(const tCIDLib::EStdFiles eStdFile) :

    m_bflData(eStdFile)
    , m_eEncoding(tCIDBuild::EEncodings::ASCII)
{
}

TTextFile::TTextFile(const  TBldStr&                strName
                    , const tCIDBuild::EEncodings   eEncoding
                    , const tCIDLib::EAccessModes   eAccess
                    , const tCIDLib::ECreateActs eAction) :

    m_bflData(strName, eAccess, eAction)
    , m_eEncoding(eEncoding)
{
}

TTextFile::~TTextFile()
{
}


// ---------------------------------------------------------------------------
//  TTextFile: Public operators
// ---------------------------------------------------------------------------
TTextFile& TTextFile::operator<<(const tCIDBuild::ESpecialChars eSpecial)
{
    if (eSpecial == tCIDBuild::ESpecialChars::EndLine)
        *this << L"\n";

    return *this;
}

TTextFile& TTextFile::operator<<(const tCIDLib::TSCh* const pszToPrint)
{
    if (m_eEncoding == tCIDBuild::EEncodings::UTF16)
    {
        tCIDLib::TCh* pszTmp = TRawStr::pszTranscode(pszToPrint);
        m_bflData.c4Write
        (
            pszTmp
            , TRawStr::c4StrLen(pszTmp) * kCIDLib::c4CharBytes
        );
        delete [] pszTmp;
    }
     else
    {
        m_bflData.c4Write(pszToPrint, strlen(pszToPrint));
    }
    return *this;
}

TTextFile& TTextFile::operator<<(const tCIDLib::TCh* const pszToPrint)
{
    if (m_eEncoding == tCIDBuild::EEncodings::ASCII)
    {
        tCIDLib::TSCh* pszTmp = TRawStr::pszTranscode(pszToPrint);
        m_bflData.c4Write(pszTmp, strlen(pszTmp));
        delete [] pszTmp;
    }
     else
    {
        //
        //  If the local wide char type and the Unicode char type is
        //  the same size, then write it as is. Otherwise, allocate a
        //  temp buffer and truncate them back and write that.
        //
        if (kCIDLib::c4CharBytes == kCIDLib::c4UniBytes)
        {
            m_bflData.c4Write
            (
                pszToPrint
                , TRawStr::c4StrLen(pszToPrint) * kCIDLib::c4CharBytes
            );
        }
         else
        {
            const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszToPrint);
            tCIDLib::TUniCh* pszTmp = new tCIDLib::TUniCh[c4Len + 1];
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Len; c4Index++)
                pszTmp[c4Index] = tCIDLib::TUniCh(pszToPrint[c4Index]);
            m_bflData.c4Write(pszTmp, c4Len * kCIDLib::c4UniBytes);
            delete [] pszTmp;
        }
    }
    return *this;
}

TTextFile& TTextFile::operator<<(const tCIDLib::TCh chToPrint)
{
    if (m_eEncoding == tCIDBuild::EEncodings::ASCII)
    {
        tCIDLib::TCh szTmp[2];
        szTmp[0] = chToPrint;
        szTmp[1] = 0;
        tCIDLib::TSCh* pszTmp = TRawStr::pszTranscode(szTmp);
        m_bflData.c4Write(pszTmp, strlen(pszTmp));
        delete [] pszTmp;
    }
     else
    {
        //
        //  If the local wide char type is the same size as the Unicode
        //  char type, then just write it as is. Otherwise, truncate it
        //  back.
        //
        if (kCIDLib::c4CharBytes == kCIDLib::c4UniBytes)
        {
            m_bflData.c4Write(&chToPrint, kCIDLib::c4CharBytes);
        }
         else
        {
            tCIDLib::TUniCh chTmp = tCIDLib::TUniCh(chToPrint);
            m_bflData.c4Write(&chTmp, kCIDLib::c4UniBytes);
        }
    }
    return *this;
}

TTextFile& TTextFile::operator<<(const tCIDLib::TCard4 c4ToPrint)
{
    tCIDLib::TCh szTmp[128];
    TRawStr::FormatVal(c4ToPrint, szTmp, 127, 10);
    *this << szTmp;

    return *this;
}


TTextFile& TTextFile::operator<<(const tCIDLib::TInt4 i4ToPrint)
{
    tCIDLib::TCh szTmp[128];
    TRawStr::FormatVal(i4ToPrint, szTmp, 127);
    *this << szTmp;

    return *this;
}


// ---------------------------------------------------------------------------
//  TTextFile: Public method
// ---------------------------------------------------------------------------
tCIDLib::TVoid TTextFile::Close()
{
    m_bflData.Close();
}


tCIDLib::TVoid TTextFile::ConIndent(const tCIDLib::TCard4 c4Level)
{
    static const tCIDLib::TSCh* aschLevel = "|  ";
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Level; c4Index++)
        *this << aschLevel;
}


tCIDLib::TVoid TTextFile::Indent(const tCIDLib::TCard4 c4Level)
{
    static const tCIDLib::TSCh* aschLevel = "   ";
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Level; c4Index++)
        *this << aschLevel;
}


tCIDLib::TVoid
TTextFile::Open(const   TBldStr&                strName
                , const tCIDBuild::EEncodings   eEncoding
                , const tCIDLib::EAccessModes   eAccess
                , const tCIDLib::ECreateActs eAction)
{
    m_eEncoding = eEncoding;
    m_bflData.Open(strName, eAccess, eAction);
}


