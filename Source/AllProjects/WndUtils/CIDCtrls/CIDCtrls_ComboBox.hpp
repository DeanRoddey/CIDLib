//
// FILE NAME: CIDCtrls_ComboBox.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/12/2015
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
//  This is a standard combo box type control.
//
//  This guy is a sort of weird one. If you ask it for the current selection, you get
//  the selection in the list. If the list is dropped down and the user is moving the
//  mouse over it, you get that value, even though it's not been selected yet.
//
//  So we store a last actually selected (or manually set) value, and watch for the
//  command msgs that indicates a select and list close and store that. When we are
//  asked for the current value, we return that.
//
// CAVEATS/GOTCHAS:
//
//  1)  This code does not handle the 'simple' mode combo box. It would need to be
//      updated if you want to support that.
//
//  2)  If you enable sorting, then you have to either set an id on each item that
//      lets you identify the selection, or go by the text values.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once

#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TComboBox
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TComboBox : public TListWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TComboBox();

        ~TComboBox();


        // -------------------------------------------------------------------
        //  Public, inherited methods
        // -------------------------------------------------------------------
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
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TBoolean       bSelectAnother = kCIDLib::True
        )   override;

        tCIDLib::TVoid SelectByIndex
        (
            const   tCIDLib::TCard4         c4ToSel
            , const tCIDLib::TBoolean       bForceEvent = kCIDLib::False
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bQueryCurVal
        (
                    TString&                strToFill
        )   const;

        tCIDLib::TCard4 c4AddItem
        (
            const   TString&                strInsert
            , const tCIDLib::TCard4         c4Id = kCIDLib::c4MaxCard - 2
        );

        tCIDLib::TCard4 c4IdToIndex
        (
            const   tCIDLib::TCard4         c4Id
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::True
        )   const;

        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EComboStyles eComboStyles = tCIDCtrls::EComboStyles::None
        );

        tCIDLib::TVoid InsertItem
        (
            const   tCIDLib::TCard4         c4At
            , const TString&                strInsert
            , const tCIDLib::TCard4         c4Id = kCIDLib::c4MaxCard - 2
        );

        tCIDLib::TVoid LoadNumRange
        (
            const   tCIDLib::TInt4          i4First
            , const tCIDLib::TInt4          i4Last
        );

        tCIDLib::TVoid LoadList
        (
            const   tCIDLib::TStrCollect&   colLoadWith
        );

        tCIDLib::TCard4 c4SelectById
        (
            const   tCIDLib::TCard4         c4ToSel
            , const tCIDLib::TBoolean       bForceEvent = kCIDLib::False
            , const tCIDLib::TBoolean       bThrowIfNot = kCIDLib::False
        );

        tCIDLib::TVoid SetCueText
        (
            const   TString&                strToSet
        );

        tCIDLib::TVoid UpdateIdAt
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
        tCIDLib::TBoolean bNotReflect
        (
                    TWindow&                wndTar
            , const tCIDCtrls::TWndMsg      wmsgMsg
            , const tCIDCtrls::TWParam      wParam
            , const tCIDCtrls::TLParam      lParam
            ,       tCIDCtrls::TMsgResult&  mresRet
        )   override;

        tCIDLib::TVoid Destroyed() override;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TComboBox(const TComboBox&);
        tCIDLib::TVoid operator=(const TComboBox&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4CurSel
        //      The last selected list index. See the file comments above for why
        //      we do this. It will be c4MaxCard if no selection yet.
        //
        //  m_eComboStyles
        //      The styles set on us, in our form.
        //
        //  m_pwndEditor
        //      We get a pointer to the entry field as well, so that we can get the
        //      actual text for non-read only combos.
        // -------------------------------------------------------------------
        tCIDLib::TCard4         m_c4CurSel;
        tCIDCtrls::EComboStyles m_eComboStyles;
        TWindow*                m_pwndEditor;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TComboBox, TListWnd)
};

//
//  A helper macro that will load all of the translated or loaded values of an
//  enum into the list, which is a common operation.
//
#define AltXlatEnumToCombo(NS, E, CB) \
{ \
    for (NS##::E eIndex = NS##::E##::Min; eIndex <= NS##::E##::Max; eIndex++) \
        (CB).c4AddItem(NS##::strAltXlat##E(eIndex)); \
}

#define XlatEnumToCombo(NS, E, CB) \
{ \
    for (NS##::E eIndex = NS##::E##::Min; eIndex <= NS##::E##::Max; eIndex++) \
        (CB).c4AddItem(NS##::strXlat##E(eIndex)); \
}

#define LoadEnumToCombo(NS, E, CB) \
{ \
    for (NS##::E eIndex = NS##::E##::Min; eIndex <= NS##::E##::Max; eIndex++) \
        (CB).c4AddItem(NS##::strLoad##E(eIndex)); \
}

#pragma CIDLIB_POPPACK

