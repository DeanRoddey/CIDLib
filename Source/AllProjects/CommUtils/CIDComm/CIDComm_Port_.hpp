//
// FILE NAME: CIDComm_Port_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/28/2000
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
//  This is the header for the CIDComm_Port.cpp file, which derives a
//  class, TCommPort, from the TCommPortBase class. TCommPortBase is an
//  abstract base for comm ports, and this one creates a derivate of that
//  interface for real local comm ports.
//
//  We don't have to export this class because outsiders create new com
//  port objects via the facility class, because of our need to potentially
//  support non-local ports tranparently in owning applications.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once



#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TCommPort
// PREFIX: comm
// ---------------------------------------------------------------------------
class TCommPort : public TCommPortBase
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCommPort();

        TCommPort
        (
            const   TString&                strPath
            , const tCIDLib::TCard4         c4PortNum
        );

        TCommPort(const TCommPort&) = delete;

        ~TCommPort();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCommPort& operator=(const TCommPort&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsOpen() const override;

        tCIDLib::TBoolean bQueryCTSOn() const override;

        tCIDLib::TBoolean bQueryDSROn() const override;

        tCIDLib::TCard4 c4ReadRawBufMS
        (
                    tCIDLib::TVoid* const   pToFill
            , const tCIDLib::TCard4         c4MaxBytes
            , const tCIDLib::TCard4         c4Wait
        ) override;

        tCIDLib::TCard4 c4ReadRawBuf
        (
                    tCIDLib::TVoid* const   pToFill
            , const tCIDLib::TCard4         c4MaxBytes
            , const tCIDLib::TEncodedTime   enctWait
        ) override;

        tCIDLib::TCard4 c4WriteRawBufMS
        (
            const   tCIDLib::TVoid* const   pToWrite
            , const tCIDLib::TCard4         c4BytesToWrite
            , const tCIDLib::TCard4         c4Wait
        ) override;

        tCIDLib::TVoid Close() override;

        tCIDLib::TVoid Cycle
        (
            const   tCIDComm::EOpenFlags    eFlags = tCIDComm::EOpenFlags::None
            , const tCIDLib::TCard4         c4ReadBufSz = 0
            , const tCIDLib::TCard4         c4WriteBufSz = 0
        ) override;

        tCIDComm::EFlags eFlags() const override;

        tCIDLib::TVoid FlushWriteBuffer() override;

        tCIDLib::TVoid Open
        (
            const   tCIDComm::EOpenFlags    eFlags = tCIDComm::EOpenFlags::None
            , const tCIDLib::TCard4         c4ReadBufSz = 0
            , const tCIDLib::TCard4         c4WriteBufSz = 0
        ) override;

        tCIDLib::TVoid PurgeReadData() override;

        tCIDLib::TVoid PurgeWriteData() override;

        tCIDLib::TVoid QueryCfg
        (
                    TCommPortCfg&           cpcfgToFill
        )   const override;

        tCIDLib::TVoid QueryLineStates
        (
                    tCIDLib::TBoolean&      bCTSOn
            ,       tCIDLib::TBoolean&      bDSROn
            ,       tCIDLib::TBoolean&      bRingOn
            ,       tCIDLib::TBoolean&      bRLSDOn
        )   const override;

        tCIDLib::TVoid QueryXChars
        (
                    tCIDLib::TSCh&          schOn
            ,       tCIDLib::TSCh&          schOff
        )   const override;

        tCIDLib::TVoid Reset() override;

        tCIDLib::TVoid SetBaudRate
        (
            const   tCIDLib::TCard4         c4ToSet
        ) override;

        tCIDLib::TVoid SetBreak
        (
            const   tCIDLib::TBoolean       bState
        ) override;

        tCIDLib::TVoid SetDataBits
        (
            const   tCIDComm::EDataBits     eToSet
        ) override;

        tCIDLib::TVoid SetDTR
        (
            const   tCIDComm::EPortDTR      eToSet
        ) override;

        tCIDLib::TVoid SetParity
        (
            const   tCIDComm::EParities     eToSet
        ) override;

        tCIDLib::TVoid SetRTS
        (
            const   tCIDComm::EPortRTS      eToSet
        ) override;

        tCIDLib::TVoid SetStopBits
        (
            const   tCIDComm::EStopBits     eToSet
        ) override;

        tCIDLib::TVoid SetCfg
        (
            const   TCommPortCfg&           cpcfgToSet
        ) override;

        tCIDLib::TVoid SetEOFChar
        (
            const   tCIDLib::TSCh           schToSet
        ) override;

        tCIDLib::TVoid SetFlags
        (
            const   tCIDComm::EFlags        eToSet
            , const tCIDComm::EFlags        eMask
        ) override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4PortNum() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_kcommThis
        //      The kernel object that encapsulates the comm port services
        //      for local ports.
        // -------------------------------------------------------------------
        TKrnlCommPort   m_kcommThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCommPort,TCommPortBase)
};

#pragma CIDLIB_POPPACK


