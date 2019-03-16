//
// FILE NAME: CIDWUtils_SetTimeDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/20/2016
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
//  Implements the TSetTimeDlg dialog.
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
#include    "CIDWUtils_SetTimeDlg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TSetTimeDlg, TDlgBox)



// ---------------------------------------------------------------------------
//   CLASS: TSetTimeDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TSetTimeDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TSetTimeDlg::TSetTimeDlg() :

    m_enctEdit(0)
    , m_pwndHours(nullptr)
    , m_pwndMins(nullptr)
    , m_pwndSecs(nullptr)
{
}

TSetTimeDlg::~TSetTimeDlg()
{
}


// ---------------------------------------------------------------------------
//  TSetTimeDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TSetTimeDlg::bRun(  const   TWindow&                wndOwner
                    , const TString&                strTitle
                    ,       tCIDLib::TEncodedTime&  enctEdit)
{
    // Store the incoming info till we can put it to use
    m_enctEdit = enctEdit;
    m_strTitle  = strTitle;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_SetTime
    );

    if (c4Res == kCIDWUtils::ridDlg_SetTime_Save)
    {
        enctEdit = m_enctEdit;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TSetTimeDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TSetTimeDlg::bCreated()
{
    // Now call our parent first and return his return
    const tCIDLib::TBoolean bRes = TParent::bCreated();

    // Load up the title we were given
    strWndText(m_strTitle);

    // Register a handler for the buttons
    pwndChildAs<TPushButton>(kCIDWUtils::ridDlg_SetTime_Save)->pnothRegisterHandler
    (
        this, &TSetTimeDlg::eClickHandler
    );
    pwndChildAs<TPushButton>(kCIDWUtils::ridDlg_SetTime_Cancel)->pnothRegisterHandler
    (
        this, &TSetTimeDlg::eClickHandler
    );

    // Get the combos and load them up
    CastChildWnd(*this, kCIDWUtils::ridDlg_SetTime_Hours, m_pwndHours);
    CastChildWnd(*this, kCIDWUtils::ridDlg_SetTime_Mins, m_pwndMins);
    CastChildWnd(*this, kCIDWUtils::ridDlg_SetTime_Secs, m_pwndSecs);

    TString strVal;
    for (tCIDLib::TCard4 c4Val = 0; c4Val < 72; c4Val++)
    {
        strVal.SetFormatted(c4Val);
        m_pwndHours->c4AddItem(strVal);
    }

    for (tCIDLib::TCard4 c4Val = 0; c4Val < 60; c4Val++)
    {
        strVal.SetFormatted(c4Val);
        m_pwndMins->c4AddItem(strVal);
        m_pwndSecs->c4AddItem(strVal);
    }

    // Break out the values from the time and select them in the combos
    tCIDLib::TEncodedTime enctLoad = m_enctEdit;

    tCIDLib::TCard4 c4Hours = tCIDLib::TCard4(enctLoad / kCIDLib::enctOneHour);
    enctLoad -= (c4Hours * kCIDLib::enctOneHour);
    tCIDLib::TCard4 c4Mins = tCIDLib::TCard4(enctLoad / kCIDLib::enctOneMinute);
    enctLoad -= (c4Mins * kCIDLib::enctOneMinute);
    tCIDLib::TCard4 c4Secs = tCIDLib::TCard4(enctLoad / kCIDLib::enctOneSecond);

    if (c4Hours > 71)
        c4Hours = 71;
    if (c4Mins > 59)
        c4Mins = 59;
    if (c4Secs > 59)
        c4Secs = 59;

    m_pwndHours->SelectByIndex(c4Hours);
    m_pwndMins->SelectByIndex(c4Mins);
    m_pwndSecs->SelectByIndex(c4Secs);

    return bRes;
}


// ---------------------------------------------------------------------------
//  TSetTimeDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses TSetTimeDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    // If saving, set the time back up again wtih the selected values
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_SetTime_Save)
    {
        const tCIDLib::TCard4 c4Hours = m_pwndHours->c4CurItem();
        const tCIDLib::TCard4 c4Mins = m_pwndMins->c4CurItem();
        const tCIDLib::TCard4 c4Secs = m_pwndSecs->c4CurItem();

        m_enctEdit = tCIDLib::TEncodedTime
        (
            (c4Hours * kCIDLib::enctOneHour)
            + (c4Mins * kCIDLib::enctOneMinute)
            + (c4Secs * kCIDLib::enctOneSecond)
        );
    }

    // End the dialog with the id of the button clicked
    EndDlg(wnotEvent.widSource());

    return tCIDCtrls::EEvResponses::Handled;
}



