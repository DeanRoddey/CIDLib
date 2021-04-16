//
// FILE NAME: CIDCtrls_MultiColListBox.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/15/2015
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
//  This is a fancy, multi-column list box, which is really a list view control that only
//  supports the 'report' view. It derives from the standard list wnd so that it can share
//  its event notification scheme and be interacted with via that abstract base class.
//
//  Each row can have a row id and a string associated with it, as well as a boolean
//  flag. This makes it fairly easy to map between selected list items and the original
//  data it is displaying.
//
//  You can either load the rows as quoted comma lists, one for each row, with each value in
//  the list being a column, or you can create a TMCListOps class derivative which knows how
//  to extract the info that the list box needs to get to the column texts and the id. Or you
//  can just load them a row at a time using a vector of strings, one entry per column.
//
//  The ops object can be used to add a single object or a whole list. The list object will
//  use the c4RowCount() method to figure out how many rows are available to load.
//
//  We support automatic in place editing. We create a small derivative of the standard IPE
//  interface, adding an extra method. The reason being that we have to get the client code
//  to gen up an attr data object on the fly for the column (and row of course) being edited,
//  and to ask them if they want to allow that column to even be edited. The underlying IPE
//  interface handles the rest of the interfacing required. This makes it fairly easy for a
//  window or dialog box to support ad hoc in place editing.
//
//
//  !!!!
//  This this is horribly unreliable when doing composited drawing, which we have to do in
//  order to make resizing smooth (where this guy is one of the worst thus forcing composited
//  drawing.) So, we have to jump through a lot of hoops.
//
//  1.  We don't directly sub-class the listview window control. We create a nested one.
//  2.  We internally sub-class it, and forward messages to an internal handler that can
//      process the ones we need. We have to pass on anything that would go to the actual
//      listview (as apposed to being sent as a notification.)
//  3.  We disable notification forwarding (at the TWindow level) so that the listview's
//      notifications don't get reflected back to him but come to us.
//  4.  We create the listview in our client area, so that this window can draw the border,
//      since the standard listview is horrible about drawing the border reliably.
//  5.  The primary thing we handle in the internal sub-class is extended keys and mouse
//      clicks. In the extended key scenario we turn that into a call to our standard
//      extended key handler (something the TWindow class would normally do), so that any
//      subclasses of ours can intercept other keys easily. Clicks we process internally,
//      since we don't really need derived classes doing any other click processing than
//      what we already report.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  We are a 'user data' type list, so we manage the data ourself. So we do most of the
//      work. Sorting is done by us. If sorting is enabled, then we don't send any
//      notifications about the user clicking column titles, we handle that internally.
//
//  2)  This also means that we really only need to tell the window itself when the size
//      of the list changes so that he can know how many elements. When needs info he
//      asks us for it, so if we do something like swap two rows, he doesn't care (though
//      we have to appropriately get him to redraw to reflect the change. So we tell him
//      about removes and adds, so that he keeps the list size correct. And he maintains
//      the built in state stuff about each item (marked, selected, etc...) so we go
//      through him for that. But we can keep any other info we need for each row since
//      we keep the actual data.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)


class TMultiColListBox;


// ---------------------------------------------------------------------------
//  CLASS: TMCListOps
// PREFIX: mclops
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TMCListOps
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructors
        // -------------------------------------------------------------------
        TMCListOps(const TMCListOps&) = delete;

        virtual ~TMCListOps();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMCListOps& operator=(const TMCListOps&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TCard4 c4ColCount() const = 0;

        virtual tCIDLib::TCard4 c4RowCount() const = 0;

        virtual tCIDLib::TCard4 c4RowId
        (
            const   tCIDLib::TCard4         c4RowIndex
        )   const = 0;

        virtual const TString& strColumnText
        (
            const   tCIDLib::TCard4         c4RowIndex
            , const tCIDLib::TCard4         c4ColIndex
        )   const = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TMCListOps();
};



// ---------------------------------------------------------------------------
//  CLASS: MMCLBIPEIntf
// PREFIX: ipei
// ---------------------------------------------------------------------------
class CIDCTRLSEXP MMCLBIPEIntf : public MIPEIntf
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        MMCLBIPEIntf();

        MMCLBIPEIntf(const MMCLBIPEIntf&) = delete;

        virtual ~MMCLBIPEIntf();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        MMCLBIPEIntf& operator=(const MMCLBIPEIntf&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        //
        //  If the client code says yes, edit it, it has to fill in the attr data
        //  object. If it sets the value, it should set bValueSet, else we will
        //  set it generically as string data (from the list column clicked.)
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bIPETestCanEdit
        (
            const   tCIDCtrls::TWndId       widSrc
            , const tCIDLib::TCard4         c4ColInd
            ,       TAttrData&              adatFill
            ,       tCIDLib::TBoolean&      bValueSet
        )   = 0;
};



// ---------------------------------------------------------------------------
//  CLASS: TMultiColListBox
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TMultiColListBox : public TListWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMultiColListBox();

        TMultiColListBox(const TMultiColListBox&) = delete;

        ~TMultiColListBox();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMultiColListBox& operator=(const TMultiColListBox&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsMultiSel() const override;

        tCIDLib::TBoolean bSelectByText
        (
            const   TString&                strToSel
            ,       tCIDLib::TCard4&        c4FoundAt
            , const tCIDLib::TBoolean       bForceEvent = kCIDLib::False
        )   override;

        tCIDLib::TCard4 c4CurItem() const override;

        tCIDLib::TCard4 c4FindByText
        (
            const   TString&                strToFind
        )   const override;

        tCIDLib::TCard4 c4IndexToId
        (
            const   tCIDLib::TCard4         c4Index
        )   const override;

        tCIDLib::TCard4 c4ItemCount() const override;

        tCIDLib::TCard4 c4StartsWith
        (
            const   TString&                strToFind
        )   const override;

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

        tCIDLib::TVoid RemoveAll
        (
            const   tCIDLib::TBoolean       bNoEvent = kCIDLib::False
        )   override;

        tCIDLib::TVoid RemoveAt
        (
            const   tCIDLib::TCard4         c4ToSel
            , const tCIDLib::TBoolean       bSelectAnother = kCIDLib::True
        )   override;

        tCIDLib::TVoid SelectByIndex
        (
            const   tCIDLib::TCard4         c4ToSel
            , const tCIDLib::TBoolean       bForceEvent = kCIDLib::False
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   override;

        tCIDLib::TVoid SetWndStyle
        (
            const   tCIDCtrls::EWndStyles   eToSet
            , const tCIDLib::TBoolean       bSetIt
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AutoSizeCol
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bUseHeader
        );

        tCIDLib::TBoolean bFlagAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TBoolean bIsMarked
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TBoolean bMoveDn
        (
            const   tCIDLib::TBoolean       bFollowSel
        );

        tCIDLib::TBoolean bMoveUp
        (
            const   tCIDLib::TBoolean       bFollowSel
        );

        tCIDLib::TCard4 c4AddItem
        (
            const   tCIDLib::TStrCollect&   colColVals
            , const tCIDLib::TCard4         c4Id
            , const tCIDLib::TCard4         c4InsertAt = kCIDLib::c4MaxCard
            , const tCIDLib::TBoolean       bSelect = kCIDLib::False
        );

        tCIDLib::TCard4 c4AddItem
        (
            const   tCIDLib::TStrCollect&   colColVals
            , const tCIDLib::TCard4         c4Id
            , const TString&                strUserData
            , const tCIDLib::TCard4         c4InsertAt = kCIDLib::c4MaxCard
            , const tCIDLib::TBoolean       bSelect = kCIDLib::False
        );

        tCIDLib::TCard4 c4ColCount() const;

        tCIDLib::TCard4 c4ColsWidth() const;

        tCIDLib::TCard4 c4FindByUserData
        (
            const   tCIDLib::TCard4         c4ToFind
            , const tCIDLib::TCard4         c4StartInd = 0
        )   const;

        tCIDLib::TCard4 c4FindByUserData
        (
            const   tCIDLib::TCard8         c8ToFind
            , const tCIDLib::TCard4         c4StartInd = 0
        )   const;

        tCIDLib::TCard4 c4FindByUserData
        (
            const   TString&                strToFind
            , const tCIDLib::TCard4         c4StartInd
            , const tCIDLib::TBoolean       bCaseSensitive
        )   const;

        tCIDLib::TCard4 c4FindText
        (
            const   TString&                strFind
            , const tCIDLib::TCard4         c4ColIndex
            , const tCIDLib::TCard4         c4StartInd
            , const tCIDLib::EMatchTypes    eType
            , const tCIDLib::TBoolean       bCaseSensitive
        )   const;

        tCIDLib::TCard4 c4IdToIndex
        (
            const   tCIDLib::TCard4         c4Id
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        )   const;

        tCIDLib::TCard4 c4SearchCol() const;

        tCIDLib::TCard4 c4SearchCol
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TCard4 c4SelectById
        (
            const   tCIDLib::TCard4         c4IdToSel
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
            , const tCIDLib::TBoolean       bForceEvent = kCIDLib::False
        );

        tCIDLib::TCard4 c4SortCol() const;

        tCIDLib::TCard4 c4TextToIndex
        (
            const   TString&                strKey
            , const tCIDLib::TCard4         c4ColIndex
        )   const;

        tCIDLib::TCard4 c4TopIndex() const;

        tCIDLib::TCard4 c4UserDataAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TCard8 c8UserDataAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TVoid ClearSelection();

        tCIDLib::TVoid CreateMCLB
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDLib::TStrCollect&   colColTites
            , const tCIDLib::TCard4         c4SearchCol
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EMCLBStyles  eMCLBStyles = tCIDCtrls::EMCLBStyles::None
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDCtrls::EMCLBStyles eMCLBStyles() const;

        tCIDLib::TVoid EditCell
        (
            const   tCIDLib::TCard4         c4Row
            , const tCIDLib::TCard4         c4Col
        );

        tCIDLib::TVoid LoadList
        (
            const   tCIDLib::TStrCollect&   colLoadWith
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        );

        tCIDLib::TVoid LoadList
        (
            const   tCIDLib::TStrList&      colValues
            , const tCIDLib::TCardList&     fcolIds
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        );

        tCIDLib::TVoid LoadList
        (
            const   TMCListOps&             mclopsSrc
            , const tCIDLib::TBoolean       bAppend = kCIDLib::False
        );

        tCIDLib::TVoid MakeItemVisible
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bFullyVisible
        );

        tCIDLib::TVoid MarkAll
        (
            const   tCIDLib::TBoolean       bState
        );

        tCIDLib::TVoid MarkAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bState
        );

        tCIDLib::TVoid MoveRow
        (
            const   tCIDLib::TCard4         c4From
            , const tCIDLib::TCard4         c4To
            , const tCIDLib::TBoolean       bFollowSel = kCIDLib::False
        );

        tCIDLib::TVoid QueryColArea
        (
            const   tCIDLib::TCard4         c4Row
            , const tCIDLib::TCard4         c4Col
            ,       TArea&                  areaToFill
            , const tCIDLib::TBoolean       bScreenRel = kCIDLib::False
        )   const;

        tCIDLib::TVoid QueryColText
        (
            const   tCIDLib::TCard4         c4RowIndex
            , const tCIDLib::TCard4         c4ColIndex
            ,       TString&                strToFill
        )   const;

        tCIDLib::TVoid QueryColWidths
        (
                    tCIDLib::TCardList&     fcolToFill
        )   const;

        tCIDLib::TVoid QueryRowArea
        (
            const   tCIDLib::TCard4         c4Row
            ,       TArea&                  areaToFill
        )   const;

        tCIDLib::TVoid RedrawRange
        (
            const   tCIDLib::TCard4         c4StartInd
            , const tCIDLib::TCard4         c4EndInd
        );

        tCIDLib::TVoid RedrawRow
        (
            const   tCIDLib::TCard4         c4RowIndex
        );

        tCIDLib::TVoid RedrawVisible();

        tCIDLib::TVoid RemoveRange
        (
            const   tCIDLib::TCard4         c4Start
            , const tCIDLib::TCard4         c4End
        );

        tCIDLib::TVoid Resort();

        const TString& strUserDataAt
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        TSize szGridExtent() const;

        tCIDLib::TVoid ScrollToBottom();

        tCIDLib::TVoid SetAutoIPE
        (
                    MMCLBIPEIntf* const      pmcipeAdopt
        );

        tCIDLib::TVoid SetColTo
        (
            const   tCIDLib::TCard4         c4ColIndex
            , const TString&                strToSet
        );

        tCIDLib::TVoid SetColumns
        (
            const   tCIDLib::TStrCollect&   colColTitles
            , const tCIDLib::TCard4         c4SearchCol = 0
        );

        tCIDLib::TVoid SetColOpts
        (
            const   tCIDLib::TCard4         c4ColIndex
            , const tCIDLib::TCard4         c4Width = kCIDLib::c4MaxCard
            , const tCIDLib::EHJustify      eJustify = tCIDLib::EHJustify::Left
            , const tCIDLib::TCard4         c4MinWidth = 0
        );

        tCIDLib::TVoid SetColText
        (
            const   tCIDLib::TCard4         c4RowIndex
            , const tCIDLib::TCard4         c4ColIndex
            , const TString&                strToSet
        );

        tCIDLib::TVoid SetColTitle
        (
            const   tCIDLib::TCard4         c4ColIndex
            , const TString&                strToSet
        );

        tCIDLib::TVoid SetColWidths
        (
            const   tCIDLib::TCardList&     fcolToSet
        );

        tCIDLib::TVoid SetCustomDrawOpts
        (
            const   tCIDCtrls::EMCLBCustOpts eToSet
        );

        tCIDLib::TVoid SetFlagAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TVoid SetMCLBStyle
        (
            const   tCIDCtrls::EMCLBStyles  eToChange
            , const tCIDLib::TBoolean       bNewState
        );

        tCIDLib::TVoid SetSortColumn
        (
            const   tCIDLib::TCard4         c4Index
        );

        tCIDLib::TVoid SetUserDataAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid SetUserDataAt
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TCard8         c8ToSet
        );

        tCIDLib::TVoid SetUserDataAt
        (
            const   tCIDLib::TCard4         c4Index
            , const TString&                strToSet
        );

        tCIDLib::TVoid SwapRows
        (
            const   tCIDLib::TCard4         c4First
            , const tCIDLib::TCard4         c4Sec
            , const tCIDLib::TBoolean       bFollowSel = kCIDLib::False
        );

        tCIDLib::TVoid UpdateIdAt
        (
            const   tCIDLib::TCard4         c4RowIndex
            , const tCIDLib::TCard4         c4NewId
        );

        tCIDLib::TVoid UpdateRowAt
        (
            const   TMCListOps&             mclopsSrc
            , const tCIDLib::TCard4         c4RowIndex
        );

        tCIDLib::TVoid UpdateRowAt
        (
            const   tCIDLib::TStrList&      colVals
            , const tCIDLib::TCard4         c4RowIndex
            , const tCIDLib::TCard4         c4RowId
            , const tCIDLib::TBoolean       bRedraw = kCIDLib::True
        );


    protected :
        // -------------------------------------------------------------------
        //  Declare our friends
        // -------------------------------------------------------------------
        friend tCIDCtrls::TMsgResult WNDPROCAPI mresLVDispatch
        (
            const   tCIDCtrls::TWndHandle   hwndThis
            , const tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      iParam
        );


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TBoolean bInvoked
        (
            const   tCIDLib::TCard4         c4Row
            , const tCIDLib::TCard4         c4Id
        );

        virtual tCIDLib::TVoid CellClicked
        (
            const   tCIDLib::TCard4         c4Row
            , const tCIDLib::TCard4         c4Column
            , const tCIDLib::TBoolean       bLeftButton
        );

        virtual tCIDCtrls::EMCLBCustRets eCustomDraw
        (
                    TGraphDrawDev&          gdevTar
            , const tCIDLib::TCard4         c4Row
            , const tCIDLib::TCard4         c4Column
            , const tCIDLib::TBoolean       bPost
            , const TArea&                  areaAt
            ,       TRGBClr&                rgbBgn
            ,       TRGBClr&                rgbText
        );

        virtual tCIDLib::TVoid ColumnsSet
        (
            const   tCIDLib::TCard4         c4Count
        );

        virtual tCIDLib::ESortComps eCompColVals
        (
            const   TString&                strVal1
            , const TString&                strVal2
            , const tCIDLib::TCard4         c4ColInd
        )   const;

        virtual tCIDLib::TVoid TitleClicked
        (
            const   tCIDLib::TCard4         c4ColIndex
        );


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

        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TBoolean bEraseBgn
        (
                    TGraphDrawDev&          gdevToUse
        )   override;

        tCIDLib::TBoolean bExtKey
        (
            const   tCIDCtrls::EExtKeys     eKey
            , const tCIDLib::TBoolean       bAlt
            , const tCIDLib::TBoolean       bCtrl
            , const tCIDLib::TBoolean       bShift
            , const TWindow* const          pwndChild = nullptr
        )   override;

        tCIDLib::TBoolean bNotReflect
        (
                    TWindow&                wndTar
            , const tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::TMsgResult&  mresRet
        )   override;

        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;

        tCIDLib::TVoid Destroyed() override;

        tCIDLib::TVoid GettingFocus() override;


    private :
        // -------------------------------------------------------------------
        //  Private data types
        //
        //  We are owner data, since otherwise it's too stupid to send us the bulk
        //  mode state change notifications and it's useless without those. Since
        //  we have to manage the data, we can provide more functionality as well.
        // -------------------------------------------------------------------
        class TItemData
        {
            public :
                TItemData
                (
                    const   tCIDLib::TStrCollect&   colVals
                    , const tCIDLib::TCard4         c4RowId
                );

                TItemData(const tCIDLib::TCard4 c4Cols);

                TItemData
                (
                    const   TItemData&              idSrc
                );

                ~TItemData();

                TItemData& operator=
                (
                    const   TItemData&              idSrc
                );

                // Public data
                tCIDLib::TBoolean   m_bFlag;
                tCIDLib::TCard4     m_c4RowId;
                tCIDLib::TCard4     m_c4UserData;
                tCIDLib::TCard8     m_c8UserData;
                tCIDLib::TStrList   m_colCols;
                TString*            m_pstrUserData;
        };
        typedef TVector<TItemData>  TListData;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AutoIPE
        (
            const   tCIDLib::TCard4         c4Row
            , const tCIDLib::TCard4         c4Col
        );

        tCIDLib::TBoolean bHandleLVMsg
        (
            const   tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::TMsgResult&  mresRet
        );

        tCIDLib::TVoid CheckRowIndex
        (
            const   tCIDLib::TCard4         c4ToCheck
            , const tCIDLib::TCard4         c4Line
        )   const;

        tCIDLib::TVoid LoadColumns();

        tCIDLib::TVoid RemoveColumns();

        tCIDLib::TVoid SortList();

        tCIDLib::TVoid UpdateAutoLast();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bSortAsc
        //      If we support auto-column sorting, we will flip the sort order if they click
        //      the header of the column that we are already sorted by. So we need to remember
        //      what the current order is. If they select a new column, we set this initially
        //      to ascending.
        //
        //  m_c4SearchCol
        //      Defaults to 0, but they can change this to have us do searches against
        //      another column if desired. It is set in the process of setting the
        //      columns. This is separate from the sort column. We can search non-sorted
        //      columns, it's just a lot less efficient.
        //
        //  m_c4SortCol
        //      If sorting is enabled, this indicates the column we are sorting by.
        //      Defaults to 0, but can be changed. It's only used if the Sorted style
        //      is on. If ColSort style is also enabled, this is only the initial sort
        //      column, but the user can change it via the column headers. If sorting is
        //      not enabled, this is meaningless and attempts to read/write it will throw.
        //
        //  m_colList
        //      We manage the data for the list, and this is our list data. The row data is
        //      always just a string per column, so we always store data as text. Plus
        //      we keep up with a bit of other info per row as well, the per-row id and
        //      per-row user data.
        //
        //  m_colColTitles
        //      The list of column titles we were given, which also tells us the number
        //      of columns. Since we owner draw the headers, we need this for that as well.
        //
        //  m_eCustOpts
        //      Custom drawing options. Default to none. Set to get custom
        //      drawing callbacks.
        //
        //  m_eMCLBStyles
        //      The class specific styles set on us, in our form.
        //
        //  m_hwndListView
        //      See class comments above. The listview has significant issues and we work
        //      around some by creating a nested listview window inside us.
        //
        //  m_pfnSortComp
        //      We have our own internal sorting methods, but
        //
        //  m_pipeiAuto
        //      If this is set, then we do auto-inplace editing, and this is used
        //      to allow the parent window (usually) handle validation and storage
        //      of changed values. We don't own it.
        //
        //  m_rgbCustBgn
        //  m_rgbCustText
        //      These are temps for use by the custom drawing handler, so it doesn't
        //      have to create temps on every call, even if not needed.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean           m_bSortAsc;
        tCIDLib::TCard4             m_c4SearchCol;
        tCIDLib::TCard4             m_c4SortCol;
        TListData                   m_colList;
        tCIDLib::TStrList           m_colColTitles;
        tCIDCtrls::EMCLBCustOpts    m_eCustOpts;
        tCIDCtrls::EMCLBStyles      m_eMCLBStyles;
        tCIDCtrls::EWndStyles       m_eOrgStyles;
        tCIDCtrls::TWndHandle       m_hwndListView;
        MMCLBIPEIntf*               m_pipeiAuto;
        TRGBClr                     m_rgbCustBgn;
        TRGBClr                     m_rgbCustText;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMultiColListBox, TListWnd)
};

#pragma CIDLIB_POPPACK


