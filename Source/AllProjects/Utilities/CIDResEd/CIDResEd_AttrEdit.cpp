//
// FILE NAME: CIDResEd_AttrEdit.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/2015
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
//  Implements our derivative of the attribute editing control window.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "CIDResEd.hpp"



// ---------------------------------------------------------------------------
//  Magic macros
// ---------------------------------------------------------------------------
RTTIDecls(TResDlgAttrEd, TAttrEditWnd)
RTTIDecls(TResItemAttrEd, TAttrEditWnd)
RTTIDecls(TResAttrEdPane, TPaneWndCont)



// ---------------------------------------------------------------------------
//   CLASS: TResDlgAttrEd
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// -------------------------------------------------------------------
//  TResDlgAttrEd: Constructors and Destructor
// -------------------------------------------------------------------
TResDlgAttrEd::TResDlgAttrEd()
{
}

TResDlgAttrEd::~TResDlgAttrEd()
{
}



// -------------------------------------------------------------------
//  TResDlgAttrEd: Protected, inherited methods
// -------------------------------------------------------------------
tCIDLib::TBoolean
TResDlgAttrEd::bEditValue(          TAttrData&      adataEdit
                            , const TArea&          areaCell
                            , const tCIDLib::TCard8 c8UserId)
{
    // If our parent handles it, then we are done
    if (TParent::bEditValue(adataEdit, areaCell, c8UserId))
        return kCIDLib::True;

    // Else we handle it
    return kCIDLib::True;
}


tCIDLib::TBoolean
TResDlgAttrEd::bVisEdit(        TAttrData&      adatEdit
                        , const TArea&          areaCellScr
                        , const tCIDLib::TCard8 c8UserId)
{
    // If our parent handles it, we are done
    if (TParent::bVisEdit(adatEdit, areaCellScr, c8UserId))
        return kCIDLib::True;

    if (adatEdit.strId() == kCIDResEd::strAttr_Title)
    {
        TString strSymbol;
        tCIDLib::TMsgId midMsg = 0;
        tCIDResEd::EMsgTypes eType = facCIDResEd.eParseTextSym
        (
            adatEdit.strValue(), strSymbol
        );

        if (eType < tCIDResEd::EMsgTypes::Count)
        {
            const TResEdMsgItem* premitMsg = facCIDResEd.wndMain().premitFind
            (
                eType, strSymbol
            );

            if (premitMsg)
                midMsg = premitMsg->c4Id();
        }

        TResEdTextDlg dlgEd;
        if (dlgEd.bRunDlg(*this, eType, midMsg, strSymbol, kCIDLib::True))
            adatEdit.SetString(strSymbol);
    }
     else
    {
        return kCIDLib::False;
    }

    return kCIDLib::True;
}




// ---------------------------------------------------------------------------
//   CLASS: TResItemAttrEd
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResItemAttrEd: Constructors and Destructor
// ---------------------------------------------------------------------------
TResItemAttrEd::TResItemAttrEd()
{
}

TResItemAttrEd::~TResItemAttrEd()
{
}


// ---------------------------------------------------------------------------
//  TResItemAttrEd: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TBoolean
TResItemAttrEd::bEditValue(         TAttrData&      adataEdit
                            , const TArea&          areaCell
                            , const tCIDLib::TCard8 c8UserId)
{
    // If our parent handles it, then we are done
    if (TParent::bEditValue(adataEdit, areaCell, c8UserId))
        return kCIDLib::True;

    // Else we handle it


    // Never got handled
    return kCIDLib::False;
}


tCIDLib::TBoolean
TResItemAttrEd::bVisEdit(       TAttrData&      adatEdit
                        , const TArea&          areaCellScr
                        , const tCIDLib::TCard8 c8UserId)
{
    //
    //  Handle the loadable text. We have to do this first, to override the default
    //  handling by the parent class.
    //
    if (adatEdit.strSpecType() == kCIDResEd::strSpecType_LoadText)
    {
        //
        //  Get the current symbol info if any, to pass in as the initial selection
        //  in the dialog box.
        //
        TString strSymbol;
        tCIDLib::TMsgId midMsg = 0;
        tCIDResEd::EMsgTypes eType = facCIDResEd.eParseTextSym
        (
            adatEdit.strValue(), strSymbol
        );

        if (eType < tCIDResEd::EMsgTypes::Count)
        {
            const TResEdMsgItem* premitMsg = facCIDResEd.wndMain().premitFind
            (
                eType, strSymbol
            );

            if (premitMsg)
                midMsg = premitMsg->c4Id();
        }

        TResEdTextDlg dlgEd;
        if (dlgEd.bRunDlg(*this, eType, midMsg, strSymbol, kCIDLib::True))
            adatEdit.SetString(strSymbol);

        return kCIDLib::True;
    }

    // Pass it on to our parents, since it's not something we handle ourself
    return TParent::bVisEdit(adatEdit, areaCellScr, c8UserId);
}




// ---------------------------------------------------------------------------
//   CLASS: TResAttrEdPane
//  PREFIX: wnd
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResAttrEdPane: Constructors and Destructor
// ---------------------------------------------------------------------------
TResAttrEdPane::TResAttrEdPane() :

    m_pwndDlgAttrs(0)
    , m_pwndItemAttrs(0)
{
}

TResAttrEdPane::~TResAttrEdPane()
{
}


// ---------------------------------------------------------------------------
//  TResAttrEdPane: Public, non-virtual methods
// ---------------------------------------------------------------------------

// Provide access to our attribute editor windows
TResDlgAttrEd& TResAttrEdPane::wndDlgAttrs()
{
    CIDAssert(m_pwndDlgAttrs!= nullptr, L"The dlg attr window has not been set yet")
    return *m_pwndDlgAttrs;
}


TResItemAttrEd& TResAttrEdPane::wndItemAttrs()
{
    CIDAssert(m_pwndItemAttrs!= nullptr, L"The item attr window has not been set yet")
    return *m_pwndItemAttrs;
}



// ---------------------------------------------------------------------------
//  TResAttrEdPane: Protected, inherited methods
// ---------------------------------------------------------------------------

tCIDLib::TBoolean TResAttrEdPane::bCreated()
{
    if (!TParent::bCreated())
        return kCIDLib::False;

    // Create the two attribute editor windows
    m_pwndDlgAttrs = new TResDlgAttrEd();
    m_pwndDlgAttrs->CreateAttrEd
    (
        *this
        , kCIDCtrls::widFirstCtrl
        , TArea(0, 0, 16, 16)
        , tCIDCtrls::EWndStyles::VisTabChild
    );
    m_pwndDlgAttrs->strName(kCIDResEd::strWnd_DlgAttrEd);

    m_pwndItemAttrs = new TResItemAttrEd();
    m_pwndItemAttrs->CreateAttrEd
    (
        *this
        , kCIDCtrls::widFirstCtrl + 1
        , TArea(0, 0, 16, 16)
        , tCIDCtrls::EWndStyles::VisTabChild
    );
    m_pwndItemAttrs->strName(kCIDResEd::strWnd_ItemAttrEd);

    //
    //  Set up the main window manager. We install a standard 2 horizontal panes
    //  layout manager.
    //
    SetLayout(new TPaneLayout2Horz);
    SetPane(m_pwndDlgAttrs, 0, TSize(0, 64), TSize(0, 128));
    SetPane(m_pwndItemAttrs, 1, TSize(0, 64), TSize(0, 128));

    // Set our background to a dark gray, to provide a border between the two editors
    SetBgnColor(facCIDGraphDev().rgbDarkGrey);

    return kCIDLib::True;
}

