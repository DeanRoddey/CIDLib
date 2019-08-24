//
// FILE NAME: CIDBuild_ResCompiler_Msgs.cpp
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
//  This file implements the message compiler parts of the resource compiler
//  class.
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
//  TResCompiler: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TResCompiler::bMassageMsgText(  const   TBldStr&        strOriginal
                                ,       TBldStr&        strToFill)
{
    // Init the buffer to empty
    strToFill.Clear();

    //
    //  If the first character is a quote, then this is set and white
    //  space is not trimmed.
    //
    tCIDLib::TBoolean bKeepWS = kCIDLib::False;

    // Loop until we get a whitespace or end of data
    tCIDLib::TCard4 c4SrcIndex = 0;
    while (1)
    {
        // Get the next source character
        tCIDLib::TCh chNext = strOriginal[c4SrcIndex++];

        // If its leading whitespace, then skip it
        if (strToFill.bEmpty() && TRawStr::bIsSpace(chNext) && !bKeepWS)
            continue;

        //
        //  If this is the first non-whitespace and its a quote, then this
        //  is a quoted message, so we toss this char and set the bKeepWS
        //  flag.
        //
        if (strToFill.bEmpty() && (chNext == L'"'))
        {
            bKeepWS = kCIDLib::True;
            continue;
        }

        //
        //  If we've hit the end of the line, then lets wrap up the
        //  string and do any whitespace stripping and whatnot, then
        //  return.
        //
        if ((chNext == L'\n') || !chNext)
        {
            // If the previous char is a CR, then eat it.
            if (!strToFill.bEmpty())
            {
                if (strToFill.chLast() == L'\r')
                    strToFill.DeleteLast();
            }

            //
            //  If bKeepWS is set, and the last char is a quote, then throw
            //  away the quote.
            //
            if (!strToFill.bEmpty() && bKeepWS)
            {
                if (strToFill.chLast() == L'"')
                    strToFill.DeleteLast();
            }
            break;
        }

        //
        //  If this is a backslash character, then we need to see if its
        //  one of the standard escape sequences and translate it into a
        //  single, binary control character. If not, just put out the
        //  slash itself.
        //
        if (chNext == kCIDBuild::chPathSep)
        {
            tCIDLib::TCh chPeek = strOriginal[c4SrcIndex];

            tCIDLib::TCh chActual = 0;
            if (chPeek == L'r')
                chActual = L'\r';
            else if (chPeek == L'n')
                chActual = L'\n';
            else if (chPeek == L't')
                chActual = L'\t';
            else
                chActual = kCIDBuild::chPathSep;

            strToFill.Append(chActual);
            c4SrcIndex++;
        }
         else
        {
            // Nothing special, just put it in the string
            strToFill.Append(chNext);
        }
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TResCompiler::bProcessMsgs()
{
    //
    //  This flag is used to optimize. If we have to rebuild because the output doesn't
    //  exist, then there is no need to rebuild the header file (which causes more
    //  rebuilds.) So this flag will be set below if any headers need to be regenerated.
    //
    //  If we are being forced, then just do them.
    //
    m_bGenHeaders = m_bForce;

    //
    //  See if we need to do anything. If the source file is newer than the target binary
    //  file or either of the header files do not exist, then we have to build. If the
    //  Force flag is on, then we just assume we have to build.
    //
    tCIDLib::TBoolean bBuild = m_bForce;

    try
    {
        // If the force flag is on and verbose is on, then announce forced update
        if (m_bVerbose && m_bForce)
            stdOut << L"    Updating messages due to /Force flag\n" << kCIDBuild::EndLn;

        // If the source is not found, we are in trouble and have to bail out.
        TFindInfo fndiSrc;
        if (!TFindInfo::bFindAFile(m_projiSrc.strMsgSrc(), fndiSrc))
        {
            stdOut  << L"The source .MsgText file: '"
                    << m_projiSrc.strMsgSrc()
                    << L"' was not found" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::NotFound;
        }

        if (!bBuild)
        {
            //
            //  See which of the files exist, and get their info, which will
            //  give us timestamp info.
            //
            TFindInfo fndiErrHpp;
            TFindInfo fndiMsgHpp;
            TFindInfo fndiTarget;

            if (!TFindInfo::bFindAFile(m_projiSrc.strOutErrHpp(), fndiErrHpp))
            {
                m_bGenHeaders = kCIDLib::True;
                bBuild = kCIDLib::True;
                if (m_bVerbose)
                {
                    stdOut  << L"    " << m_projiSrc.strOutErrHpp()
                            << L" does not exist" << kCIDBuild::EndLn;
                }
            }
             else if (!TFindInfo::bFindAFile(m_projiSrc.strOutMsgHpp(), fndiMsgHpp))
            {
                m_bGenHeaders = kCIDLib::True;
                bBuild = kCIDLib::True;
                if (m_bVerbose)
                {
                    stdOut  << L"    " << m_projiSrc.strOutMsgHpp()
                            << L" does not exist" << kCIDBuild::EndLn;
                }
            }
             else
            {
                //
                //  If the target file doesn't exist, then obviously we have
                //  to build.
                //
                if (!TFindInfo::bFindAFile(m_projiSrc.strOutMsgs(), fndiTarget))
                {
                    bBuild = kCIDLib::True;
                    if (m_bVerbose)
                    {
                        stdOut  << L"    " << m_projiSrc.strOutMsgs()
                                << L" does not exist" << kCIDBuild::EndLn;
                    }
                }
                 else
                {
                    //
                    //  If the source is newer than the target, then we have
                    //  to build, but not necessarily regen headers.
                    //
                    if (fndiSrc > fndiTarget)
                    {
                        bBuild = kCIDLib::True;
                        if (m_bVerbose)
                        {
                            stdOut  << L"    " << m_projiSrc.strMsgSrc()
                                    << L"     is newer than target"
                                    << kCIDBuild::EndLn;
                        }
                    }
                }

                //
                //  If the source is newthan the headers, then we have
                //  to rebuild and regen headers.
                //
                if (fndiSrc > fndiErrHpp)
                {
                    m_bGenHeaders = kCIDLib::True;
                    bBuild = kCIDLib::True;
                    if (m_bVerbose)
                    {
                        stdOut  << L"    " << m_projiSrc.strMsgSrc()
                                << L" is newer than error header"
                                << kCIDBuild::EndLn;
                    }
                }

                if (fndiSrc > fndiMsgHpp)
                {
                    m_bGenHeaders = kCIDLib::True;
                    bBuild = kCIDLib::True;
                    if (m_bVerbose)
                    {
                        stdOut  << L"    " << m_projiSrc.strMsgSrc()
                                << L" is newer than message header"
                                << kCIDBuild::EndLn;
                    }
                }
            }
        }

        // If we don't have to build, say we are done
        if (!bBuild)
        {
            //
            //  However, if we have a resource file, then when its built,
            //  we might need to access the symbols, so do a load of the
            //  message id symbols.
            //
            if (m_projiSrc.bHasResFile())
            {
                m_plsplInput = new TLineSpooler
                (
                    m_projiSrc.strMsgSrc().pszBuffer()
                    , kCIDLib::True
                );
                ProcessMsgFile(m_listMsgIds);
                delete m_plsplInput;
                m_plsplInput = 0;
            }
            return bBuild;
        }

        //
        //  Open a spooler for the input source file. This guy will handle
        //  transcoding the text to Unicode mode for us. Tell it to strip
        //  leading and trailing whitespace from lines since that is of no
        //  interest to us here.
        //
        m_plsplInput = new TLineSpooler
        (
            m_projiSrc.strMsgSrc().pszBuffer(), kCIDLib::True
        );

        //
        //  And now we can start processing the input file. This will build
        //  up the list of id information objects, which contains all of
        //  the info we need to build the output files.
        //
        ProcessMsgFile(m_listMsgIds);

        // And now we can clean up the input spooler
        delete m_plsplInput;
        m_plsplInput = 0;

        //
        //  Create the output headers files, if we have to regenerate headers.
        //  These need to be in ASCII mode since they will be C++ source files.
        //
        if (m_bGenHeaders)
            WriteMsgHeaders();

        // And output the binary message file data
        OutputBinMsgData(m_projiSrc.strOutMsgs(), m_listMsgIds);
    }

    catch(...)
    {
        stdOut  << L"Error occured during processing of file: "
                << m_projiSrc.strMsgSrc()
                << L". Cleaning up partially built output files..."
                << kCIDBuild::EndLn;

        // Close any output files that got created
        delete m_plsplInput;
        m_plsplInput = 0;
        throw;
    }

    //
    //  We return whether we built or not, because if we did, then the GUI
    //  resources kind of have to be done (since they could refer to message
    //  text symbols.)
    //
    return bBuild;
}


//
//  Writes out the loadable binary file that is used at runtime to load up
//  the generated loadable text for use. We write to wherever the m_pbflBinOut
//  file is pointing.
//
tCIDLib::TVoid
TResCompiler::OutputBinMsgData( const   TBldStr&        strPath
                                ,       TList<TIdInfo>& listToWrite)
{
    // Create the output file we'll write to
    TBinFile bflBinOut
    (
        strPath, tCIDLib::EAccessModes::Write, tCIDLib::ECreateActs::CreateAlways
    );

    // Get the count of ids we found
    const tCIDLib::TCard4 c4MsgCount = listToWrite.c4ElemCount();

    //
    //  Calculate the offset where the text starts, which is after the
    //  index section. Note we add 4 bytes because the number of messages
    //  is written first.
    //
    tCIDLib::TCard4 c4StartText = (sizeof(tCIDLib::TMsgIndexEntry) * c4MsgCount)
                                   + 4;

    // Write out the number of messages in the file
    bflBinOut << c4MsgCount;

    //
    //  Now that we know the offset of the text, we can write out the index
    //  information.
    //
    TList<TIdInfo>::TCursor cursIds(&listToWrite);
    if (cursIds.bResetIter())
    {
        tCIDLib::TCard4 c4NextText = c4StartText;
        do
        {
            const TIdInfo& idiCur = cursIds.tCurElement();

            // Fill in a message index entry for this element
            tCIDLib::TMsgIndexEntry mieCur;
            mieCur.midThis = idiCur.c4Id();
            mieCur.c4TextOffset = c4NextText;

            mieCur.c2TextChars = tCIDLib::TCard2
            (
                idiCur.strText().c4Length() + 1
            );

            // Now write out the structure
            bflBinOut.c4Write(&mieCur, sizeof(mieCur));

            // Add the bytes to the next text offset
            c4NextText += mieCur.c2TextChars * kCIDLib::c4CharBytes;

        }   while (cursIds.bNext());

        //
        //  Query the current position now. It should be the position that
        //  we calculated as the starting text position above.
        //
        if (bflBinOut.c4CurPos() != c4StartText)
        {
            stdOut  << L"The starting text position != the calculated position"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::Internal;
        }

        //
        //  Now we must make one last pass to write out the text of each
        //  message.
        //
        cursIds.bResetIter();
        do
        {
            const TIdInfo& idiCur = cursIds.tCurElement();
            const tCIDLib::TCard4 c4Bytes =
            (
                idiCur.strText().c4Length() + 1
            ) * kCIDLib::c4CharBytes;
            bflBinOut.c4Write(idiCur.strText().pszBuffer(), c4Bytes);

        }   while (cursIds.bNext());
    }
}


tCIDLib::TVoid TResCompiler::ProcessMsgFile(TList<TIdInfo>& listToFill)
{
    //
    //  Lets go into the file reading loop. We read through the whole file
    //  here.
    //
    TBldStr strInBuf;

    // First we have to see a control block
    ProcessMsgCtrl();

    // Now we can see other blocks
    while (kCIDLib::True)
    {
        //
        //  Get the next input line, breaking out when no more. Note that
        //  we told the spooler to eat empty lines and comment lines, and
        //  to strip leading and trailing whitespace before giving us the
        //  lines.
        //
        if (!m_plsplInput->bReadLine(strInBuf))
            break;

        //
        //  We have to see either a MESSAGES= or ERRORS= keyword, to start
        //  a block of messages or errors. If it is, then set the type for
        //  the current block, else its an error so give up.
        //
        TIdInfo::ETypes eMsgType;
        tCIDLib::TCard4 c4MaxId, c4MinId;
        if (strInBuf == L"MESSAGES=")
        {
            eMsgType = TIdInfo::ETypes::Msg;
            c4MinId = kCIDLib::c4MsgId_First;
            c4MaxId = kCIDLib::c4MsgId_Last;
        }
         else if (strInBuf == L"ERRORS=")
        {
            eMsgType = TIdInfo::ETypes::Err;
            c4MinId = kCIDLib::c4ErrId_First;
            c4MaxId = kCIDLib::c4ErrId_Last;
        }
         else if (strInBuf == L"COMMON=")
        {
             eMsgType = TIdInfo::ETypes::Common;
             c4MinId = kCIDLib::c4ComId_First;
             c4MaxId = kCIDLib::c4ComId_Last;
        }
         else
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") MESSAGES= or ERRORS=" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }

        //
        //  Ok now we are processing either errors or messages until we
        //  see the end of the block.
        //
       TBldStr strEndBlock(L"END ");
       strEndBlock.Append(strInBuf);
       strEndBlock.DeleteLast();

        while (kCIDLib::True)
        {
            if (!m_plsplInput->bReadLine(strInBuf))
            {
                // Not legal to end here
                stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                        << L") File ended while in a message or error block"
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::UnexpectedEOF;
            }

            // Watch for the end of block marker
            if (strInBuf == strEndBlock)
                break;

            //
            //  Looks like a message line, or at least its supposed to be,
            //  so process it into a new id info object, and add that to
            //  the passed list.
            //
            TIdInfo* pidiNew = pidiProcessMsg(strInBuf, eMsgType);
            if ((pidiNew->c4Id() < c4MinId) || (pidiNew->c4Id() > c4MaxId))
            {
                stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                        << L") Msgs of this type must have ids from "
                        << c4MinId << L" to " << c4MaxId
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::IndexError;
            }
            listToFill.Add(pidiNew);
        }
    }
}


TIdInfo* TResCompiler::pidiProcessMsg(          TBldStr&        strFirstLine
                                        , const TIdInfo::ETypes eMsgType)
{
    //
    //  Parse out the three parts of the file, which are the id, the
    //  id number, and the text.
    //
    TBldStr strName;
    TBldStr strId;
    TBldStr strText;

    if (!bFindMsgEntryParts(strFirstLine, strName, strId, strText))
    {
        stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                << L") Incorrect error or message entry"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileFormat;
    }

    // Make sure the id is legal
    tCIDLib::TCard4 c4Id;
    if (!TRawStr::bXlatCard4(strId.pszBuffer(), c4Id, 0))
    {
        stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                << L") Id field was not a valid number"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileFormat;
    }

    TBldStr strNewText;
    if (!bMassageMsgText(strText, strNewText))
    {
        stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                << L") Msg text field was not validly formatted"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileFormat;
    }

    // Create a new id info object and return it
    return new TIdInfo(strName, eMsgType, c4Id, strNewText);
}


tCIDLib::TVoid TResCompiler::WriteMsgHeaders()
{
    //
    //  We write out some headers to our output files, which indicate that
    //  they are machine generated by this program.
    //
    const tCIDLib::TSCh* const pszHeaderHdr =
            "\n//\n"
            "// This file is created automatically by the CIDBuild utility \n"
            "// from the project's .MsgText file. Do not edit!\n"
            "//\n\n";

    //
    //  Create the two output files. We first write them to temp files
    //  until they are completely written out successfully, then we will
    //  rename them to the real thing (if the new ones are different
    //  from the old ones.)
    //
    TBldStr strTmpErrHpp(m_projiSrc.strOutErrHpp());
    strTmpErrHpp.Append(L"_Tmp");

    TBldStr strTmpMsgHpp(m_projiSrc.strOutMsgHpp());
    strTmpMsgHpp.Append(L"_Tmp");
    {
        TTextFile tflErrHpp
        (
            strTmpErrHpp
            , tCIDBuild::EEncodings::ASCII
            , tCIDLib::EAccessModes::Write
            , tCIDLib::ECreateActs::CreateAlways
        );

        TTextFile tflMsgHpp
        (
            strTmpMsgHpp
            , tCIDBuild::EEncodings::ASCII
            , tCIDLib::EAccessModes::Write
            , tCIDLib::ECreateActs::CreateAlways
        );

        tflErrHpp   << "#pragma once\n"
                    << "//\n// FILE: "
                    << m_projiSrc.strOutErrHpp()
                    << pszHeaderHdr
                    << "namespace "
                    << m_strErrNamespace
                    << "\n{\n";

        tflMsgHpp   << "#pragma once\n"
                    << "//\n// FILE: "
                    << m_projiSrc.strOutMsgHpp()
                    << pszHeaderHdr
                    << "namespace "
                    << m_strMsgNamespace
                    << "\n{\n";

        //
        //  Loop through the ids and write them out to their respective headers
        //  files.
        //
        TList<TIdInfo>::TCursor cursIds(&m_listMsgIds);
        if (cursIds.bResetIter())
        {
            do
            {
                // Get a ref to the current id
                const TIdInfo& idiCur = cursIds.tCurElement();

                TBldStr strIdFmt;
                strIdFmt.Format(idiCur.c4Id());

                if (idiCur.eType() == TIdInfo::ETypes::Msg)
                {
                    tflMsgHpp   << "    const tCIDLib::TMsgId "
                                << idiCur.strName().pszBuffer()
                                << " = "
                                << strIdFmt
                                << ";\n";
                }
                 else
                {
                    tflErrHpp   << "    const tCIDLib::TErrCode "
                                << idiCur.strName().pszBuffer()
                                << " = "
                                << strIdFmt
                                << ";\n";
                }
            }   while (cursIds.bNext());
        }

        // Cap off the Hpp files
        tflErrHpp << "}\n";
        tflMsgHpp << "}\n";
    }

    // And update the originals if needed
    TUtils::bUpdateOutputFile(strTmpErrHpp, m_projiSrc.strOutErrHpp());
    TUtils::bUpdateOutputFile(strTmpMsgHpp, m_projiSrc.strOutMsgHpp());
}


tCIDLib::TBoolean
TResCompiler::bFindMsgEntryParts(TBldStr&   strSrcLine
                                , TBldStr&  strName
                                , TBldStr&  strId
                                , TBldStr&  strText)
{
    tCIDLib::TCh* pszCur = strSrcLine.pszBuffer();
    tCIDLib::TCh* pszEnd = pszCur;

    //
    //  First should be the name, so lets store the current position in
    //  the pszName parameter. But run up past any spaces that are between
    //  the start of the line and the name.
    //
    while (TRawStr::bIsSpace(*pszCur))
        pszCur++;

    //
    //  Now run the end up to the next whitespace, which is the end of the
    //  name, and put a null there.
    //
    pszEnd = pszCur;
    while (!TRawStr::bIsSpace(*pszEnd))
        pszEnd++;
    if (!*pszEnd)
        return kCIDLib::False;
    *pszEnd = 0;
    strName = pszCur;

    // Move our current pointer past the end of the name
    pszCur = pszEnd + 1;

    //
    //  Now lets run up to the next non-whitespace. This will be the first
    //  char of the id, so store it in the pszId parameter.
    //
    while (TRawStr::bIsSpace(*pszCur))
        pszCur++;
    if (!*pszCur)
        return kCIDLib::False;

    //
    //  Run the end up to the next whitespace character. This will be the
    //  end of the id, so put a null there.
    //
    pszEnd = pszCur;
    while (!TRawStr::bIsSpace(*pszEnd))
        pszEnd++;
    if (!*pszEnd)
        return kCIDLib::False;
    *pszEnd = 0;
    strId = pszCur;

    // And move the current pointer past the end of the id
    pszCur = pszEnd + 1;

    //
    //  And now run up to the next non-whitespace. This will be the first
    //  character of the text of the message, so store it in the pszText
    //  parameter.
    //
    while (TRawStr::bIsSpace(*pszCur))
        pszCur++;
    if (!*pszCur)
        return kCIDLib::False;

    //
    //  The text can be continued with a \ character at the end of the line.
    //  so lets loop as long as we have continuations, and just append it
    //
    strText = pszCur;
    while (strText.chLast() == kCIDBuild::chPathSep)
    {
        // Throw away the trailing slash
        strText.DeleteLast();

        // Read in the next line
        if (!m_plsplInput->bReadLine(strSrcLine))
        {
            // Not legal to end here
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") File ended after a line continuation"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        // Throw away any leading/trailing whitespace
        strSrcLine.StripWhitespace();

        // And append it to our current text
        strText.Append(strSrcLine);
    }

    return kCIDLib::True;
}


// Called to handle the CTRL= block that must come first in the message text
tCIDLib::TVoid TResCompiler::ProcessMsgCtrl()
{
    TBldStr strInBuf;

    // The first non-comment line has to be the CTRL= line
    if (!m_plsplInput->bReadLine(strInBuf)
    ||   (strInBuf != L"CTRL="))
    {
        stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                << L") Expected to see the required CTRL= block first"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileFormat;
    }

    // Now process any statements in this block
    while (kCIDLib::True)
    {
        //
        //  Get the next input line, breaking out when no more. Note that
        //  we told the spooler to eat empty lines and comment lines, and
        //  to strip leading and trailing whitespace before giving us the
        //  lines.
        //
        if (!m_plsplInput->bReadLine(strInBuf))
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") File ended within CTRL= block"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        // Break out on the end line of the block
        if (strInBuf == L"END CTRL")
            break;

        if (strInBuf.bStartsWith(L"ErrPref=")
        ||  strInBuf.bStartsWith(L"MsgPref="))
        {
            const tCIDLib::TBoolean bErrs = strInBuf.bStartsWith(L"ErrPref=");
            strInBuf.Cut(8);

            if (strInBuf.c4Length() < 3)
            {
                stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                        << L") Expected a namespace prefix here"
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::FileFormat;
            }

            if (bErrs)
                m_strErrNamespace = strInBuf;
            else
                m_strMsgNamespace = strInBuf;
        }
         else
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Expected msg file control statement or END CTRL"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
    }
}


