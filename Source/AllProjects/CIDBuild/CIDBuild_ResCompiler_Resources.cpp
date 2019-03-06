//
// FILE NAME: CIDBuild_ResCompiler_Msgs.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/29/1998
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the main GUI resource compilation code.
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
//  TResCompiler: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TResCompiler::ProcessResources(const tCIDLib::TBoolean bMsgsBuilt)
{
    // Initialize the resource file header structure
    m_reshOut.c4MagicVal = kCIDCtrls::c4ResMagicVal;
    m_reshOut.c4FmtVersion = kCIDCtrls::c4ResFmtVersion;
    m_reshOut.c4ResCount = 0;
    m_reshOut.c4Size = 0;

    //
    //  If we are in force mode, or if the messages were built, then we have
    //  to assume the worst. Else, do the normal checks.
    //
    tCIDLib::TBoolean bBuild = m_bForce | bMsgsBuilt;

    TBldStr strTmpBinPath;
    TBldStr strTmpHdrPath;
    try
    {
        //
        //  Change to the project directory. This way, any relative references
        //  to files in the CIDRC file will naturally be correct.
        //
        if (!TUtils::bChangeDir(m_projiSrc.strProjectDir()))
        {
            stdOut  << L"Could not change to project directory: "
                    << m_projiSrc.strProjectDir() << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::Internal;
        }

        //
        //  If the source file doesn't exist, then we are in serious trouble
        //  and have to bail out.
        //
        TFindInfo fndiSrc;
        if (!TFindInfo::bFindAFile(m_projiSrc.strResSrc(), fndiSrc))
        {
            stdOut  << L"The source .CIDRC file: "
                    << m_projiSrc.strResSrc()
                    << L" was not found" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::NotFound;
        }

        // If the force flag is on and verbose is on, then announce forced update
        if (m_bVerbose && m_bForce)
            stdOut << L"    Updating resources due to /Force flag\n" << kCIDBuild::EndLn;

        //
        //  This flag is used to optimize. If we have to rebuild because the
        //  output doesn't exist, then there is no need to rebuild the header
        //  file (which causes more rebuilds.) So this flag will be set below
        //  if any headers need to be regenerated.
        //
        //  If we are being forced, then just do them.
        //
        m_bGenHeaders = m_bForce;

        //
        //  If we aren't forced to build, then see if we need for any immediately
        //  obvious reason, such as the target's not existing or the main
        //  source file being newer.
        //
        TFindInfo   fndiHeader;
        TFindInfo   fndiTarget;
        if (!m_bForce)
        {
            //
            //  Get time stamp info on the output header. If it doesn't exist
            //  then we obviously have to build and regen headers.
            //
            if (!TFindInfo::bFindAFile(m_projiSrc.strOutResHpp(), fndiHeader))
            {
                m_bGenHeaders = kCIDLib::True;
                bBuild = kCIDLib::True;
                if (m_bVerbose)
                {
                    stdOut  << L"    " << m_projiSrc.strOutResHpp()
                            << L"  does not exist"
                            << kCIDBuild::EndLn;
                }
            }
             else
            {
                //
                //  And check the target. If it doesn't exist then we have to
                //  build, but might not have to generate headers.
                //
                if (!TFindInfo::bFindAFile(m_projiSrc.strOutRes(), fndiTarget))
                {
                    bBuild = kCIDLib::True;
                    if (m_bVerbose)
                    {
                        stdOut  << L"    " << m_projiSrc.strOutRes()
                                << L" does not exist"
                                << kCIDBuild::EndLn;
                    }
                }
                 else
                {
                    //
                    //  If it exists, we only have to build if the source is
                    //  newer than the target.
                    //
                    if (fndiSrc > fndiTarget)
                    {
                        bBuild = kCIDLib::True;
                        if (m_bVerbose)
                        {
                            stdOut  << L"    " << m_projiSrc.strOutRes()
                                    << L" is older than the source file"
                                    << kCIDBuild::EndLn;
                        }
                    }
                }

                //
                //  At this point, we only have to regenerate the header if
                //  the source is newer than it is.
                //
                if (fndiSrc > fndiHeader)
                {
                    m_bGenHeaders = kCIDLib::True;
                    bBuild = kCIDLib::True;
                    if (m_bVerbose)
                    {
                        stdOut  << L"    " << m_projiSrc.strOutResHpp()
                                << L" is older than the source file"
                                << kCIDBuild::EndLn;
                    }
                }
            }
        }
         else if (bMsgsBuilt && !m_bForce)
        {
            //
            //  If we are building because the message file was updated,
            //  then the checks above didn't happen. But, we don't have
            //  to generate the header just because of that. So check
            //  whether the header exists. If it does, then we don't have
            //  to do it.
            //
            //  But if being forced, do it regardless
            //
            m_bGenHeaders = !TUtils::bExists(m_projiSrc.strOutResHpp());
        }

        //
        //  Open a spooler for the input source file. This guy will handle
        //  transcoding the text to Unicode mode for us. Tell it to strip
        //  leading and trailing whitespace from lines since that is of no
        //  interest to us here.
        //
        m_plsplInput = new TLineSpooler(m_projiSrc.strResSrc(), kCIDLib::True);

        //
        //  Do a rough scan of the file. This achieves two goals. If we aren't
        //  already going to build, it will check to see whether any files
        //  that are referenced in the CIDRC file are newer than the target,
        //  in which case we need to build. And, if we do build, it will count
        //  the number of resources defined, which we need to know before we
        //  start building, so that we can leave space at the beginning of
        //  the file for the map info.
        //
        if (bScanForUpdates(fndiTarget, !bBuild))
            bBuild = kCIDLib::True;

        // Build if we have to
        if (bBuild)
        {
            m_plsplInput->Reset();

            //
            //  Create the binary and text header output files. We write
            //  first to temp files and only replace the previous output
            //  files if the new ones are different.
            //
            strTmpBinPath = m_projiSrc.strOutRes();
            strTmpBinPath.Append(L"_Tmp");
            strTmpHdrPath = m_projiSrc.strOutResHpp();
            strTmpHdrPath.Append(L"_Tmp");

            TBinFile bflBinOut
            (
                strTmpBinPath
                , tCIDLib::EAccessModes::Write
                , tCIDLib::ECreateActs::CreateAlways
            );

            // And create the output header file if needed
            TTextFile tflResHpp;
            if (m_bGenHeaders)
            {
                tflResHpp.Open
                (
                    strTmpHdrPath
                    , tCIDBuild::EEncodings::ASCII
                    , tCIDLib::EAccessModes::Write
                    , tCIDLib::ECreateActs::CreateAlways
                );
            }

            //
            //  Do the initial write out of the header, and the empty map items
            //  that we'll come back and fill in later.
            //
            bflBinOut.c4Write(&m_reshOut, sizeof(m_reshOut));

            tCIDLib::TResItemMap rimTmp = {0};
            for (tCIDLib::TCard4 c4Index = 0; c4Index < m_reshOut.c4ResCount; c4Index++)
                bflBinOut.c4Write(&rimTmp, sizeof(rimTmp));

            //
            //  Now reset the resource counter, which will now be used as an
            //  index as we parse each resource. Remember it though, and compare
            //  them again at the end, to make sure that things went right.
            //
            const tCIDLib::TCard4 c4FirstResCount = m_reshOut.c4ResCount;
            m_reshOut.c4ResCount = 0;

            // Process the CTRL block which must be first
            ProcessResCtrl(tflResHpp);

            //
            //  Write out the open part of the resource id header if
            //  generating headers. Note that the resource control block
            //  that we just processed can override the default namespace
            //  so we have to do this after that.
            //
            if (m_bGenHeaders)
            {
                tflResHpp   << L"#pragma once\n"
                            << L"namespace " << m_strResNamespace
                            << L"\n{" << kCIDBuild::EndLn;
            }

            //
            //  OK, lets parse through the top level stuff. When we see a resource
            //  we'll branch off to a method that handles that type of resource.
            //
            TBldStr strReadBuf;
            while (kCIDLib::True)
            {
                // Get the next line. Break out when we hit the ned
                if (!m_plsplInput->bReadLine(strReadBuf))
                    break;

                //
                //  If this is one of the ones we are interested in, then
                //  lets handle it. Else, just scan for the end marker.
                //
                if (strReadBuf == L"CTRL=")
                {
                    //
                    //  This one was handled specially above and it must be
                    //  the first block, so it we can't see another one.
                    //
                    stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                            << L") The CTRL= block must be the first block in the resource file"
                            << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::FileFormat;
                }
                 else if (strReadBuf == L"ICONS=")
                {
                    ProcessImages
                    (
                        L"END ICONS"
                        , tCIDLib::EResTypes::Icon
                        , bflBinOut
                        , tflResHpp
                    );
                }
                 else if (strReadBuf == L"BITMAPS=")
                {
                    ProcessImages
                    (
                        L"END BITMAPS"
                        , tCIDLib::EResTypes::Bitmap
                        , bflBinOut
                        , tflResHpp
                    );
                }
                 else if (strReadBuf == L"PNGS=")
                {
                    ProcessImages
                    (
                        L"END PNGS"
                        , tCIDLib::EResTypes::PNG
                        , bflBinOut
                        , tflResHpp
                    );
                }
                 else if (strReadBuf == L"JPEGS=")
                {
                    ProcessImages
                    (
                        L"END JPEGS"
                        , tCIDLib::EResTypes::JPEG
                        , bflBinOut
                        , tflResHpp
                    );
                }
                 else if (strReadBuf == L"DIALOG=")
                {
                    ProcessDialog(bflBinOut, tflResHpp);
                }
                 else if (strReadBuf == L"MENU=")
                {
                    ProcessMenu(bflBinOut, tflResHpp);
                }
                 else
                {
                    stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                            << L") Expected CTRL=, ICONS=, PNGS=, JPEGS=, BITMAPS=, MENU=, or DIALOG="
                            << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::FileFormat;
                }
            }

            // Do a sanity check
            if (m_reshOut.c4ResCount != c4FirstResCount)
            {
                stdOut  << L"First and second resource counts don't match"
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::Internal;
            }

            // Write out the closing part of the resource id header, if needed
            if (m_bGenHeaders)
                tflResHpp << L"\n}\n" << kCIDBuild::EndLn;

            // Fill in the final size field in the header and seek back and write it
            m_reshOut.c4Size = bflBinOut.c4CurPos();
            bflBinOut.SeekTo(0);
            bflBinOut.c4Write(&m_reshOut, sizeof(m_reshOut));

            // Clean up the files, which will flush them out
            bflBinOut.Close();
            tflResHpp.Close();

            // Update the original files if required, else toss the new ones
            TUtils::bUpdateOutputFile(strTmpBinPath, m_projiSrc.strOutRes());
            if (m_bGenHeaders)
                TUtils::bUpdateOutputFile(strTmpHdrPath, m_projiSrc.strOutResHpp());
        }
    }

    catch(...)
    {
        stdOut  << L"Error occured during processing of file: "
                << m_projiSrc.strResSrc()
                << L". Cleaning up partially built output files..."
                << kCIDBuild::EndLn;

        if (TUtils::bExists(strTmpBinPath)
        && !TUtils::bDeleteFile(strTmpBinPath))
        {
            stdOut  << L"Couldn't delete file: " << strTmpBinPath
                    << kCIDBuild::EndLn;
        }

        if (TUtils::bExists(strTmpHdrPath)
        &&  !TUtils::bDeleteFile(strTmpHdrPath))
        {
            stdOut  << L"Couldn't delete file: " << strTmpHdrPath
                    << kCIDBuild::EndLn;
        }
        throw;
    }
}



