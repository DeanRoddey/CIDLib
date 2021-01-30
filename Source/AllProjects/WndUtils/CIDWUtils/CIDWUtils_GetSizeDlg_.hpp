//
// FILE NAME: CIDWUtils_GetSizeDlg_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/04/2004
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
//  This is the header for the CIDWUtils_GetSizeDlg.cpp file, which provides
//  some dialogs that get various types of size info from a user.
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
//   CLASS: TGetTSizeDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class CIDWUTILSEXP TGetTSizeDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TGetTSizeDlg();

        TGetTSizeDlg(const TGetTSizeDlg&) = delete;
        TGetTSizeDlg(TGetTSizeDlg&&) = delete;

        ~TGetTSizeDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGetTSizeDlg& operator=(const TGetTSizeDlg&) = delete;
        TGetTSizeDlg& operator=(TGetTSizeDlg&&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRunDlg
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            ,       TSize&                  szToFill
            , const TSize&                  szInit
            , const TSize&                  szMin
            , const TSize&                  szMax
            , const tCIDLib::TBoolean       bForceAR
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );

        tCIDLib::TBoolean bRunDlg
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            ,       TSize&                  szToFill
            , const TSize&                  szMin
            , const TSize&                  szMax
            , const tCIDLib::TBoolean       bForceAR
            , const tCIDLib::ERadices       eRadix = tCIDLib::ERadices::Dec
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;


    private :
        // -------------------------------------------------------------------
        //  Private class type
        // -------------------------------------------------------------------
        enum class ETypes
        {
            Both
            , Width
            , Height
        };


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );

        tCIDCtrls::EEvResponses eEFHandler
        (
                    TEFChangeInfo&          wnotEvent
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bForceAR
        //      Indicate whether we should only allow the options that keep
        //      the original aspect ratio.
        //
        //  m_eRadix
        //      The radix that they tell us the value should be in, which we
        //      need to format the initial values in and to interpret the
        //      values we get out.
        //
        //  m_eType
        //      As a convenience, we update this as the user clicks on the
        //      option radio buttons, so that we can quickly figure out how
        //      to react to size changes.
        //
        //  m_f8OrgAR
        //      The original ratio of the width to height, so that we can
        //      adjust either to match if required.
        //
        //  m_pwndXXX
        //      Since we have to interact with these controls a lot, we get
        //      convenience, typed, pointers to them. We don't own these, we
        //      just are looking at them a convenient way.
        //
        //  m_strTitle
        //      The caller provides the title text since this is a reusable
        //      dialog.
        //
        //  m_szXXX
        //      The caller provides us with initial, min, and max values for
        //      the horz/vert values.
        //
        //  m_szNew
        //      We keep this guy updated with the new size as it changes.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bForceAR;
        tCIDLib::ERadices   m_eRadix;
        ETypes              m_eType;
        tCIDLib::TFloat8    m_f8OrgAR;
        TPushButton*        m_pwndAcceptButton;
        TRadioButton*       m_pwndAdjBoth;
        TRadioButton*       m_pwndAdjHeight;
        TRadioButton*       m_pwndAdjWidth;
        TPushButton*        m_pwndCancelButton;
        TEntryField*        m_pwndNewH;
        TEntryField*        m_pwndNewW;
        TPushButton*        m_pwndRevertButton;
        TString             m_strTitle;
        TSize               m_szInit;
        TSize               m_szNew;
        TSize               m_szMax;
        TSize               m_szMin;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TGetTSizeDlg,TDlgBox)
};

#pragma CIDLIB_POPPACK

