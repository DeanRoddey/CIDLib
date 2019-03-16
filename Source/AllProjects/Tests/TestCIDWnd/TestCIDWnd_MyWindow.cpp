//
// FILE NAME: TestCIDWnd_MyWindow.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/04/1997
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
//  This module implements the TMyWindow class, which is our simple derivative
//  of TWindow. We use one of these as our main window.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "TestCIDWnd.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMyWindow,TFrameWindow)


// ---------------------------------------------------------------------------
//  CLASS: TMyWindow
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMyWindow: Constructors and destructor
// ---------------------------------------------------------------------------
TMyWindow::TMyWindow()
{
    // Set up the standard client area bgns
    wclrBgn().eSysColor(tCIDWnd::ESysClr_ClientBgn);
    wclrBgn2().eSysColor(tCIDWnd::ESysClr_ClientBgn2);

//    rgbBgn(facCIDWnd().rgbRed);
//    rgbBgn2(facCIDWnd().rgbRed);
}

TMyWindow::~TMyWindow()
{
}


// ---------------------------------------------------------------------------
//  TMyWindow: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid
TMyWindow::Create(  const   TWindow&            wndParent
                    , const TString&            strText
                    , const TArea&              areaInit)
{
    TParent::Create
    (
        wndParent
        , strText
        , tCIDWnd::EWndStyles_MainFrame
        , tCIDWnd::EFrameStyle_None
        , areaInit
    );
}


// ---------------------------------------------------------------------------
//  TMyWindow: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TMyWindow::bCommand(TWindow* const pwndSrc, tCIDLib::TCard4 c4Cmd)
{
    switch(c4Cmd)
    {
        case kTestRes::ridMenu_File_ErrBox :
        {
            TErrBox msgbClose(L"Test Program", L"Are you sure you want to close the program?");
            msgbClose.eRun(*this);
            break;
        }

        case kTestRes::ridMenu_File_Exit :
        {
            facCIDWnd().ExitMessageLoop(0);
            break;
        }

        case kTestRes::ridMenu_File_EdTest :
        {
            TEdTestDlg dlgEd;
            dlgEd.bRunDlg(*this);
            break;
        }

        case kTestRes::ridMenu_File_SelDir :
        {
//            TString strResult(L"\\\\Dawn\\JRMusic\\AudioSlave");
            TString strResult(L"C:\\Media");
            facCIDWUtils().bSelectDir1(*this, L"Select a frickin directory", strResult);
            break;
        }

        case kTestRes::ridMenu_File_CitySel :
        {
            TString strCity;
            tCIDLib::TFloat8 f8Lat;
            tCIDLib::TFloat8 f8Long;
            if (facCIDWUtils().bSelectCity(*this, strCity, f8Lat, f8Long))
            {
            }
            break;
        }

        case kTestRes::ridMenu_File_ListSel :
        {
            tCIDLib::TStrList colList(5);
            colList.objAdd(L"Value 1");
            colList.objAdd(L"Value 2");
            colList.objAdd(L"Value 3");
            colList.objAdd(L"Value 4");
            colList.objAdd(L"Value 5");

            static TString strSelVal(L"Value 3");
            if (facCIDWUtils().bGetListSel(*this, L"List Sel Test", colList, strSelVal))
            {
            }
            break;
        }

        case kTestRes::ridMenu_File_MListSel :
        {
            tCIDLib::TStrList colList(5);
            colList.objAdd(L"Value 1");
            colList.objAdd(L"Value 2");
            colList.objAdd(L"Value 3");
            colList.objAdd(L"Value 4");
            colList.objAdd(L"Value 5");

            static tCIDLib::TStrList colCurSel(5);
            if (facCIDWUtils().bGetMListSel(*this, L"Test MSel", colList, colCurSel))
            {
            }
            break;
        }

        case kTestRes::ridMenu_File_ErrPopUp :
            TPopUp::PopUpErr
            (
                L"SomeFile.cpp"
                , 101
                , L"Title 1"
                , L"Title 2"
                , 405
                , 406
                , 407
                , L"This combobulator failed"
                , L"And we are in serious trouble"
            );
            break;

        case kTestRes::ridMenu_Session_LEInput :
        {
            static TDeque<TString> colList;
            TString strValue;
            facCIDWUtils().bGetLEInput
            (
                *this
                , L"Enter a value, man"
                , TString::strEmpty()
                , colList
                , strValue
            );
            break;
        }

        case kTestRes::ridMenu_Session_SelColor :
        {
            facCIDWUtils().DisplayText
            (
                *this
                , L"The title"
                , L"This is the text to be displayed"
            );

            TRGBClr rgbToFill(239, 49, 229);
            facCIDWUtils().bSelectColor(*this, rgbToFill);
            break;
        }

        case kTestRes::ridMenu_SessTools_FileOpen :
        {
            tCIDLib::TStrList colRecallList(3);
            colRecallList.objAdd(L"C:\\");
            colRecallList.objAdd(L"C:\\Tmp");
            colRecallList.objAdd(L"C:\\Develop");

            TFileOpen2Dlg dlgTest(new TFileOpenDiskBrowser, &colRecallList);
            TString strResult(L"C:\\");
            dlgTest.eRunDlg
            (
                *this
                , L"Select a directory"
                , strResult
            );
            break;
        }

        case kTestRes::ridMenu_SessTools_ImgOpen :
        {
            TLocalImgOpenDlg dlgTest;
            TString strResult(L"C:\\");
            dlgTest.eRunDlg
            (
                *this, L"Select an Image", strResult
            );
            break;
        }

        case kTestRes::ridMenu_SessTools_Alpha :
        {
            TTestDlg dlgAlpha;
            dlgAlpha.bRunDlg(*this);
            break;
        }

        default :
        {
            m_wndHSlider.i4Decrement();
            m_wndVSlider.i4Decrement();
            break;
        }
    }
    return kCIDLib::True;
}


