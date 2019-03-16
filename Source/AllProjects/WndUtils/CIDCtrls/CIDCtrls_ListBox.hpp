//
// FILE NAME: CIDCtrls_ListBox.hpp
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
//  This is a standard list box control. It's fairly straightforward. It only deals
//  with strings. It supports custom drawing, but nothing in this class has to
//  know about it, other than a class specific style to enable it. We support
//  each item having a numeric id stored with it if desired.
//
//  We also define an 'ops' class, which can be used to load the list directly
//  from source data. It's an abstact interface that one of the LoadList() methods
//  will accept. It allows us to iterate the available source objects and to query
//  the text and id associted with each one.
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
//  CLASS: TListBoxOps
// PREFIX: lbops
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TListBoxOps
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and destructors
        // -------------------------------------------------------------------
        TListBoxOps(const TListBoxOps&) = delete;

        virtual ~TListBoxOps();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TListBoxOps& operator=(const TListBoxOps&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TCard4 c4RowCount() const = 0;

        virtual tCIDLib::TCard4 c4RowId
        (
            const   tCIDLib::TCard4         c4RowIndex
        )   const = 0;

        virtual const TString& strRowText
        (
            const   tCIDLib::TCard4         c4RowIndex
        )   const = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TListBoxOps();
};


// ---------------------------------------------------------------------------
//  CLASS: TListBox
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TListBox : public TListWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TListBox();

        TListBox(const TListBox&) = delete;

        ~TListBox();


        // -------------------------------------------------------------------
        //  Public oeprators
        // -------------------------------------------------------------------
        TListBox& operator=(const TListBox&) = delete;


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

        tCIDLib::TCard4 c4ItemCount() const;

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
        )  override;

        tCIDLib::TVoid SelectByIndex
        (
            const   tCIDLib::TCard4         c4ToSel
            , const tCIDLib::TBoolean       bForceEvent = kCIDLib::False
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bIsMarked
        (
            const   tCIDLib::TCard4         c4Index
        )   const;

        tCIDLib::TBoolean bMoveDn
        (
            const   tCIDLib::TBoolean       bMoveSelection
        );

        tCIDLib::TBoolean bMoveUp
        (
            const   tCIDLib::TBoolean       bMoveSelection
        );

        tCIDLib::TCard4 c4AddItem
        (
            const   TString&                strToAdd
            , const tCIDLib::TCard4         c4ItemId = tCIDLib::TCard4(kCIDLib::i4MaxInt)
        );

        tCIDLib::TCard4 c4SetText
        (
            const   tCIDLib::TCard4         c4Index
            , const TString&                strToSet
        );

        tCIDLib::TVoid CreateListBox
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::ELBStyles    eLBStyles = tCIDCtrls::ELBStyles::None
            , const tCIDCtrls::EExWndStyles eExStyles = tCIDCtrls::EExWndStyles::None
        );

        tCIDLib::TVoid InsertItem
        (
            const   TString&                strToAdd
            , const tCIDLib::TCard4         c4At
            , const tCIDLib::TCard4         c4ItemId = tCIDLib::TCard4(kCIDLib::i4MaxInt)
        );

        tCIDLib::TVoid LoadList
        (
            const   tCIDLib::TStrCollect&   colLoadWith
        );

        tCIDLib::TVoid LoadList
        (
            const   tCIDLib::TStrIdList&    colLoadWith
        );

        tCIDLib::TVoid LoadList
        (
                    TListBoxOps&            lbopsToUse
        );

        tCIDLib::TVoid MarkAll
        (
            const   tCIDLib::TBoolean       bState
        );

        tCIDLib::TVoid MarkItem
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TBoolean       bState
        );

        tCIDLib::TCard4 c4QuerySelList
        (
                    tCIDLib::TCardList&     fcolToFill
        )   const;

        TString strCurSelection() const;

        tCIDLib::TVoid SetItemData
        (
            const   tCIDLib::TCard4         c4Index
            , const tCIDLib::TCard4         c4ToSet
        );

        tCIDLib::TVoid ValueAt
        (
            const   tCIDLib::TCard4         c4Index
            ,       TString&                strToFill
        )   const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
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


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CheckMultiSelect
        (
            const   tCIDLib::TCard4         c4Line
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eLBStyles
        //      The styles set on us, in our form.
        // -------------------------------------------------------------------
        tCIDCtrls::ELBStyles    m_eLBStyles;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TListBox, TListWnd)
};


//
//  A helper macro that will load all of the translated or loaded values of an
//  enum into the list, which is a common operation.
//
#define AltXlatEnumToListBox(NS, E, LB) \
{ \
    for (NS##::E eIndex = NS##::E##::Min; eIndex <= NS##::E##::Max; eIndex++) \
        (LB).c4AddItem(NS##::strAltXlat##E(eIndex)); \
}

#define XlatEnumToListBox(NS, E, LB) \
{ \
    for (NS##::E eIndex = NS##::E##::Min; eIndex <= NS##::E##::Max; eIndex++) \
        (LB).c4AddItem(NS##::strXlat##E(eIndex)); \
}

#define LoadEnumToListBox(NS, E, LB) \
{ \
    for (NS##::E eIndex = NS##::E##::Min; eIndex <= NS##::E##::Max; eIndex++) \
        (LB).c4AddItem(NS##::strLoad##E(eIndex)); \
}

#pragma CIDLIB_POPPACK


