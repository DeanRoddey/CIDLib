//
// FILE NAME: CIDCtrl2_MultiEdit.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/08/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is a standard list box control.
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
//   CLASS: TMLEChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TMLEChangeInfo : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMLEChangeInfo() = delete;

        TMLEChangeInfo
        (
            const   tCIDCtrls::EMLEEvents   eEvent
            , const TWindow&                wndSrc

        );

        TMLEChangeInfo
        (
            const   TMLEChangeInfo&         wnotToCopy
        );

        ~TMLEChangeInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMLEChangeInfo& operator=
        (
            const   TMLEChangeInfo&         wnotToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDCtrls::EMLEEvents eEvent() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eEvent
        //      Indicates the particular type of event.
        // -------------------------------------------------------------------
        tCIDCtrls::EMLEEvents   m_eEvent;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMLEChangeInfo,TCtrlNotify)
        DefPolyDup(TMLEChangeInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TMultiEdit
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TMultiEdit : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TNotificationId        nidChange;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMultiEdit();

        TMultiEdit(const TMultiEdit&) = delete;

        ~TMultiEdit();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMultiEdit& operator=(const TMultiEdit&) = delete;


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

        tCIDLib::TVoid SetBgnColor
        (
            const   TRGBClr&                rgbToSet
            , const tCIDLib::TBoolean       bOver = kCIDLib::True
            , const tCIDLib::TBoolean       bRedraw = kCIDLib::False
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFindText
        (
            const   TString&                strToFind
            ,       tCIDLib::TCard4&        c4StartOfs
            ,       tCIDLib::TCard4&        c4EndOfs
            , const tCIDLib::TBoolean       bFullMatchOnly
            , const tCIDLib::TBoolean       bFirst
        );

        tCIDLib::TBoolean bQuerySelectRange
        (
                    tCIDLib::TCard4&        c4StartOfs
            ,       tCIDLib::TCard4&        c4EndOfs
        );

        tCIDLib::TCard4 c4QueryCharCount() const;

        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EMLEStyles   eMLEStyles = tCIDCtrls::EMLEStyles::None
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDLib::TVoid GoToLine
        (
            const   tCIDLib::TCard4         c4LineNum
        );

        tCIDLib::TVoid SelectAll();

        tCIDLib::TVoid SelectRange
        (
            const   tCIDLib::TCard4         c4StartOfs
            , const tCIDLib::TCard4         c4EndOfs
        );

        tCIDLib::TVoid SetNewFont
        (
            const   TFontSelAttrs&          fselToSet
        );

        tCIDLib::TVoid SetWantEnter
        (
            const   tCIDLib::TBoolean       bToSet
        );


        // -------------------------------------------------------------------
        //  We provide a templatized method to allow for typesafe registration
        //  of event handlers.
        // -------------------------------------------------------------------
        template <class T> TNotHandlerBase*
        pnothRegisterHandler(       T* const pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TMLEChangeInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(this, pTarget);
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, TMLEChangeInfo>(pTarget, TMultiEdit::nidChange)
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TMLEChangeInfo>
            (
                pTarget, TMultiEdit::nidChange, pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bEatSubClassMsg
        (
            const   tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::TMsgResult&  mresRet
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
        //  m_eMLEStyles
        //      Our class specific styles
        // -------------------------------------------------------------------
        tCIDCtrls::EMLEStyles   m_eMLEStyles;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMultiEdit, TStdCtrlWnd)
};

#pragma CIDLIB_POPPACK