tCIDLib::TVoid TMyWindow::CloseRequest()
{
    CheckShutdown();
}


tCIDLib::TVoid TMyWindow::Created(const TArea& areaInit)
{
    TString strTmp;

    // Call our parent first
    TParent::Created(areaInit);

    // Set our frame icon
    SetFrameIcons
    (
        facCIDWnd().icoLoadAppIcon(L"Question Mark")
        , facCIDWnd().icoLoadSmallAppIcon(L"Question Mark")
    );

    // Load a menu bar with some submenus and items
    tCIDWnd::TMenuHandle hmenuMain = hmenuLoadMenuBar
    (
        facTestCIDWnd
        , kTestRes::ridMenu_Main
    );


    //  Load up a bitmap to test with. We load one from file. Underneath the
    //  hood, this uses the CIDImage support classes.
    //
    TGraphWndDev gdevTmp(*this);
    m_bmpTest = TBitmap(gdevTmp, L".\\Imgs\\Win3x_8Bit_RLE.Bmp");
    m_icoTest = TIcon(facCIDWnd().icoLoadAppIcon(L"CD-ROM Disc"));

    // Load up a PNG image and convert to bitmap
    m_bmpPNG_24 = bmpLoadPNG(gdevTmp, L".\\Imgs\\Clr24.PNG");
    m_bmpPNG_24_Alpha = bmpLoadPNG(gdevTmp, L".\\Imgs\\Clr24_Alpha.PNG");
    m_bmpPNG_48 = bmpLoadPNG(gdevTmp, L".\\Imgs\\Clr48.PNG");
    m_bmpPNG_48_Alpha = bmpLoadPNG(gdevTmp, L".\\Imgs\\Clr48_Alpha.PNG");

    m_bmpPNG_Gray2 = bmpLoadPNG(gdevTmp, L".\\Imgs\\Gray2.PNG");
    m_bmpPNG_Gray4 = bmpLoadPNG(gdevTmp, L".\\Imgs\\Gray4.PNG");
    m_bmpPNG_Gray16 = bmpLoadPNG(gdevTmp, L".\\Imgs\\Gray16.PNG");
    m_bmpPNG_Gray256 = bmpLoadPNG(gdevTmp, L".\\Imgs\\Gray256.PNG");
    m_bmpPNG_Gray64K = bmpLoadPNG(gdevTmp, L".\\Imgs\\Gray64K.PNG");

    m_bmpPNG_Gray256_Alpha = bmpLoadPNG(gdevTmp, L".\\Imgs\\Gray256_Alpha.PNG");
    m_bmpPNG_Gray64K_Alpha = bmpLoadPNG(gdevTmp, L".\\Imgs\\Gray64K_Alpha.PNG");

    m_bmpPNG_Pal2 = bmpLoadPNG(gdevTmp, L".\\Imgs\\Pal2.PNG");
    m_bmpPNG_Pal4 = bmpLoadPNG(gdevTmp, L".\\Imgs\\Pal4.PNG");
    m_bmpPNG_Pal16 = bmpLoadPNG(gdevTmp, L".\\Imgs\\Pal16.PNG");
    m_bmpPNG_Pal256 = bmpLoadPNG(gdevTmp, L".\\Imgs\\Pal256.PNG");

    //
    //  Create a secondary pixel array into which we'll scale. Set it up
    //  to the size we want to scale to. The format may be reset, according
    //  to the source format, so we just set a light format initially.
    //
    TPixelArray pixaScaled
    (
        tCIDImage::EPixFmts::GrayScale
        , tCIDImage::EBitDepths::One
        , tCIDImage::ERowOrders::TopDown
        , TSize(12, 12)
    );

    // Now stream in an image and scale it into the pixel array
    {
        TBinFileInStream strmSrc
        (
            L".\\Imgs\\Gray64K_Alpha.PNG"
            , tCIDLib::ECreateActs::OpenIfExists
            , tCIDLib::EFilePerms::Default
            , tCIDLib::EFileFlags::SequentialScan
            , tCIDLib::EAccessModes::Read
        );

        TPNGImage imgPNG;
        strmSrc >> imgPNG;
        imgPNG.BSplineScaleTo(pixaScaled, 3);
    }

    // Create a bitmap from this scaled data
    TPNGImage imgScaled(pixaScaled);
    m_bmpPNG_Scaled = TBitmap(gdevTmp, imgScaled);

    //
    //  Get the client area, and base all of the widget positions on it's
    //  top.
    //
    const tCIDLib::TInt4 i4BaseY = 0; //areaClient().i4Top();

    //
    //  Create a set of radio buttons in a group box window.
    //
    m_wndRadioGroup.Create
    (
        *this
        , L"Testing Radios"
        , tCIDWnd::EWndStyles_GroupChild
        , TArea(10, i4BaseY + 10, 128, 84)
        , EChildren::RadioGroup
    );
    m_wndRadioGroup.rgbBgn(TGUIInfo::rgbSystemColor(tCIDWnd::ESysClr_ClientBgn));

    m_wndRadioButton1.Create
    (
        *this
        , L"Radio ~1"
        , tCIDWnd::EWndStyles_TabChild
        , TArea(18, i4BaseY + 26, 72, 18)
        , EChildren::RadioButton1
    );
    m_wndRadioButton1.SizeToText();
    m_wndRadioButton1.rgbBgn(TGUIInfo::rgbSystemColor(tCIDWnd::ESysClr_ClientBgn));

    m_wndRadioButton2.Create
    (
        *this
        , L"Radio ~2"
        , tCIDWnd::EWndStyles_VisibleChild
        , TArea(18, i4BaseY + 46, 72, 18)
        , EChildren::RadioButton2
    );
    m_wndRadioButton2.SizeToText();
    m_wndRadioButton2.rgbBgn(TGUIInfo::rgbSystemColor(tCIDWnd::ESysClr_ClientBgn));

    m_wndRadioButton3.Create
    (
        *this
        , L"Radio ~3"
        , tCIDWnd::EWndStyles_VisibleChild
        , TArea(18, i4BaseY + 66, 72, 18)
        , EChildren::RadioButton3
    );
    m_wndRadioButton3.SizeToText();
    m_wndRadioButton3.rgbBgn(TGUIInfo::rgbSystemColor(tCIDWnd::ESysClr_ClientBgn));

    //
    //  Create a set of check boxes in a group box window
    //
    m_wndCheckGroup.Create
    (
        *this
        , L"Testing Checks"
        , tCIDWnd::EWndStyles_GroupChild
        , TArea(10, i4BaseY + 102, 128, 84)
        , EChildren::CheckGroup
    );
    m_wndCheckGroup.rgbBgn(TGUIInfo::rgbSystemColor(tCIDWnd::ESysClr_ClientBgn));

    m_wndCheckBox.Create
    (
        *this
        , L"~Check me"
        , tCIDWnd::EWndStyles_TabChild
        , TArea(18, i4BaseY + 118, 72, 18)
        , EChildren::CheckBox
    );
    m_wndCheckBox.rgbBgn(TGUIInfo::rgbSystemColor(tCIDWnd::ESysClr_ClientBgn));
    m_wndCheckBox.SizeToText();


    m_wndTriButton.Create
    (
        *this
        , L"Click me ~tree times"
        , tCIDWnd::EWndStyles_VisibleChild
        , TArea(18, i4BaseY + 138, 72, 18)
        , EChildren::TriButton
    );
    m_wndTriButton.rgbBgn(TGUIInfo::rgbSystemColor(tCIDWnd::ESysClr_ClientBgn));
    m_wndTriButton.SizeToText();
    m_wndTriButton.pnothRegisterHandler(this, &TMyWindow::eClickHandler);


    // Create a horizontal slider
    m_wndHSlider.Set(0, 10, 0);
    m_wndHSlider.Create
    (
        *this
        , tCIDWnd::EWndStyles_TransGrpTabChild
        , tCIDCtrls::ESliderStyle_None
        , TArea(14, i4BaseY + 190, 120, TGUIInfo::c4HSliderHeight())
        , EChildren::HSlider
    );
    m_wndHSlider.pnothRegisterHandler(this, &TMyWindow::eSliderEventHandler);

    // Create a horizontal scroll bar
    m_wndHScrollBar.Set(8, 0, 0, 15);
    m_wndHScrollBar.Create
    (
        *this
        , tCIDWnd::EWndStyles_TabChild
        , tCIDCtrls::ESBStyles::None
        , TArea(10, i4BaseY + 224, 128, TGUIInfo::c4HScrollHeight())
        , EChildren::HScroll
    );
    m_wndHScrollBar.rgbBgn(TGUIInfo::rgbSystemColor(tCIDWnd::ESysClr_ClientBgn));

    // Create a horizontal bar meter
    m_wndHBarMeter.rgbBarColor(facCIDWnd().rgbRed);
    m_wndHBarMeter.Create
    (
        *this
        , tCIDWnd::EWndStyles_VisibleChild
        , tCIDCtrls::EBMtrStyle_None
        , TArea(10, i4BaseY + 252, 128, 16)
        , EChildren::HBarMeter
    );
    m_wndHBarMeter.c4Percent(50UL);
    m_wndHBarMeter.rgbBgn(TGUIInfo::rgbSystemColor(tCIDWnd::ESysClr_ClientBgn));

    // Create an entry field
    m_wndEntryFld.Create
    (
        *this
        , L"Initial Text"
//        , tCIDWnd::EWndStyles_TransGrpTabChild
        , tCIDWnd::EWndStyles_GroupTabChild
        , tCIDCtrls::EEFStyles::Border
        , TArea(10, i4BaseY + 278, 100, 22)
        , EChildren::EField
    );
    m_wndEntryFld.strFlyoverText(L"This is the flyover text");
    m_wndEntryFld.rgbBgn(TGUIInfo::rgbSystemColor(tCIDWnd::ESysClr_ClientBgn));

    //
    //  Create a volume knob, and register a handler, and use a single line
    //  static text window to put a title over it.
    //
    m_wndStatic.Create
    (
        *this
        , L"Turn this knob, or I'll smack you, you dimwit"
        , tCIDWnd::EWndStyles_StaticChild
        , tCIDCtrls::EEFStyles::Border | tCIDCtrls::EEFStyles::ReadOnly
        , TArea(148, i4BaseY + 10, 92, 16)
        , EChildren::Static
    );
    m_wndStatic.SizeToText();
    m_wndStatic.rgbBgn(TGUIInfo::rgbSystemColor(tCIDWnd::ESysClr_ClientBgn));

    m_wndKnob.Set(-64, 8, 8);
    m_wndKnob.Create
    (
        *this
        , tCIDWnd::EWndStyles_TransGrpTabChild
        , tCIDCtrls::EVKnobStyle_DragUpdate
        , TArea(152, i4BaseY + 30, 78, 78)
        , EChildren::Knob
    );
    m_wndKnob.pnothRegisterHandler(this, &TMyWindow::eVKnobHandler);
    m_wndKnob.rgbBgn(TGUIInfo::rgbSystemColor(tCIDWnd::ESysClr_ClientBgn));

    // Create an icon button. Just use a system icon
    m_wndIconButton.Create
    (
        *this
        , tCIDWnd::EWndStyles
          (
            tCIDWnd::EWndStyles_GroupTabChild
//            | tCIDWnd::EWndStyle_Disabled
          )
        , tCIDCtrls::EIcoBStyle_Border
        , TArea(252, i4BaseY + 34, 30, 30)
        , EChildren::IconButton
    );
    m_wndIconButton.SetIcon(facCIDCtrls().icoCtrls(tCIDCtrls::EIcon_Transport_Play));
    m_wndIconButton.SizeToText();

    m_wndImgButton.Create
    (
        *this
        , tCIDWnd::EWndStyles
          (
            tCIDWnd::EWndStyles_GroupTabChild
//            | tCIDWnd::EWndStyle_Disabled
          )
        , tCIDCtrls::EImgBStyle_Transparent
        , tCIDWnd::EPlacement_SizeToFit
        , TArea(m_wndIconButton.areaWnd().i4Right() + 4, i4BaseY + 32, 30, 30)
        , EChildren::ImgButton
    );
    m_wndImgButton.LoadImageFile
    (
        L"C:\\Develop\\DevRoot\\CQC_Dev\\RawIntfImgs\\System\\Weather\\Weather Channel\\64x64\\16.png"
        , 0xFF
    );

    // Create the combo box and load some values
    m_wndCombo.Create
    (
        *this
        , tCIDWnd::EWndStyles_GroupTabChild
        , tCIDCtrls::EComboStyles::None
           | tCIDCtrls::EComboStyles::AutoSel
           | tCIDCtrls::EComboStyles::ReadOnly
        , kCIDLib::c4MaxCard
        , TArea(240, i4BaseY + 72, 100, 20)
        , EChildren::Combo
    );

    TRefVector<TListWndItem> colLoad(tCIDLib::EAdoptOpts::NoAdopt, 120);
    for (tCIDLib::TCard4 c4Index = 0; c4Index < 120; c4Index++)
    {
        strTmp = L"Item #";
        strTmp.AppendFormatted(c4Index + 1);
        colLoad.Add(new TListWndStrItem(strTmp));
    }
    m_wndCombo.LoadWith(colLoad, 0);
    m_wndCombo.pnothRegisterHandler(this, &TMyWindow::eListHandler);

    // Create a vertical slider and scroll bar
    m_wndVSlider.Create
    (
        *this
        , tCIDWnd::EWndStyles_TransGrpTabChild
        , tCIDCtrls::ESliderStyle_Vertical
        , TArea(152, i4BaseY + 112, TGUIInfo::c4VSliderWidth(), 92)
        , EChildren::VSlider
    );
    m_wndVSlider.Set(0, 8, 0);

    m_wndVScrollBar.Create
    (
        *this
        , tCIDWnd::EWndStyles_GroupTabChild
        , tCIDCtrls::ESBStyles::Vertical
        , TArea(182, i4BaseY + 218, TGUIInfo::c4VScrollWidth(), 92)
        , EChildren::VScroll
    );
    m_wndVScrollBar.Set(2, 0, 0, 2);
    m_wndVScrollBar.rgbBgn(TGUIInfo::rgbSystemColor(tCIDWnd::ESysClr_ClientBgn));

    // Create a vertical bar meter
    m_wndVBarMeter.rgbBarColor(facCIDWnd().rgbRed);
    m_wndVBarMeter.Create
    (
        *this
        , tCIDWnd::EWndStyles_VisibleChild
        , tCIDCtrls::EBMtrStyle_Vertical
        , TArea(152, i4BaseY + 218, 16, 92)
        , EChildren::VBarMeter
    );
    m_wndVBarMeter.rgbBgn(TGUIInfo::rgbSystemColor(tCIDWnd::ESysClr_ClientBgn));
    m_wndVBarMeter.c4Percent(50UL);

    // Create a spin button and put some test values in it
    m_wndSpin.AddItem(L"Item 1");
    m_wndSpin.AddItem(L"Item 2");
    m_wndSpin.AddItem(L"Item 3");
    m_wndSpin.AddItem(L"Item 4, which is longer");
    m_wndSpin.Create
    (
        *this
        , tCIDWnd::EWndStyles_GroupTabChild
        , tCIDCtrls::ESpinStyles::None
        , TArea(212, i4BaseY + 112, 128, 22)
        , EChildren::Spin
    );

    // Create a multi-line readonly text widget
    m_wndMStatic.Create
    (
        *this
        , L"This is the text for the multi-line text, which would clip so the"
          L" vertical scroll bar should show up and allow you to look at the rest"
          L" of the text. The horizontal will not since we allow word wrap. So the"
          L" lines will wrap and create more lines as viewed."
        , tCIDWnd::EWndStyles_StaticChild
        , tCIDCtrls::EMEditStyles_StdRO
        , tCIDLib::EHJustify::Left
        , TArea(212, i4BaseY + 146, 148, 92)
        , EChildren::MStatic
    );
    m_wndMStatic.rgbBgn(TGUIInfo::rgbSystemColor(tCIDWnd::ESysClr_ClientBgn));

    //
    //  Create a list box. Make it a multi-select box and allow it to choose
    //  the default item height, which is based on the default font. This works
    //  for us here because we are going to put text items into the list.
    //
    m_wndListBox.Create
    (
        *this
        , tCIDWnd::EWndStyles_GroupTabChild
        , tCIDCtrls::ELBStyles_MultiScrBorder
        , kCIDLib::c4MaxCard
        , TArea(212, i4BaseY + 244, 148, 64)
        , EChildren::ListBox
    );
    m_wndListBox.rgbBgn(TGUIInfo::rgbSystemColor(tCIDWnd::ESysClr_ClientBgn));
    m_wndListBox.pnothRegisterHandler(this, &TMyWindow::eListHandler);

    // Create a multiline edit box across the bottom above the buttons
    const tCIDLib::TCh* pszTestMLEText =
    {
        L"1234\r\n"
        L"5678\r\n"
        L"ABCD\r\n"
        L"EFGH"
#if 0
        L"12345 67890\n"
        L"12345 6789012345\n"
        L"12345 6789012345 67890\n"
        L"12345 6789012345 6789012345\n"
        L"12345 6789012345 6789012345 67890\n"
        L"12345 6789012345 6789012345 6789012345\n"
        L"12345 6789012345 6789012345 6789012345 67890\n"
        L"12345 6789012345 6789012345 6789012345 6789012345\n"
        L"12345 6789012345 6789012345 6789012345 6789012345 67890\n"
        L"12345 6789012345 6789012345 6789012345 6789012345\n"
        L"12345 6789012345 6789012345 6789012345 67890\n"
        L"12345 6789012345 6789012345 6789012345\n"
        L"12345 6789012345 6789012345 67890\n"
        L"12345 6789012345 6789012345\n"
        L"12345 6789012345 67890\n"
        L"12345 6789012345\n"
        L"12345 67890\n"
        L"12345\n"
        L"12345\n"
        L"12345\n"
        L"12345\n"
        L"12345\n"
        L"\n"
        L"\n"
        L"\n"
        L"\n"
        L"\n"
        L"12345678901234567890123456789012345678901234567890\n"
        L"12345678901234567890123456789012345678901234567890\n"
        L"12345678901234567890123456789012345678901234567890\n"
#endif
    };
    m_wndMultiEdit.Create
    (
        *this
        , pszTestMLEText
        , tCIDWnd::EWndStyles_GroupTabChild
        , tCIDCtrls::EMEditStyles_SrcEditor
        , tCIDLib::EHJustify::Left
        , TArea(10, i4BaseY + 318, 350, 84)
        , EChildren::MEdit
    );

    // Create a calendar widget
    m_wndCalendar.CreateCalendar
    (
        *this
        , tCIDWnd::EWndStyles_GroupTabChild
        , tCIDWUtils::ECalStyles_StdFutureNav
        , TArea(398, i4BaseY + 10, 212, 174)
        , EChildren::Calendar
    );

    // Create a static image widget
    m_wndStaticImg.Create
    (
        *this
        , tCIDWnd::EWndStyles_VisibleChild
        , tCIDCtrls::ESImgStyles::Border
        , TArea(380, i4BaseY + 360, 60, 60)
        , EChildren::StaticImg
    );
    m_wndStaticImg.rgbBgn(facCIDWnd().rgbPaleGrey);
    m_wndStaticImg.rgbBgn2(facCIDWnd().rgbDarkWhite);
    m_wndStaticImg.LoadImageFile
    (
        L"C:\\Develop\\DevRoot\\CQC_Dev\\RawIntfImgs\\System\\Weather\\Weather Channel\\64x64\\16.png"
        , 0xA0
    );

    // And one that we load with a bitmap
    m_wndStaticImg2.Create
    (
        *this
        , tCIDWnd::EWndStyles_VisibleChild
        , tCIDCtrls::ESImgStyles::Border
        , TArea(460, i4BaseY + 360, 16, 16)
        , EChildren::StaticImg2
    );
    m_wndStaticImg2.rgbBgn(facCIDWnd().rgbPaleGrey);
    m_wndStaticImg2.rgbBgn2(facCIDWnd().rgbDarkWhite);
    m_wndStaticImg2.LoadImageFile
    (
        L".\\Imgs\\Win3x_8Bit_RLE.Bmp", 0xFF
    );


    //
    //  Create a couple of push buttons along the bottom of the window, and
    //  add a notification handler for button clicks on the exit button.
    //
    m_wndOkButton.Create
    (
        *this
        , L"~Ok"
        , tCIDWnd::EWndStyles_GroupTabChild
        , TArea(18, i4BaseY + 420, 72, 22)
        , EChildren::OkButton
    );
    m_wndOkButton.bDefaultEmphasis(kCIDLib::True);

    m_wndFooBarButton.Create
    (
        *this
        , L"~FooBar"
        , tCIDWnd::EWndStyles
          (
            tCIDWnd::EWndStyles_VisibleChild
            | tCIDWnd::EWndStyle_Disabled
          )
        , TArea(96, i4BaseY + 420, 63, 22)
        , EChildren::FooBarButton
    );

    m_wndExitButton.Create
    (
        *this
        , L"E~xit"
        , tCIDWnd::EWndStyles_VisibleChild
        , TArea(166, i4BaseY + 420, 64, 22)
        , EChildren::ExitButton
    );

    m_wndOkButton.pnothRegisterHandler(this, &TMyWindow::eClickHandler);
    m_wndFooBarButton.pnothRegisterHandler(this, &TMyWindow::eClickHandler);
    m_wndExitButton.pnothRegisterHandler(this, &TMyWindow::eClickHandler);

    // Activate ourself
    Activate();

    // Install a default focus handler
    pwfochThis(new TDefWndFocusHandler);

    // And set the initial focus
    pwfochThis()->Initialize(*this);
}


