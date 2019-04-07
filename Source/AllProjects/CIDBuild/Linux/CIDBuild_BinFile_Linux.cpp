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



// ---------------------------------------------------------------------------
//  TBinFile: Public implementation methods
// ---------------------------------------------------------------------------
tCIDBuild::TUInt TBinFile::uiCurPos() const
{
    // Do a seek relative to the current position
    off_t Offset = ::lseek(__hflThis, 0, SEEK_CUR);

    // See if it there was an error or just a position over 32 bits
    if (Offset == -1)
    {
        stdOut  << NStr("Warning: Could not query current position in file: ")
                << __strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::SeekError;
    }

    return Offset;
}


tCIDBuild::TUInt TBinFile::uiSize() const
{
    struct stat StatBuf;
    if (::fstat(__hflThis, &StatBuf))
    {
        stdOut  << NStr("Warning: Could not query size of file: ")
                << __strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::QueryError;
    }
    return StatBuf.st_size;
}


// ---------------------------------------------------------------------------
//  TBinFile: Private implementation methods
// ---------------------------------------------------------------------------
tCIDBuild::TVoid TBinFile::__Close()
{
    if (::close(__hflThis))
    {
        stdOut  << NStr("Warning: Could not close file: ")
                << __strFileName << kCIDBuild::EndLn;
    }
}


tCIDBuild::TVoid TBinFile::__Open(  const   tCIDBuild::EAccessModes eAccess
                                    , const tCIDBuild::EOpenActions eAction)
{
    tCIDBuild::TInt iFlags = 0;
    if (eAction == tCIDBuild::EOpenAction_Truncate)
        iFlags = O_TRUNC;
    else if (eAction == tCIDBuild::EOpenAction_FailIfExists)
        iFlags = O_CREAT | O_EXCL;
    else if (eAction == tCIDBuild::EOpenAction_OpenExisting)
        iFlags = O_APPEND;
    else if (eAction == tCIDBuild::EOpenAction_CreateAlways)
        iFlags = O_CREAT | O_TRUNC;
    else if (eAction == tCIDBuild::EOpenAction_OpenAlways)
        iFlags = O_CREAT | O_APPEND;

    mode_t Mode = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP;
    if (eAccess == tCIDBuild::EAccess_Read)
        iFlags |= O_RDONLY;
    else if (eAccess == tCIDBuild::EAccess_Write)
        iFlags |= O_WRONLY;
    else if (eAccess == tCIDBuild::EAccess_ReadWrite)
        iFlags |= O_RDWR;

    tCIDBuild::TNatCh* pszName = TRawStr::pszTranscode(__strFileName.pszBuffer());
    TArrayJanitor<tCIDBuild::TNatCh> janName(pszName);

    tCIDBuild::TInt iNew = ::open(pszName, iFlags, Mode);

    if (iNew == -1)
    {
        stdOut  << NStr("Warning: Could not open file: ")
                << __strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::OpenError;
    }

    __hflThis = iNew;
}


tCIDBuild::TVoid TBinFile::__Open(const tCIDBuild::EStdFiles eStdFile)
{
    if (eStdFile == tCIDBuild::EStdFiles::In)
        __hflThis = 0;
    else if (eStdFile == tCIDBuild::EStdFiles::Out)
        __hflThis = 1;
    else if (eStdFile == tCIDBuild::EStdFiles::Err)
        __hflThis = 2;
}


tCIDBuild::TUInt
TBinFile::__uiReadBytes(        tCIDBuild::TVoid* const pToFill
                        , const tCIDBuild::TUInt        uiMax)
{
    ssize_t SizeRead = ::read(__hflThis, pToFill, uiMax);
    if (SizeRead == -1)
    {
        stdOut << NStr("Error reading file: ") << __strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::ReadError;
    }
    return SizeRead;
}


tCIDBuild::TUInt
TBinFile::__uiWriteBytes(   const   tCIDBuild::TVoid* const pToWrite
                            , const tCIDBuild::TUInt        uiToWrite)
{
    ssize_t SizeRead = ::write(__hflThis, pToWrite, uiToWrite);
    if (SizeRead == -1)
    {
        stdOut << NStr("Error writing file: ") << __strFileName << kCIDBuild::EndLn;
        throw tCIDBuild::EErrors::ReadError;
    }
    return SizeRead;
}
