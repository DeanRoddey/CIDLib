//
// FILE NAME: CIDBuild_IDLInfo.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/27/2001
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
//  This is the header for the CIDBuild_IDLInfo.cpp file. This file implements
//  the small class, TIDLInfo, which is used to hold the information that is
//  provided in the project file for each IDL file that needs to be compiled.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


class TProjectInfo;
class TLineSpooler;


// ---------------------------------------------------------------------------
//   CLASS: TIDLInfo
//  PREFIX: idli
// ---------------------------------------------------------------------------
class TIDLInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TIDLInfo();

        TIDLInfo
        (
            const   TIDLInfo&               idliToCopy
        );

        ~TIDLInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TIDLInfo& operator=
        (
            const   TIDLInfo&               idliToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bGenClient() const;

        tCIDLib::TBoolean bGenSharp() const;

        tCIDLib::TBoolean bGenGlobals() const;

        tCIDLib::TBoolean bGenServer() const;

        tCIDLib::TBoolean bGenTypeScript() const;

        tCIDLib::TVoid GenerateCode
        (
            const   TProjectInfo&           projiSrc
        );

        tCIDLib::TVoid Parse
        (
                    TLineSpooler&           lsplSource
        );

        const TBldStr& strNameExt() const;

        const TBldStr& strSrcFile() const;

        const TBldStr& strTSOutPath() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bGenClient
        //  m_bGenGlobals
        //  m_bGenServer
        //      These indicate which bits of output the project asked to have generated for
        //      this source IDL file.
        //
        //  m_bGenCSharp
        //  m_bGenTypeScript
        //      Indicate which languages, beside C++ which is always done, that output
        //      files should be generated for. Only globals are done for languages other
        //      that C++ at this time. And, with the globals as well, in some cases some
        //      things are not generated if the language cannot handle it.
        //
        //  m_listMappings
        //      A list of entity id mappings that we pass to the IDL compiler.
        //
        //  m_strNameExt
        //      This is the name extension used for generating output files.
        //
        //  m_strSrcFile
        //      This is the source CIDIDL file to parse.
        //
        //  m_strTSOutPath
        //      If generating type script, indicates the output path for the file
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bGenClient;
        tCIDLib::TBoolean       m_bGenCSharp;
        tCIDLib::TBoolean       m_bGenGlobals;
        tCIDLib::TBoolean       m_bGenServer;
        tCIDLib::TBoolean       m_bGenTypeScript;
        TList<TBldStr>          m_listMappings;
        TBldStr                 m_strNameExt;
        TBldStr                 m_strSrcFile;
        TBldStr                 m_strTSOutPath;
};

