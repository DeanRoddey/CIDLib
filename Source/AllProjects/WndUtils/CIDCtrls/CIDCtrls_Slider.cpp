//
// FILE NAME: CIDCtrls_Slider.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/16/2015
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
//  This file implements the check box control.
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
RTTIDecls(TSliderChangeInfo,TCtrlNotify)
AdvRTTIDecls(TSlider, TStdCtrlWnd)



// ---------------------------------------------------------------------------
//   CLASS: TSliderChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSliderChangeInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TSliderChangeInfo::TSliderChangeInfo(const  tCIDLib::TInt4          i4Value
                                    , const tCIDCtrls::ESldrEvents  eEvent
                                    , const TWindow&                wndSrc) :

    TCtrlNotify(wndSrc)
    , m_eEvent(eEvent)
    , m_i4Value(i4Value)
{
}

TSliderChangeInfo::TSliderChangeInfo(const TSliderChangeInfo& wnotToCopy) :

    TCtrlNotify(wnotToCopy)
    , m_eEvent(wnotToCopy.m_eEvent)
    , m_i4Value(wnotToCopy.m_i4Value)
{
}

TSliderChangeInfo::~TSliderChangeInfo()
{
}


// ---------------------------------------------------------------------------
//  TSliderChangeInfo: Public operators
// ---------------------------------------------------------------------------
TSliderChangeInfo&
TSliderChangeInfo::operator=(const TSliderChangeInfo& wnotToAssign)
{
    if (this != &wnotToAssign)
    {
        TParent::operator=(wnotToAssign);
        m_eEvent    = wnotToAssign.m_eEvent;
        m_i4Value   = wnotToAssign.m_i4Value;
    }
    return *this;
}


// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDCtrls::ESldrEvents TSliderChangeInfo::eEvent() const
{
    return m_eEvent;
}


tCIDLib::TInt4 TSliderChangeInfo::i4Value() const
{
    return m_i4Value;
}




// ---------------------------------------------------------------------------
//  CLASS: TSlider
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSlider: Public static data
// ---------------------------------------------------------------------------
const TNotificationId TSlider::nidChange(L"SliderChangeEvent");


// ---------------------------------------------------------------------------
// TSlider: Constructors and Destructor
// ---------------------------------------------------------------------------
TSlider::TSlider() :

    TStdCtrlWnd()
    , m_eSliderStyles(tCIDCtrls::ESldrStyles::None)
    , m_i4Max(100)
    , m_i4Min(0)
{
}

TSlider::~TSlider()
{
}


// ---------------------------------------------------------------------------
// TSlider: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TSlider::InitFromDesc(  const   TWindow&                wndParent
                        , const TDlgItem&               dlgiSrc
                        , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::ESldrStyles eSldrStyles = tCIDCtrls::ESldrStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild
    );
    tCIDLib::TCard4 c4Min = 0;
    tCIDLib::TCard4 c4Max = 100;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Group))
        eStyles |= tCIDCtrls::EWndStyles::Group;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_LiveTrack))
        eSldrStyles |= tCIDCtrls::ESldrStyles::TrackEvs;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Ticks))
        eSldrStyles |= tCIDCtrls::ESldrStyles::Ticks;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_AutoTicks))
        eSldrStyles |= tCIDCtrls::ESldrStyles::AutoTicks;

    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , c4Min
        , c4Max
        , eStyles
        , eSldrStyles
    );
}


tCIDLib::TVoid TSlider::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_Group);
    colToFill.objAdd(kCIDCtrls::strHint_LiveTrack);
    colToFill.objAdd(kCIDCtrls::strHint_Ticks);
}



// ---------------------------------------------------------------------------
// TSlider: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TInt4 TSlider::i4CurValue() const
{
    // Get the raw position
    tCIDLib::TCard4 c4Pos = ::SendMessage(hwndSafe(), TBM_GETPOS, 0, 0);

    // Convert to the virtual range and return
    return (m_i4Min + tCIDLib::TInt4(c4Pos));
}


