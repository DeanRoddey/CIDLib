//
// FILE NAME: CIDGraphDev_PrintDevice.hpp
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
//  This is the header for the CIDGraphDev_PrintDevice.cpp file. This file
//  implements the TGraphPrintDev class, which is a derivative of the basic
//  drawable device used to print to hardcopy.
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
//  CLASS: TGraphPrintDev
// PREFIX: gdev
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TGraphPrintDev : public TGraphDrawDev
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TGraphPrintDev() = delete;

        TGraphPrintDev
        (
            const   TGraphPrintDev&
        ) = delete;

        TGraphPrintDev
        (
            const   TString&                    strDriverName
            , const TString&                    strPrinterName
        );

        ~TGraphPrintDev();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGraphPrintDev& operator=(const TGraphPrintDev&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGraphPrintDev,TGraphDrawDev)
};

#pragma CIDLIB_POPPACK

