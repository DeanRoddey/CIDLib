//
// FILE NAME: CIDKernel_Type_Ctrls.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/31/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file contains some stuff related to window controls. There isn't any
//  functionality down here that accesses it, so it can be put into a esparate
//  header that we don't include. But the build tool will need it (for resource
//  file compilation.)
//
//  So CIDBuild can include it, and then CIDCtrls itself can include it for anyone
//  downstream from it. This avoids a lot of unnecessary recompilation.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Since this is ad hoc included, we need a multiple inclusion avoidance mechanism.
// ---------------------------------------------------------------------------
#define CIDKERNEL_TYPE_CTRLS 1


// ---------------------------------------------------------------------------
//  In order to be able to share resource file format between the non-GUI resource
//  compiler and the CIDCtrls facility that uses them, we put them here but make
//  them look like they are CIDWnd stuff.
// ---------------------------------------------------------------------------
namespace tCIDCtrls
{
    // --------------------------------------------------------------------
    //  Some flags that are set on dialogs, based on stuff in the .CIDRC
    //  file's dialog definitions.
    //
    //  UseOrigin means don't auto-center, but use the configured dialog origin
    //
    //  ScreenOrigin is only used if UseOrigin is set. It indicates that the origin
    //  should be taken as screen relative instead of owner relative.
    // --------------------------------------------------------------------
    enum class EDlgFlags : tCIDLib::TCard4
    {
        None                = 0x00000000
        , Sizeable          = 0x00000001
        , UseOrigin         = 0x00000002
        , ScreenOrigin      = 0x00000004
        , SetFgn            = 0x00000008
    };


    // --------------------------------------------------------------------
    //  Some flags that are set on dialog items, based on stuff in the .CIDRC
    //  file's dialog definitions.
    // --------------------------------------------------------------------
    enum class EDlgItemFlags : tCIDLib::TCard4
    {
        None            = 0x00000000
        , Disabled      = 0x00000001
        , InitFocus     = 0x00000002
    };


    // --------------------------------------------------------------------
    //    These are used by the TWindow class to allow a given class to indicate that
    //    it wants to stick to the bottom or right edges when the parent window is
    //    resized.
    //
    //    They can either anchor their right/bottom edges to the right/bottom parent
    //    edge, to make them size up/down. Or, they can anchor their position, to
    //    make them move.
    // --------------------------------------------------------------------
    enum class EEdgeAnchors
    {
        None
        , MoveBottom
        , MoveRight
        , MoveBottomRight

        , SizeBottom
        , SizeRight
        , SizeBottomRight

        , MoveBottomSizeRight
        , SizeBottomMoveRight

        , Count
        , Min           = None
        , Max           = SizeBottomMoveRight
    };


    // -----------------------------------------------------------------------
    //  This is used by the TTouchWnd class, when reporting touch events to the
    //  derived class, to indicate which event it is reporting.
    // -----------------------------------------------------------------------
    enum class EGestEvs
    {
        None
        , Start
        , Drag
        , Inertia
        , End
    };


    // -----------------------------------------------------------------------
    //  When a gesture starts, TTouchWnd calls this to allow the derived class
    //  indicate what type of gesture it will accept, if any, depending on its
    //  current state.
    // -----------------------------------------------------------------------
    enum class EGestOpts
    {
        Disable
        , Pan
        , PanInertia
        , Flick
    };


    // -----------------------------------------------------------------------
    //  Passed to the gesture target at different steps in processing a
    //  gesture, to give it a chance to set up and tear down.
    // -----------------------------------------------------------------------
    enum class EGestReset
    {
        Starting
        , Ended
        , Cancelled
    };



    // -----------------------------------------------------------------------
    //  The types of menu items
    // -----------------------------------------------------------------------
    enum class EMenuItemTypes
    {
        Decoration
        , ActionItem
        , SubMenu

        , Count
        , Min           = Decoration
        , Max           = SubMenu
    };


    // -----------------------------------------------------------------------
    //  The basic position states of a window, reported when a window position
    //  or size changes.
    // -----------------------------------------------------------------------
    enum class EPosStates
    {
        Minimized
        , Maximized
        , Restored

        , Count
        , Min           = 0
        , Max           = Restored
    };


    // -----------------------------------------------------------------------
    //  Flags for the TWindow::Redraw method
    // -----------------------------------------------------------------------
    enum class ERedrawFlags
    {
        None                = 0x00000000
        , Erase             = 0x00000001
        , Immediate         = 0x00000002
        , Children          = 0x00000004

        , Reserve           = 0x10000000

