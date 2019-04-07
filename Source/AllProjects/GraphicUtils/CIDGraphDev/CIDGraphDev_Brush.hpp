//
// FILE NAME: CIDGraphDev_Brush.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/24/2002
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
//  This file implements the brush classes. Brushes represent GUI objects
//  that can be used to fill background areas. It has attributes such as
//  mix mode, pattern, fill color, etc... The base class is TGUIBrush, which
//  allows code to accept different types of brushes polymorphically, while
//  maintaining simpler interfaces for creating each type of brush.
//
//  We also define the TBrushJanitor class, which will apply a brush to a
//  graphic device on a scoped basis, which is how they almost always are
//  applied, returning the original brush upon exit from the scope. It a very
//  simple class, so we don't bother making it a TObject derivative.
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
//  CLASS: TGUIBrush
// PREFIX: gbr
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TGUIBrush : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        ~TGUIBrush();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDGraphDev::TBrushHandle hbrThis() const;

        tCIDGraphDev::TBrushHandle hbrSetOnDevice
        (
                    TGraphDrawDev&              gdevTarget
        )   const;

        tCIDLib::TVoid Unset
        (
                    TGraphDrawDev&              gdevTarget
            , const tCIDGraphDev::TBrushHandle  hbrToPutBack
        )   const;


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TGUIBrush();

        TGUIBrush
        (
            const   TGUIBrush&                  gbrToCopy
        );

        tCIDLib::TVoid operator=
        (
            const   TGUIBrush&                  gbrToAssign
        );


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDGraphDev::TBrushHandle hbrCreateBrush() const = 0;


        // -------------------------------------------------------------------
        //  Protected, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Reset();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bSetOnDevice
        //      We keep up with whether we are currently set on a device,
        //      so that we can catch errors of double sets or unsets when
        //      we aren't set.
        //
        //  m_hbrThis
        //      The handle to the brush. We fault it in upon first use. The
        //      derived class creates it, and we store it.
        // -------------------------------------------------------------------
        mutable tCIDLib::TBoolean           m_bSetOnDevice;
        mutable tCIDGraphDev::TBrushHandle  m_hbrThis;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGUIBrush,TObject)
        NulObject(TGUIBrush)
};


// ---------------------------------------------------------------------------
//  CLASS: TSolidBrush
// PREFIX: gbr
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TSolidBrush : public TGUIBrush
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TSolidBrush();

        TSolidBrush
        (
            const   TRGBClr&                rgbClr
        );

        TSolidBrush
        (
            const   TSolidBrush&            gbrToCopy
        );

        ~TSolidBrush();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        TSolidBrush& operator=
        (
            const   TSolidBrush&            gbrToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TRGBClr& rgbColor() const;

        tCIDLib::TVoid SetColor
        (
            const   TRGBClr&                rgbToSet
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDGraphDev::TBrushHandle hbrCreateBrush() const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_rgbClr
        //      The color used as the fill color in this brush.
        // -------------------------------------------------------------------
        TRGBClr     m_rgbClr;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TSolidBrush,TGUIBrush)
};


// ---------------------------------------------------------------------------
//  CLASS: THatchedBrush
// PREFIX: gbr
// ---------------------------------------------------------------------------
class CIDGRDEVEXP THatchedBrush : public TGUIBrush
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        THatchedBrush();

        THatchedBrush
        (
            const   TRGBClr&                rgbFgnClr
            , const tCIDGraphDev::EPatterns ePattern
        );

        THatchedBrush
        (
            const   THatchedBrush&          gbrToCopy
        );

        ~THatchedBrush();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        THatchedBrush& operator=
        (
            const   THatchedBrush&          gbrToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TVoid Set
        (
            const   TRGBClr&                rgbFgnClr
            , const tCIDGraphDev::EPatterns ePattern
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDGraphDev::TBrushHandle hbrCreateBrush() const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_ePattern
        //      The pattern that we use on this brush.
        //
        //  m_rgbClr
        //      The color used as the fill color in this brush.
        // -------------------------------------------------------------------
        tCIDGraphDev::EPatterns m_ePattern;
        TRGBClr                 m_rgbClr;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(THatchedBrush,TGUIBrush)
};



// ---------------------------------------------------------------------------
//  CLASS: TBitmapBrush
// PREFIX: gbr
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TBitmapBrush : public TGUIBrush
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBitmapBrush
        (
            const   TBitmap&                bmpToUse
        );

        TBitmapBrush
        (
            const   TBitmapBrush&           gbrToCopy
        );

        ~TBitmapBrush();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TBitmapBrush& operator=
        (
            const   TBitmapBrush&           gbrToAssign
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDGraphDev::TBrushHandle hbrCreateBrush() const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_pbmpToUse
        //      This is the bitmap that we use as the fill pattern. Because of
        //      circular dependency issues, we have to use a pointer here, but
        //      we do make a copy of the bitmap. Bitmaps are just ref counters
        //      anyway, so this is light.
        // -------------------------------------------------------------------
        TBitmap*    m_pbmpToUse;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TBitmapBrush,TGUIBrush)
};


// ---------------------------------------------------------------------------
//  CLASS: TNullBrush
// PREFIX: gbr
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TNullBrush : public TGUIBrush
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TNullBrush();

        TNullBrush
        (
            const   TNullBrush&          gbrToCopy
        );

        ~TNullBrush();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TNullBrush& operator=
        (
            const   TNullBrush&          gbrToAssign
        );


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDGraphDev::TBrushHandle hbrCreateBrush() const override;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TNullBrush,TGUIBrush)
};



// ---------------------------------------------------------------------------
//  CLASS: TBrushJanitor
// PREFIX: jan
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TBrushJanitor
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TBrushJanitor
        (
                    TGraphDrawDev* const    pgdevTarget
            , const TGUIBrush* const        pgbrToSet
            , const tCIDLib::EAdoptOpts     eAdopt = tCIDLib::EAdoptOpts::NoAdopt
        );

        ~TBrushJanitor();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eAdopt
        //      Indicates whether we adopted the brush and should delete it
        //      when we are done with it.
        //
        //  m_hbrRestore
        //      The old brush we must restore.
        //
        //  m_pgdevTarget
        //      The graphics device we put the brush on and must restore the
        //      old brush to.
        //
        //  m_pgbrSet
        //      The brush that we set and must remove on destruction.
        // -------------------------------------------------------------------
        tCIDLib::EAdoptOpts         m_eAdopt;
        tCIDGraphDev::TBrushHandle  m_hbrRestore;
        TGraphDrawDev*              m_pgdevTarget;
        const TGUIBrush*            m_pgbrSet;
};

#pragma CIDLIB_POPPACK


