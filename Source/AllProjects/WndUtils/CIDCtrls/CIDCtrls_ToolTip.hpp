//
// FILE NAME: CIDCtrl2_ToolTip.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/26/2015
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements a wrapper around the tooltip class. It supports the bubble
//  type tooltip, with text and optional title.
//
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
//  CLASS: TToolTip
// PREFIX: wnd
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TToolTip : public TWindow
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TToolTip();

        TToolTip(const TToolTip&) = delete;

        ~TToolTip();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TToolTip& operator=(const TToolTip&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Create
        (
            const   TWindow&                wndOwner
            , const TString&                strTitle
            , const tCIDCtrls::ETTipIcons   eIcon
        );

        tCIDLib::TVoid Display
        (
            const   TWindow&                wndOwner
            , const TPoint&                 pntAt
            , const TString&                strText
        );

        tCIDLib::TVoid Hide();


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TToolTip, TWindow)
};

#pragma CIDLIB_POPPACK


