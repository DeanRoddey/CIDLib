//
// FILE NAME: CIDWUtils_OverOptsDlg_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/2015
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
//  This is the header for the CIDWUtils_OverOptsDlg.cpp file, which provides a
//  general dialog for getting overwrite/cancel options from the user during file
//  operations.
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
//   CLASS: TOverOptsDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TOverOptsDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TOverOptsDlg();

        TOverOptsDlg(const TOverOptsDlg&) = delete;

        ~TOverOptsDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TOverOptsDlg& operator=(const TOverOptsDlg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDWUtils::EOverOpts eRun
        (
            const   TWindow&                wndOwner
            ,       tCIDLib::TBoolean&      bDoAll
            , const TString&                strTarget
            , const tCIDLib::TBoolean       bMultiMode
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
        //  m_bDoAll
        //      To save the 'do all' check box state until we can get it returned to
        //      the caller.
        //
        //  m_bMultiMode
        //      If in multi-mode, we enable the 'ignore all' check box and return whatever
        //      the user indicates. Else, we hide it, and the do again is always false.
        //
        //  m_strTarget
        //      To save the incoming target until we can get it displayed
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bDoAll;
        tCIDLib::TBoolean   m_bMultiMode;
        TPushButton*        m_pwndCancel;
        TPushButton*        m_pwndDiscard;
        TCheckBox*          m_pwndDoAll;
        TPushButton*        m_pwndOverwrite;
        TStaticText*        m_pwndTarget;
        TString             m_strTarget;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TOverOptsDlg,TDlgBox)
};

#pragma CIDLIB_POPPACK

