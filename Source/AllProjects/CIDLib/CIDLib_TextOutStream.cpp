//
// FILE NAME: CIDLib_TextOutStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/08/1999
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
//  This file implements the TTextOutStream class, which provides text mode
//  output over a binary stream.
//
// CAVEATS/GOTCHAS:
//
//  1)  In order to maintain sanity, all writes to the cache are done via
//      a single private method. This allows for 0x0D/0x0A normalization to
//      be done without undue complexity for instance. That method will
//      flush the cache when it becomes full and then continue again.
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
//  Do our standard members macros
// ---------------------------------------------------------------------------
RTTIDecls(TTextOutStream,TObject)



// ---------------------------------------------------------------------------
//  Local functions
// ---------------------------------------------------------------------------
static const tCIDLib::TCh* pszBool(const tCIDLib::TBoolean bVal)
{
    static TAtomicFlag          atomInitDone;
    static const tCIDLib::TCh*  pszFalse   = kCIDLib_::pszFalse;
    static const tCIDLib::TCh*  pszTrue    = kCIDLib_::pszTrue;

    if (!atomInitDone)
    {
        TBaseLock lockInit;
        if (!atomInitDone)
        {
            const tCIDLib::TCh* pszTmp;
            pszTmp = facCIDLib().pszLoadCIDMsg(kCIDMsgs::midGen_False);
            if (pszTmp)
                pszFalse = pszTmp;

            pszTmp = facCIDLib().pszLoadCIDMsg(kCIDMsgs::midGen_True);
            if (pszTmp)
                pszTrue = TRawStr::pszReplicate(pszTmp);

            atomInitDone.Set();
        }
    }
    if (bVal)
        return pszTrue;
    return pszFalse;
}


// ---------------------------------------------------------------------------
//   CLASS: TTextOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTextOutStream: Public, static data lazy evaluation methods
// ---------------------------------------------------------------------------
const TStreamFmt& TTextOutStream::strmfDefault()
{
    static const TStreamFmt strmfDef(0, 2, tCIDLib::EHJustify::Left, kCIDLib::chSpace);
    return strmfDef;
}

const TStreamFmt& TTextOutStream::strmfHex()
{
    static const TStreamFmt strmfHex
    (
        0, 2, tCIDLib::EHJustify::Left, kCIDLib::chSpace, tCIDLib::ERadices::Hex
    );
    return strmfHex;
}

const TStreamFmt& TTextOutStream::strmfHex2()
{
    static const TStreamFmt strmfHex2
    (
        2, 2, tCIDLib::EHJustify::Right, kCIDLib::chDigit0, tCIDLib::ERadices::Hex
    );
    return strmfHex2;
}

const TStreamFmt& TTextOutStream::strmfHex4()
{
    static const TStreamFmt strmfHex4
    (
        4, 2, tCIDLib::EHJustify::Right, kCIDLib::chDigit0, tCIDLib::ERadices::Hex
    );
    return strmfHex4;
}



// ---------------------------------------------------------------------------
//  TTextOutStream: Constructors and operators
// ---------------------------------------------------------------------------

// We don't initialize the arrays since that's just unneeded overhead
#pragma warning(suppress : 26495)
TTextOutStream::TTextOutStream(         TBinOutStream* const    pstrmToAdopt
                                ,       TTextConverter* const   ptcvtToAdopt) :

    m_bIndentNext(kCIDLib::False)
    , m_bSawOD(kCIDLib::False)
    , m_bSuppressIndent(kCIDLib::False)
    , m_chFill(kCIDLib::chSpace)
    , m_c4Indent(0)
    , m_c4Index(0)
    , m_c4Precision(2)
    , m_c4TrailingSp(0)
    , m_c4Width(0)
    , m_eJustification(tCIDLib::EHJustify::Left)
    , m_eNewLineType(tCIDLib::ENewLineTypes::CRLF)
    , m_eRadix(tCIDLib::ERadices::Dec)
    , m_pstrmOut(pstrmToAdopt)
    , m_ptcvtThis(ptcvtToAdopt)
{
    // If no converter is provided, then create a default one
    if (!m_ptcvtThis)
        m_ptcvtThis = new TUTFConverter;
}

