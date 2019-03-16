//
// FILE NAME: CIDCtrls_WndDevice.hpp
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
//  This is the header for the CIDCtrls_WndDevice.cpp file. This file
//  implements the TGraphWndDev class, which is a derivative of the drawable
//  derive class used for ad hoc drawing onto windows.
//
//  Also provided here is the TGraphDesktopDevice class, which provides a
//  drawable device for output to the desktop. Its just a simple derivative
//  of TGraphWndDev.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
class   TWindow;


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TGraphWndDev
// PREFIX: gdev
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TGraphWndDev : public TGraphDrawDev
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TGraphWndDev
        (
            const   tCIDGraphDev::TDeviceHandle hdevPaint
            , const tCIDLib::EAdoptOpts         eAdopt = tCIDLib::EAdoptOpts::Adopt
        );

        TGraphWndDev
        (
            const   TWindow&                    wndSource
        );

        ~TGraphWndDev();


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TGraphWndDev();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TGraphWndDev(const TGraphWndDev&);
        tCIDLib::TVoid operator=(const TGraphWndDev&);


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGraphWndDev,TGraphDrawDev)
};



// ---------------------------------------------------------------------------
//  CLASS: TGraphDesktopDev
// PREFIX: gdev
// ---------------------------------------------------------------------------
class CIDCTRLSEXP TGraphDesktopDev : public TGraphWndDev
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TGraphDesktopDev();

        ~TGraphDesktopDev();


    private :
        // -------------------------------------------------------------------
        //  Unimplemented constructors and operators
        // -------------------------------------------------------------------
        TGraphDesktopDev(const TGraphDesktopDev&);
        tCIDLib::TVoid operator=(const TGraphDesktopDev&);


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGraphDesktopDev,TGraphWndDev)
};

#pragma CIDLIB_POPPACK


