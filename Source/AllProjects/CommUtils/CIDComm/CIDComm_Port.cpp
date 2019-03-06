//
// FILE NAME: CIDComm_Port.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/28/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TCommPort class.
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
#include    "CIDComm_.hpp"


// ---------------------------------------------------------------------------
//  Magic RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCommPort,TCommPortBase)



// ---------------------------------------------------------------------------
//  CLASS: TCommPort
// PREFIX: comm
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCommPort: Constructors and Destructor
// ---------------------------------------------------------------------------
TCommPort::TCommPort()
{
}

TCommPort::TCommPort(const TString& strPath, const tCIDLib::TCard4 c4PortNum) :

    TCommPortBase(strPath)
    , m_kcommThis(c4PortNum)
{
}

TCommPort::~TCommPort()
{
}


// ---------------------------------------------------------------------------
//  TCommPort: Public, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCommPort::bIsOpen() const
{
    return m_kcommThis.bIsOpen();
}


tCIDLib::TBoolean TCommPort::bQueryCTSOn() const
{
    tCIDLib::TBoolean   bCTSOn;
    tCIDLib::TBoolean   bDSROn;
    tCIDLib::TBoolean   bRingOn;
    tCIDLib::TBoolean   bRLSDOn;
    if (!m_kcommThis.bQueryLineStates(bCTSOn, bDSROn, bRingOn, bRLSDOn))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_QueryLineState
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
    return bCTSOn;
}


tCIDLib::TBoolean TCommPort::bQueryDSROn() const
{
    tCIDLib::TBoolean   bCTSOn;
    tCIDLib::TBoolean   bDSROn;
    tCIDLib::TBoolean   bRingOn;
    tCIDLib::TBoolean   bRLSDOn;
    if (!m_kcommThis.bQueryLineStates(bCTSOn, bDSROn, bRingOn, bRLSDOn))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_QueryLineState
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
    return bDSROn;
}


tCIDLib::TCard4
TCommPort::c4ReadRawBuf(        tCIDLib::TVoid* const   pToFill
                        , const tCIDLib::TCard4         c4MaxBytes
                        , const tCIDLib::TEncodedTime   enctWait)
{
    tCIDLib::TBoolean       bRes;
    tCIDLib::TCard4         c4ThisTime;
    tCIDLib::TCard4         c4Read = 0;
    tCIDComm::EErrors       eErrors;
    tCIDLib::TCard1*        pc1Out = reinterpret_cast<tCIDLib::TCard1*>(pToFill);
    TThread*                pthrCaller = TThread::pthrCaller();

    tCIDLib::TEncodedTime   enctCur = TTime::enctNow();
    tCIDLib::TEncodedTime   enctCurWait;
    tCIDLib::TEncodedTime   enctEnd;
    if (enctWait == kCIDLib::enctMaxWait)
        enctEnd = kCIDLib::enctMaxWait;
    else
        enctEnd = enctCur + enctWait;

    while ((enctCur < enctEnd) && (c4Read < c4MaxBytes))
    {
        // Wait for up to half a second each time around
        if (enctCur + kCIDLib::enctHalfSecond > enctEnd)
            enctCurWait = enctEnd - enctCur;
        else
            enctCurWait = kCIDLib::enctHalfSecond;

        bRes = m_kcommThis.bReadRawBuf
        (
            &pc1Out[c4Read], c4MaxBytes - c4Read, c4ThisTime, eErrors, enctCurWait
        );

        if (!bRes)
        {
            TTextStringOutStream strTmp(64);
            strTmp << eErrors << kCIDLib::FlushIt;
            facCIDComm().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kCommErrs::errcPort_ReadData
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::LostConnection
                , strPath()
                , strTmp.strData()
            );
        }

        // Add the amount we got this time to the total
        c4Read += c4ThisTime;

        // Check for shutdown request
        if (pthrCaller->bCheckShutdownRequest())
            break;

        enctCur = TTime::enctNow();
    }
    return c4Read;
}

