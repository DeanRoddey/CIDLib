//
// FILE NAME: CIDCtrl_Type.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/07/2015
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
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


namespace tCIDCtrls
{
    // -----------------------------------------------------------------------
    //  Our standard app images come in 3 sizes, and the methods that load
    //  them (on the facility class) take one of these to indicaet which size
    //  to load. They can also ask for the best one for the local system.
    // -----------------------------------------------------------------------
    enum class EAppImgSz
    {
        Default
        , Small
        , Medium
        , Large

        , Count
    };



    // -----------------------------------------------------------------------
    //  The supported clipboard formats
    // -----------------------------------------------------------------------
    enum class EClipFormats
    {
        None
        , Text
        , Bitmap
        , FileDropList
        , StringList
        , Object
        , Class
        , FlatObj

        , Count
        , Min           = None
        , Max           = FlatObj
    };


    // -----------------------------------------------------------------------
    //  Column header styles
    // -----------------------------------------------------------------------
    enum class ECHdrStyles : tCIDLib::TCard4
    {
        None            = 0x0
    };


    // -----------------------------------------------------------------------
    //  Column header events
    // -----------------------------------------------------------------------
    enum class ECHdrEvents
    {
        ColClick
        , DragEnd
        , DragDrag
        , DragStart
    };


    // -----------------------------------------------------------------------
    //  Column header display styles
    // -----------------------------------------------------------------------
    enum class ECHdrTypes
    {
        BitmapOnly
        , TextOnly
        , CenterText
        , CenterBmp
        , BmpRight
        , BmpLeft

        , Count
    };


    // -----------------------------------------------------------------------
    //  Used in the c4FindCtrlData() method of TWindow, to indicate what data is
    //  to be found.
    // -----------------------------------------------------------------------
    enum class EFindCtrlData
    {
        None
        , Text
        , PrefixText
    };


    // -----------------------------------------------------------------------
    //  Context menu options. The class handling TWindow::bShowContextMenu can
    //  indicate the following options:
    //
    //  Done        - It showed a menu or it didn't but doesn't want one to be
    //                shown by a parent or underlying class.
    //  Pass        - Only useful for overridden standard controls, indicates to
    //                just let it pass on to the underlying control. For a regular
    //                custom window this will be treated like propogate.
    //  Propogate   - Make it propogate up to the parent, which can be used to
    //                in the case of overridden standard controls as well or by
    //                custom windows, to indicate it should just propogate by
    //                passing it to the default handler.
    // -----------------------------------------------------------------------
    enum class ECtxMenuOpts
    {
        Done
        , Pass
        , Propogate
    };


    // -----------------------------------------------------------------------
    //  Entry field events
    // -----------------------------------------------------------------------
    enum class EEFEvents
    {
        Changed
    };


    // -----------------------------------------------------------------------
    //  The ways an event can be responded to
    // -----------------------------------------------------------------------
    enum class EEvResponses
    {
        Handled
        , Propagate

        , Count
        , Min           = Handled
        , Max           = Propagate
    };


    // -----------------------------------------------------------------------
    //  The types of things to allow the user to select in the file open dialog. The
    //  values of course allow us to use this is a bitmap.
    // -----------------------------------------------------------------------
    enum class EFOpenTypes
    {
        Files
        , Paths
        , Either
    };


    // -----------------------------------------------------------------------
    //  File save dialog options
    // -----------------------------------------------------------------------
    enum class EFSaveOpts : tCIDLib::TCard4
    {
        None                = 0x0001
        , FilesOnly         = 0x0002
        , FileMustExist     = 0x0004
        , PathMustExist     = 0x0008
        , ConfirmPrompt     = 0x0010
        , StrictTypes       = 0x0020
    };


    // -----------------------------------------------------------------------
    //  List window notification events. This are for list boxes, combo boxes, or other
    //  simple list type controls. ColClicked is only for the multi-column variations.
    //  Marked is only for multi-selection lists and is sent when a new item is marked
    //  (or if a range of marks was changed the index will be c4MaxCard.) Unmarking is
    //  sadly not supportable because it's not reported except for the current item
    //  being toggled. If the list sees a delete key, it sends a delete request so the
    //  app can delete the current item if desired. It's only supported by the list box and
    //  multi-column list box.
    // -----------------------------------------------------------------------
    enum class EListEvents
    {
        Cleared
        , ColClicked
        , DeleteReq
        , Invoked
        , Marked
        , SelChanged
    };


    // -----------------------------------------------------------------------
    //  This is the parameter to the MediaChanged() callback of TWindow to
    //  indicate what type of event.
    // -----------------------------------------------------------------------
    enum class EMediaChTypes
    {
        Arrived
        , RemovePending
        , RemoveComplete

        , Count
    };


    // -----------------------------------------------------------------------
    //  Multi-edit events
    // -----------------------------------------------------------------------
    enum class EMLEEvents
    {
        Changed
    };


    // -----------------------------------------------------------------------
    //  Flags used by TWindow::eDoModal
    // -----------------------------------------------------------------------
    enum class EModalFlags : tCIDLib::TCard4
    {
        None                = 0x000000
        , BreakOnEsc        = 0x000001
        , NoChildMouse      = 0x000002
        , BreakOnExtClick   = 0x000004
        , StopTimers        = 0x000008
        , NoKeys            = 0x000010
        , NoClicks          = 0x000020
        , NoInput           = 0x000040
    };


    // -----------------------------------------------------------------------
    //  The values returned from TWindow::eDoMadal
    // -----------------------------------------------------------------------
    enum class EModalRes
    {
        Unknown
        , DoneFlag
        , Timeout
        , Escape
        , PopUp
        , Quit
        , Exception
        , ExtClick
    };


