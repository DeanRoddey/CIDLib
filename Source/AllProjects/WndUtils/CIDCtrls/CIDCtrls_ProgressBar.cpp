//
// FILE NAME: CIDCtrls_ProgressBar.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/09/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a couple variations on the progress bar
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
#include    "CIDCtrls_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TActivityBar, TStdCtrlWnd)
AdvRTTIDecls(TProgressBar, TStdCtrlWnd)



// ---------------------------------------------------------------------------
//  CLASS: TActivityBar
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TActivityBar: Constructors and Destructor
// ---------------------------------------------------------------------------
TActivityBar::TActivityBar() :

    TStdCtrlWnd()
    , m_ePBarStyles(tCIDCtrls::EPBarStyles::None)
{
}

TActivityBar::~TActivityBar()
{
}


// ---------------------------------------------------------------------------
//  TActivityBar: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TActivityBar::InitFromDesc( const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EPBarStyles  ePBStyles = tCIDCtrls::EPBarStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild
    );

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Vertical))
        ePBStyles |= tCIDCtrls::EPBarStyles::Vertical;

    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , eStyles
        , ePBStyles
    );
}


tCIDLib::TVoid TActivityBar::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_Vertical);
}



// ---------------------------------------------------------------------------
// TActivityBar: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Just set up the styles and call our parent to create the control
tCIDLib::TVoid
TActivityBar::Create(const  TWindow&                wndParent
                    , const tCIDCtrls::TWndId       widThis
                    , const TArea&                  areaInit
                    , const tCIDCtrls::EWndStyles   eStyles
                    , const tCIDCtrls::EPBarStyles  ePBarStyles)
{
    //
    //  Start with any styles we enforce. We only turn on the marque style when
    //  we are running it in marquee mode, so that we can set the position otherwise.
    //
    tCIDLib::TCard4 c4Styles = 0; // PBS_MARQUEE;

    // Add any class specific ones
    if (tCIDLib::bAllBitsOn(ePBarStyles, tCIDCtrls::EPBarStyles::Vertical))
        c4Styles |= PBS_VERTICAL;

    // Store away our class specific styles
    m_ePBarStyles = ePBarStyles;

    // And create the control
    CreateWnd
    (
        wndParent.hwndThis()
        , PROGRESS_CLASS
        , kCIDLib::pszEmptyZStr
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles(c4Styles)
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );

    // All standard controls must be subclassed
    SetSubclass();
}


// Pause/resume the bar
tCIDLib::TVoid TActivityBar::StartStop(const tCIDLib::TBoolean bToSet)
{
    tCIDCtrls::TWndHandle hwndUs = hwndSafe();
    DWORD styles = ::GetWindowLong(hwndUs, GWL_STYLE);

    if (bToSet)
    {
        // When starting, enable the marque style and start it
        styles |= PBS_MARQUEE;
        ::SetWindowLong(hwndUs, GWL_STYLE, styles);
        ::SendMessage(hwndSafe(), PBM_SETMARQUEE, 1, 0);
    }
     else
    {
        // When stopping, stop it first, then disable the marque style
        ::SendMessage(hwndSafe(), PBM_SETMARQUEE, 0, 0);
        styles &= ~PBS_MARQUEE;
        ::SetWindowLong(hwndUs, GWL_STYLE, styles);

        // Then we can clear it by setting a zero position
        ::SendMessage(hwndSafe(), PBM_SETPOS, 0, 0);
    }
}




// ---------------------------------------------------------------------------
//  CLASS: TProgressBar
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TProgressBar: Constructors and Destructor
// ---------------------------------------------------------------------------
TProgressBar::TProgressBar() :

    TStdCtrlWnd()
    , m_ePBarStyles(tCIDCtrls::EPBarStyles::None)
{
}

TProgressBar::~TProgressBar()
{
}


// ---------------------------------------------------------------------------
//  TProgressBar: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TProgressBar::InitFromDesc( const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EPBarStyles ePBStyles = tCIDCtrls::EPBarStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild
    );

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Vertical))
        ePBStyles |= tCIDCtrls::EPBarStyles::Vertical;

    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , eStyles
        , ePBStyles
    );
}


tCIDLib::TVoid TProgressBar::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_Vertical);
}



// ---------------------------------------------------------------------------
// TProgressBar: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Just set up the styles and call our parent to create the control
tCIDLib::TVoid
TProgressBar::Create(const  TWindow&                wndParent
                    , const tCIDCtrls::TWndId       widThis
                    , const TArea&                  areaInit
                    , const tCIDCtrls::EWndStyles   eStyles
                    , const tCIDCtrls::EPBarStyles  ePBarStyles)
{
    // Start with any styles we enforce
    tCIDLib::TCard4 c4Styles = PBS_SMOOTH;

    // Add any class specific ones
    if (tCIDLib::bAllBitsOn(ePBarStyles, tCIDCtrls::EPBarStyles::Vertical))
        c4Styles |= PBS_VERTICAL;

    // Store away our class specific styles
    m_ePBarStyles = ePBarStyles;

    // And create the control
    CreateWnd
    (
        wndParent.hwndThis()
        , PROGRESS_CLASS
        , L""
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles(c4Styles)
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );

    // All standard controls must be subclassed
    SetSubclass();
}


// Get the current position
tCIDLib::TCard4 TProgressBar::c4CurValue() const
{
    return ::SendMessage(hwndSafe(), PBM_GETPOS, 0, 0);
}


// Set the min/max range values
tCIDLib::TVoid
TProgressBar::SetRange(const tCIDLib::TCard4 c4Min, const tCIDLib::TCard4 c4Max)
{
    if ((c4Min > 0xFFFF) || (c4Max > 0xFFFF))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_ValueRange
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"progress range")
            , TCardinal(widThis())
        );
    }

    tCIDCtrls::TLParam lRange = (c4Min & 0xFFFF) | ((c4Max & 0xFFFF) << 16);
    if (!::SendMessage(hwndSafe(), PBM_SETRANGE, 0, lRange))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcWnd_SetValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
            , TString(L"progress range")
            , TCardinal(widThis())
        );
    }
}


tCIDLib::TVoid TProgressBar::SetError()
{
    ::SendMessage(hwndSafe(), PBM_SETSTATE, PBST_ERROR, 0);
}

tCIDLib::TVoid TProgressBar::SetNormal()
{
    ::SendMessage(hwndSafe(), PBM_SETSTATE, PBST_NORMAL, 0);
}

tCIDLib::TVoid TProgressBar::SetPaused()
{
    ::SendMessage(hwndSafe(), PBM_SETSTATE, PBST_PAUSED, 0);
}


// Set the current progress value
tCIDLib::TVoid TProgressBar::SetValue(const tCIDLib::TCard4 c4ToSet)
{
    ::SendMessage(hwndSafe(), PBM_SETPOS, c4ToSet, 0);
}