// ---------------------------------------------------------------------------
//  TResCompiler: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  This method just does a rough scan of the source file, looking for resources
//  which refer to other source files, and checks them again the passed target
//  file. If any is more recent than the target, it returns kCIDLib::True.
//
//  It also counts the number of resources, which is important for later use
//  if we need to buld.
//
tCIDLib::TBoolean
TResCompiler::bScanForUpdates(  const   TFindInfo&          fndiTarget
                                , const tCIDLib::TBoolean   bCheckDates)
{
    TBldStr             strReadBuf;
    TFindInfo           fndiFile;
    tCIDLib::TBoolean   bRet = kCIDLib::False;
    while (kCIDLib::True)
    {
        // Get the next line, break out if no more input
        if (!m_plsplInput->bReadLine(strReadBuf))
            break;

        //
        //  If this is one of the ones we are interested in, then lets handle
        //  it. Else, just scan for the end marker.
        //
        if ((strReadBuf == L"ICONS=")
        ||  (strReadBuf == L"BITMAPS=")
        ||  (strReadBuf == L"JPEGS=")
        ||  (strReadBuf == L"PNGS="))
        {
            // Build up the name we are looking for (end of this block.)
            strReadBuf.DeleteLast();
            TBldStr strEnd(L"END ");
            strEnd.Append(strReadBuf);

            //
            //  These are both the same. We just want to skip over the symbol
            //  name and the id, then get the file name.
            //
            while (true)
            {
                if (!m_plsplInput->bReadLine(strReadBuf))
                {
                    stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                            << L") Expected resource or " << strEnd
                            << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::UnexpectedEOF;
                }

                // Watch for the end of this block
                if (strReadBuf == strEnd)
                    break;

                // Look for the opening quote of the file name
                tCIDLib::TSInt iOfs = strReadBuf.iFindFirst(L'"');
                if (iOfs == -1)
                {
                    stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                            << L") Badly formed resource. Expected quoted string as 3rd field"
                            << kCIDBuild::EndLn;
                    continue;
                }

                // Cut out the text up past the quote
                strReadBuf.Cut(tCIDLib::TCard4(iOfs + 1));

                // The last char should be a quote also
                if (strReadBuf.chLast() != L'"')
                {
                    stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                            << L") Badly formed resource. Quoted file name not terminate"
                            << kCIDBuild::EndLn;
                    continue;
                }

                // Cap it at the last character
                strReadBuf.DeleteLast();

                // And now check for this file.
                if (!TFindInfo::bFindAFile(strReadBuf, fndiFile))
                {
                    stdOut  << L"    Source file does not exist: "
                            << strReadBuf << kCIDBuild::EndLn;
                }

                // If its out of date, then set the return flag
                if (bCheckDates && (fndiFile > fndiTarget))
                {
                    if (m_bVerbose)
                    {
                        stdOut  << L"    Image file is newer: "
                                << strReadBuf << kCIDBuild::EndLn;
                    }
                    bRet = kCIDLib::True;
                }

                // Bump up the resource count
                m_reshOut.c4ResCount++;
            }
        }
         else if ((strReadBuf == L"CTRL=")
              ||  (strReadBuf == L"DIALOG=")
              ||  (strReadBuf == L"MENU="))
        {
            // Build up the name we are looking for (end of this block.)
            strReadBuf.DeleteLast();
            TBldStr strEnd(L"END ");
            strEnd.Append(strReadBuf);

            while (true)
            {
                if (!m_plsplInput->bReadLine(strReadBuf))
                {
                    stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                            << L") Expected resource or " << strEnd
                            << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::UnexpectedEOF;
                }

                if (strReadBuf == strEnd)
                    break;
            }

            // Bump up the resource count if not the control block
            if (strReadBuf != L"END CTRL")
                m_reshOut.c4ResCount++;
        }
         else
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Expected BITMAPS=, ICONS=, JPEGS=, PNGS= or DIALOG="
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
    }

    // No out of date files
    return bRet;
}


