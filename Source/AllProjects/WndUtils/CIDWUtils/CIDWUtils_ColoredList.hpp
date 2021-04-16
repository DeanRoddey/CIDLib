//
// FILE NAME: CIDCtrls_ColoredList.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 07/28/2016
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
//  This file implements a convenient variation of the multi-column list box. It allows
//  you to mark some rows for color coding. You can set a foreground and background
//  color that will be used for those rows. They are marked by setting the user flag
//  on those rows.
//
//  Otherwise it is a straight multi-column list box and provides no other functionality
//  beyond that. It is read only unless the application derives from it and does the
//  appropriate things. This guy overrides CellClicked() and just does nothing.
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
//  CLASS: TColoredList
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDWUTILSEXP TColoredList : public TMultiColListBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TColoredList();

        TColoredList(const TColoredList&) = delete;
        TColoredList(TColoredList&&) = delete;

        ~TColoredList();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TColoredList& operator=(const TColoredList&) = delete;
        TColoredList& operator=(TColoredList&&) = delete;


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
        tCIDLib::TVoid SetMarkedColors
        (
            const   TRGBClr&                rgbBgn
            , const TRGBClr&                rgbText
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
        //  m_rgbBgn
        //  m_rgbText
        //      The foreground and background colors we use on marked rows. We default
        //      them to the already set background color of this window and black. The
        //      containing application has to set them to what it wants.
        // -------------------------------------------------------------------
        TRGBClr m_rgbBgn;
        TRGBClr m_rgbText;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TColoredList, TMultiColListBox)
};

#pragma CIDLIB_POPPACK

