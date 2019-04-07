//
// FILE NAME: MakeDocs_ThisFacility.cpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
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
//  This module implements the facility class for this program. This program's
//  facility class provides the centralized access to program functionality.
//  Everything goes through it, making it easy and convenient to coordinate
//  the program's work.
//
// CAVEATS/GOTCHAS:
//
// LOG:
//
//  $_CIDLib_Log_$
//

// -----------------------------------------------------------------------------
//  Include underlying headers.
// -----------------------------------------------------------------------------
#include    "MakeDocs.hpp"


// ----------------------------------------------------------------------------
//  Magic macros
// ----------------------------------------------------------------------------
RTTIDecls(TFacMakeDocs, TFacility)


// ----------------------------------------------------------------------------
//  CLASS: TFacMakeDocs
// PREFIX: fac
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  TFacMakeDocs: Constructors and destructors
// ----------------------------------------------------------------------------
TFacMakeDocs::TFacMakeDocs() :

    TFacility
    (
        L"MakeDocs"
        , tCIDLib::EModTypes::Exe
        , kCIDLib::c4MajVersion
        , kCIDLib::c4MinVersion
        , kCIDLib::c4Revision
        , tCIDLib::EModFlags::HasMsgFile
    )
{
}

TFacMakeDocs::~TFacMakeDocs()
{
}


// ----------------------------------------------------------------------------
//  TFacMakeDocs: Public, non-virtual methods
// ----------------------------------------------------------------------------