// We don't initialize the arrays since that's just unneeded overhead
#pragma warning(suppress : 26495)
TTextOutStream::TTextOutStream(         TBinOutStream* const    pstrmToAdopt
                                , const TStreamFmt&             strmfToUse
                                ,       TTextConverter* const   ptcvtToAdopt) :
    m_bIndentNext(kCIDLib::False)
    , m_bSawOD(kCIDLib::False)
    , m_bSuppressIndent(kCIDLib::False)
    , m_chFill(strmfToUse.m_chFill)
    , m_c4Indent(0)
    , m_c4Index(0)
    , m_c4Precision(strmfToUse.m_c4Precision)
    , m_c4TrailingSp(strmfToUse.m_c4TrailingSp)
    , m_c4Width(strmfToUse.m_c4Width)
    , m_eJustification(strmfToUse.m_eJustification)
    , m_eNewLineType(tCIDLib::ENewLineTypes::CRLF)
    , m_eRadix(tCIDLib::ERadices::Dec)
    , m_pstrmOut(pstrmToAdopt)
    , m_ptcvtThis(ptcvtToAdopt)
{
    // If no converter is provided, then create a default one
    if (!m_ptcvtThis)
        m_ptcvtThis = new TUTFConverter;
}

// We don't initialize the arrays since that's just unneeded overhead
#pragma warning(suppress : 26495)
TTextOutStream::TTextOutStream( const   TStreamFmt&             strmfToUse
                                ,       TTextConverter* const   ptcvtToAdopt) :

    m_bIndentNext(kCIDLib::False)
    , m_bSawOD(kCIDLib::False)
    , m_bSuppressIndent(kCIDLib::False)
    , m_chFill(strmfToUse.m_chFill)
    , m_c4Indent(0)
    , m_c4Index(0)
    , m_c4Precision(strmfToUse.m_c4Precision)
    , m_c4TrailingSp(strmfToUse.m_c4TrailingSp)
    , m_c4Width(strmfToUse.m_c4Width)
    , m_eJustification(strmfToUse.m_eJustification)
    , m_eNewLineType(tCIDLib::ENewLineTypes::CRLF)
    , m_eRadix(tCIDLib::ERadices::Dec)
    , m_pstrmOut(nullptr)
    , m_ptcvtThis(ptcvtToAdopt)
{
    // If no converter is provided, then create a default one
    if (!m_ptcvtThis)
        m_ptcvtThis = new TUTFConverter;
}

TTextOutStream::~TTextOutStream()
{
    //
    //  Flush any remaining cache content if the underlying stream is still
    //  open. Since this is an implicit write of data, the client code cannot
    //  be forced to keep them open just for this.
    //
    if (m_pstrmOut)
    {
        if (m_pstrmOut->bIsOpen())
        {
            try
            {
                Flush();
            }

            catch(...)
            {
                // This could throw if it can't be flushed out
            }
        }
    }

    // Then clean up our allocated members
    delete m_pstrmOut;
    delete m_ptcvtThis;
}



