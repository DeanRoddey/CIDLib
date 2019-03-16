//
// FILE NAME: CIDCtrls_CtrlWindow.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
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
//  A simple base class from which all of the windows that implement the standard
//  type controls derive from, so that we can have some shared functionality and
//  virtual interfaces.
//
//  It also provides a protected CreateCtrl method that custom control windows created
//  outside of this facility, where they don't have access to platform specific
//  stuff. We create an internal window class that will be used for all of them.
//  Actually we create two, one for h/v redraw type and one not.
//
//  And finally we create a small abstract derivative from which all of the control
//  window classes that encapsulate standard controls should derive. It allows us
//  to provide some standard functionality for all of them, as apposed to custom
//  controls.
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
//  CLASS: TCtrlWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TCtrlWnd : public TWindow
{
    public :
        // -------------------------------------------------------------------
        //  Destructure, ctors are hidden
        // -------------------------------------------------------------------
        ~TCtrlWnd();


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid InitFromDesc
        (
            const   TWindow&                wndParent
            , const TDlgItem&               dlgiSrc
            , const tCIDCtrls::EWndThemes   eTheme
        ) = 0;

        virtual tCIDLib::TVoid QueryHints
        (
                    tCIDLib::TStrCollect&   colToFill
        )   const;

        virtual TSize szDefault() const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TCtrlWnd();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TCtrlWnd(const TCtrlWnd&);
        tCIDLib::TVoid operator=(const TCtrlWnd&);


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCtrlWnd, TWindow)
};



// ---------------------------------------------------------------------------
//  CLASS: TStdCtrlWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TStdCtrlWnd : public TCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Destructure, ctors are hidden
        // -------------------------------------------------------------------
        ~TStdCtrlWnd();


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TStdCtrlWnd();


        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bCreated() override;

        tCIDCtrls::ECtxMenuOpts eShowContextMenu
        (
            const   TPoint&                 pntAt
            , const tCIDCtrls::TWndId       widSrc
        )   override;

        tCIDLib::TVoid WndTextPreQuery
        (
                    TString&                strToFill
        )   const override;

        tCIDLib::TVoid WndTextStore
        (
            const   TString&                strToStore
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TStdCtrlWnd(const TStdCtrlWnd&);
        tCIDLib::TVoid operator=(const TStdCtrlWnd&);


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStdCtrlWnd, TCtrlWnd)
};

#pragma CIDLIB_POPPACK



