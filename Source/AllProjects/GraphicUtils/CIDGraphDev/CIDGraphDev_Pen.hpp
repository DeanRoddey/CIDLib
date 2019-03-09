//
// FILE NAME: CIDGraphDev_Pen.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/24/2002
//
// COPYRIGHT: $_CIDLib_CopyRight_$
//
//  $_CIDLib_CopyRight2_$
//
// DESCRIPTION:
//
//  This file implements the pen classes. Pens represent GUI objects
//  that can be used to draw lines of varying widths and styles. It has
//  attributes such as mix mode, style, end join, etc... The base class is
//  TGUIPen, which allows code to accept different types of pens
//  polymorphically, while maintaining simpler interfaces for creating
//  each type of pen.
//
//  We also define the TPenJanitor class, which will apply a pen to a
//  graphic device on a scoped basis, which is how they almost always are
//  applied, returning the original pen upon exit from the scope. It a very
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
//  CLASS: TGUIPen
// PREFIX: gpen
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TGUIPen : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TGUIPen(const TGUIPen&) = delete;

        ~TGUIPen();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGUIPen& operator=(const TGUIPen&) = delete;


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        tCIDLib::TBoolean bSetOnDevice() const;

        tCIDLib::TBoolean bSetOnDevice
        (
            const   tCIDLib::TBoolean           bToSet
        );

        tCIDGraphDev::TPenHandle hpenThis() const;

        tCIDLib::TVoid Unset
        (
                    TGraphDrawDev&              gdevTarget
            ,       tCIDGraphDev::TPenHandle    hpenToPutBack
        );


    protected :
        // -------------------------------------------------------------------
        //  Hidden constructors
        // -------------------------------------------------------------------
        TGUIPen();


        // -------------------------------------------------------------------
        //  Protected, virtual methods
        // -------------------------------------------------------------------
        virtual tCIDGraphDev::TPenHandle hpenCreatePen() const = 0;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_bSetOnDevice
        //      We keep up with whether we are currently set on a device,
        //      so that we can catch errors of double sets or unsets when
        //      we aren't set.
        //
        //  m_hpenThis
        //      The handle to the pen. We fault it in upon first use. The
        //      derived class creates it, and we store it.
        // -------------------------------------------------------------------
        tCIDLib::TBoolean                m_bSetOnDevice;
        mutable tCIDGraphDev::TPenHandle m_hpenThis;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGUIPen,TObject)
        NulObject(TGUIPen)
};


// ---------------------------------------------------------------------------
//  CLASS: TCosmeticPen
// PREFIX: gpen
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TCosmeticPen : public TGUIPen
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TCosmeticPen
        (
            const   TRGBClr&                    rgbClr
            , const tCIDGraphDev::ELineStyles   eStyle = tCIDGraphDev::ELineStyles::Solid
        );

        TCosmeticPen(const TCosmeticPen&) = delete;

        ~TCosmeticPen();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TCosmeticPen& operator=(const TCosmeticPen&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDGraphDev::TPenHandle hpenCreatePen() const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_eStyle
        //      The line style, i.e. dashed, dotted, etc...
        //
        //  m_rgbClr
        //      The color used as the line color for this pen.
        // -------------------------------------------------------------------
        tCIDGraphDev::ELineStyles   m_eStyle;
        TRGBClr                     m_rgbClr;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TCosmeticPen,TGUIPen)
};


// ---------------------------------------------------------------------------
//  CLASS: TGeomPen
// PREFIX: gpen
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TGeomPen : public TGUIPen
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TGeomPen
        (
            const   TRGBClr&                    rgbClr
            , const tCIDLib::TCard4             c4Width
        );

        TGeomPen
        (
            const   TRGBClr&                    rgbClr
            , const tCIDLib::TCard4             c4Width
            , const tCIDGraphDev::ELineEnds     eEndType
            , const tCIDGraphDev::ELineJoins    eJoinType
            , const tCIDGraphDev::ELineStyles   eStyle
        );

        TGeomPen
        (
            const   TRGBClr&                    rgbClr
            , const tCIDLib::TCard4             c4Width
            , const tCIDGraphDev::ELineEnds     eEndType
            , const tCIDGraphDev::ELineJoins    eJoinType
            , const tCIDGraphDev::EPatterns     ePattern
        );

        TGeomPen(const TGeomPen&) = delete;

        ~TGeomPen();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGeomPen& operator=(const TGeomPen&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDGraphDev::TPenHandle hpenCreatePen() const override;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_c4Width
        //      The width in logical units of the line
        //
        //  m_eEndType
        //  m_eJoinType
        //      Controls the way that lines butt together and how the end
        //      caps are drawn.
        //
        //  m_eStyle
        //      Controls the style, e.g. dotted, dashed, etc... Note that
        //      the 'alternate' scheme is not legal for geometric pens and
        //      and will cause an error.
        //
        //  m_rgbClr
        //      The color used as the fill color in this pen.
        // -------------------------------------------------------------------
        tCIDLib::TCard4             m_c4Width;
        tCIDGraphDev::ELineEnds     m_eEndType;
        tCIDGraphDev::ELineJoins    m_eJoinType;
        tCIDGraphDev::EPatterns     m_ePattern;
        tCIDGraphDev::ELineStyles   m_eStyle;
        TRGBClr                     m_rgbClr;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGeomPen,TGUIPen)
};



// ---------------------------------------------------------------------------
//  CLASS: TNullPen
// PREFIX: gpen
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TNullPen : public TGUIPen
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TNullPen();

        TNullPen(const TNullPen&) = delete;

        ~TNullPen();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TNullPen& operator=(const TNullPen&) = delete;


    protected :
        // -------------------------------------------------------------------
        //  Protected, inherited methods
        // -------------------------------------------------------------------
        tCIDGraphDev::TPenHandle hpenCreatePen() const override;


    private :
        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TNullPen,TGUIPen)
};



// ---------------------------------------------------------------------------
//  CLASS: TPenJanitor
// PREFIX: jan
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TPenJanitor
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TPenJanitor() = delete;

        TPenJanitor
        (
                    TGraphDrawDev* const        pgdevTarget
            ,       TGUIPen* const              pgpenToSet
        );

        TPenJanitor(const  TPenJanitor&) = delete;

        ~TPenJanitor();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TPenJanitor& operator=(const TPenJanitor&) = delete;


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hpenRestore
        //      The old pen we must restore.
        //
        //  m_pgdevTarget
        //      The graphics device we put the pen on and must restore the
        //      old pen to.
        //
        //  m_pgpenSet
        //      The pen that we set and must remove on destruction.
        // -------------------------------------------------------------------
        tCIDGraphDev::TPenHandle    m_hpenRestore;
        TGraphDrawDev*              m_pgdevTarget;
        TGUIPen*                    m_pgpenSet;
};

#pragma CIDLIB_POPPACK


