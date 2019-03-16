//
// FILE NAME: MakeDocs.hpp
//
// AUTHOR: Dean Roddey
//
// CREATED: 06/10/1997
//
// COPYRIGHT: Charmed Quark Systems, Ltd - 2019
//
//  This file is part of a demonstration program of the CIDLib C++
//  Frameworks. Its contents are distributed 'as is', to provide guidance on
//  the use of the CIDLib system. However, these demos are not intended to
//  represent a full fledged applications. Any direct use of demo code in
//  user applications is at the user's discretion, and no warranties are
//  implied as to its correctness or applicability.
//
// DESCRIPTION:
//
//  This is the main header for the program. It brings in any other headers
//  that are needed.
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
#include    "MakeDocs_Type.hpp"
#include    "MakeDocs_ErrorIds.hpp"
#include    "MakeDocs_MessageIds.hpp"

#include    "MakeDocs_Translator.hpp"
#include    "MakeDocs_Parser.hpp"
#include    "MakeDocs_ClassInfo.hpp"
#include    "MakeDocs_ThisFacility.hpp"

#include    "MakeDocs_HtmlTranslator.hpp"
#include    "MakeDocs_ParserError.hpp"


// -----------------------------------------------------------------------------
//  Export the facility object internally
// -----------------------------------------------------------------------------
extern TFacMakeDocs facMakeDocs;
