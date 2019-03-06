//
// FILE NAME: CIDCtrls_Menu.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the menu related classes. Note that menus are not windows
//  at our class level. Though under the hood they are, we don't treat them as
//  such.
//
//  We declare a base class and then derivatives for popup menus, menu bars, and
//  sub-menus. Menu bars get assigned to a window, while popups are free floating.
//
//  Sub-menus just provide temporary access to sub-menus within existing menus
//  (or other sub-menus.) We need to be able to get to them via the TMenu class
//  so that we can manipulate them. They don't own the handle, and must be dropped
//  before the parent menu is. It has to be constructed from the immediate parent
//  menu.
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
//  CLASS: TMenu
// PREFIX: menu
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TMenu : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Public class types
        // -------------------------------------------------------------------
        enum class EMenuTypes
        {
            MenuBar
            , PopUp
            , SubMenu
        };


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TMenu();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddActionItem
        (
            const   tCIDLib::TResId         ridTarMenu
            , const TString&                strName
            , const TString&                strText
            , const tCIDLib::TCard4         c4Id
            , const tCIDLib::TBoolean       bEnabled = kCIDLib::True
        );

        tCIDLib::TVoid AddActionItem
        (
            const   TString&                strName
            , const TString&                strText
            , const tCIDLib::TCard4         c4Id
            , const tCIDLib::TBoolean       bEnabled = kCIDLib::True
        );

        tCIDLib::TVoid AddSeparator();

        tCIDLib::TVoid AddSubMenu
        (
            const   TString&                strName
            , const TString&                strText
            , const tCIDLib::TCard4         c4Id
        );

        tCIDLib::TBoolean bIsItemChecked
        (
            const   tCIDLib::TCard4         c4Id
        )   const;

        tCIDLib::TBoolean bIsValid() const;

        tCIDLib::TVoid CleanMenu
        (
            const   tCIDLib::TResId         ridMenu
        );

        tCIDLib::TVoid DeleteItem
        (
            const   tCIDLib::TResId         ridToDel
        );

        tCIDLib::TVoid LoadFromResource
        (
            const   TGUIFacility&           facSrc
            , const tCIDLib::TResId         ridMenu
        );

        tCIDLib::TVoid QueryAccels
        (
                    tCIDLib::TStrList&      colKeyStrs
            ,       tCIDLib::TCardList&     fcolCmds
        )   const;

        tCIDLib::TVoid QueryItemText
        (
            const   tCIDLib::TCard4         c4Id
            ,       TString&                strToFill
        )   const;

        tCIDLib::TVoid QuerySubItemText
        (
            const   tCIDLib::TCard4         c4SubMenuId
            , const tCIDLib::TCard4         c4ItemId
            ,       TString&                strToFill
        )   const;

        tCIDLib::TVoid SetItemCheck
        (
            const   tCIDLib::TCard4         c4Id
            , const tCIDLib::TBoolean       bState
            , const tCIDLib::TBoolean       bUncheckSiblings = kCIDLib::False
        );

        tCIDLib::TVoid SetItemEnable
        (
            const   tCIDLib::TCard4         c4Id
            , const tCIDLib::TBoolean       bState
        );

        tCIDLib::TVoid SetItemText
        (
            const   tCIDLib::TCard4         c4Id
            , const TString&                strToSet
        );

        tCIDLib::TVoid ToggleItemCheck
        (
            const   tCIDLib::TCard4         c4Id
        );


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend class TFrameWnd;
        friend class TSubMenu;


        // -------------------------------------------------------------------
        //  Protected data types
        // -------------------------------------------------------------------
        typedef TFundStack<tCIDCtrls::TMenuHandle>  THandStack;
        typedef TFundStack<tCIDLib::TCard4>         TIdStack;


        // -------------------------------------------------------------------
        //  Hidden Constructors
        // -------------------------------------------------------------------
        TMenu
        (
            const   tCIDLib::TCh* const     pszName
            , const EMenuTypes              eType
        );

        TMenu
        (
            const   TMenu&                  menuParent
            , const tCIDLib::TResId         ridToFind
        );


        // -------------------------------------------------------------------
        //  Protected, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TBoolean bFindItem
        (
                    tCIDCtrls::TMenuHandle  hmenuParent
            , const tCIDLib::TCard4         c4ToFind
            ,       tCIDCtrls::TMenuHandle& hmenuToFill
            ,       tCIDLib::TCard4&        c4Offset
        );

        static tCIDLib::TBoolean bFindItem
        (
                    tCIDCtrls::TMenuHandle  hmenuParent
            , const tCIDLib::TResId         ridToFind
            ,       THandStack&             fcolHandles
            ,       TIdStack&               fcolIds
        );

        static tCIDLib::TBoolean bFindSubMenu
        (
                    tCIDCtrls::TMenuHandle  hmenuParent
            , const tCIDLib::TResId         ridToFind
            ,       tCIDCtrls::TMenuHandle& hmenuToFill
        );

        static tCIDLib::TVoid LoadToMenu
        (
                    tCIDCtrls::TMenuHandle  hmenuTarget
            , const TGUIFacility&           facSrc
            , const tCIDCtrls::TRawMenuItem& rmiCurLevel
            , const TString&                strName
            ,       tCIDLib::TCard4&        c4Count
        );


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AssignToWnd
        (
            const   TFrameWnd* const        pwndTar
        );

        tCIDLib::TCard4 c4FindItem
        (
            const   tCIDLib::TCard4         c4ToFind
            ,       tCIDCtrls::TMenuHandle& hmenuToFill
        )   const;

        tCIDLib::TVoid CheckExists() const;

        tCIDLib::TVoid CheckHandle() const;

        tCIDCtrls::TMenuHandle hmenuFindSubMenu
        (
            const   tCIDLib::TResId         ridToFind
        )   const;

        tCIDCtrls::TMenuHandle hmenuThis() const;

        tCIDCtrls::TWndHandle hwndAssoc() const;

        const TString& strName() const;

        tCIDLib::TVoid StoreHandle
        (
            const   tCIDCtrls::TMenuHandle  hmenuToStore
            , const tCIDLib::TBoolean       bOverwrite = kCIDLib::False
        );

        tCIDLib::TVoid ThrowGetInfo
        (
            const   tCIDLib::TCard4         c4Id
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid Unassign();

    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TMenu(const TMenu&);
        tCIDLib::TVoid operator=(const TMenu&);



        // -------------------------------------------------------------------
        //  Private, static methods
        // -------------------------------------------------------------------
        static tCIDLib::TVoid AddActItem
        (
                    tCIDCtrls::TMenuHandle  hmenuTarget
            , const TString&                strName
            , const TString&                strText
            , const tCIDLib::TCard4         c4Id
            , const tCIDLib::TBoolean       bEnabled = kCIDLib::True
        );

        static tCIDLib::TVoid AddSep
        (
                    tCIDCtrls::TMenuHandle  hmenuTarget
            , const TString&                strName
        );

        static tCIDLib::TVoid AddToMenu
        (
                    tCIDCtrls::TMenuHandle  hmenuTarget
            , const TString&                strName
            , const TGUIFacility&           facSrc
            , const tCIDCtrls::TRawMenuItem& rmiToAdd
        );

        static tCIDLib::TVoid FindHotKeys
        (
            const   tCIDCtrls::TMenuHandle  hmenuCur
            ,       tCIDLib::TStrList&      colKeyStrs
            ,       tCIDLib::TCardList&     fcolCmds
            , const TString&                strSepChar
            ,       TString&                strTmpStr
            ,       tCIDLib::TCh* const     pszBuf
            , const tCIDLib::TCard4         c4BufSz
        );

        static tCIDCtrls::TMenuHandle hmenuAddSub
        (
                    tCIDCtrls::TMenuHandle  hmenuTarget
            , const TString&                strName
            , const TString&                strText
            , const tCIDLib::TCard4         c4Id
            , const tCIDLib::TBoolean       bEnabled = kCIDLib::True
        );

        static tCIDCtrls::TMenuHandle hmenuCreate
        (
            const   EMenuTypes              eType
        );



        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eType
        //      The derived ctor passes this value to us. It lets us know if we
        //      are dealing with a menu bar, popup, sub-menu, etc... so that we
        //      can do things differently as requied.
        //
        //  m_hmenuThis
        //      The menu handle.
        //
        //  m_hwndAssoc
        //      If a menu bar type, we can be associated with a window, and we
        //      remember its window handle since we have to have it for some things.
        //      THis is safe since the window will destroy us when he is destroyed
        //      so this handle won't ever become invalid. If this is not
        //      hwndInvalid, then we assume we are assigned, so this is also the
        //      'is assigned' flag.
        //
        //  m_strName
        //      The name of the menu, for logging / debugging purposes
        // -------------------------------------------------------------------
        EMenuTypes              m_eType;
        tCIDCtrls::TMenuHandle  m_hmenuThis;
        tCIDCtrls::TWndHandle   m_hwndAssoc;
        TString                 m_strName;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMenu, TObject)
};


