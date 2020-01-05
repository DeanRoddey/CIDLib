//
// FILE NAME: TestCIDCtrls.Hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/28/97
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
//  This is the main header of the program. It brings in any needed underlying
//  headers, and any program headers, and in this simple application provides
//  the main classes of the program.
//
//  We need a simple derviatve of the log event class, so that we can assign a
//  unique id to each one (used to manage them in the list box.)
//
//  We need a derivative of the frame window class, to provide our own custom
//  main window processing.
//
//  And we have a simple derivative of the multi-column list box so that we can
//  do some custom drawing.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ----------------------------------------------------------------------------
//  Includes
// ----------------------------------------------------------------------------
#include    "CIDOrbUC.hpp"
#include    "CIDCtrls.hpp"
#include    "CIDLogMon_MessageIds.hpp"
#include    "CIDLogMon_ResourceIds.hpp"


// ----------------------------------------------------------------------------
//  Forward references
// ----------------------------------------------------------------------------
class TMainFrame;



// ----------------------------------------------------------------------------
//  Global data from CIDLogMon.Cpp
// ----------------------------------------------------------------------------
extern TGUIFacility facCIDLogMon;


// ----------------------------------------------------------------------------
//  Constants namespace
// ----------------------------------------------------------------------------
namespace kCIDLogMon
{
    // -------------------------------------------------------------------
    //  Private types and constants
    // -------------------------------------------------------------------
    const tCIDLib::TCard4    c4Col_Dummy     = 0;
    const tCIDLib::TCard4    c4Col_Host      = 1;
    const tCIDLib::TCard4    c4Col_Process   = 2;
    const tCIDLib::TCard4    c4Col_Line      = 3;
    const tCIDLib::TCard4    c4Col_Time      = 4;
    const tCIDLib::TCard4    c4Col_Message   = 5;
    const tCIDLib::TCard4    c4Col_Count     = 6;
}


// ----------------------------------------------------------------------------
//   CLASS: TLogItem
//  PREFIX: li
// ----------------------------------------------------------------------------
class TLogItem : public TLogEvent
{
    public :
        // --------------------------------------------------------------------
        //  Constructors and Destructor
        // --------------------------------------------------------------------
        TLogItem
        (
            const   TLogEvent&              logevSrc
            , const tCIDLib::TCard4         c4UniqueId
        );

        TLogItem(const TLogItem&) = default;

        ~TLogItem();


        // --------------------------------------------------------------------
        //  Public operators
        // --------------------------------------------------------------------
        TLogItem& operator=(const TLogItem&) = default;


        // --------------------------------------------------------------------
        //  Public data members
        // --------------------------------------------------------------------
        tCIDLib::TCard4     m_c4UniqueId;
};


// ----------------------------------------------------------------------------
//  Some types we use internally
// ----------------------------------------------------------------------------
using TLogEvPtr = TCntPtr<TLogItem>;
using TLogEvRef = TWeakPtr<TLogItem>;
using TLogEvDeque = TDeque<TLogEvPtr>;
using TEvList = TSafeVector<TLogEvPtr>;
using TEvRefList = TVector<TLogEvRef>;


// ---------------------------------------------------------------------------
//  CLASS: TItemListWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class TItemListWnd : public TMultiColListBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TItemListWnd();

        TItemListWnd(const TItemListWnd&) = delete;

        ~TItemListWnd();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TItemListWnd& operator=(const TItemListWnd&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid SetParFrame
        (
            const   TMainFrame* const       pwndParent
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() final;

        tCIDCtrls::EMCLBCustRets eCustomDraw
        (
                    TGraphDrawDev&          gdevTar
            , const tCIDLib::TCard4         c4Row
            , const tCIDLib::TCard4         c4Column
            , const tCIDLib::TBoolean       bPost
            , const TArea&                  areaAt
            ,       TRGBClr&                rgbBgn
            ,       TRGBClr&                rgbText
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pwndParent
        //      We get a pointer to our parent so that we can access the log event
        //      items for custom drawing.
        // -------------------------------------------------------------------
        const TMainFrame*   m_pwndParent;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TItemListWnd, TMultiColListBox)
};



// ----------------------------------------------------------------------------
//   CLASS: TMainFrame
//  PREFIX: wnd
// ----------------------------------------------------------------------------
class TMainFrame : public TFrameWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMainFrame();

        TMainFrame(const TMainFrame&) = delete;

        ~TMainFrame();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMainFrame& operator=(const TMainFrame&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreateMain();

        TLogEvPtr cptrFindById
        (
            const   tCIDLib::TCard4         c4IdToFind
        )   const;


    protected :
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
        )   final;

        tCIDLib::TBoolean bAllowShutdown() final;

        tCIDLib::TBoolean bCreated() final;

         tCIDLib::TBoolean bEraseBgn
        (
                    TGraphDrawDev&          gdevToUse
        )   final;

        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   final;

        tCIDLib::TVoid CodeReceived
        (
            const   tCIDLib::TInt4          i4Code
            , const tCIDLib::TCard4         c4Data
        )   final;

        tCIDLib::TVoid Destroyed() final;

        tCIDLib::TVoid DragEnd() final;

        tCIDLib::TVoid MenuCommand
        (
            const   tCIDLib::TResId         ridItem
            , const tCIDLib::TBoolean       bChecked
            , const tCIDLib::TBoolean       bEnabled
        )   final;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bFiltered
        (
            const   TLogEvent&              logevToCheck
        );

        tCIDCtrls::EEvResponses eClickHandler
        (
                    TButtClickInfo&         wnotEvent
        );

        tCIDCtrls::EEvResponses eListHandler
        (
                    TListChangeInfo&        wnotEvent
        );

        tCIDLib::EExitCodes ePollThread
        (
                    TThread&                thrThis
            ,       tCIDLib::TVoid*         pData
        );

        tCIDLib::TVoid GetNewMsgs();

        tCIDLib::TVoid SnapToFile();

        tCIDLib::TVoid UpdateFilters();



        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bShowXXX
        //      A set of filters that are set to allow msgs of that severity to
        //      be shown.
        //
        //  m_c4NextMsgId
        //      The log server gives us back an id each time we call it, and
        //      we have to pass that in each time we call it. This lets it
        //      know what messages we've read so far, so it only gives us what
        //      we've not seen yet.
        //
        //  m_c4NextUID
        //      We assign a unique id to each new item we put into the list, using
        //      this as a running counter. This lets us map between the list window
        //      and our m_colList collection.
        //
        //  m_colList
        //      This is a deque that holds the full list of available items. It's
        //      a deque so that we can throw out old ones efficiently and add new
        //      ones. According to filters we may not be displaying all of these
        //      so the unique message id is used to map back from a list box selection
        //      to one of these items. We add at the top and throw old ones out at
        //      the bottom (and the forward cursor order is bottom to top, so that
        //      keeps them in the right order for when we need to reload due to
        //      filtering.
        //
        //      It holds counted pointers to log events, so that we can give out
        //      weak pointers for things like loading the list or loading new items,
        //      since we have to dump items if this guy starts getting full.
        //
        //  m_colNewEvents
        //      The bgn thread does a query using the m_colQuery below. Once
        //      it has the new events, it will lock this collection and copy
        //      over the data to here. This is one created thread safe and
        //      becomes the means of synchronization between the bgn and GUI
        //      threads. The GUI thread locks it and pulls the items out and adds
        //      them to the main deque used to hold the current items.
        //
        //      This guys holds counted pointers to log event objects. See m_colList
        //      above for why. We just copy them over to the m_colList and do some
        //      processing, then free it up again for the bgn thread to use.
        //
        //  m_colQuery
        //      The collection which we pass to the log server to be filled in
        //      with new events. It has to be events by value since it's being used
        //      to return them via remote call.
        //
        //  m_porbcLogger
        //      We need a log server client proxy. It's created as possible and
        //      recreated when possible if communications is lost.
        //
        //  m_pmenuBar
        //      We have a resource defined menu bar, which we load into this object
        //      and then assign to us as our frame menu.
        //
        //  m_pwndXXX
        //      Pointers to various controls that we want to interact with on a typed
        //      basis. We don't own them.
        //
        //  m_pathLastSavePath
        //      The last directory to which a snap to file was done, or empty if
        //      none yet.
        //
        //  m_strTmpFmt
        //      For the GUI thread's use for formatting data for display.
        //
        //  m_thrPoll
        //      This the thread that goes to the log server and gets the
        //      most recent data. It drops new events into the m_colNewEvents
        //      collection, and posts a message to the GUI, which will come
        //      grab them and write them to the window.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bShowFailures;
        tCIDLib::TBoolean           m_bShowInfos;
        tCIDLib::TBoolean           m_bShowStatuses;
        tCIDLib::TBoolean           m_bShowWarnings;
        tCIDLib::TCard4             m_c4NextMsgId;
        tCIDLib::TCard4             m_c4NextUID;
        TLogEvDeque                 m_colList;
        TEvList                     m_colNewEvents;
        TVector<TLogEvent>          m_colQuery;
        TPathStr                    m_pathLastSavePath;
        TCIDLogSrvClientProxy*      m_porbcLogger;
        TStaticText*                m_pwndAuxText;
        TPushButton*                m_pwndClear;
        TGenericWnd*                m_pwndClient;
        TPushButton*                m_pwndClose;
        TStaticText*                m_pwndErrors;
        TStaticText*                m_pwndFacName;
        TStaticText*                m_pwndFileName;
        TItemListWnd*               m_pwndList;
        TPushButton*                m_pwndSnap;
        TStaticText*                m_pwndThreadName;
        TString                     m_strTmpFmt;
        TThread                     m_thrPoll;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMainFrame, TFrameWnd)
};

