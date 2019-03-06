//
// FILE NAME: CIDComm_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/27/2000
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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