// ---------------------------------------------------------------------------
//  TTextOutStream: Public operators
// ---------------------------------------------------------------------------
TTextOutStream& TTextOutStream::operator<<(const tCIDLib::TCard1 c1ToWrite)
{
    // Just call the TCard4 version
    operator<<(tCIDLib::TCard4(c1ToWrite));
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const tCIDLib::TCard2 c2ToWrite)
{
    // Just call the TCard4 version
    operator<<(tCIDLib::TCard4(c2ToWrite));
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const tCIDLib::TFloat4 f4ToWrite)
{
    // Just call the TFloat8 version
    operator<<(tCIDLib::TFloat8(f4ToWrite));
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const tCIDLib::TInt1 i1ToWrite)
{
    // Just call the TInt4 version
    operator<<(tCIDLib::TInt4(i1ToWrite));
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const tCIDLib::TInt2 i2ToWrite)
{
    // Just call the TInt4 version
    operator<<(tCIDLib::TInt4(i2ToWrite));
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const tCIDLib::TSInt iToWrite)
{
    // Just call the TInt4 version
    operator<<(tCIDLib::TInt4(iToWrite));
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const TTextOutStream::Fill NewFill)
{
    m_chFill = NewFill.chFill;
    return *this;
}

TTextOutStream&
TTextOutStream::operator<<(const TTextOutStream::Justify NewJustification)
{
    m_eJustification = NewJustification.eJustification;
    return *this;
}

TTextOutStream&
TTextOutStream::operator<<(const TTextOutStream::Precision NewPrecision)
{
    m_c4Precision = NewPrecision.c4Precision;
    return *this;
}

TTextOutStream&
TTextOutStream::operator<<(const TTextOutStream::Radix NewRadix)
{
    m_eRadix = NewRadix.eRadix;
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const TTextOutStream::Width NewWidth)
{
    m_c4Width = NewWidth.c4Width;
    return *this;
}