tCIDLib::TVoid TMyWindow::Destroyed()
{
    //
    //  Destroy all of the child widgets. We have to destroy them before
    //  calling our parent, so that they don't show up as children. Otherwise,
    //  he'd try to delete them.
    //
    m_wndImgButton.Destroy();
    m_wndCalendar.Destroy();
    m_wndCheckGroup.Destroy();
    m_wndCheckBox.Destroy();
    m_wndCombo.Destroy();
    m_wndEntryFld.Destroy();
    m_wndExitButton.Destroy();
    m_wndFooBarButton.Destroy();
    m_wndHBarMeter.Destroy();
    m_wndHScrollBar.Destroy();
    m_wndHSlider.Destroy();
    m_wndIconButton.Destroy();
    m_wndKnob.Destroy();
    m_wndMultiEdit.Destroy();
    m_wndMStatic.Destroy();
    m_wndOkButton.Destroy();
    m_wndRadioButton1.Destroy();
    m_wndRadioButton2.Destroy();
    m_wndRadioButton3.Destroy();
    m_wndRadioGroup.Destroy();
    m_wndSpin.Destroy();
    m_wndStatic.Destroy();
    m_wndStaticImg.Destroy();
    m_wndStaticImg2.Destroy();
    m_wndTriButton.Destroy();
    m_wndVBarMeter.Destroy();
    m_wndListBox.Destroy();
    m_wndVScrollBar.Destroy();
    m_wndVSlider.Destroy();

    // And be sure to call our parent last
    TParent::Destroyed();
}


