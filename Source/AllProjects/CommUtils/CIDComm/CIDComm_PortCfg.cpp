//
// FILE NAME: CIDComm_PortCfg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/03/2000
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
//  This file implements the TCommPortCfg class.
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
RTTIDecls(TCommPortCfg,TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDComm_PortCfg
{
    const tCIDLib::TCard2   c2FmtVersion = 1;
}



// ---------------------------------------------------------------------------
//  CLASS: TCommPortCfg
// PREFIX: cpcfg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCommPortCfg: Constructors and Destructor
// ---------------------------------------------------------------------------
TCommPortCfg::TCommPortCfg() :

    m_c4Baud(9600)
    , m_eDataBits(tCIDComm::EDataBits::Eight)
    , m_eDTR(tCIDComm::EPortDTR::Disable)
    , m_eFlags(tCIDComm::EFlags::None)
    , m_eParity(tCIDComm::EParities::None)
    , m_eRTS(tCIDComm::EPortRTS::Disable)
    , m_eStopBits(tCIDComm::EStopBits::One)
{
}

TCommPortCfg::TCommPortCfg( const   tCIDLib::TCard4         c4Baud
                            , const tCIDComm::EFlags        eFlags
                            , const tCIDComm::EParities     eParity
                            , const tCIDComm::EDataBits     eDataBits
                            , const tCIDComm::EStopBits     eStopBits
                            , const tCIDComm::EPortDTR      eDTR
                            , const tCIDComm::EPortRTS      eRTS) :
    m_c4Baud(c4Baud)
    , m_eDataBits(eDataBits)
    , m_eDTR(eDTR)
    , m_eFlags(eFlags)
    , m_eParity(eParity)
    , m_eRTS(eRTS)
    , m_eStopBits(eStopBits)
{
}


// ---------------------------------------------------------------------------
//  TCommPortCfg: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCommPortCfg::operator==(const TCommPortCfg& cpcfgToCompare) const
{
    // Put the ones most likely to differ up front!
    if ((m_c4Baud != cpcfgToCompare.m_c4Baud)
    ||  (m_eDataBits != cpcfgToCompare.m_eDataBits)
    ||  (m_eStopBits != cpcfgToCompare.m_eStopBits)
    ||  (m_eParity != cpcfgToCompare.m_eParity)
    ||  (m_eDTR != cpcfgToCompare.m_eDTR)
    ||  (m_eFlags != cpcfgToCompare.m_eFlags)
    ||  (m_eRTS != cpcfgToCompare.m_eRTS))
    {
        return kCIDLib::False;
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean
TCommPortCfg::operator!=(const TCommPortCfg& cpcfgToCompare) const
{
    return !operator==(cpcfgToCompare);
}


// ---------------------------------------------------------------------------
//  TCommPortCfg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCard4 TCommPortCfg::c4Baud() const
{
    return m_c4Baud;
}

tCIDLib::TCard4 TCommPortCfg::c4Baud(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Baud = c4ToSet;
    return m_c4Baud;
}

tCIDComm::EDataBits TCommPortCfg::eDataBits() const
{
    return m_eDataBits;
}

tCIDComm::EDataBits
TCommPortCfg::eDataBits(const tCIDComm::EDataBits eToSet)
{
    m_eDataBits = eToSet;
    return m_eDataBits;
}

tCIDComm::EPortDTR TCommPortCfg::eDTR() const
{
    return m_eDTR;
}

tCIDComm::EPortDTR TCommPortCfg::eDTR(const tCIDComm::EPortDTR eToSet)
{
    m_eDTR = eToSet;
    return m_eDTR;
}

tCIDComm::EFlags TCommPortCfg::eFlags() const
{
    return m_eFlags;
}

tCIDComm::EFlags TCommPortCfg::eFlags(const tCIDComm::EFlags eToSet)
{
    m_eFlags = eToSet;
    return m_eFlags;
}

tCIDComm::EParities TCommPortCfg::eParity() const
{
    return m_eParity;
}

tCIDComm::EParities TCommPortCfg::eParity(const tCIDComm::EParities eToSet)
{
    m_eParity = eToSet;
    return m_eParity;
}

tCIDComm::EPortRTS TCommPortCfg::eRTS() const
{
    return m_eRTS;
}

tCIDComm::EPortRTS TCommPortCfg::eRTS(const tCIDComm::EPortRTS eToSet)
{
    m_eRTS = eToSet;
    return m_eRTS;
}

tCIDComm::EStopBits TCommPortCfg::eStopBits() const
{
    return m_eStopBits;
}

tCIDComm::EStopBits TCommPortCfg::eStopBits(const tCIDComm::EStopBits eToSet)
{
    m_eStopBits = eToSet;
    return m_eStopBits;
}


tCIDLib::TVoid TCommPortCfg::Reset()
{
    m_c4Baud    = 9600;
    m_eDataBits = tCIDComm::EDataBits::Eight;
    m_eDTR      = tCIDComm::EPortDTR::Disable;
    m_eFlags    = tCIDComm::EFlags::None;
    m_eParity   = tCIDComm::EParities::None;
    m_eRTS      = tCIDComm::EPortRTS::Disable;
    m_eStopBits = tCIDComm::EStopBits::One;
}


tCIDLib::TVoid
TCommPortCfg::SetAll(const  tCIDLib::TCard4     c4Baud
                    , const tCIDComm::EFlags    eFlags
                    , const tCIDComm::EParities eParity
                    , const tCIDComm::EDataBits eDataBits
                    , const tCIDComm::EStopBits eStopBits
                    , const tCIDComm::EPortDTR  eDTR
                    , const tCIDComm::EPortRTS  eRTS)
{
    m_c4Baud    = c4Baud;
    m_eDataBits = eDataBits;
    m_eDTR      = eDTR;
    m_eFlags    = eFlags;
    m_eParity   = eParity;
    m_eRTS      = eRTS;
    m_eStopBits = eStopBits;
}


// ---------------------------------------------------------------------------
//  TCommPortCfg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCommPortCfg::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Make sure we get the start marker
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (c2FmtVersion != CIDComm_PortCfg::c2FmtVersion)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , clsThis()
        );
    }

    // Looks ok, so stream our other stuff in
    strmToReadFrom  >> m_c4Baud
                    >> m_eDataBits
                    >> m_eDTR
                    >> m_eFlags
                    >> m_eParity
                    >> m_eRTS
                    >> m_eStopBits;

    // And it should end with an end marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TCommPortCfg::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDComm_PortCfg::c2FmtVersion
                    << m_c4Baud
                    << m_eDataBits
                    << m_eDTR
                    << m_eFlags
                    << m_eParity
                    << m_eRTS
                    << m_eStopBits
                    << tCIDLib::EStreamMarkers::EndObject;
}


