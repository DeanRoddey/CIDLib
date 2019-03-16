//
// FILE NAME: CIDCtrls_GenericWnd.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 12/06/2015
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This software is copyrighted by 'Charmed Quark Systems, Ltd' and 
//  the author (Dean Roddey.) It is licensed under the MIT Open Source 
//  license:
//
//  https://opensource.org/licenses/MIT
//
// DESCRIPTION:
//
//  This is a very simple window that is just for those places where you need a
//  container window to put other controls in. It doesn't provide any functionality
//  of its own other than just the ability to create it, and the standard window
//  functionality of the base window class. Typically you would want to make sure
//  it has the 'control parent' extended style, if it does contain controls.
//
//  Since it's intended to contain other controls, it watches for size changes and
//  does an auto-child resize, in case any of them have anchors set.
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
//  CLASS: TGenericWnd
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TGenericWnd : public TCtrlWnd
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TGenericWnd();

        TGenericWnd(const TGenericWnd&) = delete;

        ~TGenericWnd();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGenericWnd& operator=(const TGenericWnd&) = delete;


        // -------------------------------------------------------------------
        //  Public, virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid InitFromDesc
        (
            const   TWindow&                wndParent
            , const TDlgItem&               dlgiSrc
            , const tCIDCtrls::EWndThemes   eTheme
        )   override;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid CreateGenWnd
        (
            const   TWindow&                wndParent
            , const TArea&                  areaInit
            , const tCIDCtrls::EWndStyles   eStyles
            , const tCIDCtrls::EExWndStyles eExStyles
            , const tCIDCtrls::TWndId       widThis
        );


        tCIDLib::TVoid PopulateFromDlg
        (
            const   TDlgDesc&               dlgdToLoad
            ,       tCIDLib::TCard4&        c4InitFocus
            , const tCIDLib::TBoolean       bDoInitSize = kCIDLib::False
            , const tCIDLib::TBoolean       bLimitMinSz = kCIDLib::True
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AreaChanged
        (
            const   TArea&                  areaPrev
            , const TArea&                  areaNew
            , const tCIDCtrls::EPosStates   ePosState
            , const tCIDLib::TBoolean       bOrgChanged
            , const tCIDLib::TBoolean       bSizeChanged
            , const tCIDLib::TBoolean       bStateChanged
        )   override;

        tCIDLib::TBoolean bEraseBgn
        (
                    TGraphDrawDev&          gdevToUse
        )   override;

        tCIDLib::TBoolean bPaint
        (
                    TGraphDrawDev&          gdevToUse
            , const TArea&                  areaUpdate
        )   override;


    private :
        // -------------------------------------------------------------------
        //  Private non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid DoAdjustment
        (
            const   TArea&                  areaPrev
            , const TArea&                  areaNew
        );


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bLimitMinSz
        //      We have an issue in that, if we are sized too small, we may try to auto-
        //      size our child controls too small or even get coordinate underflows. We
        //      can be told to limit the minium size, in which case we remember the
        //      size of the original dialog contents. If we get smaller than that, we
        //      use that as the size, which will just let our contents clip
        //
        //      This is only used if they load us from a dialog description.
        // -------------------------------------------------------------------
        TSize               m_szOrg;
        tCIDLib::TBoolean   m_bLimitMinSz;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGenericWnd, TCtrlWnd)
};

#pragma CIDLIB_POPPACK


