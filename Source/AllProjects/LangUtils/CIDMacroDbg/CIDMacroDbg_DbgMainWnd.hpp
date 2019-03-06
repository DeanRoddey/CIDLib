//
// FILE NAME: CIDMacroDbg_DbgMainWnd.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/14/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the file file CIDMacroDlg_DbgMainWnd.cpp, which
//  implements the main window of the debugger. Note that the debugger is
//  always nested within some other application, which is why it is implemented
//  as a library. This top level window is a derivative of the pane window
//  manager control and it contains the other debugger windows within it. It
//  provides methods to make it do things and depends on the app to provide
//  the menu items or other means for the user to make them happen (though
//  we provide some hot key support here.)
//
//  This window, being derived from the standard pane window container control,
//  manages tiled panes. The top one is the man source viewer window, and the bottom
//  is a tabbed window that contains all of the various status output windows.
//
//  The caller has to provide us with path and file resolvers, which we adopt.
//  These are used during parsing and to do checkout/load operations of macro
//  source as it is required, and to resolve macro file paths during runtime.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TMacroDbgSrcViewWnd;
class TBreakPntsTab;
class TBuildResTab;
class TTextOutTab;
class TObjListTab;
class TSearchTab;
class TSrcTab;
class TPrsErrPosInfo;


// ---------------------------------------------------------------------------
//   CLASS: TCMLDbgChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDMACRODBGEXP TCMLDbgChangeInfo : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCMLDbgChangeInfo() = delete;

        TCMLDbgChangeInfo
        (
            const   tCIDMacroDbg::EEvents   eEvent
            , const TString&                strCurFile
            , const tCIDLib::TCard4         c4LineNum
            , const TWindow&                wndSrc
        );

        TCMLDbgChangeInfo
        (
            const   TString&                strNewState
            , const TWindow&                wndSrc
        );

        TCMLDbgChangeInfo
        (
            const   TCMLDbgChangeInfo&      wnotToCopy
        );

        ~TCMLDbgChangeInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCMLDbgChangeInfo& operator=
        (
            const   TCMLDbgChangeInfo&      wnotToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4LineNum() const;

        tCIDMacroDbg::EEvents eEvent() const;

        const TString& strCurFile() const;

        const TString& strState() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4LineNum
        //      The current line in the current file. It will be 1 if there is no current
        //      file.
        //
        //  m_eEvent
        //      Indicates the particular type of event.
        //
        //  m_strCurFile
        //      The path to the current file. If there is no current file, then it will
        //      be max card.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4LineNum;
        tCIDMacroDbg::EEvents   m_eEvent;
        TString                 m_strCurFile;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCMLDbgChangeInfo,TCtrlNotify)
        DefPolyDup(TCMLDbgChangeInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TCMLCustomTab
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDMACRODBGEXP TCMLCustomTab : public TTabWindow
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TCMLCustomTab(const TCMLCustomTab&) = delete;

        ~TCMLCustomTab();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCMLCustomTab& operator=(const TCMLCustomTab&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid RunStatusChanged
        (
            const   tCIDLib::TBoolean       bNewState
        ) = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TCMLCustomTab();


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCMLCustomTab, TTabWindow)
};