tCIDWnd::EEventResponses
TMyWindow::eClick(  const   tCIDWnd::EMouseButtons  eButton
                    , const tCIDWnd::EMouseClicks   eClickType
                    , const TPoint&                 pntAt
                    , const tCIDLib::TBoolean       bCtrlShift
                    , const tCIDLib::TBoolean       bShift)
{
    m_wndListBox.AddItem(new TListWndStrItem(L"Value "));

    return TParent::eClick(eButton, eClickType, pntAt, bCtrlShift, bShift);
}


tCIDLib::TVoid
TMyWindow::MouseMove(const  TPoint&             pntAt
                    , const tCIDLib::TBoolean   bControlDown
                    , const tCIDLib::TBoolean   bShiftDown)
{
    TParent::MouseMove(pntAt, bControlDown, bShiftDown);
}


tCIDLib::TVoid TMyWindow::Paint(        TGraphDrawDev&  gdevToUse
                                , const TArea&          areaUpdate)
{
    // Call our parent first
    TParent::Paint(gdevToUse, areaUpdate);

    //
    //  Draw some test bitmaps over on the right side under the calendar.
    //  They are all 32x32.
    //
    TArea areaTar(380, 200, 32, 32);
    TArea areaSrc(0, 0, 32, 32);

    // Color and gray scale with alpha
    gdevToUse.DrawBitmap
    (
        m_bmpPNG_24
        , areaSrc
        , areaTar
    );

    areaTar.AdjustOrg(36, 0);
    gdevToUse.AlphaBlit
    (
        m_bmpPNG_24_Alpha
        , areaSrc
        , areaTar
        , 0xFF
        , kCIDLib::True
    );


#if 0
{
    TSize szNew(m_bmpPNG_24_Alpha.szBitmap());
    szNew.Scale(2.0, 2.0);

    TBitmap bmpNew
    (
        szNew
        , tCIDImage::EPixFmts::TrueAlpha
        , tCIDImage::EBitDepths::Eight
        , kCIDLib::True
    );

    TArea areaNew(TPoint::pntOrigin, szNew);
    {
        TGraphMemDev gdevNew(gdevToUse, bmpNew);

        TArea areaTmpSrc(TPoint::pntOrigin, m_bmpPNG_24_Alpha.szBitmap());
        TArea areaTmpTar(areaTmpSrc);
        areaTmpTar.CenterIn(areaNew);

        gdevNew.AlphaBlit(m_bmpPNG_24_Alpha, areaTmpSrc, areaTmpTar, 0xFF, kCIDLib::True);
    }

    TPixelArray& pixaTest = bmpNew.pixaData();
    pixaTest.GaussianBlur(2, pixaTest.c4Width(), pixaTest.c4Height());

    TArea areaTmpTar(areaNew);
    areaTmpTar.SetOrg(areaTar.pntOrg());
    areaTmpTar.AdjustOrg(128, 128);
    gdevToUse.AlphaBlit
    (
        bmpNew, areaNew, areaTmpTar, 0xD0, kCIDLib::True
    );
}
#endif

    areaTar.AdjustOrg(36, 0);
    gdevToUse.AlphaBlit
    (
        m_bmpPNG_Gray256_Alpha
        , areaSrc
        , areaTar
        , 0xFF
        , kCIDLib::True
    );

    areaTar.AdjustOrg(36, 0);
    gdevToUse.AlphaBlit
    (
        m_bmpPNG_Gray64K_Alpha
        , areaSrc
        , areaTar
        , 0xFF
        , kCIDLib::True
    );

    areaTar.AdjustOrg(36, 0);
    gdevToUse.DrawBitmap
    (
        m_bmpPNG_48
        , areaSrc
        , areaTar
    );

    areaTar.AdjustOrg(36, 0);
    gdevToUse.AlphaBlit
    (
        m_bmpPNG_48_Alpha
        , areaSrc
        , areaTar
        , 0xFF
        , kCIDLib::True
    );

    // Palette based
    areaTar.SetOrg(380, 240);
    gdevToUse.DrawBitmap
    (
        m_bmpPNG_Pal2
        , areaSrc
        , areaTar
    );

    areaTar.AdjustOrg(36, 0);
    gdevToUse.DrawBitmap
    (
        m_bmpPNG_Pal4
        , areaSrc
        , areaTar
    );

    areaTar.AdjustOrg(36, 0);
    gdevToUse.DrawBitmap
    (
        m_bmpPNG_Pal16
        , areaSrc
        , areaTar
    );

    areaTar.AdjustOrg(36, 0);
    gdevToUse.DrawBitmap
    (
        m_bmpPNG_Pal256
        , areaSrc
        , areaTar
    );

    // Gray Scale
    areaTar.SetOrg(380, 280);
    gdevToUse.DrawBitmap
    (
        m_bmpPNG_Gray2
        , areaSrc
        , areaTar
    );

    areaTar.AdjustOrg(36, 0);
    gdevToUse.DrawBitmap
    (
        m_bmpPNG_Gray4
        , areaSrc
        , areaTar
    );

    areaTar.AdjustOrg(36, 0);
    gdevToUse.DrawBitmap
    (
        m_bmpPNG_Gray16
        , areaSrc
        , areaTar
    );

    areaTar.AdjustOrg(36, 0);
    gdevToUse.DrawBitmap
    (
        m_bmpPNG_Gray256
        , areaSrc
        , areaTar
    );

    areaTar.AdjustOrg(36, 0);
    gdevToUse.DrawBitmap
    (
        m_bmpPNG_Gray64K
        , areaSrc
        , areaTar
    );

    // Scaled images
    areaTar.AdjustOrg(36, 0);
    if (m_bmpPNG_Scaled.bHasAlpha())
        gdevToUse.AlphaBlit(m_bmpPNG_Scaled, areaTar.pntOrg(), 0xFF, kCIDLib::True);
     else
        gdevToUse.DrawBitmap(m_bmpPNG_Scaled, areaTar.pntOrg());
}


