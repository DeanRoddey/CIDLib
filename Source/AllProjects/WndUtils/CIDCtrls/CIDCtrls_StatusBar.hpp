//
// FILE NAME: CIDCtrls_StatusBar.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/01/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the TStatusBar class, which is used to sit at the bottom of
//  a frame window and provide a status display area.
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
//  CLASS: TStatusBar
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TStatusBar : public TWindow
{
    public :
        // -------------------------------------------------------------------
        //  Constructor and Destructor
        // -------------------------------------------------------------------
        TStatusBar();

        ~TStatusBar();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Create
        (
            const   TWindow&                wndParent
            , const tCIDCtrls::TWndId       widThis
        );

        tCIDLib::TVoid ParentSizeChanged();

        tCIDLib::TVoid SetAreas
        (
            const   tCIDLib::TCardList&     fcolValues
            , const tCIDLib::TBoolean       bPercents
        );

        tCIDLib::TVoid SetAreaText
        (
            const   tCIDLib::TCard4         c4Index
            , const TString&                strToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Unimplemented
        // -------------------------------------------------------------------
        TStatusBar(const TStatusBar&);
        tCIDLib::TVoid operator=(const TStatusBar&);


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TStatusBar, TWindow)
};

#pragma CIDLIB_POPPACK


