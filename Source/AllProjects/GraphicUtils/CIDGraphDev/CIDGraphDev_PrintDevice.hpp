//
// FILE NAME: CIDGraphDev_PrintDevice.hpp
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

