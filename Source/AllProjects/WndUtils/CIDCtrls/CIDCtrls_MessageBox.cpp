//
// FILE NAME: CIDCtrls_MsgBox.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/27/2015
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
//  This file implements the message box interface.
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
RTTIDecls(TMsgBox, TObject)
RTTIDecls(TErrBox, TMsgBox)
RTTIDecls(TOkBox, TMsgBox)
RTTIDecls(TRetryCancelBox, TMsgBox)
RTTIDecls(TYesNoBox, TMsgBox)
RTTIDecls(TYesNoCancelBox, TMsgBox)



// ---------------------------------------------------------------------------
//  CLASS: TMsgBox
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TMsgBox: Constructors and Destructor
// ---------------------------------------------------------------------------
TMsgBox::TMsgBox(const TString& strTitle, const TString& strText) :

    m_strText(strText)
    , m_strTitle(strTitle)
{
}

TMsgBox::TMsgBox(const TString& strText) :

    m_strText(strText)
{
}

TMsgBox::~TMsgBox()
{
}


// ---------------------------------------------------------------------------
// TMsgBox: Public, non-virtual methods
// ---------------------------------------------------------------------------

//
//  Show the msg box and wait for it to return. We return the status of the button
//  the user selected.
//
tCIDCtrls::EMsgBoxRets
TMsgBox::eShow( const   TWindow&                wndOwner
                , const tCIDCtrls::EMsgBoxTypes eType
                , const tCIDCtrls::EMsgBoxIcons eIcon
                , const tCIDLib::TBoolean       bAppModal
                , const tCIDLib::TBoolean       bTopMost)
{
    // Translate our enums to internal types
    UINT uType = 0;

    switch(eType)
    {
        case tCIDCtrls::EMsgBoxTypes::AbortRetryIgnore :
            uType |= MB_ABORTRETRYIGNORE;
            break;

        case tCIDCtrls::EMsgBoxTypes::CancelRetryContinue :
            uType |= MB_CANCELTRYCONTINUE;
            break;

        case tCIDCtrls::EMsgBoxTypes::Help :
            uType |= MB_HELP;
            break;

        case tCIDCtrls::EMsgBoxTypes::OK :
            uType |= MB_OK;
            break;

        case tCIDCtrls::EMsgBoxTypes::OKCancel :
            uType |= MB_OKCANCEL;
            break;

        case tCIDCtrls::EMsgBoxTypes::RetryCancel :
            uType |= MB_RETRYCANCEL;
            break;

        case tCIDCtrls::EMsgBoxTypes::YesNo :
            uType |= MB_YESNO;
            break;

        case tCIDCtrls::EMsgBoxTypes::YesNoCancel :
            uType |= MB_YESNOCANCEL;
            break;

        default :
            break;
    };

    switch(eIcon)
    {
        case tCIDCtrls::EMsgBoxIcons::Asterisk :
            uType |= MB_ICONASTERISK;
            break;

        case tCIDCtrls::EMsgBoxIcons::Exclamation :
            uType |= MB_ICONEXCLAMATION;
            break;

        case tCIDCtrls::EMsgBoxIcons::Error :
            uType |= MB_ICONERROR;
            break;

        case tCIDCtrls::EMsgBoxIcons::Hand :
            uType |= MB_ICONHAND;
            break;

        case tCIDCtrls::EMsgBoxIcons::Information :
            uType |= MB_ICONINFORMATION;
            break;

        case tCIDCtrls::EMsgBoxIcons::Question :
            uType |= MB_ICONQUESTION;
            break;

        case tCIDCtrls::EMsgBoxIcons::Stop :
            uType |= MB_ICONSTOP;
            break;

        case tCIDCtrls::EMsgBoxIcons::Warning :
            uType |= MB_ICONWARNING;
            break;

        default :
            break;
    };

    if (bAppModal)
        uType |= MB_APPLMODAL;
    else
        uType |= MB_TASKMODAL;

    if (bTopMost)
        uType |= MB_TOPMOST;

    // If no title set, then use the passed window's text
    if (m_strTitle.bIsEmpty())
        m_strTitle = wndOwner.strWndText();

    // And now let's run it
    int iRes = ::MessageBox
    (
        wndOwner.hwndSafe(), m_strText.pszBuffer(), m_strTitle.pszBuffer(), uType
    );

    tCIDCtrls::EMsgBoxRets eRes;
    switch(iRes)
    {
        case IDABORT :
            eRes = tCIDCtrls::EMsgBoxRets::Abort;
            break;

        case IDCANCEL :
            eRes = tCIDCtrls::EMsgBoxRets::Cancel;
            break;

        case IDCONTINUE :
            eRes = tCIDCtrls::EMsgBoxRets::Continue;
            break;

        case IDIGNORE :
            eRes = tCIDCtrls::EMsgBoxRets::Ignore;
            break;

        case IDNO :
            eRes = tCIDCtrls::EMsgBoxRets::No;
            break;

        case IDOK :
            eRes = tCIDCtrls::EMsgBoxRets::OK;
            break;

        case IDRETRY :
        case IDTRYAGAIN :
            eRes = tCIDCtrls::EMsgBoxRets::Retry;
            break;

        case IDYES :
            eRes = tCIDCtrls::EMsgBoxRets::Yes;
            break;

        default :
            eRes = tCIDCtrls::EMsgBoxRets::Other;
            break;
    };

    return eRes;
}




