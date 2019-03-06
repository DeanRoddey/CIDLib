//
// FILE NAME: CIDCtrls_2ColSectList.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 01/20/2016
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a convenient variation of the multi-column list box. It allows
//  you to display lists of key=value pair values with section titles, and nice divider
//  lines. Mostly it's just handling some custom draw.
//
//  Client code just needs to set the per-row flag to true to have it be treated as a
//  section divider, in which case only the key part of the row is used, as the divider
//  text. We provide helper methods for adding a divider and non-divider row, which you
//  should use.
//
//  Otherwise it's basically just a multi-column list box. Though we do create a
//  zero width 0th column, so that the left column can be right justified.
//
//  It's read only, unless the client code derives from it and enabled any sort of
//  editing.
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
//  CLASS: T2ColSectList
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDWUTILSEXP T2ColSectList : public TMultiColListBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        T2ColSectList();

        T2ColSectList(const T2ColSectList&) = delete;

        ~T2ColSectList();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        T2ColSectList& operator=(const T2ColSectList&) = delete;


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


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddDataRow
        (
            const   TString&                strKey
            , const TString&                strValue
            , const tCIDLib::TCard4         c4RowId = 0
        );

        tCIDLib::TVoid AddSectTitle
        (
            const   TString&                strTitle
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TBoolean bEraseBgn
        (
                    TGraphDrawDev&          gdevToUse
        )   override;

        tCIDLib::TVoid CellClicked
        (
            const   tCIDLib::TCard4         c4Row
            , const tCIDLib::TCard4         c4Column
            , const tCIDLib::TBoolean       bLeftButton
        )   override;

        tCIDCtrls::EMCLBCustRets eCustomDraw
        (
                    TGraphDrawDev&          gdevTar
            , const tCIDLib::TCard4         c4Row
            , const tCIDLib::TCard4         c4Column
            , const tCIDLib::TBoolean       bPost
            , const TArea&                  areaAt
            ,       TRGBClr&                rgbBgn
            ,       TRGBClr&                rgbText
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_colTmpCols
        //      For internal efficiency, for loading up rows.
        // -------------------------------------------------------------------
        tCIDLib::TStrList   m_colTmpCols;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(T2ColSectList, TMultiColListBox)
};

#pragma CIDLIB_POPPACK

