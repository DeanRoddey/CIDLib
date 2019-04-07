//
// FILE NAME: CIDLib_FileStreamImpl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/24/1996
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
//  This file implements the TFileInStreamImpl and TFileOutStreamImpl classes,
//  which provide the ability for a binary input or output stream to use a
//  binary file object as the underlying data sink/source.
//
//  Also defined here is the MFileStreamImpl, which provides common code that
//  is used by both of the other classes. It is mixed into each of them, and
//  provides the code that is not related to reading or writing data.
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
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFileInStreamImpl,TInStreamImpl)
RTTIDecls(TFileOutStreamImpl,TOutStreamImpl)



// ---------------------------------------------------------------------------
//   CLASS: TFileInStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFileInStreamImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TFileInStreamImpl::TFileInStreamImpl(       TBinaryFile* const  pflToUse
                                    , const tCIDLib::EAdoptOpts eAdopt) :
    m_eAdopted(eAdopt)
    , m_pflData(pflToUse)
{
    CIDAssert(m_pflData != 0, L"The adopted binary stream cannot be null");
}

TFileInStreamImpl::~TFileInStreamImpl()
{
    if (m_eAdopted == tCIDLib::EAdoptOpts::Adopt)
    {
        m_pflData->Close();
        delete m_pflData;
    }
}


// ---------------------------------------------------------------------------
//  TFileInStreamImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TFileInStreamImpl::c4ReadBytes(         tCIDLib::TVoid* const   pBuffer
                                , const tCIDLib::TCard4         c4BytesToRead)
{
    // Read int othe buffer, and return how many we actually wrote
    return m_pflData->c4ReadBuffer(pBuffer, c4BytesToRead);
}

tCIDLib::TVoid TFileInStreamImpl::SkipForwardBy(const tCIDLib::TCard4 c4SkipBy)
{
    // Remember the current position
    tCIDLib::TCard8 c8Current = m_pflData->c8CurPos();

    // And then offset the pointer and save the new position
    tCIDLib::TCard8 c8Actual = m_pflData->c8OffsetFilePos
    (
        tCIDLib::TInt8(c4SkipBy)
    );

    //
    //  If the current position plus the skip is not equal to actual, then
    //  we did not succeed.
    //
    if (c8Current + c4SkipBy != c8Actual)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_InvalidRelSeek
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4SkipBy)
        );
    }
}


// ---------------------------------------------------------------------------
//  TFileInStreamImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFileInStreamImpl::bIsOpen() const
{
    // If no file object, then definitely not
    if (!m_pflData)
        return kCIDLib::False;

    // Else ask the file object if its open
    return m_pflData->bIsOpen();
}

tCIDLib::TCard8 TFileInStreamImpl::c8CurPos() const
{
    return m_pflData->c8CurPos();
}

tCIDLib::TVoid TFileInStreamImpl::Reset()
{
    // Just seek back to the start
    m_pflData->SetFilePos(0);
}


// ---------------------------------------------------------------------------
//  TFileInStreamImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFileInStreamImpl::AdoptFile()
{
    m_eAdopted = tCIDLib::EAdoptOpts::Adopt;
}

tCIDLib::TBoolean TFileInStreamImpl::bFileIsAdopted() const
{
    return (m_eAdopted == tCIDLib::EAdoptOpts::Adopt);
}

tCIDLib::TVoid TFileInStreamImpl::Close()
{
    if (m_pflData)
        m_pflData->Close();
}

tCIDLib::TCard8 TFileInStreamImpl::c8CurSize() const
{
    return m_pflData->c8CurSize();
}

tCIDLib::TVoid
TFileInStreamImpl::Open(const   tCIDLib::ECreateActs eAction
                        , const tCIDLib::EFilePerms     ePerms
                        , const tCIDLib::EFileFlags     eFlags)
{
    // Pass on what we got, and ask for read access only
    m_pflData->Open(tCIDLib::EAccessModes::Read, eAction, ePerms, eFlags);
}

const TString& TFileInStreamImpl::strFileName() const
{
    return m_pflData->strName();
}




// ---------------------------------------------------------------------------
//   CLASS: TFileOutStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFileOutStreamImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TFileOutStreamImpl::TFileOutStreamImpl(         TBinaryFile* const  pflToUse
                                        , const tCIDLib::EAdoptOpts eAdopt) :
    m_eAdopted(eAdopt)
    , m_pflData(pflToUse)
{
    CIDAssert(m_pflData != 0, L"The adopted binary stream cannot be null");
}

TFileOutStreamImpl::~TFileOutStreamImpl()
{
    if (m_eAdopted == tCIDLib::EAdoptOpts::Adopt)
    {
        m_pflData->Close();
        delete m_pflData;
    }
}


// ---------------------------------------------------------------------------
//  TFileOutStreamImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TFileOutStreamImpl::c4WriteBytes(const  tCIDLib::TVoid* const   pBuffer
                                , const tCIDLib::TCard4         c4BytesToWrite)
{
    // Write the buffer and return how many we actually wrote
    return m_pflData->c4WriteBuffer(pBuffer, c4BytesToWrite);
}


tCIDLib::TVoid TFileOutStreamImpl::SeekToEnd()
{
    // Just set the file pointer to the reported size of the file
    m_pflData->SetFilePos(m_pflData->c8CurSize());
}



// ---------------------------------------------------------------------------
//  TFileOutStreamImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TFileOutStreamImpl::bIsOpen() const
{
    // If no file object, then definitely not
    if (!m_pflData)
        return kCIDLib::False;

    // Else ask the file object if its open
    return m_pflData->bIsOpen();
}

tCIDLib::TCard8 TFileOutStreamImpl::c8CurPos() const
{
    return m_pflData->c8CurPos();
}

tCIDLib::TVoid TFileInStreamImpl::Open(const tCIDLib::EStdFiles eFile)
{
    m_pflData->Open(eFile);
}

tCIDLib::TVoid TFileOutStreamImpl::Reset()
{
    // Truncate the file back to zero size
    m_pflData->TruncateAt(0);
}




// ---------------------------------------------------------------------------
//  TFileOutStreamImpl: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TFileOutStreamImpl::AdoptFile()
{
    m_eAdopted = tCIDLib::EAdoptOpts::Adopt;
}

tCIDLib::TBoolean TFileOutStreamImpl::bFileIsAdopted() const
{
    return (m_eAdopted == tCIDLib::EAdoptOpts::Adopt);
}

tCIDLib::TVoid TFileOutStreamImpl::Close()
{
    if (m_pflData)
        m_pflData->Close();
}

tCIDLib::TCard8 TFileOutStreamImpl::c8CurSize() const
{
    return m_pflData->c8CurSize();
}

tCIDLib::TVoid
TFileOutStreamImpl::Open(const  tCIDLib::ECreateActs eAction
                        , const tCIDLib::EFilePerms     ePerms
                        , const tCIDLib::EFileFlags     eFlags)
{
    // Pass on what we got, and ask for write access only
    m_pflData->Open(tCIDLib::EAccessModes::Write, eAction, ePerms, eFlags);
}

tCIDLib::TVoid TFileOutStreamImpl::Open(const tCIDLib::EStdFiles eFile)
{
    m_pflData->Open(eFile);
}

const TString& TFileOutStreamImpl::strFileName() const
{
    return m_pflData->strName();
}

tCIDLib::TVoid
TFileOutStreamImpl::TruncateAt(const tCIDLib::TCard8& c8Pos)
{
    m_pflData->TruncateAt(c8Pos);
}

