//
// FILE NAME: CIDWUtils_24BitPaletteDlg_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/28/2002
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
//  This is the header for the CIDWUtils_24BitPalDlg.cpp file, which presents
//  a 24 bit color palette to the user, to let him/her select a desired
//  color.
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
//   CLASS: T24BitPalDlg
//  PREFIX: dlg
// ---------------------------------------------------------------------------
class T24BitPalDlg : public TDlgBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        T24BitPalDlg();

        T24BitPalDlg(const T24BitPalDlg&) = delete;

        ~T24BitPalDlg();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        T24BitPalDlg& operator=(const T24BitPalDlg&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bRun
        (
            const   TWindow&                wndOwner
            ,       TRGBClr&                rgbSelected
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bClick
        (
            const   tCIDCtrls::EMouseButts  eButton
            , const tCIDCtrls::EMouseClicks eClickType
            , const TPoint&                 pntAt
            , const tCIDLib::TBoolean       bCtrlShift
            , const tCIDLib::TBoolean       bShift
        )   override;

        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TBoolean bMouseMove
        (
            const   TPoint&                 pntAt
            , const tCIDLib::TBoolean       bControlDown
            , const tCIDLib::TBoolean       bShiftDown
        )   override;

        tCIDLib::TBoolean bSetPointer
        (
            const   tCIDCtrls::EWndAreas      eArea
        )   override;

        tCIDLib::TVoid LosingFocus() override;

        tCIDLib::TVoid StaticChildClick
        (
            const   tCIDCtrls::TWndId       widChild
            , const tCIDCtrls::EMouseButts  eButton
            , const tCIDCtrls::EMouseClicks eClickType
        )   override;



    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EEvResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );

        tCIDCtrls::EEvResponses ePalHandler
        (
                    T24BPalChangeInfo&      wnotEvent
        );

        tCIDLib::TVoid UpdateValues
        (
            const   TRGBClr&                rgbToShow
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaSampleBorder
        //      We do a border around the color sample. We don't have it do
        //      its own because we want a little space between the border and
        //      the color sample, since the color might end up being the same
        //      as the border.
        //
        //  m_bCaptureMode
        //      We provide a menu option to allow the user to capture a color
        //      from the screen. When they select it, we capture the mouse
        //      and set this flag. This cause our mouse movement code to set
        //      the pointer to a special shape and, when they click the mouse
        //      we grab the color at that screen location.
        //
        //  m_pwndPal
        //      Some typed pointers to dialog widgets, for convenience. We
        //      don't own them, we just reference them.
        //
        //  m_rgbEdit
        //      This is the color we are editing. Its set to the color we were
        //      give to start with so that the pre-loop handler can use use it
        //      to initialized the palette. And it holds the select value until
        //      we can get it back to the caller's output parm.
        // -------------------------------------------------------------------
        TArea               m_areaSampleBorder;
        tCIDLib::TBoolean   m_bCaptureMode;
        TStaticImg*         m_pwndCapture;
        TPushButton*        m_pwndCopy;
        TCheckBox*          m_pwndHex;
        TStaticText*        m_pwndHSV_Hue;
        TStaticText*        m_pwndHSV_Sat;
        TStaticText*        m_pwndHSV_Val;
        T24BitPalette*      m_pwndPal;
        TStaticText*        m_pwndRGB_Blue;
        TStaticText*        m_pwndRGB_Green;
        TStaticText*        m_pwndRGB_Red;
        TStaticColor*       m_pwndSample;
        TRGBClr             m_rgbEdit;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(T24BitPalDlg,TDlgBox)
};

#pragma CIDLIB_POPPACK



