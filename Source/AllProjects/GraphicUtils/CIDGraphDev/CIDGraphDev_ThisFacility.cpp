//
// FILE NAME: CIDGraphDev_ThisFacility.cpp
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
//  This file implements the facility class for this facility. It is a
//  derivative (as all facility objects are) of TFacility.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Facility specific includes
// ---------------------------------------------------------------------------
#include    "CIDGraphDev_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TFacCIDGraphDev,TFacility)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDGraphDev_ThisFacility
{
    namespace
    {
        // -----------------------------------------------------------------------
        //  This is the map that we load up the bitmaps into. This is done lazily
        // -----------------------------------------------------------------------
        TEArray<TBitmap*, tCIDGraphDev::ESysBmps, tCIDGraphDev::ESysBmps::Count>   apbmpMap
        (
            static_cast<TBitmap*>(nullptr)
        );
    }
};


// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
static tCIDLib::TBoolean bDevErrToIgnore()
{
    tCIDLib::TCard4 c4LastErr = ::GetLastError();
    if (!c4LastErr
    ||  (c4LastErr == ERROR_ACCESS_DENIED)
    ||  (c4LastErr == ERROR_INVALID_HANDLE)
    ||  (c4LastErr == ERROR_INVALID_WINDOW_HANDLE))
    {
        return kCIDLib::True;
    }
    TKrnlError::SetLastHostError(c4LastErr);
    return kCIDLib::False;
}

// Lazily faults in our system bitmaps
static tCIDLib::TVoid FaultInBitmaps()
{
    // The caller already did one check so assume we have to lock
    TBaseLock lockBmp;
    if (!CIDGraphDev_ThisFacility::apbmpMap.bIsLoaded())
    {
        tCIDLib::ForEachE<tCIDGraphDev::ESysBmps>
        (
            [](const tCIDGraphDev::ESysBmps eBmp)
            {
                if (!CIDGraphDev_ThisFacility::apbmpMap[eBmp])
                    CIDGraphDev_ThisFacility::apbmpMap[eBmp] = new TBitmap(eBmp);
            }
        );
    }
    CIDGraphDev_ThisFacility::apbmpMap.SetLoaded();
}




// ---------------------------------------------------------------------------
//   CLASS: TFacCIDGraphDev
//  PREFIX: fac
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFacCIDGraphDev: Constructors and operators
// ---------------------------------------------------------------------------
TFacCIDGraphDev::TFacCIDGraphDev() :

    TFacility
    (
        L"CIDGraphDev"
        , tCIDLib::EModTypes::SharedLib
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
    , rgbBlack(0,0,0)
    , rgbBlue(0, 0, 255)
    , rgbBrown(128, 128, 0)
    , rgbCornflowerBlue(100, 149, 237)
    , rgbCyan(0, 255, 255)
    , rgbDarkBlue(0, 0, 128)
    , rgbDarkCyan(0, 128, 128)
    , rgbDarkGreen(0, 128, 0)
    , rgbDarkGrey(128, 128, 128)
    , rgbDarkPink(128, 0, 128)
    , rgbDarkRed(128, 0, 0)
    , rgbDarkWhite(240, 240, 240)
    , rgbGreen(0, 255, 0)
    , rgbOffWhite(250, 250, 250)
    , rgbOrange(255, 112, 43)
    , rgbPaleGrey(204, 204, 204)
    , rgbPaleYellow(255,255,128)
    , rgbPink(255, 0, 255)
    , rgbRed(255, 0, 0)
    , rgbTeal(64,128,128)
    , rgbWhite(255, 255, 255)
    , rgbYellow(255, 255, 0)
{
}

TFacCIDGraphDev::~TFacCIDGraphDev()
{
}


// ---------------------------------------------------------------------------
//  TFacCIDGraphDev: Public, static methods
// ---------------------------------------------------------------------------

TBitmap TFacCIDGraphDev::bmpSysBitmaps(const tCIDGraphDev::ESysBmps eBmp) const
{
    // Sanity check the value
    if (eBmp >= tCIDGraphDev::ESysBmps::Count)
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_BadEnumValue
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::BadParms
            , TCardinal(tCIDLib::c4EnumOrd(eBmp))
            , TString(L"tCIDGraphDev::EBitmaps")
        );
    }

    // Fault them in if needed
    if (!CIDGraphDev_ThisFacility::apbmpMap.bIsLoaded())
        FaultInBitmaps();

    //
    //  And now return a copy of the icon. Note that icons ref-count their
    //  underlying actual icon data, so this is not as piggy as it looks.
    //
    return *CIDGraphDev_ThisFacility::apbmpMap[eBmp];
}


