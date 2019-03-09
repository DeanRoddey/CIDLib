//
// FILE NAME: CIDGraphDev_WndDevice_.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This is the header for the CIDGraphDev_WndDevice.cpp file. There is a TGraphWndDev
//  class in CIDCtrls, and that's the only place it would need to be used publically.
//  But we have a few needs internally to wrap something gotten from the desktop window
//  and we don't want to have to clean it up manually, so we create this simple internal
//  window device wrapper class.
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
//  CLASS: TIntGraphWndDev
// PREFIX: gdev
// ---------------------------------------------------------------------------
class TIntGraphWndDev : public TGraphDrawDev
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TIntGraphWndDev();

        TIntGraphWndDev(const TIntGraphWndDev&) = delete;

        ~TIntGraphWndDev();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TIntGraphWndDev& operator=(const TIntGraphWndDev&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TIntGraphWndDev, TGraphDrawDev)
};

#pragma CIDLIB_POPPACK