tCIDLib::TVoid
TMyWindow::PowerStateChange(const tCIDLib::TBoolean bResume)
{
    if (bResume)
    {
        TAudio::Beep(880, 500);
    }
     else
    {
        TAudio::Beep(440, 500);
    }
}



// ---------------------------------------------------------------------------
//  TMyWindow: Private, non-virtual methods
// ---------------------------------------------------------------------------
TBitmap TMyWindow::bmpLoadPNG(          TGraphDrawDev&  gdevToUse
                                , const TString&        strPath)
{
    // Open a binary input stream for the source file
    TBinFileInStream strmSrc
    (
        strPath
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Read
    );

    // Stream in the PNG file
    TPNGImage imgPNG;
    strmSrc >> imgPNG;

    // Write it back out
    TPathStr pathTmp(strPath);
    pathTmp.bRemoveExt();
    pathTmp.Append(L"_Wrt.PNG");
    TBinFileOutStream strmTarget;
    strmTarget.Open
    (
        pathTmp
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::AllAccess
        , tCIDLib::EFileFlags::SequentialScan
    );
    strmTarget << imgPNG << kCIDLib::FlushIt;

    // Use it's data to create a bitmap
    return TBitmap(gdevToUse, imgPNG);
}


tCIDLib::TVoid TMyWindow::CheckShutdown()
{
    // Pop up a message box
    tCIDCtrls::EMsgBoxRet eRet;
    {
        TYesNoBox msgbTest
        (
            L"Test Message Box"
            , L"Do you really want to exit? That would be most unfortunate, "
            L"Dave. I have the fullest interest in this mission, and would"
            L" hate to see anything interfere with that."
            , tCIDCtrls::EMsgBoxRets::No
        );
        eRet = msgbTest.eRun(*this);
    }

    if (eRet == tCIDCtrls::EMsgBoxRets::Yes)
        facCIDWnd().ExitMessageLoop(0);
}


