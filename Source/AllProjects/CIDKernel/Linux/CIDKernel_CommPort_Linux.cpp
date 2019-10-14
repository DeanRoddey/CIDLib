//
// FILE NAME: CIDKernel_CommPort_Linux.Cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/12/2019
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
//  This file provides the Linux specific implementation for the class
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
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;

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
    m_phcommiThis->hComm = nullptr;
}

TCommHandle::TCommHandle(const TCommHandle& hsemToCopy) :

    m_phcommiThis(nullptr)
{
    m_phcommiThis = new TCommHandleImpl;
    m_phcommiThis->hComm = hsemToCopy.m_phcommiThis->hComm;
}

TCommHandle::~TCommHandle()
{
    delete m_phcommiThis;
    m_phcommiThis = nullptr;
}


// -------------------------------------------------------------------
//  Public operators
// -------------------------------------------------------------------
TCommHandle& TCommHandle::operator=(const TCommHandle& hsemToAssign)
{
    if (this != &hsemToAssign)
        m_phcommiThis->hComm = hsemToAssign.m_phcommiThis->hComm;
    return *this;
}


tCIDLib::TBoolean
TCommHandle::operator==(const TCommHandle& hsemToCompare) const
{
    return (m_phcommiThis->hComm == hsemToCompare.m_phcommiThis->hComm);
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
//  TKrnlCommPort: An opaque type for any per-platform data we need to store for each
//  created port object.
// ---------------------------------------------------------------------------
struct TKrnlCommPort::TPerPlat
{
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
}

TKrnlCommPort::TKrnlCommPort(const tCIDLib::TCard4 c4PortNum) :

    m_c4PortNum(c4PortNum)
    , m_pExtra(nullptr)
{
    m_pExtra = new TPerPlat;
}

TKrnlCommPort::~TKrnlCommPort()
{
    // Clean up our extra data
    delete m_pExtra;

    // Close our port handle if it got opened

}


// ---------------------------------------------------------------------------
//  TKrnlCommPort: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TKrnlCommPort::bClose()
{
    // Reset the the extra data for next time
    m_pExtra->c4ReadTimeout = kCIDLib::c4MaxCard;

    // If open, then try to close it


    return kCIDLib::True;
}


tCIDLib::TBoolean
TKrnlCommPort::bCycle(  const   tCIDComm::EOpenFlags    eFlags
                        , const tCIDLib::TCard4         c4ReadBufSz
                        , const tCIDLib::TCard4         c4WriteBufSz)
{
    // Save the current configuration

    // Close the port and re-open it

    if (!bOpen(eFlags, c4ReadBufSz, c4WriteBufSz))
        return kCIDLib::False;

    // Set the original config back

    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlCommPort::bFlushWriteBuffer()
{
    return kCIDLib::True;
}


tCIDLib::TBoolean TKrnlCommPort::bIsOpen() const
{
    return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlCommPort::bOpen(const  tCIDComm::EOpenFlags    eFlags
                    , const tCIDLib::TCard4         c4ReadBufSz
                    , const tCIDLib::TCard4         c4WriteBufSz)
{

    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlCommPort::bPurgeReadData()
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlCommPort::bPurgeWriteData()
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
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
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlCommPort::bQueryCTSOn(tCIDLib::TBoolean& bToFill) const
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlCommPort::bQueryDSROn(tCIDLib::TBoolean& bToFill) const
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlCommPort::bQueryFlags(tCIDComm::EFlags& eFlags) const
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlCommPort::bQueryLineStates(tCIDLib::TBoolean&      bCTSOn
                                , tCIDLib::TBoolean&    bDSROn
                                , tCIDLib::TBoolean&    bRingOn
                                , tCIDLib::TBoolean&    bRLSDOn) const
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlCommPort::bQueryXChars(tCIDLib::TSCh& schOn, tCIDLib::TSCh& schOff) const
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlCommPort::bReadRawBuf(         tCIDLib::TVoid* const   pToFill
                            , const tCIDLib::TCard4         c4MaxBytes
                            ,       tCIDLib::TCard4&        c4BytesRead
                            ,       tCIDComm::EErrors&      eErrors
                            , const tCIDLib::TEncodedTime   enctWait)
{
    eErrors = tCIDComm::EErrors::None;
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


// Sets the passed baud rate on this port
tCIDLib::TBoolean TKrnlCommPort::bSetBaudRate(const tCIDLib::TCard4 c4ToSet)
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


// Set or clear the break state on the port
tCIDLib::TBoolean TKrnlCommPort::bSetBreak(const tCIDLib::TBoolean bState)
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
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
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlCommPort::bSetDataBits(const tCIDComm::EDataBits eToSet)
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlCommPort::bSetDefCfg()
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlCommPort::bSetDTR(const tCIDComm::EPortDTR eToSet)
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlCommPort::bSetEOFChar(const tCIDLib::TSCh schToSet)
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlCommPort::bSetFlags(const  tCIDComm::EFlags eToSet
                        , const tCIDComm::EFlags eMask)
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlCommPort::bSetParity(const tCIDComm::EParities eToSet)
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlCommPort::bSetPort(const tCIDLib::TCard4 c4PortNum)
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlCommPort::bSetRTS(const tCIDComm::EPortRTS eToSet)
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean TKrnlCommPort::bSetStopBits(const tCIDComm::EStopBits eToSet)
{
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}


tCIDLib::TBoolean
TKrnlCommPort::bWriteRawBuf(const   tCIDLib::TVoid* const   pToWrite
                            , const tCIDLib::TCard4         c4BytesToWrite
                            ,       tCIDLib::TCard4&        c4BytesWritten
                            ,       tCIDComm::EErrors&      eErrors
                            , const tCIDLib::TEncodedTime   enctWait)
{
    eErrors = tCIDComm::EErrors::None;
    TKrnlError::SetLastError(kKrnlErrs::errcGen_NotSupported);
    return kCIDLib::False;
}

