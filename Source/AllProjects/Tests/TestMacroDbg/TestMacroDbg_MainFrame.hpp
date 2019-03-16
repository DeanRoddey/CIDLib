//
// FILE NAME: TestMacroDbg_DbgFrame.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2003
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
//  This is the header for the file file TestMacroDlg_DbgFrame.cpp, which
//  implements the main frame of the debugger. Note that the debugger is
//  always nested within some other application, which is why it is implemented
//  as a library. So this frame will run as an owned window of some other
//  application window.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  CLASS: TMEngDbgFrameWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class TMEngDbgFrameWnd : public TFrameWnd
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and destructor
        // -------------------------------------------------------------------
        TMEngDbgFrameWnd();

        ~TMEngDbgFrameWnd();


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
        tCIDLib::TVoid CreateDbgFrame();

        TMacroDbgMainWnd& wndDbgClient();


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

        tCIDLib::TVoid Destroyed() override;

        tCIDLib::TVoid MenuCommand
        (
            const   tCIDLib::TResId         ridItem
            , const tCIDLib::TBoolean       bChecked
            , const tCIDLib::TBoolean       bEnabled
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCloseCurrent();

        tCIDLib::TVoid CheckShutdown();

        tCIDCtrls::EEvResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );

        tCIDLib::TVoid NewSession();

        tCIDLib::TVoid SaveState();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_mecmToUse
        //  m_mefrToUse
        //      The class manager and file resolver htat we want the debugger
        //      to use. It just references objects that we own.
        //
        //  m_pathCurClass
        //      The name of the class that we are currently parsing. If none,
        //      it will be empty. We use a path string because we build this
        //      up from a file name, and need to add and remove parts.
        //
        //  m_pwndDbgClient
        //      A pointer to the client window. Our parent frame class owns
        //      it, but we want a typed pointer to avoid lots of downcasting.
        //
        //  m_strState_xxx
        //      Names for the window states we use.
        //
        //  m_strTitleText
        //      To avoid reloading the text every time we open a new macro
        //      for debugging, we preload it.
        // -------------------------------------------------------------------
        TMacroDbgClassMgr           m_mecmToUse;
        TMEngFixedBaseFileResolver  m_mefrToUse;
        TPathStr                    m_pathCurClass;
        TMacroDbgMainWnd*           m_pwndDbgClient;
        const TString               m_strState_NoSession;
        const TString               m_strState_Session;
        TString                     m_strTitleText;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMEngDbgFrameWnd,TFrameWnd)
};


