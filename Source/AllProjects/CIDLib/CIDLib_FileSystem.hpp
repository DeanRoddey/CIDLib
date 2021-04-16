//
// FILE NAME: CIDLib_FileSystem.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/12/1993
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
//  This file implements the TFileSys namespace, which provides directory
//  searching, volume management, and those file operations that are
//  performed on files when they are closed such as rename, copy, delete,
//  attribute management, etc...
//
//  We also define a simple class here that is used to return some information
//  from the c4QueryAvailableVolumes() method. It queries a list of all the
//  available volumes. However, it cannot get info on volumes that are on
//  hardware that is not available, such as an empty floppy drive or an
//  unsupported file system on multi-boot systems. This class defines the
//  info returned for a failure.
//
//  And we provide a simple janitor class that allows you to safely change
//  the current directory on a scoped basis and be sure that it will be
//  restored even if an exception is thrown.
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
//   CLASS: TVolFailureInfo
//  PREFIX: volfi
// ---------------------------------------------------------------------------
class CIDLIBEXP TVolFailureInfo : public TObject, public MStreamable
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static const TString& strVolKey
        (
            const   TVolFailureInfo&        volfiSrc
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TVolFailureInfo();

        TVolFailureInfo
        (
            const   TKrnlVolFailureInfo&    kvolfiSrc
        );

        TVolFailureInfo
        (
            const   TVolFailureInfo&        volfiToCopy
        );

        ~TVolFailureInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TVolFailureInfo& operator=
        (
            const   TVolFailureInfo&        volfiToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EVolHWTypes eHWType() const;

        tCIDLib::TErrCode errcReason() const;

        const TString& strVolume() const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eHWType
        //      The hardware type of the failed volume, if it could be
        //      determined.
        //
        //  m_kerrReason
        //      This is the kernel error object that was returned for the
        //      failed volume info query.
        //
        //  m_strVolume
        //      The name of the volume that failed.
        // -------------------------------------------------------------------
        tCIDLib::EVolHWTypes    m_eHWType;
        TKrnlError              m_kerrReason;
        TString                 m_strVolume;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TVolFailureInfo, TObject)
};



// ---------------------------------------------------------------------------
//   CLASS: TCurDirJanitor
//  PREFIX: jan
// ---------------------------------------------------------------------------
class CIDLIBEXP TCurDirJanitor
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCurDirJanitor() = delete;

        TCurDirJanitor
        (
            const   TString&                strNewDir
        );

        TCurDirJanitor(const TCurDirJanitor&) = delete;

        ~TCurDirJanitor();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCurDirJanitor& operator=(const TCurDirJanitor&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_strOldDir
        //      The old directory that was initial set, and which we will put
        //      back.
        // -------------------------------------------------------------------
        TString     m_strOldDir;
};


// ---------------------------------------------------------------------------
//  NAMESPACE: TFileSys
// ---------------------------------------------------------------------------
namespace TFileSys
{
    CIDLIBEXP tCIDLib::TBoolean bExists
    (
        const   TString&                strSpec
        , const tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::All
    );

    CIDLIBEXP tCIDLib::TBoolean bExists
    (
        const   TString&                strDirectory
        , const TString&                strName
        , const tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::All
    );

    CIDLIBEXP tCIDLib::TBoolean bExists
    (
        const   TString&                strSpec
        ,       TFindBuf&               fndbToFill
        , const tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::All
    );

    CIDLIBEXP tCIDLib::TBoolean bExists
    (
        const   TString&                strDirectory
        , const TString&                strName
        ,       TFindBuf&               fndbToFill
        , const tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::All
    );

    CIDLIBEXP tCIDLib::TBoolean bExistsInPath
    (
        const   TString&                strSpec
        , const TString&                strSearchPath
        , const tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::All
    );

    CIDLIBEXP tCIDLib::TBoolean bExistsInPath
    (
        const   TString&                strSpec
        , const TString&                strSearchPath
        ,       TFindBuf&               fndbTarget
        , const tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::All
    );

    CIDLIBEXP tCIDLib::TBoolean bExistsInSysPath
    (
        const   TString&                strSpec
        ,       TFindBuf&               fndbTarget
        , const tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::All
    );

    CIDLIBEXP tCIDLib::TBoolean bHasWildCards
    (
        const   TString&                strToSearch
    );

    CIDLIBEXP tCIDLib::TBoolean bIsDirectory
    (
        const   TString&                strPath
    );

    CIDLIBEXP tCIDLib::TBoolean bIsFQPath
    (
        const   TString&                strPath
    );

    CIDLIBEXP tCIDLib::TBoolean bIsNormalDir
    (
        const   TString&                strPath
    );

    CIDLIBEXP tCIDLib::TBoolean bIsRedirected
    (
        const   tCIDLib::EStdFiles      eStdFile
    );

    tCIDLib::TBoolean bRemoveTrailingSeparator
    (
                TString&                strSrc
    );

    CIDLIBEXP tCIDLib::TCard4 c4BuildDirTree
    (
        const   TString&                strWildCard
        ,       TFindBuf&               fndbMatches
    );

    CIDLIBEXP tCIDLib::TCard4 c4BuildFileTree
    (
        const   TString&                strWildCard
        ,       TFindBuf&               fndbMatches
        , const tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::All
    );

