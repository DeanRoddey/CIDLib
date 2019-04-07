//
// FILE NAME: CIDWUtils_FindTextDlg_.hpp
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
//  This is the header for the CIDWUtils_FindTextDlg.cpp file, which provides a
//  general dialog that lets the user enter a value to find, and provides a recall
//  list and lets them provide a couple of common search options.
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
//   CLASS: TFindTextDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TFindTextDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFindTextDlg();

        TFindTextDlg(const TFindTextDlg&) = delete;

        ~TFindTextDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFindTextDlg& operator=(const TFindTextDlg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRun
        (
            const   TWindow&                wndOwner
            , const tCIDLib::TBoolean       bReplaceMode
            ,       TString&                strToFind
            ,       TString&                strReplaceWith
            ,       tCIDLib::TStrList&      colFindRecall
            ,       tCIDLib::TStrList&      colRepRecall
            ,       tCIDLib::TBoolean&      bFullMatchOnly
            ,       tCIDLib::TBoolean&      bSelectionOnly
            , const tCIDWUtils::EFindFlags  eFlags
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
        //  m_bFullMatchOnly
        //  m_bSelectionOnly
        //      To save these flags on the way in till we can set them, and to give back the
        //      currently selected values for return.
        //
        //  m_bRepMode
        //      The caller tells us if we are in search mode or replace mode.
        //
        //  m_eFlags
        //      As set of flags passed in to control some aspects of how we operate.
        //
        //  m_pcolFindList
        //  m_pcolRepList
        //      Pointers to the caller's recall slist so we can update if needed and so we
        //      can load in the previous values.
        //
        //  m_strRepWith
        //  m_strToFind
        //      To save the incoming text until we can load it up, and to return the new
        //      values (if it changed.)
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bFullMatchOnly;
        tCIDLib::TBoolean       m_bRepMode;
        tCIDLib::TBoolean       m_bSelectionOnly;
        tCIDWUtils::EFindFlags  m_eFlags;
        tCIDLib::TStrList*      m_pcolFindRecall;
        tCIDLib::TStrList*      m_pcolRepRecall;
        TPushButton*            m_pwndClose;
        TPushButton*            m_pwndFind;
        TCheckBox*              m_pwndFullMatch;
        TComboBox*              m_pwndFindList;
        TComboBox*              m_pwndRepList;
        TCheckBox*              m_pwndSelection;
        TString                 m_strRepWith;
        TString                 m_strToFind;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TFindTextDlg,TDlgBox)
};

#pragma CIDLIB_POPPACK

