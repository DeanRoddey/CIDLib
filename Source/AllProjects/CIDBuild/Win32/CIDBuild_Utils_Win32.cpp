//
// FILE NAME: CIDBuild_Util.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/29/1998
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
//  This file provides the Win32 specific parts of the TUtils namespace.
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

#define     WIN32_LEAN_AND_MEAN
#include    <windows.h>


// ---------------------------------------------------------------------------
//  Local types
// ---------------------------------------------------------------------------
struct TSaveInfo
{
    BOOL        bInCon;
    HANDLE      hIn;
    DWORD       dwInFlags;

    BOOL        bOutCon;
    HANDLE      hOut;
    DWORD       dwOutFlags;
};



// ---------------------------------------------------------------------------
//  TUtils: Implementations of platform specific methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TUtils::bExists(const TBldStr& strFile)
{
    tCIDLib::TCh* pszFile = TRawStr::pszDupString(strFile.pszBuffer());
    TArrayJanitor<tCIDLib::TCh> janFile(pszFile);

    const tCIDLib::TCard4 c4Len = TRawStr::c4StrLen(pszFile);
    if (c4Len)
    {
        if (pszFile[c4Len-1] == L'\\')
            pszFile[c4Len-1] = 0;
    }

    WIN32_FIND_DATA FindData;
    HANDLE hTmp = ::FindFirstFile(pszFile, &FindData);
    const tCIDLib::TBoolean bRes = (hTmp != INVALID_HANDLE_VALUE);

    if (bRes)
        ::FindClose(hTmp);

    return bRes;
}

