//
// FILE NAME: CIDCtrls_PaneManager.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/18/2015
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
//  This file implements a class that provides assistance to windows that want to
//  manage 1, 2, or 3 child windows in common tiled pane layouts. All the window
//  has to do is tell us about the layout and windows that make up the panes and
//  call us any time his size changes. This guy resizes the windows appropriately.
//
//  The layout is controlled by a derivative of an ABC TPaneLayout. It provides
//  virtual callbacks that derived classes can implement. We provide standard
//  implementations for some common layouts. The client code creates a derivative and
//  passes it in to be adopted.
//
//  We also provide help for user adjustment of the pane divisions, by providing
//  a hit testing call, and a modal dragging method that does the actual dragging.
//  This minimizes the hit to client apps for supporting this functionality.
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
//   CLASS: TPaneWnd
//  PREFIX: panew
// ---------------------------------------------------------------------------
class TPaneWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructor
        // -------------------------------------------------------------------
        TPaneWnd();

        TPaneWnd
        (
                    TWindow* const          pwndMng
            , const TSize&                  szMin
            , const TSize&                  szInit
        );

        TPaneWnd
        (
            const   TPaneWnd&               panewSrc
        );

        ~TPaneWnd();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPaneWnd& operator=
        (
            const   TPaneWnd&               panewSrc
        );

        tCIDLib::TVoid operator=
        (
            const   TArea&                  areaToSet
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TArea& areaCur() const;

        const TArea& areaPrev() const;

        tCIDLib::TBoolean bNewArea
        (
                    TArea&                  areaNewAdj
            ,       TArea&                  areaPrevAdj
        );

        tCIDLib::TCard4 c4Min
        (
            const   tCIDLib::TBoolean       bHorz
        )   const;

        tCIDLib::TCard4 c4Size
        (
            const   tCIDLib::TBoolean       bHorz
        )   const;

        tCIDLib::TInt4 i4Bottom() const;

        tCIDLib::TInt4 i4Left() const;

        tCIDLib::TInt4 i4Right() const;

        tCIDLib::TInt4 i4Top() const;

        tCIDLib::TVoid JustifyIn
        (
            const   TArea&                  areaTar
            , const tCIDLib::TBoolean       bHorz
        );

        TWindow* pwndPane();

        const TWindow* pwndPane() const;

        const TSize& szInit() const;

        const TSize& szMin() const;

        tCIDLib::TVoid SetBottom
        (
            const   tCIDLib::TInt4          i4ToSet
        );

        tCIDLib::TVoid SetHeight
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid SetLeft
        (
            const   tCIDLib::TInt4          i4ToSet
        );

        tCIDLib::TVoid SetRight
        (
            const   tCIDLib::TInt4          i4ToSet
        );

        tCIDLib::TVoid SetTop
        (
            const   tCIDLib::TInt4          i4ToSet
        );

        tCIDLib::TVoid SetWidth
        (
            const   tCIDLib::TCard4         c4ToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        TArea       m_areaCur;
        TArea       m_areaPrev;
        TWindow*    m_pwndPane;
        TSize       m_szInit;
        TSize       m_szMin;
};



//
//  Slip in a list of pane windows. All of the rest of the classes below need to
//  see this type.
//
namespace tCIDCtrls
{
    typedef TVector<TPaneWnd>       TPaneList;
}


// ---------------------------------------------------------------------------
//   CLASS: TPaneLayout
//  PREFIX: play
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TPaneLayout
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPaneLayout(const TPaneLayout&) = delete;

        virtual ~TPaneLayout();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPaneLayout& operator=(const TPaneLayout&) = delete;



        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid AdjustPanes
        (
                    tCIDCtrls::TPaneList&   colPanes
            , const TArea&                  areaNew
            , const TArea&                  areaOld
            , const tCIDLib::TBoolean       bEmphDir
        ) = 0;

        virtual tCIDLib::TBoolean bCheckOnDiv
        (
            const   tCIDCtrls::TPaneList&   colPanes
            , const TPoint&                 pntAt
            ,       tCIDLib::TCard4&        c4First
            ,       tCIDLib::TCard4&        c4Sec
            ,       tCIDLib::TBoolean&      bHorz
        )   const = 0;

        virtual tCIDLib::TBoolean bDragAdjust
        (
                    tCIDCtrls::TPaneList&   colPanes
            , const tCIDLib::TCard4         c4First
            , const tCIDLib::TCard4         c4Sec
            , const TPoint&                 pntAt
            , const TArea&                  areaAvail
        ) = 0;

        virtual tCIDLib::TBoolean bVertDrag() const = 0;

        virtual tCIDLib::TCard4 c4PaneCount() const = 0;

        virtual tCIDLib::TVoid InitPanes
        (
                    tCIDCtrls::TPaneList&   colPanes
            , const TArea&                  areaAvail
            , const tCIDLib::TBoolean       bEmphDir
        ) = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TPaneLayout();
};