// ---------------------------------------------------------------------------
//  CLASS: TMenuBar
// PREFIX: menu
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TMenuBar : public TMenu
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMenuBar();

        ~TMenuBar();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsAssigned() const;

        tCIDLib::TVoid Create();

        tCIDLib::TVoid Create
        (
            const   TGUIFacility&           facSrc
            , const tCIDLib::TResId         ridMenu
        );

        tCIDLib::TVoid Disassociate();

        tCIDLib::TVoid Redraw();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TMenuBar(const TMenuBar&);
        tCIDLib::TVoid operator=(const TMenuBar&);


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMenuBar, TMenu)
};



// ---------------------------------------------------------------------------
//  CLASS: TPopupMenu
// PREFIX: menu
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TPopupMenu : public TMenu
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPopupMenu
        (
            const   tCIDLib::TCh* const     pszName
        );

        ~TPopupMenu();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4Process
        (
            const   TWindow&                wndOwner
            , const TPoint&                 pntAt
            , const tCIDLib::EVJustify      eVJustify = tCIDLib::EVJustify::Top
            , const tCIDLib::EHJustify      eHJustify = tCIDLib::EHJustify::Count
        );

        tCIDLib::TCard4 c4Process
        (
            const   TWindow&                wndOwner
            , const TPoint&                 pntAt
            , const TArea&                  areaAvoid
            , const tCIDLib::EVJustify      eVJustify = tCIDLib::EVJustify::Top
            , const tCIDLib::EHJustify      eHJustify = tCIDLib::EHJustify::Count
        );

        tCIDLib::TVoid Create();

        tCIDLib::TVoid Create
        (
            const   TGUIFacility&           facSrc
            , const tCIDLib::TResId         ridMenu
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TPopupMenu(const TPopupMenu&);
        tCIDLib::TVoid operator=(const TPopupMenu&);


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TPopupMenu, TMenu)
};



// ---------------------------------------------------------------------------
//  CLASS: TSubMenu
// PREFIX: menu
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TSubMenu : public TMenu
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSubMenu();

        TSubMenu
        (
            const   TMenu&                  menuParent
            , const tCIDLib::TResId         ridToFind
        );

        ~TSubMenu();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Set
        (
            const   TMenu&                  menuParent
            , const tCIDLib::TResId         ridToFind
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TSubMenu(const TSubMenu&);
        tCIDLib::TVoid operator=(const TSubMenu&);


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSubMenu, TMenu)
};

#pragma CIDLIB_POPPACK


