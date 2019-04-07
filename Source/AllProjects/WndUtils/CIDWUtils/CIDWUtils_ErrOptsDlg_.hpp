//
// FILE NAME: CIDWUtils_ErrOptsDlg_.hpp
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
//  This is the header for the CIDWUtils_ErrOptsDlg.cpp file, which provides two general
//  dialogs for letting the user to indicate whether to continue or not when one step of
//  of some multi-step operation fails (such as saving a set of files or some such thing.)
//  We let them cancel the whole thing or ignore or discard the error/changes, and the
//  option of doing the same ignore/discard on subsequent changes.
//
//  We also support a single shot mode, and hide the 'do the same for all' check box.
//
//  One just shows what failed, and another shows a target that failed plus an error
//  message.
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
//   CLASS: TErrOptsDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TErrOptsDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TErrOptsDlg();

        TErrOptsDlg(const TErrOptsDlg&) = delete;

        ~TErrOptsDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TErrOptsDlg& operator=(const TErrOptsDlg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDWUtils::EErrOpts eRun
        (
            const   TWindow&                wndOwner
            ,       tCIDLib::TBoolean&      bDoAll
            , const TString&                strOpName
            , const tCIDLib::TBoolean       bMultiMode
            , const tCIDLib::TBoolean       bDiscardMode
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
        //  m_bDiscardMode
        //      We are in ignore or discard mode, which just makes us change the text of
        //      the ignore button. This allows clients to use this dialog for multiple
        //      scenarios, either save or discard or save and ignore (store with errors.)
        //
        //  m_bDoAll
        //      To store the ignore/discard all check box state until we can get it back
        //      to the caller.
        //
        //  m_bMultiMode
        //      If in multi-mode, we enable the 'ignore all' check box and return whatever
        //      the user indicates. Else, we hie it, and 'do all' is always false.
        //
        //  m_strOpName
        //      To save the incoming operation name until we can get it displayed
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bDiscardMode;
        tCIDLib::TBoolean   m_bDoAll;
        tCIDLib::TBoolean   m_bMultiMode;
        TPushButton*        m_pwndCancel;
        TPushButton*        m_pwndDo;
        TCheckBox*          m_pwndDoAll;
        TStaticMultiText*   m_pwndInstruct;
        TStaticText*        m_pwndOpName;
        TString             m_strOpName;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TErrOptsDlg,TDlgBox)
};



// ---------------------------------------------------------------------------
//   CLASS: TErrOptsDlg2
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TErrOptsDlg2 : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TErrOptsDlg2();

        TErrOptsDlg2(const TErrOptsDlg2&) = delete;

        ~TErrOptsDlg2();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TErrOptsDlg2& operator=(const TErrOptsDlg2&);


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDWUtils::EErrOpts eRun
        (
            const   TWindow&                wndOwner
            ,       tCIDLib::TBoolean&      bDoAll
            , const TString&                strOpName
            , const TString&                strErrMsg
            , const tCIDLib::TBoolean       bMultiMode
            , const tCIDLib::TBoolean       bDiscardMode
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
        //  m_bDiscardMode
        //      We are in ignore or discard mode, which just makes us change the text of
        //      the ignore button. This allows clients to use this dialog for multiple
        //      scenarios, either save or discard or save and ignore (store with errors.)
        //
        //  m_bDoAll
        //      To store the ignore/dicard all check box state until we can get it back to
        //      the caller.
        //
        //  m_bMultiMode
        //      If in multi-mode, we enable the 'ignore all' check box and return whatever
        //      the user indicates. Else, we hide it.
        //
        //  m_pwndXXX
        //      Types pointers to controls we look up so that we can interact with them.
        //
        //  m_strOpName
        //      To save the incoming operation name until we can get it displayed
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bDiscardMode;
        tCIDLib::TBoolean   m_bDoAll;
        tCIDLib::TBoolean   m_bMultiMode;
        TPushButton*        m_pwndCancel;
        TStaticMultiText*   m_pwndErrorMsg;
        TPushButton*        m_pwndDo;
        TCheckBox*          m_pwndDoAll;
        TStaticMultiText*   m_pwndInstruct;
        TStaticText*        m_pwndTarget;
        TString             m_strErrorMsg;
        TString             m_strTarget;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TErrOptsDlg2,TDlgBox)
};

#pragma CIDLIB_POPPACK
