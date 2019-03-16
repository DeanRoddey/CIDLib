//
// FILE NAME: CIDCtrl_Spin.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/15/2015
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
//  This file implements some variations on a spin box. We implement a basic up/down
//  control that can be used as a standalone means of bumping something up down.
//  Then we create a composite control that contains an entry field and an up/down
//  and handles linking them together.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

class TSpinEFWnd;

// ---------------------------------------------------------------------------
//   CLASS: TSpinChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TSpinChangeInfo : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSpinChangeInfo() = delete;

        TSpinChangeInfo
        (
            const   tCIDLib::TInt4          i4Position
            , const TWindow&                wndSrc
        );

        TSpinChangeInfo
        (
            const   TSpinChangeInfo&        wnotSrc
        );

        ~TSpinChangeInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSpinChangeInfo& operator=
        (
            const   TSpinChangeInfo&        wnotSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TInt4 i4Value() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_i4Value
        //      The new position of the spin control. This is used for both the
        //      up/down (which is always positive) and the spin variations, which
        //      are signed, so we use an int to cover both.
        // -------------------------------------------------------------------
        tCIDLib::TInt4      m_i4Value;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSpinChangeInfo,TCtrlNotify)
        DefPolyDup(TSpinChangeInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TUpDnCtrl
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TUpDnCtrl : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TNotificationId        nidChange;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TUpDnCtrl();

        ~TUpDnCtrl();


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
        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EUpDnStyles  eUpDnStyles
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDLib::TCard4 c4CurValue() const;

        tCIDLib::TVoid Setup
        (
            const   tCIDLib::TCard4         c4Max
            , const tCIDLib::TCard4         c4Init
        );

        tCIDLib::TVoid SetFocusPass
        (
                    TWindow* const          pwndFocus
        );

        tCIDLib::TVoid SetValue
        (
            const   tCIDLib::TCard4         c4ToSet
        );


        // -------------------------------------------------------------------
        //  We provide a templatized method to allow for typesafe registration
        //  of event handlers.
        // -------------------------------------------------------------------
        template <class T> TNotHandlerBase*
        pnothRegisterHandler(       T* const        pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TSpinChangeInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(this, pTarget);

            // Add a request object to us (the source button)
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, TSpinChangeInfo>(pTarget, TUpDnCtrl::nidChange)
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TSpinChangeInfo>
            (
                pTarget, TUpDnCtrl::nidChange, pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
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
        TUpDnCtrl(const TUpDnCtrl&);
        tCIDLib::TVoid operator=(const TUpDnCtrl&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eUpDnStyles
        //      Our class specific styles
        //
        //  m_pwndFocus
        //      This guy doesn't take focus. If it's used in conjunction wtih some
        //      other control, such as in our spin box below, we need to make sure
        //      it gets focus to that other thing.
        // -------------------------------------------------------------------
        tCIDCtrls::EUpDnStyles  m_eUpDnStyles;
        TWindow*                m_pwndFocus;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TUpDnCtrl, TStdCtrlWnd)
};



// ---------------------------------------------------------------------------
//  CLASS: TSpinBase
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TSpinBase : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TNotificationId        nidChange;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TSpinBase();


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
        tCIDLib::TCard4 c4SpinIndex() const;

        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::ESpinStyles  eSpinStyles
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDLib::TVoid QueryCurText
        (
                    TString&                strToFill
        )   const;

        tCIDLib::TVoid SetSpinIndex
        (
            const   tCIDLib::TCard4         c4ToSet
        );


        // -------------------------------------------------------------------
        //  We provide a templatized method to allow for typesafe registration
        //  of event handlers.
        // -------------------------------------------------------------------
        template <class T> TNotHandlerBase*
        pnothRegisterHandler(       T* const        pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TSpinChangeInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(this, pTarget);

            // Add a request object to us (the source button)
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, TSpinChangeInfo>(pTarget, TSpinBase::nidChange)
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TSpinChangeInfo>
            (
                pTarget, TSpinBase::nidChange, pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
        }


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TSpinEFWnd;


        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TSpinBase();


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TInt4 i4FmtValueAt
        (
            const   tCIDLib::TInt4          i4Index
            ,       TString&                strToFill
        )   = 0;


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
        )  override;

        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TBoolean bNotReflect
        (
                    TWindow&                wndTar
            , const tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::TMsgResult&  mresRet
        )   override;

        tCIDLib::TVoid GettingFocus() override;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdjustValue
        (
            const   tCIDLib::TBoolean       bInc
        );

        tCIDLib::TVoid GoHomeEnd
        (
            const   tCIDLib::TBoolean       bEnd
        );

        tCIDLib::TVoid SetupSpin
        (
            const   tCIDLib::TCard4         c4MaxValue
            , const tCIDLib::TCard4         c4InitValue
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TSpinBase(const TSpinBase&);
        tCIDLib::TVoid operator=(const TSpinBase&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CalcAreas
        (
            const   TArea&                  areaNew
            ,       TArea&                  areaEF
            ,       TArea&                  areaUP
        );

        tCIDCtrls::EEvResponses eSpinHandler
        (
                    TSpinChangeInfo&        wnotThis
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4MaxVal
        //      The derived class tells us the maximum spin value, which we pass
        //      on to the Up/Dn control.
        //
        //  m_eSpinStyles
        //      Our class specific styles, which really just are used to set the
        //      appropriate styles on our underlying controls.
        //
        //  m_pwndEF
        //  m_pwndUD
        //      Our entry field and up/down controls that we create.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4MaxVal;
        tCIDCtrls::ESpinStyles  m_eSpinStyles;
        TSpinEFWnd*             m_pwndEF;
        TUpDnCtrl*              m_pwndUD;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSpinBase, TStdCtrlWnd)
};



// ---------------------------------------------------------------------------
//  CLASS: TNumSpinBox
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TNumSpinBox : public TSpinBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TNumSpinBox();

        ~TNumSpinBox();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4NumRange() const;

        tCIDLib::TInt4 i4CurValue() const;

        tCIDLib::TVoid SetNumRange
        (
            const   tCIDLib::TInt4          i4MinValue
            , const tCIDLib::TInt4          i4MaxValue
            , const tCIDLib::TInt4          i4InitValue
        );

        tCIDLib::TVoid SetValue
        (
            const   tCIDLib::TInt4          i4ToSet
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TInt4 i4FmtValueAt
        (
            const   tCIDLib::TInt4          i4Index
            ,       TString&                strToFill
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TNumSpinBox(const TNumSpinBox&);
        tCIDLib::TVoid operator=(const TNumSpinBox&);



        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_i4Max
        //  m_i4Min
        //      The min max values that we support
        // -------------------------------------------------------------------
        tCIDLib::TInt4      m_i4Min;
        tCIDLib::TInt4      m_i4Max;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TNumSpinBox, TSpinBase)
};


// ---------------------------------------------------------------------------
//  CLASS: TEnumSpinBox
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TEnumSpinBox : public TSpinBase
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TEnumSpinBox();

        ~TEnumSpinBox();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bTextIsValid
        (
            const   tCIDLib::TBoolean       bCaseSensitive = kCIDLib::False
        )   const;

        tCIDLib::TVoid SetEnumList
        (
            const   tCIDLib::TStrCollect&   colToSet
            , const tCIDLib::TCard4         c4InitVal = 0
        );

        tCIDLib::TVoid SetValue
        (
            const   TString&                strToSet
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TInt4 i4FmtValueAt
        (
            const   tCIDLib::TInt4          i4Index
            ,       TString&                strToFill
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TEnumSpinBox(const TEnumSpinBox&);
        tCIDLib::TVoid operator=(const TEnumSpinBox&);



        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colList
        //      The list of values we support
        // -------------------------------------------------------------------
        tCIDLib::TStrList   m_colList;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TEnumSpinBox, TSpinBase)
};


#pragma CIDLIB_POPPACK


