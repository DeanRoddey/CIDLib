//
// FILE NAME: CIDCtrls_Spin.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 15/15/2015
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
//  This file implements some spin box variations.
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
RTTIDecls(TSpinChangeInfo,TCtrlNotify)
AdvRTTIDecls(TUpDnCtrl, TStdCtrlWnd)
RTTIDecls(TSpinBase, TStdCtrlWnd)
AdvRTTIDecls(TNumSpinBox, TSpinBase)
AdvRTTIDecls(TEnumSpinBox, TSpinBase)



// ---------------------------------------------------------------------------
//  We need to create a simple derivative of the entry field so that we can capture
//  up/down arrows and pass them along to the containing spin class. He'll use
//  those to send next/previous commands to the up/down control.
// ---------------------------------------------------------------------------
class TSpinEFWnd : public TEntryField
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSpinEFWnd(TSpinBase* pwndParent) :

            TEntryField()
            , m_pwndParent(pwndParent)
        {
        }

        ~TSpinEFWnd()
        {
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bExtKey
        (
            const   tCIDCtrls::EExtKeys     eKey
            , const tCIDLib::TBoolean       bAlt
            , const tCIDLib::TBoolean       bCtrl
            , const tCIDLib::TBoolean       bShift
            , const TWindow* const          pwndChild
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        TSpinBase*  m_pwndParent;
};


tCIDLib::TBoolean
TSpinEFWnd::bExtKey(const   tCIDCtrls::EExtKeys eKey
                    , const tCIDLib::TBoolean   bAlt
                    , const tCIDLib::TBoolean   bCtrl
                    , const tCIDLib::TBoolean   bShift
                    , const TWindow* const      pwndChild)
{
    if ((eKey == tCIDCtrls::EExtKeys::Down) || (eKey == tCIDCtrls::EExtKeys::Up))
    {
        m_pwndParent->AdjustValue(eKey == tCIDCtrls::EExtKeys::Up);
        return kCIDLib::True;
    }
     else if ((eKey == tCIDCtrls::EExtKeys::Home) || (eKey == tCIDCtrls::EExtKeys::End))
    {
        // If read only, we use these to move to the first/last values
        if (tCIDLib::bAllBitsOn(eEFStyles(), tCIDCtrls::EEFStyles::ReadOnly))
            m_pwndParent->GoHomeEnd(eKey == tCIDCtrls::EExtKeys::End);
    }
     else if ((eKey == tCIDCtrls::EExtKeys::Left) || (eKey == tCIDCtrls::EExtKeys::Right))
    {
        // If read only, eat the left/right arrows since they can't edit
        if (tCIDLib::bAllBitsOn(eEFStyles(), tCIDCtrls::EEFStyles::ReadOnly))
            return kCIDLib::True;
    }
    return kCIDLib::False;
}



// ---------------------------------------------------------------------------
//   CLASS: TSpinChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSpinChangeInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TSpinChangeInfo::TSpinChangeInfo(const  tCIDLib::TInt4  i4Value
                                , const TWindow&        wndSrc) :

    TCtrlNotify(wndSrc)
    , m_i4Value(i4Value)
{
}

TSpinChangeInfo::TSpinChangeInfo(const TSpinChangeInfo& wnotToCopy) :

    TCtrlNotify(wnotToCopy)
    , m_i4Value(wnotToCopy.m_i4Value)
{
}

TSpinChangeInfo::~TSpinChangeInfo()
{
}


// ---------------------------------------------------------------------------
//  TSpinChangeInfo: Public operators
// ---------------------------------------------------------------------------
TSpinChangeInfo&
TSpinChangeInfo::operator=(const TSpinChangeInfo& wnotToAssign)
{
    if (this != &wnotToAssign)
    {
        TParent::operator=(wnotToAssign);
        m_i4Value   = wnotToAssign.m_i4Value;
    }
    return *this;
}


// -------------------------------------------------------------------
//  Public, non-virtual methods
// -------------------------------------------------------------------
tCIDLib::TInt4 TSpinChangeInfo::i4Value() const
{
    return m_i4Value;
}




// ---------------------------------------------------------------------------
//  CLASS: TUpDnCtrl
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TUpDnCtrl: Public static data
// ---------------------------------------------------------------------------
const TNotificationId TUpDnCtrl::nidChange(L"UpDnChangeEvent");


