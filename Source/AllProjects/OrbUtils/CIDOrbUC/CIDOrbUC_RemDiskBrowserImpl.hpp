//
// FILE NAME: CIDOrbUC_RemDiskBrowserImpl.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/29/2004
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
//  The header for the CIDOrbUC_RemDiskBrowserImpl.cpp file, which implements
//  a derivative of the remote disk browser interface for general purpose,
//  open ended disk browsing, i.e. full access to the remote system's
//  volumes.
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
//   CLASS: TRemDiskBrowserImpl
//  PREFIX: orbs
// ---------------------------------------------------------------------------
class CIDORBUCEXP TRemDiskBrowserImpl : public TRemBrowserServerBase
{
    public :
        // -------------------------------------------------------------------
        //  Class types
        // -------------------------------------------------------------------
        using TVolList = TKeyedHashSet<TVolumeInfo, TString, TStringKeyOps>;
        using TFailList = TKeyedHashSet<TVolFailureInfo, TString, TStringKeyOps>;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRemDiskBrowserImpl();

        TRemDiskBrowserImpl(const TRemDiskBrowserImpl&) = delete;

        ~TRemDiskBrowserImpl();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TRemDiskBrowserImpl& operator=(const TRemDiskBrowserImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFileExists
        (
            const   TString&                strToCheck
        )   override;

        tCIDLib::TBoolean bFind
        (
            const   TString&                strPath
            , const TString&                strToFind
            ,       TFindBuf&               fndbToFill
        )   override;

        tCIDLib::TBoolean bFind2
        (
            const   TString&                strPath
            ,       TFindBuf&               fndbToFill
        )   override;

        tCIDLib::TBoolean bIsDirectory
        (
            const   TString&                strToCheck
        )   override;

        tCIDLib::TCard4 c4QueryVolumes
        (
                    TVolList&               colVols
            ,       TFailList&              colFails
        )   override;

        tCIDLib::TVoid LoadFiles
        (
            const   TString&                strPath
            , const TString&                strWildcard
            ,       TVector<TFindBuf>&      colSubDirs
            ,       TVector<TFindBuf>&      colFiles
        )   override;

        tCIDLib::TVoid MakeDirectory
        (
            const   TString&                strBasePath
            , const TString&                strNewDir
        )   override;

        tCIDLib::TVoid QueryHostName
        (
                    TString&                strToFill
        )   override;

        tCIDLib::TVoid ReadFile
        (
            const   TString&                strToRead
            ,       tCIDLib::TCard4&        c4Bufsz
            ,       THeapBuf&               mbufToFill
        )   override;

        tCIDLib::TVoid RemoveSelected
        (
            const   TString&                strToRemove
        )   override;

        tCIDLib::TVoid Rename
        (
            const   TString&                strOrgName
            , const TString&                strNewName
            , const tCIDLib::TBoolean       bIsScope
        )   override;


        // --------------------------------------------------------------------
        //  Protected, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::TVoid Initialize() override;

        tCIDLib::TVoid Terminate() override;


    private :
        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TRemDiskBrowserImpl,TRemBrowserServerBase)
};

#pragma CIDLIB_POPPACK


