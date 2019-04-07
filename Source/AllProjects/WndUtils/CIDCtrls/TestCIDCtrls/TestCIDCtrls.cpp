//
// CID_FILE NAME: TestCIDCtrls.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/28/1997
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
//  This is the main module of the program. This program tests the functionality of
//  the CIDCtrls facility.
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
#include    "TestCIDCtrls.hpp"



// ----------------------------------------------------------------------------
//  Local, const data
// ----------------------------------------------------------------------------


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
RTTIDecls(TTestFrameWnd, TFrameWnd)


// ---------------------------------------------------------------------------
//  Global data
// ---------------------------------------------------------------------------
TGUIFacility facTestCIDCtrls
(
    L"TestCIDCtrls"
    , tCIDLib::EModTypes::Exe
    , kCIDLib::c4MajVersion
    , kCIDLib::c4MinVersion
    , kCIDLib::c4Revision
    , tCIDLib::EModFlags::HasMsgsAndRes
);



// ---------------------------------------------------------------------------
//  CLASS: TTestDlgBox
// PREFIX: wnd
// ---------------------------------------------------------------------------
class TTestDlgBox : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTestDlgBox() {}

        ~TTestDlgBox() {}


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TTestDlgBox(const TTestDlgBox&);
        tCIDLib::TVoid operator=(const TTestDlgBox&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pwndXXX
        //      We get pointers to the widgets we need to interact with. We don't
        //      own them. They will be destroyed when the dialog is destroyed.
        // -------------------------------------------------------------------
        TPushButton* m_pwndCancel;
        TPushButton* m_pwndOK;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );
};

tCIDLib::TBoolean TTestDlgBox::bCreated()
{
    const tCIDLib::TBoolean bRet = TDlgBox::bCreated();


    // Just set up event handlers
    m_pwndCancel = pwndChildAs<TPushButton>(kCIDCtrls::widCancel);
    m_pwndOK = pwndChildAs<TPushButton>(kCIDCtrls::widOK);

    m_pwndCancel->pnothRegisterHandler(this, &TTestDlgBox::eClickHandler);
    m_pwndOK->pnothRegisterHandler(this, &TTestDlgBox::eClickHandler);

    TNumSpinBox* pNumSpin = pwndChildAs<TNumSpinBox>(kTestRes::ridDlg_Test_NumSpin);
    pNumSpin->SetNumRange(-50, 60, 5);

    TEnumSpinBox* pEnumSpin = pwndChildAs<TEnumSpinBox>(kTestRes::ridDlg_Test_EnumSpin);
    tCIDLib::TStrList colVals;
    colVals.objAdd(L"Value 1");
    colVals.objAdd(L"Value 2");
    colVals.objAdd(L"Value 3");
    colVals.objAdd(L"Value 4");
    colVals.objAdd(L"Value 5");
    colVals.objAdd(L"Value 6");
    pEnumSpin->SetEnumList(colVals, 2);


    TRadioButton* pwndFirst = pwndChildAs<TRadioButton>(kTestRes::ridDlg_Test_First);
    TRadioButton* pwndSec = pwndChildAs<TRadioButton>(kTestRes::ridDlg_Test_Second);

    TWindow* pwndFirstOfGrp = pwndFirstOfGroup(*pwndFirst);
    pwndFirstOfGrp = pwndFirstOfGroup(*pwndSec);

    pwndFirstOfGrp = pwndFirstOfNextGroup(*pwndFirst);

    TString strDPI(L"DPI: ");
    TGraphDesktopDev gdevTmp;
    strDPI.AppendFormatted(gdevTmp.szPixelsPerInch().c4Height());
    strChildText(kTestRes::ridDlg_Test_DPI, strDPI);

    return bRet;
}

tCIDCtrls::EEvResponses TTestDlgBox::eClickHandler(TButtClickInfo& wnotEvent)
{
    EndDlg(wnotEvent.widSource());
    return tCIDCtrls::EEvResponses::Handled;
}


class TTestTab : public TTabWindow
{
    public :
        // --------------------------------------------------------------------
        //  Constructors and Destructor
        // --------------------------------------------------------------------
        TTestTab() = delete;

        TTestTab
        (
            const   TString&                strTabId
            , const TString&                strTabType
            , const tCIDLib::TBoolean       bCanClose
        );

        ~TTestTab();


    protected :
        // --------------------------------------------------------------------
        //  Protected, inherited methods
        // --------------------------------------------------------------------
        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;

        tCIDLib::TBoolean bCreated() override;
};

TTestTab::TTestTab( const   TString&            strTabId
                    , const TString&            strTabType
                    , const tCIDLib::TBoolean   bCanClose) :

    TTabWindow(strTabId, strTabType, bCanClose, kCIDLib::False)
{
}

