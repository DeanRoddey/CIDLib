//
// CID_FILE NAME: CIDLogMon.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/28/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the main module of the program.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDLogMon.hpp"



// ----------------------------------------------------------------------------
//  Local, const data
// ----------------------------------------------------------------------------
namespace CIDLogMon
{
    // The max number of log items we will show
    const tCIDLib::TCard4   c4MaxItems = 1024;
}


// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread&, tCIDLib::TVoid*);


// ----------------------------------------------------------------------------
//  Magic main module code
// ----------------------------------------------------------------------------
CIDLib_CtrlsModule(TThread(L"MainThread", eMainThreadFunc))


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMainFrame, TFrameWnd)
AdvRTTIDecls(TItemListWnd, TMultiColListBox)



// ----------------------------------------------------------------------------
//   CLASS: TLogItem
//  PREFIX: li
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TLogItem: Constructors and Destructor
// ----------------------------------------------------------------------------
TLogItem::TLogItem(const TLogEvent& errSrc, const tCIDLib::TCard4 c4UniqueId) :

    TLogEvent(errSrc)
    , m_c4UniqueId(c4UniqueId)
{
}

TLogItem::TLogItem(const TLogItem& liSrc) :

    TLogEvent(liSrc)
    , m_c4UniqueId(liSrc.m_c4UniqueId)
{
}

TLogItem::~TLogItem()
{
}


// ----------------------------------------------------------------------------
//  TLogItem: Public operators
// ----------------------------------------------------------------------------
TLogItem& TLogItem::operator=(const TLogItem& liSrc)
{
    if (&liSrc != this)
    {
        TLogEvent::operator=(liSrc);
        m_c4UniqueId = liSrc.m_c4UniqueId;
    }
    return *this;
}



// ---------------------------------------------------------------------------
//  CLASS: TMCListItemOps
// PREFIX: mclops
//
//  This is a multi-column list box ops derivative that makes it convenient to
//  load our log event objects into the list box.
// ---------------------------------------------------------------------------
class TMCListItemOps : public TMCListOps
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructors
        // -------------------------------------------------------------------
        TMCListItemOps(const TRefVector<const TLogItem>* const pcolList) :

            m_pcolList(pcolList)
        {
            m_tmFmt.strDefaultFormat(TTime::strMMDD_24HHMMSS());
        }

        TMCListItemOps(const TMCListItemOps&) = delete;

        ~TMCListItemOps() {}


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMCListItemOps& operator=(const TMCListItemOps&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4ColCount() const override
        {
            return 6;
        }

        tCIDLib::TCard4 c4RowCount() const override
        {
            return m_pcolList->c4ElemCount();
        }

        tCIDLib::TCard4 c4RowId(const tCIDLib::TCard4 c4RowIndex) const override
        {
            // Give back the unique id assigned to this item
            return m_pcolList->pobjAt(c4RowIndex)->m_c4UniqueId;
        }

        const TString& strColumnText
        (
            const   tCIDLib::TCard4     c4RowIndex
            , const tCIDLib::TCard4     c4ColIndex
        )   const override;


        const TRefVector<const TLogItem>* const m_pcolList;
        mutable TPathStr                        m_pathTmp;
        mutable TString                         m_strTmpFmt;
        mutable TTime                           m_tmFmt;
};


const TString&
TMCListItemOps::strColumnText(  const   tCIDLib::TCard4     c4RowIndex
                                , const tCIDLib::TCard4     c4ColIndex) const
{
    const TLogItem& errCur = *m_pcolList->pobjAt(c4RowIndex);

    // Column zero is a custom drawn color fill
    switch(c4ColIndex)
    {
        case 1 :
            return errCur.strHostName();

        case 2 :
            return errCur.strProcess();

        case 3 :
            m_strTmpFmt.SetFormatted(errCur.c4LineNum());
            return m_strTmpFmt;

        case 4 :
            m_tmFmt.enctTime(errCur.enctLogged());
            m_tmFmt.FormatToStr(m_strTmpFmt);
            return m_strTmpFmt;

        case 5 :
            return errCur.strErrText();

        default :
            break;
    };
    return TString::strEmpty();
}



// ---------------------------------------------------------------------------
//  Global data
// ---------------------------------------------------------------------------
TGUIFacility facCIDLogMon
(
    L"CIDLogMon"
    , tCIDLib::EModTypes::Exe
    , kCIDLib::c4MajVersion
    , kCIDLib::c4MinVersion
    , kCIDLib::c4Revision
    , tCIDLib::EModFlags::HasMsgsAndRes
);