// ---------------------------------------------------------------------------
//   CLASS: TPaneLayout2Horz
//  PREFIX: play
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TPaneLayout2Horz : public TPaneLayout
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPaneLayout2Horz();

        TPaneLayout2Horz(const TPaneLayout2Horz&) = delete;

        ~TPaneLayout2Horz();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPaneLayout& operator=(const TPaneLayout&) = delete;



        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdjustPanes
        (
                    tCIDCtrls::TPaneList&   colPanes
            , const TArea&                  areaNew
            , const TArea&                  areaOld
            , const tCIDLib::TBoolean       bEmphDir
        )   override;

        tCIDLib::TBoolean bCheckOnDiv
        (
            const   tCIDCtrls::TPaneList&   colPanes
            , const TPoint&                 pntAt
            ,       tCIDLib::TCard4&        c4First
            ,       tCIDLib::TCard4&        c4Sec
            ,       tCIDLib::TBoolean&      bHorz
        )   const override;

        tCIDLib::TBoolean bDragAdjust
        (
                    tCIDCtrls::TPaneList&   colPanes
            , const tCIDLib::TCard4         c4First
            , const tCIDLib::TCard4         c4Sec
            , const TPoint&                 pntAt
            , const TArea&                  areaAvail
        )   override;

        tCIDLib::TBoolean bVertDrag() const override;

        tCIDLib::TCard4 c4PaneCount() const override;

        tCIDLib::TVoid InitPanes
        (
                    tCIDCtrls::TPaneList&   colPanes
            , const TArea&                  areaAvail
            , const tCIDLib::TBoolean       bEmphDir
        )   override;
};



// ---------------------------------------------------------------------------
//   CLASS: TPaneLayout2Vert
//  PREFIX: play
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TPaneLayout2Vert : public TPaneLayout
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPaneLayout2Vert();

        TPaneLayout2Vert(const TPaneLayout2Vert&) = delete;

        ~TPaneLayout2Vert();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPaneLayout& operator=(const TPaneLayout&) = delete;



        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdjustPanes
        (
                    tCIDCtrls::TPaneList&   colPanes
            , const TArea&                  areaNew
            , const TArea&                  areaOld
            , const tCIDLib::TBoolean       bEmphDir
        )   override;

        tCIDLib::TBoolean bCheckOnDiv
        (
            const   tCIDCtrls::TPaneList&   colPanes
            , const TPoint&                 pntAt
            ,       tCIDLib::TCard4&        c4First
            ,       tCIDLib::TCard4&        c4Sec
            ,       tCIDLib::TBoolean&      bHorz
        )   const override;

        tCIDLib::TBoolean bDragAdjust
        (
                    tCIDCtrls::TPaneList&   colPanes
            , const tCIDLib::TCard4         c4First
            , const tCIDLib::TCard4         c4Sec
            , const TPoint&                 pntAt
            , const TArea&                  areaAvail
        )   override;

        tCIDLib::TBoolean bVertDrag() const override;

        tCIDLib::TCard4 c4PaneCount() const override;

        tCIDLib::TVoid InitPanes
        (
                    tCIDCtrls::TPaneList&   colPanes
            , const TArea&                  areaAvail
            , const tCIDLib::TBoolean       bEmphDir
        )   override;
};



// ---------------------------------------------------------------------------
//   CLASS: TPaneLayout3Vert
//  PREFIX: play
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TPaneLayout3Vert : public TPaneLayout
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPaneLayout3Vert();

        TPaneLayout3Vert(const TPaneLayout3Vert&) = delete;

        ~TPaneLayout3Vert();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPaneLayout& operator=(const TPaneLayout&) = delete;



        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdjustPanes
        (
                    tCIDCtrls::TPaneList&   colPanes
            , const TArea&                  areaNew
            , const TArea&                  areaOld
            , const tCIDLib::TBoolean       bEmphDir
        )   override;

        tCIDLib::TBoolean bCheckOnDiv
        (
            const   tCIDCtrls::TPaneList&   colPanes
            , const TPoint&                 pntAt
            ,       tCIDLib::TCard4&        c4First
            ,       tCIDLib::TCard4&        c4Sec
            ,       tCIDLib::TBoolean&      bHorz
        )   const override;

        tCIDLib::TBoolean bDragAdjust
        (
                    tCIDCtrls::TPaneList&   colPanes
            , const tCIDLib::TCard4         c4First
            , const tCIDLib::TCard4         c4Sec
            , const TPoint&                 pntAt
            , const TArea&                  areaAvail
        )   override;

        tCIDLib::TBoolean bVertDrag() const override;

        tCIDLib::TCard4 c4PaneCount() const override;

        tCIDLib::TVoid InitPanes
        (
                    tCIDCtrls::TPaneList&   colPanes
            , const TArea&                  areaAvail
            , const tCIDLib::TBoolean       bEmphDir
        )   override;
};




