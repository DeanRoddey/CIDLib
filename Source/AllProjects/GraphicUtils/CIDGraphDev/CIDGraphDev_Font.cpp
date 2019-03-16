//
// FILE NAME: CIDGraphDev_Font.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/25/2002
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
//  This file implements the font class
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include    "CIDGraphDev_.hpp"



// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TGUIFont,TObject)


// ---------------------------------------------------------------------------
//  CLASS: TGUIFont
// PREFIX: font
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TGUIFont: Constructors and Destructor
// ---------------------------------------------------------------------------
TGUIFont::TGUIFont() :

    m_fselAttrs()
    , m_hfontThis(kCIDGraphDev::hfontInvalid)
{
}

TGUIFont::TGUIFont(const TFontSelAttrs& fselAttrs) :

    m_fselAttrs(fselAttrs)
    , m_hfontThis(kCIDGraphDev::hfontInvalid)
{
}

TGUIFont::TGUIFont(const TGUIFont& gfontToCopy) :

    m_fselAttrs(gfontToCopy.m_fselAttrs)
    , m_hfontThis(kCIDGraphDev::hfontInvalid)
{
}

TGUIFont::~TGUIFont()
{
    // If we created the font, then delete it
    if (m_hfontThis != kCIDGraphDev::hfontInvalid)
        ::DeleteObject(m_hfontThis);
}


// ---------------------------------------------------------------------------
//  TGUIFont: Public operators
// ---------------------------------------------------------------------------
TGUIFont& TGUIFont::operator=(const TGUIFont& gfontToAssign)
{
    if (this != &gfontToAssign)
    {
        // Get rid of any existing font
        if (m_hfontThis != kCIDGraphDev::hfontInvalid)
        {
            ::DeleteObject(m_hfontThis);
            m_hfontThis = kCIDGraphDev::hfontInvalid;
        }

        // And copy the selection attributes
        m_fselAttrs = gfontToAssign.m_fselAttrs;
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TGUIFont: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TFontSelAttrs& TGUIFont::fselCurrent() const
{
    return m_fselAttrs;
}


tCIDGraphDev::TFontHandle TGUIFont::hfontThis() const
{
    //
    //  If we've not created the font yet, then lets fault it in at this
    //  point.
    //
    if (m_hfontThis == kCIDGraphDev::hfontInvalid)
    {
        m_hfontThis = ::CreateFontIndirect(&m_fselAttrs.HostInfo());
        if (m_hfontThis == kCIDGraphDev::hfontInvalid)
        {
            TKrnlError::SetLastHostError(::GetLastError());
            facCIDGraphDev().ThrowKrnlErr
            (
                CID_FILE
                , CID_LINE
                , kGrDevErrs::errcDev_CreateFont
                , TKrnlError::kerrLast()
                , tCIDLib::ESeverities::Failed
                , tCIDLib::EErrClasses::CantDo
                , m_fselAttrs.strFaceName()
            );
        }
    }
    return m_hfontThis;
}


tCIDLib::TVoid TGUIFont::SetSelAttrs(const TFontSelAttrs& fselToSet)
{
    //
    //  Store the new attrs, and delete the current font so that it will
    //  re-allocate the next time.
    //
    m_fselAttrs = fselToSet;
    if (m_hfontThis != kCIDGraphDev::hfontInvalid)
    {
        ::DeleteObject(m_hfontThis);
        m_hfontThis = kCIDGraphDev::hfontInvalid;
    }
}



// ---------------------------------------------------------------------------
//  CLASS: TFontJanitor
// PREFIX: jan
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TFontJanitor: Constructors and Destructor
// ---------------------------------------------------------------------------
TFontJanitor::TFontJanitor(         TGraphDrawDev* const  pgdevTarget
                            , const TGUIFont* const       pgfontToSet) :

    m_hfontRestore(kCIDGraphDev::hfontInvalid)
    , m_pgdevTarget(pgdevTarget)
    , m_pgfontSet(pgfontToSet)
{
    // Set the passed font and store the old handle
    if (m_pgfontSet)
        m_hfontRestore = pgdevTarget->hfontSetFont(*pgfontToSet);
}

TFontJanitor::~TFontJanitor()
{
    // If we set a font, then unset it. This will call our UnsetFromDev method
    if (m_pgfontSet)
        m_pgdevTarget->PopFont(*m_pgfontSet, m_hfontRestore);
}


// ---------------------------------------------------------------------------
//  TFontJanitor: Public, non-virtual methods
// ---------------------------------------------------------------------------
const TGUIFont* TFontJanitor::pgfontThis()
{
    return m_pgfontSet;
}

