//
// FILE NAME: CIDPack.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 03/21/2015
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
//  This is the public header for the CIDPack Facility. This facility provides
//  an engine for compressing and storing lists of files into a package that
//  can be later decompressed, to get all of the files back out. Though not
//  supported yet, it has the appropriate flags to allow for encryption of the
//  data later if desired.
//
//  The primary purpose of this facility is to support installers, so there's
//  not support currently for incremental addition or removal of files. They
//  are all processed at once and the package created. Building a package doesn't
//  depend on a seekable output stream, it's all done linearly as we go with
//  enough data written at each step to get to the next chunk of data. There's no
//  table at the front or anything that would have to be gone back to and updated
//  once we've written out the files.
//
//  The files of coures must be compressed separately, which will be done in
//  memory, and when the amount of compressed data is known, we can write the
//  header for that file, followed by the compressed data.
//
//  The structure of the file is:
//
//      Package Header
//          File 1 Header, which has size of compressed data
//          File 1 Compressed data
//          File 2 Header, which has size of compressed data
//          File 2 Compressed data
//          .....
//
//  So we can read in file 1's header, which tells us how much compressed data to
//  read, which we can use to expand file one. And this leaves us at the header
//  for file 2 and so forth. So we just stream through it linearly. Given a seek
//  capable input stream, you can scan forward for a specific file by jumping over
//  compressed data and just reading headers. But you have to do a linear search
//  through the headers, there's no lookup table.
//
//  Each file's uncompressed data is MD5 hashed, so that we can be sure that we
//  get back out what we put in.
//
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



// ---------------------------------------------------------------------------
//  Set up our import/export attributes
// ---------------------------------------------------------------------------
#if     defined(PROJ_CIDPACK)
#define CIDPACKEXP  CID_DLLEXPORT
#else
#define CIDPACKEXP  CID_DLLIMPORT
#endif


// ---------------------------------------------------------------------------
//  Facility constants
// ---------------------------------------------------------------------------
namespace kCIDPack
{
    // -----------------------------------------------------------------------
    //  This is a marker sequence that is written to the very start of a CIDPack
    //  file that we create. It is basically the ASCII for:
    //
    //      (CIDPack) V1
    // -----------------------------------------------------------------------
    const tCIDLib::TCard1 ac1Marker[] =
    {
        0x28, 0x43, 0x49, 0x44, 0x50, 0x61, 0x63, 0x6B, 0x29, 0x20, 0x56, 0x31
    };
    const tCIDLib::TCard4 c4MarkerLen = tCIDLib::c4ArrayElems(ac1Marker);
}



// ---------------------------------------------------------------------------
//  Include all of the public source module headers. These includes are in the
//  correct order of dependency so that the client application does not need
//  to worry about it.
// ---------------------------------------------------------------------------
#include    "CIDPack_ErrorIds.hpp"
#include    "CIDPack_Type.hpp"
#include    "CIDPack_ThisFacility.hpp"


// ---------------------------------------------------------------------------
//  Export the facility lazy evaluation object
// ---------------------------------------------------------------------------
extern CIDPACKEXP TFacCIDPack& facCIDPack();


