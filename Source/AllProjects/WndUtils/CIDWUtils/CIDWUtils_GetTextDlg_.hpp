//
// FILE NAME: CIDWUtils_GetTextDlg_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/27/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDWUtils_GetTextDlg.cpp file, which provides a
//  simple dialog for getting a simple, single line text value from the user. There's
//  one that just has a title and entry field, and another that allows for more
//  detailed instructions to be provided.
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
//   CLASS: TGetTextDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TGetTextDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TGetTextDlg();

        TGetTextDlg(const TGetTextDlg&) = delete;

        ~TGetTextDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGetTextDlg& operator=(const TGetTextDlg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRun
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const TString&                strOrgText
            ,       TString&                strNewText
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
        //  m_strText
        //      To save the incoming text till we can get it loaded, and to save
        //      the edited text until we can get it back to the caller's parameter.
        //
        //  m_strTitle
        //      To save the provided title text until we can get it loaded.
        // -------------------------------------------------------------------
        TString             m_strText;
        TString             m_strTitle;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TGetTextDlg,TDlgBox)
};



// ---------------------------------------------------------------------------
//   CLASS: TGetTextDlg2
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TGetTextDlg2 : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TGetTextDlg2();

        TGetTextDlg2(const TGetTextDlg2&) = delete;

        ~TGetTextDlg2();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGetTextDlg2& operator=(const TGetTextDlg2&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRun
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const TString&                strInstruct
            , const TString&                strOrgText
            ,       TString&                strNewText
            , const TString&                strRegEx
            , const tCIDLib::TBoolean       bSecret
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TVoid ModDlgItem
        (
                    TDlgItem&               dlgiToMod
        )   override;


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
        //  m_bSecret
        //      They can ask us to use secret mode on the entry field, so we store it
        //      here until we can set that.
        //
        //  m_pregxFmt
        //      If we get a regular expression string, we create this and set it up
        //      for use later when we need to validate the value.
        //
        //  m_strFmtDescr
        //      If they give us a regular expression, and we reject a value because of
        //      it, we display this to the user to tell them what is expected.
        //
        //  m_strInstruct
        //      To hold the incoming instruction text until we can get it loaded.
        //
        //  m_strText
        //      To save the incoming text till we can get it loaded, and to save
        //      the edited text until we can get it back to the caller's parameter.
        //
        //  m_strTitle
        //      To save the provided title text until we can get it loaded.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bSecret;
        TRegEx*             m_pregxFmt;
        TString             m_strFmtDescr;
        TString             m_strInstruct;
        TString             m_strText;
        TString             m_strTitle;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TGetTextDlg2,TDlgBox)
};

#pragma CIDLIB_POPPACK




