//
// FILE NAME: CIDLib_BinaryFileStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/26/1996
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
//  This file implements simple derivatives of the binary stream classes.
//  These handle the issues of creating the stream implementation object and
//  getting the file created and opened. You can do this yourself and use a
//  TBinInStream/TBinOutStream directly, but this one is safer and more
//  convenient.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macros
// ---------------------------------------------------------------------------
RTTIDecls(TBinFileInStream,TBinInStream)
RTTIDecls(TBinFileOutStream,TBinOutStream)



// ---------------------------------------------------------------------------
//   CLASS: TBinFileInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBinFileInStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TBinFileInStream::TBinFileInStream() :

    m_pstrmiFile(0)
{
}

TBinFileInStream::TBinFileInStream(const tCIDLib::EStdFiles eFile) :

    m_pstrmiFile(0)
{
    Open(eFile);
}

TBinFileInStream::
TBinFileInStream(const  TString&                strFileName
                , const tCIDLib::ECreateActs eAction
                , const tCIDLib::EFilePerms     ePerms
                , const tCIDLib::EFileFlags     eFlags
                , const tCIDLib::EAccessModes   eAccess) :

    m_pstrmiFile(0)
{
    Open(strFileName, eAction, ePerms, eFlags, eAccess);
}

TBinFileInStream::~TBinFileInStream()
{
    // Our parent owns the impl object!
}


// ---------------------------------------------------------------------------
//  TBinFileInStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard8 TBinFileInStream::c8CurSize() const
{
    return m_pstrmiFile->c8CurSize();
}


tCIDLib::TVoid TBinFileInStream::Close()
{
    //
    //  First check that there is an implementation object. This can sometimes
    //  get called after a failed open, and we want to allow it to happen.
    //
    if (m_pstrmiFile)
    {
        m_pstrmiFile->Close();
        m_pstrmiFile = 0;

        // Ask our parent to clean up the implementation object
        DeleteImplObject();
    }
}


tCIDLib::TVoid
TBinFileInStream::Open( const   TString&                strFileName
                        , const tCIDLib::ECreateActs eAction
                        , const tCIDLib::EFilePerms     ePerms
                        , const tCIDLib::EFileFlags     eFlags
                        , const tCIDLib::EAccessModes   eAccess)
{
    // Make sure its not already open
    if (m_pstrmiFile)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_AlreadyOpen
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , strFileName
        );
    }

    // Init these guys so we can clean up in case of error
    TBinaryFile*        pflToUse    = 0;
    TFileInStreamImpl*  pstrmiToUse = 0;
    try
    {
        // Construct the binary file object
        pflToUse = new TBinaryFile(strFileName);

        // Try to open/create it with the provided information
        pflToUse->Open(eAccess, eAction, ePerms, eFlags);

        //
        //  Ok, create the file stream implementation object and give it
        //  the file we just created and tell it to adopt it.
        //
        pstrmiToUse = new TFileInStreamImpl(pflToUse, tCIDLib::EAdoptOpts::Adopt);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        if (pstrmiToUse)
        {
            delete pstrmiToUse;
            pstrmiToUse = 0;
        }
         else
        {
            delete pflToUse;
            pflToUse = 0;
        }
        throw;
    }

    // Pass on the new stream implementation object to our base class
    AdoptImplObject(pstrmiToUse);

    // And save our copy
    m_pstrmiFile = pstrmiToUse;
}


tCIDLib::TVoid TBinFileInStream::Open(const tCIDLib::EStdFiles eFile)
{
    // Make sure its a standard file that's meaningful for this stream type
    if (eFile != tCIDLib::EStdFiles::StdIn)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_WrongStdFileType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // Make sure its not already open
    if (m_pstrmiFile)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_AlreadyOpen
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , strFileName()
        );
    }

    // Init these guys so we can clean up in case of error
    TBinaryFile*        pflToUse    = 0;
    TFileInStreamImpl*  pstrmiToUse = 0;

    try
    {
        // Construct the binary file object for the standard file
        pflToUse = new TBinaryFile;
        pflToUse->Open(eFile);

        //
        //  Ok, create the file stream implementation object and give it
        //  the file we just created and tell it to adopt it.
        //
        pstrmiToUse = new TFileInStreamImpl(pflToUse, tCIDLib::EAdoptOpts::Adopt);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        if (pstrmiToUse)
        {
            delete pstrmiToUse;
            pstrmiToUse = 0;
        }
         else
        {
            delete pflToUse;
            pflToUse = 0;
        }
        throw;
    }

    // Pass on the new stream implementation object to our base class
    AdoptImplObject(pstrmiToUse);

    // And save our copy
    m_pstrmiFile = pstrmiToUse;
}


const TString& TBinFileInStream::strFileName() const
{
    return m_pstrmiFile->strFileName();
}




// ---------------------------------------------------------------------------
//   CLASS: TBinOutFileStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TBinFileOutStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TBinFileOutStream::TBinFileOutStream() :

    m_pstrmiFile(0)
{
}

