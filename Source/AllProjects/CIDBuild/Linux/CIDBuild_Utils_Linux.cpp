//
// FILE NAME: CIDBuild_Utils_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 02/24/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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

#include <ctype.h>

// ---------------------------------------------------------------------------
//  TUtils: Implementations of platform specific methods
// ---------------------------------------------------------------------------
tCIDBuild::TBoolean TUtils::bExists(const TBldStr& strFile)
{
    tCIDBuild::TNatCh* pszFile = TRawStr::pszTranscode(strFile.pszBuffer());
    TArrayJanitor<tCIDBuild::TNatCh> janFile(pszFile);

    return (::access(pszFile, F_OK) == 0);
}

tCIDBuild::TBoolean TUtils::bChangeDir(const TBldStr& strChangeTo)
{
    tCIDBuild::TNatCh* pszDir = TRawStr::pszTranscode(strChangeTo.pszBuffer());
    TArrayJanitor<tCIDBuild::TNatCh> janDir(pszDir);

    return (::chdir(pszDir) == 0);
}

tCIDBuild::TBoolean
TUtils::bCopyFile(  const   TBldStr&            strSourceName
                    , const TBldStr&            strTargetName
                    , const tCIDBuild::TBoolean bFailIfExists)
{
    tCIDBuild::TNatCh* pszSrc = TRawStr::pszTranscode(strSourceName.pszBuffer());
    tCIDBuild::TNatCh* pszTar = TRawStr::pszTranscode(strTargetName.pszBuffer());
    TArrayJanitor<tCIDBuild::TNatCh> janSrc(pszSrc);
    TArrayJanitor<tCIDBuild::TNatCh> janTar(pszTar);

    if (!::access(pszTar, F_OK) && bFailIfExists)
        return false;

    struct stat StatBuf;
    if (::stat(pszSrc, &StatBuf))
    {
        stdOut << NStr("While copying '") << pszSrc << NStr("' to '")
               << pszTar << NStr("': ") << ::strerror(errno) << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }

    if (!S_ISREG(StatBuf.st_mode))
    {
        stdOut << NStr("'") << pszSrc << NStr("' is not a regular file") << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }

    tCIDBuild::TInt iSrcFd = ::open(pszSrc, O_RDONLY);
    if (iSrcFd == -1)
    {
        stdOut << NStr("While copying '") << pszSrc << NStr("' to '")
               << pszTar << NStr("': ") << ::strerror(errno) << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }

    tCIDBuild::TInt iTgtFd = ::open(pszTar, O_WRONLY | O_CREAT | O_TRUNC);
    if (iTgtFd == -1)
    {
        ::close(iSrcFd);
        stdOut << NStr("While copying '") << pszSrc << NStr("' to '")
               << pszTar << NStr("': ") << ::strerror(errno) << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }

    tCIDBuild::TBoolean bReturn = true;
    unsigned char* pBuf = new unsigned char[StatBuf.st_blksize];
    TArrayJanitor<unsigned char> janBuf(pBuf);
    tCIDBuild::TInt iRead, iWritten;
    while (true)
    {
        iRead = ::read(iSrcFd, pBuf, StatBuf.st_blksize);
        if (iRead == -1)
        {
            stdOut << NStr("Error copying '") << pszSrc << NStr("' to '")
                   << pszTar << NStr("'") << kCIDBuild::EndLn;
            bReturn = false;
            break;
        }

        if (!iRead)
            break;

        iWritten = ::write(iTgtFd, pBuf, iRead);
        if (iWritten == -1)
        {
            stdOut << NStr("Error copying '") << pszSrc << NStr("' to '")
                   << pszTar << NStr("'") << kCIDBuild::EndLn;
            bReturn = false;
            break;
        }
    }

    ::close(iSrcFd);

    if (::fchmod(iTgtFd, StatBuf.st_mode))
    {
        stdOut << NStr("Error setting permissions on '") << pszTar << NStr("'") << kCIDBuild::EndLn;
        bReturn = false;
    }
    ::close(iTgtFd);

    return bReturn;
}


tCIDBuild::TBoolean TUtils::bDeleteFile(const TBldStr& strToDelete)
{
    tCIDBuild::TNatCh* pszDel = TRawStr::pszTranscode(strToDelete.pszBuffer());
    TArrayJanitor<tCIDBuild::TNatCh> janDel(pszDel);

    return (::unlink(pszDel) == 0);
}


tCIDBuild::TBoolean TUtils::bMakeDir(const TBldStr& strToMake)
{
    tCIDBuild::TNatCh* pszDir = TRawStr::pszTranscode(strToMake.pszBuffer());
    TArrayJanitor<tCIDBuild::TNatCh> janDir(pszDir);

    return (::mkdir(pszDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0);
}


tCIDBuild::TBoolean
TUtils::bExec(  const   tCIDBuild::TCh* const *     apszParams
                , const tCIDBuild::TUInt            uiCount
                ,       tCIDBuild::TUInt&           uiResult)
{
    tCIDBuild::TUInt uiTmp;
    tCIDBuild::TNatCh** apszArgs = new tCIDBuild::TNatCh*[uiCount + 2];
    apszArgs[0] = TRawStr::pszTranscode(apszParams[0]);
    for (uiTmp = 0; uiTmp < uiCount; uiTmp++)
        apszArgs[uiTmp + 1] = TRawStr::pszTranscode(apszParams[uiTmp]);
    apszArgs[uiCount + 1] = 0;

    pid_t pid = ::fork();
    if (pid == -1)
    {
        uiTmp = 0;
        while (apszArgs[uiTmp])
            delete [] apszArgs[uiTmp++];
        delete [] apszArgs;
        stdOut << NStr("Unable to fork process") << kCIDBuild::EndLn;
        return false;
    }

    if (pid == 0)
    {
       ::execvp(apszArgs[0], &apszArgs[1]);
    }

    int status;
    ::waitpid(pid, &status, WUNTRACED);

    if (WIFEXITED(status))
        uiResult = WEXITSTATUS(status);

    tCIDBuild::TBoolean bReturn = true;

    if (!WIFEXITED(status))
    {
        stdOut << NStr("Failed to execute: ") << apszArgs[0] << kCIDBuild::EndLn;
        bReturn = false;
    }

    uiTmp = 0;
    while (apszArgs[uiTmp])
        delete [] apszArgs[uiTmp++];
    delete [] apszArgs;

    return bReturn;
}


tCIDBuild::TVoid TUtils::Beep(  const   tCIDBuild::TUInt
                                , const tCIDBuild::TUInt)
{
    // A no op
}

tCIDBuild::TVoid TUtils::LocalizePath(TBldStr& strPath)
{
    // Another no op
}