//
//  Handles parsing the CTRL= block, which must be the first one in the
//  file.
//
tCIDLib::TVoid TResCompiler::ProcessResCtrl(TTextFile& tflResHpp)
{
    TBldStr strReadBuf;
    if (!m_plsplInput->bReadLine(strReadBuf))
    {
        // There aren't any blocks, so just return
        return;
    }

    if (strReadBuf != L"CTRL=")
    {
        stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                << L") The CTRL= block must be the first block in the resource file"
                << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileFormat;
    }

    // Process this block until we hit the end
    while (true)
    {
        if (!m_plsplInput->bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") End of file occurred within CTRL= block"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        // Watch for the end of this block
        if (strReadBuf == L"END CTRL")
            break;

        if (strReadBuf.bStartsWith(L"ResPref="))
        {
            strReadBuf.Cut(8);
            if (strReadBuf.c4Length() < 3)
            {
                stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                        << L") Expected a resource prefix here"
                        << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::FileFormat;
            }
            m_strResNamespace = strReadBuf;
        }
         else
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Expected resource control statement or END CTRL"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
    }
}


//
//  Process any of the image definition blocks.
//
tCIDLib::TVoid
TResCompiler::ProcessImages(const   tCIDLib::TCh* const pszEndText
                            , const tCIDLib::EResTypes  eType
                            ,       TBinFile&           bflBinOut
                            ,       TTextFile&          tflResHpp)
{
    TBldStr strReadBuf;
    while (true)
    {
        if (!m_plsplInput->bReadLine(strReadBuf))
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Expected resource or " << pszEndText
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::UnexpectedEOF;
        }

        // Watch for the end of this block
        if (strReadBuf == pszEndText)
            break;

        //
        //  Break out the three parameters. They are:
        //
        //  symbol  id  filename
        //
        tCIDLib::TCh* pszTmp = strReadBuf.pszDupBuffer();
        TArrayJanitor<tCIDLib::TCh> janTmp(pszTmp);

        tCIDLib::TCh* pszSymbol = TRawStr::pszStrTok(pszTmp, kCIDLib::szWhitespace);
        tCIDLib::TCh* pszId = 0;
        tCIDLib::TCh* pszFile = 0;
        if (pszSymbol)
        {
            pszId = TRawStr::pszStrTok(0, kCIDLib::szWhitespace);
            if (pszId)
                pszFile =  TRawStr::pszStrTok(0, kCIDLib::szWhitespace);
        }

        //
        //  If we didn't get one of the fields, or if we got too many fields,
        //  then issue an error and bail out.
        //
        if (!pszSymbol
        ||  !pszId
        ||  !pszFile
        ||  TRawStr::pszStrTok(0, kCIDLib::szWhitespace))
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Badly formed resource. Expected: symbol  id  filename"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }

        // Get rid of the leading and trailing quote on the file name
        strReadBuf = pszFile;
        if ((strReadBuf.chFirst() != L'"') || (strReadBuf.chLast() != L'"'))
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Badly formed image file name, not quoted"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }
        strReadBuf.Cut(1);
        strReadBuf.DeleteLast();

        // Get the file info
        TFindInfo fndiFile;
        if (!TFindInfo::bFindAFile(strReadBuf, fndiFile))
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") image file" << strReadBuf
                    << L" doesn't exist" << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileNotFound;
        }

        //
        //  Start filling in the temp map item for this resource. We can set
        //  the type and its offset in the output file, which is our current
        //  position in the file. And we can store the size of the resource
        //  which is in the file find data.
        //
        tCIDLib::TResItemMap rimCur;
        rimCur.c4At = bflBinOut.c4CurPos();
        rimCur.eType = eType;
        rimCur.c4Size = fndiFile.c4Size();

        // And now try to convert the id to a number
        if (!TRawStr::bXlatCard4(pszId, rimCur.ridRes))
        {
            stdOut  << L"(Line " << m_plsplInput->c4CurLine()
                    << L") Resource id field must be a decimal number"
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::FileFormat;
        }

        //
        //  Ok, lets read the source file in and write it out to the output
        //  file.
        //
        TBinFile bflRes
        (
            strReadBuf
            , tCIDLib::EAccessModes::Read
            , tCIDLib::ECreateActs::OpenIfExists
        );

        tCIDLib::TCard1* pc1Buf = new tCIDLib::TCard1[rimCur.c4Size];
        TArrayJanitor<tCIDLib::TCard1> janBuf(pc1Buf);
        bflRes.c4Read(pc1Buf, rimCur.c4Size);
        bflBinOut.c4Write(pc1Buf, rimCur.c4Size);

        // Seek back and write out our map item
        bflBinOut.SeekTo
        (
            sizeof(tCIDLib::TResHeader)
            + (sizeof(tCIDLib::TResItemMap) * m_reshOut.c4ResCount)
        );
        bflBinOut.c4Write(&rimCur, sizeof(tCIDLib::TResItemMap));

        // Seek back to the end again
        bflBinOut.c4SeekToEnd();

        // Write out the header entry for this one, if needed
        if (m_bGenHeaders)
        {
            tflResHpp   << L"    const tCIDLib::TResId  "
                        << pszSymbol << L" = " << pszId << L";\n";
        }

        // Bump up the resource index
        m_reshOut.c4ResCount++;
    }
}