// ---------------------------------------------------------------------------
//  CLASS: TItemListWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TItemListWnd: Constructors and Destructor
// ---------------------------------------------------------------------------
TItemListWnd::TItemListWnd() :

    m_pwndParent(nullptr)
{
}

TItemListWnd::~TItemListWnd()
{
}

// ---------------------------------------------------------------------------
//  TItemListWnd: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TItemListWnd::SetParFrame(const TMainFrame* const pwndParent)
{
    m_pwndParent = pwndParent;
}


// ---------------------------------------------------------------------------
// TItemListWnd: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TItemListWnd::bCreated()
{
    TParent::bCreated();
    SetCustomDrawOpts
    (
        tCIDCtrls::EMCLBCustOpts
        (
            tCIDCtrls::EMCLBCustOpts::PreCol
        )
    );
    return kCIDLib::True;
}


tCIDCtrls::EMCLBCustRets
TItemListWnd::eCustomDraw(          TGraphDrawDev&      gdevTar
                            , const tCIDLib::TCard4     c4Row
                            , const tCIDLib::TCard4     c4Column
                            , const tCIDLib::TBoolean   bPost
                            , const TArea&              areaAt
                            ,       TRGBClr&            rgbBgn
                            ,       TRGBClr&            rgbText)
{
    tCIDCtrls::EMCLBCustRets eRet = tCIDCtrls::EMCLBCustRets::None;
    if (!bPost && m_pwndParent)
    {
        //
        //  Deal with cell drawing for column 0. We fill it with a color based
        //  on the severity of the msg.
        //
        if ((c4Row != kCIDLib::c4MaxCard) && (c4Column == 0))
        {
            // Find the target log event and figure out the color
            const TLogItem* perrCur = m_pwndParent->pliFindById(c4IndexToId(c4Row));
            if (perrCur)
            {
                switch(perrCur->eSeverity())
                {
                    case tCIDLib::ESeverities::Info :
                        rgbBgn = facCIDGraphDev().rgbGreen;
                        break;

                    case tCIDLib::ESeverities::Warn :
                        rgbBgn = facCIDGraphDev().rgbPaleGrey;
                        break;

                    case tCIDLib::ESeverities::Failed :
                    case tCIDLib::ESeverities::ProcFatal :
                    case tCIDLib::ESeverities::SysFatal :
                        rgbBgn = facCIDGraphDev().rgbRed;
                        break;

                    case tCIDLib::ESeverities::Status :
                        rgbBgn = facCIDGraphDev().rgbWhite;
                        break;

                    default :
                        break;
                };
                eRet = tCIDLib::eOREnumBits(eRet, tCIDCtrls::EMCLBCustRets::NewBgnClr);
            }
        }
    }
    return eRet;
}



// ----------------------------------------------------------------------------
//   CLASS: TMainFrame
//  PREFIX: wnd
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TTextFrameWnd: PConstructors and Destructor
// ----------------------------------------------------------------------------
TMainFrame::TMainFrame() :

    m_bShowFailures(kCIDLib::True)
    , m_bShowInfos(kCIDLib::True)
    , m_bShowStatuses(kCIDLib::True)
    , m_c4NextMsgId(0)
    , m_c4NextUID(1)
    , m_colList()
    , m_colLoad(tCIDLib::EAdoptOpts::NoAdopt, CIDLogMon::c4MaxItems / 2)
    , m_colNewEvents(CIDLogMon::c4MaxItems, tCIDLib::EMTStates::Safe)
    , m_colQuery()
    , m_porbcLogger(nullptr)
    , m_pwndAuxText(nullptr)
    , m_pwndClear(nullptr)
    , m_pwndClient(nullptr)
    , m_pwndClose(nullptr)
    , m_pwndErrors(nullptr)
    , m_pwndFacName(nullptr)
    , m_pwndFileName(nullptr)
    , m_pwndList(nullptr)
    , m_pwndSnap(nullptr)
    , m_pwndThreadName(nullptr)
    , m_thrPoll
      (
        L"LogPollThread"
        , TMemberFunc<TMainFrame>(this, &TMainFrame::ePollThread)
      )
{
}

TMainFrame::~TMainFrame()
{
}


// ----------------------------------------------------------------------------
//  TMainFrame: Public, non-virtual methods
// ----------------------------------------------------------------------------

