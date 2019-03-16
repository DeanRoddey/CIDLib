//
// FILE NAME: CIDMacroDbg_GetParmsDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/25/2003
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
//  This file implements a simple dialog that gets command line parameters
//  from the user, for the macro entry point invocation.
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
#include    "CIDMacroDbg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TGetParmsDlg,TDlgBox)



// ---------------------------------------------------------------------------
//  CLASS: TGetParmsDlg
// PREFIX: dlgb
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//  TGetParmsDlg: Constructors and Destructors
// ---------------------------------------------------------------------------
TGetParmsDlg::TGetParmsDlg() :

    m_c4ExpectedCount(0)
    , m_pwndAcceptButton(nullptr)
    , m_pwndCancelButton(nullptr)
    , m_pwndParms(nullptr)
{
}

TGetParmsDlg::~TGetParmsDlg()
{
    // Call our own destroy method. We don't own the display value
    Destroy();
}


// ---------------------------------------------------------------------------
//  TGetParmsDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TGetParmsDlg::bRunDlg(          TWindow&        wndOwner
                        ,       TString&        strToEdit
                        , const tCIDLib::TCard4 c4ExpectedCount)
{
    // Save the incoming stuff for later use
    m_c4ExpectedCount = c4ExpectedCount;
    m_strToEdit = strToEdit;

    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDMacroDbg(), kCIDMacroDbg::ridDlg_GetParms
    );

    if (c4Res && (c4Res != kCIDLib::c4MaxCard))
    {
        strToEdit = m_strToEdit;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TGetParmsDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TGetParmsDlg::bCreated()
{
    const tCIDLib::TBoolean bRet = TParent::bCreated();

    // Get typed pointers to the widget we access a lot
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_GetParms_Accept, m_pwndAcceptButton);
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_GetParms_Cancel, m_pwndCancelButton);
    CastChildWnd(*this, kCIDMacroDbg::ridDlg_GetParms_Parms, m_pwndParms);

    // Install handlers
    m_pwndAcceptButton->pnothRegisterHandler(this, &TGetParmsDlg::eClickHandler);
    m_pwndCancelButton->pnothRegisterHandler(this, &TGetParmsDlg::eClickHandler);

    // Set any initial incoming text
    m_pwndParms->strWndText(m_strToEdit);

    return bRet;
}


// ---------------------------------------------------------------------------
//  TGetParmsDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDCtrls::EEvResponses
TGetParmsDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    // Just exit on the ok button
    if (wnotEvent.widSource() == kCIDMacroDbg::ridDlg_GetParms_Accept)
    {
        // See if we got the correct number
        TVector<TString> colSrcParms;
        const tCIDLib::TCard4 c4ParmCount = TExternalProcess::c4BreakOutParms
        (
            m_pwndParms->strWndText(), colSrcParms
        );

        if (c4ParmCount != m_c4ExpectedCount)
        {
            TErrBox msgbWarn
            (
                facCIDMacroDbg().strMsg
                (
                    kMDbgMsgs::midMsg_BadParmCount
                    , TCardinal(m_c4ExpectedCount)
                    , TCardinal(c4ParmCount)
                )
            );
            msgbWarn.ShowIt(*this);
        }
         else
        {
            m_strToEdit = m_pwndParms->strWndText();
            EndDlg(1);
        }
    }
     else if (wnotEvent.widSource() == kCIDMacroDbg::ridDlg_GetParms_Cancel)
    {
        EndDlg(0);
    }
    return tCIDCtrls::EEvResponses::Handled;
}


