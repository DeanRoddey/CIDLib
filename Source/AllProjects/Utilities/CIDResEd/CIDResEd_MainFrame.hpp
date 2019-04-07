//
// FILE NAME: CIDResEd_MainFrame.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/17/2000
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
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TResEdMainFrame
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class TResEdMainFrame : public TFrameWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TResEdMainFrame();

        TResEdMainFrame(const TResEdMainFrame&) = delete;

        ~TResEdMainFrame();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TResEdMainFrame& operator=(const TResEdMainFrame&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bProcessAccel
        (
            const   tCIDLib::TVoid* const   pMsgData
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bChanges() const;

        tCIDLib::TBoolean bFindTextSym
        (
            const   TString&                strToFind
            ,       TTextSym&               tsymToLoad
        )   const;

        tCIDLib::TBoolean bTextSymExists
        (
            const   TString&                strToFind
        )   const;

        tCIDLib::TBoolean bSessionOpen() const;

        tCIDLib::TCard4 c4DialogCount() const;

        tCIDLib::TVoid ClearDlgInfo();

        tCIDLib::TVoid ClearItemInfo();

        tCIDLib::TVoid CloseSession();

        tCIDLib::TVoid CreateMain();

        tCIDCtrls::EEvResponses eListHandler
        (
                    TListChangeInfo&        wnotEvent
        );

        tCIDLib::TVoid OpenSession();

        tCIDLib::TVoid QueryTextSym
        (
            const   tCIDResEd::EMsgTypes    eType
            , const tCIDLib::TCard4         c4Index
            ,       TString&                strSym
            ,       tCIDLib::TCard4&        c4TextId
            ,       TString&                strMsgText
        );

        const TResEdDlgDesc* preddByEditId
        (
            const   tCIDLib::TCard4         c4ToFind
        )   const;

        const TResEdDlgDesc* preddByResId
        (
            const   tCIDLib::TResId         ridToFind
        )   const;

        const TResEdDlgDesc* preddByName
        (
            const   TString&                strName
        )   const;

        const TResEdMsgItem* premitFind
        (
            const   tCIDResEd::EMsgTypes    eType
            , const TString&                strName
        )   const;

        const TResEdMsgList& remilByType
        (
            const   tCIDResEd::EMsgTypes    eType
        )   const;

        const TString& strCurFile() const;

        const TString& strFacBaseName() const;

        const TString& strFacPath() const;

        const TString& strLoadResText
        (
            const   TTextSym&               tsymToLoad
        );

        tCIDLib::TVoid SaveSession();

        tCIDLib::TVoid SetSessionState
        (
            const   tCIDLib::TBoolean       bOpen
            , const TString&                strTitle
        );

        tCIDLib::TVoid StoreMsgText
        (
            const   TResEdMsgList&          remilErrs
            , const TResEdMsgList&          remilMsgs
        );

        tCIDLib::TVoid UpdateDlgInfo
        (
            const   TString&                strSymbol
            , const tCIDCtrls::TWndId       widSymbol
        );

        tCIDLib::TVoid UpdateDlg
        (
            const   TResEdDlgDesc&          reddNew
        );

        tCIDLib::TVoid UpdateItemInfo
        (
            const   TString&                strSymbol
            , const tCIDCtrls::TWndId       widSymbol
            , const TString&                strType
            , const TArea&                  areaItem
        );

        tCIDLib::TVoid UpdateMousePos
        (
            const   TString&                strPos
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AreaChanged
        (
            const   TArea&                  areaPrev
            , const TArea&                  areaNew
            , const tCIDCtrls::EPosStates   ePosState
            , const tCIDLib::TBoolean       bOrgChanged
            , const tCIDLib::TBoolean       bSizeChanged
            , const tCIDLib::TBoolean       bStateChanged
        )   override;

        tCIDLib::TBoolean bAllowShutdown() override;

        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TVoid CodeReceived
        (
            const   tCIDLib::TInt4          i4Code
            , const tCIDLib::TCard4         c4Data
        )   override;

        tCIDLib::TVoid Destroyed() override;

        tCIDLib::TVoid DragEnd() override;

        tCIDLib::TVoid MenuCommand
        (
            const   tCIDLib::TResId         ridItem
            , const tCIDLib::TBoolean       bChecked
            , const tCIDLib::TBoolean       bEnabled
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bOpenSession
        (
            const   TString&                strToOpen
            , const tCIDLib::TBoolean       bNew
        );

        tCIDLib::TBoolean ValidateLoadRes();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pwndClient
        //      A pane container window that we create as a client of this main
        //      frame window. We keep it sized to fit the available client area.
        //      To it we add the three pane windows that make up the main part of
        //      our interface.
        //
        //  m_pwndXXX
        //      Pointers to the three main pane windows that we create to fill our
        //      client area.
        //
        //  m_redatCur
        //      The resource data that we parsed from the file we are editing.
        //      If the current file member is empty, then this will have no
        //      meaningful contents.
        //
        //  m_redatOrg
        //      We keep an original copy of the data so we can know if there
        //      are changes. Any time the user saves, we copy the current
        //      contents to here as a new baseline.
        //
        //  m_strCurFile
        //      If we are currently editing a file, this is where we loaded it
        //      from. Else its empty.
        //
        //  m_strFacBaseName
        //      The base name, i.e. the facility name, which is the base part
        //      of the file name of the .CIDRC file we opened. This will be
        //      used as the base name of any other files we write out.
        //
        //  m_strFacPath
        //      The path of the facility we are editing. We get it from the
        //      path of the .CIDRC file that is opened.
        //
        //  m_strState_xxx
        //      We pre-set up names for the window states we support.
        //
        //  m_strSBTmpFmt
        //      A temp string for use by the UpdateXXX() methods that are called
        //      to update the status bar.
        // -------------------------------------------------------------------
        TResEdData          m_redatCur;
        TResEdData          m_redatOrg;
        TString             m_strCurFile;
        TString             m_strFacBaseName;
        TString             m_strFacPath;
        TResAttrEdPane*     m_pwndAttrsPane;
        TPaneWndCont*       m_pwndClient;
        TResEdCont*         m_pwndEditCont;
        TScrollArea*        m_pwndEditPane;
        TTabbedWnd*         m_pwndTabsPane;
        TDialogsTab*        m_pwndTab_Dialogs;
        const TString       m_strState_NoSession;
        const TString       m_strState_Session;
        TString             m_strSBTmpFmt;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TResEdMainFrame,TFrameWnd)
};


