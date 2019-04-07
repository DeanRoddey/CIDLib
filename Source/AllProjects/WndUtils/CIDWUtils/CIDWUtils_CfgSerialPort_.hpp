//
// FILE NAME: CIDWUtils_CfgSerialPortDlg_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 09/01/2003
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
//  This dialog allows the user to configure a serial port's attributes.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//   CLASS: TCfgSerialPortDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TCfgSerialPortDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCfgSerialPortDlg();

        TCfgSerialPortDlg(const TCfgSerialPortDlg&) = delete;

        ~TCfgSerialPortDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCfgSerialPortDlg& operator=(const TCfgSerialPortDlg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRunDlg
        (
            const   TWindow&                wndOwner
            ,       TCommPortCfg&           cpcfgToEdit
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bValidate();

        tCIDCtrls::EEvResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );

        tCIDLib::TVoid LoadData();

        tCIDLib::TVoid SelectValues();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_cpcfgWorking
        //  m_pcpcfgOrg
        //      The orignal configuration and working configuration. We keep a
        //      copy of the original to support the Revert button. We only need to
        //      keep a pointer to the original, which we use to be able to revert
        //      to the original content. We make it constant and only copy back to
        //      it at the end.
        //
        //  m_pwndXXX
        //      Since we have to interact with these controls a lot, we get
        //      convenience, typed, pointers to them. We don't own these, we
        //      just are looking at them a convenient way.
        // -------------------------------------------------------------------
        TCommPortCfg        m_cpcfgWorking;
        const TCommPortCfg* m_pcpcfgOrg;
        TEntryField*        m_pwndBaud;
        TPushButton*        m_pwndCancelButton;
        TCheckBox*          m_pwndCheckParity;
        TCheckBox*          m_pwndCTSOut;
        TCheckBox*          m_pwndDSROut;
        TComboBox*          m_pwndDTR;
        TComboBox*          m_pwndDataBits;
        TComboBox*          m_pwndParity;
        TPushButton*        m_pwndRevertButton;
        TComboBox*          m_pwndRTS;
        TPushButton*        m_pwndSaveButton;
        TComboBox*          m_pwndStopBits;
        TCheckBox*          m_pwndXIn;
        TCheckBox*          m_pwndXOut;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TCfgSerialPortDlg,TDlgBox)
};

#pragma CIDLIB_POPPACK




