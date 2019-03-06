//
// FILE NAME: TestCIDWnd_TestDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/04/2001
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
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
RTTIDecls(TTestDlg,TDlgBox)


// ---------------------------------------------------------------------------
//  TTestDlg: Constructors and destructor
// ---------------------------------------------------------------------------
TTestDlg::TTestDlg() :

    m_pwndClose(0)
{
}

TTestDlg::~TTestDlg()
{
    Destroy();
}


// ---------------------------------------------------------------------------
//  TTestDlg: Public, inherited methods
//
//  These handle callbacks of the gesture handler target mixin. The gesture
//  handler in the base window class calls us back here.
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TTestDlg::bProcessGestEv(const  tCIDWnd::EGestEvs   eEv
                        , const TPoint&             pntStart
                        , const TPoint&             pntAt
                        , const TPoint&             pntDelta
                        , const tCIDLib::TBoolean   bTwoFingers)
{
    if (eEv == tCIDWnd::EGestEv_Start)
    {
        m_strTmpMsg = L"GEST Start   F:";
        if (bTwoFingers)
            m_strTmpMsg.Append(L"2");
        else
            m_strTmpMsg.Append(L"1");
        m_strTmpMsg.Append(L"   AT:");
        m_strTmpMsg.AppendFormatted(pntAt.i4X());
        m_strTmpMsg.Append(L'.');
        m_strTmpMsg.AppendFormatted(pntAt.i4Y());

        m_pwndList->AppendText(m_strTmpMsg, facCIDWnd().rgbBlack);

        //
        //  Simulate a little time to set up the scrollable contents that would
        //  be required in most real apps.
        //
//        TThread::Sleep(500);

        // Move the circle to here
        TGraphWndDev gdevTmp(*this);
        MoveCircle(gdevTmp, pntAt);
    }
     else if ((eEv == tCIDWnd::EGestEv_End)
          ||  (eEv == tCIDWnd::EGestEv_Drag)
          ||  (eEv == tCIDWnd::EGestEv_Inertia))
    {
#if 0
        // These are essentially the same in what we do
        if (eEv == tCIDWnd::EGestEv_End)
            m_strTmpMsg = L"GEST End   F:";
        else if (eEv == tCIDWnd::EGestEv_Inertia)
            m_strTmpMsg = L"GEST Inert F:";
        else
            m_strTmpMsg = L"GEST Move  F:";

        if (bTwoFingers)
            m_strTmpMsg.Append(L"2");
        else
            m_strTmpMsg.Append(L"1");

        m_strTmpMsg.Append(L"   AT:");
        m_strTmpMsg.AppendFormatted(pntAt.i4X());
        m_strTmpMsg.Append(L'.');
        m_strTmpMsg.AppendFormatted(pntAt.i4Y());
        m_strTmpMsg.Append(L"   DELTA:");
        m_strTmpMsg.AppendFormatted(pntDelta.i4X());
        m_strTmpMsg.Append(L'.');
        m_strTmpMsg.AppendFormatted(pntDelta.i4Y());

        m_pwndList->AppendText(m_strTmpMsg);
#endif

        TGraphWndDev gdevTmp(*this);
        AdjustCircle(gdevTmp, pntDelta);
    }
    return kCIDLib::True;
}


tCIDLib::TVoid TTestDlg::Clicked(const TPoint& pntAt)
{
    m_strTmpMsg = L"CLICKED  ";
    m_strTmpMsg.AppendFormatted(pntAt.i4X());
    m_strTmpMsg.Append(L'.');
    m_strTmpMsg.AppendFormatted(pntAt.i4Y());

    m_pwndList->AppendText(m_strTmpMsg, facCIDWnd().rgbRed);
}


//
//  We don't use this. We just call our parent to get the default action,
//  but override in cae we want to display something for debugging.
//
tCIDLib::TVoid
TTestDlg::GestInertiaIdle(  const   tCIDLib::TEncodedTime   enctEnd
                            , const tCIDLib::TCard4         c4Millis
                            , const tCIDLib::EDirs          eDir)
{
    TParent::GestInertiaIdle(enctEnd, c4Millis, eDir);
}


// We call our parent then output the reason code for debugging
tCIDLib::TVoid TTestDlg::GestReset(const tCIDWnd::EGestReset eType)
{
    m_strTmpMsg = L"RESET  ";
    switch(eType)
    {
        case tCIDWnd::EGestReset_Starting :
            m_strTmpMsg.Append(L"Start");
            break;

        case tCIDWnd::EGestReset_Ended :
            m_strTmpMsg.Append(L"End");
            break;

        case tCIDWnd::EGestReset_Cancelled :
            m_strTmpMsg.Append(L"Cancel");
            break;

        default :
            m_strTmpMsg.Append(L"???");
            break;
    };

    m_pwndList->AppendText(m_strTmpMsg, facCIDWnd().rgbBlack);
}


