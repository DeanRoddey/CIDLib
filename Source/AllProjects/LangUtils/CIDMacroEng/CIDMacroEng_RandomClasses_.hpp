//
// FILE NAME: CIDMacroEng_RandomClasses.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/02/2007
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
//  This is the header file for the CIDMacroEng_RandomClasses.cpp file,
//  which implements the info/value derivatives needed to create random
//  number generator CML class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TRandomNum;

// ---------------------------------------------------------------------------
//  CLASS: TMEngRandomNumVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngRandomNumVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngRandomNumVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngRandomNumVal(const TMEngRandomNumVal&) = delete;

        ~TMEngRandomNumVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngRandomNumVal& operator=(const TMEngRandomNumVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4NextVal
        (
            const   tCIDLib::TCard4         c4Modulus
        );

        tCIDLib::TVoid Seed
        (
            const   tCIDLib::TCard4         c4Seed
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_prandValue
        //      The random number generator from CIDMath. We use a pointer so
        //      that we don't force CIDMath on our clients.
        // -------------------------------------------------------------------
        TRandomNum* m_prandValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngRandomNumVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngRandomNumInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngRandomNumInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngRandomNumInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngRandomNumInfo(const TMEngRandomNumInfo&) = delete;

        ~TMEngRandomNumInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngRandomNumInfo& operator=(const TMEngRandomNumInfo&) = delete;


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
        //      To speed up dispatching, which we handle at the class level
        //      since this is a fully 'external' class, we store the ids of
        //      our methods as we register them.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_GetNextVal;
        tCIDLib::TCard2 m_c2MethId_Seed;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngRandomNumInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK

