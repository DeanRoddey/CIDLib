//
// FILE NAME: CIDResEd_EditAttr.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/2015
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
//  We have to create derivatives of the attribute editing window, so that we can
//  handle custom validation and custom value entry. We create one for dialog
//  items and one for the dailog itself.
//
//  We also create a container window that in turn creates the two attr edit
//  windows as children of itself. The main window makes this container the right
//  side panel. The container derives from a standard helper class in CIDWUtils
//  that makes it easy to deal with managed pane children.
//
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//   CLASS: TResDlgAttrEd
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class TResDlgAttrEd : public TAttrEditWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TResDlgAttrEd();

        TResDlgAttrEd(const TResDlgAttrEd&) = delete;

        ~TResDlgAttrEd();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TResDlgAttrEd& operator=(const TResDlgAttrEd&) = delete;



    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bEditValue
        (
                    TAttrData&              adataEdit
            , const TArea&                  areaCell
            , const tCIDLib::TCard8         c8UserId
        )   override;

        tCIDLib::TBoolean bVisEdit
        (
                    TAttrData&              adataEdit
            , const TArea&                  areaCellScr
            , const tCIDLib::TCard8         c8UserId
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TResDlgAttrEd, TAttrEditWnd)
};



// ---------------------------------------------------------------------------
//   CLASS: TResItemAttrEd
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class TResItemAttrEd : public TAttrEditWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TResItemAttrEd();

        TResItemAttrEd(const TResItemAttrEd&) = delete;

        ~TResItemAttrEd();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TResItemAttrEd& operator=(const TResItemAttrEd&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bEditValue
        (
                    TAttrData&              adataEdit
            , const TArea&                  areaCell
            , const tCIDLib::TCard8         c8UserId
        )   override;

        tCIDLib::TBoolean bVisEdit
        (
                    TAttrData&              adataEdit
            , const TArea&                  areaCellScr
            , const tCIDLib::TCard8         c8UserId
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TResItemAttrEd, TAttrEditWnd)
};



// ---------------------------------------------------------------------------
//   CLASS: TResAttrEdPane
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class TResAttrEdPane : public TPaneWndCont
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TResAttrEdPane();

        TResAttrEdPane(const TResAttrEdPane&) = delete;

        ~TResAttrEdPane();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TResAttrEdPane& operator=(const TResAttrEdPane&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TResDlgAttrEd& wndDlgAttrs();

        TResItemAttrEd& wndItemAttrs();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pwndDlgAttrs
        //  m_pwndItemAttrs
        //      Our child windows. The dialog editing pane is put on top and the
        //      item editing pane is on the bottom.
        // -------------------------------------------------------------------
        TResDlgAttrEd*      m_pwndDlgAttrs;
        TResItemAttrEd*     m_pwndItemAttrs;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TResAttrEdPane, TPaneWndCont)
};