tCIDLib::TCard4
TCommPort::c4ReadRawBufMS(          tCIDLib::TVoid* const   pToFill
                            , const tCIDLib::TCard4         c4MaxBytes
                            , const tCIDLib::TCard4         c4WaitMS)
{
    const tCIDLib::TEncodedTime enctWait(c4WaitMS * kCIDLib::enctOneMilliSec);
    return c4ReadRawBuf(pToFill, c4MaxBytes, enctWait);
}


tCIDLib::TCard4
TCommPort::c4WriteRawBufMS( const   tCIDLib::TVoid* const   pToWrite
                            , const tCIDLib::TCard4         c4BytesToWrite
                            , const tCIDLib::TCard4         c4WaitMS)
{
    const tCIDLib::TEncodedTime enctWait(c4WaitMS * kCIDLib::enctOneMilliSec);

    tCIDLib::TCard4     c4Written = 0;
    tCIDComm::EErrors   eErrors;
    if (!m_kcommThis.bWriteRawBuf(pToWrite, c4BytesToWrite, c4Written, eErrors, enctWait))
    {
        TTextStringOutStream strmTmp(64);
        strmTmp << eErrors << kCIDLib::FlushIt;

        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_WriteData
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::LostConnection
            , strPath()
            , strmTmp.strData()
        );
    }
    return c4Written;
}


tCIDLib::TVoid TCommPort::Close()
{
    // Just delegate to the kernel implementation object
    if (!m_kcommThis.bClose())
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_Close
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
}