// ---------------------------------------------------------------------------
// TUpDnCtrl: Constructors and Destructor
// ---------------------------------------------------------------------------
TUpDnCtrl::TUpDnCtrl() :

    TStdCtrlWnd()
    , m_eUpDnStyles(tCIDCtrls::EUpDnStyles::None)
    , m_pwndFocus(nullptr)
{
}

TUpDnCtrl::~TUpDnCtrl()
{
}


// ---------------------------------------------------------------------------
// TUpDnCtrl: Public, inherited methods
// ---------------------------------------------------------------------------


tCIDLib::TVoid
TUpDnCtrl::InitFromDesc(const   TWindow&                wndParent
                        , const TDlgItem&               dlgiSrc
                        , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EUpDnStyles eUDStyles = tCIDCtrls::EUpDnStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild
    );
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None;

    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , eStyles
        , eUDStyles
        , eExStyles
    );
}


tCIDLib::TVoid TUpDnCtrl::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);

}


// ---------------------------------------------------------------------------
// TUpDnCtrl: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Just set up the styles and call our parent to create the control
tCIDLib::TVoid
TUpDnCtrl::Create(  const   TWindow&                wndParent
                    , const tCIDCtrls::TWndId       widThis
                    , const TArea&                  areaInit
                    , const tCIDCtrls::EWndStyles   eStyles
                    , const tCIDCtrls::EUpDnStyles  eUpDnStyles
                    , const tCIDCtrls::EExWndStyles eExStyles)
{
    // Store away our class specific styles
    m_eUpDnStyles = eUpDnStyles;

    tCIDLib::TCard4 c4UpDnStyles = 0;
    if (tCIDLib::bAllBitsOn(eUpDnStyles, tCIDCtrls::EUpDnStyles::Wrap))
        c4UpDnStyles |= UDS_WRAP;

    CreateWnd
    (
        wndParent.hwndSafe()
        , UPDOWN_CLASS
        , kCIDLib::pszEmptyZStr
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles(c4UpDnStyles | UDS_ARROWKEYS)
        , eExStyles
        , widThis
    );

    // All standard controls must be subclassed
    SetSubclass();
}


tCIDLib::TCard4 TUpDnCtrl::c4CurValue() const
{
    BOOL bRes = FALSE;
    tCIDCtrls::TMsgResult mresPos = ::SendMessage
    (
        hwndSafe(), UDM_GETPOS32, 0, tCIDCtrls::TLParam(&bRes)
    );

    if (!bRes)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcSpin_GetCurVal
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::CantDo
        );
    }
    return tCIDLib::TCard4(mresPos);
}


tCIDLib::TVoid
TUpDnCtrl::Setup(const  tCIDLib::TCard4  c4Max
                , const tCIDLib::TCard4  c4Init)
{
    if (c4Init > c4Max)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcRange_NewVal
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::InitFailed
            , TCardinal(c4Init)
            , TCardinal(c4Max)
        );
    }

    ::SendMessage(hwndSafe(), UDM_SETRANGE32, 0, c4Max);
    ::SendMessage(hwndThis(), UDM_SETPOS32, 0, c4Init);

    // It's too stupid to send a notification, so do it ourself
    TSpinChangeInfo wnotSend(c4CurValue(), *this);
    SendSyncNotify(wnotSend, TUpDnCtrl::nidChange);
}


tCIDLib::TVoid TUpDnCtrl::SetFocusPass(TWindow* const pwndFocusTar)
{
    m_pwndFocus = pwndFocusTar;
}


tCIDLib::TVoid TUpDnCtrl::SetValue(const tCIDLib::TCard4 c4ToSet)
{
    ::SendMessage(hwndSafe(), UDM_SETPOS32, 0, c4ToSet);

    // It's too stupid to send a notification, so do it ourself
    TSpinChangeInfo wnotSend(c4CurValue(), *this);
    SendSyncNotify(wnotSend, TUpDnCtrl::nidChange);
}


