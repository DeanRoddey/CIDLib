//
// FILE NAME: CIDKernel_CommPort_Win32.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/27/2000
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
//  This file provides the Win32 specific implementation for the class
//  TKrnlCommPort.
//
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
#include    "CIDKernel_.hpp"


// ---------------------------------------------------------------------------
//  Local, static methods
// ---------------------------------------------------------------------------
//
//  This method is called just after opening a port. It sets up the port
//  to the portable defaults that we want any opened port to be set up to,
//  no matter what platform. Otherwise, the client would have to do a
//  complete setup every time, to insure portability.
//
static tCIDLib::TBoolean bInitState(        HANDLE          hToInit
                                    , const tCIDLib::TCard4 c4ReadBufSz
                                    , const tCIDLib::TCard4 c4WriteBufSz)
{
    //
    //  Do the basic setup of buffer sizes. This can be rejected, so we
    //  don't bother checking the result. Its just advisory anyway.
    //
    const tCIDLib::TCard4 c4InQ     = c4ReadBufSz ? c4ReadBufSz : 128;
    const tCIDLib::TCard4 c4OutQ    = c4WriteBufSz ? c4WriteBufSz : 128;
    ::SetupComm(hToInit, c4InQ, c4OutQ);

    // And now set up the defaults
    DCB State = {0};
    State.DCBlength     = sizeof(State);
    State.BaudRate      = CBR_9600;
    State.fBinary       = 1;
    State.fAbortOnError = 1;
    State.ByteSize      = 8;
    State.StopBits      = ONESTOPBIT;

    if (!::SetCommState(hToInit, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Set both timeouts to never timeout
    COMMTIMEOUTS Data = {0};
    if (!::SetCommTimeouts(hToInit, &Data))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Purge any data in the buffers now
    ::PurgeComm(hToInit, PURGE_TXCLEAR | PURGE_RXCLEAR);

    return kCIDLib::True;
}

static tCIDLib::TBoolean bXlatDTR(DCB& toFill, const tCIDComm::EPortDTR eDTR)
{
    if (eDTR == tCIDComm::EPortDTR::Disable)
        toFill.fDtrControl = DTR_CONTROL_DISABLE;
    else if (eDTR == tCIDComm::EPortDTR::Enable)
        toFill.fDtrControl = DTR_CONTROL_ENABLE;
    else if (eDTR == tCIDComm::EPortDTR::Handshake)
        toFill.fDtrControl = DTR_CONTROL_HANDSHAKE;
    else
        return kCIDLib::False;

    return kCIDLib::True;
}

static tCIDLib::TBoolean bXlatFlags(        DCB&                toSet
                                    , const tCIDComm::EFlags    eToSet
                                    , const tCIDComm::EFlags    eMask)
{
    if (tCIDLib::bAllBitsOn(eMask, tCIDComm::EFlags::ParityChecking))
        toSet.fParity = tCIDLib::bAllBitsOn(eToSet, tCIDComm::EFlags::ParityChecking) ? 1 : 0;
    if (tCIDLib::bAllBitsOn(eMask, tCIDComm::EFlags::CTSOut))
        toSet.fOutxCtsFlow = tCIDLib::bAllBitsOn(eToSet, tCIDComm::EFlags::CTSOut) ? 1 : 0;
    if (tCIDLib::bAllBitsOn(eMask, tCIDComm::EFlags::DSROut))
        toSet.fOutxDsrFlow = tCIDLib::bAllBitsOn(eToSet, tCIDComm::EFlags::DSROut) ? 1 : 0;
    if (tCIDLib::bAllBitsOn(eMask, tCIDComm::EFlags::XOut))
        toSet.fOutX = tCIDLib::bAllBitsOn(eToSet, tCIDComm::EFlags::XOut) ? 1 : 0;
    if (tCIDLib::bAllBitsOn(eMask, tCIDComm::EFlags::XIn))
        toSet.fInX = tCIDLib::bAllBitsOn(eToSet, tCIDComm::EFlags::XIn) ? 1 : 0;

    return kCIDLib::True;
}

static tCIDLib::TBoolean bXlatRTS(DCB& toFill, const tCIDComm::EPortRTS eRTS)
{
    if (eRTS == tCIDComm::EPortRTS::Disable)
        toFill.fRtsControl = RTS_CONTROL_DISABLE;
    else if (eRTS == tCIDComm::EPortRTS::Enable)
        toFill.fRtsControl = RTS_CONTROL_ENABLE;
    else if (eRTS == tCIDComm::EPortRTS::Handshake)
        toFill.fRtsControl = RTS_CONTROL_HANDSHAKE;
    else if (eRTS == tCIDComm::EPortRTS::Toggle)
        toFill.fRtsControl = RTS_CONTROL_TOGGLE;
    else
        return kCIDLib::False;

    return kCIDLib::True;
}

static tCIDComm::EPortDTR eXlatDTR(const DCB& srcData)
{
    tCIDComm::EPortDTR eRet;

    if (srcData.fDtrControl == DTR_CONTROL_DISABLE)
        eRet = tCIDComm::EPortDTR::Disable;
    else if (srcData.fDtrControl == DTR_CONTROL_ENABLE)
        eRet = tCIDComm::EPortDTR::Enable;
    else if (srcData.fDtrControl == DTR_CONTROL_HANDSHAKE)
        eRet = tCIDComm::EPortDTR::Handshake;
    else
        eRet = tCIDComm::EPortDTR::Unknown;

    return eRet;
}

static tCIDComm::EErrors eXlatErr(const tCIDLib::TCard4 c4ToXlat)
{
    tCIDComm::EErrors eRet = tCIDComm::EErrors::None;

    if (c4ToXlat & CE_BREAK)
        eRet = tCIDLib::eOREnumBits(eRet, tCIDComm::EErrors::Break);
    if (c4ToXlat & CE_FRAME)
        eRet = tCIDLib::eOREnumBits(eRet, tCIDComm::EErrors::Frame);
    if (c4ToXlat & CE_IOE)
        eRet = tCIDLib::eOREnumBits(eRet, tCIDComm::EErrors::Error);
    if (c4ToXlat & CE_MODE)
        eRet = tCIDLib::eOREnumBits(eRet, tCIDComm::EErrors::BadMode);
    if (c4ToXlat & CE_OVERRUN)
        eRet = tCIDLib::eOREnumBits(eRet, tCIDComm::EErrors::Overrun);
    if (c4ToXlat & CE_RXOVER)
        eRet = tCIDLib::eOREnumBits(eRet, tCIDComm::EErrors::RXOver);
    if (c4ToXlat & CE_RXPARITY)
        eRet = tCIDLib::eOREnumBits(eRet, tCIDComm::EErrors::Parity);
    if (c4ToXlat & CE_TXFULL)
        eRet = tCIDLib::eOREnumBits(eRet, tCIDComm::EErrors::TXFull);

    // If any unknown bits set, add 'other'
    if ((c4ToXlat & ~tCIDLib::c4EnumOrd(tCIDComm::EErrors::AllBits)) != 0)
        eRet = tCIDLib::eOREnumBits(eRet, tCIDComm::EErrors::Other);

    return eRet;
}

static tCIDComm::EFlags eXlatFlags(const DCB& srcData)
{
    // Build up the flags
    tCIDComm::EFlags eFlags = tCIDComm::EFlags::None;

    if (srcData.fParity)
        eFlags = tCIDLib::eOREnumBits(eFlags, tCIDComm::EFlags::ParityChecking);
    if (srcData.fOutxCtsFlow)
        eFlags = tCIDLib::eOREnumBits(eFlags, tCIDComm::EFlags::CTSOut);
    if (srcData.fOutxDsrFlow)
        eFlags = tCIDLib::eOREnumBits(eFlags, tCIDComm::EFlags::DSROut);
    if (srcData.fOutX)
        eFlags = tCIDLib::eOREnumBits(eFlags, tCIDComm::EFlags::XOut);
    if (srcData.fInX)
        eFlags = tCIDLib::eOREnumBits(eFlags, tCIDComm::EFlags::XIn);

    return eFlags;
}

static tCIDComm::EPortRTS eXlatRTS(const DCB& srcData)
{
    tCIDComm::EPortRTS eRet;

    if (srcData.fRtsControl == RTS_CONTROL_DISABLE)
        eRet = tCIDComm::EPortRTS::Disable;
    else if (srcData.fRtsControl == RTS_CONTROL_ENABLE)
        eRet = tCIDComm::EPortRTS::Enable;
    else if (srcData.fRtsControl == RTS_CONTROL_HANDSHAKE)
        eRet = tCIDComm::EPortRTS::Handshake;
    else if (srcData.fRtsControl == RTS_CONTROL_TOGGLE)
        eRet = tCIDComm::EPortRTS::Toggle;
    else
        eRet = tCIDComm::EPortRTS::Unknown;

    return eRet;
}



// ---------------------------------------------------------------------------
//   CLASS: TCommHandle
//  PREFIX: hcomm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCommHandle: Constructors and Destructor
// ---------------------------------------------------------------------------
TCommHandle::TCommHandle() :

    m_phcommiThis(nullptr)
{
    m_phcommiThis = new TCommHandleImpl;
    m_phcommiThis->hComm = 0;
}

TCommHandle::TCommHandle(const TCommHandle& hsemSrc) :

    m_phcommiThis(nullptr)
{
    m_phcommiThis = new TCommHandleImpl;
    m_phcommiThis->hComm = hsemSrc.m_phcommiThis->hComm;
}

TCommHandle::TCommHandle(TCommHandle&& hsemSrc) :

    m_phcommiThis(nullptr)
{
    m_phcommiThis = new TCommHandleImpl;
    m_phcommiThis->hComm = 0;
    *this = tCIDLib::ForceMove(hsemSrc);
}

TCommHandle::~TCommHandle()
{
    delete m_phcommiThis;
    m_phcommiThis = nullptr;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TCommHandle& TCommHandle::operator=(const TCommHandle& hsemSrc)
{
    if (this != &hsemSrc)
        m_phcommiThis->hComm = hsemSrc.m_phcommiThis->hComm;

    return *this;
}


TCommHandle& TCommHandle::operator=(TCommHandle&& hsemSrc)
{
    if (this != &hsemSrc)
        tCIDLib::Swap(m_phcommiThis, hsemSrc.m_phcommiThis);
    return *this;
}


tCIDLib::TBoolean
TCommHandle::operator==(const TCommHandle& hsemSrc) const
{
    return (m_phcommiThis->hComm == hsemSrc.m_phcommiThis->hComm);
}


// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TBoolean TCommHandle::bIsValid() const
{
    return (m_phcommiThis->hComm != 0);
}

tCIDLib::TVoid TCommHandle::Clear()
{
    m_phcommiThis->hComm = 0;
}

tCIDLib::TVoid
TCommHandle::FormatToStr(       tCIDLib::TCh* const pszToFill
                        , const tCIDLib::TCard4     c4MaxChars) const
{
    TRawStr::bFormatVal
    (
        tCIDLib::TCard4(m_phcommiThis->hComm)
        , pszToFill
        , c4MaxChars
        , tCIDLib::ERadices::Hex
    );
}




// ---------------------------------------------------------------------------
//   CLASS: TKrnlCommPort
//  PREFIX: kcomm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TKrnlCommPort: An opaque type for the per-platform data. We store the
//  last timeout values used. Win32 sets timeouts persistently, so we have
//  to fake the per-call timeouts that CIDComm supports. We use these to
//  only have to update them when they actually change.
// ---------------------------------------------------------------------------
struct TKrnlCommPort::TPerPlat
{
    tCIDLib::TCard4     c4ReadTimeout;
    tCIDLib::TCard4     c4WriteTimeout;
};


// ---------------------------------------------------------------------------
//  TKrnlCommPort: Public, static methods
// ---------------------------------------------------------------------------

//
//  The caller passes us an array of booleans and a count of values in the
//  array. We scan for many ports and set the slot in the array for each
//  port we find.
//
tCIDLib::TCard4
TKrnlCommPort::c4EnumPorts(         tCIDLib::TBoolean* const pbToFill
                            , const tCIDLib::TCard4          c4Count)
{
    tCIDLib::TCard4 c4Ret = 0;

    // We'll just keep appending a new number to the end of this prefix
    tCIDLib::TCh szSrcBuf[16];
    TRawStr::CopyStr(szSrcBuf, L"COM");

    constexpr tCIDLib::TCard4 c4BufSz = 1024;
    tCIDLib::TCh szOutBuf[c4BufSz + 1];
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        TRawStr::bFormatVal(c4Index + 1, &szSrcBuf[3], 4);
        pbToFill[c4Index] = ::QueryDosDevice(szSrcBuf, szOutBuf, c4BufSz) != 0;
    }
    return c4Ret;
}


// ---------------------------------------------------------------------------
//  TKrnlCommPort: Constructors and Destructor
// ---------------------------------------------------------------------------
TKrnlCommPort::TKrnlCommPort() :

    m_c4PortNum(0)
    , m_pExtra(nullptr)
{
    m_pExtra = new TPerPlat;
    m_pExtra->c4ReadTimeout = kCIDLib::c4MaxCard;
    m_pExtra->c4WriteTimeout = kCIDLib::c4MaxCard;
}

TKrnlCommPort::TKrnlCommPort(const tCIDLib::TCard4 c4PortNum) :

    m_c4PortNum(c4PortNum)
    , m_pExtra(nullptr)
{
    m_pExtra = new TPerPlat;
    m_pExtra->c4ReadTimeout = kCIDLib::c4MaxCard;
    m_pExtra->c4WriteTimeout = kCIDLib::c4MaxCard;
}

TKrnlCommPort::TKrnlCommPort(TKrnlCommPort&& kcommSrc) :

    TKrnlCommPort()
{
    *this = tCIDLib::ForceMove(kcommSrc);
}

TKrnlCommPort::~TKrnlCommPort()
{
    if (m_hcommThis.m_phcommiThis->hComm)
        ::CloseHandle(m_hcommThis.m_phcommiThis->hComm);

    // Clean up our extra data
    delete m_pExtra;
}


// ---------------------------------------------------------------------------
//  TKrnlCommPort: Public operators
// ---------------------------------------------------------------------------
TKrnlCommPort& TKrnlCommPort::operator=(TKrnlCommPort&& kcommSrc)
{
    if (this != &kcommSrc)
    {
        tCIDLib::Swap(m_c4PortNum, kcommSrc.m_c4PortNum);
        tCIDLib::Swap(m_pExtra, kcommSrc.m_pExtra);
        m_hcommThis = tCIDLib::ForceMove(kcommSrc.m_hcommThis);
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TKrnlCommPort: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlCommPort::bClose()
{
    // Reset the timeout settings for next time
    m_pExtra->c4ReadTimeout = kCIDLib::c4MaxCard;
    m_pExtra->c4WriteTimeout = kCIDLib::c4MaxCard;

    // If open, then try to close it
    if (m_hcommThis.m_phcommiThis->hComm)
    {
        if (!::CloseHandle(m_hcommThis.m_phcommiThis->hComm))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_CloseHandle, ::GetLastError());
            return kCIDLib::False;
        }
        m_hcommThis.m_phcommiThis->hComm = 0;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlCommPort::bCycle(  const   tCIDComm::EOpenFlags    eFlags
                        , const tCIDLib::TCard4         c4ReadBufSz
                        , const tCIDLib::TCard4         c4WriteBufSz)
{
    // Save the current configuration
    DCB State;
    State.DCBlength = sizeof(State);
    if (!::GetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_GetCommState, ::GetLastError());
        return kCIDLib::False;
    }

    // Close the port and re-open it
    ::CloseHandle(m_hcommThis.m_phcommiThis->hComm);
    m_hcommThis.m_phcommiThis->hComm = 0;

    m_hcommThis.m_phcommiThis->hComm = 0;
    if (!bOpen(eFlags, c4ReadBufSz, c4WriteBufSz))
        return kCIDLib::False;

    // Set the original config back
    if (!::SetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_SetCommState, ::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlCommPort::bFlushWriteBuffer()
{
    if (!::FlushFileBuffers(m_hcommThis.m_phcommiThis->hComm))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlCommPort::bIsOpen() const
{
    return (m_hcommThis.m_phcommiThis->hComm != 0);
}


tCIDLib::TBoolean
TKrnlCommPort::bOpen(const  tCIDComm::EOpenFlags    eFlags
                    , const tCIDLib::TCard4         c4ReadBufSz
                    , const tCIDLib::TCard4         c4WriteBufSz)
{
    // Make sure its not already open
    if (m_hcommThis.m_phcommiThis->hComm)
    {
        TKrnlError::SetLastError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    #if CID_DEBUG_ON
    if (!m_c4PortNum || (m_c4PortNum > 255))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_BadPortNum);
        return kCIDLib::False;
    }
    #endif

    // Translate the flags
    tCIDLib::TCard4 c4ExtraFlags = 0;
    if (tCIDLib::bAllBitsOn(eFlags, tCIDComm::EOpenFlags::WriteThrough))
        c4ExtraFlags |= FILE_FLAG_WRITE_THROUGH;

    // Create the local port path
    tCIDLib::TCh achNum[9];
    if (!TRawStr::bFormatVal(m_c4PortNum, achNum, 8))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_BadPortNum);
        return kCIDLib::False;
    }

    tCIDLib::TCh achPath[17];
    TRawStr::CopyStr(achPath, L"\\\\.\\COM", 16);
    TRawStr::CatStr(achPath, achNum, 16);

    // Try to create it into a temp handle
    HANDLE hTmp = ::CreateFile
    (
        achPath
        , GENERIC_READ | GENERIC_WRITE
        , 0
        , 0
        , OPEN_EXISTING
        , FILE_ATTRIBUTE_NORMAL | c4ExtraFlags
        , 0
    );

    // If it failed, store the last platform error and return failure
    if (hTmp == INVALID_HANDLE_VALUE)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcGen_OpenFailed, ::GetLastError());
        return kCIDLib::False;
    }

    // Initialize the port to the portable default settings
    if (!bInitState(hTmp, c4ReadBufSz, c4WriteBufSz))
        return kCIDLib::False;

    // It worked so store the handle and return success
    m_hcommThis.m_phcommiThis->hComm = hTmp;
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlCommPort::bPurgeReadData()
{
    if (!::PurgeComm(m_hcommThis.m_phcommiThis->hComm, PURGE_RXCLEAR))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlCommPort::bPurgeWriteData()
{
    if (!::PurgeComm(m_hcommThis.m_phcommiThis->hComm, PURGE_TXCLEAR))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlCommPort::bQueryCfg(   tCIDLib::TCard4&        c4Baud
                            , tCIDComm::EParities&  eParity
                            , tCIDComm::EDataBits&  eDataBits
                            , tCIDComm::EStopBits&  eStopBits
                            , tCIDComm::EFlags&     eFlags
                            , tCIDComm::EPortDTR&   eDTRFlow
                            , tCIDComm::EPortRTS&   eRTSFlow) const
{
    DCB State;
    State.DCBlength = sizeof(State);

    // Get the current state, then update it and put it back
    if (!::GetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_GetCommState, ::GetLastError());
        return kCIDLib::False;
    }

    // Get the data out into our format
    c4Baud      = State.BaudRate;
    eParity     = tCIDComm::EParities(State.Parity);
    eStopBits   = tCIDComm::EStopBits(State.StopBits);
    eDataBits   = tCIDComm::EDataBits(State.ByteSize);

    // Translate the other stuff into our portable representation
    eFlags      = eXlatFlags(State);
    eDTRFlow    = eXlatDTR(State);
    eRTSFlow    = eXlatRTS(State);

    if (eDTRFlow == tCIDComm::EPortDTR::Unknown)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_BadDTR);
        return kCIDLib::False;
    }

    if (eRTSFlow == tCIDComm::EPortRTS::Unknown)
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_BadRTS);
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlCommPort::bQueryCTSOn(tCIDLib::TBoolean& bToFill) const
{
    DWORD Flags;
    if (!::GetCommModemStatus(m_hcommThis.m_phcommiThis->hComm, &Flags))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_GetLineState, ::GetLastError());
        return kCIDLib::False;
    }

    bToFill = (Flags & MS_CTS_ON) != 0;
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlCommPort::bQueryDSROn(tCIDLib::TBoolean& bToFill) const
{
    DWORD Flags;
    if (!::GetCommModemStatus(m_hcommThis.m_phcommiThis->hComm, &Flags))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_GetLineState, ::GetLastError());
        return kCIDLib::False;
    }

    bToFill = (Flags & MS_DSR_ON) != 0;
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlCommPort::bQueryFlags(tCIDComm::EFlags& eFlags) const
{
    DCB State;
    State.DCBlength = sizeof(State);

    // Get the current state
    if (!::GetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_GetLineState, ::GetLastError());
        return kCIDLib::False;
    }

    // Build up the flags and store them
    eFlags = eXlatFlags(State);
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlCommPort::bQueryLineStates(tCIDLib::TBoolean&      bCTSOn
                                , tCIDLib::TBoolean&    bDSROn
                                , tCIDLib::TBoolean&    bRingOn
                                , tCIDLib::TBoolean&    bRLSDOn) const
{
    DWORD Flags;
    if (!::GetCommModemStatus(m_hcommThis.m_phcommiThis->hComm, &Flags))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_GetLineState, ::GetLastError());
        return kCIDLib::False;
    }

    bCTSOn  = (Flags & MS_CTS_ON) != 0;
    bDSROn  = (Flags & MS_DSR_ON) != 0;
    bRingOn = (Flags & MS_RING_ON) != 0;
    bRLSDOn = (Flags & MS_RLSD_ON) != 0;
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlCommPort::bQueryXChars(tCIDLib::TSCh& schOn, tCIDLib::TSCh& schOff) const
{
    DCB State;
    State.DCBlength = sizeof(State);

    // Get the current state
    if (!::GetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // Build up the flags and store them
    schOn = State.XonChar;
    schOff = State.XoffChar;

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlCommPort::bReadRawBuf(         tCIDLib::TVoid* const   pToFill
                            , const tCIDLib::TCard4         c4MaxBytes
                            ,       tCIDLib::TCard4&        c4BytesRead
                            ,       tCIDComm::EErrors&      eErrors
                            , const tCIDLib::TEncodedTime   enctWait)
{
    eErrors = tCIDComm::EErrors::None;

    // Make sure no outstanding errors
    tCIDLib::TCard4 c4Err;
    if (!::ClearCommError(m_hcommThis.m_phcommiThis->hComm, &c4Err, 0))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_ClearCommErr, ::GetLastError());
        return kCIDLib::False;
    }

    // If the timeout is different from last time, then update it.
    const tCIDLib::TCard4 c4Timeout = tCIDLib::TCard4(enctWait / kCIDLib::enctOneMilliSec);
    if (c4Timeout != m_pExtra->c4ReadTimeout)
    {
        COMMTIMEOUTS Data;
        if (!::GetCommTimeouts(m_hcommThis.m_phcommiThis->hComm, &Data))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_GetTimeouts, ::GetLastError());
            return kCIDLib::False;
        }

        if (c4Timeout == kCIDLib::c4MaxCard)
        {
            // This tells Win32 not to ever timeout
            Data.ReadTotalTimeoutConstant = 0;
            Data.ReadTotalTimeoutMultiplier = 0;
            Data.ReadIntervalTimeout = 0;
        }
         else if (!c4Timeout)
        {
            // This tells Win32 to read what's there and return immediately
            Data.ReadTotalTimeoutConstant = 0;
            Data.ReadTotalTimeoutMultiplier = 0;
            Data.ReadIntervalTimeout = kCIDLib::c4MaxCard;
        }
         else
        {
            //
            //  Use their total timeout, but add a wee interval timeout behind
            //  their back, so as to accomodate bigger data.
            //
            Data.ReadTotalTimeoutMultiplier = 0;
            Data.ReadIntervalTimeout = 0;
            Data.ReadTotalTimeoutConstant = c4Timeout;
        }

        if (!::SetCommTimeouts(m_hcommThis.m_phcommiThis->hComm, &Data))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_SetTimeouts, ::GetLastError());
            return kCIDLib::False;
        }

        // And remember this as the current read timeout
        m_pExtra->c4ReadTimeout = c4Timeout;
    }

    if (!::ReadFile(m_hcommThis.m_phcommiThis->hComm, pToFill, c4MaxBytes, &c4BytesRead, 0))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_Read, ::GetLastError());

        if (::ClearCommError(m_hcommThis.m_phcommiThis->hComm, &c4Err, 0))
            eErrors = eXlatErr(c4Err);
        else
            eErrors = tCIDComm::EErrors::Unknown;

        ::PurgeComm(m_hcommThis.m_phcommiThis->hComm, PURGE_RXCLEAR);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Sets the passed baud rate on this port
