//
// FILE NAME: CIDBuild_Facility.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/21/1998
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
//  This is the header for the CIDBuild_Facility.Cpp file. This file
//  implements the TFacCIDBuild class, which is the 'pseudio facility class'
//  for this facility. It contains all of the controlling intelligence for
//  the program.
//
// CAVEATS/GOTCHAS:
//
//  1)  Because of the size of this file, the private methods associated
//      with the direct carrying out of the user's actions are implemented
//      in the CIDBuild_FacilityActions.Cpp file.
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TFacCIDBuild
//  PREFIX: fac
// ---------------------------------------------------------------------------
class TFacCIDBuild
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDBuild();

        TFacCIDBuild(const TFacCIDBuild&) = delete;

        ~TFacCIDBuild();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDBuild& operator=(const TFacCIDBuild&) = delete;


        // -------------------------------------------------------------------
        //  Pubic, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bDependsOn
        (
            const   TBldStr&                strSrcProj
            , const TBldStr&                strToCheck
            , const tCIDLib::TBoolean       bMustExist
        );

         tCIDLib::TBoolean TFacCIDBuild::bForce() const
        {
            return m_bForce;
        }

        tCIDLib::TBoolean bGetMacroValue
        (
            const   TBldStr&                strMacroName
            ,       TBldStr&                strMacroValue
        );

        inline tCIDLib::TBoolean bLowPrio() const
        {
            return m_bLowPrio;
        }

        inline tCIDLib::TBoolean bMaxWarn() const
        {
            return m_bMaxWarn;
        }

        inline tCIDLib::TBoolean bNonPermissive() const
        {
            return m_bNonPermissive;
        }

        inline tCIDLib::TBoolean bNoRecurse() const
        {
            return m_bNoRecurse;
        }
        tCIDLib::TBoolean bSingle() const
        {
            return m_bSingle;
        }

         tCIDLib::TBoolean bVerbose() const
        {
            return m_bVerbose;
        }

        tCIDLib::TCard4 c4CPUCount() const
        {
            return m_c4CPUCount;
        }

         tCIDLib::TCard4 c4MajVer() const
        {
            return m_c4MajVer;
        }

         tCIDLib::TCard4 c4MinVer() const
        {
            return m_c4MinVer;
        }

        tCIDLib::TCard4 c4Revision() const
        {
            return m_c4Revision;
        }

        tCIDBuild::EBldModes eBldMode() const;

        tCIDBuild::EHdrDmpModes eHdrDumpMode() const;

        tCIDLib::TVoid GenIDL
        (
            const   TProjectInfo&           projiCur
        );

        tCIDLib::TSInt iMain
        (
                    tCIDLib::TCard4         c4Args
            , const tCIDLib::TCh* const*    apszArgs
        );

        const TList<TBldStr>& listExtLibs() const;

        const TList<TBldStr>& listExtLibPaths() const;

        const TList<TBldStr>& listExtIncludePaths() const;

        const TList<TBldStr>& listIDLMappings() const;

        tCIDLib::TVoid ParseAllProjects
        (
                    TLineSpooler&           lsplSrc
        );

        const TProjectInfo& projiByName
        (
            const   TBldStr&                strName
        )   const;

        const TProjectList& plistProjects() const;

        const TBldStr& strCIDLibSrcDir() const;

        const TBldStr& strIncludeDir() const;

        const TBldStr& strLangSuffix() const;

        const TBldStr& strOutDir() const;

        const TBldStr& strBasePlatform() const;

        const TBldStr& strFullPlatform() const;

        const TBldStr& strPrivIncludeDir() const;

        const TBldStr& strPPIncludeDir() const;

        const TBldStr& strPPPrivIncludeDir() const;

        const TBldStr& strRootDir() const;

        const TBldStr& strVersionSuffix() const;

        const TBldStr& strTarget() const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBuildCallBack
        (
            const   TBldStr&                strTarget
            , const tCIDLib::TCard4         c4Level
        );

        tCIDLib::TBoolean bCopyHeadersCallBack
        (
            const   TBldStr&                strTarget
            , const tCIDLib::TCard4         c4Level
        );

        tCIDLib::TBoolean bCustomCmdsCallBack
        (
            const   TBldStr&        strTarget
            , const tCIDLib::TCard4
        );

        tCIDLib::TBoolean bDumpDependsCallBack
        (
            const   TBldStr&                strTarget
            , const tCIDLib::TCard4         c4Level
        );

        tCIDLib::TBoolean bFileCopiesCallBack
        (
            const   TBldStr&                strTarget
            , const tCIDLib::TCard4         c4Level
        );

        tCIDLib::TBoolean bIDLGenCallBack
        (
            const   TBldStr&                strTarget
            , const tCIDLib::TCard4         c4Level
        );

        tCIDLib::TBoolean bMakeDependsCallBack
        (
            const   TBldStr&                strTarget
            , const tCIDLib::TCard4         c4Level
        );

        tCIDLib::TBoolean bMakeResCallBack
        (
            const   TBldStr&                strTarget
            , const tCIDLib::TCard4         c4Level
        );

        tCIDLib::TVoid Build
        (
            const   TBldStr&                strTarget
        );

        tCIDLib::TVoid CheckEnv();

        tCIDLib::TVoid CopyHeaders
        (
            const   TBldStr&                strTarget
        );

        tCIDLib::TVoid DoFileCopies
        (
            const   TBldStr&                strTarget
        );

        tCIDLib::TVoid DumpDepends
        (
            const   TBldStr&                strTarget
        );

        tCIDLib::TVoid GenIDLFiles
        (
            const   TBldStr&                strTarget
        );

        tCIDLib::TVoid MakeDepends
        (
            const   TBldStr&                strTarget
        );

        tCIDLib::TVoid MakeRelease
        (
            const   tCIDBuild::EActions     eAction
            , const TBldStr&                strTargetDir
        );

        tCIDLib::TVoid MakeResources
        (
            const   TBldStr&                strTarget
        );

        tCIDLib::TVoid MakeTargetDirs();

        tCIDLib::TVoid ParseParms
        (
            const   tCIDLib::TCard4         c4Args
            , const tCIDLib::TCh* const*    apszArgs
        );

        tCIDLib::TVoid ProcessVerParm
        (
            const   tCIDLib::TCh* const     pszVer
        );

        tCIDLib::TVoid RunCustomCmds
        (
            const   TBldStr&                strTarget
        );

        tCIDLib::TVoid ShowLogo();

        tCIDLib::TVoid ShowParms();

        tCIDLib::TVoid ShowUsage();


        // -------------------------------------------------------------------
        //  These are provided by the platform driver
        // -------------------------------------------------------------------
        tCIDLib::TVoid BuildVersionString
        (
                    TBldStr&                strToFill
            , const tCIDLib::TCard4         c4MajVer
            , const tCIDLib::TCard4         c4MinVer
        );

        tCIDLib::TVoid CopyUtilReleaseFiles
        (
            const   TBldStr&                strTargetDir
        );

        tCIDLib::TVoid CopyLibReleaseFiles
        (
            const   TBldStr&                strTargetDir
            , const TProjectInfo&           projiCur
            , const tCIDLib::TBoolean       bCopyLibFile
        );

        tCIDLib::TVoid MakePlatRelease
        (
            const   tCIDBuild::EActions     eAction
            , const TBldStr&                strTargetDir
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bForce
        //      This is set by the /Force flag. It will cause all targets to
        //      appear to be out of date.
        //
        //  m_bLowPrio
        //      The /LowPrio flag on the command line will set this. If its
        //      set, then we exec all development tools with a lower priority
        //      so that it does not interfere with other work.
        //
        //  m_bMaxWarn
        //      This is set by the /MaxWarn flag. It will cause the tools
        //      driver to force on the highest warning level of the compiler.
        //
        //  m_bNonPermissive
        //      Set via the /NonPermissive flag, which can enable tediously strong
        //      checks. There's no way we can turn it on all the time at this point
        //      since it will take forever to get rid of all of the errors. So it's
        //      an option that we can turn on once in a while when we want to work
        //      on this stuff.
        //
        //  m_bNoRecurse
        //      The build will only do the target project and not recurse
        //      through its dependent projects. By default this is false, but
        //      it can be set via the /NoRecurse option.
        //
        //  m_bSingle
        //      Invokes the compiler in single threaded (non-parallel) mode, which
        //      is often more convenient when dealing with errors.
        //
        //  m_bSupressLogo
        //      When this is set, the program blurb will not be shown.
        //
        //  m_bVerbose
        //      This is set by the /Verbose flag. It will cause more info
        //      to be output during processing.
        //
        //  m_c4CPUCount
        //      To avoid constantly making a system call, we query the CPU count
        //      from the utilities and make it available.
        //
        //  m_c4MajVer
        //  m_c4MinVer
        //  m_c4Revision
        //      These are the release version values provided on the command
        //      line. These are fed to the source via command line defines
        //      and are used to create the names of some output files.
        //
        //  m_eAction
        //      The action that we are to perform for this run.
        //
        //  m_eBldMode
        //      The mode that we are working in, either a production or
        //      development build.
        //
        //  m_eHdrDumpMode
        //      This is set by the /HdrDump flag. It will cause us to output
        //      the header output dependency info, i.e. what is included in
        //      what.
        //
        //  m_listExtLibs
        //      These are external libraries that all projects should link
        //      to. They can be fully qualfied, but usually aren't. They are
        //      set via the EXTLIBS= block of the ALLPROJECTS block.
        //
        //  m_listExtLibPaths
        //      These are external lib paths that all projects should use.
        //      They are set via the EXTLIBPATHS= block of the ALLPROJECTS
        //      block.
        //
        //  m_listExtIncludePaths
        //      These are external include paths that all projects should use.
        //      They are set via the EXTINCLUDEPATHS= block of the ALLPROJECTS
        //      block.
        //
        //  m_listIDLMappings
        //      The list of XML entity mappings to be applied to all IDL
        //      files. They can add others, but these are applied to all.
        //
        //  m_listMacros
        //      A list of key/value pairs that are set via the MACROS= sub
        //      block of the ALLPROJECTS= block of the projects file.
        //
        //  m_pplistProjs
        //      The project list object, which contains all of the data
        //      about projects, their settings, their dependencies, etc...
        //      This is the object that parses the .Projects file.
        //
        //      !It is dynamically allocated because its init will require
        //      command line stuff that would not be parsed out during
        //      the facility object's construction.
        //
        //  m_ptdrvBuild
        //      The tools driver that is used to do builds. It abstracts all
        //      of the actions of compiling and linking. The correct one is
        //      created at construction time with a little conditional code.
        //
        //  m_strCIDLibSrcDir
        //      This is set via the CIDLIB_SRCDIR environment variable and
        //      points to the top level of the CIDLib tree (or to the
        //      packaged CIDLib system if not working against the original
        //      source.) It's redundant in a way when building CIDLib itself,
        //      since the RootDir value below is pointing to the same place.
        //      But when building CIDLib based stuff, it lets us get to
        //      some things in CIDLib that we need.
        //
        //  m_strLangSuffix
        //      This is the suffix for the target language, which is used
        //      to figure out which version of the message files to compile.
        //
        //  m_strIncludeDir
        //  m_strPrivIncludeDir
        //  m_strPPIncludeDir
        //  m_strPPPrivIncludeDir
        //      These are the include directories to which all headers are
        //      are copied. They are built from the root directory and the
        //      build mode. The PP versions are the per-platform versions and
        //      are just the other versions plus the per-platform irectory.
        //
        //  m_strOutDir
        //      The main output directory. Comes from the environment variable
        //      CID_RESDIR. It is required and is guaranteed to end with a slash.
        //
        //  m_strBasePlatform
        //  m_strFullPlatform
        //      These are set from the constants that the per-platform _Platform.hpp
        //      files create based on the CID_PLATFORM environment variable.
        //
        //  m_strRootDir
        //      This is filled with the root directory of the build tree.
        //      It is required, and is guaranteed to end with a slash.
        //
        //  m_strTarget
        //      This is filled with the target facility to build. If there
        //      was not one, its set to nul, which implies build everything.
        //
        //  m_strVersionSuffix
        //      The version string for the version we were told to do. This
        //      is the version for appending to 'versioned' filed names. It
        //      is set by the platform, since it varies by platform. It should
        //      in some way include the major and minor version, but not the
        //      revision!
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bForce;
        tCIDLib::TBoolean       m_bLowPrio;
        tCIDLib::TBoolean       m_bMaxWarn;
        tCIDLib::TBoolean       m_bNonPermissive;
        tCIDLib::TBoolean       m_bNoRecurse;
        tCIDLib::TBoolean       m_bSingle;
        tCIDLib::TBoolean       m_bSupressLogo;
        tCIDLib::TBoolean       m_bVerbose;
        tCIDLib::TCard4         m_c4CPUCount;
        tCIDLib::TCard4         m_c4MajVer;
        tCIDLib::TCard4         m_c4MinVer;
        tCIDLib::TCard4         m_c4Revision;
        tCIDBuild::EActions     m_eAction;
        tCIDBuild::EBldModes    m_eBldMode;
        tCIDBuild::EHdrDmpModes m_eHdrDumpMode;
        TList<TBldStr>          m_listExtLibs;
        TList<TBldStr>          m_listExtLibPaths;
        TList<TBldStr>          m_listExtIncludePaths;
        TList<TBldStr>          m_listIDLMappings;
        TList<TKeyValuePair>    m_listMacros;
        TProjectList*           m_pplistProjs;
        TBldStr                 m_strCIDLibSrcDir;
        TBldStr                 m_strIncludeDir;
        TBldStr                 m_strLangSuffix;
        TBldStr                 m_strOutDir;
        TBldStr                 m_strBasePlatform;
        TBldStr                 m_strFullPlatform;
        TBldStr                 m_strPrivIncludeDir;
        TBldStr                 m_strPPIncludeDir;
        TBldStr                 m_strPPPrivIncludeDir;
        TBldStr                 m_strRootDir;
        TBldStr                 m_strTarget;
        TToolsDriver*           m_ptdrvBuild;
        TBldStr                 m_strVersionSuffix;
};


