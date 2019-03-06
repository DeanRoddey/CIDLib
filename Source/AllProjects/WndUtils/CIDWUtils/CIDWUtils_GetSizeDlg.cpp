//
// FILE NAME: CIDWUtils_GetSizeDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/04/2004
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  Implements the size input dialogs.
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
#include    "CIDWUtils_GetSizeDlg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TGetTSizeDlg, TDlgBox)



// ---------------------------------------------------------------------------
//   CLASS: TGetTSizeDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGetTSizeDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TGetTSizeDlg::TGetTSizeDlg() :

    m_bForceAR(kCIDLib::False)
    , m_eRadix(tCIDLib::ERadices::Dec)
    , m_eType(ETypes::Height)
    , m_f8OrgAR(0)
    , m_pwndAcceptButton(nullptr)
    , m_pwndAdjBoth(nullptr)
    , m_pwndAdjHeight(nullptr)
    , m_pwndAdjWidth(nullptr)
    , m_pwndCancelButton(nullptr)
    , m_pwndNewH(nullptr)
    , m_pwndNewW(nullptr)
    , m_pwndRevertButton(nullptr)
{
}

TGetTSizeDlg::~TGetTSizeDlg()
{
}


// ---------------------------------------------------------------------------
//  TGetTSizeDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TGetTSizeDlg::bRunDlg(  const   TWindow&            wndOwner
                        , const TString&            strTitle
                        ,       TSize&              szToFill
                        , const TSize&              szInit
                        , const TSize&              szMin
                        , const TSize&              szMax
                        , const tCIDLib::TBoolean   bForceAR
                        , const tCIDLib::ERadices   eRadix)
{
    // Store the incoming stuff for later use, and the title
    m_bForceAR = bForceAR;
    m_eRadix   = eRadix;
    m_strTitle = strTitle;
    m_szInit   = szInit;
    m_szMax    = szMax;
    m_szMin    = szMin;

    // Set the new size initially to the init size
    m_szNew = m_szInit;

    //
    //  Sanity check the values. If the init is outside the min/max in
    //  either axis, force it back in.
    //
    m_szInit.TakeLarger(m_szMin);
    m_szInit.TakeSmaller(m_szMax);

    // Calc the init aspect ratio based on the initial size
    m_f8OrgAR = tCIDLib::TFloat8(m_szInit.c4Width())
                / tCIDLib::TFloat8(m_szInit.c4Height());

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner
        , facCIDWUtils()
        , kCIDWUtils::ridDlg_GetTSize
    );

    if (c4Res == kCIDWUtils::ridDlg_GetTSize_Accept)
    {
        // Give them back the new size we stored
        szToFill = m_szNew;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


tCIDLib::TBoolean
TGetTSizeDlg::bRunDlg(  const   TWindow&            wndOwner
                        , const TString&            strTitle
                        ,       TSize&              szToFill
                        , const TSize&              szMin
                        , const TSize&              szMax
                        , const tCIDLib::TBoolean   bForceAR
                        , const tCIDLib::ERadices   eRadix)
{
    // Just call the other and provide a default initial value
    return bRunDlg(wndOwner, strTitle, szToFill, szMin, szMin, szMax, bForceAR, eRadix);
}


// ---------------------------------------------------------------------------
//  TGetTSizeDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TGetTSizeDlg::bCreated()
{
    // Call our parent first
    const tCIDLib::TBoolean bRet = TParent::bCreated();

    // Store our pre-typed child widget pointers
    CastChildWnd(*this, kCIDWUtils::ridDlg_GetTSize_Accept, m_pwndAcceptButton);
    CastChildWnd(*this, kCIDWUtils::ridDlg_GetTSize_Cancel, m_pwndCancelButton);
    CastChildWnd(*this, kCIDWUtils::ridDlg_GetTSize_AdjBoth, m_pwndAdjBoth);
    CastChildWnd(*this, kCIDWUtils::ridDlg_GetTSize_AdjHeight, m_pwndAdjHeight);
    CastChildWnd(*this, kCIDWUtils::ridDlg_GetTSize_AdjWidth, m_pwndAdjWidth);
    CastChildWnd(*this, kCIDWUtils::ridDlg_GetTSize_NewHeight, m_pwndNewH);
    CastChildWnd(*this, kCIDWUtils::ridDlg_GetTSize_NewWidth, m_pwndNewW);
    CastChildWnd(*this, kCIDWUtils::ridDlg_GetTSize_Revert, m_pwndRevertButton);

    // And register our button event handlers
    m_pwndAcceptButton->pnothRegisterHandler(this, &TGetTSizeDlg::eClickHandler);
    m_pwndCancelButton->pnothRegisterHandler(this, &TGetTSizeDlg::eClickHandler);
    m_pwndAdjBoth->pnothRegisterHandler(this, &TGetTSizeDlg::eClickHandler);
    m_pwndAdjHeight->pnothRegisterHandler(this, &TGetTSizeDlg::eClickHandler);
    m_pwndAdjWidth->pnothRegisterHandler(this, &TGetTSizeDlg::eClickHandler);
    m_pwndRevertButton->pnothRegisterHandler(this, &TGetTSizeDlg::eClickHandler);

    // Set the initial values into their respective entry fields
    TString strTmp;
    strTmp.AppendFormatted(m_szInit.c4Width(), m_eRadix);
    m_pwndNewW->strWndText(strTmp);
    strTmp.SetFormatted(m_szInit.c4Height(), m_eRadix);
    m_pwndNewH->strWndText(strTmp);

    // Set the title they gave us
    strWndText(m_strTitle);

    //
    //  Register handlers for the entry fields so that we can respond to changes
    //  that the user makes.
    //
    m_pwndNewW->pnothRegisterHandler(this, &TGetTSizeDlg::eEFHandler);
    m_pwndNewH->pnothRegisterHandler(this, &TGetTSizeDlg::eEFHandler);

    // Initially select the height based scaling, forcing an event
    m_pwndAdjHeight->SetCheckedState(kCIDLib::True, kCIDLib::True);

    return kCIDLib::True;
}


// ---------------------------------------------------------------------------
//  TGetTSizeDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses
TGetTSizeDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_GetTSize_Accept)
    {
        //
        //  We store the values in the new size member, so if we end the
        //  dialog they'll be stored away already and an just be copied out
        //  from that member.
        //
        tCIDLib::TCard4 c4Size;
        tCIDLib::TBoolean bHOk = m_pwndNewW->strWndText().bToCard4(c4Size, m_eRadix);
        if (bHOk)
        {
            m_szNew.c4Width(c4Size);
            bHOk = ((m_szNew.c4Width() >= m_szMin.c4Width())
                   && (m_szNew.c4Width() <= m_szMax.c4Width()));
        }

        tCIDLib::TBoolean bVOk = m_pwndNewH->strWndText().bToCard4(c4Size, m_eRadix);
        if (bVOk)
        {
            m_szNew.c4Height(c4Size);
            bVOk = ((m_szNew.c4Height() >= m_szMin.c4Height())
                   && (m_szNew.c4Height() <= m_szMax.c4Height()));
        }

        if (bHOk && bVOk)
        {
            EndDlg(kCIDWUtils::ridDlg_GetTSize_Accept);
        }
         else
        {
            // Tell them the values were bad
            TErrBox msgbTest
            (
                facCIDWUtils().strMsg
                (
                    kWUtilErrs::errcGVal_BadValues
                    , TCardinal(m_szMin.c4Width(), m_eRadix)
                    , TCardinal(m_szMax.c4Width(), m_eRadix)
                    , TCardinal(m_szMin.c4Height(), m_eRadix)
                    , TCardinal(m_szMax.c4Height(), m_eRadix)
                )
            );
            msgbTest.ShowIt(*this);
        }
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_GetTSize_Cancel)
    {
        EndDlg(kCIDWUtils::ridDlg_GetTSize_Cancel);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_GetTSize_AdjBoth)
    {
        m_eType = ETypes::Both;

        m_pwndNewH->SetEnable(kCIDLib::True);
        m_pwndNewW->SetEnable(kCIDLib::True);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_GetTSize_AdjHeight)
    {
        m_eType = ETypes::Height;

        m_pwndNewH->SetEnable(kCIDLib::True);
        if (m_pwndNewW->bHasFocus())
            m_pwndNewH->TakeFocus();
        m_pwndNewW->SetEnable(kCIDLib::False);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_GetTSize_AdjWidth)
    {
        m_eType = ETypes::Width;

        m_pwndNewW->SetEnable(kCIDLib::True);
        if (m_pwndNewH->bHasFocus())
            m_pwndNewW->TakeFocus();
        m_pwndNewH->SetEnable(kCIDLib::False);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_GetTSize_Revert)
    {
        m_szNew = m_szInit;
        TString strTmp;
        strTmp.AppendFormatted(m_szInit.c4Width(), m_eRadix);
        m_pwndNewW->strWndText(strTmp);
        strTmp.SetFormatted(m_szInit.c4Height(), m_eRadix);
        m_pwndNewH->strWndText(strTmp);
    }
    return tCIDCtrls::EEvResponses::Handled;
}