// ---------------------------------------------------------------------------
//  TUpDnCtrl: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TUpDnCtrl::bClick(  const   tCIDCtrls::EMouseButts  eButton
                    , const tCIDCtrls::EMouseClicks eClickType
                    , const TPoint&                 pntAt
                    , const tCIDLib::TBoolean       bCtrlShift
                    , const tCIDLib::TBoolean       bShift)
{
    if (eClickType == tCIDCtrls::EMouseClicks::Down)
    {
        if (m_pwndFocus && m_pwndFocus->bIsValid() && !m_pwndFocus->bHasFocus())
            m_pwndFocus->TakeFocus();
    }

    // Let them all go through to the underlying control
    return kCIDLib::False;
}


tCIDLib::TBoolean
TUpDnCtrl::bNotReflect(         TWindow&                wndTar
                        , const tCIDCtrls::TWndMsg      wmsgMsg
                        , const tCIDCtrls::TWParam      wParam
                        , const tCIDCtrls::TLParam      lParam
                        ,       tCIDCtrls::TMsgResult&  mresResult)
{
    // We expect a notify type message. If not, nothing to do
    if (wmsgMsg != WM_NOTIFY)
        return kCIDLib::False;

    NMHDR* pHdr = (NMHDR*)lParam;
    if (pHdr->code == UDN_DELTAPOS)
    {
        NMUPDOWN* pNot = (NMUPDOWN*)lParam;

        // Get the range and make sure the change would not exceed that
        int iMin, iMax;
        ::SendMessage
        (
            hwndThis(), UDM_GETRANGE32, tCIDCtrls::TWParam(&iMin), tCIDCtrls::TLParam(&iMax)
        );

        int iNewVal = pNot->iPos + pNot->iDelta;
        if ((iNewVal >= iMin) && (iNewVal <= iMax))
        {
            // Send the current position plus the delta, since that's what it's going to become
            TSpinChangeInfo wnotSend(pNot->iPos + pNot->iDelta, *this);
            SendSyncNotify(wnotSend, TUpDnCtrl::nidChange);

            // Return zero to allow the change
            mresResult = 0;
        }
         else
        {
            // Disallow the change, it would be out of range
            mresResult = 1;
        }
        return kCIDLib::True;
    }
    return kCIDLib::False;
}





// ---------------------------------------------------------------------------
//  CLASS: TSpinBase
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSpinBase: Public static data
// ---------------------------------------------------------------------------
const TNotificationId TSpinBase::nidChange(L"SpinChangeEvent");


// ---------------------------------------------------------------------------
// TSpinBase: Constructors and Destructor
// ---------------------------------------------------------------------------
TSpinBase::TSpinBase() :

    TStdCtrlWnd()
    , m_c4MaxVal(100)
    , m_eSpinStyles(tCIDCtrls::ESpinStyles::None)
    , m_pwndEF(nullptr)
    , m_pwndUD(nullptr)
{
}

TSpinBase::~TSpinBase()
{
}


// ---------------------------------------------------------------------------
// TSpinBase: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TSpinBase::InitFromDesc(const   TWindow&                wndParent
                        , const TDlgItem&               dlgiSrc
                        , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::ESpinStyles  eSpinStyles = tCIDCtrls::ESpinStyles::None;
    tCIDCtrls::EWndStyles   eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild
    );
    tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None;

    //
    //  We always do a regular border unless told not to. We don't change the
    //  border type based on the theme.
    //
    if (!dlgiSrc.bHasHint(kCIDCtrls::strHint_NoBorder))
        eStyles |= tCIDCtrls::EWndStyles::Border;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Number))
        eSpinStyles |= tCIDCtrls::ESpinStyles::Numeric;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_ReadOnly))
        eSpinStyles |= tCIDCtrls::ESpinStyles::ReadOnly;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Wrap))
        eSpinStyles |= tCIDCtrls::ESpinStyles::Wrap;

    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , eStyles
        , eSpinStyles
        , eExStyles
    );
}


tCIDLib::TVoid TSpinBase::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_NoBorder);
    colToFill.objAdd(kCIDCtrls::strHint_Number);
    colToFill.objAdd(kCIDCtrls::strHint_ReadOnly);
    colToFill.objAdd(kCIDCtrls::strHint_Wrap);
}


// ---------------------------------------------------------------------------
// TSpinBase: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Return the raw spin index from the up/down control, always 0 based for us.
tCIDLib::TCard4 TSpinBase::c4SpinIndex() const
{
    CIDAssert(m_pwndUD != nullptr, L"The spin base class is not set up yet");
    return m_pwndUD->c4CurValue();
}


