//
// FILE NAME: CIDCtrl2_ObjView.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/27/2015
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
//  This class implements a standard object oriented type of window, where each object
//  is represented by images with text under them. It uses the tree view internally.
//
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
//   CLASS: TObjVChangeInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TObjVChangeInfo : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TObjVChangeInfo
        (
            const   tCIDCtrls::EObjVEvents  eEvent
            , const tCIDLib::TCard4         c4ObjId
            , const TWindow&                wndSrc
        );

        TObjVChangeInfo
        (
            const   tCIDCtrls::EObjVEvents  eEvent
            , const tCIDLib::TCard4         c4ObjId
            , const tCIDLib::TCard4         c4Action
            , const TWindow&                wndSrc
        );

        TObjVChangeInfo
        (
            const   TObjVChangeInfo&        wnotToCopy
        );

        ~TObjVChangeInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TObjVChangeInfo& operator=
        (
            const   TObjVChangeInfo&        wnotToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Action() const;

        tCIDLib::TCard4 c4ObjId() const;

        tCIDCtrls::EObjVEvents eEvent() const;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TObjVChangeInfo();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Action
        //      If it's a menu select event, this is the menu id.
        //
        //  m_c4ObjId
        //      The unique id for the object that was affected. If not an object
        //      specific event, then it's max card.
        //
        //  m_eEvent
        //      The specific event that occurred.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4Action;
        tCIDLib::TCard4         m_c4ObjId;
        tCIDCtrls::EObjVEvents  m_eEvent;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TObjVChangeInfo,TCtrlNotify)
        DefPolyDup(TObjVChangeInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TObjView
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TObjView : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TNotificationId    nidObjViewChange;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TObjView();

        ~TObjView();


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
        tCIDLib::TVoid AddItem
        (
            const   tCIDLib::TCard4         c4ImgIndex
            , const TString&                strText
            , const tCIDLib::TCard4         c4Id
            , const tCIDLib::TCard4         c4At = kCIDLib::c4MaxCard
        );

        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const TImageList&             imglIcons
            , const tCIDCtrls::EObjVStyles  eObjVStyles = tCIDCtrls::EObjVStyles::None
        );

        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EObjVStyles  eObjVStyles = tCIDCtrls::EObjVStyles::None
        );

        tCIDLib::TVoid SetActionMenu
        (
                    TGUIFacility&           facSrc
            , const tCIDLib::TResId         ridMenu
        );


        // -------------------------------------------------------------------
        //  We provide a templatized method to allow for typesafe registration
        //  of event handlers.
        // -------------------------------------------------------------------
        template <class T> TNotHandlerBase*
        pnothRegisterHandler(       T* const        pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TObjVChangeInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(this, pTarget);

            // Add a request object to us (the source button)
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, TObjVChangeInfo>(pTarget, TObjView::nidObjViewChange)
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TObjVChangeInfo>
            (
                pTarget, TObjView::nidObjViewChange, pfnTarget
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
        TObjView(const TObjView&);
        tCIDLib::TVoid operator=(const TObjView&);


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SendObjVEvent
        (
            const   tCIDCtrls::EObjVEvents  eEvent
            , const tCIDLib::TCard4         c4ObjId
            , const tCIDLib::TCard4         c4Action = kCIDLib::c4MaxCard
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eObjVStyles
        //      The class specific styles set on us.
        //
        //  m_menuAction
        //      A resource based menu can be loaded into this and if it's valid, we
        //      will pop it up when an item is right clicked on. We'll send the
        //      selected item via notification.
        // -------------------------------------------------------------------
        tCIDCtrls::EObjVStyles  m_eObjVStyles;
        TPopupMenu              m_menuAction;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TObjView, TStdCtrlWnd)
};

#pragma CIDLIB_POPPACK


