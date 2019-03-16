//
// FILE NAME: CIDGraphDev_DeviceJanitor.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/27/1997
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
//  This is the header for the CIDGraphDev_DeviceJanitor.cpp file. This file
//  implements the graphics device janitor classes. These classes make it
//  safe to modify graphics attributes on a device and insure that they
//  get restored.
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
//  CLASS: TGraphicDevJanitor
// PREFIX: jan
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TGraphicDevJanitor : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TGraphicDevJanitor() = delete;

        TGraphicDevJanitor
        (
                    TGraphDrawDev* const    pgdevToSanitize
        );

        TGraphicDevJanitor(const TGraphicDevJanitor&) = delete;

        ~TGraphicDevJanitor();


        // -------------------------------------------------------------------
        //  Public orepators
        // -------------------------------------------------------------------
        TGraphicDevJanitor& operator=(const TGraphicDevJanitor&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pgdevSanitize
        //      This is the device object that we are to restore the state of
        //      when we die.
        //
        //  m_pntTranslate
        //      This is some state that the device itself has that we need to
        //      save and restore. Later, the device class should be updated
        //      to have its own stack or something, but for now this is all
        //      that is involved.
        // -------------------------------------------------------------------
        TGraphDrawDev*  m_pgdevSanitize;
        TPoint          m_pntTranslate;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGraphicDevJanitor,TObject)
};

#pragma CIDLIB_POPPACK


