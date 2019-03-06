//
// FILE NAME: CIDMacroDbg_TextOutTab.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/11/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for a common text output tab window. This is used by some of
//  the output windows that just need to spit out text.
//
//  It has to derive from the tab window client, since it's going to be inserted as
//  a tab. It creates a scroll area and a text out window. The scroll area is the
//  tab contents and the text out window goes inside the scroll area. We provide
//  a few methods for the debugger to do what it needs to do to us and pass them
//  on to the other windows as required.
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
//   CLASS: TTextOutTab
//  PREFIX: wnd
// ---------------------------------------------------------------------------
class TTextOutTab : public TTabWindow
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TTextOutTab
        (
            const   TString&                strName
        );

        ~TTextOutTab();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid AddLines
        (
            const   tCIDLib::TStrList&      colLines
        );

        tCIDLib::TVoid CreateTOTab
        (
                    TTabbedWnd&             wndParent
            , const TString&                strTabText
        );

        tCIDLib::TVoid Reset();


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
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

        tCIDLib::TBoolean bCreated() override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pwndTextOut
        //      The text output that we use to display the text
        // -------------------------------------------------------------------
        TTextOutWnd*    m_pwndTextOut;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TTextOutTab, TTabWindow)
};


#pragma CIDLIB_POPPACK