TTestTab::~TTestTab()
{
}

tCIDLib::TBoolean TTestTab::bPaint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
//    gdevToUse.Fill(areaUpdate, facCIDGraphDev().rgbWhite);
    return kCIDLib::False;
}

tCIDLib::TBoolean TTestTab::bCreated()
{

    TArea areaButt(0, 0, 96, 24);
    areaButt.CenterIn(areaWndSize());

    TPushButton* pwndButt = new TPushButton;
    pwndButt->CreateButton
    (
        *this
        , kCIDCtrls::widFirstCtrl
        , areaButt
        , strWndText()
        , tCIDCtrls::EWndStyles::VisTabChild
    );
    return kCIDLib::True;
}



// ----------------------------------------------------------------------------
//   CLASS: TTestFrameWnd
//  PREFIX: wnd
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TTextFrameWnd: PConstructors and Destructor
// ----------------------------------------------------------------------------
TTestFrameWnd::TTestFrameWnd() :

    m_pwndActBar(nullptr)
    , m_pwndCalendar(nullptr)
    , m_pwndCancel(nullptr)
    , m_pwndCheckBox(nullptr)
    , m_pwndComboBox(nullptr)
    , m_pwndEntryFld(nullptr)
    , m_pwndImgButton(nullptr)
    , m_pwndListBox(nullptr)
    , m_pwndMultiEdit(nullptr)
    , m_pwndMCListBox(nullptr)
    , m_pwndObjView(nullptr)
    , m_pwndOK(nullptr)
    , m_pwndProgBar(nullptr)
    , m_pwndSlider(nullptr)
    , m_pwndStaticImg(nullptr)
    , m_pwndStatusBar(nullptr)
    , m_pwndTree(nullptr)
    , m_pwndMultiStaticText(nullptr)
    , m_pwndStaticText(nullptr)
    , m_pwndTest(nullptr)
    , m_pwndTabs(nullptr)
{
}

TTestFrameWnd::~TTestFrameWnd()
{
}


tCIDLib::TVoid TTestFrameWnd::DragExit()
{
    // Nothing really to do in this case
}


tCIDLib::TBoolean
TTestFrameWnd::bCanAcceptFiles( const   tCIDLib::TStrList&      colFiles
                                , const tCIDLib::TStrHashSet&   colExts) const
{
    // If there's a JPEG or PNG file in the list, we can accept it
    return  colExts.bHasElement(L"PNG")
            || colExts.bHasElement(L"JPG")
            || colExts.bHasElement(L"JPEG");
}


tCIDLib::TBoolean
TTestFrameWnd::bDragDropped(const   TPoint&                 pntAt
                            , const tCIDLib::TStrList&      colFiles
                            , const tCIDLib::TStrHashSet&   colExts)
{
    // Display the name of the first file
    m_pwndEntryFld->strWndText(colFiles[0]);

    //
    //  Indicate we ultimately accepted it. Normally we'd just post ourself a command
    //  to do this async and return this true immediately, so as not to lock up the
    //  shell, but for this simple test it's fine.
    //
    TOkBox msgbOK(strWndText(), L"Accepted the files");
    msgbOK.ShowIt(*this);

    return kCIDLib::True;
}


//
//  We aren't position sensitive, so once we have indicated we can accept the files
//  above, we can just return true until the drop.
//
tCIDLib::TBoolean
TTestFrameWnd::bDragEnter(  const   TPoint&
                            , const tCIDLib::TStrList&
                            , const tCIDLib::TStrHashSet&)
{
    return kCIDLib::True;
}


tCIDLib::TBoolean
TTestFrameWnd::bDragOver(const  TPoint&
                        , const tCIDLib::TStrList&
                        , const tCIDLib::TStrHashSet&)
{
    return kCIDLib::True;
}



// ----------------------------------------------------------------------------
//  TTestFrameWnd: Protected, inherited method
// ----------------------------------------------------------------------------

tCIDLib::TVoid
TTestFrameWnd::AreaChanged( const   TArea&                  areaPrev
                            , const TArea&                  areaNew
                            , const tCIDCtrls::EPosStates   ePosState
                            , const tCIDLib::TBoolean       bOrgChanged
                            , const tCIDLib::TBoolean       bSizeChanged
                            , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    if ((ePosState != tCIDCtrls::EPosStates::Minimized) && bSizeChanged && m_pwndStatusBar)
        m_pwndStatusBar->ParentSizeChanged();
}


