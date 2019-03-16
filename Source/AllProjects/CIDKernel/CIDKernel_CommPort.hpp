//
// FILE NAME: CIDKernel_CommPort.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/27/2000
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
//  This is the header for the CIDKernel_CommPort.cpp file, which implements
//  the TKrnlCommPort class. This class represents an RS-232 port and provdes
//  the mechanisms to maniuplate the port's state and send/receive data.
//
// CAVEATS/GOTCHAS:
//
//  1)  In order to allow for portability, the platform support must init
//      a newly opened port to the following defaults. Without this assumed
//      initial state, every program would have to do a complete set up of
//      the port every time, in order to insure portability.
//
//              Baud: 9600
//            Parity: Disabled
//         Stop Bits: 1
//              Flow: All hardware and software flow control off
//         Handshake: All hardware handshaking is disabled
//            Errors: Read/write errors stop the current read/write op
//
//  2)  Read/write operations must be able to happen simultaneously because
//      it is common for a program to have one thread blocking on input while
//      another thread periodically writes stuff. Other operations require
//      synchronization by the user of the port.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TKrnlCommPort
//  PREFIX: kcomm
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlCommPort
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TCard4 c4EnumPorts
        (
                    tCIDLib::TBoolean* const pbToFill
            , const tCIDLib::TCard4          c4Count
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlCommPort();

        TKrnlCommPort
        (
            const   tCIDLib::TCard4         c4PortNum
        );

        TKrnlCommPort(const TKrnlCommPort&) = delete;

        ~TKrnlCommPort();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlCommPort& operator=(const TKrnlCommPort&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bClose();

        tCIDLib::TBoolean bCycle
        (
            const   tCIDComm::EOpenFlags    eFlags
            , const tCIDLib::TCard4         c4ReadBufSz
            , const tCIDLib::TCard4         c4WriteBufSz
        );

        tCIDLib::TBoolean bFlushWriteBuffer();

        tCIDLib::TBoolean bIsOpen() const;

        tCIDLib::TBoolean bOpen
        (
            const   tCIDComm::EOpenFlags    eFlags
            , const tCIDLib::TCard4         c4ReadBufSz
            , const tCIDLib::TCard4         c4WriteBufSz
        );

        tCIDLib::TBoolean bPurgeReadData();

        tCIDLib::TBoolean bPurgeWriteData();

        tCIDLib::TBoolean bQueryCfg
        (
                    tCIDLib::TCard4&        c4Baud
            ,       tCIDComm::EParities&    eParity
            ,       tCIDComm::EDataBits&    eDataBits
            ,       tCIDComm::EStopBits&    eStopBits
            ,       tCIDComm::EFlags&       eFlags
            ,       tCIDComm::EPortDTR&     eDTRFlow
            ,       tCIDComm::EPortRTS&     eRTSFlow
        )   const;

        tCIDLib::TBoolean bQueryCTSOn
        (
                    tCIDLib::TBoolean&      bToFill
        )   const;

        tCIDLib::TBoolean bQueryDSROn
        (
                    tCIDLib::TBoolean&      bToFill
        )   const;

        tCIDLib::TBoolean bQueryFlags
        (
                    tCIDComm::EFlags&       eFlags
        )   const;

        tCIDLib::TBoolean bQueryLineStates
        (
                    tCIDLib::TBoolean&      bCTSOn
            ,       tCIDLib::TBoolean&      bDSROn
            ,       tCIDLib::TBoolean&      bRingOn
            ,       tCIDLib::TBoolean&      bRLSDOn
        )   const;

        tCIDLib::TBoolean bQueryXChars
        (
                    tCIDLib::TSCh&          schOn
            ,       tCIDLib::TSCh&          schOff
        )   const;

        tCIDLib::TBoolean bReadRawBuf
        (
                    tCIDLib::TVoid* const   pToFill
            , const tCIDLib::TCard4         c4MaxBytes
            ,       tCIDLib::TCard4&        c4BytesRead
            ,       tCIDComm::EErrors&      eErrors
            , const tCIDLib::TEncodedTime   enctWait
        );

        tCIDLib::TBoolean bSetBaudRate
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TBoolean bSetBreak
        (
            const   tCIDLib::TBoolean       bState
        );

        tCIDLib::TBoolean bSetCfg
        (
            const   tCIDLib::TCard4         c4Baud
            , const tCIDComm::EParities     eParity
            , const tCIDComm::EDataBits     eDataBits
            , const tCIDComm::EStopBits     eStopBits
            , const tCIDComm::EFlags        eFlags
            , const tCIDComm::EPortDTR      eDTRFlow
            , const tCIDComm::EPortRTS      eRTSFlow
        );

        tCIDLib::TBoolean bSetDataBits
        (
            const   tCIDComm::EDataBits     eToSet
        );

        tCIDLib::TBoolean bSetDefCfg();

        tCIDLib::TBoolean bSetDTR
        (
            const   tCIDComm::EPortDTR      eToSet
        );

        tCIDLib::TBoolean bSetEOFChar
        (
            const   tCIDLib::TSCh           schToSet
        );

        tCIDLib::TBoolean bSetFlags
        (
            const   tCIDComm::EFlags        eToSet
            , const tCIDComm::EFlags        eMask
        );

        tCIDLib::TBoolean bSetParity
        (
            const   tCIDComm::EParities     eToSet
        );

        tCIDLib::TBoolean bSetPort
        (
            const   tCIDLib::TCard4         c4PortNum
        );

        tCIDLib::TBoolean bSetRTS
        (
            const   tCIDComm::EPortRTS      eToSet
        );

        tCIDLib::TBoolean bSetStopBits
        (
            const   tCIDComm::EStopBits     eToSet
        );

        tCIDLib::TBoolean bWriteRawBuf
        (
            const   tCIDLib::TVoid* const   pToWrite
            , const tCIDLib::TCard4         c4BytesToWrite
            ,       tCIDLib::TCard4&        c4BytesWritten
            ,       tCIDComm::EErrors&      eErrors
            , const tCIDLib::TEncodedTime   enctWait
        );

        tCIDLib::TCard4 c4PortNum() const;



    private :
        // -------------------------------------------------------------------
        //  An opaque type for the per-platform data
        // -------------------------------------------------------------------
        struct TPerPlat;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4PortNum
        //      This class represents a real, local port number. When the
        //      CIDComm layer creates one of these, he will pass us a number
        //      from 1 to whatever, that represents the port number. The
        //      per-platform code will translate this into the appropriate
        //      form for the local platform.
        //
        //  m_hcommThis
        //      This is the handle to the comm port.
        //
        //  m_pExtra
        //      This can be used by per-platform drivers to hold extra info
        //      if they need to.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4PortNum;
        TCommHandle     m_hcommThis;
        TPerPlat*       m_pExtra;
};

#pragma CIDLIB_POPPACK


// ---------------------------------------------------------------------------
//  TKrnlCommPort: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDLib::TCard4 TKrnlCommPort::c4PortNum() const
{
    return m_c4PortNum;
}