tCIDLib::TVoid
TTestDlg::PerGestOpts(  const   TPoint&                 pntAt
                        , const tCIDLib::EDirs          eDir
                        , const tCIDLib::TBoolean       bTwoFingers
                        ,       tCIDWnd::EGestOpts&     eGestOps
                        ,       tCIDLib::TFloat4&       f4VScale)
{
//    if ((eDir == tCIDLib::EDirs::Left) || (eDir == tCIDLib::EDirs::Right))
        eGestOps = tCIDWnd::EGestOpt_PanInertia;
//    else if (!bTwoFingers)
//        eGestOps = tCIDWnd::EGestOpt_Flick;

    f4VScale = 0.8F;
}


tCIDLib::TVoid
TTestDlg::ProcessFlick( const   tCIDLib::EDirs  eDirection
                        , const TPoint&         pntStartPos)
{
    m_strTmpMsg = L"FLICK ";
    m_strTmpMsg.AppendFormatted(pntStartPos.i4X());
    m_strTmpMsg.Append(L'.');
    m_strTmpMsg.AppendFormatted(pntStartPos.i4Y());
    m_strTmpMsg.Append(L"   ");
    switch(eDirection)
    {
        case tCIDLib::EDirs::Left :
            m_strTmpMsg.Append(L"Left");
            break;

        case tCIDLib::EDirs::Right :
            m_strTmpMsg.Append(L"Right");
            break;

        case tCIDLib::EDirs::Up :
            m_strTmpMsg.Append(L"Up");
            break;

        case tCIDLib::EDirs::Down :
            m_strTmpMsg.Append(L"Down");
            break;

        default :
            m_strTmpMsg.Append(L"[Unknown direction]");
            break;
    };

    m_pwndList->AppendText(m_strTmpMsg, facCIDWnd().rgbRed);
}



// ---------------------------------------------------------------------------
//  TTestDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TTestDlg::bRunDlg(TFrameWindow& wndOwner)
{
    if (c4Run(wndOwner, facTestCIDWnd, kTestRes::ridDlg_Test))
        return kCIDLib::True;
    return kCIDLib::False;
}



// ---------------------------------------------------------------------------
//  TTestDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TTestDlg::bPreLoop()
{
    // Call our parent first
    TParent::bPreLoop();

    pwndChildAs<TPushButton>(kTestRes::ridDlg_Test_Close)->pnothRegisterHandler
    (
        this, &TTestDlg::eClickHandler
    );

    pwndChildAs<TPushButton>(kTestRes::ridDlg_Test_Draw)->pnothRegisterHandler
    (
        this, &TTestDlg::eClickHandler
    );

    // Get the place holder widget's area then destroy it
    m_areaDisplay = areaChildWnd(kTestRes::ridDlg_Test_Display);
    CastChildWnd(*this, kTestRes::ridDlg_Test_Display, m_pwndList);

    EnableGestures(kCIDLib::True);

    m_pntLast = areaWndSize().pntCenter();

    rgbBgn(facCIDWnd().rgbPaleGrey);
    rgbBgn2(facCIDWnd().rgbPaleGrey);


    // Indicate that the dialog box should be made visible now
    return kCIDLib::True;
}


