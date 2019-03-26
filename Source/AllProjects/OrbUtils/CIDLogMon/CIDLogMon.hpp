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
            const   TLogEvent&              errSrc
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
        tCIDLib::TBoolean bCreated() override;

        tCIDCtrls::EMCLBCustRets eCustomDraw
        (
                    TGraphDrawDev&          gdevTar
            , const tCIDLib::TCard4         c4Row
            , const tCIDLib::TCard4         c4Column
            , const tCIDLib::TBoolean       bPost
            , const TArea&                  areaAt
            ,       TRGBClr&                rgbBgn
            ,       TRGBClr&                rgbText
        )   override;


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

        ~TMainFrame();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreateMain();

        const TLogItem* pliFindById
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
        )   override;

        tCIDLib::TBoolean bAllowShutdown() override;

        tCIDLib::TBoolean bCreated() override;

         tCIDLib::TBoolean bEraseBgn
        (
                    TGraphDrawDev&          gdevToUse
        )   override;

        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;

        tCIDLib::TVoid CodeReceived
        (
            const   tCIDLib::TInt4          i4Code
            , const tCIDLib::TCard4         c4Data
        )   override;

        tCIDLib::TVoid Destroyed() override;

        tCIDLib::TVoid DragEnd() override;

        tCIDLib::TVoid MenuCommand
        (
            const   tCIDLib::TResId         ridItem
            , const tCIDLib::TBoolean       bChecked
            , const tCIDLib::TBoolean       bEnabled
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TMainFrame(const TMainFrame&);
        tCIDLib::TVoid operator=(const TMainFrame&);


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
        //  m_colLoad
        //      For loading items into the list box and to deal with filtering
        //      we need to get a filtered list. So we use a non-adopting ref vector
        //      to point at items in the main m_colList long enough to get them
        //      loaded into the list. After that, the unique ids in each item are
        //      used to refer back to the main list.
        //
        //  m_colNewEvents
        //      The bgn thread does a query using the m_colQuery below. Once
        //      it has the new events, it will lock this collection and copy
        //      over the data to here. This is one created thread safe and
        //      becomes the means of synchronization between the bgn and GUI
        //      threads. The GUI thread locks it and pulls the items out and adds
        //      them to the main deque used to hold the current items.
        //
        //  m_colQuery
        //      The collection which we pass to the log server to be filled in
        //      with new events. We don't use m_colNewEvents because worst
        //      case the GUI thread might not have got the last set that we
        //      put there. Also, this means we don't have to block the other
        //      collection while we query, only for the short period when we
        //      are loading new events into it.
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
        TDeque<TLogItem>            m_colList;
        TRefVector<const TLogItem>  m_colLoad;
        TVector<TLogEvent>          m_colNewEvents;
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

