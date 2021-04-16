//
// FILE NAME: CIDWUtils_ShowTextDlg_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/28/2016
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
//  This is the header for the CIDWUtils_ShowTextDlg.cpp file, which provides a
//  simple dialog for displaying a scrollable list of text output. It's not for large
//  amounts of data, but for smaller bits of text output where there's no need for
//  anything bespoke, say 512 lines or less.
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
//   CLASS: TShowTextDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TShowTextDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TShowTextDlg();

        TShowTextDlg(const TShowTextDlg&) = delete;
        TShowTextDlg(TShowTextDlg&&) = delete;

        ~TShowTextDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TShowTextDlg& operator=(const TShowTextDlg&) = delete;
        TShowTextDlg& operator=(TShowTextDlg&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Run
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const TString&                strToShow
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
        //  m_pstrText
        //      We just take a pointer to the caller's text so that we can get to it
        //      when we need to load up upon init.
        //
        //  m_strTitle
        //      To save the provided title text until we can get it loaded.
        // -------------------------------------------------------------------
        const TString*  m_pstrText;
        TString         m_strTitle;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TShowTextDlg,TDlgBox)
};


#pragma CIDLIB_POPPACK




