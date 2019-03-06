//
// FILE NAME: CIDCtrl2_StaticText.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/10/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is a single line static text control.
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
//  CLASS: TStaticText
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TStaticText : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStaticText();

        ~TStaticText();


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
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::ESTextStyles eSTextStyles
            , const tCIDLib::EHJustify      eHJustify = tCIDLib::EHJustify::Left
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDLib::TVoid SetTextColor
        (
            const   TRGBClr&                rgbToSet
        );

        tCIDLib::TVoid UseDefTextColor();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AreaChanged
        (
            const   TArea&                  areaPrev
            , const TArea&                  areaNew
            , const tCIDCtrls::EPosStates   ePosState
            , const tCIDLib::TBoolean       bOrgChanged
            , const tCIDLib::TBoolean       bSizeChanged
            , const tCIDLib::TBoolean       bStateChanged
        )   override;

        tCIDLib::TBoolean bClick
        (
            const   tCIDCtrls::EMouseButts  eButton
            , const tCIDCtrls::EMouseClicks eClickType
            , const TPoint&                 pntAt
            , const tCIDLib::TBoolean       bCtrlShift
            , const tCIDLib::TBoolean       bShift
        )   override;

        tCIDLib::TBoolean bNotReflect
        (
                    TWindow&                wndTar
            , const tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::TMsgResult&  mresRet
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TStaticText(const TStaticText&);
        tCIDLib::TVoid operator=(const TStaticText&);


        // -------------------------------------------------------------------
        //  Private non-virtual methods
        //
        //  m_eSTextStyles
        //      Our class specific styles
        //
        //  m_prgbText
        //      They can set a text color on us, to override the default. If not
        //      this is null and we will get the system default.
        // -------------------------------------------------------------------
        tCIDCtrls::ESTextStyles m_eSTextStyles;
        TRGBClr*                m_prgbText;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStaticText, TStdCtrlWnd)
};

#pragma CIDLIB_POPPACK

