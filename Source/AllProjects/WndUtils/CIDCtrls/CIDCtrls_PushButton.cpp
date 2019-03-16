//
// FILE NAME: CIDCtrls_PushButton.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/09/2015
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
//  This file implements the
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
AdvRTTIDecls(TPushButton, TStdCtrlWnd)
AdvRTTIDecls(TImgPushButton, TStdCtrlWnd)



// ---------------------------------------------------------------------------
//  CLASS: TPushButton
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TPushButton: Constructors and Destructor
// ---------------------------------------------------------------------------
TPushButton::TPushButton() :

    TStdCtrlWnd()
    , m_ePButtStyles(tCIDCtrls::EPButtStyles::None)
{
}

TPushButton::~TPushButton()
{
}


// ---------------------------------------------------------------------------
// TPushButton: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TPushButton::InitFromDesc(  const   TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EPButtStyles ePBStyles = tCIDCtrls::EPButtStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild
    );

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Group))
        eStyles |= tCIDCtrls::EWndStyles::Group;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_DefButton))
        ePBStyles |= tCIDCtrls::EPButtStyles::Default;

    CreateButton
    (
        wndParent
        , dlgiSrc.ridChild()
        , dlgiSrc.areaPos()
        , TString::strEmpty()
        , eStyles
        , ePBStyles
    );

    // Set the text now the font is set
    if (!dlgiSrc.strText().bIsEmpty())
        strWndText(dlgiSrc.strText());
}


tCIDLib::TVoid TPushButton::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);

    colToFill.objAdd(kCIDCtrls::strHint_DefButton);
    colToFill.objAdd(kCIDCtrls::strHint_Group);
}


TSize TPushButton::szDefault() const
{
    TSize szInit(52, 14);
    TGUIFacility::AdjustDlgUnits(szInit);
    return szInit;
}



// ---------------------------------------------------------------------------
// TPushButton: Public, non-virtual methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TPushButton::CreateButton(  const   TWindow&                wndParent
                            , const tCIDCtrls::TWndId       widThis
                            , const TArea&                  areaInit
                            , const TString&                strText
                            , const tCIDCtrls::EWndStyles   eStyles
                            , const tCIDCtrls::EPButtStyles ePButtStyles)
{
    // Has to be a child window and add any button styles we enforce
    tCIDLib::TCard4 c4Styles = BS_PUSHBUTTON | BS_CENTER;

    // Add any class specific ones
    if (tCIDLib::bAllBitsOn(ePButtStyles, tCIDCtrls::EPButtStyles::Default))
        c4Styles |= BS_DEFPUSHBUTTON;

    // Store away our class specific styles
    m_ePButtStyles = ePButtStyles;

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


tCIDLib::TVoid TPushButton::SetDefButton()
{
    ::SendMessage(hwndSafe(), BM_SETSTYLE, BS_DEFPUSHBUTTON, 1);
}



// ---------------------------------------------------------------------------
//  TPushButton: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TPushButton::bNotReflect(       TWindow&                wndTar
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

    // Pushbuttons don't have a state, so we always just send a true state
    if (c4Type == BN_CLICKED)
    {
        SendClickNot(kCIDLib::True);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TPushButton: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TPushButton::SendClickNot(const tCIDLib::TBoolean bState)
{
    SendAsyncNotify(new TButtClickInfo(bState, *this), TButtClickInfo::nidClick);
}




// ---------------------------------------------------------------------------
//  CLASS: TImgPushButton
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TImgPushButton: Constructors and Destructor
// ---------------------------------------------------------------------------
TImgPushButton::TImgPushButton() :

    TStdCtrlWnd()
    , m_ePButtStyles(tCIDCtrls::EPButtStyles::None)
{
}

TImgPushButton::~TImgPushButton()
{
}


// ---------------------------------------------------------------------------
// TImgPushButton: Public, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TVoid
TImgPushButton::InitFromDesc(const  TWindow&                wndParent
                            , const TDlgItem&               dlgiSrc
                            , const tCIDCtrls::EWndThemes   eTheme)
{
    tCIDCtrls::EPButtStyles ePBStyles = tCIDCtrls::EPButtStyles::None;
    tCIDCtrls::EWndStyles eStyles = tCIDCtrls::EWndStyles
    (
        tCIDCtrls::EWndStyles::VisTabChild
    );

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_Group))
        eStyles |= tCIDCtrls::EWndStyles::Group;

    if (dlgiSrc.bHasHint(kCIDCtrls::strHint_DefButton))
        ePBStyles |= tCIDCtrls::EPButtStyles::Default;

    //
    //  If we got the image as an app image or resource, then we can set it now. If we
    //  didn't we call the non-image version first.
    //
    const TString& strImg = dlgiSrc.strImage();
    if (strImg.bIsEmpty())
    {
        CreateButton
        (
            wndParent
            , dlgiSrc.ridChild()
            , dlgiSrc.areaPos()
            , dlgiSrc.strText()
            , eStyles
            , ePBStyles
        );
    }
     else
    {
        // See if it's an app image
        TString strToLoad;
        const tCIDCtrls::EAppImgSz eSz = TDlgItem::eParseAppImage(strImg, strToLoad);

        TBitmap bmpImg;
        if (strToLoad.bIsEmpty())
        {
            TString strFacName;
            tCIDLib::TResId ridToLoad;
            tCIDImage::EImgTypes eType;
            if (TDlgItem::bParseResImage(strImg, ridToLoad, strFacName, eType))
            {
                // Look up the facility
                TFacility* pfacSrc = TFacility::pfacFromName(strFacName);
                if (pfacSrc)
                    bmpImg = facCIDCtrls().bmpLoadResourceImg(ridToLoad, kCIDLib::True);
            }
        }
         else
        {
            bmpImg = facCIDCtrls().bmpLoadAppImg(strToLoad, eSz);
        }

        CreateButton
        (
            wndParent
            , dlgiSrc.ridChild()
            , dlgiSrc.areaPos()
            , TString::strEmpty()
            , bmpImg
            , eStyles
            , ePBStyles
        );
    }

    // Set the text now the font is set
    if (!dlgiSrc.strText().bIsEmpty())
        strWndText(dlgiSrc.strText());
}


