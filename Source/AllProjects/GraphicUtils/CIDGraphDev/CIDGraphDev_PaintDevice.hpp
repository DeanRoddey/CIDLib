//
// FILE NAME: CIDGraphDev_PaintDevice.hpp
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
//  This is the header for the CIDGraphDev_PaintDevice.cpp file. This file
//  implements the TGraphPaintDev class, which is a derivative of the basic
//  drawable device used for paint events to windows
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Map a host paint information structure to our name
// ---------------------------------------------------------------------------
struct  tagPAINTSTRUCT;
#define THostPaintInfo  tagPAINTSTRUCT


#pragma CIDLIB_PACK(CIDLIBPACK)


// ---------------------------------------------------------------------------
//  CLASS: TGraphPaintDev
// PREFIX: gdev
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TGraphPaintDev : public TGraphDrawDev
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TGraphPaintDev() = delete;

        TGraphPaintDev
        (
            const   tCIDGraphDev::TDeviceHandle hdevPaint
            , const THostPaintInfo&             hpiToUse
        );

        TGraphPaintDev(const TGraphPaintDev&) = delete;

        ~TGraphPaintDev();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGraphPaintDev& operator=(const TGraphPaintDev&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_phpiThis
        //      For paint devices, this is some extra info needed to clean the thing up.
        //      On other platforms, this may just be a dummystructure with nothing in it.
        //      Its allocated when needed so that every object does not pay the cost.
        // -------------------------------------------------------------------
        THostPaintInfo* m_phpiThis;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGraphPaintDev,TGraphDrawDev)
};

#pragma CIDLIB_POPPACK