// Just set up the styles and call our parent to create the control
tCIDLib::TVoid
TSlider::Create(const   TWindow&                wndParent
                , const tCIDCtrls::TWndId       widThis
                , const TArea&                  areaInit
                , const tCIDLib::TInt4          i4MinVal
                , const tCIDLib::TInt4          i4MaxVal
                , const tCIDCtrls::EWndStyles   eStyles
                , const tCIDCtrls::ESldrStyles  eSliderStyles)
{
    // Has to be a child window and add any slider styles we enforce
    tCIDLib::TCard4 c4Styles = 0;

    // Add any control specific styles
    if (tCIDLib::bAllBitsOn(eSliderStyles, tCIDCtrls::ESldrStyles::Ticks))
    {
        if (tCIDLib::bAllBitsOn(eSliderStyles, tCIDCtrls::ESldrStyles::Vertical))
            c4Styles |= TBS_RIGHT;
        else
            c4Styles |= TBS_BOTTOM;

        // If auto-ticks, set that
        if (tCIDLib::bAllBitsOn(eSliderStyles, tCIDCtrls::ESldrStyles::AutoTicks))
            c4Styles |= TBS_AUTOTICKS;
    }
     else
    {
        c4Styles |= TBS_NOTICKS;
    }

    // Set the orientation
    if (tCIDLib::bAllBitsOn(eSliderStyles, tCIDCtrls::ESldrStyles::Vertical))
        c4Styles |= TBS_VERT;

    // Store our class specific styles
    m_eSliderStyles = eSliderStyles;

    // And create the class
    CreateWnd
    (
        wndParent.hwndThis()
        , TRACKBAR_CLASS
        , kCIDLib::pszEmptyZStr
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles(c4Styles)
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );

    // Set the range now
    SetRange(i4MinVal, i4MaxVal);

    // All standard controls must be subclassed
    SetSubclass();
}


// Set a new range. Sometimes it's not known at creation time
tCIDLib::TVoid
TSlider::SetRange(const tCIDLib::TInt4 i4Min, const tCIDLib::TInt4 i4Max)
{
    // Make sure the range is valid
    if (i4Min >= i4Max)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcRange_MinMax
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Range
            , TInteger(i4Min)
            , TInteger(i4Max)
        );
    }

    // Store the vitual range value
    m_i4Min = i4Min;
    m_i4Max = i4Max;

    //
    //  Caculate the range and set that on the control itself. It's inclusive so
    //  we don't subtract 1. Min is always zero internally.
    //
    ::SendMessage
    (
        hwndSafe()
        , TBM_SETRANGE
        , 1
        , MAKELONG(0, tCIDLib::TCard4(i4Max - i4Min))
    );
}


// Set the number of slots between ticks
tCIDLib::TVoid TSlider::SetTickFreq(const tCIDLib::TCard4 c4EveryN)
{
    ::SendMessage(hwndSafe(), TBM_SETTICFREQ, c4EveryN, 0);
}


// Set the slider thumb postion
tCIDLib::TVoid TSlider::SetValue(const tCIDLib::TInt4 i4ToSet)
{
    // Make sure the new value is valid
    if ((i4ToSet < m_i4Min) || (i4ToSet > m_i4Max))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcRange_NewVal2
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Range
            , TInteger(i4ToSet)
            , TInteger(m_i4Min)
            , TInteger(m_i4Max)
        );
    }

    // Adjust it to the internal zero based range
    tCIDLib::TCard4 c4ToSet(i4ToSet - m_i4Min);

    ::SendMessage(hwndSafe(), TBM_SETPOS, 1, c4ToSet);
}



// ---------------------------------------------------------------------------
//  TSlider: Protected, inherited methods
// ---------------------------------------------------------------------------

// We can handle clicked notifications on behalf of all of our derivatives
tCIDLib::TBoolean
TSlider::bNotReflect(       TWindow&                wndTar
                    , const tCIDCtrls::TWndMsg      wmsgMsg
                    , const tCIDCtrls::TWParam      wParam
                    , const tCIDCtrls::TLParam      lParam
                    ,       tCIDCtrls::TMsgResult&  mresResult)
{
    // We expect an H/V scroll message
    if ((wmsgMsg != WM_HSCROLL) && (wmsgMsg != WM_VSCROLL))
        return kCIDLib::False;

    // Set the correct event type
    tCIDLib::TCard4 c4Pos;
    tCIDCtrls::ESldrEvents eEv = tCIDCtrls::ESldrEvents::Count;
    switch(wParam & 0xFFFF)
    {
        case SB_THUMBTRACK :
            // Only do this if they asked for tracking events
            if (tCIDLib::bAllBitsOn(m_eSliderStyles, tCIDCtrls::ESldrStyles::TrackEvs))
            {
                eEv = tCIDCtrls::ESldrEvents::Track;
                c4Pos = wParam >> 16;
            }
            break;

        case SB_THUMBPOSITION :
            eEv = tCIDCtrls::ESldrEvents::EndTrack;
            c4Pos = wParam >> 16;
            break;

        default :
            eEv = tCIDCtrls::ESldrEvents::Change;

            // For these we have to query the position
            c4Pos = ::SendMessage(hwndThis(), TBM_GETPOS, 0, 0);
            break;
    };

    if (eEv != tCIDCtrls::ESldrEvents::Count)
    {
        // Convert the value to the client's virtual range
        const tCIDLib::TInt4 i4Pos = m_i4Min + tCIDLib::TInt4(c4Pos);

        TSliderChangeInfo wnotSend(i4Pos, eEv, *this);
        SendSyncNotify(wnotSend, TSlider::nidChange);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}