//
//  This is a little redundant, since we have to load our dialog description in
//  bCreated() in order to create all the child controls. But, we want to know the
//  correct initial size of the window. So we load the dialog descr, use that to
//  calculate a correct size to fit that content, and the create ourself to that
//  size.
//
tCIDLib::TBoolean TMainFrame::bCreateMain()
{
    // We load our content from a dialog
    TDlgDesc dlgdCreate;
    if (!facCIDLogMon.bCreateDlgDesc(kLogMon::rid_Main, dlgdCreate))
        return kCIDLib::False;

    // Get the area of the content and adjust it to window units
    TArea areaDlg = dlgdCreate.areaPos();

    //
    //  Take that and get a frame area tht would fit it, given teh styles we are
    //  going to use and the fact that it will have a menu.
    //
    const tCIDCtrls::EWndStyles eWndStyles = tCIDCtrls::EWndStyles::StdFrameNV;
    TArea areaInit;
    TFrameWnd::AreaForClient
    (
        areaDlg
        , areaInit
        , eWndStyles
        , tCIDCtrls::EExWndStyles::None
        , kCIDLib::True
    );

    // Let the system pick the origin
    areaInit.i4X(kCIDCtrls::wcrDefault);
    areaInit.i4Y(kCIDCtrls::wcrDefault);

    CreateFrame
    (
        nullptr
        , areaInit
        , facCIDLogMon.strMsg(kLogMonMsgs::midTitle)
        , eWndStyles
        , tCIDCtrls::EExWndStyles::None
        , tCIDCtrls::EFrameStyles::StdFrame
        , kCIDLib::False
    );

    // And set our current size as our minimum size
    SetMinMaxSize(areaWndSize().szArea(), TSize(0, 0));

    return kCIDLib::True;
}


// Search the list for an item with this id
const TLogItem* TMainFrame::pliFindById(const tCIDLib::TCard4 c4IdToFind) const
{
    TDeque<TLogItem>::TCursor cursFind(&m_colList);
    if (!cursFind.bReset())
        return nullptr;

    do
    {
        if (cursFind.objRCur().m_c4UniqueId == c4IdToFind)
            return &cursFind.objRCur();
    }   while(cursFind.bNext());
    return nullptr;
}


// ----------------------------------------------------------------------------
//  TMainFrame: Protected, inherited method
// ----------------------------------------------------------------------------

tCIDLib::TVoid
TMainFrame::AreaChanged(const   TArea&                  areaPrev
                        , const TArea&                  areaNew
                        , const tCIDCtrls::EPosStates   ePosState
                        , const tCIDLib::TBoolean       bOrgChanged
                        , const tCIDLib::TBoolean       bSizeChanged
                        , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    //
    //  Resize our client window to fit, he will resize/pos any children that are set
    //  up for that.
    //
    if (bSizeChanged && (ePosState != tCIDCtrls::EPosStates::Minimized))
    {
        // We might not have created the client window the first time this happens
        if (m_pwndClient)
        {
            TArea areaClient;
            QueryClientArea(areaClient, kCIDLib::False);
            m_pwndClient->SetSizePos(areaClient, kCIDLib::False);
        }
    }
}


//
//  This is a simple program, that never creates anything that is required to be saved,
//  so we override this and just always say yes.
//
tCIDLib::TBoolean TMainFrame::bAllowShutdown()
{
    return kCIDLib::True;
}


