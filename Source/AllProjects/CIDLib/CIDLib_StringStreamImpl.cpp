//
// FILE NAME: CIDLib_StringStreamImpl.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/28/1996
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
//  This method implements the stream implementation derivatives that allow
//  a stream to use a string as a data sink/source.
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
RTTIDecls(TStringInStreamImpl,TInStreamImpl)
RTTIDecls(TStringOutStreamImpl,TOutStreamImpl)




// ---------------------------------------------------------------------------
//   CLASS: TStringStreamImplInfo
//  PREFIX: sii
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStringStreamImplInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TStringStreamImplInfo::
TStringStreamImplInfo(  const   TString* const      pstrToUse
                        , const tCIDLib::EAdoptOpts eAdoptBuf) :
    eAdopted(eAdoptBuf)
    , pstrData(pstrToUse)
{
}

TStringStreamImplInfo::~TStringStreamImplInfo()
{
    // If we adopted the string then clean it up now
    if (eAdopted == tCIDLib::EAdoptOpts::Adopt)
    {
        delete pstrData;
        pstrData = 0;
    }
}



// ---------------------------------------------------------------------------
//   CLASS: TStringInStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStringInStreamImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TStringInStreamImpl::TStringInStreamImpl(const  TString* const      pstrToUse
                                        , const tCIDLib::EAdoptOpts eAdopt) :
    m_c4CurPos(0)
    , m_pstrIn(pstrToUse)
{
    // Looks ok, so create the info object and store it
    m_cptrInfo.SetPointer(new TStringStreamImplInfo(pstrToUse, eAdopt));
}

TStringInStreamImpl::TStringInStreamImpl(const tCIDLib::TCard4 c4InitSize) :

    m_c4CurPos(0)
    , m_pstrIn(0)
{
    // Create the new string and store our own pointer to it
    m_pstrIn = new TString(c4InitSize);

    // And create the info object and pass it the string to adopt
    m_cptrInfo.SetPointer
    (
        new TStringStreamImplInfo(m_pstrIn, tCIDLib::EAdoptOpts::Adopt)
    );
}

TStringInStreamImpl::
TStringInStreamImpl(const TStringOutStreamImpl& strmiToSyncWith) :

    m_c4CurPos(0)
    , m_cptrInfo(strmiToSyncWith.m_cptrInfo)
    , m_pstrIn(0)
{
    // Get a pointer to the string string
    m_pstrIn = m_cptrInfo->pstrData;
}


TStringInStreamImpl::~TStringInStreamImpl()
{
    // Dtor of the counter pointer will clean up info object if no more refs
}


// ---------------------------------------------------------------------------
//  TStringInStreamImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TStringInStreamImpl::c4ReadBytes(       tCIDLib::TVoid* const   pBuffer
                                , const tCIDLib::TCard4         c4BytesToRead)
{
    //
    //  Do a sanity check that the number of bytes is an even multiple of
    //  of the native character size. Since a string text stream will always
    //  have a native wide char converter, this should always be true.
    //
    if (c4BytesToRead % kCIDLib::c4CharBytes)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_OddByteCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4BytesToRead)
        );
    }

    //
    //  Calculate the chars we can read. Its the smaller of either the chars
    //  left in the string, or the chars asked for.
    //
    //  If debugging, do an emergency check that the current position isn't
    //  beyond the current available length. This can happen if they read
    //  into the same string that they provided as the source for the stream.
    //
    #if CID_DEBUG_ON
    if (m_pstrIn->c4Length() < m_c4CurPos)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_ImplDataSzErr
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Internal
        );
    }
    #endif
    const tCIDLib::TCard4 c4Actual = tCIDLib::MinVal
    (
        m_pstrIn->c4Length() - m_c4CurPos
        , c4BytesToRead / kCIDLib::c4CharBytes
    );

    //
    //  So now just copy the requested bytes from the current position in the
    //  string to the caller's buffer.
    //
    const tCIDLib::TCh* pszTmp = m_pstrIn->pszBuffer();
    TRawMem::CopyMemBuf
    (
        pBuffer
        , &pszTmp[m_c4CurPos]
        , c4Actual * kCIDLib::c4CharBytes
    );

    // Adjust the current position by the chars read
    m_c4CurPos += c4Actual;

    // Return the bytes read
    return c4Actual * kCIDLib::c4CharBytes;
}