TBinFileOutStream::
TBinFileOutStream(  const   TString&                strFileName
                    , const tCIDLib::ECreateActs eAction
                    , const tCIDLib::EFilePerms     ePerms
                    , const tCIDLib::EFileFlags     eFlags
                    , const tCIDLib::EAccessModes   eAccess) :

    m_pstrmiFile(0)
{
    Open(strFileName, eAction, ePerms, eFlags, eAccess);
}

TBinFileOutStream::TBinFileOutStream(const tCIDLib::EStdFiles eFile) :

    m_pstrmiFile(0)
{
    Open(eFile);
}

TBinFileOutStream::~TBinFileOutStream()
{
    // Our parent owns the impl object!
}


// ---------------------------------------------------------------------------
//  TBinFileOutStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard8 TBinFileOutStream::c8CurSize() const
{
    CIDAssert(m_pstrmiFile != 0, L"The stream impl object has not been created");
    return m_pstrmiFile->c8CurSize();
}


tCIDLib::TVoid TBinFileOutStream::Close()
{
    //
    //  First check that there is an implementation object. This can
    //  possibly get called after a failed open, and we want to allow it
    //  to happen.
    //
    if (m_pstrmiFile)
    {
        m_pstrmiFile->Close();
        m_pstrmiFile = 0;
        DeleteImplObject();
    }
}


tCIDLib::TVoid
TBinFileOutStream::Open(const   TString&                strFileName
                        , const tCIDLib::ECreateActs eAction
                        , const tCIDLib::EFilePerms     ePerms
                        , const tCIDLib::EFileFlags     eFlags
                        , const tCIDLib::EAccessModes   eAccess)
{
    // Make sure its not already open
    if (m_pstrmiFile)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcFile_AlreadyOpen
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Already
            , strFileName
        );
    }

    // Init these guys so we can clean up in case of error
    TBinaryFile*        pflToUse    = 0;
    TFileOutStreamImpl* pstrmiToUse = 0;

    try
    {
        // Construct the binary file object
        pflToUse = new TBinaryFile(strFileName);

        // Try to open/create it with the provided information
        pflToUse->Open(eAccess, eAction, ePerms, eFlags);

        //
        //  Ok, create the file stream implementation object and give it
        //  the file we just created and tell it to adopt it.
        //
        pstrmiToUse = new TFileOutStreamImpl(pflToUse, tCIDLib::EAdoptOpts::Adopt);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        if (pstrmiToUse)
        {
            delete pstrmiToUse;
            pstrmiToUse = 0;
        }
         else
        {
            delete pflToUse;
            pflToUse = 0;
        }
        throw;
    }

    // Pass on the new stream implementation object to our base class
    AdoptImplObject(pstrmiToUse);

    // And store our version
    m_pstrmiFile = pstrmiToUse;
}


tCIDLib::TVoid TBinFileOutStream::Open(const tCIDLib::EStdFiles eFile)
{
    // Make sure its a standard file that's meaningful for this stream type
    if (eFile == tCIDLib::EStdFiles::StdIn)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_WrongStdFileType
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::AppError
        );
    }

    // If we already have an impl object, then just ask it to open
    if (m_pstrmiFile)
    {
        m_pstrmiFile->Open(eFile);
        return;
    }

    // Init these guys so we can clean up in case of error
    TBinaryFile*        pflToUse    = 0;
    TFileOutStreamImpl* pstrmiToUse = 0;

    try
    {
        // Construct the binary file object for the standard file
        pflToUse = new TBinaryFile;
        pflToUse->Open(eFile);

        //
        //  Ok, create the file stream implementation object and give it
        //  the file we just created and tell it to adopt it.
        //
        pstrmiToUse = new TFileOutStreamImpl(pflToUse, tCIDLib::EAdoptOpts::Adopt);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
        if (pstrmiToUse)
        {
            delete pstrmiToUse;
            pstrmiToUse = 0;
        }
         else
        {
            delete pflToUse;
            pflToUse = 0;
        }
        throw;
    }

    catch(...)
    {
        if (pstrmiToUse)
        {
            delete pstrmiToUse;
            pstrmiToUse = 0;
        }
         else
        {
            delete pflToUse;
            pflToUse = 0;
        }
        throw;
    }

    // Pass on the new stream implementation object to our base class
    AdoptImplObject(pstrmiToUse);

    // And store our version
    m_pstrmiFile = pstrmiToUse;
}


const TString& TBinFileOutStream::strFileName() const
{
    CIDAssert(m_pstrmiFile != 0, L"The stream impl object has not been created");
    return m_pstrmiFile->strFileName();
}


tCIDLib::TVoid TBinFileOutStream::SeekToEnd()
{
    CIDAssert(m_pstrmiFile != 0, L"The stream impl object has not been created");
    m_pstrmiFile->SeekToEnd();
}


tCIDLib::TVoid TBinFileOutStream::TruncateAt(const tCIDLib::TCard8& c8At)
{
    CIDAssert(m_pstrmiFile != 0, L"The stream impl object has not been created");

    // Flush the cache first, then truncate it
    Flush();
    m_pstrmiFile->TruncateAt(c8At);
}

