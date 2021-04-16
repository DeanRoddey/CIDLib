//
// FILE NAME: CIDMacroEng_FileResolver.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/15/2003
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
//  This is the header file for the CIDMacroEng_FileResolver.cpp file, which
//  implements the MMEngFileResolver class. This is a pure abstract base class,
//  intended to be used as a mixin, that allows individual implementations to
//  plug in a means of converting macro file paths into file system paths.
//
//  We supply a prefab derivative that supports the very common scenario of
//  just basing the macro file hierarchy under some fixed disk directory.
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
//  CLASS: MMEngFileResolver
// PREFIX: mefr
// ---------------------------------------------------------------------------
class CIDMACROENGEXP MMEngFileResolver
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        MMEngFileResolver(const MMEngFileResolver&) = delete;

        ~MMEngFileResolver();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        MMEngFileResolver& operator=(const MMEngFileResolver&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid ContractPath
        (
            const   TString&                strOSFilePath
            ,       TPathStr&               pathToFill
        ) = 0;

        virtual tCIDLib::TVoid ExpandPath
        (
            const   TString&                strMacroFilePath
            ,       TPathStr&               pathToFill
        ) = 0;



    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        MMEngFileResolver();

};


// ---------------------------------------------------------------------------
//  CLASS: TMEngFixedBaseFileResolver
// PREFIX: mepr
// ---------------------------------------------------------------------------
class CIDMACROENGEXP TMEngFixedBaseFileResolver

    : public TObject, public MMEngFileResolver
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngFixedBaseFileResolver();

        TMEngFixedBaseFileResolver
        (
            const   TString&                strBasePath
        );

        TMEngFixedBaseFileResolver
        (
            const   TString&                strBasePath
            , const TString&                strSubDir
        );

        TMEngFixedBaseFileResolver(const TMEngFixedBaseFileResolver&) = delete;

        ~TMEngFixedBaseFileResolver();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngFixedBaseFileResolver& operator=(const TMEngFixedBaseFileResolver&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ContractPath
        (
            const   TString&                strOSFilePath
            ,       TPathStr&               pathToFill
        )   override;

        tCIDLib::TVoid ExpandPath
        (
            const   TString&                strMacroFilePath
            ,       TPathStr&               pathToFill
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TString& strBasePath() const;

        const TString& strBasePath
        (
            const   TString&                strToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strBasePath
        //      This is the base path to which file paths are appended.
        // -------------------------------------------------------------------
        TString     m_strBasePath;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngFixedBaseFileResolver,TObject)
};

#pragma CIDLIB_POPPACK



