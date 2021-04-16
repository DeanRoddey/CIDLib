//
// FILE NAME: CIDLib_FileLogger.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/27/1996
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
//  This file implements a file logger, which sends logged errors to a
//  binary output file, as flattened objects.
//
// CAVEATS/GOTCHAS:
//
//  1)  In order to speed things up, it formats first into a memory buffer
//      then writes the whole thing to disk as a binary blob. This is much
//      faster than flattening to a disk file stream directly.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TFileLogger,TObject)


// ---------------------------------------------------------------------------
//  TFileLogger: Constructors and operators
// ---------------------------------------------------------------------------
TFileLogger::TFileLogger() :

    m_pmtxSync(nullptr)
{
}

TFileLogger::TFileLogger(   const   tCIDLib::TCh* const pszFile
                            , const TResourceName&      rsnMutex) :
    m_pmtxSync(nullptr)
{
    Create(pszFile, rsnMutex);
}

TFileLogger::TFileLogger(   const   TString&        strFile
                            , const TResourceName&  rsnMutex) :
    m_pmtxSync(nullptr)
{
    Create(strFile, rsnMutex);
}

TFileLogger::~TFileLogger()
{
    try
    {
        m_flTarget.Close();
        delete m_pmtxSync;
    }

    catch(...)
    {
        // Can't log anything here, since we are a logger
    }
}


// ---------------------------------------------------------------------------
//  TFileLogger: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFileLogger::LogEvent(const TLogEvent& logevToLog)
{
    //
    //  Create a static memory buffer stream for formatting the errors
    //  to. We can get away with this because calls to this method are
    //  synced between threads in this process.
    //
    static TBinMBufOutStream strmFormat(kCIDLib::c4MemPageSize * 4);

    // Reset the stream pointer for the new message
    strmFormat.Reset();

    // And flatten the object to it
    strmFormat << logevToLog << kCIDLib::FlushIt;

    if (m_pmtxSync)
    {
        TLocker lockrSync(m_pmtxSync);
        m_flTarget.c4WriteBuffer
        (
            strmFormat.mbufData().pData()
            , strmFormat.c4CurPos()
        );
    }
     else
    {
        m_flTarget.c4WriteBuffer
        (
            strmFormat.mbufData().pData(), strmFormat.c4CurPos()
        );
    }
}


// ---------------------------------------------------------------------------
//  TFileLogger: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFileLogger::Create( const   TString&        strFile
                                    , const TResourceName&  rsnMutex)
{
    CIDAssert(m_pmtxSync == nullptr, L"This file logger was already created");

    m_flTarget.strName(strFile);
    m_flTarget.Open
    (
        tCIDLib::EAccessModes::Write
        , tCIDLib::ECreateActs::OpenOrCreate
        , tCIDLib::EFilePerms::AllAccess
        , tCIDLib::EFileFlags::SafeStream
    );

    // Seek to the end for appending
    m_flTarget.SeekToEnd();

    // Create the new mutex that we will use to sync if they want us to
    if (!TResourceName::bIsNullObject(rsnMutex))
        m_pmtxSync = new TMutex(rsnMutex, tCIDLib::ELockStates::Unlocked);
}