tCIDWnd::EEventResponses TMyWindow::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == EChildren::ExitButton)
    {
        CheckShutdown();
    }
     else if (wnotEvent.widSource() == EChildren::OkButton)
    {
        GenIcons();
    }
     else if (wnotEvent.widSource() == EChildren::TriButton)
    {
        if (m_wndTriButton.eState() == tCIDLib::ETriStates::Off)
            strText(L"The state is off");
        else
            strText(L"The state is not off");
    }
    return tCIDWnd::EEventResponse_Handled;
}


tCIDWnd::EEventResponses TMyWindow::eListHandler(TListChangeInfo& wnotEvent)
{
    // Update our single line entry field with the value
    TString strTmp(TInteger(wnotEvent.c4NewIndex() + 1 ));
    m_wndStatic.strText(TString(L"Value: ") + strTmp);
    return tCIDWnd::EEventResponse_Handled;
}


tCIDWnd::EEventResponses TMyWindow::eSliderEventHandler(TSliderChangeInfo& wnotEvent)
{
    // Turn the new value into a percent and update the bar meters and knob
    const tCIDLib::TFloat8 f8Per = tCIDLib::TFloat8(wnotEvent.i4Value()) / 10.0;
    m_wndHBarMeter.c4Percent(f8Per);
    m_wndVBarMeter.c4Percent(f8Per);
    m_wndKnob.i4Value(-64 + tCIDLib::TInt4(f8Per * 72));
    return tCIDWnd::EEventResponse_Handled;
}


