//
// FILE NAME: CIDMacroEng_SysInfoClass_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/22/2003
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
//  This is the header file for the CIDMacroEng_SysInfoClass.cpp file, which
//  implements the info and value classes required to create a new macro
//  class for providing some basic system information, such as host name,
//  IP address, OS version, and so forth.
//
//  Since we don't really need any instance data, i.e. all of the system info
//  we get can be gotten in a static fashion internally, we don't declare our
//  own value class. We just use the TMEngStdClassVal, which is what is used
//  by the macro level classes.
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
//  CLASS: TMEngSysInfoInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngSysInfoInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngSysInfoInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngSysInfoInfo(const TMEngSysInfoInfo&) = delete;

        ~TMEngSysInfoInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngSysInfoInfo& operator=(const TMEngSysInfoInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Init
        (
                    TCIDMacroEngine&        meOwner
        )   override;

        TMEngClassVal* pmecvMakeStorage
        (
            const   TString&                strName
            ,       TCIDMacroEngine&        meOwner
            , const tCIDMacroEng::EConstTypes  eConst
        )   const override;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bInvokeMethod
        (
                    TCIDMacroEngine&        meOwner
            , const TMEngMethodInfo&        methiTarget
            ,       TMEngClassVal&          mecvInstance
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_GetCPUCount;
        tCIDLib::TCard2 m_c2MethId_GetNodeName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngSysInfoInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


