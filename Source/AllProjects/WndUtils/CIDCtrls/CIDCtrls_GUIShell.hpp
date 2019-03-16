//
// FILE NAME: CIDCtrls_GUIShell.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/27/2001
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
//  This is the header for the CIDWnd_Shell.cpp file. This file implements
//  a namespace that provides some functionality for manipulating the Windows
//  shell, to create new items and such.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//   CLASS: TGUIShellShortCut
//  PREFIX: gssc
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TGUIShellShortCut : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TGUIShellShortCut();

        TGUIShellShortCut
        (
            const   TGUIShellShortCut&  gsscToCopy
        );

        TGUIShellShortCut
        (
            const   TString&            strPathToSet
            , const TString&            strDescrToSet
            , const TString&            strArgsToSet
            , const TString&            strIconPath
            , const TString&            strLinkName
            , const TString&            strWorkingPath
            , const tCIDCtrls::EPosStates eInitState = tCIDCtrls::EPosStates::Restored
        );

        ~TGUIShellShortCut();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGUIShellShortCut& operator=
        (
            const   TGUIShellShortCut&  gsscToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsInitialized() const;

        tCIDCtrls::EPosStates eInitState() const;

        const TString& strArgs() const;

        const TString& strDescr() const;

        const TString& strIconPath() const;

        const TString& strLinkName() const;

        const TString& strPath() const;

        const TString& strWorkingPath() const;

        tCIDLib::TVoid Set
        (
            const   TString&            strPathToSet
            , const TString&            strDescrToSet
            , const TString&            strArgsToSet
            , const TString&            strIconPath
            , const TString&            strLinkName
            , const TString&            strWorkingPath
            , const tCIDCtrls::EPosStates eInitState = tCIDCtrls::EPosStates::Restored
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eInitState
        //      The initial display state of the new short cut. Defaults to
        //      restored.
        //
        //  m_strArgs
        //      The arguments to pass to the short cutted program, if its a
        //      program.
        //
        //  m_strDescr
        //      The discriptive text for the short cut, which will show up
        //      the properties.
        //
        //  m_strIconPath
        //      The path to the icon that this short cut should display.
        //
        //  m_strLinkName
        //      The name to give to the link. This will show up under the
        //      icon. It should be a valid file system root file name, i.e.
        //      no path and no extension, but otherwise using only valid
        //      file system characters.
        //
        //  m_strPath
        //      The path to the thing that the short cut represents.
        //
        //  m_strWorkingPath
        //      The path that should become the current directory when the
        //      think linked to is run.
        // -------------------------------------------------------------------
        tCIDCtrls::EPosStates   m_eInitState;
        TString                 m_strArgs;
        TString                 m_strDescr;
        TString                 m_strIconPath;
        TString                 m_strLinkName;
        TString                 m_strPath;
        TString                 m_strWorkingPath;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGUIShellShortCut,TObject)
};


namespace TGUIShell
{
    // -----------------------------------------------------------------------
    //  To save a lot of typing and verbiage, define a typedef for a collection
    //  of short cuts.
    // -----------------------------------------------------------------------
    typedef TCollection<TGUIShellShortCut>  TShortCutList;


    CIDCTRLSEXP tCIDLib::TBoolean bQueryShortCut
    (
        const   TString&                strLinkDir
        ,       TGUIShellShortCut&      gsscToFill
        ,       tCIDLib::TCard4&        c4Error
    );

    CIDCTRLSEXP tCIDLib::TVoid CreateShortCut
    (
        const   TString&                strLinkDir
        , const TGUIShellShortCut&      gsscToCreate
    );

    CIDCTRLSEXP tCIDLib::TVoid CreateShortCut
    (
        const   TGUIShellShortCut&      gsscToCreate
    );

    CIDCTRLSEXP tCIDLib::TVoid CreateShortCutGroup
    (
        const   TString&                strLinkDir
        , const TShortCutList&          colShortCuts
    );

    CIDCTRLSEXP tCIDLib::TVoid CreateShortCutGroup
    (
        const   tCIDLib::ESpecialPaths  eFolder
        , const TShortCutList&          colShortCuts
    );

    CIDCTRLSEXP tCIDLib::TVoid InvokeDefBrowser
    (
                TWindow&                wndOwner
        , const TString&                strURL
    );

    CIDCTRLSEXP tCIDLib::TVoid InvokeDefFileAction
    (
                TWindow&                wndOwner
        , const TString&                strFilePath
        , const tCIDCtrls::EPosStates   ePosState
    );
}

