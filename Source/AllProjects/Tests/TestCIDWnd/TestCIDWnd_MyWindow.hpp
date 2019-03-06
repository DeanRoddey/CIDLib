//
// FILE NAME: TestCIDWnd_MyWindow.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/04/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the TestCIDWnd_MyWindow.Cpp module. This module
//  implements a simple derivative of TWindow to use as the main window of
//  the program.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  CLASS: TMyWindow
// PREFIX: wnd
// ---------------------------------------------------------------------------
class TMyWindow : public TFrameWindow
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and destructor
        // -------------------------------------------------------------------
        TMyWindow();

        ~TMyWindow();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const TString&                strText
            , const TArea&                  areaInit
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCommand
        (
                    TWindow* const          pwndSrc
            , const tCIDLib::TCard4         c4Cmd
        );

        tCIDLib::TVoid CloseRequest();

        tCIDLib::TVoid Created
        (
            const   TArea&                  areaInit
        );

        tCIDLib::TVoid Destroyed();

        tCIDWnd::EEventResponses eClick
        (
            const   tCIDWnd::EMouseButtons  eButton
            , const tCIDWnd::EMouseClicks   eClickType
            , const TPoint&                 pntAt
            , const tCIDLib::TBoolean       bCtrlShift
            , const tCIDLib::TBoolean       bShift
        );

        tCIDLib::TVoid MouseMove
        (
            const   TPoint&                 pntAt
            , const tCIDLib::TBoolean       bControlDown
            , const tCIDLib::TBoolean       bShiftDown
        );

        tCIDLib::TVoid Paint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        );

        tCIDLib::TVoid PowerStateChange
        (
            const   tCIDLib::TBoolean       bResume
        );


    private :
        // -------------------------------------------------------------------
        //  Private types
        // -------------------------------------------------------------------
        enum class EChildren
        {
            RadioGroup       = 1
            , RadioButton1   = 2
            , RadioButton2   = 3
            , RadioButton3   = 4

            , CheckGroup     = 10
            , CheckBox       = 11
            , TriButton      = 12

            , HSlider        = 20
            , HScroll        = 21

            , HBarMeter      = 25

            , EField         = 30

            , Static         = 40
            , Knob           = 41

            , IconButton     = 45
            , ImgButton      = 46

            , Combo          = 50

            , VSlider        = 55
            , VScroll        = 56

            , VBarMeter      = 60

            , Spin           = 65
            , MStatic        = 66

            , ListBox        = 70

            , MEdit          = 75

            , Calendar       = 80

            , StaticImg      = 90
            , StaticImg2     = 91

            , OkButton       = 100
            , FooBarButton   = 101
            , ExitButton     = 102

            , Client         = 999
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        TBitmap bmpLoadPNG
        (
                    TGraphDrawDev&          gdevToUse
            , const TString&                strPath
        );

        tCIDLib::TVoid CheckShutdown();

        tCIDWnd::EEventResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );

        tCIDWnd::EEventResponses eListHandler
        (
                    TListChangeInfo&        wnotEvent
        );

        tCIDWnd::EEventResponses eSliderEventHandler
        (
                    TSliderChangeInfo&      wnotEvent
        );

        tCIDWnd::EEventResponses eVKnobHandler
        (
                    TVKnobChangeInfo&       wnotEvent
        );

        tCIDLib::TVoid LoadPNG
        (
                    TGraphDrawDev&          gdevToUse
            , const TString&                strPath
            , const tCIDLib::TBoolean       bTransClr
            , const tCIDLib::TCard4         c4TransClr
            ,       TPNGImage&              pngUse
        );

        tCIDLib::TVoid GenIcon
        (
            const   TString&                strName
            , const   TCIDImage&              imgSrc
        );

        tCIDLib::TVoid GenIcons();

        TCIDImage* pimgLoadBmp
        (
                    TGraphDrawDev&          gdevToUse
            , const TString&                strPath
            , const tCIDLib::TBoolean       bTransClr
            , const tCIDLib::TCard4         c4TransClr
        );



        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bmpPNG
        //  m_bmpTest
        //      Some test bitmap objects to draw.
        //
        //  m_icoTest
        //      A test icon to draw
        //
        //  m_wndCheckBox
        //      ....
        //      Some child windows for testing
        // -------------------------------------------------------------------
        TBitmap             m_bmpPNG_24;
        TBitmap             m_bmpPNG_24_Alpha;
        TBitmap             m_bmpPNG_48;
        TBitmap             m_bmpPNG_48_Alpha;
        TBitmap             m_bmpPNG_Gray2;
        TBitmap             m_bmpPNG_Gray4;
        TBitmap             m_bmpPNG_Gray16;
        TBitmap             m_bmpPNG_Gray256;
        TBitmap             m_bmpPNG_Gray256_Alpha;
        TBitmap             m_bmpPNG_Gray64K;
        TBitmap             m_bmpPNG_Gray64K_Alpha;
        TBitmap             m_bmpPNG_Pal2;
        TBitmap             m_bmpPNG_Pal4;
        TBitmap             m_bmpPNG_Pal16;
        TBitmap             m_bmpPNG_Pal256;
        TBitmap             m_bmpPNG_Scaled;
        TBitmap             m_bmpTest;
        TIcon               m_icoTest;
        TRadioButton        m_wndRadioButton1;
        TRadioButton        m_wndRadioButton2;
        TRadioButton        m_wndRadioButton3;
        TGroupWnd           m_wndRadioGroup;
        TGroupWnd           m_wndCheckGroup;
        TCheckBox           m_wndCheckBox;
        TTriStateButton     m_wndTriButton;
        TSlider             m_wndHSlider;
        TScrollBar          m_wndHScrollBar;
        TBarMeter           m_wndHBarMeter;
        TEntryField         m_wndStatic;
        TVolumeKnob         m_wndKnob;
        TIconButton         m_wndIconButton;
        TImgButton          m_wndImgButton;
        TComboBox           m_wndCombo;
        TSlider             m_wndVSlider;
        TScrollBar          m_wndVScrollBar;
        TBarMeter           m_wndVBarMeter;
        TSpinBox            m_wndSpin;
        TMultiEdit          m_wndMStatic;
        TListBox            m_wndListBox;
        TEntryField         m_wndEntryFld;
        TMultiEdit          m_wndMultiEdit;
        TCalendarWnd        m_wndCalendar;
        TStaticImage        m_wndStaticImg;
        TStaticImage        m_wndStaticImg2;
        TPushButton         m_wndOkButton;
        TPushButton         m_wndFooBarButton;
        TPushButton         m_wndExitButton;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMyWindow,TFrameWindow)
};