// Just set up the styles and call our parent to create the control
tCIDLib::TVoid
TSpinBase::Create(const TWindow&                wndParent
                , const tCIDCtrls::TWndId       widThis
                , const TArea&                  areaInit
                , const tCIDCtrls::EWndStyles   eStyles
                , const tCIDCtrls::ESpinStyles  eSpinStyles
                , const tCIDCtrls::EExWndStyles eExStyles)
{
    // If we haven't registered our window class, then do that
    static const tCIDLib::TCh* pszClass = L"CIDSpinBox";
    static TAtomicFlag atomInitDone;
    if (!atomInitDone)
    {
        // Lock while we do this
        TBaseLock lockInit;
        if (!atomInitDone)
        {
            const TRGBClr rgbBgn = facCIDCtrls().rgbSysClr(tCIDCtrls::ESysColors::Window);
            RegWndClass
            (
                pszClass
                , kCIDLib::False
                , kCIDLib::False
                , 0
                , rgbBgn
                , kCIDLib::False
            );
            atomInitDone.Set();
        }
    }

    // Store these for later use
    m_eSpinStyles = eSpinStyles;

    // We force on clip children and control parent since we are a container
    CreateWnd
    (
        wndParent.hwndSafe()
        , pszClass
        , L""
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles(WS_CLIPCHILDREN)
        , eExStyles | tCIDCtrls::EExWndStyles::ControlParent
        , widThis
    );
}


//
//  Get the current text, which may not be one of the defined values if we are not
//  read only.
//
tCIDLib::TVoid TSpinBase::QueryCurText(TString& strToFill) const
{
    strToFill = m_pwndEF->strWndText();
}


//
//  Set a current spin index, which we just pass to the up/dnc control. That will
//  cause us to get an event back and we'll update in return. This is legal no matter
//  what the derivative since they program should always be able to select an item in
//  the sping box by its index.
//
tCIDLib::TVoid TSpinBase::SetSpinIndex(const tCIDLib::TCard4 c4ToSet)
{
    CIDAssert(m_pwndUD != nullptr, L"The spin base class is not set up yet");

    if (c4ToSet > m_c4MaxVal)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcRange_NewVal
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::InitFailed
            , TCardinal(c4ToSet)
            , TCardinal(m_c4MaxVal)
        );
    }
    m_pwndUD->SetValue(c4ToSet);
}


// ---------------------------------------------------------------------------
//  TSpinBase: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TSpinBase::AreaChanged( const   TArea&                  areaPrev
                        , const TArea&                  areaNew
                        , const tCIDCtrls::EPosStates   ePosState
                        , const tCIDLib::TBoolean       bOrgChanged
                        , const tCIDLib::TBoolean       bSizeChanged
                        , const tCIDLib::TBoolean       bStateChanged)
{
    TParent::AreaChanged(areaPrev, areaNew, ePosState, bOrgChanged, bSizeChanged, bStateChanged);

    // If our size changed, adjust our controls if they have been created
    if (bSizeChanged
    &&  m_pwndEF && m_pwndEF->bIsValid()
    &&  m_pwndUD && m_pwndUD->bIsValid())
    {
        TArea areaEF, areaUD;
        CalcAreas(areaNew, areaEF, areaUD);
        m_pwndUD->SetSizePos(areaUD, kCIDLib::False);
        m_pwndEF->SetSizePos(areaEF, kCIDLib::False);
    }
}