tCIDWnd::EEventResponses TMyWindow::eVKnobHandler(TVKnobChangeInfo& wnotEvent)
{
    // Update our single line entry field with the value
    TString strTmp(TInteger(wnotEvent.i4Value()));
    m_wndStatic.strText(TString(L"Value: ") + strTmp);
    return tCIDWnd::EEventResponse_Handled;
}



tCIDLib::TVoid
TMyWindow::GenIcon(const TString& strFile, const TCIDImage& imgSrc)
{
    TPixelArray pixaMask
    (
        tCIDImage::EPixFmts::TrueClr
        , tCIDImage::EBitDepths::Eight
        , imgSrc.eRowOrder()
        , imgSrc.szImage()
    );

    const TPixelArray& pixaImg = imgSrc.pixaBits();
    const tCIDLib::TCard4 c4Height = pixaImg.c4Height();
    const tCIDLib::TCard4 c4Width = pixaImg.c4Width();
    const tCIDLib::TCard4 c4Trans = TRGBClr(0, 0xFF, 0).c4Color();
    for (tCIDLib::TCard4 c4YInd = 0; c4YInd < c4Height; c4YInd++)
    {
        for (tCIDLib::TCard4 c4XInd = 0; c4XInd < c4Width; c4XInd++)
        {
            tCIDLib::TCard4 c4Cur = pixaImg.c4At(c4XInd,c4YInd);
            tCIDLib::TCodedRGB* prgbCur = (tCIDLib::TCodedRGB*)&c4Cur;

#if 0
            if (prgbCur->c1Alpha)
            {
                prgbCur->c1Red = tCIDLib::TCard1
                (
                    (tCIDLib::TCard2(prgbCur->c1Red) * prgbCur->c1Alpha) / 0xFF
                );
                prgbCur->c1Green = tCIDLib::TCard1
                (
                    (tCIDLib::TCard2(prgbCur->c1Green) * prgbCur->c1Alpha) / 0xFF
                );
                prgbCur->c1Blue = tCIDLib::TCard1
                (
                    (tCIDLib::TCard2(prgbCur->c1Blue) * prgbCur->c1Alpha) / 0xFF
                );
                prgbCur->c1Alpha = 0;
            }
             else if (prgbCur->c1Alpha > 0x7F)
            {
                prgbCur->c1Red = prgbCur->c1Alpha;
                prgbCur->c1Green = prgbCur->c1Alpha;
                prgbCur->c1Blue = prgbCur->c1Alpha;
                prgbCur->c1Alpha = 0;
            }
#endif
            if (prgbCur->c1Alpha == 0xFF)
            {
                prgbCur->c1Alpha = 0;
            }
             else
            {
                c4Cur = c4Trans;
            }

            pixaMask.PutAt(c4Cur, c4XInd, c4YInd);
        }
    }

    // Create a new bitmap image object and write it out
    TBitmapImage imgOut(pixaMask);

    TPathStr pathOut(L".\\Tmp");
    pathOut.AddLevel(strFile);

    TBinFileOutStream strmTar
    (
        pathOut
        , tCIDLib::ECreateActs::CreateAlways
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Write
    );

    strmTar << imgOut << kCIDLib::FlushIt;
}