    // -----------------------------------------------------------------------
    //  The power states accepted by the facility class' SetMonitorPowerState
    //  method.
    // -----------------------------------------------------------------------
    enum class EMonPowerSt
    {
        Off
        , On
        , Standby
    };


    // -----------------------------------------------------------------------
    //  Message box types, icons, and return values
    // -----------------------------------------------------------------------
    enum class EMsgBoxIcons
    {
        Asterisk
        , Exclamation
        , Error
        , Hand
        , Information
        , Question
        , Stop
        , Warning

        , Count
    };

    enum class EMsgBoxRets
    {
        Abort
        , Cancel
        , Continue
        , Ignore
        , No
        , OK
        , Retry
        , Yes

        , Other
    };

    enum class EMsgBoxTypes
    {
        AbortRetryIgnore
        , CancelRetryContinue
        , Help
        , OK
        , OKCancel
        , RetryCancel
        , YesNo
        , YesNoCancel

        , Count
    };


    // -----------------------------------------------------------------------
    //  Object view events
    // -----------------------------------------------------------------------
    enum class EObjVEvents
    {
        Invoked
        , SelChanged
        , Cleared
        , MenuSelect
    };




    // -----------------------------------------------------------------------
    //  Scroll events, sent from scroll bars or built in scroll bars
    // -----------------------------------------------------------------------
    enum class EScrollEvs
    {
        None
        , End
        , Home
        , NextLn
        , NextPg
        , PrevLn
        , PrevPg
    };


    // -----------------------------------------------------------------------
    //  Slider events
    // -----------------------------------------------------------------------
    enum class ESldrEvents
    {
        None
        , Change
        , EndTrack
        , Track

        , Count
    };


    // -----------------------------------------------------------------------
    //  The available system bitmaps
    // -----------------------------------------------------------------------
    enum class ESysBmps
    {
        Size
        , Check
        , Close
        , DnArrowBoxed
        , DnArrow
        , LeftArrow
        , RightArrow
        , UpArrow
    };


    // -----------------------------------------------------------------------
    //  The available system pointers
    // -----------------------------------------------------------------------
    enum class ESysPtrs
    {
        AppStarting
        , Arrow
        , Cross
        , Hand
        , Help
        , IBeam
        , No
        , SizeAll
        , SizeNESW
        , SizeNS
        , SizeNWSE
        , SizeWE
        , UpArrow
        , Wait

        , Count
        , Min           = AppStarting
        , Max           = Wait
    };


    // ----------------------------------------------------------------------
    //  Tabbed window events
    // ----------------------------------------------------------------------
    enum class ETabWEvents
    {
        None
        , PreChange
        , Change
        , Close
        , Hit

        , Count
    };


    // ----------------------------------------------------------------------
    //  Tabs can be marked as in a few common states and update its tab header display
    //  accordingly.
    //
    //  Normal  - Just do normal display
    //  Changes - Display an asterisk beside the text
    //  Errors  - Display a exclamation point beside the text
    // ----------------------------------------------------------------------
    enum class ETabStates
    {
        Normal
        , Changes
        , Errors
    };


    // -----------------------------------------------------------------------
    //  Tool tip icons
    // -----------------------------------------------------------------------
    enum class ETTipIcons
    {
        None
        , Info
        , Warning
        , Error
    };


    // -----------------------------------------------------------------------
    //  Tree view notification events
    // -----------------------------------------------------------------------
    enum class ETreeEvents
    {
        Collapsing
        , Collapsed
        , Invoked
        , Menu
        , Expanding
        , Expanded
        , Rename
        , SelChanging
        , SelChanged
        , CheckState

        , Count
    };


    // -----------------------------------------------------------------------
    //  Some stuff we had to put down in CIDKernel so the build tool can get to it.
    //  So no IDL, and we have to provide any sort of translation stuff ourself.
    // -----------------------------------------------------------------------
    CIDCTRLSEXP tCIDCtrls::EEdgeAnchors eXlatEEdgeAnchors(const TString&);
    CIDCTRLSEXP const TString& strLoadEEdgeAnchors(const tCIDCtrls::EEdgeAnchors);
    CIDCTRLSEXP tCIDCtrls::EWndThemes eXlatEWndThemes(const TString&);
    CIDCTRLSEXP const TString& strLoadEWndThemes(const tCIDCtrls::EWndThemes);


    // ---------------------------------------------------------------------------
    //  Provide streaming and standard enum class support for some of our enums that
    //  have to be defined down below the ability to use the IDL compiler.
    // ---------------------------------------------------------------------------
    #if !defined(CIDCTRLS_NOCLASSES)
    EnumBinStreamMacros(tCIDCtrls::EEdgeAnchors)
    EnumBinStreamMacros(tCIDCtrls::EPosStates)
    EnumBinStreamMacros(tCIDCtrls::EWndStyles)
    EnumBinStreamMacros(tCIDCtrls::EWndThemes)

    StdEnumTricks(tCIDCtrls::EEdgeAnchors)
    StdEnumTricks(tCIDCtrls::EWndThemes)

    BmpEnumTricks(tCIDCtrls::EDlgFlags)
    BmpEnumTricks(tCIDCtrls::EDlgItemFlags)
    BmpEnumTricks(tCIDCtrls::ERedrawFlags)
    BmpEnumTricks(tCIDCtrls::EWndStyles)
    BmpEnumTricks(tCIDCtrls::EExWndStyles)

}

#endif

