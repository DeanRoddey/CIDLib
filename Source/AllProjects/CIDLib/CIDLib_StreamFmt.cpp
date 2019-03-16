//
// FILE NAME: CIDLib_StreamFmt.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 04/03/1994
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
//  This file implements the TStreamFmt class, which is a helper class for
//  the TTextOutStream class. It provides a way to manipulate all of the
//  formatting attributes of a text stream as a whole.
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
#include    "CIDLib_.hpp"


// ---------------------------------------------------------------------------
//  Do our standard members macros
// ---------------------------------------------------------------------------
AdvRTTIDecls(TStreamFmt,TObject)



// ---------------------------------------------------------------------------
//   CLASS: TStreamFmt
//  PREFIX: strmf
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TStreamFmt: Public, static methods
// ---------------------------------------------------------------------------
TStreamFmt& TStreamFmt::Nul_TStreamFmt()
{
    static TStreamFmt* pstrmfNull = 0;
    if (!pstrmfNull)
    {
        TBaseLock lockInit;
        if (!pstrmfNull)
            TRawMem::pExchangePtr(&pstrmfNull, new TStreamFmt);
    }
    return *pstrmfNull;
}


// ---------------------------------------------------------------------------
//  TStreamFmt: Constructors and Destructor
// ---------------------------------------------------------------------------
TStreamFmt::TStreamFmt( const   tCIDLib::TCard4     c4Width
                        , const tCIDLib::TCard4     c4Precision
                        , const tCIDLib::EHJustify  eJustification
                        , const tCIDLib::TCard4     c4TrailingSp) :
    m_c4Precision(c4Precision)
    , m_c4TrailingSp(c4TrailingSp)
    , m_c4Width(c4Width)
    , m_eJustification(eJustification)
{
}

TStreamFmt::TStreamFmt( const   tCIDLib::TCard4     c4Width
                        , const tCIDLib::TCard4     c4Precision
                        , const tCIDLib::EHJustify  eJustification
                        , const tCIDLib::TCh        chFill
                        , const tCIDLib::TCard4     c4TrailingSp) :

    m_c4Precision(c4Precision)
    , m_c4TrailingSp(c4TrailingSp)
    , m_c4Width(c4Width)
    , m_chFill(chFill)
    , m_eJustification(eJustification)
{
}

TStreamFmt::TStreamFmt( const   tCIDLib::TCard4     c4Width
                        , const tCIDLib::TCard4     c4Precision
                        , const tCIDLib::EHJustify  eJustification
                        , const tCIDLib::TCh        chFill
                        , const tCIDLib::ERadices   eRadix
                        , const tCIDLib::TCard4     c4TrailingSp) :

    m_c4Precision(c4Precision)
    , m_c4TrailingSp(c4TrailingSp)
    , m_c4Width(c4Width)
    , m_chFill(chFill)
    , m_eJustification(eJustification)
    , m_eRadix(eRadix)
{
}

TStreamFmt::TStreamFmt(const tCIDLib::ERadices eRadix) :

    m_eRadix(eRadix)
{
}

TStreamFmt::TStreamFmt(const TTextOutStream& strmSrc) :

    m_c4Precision(strmSrc.m_c4Precision)
    , m_c4TrailingSp(strmSrc.m_c4TrailingSp)
    , m_c4Width(strmSrc.m_c4Width)
    , m_chFill(strmSrc.m_chFill)
    , m_eJustification(strmSrc.m_eJustification)
    , m_eRadix(strmSrc.m_eRadix)
{
}


// ---------------------------------------------------------------------------
//  TStreamFmt: Public operators
// ---------------------------------------------------------------------------
tCIDLib::TBoolean TStreamFmt::operator==(const TStreamFmt& strmfSrc) const
{
    if (this != &strmfSrc)
    {
        if ((m_c4Precision != strmfSrc.m_c4Precision)
        ||  (m_c4TrailingSp != strmfSrc.m_c4TrailingSp)
        ||  (m_c4Width != strmfSrc.m_c4Width)
        ||  (m_chFill != strmfSrc.m_chFill)
        ||  (m_eJustification != strmfSrc.m_eJustification)
        ||  (m_eRadix != strmfSrc.m_eRadix))
        {
            return kCIDLib::False;
        }
    }
    return kCIDLib::True;
}

tCIDLib::TBoolean TStreamFmt::operator!=(const TStreamFmt& strmfSrc) const
{
    return !operator==(strmfSrc);
}


// ---------------------------------------------------------------------------
//  TStreamFmt: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TCh TStreamFmt::chFill() const
{
    return m_chFill;
}

tCIDLib::TCh TStreamFmt::chFill(const tCIDLib::TCh chToSet)
{
    m_chFill = chToSet;
    return m_chFill;
}


tCIDLib::TCard4 TStreamFmt::c4Precision() const
{
    return m_c4Precision;
}

tCIDLib::TCard4 TStreamFmt::c4Precision(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Precision = c4ToSet;
    return m_c4Precision;
}


tCIDLib::TCard4 TStreamFmt::c4TrailingSpaces() const
{
    return m_c4TrailingSp;
}

tCIDLib::TCard4 TStreamFmt::c4TrailingSpaces(const tCIDLib::TCard4 c4ToSet)
{
    m_c4TrailingSp = c4ToSet;
    return m_c4TrailingSp;
}


tCIDLib::TCard4 TStreamFmt::c4Width() const
{
    return m_c4Width;
}

tCIDLib::TCard4 TStreamFmt::c4Width(const tCIDLib::TCard4 c4ToSet)
{
    m_c4Width = c4ToSet;
    return m_c4Width;
}


tCIDLib::EHJustify TStreamFmt::eJustification() const
{
    return m_eJustification;
}

tCIDLib::EHJustify
TStreamFmt::eJustification(const tCIDLib::EHJustify eToSet)
{
    m_eJustification = eToSet;
    return m_eJustification;
}


tCIDLib::ERadices TStreamFmt::eRadix() const
{
    return m_eRadix;
}

tCIDLib::ERadices TStreamFmt::eRadix(const tCIDLib::ERadices eToSet)
{
    m_eRadix = eToSet;
    return m_eRadix;
}

