//
// FILE NAME: CIDCtrl2_TreeView.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/12/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is a tree view control. In order to make this control remotely convenient
//  to use and not expose Windows specific stuff, we want to be able to deal wtih
//  items via some sort of unique id. The most reasonable one is a path-like string
//  made up of the item text of each item down the tree to a given item, so like:
//
//      /Customers/Northeast/NewYork/Bubba Jones
//
//  This is a representation that can be used to represent any item in the list
//  uniquely, as long as duplicates are not allowed in terms of the non-terminal
//  (scope) entries, which wouldn't make much sense to allow. This class assumes
//  that such duplicates won't happen.
//
//  Since there's no way to really easily store such an id path on each item, without
//  some concerns about memory management, we just maintain our own mapping between
//  this type of path and the underlying tree item handles used by the OS. We actually
//  keep two views. One is a hash set of paths that map to the handles. The other
//  is a sorted list of handles that map back to the paths. So we can fairly
//  quickly map from handles to path (in response to internally generated events)
//  and paths to handles (in response to incoming commands from the outside.) The
//  ref hash set owns the items, and the other (a ref vector) just references the
//  items.
//
//
//  We support lazily faulted in trees, which is often a requirement if the tree is
//  displaying server based information or some other high overhead retrieval
//  mechanism. This is accomplished in two ways:
//
//  1.  Setting newly added scopes as 'virtual', which means that they show a + sign
//      but are marked as indeterminate in terms of how many children there are.
//  2.  Knowing whether a scope has previously been expanded or not. Any scope not
//      previous expanded will cause an 'expanding' notification to the derived
//      class when it is expanded. The derived class loads the contents, updates the
//      child count indicator (which gets rid of the +) if there are no children,
//      and marks the scope as having been expanded.
//
//  Now subsequent expansions will not cause an event, it will just expand to show
//  the existing contents. In theory the tree control handles the setting of the
//  'expanded once' setting, but in actual fact it doesn't always, such as if the
//  scope ends up having no children. This will cause problems, so be sure to mark
//  them expanded once you have reacted to an expanding notification, even if they
//  are empty.
//
//  Otherwise, if the user drags files to that scope or does a New file operation
//  on it or something, items will get added. Later, the user expands it and you get
//  an expanding notification and try to load it, only to have duplicate id errors
//  because there are already things inside it.
//
//  ForceExpandedOnce() and UpdateChildCount() handle the two scenarios above. If
//  for some reason, you need to remove the contents of a scope (and later fault it
//  back in if needed), use the SetAutoChild() command. It will collapse the item,
//  and remove any contents, clearing the expanded once flag.
//
//  bHasBeenExpanded() tells you if a scope has been expanded before or not (or maybe
//  it has been but has since been reset by SetAutoChild(). When you add new scopes
//  (either via user action or when faulting in a scope's contents and adding any
//  child scopes), set the bVirtual flag to true so that you will later get the
//  expanding notification and can fault it in.
//
//  Otherwise, outside of that lazy faultin in stuff, this class should correctly
//  manage the +/- sign and the expanded once stuff doesn't matter if you aren't
//  lazily faulting in the list.
//
//
//
// CAVEATS/GOTCHAS:
//
//  1)  Each of the entries in our map contains the full path, which is necessary
//      in order to support a hash set for fast lookup. Otherwise we'd have to
//      traverse the tree every time we wanted to find an item by path. This means
//      that, if an item is renamed, and it's a scope, we have to update all of
//      other items that are under that modified path. The UpdateItem() method
//      handles this operation.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

class TTreeMapItem;
class TTreeView;


