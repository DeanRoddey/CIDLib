//
// FILE NAME: CIDCtrl2_PushButton.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/09/2015
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
//  This is a standard push button type control. There is another variation for an image
//  based push button.
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
//  CLASS: TPushButton
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TPushButton : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPushButton();

        TPushButton(const TPushButton&) = delete;

        ~TPushButton();


        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TPushButton& operator=(const TPushButton&) = delete;


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
        tCIDLib::TVoid CreateButton
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const TString&                strText
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EPButtStyles ePButtStyles = tCIDCtrls::EPButtStyles::None
        );

        tCIDCtrls::EPButtStyles ePButtStyles() const;

        tCIDLib::TVoid SetDefButton();


        // -------------------------------------------------------------------
        //  We provide a templatized method to allow for typesafe registration
        //  of event handlers.
        // -------------------------------------------------------------------
        template <typename T> TNotHandlerBase*
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
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SendClickNot
        (
            const   tCIDLib::TBoolean       bState
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ePButtStyles
        //      Our class specific styles
        // -------------------------------------------------------------------
        tCIDCtrls::EPButtStyles m_ePButtStyles;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TPushButton, TStdCtrlWnd)
};


// ---------------------------------------------------------------------------
//  CLASS: TImgPushButton
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TImgPushButton : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TImgPushButton();

        TImgPushButton(const TImgPushButton&) = delete;

        ~TImgPushButton();


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
        //  Public operators
        // -------------------------------------------------------------------
        TImgPushButton& operator=(const TImgPushButton&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CreateButton
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const TString&                strText
            , const tCIDCtrls::EWndStyles   eStyles = tCIDCtrls::EWndStyles::VisChild
            , const tCIDCtrls::EPButtStyles ePButtStyles = tCIDCtrls::EPButtStyles::None
        );

        tCIDLib::TVoid CreateButton
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const TString&                strText
            , const TBitmap&                bmpImage
            , const tCIDCtrls::EWndStyles   eStyles = tCIDCtrls::EWndStyles::VisChild
            , const tCIDCtrls::EPButtStyles ePButtStyles = tCIDCtrls::EPButtStyles::None
        );

        tCIDLib::TVoid CreateButton
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const TString&                strText
            , const TGUIFacility&           facImgSrc
            , const tCIDLib::TResId         ridImg
            , const tCIDCtrls::EWndStyles   eStyles = tCIDCtrls::EWndStyles::VisChild
            , const tCIDCtrls::EPButtStyles ePButtStyles = tCIDCtrls::EPButtStyles::None
        );

        tCIDCtrls::EPButtStyles ePButtStyles() const;

        tCIDLib::TVoid SetDefButton();

        tCIDLib::TVoid SetImage
        (
            const   TBitmap&                bmpToSet
        );


        // -------------------------------------------------------------------
        //  We provide a templatized method to allow for typesafe registration
        //  of event handlers.
        // -------------------------------------------------------------------
        template <typename T> TNotHandlerBase*
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
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SendClickNot
        (
            const   tCIDLib::TBoolean       bState
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bmpImage
        //      We deep copy the image so that we can be sure it stays around.
        //
        //  m_ePButtStyles
        //      Our class specific styles
        // -------------------------------------------------------------------
        TBitmap                 m_bmpImage;
        tCIDCtrls::EPButtStyles m_ePButtStyles;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TImgPushButton, TStdCtrlWnd)
};

#pragma CIDLIB_POPPACK


