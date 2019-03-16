//
// FILE NAME: CIDGraphDev_MemDevice.hpp
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
//  This is the header for the CIDGraphDev_MemDevice.cpp file. This file
//  implements the TGraphMemDev class, which is a derivative of the drawable
//  derive class used for drawing into memory.
//
// CAVEATS/GOTCHAS:
//
//  We do not adopt the bitmap that we draw on. We just draw on it, so the
//  caller must keep it alive while using this object, and must keep ownership
//  of it and clean it up when appropriate.
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
//  CLASS: TGraphMemDev
// PREFIX: gdev
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TGraphMemDev : public TGraphDrawDev
{
    public  :
        // -------------------------------------------------------------------
        // Constructors and Destructor
        // -------------------------------------------------------------------
        TGraphMemDev() = delete;

        TGraphMemDev
        (
            const   TGraphDrawDev&          gdevCompatible
            , const TBitmap&                bmpSurface
        );

        TGraphMemDev
        (
            const   TGraphDrawDev&          gdevCompatible
            , const TSize&                  szBitmap
        );

        TGraphMemDev
        (
            const   TSize&                  szMax
            , const tCIDImage::EBitDepths   eDepth
            , const tCIDImage::EPixFmts     eFormat
        );

        TGraphMemDev(const TGraphMemDev&) = delete;

        ~TGraphMemDev();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGraphMemDev& operator=(const TGraphMemDev&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TBitmap& bmpCurrent() const;

        tCIDLib::TVoid Resize
        (
            const   TGraphDrawDev&          gdevCompatible
            , const TSize&                  szNew
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eAdopted
        //      Indicates whether we've adopted the surface bitmap or not.
        //
        //  m_hbmpOld
        //      Just used to save the original bitmap for restoral on
        //      destruction.
        //
        //  m_bmpSurface
        //      The bitmap that is the surface of this memory device.
        // -------------------------------------------------------------------
        tCIDLib::EAdoptOpts         m_eAdopted;
        tCIDGraphDev::TBmpHandle    m_hbmpOld;
        TBitmap                     m_bmpSurface;


        // -------------------------------------------------------------------
        //  Do any needed magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGraphMemDev,TGraphDrawDev)
};

#pragma CIDLIB_POPPACK

