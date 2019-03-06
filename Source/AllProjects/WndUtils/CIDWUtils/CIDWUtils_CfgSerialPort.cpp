//
// FILE NAME: CIDWUtils_CfgSerialPortDlg.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/01/2003
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  Implements the TCfgSerialPortDlg dialog box, which allows the user to
//  configure the attributes of a serial port.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDWUtils_.hpp"
#include    "CIDWUtils_CfgSerialPort_.hpp"



// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TCfgSerialPortDlg,TDlgBox)


// ---------------------------------------------------------------------------
//   CLASS: TCfgSerialPortDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TCfgSerialPortDlg: Constructors and Destructor
// ---------------------------------------------------------------------------
TCfgSerialPortDlg::TCfgSerialPortDlg() :

    m_pcpcfgOrg(nullptr)
    , m_pwndBaud(nullptr)
    , m_pwndCancelButton(nullptr)
    , m_pwndCheckParity(nullptr)
    , m_pwndCTSOut(nullptr)
    , m_pwndDSROut(nullptr)
    , m_pwndDTR(nullptr)
    , m_pwndDataBits(nullptr)
    , m_pwndParity(nullptr)
    , m_pwndRevertButton(nullptr)
    , m_pwndRTS(nullptr)
    , m_pwndSaveButton(nullptr)
    , m_pwndStopBits(nullptr)
    , m_pwndXIn(nullptr)
    , m_pwndXOut(nullptr)
{
}

TCfgSerialPortDlg::~TCfgSerialPortDlg()
{
}


// ---------------------------------------------------------------------------
//  TCfgSerialPortDlg: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TCfgSerialPortDlg::bRunDlg(const TWindow& wndOwner, TCommPortCfg& cpcfgToEdit)
{
    // Store a pointer to the original and a copy of it for working purposes
    m_pcpcfgOrg = &cpcfgToEdit;
    m_cpcfgWorking = cpcfgToEdit;

    //
    //  We got it, so try to run the dialog. If successful and we get a
    //  positive response, then fill in the caller's parameter with the
    //  info on the selected device.
    //
    const tCIDLib::TCard4 c4Res = c4RunDlg
    (
        wndOwner, facCIDWUtils(), kCIDWUtils::ridDlg_CfgPort
    );

    if (c4Res == kCIDWUtils::ridDlg_CfgPort_Save)
    {
        // They committed so copy the working back to the caller's object
        cpcfgToEdit = m_cpcfgWorking;
        return kCIDLib::True;
    }
    return kCIDLib::False;
}


// ---------------------------------------------------------------------------
//  TCfgSerialPortDlg: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCfgSerialPortDlg::bCreated()
{
    // Call our parent first
    const tCIDLib::TBoolean bRet = TParent::bCreated();

    // Store our pre-typed child widget pointers
    CastChildWnd(*this, kCIDWUtils::ridDlg_CfgPort_Baud, m_pwndBaud);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CfgPort_Cancel, m_pwndCancelButton);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CfgPort_CTSOut, m_pwndCTSOut);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CfgPort_DSROut, m_pwndDSROut);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CfgPort_DataBits, m_pwndDataBits);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CfgPort_DTR, m_pwndDTR);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CfgPort_Parity, m_pwndParity);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CfgPort_CheckParity, m_pwndCheckParity);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CfgPort_Revert, m_pwndRevertButton);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CfgPort_RTS, m_pwndRTS);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CfgPort_Save, m_pwndSaveButton);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CfgPort_StopBits, m_pwndStopBits);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CfgPort_XIn, m_pwndXIn);
    CastChildWnd(*this, kCIDWUtils::ridDlg_CfgPort_XOut, m_pwndXOut);

    // Load up the combo boxes with the available values
    LoadData();

    //
    //  And register the needed handlers. Most we don't care about until it's
    //  time to gather up the data.
    //
    m_pwndCancelButton->pnothRegisterHandler(this, &TCfgSerialPortDlg::eClickHandler);
    m_pwndRevertButton->pnothRegisterHandler(this, &TCfgSerialPortDlg::eClickHandler);
    m_pwndSaveButton->pnothRegisterHandler(this, &TCfgSerialPortDlg::eClickHandler);

    // Select the incoming values
    SelectValues();

    return bRet;
}


