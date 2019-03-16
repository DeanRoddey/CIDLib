//
// FILE NAME: CIDGraphDev_Font.hpp
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
//  This is the header for the CIDGraphDev_Font.cpp file. This file implements the
//  TGUIFont class, which represents a realized font that can be selected into
//  a device.
//
// CAVEATS/GOTCHAS:
//
//  1)  Unlike some other GUI resources, fonts can be set on multiple devices, so it's
//      a lot tricker to try to do the checks as to whether the font is selected or not
//      which is otherwise used to catch bogus (unbalanced) deselects. So we just don't
//      to do it for fonts.
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


#pragma CIDLIB_PACK(CIDLIBPACK)

// ---------------------------------------------------------------------------
//  CLASS: TGUIFont
// PREFIX: gfont
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TGUIFont : public TObject
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TGUIFont();

        TGUIFont
        (
            const   TFontSelAttrs&          fselAttrs
        );

        TGUIFont
        (
            const   TGUIFont&               gfontToCopy
        );

        ~TGUIFont();


        // -------------------------------------------------------------------
        //  Public operators
        // -------------------------------------------------------------------
        TGUIFont& operator=
        (
            const   TGUIFont&               gfontToAssign
        );


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TFontSelAttrs& fselCurrent() const;

        tCIDGraphDev::TFontHandle hfontThis() const;

        tCIDLib::TVoid SetSelAttrs
        (
            const   TFontSelAttrs&          fselToSet
        );


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_fselAttrs
        //      The font selection attributes. We need this in order to do
        //      copy/assignment, since we have to be able to recreate the
        //      font in the new object.
        //
        //  m_hfontThis
        //      This is our font handle, which we lazy fault in and destroy
        //      in the dtor if it got created.
        // -------------------------------------------------------------------
        TFontSelAttrs                       m_fselAttrs;
        mutable tCIDGraphDev::TFontHandle   m_hfontThis;


        // -------------------------------------------------------------------
        //  Magic macros
        // -------------------------------------------------------------------
        RTTIDefs(TGUIFont,TObject)
};


// ---------------------------------------------------------------------------
//  CLASS: TFontJanitor
// PREFIX: jan
// ---------------------------------------------------------------------------
class CIDGRDEVEXP TFontJanitor
{
    public  :
        // -------------------------------------------------------------------
        //  Constructors and Destructor
        // -------------------------------------------------------------------
        TFontJanitor
        (
                    TGraphDrawDev* const    pgdevTarget
            , const TGUIFont* const         pgfontToSet
        );

        ~TFontJanitor();


        // -------------------------------------------------------------------
        //  Public, non-virtual methods
        // -------------------------------------------------------------------
        const TGUIFont* pgfontThis();


    private :
        // -------------------------------------------------------------------
        //  Private data members
        //
        //  m_hfontRestore
        //      The old font we must restore.
        //
        //  m_pgdevTarget
        //      The graphics device we put the font on and must restore the
        //      old font to.
        //
        //  m_pgfontSet
        //      The font that we set and must remove on destruction. It might
        //      be null, in which case we do nothing.
        // -------------------------------------------------------------------
        tCIDGraphDev::TFontHandle   m_hfontRestore;
        TGraphDrawDev*              m_pgdevTarget;
        const TGUIFont*             m_pgfontSet;
};

#pragma CIDLIB_POPPACK


