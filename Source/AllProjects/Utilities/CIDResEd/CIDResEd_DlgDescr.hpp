//
// FILE NAME: CIDResEd_DlgDescr.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/26/2010
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDResEd_DlgDescr.cpp file, which implements
//  a dialog description, the dialog itself and a list of dialog items.
//
//  We don't need to store these, but they have to be streamable for clipboard
//  copy/paste reasons.
//
// CAVEATS/GOTCHAS:
//
//  1)  We never want to look up controls by resource id. We just want to make sure
//      that, if a control has a symbol, then it has a unique id within its parent
//      dialog. Any that don't have a symbol, all have an id of 0, so they are not
//      unique and so looking up by resource id isn't very useful.
//
//      Instead of use the edit ids, which we know are unique and we always assign.
//
// LOG:
//
//  $_CIDLib_Log_$
//



// ---------------------------------------------------------------------------
//   CLASS: TResEdDlgDesc
//  PREFIX: redd
// ---------------------------------------------------------------------------
class TResEdDlgDesc : public TObject
{
    public :
        // -------------------------------------------------------------------
        //  Public, static methods
        // -------------------------------------------------------------------
        static tCIDLib::ESortComps eComp
        (
            const   TResEdDlgDesc&          redd1
            , const TResEdDlgDesc&          redd2
        );


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TResEdDlgDesc();

        TResEdDlgDesc
        (
            const   TString&                strName
            , const tCIDLib::TResId         ridDlg
        );

        TResEdDlgDesc
        (
            const   TResEdDlgDesc&          reddToCopy
        );

        ~TResEdDlgDesc();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TResEdDlgDesc& operator=
        (
            const   TResEdDlgDesc&          reddToAssign
        );

        tCIDLib::TBoolean operator==
        (
            const   TResEdDlgDesc&          reddToComp
        )   const;

        tCIDLib::TBoolean operator!=
        (
            const   TResEdDlgDesc&          reddToComp
        )   const;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TArea& areaDlg() const;

        const TArea& areaDlg
        (
            const   TArea&                  areaDlg
        );

        TArea areaHull() const;

        tCIDLib::TVoid AdjustZOrder
        (
            const   TResEdDlgItem&          reditToZ
            , const tCIDLib::TBoolean       bForward
        );

        tCIDLib::TBoolean bResIdUsed
        (
            const   tCIDLib::TResId         ridToFind
        )   const;


        tCIDLib::TCard4 c4AddNew
        (
            const   TResEdDlgItem&          reditToAdd
        );

        tCIDLib::TCard4 c4ChildCount() const;

        tCIDLib::TCard4 c4EditId() const;

        tCIDLib::TVoid DeleteItems
        (
            const   tCIDResEd::TDlgItemRefList& colList
        );

        tCIDCtrls::EDlgFlags eFlags() const;

        tCIDCtrls::EWndThemes eTheme() const;

        tCIDLib::TVoid QueryAttrs
        (
                    tCIDMData::TAttrList&   colTar
            ,       TAttrData&              adatTmp
        )   const;

        const TResEdDlgItem* preditByEditId
        (
            const   tCIDLib::TCard4         c4ToFind
        )   const;

        const TResEdDlgItem* preditBySymName
        (
            const   TString&                strToFind
        )   const;

        const TResEdDlgItem* preditByWndName
        (
            const   TString&                strToFind
        )   const;

        tCIDLib::TVoid ParseFrom
        (
                    TResEdSpooler&          spoolSrc
        );

        const TResEdDlgItem& reditAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        TResEdDlgItem& reditAt
        (
            const   tCIDLib::TCard4         c4At
        );

        tCIDLib::TResId ridDlg() const;

        tCIDLib::TVoid Reset();

        const TString& strName() const;

        tCIDLib::TVoid SetAttr
        (
                    TWindow&                wndOwner
            , const TAttrData&              adatNew
        );

        tCIDLib::TVoid SetInitFocus
        (
            const   tCIDLib::TCard4         c4EditId
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

        const TTextSym& tsymText() const;

        tCIDLib::TVoid ToBottom
        (
            const   TResEdDlgItem&          reditToZ
        );

        tCIDLib::TVoid ToTop
        (
            const   TResEdDlgItem&          reditToZ
        );

        tCIDLib::TVoid WriteTo
        (
                    TTextOutStream&         strmTar
        )   const;


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TCard4 c4FindIndex
        (
            const   TResEdDlgItem&          reditToFind
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaDlg
        //      The coordinates of the dialog item in dialog units.
        //
        //  m_c4EditId
        //      A unique edit time id assigned to us, so that a copy being edited
        //      can be linked back to the original, even if the list has changed
        //      in the meantime. We update it every time new info is loaded into
        //      us.
        //
        //  m_colItems
        //      Our list of dialog items that describe the child controls of
        //      this dialog.
        //
        //  m_eFlags
        //      The defined dialog flag bits.
        //
        //  m_eTheme
        //      The theme that this dialog should use to style itself and child
        //      controls.
        //
        //  m_ridDlg
        //      The resource id of this dialog
        //
        //  m_strName
        //      The symbolic name of this dialog
        //
        //  m_tsymText
        //      The symbolic text info that is the title of this dialog, if
        //      any.
        // -------------------------------------------------------------------
        TArea                   m_areaDlg;
        tCIDLib::TCard4         m_c4EditId;
        tCIDResEd::TDlgItemList m_colList;
        tCIDCtrls::EDlgFlags    m_eFlags;
        tCIDCtrls::EWndThemes   m_eTheme;
        tCIDLib::TResId         m_ridDlg;
        TString                 m_strName;
        TTextSym                m_tsymText;


        // -------------------------------------------------------------------
        //  Magic Macros
        // -------------------------------------------------------------------
        RTTIDefs(TResEdDlgDesc,TObject)
};