TTextOutStream& TTextOutStream::operator<<(const tCIDLib::TBoolean bToWrite)
{
    tCIDLib::TZStr8 szTmp;
    if (bToWrite)
        TRawStr::CopyStr(szTmp, pszBool(bToWrite), c4MaxBufChars(szTmp));
    else
        TRawStr::CopyStr(szTmp, pszBool(bToWrite), c4MaxBufChars(szTmp));

    if (m_c4Width)
    {
        TString strFmt(m_c4Width + m_c4TrailingSp);
        strFmt.FormatToFld(szTmp, m_c4Width, m_eJustification, m_chFill, m_c4TrailingSp);
        WriteChars(strFmt.pszBuffer());
    }
     else
    {
        WriteChars(szTmp);
    }
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const tCIDLib::TCard4 c4ToWrite)
{
    tCIDLib::TZStr64 szTmp;
    if (!TRawStr::bFormatVal(c4ToWrite, szTmp, c4MaxBufChars(szTmp), m_eRadix))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTStrm_FormatFor
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    if (m_c4Width)
    {
        TString  strFmt(m_c4Width + m_c4TrailingSp);
        strFmt.FormatToFld(szTmp, m_c4Width, m_eJustification, m_chFill, m_c4TrailingSp);
        WriteChars(strFmt.pszBuffer());
    }
     else
    {
        WriteChars(szTmp);
    }
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const tCIDLib::TCard8& c8ToWrite)
{
    tCIDLib::TZStr128 szTmp;
    if (!TRawStr::bFormatVal(c8ToWrite, szTmp, c4MaxBufChars(szTmp), m_eRadix))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTStrm_FormatFor
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    if (m_c4Width)
    {
        TString  strFmt(m_c4Width + m_c4TrailingSp);
        strFmt.FormatToFld(szTmp, m_c4Width, m_eJustification, m_chFill, m_c4TrailingSp);
        WriteChars(strFmt.pszBuffer());
    }
     else
    {
        WriteChars(szTmp);
    }
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const tCIDLib::TFloat8& f8ToWrite)
{
    tCIDLib::TZStr512   szTmp;
    if (!TRawStr::bFormatVal(f8ToWrite
                            , szTmp
                            , m_c4Precision
                            , c4MaxBufChars(szTmp)
                            , tCIDLib::ETrailFmts::Ignore))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTStrm_FormatFor
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    if (m_c4Width)
    {
        TString  strFmt(m_c4Width + m_c4TrailingSp);
        strFmt.FormatToFld(szTmp, m_c4Width, m_eJustification, m_chFill, m_c4TrailingSp);
        WriteChars(strFmt.pszBuffer());
    }
     else
    {
        WriteChars(szTmp);
    }
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const tCIDLib::TInt4 i4ToWrite)
{
    tCIDLib::TZStr64 szTmp;
    if (!TRawStr::bFormatVal(i4ToWrite, szTmp, c4MaxBufChars(szTmp), m_eRadix))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTStrm_FormatFor
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    if (m_c4Width)
    {
        TString  strFmt(m_c4Width + m_c4TrailingSp);
        strFmt.FormatToFld(szTmp, m_c4Width, m_eJustification, m_chFill, m_c4TrailingSp);
        WriteChars(strFmt.pszBuffer());
    }
     else
    {
        WriteChars(szTmp);
    }
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const tCIDLib::TInt8& i8ToWrite)
{
    tCIDLib::TZStr128 szTmp;
    if (!TRawStr::bFormatVal(i8ToWrite, szTmp, c4MaxBufChars(szTmp), m_eRadix))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTStrm_FormatFor
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    if (m_c4Width)
    {
        TString  strFmt(m_c4Width + m_c4TrailingSp);
        strFmt.FormatToFld(szTmp, m_c4Width, m_eJustification, m_chFill, m_c4TrailingSp);
        WriteChars(strFmt.pszBuffer());
    }
     else
    {
        WriteChars(szTmp);
    }
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const tCIDLib::TUInt uToWrite)
{
    tCIDLib::TZStr128 szTmp;
    if (!TRawStr::bFormatVal(uToWrite, szTmp, c4MaxBufChars(szTmp), m_eRadix))
    {
        facCIDLib().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTStrm_FormatFor
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }

    if (m_c4Width)
    {
        TString  strFmt(m_c4Width + m_c4TrailingSp);
        strFmt.FormatToFld(szTmp, m_c4Width, m_eJustification, m_chFill, m_c4TrailingSp);
        WriteChars(strFmt.pszBuffer());
    }
     else
    {
        WriteChars(szTmp);
    }
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const tCIDLib::TCh chToWrite)
{
    WriteChars(&chToWrite, 1);
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const tCIDLib::TCh* const pszToWrite)
{

    //
    //  If the passed buffer pointer is null, then substitute the magic
    //  null pointer string.
    //
    const tCIDLib::TCh* const pszActual = pszToWrite ? pszToWrite
                                                     : kCIDLib::pszNullStr;
    if (m_c4Width)
    {
        TString  strFmt(m_c4Width + m_c4TrailingSp);
        strFmt.FormatToFld(pszActual, m_c4Width, m_eJustification, m_chFill, m_c4TrailingSp);
        WriteChars(strFmt.pszBuffer());
    }
     else
    {
        WriteChars(pszActual);
    }
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const Spaces SpacesToWrite)
{
    // If 0, then nothing to do
    if (!SpacesToWrite.c4Count)
        return *this;

    // Just call the version that will output repeated chars
    return operator<<(RepChars(kCIDLib::chSpace, SpacesToWrite.c4Count));
}

TTextOutStream& TTextOutStream::operator<<(const tCIDLib::ETStrmSpecial eSpecial)
{
    // Convert the enum into a string to output
    const tCIDLib::TCh* pszOut = L"";
    if (eSpecial == tCIDLib::ETStrmSpecial::DblNewLine)
    {
        pszOut = L"\n\n";
    }
     else if ((eSpecial == tCIDLib::ETStrmSpecial::NewLine)
          ||  (eSpecial == tCIDLib::ETStrmSpecial::EndLine))
    {
        pszOut = L"\n";
    }
     else if (eSpecial == tCIDLib::ETStrmSpecial::NewEndLine)
    {
        // A combo of new line and end line, so two new lines and a flush below
        pszOut = L"\n\n";
    }
     else
    {
        #if CID_DEBUG_ON
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTStrm_BadSpecial
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eSpecial))
        );
        #endif
    }

    // Write out the string
    WriteChars(pszOut);

    // If its one of the end line values, then flush
    if ((eSpecial == tCIDLib::ETStrmSpecial::EndLine)
    ||  (eSpecial == tCIDLib::ETStrmSpecial::NewEndLine))
    {
        Flush();
    }

    return *this;
}