// ---------------------------------------------------------------------------
//  CLASS: TErrBox
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TErrBox: Constructors and Destructor
// ---------------------------------------------------------------------------
TErrBox::TErrBox(const TString& strTitle, const TString& strText) :

    TMsgBox(strTitle, strText)
{
}

TErrBox::TErrBox(const TString& strText) :

    TMsgBox(strText)
{
}

TErrBox::~TErrBox()
{
}


// ---------------------------------------------------------------------------
// TErrBox: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Just call eShow(), providng most of the values
tCIDLib::TVoid TErrBox::ShowIt(const TWindow& wndOwner)
{
    eShow
    (
        wndOwner, tCIDCtrls::EMsgBoxTypes::OK, tCIDCtrls::EMsgBoxIcons::Error
    );
}



// ---------------------------------------------------------------------------
//  CLASS: TOkBox
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TOkBox: Constructors and Destructor
// ---------------------------------------------------------------------------
TOkBox::TOkBox(const TString& strTitle, const TString& strText) :

    TMsgBox(strTitle, strText)
{
}

TOkBox::TOkBox(const TString& strText) :

    TMsgBox(strText)
{
}

TOkBox::~TOkBox()
{
}


// ---------------------------------------------------------------------------
// TOkBox: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Just call eShow(), providng most of the values
tCIDLib::TVoid TOkBox::ShowIt(const TWindow& wndOwner)
{
    eShow
    (
        wndOwner, tCIDCtrls::EMsgBoxTypes::OK, tCIDCtrls::EMsgBoxIcons::Information
    );
}



// ---------------------------------------------------------------------------
//  CLASS: TRetryCancelBox
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TRetryCancelBox: Constructors and Destructor
// ---------------------------------------------------------------------------
TRetryCancelBox::TRetryCancelBox(const TString& strTitle, const TString& strText) :

    TMsgBox(strTitle, strText)
{
}

TRetryCancelBox::TRetryCancelBox(const TString& strText) :

    TMsgBox(strText)
{
}

TRetryCancelBox::~TRetryCancelBox()
{
}


// ---------------------------------------------------------------------------
// TRetryCancelBox: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EMsgBoxRets TRetryCancelBox::eShowIt(const TWindow& wndOwner)
{
    return eShow
    (
        wndOwner, tCIDCtrls::EMsgBoxTypes::RetryCancel, tCIDCtrls::EMsgBoxIcons::Error
    );
}




// ---------------------------------------------------------------------------
//  CLASS: TYesNoBox
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TYesNoBox: Constructors and Destructor
// ---------------------------------------------------------------------------
TYesNoBox::TYesNoBox(const TString& strTitle, const TString& strText) :

    TMsgBox(strTitle, strText)
{
}

TYesNoBox::TYesNoBox(const TString& strText) :

    TMsgBox(strText)
{
}

TYesNoBox::~TYesNoBox()
{
}


// ---------------------------------------------------------------------------
// TYesNoBox: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Just call eShow(), providng most of the values
tCIDLib::TBoolean TYesNoBox::bShowIt(const TWindow& wndOwner)
{
    const tCIDCtrls::EMsgBoxRets eRes = eShow
    (
        wndOwner, tCIDCtrls::EMsgBoxTypes::YesNo, tCIDCtrls::EMsgBoxIcons::Question
    );

    return (eRes == tCIDCtrls::EMsgBoxRets::Yes);
}




// ---------------------------------------------------------------------------
//  CLASS: TYesNoCancelBox
// PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TYesNoCancelBox: Constructors and Destructor
// ---------------------------------------------------------------------------
TYesNoCancelBox::TYesNoCancelBox(const TString& strTitle, const TString& strText) :

    TMsgBox(strTitle, strText)
{
}

TYesNoCancelBox::TYesNoCancelBox(const TString& strText) :

    TMsgBox(strText)
{
}

TYesNoCancelBox::~TYesNoCancelBox()
{
}


// ---------------------------------------------------------------------------
// TYesNoCancelBox: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EMsgBoxRets TYesNoCancelBox::eShowIt(const TWindow& wndOwner)
{
    return eShow
    (
        wndOwner, tCIDCtrls::EMsgBoxTypes::YesNoCancel, tCIDCtrls::EMsgBoxIcons::Question
    );
}


