//
// FILE NAME: CIDBuild_ProjectInfo.Hpp
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
//  This is the header for the CIDBuild_ProjectInfo.Cpp file. This file
//  implements the TProjectInfo class, which holds the platform independent
//  form of the settings for a project. It also holds other information
//  gathered about the project during processing, such as a list of all of
//  the Cpp/Hpp/H files that make up the source files of the project, the
//  index of this project in the dependency graph, etc...
//
//  We define a simple class to hold a file copy block. A project can define
//  a set of file copy blocks, each of which provides a target path (relative
//  to the project output, or absolute) and a list of files (relative to the
//  project directory) to copy there.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TProjFileCopy
//  PREFIX: pfc
// ---------------------------------------------------------------------------
class TProjFileCopy
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TProjFileCopy
        (
            const   TBldStr&                strOutPath
        );

        ~TProjFileCopy();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddSrcFile
        (
            const   TBldStr&                strToAdd
        );

        const TList<TBldStr>& listSrcFiles() const;

        tCIDLib::TVoid RemoveAll();

        const TBldStr& strOutPath() const;

        TBldStr& strOutPath
        (
            const   TBldStr&                strToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_llstSrcFiles
        //      The list of source project files to copy
        //
        //  m_strOutPath
        //      The copy to path
        // -------------------------------------------------------------------
        TList<TBldStr>  m_listSrcFiles;
        TBldStr         m_strOutPath;
};