TTextOutStream& TTextOutStream::operator<<(const tCIDLib::EStrmSpecial eSpecial)
{
    // If its a flush, just do that one up front
    if (eSpecial == tCIDLib::EStrmSpecial::FlushIt)
    {
        Flush();
    }
    #if CID_DEBUG_ON
    else
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcTStrm_BadSpecial
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eSpecial))
        );
    }
    #endif
    return *this;
}


TTextOutStream& TTextOutStream::operator<<(const RepChars& RepChars)
{
    // The size of the buffer we use to output repeated sequences
    const tCIDLib::TCard4 c4BufSize = 128;

    // If 0 count, then nothing to do
    if (!RepChars.c4Count)
        return *this;

    // Create a counter we can adjust
    tCIDLib::TCard4 c4OutCount = RepChars.c4Count;

    tCIDLib::TCh szBuf[c4BufSize + 1];
    TRawMem::SetMemBuf(szBuf, RepChars.chOut, c4BufSize);
    szBuf[c4BufSize] = kCIDLib::chNull;

    //
    //  Handle the multiples of c4BufSize first by just filling the
    //  string and bulk outputting until we get below c4BufSize.
    //
    if (c4OutCount > c4BufSize)
    {
        while (c4OutCount > c4BufSize)
        {
            WriteChars(szBuf);
            c4OutCount -= c4BufSize;
        }
    }

    // If not an even multiple of c4BufSize, then handle the last bit
    if (c4OutCount)
    {
        szBuf[c4OutCount] = kCIDLib::chNull;
        WriteChars(szBuf);
    }
    return *this;
}

TTextOutStream& TTextOutStream::operator<<(const TStreamFmt& strmfNewFormat)
{
    m_c4Precision    = strmfNewFormat.m_c4Precision;
    m_c4TrailingSp   = strmfNewFormat.m_c4TrailingSp;
    m_c4Width        = strmfNewFormat.m_c4Width;
    m_chFill         = strmfNewFormat.m_chFill;
    m_eJustification = strmfNewFormat.m_eJustification;
    m_eRadix         = strmfNewFormat.m_eRadix;
    return *this;
}


// ---------------------------------------------------------------------------
//  TTextOutStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TTextOutStream::bSuppressIndent() const
{
    return m_bSuppressIndent;
}

tCIDLib::TBoolean TTextOutStream::bSuppressIndent(const tCIDLib::TBoolean bToSet)
{
    m_bSuppressIndent = bToSet;
    return m_bSuppressIndent;
}


tCIDLib::TCh TTextOutStream::chFill() const
{
    return m_chFill;
}

tCIDLib::TCh TTextOutStream::chFill(const tCIDLib::TCh chToSet)
{
    m_chFill = chToSet;
    return m_chFill;
}


tCIDLib::TCard4 TTextOutStream::c4Indent() const
{
    return m_c4Indent;
}

tCIDLib::TCard4 TTextOutStream::c4Indent(const tCIDLib::TCard4 c4ToSet)
{
    // Clip it to no more than 128
    if (c4ToSet > 128)
        m_c4Indent = 128;
    else
        m_c4Indent = c4ToSet;
    return m_c4Indent;
}


tCIDLib::TCard4 TTextOutStream::c4Precision() const
{
    return m_c4Precision;
}

tCIDLib::TCard4 TTextOutStream::c4Precision(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Precision = c4ToSet;
    return m_c4Precision;
}


tCIDLib::TCard4 TTextOutStream::c4TrailingSp() const
{
    return m_c4TrailingSp;
}

