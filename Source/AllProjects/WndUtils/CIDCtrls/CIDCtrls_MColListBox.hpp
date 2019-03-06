//
// FILE NAME: CIDCtrls_MCColListBox.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/28/2017
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  The list view control, if you want to custom draw it and if you are using buffered
//  drawing, which mostly all client applications will for performance, is just a huge
//  piece of crap. It's almost impossible to get it working without major drawing artifacts
//  and it's just one hack on top of another.
//
//  So, after suffering for a year or so with that dog, I decided to just create my own.
//  It's a bit complicated but ultimately worth it because this multi-column list box
//  type interface is used all over the place, with in-place editing often to create
//  a very simple but convenient configuration or settings interface.
//
//  To make things gigantically simpler, we create two child controls, one that displays
//  the headers and one that display sthe grid of items, plus h/v scroll bars. So, at the
//  highest level we just create those and size and position them accoringly within ourself.
//  They handle their own affairs.
//
//  Data:
//
//  We store the data ourself, as text. So we have a vector of vectors of strings basically.
//  The outer vector is the rows, and the inner vector is the columns. We keep more than
//  just the text. Each row object can indicate a background color and bold/normal text
//  style. A row can also be marked as a label row, in which case only the first column
//  is used, and is assumed to be label text, which is drawn bold and isn't limited to the
//  width of the actual first column.
//
//  Justification of the column text is driven by the header control. So each column matches
//  the justification of the assocated header slot.
//
//  We provide a simple class to describe a column. The client code sets up a collection of
//  these and calls SetColumns() to set up our columns and tell us how to draw them.
//
//  Header:
//
//  The header control sends us notifications about resizing of columns and selection of
//  columns (which is used in sortable lists to select a new sort column.) We use a generic
//  header object (which is also one of our custom classes.)
//
//  Sorting:
//
//  For sorting we use a simple abstract class that client code can create derivatives
//  of as desired. We provide a default one that just sorts (using the raw column text) of
//  a single column. They can create ones that understand what the underlying data represents
//  and sort by non-textual ordering, and also they can sort by main and sub-column for two
//  level sorting, and whatever else they choose.
//
//  Drawing:
//
//  Obviously we have way more text than we can show. So we have a 'virtual drawing space' the
//  actual size required and just store an origin that controls how far up and left that
//  virtual space is shifted. So drawing means finding the row/column that first is visible in
//  the upper left (often partially) and the one in the lower right. That provides a rectangle
//  of cells that could possibly be visible. We then redraw the ones of those that have been
//  invalidated.
//
//  So we have core helpers that calculate those LF/LR visible cells, that calculate the area
//  of a given cell, figure out the row/column of the cell (if any) that falls under a given
//  point (for hit testing), calculate the visible area of a whole row or column, and so forth.
//
//  As mentioned above we have two children, a header and the display window. We don't draw
//  into our own area because it vastly complicates things. Doing it this way, we don't have to
//  deal with the fact that not all our window is visible (header and scroll bars), the area of
//  the displary window is the display area. To further simplify things, we keep the scroll bars
//  visible all the time, and they are part of the main window, not the display window. So the
//  size of the display window doesn't have to change if we move from too much too much to display
//  at once to too little, or vice versa. And of course it also means that we again don't have
//  to deal with the fact that not all of the display area is actually available.
//
//  The scroll bars are just enabled when needed and disabled whend not. We keep our scroll
//  position in the actual scroll bars, so that we don't have two copies of it to keep in sync.
//  If a bar is disabled, then we know the scroll position is zero. If not, then we take the
//  values of the two scroll bars and invert them to negative values and that is our virtual
//  display space origin relative to the display window.
//
//  Management:
//
//  The main window really doesn't do much other than enable/disable the scroll bars and pass
//  on incoming commands as required. The display window handles all of the real work, and we
//  can keep it internal which vastly reduces our public API footprint and need for rebuilds.
//  It means all our row and column classes can be internal as well.
//
//  We just have to be careful to make it look like our main window is the actual control to
//  the outside world. So the display window tells our main window about what is happening, and
//  the main window sends out notifications as necessary. It also handles events from the header
//  and passes them to the display window. And it provides the public API most of which is just
//  passthroughs to the display window.
//
//  Since the display window is pretty complicated and we want to keep a very clean separation
//  we have a separate internal only header/implementation file for that. We just forward ref
//  the display window class here.
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

class TMColListBox;
class TMColListBoxImpl;