tCIDLib::TBoolean TUtils::bChangeDir(const TBldStr& strChangeTo)
{
    if (!::SetCurrentDirectory(strChangeTo.pszBuffer()))
        return kCIDLib::False;
    return kCIDLib::True;
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
                    , const tCIDLib::TBoolean   bFailIfExists)
{
    const tCIDLib::TCh* pszSrc = strSourceName.pszBuffer();
    const tCIDLib::TCh* pszTar = strTargetName.pszBuffer();

    BOOL bCancel = kCIDLib::False;
    if (!::CopyFile(pszSrc, pszTar, bFailIfExists))
    {
        if (::GetLastError() == ERROR_FILE_EXISTS)
            return kCIDLib::False;

        stdOut << L"Failed to copy from '" << strSourceName
               << L"' to '" << strTargetName << L"'" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::CopyFailed;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TUtils::bDeleteFile(const TBldStr& strToDelete)
{
    if (!::DeleteFile(strToDelete.pszBuffer()))
        return kCIDLib::False;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TUtils::bExec(  const   tCIDLib::TCh* const*    apszParams
                , const tCIDLib::TCard4         c4Count
                ,       tCIDLib::TCard4&        c4Result
                , const tCIDLib::TBoolean       bLowPrio)
{
    tCIDLib::TBoolean bRet = kCIDLib::False;

    // Build up the command line parms
    static tCIDBuild::TCh szCmdLine[32 * 1024];
    szCmdLine[0] = 0;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        const tCIDLib::TCh* pszCur = apszParams[c4Index];

        // See if it has any spaces
        tCIDLib::TBoolean bHasSpaces = (TRawStr::pszFindChar(pszCur, L' ') != 0);

        // If it doesn't start with a quote and has spaces, then add one
        if (bHasSpaces && (pszCur[0] != L'"'))
            TRawStr::CatStr(szCmdLine, L"\"");

        // Add the actual parameter
        TRawStr::CatStr(szCmdLine, pszCur);

        //
        //  If it doesn't end with a quote, and has spaces, then added one.
        //  If we have to add one, and it ends with a slash, add another one
        //  so that the compiler doesn't interpret it as an escaped slash.
        //
        const tCIDLib::TCh chLastChar(pszCur[TRawStr::c4StrLen(pszCur) - 1]);
        if (bHasSpaces && (chLastChar != L'"'))
        {
            if (chLastChar == L'\\')
                TRawStr::CatStr(szCmdLine, L"\\");
            TRawStr::CatStr(szCmdLine, L"\"");
        }

        // ANd a space before the next one
        TRawStr::CatStr(szCmdLine, L" ");
    }

// stdOut << szCmdLine << kCIDBuild::EndLn;

    tCIDLib::TCard4 c4CreateFlags = CREATE_DEFAULT_ERROR_MODE;
    if (bLowPrio)
        c4CreateFlags |= IDLE_PRIORITY_CLASS;

    STARTUPINFO Startup = {0};
    Startup.cb = sizeof(STARTUPINFO);
    Startup.dwFlags = STARTF_USESTDHANDLES;
    Startup.hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
    Startup.hStdOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
    Startup.hStdError = ::GetStdHandle(STD_ERROR_HANDLE);

    //
    //  To prevent the child process from freaking out our console handle
    //  mode settings, save them and restore them.
    //
    TSaveInfo SaveInfo;
    SaveInfo.hIn = ::GetStdHandle(STD_INPUT_HANDLE);
    SaveInfo.bInCon = ::GetConsoleMode(SaveInfo.hIn, &SaveInfo.dwInFlags);

    SaveInfo.hOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
    SaveInfo.bOutCon = ::GetConsoleMode(SaveInfo.hOut, &SaveInfo.dwOutFlags);

    PROCESS_INFORMATION ProcInfo;
    BOOL bRes = ::CreateProcess
    (
        0
        , szCmdLine
        , 0
        , 0
        , 1
        , c4CreateFlags
        , 0
        , 0
        , &Startup
        , &ProcInfo
    );

    if (!bRes)
    {
        // Reuse the command line buffer, since its nice and large
        tCIDLib::TCard4 c4Chars = ::FormatMessage
        (
            FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM
            , 0
            , GetLastError()
            , MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
            , szCmdLine
            , 8192
            , 0
        );
        stdOut << L"Failed to execute: " << apszParams[0] << kCIDBuild::EndLn;
        if (c4Chars)
            stdOut << L"    REASON: " << szCmdLine << kCIDBuild::EndLn;
        return kCIDLib::False;
    }

    // Get rid of the thread handle
    ::CloseHandle(ProcInfo.hThread);

    // And now wait for the process to end
    if (::WaitForSingleObject(ProcInfo.hProcess, INFINITE) == WAIT_OBJECT_0)
        bRet = kCIDLib::True;
    else
        bRet = kCIDLib::False;

    // Get the exit code if we succeeded
    if (bRet)
    {
        DWORD dwRes;
        ::GetExitCodeProcess(ProcInfo.hProcess, &dwRes);
        c4Result = dwRes;

        // Restore our console settings
        if (SaveInfo.bInCon)
            ::SetConsoleMode(SaveInfo.hIn, SaveInfo.dwInFlags);
        if (SaveInfo.bOutCon)
            ::SetConsoleMode(SaveInfo.hOut, SaveInfo.dwOutFlags);
    }
    return bRet;
}


tCIDLib::TBoolean TUtils::bGetEnvVar(const  TBldStr&    strVarName
                                    ,       TBldStr&    strToFill)
{
    const tCIDLib::TCard4 c4BufSz = 2047;
    tCIDLib::TCh szBuf[c4BufSz + 1];
    if (!::GetEnvironmentVariableW(strVarName.pszBuffer(), szBuf, c4BufSz))
        return kCIDLib::False;
    strToFill = szBuf;
    return kCIDLib::True;
}


//
//  Checks to see if the passed string represents a fully qualified Windows
//  path.
//
tCIDLib::TBoolean TUtils::bIsFQPath(const TBldStr& strFileName)
{
    // Check for a UNC name
    if (strFileName.bStartsWith(L"\\\\"))
        return kCIDLib::True;

    // Check for a standard FQ path
    if (strFileName.c4Length() < 2)
        return kCIDLib::False;

    const tCIDLib::TCh chFirst = strFileName[0];
    if (((chFirst >= L'A') && (chFirst <= L'Z'))
    ||  ((chFirst >= L'a') && (chFirst <= L'z'))
    &&  (strFileName[1] == L':'))
    {
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean TUtils::bMakeDir(const TBldStr& strToMake)
{
    if (!::CreateDirectory(strToMake.pszBuffer(), 0))
        return kCIDLib::False;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TUtils::bMakePath(const TBldStr& strParent, const TBldStr& strToMake)
{
    TFindInfo   fndiToFill;

    // First make sure the parent exists
    TBldStr strName(strParent);
    if (strName.chLast() == L'\\')
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
    if (strTarPath.chLast() == L'\\')
        strTarPath.DeleteLast();

    // Get a raw buffer copy of the string, so we can tokenize it
    tCIDLib::TCh* pszSrc = strTarPath.pszDupBuffer();
    TArrayJanitor<tCIDLib::TCh> janSrc(pszSrc);

    // Start with the parent path and we'll build up layers as we go
    tCIDLib::TCh* pszTok = TRawStr::pszStrTok(pszSrc, L"\\");
    while (pszTok)
    {
        strName.Append(L"\\");
        strName.Append(pszTok);
        strName.StripWhitespace();
        if (!TFindInfo::bIsDirectory(strName))
        {
            if (!::CreateDirectory(strName.pszBuffer(), 0))
            {
                stdOut << L"Could not create directory: " << strName << kCIDBuild::EndLn;
                throw tCIDBuild::EErrors::CreateError;
            }
        }
        pszTok = TRawStr::pszStrTok(0, L"\\");
    }

    // Indicate we create some path components
    return kCIDLib::True;
}


tCIDLib::TBoolean
TUtils::bRunCmdLine(const   TBldStr&            strToRun
                    ,       tCIDLib::TCard4&    c4Result
                    , const tCIDLib::TBoolean   bLowPrio)
{
    tCIDLib::TBoolean bRet = kCIDLib::False;

    tCIDLib::TCard4 c4CreateFlags = CREATE_DEFAULT_ERROR_MODE;
    if (bLowPrio)
        c4CreateFlags |= IDLE_PRIORITY_CLASS;

    STARTUPINFO Startup = {0};
    Startup.cb = sizeof(STARTUPINFO);
    Startup.dwFlags = STARTF_USESTDHANDLES;
    Startup.hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
    Startup.hStdOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
    Startup.hStdError = ::GetStdHandle(STD_ERROR_HANDLE);

    //
    //  To prevent the child process from freaking out our console handle
    //  mode settings, save them and restore them.
    //
    TSaveInfo SaveInfo;
    SaveInfo.hIn = ::GetStdHandle(STD_INPUT_HANDLE);
    SaveInfo.bInCon = ::GetConsoleMode(SaveInfo.hIn, &SaveInfo.dwInFlags);

    SaveInfo.hOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
    SaveInfo.bOutCon = ::GetConsoleMode(SaveInfo.hOut, &SaveInfo.dwOutFlags);

    PROCESS_INFORMATION ProcInfo;
    BOOL bRes = ::CreateProcess
    (
        0
        , (LPWSTR)strToRun.pszBuffer()
        , 0
        , 0
        , 1
        , c4CreateFlags
        , 0
        , 0
        , &Startup
        , &ProcInfo
    );

    if (!bRes)
    {
        // Reuse the command line buffer, since its nice and large
        tCIDLib::TCh szMsgBuf[4192];
        tCIDLib::TCard4 c4Chars = ::FormatMessage
        (
            FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM
            , 0
            , GetLastError()
            , MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
            , szMsgBuf
            , 4191
            , 0
        );
        stdOut << L"Failed to execute: " << strToRun.pszBuffer() << kCIDBuild::EndLn;
        if (c4Chars)
            stdOut << L"    REASON: " << szMsgBuf << kCIDBuild::EndLn;
        return kCIDLib::False;
    }

    // Get rid of the thread handle
    ::CloseHandle(ProcInfo.hThread);

    // And now wait for the process to end
    if (::WaitForSingleObject(ProcInfo.hProcess, INFINITE) == WAIT_OBJECT_0)
        bRet = kCIDLib::True;
    else
        bRet = kCIDLib::False;

    // Get the exit code if we succeeded
    if (bRet)
    {
        DWORD dwRes;
        ::GetExitCodeProcess(ProcInfo.hProcess, &dwRes);
        c4Result = dwRes;

        // Restore our console settings
        if (SaveInfo.bInCon)
            ::SetConsoleMode(SaveInfo.hIn, SaveInfo.dwInFlags);
        if (SaveInfo.bOutCon)
            ::SetConsoleMode(SaveInfo.hOut, SaveInfo.dwOutFlags);
    }
    return bRet;
}


tCIDLib::TVoid TUtils::Beep(const   tCIDLib::TCard4 c4Freq
                            , const tCIDLib::TCard4 c4Duration)
{
    ::Beep(c4Freq, c4Duration);
}


// Return the number of CPUs (effective) on this machine
tCIDLib::TCard4 TUtils::c4CPUCount()
{
    SYSTEM_INFO SystemInfo = {0};
    ::GetSystemInfo(&SystemInfo);
    return SystemInfo.dwNumberOfProcessors;
}


tCIDLib::TVoid
TUtils::CompletePath(const TBldStr& strOrgName, TBldStr& strFullName)
{
    // Get the full path into a temp buffer
    const tCIDLib::TCard4 c4BufSz = 2048;
    tCIDLib::TCh szTmp[c4BufSz + 1];
    tCIDLib::TCh* pszDummy;
    tCIDLib::TCard4 c4Len = ::GetFullPathName
    (
        strOrgName.pszBuffer()
        , c4BufSz
        , szTmp
        , &pszDummy
    );

    if (!c4Len)
    {
        stdOut << L"Could not complete path: " << strOrgName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::FileNotFound;
    }

    // Copy it back to the caller's buffer
    szTmp[c4Len] = 0;
    strFullName = szTmp;
}


tCIDLib::TVoid TUtils::MakeTmpFileName(TBldStr& strToFill)
{
    // Get the temp path first
    tCIDLib::TCh szPath[kCIDLib::c4MaxPathLen - 14];
    if (!::GetTempPath(kCIDLib::c4MaxPathLen - 14, szPath))
    {
        stdOut << L"Could not get temp path" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::MakeTmpFile;
    }

    // And get a temp file in that temp path
    tCIDLib::TCh szFile[kCIDLib::c4MaxPathLen];
    if (!::GetTempFileName(szPath, L"CID", 0, szFile))
    {
        stdOut << L"Could not get temp file" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::MakeTmpFile;
    }
    strToFill = szFile;
}


tCIDLib::TVoid TUtils::QueryCurDir(TBldStr& strToFill)
{
    const tCIDLib::TCard4   c4BufLen = 2047;
    tCIDLib::TCh            achBuf[c4BufLen + 1];
    if (!::GetCurrentDirectoryW(c4BufLen, achBuf))
    {
        stdOut << L"Could not get current directory" << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::QueryCurDir;
    }
    strToFill = achBuf;
}


tCIDLib::TVoid
TUtils::SetEnvVar(const TBldStr& strVar, const TBldStr& strValue)
{
    if (!::SetEnvironmentVariable(strVar.pszBuffer(), strValue.pszBuffer()))
    {
        stdOut << L"Could not set env var" << strVar << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::Internal;
    }
}