tCIDLib::TCard4 TTextOutStream::c4TrailingSp(const tCIDLib::TCard4 c4ToSet)
{
    m_c4TrailingSp = c4ToSet;
    return m_c4TrailingSp;
}

tCIDLib::TCard4 TTextOutStream::c4Width() const
{
    return m_c4Width;
}

tCIDLib::TCard4 TTextOutStream::c4Width(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Width = c4ToSet;
    return m_c4Width;
}


tCIDLib::EHJustify TTextOutStream::eJustification() const
{
    return m_eJustification;
}

tCIDLib::EHJustify
TTextOutStream::eJustification(const tCIDLib::EHJustify eToSet)
{
    m_eJustification = eToSet;
    return m_eJustification;
}


tCIDLib::ENewLineTypes TTextOutStream::eNewLineType() const
{
    return m_eNewLineType;
}

tCIDLib::ENewLineTypes
TTextOutStream::eNewLineType(const tCIDLib::ENewLineTypes eToSet)
{
    m_eNewLineType = eToSet;
    return m_eNewLineType;
}


tCIDLib::ERadices TTextOutStream::eRadix() const
{
    return m_eRadix;
}

tCIDLib::ERadices
TTextOutStream::eRadix(const tCIDLib::ERadices eToSet)
{
    m_eRadix = eToSet;
    return m_eRadix;
}


tCIDLib::TVoid TTextOutStream::Flush()
{
    // Optimize if the cache is empty
    if (!m_c4Index)
        return;

    //
    //  We use this as a temp buffer for converting text to the required
    //  output encoding.
    //
    const tCIDLib::TCard4 c4TmpBufSz = 2048;
    tCIDLib::TCard1 ac1TmpBuf[c4TmpBufSz];

    //
    //  Loop until we have converted all of the cache character into the
    //  output encoding and written them out. c4Count is used to track how
    //  many chars from the cache we've processed so far.
    //
    tCIDLib::TCard4 c4Count = 0;
    tCIDLib::TCard4 c4OutCount;
    while (c4Count < m_c4Index)
    {
        //
        //  Convert as many chars as we can this time around. On each
        //  round we start at the next point in the source temp buffer
        //  that we left off on.
        //
        const tCIDLib::TCard4 c4Converted = m_ptcvtThis->c4ConvertTo
        (
            &m_achCache[c4Count]
            , m_c4Index - c4Count
            , ac1TmpBuf
            , c4TmpBufSz
            , c4OutCount
        );

        // Add to the count the number of chars we did this time
        c4Count += c4Converted;

        //
        //  Write out the bytes we got this time. The default is to throw if
        //  it cannot be fully written.
        //
        m_pstrmOut->c4WriteRawBuffer(ac1TmpBuf, c4OutCount);
    }

    // And now flush the underlying binary stream
    m_pstrmOut->Flush();

    // Reset the cache index now
    m_c4Index = 0;
}


tCIDLib::TVoid TTextOutStream::PutLine(const TString& strToWrite)
{
    WriteChars(strToWrite.pszBuffer());
}


tCIDLib::TVoid TTextOutStream::PutLine(const tCIDLib::TCh* const pszToWrite)
{
    WriteChars(pszToWrite);
}


tCIDLib::TVoid TTextOutStream::Reset()
{
    // Throw away any data in the cache
    m_c4Index = 0;

    // Then reset the underlying binary stream
    m_pstrmOut->Reset();

    // And reset our newline state flags
    m_bSawOD = kCIDLib::False;
    m_bIndentNext = kCIDLib::False;
}


const TString& TTextOutStream::strEncodingName() const
{
    return m_ptcvtThis->strEncodingName();
}


tCIDLib::TVoid TTextOutStream::SeekToEnd()
{
    // Flush any current data first then seek the underlying string
    Flush();
    m_pstrmOut->SeekToEnd();
}