tCIDLib::TVoid TCommPort::Cycle(const   tCIDComm::EOpenFlags    eFlags
                                , const tCIDLib::TCard4         c4ReadBufSz
                                , const tCIDLib::TCard4         c4WriteBufSz)
{
    // Just delegate to the kernel implementation object
    if (!m_kcommThis.bCycle(eFlags, c4ReadBufSz, c4WriteBufSz))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_Cycle
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


tCIDComm::EFlags TCommPort::eFlags() const
{
    tCIDComm::EFlags eRet;
    if (!m_kcommThis.bQueryFlags(eRet))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_QueryFlags
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
    return eRet;
}


tCIDLib::TVoid TCommPort::FlushWriteBuffer()
{
    if (!m_kcommThis.bFlushWriteBuffer())
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_FlushWrite
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


tCIDLib::TVoid TCommPort::Open( const   tCIDComm::EOpenFlags    eFlags
                                , const tCIDLib::TCard4         c4ReadBufSz
                                , const tCIDLib::TCard4         c4WriteBufSz)
{
    // Just delegate to the kernel implementation object
    if (!m_kcommThis.bOpen(eFlags, c4ReadBufSz, c4WriteBufSz))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_Open
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


tCIDLib::TVoid TCommPort::PurgeReadData()
{
    // Just delegate to the kernel implementation object
    if (!m_kcommThis.bPurgeReadData())
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_PurgeRead
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


tCIDLib::TVoid TCommPort::PurgeWriteData()
{
    // Just delegate to the kernel implementation object
    if (!m_kcommThis.bPurgeWriteData())
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_PurgeWrite
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


tCIDLib::TVoid TCommPort::QueryCfg(TCommPortCfg& cpcfgToFill) const
{
    // Load into locals first, since the kernel knows not of our classes
    tCIDLib::TCard4     c4Baud;
    tCIDComm::EParities eParity;
    tCIDComm::EDataBits eDataBits;
    tCIDComm::EStopBits eStopBits;
    tCIDComm::EFlags    eFlags;
    tCIDComm::EPortDTR  eDTRFlow;
    tCIDComm::EPortRTS  eRTSFlow;

    if (!m_kcommThis.bQueryCfg(c4Baud, eParity, eDataBits, eStopBits, eFlags, eDTRFlow, eRTSFlow))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_QueryCfg
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }

    // It worked, so set up the caller's object
    cpcfgToFill.SetAll(c4Baud, eFlags, eParity, eDataBits, eStopBits, eDTRFlow, eRTSFlow);
}


tCIDLib::TVoid
TCommPort::QueryLineStates( tCIDLib::TBoolean&      bCTSOn
                            , tCIDLib::TBoolean&    bDSROn
                            , tCIDLib::TBoolean&    bRingOn
                            , tCIDLib::TBoolean&    bRLSDOn) const
{
    if (!m_kcommThis.bQueryLineStates(bCTSOn, bDSROn, bRingOn, bRLSDOn))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_QueryLineState
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


tCIDLib::TVoid
TCommPort::QueryXChars(tCIDLib::TSCh& schOn, tCIDLib::TSCh& schOff) const
{
    if (!m_kcommThis.bQueryXChars(schOn, schOff))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_QueryXChars
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


tCIDLib::TVoid TCommPort::Reset()
{
    if (!m_kcommThis.bSetDefCfg())
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_Reset
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


tCIDLib::TVoid TCommPort::SetBaudRate(const tCIDLib::TCard4 c4ToSet)
{
    if (!m_kcommThis.bSetBaudRate(c4ToSet))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_SetBaud
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TCardinal(c4ToSet)
            , strPath()
        );
    }
}


tCIDLib::TVoid TCommPort::SetBreak(const tCIDLib::TBoolean bState)
{
    if (!m_kcommThis.bSetBreak(bState))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_SetBreak
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


tCIDLib::TVoid TCommPort::SetCfg(const TCommPortCfg& cpcfgToSet)
{
    if (!m_kcommThis.bSetCfg(   cpcfgToSet.c4Baud()
                                , cpcfgToSet.eParity()
                                , cpcfgToSet.eDataBits()
                                , cpcfgToSet.eStopBits()
                                , cpcfgToSet.eFlags()
                                , cpcfgToSet.eDTR()
                                , cpcfgToSet.eRTS()))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_SetCfg
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


tCIDLib::TVoid TCommPort::SetDataBits(const tCIDComm::EDataBits eToSet)
{
    if (!m_kcommThis.bSetDataBits(eToSet))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_SetDataBits
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


tCIDLib::TVoid TCommPort::SetDTR(const tCIDComm::EPortDTR eToSet)
{
    if (!m_kcommThis.bSetDTR(eToSet))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_SetDTR
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


tCIDLib::TVoid TCommPort::SetParity(const tCIDComm::EParities eToSet)
{
    if (!m_kcommThis.bSetParity(eToSet))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_SetParity
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


tCIDLib::TVoid TCommPort::SetRTS(const tCIDComm::EPortRTS eToSet)
{
    if (!m_kcommThis.bSetRTS(eToSet))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_SetRTS
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


tCIDLib::TVoid TCommPort::SetStopBits(const tCIDComm::EStopBits eToSet)
{
    if (!m_kcommThis.bSetStopBits(eToSet))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_SetStopBits
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


tCIDLib::TVoid TCommPort::SetEOFChar(const tCIDLib::TSCh schToSet)
{
    if (!m_kcommThis.bSetEOFChar(schToSet))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_SetEOFChar
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


tCIDLib::TVoid TCommPort::SetFlags( const   tCIDComm::EFlags    eToSet
                                    , const tCIDComm::EFlags    eMask)
{
    if (!m_kcommThis.bSetFlags(eToSet, eMask))
    {
        facCIDComm().ThrowKrnlErr
        (
            CID_FILE
            , CID_LINE
            , kCommErrs::errcPort_SetFlags
            , TKrnlError::kerrLast()
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , strPath()
        );
    }
}


// ---------------------------------------------------------------------------
//  TCommPort: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TCommPort::c4PortNum() const
{
    return m_kcommThis.c4PortNum();
}