tCIDLib::TVoid TStringInStreamImpl::SkipForwardBy(const tCIDLib::TCard4 c4SkipBy)
{
    // Calculate some stuff up front
    const tCIDLib::TCard4 c4Length = m_pstrIn->c4Length();
    const tCIDLib::TCard4 c4SeekChars = c4SkipBy / kCIDLib::c4CharBytes;
    const tCIDLib::TCard4 c4NewPos = m_c4CurPos + c4SeekChars;

    //
    //  See whether this would seek beyond the logcal end. If so, this
    //  is an error. Also, make sure its an even number of bytes we are being
    //  asked to seek, since this type of stream can only work on even bytes.
    //
    if ((c4NewPos > c4Length) || (c4SkipBy % 0x1))
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

    // Move up to the new position
    m_c4CurPos = c4NewPos;
}




// ---------------------------------------------------------------------------
//   CLASS: TStringOutStreamImpl
//  PREFIX: strmi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStringOutStreamImpl: Constructors and Destructor
// ---------------------------------------------------------------------------
TStringOutStreamImpl::TStringOutStreamImpl(         TString* const      pstrToUse
                                          , const   tCIDLib::EAdoptOpts eAdopt
                                          , const   tCIDLib::TBoolean   bAppend) :
    m_c4CurPos(0)
    , m_pstrOut(pstrToUse)
{
    // If appending, keep the current content, else clear it for output
    if (bAppend)
        m_c4CurPos = m_pstrOut->c4Length();
    else
        m_pstrOut->Clear();
    m_cptrInfo.SetPointer(new TStringStreamImplInfo(pstrToUse, eAdopt));
}

TStringOutStreamImpl::TStringOutStreamImpl(const tCIDLib::TCard4 c4InitSize) :

    m_c4CurPos(0)
    , m_pstrOut(0)
{
    // Create the string and store our pointer to it
    m_pstrOut = new TString(kCIDLib::pszEmptyZStr, c4InitSize);
    m_cptrInfo.SetPointer
    (
        new TStringStreamImplInfo(m_pstrOut, tCIDLib::EAdoptOpts::Adopt)
    );
}

TStringOutStreamImpl::~TStringOutStreamImpl()
{
    // Dtor of the counter pointer will clean up info object if no more refs
}


// ---------------------------------------------------------------------------
//  TStringOutStreamImpl: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4
TStringOutStreamImpl::c4WriteBytes( const   tCIDLib::TVoid* const pToWrite
                                    , const tCIDLib::TCard4       c4BytesToWrite)
{
    //
    //  Do a sanity check that the number of bytes is an even multiple of
    //  of the native character size. Since a string text stream will always
    //  have a native wide char converter, this should always be true.
    //
    if (c4BytesToWrite % kCIDLib::c4CharBytes)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcStrm_OddByteCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4BytesToWrite)
        );
    }

    //
    //  Calc the number of chars to write. Its the smaller of the chars that
    //  we could write and the chars requested.
    //
    const tCIDLib::TCard4 c4ActualChars = tCIDLib::MinVal
    (
        ((kCIDLib::c4MaxCard - m_pstrOut->c4Length()) / kCIDLib::c4CharBytes)
        , c4BytesToWrite / kCIDLib::c4CharBytes
    );

    //
    //  If this would exceed the current allocation of the string, then
    //  expand it. We first calculate the size required to hold the new data,
    //  then add 25 percent. If this is larger than the max expansion, then
    //  its clipped back.
    //
    if (m_c4CurPos + c4ActualChars > m_pstrOut->c4BufChars())
    {
        tCIDLib::TCard4 c4New = tCIDLib::TCard4
        (
            (m_pstrOut->c4BufChars() + c4ActualChars) * 1.25
        );

        // The default is to preserve existing content, which is what we want
        m_pstrOut->Reallocate(c4New);
    }

    //
    //  Calc the actual bytes to write and copy the text into the string
    //  object's raw buffer.
    //
    m_pstrOut->AppendSubStr
    (
        reinterpret_cast<const tCIDLib::TCh*>(pToWrite), 0, c4ActualChars
    );

    // Bump up the current position by the chars written
    m_c4CurPos += c4ActualChars;

    // Return the bytes we actually wrote
    return c4ActualChars * kCIDLib::c4CharBytes;
}