// ---------------------------------------------------------------------------
//  CLASS: TPaneWndCont
// PREFIX: pwm
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TPaneWndCont : public TWindow
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPaneWndCont();

        TPaneWndCont(const TPaneWndCont&) = delete;

        ~TPaneWndCont();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPaneWndCont& operator=(const TPaneWndCont&) = delete;


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
        tCIDLib::TCard4 c4PaneCount() const;

        tCIDLib::TVoid  CreatePaneCont
        (
            const  TWindow&                 wndParent
            , const TArea&                  areaInit
            , const tCIDCtrls::TWndId       widThis
            , const tCIDCtrls::EWndStyles   eStyles = tCIDCtrls::EWndStyles::PaneContainer
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::ControlParent
        );

        tCIDLib::TVoid InitPanes
        (
            const   tCIDLib::TBoolean       bEmphDir
        );

        TWindow* pwndPaneAt
        (
            const   tCIDLib::TCard4         c4At
        );

        tCIDLib::TVoid SetLayout
        (
                    TPaneLayout* const      playToAdopt
        );

        tCIDLib::TVoid SetPane
        (
                    TWindow* const          pwndAdd
            , const tCIDLib::TCard4         c4PaneInd
            , const TSize&                  szMin
            , const TSize&                  szInit
        );

        tCIDLib::TVoid ShowPanes();


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
        );

        tCIDLib::TBoolean bMouseMove
        (
            const   TPoint&                 pntAt
            , const tCIDLib::TBoolean       bCtrlShift
            , const tCIDLib::TBoolean       bShift
        )   override;

        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AdjustForDiv
        (
                    TArea&                  areaAdjust
            , const tCIDLib::TBoolean       bVertDivs
            , const tCIDLib::TCard4         c4PaneCnt
            , const tCIDLib::TCard4         c4PaneInd
        );

        tCIDLib::TVoid AdjustWnds
        (
            const   TArea&                  areaNew
            , const TArea&                  areaOld
        );

        tCIDLib::TBoolean bCheckOnDiv
        (
            const   TPoint&                 pntAt
            ,       tCIDLib::TCard4&        c4First
            ,       tCIDLib::TCard4&        c4Sec
            ,       tCIDLib::TBoolean&      bHorz
        )   const;

        tCIDLib::TVoid DragDivider
        (
                    TWindow&                wndParent
            , const TArea&                  areaAvail
            , const TPoint&                 pntAt
            , const tCIDLib::TCard4         c4First
            , const tCIDLib::TCard4         c4Sec
            , const tCIDLib::TBoolean       bHorz
        );

        tCIDLib::TVoid MinPaneAdjust
        (
                    TGraphDrawDev&          gdevFill
            , const tCIDLib::TCard4         c4First
            , const tCIDLib::TCard4         c4Sec
            , const TArea&                  areaAvail
            , const tCIDLib::TBoolean       bHorz
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaTmp
        //  m_areaTmp2
        //      For use in doing calculations. Don't assume it'll stay the same across
        //      any calls.
        //
        //  m_bEmphDir
        //      We keep the emphasis direction value around so we can pass it back to
        //      the layout upon resize, so that they can continue to give the correct
        //      emphasis. True means the right/bottom side is the main area, and false
        //      means the left/top is.
        //
        //  m_colPanes
        //      The list of pane windows we manage.
        //
        //  m_playMgr
        //      Our layout manager. It is a plugin interface that the user provides. We
        //      adopt it and destroy it when we are destroyed.
        // -------------------------------------------------------------------
        TArea                   m_areaTmp;
        TArea                   m_areaTmp2;
        tCIDLib::TBoolean       m_bEmphDir;
        tCIDCtrls::TPaneList    m_colPanes;
        TPaneLayout*            m_playMgr;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TPaneWndCont,TWindow)
};

#pragma CIDLIB_POPPACK



