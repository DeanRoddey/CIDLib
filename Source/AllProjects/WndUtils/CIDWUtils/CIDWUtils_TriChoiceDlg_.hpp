//
// FILE NAME: CIDWUtils_TriChoiceDlg_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 08/20/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDWUtils_TriChoiceDlg.cpp file, which provides a
//  general dialog for letting the user select from one of three options.
//
//  The third option should be the 'cancel' type option, or whatever is the
//  equivalent for the particular scenario. It's what will be returned if they
//  just hit escape (or hit the third option button of course.)
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
//   CLASS: TTriChoiceDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TTriChoiceDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTriChoiceDlg();

        TTriChoiceDlg(const TTriChoiceDlg&) = default;

        ~TTriChoiceDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTriChoiceDlg& operator=(const TTriChoiceDlg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Run
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const TString&                strInstruct
            , const TString&                strOpt1
            , const TString&                strOpt2
            , const TString&                strOpt3
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
        //  m_bDoAgain
        //      To save the doagain check box state until we can get it returned
        //      tot he caller.
        //
        //  m_strXXX
        //      To save the incoming info until we can get it loaded.
        // -------------------------------------------------------------------
        TString     m_strInstruct;
        TString     m_strOpt1;
        TString     m_strOpt2;
        TString     m_strOpt3;
        TString     m_strTitle;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TTriChoiceDlg,TDlgBox)
};

#pragma CIDLIB_POPPACK



