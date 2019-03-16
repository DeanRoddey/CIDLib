//
// FILE NAME: CIDCtrls_RadioButton.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/28/1997
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
//  This is the header for the CIDCtrls_RadioButton.cpp file. This file implements the
//  radio button widget class.
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
//   CLASS: TRadioButton
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TRadioButton : public TCtrlWnd
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRadioButton();

        TRadioButton(const TRadioButton&) = delete;

        ~TRadioButton();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TRadioButton& operator=(const TRadioButton&) = delete;


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
        tCIDLib::TBoolean bCheckedState() const;

        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const TString&                strText
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::ERBStyles    eRBStyles = tCIDCtrls::ERBStyles::None
        );

        tCIDLib::TVoid SetCheckedState
        (
            const   tCIDLib::TBoolean       bToSet
            , const tCIDLib::TBoolean       bForceEvent = kCIDLib::False
        );


        // -------------------------------------------------------------------
        //  We provide a templatized method to allow for typesafe registration
        //  of event handlers.
        // -------------------------------------------------------------------
        template <class T> TNotHandlerBase*
        pnothRegisterHandler(        T* const        pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TButtClickInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(this, pTarget);

            // Add a request object to us (the source button)
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, TButtClickInfo>(pTarget, TButtClickInfo::nidClick)
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TButtClickInfo>
            (
                pTarget, TButtClickInfo::nidClick, pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
        }


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

        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TBoolean bExtKey
        (
            const   tCIDCtrls::EExtKeys     eKey
            , const tCIDLib::TBoolean       bAlt
            , const tCIDLib::TBoolean       bCtrl
            , const tCIDLib::TBoolean       bShift
            , const TWindow* const          pwndChild = nullptr
        )   override;

        tCIDLib::TBoolean bMouseMove
        (
            const   TPoint&                 pntAt
            , const tCIDLib::TBoolean       bControlDown
            , const tCIDLib::TBoolean       bShiftDown
        )   override;

        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevTarget
            , const TArea&                  areaUpdate
        )   override;

        tCIDLib::TVoid GettingFocus() override;

        tCIDLib::TVoid LosingFocus() override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CalcAreas();

        tCIDLib::TVoid DrawIndicator
        (
                    TGraphDrawDev&          gdevTarget
        );

        tCIDLib::TVoid ProcessMouse
        (
            const   TPoint&                 pntAt
        );

        tCIDLib::TVoid SendClickNot();

        tCIDLib::TVoid ToggleState();

        tCIDLib::TVoid UncheckOthers();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaIndicator
        //      This is the area of the state indicator, which is to the
        //      left of the text. We pre-calculate it on size change and keep
        //      it around.
        //
        //  m_bInWnd
        //      When we have the mouse captured, we have to unpress the button
        //      if the mouse moves out and repress it if the mouse moves back
        //      in. If we aren't tracking, then we show an emphasis border
        //      when the mouse is inside the window. This flag is used to
        //      track transitions in or out.
        //
        //  m_bState
        //      This is used to store the on/off state of the button.
        //
        //  m_bTracking
        //      This is used to know when we are tracking the mouse. If its True, then we
        //      have the mouse captured.
        //
        //  m_eRBStyles
        //      Our class specific styles
        //
        //  m_rgbText
        //      Our text color
        // -------------------------------------------------------------------
        TArea                       m_areaIndicator;
        tCIDLib::TBoolean           m_bInWnd;
        tCIDLib::TBoolean           m_bState;
        tCIDLib::TBoolean           m_bTracking;
        tCIDCtrls::ERBStyles        m_eRBStyles;
        TRGBClr                     m_rgbText;


        // -------------------------------------------------------------------
        //  s_bLoadedBmp
        //      Used to fault in the bitmap
        //
        //  s_bmpChecked
        //  s_bmpUnchecked
        //  s_bmpPressedChecked
        //  s_bmpPressedUnchecked
        //      We create these on the fly to fit our default font size, one for checked
        //      state and one for unchecked state, plus one for when we are pressed during
        //      tracking.
        //
        //  s_szBitmap
        //      The size of the bitmap we use, which we need quite a bit.
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean    s_bLoadedBmp;
        static TBitmap              s_bmpChecked;
        static TBitmap              s_bmpUnchecked;
        static TBitmap              s_bmpPressedChecked;
        static TBitmap              s_bmpPressedUnchecked;
        static TSize                s_szBitmap;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TRadioButton, TCtrlWnd)
};

#pragma CIDLIB_POPPACK

