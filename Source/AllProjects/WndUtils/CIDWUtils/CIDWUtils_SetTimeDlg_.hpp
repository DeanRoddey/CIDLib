//
// FILE NAME: CIDWUtils_SetTimeDlg_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/20/2016
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
//  This is the header for the CIDWUtils_SetTimeDlg.cpp file, which provides a
//  simple dialog for setting the hours, minutes, and seconds of a time stamp, in
//  the 100ns format, which is very tedious to do by hand.
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
//   CLASS: TSetTimeDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TSetTimeDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSetTimeDlg();

        TSetTimeDlg(const TSetTimeDlg&) = delete;

        ~TSetTimeDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSetTimeDlg& operator=(const TSetTimeDlg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRun
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            ,       tCIDLib::TEncodedTime&  enctToEdit
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
        tCIDCtrls::EEvResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_enctEdit
        //      To save the incoming time value till we can load it, and it and
        //      to put back the saved value until we can get it back to the caller's
        //      parameter.
        //
        //  m_strTitle
        //      To save the provided title text until we can get it loaded.
        // -------------------------------------------------------------------
        tCIDLib::TEncodedTime   m_enctEdit;
        TPushButton*            m_pwndCancel;
        TComboBox*              m_pwndHours;
        TComboBox*              m_pwndMins;
        TPushButton*            m_pwndSave;
        TComboBox*              m_pwndSecs;
        TString                 m_strTitle;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TSetTimeDlg,TDlgBox)
};

#pragma CIDLIB_POPPACK