//
//  When we are created, in turn create all of our child windows, which we do manually
//  in this simple test.
//
tCIDLib::TBoolean TMainFrame::bCreated()
{
    // Call our parent first
    TParent::bCreated();

    // Set our icon
    facCIDLogMon.SetFrameIcon(*this, L"LogMon");

    //
    //  Load our menu bar and assign it to us. Check all of our filter check boxes
    //  initially, so that we show all content.
    //
    SetMenuBar(facCIDLogMon, kLogMon::ridMenu_Main);
    {
        TMenuBar& menuFrame = menuCur();
        menuFrame.SetItemCheck(kLogMon::ridMenu_Filter_Info, kCIDLib::True);
        menuFrame.SetItemCheck(kLogMon::ridMenu_Filter_Failures, kCIDLib::True);
        menuFrame.SetItemCheck(kLogMon::ridMenu_Filter_Warnings, kCIDLib::True);
        menuFrame.SetItemCheck(kLogMon::ridMenu_Filter_Status, kCIDLib::True);
    }

    // Load up the dialog description we use to create our controls
    TDlgDesc dlgdCreate;
    facCIDLogMon.bCreateDlgDesc(kLogMon::rid_Main, dlgdCreate);

    // Create our client window, which is just a generic one
    TArea areaClient;
    QueryClientArea(areaClient, kCIDLib::True);

    m_pwndClient = new TGenericWnd();
    m_pwndClient->CreateGenWnd
    (
        *this
        , areaClient
        , tCIDCtrls::EWndStyles::ClippingVisChild
        , tCIDCtrls::EExWndStyles::ControlParent
        , kCIDCtrls::widFirstCtrl
    );


    //
    //  Get the correct area for a client of the size of the dialog contents, and
    //  size us to that.
    //
    TArea areaNew;
    AreaForClient
    (
        dlgdCreate.areaPos(), areaNew, eWndStyles(), eExWndStyles(), kCIDLib::True
    );
    SetSize(areaNew.szArea(), kCIDLib::True);

    // Find the list box item and change the C++ class that it maps to.
    dlgdCreate.SetCppType(kLogMon::ridMain_List, L"TItemListWnd");

    // Now we can create the dialog items
    tCIDCtrls::TWndId widInitFocus;
    m_pwndClient->PopulateFromDlg(dlgdCreate, widInitFocus);

    // Get pointers to the widgets we want to interact with
    CastChildWnd(*m_pwndClient, kLogMon::ridMain_AuxText, m_pwndAuxText);
    CastChildWnd(*m_pwndClient, kLogMon::ridMain_ClearButton, m_pwndClear);
    CastChildWnd(*m_pwndClient, kLogMon::ridMain_CloseButton, m_pwndClose);
    CastChildWnd(*m_pwndClient, kLogMon::ridMain_Errors, m_pwndErrors);
    CastChildWnd(*m_pwndClient, kLogMon::ridMain_FacName, m_pwndFacName);
    CastChildWnd(*m_pwndClient, kLogMon::ridMain_FileName, m_pwndFileName);
    CastChildWnd(*m_pwndClient, kLogMon::ridMain_List, m_pwndList);
    CastChildWnd(*m_pwndClient, kLogMon::ridMain_SnapButton, m_pwndSnap);
    CastChildWnd(*m_pwndClient, kLogMon::ridMain_ThreadName, m_pwndThreadName);

    // Give the list window a pointer to us
    m_pwndList->SetParFrame(this);

    // Register handlers
    m_pwndClear->pnothRegisterHandler(this, &TMainFrame::eClickHandler);
    m_pwndClose->pnothRegisterHandler(this, &TMainFrame::eClickHandler);
    m_pwndList->pnothRegisterHandler(this, &TMainFrame::eListHandler);
    m_pwndSnap->pnothRegisterHandler(this, &TMainFrame::eClickHandler);

    // Set up our columns. We have a good number of them
    tCIDLib::TStrList colColTitles(6);
    colColTitles.objAdd(L"");
    colColTitles.objAdd(L"Host");
    colColTitles.objAdd(L"Process");
    colColTitles.objAdd(L"Line");
    colColTitles.objAdd(L"Time");
    colColTitles.objAdd(L"Message Text");
    m_pwndList->SetColumns(colColTitles);

    // Set some obvious initial column sizes
    m_pwndList->SetColOpts(0, TGUIFacility::c4AdjustHDlgUnit(10));
    m_pwndList->SetColOpts(1, TGUIFacility::c4AdjustHDlgUnit(48));
    m_pwndList->SetColOpts(2, TGUIFacility::c4AdjustHDlgUnit(52));
    m_pwndList->SetColOpts(3, TGUIFacility::c4AdjustHDlgUnit(28));
    m_pwndList->SetColOpts(4, TGUIFacility::c4AdjustHDlgUnit(54));
    m_pwndList->SetColOpts(5, TGUIFacility::c4AdjustHDlgUnit(220));

    // And now set the correct initial show state state
    facCIDCtrls().ShowGUI(*this);

    // If there was an initial focus in the dialog desc, set it
    if (widInitFocus)
        m_pwndList->SetChildFocus(widInitFocus);

    // Kick off the polling thread
    m_thrPoll.Start();

    return kCIDLib::True;
}


tCIDLib::TBoolean TMainFrame::bEraseBgn(TGraphDrawDev&)
{
    return kCIDLib::True;
}


//
//  The stupid multi-column list box won't reliably draw his border. So we just
//  turn off his border and draw it ourself.
//
tCIDLib::TBoolean
TMainFrame::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    //
    //  Draw a border around the list control, which is too stupid to reliably draw
    //  it's own border in our double buffered setup.
    //
    TArea areaBorder = m_pwndList->areaWnd();
    areaBorder.Inflate(1, 1);
    TGraphWndDev gdevClient(*m_pwndClient);
    gdevClient.Stroke(areaBorder, facCIDGraphDev().rgbDarkGrey);

    return kCIDLib::True;
}


