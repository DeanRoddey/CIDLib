//
// FILE NAME: CIDBuild_LineSpooler.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/22/1998
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
//  This file implements the TLineSpooler class.
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
//   CLASS: TLineSpooler
//  PREFIX: lspl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TLineSpooler: Constructors and Destructor
// ---------------------------------------------------------------------------
TLineSpooler::TLineSpooler( const   TBldStr&            strFileName
                            , const tCIDLib::TBoolean   bStripSpace) :

    m_bDisableMacros(kCIDLib::False)
    , m_bStripSpace(bStripSpace)
    , m_pchReadBuf(0)
    , m_strFileName(strFileName)
    , m_c4CurLine(0)
    , m_c4CurOfs(0)
    , m_c4BufSize(0)
{
    // Allocate the buffer and store the size read
    m_pchReadBuf = pchAllocReadBuf(m_c4BufSize);
}

TLineSpooler::~TLineSpooler()
{
    delete [] m_pchReadBuf;
}


// ---------------------------------------------------------------------------
//  TLineSpooler: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TLineSpooler::bEndOfFile() const
{
    return (m_c4CurOfs == m_c4BufSize);
}


tCIDLib::TBoolean TLineSpooler::bReadLine(TBldStr& strTarget)
{
    while (1)
    {
        if (bEndOfFile())
        {
            strTarget.Clear();
            return kCIDLib::False;
        }

        // Read a line into the line buffer
        GetLine(strTarget);
        m_c4CurLine++;

        if (m_bStripSpace)
        {
            strTarget.StripWhitespace();

            if (!strTarget.bEmpty() && (strTarget.chFirst() != L';'))
                break;
        }
    }

    // Expand any keywords, unless its disabled
    if (!m_bDisableMacros)
        ExpandKeywords(strTarget);

    return kCIDLib::True;
}


tCIDLib::TCh TLineSpooler::chGetNext()
{
    // If EOF, then get out now
    if (m_c4CurOfs >= m_c4BufSize)
        return 0;

    tCIDLib::TCh chRet = m_pchReadBuf[m_c4CurOfs++];

    //
    //  If this char is a 0x0D/0x0A combo, then eat the 0x0A. If its an
    //  0x0D, just return an 0x0A.
    //
    if (chRet == 0x0D)
    {
        if ((m_c4CurOfs < m_c4BufSize) && (m_pchReadBuf[m_c4CurOfs] == 0x0A))
            m_c4CurOfs++;
        chRet = 0x0A;
    }
    return chRet;
}


tCIDLib::TVoid TLineSpooler::Reset()
{
    // Since we just spool out of a buffer, this is all it takes
    m_c4CurLine = 0;
    m_c4CurOfs  = 0;
}


