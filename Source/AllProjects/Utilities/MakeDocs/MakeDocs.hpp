//
// FILE NAME: MakeDocs.hpp
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
//  This is the main header for the program. It brings in any other headers
//  that are needed. All our cpp files include this.
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
#include    "CIDLib.hpp"


// -----------------------------------------------------------------------------
//  Include our intra-facility headers
// -----------------------------------------------------------------------------
#include    "MakeDocs_ErrorIds.hpp"
#include    "MakeDocs_MessageIds.hpp"
#include    "MakeDocs_ParserError.hpp"
#include    "MakeDocs_Parser.hpp"
#include    "MakeDocs_ThisFacility.hpp"


// -----------------------------------------------------------------------------
//  Export the facility object internally
// -----------------------------------------------------------------------------
extern TFacMakeDocs facMakeDocs;
