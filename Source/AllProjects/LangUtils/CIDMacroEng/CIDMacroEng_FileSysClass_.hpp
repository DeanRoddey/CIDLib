//
// FILE NAME: CIDMacroEng_FileSysClass.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/25/2005
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header file for the CIDMacroEng_FileSysClass.cpp file, which
//  implements some file system related CML wrappers around underlying C++
//  classes.
//
//  The FileSystem class provides the usual stuff for seeing if files or
//  directories exist, making and removing sub-directories, simple file
//  system iteration, and so forth. Since there is no per-instance data
//  associated with the underlying C++ TFileSys stuff, our value class is
//  just a dummy placeholder, where we might put some instance stuff later
//  if needed.
//
//  The other is the DirIter class, which supports incremental directory
//  iteration, instead of just getting them all back at once.
//
//  Note that, in both cases, all class paths are relative to a root path
//  set in the macro engine. This limits all CML file access to a specific
//  directory tree. So all CML paths start with \ and have no volume or
//  drive component.
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
//  CLASS: TMEngDirIterVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngDirIterVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngDirIterVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngDirIterVal(const TMEngDirIterVal&) = delete;

        ~TMEngDirIterVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngDirIterVal& operator=(const TMEngDirIterVal&) = delete;


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
        const TDirIter& diterValue() const;

        TDirIter& diterValue();

        const TFindBuf& fndbTmp() const;

        TFindBuf& fndbTmp();



    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_diterValue
        //      This is the underlying C++ class that provides the directory
        //      iteration.
        //
        //  m_fndbTmp
        //      A find buffer that will be used for all of the findfirst/next
        //      calls done on this object. In theory we could have a single
        //      one on the info class, but it's just not worth the possible
        //      worries.
        // -------------------------------------------------------------------
        TDirIter    m_diterValue;
        TFindBuf    m_fndbTmp;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngDirIterVal,TMEngClassVal)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngDirIterInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngDirIterInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngDirIterInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngDirIterInfo(const TMEngDirIterInfo&) = delete;

        ~TMEngDirIterInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngDirIterInfo& operator=(const TMEngDirIterInfo&) = delete;


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
        //  m_c4ErrXXX
        //      We have an error enum, so we store the ordinals so that we
        //      can throw macro level errors internally if needed.
        //
        //  m_pmeciErrors
        //      A pointer the error enum we create for our errors.
        //
        //  m_pathExpand
        //      A temp path that we use for massaging between the internal
        //      and external view of the paths.
        //
        //  m_pathOrgPath
        //      The original macro level path, so that we can use it in any
        //      error messages and such.
        // -------------------------------------------------------------------
        tCIDLib::TCard2     m_c2EnumId_Errors;
        tCIDLib::TCard2     m_c2MethId_DefCtor;
        tCIDLib::TCard2     m_c2MethId_FindFirst;
        tCIDLib::TCard2     m_c2MethId_FindNext;
        tCIDLib::TCard4     m_c4ErrFindFailed;
        TMEngEnumInfo*      m_pmeciErrors;
        TPathStr            m_pathExpand1;
        TPathStr            m_pathExpand2;
        TPathStr            m_pathOrgPath;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngDirIterInfo,TMEngClassInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMEngFileSysVal
// PREFIX: mecv
// ---------------------------------------------------------------------------
class TMEngFileSysVal : public TMEngClassVal
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngFileSysVal
        (
            const   TString&                strName
            , const tCIDLib::TCard2         c2Id
            , const tCIDMacroEng::EConstTypes  eConst
        );

        TMEngFileSysVal(const TMEngFileSysVal&) = delete;

        ~TMEngFileSysVal();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngFileSysVal& operator=(const TMEngFileSysVal&) = delete;


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



    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngFileSysVal,TMEngClassVal)
};


// ---------------------------------------------------------------------------
//  CLASS: TMEngFileSysInfo
// PREFIX: meci
// ---------------------------------------------------------------------------
class TMEngFileSysInfo : public TMEngClassInfo
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strPath();


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMEngFileSysInfo
        (
                    TCIDMacroEngine&        meOwner
        );

        TMEngFileSysInfo(const TMEngFileSysInfo&) = delete;

        ~TMEngFileSysInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMEngFileSysInfo& operator=(const TMEngFileSysInfo&) = delete;


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
        tCIDLib::TVoid ThrowPathNotFQ
        (
                    TCIDMacroEngine&        meOwner
            , const TString&                strThrowingClass
        );

        tCIDLib::TVoid ThrowOpenFailed
        (
                    TCIDMacroEngine&        meOwner
            , const TString&                strThrowingClass
            , const TString&                strPath
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
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const TString&                strThrowingClass
        );

        tCIDLib::TVoid ThrowAnErr
        (
                    TCIDMacroEngine&        meOwner
            , const tCIDLib::TCard4         c4ToThrow
            , const TString&                strMsg
            , const TString&                strThrowingClass
        );

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
        //  m_c4ErrXXX
        //      We have an error enum, so we store the ordinals so that we
        //      can throw macro level errors internally if needed.
        //
        //  m_pmeciErrors
        //      A pointer the error enum we create for our errors.
        //
        //  m_pmeciXXX
        //      We lookup some enums that we use in params, so that we can
        //      quickly get access to their values.
        //
        //  m_pathExpand1
        //  m_pathExpand2
        //      All the paths are macro language based paths, which have to
        //      be expanded to local file system paths internally. We keep
        //      these around for doing that.
        // -------------------------------------------------------------------
        tCIDLib::TCard2     m_c2EnumId_Errors;
        tCIDLib::TCard2     m_c2MethId_CopyFile;
        tCIDLib::TCard2     m_c2MethId_DefCtor;
        tCIDLib::TCard2     m_c2MethId_DelFile;
        tCIDLib::TCard2     m_c2MethId_DirExists;
        tCIDLib::TCard2     m_c2MethId_FileExists;
        tCIDLib::TCard2     m_c2MethId_FindDirs;
        tCIDLib::TCard2     m_c2MethId_FindFiles;
        tCIDLib::TCard2     m_c2MethId_HasWildCards;
        tCIDLib::TCard2     m_c2MethId_MakePath;
        tCIDLib::TCard2     m_c2MethId_MakeSubDir;
        tCIDLib::TCard2     m_c2TypeId_PathList;
        tCIDLib::TCard4     m_c4ErrCopyFailed;
        tCIDLib::TCard4     m_c4ErrCreateFailed;
        tCIDLib::TCard4     m_c4ErrDelFailed;
        tCIDLib::TCard4     m_c4ErrGenErr;
        tCIDLib::TCard4     m_c4ErrOpenFailed;
        tCIDLib::TCard4     m_c4ErrPathNotFound;
        tCIDLib::TCard4     m_c4ErrPathNotFQ;
        tCIDLib::TCard4     m_c4ErrSearchFailed;
        TMEngEnumInfo*      m_pmeciErrors;
        TPathStr            m_pathExpand1;
        TPathStr            m_pathExpand2;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngFileSysInfo,TMEngClassInfo)
};

#pragma CIDLIB_POPPACK