//
//  The polling thread sends us an async code to let us know that new items are
//  available in the list. We need to pull them out and get them into the list
//  box.
//
tCIDLib::TVoid
TMainFrame::CodeReceived(const  tCIDLib::TInt4  i4Code
                        , const tCIDLib::TCard4 c4Data)
{
    // Prevent repainting until we are done
    TWndPaintJanitor janPaint(m_pwndList);

    // We need to lock until we get the new events into our main list
    {
        TMtxLocker lockEvents(m_colNewEvents.pmtxLock());

        const tCIDLib::TCard4 c4Count = m_colNewEvents.c4ElemCount();
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            //
            //  If the deque is full, throw out the oldest one, from from the list.
            //  and, if it is currently displayed, remove it from there as well.
            //
            if (m_colList.bIsFull(CIDLogMon::c4MaxItems))
            {
                tCIDLib::TCard4 c4Id = m_colList.objPeekBottom().m_c4UniqueId;
                m_colList.objPopBottom();

                // See if it's in the list, don't throw if not
                const tCIDLib::TCard4 c4RemoveInd = m_pwndList->c4IdToIndex(c4Id, kCIDLib::False);
                if (c4RemoveInd != kCIDLib::c4MaxCard)
                    m_pwndList->RemoveAt(c4RemoveInd);
            }

            // Add this one one to the list
            TLogItem liTmp(m_colNewEvents[c4Index], m_c4NextUID++);
            m_colList.objPushTop(liTmp);
        }

        //
        //  Flush the new events list since we have them all now, Then the bgn thread
        //  is free to start loading it up again.
        //
        m_colNewEvents.RemoveAll();
    }

    //
    //  Now add any non-filtered ones to our load list. We can't do it above since
    //  items in the deque can get removed if it gets full. So that would invalidate
    //  our list (which is just referencing items in that deque.) We'd have to handle
    //  going back and removing them from this list. And this let's us release the lock
    //  as soon as the new events are in the deque.
    //
    m_colLoad.RemoveAll();
    TDeque<TLogItem>::TCursor cursDeque(&m_colList);
    if (cursDeque.bReset())
    {
        do
        {
            // If not filtered out, then get a ref
            if (!bFiltered(cursDeque.objRCur()))
                m_colLoad.Add(&cursDeque.objRCur());
        }   while (cursDeque.bNext());
    }

    //
    //  Now bulk load these new events into the output window. Indicate we want to
    //  append to it. Remember if it was empty, and do an initial selection after
    //  loading if so.
    //
    const tCIDLib::TBoolean bWasEmpty(m_pwndList->c4ItemCount() == 0);
    TMCListItemOps mclopsLoad(&m_colLoad);
    m_pwndList->LoadList(mclopsLoad, kCIDLib::True);

    // Keep us bottom scrolled
    m_pwndList->ScrollToBottom();

    // If we were empty, but not now, select the last one
    if (bWasEmpty && m_pwndList->c4ItemCount())
        m_pwndList->SelectByIndex(m_pwndList->c4ItemCount() - 1);
}


tCIDLib::TVoid TMainFrame::Destroyed()
{
    // Shut down the polling thread
    try
    {
        m_thrPoll.ReqShutdownSync(5000);
        m_thrPoll.eWaitForDeath(5000);
    }

    catch(TError& errToCatch)
    {
        if (!errToCatch.bLogged())
        {
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            TModule::LogEventObj(errToCatch);
        }
    }

    // Now close down our orb client, if any
    if (m_porbcLogger)
    {
        try
        {
            delete m_porbcLogger;
        }

        catch(TError& errToCatch)
        {
            if (!errToCatch.bLogged())
            {
                errToCatch.AddStackLevel(CID_FILE, CID_LINE);
                TModule::LogEventObj(errToCatch);
            }
        }
    }

    TParent::Destroyed();
}


tCIDLib::TVoid TMainFrame::DragEnd()
{
    //
    //  The stupid multi-column list box is a joke in composited systems, so after a
    //  resize, force it to redraw.
    //
    m_pwndList->Redraw(m_pwndClient->areaWndSize(), tCIDCtrls::ERedrawFlags::Full);
}