// ---------------------------------------------------------------------------
//  CLASS: TMCLBColDef
// PREFIX: mclbcd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TMCLBColDef
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMCLBColDef();

        TMCLBColDef
        (
            const   TString&                strTitle
            , const tCIDLib::EHJustify      eHJustify
            , const tCIDLib::TCard4         c4Width = 24
            , const tCIDLib::TBoolean       bBold = kCIDLib::False
        );

        TMCLBColDef
        (
            const   TString&                strTitle
            , const tCIDLib::EHJustify      eHJustify
            , const TRGBClr&                rgbBgnClr
            , const TRGBClr&                rgbTextClr
            , const tCIDLib::TCard4         c4Width = 24
            , const tCIDLib::TBoolean       bBold = kCIDLib::False
        );

        TMCLBColDef
        (
            const   TMCLBColDef&            mclbcdSrc
        );

        ~TMCLBColDef();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMCLBColDef& operator=
        (
            const   TMCLBColDef&            mclbcdSrc
        );


    protected :
        // -------------------------------------------------------------------
        //  The public and impl classes are our friends so that they can see the members
        // -------------------------------------------------------------------
        friend class TMColListBoxImpl;
        friend class TMColListBox;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bBold;
        tCIDLib::TCard4     m_c4Width;
        tCIDLib::EHJustify  m_eHJustify;
        TRGBClr*            m_prgbBgnClr;
        TRGBClr*            m_prgbTextClr;
        TString             m_strTitle;
};



// ---------------------------------------------------------------------------
//  CLASS: TMColListBox
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TMColListBox : public TCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Public class types
        // -------------------------------------------------------------------
        typedef TVector<TMCLBColDef>        TColDefs;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMColListBox();

        TMColListBox(const TMColListBox&) = delete;

        ~TMColListBox();


        // -------------------------------------------------------------------
        //  Public oeprators
        // -------------------------------------------------------------------
        TMColListBox& operator=(const TMColListBox&) = delete;


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

        TSize szDefault() const override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CreateMColLB
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EMCLBStyles  eMCLBStyles = tCIDCtrls::EMCLBStyles::None
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDLib::TVoid SetColumns
        (
            const   TColDefs&               colToSet
        );


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

        tCIDLib::TVoid Destroyed() override;


    private :
        // -------------------------------------------------------------------
        //  Private types and constants
        // -------------------------------------------------------------------
        const tCIDCtrls::TWndId             kWndId_Hdr = kCIDCtrls::widFirstCtrl;
        const tCIDCtrls::TWndId             kWndId_Display = kCIDCtrls::widFirstCtrl + 1;
        const tCIDCtrls::TWndId             kWndId_HScroll = kCIDCtrls::widFirstCtrl + 2;
        const tCIDCtrls::TWndId             kWndId_VScroll = kCIDCtrls::widFirstCtrl + 3;


        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CalcChildAreas
        (
            const   TArea&                  areaAvail
            ,       TArea&                  areaHdr
            ,       TArea&                  areaDisplay
            ,       TArea&                  areaHScroll
            ,       TArea&                  areaVScroll
        );

        tCIDLib::TVoid SetHorzScroll();


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaLRCorner
        //      The only part of our window that is covered by other stuff is the lower right
        //      corner where the two scroll bars come together. So any time we calc the child
        //      areas this is set to that, so we can fill it when neeed.
        //
        //  m_c4FontHeight
        //      A lot of stuff is drivn by the font height, so we get it up front. We pass it
        //      to the display window as well since he will use it even more.
        //
        //  m_eMCLBStyles
        //      Our class specific styles.
        //
        //  m_pwndDisplay
        //      Our private implementation window that handles the actual management and
        //      display of the list cells.
        //
        //  m_pwndHeader
        //      Our column header control.
        //
        //  m_pwndScrollH
        //  m_pwndScrollV
        //      Our h/v scroll bars which we do as separate controls, not as the built in
        //      ones because these we can keep visible.
        // -------------------------------------------------------------------
        TArea                   m_areaLRCorner;
        tCIDLib::TCard4         m_c4FontHeight;
        tCIDCtrls::EMCLBStyles  m_eMCLBStyles;
        TMColListBoxImpl*       m_pwndDisplay;
        TColHeader*             m_pwndHeader;
        TScrollBar*             m_pwndScrollH;
        TScrollBar*             m_pwndScrollV;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMColListBox, TCtrlWnd)
};


#pragma CIDLIB_POPPACK

