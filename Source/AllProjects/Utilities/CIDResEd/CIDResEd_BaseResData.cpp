//
// FILE NAME: CIDResEd_BaseResData.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/26/2010
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
//  We need some basic classes to represent the resource data we are editing.
//  Some of these may need to be streamable, but only for clipboard cut/paste
//  purposes, it's not persisted. The actual persisted format is writing back
//  to the CIDRC and MsgText files. So we don't need to do any version
//  formatting on streamed data here.
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
#include "CIDResEd.hpp"


// ---------------------------------------------------------------------------
//  Magic Macros
// ---------------------------------------------------------------------------
RTTIDecls(TTextSym,TObject)



// ----------------------------------------------------------------------------
//   CLASS: TTextSym
//  PREFIX: tsym
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TTextSym: Constructors and Destructor
// ----------------------------------------------------------------------------
TTextSym::TTextSym(const TString& strSym, const tCIDResEd::EMsgTypes eType)
{
    Set(strSym, eType);
}

// ----------------------------------------------------------------------------
//  TTextSym: Public operators
// ----------------------------------------------------------------------------
tCIDLib::TBoolean TTextSym::operator==(const TTextSym& tsymToComp) const
{
    return (m_eType == tsymToComp.m_eType)
           && (m_strSym == tsymToComp.m_strSym);
}

tCIDLib::TBoolean TTextSym::operator!=(const TTextSym& tsymToComp) const
{
    return !operator==(tsymToComp);
}


// ----------------------------------------------------------------------------
//  TTextSym: Public, non-virtual methods
// ----------------------------------------------------------------------------

// If the real symbolic name is empty, it's not valid
tCIDLib::TBoolean TTextSym::bIsValid() const
{
    return !m_strRealSym.bIsEmpty();
}


tCIDResEd::EMsgTypes TTextSym::eType() const
{
    return m_eType;
}


tCIDLib::TVoid TTextSym::Reset()
{
    m_eType = tCIDResEd::EMsgTypes::Message;
    m_strSym.Clear();
    m_strRealSym.Clear();
}


// Return the real symbolic name, which may be different from as displayed
const TString& TTextSym::strRealSym() const
{
    return m_strRealSym;
}


// Return the display symbolic name
const TString& TTextSym::strSymName() const
{
    return m_strSym;
}


tCIDLib::TVoid
TTextSym::Set(const TString& strSym, const tCIDResEd::EMsgTypes eType)
{
    m_eType  = eType;
    m_strSym = strSym;
    m_strRealSym = m_strSym;

    // If common text, we have to swizzle the id
    if (eType == tCIDResEd::EMsgTypes::Common)
    {
        m_strRealSym.Cut(0, 1);
        m_strRealSym.DeleteLast();
        m_strRealSym.Prepend(L"midCTxt_");
    }
}


// -------------------------------------------------------------------
//  TTextSym: Protected, inherited methods
// -------------------------------------------------------------------
tCIDLib::TVoid TTextSym::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Our stuff should start with a frame marker
    strmToReadFrom.CheckForFrameMarker(CID_FILE, CID_LINE);

    strmToReadFrom  >> m_eType
                    >> m_strSym
                    >> m_strRealSym;

    // Make sure we get the end marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TTextSym::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << tCIDLib::EStreamMarkers::Frame
                    << m_eType
                    << m_strSym
                    << m_strRealSym
                    << tCIDLib::EStreamMarkers::EndObject;
}

