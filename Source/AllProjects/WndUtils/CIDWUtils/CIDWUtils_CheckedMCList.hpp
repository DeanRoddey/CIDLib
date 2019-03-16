//
// FILE NAME: CIDCtrls_CheckedMCList.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/21/2015
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
//  This file implements a simple derivative of the multi-column list box. This one
//  is used to implement a list box, where each item can be checked or not. Those
//  that are checked show a check mark bitmap in the 0th column.
//
//  The client code can have as many columns as they want, but should leave a 0th column
//  for the check mark. Any text they set on the 0th column will be ignored by our custom
//  drawing. We provide methods for them to check/uncheck a row, and to get the checked
//  state.
//
//
// CAVEATS/GOTCHAS:
//
//  1)  We have some special init issues. We have to call our parent's bCreated()
//      method. But, if columns were passed during the creation, it will set the
//      columns during that call. But, we won't have done our setup yet, so when
//      we get the ColumnsSet() method, we won't be ready.
//
//      So, we have an init flag and just fault in the init during the ColumnsSet
//      callback. We need the flag to avoid doing it multiple times if they set
//      the columns again.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TCheckedMCListWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDWUTILSEXP TCheckedMCListWnd : public TMultiColListBox
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCheckedMCListWnd();

        TCheckedMCListWnd(const TCheckedMCListWnd&) = delete;

        ~TCheckedMCListWnd();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCheckedMCListWnd& operator=(const TCheckedMCListWnd&) = delete;


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
        tCIDLib::TBoolean bAutoCheck() const;

        tCIDLib::TBoolean bAutoCheck
        (
            const   tCIDLib::TBoolean       bToSet
        );

        tCIDLib::TBoolean bIsCheckedAt
        (
            const   tCIDLib::TCard4         c4At
        )   const;

        tCIDLib::TVoid SetCheckAt
        (
            const   tCIDLib::TCard4         c4At
            , const tCIDLib::TBoolean       bState
        );

    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;

        tCIDLib::TBoolean bInvoked
        (
            const   tCIDLib::TCard4         c4Row
            , const tCIDLib::TCard4         c4Id
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

        tCIDLib::TVoid ColumnsSet
        (
            const   tCIDLib::TCard4         c4Count
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_areaBmpSrc
        //      We pre-load the src area of the check mark bitmap, for use when
        //      drawing it. We don't let it scale.
        //
        //  m_areaCell
        //  m_areaTar
        //      Temp areas for use during custom drawing
        //
        //  m_bInitDone
        //      We have some special initialization issues. See the file comments
        //      above.
        //
        //  m_bmpCheck
        //      The check mark bitmap we use.
        //
        //  m_bAutoCheck
        //      We can handle double clicks ourself and toggle the clicked row. This
        //      is the default
        //
        //  m_strTmp
        //      For use during custom drawing to avoid a lot of overhead.
        // -------------------------------------------------------------------
        TArea               m_areaBmpSrc;
        TArea               m_areaCell;
        TArea               m_areaTar;
        tCIDLib::TBoolean   m_bInitDone;
        TBitmap             m_bmpCheck;
        tCIDLib::TBoolean   m_bAutoCheck;
        TString             m_strTmp;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCheckedMCListWnd, TMultiColListBox)
};

#pragma CIDLIB_POPPACK



