//
// FILE NAME: CIDCtrl2_ProgressBar.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/12/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a couple variations of the progress bar, one that is range
//  oriented and one that just shows activity is ongoing.
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
//  CLASS: TActivityBar
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TActivityBar : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TActivityBar();

        TActivityBar(const TActivityBar&) = delete;

        ~TActivityBar();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TActivityBar& operator=(const TActivityBar&) = delete;


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
            , const tCIDCtrls::EPBarStyles  ePBarStyles = tCIDCtrls::EPBarStyles::None
        );

        tCIDLib::TVoid StartStop
        (
            const   tCIDLib::TBoolean       bToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ePBarStyles
        //      Our class specific styles
        // -------------------------------------------------------------------
        tCIDCtrls::EPBarStyles  m_ePBarStyles;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TActivityBar, TStdCtrlWnd)
};



// ---------------------------------------------------------------------------
//  CLASS: TProgressBar
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TProgressBar : public TStdCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TProgressBar();

        TProgressBar(const TProgressBar&) = delete;

        ~TProgressBar();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TProgressBar& operator=(const TProgressBar&) = delete;


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
            , const tCIDCtrls::EPBarStyles  ePBarStyles = tCIDCtrls::EPBarStyles::None
        );

        tCIDLib::TCard4 c4CurValue() const;

        tCIDLib::TVoid SetRange
        (
            const   tCIDLib::TCard4         c4Min
            , const tCIDLib::TCard4         c4Max
        );

        tCIDLib::TVoid SetError();

        tCIDLib::TVoid SetNormal();

        tCIDLib::TVoid SetPaused();

        tCIDLib::TVoid SetValue
        (
            const   tCIDLib::TCard4         c4ToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ePBarStyles
        //      Our class specific styles
        // -------------------------------------------------------------------
        tCIDCtrls::EPBarStyles  m_ePBarStyles;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TProgressBar, TStdCtrlWnd)
};

#pragma CIDLIB_POPPACK

