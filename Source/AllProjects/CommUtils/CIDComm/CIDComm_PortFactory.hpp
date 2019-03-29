//
// FILE NAME: CIDComm_PortFactory.hpp
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
//  This is the header for the CIDComm_PortFactory.cpp file. This file
//  implements an abstract base class that is used to create new serial
//  port objects. Though most serial ports are local hardware ports, not
//  all of them are. We have a base com port abstraction from which various
//  types of serial ports can be derived, but we want to make that as
//  transparent as possible to applications that use serial ports. So we
//  provide a means for the hosting app to register port factory objects
//  with us.
//
//  This let's us generically support any type of serial port. Each factory
//  tells us what ports it has available, and can indentify a port path
//  as one of its ports or not. When it's time to create a port, we can
//  iterate the port factories till we find the guy who owns that particular
//  port and ask it to create a port object for us, which it returns via the
//  base port class.
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
//  CLASS: TComPortFactory
// PREFIX: pfact
// ---------------------------------------------------------------------------
class CIDCOMMEXP TComPortFactory : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TComPortFactory() = delete;

        TComPortFactory(const TComPortFactory&) = delete;

        ~TComPortFactory() = default;


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TComPortFactory& operator=(const TComPortFactory&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bQueryPorts
        (
                    tCIDLib::TStrCollect&   colToFill
            , const tCIDLib::TBoolean       bAvailOnly
        )   const = 0;

        virtual tCIDLib::TBoolean bTestCfg
        (
            const   TCommPortCfg&           cpcfgTest
            ,       TString&                strReason
        )   const = 0;

        virtual tCIDLib::TBoolean bValidatePath
        (
            const   TString&                strPath
        )   const = 0;

        virtual TCommPortBase* pcommMakeNew
        (
            const   TString&                strPath
        ) = 0;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCanConfigure() const;

        tCIDLib::TBoolean bOwnsThisPort
        (
            const   TString&                strPath
        )   const;

        const TString& strId() const;

        const TString& strNamespace() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden Constructors
        // -------------------------------------------------------------------
        TComPortFactory
        (
            const   TString&                strId
            , const TString&                strNamespace
            , const tCIDLib::TBoolean       bCanConfigure
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetNamespace
        (
            const   TString&                strNamespace
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        //
        //  m_bCanConfigure
        //      Some types of ports we cannot set configuration on, we can only read
        //      and write data. The derived factory class passes us this.
        //
        //  m_strId
        //      An identifier for this factory, used to later deregister it or to
        //      otherwise specifically indicate this factory.
        //
        //  m_strNamespace
        //      In the ctor the derived class sets a namespace on us that represents
        //      the prefix that any of it's port's paths will start with. This let's
        //      us provide the strNamespace() and bOwnsThisPort() methods for all
        //      dervatives.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bCanConfigure;
        TString             m_strId;
        TString             m_strNamespace;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TComPortFactory,TObject)
};


#pragma CIDLIB_POPPACK



