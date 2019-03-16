//
// FILE NAME: CIDCtrls_CheckBox.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/09/2015
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
//  This is a standard checkbox type control.
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
//  CLASS: TCheckBox
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TCheckBox : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCheckBox();

        TCheckBox(const TCheckBox&) = delete;

        ~TCheckBox();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCheckBox& operator=(const TCheckBox&) = delete;


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

        tCIDLib::TCard4 c4FindByText
        (
            const   TString&                strToFind
        )   const;

        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const TString&                strText
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::ECBStyles    eCBStyles = tCIDCtrls::ECBStyles::None
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
        //  m_eCBStyles
        //      Our class specific styles
        // -------------------------------------------------------------------
        tCIDCtrls::ECBStyles    m_eCBStyles;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCheckBox, TStdCtrlWnd)
};

#pragma CIDLIB_POPPACK

