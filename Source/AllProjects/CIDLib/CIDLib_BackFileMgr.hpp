//
// FILE NAME: CIDLib_BackFileMgr.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/17/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDLib_BackFileMgr.cpp file, which implements
//  the TBackFileMgr class. Since a lot of our stuff maintains backups of
//  their files, and needs to do common stuff like rotate all the files back
//  to store a new version (demote), or to bring forward old ones if the
//  current active file can't be opened for some reason (promote.)
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
//   CLASS: TBackFileMgr
//  PREFIX: bfm
// ---------------------------------------------------------------------------
class CIDLIBEXP TBackFileMgr : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBackFileMgr();

        TBackFileMgr(const TBackFileMgr&) = delete;

        TBackFileMgr
        (
            const   TString&                strFileName
            , const tCIDLib::TCard4         c4Backups
            , const TString&                strTargetPath
        );

        ~TBackFileMgr();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBackFileMgr& operator=(const TBackFileMgr&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bPromote
        (
            const   TString&                strSaveCurAs
        );

        tCIDLib::TBoolean bSaveCur
        (
            const   TString&                strSaveAs
        );

        tCIDLib::TCard4 c4QueryNextFile
        (
                    TString&                strToFill
            , const tCIDLib::TCard4         c4StartAt = 0
        );

        tCIDLib::TVoid Demote();

        tCIDLib::TVoid PromoteFrom
        (
            const   tCIDLib::TCard4         c4TargetInd
            , const TString&                strSaveCurAs
        );

        tCIDLib::TVoid Setup
        (
            const   TString&                strFileName
            , const tCIDLib::TCard4         c4Backups
            , const TString&                strTargetPath
        );

        const TString& strFileName() const;

        const TString& strTargetPath() const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckInitData();

        tCIDLib::TVoid CheckReady();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Backups
        //      The number of backups we do. Limited to from 1 to 8.
        //
        //  m_strBakName
        //      We have to do a lot of operations where we put a new number
        //      onto the end of the backup file name. So we pre-build up the
        //      path plus the base name plus the bak extensions, but without
        //      the number. This gives us a quick staring point.
        //
        //  m_strFileName
        //      The target file name that we are provding backup for. It will
        //      be the fully qualified path.
        //
        //  m_strTargetPath
        //      The path where we manage the files.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4Backups;
        TString         m_strBakName;
        TString         m_strFileName;
        TString         m_strTargetPath;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TBackFileMgr,TObject)
};

#pragma CIDLIB_POPPACK