// Called any time we are to exit, to let the user decide
tCIDLib::TBoolean TTestFrameWnd::bAllowShutdown()
{
    TYesNoBox msgbAsk(L"Test CIDCtrls", L"Do you want to exit the program?");
    if (msgbAsk.bShowIt(*this))
    {
        TermDragAndDrop(*this);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


//
//  React to mouse clicks
//
tCIDLib::TBoolean
TTestFrameWnd::bClick(  const   tCIDCtrls::EMouseButts  eButton
                        , const tCIDCtrls::EMouseClicks eClickType
                        , const TPoint&                 pntAt
                        , const tCIDLib::TBoolean       bCtrlShift
                        , const tCIDLib::TBoolean       bShift)
{
    TString strFmt = L"Mouse ";
    strFmt.Append(tCIDCtrls::strXlatEMouseButts(eButton));
    strFmt.Append(L" ");
    strFmt.Append(tCIDCtrls::strXlatEMouseClicks(eClickType));
    strFmt.Append(L" click.");
    m_pwndEntryFld->strWndText(strFmt);

    if ((eButton == tCIDCtrls::EMouseButts::Right)
    &&  (eClickType == tCIDCtrls::EMouseClicks::Down))
    {
        // Convert the point to screen coordinates
        TPoint pntScr;
        ToScreenCoordinates(pntAt, pntScr);

        TPopupMenu menuTest(L"Test Popup");
        menuTest.Create(facTestCIDCtrls, kTestRes::ridMenu_Main);

        menuTest.SetItemCheck(kTestRes::ridMenu_Rate_Medium, kCIDLib::True);

        tCIDLib::TCard4 c4Res = menuTest.c4Process(*this, pntScr, tCIDLib::EVJustify::Bottom);

        if (c4Res == kCIDLib::c4MaxCard)
        {
            m_pwndEntryFld->strWndText(L"The popup menu failed");
        }
         else if (c4Res)
        {
            strFmt = L"Menu Result is ";
            strFmt.AppendFormatted(c4Res);
            m_pwndEntryFld->strWndText(strFmt);
        }
         else
        {
            m_pwndEntryFld->strWndText(L"Menu was cancelled");
        }
    }

    // Say we processed them all
    return kCIDLib::True;
}


//
//  When we are created, in turn create all of our child windows, which we do manually
//  in this simple test.
//
tCIDLib::TBoolean TTestFrameWnd::bCreated()
{
    // Call our parent first
    TParent::bCreated();

    tCIDCtrls::TWndId widCur = kCIDCtrls::widFirstCtrl;
    tCIDLib::TCard4 c4At;
    TString strTmp;

    // Load our menu bar and assign it to us
    SetMenuBar(facTestCIDCtrls, kTestRes::ridMenu_Main);
    menuCur().SetItemCheck(kTestRes::ridMenu_Rate_Medium, kCIDLib::True);

    // Create a static text
    m_pwndStaticText = new TStaticText();
    m_pwndStaticText->Create
    (
        *this
        , widCur++
        , TArea(8, 8, 128, 22)
        , L"&Prefix:"
        , tCIDCtrls::EWndStyles::VisChild
        , tCIDCtrls::ESTextStyles::None
        , tCIDLib::EHJustify::Left
    );

    // Create an entry field
    m_widEntryFld = widCur++;
    m_pwndEntryFld = new TEntryField();
    m_pwndEntryFld->Create
    (
        *this
        , m_widEntryFld
        , TArea(12, 30, 192, 22)
        , L"Initial Text"
        , tCIDCtrls::EWndStyles::VisTabGroupChild | tCIDCtrls::EWndStyles::Border
        , tCIDCtrls::EEFStyles::None
    );
    m_pwndEntryFld->SetCueText(L"Enter text");
    m_pwndEntryFld->pnothRegisterHandler(this, &TTestFrameWnd::eEFHandler);


#if 0
TWindow* pwndTopMost = pwndTopChild();
TWindow* pwndBottomMost = pwndBottomChild();

TWindow* pwndAbove = pwndNextChild(*pwndTopMost);
pwndBelow = pwndAboveChild(*pwndBottomMost);

TWindow* pwndAbove = pwndPrevChild(*pwndTopMost);
pwndAbove = pwndPrevChild(*pwndBottomMost);
#endif


    m_widCheckBox = widCur++;
    m_pwndCheckBox = new TCheckBox;
    m_pwndCheckBox->Create
    (
        *this
        , m_widCheckBox
        , TArea(260, 28, 160, 22)
        , L"Check box me, baby"
        , tCIDCtrls::EWndStyles::VisTabGroupChild
        , tCIDCtrls::ECBStyles::None
    );

    m_pwndComboBox = new TComboBox;
    m_pwndComboBox->Create
    (
        *this
        , widCur++
        , TArea(440, 28, 192, 24)
        , tCIDCtrls::EWndStyles::VisTabGroupChild
        , tCIDCtrls::EComboStyles::None
    );
    m_pwndComboBox->SetCueText(L"Make a Selection");
    m_pwndComboBox->pnothRegisterHandler(this, &TTestFrameWnd::eListHandler);

    // Create a list box and combo box
    m_pwndListBox = new TListBox;
    m_pwndListBox->CreateListBox
    (
        *this
        , widCur++
        , TArea(12, 60, 192, 222)
        , tCIDCtrls::EWndStyles::VisTabGroupChild | tCIDCtrls::EWndStyles::Border
        , tCIDCtrls::ELBStyles::MultiSel
    );
    m_pwndListBox->pnothRegisterHandler(this, &TTestFrameWnd::eListHandler);

    // Create a multiline editor
    const TString strMLEText
    (
        L"This is the text to test. We'll load this test up to see if it shows "
        L"up correctly upon creation.\n\n"
        L"This is a second line.\n"
    );

    m_pwndMultiEdit = new TMultiEdit;
    m_pwndMultiEdit->Create
    (
        *this
        , widCur++
        , TArea(220, 60, 192, 222)
        , tCIDCtrls::EWndStyles::VisTabGroupChild
        , tCIDCtrls::EMLEStyles::None
    );
    m_pwndMultiEdit->strWndText(strMLEText);

    //
    //  Read the text back and make sure it's the same. IT may have changed the
    //  line ends so adjust those.
    //
    strTmp = m_pwndMultiEdit->strWndText();
    c4At = 0;
    strTmp.bReplaceSubStr(L"\r\n", L"\n", c4At, kCIDLib::True);
    if (strMLEText != strTmp)
        m_pwndEntryFld->strWndText(L"Initial text did not match queried text");

    m_pwndMultiStaticText = new TStaticMultiText();
    m_pwndMultiStaticText->Create
    (
        *this
        , widCur++
        , TArea(420, 60, 192, 222)
        , strMLEText
        , tCIDCtrls::EWndStyles::VisChild | tCIDCtrls::EWndStyles::Border
        , tCIDCtrls::EMLSStyles::None
        , tCIDLib::EHJustify::Center
    );

    m_pwndStaticImg = new TStaticImg();
    m_pwndStaticImg->CreateStaticImg
    (
        *this
        , widCur++
        , TArea(640, 60, 64, 64)
        , tCIDGraphDev::EPlacement::Center
        , tCIDCtrls::EWndStyles::VisChild
        , tCIDCtrls::ESImgStyles::None
    );

    TPathStr pathImg;
    if (TProcEnvironment::bFind(L"CIDLIB_SRCDIR", pathImg))
    {
        pathImg.AddLevel(L"\\Source\\AllProjects\\WndUtils\\CIDCtrls\\TestCIDCtrls\\Win3X_24Bit.bmp");
        m_pwndStaticImg->SetFromBmpFile(pathImg);
    }


    tCIDLib::TStrList colCols(2);
    colCols.objAdd(L"The Value");
    colCols.objAdd(L"More Info");

    m_widMCListBox = widCur++;
    m_pwndMCListBox = new TMultiColListBox();
    m_pwndMCListBox->CreateMCLB
    (
        *this
        , m_widMCListBox
        , TArea(12, 290, 420, 160)
        , colCols
        , 0
        , tCIDCtrls::EWndStyles::VisTabGroupChild | tCIDCtrls::EWndStyles::Border
        , tCIDLib::eOREnumBits
          (
            tCIDCtrls::EMCLBStyles::AutoLastCol
            , tCIDCtrls::EMCLBStyles::FullRowSel
            , tCIDCtrls::EMCLBStyles::MultiSel
            , tCIDCtrls::EMCLBStyles::AutoSort
          )
    );
    m_pwndMCListBox->pnothRegisterHandler(this, &TTestFrameWnd::eListHandler);

#if 0
    m_widTabs = widCur++;
    m_pwndTabs = new TTabbedWnd();
    m_pwndTabs->CreateTabbedWnd
    (
        *this
        , m_widTabs
        , TArea(12, 290, 420, 160)
        , tCIDCtrls::EWndStyles::VisGroupChild
          | tCIDCtrls::EWndStyles::Border
          | tCIDCtrls::EWndStyles::ClipChildren
        , tCIDCtrls::ETabWStyles::TwoLineHdr
        , tCIDCtrls::EExWndStyles::ControlParent
    );

    TTestTab* pwndTab = new TTestTab
    (
        L"/Customize/Templates/User/Patio Lights", L"Template", kCIDLib::False
    );
    m_pwndTabs->c4CreateTab(pwndTab, L"Patio Lights");

    pwndTab = new TTestTab
    (
        L"/Customize/Events/Triggered/User/Sunrise Lights", L"Triggered Event", kCIDLib::True
    );
    m_pwndTabs->c4CreateTab(pwndTab, L"Sunrise Lights");

    pwndTab = new TTestTab
    (
        L"/Customize/Events/Triggered/User/Sunset Lights", L"Triggered Event", kCIDLib::True
    );
    m_pwndTabs->c4CreateTab(pwndTab, L"Sunset Lights");

    pwndTab = new TTestTab
    (
        L"/Customize/Events/Triggered/User/System Armed", L"Triggered Event", kCIDLib::True
    );
    m_pwndTabs->c4CreateTab(pwndTab, L"System Armed");

    pwndTab = new TTestTab
    (
        L"/Customize/Events/Scheduled/User/Stop Motor", L"Scheduled Event", kCIDLib::True
    );
    m_pwndTabs->c4CreateTab(pwndTab, L"Stop Motor");
#endif


#if 0
    m_pwndCalendar = new TCalendar();
    m_pwndCalendar->Create
    (
        *this
        , widCur++
        , TArea(12, 290, 420, 200)
        , tCIDCtrls::EWndStyles::VisTabGroupChild
        , tCIDCtrls::ECalStyles::SizeTo
    );
#endif


    // Create a tree view and add some content
    m_widTreeView = widCur++;
    m_pwndTree = new TTreeView();
    m_pwndTree->Create
    (
        *this
        , m_widTreeView
        , TArea(460, 290, 240, 310)
        , tCIDCtrls::EWndStyles::VisTabGroupChild
        , tCIDCtrls::EExWndStyles::None
        , tCIDLib::eOREnumBits
          (
            tCIDCtrls::ETreeStyles::HasLines
            , tCIDCtrls::ETreeStyles::ReadOnly
            , tCIDCtrls::ETreeStyles::CheckBoxes
          )
    );
    m_pwndTree->pnothRegisterHandler(this, &TTestFrameWnd::eTreeHandler);

    m_pwndTree->AddItem(L"/", L"Child 1");
    m_pwndTree->AddItem(L"/", L"Child 2");

    m_pwndTree->AddScope(L"/", L"Scope 1", kCIDLib::False);
    m_pwndTree->AddItem(L"/Scope 1", L"Ziggy");
    m_pwndTree->AddItem(L"/Scope 1", L"Cathy");
    m_pwndTree->AddItem(L"/Scope 1", L"Bob2");

    m_pwndTree->AddScope(L"/", L"Scope 2", kCIDLib::True);

#if 0

    // Set up an image list for our object view
    TBitmap bmpComputer = facCIDCtrls().bmpLoadLargeAppImg(L"Computer System");
    TBitmap bmpController = facCIDCtrls().bmpLoadLargeAppImg(L"Controller");
    TImageList imglNormal;
    imglNormal->Create(bmpComputer.szBitmap(), 2, 2);
    imglNormal.c4AddImage(bmpComputer);
    imglNormal.c4AddImage(bmpController);

    m_pwndObjView = new TObjectView();
    m_pwndObjView->Create
    (
        *this
        , widCur++
        , TArea(460, 290, 240, 300)
        , tCIDCtrls::EWndStyles::VisTabGroupChild | tCIDCtrls::EWndStyles::Border
        , imglNormal
        , tCIDCtrls::EObjVStyles::None
    );

    m_pwndObjView->AddItem(0, L"Text Item", 1);
    m_pwndObjView->SetActionMenu(facTestCIDCtrls, kTestRes::ridMenu_Main);
#endif


#if 0
    m_pwndSlider = new TSlider();
    m_pwndSlider->Create
    (
        *this
        , widCur++
        , TArea(12, 440, 430, 32)
        , 0
        , 100
        , tCIDCtrls::EWndStyles::VisTabGroupChild
        , tCIDLib::eOREnumBits
          (
            tCIDCtrls::ESldrStyles::Ticks, tCIDCtrls::ESldrStyles::TrackEvs
          )
    );
    m_pwndSlider->SetValue(25);
    m_pwndSlider->pnothRegisterHandler(this, &TTestFrameWnd::eSliderHandler);
#endif

    // Create an OK window
    m_widOK = kCIDCtrls::widOK;
    m_pwndOK = new TPushButton();
    m_pwndOK->CreateButton
    (
        *this
        , m_widOK
        , TArea(12, 500, 64, 24)
        , L"&OK"
        , tCIDCtrls::EWndStyles::VisTabGroupChild
        , tCIDCtrls::EPButtStyles::Default
    );

    // A button for testing various things
    m_widTest = widCur++;
    m_pwndTest = new TPushButton();
    m_pwndTest->CreateButton
    (
        *this
        , m_widTest
        , TArea(80, 500, 64, 24)
        , L"&Test"
        , tCIDCtrls::EWndStyles::VisChild
    );

    // Create a cancel button
    m_widCancel = kCIDCtrls::widCancel;
    m_pwndCancel = new TPushButton();
    m_pwndCancel->CreateButton
    (
        *this
        , m_widCancel
        , TArea(152, 500, 64, 24)
        , L"&Cancel"
        , tCIDCtrls::EWndStyles::VisChild
    );


    // Create an activity bar and start it animating
    m_pwndActBar = new TActivityBar();
    m_pwndActBar->Create
    (
        *this
        , widCur++
        , TArea(12, 540, 250, 18)
        , tCIDCtrls::EWndStyles::VisChild
        , tCIDCtrls::EPBarStyles::None
    );
    m_pwndActBar->StartStop(kCIDLib::True);


    // Create a progress bar and set it to some sample percentage
    m_pwndProgBar = new TProgressBar();
    m_pwndProgBar->Create
    (
        *this
        , widCur++
        , TArea(12, 564, 250, 18)
        , tCIDCtrls::EWndStyles::VisChild
        , tCIDCtrls::EPBarStyles::None
    );
    m_pwndProgBar->SetValue(50);

    TBitmap bmpTest = facTestCIDCtrls.bmpLoadResourceImg(kTestRes::ridPNG_Test, kCIDLib::False);
    m_pwndImgButton = new TImgPushButton();
    m_pwndImgButton->CreateButton
    (
        *this
        , widCur++
        , TArea(288, 540, 48, 48)
        , L"Test Text"
        , bmpTest
        , tCIDCtrls::EWndStyles::VisChild
          | tCIDCtrls::EWndStyles::Border
          | tCIDCtrls::EWndStyles::TabStop
    );

    // Put focus on the entry field
    m_pwndEntryFld->TakeFocus();


    // Load some values into the list controls
    tCIDLib::TStrList colToLoad(8);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 8; c4Index++)
    {
        strTmp = L"This is value #";
        strTmp.AppendFormatted(c4Index);
        colToLoad.objAdd(strTmp);
    }
    m_pwndComboBox->LoadList(colToLoad);
    m_pwndListBox->LoadList(colToLoad);

    m_pwndComboBox->SelectByIndex(2);
    m_pwndListBox->SelectByIndex(1);
    //m_pwndListBox->SelectByText(L"This is value #5");


    // Register handlers
    m_pwndOK->pnothRegisterHandler(this, &TTestFrameWnd::eClickHandler);
    m_pwndCancel->pnothRegisterHandler(this, &TTestFrameWnd::eClickHandler);
    m_pwndCheckBox->pnothRegisterHandler(this, &TTestFrameWnd::eClickHandler);
    m_pwndTest->pnothRegisterHandler(this, &TTestFrameWnd::eClickHandler);


    // Set up a status bar and set the areas and put some text into them
    m_pwndStatusBar = new TStatusBar();
    m_pwndStatusBar->Create(*this, widCur++);
    tCIDLib::TCardList fcolPercents(4);
    fcolPercents.c4AddElement(10);
    fcolPercents.c4AddElement(10);
    fcolPercents.c4AddElement(10);
    fcolPercents.c4AddElement(kCIDLib::c4MaxCard);
    m_pwndStatusBar->SetAreas(fcolPercents, kCIDLib::True);

    m_pwndStatusBar->SetAreaText(0, L"80x120");
    m_pwndStatusBar->SetAreaText(1, L"Active");
    m_pwndStatusBar->SetAreaText(2, L"156");
    m_pwndStatusBar->SetAreaText(3, L"The client area is not appropriate");


    // Initialize drag and drop support on this window
    InitDragAndDrop(*this);

    return kCIDLib::True;
}


tCIDLib::TVoid TTestFrameWnd::Destroyed()
{
    // If not already terminated, then do it
    TermDragAndDrop(*this);

    TParent::Destroyed();
}



tCIDCtrls::ECtxMenuOpts
TTestFrameWnd::eShowContextMenu(const TPoint& pntAt, const tCIDCtrls::TWndId widSrc)
{
    //
    //  If the source id isn't zero, then it came from something else, which we don't
    //  expect, since none of our children are set up to pass this on.
    //
    if (widSrc != 0)
    {
        TErrBox msgbErr(L"Context Menu", L"Expected only ourself as popup menu source");
        msgbErr.ShowIt(*this);
    }
     else
    {
        // Convert the point to screen coordinates
        TPoint pntScr;
        ToScreenCoordinates(pntAt, pntScr);

        TPopupMenu menuTest(L"Test Popup");
        menuTest.Create(facTestCIDCtrls, kTestRes::ridMenu_Main);
        menuTest.c4Process(*this, pntScr, tCIDLib::EVJustify::Bottom);
    }
    return tCIDCtrls::ECtxMenuOpts::Done;
}


tCIDLib::TVoid
TTestFrameWnd::MenuCommand( const   tCIDLib::TResId     ridItem
                            , const tCIDLib::TBoolean   bChecked
                            , const tCIDLib::TBoolean   bEnabled)
{
    if ((ridItem == kTestRes::ridMenu_Rate_Low)
    ||  (ridItem == kTestRes::ridMenu_Rate_Medium)
    ||  (ridItem == kTestRes::ridMenu_Rate_High))
    {
        menuCur().SetItemCheck(ridItem, kCIDLib::True, kCIDLib::True);
    }
     else if (ridItem == kTestRes::ridMenu_File_SelDir)
    {
        TError errToShow
        (
            L"TestFac"
            , L"TestFile.cpp"
            , 10
            , L"The baffle plate is warped"
        );

        TExceptDlg dlgShow
        (
            *this, L"This is a test", L"Something went really wrong", errToShow
        );
    }
     else if (ridItem == kTestRes::ridMenu_File_OpenFile)
    {
        tCIDLib::TKVPList colTypes;
        colTypes.objAdd(TKeyValuePair(L"CQC Driver Manifests", L"*.Manifest"));

        tCIDLib::TStrList colSel;
        const tCIDLib::TBoolean bRes = facCIDCtrls().bOpenFileDlg
        (
            *this
            , L"Open a Manifest File"
            , L"C:\\Tmp"
            , colSel
            , colTypes
            , tCIDLib::eOREnumBits
              (
                tCIDCtrls::EFOpenOpts::FileMustExist
                , tCIDCtrls::EFOpenOpts::StrictTypes
                , tCIDCtrls::EFOpenOpts::ConfirmPrompt
              )
        );

        if (bRes)
            m_pwndEntryFld->strWndText(colSel[0]);
    }
     else if (ridItem == kTestRes::ridMenu_File_Exit)
    {
        if (bAllowShutdown())
            facCIDCtrls().ExitLoop(0);
    }
}


// ----------------------------------------------------------------------------
//  TTextFrameWnd: Private, non-virtaul method
// ----------------------------------------------------------------------------

tCIDCtrls::EEvResponses
TTestFrameWnd::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == m_widCancel)
    {
        if (bAllowShutdown())
            facCIDCtrls().ExitLoop(0);
    }
     else if (wnotEvent.widSource() == m_widCheckBox)
    {
        if (m_pwndCheckBox->bCheckedState())
            m_pwndEntryFld->strWndText(L"Check box checked");
        else
            m_pwndEntryFld->strWndText(L"Check box unchecked");
    }
     else if (wnotEvent.widSource() == m_widOK)
    {
        tCIDLib::TStrList colChecked;
        tCIDLib::TCardList fcolIds;
        m_pwndTree->c4QueryAllChecked(colChecked, fcolIds);

        if (m_pwndListBox)
            m_pwndListBox->RemoveAll();

        if (m_pwndMCListBox)
        {
            const tCIDLib::TCard4 c4CurCount = m_pwndMCListBox->c4ItemCount();
            tCIDLib::TStrList colVals(2);
            TString strCurVal(L"Value ");
            strCurVal.AppendFormatted(c4CurCount);
            colVals.objAdd(strCurVal);
            colVals.objAdd(L"Milli");
            m_pwndMCListBox->c4AddItem(colVals, c4CurCount);

            tCIDLib::TCard4 c4At = m_pwndMCListBox->c4FindByText(strCurVal);
            if (c4At == 1)
            {
            }

            c4At = m_pwndMCListBox->c4FindByUserData(c4CurCount, 0);
            if (c4At == 2)
            {
            };
        }
    }
     else if (wnotEvent.widSource() == m_widTest)
    {
        TTestDlgBox dlgTest;
        tCIDLib::TCard4 c4Res = dlgTest.c4RunDlg
        (
            *this, facTestCIDCtrls, kTestRes::ridDlg_Test
        );

        TString strText(L"Dialog Result is ");
        strText.AppendFormatted(c4Res);
        m_pwndEntryFld->strWndText(strText);
    }
    return tCIDCtrls::EEvResponses::Handled;
}



