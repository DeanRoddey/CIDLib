//
// FILE NAME: CIDWUtils_24BitPaletteDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/28/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  Implements the T24BitPalDlg dialog.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDWUtils_.hpp"
#include    "CIDWUtils_24BitPaletteDlg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(T24BitPalDlg, TDlgBox)



// ---------------------------------------------------------------------------
//   CLASS: T24BitPalDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  T24BitPalDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
T24BitPalDlg::T24BitPalDlg() :

    m_bCaptureMode(kCIDLib::False)
    , m_pwndCapture(nullptr)
    , m_pwndCopy(nullptr)
    , m_pwndHex(nullptr)
    , m_pwndHSV_Hue(nullptr)
    , m_pwndHSV_Sat(nullptr)
    , m_pwndHSV_Val(nullptr)
    , m_pwndPal(nullptr)
    , m_pwndRGB_Blue(nullptr)
    , m_pwndRGB_Green(nullptr)
    , m_pwndRGB_Red(nullptr)
    , m_pwndSample(nullptr)
{
}

T24BitPalDlg::~T24BitPalDlg()
{
}


// ---------------------------------------------------------------------------
//  T24BitPalDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean T24BitPalDlg::bRun(const TWindow& wndOwner, TRGBClr& rgbToFill)
{
    // Get the starting color stored so we can see it below
    m_rgbEdit = rgbToFill;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_ClrPal
    );

    if (c4Res == kCIDWUtils::ridDlg_CPal_Accept)
    {
        rgbToFill = m_rgbEdit;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  T24BitPalDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean T24BitPalDlg::bCreated()
{
    // Now call our parent first and return his return
    const tCIDLib::TBoolean bRes = TParent::bCreated();

    // Get some typed pointers to some of our widgets, for convenience
    CastChildWnd(*this, kCIDWUtils::ridDlg_CPal_Capture, m_pwndCapture);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CPal_HexRadix, m_pwndHex);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CPal_Hue, m_pwndHSV_Hue);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CPal_Sat, m_pwndHSV_Sat);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CPal_Val, m_pwndHSV_Val);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CPal_Pal, m_pwndPal);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CPal_Blue, m_pwndRGB_Blue);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CPal_Green, m_pwndRGB_Green);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CPal_Red, m_pwndRGB_Red);

    CastChildWnd(*this, kCIDWUtils::ridDlg_CPal_Sample, m_pwndSample);

    // Store the sample window's area, slightly blown up
    m_areaSampleBorder = m_pwndSample->areaWnd();
    m_areaSampleBorder.Inflate(2);

    // Register our handlers
    pwndChildAs<TPushButton>(kCIDWUtils::ridDlg_CPal_Accept)->pnothRegisterHandler
    (
        this
        , &T24BitPalDlg::eClickHandler
    );
    pwndChildAs<TPushButton>(kCIDWUtils::ridDlg_CPal_Cancel)->pnothRegisterHandler
    (
        this
        , &T24BitPalDlg::eClickHandler
    );
    pwndChildAs<TPushButton>(kCIDWUtils::ridDlg_CPal_Copy)->pnothRegisterHandler
    (
        this
        , &T24BitPalDlg::eClickHandler
    );
    m_pwndHex->pnothRegisterHandler(this, &T24BitPalDlg::eClickHandler);
    m_pwndPal->pnothRegisterHandler(this, &T24BitPalDlg::ePalHandler);

    // Load the capture window's image
    m_pwndCapture->SetAppImage(L"Mouse", tCIDCtrls::EAppImgSz::Small);

    // Do an initial set of the color to the starting color we were given
    m_pwndPal->SetColor(m_rgbEdit);

    return bRes;
}


