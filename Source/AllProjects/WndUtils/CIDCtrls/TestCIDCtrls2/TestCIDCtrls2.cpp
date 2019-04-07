//
// CID_FILE NAME: TestCIDCtrls2.cpp
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
//  This is the main module of the program. This program tests the pane window
//  container/layout manager stuff.
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
#include    "TestCIDCtrls2.hpp"



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
TGUIFacility facTestCIDCtrls2
(
    L"TestCIDCtrls2"
    , tCIDLib::EModTypes::Exe
    , kCIDLib::c4MajVersion
    , kCIDLib::c4MinVersion
    , kCIDLib::c4Revision
    , tCIDLib::EModFlags::HasMsgFile
);



// ----------------------------------------------------------------------------
//   CLASS: TTestFrameWnd
//  PREFIX: wnd
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TTextFrameWnd: PConstructors and Destructor
// ----------------------------------------------------------------------------
TTestFrameWnd::TTestFrameWnd() :

    m_pwndOutput(nullptr)
    , m_pwndTest(nullptr)
{
}

TTestFrameWnd::~TTestFrameWnd()
{
}


// ----------------------------------------------------------------------------
//  TTestFrameWnd: Public, non-virtual method
// ----------------------------------------------------------------------------

tCIDLib::TVoid TTestFrameWnd::CreateMain()
{
    CreateFrame
    (
        nullptr
        , facCIDCtrls().areaDefWnd()
        , L"TestCtrls2 Frame"
        , tCIDCtrls::EWndStyles::StdFrameNV
        , tCIDCtrls::EExWndStyles::None
        , tCIDCtrls::EFrameStyles::None
        , kCIDLib::False
    );
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

}


//
//  When we are created, in turn create all of our child windows, which we do manually
//  in this simple test.
//
tCIDLib::TBoolean TTestFrameWnd::bCreated()
{
    // Call our parent first
    TParent::bCreated();

    m_pwndTest = new TMColListBox();
    m_pwndTest->CreateMColLB
    (
        *this
        , kCIDCtrls::widFirstCtrl
        , TArea(10, 10, 480, 320)
        , tCIDLib::eOREnumBits
          (
            tCIDCtrls::EWndStyles::VisTabChild, tCIDCtrls::EWndStyles::Border
          )
    );

    TMColListBox::TColDefs colCols(3);
    colCols.objAdd
    (
        TMCLBColDef(L"First Slot", tCIDLib::EHJustify::Right, 128, kCIDLib::True)
    );
    colCols.objAdd
    (
        TMCLBColDef(L"Second Slot", tCIDLib::EHJustify::Left, 132)
    );
    colCols.objAdd
    (
        TMCLBColDef(L"The Third Longer Slot", tCIDLib::EHJustify::Left, 256)
    );
    m_pwndTest->SetColumns(colCols);


//    m_pwndHdr->pnothRegisterHandler(this, &TTestFrameWnd::eHdrHandler);


    m_pwndOutput = new TStaticText();
    m_pwndOutput->Create
    (
        *this
        , kCIDCtrls::widFirstCtrl + 1
        , TArea(10, 512, 320, 16)
        , TString::strEmpty()
        , tCIDLib::eOREnumBits
          (
            tCIDCtrls::EWndStyles::VisChild, tCIDCtrls::EWndStyles::Border
          )
        , tCIDCtrls::ESTextStyles::None
    );

    return kCIDLib::True;
}


tCIDLib::TVoid TTestFrameWnd::Destroyed()
{
    TParent::Destroyed();
}



// ----------------------------------------------------------------------------
//  TTestFrameWnd: Private, non-virtual method
// ----------------------------------------------------------------------------

tCIDCtrls::EEvResponses TTestFrameWnd::eHdrHandler(TColHdrChangeInfo& wnotEvent)
{
    /*
    if (wnotEvent.eEvent() == tCIDCtrls::ECHdrEvents::ColClick)
    {
        TTextStringOutStream strmTar(256UL);
        strmTar << L"Got click on slot: " << wnotEvent.c4ColIndex() << kCIDLib::FlushIt;
        m_pwndOutput->strWndText(strmTar.strData());
    }
     else if ((wnotEvent.eEvent() == tCIDCtrls::ECHdrEvents::DragStart)
          ||  (wnotEvent.eEvent() == tCIDCtrls::ECHdrEvents::DragEnd)
          ||  (wnotEvent.eEvent() == tCIDCtrls::ECHdrEvents::DragDrag))
    {
        TTextStringOutStream strmTar(256UL);
        if (wnotEvent.eEvent() == tCIDCtrls::ECHdrEvents::DragStart)
        {
            strmTar << L"Start drag on slot: " << wnotEvent.c4ColIndex();
        }
         else if (wnotEvent.eEvent() == tCIDCtrls::ECHdrEvents::DragEnd)
        {
            strmTar << L"End drag on slot: " << wnotEvent.c4ColIndex();
        }
         else if (wnotEvent.eEvent() == tCIDCtrls::ECHdrEvents::DragDrag)
        {
            strmTar << L"Dragging slot: " << wnotEvent.c4ColIndex()
                    << L", old=" << wnotEvent.c4OldWidth()
                    << L", new=" << wnotEvent.c4NewWidth();
        }
         else
        {
            strmTar << L"Unknown event";
        }

        strmTar.Flush();
        m_pwndOutput->strWndText(strmTar.strData());
    }
     else
    {

    }
    */

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
        wndMain.CreateMain();
        facCIDCtrls().ShowGUI(wndMain);
        facCIDCtrls().uMainMsgLoop(wndMain);
        wndMain.Destroy();
    }

    catch(TError& errToCatch)
    {
        errToCatch.AddStackLevel(CID_FILE, CID_LINE);
    }

    return tCIDLib::EExitCodes::Normal;
}