// ---------------------------------------------------------------------------
//  TFacCIDBuild: Public, non-virtual methods
// ---------------------------------------------------------------------------
inline tCIDBuild::EBldModes TFacCIDBuild::eBldMode() const
{
    return m_eBldMode;
}

inline tCIDBuild::EHdrDmpModes TFacCIDBuild::eHdrDumpMode() const
{
    return m_eHdrDumpMode;
}

inline const TList<TBldStr>& TFacCIDBuild::listExtLibs() const
{
    return m_listExtLibs;
}

inline const TList<TBldStr>& TFacCIDBuild::listExtLibPaths() const
{
    return m_listExtLibPaths;
}

inline const TList<TBldStr>& TFacCIDBuild::listExtIncludePaths() const
{
    return m_listExtIncludePaths;
}

inline const TList<TBldStr>& TFacCIDBuild::listIDLMappings() const
{
    return m_listIDLMappings;
}

inline const TProjectInfo&
TFacCIDBuild::projiByName(const TBldStr& strName) const
{
    // Just pass through to the project list
    return m_pplistProjs->projiByName(strName);
}

inline const TProjectList& TFacCIDBuild::plistProjects() const
{
    return *m_pplistProjs;
}

inline const TBldStr& TFacCIDBuild::strCIDLibSrcDir() const
{
    return m_strCIDLibSrcDir;
}

