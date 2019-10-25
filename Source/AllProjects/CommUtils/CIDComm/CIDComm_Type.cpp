//
// FILE NAME: CIDComm_Type.cpp
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
//  This file provides formatting for the facility specific enums from
//  CIDComm_Type.hpp.
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
//  These are in the facility types namespace, so as to match the scheme that the
//  IDL compiler generates.
// ---------------------------------------------------------------------------
TString tCIDComm::strXlatEDataBits(const tCIDComm::EDataBits eToXlat)
{
    if ((eToXlat < tCIDComm::EDataBits::Min)
    ||  (eToXlat > tCIDComm::EDataBits::Max))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eToXlat))
            , TString(L"tCIDComm::EDataBits")
        );
    }

    //
    //  We can't use the enums as indicates on these because they are not
    //  zero based, so just use a raw array. BE SURE to update this if the
    //  enum definition changes down in CIDKernel_Types.hpp.
    //
    static const tCIDLib::TCh* const apszDataBits[tCIDLib::c4EnumOrd(tCIDComm::EDataBits::Count)]
    {
        L"Four"
        , L"Five"
        , L"Six"
        , L"Seven"
        , L"Eight"
    };
    return TString
    (
        apszDataBits
        [
            tCIDLib::c4EnumOrd(eToXlat) - tCIDLib::c4EnumOrd(tCIDComm::EDataBits::Min)
        ]
    );
}


const TString& tCIDComm::strXlatEParities(const tCIDComm::EParities eToXlat)
{
    if (eToXlat > tCIDComm::EParities::Max)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eToXlat))
            , TString(L"tCIDComm::EParities")
        );
    }

    static TEArray<TString, tCIDComm::EParities, tCIDComm::EParities::Count>
    astrValues(TString::strEmpty());
    if (!astrValues.bIsLoaded())
    {
        TBaseLock lockInit;
        if (!astrValues.bIsLoaded())
        {
            astrValues[tCIDComm::EParities::None] = L"None";
            astrValues[tCIDComm::EParities::Odd] = L"Odd";
            astrValues[tCIDComm::EParities::Even] = L"Even";
            astrValues[tCIDComm::EParities::Mark] = L"Mark";
            astrValues[tCIDComm::EParities::Space] = L"Space";

            astrValues.SetLoaded();
        }
    }
    return astrValues[eToXlat];
}


const TString& tCIDComm::strXlatEPortDTR(const tCIDComm::EPortDTR eToXlat)
{
    if (eToXlat > tCIDComm::EPortDTR::Max)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eToXlat))
            , TString(L"tCIDComm::EPortDTR")
        );
    }

    static TEArray<TString, tCIDComm::EPortDTR, tCIDComm::EPortDTR::Count>
    astrValues(TString::strEmpty());
    if (!astrValues.bIsLoaded())
    {
        TBaseLock lockInit;
        if (!astrValues.bIsLoaded())
        {
            astrValues[tCIDComm::EPortDTR::Disable] = L"Disable";
            astrValues[tCIDComm::EPortDTR::Enable] = L"Enable";
            astrValues[tCIDComm::EPortDTR::Handshake] = L"Handshake";

            astrValues.SetLoaded();
        }
    }
    return astrValues[eToXlat];
}


const TString& tCIDComm::strXlatEPortRTS(const tCIDComm::EPortRTS eToXlat)
{
    if (eToXlat > tCIDComm::EPortRTS::Max)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eToXlat))
            , TString(L"tCIDComm::EPortRTS")
        );
    }

    static TEArray<TString, tCIDComm::EPortRTS, tCIDComm::EPortRTS::Count>
    astrValues(TString::strEmpty());
    if (!astrValues.bIsLoaded())
    {
        TBaseLock lockInit;
        if (!astrValues.bIsLoaded())
        {
            astrValues[tCIDComm::EPortRTS::Disable] = L"Disable";
            astrValues[tCIDComm::EPortRTS::Enable] = L"Enable";
            astrValues[tCIDComm::EPortRTS::Handshake] = L"Handshake";
            astrValues[tCIDComm::EPortRTS::Toggle] = L"Toggle";

            astrValues.SetLoaded();
        }
    }
    return astrValues[eToXlat];
}


const TString& tCIDComm::strXlatEStopBits(const tCIDComm::EStopBits eToXlat)
{
    if (eToXlat > tCIDComm::EStopBits::Max)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(eToXlat))
            , TString(L"tCIDComm::EStopBits")
        );
    }

    static TEArray<TString, tCIDComm::EStopBits, tCIDComm::EStopBits::Count>
    astrValues(TString::strEmpty());
    if (!astrValues.bIsLoaded())
    {
        TBaseLock lockInit;
        if (!astrValues.bIsLoaded())
        {
            astrValues[tCIDComm::EStopBits::One] = L"1";
            astrValues[tCIDComm::EStopBits::OnePointFive] = L"1.5";
            astrValues[tCIDComm::EStopBits::Two] = L"2";

            astrValues.SetLoaded();
        }
    }
    return astrValues[eToXlat];
}



// ---------------------------------------------------------------------------
//  Global functions
// ---------------------------------------------------------------------------
TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDComm::EDataBits eBits)
{
    strmToWriteTo << tCIDComm::strXlatEDataBits(eBits);
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDComm::EErrors eErr)
{
    strmToWriteTo << L"{";

    if (eErr == tCIDComm::EErrors::None)
    {
        strmToWriteTo << L"None";
    }
     else if (eErr == tCIDComm::EErrors::Unknown)
    {
        strmToWriteTo << L"Unknown";
    }
     else
    {
        tCIDLib::TCard4 c4Count = 0;
        if (tCIDLib::bAllBitsOn(eErr, tCIDComm::EErrors::Break))
        {
            c4Count++;
            strmToWriteTo << L"Break";
        }
        if (tCIDLib::bAllBitsOn(eErr, tCIDComm::EErrors::Frame))
        {
            if (c4Count)
                strmToWriteTo << L",";
            c4Count++;
            strmToWriteTo << L"Frame";
        }
        if (tCIDLib::bAllBitsOn(eErr, tCIDComm::EErrors::Error))
        {
            if (c4Count)
                strmToWriteTo << L",";
            c4Count++;
            strmToWriteTo << L"Error";
        }
        if (tCIDLib::bAllBitsOn(eErr, tCIDComm::EErrors::BadMode))
        {
            if (c4Count)
                strmToWriteTo << L",";
            c4Count++;
            strmToWriteTo << L"BadMode";
        }
        if (tCIDLib::bAllBitsOn(eErr, tCIDComm::EErrors::Overrun))
        {
            if (c4Count)
                strmToWriteTo << L",";
            c4Count++;
            strmToWriteTo << L"Overrun";
        }
        if (tCIDLib::bAllBitsOn(eErr, tCIDComm::EErrors::RXOver))
        {
            if (c4Count)
                strmToWriteTo << L",";
            c4Count++;
            strmToWriteTo << L"RXOver";
        }
        if (tCIDLib::bAllBitsOn(eErr, tCIDComm::EErrors::TXFull))
        {
            if (c4Count)
                strmToWriteTo << L",";
            c4Count++;
            strmToWriteTo << L"TXFull";
        }
        if (tCIDLib::bAllBitsOn(eErr, tCIDComm::EErrors::Parity))
        {
            if (c4Count)
                strmToWriteTo << L",";
            c4Count++;
            strmToWriteTo << L"Parity";
        }
        if (tCIDLib::bAllBitsOn(eErr, tCIDComm::EErrors::Other))
        {
            if (c4Count)
                strmToWriteTo << L",";
            c4Count++;
            strmToWriteTo << L"Other";
        }
    }
    strmToWriteTo << L"}";

    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDComm::EParities eParity)
{
    strmToWriteTo << tCIDComm::strXlatEParities(eParity);
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDComm::EPortDTR eDTR)
{
    strmToWriteTo << tCIDComm::strXlatEPortDTR(eDTR);
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDComm::EPortRTS eRTS)
{
    strmToWriteTo << tCIDComm::strXlatEPortRTS(eRTS);
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDComm::EPortTypes ePortType)
{
    if (ePortType == tCIDComm::EPortTypes::RS232)
        strmToWriteTo << L"RS-232";
    else if (ePortType == tCIDComm::EPortTypes::RS422)
        strmToWriteTo << L"RS-422";
    else
    {
        // We are throwing a CIDLib error here, not one of ours!
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::ProcFatal
            , tCIDLib::EErrClasses::BadParms
            , TInteger(tCIDLib::i4EnumOrd(ePortType))
            , TString(L"tCIDComm::EPortType")
        );
    }
    return strmToWriteTo;
}


TTextOutStream&
operator<<(TTextOutStream& strmToWriteTo, const tCIDComm::EStopBits eBits)
{
    strmToWriteTo << tCIDComm::strXlatEStopBits(eBits);
    return strmToWriteTo;
}

