//
// FILE NAME: CIDKernel_FileSystem.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/19/1996
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
//  This is the header for the CIDKernel_FileSystem.Cpp file. This file
//  implements the TKrnlFileSys namespace, which provides access to file
//  system services, such as directory searching, file deleting, setting of
//  file attributes, etc... In other words the stuff that does not require
//  a handle to a particular file.
//
//  A number of small information classes are also implemented here. One is
//  the TKrnlDirSearch class, which is used to abstract the mechanisms needed
//  to maintain context across a directory search.
//
//  Another is the TKrnlVolumeInfo class, which is used to return information
//  about a single disk volume.
//
//  And we define a simple class used to return error information from the
//  bQueryVolumeList() method.
//
//  TKrnlDirChangeMon and TKrnlDirChangeInfo support monitoring content changes
//  in or in and under a directory. The caller must create a thread that constantly
//  is calling into the bReadChanges() method and immediately process the returned
//  changed info objects to queue them up in some processing queue of its own, then
//  call back in again. Otherwise, the system buffer used to hold these changes
//  could overflow.
//
//  The caller creates a Kernel linked list of change info objects. The read changes
//  method doesn't reset it each time, it just uses as many of them as it needs, to
//  avoid constant creation/destruction. It tells you each time how many of them are
//  valid, and will expand it out to hold however many events are ready when you call
//  it. You can of course monitor that list and account for exception scenarios by
//  pruning it back. It will be re-expanded if needed in subsequent calls.
//
// CAVEATS/GOTCHAS:
//
//  1)  bFindFirst() and bFindNext() return a boolean to indicate whether
//      a match was found. The only reason False is returned is if the file
//      is not found. Any other error is thrown as an exception.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//   CLASS: TKrnlVolFailureInfo
//  PREFIX: kvolfi
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlVolFailureInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlVolFailureInfo();

        TKrnlVolFailureInfo
        (
            const   tCIDLib::TCh* const     pszVolume
            , const tCIDLib::EVolHWTypes    eHWType
            , const TKrnlError&             kerrReason
        );

        TKrnlVolFailureInfo
        (
            const   TKrnlVolFailureInfo&    kvolfiToCopy
        );

        ~TKrnlVolFailureInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlVolFailureInfo& operator=
        (
            const   TKrnlVolFailureInfo&    kvolfiToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::EVolHWTypes eHWType() const;

        tCIDLib::TErrCode errcReason() const;

        const TKrnlError& kerrReason() const;

        const tCIDLib::TCh* pszVolume() const;

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCh* const     pszVolume
            , const tCIDLib::EVolHWTypes    eHWType
            , const TKrnlError&             kerrReason
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eHWType
        //      An enum that indicates roughly the type of hardware that the
        //      volume lives on.
        //
        //  m_kerrReason
        //      This is the kernel error object that was returned for the
        //      failed volume info query.
        //
        //  m_pkvolfiNext
        //      Next pointer in the linked list.
        //
        //  m_pszVolume
        //      The name of the volume that failed.
        // -------------------------------------------------------------------
        tCIDLib::EVolHWTypes    m_eHWType;
        TKrnlError              m_kerrReason;
        tCIDLib::TCh*           m_pszVolume;
};