tCIDLib::TVoid
TTestDlg::Paint(TGraphDrawDev& gdevToUse, const TArea& areaUpdate)
{
    TParent::Paint(gdevToUse, areaUpdate);
    gdevToUse.DrawFilledCircle(8, m_pntLast);


    const TString strText(L"This is the text to draw");
    const tCIDLib::TCard4 c4Height(32);
    const tCIDLib::TCard4 c4Move(36);


#if 0
    TArea areaOutput(600, 16, 240, c4Height);
    const TArea areaZOutput(0, 0, 240, c4Height);

    // Get the font set on the incoming device, so we can set it on the memory ones
    TFontSelAttrs fselToUse(gdevToUse);
    TGUIFont gfontToUse(fselToUse);

    // -------------------------------------------------------
    //  White on black
    // -------------------------------------------------------

    // Direct, white text on black
    {
        gdevToUse.Fill(areaOutput, facCIDWnd().rgbBlack);
        gdevToUse.eBackMixMode(tCIDWnd::EBackMixMode_Transparent);
        gdevToUse.SetTextColor(facCIDWnd().rgbWhite);
        TString strPerText(strText, L" DirWoB");
        gdevToUse.DrawText(strPerText, areaOutput);
    }
    areaOutput.AdjustOrg(0, c4Move);

    // Indirect, white text on black
    {
        TBitmap bmpText
        (
            areaOutput.szArea()
            , tCIDImage::EPixFmts::TrueAlpha
            , tCIDImage::EBitDepths::Eight
            , kCIDLib::True
        );

        TGraphMemDev gdevCont(gdevToUse, bmpText);
        TFontJanitor janFontDraw(&gdevCont, &gfontToUse);
        gdevCont.Fill(areaZOutput, facCIDWnd().rgbBlack);
        gdevCont.eBackMixMode(tCIDWnd::EBackMixMode_Transparent);
        gdevCont.SetTextColor(facCIDWnd().rgbWhite);

        TString strPerText(strText, L" IndWoB");
        gdevCont.DrawText(strPerText, areaZOutput);
        gdevToUse.CopyBits(gdevCont, areaZOutput, areaOutput);
    }
    areaOutput.AdjustOrg(0, c4Move);

    // Alpha, white text on black
    {
        TBitmap bmpText
        (
            areaOutput.szArea()
            , tCIDImage::EPixFmts::TrueAlpha
            , tCIDImage::EBitDepths::Eight
            , kCIDLib::True
        );

        {
            TGraphMemDev gdevCont(gdevToUse, bmpText);
            TFontJanitor janFontDraw(&gdevCont, &gfontToUse);
            gdevCont.Fill(areaZOutput, facCIDWnd().rgbBlack);
            gdevCont.eBackMixMode(tCIDWnd::EBackMixMode_Transparent);
            gdevCont.SetTextColor(facCIDWnd().rgbWhite);

            bmpText.pixaData().SetAllAlpha(0xFF);
            TString strPerText(strText, L" AlpWoB");
            gdevCont.DrawText(strPerText, areaZOutput);
        }
        bmpText.pixaData().InvertAlphaChannel();
        bmpText.pixaData().Premultiply();

        gdevToUse.Fill(areaOutput, facCIDWnd().rgbBlack);
        gdevToUse.AlphaBlit(bmpText, areaZOutput, areaOutput, 0xFF, kCIDLib::True);
    }
    areaOutput.AdjustOrg(0, c4Move);



    // -------------------------------------------------------
    //  Bkack on white
    // -------------------------------------------------------

    // Direct, black text on white
    {
        gdevToUse.Fill(areaOutput, facCIDWnd().rgbWhite);
        gdevToUse.eBackMixMode(tCIDWnd::EBackMixMode_Transparent);
        gdevToUse.SetTextColor(facCIDWnd().rgbBlack);

        TString strPerText(strText, L" DirBoW");
        gdevToUse.DrawText(strPerText, areaOutput);
    }
    areaOutput.AdjustOrg(0, c4Move);


    // Indirect, black text on white
    {
        TBitmap bmpText
        (
            areaOutput.szArea()
            , tCIDImage::EPixFmts::TrueAlpha
            , tCIDImage::EBitDepths::Eight
            , kCIDLib::True
        );

        TGraphMemDev gdevCont(gdevToUse, bmpText);
        TFontJanitor janFontDraw(&gdevCont, &gfontToUse);
        gdevCont.Fill(areaZOutput, facCIDWnd().rgbWhite);
        gdevCont.eBackMixMode(tCIDWnd::EBackMixMode_Transparent);
        gdevCont.SetTextColor(facCIDWnd().rgbBlack);

        TString strPerText(strText, L" IndBoW");
        gdevCont.DrawText(strPerText, areaZOutput);
        gdevToUse.CopyBits(gdevCont, areaZOutput, areaOutput);
    }
    areaOutput.AdjustOrg(0, c4Move);

    // Alpha, black text on white
    {
        TBitmap bmpText
        (
            areaOutput.szArea()
            , tCIDImage::EPixFmts::TrueAlpha
            , tCIDImage::EBitDepths::Eight
            , kCIDLib::True
        );

        {
            TGraphMemDev gdevCont(gdevToUse, bmpText);
            TFontJanitor janFontDraw(&gdevCont, &gfontToUse);
            gdevCont.Fill(areaZOutput, facCIDWnd().rgbWhite);
            gdevCont.eBackMixMode(tCIDWnd::EBackMixMode_Transparent);
            gdevCont.SetTextColor(facCIDWnd().rgbBlack);

            bmpText.pixaData().SetAllAlpha(0xFF);
            TString strPerText(strText, L" AlpBoW");
            gdevCont.DrawText(strPerText, areaZOutput);
        }
        bmpText.pixaData().InvertAlphaChannel();

        gdevToUse.Fill(areaOutput, facCIDWnd().rgbWhite);
        gdevToUse.AlphaBlit(bmpText, areaZOutput, areaOutput, 0xFF, kCIDLib::True);
    }
    areaOutput.AdjustOrg(0, c4Move);



    // Direct, white text on red
    {
        gdevToUse.Fill(areaOutput, facCIDWnd().rgbRed);
        gdevToUse.eBackMixMode(tCIDWnd::EBackMixMode_Transparent);
        gdevToUse.SetTextColor(facCIDWnd().rgbWhite);

        TString strPerText(strText, L" DirWoR");
        gdevToUse.DrawText(strPerText, areaOutput);
    }
    areaOutput.AdjustOrg(0, c4Move);

    // Alpha, white text on red
    {
        TBitmap bmpText
        (
            areaOutput.szArea()
            , tCIDImage::EPixFmts::TrueAlpha
            , tCIDImage::EBitDepths::Eight
            , kCIDLib::True
        );

        {
            TGraphMemDev gdevCont(gdevToUse, bmpText);
            TFontJanitor janFontDraw(&gdevCont, &gfontToUse);
            gdevCont.Fill(areaZOutput, facCIDWnd().rgbRed);
            gdevCont.eBackMixMode(tCIDWnd::EBackMixMode_Transparent);
            gdevCont.SetTextColor(facCIDWnd().rgbWhite);

            bmpText.pixaData().SetAllAlpha(0xFF);
            TString strPerText(strText, L" AlpWoR");
            gdevCont.DrawText(strPerText, areaZOutput);
        }
        bmpText.pixaData().InvertAlphaChannel();
        bmpText.pixaData().Premultiply();

        gdevToUse.Fill(areaOutput, facCIDWnd().rgbRed);
        gdevToUse.AlphaBlit(bmpText, areaZOutput, areaOutput, 0xFF, kCIDLib::True);
    }
    areaOutput.AdjustOrg(0, c4Move);



    // Direct, white text on a gradient background
    {
        gdevToUse.GradientFill
        (
            areaOutput
            , facCIDWnd().rgbWhite
            , facCIDWnd().rgbBlack
            , tCIDWnd::EGradientDir_Vertical
        );
        gdevToUse.eBackMixMode(tCIDWnd::EBackMixMode_Transparent);
        gdevToUse.SetTextColor(facCIDWnd().rgbWhite);

        TString strPerText(strText, L" DirWoGr");
        gdevToUse.DrawText(strPerText, areaOutput);
    }
    areaOutput.AdjustOrg(0, c4Move);

    // Alpha, white text on a gradient background
    {
        TBitmap bmpText
        (
            areaOutput.szArea()
            , tCIDImage::EPixFmts::TrueAlpha
            , tCIDImage::EBitDepths::Eight
            , kCIDLib::True
        );

        {
            TGraphMemDev gdevCont(gdevToUse, bmpText);
            TFontJanitor janFontDraw(&gdevCont, &gfontToUse);
            gdevCont.GradientFill
            (
                areaZOutput
                , facCIDWnd().rgbWhite
                , facCIDWnd().rgbBlack
                , tCIDWnd::EGradientDir_Vertical
            );
            gdevCont.eBackMixMode(tCIDWnd::EBackMixMode_Transparent);
            gdevCont.SetTextColor(facCIDWnd().rgbWhite);

            bmpText.pixaData().SetAllAlpha(0xFF);
            TString strPerText(strText, L" AlpWoGr");
            gdevCont.DrawText(strPerText, areaZOutput);
        }
        bmpText.pixaData().InvertAlphaChannel();
        bmpText.pixaData().Premultiply();

        gdevToUse.GradientFill
        (
            areaOutput
            , facCIDWnd().rgbWhite
            , facCIDWnd().rgbBlack
            , tCIDWnd::EGradientDir_Vertical
        );
        gdevToUse.AlphaBlit(bmpText, areaZOutput, areaOutput, 0xFF, kCIDLib::True);
    }
    areaOutput.AdjustOrg(0, c4Move);
#endif

}


// ---------------------------------------------------------------------------
//  TTestDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TTestDlg::AdjustCircle(TGraphDrawDev& gdevTmp, const TPoint& pntDelta)
{
    // Adjust the current point and move the circle to there
    TPoint pntNew = m_pntLast;
    pntNew += pntDelta;
    MoveCircle(gdevTmp, pntNew);
}


tCIDWnd::EEventResponses TTestDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kTestRes::ridDlg_Test_Close)
    {
        EndDlg(0);
    }
     else if (wnotEvent.widSource() == kTestRes::ridDlg_Test_Draw)
    {
        m_pwndList->Clear();
    }
    return tCIDWnd::EEventResponse_Handled;
}


tCIDLib::TVoid
TTestDlg::MoveCircle(TGraphDrawDev& gdevTmp, const TPoint& pntNew)
{
    // Remove the previous circle
    TArea areaCur(m_pntLast, TSize(18, 18));
    areaCur.AdjustOrg(-9, -9);
    gdevTmp.Fill(areaCur, facCIDWnd().rgbPaleGrey);

    // Move to the new position
    m_pntLast = pntNew;
    gdevTmp.DrawFilledCircle(8, m_pntLast);
}