tCIDLib::TBoolean T24BitPalDlg::bSetPointer(const tCIDCtrls::EWndAreas)
{
    if (m_bCaptureMode)
    {
        facCIDCtrls().SetPointer(tCIDCtrls::ESysPtrs::Cross);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
T24BitPalDlg::bClick(const  tCIDCtrls::EMouseButts  eButton
                    , const tCIDCtrls::EMouseClicks eClickType
                    , const TPoint&                 pntAt
                    , const tCIDLib::TBoolean       bCtrlDown
                    , const tCIDLib::TBoolean       bShift)
{
    if (m_bCaptureMode)
    {
        // Just release the mouse and exit capture mode
        ReleaseMouse();
        m_bCaptureMode = kCIDLib::False;
    }

    // And pass to our parent class now for regular processing
    return TParent::bClick(eButton, eClickType, pntAt, bCtrlDown, bShift);
}


tCIDLib::TBoolean
T24BitPalDlg::bMouseMove(const  TPoint&             pntAt
                        , const tCIDLib::TBoolean   bCtrlShift
                        , const tCIDLib::TBoolean   bShift)
{
    if (m_bCaptureMode)
    {
        // Get a device for the whole screen
        TGraphDesktopDev gdevDesktop;

        // Convert the point to screen coordinates
        TPoint pntActual;
        ToScreenCoordinates(pntAt, pntActual);

        // Get the pixel at this point and use it to update our info
        m_rgbEdit = gdevDesktop.rgbAt(pntActual);
        m_pwndPal->SetColor(m_rgbEdit);
    }
    return TParent::bMouseMove(pntAt, bCtrlShift, bShift);
}



tCIDLib::TVoid T24BitPalDlg::LosingFocus()
{
    if (m_bCaptureMode)
    {
        ReleaseMouse();
        m_bCaptureMode = kCIDLib::False;
    }
    TParent::LosingFocus();
}


tCIDLib::TVoid
T24BitPalDlg::StaticChildClick( const   tCIDCtrls::TWndId       widChild
                                , const tCIDCtrls::EMouseButts  eButton
                                , const tCIDCtrls::EMouseClicks eClickType)
{
    if ((eButton == tCIDCtrls::EMouseButts::Left)
    &&  (eClickType == tCIDCtrls::EMouseClicks::Down)
    &&  (widChild == kCIDWUtils::ridDlg_CPal_Capture))
    {
        // They left clicked in our capture widget, so start the capture mode
        CaptureMouse();
        facCIDCtrls().SetPointer(tCIDCtrls::ESysPtrs::Cross);
        m_bCaptureMode = kCIDLib::True;
    }
}




// ---------------------------------------------------------------------------
//  T24BitPalDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses
T24BitPalDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_CPal_Accept)
    {
        // Save the current color for return
        m_rgbEdit = m_pwndPal->rgbCurrent();
        EndDlg(kCIDWUtils::ridDlg_CPal_Accept);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_CPal_Cancel)
    {
        EndDlg(kCIDWUtils::ridDlg_CPal_Cancel);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_CPal_Copy)
    {
        // Format the value to text and copy it to the clipboard
        TRGBClr rgbFmt = m_pwndPal->rgbCurrent();
        TString strFmt;
        rgbFmt.FormatToText(strFmt, kCIDLib::chComma);

        // And paste it to the clipboard
        TGUIClipboard gclipCopy(*this);
        gclipCopy.Clear();
        gclipCopy.StoreText(strFmt);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_CPal_HexRadix)
    {
        UpdateValues(m_pwndPal->rgbCurrent());
    }
    return tCIDCtrls::EEvResponses::Handled;
}


tCIDCtrls::EEvResponses
T24BitPalDlg::ePalHandler(T24BPalChangeInfo& wnotEvent)
{
    // Update the sample color window and the numeric color display
    const TRGBClr& rgbToShow = wnotEvent.rgbNewColor();
    m_pwndSample->SetBgnColor(rgbToShow, kCIDLib::True);
    m_pwndSample->ForceRepaint();

    UpdateValues(rgbToShow);
    return tCIDCtrls::EEvResponses::Handled;
}


tCIDLib::TVoid T24BitPalDlg::UpdateValues(const TRGBClr& rgbToShow)
{
    tCIDLib::ERadices eRadix = tCIDLib::ERadices::Dec;
    if (m_pwndHex->bCheckedState())
        eRadix = tCIDLib::ERadices::Hex;

    TString strTmp;
    strTmp.SetFormatted(rgbToShow.c4Red(), eRadix);
    pwndChildById(kCIDWUtils::ridDlg_CPal_Red)->strWndText(strTmp);

    strTmp.SetFormatted(rgbToShow.c4Green(), eRadix);
    pwndChildById(kCIDWUtils::ridDlg_CPal_Green)->strWndText(strTmp);

    strTmp.SetFormatted(rgbToShow.c4Blue(), eRadix);
    pwndChildById(kCIDWUtils::ridDlg_CPal_Blue)->strWndText(strTmp);

    THSVClr hsvTmp(rgbToShow);

    strTmp.SetFormatted(hsvTmp.f4Hue());
    pwndChildById(kCIDWUtils::ridDlg_CPal_Hue)->strWndText(strTmp);

    strTmp.SetFormatted(hsvTmp.f4Saturation());
    pwndChildById(kCIDWUtils::ridDlg_CPal_Sat)->strWndText(strTmp);

    strTmp.SetFormatted(hsvTmp.f4Value());
    pwndChildById(kCIDWUtils::ridDlg_CPal_Val)->strWndText(strTmp);
}