// ---------------------------------------------------------------------------
//   CLASS: TKrnlVolumeInfo
//  PREFIX: kvoli
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlVolumeInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlVolumeInfo();

        TKrnlVolumeInfo
        (
            const   tCIDLib::TCard4         c4MaxPathCompLen
            , const tCIDLib::EVolumeFlags   eFlags
            , const tCIDLib::EVolHWTypes    eHWType
            , const tCIDLib::TCh* const     pszFileSysType
            , const tCIDLib::TCh* const     pszVolume
            , const tCIDLib::TCh* const     pszVolumeLabel
            , const tCIDLib::TCh* const     pszVolumeSerialNum
        );

        TKrnlVolumeInfo
        (
            const   TKrnlVolumeInfo&        kvoliToCopy
        );

        ~TKrnlVolumeInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlVolumeInfo& operator=
        (
            const   TKrnlVolumeInfo&        kvoliToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4MaxPathCompLen() const;

        tCIDLib::TVoid ClearUsedInfo();

        tCIDLib::EVolumeFlags eFlags() const;

        tCIDLib::EVolHWTypes eHWType() const;

        const tCIDLib::TCh* pszFileSysType() const;

        const tCIDLib::TCh* pszVolume() const;

        const tCIDLib::TCh* pszVolumeLabel() const;

        const tCIDLib::TCh* pszVolumeSerialNum() const;

        tCIDLib::TVoid Set
        (
            const   tCIDLib::TCard4         c4MaxPathCompLen
            , const tCIDLib::EVolumeFlags   eFlags
            , const tCIDLib::EVolHWTypes    eHWType
            , const tCIDLib::TCh* const     pszFileSysType
            , const tCIDLib::TCh* const     pszVolume
            , const tCIDLib::TCh* const     pszVolumeLabel
            , const tCIDLib::TCh* const     pszVolumeSerialNum
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4MaxPathCompLen
        //      The maximum length of a path component, not the whole path but
        //      each individual component.
        //
        //  m_eFlags
        //      A set of flags of volume attributes.
        //
        //  m_eHWType
        //      An enum that indicates roughly the type of hardware that the
        //      volume lives on.
        //
        //  m_pszFileSysType
        //      A string that describes the file system type.
        //
        //  m_pszVolume
        //      The actual volume name. This varies in form by system. On
        //      Win32 systems, its the root directory, e.g. "C:/".
        //
        //  m_pszVolumeLabel
        //  m_pszVolumeSerialNum
        //      Some systems maintain a volume label for human consumption
        //      and a serial number for machine consumption.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4MaxPathCompLen;
        tCIDLib::EVolumeFlags   m_eFlags;
        tCIDLib::EVolHWTypes    m_eHWType;
        tCIDLib::TCh*           m_pszFileSysType;
        tCIDLib::TCh*           m_pszVolume;
        tCIDLib::TCh*           m_pszVolumeLabel;
        tCIDLib::TCh*           m_pszVolumeSerialNum;
};


class KRNLEXPORT TKrnlVolLists
{
    public :
        TKrnlVolLists();
        ~TKrnlVolLists();


    private :

};


// ---------------------------------------------------------------------------
//   CLASS: TKrnlDirChangeInfo
//  PREFIX: kdchi
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlDirChangeInfo
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlDirChangeInfo();

        TKrnlDirChangeInfo(const TKrnlDirChangeInfo&) = delete;

        ~TKrnlDirChangeInfo() {}


        // -------------------------------------------------------------------
        //  Public operator
        // -------------------------------------------------------------------
        TKrnlDirChangeInfo& operator=
        (
            const   TKrnlDirChangeInfo&     kdchiSrc
        );


        // -------------------------------------------------------------------
        //  Public data members
        //
        //  m_eChanges
        //      The change being reported
        //
        //  m_kstrName
        //      The (directory relative) name of the thing being reported on
        //
        //  m_kstrNew
        //      Only used for a renamed change, and indicates the new name, while
        //      m_kstrName is the original name.
        // -------------------------------------------------------------------
        tCIDLib::EDirChanges    m_eChange;
        TKrnlString             m_kstrName;
        TKrnlString             m_kstrNew;
};



