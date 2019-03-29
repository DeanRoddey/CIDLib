//
// FILE NAME: CIDComm_LocalPortFactory_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/12/2005
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
//  This is the header for the CIDComm_LocalPortFactory.cpp file, which
//  implements the pluggable com port factory for local ports. This one is
//  alwys plugged in automatically so that local ports are always supported.
//
//  This doesn't have to be exported, since ports are created via the
//  facility class so that any kind of plugged in port support will be
//  transparent to outside code.
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
//  CLASS: TLocalComPortFactory
// PREFIX: pfact
// ---------------------------------------------------------------------------
class TLocalComPortFactory : public TComPortFactory
{
    public  :
        // -------------------------------------------------------------------
        //  Destructor
        // -------------------------------------------------------------------
        TLocalComPortFactory();

        TLocalComPortFactory(const TLocalComPortFactory&) = delete;

        ~TLocalComPortFactory() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TLocalComPortFactory& operator=(const TLocalComPortFactory&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bQueryPorts
        (
                    tCIDLib::TStrCollect&   colToFill
            , const tCIDLib::TBoolean       bAvailOnly
        )   const override;

        tCIDLib::TBoolean bTestCfg
        (
            const   TCommPortCfg&           cpcfgTest
            ,       TString&                strReason
        )   const override;

        tCIDLib::TBoolean bValidatePath
        (
            const   TString&                strPath
        )   const override;

        TCommPortBase* pcommMakeNew
        (
            const   TString&                strPath
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bValidate
        (
            const   TString&                strPath
            ,       tCIDLib::TCard4&        c4PortNum
        )   const;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TLocalComPortFactory,TComPortFactory)
};

#pragma CIDLIB_POPPACK



