//
// FILE NAME: CIDOrbWU_RemFileOpenBrowser.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/98/2004
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
//  This is the header for the CIDOrbWU_RemFldDlgBrowser.cpp file, which
//  implements a derivative of the TFileOpenBrowser abstract base class.
//  This class is provided by CIDWUtils and is used in conjunction with the
//  advanced file open dialog to allow that dialog to operate on any number
//  of target stores.
//
//  In this case, we use a remote browser object, provided by CIDOrbUC, to
//  allow browsing of a remote system via any application which implements
//  the server side of the remote browser interface.
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
//   CLASS: TRemFileOpenBrowser
//  PREFIX: fob
// ---------------------------------------------------------------------------
class CIDORBWUEXP TRemFileOpenBrowser : public TFileOpenBrowser
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRemFileOpenBrowser();

        TRemFileOpenBrowser
        (
            const   TString&                strBindingName
        );

        ~TRemFileOpenBrowser();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bBuildSelectedPath
        (
                    THListWnd&              wndList
            , const TString&                strBase
            ,       TString&                strToFill
        );

        tCIDLib::TBoolean bExtractPath
        (
            const   TString&                strSrc
            ,       TString&                strToFill
        );

        tCIDLib::TBoolean bFileExists
        (
            const   TString&                strToCheck
        );

        tCIDLib::TBoolean bHasWildCards
        (
            const   TString&                strToCheck
        );

        tCIDLib::TBoolean bIsFQDirectory
        (
            const   TString&                strToCheck
        );

        tCIDLib::TBoolean bIsSelectedADir
        (
                    THListWnd&              wndList
        );

        tCIDLib::TBoolean bListSelect
        (
                    TEntryField&            wndEF
            ,       THListWnd&              wndList
            ,       TString&                strDirToFill
        );

        tCIDLib::TBoolean bLoadFiles
        (
            const   TFrameWindow&           wndOwner
            , const TString&                strSrcPath
            , const TString&                strWildCard
            ,       TRefVector<TListWndItem>& colToFill
        );

        tCIDLib::TBoolean bRemovePathLevel
        (
                    TString&                strTarget
            , const TString&                strVolume
        );

        tCIDLib::TVoid BuildPath
        (
            const   TString&                strBase
            , const TString&                strName
            ,       TString&                strToFill
        );

        tCIDLib::TCard4 c4ReadFile
        (
            const   TString&                strToRead
            ,       THeapBuf&               mbufToFill
        );

        tCIDLib::TVoid ChangeVolume
        (
                    TComboBox&              wndVolumes
            ,       TString&                strLastCurDir
        );

        tCIDLib::TVoid DeleteSelected
        (
                    THListWnd&              wndList
        );

        tCIDLib::ERenameRes eRenameSelected
        (
                    THListWnd&              wndList
            , const TString&                strNewName
        );

        tCIDLib::TVoid Initialize
        (
                    TString&                strInitPath
            ,       TTypesList&             colFileTypes
            , const TString&                strTitle
        );

        tCIDLib::TVoid LoadVolumes
        (
            const   TString&                strPath
            ,       TComboBox&              wndToLoad
        );

        tCIDLib::TVoid MakeNewDir
        (
            const   TString&                strCurDir
            , const TString&                strSubDir
            ,       THListWnd&              wndFileList
        );

        tCIDLib::TVoid RemovePathPart
        (
                    TString&                strTarget
        );

        TString strSelected
        (
                    THListWnd&              wndList
        );

        TString strVolComboClass() const;

        tCIDLib::TVoid SelectVolume
        (
                    TComboBox&              wndVolumes
            , const TString&                strVolume
        );

        tCIDLib::TVoid SetCurDir
        (
                    TComboBox&              wndVolumes
            , const TString&                strToSet
        );

        tCIDLib::TVoid SplitPath
        (
            const   TString&                strToSplit
            ,       TString&                strPath
            ,       TString&                strNameExt
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetBinding
        (
            const   TString&                strBindingName
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TRemFileOpenBrowser(const TRemFileOpenBrowser&);
        tCIDLib::TVoid operator=(const TRemFileOpenBrowser&);



        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pathTmp
        //      Since this browser is for standard file system browsing,
        //      the TPathStr class provides most of our functionality, so we
        //      keep a temp one to use in calls, to avoid creating and
        //      destroying them constantly.
        //
        //  m_strBinding
        //      The name server binding name for the remote host's browser
        //      object. We need this in order to look up the object id and
        //      make a client proxy.
        // -------------------------------------------------------------------
        TPathStr    m_pathTmp;
        TString     m_strBinding;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TRemFileOpenBrowser,TFileOpenBrowser)
};


#pragma CIDLIB_POPPACK


