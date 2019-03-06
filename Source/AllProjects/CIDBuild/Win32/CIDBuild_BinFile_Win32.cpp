//
// FILE NAME: CIDBuild_BinFile_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/1999
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
//  TBinFile: Public implementation methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TBinFile::c4CurPos() const
{
    // Do a seek relative to the current position
    const tCIDLib::TCard4 c4Ret = ::SetFilePointer(m_hflThis, 0, 0, FILE_CURRENT);

    // See if it there was an error or just a position over 32 bits
    if (c4Ret == 0xFFFFFFFF)
    {
        if (::GetLastError())
        {
            stdOut  << L"Warning: Could not query current position in file: "
                    << m_strFileName << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::SeekError;
        }
    }
    return c4Ret;
}


tCIDLib::TCard4 TBinFile::c4SeekToEnd()
{
    // Do a seek relative to the current position
    const tCIDLib::TCard4 c4Ret = ::SetFilePointer(m_hflThis, 0, 0, FILE_END);
    if (c4Ret == 0xFFFFFFFF)
    {
        if (::GetLastError())
        {
            stdOut  << L"Warning: Could not seek to end in file: "
                    << m_strFileName << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::SeekError;
        }
    }
    return c4Ret;
}


tCIDLib::TCard4 TBinFile::c4Size() const
{
    BY_HANDLE_FILE_INFORMATION  FileData;
    if (!::GetFileInformationByHandle(m_hflThis, &FileData))
    {
        stdOut  << L"Warning: Could not query size of file: "
                << m_strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::QueryError;
    }
    return FileData.nFileSizeLow;
}


tCIDLib::TVoid TBinFile::Close()
{
    if (m_hflThis)
    {
        if (!::CloseHandle(m_hflThis))
        {
            stdOut  << L"Warning: Could not close file: "
                    << m_strFileName << kCIDBuild::EndLn;
        }
        m_hflThis = 0;
    }
}


tCIDLib::TVoid TBinFile::Open(  const   TBldStr&                strName
                                , const tCIDLib::EAccessModes   eAccess
                                , const tCIDLib::ECreateActs eAction)
{
    if (m_hflThis != 0)
    {
        stdOut << L"The file " << strName << L" is already open"
               << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::OpenError;
    }

    m_strFileName = strName;

    // Convert the open action to a Win32 flag
    tCIDLib::TCard4 c4Action = 0;
    if (eAction == tCIDLib::ECreateActs::CreateIfNew)
        c4Action = CREATE_NEW;
    else if (eAction == tCIDLib::ECreateActs::CreateAlways)
        c4Action = CREATE_ALWAYS;
    else if (eAction == tCIDLib::ECreateActs::OpenIfExists)
        c4Action = OPEN_EXISTING;
    else if (eAction == tCIDLib::ECreateActs::OpenOrCreate)
        c4Action = OPEN_ALWAYS;
    else if (eAction == tCIDLib::ECreateActs::TruncateExisting)
        c4Action = TRUNCATE_EXISTING;

    // Convert the access mode to the correct platform flags
    tCIDLib::TCard4 c4Access = 0;
    if (eAccess == tCIDLib::EAccessModes::Read)
        c4Access = GENERIC_READ;
    else if (eAccess == tCIDLib::EAccessModes::Write)
        c4Access = GENERIC_WRITE;
    else
        c4Access = GENERIC_WRITE | GENERIC_READ;

    tCIDLib::TCard4 c4Share = 0;

    HANDLE hNew = ::CreateFile
    (
        m_strFileName.pszBuffer()
        , c4Access
        , c4Share
        , 0
        , c4Action
        , FILE_ATTRIBUTE_NORMAL
        , 0
    );

    if (hNew == INVALID_HANDLE_VALUE)
    {
        stdOut  << L"Warning: Could not open file: "
                << m_strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::OpenError;
    }

    // Store the new handle
    m_hflThis = hNew;
}


tCIDLib::TVoid TBinFile::Open(const tCIDLib::EStdFiles eStdFile)
{
    if (m_hflThis != 0)
    {
        stdOut << L"The standard file is already open"
               << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::OpenError;
    }

    if (eStdFile == tCIDLib::EStdFiles::StdIn)
        m_hflThis = ::GetStdHandle(STD_INPUT_HANDLE);
    else if (eStdFile == tCIDLib::EStdFiles::StdOut)
        m_hflThis = ::GetStdHandle(STD_OUTPUT_HANDLE);
    else if (eStdFile == tCIDLib::EStdFiles::StdErr)
        m_hflThis = ::GetStdHandle(STD_ERROR_HANDLE);
}


tCIDLib::TVoid TBinFile::SeekTo(const tCIDLib::TCard4 c4To)
{
    // Do a seek relative to the current position
    if (::SetFilePointer(m_hflThis, c4To, 0, FILE_BEGIN) == 0xFFFFFFFF)
    {
        if (::GetLastError())
        {
            stdOut  << L"Warning: Could not seek to in file: "
                    << m_strFileName << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::SeekError;
        }
    }
}


// ---------------------------------------------------------------------------
//  TBinFile: Private implementation methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TBinFile::c4ReadBytes(          tCIDLib::TVoid* const   pToFill
                        , const tCIDLib::TCard4         c4Max)
{
    DWORD dwActual;
    if (!::ReadFile(m_hflThis, pToFill, c4Max, &dwActual, 0))
    {
        stdOut << L"Error reading file: " << m_strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::ReadError;
    }
    return dwActual;
}


tCIDLib::TCard4
TBinFile::c4WriteBytes( const   tCIDLib::TVoid* const   pToWrite
                        , const tCIDLib::TCard4         c4ToWrite)
{
    DWORD dwActual;
    if (!::WriteFile(m_hflThis, pToWrite, c4ToWrite, &dwActual, 0))
    {
        stdOut << L"Error writing file: " << m_strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::ReadError;
    }
    return dwActual;
}