tCIDLib::TBoolean TSpinBase::bCreated()
{
    if (!TParent::bCreated())
        return kCIDLib::False;

    //
    //  Create our entry field and up/down control and position them to our current
    //  size.
    //
    TArea areaEF, areaUD;
    CalcAreas(areaWndSize(), areaEF, areaUD);

    // Create the entry field
    tCIDCtrls::EEFStyles eEFStyles = tCIDCtrls::EEFStyles::None;
    if (tCIDLib::bAllBitsOn(m_eSpinStyles, tCIDCtrls::ESpinStyles::Numeric))
        eEFStyles |= tCIDCtrls::EEFStyles::Number;

    if (tCIDLib::bAllBitsOn(m_eSpinStyles, tCIDCtrls::ESpinStyles::ReadOnly))
        eEFStyles |= tCIDCtrls::EEFStyles::ReadOnly;

    m_pwndEF = new TSpinEFWnd(this);
    m_pwndEF->Create
    (
        *this
        , kCIDCtrls::widFirstCtrl
        , areaEF
        , TString::strEmpty()
        , tCIDCtrls::EWndStyles::VisTabChild
        , eEFStyles
    );

    // Create the up/down control
    tCIDCtrls::EUpDnStyles eUDStyles = tCIDCtrls::EUpDnStyles::None;
    if (tCIDLib::bAllBitsOn(m_eSpinStyles, tCIDCtrls::ESpinStyles::Wrap))
        eUDStyles |= tCIDCtrls::EUpDnStyles::Wrap;

    m_pwndUD = new TUpDnCtrl();
    m_pwndUD->Create
    (
        *this
        , kCIDCtrls::widFirstCtrl + 1
        , areaUD
        , tCIDCtrls::EWndStyles::VisChild
        , eUDStyles
    );

    // It seems to not completely take the area, so force it
    m_pwndUD->SetSizePos(areaUD, kCIDLib::False);

    // Register for events from the up/down control
    m_pwndUD->pnothRegisterHandler(this, &TSpinBase::eSpinHandler);

    // Have the UD control put the focus on the entry field when it's clicked
    m_pwndUD->SetFocusPass(m_pwndEF);

    // Get an initial value displayed
    TString strNewVal;
    i4FmtValueAt(m_pwndUD->c4CurValue(), strNewVal);
    m_pwndEF->strWndText(strNewVal);

    return kCIDLib::True;
}


tCIDLib::TBoolean
TSpinBase::bNotReflect(         TWindow&                wndTar
                        , const tCIDCtrls::TWndMsg      wmsgMsg
                        , const tCIDCtrls::TWParam      wParam
                        , const tCIDCtrls::TLParam      lParam
                        ,       tCIDCtrls::TMsgResult&  mresResult)
{
    return kCIDLib::False;
}


// If read only, make sure all our text is selected when we get focus
tCIDLib::TVoid TSpinBase::GettingFocus()
{
    if (m_pwndEF && tCIDLib::bAllBitsOn(m_eSpinStyles, tCIDCtrls::ESpinStyles::ReadOnly))
        m_pwndEF->SelectAll();
}



// ---------------------------------------------------------------------------
//  TSpinBase: Protected, non-virtual methods
// ---------------------------------------------------------------------------

// Adjust the current value up or down
tCIDLib::TVoid TSpinBase::AdjustValue(const tCIDLib::TBoolean bInc)
{
    CIDAssert(m_pwndUD != nullptr, L"The spin base class is not set up yet");

    // Get the current value, adjust if we can. If so, send it back
    tCIDLib::TCard4 c4Val = m_pwndUD->c4CurValue();

    tCIDLib::TCard4 c4NewVal = c4Val;
    if (bInc)
    {
        if (c4NewVal < m_c4MaxVal)
            c4NewVal++;
    }
     else
    {
        if (c4NewVal)
            c4NewVal--;
    }

    //
    //  If we set a new value, this should make us get a notification and we'll
    //  update.
    //
    if (c4Val != c4NewVal)
        m_pwndUD->SetValue(c4NewVal);
}


tCIDLib::TVoid TSpinBase::GoHomeEnd(const tCIDLib::TBoolean bEnd)
{
    CIDAssert(m_pwndUD != nullptr, L"The spin base class is not set up yet");

    // Get the current value, adjust if we can. If so, send it back
    tCIDLib::TCard4 c4Val = m_pwndUD->c4CurValue();

    tCIDLib::TCard4 c4NewVal = c4Val;
    if (bEnd)
        c4NewVal = m_c4MaxVal;
    else
        c4NewVal = 0;

    //
    //  If we set a new value, this should make us get a notification and we'll
    //  update.
    //
    if (c4Val != c4NewVal)
        m_pwndUD->SetValue(c4NewVal);
}


// Store the max value and set up the Up/Dn control, and set an initial value
tCIDLib::TVoid TSpinBase::SetupSpin(const   tCIDLib::TCard4 c4Max
                                    , const tCIDLib::TCard4 c4Init)
{
    CIDAssert(m_pwndUD != nullptr, L"The spin base class is not set up yet");

    if (c4Init > c4Max)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcRange_NewVal
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::InitFailed
            , TCardinal(c4Init)
            , TCardinal(c4Max)
        );
    }

    // Store the new max, then set up the up/down control
    m_c4MaxVal = c4Max;

    // Set up the up/down control
    m_pwndUD->Setup(c4Max, c4Init);
}


// ---------------------------------------------------------------------------
//  TSpinBase: Private, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Give a new size, we calculate the areas for the two windows.
//
tCIDLib::TVoid TSpinBase::CalcAreas(const TArea& areaNew, TArea& areaEF, TArea& areaUD)
{
    // Get a zero based version of the new area
    TArea areaTar(areaNew);
    areaTar.ZeroOrg();

    if (tCIDLib::bAllBitsOn(eWndStyles(), tCIDCtrls::EWndStyles::Border))
        areaTar.Deflate(1, 1);

    areaUD.Set(0, 0, ::GetSystemMetrics(SM_CXVSCROLL), areaTar.c4Height() + 4);

    // Make sure it doesn't come out wider than us. If so, give it half
    if (areaUD.c4Width() > areaTar.c4Width())
        areaUD.c4Width(areaTar.c4Width() / 2);
    areaUD.RightJustifyIn(areaTar, kCIDLib::True);

    areaEF = areaTar;
    areaEF.i4Right(areaUD.i4X() - 1);
}


//
//  We register with the up/down control, and update ourself based on the new position.
//  We also in turn send out our own notification event. Since all changes will come
//  back to us here, we only need this one place to send it.
//
tCIDCtrls::EEvResponses TSpinBase::eSpinHandler(TSpinChangeInfo& wnotThis)
{
    //
    //  Ask the derived class for the new value and load it to the entry field. It will
    //  also return the appropriate virtual spin value for it's particular view of the
    //  range. We need this for our outgoing notification.
    //
    TString strNewVal;
    const tCIDLib::TInt4 i4VirtVal = i4FmtValueAt(wnotThis.i4Value(), strNewVal);
    m_pwndEF->strWndText(strNewVal);

    // Keep it all selected if read only
    if (tCIDLib::bAllBitsOn(m_eSpinStyles, tCIDCtrls::ESpinStyles::ReadOnly))
        m_pwndEF->SelectAll();

    // In turn send out our own notification.
    TSpinChangeInfo wnotSend(i4VirtVal, *this);
    SendSyncNotify(wnotSend, TSpinBase::nidChange);
    return tCIDCtrls::EEvResponses::Handled;
}



// ---------------------------------------------------------------------------
//  CLASS: TNumSpinBox
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TNumSpinBox: Constructors and Destructor
// ---------------------------------------------------------------------------
TNumSpinBox::TNumSpinBox() :

    m_i4Max(100)
    , m_i4Min(0)
{
}

TNumSpinBox::~TNumSpinBox()
{
}


// ---------------------------------------------------------------------------
//  TNumSpinBox: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TCard4 TNumSpinBox::c4NumRange() const
{
    return (m_i4Max - m_i4Min) + 1;
}


tCIDLib::TInt4 TNumSpinBox::i4CurValue() const
{
    // Get the raw value from the base class
    tCIDLib::TCard4 c4RawVal = c4SpinIndex();

    // And convert to our range and return it
    return m_i4Min + tCIDLib::TInt4(c4RawVal);
}


tCIDLib::TVoid
TNumSpinBox::SetNumRange(const  tCIDLib::TInt4 i4Min
                        , const tCIDLib::TInt4 i4Max
                        , const tCIDLib::TInt4 i4Init)
{
    if (i4Max <= i4Min)
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

    // Make sure the init is wtihin it
    if ((i4Init < i4Min) || (i4Init > i4Max))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcRange_NewVal2
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::InitFailed
            , TInteger(i4Init)
            , TInteger(i4Min)
            , TInteger(i4Max)
        );
    }

    //
    //  Calc what the new zero based max is to set on the up base class. The values
    //  are inclusive, so we don't subtract one.
    //
    const tCIDLib::TCard4 c4NewMax = tCIDLib::TCard4(i4Max - i4Min);

    // Store the new min/max and set up the new range/init value on our parent
    m_i4Max = i4Max;
    m_i4Min = i4Min;
    SetupSpin(c4NewMax, tCIDLib::TCard4(i4Init - m_i4Min));
}


