//
// FILE NAME: CIDMacroDbg_StateInfo.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 02/19/2003
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
//  This file implements our derivative of the list box item, which is used
//  to put parser error info into a list box for the user to view.
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
#include    "CIDMacroDbg_.hpp"


// ---------------------------------------------------------------------------
//  Do our RTTI macros
// ---------------------------------------------------------------------------
RTTIDecls(TMacroDbgStateInfo,TObject)


// ---------------------------------------------------------------------------
//  Local data
// ---------------------------------------------------------------------------
namespace CIDMacroDbg_StateInfo
{
    // Our current persistent format version
    const   tCIDLib::TCard2     c2FmtVersion = 1;
}


// ---------------------------------------------------------------------------
//   CLASS: TMacroDbgStateInfo
//  PREFIX: sti
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  TMacroDbgStateInfo: Constructors and Destructor
// ---------------------------------------------------------------------------
TMacroDbgStateInfo::TMacroDbgStateInfo()
{
}

TMacroDbgStateInfo::TMacroDbgStateInfo(const TMacroDbgStateInfo& stiToCopy)
{
}

TMacroDbgStateInfo::~TMacroDbgStateInfo()
{
}


// ---------------------------------------------------------------------------
//  TMacroDbgStateInfo: Public operators
// ---------------------------------------------------------------------------
TMacroDbgStateInfo&
TMacroDbgStateInfo::operator=(const TMacroDbgStateInfo& stiToAssign)
{
    if (this != &stiToAssign)
    {
    }
    return *this;
}


// ---------------------------------------------------------------------------
//  TMacroDbgStateInfo: Public, non-virtual methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMacroDbgStateInfo::Reset()
{
}


// ---------------------------------------------------------------------------
//  TMacroDbgStateInfo: Protected, inherited methods
// ---------------------------------------------------------------------------
tCIDLib::TVoid TMacroDbgStateInfo::StreamFrom(TBinInStream& strmToReadFrom)
{
    strmToReadFrom.CheckForStartMarker(CID_FILE, CID_LINE);

    // Check the format version
    tCIDLib::TCard2 c2FmtVersion;
    strmToReadFrom >> c2FmtVersion;
    if (c2FmtVersion != CIDMacroDbg_StateInfo::c2FmtVersion)
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

    strmToReadFrom.CheckForEndMarker(CID_FILE, CID_LINE);
}

tCIDLib::TVoid TMacroDbgStateInfo::StreamTo(TBinOutStream& strmToWriteTo) const
{
    // Start with a marker and version format
    strmToWriteTo   << tCIDLib::EStreamMarkers::StartObject
                    << CIDMacroDbg_StateInfo::c2FmtVersion;


    // End it off with a marker
    strmToWriteTo   << tCIDLib::EStreamMarkers::EndObject;
}