tCIDLib::TBoolean TKrnlCommPort::bSetBaudRate(const tCIDLib::TCard4 c4ToSet)
{
    DCB State;
    State.DCBlength = sizeof(State);

    // Get the current state, then update it and put it back
    if (!::GetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    State.BaudRate = c4ToSet;
    if (!::SetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


// Set or clear the break state on the port
tCIDLib::TBoolean TKrnlCommPort::bSetBreak(const tCIDLib::TBoolean bState)
{
    tCIDLib::TBoolean bRes;
    if (bState)
        bRes = (::SetCommBreak(m_hcommThis.m_phcommiThis->hComm) != 0);
    else
        bRes = (::ClearCommBreak(m_hcommThis.m_phcommiThis->hComm) != 0);

    if (!bRes)
        TKrnlError::SetLastHostError(::GetLastError());
    return bRes;
}


// Sets up this port with the passed configuration settings
tCIDLib::TBoolean
TKrnlCommPort::bSetCfg( const   tCIDLib::TCard4         c4Baud
                        , const tCIDComm::EParities     eParity
                        , const tCIDComm::EDataBits     eDataBits
                        , const tCIDComm::EStopBits     eStopBits
                        , const tCIDComm::EFlags        eFlags
                        , const tCIDComm::EPortDTR      eDTRFlow
                        , const tCIDComm::EPortRTS      eRTSFlow)
{
    DCB State;
    State.DCBlength = sizeof(State);

    // Get the current state, then update it and put it back
    if (!::GetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    // These just work as is
    State.BaudRate  = c4Baud;
    State.Parity    = tCIDLib::TCard1(eParity);
    State.StopBits  = tCIDLib::TCard1(eStopBits);
    State.ByteSize  = tCIDLib::TCard1(eDataBits);

    // And these require translation
    if (!bXlatFlags(State, eFlags, tCIDComm::EFlags::AllBits))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_BadFlags);
        return kCIDLib::False;
    }

    if (!bXlatDTR(State, eDTRFlow))
    {
        TKrnlError::SetLastError(kKrnlErrs::errcComm_BadDTR);
        return kCIDLib::False;
    }

    if (!bXlatRTS(State, eRTSFlow))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_BadRTS);
        return kCIDLib::False;
    }

    // And try to set them back on the port
    if (!::SetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlCommPort::bSetDataBits(const tCIDComm::EDataBits eToSet)
{
    DCB State;
    State.DCBlength = sizeof(State);

    // Get the current state, then update it and put it back
    if (!::GetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    State.ByteSize = tCIDLib::TCard1(eToSet);

    if (!::SetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlCommPort::bSetDefCfg()
{
    return bInitState(m_hcommThis.m_phcommiThis->hComm, 0, 0);
}


tCIDLib::TBoolean TKrnlCommPort::bSetDTR(const tCIDComm::EPortDTR eToSet)
{
    DCB State;
    State.DCBlength = sizeof(State);

    // Get the current state, then update it and put it back
    if (!::GetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    if (!bXlatDTR(State, eToSet))
    {
        TKrnlError::SetLastError(kKrnlErrs::errcComm_BadDTR);
        return kCIDLib::False;
    }

    if (!::SetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlCommPort::bSetEOFChar(const tCIDLib::TSCh schToSet)
{
    DCB State;
    State.DCBlength = sizeof(State);

    // Get the current state, then update it and put it back
    if (!::GetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    State.EofChar = schToSet;

    if (!::SetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlCommPort::bSetFlags(const  tCIDComm::EFlags eToSet
                        , const tCIDComm::EFlags eMask)
{
    DCB State;
    State.DCBlength = sizeof(State);

    // Get the current state, then update it and put it back
    if (!::GetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    if (!bXlatFlags(State, eToSet, eMask))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_BadFlags);
        return kCIDLib::False;
    }

    if (!::SetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlCommPort::bSetParity(const tCIDComm::EParities eToSet)
{
    DCB State;
    State.DCBlength = sizeof(State);

    // Get the current state, then update it and put it back
    if (!::GetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    State.fParity = tCIDLib::TCard1(eToSet);

    if (!::SetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlCommPort::bSetPort(const tCIDLib::TCard4 c4PortNum)
{
    // Make sure its not open, since this is not legal then
    if (m_hcommThis.m_phcommiThis->hComm)
    {
        TKrnlError::SetLastError(kKrnlErrs::errcGen_AlreadyOpen);
        return kCIDLib::False;
    }

    m_c4PortNum = c4PortNum;
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlCommPort::bSetRTS(const tCIDComm::EPortRTS eToSet)
{
    DCB State;
    State.DCBlength = sizeof(State);

    // Get the current state, then update it and put it back
    if (!::GetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    if (!bXlatRTS(State, eToSet))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_BadRTS);
        return kCIDLib::False;
    }

    if (!::SetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlCommPort::bSetStopBits(const tCIDComm::EStopBits eToSet)
{
    DCB State;
    State.DCBlength = sizeof(State);

    // Get the current state, then update it and put it back
    if (!::GetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }

    State.StopBits = tCIDLib::TCard1(eToSet);

    if (!::SetCommState(m_hcommThis.m_phcommiThis->hComm, &State))
    {
        TKrnlError::SetLastHostError(::GetLastError());
        return kCIDLib::False;
    }
    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlCommPort::bWriteRawBuf(const   tCIDLib::TVoid* const   pToWrite
                            , const tCIDLib::TCard4         c4BytesToWrite
                            ,       tCIDLib::TCard4&        c4BytesWritten
                            ,       tCIDComm::EErrors&      eErrors
                            , const tCIDLib::TEncodedTime   enctWait)
{
    eErrors = tCIDComm::EErrors::None;

    // Make sure no outstanding errors
    tCIDLib::TCard4 c4Err;
    if (!::ClearCommError(m_hcommThis.m_phcommiThis->hComm, &c4Err, 0))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_ClearCommErr, ::GetLastError());
        return kCIDLib::False;
    }

    // If the timeout is different from last time, then update it.
    const tCIDLib::TCard4 c4Timeout = tCIDLib::TCard4(enctWait / kCIDLib::enctOneMilliSec);
    if (c4Timeout != m_pExtra->c4WriteTimeout)
    {
        COMMTIMEOUTS Data;
        if (!::GetCommTimeouts(m_hcommThis.m_phcommiThis->hComm, &Data))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_GetTimeouts, ::GetLastError());
            return kCIDLib::False;
        }

        if (c4Timeout == kCIDLib::c4MaxCard)
        {
            // This tells Win32 not to ever timeout
            Data.WriteTotalTimeoutConstant = 0;
            Data.WriteTotalTimeoutMultiplier = 0;
        }
         else if (!c4Timeout)
        {
            // This tells Win32 not to read what's there and return immediately
            Data.WriteTotalTimeoutMultiplier = 0;
            Data.WriteTotalTimeoutConstant = 0;
        }
         else
        {
            //
            //  Use their total timeout, but add a wee interval timeout behind
            //  their back, so as to accomodate bigger data.
            //
            Data.WriteTotalTimeoutMultiplier = 5;
            Data.WriteTotalTimeoutConstant = c4Timeout;
        }

        if (!::SetCommTimeouts(m_hcommThis.m_phcommiThis->hComm, &Data))
        {
            TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_SetTimeouts, ::GetLastError());
            return kCIDLib::False;
        }

        // And remember this as the current write timeout
        m_pExtra->c4WriteTimeout = c4Timeout;
    }

    if (!::WriteFile(m_hcommThis.m_phcommiThis->hComm, pToWrite, c4BytesToWrite, &c4BytesWritten, 0))
    {
        TKrnlError::SetLastKrnlError(kKrnlErrs::errcComm_Write, ::GetLastError());

        if (::ClearCommError(m_hcommThis.m_phcommiThis->hComm, &c4Err, 0))
            eErrors = eXlatErr(c4Err);
        else
            eErrors = tCIDComm::EErrors::Unknown;

        ::PurgeComm(m_hcommThis.m_phcommiThis->hComm, PURGE_TXCLEAR);
        return kCIDLib::False;
    }
    return kCIDLib::True;
}

