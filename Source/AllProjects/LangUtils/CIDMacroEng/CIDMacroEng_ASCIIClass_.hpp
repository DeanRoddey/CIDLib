//
// FILE NAME: CIDMacroEng_ACIIClass_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/30/2003
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
//  This is the header file for the CIDMacroEng_ASCIIClass.cpp file, which
//  implements the info class that implements the MEng.ASCII macro level
//  class. This class needs no C++ level data members, or macro level ones
//  for that matter, so we just use the standard value class. Mainly this
//  class provides a set of literals and conversion methods.
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
//  CLASS: TMEngASCIIInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngASCIIInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngASCIIInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngASCIIInfo(const TMEngASCIIInfo&) = delete;

        ~TMEngASCIIInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngASCIIInfo& operator=(const TMEngASCIIInfo&) = delete;


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



        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const MFormattable&           fmtblToken
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      To speed up dispatching, which we handle at the class level
        //      since this is a fully 'external' class, we store the ids of
        //      our methods as we register them.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2EnumId_Errors;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_FromDecDigit;
        tCIDLib::TCard2 m_c2MethId_FromHexDigit;
        tCIDLib::TCard2 m_c2MethId_IsDecDigit;
        tCIDLib::TCard2 m_c2MethId_IsHexDigit;
        tCIDLib::TCard2 m_c2MethId_ToDecDigit;
        tCIDLib::TCard2 m_c2MethId_ToHexDigit;
        tCIDLib::TCard4 m_c4Err_NotDecDig;
        tCIDLib::TCard4 m_c4Err_NotHexDig;
        tCIDLib::TCard4 m_c4Err_Overflow;
        TMEngEnumInfo*  m_pmeciErrors;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngASCIIInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK



