//
// FILE NAME: CIDNet_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/05/2000
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
//  This is the header file for the CIDNet_ThisFacility.cpp file. This
//  file implements the facility class for this facility.
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
//  CLASS: TFacCIDNet
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDNETEXP TFacCIDNet : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDNet();

        ~TFacCIDNet();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTestLogFail
        (
            const   tCIDNet::ELogFlags      eFlag
        )   const;

        tCIDLib::TBoolean bTestLogInfo
        (
            const   tCIDNet::ELogFlags      eFlag
        )   const;

        tCIDLib::TBoolean bTestLog
        (
            const   tCIDLib::ESeverities    eToCheck
            , const tCIDNet::ELogFlags      eFlag
        )   const;

        tCIDLib::TBoolean bTestLog
        (
            const   TError&                 errToCheck
            , const tCIDNet::ELogFlags      eFlag
        )   const;

        tCIDLib::TBoolean bTestLogStatus
        (
            const   tCIDNet::ELogFlags      eFlag
        )   const;

        tCIDLib::TBoolean bTestLogWarn
        (
            const   tCIDNet::ELogFlags      eFlag
        )   const;

        tCIDLib::TVoid ParseMultiPartMIME
        (
            const   TMemBuf&                mbufSrc
            , const tCIDLib::TCard4         c4SrcBytes
            , const TString&                strContType
            ,       TVector<THeapBuf>&      colParts
            ,       tCIDLib::TCardList&     fcolSizes
            ,       tCIDLib::TStrList&      colContTypes
            ,       tCIDLib::TStrList&      colDispositions
            ,       tCIDLib::TStrList&      colTransEncodings
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDNet,TFacility)
};

#pragma CIDLIB_POPPACK


// These just become calls to our parent class
inline tCIDLib::TBoolean
TFacCIDNet::bTestLogFail(const tCIDNet::ELogFlags eFlag) const
{
    return bCheckLogFlags(tCIDLib::ESeverities::Failed, tCIDLib::TCard4(eFlag));
}

inline tCIDLib::TBoolean
TFacCIDNet::bTestLogInfo(const tCIDNet::ELogFlags eFlag) const
{
    return bCheckLogFlags(tCIDLib::ESeverities::Info, tCIDLib::TCard4(eFlag));
}

inline tCIDLib::TBoolean
TFacCIDNet::bTestLog(const  tCIDLib::ESeverities    eToCheck
                    , const tCIDNet::ELogFlags      eFlag) const
{
    return bCheckLogFlags(eToCheck, tCIDLib::TCard4(eFlag));
}

inline tCIDLib::TBoolean
TFacCIDNet::bTestLog(const  TError&             errToCheck
                    , const tCIDNet::ELogFlags  eFlag) const
{
    return bCheckLogFlags(errToCheck, tCIDLib::TCard4(eFlag));
}

inline tCIDLib::TBoolean
TFacCIDNet::bTestLogStatus(const tCIDNet::ELogFlags eFlag) const
{
    return bCheckLogFlags(tCIDLib::ESeverities::Status, tCIDLib::TCard4(eFlag));
}

inline tCIDLib::TBoolean
TFacCIDNet::bTestLogWarn(const tCIDNet::ELogFlags eFlag) const
{
    return bCheckLogFlags(tCIDLib::ESeverities::Warn, tCIDLib::TCard4(eFlag));
}