    CIDLIBEXP tCIDLib::TCard4 c4QueryAvailableVolumes
    (
                TCollection<TVolumeInfo>& colVolsToFill
        ,       TCollection<TVolFailureInfo>& colErrsToFill
        , const tCIDLib::TBoolean       bIncludeShares
        , const tCIDLib::TBoolean       bForceUpdate
    );

    CIDLIBEXP tCIDLib::TCard4 c4SearchDir
    (
        const   TString&                strSpec
        ,       TCollection<TFindBuf>&  colTarget
        , const tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::All
    );

    CIDLIBEXP tCIDLib::TCard4 c4SearchDir
    (
        const   TString&                strPath
        , const TString&                strWildCard
        ,       TCollection<TFindBuf>&  colTarget
        , const tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::All
    );

    CIDLIBEXP tCIDLib::TVoid CleanPath
    (
        const   TString&                strSpec
        , const tCIDLib::ETreeDelModes  eMode = tCIDLib::ETreeDelModes::Remove
    );

    CIDLIBEXP tCIDLib::TVoid CopyFile
    (
        const   TString&                strSourcePath
        , const TString&                strTargetPath
        , const tCIDLib::TBoolean       bFailIfExists = kCIDLib::False
    );

    CIDLIBEXP tCIDLib::TVoid CreateTmpFileName
    (
                TString&                strToFill
        , const tCIDLib::TCard4         c4UniqueId = 0
    );

    CIDLIBEXP tCIDLib::TVoid CreateTmpFileNameIn
    (
        const   TString&                strTarDir
        ,       TString&                strToFill
        , const tCIDLib::TCard4         c4UniqueId = 0
    );

    CIDLIBEXP tCIDLib::TVoid CreateTmpSubDir
    (
        const   TString&                strName
        ,       TString&                strToFill
        , const tCIDLib::TBoolean       bCleanContents
    );

    CIDLIBEXP tCIDLib::TVoid DeleteFile
    (
        const   TString&                strSpec
    );

    CIDLIBEXP tCIDLib::TVoid DeleteFile
    (
        const   TString&                strDirectory
        , const TString&                strName
    );

    CIDLIBEXP tCIDLib::EConnRes eAccessRemoteShare
    (
        const   TString&                strRemotePath
        , const TString&                strUserName
        , const TString&                strPassword
        , const tCIDLib::TBoolean       bTemporary
        , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
    );

    CIDLIBEXP tCIDLib::TVoid MakeDirectory
    (
        const   TString&                strToCreate
        , const tCIDLib::TBoolean       bErrIfExists = kCIDLib::True
        , const tCIDLib::TBoolean       bAllAccess = kCIDLib::False
    );

    CIDLIBEXP tCIDLib::TVoid MakePath
    (
        const   TString&                strToCreate
    );

    CIDLIBEXP tCIDLib::TVoid MakeSubDirectory
    (
        const   TString&                strToCreate
        , const TString&                strParent
    );

    CIDLIBEXP tCIDLib::TVoid NormalizePath
    (
        const   TString&                strToNormalize
        ,       TString&                strNormalized
    );

    CIDLIBEXP tCIDLib::TVoid QueryCurrentDir
    (
                TString&                strToFill
    );

    CIDLIBEXP tCIDLib::TVoid QueryFullPath
    (
        const   TString&                strPartialPath
        ,       TString&                strFullReturnedPath
    );

    CIDLIBEXP tCIDLib::TVoid QueryMountInfo
    (
        const   TString&                strVolume
        ,       TString&                strHost
        ,       TString&                strMountInfo
    );

    CIDLIBEXP tCIDLib::TVoid QueryVolumeInfo
    (
        const   TString&                strVolume
        ,       tCIDLib::TCard8&        c8TotalBytes
        ,       tCIDLib::TCard8&        c8VolFreeBytes
        ,       tCIDLib::TCard8&        c8UserAvailBytes
    );

    CIDLIBEXP tCIDLib::TVoid RemoveDirectory
    (
        const   TString&                strSpec
    );

    CIDLIBEXP tCIDLib::TVoid RemovePath
    (
        const   TString&                strStartDir
        , const tCIDLib::ETreeDelModes  eMode = tCIDLib::ETreeDelModes::Remove
        , const tCIDLib::TBoolean       bRemoveStart = kCIDLib::True
    );

    CIDLIBEXP tCIDLib::TVoid Rename
    (
        const   TString&                strSource
        , const TString&                strDest
    );

    CIDLIBEXP tCIDLib::TVoid ReplaceFile
    (
        const   TString&                strToReplace
        , const TString&                strNewFile
        , const TString&                strBackupName
    );

    CIDLIBEXP tCIDLib::TVoid ReplaceFile
    (
        const   TString&                strToReplace
        , const TString&                strNewFile
    );

    CIDLIBEXP tCIDLib::TVoid SetCurrentDir
    (
        const   TString&                strToSet
    );

    CIDLIBEXP tCIDLib::TVoid SetPerms
    (
        const   TString&                strTarget
        , const tCIDLib::EFilePerms     eToSet
    );
}

#pragma CIDLIB_POPPACK