// ---------------------------------------------------------------------------
//  TCfgSerialPortDlg: Private, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TCfgSerialPortDlg::bValidate()
{
    // Make sure we can convert the baud to a number
    tCIDLib::TCard4 c4Baud;
    if (!m_pwndBaud->strWndText().bToCard4(c4Baud) || (c4Baud < 300) || (c4Baud > 921600))
    {
        TErrBox msgbErr
        (
            facCIDWUtils().strMsg(kWUtilErrs::errcConn_BadBaud, m_pwndBaud->strWndText())
        );
        msgbErr.ShowIt(*this);
        return kCIDLib::False;
    }

    //
    //  Everything has to be correct, so let's gather up the info and put
    //  it into the working object.
    //
    m_cpcfgWorking.c4Baud(c4Baud);
    m_cpcfgWorking.eDataBits
    (
        tCIDComm::EDataBits
        (
            m_pwndDataBits->c4CurItem()
            + tCIDLib::c4EnumOrd(tCIDComm::EDataBits::Min)
        )
    );
    m_cpcfgWorking.eDTR(tCIDComm::EPortDTR(m_pwndDTR->c4CurItem()));
    m_cpcfgWorking.eParity(tCIDComm::EParities(m_pwndParity->c4CurItem()));
    m_cpcfgWorking.eRTS(tCIDComm::EPortRTS(m_pwndRTS->c4CurItem()));
    m_cpcfgWorking.eStopBits(tCIDComm::EStopBits(m_pwndStopBits->c4CurItem()));

    // The rest go into the flags field
    tCIDComm::EFlags eFlags = tCIDComm::EFlags::None;

    if (m_pwndCheckParity->bCheckedState())
        eFlags = tCIDLib::eOREnumBits(eFlags, tCIDComm::EFlags::ParityChecking);
    if (m_pwndCTSOut->bCheckedState())
        eFlags = tCIDLib::eOREnumBits(eFlags, tCIDComm::EFlags::CTSOut);
    if (m_pwndDSROut->bCheckedState())
        eFlags = tCIDLib::eOREnumBits(eFlags, tCIDComm::EFlags::DSROut);
    if (m_pwndXOut->bCheckedState())
        eFlags = tCIDLib::eOREnumBits(eFlags, tCIDComm::EFlags::XOut);
    if (m_pwndXIn->bCheckedState())
        eFlags = tCIDLib::eOREnumBits(eFlags, tCIDComm::EFlags::XIn);
    m_cpcfgWorking.eFlags(eFlags);

    return kCIDLib::True;
}


tCIDCtrls::EEvResponses TCfgSerialPortDlg::eClickHandler(TButtClickInfo& wnotEvent)
{
    if (wnotEvent.widSource() == kCIDWUtils::ridDlg_CfgPort_Save)
    {
        // Validate the values and if ok, exit
        if (bValidate())
            EndDlg(kCIDWUtils::ridDlg_CfgPort_Save);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_CfgPort_Cancel)
    {
        EndDlg(kCIDWUtils::ridDlg_CfgPort_Cancel);
    }
     else if (wnotEvent.widSource() == kCIDWUtils::ridDlg_CfgPort_Revert)
    {
        m_cpcfgWorking = *m_pcpcfgOrg;
        SelectValues();
    }
    return tCIDCtrls::EEvResponses::Handled;
}


// Load up all of the combo boxes with the choices that are available
tCIDLib::TVoid TCfgSerialPortDlg::LoadData()
{
    XlatEnumToCombo(tCIDComm, EDataBits, *m_pwndDataBits);
    XlatEnumToCombo(tCIDComm, EStopBits, *m_pwndStopBits);
    XlatEnumToCombo(tCIDComm, EParities, *m_pwndParity);
    XlatEnumToCombo(tCIDComm, EPortDTR, *m_pwndDTR);
    XlatEnumToCombo(tCIDComm, EPortRTS, *m_pwndRTS);
}



tCIDLib::TVoid TCfgSerialPortDlg::SelectValues()
{
    // Do all the boolean flags
    m_pwndCheckParity->SetCheckedState
    (
        tCIDLib::bAllBitsOn(m_cpcfgWorking.eFlags(), tCIDComm::EFlags::ParityChecking)
    );

    m_pwndCTSOut->SetCheckedState
    (
        tCIDLib::bAllBitsOn(m_cpcfgWorking.eFlags(), tCIDComm::EFlags::CTSOut)
    );

    m_pwndDSROut->SetCheckedState
    (
        tCIDLib::bAllBitsOn(m_cpcfgWorking.eFlags(), tCIDComm::EFlags::DSROut)
    );

    m_pwndXOut->SetCheckedState
    (
        tCIDLib::bAllBitsOn(m_cpcfgWorking.eFlags(), tCIDComm::EFlags::XOut)
    );

    m_pwndXIn->SetCheckedState
    (
        tCIDLib::bAllBitsOn(m_cpcfgWorking.eFlags(), tCIDComm::EFlags::XIn)
    );

    // And do the combo boxes
    m_pwndDataBits->SelectByIndex
    (
        tCIDLib::c4EnumOrd(m_cpcfgWorking.eDataBits())
        - tCIDLib::c4EnumOrd(tCIDComm::EDataBits::Min)
    );

    m_pwndDTR->SelectByIndex(tCIDLib::c4EnumOrd(m_cpcfgWorking.eDTR()));
    m_pwndParity->SelectByIndex(tCIDLib::c4EnumOrd(m_cpcfgWorking.eParity()));
    m_pwndRTS->SelectByIndex(tCIDLib::c4EnumOrd(m_cpcfgWorking.eRTS()));
    m_pwndStopBits->SelectByIndex(tCIDLib::c4EnumOrd(m_cpcfgWorking.eStopBits()));

    // And format the baud as text and set the entry field
    TString strBaud(TCardinal(m_cpcfgWorking.c4Baud()));
    m_pwndBaud->strWndText(strBaud);
}