tCIDLib::TVoid TTextOutStream::SetDefaultFormat()
{
    m_c4Precision       = 2;
    m_c4TrailingSp      = 0;
    m_c4Width           = 0;
    m_chFill            = kCIDLib::chSpace;
    m_eJustification    = tCIDLib::EHJustify::Left;
    m_eRadix            = tCIDLib::ERadices::Dec;
}


tCIDLib::TVoid TTextOutStream::SetFormat(const TStreamFmt& strmfToSet)
{
    m_c4Precision    = strmfToSet.m_c4Precision;
    m_c4TrailingSp   = strmfToSet.m_c4TrailingSp;
    m_c4Width        = strmfToSet.m_c4Width;
    m_chFill         = strmfToSet.m_chFill;
    m_eJustification = strmfToSet.m_eJustification;
    m_eRadix         = strmfToSet.m_eRadix;
}


const TTextConverter& TTextOutStream::tcvtThis() const
{
    return *m_ptcvtThis;
}


//
//  This method is the one point of actual output of text, so all other
//  output methods call here eventually. This guy spools out the passed text
//  to the cache, flushing the cache as required.
//
tCIDLib::TVoid
TTextOutStream::WriteChars( const   tCIDLib::TCh* const pszToWrite
                            , const tCIDLib::TCard4     c4Len)
{
    //
    //  Calculate the actual length. If c4Len is zero, then we calculate the
    //  length ourself.
    //
    const tCIDLib::TCard4 c4Actual = c4Len ? c4Len : TRawStr::c4StrLen(pszToWrite);

    //
    //  Unfortunately, for performance reasons, we have to do newline
    //  normalization work. So, instead of just do a buffer copy into the
    //  cache, we have to spool it out one char at a time. We turn any set
    //  of new line characters into an 0x0D/0x0A sequence as we go. Each
    //  time we fill up the cache, we flush it out.
    //
    tCIDLib::TBoolean   bNewLn = kCIDLib::False;
    tCIDLib::TCard4     c4SrcCount = 0;
    while (c4SrcCount < c4Actual)
    {
        // Get the current char out and bump the index
        tCIDLib::TCh chCur = pszToWrite[c4SrcCount++];

        if (chCur == 0x0D)
        {
            //
            //  Indicate that the last char we saw was a CR, and indicate
            //  we have to put out a new line.
            //
            m_bSawOD = kCIDLib::True;
            bNewLn = kCIDLib::True;
        }
         else if (chCur == 0x0A)
        {
            //
            //  If we last saw a CR, then we eat this one, else we put
            //  out our configured new line sequence.
            //
            if (m_bSawOD)
                m_bSawOD = kCIDLib::False;
            else
                bNewLn = kCIDLib::True;
        }
         else
        {
            // Just  regular char, so clean the OD flag
            m_bSawOD = kCIDLib::False;
        }

        //
        //  If we need to do a newline, the put out the newline type that
        //  we are configured for, else put out the current char.
        //
        if (bNewLn)
        {
            if (m_eNewLineType == tCIDLib::ENewLineTypes::CR)
            {
                m_achCache[m_c4Index++] = kCIDLib::chCR;
            }
             else if (m_eNewLineType == tCIDLib::ENewLineTypes::LF)
            {
                m_achCache[m_c4Index++] = kCIDLib::chLF;
            }
             else
            {
                //
                //  This one is more complex. We have to put two chars, but
                //  might only have space for one, so we might have to flush.
                //
                m_achCache[m_c4Index++] = kCIDLib::chCR;
                if (m_c4Index == c4CacheBufSize)
                    Flush();
                m_achCache[m_c4Index++] = kCIDLib::chLF;
            }

            // Be sure to clear the flag now
            bNewLn = kCIDLib::False;

            // And do an indent on the next non-new line
            m_bIndentNext = kCIDLib::True;
        }
         else
        {
            //
            //  If the indent next flag is set, and we have any indent to
            //  do, and it's not being suppressed, then do the indent.
            //
            if (m_bIndentNext && m_c4Indent && !m_bSuppressIndent)
            {
                //
                //  If we have an indent, then put out that many spaces,
                //  flushing the cache as we fill it. Becuase of the
                //  possibility of doing something horrible, verify that
                //  it's not beyond the legal max and zero it if so.
                //
                if (m_c4Indent > 128)
                {
                    #if CID_DEBUG_ON
                    facCIDLib().MsgPopUp
                    (
                        L"Stream indent is > 128"
                        , CID_FILE
                        , CID_LINE
                    );
                    #endif
                    m_c4Indent = 0;
                }

                for (tCIDLib::TCard4 c4Index = 0; c4Index < m_c4Indent; c4Index++)
                {
                    #pragma warning(suppress : 6386) // We are flushing and resetting
                    m_achCache[m_c4Index++] = kCIDLib::chSpace;
                    if (m_c4Index == c4CacheBufSize)
                        Flush();
                }
            }

            // Clear the flag again one way or another
            m_bIndentNext = kCIDLib::False;

            // And finally put the actual new character out
            #pragma warning(suppress : 6386) // We are flushing and resetting as required
            m_achCache[m_c4Index++] = chCur;
        }

        //
        //  If we've filled the cache, then flush it. This will reset the
        //  m_c4Index member.
        //
        if (m_c4Index == c4CacheBufSize)
            Flush();
    }
}



