//
// FILE NAME: CIDMacroDbg_GetParmsDlg_.hpp
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
//  This is the header for the CIDMacroDbg_GetParmsDlg.cpp file, which
//  implements a small dialog to let the user edit macro parameters.
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
//  CLASS: TGetParmsDlg
// PREFIX: dlg
// ---------------------------------------------------------------------------
class TGetParmsDlg : public TDlgBox
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and destructor
        // -------------------------------------------------------------------
        TGetParmsDlg();

        TGetParmsDlg(const TGetParmsDlg&) = delete;

        ~TGetParmsDlg();


        // -------------------------------------------------------------------
        //  Public oeprators
        // -------------------------------------------------------------------
        TGetParmsDlg& operator=(const TGetParmsDlg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRunDlg
        (
                    TWindow&                wndOwner
            ,       TString&                strToEdit
            , const tCIDLib::TCard4         c4ExpectedCount
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4ExpectedCount
        //      The number of parameters that the target macro is going to
        //      expect, so we require that many before we will accept the
        //      input.
        //
        //  m_pwndXXX
        //      Some typed pointers to some of our widgets that we need to
        //      interact with a lot. We don't own them.
        //
        //  m_strToEdit
        //      The incoming text is stored here so we can use it as the initial
        //      content. And it's used to return the entered content to get it
        //      back to the caller's parm.
        // -------------------------------------------------------------------
        tCIDLib::TCard4 m_c4ExpectedCount;
        TPushButton*    m_pwndAcceptButton;
        TPushButton*    m_pwndCancelButton;
        TEntryField*    m_pwndParms;
        TString         m_strToEdit;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGetParmsDlg,TDlgBox)
};

#pragma CIDLIB_POPPACK


