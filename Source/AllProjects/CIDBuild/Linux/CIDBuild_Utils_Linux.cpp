//
// FILE NAME: CIDBuild_Utils_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 02/24/1999
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
//  This file provides the Linux specific parts of the TUtils namespace.
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
#include    "../CIDBuild.hpp"
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <ctype.h>


// ---------------------------------------------------------------------------
//  TUtils: Implementations of platform specific methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TUtils::bExists(const TBldStr& strFile)
{
    tCIDLib::TSCh* pszFile = TRawStr::pszTranscode(strFile.pszBuffer());
    TArrayJanitor<tCIDLib::TSCh> janFile(pszFile);

    return (::access(pszFile, F_OK) == 0);
}

tCIDLib::TBoolean TUtils::bChangeDir(const TBldStr& strChangeTo)
{
    tCIDLib::TSCh* pszDir = TRawStr::pszTranscode(strChangeTo.pszBuffer());
    TArrayJanitor<tCIDLib::TSCh> janDir(pszDir);

    return (::chdir(pszDir) == 0);
}

tCIDLib::TBoolean
TUtils::bCmpBufs(const  tCIDLib::TCard1* const  pc1Buf1
                , const tCIDLib::TCard1* const  pc1Buf2
                , const tCIDLib::TCard4         c4Count)
{
    return (memcmp(pc1Buf1, pc1Buf2, c4Count) != 0);
}


tCIDLib::TBoolean
TUtils::bCopyFile(  const   TBldStr&            strSourceName
                    , const TBldStr&            strTargetName
                    , const tCIDLib::TBoolean bFailIfExists)
{
    tCIDLib::TSCh* pszSrc = TRawStr::pszTranscode(strSourceName.pszBuffer());
    tCIDLib::TSCh* pszTar = TRawStr::pszTranscode(strTargetName.pszBuffer());
    TArrayJanitor<tCIDLib::TSCh> janSrc(pszSrc);
    TArrayJanitor<tCIDLib::TSCh> janTar(pszTar);

    if (!::access(pszTar, F_OK) && bFailIfExists)
        return false;

    struct stat StatBuf;
    if (::stat(pszSrc, &StatBuf))
    {
        stdOut << L"While copying '" << pszSrc << L"' to '"
               << pszTar << L"': " << ::strerror(errno) << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }

    if (!S_ISREG(StatBuf.st_mode))
    {
        stdOut << L"'" << pszSrc << L"' is not a regular file" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }

    tCIDLib::TSInt iSrcFd = ::open(pszSrc, O_RDONLY);
    if (iSrcFd == -1)
    {
        stdOut << L"While copying '" << pszSrc << L"' to '"
               << pszTar << L"': " << ::strerror(errno) << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }

    tCIDLib::TSInt iTgtFd = ::open(pszTar, O_WRONLY | O_CREAT | O_TRUNC);
    if (iTgtFd == -1)
    {
        ::close(iSrcFd);
        stdOut << L"While copying '" << pszSrc << L"' to '"
               << pszTar << L"': " << ::strerror(errno) << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }

    tCIDLib::TBoolean bReturn = true;
    unsigned char* pBuf = new unsigned char[StatBuf.st_blksize];
    TArrayJanitor<unsigned char> janBuf(pBuf);
    tCIDLib::TSInt iRead, iWritten;
    while (true)
    {
        iRead = ::read(iSrcFd, pBuf, StatBuf.st_blksize);
        if (iRead == -1)
        {
            stdOut << L"Error copying '" << pszSrc << L"' to '"
                   << pszTar << L'\'' << kCIDBuild::EndLn;
            bReturn = false;
            break;
        }

        if (!iRead)
            break;

        iWritten = ::write(iTgtFd, pBuf, iRead);
        if (iWritten == -1)
        {
            stdOut << L"Error copying '" << pszSrc << L"' to '"
                   << pszTar << L"'" << kCIDBuild::EndLn;
            bReturn = false;
            break;
        }
    }

    ::close(iSrcFd);

    if (::fchmod(iTgtFd, StatBuf.st_mode))
    {
        stdOut  << L"Error setting permissions on '"
                << pszTar << L"'" << kCIDBuild::EndLn;
        bReturn = false;
    }
    ::close(iTgtFd);

    return bReturn;
}