// ---------------------------------------------------------------------------
//  TTextOutStream: Hidden constructors
// ---------------------------------------------------------------------------

// We don't initialize the arrays since that's just unneeded overhead
#pragma warning(suppress : 26495)
TTextOutStream::TTextOutStream(TTextConverter* const ptcvtToAdopt) :

    m_bIndentNext(kCIDLib::False)
    , m_bSawOD(kCIDLib::False)
    , m_bSuppressIndent(kCIDLib::False)
    , m_chFill(kCIDLib::chSpace)
    , m_c4Indent(0)
    , m_c4Index(0)
    , m_c4Precision(2)
    , m_c4TrailingSp(0)
    , m_c4Width(0)
    , m_eJustification(tCIDLib::EHJustify::Left)
    , m_eNewLineType(tCIDLib::ENewLineTypes::CRLF)
    , m_eRadix(tCIDLib::ERadices::Dec)
    , m_pstrmOut(nullptr)
    , m_ptcvtThis(ptcvtToAdopt)
{
    // If no converter is provided, then create a default one
    if (!m_ptcvtThis)
        m_ptcvtThis = new TUTFConverter;
}


// ---------------------------------------------------------------------------
//  TTextOutStream: Protected, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TTextOutStream::AdoptStream(TBinOutStream* const pstrmToAdopt)
{
    //
    //  We are responsible for the new stream, so make sure it's
    //  cleaned up if we don't end up taking it.
    //
    TJanitor<TBinOutStream> janAdopt(pstrmToAdopt);

    // The passed stream can't be null and we can't already have a stream
    if (bCIDPreCond(pstrmToAdopt != nullptr)
    &&  bCIDPreCond(m_pstrmOut == nullptr))
    {
        m_pstrmOut = janAdopt.pobjOrphan();
    }
}


TBinOutStream& TTextOutStream::strmOut()
{
    // It won't actually ever return false, it'll throw
    if (!bCIDAssert(m_pstrmOut != nullptr, L"The underlying binary stream isn't set"))
        return *static_cast<TBinOutStream*>(nullptr);

    return *m_pstrmOut;
}


const TBinOutStream& TTextOutStream::strmOut() const
{
    // It won't actually ever return false, it'll throw
    if (!bCIDAssert(m_pstrmOut != nullptr, L"The underlying binary stream isn't set"))
        return *static_cast<TBinOutStream*>(nullptr);

    return *m_pstrmOut;
}

