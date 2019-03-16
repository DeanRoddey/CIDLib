//
// FILE NAME: CIDCtrl2_StaticMultiText.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/10/2015
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
//  This is a multi line static text control.
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
//  CLASS: TStaticMultiText
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TStaticMultiText : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TStaticMultiText();

        TStaticMultiText(const TStaticMultiText&) = delete;

        ~TStaticMultiText();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TStaticMultiText& operator=(const TStaticMultiText&) = delete;


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


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4GetText
        (
                    TString&                strToFill
        );

        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const TString&                strText
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EMLSStyles   eMLSStyles
            , const tCIDLib::EHJustify      eHJustify = tCIDLib::EHJustify::Left
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDLib::TVoid SetText
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid SetTextColor
        (
            const   TRGBClr&                rgbToSet
        );

        tCIDLib::TVoid SizeToText();

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
        //  Private data members
        //
        //  m_eMLSStyles
        //      Our class specific styles
        //
        //  m_prgbText
        //      They can set a text color on us, to override the default. If not this is
        //      null and we will get the system default.
        // -------------------------------------------------------------------
        tCIDCtrls::EMLSStyles   m_eMLSStyles;
        TRGBClr*                m_prgbText;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStaticMultiText, TStdCtrlWnd)
};

#pragma CIDLIB_POPPACK




