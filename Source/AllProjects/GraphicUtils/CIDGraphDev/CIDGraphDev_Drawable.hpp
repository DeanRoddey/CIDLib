//
// FILE NAME: CIDGraphDev_Drawable.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 8/24/2001
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
//  This file defines a simple mixin interface that defines something that
//  can be drawn on a graphics device. This is an important mixin that allows
//  us to create a drawable object system, since a lot of code can be written
//  in generic terms of things that it can draw on a device it has.
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
//   CLASS: MDrawable
//  PREFIX: drwbl
// ---------------------------------------------------------------------------
class CIDGRDEVEXP MDrawable
{
    public :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        MDrawable(const MDrawable&) = delete;

        ~MDrawable()
        {
        }


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        MDrawable& operator=(const MDrawable&) = delete;


        // -------------------------------------------------------------------
        //  Public, pure virtual methods
        // -------------------------------------------------------------------
        virtual tCIDLib::TVoid DrawOn
        (
                    TGraphDrawDev&              gdevTarget
            , const TPoint&                     pntOrg
        )   const = 0;

        virtual tCIDLib::TVoid DrawOn
        (
                    TGraphDrawDev&              gdevTarget
            , const TArea&                      areaToFill
        )   const = 0;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        MDrawable()
        {
        }
};

#pragma CIDLIB_POPPACK