// ---------------------------------------------------------------------------
//  TLineSpooler: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This is called after a line is read in. We search for keywords, and
//  expand them. THey are in the form:
//
//      $(keyword)
//
tCIDLib::TVoid TLineSpooler::ExpandKeywords(TBldStr& strToExpand)
{
    // A local enum to use in our state machine
    enum EStates
    {
        EState_Input
        , EState_OpenParen
        , EState_MacroName
    };

    // If the source is empty, we are done
    if (strToExpand.bEmpty())
        return;

    // Buffers to build up macro names and values and such
    TBldStr strMacroName;
    TBldStr strMacroValue;
    TBldStr strTmp;

    //
    //  We work through the source string one character at a time. Until
    //  we hit a macro reference we just copy them to the output string.
    //  When we hit a macro reference, we pull it out, look it up, and
    //  copy its text into the target string.
    //
    //  We continue this process, over and over, until all recursive macros
    //  have been expanded or we detect a recursive definition.
    //
    tCIDLib::TBoolean   bExpanded = kCIDLib::True;
    tCIDLib::TCard4     c4RoundCount = 0;
    tCIDLib::TCard4     c4SrcInd;
    EStates             eState;

    while (bExpanded)
    {
        // Bump up the round counter
        c4RoundCount++;

        //
        //  If it hits 64, then we probably have some recursive macro
        //  definition, so break out.
        //
        if (c4RoundCount == 64)
            break;

        // Reset everthing for this round
        bExpanded = kCIDLib::False;
        c4SrcInd = 0;
        eState = EState_Input;

        //
        //  For this round, copy the output string that we built up on
        //  the last round do the temp string, and clear out the output
        //  string.
        //
        strTmp = strToExpand;
        strToExpand.Clear();

        // And process the input again for any macros to expand
        while (kCIDLib::True)
        {
            // Get the current source character
            const tCIDLib::TCh chCur = strTmp[c4SrcInd++];

            // Is it a null char? If so, then we are done for this round
            if (chCur == kCIDLib::chNull)
                break;

            switch(eState)
            {
                case EState_Input :
                    //
                    //  If its a dollar sign, then change state, else just
                    //  store in the output.
                    //
                    if (chCur == kCIDLib::chDollarSign)
                        eState = EState_OpenParen;
                     else
                        strToExpand.AppendCh(chCur);
                    break;

                case EState_OpenParen :
                    //
                    //  If this char is a dollar also, then it was an escaped
                    //  dollar, so store one dollar and go back to input state.
                    //
                    //  If its an open paren, move forward to the next state.
                    //  Else, just reset and assume it just happened to be a
                    //  dollar sign at random.
                    //
                    if (chCur == kCIDLib::chDollarSign)
                    {
                        eState = EState_Input;
                        strToExpand.AppendCh(chCur);
                    }
                     else if (chCur == kCIDLib::chOpenParen)
                    {
                        eState = EState_MacroName;
                        strMacroName.Clear();
                    }
                     else
                    {
                        eState = EState_Input;
                    }
                    break;

                case EState_MacroName :
                    //
                    //  We are now in the macro name part. So we copy chars to
                    //  the szMacroName buffer until we hit the close paren.
                    //
                    if (chCur == kCIDLib::chCloseParen)
                    {
                        //
                        //  We have the macro name now. Note that there are a
                        //  few that are special because they cannot be done
                        //  until later. So we check for those and just spit
                        //  the macro text back out again, to be interpreted
                        //  later. I.e. we set the expanded macro value to
                        //  the original macro that we just parsed. Inefficient
                        //  but easier to do this way.
                        //
                        if (strMacroName == L"ProjDir")
                        {
                            strMacroValue = L"$(ProjDir)";
                        }
                         else
                        {
                            if (!facCIDBuild.bGetMacroValue(strMacroName
                                                            , strMacroValue))
                            {
                                stdOut  << L"(Line " << m_c4CurLine
                                        << L") Could not find macro: "
                                        << strMacroName << kCIDBuild::EndLn;
                                throw tCIDBuild::EErrors::FileFormat;
                            }
                        }

                        // Append this value to the output buffer
                        strToExpand.Append(strMacroValue);

                        // Reset the state to input again
                        eState = EState_Input;

                        // And remember that we expanded something
                        bExpanded = kCIDLib::True;
                    }
                     else
                    {
                        strMacroName.AppendCh(chCur);
                    }
                    break;
            }
        }
    }
}


tCIDLib::TVoid TLineSpooler::GetLine(TBldStr& strBuf)
{
    const tCIDLib::TCard4  c4Max = 2048;
    tCIDLib::TCh          szTmp[c4Max+1];

    //
    //  Copy over characters until we hit the max chars or the next end
    //  of line character.
    //
    tCIDLib::TCard4 c4Count = 0;
    tCIDLib::TCh    chCur = 0;
    while (c4Count < c4Max)
    {
        chCur = chGetNext();

        // Watch for EOF
        if (!chCur)
            break;

        // Break out on a new line
        if (chCur == L'\n')
            break;

        // Not a new line, so store it
        szTmp[c4Count++] = chCur;
    }

    // Cap off the string buffer
    szTmp[c4Count] = 0;

    // And copy to the caller's buffer
    strBuf = szTmp;
}