        , EraseChildren     = 0x00000005
        , ImmedChildren     = 0x00000006
        , ImmedErase        = 0x00000003
        , Full              = 0x00000007
    };


    // ----------------------------------------------------------------------
    //  The available system colors
    //
    //  IF YOU CHANGE THIS, update the QuerySystemColor method in the facility class
    //  to match. These must be contiguous and zero based! Note that we create
    //  some 'synonyms' to make things more self-documenting, instead of using
    //  generic names all the time. Ones ending in NF are the ones to use when you
    //  don't have focus.
    // ----------------------------------------------------------------------
    enum class ESysColors
    {
        None
        , Highlight3D
        , AppWorkspace
        , HighlightBgn
        , HighlightBgnNF
        , HighlightText
        , HighlightTextNF
        , Window
        , WindowText

        // Ones we create ourself
        , DialogBgn
        , DlgCtrlFill
        , EditWndBgn

        , Count
        , Syn_FirstCust = DialogBgn
        , Min           = None
        , Max           = EditWndBgn
    };



    // -----------------------------------------------------------------------
    //  The special regions of a window
    // -----------------------------------------------------------------------
    enum class EWndAreas : tCIDLib::TInt4
    {
        Transparent         = -1
        , Nowhere           = 0
        , Client            = 1
        , Caption           = 2
        , SysMenu           = 3
        , Size              = 4
        , Menu              = 5
        , HScroll           = 6
        , VScroll           = 7
        , MinButton         = 8
        , MaxButton         = 9
        , Left              = 10
        , Right             = 11
        , Top               = 12
        , TopLeft           = 13
        , TopRight          = 14
        , Bottom            = 15
        , BottomLeft        = 16
        , BottomRight       = 17
        , Border            = 18
        , CloseButton       = 20
    };



    // -----------------------------------------------------------------------
    //  The standard window styles we support
    // -----------------------------------------------------------------------
    enum class EWndStyles : tCIDLib::TCard4
    {
        None                    = 0x0

        , Border                = 0x00800000
        , Caption               = 0x00C00000
        , Child                 = 0x40000000
        , ClipChildren          = 0x02000000
        , ClipSiblings          = 0x04000000
        , Disabled              = 0x08000000
        , DlgFrame              = 0x00400000
        , Group                 = 0x00020000
        , HScroll               = 0x00100000
        , Popup                 = 0x80000000
        , SizeBorder            = 0x00040000
        , SysMenu               = 0x00080000
        , TabStop               = 0x00010000
        , Visible               = 0x10000000
        , VScroll               = 0x00200000

        // Common combos
        , HelperFrame           = 0x16CC0000
        , PaneContainer         = 0x52000000
        , StdFrameNV            = 0x02CC0000
        , StdFrame              = 0x12CC0000

        , ClippingChild         = 0x42000000
        , ClippingTabChild      = 0x42010000
        , ClippingVisChild      = 0x52000000
        , ClippingVisGrpChild   = 0x52020000
        , ClippingVisTabChild   = 0x52010000
        , TabChild              = 0x40010000
        , TabGroupChild         = 0x40030000
        , TabGroup              = 0x00030000
        , VisTabGroupChild      = 0x50030000
        , VisTabChild           = 0x50010000
        , VisChild              = 0x50000000
        , VisGroupChild         = 0x50020000
    };


    // -----------------------------------------------------------------------
    //  The extended window styles we support
    // -----------------------------------------------------------------------
    enum class EExWndStyles : tCIDLib::TCard4
    {
        None                = 0x0

        , AcceptFiles       = 0x00000010
        , AppWindow         = 0x00040000
        , Composited        = 0x02000000
        , ContextHelp       = 0x00000400
        , ControlParent     = 0x00010000
        , Layered           = 0x00080000
        , NoActivate        = 0x80000000
        , RaisedEdge        = 0x00000100
        , StaticEdge        = 0x00020000
        , SunkenEdge        = 0x00000200
        , ToolBar           = 0x00000080
        , TopMost           = 0x00000008
        , Transparent       = 0x00000020
    };


    // -----------------------------------------------------------------------
    //  The window themes we support. Each top level frame/dialog has a theme
    //  and all its children adjust themselves based on that theme.
    //
    //  We provide format/translate methods for this in the types file of the
    //  CIDCtrls facility. So update that if you change this.
    // -----------------------------------------------------------------------
    enum class EWndThemes : tCIDLib::TCard4
    {
        None
        , MainWnd
        , DialogBox

        , Count
        , Min               = None
        , Max               = DialogBox
    };


    #pragma CIDLIB_PACK(1)

