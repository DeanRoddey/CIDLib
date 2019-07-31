//
// FILE NAME: CIDBuild_BinFile_Linux.Cpp
//
// AUTHOR: Will Mason
//
// CREATED: 02/23/1999
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
#include    <sys/stat.h>
#include    <fcntl.h>
#include    <unistd.h>



// ---------------------------------------------------------------------------
//  TBinFile: Public implementation methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TBinFile::c4CurPos() const
{
    // Do a seek relative to the current position
    off_t Offset = ::lseek(m_hflThis, 0, SEEK_CUR);

    // See if it there was an error or just a position over 32 bits
    if (Offset == -1)
    {
        stdOut  << L"Warning: Could not query current position in file: "
                << m_strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::SeekError;
    }

    return tCIDLib::TCard4(Offset);
}


tCIDLib::TCard4 TBinFile::c4SeekToEnd()
{
    // Do a seek relative to the current position
    off_t Offset = ::lseek(m_hflThis, 0, SEEK_END);

    // See if it there was an error or just a position over 32 bits
    if (Offset == -1)
    {
        stdOut  << L"Warning: Could not query current position in file: "
                << m_strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::SeekError;
    }

    return tCIDLib::TCard4(Offset);
}


tCIDLib::TCard4 TBinFile::c4Size() const
{
    struct stat StatBuf;
    if (::fstat(m_hflThis, &StatBuf))
    {
        stdOut  << L"Warning: Could not query size of file: "
                << m_strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::QueryError;
    }
    return StatBuf.st_size;
}


tCIDLib::TVoid TBinFile::Close()
{
    if (::close(m_hflThis))
    {
        stdOut  << L"Warning: Could not close file: "
                << m_strFileName << kCIDBuild::EndLn;
    }    
}


tCIDLib::TVoid TBinFile::Open(  const   TBldStr&                strName
                                , const tCIDLib::EAccessModes   eAccess
                                , const tCIDLib::ECreateActs    eAction)
{
    tCIDLib::TSInt iFlags = 0;
    if (eAction == tCIDLib::ECreateActs::TruncateExisting)
        iFlags = O_TRUNC;
    else if (eAction == tCIDLib::ECreateActs::CreateIfNew)
        iFlags = O_CREAT | O_EXCL;
    else if (eAction == tCIDLib::ECreateActs::OpenIfExists)
        iFlags = O_APPEND;
    else if (eAction == tCIDLib::ECreateActs::CreateAlways)
        iFlags = O_CREAT | O_TRUNC;
    else if (eAction == tCIDLib::ECreateActs::OpenOrCreate)
        iFlags = O_CREAT | O_APPEND;

    mode_t Mode = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP;
    if (eAccess == tCIDLib::EAccessModes::Read)
        iFlags |= O_RDONLY;
    else if (eAccess == tCIDLib::EAccessModes::Write)
        iFlags |= O_WRONLY;
    else if (eAccess == tCIDLib::EAccessModes::ReadWrite)
        iFlags |= O_RDWR;

    tCIDBuild::TSCh* pszName = TRawStr::pszTranscode(m_strFileName.pszBuffer());
    TArrayJanitor<tCIDBuild::TSCh> janName(pszName);

    tCIDLib::TSInt iNew = ::open(pszName, iFlags, Mode);

    if (iNew == -1)
    {
        stdOut  << L"Warning: Could not open file: "
                << m_strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::OpenError;
    }

    m_hflThis = iNew;    
}


tCIDLib::TVoid TBinFile::Open(const tCIDLib::EStdFiles eStdFile)
{
    if (eStdFile == tCIDLib::EStdFiles::StdIn)
        m_hflThis = 0;
    else if (eStdFile == tCIDLib::EStdFiles::StdOut)
        m_hflThis = 1;
    else if (eStdFile == tCIDLib::EStdFiles::StdErr)
        m_hflThis = 2;
}


tCIDLib::TVoid TBinFile::SeekTo(const tCIDLib::TCard4 c4To)
{
    // Do a seek relative to the current position
    off_t Offset = ::lseek(m_hflThis, c4To, SEEK_SET);

    // See if it there was an error or just a position over 32 bits
    if (Offset == -1)
    {
        stdOut  << L"Warning: Could not set to position in file: "
                << m_strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::SeekError;
    }
}


// ---------------------------------------------------------------------------
//  TBinFile: Private implementation methods
// ---------------------------------------------------------------------------

tCIDLib::TCard4
TBinFile::c4ReadBytes(          tCIDLib::TVoid* const   pToFill
                        , const tCIDLib::TCard4         c4Max)
{
    ssize_t SizeRead = ::read(m_hflThis, pToFill, c4Max);
    if (SizeRead == -1)
    {
        stdOut << L"Error reading file: " << m_strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::ReadError;
    }
    return SizeRead;
}


tCIDLib::TCard4
TBinFile::c4WriteBytes( const   tCIDLib::TVoid* const   pToWrite
                        , const tCIDLib::TCard4         c4ToWrite)
{
    ssize_t SizeRead = ::write(m_hflThis, pToWrite, c4ToWrite);
    if (SizeRead == -1)
    {
        stdOut << L"Error writing file: " << m_strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::ReadError;
    }
    return SizeRead;
}
