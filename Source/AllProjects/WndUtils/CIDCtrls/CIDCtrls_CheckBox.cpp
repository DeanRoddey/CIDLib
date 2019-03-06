//
// FILE NAME: CIDCtrls_CheckBox.cpp
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
AdvRTTIDecls(TCheckBox, TStdCtrlWnd)




// ---------------------------------------------------------------------------
//  CLASS: TCheckBox
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TCheckBox: Constructors and Destructor
// ---------------------------------------------------------------------------
TCheckBox::TCheckBox() :

    TStdCtrlWnd()
    , m_eCBStyles(tCIDCtrls::ECBStyles::None)
{
}

TCheckBox::~TCheckBox()
{
}

// ---------------------------------------------------------------------------
// TCheckBox: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TCheckBox::InitFromDesc(const   TWindow&                wndParent
                        , const TDlgItem&               dlgiSrc
                        , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::ECBStyles eCBStyles = tCIDCtrls::ECBStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild
    );

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_NoAutoCheck))
        eCBStyles |= tCIDCtrls::ECBStyles::NoAuto;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Group))
        eStyles |= tCIDCtrls::EWndStyles::Group;

    Create
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , dlgiSrc.strText()
        , eStyles
        , eCBStyles
    );
}


tCIDLib::TVoid TCheckBox::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);
    colToFill.objAdd(kCIDCtrls::strHint_Group);
    colToFill.objAdd(kCIDCtrls::strHint_NoAutoCheck);
}


TSize TCheckBox::szDefault() const
{
    TSize szInit(96, 8);
    TGUIFacility::AdjustDlgUnits(szInit);
    return szInit;
}


// ---------------------------------------------------------------------------
// TCheckBox: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Return our current checked state
tCIDLib::TBoolean TCheckBox::bCheckedState() const
{
    tCIDCtrls::TMsgResult mresState = ::SendMessage(hwndSafe(), BM_GETCHECK, 0, 0);
    return (mresState & BST_CHECKED) != 0;
}


// Just set up the styles and call our parent to create the control
tCIDLib::TVoid
TCheckBox::Create(  const   TWindow&                wndParent
                    , const tCIDCtrls::TWndId       widThis
                    , const TArea&                  areaInit
                    , const TString&                strText
                    , const tCIDCtrls::EWndStyles   eStyles
                    , const tCIDCtrls::ECBStyles    eCBStyles)
{
    // Store our class specific styles
    m_eCBStyles = eCBStyles;

    tCIDLib::TCard4 c4Styles = 0;
    if (tCIDLib::bAllBitsOn(eCBStyles, tCIDCtrls::ECBStyles::NoAuto))
        c4Styles |= BS_CHECKBOX;
    else
        c4Styles |= BS_AUTOCHECKBOX;

    CreateWnd
    (
        wndParent.hwndThis()
        , L"BUTTON"
        , strText.pszBuffer()
        , areaInit
        , eStyles | tCIDCtrls::EWndStyles(c4Styles)
        , tCIDCtrls::EExWndStyles::None
        , widThis
    );

    // All standard controls must be subclassed
    SetSubclass();
}


// Set our checked state
tCIDLib::TVoid
TCheckBox::SetCheckedState( const   tCIDLib::TBoolean   bToSet
                            , const tCIDLib::TBoolean   bForceEvent)
{
    ::SendMessage
    (
        hwndSafe()
        , BM_SETCHECK
        , bToSet ? BST_CHECKED : BST_UNCHECKED
        , 0
    );
    if (bForceEvent)
        SendClickNot(bToSet);
}


// ---------------------------------------------------------------------------
//  TCheckBox: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TCheckBox::bNotReflect(         TWindow&                wndTar
                        , const tCIDCtrls::TWndMsg      wmsgMsg
                        , const tCIDCtrls::TWParam      wParam
                        , const tCIDCtrls::TLParam      lParam
                        ,       tCIDCtrls::TMsgResult&  mresResult)
{
    // We expect a command type message, not notifity
    if (wmsgMsg != WM_COMMAND)
        return kCIDLib::False;

    // Get the type. The source is us, so we don't care about that
    const tCIDLib::TCard4 c4Type = (wParam & 0xFFFF0000) >> 16;

    //
    //  Note that, if this is a 'no auto-check' type, then we are sending him the
    //  current state, which he can change if he wants.
    //
    if (c4Type == BN_CLICKED)
    {
        SendClickNot(bCheckedState());
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TCheckBox: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TCheckBox::SendClickNot(const tCIDLib::TBoolean bState)
{
    SendAsyncNotify(new TButtClickInfo(bState, *this), TButtClickInfo::nidClick);
}