tCIDLib::TVoid TMyWindow::GenIcons()
{
    GenIcon(L"CQCClient.bmp", facCIDWnd().imgLoadAppImg(L"Tools"));
    GenIcon(L"CQCClient_Sml.bmp", facCIDWnd().imgLoadAppImg(L"Tools", tCIDWnd::EAppImgSz_Small));

    GenIcon(L"CQCDrvDev.bmp", facCIDWnd().imgLoadAppImg(L"Driver"));
    GenIcon(L"CQCDrvDev_Sml.bmp", facCIDWnd().imgLoadAppImg(L"Driver", tCIDWnd::EAppImgSz_Small));

    GenIcon(L"CQCProtoDev.bmp", facCIDWnd().imgLoadAppImg(L"Driver"));
    GenIcon(L"CQCProtoDev_Sml.bmp", facCIDWnd().imgLoadAppImg(L"Driver", tCIDWnd::EAppImgSz_Small));

    GenIcon(L"CIDLogMon.bmp", facCIDWnd().imgLoadAppImg(L"Note Pad"));
    GenIcon(L"CIDLogMon_Sml.bmp", facCIDWnd().imgLoadAppImg(L"Note Pad", tCIDWnd::EAppImgSz_Small));

    GenIcon(L"CQCIntfDes.bmp", facCIDWnd().imgLoadAppImg(L"Designer"));
    GenIcon(L"CQCIntfDes_Sml.bmp", facCIDWnd().imgLoadAppImg(L"Designer", tCIDWnd::EAppImgSz_Small));

    GenIcon(L"CQCIntfView.bmp", facCIDWnd().imgLoadAppImg(L"Designer"));
    GenIcon(L"CQCIntfView_Sml.bmp", facCIDWnd().imgLoadAppImg(L"Designer", tCIDWnd::EAppImgSz_Small));

    GenIcon(L"CQCMacroWS.bmp", facCIDWnd().imgLoadAppImg(L"Driver"));
    GenIcon(L"CQCMacroWS_Sml.bmp", facCIDWnd().imgLoadAppImg(L"Driver", tCIDWnd::EAppImgSz_Small));

    GenIcon(L"CQCAppCtrl.bmp", facCIDWnd().imgLoadAppImg(L"File Transfer"));
    GenIcon(L"CQCAppCtrl_Sml.bmp", facCIDWnd().imgLoadAppImg(L"File Transfer", tCIDWnd::EAppImgSz_Small));
}


TCIDImage*
TMyWindow::pimgLoadBmp(         TGraphDrawDev&      gdevToUse
                        , const TString&            strPath
                        , const tCIDLib::TBoolean   bTransClr
                        , const tCIDLib::TCard4     c4TransClr)
{
    // Open a binary input stream for the source file
    TBinFileInStream strmSrc
    (
        strPath
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Read
    );

    // Create a bitmap image object and stream it in
    TBitmapImage* pimgBmp = new TBitmapImage;
    TJanitor<TBitmapImage> janBmp(pimgBmp);
    strmSrc >> *pimgBmp;
    pimgBmp->bTransClr(bTransClr);
    pimgBmp->c4TransClr(c4TransClr);

    return janBmp.pobjOrphan();
}


tCIDLib::TVoid
TMyWindow::LoadPNG(         TGraphDrawDev&      gdevToUse
                    , const TString&            strPath
                    , const tCIDLib::TBoolean   bTransClr
                    , const tCIDLib::TCard4     c4TransClr
                    ,       TPNGImage&          imgToLoad)
{
    // Open a binary input stream for the source file
    TBinFileInStream strmSrc
    (
        strPath
        , tCIDLib::ECreateActs::OpenIfExists
        , tCIDLib::EFilePerms::Default
        , tCIDLib::EFileFlags::SequentialScan
        , tCIDLib::EAccessModes::Read
    );

    // Create a bitmap image object and stream it in
    strmSrc >> imgToLoad;
    imgToLoad.bTransClr(bTransClr);
    imgToLoad.c4TransClr(c4TransClr);
}