// ---------------------------------------------------------------------------
//   CLASS: TProjectInfo
//  PREFIX: proji
// ---------------------------------------------------------------------------
class TProjectInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TProjectInfo() = delete;

        TProjectInfo
        (
            const   TBldStr&                strName
            , const tCIDBuild::TStrList&    listPlatIncl
            , const tCIDBuild::TStrList&    listPlatExcl
        );

        TProjectInfo(const TProjectInfo&) = delete;

        ~TProjectInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TProjectInfo& operator=(const TProjectInfo&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddToDepGraph
        (
                    TDependGraph&           depgTarget
        );

        tCIDLib::TBoolean bDefineExists
        (
            const   TBldStr&                strToFind
        )   const;

        tCIDLib::TBoolean bHasIDLFiles() const;

        tCIDLib::TBoolean bHasMsgFile() const;

        tCIDLib::TBoolean bHasResFile() const;

        tCIDLib::TBoolean bIsSample() const;

        tCIDLib::TBoolean bMakeOutDir() const;

        tCIDLib::TBoolean bNeedsAdminPrivs() const;

        tCIDLib::TBoolean bPlatformDir() const;

        tCIDLib::TBoolean bPureCpp() const;

        tCIDLib::TBoolean bSupportsThisPlatform() const;

        tCIDLib::TBoolean bSupportsPlatform
        (
            const   TBldStr&                strToCheck
        )   const;

        tCIDLib::TBoolean bUseSysLibs() const;

        tCIDLib::TBoolean bUsesExtLib
        (
            const   TBldStr&                strToCheck
        )   const;

        tCIDLib::TBoolean bVarArgs() const;

        tCIDLib::TBoolean bVersioned() const;

        tCIDLib::TCard4 c4Base() const;

        tCIDLib::TCard4 c4CppCount() const;

        tCIDLib::TCard4 c4DefCount() const;

        tCIDLib::TCard4 c4DepCount() const;

        tCIDLib::TCard4 c4DepIndex() const;

        tCIDLib::TCard4 c4HppCount() const;

        tCIDLib::TVoid DumpSettings() const;

        tCIDBuild::EDisplayTypes eDisplayType() const;

        tCIDBuild::EProjTypes eType() const;

        const TList<TFindInfo>& listCpps() const;

        const TList<TBldStr>& listCustomCmds() const;

        const TList<TBldStr>& listDeps() const;

        const TList<TProjFileCopy>& listFileCopies() const;

        const TList<TBldStr>& listExtLibs() const;

        const TList<TFindInfo>& listHpps() const;

        const TList<TIDLInfo>& listIDLFiles() const;

        const TList<TBldStr>& listIncludePaths() const;

        tCIDLib::TVoid LoadFileLists();

        const TKeyValuePair* pkvpFindOption
        (
            const   TBldStr&                strOption
        )   const;

        tCIDLib::TVoid ParseContent
        (
                    TLineSpooler&           lsplSource
        );

        const TBldStr& strDirectory() const;

        const TBldStr& strExportKeyword() const;

        const TBldStr& strMsgSrc() const;

        const TBldStr& strResSrc() const;

        const TBldStr& strOutDir() const;

        const TBldStr& strOutErrHpp() const;

        const TBldStr& strOutMsgHpp() const;

        const TBldStr& strOutMsgs() const;

        const TBldStr& strOutBin() const;

        const TBldStr& strOutResHpp() const;

        const TBldStr& strOutRes() const;

        const TBldStr& strProjectName() const;

        const TBldStr& strProjectDir() const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBlockForThisPlatform
        (
                    TLineSpooler&           lsplSource
            , const TBldStr&                strPlatforms
            , const TBldStr&                strEndBlock
            , const tCIDLib::TCh* const     pszExpected
        );

        tCIDLib::TBoolean bSetSetting
        (
            const   TBldStr&                strName
            , const TBldStr&                strValue
        );

        tCIDLib::TVoid ParseCustCmds
        (
                    TLineSpooler&           lsplSource
        );

        tCIDLib::TVoid ParseDefines
        (
                    TLineSpooler&           lsplSource
        );

        tCIDLib::TVoid ParseDependents
        (
            const   TBldStr&                strOptions
            ,       TLineSpooler&           lsplSource
        );

        tCIDLib::TVoid ParseExtLibs
        (
                    TLineSpooler&           lsplSource
        );

        tCIDLib::TVoid ParseFileCopies
        (
                    TLineSpooler&           lsplSource
            , const TBldStr&                strTarPath
        );

        tCIDLib::TVoid ParseIDLFiles
        (
                    TLineSpooler&           lsplSource
        );

        tCIDLib::TVoid ParseIncludePaths
        (
            const   TBldStr&                strOptions
            ,       TLineSpooler&           lsplSource
        );

        tCIDLib::TVoid ParseInitLine
        (
            const   TBldStr&                strList
            , const tCIDLib::TCard4         c4Line
        );

        tCIDLib::TVoid ParseOptions
        (
            const   TBldStr&                strPlatforms
            ,       TLineSpooler&           lsplSource
        );

        tCIDLib::TVoid ParseSettings
        (
                    TLineSpooler&           lsplSource
        );


        // -------------------------------------------------------------------
        //  Private, non-virtual platform dependent methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid BuildOutputFileName();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bIsSample
        //      Indicates if this project is a sample project that should go out in
        //      the developer's release.
        //
        //  m_bMsgFile
        //      This project has a message file that must be built. It is assumed to
        //      have the same name as the project and the standard extension of MsgText.
        //
        //  m_bNeedsAdminPrivs
        //      Indicates that the program requires administrative privs, which can
        //      be used if required by the platform specific handlers to generate
        //      special build stuff to support that.
        //
        //  m_bPlatformDir
        //      This project has a per-platform directory underneath it, so
        //      the files in that directory must be part of the project.
        //      The name of the per-platform directory is a constant from
        //      kCIDBuild, which is set by conditional compilation for each
        //      platform.
        //
        //  m_bPureCpp
        //      This is defaulted to enabled, but can be disabled for lower level
        //      stuff that has to import system headers that will require some sort
        //      of permissive mode or suppression of errors.
        //
        //  m_bResFile
        //      This project has a resource file that must be built. It is
        //      assumed to have the same name as the project and the standard
        //      extension of CIDRC.
        //
        //  m_bUseSysLibs
        //      Indicates that this project needs to have the standard set
        //      of host OS libs linked in. Only for core low level projects
        //      generally.
        //
        //  m_bVarArgs
        //      Indicates whether this project needs to support the standard
        //      C++ variable argument support. This usually involves linking
        //      in some object file, but its different per development
        //      environment.
        //
        //  m_bVersioned
        //      Indicates that the distributable output files of this project
        //      are postfixed with the release version string. Only the
        //      end client user files, i.e. DLL and CIDMsg, are versioned
        //      so that multiple versions can be installed in the same dir.
        //
        //  m_eDisplayMode
        //      The type of display this project has, which is N/A for DLLs,
        //      and is either GUI or Console for Exes.
        //
        //  m_eType
        //      The type of project this is, which tells the platform driver what
        //      types of switches are required.
        //
        //  m_listCpps
        //      The list of Cpp files that are part of this project. They are relative
        //      to the project directory.
        //
        //  m_listCustomCmds
        //      We support a list of custom commands, just new line separated. This list
        //      holds those commands. They are just passed to the command processor to
        //      be run. These are run after any compile/link steps.
        //
        //  m_listDefs
        //      This list of project specific defines for this project.
        //      Usually there won't be, but sometimes there is a need for
        //      a couple that are only passed to that particular project.
        //      Its elements are the standard key/value pair class, which is
        //      exactly what we need here.
        //
        //  m_listDeps
        //      The names of the other projects upon which this project is
        //      dependent (build before and link to.)
        //
        //  m_listExtLibs
        //      This is the list of external libs to link in. They can be either
        //      fully qualified paths, or just libs that will be found via the
        //      lib path (set externally.)
        //
        //  m_listFileCopies
        //      A list of filess that the project indicated should be copied
        //      from (relative to) the project to some place (relative to) the
        //      output directory.
        //
        //  m_listHpps
        //      The list of Hpp files that are part of this project. They are
        //      relative to the project directory.
        //
        //  m_listIDLFiles
        //      If this project has any IDL source files, this list will have
        //      an IDLInfo object for each source IDL.
        //
        //  m_listIncludePaths
        //      Any include paths needed for this project beyond the standard
        //      stuff that the framework always sets up. C++ includes are
        //      assumed to be set up in the environment. From the
        //      EXTINCLUDEPATHS block.
        //
        //  m_listOptions
        //      A list of key/value pairs that contain options that apply to the current
        //      platform.
        //
        //  m_listPlatformsIncl
        //  m_listPlatformsExcl
        //      Lists of platforms to explicitly include or exclude. By default both
        //      are empty and this project applies to all platforms. See the
        //      TUtils::bSupportsPlatform() method for a description of how these
        //      work together.
        //
        //  m_listTmpX
        //      For local use during parsing to avoid creating and destroying them all
        //      the time.
        //
        //  m_strDirectory
        //      The directory under the AllProjects directory for this project. Usually
        //      the same as the project name, but not always.
        //
        //  m_strExportKeyword
        //      This is the export keyword used by this project. We use this anywhere in
        //      the build process that we are expected to insert an export keyword.
        //
        //  m_strMsgSrc
        //      The path to the source message text file, if there is one.
        //
        //  m_strResSrc
        //      The path to the source resource text file, if there is one.
        //
        //  m_strOutBin
        //      The main target output file, i.e. the executable or library
        //      that is going to be built (or for file copy projects the
        //      target directory for the copied files.)
        //
        //  m_strOutDir
        //      The path to the output directory for this project. This is
        //      the directory under the output directory where this project's
        //      temporary files go.
        //
        //  m_strOutErrHpp
        //      The path to the header file for error ids, if this project
        //      has a message file.
        //
        //  m_strOutMsgHpp
        //      The path to the header file for message ids, if this project
        //      has a message file.
        //
        //  m_strOutMsgs
        //      The output binary message file, if this project has a message
        //      file.
        //
        //  m_strOutRes
        //  m_strOutResHpp
        //      The output binary resource file and the header for its resource
        //      ids, if this project has any resources.
        //
        //  m_strProjectName
        //      The name of the project. This drives a lot of stuff in the
        //      build process. All of the main project files have this as
        //      the base part of the name.
        //
        //  m_strProjectDir
        //      The full path to the project. This is used a lot so it makes
        //      sense to set it up once and keep it around.
        //
        //  m_c4Base
        //      This field indicates whether this project should be based
        //      at a particular address. Its not the actual address, which
        //      would be platform dependent. Its just a number, from 0 to
        //      whatever, which indicates the position. 0 means unbased, and
        //      all others indicate a based position. The platform driver
        //      will select the correct overall base and spacing for each
        //      such project. Some platforms might not support this, in which
        //      case its ignored.
        //
        //  m_c4DepIndex
        //      This project's index in the dependency graph. This is stored
        //      here in order to allow for much faster access to the
        //      dependency information. It avoids the need for a name lookup.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bIsSample;
        tCIDLib::TBoolean           m_bMsgFile;
        tCIDLib::TBoolean           m_bNeedsAdminPrivs;
        tCIDLib::TBoolean           m_bPlatformDir;
        tCIDLib::TBoolean           m_bPureCpp;
        tCIDLib::TBoolean           m_bResFile;
        tCIDLib::TBoolean           m_bUseSysLibs;
        tCIDLib::TBoolean           m_bVarArgs;
        tCIDLib::TBoolean           m_bVersioned;
        tCIDBuild::EDisplayTypes    m_eDisplayType;
        tCIDBuild::EProjTypes       m_eType;
        TList<TFindInfo>            m_listCpps;
        tCIDBuild::TStrList         m_listCustomCmds;
        tCIDBuild::TKVPList         m_listDefs;
        tCIDBuild::TStrList         m_listDeps;
        tCIDBuild::TStrList         m_listExtLibs;
        TList<TProjFileCopy>        m_listFileCopies;
        TList<TFindInfo>            m_listHpps;
        TList<TIDLInfo>             m_listIDLFiles;
        tCIDBuild::TStrList         m_listIncludePaths;
        tCIDBuild::TKVPList         m_listOptions;
        tCIDBuild::TStrList         m_listPlatformsExcl;
        tCIDBuild::TStrList         m_listPlatformsIncl;
        tCIDBuild::TStrList         m_listTmp1;
        tCIDBuild::TStrList         m_listTmp2;
        TBldStr                     m_strCopyOutDir;
        TBldStr                     m_strDirectory;
        TBldStr                     m_strExportKeyword;
        TBldStr                     m_strMsgSrc;
        TBldStr                     m_strResSrc;
        TBldStr                     m_strOutBin;
        TBldStr                     m_strOutDir;
        TBldStr                     m_strOutErrHpp;
        TBldStr                     m_strOutMsgHpp;
        TBldStr                     m_strOutMsgs;
        TBldStr                     m_strOutRes;
        TBldStr                     m_strOutResHpp;
        TBldStr                     m_strProjectName;
        mutable TBldStr             m_strProjectDir;
        tCIDLib::TCard4             m_c4Base;
        tCIDLib::TCard4             m_c4DepIndex;
};