inline const TBldStr& TFacCIDBuild::strLangSuffix() const
{
    return m_strLangSuffix;
}

inline const TBldStr& TFacCIDBuild::strIncludeDir() const
{
    return m_strIncludeDir;
}

inline const TBldStr& TFacCIDBuild::strPPIncludeDir() const
{
    return m_strPPIncludeDir;
}

inline const TBldStr& TFacCIDBuild::strOutDir() const
{
    return m_strOutDir;
}

inline const TBldStr& TFacCIDBuild::strBasePlatform() const
{
    return m_strBasePlatform;
}

inline const TBldStr& TFacCIDBuild::strFullPlatform() const
{
    return m_strFullPlatform;
}

inline const TBldStr& TFacCIDBuild::strPrivIncludeDir() const
{
    return m_strPrivIncludeDir;
}

inline const TBldStr& TFacCIDBuild::strPPPrivIncludeDir() const
{
    return m_strPPPrivIncludeDir;
}

inline const TBldStr& TFacCIDBuild::strRootDir() const
{
    return m_strRootDir;
}

inline const TBldStr& TFacCIDBuild::strTarget() const
{
    // Either target project, or target directory for release actions
    return m_strTarget;
}

inline const TBldStr& TFacCIDBuild::strVersionSuffix() const
{
    return m_strVersionSuffix;
}

