//
// FILE NAME: CIDCtrls_MCColListBoxImpl_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/07/2017
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
//  This is the internal implementation of the multi-column list box control. Because it is
//  fairly complex and we want to minimize rebuild overhead on clients and such, and since
//  we already were for good design reasons keeping the actual content display in a separate
//  sub-window, we can keep this internal. The main window creates a header control and one
//  of these and just keeps them sized and positioned appropriately. This means we can also
//  keep our row and column classes internal as well.
//
//  See the main window class' comments for big picture details.
//
//  We treat rows by reference because we need to be able to resort them quickly and
//  efficiently. Column order is fixed once set, so those we do by value. Currently the
//  only data for a cell is the value, so we just do a string list, to keep it light.
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


// ---------------------------------------------------------------------------
//  CLASS: TMCLBRow
// PREFIX: mclbr
// ---------------------------------------------------------------------------
class TMCLBRow
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMCLBRow() = delete;

        TMCLBRow
        (
                    TMColListBoxImpl&       wndCont
            , const tCIDLib::TCard4         colCount
        );

        TMCLBRow(const TMCLBRow&) = delete;

        ~TMCLBRow();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TMCLBRow& operator=(const TMCLBRow&) = delete;


        // -------------------------------------------------------------------
        //  Public data members
        //
        //  m_bTitle
        //      Used to indicate a title row. In this case, only the 0th cell is used and
        //      is assumed to contain title text. This row is usually set to a different
        //      background color, and it is always drawn in bold font.
        //
        //  m_colCells
        //      Our list of cells for this row. The count is driven by the number of columns
        //      that have been set on the list box window.
        //
        //  m_rgbBgn
        //      The background color for this row. It will initially be set to the window's
        //      background fill color.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean   m_bTitle;
        tCIDLib::TStrList   m_colCells;
        TRGBClr             m_rgbBgn;
};



// ---------------------------------------------------------------------------
//  CLASS: TMColListBoxImpl
// PREFIX: wnd
// ---------------------------------------------------------------------------
class TMColListBoxImpl : public TWindow
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TMColListBoxImpl();

        TMColListBoxImpl(const TMColListBoxImpl&) = delete;

        ~TMColListBoxImpl();


        // -------------------------------------------------------------------
        //  Public oeprators
        // -------------------------------------------------------------------
        TMColListBoxImpl& operator=(const TMColListBoxImpl&) = delete;


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CreateMCLBImpl
        (
                    TMColListBox&           wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDLib::TCard4         c4RowHeight
        );

        tCIDLib::TVoid RemoveAll();

        tCIDLib::TVoid SetColumns
        (
            const   TMColListBox::TColDefs& colToSet
        );


    protected :
        // -------------------------------------------------------------------
        //  The parent is our friend
        // -------------------------------------------------------------------
        friend class TMColListBox;


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
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



    private :
        // -------------------------------------------------------------------
        //  Private data types
        // -------------------------------------------------------------------
        typedef TRefVector<TMCLBRow>        TRowList;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4RowHeight
        //      The row height, which drives a lot of stuff. It's based on the font height
        //      and given to us by the parent window.
        //
        //  m_colCols
        //      Our current column layout. This tells us how many columns, how they are
        //      to be justified, colors to use, and so forth.
        //
        //  m_colRows
        //      Our list rows. It's an adopting by ref vector of row objects, so we can
        //      re-sort it very efficiently.
        //
        //  m_pwndParent
        //      We keep a pointer to our parent window since we need to talk to him quite
        //       a bit.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4RowHeight;
        TMColListBox::TColDefs  m_colCols;
        TRowList                m_colRows;
        TMColListBox*           m_pwndParent;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TMColListBoxImpl, TWindow)
};


#pragma CIDLIB_POPPACK
