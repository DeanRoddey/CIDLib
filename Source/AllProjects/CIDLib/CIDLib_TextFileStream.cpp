//
// FILE NAME: CIDLib_TextFileStream.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/28/1996
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TTextFileInStream and TTextFileOutStream
//  classes. These are convenience classes which derive from TTextInStream
//  and TTextOutStream, respectively. They just do the grunt work required to
//  set up a file based binary stream under the text stream. You could
//  easily do the same stuff, but these just make it more convient and more
//  self documenting.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Our magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TTextFileInStream,TTextInStream)
RTTIDecls(TTextFileOutStream,TTextOutStream)



// ---------------------------------------------------------------------------
//   CLASS: TTextFileInStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTextFileInStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TTextFileInStream::TTextFileInStream(TTextConverter* const ptcvtToAdopt) :

    TTextInStream(ptcvtToAdopt)
    , m_pstrmIn(0)
{
}

TTextFileInStream::
TTextFileInStream(  const   TString&                strFileName
                    , const tCIDLib::ECreateActs eAction
                    , const tCIDLib::EFilePerms     ePerms
                    , const tCIDLib::EFileFlags     eFlags
                    , const tCIDLib::EAccessModes   eAccess
                    ,       TTextConverter* const   ptcvtToAdopt) :

    TTextInStream(ptcvtToAdopt)
    , m_pstrmIn(0)
{
    Open(strFileName, eAction, ePerms, eFlags, eAccess);
}

TTextFileInStream::
TTextFileInStream(  const   tCIDLib::EStdFiles      eFile
                    ,       TTextConverter* const   ptcvtToAdopt) :

    TTextInStream(ptcvtToAdopt)
    , m_pstrmIn(0)
{
    Open(eFile);
}

TTextFileInStream::~TTextFileInStream()
{
}


// ---------------------------------------------------------------------------
//  TTextFileStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TTextFileInStream::bIsOpen() const
{
    // If not underlying binary output stream, then obviousky not
    if (!m_pstrmIn)
        return kCIDLib::False;

    // Else ask the stream
    return m_pstrmIn->bIsOpen();
}


tCIDLib::TVoid TTextFileInStream::Close()
{
    if (m_pstrmIn)
        m_pstrmIn->Close();
}


tCIDLib::TCard8 TTextFileInStream::c8CurSize() const
{
    if (!m_pstrmIn)
        ThrowNotReady(CID_LINE);
    return m_pstrmIn->c8CurSize();
}


tCIDLib::TVoid
TTextFileInStream::Open(const   TString&                strFileName
                        , const tCIDLib::ECreateActs eAction
                        , const tCIDLib::EFilePerms     ePerms
                        , const tCIDLib::EFileFlags     eFlags
                        , const tCIDLib::EAccessModes   eAccess)
{
    //
    //  Create a binary file input stream if its not already. If its already
    //  created, then just try to open it.
    //
    if (!m_pstrmIn)
    {
        m_pstrmIn = new TBinFileInStream
        (
            strFileName
            , eAction
            , ePerms
            , eFlags
            , eAccess
        );

        // Pass on the new stream object to our base class, who owns it
        AdoptStream(m_pstrmIn);
    }
     else
    {
        m_pstrmIn->Open
        (
            strFileName
            , eAction
            , ePerms
            , eFlags
            , eAccess
        );
    }
}


tCIDLib::TVoid TTextFileInStream::Open(const tCIDLib::EStdFiles eFile)
{
    //
    //  Create a binary file output stream if its not already. If its already
    //  created, then just try to open it.
    //
    if (!m_pstrmIn)
    {
        m_pstrmIn = new TBinFileInStream(eFile);

        // Pass on the new stream object to our base class, who owns it
        AdoptStream(m_pstrmIn);
    }
     else
    {
        m_pstrmIn->Open(eFile);
    }
}


const TString& TTextFileInStream::strFileName() const
{
    if (!m_pstrmIn)
        ThrowNotReady(CID_LINE);

    // Just delegate to the underlying binary file stream
    return m_pstrmIn->strFileName();
}


// ---------------------------------------------------------------------------
//  Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid TTextFileInStream::ThrowNotReady(const tCIDLib::TCard4 c4Line) const
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , c4Line
        , kCIDErrs::errcStrm_FileTextInit
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotReady
    );
}






