//
// FILE NAME: CIDCtrl2_Slider.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/16/2015
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
//  This is a slider type control.
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
//   CLASS: TSliderChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TSliderChangeInfo : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSliderChangeInfo
        (
            const   tCIDLib::TInt4          i4Value
            , const tCIDCtrls::ESldrEvents  eEvent
            , const TWindow&                wndSrc
        );

        TSliderChangeInfo
        (
            const   TSliderChangeInfo&      wnotToCopy
        );

        ~TSliderChangeInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSliderChangeInfo& operator=
        (
            const   TSliderChangeInfo&      wnotToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::ESldrEvents eEvent() const;

        tCIDLib::TInt4 i4Value() const;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TSliderChangeInfo();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eEvent
        //      The event type, which is either a value change or a drag
        //      stop/start (which also provides the current value.)
        //
        //  m_i4Value
        //      The new value of the slider.
        // -------------------------------------------------------------------
        tCIDCtrls::ESldrEvents  m_eEvent;
        tCIDLib::TInt4          m_i4Value;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSliderChangeInfo,TCtrlNotify)
        DefPolyDup(TSliderChangeInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TSlider
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TSlider : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TNotificationId        nidChange;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSlider();

        ~TSlider();


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
        tCIDLib::TInt4 i4CurValue() const;

        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDLib::TInt4          i4MinVal
            , const tCIDLib::TInt4          i4MaxVal
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::ESldrStyles  eSliderStyles = tCIDCtrls::ESldrStyles::None
        );

        tCIDLib::TVoid SetRange
        (
            const   tCIDLib::TInt4          i4MinVal
            , const tCIDLib::TInt4          i4MaxVal
        );

        tCIDLib::TVoid SetTickFreq
        (
            const   tCIDLib::TCard4         c4EveryN
        );

        tCIDLib::TVoid SetValue
        (
            const   tCIDLib::TInt4          i4ToSet
        );


        // -------------------------------------------------------------------
        //  We provide a templatized method to allow for typesafe registration
        //  of event handlers.
        // -------------------------------------------------------------------
        template <typename T> TNotHandlerBase*
        pnothRegisterHandler(        T* const        pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TSliderChangeInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(this, pTarget);

            // Add a request object to us (the source button)
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, TSliderChangeInfo>(pTarget, TSlider::nidChange)
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TSliderChangeInfo>
            (
                pTarget, TSlider::nidChange, pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
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
        TSlider(const TSlider&);
        tCIDLib::TVoid operator=(const TSlider&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eSliderStyles
        //      Our class specific styles
        //
        //  m_i4Max
        //  m_i4Min
        //      The virtual min/max value. The slider itself works in terms of
        //      unsigned values. So we have to translate between the schemes.
        // -------------------------------------------------------------------
        tCIDCtrls::ESldrStyles  m_eSliderStyles;
        tCIDLib::TInt4          m_i4Max;
        tCIDLib::TInt4          m_i4Min;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSlider, TStdCtrlWnd)
};

#pragma CIDLIB_POPPACK



