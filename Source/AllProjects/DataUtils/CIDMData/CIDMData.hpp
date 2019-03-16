//
// FILE NAME: CIDMData.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 11/03/2015
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
//  This is the public header for the CIDMData Facility. This facility exists
//  just to provide some general purpose data oriented classes that we don't want
//  to put into CIDLib because it is not able to use the IDL compiler (too low level)
//  and that we don't want to put into GUI code because it might be used by back
//  end programs, and there's nowhere else that makes sense to put them.
//
//  We have the attribute data classes, which are key constituents of visual attribute
//  editing. It's used heavily in the CQC UI to query editable attributes of objects,
//  load them into the attribute editor window (in CIDWUtils) and then to write them
//  back to the object when changed.
//
//
//  We have the chunked file classes, which provide a simple chunked file format that
//  can be used by lots of stuff. It allows for various well known named chunks of
//  information, plus the application can create ones for itself. This provides a
//  simple way to include ancillary data in a file along with the primary data that
//  is being stored, which may be a well defined public format like PNG or whatever.
//  Of course PNG is itself a chunked format as well, so kind of nested chunked format
//  in that case.
//
//  Commonly in CQC it is used to store hash info and some extra info that we want
//  the data server to be able to serve up without having to understand the actual
//  data format. So some info from the main data will be redundantly stored in another
//  (meta) chunk for generic access without having to stream in the actual data. The
//  meta chunk is key=value pairs so that it is very easily accessed by any program.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//
#pragma once


// ---------------------------------------------------------------------------
//  Import any needed underlying headers
// ---------------------------------------------------------------------------
#include    "CIDLib.hpp"
#include    "CIDCrypto.hpp"


// ---------------------------------------------------------------------------
//  Set up our import/export attributes
// ---------------------------------------------------------------------------
#if     defined(PROJ_CIDMDATA)
#define CIDMDATAEXP    CID_DLLEXPORT
#else
#define CIDMDATAEXP    CID_DLLIMPORT
#endif



// ---------------------------------------------------------------------------
//  Include all of the public source module headers. These includes are in the
//  correct order of dependency so that the client application does not need
//  to worry about it.
// ---------------------------------------------------------------------------
#include    "CIDMData_ErrorIds.hpp"
#include    "CIDMData_Type.hpp"
#include    "CIDMData_Shared.hpp"
#include    "CIDMData_AttrData.hpp"
#include    "CIDMData_ChunkedFile.hpp"

// Slip in some more facility types
namespace tCIDMData
{
    using TAttrList = TVector<TAttrData>;
}

#include    "CIDMData_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility lazy evaluation object
// ---------------------------------------------------------------------------
extern CIDMDATAEXP TFacCIDMData& facCIDMData();