tCIDCtrls::EEvResponses
TTestFrameWnd::eEFHandler(TEFChangeInfo& wnotEvent)
{
    if ((wnotEvent.widSource() == m_widEntryFld)
    &&  (wnotEvent.eEvent() == tCIDCtrls::EEFEvents::Changed))
    {
        // TAudio::Beep(880, 100);
    }
    return tCIDCtrls::EEvResponses::Handled;
}


tCIDCtrls::EEvResponses
TTestFrameWnd::eListHandler(TListChangeInfo& wnotEvent)
{
    TString strText;
    if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::Cleared)
    {
        m_pwndEntryFld->strWndText(L"List was cleared");
    }
     else if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::DeleteReq)
    {
        strText = L"List delete request at: ";
        strText.AppendFormatted(wnotEvent.c4Index());
        m_pwndEntryFld->strWndText(strText);
    }
     else if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::Invoked)
    {
        strText = L"List double clicked at: ";
        strText.AppendFormatted(wnotEvent.c4Index());
        strText.Append(L", Column=");
        strText.AppendFormatted(wnotEvent.c4ColIndex());
        m_pwndEntryFld->strWndText(strText);
    }
     else if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::SelChanged)
    {
        strText = L"New list index is: ";
        strText.AppendFormatted(wnotEvent.c4Index());
        m_pwndEntryFld->strWndText(strText);
    }
     else if (wnotEvent.eEvent() == tCIDCtrls::EListEvents::Marked)
    {
        strText = L"List marked change at: ";
        strText.AppendFormatted(wnotEvent.c4Index());
        m_pwndEntryFld->strWndText(strText);
    }

    if (!strText.bIsEmpty())
    {
        facTestCIDCtrls.LogMsg
        (
            CID_FILE
            , CID_LINE
            , strText
            , tCIDLib::ESeverities::Status
        );
    }
    return tCIDCtrls::EEvResponses::Handled;
}