// ---------------------------------------------------------------------------
//   CLASS: TTextFileOutStream
//  PREFIX: strm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TTextFileOutStream: Constructors and Destructor
// ---------------------------------------------------------------------------
TTextFileOutStream::TTextFileOutStream(TTextConverter* const ptcvtToAdopt) :

    TTextOutStream(ptcvtToAdopt)
    , m_pstrmOut(0)
{
}

TTextFileOutStream::
TTextFileOutStream( const   TString&                strFileName
                    , const tCIDLib::ECreateActs eAction
                    , const tCIDLib::EFilePerms     ePerms
                    , const tCIDLib::EFileFlags     eFlags
                    , const tCIDLib::EAccessModes   eAccess
                    ,       TTextConverter* const   ptcvtToAdopt) :

    TTextOutStream(ptcvtToAdopt)
    , m_pstrmOut(0)
{
    Open(strFileName, eAction, ePerms, eFlags, eAccess);
}

TTextFileOutStream::
TTextFileOutStream( const   tCIDLib::EStdFiles      eFile
                    ,       TTextConverter* const   ptcvtToAdopt) :
    TTextOutStream(ptcvtToAdopt)
    , m_pstrmOut(0)
{
    Open(eFile);
}

TTextFileOutStream::~TTextFileOutStream()
{
}


// ---------------------------------------------------------------------------
//  TTextFileStream: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TTextFileOutStream::bIsOpen() const
{
    // If not underlying binary output stream, then obviousky not
    if (!m_pstrmOut)
        return kCIDLib::False;

    // Else ask the stream
    return m_pstrmOut->bIsOpen();
}


tCIDLib::TVoid TTextFileOutStream::Close()
{
    if (m_pstrmOut)
        m_pstrmOut->Close();
}


tCIDLib::TCard8 TTextFileOutStream::c8CurSize() const
{
    if (!m_pstrmOut)
        ThrowNotReady(CID_LINE);

    return m_pstrmOut->c8CurSize();
}


tCIDLib::TVoid
TTextFileOutStream::Open(const  TString&                strFileName
                        , const tCIDLib::ECreateActs eAction
                        , const tCIDLib::EFilePerms     ePerms
                        , const tCIDLib::EFileFlags     eFlags
                        , const tCIDLib::EAccessModes   eAccess)
{
    //
    //  Create a binary file output stream if its not already. If its already
    //  created, then just try to open it.
    //
    if (!m_pstrmOut)
    {
        m_pstrmOut = new TBinFileOutStream
        (
            strFileName
            , eAction
            , ePerms
            , eFlags
            , eAccess
        );

        // Pass on the new stream object to our base class, who owns it
        AdoptStream(m_pstrmOut);
    }
     else
    {
        m_pstrmOut->Open
        (
            strFileName
            , eAction
            , ePerms
            , eFlags
            , eAccess
        );
    }
}

tCIDLib::TVoid TTextFileOutStream::Open(const tCIDLib::EStdFiles eFile)
{
    //
    //  Create a binary file output stream if its not already. If its already
    //  created, then just try to open it.
    //
    if (!m_pstrmOut)
    {
        m_pstrmOut = new TBinFileOutStream(eFile);

        // Pass on the new stream object to our base class, who owns it
        AdoptStream(m_pstrmOut);
    }
     else
    {
        m_pstrmOut->Open(eFile);
    }
}


const TString& TTextFileOutStream::strFileName() const
{
    if (!m_pstrmOut)
        ThrowNotReady(CID_LINE);

    // Just delegate to the underlying binary file stream
    return m_pstrmOut->strFileName();
}


tCIDLib::TVoid TTextFileOutStream::SeekToEnd()
{
    if (!m_pstrmOut)
        ThrowNotReady(CID_LINE);

    m_pstrmOut->SeekToEnd();
}



// ---------------------------------------------------------------------------
//  Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid TTextFileOutStream::ThrowNotReady(const tCIDLib::TCard4 c4Line) const
{
    facCIDLib().ThrowErr
    (
        CID_FILE
        , c4Line
        , kCIDErrs::errcStrm_FileTextInit
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotReady
    );
}