tCIDCtrls::EEvResponses TGetTSizeDlg::eEFHandler(TEFChangeInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_GetTSize_NewHeight)
    {
        //
        //  Get the new height value out and store it if valid. If the type
        //  indicates it, update the width accordingly.
        //
        tCIDLib::TCard4 c4Size;
        tCIDLib::TBoolean bOk = m_pwndNewH->strWndText().bToCard4(c4Size, m_eRadix);
        if (bOk)
        {
            m_szNew.c4Height(c4Size);
            if (m_eType == ETypes::Height)
            {
                m_szNew.c4Width(tCIDLib::TCard4((c4Size * m_f8OrgAR) + 0.5));
                TString strTmp;
                strTmp.SetFormatted(m_szNew.c4Width(), m_eRadix);
                m_pwndNewW->strWndText(strTmp);
            }
        }
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_GetTSize_NewWidth)
    {
        //
        //  Get the new width value out and store it if valid. If the type
        //  indicates it, update the height accordingly.
        //
        tCIDLib::TCard4 c4Size;
        tCIDLib::TBoolean bOk = m_pwndNewW->strWndText().bToCard4(c4Size, m_eRadix);
        if (bOk)
        {
            m_szNew.c4Width(c4Size);
            if (m_eType == ETypes::Width)
            {
                m_szNew.c4Height(tCIDLib::TCard4((c4Size / m_f8OrgAR) + 0.5));
                TString strTmp;
                strTmp.SetFormatted(m_szNew.c4Height(), m_eRadix);
                m_pwndNewH->strWndText(strTmp);
            }
        }
    }
    return tCIDCtrls::EEvResponses::Handled;
}

