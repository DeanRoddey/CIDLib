//
// FILE NAME: CIDBuild_BinFile.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/1999
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
//  This file implements the TBinFile class.
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
//   CLASS: TBinFile
//  PREFIX: bfl
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBinFile: Constructors and Destructor
// ---------------------------------------------------------------------------
TBinFile::TBinFile() :

    m_hflThis(0)
{
}

TBinFile::TBinFile( const   TBldStr&                strName
                    , const tCIDLib::EAccessModes   eAccess
                    , const tCIDLib::ECreateActs eAction) :
    m_hflThis(0)
{
    Open(strName, eAccess, eAction);
}

TBinFile::TBinFile(const tCIDLib::EStdFiles eStdFile) :

    m_hflThis(0)
{
    if (eStdFile == tCIDLib::EStdFiles::StdIn)
        m_strFileName = L"StdIn";
    else if (eStdFile == tCIDLib::EStdFiles::StdOut)
        m_strFileName = L"StdOut";
    else if (eStdFile == tCIDLib::EStdFiles::StdErr)
        m_strFileName = L"StdErr";

    Open(eStdFile);
}

TBinFile::~TBinFile()
{
    Close();
}


// ---------------------------------------------------------------------------
//  TBinFile: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TBinFile::c4Read(       tCIDLib::TVoid* const   pToFill
                , const tCIDLib::TCard4         c4Max
                , const tCIDLib::TBoolean       bMustReadAll)
{
    // Call the implementation to do the work
    const tCIDLib::TCard4 c4Actual = c4ReadBytes(pToFill, c4Max);

    // If we have to read all bytes, then check it
    if (bMustReadAll)
    {
        if (c4Actual != c4Max)
        {
            stdOut  << L"Not all bytes read from file: " << m_strFileName
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::ReadError;
        }
    }
    return c4Actual;
}

tCIDLib::TCard4
TBinFile::c4Write(  const   tCIDLib::TVoid* const   pToWrite
                    , const tCIDLib::TCard4         c4ToWrite
                    , const tCIDLib::TBoolean       bMustWriteAll)
{
    // Call the implementation to do the work
    const tCIDLib::TCard4 c4Actual = c4WriteBytes(pToWrite, c4ToWrite);

    // If we have to read all bytes, then check it
    if (bMustWriteAll)
    {
        if (c4Actual != c4ToWrite)
        {
            stdOut  << L"Not all bytes written to file: " << m_strFileName
                    << kCIDBuild::EndLn;
            throw tCIDBuild::EErrors::WriteError;
        }
    }
    return c4Actual;
}
