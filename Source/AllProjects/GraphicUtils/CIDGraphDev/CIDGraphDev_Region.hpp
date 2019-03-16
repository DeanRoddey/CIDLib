//
// FILE NAME: CIDGraphDev_Region.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/24/2002
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
//  This is the header for the CIDGraphDev_Region.cpp file. This file implements
//  the TGUIRegion class, which represents a graphic device region. It may
//  or may not be a single, contiguous area.
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
//  CLASS: TGUIRegion
// PREFIX: grgn
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TGUIRegion : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TGUIRegion();

        TGUIRegion
        (
            const   TArea&                      areaRegion
        );

        TGUIRegion
        (
            const   TArea&                      areaRegion
            , const tCIDLib::TCard4             c4Rounding
        );

        TGUIRegion
        (
            const   TGUIRegion&                 grgnSrc
        );

        ~TGUIRegion();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGUIRegion& operator=
        (
            const   TGUIRegion&                 grgnSrc
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TArea areaBounds() const;

        tCIDLib::TBoolean bContainsPnt
        (
            const   TPoint&                     pntTest
        )   const;

        tCIDLib::TBoolean bIntersects
        (
            const   TArea&                      areaTest
        )   const;

        tCIDLib::TBoolean bIsEmpty() const;

        tCIDLib::TBoolean bSetFromDev
        (
                    TGraphDrawDev&              gdevSrc
        );

        tCIDLib::TCard4 c4Rounding() const;

        tCIDLib::TVoid CombineWith
        (
            const   TGUIRegion&                 grgnCombineWith
            , const tCIDGraphDev::EClipModes    eMode
        );

        tCIDLib::TVoid CombineWith
        (
            const   TArea&                      areaCombineWith
            , const tCIDGraphDev::EClipModes    eMode
        );

        tCIDLib::TVoid Deflate
        (
            const   tCIDLib::TCard4             c4Horz
            , const tCIDLib::TCard4             c4Vert
        );

        tCIDGraphDev::TRegionHandle hrgnCopy() const;

        tCIDGraphDev::TRegionHandle hrgnThis() const;

        tCIDLib::TVoid Inflate
        (
            const   tCIDLib::TCard4             c4Horz
            , const tCIDLib::TCard4             c4Vert
        );

        tCIDLib::TVoid Offset
        (
            const   TPoint&                     pntBy
        );

        tCIDLib::TVoid Offset
        (
            const   tCIDLib::TInt4              i4XOfs
            , const tCIDLib::TInt4              i4YOfs
        );

        tCIDLib::TVoid Set
        (
            const   TArea&                      areaToSet
        );

        tCIDLib::TVoid Set
        (
            const   TArea&                      areaToSet
            , const tCIDLib::TCard4             c4Rounding
        );

        tCIDLib::TVoid SetOnDevice
        (
                    TGraphDrawDev&              gdevTarget
            , const tCIDGraphDev::EClipModes    eMode
        );


    private :
        // -------------------------------------------------------------------
        //  Private, non-virtual methods
        // -------------------------------------------------------------------
        tCIDGraphDev::TRegionHandle hrgnMakeCopy
        (
            const   tCIDGraphDev::TRegionHandle hrgnSrc
        )   const;


        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Rounding
        //      We can't get the rounding info back out of the region after it
        //      is created. So we remember it ourself so that client code can
        //      recover it.
        //
        //  m_hrgnThis
        //      This is our region handle, which we create in the ctor and
        //      destroy in the dtor.
        // -------------------------------------------------------------------
        tCIDLib::TCard4             m_c4Rounding;
        tCIDGraphDev::TRegionHandle m_hrgnThis;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGUIRegion,TObject)
};


// ---------------------------------------------------------------------------
//  CLASS: TRegionJanitor
// PREFIX: jan
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TRegionJanitor
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TRegionJanitor
        (
                    TGraphDrawDev* const        pgdevTarget
            ,       TGUIRegion&                 grgnToSet
            , const tCIDGraphDev::EClipModes    eMode
            , const tCIDLib::TBoolean           bClip = kCIDLib::True
        );

        TRegionJanitor
        (
                    TGraphDrawDev* const        pgdevTarget
            , const TArea&                      areaRegion
            , const tCIDGraphDev::EClipModes    eMode
            , const tCIDLib::TBoolean           bClip = kCIDLib::True
        );

        TRegionJanitor
        (
                    TGraphDrawDev* const        pgdevTarget
            , const TArea&                      areaRegion
            , const tCIDGraphDev::EClipModes    eMode
            , const tCIDLib::TCard4             c4Rounding
            , const tCIDLib::TBoolean           bClip = kCIDLib::True
        );

        ~TRegionJanitor();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hrgnOld
        //      The old region that we need to put back.
        //
        //  m_pgdevTarget
        //      The graphics device we put the new region on and must restore
        //      the old region to. If its zero, then the dtor won't restore.
        // -------------------------------------------------------------------
        tCIDGraphDev::TRegionHandle m_hrgnOld;
        TGraphDrawDev*              m_pgdevTarget;
};

#pragma CIDLIB_POPPACK