// ---------------------------------------------------------------------------
//  CLASS: TMacroDbgMainWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDMACRODBGEXP
TMacroDbgMainWnd : public TPaneWndCont, public MMEngClassMgr, public MMEngDebugIntf
{
    public  :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TNotificationId        nidChange;


        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TMacroDbgMainWnd
        (
                    MMEngClassMgr* const    pmecmToUse
            ,       MMEngFileResolver* const pmefrToUse
        );

        TMacroDbgMainWnd(const TMacroDbgMainWnd&) = delete;

        ~TMacroDbgMainWnd();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMacroDbgMainWnd& operator=(const TMacroDbgMainWnd&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bProcessAccel
        (
            const   tCIDLib::TVoid* const   pMsgData
        )   const override;

        tCIDLib::TBoolean bSleepTest() const override;

        tCIDLib::TVoid CallStackChange() override;

        tCIDMacroEng::EDbgActs eAtLine
        (
            const   TMEngClassInfo&         meciCurrent
            , const TMEngMethodInfo&        methiCurrent
            , const TMEngOpMethodImpl&      methCurrent
            ,       TMEngClassVal&          mecvCurrent
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TCard4         c4CurIP
        )   override;

        tCIDLib::TVoid FinishedDbg
        (
            const   tCIDMacroEng::EDbgActs     eReason
        )   override;

        tCIDLib::TVoid LocalsChange
        (
            const   tCIDLib::TBoolean       bCreated
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddCustomTab
        (
                    TCMLCustomTab* const    pwndToAdopt
            , const TString&                strTabText
        );

        tCIDLib::TVoid AddRemoveBP
        (
            const   TString&                strClassPath
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TBoolean       bState
        );

        tCIDLib::TBoolean bAnyChanges() const;

        tCIDLib::TBoolean bCloseSession();

        tCIDLib::TBoolean bIsOpen
        (
            const   TString&                strClassPath
        )   const;

        tCIDLib::TBoolean bOpenSession
        (
            const   TString&                strClassPath
            , const tCIDLib::TBoolean       bNewFile = kCIDLib::False
        );

        tCIDLib::TBoolean bQuerySessionClasses
        (
                    tCIDLib::TStrCollect&   colToFill
        );

        tCIDLib::TBoolean bSaveAll
        (
                    TString&                strErr
        );

        tCIDLib::TBoolean bSessionOpen() const;

        tCIDLib::TBoolean bSessionOpen
        (
                    TString&                strClassPath
        )   const;

        tCIDLib::TVoid CloseAll();

        tCIDMacroEng::EResModes eCurSrcMode() const;

        tCIDLib::TVoid EnableBP
        (
            const   TString&                strClassPath
            , const tCIDLib::TCard4         c4LineNum
            , const tCIDLib::TBoolean       bState
        );

        tCIDLib::TVoid ForceTabClosed
        (
            const   TString&                strClassPath
        );

        tCIDLib::TVoid Goto
        (
            const   TString&                strClassPath
            , const tCIDLib::TCard4         c4Line
        );

        tCIDLib::TVoid Init
        (
            const   TMacroDbgStateInfo&     stiToUse
            , const TCIDUserCtx* const      pcuctxRights
        );

        tCIDLib::TVoid QueryStateInfo
        (
                    TMacroDbgStateInfo&     stiToFill
        );

        const TCIDMacroEngine& meIDE() const;

        TCIDMacroEngine& meIDE();

        tCIDLib::TVoid OpenNewFile
        (
            const   TString&                strClassPath
            , const tCIDLib::TBoolean       bEditMode
        );

        const TSrcTab* pwndCurSrcTab() const;

        TSrcTab* pwndCurSrcTab();

        tCIDLib::TVoid RemoveAllBPs();

        tCIDLib::TVoid RemoveAllSrcBPs
        (
            const   TString&                strClassPath
        );

        const TString& strCurClassPath() const;

        tCIDLib::TVoid StopSession();


        // -------------------------------------------------------------------
        //  We provide a templatized method to allow the outside world to register
        //  for some events. We keep it up to date with changes in current file and
        //  current line number for the current file.
        // -------------------------------------------------------------------
        template <class T> TNotHandlerBase*
        pnothRegisterHandler(       T* const pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TCMLDbgChangeInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(this, pTarget);
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, TCMLDbgChangeInfo>(pTarget, TMacroDbgMainWnd::nidChange)
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TCMLDbgChangeInfo>
            (
                pTarget, TMacroDbgMainWnd::nidChange, pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methodsd
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCheckIfExists
        (
            const   TString&                strToCheck
        )   override;

        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TVoid CodeReceived
        (
            const   tCIDLib::TInt4          i4Code
            , const tCIDLib::TCard4         c4Data
        )   override;

        tCIDLib::TVoid Destroyed() override;

        tCIDLib::TVoid DoStore
        (
            const   TString&                strClassPath
            , const TString&                strText
        )   override;

        tCIDLib::TVoid DoUndoWriteMode
        (
            const   TString&                strClassPath
        )   override;

        tCIDLib::EOpenRes eDoSelect
        (
                    TString&                strToFill
            , const tCIDMacroEng::EResModes    eMode
        )   override;

        tCIDCtrls::ECtxMenuOpts eShowContextMenu
        (
            const   TPoint&                 pntAt
            , const tCIDCtrls::TWndId       widSrc
        )   override;

        tCIDLib::TVoid MenuCommand
        (
            const   tCIDLib::TResId         ridItem
            , const tCIDLib::TBoolean       bChecked
            , const tCIDLib::TBoolean       bEnabled
        )   override;

        TTextInStream* pstrmDoLoad
        (
            const   TString&                strClassPath
            , const tCIDMacroEng::EResModes    eMode
        )   override;

        tCIDLib::TVoid Timer
        (
            const   tCIDCtrls::TTimerId     tmidSrc
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        // -------------------------------------------------------------------
        enum class EActionReqs
        {
            None
            , Break
            , BreakAndGo
            , Exit
        };

        enum class EGoActions
        {
            Go
            , Compile
            , StepInto
            , StepOver
        };

        enum class EOneTimes
        {
            None
            , Hit
            , Step
        };

        enum class EStates
        {
            NoSession
            , Editing
            , Running
            , Stopped
        };

        typedef TFundVector<tCIDLib::TCard4> TPrevLineList;



        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses ePrsErrHandler
        (
                    TPrsErrPosInfo&         wnotEvent
        );

        tCIDCtrls::EEvResponses eSrcEdHandler
        (
                    TSrcEdChangeInfo&       wnotEdit
        );

        tCIDCtrls::EEvResponses eSrcTabHandler
        (
                    TTabEventInfo&          wnotEdit
        );

        tCIDLib::EExitCodes eRunThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::TVoid ExportClass();

        tCIDLib::TVoid GoOrRestart
        (
            const   EGoActions              eAction
        );

        tCIDLib::TVoid MakeShortName
        (
            const   TString&                strClassPath
            ,       TString&                strTabText
        );

        tCIDLib::TVoid OpenMacro
        (
            const   tCIDLib::TBoolean       bForEdit
        );

        TSrcTab* pwndLoadFileToTab
        (
            const   TString&                strClassPath
            , const tCIDMacroEng::EResModes    eMode
        );

        TSrcTab* pwndLoadNewFileToTab
        (
            const   TString&                strClassPath
        );

        const TSrcTab* pwndFindSrcTab
        (
            const   TString&                strClassPath
        )   const;

        TSrcTab* pwndFindSrcTab
        (
            const   TString&                strClassPath
        );

        const TSrcTab* pwndSrcTabAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        TSrcTab* pwndSrcTabAt
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid RemoveCurLine
        (
            const   tCIDLib::TCard4         c4Exclude
        );

        tCIDLib::TVoid SaveAll();

        tCIDLib::TVoid SendFileEvent
        (
            const   TString&                strCurFile
            , const tCIDLib::TCard4         c4LineNum
        );

        tCIDLib::TVoid SendLineEvent
        (
            const   tCIDLib::TCard4         c4LineNum
        );

        tCIDLib::TVoid SendStateEvent
        (
            const   TString&                strState
        );

        tCIDLib::TVoid SetCurEditPos
        (
            const   TString&                strClassPath
            , const tCIDLib::TCard4         c4Line
            , const tCIDLib::TCard4         c4Col
            , const tCIDLib::TBoolean       bSetCurLine
        );

        tCIDLib::TVoid SetEditorROState
        (
            const   tCIDLib::TBoolean       bReadOnly
        );

        tCIDLib::TVoid SetObjWndRadix
        (
            const   tCIDLib::ERadices       eToSet
        );

        tCIDLib::TVoid SetState
        (
            const   EStates                 eToSet
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_accelDbg
        //      Our accelerator table. The client program must call us to process
        //      accelerators. We pass it on to the active source tab. If it does not process
        //      it, we check it see if it's one of ours. Else we let it propogate back to the
        //      client program.
        //
        //  m_c2OneTimeMethod
        //      If we are doing a step over one time break (indicated by the m_eOneTimeFlag
        //      member), this is the method we started in and therefore the one we need to
        //      hit in order to break.
        //
        //  m_c4NewIP
        //      This is the new IP address that was passed in by the debugger interface,
        //      i.e. the offset of the opcode in the method.
        //
        //  m_c4LastLine
        //  m_c4NewLine
        //      Along wtih m_pmeciLast and m_pmethiLast, these members are set by the
        //      debugger callback, each time it hits a line number opcode.
        //
        //  m_c4OneTimeLine
        //      Set to a line number when a one time break point is done, so that we can
        //      watch for this line to be hit. Used with the other OneTime members.
        //
        //  m_c4LastStackUpdate
        //  m_c4StackChanges
        //      In order to keep from updating the call stack window every time, we bump
        //      this every time we get a call stack change callback from the engine. When
        //      we break, and the changes value is larger than the last value, then we need
        //      to update the call stack window.
        //
        //      We need this to get new macro console output strings, and perhaps for some
        //      other temp purposes.
        //
        //  m_c4TabId_Session
        //      We remember the id of the session tab when we add it, since we treat it
        //      specially.
        //
        //  m_eActionReq
        //      This is used by the GUI to ask the runner thread to do things, such as break
        //      or exit. The GUI sets it, and the runner thread will see it and do the
        //      appropriate thing.
        //
        //  m_eCurState
        //      Our current state, which tells us how to set the menus, how to respond to
        //      key strokes, and so forth.
        //
        //  m_evSync
        //      We use an event and mutex to sync with the thread that runs the macro. The
        //      event is used by the thread to block himself while he waits for us to attend
        //      to whatever made him stop.
        //
        //  m_menuContext
        //      Our context menu. They click on the editor, but it will let the invocation
        //      propogate up to us.
        //
        //  m_meDebug
        //      The macro engine we use for debugging. We just reuse it over and over as
        //      long as this debugger instance is opened.
        //
        //  m_meprsDebug
        //      The macro parser use to parse the classes upon go.
        //
        //  m_mtxSync
        //      We use a mutex and an event to sync with the thread that runs the macro.
        //      The mutex is used to adjust data without stepping on each other's toes. It
        //      is also used by the runner thread to lock the file list while it checks to
        //      see if there is a break point on the current line, so we have to lock this
        //      before removing a file from the list (which can happen while the macro is
        //      running.)
        //
        //  m_pcuctxRights
        //      The client app that creates us has to provide us with a user rights object.
        //
        //  m_pmeciMain
        //      The main class point is store here after a parse. The runner thread picks
        //      this up to start the run.
        //
        //  m_pmeciLast
        //  m_pmethiLast
        //  m_pmethLast
        //  m_pmecvLast
        //      These are the previous class, method info, and value objects that the
        //      debugger callback gave us in the AtLine() callback. These are related to
        //      the m_c4LastLine member, which stores the line number it was at. These will
        //      have just been superseded by the New versions below.
        //
        //  m_pmeciNew
        //  m_pmethiNew
        //  m_pmethNew
        //  m_pmecvNew
        //      The new class, method info/impl, and value objects that the debugger callback
        //      gave us in the AtLine() callback. These supersede the Last versions above,
        //      which store the previous information.
        //
        //  m_pmecmToUse
        //  m_pmefrToUse
        //      Whoever creates us provides us with a file resolver and class manaager. We
        //      don't own them, we just use them.
        //
        //      NOTE THAT we implement the class manager mixin and pass ourself to the macro
        //      engine as the class manager. This lets us intercept all the class manager calls
        //      and do things we need to do. Then we delegate the actual work to the caller's
        //      class manager.
        //
        //      SO DON'T call the caller's class manager directly. Call the methods on us
        //      instead. They will ultimately get to the caller's guy.
        //
        //  m_pwndTabsSrc
        //  m_pwndTabsStatus
        //      These are the two windows that we make our top and bottom panes. The top
        //      one contains a tab per CML class opened. And the bottom contains a tab for
        //      each of the specialized output windows that shows the state of the session.
        //
        //      The first source tab is the main class of the session.
        //
        //  m_pwndCustomTab
        //      We allow the containing application to insert a single custom tab of his
        //      own. We adopt it, but he can of course keep a reference and keep it up to
        //      date. He should only do that from the GUI thread of course, and typically
        //      it will be driven purely by his own data. It must derive from our simple
        //      TCMLCustomTab class, so that we can make some calls to it, to let it know
        //      when we are running or stopped and such.
        //
        //      He should create the object, but not the window. We will create the tab
        //      window, since that requires passing it to the tabbed window control that
        //      we don't expose.
        //
        //  m_pwndTabBuildRes
        //  m_pwndTabConOut
        //  m_pwndTabLocals
        //  m_pwndTabMembers
        //      These are all child tabs of one of the two main tabbed windows, but some we
        //      need to interact with more than others so we keep them around.
        //
        //  m_strCmdLineParms
        //      The command line parameters to pass to the macro entry point. We prompt the
        //      user to enter these if the entry point has any parms.
        //
        //  m_strCurClassPath
        //      The class path of our session macro. This is the one source file that we
        //      won't allow the user to close while we are debugging it. If m_bSessionOpen
        //      is false, then this is ignored.
        //
        //  m_strmConStream
        //      This is a queued text output stream. It's an output text stream that dumps
        //      to a queue of strings. It is designed to allow a GUI app to get text stream
        //      output from a background thread and get it into a window. We install this on
        //      the engine as the console stream. When we start running the macro, we start a
        //      timer which periodically checks this stream and pulls new messages out and
        //      puts them into the macro output window.
        //
        //  m_tmidMacroOut
        //      We spin up a timer when we are running, to pull macro output from the
        //      m_strmConStream stream and write it to the macro output tab.
        //
        //  m_thrRun
        //      The thread that runs the macro. We start it on a private method of this
        //      class, to make it easy to interact with.
        // -------------------------------------------------------------------
        TAccelTable                 m_accelDbg;
        tCIDLib::TCard2             m_c2OneTimeMethod;
        tCIDLib::TCard4             m_c4GoToLine;
        tCIDLib::TCard4             m_c4LastLine;
        tCIDLib::TCard4             m_c4NewIP;
        tCIDLib::TCard4             m_c4NewLine;
        tCIDLib::TCard4             m_c4LastStackUpdate;
        tCIDLib::TCard4             m_c4OneTimeLine;
        tCIDLib::TCard4             m_c4StackChanges;
        tCIDLib::TCard4             m_c4TabId_Session;
        tCIDLib::TStrList           m_colTmpStrs;
        EActionReqs                 m_eActionReq;
        EStates                     m_eCurState;
        EOneTimes                   m_eOneTimeFlag;
        TEvent                      m_evSync;
        TPopupMenu                  m_menuContext;
        TCIDMacroEngine             m_meDebug;
        TMacroEngParser             m_meprsDebug;
        TMutex                      m_mtxSync;
        const TCIDUserCtx*          m_pcuctxRights;
        const TMEngClassInfo*       m_pmeciLast;
        const TMEngClassInfo*       m_pmeciNew;
        TMEngClassInfo*             m_pmeciMain;
        MMEngClassMgr*              m_pmecmToUse;
        TMEngClassVal*              m_pmecvLast;
        TMEngClassVal*              m_pmecvNew;
        const TMEngOpMethodImpl*    m_pmethLast;
        const TMEngOpMethodImpl*    m_pmethNew;
        const TMEngMethodInfo*      m_pmethiLast;
        const TMEngMethodInfo*      m_pmethiNew;
        MMEngFileResolver*          m_pmefrToUse;
        TCMLCustomTab*              m_pwndCustomTab;
        TBreakPntsTab*              m_pwndTabBreakPnts;
        TBuildResTab*               m_pwndTabBuildRes;
        TTextOutTab*                m_pwndTabConOut;
        TObjListTab*                m_pwndTabLocals;
        TObjListTab*                m_pwndTabMembers;
        TObjListTab*                m_pwndTabParms;
        TSearchTab*                 m_pwndTabSearch;
        TTabbedWnd*                 m_pwndTabsSrc;
        TTabbedWnd*                 m_pwndTabsStatus;
        TString                     m_strCmdLineParms;
        TString                     m_strCurClassPath;
        TString                     m_strGoToClass;
        TString                     m_strOneTimeClass;
        TString                     m_strRepStr;
        TString                     m_strTitleText;
        TString                     m_strTmpOutput;
        TTextQOutStream             m_strmConStream;
        tCIDCtrls::TTimerId         m_tmidMacroOut;
        TThread                     m_thrRun;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMacroDbgMainWnd,TPaneWndCont)
};

#pragma CIDLIB_POPPACK


