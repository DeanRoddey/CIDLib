//
// FILE NAME: CIDMacroEng_BinFileClass_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/25/2005
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
//  This is the header file for the CIDMacroEng_BinFileClass.cpp file, which
//  implements the info and value classes required to create a new macro
//  class, in this case a wrapper around the TBinaryFile C++ class, which
//  provides random access to disk files.
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
//  CLASS: TMEngBinFileVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngBinFileVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngBinFileVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngBinFileVal(const TMEngBinFileVal&) = delete;

        ~TMEngBinFileVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngBinFileVal& operator=(const TMEngBinFileVal&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDbgFormat
        (
                    TTextOutStream&         strmTarget
            , const TMEngClassInfo&         meciThis
            , const tCIDMacroEng::EDbgFmts     eFormat
            , const tCIDLib::ERadices       eRadix
            , const TCIDMacroEngine&        meOwner
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TBinaryFile& bflValue();

        tCIDLib::TVoid Close();

        tCIDLib::TVoid Open
        (
            const   TString&                strFileName
            , const tCIDLib::EAccessModes   eAccess
            , const tCIDLib::ECreateActs    eAction
            , const tCIDLib::EFilePerms     ePerms
            , const tCIDLib::EFileFlags     eFlags
        );


    private :
        // -------------------------------------------------------------------
        //  Private, data members
        //
        //  m_bflValue
        //      This is the binary file object we work in terms of.
        // -------------------------------------------------------------------
        TBinaryFile m_bflValue;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngBinFileVal,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngBinFileInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngBinFileInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngBinFileInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        ~TMEngBinFileInfo();


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
        //  Public operators
        // -------------------------------------------------------------------
        TMEngBinFileInfo(const TMEngBinFileInfo&) = delete;
        TMEngBinFileInfo& operator=(const TMEngBinFileInfo&) = delete;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckIsOpen
        (
                    TCIDMacroEngine&        meOwner
            , const TBinaryFile&            bflToCheck
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const TString&                strText
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
        //  m_c4ErrXXX
        //      We have an error enum, so we store the ordinals so that we
        //      can throw macro level errors internally if needed.
        //
        //  m_pmeciErrors
        //      A pointer the error enum we create for our errors.
        //
        //  m_pmeciCreateActs
        //      We lookup some enums that we use in params, so that we can
        //      quickly get access to their values.
        // -------------------------------------------------------------------
        tCIDLib::TCard2     m_c2EnumId_Errors;
        tCIDLib::TCard2     m_c2MethId_Close;
        tCIDLib::TCard2     m_c2MethId_DefCtor;
        tCIDLib::TCard2     m_c2MethId_Flush;
        tCIDLib::TCard2     m_c2MethId_GetCurPos;
        tCIDLib::TCard2     m_c2MethId_GetCurSize;
        tCIDLib::TCard2     m_c2MethId_GetIsOpen;
        tCIDLib::TCard2     m_c2MethId_GetLastAcc;
        tCIDLib::TCard2     m_c2MethId_GetLastMod;
        tCIDLib::TCard2     m_c2MethId_OffsetCurPos;
        tCIDLib::TCard2     m_c2MethId_Open;
        tCIDLib::TCard2     m_c2MethId_ReadBuffer;
        tCIDLib::TCard2     m_c2MethId_SetCurPos;
        tCIDLib::TCard2     m_c2MethId_TruncateAt;
        tCIDLib::TCard2     m_c2MethId_WriteBuffer;
        tCIDLib::TCard4     m_c4ErrCloseFailed;
        tCIDLib::TCard4     m_c4ErrGenErr;
        tCIDLib::TCard4     m_c4ErrOpenFailed;
        tCIDLib::TCard4     m_c4ErrNotOpen;
        tCIDLib::TCard4     m_c4ErrReadFailed;
        tCIDLib::TCard4     m_c4ErrWriteFailed;
        TMEngEnumInfo*      m_pmeciErrors;
        TMEngEnumInfo*      m_pmeciCreateActs;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngBinFileInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