tCIDLib::TVoid TNumSpinBox::SetValue(const tCIDLib::TInt4 i4ToSet)
{
    // Make sure the init is wtihin it
    if ((i4ToSet < m_i4Min) || (i4ToSet > m_i4Max))
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcRange_NewVal2
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::InitFailed
            , TInteger(i4ToSet)
            , TInteger(m_i4Min)
            , TInteger(m_i4Max)
        );
    }

    // Pass it to our parent class, adjusted to his 0 based range
    SetSpinIndex(i4ToSet - m_i4Min);
}


// ---------------------------------------------------------------------------
//  TNumSpinBox: Protected, inherited methods
// ---------------------------------------------------------------------------

//
//  Note that the values we get will always be positive, since the base class works
//  in terms of a zero based range.
//
tCIDLib::TInt4
TNumSpinBox::i4FmtValueAt(const tCIDLib::TInt4 i4Index, TString& strToFill)
{
    // Convert the 0 based index to our range
    const tCIDLib::TInt4 i4Ret = m_i4Min + i4Index;
    strToFill.SetFormatted(i4Ret);
    return i4Ret;
}




// ---------------------------------------------------------------------------
//  CLASS: TEnumSpinBox
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TEnumSpinBox: Constructors and Destructor
// ---------------------------------------------------------------------------
TEnumSpinBox::TEnumSpinBox()
{
    // A single dummy value until they set something up
    m_colList.objAdd(L"");
}

TEnumSpinBox::~TEnumSpinBox()
{
}


// ---------------------------------------------------------------------------
//  TEnumSpinBox: Public, non-virtual methods
// ---------------------------------------------------------------------------

// If not read only, the text might not be a valid value
tCIDLib::TBoolean
TEnumSpinBox::bTextIsValid(const tCIDLib::TBoolean bCaseSensitive) const
{
    // Query the current text
    TString strTest;
    QueryCurText(strTest);

    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    if (bCaseSensitive)
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            if (m_colList[c4Index].bCompare(strTest))
                return kCIDLib::True;
        }
    }
     else
    {
        for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        {
            if (m_colList[c4Index].bCompareI(strTest))
                return kCIDLib::True;
        }
    }
    return kCIDLib::False;
}


tCIDLib::TVoid
TEnumSpinBox::SetEnumList(  const   tCIDLib::TStrCollect&   colToSet
                            , const tCIDLib::TCard4         c4Init)
{
    TColCursor<TString>* pcursSet = colToSet.pcursNew();
    TJanitor<TColCursor<TString>> janCurso(pcursSet);
    m_colList.RemoveAll();
    if (pcursSet->bIsValid())
    {
        for (; pcursSet->bIsValid(); pcursSet->bNext())
            m_colList.objAdd(pcursSet->objRCur());
    }
     else
    {
        m_colList.objAdd(L"");
    }

    // Make sure the init is valid
    const tCIDLib::TCard4 c4NewMax = m_colList.c4ElemCount() - 1;
    if (c4Init > c4NewMax)
    {
        facCIDCtrls().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCtrlsErrs::errcRange_NewVal
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::InitFailed
            , TCardinal(c4Init)
            , TCardinal(c4NewMax)
        );
    }

    SetupSpin(c4NewMax, c4Init);
}


tCIDLib::TVoid TEnumSpinBox::SetValue(const TString& strToSet)
{
    // Search the list for this value
    const tCIDLib::TCard4 c4Count = m_colList.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        if (m_colList[c4Index].bCompareI(strToSet))
        {
            SetSpinIndex(c4Index);
            return;
        }
    }

    // We never found it
    facCIDCtrls().ThrowErr
    (
        CID_FILE
        , CID_LINE
        , kCtrlsErrs::errcRange_EnumVal
        , tCIDLib::ESeverities::Failed
        , tCIDLib::EErrClasses::NotFound
        , strToSet
    );
}



// ---------------------------------------------------------------------------
//  TEnumSpinBox: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TInt4
TEnumSpinBox::i4FmtValueAt(const tCIDLib::TInt4 i4Index, TString& strToFill)
{
    strToFill = m_colList[i4Index];

    //
    //  We just use a zero based range, which the base class does, so we can just
    //  return it as is.
    //
    return i4Index;
}