//
//  Handles menu bar and accelerator commands.
//
tCIDLib::TVoid
TMainFrame::MenuCommand(const   tCIDLib::TResId     ridItem
                        , const tCIDLib::TBoolean   bChecked
                        , const tCIDLib::TBoolean   bEnabled)
{
    if (ridItem == kLogMon::ridMenu_File_Save)
    {
        // Let the user save the currently displayed values to a file
        SnapToFile();
    }
     else if ((ridItem == kLogMon::ridMenu_Filter_Info)
          ||  (ridItem == kLogMon::ridMenu_Filter_Failures)
          ||  (ridItem == kLogMon::ridMenu_Filter_Warnings)
          ||  (ridItem == kLogMon::ridMenu_Filter_Status))
    {
        //
        //  Toggle the checked state of the selected item, then redisplay based
        //  on new filters.
        //
        menuCur().ToggleItemCheck(ridItem);
        UpdateFilters();
    }
     else if (ridItem == kLogMon::ridMenu_Filter_ShowAll)
    {
        // Turn all of the filters back on
        TMenuBar& menuBar = menuCur();
        menuBar.SetItemCheck(kLogMon::ridMenu_Filter_Info, kCIDLib::True);
        menuBar.SetItemCheck(kLogMon::ridMenu_Filter_Failures, kCIDLib::True);
        menuBar.SetItemCheck(kLogMon::ridMenu_Filter_Warnings, kCIDLib::True);
        menuBar.SetItemCheck(kLogMon::ridMenu_Filter_Status, kCIDLib::True);

        // And redisplay based on the new filters
        UpdateFilters();
    }
     else if (ridItem == kLogMon::ridMenu_File_Exit)
    {
        if (bAllowShutdown())
            facCIDCtrls().ExitLoop(0);
    }
}



// ----------------------------------------------------------------------------
//  TTextFrameWnd: Private, non-virtaul method
// ----------------------------------------------------------------------------

//
//  A helper to filter out log events from our list, to see if they should go into
//  the actual display window. We let them filter by severity. We return true if it
//  is filtered out.
//
tCIDLib::TBoolean TMainFrame::bFiltered(const TLogEvent& logevToCheck)
{
    tCIDLib::TBoolean bRet = kCIDLib::False;

    switch(logevToCheck.eSeverity())
    {
        case tCIDLib::ESeverities::Info :
            bRet = !m_bShowInfos;
            break;

        case tCIDLib::ESeverities::Warn :
            bRet = !m_bShowWarnings;
            break;

        case tCIDLib::ESeverities::Failed :
        case tCIDLib::ESeverities::ProcFatal :
        case tCIDLib::ESeverities::SysFatal :
            bRet = !m_bShowFailures;
            break;

        case tCIDLib::ESeverities::Status :
            bRet = !m_bShowStatuses;
            break;

        default :
            break;
    };
    return bRet;
}


// Handle button clicks
tCIDCtrls::EEvResponses
TMainFrame::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDCtrls::widCancel)
    {
        if (bAllowShutdown())
            facCIDCtrls().ExitLoop(0);
    }
     else if (wnotEvent.widSource() == kLogMon::ridMain_ClearButton)
    {
        // Clear our list and remove all items from the list box
        m_colList.RemoveAll();
        m_pwndList->RemoveAll();
    }
     else if (wnotEvent.widSource() == kLogMon::ridMain_SnapButton)
    {
        SnapToFile();
    }
    return tCIDCtrls::EEvResponses::Handled;
}


//
//  We have some info that's only displayed for the selected item, so we have to
//  watch for selection changes.
//
tCIDCtrls::EEvResponses
TMainFrame::eListHandler(TListChangeInfo& wnotEvent)
{
    if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::SelChanged)
    {
        // Find the item with the incoming id
        const TLogItem* pliSel = pliFindById(wnotEvent.c4Id());
        if (pliSel)
        {
            // We found it so update our display values
            m_pwndFacName->strWndText(pliSel->strFacName());
            m_pwndFileName->strWndText(pliSel->strFileName());
            m_pwndThreadName->strWndText(pliSel->strThread());

            m_strTmpFmt.SetFormatted(pliSel->errcId());
            m_strTmpFmt.Append(kCIDLib::chForwardSlash);
            m_strTmpFmt.AppendFormatted(pliSel->errcKrnlId());
            m_strTmpFmt.Append(kCIDLib::chForwardSlash);
            m_strTmpFmt.AppendFormatted(pliSel->errcHostId());
            m_pwndErrors->strWndText(m_strTmpFmt);

            if (pliSel->bHasAuxText())
                m_pwndAuxText->strWndText(pliSel->strAuxText());
            else
                m_pwndAuxText->ClearText();
        }
         else
        {
            // Shouldn't happen, but clear them if so
            m_pwndAuxText->ClearText();
            m_pwndErrors->ClearText();
            m_pwndFacName->ClearText();
            m_pwndFileName->ClearText();
            m_pwndThreadName->ClearText();
        }
    }
     else if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::Cleared)
    {
        m_pwndAuxText->ClearText();
        m_pwndErrors->ClearText();
        m_pwndFacName->ClearText();
        m_pwndFileName->ClearText();
        m_pwndThreadName->ClearText();
    }
    return tCIDCtrls::EEvResponses::Handled;
}