// ---------------------------------------------------------------------------
//   CLASS: TTreeEventInfo
//  PREFIX: wnot
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TTreeEventInfo : public TCtrlNotify
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTreeEventInfo
        (
            const   tCIDCtrls::ETreeEvents  eEvent
            , const TString&                strPath
            , const TWindow&                wndSrc
        );

        TTreeEventInfo
        (
            const   tCIDCtrls::ETreeEvents  eEvent
            , const TString&                strNewPath
            , const TString&                strOldPath
            , const TWindow&                wndSrc
        );

        TTreeEventInfo
        (
            const   tCIDCtrls::ETreeEvents  eEvent
            , const TString&                strPath
            , const tCIDLib::TCard4         c4Action
            , const TWindow&                wndSrc
            , const tCIDLib::TBoolean       bState = kCIDLib::False
        );

        TTreeEventInfo
        (
            const   TTreeEventInfo&         wnotSrc
        );

        ~TTreeEventInfo();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTreeEventInfo& operator=
        (
            const   TTreeEventInfo&         wnotSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bState() const;

        tCIDLib::TCard4 c4Action() const;

        tCIDCtrls::ETreeEvents eEvent() const;

        const TString& strOldPath() const;

        const TString& strPath() const;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TTreeEventInfo();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bState
        //      Used when check boxes are enabled, to report the new check state when it
        //      changes
        //
        //  m_c4Action
        //      If it's a menu select event, this is the menu id.
        //
        //  m_strPath
        //      If the event references a particular item in the tree, this is
        //      the path of that item.
        //
        //  m_c4OldPath
        //      For events that refer to an old and new item (like selection change
        //      events) this is the old path. Could be null if we got to the new one
        //      because the first one was added.
        //
        //  m_eEvent
        //      The event that occurred
        // -------------------------------------------------------------------
        tCIDLib::TBoolean       m_bState;
        tCIDLib::TCard4         m_c4Action;
        tCIDCtrls::ETreeEvents  m_eEvent;
        TString                 m_strOldPath;
        TString                 m_strPath;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTreeEventInfo,TCtrlNotify)
        DefPolyDup(TTreeEventInfo)
};