tCIDLib::TCh* TLineSpooler::pchAllocReadBuf(tCIDLib::TCard4& c4CharsRead)
{
    // Find the file, to make sure it exists and to get its size
    TFindInfo fndiFile;
    if (!TFindInfo::bFindAFile(m_strFileName, fndiFile))
    {
        stdOut  << L"  TLineSpooler - Could not find file: "
                << m_strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileNotFound;
    }

    // Get the size of the file contents
    const tCIDLib::TCard4 c4FileSize = fndiFile.c4Size();

    // If its zero sized, then deal with that now
    if (!c4FileSize)
    {
        c4CharsRead = 0;
        return TRawStr::pszDupString(L"");
    }

    //
    //  Try to open the file. The default open action is to open an existing
    //  file, so an exception will occur if it does not exist.
    //
    TBinFile bflSrc(m_strFileName);

    //
    //  Seems like we got it, so allocate a buffer to read it into. We
    //  don't know its format yet, so just make it a raw byte buffer.
    //
    tCIDLib::TCard1* pbTmp = new tCIDLib::TCard1[c4FileSize];

    //
    //  Try to read the whole file into our buffer and confirm that we
    //  read that much. If not, its an error so give up.
    //
    tCIDLib::TCard4 c4Read = bflSrc.c4Read(pbTmp, c4FileSize);
    if (c4Read != c4FileSize)
    {
        delete [] pbTmp;
        stdOut  << L"  TLineSpooler - Could not read in file: "
                << m_strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Internal;
    }

    //
    //  When we are done, then pszActualBuf points to the actual buffer we
    //  want to use, i.e. what we return. And c4CharsRead will be set to the
    //  wide character chars we are returning.
    //
    //
    tCIDLib::TCh* pszActualBuf = 0;
    c4CharsRead = 0;

    //
    //  Now we need to get the data into wide char format, in the native
    //  byte order. So look for the Unicode marker. If there, then its in
    //  UTF-16 format, so we need to get it into local byte order wide char
    //  format. If not, then assume its in the local code page format and
    //  convert it.
    //
    tCIDLib::TUniCh uchAsUni = *reinterpret_cast<tCIDLib::TUniCh*>(pbTmp);
    if ((uchAsUni == 0xFEFF) || (uchAsUni == 0xFFFE))
    {
        tCIDLib::TBoolean bSwapped = kCIDLib::False;
        if (uchAsUni == 0xFEFF)
        {
            #if defined(CIDBUILD_BIGENDIAN)
            bSwapped = kCIDLib::True;
            #endif
        }
         else
        {
            #if defined(CIDBUILD_LITTLEENDIAN)
            bSwapped = kCIDLib::True;
            #endif
        }

        // Convert the buffer size to a Unicode char count
        c4CharsRead = c4FileSize / kCIDLib::c4UniBytes;

        //
        //  Bump it down to account for the byte order mark, which we will
        //  discard below.
        //
        c4CharsRead--;

        //
        //  If the wide char and Unicode char are the same size, we can just
        //  keep the current buffer, though we may have to byte flip it. If
        //  they are not the same size, then allocate a wide char buffer and
        //  load into it, handling byte order if required.
        //
        //  Note that we move the bytes down as we go in all these loops,
        //  in order to throw away the byte order mark.
        //
        if (kCIDLib::c4CharBytes == kCIDLib::c4UniBytes)
        {
            // We can just keep the buffer we read into, cast to the right type
            pszActualBuf = reinterpret_cast<tCIDLib::TCh*>(pbTmp);

            //
            //  If its swapped, then run through the buffer and swap it all,
            //  throwing away the byte order mark as we go. If not swapped,
            //  we just need to get rid of the byte order mark.
            //
            if (bSwapped)
            {
                for (unsigned int c4Index = 0; c4Index < c4CharsRead; c4Index++)
                {
                    const tCIDLib::TCh chCur = pszActualBuf[c4Index + 1];
                    pszActualBuf[c4Index] = (chCur >> 8) | (chCur << 8);
                }
            }
             else
            {
                for (unsigned int c4Index = 0; c4Index < c4CharsRead; c4Index++)
                    pszActualBuf[c4Index] = pszActualBuf[c4Index + 1];
            }
        }
         else
        {
            //
            //  Allocate a wide character buffer big enough to hold all of
            //  the chars in the buffer.
            //
            pszActualBuf = new tCIDLib::TCh[c4CharsRead];

            //
            //  Copy over the chars to the new buffer, flipping as we go if
            //  that is required.
            //
            tCIDLib::TUniCh* pszAsUni = reinterpret_cast<tCIDLib::TUniCh*>(pbTmp);
            if (bSwapped)
            {
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4CharsRead; c4Index++)
                {
                    const tCIDLib::TUniCh chCur = pszAsUni[c4Index + 1];
                    pszActualBuf[c4Index] = (chCur >> 8) | (chCur << 8);
                }
            }
             else
            {
                for (tCIDLib::TCard4 c4Index = 0; c4Index < c4CharsRead; c4Index++)
                        pszActualBuf[c4Index] = pszAsUni[c4Index + 1];
            }

            // And delete the original buffer
            delete [] pbTmp;
        }
    }
     else
    {
        //
        //  Its not Unicode, so we have to transcode it into another buffer.
        //  It will set c4CharsRead to the count of Unicode chars actually
        //  transcoded out.
        //
        pszActualBuf = TRawStr::pszTranscode
        (
            reinterpret_cast<tCIDLib::TSCh*>(pbTmp)
            , c4CharsRead
            , c4FileSize
        );

        // And clean up the old buffer
        delete [] pbTmp;
    }
    return pszActualBuf;
}