//
//  We start our polling thread on this method. He just checks periodically
//  for new log contents to be available from the log server. If so, he adds
//  then to our list and spits out any that aren't filtered out out to the
//  display window.
//
tCIDLib::EExitCodes
TMainFrame::ePollThread(TThread& thrThis, tCIDLib::TVoid* pData)
{
    // Let the calling thread go
    thrThis.Sync();

    TOrbObjId ooidSrv;
    while (kCIDLib::True)
    {
        // Sleep for a bit. If interrupted by a shutdown request, just exit
        if (!thrThis.bSleep(2000))
            break;

        try
        {
            // Do a poll cycle, or try to reconnect if disconnected
            if (!m_porbcLogger)
            {
                m_porbcLogger = facCIDOrbUC().porbcMakeClient<TCIDLogSrvClientProxy>
                (
                    TCIDLogSrvClientProxy::strBinding, 1000
                );
            }

            // If we had or got a logger, let's get the most recent msgs
            if (m_porbcLogger)
                GetNewMsgs();
        }

        catch(const TError&)
        {
            delete m_porbcLogger;
            m_porbcLogger = 0;
            m_c4NextMsgId = 0;
        }

        catch(...)
        {
            delete m_porbcLogger;
            m_porbcLogger = 0;
            m_c4NextMsgId = 0;
        }
    }
    return tCIDLib::EExitCodes::Normal;
}


//
//  The polling thread calls this periodically to check for new message to be
//  available, if it's connected to the log server.
//
tCIDLib::TVoid TMainFrame::GetNewMsgs()
{
    //
    //  Ask for the latest messages. If new ones are available, he'll update
    //  the message id to the id of the last one we got.
    //
    if (m_porbcLogger->bGetLiveEvents(m_c4NextMsgId, m_colQuery))
    {
        //
        //  We got some, so lock the new events collection add these to it
        //  and send (async) a message to the GUI thread that new stuff is
        //  available for it to display.
        //
        //  Note that we ADD here, we don't clean it out. Worst case, the
        //  GUI thread may have not gotten the last ones we stored, so we
        //  just new ones. He cleans it out when he grabs them for display.
        //
        TMtxLocker lockEvents(m_colNewEvents.pmtxLock());

        try
        {
            tCIDLib::TCard4 c4Count = m_colQuery.c4ElemCount();
            for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
            {
                //
                //  If the new events list is full, output an error msg of our own to
                //  indicate we missed some messages.
                //
                if (m_colNewEvents.bIsFull(CIDLogMon::c4MaxItems * 2))
                {
                    m_colNewEvents.RemoveAt(0);
                    TLogEvent errFull
                    (
                        facCIDLogMon.strName()
                        , CID_FILE
                        , CID_LINE
                        , L"The msg input queue is full, so some messages were dropped"
                        , tCIDLib::ESeverities::Failed
                        , tCIDLib::EErrClasses::Internal
                    );
                    m_colNewEvents.objAdd(errFull);
                    break;
                }
                m_colNewEvents.objAdd(m_colQuery[c4Index]);
            }
        }

        catch(TError& errToCatch)
        {
            // Don't log it, just put it into the list
            errToCatch.AddStackLevel(CID_FILE, CID_LINE);
            if (m_colNewEvents.bIsFull(CIDLogMon::c4MaxItems * 2))
                m_colNewEvents.RemoveAt(0);
            m_colNewEvents.objAdd(errToCatch);
        }

        // Let the GUI thread there are items to display
        SendCode(1, 0);
    }
}