tCIDGraphDev::EBltTypes
TFacCIDGraphDev::eCalcPlacement(const   TArea&                      areaPlaceIn
                                ,       TArea&                      areaSrc
                                ,       TArea&                      areaDest
                                , const tCIDGraphDev::EPlacement    ePlacement
                                , const tCIDLib::TBoolean           bPressEmph)
{
    tCIDGraphDev::EBltTypes eBlt = tCIDGraphDev::EBltTypes::None;

    //
    //  According to the placement type and the size relationship of the
    //  size of the image and the target area, set up the source and
    //  target areas.
    //
    areaDest = areaPlaceIn;
    if (ePlacement == tCIDGraphDev::EPlacement::Center)
    {
        //
        //  If either axis of the image is larger than the drawing area,
        //  then we need to do some work, else we just use a target area
        //  the same size as the image, centered in the area.
        //
        if ((areaSrc.c4Width() < areaPlaceIn.c4Width())
        &&  (areaSrc.c4Height() < areaPlaceIn.c4Height()))
        {
            areaDest.SetSize(areaSrc.szArea());
            areaDest.CenterIn(areaPlaceIn);
        }
         else
        {
            //
            //  Get a temp area of the drawing area and make it zero based.
            //  Then take the smaller of the sizes of the drawing area and
            //  the src image area, then center that in the zero based drawing
            //  area. That will create a source area that pulls out the part
            //  of the image that will fit.
            //
            TArea areaTmp(areaSrc);
            areaSrc.TakeSmaller(areaPlaceIn);
            areaSrc.CenterIn(areaTmp);

            //
            //  Make the actual target the same size as the bit of the image
            //  that we are going to show, and center it in the real drawing
            //  area.
            //
            areaDest = areaSrc;
            areaDest.CenterIn(areaPlaceIn);

            // They are the same size, so we just need clipping
            eBlt = tCIDGraphDev::EBltTypes::Clip;
        }
    }
     else if (ePlacement == tCIDGraphDev::EPlacement::UpperLeft)
    {
        eBlt = tCIDGraphDev::EBltTypes::Clip;
        areaDest = areaPlaceIn;
        areaSrc.TakeSmaller(areaDest);
        areaDest.TakeSmaller(areaSrc);
    }
     else if ((ePlacement == tCIDGraphDev::EPlacement::SizeIfTooBig)
          ||  (ePlacement == tCIDGraphDev::EPlacement::SizeToFit))
    {
        // If it'll fit, then center it, else size it so that it fits
        if ((ePlacement == tCIDGraphDev::EPlacement::SizeIfTooBig)
        &&  (areaSrc.c4Width() <= areaPlaceIn.c4Width())
        &&  (areaSrc.c4Height() <= areaPlaceIn.c4Height()))
        {
            areaDest.SetSize(areaSrc.szArea());
            areaDest.CenterIn(areaPlaceIn);
        }
         else if ((ePlacement == tCIDGraphDev::EPlacement::SizeToFit)
              &&  (areaSrc.c4Width() == areaPlaceIn.c4Width())
              &&  (areaSrc.c4Height() == areaPlaceIn.c4Height()))
        {
            //
            //  Already the same size, so stretching or clipping required,
            //  just center the dest area in the target area.
            //
            areaDest.CenterIn(areaPlaceIn);
        }
         else
        {
            eBlt = tCIDGraphDev::EBltTypes::Stretch;
        }
    }
     else if ((ePlacement == tCIDGraphDev::EPlacement::KeepAR)
          ||  (ePlacement == tCIDGraphDev::EPlacement::FitAR))
    {
        //
        //  If doing KeepAR and it'll fit, then center it without scaling,
        //  else size it so that we keep the original aspect ratio.
        //
        if ((ePlacement == tCIDGraphDev::EPlacement::KeepAR)
        &&  (areaSrc.c4Width() <= areaPlaceIn.c4Width())
        &&  (areaSrc.c4Height() <= areaPlaceIn.c4Height()))
        {
            areaDest.SetSize(areaSrc.szArea());
            areaDest.CenterIn(areaPlaceIn);
        }
         else
        {
            eBlt = tCIDGraphDev::EBltTypes::Stretch;

            // Calc the original aspect ratio
            const tCIDLib::TFloat8 f8OrgAR
            (
                tCIDLib::TFloat8(areaSrc.c4Width())
                / tCIDLib::TFloat8(areaSrc.c4Height())
            );

            //
            //  If the target area has a wider ratio, then we will try
            //  to fit the height and scale the width, since we know that
            //  it will fit. If the target has thinner ratio, we'll scale
            //  the height and make the width fit. If they are the same
            //  it doesn't matter which one we scale.
            //
            const tCIDLib::TFloat8 f8TarAR
            (
                tCIDLib::TFloat8(areaPlaceIn.c4Width())
                / tCIDLib::TFloat8(areaPlaceIn.c4Height())
            );

            if (f8TarAR > f8OrgAR)
            {
                areaDest.c4Width(tCIDLib::TCard4(areaDest.c4Height() * f8OrgAR));
            }
             else
            {
                areaDest.c4Height(tCIDLib::TCard4(areaDest.c4Width() / f8OrgAR));
            }

            areaDest.CenterIn(areaPlaceIn);
        }
    }
     else if (ePlacement == tCIDGraphDev::EPlacement::Tile)
    {
        // Dest area is already correct
        eBlt = tCIDGraphDev::EBltTypes::Tile;
    }

    // If the button is pushed, then shift the contents over one.
    if (bPressEmph)
        areaDest.AdjustOrg(1, 1);

    return eBlt;
}


tCIDLib::TVoid TFacCIDGraphDev::QuerySysBmps(TCollection<TSysBitmapInfo>& colToFill)
{
    if (!CIDGraphDev_ThisFacility::apbmpMap.bIsLoaded())
        FaultInBitmaps();

    // Iterate the list and put an item in the list for each one
    tCIDLib::ForEachE<tCIDGraphDev::ESysBmps>
    (
        [&colToFill](const tCIDGraphDev::ESysBmps eBmp)
        {
            colToFill.objAdd
            (
                TSysBitmapInfo
                (
                    TBitmap::pszToName(eBmp), *CIDGraphDev_ThisFacility::apbmpMap[eBmp]
                )
            );
        }
    );
}

