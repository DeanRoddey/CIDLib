//
// FILE NAME: CIDCtrls_ExceptDlgs.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/13/2015
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
//  This class implements some standard dialogs for displaying errors when there is
//  a caught TError exception involved.
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
//  CLASS: TExceptDlg
// PREFIX: dlg
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TExceptDlg : public TDlgBox
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and destructor
        // -------------------------------------------------------------------
        TExceptDlg
        (
            const   TWindow&                wndOwner
            , const TString&                strTitleText
            , const TString&                strGenMsg
            , const TError&                 errToShow
        );

        TExceptDlg
        (
            const   TWindow&                wndOwner
            , const TString&                strGenMsg
            , const TError&                 errToShow
        );

        ~TExceptDlg();


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
        //  m_errToShow
        //      This is the exception info that we were given in the ctor,
        //      which we are displaying.
        //
        //  m_strGenMsg
        //      This is the generic, high level message of what went wrong.
        //      This is all that is shown to the user at this level. If they
        //      want more info, they can use the detail button.
        //
        //  m_strTitleText
        //      This is the text to put into the title bar. This is application
        //      specific and is provided to us in the ctor.
        // -------------------------------------------------------------------
        TError              m_errToShow;
        TString             m_strGenMsg;
        TString             m_strTitleText;
};


// ---------------------------------------------------------------------------
//  CLASS: TErrorDlg
// PREFIX: dlgb
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TErrorDlg : public TDlgBox
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and destructor
        // -------------------------------------------------------------------
        TErrorDlg
        (
            const   TWindow&                wndOwner
            , const TError&                 errToShow
        );

        ~TErrorDlg();


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
        //  m_errToShow
        //      This is the exception info that we were given in the ctor,
        //      which we are displaying.
        // -------------------------------------------------------------------
        TError      m_errToShow;
};

#pragma CIDLIB_POPPACK


