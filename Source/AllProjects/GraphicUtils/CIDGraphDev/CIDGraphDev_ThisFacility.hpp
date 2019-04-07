//
// FILE NAME: CIDGraphDev_ThisFacility.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/04/1997
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
//  This is the header file for the CIDGraphDev_ThisFacility.cpp file. This
//  file implements the facility class for this facility.
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
//  CLASS: TFacCIDGraphDev
// PREFIX: fac
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TFacCIDGraphDev : public TFacility
{
    public  :
        // -------------------------------------------------------------------
        //  Public data
        // -------------------------------------------------------------------
        const TRGBClr   rgbBlack;
        const TRGBClr   rgbBlue;
        const TRGBClr   rgbBrown;
        const TRGBClr   rgbCornflowerBlue;
        const TRGBClr   rgbCyan;
        const TRGBClr   rgbDarkBlue;
        const TRGBClr   rgbDarkCyan;
        const TRGBClr   rgbDarkGreen;
        const TRGBClr   rgbDarkGrey;
        const TRGBClr   rgbDarkPink;
        const TRGBClr   rgbDarkRed;
        const TRGBClr   rgbDarkWhite;
        const TRGBClr   rgbGreen;
        const TRGBClr   rgbOffWhite;
        const TRGBClr   rgbOrange;
        const TRGBClr   rgbPaleGrey;
        const TRGBClr   rgbPaleYellow;
        const TRGBClr   rgbPink;
        const TRGBClr   rgbRed;
        const TRGBClr   rgbTeal;
        const TRGBClr   rgbWhite;
        const TRGBClr   rgbYellow;


        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFacCIDGraphDev();

        TFacCIDGraphDev(const TFacCIDGraphDev&) = delete;

        ~TFacCIDGraphDev();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TFacCIDGraphDev& operator=(const TFacCIDGraphDev&) = delete;

        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TBitmap bmpSysBitmaps
        (
            const   tCIDGraphDev::ESysBmps   eBmp
        )   const;

        tCIDGraphDev::EBltTypes eCalcPlacement
        (
            const   TArea&                      areaPlaceIn
            ,       TArea&                      areaSrc
            ,       TArea&                      areaDest
            , const tCIDGraphDev::EPlacement    ePlacement
            , const tCIDLib::TBoolean           bPressEmph
        );

        tCIDLib::TVoid QuerySysBmps
        (
                    TCollection<TSysBitmapInfo>& colToFill
        );


    private :
        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TFacCIDGraphDev,TFacility)
};

#pragma CIDLIB_POPPACK