//
//  The user can snap the contents of the display window to a text file for
//  sending to us.
//
tCIDLib::TVoid TMainFrame::SnapToFile()
{
    // If not previous path, provide a default
    if (m_pathLastSavePath.bIsEmpty())
        m_pathLastSavePath = TSysInfo::strSpecialPath(tCIDLib::ESpecialPaths::UserAppData);

    // Get the target file
    tCIDLib::TKVPList colTypes(1);
    colTypes.objAdd(TKeyValuePair(L"Text Files", L"*.Txt"));

    const TString strTitle(kLogMonMsgs::midPrompt_SelSnapPath, facCIDLogMon);
    TString strTarget;
    tCIDLib::TBoolean bRes = facCIDCtrls().bSaveFileDlg
    (
        *this
        , strTitle
        , m_pathLastSavePath
        , L"CIDLog.Txt"
        , strTarget
        , colTypes
        , tCIDLib::eOREnumBits
          (
            tCIDCtrls::EFSaveOpts::FilesOnly
            , tCIDCtrls::EFSaveOpts::ConfirmPrompt
            , tCIDCtrls::EFSaveOpts::StrictTypes
          )
    );

    if (!bRes)
        return;

    try
    {
        //
        //  Open a text output stream for the target path. Use a native
        //  wide character transcoder so that they can read it on their
        //  machine before sending it.
        //
        TTextFileOutStream strmOut(new TUTF8Converter);
        strmOut.Open
        (
            strTarget
            , tCIDLib::ECreateActs::CreateAlways
            , tCIDLib::EFilePerms::AllAccess
            , tCIDLib::EFileFlags::SequentialScan
            , tCIDLib::EAccessModes::Excl_OutStream
        );
        strmOut << kCIDLib::chUniBOM;

        //
        //  Go through our list and for any items in use and not filtered
        //  by the current filter settings, write it out.
        //
        TTime tmLogged;
        tmLogged.strDefaultFormat(L"%(M,2,0)/%(D,2,0) %(H,2,0):%(u,2,0):%(s,2,0)");

        TDeque<TLogItem>::TCursor cursSave(&m_colList);
        if (cursSave.bReset())
        {
            do
            {
                if (!bFiltered(cursSave.objRCur()))
                    cursSave.objRCur().AdvFormat(strmOut, tmLogged);
            }   while (cursSave.bNext());
        }
        strmOut.Flush();
        strmOut.Close();

        // Save the path part of the target as the last save path
        m_pathLastSavePath = strTarget;
        m_pathLastSavePath.bRemoveNameExt();

        // Let them know it was saved
        TOkBox msgbSaved
        (
            strWndText(), facCIDLogMon.strMsg(kLogMonMsgs::midPrompt_SavedOk)
        );
        msgbSaved.ShowIt(*this);
    }

    catch(const TError& errToCatch)
    {
        // If something goes awry, then tel the user
        TErrorDlg dlgFailed(*this, errToCatch);
    }
}




//
//  When the user changes the filter settings in the menu bar, we are called here to
//  refilter the list.
//
tCIDLib::TVoid TMainFrame::UpdateFilters()
{
    // Match our boolean flags with the checked states of the filter menu items
    TMenuBar& menuBar = menuCur();

    m_bShowFailures = menuBar.bIsItemChecked(kLogMon::ridMenu_Filter_Failures);
    m_bShowInfos    = menuBar.bIsItemChecked(kLogMon::ridMenu_Filter_Info);
    m_bShowStatuses = menuBar.bIsItemChecked(kLogMon::ridMenu_Filter_Status);
    m_bShowWarnings = menuBar.bIsItemChecked(kLogMon::ridMenu_Filter_Warnings);

    // Clear the list box, and reload with any that are not filtered out.
    m_pwndList->RemoveAll();
    m_colLoad.RemoveAll();
    TDeque<TLogItem>::TCursor cursFind(&m_colList);
    if (cursFind.bReset())
    {
        do
        {
            if (!bFiltered(cursFind.objRCur()))
                m_colLoad.Add(&cursFind.objRCur());
        }   while (cursFind.bNext());
    }

    TMCListItemOps mclopsLoad(&m_colLoad);
    m_pwndList->LoadList(mclopsLoad, kCIDLib::False);

    // If not empty, then select the last one
    if (m_pwndList->c4ItemCount())
    {
        // Keep us bottom scrolled
        m_pwndList->MakeItemVisible(m_pwndList->c4ItemCount() - 1, kCIDLib::True);

        m_pwndList->SelectByIndex(m_pwndList->c4ItemCount() - 1);
    }
}


// ----------------------------------------------------------------------------
//  Local functions
// ----------------------------------------------------------------------------
tCIDLib::EExitCodes eMainThreadFunc(TThread& thrThis, tCIDLib::TVoid*)
{
    // We have to let our calling thread go first
    thrThis.Sync();

    try
    {
        //
        //  Check to see if there is already an instance running. If so, activate
        //  it and just exit.
        //
        {
            TResourceName rsnInstance(L"CQSL", L"CIDLogMon", L"SingleInstanceInfo");
            if (TProcess::bSetSingleInstanceInfo(rsnInstance, kCIDLib::True))
                return tCIDLib::EExitCodes::Normal;
        }

        // Initialize the ORB client
        facCIDOrb().InitClient();

        // Create the main frame window and enter the msg loop
        TMainFrame wndMain;
        if (wndMain.bCreateMain())
            facCIDCtrls().uMainMsgLoop(wndMain);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
    }

    return tCIDLib::EExitCodes::Normal;
}