    // -----------------------------------------------------------------------
    //  The format of a dialog description resource. These must be built per
    //  platform, so we can just store them in a raw structure in the native
    //  format.
    //
    //  The defined bits for the c4Flags member are:
    //
    //      0x000001 - Resizeable dialog
    // -----------------------------------------------------------------------
    struct TRawDlgDesc
    {
        tCIDLib::TCard4     c4FormatVersion;
        tCIDLib::TResId     ridDlg;
        tCIDLib::TInt4      i4X;
        tCIDLib::TInt4      i4Y;
        tCIDLib::TCard4     c4CX;
        tCIDLib::TCard4     c4CY;
        EDlgFlags           eFlags;
        EWndThemes          eTheme;
        tCIDLib::TMsgId     midTitle;
        tCIDLib::TCard4     c4ChildCount;
    };


    // -----------------------------------------------------------------------
    //  If the high bit of midText is set (and it's not c4MaxCard), then it's
    //  one of the special text ids, and refers to an id in the common messages
    //  text. This allows us to avoid hundreds of redundant Save, Cancel, etc...
    //  loadable text items and helps lower translation costs. Other loadable
    //  text is always from the messages set of loadable text.
    //
    //  The trailing hints string is a comma separated list of text tokens, which
    //  provide hints to the control about how it should set itself up, in
    //  conjunction with the parent class' theme.
    //
    //  The trailing item type string is the actual item type from the RC file,
    //  which CIDCtrls will translate into a real window class.
    //
    //  The trailing image string is an app image name, and is usually empty for
    //  most control types.
    //
    //  NOTE:
    //  Any text values are after the structures. They are written, with null
    //  terminator, after the structure, and a length member in the structure is
    //  set to indicate how many characters (not including null) are there. If
    //  zero, then the value isn't used. The length members are the final members
    //  of the structure and the strings are in the same order. Be sure to include
    //  the null when jumping forward to the next one!
    //
    //  Remember the strings are native TCh format, so each char will be
    //  kCIDLib::c4CharBytes in size! So be sure to adjust for this when you
    //  do offset calculations.
    // -----------------------------------------------------------------------
    struct TRawDlgItem
    {
        tCIDLib::TInt4      i4X;
        tCIDLib::TInt4      i4Y;
        tCIDLib::TCard4     c4CX;
        tCIDLib::TCard4     c4CY;
        tCIDLib::TResId     ridThis;
        tCIDLib::TMsgId     midText;
        tCIDLib::TMsgId     midFlyOver;
        tCIDLib::TMsgId     midCue;
        EEdgeAnchors        eAnchor;
        EDlgItemFlags       eFlags;
        tCIDLib::TCard4     c4HintsLen;
        tCIDLib::TCard4     c4ItemTypeLen;
        tCIDLib::TCard4     c4ImageLen;
    };


    // -----------------------------------------------------------------------
    //  The format of a menu resource item.
    //
    //  eType
    //      Indicates the type of item. If a decoration, this is the only
    //      field that is used. For action items and sub-menus, most of the
    //      other fields are used.
    //
    //  midText
    //      The message id of the display text. This is not used for
    //      decorations, but is used for others.
    //
    //  c4Id
    //      The command id for this menu item, i.e. the value sent to the
    //      owner window.
    //
    //  c2SubCount
    //      For submenus only. It is used to store the count of immediate
    //      child items.
    //
    //  c2NextOfs
    //      The byte offset to the next sibling item of this item. Since
    //      submenus can be multiply nested, its not always easy to find the
    //      next sibling, so we store that data for faster access at runtime.
    // -----------------------------------------------------------------------
    struct TRawMenuItem
    {
        EMenuItemTypes      eType;
        tCIDLib::TMsgId     midText;
        tCIDLib::TCard4     c4Id;

        tCIDLib::TCard2     c2SubCount;
        tCIDLib::TCard2     c2NextOfs;
    };


    // -----------------------------------------------------------------------
    //  The format of a menu resource description.
    //
    //  c2Count
    //      The count of entries in the armiList. If there are no entries
    //      defined, the resource compiler won't store the resource and the
    //      runtime loader will throw an exception.
    //
    //  armiList
    //      The list of raw menu items. These are not stored in a regular
    //      array. The notification id text for each action item follows it
    //      in memory. This field just exist to provide a convenient starting
    //      point for processing the list.
    // -----------------------------------------------------------------------
    struct TRawMenuDesc
    {
        tCIDLib::TCard2     c2Count;
        TRawMenuItem        armiList[1];
    };

    #pragma CIDLIB_POPPACK
}

