//
// FILE NAME: CIDWUtils_FontSelDlg_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 10/27/2015
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
//  This is the header for the CIDWUtils_FontSelDlg.cpp file, which allows the
//  user to select a font and various font attributes. And we have another that only
//  allows for face selection.
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
//   CLASS: TFontSelDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TFontSelDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFontSelDlg();

        TFontSelDlg(const TFontSelDlg&) = delete;

        ~TFontSelDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFontSelDlg& operator=(const TFontSelDlg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRun
        (
            const   TWindow&                wndOwner
            ,       TString&                strFaceName
            ,       tCIDLib::TCard4&        c4Height
            ,       tCIDLib::TBoolean&      bBold
            ,       tCIDLib::TBoolean&      bItalic
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

        tCIDCtrls::EEvResponses eListHandler
        (
                    TListChangeInfo&        wnotEvent
        );

        tCIDCtrls::EEvResponses eSliderHandler
        (
                    TSliderChangeInfo&      wnotEvent
        );

        tCIDLib::TVoid UpdateSample();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bBold
        //  m_bItalic
        //  m_c4Height
        //  m_strFaceName
        //      A place to hold the incoming values till we can get them loaded, and
        //      to hold the changes till we can get them put back.
        //
        //  m_gfontSample
        //      We have to create a new font to set on the sample text window. It
        //      has to be kept around or the handle will be destroyed and the window
        //      will go back toa  default.
        //
        //  m_pwndXXX
        //      Typed pointers to some of the widgets we need to interact with.
        //
        //  m_strTmpFmt
        //      For temporary formatting purposes.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bBold;
        tCIDLib::TBoolean   m_bItalic;
        tCIDLib::TCard4     m_c4Height;
        TString             m_strFaceName;

        TGUIFont            m_gfontSample;

        TCheckBox*          m_pwndBold;
        TPushButton*        m_pwndCancel;
        TStaticText*        m_pwndCurSize;
        TListBox*           m_pwndFontList;
        TCheckBox*          m_pwndItalic;
        TStaticText*        m_pwndSample;
        TPushButton*        m_pwndSave;
        TSlider*            m_pwndSize;
        TCheckBox*          m_pwndUnderline;

        TString             m_strTmpFmt;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TFontSelDlg,TDlgBox)
};


// ---------------------------------------------------------------------------
//   CLASS: TFontFaceSelDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class TFontFaceSelDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFontFaceSelDlg();

        TFontFaceSelDlg(const TFontFaceSelDlg&) = delete;

        ~TFontFaceSelDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFontFaceSelDlg& operator=(const TFontFaceSelDlg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRun
        (
            const   TWindow&                wndOwner
            ,       TString&                strFaceName
            , const tCIDLib::TCard4         c4Height
            , const tCIDLib::TBoolean       bBold
            , const tCIDLib::TBoolean       bItalic
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

        tCIDCtrls::EEvResponses eListHandler
        (
                    TListChangeInfo&        wnotEvent
        );

        tCIDLib::TVoid UpdateSample();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bBold
        //  m_bItalic
        //  m_c4Height
        //      These are just used for preview, so that the caller can see the sample
        //      in a style he's currently using.
        //
        //  m_strFaceName
        //      A place to hold the incoming value till we can get it loaded, and
        //      to hold the changes till we can get it put back.
        //
        //  m_gfontSample
        //      We have to create a new font to set on the sample text window. It
        //      has to be kept around or the handle will be destroyed and the window
        //      will go back to a default.
        //
        //  m_pwndXXX
        //      Typed pointers to some of the widgets we need to interact with.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bBold;
        tCIDLib::TBoolean   m_bItalic;
        tCIDLib::TCard4     m_c4Height;
        TString             m_strFaceName;

        TGUIFont            m_gfontSample;

        TPushButton*        m_pwndCancel;
        TListBox*           m_pwndFontList;
        TStaticText*        m_pwndSample;
        TPushButton*        m_pwndSave;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TFontFaceSelDlg,TDlgBox)
};

#pragma CIDLIB_POPPACK