// ---------------------------------------------------------------------------
//   CLASS: TKrnlDirChangeMon
//  PREFIX: kdchm
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlDirChangeMon
{
    public :
        // -------------------------------------------------------------------
        //  Public types
        // -------------------------------------------------------------------
        using TChangeList = TKrnlLList<TKrnlDirChangeInfo>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlDirChangeMon();

        TKrnlDirChangeMon(const TKrnlDirChangeMon&) = delete;

        ~TKrnlDirChangeMon();


        // -------------------------------------------------------------------
        //  Public operator
        // -------------------------------------------------------------------
        TKrnlDirChangeMon& operator=(const TKrnlDirChangeMon&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCancel();

        tCIDLib::TBoolean bCleanup();

        tCIDLib::TBoolean bDoSubDirs() const
        {
            return m_bDoSubDirs;
        }

        tCIDLib::TBoolean bIsReady() const;

        tCIDLib::TBoolean bReadChanges
        (
                    TChangeList&            kllstToFill
            ,       tCIDLib::TCard4&        c4ValidCnt
        );

        tCIDLib::TBoolean bStartMonitor
        (
            const   tCIDLib::TCh* const     pszTargetDir
            , const tCIDLib::EDirChFilters  eFilters
            , const tCIDLib::TBoolean       bDoSubDirs
        );

        const TKrnlString& kstrTargetDir() const
        {
            return m_kstrTargetDir;
        }


    private :
        // -------------------------------------------------------------------
        //  Private forward ref
        // -------------------------------------------------------------------
        struct TDirChangeMonitorData;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bDoSubDirs
        //      The caller tells us if he wants to monitor subdirectories as well.
        //
        //  m_eFilters
        //      The types of changes of interest. These are set up to match the Windows
        //      flags.
        //
        //  m_kdchiOwedRename
        //      Renames are sent as two different events, but we report them as a
        //      single one. Since they may not happen in the same read, if we see
        //      the old name event, we store it here, and the next one we see (either
        //      in that read or the next) should be the new. If not, we drop this
        //      one. It's in use if the change isn't None.
        //
        //  m_pData
        //      An opaque structure for use by the per-platform implementation of
        //      this class.
        //
        //  m_kstrTargetDir
        //      If currently active, then this is the target directory we are monitoring.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bDoSubDirs;
        tCIDLib::EDirChFilters  m_eFilters;
        TKrnlDirChangeInfo      m_kdchiOwedRename;
        TDirChangeMonitorData*  m_pData;
        TKrnlString             m_kstrTargetDir;
};




// ---------------------------------------------------------------------------
//  NAMESPACE: TKrnlFileSys
// ---------------------------------------------------------------------------
namespace TKrnlFileSys
{
    // -----------------------------------------------------------------------
    //  The structure returned by directory search operations, volume info
    //  queries, and path parsing operations.
    // -----------------------------------------------------------------------
    struct TRawFileFind
    {
        tCIDLib::EFileInfoFlags eInfoFlags;
        tCIDLib::TEncodedTime   enctCreationTime;
        tCIDLib::TEncodedTime   enctLastAccessTime;
        tCIDLib::TEncodedTime   enctLastWriteTime;
        tCIDLib::TCard8         c8FileSize;
        tCIDLib::TCh            szName[kCIDLib::c4MaxPathLen+1];
    };


    // -----------------------------------------------------------------------
    //  Public functions
    // -----------------------------------------------------------------------
    KRNLEXPORT tCIDLib::TBoolean bCopy
    (
        const   tCIDLib::TCh* const     pszSourceFile
        , const tCIDLib::TCh* const     pszTargetFile
        , const tCIDLib::TBoolean       bFailIfExists = kCIDLib::False
    );

    KRNLEXPORT tCIDLib::TBoolean bCreateTmpFileName
    (
                tCIDLib::TCh* const     pszToFillIn
        , const tCIDLib::TCard4         c4BufChars
        , const tCIDLib::TCard4         c4UniqueNum = 0
    );

    KRNLEXPORT tCIDLib::TBoolean bCreateTmpFileNameIn
    (
        const   tCIDLib::TCh* const     pszTarDir
        ,       tCIDLib::TCh* const     pszToFillIn
        , const tCIDLib::TCard4         c4BufChars
        , const tCIDLib::TCard4         c4UniqueNum = 0
    );

    KRNLEXPORT tCIDLib::TBoolean bExists
    (
        const   tCIDLib::TCh* const     pszToFind
        ,       tCIDLib::TBoolean&      bResult
        ,       tCIDLib::TBoolean&      bIsUnique
        , const tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::All
    );

    KRNLEXPORT tCIDLib::TBoolean bExists
    (
        const   tCIDLib::TCh* const     pszToFind
        ,       tCIDLib::TBoolean&      bResult
        , const tCIDLib::EDirSearchFlags eFlags = tCIDLib::EDirSearchFlags::All
    );

    KRNLEXPORT tCIDLib::TBoolean bFindInPath
    (
        const   tCIDLib::TCh* const     pszPath
        , const tCIDLib::TCh* const     pszSpec
        ,       tCIDLib::TCh* const     pszMatch
        , const tCIDLib::TCard4         c4BufChars
    );

    KRNLEXPORT tCIDLib::TBoolean bFindInSysPath
    (
        const   tCIDLib::TCh* const     pszSpec
        ,       tCIDLib::TCh* const     pszMatch
        , const tCIDLib::TCard4         c4BufChars
    );

    KRNLEXPORT tCIDLib::TBoolean bHasWildCards
    (
        const   tCIDLib::TCh* const     pszToCheck
    );

    KRNLEXPORT tCIDLib::TBoolean bIsDirectory
    (
        const   tCIDLib::TCh* const     pszDirToCheck
    );

    KRNLEXPORT tCIDLib::TBoolean bIsNormalDir
    (
        const   tCIDLib::TCh* const     pszDirToCheck
    );

    KRNLEXPORT tCIDLib::TBoolean bIsRedirected
    (
        const   tCIDLib::EStdFiles      eStdFile
    );

    KRNLEXPORT tCIDLib::TBoolean bMakeDirectory
    (
        const   tCIDLib::TCh* const     pszToCreate
        , const tCIDLib::TBoolean       bErrIfExists = kCIDLib::True
        , const tCIDLib::TBoolean       bAllAccess = kCIDLib::False
    );

    KRNLEXPORT tCIDLib::TBoolean bMakePath
    (
        const   tCIDLib::TCh* const     pszToCreate
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryCurrentDir
    (
                tCIDLib::TCh* const     pszToFillIn
        , const tCIDLib::TCard4         c4BufChars
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryFullPath
    (
        const   tCIDLib::TCh* const     pszPartialPath
        ,       tCIDLib::TCh* const     pszToFillIn
        , const tCIDLib::TCard4         c4BufChars
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryMountInfo
    (
        const   tCIDLib::TCh* const     pszVolume
        ,       tCIDLib::TCh* const     pszHostToFill
        , const tCIDLib::TCard4         c4MaxHostChars
        ,       tCIDLib::TCh* const     pszMountInfo
        , const tCIDLib::TCard4         c4MaxInfoChars
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryTmpPath
    (
                tCIDLib::TCh* const     pszToFillIn
        , const tCIDLib::TCard4         c4BufChars
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryVolHWType
    (
        const   tCIDLib::TCh* const     pszVolPath
        ,       tCIDLib::EVolHWTypes&   eType
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryVolumeInfo
    (
        const   tCIDLib::TCh* const     pszSrcPath
        ,       tCIDLib::TCard8&        c8VolTotalBytes
        ,       tCIDLib::TCard8&        c8VolFreeBytes
        ,       tCIDLib::TCard8&        c8UserAvailBytes
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryVolumeInfo
    (
        const   tCIDLib::TCh* const     pszSrcPath
        ,       TKrnlVolumeInfo&        kvoliToFill
    );

    KRNLEXPORT tCIDLib::TBoolean bQueryVolumeList
    (
                TKrnlLList<TKrnlVolumeInfo>& kllistGood
        ,       tCIDLib::TCard4&        c4VolCount
        ,       TKrnlLList<TKrnlVolFailureInfo>& kllistFail
        ,       tCIDLib::TCard4&        c4ErrCount
        , const tCIDLib::TBoolean       bIncludeShares
        , const tCIDLib::TBoolean       bForceUpdate
    );

    KRNLEXPORT tCIDLib::TBoolean bNormalizePath
    (
        const  tCIDLib::TCh* const      pszToNormalize
        ,       tCIDLib::TCh* const     pszResult
        , const tCIDLib::TCard4         c4MaxChars
    );

    KRNLEXPORT tCIDLib::TBoolean bRemoveDir
    (
        const   tCIDLib::TCh* const     pszToRemove
    );

    KRNLEXPORT tCIDLib::TBoolean bRemoveFile
    (
        const   tCIDLib::TCh* const     pszToRemove
    );

    KRNLEXPORT tCIDLib::TBoolean bRemovePath
    (
        const   tCIDLib::TCh* const     pszStartDir
        , const tCIDLib::ETreeDelModes  eMode
        , const tCIDLib::TBoolean       bRemoveStartDir
    );

    KRNLEXPORT tCIDLib::TBoolean bRename
    (
        const   tCIDLib::TCh* const     pszOldName
        , const tCIDLib::TCh* const     pszNewName
    );

    KRNLEXPORT tCIDLib::TBoolean bReplaceFile
    (
        const   tCIDLib::TCh* const     pszToReplace
        , const tCIDLib::TCh* const     pszNewFile
        , const tCIDLib::TCh* const     pszBackupName
    );

    KRNLEXPORT tCIDLib::TBoolean bSetCurrentDir
    (
        const   tCIDLib::TCh* const     pszNewDir
    );

    KRNLEXPORT tCIDLib::TBoolean bSetPerms
    (
        const   tCIDLib::TCh* const     pszTarget
        , const tCIDLib::EFilePerms     ePerms
    );

    KRNLEXPORT tCIDLib::TCard4 c4FSRetryCount();

    KRNLEXPORT tCIDLib::TCard4 c4VolListSerialNum();

    KRNLEXPORT tCIDLib::EConnRes eAccessRemoteShare
    (
        const   tCIDLib::TCh* const     pszTarShare
        , const tCIDLib::TCh* const     pszUserName
        , const tCIDLib::TCh* const     pszPassword
        , const tCIDLib::TBoolean       bTemporary
    );
}




// ---------------------------------------------------------------------------
//   CLASS: TKrnlDirSearch
//  PREFIX: kds
// ---------------------------------------------------------------------------
class KRNLEXPORT TKrnlDirSearch
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TKrnlDirSearch();

        TKrnlDirSearch(const TKrnlDirSearch&) = delete;

        ~TKrnlDirSearch();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TKrnlDirSearch& operator=(const TKrnlDirSearch&) = delete;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bClose();

        tCIDLib::TBoolean bFindFirst
        (
            const   tCIDLib::TCh* const         pszToSearch
            ,       TKrnlFileSys::TRawFileFind& fndbToFill
            ,       tCIDLib::TBoolean&          bFound
            , const tCIDLib::EDirSearchFlags    eFlags = tCIDLib::EDirSearchFlags::All
        );

        tCIDLib::TBoolean bFindNext
        (
                    TKrnlFileSys::TRawFileFind& fndbToFill
            ,       tCIDLib::TBoolean&          bFound
        );

        tCIDLib::TBoolean bSearchIsOpen() const;

        tCIDLib::EDirSearchFlags eSearchFlags() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        TDirSearchHandle            m_hdirSearch;
        tCIDLib::EDirSearchFlags    m_eSearchFlags;
};

#pragma CIDLIB_POPPACK


