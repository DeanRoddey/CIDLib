//
// FILE NAME: CIDComm_PortCfgInfo.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/03/2000
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
//  This is the header for the CIDComm_PortCfgInfo.cpp file, which implements
//  the TCommPortCfg class. Each instance of this class contains the data
//  for configuring a Comm port.
//
// CAVEATS/GOTCHAS:
//
//  1)  The default constructor will create a config object that matches the
//      default port settings of:
//
//              Baud: 9600
//            Parity: None
//         Stop Bits: 1
//              Flow: All hardware and software flow control off
//         Handshake: All hardware handshaking is disabled
//            Errors: Read/write errors stop the current read/write op
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TCommPortCfg
// PREFIX: cpcfg
// ---------------------------------------------------------------------------
class CIDCOMMEXP TCommPortCfg : public TObject, public MStreamable
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCommPortCfg();

        TCommPortCfg
        (
            const   tCIDLib::TCard4         c4Baud
            , const tCIDComm::EFlags        eFlags
            , const tCIDComm::EParities     eParity
            , const tCIDComm::EDataBits     eDataBits
            , const tCIDComm::EStopBits     eStopBits
            , const tCIDComm::EPortDTR      eDTR
            , const tCIDComm::EPortRTS      eRTS
        );

        TCommPortCfg(const TCommPortCfg&) = default;

        ~TCommPortCfg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCommPortCfg& operator=(const TCommPortCfg&) = default;

        tCIDLib::TBoolean operator==
        (
            const   TCommPortCfg&           cpcfgToCompare
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TCommPortCfg&           cpcfgToCompare
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Baud() const;

        tCIDLib::TCard4 c4Baud
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDComm::EDataBits eDataBits() const;

        tCIDComm::EDataBits eDataBits
        (
            const   tCIDComm::EDataBits     eToSet
        );

        tCIDComm::EPortDTR eDTR() const;

        tCIDComm::EPortDTR eDTR
        (
            const   tCIDComm::EPortDTR      eToSet
        );

        tCIDComm::EFlags eFlags() const;

        tCIDComm::EFlags eFlags
        (
            const   tCIDComm::EFlags        eToSet
        );

        tCIDComm::EParities eParity() const;

        tCIDComm::EParities eParity
        (
            const   tCIDComm::EParities     eToSet
        );

        tCIDComm::EPortRTS eRTS() const;

        tCIDComm::EPortRTS eRTS
        (
            const   tCIDComm::EPortRTS      eToSet
        );

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid SetAll
        (
            const   tCIDLib::TCard4         c4Baud
            , const tCIDComm::EFlags        eFlags
            , const tCIDComm::EParities     eParity
            , const tCIDComm::EDataBits     eDataBits
            , const tCIDComm::EStopBits     eStopBits
            , const tCIDComm::EPortDTR      eDTR
            , const tCIDComm::EPortRTS      eRTS
        );

        tCIDComm::EStopBits eStopBits() const;

        tCIDComm::EStopBits eStopBits
        (
            const   tCIDComm::EStopBits     eToSet
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Baud
        //      The baud rate in bits per second.
        //
        //  m_eDataBits
        //      The number of data bits to use.
        //
        //  m_eFlags
        //  m_eDTS
        //  m_eParity
        //  m_eRTS
        //      The various port setup enum flags. Note that eParity sets the
        //      parity bits generated by this port. The parity flag in the
        //      m_eFlags field controls whether this port checks incoming
        //      data for parity errors.
        //
        //  m_eStopBits
        //      The number of stop bits to use.
        // -------------------------------------------------------------------
        tCIDLib::TCard4     m_c4Baud;
        tCIDComm::EDataBits m_eDataBits;
        tCIDComm::EPortDTR  m_eDTR;
        tCIDComm::EFlags    m_eFlags;
        tCIDComm::EParities m_eParity;
        tCIDComm::EPortRTS  m_eRTS;
        tCIDComm::EStopBits m_eStopBits;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCommPortCfg,TObject)
};

#pragma CIDLIB_POPPACK