tCIDLib::TBoolean TUtils::bDeleteFile(const TBldStr& strToDelete)
{
    tCIDLib::TSCh* pszDel = TRawStr::pszTranscode(strToDelete.pszBuffer());
    TArrayJanitor<tCIDLib::TSCh> janDel(pszDel);

    return (::unlink(pszDel) == 0);
}



tCIDLib::TBoolean
TUtils::bExec(  const   tCIDLib::TCh**      apszParams
                , const tCIDLib::TCard4     c4Count
                ,       tCIDLib::TCard4&    c4Result
                , const tCIDLib::TCard4     c4Flags)
{
    tCIDLib::TUInt uiTmp;
    tCIDLib::TSCh** apszArgs = new tCIDLib::TSCh*[c4Count + 2];
    apszArgs[0] = TRawStr::pszTranscode(apszParams[0]);
    for (uiTmp = 0; uiTmp < c4Count; uiTmp++)
        apszArgs[uiTmp + 1] = TRawStr::pszTranscode(apszParams[uiTmp]);
    apszArgs[c4Count + 1] = 0;

    pid_t pid = ::fork();
    if (pid == -1)
    {
        uiTmp = 0;
        while (apszArgs[uiTmp])
            delete [] apszArgs[uiTmp++];
        delete [] apszArgs;
        stdOut << L"Unable to fork process" << kCIDBuild::EndLn;
        return false;
    }

    if (pid == 0)
    {
       ::execvp(apszArgs[0], &apszArgs[1]);
    }

    int status;
    ::waitpid(pid, &status, WUNTRACED);

    if (WIFEXITED(status))
        c4Result = WEXITSTATUS(status);

    tCIDLib::TBoolean bReturn = true;

    if (!WIFEXITED(status))
    {
        stdOut << L"Failed to execute: " << apszArgs[0] << kCIDBuild::EndLn;
        bReturn = false;
    }

    uiTmp = 0;
    while (apszArgs[uiTmp])
        delete [] apszArgs[uiTmp++];
    delete [] apszArgs;

    return bReturn;
}


tCIDLib::TBoolean TUtils::bGetEnvVar(const  TBldStr&    strVarName
                                    ,       TBldStr&    strToFill)
{
    tCIDLib::TSCh* pszVar = TRawStr::pszTranscode(strVarName.pszBuffer());
    TArrayJanitor<tCIDLib::TSCh> janDir(pszVar);

    const char* pszVal = ::getenv(pszVar);
    if (!pszVal)
        return kCIDLib::False;

    strToFill = pszVal;
    return kCIDLib::True;
}


//
//  Checks to see if the passed string represents a fully qualified Windows
//  path.
//
tCIDLib::TBoolean TUtils::bIsFQPath(const TBldStr& strFileName)
{
    return (!strFileName.bEmpty() && strFileName[0] == '/');
}


tCIDLib::TBoolean TUtils::bMakeDir(const TBldStr& strToMake)
{
    tCIDLib::TSCh* pszDir = TRawStr::pszTranscode(strToMake.pszBuffer());
    TArrayJanitor<tCIDLib::TSCh> janDir(pszDir);

    return (::mkdir(pszDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0);
}


tCIDLib::TBoolean
TUtils::bMakePath(const TBldStr& strParent, const TBldStr& strToMake)
{
    TFindInfo   fndiToFill;

    // First make sure the parent exists
    TBldStr strName(strParent);
    if (strName.chLast() == L'/')
        strName.DeleteLast();
    if (!TFindInfo::bFindAFile(strName, fndiToFill))
    {
        stdOut << L"Parent diretory does not exist: " << strParent << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileNotFound;
    }

    //
    //  Ok, we need to start pulling out the components of the part of the
    //  path we are to create and work our way down it. If the string to
    //  make is empty, we are done, so return false to say we created nothing.
    //
    if (strToMake.bEmpty())
        return kCIDLib::False;

    // Get a copy of it so we can modify it a bit if needed
    TBldStr strTarPath(strToMake);
    strTarPath.StripWhitespace();

    // If it has a trailing slash, remove it
    if (strTarPath.chLast() == L'/')
        strTarPath.DeleteLast();

    // Get a raw buffer copy of the string, so we can tokenize it
    tCIDLib::TCh* pszSrc = strTarPath.pszDupBuffer();
    TArrayJanitor<tCIDLib::TCh> janSrc(pszSrc);

    // Start with the parent path and we'll build up layers as we go
    tCIDLib::TCh* pszTok = TRawStr::pszStrTok(pszSrc, L"/");
    while (pszTok)
    {
        strName.Append(L"/");
        strName.Append(pszTok);
        strName.StripWhitespace();
        if (!TFindInfo::bIsDirectory(strName))
        {
            if (!bMakeDir(strName))
            {
                stdOut << L"Could not create directory: " << strName << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::CreateError;
            }
        }
        pszTok = TRawStr::pszStrTok(0, L"/");
    }

    // Indicate we create some path components
    return kCIDLib::True;
}


tCIDLib::TBoolean
TUtils::bRunCmdLine(const   TBldStr&            strToRun
                    ,       tCIDLib::TCard4&    c4Result
                    , const tCIDLib::TBoolean   bLowPrio)
{
    tCIDLib::TSCh* pszCmd = TRawStr::pszTranscode(strToRun.pszBuffer());
    TArrayJanitor<tCIDLib::TSCh> janOrg(pszCmd);

    int iRes = ::system(pszCmd);
    c4Result = tCIDLib::TCard4(iRes);

    return kCIDLib::True;
}


tCIDLib::TVoid TUtils::Beep(const   tCIDLib::TCard4 c4Freq
                            , const tCIDLib::TCard4 c4Duration)
{
    // A no op for now
}


// Return the number of CPUs (effective) on this machine
tCIDLib::TCard4 TUtils::c4CPUCount()
{
    return get_nprocs();
}


tCIDLib::TVoid
TUtils::CompletePath(const TBldStr& strOrgName, TBldStr& strFullName)
{
    tCIDLib::TSCh* pszOrgName = TRawStr::pszTranscode(strOrgName.pszBuffer());
    TArrayJanitor<tCIDLib::TSCh> janOrg(pszOrgName);

    tCIDLib::TSCh pszResolved[PATH_MAX];
    if (!realpath(pszOrgName, pszResolved))
    {
        stdOut << "Could not complete path: " << strOrgName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileNotFound;    
    }

    // Convert it and store it
    tCIDLib::TCh* pszFullPath = TRawStr::pszTranscode(pszResolved);
    strFullName = pszFullPath;
    delete [] pszFullPath;
}


tCIDLib::TVoid TUtils::QueryCurDir(TBldStr& strToFill)
{
    char pszCurDir[PATH_MAX];
    if (::getcwd(pszCurDir, sizeof(pszCurDir)))
        strToFill = pszCurDir;
}


tCIDLib::TVoid
TUtils::SetEnvVar(const TBldStr& strVar, const TBldStr& strValue)
{
    tCIDLib::TSCh* pszVar = TRawStr::pszTranscode(strVar.pszBuffer());
    TArrayJanitor<tCIDLib::TSCh> janVar(pszVar);
    tCIDLib::TSCh* pszVal = TRawStr::pszTranscode(strValue.pszBuffer());
    TArrayJanitor<tCIDLib::TSCh> janVal(pszVal);    

    ::setenv(pszVar, pszVal, 1);
}
