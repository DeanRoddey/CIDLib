//
// FILE NAME: CIDResEd_CBData.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 05/29/2010
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
//  This file implements the classes that are used to put stuff on the
//  clip board.
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
AdvRTTIDecls(TResEdCBItems,TObject)


// ---------------------------------------------------------------------------
//  Local types and data
// ---------------------------------------------------------------------------
namespace CIDResEd_DBData
{
    // -----------------------------------------------------------------------
    //  Our persistent format version
    // -----------------------------------------------------------------------
    const tCIDLib::TCard2   c2FmtVersion = 1;
}



// ---------------------------------------------------------------------------
//   CLASS: TResEdCBItems
//  PREFIX: cbi
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TResEdCBItems: Constructors and Destructor
// ---------------------------------------------------------------------------
TResEdCBItems::TResEdCBItems()
{
}

TResEdCBItems::TResEdCBItems(const tCIDResEd::TDlgItemRefList& colItems) :

    m_colItems(colItems.c4ElemCount())
{
    // Make copies of all the widgets in the list
    tCIDLib::TCard4 c4Count = colItems.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
        m_colItems.objAdd(*colItems[c4Index]);
}

TResEdCBItems::~TResEdCBItems()
{
}



// ---------------------------------------------------------------------------
//  TResEdCBItems: Public, non-virtual methods
// ---------------------------------------------------------------------------
const tCIDResEd::TDlgItemList& TResEdCBItems::colItems() const
{
    return m_colItems;
}


// ---------------------------------------------------------------------------
//  TResEdCBItems: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TResEdCBItems::StreamFrom(TBinInStream& strmToReadFrom)
{
    // Our stuff should start with a frame marker
    strmToReadFrom.CheckForFrameMarker(CID_FILE, CID_LINE);

    // And next should be the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom  >> c2FmtVersion;
    if (!c2FmtVersion || (c2FmtVersion > CIDResEd_DBData::c2FmtVersion))
    {
        facCIDLib().ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kCIDErrs::errcGen_UnknownFmtVersion
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
            , TCardinal(c2FmtVersion)
            , clsThis()
        );
    }

    // And get our stuff out
    tCIDLib::TCard4 c4Count;
    tCIDLib::TCard4 c4XorCount;
    strmToReadFrom  >> c4Count >> c4XorCount;
    if ((c4Count ^ kCIDLib::c4MaxCard) != c4XorCount)
    {
        facCIDResEd.ThrowErr
        (
            CID_FILE
            , CID_LINE
            , kResEdErrs::errcCB_BadCount
            , tCIDLib::ESeverities::Failed
            , tCIDLib::EErrClasses::Format
        );
    }

    // Get rid of any current content and reallocate to the needed size
    m_colItems.Reallocate(c4Count, kCIDLib::False);

    TResEdDlgItem reditCur;
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        strmToReadFrom >> reditCur;
        m_colItems.objAdd(reditCur);
        strmToReadFrom.CheckForFrameMarker(CID_FILE, CID_LINE);
    }

    // Make sure we get the end marker
    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}


tCIDLib::TVoid TResEdCBItems::StreamTo(TBinOutStream& strmToWriteTo) const
{
    strmToWriteTo   << tCIDLib::EStreamMarkers::Frame
                    << CIDResEd_DBData::c2FmtVersion
                    << m_colItems.c4ElemCount()
                    << tCIDLib::TCard4(m_colItems.c4ElemCount() ^ kCIDLib::c4MaxCard);

    const tCIDLib::TCard4 c4Count = m_colItems.c4ElemCount();
    for (tCIDLib::TCard4 c4Index = 0; c4Index < c4Count; c4Index++)
    {
        strmToWriteTo << m_colItems[c4Index]
                      << tCIDLib::EStreamMarkers::Frame;
    }

    strmToWriteTo   << tCIDLib::EStreamMarkers::EndObject;
}