tCIDCtrls::EEvResponses
TTestFrameWnd::eSliderHandler(TSliderChangeInfo& wnotEvent)
{
    if (wnotEvent.eEvent() == tCIDCtrls::ESldrEvents::Change)
    {
        TString strFmt = L"Slider change. Value=";
        strFmt.AppendFormatted(wnotEvent.i4Value());
        m_pwndEntryFld->strWndText(strFmt);
    }
     else if (wnotEvent.eEvent() == tCIDCtrls::ESldrEvents::Track)
    {
        TString strFmt = L"Slider track. Value=";
        strFmt.AppendFormatted(wnotEvent.i4Value());
        m_pwndEntryFld->strWndText(strFmt);
    }
     else if (wnotEvent.eEvent() == tCIDCtrls::ESldrEvents::EndTrack)
    {
        TString strFmt = L"Slider end track. Value=";
        strFmt.AppendFormatted(wnotEvent.i4Value());
        m_pwndEntryFld->strWndText(strFmt);
    }
    return tCIDCtrls::EEvResponses::Handled;
}


tCIDCtrls::EEvResponses
TTestFrameWnd::eTreeHandler(TTreeEventInfo& wnotEvent)
{
    if (wnotEvent.eEvent() == tCIDCtrls::ETreeEvents::Invoked)
    {
        TString strText;
        m_pwndTree->QueryItemText(wnotEvent.strPath(), strText);

        strText.Insert(L"Tree double. Name= ", 0);
        m_pwndEntryFld->strWndText(strText);
    }
     else if (wnotEvent.eEvent() == tCIDCtrls::ETreeEvents::CheckState)
    {
        TString strText;
        m_pwndTree->QueryItemText(wnotEvent.strPath(), strText);

        if (wnotEvent.bState())
            strText.Insert(L"Tree item checked, Name:= ", 0);
        else
            strText.Insert(L"Tree item unchecked, Name:= ", 0);

        m_pwndEntryFld->strWndText(strText);

        if (m_pwndTree->bIsScope(wnotEvent.strPath()))
            m_pwndTree->SetAllChildChecks(wnotEvent.strPath(), wnotEvent.bState());
    }
    return tCIDCtrls::EEvResponses::Handled;
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
        // Create a frame window
        TTestFrameWnd wndMain;
        wndMain.CreateFrame
        (
            nullptr
            , TArea(256, 256, 750, 680)
            , L"Test Ctrls Frame"
            , tCIDCtrls::EWndStyles::StdFrame
            , tCIDCtrls::EExWndStyles::None
            , tCIDCtrls::EFrameStyles::StdFrame
            , kCIDLib::False
        );

        facCIDCtrls().uMainMsgLoop(wndMain);
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
    }

    return tCIDLib::EExitCodes::Normal;
}

