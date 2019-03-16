//
// FILE NAME: CIDBuild_Utils.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/1998
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
//  This file implements the TUtils namespace, which just provides some
//  grunt work helper functions for the program. Some of these are implemented
//  in the per-platform section.
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
//  TUtils: Public methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TUtils::bCopyAll(const TBldStr& strSrcDir, const TBldStr& strTargetDir)
{
    TBldStr strSrcFile;
    TBldStr strDestFile;

    TList<TFindInfo> listFiles;
    TListCursor<TFindInfo> cursFiles(&listFiles);

    strSrcFile = strSrcDir;
    strSrcFile.Append(L"*");
    listFiles.RemoveAll();
    if (TFindInfo::c4FindFiles(strSrcFile, listFiles))
    {
        //
        //  Cursor through the files we found and copy them over
        //  to the output directory.
        //
        cursFiles.bResetIter();
        do
        {
            // Get the current find info object
            const TFindInfo& fndiCur = cursFiles.tCurElement();

            // Skip over any directories
            if (!fndiCur.bIsDirectory())
            {
                // Build up the source/dest file names
                strSrcFile = strSrcDir;
                strSrcFile.Append(fndiCur.strFileName());
                strDestFile = strTargetDir;
                strDestFile.Append(fndiCur.strFileName());

                if (!TUtils::bCopyFile(strSrcFile, strDestFile))
                    return kCIDLib::False;
            }
        }   while (cursFiles.bNext());
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean
TUtils::bCopyFile(  const   TBldStr&            strSourceName
                    , const TBldStr&            strSourceDir
                    , const TBldStr&            strTargetName
                    , const TBldStr&            strTargetDir
                    , const tCIDLib::TBoolean bFailIfExists)
{
    TBldStr strSrc(strSourceDir);
    if (strSrc.chLast() != L'\\')
        strSrc.Append(L'\\');
    strSrc.Append(strSourceName);

    TBldStr strTarget(strTargetDir);
    if (strTarget.chLast() != L'\\')
        strTarget.Append(L'\\');
    strTarget.Append(strTargetName);

    return bCopyFile(strSrc, strTarget, bFailIfExists);
}

tCIDLib::TBoolean TUtils::bFindNVParts(const  TBldStr&    strSource
                                        ,       TBldStr&    strName
                                        ,       TBldStr&    strValue)
{
    // Assume the worst
    strName.Clear();
    strValue.Clear();

    // Temp buffer to copy into util we get a whole name or value
    const tCIDLib::TCard4 ulMaxChars = 2048;
    tCIDLib::TCh szTmp[ulMaxChars+1];

    //
    //  The name goes from the first non-whitespace up to (but not including)
    //  the next whitespace or = sign.
    //
    const tCIDLib::TCh* pszCur = strSource.pszBuffer();

    // Find first non-whitespace
    while (TRawStr::bIsSpace(*pszCur))
        pszCur++;

    // End of string is not valid at this point
    if (!*pszCur)
        return kCIDLib::False;

    tCIDLib::TCh  chCur = 0;
    tCIDLib::TCard4  ulInd = 0;

    // Copy out the name characters
    while (kCIDLib::True)
    {
        const tCIDLib::TCh chCur = *pszCur;

        // We have to see the = sign, even if there is no value
        if (!chCur)
            return kCIDLib::False;

        // The end of the name, so cap it and break out
        if ((chCur == L'=') || TRawStr::bIsSpace(chCur))
        {
            szTmp[ulInd] = 0;
            break;
        }

        szTmp[ulInd++] = chCur;
        pszCur++;

        // Don't overflow the buffer
        if (ulInd >= ulMaxChars)
            return kCIDLib::False;
    }

    // Copy over the name part
    strName = szTmp;

    //
    //  If we did not break out because of the =, then we have to find
    //  that next.
    //
    if (chCur != L'=')
    {
        while (*pszCur && (*pszCur != L'='))
            pszCur++;

        // Have to find an equal sign
        if (!*pszCur)
            return kCIDLib::False;
    }

    // And move past the equal sign
    pszCur++;

    // Find the next non-whitespace
    while (TRawStr::bIsSpace(*pszCur))
        pszCur++;

    //
    //  End of string is valid at this point. Just means that this setting
    //  has an empty value.
    //
    if (!*pszCur)
        return kCIDLib::True;

    //
    //  And now we can copy out the rest of the string as the value. We do
    //  get rid of the trailing whitespace, but we can't do that until we've
    //  copied out the whole line.
    //
    ulInd = 0;
    while (kCIDLib::True)
    {
        tCIDLib::TCh chCur = *pszCur;

        // Break out when we hit the end
        if (!chCur)
        {
            szTmp[ulInd] = 0;
            break;
        }

        szTmp[ulInd++] = chCur;
        pszCur++;

        // Buffer overflow is an error here
        if (ulInd >= ulMaxChars)
            return kCIDLib::False;
    }

    //
    //  Start working backwards to trash any trailing whitespace. We just
    //  can put a new null in to get rid of it.
    //
    if (ulInd)
    {
        while (TRawStr::bIsSpace(szTmp[ulInd-1]))
        {
            ulInd--;
            if (!ulInd)
                break;
        }
        szTmp[ulInd] = 0;
    }

    // And copy over what we got for the value
    strValue = szTmp;

    return kCIDLib::True;
}


tCIDLib::TBoolean TUtils::bIsHppFile(const TBldStr& strFileName)
{
    // Find the last extension separator
    const tCIDLib::TCh* pszPeriod = TRawStr::pszFindLastChar
    (
        strFileName.pszBuffer()
        , kCIDBuild::chExtSep
    );

    // If none, then just say its not a private header
    if (!pszPeriod)
        return kCIDLib::False;

    // See if the string at that point is the extension we want
    if (!TRawStr::iCompIStr(pszPeriod, kCIDBuild::pszHppExt))
        return kCIDLib::True;
    return kCIDLib::False;
}


tCIDLib::TBoolean TUtils::bIsPrivateHeader(const TBldStr& strToTest)
{
    // Find the last period
    const tCIDLib::TCh* pszPeriod = TRawStr::pszFindLastChar
    (
        strToTest.pszBuffer()
        , kCIDBuild::chExtSep
    );

    // If none, then just say its not a private header
    if (!pszPeriod)
        return kCIDLib::False;

    // If its at the 0th index, it cannot be
    if (pszPeriod == strToTest.pszBuffer())
        return kCIDLib::False;

    // If the char before the period is an underline it is private
    return (*(pszPeriod-1) == L'_');
}


tCIDLib::TBoolean
TUtils::bReplicateTree( const   TBldStr&    strSrcDir
                        , const TBldStr&    strTargetDir)
{
    //
    //  Get a list of files and directories at this level, and iterate them
    //  recursing where required.
    //
    TList<TFindInfo> listFiles;
    TBldStr strTmp = strSrcDir;
    strTmp.Append(L"*");

    // If we find no files at this level, then return
    if (!TFindInfo::c4FindFiles(strTmp, listFiles))
        return kCIDLib::True;

    // If the target directory does not exist, then create it
    if (!TUtils::bExists(strTargetDir))
    {
        if (!TUtils::bMakeDir(strTargetDir))
            return kCIDLib::False;
    }

    // Get a cursor for the list of files and iterate them
    TListCursor<TFindInfo> cursFiles(&listFiles);
    TBldStr strDest;
    if (cursFiles.bResetIter())
    {
        do
        {
            // Get the current entry
            const TFindInfo& fndiCur = cursFiles.tCurElement();

            //
            //  If its a directory, then recurse. If its a file, then copy
            //  it to the target.
            //
            if (fndiCur.bIsDirectory())
            {
                //
                //  Don't do the special directory entries, and skip all of
                //  the CVS directories, since they are never copied to any
                //  release output.
                //
                if (!fndiCur.bIsSpecialDirectory()
                &&  (fndiCur.strFileName() != L"CVS"))
                {
                    strTmp = strSrcDir;
                    strTmp.Append(fndiCur.strFileName());
                    strTmp.Append(L'\\');
                    strDest = strTargetDir;
                    strDest.Append(fndiCur.strFileName());
                    strDest.Append(L'\\');

                    if (!bReplicateTree(strTmp, strDest))
                        return kCIDLib::False;
                }
            }
             else
            {
                if (!TUtils::bCopyFile( fndiCur.strFileName()
                                        , strSrcDir
                                        , fndiCur.strFileName()
                                        , strTargetDir))
                {
                    stdOut  << L"Could not copy file: " << fndiCur.strFileName()
                            << kCIDBuild::EndLn;
                    throw tCIDBuild::EErrors::CopyFailed;
                }
            }
        }   while (cursFiles.bNext());
    }
    return kCIDLib::True;
}


//
//  Given a new output file and and old one, we check to see if the new one
//  is different from the old one (or the old one doesn't exist.) If so,
//  we get rid of the old one and copy the new one to its place.
//
//  This is mostly used on header files, which cause lots of rebuilds if
//  they change. So we output to temps first and only copy over once they
//  done and if they have changed.
//
tCIDLib::TBoolean
TUtils::bUpdateOutputFile(  const   TBldStr&    strNewFile
                            , const TBldStr&    strOldFile)
{
    //
    //  If the old file doesn't exist, then nothing to think about. Copy the
    //  new file to the old, and delete the new one.
    //
    TFindInfo fndiOld;
    if (!TFindInfo::bFindAFile(strOldFile, fndiOld))
    {
        bCopyFile(strNewFile, strOldFile);
        bDeleteFile(strNewFile);
        return kCIDLib::True;
    }

    //
    //  It exists, so read them both in and see if they've changed.
    //  If so, we replace the old one.
    //
    TFindInfo fndiNew;
    if (!TFindInfo::bFindAFile(strNewFile, fndiNew))
    {
        // This is bad
        stdOut << L"The source file to UpdateOutFile did not exist"
               << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }

    //
    //  If they are different sizes, no need to look further, else we
    //  have to compare them.
    //
    tCIDLib::TBoolean bReplace = fndiNew.c4Size() != fndiOld.c4Size();
    if (!bReplace)
    {
        TBinFile bflSrc(strNewFile);
        TBinFile bflTar(strOldFile);

        const tCIDLib::TCard4 c4Count = fndiNew.c4Size();
        tCIDLib::TCard1* pc1New = new tCIDLib::TCard1[c4Count];
        tCIDLib::TCard1* pc1Old = new tCIDLib::TCard1[c4Count];

        bflSrc.c4Read(pc1New, c4Count);
        bflTar.c4Read(pc1Old, c4Count);

        bReplace = TUtils::bCmpBufs(pc1New, pc1Old, c4Count);
    }

    // IF we need to replace it, copy new to old. Either way delete new
    if (bReplace)
        bCopyFile(strNewFile, strOldFile);
    bDeleteFile(strNewFile);

    return bReplace;
}


// Translates a boolean text representation to the binary representation
tCIDLib::TBoolean
TUtils::bXlatBool(  const   TBldStr&            strValue
                    ,       tCIDLib::TBoolean&  bToFill)
{
    if (strValue.bIEquals(L"Yes"))
        bToFill = kCIDLib::True;
    else if (strValue.bIEquals(L"True"))
        bToFill = kCIDLib::True;
    else if (strValue.bIEquals(L"No"))
        bToFill = kCIDLib::False;
    else if (strValue.bIEquals(L"False"))
        bToFill = kCIDLib::False;
    else
        return kCIDLib::False;
    return kCIDLib::True;
}


tCIDLib::TVoid TUtils::CompletePath(const   TBldStr&    strWCSpec
                                    , const TBldStr&    strFileFound
                                    ,       TBldStr&    strFullName)
{
    tCIDLib::TCard4 c4NameOfs;
    tCIDLib::TCard4 c4ExtOfs;

    //
    //  Complete the wild card spec to get a full path. Then cut off the
    //  name. Then copy the name of the actual file found back onto the end.
    //
    TUtils::CompletePath(strWCSpec, strFullName);
    TUtils::FindPathParts(strFullName, c4NameOfs, c4ExtOfs);
    strFullName.CapAt(c4NameOfs);
    strFullName.Append(strFileFound);
}


tCIDLib::TVoid
TUtils::FindPathParts(  const   TBldStr&            strSource
                        ,       tCIDLib::TCard4&    c4Name
                        ,       tCIDLib::TCard4&    c4Extension)
{
    // Get a pointer to the raw buffer
    const tCIDLib::TCh* pszRaw = strSource.pszBuffer();

    // Assume the worst
    c4Name = kCIDBuild::c4NotFound;
    c4Extension = kCIDBuild::c4NotFound;

    // The source is empty, then obviously nothing to do
    if (!TRawStr::c4StrLen(pszRaw))
        return;

    //
    //  Find the last extension separator in the path name. This should be
    //  where the extension starts.
    //
    const tCIDLib::TCh* pszExtension = TRawStr::pszFindLastChar
    (
        pszRaw
        , kCIDBuild::chExtSep
    );

    //
    //  And find the last file component separator. If it is past the period
    //  we found above, then the period cannot be the extension so null it
    //  back out.
    //
    const tCIDLib::TCh* pszName = TRawStr::pszFindLastChar(pszRaw, L'\\');
    if (pszName > pszExtension)
        pszExtension = 0;

    // Move up past the separator
    if (pszName)
        pszName++;
    else
        pszName = pszRaw;

    // Now calculate the two offsets
    c4Name = pszName - pszRaw;

    if (pszExtension)
        c4Extension = pszExtension - pszRaw;
}

