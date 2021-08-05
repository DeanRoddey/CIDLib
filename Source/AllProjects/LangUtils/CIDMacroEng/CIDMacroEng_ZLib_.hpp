//
// FILE NAME: CIDMacroEng_ZLibClass_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/311/2021
//
// COPYRIGHT: Charmed Quark Systems, Ltd @ 2021
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and
//  the author (Dean Roddey.) It is licensed under the MIT Open Source
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_ZLibClass.cpp file, which
//  implements the info and value classes required to create a new macro
//  class, in this case a wrapper around the underlying C++ ZLib support.
//
//  It implements the MEng.System.Runtime.ZLib class.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

// Only need this internally
class TZLibCompressor;

#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TMEngZLibVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngZLibVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngZLibVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngZLibVal(const TMEngZLibVal&) = delete;

        ~TMEngZLibVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngZLibVal& operator=(const TMEngZLibVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Compress
        (
            const   TMemBuf&                mbufSrc
            ,       TMemBuf&                mbufTar
            , const tCIDLib::TCard4         c4SrcCount
        );

        tCIDLib::TCard4 c4Decompress
        (
            const   TMemBuf&                mbufSrc
            ,       TMemBuf&                mbufTar
            , const tCIDLib::TCard4         c4SrcCount
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_pzlibImpl
        //      Just needed internally, so we just use a pointer.
        // -------------------------------------------------------------------
        TZLibCompressor*    m_pzlibImpl;

        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngZLibVal,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngZLibInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngZLibInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngZLibInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngZLibInfo(const TMEngZLibInfo&) = delete;

        ~TMEngZLibInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngZLibInfo& operator=(const TMEngZLibInfo&) = delete;


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
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            ,       TError&                 errCaught
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c2EnumId_XXX
        //      We create some enum nested types, so we store the ids here
        //      for quick access later.
        //
        //  m_c2MethId_XXX
        //      To speed up dispatch, we store the ids of the methods that
        //      we set up during init.
        //
        //  m_c4ErrBadSet
        //      We have an error enum, so we store the ordinals so that we
        //      can throw macro level errors internally if needed.
        // -------------------------------------------------------------------
        tCIDLib::TCard2 m_c2EnumId_Errors;
        tCIDLib::TCard2 m_c2MethId_DefCtor;
        tCIDLib::TCard2 m_c2MethId_Compress;
        tCIDLib::TCard2 m_c2MethId_Decompress;
        tCIDLib::TCard4 m_c4ErrCompFailed;
        tCIDLib::TCard4 m_c4ErrDecompFailed;
        TMEngEnumInfo*  m_pmeciErrors;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngZLibInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK
