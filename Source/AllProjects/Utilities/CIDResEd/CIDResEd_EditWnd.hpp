//
// FILE NAME: CIDResEd_EditWnd.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/17/2000
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
//  We have to create three windows here. One is a container that is what we put
//  into the scroll area window. Within it is a passive display window that contains
//  the widgets. It just does what it is told. On top of that is a transparent
//  window that is the actual editor and handles all of the user interaction and
//  drawing of selection handles.
//
//  The container window keeps the other two sized to fit itself. Though the
//  transparent window handles mouse input, when the hit is on the right/bottom
//  sides or the LR corner, it passes those through to the container, because the
//  container is the one that has to handle sizing of the dailog contents. Otherwise
//  the transparent window just looks at what is under it in the display window
//  and does hit testing on those controls.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


class TResEdCont;

// ---------------------------------------------------------------------------
//   CLASS: TResEdDisplay
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class TResEdDisplay : public TWindow
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TResEdDisplay();

        TResEdDisplay(const TResEdDisplay&) = delete;

        ~TResEdDisplay();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TResEdDisplay& operator=(const TResEdDisplay&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Create
        (
            const   TResEdCont&             wndParent
            , const tCIDCtrls::TWndId       widThis
        );

        tCIDLib::TVoid DeleteCtrls
        (
            const   tCIDResEd::TDlgItemRefList& colDelete
        );

        tCIDLib::TVoid LoadDialog
        (
            const   TResEdDlgDesc&          reddLoad
        );

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid ResizeCtrl
        (
            const   TResEdDlgItem&          reditTar
            , const TArea&                  areaNew
        );

        TWindow& wndByEditId
        (
            const   tCIDLib::TCard4         c4EditId
        );


    private :
        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TResEdDisplay, TWindow)
};




// ---------------------------------------------------------------------------
//   CLASS: TResEdEditor
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class TResEdEditor : public TWindow, public MIPEIntf
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TResEdEditor();

        TResEdEditor(const TResEdEditor&) = delete;

        ~TResEdEditor();


        // -------------------------------------------------------------------
        //  Public opeators
        // -------------------------------------------------------------------
        TResEdEditor& operator=(const TResEdEditor&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bProcessAccel
        (
            const   tCIDLib::TVoid* const   pMsgData
        )   const   override;

        tCIDLib::TBoolean bIPEValidate
        (
            const   TString&                strSrc
            ,       TAttrData&              adatTar
            , const TString&                strNewVal
            ,       TString&                strErrMsg
            ,       tCIDLib::TCard8&        c8UserId
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bChanges() const;

        tCIDLib::TVoid CommitChanges();

        tCIDLib::TVoid Create
        (
            const   TResEdCont&             wndParent
            , const tCIDCtrls::TWndId       widThis
            ,       TResEdDisplay* const    pwndDisplay
            ,       TResAttrEdPane* const   pwndAttrPane
        );

        tCIDLib::TVoid EditDlg
        (
            const   TResEdDlgDesc&          reddEdit
        );

        tCIDLib::TVoid SaveChanges
        (
                    TResEdData&             redatTar
        );

        tCIDLib::TVoid Reset();


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

        tCIDLib::TBoolean bEraseBgn
        (
                    TGraphDrawDev&          gdevToUse
        )   override;

        tCIDLib::TBoolean bMouseMove
        (
            const   TPoint&                 pntAt
            , const tCIDLib::TBoolean       bCtrlShift
            , const tCIDLib::TBoolean       bShift
        )   override;

        virtual tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;

        tCIDLib::TVoid MenuCommand
        (
            const   tCIDLib::TResId         ridItem
            , const tCIDLib::TBoolean       bChecked
            , const tCIDLib::TBoolean       bEnabled
        )   override;

        tCIDLib::TVoid MouseExit() override;



    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        TArea areaAllBounds() const;

        TArea areaAllBounds
        (
            const   tCIDLib::TInt4          i4XOfs
            , const tCIDLib::TInt4          i4YOfs
            , const tCIDLib::TBoolean       bOrigin
        )   const;

        TArea areaSelBounds() const;

        TArea areaSelBounds
        (
            const   tCIDLib::TInt4          i4XOfs
            , const tCIDLib::TInt4          i4YOfs
            , const tCIDLib::TBoolean       bOrigin
        )   const;

        tCIDLib::TBoolean bDragItems
        (
            const   TPoint&                 pntAt
        );

        tCIDLib::TBoolean bDragWndSize
        (
            const   TPoint&                 pntAt
        );

        tCIDLib::TBoolean bIsSelected
        (
                    TResEdDlgItem* const    preditToCheck
            ,       tCIDLib::TCard4&        c4Index
        );

        tCIDLib::TCard4 c4ToIndex
        (
            const   TResEdDlgItem&          reditFind
        );

        tCIDLib::TVoid CopyCutSelected
        (
            const   tCIDLib::TBoolean       bCut
        );

        tCIDLib::TVoid DeleteSelected();

        tCIDLib::TVoid DeselectAll();

        tCIDLib::TVoid DeselectOne
        (
            const   tCIDLib::TCard4         c4SelIndex
        );

        tCIDLib::TVoid DrawAllSelHandles
        (
                      TGraphDrawDev&        gdevToUse
            , const   TArea&                areaUpdate
        );

        tCIDLib::TVoid DrawSelHandles
        (
                    TGraphDrawDev&          gdevTar
            , const TArea&                  areaTar
            ,       TCosmeticPen&           gpenStroke
        );

        tCIDLib::TVoid DrawSelHandles
        (
            const   TArea&                  areaTar
        );

        tCIDCtrls::EEvResponses eAttrEditHandler
        (
                    TAttrEditInfo&          wnotInfo
        );

        tCIDCtrls::ESysPtrs ePtrForArea
        (
            const   tCIDCtrls::EWndAreas    eArea
            , const tCIDLib::TBoolean       bOverWindow
        )   const;

        tCIDLib::TVoid EdRedraw
        (
            const   TArea&                  areaTar
        );

        tCIDLib::TVoid EdRedraw
        (
            const   TGUIRegion&             grgnTar
        );

        tCIDLib::TVoid LoadDlgAttrs();

        tCIDLib::TVoid LoadItemAttrs
        (
            const   TResEdDlgItem&          reditSrc
        );

        tCIDLib::TVoid NudgeSelItems
        (
            const   tCIDLib::TInt4          i4XOfs
            , const tCIDLib::TInt4          i4YOfs
            , const tCIDLib::TBoolean       bOrigin
        );

        TResEdDlgItem* preditHitTest
        (
            const   TPoint&                 pntTest
            ,       tCIDLib::TCard4&        c4Index
        );

        tCIDLib::TVoid PasteItems
        (
            const   TPoint&                 pntAt
        );

        tCIDLib::TVoid PopupMenu
        (
            const   TPoint&                 pntAt
        );

        tCIDLib::TResId ridNextAvail() const;

        tCIDLib::TVoid ResizeOneItem
        (
            const   tCIDLib::TCard4         c4SelInd
            , const TArea&                  areaNewDU
        );

        tCIDLib::TVoid SelectIntersecting
        (
            const   TArea&                  areaTest
        );

        tCIDLib::TVoid SelectItem
        (
                    TResEdDlgItem* const    preditSel
        );

        tCIDLib::TVoid UpdateItemAttrs();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_accelEd
        //      Our accelerator table. If we have the focus and we have a dialog
        //      open, then we process it in case it's one of our hot keys.
        //
        //  m_bHideSelHandles
        //      Sometimes we need to hide selection handles while we do something.
        //      So we can set this, redraw the area (bPaint will no redraw the
        //      handles), do we what we need to do, then clear this flag and redraw
        //      the area again.
        //
        //  m_c4EditId
        //      The edit id of the dialog we are editing. If none, it is max card.
        //
        //  m_colSelList
        //      The selection list, into which we put pointesr to the dialog item
        //      for each control that is selected.
        //
        //  m_pwndAttrPane
        //      We get a pointer to the attribute editor pane, so that we can
        //      load up info into the attribute editor windows he contains.
        //
        //  m_pwndDisplay
        //      We get a pointer to the display window so that we can invoke methods
        //      on him to manage the controls.
        //
        //  m_reddEdit
        //      The dialog description that we are editing. This is a copy of
        //      the original. It'll only get written back if we save it.
        //
        //  m_reddOrg
        //      A copy of the original incoming, so that we can undo any
        //      changes and go back if want.
        // -------------------------------------------------------------------
        TAccelTable                 m_accelEd;
        tCIDLib::TBoolean           m_bHideSelHandles;
        tCIDLib::TCard4             m_c4EditId;
        tCIDResEd::TDlgItemRefList  m_colSelList;
        TResAttrEdPane*             m_pwndAttrPane;
        TResEdDisplay*              m_pwndDisplay;
        TResEdDlgDesc               m_reddEdit;
        TResEdDlgDesc               m_reddOrg;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TResEdEditor, TWindow)
};



// ---------------------------------------------------------------------------
//   CLASS: TResEdCont
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class TResEdCont : public TWindow
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TResEdCont();

        TResEdCont(const TResEdCont&) = delete;

        ~TResEdCont();



        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TResEdCont& operator=(const TResEdCont&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bProcessAccel
        (
            const   tCIDLib::TVoid* const   pMsgData
        )   const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bChanges() const;

        tCIDLib::TVoid Create
        (
            const   TScrollArea&            wndParent
            , const tCIDCtrls::TWndId       widThis
            ,       TResAttrEdPane* const   pwndAttrPane
        );

        tCIDLib::TVoid EditDlg
        (
            const   TResEdDlgDesc&          reddEdit
        );

        tCIDLib::TVoid Reset
        (
            const   tCIDLib::TBoolean       bHide
        );

        TResEdEditor& wndEditor();


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
        )  override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pwndDisplay
        //  m_pwndEditor
        //      Our child windows, which we create overlaying each other exactly
        //      and keep them that way.
        // -------------------------------------------------------------------
        TResEdDisplay*  m_pwndDisplay;
        TResEdEditor*   m_pwndEditor;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TResEdCont, TWindow)
};


