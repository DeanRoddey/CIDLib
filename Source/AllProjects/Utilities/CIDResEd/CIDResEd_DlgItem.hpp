//
// FILE NAME: CIDResEd_DlgItem.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/26/2010
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
//  This is the header for the CIDResEd_DlgItem.cpp file, which implements the class
//  that represents a single dialog item. The dialog dscription objects contain a lists
//  of these.
//
//  For our own runtime purposes, because the dialog items don't have to have a symbol,
//  and therefore the id is 0, that means we can't use the window id to get from one of
//  these to the associated window. So, we assign a unique window name (not the dialog
//  item symbolic name but the window name) to each one and set that on the window when
//  it is created as well. We use this later to get back to the window. This isn't
//  something that will be saved, it's just for us.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


class TResEdDlgDesc;

// ---------------------------------------------------------------------------
//   CLASS: TResEdDlgItem
//  PREFIX: redit
// ---------------------------------------------------------------------------
class TResEdDlgItem : public TObject, public MStreamable
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::ESortComps eComp
        (
            const   TResEdDlgItem&          redit1
            , const TResEdDlgItem&          redit2
        );

        static tCIDLib::TVoid InitCtrl
        (
            const   TResEdDlgItem&          reditInit
            , const tCIDCtrls::EWndThemes   eTheme
            , const TWindow&                wndParent
            , const TWindow* const          pwndAfter
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TResEdDlgItem();

        TResEdDlgItem
        (
            const   TString&                strType
            , const TArea&                  areaPos
            , const tCIDCtrls::TWndId       widThis
        );

        TResEdDlgItem
        (
            const   TString&                strType
        );

        TResEdDlgItem
        (
            const   TResEdDlgItem&          reditToCopy
        );

        ~TResEdDlgItem();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TResEdDlgItem& operator=
        (
            const   TResEdDlgItem&          reditToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const   TResEdDlgItem&          reditToAssign
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TResEdDlgItem&          reditToAssign
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TArea& areaItem() const;

        const TArea& areaItem
        (
            const   TArea&                  areaToSet
        );

        tCIDLib::TBoolean bHaveHint
        (
            const   TString&                strToCheck
            ,       tCIDLib::TCard4&        c4At
        );

        tCIDLib::TCard4 c4EditId() const;

        tCIDLib::TCard4 c4EditId
        (
            const   tCIDLib::TCard4         c4ToSet
        );

        tCIDCtrls::EDlgItemFlags eFlags() const;

        tCIDCtrls::EDlgItemFlags eFlags
        (
            const   tCIDCtrls::EDlgItemFlags eToSet
        );

        tCIDLib::TVoid ParseFrom
        (
                    TResEdSpooler&          spoolSrc
            , const TString&                strType
            , const TString&                strEndText
        );

        tCIDLib::TVoid QueryAttrs
        (
                    tCIDMData::TAttrList&   colTar
            ,       TAttrData&              adatTmp
            , const TCtrlWnd&               wndSrc
            , const TResEdDlgDesc&          reddParent
        )   const;

        tCIDLib::TResId ridThis() const;

        tCIDLib::TResId ridThis
        (
            const   tCIDLib::TResId         ridToSet
        );

        tCIDLib::TVoid Reset();

        tCIDLib::TVoid ResetTextSym
        (
            const   tCIDResEd::ETextSyms    eSym
        );

        const TString& strSymName() const;

        const TString& strSymName
        (
            const   TString&                strToSet
        );

        const TString& strType() const;

        const TString& strWndName() const;

        tCIDLib::TVoid SetAttr
        (
                    TWindow&                wndOwner
            , const TAttrData&              adatNew
        );

        tCIDLib::TVoid SetOrg
        (
            const   tCIDLib::TInt4          i4X
            , const tCIDLib::TInt4          i4Y
        );

        tCIDLib::TVoid SetSize
        (
            const   tCIDLib::TCard4         c4CX
            , const tCIDLib::TCard4         c4CY
        );

        const TTextSym& tsymFor
        (
            const   tCIDResEd::ETextSyms    eSym
        )   const;

        tCIDLib::TVoid ToDlgItem
        (
                    TDlgItem&               dlgiToFill
        )   const;

        tCIDLib::TVoid WriteTo
        (
                    TTextOutStream&         strmTar
        )   const;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid StreamFrom
        (
                    TBinInStream&           strmToReadFrom
        )   override;

        tCIDLib::TVoid StreamTo
        (
                    TBinOutStream&          strmToWriteTo
        )   const override;


    private :
        // -------------------------------------------------------------------
        //  Private class types
        // -------------------------------------------------------------------
        typedef TObjArray<TTextSym, tCIDResEd::ETextSyms>   TSymList;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaItem
        //      The area of the dialog item, expressed in dialog units.
        //
        //  m_objaTextSyms
        //      Our text symbols, indexed by the ETextSyms enum.
        //
        //  m_c4EditId
        //      A unique edit time id assigned to us, so that a copy being edited
        //      can be linked back to the original, even if the list has changed
        //      in the meantime. We update it every time new info is loaded into
        //      us.
        //
        //  m_colHints
        //      The hints string for this item. We parse out the tokens from the
        //      string for our internal editing use. We format it back out to
        //      write it out. It can't be a hash set since we can't guarantee
        //      the case of the values.
        //
        //  m_eAnchor
        //      The auto-resize/position anchor for this item.
        //
        //  m_eFlags
        //      A set of miscellaneous boolean flags for this item.
        //
        //  m_ridThis
        //      The resource id that should be given to this dialog item.
        //
        //  m_strImage
        //      An optional app image name, used by some controls.
        //
        //  m_strSymName
        //      The symbolic name of this dialog control
        //
        //  m_strType
        //      The control type, which we need to remember so we can write
        //      it back out, and put it into the compiled CIDRES file.
        //
        //  m_strWndName
        //      A unique window name which we use to map between us and the window, since
        //      they may not have a unique window id (they are zero if they don't have
        //      a symbolic name.) This is a text prefix followed by the m_c4EditId value
        //      above.
        // -------------------------------------------------------------------
        TArea                       m_areaItem;
        TSymList                    m_objaTextSyms;
        tCIDLib::TCard4             m_c4EditId;
        tCIDCtrls::THintsList       m_colHints;
        tCIDCtrls::EEdgeAnchors     m_eAnchor;
        tCIDCtrls::EDlgItemFlags    m_eFlags;
        tCIDLib::TResId             m_ridThis;
        TString                     m_strImage;
        TString                     m_strSymName;
        TString                     m_strType;
        TString                     m_strWndName;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TResEdDlgItem,TObject)
};


