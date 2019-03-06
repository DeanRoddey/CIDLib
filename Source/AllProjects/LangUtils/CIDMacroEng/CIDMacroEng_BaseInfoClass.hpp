//
// FILE NAME: CIDMacroEng_BaseInfoClass.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/24/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_BaseInfoClasses.cpp file,
//  which implements a class that holds some general literals and enum
//  types that are widely used and don't belong to a single other class.
//
//  This guy has no functionality itself, so we just use the standard class
//  value class.
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
//  CLASS: TMEngBaseInfoInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngBaseInfoInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  We define a set of general errors that other classes want to
        //  throw. In order to throw them, they need access to the error ids,
        //  so we make them public. Else we'd end up with a lot of access
        //  methods just to get to them.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ErrIdBadEntryParm;
        tCIDLib::TCard4 c4ErrIdNotOpened;


        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strCreateActsPath();
        static const TString& strEngineErrPath();
        static const TString& strFindResPath();
        static const TString& strFQTypesPath();
        static const TString& strHorzJustifyPath();
        static const TString& strPath();
        static const TString& strRadixPath();
        static const TString& strUpDnClPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngBaseInfoInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        ~TMEngBaseInfoInfo();


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


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TMEngEnumInfo& meciCreateActs() const;

        const TMEngEnumInfo& meciFindRes() const;

        const TMEngEnumInfo& meciFQTypes() const;

        const TMEngEnumInfo& meciMEngErrors() const;

        const TMEngEnumInfo& meciUpDnCl() const;

        tCIDLib::TVoid ThrowErr
        (
            const   tCIDLib::TCard4         c4ToThrow
            , const TString&                strClassPath
            ,       TCIDMacroEngine&        meOwner
        );

        tCIDLib::TVoid ThrowErr1
        (
            const   tCIDLib::TCard4         c4ToThrow
            , const TString&                strClassPath
            ,       TCIDMacroEngine&        meOwner
            , const MFormattable&           mfmtToken
        );


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
        //  Public operators
        // -------------------------------------------------------------------
        TMEngBaseInfoInfo(const TMEngBaseInfoInfo&) = delete;
        TMEngBaseInfoInfo& operator=(const TMEngBaseInfoInfo&) = delete;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2MethId_XXX
        //      The ids of our registered methods.
        //
        //  m_c4Err_XXX
        //      We store the ordinals of some errors so that we can make them
        //      available to the macro engine, which wants to throw some of
        //      them directly.
        //
        //  m_pmeciXXX
        //      The pointers to all of the enumerated types we add to ourself.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_IsInDebugMode;
        TMEngEnumInfo*  m_pmeciCreateActs;
        TMEngEnumInfo*  m_pmeciFindRes;
        TMEngEnumInfo*  m_pmeciFQTypes;
        TMEngEnumInfo*  m_pmeciJustify;
        TMEngEnumInfo*  m_pmeciMEngErrors;
        TMEngEnumInfo*  m_pmeciRadices;
        TMEngEnumInfo*  m_pmeciUpDnCl;



        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngBaseInfoInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


