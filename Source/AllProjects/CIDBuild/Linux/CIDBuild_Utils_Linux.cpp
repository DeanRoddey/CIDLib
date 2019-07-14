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
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
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
TUtils::bExec(  const   tCIDLib::TCh* const*    apszParams
                , const tCIDLib::TCard4         c4Count
                ,       tCIDLib::TCard4&        c4Result
                , const tCIDLib::TCard4         c4Flags)
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

    return kCIDLib::True;
}


//
//  Checks to see if the passed string represents a fully qualified Windows
//  path.
//
tCIDLib::TBoolean TUtils::bIsFQPath(const TBldStr& strFileName)
{
    return kCIDLib::False;
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
    return kCIDLib::False;
}


tCIDLib::TBoolean
TUtils::bRunCmdLine(const   TBldStr&            strToRun
                    ,       tCIDLib::TCard4&    c4Result
                    , const tCIDLib::TBoolean   bLowPrio)
{
    return kCIDLib::False;
}


tCIDLib::TVoid TUtils::Beep(const   tCIDLib::TCard4 c4Freq
                            , const tCIDLib::TCard4 c4Duration)
{
    // A no op for now
}


// Return the number of CPUs (effective) on this machine
tCIDLib::TCard4 TUtils::c4CPUCount()
{
    return 1;
}


tCIDLib::TVoid
TUtils::CompletePath(const TBldStr& strOrgName, TBldStr& strFullName)
{

}


tCIDLib::TVoid TUtils::MakeTmpFileName(TBldStr& strToFill)
{
}


tCIDLib::TVoid TUtils::QueryCurDir(TBldStr& strToFill)
{
}


tCIDLib::TVoid
TUtils::SetEnvVar(const TBldStr& strVar, const TBldStr& strValue)
{
}