tCIDLib::TVoid TImgPushButton::QueryHints(tCIDLib::TStrCollect& colToFill) const
{
    TParent::QueryHints(colToFill);

    colToFill.objAdd(kCIDCtrls::strHint_DefButton);
    colToFill.objAdd(kCIDCtrls::strHint_Group);
}


TSize TImgPushButton::szDefault() const
{
    TSize szInit(52, 14);
    TGUIFacility::AdjustDlgUnits(szInit);
    return szInit;
}



// ---------------------------------------------------------------------------
// TImgPushButton: Public, non-virtual methods
// ---------------------------------------------------------------------------

// This one leaves the setting of the image until later
tCIDLib::TVoid
TImgPushButton::CreateButton(const  TWindow&                wndParent
                            , const tCIDCtrls::TWndId       widThis
                            , const TArea&                  areaInit
                            , const TString&                strText
                            , const tCIDCtrls::EWndStyles   eStyles
                            , const tCIDCtrls::EPButtStyles ePButtStyles)
{
    // Has to be a child window and add any button styles we require
    tCIDLib::TCard4 c4Styles = BS_PUSHBUTTON | BS_CENTER | BS_BITMAP;

    // Add any class specific ones
    if (tCIDLib::bAllBitsOn(ePButtStyles, tCIDCtrls::EPButtStyles::Default))
        c4Styles |= BS_DEFPUSHBUTTON;

    // Store away our class specific styles
    m_ePButtStyles = ePButtStyles;

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


// This one takes an image up front
tCIDLib::TVoid
TImgPushButton::CreateButton(const  TWindow&                wndParent
                            , const tCIDCtrls::TWndId       widThis
                            , const TArea&                  areaInit
                            , const TString&                strText
                            , const TBitmap&                bmpImage
                            , const tCIDCtrls::EWndStyles   eStyles
                            , const tCIDCtrls::EPButtStyles ePButtStyles)
{
    // Has to be a child window and add any button styles we require
    tCIDLib::TCard4 c4Styles = BS_PUSHBUTTON | BS_CENTER | BS_BITMAP;

    // Add any class specific ones
    if (tCIDLib::bAllBitsOn(ePButtStyles, tCIDCtrls::EPButtStyles::Default))
        c4Styles |= BS_DEFPUSHBUTTON;

    // Store away our class specific styles
    m_ePButtStyles = ePButtStyles;

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

    // Set the incoming image
    SetImage(bmpImage);
}


// This one takes an image up front, but does the loading for the caller
tCIDLib::TVoid
TImgPushButton::CreateButton(const  TWindow&                wndParent
                            , const tCIDCtrls::TWndId       widThis
                            , const TArea&                  areaInit
                            , const TString&                strText
                            , const TGUIFacility&           facImgSrc
                            , const tCIDLib::TResId         ridImg
                            , const tCIDCtrls::EWndStyles   eStyles
                            , const tCIDCtrls::EPButtStyles ePButtStyles)
{
    // Has to be a child window and add any button styles we require
    tCIDLib::TCard4 c4Styles = BS_PUSHBUTTON | BS_CENTER | BS_BITMAP;

    // Add any class specific ones
    if (tCIDLib::bAllBitsOn(ePButtStyles, tCIDCtrls::EPButtStyles::Default))
        c4Styles |= BS_DEFPUSHBUTTON;

    // Store away our class specific styles
    m_ePButtStyles = ePButtStyles;

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

    // Set the incoming image and set it
    TBitmap bmpImage(facImgSrc, ridImg);
    SetImage(bmpImage);
}



tCIDLib::TVoid TImgPushButton::SetDefButton()
{
    ::SendMessage(hwndSafe(), BM_SETSTYLE, BS_DEFPUSHBUTTON, 1);
}


// Set an image on this button
tCIDLib::TVoid TImgPushButton::SetImage(const TBitmap& bmpToSet)
{
    // Deep copy the image so we keep the handle around
    m_bmpImage = bmpToSet;
    m_bmpImage.DeepCopy();

    // Set the image
    ::SendMessage
    (
        hwndThis()
        , BM_SETIMAGE
        , tCIDCtrls::TWParam(IMAGE_BITMAP)
        , tCIDCtrls::TLParam(m_bmpImage.hbmpThis())
    );
}



// ---------------------------------------------------------------------------
//  TPushButton: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean
TImgPushButton::bNotReflect(        TWindow&                wndTar
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

    // Pushbuttons don't have a state, so we always just send a true state
    if (c4Type == BN_CLICKED)
    {
        SendClickNot(kCIDLib::True);
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TPushButton: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TImgPushButton::SendClickNot(const tCIDLib::TBoolean bState)
{
    SendAsyncNotify(new TButtClickInfo(bState, *this), TButtClickInfo::nidClick);
}