// ---------------------------------------------------------------------------
//  CLASS: TTreeView
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TTreeView : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Public, static data
        // -------------------------------------------------------------------
        static const TNotificationId    nidTreeEvent;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTreeView();

        TTreeView(const TTreeView&) = delete;

        ~TTreeView();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TTreeView& operator=(const TTreeView&) = delete;


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
            const   TString&                strParPath
            , const TString&                strText
            , const tCIDLib::TCard4         c4UserFlag = 0
        );

        tCIDLib::TVoid AddScope
        (
            const   TString&                strParPath
            , const TString&                strText
            , const tCIDLib::TBoolean       bVirtual
        );

        tCIDLib::TBoolean bExpandPath
        (
            const   TString&                strPath
            ,       TString&                strGotToPath
        );

        tCIDLib::TBoolean bHasBeenExpanded
        (
            const   TString&                strPath
        )   const;

        tCIDLib::TBoolean bIsChecked
        (
            const   TString&                strPath
        )   const;

        tCIDLib::TBoolean bIsExpanded
        (
            const   TString&                strPath
        )   const;

        tCIDLib::TBoolean bIsRoot
        (
            const   TString&                strPath
        )   const;

        tCIDLib::TBoolean bIsScope
        (
            const   TString&                strPath
        )   const;

        tCIDLib::TBoolean bItemAt
        (
            const   TPoint&                 pntAt
            ,       TString&                strPath
        )   const;

        tCIDLib::TBoolean bQueryItemArea
        (
            const   TString&                strPath
            ,       TArea&                  areaToFill
            , const tCIDLib::TBoolean       bThrowIfNot
        )   const;

        tCIDLib::TBoolean bPathExists
        (
            const   TString&                strToCheck
        )   const;

        tCIDLib::TBoolean bSelectBestTry
        (
            const   TString&                strPath
        );

        tCIDLib::TBoolean bSelectedItem
        (
                    TString&                strToFill
        )   const;

        tCIDLib::TCard4 c4QueryAllChecked
        (
                    tCIDLib::TStrCollect&   colToFill
            ,       tCIDLib::TCardList&     fcolFlags
        )   const;

        tCIDLib::TCard4 c4UserFlag
        (
            const   TString&                strPath
            , const tCIDLib::TBoolean       bThrowIfNot
        )   const;

        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
            , const tCIDCtrls::ETreeStyles  eTreeStyles = tCIDCtrls::ETreeStyles::None
        );

        tCIDCtrls::ETreeStyles eTreeStyles() const;

        tCIDLib::TVoid ForceExpandedOnce
        (
            const   TString&                strPath
        );

        tCIDLib::TVoid MakeItemVisible
        (
            const   TString&                strPath
        );

        TObject* pobjUser
        (
            const   TString&                strPath
            , const tCIDLib::TBoolean       bThrowIfNot
        );

        const TObject* pobjUser
        (
            const   TString&                strPath
            , const tCIDLib::TBoolean       bThrowIfNot
        )   const;

        tCIDLib::TVoid QueryChildren
        (
            const   TString&                strParScope
            ,       tCIDLib::TStrList&      colToFill
            , const tCIDLib::TBoolean       bItemsOnly
        );

        tCIDLib::TVoid QueryItemText
        (
            const   TString&                strPath
            ,       TString&                strToFill
        );

        tCIDLib::TVoid RemoveChildrenOf
        (
            const   TString&                strParScope
        );

        tCIDLib::TVoid RemoveItem
        (
            const   TString&                strPath
        );

        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TBoolean       bVirtual
        );

        tCIDLib::TVoid SelectPath
        (
            const   TString&                strPath
        );

        tCIDLib::TVoid SetAllChecks
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TVoid SetAllChildChecks
        (
            const   TString&                strPath
            , const tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TVoid SetAutoChild
        (
            const   TString&                strPath
        );

        tCIDLib::TVoid SetCustomDrawOpts
        (
            const   tCIDCtrls::ETreeCustOpts eToSet
        );

        tCIDLib::TVoid SetExpandState
        (
            const   TString&                strPath
            , const tCIDLib::TBoolean       bExpand
        );

        tCIDLib::TVoid SetUserFlag
        (
            const   TString&                strPath
            , const tCIDLib::TCard4         c4ToSet
            , const tCIDLib::TBoolean       bRedraw = kCIDLib::False
        );

        tCIDLib::TVoid SetUserObj
        (
            const   TString&                strPath
            ,       TObject* const          pobjToAdopt
        );

        tCIDLib::TVoid UpdateChildCnt
        (
            const   TString&                strPath
        );

        tCIDLib::TVoid UpdateItem
        (
            const   TString&                strParPath
            , const TString&                strOldText
            , const TString&                strNewText
        );


        // -------------------------------------------------------------------
        //  We provide a templatized method to allow for typesafe registration
        //  of event handlers.
        // -------------------------------------------------------------------
        template <class T> TNotHandlerBase*
        pnothRegisterHandler(        T* const        pTarget
                            ,       tCIDCtrls::EEvResponses (T::*pfnTarget)(TTreeEventInfo&)
                            , const tCIDLib::EEnds  eEnd = tCIDLib::EEnds::Last)
        {
            CheckHandlerRegParms(this, pTarget);

            // Add a request object to us (the source button)
            AddNotifyRequest
            (
                new TNotifyRequestFor<T, TTreeEventInfo>(pTarget, TTreeView::nidTreeEvent)
            );

            // Add a handler object to the target
            TNotHandlerBase* pnothNew = new TNotifyHandlerFor<T, TTreeEventInfo>
            (
                pTarget, TTreeView::nidTreeEvent, pfnTarget
            );
            pTarget->AddNotifyHandler(pnothNew, eEnd);
            return pnothNew;
        }


    protected :
        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDCtrls::ETreeCustRets eCustomDraw
        (
            const   TString&                strPath
            ,       TObject* const          pobjUser
            , const tCIDLib::TCard4         c4UserFlag
            , const tCIDLib::TBoolean       bPost
            , const tCIDLib::TBoolean       bSelected
            , const tCIDLib::TCard4         c4Level
            ,       TRGBClr&                rgbBgn
            ,       TRGBClr&                rgbText
        );


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;

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

        tCIDLib::TVoid Destroyed() override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid RedrawItem
        (
            const   TString&                strPath
        );


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        using TItemMap      = TRefKeyedHashSet<TTreeMapItem, TString, TStringKeyOps>;
        using TItemAltMap   = TRefVector<TTreeMapItem>;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckIsScope
        (
            const   TTreeMapItem&           tmiCheck
            , const tCIDLib::TCard4         c4Line
        )   const;

        TTreeMapItem* ptmiAddMapItem
        (
            const   TString&                strParPath
            , const TString&                strName
            ,       tCIDLib::TSInt          hItem
            , const tCIDLib::TBoolean       bIsScope
            , const tCIDLib::TBoolean       bIsVirtual
            , const tCIDLib::TCard4         c4UserFlag
        );

        const TTreeMapItem* ptmiFromHandle
        (
            const   tCIDLib::TSInt          hItem
            , const tCIDLib::TBoolean       bThrowIfNot
            ,       tCIDLib::TCard4&        c4At
        )   const;

        TTreeMapItem* ptmiFromHandle
        (
            const   tCIDLib::TSInt          hItem
            , const tCIDLib::TBoolean       bThrowIfNot
            ,       tCIDLib::TCard4&        c4At
        );

        const TTreeMapItem* ptmiFromPath
        (
            const   TString&                strPath
            , const tCIDLib::TBoolean       bThrowIfNot
            ,       tCIDLib::TCard4&        c4At
        )   const;

        TTreeMapItem* ptmiFromPath
        (
            const   TString&                strPath
            , const tCIDLib::TBoolean       bThrowIfNot
            ,       tCIDLib::TCard4&        c4At
        );

        tCIDLib::TVoid RemoveMapItem
        (
            const   TString&                strPath
        );

        tCIDLib::TVoid RemoveMapItem
        (
            const   tCIDLib::TSInt          hItem
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eCustOpts
        //      Custom drawing options. Default to none. Set to get custom
        //      drawing callbacks.
        //
        //  m_eTreeStyles
        //      Our class specific styles
        //
        //  m_pcolAltMap
        //  m_pcolMap
        //      We maintain an internal mapping between the tree's own storage and
        //      our our map items. We keep two maps. The main one is adopting and
        //      is a hash set keyed on the item's tree path. The other is non-
        //      adopting and is a vector sorted on the item handles. So we can do
        //      quick mappings from the item handle ot the path or the path to the
        //      item handle.
        //
        //      We have two methods, one to create and one to remove items, and they
        //      make sure these two lists stay in sync.
        //
        //      These are both pointers so that we can keep the map item class
        //      internal and just have a forward reference in this header, so that
        //      we can create the two collections.
        //
        //  m_rgbCustBgn
        //  m_rgbCustText
        //      These are temps for use by the custom drawing handler, so it doesn't
        //      have to create temps on every call, even if not needed.
        // -------------------------------------------------------------------
        tCIDCtrls::ETreeCustOpts    m_eCustOpts;
        tCIDCtrls::ETreeStyles      m_eTreeStyles;
        TItemAltMap*                m_pcolAltMap;
        TItemMap*                   m_pcolMap;
        TRGBClr                     m_rgbCustBgn;
        TRGBClr                     m_rgbCustText;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTreeView, TStdCtrlWnd)
};


// ---------------------------------------------------------------------------
//   CLASS: TTreeItemJan
//  PREFIX: jan
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TTreeItemJan
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTreeItemJan
        (
                    TTreeView* const        pwndTar
            , const TString&                strPath
        );

        ~TTreeItemJan();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Orphan();

        const TString& strPath() const;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pwndTar
        //      The target tree view to operate on. It could be null in which case we
        //      do nothing.
        //
        //  m_strPath
        //      The path to remove, if not orphaned by the dtor.
        // -------------------------------------------------------------------
        TTreeView*  m_pwndTar;
        TString     m_strPath;
};

#pragma CIDLIB_POPPACK


