//
// FILE NAME: CIDCtrls_Calendar.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/16/2015
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
//  This is a standard calendar control.
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
//  CLASS: TCalendar
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TCalendar : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCalendar();

        ~TCalendar();


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
        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::ECalStyles   eCalStyle
        );

        tCIDLib::TVoid GetSelection
        (
                    TTime&                  tmToFill
        );

        tCIDLib::TVoid SetSelection
        (
            const   TTime&                  tmToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TCalendar(const TCalendar&);
        tCIDLib::TVoid operator=(const TCalendar&);


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eCalStyles
        //      Our class specific styles
        // -------------------------------------------------------------------
        tCIDCtrls::ECalStyles   m_eCalStyle;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCalendar, TStdCtrlWnd)
};

#pragma CIDLIB_POPPACK


