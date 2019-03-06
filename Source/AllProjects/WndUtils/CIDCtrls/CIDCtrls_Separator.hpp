//
// FILE NAME: CIDCtrls_Separator.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/23/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a couple simple static custom controls that are used for
//  separating sections of controls vertically and/or horizontally.
//
//  We have one that is a horizontal line with text to the left. And we have another
//  that is used as left/right brackets around a chunk of content. On the left side
//  it shows the text label vertically, on the right it's just the label. A style
//  tells it which side it's doing.
//
//  These guys always have the Group style since they are serving the same purpose as
//  a group box normally would.
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
//  CLASS: TSeparatorWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TSeparatorWnd : public TCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSeparatorWnd();

        TSeparatorWnd(const TSeparatorWnd&) = delete;

        ~TSeparatorWnd();


        // -------------------------------------------------------------------
        //  Public oeprators
        // -------------------------------------------------------------------
        TSeparatorWnd& operator=(const TSeparatorWnd&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid InitFromDesc
        (
            const   TWindow&                wndParent
            , const TDlgItem&               dlgiSrc
            , const tCIDCtrls::EWndThemes   eTheme
        )   override;

        tCIDLib::TVoid QueryHints
        (
                    tCIDLib::TStrCollect&   colToFill
        )   const override;

        TSize szDefault() const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const TString&                strText
            , const TRGBClr&                rgbText
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDLib::EHJustify      eHorzJustify
        );

        tCIDLib::TVoid SetText
        (
            const   TString&                strText
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eHorzJustify
        //      We can justify the text relative to the line.
        //
        //  m_rgbLine
        //      The color to use for the text. These typically want to be in
        //      some contrasting color.
        // -------------------------------------------------------------------
        tCIDLib::EHJustify  m_eHorzJustify;
        TRGBClr             m_rgbText;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSeparatorWnd, TCtrlWnd)
};


// ---------------------------------------------------------------------------
//  CLASS: TVSeparatorWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TVSeparatorWnd : public TCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TVSeparatorWnd();

        TVSeparatorWnd(const TVSeparatorWnd&) = delete;

        ~TVSeparatorWnd();


        // -------------------------------------------------------------------
        //  Public oeprators
        // -------------------------------------------------------------------
        TVSeparatorWnd& operator=(const TVSeparatorWnd&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid InitFromDesc
        (
            const   TWindow&                wndParent
            , const TDlgItem&               dlgiSrc
            , const tCIDCtrls::EWndThemes   eTheme
        )   override;

        tCIDLib::TVoid QueryHints
        (
                    tCIDLib::TStrCollect&   colToFill
        )   const override;

        TSize szDefault() const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const TString&                strText
            , const TRGBClr&                rgbText
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EVSepStyles  eSepStyles
        );

        tCIDLib::TVoid SetText
        (
            const   TString&                strText
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eStyles
        //      Our type specific styles
        //
        //  m_rgbLine
        //      The color to use for the text. These typically want to be in
        //      some contrasting color.
        // -------------------------------------------------------------------
        tCIDCtrls::EVSepStyles  m_eVSepStyles;
        TRGBClr                 m_rgbText;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TVSeparatorWnd, TCtrlWnd)
};


#pragma CIDLIB_POPPACK



