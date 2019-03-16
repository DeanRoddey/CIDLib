//
// FILE NAME: CIDCtrls_SectionalList.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/06/2016
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
//  This file implements a convenient variation of the multi-column list box. It supports
//  a sectional list which has divider rows and data rows. The divider rows have a colored
//  background and bold font. The data rows can have however many columns the client code
//  wants. The divider rows just ignore the columns and draws across the whole width.
//
//  Client code just needs to set the per-row flag to true to have it be treated as a
//  section divider. The text of the 0th column is the only one used in that case and the
//  rest should be empty. Data rows they insert themselves as normal, providing all of the
//  required column values.
//
//  Otherwise it's basically just a multi-column list box. This class handles custom drawing
//  in order to achieve the desired display effects.
//
//  It's read only, unless the client code derives from it and enabled any sort of
//  editing.
//
//  It makes no sense to ever enable sorting on this type of list.
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
//  CLASS: TSectionalList
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDWUTILSEXP TSectionalList : public TMultiColListBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSectionalList();

        TSectionalList(const TSectionalList&) = delete;

        ~TSectionalList();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TSectionalList& operator=(const TSectionalList&) = delete;


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
        tCIDLib::TCard4 c4AddDivider
        (
            const   TString&                strText
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
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSectionalList, TMultiColListBox)
};

#pragma CIDLIB_POPPACK

