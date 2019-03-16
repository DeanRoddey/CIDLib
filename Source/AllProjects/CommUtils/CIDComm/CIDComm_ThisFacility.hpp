//
// FILE NAME: CIDComm_ThisFacility.hpp
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
//  This is the header file for the CIDComm_ThisFacility.cpp file. This
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
//  CLASS: TFacCIDComm
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDCOMMEXP TFacCIDComm : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TString strLocalFactoryId;
        static const TString strLocalPortPrefix;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDComm();

        ~TFacCIDComm();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCanConfigure
        (
            const   TString&                strPath
        )   const;

        tCIDLib::TBoolean bIsValidPortName
        (
            const   TString&                strToCheck
        )   const;

        tCIDLib::TBoolean bQueryPorts
        (
                    tCIDLib::TStrCollect&   colToFill
            , const tCIDLib::TBoolean       bLocalOnly
            , const tCIDLib::TBoolean       bAvailOnly
        )   const;

        tCIDLib::TVoid DeregisterFactory
        (
            const   TString&                strFactoryId
        );

        TCommPortBase* pcommMakeNew
        (
            const   TString&                strPath
        );

        TComPortFactory* pfactById
        (
            const   TString&                strID
        );

        TMutex* pmtxlLock();

        tCIDLib::TVoid RegisterFactory
        (
                    TComPortFactory* const  pfactToAdopt
        );

    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDComm,TFacility)
};

#pragma CIDLIB_POPPACK

